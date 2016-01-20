//®PL1¯®FD1¯®BT0¯®TP0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "mmsystem.h"

#ifdef USEWAVEMIX
#include <wavemix.h>
#endif

static HINSTANCE hSoundLib;
static HGLOBAL hWaveSaved;
static BOOL bOff, bLoopSaved;

#ifdef USEWAVEMIX
// stuff for wavemix.dll
#define MAX_MIX_CHANNELS	8
static	HANDLE ghMixSession;
static	MIXPLAYPARAMS MixPlayParams;
static	LPMIXWAVE mixWave[MAX_MIX_CHANNELS];
static	int	iCurMixWave;
static	BOOL bMixActive;
#endif // USEWAVEMIX

static  BOOL bDeferedSound = FALSE;

BOOL CALLBACK EXPORT SoundStartResourceID (UINT ID, BOOL bLoop, HINSTANCE hInstance);

//************************************************************************
BOOL SoundIsOn( void )
//************************************************************************
{
return( !bOff );
}

//************************************************************************
void SoundToggle( void )
//************************************************************************
{
bOff = !bOff;
}

void SetDeferedSound(BOOL bDefer)
{
	bDeferedSound = bDefer;
}

BOOL GetDeferedSound()
{
	return bDeferedSound;
}

//************************************************************************

//************************************************************************
BOOL SoundOpen( LPSTR lpSoundLibName )
//************************************************************************
{
FNAME szSoundLibName;
#ifdef USEWAVEMIX
WAVEMIXINFO	Info;
MIXCONFIG	config;
#endif // USEWAVEMIX

#ifdef WIN32
 #ifndef _MAC
	return FALSE;
 #endif
#endif

if ( hSoundLib )
	SoundClose();
lstrcpy( szSoundLibName, Control.PouchPath );
lstrcat( szSoundLibName, lpSoundLibName );

// Perform a LoadLibrary to get the instance handle
if ( (hSoundLib = LoadLibrary( szSoundLibName )) < (HINSTANCE)HINSTANCE_ERROR )
	{
	hSoundLib = NULL;
	return( NO );
	}

	// if using the wavemix.dll
#ifdef USEWAVEMIX
	if (Control.bUseWaveMix)
	{
		iCurMixWave = 0;
		Info.wSize = sizeof (WAVEMIXINFO);
		if (WaveMixGetInfo (&Info) )
			return NO;

		config.wSize = sizeof (MIXCONFIG);
		config.dwFlags = WMIX_CONFIG_CHANNELS;
		config.wChannels = 1;  // Mono, stereo doesn't work
		if (! (ghMixSession = WaveMixConfigureInit (&config)))
			return NO;

		if (WaveMixOpenChannel (ghMixSession, MAX_MIX_CHANNELS, WMIX_OPENCOUNT))
			return NO;
	}
	else ghMixSession = NULL;
	bMixActive = NO;
#endif // USEWAVEMIX

return( YES );
}

//************************************************************************
void SoundClose( void )
//************************************************************************
{
	int i;

#ifdef WIN32
	return;
#endif

	SoundStopAndFree();
	if ( hSoundLib )
		FreeLibrary( hSoundLib );
	hSoundLib = NULL;

	// if wavemix.dll was initialized
#ifdef USEWAVEMIX
	if (! ghMixSession)
		return;
	for (i = 0 ; i < MAX_MIX_CHANNELS ; i++)
	{
		if (mixWave[i] != NULL)
		{
			WaveMixFlushChannel (ghMixSession, i, WMIX_NOREMIX);
			WaveMixFreeWave (ghMixSession, mixWave[i]);
			mixWave[i] = NULL;
		}
	}
	WaveMixActivate (ghMixSession, FALSE);
	WaveMixCloseChannel (ghMixSession, 0, WMIX_ALL);
	WaveMixCloseSession (ghMixSession);
	ghMixSession = NULL;
#endif // USEWAVEMIX
}

//************************************************************************
BOOL CALLBACK EXPORT SoundStartID (ITEMID idSound, BOOL bLoop, HINSTANCE hInstance)
// idSound is an id for a WAV resource or a STRING resouce referring to file
//************************************************************************
{
FNAME szSound;

if ( !hInstance )
	hInstance = hSoundLib;

if ( LoadString( hInstance, idSound, szSound, sizeof(szSound)-1 ) )
	return( SoundStartFile( szSound, bLoop, hInstance ) );
else
#ifdef _MAC
	// Sorry there is no such thing as MAKEINTRESOURCE() on the mac.
	// There is a separate call to load a resource by ID.
	return( SoundStartResourceID(idSound, bLoop, hInstance ));
#else
	return( SoundStartResource( (LPSTR)MAKEINTRESOURCE(idSound), bLoop, hInstance ) );
#endif
}


