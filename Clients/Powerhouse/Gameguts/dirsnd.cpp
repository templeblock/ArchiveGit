#include <windows.h>
#include "proto.h"
#include "dirsnd.h"

// Statics
//************************************************************************
LPDIRECTSOUND	CDirSnd::m_pDirectSound = NULL;
CDirSnd *		CDirSnd::m_pSndList[MAX_SOUNDS];
int				CDirSnd::m_nSndCnt = 0;
//************************************************************************

// The CONSTRUCTOR
//************************************************************************
CDirSnd::CDirSnd()
//************************************************************************
{
	m_pBuffer = NULL;
	m_p3dBuff =	NULL;
	m_fXRange = (float)100;
	m_fYRange = (float)100;
	m_b3d = FALSE;
	m_idSound = 0;

	// add to the list
	for (int i=0; i<MAX_SOUNDS; i++)
	{
		if (!m_pSndList[i])
		{ // found an empty slot...
			m_pSndList[i] = this;
			m_nSndCnt++;
			break;
		}
	}
}

// The DESTRUCTOR
//************************************************************************
CDirSnd::~CDirSnd()
//************************************************************************
{
	// make sure everything is freed up.
	Close();

	// remove from the list
	for (int i=0; i<MAX_SOUNDS; i++)
	{
		if (m_pSndList[i] == this)
		{
			m_pSndList[i] = NULL;
			m_nSndCnt--;
			break;
		}
	}
}

//************************************************************************
BOOL CDirSnd::Init(HWND hWnd)
//************************************************************************
{
	// Already created
	if (m_pDirectSound)
		return(FALSE);

	if (DS_OK != DirectSoundCreate(NULL, &m_pDirectSound, NULL))
		return(FALSE);

	// Create succeeded
	m_pDirectSound->SetCooperativeLevel(hWnd, DSSCL_NORMAL);
	return(TRUE);
}

//************************************************************************
void CDirSnd::CloseAll(void)
//************************************************************************
{
	for (int i=0; i<MAX_SOUNDS; i++)
	{
		if (m_pSndList[i])
			m_pSndList[i]->Close();
	}

	if (m_pDirectSound)
	{
		m_pDirectSound->Release();
		m_pDirectSound = NULL;
	}
}

//************************************************************************
BOOL CDirSnd::Open(ITEMID idSound, HINSTANCE hInstance, BOOL b3D)
//************************************************************************
{
	// Already open
	if (m_pBuffer)
		return YES;

	m_idSound = idSound;
	m_b3d = b3D;

	// Load resource
	if (!LoadWaveResource((LPSTR)MAKEINTRESOURCE(m_idSound), hInstance, m_b3d))
		return NO;

	// Create sound buffer
	return YES;
}

//************************************************************************
BOOL CDirSnd::ReOpen(void)
//************************************************************************
{
	// Already open
	if (m_pBuffer)
		return YES;

	HINSTANCE hInstance = GetApp()->GetInstance();

	// Load resource
	if (!LoadWaveResource((LPSTR)MAKEINTRESOURCE(m_idSound), hInstance, m_b3d))
		return NO;

	// Create sound buffer
	return YES;
}

//************************************************************************
void CDirSnd::ReloadAll(void)
//************************************************************************
{
	for (int i=0; i<MAX_SOUNDS; i++)
	{
		if (m_pSndList[i])
			m_pSndList[i]->ReOpen();
	}
}

//************************************************************************
void CDirSnd::Close( void )
//************************************************************************
{
	Stop();

	// release sound buffer
	if (m_p3dBuff)
	{
		m_p3dBuff->Release();
		m_p3dBuff = NULL;
	}

	if (m_pBuffer)
	{
		m_pBuffer->Release();
		m_pBuffer = NULL;
	}
}


