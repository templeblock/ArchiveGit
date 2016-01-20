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

static int LastSize, LastStyle, LastPressure;
static int LastOpacity = -1, LastFuzzyPixels = -1;
static int xc, yc;

int HalfBrush;
int FullBrush;
int LastShape;

/************************************************************************/
VOID SetupMask()
/************************************************************************/
{
BYTE BrushVal;

if (Retouch.BrushShape == IDC_BRUSHCUSTOM)
	return;

if ( LastSize        == Retouch.BrushSize  &&
     LastStyle       == Retouch.BrushStyle &&
     LastShape       == Retouch.BrushShape &&
     LastPressure    == Retouch.Pressure   &&
     LastOpacity     == Retouch.Opacity    &&
     LastFuzzyPixels == Retouch.FuzzyPixels )
	return;

LastSize        = FullBrush = Retouch.BrushSize;
LastStyle       = Retouch.BrushStyle;
LastShape       = Retouch.BrushShape;
LastPressure    = Retouch.Pressure;
LastOpacity     = Retouch.Opacity;
LastFuzzyPixels = Retouch.FuzzyPixels;
if (FullBrush < 1) FullBrush = 1;
if (FullBrush > MAX_BRUSH_SIZE)
	FullBrush = MAX_BRUSH_SIZE;
// Circle ONLY????
if ( Retouch.BrushShape == IDC_BRUSHCIRCLE )
	{ // Make sure its odd
	if ( !(FullBrush & 1) )
		FullBrush++;
	}
HalfBrush = FullBrush/2;

BrushVal = 255;
if ( Retouch.BrushShape == IDC_BRUSHSQUARE ||
   ( Retouch.BrushShape == IDC_BRUSHCIRCLE && FullBrush < 3 ) )
	set( Retouch.BrushMask, sizeof(Retouch.BrushMask), BrushVal );
else
if ( Retouch.BrushShape == IDC_BRUSHCIRCLE )
	circle_mask( BrushVal );
else
if ( Retouch.BrushShape == IDC_BRUSHHORIZONTAL )
	horizontal_mask( BrushVal );
else
if ( Retouch.BrushShape == IDC_BRUSHVERTICAL )
	vertical_mask( BrushVal );
else
if ( Retouch.BrushShape == IDC_BRUSHSLASH )
	slash_mask( BrushVal );
else
if ( Retouch.BrushShape == IDC_BRUSHBACKSLASH )
	backslash_mask( BrushVal );

SetupFuzzyBrush();
}

/************************************************************************/
BOOL SetupCustomBrushMask()
/************************************************************************/
{
int x, y;
int bpp, type;
LPTR lpLine;
LPFRAME lpFrame, lpOldFrame;
FNAME szFileName;

LastShape = Retouch.BrushShape;
set( Retouch.BrushMask, sizeof(Retouch.BrushMask), 255);
if ( !LookupExtFile( Retouch.CustomBrushName, szFileName, IDN_CUSTOM ) )
	return(FALSE);
if (!(lpFrame = AstralFrameLoad(szFileName, 1, &bpp, &type)))
	return(FALSE);
if (lpFrame->Ysize > MAX_BRUSH_SIZE || lpFrame->Xsize > MAX_BRUSH_SIZE)
	{
	Print("Invalid image size for a custom brush");
	frame_close(lpFrame);
	return(FALSE);
	}

Retouch.BrushSize = max( lpFrame->Xsize, lpFrame->Ysize );
FullBrush = Retouch.BrushSize;
HalfBrush = FullBrush/2;
lpOldFrame = frame_set(lpFrame);
for (y = 0; y < FullBrush; ++y)
	{
	lpLine = frame_ptr(0, 0, y, NO);
	if (!lpLine)
		continue;
	for (x = 0; x < FullBrush; ++x, lpLine++)
		BRMASK(x, y) = *lpLine ^ 255;
	}
frame_set(lpOldFrame);
frame_close(lpFrame);
Retouch.FuzzyPixels = 0;
SetupFuzzyBrush();
Retouch.FuzzyPixels = LastFuzzyPixels;
LastShape = IDC_BRUSHCUSTOM;
return(TRUE);
}


/************************************************************************/
VOID horizontal_mask( BrushVal )
/************************************************************************/
int BrushVal;
{
int x;

clr( Retouch.BrushMask, sizeof(Retouch.BrushMask) );
for ( x = 0; x < FullBrush; x++ )
	BRMASK( x, HalfBrush ) = BrushVal;
}


/************************************************************************/
VOID vertical_mask( BrushVal )
/************************************************************************/
int BrushVal;
{
int y;

clr( Retouch.BrushMask, sizeof(Retouch.BrushMask) );
for ( y = 0; y < FullBrush; y++ )
	BRMASK( HalfBrush, y ) = BrushVal;
}


/************************************************************************/
VOID slash_mask( BrushVal )
/************************************************************************/
int BrushVal;
{
int x, y;

clr( Retouch.BrushMask, sizeof(Retouch.BrushMask) );
x = FullBrush - 1;
for ( y = 1; y < FullBrush; y++ )
	{
	BRMASK( x, y ) = BrushVal;
	BRMASK( --x, y ) = BrushVal;
	}
}


/************************************************************************/
VOID backslash_mask( BrushVal )
/************************************************************************/
int BrushVal;
{
int x, y;

clr( Retouch.BrushMask, sizeof(Retouch.BrushMask) );
x = 0;
for ( y = 1; y < FullBrush; y++ )
	{
	BRMASK( x, y ) = BrushVal;
	BRMASK( ++x, y ) = BrushVal;
	}
}