//************************************************************************
BOOL CALLBACK EXPORT SoundStartFile (LPSTR lpSound, BOOL bLoop, HINSTANCE hInstance)
// lpSound is a .WAV file name
//************************************************************************
{
FNAME szSound;

if ( bOff )
	return( NO );

if ( !lpSound )
	return( NO );

// force the loop flag on if the name dictates
if ( HIWORD(lpSound) && *lpSound == '-' )
	{
	bLoop = YES;
	lpSound++;
	}

if ( !hInstance )
	hInstance = hSoundLib;

lstrcpy( szSound, Control.PouchPath );
lstrcat( szSound, lpSound );
stripext( szSound );
lstrcat( szSound, ".wav" );

SoundStopAndFree(); // Stop any previous sounds and free the resources
bLoopSaved = bLoop;
sndPlaySound( szSound, SND_ASYNC | SND_NODEFAULT
	| (bLoop? SND_LOOP : 0) );
return (YES);
}


//************************************************************************
BOOL CALLBACK EXPORT SoundStartResource (LPSTR lpSound, BOOL bLoop, HINSTANCE hInstance)
// lpSound is a resource name
//************************************************************************
{
	HGLOBAL hWave;
	LPTR lpWave;
	#ifdef _MAC
	DWORD dwSize;
	#endif

	if ( bOff )
		return( NO );

	if ( !lpSound )
		return( NO );

	if (bDeferedSound)
		return FALSE;	

	// force the loop flag on if the name dictates
	if ( HIWORD(lpSound) && *lpSound == '-' )
		{
		bLoop = YES;
		lpSound++;
		}

	if ( !hInstance )
		hInstance = hSoundLib;
	if ( !hInstance )
		return( NO );

	// Load and lock the sound resource
#ifdef _MAC
	if ( !(hWave = LoadMacResource(hInstance, lpSound, "WAVE")) )
		return FALSE;
	lpWave = (LPTR)*((Handle)hWave);
#else
	if ( !(hWave = (HGLOBAL)FindResource( hInstance, lpSound, "WAVE" )) )
		return( NO );
	if ( !(hWave = LoadResource( hInstance, (HRSRC)hWave )) )
		return( NO );
	if ( !(lpWave = (LPTR)LockResource( hWave )) )
		{
		FreeResource( hWave );
		return( NO );
		}
#endif

	if (hWaveSaved != hWave)
		SoundStopAndFree(); // Stop any previous sounds and free the resources
	bLoopSaved = bLoop;
	hWaveSaved = hWave;
	#ifdef _MAC
	dwSize = GetMacResourceSize(hWave);
	PlayWaveSound( (LPSTR)lpWave, dwSize, SND_ASYNC | SND_NODEFAULT | SND_MEMORY
		| (bLoop? SND_LOOP : 0) );
	#else
	sndPlaySound( (LPSTR)lpWave, SND_ASYNC | SND_NODEFAULT | SND_MEMORY
		| (bLoop? SND_LOOP : 0) );
	#endif
	return( YES );
}




#ifdef _MAC		
// The mac needs a separate call to load a resource by ID rather than name.

//************************************************************************
BOOL CALLBACK EXPORT SoundStartResourceID (UINT ID, BOOL bLoop, HINSTANCE hInstance)
//************************************************************************
{
	HGLOBAL hWave;
	LPTR lpWave;
	#ifdef _MAC
	DWORD dwSize;
	#endif

	if ( bOff )
		return( NO );

	if (bDeferedSound)
		return FALSE;	

	// force the loop flag on if the name dictates
//	if ( HIWORD(lpSound) && *lpSound == '-' )
//		{
//		bLoop = YES;
//		lpSound++;
//		}

	if ( !hInstance )
		hInstance = hSoundLib;
	if ( !hInstance )
		return( NO );

	// Load and lock the sound resource
	if ( !(hWave = LoadMacResourceID(hInstance, ID, "WAVE")) )
		return FALSE;
	lpWave = (LPTR)*((Handle)hWave);

	SoundStopAndFree(); // Stop any previous sounds and free the resources
	bLoopSaved = bLoop;
	hWaveSaved = hWave;

	dwSize = GetMacResourceSize(hWave);
	PlayWaveSound( (LPSTR)lpWave, dwSize, SND_ASYNC | SND_NODEFAULT | SND_MEMORY
		| (bLoop? SND_LOOP : 0) );

	return( YES );
}
#endif // _MAC



//************************************************************************
void CALLBACK EXPORT SoundStopAndFree( void )
//************************************************************************
{
	if ( bLoopSaved || hWaveSaved )
		sndPlaySound( NULL, 0 );
	if ( hWaveSaved )
	{
#ifdef _MAC
		ReleaseMacResource(hWaveSaved);
#else	
		UnlockResource( hWaveSaved );
		FreeResource( hWaveSaved );
#endif		
		hWaveSaved = NULL;
	}
}


//************************************************************************
void CALLBACK EXPORT SoundStop( void )
//************************************************************************
{
if ( bLoopSaved )
	sndPlaySound( NULL, 0 );
}


//************************************************************************
int CALLBACK EXPORT mixSoundStartFile (LPSTR lpSound, BOOL bLoop, HINSTANCE hInstance,
  int iChannel)
