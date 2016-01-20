// (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// mapctl.c for map control
#include <math.h>
#include "ctlutil.h"
#include "mapctl.h"
#include "macros.h"
#include "ctltools.h"

// WINDOW WORDS/LONGS
// POINTER TO MAP POINTS MEMORY
#define GWL_MAPSTRUCT	(GWW_START+0)			
// Number of extra memory for window words
#define MAP_EXTRA	(GWW_START+4)

#define NO_ACTIVE -1

#define SMALL_MOVEMENT 3
#define HANDLE_SIZE 3

// range of input & output values
#define MAXVAL 	255
#define MINVAL	0

// numer of elements in the final map - 1.
#define MAXMAP 255

// for intrpolation
#define SSIZE 11

// struct allocated for every map control
typedef struct _mapctl
{						    
	DWORD dwStyle;		// Copy of window style for convienence
	int MaxPoints;		// The maximum number of points to allow
	LPPOINT lpPoints;	// The point list (must always be at least MaxPoints)
	int NumPoints;		// The number of 'valid' points;
	int active;			// The active point (can be NO_ACTIVE)
	POINT ptProbe;		// The currently displayed probe point (if any)
	POINT ptBrowse;		// The last point browsed (if any)
	double gamma;		// apply this gamma when displaying
	int gridX;			// how often to draw vert lines (0 for none)
	int gridY;			// how often to draw horiz lines (0 for none)
	RGBS gridRGB;		// color of grid.
	RECT rArea;			// Client rect of active area of control.
 	BOOL bConstrainX;	// constrain whne moving point
	BOOL bConstrainY;	// constrain whne moving point
	POINT ptConstrain;	// the point to constrain to.
	BYTE Map[MAXMAP+1];		// the resultant map.
	int CurveType;		// Smoothness of curve (if using curves)
}MAPCTL, FAR * LPMAPCTL;

#define DRAW_WIDTH(lpmap) \
	(RectWidth(&lpmap->rArea) - 2*(HANDLE_SIZE+1))
#define DRAW_HEIGHT(lpmap) \
	(RectHeight(&lpmap->rArea) - 2*(HANDLE_SIZE+1))
#define DRAW_LEFT(lpmap) \
	(lpmap->rArea.left + HANDLE_SIZE)
#define DRAW_RIGHT(lpmap) \
	(lpmap->rArea.right - HANDLE_SIZE-1)
#define DRAW_BOTTOM(lpmap) \
	(lpmap->rArea.bottom - HANDLE_SIZE)
#define DRAW_TOP(lpmap) \
	(lpmap->rArea.top + HANDLE_SIZE+1)

// helper macro
#define GET_LPMAPCTL(hMap) \
	(hMap ? (LPMAPCTL)GetWindowLong(hMap,GWL_MAPSTRUCT) : NULL)

#define POINT_CHANGING(hWindow) \
	(FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, \
		MN_POINTCHANGING, SendMessage ))

#define POINT_CHANGED(hWindow) \
	(FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, \
		MN_POINTCHANGED, SendMessage ))

#define POINT_BROWSE(hWindow) \
	(FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, \
		MN_BROWSE, SendMessage ))

#define GAMMA(x, y, gamma, bInv) \
	(bInv ? (255-(int)(pow((double)(255-y)/255.0,gamma)*255.0 +0.5)):(int)(pow((double)(y)/255.0,gamma)*255.0 +0.5))
#define INVGAMMA(x, y, gamma, bInv) \
	(bInv ? (255-(int)(pow((double)(255-y)/255.0,1.0/gamma)*255.0 +0.5)):(int)(pow((double)(y)/255.0,1.0/gamma)*255.0 +0.5))

#define BOUND(x,a,b) max(min(b,x),a)
#define WITHIN(val,lower,upper) \
	( (val) < (lower) ? 0 : ( (val) > (upper) ? 0 : 1 ) )


LOCAL void DrawLOCALBox( HDC hDC, LPRECT lpRect);

LOCAL BOOL map_add_point(HWND hWindow, LPMAPCTL lpmap, LPPOINT lpAdd);
LOCAL BOOL delete_point(LPMAPCTL lpmap, HWND hWindow, int index);
LOCAL BOOL map_move_point(LPMAPCTL lpmap, HWND hWindow, int index, LPPOINT lpNewPoint);
LOCAL VOID map_draw_grid(LPMAPCTL lpmap, HDC hDC, LPRECT lpArea);
LOCAL void map_draw_range(LPMAPCTL lpmap, HDC hDC, LPRECT lpArea, int first, int last, BOOL bOn, LPTR lpData = NULL);
LOCAL VOID map_draw_handles(LPMAPCTL lpmap, HDC hDC, LPRECT lpArea, BOOL bOn);
LOCAL VOID map_draw_handle(LPMAPCTL lpmap, HDC hDC, int index, BOOL bActive, BOOL bOn);
LOCAL VOID map_draw_probe(LPMAPCTL lpmap, HDC hDC, BOOL bOn);
LOCAL VOID map_compute(LPMAPCTL lpmap);
LOCAL VOID map_compute_curves(LPMAPCTL lpmap);
LOCAL VOID map_from_screen(LPMAPCTL lpmap, LPPOINT lpConvert, BOOL bGamma);
LOCAL HDC map_get_dc(HWND hWindow, LPMAPCTL lpmap);
LOCAL VOID map_done_dc(HWND hWindow, HDC hDC, BOOL bOk2Free);
LOCAL int map_close_point(LPMAPCTL lpmap, LPPOINT lpTest, int distx, int disty);
LOCAL BOOL map_close_line(LPMAPCTL lpmap, LPPOINT lpTest, int disty);
LOCAL BOOL map_SelectClipRect(HDC hDC, LPRECT lpNewRect, LPRECT lpOldRect );
LOCAL VOID map_init_mapctl(LPMAPCTL lpmap, HWND hMap);

//************************************************************************
//************************************************************************
//	MAP Access Functions
//************************************************************************
//************************************************************************


//************************************************************************
//	Map_Init
//
//	PARAMETERS:
//				int gridX- vertical grid spacing (0 for none, -1 for NoChange).
//				int gridY- horiz grid spacing (0 for none, -1 for NC).
//				double gamma- gamma used on display (-1.0 for NC).
//				int MaxPoints- the most points which can be in the map (-1 for NC).
//				WORD wStyles- any styles to set(-1 for NC).
//	DESCRIPTION: 
//			Sets the above parameters and redraws the control.
//	RETURNS: 
//		   	FALSE if allocation fails. (caller should put up memory error)
//************************************************************************
BOOL Map_Init(HWND hMap, int gridX, int gridY, double gamma, int MaxPoints, WORD wStyles)
//************************************************************************
{
	LPMAPCTL lpmap;
	LPPOINT lpOld;
	DWORD dwStyles;
	
	if (!(lpmap = GET_LPMAPCTL(hMap)))
	{
		lpmap = (LPMAPCTL)Alloc(sizeof(MAPCTL));
		if (!lpmap)
			return(FALSE);
		map_init_mapctl(lpmap, hMap);
		SetWindowLong( hMap, GWL_MAPSTRUCT, (long)lpmap);
	}
	if (MaxPoints >= 0)
	{
		if (MaxPoints > lpmap->MaxPoints)
		{
			lpOld = lpmap->lpPoints;
			lpmap->lpPoints = (LPPOINT)Alloc(MaxPoints*sizeof(POINT));
			if (lpOld && lpmap->lpPoints)
				copy ((LPTR)lpOld, (LPTR)lpmap->lpPoints, lpmap->NumPoints*sizeof(POINT));
			if (lpOld)
				FreeUp((LPTR)lpOld);
		}
	
		if (lpmap->lpPoints)
			lpmap->MaxPoints = MaxPoints;		// The maximum number of points to allow
		else
		{
			lpmap->MaxPoints = 0;		// The maximum number of points to allow
 			lpmap->NumPoints = 0;
		}
	}
	if (gamma >= 0)
		lpmap->gamma = gamma;			// apply this gamma when displaying
	if (gridX >= 0)
		lpmap->gridX = gridX;			// how often to draw vert lines (0 for none)
	if (gridY >= 0)
		lpmap->gridY = gridY;			// how often to draw horiz lines (0 for none)
	if (wStyles != -1)
	{
		dwStyles = GetWindowLong(hMap, GWL_STYLE);
		dwStyles &= 0xffff0000;
		dwStyles |= wStyles;
		SetWindowLong(hMap, GWL_STYLE, dwStyles);
		lpmap->dwStyle = dwStyles;
	}
	// draw it!
	map_compute(lpmap);
    InvalidateRect (hMap,NULL,FALSE);
    UpdateWindow (hMap); 
	return(TRUE);
}


