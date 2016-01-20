//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

static LPTR lpUCRlut; // alloc 256 bytes
static LPTR lpDotlut; // alloc 256 bytes
static LPWORD lpInklut; // alloc 65536 bytes
static void ClrFrameRGBtoCMYK(LPRGB lpRGB, LPCMYK lpCMYK, int iCount);
static void ClrFrameCMYKtoRGB(LPCMYK lpCMYK, LPRGB lpRGB, int iCount);

#ifndef STATIC16	// Only in new framelib
#define RGB2CMYKPROC LPROC
#define CMYK2RGBPROC LPROC
#endif

/***********************************************************************/
void InitRGBtoCMYK()
/***********************************************************************/
{
LFIXED rate;
LPWORD lpInk;
int v, i, j, k, l;
int c, m, y, dc, dc1, dc2, dm, dm1, dm2, dy, dy1, dy2;
LFIXED TakeMfromR, TakeYfromR, TakeYfromG, TakeCfromG, TakeCfromB, TakeMfromB;
static int UCR, Hilight, Shadow, MfromR, YfromG, CfromB;

SetRGBToCMYKProc( (RGB2CMYKPROC)ClrFrameRGBtoCMYK, (CMYK2RGBPROC)ClrFrameCMYKtoRGB);

// make sure some paramater has changed to recalculate all the tables
if (UCR == Separate.UCR &&
    Hilight == Halftone.Hilight && 
    Shadow == Halftone.Shadow &&
    MfromR == Separate.MfromR &&
    YfromG == Separate.YfromG &&
    CfromB == Separate.CfromB &&
    lpUCRlut)
	return;

if ( lpUCRlut ) { FreeUp( lpUCRlut ); lpUCRlut = NULL; }
if ( lpDotlut ) { FreeUp( lpDotlut ); lpDotlut = NULL; }
if ( lpInklut ) { FreeUp( (LPTR)lpInklut ); lpInklut = NULL; }

// Make the lpUCRlut for faster processing
if ( lpUCRlut = Alloc( 256L ) )
	{
	rate = FGET( Separate.UCR, 100 );
	for ( v=0; v<256; v++ )
		lpUCRlut[v] = FMUL( v, rate );
	}

// Make the lpDotlut for faster processing
if ( lpDotlut = Alloc( 256L ) )
	{
	for ( v=0; v<Halftone.Hilight; v++ )
		lpDotlut[v] = Halftone.Hilight;
	for ( v=Halftone.Hilight; v<Halftone.Shadow; v++ )
		lpDotlut[v] = v;
	for ( v=Halftone.Shadow; v<256; v++ )
		lpDotlut[v] = Halftone.Shadow;
	}

TakeMfromR = FGET( 100 - bound( 100+Separate.MfromR, 0, 100 ), 100 );
TakeYfromR = FGET( 100 - bound( 100-Separate.MfromR, 0, 100 ), 100 );
TakeYfromG = FGET( 100 - bound( 100+Separate.YfromG, 0, 100 ), 100 );
TakeCfromG = FGET( 100 - bound( 100-Separate.YfromG, 0, 100 ), 100 );
TakeCfromB = FGET( 100 - bound( 100+Separate.CfromB, 0, 100 ), 100 );
TakeMfromB = FGET( 100 - bound( 100-Separate.CfromB, 0, 100 ), 100 );

// Make the lpInklut for faster processing
if ( TakeMfromR || TakeYfromR || // We can skip ink corr if nothing to do
     TakeYfromG || TakeCfromG ||
     TakeCfromB || TakeMfromB )
     if ( lpInklut = (LPWORD)Alloc(65536L) )
	{
	for ( i=0; i<32; i++ )
	  {
	  c = ToMaxi[i];
	  for ( j=0; j<32; j++ )
	    {
	    m = ToMaxi[j];
	    for ( k=0; k<32; k++ )
		{
		y = ToMaxi[k];
		dc = dc1 = dc2 = 0;
		dm = dm1 = dm2 = 0;
		dy = dy1 = dy2 = 0;

		for ( l=0; l<3; l++ )
		    {
		    if ( TakeMfromR )	dm1 = FMUL( y - dy, TakeMfromR );
		    else
		    if ( TakeYfromR )	dy1 = FMUL( m - dm, TakeYfromR );

		    if ( TakeYfromG )	dy2 = FMUL( c - dc, TakeYfromG );
		    else
		    if ( TakeCfromG )	dc2 = FMUL( y - dy, TakeCfromG );

		    if ( TakeCfromB )	dc1 = FMUL( m - dm, TakeCfromB );
		    else
		    if ( TakeMfromB )	dm2 = FMUL( c - dc, TakeMfromB );

		    dc = max(dc1,dc2);
		    dm = max(dm1,dm2);
		    dy = max(dy1,dy2);

		    dc = min( c, dc );
		    dm = min( m, dm );
		    dy = min( y, dy );
		    }

		lpInk = lpInklut + ToMiniRGB( i, j, k );
		*lpInk = ToMiniRGB( DN(dc), DN(dm), DN(dy) );
		}
	    }
	  }
	}

UCR = Separate.UCR;
Hilight = Halftone.Hilight;
Shadow = Halftone.Shadow;
MfromR = Separate.MfromR;
YfromG = Separate.YfromG;
CfromB = Separate.CfromB;

if ( !Control.Retail )
	{
	Separate.DoInkCorrect = NO; // Don't allow it
	Separate.DoBlackAndUCR = NO; // Only allow the default
	}
}

