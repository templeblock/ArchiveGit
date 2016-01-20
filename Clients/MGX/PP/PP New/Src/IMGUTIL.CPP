//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "id.h"

#define THRESH  127

/***********************************************************************/

void la2con( LPTR lpIn, int iCount, LPTR lpOut, BOOL bNormal )
{
	int iMask, iValue;

	iMask  = 1<<7;
	iValue = *lpIn++;

	if (bNormal)
	{
		while(iCount-- > 0)
		{
			*lpOut++ = (iMask&iValue) ? 0xFF : 0x00;

			if (iMask & 1)
			{
				iValue = *lpIn++;
				iMask = 1<<7;
			}
			else
			{
				iMask >>= 1;
			}
		}
	}
	else
	{
		while(iCount-- > 0)
		{
			*lpOut++ = (iMask&iValue) ? 0x00 : 0xFF;

			if (iMask & 1)
			{
				iValue = *lpIn++;
				iMask = 1<<7;
			}
			else
			{
				iMask >>= 1;
			}
		}
	}
}

/***********************************************************************/

void con2la( LPTR lpIn, int iCount, LPTR lpOut )
{
	int iMask, iValue;

	iMask  = 1<<7;
	iValue = 0;

	while(iCount-- > 0)
	{
		if (*lpIn++ > THRESH)
		{
			iValue |= iMask;
		}

		if (iMask & 1)
		{
			*lpOut++ = iValue;
			iValue = 0;
			iMask = 1<<7;
		}
		else
		{
			iMask >>= 1;
		}
	}

	if (iMask != (1<<7))
	{
		*lpOut = iValue;
	}
}

/***********************************************************************/

#define MAXVALUE 255
#define CLUMP   1
#define DECISION_X ((x++ % CLUMP) == 0)

/***********************************************************************/
void diffuse( int x, int y, int iColor, LPTR lpLut, LPTR lpIn, int count,
                  LPTR lpOut )
/***********************************************************************/
{
BYTE    byte, bit;
LPINT   lpError;
int     pixel, iErrorCount;
static LPTR lpDiffuseMemory;
static int iDiffuseCount;
static ULONG lSeed = 0xDEADBABA;

count++; /* need 1 additional slot for errors */
iErrorCount = count * ( iColor ? (4*sizeof(int)) : sizeof(int) );
if ( lpDiffuseMemory && (iErrorCount != iDiffuseCount) )
	{
	FreeUp( lpDiffuseMemory );
	lpDiffuseMemory = NULL;
	}

if ( !lpDiffuseMemory )
	{
	iDiffuseCount = iErrorCount;
	if ( !(lpDiffuseMemory = Alloc( (long)iDiffuseCount )) )
		return;
	clr( lpDiffuseMemory, iDiffuseCount );
	}
else
if ( y == 0 ) /* if this the the start of the image, clear the error buffer */
	clr( lpDiffuseMemory, iDiffuseCount );

lpError = (LPINT)lpDiffuseMemory;
if ( iColor )
	lpError += (count * (iColor-1));

byte = 0;
bit = 128;

while (--count > 0)
	{
	if ( lpLut )
		pixel = lpLut[ *lpIn++ ];
	else
		pixel = *lpIn++;

	if ( (pixel += *lpError) <= THRESH )
		byte |= bit;
	else
		pixel -= MAXVALUE;

	if ( lSeed & BIT18 )
		{
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0;
		*lpError  += pixel;
		}
	else
		{
		lSeed += lSeed;
		*lpError++ = pixel;
		}

	if ( (bit >>= 1) == 0 )
		{
		*lpOut++ = byte^0xFF;
		byte = 0;
		bit = 128;
		}
	}

if ( bit != 128 )
	*lpOut++ = byte^0xFF;
}

