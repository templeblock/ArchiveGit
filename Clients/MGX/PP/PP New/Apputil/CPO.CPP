//®FD1¯®PL1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "stdafx.h"
#include "cpo.h"
#include "getline.h"

//************************************************************************
//	Optimized Palette Management.
//************************************************************************


//************************************************************************
//	Cube definition
//************************************************************************

typedef struct _cube
	{
	long lCount;
	long lPixelCount;
	BYTE x1, y1, z1, x2, y2, z2;
	}
CUBE, far *LPCUBE;

#define VOLUME(lpc) ( (long)(lpc->x2 - lpc->x1 + 1) * \
					  (long)(lpc->y2 - lpc->y1 + 1) * \
					  (long)(lpc->z2 - lpc->z1 + 1) )

//************************************************************************
//	Static prototypes
//************************************************************************

LOCAL long CreateHistogram(LPOBJECT lpObject, LPFRAME lpFrame, LPWORD lpHisto,
	   						STATUSCALLBACKPROC lpStatusCallback);
LOCAL int CreateBestPalette(LPWORD lpHistoStart, int nMaxColors, LPRGB lpRGBmap);
LOCAL InitCubeList(LPCUBE CubeList, int nMaxColors, LPWORD lpHistStart);
LOCAL void SortPalette(LPRGB lpRGBmap, int nEntries);
LOCAL void ComputePalette(LPCUBE lpCube, LPRGB lpMap, int nCubes, LPWORD lpHistoStart) ;
LOCAL int  SplitCube(struct _cube far *lpCubeList,int nCubes, LPWORD lpHisto, BOOL fByVolume);
LOCAL LPCUBE FindBiggestVolume( LPCUBE lpCubeList, int nCubes );
LOCAL LPCUBE FindBiggestColorCount( LPCUBE lpCubeList, int nCubes );
LOCAL LPCUBE FindBiggestPixelCount( LPCUBE lpCubeList, int nCubes );
LOCAL void MedianCut( LPCUBE lpLargestCube, LPCUBE lpNewCube );
LOCAL void MedianCountCut( LPCUBE lpLargestCube, LPCUBE lpNewCube, LPWORD lpHisto );
LOCAL long SumPlaneX(WORD x, LPCUBE lpCube, LPWORD lpHisto);
LOCAL long SumPlaneY(WORD y, LPCUBE lpCube, LPWORD lpHisto);
LOCAL long SumPlaneZ(WORD z, LPCUBE lpCube, LPWORD lpHisto);
LOCAL void SumCube( LPCUBE lpCube, LPWORD lpHisto);

LOCAL void Stuff8ColorPalette( LPRGB lpRGBmap );
LOCAL void Stuff16ColorPalette( LPRGB lpRGBmap );
LOCAL void Stuff256ColorPalette( LPRGB lpRGBmap );

//************************************************************************
//************************************************************************
BOOL CreateOptimizedPalette(LPOBJECT lpObject, LPFRAME lpFrame,
							LPRGB lpRGBmap, LPINT lpMaxColors,
							BOOL bOptimize,
							STATUSCALLBACKPROC lpStatusCallback)
//************************************************************************
{
int nMaxColors;
int nCubes;
LPWORD lpHistoStart;
long lHistoCount;

if (!lpFrame)
	return(FALSE);

nMaxColors = *lpMaxColors;
if ( nMaxColors > 256 )
	nMaxColors = 256;
if ( nMaxColors < 1 )
	nMaxColors = 1;

if ( !(lpHistoStart = (LPWORD)Alloc( 32768L*2L )) )
	return( FALSE );
hclr( (LPTR)lpHistoStart, 32768L*2L );

if ( bOptimize )
	{
	lHistoCount = CreateHistogram( lpObject, lpFrame, lpHistoStart, lpStatusCallback);
	nCubes = CreateBestPalette(lpHistoStart, nMaxColors, lpRGBmap);
	}
else
	{
	if ( nMaxColors == 8 ) // 8 color system palette
		{
		nCubes = 8;
		Stuff8ColorPalette( lpRGBmap );
		}
	else
	if ( nMaxColors == 16 ) // 16 color system palette
		{
		nCubes = 16;
		Stuff16ColorPalette( lpRGBmap );
		}
	else
	if ( nMaxColors == 256 ) // 256 color system palette
		{
		nCubes = 256;
		Stuff256ColorPalette( lpRGBmap );
		}
	else
		nCubes = CreateBestPalette(lpHistoStart, nMaxColors, lpRGBmap);
	}

// don't need histogram anymore
FreeUp((LPTR)lpHistoStart);

// set return color counts
*lpMaxColors = nCubes;

// return how many entries we create in palettes
return( nCubes > 0 );
}

