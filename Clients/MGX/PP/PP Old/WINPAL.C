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

static LPTR lpHLut, lpSLut, lpMLut, lpRLut, lpGLut, lpBLut;

/************************************************************************/
BOOL MapSetupLuts(HLut, SLut, MLut, RLut, GLut, BLut)
/************************************************************************/
LPTR HLut, SLut, MLut, RLut, GLut, BLut;
{
if (!HLut)
	{
	if (lpHLut)
		{
		FreeUp(lpHLut);
		lpHLut = NULL;
		}
	return(TRUE);
	}
else
	{
	if (!lpHLut)
		{
		lpHLut = Alloc(256L*6L);
		if (!lpHLut)
			return(FALSE);
		lpSLut = lpHLut + 256;
		lpMLut = lpSLut + 256;
		lpRLut = lpMLut + 256;
		lpGLut = lpRLut + 256;
		lpBLut = lpGLut + 256;
		}
	copy(HLut, lpHLut, 256);
	copy(SLut, lpSLut, 256);
	copy(MLut, lpMLut, 256);
	copy(RLut, lpRLut, 256);
	copy(GLut, lpGLut, 256);
	copy(BLut, lpBLut, 256);
	return(TRUE);
	}
}

/************************************************************************/
void ResetAllMaps()
/************************************************************************/
{
ResetMap( &MasterMap, MAPPOINTS ); MasterMap.Color = LMAP;
ResetMap( &RedMap,    MAPPOINTS ); RedMap.Color    = RMAP; // sometimes CMAP
ResetMap( &GreenMap,  MAPPOINTS ); GreenMap.Color  = GMAP; // sometimes MMAP
ResetMap( &BlueMap,   MAPPOINTS ); BlueMap.Color   = BMAP; // sometimes YMAP
ResetMap( &HueMap,    MAXPOINTS ); HueMap.Color    = HMAP;
ResetMap( &SatMap,    MAPPOINTS ); SatMap.Color    = SMAP;
}


/************************************************************************/
void ResetMap( lpMap, maxpoints )
/************************************************************************/
LPMAP lpMap;
int maxpoints;
{
int i;
FIXED rate;

rate = FGET( 255, maxpoints-1 );
lpMap->Points = maxpoints;
for ( i=0; i<maxpoints; i++ )
	lpMap->Pnt[i].x = lpMap->Pnt[i].y = FMUL( i, rate );
for ( i=maxpoints; i<MAXPOINTS; i++ )
	lpMap->Pnt[i].x = lpMap->Pnt[i].y = 0;

lpMap->bStretch = NO;
lpMap->Contrast = 0;
lpMap->Brightness = 0;
lpMap->Threshold = 0;
lpMap->Negate = NO;
lpMap->Levels = 256;
MakeMap( lpMap );
lpMap->bModified = NO;
}


/************************************************************************/
void InvertMap( lpMap )
/************************************************************************/
LPMAP lpMap;
{
int i;
POINT pt;

// Flip the x and y values
for ( i=0; i<lpMap->Points; i++ )
	{
	lpMap->Pnt[i].x = 255 - lpMap->Pnt[i].x;
	lpMap->Pnt[i].y = 255 - lpMap->Pnt[i].y;
	}
// Turn the array of points around
for ( i=0; i<lpMap->Points/2; i++ )
	{
	pt = lpMap->Pnt[i];
	lpMap->Pnt[i] = lpMap->Pnt[lpMap->Points-i-1];
	lpMap->Pnt[lpMap->Points-i-1] = pt;
	}
// Flip the brightness adder
lpMap->Brightness = -lpMap->Brightness;
}

/************************************************************************/
int map_proc(y, left, right, lpDst, lpSrc )
/************************************************************************/
int y, left, right;
LPTR lpDst, lpSrc;
{
int iCount, depth;
RGBS rgb;

depth = DEPTH;
iCount = right - left + 1;
while (--iCount >= 0)
	{
	frame_getRGB(lpDst, &rgb);
	MapRGB( &rgb.red, &rgb.green, &rgb.blue );
	frame_putRGB( lpSrc, &rgb );
	lpDst += depth;
	lpSrc += depth;
	}
}

/************************************************************************/
void ApplyMap(dirty)
/************************************************************************/
int dirty;
{
RECT rMask;
LPFRAME lpFrame;

if (!(lpFrame = frame_set(NULL)))
	return;
if ( !CacheInit( NO, YES ) )
	return;
lpImage->lpDisplay->FloatingImageProc = NULL;
mask_rect( &rMask );
MapSetupLuts(HueMap.Lut, SatMap.Lut, MasterMap.Lut, RedMap.Lut, GreenMap.Lut, BlueMap.Lut);
if (LineEngine(&rMask, NULL, NULL, lpFrame, map_proc, 0, 255))
	{
	lpImage->dirty = dirty;
	UpdateImage(&rMask, YES);
	}
MapSetupLuts(NULL, NULL, NULL, NULL, NULL, NULL);
}


