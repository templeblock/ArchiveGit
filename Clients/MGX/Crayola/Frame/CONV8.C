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

// For 8 bit display drivers
//	Convert32to8    (LPCMYK,int,LPINT,LPTR,int,int,LPBLT);
//	Convert24to8    (LPRGB,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8to8     (LPTR,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8Pto8     (LPTR,int,LPINT,LPTR,int,int,LPBLT);
//	Convert32to8G   (LPCMYK,int,LPINT,LPTR,int,int,LPBLT);
//	Convert24to8G   (LPRGB,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8to8C    (LPTR,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8Pto8G    (LPTR,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8Pto8C    (LPTR,int,LPINT,LPTR,int,int,LPBLT);
//	Convert32to8CP    (LPCMYK,int,LPINT,LPTR,int,int,LPBLT);
//	Convert24to8CP    (LPRGB,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8to8CP     (LPTR,int,LPINT,LPTR,int,int,LPBLT);

// In the gray palette, there are 5 unused entries above the first 50 grays
// and there are 5 unused, 125 colors (and 50 grays) above the last 51 grays
#define A 5
#define B 130

// In the color palette, there are 2 unused entries above the first 8 grays
// and there are 2 unused, 216 colors (and 8 grays) above the last 8 grays
#define C 2
#define D 218

static BYTE Lut8to8C[16] = {
  0+C, 1+C,  2+C,  3+C,  4+C,  5+C,  6+C,  7+C,
  8+D, 9+D, 10+D, 11+D, 12+D, 13+D, 14+D, 15+D };

static BYTE Lut8to8[256] = { 0+A,
  0+A,  0+A,  0+A,  1+A,  1+A,  2+A,  2+A,  2+A,  3+A,  3+A,  4+A,  4+A,  4+A,
  5+A,  5+A,  6+A,  6+A,  6+A,  7+A,  7+A,  8+A,  8+A,  8+A,  9+A,  9+A, 10+A,
 10+A, 10+A, 11+A, 11+A, 12+A, 12+A, 12+A, 13+A, 13+A, 14+A, 14+A, 14+A, 15+A,
 15+A, 16+A, 16+A, 16+A, 17+A, 17+A, 18+A, 18+A, 18+A, 19+A, 19+A, 20+A, 20+A,
 20+A, 21+A, 21+A, 22+A, 22+A, 22+A, 23+A, 23+A, 24+A, 24+A, 24+A, 25+A, 25+A,
 26+A, 26+A, 26+A, 27+A, 27+A, 28+A, 28+A, 28+A, 29+A, 29+A, 30+A, 30+A, 30+A,
 31+A, 31+A, 32+A, 32+A, 32+A, 33+A, 33+A, 34+A, 34+A, 34+A, 35+A, 35+A, 36+A,
 36+A, 36+A, 37+A, 37+A, 38+A, 38+A, 38+A, 39+A, 39+A, 40+A, 40+A, 40+A, 41+A,
 41+A, 42+A, 42+A, 42+A, 43+A, 43+A, 44+A, 44+A, 44+A, 45+A, 45+A, 46+A, 46+A,
 46+A, 47+A, 47+A, 48+A, 48+A, 48+A, 49+A, 49+A, 50+B, 50+B, 50+B, 51+B, 51+B,
 52+B, 52+B, 52+B, 53+B, 53+B, 54+B, 54+B, 54+B, 55+B, 55+B, 56+B, 56+B, 56+B,
 57+B, 57+B, 58+B, 58+B, 58+B, 59+B, 59+B, 60+B, 60+B, 60+B, 61+B, 61+B, 62+B,
 62+B, 62+B, 63+B, 63+B, 64+B, 64+B, 64+B, 65+B, 65+B, 66+B, 66+B, 66+B, 67+B,
 67+B, 68+B, 68+B, 68+B, 69+B, 69+B, 70+B, 70+B, 70+B, 71+B, 71+B, 72+B, 72+B,
 72+B, 73+B, 73+B, 74+B, 74+B, 74+B, 75+B, 75+B, 76+B, 76+B, 76+B, 77+B, 77+B,
 78+B, 78+B, 78+B, 79+B, 79+B, 80+B, 80+B, 80+B, 81+B, 81+B, 82+B, 82+B, 82+B,
 83+B, 83+B, 84+B, 84+B, 84+B, 85+B, 85+B, 86+B, 86+B, 86+B, 87+B, 87+B, 88+B,
 88+B, 88+B, 89+B, 89+B, 90+B, 90+B, 90+B, 91+B, 91+B, 92+B, 92+B, 92+B, 93+B,
 93+B, 94+B, 94+B, 94+B, 95+B, 95+B, 96+B, 96+B, 96+B, 97+B, 97+B, 98+B, 98+B,
 98+B, 99+B, 99+B,100+B,100+B,100+B,100+B,100+B };

