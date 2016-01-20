// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

#define THRESH	127

/***********************************************************************/
VOID ConvertData( lpIn, iInDepth, iCount, lpOut, iOutDepth )
/***********************************************************************/
LPTR lpIn;
int iInDepth;
int iCount;
LPTR lpOut;
int iOutDepth;
{
switch (iOutDepth)
    {
    case 0:
	switch (iInDepth)
	    {
	    case 0:
		copy(lpIn, lpOut, (iCount+7)/8);
		break;
	    case 1:
		con8to1(lpIn, iCount, lpOut);
		break;
	    case 2:
		con16to1((LPWORD)lpIn, iCount, lpOut);
		break;
	    case 3:
		con24to1((LPRGB)lpIn, iCount, lpOut);
		break;
	    default:
		break;
	    }
	break;
    case 1:
	switch (iInDepth)
	    {
	    case 0:
		con1to8(lpIn, iCount, lpOut);
		break;
	    case 1:
		copy(lpIn, lpOut, iCount);
		break;
	    case 2:
		con16to8((LPWORD)lpIn, iCount, lpOut);
		break;
	    case 3:
		con24to8((LPRGB)lpIn, iCount, lpOut);
		break;
	    default:
		break;
	    }
	break;
    case 2:
	switch (iInDepth)
	    {
	    case 0:
		con1to16(lpIn, iCount, (LPWORD)lpOut);
		break;
	    case 1:
		con8to16(lpIn, iCount, (LPWORD)lpOut);
		break;
	    case 2:
		copy(lpIn, lpOut, iCount);
		copy(lpIn+iCount, lpOut+iCount, iCount);
		break;
	    case 3:
		con24to16((LPRGB)lpIn, iCount, (LPWORD)lpOut);
		break;
	    default:
		break;
	    }
	break;
    case 3:
	switch (iInDepth)
	    {
	    case 0:
		con1to24(lpIn, iCount, (LPRGB)lpOut);
		break;
	    case 1:
		con8to24(lpIn, iCount, (LPRGB)lpOut);
		break;
	    case 2:
		con16to24((LPWORD)lpIn, iCount, (LPRGB)lpOut);
		break;
	    case 3:
		copy(lpIn, lpOut, iCount);
		copy(lpIn+iCount, lpOut+iCount, iCount);
		copy(lpIn+iCount+iCount, lpOut+iCount+iCount, iCount);
		break;
	    default:
		break;
	    }
	break;
    default:
	break;
    }
}

/***********************************************************************/
void con1to8( lpIn, iCount, lpOut )
/***********************************************************************/
LPTR	lpIn;
int	iCount;
LPTR 	lpOut;
{
BYTE	byte, bit, black, white;
BOOL normal = NO;

if (normal)
	{ white = 0;  black = 255; }
else	{ white = 255; black = 0;  }

bit = 1;
while ( --iCount >= 0 )
	{
	if ( --bit == 0 )
		{
		byte = *lpIn++;
		bit = 8;
		}
	if ( byte > THRESH )
		*lpOut++ = black;
	else	*lpOut++ = white;
	byte <<= 1;
	}
}

/***********************************************************************/
void con1to16( lpIn, iCount, lpOut )
/***********************************************************************/
LPTR	lpIn;
LPWORD  lpOut;
int	iCount;
{
BYTE	byte, bit;
WORD	black, white;
BOOL normal = NO;

if (normal)
	{ white = 0;  black = 0x7FFF; }
else	{ white = 0x7FFF; black = 0;  }

bit = 1;
while ( --iCount >= 0 )
	{
	if ( --bit == 0 )
		{
		byte = *lpIn++;
		bit = 8;
		}
	if ( byte > THRESH )
		*lpOut++ = black;
	else	*lpOut++ = white;
	byte <<= 1;
	}
}

