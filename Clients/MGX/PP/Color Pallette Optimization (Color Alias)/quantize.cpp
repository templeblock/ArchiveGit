#include "stdafx.h"
#include "quantize.h"

//#define NO_LUM_CHECK

CQuantize::CQuantize()
{
	m_lpHisto = NULL;
	m_lpCubes = NULL;
	m_nCubes = 0;
}

CQuantize::~CQuantize()
{
	if (m_lpHisto)
		free(m_lpHisto);
	if (m_lpCubes)
		free(m_lpCubes);
}

BOOL CQuantize::Init()
{
	m_lpHisto = (LPDWORD)calloc(HISTO_ENTRIES, sizeof(DWORD));
	return(m_lpHisto != NULL);
}

void CQuantize::Reset()
{
	ZeroMemory(m_lpHisto, HISTO_ENTRIES * sizeof(DWORD));
	if (m_lpCubes)
	{
		free(m_lpCubes);
		m_lpCubes = NULL;
	}
}

void CQuantize::CountFrequencies(LPRGBTRIPLE lpRGB, int nPixels)
{
	BYTE r, g, b;
	int min, max, lum, index;

	while (--nPixels >= 0)
	{
		r = lpRGB->rgbtRed;
		g = lpRGB->rgbtGreen;
		b = lpRGB->rgbtBlue;

		#ifndef NO_LUM_CHECK
		min = min(min(r, g), b);						
		max = max(max(r, g), b);
		lum = (min+max)>>1;
		if (lum < 32) // darks colors get quantized more (0-31)
		{
			r = (r >> SHIFT_BITS) & 0x3C;	// 16 levels
			g = (g >> SHIFT_BITS) & 0x3C;
			b = (b >> SHIFT_BITS) & 0x3C;
			index = MINIINDEX(r, g, b);
		}
		else
		if (lum < 64) // darks colors get quantized more (32-63)
		{
			r = (r >> SHIFT_BITS) & 0x3E;	// 32 levels
			g = (g >> SHIFT_BITS) & 0x3E;
			b = (b >> SHIFT_BITS) & 0x3E;
			index = MINIINDEX(r, g, b);
		}
		else // non-dark colors (64-255)
		#endif
		{
			index = INDEX(r, g, b);			// 64 levels
		}

		++m_lpHisto[index];
		++lpRGB;
	} 
}

int CQuantize::CountColors()
{
	int nColors = 0;
	LPDWORD lpHisto = m_lpHisto;
	int nEntries = HISTO_ENTRIES;
	while (--nEntries >= 0)
	{
		if (*lpHisto)
			++nColors;
		++lpHisto;
	}
	return(nColors);
}

int CQuantize::CreatePalette(LPRGBQUAD lpRGBMap, int nEntries, LPRGBQUAD lpInitialMap, int nInitialEntries)
{
	m_lpCubes = (LPCUBE)malloc(sizeof(CUBE)*nEntries);
	if (!m_lpCubes)
		return(0);

	// we're always going to add white and black to palette
	// so remove them from the histogram
	nEntries -= 2;
	m_lpHisto[INDEX(0,0,0)] = 0;
	m_lpHisto[INDEX(255,255,255)] = 0;
	if (lpInitialMap && nInitialEntries)
	{
		nEntries -= nInitialEntries;
		for (int i = 0; i < nInitialEntries; ++i)
		{
			BYTE r = lpInitialMap[i].rgbRed;
			BYTE g = lpInitialMap[i].rgbGreen;
			BYTE b = lpInitialMap[i].rgbBlue;
			m_lpHisto[INDEX(r,g,b)] = 0;
		}
	}

	InitCubes();
	while ( m_nCubes < nEntries )
	{
		if ( !SplitCube() )
			break;
	}
	CalculatePalette(lpRGBMap, lpInitialMap, nInitialEntries);
	return(m_nCubes+2+nInitialEntries);
}