void Convert32to8(
	LPCMYK lpPixel,
	int    iCount,
	LPINT  lpError,
	LPTR   lpOutput,
	int    xDiva, 
	int    yDiva,
	LPBLTSESSION  lpBltSession)
{
	int *lpMatrix;
	int iMatrix;
	int r, g, b;
	STATIC16 int MyMatrix[16];

	for(iMatrix=0;iMatrix<16;iMatrix++)
	{
		MyMatrix[iMatrix] = Matrix[((yDiva&15)<<4) + ((iMatrix+xDiva)&15)]/5;
	}

	lpMatrix = &MyMatrix[0];
	iMatrix  = 0;

	while( --iCount >= 0 )
	{
		register p, v;
		int thresh, negthresh;

		// Get the threshold values from matrix
		thresh = lpMatrix[iMatrix++];
		if (iMatrix == 16) iMatrix = 0;

		negthresh = thresh - VALUE1;

		p = 2 + 8; // the starting index into the color palette

		RGB_FROM_CMYK(r, g, b, lpPixel->c, lpPixel->m, lpPixel->y, lpPixel->k );
		lpPixel++;

		// Cyan - Handle the cyan component
		v = r;

		HandleREDComponent(v, p);

		if (v > thresh)
			p += RED_1;
		else
		if (v < negthresh)
			p -= RED_1;

		// Magenta - Handle the Magenta component
		v = g;

		HandleGRNComponent(v, p);

		if (v > thresh)
			p += GRN_1;
		else
		if (v < negthresh)
			p -= GRN_1;

		// Yellow - Handle the Yellow component
		v = b;

		HandleBLUComponent(v, p);

		if (v > thresh)
			p += BLU_1;
		else
		if (v < negthresh)
			p -= BLU_1;

		*lpOutput++ = p; // Output the computed pixel
	}
}

/*=========================================================================*/

