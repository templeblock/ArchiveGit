// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include <math.h>
#include "superlib.h"
#include "macros.h"

void ResetMap( LPMAP lpMap, int maxpoints, BOOL Reverse );
void InvertMap( LPMAP lpMap );
void MakeQuickMap( LPMAP lpMap );
static void PosterizeMap(LPMAP lpMap);
static void ThresholdMap(LPMAP lpMap);

static LPINT16 HMSMaps[3]; 	// the highlight midtone shadow maps


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
int i, x, x1, x2, y1, y2, value, bStretch;
int StepMin, StepMax, Brightness[4], Contrast;
int HMSBrightness;
BOOL bReverseHMS;
LFIXED fxScale, fyScale;
long m;
BOOL bUseHMS, bGamma;
double gamma;

if (lpMap->gamma == 1.0)
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

lpMap->bModified = YES;
// If an additive model, invert the map
if ( lpMap->Reverse )
	InvertMap( lpMap );

bUseHMS = (lpMap->Brightness[BAND_HI] || lpMap->Brightness[BAND_MID] || lpMap->Brightness[BAND_SHADOWS]);

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
bGamma = (gamma != 1.0);

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
			value = y1 + (((m * (x - x1))+128) / 256L);
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
				value -= HMSBrightness;
			}
			else 
				value -= Brightness[0];
			if (bGamma)
				value = ((int)(pow((double)(value)/255.0,gamma)*255.0 + 0.5));
			value = mbound( value, 0, 255 );
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
PosterizeMap(lpMap);

/* Threshold it ( if Necessary) */
ThresholdMap(lpMap);

// Invert it back to its original state
if ( lpMap->Reverse )
	InvertMap( lpMap );
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
if ( lpMap->Reverse )
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
		lpMap->Lut[x] = 255 - value;
	else	
		lpMap->Lut[x] = value;
}

/* Posterize it (if necessary) */
PosterizeMap(lpMap);

/* Threshold it ( if Necessary) */
ThresholdMap(lpMap);

// Invert it back to its original state
if ( lpMap->Reverse )
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


