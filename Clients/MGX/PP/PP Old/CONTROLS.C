// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®PL1¯®FD1¯®BT0¯®TP0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

// Note to all GDI programmers:
// GetClientRect() returns a rectangle whose rightmost and bottom most pixels
// cannot be drawn into.  FrameRect() and FillRect() compensate for this
// problem automatically, but MoveTo() and Lineto() do not (and cannot)
// Use GetDrawClientRect if you want the actual rectangle to draw in
// and you plan to use MoveTo() LineTo instead of FrameRect() and FillRect()
// ....WHAT A PAIN!!!

extern HANDLE hInstAstral;

#define JOYSIZE		10L

/***********************************************************************/
long FAR PASCAL StickControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
PAINTSTRUCT ps;
long lValue;
BOOL Bool, bHasFocus, bChanged;
POINT Point;
FIXED Rate, Offset;
int x, y, xMax, yMax;
HRGN hRgn;
HDC hDC;
RECT ClientRect;
static HDC shDC;
static RECT ActiveRect;
static BOOL bTrack;
static POINT LastPoint;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	return(TRUE);
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_CUSTOM );

	// Draw the focus rectangle
	bHasFocus = ( GetFocus() == hWindow );
	FrameRect( hDC, &ClientRect,
		(bHasFocus ? Window.hBlackBrush : Window.hWhiteBrush) );
	// Draw the interior and exterior box frames
	InflateRect( &ClientRect, -1, -1 );
	FrameRect( hDC, &ClientRect, Window.hBlackBrush );
	InflateRect( &ClientRect, -JOYSIZE, -JOYSIZE );
	FrameRect( hDC, &ClientRect, Window.hBlackBrush );

	Rate = FGET( RectWidth( &ClientRect )-1, JOYSIZE );
	Offset = FGET( ClientRect.left, 1 );
	while ( (x = HIWORD( Offset )) <= ClientRect.right )
		{
		MoveTo( hDC, x, ClientRect.top );
		LineTo( hDC, x, ClientRect.top-JOYSIZE );
		MoveTo( hDC, x, ClientRect.bottom );
		LineTo( hDC, x, ClientRect.bottom+JOYSIZE );
		Offset += Rate;
		}
	Rate = FGET( RectHeight( &ClientRect )-1, JOYSIZE );
	Offset = FGET( ClientRect.top, 1 );
	while ( (y = HIWORD( Offset )) <= ClientRect.bottom )
		{
		MoveTo( hDC, ClientRect.left, y );
		LineTo( hDC, ClientRect.left-JOYSIZE, y );
		MoveTo( hDC, ClientRect.right, y );
		LineTo( hDC, ClientRect.right+JOYSIZE, y );
		Offset += Rate;
		}
	InflateRect( &ClientRect, -1, -1 );
	ActiveRect = ClientRect;

	// The window long holds the values from -100 and 100
	LastPoint.x = LastPoint.y = -1;
	lValue = GetWindowLong( hWindow, 0 );
	Point.x = bound( (int)LOWORD(lValue), -100, 100 );
	Point.y = bound( (int)HIWORD(lValue), -100, 100 );
	SelectClipRgn( hDC, hRgn = CreateRectRgnIndirect( &ActiveRect ) );
	DeleteObject( hRgn );
	DrawStick( hDC, &ActiveRect, &LastPoint, Point );
	EndPaint( hWindow, &ps );
	break;

    case WM_KEYDOWN:
	if ( wParam == VK_HOME )
		{
		lParam = MAKELONG(LastPoint.x,LastPoint.y-1);
		goto MouseMove;
		}
	if ( wParam == VK_END )
		{
		lParam = MAKELONG(LastPoint.x,LastPoint.y+1);
		goto MouseMove;
		}
	if ( wParam == VK_LEFT )
		{
		lParam = MAKELONG(LastPoint.x-1,LastPoint.y);
		goto MouseMove;
		}
	if ( wParam == VK_RIGHT )
		{
		lParam = MAKELONG(LastPoint.x+1,LastPoint.y);
		goto MouseMove;
		}
	if ( wParam != VK_SPACE )
		break;
	lParam = MAKELONG(LastPoint.x,LastPoint.y);

    case WM_LBUTTONDOWN:
	if ( bTrack )
		break;
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	shDC = GetDC( hWindow );
	SelectClipRgn( shDC, hRgn = CreateRectRgnIndirect( &ActiveRect ) );
	DeleteObject( hRgn );
	xMax = RectWidth( &ActiveRect ) - 1;
	yMax = RectHeight( &ActiveRect ) - 1;
	Point.x = bound( (int)LOWORD(lParam)-ActiveRect.left, 0, xMax );
	Point.y = bound( (int)HIWORD(lParam)-ActiveRect.top, 0, yMax );
	Point.x = -100 + FMUL( 200, FGET( Point.x, xMax ) );
	Point.y = -100 + FMUL( 200, FGET( Point.y, yMax ));
	Point.y = -Point.y;
	bChanged = DrawStick( shDC, &ActiveRect, &LastPoint, Point );
	if ( !bChanged )
		break;
	SetWindowLong( hWindow, 0, MAKELONG(Point.x,Point.y) );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), MAKELONG(Point.x,Point.y) );
	break;

    case WM_KEYUP:
	if ( wParam != VK_SPACE )
		break;
	lParam = MAKELONG(LastPoint.x,LastPoint.y);

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
	ReleaseDC( hWindow, shDC );
	break;

    case WM_LBUTTONDBLCLK:
	if ( bTrack )
		break;
	Point.x = Point.y = 0;
	hDC = GetDC( hWindow );
	SelectClipRgn( hDC, hRgn = CreateRectRgnIndirect( &ActiveRect ) );
	DeleteObject( hRgn );
	DrawStick( hDC, &ActiveRect, &LastPoint, Point );
	ReleaseDC( hWindow, hDC );
	SetWindowLong( hWindow, 0, MAKELONG(Point.x,Point.y) );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), MAKELONG(Point.x,Point.y) );
	break;

    MouseMove:
    case WM_MOUSEMOVE:
	if ( !bTrack )
		break;
	xMax = RectWidth( &ActiveRect ) - 1;
	yMax = RectHeight( &ActiveRect ) - 1;
	Point.x = bound( (int)LOWORD(lParam)-ActiveRect.left, 0, xMax );
	Point.y = bound( (int)HIWORD(lParam)-ActiveRect.top, 0, yMax );
	Point.x = -100 + FMUL( 200, FGET( Point.x, xMax ) );
	Point.y = -100 + FMUL( 200, FGET( Point.y, yMax ) );
	Point.y = -Point.y;
	bChanged = DrawStick( shDC, &ActiveRect, &LastPoint, Point );
	if ( !bChanged )
		break;
	SetWindowLong( hWindow, 0, MAKELONG(Point.x,Point.y) );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), MAKELONG(Point.x,Point.y) );
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	// Draw the focus rectangle
	hDC = GetDC( hWindow );
	GetClientRect( hWindow, &ClientRect );
	bHasFocus = ( message == WM_SETFOCUS );
	FrameRect( hDC, &ClientRect,
		(bHasFocus ? Window.hBlackBrush : Window.hWhiteBrush) );
	ReleaseDC( hWindow, hDC );
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
BOOL DrawStick( hDC, lpRect, lpLastPoint, Point )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
LPPOINT lpLastPoint;
POINT Point;
{
int x, y;
HBRUSH hOldBrush;
HPEN hOldPen;
POINT p;
#define R 7

// The Point holds the values from -100 and 100
Point.y = -Point.y;
x = lpRect->left + FMUL( 100+Point.x, FGET( RectWidth(lpRect)-1, 200 ) );
y = lpRect->top  + FMUL( 100+Point.y, FGET( RectHeight(lpRect)-1, 200 ) );

if ( x == lpLastPoint->x && y == lpLastPoint->y )
	return( NO );

p.x = lpLastPoint->x;
p.y = lpLastPoint->y;
if ( p.x >= 0 && p.y >= 0 )
	{ // Undraw the old one
	hOldPen = SelectObject( hDC, Window.hWhitePen );
	hOldBrush = SelectObject( hDC, Window.hWhiteBrush );
	MoveTo(hDC, p.x, lpRect->top);
	LineTo(hDC, p.x, lpRect->bottom);
	MoveTo(hDC, lpRect->left, p.y);
	LineTo(hDC, lpRect->right, p.y);
	Ellipse(hDC, p.x-R, p.y-R, p.x+R, p.y+R);
	SelectObject( hDC, hOldPen );
	SelectObject( hDC, hOldBrush );
	}

// Draw the new one
p.x = x;
p.y = y;
hOldPen = SelectObject( hDC, Window.hBlackPen );
hOldBrush = SelectObject( hDC, Window.hShadowBrush );
MoveTo( hDC, p.x, lpRect->top );
LineTo( hDC, p.x, lpRect->bottom );
MoveTo( hDC, lpRect->left, p.y );
LineTo( hDC, lpRect->right, p.y );
Ellipse( hDC, p.x-R, p.y-R, p.x+R, p.y+R );
SelectObject( hDC, hOldPen );
SelectObject( hDC, hOldBrush );

lpLastPoint->x = x;
lpLastPoint->y = y;
return( YES );
}


