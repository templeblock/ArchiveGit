// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯

#ifndef WIN32
#include <windows.h>
#endif

#include <math.h>
#include "imgutil.h"

void ResetMap( LPMAP lpMap, int maxpoints, BOOL Reverse );
void InvertMap( LPMAP lpMap );
void MakeQuickMap( LPMAP lpMap );
static void PosterizeMap(LPMAP lpMap);
static void ThresholdMap(LPMAP lpMap);

static LPINT16 HMSMaps[3]; 	// the highlight midtone shadow maps

#ifndef WIN32
static LPFLOAT lpWorkLut = NULL;
static LPFLOATPT lpPtTable = NULL;
#endif

/************************************************************************/
/************************************************************************/
BOOL SetupHMSMaps( HINSTANCE hInst, int baseid )
/************************************************************************/
{
	int      i;
	HRSRC    hData;
   HGLOBAL  hGlobal;
	LPTR     lpData;
	DWORD	dwSize;
	
	#ifndef WIN32
	lpWorkLut = (LPFLOAT)Alloc(LUTSIZE * sizeof(FLOAT));
	if (!lpWorkLut)
		return(FALSE);
	lpPtTable = (LPFLOATPT)Alloc(LUTSIZE * sizeof(FLOATPT));
	if (!lpPtTable)
		return(FALSE);
	#endif

	HMSMaps[BAND_ALL] = HMSMaps[BAND_HI] = HMSMaps[BAND_MID] = HMSMaps[BAND_SHADOWS] = NULL;
	for (i=BAND_HI; i<=BAND_SHADOWS; i++)
	{ 
		if ( !( HMSMaps[i-BAND_HI] = (LPINT16)Alloc(256*sizeof(INT16))))
			return(FALSE);
		if ( !( hData = FindResource( hInst, MAKEINTRESOURCE(i+baseid), RT_RCDATA )) )
			return(FALSE);
		dwSize = SizeofResource(hInst, hData);
		if (dwSize != (256*sizeof(INT16)) )
			return(FALSE);
		if ( !( hGlobal = LoadResource( hInst, hData )) )
			return(FALSE);
		if ( !(lpData = ( LPTR )LockResource( hGlobal )) )
			{
			FreeResource( hData );
			return(0);
			}
		copy( lpData, ( LPTR )HMSMaps[i-BAND_HI], 256*sizeof( INT16 ) );		
		UnlockResource( hGlobal );
		FreeResource( hGlobal );
	}
	return(TRUE);
}

/************************************************************************/
/************************************************************************/
void FreeupHMSMaps()									    
/************************************************************************/
{
	int i;
	
	for (i=BAND_HI; i<=BAND_SHADOWS; i++)
		if (HMSMaps[i-BAND_HI])
			FreeUp( (LPTR) HMSMaps[i-BAND_HI] );
}

/************************************************************************/
/************************************************************************/
void MakeMap( LPMAP lpMap )
/************************************************************************/
{
	#ifdef WIN32
	FLOAT lpWorkLut[LUTSIZE];
	#endif

	// Make a floating point LUT
	MakeFloatMap(lpMap, lpWorkLut);

	// Create a LUT of the floating point values
	MapCreateLut(lpWorkLut, lpMap->Lut);

	// Posterize it (if necessary)
	PosterizeMap(lpMap);

	// Threshold it ( if Necessary)
	ThresholdMap(lpMap);
}

