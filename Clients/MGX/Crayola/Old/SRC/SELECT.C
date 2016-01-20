//®PL1¯®FD1¯®TP0¯®BT0¯®PL1¯
// ( c ) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern int nScale;  // set in tdraw.c


// Static prototypes
static void MoveInsideRect( LPRECT lpRect, LPRECT lpBoundRect );
static void DrawHandles( HDC hDC, LPRECT lpSelectRect );
static void DisplayHook( HDC hDC, LPRECT lpRect );
static LPRECT GetBoundingRect( HWND hWnd );
static void GetCenter( LPRECT lpRect, LPPOINT lpPoint );

static int Constrain, Mode;
static POINT ptCenter;
static POINT ptAnchor;
static LFIXED fScale;
static LPDISPLAYHOOK lpLastDisplayHook;

#define CONSTRAINX 1
#define CONSTRAINY 2
#define DRAG_MODE 1
#define MOVE_MODE 2
#define HSIZE 2


//////////////////////////////////////////////////////////////////////////////
void StartSelection(
// PURPOSE: Begin selection of region
//////////////////////////////////////////////////////////////////////////////
HWND 	hWnd,
HDC 	hDC,
LPRECT 	lpSelectRect,
int 	fFlags,
POINT 	ptCurrent,
LFIXED 	fStatusScale)
{
    int tmp, minx, maxx, miny, maxy;
    LPRECT lpRect;
    BOOL fDoc;

    if( fDoc = IsDoc( hWnd ))
    {
	    lpLastDisplayHook = SetDisplayHook( hWnd, DisplayHook );
	    Display2File( (LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y );
    }

    fScale = fStatusScale;

    if( !( fFlags & SL_NOLIMIT ))
    {
	    lpRect = GetBoundingRect( hWnd );
	    ptCurrent.x = bound( ptCurrent.x, lpRect->left, lpRect->right );
	    ptCurrent.y = bound( ptCurrent.y, lpRect->top, lpRect->bottom );
    }

    ptAnchor = ptCurrent;

    Mode = DRAG_MODE;

    if( fFlags & SL_SPECIAL )
    {   
        // Setup a selection rectangle at the current location 
	    Constrain = 0;
	    lpSelectRect->left = ptCurrent.x;
	    lpSelectRect->top = ptCurrent.y;
	    lpSelectRect->right = ptCurrent.x;
	    lpSelectRect->bottom = ptCurrent.y;

	    InvertSelection( hWnd, hDC, lpSelectRect, fFlags );

	    GetCenter( lpSelectRect, &ptCenter );

	    return;
	}

    // Modify the existing selection rectangle
    // First clip it to the window's client area
    if( !( fFlags & SL_NOLIMIT ))
	    IntersectRect( lpSelectRect, lpSelectRect, GetBoundingRect( hWnd ));

    // Figure out which side or corner to drag
    // The bottom right point is always the one that moves
    Constrain = CONSTRAINX | CONSTRAINY;

    if( ptCurrent.x >= lpSelectRect->left - CLOSENESS &&
	    ptCurrent.x <= lpSelectRect->right + CLOSENESS &&
	    ptCurrent.y >= lpSelectRect->top - CLOSENESS &&
	    ptCurrent.y <= lpSelectRect->bottom + CLOSENESS )
	{
	    if(( fFlags & SL_TYPE ) == SL_BOXHANDLES )
		    ConstrainHandles( ptCurrent, lpSelectRect );
	    else
	    {
		    if( abs( ptCurrent.y - lpSelectRect->bottom ) <= CLOSENESS )
			    Constrain ^= CONSTRAINY;
		    else
		    if( abs( ptCurrent.y - lpSelectRect->top ) <= CLOSENESS )
		    {
			    Constrain ^= CONSTRAINY;
			    tmp = lpSelectRect->top;
			    lpSelectRect->top = lpSelectRect->bottom;
			    lpSelectRect->bottom = tmp;
		    }
		    if( abs( ptCurrent.x - lpSelectRect->right ) <= CLOSENESS )
			    Constrain ^= CONSTRAINX;
		    else
		    if( abs( ptCurrent.x - lpSelectRect->left ) <= CLOSENESS )
		    {
			    Constrain ^= CONSTRAINX;
			    tmp = lpSelectRect->left;
			    lpSelectRect->left = lpSelectRect->right;
			    lpSelectRect->right = tmp;
		    }
	    }
	    if( Constrain == ( CONSTRAINX | CONSTRAINY ))
	    {
		    minx = min( lpSelectRect->left, lpSelectRect->right )+CLOSENESS;
		    maxx = max( lpSelectRect->left, lpSelectRect->right )-CLOSENESS;
		    miny = min( lpSelectRect->top, lpSelectRect->bottom )+CLOSENESS;
		    maxy = max( lpSelectRect->top, lpSelectRect->bottom )-CLOSENESS;

		    if( ptCurrent.x >= minx && ptCurrent.x <= maxx &&
		        ptCurrent.y >= miny && ptCurrent.y <= maxy )
			{
			    Mode = MOVE_MODE;
			    lpSelectRect->left = minx-CLOSENESS;
			    lpSelectRect->right = maxx+CLOSENESS;
			    lpSelectRect->top = miny-CLOSENESS;
			    lpSelectRect->bottom = maxy+CLOSENESS;
			}
		}
	}
    GetCenter( lpSelectRect, &ptCenter );
    StatusOfRectangle( lpSelectRect, fScale );
}


//////////////////////////////////////////////////////////////////////////////
void UpdateSelection(
// PURPOSE: Update selection 
//////////////////////////////////////////////////////////////////////////////
HWND 	hWnd,
HDC 	hDC,
LPRECT  lpSelectRect,
int     fFlags,
POINT   ptCurrent,
BOOL    bConstrain,
long    AspectX,
long	AspectY,
BOOL    bMove,
BOOL    bFromCenter)
{
    LPRECT lpRect;
    BOOL fNeedDC;
    int dx, dy;
    POINT ptSaved;
    RECT OldRect, drect;
    BOOL fDoc;

    if( Mode == MOVE_MODE )
    {
	    bMove = YES;
    }
    else
    if( Constrain == ( CONSTRAINX | CONSTRAINY ))    // It hasn't been started yet */
	{
	    StartSelection( hWnd, hDC, lpSelectRect, fFlags, ptCurrent, fScale );
	}

    if( fDoc = IsDoc( hWnd ))
	    Display2File( (LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y );

    if( !( fFlags & SL_NOLIMIT ))
    {                                   // Constrain point to window client area
	    lpRect = GetBoundingRect( hWnd );
	    ptCurrent.x = bound( ptCurrent.x, lpRect->left, lpRect->right );
	    ptCurrent.y = bound( ptCurrent.y, lpRect->top, lpRect->bottom );
    }

    ptSaved = ptCurrent;

    if( !bMove )
	{
	    // Constrain point in x-direction based on where grabbed
	    if( Constrain & CONSTRAINX )
		    ptCurrent.x = lpSelectRect->right;

	    // Constrain point in y-direction based on where grabbed
	    if( Constrain & CONSTRAINY )
		    ptCurrent.y = lpSelectRect->bottom;
	}

    if( fNeedDC = ( !hDC ))
	    hDC = GetDC( hWnd );
             
    switch( fFlags & SL_TYPE )
    {
	    case SL_BOXHANDLES:
	    case SL_GRID:
	    case SL_LINE:

        // 2D shapes
	    case SL_BOX:
        case SL_ELLIPSE:
	    case SL_HTRIANGLE:
	    case SL_VTRIANGLE:
	    case SL_HEXAGON:   
	    case SL_OCTAGON:   
	    case SL_HSEMICIRCLE:
	    case SL_VSEMICIRCLE:

        // 3D shapes
	    case SL_CUBE:
        case SL_PCUBE:
	    case SL_CYLINDER:  
        case SL_PCYLINDER:
	    case SL_PYRAMID:
        case SL_PPYRAMID:
	    case SL_WEDGE:
        case SL_PWEDGE:
	    case SL_HCONE:
	    case SL_VCONE:
		    InvertSelection( hWnd, hDC, lpSelectRect, fFlags );

		    if( bMove )
			{
			    dx = ptCurrent.x - ptAnchor.x;
			    dy = ptCurrent.y - ptAnchor.y;

			    lpSelectRect->right  += dx;
			    lpSelectRect->bottom += dy;
			    lpSelectRect->top    += dy;
			    lpSelectRect->left   += dx;

			    ptCenter.x += dx;
			    ptCenter.y += dy;
			}
		    else
			{
			    dx = ptCurrent.x - lpSelectRect->right;
			    dy = ptCurrent.y - lpSelectRect->bottom;

			    lpSelectRect->right = ptCurrent.x;
			    lpSelectRect->bottom = ptCurrent.y;
			}

		    if( bConstrain )
			    ConstrainSelection( lpSelectRect, AspectX, AspectY, bFromCenter );

		    dx = lpSelectRect->right - lpSelectRect->left;
		    dy = lpSelectRect->bottom - lpSelectRect->top;

		    if( bFromCenter )
		    {
			    lpSelectRect->left  = ptCenter.x - dx / 2;
			    lpSelectRect->top   = ptCenter.y - dy / 2;
			    lpSelectRect->right  = ptCenter.x + dx - dx / 2;
			    lpSelectRect->bottom   = ptCenter.y + dy - dy / 2;
		    }

		    GetCenter( lpSelectRect, &ptCenter );

		    // Keep the new rectangle inside the window's client area
		    if( !( fFlags & SL_NOLIMIT ))
			    MoveInsideRect( lpSelectRect, GetBoundingRect( hWnd ));

		    InvertSelection( hWnd, hDC, lpSelectRect, fFlags );
		    break;


	    case SL_BLOCK:
		    OldRect = *lpSelectRect;
		    dx = ptCurrent.x - lpSelectRect->right;
		    dy = ptCurrent.y - lpSelectRect->bottom;
		    lpSelectRect->right = ptCurrent.x;
		    lpSelectRect->bottom = ptCurrent.y;

		    if ( bConstrain )
			    ConstrainSelection( lpSelectRect, AspectX, AspectY, bFromCenter );

		    if ( bFromCenter )
		    {
			    lpSelectRect->left  = ptCenter.x - dx / 2;
			    lpSelectRect->top   = ptCenter.y - dy / 2;
			    lpSelectRect->right  = ptCenter.x + dx - dx / 2;
			    lpSelectRect->bottom   = ptCenter.y + dy - dy / 2;
		    }

		    GetCenter( lpSelectRect, &ptCenter );
		    drect.left = OldRect.left - lpSelectRect->left;
		    drect.right = OldRect.right - lpSelectRect->right;
		    drect.top = OldRect.top - lpSelectRect->top;
		    drect.bottom = OldRect.bottom - lpSelectRect->bottom;

		    PatBlt( hDC, lpSelectRect->left, lpSelectRect->bottom,
			    OldRect.right - lpSelectRect->left,
			    drect.bottom, DSTINVERT );
		    PatBlt( hDC, lpSelectRect->right, OldRect.top,
			    drect.right,
			    lpSelectRect->bottom - OldRect.top, DSTINVERT );
		    PatBlt( hDC, OldRect.left, OldRect.top,
			    lpSelectRect->right-OldRect.left,
			    drect.top, DSTINVERT );
		    PatBlt( hDC, OldRect.left, lpSelectRect->top,
			    drect.left,
			    OldRect.bottom-lpSelectRect->top, DSTINVERT );

		    break;
	}

    ptAnchor = ptSaved;
    StatusOfRectangle( lpSelectRect, fScale );

    if( fNeedDC )
	    ReleaseDC( hWnd, hDC );
}


//////////////////////////////////////////////////////////////////////////
void UpdateExtrudedSelection(
// PURPOSE: Update extruded selection 
//////////////////////////////////////////////////////////////////////////
HWND 	hWnd,
HDC 	hDC,
LPRECT  lpSelectRect,
LPRECT  lpSelectRectNext,
int     fFlags,
POINT   ptCurrent,
BOOL    bConstrain,
long    AspectX,
long	AspectY,
BOOL    bMove,
BOOL    bFromCenter)
{
    LPRECT lpRect;
    BOOL fNeedDC;
    int dx, dy;
    POINT ptSaved;
    BOOL fDoc;


    if( fDoc = IsDoc( hWnd ))
	    Display2File( (LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y );

    if( !( fFlags & SL_NOLIMIT ))
    {                                   // Constrain point to window client area
	    lpRect = GetBoundingRect( hWnd );
	    ptCurrent.x = bound( ptCurrent.x, lpRect->left, lpRect->right );
	    ptCurrent.y = bound( ptCurrent.y, lpRect->top, lpRect->bottom );
    }

    ptSaved = ptCurrent;

    if( fNeedDC = ( !hDC ))
	    hDC = GetDC( hWnd );

    // erase current position
   	InvertExtrudedSelection( hWnd, hDC, lpSelectRect, lpSelectRectNext, fFlags );

    if((( fFlags & SL_TYPE ) == SL_CYLINDER ) || (( fFlags & SL_TYPE ) == SL_PCYLINDER ))
    {
        if(( lpSelectRect->bottom - lpSelectRect->top ) <=
           ( lpSelectRect->right - lpSelectRect->left ))
        {
		    dx = 0;         // vertical cylinder only
        	dy = ptCurrent.y - ptAnchor.y;
        }
        else                // horizontal cylinder only
        {
    		dx = ptCurrent.x - ptAnchor.x;
        	dy = 0;
        }
    }
    else
    if(( fFlags & SL_TYPE ) == SL_HCONE )
    {
        // horizontal extrusion only
   		dx = ptCurrent.x - ptAnchor.x;
       	dy = 0;
    }
    else
    if(( fFlags & SL_TYPE ) == SL_VCONE )
    {
        // vertical extrusion only
	    dx = 0;         
       	dy = ptCurrent.y - ptAnchor.y;
    }
    else
    {
		dx = ptCurrent.x - ptAnchor.x;
    	dy = ptCurrent.y - ptAnchor.y;
    }

	lpSelectRectNext->right  += dx;
	lpSelectRectNext->bottom += dy;
	lpSelectRectNext->top    += dy;
	lpSelectRectNext->left   += dx;

	ptCenter.x += dx;
	ptCenter.y += dy;

	// Keep the new rectangle inside the window's client area 
	if( !( fFlags & SL_NOLIMIT ))
		MoveInsideRect( lpSelectRectNext, GetBoundingRect( hWnd ));

   	InvertExtrudedSelection( hWnd, hDC, lpSelectRect, lpSelectRectNext, fFlags );

    ptAnchor = ptSaved;

    StatusOfRectangle( lpSelectRectNext, fScale );

    if( fNeedDC )
	    ReleaseDC( hWnd, hDC );
}


//////////////////////////////////////////////////////////////////////////////
static void GetCenter( LPRECT lpRect, LPPOINT lpPoint )
// PURPOSE: Gets center of rect
//////////////////////////////////////////////////////////////////////////////
{
    int dx, dy;

    dx = lpRect->right - lpRect->left;
    dy = lpRect->bottom - lpRect->top;

    // choose new center
    lpPoint->x = lpRect->left + dx / 2;
    lpPoint->y = lpRect->top + dy / 2;
}


//////////////////////////////////////////////////////////////////////////////
static void MoveInsideRect( LPRECT lpRect, LPRECT lpBoundRect )
//  Assumes that lpBoundRect is Ordered and that lpRect may not be
//  Also assumes that lpRect fits within lpBundRect
//////////////////////////////////////////////////////////////////////////////
{ 
    int x, y, dx, dy, l, r, t, b;

    if(( l = lpRect->left ) > ( r = lpRect->right ))
	{ 
        x = l; l = r; r = x; 
    }
    if(( x = lpBoundRect->left - l ) > 0 )      // too far left
	    dx = x;                                 // push it to right
    else
    if(( x = lpBoundRect->right - r ) < 0 )     // too far right
	    dx = x;                                 // push it to left
    else
	    dx = 0;

    if(( t = lpRect->top ) > ( b = lpRect->bottom ))
	{ 
        y = t; t = b; b = y; 
    }

    if(( y = lpBoundRect->top - t ) > 0 )       // too far up
	    dy = y;                                 // push it down
    else
    if(( y = lpBoundRect->bottom - b ) < 0 )    // too far down
	    dy = y;                                 // push it up
    else
	    dy = 0;

    OffsetRect(  lpRect, dx, dy );
}


//////////////////////////////////////////////////////////////////////////////
void InvertSelection( HWND hWnd, HDC hDC, LPRECT lpRect, int fFlags )
// PURPOSE: Inverts the current selection
//////////////////////////////////////////////////////////////////////////////
{
    HBRUSH hOldBrush;
    short OldROP;
    BOOL fNeedDC;
    int x, y, dx, dy, type;
    RECT ClientRect, SelectRect;
    static RECT LastSelectRect;
    static int LastFlags;
    BOOL  fDoc;
    POINT Points[20];
    POINT deltas, mids;
    int nPoints, index;
	LPPOINT lpPoints;
    TFORM tform;

    if( fFlags && lpRect )
	{                                   // Saved for the display hook
	    LastSelectRect = *lpRect;
	    LastFlags = fFlags;
	}
    else
    {                                   // Called from the display hook
	    fFlags = LastFlags;
    }

    type = fFlags & SL_TYPE;
    SelectRect = LastSelectRect;

    if( fDoc = IsDoc( hWnd ))
    {
	    if( type == SL_LINE || type == SL_HTRIANGLE || type == SL_VTRIANGLE )
	    {
		    File2DisplayEx( (LPINT)&SelectRect.left,  (LPINT)&SelectRect.top, YES );
		    File2DisplayEx( (LPINT)&SelectRect.right, (LPINT)&SelectRect.bottom, YES );
	    }
	    else
	    {
            if( type == SL_WEDGE    || type == SL_PWEDGE    || 
                type == SL_PYRAMID  || type == SL_PPYRAMID  || 
                type == SL_VSEMICIRCLE )
    	   	    OrderHPoints( &SelectRect, &SelectRect );
            else
            if( type == SL_HSEMICIRCLE )
    	   	    OrderVPoints( &SelectRect, &SelectRect );
            else 
	   	        OrderRect( &SelectRect, &SelectRect );

		    File2DispRectExact( hWnd, &SelectRect, &SelectRect );	
	    }
    }

    if( fNeedDC = ( !hDC ))
	    hDC = GetDC( hWnd );

    switch( type )
    {
	    case SL_BLOCK:
	        PatBlt( hDC, SelectRect.left, SelectRect.top,
		        SelectRect.right - SelectRect.left,
		        SelectRect.bottom - SelectRect.top, DSTINVERT );
	        break;


        case SL_BOXHANDLES:
	        OldROP = SetROP2( hDC, R2_NOT );
	        MoveToEx( hDC, SelectRect.left, SelectRect.top, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.top );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );
	        LineTo( hDC, SelectRect.left, SelectRect.top );
	        SetROP2( hDC, OldROP );
	        DrawHandles( hDC, &SelectRect );
	        break;


	    case SL_GRID:
	        OldROP = SetROP2( hDC, R2_NOT );
	        GetClientRect( hWnd, &ClientRect );
	        dx = abs( RectWidth( &SelectRect ));
	        dy = abs( RectHeight( &SelectRect ));

	        if ( dx > CLOSENESS && dy > CLOSENESS )
		    {
		        x = min( SelectRect.left, SelectRect.right );
		        while ( x >= ClientRect.left )
			    {
			        MoveToEx( hDC, x, ClientRect.top, NULL );
			        LineTo( hDC, x, ClientRect.bottom );
			        x -= dx;
			    }

		        x = max( SelectRect.left, SelectRect.right );
		        while ( x <= ClientRect.right )
			    {
			        MoveToEx( hDC, x, ClientRect.top, NULL );
			        LineTo( hDC, x, ClientRect.bottom );
			        x += dx;
			    }

		        y = min( SelectRect.top, SelectRect.bottom );
		        while ( y >= ClientRect.top )
			    {
			        MoveToEx( hDC, ClientRect.left, y, NULL );
			        LineTo( hDC, ClientRect.right, y );
			        y -= dy;
			    }

		        y = max( SelectRect.top, SelectRect.bottom );
		        while ( y <= ClientRect.bottom )
			    {
			        MoveToEx( hDC, ClientRect.left, y, NULL );
			        LineTo( hDC, ClientRect.right, y );
			        y += dy;
			    }
		    }
	        else
		    {
		        MoveToEx( hDC, SelectRect.left, SelectRect.top, NULL );
		        LineTo( hDC, SelectRect.right, SelectRect.top );
		        LineTo( hDC, SelectRect.right, SelectRect.bottom );
		        LineTo( hDC, SelectRect.left, SelectRect.bottom );
		        LineTo( hDC, SelectRect.left, SelectRect.top );
		    }
	        SetROP2( hDC, OldROP );
	        break;


	    case SL_LINE:
	        OldROP = SetROP2( hDC, R2_NOT );
	        MoveToEx( hDC, SelectRect.left, SelectRect.top, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );
	        SetROP2( hDC, OldROP );
	        break;


	    case SL_BOX:
       	    OldROP = SetROP2( hDC, R2_NOT );
	        MoveToEx( hDC, SelectRect.left, SelectRect.top, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.top );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );
	        LineTo( hDC, SelectRect.left, SelectRect.top );
	        SetROP2( hDC, OldROP );
	        break;


	    case SL_ELLIPSE:
	        OldROP = SetROP2( hDC, R2_NOT );
	        hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject( NULL_BRUSH ));
	        Ellipse( hDC, SelectRect.left, SelectRect.top,
		        SelectRect.right, SelectRect.bottom );
	        SetROP2( hDC, OldROP );
	        SelectObject( hDC, hOldBrush );
	        break;


	    case SL_HTRIANGLE:
	        OldROP = SetROP2( hDC, R2_NOT );
	        MoveToEx( hDC, SelectRect.left, SelectRect.top, NULL );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );
	        LineTo( hDC, SelectRect.right, ( SelectRect.top+SelectRect.bottom ) / 2 );
	        LineTo( hDC, SelectRect.left, SelectRect.top );
	        SetROP2( hDC, OldROP );
	        break;


        case SL_VTRIANGLE:
	        OldROP = SetROP2( hDC, R2_NOT );
	        MoveToEx( hDC, SelectRect.left, SelectRect.bottom, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );
	        LineTo( hDC, ( SelectRect.left + SelectRect.right ) / 2, SelectRect.top );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );
	        SetROP2( hDC, OldROP );
	        break;


	    case SL_HEXAGON:
       	    OldROP = SetROP2( hDC, R2_NOT );
	        MoveToEx( hDC, SelectRect.left, ( SelectRect.top + SelectRect.bottom ) / 2, NULL );
 	        LineTo( hDC, SelectRect.left + (( SelectRect.right - SelectRect.left ) / 4 ), SelectRect.bottom );
	        LineTo( hDC, SelectRect.left + ( 3 * ( SelectRect.right - SelectRect.left ) / 4 ), SelectRect.bottom );
	        LineTo( hDC, SelectRect.right, ( SelectRect.top + SelectRect.bottom ) / 2 );
	        LineTo( hDC, SelectRect.left + ( 3 * ( SelectRect.right - SelectRect.left ) / 4 ), SelectRect.top );
	        LineTo( hDC, SelectRect.left + (( SelectRect.right - SelectRect.left ) / 4 ), SelectRect.top );
	        LineTo( hDC, SelectRect.left, ( SelectRect.top + SelectRect.bottom ) / 2 );
	        SetROP2( hDC, OldROP );
	        break;


	    case SL_OCTAGON:
       	    OldROP = SetROP2( hDC, R2_NOT );
 	        MoveToEx( hDC, SelectRect.left, SelectRect.top + ( 3 * ( SelectRect.bottom - SelectRect.top ) / 10 ), NULL);
	        LineTo( hDC, SelectRect.left, SelectRect.bottom - ( 3 * ( SelectRect.bottom - SelectRect.top ) / 10 ));
	        LineTo( hDC, SelectRect.left + ( 3 * ( SelectRect.right - SelectRect.left ) / 10 ), SelectRect.bottom );
	        LineTo( hDC, SelectRect.right - ( 3 * ( SelectRect.right - SelectRect.left ) / 10 ), SelectRect.bottom );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom - ( 3 * ( SelectRect.bottom - SelectRect.top ) / 10 ));
	        LineTo( hDC, SelectRect.right, SelectRect.top + ( 3 * ( SelectRect.bottom - SelectRect.top ) / 10 ));
	        LineTo( hDC, SelectRect.right - ( 3 * ( SelectRect.right - SelectRect.left ) / 10 ), SelectRect.top );
	        LineTo( hDC, SelectRect.left + ( 3 * ( SelectRect.right - SelectRect.left ) / 10 ), SelectRect.top );
 	        LineTo( hDC, SelectRect.left, SelectRect.top + ( 3 * ( SelectRect.bottom - SelectRect.top ) / 10 ));
	        SetROP2( hDC, OldROP );
	        break;


	    case SL_HSEMICIRCLE:
	        OldROP = SetROP2( hDC, R2_NOT );

	        deltas.x = ( 2 * (SelectRect.right - SelectRect.left)) / 9;
	        deltas.y = ( 2 * (SelectRect.bottom - SelectRect.top)) / 9;
            mids.x = SelectRect.left;
	        mids.y = ( SelectRect.top + SelectRect.bottom ) / 2;

            // upper right bezier arc
	        Points[0].x = BEZIER_MARKER;
	        Points[0].y = 4;
	        Points[1].x = mids.x;
	        Points[1].y = SelectRect.top;
	        Points[2].x = SelectRect.right - deltas.x;
	        Points[2].y = SelectRect.top;
	        Points[3].x = SelectRect.right;
	        Points[3].y = SelectRect.top + deltas.y;
	        Points[4].x = SelectRect.right;
	        Points[4].y = mids.y;

            // lower right bezier arc
	        Points[5].x = BEZIER_MARKER;
	        Points[5].y = 4;
	        Points[6].x = SelectRect.right;
	        Points[6].y = mids.y;
	        Points[7].x = SelectRect.right;
	        Points[7].y = SelectRect.bottom - deltas.y;
	        Points[8].x = SelectRect.right - deltas.x;
	        Points[8].y = SelectRect.bottom;
	        Points[9].x = mids.x;
	        Points[9].y = SelectRect.bottom;
	        nPoints = 10;

        	lpPoints = ( LPPOINT )LineBuffer[0];
            TInit( &tform );
            nPoints = ExpandPoints( &Points[0], lpPoints, nPoints, 
         	         			    &tform, &lpPoints[0], &lpPoints[ nPoints - 1 ] );
            // draw semicircle
	        MoveToEx( hDC, lpPoints[0].x, lpPoints[0].y, NULL );

    	    for( index = 1; index < nPoints; index++ )
     	        LineTo( hDC, lpPoints[index].x, lpPoints[index].y );

   	        LineTo( hDC, lpPoints[0].x, lpPoints[0].y );

	        SetROP2( hDC, OldROP );
	        SelectObject( hDC, hOldBrush );
	        break;


	    case SL_VSEMICIRCLE:
	        OldROP = SetROP2( hDC, R2_NOT );

	        deltas.x = ( 2 * (SelectRect.right - SelectRect.left)) / 9;
	        deltas.y = ( 2 * (SelectRect.bottom - SelectRect.top)) / 9;
            mids.x = (SelectRect.left + SelectRect.right ) / 2;
	        mids.y = SelectRect.bottom;

            // upper left bezier arc
	        Points[0].x = BEZIER_MARKER;
	        Points[0].y = 4;
	        Points[1].x = SelectRect.left;
	        Points[1].y = mids.y;
	        Points[2].x = SelectRect.left;
	        Points[2].y = SelectRect.top + deltas.y;
	        Points[3].x = SelectRect.left + deltas.x;
	        Points[3].y = SelectRect.top;
	        Points[4].x = mids.x;
	        Points[4].y = SelectRect.top;

            // upper right bezier arc
	        Points[5].x = BEZIER_MARKER;
	        Points[5].y = 4;
	        Points[6].x = mids.x;
	        Points[6].y = SelectRect.top;
	        Points[7].x = SelectRect.right - deltas.x;
	        Points[7].y = SelectRect.top;
	        Points[8].x = SelectRect.right;
	        Points[8].y = SelectRect.top + deltas.y;
	        Points[9].x = SelectRect.right;
	        Points[9].y = mids.y;
	        nPoints = 10;

        	lpPoints = ( LPPOINT )LineBuffer[0];
            TInit( &tform );
            nPoints = ExpandPoints( &Points[0], lpPoints, nPoints, 
         	         			    &tform, &lpPoints[0], &lpPoints[ nPoints - 1 ] );
            // draw semicircle
	        MoveToEx( hDC, lpPoints[0].x, lpPoints[0].y, NULL );

    	    for( index = 1; index < nPoints; index++ )
     	        LineTo( hDC, lpPoints[index].x, lpPoints[index].y );

   	        LineTo( hDC, lpPoints[0].x, lpPoints[0].y );

	        SetROP2( hDC, OldROP );
	        SelectObject( hDC, hOldBrush );
	        break;


        case SL_WEDGE:
        case SL_PWEDGE:
        case SL_PYRAMID:
        case SL_PPYRAMID:
	        OldROP = SetROP2( hDC, R2_NOT );
	        MoveToEx( hDC, SelectRect.left, SelectRect.bottom, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );
	        LineTo( hDC, ( SelectRect.left + SelectRect.right ) / 2, SelectRect.top );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );
	        SetROP2( hDC, OldROP );
	        break;


	    case SL_CUBE:
	    case SL_PCUBE:
       	    OldROP = SetROP2( hDC, R2_NOT );
	        MoveToEx( hDC, SelectRect.left, SelectRect.top, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.top );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );
	        LineTo( hDC, SelectRect.left, SelectRect.top );
	        SetROP2( hDC, OldROP );
	        break;


        case SL_HCONE:
        case SL_VCONE:
       	    OldROP = SetROP2( hDC, R2_NOT );

 	        deltas.x = ( 2 * ( SelectRect.right - SelectRect.left )) / 9;
	        deltas.y = ( 2 * ( SelectRect.bottom - SelectRect.top )) / 9;
            mids.x   = ( SelectRect.left + SelectRect.right ) / 2;
	        mids.y   = ( SelectRect.top + SelectRect.bottom ) / 2;

            // upper right arc
	        Points[0].x = BEZIER_MARKER;
	        Points[0].y = 4;
	        Points[1].x = mids.x;
	        Points[1].y = SelectRect.top;
	        Points[2].x = SelectRect.right - deltas.x;
	        Points[2].y = SelectRect.top;
	        Points[3].x = SelectRect.right;
	        Points[3].y = SelectRect.top + deltas.y;
	        Points[4].x = SelectRect.right;
	        Points[4].y = mids.y;

            // lower right arc
	        Points[5].x = BEZIER_MARKER;
	        Points[5].y = 4;
	        Points[6].x = SelectRect.right;
	        Points[6].y = mids.y;
	        Points[7].x = SelectRect.right;
	        Points[7].y = SelectRect.bottom - deltas.y;
	        Points[8].x = SelectRect.right - deltas.x;
	        Points[8].y = SelectRect.bottom;
	        Points[9].x = mids.x;
	        Points[9].y = SelectRect.bottom;

            // lower left arc
	        Points[10].x = BEZIER_MARKER;
	        Points[10].y = 4;
	        Points[11].x = mids.x;
	        Points[11].y = SelectRect.bottom;
	        Points[12].x = SelectRect.left + deltas.x;
	        Points[12].y = SelectRect.bottom;
	        Points[13].x = SelectRect.left;
	        Points[13].y = SelectRect.bottom - deltas.y;
	        Points[14].x = SelectRect.left;
	        Points[14].y = mids.y;

            // upper left arc
	        Points[15].x = BEZIER_MARKER;
	        Points[15].y = 4;
	        Points[16].x = SelectRect.left;
	        Points[16].y = mids.y;
	        Points[17].x = SelectRect.left;
	        Points[17].y = SelectRect.top + deltas.y;
	        Points[18].x = SelectRect.left + deltas.x;
	        Points[18].y = SelectRect.top;
	        Points[19].x = mids.x;
	        Points[19].y = SelectRect.top;

	        nPoints = 20;

        	lpPoints = ( LPPOINT )LineBuffer[0];
            TInit( &tform );
            nPoints = ExpandPoints( &Points[0], lpPoints, nPoints, 
         	         			    &tform, &lpPoints[0], &lpPoints[ nPoints - 1 ] );

            // draw ellipse
	        MoveToEx( hDC, lpPoints[0].x, lpPoints[0].y, NULL );

    	    for( index = 1; index < nPoints; index++ )
     	        LineTo( hDC, lpPoints[index].x, lpPoints[index].y );

   	        LineTo( hDC, lpPoints[0].x, lpPoints[0].y );

	        SetROP2( hDC, OldROP );
	        break;


	    case SL_CYLINDER:
	    case SL_PCYLINDER:
       	    OldROP = SetROP2( hDC, R2_NOT );
	        hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject( NULL_BRUSH ));

	        Ellipse( hDC, SelectRect.left, SelectRect.top,
		        SelectRect.right, SelectRect.bottom );

	        SetROP2( hDC, OldROP );
	        SelectObject( hDC, hOldBrush );
	        break;
	}

    if( fNeedDC )
	    ReleaseDC( hWnd, hDC );
}