/***********************************************************************/
long FAR PASCAL LeverControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
PAINTSTRUCT ps;
int iValue, iOldValue;
BOOL Bool, bHasFocus;
int xMax, yMax;
HRGN hRgn;
HDC hDC;
RECT ClientRect;
static HDC shDC;
static RECT ActiveRect;
static BOOL bTrack;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	return(TRUE);
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_CUSTOM );

	// Draw the focus rectangle
	bHasFocus = ( GetFocus() == hWindow );
	FrameRect( hDC, &ClientRect,
		(bHasFocus ? Window.hBlackBrush : Window.hWhiteBrush) );
	// Draw the box frame
	InflateRect( &ClientRect, -1, -1 );
	FrameRect( hDC, &ClientRect, Window.hBlackBrush );
	InflateRect( &ClientRect, -1, -1 );
	ActiveRect = ClientRect;
	SelectClipRgn( hDC, hRgn = CreateRectRgnIndirect( &ActiveRect ) );
	DeleteObject( hRgn );

	// The int holds the 2 color percentages from -100 to 100
	iOldValue = 32767;
	iValue = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, YES );
	DrawLever( hDC, &ActiveRect, iOldValue, iValue,
		GetWindowLong( hWindow, 0 ) );
	EndPaint( hWindow, &ps );
	break;

    case WM_LBUTTONDOWN:
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	shDC = GetDC( hWindow );
	SelectClipRgn( shDC, hRgn = CreateRectRgnIndirect( &ActiveRect ) );
	DeleteObject( hRgn );
	xMax = RectWidth( &ActiveRect ) - 1;
	yMax = RectHeight( &ActiveRect ) - 1;
	if ( xMax > yMax )
		{
		iValue = bound( (int)LOWORD(lParam)-ActiveRect.left, 0, xMax );
		iValue = -100 + FMUL( 200, FGET( iValue, xMax ) );
		}
	else	{
		iValue = bound( (int)HIWORD(lParam)-ActiveRect.top, 0, yMax );
		iValue = -100 + FMUL( 200, FGET( iValue, yMax ));
		iValue = -iValue;
		}
	iOldValue = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, YES );
	if ( iValue == iOldValue )
		break;
	DrawLever( shDC, &ActiveRect, iOldValue, iValue,
		GetWindowLong( hWindow, 0 ) );
	SetDlgItemInt( GetParent(hWindow), GetWindowWord(hWindow, GWW_ID),
		iValue, YES );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), (long)iValue );
	break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
	ReleaseDC( hWindow, shDC );
	break;

    case WM_LBUTTONDBLCLK:
	if ( bTrack )
		break;
	hDC = GetDC( hWindow );
	SelectClipRgn( hDC, hRgn = CreateRectRgnIndirect( &ActiveRect ) );
	DeleteObject( hRgn );
	iValue = 0;
	iOldValue = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, YES );
	DrawLever( hDC, &ActiveRect, iOldValue, iValue,
		GetWindowLong( hWindow, 0 ) );
	ReleaseDC( hWindow, hDC );
	SetDlgItemInt( GetParent(hWindow), GetWindowWord(hWindow, GWW_ID),
		iValue, YES );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), (long)iValue );
	break;

    case WM_MOUSEMOVE:
	if ( !bTrack )
		break;
	xMax = RectWidth( &ActiveRect ) - 1;
	yMax = RectHeight( &ActiveRect ) - 1;
	if ( xMax > yMax )
		{
		iValue = bound( (int)LOWORD(lParam)-ActiveRect.left, 0, xMax );
		iValue = -100 + FMUL( 200, FGET( iValue, xMax ) );
		}
	else	{
		iValue = bound( (int)HIWORD(lParam)-ActiveRect.top, 0, yMax );
		iValue = -100 + FMUL( 200, FGET( iValue, yMax ));
		iValue = -iValue;
		}
	iOldValue = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, YES );
	if ( iValue == iOldValue )
		break;
	DrawLever( shDC, &ActiveRect, iOldValue, iValue,
		GetWindowLong( hWindow, 0 ) );
	SetDlgItemInt( GetParent(hWindow), GetWindowWord(hWindow, GWW_ID),
		iValue, YES );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), (long)iValue );
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	// Draw the focus rectangle
	hDC = GetDC( hWindow );
	GetClientRect( hWindow, &ClientRect );
	bHasFocus = ( message == WM_SETFOCUS );
	FrameRect( hDC, &ClientRect,
		(bHasFocus ? Window.hBlackBrush : Window.hWhiteBrush) );
	ReleaseDC( hWindow, hDC );
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
void DrawLever( hDC, lpRect, iOldValue, iValue, Color )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
int iOldValue, iValue;
COLOR Color;
{
int nx, ox, mx, my;
HBRUSH hBrush;
RECT rect, rPaint, rClip;
COLOR rgb1, rgb2;
BOOL bOKtoClip;

hBrush = NULL;
rPaint = rClip = *lpRect;

if ( Color == RGB(255,0,0) ) { rgb1 = RGB(255,0,255); rgb2 = RGB(255,255,0); }
else
if ( Color == RGB(0,255,0) ) { rgb1 = RGB(255,255,0); rgb2 = RGB(0,255,255); }
else
if ( Color == RGB(0,0,255) ) { rgb1 = RGB(0,255,255); rgb2 = RGB(255,0,255); }

mx = ( lpRect->right + lpRect->left ) / 2;
my = ( lpRect->bottom + lpRect->top ) / 2;

// The iValue also holds the values from -100 to 100
iValue = bound( iValue, -100, 100 );
nx = lpRect->left + FMUL( 100+iValue, FGET( RectWidth(lpRect)-1, 200 ) );

// The iOldValue holds the values from -100 to 100
bOKtoClip = ( iOldValue != 32767 );
iOldValue = bound( iOldValue, -100, 100 );
ox = lpRect->left + FMUL( 100+iOldValue, FGET( RectWidth(lpRect)-1, 200 ) );

if ( bOKtoClip )
	{
	if ( nx < ox )
		{ rClip.left = nx; rClip.right = ox; }
	else	{ rClip.left = ox; rClip.right = nx; }
	}

rPaint.left = lpRect->left;
rPaint.right = min(nx,mx);
if ( IntersectRect( &rect, &rClip, &rPaint ) )
	{
	FillRect( hDC, &rect, hBrush = CreateSolidBrush(rgb1) );
	DeleteObject( hBrush );
	}

rPaint.left = min(nx,mx);
rPaint.right = mx;
if ( IntersectRect( &rect, &rClip, &rPaint ) )
	FillRect( hDC, &rect, Window.hWhiteBrush );

rPaint.left = mx;
rPaint.right = max(nx,mx);
if ( IntersectRect( &rect, &rClip, &rPaint ) )
	FillRect( hDC, &rect, Window.hWhiteBrush );

rPaint.left = max(nx,mx);
rPaint.right = lpRect->right;
if ( IntersectRect( &rect, &rClip, &rPaint ) )
	{
	FillRect( hDC, &rect, hBrush = CreateSolidBrush(rgb2) );
	DeleteObject( hBrush );
	}
}


/***********************************************************************/
long FAR PASCAL MapControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
PAINTSTRUCT ps;
BOOL Bool, bHasFocus;
POINT Point;
FIXED xRate, yRate, xOffset, yOffset;
int i, x, y, dx, dy, xMax, yMax;
HRGN hRgn;
HDC hDC;
RECT ClientRect, Rect;
DWORD dwStyle;
static LPRECT lpActiveRect;
static LPPOINT lpLastPoints;
static int nPoints;
static HDC shDC;
static BOOL bTrack, bDrawHandles;
static WORD idPointToChange;
static POINT *lpPoints; //since this is a static pointer to a static structure,
// don't make it a far pointer because the data segment might move

switch ( message )
    {
    case WM_CREATE:
	lpLastPoints = (LPPOINT)Alloc( (long)MAXPOINTS*sizeof(POINT) +
					sizeof(RECT) );
	for ( i=0; i<MAXPOINTS; i++ )
		(lpLastPoints+i)->x = (lpLastPoints+i)->y = -1;
	SetWindowLong( hWindow, 4, (long)lpLastPoints );
	break;

    case WM_DESTROY:
	if ( !(lpLastPoints = (LPPOINT)GetWindowLong( hWindow, 4 )) )
		FreeUp( (LPTR)lpLastPoints );
	break;

    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	return(TRUE);
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_CUSTOM );

	// The window 1st long holds a pointer to the point list
	// The window 2nd long holds a pointer to the old point list
	// The window int holds the number of points
	lpPoints = (POINT*)GetWindowLong( hWindow, 0 );
	lpLastPoints = (LPPOINT)GetWindowLong( hWindow, 4 );
	lpActiveRect = (LPRECT)(lpLastPoints+MAXPOINTS);
	nPoints = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, NO );
	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	bDrawHandles = !(dwStyle & WS_DISABLED);

	// Draw the focus rectangle
	bHasFocus = ( GetFocus() == hWindow );
	FrameRect( hDC, &ClientRect,
		(bHasFocus ? Window.hBlackBrush : Window.hWhiteBrush) );
	InflateRect( &ClientRect, -1, -1 );

	xRate = FGET( RectWidth( &ClientRect )-1, 11 );
	dx = HIWORD( xRate );
	xOffset = FGET( ClientRect.left + dx, 1 );
	xOffset += xRate;

	yRate = FGET( RectHeight( &ClientRect )-1, 11 );
	dy = HIWORD( yRate );
	yOffset = FGET( ClientRect.bottom - dy, 1 );
	yOffset -= yRate;

	// Draw the interior and exterior box frames
	ClientRect.right++; ClientRect.bottom++;
	FillRect( hDC, &ClientRect, Window.hButtonBrush );
	Rect = ClientRect; Rect.left += dx; Rect.bottom -= dy;
	FillRect( hDC, &Rect, Window.hWhiteBrush );
	FrameRect( hDC, &ClientRect, Window.hBlackBrush );
	ClientRect.right--; ClientRect.bottom--;

	MoveTo( hDC, ClientRect.left + dx, ClientRect.top );
	LineTo( hDC, ClientRect.left + dx, ClientRect.bottom );
	while ( (x = HIWORD( xOffset )) < ClientRect.right )
		{
		MoveTo( hDC, x, ClientRect.bottom - dy );
		LineTo( hDC, x, ClientRect.bottom );
		xOffset += xRate;
		}
	MoveTo( hDC, ClientRect.left, ClientRect.bottom - dy );
	LineTo( hDC, ClientRect.right, ClientRect.bottom - dy );
	while ( (y = HIWORD( yOffset )) > ClientRect.top )
		{
		MoveTo( hDC, ClientRect.left, y );
		LineTo( hDC, ClientRect.left + dx, y );
		yOffset -= yRate;
		}

	ClientRect.left += dx;
	ClientRect.bottom -= dy;
	InflateRect( &ClientRect, -1, -1 );

	*lpActiveRect = ClientRect;
	SelectClipRgn( hDC, hRgn = CreateRectRgnIndirect( lpActiveRect ) );
	DeleteObject( hRgn );
	DrawMap( hDC, lpActiveRect, lpLastPoints, lpPoints,
		nPoints, NULL, Point, bDrawHandles );
	EndPaint( hWindow, &ps );
	break;

    case WM_LBUTTONDOWN:
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	// Set some statics needed during tracking
	shDC = GetDC( hWindow );
	// The window 1st long holds a pointer to the point list
	// The window 2nd long holds a pointer to the old point list
	// The window int holds the number of points
	lpPoints = (POINT*)GetWindowLong( hWindow, 0 );
	lpLastPoints = (LPPOINT)GetWindowLong( hWindow, 4 );
	lpActiveRect = (LPRECT)(lpLastPoints+MAXPOINTS);
	nPoints = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, NO );
	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	bDrawHandles = !(dwStyle & WS_DISABLED);

	SelectClipRgn( shDC, hRgn = CreateRectRgnIndirect( lpActiveRect ) );
	DeleteObject( hRgn );
	xMax = RectWidth( lpActiveRect ) - 1;
	yMax = RectHeight( lpActiveRect ) - 1;
	Point.x = bound( (int)LOWORD(lParam)-lpActiveRect->left, 0, xMax );
	Point.y = bound( (int)HIWORD(lParam)-lpActiveRect->top, 0, yMax );
	Point.x = FMUL( 255, FGET( Point.x, xMax ) );
	Point.y = FMUL( 255, FGET( Point.y, yMax ) );
	Point.y = 255 - Point.y;
	idPointToChange = ClosestPoint( lpPoints, nPoints, Point );
	break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
	ReleaseDC( hWindow, shDC );
	break;

    case WM_LBUTTONDBLCLK:
	if ( bTrack )
		break;
	break;

    case WM_MOUSEMOVE:
	if ( !bTrack )
		break;
	xMax = RectWidth( lpActiveRect ) - 1;
	yMax = RectHeight( lpActiveRect ) - 1;
	Point.x = bound( (int)LOWORD(lParam)-lpActiveRect->left, 0, xMax );
	Point.y = bound( (int)HIWORD(lParam)-lpActiveRect->top, 0, yMax );
	Point.x = FMUL( 255, FGET( Point.x, xMax ) );
	Point.y = FMUL( 255, FGET( Point.y, yMax ) );
	Point.y = 255 - Point.y;
	if ( !idPointToChange )
	    if ( !bDrawHandles
	      || !(idPointToChange = ClosestPoint( lpPoints, nPoints, Point )) )
		{
		SendMessage( GetParent(hWindow), WM_COMMAND,
		   GetWindowWord(hWindow, GWW_ID), MAKELONG(-1,Point.x) );
		break;
		}
	if ( DrawMap( shDC, lpActiveRect, lpLastPoints, lpPoints,
	    nPoints, idPointToChange, Point, bDrawHandles ) )
		SendMessage( GetParent(hWindow), WM_COMMAND,
		   GetWindowWord(hWindow, GWW_ID), (long)(idPointToChange-1) );
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	// Draw the focus rectangle
	hDC = GetDC( hWindow );
	GetClientRect( hWindow, &ClientRect );
	bHasFocus = ( message == WM_SETFOCUS );
	FrameRect( hDC, &ClientRect,
		(bHasFocus ? Window.hBlackBrush : Window.hWhiteBrush) );
	ReleaseDC( hWindow, hDC );
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
int ClosestPoint( lpPoints, nPoints, Point )
/***********************************************************************/
LPPOINT lpPoints;
int nPoints;
POINT Point;
{
int i;

for ( i=0; i<nPoints; i++ )
	{
	if ( abs( lpPoints->x - Point.x ) <= SMALL_MOVEMENT &&
	     abs( lpPoints->y - Point.y ) <= SMALL_MOVEMENT )
		return( i+1 );
	lpPoints++;
	}
return( 0 );
}