//************************************************************************
BOOL CDirSnd::LoadWaveResource(LPSTR lpSound, HINSTANCE hInstance, BOOL b3D)
// lpSound is a resource name
//************************************************************************
{
	HGLOBAL hWave;
	LPTR lpWave;
    PCMWAVEFORMAT pcm; 
	HMMIO hio;
	MMCKINFO RiffChunk;
	MMCKINFO FmtChunk;
	MMCKINFO DataChunk;
	HRESULT hr;
	MMRESULT Res;
	MMIOINFO Info;
	long lRead;

	WAVEFORMATEX Format;
	DSBUFFERDESC BuffDesc;

	BYTE *pData1;
	BYTE *pData2;
	DWORD dwBytes1;
	DWORD dwBytes2;

	Init( GetApp()->GetMainWnd() );

	if ( !m_pDirectSound )
		return NO;

	if ( !hInstance )
		return NO;

	// Find the resource
	if ( !(hWave = (HGLOBAL)FindResource( hInstance, lpSound, "WAVE" )) )
		return NO;

	// Load the resource
	if ( !(hWave = LoadResource( hInstance, (HRSRC)hWave )) )
		return( NO );

	// Lock it
	if ( !(lpWave = (LPTR)LockResource( hWave )) )
	{
		FreeResource( hWave );
		return NO;
	}

	// Set up mmio Info structure for opening memory for mmio
	memset(&Info, 0, sizeof(MMIOINFO));
	Info.pchBuffer = (LPSTR)lpWave;
	Info.fccIOProc = FOURCC_MEM;
	Info.cchBuffer = SizeofResource(hInstance, (HRSRC)hWave);  
	Info.adwInfo[0] = 0;

	// Open memory for mmio
	hio = mmioOpen(NULL, &Info, MMIO_READ); 
	if (!hio)
		return 0;

	// Descened into the WAVE section
	RiffChunk.ckid = FOURCC_RIFF;
	RiffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E' );
	Res = mmioDescend(hio, &RiffChunk, NULL, MMIO_FINDRIFF);
	if (Res != 0)
		goto ERROR_READING_WAVE;

    // Descend into the fmt chunk
    FmtChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
    Res = mmioDescend(hio, &FmtChunk, &RiffChunk, MMIO_FINDCHUNK);
	if (Res != 0)
		goto ERROR_READING_WAVE;
                                                                                                                                                               
	// Read the 'fmt ' chunk into <pcmWaveFormat>
    if (mmioRead(hio, (HPSTR) &pcm, (long)sizeof(pcm)) 
		!= (long) sizeof(pcm))
	    goto ERROR_READING_WAVE;
                                                                                                           

	DataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	Res = mmioDescend(hio, &DataChunk, &RiffChunk, MMIO_FINDCHUNK);
	if (Res != 0)
		goto ERROR_READING_WAVE;

	// Set the wave format 
	memset(&Format, 0, sizeof(WAVEFORMATEX));
	Format.wFormatTag = pcm.wf.wFormatTag;
	Format.nChannels = pcm.wf.nChannels;
	Format.nSamplesPerSec = pcm.wf.nSamplesPerSec;
	Format.nAvgBytesPerSec = pcm.wf.nAvgBytesPerSec;
	Format.nBlockAlign = pcm.wf.nBlockAlign;
	Format.wBitsPerSample = pcm.wBitsPerSample;

	// Set up the sound buffer description
	memset(&BuffDesc, 0, sizeof(BuffDesc));
	BuffDesc.dwSize = sizeof(DSBUFFERDESC);
	if (b3D)
		BuffDesc.dwFlags = DSBCAPS_CTRLVOLUME|DSBCAPS_CTRL3D;
	else
		BuffDesc.dwFlags = DSBCAPS_CTRLDEFAULT;
	BuffDesc.dwBufferBytes = DataChunk.cksize;
	BuffDesc.lpwfxFormat = &Format;


	// Create the buffer
    hr =  m_pDirectSound->CreateSoundBuffer( &BuffDesc, &m_pBuffer, NULL);

	if (hr != DS_OK)
	{
		goto ERROR_READING_WAVE;
	}

	if (b3D)
	{
		hr = m_pBuffer->QueryInterface(IID_IDirectSound3DBuffer, (void **)&m_p3dBuff);
		//hr = m_pBuffer->QueryInterface(IID_IMAPISession, (void **)&m_p3dBuff);
		if (SUCCEEDED(hr)) 
		{
			// Set 3D parameters of this sound.
			hr = m_p3dBuff->SetPosition(D3DVAL(0), D3DVAL(0), D3DVAL(0), DS3D_IMMEDIATE);
		}
	}

	// Lock the buffer so we can read into it
	hr = m_pBuffer->Lock(0, DataChunk.cksize, &pData1, &dwBytes1, 
		&pData2, &dwBytes2, 0);
    
    // If we got DSERR_BUFFERLOST, restore and retry lock.
    if (DSERR_BUFFERLOST == hr)
	{
        m_pBuffer->Restore();
		hr = m_pBuffer->Lock(0, DataChunk.cksize, &pData1, &dwBytes1, 
			&pData2, &dwBytes2, 0);
    }

	// Read the data
	lRead = mmioRead(hio, (HPSTR)pData1, dwBytes1);
	if (pData2 != NULL)
		lRead = mmioRead(hio, (HPSTR)pData2, dwBytes2);

    // Release the data back to DirectSound.
    hr = m_pBuffer->Unlock(pData1, dwBytes1, pData2, dwBytes2);

	ERROR_READING_WAVE:

	mmioClose(hio, 0);	
	
	return YES;
}

