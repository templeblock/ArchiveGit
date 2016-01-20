// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®PL1¯®FD1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

/***********************************************************************/
LPFRAME ConvertLongColor( lpFrame )
/***********************************************************************/
LPFRAME lpFrame;
{
LPFRAME lpNewFrame;
int x, y, yTotal;
LPTR lpOut, lp, lpBuffer;

/* Open the new image */
x = lpFrame->Xsize;
y = lpFrame->Ysize/3;
yTotal = y;
if ( !(lpNewFrame = frame_open( 3, x, y, lpFrame->Resolution )) )
	{
	Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
	return( NULL );
	}

/*if ( !AllocLines( &lpBuffer, 1, x, 3 ) )
	{
	Message( IDS_EMEMALLOC );
	frame_close( lpNewFrame );
	return( NULL );
	}*/

/* Do the frame conversion */
for ( y=0; y<yTotal; y++ )
	{
	AstralClockCursor( y, yTotal );
	if ( !(lpOut = frame_ptr( 0, 0, y, YES )) )
		break;
	//ConvertData( lpBuffer, 3, x, lpOut, 2 );
	frame_set( lpFrame );
	if ( !(lp = frame_ptr( 0, 0, y, NO )) )
		break;
	SetEachThirdValue( lp, lpOut, x );
	if ( !(lp = frame_ptr( 0, 0, y+yTotal, NO )) )
		break;
	SetEachThirdValue( lp, lpOut+1, x );
	if ( !(lp = frame_ptr( 0, 0, y+yTotal+yTotal, NO )) )
		break;
	SetEachThirdValue( lp, lpOut+2, x );
	frame_set( lpNewFrame );
	}

//FreeUp( lpBuffer );
if ( !lp || !lpOut )
	{ // If we got a null pointer reading the image
	Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
	frame_set( lpFrame );
	return( NULL );
	}

frame_set( lpNewFrame );
return( lpNewFrame );
}


/***********************************************************************/
void ConvertLongColor24( lpFrame, lpNewFrame )
/***********************************************************************/
LPFRAME lpFrame;
LPFRAME lpNewFrame;
{
int i, x, y, yTotal;
LPTR lpOut, lp;

/* Open the new image */
x = lpFrame->Xsize;
y = lpFrame->Ysize/3;

yTotal = y;
/* Do the frame conversion */
for ( y=0; y<yTotal; y++ )
	{
	AstralClockCursor( y, yTotal );
	frame_set( lpNewFrame );
	if ( !(lpOut = frame_ptr( 0, 0, y, YES )) )
		break;
	frame_set( lpFrame );
	if ( !(lp = frame_ptr( 0, 0, y, NO )) )
		break;
	SetEachThirdValue( lp, lpOut, x );
	if ( !(lp = frame_ptr( 0, 0, y+yTotal, NO )) )
		break;
	SetEachThirdValue( lp, lpOut+1, x );
	if ( !(lp = frame_ptr( 0, 0, y+yTotal+yTotal, NO )) )
		break;
	SetEachThirdValue( lp, lpOut+2, x );
	}

if ( !lp || !lpOut )
	{ /* If we got a null pointer reading the image */
	Print( "Error writing to cache file" );
	frame_set( lpFrame );
	return;
	}

frame_set( lpNewFrame );
}


/***********************************************************************/
void SetEachThirdValue( lpIn, lpOut, iCount )
/***********************************************************************/
LPTR lpIn, lpOut;
int iCount;
{
while ( --iCount >= 0 )
	{
	*lpOut = *lpIn++;
	lpOut += 3;
	}
}


/***********************************************************************/
LPFRAME ConvertWideColor( lpFrame )
/***********************************************************************/
LPFRAME lpFrame;
{
LPFRAME lpNewFrame;
SPAN span;
int x, y, yTotal;
LPTR lpOut;
LPRGB lpRGB;

/* Open the new image */
x = lpFrame->Xsize/3;
y = lpFrame->Ysize;
yTotal = y;
if ( !(lpNewFrame = frame_open( 2, x, y, lpFrame->Resolution )) )
	{
	Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
	return( NULL );
	}

/* Allocate space for the output line buffer */
if ( !AllocLines( &lpOut, 1, x, 2 ) )
	{
	Message( IDS_EMEMALLOC );
	frame_close( lpNewFrame );
	return( NULL );
	}

/* Do the frame conversion */
span.sx = 0;
span.sy = 0;
span.dx = x;
span.dy = 1;
for ( y=0; y<yTotal; y++ )
	{
	AstralClockCursor( y, yTotal );
	frame_set( lpFrame );
	if ( !(lpRGB = (LPRGB)frame_ptr( 0, 0, y, NO )) )
		break;
	ConvertData( (LPTR)lpRGB, 3, x, lpOut, 2 );
	frame_set( lpNewFrame );
	if ( !frame_write( &span, lpOut, x ) )
		{
		Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
		}
	span.sy++;
	}

FreeUp( lpOut );
if ( !lpRGB )
	{ // If we got a null pointer reading the image
	Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
	frame_set( lpFrame );
	return( NULL );
	}

frame_set( lpNewFrame );
return( lpNewFrame );
}


