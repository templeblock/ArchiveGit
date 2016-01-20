/** wavesup.c
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
 ** */


//
//  the includes we need
//
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "mmreg.h"
#include "wavesup.h"

//
//  external prototype from muldiv32.asm
//
DWORD FAR PASCAL muldiv32(DWORD dwNumber, DWORD dwNumerator, DWORD dwDenominator);


/** DWORD WSUPAPI wsupBytesToSamples(LPWAVEFORMATEX pwfx, DWORD dwBytes)
 *
 *  DESCRIPTION:
 *      convert a byte offset into a sample offset.
 *
 *      dwSamples = (dwBytes / nAveBytesPerSec) * nSamplesPerSec
 *
 *  ARGUMENTS:
 *      (LPWAVEFORMATEX pwfx, DWORD dwBytes)
 *
 *  RETURN (DWORD WSUPAPI):
 *
 *
 *  NOTES:
 *
 ** */

DWORD WSUPAPI wsupBytesToSamples(LPWAVEFORMATEX pwfx, DWORD dwBytes)
{
    return (muldiv32(dwBytes, pwfx->nSamplesPerSec, pwfx->nAvgBytesPerSec));
} /* wsupBytesToSamples() */


/** DWORD WSUPAPI wsupSamplesToBytes(LPWAVEFORMATEX pwfx, DWORD dwSamples)
 *
 *  DESCRIPTION:
 *      convert a sample offset into a byte offset, with correct alignment
 *      to nBlockAlign.
 *
 *      dwBytes = (dwSamples / nSamplesPerSec) * nBytesPerSec
 *
 *  ARGUMENTS:
 *      (LPWAVEFORMATEX pwsup, DWORD dwSamples)
 *
 *  RETURN (DWORD WSUPAPI):
 *
 *
 *  NOTES:
 *
 ** */

DWORD WSUPAPI wsupSamplesToBytes(LPWAVEFORMATEX pwfx, DWORD dwSamples)
{
    DWORD   dwBytes;

    dwBytes = muldiv32(dwSamples, pwfx->nAvgBytesPerSec, pwfx->nSamplesPerSec);

    // now align the byte offset to nBlockAlign
#ifdef ROUND_UP
    dwBytes = ((dwBytes + pwfx->nBlockAlign - 1) / pwfx->nBlockAlign) * pwfx->nBlockAlign;
#else
    dwBytes = (dwBytes / pwfx->nBlockAlign) * pwfx->nBlockAlign;
#endif

    return (dwBytes);
} /* wsupSamplesToBytes() */


/** DWORD WSUPAPI wsupSamplesToTime(LPWAVEFORMATEX pwfx, DWORD dwSamples)
 *
 *  DESCRIPTION:
 *      convert a sample offset into a time offset in milliseconds.
 *
 *      dwTime = (dwSamples/nSamplesPerSec) * 1000
 *
 *  ARGUMENTS:
 *      (LPWAVEFORMATEX pwfx, DWORD dwSamples)
 *
 *  RETURN (DWORD WSUPAPI):
 *
 *
 *  NOTES:
 *
 ** */

DWORD WSUPAPI wsupSamplesToTime(LPWAVEFORMATEX pwfx, DWORD dwSamples)
{
    return (muldiv32(dwSamples, 1000, pwfx->nSamplesPerSec));
} /* wsupSamplesToTime() */


/** DWORD WSUPAPI wsupTimeToSamples(LPWAVEFORMATEX pwfx, DWORD dwTime)
 *
 *  DESCRIPTION:
 *      convert a time index in milliseconds to a sample offset.
 *
 *      dwSamples = (dwTime / 1000) * nSamplesPerSec
 *
 *  ARGUMENTS:
 *      (LPWAVEFORMATEX pwfx, DWORD dwTime)
 *
 *  RETURN (DWORD WSUPAPI):
 *
 *
 *  NOTES:
 *
 ** */

DWORD WSUPAPI wsupTimeToSamples(LPWAVEFORMATEX pwfx, DWORD dwTime)
{
    return (muldiv32(dwTime, pwfx->nSamplesPerSec, 1000));
} /* wsupTimeToSamples() */


typedef struct strformat
{
    char *  pszformat;
    UINT    wFormat;
} STRFORMAT;

static STRFORMAT formats[] =
{
    {"Reserved for Unknown", WAVE_FORMAT_UNKNOWN    },
    {"PCM",                  WAVE_FORMAT_PCM        },
    {"Microsoft ADPCM",      WAVE_FORMAT_ADPCM      },
    {"IBM CVSD",             WAVE_FORMAT_IBM_CVSD   },
    {"CCITT A-Law",          WAVE_FORMAT_ALAW       },
    {"CCITT mu-Law",         WAVE_FORMAT_MULAW      },
    {"OKI ADPCM",            WAVE_FORMAT_OKI_ADPCM  },
    {"Digisound DIGISTD",    WAVE_FORMAT_DIGISTD    },
    {"Digisound DIGIFIX",    WAVE_FORMAT_DIGIFIX    },
    {NULL,                   0                      }
};


/** void WSUPAPI wsupGetFormatName(char FAR * lpszBuf, LPWAVEFORMATEX lpwfx)
 *
 *  DESCRIPTION:
 *      
 *
 *  ARGUMENTS:
 *
 *  RETURN void
 *
 *
 *  NOTES:
 *
 ** */
void WSUPAPI wsupGetFormatName(LPWAVEFORMATEX lpwfx, LPSTR lpszBuf)
{

    unsigned u;
    for(u=0;formats[u].pszformat;u++)
    {
	if(formats[u].wFormat==lpwfx->wFormatTag)
	{
	    lstrcpy(lpszBuf,formats[u].pszformat);
	    return;
	}
    }

    lstrcpy(lpszBuf,"<<Unknown Format>>");
    
}



/** EOF: wavesup.c **/