//************************************************************************
BOOL CDirSnd::Play(BOOL bLooping, BOOL bInit3dPos, BOOL bReset)
//************************************************************************
{
	if (!m_pBuffer)
		return FALSE;

	HRESULT hr;
	if (bReset)
		 m_pBuffer->SetCurrentPosition(0);

	if (m_p3dBuff && bInit3dPos)
		hr = m_p3dBuff->SetPosition(D3DVAL(0), D3DVAL(0), D3DVAL(0), DS3D_IMMEDIATE);

	hr = m_pBuffer->Play(0, 0, bLooping ? DSBPLAY_LOOPING : 0);
	
	return (hr == DS_OK);
}

//************************************************************************
void CDirSnd::StopAndFree( void )
//************************************************************************
{
}


//************************************************************************
BOOL CDirSnd::Stop( void )
//************************************************************************
{
	if (!m_pBuffer)
		return FALSE;

	HRESULT hr = m_pBuffer->Stop();

	return (hr == DS_OK);
}

//************************************************************************
BOOL CDirSnd::SetVolume(LONG lVolume)
//************************************************************************
{
	if (!m_pBuffer)
		return FALSE;

	HRESULT hr = m_pBuffer->SetVolume(lVolume);

	return (hr == DS_OK);
}

//************************************************************************
BOOL CDirSnd::Stopped(void)
//************************************************************************
{
	if (!m_pBuffer)
		return TRUE;

	DWORD dwStatus;
	BOOL bRet = TRUE;

	HRESULT hr = m_pBuffer->GetStatus(&dwStatus);
	if (hr == DS_OK)
	{
		if (dwStatus & DSBSTATUS_PLAYING)
			bRet = FALSE;
	}

	return bRet;
}

//************************************************************************
BOOL CDirSnd::SetPosition(int x, int y, int z)
//************************************************************************
{
	if (!m_p3dBuff)
		return FALSE;

	// Convert coordinates to 3d values
	float fx = (float)x / m_fXRange;
	float fy = (float)y / m_fYRange;

	//float f
	HRESULT hr = m_p3dBuff->SetPosition(D3DVAL(fx), D3DVAL(fy), D3DVAL(0), DS3D_IMMEDIATE);

	return (hr == DS_OK);
}

//************************************************************************
CDirSnd * CDirSnd::Duplicate(void)
//************************************************************************
{
	CDirSnd *pSnd = new CDirSnd();

	if (!m_pDirectSound || !m_pBuffer)
		return NULL;

	HRESULT hr = m_pDirectSound->DuplicateSoundBuffer(m_pBuffer, &pSnd->m_pBuffer);

	if (hr != DS_OK)
	{
		delete pSnd;
		return NULL;
	}

	if (m_p3dBuff)
	{
		hr = m_pBuffer->QueryInterface(IID_IDirectSound3DBuffer, (void **)&pSnd->m_p3dBuff);
		if (hr != DS_OK)
		{
			delete pSnd;
			return NULL;
		}
	}

	pSnd->m_fXRange = m_fXRange;
	pSnd->m_fYRange = m_fYRange;

	return pSnd;
}