/************************************************************************/
WORD AverageMiniRGB( val1, val2 )
/************************************************************************/
WORD val1, val2;
{
WORD r, g, b;

r = ( MiniR(val1) + MiniR(val2) ) / 2;
g = ( MiniG(val1) + MiniG(val2) ) / 2;
b = ( MiniB(val1) + MiniB(val2) ) / 2;
return( ToMiniRGB( r, g, b ) );
}


/************************************************************************/
WORD AverageRGB( lpRGB1, lpRGB2 )
/************************************************************************/
LPRGB lpRGB1, lpRGB2;
{
RGBS rgb1;

rgb1.red   = ( (WORD)lpRGB1->red + (WORD)lpRGB2->red ) >> 1;
rgb1.green = ( (WORD)lpRGB1->green + (WORD)lpRGB2->green ) >> 1;
rgb1.blue  = ( (WORD)lpRGB1->blue + (WORD)lpRGB2->blue ) >> 1;
return( RGBtoMiniRGB((LPRGB)&rgb1) );
}


/***********************************************************************/
void ExpandRGB( lpMiniRGB, lpRGB, iCount )
/***********************************************************************/
LPWORD lpMiniRGB;
LPRGB lpRGB;
int iCount;
{
WORD wRGB;

while ( --iCount >= 0 )
	{
	wRGB = *lpMiniRGB++;
	lpRGB->red   = MaxiR( wRGB );
	lpRGB->green = MaxiG( wRGB );
	lpRGB->blue  = MaxiB( wRGB );
	lpRGB++;
	}
}


/************************************************************************/
void UnshuffleRGB( lpRGB, lpRed, lpGreen, lpBlue, iCount )
/************************************************************************/
LPRGB lpRGB;
LPTR lpRed, lpGreen, lpBlue;
int iCount;
{
LPTR lp;

lp = (LPTR)lpRGB;
while ( --iCount >= 0 )
	{
	*lpRed++   = *lp++;
	*lpGreen++ = *lp++;
	*lpBlue++  = *lp++;
	}
}


/***********************************************************************/
VOID RGBtoCMYK( lpRGB, lpCyan, lpMagenta, lpYellow, lpBlack, iCount )
/***********************************************************************/
LPRGB lpRGB;
register LPTR lpCyan, lpMagenta, lpYellow, lpBlack;
register int iCount;
{
register LPTR lp;
register int c, m, y, k;
WORD v;

lp = (LPTR)lpRGB;
while ( --iCount >= 0 )
	{
	// RGB to CMY conversion - are Ci, Mi, Yi Log or Linear?
	c = 255 - *lp++;
	m = 255 - *lp++;
	y = 255 - *lp++;

	// Gamut correction - Tone Reproduction
	//c = ToneRepro[c];
	//m = ToneRepro[m];
	//y = ToneRepro[y];

	if ( !Printer.DoBlackAndUCR )
		k = 0;
	else	{
		// Black generation
		v = min( c, min( m, y ) );
		k = Printer.BGMap.Lut[v];
		if ( k > v ) k = v;
		// Under Color removal
		if ( v = Page.UCRlut[ k ] )
			{
			c = c - v; // no bounding necessary if k is bound above
			m = m - v;
			y = y - v;
			}
		}

	if ( Printer.DoInkCorrect )
		{
		// Ink correction - None for black?
		//c = ci - (y * CinG) - (m * CinB);   MFORMR
		//m = mi - (c * MinB) - (y * MinR);   YFROMG
		//y = yi - (m * YinR) - (c * YinG);   CFROMB
		}

	if ( Printer.DoHalftoning )
		{
		// Dot clipping - part of Tone Repro?
		c = Page.Dotlut[ c ];
		m = Page.Dotlut[ m ];
		y = Page.Dotlut[ y ];
		k = Page.Dotlut[ k ];
		}

	if ( Printer.DoCalibration )
		{
		// Calibration
		c = Printer.CalMap[0].Lut[ c ];
		m = Printer.CalMap[1].Lut[ m ];
		y = Printer.CalMap[2].Lut[ y ];
		k = Printer.CalMap[3].Lut[ k ];
		}

	*lpCyan++    = c;
	*lpMagenta++ = m;
	*lpYellow++  = y;
	*lpBlack++   = k;
	}
}
