#include <windows.h>
#include "proto.h"
#include "sound.h"
#include "wavemix.h"
#include "mmsystem.h"

// The CONSTRUCTOR
//************************************************************************
CSound::CSound(BOOL bUseWaveMix)
//************************************************************************
{
	int i;

	m_bUseWaveMix = bUseWaveMix;
	m_hLib = NULL;
	m_hWaveSaved = NULL;
	m_bOff = FALSE;
	m_bLoopSaved = FALSE;
	m_bDefered = FALSE;
	m_ghMixSession = NULL;
	for (i = 0; i < MAX_MIX_CHANNELS; ++i)
		m_mixWave[i] = NULL;
	m_iCurMixWave = 0;

	lpSaveWave = NULL;
}

// The DESTRUCTOR
//************************************************************************
CSound::~CSound()
//************************************************************************
{
	// make sure everything is freed up.
	Close();
}

//************************************************************************
BOOL CSound::IsOn( void )
//************************************************************************
{
	return( !m_bOff );
}

//************************************************************************
void CSound::Toggle( void )
//************************************************************************
{
	m_bOff = !m_bOff;
}

//************************************************************************
void CSound::SetDefered(BOOL bDefer)
//************************************************************************
{
	m_bDefered = bDefer;
}

//************************************************************************
BOOL CSound::GetDefered()
//************************************************************************
{
	return( m_bDefered );
}

//************************************************************************
BOOL CSound::Open( LPSTR lpSoundLibName, BOOL fActivate )
//************************************************************************
{
	WAVEMIXINFO Info; 
	MIXCONFIG config;

	if ( m_hLib || m_ghMixSession )
		Close();

	// Try to initialize the wavemix.dll
	m_iCurMixWave = 0;
	Info.wSize = sizeof (WAVEMIXINFO);
	if ( !m_bUseWaveMix || WaveMixGetInfo(&Info) )
		m_ghMixSession = NULL;
	else
	{
		config.wSize = sizeof(MIXCONFIG);
		config.dwFlags = WMIX_CONFIG_CHANNELS;
		config.wChannels = 1;  // Mono, stereo doesn't work
		if ( m_ghMixSession = WaveMixConfigureInit( &config ) )
		{
			if ( WaveMixOpenChannel( m_ghMixSession, MAX_MIX_CHANNELS, WMIX_OPENCOUNT ) )
			{
				WaveMixCloseSession( m_ghMixSession );
				m_ghMixSession = NULL;
			}
		}
	}

	// Load the sound resource library (if specified) to get the instance handle
	if ( lpSoundLibName )
	{
		if ( !(m_hLib = PHLoadLibrary( lpSoundLibName )) )
			return( NO );
	}

	if (fActivate)
		Activate(TRUE);
	return( YES );
}

//************************************************************************
void CSound::Close( void )
//************************************************************************
{
	int i;

	StopAndFree();

	if ( m_hLib )
	{
		FreeLibrary( m_hLib );
		m_hLib = NULL;
	}

	// if wavemix.dll was initialized
	if ( m_ghMixSession )
	{
		for (i = 0 ; i < MAX_MIX_CHANNELS ; i++)
		{
			if ( m_mixWave[i] )
			{
				WaveMixFlushChannel (m_ghMixSession, i, WMIX_NOREMIX);
				WaveMixFreeWave (m_ghMixSession, m_mixWave[i]);
				m_mixWave[i] = NULL;
			}
		}
		WaveMixActivate (m_ghMixSession, FALSE);
		WaveMixCloseChannel (m_ghMixSession, 0, WMIX_ALL);
		WaveMixCloseSession (m_ghMixSession);
		m_ghMixSession = NULL;
	}
}

//************************************************************************
BOOL CSound::StartResourceID( ITEMID idSound, BOOL bLoop, int iChannel, HINSTANCE hInstance, BOOL bWait, HWND hWndNotify )
// idSound is an id for a WAV resource or a STRING resource referring to file
//************************************************************************
{
	if ( !hInstance )
		hInstance = m_hLib;

	if ( StartResource( (LPSTR)MAKEINTRESOURCE(idSound), bLoop, iChannel, hInstance, bWait, hWndNotify ) )
		return( YES );

	return( NO );
}


