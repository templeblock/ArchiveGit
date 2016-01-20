//®FD1¯®PL1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "macros.h"

//************************************************************************
//	Optimized Palette Management.
//************************************************************************

//************************************************************************
//	Cube definition
//************************************************************************

typedef struct _cube
	{
	long lCount;
	BYTE x1, y1, z1, x2, y2, z2;
	}
CUBE, far *LPCUBE;

#define VOLUME(lpc) ( (long)(lpc->x2 - lpc->x1 + 1) * \
					  (long)(lpc->y2 - lpc->y1 + 1) * \
					  (long)(lpc->z2 - lpc->z1 + 1) )

//************************************************************************
//	Static prototypes
//************************************************************************

static LPTR CreateOptimizedPalette( LPOBJECT lpObject, LPFRAME lpFrame, LPRGB lpRGBmap, LPINT lpMaxColors,
	STATUSCALLBACKPROC lpStatusCallback, BOOL bOptimize, BOOL bDither, BOOL bMinColors );
static long CreateHistogram( LPOBJECT lpObject, LPFRAME lpFrame, LPWORD lpHisto, LPRGB lpRGBmap,
	LPINT lpMaxColors, STATUSCALLBACKPROC lpStatusCallback);
static int  SplitCube(struct _cube far *lpCubeList,int nCubes);
static long SumPlaneX(WORD x, LPCUBE lpCube);
static long SumPlaneY(WORD y, LPCUBE lpCube);
static long SumPlaneZ(WORD z, LPCUBE lpCube);
static long SumCube( LPCUBE lpCube);
static int FindClosestColor( WORD wRGB, LPRGB lpRGBmap, int nCubes );

static int  FindColor(LPDWORD lpTable, int iEntries, DWORD lValue);
static void InsertColor(LPDWORD lpTable, int iEntries, DWORD lValue);
static void RGB2ColorLut(LPRGB lpRGBLut, int iEntries, LPDWORD lpColorLut);
static void Color2RGBLut(LPDWORD lpColorLut, int iEntries, LPRGB lpRGBLut);
static void ReduceFindColor( int x, int y, int dx, LPTR lpSrc, LPTR lpDst, int depth );
static void ReduceScatter( int x, int y, int dx, LPTR lpSrc, LPTR lpDst, int depth );
static void ReducePattern( int x, int y, int dx, LPTR lpSrc, LPTR lpDst, int depth );
static void IndexedFindColor( int x, int y, int dx, LPTR lpSrc, LPTR lpDst, int depth );
static void IndexedScatter( int x, int y, int dx, LPTR lpSrc, LPTR lpDst, int depth );
static void IndexedPattern( int x, int y, int dx, LPTR lpSrc, LPTR lpDst, int depth );
static void Stuff8ColorPalette( LPRGB lpRGBmap );
static void Stuff16ColorPalette( LPRGB lpRGBmap );
static void Stuff256ColorPalette( LPRGB lpRGBmap );

// Static data
static LPWORD lpHistoStart;

//static void ReduceFindColor( int x, int y, int dx, LPTR lpSrc, LPTR lpDst, int depth)
typedef void ( FAR *OPTPROC )(int, int, int, LPTR, LPTR, int );

static LPTR lpMap;
static LPRGB lpRGBmap;
static LPDWORD lpColormap;
static LPTR lpColorLut1, lpColorLut2;
static BOOL bUseFindColor;
static int nEntries;
static OPTPROC lpOptProc;
static LPINT lpErrorBuf;

//************************************************************************
int OptimizeBegin( LPOBJECT lpObject, LPFRAME lpFrame, LPRGB RGBmapIn, int nColors,
	STATUSCALLBACKPROC lpStatusCallback, BOOL bReduction, BOOL bOptimize,
	BOOL bScatter, BOOL bDither, int dx)
//************************************************************************
{
RGBS RGBmap[256];
LPRGB lpSrcRGB;
int	nMaxColors;
long lSize;
BOOL bNeedErrorBuf;

lpColorLut1 = NULL;
lpMap = NULL;
lpErrorBuf = NULL;

if (bScatter)
	bDither = NO;

if (!lpFrame)
	return(0);

nMaxColors = nColors;

if (RGBmapIn)
	lpSrcRGB = RGBmapIn;
else
	lpSrcRGB = RGBmap;

lpColorLut1 = CreateOptimizedPalette( lpObject, lpFrame, lpSrcRGB, &nMaxColors,
	lpStatusCallback, bOptimize, bDither, YES);

// if we get a NULL colorlut back, it may mean we are using
// the already had equal of fewer colors than requested
// if that is so, this must be true: 0 < nMaxColors <= nColors

if (!lpColorLut1 && (nMaxColors <= 0 || nMaxColors > nColors))
	return(0);

if (bUseFindColor = (lpColorLut1 == NULL))
	nEntries = nMaxColors;
else
	nEntries = nColors;

if (!dx) // Special case to just create a palette
	{
	OptimizeEnd();
	return(nEntries);
	}
lpColorLut2 = lpColorLut1 + 32768;

if (!(lpMap = Alloc(256L*sizeof(long))))
	{
	OptimizeEnd();
	return(0);
	}

if (bUseFindColor)
	{
	lpColormap = (LPDWORD)lpMap;
	RGB2ColorLut(lpSrcRGB, nEntries, lpColormap);
	}
else
	{
	lpRGBmap = (LPRGB)lpMap;
	copy((LPTR)lpSrcRGB, (LPTR)lpRGBmap, sizeof(RGBS)*nEntries);
	}

bNeedErrorBuf = NO;
if (bReduction)
	{
	if (bUseFindColor)
		lpOptProc = ReduceFindColor;
	else
	if (bScatter)
		{
		bNeedErrorBuf = YES;
		lpOptProc = ReduceScatter;
		}
	else
		{
		lpOptProc = ReducePattern;
		}
	}
else
	{
	if (bUseFindColor)
		lpOptProc = IndexedFindColor;
	else
	if (bScatter)
		{
		bNeedErrorBuf = YES;
		lpOptProc = IndexedScatter;
		}
	else
		{
		lpOptProc = IndexedPattern;
		}
	}

if (bNeedErrorBuf)
	{
	lSize = 3L * sizeof(int) * (dx+2); // add 2 extra error pixels
	if ( !(lpErrorBuf = (LPINT)Alloc(lSize)) )
		{
		OptimizeEnd();
		return(0);
		}
	lclr( (LPTR)lpErrorBuf, lSize );
	}

return(nEntries);
}


