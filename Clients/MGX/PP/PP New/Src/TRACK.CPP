//®PL1¯®FD1¯®TP0¯®BT0¯®RM150¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"

extern "C"
	{
	double atan2(double, double);
	}

#define H_UL			1	// upper-left
#define H_UR 			2	// upper-right
#define H_LR			3	// lower-right
#define H_LL			4	// lower-left
#define H_TOP			5	// top middle
#define H_RIGHT 		6	// right middle
#define H_BOTTOM 		7	// bottom middle
#define H_LEFT			8	// left middle
#define H_ROTATE		9	// rotation point
#define H_CENTER		10	// rotation center point
#define H_CORNER_UL		11	// distorted upper-left
#define H_CORNER_UR 	12	// distorted upper-right
#define H_CORNER_LR		13	// distorted lower-right
#define H_CORNER_LL		14	// distorted lower-left
#define H_MOVE			16	// move object pseudo-handle

#define NUM_HANDLES 14
#define H_SIZE 6
#define H_OFFSET (H_SIZE/2)

// Static prototypes
static void TrackMapOriginalPoint( LPDISTORT lpDistort, LPPOINT lpSrc, LPPOINT lpDst );
static int ClosestDistortPoint( int index, int iMode );
static void IntersectSegments( POINT p0, POINT p1, POINT p2, POINT p3, LPPOINT lpPt );
static void TrackRotate( int x, int y, int hx, int hy );
static void TrackScale( int x, int y, int hx, int hy, int ax, int ay, BOOL fConstrain );
static void TrackShear( int x, int y, int hx, int hy, int fConstrainX, int fConstrainY );
static void TrackDrawProc( HWND hWnd, HDC hDC, BOOL on );
static void TrackDrawHandles( HWND hWnd, HDC hDC, BOOL on, LPPOINT lpPt);
static void TrackDrawHandle( HWND hWnd, HDC hDC, LPRECT lpRect, BOOL fCircle, int on );

// Static data
static int handle_grabbed, iHiHandleInUse, iLoHandleInUse, track_moved;
static int perspective, iTrackMode, last_x, last_y;
static BOOL fRotateConstrainX, fRotateConstrainY, fMoveOnly;
static void (FAR *lpTrackDrawRoutine)(HWND hWnd, HDC hDC, BOOL on);
static TFORM FTrackTForm;
static LFIXED TrackAngleX, TrackAngleY;
static LFIXED TrackScaleX, TrackScaleY;
static POINT ptCenter, ptRotate;
static DISTORT Distort;
static RECT hRects[NUM_HANDLES];
static HPEN hPen;
static int  FrameRes;

static RECT StartRect;
static TFORM StartTForm;
static LFIXED StartAngleX, StartAngleY, StartRotateX, StartRotateY;
static LFIXED StartScaleX, StartScaleY;

/************************************************************************/
void TrackInit(HWND hWnd, int iMode,
				void (FAR *tlpDrawRoutine)(HWND hWnd, HDC hDC, BOOL on),
				LPRECT lpRect,
				LFIXED fScale)
/************************************************************************/
{
LFIXED fdx, fdy;

// setup plug in drawing routine
if ( !(lpTrackDrawRoutine = tlpDrawRoutine) )
	lpTrackDrawRoutine = TrackDrawProc;

hPen = CreatePen(PS_DOT, 1, RGB(0,0,0));
GetStockObject(WHITE_PEN);

// get bounding rectangle we are working with
Distort.Rect = Distort.RectOrig = *lpRect;
Distort.p[0].x = Distort.Rect.left;  Distort.p[0].y = Distort.Rect.top;
Distort.p[1].x = Distort.Rect.right; Distort.p[1].y = Distort.Rect.top;
Distort.p[2].x = Distort.Rect.right; Distort.p[2].y = Distort.Rect.bottom;
Distort.p[3].x = Distort.Rect.left;  Distort.p[3].y = Distort.Rect.bottom;
ptCenter.x = (Distort.Rect.left+Distort.Rect.right)/2;
ptCenter.y = (Distort.Rect.top+Distort.Rect.bottom)/2;
ptRotate.x = ptCenter.x;
ptRotate.y = (Distort.Rect.top+ptCenter.y)/2;

// init to no handles grabbed and no transforming done
handle_grabbed = 0;
TrackAngleX = TrackAngleY = 0;
TrackScaleX = TrackScaleY = fScale;

// Save file tform and setup display tform
TInit( &FTrackTForm );
if (TrackScaleX != FUNITY || TrackScaleY != FUNITY)
	{
	fdx = FGET(Distort.Rect.left + Distort.Rect.right, 2);
	fdy = FGET(Distort.Rect.top + Distort.Rect.bottom, 2);
	TFMove(&FTrackTForm, -fdx, -fdy);
	TScale(&FTrackTForm, TrackScaleX, TrackScaleY);
	TFMove(&FTrackTForm, fdx, fdy);
	}
TrackMode( hWnd, iMode, NO );
TrackFlip( hWnd, NO, NO, NO );
TrackRotateConstrain( NO, NO );


fMoveOnly = YES;
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
	p2.x  = Distort.p[2].x + FMUL( Distort.p[3].x - Distort.p[2].x, -fPercentX+FUNITY );
	p2.y  = Distort.p[2].y + FMUL( Distort.p[3].y - Distort.p[2].y, -fPercentX+FUNITY );
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
	p3.x  = Distort.p[3].x + FMUL( Distort.p[0].x - Distort.p[3].x, -fPercentY+FUNITY );
	p3.y  = Distort.p[3].y + FMUL( Distort.p[0].y - Distort.p[3].y, -fPercentY+FUNITY );
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
fxAB = MAKEFIXED( B0.x - A0.x );
fyAB = MAKEFIXED( B0.y - A0.y );
fNumer = fxAB - FIXMUL( fyAB, fSlopeB );
fDenom = FUNITY-FIXMUL( fSlopeA, fSlopeB );
dx = FIXDIVWHOLE(fNumer, fDenom );
lpPt->x = A0.x + dx;
lpPt->y = A0.y + FMUL( dx, fSlopeA );
}


