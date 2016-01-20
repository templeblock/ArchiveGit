/*=======================================================================*\

	FRMHSL.C - HSL / RGB / CMYK Conversion routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "mgxframe.h"
#include "frameint.h"

#define RGBMAX 255
#define HSLMAX 255
#define MAX_HUES 252 /* must be divisible by 6 */
#define SMAX 255
#define LMAX 255
#define DEGREE60  ((1*MAX_HUES)/6)
#define DEGREE120 ((2*MAX_HUES)/6)
#define DEGREE180 ((3*MAX_HUES)/6)
#define DEGREE240 ((4*MAX_HUES)/6)
#define DEGREE300 ((5*MAX_HUES)/6)
#define DEGREE360 ((6*MAX_HUES)/6)
#define UNDEFINED 0

/*=================================*/

static RGB2CMYKPROC lpRGBtoCMYKProc = NULL;
static CMYK2RGBPROC lpCMYKtoRGBProc = NULL;

/*=================================*/

void SetRGBToCMYKProc(RGB2CMYKPROC lpRGBtoCMYK, CMYK2RGBPROC lpCMYKtoRGB)
{
	lpRGBtoCMYKProc = lpRGBtoCMYK;
	lpCMYKtoRGBProc = lpCMYKtoRGB;
}

/*=================================*/

void RGBtoCMYK( BYTE R, BYTE G, BYTE B, LPCMYK lpCMYK )
{
	BYTE C, M, Y, K;
	RGBS rgb;

	if (lpRGBtoCMYKProc)
	{
		rgb.red   = R;
		rgb.green = G;
		rgb.blue  = B;
		(*lpRGBtoCMYKProc)( (LPRGB)&rgb, lpCMYK, 1);
	}
	else
	{
		C = R^0xFF;
		M = G^0xFF;
		Y = B^0xFF;
		K = 0;

		lpCMYK->c = C;
		lpCMYK->m = M;
		lpCMYK->y = Y;
		lpCMYK->k = K;
	}
}

/*=================================*/

void RGBtoCMYKScanline( LPRGB lpRGB, LPCMYK lpCMYK, int iCount )
{
	if (lpRGBtoCMYKProc)
	{
		(*lpRGBtoCMYKProc)( lpRGB, lpCMYK, iCount );
	}
	else
	{
		while( iCount-- > 0)
		{
			lpCMYK->c = lpRGB->red  ^0xFF;
			lpCMYK->m = lpRGB->green^0xFF;
			lpCMYK->y = lpRGB->blue ^0xFF;
			lpCMYK->k = 0;
			lpRGB++;
			lpCMYK++;
		}
	}
}

/*=================================*/

void CMYKtoRGB( BYTE C, BYTE M, BYTE Y, BYTE K, LPRGB lpRGB)
{
	CMYKS cmyk;
	int R, G, B;

	if (lpCMYKtoRGBProc)
	{
		cmyk.c = C;
		cmyk.m = M;
		cmyk.y = Y;
		cmyk.k = K;
		(*lpCMYKtoRGBProc)( (LPCMYK)&cmyk, (LPRGB)lpRGB, 1 );
	}
	else
	{
		R = C^0xFF;
		G = M^0xFF;
		B = Y^0xFF;

		if (K)
		{
			R -= (R * (long)K) >> 8;
			G -= (G * (long)K) >> 8;
			B -= (B * (long)K) >> 8;
		}

		lpRGB->red   = R;
		lpRGB->green = G;
		lpRGB->blue  = B;
	}
}

/*=================================*/

void CMYKtoRGBScanline( LPCMYK lpCMYK, LPRGB lpRGB, int iCount )
{
	int R, G, B;

	if (lpCMYKtoRGBProc)
	{
		(*lpCMYKtoRGBProc)( (LPCMYK)lpCMYK, (LPRGB)lpRGB, iCount );
	}
	else
	{
		while(iCount-- > 0)
		{
			R = lpCMYK->c^0xFF;
			G = lpCMYK->m^0xFF;
			B = lpCMYK->y^0xFF;

			if (lpCMYK->k)
			{
				R -= (R * (long)lpCMYK->k) >> 8;
				G -= (G * (long)lpCMYK->k) >> 8;
				B -= (B * (long)lpCMYK->k) >> 8;
			}

			lpRGB->red   = R;
			lpRGB->green = G;
			lpRGB->blue  = B;

			lpCMYK++;
			lpRGB++;
		}
	}
}

/*=================================*/