//////////////////////////////////////////////////////////////////////////
void InvertExtrudedSelection( HWND hWnd, HDC hDC, 
                              LPRECT lpRect, LPRECT lpRectNext, int fFlags )
// PURPOSE: Inverts the current extruded selection 
//////////////////////////////////////////////////////////////////////////
{
    HBRUSH hOldBrush;
    short OldROP;
    BOOL fNeedDC;
    int type;
    RECT SelectRect, SelectRectNext;
    static RECT LastSelectRect;
    static int LastFlags;
    BOOL fDoc;
    POINT Apex;
    POINT dist;
    int distance, scale;
    float aspect;
    POINT ptCenter;
    POINT Points[20];
    POINT deltas, mids;
    int nPoints, index;
	LPPOINT lpPoints;
    TFORM tform;
    float LineSlope, VectorSlope;


    if( fFlags && lpRectNext )      // Saved for the display hook
	{                                   
	    LastSelectRect = *lpRectNext;
	    LastFlags = fFlags;
	}
    else                            // Called from the display hook
	    fFlags = LastFlags;

    type = fFlags & SL_TYPE;

    SelectRectNext = LastSelectRect;
    SelectRect = *lpRect;

    if( type == SL_PCUBE || type == SL_PCYLINDER || type == SL_PPYRAMID || type == SL_PWEDGE )
    {
        ptCenter.x = ( SelectRectNext.left + SelectRectNext.right ) / 2;
        ptCenter.y = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

        dist.x = abs( ptCenter.x - (( SelectRect.left + SelectRect.right ) / 2 )); 
        dist.y = abs( ptCenter.y - (( SelectRect.top + SelectRect.bottom ) / 2 ));

        distance  = max( dist.x, dist.y );
        scale = distance * nScale;

         // special case for these two styles when apex points down
        if(( type == SL_PPYRAMID || type == SL_PWEDGE ) &&
            ( SelectRectNext.bottom < SelectRectNext.top ))
        {
            SelectRectNext.left -= 2 * scale / 100; 
            SelectRectNext.top  += 2 * scale / 100; 
        }
        else
        if( type == SL_PCYLINDER )
        {
            // height / width
            aspect = ( float )( SelectRect.bottom - SelectRect.top ) /
                     ( float )( SelectRect.right - SelectRect.left );

            // if vertical
            if(( SelectRect.bottom - SelectRect.top ) <=
               ( SelectRect.right - SelectRect.left ))
            {
                SelectRectNext.left    -= scale / 100; 
                SelectRectNext.top     -= scale * (int)( aspect / 100. ); 
                SelectRectNext.right   += scale / 100; 
                SelectRectNext.bottom  += scale * (int)( aspect / 100. );
            }
            // if horizontal
            else 
            {
                SelectRectNext.left    -= scale / (int)( aspect * 100. ); 
                SelectRectNext.top     -= scale / 100; 
                SelectRectNext.right   += scale / (int)( aspect * 100. ); 
                SelectRectNext.bottom  += scale / 100;
            }
        }
        else
        {
            SelectRectNext.left -= 2 * scale / 100; 
            SelectRectNext.top  -= 2 * scale / 100; 
        }
    }

    if( fDoc = IsDoc( hWnd ))
    {
		File2DispRectExact( hWnd, &SelectRect, &SelectRect );	
		File2DispRectExact( hWnd, &SelectRectNext, &SelectRectNext );	
    }

    if( fNeedDC = ( !hDC ))
	    hDC = GetDC( hWnd );

    switch( type )
    {
	    case SL_CUBE:
	    case SL_PCUBE:
       	    OldROP = SetROP2( hDC, R2_NOT );

            // draw ends
            MoveToEx( hDC, SelectRect.left, SelectRect.top, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.top );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );
	        LineTo( hDC, SelectRect.left, SelectRect.top );

            MoveToEx( hDC, SelectRectNext.left, SelectRectNext.top, NULL );
	        LineTo( hDC, SelectRectNext.right, SelectRectNext.top );
	        LineTo( hDC, SelectRectNext.right, SelectRectNext.bottom );
	        LineTo( hDC, SelectRectNext.left, SelectRectNext.bottom );
	        LineTo( hDC, SelectRectNext.left, SelectRectNext.top );

            // draw sides
            MoveToEx( hDC, SelectRectNext.left, SelectRectNext.top, NULL );
	        LineTo( hDC, SelectRect.left, SelectRect.top );

	        MoveToEx( hDC, SelectRectNext.right, SelectRectNext.top, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.top );

	        MoveToEx( hDC, SelectRectNext.right, SelectRectNext.bottom, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );

	        MoveToEx( hDC, SelectRectNext.left, SelectRectNext.bottom, NULL );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );
	        SetROP2( hDC, OldROP );
	        break;


	    case SL_CYLINDER:
	    case SL_PCYLINDER:
       	    OldROP = SetROP2( hDC, R2_NOT );
	        hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject( NULL_BRUSH ));

	        Ellipse( hDC, SelectRect.left, SelectRect.top,
		        SelectRect.right, SelectRect.bottom );

	        Ellipse( hDC, SelectRectNext.left, SelectRectNext.top,
		        SelectRectNext.right, SelectRectNext.bottom );

            // draw sides
            if(( SelectRect.bottom - SelectRect.top ) <=
               ( SelectRect.right  - SelectRect.left ))
            {
                MoveToEx( hDC, SelectRectNext.left, ( SelectRectNext.top + SelectRectNext.bottom ) / 2, NULL );
                LineTo( hDC, SelectRect.left, ( SelectRect.top + SelectRect.bottom ) / 2 );
                MoveToEx( hDC, SelectRectNext.right, ( SelectRectNext.top + SelectRectNext.bottom ) / 2, NULL );
                LineTo( hDC, SelectRect.right, ( SelectRect.top + SelectRect.bottom ) / 2 );
            }
            else
            {
                MoveToEx( hDC,( SelectRectNext.left + SelectRectNext.right ) / 2, SelectRectNext.top, NULL );
                LineTo( hDC,( SelectRect.left + SelectRect.right ) / 2, SelectRect.top );
                MoveToEx( hDC,( SelectRectNext.left + SelectRectNext.right ) / 2, SelectRectNext.bottom, NULL );
                LineTo( hDC,( SelectRect.left + SelectRect.right ) / 2, SelectRect.bottom );
            }
	        SetROP2( hDC, OldROP );
	        SelectObject( hDC, hOldBrush );
	        break;


	    case SL_PYRAMID:
	    case SL_PPYRAMID:
       	    OldROP = SetROP2( hDC, R2_NOT );

            Apex.x = ( SelectRect.left + SelectRect.right + SelectRectNext.left + SelectRectNext.right ) / 4;
            Apex.y = ( SelectRect.top + SelectRectNext.top ) / 2;

	        MoveToEx( hDC, SelectRect.left, SelectRect.bottom, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );
	        LineTo( hDC, Apex.x, Apex.y );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );

	        MoveToEx( hDC, SelectRectNext.left, SelectRectNext.bottom, NULL );
	        LineTo( hDC, SelectRectNext.right, SelectRectNext.bottom );
	        LineTo( hDC, Apex.x, Apex.y );
	        LineTo( hDC, SelectRectNext.left, SelectRectNext.bottom );

            // draw base
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );
	        MoveToEx( hDC, SelectRect.right, SelectRect.bottom, NULL );
	        LineTo( hDC, SelectRectNext.right, SelectRectNext.bottom );

	        SetROP2( hDC, OldROP );
	        break;


	    case SL_WEDGE:
	    case SL_PWEDGE:
       	    OldROP = SetROP2( hDC, R2_NOT );

            // draw ends
	        MoveToEx( hDC, SelectRect.left, SelectRect.bottom, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );
	        LineTo( hDC, ( SelectRect.left + SelectRect.right ) / 2, SelectRect.top );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );

	        MoveToEx( hDC, SelectRectNext.left, SelectRectNext.bottom, NULL );
	        LineTo( hDC, SelectRectNext.right, SelectRectNext.bottom );
	        LineTo( hDC, ( SelectRectNext.left + SelectRectNext.right ) / 2, SelectRectNext.top );
	        LineTo( hDC, SelectRectNext.left, SelectRectNext.bottom );

            // draw sides
            MoveToEx( hDC, SelectRectNext.left, SelectRectNext.bottom, NULL );
	        LineTo( hDC, SelectRect.left, SelectRect.bottom );
	        MoveToEx( hDC, SelectRectNext.right, SelectRectNext.bottom, NULL );
	        LineTo( hDC, SelectRect.right, SelectRect.bottom );
	        MoveToEx( hDC, ( SelectRectNext.left + SelectRectNext.right ) / 2, SelectRectNext.top, NULL );
	        LineTo( hDC, ( SelectRect.left + SelectRect.right ) / 2, SelectRect.top );

	        SetROP2( hDC, OldROP );
	        break;


	    case SL_HCONE:
       	    OldROP = SetROP2( hDC, R2_NOT );
 	        deltas.x = ( 2 * ( SelectRectNext.right - SelectRectNext.left )) / 9;
	        deltas.y = ( 2 * ( SelectRectNext.bottom - SelectRectNext.top )) / 9;
            mids.x   = ( SelectRectNext.left + SelectRectNext.right ) / 2;
	        mids.y   = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

            // upper right arc
	        Points[0].x = BEZIER_MARKER;
	        Points[0].y = 4;
	        Points[1].x = mids.x;
	        Points[1].y = SelectRectNext.top;
	        Points[2].x = SelectRectNext.right - deltas.x;
	        Points[2].y = SelectRectNext.top;
	        Points[3].x = SelectRectNext.right;
	        Points[3].y = SelectRectNext.top + deltas.y;
	        Points[4].x = SelectRectNext.right;
	        Points[4].y = mids.y;

            // lower right arc
	        Points[5].x = BEZIER_MARKER;
	        Points[5].y = 4;
	        Points[6].x = SelectRectNext.right;
	        Points[6].y = mids.y;
	        Points[7].x = SelectRectNext.right;
	        Points[7].y = SelectRectNext.bottom - deltas.y;
	        Points[8].x = SelectRectNext.right - deltas.x;
	        Points[8].y = SelectRectNext.bottom;
	        Points[9].x = mids.x;
	        Points[9].y = SelectRectNext.bottom;

            // lower left arc
	        Points[10].x = BEZIER_MARKER;
	        Points[10].y = 4;
	        Points[11].x = mids.x;
	        Points[11].y = SelectRectNext.bottom;
	        Points[12].x = SelectRectNext.left + deltas.x;
	        Points[12].y = SelectRectNext.bottom;
	        Points[13].x = SelectRectNext.left;
	        Points[13].y = SelectRectNext.bottom - deltas.y;
	        Points[14].x = SelectRectNext.left;
	        Points[14].y = mids.y;

            // upper left arc
	        Points[15].x = BEZIER_MARKER;
	        Points[15].y = 4;
	        Points[16].x = SelectRectNext.left;
	        Points[16].y = mids.y;
	        Points[17].x = SelectRectNext.left;
	        Points[17].y = SelectRectNext.top + deltas.y;
	        Points[18].x = SelectRectNext.left + deltas.x;
	        Points[18].y = SelectRectNext.top;
	        Points[19].x = mids.x;
	        Points[19].y = SelectRectNext.top;
	        nPoints = 20;

        	lpPoints = ( LPPOINT )LineBuffer[0];
            TInit( &tform );
            nPoints = ExpandPoints( &Points[0], lpPoints, nPoints, 
         	         			    &tform, &lpPoints[0], &lpPoints[ nPoints - 1 ] );

            // draw ellipse
	        MoveToEx( hDC, lpPoints[0].x, lpPoints[0].y, NULL );

    	    for( index = 1; index < nPoints; index++ )
     	        LineTo( hDC, lpPoints[index].x, lpPoints[index].y );

   	        LineTo( hDC, lpPoints[0].x, lpPoints[0].y );

            // if dragged right
            if( SelectRectNext.left >= (( SelectRect.left + SelectRect.right ) / 2 ))
            {
                for( index = ( nPoints / 2 ) - 1; index < 3 * ( nPoints / 4 ) - 1; index++ )
                {
                    if( lpPoints[index].x - lpPoints[index + 1].x )
                    {
                        VectorSlope = ( float )( lpPoints[index + 1].y - lpPoints[index].y ) /
                                      ( float )( lpPoints[index].x - lpPoints[index + 1].x );

                        LineSlope = ( float )((( SelectRect.top + SelectRect.bottom ) / 2 ) - lpPoints[index].y ) / 
                                    ( float )( lpPoints[index].x - (( SelectRect.left + SelectRect.right ) / 2 ));

                        if( VectorSlope < LineSlope )
                        {
                            MoveToEx( hDC, ( SelectRect.left + SelectRect.right ) / 2, ( SelectRect.top + SelectRect.bottom ) / 2, NULL );
                            LineTo( hDC, lpPoints[index].x, lpPoints[index].y );
                            MoveToEx( hDC, ( SelectRect.left + SelectRect.right ) / 2, ( SelectRect.top + SelectRect.bottom ) / 2, NULL );
                            LineTo( hDC, lpPoints[nPoints - ( index + 1 - ( nPoints / 2 ))].x, lpPoints[nPoints - ( index + 1 - ( nPoints / 2 ))].y );

                            break;
                        }
                    }
                }
            }
            else    // if dragged left
            if( SelectRectNext.right <= (( SelectRect.left + SelectRect.right ) / 2 ))
            {
                for( index = 0; index < nPoints / 4 - 1; index++ )
                {
                    if( lpPoints[index+1].x - lpPoints[index].x )
                    {
                        VectorSlope = ( float )( lpPoints[index].y - lpPoints[index+1].y ) /
                                      ( float )( lpPoints[index+1].x - lpPoints[index].x );

                        LineSlope = ( float )( lpPoints[index].y - (( SelectRect.top + SelectRect.bottom ) / 2 )) / 
                                    ( float )((( SelectRect.left + SelectRect.right ) / 2 ) - lpPoints[index].x );

                        if( VectorSlope < LineSlope )
                        {
                            MoveToEx( hDC, ( SelectRect.left + SelectRect.right ) / 2, ( SelectRect.top + SelectRect.bottom ) / 2, NULL );
                            LineTo( hDC, lpPoints[index].x, lpPoints[index].y );
                            MoveToEx( hDC, ( SelectRect.left + SelectRect.right ) / 2, ( SelectRect.top + SelectRect.bottom ) / 2, NULL );
                            LineTo( hDC, lpPoints[( nPoints / 2 - 1 ) - index].x, lpPoints[( nPoints / 2 - 1 ) - index].y );

                            break;
                        }
                    }
                }
            }

	        SetROP2( hDC, OldROP );
	        break;


	    case SL_VCONE:
       	    OldROP = SetROP2( hDC, R2_NOT );

 	        deltas.x = ( 2 * ( SelectRectNext.right - SelectRectNext.left )) / 9;
	        deltas.y = ( 2 * ( SelectRectNext.bottom - SelectRectNext.top )) / 9;
            mids.x   = ( SelectRectNext.left + SelectRectNext.right ) / 2;
	        mids.y   = ( SelectRectNext.top + SelectRectNext.bottom ) / 2;

            // upper right arc
	        Points[0].x = BEZIER_MARKER;
	        Points[0].y = 4;
	        Points[1].x = mids.x;
	        Points[1].y = SelectRectNext.top;
	        Points[2].x = SelectRectNext.right - deltas.x;
	        Points[2].y = SelectRectNext.top;
	        Points[3].x = SelectRectNext.right;
	        Points[3].y = SelectRectNext.top + deltas.y;
	        Points[4].x = SelectRectNext.right;
	        Points[4].y = mids.y;

            // lower right arc
	        Points[5].x = BEZIER_MARKER;
	        Points[5].y = 4;
	        Points[6].x = SelectRectNext.right;
	        Points[6].y = mids.y;
	        Points[7].x = SelectRectNext.right;
	        Points[7].y = SelectRectNext.bottom - deltas.y;
	        Points[8].x = SelectRectNext.right - deltas.x;
	        Points[8].y = SelectRectNext.bottom;
	        Points[9].x = mids.x;
	        Points[9].y = SelectRectNext.bottom;

            // lower left arc
	        Points[10].x = BEZIER_MARKER;
	        Points[10].y = 4;
	        Points[11].x = mids.x;
	        Points[11].y = SelectRectNext.bottom;
	        Points[12].x = SelectRectNext.left + deltas.x;
	        Points[12].y = SelectRectNext.bottom;
	        Points[13].x = SelectRectNext.left;
	        Points[13].y = SelectRectNext.bottom - deltas.y;
	        Points[14].x = SelectRectNext.left;
	        Points[14].y = mids.y;

            // upper left arc
	        Points[15].x = BEZIER_MARKER;
	        Points[15].y = 4;
	        Points[16].x = SelectRectNext.left;
	        Points[16].y = mids.y;
	        Points[17].x = SelectRectNext.left;
	        Points[17].y = SelectRectNext.top + deltas.y;
	        Points[18].x = SelectRectNext.left + deltas.x;
	        Points[18].y = SelectRectNext.top;
	        Points[19].x = mids.x;
	        Points[19].y = SelectRectNext.top;

            nPoints = 20;

        	lpPoints = ( LPPOINT )LineBuffer[0];
            TInit( &tform );
            nPoints = ExpandPoints( &Points[0], lpPoints, nPoints, 
         	         			    &tform, &lpPoints[0], &lpPoints[ nPoints - 1 ] );

	        MoveToEx( hDC, lpPoints[0].x, lpPoints[0].y, NULL );

    	    for( index = 1; index < nPoints; index++ )
     	        LineTo( hDC, lpPoints[index].x, lpPoints[index].y );

   	        LineTo( hDC, lpPoints[0].x, lpPoints[0].y );

            // if dragged down
            if( SelectRectNext.top >= (( SelectRect.top + SelectRect.bottom ) / 2 ))
            {
                for( index = nPoints / 4 - 1 ; index > 0; index-- )
                {
                    if( lpPoints[index].x - lpPoints[index - 1].x )
                    {
                        VectorSlope = ( float )( lpPoints[index - 1].y - lpPoints[index].y ) /
                                      ( float )( lpPoints[index].x - lpPoints[index - 1].x );

                        LineSlope = ( float )((( SelectRect.top + SelectRect.bottom ) / 2 ) - lpPoints[index].y ) / 
                                    ( float )( lpPoints[index].x - (( SelectRect.left + SelectRect.right ) / 2 ));

                        if( VectorSlope > LineSlope )
                        {
                            MoveToEx( hDC, ( SelectRect.left + SelectRect.right ) / 2, ( SelectRect.top + SelectRect.bottom ) / 2, NULL );
                            LineTo( hDC, lpPoints[index].x, lpPoints[index].y );
                            MoveToEx( hDC, ( SelectRect.left + SelectRect.right ) / 2, ( SelectRect.top + SelectRect.bottom ) / 2, NULL );
                            LineTo( hDC, lpPoints[( nPoints - 1 ) - index].x, lpPoints[( nPoints - 1 ) - index].y );

                            break;
                        }
                    }
                }
            }
            else    // if dragged up
            if( SelectRectNext.bottom <= (( SelectRect.top + SelectRect.bottom ) / 2 ))
            {
                for( index = nPoints / 4 - 1 ; index < ( nPoints / 2 ); index++ )
                {
                    if( lpPoints[index].x - lpPoints[index + 1].x )
                    {
                        VectorSlope = ( float )( lpPoints[index + 1].y - lpPoints[index].y ) /
                                      ( float )( lpPoints[index].x - lpPoints[index + 1].x );

                        LineSlope = ( float )((( SelectRect.top + SelectRect.bottom ) / 2 ) - lpPoints[index].y ) / 
                                    ( float )( lpPoints[index].x - (( SelectRect.left + SelectRect.right ) / 2 ));

                        if( VectorSlope < LineSlope )
                        {
                            MoveToEx( hDC, ( SelectRect.left + SelectRect.right ) / 2, ( SelectRect.top + SelectRect.bottom ) / 2, NULL );
                            LineTo( hDC, lpPoints[index].x, lpPoints[index].y );
                            MoveToEx( hDC, ( SelectRect.left + SelectRect.right ) / 2, ( SelectRect.top + SelectRect.bottom ) / 2, NULL );
                            LineTo( hDC, lpPoints[( nPoints - 1 ) - index].x, lpPoints[( nPoints - 1 ) - index].y );

                            break;
                        }
                    }
                }
            }
	        SetROP2( hDC, OldROP );
	        break;
	}
    if( fNeedDC )
	    ReleaseDC( hWnd, hDC );
}


