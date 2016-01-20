// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®PL1¯®FD1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/************************************************************************/
/* Cube definition							*/
/************************************************************************/
typedef struct _cube
	{
	long lCount;
	BYTE x1, y1, z1, x2, y2, z2;
} CUBE;
typedef CUBE far *LPCUBE;

#define CUBESIZE(lpc) ( (long)(lpc->x2 - lpc->x1 + 1) * \
			(long)(lpc->y2 - lpc->y1 + 1) * \
			(long)(lpc->z2 - lpc->z1 + 1) )

/***********************************************************************/
static BOOL SplitCube( lpCubeList, nCubes )
/***********************************************************************/
LPCUBE lpCubeList;
int nCubes;
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
	     CUBESIZE(lpCube) > CUBESIZE(lpLargestCube) ) )
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
lpCubeList[nCubes] = NewCube; /* Copy the new cube to the end of the list */
return( YES );
}


static LPWORD lpHistoStart;

/***********************************************************************/
static long SumPlaneX( x, lpCube )
/***********************************************************************/
WORD x;
LPCUBE lpCube;
{
long lCount;
WORD y, z;

lCount = 0;
for ( y=lpCube->y1; y<=lpCube->y2; y++ )
for ( z=lpCube->z1; z<=lpCube->z2; z++ )
	lCount += *(lpHistoStart + ToMiniRGB( x, y, z ));
return( lCount );
}


/***********************************************************************/
static long SumPlaneY( y, lpCube )
/***********************************************************************/
WORD y;
LPCUBE lpCube;
{
long lCount;
WORD x, z;

lCount = 0;
for ( x=lpCube->x1; x<=lpCube->x2; x++ )
for ( z=lpCube->z1; z<=lpCube->z2; z++ )
	lCount += *(lpHistoStart + ToMiniRGB( x, y, z ));
return( lCount );
}


/***********************************************************************/
static long SumPlaneZ( z, lpCube )
/***********************************************************************/
WORD z;
LPCUBE lpCube;
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
long CreateHistogram(lpHisto)
/***********************************************************************/
LPWORD lpHisto;
{
LPFRAME lpFrame;
long lHistoCount;
LPTR lpLine;
RGBS rgb;
WORD wHSL;
LPWORD lpwEntry;
int x, y;

lpFrame = frame_set(NULL);
lHistoCount = 0;
for (y = 0; y < lpFrame->Ysize; ++y)
    {
    lpLine = frame_ptr(0, 0, y, NO);
    if (!lpLine)
        continue;
    for (x = 0; x < lpFrame->Xsize; ++x, lpLine += DEPTH)
        {
        frame_getRGB( lpLine, &rgb );
	    wHSL = RGBtoMiniRGB( &rgb );
	    if (lpHisto)
	        {
	        lpwEntry = lpHisto + wHSL;
	        if ( *lpwEntry < 65535 )
		        {
		        *lpwEntry += 1;
		        lHistoCount++;
		        }
		    }
		}
    }
return(lHistoCount);
}

/***********************************************************************/
LPTR CreateOptimizedPalette(lpRGBmap)
/***********************************************************************/
LPRGB lpRGBmap;
{
CUBE CubeList[256];
LPCUBE lpCube;
int nCubes, nEntry;
BYTE x, y, z;
DWORD lBestX, lBestY, lBestZ;
LPWORD lpwEntry;
WORD i, j, wMini;
DWORD lEntry, iValue, jValue;
CUBE TempCube;
HSLS hsl;
LPTR lpPalette;
long lHistoCount;
LPWORD lpHisto;

if ( !(lpHisto = (LPWORD)Alloc( 32768L*2L )) )
	return( NULL );
lclr( (LPTR)lpHisto, 32768L*2L );
lHistoCount = CreateHistogram(lpHisto);
lpHistoStart = lpHisto;

/* Start with a single cube, and split it till we reach our palette size */
CubeList[0].lCount = lHistoCount;
CubeList[0].x1 = 0; CubeList[0].x2 = 31;
CubeList[0].y1 = 0; CubeList[0].y2 = 31;
CubeList[0].z1 = 0; CubeList[0].z2 = 31;
nCubes = 1;
while ( nCubes < 256 )
	{
	if ( !SplitCube( CubeList, nCubes ) )
		break;
	nCubes++;
	}

/* Sort the cubes by its average HSL value */
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

/* Go through each cube entry
   1. compute a weighted RGB average for the cube
   2. set each entry to the current palette index
   3. compute the HSL color and stuff it in the HSLmap */
nEntry = 0;
lpCube = CubeList;
while ( --nCubes >= 0 )
	{
	lBestX = lBestY = lBestZ = 0;
	for ( x=lpCube->x1; x<=lpCube->x2; x++ )
	for ( y=lpCube->y1; y<=lpCube->y2; y++ )
	for ( z=lpCube->z1; z<=lpCube->z2; z++ )
		{
		lpwEntry = lpHisto + ToMiniRGB( x, y, z );
		if ( lEntry = (DWORD)*lpwEntry )
			{
			lBestX += ( lEntry * x );
			lBestY += ( lEntry * y );
			lBestZ += ( lEntry * z );
			}
		*lpwEntry = nEntry;
		}

	if ( !(lpCube->lCount) ) /* If it's an empty cube... */
		{
		lBestX = ( lpCube->x1 + lpCube->x2 )<<2;
		lBestY = ( lpCube->y1 + lpCube->y2 )<<2;
		lBestZ = ( lpCube->z1 + lpCube->z2 )<<2;
		}
	else	{
		/* Get lBestx into a range of 0-255 */
		lBestX *= 8;
		if ( (lBestX /= lpCube->lCount) > 255 ) lBestX = 255;
		lBestY *= 8;
		if ( (lBestY /= lpCube->lCount) > 255 ) lBestY = 255;
		lBestZ *= 8;
		if ( (lBestZ /= lpCube->lCount) > 255 ) lBestZ = 255;
		}

    	lpRGBmap->red = lBestX;
    	lpRGBmap->green = lBestY;
    	lpRGBmap->blue = lBestZ;
    	wMini = RGBtoMiniRGB(lpRGBmap);
    	lpRGBmap->red = MaxiR(wMini);
    	lpRGBmap->green = MaxiG(wMini);
    	lpRGBmap->blue = MaxiB(wMini);
	
	lpRGBmap++;
	lpCube++;
	nEntry++;
	}

/* Make the word lut into a byte lut */
lpPalette = (LPTR)lpHistoStart;
lpHisto = lpHistoStart;
for ( i=0; i<=32767; i++ )
	*lpPalette++ = *lpHisto++;
return( (LPTR)lpHistoStart );
}