/***********************************************************************/
void dither( int x, int y, LPTR lpIn, int count, LPTR lpOut )
/***********************************************************************/
{
BYTE    byte, bit;
PTR     pPat, pEndPat, pStartPat;
static BYTE Pattern[] = {
	112, 96,104,120,143,159,151,135, 
	 72,  0,  8, 32,183,254,247,223, 
	 80, 16, 24, 40,175,239,231,215, 
	 88, 48, 56, 64,167,207,199,191, 
	139,155,147,131,116,100,108,124, 
	179,254,243,219, 76,  0, 12, 36, 
	171,235,227,211, 84, 20, 28, 44, 
	163,203,195,187, 92, 52, 60, 68 };

pStartPat = Pattern   + 8*(y&7);
pEndPat   = pStartPat + 8;
pPat      = pStartPat + (x&7);

byte = 0;
bit = 128;
while (--count >= 0)
	{
	if (*lpIn++ > *pPat++)
		byte |= bit;
	
	if ( (bit >>= 1) == 0 )
		{
		*lpOut++ = byte;
		byte = 0;
		bit = 128;
		}

	if ( pPat >= pEndPat )
		pPat = pStartPat;
	}

if ( bit != 128 )
	*lpOut++ = byte;
}
#ifdef UNUSED
// Static prototypes
extern void con1to16 (LPTR   lpIn,     int iCount,LPWORD lpOut);
extern void con8to16 (LPTR   lpIn,     int iCount,LPWORD lpOut);
extern void con24to16(LPRGB  lpRGB,    int iCount,LPWORD lpMiniRGB);
extern void con32to16(LPCMYK lpCMYK,   int iCount,LPWORD lpMiniRGB);

extern void con16to1 (LPWORD lpIn,     int iCount,LPTR   lpOut);
extern void con16to8 (LPWORD lpMiniRGB,int iCount,LPTR   lpOut);
extern void con16to24(LPWORD lpMiniRGB,int iCount,LPRGB  lpRGB);
extern void con16to32(LPWORD lpMiniRGB,int iCount,LPCMYK lpCMYK);

#define RGB_TO_GRAY(rgb) \
	(((rgb->red*38) + (rgb->green*76) + (rgb->blue*14))>>7)

/***********************************************************************/

void ConvertData( LPTR lpIn, int iInDepth, int iCount, LPTR lpOut,
                     int iOutDepth )
{
	static CFrameTypeConvert TypeConvert;

	switch (iOutDepth)
	{
		case 0:
			switch (iInDepth)
		    {
	    		case 0:
					copy(lpIn, lpOut, (iCount+7)/8);
				break;

			    case 1:
					TypeConvert.GrayScaleToLineArt(lpIn, lpOut, 0, iCount);
				break;

			    case 2:
					con16to1((LPWORD)lpIn, iCount, lpOut);
				break;

			    case 3:
					TypeConvert.RGBColorToLineArt((LPRGB)lpIn, lpOut, 0, iCount );
				break;

				case 4:
					TypeConvert.CMYKColorToLineArt((LPCMYK)lpIn, lpOut, 0, iCount);
				break;
		    }
		break;

		case 1:
			switch (iInDepth)
		    {
			    case 0:
					TypeConvert.LineArtToGrayScale(lpIn, lpOut, 0, iCount);
				break;

			    case 1:
					copy(lpIn, lpOut, iCount);
				break;

			    case 2:
					con16to8((LPWORD)lpIn, iCount, lpOut);
				break;

			    case 3:
					TypeConvert.RGBColorToGrayScale( (LPRGB)lpIn, lpOut, 0, iCount );
				break;

				case 4:
					TypeConvert.CMYKColorToGrayScale( (LPCMYK)lpIn, lpOut, 0, iCount );
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

				case 4:
					con32to16((LPCMYK)lpIn, iCount, (LPWORD)lpOut);
				break;
		    }
		break;

		case 3:
			switch (iInDepth)
		    {
			    case 0:
					TypeConvert.LineArtToRGBColor(lpIn, (LPRGB)lpOut, 0, iCount);
				break;

			    case 1:
					TypeConvert.GrayScaleToRGBColor(lpIn, (LPRGB)lpOut, 0, iCount );
				break;

			    case 2:
					con16to24((LPWORD)lpIn, iCount, (LPRGB)lpOut);
				break;

			    case 3:
					copy(lpIn, lpOut, iCount);
					copy(lpIn+iCount, lpOut+iCount, iCount);
					copy(lpIn+iCount+iCount, lpOut+iCount+iCount, iCount);
				break;

				case 4:
					TypeConvert.CMYKColorToRGBColor( (LPCMYK)lpIn, (LPRGB)lpOut, 0, iCount );
				break;
	    	}
		break;

		case 4:
			switch (iInDepth)
		    {
			    case 0:
					TypeConvert.LineArtToCMYKColor(lpIn, (LPCMYK)lpOut, 0, iCount);
				break;

			    case 1:
					TypeConvert.GrayScaleToCMYKColor(lpIn, (LPCMYK)lpOut, 0, iCount);
				break;

			    case 2:
					con16to32((LPWORD)lpIn, iCount, (LPCMYK)lpOut);
				break;

			    case 3:
					TypeConvert.RGBColorToCMYKColor( (LPRGB)lpIn, (LPCMYK)lpOut, 0, iCount );
				break;

				case 4:
					copy(lpIn, lpOut, iCount);
					copy(lpIn+iCount, lpOut+iCount, iCount);
					copy(lpIn+iCount+iCount, lpOut+iCount+iCount, iCount);
					copy(lpIn+iCount+iCount+iCount, lpOut+iCount+iCount+iCount, iCount);
				break;
			}
		break;
	}
}