//************************************************************************
//************************************************************************
BOOL ReducePalette(LPRGB lpRGBmap, int nColors,
					LPRGB lpNewRGBmap, int nNewColors)
//************************************************************************
{
int nCubes, i;
LPWORD lpHistoStart;
LPRGB lpRGB;
WORD wRGB;

if ( !(lpHistoStart = (LPWORD)Alloc( 32768L*2L )) )
	return( FALSE );
hclr( (LPTR)lpHistoStart, 32768L*2L );

// find what colors are in our palette
for (i = 0; i < nColors; ++i)
	{
	lpRGB = &lpRGBmap[i];
	wRGB = RGBtoMiniRGB(lpRGB);
	lpHistoStart[wRGB] += 1;
	}

// compute new palette
nCubes = CreateBestPalette(lpHistoStart, nNewColors, lpNewRGBmap);

// don't need histogram anymore
FreeUp((LPTR)lpHistoStart);

// return how many entries we create in palettes
return( nCubes > 0 );
}

#ifdef UNUSED
// Our palette is now complete.  Time to create the 32K pixel remapping table
// Use the histogram buffer as a table lookup; stuff it with an initial value
lpTable = CreatePaletteLut15(lpRGBmap, nCubes, (LPTR)lpHistoStart, lpStatusCallback);
if (!lpTable)
	{
	FreeUp((LPTR)lpHistoStart);
	return(NULL);
	}

// Add an error palette to the end for pattern dithering
CreateErrorLut15(lpRGBmap, lpTable, lpTable+32768L, bDither);
#endif

//************************************************************************
LOCAL int CreateBestPalette(LPWORD lpHistoStart,
							int nMaxColors, LPRGB lpRGBmap)
//************************************************************************
{
LPCUBE CubeList;
int nCubes;

// allocate memory for cube list
CubeList = (LPCUBE)Alloc((long)sizeof(CUBE)*nMaxColors);
if (!CubeList)
	return(-1);

// initialize some cubes that we always start with
// at least one big one encompassing the entire color space
nCubes = InitCubeList(CubeList, nMaxColors, lpHistoStart);

// original idea was to split half of cubes by volume
// works better to split all by count
//int nHalfMaxColors = nMaxColors / 2;
// split cubes until we get enough for the image palette
while ( nCubes < nMaxColors )
	{
	if ( !SplitCube( CubeList, nCubes, lpHistoStart, FALSE /*nCubes > nHalfMaxColors*/ ) )
		break;
	nCubes++;
	}

// Go through each cube entry, compute a weighted RGB
// average for the cube and stuff it into the RGBmap
ComputePalette(CubeList, lpRGBmap, nCubes, lpHistoStart);

// sort the palette by HSL
SortPalette(lpRGBmap, nCubes);

return(nCubes);
}

//************************************************************************
LOCAL int InitCubeList(LPCUBE CubeList, int nMaxColors, LPWORD lpHistoStart)
//***********************************************************************
{
//BYTE S[] = { 0, 8, 16, 24 }, E[] = { 7, 15, 23, 31 };
BYTE S[] = { 0, 11, 22 }, E[] = { 10, 21, 31 };
BYTE x, y, z;
int nCubes;

// Start with a single cube or multiple cubes that will be split
// until we reach the desired palette size
if ( TRUE /*nMaxColors < 27*/ )
	{ // Start with a single cube
	nCubes = 0;
	CubeList[nCubes].x1 = 0; CubeList[nCubes].x2 = 31;
	CubeList[nCubes].y1 = 0; CubeList[nCubes].y2 = 31;
	CubeList[nCubes].z1 = 0; CubeList[nCubes].z2 = 31;
	SumCube( &CubeList[nCubes], lpHistoStart );
	++nCubes;
	// for 256 add in a black and a white
	if (nMaxColors == 256)
		{
		CubeList[nCubes].x1 = CubeList[nCubes].x2 = 31;
		CubeList[nCubes].y1 = CubeList[nCubes].y2 = 31;
		CubeList[nCubes].z1 = CubeList[nCubes].z2 = 31;
		SumCube( &CubeList[nCubes], lpHistoStart );
		++nCubes;
		CubeList[nCubes].x1 = CubeList[nCubes].x2 = 0;
		CubeList[nCubes].y1 = CubeList[nCubes].y2 = 0;
		CubeList[nCubes].z1 = CubeList[nCubes].z2 = 0;
		SumCube( &CubeList[nCubes], lpHistoStart );
		++nCubes;
		}
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
		SumCube( &CubeList[nCubes], lpHistoStart );
		nCubes++;
		}
	}