/************************************************************************/
VOID circle_mask( BrushVal )
/************************************************************************/
int BrushVal;
{
static BYTE BrushThreshold[ (MAX_BRUSH_SIZE+1) * (MAX_BRUSH_SIZE+1) ];
static BOOL first = TRUE;
int xcenter, ycenter, xpos, ypos, x, y, thresh;

if (first)
    {
    BuildThresholdTable(BrushThreshold);
    first = FALSE;
    }
clr( Retouch.BrushMask, sizeof(Retouch.BrushMask) );
xcenter = (MAX_BRUSH_SIZE+1)/2;
ycenter = (MAX_BRUSH_SIZE+1)/2;
ypos = ycenter - HalfBrush;
for (y = 0; y < FullBrush; ++y, ++ypos)
    {
    xpos = xcenter - HalfBrush;
    for (x = 0; x < FullBrush; ++x, ++xpos)
	{
	thresh = BrushThreshold[(ypos*(MAX_BRUSH_SIZE+1))+xpos];
	if (thresh && thresh <= FullBrush)
	    BRMASK(x, y) = BrushVal;
	}
    }
}


/************************************************************************/
VOID DrawBrushOutline(xcenter, ycenter, dxBrush, dyBrush, BrushOutline)
/************************************************************************/
int xcenter;
int ycenter;
int dxBrush;
int dyBrush;
BYTE BrushOutline[ (MAX_BRUSH_SIZE+1) * (MAX_BRUSH_SIZE+1) ];
{
int x = 0;
int y = dyBrush;
long a = dxBrush;
long b = dyBrush;
long asqrd = a * a;
long twoasqrd = 2 * asqrd;
long bsqrd = b * b;
long twobsqrd = 2 * bsqrd;
long d,dx,dy;

xc = xcenter;
yc = ycenter;
d = bsqrd - asqrd*b + asqrd/4L;
dx = 0;
dy = twoasqrd * b;

while(dx<dy)
	{
	set4pixels(x,y,BrushOutline);
	if(d > 0L)
		{
		--y;
		dy -= twoasqrd;
		d -= dy;
		}
	++x;
	dx += twobsqrd;
	d += bsqrd + dx;
	}

d += (3L*(asqrd-bsqrd)/2L - (dx+dy)) / 2L;

while(y>=0)
	{
	set4pixels(x,y,BrushOutline);
	if(d < 0L)
		{
		++x;
		dx += twobsqrd;
		d += dx;
		}
	--y;
	dy -= twoasqrd;
	d += asqrd - dy;
	}
}

/************************************************************************/
VOID set4pixels( xp, yp, BrushOutline )
/************************************************************************/
int xp, yp;
BYTE BrushOutline[ (MAX_BRUSH_SIZE+1) * (MAX_BRUSH_SIZE+1) ];
{
int x, y;

x = xc+xp; y = yc+yp; BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x] = ON;
x = xc-xp; y = yc+yp; if ( xp ) BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x] = ON;
x = xc+xp; y = yc-yp; if ( yp ) BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x] = ON;
x = xc-xp; y = yc-yp; if ( xp && yp ) BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x] = ON;
}

/************************************************************************/
VOID FillBrushOutline(xcenter, ycenter, BrushOutline)
/************************************************************************/
int xcenter;
int ycenter;
BYTE BrushOutline[ (MAX_BRUSH_SIZE+1) * (MAX_BRUSH_SIZE+1) ];
{
int x, y;

for (y = ycenter; y >= 0; --y)
    {
    if (BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + xcenter])
	break;
    for (x = xcenter; x >= 0; --x)
	{
        if (BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x])
	    break;
	BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x] = ON;
        }
    for (x = xcenter+1; x < (MAX_BRUSH_SIZE+1); ++x)
	{
        if (BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x])
	    break;
	BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x] = ON;
        }
    }
for (y = ycenter+1; y < (MAX_BRUSH_SIZE+1); ++y)
    {
    if (BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + xcenter])
	break;
    for (x = xcenter; x >= 0; --x)
	{
        if (BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x])
	    {
	    break;
	    }
	BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x] = ON;
        }
    for (x = xcenter+1; x < (MAX_BRUSH_SIZE+1); ++x)
	{
        if (BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x])
	    break;
	BrushOutline[(y*(MAX_BRUSH_SIZE+1)) + x] = ON;
        }
    }
}

/************************************************************************/
VOID BuildThresholdTable(BrushThreshold)
/************************************************************************/
BYTE BrushThreshold[ (MAX_BRUSH_SIZE+1) * (MAX_BRUSH_SIZE+1) ];
{
BYTE BrushOutline[ (MAX_BRUSH_SIZE+1) * (MAX_BRUSH_SIZE+1) ];
int BrushSize, xcenter, ycenter;
int iTotalSize, i;

iTotalSize = (MAX_BRUSH_SIZE+1)*(MAX_BRUSH_SIZE+1);
xcenter = (MAX_BRUSH_SIZE+1)/2;
ycenter = (MAX_BRUSH_SIZE+1)/2;
clr(BrushThreshold, iTotalSize);
for (BrushSize = 1; BrushSize <= (MAX_BRUSH_SIZE+1); ++BrushSize)
    {
    clr(BrushOutline, iTotalSize);
    DrawBrushOutline(xcenter, ycenter, BrushSize/2, BrushSize/2, BrushOutline);
    if (BrushSize > 1)
        FillBrushOutline(xcenter, ycenter, BrushOutline);
    for (i = 0; i < iTotalSize; ++i)
	{
	if (BrushOutline[i])
	    if (!BrushThreshold[i])
		BrushThreshold[i] = BrushSize;
	}
    }
}