/************************************************************************/
void MakeMap( lpMap )
/************************************************************************/
LPMAP lpMap;
{
int i, j, x, x1, x2, y1, y2, brk, value, bStretch;
int StepMin, StepMax, Brightness, Contrast;
FIXED fbreak, fbreakincr, fvalue, fvalueincr, fxScale, fyScale;
long m;

lpMap->bModified = YES;
// If necessary, invert the map
if ( lpMap->Color == LMAP || lpMap->Color == RMAP ||
     lpMap->Color == GMAP || lpMap->Color == BMAP )
	InvertMap( lpMap );

if ( (Brightness = lpMap->Brightness) < 0 )
	Brightness = -TOGRAY( -Brightness );
else	Brightness =  TOGRAY(  Brightness );

fxScale = fyScale = -1;
if ( Contrast = lpMap->Contrast )
   if ( Contrast < 0 )
	fyScale = FGET( 100+Contrast, 100 ); // Squash vertically - y scaling
   else	fxScale = FGET( 100-Contrast, 100 ); // Squash horizontally - x scaling

/* First check to see if the points need to be stretched */
bStretch = (lpMap->bStretch == YES);
if ( bStretch && lpMap->Points == CALPOINTS )
	{
	/* Compute StepMin and StepMax */
	StepMin = StepMax = lpMap->Pnt[CALPOINTS-1].x;
	for ( i=0; i<CALPOINTS-1; i++ )
		{
		if ( lpMap->Pnt[i].x > StepMax )
			StepMax = lpMap->Pnt[i].x;
		if ( lpMap->Pnt[i].x < StepMin )
			StepMin = lpMap->Pnt[i].x;
		}
	if ( StepMin > 0 || StepMax < 255 )
		StepMax -= StepMin;
	else	bStretch = NO;
	}

/* Compute the luts values from the connected points */
x1 = 0;
y1 = lpMap->Pnt[0].y;
if ( fyScale >= 0 )
	y1 = FMUL( (y1 - 128), fyScale ) + 128;

for (i=0; i<=lpMap->Points; i++) /* go through all points */
	{
	if ( i == lpMap->Points )
		{
		x2 = 255;
		y2 = lpMap->Pnt[lpMap->Points-1].y;
		}
	else	{
		x2 = lpMap->Pnt[i].x;
		y2 = lpMap->Pnt[i].y;
		if ( bStretch )
			x2 = ((255L * (x2-StepMin)) + (StepMax/2)) / StepMax;
		if ( fxScale >= 0 )
			x2 = FMUL( (x2 - 128), fxScale ) + 128;
		}
	if ( fyScale >= 0 )
		y2 = FMUL( (y2 - 128), fyScale ) + 128;
	if ( x1 != x2 ) /* don't do anything if points are coincident */
		{
		m = (256L * (y2 - y1)) / (x2 - x1);
		for (x=x1; x<=x2; x++)
			{
			value = y1 + ((m * (x - x1)) / 256L);
			value -= Brightness;
			value = bound( value, 0, 255 );
			if ( lpMap->Negate )
				lpMap->Lut[x] = 255 - value;
			else	lpMap->Lut[x] = value;
			}
/*		if ( lpMap->Negate )
			ramp( lpMap->Lut, x1, 255-y1, x2, 255-y2 );
		else	ramp( lpMap->Lut, x1, y1, x2, y2 ); */
		}
	x1 = x2;
	y1 = y2;
	}

/* Posterize it (if necessary) */
if ( lpMap->Levels < 256 )
	{
	if ( lpMap->Levels < 2 )
		lpMap->Levels = 2;
	fbreakincr = FGET( 256, lpMap->Levels );
	fbreak = fbreakincr;
	brk = FMUL( 1, fbreak );
	fvalueincr = FGET( 256, lpMap->Levels-1 );
	fvalue = 0;
	value = lpMap->Lut[0];
	for (i=0; i<256; i++)
		{
		if ( i >= brk )
			{
			fvalue += fvalueincr;
			value = FMUL( 1, fvalue );
			value = lpMap->Lut[ bound( value, 0, 255 ) ];
			fbreak += fbreakincr;
			brk = FMUL( 1, fbreak );
			}
		lpMap->Lut[i] = value;
		}
	}

/* Threshold it ( if Necessary) */
if ( lpMap->Threshold )
	{
	j = TOGRAY(lpMap->Threshold );
	for (i=0; i<256; i++)
		{
		if ( lpMap->Lut[i] < j )
			lpMap->Lut[i] = 0;   /* set if should be white */
		else	lpMap->Lut[i] = 255; /* set if should be black */
		}
	}

// Invert it back to its original state
if ( lpMap->Color == LMAP || lpMap->Color == RMAP ||
     lpMap->Color == GMAP || lpMap->Color == BMAP )
	InvertMap( lpMap );
}