/***********************************************************************/

void EndRGBtoCMYK()
{
	SetRGBToCMYKProc( (RGB2CMYKPROC)NULL, (CMYK2RGBPROC)NULL );
}

/***********************************************************************/

static void ClrFrameRGBtoCMYK(LPRGB lpRGB, LPCMYK lpCMYK, int iCount)
{
	register int c, m, y, k;
	WORD v, wMini;

	while ( --iCount >= 0 )
	{
		// RGB to CMY conversion
		c = 255 - lpRGB->red;
		m = 255 - lpRGB->green;
		y = 255 - lpRGB->blue;
		lpRGB++;

		if ( Separate.DoBlackAndUCR && lpUCRlut )
		{ // Black generation
			v = min( c, min( m, y ) );
			k = Separate.BGMap.Lut[v];

			if ( k > v ) k = v;

			// Under Color removal
			if ( v = lpUCRlut[ k ] )
			{
				c = c - v; // no bounding necessary if k is bound above
				m = m - v;
				y = y - v;
			}
		}
		else
			k = 0;

		if ( Separate.DoInkCorrect && lpInklut )
		{ // Ink Correction
			wMini = *(lpInklut + ToMiniRGB( DN(c), DN(m), DN(y) ));
			c -= MaxiR( wMini );
			m -= MaxiG( wMini );
			y -= MaxiB( wMini );
		}

		lpCMYK->c = c;
		lpCMYK->m = m;
		lpCMYK->y = y;
		lpCMYK->k = k;

		lpCMYK++;
	}
}

// Used for both GDI printing and DCS
/***********************************************************************/
VOID ClrRGBtoCMYK(
/***********************************************************************/
LPRGB 	lpRGB,
LPTR 	lpCyan,
LPTR	lpMagenta, 
LPTR	lpYellow, 
LPTR	lpBlack,
int 	iCount,
BOOL 	fCalibrate)
{
register int c, m, y, k;
WORD v, wMini;

while ( --iCount >= 0 )
	{
	// RGB to CMY conversion
	c = 255 - lpRGB->red;
	m = 255 - lpRGB->green;
	y = 255 - lpRGB->blue;
	lpRGB++;

	if ( Separate.DoBlackAndUCR && lpUCRlut )
		{ // Black generation
		v = min( c, min( m, y ) );
		k = Separate.BGMap.Lut[v];
		if ( k > v ) k = v;
		// Under Color removal
		if ( v = lpUCRlut[ k ] )
			{
			c = c - v; // no bounding necessary if k is bound above
			m = m - v;
			y = y - v;
			}
		}
	else
		k = 0;

	if ( Separate.DoInkCorrect && lpInklut )
		{ // Ink Correction
		wMini = *(lpInklut + ToMiniRGB( DN(c), DN(m), DN(y) ));
		c -= MaxiR( wMini );
		m -= MaxiG( wMini );
		y -= MaxiB( wMini );
		}

	if ( Halftone.DoHalftoning && lpDotlut && fCalibrate )
		{ // Dot Clipping
		c = lpDotlut[ c ];
		m = lpDotlut[ m ];
		y = lpDotlut[ y ];
		k = lpDotlut[ k ];
		}

	if ( Calibrate.DoCalibration && fCalibrate )
		{ // Calibration
		k = Calibrate.CalMap[0].Lut[ k ];
		c = Calibrate.CalMap[1].Lut[ c ];
		m = Calibrate.CalMap[2].Lut[ m ];
		y = Calibrate.CalMap[3].Lut[ y ];
		}

	*lpCyan++    = c;
	*lpMagenta++ = m;
	*lpYellow++  = y;
	*lpBlack++   = k;
	}
}

