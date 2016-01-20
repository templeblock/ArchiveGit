/** waveio.h
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
	Anonymous FTP from ftp.uu.net vendors\microsoft\multimedia
 *
 ** */


#ifndef _WAVEIO_H_
#define _WAVEIO_H_

#define WIOAPI	FAR PASCAL

#define MAX_WAVEHDRS    10


// default to total of 4 seconds (as MCI WAVE does)
#define AVG_BUF_MS	(4000/MAX_WAVEHDRS)
// play 4 seconds of audio for synchronous driver
#define SYNC_BUF_MS	(4000)

		    
typedef struct tWAVEIOCB
{
    HWAVEOUT	    hwo;
    DWORD           dwSize;
    DWORD           dwFlags;
    HMMIO           hmmio;

    DWORD	    dwDataOffset;
    DWORD           dwDataBytes;
    DWORD           dwDataSamples;
    DWORD	    dwBytesLeft;
    DWORD	    dwBytesPerBuffer;
    
    DISP FAR *	    pDisp;
    INFOCHUNK FAR * pInfo;
    WAVEHDR FAR *   apWaveHdr[MAX_WAVEHDRS];
    WORD	    apWHUsed[MAX_WAVEHDRS];

    WORD            wBytesPerBuffer;

    //
    //  this MUST be the last element in this structure--its length is
    //  not fixed; use ab[] to get at any extra bytes (note! the length
    //  of ab[] is in wfx.cbSize--this CAN be zero!)
    //
    WAVEFORMATEX    wfx;
    BYTE            ab[];

} WAVEIOCB, *PWAVEIOCB, FAR *LPWAVEIOCB;

DECLARE_HANDLE(HWIO);

typedef HWIO       *PHWIO;
typedef HWIO FAR   *LPHWIO;


//
//  error returns from waveio functions
//
#define WIOERR_BASE         (100)
#define WIOERR_NOERROR      (0)
#define WIOERR_ERROR        (WIOERR_BASE+1)
#define WIOERR_BADHANDLE    (WIOERR_BASE+2)
#define WIOERR_BADFLAGS     (WIOERR_BASE+3)
#define WIOERR_BADPARAM     (WIOERR_BASE+4)
#define WIOERR_BADSIZE      (WIOERR_BASE+5)
#define WIOERR_FILEERROR    (WIOERR_BASE+6)
#define WIOERR_NOMEM        (WIOERR_BASE+7)
#define WIOERR_BADFILE	    (WIOERR_BASE+8)
#define WIOERR_NODEVICE	    (WIOERR_BASE+9)
#define WIOERR_BADFORMAT    (WIOERR_BASE+10)
#define WIOERR_ALLOCATED    (WIOERR_BASE+11)
#define WIOERR_NOTSUPPORTED (WIOERR_BASE+12)

//
//  function prototypes and flag definitions
//
LRESULT WIOAPI	 wioFileOpen(LPWAVEIOCB FAR * lplpwio, LPCSTR lpszFilePath, DWORD dwFlags);
LRESULT WIOAPI	wioFileClose(LPWAVEIOCB FAR * lplpwio, DWORD dwFlags);


LRESULT WIOAPI  wioPlayWave(HWND hwnd, LPWAVEIOCB lpwio, LPCSTR lpszFilePath, DWORD dwFlags);
void WIOAPI  wioWaveOutDone(const LPWAVEIOCB lpwio, const LPWAVEHDR pWaveHdr);
void WIOAPI	wioStopWave(const LPWAVEIOCB lpwio);
BOOL WIOAPI    wioIsPlaying(const LPWAVEIOCB lpwio);

#endif


/** EOF: waveio.h **/
