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
#define SMAX 255
#define LMAX 255
#define DEGREE60  ((1*HMAX)/6)
#define DEGREE120 ((2*HMAX)/6)
#define DEGREE180 ((3*HMAX)/6)
#define DEGREE240 ((4*HMAX)/6)
#define DEGREE300 ((5*HMAX)/6)
#define DEGREE360 ((6*HMAX)/6)
#define UNDEFINED 0

/***********************************************************************/
BYTE MiniRGBtoL( miniRGB )
/***********************************************************************/
WORD miniRGB;
{
RGBS rgb1;

/* Calculate lightness; Actually, L = ((cMax+cMin)/2 * HSLMAX) / RGBMAX;*/
ExpandRGB( &miniRGB, (LPRGB)&rgb1, 1 );
return( TOLUM( rgb1.red, rgb1.green, rgb1.blue ) );
}

/***********************************************************************/
BYTE RGBtoL( lpRGB )
/***********************************************************************/
LPRGB lpRGB;
{
register BYTE r, g, b;

/* Calculate lightness; Actually, L = ((cMax+cMin)/2 * HSLMAX) / RGBMAX;*/
r = lpRGB->red;
g = lpRGB->green;
b = lpRGB->blue;
return( TOLUM( r, g, b ) );
}

/***********************************************************************/
void MiniRGBtoHSL( miniRGB, lpHSL )
/***********************************************************************/
WORD miniRGB;
LPHSL lpHSL;
{
RGBS rgb1;

ExpandRGB((LPWORD)&miniRGB, (LPRGB)&rgb1, 1);
RGBtoHSL(rgb1.red, rgb1.green, rgb1.blue, (LPHSL)lpHSL);
}

/***********************************************************************/
WORD HSLtoMiniRGB( lpHSL )
/***********************************************************************/
LPHSL lpHSL;
{
RGBS rgb1;

HSLtoRGB(lpHSL->hue, lpHSL->sat, lpHSL->lum, (LPRGB)&rgb1);
return(RGBtoMiniRGB((LPRGB)&rgb1));
}

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
L = sum >> 1; // Actually L = ((sum/2) * HSLMAX) / RGBMAX;

if ( dif )
	{ /* chromatic case */
	/* saturation */
	if ( sum > RGBMAX )
		sum = 2*RGBMAX - sum;
	S = ( dif * HSLMAX ) / sum; // Actually, S = 1 - (cMin/L)

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
int n, n1, n2;	/* calculated magic numbers (really!) */

if ( !sat )
	{ /* achromatic case */
	lpRGB->red = lpRGB->green = lpRGB->blue = lum;
	// Actually = (RGBMAX * lum) / LMAX
	return;
	}

/* chromatic case */
/* range check: note values passed add/subtract thirds of range */
if ( hue > DEGREE360 )	hue -= DEGREE360;

/* set up magic numbers */
n = ((WORD)lum * sat) / SMAX;
if ( lum <= LMAX/2 )
	{
	n1 = (int)lum - n;
	n2 = (int)lum + n;
	}
else	{
	n1 = (int)lum + n - sat;
	n2 = (int)lum - n + sat;
	}

switch( hue/DEGREE60 )
	{
	case 0: /* 0-59 degrees */
	R = n2;
	G = n1 + (((n2-n1)*hue)/DEGREE60);
	B = n1;
	break;

	case 1: /* 60-119 degrees */
	R = n1 + (((n2-n1)*(DEGREE120-hue))/DEGREE60);
	G = n2;
	B = n1;
	break;

	case 2: /* 120-179 degrees */
	R = n1;
	G = n2;
	B = n1 + (((n2-n1)*(hue-DEGREE120))/DEGREE60);
	break;

	case 3: /* 180-239 degrees */
	R = n1;
	G = n1 + (((n2-n1)*(DEGREE240-hue))/DEGREE60);
	B = n2;
	break;

	case 4: /* 240-299 degrees */
	R = n1 + (((n2-n1)*(hue-DEGREE240))/DEGREE60);
	G = n1;
	B = n2;
	break;

	default: /* 300-359 degrees */
	R = n2;
	G = n1;
	B = n1 + (((n2-n1)*(DEGREE360-hue))/DEGREE60);
	break;
	}

// Actually, R = (RGBMAX * R) / HSLMAX, etc.
lpRGB->red   = R;
lpRGB->green = G;
lpRGB->blue  = B;
}