/************************************************************************/
/************************************************************************/
void MakeFloatMap( LPMAP lpMap, LPFLOAT lpFloatLut )
/************************************************************************/
{
int i, x, x1, x2, y1, y2, bStretch, minx, maxx;
int StepMin, StepMax, Brightness[4], Contrast;
int HMSBrightness;
BOOL bReverseHMS;
LFIXED fxScale, fyScale;
long m;
BOOL bUseHMS, bGamma, bCurves;
double gamma, val;
double fValue;
POINT ptCurve[MAXPOINTS+2];
int iCurvePoints;
double spline[MAXPOINTS+2];

#ifdef UNUSED
if (lpMap->gamma == 1.0 && !(lpMap->Reverse & REVERSE_LUTXY))
{
	for ( i=0; i<lpMap->Points; i++ )
		if (lpMap->Pnt[i].x != lpMap->Pnt[i].y)
			break;

	if (i==lpMap->Points)
	{
		MakeQuickMap( lpMap );
		return;
	}
}
#endif

lpMap->bModified = YES;
// If an additive model, invert the map
if ( lpMap->Reverse & REVERSE_POINTSXY)
	InvertMap( lpMap );

bUseHMS = (lpMap->Brightness[BAND_HI] || lpMap->Brightness[BAND_MID] || lpMap->Brightness[BAND_SHADOWS]);

bCurves = ((lpMap->iCurves != 0) && (lpMap->Points > 2));

if (bCurves)
{
	val = (double)lpMap->iCurves/10.0;
	val = Bound(val, 6.0, 17.0);
}
	

for (i=BAND_ALL; i<=BAND_SHADOWS; i++)
	if ( Brightness[i] = lpMap->Brightness[i] )
   	{
   		if ( Brightness[i] < 0 )
			Brightness[i] = -TOGRAY( -Brightness[i] );
  		 else	Brightness[i] =  TOGRAY(  Brightness[i] );
	}

fxScale = fyScale = -1;
if ( Contrast = lpMap->Contrast )
   if ( Contrast < 0 )
	fyScale = FGET( 100+Contrast, 100 ); // Squash vertically - y scaling
   else	fxScale = FGET( 100-Contrast, 100 ); // Squash horizontally - x scaling

/* First check to see if the points need to be stretched */
bStretch = (lpMap->bStretch == YES && lpMap->Points == CALPOINTS);
if ( bStretch )
	{
	/* Compute StepMin and StepMax */
	StepMin = StepMax = lpMap->Pnt[lpMap->Points-1].x;
	for ( i=0; i<lpMap->Points-1; i++ )
		{
		x = lpMap->Pnt[i].x;
		if ( x > StepMax ) StepMax = x;
		if ( x < StepMin ) StepMin = x;
		}
	if ( StepMin > 0 || StepMax < 255 )
		StepMax -= StepMin;
	else	bStretch = NO;
	}

bReverseHMS = lpMap->bReverseHMS;
gamma = lpMap->gamma;
if (gamma != 1.0)
{
	// doble negative is positive
 	if (((lpMap->Reverse & REVERSE_POINTSXY) || lpMap->Negate) &&
 			!((lpMap->Reverse & REVERSE_POINTSXY) && lpMap->Negate))
		bGamma = -1;
	else
		bGamma = 1;
}
/* Compute the luts values from the connected points */
// setup interpolation
if (bCurves)
{
	// setup interpolation
	iCurvePoints = 0;	
	i = 0;
	ptCurve[iCurvePoints] = lpMap->Pnt[i++];
	iCurvePoints++;
	if (ptCurve[0].x > 0)
	{
		ptCurve[1] = ptCurve[0];
		ptCurve[0].x = 0;
		iCurvePoints++;
	}	
	for (;i<lpMap->Points;i++)
	{
		ptCurve[iCurvePoints] = lpMap->Pnt[i];
		// weed out duplicate x's
		if (ptCurve[iCurvePoints].x > ptCurve[iCurvePoints-1].x)
			iCurvePoints++;
	}
	if (ptCurve[iCurvePoints-1].x < 255)
	{
		// ran out!
		ptCurve[iCurvePoints].y = ptCurve[iCurvePoints-1].y;
		ptCurve[iCurvePoints].x = 255;
		iCurvePoints++;
	}
	Spline(ptCurve, iCurvePoints, 1.0e30, 1.0e30, spline, val);
}
x1 = 0;
y1 = lpMap->Pnt[0].y;
minx = lpMap->Pnt[0].x;
maxx = lpMap->Pnt[lpMap->Points-1].x;
if ( fyScale >= 0 )
	y1 = FMUL( (y1 - 128), fyScale ) + 128;

for (i=0; i<=lpMap->Points; i++) /* go through all points */
	{
	if ( i == lpMap->Points )
		{
		x2 = 255;
		y2 = lpMap->Pnt[lpMap->Points-1].y;
		}
	else	
		{
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
			if (bCurves)
			{
				// ensure that ends are horizontal
				if (!WITHIN(x, minx,maxx))
//					value = y1 + (((m * (x - x1))+128) / 256L);
					fValue = (double)y1 + ((double)(m * (x - x1)) / 256.);
				else
				{
//					value = (int)Splint(lpMap->Pnt, spline, lpMap->Points, x, val);
					fValue = Splint(lpMap->Pnt, spline, lpMap->Points, x, val);
					fValue = Bound(fValue, 0., 255.);
				}
			}
			else
//				value = y1 + (((m * (x - x1))+128) / 256L);
				fValue = (double)y1 + ((double)(m * (x - x1)) / 256.);
			if (bUseHMS)
			{
				HMSBrightness = Brightness[0];
				if (bReverseHMS)
				{
					HMSBrightness += ((long)(Brightness[1])*HMSMaps[0][255-x])>>8L;
					HMSBrightness += ((long)(Brightness[2])*HMSMaps[1][255-x])>>8L;
					HMSBrightness += ((long)(Brightness[3])*HMSMaps[2][255-x])>>8L;
				}
				else
				{
					HMSBrightness += ((long)(Brightness[1])*HMSMaps[0][x])>>8L;
					HMSBrightness += ((long)(Brightness[2])*HMSMaps[1][x])>>8L;
					HMSBrightness += ((long)(Brightness[3])*HMSMaps[2][x])>>8L;
				}
				fValue -= HMSBrightness;
			}
			else 
				fValue -= Brightness[0];
			
			// do gamma
			if (bGamma)
			{
				if (bGamma > 0)
					fValue = pow(fValue/255.0,gamma)*255.0;
				else
					fValue = (1.0-pow((1.0-fValue/255.0),gamma))*255.0;
					
			}
			fValue = mbound( fValue, 0.0, 255.0 );
			if ( lpMap->Negate )
//				lpMap->Lut[x] = 255 - value;
				lpFloatLut[x] = (float)(255. - fValue);
			else
//				lpMap->Lut[x] = value;
				lpFloatLut[x] = (float)fValue;
			}
/*		if ( lpMap->Negate )
			ramp( lpMap->Lut, x1, 255-y1, x2, 255-y2 );
		else	ramp( lpMap->Lut, x1, y1, x2, y2 ); */
		}
	x1 = x2;
	y1 = y2;
	}