/************************************************************************/
void TrackMode( HWND hWnd, int iMode, BOOL fDisplay )
/************************************************************************/
{
if (fDisplay)
	TrackDraw(hWnd, NULL,OFF);

iTrackMode = iMode;
if ( iTrackMode != IDC_TRANSPERSPECTIVE && iTrackMode != IDC_TRANSDISTORT )
	{
	iLoHandleInUse = 1;
	iHiHandleInUse = 10;
	}
else
	{
	iLoHandleInUse = 9;
	iHiHandleInUse = 14;
	}

if (fDisplay)
	TrackDraw(hWnd, NULL,ON);
}

/************************************************************************/
void TrackRotateConstrain( BOOL fConstrainX, BOOL fConstrainY )
/************************************************************************/
{
fRotateConstrainX = fConstrainX;
fRotateConstrainY = fConstrainY;
}

/************************************************************************/
void TrackFlip(HWND hWnd, BOOL fFlipX, BOOL fFlipY, BOOL fDisplay)
/************************************************************************/
{
RECT rect;
LFIXED fcx, fcy, fsx, fsy;

if (fDisplay)
	TrackDraw(hWnd, NULL,OFF);

fMoveOnly = NO;
TransformRect(&FTrackTForm, &Distort.Rect, &rect);
fcx = FGET(rect.left+rect.right, 2);
fcy = FGET(rect.top+rect.bottom, 2);
TFMove(&FTrackTForm, -fcx, -fcy);
fsx = FUNITY;
fsy = FUNITY;
if (fFlipX)
	fsx = -fsx;
if (fFlipY)
	fsy = -fsy;
TScale(&FTrackTForm, fsx, fsy);
TFMove(&FTrackTForm, fcx, fcy);

if (fDisplay)
	TrackDraw(hWnd, NULL,ON);
}

/************************************************************************/
void TrackGrab(HWND hWnd, int x, int y)
/************************************************************************/
{
RECT rect;
int i, h;
POINT Point;

track_moved = FALSE;
handle_grabbed = 0;
Point.x = x;
Point.y = y;
for (h = iLoHandleInUse; h <= iHiHandleInUse; ++h)
	{
	i = h - 1;
	if (PtInRect(&hRects[i], Point))
		{
		handle_grabbed = h;
		break;
		}
	}
if (handle_grabbed)
	{
	if ( iTrackMode == IDC_TRANSSKEW
		&& handle_grabbed != H_MOVE
		&& handle_grabbed != H_CENTER
		&& handle_grabbed != H_ROTATE )
		{ // Shear when a corner or side handle is grabbed
      FrameRes = FrameResolution(ImgGetBaseEditFrame(GetImagePtr(hWnd)));
		if ( !CONTROL )
			{
			ConstrainXY(&x, &y, TRUE, TRUE, TRUE);
			}
		}
	}
else
	{
	TransformRect(&FTrackTForm, &Distort.Rect, &rect);
	File2DispRect(hWnd,&rect, &rect);
	if (PtInRect(&rect, Point))
		handle_grabbed = H_MOVE;
	}
if (handle_grabbed)
	{
	last_x = x;
	last_y = y;
	Display2File(hWnd,&last_x, &last_y);
	TransformRect(&FTrackTForm, &Distort.Rect, &StartRect);
	StartTForm = FTrackTForm;
	StartScaleX = TrackScaleX;
	StartScaleY = TrackScaleY;
	StartAngleX = TrackAngleX;
	StartAngleY = TrackAngleY;
	StartRotateX = ptRotate.x;
	StartRotateY = ptRotate.y;
	}
}

