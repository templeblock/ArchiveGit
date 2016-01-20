// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

#define RGBMAX 255
#define HSLMAX 255
#define HMAX 252 /* must be divisible by 6 */
#define DEGREE60  ((1*HMAX)/6)
#define DEGREE120 ((2*HMAX)/6)
#define DEGREE180 ((3*HMAX)/6)
#define DEGREE240 ((4*HMAX)/6)
#define DEGREE300 ((5*HMAX)/6)
#define DEGREE360 ((6*HMAX)/6)
#define UNDEFINED 0

/***********************************************************************/
void RGBtoHSL( R, G, B, lpHSL )
/***********************************************************************/
BYTE R,G,B;          /* input RGB values */
LPHSL lpHSL;
{
BYTE cMax,cMin;      /* max and min RGB values */
WORD sum, dif;
int i, H;
BYTE L,S;

/* calculate lightness */
cMax = max( max( R, G ), B );
cMin = min( min( R, G ), B );
sum = cMax + cMin;
dif = cMax - cMin;
L = (cMax * HSLMAX) / RGBMAX;

if ( dif )
	{ /* chromatic case */
	/* saturation */
	S = (dif * HSLMAX) / cMax; /* always = 1 - (cMin/L) */

	/* hue */
	if (R == cMax)	H = ((DEGREE60 * (i = G - B)) / (int)dif);
	else
	if (G == cMax)	H = ((DEGREE60 * (i = B - R)) / (int)dif) + DEGREE120;
	else
	if (B == cMax)	H = ((DEGREE60 * (i = R - G)) / (int)dif) + DEGREE240;

	if (H < 0)		H += DEGREE360;
	if (H > DEGREE360)	H -= DEGREE360;
	}
else	{ /* r=g=b --> achromatic case */
	S = 0;                     /* saturation */
	H = UNDEFINED;             /* hue */
	}

lpHSL->hue = H;
lpHSL->sat = S;
lpHSL->lum = L;
}


/***********************************************************************/
void HSLtoRGB( hue, sat, lum, lpRGB )
/***********************************************************************/
BYTE hue, lum, sat;
LPRGB lpRGB;
{
BYTE R,G,B;	/* RGB component values */
int n1, n2, fract; /* calculated magic numbers (really!) */

if ( !sat )
	{ /* achromatic case */
	R = G = B = (RGBMAX * lum) / HSLMAX;
	}
else 	{ /* chromatic case */
	/* range check: note values passed add/subtract thirds of range */
	if ( hue > DEGREE360 )	hue -= DEGREE360;

	/* calculate triplet */
	fract = ((hue*255)/DEGREE60) & 0xff;
	n1 = (sat * lum) / HSLMAX;
	n2 = (n1*fract)/255;

	switch( hue/DEGREE60 )
		{
		case 0: /* 0-59 degrees */
		R = lum;
		G = lum - n1 + n2;
		B = lum - n1;
		break;

		case 1: /* 60-119 degrees */
		R = lum - n2;
		G = lum;
		B = lum - n1;
		break;

		case 2: /* 120-179 degrees */
		R = lum - n1;
		G = lum;
		B = lum - n1 + n2;
		break;

		case 3: /* 180-239 degrees */
		R = lum - n1;
		G = lum - n2;
		B = lum;
		break;

		case 4: /* 240-299 degrees */
		R = lum - n1 + n2;
		G = lum - n1;
		B = lum;
		break;

		default: /* 300-359 degrees */
		R = lum;
		G = lum - n1;
		B = lum - n2;
		break;
		}

	R = (RGBMAX * R) / HSLMAX;
	G = (RGBMAX * G) / HSLMAX;
	B = (RGBMAX * B) / HSLMAX;
	}

lpRGB->red = R;
lpRGB->green = G;
lpRGB->blue = B;
}