// Invert it back to its original state
if ( lpMap->Reverse & REVERSE_POINTSXY)
	InvertMap( lpMap );
if ( lpMap->Reverse & REVERSE_LUTXY)
	MapReflectXY(lpFloatLut);
}

//************************************************************************/
void MapCreateLut(LPFLOAT lpPoints, LPTR lpLut)
//************************************************************************/
{
	int i;
	for (i = 0; i < LUTSIZE; ++i)
		lpLut[i] = (int)(lpPoints[i] + 0.5);
}

//************************************************************************/
void MapCreateFloatMap(LPTR lpLut, LPFLOAT lpPoints)
//************************************************************************/
{
	int i;
	for (i = 0; i < LUTSIZE; ++i)
		lpPoints[i] = (FLOAT)lpLut[i];
}

//************************************************************************/
void MapReflectXY(LPFLOAT lpPoints)
//************************************************************************/
{
	#ifdef WIN32
	FLOATPT lpPtTable[LUTSIZE];
	#endif
	int j;

	// now flip GainPnts over y == x to create
	// compensation curve
	for (j = 0; j < LUTSIZE; ++j)
	{
		lpPtTable[j].x = lpPoints[j];
		lpPtTable[j].y = ( float )j;
		lpPoints[j]    = ( float )j;
	}
	MapLookup(lpPoints, lpPtTable);
}

