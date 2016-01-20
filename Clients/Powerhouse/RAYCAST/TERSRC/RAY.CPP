#include <math.h>
#include <stdlib.h>
#include "Ray.hpp"

#define ASSERT(x) { if (!(x)) exit(1); }

const int FIX2BITS = 16;
const int FIX2 = 1 << FIX2BITS;

typedef void (*SectProc)(int projZ, int surfaceZ, int surfaceX, int sectFrac, int minorDirection);

int iMin, iMaj;
int iMinErr, iMajErr;

void InitRayCaster(int oMaj, int oMin)
{
	// Initialize minor axis error term for fractional view position
	iMinErr = (oMin & 15) << (FIX2BITS-4);
	// Isolate minor axis whole term
	iMin = oMin / 16;
	// Verify algorithm:  minErr must be fractional
	ASSERT(iMinErr<FIX2);

	/*
	// Initialize major axis error term for fractional view position
	iMajErr = (oMaj & 15) << (FIX2BITS-4);
	// Isolate major axis whole term
	iMaj = oMaj / 16;
	// Verify algorithm:  majErr must be fractional
	ASSERT(iMajErr<FIX2);
	*/
	
	iMajErr = 0;
	iMaj = oMaj / 16;
}

void CastRayMajor(int major, int minor, int dmin, int steps, SectProc sectMaj)
//
//
// Cast a ray along a line with slope major/minor
// and origin at (oMaj, oMin)
// Assumes 1 < major/minor < 2
//
// Same as below but ignore minor axis intersections (much faster; but results in jaggy artifacts along ridges)
//
{
	int majStep = 1;
	int minStep = 0;

	int minErrStep 	= minor*FIX2/major;
	int minErr 		= iMinErr;
	int oMin  		= iMin;
	if (dmin<0 && minErr)
	{
		minErr = FIX2 - minErr;
		oMin++;
	}
	
	int oMaj = iMaj;
	
	while (majStep < steps)
	{
		minErr += minErrStep;
		if (minErr>=FIX2)
		{
			minStep += dmin;
			minErr -= FIX2;
		}
		
		majStep++;
		sectMaj(majStep, majStep+oMaj, minStep+oMin, minErr >> (FIX2BITS-8), dmin);
	}
}

void CastRay(int major, int minor, int dmin, int steps, SectProc sectMin, SectProc sectMaj)
//
// Cast a ray along a line with slope major/minor
// and origin at (oMaj, oMin)
// Assumes 1 < major/minor < 2
//
{
	// Initialize starting positions
	int majStep = 1;
	int minStep = 0;

	// Calculate error steps (fixed-point slopes)
	int minErrStep = minor*FIX2/major;
	int majErrStep = (minor ? major*FIX2/minor : 1);

	// Initialize minor axis whole term
	int oMin = iMin;
	// Initialize minor axis error term
	int minErr = iMinErr;
	
	// Reflect minor axis if necessary	
	if (dmin<0 && minErr)
	{
		minErr = FIX2 - minErr;
		oMin++;
	}
	// Verify algorithm:  minErr must be fractional
	ASSERT(minErr<FIX2);

	// Initialize major axis error term
	int majErr = iMajErr;
	majErr += (FIX2 - minErr) * majErrStep / FIX2;
	majErr &= (FIX2-1);
	
	// Isolate fractional portion of the major axis slope
	majErrStep &= (FIX2-1);
	// Initialize major axis whole term
	int oMaj = iMaj;

	// For each step on the major axis
	while (majStep < steps)
	{
		// Is crossing the minor axis
		if (minErr+minErrStep > FIX2)
		{
			// Next minor value
			minStep += dmin;

			// Let client handle the intersection
			sectMin(majStep, majStep+oMaj, minStep+oMin, majErr >> (FIX2BITS-8), dmin);

			// Increment the major axis error term
			majErr += majErrStep;
			// Remove the the whole portion, we only want the fraction
			majErr &= (FIX2-1);

			// Remove the the whole portion, we only want the fraction
			minErr -= FIX2;
		}

		// Next major axis step
		majStep++;

		// Increment the minor axis error term
		minErr += minErrStep;
		// If intersecting both axes,
		if (minErr==FIX2)
		{
			// Handle as special case for consistent rendering:
			//	treat intersection as occuring in previous polygon, i.e. don't advance minStep yet
			sectMaj(majStep, majStep+oMaj, minStep+oMin, minErr >> (FIX2BITS-8), dmin);
			
			// Next minor value
			minStep += dmin;
			// Remove the the whole portion, we only want the fraction
			minErr = 0;

			// Increment the major axis error term
			majErr += majErrStep;
			// Remove the the whole portion, we only want the fraction
			majErr &= (FIX2-1);
		}
		else
		{		
			// Let client handle the intersection
			sectMaj(majStep, majStep+oMaj, minStep+oMin, minErr >> (FIX2BITS-8), dmin);
		}
	}
}