void Convert32to8G(
	LPCMYK lpPixel,
	int    iCount,
	LPINT  lpError,
	LPTR   lpOutput,
	int    xDiva, 
	int    yDiva,
	LPBLTSESSION  lpBltSession)
{
int thresh, negthresh;
LPTR lp, lpLast, lpFirst;
int red, green, blue;
register BYTE p;

lpFirst = Matrix + (16*(yDiva&15));
lpLast = lpFirst + 15;
lp = lpFirst + (xDiva&15);

red = green = blue = 0;
while ( --iCount >= 0 )
	{
	p = 5 + 50; // the starting index into the gray palette
	// Compute the accumulated pixel values

	RGB_FROM_CMYK(
		red,
		green,
		blue,
		lpPixel->c,
		lpPixel->m,
		lpPixel->y,
		lpPixel->k);

	++lpPixel;

	thresh = *lp / 4; // 0-255 maps to 0-51
	negthresh = thresh - 64; // 0-255 maps to (-64)-0
	if ( ++lp > lpLast )
		lp = lpFirst;

// RED - Handle the red component
	if ( red < 96 )
		{
		if ( red < 32 )         { /* p += 25*0; red -= 0; */ }
		else                    { p += 25*1; red -= 64; }
		}
	else    {
		if ( red < 160 )        { p += 25*2; red -= 128; }
		else if (red < 224)     { p += 25*3; red -= 192; }
		else                    { p += 25*4; red -= 255; }
		}

	if ( red > thresh )
		p += 25;
	else
	if ( red < negthresh )
		p -= 25;

// GREEN - Handle the green component
	if ( green < 96 )
		{
		if ( green < 32 )       { /* p += 5*0; green -= 0; */ }
		else                    { p += 5*1; green -= 64; }
		}
	else    {
		if ( green < 160 )      { p += 5*2; green -= 128; }
		else if (green < 224)   { p += 5*3; green -= 192; }
		else                    { p += 5*4; green -= 255; }
		}

	if ( green > thresh )
		p += 5;
	else
	if ( green < negthresh )
		p -= 5;

// BLUE - Handle the blue component
	if ( blue < 96 )
		{
		if ( blue < 32 )        { /* p += 0; blue -= 0; */ }
		     else               { p += 1; blue -= 64; }
		}
	else    {
		if ( blue < 160 )       { p += 2; blue -= 128; }
		else if ( blue < 224 )  { p += 3; blue -= 192; }
		     else               { p += 4; blue -= 255; }
		}

	if ( blue > thresh )
		p++;
	else
	if ( blue < negthresh )
		p--;

	*lpOutput++ = p; // Output the computed pixel
	}
}

/************************************************************************/
void Convert32to8CP(
	LPCMYK lpPixel,
	int    iCount,
	LPINT  lpError,
	LPTR   lpOutput,
	int    xDiva, 
	int    yDiva,
	LPBLTSESSION  lpBltSession)
/************************************************************************/
{
LPTR lpPaletteLUT;
WORD wRGB;
int red, green, blue;

lpPaletteLUT = lpBltSession->lpBlt->lpPaletteLUT;
while (--iCount >= 0)
	{
	RGB_FROM_CMYK(
		red,
		green,
		blue,
		lpPixel->c,
		lpPixel->m,
		lpPixel->y,
		lpPixel->k);
	++lpPixel;
	wRGB = RGB3toMiniRGB(red, green, blue);
	*lpOutput++ = lpPaletteLUT[wRGB];
	}
}

#ifdef C_CODE
/************************************************************************/
void Convert24to8( LPRGB lpPixel, int iCount, register LPINT lpError,
                        LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
int thresh, negthresh;
LPTR lp, lpLast, lpFirst;
int red, green, blue;
register BYTE p;

lpFirst = Matrix + (16*(yDiva&15));
lpLast = lpFirst + 15;
lp = lpFirst + (xDiva&15);

red = green = blue = 0;
while ( --iCount >= 0 )
	{
	p = 2 + 8; // the starting index into the color palette

	// Compute the accumulated pixel values
	red   = lpPixel->red;
	green = lpPixel->green;
	blue  = lpPixel->blue;
	lpPixel++;

	thresh = *lp / 5; // 0-255 maps to 0-51
	negthresh = thresh - 51; // 0-255 maps to (-51)-0
	if ( ++lp > lpLast )
		lp = lpFirst;

// RED - Handle the red component
	if ( red < 128 )
		{
		if ( red < 26 )         { /* p += 36*0; red -= 0; */ }
		else if ( red < 77 )    { p += 36*1; red -= 51; }
		     else               { p += 36*2; red -= 102; }
		}
	else
		{
		if ( red < 179 )        { p += 36*3; red -= 153; }
		else if ( red < 230 )   { p += 36*4; red -= 204; }
		     else               { p += 36*5; red -= 255; }
		}

	if ( red > thresh )
		p += 36;
	else
	if ( red < negthresh )
		p -= 36;

// GREEN - Handle the green component
	if ( green < 128 )
		{
		if ( green < 26 )       { /* p += 6*0; green -= 0; */ }
		else if ( green < 77 )  { p += 6*1; green -= 51; }
		     else               { p += 6*2; green -= 102; }
		}
	else
		{
		if ( green < 179 )      { p += 6*3; green -= 153; }
		else if ( green < 230 ) { p += 6*4; green -= 204; }
		     else               { p += 6*5; green -= 255; }
		}

	if ( green > thresh )
		p += 6;
	else
	if ( green < negthresh )
		p -= 6;

// BLUE - Handle the blue component
	if ( blue < 128 )
		{
		if ( blue < 26 )        { /* p += 0; blue -= 0; */ }
		else if ( blue < 77 )   { p += 1; blue -= 51; }
		     else               { p += 2; blue -= 102; }
		}
	else
		{
		if ( blue < 179 )       { p += 3; blue -= 153; }
		else if ( blue < 230 )  { p += 4; blue -= 204; }
		     else               { p += 5; blue -= 255; }
		}

	if ( blue > thresh )
		p++;
	else
	if ( blue < negthresh )
		p--;

	*lpOutput++ = p; // Output the computed pixel
	}
}
#endif