//////////////////////////////////////////////////////////////////////////////
void EndSelection(
// PURPOSE: End selection of region
//////////////////////////////////////////////////////////////////////////////
HWND 	hWnd,
HDC 	hDC,
LPRECT 	lpSelectRect,
int 	fFlags,
BOOL 	bRemove)
{
    int type;
    BOOL fDoc;

    if( bRemove )
	    InvertSelection( hWnd, hDC, lpSelectRect, fFlags );

    if( fDoc = IsDoc( hWnd ))
	    SetDisplayHook( hWnd, lpLastDisplayHook );

    type = fFlags & SL_TYPE;

    if( type != SL_LINE && type != SL_HTRIANGLE && type != SL_VTRIANGLE )
    {
        if( type == SL_WEDGE    || type == SL_PWEDGE    || 
            type == SL_PYRAMID  || type == SL_PPYRAMID  ||
            type == SL_VSEMICIRCLE )
    	   	OrderHPoints( lpSelectRect, lpSelectRect );
        else
        if( type == SL_HSEMICIRCLE )
    	   	OrderVPoints( lpSelectRect, lpSelectRect );
        else
	   	    OrderRect( lpSelectRect, lpSelectRect );
    }

    MessageStatus( NULL );
}


//////////////////////////////////////////////////////////////////////////////
void EndExtrudedSelection(
// PURPOSE: End selection of region
//////////////////////////////////////////////////////////////////////////////
HWND 	hWnd,
HDC 	hDC,
LPRECT 	lpSelectRect,
LPRECT	lpSelectRectNext,
int 	fFlags,
BOOL 	bRemove)
{
    BOOL fDoc;
    int type = fFlags & SL_TYPE;

    if( bRemove )
    	InvertExtrudedSelection( hWnd, hDC, lpSelectRect, lpSelectRectNext, fFlags );

    if( fDoc = IsDoc( hWnd ))
	    SetDisplayHook( hWnd, lpLastDisplayHook );

    MessageStatus( NULL );
}