/***********************************************************************/
BOOL DrawMap( hDC, lpRect, lpLastPoints, lpPoints, nPoints, idPointToChange, Point, bDrawHandles )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
LPPOINT lpLastPoints, lpPoints;
int nPoints, idPointToChange;
POINT Point;
BOOL bDrawHandles;
{
int i, x, y, start, end, lo, hi;
HBRUSH hOldBrush;
HPEN hOldPen;
#define RAD SMALL_MOVEMENT

// The Point holds the values from 0 and 255
if ( idPointToChange )
	{
	start = idPointToChange - 1;
	end = idPointToChange;
	if ( end >= nPoints )
		end = nPoints - 1;
	if ( idPointToChange == 1 )
		lo = 0;
	else	lo = (lpPoints + idPointToChange - 2)->x;
	if ( idPointToChange == nPoints )
		hi = 255;
	else	hi = (lpPoints + idPointToChange)->x;
	(lpPoints + start)->x = bound( Point.x, lo, hi );
	(lpPoints + start)->y = Point.y;
	}
else	{
	start = 0;
	end = nPoints - 1;
	}

// Undraw the old lines
hOldPen = SelectObject( hDC, Window.hWhitePen );
hOldBrush = SelectObject( hDC, Window.hWhiteBrush );
if ( start ) // get the previous point
	Point = *(lpLastPoints+start-1);
else	{ // get the left edge to use as the previous point
	Point = *lpLastPoints;
	Point.x = lpRect->left;
	}
MoveTo( hDC, Point.x, Point.y );
for ( i=start; i<=end; i++ )
	{ // draw the lines
	Point = *(lpLastPoints+i);
	if ( Point.x < 0 && Point.y < 0 )
		continue;
	LineTo( hDC, Point.x, Point.y );
	}
if ( end == nPoints-1 ) // draw the last line
	LineTo( hDC, lpRect->right, Point.y );

if ( bDrawHandles )
    for ( i=start-1; i<=end; i++ )
	{ // Undraw the handles
	if ( i < 0 )
		continue;
	if ( idPointToChange && i != start )
		continue;
	Point = *(lpLastPoints+i);
	Ellipse( hDC, Point.x-RAD, Point.y-RAD, Point.x+RAD, Point.y+RAD );
	}

SelectObject( hDC, hOldPen );
SelectObject( hDC, hOldBrush );

// Draw the new lines
hOldPen = SelectObject( hDC, Window.hBlackPen );
hOldBrush = SelectObject( hDC, Window.hShadowBrush );
if ( start ) // get the previous point
	Point = *(lpPoints+start-1);
else	{ // get the left edge to use as the previous point
	Point = *lpPoints;
	Point.x = 0;
	}
Point.y = 255 - Point.y;
Point.x = lpRect->left + FMUL( Point.x, FGET( RectWidth(lpRect)-1, 255 ));
Point.y = lpRect->top  + FMUL( Point.y, FGET( RectHeight(lpRect)-1, 255 ));
MoveTo( hDC, Point.x, Point.y );
for ( i=start; i<=end; i++ )
	{ // draw the lines
	Point = *(lpPoints+i);
	Point.y = 255 - Point.y;
	Point.x = lpRect->left + FMUL( Point.x, FGET(RectWidth(lpRect)-1,255));
	Point.y = lpRect->top  + FMUL( Point.y, FGET(RectHeight(lpRect)-1,255));
	*(lpLastPoints+i) = Point;
	LineTo( hDC, Point.x, Point.y );
	}
if ( end == nPoints-1 ) // draw the last line
	LineTo( hDC, lpRect->right, Point.y );

if ( bDrawHandles )
    for ( i=start-1; i<=end; i++ )
	{ // Draw the handles
	if ( i < 0 )
		continue;
	Point = *(lpLastPoints+i);
	Ellipse( hDC, Point.x-RAD, Point.y-RAD, Point.x+RAD, Point.y+RAD );
	}

SelectObject( hDC, hOldPen );
SelectObject( hDC, hOldBrush );

return( YES );
}