/************************************************************************/
void Convert24to8G( LPRGB lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                        int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
	int thresh, negthresh;
	LPTR lp, lpLast, lpFirst;
	int red, green, blue;
	BYTE p;

	lpFirst = Matrix + (16*(yDiva&15));
	lpLast = lpFirst + 15;
	lp = lpFirst + (xDiva&15);

	red = green = blue = 0;

	while ( --iCount >= 0 )
	{
		p = 5 + 50; // the starting index into the gray palette

		// Compute the accumulated pixel values
		red   = lpPixel->red;
		green = lpPixel->green;
		blue  = lpPixel->blue;
		++lpPixel;

		if ((red == green) && (green == blue))
		{
			*lpOutput++ = Lut8to8[red];
		}
		else
		{
			thresh = *lp / 4; // 0-255 maps to 0-51
			negthresh = thresh - 64; // 0-255 maps to (-64)-0
			if ( ++lp > lpLast )
				lp = lpFirst;

			// RED - Handle the red component
			if ( red < 96 )
			{
				if ( red < 32 )         { /* p += 25*0; red -= 0; */ }
				else                    { p += 25*1; red -= 64; }
			}
			else
			{
				if ( red < 160 )        { p += 25*2; red -= 128; }
				else if (red < 224)     { p += 25*3; red -= 192; }
				else                    { p += 25*4; red -= 255; }
			}

			if ( red > thresh )
				p += 25;
			else
			if ( red < negthresh )
				p -= 25;

			// GREEN - Handle the green component
			if ( green < 96 )
			{
				if ( green < 32 )       { /* p += 5*0; green -= 0; */ }
				else                    { p += 5*1; green -= 64; }
			}
			else
			{
				if ( green < 160 )      { p += 5*2; green -= 128; }
				else if (green < 224)   { p += 5*3; green -= 192; }
				else                    { p += 5*4; green -= 255; }
			}

			if ( green > thresh )
				p += 5;
			else
			if ( green < negthresh )
				p -= 5;

			// BLUE - Handle the blue component
			if ( blue < 96 )
			{
				if ( blue < 32 )        { /* p += 0; blue -= 0; */ }
		     		else               { p += 1; blue -= 64; }
			}
			else
			{
				if ( blue < 160 )       { p += 2; blue -= 128; }
				else if ( blue < 224 )  { p += 3; blue -= 192; }
		     		else               { p += 4; blue -= 255; }
			}

			if ( blue > thresh )
				p++;
			else
			if ( blue < negthresh )
				p--;

			*lpOutput++ = p; // Output the computed pixel
		}
	}
}

/************************************************************************/
void Convert24to8CP( LPRGB lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                        int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
LPTR lpPaletteLUT;
WORD wRGB;

lpPaletteLUT = lpBltSession->lpBlt->lpPaletteLUT;
while (--iCount >= 0)
	{
	wRGB = RGBtoMiniRGB(lpPixel);
	++lpPixel;
	*lpOutput++ = lpPaletteLUT[wRGB];
	}
}