void CQuantize::InitCubes()
{
	m_nCubes = 0;
	m_lpCubes[m_nCubes].r1 = 0; m_lpCubes[m_nCubes].r2 = MAXVAL;
	m_lpCubes[m_nCubes].g1 = 0; m_lpCubes[m_nCubes].g2 = MAXVAL;
	m_lpCubes[m_nCubes].b1 = 0; m_lpCubes[m_nCubes].b2 = MAXVAL;
	SumCube( &m_lpCubes[m_nCubes] );
	++m_nCubes;
}

void CQuantize::SumCube( LPCUBE lpCube )
{
	long dwCount, dwAmount, dwPixelCount;
	BYTE r, g, b;
	BYTE minr, maxr;
	BYTE ming, maxg;
	BYTE minb, maxb;

	// calculate the sum of color occurence as well as 
	// the number of pixels in the cube, this also shrinks
	// the cube to make it the smallest possible and still
	// contain all the colors in the image
	dwCount = dwPixelCount = 0;
	minr = lpCube->r2;
	maxr = lpCube->r1;
	ming = lpCube->g2;
	maxg = lpCube->g1;
	minb = lpCube->b2;
	maxb = lpCube->b1;
	for ( r=lpCube->r1; r<=lpCube->r2; r++ )
	{
		for ( g=lpCube->g1; g<=lpCube->g2; g++ )
		{
			for ( b=lpCube->b1; b<=lpCube->b2; b++ )
			{
				int index = MINIINDEX( r, g, b );
				dwAmount = m_lpHisto[index];
				if (dwAmount)
				{
					if (r < minr)
						minr = r;
					if (r > maxr)
						maxr = r;
					if (g < ming)
						ming = g;
					if (g > maxg)
						maxg = g;
					if (b < minb)
						minb = b;
					if (b > maxb)
						maxb = b;
					dwPixelCount += dwAmount;
					++dwCount;
				}
			}
		}
	}
	lpCube->dwCount = dwCount;
	lpCube->dwPixelCount = dwPixelCount;
	lpCube->r1 = minr;
	lpCube->r2 = maxr;
	lpCube->g1 = ming;
	lpCube->g2 = maxg;
	lpCube->b1 = minb;
	lpCube->b2 = maxb;
}

BOOL CQuantize::SplitCube()
{
	LPCUBE lpLargestCube, lpNewCube;

	// find the cube to split.  This should be the cube with
	// the most pixels in it.
	//lpLargestCube = FindCubeWithMostPixels();
	lpLargestCube = FindCubeWithMostColors();

	// nothing splittable
	if (!lpLargestCube)
		return(FALSE);

	// split the cube along the longest dimension
	lpNewCube = MedianCut(lpLargestCube, FALSE);

	// shrink both cubes and recalculate pixel and color sums
	SumCube( lpNewCube );
	SumCube( lpLargestCube );
	if (lpNewCube->dwPixelCount == 0 || lpLargestCube->dwPixelCount == 0)
	{
		MessageBox(NULL, "Created Empty cubes", "SplitCube()", MB_OK);
	}
	

	return( lpNewCube != NULL );
}

#define SPLITTABLE(lpCube) (lpCube->r2 > lpCube->r1 ||	lpCube->g2 > lpCube->g1 || lpCube->b2 > lpCube->b1)
#define VOLUME(lpc) ( lpc == NULL ? 0L : ((long)(lpc->r2 - lpc->r1 + 1) * \
					  (long)(lpc->g2 - lpc->g1 + 1) * \
					  (long)(lpc->b2 - lpc->b1 + 1)) )