//////////////////////////////////////////////////////////////////////////////
void OrderRect(
//////////////////////////////////////////////////////////////////////////////
LPRECT	lpSrcRect,
LPRECT	lpDstRect)
{
    int tmp;

    *lpDstRect = *lpSrcRect;

    if( lpDstRect->right < lpDstRect->left )
	{
	    tmp = lpDstRect->right;
	    lpDstRect->right = lpDstRect->left;
	    lpDstRect->left = tmp;
	}

    if( lpDstRect->bottom < lpDstRect->top )
	{
	    tmp = lpDstRect->bottom;
	    lpDstRect->bottom = lpDstRect->top;
	    lpDstRect->top = tmp;
	}
}


//////////////////////////////////////////////////////////////////////////////
void OrderVPoints(
// orders vertical points only
//////////////////////////////////////////////////////////////////////////////
LPRECT	lpSrcRect,
LPRECT	lpDstRect)
{
    int tmp;

    *lpDstRect = *lpSrcRect;

    if( lpDstRect->bottom < lpDstRect->top )
	{
	    tmp = lpDstRect->bottom;
	    lpDstRect->bottom = lpDstRect->top;
	    lpDstRect->top = tmp;
	}
}


//////////////////////////////////////////////////////////////////////////////
void OrderHPoints(
// orders horizontal points only
//////////////////////////////////////////////////////////////////////////////
LPRECT	lpSrcRect,
LPRECT	lpDstRect)
{
    int tmp;

    *lpDstRect = *lpSrcRect;

    if( lpDstRect->right < lpDstRect->left )
	{
	    tmp = lpDstRect->right;
	    lpDstRect->right = lpDstRect->left;
	    lpDstRect->left = tmp;
	}
}


