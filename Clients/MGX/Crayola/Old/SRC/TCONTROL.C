//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes
static void NewGridFromPoint( LPRECT lpGridRect, long lPoint );

// Static data
static int iImageWidth, iImageHeight, iImageDepth;
static RECT rControl;

/************************************************************************/
void SetImageControlMapping( int iImageW, int iImageH, LPRECT lpControlRect )
/************************************************************************/
{
iImageWidth  = iImageW;
iImageHeight = iImageH;
rControl = *lpControlRect;
}


/************************************************************************/
void Image2Control( LPINT lpx, LPINT lpy )
/************************************************************************/
{
int x, y;
LFIXED xrate, yrate;

xrate = FGET( RectWidth(&rControl), iImageWidth );
yrate = FGET( RectHeight(&rControl), iImageHeight );

x = *lpx;
*lpx = rControl.left + FMUL( x, xrate );
y = *lpy;
*lpy = rControl.top + FMUL( y, yrate );
}


// This routine returns the file pixel which is closest on the
// display to the passed in display pixel. (i.e. - uses rounding)
/************************************************************************/
void Control2Image( LPINT lpx, LPINT lpy )
/************************************************************************/
{
int x, y;
LFIXED xrate, yrate;

xrate = FGET( iImageWidth, RectWidth(&rControl) );
yrate = FGET( iImageHeight, RectHeight(&rControl) );

x = *lpx;
*lpx = FMUL( x - rControl.left, xrate );
y = *lpy;
*lpy = FMUL( y - rControl.top, yrate );
}


/***********************************************************************/
void DrawGrid( HWND hWindow, HDC hDC, LPRECT lpGridRect, int iType )
/***********************************************************************/
{
RECT rRect;

rRect = *lpGridRect;
Image2Control( (LPINT)&rRect.left,  (LPINT)&rRect.top );
Image2Control( (LPINT)&rRect.right, (LPINT)&rRect.bottom );
InvertSelection( hWindow, hDC, &rRect, iType );
}

/***********************************************************************/
void InvertGrid( HWND hWindow, HDC hDC, LPRECT lpGridRect, long lPoint )
/***********************************************************************/
{
RECT rRect1, rRect2;

rRect1 = *lpGridRect;
Image2Control( (LPINT)&rRect1.left,  (LPINT)&rRect1.top );
Image2Control( (LPINT)&rRect1.right, (LPINT)&rRect1.bottom );
if ( !lPoint )
	{
	InvertSelection( hWindow, hDC, &rRect1, SL_BLOCK );
	return;
	}

NewGridFromPoint( lpGridRect, lPoint );
rRect2 = *lpGridRect;
Image2Control( (LPINT)&rRect2.left,  (LPINT)&rRect2.top );
Image2Control( (LPINT)&rRect2.right, (LPINT)&rRect2.bottom );

if ( !EqualRect( &rRect1, &rRect2 ) )
	{
	InvertSelection( hWindow, hDC, &rRect1, SL_BLOCK );
	InvertSelection( hWindow, hDC, &rRect2, SL_BLOCK );
	}
}


/***********************************************************************/
static void NewGridFromPoint( LPRECT lpGridRect, long lPoint )
/***********************************************************************/
{
int x, y, dx, dy;

x = LOWORD(lPoint);
y = HIWORD(lPoint);
Control2Image( &x, &y );

dx = abs( RectWidth(lpGridRect) );
dy = abs( RectHeight(lpGridRect) );
while ( x < lpGridRect->left )		OffsetRect( lpGridRect, -dx, 0 );
while ( x > lpGridRect->right )		OffsetRect( lpGridRect, dx, 0 );
while ( y < lpGridRect->top )		OffsetRect( lpGridRect, 0, -dy );
while ( y > lpGridRect->bottom )	OffsetRect( lpGridRect, 0, dy );
}