#ifndef C_CODE
extern "C" {
extern void Convert8to8ASM( LPTR lpSrc, LPTR lpDst, LPTR lpLut, int iCount );
extern void Convert8to8ASM2( LPTR lpSrc, LPTR lpDst, LPTR lpLut, int iCount );
}
#endif // C_CODE

/************************************************************************/
void Convert8to8( LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                     int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	*lpOutput++ = Lut8to8[*lpPixel++];
#else // !C_CODE
	Convert8to8ASM( lpPixel, lpOutput, Lut8to8, iCount);
#endif // !C_CODE
}

/************************************************************************/
void Convert8to8C( LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                     int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
#ifdef C_CODE
while ( --iCount >= 0 )
	*lpOutput++ = Lut8to8C[(*lpPixel++)>>4];
#else // !C_CODE
	Convert8to8ASM2( lpPixel, lpOutput, Lut8to8C, iCount);
#endif // !C_CODE
}

/************************************************************************/
void Convert8to8CP( LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                     int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
LPTR lpPaletteLUT;
BYTE i;
WORD wRGB;

lpPaletteLUT = lpBltSession->lpBlt->lpPaletteLUT;
while (--iCount >= 0)
	{
	i = *lpPixel++;
	wRGB = GraytoMiniRGB(i);
	*lpOutput++ = lpPaletteLUT[wRGB];
	}
}

/************************************************************************/
void Convert8Pto8( LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                     int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
LPRGB lpRGBmap, lpRGB;
LPTR lpPaletteLUT;
BYTE i;
WORD wRGB;

lpRGBmap = lpBltSession->TypeInfo.ColorMap->RGBData;
lpPaletteLUT = lpBltSession->lpBlt->lpPaletteLUT;
while (--iCount >= 0)
	{
	i = *lpPixel++;
	lpRGB = lpRGBmap + i;
	wRGB = RGBtoMiniRGB(lpRGB);
	*lpOutput++ = lpPaletteLUT[wRGB];
	}
}

/************************************************************************/
void Convert8Pto8G( LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                     int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
	int thresh, negthresh;
	LPTR lp, lpLast, lpFirst;
	int red, green, blue;
	BYTE p;
	LPRGB lpRGBmap, lpRGB;
	BYTE i;

	lpRGBmap = lpBltSession->TypeInfo.ColorMap->RGBData;
	lpFirst = Matrix + (16*(yDiva&15));
	lpLast = lpFirst + 15;
	lp = lpFirst + (xDiva&15);

	red = green = blue = 0;

	while ( --iCount >= 0 )
	{
		p = 5 + 50; // the starting index into the gray palette

		i = *lpPixel++;
		lpRGB = lpRGBmap + i;

		// Compute the accumulated pixel values
		red   = lpRGB->red;
		green = lpRGB->green;
		blue  = lpRGB->blue;

		if ((red == green) && (green == blue))
		{
			*lpOutput++ = Lut8to8[red];
		}
		else
		{
			thresh = *lp / 4; // 0-255 maps to 0-51
			negthresh = thresh - 64; // 0-255 maps to (-64)-0
			if ( ++lp > lpLast )
				lp = lpFirst;

			// RED - Handle the red component
			if ( red < 96 )
			{
				if ( red < 32 )         { /* p += 25*0; red -= 0; */ }
				else                    { p += 25*1; red -= 64; }
			}
			else
			{
				if ( red < 160 )        { p += 25*2; red -= 128; }
				else if (red < 224)     { p += 25*3; red -= 192; }
				else                    { p += 25*4; red -= 255; }
			}

			if ( red > thresh )
				p += 25;
			else
			if ( red < negthresh )
				p -= 25;

			// GREEN - Handle the green component
			if ( green < 96 )
			{
				if ( green < 32 )       { /* p += 5*0; green -= 0; */ }
				else                    { p += 5*1; green -= 64; }
			}
			else
			{
				if ( green < 160 )      { p += 5*2; green -= 128; }
				else if (green < 224)   { p += 5*3; green -= 192; }
				else                    { p += 5*4; green -= 255; }
			}

			if ( green > thresh )
				p += 5;
			else
			if ( green < negthresh )
				p -= 5;

			// BLUE - Handle the blue component
			if ( blue < 96 )
			{
				if ( blue < 32 )        { /* p += 0; blue -= 0; */ }
		     		else               { p += 1; blue -= 64; }
			}
			else
			{
				if ( blue < 160 )       { p += 2; blue -= 128; }
				else if ( blue < 224 )  { p += 3; blue -= 192; }
		     		else               { p += 4; blue -= 255; }
			}

			if ( blue > thresh )
				p++;
			else
			if ( blue < negthresh )
				p--;

			*lpOutput++ = p; // Output the computed pixel
		}
	}
}