/************************************************************************/
BOOL TrackMove(HWND hWnd, int x, int y)
/************************************************************************/
{
POINT pt;
int i, midx, midy, dx, dy, dummy;
RECT StatusRect, FileRect;
TFORM DTrackTForm, ITrackTForm;
static int j, xPer, yPer;

if (!handle_grabbed)
	{
	TrackSetCursor(hWnd, x, y);
	return(FALSE);
	}
Display2File(hWnd,&x, &y);
FileRect = GetDisplayPtr(hWnd)->FileRect;
x = bound(x, FileRect.left, FileRect.right);
y = bound(y, FileRect.top, FileRect.bottom);
if (x == last_x && y == last_y)
	return(FALSE);
track_moved = YES;
TrackDraw(hWnd,NULL,OFF);

switch (handle_grabbed)
	{
	case H_UL:
		fMoveOnly = NO;
		if (iTrackMode == IDC_TRANSSKEW)
			TrackShear(x, y, Distort.Rect.left, Distort.Rect.top,
				ConstrainX(), ConstrainY());
		else
		if (iTrackMode == IDC_TRANSSCALE)
			TrackScale(x, y, Distort.Rect.left, Distort.Rect.top,
				Distort.Rect.right, Distort.Rect.bottom, CONTROL);
		break;

	case H_UR:
		fMoveOnly = NO;
		if (iTrackMode == IDC_TRANSSKEW)
			TrackShear(x, y, Distort.Rect.right, Distort.Rect.top,
				ConstrainX(), ConstrainY());
		else
		if (iTrackMode == IDC_TRANSSCALE)
			TrackScale(x, y, Distort.Rect.right, Distort.Rect.top,
				Distort.Rect.left, Distort.Rect.bottom, CONTROL);
		break;

	case H_LR:
		fMoveOnly = NO;
		if (iTrackMode == IDC_TRANSSKEW)
			TrackShear(x, y, Distort.Rect.right, Distort.Rect.bottom,
				ConstrainX(), ConstrainY());
		else
		if (iTrackMode == IDC_TRANSSCALE)
			TrackScale(x, y, Distort.Rect.right, Distort.Rect.bottom,
				Distort.Rect.left, Distort.Rect.top, CONTROL);
		break;

	case H_LL:
		fMoveOnly = NO;
		if (iTrackMode == IDC_TRANSSKEW)
			TrackShear(x, y, Distort.Rect.left, Distort.Rect.bottom,
				ConstrainX(), ConstrainY());
		else
		if (iTrackMode == IDC_TRANSSCALE)
			TrackScale(x, y, Distort.Rect.left, Distort.Rect.bottom,
				Distort.Rect.right, Distort.Rect.top, CONTROL);
		break;

	case H_TOP:
		fMoveOnly = NO;
		midx = (Distort.Rect.left + Distort.Rect.right)/2;
		if (iTrackMode == IDC_TRANSSKEW)
			TrackShear(x, y, midx, Distort.Rect.top,
				ConstrainX(), ConstrainY());
		else
		if (iTrackMode == IDC_TRANSSCALE)
			{
			pt.x = midx; pt.y = Distort.Rect.top;
			Transformer(&FTrackTForm, &pt, (LPINT)&x, (LPINT)&dummy);
			TrackScale(x, y, midx, Distort.Rect.top, midx, Distort.Rect.bottom, YES);
			}
		break;

	case H_RIGHT:
		fMoveOnly = NO;
		midy = (Distort.Rect.top + Distort.Rect.bottom)/2;
		if (iTrackMode == IDC_TRANSSKEW)
			TrackShear(x, y, Distort.Rect.right, midy,
				ConstrainX(), ConstrainY());
		else
		if (iTrackMode == IDC_TRANSSCALE)
			{
			pt.x = Distort.Rect.right, pt.y = midy;
			Transformer(&FTrackTForm, &pt, (LPINT)&dummy, (LPINT)&y);
			TrackScale(x, y, Distort.Rect.right, midy, Distort.Rect.left, midy, YES);
			}
		break;

	case H_BOTTOM:
		fMoveOnly = NO;
		midx = (Distort.Rect.left + Distort.Rect.right)/2;
		if (iTrackMode == IDC_TRANSSKEW)
			TrackShear(x, y, midx, Distort.Rect.bottom,
				ConstrainX(), ConstrainY());
		else
		if (iTrackMode == IDC_TRANSSCALE)
			{
			pt.x = midx; pt.y = Distort.Rect.bottom;
			Transformer(&FTrackTForm, &pt, (LPINT)&x, (LPINT)&dummy);
			TrackScale(x, y, midx, Distort.Rect.bottom, midx, Distort.Rect.top, YES);
			}
		break;

	case H_LEFT:
		fMoveOnly = NO;
		midy = (Distort.Rect.top + Distort.Rect.bottom)/2;
		if (iTrackMode == IDC_TRANSSKEW)
			TrackShear(x, y, Distort.Rect.left, midy,
				ConstrainX(), ConstrainY());
		else
		if (iTrackMode == IDC_TRANSSCALE)
			{
			pt.x = Distort.Rect.left; pt.y = midy;
			Transformer(&FTrackTForm, &pt, (LPINT)&dummy, (LPINT)&y);
			TrackScale(x, y, Distort.Rect.left, midy, Distort.Rect.right, midy, YES);
			}
		break;

	case H_CENTER:
		// transform points to transformed position
		Transformer(&FTrackTForm, &ptCenter, (LPINT)&ptCenter.x, (LPINT)&ptCenter.y);
		Transformer(&FTrackTForm, &ptRotate, (LPINT)&ptRotate.x, (LPINT)&ptRotate.y);
		dx = x - ptCenter.x;
		dy = y - ptCenter.y;
		ptRotate.x += dx;
		ptRotate.y += dy;
		ptCenter.x = x;
		ptCenter.y = y;
		ITrackTForm = FTrackTForm;
		TInvert(&ITrackTForm);
		// transform points back to untransformed position
		Transformer(&ITrackTForm, &ptCenter, (LPINT)&ptCenter.x, (LPINT)&ptCenter.y);
		Transformer(&ITrackTForm, &ptRotate, (LPINT)&ptRotate.x, (LPINT)&ptRotate.y);
		break;

	case H_CORNER_UL:
	case H_CORNER_UR:
	case H_CORNER_LR:
	case H_CORNER_LL:
		fMoveOnly = NO;
		i = handle_grabbed - H_CORNER_UL;
		ITrackTForm = FTrackTForm;
		TInvert(&ITrackTForm);
		// transform the new point back to its untransformed position
		pt.x = x;
		pt.y = y;
		Transformer(&ITrackTForm, &pt, (LPINT)&pt.x, (LPINT)&pt.y);
		if ( iTrackMode != IDC_TRANSPERSPECTIVE )
			{
			Distort.p[i].x = pt.x;
			Distort.p[i].y = pt.y;
			}
		else
			{
			if ( !perspective )
				{ // if not started yet...
				// transform the current point back to untransformed position
				// check the amount of the movement
				dx = abs( pt.x - Distort.p[i].x );
				dy = abs( pt.y - Distort.p[i].y );
				if ( (dx < SMALL_MOVEMENT && dy < SMALL_MOVEMENT) || dx == dy )
					break;
				if ( dx > dy )
					{ // Y constrain
					perspective = 'y';
					j = ClosestDistortPoint( i, perspective );
					if ( j == 0 )
						{ xPer = Distort.p[2].x + Distort.p[3].x; }
					else
					if ( j == 1 )
						{ xPer = Distort.p[2].x + Distort.p[3].x; }
					else
					if ( j == 2 )
						{ xPer = Distort.p[0].x + Distort.p[1].x; }
					else
					if ( j == 3 )
						{ xPer = Distort.p[0].x + Distort.p[1].x; }
					}
				else
					{ // X constrain
					perspective = 'x';
					j = ClosestDistortPoint( i, perspective );
					if ( j == 0 )
						{ yPer = Distort.p[1].y + Distort.p[2].y; }
					else
					if ( j == 1 )
						{ yPer = Distort.p[0].y + Distort.p[3].y; }
					else
					if ( j == 2 )
						{ yPer = Distort.p[0].y + Distort.p[3].y; }
					else
					if ( j == 3 )
						{ yPer = Distort.p[1].y + Distort.p[2].y; }
					}
				}
			Distort.p[i].x = pt.x;
			Distort.p[i].y = pt.y;
			if ( perspective == 'y' )
				{ // Y constrain
				Distort.p[j].x = xPer - pt.x;
				Distort.p[j].y = pt.y;
				}
			else // ( perspective == 'x' )
				{ // X constrain
				Distort.p[j].y = yPer - pt.y;
				Distort.p[j].x = pt.x;
				}
			}
		// Update the Distort.Rect rectangle
		Distort.Rect.left = min( min( min( Distort.p[0].x, Distort.p[1].x ),
			Distort.p[2].x ), Distort.p[3].x );
		Distort.Rect.right = max( max( max( Distort.p[0].x, Distort.p[1].x ),
			Distort.p[2].x ), Distort.p[3].x );
		Distort.Rect.top = min( min( min( Distort.p[0].y, Distort.p[1].y ),
			Distort.p[2].y ), Distort.p[3].y );
		Distort.Rect.bottom = max( max( max( Distort.p[0].y, Distort.p[1].y ),
			Distort.p[2].y ), Distort.p[3].y );
		break;

	case H_ROTATE:
		fMoveOnly = NO;
		// transform points to transformed position
		TrackRotate( x, y, StartRotateX, StartRotateY );
		ptRotate.x = x;
		ptRotate.y = y;
		ITrackTForm = FTrackTForm;
		TInvert(&ITrackTForm);
		// transform points back to untransformed position
		Transformer(&ITrackTForm, &ptRotate, (LPINT)&ptRotate.x, (LPINT)&ptRotate.y);
		break;

	case H_MOVE:
		dx = x - last_x;
		dy = y - last_y;
		TMove(&FTrackTForm, dx, dy);
		DTrackTForm = FTrackTForm;
		TransformRect(&FTrackTForm, &Distort.Rect, &StatusRect);
		StatusOfRectangle( &StatusRect, 0L, 
			FrameResolution(ImgGetBaseEditFrame(GetImagePtr(hWnd))));
		break;

	default:
		return(FALSE);
		break;
	}

TrackDraw(hWnd,NULL,ON);
last_x = x; last_y = y;
return(TRUE);
}