//************************************************************************
void OptimizeData(int x, int y, int iCount, LPTR lpIn, LPTR lpOut, int depth)
//************************************************************************
{
(*lpOptProc)(x, y, iCount, lpIn, lpOut, depth);
}


//************************************************************************
void OptimizeEnd()
//************************************************************************
{
if (lpColorLut1)
	{
	FreeUp(lpColorLut1);
	lpColorLut1 = NULL;
	}

if (lpMap)
	{
	FreeUp(lpMap);
	lpMap = NULL;
	}

if (lpErrorBuf)
	{
	FreeUp((LPTR)lpErrorBuf);
	lpErrorBuf = NULL;
	}
}


//************************************************************************
static LPTR CreateOptimizedPalette( LPOBJECT lpObject, LPFRAME lpFrame, LPRGB lpRGBmap, LPINT lpMaxColors,
	STATUSCALLBACKPROC lpStatusCallback, BOOL bOptimize, BOOL bDither, BOOL bMinColors )
//************************************************************************
{
CUBE CubeList[256], TempCube;
LPCUBE lpCube;
int i, j, r, g, b, nCubes, nMaxColors;
BYTE x, y, z;
WORD wEntry, wBestX, wBestY, wBestZ, wRGB, wCount, wIndex, wMin, wMax;
LPWORD lpwEntry;
RGBS rgb1, rgb2;
DWORD iValue, jValue;
LPRGB lpMap;
LPTR lpTable, lpErrorTable;
long lHistoCount, lCount;
LFIXED ratio;
HSLS hsl;
//BYTE S[] = { 0, 8, 16, 24 }, E[] = { 7, 15, 23, 31 };
BYTE S[] = { 0, 11, 22 }, E[] = { 10, 21, 31 };
#define INITVAL 0xFFFF

if (!lpFrame)
	return(NULL);

nMaxColors = *lpMaxColors;
if ( nMaxColors > 256 )
	nMaxColors = 256;
if ( nMaxColors < 1 )
	nMaxColors = 1;

if ( !(lpHistoStart = (LPWORD)Alloc( 32768L*2L )) )
	return( NULL );
lclr( (LPTR)lpHistoStart, 32768L*2L );

if ( bOptimize )
	{
	lHistoCount = CreateHistogram( lpObject, lpFrame, lpHistoStart,
		(bMinColors ? lpRGBmap : NULL), lpMaxColors, lpStatusCallback);

	if (lHistoCount && bMinColors && *lpMaxColors <= nMaxColors)
		{
		FreeUp((LPTR)lpHistoStart);
		return(NULL);
		}
	}
else
	{
	if ( nMaxColors == 8 ) // 8 color system palette
		{
		nCubes = 8;
		Stuff8ColorPalette( lpRGBmap );
		goto PaletteComplete;
		}
	if ( nMaxColors == 16 ) // 16 color system palette
		{
		nCubes = 16;
		Stuff16ColorPalette( lpRGBmap );
		goto PaletteComplete;
		}
	if ( nMaxColors == 256 ) // 256 color system palette
		{
		nCubes = 256;
		Stuff256ColorPalette( lpRGBmap );
		goto PaletteComplete;
		}
	lHistoCount = 0L;
	}

// Start with a single cube or multiple cubes that will be split
// until we reach the desired palette size
if ( nMaxColors < 27 )
	{ // Start with a single cube
	CubeList[0].lCount = lHistoCount;
	CubeList[0].x1 = 0; CubeList[0].x2 = 31;
	CubeList[0].y1 = 0; CubeList[0].y2 = 31;
	CubeList[0].z1 = 0; CubeList[0].z2 = 31;
	nCubes = 1;
	}
else
	{ // Start with 27 cubes (3 to a side)
	nCubes = 0;
	for ( x=0; x<3; x++ )
	for ( y=0; y<3; y++ )
	for ( z=0; z<3; z++ )
		{
		CubeList[nCubes].x1 = S[x]; CubeList[nCubes].x2 = E[x];
		CubeList[nCubes].y1 = S[y]; CubeList[nCubes].y2 = E[y];
		CubeList[nCubes].z1 = S[z]; CubeList[nCubes].z2 = E[z];
		CubeList[nCubes].lCount = SumCube( &CubeList[nCubes] );
		nCubes++;
		}
	}

while ( nCubes < nMaxColors )
	{
	if ( !SplitCube( CubeList, nCubes ) )
		break;
	nCubes++;
	}

// Sort the cubes by its average HSL value (just to organize it)
for ( i=0; i<nCubes; i++ )
	{
	lpCube = &CubeList[i];
	x = ( lpCube->x1 + lpCube->x2 )<<2;
	y = ( lpCube->y1 + lpCube->y2 )<<2;
	z = ( lpCube->z1 + lpCube->z2 )<<2;
	RGBtoHSL( x, y, z, &hsl );
	iValue = (((DWORD) (hsl.lum << 8 | hsl.sat) << 8) | hsl.hue);

	for ( j=i+1; j<nCubes; j++ )
		{
		lpCube = &CubeList[j];
		x = ( lpCube->x1 + lpCube->x2 )<<2;
		y = ( lpCube->y1 + lpCube->y2 )<<2;
		z = ( lpCube->z1 + lpCube->z2 )<<2;
		RGBtoHSL( x, y, z, &hsl );
		jValue = (((DWORD) (hsl.lum << 8 | hsl.sat) << 8) | hsl.hue);
		if ( jValue < iValue )
			{
			iValue = jValue;
			TempCube = CubeList[i];
			CubeList[i] = CubeList[j];
			CubeList[j] = TempCube;
			}
		}
	}

// Go through each cube entry, compute a weighted RGB
// average for the cube and stuff it into the RGBmap

lpCube = CubeList;
lpMap = lpRGBmap;

for ( i=0; i<nCubes; i++ )
	{
	lCount = lpCube->lCount;
	wBestX = wBestY = wBestZ = 0;

	for ( x=lpCube->x1; x<=lpCube->x2; x++ )
	for ( y=lpCube->y1; y<=lpCube->y2; y++ )
	for ( z=lpCube->z1; z<=lpCube->z2; z++ )
		{
		lpwEntry = lpHistoStart + ToMiniRGB( x, y, z );
		wEntry = *lpwEntry;
		*lpwEntry = i;
		if ( wEntry && lCount )
			{
			ratio = FGET( wEntry, lCount );
			wBestX += FMUL( ToMaxi[x], ratio );
			wBestY += FMUL( ToMaxi[y], ratio );
			wBestZ += FMUL( ToMaxi[z], ratio );
			}
		}

	if ( !lCount ) // If it's an empty cube...
		{
		lpMap->red   = ( lpCube->x1 + lpCube->x2 ) << 2;
		lpMap->green = ( lpCube->y1 + lpCube->y2 ) << 2;
		lpMap->blue  = ( lpCube->z1 + lpCube->z2 ) << 2;
		lpMap++;
		}
	else
		{ // don't let value go out of range of cube
		wMin = ToMaxi[lpCube->x1];
		wMax = ToMaxi[lpCube->x2];
		if (wBestX < wMin)	wBestX = wMin; else
		if (wBestX > wMax)	wBestX = wMax;

		wMin = ToMaxi[lpCube->y1];
		wMax = ToMaxi[lpCube->y2];
		if (wBestY < wMin)	wBestY = wMin; else
		if (wBestY > wMax)	wBestY = wMax;

		wMin = ToMaxi[lpCube->z1];
		wMax = ToMaxi[lpCube->z2];
		if (wBestZ < wMin)	wBestZ = wMin; else
		if (wBestZ > wMax)	wBestZ = wMax;

		lpMap->red   = wBestX;
		lpMap->green = wBestY;
		lpMap->blue  = wBestZ;
		lpMap++;
		}

	lpCube++;
	}

PaletteComplete:
// Our palette is now complete.  Time to create the 32K pixel remapping table
// Use the histogram buffer as a table lookup; stuff it with an initial value
lpwEntry = lpHistoStart;
for ( wCount=0; wCount<=32767; wCount++ )
	*lpwEntry++ = INITVAL; // the initial value

wCount = 32768; // the number of table entries left to set
j = -1; // the amount of growth around each palette color

// Loop until we have no more entries to set
while ( wCount )
	{
	// Seed the table with the palette we just created; grow a cube
	// around each color it until all 32k values are set
	j++;
	for ( i=0; i<nCubes; i++ )
		{
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

		for ( x=rgb1.red;   x<=rgb2.red;   x++ )
		for ( y=rgb1.green; y<=rgb2.green; y++ )
		for ( z=rgb1.blue;  z<=rgb2.blue;  z++ )
			{
			lpwEntry = lpHistoStart + ToMiniRGB( x, y, z );
			if ( *lpwEntry != INITVAL ) // if it's already stuffed, move along
				continue;
			// if we haven't visited this entry yet...
			*lpwEntry = i; // put in the color index
			if ( !(--wCount) )
				goto Cont;
			if (lpStatusCallback && (wCount&0x00fff))
				{
					(*lpStatusCallback)((int)(16384-(wCount>>2L)), (int)16384, NO);
				}
			}
		}
	}

Cont:
// Convert the table lookup words into table lookup bytes
lpTable = (LPTR)lpHistoStart;
lpwEntry = lpHistoStart;
for ( wCount=0; wCount<=32767; wCount++ )
	*lpTable++ = *lpwEntry++;

// Add an error palette to the end for pattern dithering
lpTable = (LPTR)lpHistoStart;
lpErrorTable = lpTable + 32768;
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
		rgb1.red   = mbound( r, 0, 255 );
		rgb1.green = mbound( g, 0, 255 );
		rgb1.blue  = mbound( b, 0, 255 );

		// convert to MiniRGB to create a new table index
		wIndex = lpTable[ RGBtoMiniRGB(&rgb1) ];
		}

	lpErrorTable[wRGB] = wIndex;
	}

