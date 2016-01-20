//®FD1¯®PL1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "framelib.h"
#include "macros.h"

//************************************************************************
LPTR CreatePaletteLut15(LPRGB lpRGBmap, int nCubes, LPTR lpLut15, STATUSCALLBACKPROC lpStatusCallback) 
//************************************************************************
{
int j, i;
RGBS rgb1, rgb2;
LPTR lpLut, lpVisit, lpTmpVisit;
BYTE Grow[256];
WORD wCount, wRGB;
BYTE x, y, z;

if (!(lpLut = lpLut15))
	{
	lpLut = Alloc(32768L);
	if (!lpLut)
		return(NULL);
	}
lpVisit = Alloc(32768L);
if (!lpVisit)
	{
	if (!lpLut15)
		FreeUp(lpLut);
	return(NULL);
	}
hclr(lpVisit, 32768L);

wCount = 32768; // the number of table entries left to set
j = -1; // the amount of growth around each palette color
for (i = 0; i < nCubes; ++i)
	Grow[i] = YES;
// Loop until we have no more entries to set
while ( wCount )
	{
	// Seed the table with the palette we just created; grow a cube
	// around each color it until all 32k values are set
	j++;
	for ( i=0; i<nCubes; i++ )
		{
		if (!Grow[i])
			continue;

		rgb2 = lpRGBmap[i];

		rgb2.red >>= 3;
		rgb1.red = bound( (int)rgb2.red - j, 0, 31 );
		rgb2.red = bound( (int)rgb2.red + j, 0, 31 );

		rgb2.green >>= 3;
		rgb1.green = bound( (int)rgb2.green - j, 0, 31 );
		rgb2.green = bound( (int)rgb2.green + j, 0, 31 );

		rgb2.blue >>= 3;
		rgb1.blue = bound( (int)rgb2.blue - j, 0, 31 );
		rgb2.blue = bound( (int)rgb2.blue + j, 0, 31 );

		Grow[i] = NO;
		for ( x=rgb1.red;   x<=rgb2.red;   x++ )
		for ( y=rgb1.green; y<=rgb2.green; y++ )
		for ( z=rgb1.blue;  z<=rgb2.blue;  z++ )
			{
			wRGB = ToMiniRGB(x, y, z);
			lpTmpVisit = lpVisit + wRGB;
			if (*lpTmpVisit)
				continue;
			Grow[i] = YES;
			*lpTmpVisit = YES;
			// if we haven't visited this entry yet...
			lpLut[wRGB] = i;
			if ( !(--wCount) )
				goto Done;
			if (lpStatusCallback && (wCount&0x00fff))
				{
					(*lpStatusCallback)((int)(16384-(wCount>>2L)), (int)16384, NO);
				}
			}
		}
	}
Done:
FreeUp(lpVisit);
return(lpLut);
}

//************************************************************************
void CreateErrorLut15(LPRGB lpRGBmap, LPTR lpTable, LPTR lpErrorTable, BOOL bDither) 
//************************************************************************
{
WORD wRGB, wIndex;
int r, g, b;
RGBS rgb;

for ( wRGB=0; wRGB<=32767; wRGB++ )
	{
	// get the original index
	wIndex = lpTable[wRGB];

	if ( bDither )
		{
		// get desired color for this entry
		r = MaxiR(wRGB);
		g = MaxiG(wRGB);
		b = MaxiB(wRGB);

		// add in the error to the desired color; error=desired-actual
		r += (r - (int)lpRGBmap[wIndex].red);
		g += (g - (int)lpRGBmap[wIndex].green);
		b += (b - (int)lpRGBmap[wIndex].blue);

		// bound color value
		rgb.red   = mbound( r, 0, 255 );
		rgb.green = mbound( g, 0, 255 );
		rgb.blue  = mbound( b, 0, 255 );

		// convert to MiniRGB to create a new table index
		wIndex = lpTable[ RGBtoMiniRGB(&rgb) ];
		}
	lpErrorTable[wRGB] = wIndex;
	}
}


