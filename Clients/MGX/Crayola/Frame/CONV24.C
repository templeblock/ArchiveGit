// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "superlib.h"
#include "macros.h"
#include "convert.h"

// The following converter are located here.
// Some have been converted to assembler and are #ifdef'd out

// For 24 bit display drivers
//	Convert32to24   (LPCMYK,int,LPINT,LPTR,int,int,LPBLT);
//	Convert24to24   (LPRGB,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8to24    (LPTR,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8Pto24    (LPTR,int,LPINT,LPTR,int,int,LPBLT);

/*=========================================================================*/

void Convert32to24(
	LPCMYK lpPixel,
	int    iCount,
	LPINT  lpError,
	LPTR   lpOutput,
	int    xDiva, 
	int    yDiva,
	LPBLTSESSION  lpBltSession)
{
	LPTR lpSrc = (LPTR)lpPixel;
	LPTR lpDst = (LPTR)lpOutput;
	int iRed, iGrn, iBlu, iBlk;

	while( iCount-- > 0)
	{
		iRed = (*lpSrc++);
		iGrn = (*lpSrc++);
		iBlu = (*lpSrc++);
		iBlk = (*lpSrc++);

		RGB_FROM_CMYK(iRed, iGrn, iBlu, iRed, iGrn, iBlu, iBlk);

		(*lpDst++) = iBlu;
		(*lpDst++) = iGrn;
		(*lpDst++) = iRed;
	}
}

#ifdef C_CODE
/************************************************************************/
void Convert24to24( LPRGB lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                        int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
while (--iCount >= 0)
    {
	 *lpOutput++ = lpPixel->blue;
	 *lpOutput++ = lpPixel->green;
	 *lpOutput++ = lpPixel->red;
	 ++lpPixel;
	 }
}
#endif

#ifdef C_CODE
/************************************************************************/
void Convert8to24( LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                        int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
BYTE gray;

while (--iCount >= 0)
    {
    gray = *lpPixel++;
    *lpOutput++ = gray;
    *lpOutput++ = gray;
    *lpOutput++ = gray;
    }
}
#endif

/************************************************************************/
void Convert8Pto24( LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                        int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
LPRGB lpRGBmap, lpRGB;
BYTE i;

lpRGBmap = lpBltSession->TypeInfo.ColorMap->RGBData;
while (--iCount >= 0)
	{
	i = *lpPixel++;
	lpRGB = lpRGBmap + i;
	*lpOutput++ = lpRGB->blue;
	*lpOutput++ = lpRGB->green;
	*lpOutput++ = lpRGB->red;
	}
}


