// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/

#ifndef WIN32
#include <windows.h>
#endif

#include "superblt.h"
#include "convert.h"

// The following converter are located here.
// Some have been converted to assembler and are #ifdef'd out

// For 4 bit display drivers
//	Convert32to4    (LPRGB,int,LPINT,LPTR,int,int,LPBLT);
//	Convert24to4    (LPRGB,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8to4     (LPTR,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8Pto4    (LPTR,int,LPINT,LPTR,int,int,LPBLT);

// For printer drivers
// Scattered
//	Convert32to4S   (LPCMYK,int,LPINT,LPTR,int,int,LPBLT); 
//	Convert24to4S   (LPRGB,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8to4S    (LPTR,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8Pto4S    (LPTR,int,LPINT,LPTR,int,int,LPBLT);

// Mononochrome Scattered
//	Convert32to4MS  (LPCMYK,int,LPINT,LPTR,int,int,LPBLT);
//	Convert24to4MS  (LPRGB,int,LPINT,LPTR,int,int,LPBLT);

// Roll our own Halftoning
//	Convert32to4MHT  (LPCMYK,int,LPINT,LPTR,int,int,LPBLT);
//	Convert24to4MHT  (LPRGB,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8to4HT    (LPTR,int,LPINT,LPTR,int,int,LPBLT);
//	Convert8Pto4MHT    (LPTR,int,LPINT,LPTR,int,int,LPBLT);

#define C_WHITE		15
#define C_BLACK		0
#define C_RED		9
#define C_BLUE		12
#define C_GREEN		10
#define C_CYAN		14
#define C_MAGENTA	13
#define C_YELLOW	11

static DWORD lSeed = 0xDEADBABA;

/*=========================================================================*/

#define RED_HI 0x90
#define GRN_HI 0xA0
#define BLU_HI 0xC0

#define RED_LO 0x09
#define GRN_LO 0x0A
#define BLU_LO 0x0C

void Convert32to4(
	LPCMYK lpPixel,
	int    iCount,
	LPINT  lpError,
	LPTR   lpOutput,
	int    xDiva, 
	int    yDiva,
	LPBLTSESSION  lpBltSession)
{
	LPTR lpSrc = (LPTR)lpPixel;
	LPTR lpMatrix;
	int  iMyCount = (iCount+1)/2;
	int  iMatrix;
	int  thresh;
	int  r, g, b;
	int  p;

	lpMatrix = Matrix + ((yDiva&15)*16);
	iMatrix  = (xDiva&15);

	while (iMyCount-- > 0)
	{
		p = 0;

		// Compute first nybble
		thresh = lpMatrix[(iMatrix++)&15];

		RGB_FROM_CMYK(r, g, b, lpPixel->c, lpPixel->m, lpPixel->y, lpPixel->k );
		lpPixel++;

		if (r >= thresh) p |= RED_HI;
		if (g >= thresh) p |= GRN_HI;
		if (b >= thresh) p |= BLU_HI;

		// Compute second nybble
		thresh = lpMatrix[(iMatrix++)&15];

		RGB_FROM_CMYK(r, g, b, lpPixel->c, lpPixel->m, lpPixel->y, lpPixel->k );
		lpPixel++;

		if (r >= thresh) p |= RED_LO;
		if (g >= thresh) p |= GRN_LO;
		if (b >= thresh) p |= BLU_LO;

		*lpOutput++ = p;
	}
}

/*=========================================================================*/

#define C_WHITE         15
#define C_BLACK         0
#define C_RED           9
#define C_BLUE          12
#define C_GREEN         10
#define C_CYAN          14
#define C_MAGENTA       13
#define C_YELLOW        11

#ifdef SCATTERED

