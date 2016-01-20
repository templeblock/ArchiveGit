/** waveio.c
 *
 *
     (C) Copyright Microsoft Corp. 1991, 1992.  All rights reserved.

     You have a royalty-free right to use, modify, reproduce and 
     distribute the Sample Files (and/or any modified version) in 
     any way you find useful, provided that you agree that 
     Microsoft has no warranty obligations or liability for any 
     Sample Application Files which are modified. 
	 
     If you did not get this from Microsoft Sources, then it may not be the
     most current version.  This sample code in particular will be updated
     and include more documentation.  

     Sources are:
     	The MM Sys File Transfer BBS: The phone number is 206 936-4082.
	CompuServe: WINSDK forum, MDK section.
	Anonymous FTP from ftp.uu.net vendor\microsoft\multimedia
 *
 **  */


#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <memory.h>
#include <mmreg.h>
#include "waveconv.h"
#include "riffsup.h"
#include "wavesup.h"
#include "waveio.h"

extern LPWAVEIOCB      glpwio;
LPINFOCHUNK glpInfo;

BOOL NEAR PASCAL FreeWaveHeaders(const LPWAVEIOCB lpwio);

#if (_MSC_VER < 700)
//
//  this silly stuff is to get around a bug in the C6 compiler's optimizer
//
#ifdef GlobalFreePtr
#undef GlobalFreePtr
#define GlobalFreePtr(x)    GlobalFree((HGLOBAL)SELECTOROF(x))
#endif

#endif

/** LRESULT WIOAPI wioFileOpen(LPHWIO lphwio, LPCSTR lpszFilePath, DWORD dwFlags)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (LPHWIO lphwio, LPSTR lpszFilePath, DWORD dwFlags)
 *
 *  RETURN (LRESULT WIOAPI):
 *
 *
 *  NOTES:
 *
 **  */

