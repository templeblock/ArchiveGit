// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

static DWORD lSeed = 0xDEADBABA;

/***********************************************************************/
// Cache Data Converters
/***********************************************************************/

BYTE Matrix[256] = {
  2, 236,  60, 220,  16, 232,  56, 216,   3, 233,  57, 217,  13, 229,  53, 213,
130,  66, 188, 124, 144,  80, 184, 120, 131,  67, 185, 121, 141,  77, 181, 117,
 34, 194,  18, 252,  48, 208,  32, 248,  35, 195,  19, 249,  45, 205,  29, 245,
162,  98, 146,  82, 176, 112, 160,  96, 163,  99, 147,  83, 173, 109, 157,  93,
 10, 226,  50, 210,   6, 240,  64, 224,  11, 227,  51, 211,   7, 237,  61, 221,
138,  74, 178, 114, 134,  70, 192, 128, 139,  75, 179, 115, 135,  71, 189, 125,
 42, 202,  26, 242,  38, 198,  22, 255,  43, 203,  27, 243,  39, 199,  23, 253,
170, 106, 154,  90, 166, 102, 150,  86, 171, 107, 155,  91, 167, 103, 151,  87,
  4, 234,  58, 218,  14, 230,  54, 214,   1, 235,  59, 219,  15, 231,  55, 215,
132,  68, 186, 122, 142,  78, 182, 118, 129,  65, 187, 123, 143,  79, 183, 119,
 36, 196,  20, 250,  46, 206,  30, 246,  33, 193,  17, 251,  47, 207,  31, 247,
164, 100, 148,  84, 174, 110, 158,  94, 161,  97, 145,  81, 175, 111, 159,  95,
 12, 228,  52, 212,   8, 238,  62, 222,   9, 225,  49, 209,   5, 239,  63, 223,
140,  76, 180, 116, 136,  72, 190, 126, 137,  73, 177, 113, 133,  69, 191, 127,
 44, 204,  28, 244,  40, 200,  24, 254,  41, 201,  25, 241,  37, 197,  21, 255,
172, 108, 156,  92, 168, 104, 152,  88, 169, 105, 153,  89, 165, 101, 149,  85};

#ifndef C_CODE
/************************************************************************/
void Convert24to8( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPRGB lpPixel;
int iCount;
register LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
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
	p = 0;

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
		if ( red < 26 )		{ /* p += 36*0; red -= 0; */ }
		else if ( red < 77 )	{ p += 36*1; red -= 51; }
		     else		{ p += 36*2; red -= 102; }
		}
	else	{
		if ( red < 179 )	{ p += 36*3; red -= 153; }
		else if ( red < 230 )	{ p += 36*4; red -= 204; }
		     else		{ p += 36*5; red -= 255; }
		}

	if ( red > thresh )
		p += 36;
	else
	if ( red < negthresh )
		p -= 36;

// GREEN - Handle the green component
	if ( green < 128 )
		{
		if ( green < 26 )	{ /* p += 6*0; green -= 0; */ }
		else if ( green < 77 )	{ p += 6*1; green -= 51; }
		     else		{ p += 6*2; green -= 102; }
		}
	else	{
		if ( green < 179 )	{ p += 6*3; green -= 153; }
		else if ( green < 230 )	{ p += 6*4; green -= 204; }
		     else		{ p += 6*5; green -= 255; }
		}

	if ( green > thresh )
		p += 6;
	else
	if ( green < negthresh )
		p -= 6;