/***********************************************************************/

static void con1to16( LPTR lpIn, int iCount, LPWORD lpOut )
{
	register int mask, byte;

	byte = *lpIn++;
	mask = 1<<7;

	while (iCount-- > 0)
	{
		*lpOut++ = (mask & byte) ? 0x7FFF : 0x0000;

		if (mask & 1)
		{
			byte = *lpIn++;
			mask = 1<<7;
		}
		else
		{
			mask >>= 1;
		}
	}
}

/***********************************************************************/

static void con8to16( LPTR lpIn, int iCount, LPWORD lpOut )
{
	RGBS rgb;

	while (iCount-- > 0)
	{
		rgb.red   =
		rgb.green =
		rgb.blue  = *lpIn++;

		*lpOut++ = RGBtoMiniRGB(&rgb);
	}
}

/***********************************************************************/

static void con24to16( LPRGB lpRGB, int iCount, LPWORD lpMiniRGB )
{
	while (iCount-- > 0)
	{
		*lpMiniRGB++ = RGBtoMiniRGB(lpRGB);
		lpRGB++;
	}
}

/***********************************************************************/

static void con32to16( LPCMYK lpCMYK, int iCount, LPWORD lpMiniRGB )
{
	RGBS rgb;

	while (iCount-- > 0)
	{
		rgb.red   = lpCMYK->c^0xFF;
		rgb.green = lpCMYK->m^0xFF;
		rgb.blue  = lpCMYK->y^0xFF;

		if (lpCMYK->k)
		{
			rgb.red   -= (rgb.red   * lpCMYK->k) >> 8;
			rgb.green -= (rgb.green * lpCMYK->k) >> 8;
			rgb.blue  -= (rgb.blue  * lpCMYK->k) >> 8;
		}

		*lpMiniRGB++ = RGBtoMiniRGB(&rgb);

		lpCMYK++;
	}
}

/***********************************************************************/

static void con16to1( LPWORD lpIn, int iCount, LPTR lpOut )
{
	register byte, mask;

	byte = 0;
	mask = 1<<7;

	while (iCount-- > 0)
	{
		if (MiniRGBtoL(*lpIn++) > THRESH)
			byte |= mask;

		if (mask & 1)
		{
			*lpOut++ = byte;
			byte = 0;
			mask = 1<<7;
		}
		else
		{
			mask >>= 1;
		}
	}

	if (mask != (1<<7))
		*lpOut++ = byte;
}

/***********************************************************************/

static void con16to8( LPWORD lpMiniRGB, int iCount, LPTR lpOut )
{
	while (iCount-- > 0)
	{
		*lpOut++ = MiniRGBtoL(*lpMiniRGB);
		lpMiniRGB++;
	}
}

/***********************************************************************/

static void con16to24( LPWORD lpMiniRGB, int iCount, LPRGB lpRGB )
{
	WORD wRGB;

	while (iCount-- > 0)
	{
		wRGB = *lpMiniRGB++;

		lpRGB->red   = MaxiR( wRGB );
		lpRGB->green = MaxiG( wRGB );
		lpRGB->blue  = MaxiB( wRGB );
		lpRGB++;
	}
}

/***********************************************************************/

static void con16to32( LPWORD lpMiniRGB, int iCount, LPCMYK lpCMYK )
{
	WORD wRGB;

	while (iCount-- > 0)
	{
		wRGB = *lpMiniRGB++;

		lpCMYK->c = MaxiR( wRGB )^0xFF;
		lpCMYK->m = MaxiG( wRGB )^0xFF;
		lpCMYK->y = MaxiB( wRGB )^0xFF;
		lpCMYK->k = 0;

		lpCMYK++;
	}
}
#endif