return(nCubes);
}

//************************************************************************
LOCAL void SortPalette(LPRGB lpRGBmap, int nEntries)
//***********************************************************************
{
int i, j;
HSLS hsl;
DWORD iValue, jValue;
RGBS TempRGB;
LPRGB lpRGB;

// Sort the cubes by its average HSL value (just to organize it)
for ( i=0; i<nEntries; i++ )
	{
	lpRGB = &lpRGBmap[i];
	RGBtoHSL( lpRGB->red, lpRGB->green, lpRGB->blue, &hsl );
	iValue = (((DWORD) (hsl.hue << 8 | hsl.sat) << 8) | hsl.lum);

	for ( j=i+1; j<nEntries; j++ )
		{
		lpRGB = &lpRGBmap[j];
		RGBtoHSL( lpRGB->red, lpRGB->green, lpRGB->blue, &hsl );
		jValue = (((DWORD) (hsl.hue << 8 | hsl.sat) << 8) | hsl.lum);
		if ( jValue < iValue )
			{
			iValue = jValue;
			TempRGB = lpRGBmap[i];
			lpRGBmap[i] = lpRGBmap[j];
			lpRGBmap[j] = TempRGB;
			}
		}
	}
}

//************************************************************************
LOCAL void ComputePalette(LPCUBE lpCube, LPRGB lpMap, int nCubes, LPWORD lpHistoStart) 
//************************************************************************
{
int i;
long lCount;
BYTE x, y, z;
WORD wEntry, wBestX, wBestY, wBestZ, wMin, wMax;
LPWORD lpwEntry;
LFIXED ratio;

for ( i=0; i<nCubes; i++ )
	{
	lCount = lpCube->lPixelCount;
	wBestX = wBestY = wBestZ = 0;

	for ( x=lpCube->x1; x<=lpCube->x2; x++ )
	for ( y=lpCube->y1; y<=lpCube->y2; y++ )
	for ( z=lpCube->z1; z<=lpCube->z2; z++ )
		{
		lpwEntry = lpHistoStart + ToMiniRGB( x, y, z );
		wEntry = *lpwEntry;
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

		lpMap->red   = (BYTE)wBestX;
		lpMap->green = (BYTE)wBestY;
		lpMap->blue  = (BYTE)wBestZ;
		lpMap++;
		}
	lpCube++;
	}
}

//************************************************************************
LOCAL long CreateHistogram(LPOBJECT lpObject, LPFRAME lpFrame, LPWORD lpHisto,
	   						STATUSCALLBACKPROC lpStatusCallback)
//************************************************************************
{
long lHistoCount;
LPTR lpLine;
RGBS rgb;
WORD wRGB;
LPWORD lpwEntry;
int x, y, depth;
LPTR lpImgScanline;

lpImgScanline = NULL;

if (!lpFrame)
	return( 0L );

if ( !lpHisto )
	return( 0L );

lHistoCount = 0;
depth = FrameDepth(lpFrame);

AllocLines(&lpImgScanline, 1, FrameXSize(lpFrame), depth );

if (!lpImgScanline)
	return( 0L );

FRMTYPEINFO Type = FrameTypeInfo( lpFrame );

for (y = 0; y < FrameYSize(lpFrame); ++y)
	{
	if (lpStatusCallback)
		{
		(*lpStatusCallback)(y, FrameYSize(lpFrame), NO);
		}

	if (lpObject)
	{
		if (!ObjGetLine( lpObject, 0, y, FrameXSize(lpFrame), lpImgScanline))
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
        if( Type.DataType == FDT_PALETTECOLOR )
            FrameGetRGB( lpFrame, lpLine, &rgb, 1 );
        else
		    frame_getRGB( lpLine, depth, &rgb );

		wRGB = RGBtoMiniRGB( &rgb );

		lpwEntry = lpHisto + wRGB;
		if ( *lpwEntry >= 65535 )
			continue;
		*lpwEntry += 1;
		lHistoCount++;
		}
	}

if (lpImgScanline)
	FreeUp( lpImgScanline );

return( lHistoCount );
}