/************************************************************************/
static int ClosestDistortPoint( int index, int iMode )
/************************************************************************/
{
int i, iDelta, delta, iClosest;

if ( iMode == 'x' )
	{
	switch ( index )
		{
		case 0:
			return( 3 );
		case 1:
			return( 2 );
		case 2:
			return( 1 );
		case 3:
			return( 0 );
		}
	}
else // if ( iMode == 'y' )
	{
	switch ( index )
		{
		case 0:
			return( 1 );
		case 1:
			return( 0 );
		case 2:
			return( 3 );
		case 3:
			return( 2 );
		}
	}

iDelta = INT_MAX;
for ( i=0; i<4; i++ )
	{
	if ( i == index )
		continue;
	if ( iMode == 'x' )
		delta = abs( Distort.p[index].x - Distort.p[i].x );
	else // if ( iMode == 'y' )
		delta = abs( Distort.p[index].y - Distort.p[i].y );
	if ( delta < iDelta )
		{
		iClosest = i;
		iDelta = delta;
		}
	}
return( iClosest );
}


/************************************************************************/
void TrackUp(HWND hWnd, int x, int y)
/************************************************************************/
{
handle_grabbed = 0;
perspective = 0;
}

/************************************************************************/
BOOL TrackDone(HWND hWnd, BOOL undraw)
/************************************************************************/
{
if ( undraw )
	TrackDraw(hWnd, NULL,OFF);
if (hPen)
	{
	DeleteObject(hPen);
	hPen = NULL;
	}
SetClassCursor(hWnd, Window.hCursor );

// return whether the object was only moved
return( fMoveOnly );
}