LRESULT WIOAPI wioFileOpen(LPWAVEIOCB FAR * lplpwio, LPCSTR lpszFilePath, DWORD dwFlags)
{
    HMMIO       hmmio;
    WAVEIOCB    wio;
    LPWAVEIOCB  lpwio;
    MMCKINFO    ckRIFF;
    MMCKINFO    ck;
    DWORD       dw;
    LRESULT     lr;
    LRESULT	lrt;

    //
    //  validate a couple of things...
    //
    if (!lplpwio)
        return (WIOERR_BADPARAM);

    *lplpwio = NULL;

    //
    //  default our error return (assume the worst)
    //
    lr    = WIOERR_FILEERROR;
    lpwio = NULL;

    //
    //  first try to open the file, etc.. open the given file for reading
    //  using buffered I/O
    //
    hmmio = mmioOpen((LPSTR)lpszFilePath, NULL, MMIO_READ | MMIO_ALLOCBUF);
    if (!hmmio)
        goto wio_Open_Error;

    //
    //  locate a 'WAVE' form type...
    //
    ckRIFF.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(hmmio, (LPMMCKINFO)&ckRIFF, NULL, MMIO_FINDRIFF))
        goto wio_Open_Error;

    //
    //  we found a WAVE chunk--now go through and get all subchunks that
    //  we know how to deal with...
    //
    //
    lr = WIOERR_FILEERROR;
    
    _fmemset(&wio, 0, sizeof(wio));

    wio.dwDataSamples  = (DWORD)-1L;

    if(lrt=riffInitINFO(&wio.pInfo))
    {
	lr=lrt;
	goto wio_Open_Error;
    }
    
    while (mmioDescend(hmmio, &ck, &ckRIFF, 0) == 0)
    {
        //
        //  quickly check for corrupt RIFF file--don't ascend past end!
        //
        if ((ck.dwDataOffset + ck.cksize) > (ckRIFF.dwDataOffset + ckRIFF.cksize))
            goto wio_Open_Error;

        switch (ck.ckid)
        {
            case mmioFOURCC('L', 'I', 'S', 'T'):
		if(ck.fccType==mmioFOURCC('I', 'N', 'F', 'O'))
		    if(lrt=riffReadINFO(hmmio, &ck, wio.pInfo))
		    {
			lr=lrt;
			goto wio_Open_Error;
		    }
		break;
		
            case mmioFOURCC('D', 'I', 'S', 'P'):
		riffReadDISP(hmmio, &ck, &(wio.pDisp));
		break;
		
            case mmioFOURCC('f', 'm', 't', ' '):
                //
                //  !?! another format chunk !?!
                //
                if (lpwio)
                    break;

                //
                //  get size of the format chunk, allocate and lock memory
                //  for it. we always alloc a complete extended format header
                //  (even for PCM headers that do not have the cbSize field
                //  defined--we just set it to zero).
                //
                dw = ck.cksize;
                if (dw < sizeof(WAVEFORMATEX))
                    dw = sizeof(WAVEFORMATEX);

                dw += sizeof(WAVEIOCB) - sizeof(WAVEFORMATEX);

                lpwio = (LPWAVEIOCB)GlobalAllocPtr(GHND, dw);
                if (!lpwio)
                {
                    lr = WIOERR_NOMEM;
                    goto wio_Open_Error;
                }

                lpwio->dwSize = dw;

                //
                //  read the format chunk
                //
                lr = WIOERR_FILEERROR;
                dw = ck.cksize;
                if (mmioRead(hmmio, (HPSTR)&lpwio->wfx, dw) != (LONG)dw)
                    goto wio_Open_Error;
                break;


            case mmioFOURCC('d', 'a', 't', 'a'):
                //
                //  !?! multiple data chunks !?!
                //
                if (wio.dwDataBytes)
                    break;

                //
                //  just hang on to the total length in bytes of this data
                //  chunk..
                //
                wio.dwDataBytes = ck.cksize;
		wio.dwDataOffset= ck.dwDataOffset;  // offset of data portion
						    // of data chunk
                break;


            case mmioFOURCC('f', 'a', 'c', 't'):
                //
                //  !?! multiple fact chunks !?!
                //
                if (wio.dwDataSamples != -1L)
                    break;

                //
                //  read the first dword in the fact chunk--it's the only
                //  info we need (and is currently the only info defined for
                //  the fact chunk...)
                //
                //  if this fails, dwDataSamples will remain -1 so we will
                //  deal with it later...
                //
                mmioRead(hmmio, (HPSTR)&wio.dwDataSamples, sizeof(DWORD));
                break;
        }

        //
        //  step up to prepare for next chunk..
        //
        mmioAscend(hmmio, &ck, 0);
    }

    //
    //  if no fmt chunk was found, then die!
    //
    if (!lpwio)
    {
        lr = WIOERR_ERROR;
        goto wio_Open_Error;
    }

    //
    //  all wave files other than PCM are _REQUIRED_ to have a fact chunk
    //  telling the number of samples that are contained in the file. it
    //  is optional for PCM (and if not present, we compute it here).
    //
    //  if the file is not PCM and the fact chunk is not found, then fail!
    //
    if (wio.dwDataSamples == -1L)
    {
        if (lpwio->wfx.wFormatTag != WAVE_FORMAT_PCM)
        {
#if 0
            goto wio_Open_Error;
#else
            UINT    u;

            //
            //  !!! HACK HACK HACK !!!
            //
            //  although this should be considered an invalid wave file, we
            //  will bring up a message box describing the error--hopefully
            //  people will start realizing that something is missing???
            //
            u = MessageBox(NULL, "This wave file does not have a 'fact' chunk and requires one! This is completely invalid and MUST be fixed! Attempt to load it anyway?",
                            "wioFileOpen", MB_YESNO | MB_ICONEXCLAMATION | MB_TASKMODAL);
            if (u == IDNO)
	    {
		lr=WIOERR_BADFILE;
                goto wio_Open_Error;
	    }

            //
            //  !!! need to hack stuff in here !!!
            //
            wio.dwDataSamples = 0L;
#endif
        }
        else
        {
            wio.dwDataSamples = wsupBytesToSamples(&lpwio->wfx, wio.dwDataBytes);
        }
    }

    //
    //  cool! no problems.. 
    //
    lpwio->hmmio          = NULL;


    lpwio->dwFlags        = wio.dwFlags;
    lpwio->dwDataBytes    = wio.dwDataBytes;
    lpwio->dwDataSamples  = wio.dwDataSamples;
    lpwio->dwDataOffset   = wio.dwDataOffset;
    lpwio->pDisp	  = wio.pDisp;
    lpwio->pInfo	  = wio.pInfo;

    if (hmmio)
        mmioClose(hmmio, 0);

    *lplpwio = lpwio;

    return (WIOERR_NOERROR);


    //
    //  return error (after minor cleanup)
    //