void Convert32to4(
	LPCMYK lpPixel,
	int    iCount,
	LPINT  lpError,
	LPTR   lpOutput,
	int    xDiva, 
	int    yDiva,
	LPBLTSESSION  lpBltSession)
{
int iRed, iGrn, iBlu;
int red, green, blue;
register BYTE p, op;
BOOL IsOdd;

sOdd = NO;
red = green = blue = 0;
while ( --iCount >= 0 )
	{
	// Compute the accumulated pixel values
	RGB_FROM_CMYK(
		iRed,
		iGrn,
		iBlu,
		lpPixel->c,
		lpPixel->m,
		lpPixel->y,
		lpPixel->k);
	lpPixel++;

	red   += (*(lpError+0) + iRed);
	green += (*(lpError+1) + iGrn);
	blue  += (*(lpError+2) + iBlu);

	if ( red < 64 && green < 64 && blue < 64 )
		{ // if all < 64 then it is (low intensity) black
		p = 0; // 0,0,0
		}
	else // at least one is >= 64
	if ( red < 192 && green < 192 && blue < 192 )
		{ // if all < 192 but something >= 64, then low intensity
		p = 0;
		if ( red   >=  64 ) { p += 1; red   -= 128; }
		if ( green >=  64 ) { p += 2; green -= 128; }
		if ( blue  >=  64 ) { p += 4; blue  -= 128; }
//		if ( p == 7 ) // Only necessary on EGA - VGA it's OK
//			{ p = 7; red -= 128; green -= 128; blue -= 128; }
		}
	else    { // if something >= 192, then high intensity
		p = 8; // intensity is on
		if ( red   >= 192 ) { p += 1; red   -= 255; }
		if ( green >= 192 ) { p += 2; green -= 255; }
		if ( blue  >= 192 ) { p += 4; blue  -= 255; }
//		if ( p == 8 ) // Only necessary on EGA - VGA it's OK
//			{ p = 7; red -= 128; green -= 128; blue -= 128; }
		}

	if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0; // Pass nothing down (leave it for next loop)
		*lpError++ = 0;
		*lpError++ = 0;
		}
	else    { // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = red;
		*lpError++ = green;
		*lpError++ = blue;
		red = green = blue = 0;
		}

	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else    {
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}
#endif

/*=========================================================================*/

void Convert32to4S(
	LPCMYK lpPixel,
	int    iCount,
	LPINT  lpError,
	LPTR   lpOutput,
	int    xDiva, 
	int    yDiva,
	LPBLTSESSION  lpBltSession)
{
int iRed, iGrn, iBlu;
int red, green, blue;
register BYTE p, op;
BOOL IsOdd;

IsOdd = NO;
red = green = blue = 0;
while ( --iCount >= 0 )
    {
	// Compute the accumulated pixel values
	RGB_FROM_CMYK(
		iRed,
		iGrn,
		iBlu,
		lpPixel->c,
		lpPixel->m,
		lpPixel->y,
		lpPixel->k);
	lpPixel++;

	red   += (*(lpError+0) + iRed);
	green += (*(lpError+1) + iGrn);
	blue  += (*(lpError+2) + iBlu);

    if (red >= THRESH)
	{
	if (green >= THRESH)
	    {
	    if (blue >= THRESH)
		{
		red -= 255;
		green -= 255;
		blue -= 255;
		p = C_WHITE;
		}
	    else
		{
		red -= 255;
		green -= 255;
		p = C_YELLOW;
		}
	    }
	else if (blue >= THRESH)
	    {
	    red -= 255;
	    blue -= 255;
	    p = C_MAGENTA;
	    }
	else
	    {
	    red -= 255;
	    p = C_RED;
	    }
	}
    else if (green >= THRESH)
	{
	if (blue >= THRESH)
	    {
	    green -= 255;
	    blue -= 255;
	    p = C_CYAN;
	    }
	else
	    {
	    green -= 255;
	    p = C_GREEN;
	    }
	}
    else if (blue >= THRESH)
	{
	blue -= 255;
	p = C_BLUE;
	}
    else
	{
	p = C_BLACK;
	}

    if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0; // red error
		*lpError++ = 0; // green error
		*lpError++ = 0; // blue error
		}
    else        { // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = red;       // red error
		*lpError++ = green;     // green error
		*lpError++ = blue;      // blue error
		red = 0;
		green = 0;
		blue = 0;
		}
    if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else    {
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}

/*=========================================================================*/

void Convert32to4MS(
	LPCMYK lpPixel,
	int    iCount,
	LPINT  lpError,
	LPTR   lpOutput,
	int    xDiva, 
	int    yDiva,
	LPBLTSESSION  lpBltSession)
{
int gray;
BYTE pixel;
int iRed, iGrn, iBlu;
register BYTE p, op;
BOOL IsOdd;

IsOdd = NO;
gray = 0;
while ( --iCount >= 0 )
	{
	// Compute the accumulated pixel values
	RGB_FROM_CMYK(
		iRed,
		iGrn,
		iBlu,
		lpPixel->c,
		lpPixel->m,
		lpPixel->y,
		lpPixel->k);
	lpPixel++;

	pixel = TOLUM(iRed, iGrn, iBlu);
	gray += (*lpError + pixel);

	if (gray >= THRESH)
		{
		p = C_WHITE;
		gray -= 255;
		}
	else
		{
		p = C_BLACK;
		}

	if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0;
		}
	else    { // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = gray;
		gray = 0;
		}

	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else    {
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}

/*=========================================================================*/

void Convert32to4MHT(
	LPCMYK lpPixel,
	int    iCount,
	LPINT  lpError,
	LPTR   lpOutput,
	int    xDiva, 
	int    yDiva,
	LPBLTSESSION  lpBltSession)
{
PTR pPat, pEndPat, pStartPat;
register BYTE p, op, pixel;
int iRed, iGrn, iBlu;
BOOL IsOdd;

IsOdd = NO;
pStartPat = Pattern   + 8*(yDiva&7);
pEndPat   = pStartPat + 8;
pPat      = pStartPat + (xDiva&7);
while ( --iCount >= 0 )
	{
	// Compute the accumulated pixel values
	RGB_FROM_CMYK(
		iRed,
		iGrn,
		iBlu,
		lpPixel->c,
		lpPixel->m,
		lpPixel->y,
		lpPixel->k);
	lpPixel++;

	pixel = TOLUM(iRed, iGrn, iBlu);
	if (pixel > *pPat++)
		p = C_WHITE;
	else
		p = C_BLACK;
	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else    {
		op = p << 4;
		IsOdd = YES;
		}
	if ( pPat >= pEndPat )
		pPat = pStartPat;
	}

if ( IsOdd )
	*lpOutput = op;
}

/*=========================================================================*/

#ifdef C_CODE
/************************************************************************/
void Convert24to4( LPRGB lpPixel, int iCount, register LPINT lpError,
                     LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
LPTR lpFirst, lpLast, lpMatrix;
int red, green, blue;
register int thresh;
register BYTE p, op;
BOOL IsOdd;

lpFirst = Matrix + ((yDiva&15)*16);
lpLast = lpFirst + 15;
lpMatrix = lpFirst + (xDiva&15);
IsOdd = NO;
while (--iCount >= 0)
    {
    red   = lpPixel->red;
    green = lpPixel->green;
    blue  = lpPixel->blue;
    ++lpPixel;

    thresh = *lpMatrix++;
    if (lpMatrix > lpLast)
        lpMatrix = lpFirst;

    if (red >= thresh)
        {
        if (green >= thresh)
            {
            if (blue >= thresh)
                p = C_WHITE;
            else
                p = C_YELLOW;
            }
        else
		if (blue >= thresh)
            p = C_MAGENTA;
        else
            p = C_RED;
        }
    else
	if (green >= thresh)
        {
        if (blue >= thresh)
            p = C_CYAN;
        else
            p = C_GREEN;
        }
    else
	if (blue >= thresh)
        p = C_BLUE;
    else
        p = C_BLACK;
    if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
    else
		{
		op = p << 4;
		IsOdd = YES;
		}
    }

if ( IsOdd )
    *lpOutput = op;
}
#endif

/************************************************************************/
void Convert8to4( LPTR lpPixel, int iCount, register LPINT lpError,
                     LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
LPTR lpFirst, lpLast, lpMatrix;
int gray;
register int thresh, negthresh;
register BYTE p, op;
BOOL IsOdd;

lpFirst = Matrix + ((yDiva&15)*16);
lpLast = lpFirst + 15;
lpMatrix = lpFirst + (xDiva&15);
IsOdd = NO;
while (--iCount >= 0)
    {
    gray = *lpPixel++;
    thresh = *lpMatrix++; // 0-255 maps to 0-127
    if (lpMatrix > lpLast)
        lpMatrix = lpFirst;

    if ( gray >= 160 )
		{
		if ( gray >= 224 )
		    {
		    p = 15; gray -= 255;
		    negthresh = (thresh>>2) - 63;
		    if (gray < negthresh)
				p = 7;
		    }
		else
		    {
		    thresh >>= 2;
		    p = 7; gray -= 192;
		    if (gray > thresh)
				p = 15;
		    else
				{
				negthresh = thresh - 63;
				if (gray < negthresh)
					p = 8;
				}
		    }
		}
    else
		{
		if ( gray >= 64 )
		    {
		    thresh >>= 1;
		    negthresh = thresh - 127;
		    thresh >>= 1;
		    p = 8; gray -= 128;
		    if (gray > thresh)
				p = 7;
		    else
			if (gray < negthresh)
			    p = 0;
		    }
		else
		    {
		    thresh >>= 1;
		    p = 0;
		    if (gray > thresh)
				p = 8;
		    }
		}

    if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
    else
		{
		op = p << 4;
		IsOdd = YES;
		}
    }

if ( IsOdd )
    *lpOutput = op;
}

#ifdef UNUSED
/************************************************************************/
void Convert8to4( LPTR lpPixel, int iCount, register LPINT lpError,
                     LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
int gray;
BYTE pixel;
register BYTE p, op;
BOOL IsOdd;

IsOdd = NO;
gray = 0;
while ( --iCount >= 0 )
	{
	// Compute the accumulated pixel values
	pixel = *lpPixel++;
	gray += (*lpError + pixel);

	if ( gray >= 160 )
		{
		if ( gray >= 224 )      { p = 15; gray -= 255; }
		else                    { p = 7; gray -= 192; }
		}
	else
		{
		if ( gray >= 64 )       { p = 8; gray -= 128; }
		else                    { p = 0; }
		}

	if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0;
		}
	else
		{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = gray;
		gray = 0;
		}

	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else
		{
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}
#endif

#ifdef SCATTERED
/************************************************************************/
void Convert24to4( LPRGB lpPixel, int iCount, register LPINT lpError,
                     LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
int red, green, blue;
register BYTE p, op;
BOOL IsOdd;

IsOdd = NO;
red = green = blue = 0;
while ( --iCount >= 0 )
	{
	// Compute the accumulated pixel values
	red   += (*lpError++ + lpPixel->red);
	green += (*lpError++ + lpPixel->green);
	blue  += (*lpError   + lpPixel->blue);
	lpError -= 2;
	lpPixel++;

	if ( red < 64 && green < 64 && blue < 64 )
		{ // if all < 64 then it is (low intensity) black
		p = 0; // 0,0,0
		}
	else // at least one is >= 64
	if ( red < 192 && green < 192 && blue < 192 )
		{ // if all < 192 but something >= 64, then low intensity
		p = 0;
		if ( red   >=  64 ) { p += 1; red   -= 128; }
		if ( green >=  64 ) { p += 2; green -= 128; }
		if ( blue  >=  64 ) { p += 4; blue  -= 128; }
//		if ( p == 7 ) // Only necessary on EGA - VGA it's OK
//			{ p = 7; red -= 128; green -= 128; blue -= 128; }
		}
	else
		{ // if something >= 192, then high intensity
		p = 8; // intensity is on
		if ( red   >= 192 ) { p += 1; red   -= 255; }
		if ( green >= 192 ) { p += 2; green -= 255; }
		if ( blue  >= 192 ) { p += 4; blue  -= 255; }
//		if ( p == 8 ) // Only necessary on EGA - VGA it's OK
//			{ p = 7; red -= 128; green -= 128; blue -= 128; }
		}

	if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0; // Pass nothing down (leave it for next loop)
		*lpError++ = 0;
		*lpError++ = 0;
		}
	else
		{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = red;
		*lpError++ = green;
		*lpError++ = blue;
		red = green = blue = 0;
		}

	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else
		{
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}
#endif

/************************************************************************/
void Convert24to4S( LPRGB lpPixel, int iCount, register LPINT lpError,
                        LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
int red, green, blue;
register BYTE p, op;
BOOL IsOdd;

IsOdd = NO;
red = green = blue = 0;
while ( --iCount >= 0 )
    {
	// Compute the accumulated pixel values
    red += (*(lpError+0) + lpPixel->red);
    green += (*(lpError+1) + lpPixel->green);
    blue += (*(lpError+2) + lpPixel->blue);
    ++lpPixel;

    if (red >= THRESH)
		{
		if (green >= THRESH)
		    {
		    if (blue >= THRESH)
				{
				red -= 255;
				green -= 255;
				blue -= 255;
				p = C_WHITE;
				}
		    else
				{
				red -= 255;
				green -= 255;
				p = C_YELLOW;
				}
		    }
		else
		if (blue >= THRESH)
		    {
		    red -= 255;
		    blue -= 255;
		    p = C_MAGENTA;
		    }
		else
		    {
		    red -= 255;
		    p = C_RED;
		    }
		}
    else
	if (green >= THRESH)
		{
		if (blue >= THRESH)
		    {
		    green -= 255;
		    blue -= 255;
		    p = C_CYAN;
		    }
		else
		    {
		    green -= 255;
		    p = C_GREEN;
		    }
		}
	else
	if (blue >= THRESH)
		{
		blue -= 255;
		p = C_BLUE;
		}
    else
		{
		p = C_BLACK;
		}

    if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0; // red error
		*lpError++ = 0; // green error
		*lpError++ = 0; // blue error
		}
    else
		{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = red;       // red error
		*lpError++ = green;     // green error
		*lpError++ = blue;      // blue error
		red = 0;
		green = 0;
		blue = 0;
		}
    if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else
		{
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}

/************************************************************************/
void Convert8to4S( LPTR lpPixel, int iCount, register LPINT lpError,
                        LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
int gray;
BYTE pixel;
register BYTE p, op;
BOOL IsOdd;

IsOdd = NO;
gray = 0;
while ( --iCount >= 0 )
	{
	// Compute the accumulated pixel values
	pixel = *lpPixel++;
	gray += (*lpError + pixel);

	if (gray >= THRESH)
		{
		p = C_WHITE;
		gray -= 255;
		}
	else
		{
		p = C_BLACK;
		}

	if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0;
		}
	else
		{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = gray;
		gray = 0;
		}

	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else
		{
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}

/************************************************************************/
void Convert24to4MS( LPRGB lpPixel, int iCount, register LPINT lpError,
                           LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
int gray;
BYTE pixel;
register BYTE p, op;
BOOL IsOdd;

IsOdd = NO;
gray = 0;
while ( --iCount >= 0 )
	{
	// Compute the accumulated pixel values
	pixel = RGBtoL(lpPixel);
	++lpPixel;
	gray += (*lpError + pixel);

	if (gray >= THRESH)
		{
		p = C_WHITE;
		gray -= 255;
		}
	else
		{
		p = C_BLACK;
		}

	if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0;
		}
	else
		{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = gray;
		gray = 0;
		}

	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else
		{
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}

/************************************************************************/
void Convert8to4HT( LPTR lpPixel, int iCount, register LPINT lpError,
                        LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
PTR pPat, pEndPat, pStartPat;
register BYTE p, op;
BOOL IsOdd;

IsOdd = NO;
pStartPat = Pattern   + 8*(yDiva&7);
pEndPat   = pStartPat + 8;
pPat      = pStartPat + (xDiva&7);
while ( --iCount >= 0 )
	{
	if (*lpPixel++ > *pPat++)
		p = C_WHITE;
	else
		p = C_BLACK;
	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else
		{
		op = p << 4;
		IsOdd = YES;
		}
	if ( pPat >= pEndPat )
		pPat = pStartPat;
	}

if ( IsOdd )
	*lpOutput = op;
}

/************************************************************************/
void Convert24to4MHT( LPRGB lpPixel, int iCount, register LPINT lpError,
                        LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
PTR pPat, pEndPat, pStartPat;
register BYTE p, op, pixel;
BOOL IsOdd;

IsOdd = NO;
pStartPat = Pattern   + 8*(yDiva&7);
pEndPat   = pStartPat + 8;
pPat      = pStartPat + (xDiva&7);
while ( --iCount >= 0 )
	{
	pixel = RGBtoL(lpPixel);
	++lpPixel;
	if (pixel > *pPat++)
		p = C_WHITE;
	else
		p = C_BLACK;
	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else
		{
		op = p << 4;
		IsOdd = YES;
		}
	if ( pPat >= pEndPat )
		pPat = pStartPat;
	}

if ( IsOdd )
	*lpOutput = op;
}

#ifdef UNUSED

// 24 to 8 Method 1

red = green = blue = 0;
while ( --iCount >= 0 )
	{
	p = 0;

	// Compute the accumulated pixel values
	red   += (*lpError++ + lpPixel->red);
	green += (*lpError++ + lpPixel->green);
	blue  += (*lpError   + lpPixel->blue);
	lpError -= 2;
	lpPixel++;

// RED - Handle the red component
	if ( red < 128 )
		{
		if ( red < 26 )         { /* p += 36*0; red -= 0; */ }
		else if ( red < 77 )    { p += 36*1; red -= 51; }
		     else               { p += 36*2; red -= 102; }
		}
	else    {
		if ( red < 179 )        { p += 36*3; red -= 153; }
		else if ( red < 230 )   { p += 36*4; red -= 204; }
		     else               { p += 36*5; red -= 255; }
		}

// GREEN - Handle the green component
	if ( green < 128 )
		{
		if ( green < 26 )       { /* p += 6*0; green -= 0; */ }
		else if ( green < 77 )  { p += 6*1; green -= 51; }
		     else               { p += 6*2; green -= 102; }
		}
	else    {
		if ( green < 179 )      { p += 6*3; green -= 153; }
		else if ( green < 230 ) { p += 6*4; green -= 204; }
		     else               { p += 6*5; green -= 255; }
		}

// BLUE - Handle the blue component
	if ( blue < 128 )
		{
		if ( blue < 26 )        { /* p += 0; blue -= 0; */ }
		else if ( blue < 77 )   { p += 1; blue -= 51; }
		     else               { p += 2; blue -= 102; }
		}
	else    {
		if ( blue < 179 )       { p += 3; blue -= 153; }
		else if ( blue < 230 )  { p += 4; blue -= 204; }
		     else               { p += 5; blue -= 255; }
		}

	if ( lSeed & BIT18 )
		{ // Pass the error right
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0; // Pass nothing down (leave it for next loop)
		*lpError++ = 0;
		*lpError++ = 0;
		}
	else
		{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = red;
		*lpError++ = green;
		*lpError++ = blue;
		red = green = blue = 0;
		}

// 24 to 4 method 1

	if ( TOLUM(red,green,blue) >= THRESH )
		{ // Bright colors
		p = 8;
		if ( red   > 128 ) { p += 1; red   -= 255; }
		if ( green > 128 ) { p += 2; green -= 255; }
		if ( blue  > 128 ) { p += 4; blue  -= 255; }
		if ( p == 8 ) // this is really 128,128,128
			{ red += 128; green += 128; blue += 128; }
		}
	else
		{ // Dark colors issue 0 or 128 to guns
		p = 0;
		if ( red   >  64 ) { p += 1; red   -= 128; }
		if ( green >  64 ) { p += 2; green -= 128; }
		if ( blue  >  64 ) { p += 4; blue  -= 128; }
		if ( p == 7 ) // that was 192,192,192
			{ red -= 64; green -= 64; blue -= 64; }
		}

// 24 to 4 method 2

	if ( red < 64 && green < 64 && blue < 64 )
		{ // if all < 64 then it is (low intensity) black
		p = 0; // 0,0,0
		}
	else // at least one is >= 64
	if ( red < 192 && green < 192 && blue < 192 )
		{ // if all < 192 but something >= 64, then low intensity
		p = 0;
		if ( red   >=  64 ) { p += 1; red   -= 128; }
		if ( green >=  64 ) { p += 2; green -= 128; }
		if ( blue  >=  64 ) { p += 4; blue  -= 128; }
//		if ( p == 7 ) // Only necessary on EGA - VGA it's OK
//			{ p = 7; red -= 128; green -= 128; blue -= 128; }
		}
	else
		{ // if something >= 192, then high intensity
		p = 8; // intensity is on
		if ( red   >= 192 ) { p += 1; red   -= 255; }
		if ( green >= 192 ) { p += 2; green -= 255; }
		if ( blue  >= 192 ) { p += 4; blue  -= 255; }
//		if ( p == 8 ) // Only necessary on EGA - VGA it's OK
//			{ p = 7; red -= 128; green -= 128; blue -= 128; }
		}

// 24 to 4 method 3

	if ( red < 32 && green < 32 && blue < 32 )
		{ // if all < 32 then it is (low intensity) black
		p = 0; // 0,0,0
		}
	else // at least one is >= 32
	if ( red < 64 && green < 64 && blue < 64 )
		{ // if all < 64 but something >= 32, then it is dark gray
		p = 8; // 128,128,128
		}
	else // at least one is >= 64
	if ( red < 128 && green < 128 && blue < 128 )
		{ // if all < 128 but something >= 64, then low intensity
		p = 0;
		if ( red   >=  64 ) { p += 1; red   -= 128; }
		if ( green >=  64 ) { p += 2; green -= 128; }
		if ( blue  >=  64 ) { p += 4; blue  -= 128; }
		}
	else
		{ // if something >= 128, then high intensity
		p = 8; // intensity is on
		if ( red   >= 128 ) { p += 1; red   -= 255; }
		if ( green >= 128 ) { p += 2; green -= 255; }
		if ( blue  >= 128 ) { p += 4; blue  -= 255; }
		}
#endif

/************************************************************************/
void Convert8Pto4( LPTR lpPixel, int iCount, register LPINT lpError,
                     LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
LPTR lpFirst, lpLast, lpMatrix;
int red, green, blue;
register int thresh;
register BYTE p, op;
BOOL IsOdd;
LPRGB lpRGBmap, lpRGB;
BYTE i;

lpRGBmap = lpBltSession->TypeInfo.ColorMap->RGBData;

lpFirst = Matrix + ((yDiva&15)*16);
lpLast = lpFirst + 15;
lpMatrix = lpFirst + (xDiva&15);
IsOdd = NO;
while (--iCount >= 0)
    {
	 i = *lpPixel++;
	 lpRGB = lpRGBmap + i;

    red   = lpRGB->red;
    green = lpRGB->green;
    blue  = lpRGB->blue;

    thresh = *lpMatrix++;
    if (lpMatrix > lpLast)
        lpMatrix = lpFirst;

    if (red >= thresh)
        {
        if (green >= thresh)
            {
            if (blue >= thresh)
                p = C_WHITE;
            else
                p = C_YELLOW;
            }
        else
		if (blue >= thresh)
            p = C_MAGENTA;
        else
            p = C_RED;
        }
    else
	if (green >= thresh)
        {
        if (blue >= thresh)
            p = C_CYAN;
        else
            p = C_GREEN;
        }
    else
	if (blue >= thresh)
        p = C_BLUE;
    else
        p = C_BLACK;
    if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
    else
		{
		op = p << 4;
		IsOdd = YES;
		}
    }

if ( IsOdd )
    *lpOutput = op;
}

/************************************************************************/
void Convert8Pto4S( LPTR lpPixel, int iCount, register LPINT lpError,
                        LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
int red, green, blue;
register BYTE p, op;
BOOL IsOdd;
LPRGB lpRGBmap, lpRGB;
BYTE i;

lpRGBmap = lpBltSession->TypeInfo.ColorMap->RGBData;

IsOdd = NO;
red = green = blue = 0;
while ( --iCount >= 0 )
    {
	 i = *lpPixel++;
	 lpRGB = lpRGBmap + i;

	// Compute the accumulated pixel values
    red += (*(lpError+0) + lpRGB->red);
    green += (*(lpError+1) + lpRGB->green);
    blue += (*(lpError+2) + lpRGB->blue);

    if (red >= THRESH)
		{
		if (green >= THRESH)
		    {
		    if (blue >= THRESH)
				{
				red -= 255;
				green -= 255;
				blue -= 255;
				p = C_WHITE;
				}
		    else
				{
				red -= 255;
				green -= 255;
				p = C_YELLOW;
				}
		    }
		else
		if (blue >= THRESH)
		    {
		    red -= 255;
		    blue -= 255;
		    p = C_MAGENTA;
		    }
		else
		    {
		    red -= 255;
		    p = C_RED;
		    }
		}
    else
	if (green >= THRESH)
		{
		if (blue >= THRESH)
		    {
		    green -= 255;
		    blue -= 255;
		    p = C_CYAN;
		    }
		else
		    {
		    green -= 255;
		    p = C_GREEN;
		    }
		}
	else
	if (blue >= THRESH)
		{
		blue -= 255;
		p = C_BLUE;
		}
    else
		{
		p = C_BLACK;
		}

    if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0; // red error
		*lpError++ = 0; // green error
		*lpError++ = 0; // blue error
		}
    else
		{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = red;       // red error
		*lpError++ = green;     // green error
		*lpError++ = blue;      // blue error
		red = 0;
		green = 0;
		blue = 0;
		}
    if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else
		{
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}

/************************************************************************/
void Convert8Pto4MS( LPTR lpPixel, int iCount, register LPINT lpError,
                           LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
int gray;
BYTE pixel;
register BYTE p, op;
BOOL IsOdd;
LPRGB lpRGBmap, lpRGB;
BYTE i;

lpRGBmap = lpBltSession->TypeInfo.ColorMap->RGBData;

IsOdd = NO;
gray = 0;
while ( --iCount >= 0 )
	{
	 i = *lpPixel++;
	 lpRGB = lpRGBmap + i;

	// Compute the accumulated pixel values
	pixel = RGBtoL(lpRGB);
	gray += (*lpError + pixel);

	if (gray >= THRESH)
		{
		p = C_WHITE;
		gray -= 255;
		}
	else
		{
		p = C_BLACK;
		}

	if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0;
		}
	else
		{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = gray;
		gray = 0;
		}

	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else
		{
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}

/************************************************************************/
void Convert8Pto4MHT( LPTR lpPixel, int iCount, register LPINT lpError,
                        LPTR lpOutput, int xDiva, int yDiva, LPBLTSESSION lpBltSession )
/************************************************************************/
{
PTR pPat, pEndPat, pStartPat;
register BYTE p, op, pixel;
BOOL IsOdd;
LPRGB lpRGBmap, lpRGB;
BYTE i;

lpRGBmap = lpBltSession->TypeInfo.ColorMap->RGBData;

IsOdd = NO;
pStartPat = Pattern   + 8*(yDiva&7);
pEndPat   = pStartPat + 8;
pPat      = pStartPat + (xDiva&7);
while ( --iCount >= 0 )
	{
   i = *lpPixel++;
	lpRGB = lpRGBmap + i;
	pixel = RGBtoL(lpRGB);
	if (pixel > *pPat++)
		p = C_WHITE;
	else
		p = C_BLACK;
	if ( IsOdd )
		{
		op += p;
		*lpOutput++ = op;
		IsOdd = NO;
		}
	else
		{
		op = p << 4;
		IsOdd = YES;
		}
	if ( pPat >= pEndPat )
		pPat = pStartPat;
	}

if ( IsOdd )
	*lpOutput = op;
}