return( (LPTR)lpHistoStart );
}


//************************************************************************
static long CreateHistogram( LPOBJECT lpObject, LPFRAME lpFrame, LPWORD lpHisto, LPRGB lpRGBmap,
		LPINT lpMaxColors, STATUSCALLBACKPROC lpStatusCallback)
//************************************************************************
{
long lHistoCount;
LPTR lpLine;
RGBS rgb;
WORD wRGB;
LPWORD lpwEntry;
int x, y, depth, iColors, nMaxColors;
DWORD lmap[256];
LPDWORD lpMap;
DWORD pixel, last;
int iHistoColors;
LPTR lpImgScanline;

lpImgScanline = NULL;

if (!lpFrame)
	return( 0L );

if ( !lpHisto )
	return( 0L );

nMaxColors = *lpMaxColors;
lHistoCount = 0;
depth = FrameDepth(lpFrame);
iColors = 0;
iHistoColors = 0;
last = 0xFFFFFFFFL;

AllocLines(&lpImgScanline, 1, FrameXSize(lpFrame), depth );

if (!lpImgScanline)
	return( 0L );

for (y = 0; y < FrameYSize(lpFrame); ++y)
	{
	if (lpStatusCallback)
		{
		(*lpStatusCallback)(y, (int)(FrameYSize(lpFrame)*2), NO);
		}

	if (lpObject)
	{
		if (!ImgGetLine( NULL, lpObject, 0, y,
			FrameXSize(lpFrame), lpImgScanline))
			continue;

		lpLine = lpImgScanline;
	}
	else
	{
		if ( !(lpLine = FramePointer(lpFrame, 0, y, NO)) )
			continue;
	}

	for ( x = 0; x < FrameXSize(lpFrame); ++x, lpLine += depth )
		{
		frame_getRGB( lpLine, depth, &rgb );
		if (lpRGBmap && iColors <= nMaxColors)
			{
			pixel = 0;
			CopyRGB(&rgb, &pixel);
			if (pixel != last && FindColor(lmap, iColors, pixel) < 0)
				{
				if (iColors < 256)
					InsertColor(lmap, iColors, pixel);
				++iColors;
				}
			last = pixel;
			}

		wRGB = RGBtoMiniRGB( &rgb );
		lpwEntry = lpHisto + wRGB;
		if ( *lpwEntry >= 65535 )
			continue;
		if ( !(*lpwEntry) )
			++iHistoColors;
		*lpwEntry += 1;
		lHistoCount++;
		}
	}

if (lpRGBmap)
	{
	*lpMaxColors = iColors;
	if (iColors <= nMaxColors)
		{
		Color2RGBLut(lmap, iColors, lpRGBmap);
		lpMap = lmap;
		while (--iColors >= 0)
			{
			CopyRGB(lpMap, lpRGBmap);
			++lpMap;
			++lpRGBmap;
			}
		}
	}

if (lpImgScanline)
	FreeUp( lpImgScanline );

return( lHistoCount );
}