//************************************************************************/
void MapCombine(LPFLOAT lpLut1, LPFLOAT lpLut2)
//************************************************************************/
{
	#ifdef WIN32
	FLOATPT lpPtTable[LUTSIZE];
	#endif
	int j;

	for (j = 0; j < LUTSIZE; ++j)
	{
		lpPtTable[j].x = ( float )j;
		lpPtTable[j].y = lpLut2[j];
	}
	MapLookup(lpLut1, lpPtTable);
}

// looks up lpPoints x-values in lpTable and places result
// in lpPoints y-values
//************************************************************************/
void MapLookup(LPFLOAT lpLut, LPFLOATPT lpTable)
//************************************************************************/
{
	int i;
	int lower, upper;
	float xDelta, x;
	float xLower, yLower, xDiff;
	float xUpper, yUpper, yDiff;

	// now interpolate values in curve
	lower = 0;
	upper = 0;

	// straight line over to left axis
	xLower = ( float )0;
	yLower = lpTable[lower].y;

	xUpper = xLower;
	yUpper = yLower;

	for (i = 0; i < LUTSIZE; ++i)
	{
		x = lpLut[i];
		// see if we need to get a new upper
		while (xUpper < x && upper < LUTSIZE)
		{
			if (upper > lower)
			{
				++lower;
				xLower = lpTable[lower].x;
				yLower = lpTable[lower].y;
			}
			if (upper < (LUTSIZE-1))
			{
				++upper;
				xUpper = lpTable[upper].x;
				yUpper = lpTable[upper].y;
			}
			else
			{
				xUpper = ( float )255; // over to x-axis
				yUpper = lpTable[upper].y;
			}
		}
		xDiff = xUpper - xLower;
		if (xDiff != 0.0)
		{
			yDiff = yUpper - yLower;
			xDelta = x - xLower;
			lpLut[i] = ( ((xDelta/xDiff) * yDiff) + yLower );
		}
		else
			lpLut[i] = yLower;
	}
}

//************************************************************************/
//	This assumes strait line maps (yes I know all lines are strait).
//************************************************************************/
void MakeQuickMap( LPMAP lpMap )
/************************************************************************/
{
int i, x;
int Brightness[4], Contrast;
long value;
LFIXED D;
BOOL bReverseHMS, bUseHMS;

lpMap->bModified = YES;
// If an additive model, invert the map
if ( lpMap->Reverse & REVERSE_POINTSXY)
	InvertMap( lpMap );
// get brightness for each band
for (i=BAND_ALL; i<=BAND_SHADOWS; i++)
{
	if ( Brightness[i] = lpMap->Brightness[i] )
	   if ( Brightness[i] < 0 )
			Brightness[i] = -TOGRAY( -Brightness[i] );
	   else	
	   		Brightness[i] =  TOGRAY(  Brightness[i] );
}
// Compute dy/dx
if ( !(Contrast = lpMap->Contrast))
	D = FUNITY;
else if ( Contrast < 0 )
	D = FGET(Contrast, 100) + FUNITY;
else
{
	// check extreme case of dy/dx = infinite
	if (Contrast >=100)
		D = FGET(255, 1);				//large number
	else
		D = FGET(100, 100-Contrast);
}

bUseHMS = (Brightness[BAND_HI] || Brightness[BAND_MID] || Brightness[BAND_SHADOWS]);
bReverseHMS = lpMap->bReverseHMS && bUseHMS;

// make lut
for (x=0; x<=255; x++)
{
	// special case BAND_ALL
	value = FMUL(x-128, D) + 128;
	value = mbound(value, 0, 255);  
	value -= Brightness[0];
	// do scaled bands
	if (bReverseHMS)
	{
		value -= ((long)(Brightness[BAND_HI])*HMSMaps[0][255-x])>>8L;
		value -= ((long)(Brightness[BAND_MID])*HMSMaps[1][255-x])>>8L;
		value -= ((long)(Brightness[BAND_SHADOWS])*HMSMaps[2][255-x])>>8L;
	}
	else if (bUseHMS)
	{
		value -= ((long)(Brightness[BAND_HI])*HMSMaps[0][x])>>8L;
		value -= ((long)(Brightness[BAND_MID])*HMSMaps[1][x])>>8L;
		value -= ((long)(Brightness[BAND_SHADOWS])*HMSMaps[2][x])>>8L;
	}
		value = mbound(value, 0, 255);
	if ( lpMap->Negate )
		lpMap->Lut[x] = 255 - ( BYTE )value;
	else	
		lpMap->Lut[x] = ( BYTE )value;
}

/* Posterize it (if necessary) */
PosterizeMap(lpMap);

/* Threshold it ( if Necessary) */
ThresholdMap(lpMap);

// Invert it back to its original state
if ( lpMap->Reverse & REVERSE_POINTSXY)
	InvertMap( lpMap );
}