//************************************************************************
LOCAL BOOL SplitCube(LPCUBE lpCubeList, int nCubes, LPWORD lpHisto, BOOL fByVolume)
//************************************************************************
{
LPCUBE lpLargestCube, lpNewCube;

/* Find the largest subcube in the list; that's the one we'll split */
if (fByVolume)
	lpLargestCube = FindBiggestVolume(lpCubeList, nCubes);
else
	{
	lpLargestCube = FindBiggestColorCount(lpCubeList, nCubes);
	// if no cubes splittable based on color count, split by volume
	if (!lpLargestCube)
		lpLargestCube = FindBiggestVolume(lpCubeList, nCubes);
	}

// nothing splittable
if (!lpLargestCube)
	return(NO);

// get pointer to new cube
lpNewCube = &lpCubeList[nCubes];

// split the cube along the longest dimension
MedianCut(lpLargestCube, lpNewCube);

// shrink both cubes and recalculate pixel and color sums
SumCube( lpNewCube, lpHisto );
SumCube( lpLargestCube, lpHisto );

return( YES );
}

#define SPLITTABLE(lpCube) (lpCube->x2 > lpCube->x1 ||	lpCube->y2 > lpCube->y1 || lpCube->z2 > lpCube->z1)

/***********************************************************************/
LOCAL LPCUBE FindBiggestVolume( LPCUBE lpCubeList, int nCubes )
/***********************************************************************/
{
LPCUBE lpCube, lpLargestCube;
long lCount, lLargestCount;

lpCube = lpCubeList;
lLargestCount = -1;
lpLargestCube = NULL;

while ( --nCubes >= 0 )
	{
	lCount = VOLUME(lpCube);
	if (lCount > lLargestCount)
		{
		if ( SPLITTABLE(lpCube) )
			{ // There must be something to split
			lpLargestCube = lpCube;
			lLargestCount = lCount;
			}
		}
	lpCube++;
	}
return(lpLargestCube);
}

/***********************************************************************/
LOCAL LPCUBE FindBiggestColorCount( LPCUBE lpCubeList, int nCubes )
/***********************************************************************/
{
LPCUBE lpCube, lpLargestCube;
long lLargestCount;

lpCube = lpCubeList;
lLargestCount = -1;
lpLargestCube = NULL;

while ( --nCubes >= 0 )
	{
	if ( lpCube->lCount > lLargestCount ||
   	( lpCube->lCount == lLargestCount &&
		VOLUME(lpCube) > VOLUME(lpLargestCube) ) )
		{
		if ( SPLITTABLE(lpCube) )
			{ // There must be something to split
			lpLargestCube = lpCube;
			lLargestCount = lpCube->lCount;
			}
		}
	lpCube++;
	}
return(lpLargestCube);
}

/***********************************************************************/
LOCAL LPCUBE FindBiggestPixelCount( LPCUBE lpCubeList, int nCubes )
/***********************************************************************/
{
LPCUBE lpCube, lpLargestCube;
long lLargestCount;

lpCube = lpCubeList;
lLargestCount = -1;
lpLargestCube = NULL;

while ( --nCubes >= 0 )
	{
	if ( lpCube->lPixelCount > lLargestCount ||
   	( lpCube->lPixelCount == lLargestCount &&
		VOLUME(lpCube) > VOLUME(lpLargestCube) ) )
		{
		if ( SPLITTABLE(lpCube) )
			{ // There must be something to split
			lpLargestCube = lpCube;
			lLargestCount = lpCube->lPixelCount;
			}
		}
	lpCube++;
	}
return(lpLargestCube);
}