//************************************************************************
static BOOL SplitCube(LPCUBE lpCubeList, int nCubes)
//************************************************************************
{
int i, xDelta, yDelta, zDelta, MaxDelta;
WORD p;
LPCUBE lpCube, lpLargestCube;
long lCount, lLargestCount, lTargetCount;
CUBE NewCube;

/* Find the largest subcube in the list; that's the one we'll split */
i = nCubes;
lpCube = lpCubeList;
lLargestCount = -1;
lpLargestCube = lpCube;

while ( --i >= 0 )
	{
	if ( lpCube->lCount > lLargestCount ||
	   ( lpCube->lCount == lLargestCount &&
		VOLUME(lpCube) > VOLUME(lpLargestCube) ) )
		if ( lpCube->x2 > lpCube->x1 ||
			lpCube->y2 > lpCube->y1 ||
			lpCube->z2 > lpCube->z1 )
			{ // There must be something to split
			lpLargestCube = lpCube;
			lLargestCount = lpCube->lCount;
			}
	lpCube++;
	}

/* Find the longest dimension of the subcube; we'll split along that side */
xDelta = lpLargestCube->x2 - lpLargestCube->x1;
yDelta = lpLargestCube->y2 - lpLargestCube->y1;
zDelta = lpLargestCube->z2 - lpLargestCube->z1;

Repeat:
MaxDelta = max( xDelta, max( yDelta, zDelta ) );
if ( MaxDelta <= 0 ) // The cube is a single entry - this should never happen
	return( NO );

/* Sum the counts of each plane referenced by the subcube */
/* When the sum is greater than half the total subcube count, */
/* then stop; that's the point where we split the cube into 2 cubes */
lTargetCount = lLargestCount/2;
lCount = 0;
NewCube = *lpLargestCube; /* Make a copy of the cube we're going to split */

if ( !lLargestCount )
	{ // If this is an empty cube, split it down the middle
	if ( MaxDelta == xDelta )
		{
		p = (WORD)( lpLargestCube->x1 + lpLargestCube->x2 ) / 2;
		NewCube.x2 = p;
		lpLargestCube->x1 = p+1;
		}
	else
	if ( MaxDelta == yDelta )
		{
		p = (WORD)( lpLargestCube->y1 + lpLargestCube->y2 ) / 2;
		NewCube.y2 = p;
		lpLargestCube->y1 = p+1;
		}
	else
	if ( MaxDelta == zDelta )
		{
		p = (WORD)( lpLargestCube->z1 + lpLargestCube->z2 ) / 2;
		NewCube.z2 = p;
		lpLargestCube->z1 = p+1;
		}
	}
else
if ( MaxDelta == xDelta )
	{
	p = lpLargestCube->x1;
	while (1)
		{
		lCount += SumPlaneX( p, lpLargestCube );
		if ( lCount >= lTargetCount )
			break;
		if ( (p+1) >= lpLargestCube->x2 )
			break;
		p++;
		}
	if ( !lCount )
		{ // We won't create empty cubes, so split it another way
		xDelta = -1;
		if ( yDelta > 0 || zDelta > 0 )
			goto Repeat;
		}
	NewCube.x2 = p;
	lpLargestCube->x1 = p+1;
	}
else
if ( MaxDelta == yDelta )
	{
	p = lpLargestCube->y1;
	while (1)
		{
		lCount += SumPlaneY( p, lpLargestCube );
		if ( lCount >= lTargetCount )
			break;
		if ( (p+1) >= lpLargestCube->y2 )
			break;
		p++;
		}
	if ( !lCount )
		{ // We won't create empty cubes, so split it another way
		yDelta = -1;
		if ( xDelta > 0 || zDelta > 0 )
			goto Repeat;
		}
	NewCube.y2 = p;
	lpLargestCube->y1 = p+1;
	}
else
//if ( MaxDelta == zDelta )
	{
	p = lpLargestCube->z1;
	while (1)
		{
		lCount += SumPlaneZ( p, lpLargestCube );
		if ( lCount >= lTargetCount )
			break;
		if ( (p+1) >= lpLargestCube->z2 )
			break;
		p++;
		}
	if ( !lCount )
		{ // We won't create empty cubes, so split it another way
		zDelta = -1;
		if ( xDelta > 0 || yDelta > 0 )
			goto Repeat;
		}
	NewCube.z2 = p;
	lpLargestCube->z1 = p+1;
	}

/* Make the largest cube smaller */
lpLargestCube->lCount -= lCount;

/* Create a new cube to add to the list */
NewCube.lCount = lCount;

/* Copy the new cube to the end of the list */
lpCubeList[nCubes] = NewCube;

return( YES );
}