LPCUBE CQuantize::FindCubeWithMostPixels()
{
	LPCUBE lpCube, lpLargestCube;
	DWORD dwLargestCount;
	int nCubes;

	lpCube = m_lpCubes;
	nCubes = m_nCubes;
	dwLargestCount = 0; 
	lpLargestCube = NULL;

	while ( --nCubes >= 0 )
	{
		if ( lpCube->dwPixelCount > dwLargestCount ||
	   	( lpCube->dwPixelCount == dwLargestCount &&
			VOLUME(lpCube) > VOLUME(lpLargestCube) ) )
		{
			if ( SPLITTABLE(lpCube) )
			{ // There must be something to split
				lpLargestCube = lpCube;
				dwLargestCount = lpCube->dwPixelCount;
			}
		}
		lpCube++;
	}
	return(lpLargestCube);
}

LPCUBE CQuantize::FindCubeWithMostColors()
{
	LPCUBE lpCube, lpLargestCube;
	DWORD dwLargestCount;
	int nCubes;

	lpCube = m_lpCubes;
	nCubes = m_nCubes;
	dwLargestCount = 0; 
	lpLargestCube = NULL;

	while ( --nCubes >= 0 )
	{
		if ( lpCube->dwCount > dwLargestCount ||
	   	( lpCube->dwCount == dwLargestCount &&
			VOLUME(lpCube) > VOLUME(lpLargestCube) ) )
		{
			if ( SPLITTABLE(lpCube) )
			{ // There must be something to split
				lpLargestCube = lpCube;
				dwLargestCount = lpCube->dwCount;
			}
		}
		lpCube++;
	}
	return(lpLargestCube);
}

LPCUBE CQuantize::MedianCut( LPCUBE lpLargestCube, BOOL fByPixels )
{
	int rDelta, gDelta, bDelta, MaxDelta;
	DWORD dwTargetCount;
	LPCUBE lpNewCube = NULL;

	// when splitting, split at point where pixel count
	// exceeds half the total for the cube
	if (fByPixels)
		dwTargetCount = lpLargestCube->dwPixelCount/2;
	else
		dwTargetCount = lpLargestCube->dwCount/2;

	// Find the longest dimension of the subcube; we'll split along that side
	rDelta = lpLargestCube->r2 - lpLargestCube->r1;
	gDelta = lpLargestCube->g2 - lpLargestCube->g1;
	bDelta = lpLargestCube->b2 - lpLargestCube->b1;

	while (TRUE)
	{
		// get max delta and see if we have an unsplittable cube
		MaxDelta = max( rDelta, max( gDelta, bDelta ) );
		if (!MaxDelta)
		{
			rDelta = lpLargestCube->r2 - lpLargestCube->r1;
			gDelta = lpLargestCube->g2 - lpLargestCube->g1;
			bDelta = lpLargestCube->b2 - lpLargestCube->b1;
			MaxDelta = max( rDelta, max( gDelta, bDelta ) );
			// this will force a split right down the middle
			dwTargetCount = 0;
		}
		if (MaxDelta == rDelta)
		{
			lpNewCube = SplitR(lpLargestCube, dwTargetCount, fByPixels);
			if (lpNewCube)
				return(lpNewCube);
			rDelta = 0;
		}
		else
		if (MaxDelta == gDelta)
		{
			lpNewCube = SplitG(lpLargestCube, dwTargetCount, fByPixels);
			if (lpNewCube)
				return(lpNewCube);
			gDelta = 0;
		}
		else
		if (MaxDelta == bDelta)
		{
			lpNewCube = SplitB(lpLargestCube, dwTargetCount, fByPixels);
			if (lpNewCube)
				return(lpNewCube);
			bDelta = 0;
		}
	}
	return(NULL);
}