/***********************************************************************/
long FAR PASCAL IconControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
int x, y, i;
RECT ClientRect;
PAINTSTRUCT ps;
HICON hResource;
HDC hDC;
BOOL bHasFocus, bSelected, bDown, bEnabled, bShadows, Bool;
BOOL bHasText;
WORD idIcon;
DWORD dwStyle;
STRING szString;
static BOOL bTrack, bInRect;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( wParam /*hDC*/, hWindow, &ClientRect,
		CTLCOLOR_BTN );
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );

	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	// The item id is the icon id, unless its an active tool
	idIcon = GetWindowWord( hWindow, GWW_ID );
	bSelected = GetDlgItemInt( GetParent(hWindow), idIcon, &Bool, NO );
	bHasFocus = ( GetFocus() == hWindow );
	bEnabled  = !( dwStyle & WS_DISABLED );
	bShadows  = !( dwStyle & WS_NOSHADOWS );
	bHasText  =  ( dwStyle & WS_TEXT );
	bSelected = ( bSelected == 1 && !bHasText );
	bDown = ( bSelected || (bTrack && bInRect && bHasFocus) );

	// Draw the box interior
	if ( !bShadows )
		{
		if ( bEnabled )
			FillRect( hDC, &ClientRect, Window.hWhiteBrush );
		}
	else
	if ( !bDown )
		FillRect( hDC, &ClientRect, Window.hButtonBrush );
	else	HilightRect( hDC, &ClientRect, Window.hHilightBrush );

	// Compute the client rect center
	if ( !bHasText )
		x = ( ClientRect.right + ClientRect.left + 1 ) / 2;
	else	x =   ClientRect.right - 16; // All the way to the right
	y = ( ClientRect.bottom + ClientRect.top + 1 ) / 2;
	if ( bDown )
		{ x += 2; y += 2; }

	if ( dwStyle & WS_BITMAP )
		{ // Draw the Bitmap
		if ( hResource =LoadBitmap(hInstAstral,MAKEINTRESOURCE(idIcon)))
			DrawBitmap( hDC, x, y, hResource );
		}
	else	{ // Draw the icon
		if (idIcon > LAST_ICON)
		    hResource =LoadIcon(NULL,MAKEINTRESOURCE(idIcon));
		else
		    hResource =LoadIcon(hInstAstral,MAKEINTRESOURCE(idIcon));
		if (hResource)
			DrawIcon( hDC, x-16, y-16, hResource );
		}

	if ( bHasText )
		{ // Draw the text left justified
		SelectObject( hDC, Window.hHelv10Font );
		GetWindowText( hWindow, szString, sizeof(szString) );
		if ( bDown ) i = 2; else i = 0;
		ColorText( hDC, 6+i, 2+i, szString, lstrlen(szString),
		     GetSysColor( (bEnabled || !hResource) ?
			COLOR_WINDOWTEXT : COLOR_BTNSHADOW ));
		}

	if ( !bShadows )
		{
		EndPaint( hWindow, &ps );
		break;
		}

	// Draw the box frame
	DrawSculptedBox( hDC, &ClientRect,
		bHasFocus && (dwStyle & WS_TABSTOP), bDown, YES );

	EndPaint( hWindow, &ps );
	break;

    case WM_KEYDOWN:
	if ( wParam != VK_SPACE )
		break;

    case WM_LBUTTONDOWN:
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	bInRect = YES;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	if ( WS_TOOL & GetWindowLong( hWindow, GWL_STYLE ) )
		{
		SendMessage( GetParent(hWindow), WM_COMMAND,
			GetWindowWord(hWindow, GWW_ID), 0L );
		ReleaseCapture(); bTrack = FALSE;
		}
	break;

    case WM_KEYUP:
	if ( wParam != VK_SPACE )
		break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	if ( bInRect )
	   SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), 0L );
	break;

    case WM_LBUTTONDBLCLK:
	if ( WS_TOOL & GetWindowLong( hWindow, GWL_STYLE ) )
		break;
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), 1L );
	break;

    case WM_MOUSEMOVE:
	if ( !bTrack )
		break;
	GetClientRect( hWindow, &ClientRect );
	if ( bInRect == !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
		{
		bInRect = !bInRect;
		InvalidateRect( hWindow, NULL, FALSE );
		UpdateWindow( hWindow );
		}
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case BM_SETSTATE:
    case BM_SETCHECK:
	if ( wParam == GetDlgItemInt( GetParent(hWindow),
	     GetWindowWord(hWindow, GWW_ID), &Bool, NO ) )
		break;
	SetDlgItemInt( GetParent(hWindow), GetWindowWord(hWindow, GWW_ID),
		wParam, NO );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
void DrawBitmap( hDC, x, y, hBitmap )
/***********************************************************************/
HDC hDC;
int x, y;
HBITMAP hBitmap;
{
int dx, dy;
BITMAP Data;
HDC hSrcDC;
short OldMode;
DWORD Color, OldBkColor, OldTextColor;
HBITMAP hOldBitmap;
#define RGB_BLACK  (RGB (0x00,0x00,0x00))
#define RGB_WHITE  (RGB (0xFF,0xFF,0xFF))
#define RGB_RED    (RGB (0xFF,0x00,0x00))

if ( !hDC )
	return;
if ( !(hSrcDC = CreateCompatibleDC(hDC)) )
	return;

Color = RGB_BLACK;
OldMode = SetBkMode( hDC, OPAQUE );
OldBkColor = SetBkColor( hDC, RGB_WHITE );
OldTextColor = SetTextColor( hDC, RGB_RED );
hOldBitmap = SelectObject( hSrcDC, hBitmap );

GetObject( hBitmap, sizeof(BITMAP), (LPSTR)&Data );
dx = Data.bmWidth;
dy = Data.bmHeight;
x -= dx/2;
y -= dy/2;
BitBlt( hDC, x, y, dx, dy, hSrcDC, 0, 0, SRCAND );
if ( Color != RGB_BLACK )
	{
	SetBkColor( hDC, RGB_BLACK );
	SetTextColor( hDC, Color );
	BitBlt( hDC, x, y, dx, dy, hSrcDC, 0, 0, SRCPAINT );
	}
SelectObject( hSrcDC, hOldBitmap );
SetTextColor( hDC, OldTextColor );
SetBkColor( hDC, OldBkColor );
SetBkMode( hDC, OldMode );
DeleteDC( hSrcDC );
}


/***********************************************************************/
long FAR PASCAL ScrollControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
int y;
RECT ClientRect;
PAINTSTRUCT ps;
HDC hDC;
BOOL bDown, Bool;
static BOOL bTrack, bInLeftRect, bInRightRect;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( wParam /*hDC*/, hWindow, &ClientRect,
		CTLCOLOR_SCROLLBAR );
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );

	y = ClientRect.bottom;
	ClientRect.bottom = 1 + ( ClientRect.top + ClientRect.bottom + 1 ) / 2;
	if ( ClientRect.bottom & 1 )
		ClientRect.bottom++; // make sure its even
	bDown = bInLeftRect && bTrack;
	DrawScroll( hDC, &ClientRect, 1, bDown );
	ClientRect.top = ClientRect.bottom;
	ClientRect.bottom = y;
	bDown = bInRightRect && bTrack;
	DrawScroll( hDC, &ClientRect, 0, bDown );
	EndPaint( hWindow, &ps );
	break;

    case WM_LBUTTONDOWN:
	SetFocus( GetDlgItem( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID) ) );
	SetCapture( hWindow ); bTrack = TRUE;
	GetClientRect( hWindow, &ClientRect );
	ClientRect.bottom = ( ClientRect.top + ClientRect.bottom + 1 ) / 2;
	bInLeftRect = PtInRect( &ClientRect, MAKEPOINT(lParam) );
	bInRightRect = !bInLeftRect;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	if ( bInLeftRect )
	    SendMessage( GetParent(hWindow), WM_VSCROLL, SB_LINEUP,
		(long)hWindow << 16 );
	if ( bInRightRect )
	    SendMessage( GetParent(hWindow), WM_VSCROLL, SB_LINEDOWN,
		(long)hWindow << 16 );
	SetTimer( hWindow, bTrack, 50, NULL );
	break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	KillTimer( hWindow, bTrack );
	ReleaseCapture(); bTrack = FALSE;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case WM_TIMER:
	if ( !bTrack )
		break;
	if ( bInLeftRect )
	    SendMessage( GetParent(hWindow), WM_VSCROLL, SB_LINEUP,
		(long)hWindow << 16 );
	if ( bInRightRect )
	    SendMessage( GetParent(hWindow), WM_VSCROLL, SB_LINEDOWN,
		(long)hWindow << 16 );
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_MOUSEMOVE:
	if ( !bTrack )
		break;
	GetClientRect( hWindow, &ClientRect );
	y = ClientRect.bottom;
	ClientRect.bottom = ( ClientRect.top + ClientRect.bottom ) / 2;
	if ( bInLeftRect == !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
		{
		bInLeftRect = !bInLeftRect;
		InvalidateRect( hWindow, NULL, FALSE );
		UpdateWindow( hWindow );
		}
	ClientRect.top = ClientRect.bottom;
	ClientRect.bottom = y;
	if ( bInRightRect == !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
		{
		bInRightRect = !bInRightRect;
		InvalidateRect( hWindow, NULL, FALSE );
		UpdateWindow( hWindow );
		}
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
void DrawScroll( hDC, lpRect, bPlus, bDown )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL bPlus, bDown;
{
int x1, y1, x2, y2, cx, cy, delta;
HPEN hOldPen;
RECT Rect;

// Draw the box interior
if ( bDown )
	FillRect( hDC, lpRect, Window.hButtonBrush );
else	FillRect( hDC, lpRect, Window.hWhiteBrush );

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

x1 = Rect.left;
x2 = Rect.right;
y1 = Rect.top;
y2 = Rect.bottom;

// Draw a 3 sided dark box
hOldPen = SelectObject( hDC, Window.hBlackPen );
if ( bPlus )
	{
	MoveTo( hDC, x1, y2 );
	LineTo( hDC, x1, y1 );
	LineTo( hDC, x2, y1 );
	LineTo( hDC, x2, y2 ); LineTo( hDC, x2, y2+1 );
	x1++; x2--; y1++;
	}
else	{
	MoveTo( hDC, x1, y1 );
	LineTo( hDC, x1, y2 );
	LineTo( hDC, x2, y2 );
	LineTo( hDC, x2, y1 ); LineTo( hDC, x2, y1-1 );
	x1++; x2--; y2--;
	}

x1++; x2--; y1++; y2--;

if ( bDown )
	{
	x1++; x2++; y1++; y2++;
	}

cx = ( x1 + x2 ) / 2;
cy = ( y1 + y2 ) / 2;

SelectObject( hDC, Window.hShadowPen );
// draw the horizontal rectangle
MoveTo( hDC, x1, cy-1 );
LineTo( hDC, x2, cy-1 );
LineTo( hDC, x2, cy+1 );
LineTo( hDC, x1, cy+1 );
LineTo( hDC, x1, cy-1 );

if ( bPlus )
	{
	// draw the vertical rectangle
	MoveTo( hDC, cx-1, y1 );
	LineTo( hDC, cx-1, y2 );
	LineTo( hDC, cx+1, y2 );
	LineTo( hDC, cx+1, y1 );
	LineTo( hDC, cx-1, y1 );
	SelectObject( hDC, Window.hBlackPen );
	// draw the black hilights
	MoveTo( hDC, cx+1, y1 );
	LineTo( hDC, cx+1, cy-1 ); LineTo( hDC, cx+1, cy );
	MoveTo( hDC, x2, cy );
	LineTo( hDC, x2, cy+1 );
	LineTo( hDC, cx+1, cy+1 );
	LineTo( hDC, cx+1, y2 );
	LineTo( hDC, cx-1, y2 ); LineTo( hDC, cx-1, y2+1 ); 
	MoveTo( hDC, cx-1, cy+1 );
	LineTo( hDC, x1, cy+1 ); LineTo( hDC, x1, cy );
	// draw the inside cross
	SelectObject( hDC, Window.hWhitePen );
	MoveTo( hDC, x1+1, cy );
	LineTo( hDC, x2-1, cy );
	MoveTo( hDC, cx, y1+1 );
	LineTo( hDC, cx, y2-1 );
	}
else	{
	SelectObject( hDC, Window.hBlackPen );
	// draw the black hilights
	MoveTo( hDC, x2, cy );
	LineTo( hDC, x2, cy+1 );
	LineTo( hDC, x1, cy+1 ); LineTo( hDC, x1, cy );
	// draw the inside dash
	SelectObject( hDC, Window.hWhitePen );
	MoveTo( hDC, x1+1, cy );
	LineTo( hDC, x2-1, cy );
	}

SelectObject( hDC, hOldPen );
}


#ifdef UNUSED
/***********************************************************************/
void DrawScroll( hDC, lpRect, bPlus, bDown )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL bPlus, bDown;
{
int x1, y1, x2, y2, cx, cy, delta;
HPEN hOldPen;
RECT Rect;

// Draw the box interior
FillRect( hDC, lpRect, Window.hButtonBrush );
Rect = *lpRect;
Rect.right--;
Rect.bottom--;

x1 = Rect.left;
x2 = Rect.right;
y1 = Rect.top;
y2 = Rect.bottom;

// Draw a 3 sided border
hOldPen = SelectObject( hDC, Window.hBlackPen );
MoveTo( hDC, x1, y1 );
LineTo( hDC, x2, y1 );
LineTo( hDC, x2, y2 );
LineTo( hDC, x1, y2 );
LineTo( hDC, x1, y1 );
x1++; y1++; y2--; x2--;

// Draw the sculpturing
SelectObject( hDC, ( bDown ? Window.hShadowPen : Window.hWhitePen ) );
MoveTo( hDC, x1, y2 );
LineTo( hDC, x1, y1 );
LineTo( hDC, x2, y1 );
if ( bDown )
	LineTo( hDC, x2, y1+1 );
else	{ // Draw the drop shadow
	SelectObject( hDC, Window.hShadowPen );
	LineTo( hDC, x2, y2 );
	LineTo( hDC, x1, y2 );
	}
x1++; x2--; y1++; y2--;

// Draw the horizontal dashes
SelectObject( hDC, Window.hBlackPen );
cx = ( x1 + x2 ) / 2;
cy = ( y1 + y2 ) / 2;
delta = ( bDown ? 1 : 0 );
MoveTo( hDC, x1+delta, cy+delta );
LineTo( hDC, x2+delta+1, cy+delta );
MoveTo( hDC, x1+delta, cy+delta+1 );
LineTo( hDC, x2+delta+1, cy+delta+1 );
if ( bPlus )
	{ // Draw the vertical dashes
	MoveTo( hDC, cx+delta,   y1+delta );
	LineTo( hDC, cx+delta,   y2+delta+1 );
	MoveTo( hDC, cx+delta+1, y1+delta );
	LineTo( hDC, cx+delta+1, y2+delta+1 );
	}

SelectObject( hDC, hOldPen );
}
#endif

/***********************************************************************/
long FAR PASCAL ColorControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
RECT ClientRect;
PAINTSTRUCT ps;
HDC hDC;
HPEN hOldPen, hPen;
BOOL bHasFocus, bSelected, bDown, bEnabled, Bool;
RGBS rgb;
HBRUSH hBrush;
BYTE c, tmp[MAX_STR_LEN];
COLOR Color;
DWORD dwStyle;
static BOOL bTrack, bInRect;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( wParam /*hDC*/, hWindow, &ClientRect,
		CTLCOLOR_BTN );
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );

	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	bSelected = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, NO );
	bHasFocus = ( GetFocus() == hWindow );
	bEnabled = !( dwStyle & WS_DISABLED );
	bDown = bTrack && bInRect && bHasFocus;

	// Compute the color to paint
	GetWindowText( hWindow, tmp, sizeof(tmp) );
	if ( c = tmp[0] )
		{
		AsciiRGB( &tmp[1], &rgb );
		// The first character indicates 'h' for HSL or 'r' for RGB
		if ( c == 'H' || c == 'h' )
			HSLtoRGB( rgb.red, rgb.green, rgb.blue, &rgb );
		// If lower case, use a Windows brush to draw the patch,
		// otherwise use SuperBlt
		if ( c >= 'a' && c <= 'z' )
			{
			FillRect( hDC, &ClientRect,
				hBrush = CreateSolidBrush( RGB2long(rgb) ) );
			DeleteObject( hBrush );
			}
		Color = RGB2long( rgb );
		}
	else	Color = GetWindowLong( hWindow, 0 );

	// Draw the color patch
	InflateRect( &ClientRect, -1, -1 );
	if ( !bDown )
		DrawColorPatch( hDC, &ClientRect, Color );
	else	HilightRect( hDC, &ClientRect, Window.hHilightBrush );
	InflateRect( &ClientRect, 1, 1 );

	// Draw the box frame
	if ( bEnabled )
		DrawSculptedBox( hDC, &ClientRect,
			bHasFocus && (dwStyle & WS_TABSTOP), bDown, NO );
	else	FrameRect( hDC, &ClientRect, Window.hBlackBrush );

	EndPaint( hWindow, &ps );
	break;

    case WM_LBUTTONDOWN:
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	bInRect = YES;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case WM_KEYUP:
	if ( wParam != VK_SPACE )
		break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	if ( bInRect )
	    SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), 0L );
	break;

    case WM_LBUTTONDBLCLK:
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), 1L );
	break;

    case WM_MOUSEMOVE:
	if ( !bTrack )
		{
		if ( GetWindowLong( hWindow, GWL_STYLE ) & WS_DISABLED )
			break;
		Color = GetWindowLong( hWindow, 0 );
		CopyRGB( &Color, &rgb );
		ColorStatus( &rgb, 0L );
		break;
		}
	GetClientRect( hWindow, &ClientRect );
	if ( bInRect == !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
		{
		bInRect = !bInRect;
		InvalidateRect( hWindow, NULL, FALSE );
		UpdateWindow( hWindow );
		}
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case BM_SETSTATE:
    case BM_SETCHECK:
	if ( wParam == GetDlgItemInt( GetParent(hWindow),
	     GetWindowWord(hWindow, GWW_ID), &Bool, NO ) )
		break;
	SetDlgItemInt( GetParent(hWindow), GetWindowWord(hWindow, GWW_ID),
		wParam, NO );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
void DrawColorPatch( hDC, lpRect, Color )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
COLOR Color;
{
int y;
LPTR lpBuffer;

lpBuffer = Alloc((long)RectWidth(lpRect)*3L);
if (!lpBuffer)
    return;

lpRect->right--;
lpRect->bottom--;

StartSuperBlt( hDC, &BltScreen, lpRect, 3 /*depth*/, 0, 0, 0 );
set24( (LPRGB)lpBuffer, RectWidth( lpRect ), Color );
y = RectHeight( lpRect );
while ( --y >= 0 )
    SuperBlt(lpBuffer );
SuperBlt( NULL );
FreeUp(lpBuffer);

lpRect->right++;
lpRect->bottom++;
}


/***********************************************************************/
long FAR PASCAL Color2Control( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
RECT ClientRect, SavedRect;
PAINTSTRUCT ps;
HDC hDC;
BOOL bHasFocus, bSelected, bDown, bEnabled, Bool;
BOOL bDrawingActive;
RGBS rgb;
COLOR Color;
DWORD dwStyle;
static BOOL bTrack, bInRect;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( wParam /*hDC*/, hWindow, &ClientRect,
		CTLCOLOR_BTN );
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );

	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	bSelected = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, NO );
	bHasFocus = ( GetFocus() == hWindow );
	bEnabled = !( dwStyle & WS_DISABLED );
	bDown = bTrack && bInRect && bHasFocus;

	// Draw the box frame
	FrameRect( hDC, &ClientRect, Window.hBlackBrush );
	InflateRect( &ClientRect, -1, -1 );

	// Draw the box interior
	FillRect( hDC, &ClientRect, Window.hButtonBrush );
	DrawStaticBox( hDC, &ClientRect, NO );
	InflateRect( &ClientRect, -1, -1 );

