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

#define RAND_BITS 7
#define RAND_SUB 4   /* should be (RAND_BITS+1)/2 */
#define rotx(x, y, xc, yc, cosine, sine) fix2int((cosine * (x - xc)) + (sine * (yc - y)))

static ULONG lSeed = 0xDEADBABA;
static int xr, nRepetitions, x1, y1, x2, y2;
static int startR, startG, startB;
static int Rrange, Grange, Brange;
static long D;
static FIXED sine, cosine;
static BOOL bRadial;

/************************************************************************/
int vignette_proc(y, left, right, lpDst, lpSrc )
/************************************************************************/
int y, left, right;
LPTR lpDst, lpSrc;
{
int xn, v, r, tdx, tdy;
int Rvalue, Gvalue, Bvalue;
long d;
RGBS rgb;
FIXED rate;

for (xn = left; xn <= right; ++xn, lpSrc += DEPTH)
	{
	if ( !bRadial )
		{
		d = rotx(xn, y, x1, y1, cosine, sine);
		if ( d < 0 )
			d = 0;
		if ( d > D )
			d = D;
		}
	else	{
		tdx = xn - x1;
		tdy = y - y1;
		d = lsqrt(((long)tdx*(long)tdx)+((long)tdy*(long)tdy));
		if ( d > D )
			d = D;
		}

	if ( nRepetitions == 1 )
		rate = FGET( d, D );
	else	{
		rate = FGET( (d * nRepetitions), D );
		if ( rate > UNITY )
			{
			rate--;
			if ( Mask.VigSoftTransition && (HIWORD(rate) & 1) )
				rate = UNITY - LOWORD(rate);
			else	{
				rate--;
				rate = LOWORD(rate);
				rate++;
				}
			}
		}
	if ( lSeed & BIT18 )
		{
		lSeed += lSeed;
		lSeed ^= BITMASK;
		}
	else	lSeed += lSeed;
	if ( (r = (lSeed & RAND_BITS) - RAND_SUB) >= 0 )
		r++;
	v = r + FMUL( Rrange, rate );
	Rvalue = startR + v;
	v = r + FMUL( Grange, rate );
	Gvalue = startG + v;
	v = r + FMUL( Brange, rate );
	Bvalue = startB + v;

	rgb.red = bound( Rvalue, 0, 255 );
	rgb.green = bound( Gvalue, 0, 255 );
	rgb.blue = bound( Bvalue, 0, 255 );
	frame_putRGB( lpSrc, &rgb );
	}
}

/************************************************************************/
void Vignette( sx1, sy1, sx2, sy2 )
/************************************************************************/
int sx1, sy1, sx2, sy2;
{
LPFRAME lpFrame;
RECT rMask;
int dx, dy;
RGBS startRGB, endRGB;

if (!(lpFrame = frame_set(NULL)))
	return;

x1 = sx1; y1 = sy1; x2 = sx2; y2 = sy2;

Display2File(&x1, &y1);
Display2File(&x2, &y2);
dx = x2 - x1;
dy = y2 - y1;
if (!dx && !dy)
	return;

bRadial = (Mask.VignetteType == IDC_VIGRADIAL);
if ( (nRepetitions = Mask.VigRepeatCount) <= 0 )
	nRepetitions = 1;

startRGB = Palette.ActiveRGB;
endRGB = Palette.AlternateRGB;

D = lsqrt(((long)dx*(long)dx)+((long)dy*(long)dy));
if ( !bRadial )
	{
	sine = FGET(-dy, D);
	cosine = FGET(dx, D);
	xr = rotx(x2, y2, x1, y1, cosine, sine);
	}

Rrange = (int)endRGB.red - (int)startRGB.red;
Grange = (int)endRGB.green - (int)startRGB.green;
Brange = (int)endRGB.blue - (int)startRGB.blue;
startR = startRGB.red; startG = startRGB.green; startB = startRGB.blue;

mask_rect( &rMask );
if (LineEngine(&rMask, NULL, NULL, lpFrame, vignette_proc, 0, Mask.Opacity))
	UpdateImage(&rMask, YES);
}


/************************************************************************/
void TintFill( lpRGB )
/************************************************************************/
LPRGB lpRGB;
{
RECT rMask;
LPFRAME lpFrame;

if (!(lpFrame = frame_set(NULL)))
	return;
mask_rect( &rMask );
if (LineEngine(&rMask, lpRGB, NULL, lpFrame, NULL, 0, Mask.Opacity))
	UpdateImage(&rMask, YES);
}

/************************************************************************/
void TextureFill( lpTexture, TileMode )
/************************************************************************/
LPFRAME lpTexture;
int TileMode;
{
RECT rMask;
LPFRAME lpFrame;

if ( !lpTexture )
	return;
if ( !(lpFrame = frame_set(NULL)) )
	return;
mask_rect( &rMask );
if (LineEngine(&rMask, NULL, lpTexture, lpFrame, NULL, TileMode, Mask.Opacity))
	UpdateImage(&rMask, YES);
}

/************************************************************************/
int invert_proc(y, left, right, lpDst, lpSrc )
/************************************************************************/
int y, left, right;
LPTR lpDst, lpSrc;
{
int iCount, depth;
RGBS rgb;

iCount = right - left + 1;
depth = DEPTH;
while (--iCount >= 0)
	{
	frame_getRGB(lpDst, &rgb);
	rgb.red ^= 255;
	rgb.green ^= 255;
	rgb.blue ^= 255;
	frame_putRGB(lpSrc, &rgb);
	lpDst += depth;
	lpSrc += depth;
	}
}

/************************************************************************/
void InvertFill()
/************************************************************************/
{
RECT rMask;
LPFRAME lpFrame;

if (!(lpFrame = frame_set(NULL)))
	return;
mask_rect( &rMask );
if (LineEngine(&rMask, NULL, NULL, lpFrame, invert_proc, 0, 255))
	UpdateImage(&rMask, YES);
}

