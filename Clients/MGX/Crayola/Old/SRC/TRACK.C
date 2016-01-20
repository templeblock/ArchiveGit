//®PL1¯®FD1¯®TP0¯®BT0¯®RM150¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static data
static DISTORT Distort;
static TFORM FTrackTForm;

/************************************************************************/
void TrackInit( LPRECT lpRect )
/************************************************************************/
{
// get bounding rectangle we are working with
Distort.Rect = Distort.RectOrig = *lpRect;
Distort.p[0].x = Distort.Rect.left;  Distort.p[0].y = Distort.Rect.top;
Distort.p[1].x = Distort.Rect.right; Distort.p[1].y = Distort.Rect.top;
Distort.p[2].x = Distort.Rect.right; Distort.p[2].y = Distort.Rect.bottom;
Distort.p[3].x = Distort.Rect.left;  Distort.p[3].y = Distort.Rect.bottom;

// Save file tform and setup display tform
TInit( &FTrackTForm );
}

/************************************************************************/
void TrackGetForwardDistort( LPDISTORT lpDistort )
/************************************************************************/
{
DISTORT d;

d.RectOrig = Distort.RectOrig;
Transformer( &FTrackTForm, &Distort.p[0], (LPINT)&d.p[0].x, (LPINT)&d.p[0].y );
Transformer( &FTrackTForm, &Distort.p[1], (LPINT)&d.p[1].x, (LPINT)&d.p[1].y );
Transformer( &FTrackTForm, &Distort.p[2], (LPINT)&d.p[2].x, (LPINT)&d.p[2].y );
Transformer( &FTrackTForm, &Distort.p[3], (LPINT)&d.p[3].x, (LPINT)&d.p[3].y );
d.Rect.left   = min( min( min( d.p[0].x, d.p[1].x ), d.p[2].x ), d.p[3].x );
d.Rect.right  = max( max( max( d.p[0].x, d.p[1].x ), d.p[2].x ), d.p[3].x );
d.Rect.top    = min( min( min( d.p[0].y, d.p[1].y ), d.p[2].y ), d.p[3].y );
d.Rect.bottom = max( max( max( d.p[0].y, d.p[1].y ), d.p[2].y ), d.p[3].y );
*lpDistort = d;
}

/************************************************************************/
void TrackGetRect( LPRECT lpRect )
/************************************************************************/
{
*lpRect = Distort.Rect;
TransformRect( &FTrackTForm, lpRect, lpRect );
}

/************************************************************************/
void TrackSetTForm( LPTFORM lpTForm, BOOL fDisplay )
/************************************************************************/
{
FTrackTForm = *lpTForm;
}

/************************************************************************/
void TrackGetTForm( LPTFORM lpTForm )
/************************************************************************/
{
*lpTForm = FTrackTForm;
}


/************************************************************************/
void TrackMapOriginalRect( LPDISTORT lpDistort, LPRECT lpSrcRect, LPRECT lpDstRect )
/************************************************************************/
{
POINT p0, p1, p2, p3;

// The distort structure passed in was created with TrackGetForwardDistort()
// which means the transform has already been applied to the distort points
p0.x = lpSrcRect->left;
p0.y = lpSrcRect->top;
TrackMapOriginalPoint( lpDistort, &p0, &p0 );
p1.x = lpSrcRect->right;
p1.y = lpSrcRect->top;
TrackMapOriginalPoint( lpDistort, &p1, &p1 );
p2.x = lpSrcRect->right;
p2.y = lpSrcRect->bottom;
TrackMapOriginalPoint( lpDistort, &p2, &p2 );
p3.x = lpSrcRect->left;
p3.y = lpSrcRect->bottom;
TrackMapOriginalPoint( lpDistort, &p3, &p3 );

lpDstRect->left   = min( min( min( p0.x, p1.x ), p2.x ), p3.x );
lpDstRect->right  = max( max( max( p0.x, p1.x ), p2.x ), p3.x );
lpDstRect->top    = min( min( min( p0.y, p1.y ), p2.y ), p3.y );
lpDstRect->bottom = max( max( max( p0.y, p1.y ), p2.y ), p3.y );
}


