/** pcm.c
 *
     Copyright (C) Microsoft Corp. 1991, 1992.  All rights reserved.

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

    
*/


#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "pcm.h"


#define MAX_CHANNELS	2

/*

WARNING:
    This function is completely UNTESTED!!!!  Beware.

 */

DWORD PCMAPI pcmtopcm(LPPCMWAVEFORMAT lpwfPCMSrc, HPSTR hpSrc, LPPCMWAVEFORMAT lpwfPCMDst, HPSTR hpDst, DWORD dwSrcLen)
{
    DWORD   dwTotalPos;
    DWORD   dwTotalDst;
    BYTE    bSrcChannels;
    BYTE    bDstChannels;
    DWORD   dwSrcSampleRate;
    DWORD   dwDstSampleRate;
    WORD    wSrcBPS;	// Bits Per Sample
    WORD    wDstBPS;
    short   newSample16[MAX_CHANNELS];
    BYTE    newSample8[MAX_CHANNELS];
    WORD    wDecimationFactor;
    WORD    wInterpFactor;
    
    BOOL    fBPSUP=FALSE;	// convert BPS up
    BOOL    fBPSDOWN=FALSE;	// convert BPS down
    BYTE    m;
    WORD    w;
    

    bSrcChannels=(BYTE)lpwfPCMSrc->wf.nChannels;
    bDstChannels=(BYTE)lpwfPCMDst->wf.nChannels;


    dwSrcSampleRate=lpwfPCMSrc->wf.nSamplesPerSec;
    dwDstSampleRate=lpwfPCMDst->wf.nSamplesPerSec;

    if(dwSrcSampleRate<dwDstSampleRate)
    {
	wDecimationFactor=(WORD)(dwDstSampleRate/dwSrcSampleRate);
    }
    else if(dwDstSampleRate<dwSrcSampleRate)
	wInterpFactor=(WORD)(dwSrcSampleRate/dwDstSampleRate);
    else
	wInterpFactor=1;
    

    wSrcBPS=lpwfPCMSrc->wBitsPerSample;
    wDstBPS=lpwfPCMDst->wBitsPerSample;

    if(wSrcBPS<=8 && wDstBPS>8)
	fBPSUP=TRUE;
    if(wSrcBPS>8 && wDstBPS<=8)
	fBPSDOWN=TRUE;

    dwTotalPos=dwTotalDst=0;
    
    //
    //  step through each sample of PCM data and decode it to the requested
    //  PCM format (8 or 16 bit, mono/stereo, and sample rate).
    //
    while (dwTotalPos < dwSrcLen)
    {
	if(wDecimationFactor)
	{
	    // skip 
	    if(wSrcBPS<=8)
	    {
		hpSrc+=wDecimationFactor;
		dwTotalPos+=wDecimationFactor;
	    }
	    else
	    {
		hpSrc+=2*wDecimationFactor;
		dwTotalPos+=2*wDecimationFactor;
	    }
	}
        for (m = 0; m < bSrcChannels; m++)
        {
	    if(fBPSUP)
	    {
		newSample16[m]=(*hpSrc++ << 8 ) ^ 0x8000;
		dwTotalPos++;
	    }
	    else if(fBPSDOWN)
	    {
		newSample8[m]=(BYTE)((*(short far *)hpSrc ^0x8000) + 0x0080) >> 8;
		dwTotalPos+=2;
		hpSrc+=2;
	    }
	    else if(wSrcBPS<=8)
	    {
		newSample8[m]=*hpSrc++;
		dwTotalPos++;
	    }
	    else
	    {
		newSample16[m]=*(short far*)hpSrc;
		dwTotalPos+=2;
		hpSrc+=2;
	    }
        }


	for(w=0; w < wInterpFactor; w++)
	{
	    for (m = 0; m < bDstChannels; m++)
	    {
		if(fBPSDOWN || wDstBPS<=8)
		{
		    *hpDst++=newSample8[bSrcChannels<bDstChannels ? (bSrcChannels-1) : m];
		    dwTotalDst++;
		}
		else
		{
		    *(short far *)hpDst=newSample16[ bSrcChannels<bDstChannels ? (bSrcChannels-1) : m];
		    dwTotalDst+=2;
		    hpDst+=2;
		}
	    }
	
	}
    }

    return (dwTotalDst);
}

//---------------------------------------------------------------------------
//
//  the code below provides 'support' routines for building/verifying PCM
//  headers, etc.
//
//---------------------------------------------------------------------------

BOOL PCMAPI pcmIsValidFormat(LPWAVEFORMATEX lpwfx)
{
    if (!lpwfx)
        return (FALSE);

    if (lpwfx->wFormatTag != WAVE_FORMAT_PCM)
        return (FALSE);

    if ((lpwfx->wBitsPerSample != 8) && (lpwfx->wBitsPerSample != 16))
        return (FALSE);

    if ((lpwfx->nChannels < 1) || (lpwfx->nChannels > MAX_CHANNELS))
        return (FALSE);

    if((lpwfx->nSamplesPerSec != 44100) &&
       (lpwfx->nSamplesPerSec != 22050) &&
       (lpwfx->nSamplesPerSec != 11025)	 )
	return (FALSE);
    
    return (TRUE);
} /* pcmIsValidFormat() */


/** EOF: pcm.c **/