//************************************************************************
//		Map_SetCurveType
// DESCRIPTION:
//		Sets the smoothness of the curve interpolation.
//************************************************************************
void Map_SetCurveType(HWND hMap, int CurveType)
//************************************************************************
{
	LPMAPCTL lpmap;
	
	if (!(lpmap = GET_LPMAPCTL(hMap)))
		return;
	lpmap->CurveType = Bound(CurveType,SMOOTH_EXSOFT,SMOOTH_EXHARD); 
	// draw it!
	map_compute(lpmap);
    InvalidateRect (hMap,NULL,FALSE);
    UpdateWindow(hMap); 
}


//***********************************************************************
//	Map_SetStyle
//
//	PARAMETERS:
//				DWORD dwStyle- The style bits to set or clear.
//				BOOL bOn- TRUE to set, FALSE to clear.
//	DESCRIPTION:
//			Sets or clears the given style bit(s). Redraws if needed.
//************************************************************************
void Map_SetStyle(HWND hMap, WORD wStyle, BOOL bOn)
//************************************************************************
{
	LPMAPCTL lpmap;
	DWORD dwStyles;

	if (!(lpmap = GET_LPMAPCTL(hMap)))
		return;
	dwStyles = GetWindowLong(hMap, GWL_STYLE);
	if (bOn)
		dwStyles |= wStyle;
	else
		dwStyles &= ~(LONG)wStyle;
	SetWindowLong(hMap, GWL_STYLE, dwStyles);
	lpmap->dwStyle = dwStyles;
	// draw it!
	map_compute(lpmap);
    InvalidateRect (hMap,NULL,FALSE);
    UpdateWindow(hMap); 
}


//***********************************************************************
//	Map_GetStyle
//
//	RETURNS: 
//			The style bits for the control.
//************************************************************************
WORD Map_GetStyle(HWND hMap)
//************************************************************************
{
	DWORD dwStyles;
	
	dwStyles = GetWindowLong(hMap, GWL_STYLE);
	return((WORD)dwStyles);
}


//***********************************************************************
//	Map_SetGamma
//
// PARAMETERS:
//				double gamma- gamma used on display.
// DESCRIPTION:
//			Sets gamma and redraws if needed.
//************************************************************************
void Map_SetGamma(HWND hMap, double gamma)
//************************************************************************
{
	HDC hDC;
	LPMAPCTL lpmap;
	
	if (!(lpmap = GET_LPMAPCTL(hMap)))
		return;
	hDC = map_get_dc(hMap, lpmap);
	
	// Undraw 
	map_draw_range(lpmap, hDC, &lpmap->rArea, 0, MAXMAP, OFF);
	
	lpmap->gamma = gamma;
	
	// redraw
	map_compute(lpmap);
	map_draw_range(lpmap, hDC, &lpmap->rArea, 0, MAXMAP, ON);
	map_done_dc(hMap, hDC, YES /*Free DC?*/);
}


//***********************************************************************
//	Map_GetPointList
//
// PARAMETERS:
//				LPPOINT lpResult- The list to fill.
//				int MaxPoints- The size of the list to fill.
//				BOOL bBound- Ensure points are in range?
// DESCRIPTION:
//			Fills first MaxPoint entries in lpResult.  Any unused entries 
//			are set to 0,0. Id !bBound then the raw data is output.
// RETURNS:
//			The number of valid points.
//************************************************************************
int Map_GetPointList(HWND hMap, LPPOINT lpResult, int MaxPoints, BOOL bBound)
//************************************************************************
{
	int num, i, iout;
	LPMAPCTL lpmap;

	if (!(lpmap = GET_LPMAPCTL(hMap)))
		return(0);

	num = min(MaxPoints, lpmap->NumPoints);
	// copy points
	iout = 0;
	for (i=0; i<num; i++)
	{
		if (!bBound)
			lpResult[iout] = lpmap->lpPoints[i];
		else
		{
			// bound
			lpResult[iout].x = BOUND(lpmap->lpPoints[i].x, MINVAL, MAXVAL);
			lpResult[iout].y = BOUND(lpmap->lpPoints[i].y, MINVAL, MAXVAL);
			if (iout>0)
			{
				// abort after hitting the end
				if (lpResult[iout].x == MAXVAL && lpResult[iout-1].x == MAXVAL)
					break;
				// ensure no overlap
				if (lpResult[iout].x <= lpResult[iout-1].x)
					continue;
			}
		}
		iout++;
	}
	num = iout;
	// clear extra space
	for (;iout<MaxPoints;iout++)
		lpResult[iout].x = lpResult[iout].y = 0;		
	return (num);
}


//***********************************************************************
//	Map_SetPointList
//
// PARAMETERS:
//				LPPOINT lpPoints- the new point list. (can be NULL)
//				int nPoints- the number of points in lpPoints.
// DESCRIPTION:
//			Sets/Resets the point list. If npoints > MaxPoints then 
//			MaxPoints is increased. The data is copied.
//			Redraws if needed.
// RETURNS:
//			FALSE if there is a memory error.(caller should put up message)
//************************************************************************
BOOL Map_SetPointList(HWND hMap, LPPOINT lpPoints, int nPoints)
//************************************************************************
{
	HDC hDC;
	int i, num;
	LPMAPCTL lpmap;
	
	if (!(lpmap = GET_LPMAPCTL(hMap)))
		return(FALSE);
	hDC = map_get_dc(hMap, lpmap);
	
	// Undraw 
	map_draw_range( lpmap, hDC, &lpmap->rArea, 0, MAXMAP, OFF);
	
	// make sure there is enough memory
	num = max(lpmap->MaxPoints, nPoints);
	if (num > lpmap->MaxPoints || !lpmap->lpPoints)
	{
		if (lpmap->lpPoints)
			FreeUp((LPTR)lpmap->lpPoints);
		lpmap->lpPoints = (LPPOINT)Alloc(num*sizeof(POINT));
		if (!lpmap->lpPoints)
			return(FALSE);
	}
	// copy data
	for (i=0; i<nPoints; i++)
		lpmap->lpPoints[i] = lpPoints[i];
	lpmap->NumPoints = nPoints;
	// fill any extra
	for(;i<num;i++)
		lpmap->lpPoints[i].x = lpmap->lpPoints[i].y = 0;		
	
	// redraw
	map_compute(lpmap);
	map_draw_range( lpmap, hDC, &lpmap->rArea, 0, MAXMAP, ON);
	map_done_dc(hMap, hDC, YES /*Free DC?*/);
	return(TRUE);
}



