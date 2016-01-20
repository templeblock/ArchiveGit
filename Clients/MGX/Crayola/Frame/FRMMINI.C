/*=======================================================================*\

	FRMMINI.C - 16 <--> 24 bit color conversion routines.

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

BYTE ToMaxi[32] = {
  0,   8,  16,  24,  32,  40,  48,  56,  64,  72,  80,  88,  96, 104, 112, 120,
128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 255};
/* UNUSED
  0,   8,  16,  25,  33,  41,  49,  58,  66,  74,  82,  90,  99, 107, 115, 123, 132, 140, 148, 156, 165, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255};
*/

/***********************************************************************/
void MiniRGBtoRGB( WORD miniRGB, LPRGB lpRGB )
/***********************************************************************/
{
lpRGB->red   = MaxiR( miniRGB );
lpRGB->green = MaxiG( miniRGB );
lpRGB->blue  = MaxiB( miniRGB );
}


/***********************************************************************/
BYTE MiniRGBtoL( WORD miniRGB )
/***********************************************************************/
{
RGBS rgb1;

/* Calculate lightness; Actually, L = ((cMax+cMin)/2 * HSLMAX) / RGBMAX;*/
MiniRGBtoRGB( miniRGB, &rgb1 );
return( TOLUM( rgb1.red, rgb1.green, rgb1.blue ) );
}


/***********************************************************************/
void MiniRGBtoHSL( WORD miniRGB, LPHSL lpHSL )
/***********************************************************************/
{
RGBS rgb1;

MiniRGBtoRGB( miniRGB, &rgb1 );
RGBtoHSL( rgb1.red, rgb1.green, rgb1.blue, (LPHSL)lpHSL );
}


/***********************************************************************/
WORD HSLtoMiniRGB( LPHSL lpHSL )
/***********************************************************************/
{
RGBS rgb1;

HSLtoRGB(lpHSL->hue, lpHSL->sat, lpHSL->lum, (LPRGB)&rgb1);
return( RGBtoMiniRGB( &rgb1 ) );
}

