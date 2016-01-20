/*=======================================================================*\

	FRMCNVRT.C - Bitmap Depth Conversion Routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include "framelib.h"
#include "macros.h"

/*=========================================================================*/

#define THRESH 127

/*=========================================================================*/

void ConvertData(
	LPTR lpIn, 
	int  iInDepth,
	int  iCount,
	LPTR lpOut,
	int  iOutDepth )
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
				con1to8(lpIn, iCount, lpOut, NO);
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
				copy(lpIn, lpOut, iCount*2);
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
			copy(lpIn, lpOut, iCount*3);
			break;

			default:
			break;
		}
		break;

		default:
		break;
	}
}

/*=========================================================================*/

void con1to8(LPTR lpIn, int iCount, LPTR lpOut, int normal)
{
#ifdef C_CODE
	BYTE byte, bit, black, white;

	if ( normal )
	{
		white = 0;
		black = 255;
	}
	else
	{
		white = 255;
		black = 0;
	}

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
		else
			*lpOut++ = white;

		byte <<= 1;
	}

#else
__asm
{
	push	ds
	mov     cx, iCount
	or      cx, cx
	je      la2con_end
	lds     si, lpIn
	les     di, lpOut
	cld
	mov     bl, 1           ;bl = bit
	cmp     normal, 0
	je      la2con_1
	mov     dl, 0           ;dl = white
	mov     dh, 255         ;dh = black
	jmp     la2con_loop
la2con_1:
	mov     dl, 255         ;dl = white
	mov     dh, 0           ;dh = black

la2con_loop:
	dec     bl              ;bit
	jne     la2con_2
	lodsb                   ;ax=*lpIn++
	mov     bh, al          ;bh=byte
	mov     bl, 8           ;bl=bit=8
la2con_2:
	cmp     bh, 127         ;byte
	jbe     la2con_3
	mov     al, dh          ;black
	jmp     la2con_4
la2con_3:
	mov     al, dl          ;white
la2con_4:
	stosb
	shl     bh, 1           ;byte
	loop    la2con_loop
la2con_end:
	pop		ds
}
#endif
}

/*=========================================================================*/

void con1to16(LPTR lpIn, int iCount, LPWORD lpOut)
{
	BYTE byte, bit;
	WORD black, white;
	BOOL normal = NO;

	if (normal)
	{
		white = 0;
		black = 0x7FFF;
	}
	else
	{
		white = 0x7FFF;
		black = 0;
	}

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
		else
			*lpOut++ = white;

		byte <<= 1;
	}
}

/*=========================================================================*/

void con1to24(LPTR lpIn, int iCount, LPRGB lpOut)
{
	BYTE bit, byte;
	BYTE black, white;
	BOOL normal = NO;

	if (normal)
	{
		white = 0;
		black = 255;
	}
	else
	{
		white = 255;
		black = 0;
	}

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
			lpOut->red   = black;
			lpOut->green = black;
			lpOut->blue  = black;
		}
		else	
		{
			lpOut->red   = white;
			lpOut->green = white;
			lpOut->blue  = white;
		}
		++lpOut;
		byte <<= 1;
	}
}

/*=========================================================================*/