//***********************************************************************
//	Map_SetProbe
//
// PARAMETERS:
//				int x- the x position at which to put probe.
// DESCRIPTION:
//			Draws a 'probe' at the given x value. Undraws any previous probe
// RETURNS:
//			The Y value associated with x.
//************************************************************************
int Map_SetProbe(HWND hMap, int x)
//************************************************************************
{
	LPMAPCTL lpmap;
	HDC hDC;

	if (!(lpmap = GET_LPMAPCTL(hMap)))
		return(0);
	hDC = map_get_dc(hMap, lpmap);
	map_draw_probe(lpmap, hDC, OFF);
	lpmap->ptProbe.x = BOUND(x, MINVAL, MAXVAL);
	lpmap->ptProbe.y = lpmap->Map[x];
	map_draw_probe(lpmap, hDC, ON);
	map_done_dc(hMap, hDC, YES /*Free DC?*/);
	return(lpmap->ptProbe.y);
}


//***********************************************************************
//	Map_ClearProbe
//
// DESCRIPTION:	
//			Undraws the probe.
//************************************************************************
void Map_ClearProbe(HWND hMap)
//************************************************************************
{
	LPMAPCTL lpmap;
	HDC hDC;

	if (!(lpmap = GET_LPMAPCTL(hMap)))
		return;
	if (lpmap->ptProbe.x == INT_MAX)
		return;
	hDC = map_get_dc(hMap, lpmap);
	map_draw_probe(lpmap, hDC, OFF);
	lpmap->ptProbe.x = INT_MAX;
	map_done_dc(hMap, hDC, YES /*Free DC?*/);
	return;
}


//***********************************************************************
//	Map_AddPoint
//
// PARAMETERS:
//				POINT pt- the point to add.
// DESCRIPTION:
//			Adds the given point to the map (after applying invert gamma). 
//			the new point is the active point. Redraws map.
// RETURNS:
//			FALSE if there are already MaxPoints. (will put up message)
//************************************************************************
BOOL Map_AddPoint(HWND hMap, LPPOINT lpPoint)
//************************************************************************
{
	LPMAPCTL lpmap;
	POINT pt;

	if (!(lpmap = GET_LPMAPCTL(hMap)))
		return(FALSE);
	pt.x = lpPoint->x;
	pt.y = INVGAMMA(lpPoint->x, lpPoint->y, lpmap->gamma, (lpmap->dwStyle & MS_INVERT_GAMMA));
	return(map_add_point(hMap, lpmap, &pt));
}


//***********************************************************************
//	Map_GetActivePoint
//
// PARAMETERS:
//				LPPOINT lpResult- The position of the active point. (can be NULL)
// DESCRIPTION:
//			Sets lpResult to the pos of the active point (if any).
//			The result is gamma adjusted!
// RETURNS:
//			FALSE if there was no active point.
//************************************************************************
BOOL Map_GetActivePoint(HWND hMap, LPPOINT lpResult)
//************************************************************************
{
	LPMAPCTL lpmap;

	if (!(lpmap = GET_LPMAPCTL(hMap)))
		return(FALSE);
	if (!WITHIN(lpmap->active, 0, lpmap->NumPoints-1))
		return(FALSE);
	if (lpResult)
	{
		*lpResult = lpmap->lpPoints[lpmap->active];
		lpResult->y = GAMMA(lpResult->x, lpResult->y, lpmap->gamma, (lpmap->dwStyle & MS_INVERT_GAMMA));
		if (!WITHIN(lpResult->y, 0, MAXVAL))
			lpResult->y = BOUND(lpResult->y, 0, MAXVAL);
	}
	return(TRUE);
}

//***********************************************************************
//	Map_GetBrowsePoint
//
// PARAMETERS:
//				LPPOINT lpResult- The position of the last browsed point. (can be NULL)
// DESCRIPTION:
//			Sets lpResult to the pos of the last browsed point (if any).
// RETURNS:
//			FALSE if there was no browsed point.
//************************************************************************
BOOL Map_GetBrowsePoint(HWND hMap, LPPOINT lpResult)
//************************************************************************
{
	LPMAPCTL lpmap;

	if (!(lpmap = GET_LPMAPCTL(hMap)))
		return(FALSE);
	if (lpmap->ptBrowse.x == INT_MAX)
		return(FALSE);
	if (lpResult)
		*lpResult = lpmap->ptBrowse;
	return(TRUE);
}

BOOL Map_RegisterClass (HINSTANCE hInstance, UINT ClassScope)
{
	WNDCLASS WndClass;

	/* Setup and define the window class labeled frame control */
	/* Setup and define the window class for a joystick */
	WndClass.hIcon		= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPTSTR)_T("map");
	WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
	WndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	WndClass.hInstance	= hInstance;
	WndClass.style		= CS_DBLCLKS | ClassScope;
	WndClass.lpfnWndProc	= Map_Control;
	WndClass.cbClsExtra	= 0;
	WndClass.cbWndExtra	= MAP_EXTRA;

	return(RegisterClass( &WndClass ));
}

BOOL Map_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)_T("map"), hInstance));
}

//***********************************************************************
//***********************************************************************
// MAP CONTROL
//***********************************************************************
//***********************************************************************

LOCAL BOOL Map_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
LOCAL void Map_OnPaint(HWND hWindow);
LOCAL BOOL Map_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Map_OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Map_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Map_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Map_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Map_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Map_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Map_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Map_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void Map_OnDestroy(HWND hWindow);
LOCAL BOOL Map_Draw( HDC hDC, LPRECT lpRect, LPPOINT lpLastPoints, LPPOINT lpPoints, int nPoints, int idPointToChange, POINT Point, BOOL bDrawHandles);
LOCAL int Map_ClosestPoint( LPPOINT lpPoints, int nPoints, POINT Point, int iSmallMovement );

/***********************************************************************/
LONG WINPROC EXPORT Map_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
		HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);

		HANDLE_MSG(hWindow, WM_CREATE, Map_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Map_OnDestroy);
		HANDLE_MSG(hWindow, WM_PAINT, Map_OnPaint);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Map_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Map_OnLButtonUp);
		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Map_OnLButtonDblClk);
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Map_OnMouseMove);
		HANDLE_MSG(hWindow, WM_KEYDOWN, Map_OnKeyDown);
		HANDLE_MSG(hWindow, WM_SETFOCUS, Map_OnSetFocus);
		HANDLE_MSG(hWindow, WM_KILLFOCUS, Map_OnKillFocus);

		default:
		return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL Map_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	DWORD dwStyle;
	LPMAPCTL lpmap;

	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	lpmap = (LPMAPCTL)Alloc(sizeof(MAPCTL));
	if (lpmap)
		map_init_mapctl(lpmap, hWindow);
	SetWindowLong( hWindow, GWL_MAPSTRUCT, (long)lpmap);
	dwStyle = GetWindowLong(hWindow,GWL_MAPSTRUCT);
	map_compute(lpmap);
	return(TRUE);
}

/***********************************************************************/
LOCAL void Map_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPMAPCTL lpmap;

	if (lpmap = GET_LPMAPCTL(hWindow))
	{
		if (lpmap->lpPoints)
			FreeUp( (LPTR)lpmap->lpPoints );
		FreeUp((LPTR)lpmap);
		SetWindowLong( hWindow, GWL_MAPSTRUCT, (long)NULL);
	}
	Control_OnDestroy(hWindow);
}

/***********************************************************************/
LOCAL void Map_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC         hDC;
	RECT        ClientRect;
	LPMAPCTL 	lpmap;

	hDC = BeginPaint( hWindow, &ps );
	if (!(lpmap = GET_LPMAPCTL(hWindow)))
	   	goto Done;
	
	GetClientRect(hWindow, &ClientRect);
	InflateRect( &ClientRect, -1, -1 );
	// skip the focus rect for now
	// Draw the focus rectangle
	//	bHasFocus = ( GetFocus() == hWindow );
	//	FrameRect( hDC, &ClientRect,
	//		GetStockObject( bHasFocus ? BLACK_BRUSH : WHITE_BRUSH ) );
	//	InflateRect( &ClientRect, -1, -1 );

	// Draw the interior and exterior box frames
	FillRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(WHITE_BRUSH) );
	FrameRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(BLACK_BRUSH) ); 
	InflateRect(&ClientRect, -1, -1);

	// Draw the grid
	map_draw_grid(lpmap, hDC, &ClientRect);

	// Draw the lines
	map_draw_range( lpmap, hDC, &ClientRect, 0, MAXMAP, ON);
	