wio_Open_Error:

    wioFileClose(&lpwio,0);
#if 0    
    if (lpwio)
        GlobalFreePtr(lpwio);

    if (hmmio)
        mmioClose(hmmio, 0);
#endif
    return (lr);
} /* wioFileOpen() */


/** LRESULT WIOAPI wioFileClose(HWIO hwio, DWORD dwFlags)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (HWIO hwio, DWORD dwFlags)
 *
 *  RETURN (LRESULT WIOAPI):
 *
 *
 *  NOTES:
 *
 **  */

LRESULT WIOAPI wioFileClose(LPWAVEIOCB FAR * lplpwio, DWORD dwFlags)
{
    LPWAVEIOCB  lpwio;

    //
    //  validate a couple of things...
    //
    if (!lplpwio)
        return (WIOERR_BADPARAM);

    lpwio=*lplpwio;
    if(!lpwio)
	return (WIOERR_BADPARAM);

    //
    //  get rid of stuff...
    //

    wioStopWave(lpwio);
    
    if (lpwio->hmmio)
    {
        mmioClose(lpwio->hmmio, 0);
	lpwio->hmmio=NULL;
    }
    
    FreeWaveHeaders(lpwio);

    if(lpwio->pInfo)
	riffFreeINFO(&(lpwio->pInfo));
    
    if(lpwio->pDisp)
	riffFreeDISP(&(lpwio->pDisp));

    GlobalFreePtr(lpwio);

    return (WIOERR_NOERROR);
} /* wioFileClose() */



///////////////////////////////////////////////////////////////////////////////


static BOOL fStopping=FALSE;
static BOOL fSyncDriver=FALSE;



/* wioWaveOutDone(lpwio, pWaveHdr)
 *
 * Called when wave block with header <pWaveHdr> is finished playing.
 * This function causes playing to end.
 *
 * Call when window receives MM_WOM_DONE message.
 *
 */