// lpSound is a .WAV file name
//************************************************************************
{
	FNAME	szSound;
	int		iRet;

	if (bOff || ! lpSound)
		return (-1);

	// force the loop flag on if the name dictates
	if (HIWORD(lpSound) && *lpSound == '-')
	{
		bLoop = YES;
		lpSound++;
	}

	if (! hInstance)
		hInstance = hSoundLib;

	lstrcpy (szSound, Control.PouchPath);
	lstrcat (szSound, lpSound);
	stripext (szSound);
	lstrcat (szSound, ".wav");

	SoundStopAndFree(); // Stop any previous sounds and free the resources

	if ((iRet = mixPlaySound (szSound, SND_ASYNC | SND_NODEFAULT
  	  | (bLoop? SND_LOOP : 0), TRUE, iChannel)) < 0)
		return (-1);

	return (iRet);
}


//************************************************************************
int CALLBACK EXPORT mixSoundStartResource (LPSTR lpSound, BOOL bLoop,
  HINSTANCE hInstance, int iChannel)
// lpSound is a resource name
//************************************************************************
{
	int		iRet;

	if (bOff || ! lpSound)
		return (-1);

	// force the loop flag on if the name dictates
	if (HIWORD(lpSound) && *lpSound == '-')
	{
		bLoop = YES;
		lpSound++;
	}

	SoundStopAndFree(); // Stop any previous sounds and free the resources

	if ((iRet = mixPlaySound (lpSound, SND_ASYNC | SND_NODEFAULT | SND_MEMORY
	  | (bLoop ? SND_LOOP : 0), FALSE, iChannel)) < 0)
		return (-1);

	return (iRet);
}


//************************************************************************
int mixPlaySound (LPSTR szSound, UINT uFlags, BOOL bFile, int iChannel)
//************************************************************************
{
#ifdef USEWAVEMIX
	int iRet = -1;

	if (! bMixActive)
	{
		WaveMixActivate (ghMixSession, TRUE);
		bMixActive = YES;
	}

	if (iChannel != -1)
	{
		if ((mixWave[iChannel] != NULL) && (ghMixSession != NULL))
		{
			WaveMixFlushChannel (ghMixSession, iChannel, WMIX_NOREMIX);

			MixPlayParams.wSize = sizeof(MIXPLAYPARAMS);
			MixPlayParams.hMixSession = ghMixSession;
			MixPlayParams.hWndNotify = NULL;
			MixPlayParams.dwFlags = WMIX_CLEARQUEUE | WMIX_HIPRIORITY;
			MixPlayParams.wLoops = (uFlags & SND_LOOP) ? 0xffff : 0;
			MixPlayParams.iChannel = iChannel;
			MixPlayParams.lpMixWave = mixWave[iChannel];

			WaveMixPlay (&MixPlayParams);
		}
		return (iChannel);
	}
	else
	{
		if (ghMixSession != NULL)
		{
			if (mixWave[iCurMixWave] != NULL)
			{
				WaveMixFlushChannel (ghMixSession, iCurMixWave, WMIX_NOREMIX);
				WaveMixFreeWave (ghMixSession, mixWave[iCurMixWave]);
				mixWave[iCurMixWave] = NULL;
			}

			mixWave[iCurMixWave] = WaveMixOpenWave (ghMixSession, szSound,
			  (bFile ? NULL : hSoundLib),
	  		  (bFile ? WMIX_FILE : WMIX_RESOURCE) );

			if (mixWave[iCurMixWave] != NULL)
			{
				MixPlayParams.wSize = sizeof(MIXPLAYPARAMS);
				MixPlayParams.hMixSession = ghMixSession;
				MixPlayParams.hWndNotify = NULL;
				MixPlayParams.dwFlags = WMIX_CLEARQUEUE | WMIX_HIPRIORITY;
				MixPlayParams.wLoops = (uFlags & SND_LOOP) ? 0xffff : 0;
				MixPlayParams.iChannel = iCurMixWave;
				MixPlayParams.lpMixWave = mixWave[iCurMixWave];

				if (bDeferedSound)
					MixPlayParams.dwFlags |= WMIX_WAIT;

				WaveMixPlay (&MixPlayParams);

				iRet = iCurMixWave;
			}
		}
		else
			return (-1);

		iCurMixWave++;
		if (iCurMixWave >= MAX_MIX_CHANNELS)
			iCurMixWave = 0;
	}
	return (iRet);
#else
	return 0;
#endif // USEWAVEMIX
}


//************************************************************************
void CALLBACK EXPORT mixSoundStopAll (void)
//************************************************************************
{
#ifdef USEWAVEMIX
	int i;

	if (bOff || (ghMixSession == NULL))
		return;

	for (i = 0 ; i < MAX_MIX_CHANNELS ; i++)
	{
		if (mixWave[i] != NULL)
		{
			WaveMixFlushChannel (ghMixSession, i, WMIX_NOREMIX);
			WaveMixFreeWave (ghMixSession, mixWave[i]);
			mixWave[i] = NULL;
		}
	}
	iCurMixWave = 0;

	if (bMixActive)
	{
		WaveMixActivate (ghMixSession, FALSE);
		bMixActive = NO;
	}
#endif // USEWAVEMIX
}