//	GrayWindow( hDC, hWindow, WS_NOTENABLED );
Done:
	EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Map_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	BOOL bDrawHandles, bDel;
	POINT Point;
	LONG distx, disty;
	HDC hDC;
	int iPointHit;
	LPMAPCTL lpmap;


	if ( Control_bTrack )
		return;
	if (!(lpmap = GET_LPMAPCTL(hWindow)))
		return;
	
		
	bDrawHandles = !(lpmap->dwStyle & MS_NOHANDLES);
	bDel = (keyFlags & MK_SHIFT);
	
	// prepare for constrain
	Point.x = x; Point.y = y;
	lpmap->ptConstrain = Point;
	lpmap->bConstrainX = lpmap->bConstrainY = FALSE;
	
	// convert x,y to map coords
	map_from_screen(lpmap, &Point, NO);
	
	// find 'small' movement
   	distx = (SMALL_MOVEMENT * MAXVAL)/RectWidth(&lpmap->rArea);
   	disty = (SMALL_MOVEMENT * MAXVAL)/RectHeight(&lpmap->rArea);
	
	// find newly enabled (if any)
	iPointHit = map_close_point(lpmap, &Point, distx, disty);
	if (iPointHit == NO_ACTIVE && !bDel)
	{
		// add point?
		if (map_close_line(lpmap, &Point, disty))
		{
			// add point and make it active
			Point.y = INVGAMMA(Point.x, Point.y, lpmap->gamma, (lpmap->dwStyle & MS_INVERT_GAMMA));
			if (map_add_point(hWindow, lpmap, &Point))
				iPointHit = lpmap->active;
			goto Capture;
		}
	}
	
	hDC = map_get_dc(hWindow, lpmap);
	// deselect current point
	if (lpmap->active != NO_ACTIVE && bDrawHandles)
	{
		map_draw_handle(lpmap, hDC, lpmap->active, YES /*active*/, OFF);
		map_draw_handle(lpmap, hDC, lpmap->active, NO /*active*/, ON);
	}
	
	if (bDel && iPointHit != NO_ACTIVE)
	{
		// delete the point
		lpmap->active = NO_ACTIVE;
		delete_point(lpmap, hWindow, iPointHit);
	}
	else
	{
		// select the point
		lpmap->active = iPointHit;
	
		if (lpmap->active != NO_ACTIVE && bDrawHandles)
		{
			map_draw_handle(lpmap, hDC, lpmap->active, NO /*active*/, OFF);
			map_draw_handle(lpmap, hDC, lpmap->active, YES /*active*/, ON);
		}
	}
		
Capture:
	// Set stuff needed during tracking
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	if (!bDel)
	{
		SetCapture( hWindow ); Control_bTrack = TRUE;
		Control_shDC = hDC;
	}
	// hDC relaesd by Control_LButtonUp!
	map_done_dc(hWindow, hDC, NO /*Free DC?*/);
	if (iPointHit != NO_ACTIVE)
	{
		lpmap->ptBrowse.x = INT_MAX;
		POINT_CHANGING(hWindow);
	}
	else
	{
		lpmap->ptBrowse.x = x;
		lpmap->ptBrowse.y = y;
		map_from_screen(lpmap, &lpmap->ptBrowse, NO);
		POINT_BROWSE(hWindow);
	}
}

/***********************************************************************/
LOCAL void Map_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
}

/***********************************************************************/
LOCAL void Map_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	LPMAPCTL lpmap;
	
	if (!(lpmap = GET_LPMAPCTL(hWindow)))
		return;
		
	// end browse
	if (lpmap->active == NO_ACTIVE)
	{
		lpmap->ptBrowse.x = INT_MAX;
		POINT_BROWSE(hWindow);
	}
	else if (Control_bTrack )
		POINT_CHANGED(hWindow);
	// to release capture
	Control_OnLButtonUp(hWindow, x, y, keyFlags);
}

/***********************************************************************/
LOCAL void Map_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	POINT Point;
	LPMAPCTL lpmap;
	int dx, dy;
	

	if ( !Control_bTrack )
		return;
	if (!(lpmap = GET_LPMAPCTL(hWindow)))
		return;

	// Just Browse??
	if (lpmap->active == NO_ACTIVE && (keyFlags & MK_LBUTTON))
	{
		lpmap->ptBrowse.x = x;
		lpmap->ptBrowse.y = y;
		map_from_screen(lpmap, &lpmap->ptBrowse, NO);
		POINT_BROWSE(hWindow);
		return;
	}
	
	lpmap->ptBrowse.x = INT_MAX;
	Point.x = x;
	Point.y = y;
	
	if (keyFlags & MK_CONTROL)
	{
		// constrain
		if (!lpmap->bConstrainX && !lpmap->bConstrainY)
		{
			// still looking for constrain
			dx = abs(lpmap->ptConstrain.x - Point.x);
			dy = abs(lpmap->ptConstrain.y - Point.y);
			if (dx > 1 || dy > 1)
			{
				if (dx > dy)
					lpmap->bConstrainY = TRUE;
				else
					lpmap->bConstrainX = TRUE;
			}
		}
		if (lpmap->bConstrainX)
			Point.x = lpmap->ptConstrain.x;
		if (lpmap->bConstrainY)
			Point.y = lpmap->ptConstrain.y;
	}
	else
	{
		// prepare for later constrain
		lpmap->ptConstrain = Point;
		lpmap->bConstrainX = lpmap->bConstrainY = FALSE;
	}
	
	// change point
	map_from_screen(lpmap, &Point, YES /* adjust for gamma */);
	map_move_point(lpmap, hWindow, lpmap->active, &Point);
	POINT_CHANGING(hWindow);
}

/***********************************************************************/
LOCAL BOOL Map_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
	LPMAPCTL lpmap;
	int x, y;
	POINT pt;
	HDC hDC;
	BOOL bDrawHandles;
	
	if (!(lpmap = GET_LPMAPCTL(hWindow)))
		return(1);
	
	switch (vk)
	{
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			x = (vk==VK_LEFT) ? -1:((vk==VK_RIGHT) ? 1:0);
			y = (vk==VK_UP) ? 1:((vk==VK_DOWN) ? -1:0);
			if (SHIFT)
			{
				x*=5;
				y*=5;
			}
			if (!WITHIN(lpmap->active, 0, lpmap->NumPoints))
				break;
			if (lpmap->dwStyle & MS_INVERTX)
				x = -x;
			if (lpmap->dwStyle & MS_INVERTY)
				y = -y;
			pt = lpmap->lpPoints[lpmap->active];
			pt.x+=x;
			pt.y+=y;
			map_move_point(lpmap, hWindow, lpmap->active, &pt);
			POINT_CHANGING(hWindow);
		break;
		case VK_DELETE:
			if (lpmap->active != NO_ACTIVE)
			{
				delete_point(lpmap, hWindow, lpmap->active);
				POINT_CHANGED(hWindow);
			}
		break;
		case VK_SPACE:
			if (lpmap->NumPoints < 1)
				break;
			bDrawHandles = !(lpmap->dwStyle & MS_NOHANDLES);
			// determine new active point
			x = lpmap->active + ((SHIFT) ? -1: +1);
			if (!WITHIN(x, 0, lpmap->NumPoints-1))
				x = (SHIFT) ? lpmap->NumPoints-1 : 0;
			hDC = map_get_dc(hWindow, lpmap);
			if (lpmap->active != NO_ACTIVE && bDrawHandles)
			{
				map_draw_handle(lpmap, hDC, lpmap->active, YES /*active*/, OFF);
				map_draw_handle(lpmap, hDC, lpmap->active, NO /*active*/, ON);
			}
			// select the point
			lpmap->active = x;
			if (lpmap->active != NO_ACTIVE && bDrawHandles)
			{
				map_draw_handle(lpmap, hDC, lpmap->active, NO /*active*/, OFF);
				map_draw_handle(lpmap, hDC, lpmap->active, YES /*active*/, ON);
			}
			map_done_dc(hWindow, hDC, YES /*Free DC?*/);
			POINT_CHANGED(hWindow);
		break;
		default:
			return(1);
		break;	
	}
	return(0);
}