void WIOAPI wioWaveOutDone(const LPWAVEIOCB lpwio, const LPWAVEHDR pWaveHdr)
{
    WORD w;

    // Search for which header this is
    for(w=0;w<MAX_WAVEHDRS;w++)
    {
	if(pWaveHdr==lpwio->apWaveHdr[w])
	    break;
    }

    if(w>=MAX_WAVEHDRS)
	return;	//this was not one of our buffers

    if(lpwio->hwo)
    {
	// we are done with the buffer
	waveOutUnprepareHeader(lpwio->hwo, pWaveHdr, sizeof(WAVEHDR));
	lpwio->apWHUsed[w]=FALSE;
    }
    
    if(lpwio->dwBytesLeft && !fStopping)
    {
	lpwio->apWaveHdr[w]->dwBufferLength = min(lpwio->dwBytesPerBuffer,lpwio->dwBytesLeft);
	lpwio->apWaveHdr[w]->dwFlags        = 0L;
	lpwio->apWaveHdr[w]->dwLoops        = 0L;

	// read the data from the file
	if (mmioRead(lpwio->hmmio, (HPSTR)lpwio->apWaveHdr[w]->lpData,
	    lpwio->apWaveHdr[w]->dwBufferLength) !=
	          (LONG)lpwio->apWaveHdr[w]->dwBufferLength)
	{
	    return;
	}

	// kep track of how much is left to read
	lpwio->dwBytesLeft-=lpwio->apWaveHdr[w]->dwBufferLength;

	// prepare the header
	if (waveOutPrepareHeader(lpwio->hwo, lpwio->apWaveHdr[w], sizeof(WAVEHDR)) != 0)
	    return;

	// put it into the queue to play.  If output is not paused,
	// playback will start immediately if the driver is async
	//
	// if the driver is synchronous, this call will not return
	// until buffer has been played.
	//
	if (waveOutWrite(lpwio->hwo, lpwio->apWaveHdr[w], sizeof(WAVEHDR)) != 0)
	    return;

	// we are using it
	lpwio->apWHUsed[w]=TRUE;

    }
    else
    {
	// if no more data to play and all buffers are done playing,
	// or we have been asked to stop

	for(w=0;w<MAX_WAVEHDRS;w++)
	{
	    // find out if all buffers are free
	    if(lpwio->apWHUsed[w])
		break;
	}

	// all buffers are free, close the device
	if(w>=MAX_WAVEHDRS)
	{
	    if(lpwio->hwo)
		if(!waveOutClose(lpwio->hwo))
		    // clear only if we actually closed (no error)
		    lpwio->hwo = NULL;
	}
	// else data is still playing from the buffers...
    }
}


/** BOOL NEAR PASCAL FreeWaveHeaders(const LPWAVEIOCV lpwio)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (void)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 **  */

BOOL NEAR PASCAL FreeWaveHeaders(const LPWAVEIOCB lpwio)
{
    UINT    i;

    DPF("FreeWaveHeaders!\n");

    //
    //  free any previously allocated wave headers..
    //
    for (i = 0; i < MAX_WAVEHDRS; i++)
    {
        if (lpwio->apWaveHdr[i])
        {
            GlobalFreePtr(lpwio->apWaveHdr[i]);
            lpwio->apWaveHdr[i] = NULL;
        }
	lpwio->apWHUsed[i]=FALSE;
    }

    return (TRUE);
} /* FreeWaveHeaders() */


/** BOOL NEAR PASCAL AllocWaveHeaders(LPWAVEIOCV lpwio)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (WAVEFORMATEX *pwfx)
 *
 *  RETURN (BOOL NEAR PASCAL):
 *
 *
 *  NOTES:
 *
 **  */

BOOL NEAR PASCAL AllocWaveHeaders(const LPWAVEIOCB lpwio)
{
    UINT        i;
    WORD        wBytes;
    LPWAVEHDR   pwh;
    WAVEFORMATEX FAR *pwfx;

    if(lpwio->apWaveHdr[0])
    {
	for(i=0;i<MAX_WAVEHDRS; i++)
	    lpwio->apWHUsed[i]  = FALSE;
	return TRUE;
    }
	
    pwfx=&(lpwio->wfx);

    DPF("AllocWaveHeaders: allocating %u buffers\n", MAX_WAVEHDRS);

    //
    // get how many bytes we want to have in each buffer
    //
    wBytes = (WORD)wsupTimeToSamples(pwfx, AVG_BUF_MS);
    wBytes = (WORD)wsupSamplesToBytes(pwfx, wBytes);

    // need to align on block alignment...
    lpwio->dwBytesPerBuffer = wBytes;

    //
    //  allocate all of the wave headers/buffers for streaming
    //
    for (i = 0; i < MAX_WAVEHDRS; i++)
    {
        // Add DWORD to size make sure data does not use up a partial DWORD boundary
        pwh = GlobalAllocPtr(GMEM_FIXED|GMEM_SHARE, wBytes + sizeof(WAVEHDR) + sizeof(DWORD));
        if (pwh == NULL)
            goto AWH_ERROR_NOMEM;

        // Initialize header information
        pwh->lpData         = (LPSTR)(pwh + 1); // Data occurs directly after the header
        pwh->dwBufferLength = wBytes;
        pwh->dwFlags        = 0L;
        pwh->dwLoops        = 0L;

        lpwio->apWaveHdr[i] = pwh;
	lpwio->apWHUsed[i]  = FALSE;
    }

    return (TRUE);

AWH_ERROR_NOMEM:
    FreeWaveHeaders(lpwio);
    return (FALSE);
} /* AllocWaveHeaders() */