void con8to1(LPTR lpIn, int iCount, LPTR lpOut)
{
#ifdef C_CODE
	BYTE byte, bit;

	byte = 0;
	bit  = 128;

	while ( --iCount >= 0 )
	{
		if (*lpIn++ <= THRESH)
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
#else
__asm
{
	push	ds
	mov     cx, iCount
	or      cx, cx
	je      con2la_end
	lds     si, lpIn
	les     di, lpOut
	cld
	mov     bl, 128         ; bit
	mov     bh, 0           ; byte

con2la_loop:
	lodsb                   ; al = *lpIn++
	cmp     al, 127
	jna     con2la_1
	or      bh, bl          ; byte |= bit
con2la_1:
	shr     bl, 1           ; bit
	jne     con2la_2
	mov     al, bh
	stosb
	mov     bl, 128         ; bit
	mov     bh, 0           ; byte
con2la_2:
	loop    con2la_loop

	cmp     bl, 128         ; bit
	je      con2la_end
	mov     al, bh
	stosb
con2la_end:
	pop		ds
}
#endif
}

/*=========================================================================*/

void con8to16(LPTR lpIn, int iCount, LPWORD lpOut)
{
	RGBS rgb;

	while (--iCount >= 0)
	{
		rgb.red   = 
		rgb.green = 
		rgb.blue  = *lpIn++;

		*lpOut++ = RGBtoMiniRGB(&rgb);
	}
}

/*=========================================================================*/

void con8to24(LPTR lpIn, int iCount, LPRGB lpOut)
{
	BYTE lum;

	while (--iCount >= 0)
	{
		lum = *lpIn++;

		lpOut->red   = lum;
		lpOut->green = lum;
		lpOut->blue  = lum;

		++lpOut;
	}
}

/*=========================================================================*/

void con16to1(LPWORD lpIn, int iCount, LPTR lpOut)
{
	BYTE byte, bit, lum;

	byte = 0;
	bit = 128;

	while ( --iCount >= 0 )
	{
		lum = MiniRGBtoL(*lpIn++);

		if (lum <= THRESH)
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

/*=========================================================================*/

void con16to8(LPWORD lpMiniRGB, int iCount, LPTR lpOut)
{
	while (--iCount >= 0)
	{
		*lpOut = MiniRGBtoL(*lpMiniRGB);
		++lpMiniRGB;
		++lpOut;
	}
}

/*=========================================================================*/

void con16to24(LPWORD lpMiniRGB, int iCount, LPRGB lpRGB)
{
	WORD wRGB;

	while ( --iCount >= 0 )
	{
		wRGB = *lpMiniRGB++;

		lpRGB->red   = MaxiR( wRGB );
		lpRGB->green = MaxiG( wRGB );
		lpRGB->blue  = MaxiB( wRGB );

		++lpRGB;
	}
}

/*=========================================================================*/

void con24to1(LPRGB lpIn, int iCount, LPTR lpOut)
{
	BYTE byte, bit, lum;

	byte = 0;
	bit = 128;

	while ( --iCount >= 0 )
	{
		lum = (((int)lpIn->red * 30) + 
		      ((int)lpIn->green * 59) + 
		      ((int)lpIn->blue * 11))/100;

		++lpIn;

		if (lum <= THRESH)
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

/*=========================================================================*/

void con24to8(LPRGB lpIn, int iCount, LPTR lpOut)
{
	while (--iCount >= 0)
	{

		*lpOut = (((int)lpIn->red * 30) + 
		         ((int)lpIn->green * 59) + 
		         ((int)lpIn->blue * 11))/100;

		++lpIn;
		++lpOut;
	}
}

/*=========================================================================*/

void con24to16(LPRGB lpRGB, int iCount, LPWORD lpMiniRGB)
{
	while (--iCount >= 0)
	{
		*lpMiniRGB++ = RGBtoMiniRGB(lpRGB);
		++lpRGB;
	}
}

/*=========================================================================*/

#define MAXVALUE   255
#define CLUMP      1
#define DECISION_X ((x++ % CLUMP) == 0)

void diffuse(
	int  x,
	int  y,
	int  iColor,
	LPTR lpLut,
	LPTR lpIn,
	int  iCount,
	LPTR lpOut)
{
	BYTE  byte, bit;
	LPINT lpError;
	int   pixel, iErrorCount;

	static LPTR  lpDiffuseMemory;
	static int   iDiffuseCount;
	static ULONG lSeed = 0xDEADBABA;

	iCount++; /* need 1 additional slot for errors */
	iErrorCount =iCount * ( iColor ? (4*sizeof(int)) : sizeof(int) );

	if ( lpDiffuseMemory && (iErrorCount != iDiffuseCount) )
	{
		FreeUp( lpDiffuseMemory );
		lpDiffuseMemory = NULL;
	}

	if ( !lpDiffuseMemory )
	{
		iDiffuseCount = iErrorCount;
		if ( !(lpDiffuseMemory = Alloc( iDiffuseCount )) )
			return;
		clr( lpDiffuseMemory, iDiffuseCount );
	}
	else
	{
		/* if this the the start of the image, clear the error buffer */
		if ( y == 0 )
			clr( lpDiffuseMemory, iDiffuseCount );
	}

	lpError = (LPINT)lpDiffuseMemory;

	if ( iColor )
		lpError += (iCount * (iColor-1));

	byte = 0;
	bit = 128;

	while (--iCount > 0)
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
			*lpOut++ = byte;
			byte = 0;
			bit = 128;
		}
	}

	if ( bit != 128 )
		*lpOut++ = byte;
}

/*=========================================================================*/

static BYTE Pattern[] =
{
	112, 96,104,120,143,159,151,135, 
	 72,  0,  8, 32,183,254,247,223, 
	 80, 16, 24, 40,175,239,231,215, 
	 88, 48, 56, 64,167,207,199,191, 
	139,155,147,131,116,100,108,124, 
	179,254,243,219, 76,  0, 12, 36, 
	171,235,227,211, 84, 20, 28, 44, 
	163,203,195,187, 92, 52, 60, 68
};

void dither(
	int  x,
	int  y,
	LPTR lpIn,
	int  iCount,
	LPTR lpOut)
{
	BYTE	byte, bit;
	PTR	 pPat, pEndPat, pStartPat;

	pStartPat = Pattern   + 8*(y&7);
	pEndPat   = pStartPat + 8;
	pPat	  = pStartPat + (x&7);

	byte = 0;
	bit = 128;

	while (--iCount >= 0)
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

/*=========================================================================*/