/************************************************************************/
void TrackSetCursor(HWND hWnd, int x, int y)
/************************************************************************/
{
POINT Point;
int i, h;
LPCSTR lpCursor;
HCURSOR hCursor;
HINSTANCE hInst;
RECT rect;

Point.x = x;
Point.y = y;
lpCursor = NULL;
for (h = iLoHandleInUse; h <= iHiHandleInUse; ++h)
	{
	i = h - 1;
	if ( !PtInRect(&hRects[i], Point) )
		continue;
	switch ( h )
		{
		case H_ROTATE:
			lpCursor = MAKEINTRESOURCE(ID_ROTATE);
			hInst = PictPubApp.GetResourceHandle();
			break;
		case H_CENTER:
			lpCursor = (LPCSTR)IDC_ARROW;
			hInst = NULL;
			break;
		case H_CORNER_UL:
		case H_CORNER_UR:
		case H_CORNER_LR:
		case H_CORNER_LL:
			lpCursor = (LPCSTR)IDC_ARROW;
			hInst = NULL;
			break;
		case H_UL:
		case H_LR:
			if (iTrackMode == IDC_TRANSSKEW)
				{
				lpCursor = MAKEINTRESOURCE(ID_SKEWHORZ);
				hInst = PictPubApp.GetResourceHandle();
				}
			else
				{
				lpCursor = (LPCSTR)IDC_SIZENWSE;
				hInst = NULL;
				}
			break;
		case H_UR:
		case H_LL:
			if (iTrackMode == IDC_TRANSSKEW)
				{
				lpCursor = MAKEINTRESOURCE(ID_SKEWHORZ);
				hInst = PictPubApp.GetResourceHandle();
				}
			else
				{
				lpCursor = (LPCSTR)IDC_SIZENESW;
				hInst = NULL;
				}
			break;
		case H_TOP:
		case H_BOTTOM:
			if (iTrackMode == IDC_TRANSSKEW)
				{
				lpCursor = MAKEINTRESOURCE(ID_SKEWHORZ);
				hInst = PictPubApp.GetResourceHandle();
				}
			else
				{
				lpCursor = (LPCSTR)IDC_SIZENS;
				hInst = NULL;
				}
			break;
		case H_LEFT:
		case H_RIGHT:
			if (iTrackMode == IDC_TRANSSKEW)
				{
				lpCursor = MAKEINTRESOURCE(ID_SKEWVERT);
				hInst = PictPubApp.GetResourceHandle();
				}
			else
				{
				lpCursor = (LPCSTR)IDC_SIZEWE;
				hInst = NULL;
				}
			break;
		default:
			break;
		}
	break;
	}
if (!lpCursor)
	{
	TransformRect(&FTrackTForm, &Distort.Rect, &rect);
	File2DispRect(hWnd,&rect, &rect);
	if (PtInRect(&rect, Point))
		{
		lpCursor = IDC_SIZE;
		hInst = NULL;
		}
	}
if (lpCursor)
	{
	hCursor = LoadCursor( hInst, lpCursor );
	SetClassCursor(hWnd, hCursor);
	SetCursor( hCursor );
	}
else
	{
	SetClassCursor(hWnd, Window.hCursor );
	SetCursor( Window.hCursor );
	}
}

/************************************************************************/
BOOL TrackMoved( void )
/************************************************************************/
{
return(track_moved);
}

/************************************************************************/
void TrackGetRect( LPRECT lpRect )
/************************************************************************/
{
*lpRect = Distort.Rect;
TransformRect( &FTrackTForm, lpRect, lpRect );
}

/************************************************************************/
void TrackDelta(HWND hWnd, int dx, int dy)
/************************************************************************/
{
RECT StatusRect;
TFORM DTrackTForm;

track_moved = TRUE;
TrackDraw(hWnd, NULL,OFF);
TMove(&FTrackTForm, dx, dy);
DTrackTForm = FTrackTForm;
TransformRect(&FTrackTForm, &Distort.Rect, &StatusRect);
StatusOfRectangle( &StatusRect, 0L,
	FrameResolution(ImgGetBaseEditFrame(GetImagePtr(hWnd))));
TrackDraw(hWnd, NULL,ON);
}