/***********************************************************************/
LOCAL void Map_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
	Map_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
LOCAL void Map_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
	Map_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
LOCAL void Map_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
	DWORD dwStyles = GetWindowLong(hWindow, GWL_STYLE);

	if (!(dwStyles & WS_TABSTOP))
		return;
	// skip the focus rect for now
	//	// Draw the focus rectangle
	//	hDC = GetDC( hWindow );
	//	GetClientRect( hWindow, &ClientRect );
	//	FrameRect( hDC, &ClientRect,
	//		GetStockObject( fSetFocus ? BLACK_BRUSH : WHITE_BRUSH ) );
	//	ReleaseDC( hWindow, hDC );
}
	
//***********************************************************************
LOCAL void DrawLOCALBox( HDC hDC, LPRECT lpRect)
//***********************************************************************
{
	HGDIOBJ  hOldPen, hPen;
	RECT     Rect;
	POINT    p;

	Rect = *lpRect;
	Rect.right--;
	Rect.bottom--;

	hPen = GetStockObject(BLACK_PEN); 

	// Draw the upper left border
	hOldPen = SelectObject( hDC, hPen );
	MoveToEx( hDC, Rect.left, Rect.bottom, &p);
	LineTo( hDC, Rect.left, Rect.top );
	LineTo( hDC, Rect.right, Rect.top );

	SelectObject( hDC, hOldPen );
}


//************************************************************************
//	   	map_add_point
//
// PARAMETERS: 
//					LPPOINT lpAdd- the point to add. 
// DESCRIPTION:
//				Adds lpPoint point to the map and makes it the active point.  
//				Updates display.
// RETURNS:
//				FALSE if the point can not be added (above MaxPoint)
//************************************************************************
LOCAL BOOL map_add_point(HWND hWindow, LPMAPCTL lpmap, LPPOINT lpAdd)
//***********************************************************************
{
	HDC hDC;
	int i;
	
	// be sure there is room
	if (lpmap->NumPoints >= lpmap->MaxPoints)
		return(FALSE);
		
	hDC = map_get_dc(hWindow, lpmap);
	
	// Undraw 
	map_draw_range(lpmap, hDC, &lpmap->rArea, 0, MAXMAP, OFF);
	
	// find spot to insert
	for (i=0; i<lpmap->NumPoints; i++)
	{
		// add point if we are about to pass it
		if (lpmap->lpPoints[i].x >= lpAdd->x)
			break;
	}
	// in range?
	if (i>=lpmap->MaxPoints)
		return(FALSE);
	// insert point
	if (lpmap->lpPoints[i].x != lpAdd->x)
	{
		MoveMemory((LPTR)&lpmap->lpPoints[i+1],
						(LPTR)&lpmap->lpPoints[i],
						(lpmap->MaxPoints-1-i)*sizeof(POINT));
		lpmap->NumPoints++;
	}
	lpmap->lpPoints[i] = *lpAdd;
	lpmap->active = i;
	
	// redraw
	map_compute(lpmap);
	map_draw_range(lpmap, hDC, &lpmap->rArea, 0, MAXMAP, ON);
	map_done_dc(hWindow, hDC, YES /*Free DC?*/);
	return(TRUE);
}	


//************************************************************************
//
// PARAMETERS:
//					int index- the point to remove
// DESCRIPTION:
//			Removes the given point from the map. Updates visual.
//			Will not delete from a 1 point map.
// RETURNS:
//			FALSE if point not removed.
//************************************************************************
LOCAL BOOL delete_point(LPMAPCTL lpmap, HWND hWindow, int index)
//************************************************************************
{
	HDC hDC;
	
	if (!WITHIN(index, 0, lpmap->NumPoints-1))
		return(FALSE);

	// 1 point map?
	if (lpmap->NumPoints <= 1)
		return(FALSE);
	
	// Undraw area 
	hDC = map_get_dc(hWindow, lpmap);
	map_draw_range( lpmap, hDC, &lpmap->rArea, 0, MAXMAP, OFF);
	
	// delete it
	MoveMemory((LPTR)&lpmap->lpPoints[index],
					(LPTR)&lpmap->lpPoints[index+1],
					(lpmap->MaxPoints-1-index)*sizeof(POINT));
	lpmap->lpPoints[lpmap->MaxPoints-1].x = 0;
	lpmap->NumPoints--;
	// redisplay
	map_compute(lpmap);
	map_draw_range( lpmap, hDC, &lpmap->rArea, 0, MAXMAP, ON);
	map_done_dc(hWindow, hDC, YES /*Free DC?*/);
	return(TRUE);
	
}
	
//************************************************************************
//
// PARAMETERS:
//					int index- The point to move.
//					LPPOINT lpNewPoint- The new value.
// DESCRIPTION:
//		Changes the value of the given point.  Performs bounding.  
//		Updates display and lpmap->Map.
// RETURNS:
//		FALSE if index is out of range.
//************************************************************************
LOCAL BOOL map_move_point(LPMAPCTL lpmap, HWND hWindow, int index, LPPOINT lpNewPoint)
//************************************************************************
{
	HDC hDC;
	int iPrev, iNext;
	POINT Point = *lpNewPoint;
	
	if (!WITHIN(index, 0, lpmap->NumPoints-1))
		return(FALSE);
	// Undraw area 
	hDC = map_get_dc(hWindow, lpmap);
	map_draw_range( lpmap, hDC, &lpmap->rArea, 0, MAXMAP, OFF);

	// bound to neighbors
	iPrev = 0;
	iNext = MAXVAL;
	if (index > 0)
		iPrev = lpmap->lpPoints[index-1].x+1;
	if (index+1 < lpmap->NumPoints)
		iNext = lpmap->lpPoints[index+1].x-1;
	Point.x = BOUND(Point.x, iPrev, iNext);
	Point.y = BOUND(Point.y, 0, MAXVAL);
	// set point
	lpmap->lpPoints[lpmap->active] = Point;

	// redisplay
	map_compute(lpmap);
	map_draw_range( lpmap, hDC, &lpmap->rArea, 0, MAXMAP, ON);
	map_done_dc(hWindow, hDC, YES /*Free DC?*/);
	return(TRUE);
}