//************************************************************************
static long SumPlaneX(WORD x, LPCUBE lpCube)
//************************************************************************
{
long lCount;
WORD y, z;

lCount = 0;
for ( y=lpCube->y1; y<=lpCube->y2; y++ )
	for ( z=lpCube->z1; z<=lpCube->z2; z++ )
		lCount += *(lpHistoStart + ToMiniRGB( x, y, z ));

return( lCount );
}


//************************************************************************
static long SumPlaneY(WORD y, LPCUBE lpCube)
//************************************************************************
{
long lCount;
WORD x, z;

lCount = 0;
for ( x=lpCube->x1; x<=lpCube->x2; x++ )
	for ( z=lpCube->z1; z<=lpCube->z2; z++ )
		lCount += *(lpHistoStart + ToMiniRGB( x, y, z ));

return( lCount );
}


//************************************************************************
static long SumPlaneZ(WORD z, LPCUBE lpCube)
//************************************************************************
{
long lCount;
WORD x, y;

lCount = 0;
for ( x=lpCube->x1; x<=lpCube->x2; x++ )
	for ( y=lpCube->y1; y<=lpCube->y2; y++ )
		lCount += *(lpHistoStart + ToMiniRGB( x, y, z ));

return( lCount );
}


/***********************************************************************/
static long SumCube( LPCUBE lpCube )
/***********************************************************************/
{
long lCount;
WORD x, y, z;

lCount = 0;
for ( x=lpCube->x1; x<=lpCube->x2; x++ )
for ( y=lpCube->y1; y<=lpCube->y2; y++ )
for ( z=lpCube->z1; z<=lpCube->z2; z++ )
	lCount += *(lpHistoStart + ToMiniRGB( x, y, z ));
return( lCount );
}