/***********************************************************************/
void MapRGBint( lpRed, lpGreen, lpBlue )
/***********************************************************************/
LPINT lpRed, lpGreen, lpBlue;
{
BYTE r, g, b;

r = *lpRed;
g = *lpGreen;
b = *lpBlue;
MapRGB( &r, &g, &b );
*lpRed = r;
*lpGreen = g;
*lpBlue = b;
}


/***********************************************************************/
void MapRGB( lpRed, lpGreen, lpBlue )
/***********************************************************************/
LPTR lpRed, lpGreen, lpBlue;
{
RGBS rgb;
HSLS hsl;

if (!lpHLut)
	return;
if ( HueMap.bModified || SatMap.bModified )
	{
	RGBtoHSL( *lpRed, *lpGreen, *lpBlue, &hsl );
	hsl.hue =  lpHLut[ hsl.hue ];
	hsl.sat =  lpSLut[ hsl.sat ];
	hsl.lum =  lpMLut[ hsl.lum ];
	HSLtoRGB( hsl.hue, hsl.sat, hsl.lum, &rgb );
	*lpRed   = rgb.red;
	*lpGreen = rgb.green;
	*lpBlue  = rgb.blue;
	}
else	{
	*lpRed   = lpRLut[ lpMLut[*lpRed]   ];
	*lpGreen = lpGLut[ lpMLut[*lpGreen] ];
	*lpBlue  = lpBLut[ lpMLut[*lpBlue]  ];
	}
}


/***********************************************************************/
BOOL AnimateMap( lpMap )
/***********************************************************************/
LPMAP lpMap;
{
int i;
LPPALETTEENTRY lpEntry, lpNewEntry;
PALETTEENTRY TempEntries[256+1];
BOOL bPreview;

if ( lpMap )
	MakeMap( lpMap );
if ( !lpImage )
	return( NO );
if ( !lpImage->hPal )
	return( NO );
if ( BltScreen.BitMapInfo.bmiHeader.biBitCount != 8 )
	return( NO );

bPreview = Previewing();
if (!bPreview)
	MapSetupLuts(HueMap.Lut, SatMap.Lut, MasterMap.Lut, RedMap.Lut, GreenMap.Lut, BlueMap.Lut);
lpEntry = lpImage->Palette;
lpNewEntry = TempEntries;
for ( i=0; i<lpImage->nPaletteEntries; i++ )
	{
	*lpNewEntry = *lpEntry;
	if ( !bPreview )
		MapRGB( &lpNewEntry->peRed, &lpNewEntry->peGreen,
			&lpNewEntry->peBlue );
	lpEntry++;
	lpNewEntry++;
	}

AnimatePalette( lpImage->hPal, 0, lpImage->nPaletteEntries, TempEntries );
return( YES );
}


/***********************************************************************/
void MapPreview()
/***********************************************************************/
{
BOOL bHasPalette;
RECT rMask;

if ( !lpImage )
	return;
if ( !lpImage->hWnd )
	return;
lpImage->lpDisplay->FloatingImageProc = MapPreviewProc;
bHasPalette = AnimateMap( NULL );
MapSetupLuts(HueMap.Lut, SatMap.Lut, MasterMap.Lut, RedMap.Lut, GreenMap.Lut, BlueMap.Lut);
mask_rect(&rMask);
File2DispRect(&rMask, &rMask);
AstralCursor( IDC_WAIT);
InvalidateRect(lpImage->hWnd, &rMask, FALSE);
UpdateWindow(lpImage->hWnd);
AstralCursor( NULL );
}


/***********************************************************************/
void UndoMapPreview( iFinal )
/***********************************************************************/
int iFinal;
{
BOOL bHasPalette;
RECT rMask;

if ( !lpImage )
	return;
if ( !Previewing() && iFinal != IDOK )
	return;
if ( !lpImage->hWnd )
	return;
bHasPalette = AnimateMap(NULL);
if ( !iFinal && !bHasPalette)
	lpImage->lpDisplay->FloatingImageProc = MapPreviewProc;
else
	{
	MapSetupLuts(NULL, NULL, NULL, NULL, NULL, NULL);
	lpImage->lpDisplay->FloatingImageProc = NULL;
	mask_rect(&rMask);
	File2DispRect(&rMask, &rMask);
	AstralCursor( IDC_WAIT);
	InvalidateRect(lpImage->hWnd, &rMask, FALSE);
	UpdateWindow(lpImage->hWnd);
	AstralCursor( NULL );
	}
}

/************************************************************************/
int MapPreviewProc(yline, xleft, xright, lpDst, lpSrc, lpMask)
/************************************************************************/
int yline, xleft, xright;
LPTR lpDst, lpSrc, lpMask;
{
int dx;

dx = xright-xleft+1;
if (lpMask)
	mload(xleft, yline, dx, 1, lpMask, lpDst, frame_set(NULL));
frame_mappixels(lpDst, lpMask, dx);
}

/************************************************************************/
BOOL Previewing()
/************************************************************************/
{
if (!lpImage)
	return(FALSE);
return(lpImage->lpDisplay->FloatingImageProc == MapPreviewProc);
}