//////////////////////////////////////////////////////////////////////////////
void ConstrainSelection(
//	Constrain the selection to the AspectX&AspectY.
//	If bCenter then it will try to keep the center constant.
//	Otherwise it will keep the upper left corner constant.
//////////////////////////////////////////////////////////////////////////////
LPRECT 	lpRect,
long	AspectX,
long	AspectY,
BOOL 	bCenter)
{
	int dx, dy, x, y;
	POINT End, Start;
	
	if ( !AspectX || !AspectY )
		return;

	Start.x = lpRect->left;
	Start.y = lpRect->top;
	End.x = lpRect->right;
	End.y = lpRect->bottom;

	x = End.x - Start.x + 1;
	y = End.y - Start.y + 1;

	ScaleToFit( &x, &y, ( int )AspectX, ( int )AspectY );

	if( bCenter )
	{
		dx = ( x-End.x+Start.x+1 ) / 2;
		dy = ( y-End.y+Start.y+1 ) / 2;

		lpRect->left -= dx;
		lpRect->top -= dy;
		lpRect->right = lpRect->left+x-1;
		lpRect->bottom =lpRect->top+y-1;
	}
	else 
	{
		lpRect->right = Start.x+x-1;
		lpRect->bottom = Start.y+y-1;
	}
}