/***********************************************************************/
void con1to24( lpIn, iCount, lpOut )
/***********************************************************************/
LPTR	lpIn;
LPRGB  lpOut;
int	iCount;
{
BYTE	byte, bit;
BYTE	black, white;
BOOL normal = NO;

if (normal)
	{ white = 0;  black = 255; }
else	{ white = 255; black = 0;  }

bit = 1;
while ( --iCount >= 0 )
	{
	if ( --bit == 0 )
		{
		byte = *lpIn++;
		bit = 8;
		}
	if ( byte > THRESH )
		{
		lpOut->red = black;
		lpOut->green = black;
		lpOut->blue = black;
		}
	else	
		{
		lpOut->red = white;
		lpOut->green = white;
		lpOut->blue = white;
		}
	++lpOut;
	byte <<= 1;
	}
}

/***********************************************************************/
void con8to1( lpIn, iCount, lpOut )
/***********************************************************************/
LPTR	lpIn;
int	iCount;
LPTR	lpOut;
{
BYTE	byte, bit;

byte = 0;
bit = 128;
while ( --iCount >= 0 )
	{
	if (*lpIn++ > THRESH)
		byte |= bit;
	if ( (bit >>= 1) == 0 )
		{
		*lpOut++ = byte;
		byte = 0;
		bit = 128;
		}
	}

if ( bit != 128 )
	*lpOut++ = byte;
}

/***********************************************************************/
VOID con8to16( lpIn, iCount, lpOut )
/***********************************************************************/
    LPTR lpIn;
    int iCount;
    LPWORD lpOut;
    {
    RGBS rgb;
    
    while (--iCount >= 0)
        {
        rgb.red = rgb.green = rgb.blue = *lpIn++;
        *lpOut++ = RGBtoMiniRGB(&rgb);
        }
    }
    
/***********************************************************************/
VOID con8to24( lpIn, iCount, lpOut )
/***********************************************************************/
    LPTR lpIn;
    int iCount;
    LPRGB lpOut;
    {
    RGBS rgb;
    BYTE lum;
    
    while (--iCount >= 0)
        {
        lum = *lpIn++;
	lpOut->red = lum;
	lpOut->green = lum;
	lpOut->blue = lum;
	++lpOut;
        }
    }
    
/***********************************************************************/
void con16to1( lpIn, iCount, lpOut )
/***********************************************************************/
LPWORD	lpIn;
int	iCount;
LPTR	lpOut;
{
BYTE	byte, bit, lum;

byte = 0;
bit = 128;
while ( --iCount >= 0 )
	{
        lum = MiniRGBtoL(*lpIn++);
	if (lum > THRESH)
		byte |= bit;
	if ( (bit >>= 1) == 0 )
		{
		*lpOut++ = byte;
		byte = 0;
		bit = 128;
		}
	}

if ( bit != 128 )
	*lpOut++ = byte;
}

/***********************************************************************/
VOID con16to8( lpMiniRGB, iCount, lpOut )
/***********************************************************************/
    LPWORD lpMiniRGB;
    int iCount;
    LPTR lpOut;
    {
    while (--iCount >= 0)
        {
        *lpOut = MiniRGBtoL(*lpMiniRGB);
        ++lpMiniRGB;
        ++lpOut;
        }
    }
    
/***********************************************************************/
VOID con16to24( lpMiniRGB, iCount, lpRGB )
/***********************************************************************/
    LPWORD lpMiniRGB;
    int iCount;
    LPRGB lpRGB;
    {
    WORD wRGB;
    while ( --iCount >= 0 )
	{
	wRGB = *lpMiniRGB++;
	lpRGB->red = MaxiR( wRGB );
	lpRGB->green = MaxiG( wRGB );
	lpRGB->blue = MaxiB( wRGB );
	++lpRGB;
	}
    }
    