/************************************************************************/
static void PosterizeMap( LPMAP lpMap )
/************************************************************************/
{
int i, brk, value;
LFIXED fbreak, fbreakincr, fvalue, fvalueincr;

/* Posterize it (if necessary) */
if ( lpMap->Levels < 256 )
	{
	if ( lpMap->Levels > 1 )
		{
		fbreakincr = FGET( 256, lpMap->Levels );
		fvalueincr = FGET( 256, lpMap->Levels-1 );
		fvalue = 0;
		value = lpMap->Lut[0];
		}
	else	{
		fbreakincr = FGET( 256, 1 );
		fvalueincr = 0;
		fvalue = 0;
		value = lpMap->Lut[128];
		}
	fbreak = fbreakincr;
	brk = FMUL( 1, fbreak );
	for (i=0; i<256; i++)
		{
		if ( i >= brk )
			{
			fvalue += fvalueincr;
			value = FMUL( 1, fvalue );
			value = lpMap->Lut[ mbound( value, 0, 255 ) ];
			fbreak += fbreakincr;
			brk = FMUL( 1, fbreak );
			}
		lpMap->Lut[i] = value;
		}
	}
}

/************************************************************************/
static void ThresholdMap( LPMAP lpMap )
/************************************************************************/
{
int i, j;

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
}

/************************************************************************/
void ResetMap( LPMAP lpMap, int maxpoints, int type )
/************************************************************************/
{
ResetMapEx(lpMap, maxpoints, type, NO);
}

/************************************************************************/
void ResetMapEx( LPMAP lpMap, int maxpoints, int type, BOOL bReverseHMS )
/************************************************************************/
{
int i;
LFIXED rate;

rate = FGET( 255, maxpoints-1 );
lpMap->Points = maxpoints;
for ( i=0; i<maxpoints; i++ )
	lpMap->Pnt[i].x = lpMap->Pnt[i].y = FMUL( i, rate );
for ( i=maxpoints; i<MAXPOINTS; i++ )
	lpMap->Pnt[i].x = lpMap->Pnt[i].y = 0;
if ( type >= 0 )
	lpMap->Reverse = type;

lpMap->bStretch = NO;
lpMap->iCurves = 0;
lpMap->gamma = 1.0;
lpMap->Contrast = 0;
lpMap->Brightness[0] = 0;
lpMap->Brightness[1] = 0;
lpMap->Brightness[2] = 0;
lpMap->Brightness[3] = 0;
lpMap->Threshold = 0;
lpMap->Negate = NO;
lpMap->Levels = 256;
lpMap->bReverseHMS = bReverseHMS;
MakeMap( lpMap );
lpMap->bModified = NO;
}


/************************************************************************/
void InvertMap( LPMAP lpMap )
/************************************************************************/
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
lpMap->Brightness[0] = -lpMap->Brightness[0];
lpMap->Brightness[1] = -lpMap->Brightness[1];
lpMap->Brightness[2] = -lpMap->Brightness[2];
lpMap->Brightness[3] = -lpMap->Brightness[3];
}