void CMYKtoRGBBuf(LPCMYK lpCMYK, LPRGB lpRGB, int dx,
				FRMTYPEINFO TypeInfo, int dy)
{
	int R, G, B;

	if (lpCMYKtoRGBProc)
	{
		while(dy-- > 0)
		{
			(*lpCMYKtoRGBProc)( lpCMYK, lpRGB, dx );
			lpCMYK += dx;
			lpRGB += dx;
		}
	}
	else
	{
		long lCount = (long)dx * (long)dy;
		while(lCount-- > 0)
		{
			R = lpCMYK->c^0xFF;
			G = lpCMYK->m^0xFF;
			B = lpCMYK->y^0xFF;

			if (lpCMYK->k)
			{
				R -= (R * (long)lpCMYK->k) >> 8;
				G -= (G * (long)lpCMYK->k) >> 8;
				B -= (B * (long)lpCMYK->k) >> 8;
			}

			lpRGB->red   = R;
			lpRGB->green = G;
			lpRGB->blue  = B;

			lpCMYK++;
			lpRGB++;
		}
	}
}

/*=================================*/

BYTE CMYKtoL( LPCMYK lpCMYK )
{
	RGBS rgb;

	/* Calculate lightness; Actually, L = ((cMax+cMin)/2 * HSLMAX) / RGBMAX;*/
	CMYKtoRGB( lpCMYK->c, lpCMYK->m, lpCMYK->y, lpCMYK->k, &rgb);

	return( TOLUM( rgb.red, rgb.green, rgb.blue ) );
}

/*=================================*/

void CMYKtoS( LPCMYK lpCMYK, LPHSL lpHsl)
{
	RGBS rgb;

	CMYKtoRGB( lpCMYK->c, lpCMYK->m, lpCMYK->y, lpCMYK->k, &rgb);

	RGBtoS( rgb.red, rgb.green, rgb.blue, lpHsl );
}

/*=================================*/


void CMYKtoH( LPCMYK lpCMYK, LPHSL lpHsl)
{
	RGBS rgb;

	CMYKtoRGB( lpCMYK->c, lpCMYK->m, lpCMYK->y, lpCMYK->k, &rgb);

	RGBtoH( rgb.red, rgb.green, rgb.blue, lpHsl );
}

/*=================================*/

void CMYKtoSL( LPCMYK lpCMYK, LPHSL lpHsl)
{
	RGBS rgb;

	CMYKtoRGB( lpCMYK->c, lpCMYK->m, lpCMYK->y, lpCMYK->k, &rgb);

	RGBtoSL( rgb.red, rgb.green, rgb.blue, lpHsl );
}

/*=================================*/

void CMYKtoHL( LPCMYK lpCMYK, LPHSL lpHsl)
{
	RGBS rgb;

	CMYKtoRGB( lpCMYK->c, lpCMYK->m, lpCMYK->y, lpCMYK->k, &rgb);

	RGBtoHL( rgb.red, rgb.green, rgb.blue, lpHsl );
}

/*=================================*/

void CMYKtoHS( LPCMYK lpCMYK, LPHSL lpHsl)
{
	RGBS rgb;

	CMYKtoRGB( lpCMYK->c, lpCMYK->m, lpCMYK->y, lpCMYK->k, &rgb);

	RGBtoHS( rgb.red, rgb.green, rgb.blue, lpHsl );
}

/*=================================*/

void CMYKtoHSL( BYTE C, BYTE M, BYTE Y, BYTE K, LPHSL lpHSL )
{
	RGBS rgb;

	CMYKtoRGB( C, M, Y, K, &rgb);

	RGBtoHSL( rgb.red, rgb.green, rgb.blue, lpHSL );
}

/*=================================*/

void HSLtoCMYK( BYTE hue, BYTE lum, BYTE sat, LPCMYK lpCMYK )
{
	RGBS rgb;

	HSLtoRGB(hue, lum, sat, (LPRGB)&rgb);

	RGBtoCMYK( rgb.red, rgb.green, rgb.blue, lpCMYK );
}

/***********************************************************************/
BYTE RGBtoL( LPRGB lpRGB )
/***********************************************************************/
{
register BYTE r, g, b;

/* Calculate lightness; Actually, L = ((cMax+cMin)/2 * HSLMAX) / RGBMAX;*/
r = lpRGB->red;
g = lpRGB->green;
b = lpRGB->blue;
return( TOLUM( r, g, b ) );
}

/***********************************************************************/