//************************************************************************
static int FindClosestColor( WORD wRGB, LPRGB lpRGBmap, int iEntries )
//************************************************************************
{
RGBS rgb, rgbNew;
int i, index, delta;
DWORD d, distance;

rgb.red   = MaxiR( wRGB );
rgb.green = MaxiG( wRGB );
rgb.blue  = MaxiB( wRGB );
index = 0;
distance = 0x7FFFFFFFL; // Big number
for ( i=0; i<iEntries; i++ )
	{
	rgbNew = *lpRGBmap++;
	d = 0;
	delta = (int)rgb.red   - rgbNew.red;   d += (long)delta * delta;
	delta = (int)rgb.green - rgbNew.green; d += (long)delta * delta;
	delta = (int)rgb.blue  - rgbNew.blue;  d += (long)delta * delta;
	if ( d < distance )
		{
		distance = d;
		index = i;
		if ( !distance )
			break;
		}
	}

return( index );
}


//************************************************************************
static void ReduceFindColor( int x, int y, int dx, LPTR lpSrc, LPTR lpDst,
	int depth)
//************************************************************************
{
	DWORD pixel;
	RGBS  rgb;
	int   index;

	switch(depth)
	{
		case 0 :
		case 1 :
		break;

		case 3 :
			while ( --dx >= 0 )
			{
				// get RGB from source
				frame_getpixel( lpSrc, depth, &pixel );
				if ( (index = FindColor(lpColormap, nEntries, pixel)) < 0 )
					index = 0;
				frame_putpixel( lpDst, depth, *(lpColormap+index) );
				lpSrc += depth;
				lpDst += depth;
			}
		break;

		case 4 :
			while ( --dx >= 0 )
			{
				// get RGB from source
				frame_getRGB( lpSrc, depth, &rgb );
				pixel = 0;
				CopyRGB( &rgb, &pixel );

				if ( (index = FindColor(lpColormap, nEntries, pixel)) < 0 )
					index = 0;

				CopyRGB( (lpColormap+index), &rgb );
				frame_putRGB( lpDst, depth, &rgb );

				lpSrc += 4;
				lpDst += 4;
			}
		break;
	}
}

//************************************************************************
static void ReduceScatter( int x, int y, int dx, LPTR lpSrc, LPTR lpDst,
	int depth )
//************************************************************************
{
RGBS rgb;
int	r, g, b, rError, gError, bError;
BYTE Pixel;
LPINT lpError;
LPRGB lpNewPixel;

lpError = lpErrorBuf + 3;
rError = *(lpError+0);
gError = *(lpError+1);
bError = *(lpError+2);
while ( --dx >= 0 )
	{
	// get RGB from source
	frame_getRGB( lpSrc, depth, &rgb );

	// Add in the last error values
	r = rgb.red   + rError;
	g = rgb.green + gError;
	b = rgb.blue  + bError;

	// Bound the new pixel value
	rgb.red   = mbound( r, 0, 255 );
	rgb.green = mbound( g, 0, 255 );
	rgb.blue  = mbound( b, 0, 255 );

	// convert to a Mini & lookup it up in ColorLut for actual pixel value
	Pixel = *( lpColorLut1 + RGBtoMiniRGB(&rgb) );

	// now store RGB corresponding to the pixel value in destination
	lpNewPixel = lpRGBmap + Pixel;
	frame_putRGB( lpDst, depth, lpNewPixel );

	// calculate the error to sub-divide
	r -= (int)lpNewPixel->red;
	g -= (int)lpNewPixel->green;
	b -= (int)lpNewPixel->blue;

	// Save the error to the right; I'll need for the next pixel
	rError = *(lpError+3);
	gError = *(lpError+4);
	bError = *(lpError+5);

	*(lpError-3) += ( r * 19 ) / 64;
	*(lpError-2) += ( g * 19 ) / 64;
	*(lpError-1) += ( b * 19 ) / 64;
	*(lpError+0)  = ( r * 26 ) / 64;
	*(lpError+1)  = ( g * 26 ) / 64;
	*(lpError+2)  = ( b * 26 ) / 64;
	*(lpError+3) += ( r * 19 ) / 64;
	*(lpError+4) += ( g * 19 ) / 64;
	*(lpError+5) += ( b * 19 ) / 64;
	lpError += 3;

	lpSrc += depth;
	lpDst += depth;
	}
}


//************************************************************************
static void ReducePattern( int x, int y, int dx, LPTR lpSrc, LPTR lpDst,
	int depth )
//************************************************************************
{
RGBS rgb;
WORD wRGB;
BYTE Pixel;

while ( --dx >= 0 )
	{
	frame_getRGB( lpSrc, depth, &rgb );
	wRGB = RGBtoMiniRGB( &rgb );

	if ((x ^ y) & 1)
		Pixel = *( lpColorLut2 + wRGB );
	else
		Pixel = *( lpColorLut1 + wRGB );

	frame_putRGB( lpDst, depth, lpRGBmap+Pixel );
	lpSrc += depth;
	lpDst += depth;
	++x;
	}
}


//************************************************************************
static void IndexedFindColor( int x, int y, int dx, LPTR lpSrc, LPTR lpDst,
	int depth)