/************************************************************************/
void ReverseMap( LPMAP lpMap, BOOL fMakeMap )
/************************************************************************/
{
// flip state of reverse bit
lpMap->Reverse ^= REVERSE_POINTSXY;
if (fMakeMap)
	MakeMap(lpMap);
}

//************************************************************************
//
// PARAMETERS:
//					LPPOINT lpC- The control points for the sspline.
//					int num- Size of lpControl. (max 4 as is)
//					double d1- The derivitive at lpControl[0].
//					double d2- The derivitive at lpControl[num-1].
//					double *lpOut- Should be num in size.
//					double Val- tension factor
//
// DESCRIPTION:
//			Fills lpOut with the second derivitives to be fed into
//			Splint().  
//			(From Numerical Recipies in C William H Press,  et al.)
//
//************************************************************************
void Spline(LPPOINT lpC, int num, double d1, double d2, double *lpOut, double iVal)
//************************************************************************
{
	int i, k;
	double p, qn, sig, un;
	double u[MAXPOINTS+2];
	
	if (d1 > 0.99e30)
		lpOut[0] = u[0] = 0.0;
	else
	{
		lpOut[0] = -0.5;
		u[0] = (3.0/(lpC[1].x-lpC[0].x))*
				((double)(lpC[1].y-lpC[0].y)/(lpC[1].x-lpC[0].x) - d1);
	}
	for (i=1; i<num-1;i++)
	{
		sig = (double)(lpC[i].x - lpC[i-1].x)/(lpC[i+1].x - lpC[i-1].x);
		p = sig*lpOut[i-1]+2.0;
		lpOut[i] = (sig-1.0)/p;
		u[i] = (double)(lpC[i+1].y-lpC[i].y)/(lpC[i+1].x-lpC[i].x)-
					(double)(lpC[i].y-lpC[i-1].y)/(lpC[i].x-lpC[i-1].x);
		u[i] = (6.0*u[i]/(lpC[i+1].x - lpC[i-1].x)-sig*u[i-1])/p;
	}
	if (d2 > 0.99e30)
		qn=un=0.0;
	else
	{
		qn = 0.5;
		un = (3.0/(lpC[num-1].x - lpC[num-2].x))* 
				(d2-(double)(lpC[num-1].y-lpC[num-2].y)/
				(lpC[num-1].x-lpC[num-2].x));
	}
	lpOut[num-1]= (un-qn*u[num-2])/(qn*lpOut[num-2]+1.0);
	for (k=num-2; k>=0;k--)
		lpOut[k]=lpOut[k]*lpOut[k+1]+u[k];
}


//************************************************************************
//
// PARAMETERS:
//					LPPOINT lpC- The control points for the sspline.
//					double *lpSp- the output from Spline.
//					int num- Size of lpControl. (max 4 as is)
//					int x- The coordinate at which to interpolate. 
//					double Val- tension factor
// DESCRIPTION:
//			Interpolates x on the given curve.  Call Spline() first.
//			(From Numerical Recipies in C William H Press,  et al.)
// RETURNS:
//			The interpolated y for the given x.
//************************************************************************
double Splint(LPPOINT lpC, double *lpSp, int num, int x, double Val)
//************************************************************************
{
	int klo, khi, k;
	double h,b,a, y;
	
	klo = 0;
	khi = num-1;
	while(khi-klo > 1)
	{
		k=(khi+klo) >> 1;
		if (lpC[k].x > x)
			khi = k;
		else
			klo = k;
	}
	h = (double)lpC[khi].x-lpC[klo].x;
	if (h==0)
		return(255.0);
	a = (double)(lpC[khi].x - x)/h;
	b = (double)(x-lpC[klo].x)/h;
	y = a*lpC[klo].y+b*lpC[khi].y+
			((a*a*a-a)*lpSp[klo]+(b*b*b-b)*lpSp[khi])*(h*h)/Val;
	return(y);
}