//	Take out the focus rect for now
//	if ( bHasFocus ) // Draw the focus rectangle
//		FrameRect( hDC, &ClientRect, Window.hBlackBrush );
//	else	FrameRect( hDC, &ClientRect, Window.hWhiteBrush );
	InflateRect( &ClientRect, -3, -3 );

	// Adjust the client rect for drawing the alternate color patch
	bDrawingActive = NO;
	SavedRect = ClientRect;
	if ( bSelected ) // The alternate color is higher up on the screen
		{
		ClientRect.left += RectWidth(&ClientRect)/3;
		ClientRect.bottom -= RectHeight(&ClientRect)/3;
		}
	else	{  // The alternate color is lower on the screen
		ClientRect.right -= RectWidth(&ClientRect)/3;
		ClientRect.top += RectHeight(&ClientRect)/3;
		}

	DrawColor:

	// Draw the box frame
	FrameRect( hDC, &ClientRect, Window.hBlackBrush );
	InflateRect( &ClientRect, -1, -1 );

	// Draw the color patch
	DrawColorPatch( hDC, &ClientRect,
		GetWindowLong( hWindow, bDrawingActive? 0:4 ) );

	// If we just drew the active color, we're done
	if ( bDrawingActive )
		{
		EndPaint( hWindow, &ps );
		break;
		}

	// Adjust the client rect for drawing the active color patch
	bDrawingActive = YES;
	ClientRect = SavedRect;
	if ( !bSelected ) // The active color is higher up on the screen
		{
		ClientRect.left += RectWidth(&ClientRect)/3;
		ClientRect.bottom -= RectHeight(&ClientRect)/3;
		}
	else	{  // The active color is lower on the screen
		ClientRect.right -= RectWidth(&ClientRect)/3;
		ClientRect.top += RectHeight(&ClientRect)/3;
		}
	// Go back and paint the active color
	goto DrawColor;

    case WM_LBUTTONDOWN:
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	bInRect = YES;

	// Compute the rectangle of the active color
	GetClientRect( hWindow, &ClientRect );
	if ( !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
		break;
	bSelected = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, NO );
	InflateRect( &ClientRect, -3, -3 );
	if ( !bSelected ) // The active color is higher up on the screen
		{
		ClientRect.left += RectWidth(&ClientRect)/3;
		ClientRect.bottom -= RectHeight(&ClientRect)/3;
		}
	else	{  // The active color is lower on the screen
		ClientRect.right -= RectWidth(&ClientRect)/3;
		ClientRect.top += RectHeight(&ClientRect)/3;
		}

	// If we are already in the active color, then get out right away
	if ( PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
		break;

	// Otherwise, switch the active and alternate color values and redraw
	SetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), !bSelected, NO );
	Color = GetWindowLong( hWindow, 0 );
	SetWindowLong( hWindow, 0, GetWindowLong( hWindow, 4 ) );
	SetWindowLong( hWindow, 4, Color );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
//	InvalidateRect( hWindow, NULL, FALSE );
//	UpdateWindow( hWindow );
	if ( bInRect )
	    SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), 0L );
	break;

    case WM_LBUTTONDBLCLK:
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), 1L );
	break;

    case WM_MOUSEMOVE:
	// Compute the rectangle of the active color
	GetClientRect( hWindow, &ClientRect );
	if ( !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
		break;
	bSelected = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, NO );
	InflateRect( &ClientRect, -3, -3 );
	if ( !bSelected ) // The active color is higher up on the screen
		{
		ClientRect.left += RectWidth(&ClientRect)/3;
		ClientRect.bottom -= RectHeight(&ClientRect)/3;
		}
	else	{  // The active color is lower on the screen
		ClientRect.right -= RectWidth(&ClientRect)/3;
		ClientRect.top += RectHeight(&ClientRect)/3;
		}

	if ( !bTrack )
		{
		if ( Bool = PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
			Color = GetWindowLong( hWindow, 0 );
		else	Color = GetWindowLong( hWindow, 4 );
		CopyRGB(&Color, &rgb);
		MessageStatus( GetWindowWord(hWindow, GWW_ID),
			(LPTR)( Bool ? "active " : "alternate " ),
			(int)rgb.red, (int)rgb.green, (int)rgb.blue );
		break;
		}

	// If we are in the active color, then get out without any messages
	if ( PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
		break;

	// Otherwise, switch the active and alternate color values and notify
	SetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), !bSelected, NO );
	Color = GetWindowLong( hWindow, 0 );
	SetWindowLong( hWindow, 0, GetWindowLong( hWindow, 4 ) );
	SetWindowLong( hWindow, 4, Color );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	// Draw the focus rectangle
	hDC = GetDC( hWindow );
	GetClientRect( hWindow, &ClientRect );
	InflateRect( &ClientRect, -1, -1 );
	bHasFocus = ( message == WM_SETFOCUS );
	FrameRect( hDC, &ClientRect,
		(bHasFocus ? Window.hBlackBrush : Window.hWhiteBrush) );
	ReleaseDC( hWindow, hDC );
	break;

    case BM_SETSTATE:
    case BM_SETCHECK:
	if ( wParam == GetDlgItemInt( GetParent(hWindow),
	     GetWindowWord(hWindow, GWW_ID), &Bool, NO ) )
		break;
	SetDlgItemInt( GetParent(hWindow), GetWindowWord(hWindow, GWW_ID),
		wParam, NO );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