/***********************************************************************/
void con24to1( lpIn, iCount, lpOut )
/***********************************************************************/
LPRGB	lpIn;
int	iCount;
LPTR	lpOut;
{
BYTE	byte, bit, lum;

byte = 0;
bit = 128;
while ( --iCount >= 0 )
	{
        lum = (((int)lpIn->red * 30) + ((int)lpIn->green * 59) + ((int)lpIn->blue * 11))/100;
	++lpIn;
	if (lum > THRESH)
		byte |= bit;
	if ( (bit >>= 1) == 0 )
		{
		*lpOut++ = byte;
		byte = 0;
		bit = 128;
		}
	}

if ( bit != 128 )
	*lpOut++ = byte;
}

/***********************************************************************/
VOID con24to8( lpIn, iCount, lpOut )
/***********************************************************************/
    LPRGB lpIn;
    int iCount;
    LPTR lpOut;
    {
    while (--iCount >= 0)
        {
        *lpOut = (((int)lpIn->red * 30) + ((int)lpIn->green * 59) + ((int)lpIn->blue * 11))/100;
      /*  *lpOut = RGBtoL(lpIn);*/
        ++lpIn;
        ++lpOut;
        }
    }
    
/***********************************************************************/
VOID con24to16( lpRGB, iCount, lpMiniRGB )
/***********************************************************************/
    LPRGB lpRGB;
    int iCount;
    LPWORD lpMiniRGB;
    {
    while (--iCount >= 0)
        {
        *lpMiniRGB = RGBtoMiniRGB(lpRGB);
        lpRGB += 3;
        ++lpMiniRGB;
        }
    }
    
/***********************************************************************/
BOOL CreateHisto( lpFrame, lppRHisto, lppGHisto, lppBHisto )
/***********************************************************************/
LPFRAME lpFrame;
LPPWORD lppRHisto, lppGHisto, lppBHisto;
{
RECT rMask;
RGBS rgb;
LPTR lpLine;
LPWORD lpRHisto;
LPWORD lpGHisto;
LPWORD lpBHisto;
int x, y;
WORD maxValue;

*lppRHisto = NULL;
*lppGHisto = NULL;
*lppBHisto = NULL;
if (!lpFrame)
	return(FALSE);
if (!(lpRHisto = (LPWORD)Alloc(257L * 2L)))
	return(FALSE);
if (!(lpGHisto = (LPWORD)Alloc(257L * 2L)))
	{
	FreeUp((LPTR)lpRHisto);
	return(FALSE);
	}
if (!(lpBHisto = (LPWORD)Alloc(257L * 2L)))
	{
	FreeUp((LPTR)lpRHisto);
	FreeUp((LPTR)lpGHisto);
	return(FALSE);
	}

clr((LPTR)lpRHisto, 257L * 2L);
clr((LPTR)lpGHisto, 257L * 2L);
clr((LPTR)lpBHisto, 257L * 2L);
mask_rect(&rMask);
maxValue = 0;
++lpRHisto; /* first word is maxValue */
++lpGHisto;
++lpBHisto;
for (y = rMask.top; y <= rMask.bottom; ++y)
	{
	AstralClockCursor(y-rMask.top, RectHeight(&rMask));
	lpLine = frame_ptr(0, rMask.left, y, NO);
	if (!lpLine)
		continue;
	for (x = rMask.left; x <= rMask.right; ++x, lpLine += lpFrame->Depth)
		{
		frame_getRGB(lpLine, &rgb);
		lpRHisto[rgb.red]++;
		lpGHisto[rgb.green]++;
		lpBHisto[rgb.blue]++;
		if (lpRHisto[rgb.red] > maxValue)
			maxValue = lpRHisto[rgb.red];
		if (lpRHisto[rgb.green] > maxValue)
			maxValue = lpGHisto[rgb.green];
		if (lpRHisto[rgb.blue] > maxValue)
			maxValue = lpRHisto[rgb.blue];
		}
	}
--lpRHisto;
--lpGHisto;
--lpBHisto;
*lpRHisto = maxValue;
*lpGHisto = maxValue;
*lpBHisto = maxValue;
*lppRHisto = lpRHisto;
*lppGHisto = lpGHisto;
*lppBHisto = lpBHisto;
return(TRUE);
}