//************************************************************************
BOOL CSound::StartFile( LPSTR lpSound, BOOL bLoop, int iChannel, BOOL bWait, HWND hWndNotify )
// lpSound is a .WAV file name
//************************************************************************
{
	FNAME szSound;
	FNAME szExpFileName;

	if ( m_bOff )
		return( NO );

	if ( !lpSound )
		return( NO );

	// force the loop flag on if the name dictates
	if ( HIWORD(lpSound) && *lpSound == '-' )
	{
		bLoop = YES;
		lpSound++;
	}

	//lstrcpy( szSound, Control.PouchPath );
	lstrcpy( szSound, lpSound );
	StripExt( szSound );
	lstrcat( szSound, ".wav" );

	if ( !GetApp()->FindContent( szSound, szExpFileName ) )
		return FALSE;

	m_bLoopSaved = bLoop;

	UINT sndFlag = ( bWait ? SND_SYNC : SND_ASYNC );

	if ( m_ghMixSession )
	{        
		iChannel = mixPlay( szExpFileName, sndFlag | SND_NODEFAULT | (bLoop? SND_LOOP : 0), 
			NULL, iChannel, hWndNotify );
	}
	else
	{
		if (m_hWaveSaved)
			StopAndFree(); // Stop any previous sounds and free the resources
		#ifdef WIN32
			PlaySound( szExpFileName, NULL, sndFlag | SND_NODEFAULT | SND_NOWAIT | SND_FILENAME
				| (bLoop? SND_LOOP : 0) );
		#else
			sndPlaySound( szExpFileName, sndFlag | SND_NODEFAULT
				| (bLoop? SND_LOOP : 0) );
		#endif
	}

	return( YES );
}


//************************************************************************
BOOL CSound::StartResource( LPSTR lpSound, BOOL bLoop, int iChannel, HINSTANCE hInstance, BOOL bWait, HWND hWndNotify )
// lpSound is a resource name
//************************************************************************
{
	HGLOBAL hWave;
	LPTR lpWave;

	if ( m_bOff )
		return( NO );

	if ( !lpSound )
		return( NO );

	// force the loop flag on if the name dictates
	if ( HIWORD(lpSound) && *lpSound == '-' )
	{
		bLoop = YES;
		lpSound++;
	}

	UINT sndFlag = ( bWait ? SND_SYNC : SND_ASYNC );

	if ( m_ghMixSession )
	{
		iChannel = mixPlay( lpSound, sndFlag | SND_NODEFAULT | SND_MEMORY
			| (bLoop? SND_LOOP : 0), hInstance, iChannel, hWndNotify );
	}
	else
	{
		// Load and lock the sound resource
		if ( !hInstance )
			hInstance = m_hLib;
		if ( !hInstance )
			return( NO );
		if ( !(hWave = (HGLOBAL)FindResource( hInstance, lpSound, "WAVE" )) )
			return( NO );
		if ( !(hWave = LoadResource( hInstance, (HRSRC)hWave )) )
			return( NO );
		if ( !(lpWave = (LPTR)LockResource( hWave )) )
		{
			FreeResource( hWave );
			return( NO );
		}

		if (m_hWaveSaved)
			StopAndFree(); // Stop any previous sounds and free the resources
		m_bLoopSaved = bLoop;
		m_hWaveSaved = hWave;
		#ifdef WIN32
			PlaySound( (LPSTR)lpSound, hInstance, sndFlag | SND_RESOURCE | SND_NODEFAULT | SND_NOWAIT | SND_MEMORY
				| (bLoop? SND_LOOP : 0) );
		#else
			sndPlaySound( (LPSTR)lpWave, sndFlag | SND_NODEFAULT | SND_MEMORY
				| (bLoop? SND_LOOP : 0) );
		#endif
	}

	return( YES );
}

//************************************************************************
int CSound::mixPlay( LPSTR lpSound, UINT uFlags, HINSTANCE hInstance, int iChannel, HWND hWndNotify )
//************************************************************************
{
	MIXPLAYPARAMS MixPlayParams;
	int iMixChannel;
	BOOL bUseChannel = iChannel != -1;

	if ( !m_ghMixSession )
		return( -1 );

	if (bUseChannel)
	{ // a specific channel
		iMixChannel = iChannel;
	}
	else
	{ // iChannel == -1 means use the next available channel
		iMixChannel = m_iCurMixWave;
	}

	if ( m_mixWave[iMixChannel] )
	{
		WaveMixFlushChannel (m_ghMixSession, iMixChannel, WMIX_NOREMIX);
		// don't free loaded wave for mixing, you must call Stop() or
		// StopAndFree() or mixStopAll()
		if (!bUseChannel)
		{
			WaveMixFreeWave (m_ghMixSession, m_mixWave[iMixChannel]);
			m_mixWave[iMixChannel] = NULL;
		}
	}

	// if we don't still have a channel open, then open one
	if (!m_mixWave[iMixChannel])
		m_mixWave[iMixChannel] = WaveMixOpenWave (m_ghMixSession, lpSound,
												hInstance,
												((hInstance == NULL)  ? WMIX_FILE : WMIX_RESOURCE) );

	// if we successfully opened a channel, then play that channel
	if ( m_mixWave[iMixChannel] )
	{
		MixPlayParams.wSize = sizeof(MIXPLAYPARAMS);
		MixPlayParams.hMixSession = m_ghMixSession;
		MixPlayParams.hWndNotify = hWndNotify;
		MixPlayParams.dwFlags = WMIX_CLEARQUEUE | WMIX_HIPRIORITY;
		MixPlayParams.wLoops = (uFlags & SND_LOOP) ? 0xffff : 0;
		MixPlayParams.iChannel = iMixChannel;
		MixPlayParams.lpMixWave = m_mixWave[iMixChannel];
        
		if (m_bDefered)
			MixPlayParams.dwFlags |= WMIX_WAIT;			                       

		WaveMixPlay (&MixPlayParams);
	}
	else
		iMixChannel = -1;

	if (!bUseChannel)
	{
		m_iCurMixWave++;
		if (m_iCurMixWave >= MAX_MIX_CHANNELS)
			m_iCurMixWave = 0;
	}

	return( iMixChannel );
}