/************************************************************************/
void TrackDraw( HWND hWnd, HDC hDC, BOOL on )
/************************************************************************/
{
(*lpTrackDrawRoutine)( hWnd, hDC, on );
}

/************************************************************************/
LFIXED TrackGetAngle( POINT center, int x, int y )
/************************************************************************/
{
int dx, dy;
double radians, angle;
#define PI 3.14159265

dx = x - center.x;
dy = center.y - y;

radians = atan2((double)dy, (double)dx);
angle = (radians * 180.0) / PI;
if (angle < 0)
		angle -= 0.1;
else	angle += 0.1;
return(DBL2FIX(angle));
}

/************************************************************************/
static void TrackRotate( int x, int y, int hx, int hy )
/************************************************************************/
{
LFIXED fdx, fdy;
LFIXED old_angle, new_angle, angle;
POINT center, pt;

pt.x = hx; pt.y = hy;
Transformer(&StartTForm, &pt, (LPINT)&hx, (LPINT)&hy);
Transformer(&StartTForm, &ptCenter, (LPINT)&center.x, (LPINT)&center.y);
FTransformer(&StartTForm, &ptCenter, &fdx, &fdy);
FTrackTForm = StartTForm;

TFMove( &FTrackTForm, -fdx, -fdy );
old_angle = TrackGetAngle(center, hx, hy);
new_angle = TrackGetAngle(center, x, y);
angle = old_angle - new_angle;
TRotate( &FTrackTForm,
	( fRotateConstrainX ? 0 : angle ),
	( fRotateConstrainY ? 0 : angle ) );
TFMove(&FTrackTForm, fdx, fdy);
TrackAngleX = StartAngleX + angle;
TrackAngleY = StartAngleY + angle;
if (TrackAngleX < 0)
   {
   TrackAngleX = -TrackAngleX;
	TrackAngleX = TOFIXED(360) - (TrackAngleX - ((long)(TrackAngleX / TOFIXED(360)) * 360));
   }

if (TrackAngleY < 0)
   {
   TrackAngleY = -TrackAngleY;
	TrackAngleY = TOFIXED(360) - (TrackAngleY - ((long)(TrackAngleY / TOFIXED(360)) * 360));
   }
TrackAngleX = TrackAngleX - ((long)(TrackAngleX / TOFIXED(360)) * 360);
TrackAngleY = TrackAngleY - ((long)(TrackAngleY / TOFIXED(360)) * 360);
if ( TrackAngleX != TrackAngleY )
		ReadOutF(IDS_ROTATEXY, 2, TrackAngleX, TrackAngleY );
else	ReadOutF(IDS_ROTATE, 1, TrackAngleX );

// if ( TrackAngleX != TrackAngleY )
//		MessageStatus(IDS_ROTATEXY, TrackAngleX, TrackAngleY);
// else	MessageStatus(IDS_ROTATE, TrackAngleX);
}


/************************************************************************/
static void TrackScale( int x, int y, int hx, int hy, int ax, int ay,
                           BOOL fConstrain )
/************************************************************************/
{
int minx, maxx, miny, maxy, dx, dy, midx, midy;
int old_width, old_height, new_width, new_height, xdiff, ydiff;
LFIXED scalex, scaley, fdx, fdy;
POINT pt;

pt.x = hx; pt.y = hy;
Transformer(&StartTForm, &pt, (LPINT)&hx, (LPINT)&hy);
FTrackTForm = StartTForm;
minx = StartRect.left; maxx = StartRect.right;
miny = StartRect.top; maxy = StartRect.bottom;

midx = (minx+maxx)/2;
midy = (miny+maxy)/2;
old_width = maxx - minx + 1;
old_height = maxy - miny + 1;

dx = x - hx;	/* positive is a move to the right */
dy = y - hy;	/* positive is a move to the bottom */

if (hx < midx)
		new_width = old_width - dx;
else 	new_width = old_width + dx;

if (hy < midy)
		new_height = old_height - dy;
else	new_height = old_height + dy;

if (new_width < 2)
	new_width = 2;
if (new_height < 2)
	new_height = 2;
scalex = FGET(new_width, old_width);
scaley = FGET(new_height, old_height);
if (!fConstrain)
	{
//	if (hx == minx || hx == maxx)
//		scaley = scalex;
//	else
//	if (hy == miny || hy == maxy)
//		scalex = scaley;
//	else
//		{
//		xdiff = min(abs(hx-minx), abs(hx-maxx));
//		ydiff = min(abs(hy-miny), abs(hy-maxy));
//		if (xdiff < ydiff)
//			scaley = scalex;
//		else
//			scalex = scaley;
//		}
	// to avoid scaling right off the image, always scale as little as possible
	scalex = scaley = max(scaley, scalex);
	}
pt.x = ax; pt.y = ay;
FTransformer(&FTrackTForm, &pt, &fdx, &fdy);
TFMove(&FTrackTForm, -fdx, -fdy);
TScale(&FTrackTForm, scalex, scaley);
TFMove(&FTrackTForm, fdx, fdy);

TrackScaleX = FIXMUL( StartScaleX, scalex );
TrackScaleY = FIXMUL( StartScaleY, scaley );

if ( TrackScaleX != TrackScaleY )
		ReadOutF(IDS_SCALEXY, 2, TrackScaleX * 100, TrackScaleY * 100 );
else	ReadOutF(IDS_SCALE, 1, TrackScaleX * 100 );
}