long FAR PASCAL HueControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
long lValue;
int y, dy, hue;
PAINTSTRUCT ps;
HDC hDC;
BOOL Bool, bHasFocus;
static RECT ClientRect;
static int hueDefault;
static BOOL bTrack;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( wParam /*hDC*/, hWindow, &ClientRect,
		CTLCOLOR_CUSTOM );
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );

	// The window long holds the hue and the hue default
	lValue = GetWindowLong( hWindow, 0 );
	hue = bound( (int)LOWORD(lValue), 0, MAX_HUES-1 );
	hueDefault = bound( (int)HIWORD(lValue), 0, MAX_HUES-1 );
	bHasFocus = ( GetFocus() == hWindow );
	DrawHueBar( hDC, &ClientRect, hue, hueDefault, bHasFocus );
	EndPaint( hWindow, &ps );
	break;

    case WM_LBUTTONDOWN:
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	GetClientRect( hWindow, &ClientRect );
	// The window long holds the hue and the hue default
	lValue = GetWindowLong( hWindow, 0 );
	hueDefault = bound( (int)HIWORD(lValue), 0, MAX_HUES-1 );
	y = bound( HIWORD(lParam), ClientRect.top, ClientRect.bottom );
	y -= ClientRect.top;
	dy = ClientRect.bottom - ClientRect.top;
	hue = hueDefault + FMUL( MAX_HUES-1, FGET( y, dy ) );
	while ( hue < 0 )
		hue += MAX_HUES;
	while ( hue >= MAX_HUES )
		hue -= MAX_HUES;
	hDC = GetDC( hWindow );
	bHasFocus = ( GetFocus() == hWindow );
	DrawHueBar( hDC, &ClientRect, hue, hueDefault, bHasFocus );
	ReleaseDC( hWindow, hDC );
	SetWindowLong( hWindow, 0, MAKELONG(hue,hueDefault) );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), MAKELONG(hue,hueDefault) );
	break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
	break;

    case WM_MOUSEMOVE:
	if ( !bTrack )
		break;
	// The window long holds the hue and the hue default
	lValue = GetWindowLong( hWindow, 0 );
	hueDefault = bound( (int)HIWORD(lValue), 0, MAX_HUES-1 );
	y = bound( HIWORD(lParam), ClientRect.top, ClientRect.bottom );
	y -= ClientRect.top;
	dy = ClientRect.bottom - ClientRect.top;
	hue = hueDefault + FMUL( MAX_HUES-1, FGET( y, dy ) );
	while ( hue < 0 )
		hue += MAX_HUES;
	while ( hue >= MAX_HUES )
		hue -= MAX_HUES;
	hDC = GetDC( hWindow );
	bHasFocus = ( GetFocus() == hWindow );
	DrawHueBar( hDC, &ClientRect, hue, hueDefault, bHasFocus );
	ReleaseDC( hWindow, hDC );
	SetWindowLong( hWindow, 0, MAKELONG(hue,hueDefault) );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), MAKELONG(hue,hueDefault) );
	break;

    case WM_LBUTTONDBLCLK:
	if ( bTrack )
		break;
	GetClientRect( hWindow, &ClientRect );
	// The window long holds the hue and the hue default
	lValue = GetWindowLong( hWindow, 0 );
	hueDefault = bound( (int)HIWORD(lValue), 0, MAX_HUES-1 );
	hue = hueDefault;
	hDC = GetDC( hWindow );
	bHasFocus = ( GetFocus() == hWindow );
	DrawHueBar( hDC, &ClientRect, hue, hueDefault, bHasFocus );
	ReleaseDC( hWindow, hDC );
	SetWindowLong( hWindow, 0, MAKELONG(hue,hueDefault) );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), MAKELONG(hue,hueDefault) );
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
void DrawHueBar( hDC, lpRect, hue, hueDefault, bHasFocus )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
int hue, hueDefault;
BOOL bHasFocus;
{
HBRUSH hBrush;
int y, dy, ySave;
RGBS rgb;

HSLtoRGB( hue, 255, 127, &rgb );

// Compute the y value from the hue; the default is the rectangle's midpoint
hue -= hueDefault;
while ( hue < 0 )
	hue += MAX_HUES;
while ( hue >= MAX_HUES )
	hue -= MAX_HUES;
dy = lpRect->bottom - lpRect->top;
y = FMUL( dy, FGET( hue, MAX_HUES-1 ) );
y = bound( lpRect->top + y, lpRect->top, lpRect->bottom );

// Draw the white portion of the bar
ySave = lpRect->bottom;
lpRect->bottom = y;
FillRect( hDC, lpRect, Window.hWhiteBrush );
lpRect->bottom = ySave;

// Draw the color portion of the bar
ySave = lpRect->top;
lpRect->top = y;
// Can't use SuperBlt to draw the patches because the lut changes
// DrawColorPatch( hDC, lpRect, RGB2long( rgb ) );
FillRect( hDC, lpRect, hBrush = CreateSolidBrush( RGB2long(rgb) ) );
DeleteObject( hBrush );

// Draw the focus rectangle
if ( bHasFocus )
	{
	lpRect->right--;
//	lpRect->bottom--;
	FrameRect( hDC, lpRect, Window.hBlackBrush );
	lpRect->right++;
//	lpRect->bottom++;
	}
lpRect->top = ySave;
}


/***********************************************************************/
long FAR PASCAL HistoControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
PAINTSTRUCT ps;
HDC hDC;
BOOL Bool;
int iValue;
RECT ClientRect;
char tmp[MAX_STR_LEN];
RGBS rgb;
LPWORD lpHisto;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	return(TRUE);
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_CUSTOM );

	GetWindowText( hWindow, tmp, sizeof(tmp) );
	AsciiRGB( tmp, &rgb );

	iValue = (int)GetControlValue(GetParent(hWindow), GetWindowWord(hWindow, GWW_ID), 0);
	lpHisto = (LPWORD)GetControlValue(GetParent(hWindow), GetWindowWord(hWindow, GWW_ID), 1);
	DrawHisto(hDC, &ClientRect, iValue, lpHisto, &rgb, -1);
	EndPaint( hWindow, &ps );
	break;

    case WM_LBUTTONDOWN:
	break;

    case WM_LBUTTONUP:
	break;

    case WM_MOUSEMOVE:
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	break;

    case WM_SETCONTROL:
	if (wParam == 0)  /* the value changed */
		{
		GetWindowText( hWindow, tmp, sizeof(tmp) );
		AsciiRGB( tmp, &rgb );
		hDC = GetDC(hWindow);
		iValue = (int)GetControlValue(GetParent(hWindow), GetWindowWord(hWindow, GWW_ID), 0);
		lpHisto = (LPWORD)GetControlValue(GetParent(hWindow), GetWindowWord(hWindow, GWW_ID), 1);
		GetClientRect( hWindow, &ClientRect );
		DrawHisto(hDC, &ClientRect, (int)lParam, lpHisto, &rgb, iValue);
		GetClientRect( hWindow, &ClientRect );
		DrawHisto(hDC, &ClientRect, (int)lParam, lpHisto, &rgb, (int)lParam);
		ReleaseDC(hWindow, hDC);
		}
	else if (wParam == 1) /* the lpHisto pointer changed */
		{
		InvalidateRect(hWindow, NULL, NO);
		UpdateWindow( hWindow );
		}
	break;
    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
void DrawHisto( hDC, lpRect, iValue, lpHisto, lpRGB, iDrawValue )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
int iValue;
LPWORD lpHisto;
LPRGB lpRGB;
int iDrawValue;
{
FIXED xrate, yscl, xoffset;
int index, height, x;
HPEN hOldPen, hPen, hInvertPen, hSelectPen;
WORD maxValue;
BOOL fDidDrawValue, fDidSelectedValue;

if (iDrawValue < 0)
	{
	FillRect( hDC, lpRect, Window.hWhiteBrush );
	FrameRect( hDC, lpRect, Window.hBlackBrush);
	}
if (!lpHisto)
	return;

maxValue = *lpHisto++;
InflateRect(lpRect, -1, -1);
--lpRect->right; --lpRect->bottom; /* Windows doesn't include these pixels */
hPen = CreatePen( PS_SOLID, 1, RGB(lpRGB->red, lpRGB->green, lpRGB->blue));
hInvertPen = CreatePen( PS_SOLID, 1, RGB(255-lpRGB->red, 255-lpRGB->green, 255-lpRGB->blue));
hSelectPen = CreatePen( PS_DOT, 1, RGB(lpRGB->red, lpRGB->green, lpRGB->blue));

hOldPen = SelectObject(hDC, hPen);
xrate = FGET(255, RectWidth(lpRect)-1);
yscl = FGET(RectHeight(lpRect), maxValue-1);
xoffset = xrate>>1;
fDidDrawValue = fDidSelectedValue = NO;
for (x = lpRect->left; x <= lpRect->right; ++x)
	{
	index = HIWORD(xoffset);
	xoffset += xrate;
	index = bound(index, 0, 255);
	if (iDrawValue >= 0)
		{
		if ((iDrawValue == index) ||
		    (!fDidDrawValue && (index > iDrawValue)))
			{
			SelectObject(hDC, Window.hWhitePen);
			MoveTo(hDC, x, lpRect->bottom);
			LineTo(hDC, x, lpRect->top);
			SelectObject(hDC, hPen);
			fDidDrawValue = YES;
			}
		else if ((index == iValue) ||
	                 (!fDidSelectedValue && (index > iValue)))
			{
			fDidSelectedValue = YES;
			continue;
			}
		}
	height = FMUL(lpHisto[255-index], yscl);
	if ((index == iValue) ||
	    (!fDidSelectedValue && (index > iValue)))
		{
		SelectObject(hDC, hInvertPen);
		}
	MoveTo(hDC, x, lpRect->bottom);
	LineTo(hDC, x, lpRect->bottom-height);
	if ((index == iValue) ||
	    (!fDidSelectedValue && (index > iValue)))
		{
		SelectObject(hDC, hSelectPen);
		LineTo(hDC, x, lpRect->top);
		SelectObject(hDC, hPen);
		fDidSelectedValue = YES;
		}
	}
SelectObject(hDC, hOldPen);
DeleteObject(hPen);
DeleteObject(hInvertPen);
DeleteObject(hSelectPen);
}


/***********************************************************************/
long FAR PASCAL QuartersControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
PAINTSTRUCT ps;
HDC hDC;
static BOOL bTrack = FALSE;
int iValue;
FIXED xscl;
int i, x;
static RECT ClientRect, HistoRect, MarkRect;
HICON hResource;
MAP *lpMap;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( wParam /*hDC*/, hWindow, &ClientRect,
		CTLCOLOR_CUSTOM );
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	GetWindowRect( GetDlgItem( GetParent(hWindow), IDC_BHISTO ), &HistoRect);
	GetWindowRect( hWindow, &ClientRect);
	ClientRect.left = HistoRect.left;
	ClientRect.right = HistoRect.right;
	ScreenToClient(hWindow, (LPPOINT)&ClientRect.left );
	ScreenToClient(hWindow, (LPPOINT)&ClientRect.right );
	InflateRect(&ClientRect, -1, -1);
	--ClientRect.right; --ClientRect.bottom;

	xscl = FGET(RectWidth(&ClientRect)-1, 255);
	
	lpMap = (MAP *)GetControlValue(GetParent(hWindow), GetWindowWord(hWindow, GWW_ID), 0);
	if (lpMap)
		{
		for (i = 0; i < QTONEPOINTS; ++i)
			{
			if (i != Qtone.ActiveMark)
				{
				iValue = lpMap->Pnt[i].x;
				x = FMUL(iValue, xscl) - 15;
				if ( hResource = LoadIcon( hInstAstral, MAKEINTRESOURCE(IDC_QUART1+i) ) )
					DrawIcon( hDC, ClientRect.left+x, ClientRect.top, hResource);
				}
			}
		iValue = lpMap->Pnt[Qtone.ActiveMark].x;
		x = FMUL(iValue, xscl) - 15;
		if ( hResource = LoadIcon( hInstAstral, MAKEINTRESOURCE(IDC_QUART1+Qtone.ActiveMark) ) )
			DrawIcon( hDC, ClientRect.left+x, ClientRect.top, hResource);
		}
	EndPaint( hWindow, &ps );
	break;

    case WM_LBUTTONDOWN:
	lpMap = (MAP *)GetControlValue(GetParent(hWindow), GetWindowWord(hWindow, GWW_ID), 0);
	xscl = FGET(RectWidth(&ClientRect)-1, 255);
	for (i = 0; i < QTONEPOINTS; ++i)
    		{
		iValue = lpMap->Pnt[i].x;
		MarkRect.left = FMUL(iValue, xscl) - 15 + ClientRect.left;
		MarkRect.right = MarkRect.left + 32;
		MarkRect.top = ClientRect.top;
		MarkRect.bottom = MarkRect.top + 32;
		if (PtInRect(&MarkRect, MAKEPOINT(lParam)))
			{
			SetCapture( hWindow ); bTrack = TRUE;
			SendMessage(GetParent(hWindow), WM_COMMAND, GetWindowWord(hWindow, GWW_ID), MAKELONG(iValue, i));
			break;
			}
		}
	break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
	break;

    case WM_MOUSEMOVE:
	if (bTrack)
    		{
		xscl = FGET(255, RectWidth(&ClientRect)-1);
		x = bound(LOWORD(lParam), ClientRect.left, ClientRect.right);
		x -= ClientRect.left;
		iValue = FMUL(x, xscl);
		SendMessage(GetParent(hWindow), WM_COMMAND, GetWindowWord(hWindow, GWW_ID), MAKELONG(iValue, Qtone.ActiveMark));
		}
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	break;

    case WM_SETCONTROL:
	InvalidateRect(hWindow, NULL, YES);
	UpdateWindow(hWindow);
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/************************************************************************/
VOID ModifyQuartertones(lpMap, iQTone, iValue)
/************************************************************************/
MAP *lpMap;
int iQTone;
int iValue;
{
int i, j, num;
BOOL fSlideRight;

Qtone.QTmoved[iQTone] = YES;
fSlideRight = iValue >= lpMap->Pnt[iQTone].x;
lpMap->Pnt[iQTone].x = iValue;
    
/* default values that are evenly spaced */
if ( !Qtone.QTmoved[2] )
	{
	if ( !Qtone.QTmoved[i = 1] )
		i = 0;
	if ( !Qtone.QTmoved[j = 3] )
		j = 4;
	if ((i == 0 && j == 4) || (i == 1 && j == 3)) /* between highlight and shadow */
		lpMap->Pnt[2].x = (lpMap->Pnt[i].x + lpMap->Pnt[j].x) / 2;
	else if (i == 1 && j == 4) /* between 1/4 and shadow */
		lpMap->Pnt[2].x = lpMap->Pnt[i].x + ((lpMap->Pnt[j].x - lpMap->Pnt[i].x) / 3 );
	else if (i == 0 && j == 3) /* between highlight and 3/4 */
		lpMap->Pnt[2].x = lpMap->Pnt[i].x + ((2 * (lpMap->Pnt[j].x - lpMap->Pnt[i].x)) / 3);
	}
if ( !Qtone.QTmoved[1] )
	lpMap->Pnt[1].x = (lpMap->Pnt[0].x + lpMap->Pnt[2].x) / 2;;
if ( !Qtone.QTmoved[3] )
	lpMap->Pnt[3].x = (lpMap->Pnt[2].x + lpMap->Pnt[4].x) / 2;;

/* if any points are out of order, place them in order */
if (fSlideRight)
	{
	//for (i = iQTone+1; i < QTONEPOINTS; ++i)
	//	if (lpMap->Pnt[iQTone].x >= lpMap->Pnt[i].x)
	//		Qtone.QTmoved[i] = NO;
	for ( i=1; i<QTONEPOINTS; i++ )
		lpMap->Pnt[i].x = max(lpMap->Pnt[i].x, lpMap->Pnt[i-1].x );
	}
else
	{
	//for (i = iQTone-1; i >= 0; --i)
	//	if (lpMap->Pnt[iQTone].x <= lpMap->Pnt[i].x)
	//		Qtone.QTmoved[i] = NO;
	for ( i=QTONEPOINTS-2; i >= 0; i-- )
		lpMap->Pnt[i].x = min(lpMap->Pnt[i].x, lpMap->Pnt[i+1].x );
	}
}