/***********************************************************************/
LOCAL void MedianCut( LPCUBE lpLargestCube, LPCUBE lpNewCube )
/***********************************************************************/
{
int xDelta, yDelta, zDelta, MaxDelta;
WORD p;

/* Find the longest dimension of the subcube; we'll split along that side */
xDelta = lpLargestCube->x2 - lpLargestCube->x1;
yDelta = lpLargestCube->y2 - lpLargestCube->y1;
zDelta = lpLargestCube->z2 - lpLargestCube->z1;

MaxDelta = max( xDelta, max( yDelta, zDelta ) );

*lpNewCube = *lpLargestCube; /* Make a copy of the cube we're going to split */

if ( MaxDelta == xDelta )
	{
	p = (WORD)( lpLargestCube->x1 + lpLargestCube->x2 ) / 2;
	lpNewCube->x2 = p;
	lpLargestCube->x1 = p+1;
	}
else
if ( MaxDelta == yDelta )
	{
	p = (WORD)( lpLargestCube->y1 + lpLargestCube->y2 ) / 2;
	lpNewCube->y2 = p;
	lpLargestCube->y1 = p+1;
	}
else
if ( MaxDelta == zDelta )
	{
	p = (WORD)( lpLargestCube->z1 + lpLargestCube->z2 ) / 2;
	lpNewCube->z2 = p;
	lpLargestCube->z1 = p+1;
	}
}

/***********************************************************************/
LOCAL void MedianCountCut( LPCUBE lpLargestCube, LPCUBE lpNewCube, LPWORD lpHisto )
/***********************************************************************/
{
int xDelta, yDelta, zDelta, MaxDelta;
WORD p;
long lTargetCount, lCount;

// if no pixel count, use MedianCut
if (!lpLargestCube->lPixelCount)
	{
	MedianCut(lpLargestCube, lpNewCube);
	return;
	}

/* Sum the counts of each plane referenced by the subcube */
/* When the sum is greater than half the total subcube count, */
/* then stop; that's the point where we split the cube into 2 cubes */
lTargetCount = lpLargestCube->lPixelCount/2;

*lpNewCube = *lpLargestCube; /* Make a copy of the cube we're going to split */

/* Find the longest dimension of the subcube; we'll split along that side */
xDelta = lpLargestCube->x2 - lpLargestCube->x1;
yDelta = lpLargestCube->y2 - lpLargestCube->y1;
zDelta = lpLargestCube->z2 - lpLargestCube->z1;

while (TRUE)
	{
	lCount = 0;
	MaxDelta = max( xDelta, max( yDelta, zDelta ) );
	if ( MaxDelta == xDelta )
		{
		p = lpLargestCube->x1;
		while (1)
			{
			lCount += SumPlaneX( p, lpLargestCube, lpHisto );
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
				continue;
			}
		lpNewCube->x2 = p;
		lpLargestCube->x1 = p+1;
		break;
		}
	else
	if ( MaxDelta == yDelta )
		{
		p = lpLargestCube->y1;
		while (1)
			{
			lCount += SumPlaneY( p, lpLargestCube, lpHisto );
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
				continue;
			}
		lpNewCube->y2 = p;
		lpLargestCube->y1 = p+1;
		break;
		}
	else
	//if ( MaxDelta == zDelta )
		{
		p = lpLargestCube->z1;
		while (1)
			{
			lCount += SumPlaneZ( p, lpLargestCube, lpHisto );
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
				continue;
			}
		lpNewCube->z2 = p;
		lpLargestCube->z1 = p+1;
		break;
		}
	}
}