/***********************************************************************/
void RGBtoHSL( BYTE R, BYTE G, BYTE B, LPHSL lpHSL )
/***********************************************************************/
{
BYTE cMax,cMin;      /* max and min RGB values */
WORD sum, dif;
int H;
BYTE L,S;

/* calculate lightness */
cMax = max( max( R, G ), B );
cMin = min( min( R, G ), B );
sum = cMax + cMin;
dif = cMax - cMin;
L = sum >> 1; // Actually L = ((sum/2) * HSLMAX) / RGBMAX;

if ( dif )
	{ /* chromatic case */
	/* saturation */
	if ( sum > RGBMAX )
		sum = 2*RGBMAX - sum;
	S = ( dif * HSLMAX ) / sum; // Actually, S = 1 - (cMin/L)

	/* hue */
	if (R == cMax)  H = ((DEGREE60 * (int)((int)G - B)) / (int)dif);
	else
	if (G == cMax)  H = ((DEGREE60 * (int)((int)B - R)) / (int)dif) + DEGREE120;
	else
	if (B == cMax)  H = ((DEGREE60 * (int)((int)R - G)) / (int)dif) + DEGREE240;

	if (H < 0)              H += DEGREE360;
	if (H > DEGREE360)      H -= DEGREE360;
	}
else    { /* r=g=b --> achromatic case */
	S = 0;                     /* saturation */
	H = UNDEFINED;             /* hue */
	}

lpHSL->hue = H;
lpHSL->sat = S;
lpHSL->lum = L;
}


/***********************************************************************/
void HSLtoRGB( BYTE hue, BYTE sat, BYTE lum, LPRGB lpRGB )
/***********************************************************************/
{
BYTE R,G,B; /* RGB component values */
int iHue, n, n1, n2; /* calculated magic numbers (really!) */

if ( !sat )
	{ /* achromatic case */
	lpRGB->red = lpRGB->green = lpRGB->blue = lum;
// Actually = (RGBMAX * lum) / LMAX
	return;
	}

/* chromatic case */
/* range check: note values passed add/subtract thirds of range */
if ( hue > DEGREE360 )  hue -= DEGREE360;

/* set up magic numbers */
n1 = n2 = lum;
if ( lum > LMAX/2 )
	lum = 255 - lum; // make lum no bigger than 50%
n = ((WORD)lum * sat) / SMAX; // n is never bigger that lum (or 50%)
n1 -= n; if ( n1 == 1 ) n1--;
n2 += n; if ( n2 == 254 ) n2++;

iHue = hue;
if ( iHue < DEGREE60 )
	{ /* 0-59 degrees */
	R = n2;
	G = n1 + (((n2-n1)*iHue)/DEGREE60);
	B = n1;
	}
else
if ( iHue < DEGREE120 )
	{ /* 60-119 degrees */
	iHue -= DEGREE120;
	R = n1 + (((n2-n1)*(-iHue))/DEGREE60);
	G = n2;
	B = n1;
	}
else
if ( iHue < DEGREE180 )
	{ /* 120-179 degrees */
	iHue -= DEGREE120;
	R = n1;
	G = n2;
	B = n1 + (((n2-n1)*iHue)/DEGREE60);
	}
else
if ( iHue < DEGREE240 )
	{ /* 180-239 degrees */
	iHue -= DEGREE240;
	R = n1;
	G = n1 + (((n2-n1)*(-iHue))/DEGREE60);
	B = n2;
	}
else
if ( iHue < DEGREE300 )
	{ /* 240-299 degrees */
	iHue -= DEGREE240;
	R = n1 + (((n2-n1)*iHue)/DEGREE60);
	G = n1;
	B = n2;
	}
else
//if ( iHue < DEGREE360 )
	{ /* 300-359 degrees */
	iHue -= DEGREE360;
	R = n2;
	G = n1;
	B = n1 + (((n2-n1)*(-iHue))/DEGREE60);
	}

// Actually, R = (RGBMAX * R) / HSLMAX, etc.
lpRGB->red   = R;
lpRGB->green = G;
lpRGB->blue  = B;
}

/***********************************************************************/
/***********************************************************************/
/* The routines below are here only to speed up the calculation of     */
/* individual hsl components.  Their calculations should be exactly    */
/* the same of RGBtoHSL.
/***********************************************************************/
/***********************************************************************/

/***********************************************************************/
void RGBtoHS( BYTE R, BYTE G, BYTE B, LPHSL lpHSL )
/***********************************************************************/
{
BYTE cMax,cMin;      /* max and min RGB values */
WORD sum, dif;
int H;
BYTE S;

/* calculate lightness */
cMax = max( max( R, G ), B );
cMin = min( min( R, G ), B );
sum = cMax + cMin;
dif = cMax - cMin;

if ( dif )
	{ /* chromatic case */
	/* saturation */
	if ( sum > RGBMAX )
		sum = 2*RGBMAX - sum;
	S = ( dif * HSLMAX ) / sum; // Actually, S = 1 - (cMin/L)

	/* hue */
	if (R == cMax)  H = ((DEGREE60 * (int)((int)G - B)) / (int)dif);
	else
	if (G == cMax)  H = ((DEGREE60 * (int)((int)B - R)) / (int)dif) + DEGREE120;
	else
	if (B == cMax)  H = ((DEGREE60 * (int)((int)R - G)) / (int)dif) + DEGREE240;

	if (H < 0)              H += DEGREE360;
	if (H > DEGREE360)      H -= DEGREE360;
	}
else    { /* r=g=b --> achromatic case */
	S = 0;                     /* saturation */
	H = UNDEFINED;             /* hue */
	}

lpHSL->hue = H;
lpHSL->sat = S;
}