//************************************************************************
//				map_draw_grid
// PARAMETERS:
//					HDC hDC- Can NOT be NULL.
//					LPRECT lpArea- The area to update.
// DESCRIPTION:
//				draws the grid at the given spacing (map coords).
//************************************************************************
LOCAL VOID map_draw_grid(LPMAPCTL lpmap, HDC hDC, LPRECT lpArea)
//***********************************************************************
{
	LFIXED rate;
	HPEN hOldPen, hPen;
	int i, val, offset;
	int gridX = lpmap->gridX;
	int gridY = lpmap->gridY;
	POINT    p;
	
//	hPen = CreatePen(PS_DOT, 0,
//				RGB(lpmap->gridRGB.red, lpmap->gridRGB.green, lpmap->gridRGB.blue));
	hPen = DrawTools.GetBtnFacePen();
	hOldPen = (HPEN)SelectObject( hDC, hPen );
	
	// do vertical lines
	if (gridX)
	{
		rate = FGET(DRAW_WIDTH(lpmap), MAXVAL);
		offset = DRAW_LEFT(lpmap);
		for (i=gridX; i<256; i+=gridX)
		{
			val = offset + FMUL(i, rate);
			MoveToEx( hDC, val, lpmap->rArea.top, &p);
			LineTo( hDC, val, lpmap->rArea.bottom+1);
		}
	}
	// do horizontal lines
	if (gridY)
	{
		rate = FGET(DRAW_HEIGHT(lpmap), MAXVAL);
		offset = DRAW_BOTTOM(lpmap);
		for (i=gridX; i<256; i+=gridX)
		{
			val = offset - FMUL(i, rate);
			MoveToEx( hDC, lpmap->rArea.left, val, &p);
			LineTo( hDC, lpmap->rArea.right+1, val);
		}
	}
	SelectObject( hDC, hOldPen );
//	DeleteObject(hPen);
}
	
	

//************************************************************************
//			map_draw_range
// PARAMETERS:
//					LPRECT lpArea- The area to update.
//					int first- The first position in the map to draw.
//					int last- The last position in the map to draw.
//					BOOL bOn- draw ON or draw OFF?
//					LPTR lpData- Should be full 256 map or NULL.
//
// DESCRIPTION:
//			Draws the given range from lpData.  If lpData==NULL then it 
//			will use lpmap->Map (setup in map_compute).	 Draws handles 
//			unless (dwFlags & MS_NOHANDLES). 
// RETURNS:
//
//************************************************************************
LOCAL void map_draw_range(LPMAPCTL lpmap, HDC hDC, LPRECT lpArea, int first, int last, BOOL bOn, LPTR lpData)
//***********************************************************************
{
	LFIXED rateX, rateY, lVal;
	BOOL bInvX, bInvY;
	POINT offset;
	POINT DataOut[256];
	int OldROP;
	int i, num;
	POINT    p;
	
	bInvX = (lpmap->dwStyle & MS_INVERTX);
	bInvY = (lpmap->dwStyle & MS_INVERTY);
   	rateX = FGET(DRAW_WIDTH(lpmap), MAXVAL);
   	rateY = FGET(DRAW_HEIGHT(lpmap), MAXVAL);
	offset.x = (bInvX ? DRAW_RIGHT(lpmap) : DRAW_LEFT(lpmap));
	offset.y = (bInvY ? DRAW_TOP(lpmap): DRAW_BOTTOM(lpmap));

	// detrmine source data
	if (!lpData)
		lpData = lpmap->Map;
	// convert to screen coords
	if (rateX < FUNITY)
	{
		// go through pixels
		num = DRAW_WIDTH(lpmap);
	   	rateX = FGET(MAXVAL, num);
		for (i=0; i<num; i++)
		{
			DataOut[i].x = i;
			DataOut[i].y = FMUL(lpData[FMUL(i,rateX)], rateY);
		}
	}
	else
	{
		// go through map
		lVal = 0;
		num = 256;
	 	for (i=0; i<num;i++)
		{
			DataOut[i].x = FROUND(lVal);
			DataOut[i].y = FMUL(lpData[i], rateY);
			lVal += rateX;
		}
	}
	
	// convert to screen coords
	for (i=0; i<num;i++)
	{
		DataOut[i].x = (bInvX ? (offset.x-DataOut[i].x) : (offset.x+DataOut[i].x));
		DataOut[i].y = (bInvY ? (offset.y+DataOut[i].y) : (offset.y-DataOut[i].y));
	}
	
	// draw line
	OldROP = SetROP2( hDC, R2_NOT );
	Polyline(hDC, DataOut, num);
	
	MoveToEx(hDC,bInvX ? lpmap->rArea.right:lpmap->rArea.left, DataOut[0].y, &p);
   	LineTo(hDC,DataOut[0].x, DataOut[0].y);
   	
   	MoveToEx(hDC,DataOut[num-1].x, DataOut[num-1].y, &p);
	LineTo(hDC, bInvX ? (lpmap->rArea.left):(lpmap->rArea.right+1), DataOut[num-1].y);
	SetROP2( hDC, OldROP );
	
	// draw handles
	map_draw_handles( lpmap, hDC, lpArea, bOn);
}


//************************************************************************
//			map_draw_handles
// PARAMETERS:
//					LPRECT lpArea- The area to update.
//					BOOL bOn- draw ON or draw OFF?
// DESCRIPTION:
//				Draws all handles.  Draws active handle.  Draws probe.
//				Doesn't draw flags if MS_NOHANDLES.
//************************************************************************
LOCAL VOID map_draw_handles(LPMAPCTL lpmap, HDC hDC, LPRECT lpArea, BOOL bOn)
//***********************************************************************
{
	int i;
	
	map_draw_probe(lpmap, hDC, bOn);

	if (lpmap->dwStyle & MS_NOHANDLES)
		return;
	
	for(i=0; i<lpmap->NumPoints; i++)
		map_draw_handle(lpmap, hDC, i, (i==lpmap->active), bOn);
}


//************************************************************************
//			map_draw_handle
// PARAMETERS:
//					int index- Index of the point to draw.
//					BOOL bActive- Draw as active?
//					BOOL bOn- draw ON or draw OFF?
// DESCRIPTION:
//				Draws the given handle.
//************************************************************************
LOCAL VOID map_draw_handle(LPMAPCTL lpmap, HDC hDC, int index, BOOL bActive, BOOL bOn)
//***********************************************************************
{
	POINT pt;
	RECT rect;
	int OldROP;
	POINT    p;
	
	if (!WITHIN(index, 0, lpmap->NumPoints-1))
		return;

	pt = lpmap->lpPoints[index];
   	
	// gamma adjust
	if (lpmap->gamma != 1.0)
		pt.y = GAMMA(pt.x, pt.y, lpmap->gamma, (lpmap->dwStyle & MS_INVERT_GAMMA));

	// keep visible
	pt.x = BOUND(pt.x, MINVAL, MAXVAL);
	pt.y = BOUND(pt.y, MINVAL, MAXVAL);

	// convert to screen coords
   	pt.x = ((LONG)pt.x * DRAW_WIDTH(lpmap))/MAXVAL;
   	pt.y = ((LONG)pt.y * DRAW_HEIGHT(lpmap))/MAXVAL;
	pt.x = ((lpmap->dwStyle & MS_INVERTX) ? 
			(DRAW_RIGHT(lpmap)-pt.x) : (DRAW_LEFT(lpmap)+pt.x));
	pt.y = ((lpmap->dwStyle & MS_INVERTY) ? 
			(DRAW_TOP(lpmap)+pt.y) : (DRAW_BOTTOM(lpmap)-pt.y));

	rect.left = pt.x - HANDLE_SIZE;
	rect.right = pt.x + HANDLE_SIZE+1;
	rect.top = pt.y - HANDLE_SIZE-1;
	rect.bottom = pt.y + HANDLE_SIZE;
	
	if (bActive)
	{
		rect.right++;
		rect.bottom++;
		InvertRect(hDC, &rect);
	}
	else
	{
		OldROP = SetROP2( hDC, R2_NOT );
		MoveToEx( hDC, rect.left, rect.top , &p);
		LineTo( hDC, rect.right, rect.top );
		LineTo( hDC, rect.right, rect.bottom );
		LineTo( hDC, rect.left, rect.bottom );
		LineTo( hDC, rect.left, rect.top );
		SetROP2( hDC, OldROP );
	}
}
                 