/***********************************************************************/
void SetControlValue( hDlg, nIDDlgItem, nIndex, lValue, fNotify )
/***********************************************************************/
HWND hDlg;
int nIDDlgItem, nIndex;
long lValue;
BOOL fNotify;
{
HWND hDlgItem;

hDlgItem = GetDlgItem( hDlg, nIDDlgItem );
if (!hDlgItem)
	return;
if (fNotify)
    SendMessage( hDlgItem, WM_SETCONTROL, nIndex, lValue);
SetWindowLong( hDlgItem, nIndex*4, lValue );
}

/***********************************************************************/
long GetControlValue( hDlg, nIDDlgItem, nIndex )
/***********************************************************************/
HWND hDlg;
int nIDDlgItem, nIndex;
{
HWND hDlgItem;

hDlgItem = GetDlgItem( hDlg, nIDDlgItem );
if (!hDlgItem)
	return(0L);
return(GetWindowLong( GetDlgItem( hDlg, nIDDlgItem ), nIndex*4));
}


/***********************************************************************/
long FAR PASCAL TextBlockControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND hWindow;
unsigned message;
WORD wParam;
long lParam;
{
PAINTSTRUCT PS;
HDC hDC, hPDC;
WORD id;
DWORD Style, OldTextColor;
BOOL bWindowText, bCenter;
HANDLE hTxtBlk;
STRING WindowText;
LPSTR lpTxtBlk;
RECT Area;
short OldBkMode;
HFONT hOldFont;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	return(TRUE);
	break;

    case WM_PAINT:
	if ( !(hDC = BeginPaint (hWindow,&PS)) )
		break;
	Style = GetWindowLong (hWindow,GWL_STYLE);
	if ( bWindowText = (Style & TS_WINDOWTEXT) )
		{
		lpTxtBlk = (LPSTR)WindowText;
		GetWindowText(hWindow, lpTxtBlk, sizeof(STRING));
		}
	else	{
		id = GetWindowWord(hWindow,GWW_ID);
		hTxtBlk = FindResource( hInstAstral, MAKEINTRESOURCE(id),
			RT_RCDATA);
		if ( hTxtBlk )
			hTxtBlk = LoadResource( hInstAstral, hTxtBlk );
		if ( hTxtBlk )
			lpTxtBlk = LockResource( hTxtBlk );
		if ( !hTxtBlk || ! lpTxtBlk )
			{
			EndPaint (hWindow,&PS);
			break;
			}
		}

	bCenter = (Style & TS_CENTER);
	OldBkMode = SetBkMode (hDC,TRANSPARENT);
	OldTextColor = (Style & TS_PRESETSTYLE) ?
		GetTextColor (hDC) :
		SetTextColor (hDC,GetSysColor (COLOR_WINDOWTEXT));
	if ( Style & TS_ANSIVARFONT )
		hOldFont = SelectObject( hDC, GetStockObject(ANSI_VAR_FONT) );
	else	{
		hPDC = GetDC( GetParent(hWindow) );
		hOldFont = SelectObject( hPDC, GetStockObject(ANSI_VAR_FONT) );
		SelectObject( hPDC, hOldFont );
		hOldFont = SelectObject( hDC, hOldFont );
		ReleaseDC( GetParent(hWindow), hPDC );
		}
	GetClientRect (hWindow,&Area);
	DrawText (hDC,lpTxtBlk,-1,&Area,DT_NOPREFIX | DT_WORDBREAK |
		(bCenter ? DT_CENTER : 0));
	SetTextColor (hDC,OldTextColor);
	SetBkMode (hDC,OldBkMode);
	if (hOldFont)
		SelectObject (hDC,hOldFont);
	if (!bWindowText)
		{
		UnlockResource (hTxtBlk);
		FreeResource (hTxtBlk);
		}
	EndPaint (hWindow,&PS);
	break;

    case WM_LBUTTONDOWN:
	break;

    case WM_LBUTTONUP:
	break;

    case WM_MOUSEMOVE:
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
void DrawStaticBox( hDC, lpRect, bDoubleShadow )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL bDoubleShadow;
{
HPEN hOldPen;
RECT Rect;

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

// Draw the upper left border
hOldPen = SelectObject( hDC, Window.hShadowPen );
MoveTo( hDC, Rect.left, Rect.bottom );
LineTo( hDC, Rect.left, Rect.top );
LineTo( hDC, Rect.right, Rect.top );
// Draw the drop shadow
SelectObject( hDC, Window.hWhitePen );
LineTo( hDC, Rect.right, Rect.bottom );
LineTo( hDC, Rect.left, Rect.bottom );
InflateRect( &Rect, -1, -1 );

if ( bDoubleShadow )
	{
	// Draw the upper left border
	SelectObject( hDC, Window.hShadowPen );
	MoveTo( hDC, Rect.left, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.top );
	LineTo( hDC, Rect.right, Rect.top );
	// Draw the drop shadow
	SelectObject( hDC, Window.hWhitePen );
	LineTo( hDC, Rect.right, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.bottom );
	InflateRect( &Rect, -1, -1 );
	}

SelectObject( hDC, hOldPen );
}


/***********************************************************************/
void DrawSculptedBox( hDC, lpRect, bFocus, bDown, bDoubleShadow )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL bFocus, bDown, bDoubleShadow;
{
HPEN hOldPen;
RECT Rect;

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

// Draw the box frame
hOldPen = SelectObject( hDC, Window.hBlackPen );
MoveTo( hDC, Rect.left+1, Rect.top );
LineTo( hDC, Rect.right, Rect.top );
MoveTo( hDC, Rect.right, Rect.top+1 );
LineTo( hDC, Rect.right, Rect.bottom );
MoveTo( hDC, Rect.right-1, Rect.bottom );
LineTo( hDC, Rect.left, Rect.bottom );
MoveTo( hDC, Rect.left, Rect.bottom-1 );
LineTo( hDC, Rect.left, Rect.top );
InflateRect( &Rect, -1, -1 );

if ( bFocus )
	{ // Draw the focus rectangle
	MoveTo( hDC, Rect.left, Rect.top );
	LineTo( hDC, Rect.right, Rect.top );
	LineTo( hDC, Rect.right, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.top );
	InflateRect( &Rect, -1, -1 );
	}

// Draw the upper left border
if ( bDoubleShadow )
	{
	SelectObject( hDC, ( bDown ? Window.hBlackPen : Window.hWhitePen ) );
	MoveTo( hDC, Rect.left, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.top );
	LineTo( hDC, Rect.right, Rect.top );
	if ( bDown )
		{
		LineTo( hDC, Rect.right, Rect.top+1 );
		Rect.left++;
		Rect.top++;
		}
	else	{ // Draw the drop shadow
		SelectObject( hDC, Window.hShadowPen );
		LineTo( hDC, Rect.right, Rect.bottom );
		LineTo( hDC, Rect.left, Rect.bottom );
		InflateRect( &Rect, -1, -1 );
		}
	}

SelectObject( hDC, ( bDown ? Window.hShadowPen : Window.hWhitePen ) );
MoveTo( hDC, Rect.left, Rect.bottom );
LineTo( hDC, Rect.left, Rect.top );
LineTo( hDC, Rect.right, Rect.top );
if ( bDown )
	{
	LineTo( hDC, Rect.right, Rect.top+1 );
	Rect.left++;
	Rect.top++;
	}
else	{ // Draw the drop shadow
	SelectObject( hDC, Window.hShadowPen );
	LineTo( hDC, Rect.right, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.bottom );
	InflateRect( &Rect, -1, -1 );
	}

SelectObject( hDC, hOldPen );
}


#define IS_STYLE(dwStyle, theStyle) (((dwStyle) & 0x0FL) == (theStyle))

/***********************************************************************/
long FAR PASCAL StaticTextControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
PAINTSTRUCT ps;
RECT ClientRect;
HPEN hOldPen;
HDC hDC;
DWORD dwStyle;
STRING szString;

dwStyle = GetWindowLong( hWindow, GWL_STYLE );
switch ( message )
    {
    case WM_SETFONT:
	SetWindowLong(hWindow, 0, (long)wParam);
	if (lParam)
		{
		hDC = GetDC(hWindow);
		if (IS_STYLE(dwStyle, SS_LEFT) || 
		    IS_STYLE(dwStyle, SS_CENTER) ||
		    IS_STYLE(dwStyle, SS_RIGHT))
			DrawStaticText(hDC, hWindow, NULL);
		ReleaseDC(hWindow, hDC);
		}
	break;

    case WM_ERASEBKGND:
	return(TRUE);
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	if (dwStyle & SS_DOWNBOX)
		{
		GetClientRect(hWindow, &ClientRect);
		hOldPen = SelectObject(hDC, Window.hShadowPen);
		MoveTo(hDC, ClientRect.left, ClientRect.bottom - 1);
		LineTo(hDC, ClientRect.left, ClientRect.top);
		LineTo(hDC, ClientRect.right-2, ClientRect.top);
		SelectObject(hDC, Window.hWhitePen);
		LineTo(hDC, ClientRect.right-2, ClientRect.bottom - 1);
		LineTo(hDC, ClientRect.left, ClientRect.bottom - 1);
		SelectObject(hDC, hOldPen);
		}
	if (dwStyle & SS_SDOWNBOX)
		{
		GetClientRect(hWindow, &ClientRect);
		hOldPen = SelectObject(hDC, Window.hShadowPen);
		MoveTo(hDC, ClientRect.left, ClientRect.bottom - 2);
		LineTo(hDC, ClientRect.left, ClientRect.top+1);
		LineTo(hDC, ClientRect.right-2, ClientRect.top+1);
		SelectObject(hDC, Window.hWhitePen);
		LineTo(hDC, ClientRect.right-2, ClientRect.bottom - 2);
		LineTo(hDC, ClientRect.left, ClientRect.bottom - 2);
		SelectObject(hDC, hOldPen);
		}
	if (IS_STYLE(dwStyle, SS_LEFT) || 
	    IS_STYLE(dwStyle, SS_CENTER) ||
	    IS_STYLE(dwStyle, SS_RIGHT))
		DrawStaticText(hDC, hWindow, NULL);
	else if (IS_STYLE(dwStyle, SS_SIMPLE))
		{
		GetClientRect(hWindow, &ClientRect);
		hOldPen = SelectObject(hDC, Window.hWhitePen);
		MoveTo(hDC, ClientRect.left, ClientRect.top+2);
		LineTo(hDC, ClientRect.left, ClientRect.bottom-2);
		SelectObject(hDC, Window.hButtonPen);
		MoveTo(hDC, ClientRect.left+1, ClientRect.top+1);
		LineTo(hDC, ClientRect.left+1, ClientRect.bottom-1);
		SelectObject(hDC, Window.hShadowPen);
		MoveTo(hDC, ClientRect.left+2, ClientRect.top+2);
		LineTo(hDC, ClientRect.left+2, ClientRect.bottom-2);
		SelectObject(hDC, hOldPen);
		}
	EndPaint( hWindow, &ps );
	break;

    case WM_SETTEXT:
	hDC = GetDC(hWindow);
	if (IS_STYLE(dwStyle, SS_LEFT) || 
	    IS_STYLE(dwStyle, SS_CENTER) ||
	    IS_STYLE(dwStyle, SS_RIGHT))
		DrawStaticText(hDC, hWindow, (LPSTR)lParam);
	ReleaseDC(hWindow, hDC);
    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
void DrawStaticText(hDC, hWnd, lpString)
/***********************************************************************/
HDC hDC;
HWND hWnd;
LPSTR lpString;
{
DWORD dwStyle;
WORD wFormat;
HFONT hFont, hOldFont;
RECT ClientRect;
STRING szString;
int nOldBkMode, x, y;
LOGFONT font;

GetClientRect(hWnd, &ClientRect);
++ClientRect.right; ++ClientRect.bottom;
dwStyle = GetWindowLong( hWnd, GWL_STYLE );

if (lpString)
	lstrcpy(szString, lpString);
else 
	GetWindowText(hWnd, szString, MAX_STR_LEN);
if (dwStyle & SS_LIGHT)
	hFont = Window.hLabelFont;
else
	hFont = (HFONT)GetWindowLong(hWnd, 0);
if (hFont)
	hOldFont = SelectObject(hDC, hFont);

if (dwStyle & SS_DOWNBOX || dwStyle & SS_SDOWNBOX)
	{
	ClientRect.left += 2;
	ClientRect.right -= 3;
	ClientRect.top += 2;
	ClientRect.bottom -= 3;
	}
else
	EraseControlBackground( hDC, hWnd, &ClientRect, CTLCOLOR_STATIC );

if (lstrlen(szString) <= 0)
	{
	if (!(dwStyle & SS_LIGHT) && hFont)
		SelectObject(hDC, hOldFont);
	return;
	}

nOldBkMode = SetBkMode(hDC, TRANSPARENT);
if (IS_STYLE(dwStyle, SS_RIGHT))
	wFormat = DT_RIGHT;
else if (IS_STYLE(dwStyle, SS_CENTER))
	wFormat = DT_CENTER;
else	
	wFormat = DT_LEFT;
DrawText(hDC, szString, -1, &ClientRect, wFormat | DT_WORDBREAK);
SetBkMode(hDC, nOldBkMode);
if (!(dwStyle & SS_LIGHT) && hFont)
	SelectObject(hDC, hOldFont);
}


/***********************************************************************/
void EraseControlBackground( hDC, hWnd, lpRect, wType )
/***********************************************************************/
HDC hDC;
HWND hWnd;
LPRECT lpRect;
WORD wType;
{
HBRUSH hBrush;

hBrush = GetBackgroundBrush( hDC, hWnd, wType );
FillRect( hDC, lpRect, hBrush );
}


/***********************************************************************/
HBRUSH GetBackgroundBrush( hDC, hWnd, wType)
/***********************************************************************/
HDC hDC;
HWND hWnd;
WORD wType;
{
HBRUSH hBrush;

hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLOR, hDC, MAKELONG (hWnd,wType));
if (!hBrush)
	{
	hBrush = Window.hWhiteBrush;
	SetBkColor (hDC,GetSysColor (COLOR_WINDOW));
        SetTextColor (hDC,GetSysColor (COLOR_WINDOWTEXT));
	}
return( hBrush );
}

/***********************************************************************/
HBRUSH SelectBackgroundBrush( hDC, hWnd, wType )
/***********************************************************************/
HDC hDC;
HWND hWnd;
WORD wType;
{
HBRUSH hBrush;

hBrush = GetBackgroundBrush( hDC, hWnd, wType );
return( (HBRUSH)SelectObject( hDC, hBrush ) );
}

#define BUTTON_STYLE(dwStyle)((int)((dwStyle) & 0x00FFL))
#define FONT_OFFSET	0
#define VALUE_OFFSET	4

/***********************************************************************/
long FAR PASCAL ButtonControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
int x, y, i;
RECT ClientRect;
PAINTSTRUCT ps;
HICON hResource;
HDC hDC;
BOOL bHasFocus, bSelected, bDown, bEnabled, bShadows, Bool;
BOOL bHasText;
WORD idIcon;
DWORD dwStyle;
STRING szString;
static BOOL bTrack, bInRect;

switch ( message )
    {
    case WM_CREATE:
	SetWindowLong(hWindow, VALUE_OFFSET, 0L);
	break;

    case WM_SETFONT:
	SetWindowLong(hWindow, FONT_OFFSET, (long)wParam);
	if (lParam)
		InvalidateRect(hWindow, NULL, TRUE);
	break;

    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	return(TRUE);
	break;

    case WM_PAINT:
	hDC = BeginPaint( hWindow, &ps );
	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	switch (BUTTON_STYLE(dwStyle))
		{
		case BS_PUSHBUTTON:
		case BS_DEFPUSHBUTTON:
		case BS_RADIOBUTTON:
		case BS_GROUPBOX:
		case BS_USERBUTTON:
		case BS_AUTORADIOBUTTON:
		case BS_PUSHBOX:
		case BS_OWNERDRAW:
		case BS_LEFTTEXT:
			break;
		case BS_CHECKBOX:
		case BS_AUTOCHECKBOX:
		case BS_3STATE:
		case BS_AUTO3STATE:
			DrawCheckBox(hDC, hWindow);
			break;
		default:
			break;
		}
	EndPaint( hWindow, &ps );
	break;

    case WM_KEYDOWN:
	if ( wParam != VK_SPACE )
		break;

    case WM_LBUTTONDOWN:
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	bInRect = YES;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case WM_KEYUP:
	if ( wParam != VK_SPACE )
		break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	if ( bInRect )
	   SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), 0L );
	break;

    case WM_MOUSEMOVE:
	if ( !bTrack )
		break;
	GetClientRect( hWindow, &ClientRect );
	if ( bInRect == !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
		{
		bInRect = !bInRect;
		InvalidateRect( hWindow, NULL, FALSE );
		UpdateWindow( hWindow );
		}
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case BM_SETSTATE:
    case BM_SETCHECK:
	if ( wParam == GetWindowLong(hWindow, VALUE_OFFSET))
		break;
	SetWindowLong(hWindow, VALUE_OFFSET, (long)wParam);
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}