//////////////////////////////////////////////////////////////////////////////
VOID ConstrainHandles(
//////////////////////////////////////////////////////////////////////////////
POINT 	ptCurrent,
LPRECT 	lpSelectRect)
{
    int tmp;

    if( abs ( ptCurrent.y - lpSelectRect->bottom ) <= CLOSENESS )
    {
	    if( abs ( ptCurrent.x - lpSelectRect->right ) <= CLOSENESS )
		    Constrain = 0;
	    else
	    if( abs ( ptCurrent.x - lpSelectRect->left ) <= CLOSENESS )
	    {
		    Constrain = 0;
		    tmp = lpSelectRect->left;
		    lpSelectRect->left = lpSelectRect->right;
		    lpSelectRect->right = tmp;
	    }
	    else
	    if( abs ( ptCurrent.x - (( lpSelectRect->left + lpSelectRect->right+1 ) / 2 )) <= CLOSENESS )
		    Constrain ^= CONSTRAINY;
	}
    else
    if( abs ( ptCurrent.y - lpSelectRect->top ) <= CLOSENESS )
	{
	    if( abs ( ptCurrent.x - lpSelectRect->right ) <= CLOSENESS )
		    Constrain = 0;
	    else
	    if( abs ( ptCurrent.x - lpSelectRect->left ) <= CLOSENESS )
	    {
		    Constrain = 0;
		    tmp = lpSelectRect->left;
		    lpSelectRect->left = lpSelectRect->right;
		    lpSelectRect->right = tmp;
	    }
	    else
	    if( abs ( ptCurrent.x - (( lpSelectRect->left + lpSelectRect->right+1 ) / 2 )) <= CLOSENESS )
		    Constrain ^= CONSTRAINY;

	    if( Constrain != ( CONSTRAINY | CONSTRAINX ))
	    {
		    tmp = lpSelectRect->top;
		    lpSelectRect->top = lpSelectRect->bottom;
		    lpSelectRect->bottom = tmp;
	    }
    }
    else
    if( abs ( ptCurrent.x - lpSelectRect->right ) <= CLOSENESS )
    {
	    if( abs ( ptCurrent.y - (( lpSelectRect->top + lpSelectRect->bottom+1 ) / 2 )) <= CLOSENESS )
		    Constrain ^= CONSTRAINX;
    }
    else
    if( abs ( ptCurrent.x - lpSelectRect->left ) <= CLOSENESS )
    {
	    if( abs ( ptCurrent.y - (( lpSelectRect->top + lpSelectRect->bottom+1 ) / 2 )) <= CLOSENESS )
	    {
		    Constrain ^= CONSTRAINX;
		    tmp = lpSelectRect->left;
		    lpSelectRect->left = lpSelectRect->right;
		    lpSelectRect->right = tmp;
	    }
    }
}