LPCUBE CQuantize::SplitR(LPCUBE lpLargestCube, DWORD dwTargetCount, BOOL fByPixels)
{
	int r;
	DWORD dwCount = 0;

	if (dwTargetCount && lpLargestCube->dwPixelCount)
	{
		// split cube when pixel count exceeds half
		for (r = lpLargestCube->r1; r < lpLargestCube->r2; ++r)
		{
			if (fByPixels)
				dwCount += SumPlaneRByPixels( r, lpLargestCube );
			else
				dwCount += SumPlaneRByColors( r, lpLargestCube );
			if ( dwCount > dwTargetCount )
				break;
		}
		// we went over target count with last plane
		// let's back up one so we don't create an
		// empty cube
		if (r == lpLargestCube->r2)
			--r;
	}
	// we only split non-empty cubes
	if (!dwCount)
	{
		if (dwTargetCount)
			return(NULL);
		r = ( (int)lpLargestCube->r1 + (int)lpLargestCube->r2 ) / 2;
	}
	LPCUBE lpNewCube = &m_lpCubes[m_nCubes];
	++m_nCubes;
	*lpNewCube = *lpLargestCube;
	lpNewCube->r2 = r;
	lpLargestCube->r1 = r+1;
	return(lpNewCube);
}

LPCUBE CQuantize::SplitG(LPCUBE lpLargestCube, DWORD dwTargetCount, BOOL fByPixels)
{
	int g;
	DWORD dwCount = 0;

	if (dwTargetCount && lpLargestCube->dwPixelCount)
	{
		// split cube when pixel count exceeds half
		for (g = lpLargestCube->g1; g < lpLargestCube->g2; ++g)
		{
			if (fByPixels)
				dwCount += SumPlaneGByPixels( g, lpLargestCube );
			else
				dwCount += SumPlaneGByColors( g, lpLargestCube );
			if ( dwCount > dwTargetCount )
				break;
		}
		// we went over target count with last plane
		// let's back up one so we don't create an
		// empty cube
		if (g == lpLargestCube->g2)
			--g;
	}
	// we only split non-empty cubes
	if (!dwCount)
	{
		if (dwTargetCount)
			return(NULL);
		g = ( (int)lpLargestCube->g1 + (int)lpLargestCube->g2 ) / 2;
	}
	LPCUBE lpNewCube = &m_lpCubes[m_nCubes];
	++m_nCubes;
	*lpNewCube = *lpLargestCube;
	lpNewCube->g2 = g;
	lpLargestCube->g1 = g+1;
	return(lpNewCube);
}

LPCUBE CQuantize::SplitB(LPCUBE lpLargestCube, DWORD dwTargetCount, BOOL fByPixels)
{
	int b;
	DWORD dwCount = 0;

	if (dwTargetCount && lpLargestCube->dwPixelCount)
	{
		// split cube when pixel count exceeds half
		for (b = lpLargestCube->b1; b < lpLargestCube->b2; ++b)
		{
			if (fByPixels)
				dwCount += SumPlaneBByPixels( b, lpLargestCube );
			else
				dwCount += SumPlaneBByColors( b, lpLargestCube );
			if ( dwCount > dwTargetCount )
				break;
		}
		// we went over target count with last plane
		// let's back up one so we don't create an
		// empty cube
		if (b == lpLargestCube->b2)
			--b;
	}
	// we only split non-empty cubes
	if (!dwCount)
	{
		if (dwTargetCount)
			return(NULL);
		b = ( (int)lpLargestCube->b1 + (int)lpLargestCube->b2 ) / 2;
	}
	LPCUBE lpNewCube = &m_lpCubes[m_nCubes];
	++m_nCubes;
	*lpNewCube = *lpLargestCube;
	lpNewCube->b2 = b;
	lpLargestCube->b1 = b+1;
	return(lpNewCube);
}

DWORD CQuantize::SumPlaneRByPixels(int r, LPCUBE lpCube)
{
	DWORD dwCount;
	int g, b;

	dwCount = 0;
	for ( g=lpCube->g1; g<=lpCube->g2; g++ )
		for ( b=lpCube->b1; b<=lpCube->b2; b++ )
			dwCount += m_lpHisto[MINIINDEX(r, g, b)];

	return( dwCount );
}