/** UINT NEAR PASCAL wioWaveOpen(hwnd, lphwave, lpwfx, fInput)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *      (LPHWAVE lphwave, LPWAVEFORMATEX lpwfx, BOOL fInput)
 *
 *  RETURN (UINT NEAR PASCAL):
 *
 *
 *  NOTES: C#
 *
 **  */

UINT NEAR PASCAL wioWaveOpen(HWND hwnd, HWAVEOUT FAR * lphwave, const LPWAVEFORMATEX lpwfx, BOOL fInput)
{
    UINT    uErr;

    if (!lphwave || !lpwfx)
        return (1);

    *lphwave = NULL;

    //
    //  first open the wave device DISALLOWING sync drivers (sync drivers
    //  do not work with a streaming buffer scheme; which is our preferred
    //  mode of operation)
    //
    //  if we cannot open a non-sync driver, then we will attempt for
    //  a sync driver and handle the situation
    //
    fSyncDriver = FALSE;
    if (fInput)
    {
        uErr = waveInOpen((HWAVEIN FAR *)lphwave, (UINT)WAVE_MAPPER, (WAVEFORMAT FAR* )lpwfx,
                            (DWORD)(UINT)hwnd, 0L, CALLBACK_WINDOW);
        if (uErr)
        {
            if (uErr == WAVERR_SYNC)
            {
                uErr = waveInOpen((HWAVEIN FAR *)lphwave, (UINT)WAVE_MAPPER,
                                   (WAVEFORMAT FAR* )lpwfx, (DWORD)(UINT)hwnd,
				    0L, CALLBACK_WINDOW|WAVE_ALLOWSYNC);
                if (uErr == MMSYSERR_NOERROR)
                {
                    fSyncDriver = TRUE;
                }
            }
        }
    }
    else
    {
        uErr = waveOutOpen(lphwave, (UINT)WAVE_MAPPER, (LPWAVEFORMAT)lpwfx,
                            (DWORD)(UINT)hwnd, 0L, CALLBACK_WINDOW);
        if (uErr)
        {
            if (uErr == WAVERR_SYNC)
            {
                uErr = waveOutOpen(lphwave, (UINT)WAVE_MAPPER,
                                   (LPWAVEFORMAT)lpwfx, (DWORD)(UINT)hwnd, 0L,
                                   CALLBACK_WINDOW|WAVE_ALLOWSYNC);
                if (uErr == MMSYSERR_NOERROR)
                {
                    fSyncDriver = TRUE;
                }
            }
        }
    }

    return (uErr);
}

BOOL WIOAPI wioIsPlaying(const LPWAVEIOCB lpwio)
{
    if(!lpwio)
	return FALSE;
    
    return lpwio->hwo !=0;
}


/* fOK = wioPlayWave()
 *
 * Start playing from the current position.
 *
 * On success, return TRUE.  On failure, display an error message
 * and return FALSE.
 */