//************************************************************************
//			map_draw_probe
// PARAMETERS:
//					BOOL bOn- draw ON or draw OFF?
// DESCRIPTION:
//				Draws the current probe (if any).
//************************************************************************
LOCAL VOID map_draw_probe(LPMAPCTL lpmap, HDC hDC, BOOL bOn)
//***********************************************************************
{
	POINT pt;
	RECT rect;
	
	if (lpmap->ptProbe.x == INT_MAX)
		return;

	// convert to screen coords
	pt = lpmap->ptProbe;
   	
   	pt.x = ((LONG)pt.x * DRAW_WIDTH(lpmap))/MAXVAL;
   	pt.y = ((LONG)pt.y * DRAW_HEIGHT(lpmap))/MAXVAL;
	pt.x = ((lpmap->dwStyle & MS_INVERTX) ? 
				(DRAW_RIGHT(lpmap)-pt.x) : (DRAW_LEFT(lpmap)+pt.x));
	pt.y = ((lpmap->dwStyle & MS_INVERTY) ? 
				(DRAW_TOP(lpmap)+pt.y) : (DRAW_BOTTOM(lpmap)-pt.y));

	rect.left = pt.x - 1;
	rect.right = pt.x + 1;
	rect.top = pt.y - HANDLE_SIZE - 1;
	rect.bottom = pt.y + HANDLE_SIZE;

	InvertRect(hDC, &rect);
	
	rect.left = pt.x - HANDLE_SIZE;
	rect.right = pt.x + HANDLE_SIZE + 1;
	rect.top = pt.y - 1;
	rect.bottom = pt.y + 1;

	InvertRect(hDC, &rect);
}




// used for map_compute & map_compute_curves

#define GAMMABOUND(pt) \
	{pt.x = BOUND(pt.x, MINVAL, MAXVAL); \
	 if (!bGamma){pt.y = BOUND(pt.y, MINVAL, MAXVAL);} \
	 else {gval = GAMMA(pt.x, pt.y, gamma, (lpmap->dwStyle & MS_INVERT_GAMMA)); \
			gval = BOUND(gval, MINVAL, MAXVAL); \
			pt.y = INVGAMMA(pt.x, gval, gamma, (lpmap->dwStyle & MS_INVERT_GAMMA));}} \
			
//************************************************************************
//			map_compute
// DESCRIPTION:
//			Fills lpmap->Map based on the current points, gamma, and styles
//************************************************************************
LOCAL VOID map_compute(LPMAPCTL lpmap)
//***********************************************************************
{
	BOOL bCurves, bGamma, bInvGamma;
	double gamma;
	int index, y, dy, dx, i;
	long val, gval;
	POINT pt1, pt2;
	
	
	// no curve?
	if (!lpmap->lpPoints || lpmap->NumPoints <= 0)
		return;
	
	bCurves = (lpmap->dwStyle & MS_CURVES);
	gamma = lpmap->gamma;
	bGamma = (gamma != 1.0);
	bInvGamma = (lpmap->dwStyle & MS_INVERT_GAMMA);
	
	if (bCurves && 	lpmap->NumPoints > 2)
	{
		map_compute_curves(lpmap);
		return;
	}
	
	index = 0;
	pt1 = lpmap->lpPoints[index++];
	GAMMABOUND(pt1);
	if (pt1.x != 0 || lpmap->NumPoints == 1)
	{
		// start with horizontal line
		pt1.x = 0;
		index--;
	}
	pt2 = lpmap->lpPoints[index];
	GAMMABOUND(pt2);
	dx = pt2.x - pt1.x;
	dy = pt2.y - pt1.y;
	for (i=0; i<256; i++)
	{
		if (i > pt2.x)
		{
			pt1 = pt2;
			index++;
			// new segment
			if(index >= lpmap->NumPoints)
			{
				// horizontal line to end
				pt2.x = MAXVAL;
				dx = 1;
				dy = 0;
			}
			else
			{
				// next point
				pt2 = lpmap->lpPoints[index];
				if (pt2.x == 0)
					pt2 = pt1;
				else
					GAMMABOUND(pt2);
				dx = pt2.x - pt1.x;
				dy = pt2.y - pt1.y;
			}
		}
		// compute point
		if (dx)
			y = pt1.y + (dy*(LONG)(i-pt1.x))/dx;
		else
			y = pt1.y;
		if (bGamma)
		{
			val = GAMMA(i,y,lpmap->gamma, bInvGamma);
			if (!WITHIN(val, 0, MAXVAL))
				val = BOUND(val, 0, MAXVAL);
			lpmap->Map[i] = (BYTE)val;
		}
		else
			lpmap->Map[i] = y;
	}
}

LOCAL void get_spline_end(LPPOINT pt,int num, double *d1,double *d2)
{
	*d1 =(double)(pt[1].y-pt[0].y)/(pt[1].x-pt[0].x);
	*d2 =(double)(pt[num-1].y-pt[num-2].y)/(pt[num-1].x-pt[num-2].x);
}



//************************************************************************
//			map_compute_curves
// DESCRIPTION:
//			Fills lpmap->Map based on the current points, gamma, and 
//			styles.  Assumes curve interpolation.
//************************************************************************
LOCAL VOID map_compute_curves(LPMAPCTL lpmap)
//***********************************************************************
{
	BOOL bGamma, bInvGamma;
	double gamma;
	int y, index, num, i;
	int firstreal, lastreal;
	int minx, maxx;
	long  val, curve;
	POINT pt[SSIZE];
	double spline[SSIZE], d1, d2;
	
	gamma = lpmap->gamma;
	bGamma = (gamma != 1.0);
	bInvGamma = (lpmap->dwStyle & MS_INVERT_GAMMA);
	
	// must have at least 2 points
	if (lpmap->NumPoints <= 2)
		return;
	
	num = 0;
	index = 0;
	firstreal = lastreal = 0;
	// start with horizontal line
	pt[num] = lpmap->lpPoints[index++];
//	GAMMABOUND(pt[num]);
	num++;
	if (pt[0].x > 0)
	{
		pt[1] = pt[0];
		pt[0].x = 0;
		num++;
		firstreal = 1;
	}	
	while(num < SSIZE && index < lpmap->NumPoints)
	{
		pt[num] = lpmap->lpPoints[index++];
//		GAMMABOUND(pt[num]);
		// weed out duplicate x's
		if (pt[num].x > pt[num-1].x)
			num++;
	}
	lastreal = num-1;
	if (num < SSIZE && pt[num-1].x < MAXVAL)
	{
		// ran out!
		pt[num].y = pt[num-1].y;
		pt[num].x = MAXVAL;
//		GAMMABOUND(pt[num]);
		num++;
	}

	d1 = d2 = 0;
   	curve =(int)((double)lpmap->CurveType/10.0);
	Spline(&pt[0], num, 1.0e30, 1.0e30, spline, curve);
	minx = pt[firstreal].x;
	maxx = pt[lastreal].x;
	for (i=0; i<256; i++)
	{
		// Should handle case where we don't yet have all the points
		// compute point
		if (i<minx)
			y = pt[firstreal].y;
		else if (i>maxx)
			y = pt[lastreal].y;
		else
		{
		 	y = (int)Splint(&pt[0], spline, num, i, curve);
			y = BOUND(y, 0, MAXVAL);
		}
		if (bGamma)
		{
			val = GAMMA(i,y,lpmap->gamma, bInvGamma);
			if (!WITHIN(val, 0, MAXVAL))
				val = BOUND(val, 0, MAXVAL);
			lpmap->Map[i] = (BYTE)val;
		}
		else
			lpmap->Map[i] = BOUND(y, 0, MAXVAL);
	}						   
}