DWORD CQuantize::SumPlaneGByPixels(int g, LPCUBE lpCube)
{
	DWORD dwCount;
	int r, b;

	dwCount = 0;
	for ( r=lpCube->r1; r<=lpCube->r2; r++ )
		for ( b=lpCube->b1; b<=lpCube->b2; b++ )
			dwCount += m_lpHisto[MINIINDEX(r, g, b)];

	return( dwCount );
}

DWORD CQuantize::SumPlaneBByPixels(int b, LPCUBE lpCube)
{
	DWORD dwCount;
	int r, g;

	dwCount = 0;
	for ( r=lpCube->r1; r<=lpCube->r2; r++ )
		for ( g=lpCube->g1; g<=lpCube->g2; g++ )
			dwCount += m_lpHisto[MINIINDEX(r, g, b)];

	return( dwCount );
}

DWORD CQuantize::SumPlaneRByColors(int r, LPCUBE lpCube)
{
	DWORD dwCount;
	int g, b;

	dwCount = 0;
	for ( g=lpCube->g1; g<=lpCube->g2; g++ )
		for ( b=lpCube->b1; b<=lpCube->b2; b++ )
		{
			if (m_lpHisto[MINIINDEX(r, g, b)])
				++dwCount;
		}

	return( dwCount );
}

DWORD CQuantize::SumPlaneGByColors(int g, LPCUBE lpCube)
{
	DWORD dwCount;
	int r, b;

	dwCount = 0;
	for ( r=lpCube->r1; r<=lpCube->r2; r++ )
		for ( b=lpCube->b1; b<=lpCube->b2; b++ )
		{
			if (m_lpHisto[MINIINDEX(r, g, b)])
				++dwCount;
		}

	return( dwCount );
}

DWORD CQuantize::SumPlaneBByColors(int b, LPCUBE lpCube)
{
	DWORD dwCount;
	int r, g;

	dwCount = 0;
	for ( r=lpCube->r1; r<=lpCube->r2; r++ )
		for ( g=lpCube->g1; g<=lpCube->g2; g++ )
		{
			if (m_lpHisto[MINIINDEX(r, g, b)])
				++dwCount;
		}

	return( dwCount );
}

void CQuantize::CalculatePalette(LPRGBQUAD lpMap, LPRGBQUAD lpInitialMap, int nInitialEntries) 
{
	int nCubes;
	DWORD dwSumR, dwSumG, dwSumB;
	BYTE r, g, b;
	DWORD dwEntry;
	LPCUBE lpCube;
	
	lpCube = m_lpCubes;
	// add black to the palette first
	lpMap->rgbRed = 0;
	lpMap->rgbGreen = 0;
	lpMap->rgbBlue = 0;
	lpMap++;
	// add entries from initial map
	if (lpInitialMap && nInitialEntries)
	{
		for (int i = 0; i < nInitialEntries; ++i)
		{
			*lpMap = lpInitialMap[i];
			lpMap++;
		}
	}

	nCubes = m_nCubes;
	while (--nCubes >= 0)
	{
		dwSumR = dwSumG = dwSumB = 0;
		if (lpCube->dwPixelCount)
		{
			for ( r=lpCube->r1; r<=lpCube->r2; r++ )
			for ( g=lpCube->g1; g<=lpCube->g2; g++ )
			for ( b=lpCube->b1; b<=lpCube->b2; b++ )
			{
				dwEntry = m_lpHisto[MINIINDEX(r, g, b)];
				if ( dwEntry )
				{
					dwSumR += (DWORD)r * dwEntry;
					dwSumG += (DWORD)g * dwEntry;
					dwSumB += (DWORD)b * dwEntry;
				}
			}
			r = (BYTE)(dwSumR / lpCube->dwPixelCount);
			g = (BYTE)(dwSumG / lpCube->dwPixelCount);
			b = (BYTE)(dwSumB / lpCube->dwPixelCount);
		}
		else
		{
			r = ( lpCube->r1 + lpCube->r2 ) / 2;
			g = ( lpCube->g1 + lpCube->g2 ) / 2;
			b = ( lpCube->b1 + lpCube->b2 ) / 2;
		}

		lpMap->rgbRed = ((int)r * 255) / 63;
		lpMap->rgbGreen = ((int)g * 255) / 63;
		lpMap->rgbBlue = ((int)b * 255) / 63;
		lpMap++;
		lpCube++;
	}
	// add white to the palette at end
	lpMap->rgbRed = 255;
	lpMap->rgbGreen = 255;
	lpMap->rgbBlue = 255;
}