/************************************************************************/
static void TrackShear( int x, int y, int hx, int hy, BOOL fConstrainX,
                           BOOL fConstrainY )
/************************************************************************/
{
int minx, maxx, miny, maxy, dx, dy, midx, midy, ox, oy, length, count, inc;
LFIXED fmx, fmy, fnx, fny, fhx, fhy;
POINT pt;

ox = hx; oy = hy;
pt.x = hx; pt.y = hy;
Transformer(&StartTForm, &pt, (LPINT)&hx, (LPINT)&hy);
FTrackTForm = StartTForm;
minx = StartRect.left; maxx = StartRect.right;
miny = StartRect.top; maxy = StartRect.bottom;

// get center of handle positions
midx = (minx + maxx)/2;
midy = (miny + maxy)/2;

// get delta of move
dx = x - hx;	/* positive is a move to the right */
dy = y - hy;	/* positive is a move to the bottom */

if (fConstrainY)
	// If Constraining Y, Shear in X-Direction
	{
	if (hx == minx || hx == maxx) // left or right side
		{
		if (hy > midy)	// bottom side
			{
			length = hy - miny;
			}
		else
			{	// top side
			length = maxy - hy;
			dx = -dx;
			}
		}
	else if (hy == miny)	// top side
		{
		length = maxy - miny;
		dx = -dx;
		}
	else if (hy == maxy)   // bottom side
		{
		length = hy - miny;
		}
	else
		{ // the point is not at any extreme
		if (hy > midy)	// bottom side
			{
			length = hy - miny;
			}
		else
			{	// top side
			length = maxy - hy;
			dx = -dx;
			}
		}
	if (dx < 0)
		inc = -1;
	else
		inc = 1;
	count = dx/inc;
	while (--count >= 0)
		{
		TShearX(&FTrackTForm, length, inc);
		dx -= inc;
		}
	TShearX(&FTrackTForm, length, dx);
	}
else
	{ // if Constraining X, shear in Y-Direction
	if (hy == miny || hy == maxy)	// top or bottom side
		{
		if (hx > midx)	// right side
			{
			length = hx - minx;
			}
		else
			{	// left side
			length = maxx - hx;
			dy = -dy;
			}
		}
	else if (hx == minx)	// left side
		{
		length = maxx - hx;
		dy = -dy;
		}
	else if (hx == maxx)	// right side
		{
		length = hx - minx;
		}
	else
		{ // the point is not at any extreme
		if (hx > midx)	// right side
			{
			length = hx - minx;
			}
		else
			{	// left side
			length = maxx - hx;
			dy = -dy;
			}
		}
	if (dy < 0)
		inc = -1;
	else
		inc = 1;
	count = dy/inc;
	while (--count >= 0)
		{
		TShearY(&FTrackTForm, length, inc);
		dy -= inc;
		}
	TShearY(&FTrackTForm, length, dy);
	}

pt.x = ox;
pt.y = oy;
FTransformer(&FTrackTForm, &pt, &fhx, &fhy);
if (!fConstrainX)
	fhx = TOFIXED(x);
if (!fConstrainY)
	fhy = TOFIXED(y);
FTransformer(&FTrackTForm, &pt, &fnx, &fny);
fmx = fhx - fnx;
fmy = fhy - fny;
TFMove(&FTrackTForm, fmx, fmy);
}

/************************************************************************/
static void TrackDrawProc( HWND hWnd, HDC hDC, BOOL on )
/************************************************************************/
{
POINT pt[4];
int i, maxx, maxy;

Transformer( &FTrackTForm, &Distort.p[0], (LPINT)&pt[0].x, (LPINT)&pt[0].y );
Transformer( &FTrackTForm, &Distort.p[1], (LPINT)&pt[1].x, (LPINT)&pt[1].y );
Transformer( &FTrackTForm, &Distort.p[2], (LPINT)&pt[2].x, (LPINT)&pt[2].y );
Transformer( &FTrackTForm, &Distort.p[3], (LPINT)&pt[3].x, (LPINT)&pt[3].y );

// max x and max y must incremented by 1 so the box at proper spot
maxx = INT_MIN;
maxy = INT_MIN;
for (i = 0; i < 4; ++i)
	{
	if (pt[i].x > maxx)
		maxx = pt[i].x;
	if (pt[i].y > maxy)
		maxy = pt[i].y;
	}
for (i = 0; i < 4; ++i)
	{
	if (pt[i].x == maxx)
		++pt[i].x;
	if (pt[i].y == maxy)
		++pt[i].y;
	}

File2Display(hWnd, (LPINT)&pt[0].x, (LPINT)&pt[0].y );
File2Display(hWnd, (LPINT)&pt[1].x, (LPINT)&pt[1].y );
File2Display(hWnd, (LPINT)&pt[2].x, (LPINT)&pt[2].y );
File2Display(hWnd, (LPINT)&pt[3].x, (LPINT)&pt[3].y );

// max x and max y must decremented by 1 so the box draws around
maxx = INT_MIN;
maxy = INT_MIN;
for (i = 0; i < 4; ++i)
	{
	if (pt[i].x > maxx)
		maxx = pt[i].x;
	if (pt[i].y > maxy)
		maxy = pt[i].y;
	}
for (i = 0; i < 4; ++i)
	{
	if (pt[i].x == maxx)
		--pt[i].x;
	if (pt[i].y == maxy)
		--pt[i].y;
	}

if (!hDC)
	hDC = Window.hDC;
DrawLine( hDC, pt[0].x, pt[0].y, pt[1].x, pt[1].y, hPen );
DrawLine( hDC, pt[1].x, pt[1].y, pt[2].x, pt[2].y, hPen );
DrawLine( hDC, pt[2].x, pt[2].y, pt[3].x, pt[3].y, hPen );
DrawLine( hDC, pt[3].x, pt[3].y, pt[0].x, pt[0].y, hPen );
TrackDrawHandles( hWnd, hDC, on, pt );
}