/************************************************************************/
static void TrackMapOriginalPoint( LPDISTORT lpDistort, LPPOINT lpSrc, LPPOINT lpDst )
/************************************************************************/
{
int Numer, Denom;
POINT p0, p1, p2, p3;
LFIXED fPercentX, fPercentY;
DISTORT Distort;

Distort = *lpDistort;

// Compute P0 and p2 based on the X percentage

Numer = lpSrc->x - Distort.RectOrig.left;
Denom = Distort.RectOrig.right - Distort.RectOrig.left;

if ( Numer == Denom /*UNITY*/ )
	{
	p0.x  = Distort.p[1].x;
	p0.y  = Distort.p[1].y;
	p2.x  = Distort.p[2].x;
	p2.y  = Distort.p[2].y;
	}
else
if ( !Numer )
	{
	p0.x  = Distort.p[0].x;
	p0.y  = Distort.p[0].y;
	p2.x  = Distort.p[3].x;
	p2.y  = Distort.p[3].y;
	}
else
	{
	fPercentX = FGET( Numer, Denom );
	p0.x  = Distort.p[0].x + FMUL( Distort.p[1].x - Distort.p[0].x, fPercentX );
	p0.y  = Distort.p[0].y + FMUL( Distort.p[1].y - Distort.p[0].y, fPercentX );
	p2.x  = Distort.p[2].x + FMUL( Distort.p[3].x - Distort.p[2].x, UNITY-fPercentX );
	p2.y  = Distort.p[2].y + FMUL( Distort.p[3].y - Distort.p[2].y, UNITY-fPercentX );
	}

// Compute P1 and p3 based on the Y percentage

Numer = lpSrc->y - Distort.RectOrig.top;
Denom = Distort.RectOrig.bottom - Distort.RectOrig.top;

if ( Numer == Denom /*UNITY*/ )
	{
	p1.x  = Distort.p[2].x;
	p1.y  = Distort.p[2].y;
	p3.x  = Distort.p[3].x;
	p3.y  = Distort.p[3].y;
	}
else
if ( !Numer )
	{
	p1.x  = Distort.p[1].x;
	p1.y  = Distort.p[1].y;
	p3.x  = Distort.p[0].x;
	p3.y  = Distort.p[0].y;
	}
else
	{
	fPercentY = FGET( Numer, Denom );
	p1.x  = Distort.p[1].x + FMUL( Distort.p[2].x - Distort.p[1].x, fPercentY );
	p1.y  = Distort.p[1].y + FMUL( Distort.p[2].y - Distort.p[1].y, fPercentY );
	p3.x  = Distort.p[3].x + FMUL( Distort.p[0].x - Distort.p[3].x, UNITY-fPercentY );
	p3.y  = Distort.p[3].y + FMUL( Distort.p[0].y - Distort.p[3].y, UNITY-fPercentY );
	}

// Compute the points at the same x and y percentages of each of the 4 segments
// The intersection of P02 and P13 is our new point
IntersectSegments( p0, p2, p1, p3, lpDst );
}


/************************************************************************/
static void IntersectSegments( POINT A0, POINT A1, POINT B0, POINT B1, LPPOINT lpPt )
/************************************************************************/
{ // Compute the intersection of segment A and segment B
int dx, dxA, dyA, dxB, dyB;
LFIXED fxAB, fyAB, fNumer, fDenom, fSlopeA, fSlopeB;
POINT T;

if ( (dxA = A1.x - A0.x) < 0 ) dxA = -dxA;
if ( (dyA = A1.y - A0.y) < 0 ) dyA = -dyA;
if ( (dxB = B1.x - B0.x) < 0 ) dxB = -dxB;
if ( (dyB = B1.y - B0.y) < 0 ) dyB = -dyB;
if ( min( dxA, dyB ) < min( dyA, dxB ) ) // In the quest for accuracy
	{ // swap the segments
	T = A0; A0 = B0; B0 = T;
	T = A1; A1 = B1; B1 = T;
	}

dxA = A1.x - A0.x;
dyA = A1.y - A0.y;
dxB = B1.x - B0.x;
dyB = B1.y - B0.y;

if ( !dxA || !dyB )
	{ // If the slope is still undefined, get out
	*lpPt = A0;
	return;
	}

fSlopeA = FGET( dyA, dxA );
fSlopeB = FGET( dxB, dyB );
fxAB = TOFIXED( B0.x - A0.x );
fyAB = TOFIXED( B0.y - A0.y );
fNumer = fxAB - FMUL( fyAB, fSlopeB );
fDenom = UNITY - FMUL( fSlopeA, fSlopeB );
dx = (fNumer / fDenom);
lpPt->x = A0.x + dx;
lpPt->y = A0.y + FMUL( dx, fSlopeA );
}