/************************************************************************/
void Convert8Pto8C( LPTR lpPixel, int iCount, LPINT lpError, LPTR lpOutput,
                     int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
int thresh, negthresh;
LPTR lp, lpLast, lpFirst;
int red, green, blue;
register BYTE p;
LPRGB lpRGBmap, lpRGB;
BYTE i;

lpRGBmap = lpBltSession->TypeInfo.ColorMap->RGBData;

lpFirst = Matrix + (16*(yDiva&15));
lpLast = lpFirst + 15;
lp = lpFirst + (xDiva&15);

red = green = blue = 0;
while ( --iCount >= 0 )
	{
	p = 2 + 8; // the starting index into the color palette

	// Compute the accumulated pixel values
	i = *lpPixel++;
	lpRGB = lpRGBmap + i;

	// Compute the accumulated pixel values
	red   = lpRGB->red;
	green = lpRGB->green;
	blue  = lpRGB->blue;

	thresh = *lp / 5; // 0-255 maps to 0-51
	negthresh = thresh - 51; // 0-255 maps to (-51)-0
	if ( ++lp > lpLast )
		lp = lpFirst;

// RED - Handle the red component
	if ( red < 128 )
		{
		if ( red < 26 )         { /* p += 36*0; red -= 0; */ }
		else if ( red < 77 )    { p += 36*1; red -= 51; }
		     else               { p += 36*2; red -= 102; }
		}
	else
		{
		if ( red < 179 )        { p += 36*3; red -= 153; }
		else if ( red < 230 )   { p += 36*4; red -= 204; }
		     else               { p += 36*5; red -= 255; }
		}

	if ( red > thresh )
		p += 36;
	else
	if ( red < negthresh )
		p -= 36;

// GREEN - Handle the green component
	if ( green < 128 )
		{
		if ( green < 26 )       { /* p += 6*0; green -= 0; */ }
		else if ( green < 77 )  { p += 6*1; green -= 51; }
		     else               { p += 6*2; green -= 102; }
		}
	else
		{
		if ( green < 179 )      { p += 6*3; green -= 153; }
		else if ( green < 230 ) { p += 6*4; green -= 204; }
		     else               { p += 6*5; green -= 255; }
		}

	if ( green > thresh )
		p += 6;
	else
	if ( green < negthresh )
		p -= 6;

// BLUE - Handle the blue component
	if ( blue < 128 )
		{
		if ( blue < 26 )        { /* p += 0; blue -= 0; */ }
		else if ( blue < 77 )   { p += 1; blue -= 51; }
		     else               { p += 2; blue -= 102; }
		}
	else
		{
		if ( blue < 179 )       { p += 3; blue -= 153; }
		else if ( blue < 230 )  { p += 4; blue -= 204; }
		     else               { p += 5; blue -= 255; }
		}

	if ( blue > thresh )
		p++;
	else
	if ( blue < negthresh )
		p--;

	*lpOutput++ = p; // Output the computed pixel
	}
}