//************************************************************************
//
// PARAMETERS:
//					LPPOINT lpConvert- The point to convert.
//					BOOL bGamma- Apply inverse gamma?
// DESCRIPTION:
//				Converts from screen coords to map coords. 
//				Does proper invert.
//************************************************************************
LOCAL VOID map_from_screen(LPMAPCTL lpmap, LPPOINT lpConvert, BOOL bGamma)
//***********************************************************************
{
	LONG x,y;
	
	// get raw map coords 	
   	x = ((LONG)lpConvert->x-DRAW_LEFT(lpmap)) * MAXVAL;
   	x /= DRAW_WIDTH(lpmap);
   	y = ((LONG)DRAW_BOTTOM(lpmap) - lpConvert->y) * MAXVAL;
   	y /= DRAW_HEIGHT(lpmap);
	
	// invert if needed
	if (lpmap->dwStyle & MS_INVERTX)
		x = MAXVAL - x;
	if (lpmap->dwStyle & MS_INVERTY)
		y = MAXVAL - y;
		
	if (bGamma && lpmap->gamma != 1.0)
	{
		// invert gamma!!!
		y = INVGAMMA(x, y, lpmap->gamma, (lpmap->dwStyle & MS_INVERT_GAMMA));
	}
	// bound
	x = BOUND(x,0,MAXVAL);
	y = BOUND(y,0,MAXVAL);
	lpConvert->x = x;
	lpConvert->y = y;
}	


//************************************************************************
//			map_get_dc
// PARAMETERS:
//					HWND hWindow- the window to get the HDC for.
//					LPMAPCTL lpmap- uses for cliping area.
// DESCRIPTION:
//			Creates or uses an exising HDC for the given window cliped to 
//			the map drawing	area.  
// RETURNS:
//			The HDC for drawing or NULL for failure.
//************************************************************************
LOCAL HDC map_get_dc(HWND hWindow, LPMAPCTL lpmap)
//***********************************************************************
{
	HDC hDC;
	
	hDC = GetDC(hWindow);
	map_SelectClipRect(hDC, &lpmap->rArea, NULL);
	return(hDC);
}
	

//************************************************************************
//
// PARAMETERS:
//					HWND hWindow- The window for the DC.
//					HDC hDC- Must be from map_get_dc.
//					BOOL bOk2Free- Can the DC be freed?
//
// DESCRIPTION:
//			Call when done with DC from map_get_dc.
//************************************************************************
LOCAL VOID map_done_dc(HWND hWindow, HDC hDC, BOOL bOk2Free)
//***********************************************************************
{
	if(hDC && bOk2Free)
		ReleaseDC(hWindow, hDC);
}	


//************************************************************************
//
// PARAMETERS:
//					LPPOINT lpTest- The point to test (in map coords)
//					long distsx- The max dist in the x coord for a hit.
//					long distsy- The max dist in the y coord for a hit.
// RETURNS:
//			The index of the closest point to lpTest or NO_ACTIVE if none. 
//************************************************************************
LOCAL int map_close_point(LPMAPCTL lpmap, LPPOINT lpTest, int distx, int disty)
//************************************************************************
{
	BOOL bGamma, bInvGamma;
	int iBest, i;
	long bestdist, dist;
	int minx, maxx, miny, maxy;
	POINT pt;
	
	bGamma = (lpmap->gamma != 1.0);
	bInvGamma = (lpmap->dwStyle & MS_INVERT_GAMMA);
	iBest = NO_ACTIVE;
	bestdist = (LONG)distx*distx + (LONG)disty*disty + 1;
	minx = lpTest->x-distx;
	maxx = lpTest->x+distx;
	miny = lpTest->y-disty;
	maxy = lpTest->y+disty;
	
	// go through points to find best hit
	for (i=0; i<lpmap->NumPoints; i++)
	{
		pt = lpmap->lpPoints[i];
		pt.x = BOUND(pt.x,MINVAL,MAXVAL);
		pt.y = BOUND(pt.y,MINVAL,MAXVAL);
		if (WITHIN(pt.x, minx, maxx))
		{
			if (bGamma)
			{
			 	pt.y = GAMMA(pt.x, pt.y, lpmap->gamma, bInvGamma);
				pt.y = BOUND(pt.y,MINVAL,MAXVAL);
			}
			if (WITHIN(pt.y, miny, maxy))
			{
				// within bounds!
				dist = (LONG)(lpTest->x-pt.x)*(lpTest->x-pt.x)+
						(LONG)(lpTest->y-pt.y)*(lpTest->y-pt.y);
				if (dist < bestdist)
				{
					// best so far
					iBest = i;
					bestdist = dist;
				}
			}
		}	
	}	// end for each point
	return(iBest);
}


//************************************************************************
//			map_close_line
// PARAMETERS:
//					LPPOINT lpTest- The point to test (in map coords)
//					long distsx- The max dist in the y coord for a hit.
// DESCRIPTION:
//			Determines if the point is within disty of the curve at 
//			its x coord.  It uses lpmap->Map, so map_compute must have 
//			been called first.
// RETURNS:
//	 		TRUE if within range;
//************************************************************************
LOCAL BOOL map_close_line(LPMAPCTL lpmap, LPPOINT lpTest, int disty)
//************************************************************************
{	
	int y;		
				   
	// are we close to line?
	if (!WITHIN(lpTest->x, 0, MAXVAL))
		return(FALSE);
	y = lpmap->Map[lpTest->x];
	if (!WITHIN(y, lpTest->y - disty, lpTest->y + disty))
		return(FALSE);
	return(TRUE);
}



/***********************************************************************/
LOCAL BOOL map_SelectClipRect(HDC hDC, LPRECT lpNewRect, LPRECT lpOldRect )
/***********************************************************************/
{
	HRGN	hRgn;
	int	iRet;

	// this routines takes care of the fact that windows rects are
	// different than ours.  i.e. - ours include the right/bottom pixels
	if (lpOldRect)
  	{
		GetClipBox(hDC, lpOldRect);
		--lpOldRect->right; --lpOldRect->bottom;
  	}
	++lpNewRect->right; ++lpNewRect->bottom;
	hRgn = CreateRectRgnIndirect(lpNewRect);
	--lpNewRect->right; --lpNewRect->bottom;
	if (!hRgn)
		return(FALSE);
	iRet = SelectClipRgn(hDC, hRgn);
	DeleteObject(hRgn);
	return(iRet != ERROR);
}


	

//************************************************************************
//		map_init_mapctl
// DESCRIPTION:
//		sets lpmap to default values
//************************************************************************
LOCAL VOID map_init_mapctl(LPMAPCTL lpmap, HWND hMap)
//************************************************************************
{
	if (!lpmap)
		return;
	lpmap->MaxPoints = 0;		// The maximum number of points to allow
	lpmap->lpPoints = NULL;	// The point list (should be at least MaxPoints)
	lpmap->NumPoints=0;			// number of valid points.
	lpmap->active = NO_ACTIVE;	// The active point (can be NO_ACTIVE)
	lpmap->ptProbe.x = INT_MAX;// The currently displayed probe point (if any)
	lpmap->ptBrowse.x = INT_MAX;// The last point browsed (if any)
	lpmap->CurveType = SMOOTH_NORMAL; // Smoothness of curve (if using curves)
	lpmap->gamma = 1.0;		// apply this gamma when displaying
	lpmap->gridX = 25;			// how often to draw vert lines (0 for none)
	lpmap->gridY = 25;			// how often to draw horiz lines (0 for none)
	lpmap->gridRGB.red = 0;  	// color of grid
	lpmap->gridRGB.green = 0;	
	lpmap->gridRGB.blue = 0;	
	GetClientRect(hMap, &lpmap->rArea);
	// one pixel for bevel & one for astral rect
	lpmap->rArea.left += 2;
	lpmap->rArea.top += 2;
	lpmap->rArea.right -= 3;
	lpmap->rArea.bottom -= 3;
}