/***********************************************************************/
LOCAL void SumCube( LPCUBE lpCube, LPWORD lpHisto )
/***********************************************************************/
{
long lCount, lAmount, lPixelCount;
WORD x, y, z;
BYTE minx, maxx;
BYTE miny, maxy;
BYTE minz, maxz;

// calculate the sum of color occurence as well as 
// the number of pixels in the cube, this also shrinks
// the cube to make it the smallest possible and still
// contain all the colors in the image
lCount = lPixelCount = 0;
minx = lpCube->x2;
maxx = lpCube->x1;
miny = lpCube->y2;
maxy = lpCube->y1;
minz = lpCube->z2;
maxz = lpCube->z1;
for ( x=lpCube->x1; x<=lpCube->x2; x++ )
for ( y=lpCube->y1; y<=lpCube->y2; y++ )
for ( z=lpCube->z1; z<=lpCube->z2; z++ )
	{
	lAmount = *(lpHisto + ToMiniRGB( x, y, z ));
	if (lAmount)
		{
		if (x < minx)
			minx = x;
		if (x > maxx)
			maxx = x;
		if (y < miny)
			miny = y;
		if (y > maxy)
			maxy = y;
		if (z < minz)
			minz = z;
		if (z > maxz)
			maxz = z;
		lPixelCount += lAmount;
		++lCount;
		}
	}
lpCube->lCount = lCount;
lpCube->lPixelCount = lPixelCount;
if (minx < maxx)
	{
	lpCube->x1 = minx;
	lpCube->x2 = maxx;
	}
if (miny < maxy)
	{
	lpCube->y1 = miny;
	lpCube->y2 = maxy;
	}
if (minz < maxz)
	{
	lpCube->z1 = minz;
	lpCube->z2 = maxz;
	}
}

//************************************************************************
LOCAL long SumPlaneX(WORD x, LPCUBE lpCube, LPWORD lpHisto)
//************************************************************************
{
long lCount;
WORD y, z;

lCount = 0;
for ( y=lpCube->y1; y<=lpCube->y2; y++ )
	for ( z=lpCube->z1; z<=lpCube->z2; z++ )
		lCount += *(lpHisto + ToMiniRGB( x, y, z ));

return( lCount );
}


//************************************************************************
LOCAL long SumPlaneY(WORD y, LPCUBE lpCube, LPWORD lpHisto)
//************************************************************************
{
long lCount;
WORD x, z;

lCount = 0;
for ( x=lpCube->x1; x<=lpCube->x2; x++ )
	for ( z=lpCube->z1; z<=lpCube->z2; z++ )
		lCount += *(lpHisto + ToMiniRGB( x, y, z ));

return( lCount );
}


//************************************************************************
LOCAL long SumPlaneZ(WORD z, LPCUBE lpCube, LPWORD lpHisto)
//************************************************************************
{
long lCount;
WORD x, y;

lCount = 0;
for ( x=lpCube->x1; x<=lpCube->x2; x++ )
	for ( y=lpCube->y1; y<=lpCube->y2; y++ )
		lCount += *(lpHisto + ToMiniRGB( x, y, z ));

return( lCount );
}


#define STUFFRGB( lpRGB, r, g, b ) \
	{ (lpRGB)->red=r; (lpRGB)->green=g; (lpRGB)->blue=b; }

//************************************************************************
LOCAL void Stuff8ColorPalette( LPRGB lpRGB )
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
LOCAL void Stuff16ColorPalette( LPRGB lpRGB )
//************************************************************************
{
STUFFRGB( lpRGB,   0,   0,   0 ); lpRGB++;
STUFFRGB( lpRGB, 128,   0,   0 ); lpRGB++;
STUFFRGB( lpRGB,   0, 128,   0 ); lpRGB++;
STUFFRGB( lpRGB, 128, 128,   0 ); lpRGB++;
STUFFRGB( lpRGB,   0,   0, 128 ); lpRGB++;
STUFFRGB( lpRGB, 128,   0, 128 ); lpRGB++;
STUFFRGB( lpRGB,   0, 128, 128 ); lpRGB++;
STUFFRGB( lpRGB, 128, 128, 128 ); lpRGB++;
STUFFRGB( lpRGB, 192, 192, 192 ); lpRGB++;
STUFFRGB( lpRGB, 255,   0,   0 ); lpRGB++;
STUFFRGB( lpRGB,   0, 255,   0 ); lpRGB++;
STUFFRGB( lpRGB, 255, 255,   0 ); lpRGB++;
STUFFRGB( lpRGB,   0,   0, 255 ); lpRGB++;
STUFFRGB( lpRGB, 255,   0, 255 ); lpRGB++;
STUFFRGB( lpRGB,   0, 255, 255 ); lpRGB++;
STUFFRGB( lpRGB, 255, 255, 255 );
}


//************************************************************************
LOCAL void Stuff256ColorPalette( LPRGB lpRGB )
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
	g = ((w*255)+19)/39;
// what the fuck is this supposed to do? g = min(w * 40, 255);
	STUFFRGB( lpRGB, g, g, g ); lpRGB++;
	}
}