/***********************************************************************/
void DrawCheckBox(hDC, hWnd)
/***********************************************************************/
HDC hDC;
HWND hWnd;
{
RECT ClientRect, BoxRect;
BOOL bHasFocus;
BOOL bEnabled;
HPEN hOldPen;
HFONT hFont, hOldFont;
int minsize;
STRING szString;
DWORD dwStyle, hOldTextColor;
int nOldBkMode, x, y;

GetClientRect(hWnd, &ClientRect);
EraseControlBackground(hDC, hWnd, &ClientRect, CTLCOLOR_BTN);
dwStyle = GetWindowLong( hWnd, GWL_STYLE );
bHasFocus = ( GetFocus() == hWnd );
bEnabled  = !( dwStyle & WS_DISABLED );

BoxRect = ClientRect;
InflateRect(&BoxRect, -1, -1);
minsize = min(RectWidth(&BoxRect), RectHeight(&BoxRect));
BoxRect.right = BoxRect.left + minsize;
BoxRect.bottom = BoxRect.top + minsize;
hOldPen = SelectObject(hDC, Window.hBlackPen);
MoveTo(hDC, BoxRect.left, BoxRect.top);
LineTo(hDC, BoxRect.right-1, BoxRect.top);
LineTo(hDC, BoxRect.right-1, BoxRect.bottom-1);
LineTo(hDC, BoxRect.left, BoxRect.bottom-1);
LineTo(hDC, BoxRect.left, BoxRect.top);
if (GetWindowLong(hWnd, VALUE_OFFSET) != 0)
	{
	LineTo(hDC, BoxRect.right-1, BoxRect.bottom-1);
	MoveTo(hDC, BoxRect.right-1, BoxRect.top);
	LineTo(hDC, BoxRect.left, BoxRect.bottom-1);
	}
SelectObject(hDC, hOldPen);
if (!GetWindowText(hWnd, szString, MAX_STR_LEN))
	return;

BoxRect = ClientRect;
InflateRect(&BoxRect, -1, -1);
BoxRect.left += minsize + 2;
hFont = (HFONT)GetWindowLong(hWnd, FONT_OFFSET);
if (hFont)
	hOldFont = SelectObject(hDC, hFont);
//hOldTextColor = SetTextColor(hDC, GetSysColor(COLOR_BTNTEXT));
nOldBkMode = SetBkMode(hDC, TRANSPARENT);
DrawText(hDC, szString, -1, &BoxRect, DT_LEFT | DT_WORDBREAK);
SetBkMode(hDC, nOldBkMode);
//SetTextColor(hDC, hOldTextColor);
if (hFont)
	SelectObject(hDC, hOldFont);
}