/************************************************************************/
static void TrackDrawHandles( HWND hWnd, HDC hDC, BOOL on, LPPOINT lpPt )
/************************************************************************/
{
int i, h, j;
POINT ptR, ptC;

if (!hDC)
	hDC = Window.hDC;

ptR.x = ptR.y = ptC.x = ptC.y = 0;
for (h = iLoHandleInUse; h <= iHiHandleInUse; ++h)
	{
	i = h - 1;
	switch ( h )
		{
		case H_UL:
		case H_UR:
		case H_LR:
		case H_LL:
			hRects[i].left = lpPt[i].x;
			hRects[i].top = lpPt[i].y;
			break;
		case H_TOP:
			hRects[i].left = (lpPt[0].x + lpPt[1].x)/2;
			hRects[i].top = (lpPt[0].y + lpPt[1].y)/2;
			break;
		case H_RIGHT:
			hRects[i].left = (lpPt[1].x + lpPt[2].x)/2;
			hRects[i].top = (lpPt[1].y + lpPt[2].y)/2;
			break;
		case H_BOTTOM:
			hRects[i].left = (lpPt[2].x + lpPt[3].x)/2;
			hRects[i].top = (lpPt[2].y + lpPt[3].y)/2;
			break;
		case H_LEFT:
			hRects[i].left = (lpPt[3].x + lpPt[0].x)/2;
			hRects[i].top = (lpPt[3].y + lpPt[0].y)/2;
			break;
		case H_ROTATE:
			Transformer(&FTrackTForm, &ptRotate, (LPINT)&hRects[i].left,
				(LPINT)&hRects[i].top);
			File2Display(hWnd, (LPINT)&hRects[i].left, (LPINT)&hRects[i].top);
			ptR.x = hRects[i].left;
			ptR.y = hRects[i].top;
			break;
		case H_CENTER:
			Transformer(&FTrackTForm, &ptCenter, (LPINT)&hRects[i].left,
				(LPINT)&hRects[i].top);
			File2Display(hWnd,(LPINT)&hRects[i].left, (LPINT)&hRects[i].top);
			ptC.x = hRects[i].left;
			ptC.y = hRects[i].top;
			break;
		case H_CORNER_UL:
		case H_CORNER_UR:
		case H_CORNER_LR:
		case H_CORNER_LL:
			j = h - H_CORNER_UL;
			Transformer(&FTrackTForm, &Distort.p[j], (LPINT)&hRects[i].left,
				(LPINT)&hRects[i].top);
			File2Display(hWnd,(LPINT)&hRects[i].left, (LPINT)&hRects[i].top);
			break;
		default:
			break;
		}
	hRects[i].left -= H_OFFSET;
	hRects[i].top -= H_OFFSET;
	hRects[i].right = hRects[i].left + H_SIZE;
	hRects[i].bottom = hRects[i].top + H_SIZE;
	TrackDrawHandle(hWnd, hDC, &hRects[i], h >= H_CENTER, on);
	}

if ( ptR.x || ptR.y || ptC.x || ptC.y )
	Dline( hDC, ptC.x, ptC.y, ptR.x, ptR.y, on );
}

/************************************************************************/
static void TrackDrawHandle( HWND hWnd, HDC hDC, LPRECT lpRect, BOOL fCircle, BOOL on )
/************************************************************************/
{
int x1, x2, y;
RECT DispRect;

if (!hDC)
	hDC = Window.hDC;

DispRect = GetDisplayPtr(hWnd)->DispRect;
if (lpRect->right < DispRect.left ||
	lpRect->left > DispRect.right)
	return;
if (lpRect->bottom < DispRect.top ||
	lpRect->top > DispRect.bottom)
	return;
if (fCircle)
	{
	Dellipse(hDC, lpRect, on);
	return;
	}
for (y = lpRect->top; y <= lpRect->bottom; ++y)
	{
	if (y >= DispRect.top &&
		y <= DispRect.bottom)
		{
		x1 = bound(lpRect->left, DispRect.left,	DispRect.right);
		x2 = bound(lpRect->right+1, DispRect.left, DispRect.right);
		Dline(hDC, x1, y, x2, y, on);
		}
	}
}