// BLUE - Handle the blue component
	if ( blue < 128 )
		{
		if ( blue < 26 )	{ /* p += 0; blue -= 0; */ }
		else if ( blue < 77 )	{ p += 1; blue -= 51; }
		     else		{ p += 2; blue -= 102; }
		}
	else	{
		if ( blue < 179 )	{ p += 3; blue -= 153; }
		else if ( blue < 230 )	{ p += 4; blue -= 204; }
		     else		{ p += 5; blue -= 255; }
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
void Convert24to8G( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPRGB lpPixel;
int iCount;
register LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
int thresh, negthresh;
LPTR lp, lpLast, lpFirst;
int red, green, blue;
register BYTE p;
register WORD w;

lpFirst = Matrix + (16*(yDiva&15));
lpLast = lpFirst + 15;
lp = lpFirst + (xDiva&15);

red = green = blue = 0;
while ( --iCount >= 0 )
	{
	p = 0;
	// Compute the accumulated pixel values
	red   = lpPixel->red;
	green = lpPixel->green;
	blue  = lpPixel->blue;
	++lpPixel;

	thresh = *lp / 4; // 0-255 maps to 0-51
	negthresh = thresh - 64; // 0-255 maps to (-64)-0
	if ( ++lp > lpLast )
		lp = lpFirst;

// RED - Handle the red component
	if ( red < 96 )
		{
		if ( red < 32 )		{ /* p += 25*0; red -= 0; */ }
		else 			{ p += 25*1; red -= 64; }
		}
	else	{
		if ( red < 160 )	{ p += 25*2; red -= 128; }
		else if (red < 224)     { p += 25*3; red -= 192; }
		else			{ p += 25*4; red -= 255; }
		}

	if ( red > thresh )
		p += 25;
	else
	if ( red < negthresh )
		p -= 25;

// GREEN - Handle the green component
	if ( green < 96 )
		{
		if ( green < 32 )	{ /* p += 5*0; green -= 0; */ }
		else 			{ p += 5*1; green -= 64; }
		}
	else	{
		if ( green < 160 )	{ p += 5*2; green -= 128; }
		else if (green < 224)   { p += 5*3; green -= 192; }
		else			{ p += 5*4; green -= 255; }
		}

	if ( green > thresh )
		p += 5;
	else
	if ( green < negthresh )
		p -= 5;

// BLUE - Handle the blue component
	if ( blue < 96 )
		{
		if ( blue < 32 )	{ /* p += 0; blue -= 0; */ }
		     else		{ p += 1; blue -= 64; }
		}
	else	{
		if ( blue < 160 )	{ p += 2; blue -= 128; }
		else if ( blue < 224 )	{ p += 3; blue -= 192; }
		     else		{ p += 4; blue -= 255; }
		}

	if ( blue > thresh )
		p++;
	else
	if ( blue < negthresh )
		p--;

	*lpOutput++ = (p + 101); // Output the computed pixel
	}
}

/************************************************************************/
void Convert16to8( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPWORD lpPixel;
int iCount;
LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
int thresh, negthresh;
LPTR lp, lpLast, lpFirst;
int red, green, blue;
register BYTE p;
register WORD w;

lpFirst = Matrix + (16*(yDiva&15));
lpLast = lpFirst + 15;
lp = lpFirst + (xDiva&15);

red = green = blue = 0;
while ( --iCount >= 0 )
	{
	p = 0;
	w = *lpPixel++;

	// Compute the accumulated pixel values
	red   = MaxiR(w);
	green = MaxiG(w);
	blue  = MaxiB(w);

	thresh = *lp / 5; // 0-255 maps to 0-51
	negthresh = thresh - 51; // 0-255 maps to (-51)-0
	if ( ++lp > lpLast )
		lp = lpFirst;

// RED - Handle the red component
	if ( red < 128 )
		{
		if ( red < 26 )		{ /* p += 36*0; red -= 0; */ }
		else if ( red < 77 )	{ p += 36*1; red -= 51; }
		     else		{ p += 36*2; red -= 102; }
		}
	else	{
		if ( red < 179 )	{ p += 36*3; red -= 153; }
		else if ( red < 230 )	{ p += 36*4; red -= 204; }
		     else		{ p += 36*5; red -= 255; }
		}

	if ( red > thresh )
		p += 36;
	else
	if ( red < negthresh )
		p -= 36;

// GREEN - Handle the green component
	if ( green < 128 )
		{
		if ( green < 26 )	{ /* p += 6*0; green -= 0; */ }
		else if ( green < 77 )	{ p += 6*1; green -= 51; }
		     else		{ p += 6*2; green -= 102; }
		}
	else	{
		if ( green < 179 )	{ p += 6*3; green -= 153; }
		else if ( green < 230 )	{ p += 6*4; green -= 204; }
		     else		{ p += 6*5; green -= 255; }
		}

	if ( green > thresh )
		p += 6;
	else
	if ( green < negthresh )
		p -= 6;

// BLUE - Handle the blue component
	if ( blue < 128 )
		{
		if ( blue < 26 )	{ /* p += 0; blue -= 0; */ }
		else if ( blue < 77 )	{ p += 1; blue -= 51; }
		     else		{ p += 2; blue -= 102; }
		}
	else	{
		if ( blue < 179 )	{ p += 3; blue -= 153; }
		else if ( blue < 230 )	{ p += 4; blue -= 204; }
		     else		{ p += 5; blue -= 255; }
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
void Convert16to8G( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPWORD lpPixel;
int iCount;
LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
int thresh, negthresh;
LPTR lp, lpLast, lpFirst;
int red, green, blue;
register BYTE p;
register WORD w;

lpFirst = Matrix + (16*(yDiva&15));
lpLast = lpFirst + 15;
lp = lpFirst + (xDiva&15);

red = green = blue = 0;
while ( --iCount >= 0 )
	{
	p = 0;
	w = *lpPixel++;

	// Compute the accumulated pixel values
	red   = MaxiR(w);
	green = MaxiG(w);
	blue  = MaxiB(w);

	thresh = *lp / 4; // 0-255 maps to 0-51
	negthresh = thresh - 64; // 0-255 maps to (-51)-0
	if ( ++lp > lpLast )
		lp = lpFirst;

// RED - Handle the red component
	if ( red < 96 )
		{
		if ( red < 32 )		{ /* p += 25*0; red -= 0; */ }
		else 			{ p += 25*1; red -= 64; }
		}
	else	{
		if ( red < 160 )	{ p += 25*2; red -= 128; }
		else if (red < 224)     { p += 25*3; red -= 192; }
		else			{ p += 25*4; red -= 255; }
		}

	if ( red > thresh )
		p += 25;
	else
	if ( red < negthresh )
		p -= 25;

// GREEN - Handle the green component
	if ( green < 96 )
		{
		if ( green < 32 )	{ /* p += 5*0; green -= 0; */ }
		else 			{ p += 5*1; green -= 64; }
		}
	else	{
		if ( green < 160 )	{ p += 5*2; green -= 128; }
		else if (green < 224)   { p += 5*3; green -= 192; }
		else			{ p += 5*4; green -= 255; }
		}

	if ( green > thresh )
		p += 5;
	else
	if ( green < negthresh )
		p -= 5;

// BLUE - Handle the blue component
	if ( blue < 96 )
		{
		if ( blue < 32 )	{ /* p += 0; blue -= 0; */ }
		     else		{ p += 1; blue -= 64; }
		}
	else	{
		if ( blue < 160 )	{ p += 2; blue -= 128; }
		else if ( blue < 224 )	{ p += 3; blue -= 192; }
		     else		{ p += 4; blue -= 255; }
		}

	if ( blue > thresh )
		p++;
	else
	if ( blue < negthresh )
		p--;

	*lpOutput++ = (p + 101); // Output the computed pixel
	}
}


/************************************************************************/
void Convert8to8( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPTR lpPixel;
int iCount;
LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
while ( --iCount >= 0 )
	*lpOutput++ = Lut255to100[*lpPixel++];
}

/************************************************************************/
void Convert8to8C( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPTR lpPixel;
int iCount;
LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{

while ( --iCount >= 0 )
	*lpOutput++ = (*lpPixel++>>4) + 216;
}

#define C_WHITE		15
#define C_BLACK		0
#define C_RED		9
#define C_BLUE		12
#define C_GREEN		10
#define C_CYAN		14
#define C_MAGENTA	13
#define C_YELLOW	11

/************************************************************************/
void Convert24to4( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPRGB lpPixel;
int iCount;
register LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
Convert24to4A(lpPixel, iCount, lpError, lpOutput, xDiva, yDiva, (LPTR)Matrix);
#ifdef C_CODE
LPTR lpFirst, lpLast, lpMatrix;
int red, green, blue;
register int thresh;
register BYTE p, op;
register WORD w;
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
        else if (blue >= thresh)
            p = C_MAGENTA;
        else
            p = C_RED;
        }
    else if (green >= thresh)
        {
        if (blue >= thresh)
            p = C_CYAN;
        else
            p = C_GREEN;
        }
    else if (blue >= thresh)
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
    {
   // op <<= 4;  /* Ted added this */
    *lpOutput = op;
    }
#endif
}

/************************************************************************/
void Convert16to4( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPWORD lpPixel;
int iCount;
LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
LPTR lpFirst, lpLast, lpMatrix;
int red, green, blue;
register int thresh;
register BYTE p, op;
register WORD w;
BOOL IsOdd;

lpFirst = Matrix + ((yDiva&15)*16);
lpLast = lpFirst + 15;
lpMatrix = lpFirst + (xDiva&15);
IsOdd = NO;
while (--iCount >= 0)
    {
    w = *lpPixel++;

    red = MaxiR(w);
    green = MaxiG(w);
    blue  = MaxiB(w);
    
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
        else if (blue >= thresh)
            p = C_MAGENTA;
        else
            p = C_RED;
        }
    else if (green >= thresh)
        {
        if (blue >= thresh)
            p = C_CYAN;
        else
            p = C_GREEN;
        }
    else if (blue >= thresh)
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

#ifdef SCATTERED
/************************************************************************/
void Convert24to4( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPRGB lpPixel;
int iCount;
register LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
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
	else	{ // if something >= 192, then high intensity
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
	else	{ // Pass the error down to the next line
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
	else	{
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}

/************************************************************************/
void Convert16to4( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPWORD lpPixel;
int iCount;
LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
int red, green, blue;
register BYTE p, op;
register WORD w;
BOOL IsOdd;

IsOdd = NO;
red = green = blue = 0;
while ( --iCount >= 0 )
	{
	w = *lpPixel++;

	// Compute the accumulated pixel values
	red   += (*lpError++ + MaxiR(w));
	green += (*lpError++ + MaxiG(w));
	blue  += (*lpError   + MaxiB(w));
	lpError -= 2;

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
	else	{ // if something >= 192, then high intensity
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
	else	{ // Pass the error down to the next line
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
	else	{
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}
#endif

/************************************************************************/
void Convert8to4( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPTR lpPixel;
int iCount;
register LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
int gray;
BYTE pixel, dummy;
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
		if ( gray >= 224 )	{ p = 15; gray -= 255; }
		else			{ p = 7; gray -= 192; }
		}
	else	{
		if ( gray >= 64 )	{ p = 8; gray -= 128; }
		else			{ p = 0; }
		}

	if ( lSeed & BIT18 )
		{ /* Pass the error right */
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0;
		}
	else	{ // Pass the error down to the next line
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
	else	{
		op = p << 4;
		IsOdd = YES;
		}
	}

if ( IsOdd )
	*lpOutput = op;
}

/************************************************************************/
void Convert24to24( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPRGB lpPixel;
int iCount;
LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
Convert24to24A(lpPixel, iCount, lpError, lpOutput, xDiva, yDiva, Matrix);
}

/************************************************************************/
void Convert16to24( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPWORD lpPixel;
int iCount;
LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
WORD w;

while (--iCount >= 0)
    {
    w = *lpPixel++;
    *lpOutput++ = MaxiB(w);
    *lpOutput++ = MaxiG(w);
    *lpOutput++ = MaxiR(w);
    }
}


/************************************************************************/
void Convert8to24( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPTR lpPixel;
int iCount;
LPINT lpError;
LPTR lpOutput;
int xDiva, yDiva;
{
Convert8to24A(lpPixel, iCount, lpError, lpOutput, xDiva, yDiva, Matrix);

#ifdef C_CODE
BYTE gray;

while (--iCount >= 0)
    {
    gray = *lpPixel++;
    *lpOutput++ = gray;
    *lpOutput++ = gray;
    *lpOutput++ = gray;
    }
#endif
}

/************************************************************************/
void Convert24to16( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPRGB lpPixel;
int iCount;
LPINT lpError;
LPWORD lpOutput;
int xDiva, yDiva;
{
con24to16(lpPixel, iCount, lpOutput);
}


/************************************************************************/
void Convert16to16( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPWORD lpPixel;
int iCount;
LPINT lpError;
LPWORD lpOutput;
int xDiva, yDiva;
{
while (--iCount >= 0)
    *lpOutput++ = *lpPixel++;
}


/************************************************************************/
void Convert8to16( lpPixel, iCount, lpError, lpOutput, xDiva, yDiva )
/************************************************************************/
LPTR lpPixel;
int iCount;
LPINT lpError;
LPWORD lpOutput;
int xDiva, yDiva;
{
con8to16(lpPixel, iCount, lpOutput);
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
		if ( red < 26 )		{ /* p += 36*0; red -= 0; */ }
		else if ( red < 77 )	{ p += 36*1; red -= 51; }
		     else		{ p += 36*2; red -= 102; }
		}
	else	{
		if ( red < 179 )	{ p += 36*3; red -= 153; }
		else if ( red < 230 )	{ p += 36*4; red -= 204; }
		     else		{ p += 36*5; red -= 255; }
		}

// GREEN - Handle the green component
	if ( green < 128 )
		{
		if ( green < 26 )	{ /* p += 6*0; green -= 0; */ }
		else if ( green < 77 )	{ p += 6*1; green -= 51; }
		     else		{ p += 6*2; green -= 102; }
		}
	else	{
		if ( green < 179 )	{ p += 6*3; green -= 153; }
		else if ( green < 230 )	{ p += 6*4; green -= 204; }
		     else		{ p += 6*5; green -= 255; }
		}

// BLUE - Handle the blue component
	if ( blue < 128 )
		{
		if ( blue < 26 )	{ /* p += 0; blue -= 0; */ }
		else if ( blue < 77 )	{ p += 1; blue -= 51; }
		     else		{ p += 2; blue -= 102; }
		}
	else	{
		if ( blue < 179 )	{ p += 3; blue -= 153; }
		else if ( blue < 230 )	{ p += 4; blue -= 204; }
		     else		{ p += 5; blue -= 255; }
		}

	if ( lSeed & BIT18 )
		{ // Pass the error right
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0; // Pass nothing down (leave it for next loop)
		*lpError++ = 0;
		*lpError++ = 0;
		}
	else	{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = red;
		*lpError++ = green;
		*lpError++ = blue;
		red = green = blue = 0;
		}

// 16 to 8 Method 1

red = green = blue = 0;
while ( --iCount >= 0 )
	{
	p = 0;
	w = *lpPixel++;

	// Compute the accumulated pixel values
	red   += (*lpError++ + MaxiR(w));
	green += (*lpError++ + MaxiG(w));
	blue  += (*lpError   + MaxiB(w));
	lpError -= 2;

// RED - Handle the red component
	if ( red < 128 )
		{
		if ( red < 26 )		{ /* p += 36*0; red -= 0; */ }
		else if ( red < 77 )	{ p += 36*1; red -= 51; }
		     else		{ p += 36*2; red -= 102; }
		}
	else	{
		if ( red < 179 )	{ p += 36*3; red -= 153; }
		else if ( red < 230 )	{ p += 36*4; red -= 204; }
		     else		{ p += 36*5; red -= 255; }
		}

// GREEN - Handle the green component
	if ( green < 128 )
		{
		if ( green < 26 )	{ /* p += 6*0; green -= 0; */ }
		else if ( green < 77 )	{ p += 6*1; green -= 51; }
		     else		{ p += 6*2; green -= 102; }
		}
	else	{
		if ( green < 179 )	{ p += 6*3; green -= 153; }
		else if ( green < 230 )	{ p += 6*4; green -= 204; }
		     else		{ p += 6*5; green -= 255; }
		}

// BLUE - Handle the blue component
	if ( blue < 128 )
		{
		if ( blue < 26 )	{ /* p += 0; blue -= 0; */ }
		else if ( blue < 77 )	{ p += 1; blue -= 51; }
		     else		{ p += 2; blue -= 102; }
		}
	else	{
		if ( blue < 179 )	{ p += 3; blue -= 153; }
		else if ( blue < 230 )	{ p += 4; blue -= 204; }
		     else		{ p += 5; blue -= 255; }
		}

	if ( lSeed & BIT18 )
		{ // Pass the error right
		lSeed += lSeed;
		lSeed ^= BITMASK;
		*lpError++ = 0; // Pass nothing down (leave it for next loop)
		*lpError++ = 0;
		*lpError++ = 0;
		}
	else	{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = red;
		*lpError++ = green;
		*lpError++ = blue;
		red = green = blue = 0;
		}

// 24 to 4 method 1

	if ( TOLUM(red,green,blue) >= 127 )
		{ // Bright colors
		p = 8;
		if ( red   > 128 ) { p += 1; red   -= 255; }
		if ( green > 128 ) { p += 2; green -= 255; }
		if ( blue  > 128 ) { p += 4; blue  -= 255; }
		if ( p == 8 ) // this is really 128,128,128
			{ red += 127; green += 127; blue += 127; }
		}
	else	{ // Dark colors issue 0 or 128 to guns
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
	else	{ // if something >= 192, then high intensity
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
	else	{ // if something >= 128, then high intensity
		p = 8; // intensity is on
		if ( red   >= 128 ) { p += 1; red   -= 255; }
		if ( green >= 128 ) { p += 2; green -= 255; }
		if ( blue  >= 128 ) { p += 4; blue  -= 255; }
		}
#endif