//************************************************************************
{
	DWORD pixel;
	RGBS  rgb;
	int   index;

	switch(depth)
	{
		case 0 :
		case 1 :

		case 3 :
			while ( --dx >= 0 )
			{
				// get RGB from source
				frame_getpixel( lpSrc, depth, &pixel );
				if ((index = FindColor(lpColormap, nEntries, pixel)) < 0)
					*lpDst++ = 0;
				else
					*lpDst++ = index;
				lpSrc += depth;
			}
		break;

		case 4 :
			while ( --dx >= 0 )
			{
				// get RGB from source
				frame_getRGB( lpSrc, depth, &rgb );
				pixel = 0;
				CopyRGB( &rgb, &pixel );

				if ( (index = FindColor(lpColormap, nEntries, pixel)) < 0 )
					*lpDst++ = 0;
				else
					*lpDst++ = index;
				lpSrc += 4;
			}
		break;
	}
}


//************************************************************************
static void IndexedScatter( int x, int y, int dx, LPTR lpSrc, LPTR lpDst,
	int depth )
//************************************************************************
{
static long lSeed = 0xDEADBABA;
RGBS rgb;
int	r, g, b, rError, gError, bError;
BYTE Pixel;
LPINT lpError;
LPRGB lpNewPixel;

lpError = lpErrorBuf;
while ( --dx >= 0 )
	{
	// get RGB from source
	frame_getRGB( lpSrc, depth, &rgb );

	// Add in the error values
	r = rgb.red   + *(lpError+0);
	g = rgb.green + *(lpError+1);
	b = rgb.blue  + *(lpError+2);

	// Bound the new pixel value
	rgb.red   = mbound( r, 0, 255 );
	rgb.green = mbound( g, 0, 255 );
	rgb.blue  = mbound( b, 0, 255 );

	// convert to a Mini & lookup it up in ColorLut for actual pixel value
	Pixel = *( lpColorLut1 + RGBtoMiniRGB(&rgb) );

	// now store RGB corresponding to the pixel value in destination
	*lpDst++ = Pixel;
	lpNewPixel = lpRGBmap + Pixel;

	// calculate the error (don't pass on cumulative error)
	r -= (int)lpNewPixel->red;
	g -= (int)lpNewPixel->green;
	b -= (int)lpNewPixel->blue;
	rError = mbound( r, -64, 64 );
	gError = mbound( g, -64, 64 );
	bError = mbound( b, -64, 64 );

	// either pass error to the right or pass it down to next line
	if ( lSeed & BIT18 )
		{ // Pass the error right
		lSeed += lSeed;
		lSeed ^= BITMASK;

		// Pass nothing down (leave it for next loop)
		*lpError++ = 0;
		*lpError++ = 0;
		*lpError++ = 0;
		*(lpError+0) += rError;
		*(lpError+1) += gError;
		*(lpError+2) += bError;
		}
	else
		{ // Pass the error down to the next line
		lSeed += lSeed;
		*lpError++ = rError;
		*lpError++ = gError;
		*lpError++ = bError;

		// Pass nothing right (leave it for next line)
		}

	lpSrc += depth;
	}
}


//************************************************************************
static void IndexedPattern( int x, int y, int dx, LPTR lpSrc, LPTR lpDst,
	int depth )
//************************************************************************
{
RGBS rgb;
WORD wRGB;

while ( --dx >= 0 )
	{
	frame_getRGB( lpSrc, depth, &rgb );
	wRGB = RGBtoMiniRGB( &rgb );

	if ((x ^ y) & 1)
		*lpDst++ = *( lpColorLut2 + wRGB );
	else
		*lpDst++ = *( lpColorLut1 + wRGB );

	lpSrc += depth;
	++x;
	}
}


//************************************************************************
static void InsertColor(LPDWORD lpTable, int iEntries, DWORD lValue)
//************************************************************************
{
LPDWORD lp;
DWORD lTemp;

lp = lpTable;
while (--iEntries >= 0)
	{
	if (lValue < *lp)
		break;
	++lp;
	}

if (iEntries < 0)
	{
	*lp = lValue;
	return;
	}

iEntries += 2;

while (--iEntries >= 0)
	{
	lTemp = *lp;
	*lp++ = lValue;
	lValue = lTemp;
	}
}


//************************************************************************
static int FindColor(LPDWORD lpTable, int iEntries, DWORD lValue)
//************************************************************************
{
int low, high, mid;

low = 0;
high = iEntries - 1;

while (low <= high)
	{
	mid = (low+high)>>1;
	if (lValue < lpTable[mid])
		high = mid - 1;
	else
	if (lValue > lpTable[mid])
		low = mid + 1;
	else
		return(mid);
	}

return(-1);
}


//************************************************************************
static void Color2RGBLut(LPDWORD lpColorLut, int iEntries, LPRGB lpRGBLut)
//************************************************************************
{
while (--iEntries >= 0)
	{
	CopyRGB(lpColorLut, lpRGBLut);
	++lpColorLut;
	++lpRGBLut;
	}
}


//************************************************************************
static void RGB2ColorLut(LPRGB lpRGBLut, int iEntries, LPDWORD lpColorLut)
//************************************************************************
{
while (--iEntries >= 0)
	{
	*lpColorLut = 0;
	CopyRGB(lpRGBLut, lpColorLut);
	++lpColorLut;
	++lpRGBLut;
	}
}