//************************************************************************
void CSound::StopAndFree( void )
//************************************************************************
{
	if ( m_ghMixSession )
		mixStopAll();

	if ( m_bLoopSaved || m_hWaveSaved )
	{
		#ifdef WIN32
			PlaySound( NULL, NULL, SND_SYNC );
		#else
			sndPlaySound( NULL, SND_SYNC );
		#endif
	}

	if ( m_hWaveSaved )
	{
		UnlockResource( m_hWaveSaved );
		FreeResource( m_hWaveSaved );
		m_hWaveSaved = NULL;
	}
}


//************************************************************************
void CSound::Stop( void )
//************************************************************************
{
	if ( m_ghMixSession )
		mixStopAll();

	if ( m_bLoopSaved )
	{
		#ifdef WIN32
			PlaySound( NULL, NULL, SND_SYNC );
		#else
			sndPlaySound( NULL, SND_SYNC );
		#endif
	}
}


//************************************************************************
void CSound::mixStopAll( void )
//************************************************************************
{
	int i;

	if ( !m_ghMixSession )
		return;

	for (i = 0 ; i < MAX_MIX_CHANNELS ; i++)
	{
		if ( m_mixWave[i] )
		{
			WaveMixFlushChannel (m_ghMixSession, i, WMIX_NOREMIX);
			WaveMixFreeWave (m_ghMixSession, m_mixWave[i]);
			m_mixWave[i] = NULL;
		}
	}

	m_iCurMixWave = 0;
}

//************************************************************************
void CSound::StopChannel( int iChannel )
//************************************************************************
{
	if ( m_ghMixSession )
	{
		if ( m_mixWave[iChannel] )
		{
			WaveMixFlushChannel (m_ghMixSession, iChannel, WMIX_NOREMIX);
			WaveMixFreeWave (m_ghMixSession, m_mixWave[iChannel]);
			m_mixWave[iChannel] = NULL;
		}
	}
	else
		StopAndFree();
}

//************************************************************************
void CSound::Activate(BOOL fActivate)
//************************************************************************
{
	if (m_ghMixSession)
		WaveMixActivate( m_ghMixSession, fActivate );
}

//************************************************************************
void CSound::Pump()
//************************************************************************
{
	if (m_ghMixSession)
		WaveMixPump();
}

//************************************************************************
// TEMPORARY FIX - FOR COLLISION (E3)
BOOL CSound::LoadWaveResource( ITEMID idSound, BOOL bLoop, int iChannel, HINSTANCE hInstance, BOOL bWait, HWND hWndNotify )
// lpSound is a resource name
//************************************************************************
{
	HGLOBAL hWave;

	if ( m_bOff )
		return( NO );

	LPSTR lpSound = (LPSTR)MAKEINTRESOURCE(idSound);

	if ( !lpSound )
		return( NO );

	// force the loop flag on if the name dictates
	if ( HIWORD(lpSound) && *lpSound == '-' )
	{
		bLoop = YES;
		lpSound++;
	}

	UINT sndFlag = ( bWait ? SND_SYNC : SND_ASYNC );

	// Load and lock the sound resource
	if ( !hInstance )
		hInstance = m_hLib;
	if ( !hInstance )
		return( NO );
	if ( !(hWave = (HGLOBAL)FindResource( hInstance, lpSound, "WAVE" )) )
		return( NO );
	if ( !(hWave = LoadResource( hInstance, (HRSRC)hWave )) )
		return( NO );
	if ( !(lpSaveWave = (LPTR)LockResource( hWave )) )
		{
		FreeResource( hWave );
		return( NO );
		}

	if (m_hWaveSaved)
		StopAndFree(); // Stop any previous sounds and free the resources
	m_bLoopSaved = bLoop;
	m_hWaveSaved = hWave;

	return( YES );
}

//************************************************************************
// TEMPORARY FIX - FOR COLLISION (E3)
void CSound::Play()
//************************************************************************
{
	if (!lpSaveWave)
		return;

	#ifdef WIN32
		PlaySound( (LPSTR)lpSaveWave, NULL, SND_ASYNC | SND_NODEFAULT | SND_MEMORY | SND_NOWAIT );
	#else
		sndPlaySound( (LPSTR)lpSaveWave, SND_ASYNC | SND_NODEFAULT | SND_MEMORY );
	#endif
}