//////////////////////////////////////////////////////////////////////////////
static void DrawHandles( HDC hDC, LPRECT lpSelectRect )
//////////////////////////////////////////////////////////////////////////////
{
    RECT hRect, sRect;
    HPEN hOldPen;

    hOldPen = (HPEN)SelectObject( hDC, GetStockObject( BLACK_PEN ));

    sRect.top = min( lpSelectRect->top, lpSelectRect->bottom );
    sRect.bottom = max( lpSelectRect->top, lpSelectRect->bottom );
    sRect.left = min( lpSelectRect->left, lpSelectRect->right );
    sRect.right = max( lpSelectRect->left, lpSelectRect->right );

    hRect.top = sRect.top - HSIZE;
    hRect.bottom = sRect.top + HSIZE + 1;
    hRect.left = sRect.left - HSIZE;
    hRect.right = sRect.left + HSIZE + 1;

    PatBlt( hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		    hRect.bottom-hRect.top, DSTINVERT );

    hRect.top = sRect.top - HSIZE;
    hRect.bottom = sRect.top + HSIZE + 1;
    hRect.left = sRect.right - HSIZE;
    hRect.right = sRect.right + HSIZE + 1;

    PatBlt( hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		    hRect.bottom-hRect.top, DSTINVERT );

    hRect.top = sRect.bottom - HSIZE;
    hRect.bottom = sRect.bottom + HSIZE + 1;
    hRect.left = sRect.right - HSIZE;
    hRect.right = sRect.right + HSIZE + 1;

    PatBlt( hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		    hRect.bottom-hRect.top, DSTINVERT );

    hRect.top = sRect.bottom - HSIZE;
    hRect.bottom = sRect.bottom + HSIZE + 1;
    hRect.left = sRect.left - HSIZE;
    hRect.right = sRect.left + HSIZE + 1;

    PatBlt( hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		    hRect.bottom-hRect.top, DSTINVERT );

    hRect.top = (( sRect.top+sRect.bottom+1 ) / 2 ) - HSIZE;
    hRect.bottom = (( sRect.top+sRect.bottom+1 ) / 2 ) + HSIZE + 1;
    hRect.left = sRect.left - HSIZE;
    hRect.right = sRect.left + HSIZE + 1;

    PatBlt( hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		    hRect.bottom-hRect.top, DSTINVERT );

    hRect.top = sRect.top - HSIZE;
    hRect.bottom = sRect.top + HSIZE + 1;
    hRect.left = (( sRect.left+sRect.right+1 ) / 2 ) - HSIZE;
    hRect.right = (( sRect.left+sRect.right+1 ) / 2 ) + HSIZE + 1;

    PatBlt( hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		    hRect.bottom-hRect.top, DSTINVERT );

    hRect.top = (( sRect.top+sRect.bottom+1 ) / 2 ) - HSIZE;
    hRect.bottom = (( sRect.top+sRect.bottom+1 ) / 2 ) + HSIZE + 1;
    hRect.left = sRect.right - HSIZE;
    hRect.right = sRect.right + HSIZE + 1;

    PatBlt( hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		    hRect.bottom-hRect.top, DSTINVERT );

    hRect.top = sRect.bottom - HSIZE;
    hRect.bottom = sRect.bottom + HSIZE + 1;
    hRect.left = (( sRect.left+sRect.right+1 ) / 2 ) - HSIZE;
    hRect.right = (( sRect.left+sRect.right+1 ) / 2 ) + HSIZE + 1;

    PatBlt( hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		    hRect.bottom-hRect.top, DSTINVERT );

    SelectObject( hDC, hOldPen );
}


//////////////////////////////////////////////////////////////////////////////
static void DisplayHook( HDC hDC, LPRECT lpRect )
//////////////////////////////////////////////////////////////////////////////
{
    if( !lpImage )
	    return;

    InvertSelection( lpImage->hWnd, hDC, NULL/*&SelectRect*/, NULL/*Flags*/ );

    if( lpLastDisplayHook )
	    ( *lpLastDisplayHook )( hDC, lpRect );
}


//////////////////////////////////////////////////////////////////////////////
static LPRECT GetBoundingRect( HWND hWnd )
//////////////////////////////////////////////////////////////////////////////
{
    static RECT BoundRect;

    if( lpImage && ( lpImage->hWnd == hWnd ))
    {
	    BoundRect.left = BoundRect.top = 0;
	    BoundRect.right = lpImage->npix-1;
	    BoundRect.bottom = lpImage->nlin-1;
    }
    else
	    GetClientRect( hWnd, &BoundRect );

    return( &BoundRect );
}