//************************************************************************
int CountColors( LPOBJECT lpObject, LPFRAME lpFrame, LPRGB lpRGBmap, LPRGB lpRGBData, int iLength,
	STATUSCALLBACKPROC lpStatusCallback )
//************************************************************************
{
int	iColors, y, iCount, YSize;
DWORD lmap[256], pixel, last;
int	depth;
LPTR	lp, lpImgScanline;
LPDWORD lpMap;

if ((!lpFrame) && (!lpRGBData))
	return(0);

if (lpFrame)
	{
	depth = FrameDepth( lpFrame );
	YSize = FrameYSize( lpFrame );
	}
else
	{
	depth = 3;
	YSize = 1;
	}

iColors = 0;
last = 0xFFFFFFFFL;

AllocLines(&lpImgScanline, 1, FrameXSize(lpFrame), depth);

if (!lpImgScanline)
	return( 0 );

for (y = 0; y < YSize; ++y)
	{
	if ( lpStatusCallback )
		{
		if (!(*lpStatusCallback)(y, YSize, YES))
			{
			goto Exit;
			}
		}

	if (lpFrame)
		{
		if (lpObject)
			{
			if (!ImgGetLine( NULL, lpObject, 0, y,
				FrameXSize(lpFrame), lpImgScanline))
				continue;

			lp = lpImgScanline;
			}
		else
			{
			if ( !(lp = FramePointer(lpFrame, 0, y, NO)) )
				continue;
			}

		iCount = FrameXSize(lpFrame);
		}
	else
		{
		lp = (LPTR)lpRGBData;
		iCount = iLength;
		}

	while (--iCount >= 0)
		{
		frame_getpixel(lp, depth, &pixel);
		lp += depth;

		if (pixel != last && FindColor(lmap, iColors, pixel) < 0)
			{
			if (iColors < 256)
				InsertColor(lmap, iColors, pixel);
			++iColors;
			if (iColors > 256)
				goto Exit;
			}
		last = pixel;
		}
	}
Exit:
if (lpRGBmap)
	{
	iCount = iColors;
	if (iCount > 256)
		iCount = 256;
	lpMap = lmap;

	while (--iCount >= 0)
		{
		frame_getRGB(lpMap, depth, lpRGBmap);
		++lpMap;
		++lpRGBmap;
		}
	}

if (lpImgScanline)
	FreeUp( lpImgScanline );

return(iColors);
}


#define STUFFRGB( lpRGB, r, g, b ) \
	{ (lpRGB)->red=r; (lpRGB)->green=g; (lpRGB)->blue=b; }

//************************************************************************
static void Stuff8ColorPalette( LPRGB lpRGB )
//************************************************************************
{
STUFFRGB( lpRGB,   0,   0,   0 ); lpRGB++;
STUFFRGB( lpRGB, 255,   0,   0 ); lpRGB++;
STUFFRGB( lpRGB,   0, 255,   0 ); lpRGB++;
STUFFRGB( lpRGB, 255, 255,   0 ); lpRGB++;
STUFFRGB( lpRGB,   0,   0, 255 ); lpRGB++;
STUFFRGB( lpRGB, 255,   0, 255 ); lpRGB++;
STUFFRGB( lpRGB,   0, 255, 255 ); lpRGB++;
STUFFRGB( lpRGB, 255, 255, 255 );
}


//************************************************************************
static void Stuff16ColorPalette( LPRGB lpRGB )
//************************************************************************
{
STUFFRGB( lpRGB,   0,   0,   0 ); lpRGB++;
STUFFRGB( lpRGB, 128,   0,   0 ); lpRGB++;
STUFFRGB( lpRGB,   0, 128,   0 ); lpRGB++;
STUFFRGB( lpRGB, 128, 128,   0 ); lpRGB++;
STUFFRGB( lpRGB,   0,   0, 128 ); lpRGB++;
STUFFRGB( lpRGB, 128,   0, 128 ); lpRGB++;
STUFFRGB( lpRGB,   0, 128, 128 ); lpRGB++;
STUFFRGB( lpRGB, 192, 192, 192 ); lpRGB++;
STUFFRGB( lpRGB, 128, 128, 128 ); lpRGB++;
STUFFRGB( lpRGB, 255,   0,   0 ); lpRGB++;
STUFFRGB( lpRGB,   0, 255,   0 ); lpRGB++;
STUFFRGB( lpRGB, 255, 255,   0 ); lpRGB++;
STUFFRGB( lpRGB,   0,   0, 255 ); lpRGB++;
STUFFRGB( lpRGB, 255,   0, 255 ); lpRGB++;
STUFFRGB( lpRGB,   0, 255, 255 ); lpRGB++;
STUFFRGB( lpRGB, 255, 255, 255 );
}


//************************************************************************
static void Stuff256ColorPalette( LPRGB lpRGB )
//************************************************************************
{
int r, g, b, w;

// Write out 216 colors
for ( r=0; r<6; r++ )
for ( g=0; g<6; g++ )
for ( b=0; b<6; b++ )
	{
	STUFFRGB( lpRGB, r * 51, g * 51, b * 51 ); lpRGB++;
	}

// Write out 40 grays
for ( w=0; w<40; w++ )
	{
	g = min(w * 40, 255);
	STUFFRGB( lpRGB, g, g, g ); lpRGB++;
	}
}