#ifdef GARBAGE
void CQuantize::CreateLookupTable() 
{
	int i, j, rindex, gindex, bindex;
	BYTE r, g, b;
	LPCUBE lpCube;
	long lError, lClosestError, lR, lG, lB;
	int closest;
	long lRErrors[256*(MAXVAL+1)];
	long lGErrors[256*(MAXVAL+1)];
	long lBErrors[256*(MAXVAL+1)];

	#ifdef UNUSED
	// initialize entries to impossible value
	for (i = 0; i < HISTO_ENTRIES; ++i)
		m_lpHisto[i] = 0xFFFFFFFF;
	// go through cubes and fill in lookup table for each cube
	lpCube = m_lpCubes;
	for (i = 0; i < m_nCubes; ++i)
	{
		for ( r=lpCube->r1; r<=lpCube->r2; r++ )
		for ( g=lpCube->g1; g<=lpCube->g2; g++ )
		for ( b=lpCube->b1; b<=lpCube->b2; b++ )
		{
			m_lpHisto[MINIINDEX(r, g, b)] = i;			
		}
		++lpCube;
	}
	#endif

	// precalculate errors for each entry in palette
	r = 0;
	rindex = 0;
	for (r = 0; r <= MAXVAL; ++r, rindex += 256)
	{
		for (j = 0; j < m_nCubes; ++j)
		{
 			lR = r - m_MiniMap[j].rgbRed;
 			lG = r - m_MiniMap[j].rgbGreen;
 			lB = r - m_MiniMap[j].rgbBlue;
			lRErrors[rindex|j] = lR * lR;
			lGErrors[rindex|j] = lG * lG;
			lBErrors[rindex|j] = lB * lB;
		}
	}

	// now scan through the lookup and fill in unfilled entries
	i = 0;
	for ( r=0, rindex = 0; r<=MAXVAL; r++, rindex += 256 )
	for ( g=0, gindex = 0; g<=MAXVAL; g++, gindex += 256 )
	for ( b=0, bindex = 0; b<=MAXVAL; b++, bindex += 256 )
	{
		#ifdef UNUSED
		if (m_lpHisto[i] != 0xFFFFFFFF)
		{
			++i;
			continue;
		}
		#endif

		// now look for closest entry in palette
		closest = 0;
		lClosestError = lRErrors[rindex|0] + lGErrors[gindex|0] + lBErrors[bindex|0];
		for (j = 1; j < m_nCubes; ++j)
		{
			lError = lRErrors[rindex|j] + lGErrors[gindex|j] + lBErrors[bindex|j];
			if (lError < lClosestError)
			{
				closest = j;
				lClosestError = lError;
			}
		}
		m_lpHisto[i] = closest;
		++i;
	}
}

void CQuantize::MapPixels(LPRGBTRIPLE lpRGB, LPBYTE lpOut, int nPixels, BYTE BumpValue)
{
	if (BumpValue)
	{
		while (--nPixels >= 0)
		{
			*lpOut++ = (BYTE)m_lpHisto[INDEX(lpRGB->rgbtRed, lpRGB->rgbtGreen, lpRGB->rgbtBlue)] + BumpValue;
			++lpRGB;
		}
	}
	else
	{
		while (--nPixels >= 0)
		{
			*lpOut++ = (BYTE)m_lpHisto[INDEX(lpRGB->rgbtRed, lpRGB->rgbtGreen, lpRGB->rgbtBlue)];
			++lpRGB;
		}
	}
}

#endif