LRESULT WIOAPI wioPlayWave(HWND hwnd, LPWAVEIOCB lpwio, LPCSTR lpszFilePath, DWORD dwFlags)
{
    WORD        w;
    UINT	u;
    LRESULT	lr;
    MMCKINFO    ck;

    DPF("wioPlayWave called\n");

    lr=WIOERR_NOERROR;
    
    if(!lpwio)
	lr=wioFileOpen(&lpwio, lpszFilePath, dwFlags);
    
    if(!lpwio)
	return lr;

    /* we are currently playing....*/
    if (lpwio->hwo != NULL)
	return TRUE;

    /* stop playing or recording */
    wioStopWave(lpwio);

    /* open the wave output device */
    u = wioWaveOpen(hwnd, &lpwio->hwo, &(lpwio->wfx), FALSE);
    if (u)
    {
	/* cannot open the waveform output device -- if the problem
	 * is that the wave format is not supported, tell the caller
	 */
	if (u == MMSYSERR_NOTSUPPORTED || (u == WAVERR_BADFORMAT))
	{
	    lr=WIOERR_NOTSUPPORTED;
	    goto wio_Play_Error;
	}
	else if (u == MMSYSERR_ALLOCATED)
	{
	    lr=WIOERR_ALLOCATED;
	    goto wio_Play_Error;
	}
	else
	{
	    /* unknown error */
	    lr=WIOERR_ERROR;
	    goto wio_Play_Error;
	}
    }
    // we now have the output wave device open

    //
    //  (re)open the file
    //
    if (!lpwio->hmmio)
	lpwio->hmmio = mmioOpen((LPSTR)lpszFilePath, NULL, MMIO_READ | MMIO_ALLOCBUF);
    
    if (!lpwio->hmmio)
        return WIOERR_FILEERROR;

    // get to the 'data' chunk
    if(mmioSeek(lpwio->hmmio, lpwio->dwDataOffset-sizeof(CHUNK), SEEK_SET)!=(LONG)(lpwio->dwDataOffset-sizeof(CHUNK)))
    {
	lr=WIOERR_FILEERROR;
	goto wio_Play_Error;
    }

    // now we are at start of 'data' chunk, descend into the chunk
    if (mmioDescend(lpwio->hmmio, &ck, NULL, 0))
    {
	lr=WIOERR_FILEERROR;
	goto wio_Play_Error;
    }

    if(fSyncDriver)
    {
	// driver is synchronous and will only take one buffer
	// at a time to play.... (speaker driver should be only synch driver)
	//
	WORD        wBytes;
	LPWAVEHDR   pwh=NULL;

	//
	// get how many bytes we want to have in the only buffer
	//
	wBytes = (WORD)wsupTimeToSamples(&(lpwio->wfx), SYNC_BUF_MS);
	wBytes = (WORD)wsupSamplesToBytes(&(lpwio->wfx), wBytes);

	
        pwh = GlobalAllocPtr(GMEM_FIXED|GMEM_SHARE, wBytes + sizeof(WAVEHDR) + sizeof(DWORD));
	if(!pwh)
	{
	    pwh->lpData         = (LPSTR)(pwh + 1); // Data occurs directly after the header
	    pwh->dwBufferLength = min(lpwio->dwBytesPerBuffer,lpwio->dwBytesLeft);
	    pwh->dwFlags	= 0L;
	    pwh->dwLoops	= 0L;
	    
	    // read the data from the file...
	    if (mmioRead(lpwio->hmmio, (HPSTR)lpwio->apWaveHdr[w]->lpData,
		lpwio->apWaveHdr[w]->dwBufferLength) !=
	          (LONG)lpwio->apWaveHdr[w]->dwBufferLength)
	    {
		lr=WIOERR_FILEERROR;
		goto wio_Play_Sync_Error;
	    }
	    
	    // prepare the buffer and header
	    if (waveOutPrepareHeader(lpwio->hwo, pwh, sizeof(WAVEHDR)) != 0)
	    {
		lr=WIOERR_ERROR;
		goto wio_Play_Sync_Error;
	    }

	    // since the driver is synchronous, this call will not return
	    // until buffer has been played.
	    //
	    if (waveOutWrite(lpwio->hwo, pwh, sizeof(WAVEHDR)) != 0)
	    {
		lr=WIOERR_ERROR;
		goto wio_Play_Sync_Error;
	    }

	    // data has been played...
	    
	}
wio_Play_Sync_Error:
	if(pwh)
	    GlobalFreePtr(pwh);
	waveOutClose(lpwio->hwo);
	lpwio->hwo=NULL;
	
	goto wio_Play_NoError;
    }

    // else it is an asynch driver (almost all are)
    
    AllocWaveHeaders(lpwio);

    // initialize to play entire file
    lpwio->dwBytesLeft=lpwio->dwDataBytes;

    // pause the output so the buffers won't play until we tell it to
    waveOutPause(lpwio->hwo);

    for(w=0;w<MAX_WAVEHDRS;w++)
    {
	// setup wave header
	lpwio->apWaveHdr[w]->dwBufferLength= min(lpwio->dwBytesPerBuffer,lpwio->dwBytesLeft);
	lpwio->apWaveHdr[w]->dwFlags = 0L;
	lpwio->apWaveHdr[w]->dwLoops = 0L;

	// read the data from the file...
	if (mmioRead(lpwio->hmmio, (HPSTR)lpwio->apWaveHdr[w]->lpData,
	    lpwio->apWaveHdr[w]->dwBufferLength) !=
	          (LONG)lpwio->apWaveHdr[w]->dwBufferLength)
	{
	    lr=WIOERR_FILEERROR;
	    goto wio_Play_Error;
	}

	// set the buffer length to play
	lpwio->dwBytesLeft-=lpwio->apWaveHdr[w]->dwBufferLength;

	// prepare the buffer and header
	if (waveOutPrepareHeader(lpwio->hwo, lpwio->apWaveHdr[w], sizeof(WAVEHDR)) != 0)
	    goto wio_Play_Error_WAVEOUT;

	// put it into the queue to play.  If output is not paused,
	// playback will start immediately if the driver is async
	//
	if (waveOutWrite(lpwio->hwo, lpwio->apWaveHdr[w], sizeof(WAVEHDR)) != 0)
	    goto wio_Play_Error_WAVEOUT;

	// the system is using this buffer
	lpwio->apWHUsed[w]=TRUE;

	// if no more data to read, stop getting more
	if(!lpwio->dwBytesLeft)
	    break;
    }

    // start the buffers playing (unpause)
    waveOutRestart(lpwio->hwo);

    // we are done starting the file to play.  return success
    fStopping = FALSE;
    lr=WIOERR_NOERROR;
    goto wio_Play_NoError;

wio_Play_Error_WAVEOUT:

    // there was an error working with the wave buffers
    wioStopWave(lpwio);
    for(w=0;w<MAX_WAVEHDRS;w++)
    {
	if(lpwio->apWHUsed[w])
	    waveOutUnprepareHeader(lpwio->hwo, lpwio->apWaveHdr[w], sizeof(WAVEHDR));
	lpwio->apWHUsed[w]=FALSE;
    }

    if(lpwio->hwo)
	if(!waveOutClose(lpwio->hwo))
	    lpwio->hwo = NULL;

    // fall through...
        
wio_Play_Error:

wio_Play_NoError:

    return lr;

}


/* wioStopWave(lpwio)
 *
 * Request waveform playback to stop.
 */
void WIOAPI wioStopWave(const LPWAVEIOCB lpwio)
{
    MSG             msg;

    if(!lpwio)
	return;
    
    if (lpwio->hwo != NULL)
    {
	waveOutReset(lpwio->hwo);
	fStopping = TRUE;
    }

    if (lpwio->hwo != NULL)
    {
	/* get messages from event queue and dispatch them,
	 * until all the MM_WOM_DONE messages are processed
	 */
	while (GetMessage(&msg, NULL, 0, 0))
	{
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	    
	    if (lpwio->hwo == NULL)
		break;
	}
    }
}



/** EOF: waveio.c **/