// Used for GDI printing
/***********************************************************************/
VOID CorrectRGB(
/***********************************************************************/
LPRGB 	lpRGB,
int 	iCount,
BOOL 	fCalibrate,
BOOL 	fPS)
{
int c, m, y;
WORD wMini;

if ( !fCalibrate )
	return;

if ( !(Separate.DoInkCorrect && lpInklut)
  && !(Calibrate.DoCalibration && fCalibrate) )
	return;

while ( --iCount >= 0 )
	{
	// RGB to CMY conversion
	c = 255 - lpRGB->red;
	m = 255 - lpRGB->green;
	y = 255 - lpRGB->blue;

	if ( Separate.DoInkCorrect && lpInklut && fPS )
		{ // Ink Correction
		wMini = *(lpInklut + ToMiniRGB( DN(c), DN(m), DN(y) ));
		c -= MaxiR( wMini );
		m -= MaxiG( wMini );
		y -= MaxiB( wMini );
		}

	if ( Calibrate.DoCalibration && fCalibrate )
		{ // Calibration
		c = Calibrate.CalMap[1].Lut[ c ];
		m = Calibrate.CalMap[2].Lut[ m ];
		y = Calibrate.CalMap[3].Lut[ y ];
		}

	lpRGB->red   = 255 - c;
	lpRGB->green = 255 - m;
	lpRGB->blue  = 255 - y;
	lpRGB++;
	}
}

// Used for both Postscript and GDI printing
/***********************************************************************/
VOID CorrectGray(
/***********************************************************************/
LPTR 	lpGray,
int 	iCount,
BOOL 	fCalibrate,
BOOL 	fPS)
{
int k;

if ( !fCalibrate )
	return;
if ( !Calibrate.DoCalibration )
	return;

while ( --iCount >= 0 )
	{
	k = 255 - *lpGray;
	*lpGray++ = 255 - Calibrate.CalMap[0].Lut[ k ];
	}
}

/***********************************************************************/

static void ClrFrameCMYKtoRGB(LPCMYK lpCMYK, LPRGB lpRGB, int iCount)
{
	BYTE r, g, b, k, v;

	while (--iCount >= 0)
	{
		k = lpCMYK->k;
		r = lpCMYK->c^0xFF;
		g = lpCMYK->m^0xFF;
		b = lpCMYK->y^0xFF;

		lpCMYK++;

		if ( lpUCRlut && (v = lpUCRlut[ k ]) )
		{
			if (r > v)
				r -= v;
			else	r = 0;
			if (g > v)
				g -= v;
			else	g = 0;
			if (b > v)
				b -= v;
			else	b = 0;
		}
		lpRGB->red   = r;
		lpRGB->green = g;
		lpRGB->blue  = b;
		lpRGB++;
	}
}

/***********************************************************************/
VOID ClrCMYKtoRGB(
/***********************************************************************/
LPTR 	lpCyan, 
LPTR 	lpMagenta, 
LPTR 	lpYellow, 
LPTR 	lpBlack, 
LPRGB 	lpRGB, 
int 	iCount)
{
BYTE r, g, b, k, v;

while (--iCount >= 0)
	{
	k = *lpBlack++;
	r = 255 - *lpCyan++;
	g = 255 - *lpMagenta++;
	b = 255 - *lpYellow++;
	if ( lpUCRlut && (v = lpUCRlut[ k ]) )
		{
		if (r > v)
			r -= v;
		else	r = 0;
		if (g > v)
			g -= v;
		else	g = 0;
		if (b > v)
			b -= v;
		else	b = 0;
		}
	lpRGB->red = r;
	lpRGB->green = g;
	lpRGB->blue = b;
	lpRGB++;
	}
}