/***********************************************************************/
void RGBtoHL( BYTE R, BYTE G, BYTE B, LPHSL lpHSL )
/***********************************************************************/
{
BYTE cMax,cMin;      /* max and min RGB values */
WORD sum, dif;
int H;
BYTE L;

/* calculate lightness */
cMax = max( max( R, G ), B );
cMin = min( min( R, G ), B );
sum = cMax + cMin;
dif = cMax - cMin;
L = sum >> 1; // Actually L = ((sum/2) * HSLMAX) / RGBMAX;

if ( dif )
	{ /* chromatic case */
	/* hue */
	if (R == cMax)  H = ((DEGREE60 * (int)((int)G - B)) / (int)dif);
	else
	if (G == cMax)  H = ((DEGREE60 * (int)((int)B - R)) / (int)dif) + DEGREE120;
	else
	if (B == cMax)  H = ((DEGREE60 * (int)((int)R - G)) / (int)dif) + DEGREE240;

	if (H < 0)              H += DEGREE360;
	if (H > DEGREE360)      H -= DEGREE360;
	}
else    { /* r=g=b --> achromatic case */
	H = UNDEFINED;             /* hue */
	}

lpHSL->hue = H;
lpHSL->lum = L;
}

/***********************************************************************/
void RGBtoSL( BYTE R, BYTE G, BYTE B, LPHSL lpHSL )
/***********************************************************************/
{
BYTE cMax,cMin;      /* max and min RGB values */
WORD sum, dif;
BYTE L,S;

/* calculate lightness */
cMax = max( max( R, G ), B );
cMin = min( min( R, G ), B );
sum = cMax + cMin;
dif = cMax - cMin;
L = sum >> 1; // Actually L = ((sum/2) * HSLMAX) / RGBMAX;

if ( dif )
	{ /* chromatic case */
	/* saturation */
	if ( sum > RGBMAX )
		sum = 2*RGBMAX - sum;
	S = ( dif * HSLMAX ) / sum; // Actually, S = 1 - (cMin/L)
	}
else    { /* r=g=b --> achromatic case */
	S = 0;                     /* saturation */
	}

lpHSL->sat = S;
lpHSL->lum = L;
}

/***********************************************************************/
void RGBtoH( BYTE R, BYTE G, BYTE B, LPHSL lpHSL )
/***********************************************************************/
{
BYTE cMax,cMin;      /* max and min RGB values */
WORD dif;
int H;

/* calculate lightness */
cMax = max( max( R, G ), B );
cMin = min( min( R, G ), B );
dif = cMax - cMin;

if ( dif )
	{ /* chromatic case */
	/* hue */
	if (R == cMax)  H = ((DEGREE60 * (int)((int)G - B)) / (int)dif);
	else
	if (G == cMax)  H = ((DEGREE60 * (int)((int)B - R)) / (int)dif) + DEGREE120;
	else
	if (B == cMax)  H = ((DEGREE60 * (int)((int)R - G)) / (int)dif) + DEGREE240;

	if (H < 0)              H += DEGREE360;
	if (H > DEGREE360)      H -= DEGREE360;
	}
else    { /* r=g=b --> achromatic case */
	H = UNDEFINED;             /* hue */
	}
lpHSL->hue = H;
}

/***********************************************************************/
void RGBtoS( BYTE R, BYTE G, BYTE B, LPHSL lpHSL )
/***********************************************************************/
{
BYTE cMax,cMin;      /* max and min RGB values */
WORD sum, dif;
BYTE S;

/* calculate lightness */
cMax = max( max( R, G ), B );
cMin = min( min( R, G ), B );
sum = cMax + cMin;
dif = cMax - cMin;

if ( dif )
	{ /* chromatic case */
	/* saturation */
	if ( sum > RGBMAX )
		sum = 2*RGBMAX - sum;
	S = ( dif * HSLMAX ) / sum; // Actually, S = 1 - (cMin/L)
	}
else    { /* r=g=b --> achromatic case */
	S = 0;                     /* saturation */
	}

lpHSL->sat = S;
}

