//®PL1¯®FD1¯®TP0¯®BT0¯®PL1¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

// Static prototypes
static void MoveInsideRect( LPRECT lpRect, LPRECT lpBoundRect );
static void DrawHandles(HDC hDC,struct tagRECT far *lpSelectRect);
class CDisplayHook : public CHookHandler
{
public:
    CDisplayHook() : CHookHandler() {};
    virtual void ImageHookProc(int x, int y, int dx, FRMTYPEINFO TypeInfo, LPTR lpSrc,
            LPTR lpDst, LPTR lpMsk, LPMASK lpMask) { /*no implementation*/ };
    virtual void DisplayHookProc(HWND hWnd, HDC hDC, LPRECT lpRect); 
protected:
private:
};
CDisplayHook DisplayHook;


static LPRECT GetBoundingRect( HWND hWnd );
static void GetCenter(LPRECT lpRect, LPPOINT lpPoint);

static int Constrain, Mode;
static POINT ptCenter;
static POINT ptAnchor;
static LFIXED fScale;
static CHookHandler *lpLastDisplayHook;

#define CONSTRAINX 1
#define CONSTRAINY 2
#define DRAG_MODE 1
#define MOVE_MODE 2
#define HSIZE 2

/***********************************************************************/
void StartSelection( HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags,
                        POINT ptCurrent, LFIXED fStatusScale )
/* PURPOSE: Begin selection of region					*/
/***********************************************************************/
{
int tmp, minx, maxx, miny, maxy;
LPRECT lpRect;
RECT rBound;
BOOL fDoc;

if ( fDoc = IsDoc( hWnd ) )
	{
	lpLastDisplayHook = SetDisplayHook( hWnd, &DisplayHook );
	Display2File(hWnd, (LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y );
	}
else
if ((fFlags & SL_TYPE) == SL_GRID)
	{
	Control2Image( (LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y );
	}

fScale = fStatusScale;

if (!(fFlags & SL_NOLIMIT))
	{
	if ((fFlags & SL_TYPE) == SL_GRID)
		lpRect = GetGridBoundRect(&rBound);
	else
		lpRect = GetBoundingRect( hWnd );
	ptCurrent.x = bound( ptCurrent.x, lpRect->left, lpRect->right );
	ptCurrent.y = bound( ptCurrent.y, lpRect->top, lpRect->bottom );
	}
ptAnchor = ptCurrent;

Mode = DRAG_MODE;
if ( fFlags & SL_SPECIAL )
	{ /* Setup a selection rectangle at the current location */
	Constrain = 0;
	lpSelectRect->left = ptCurrent.x;
	lpSelectRect->top = ptCurrent.y;
	lpSelectRect->right = ptCurrent.x;
	lpSelectRect->bottom = ptCurrent.y;
	InvertSelection( hWnd, hDC, lpSelectRect, fFlags );
	GetCenter(lpSelectRect, &ptCenter);
	return;
	}

/* Modify the existing selection rectangle */
/* First clip it to the window's client area */
if (!(fFlags & SL_NOLIMIT))
	{
	if ((fFlags & SL_TYPE) == SL_GRID)
		IntersectRect( lpSelectRect, lpSelectRect, GetGridBoundRect(&rBound) );
	else
		IntersectRect( lpSelectRect, lpSelectRect, GetBoundingRect(hWnd) );
	}

/* Figure out which side or corner to drag */
/* The bottom right point is always the one that moves */
Constrain = CONSTRAINX | CONSTRAINY;
if (ptCurrent.x >= lpSelectRect->left - CLOSENESS &&
	ptCurrent.x <= lpSelectRect->right + CLOSENESS &&
	ptCurrent.y >= lpSelectRect->top - CLOSENESS &&
	ptCurrent.y <= lpSelectRect->bottom + CLOSENESS)
	{
	if ((fFlags & SL_TYPE) == SL_BOXHANDLES)
		ConstrainHandles(ptCurrent, lpSelectRect);
	else
		{
		if ( abs( ptCurrent.y - lpSelectRect->bottom ) <= CLOSENESS )
			Constrain ^= CONSTRAINY;
		else
		if ( abs( ptCurrent.y - lpSelectRect->top ) <= CLOSENESS )
			{
			Constrain ^= CONSTRAINY;
			tmp = lpSelectRect->top;
			lpSelectRect->top = lpSelectRect->bottom;
			lpSelectRect->bottom = tmp;
			}
		if ( abs( ptCurrent.x - lpSelectRect->right ) <= CLOSENESS )
			Constrain ^= CONSTRAINX;
		else
		if ( abs( ptCurrent.x - lpSelectRect->left ) <= CLOSENESS )
			{
			Constrain ^= CONSTRAINX;
			tmp = lpSelectRect->left;
			lpSelectRect->left = lpSelectRect->right;
			lpSelectRect->right = tmp;
			}
		}
	if (Constrain == (CONSTRAINX | CONSTRAINY))
		{
		minx = min(lpSelectRect->left, lpSelectRect->right)+CLOSENESS;
		maxx = max(lpSelectRect->left, lpSelectRect->right)-CLOSENESS;
		miny = min(lpSelectRect->top, lpSelectRect->bottom)+CLOSENESS;
		maxy = max(lpSelectRect->top, lpSelectRect->bottom)-CLOSENESS;
		if (ptCurrent.x >= minx && ptCurrent.x <= maxx &&
		ptCurrent.y >= miny && ptCurrent.y <= maxy)
			{
			Mode = MOVE_MODE;
			lpSelectRect->left = minx-CLOSENESS;
			lpSelectRect->right = maxx+CLOSENESS;
			lpSelectRect->top = miny-CLOSENESS;
			lpSelectRect->bottom = maxy+CLOSENESS;
			}
		}
	}
GetCenter(lpSelectRect, &ptCenter);
if (fDoc)
	{
	LPIMAGE lpImage;

	lpImage = GetImagePtr(hWnd);
	DisplayInfo(ptCurrent.x, ptCurrent.y, lpSelectRect);
	StatusOfRectangle( lpSelectRect, fScale, 
				FrameResolution(ImgGetBaseEditFrame(lpImage)) );
	}
else
	StatusOfRectangle( lpSelectRect, fScale, 1000 );
}			  

/***********************************************************************/
void UpdateSelection( HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags,
                        POINT ptCurrent, BOOL bConstrain, long AspectX,
                        long AspectY, BOOL bMove, BOOL bFromCenter )
/* PURPOSE: Update selection						*/
/***********************************************************************/
{
LPRECT lpRect;
BOOL fNeedDC;
int dx, dy;
POINT ptSaved;
RECT OldRect, drect, rBound;
BOOL fDoc;

if ( Mode == MOVE_MODE )
	bMove = YES;
else
if ( Constrain == (CONSTRAINX | CONSTRAINY) ) /* It hasn't been started yet */
	{
	StartSelection( hWnd, hDC, lpSelectRect, fFlags, ptCurrent, fScale );
	}

if ( fDoc = IsDoc( hWnd ) )
	Display2File(hWnd, (LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y );
else
if ((fFlags & SL_TYPE) == SL_GRID)
	Control2Image( (LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y );

if (!(fFlags & SL_NOLIMIT))
	{ // Constrain point to window client area
	if ((fFlags & SL_TYPE) == SL_GRID)
		lpRect = GetGridBoundRect(&rBound);
	else
		lpRect = GetBoundingRect( hWnd );
	ptCurrent.x = bound( ptCurrent.x, lpRect->left, lpRect->right );
	ptCurrent.y = bound( ptCurrent.y, lpRect->top, lpRect->bottom );
	}
ptSaved = ptCurrent;

if ( !bMove )
	{
	// Constrain point in x-direction based on where grabbed
	if ( Constrain & CONSTRAINX )
		ptCurrent.x = lpSelectRect->right;

	// Constrain point in y-direction based on where grabbed
	if ( Constrain & CONSTRAINY )
		ptCurrent.y = lpSelectRect->bottom;
	}

if ( fNeedDC = (!hDC) )
	hDC = GetDC(hWnd);

switch (fFlags & SL_TYPE)
	{
	case SL_BOX:
	case SL_BOXHANDLES:
	case SL_GRID:
	case SL_LINE:
	case SL_ELLIPSE:
		InvertSelection( hWnd, hDC, lpSelectRect, fFlags );
		if ( bMove )
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
		if (bConstrain)
			ConstrainSelection(lpSelectRect, AspectX, AspectY, bFromCenter);
		dx = lpSelectRect->right - lpSelectRect->left;
		dy = lpSelectRect->bottom - lpSelectRect->top;
		if (bFromCenter)
			{
			lpSelectRect->left  = ptCenter.x - dx/2;
			lpSelectRect->top   = ptCenter.y - dy/2;
			lpSelectRect->right  = ptCenter.x + dx - dx/2;
			lpSelectRect->bottom   = ptCenter.y + dy - dy/2;
			}
		GetCenter(lpSelectRect, &ptCenter);
		/* Keep the new rectangle inside the window's client area */
		if (!(fFlags & SL_NOLIMIT))
			{
			if ((fFlags & SL_TYPE) == SL_GRID)
				MoveInsideRect( lpSelectRect, GetGridBoundRect(&rBound) );
			else
				MoveInsideRect( lpSelectRect, GetBoundingRect(hWnd) );
			}
		InvertSelection( hWnd, hDC, lpSelectRect, fFlags );
		break;

	case SL_BLOCK:
		OldRect = *lpSelectRect;
		dx = ptCurrent.x - lpSelectRect->right;
		dy = ptCurrent.y - lpSelectRect->bottom;
		lpSelectRect->right = ptCurrent.x;
		lpSelectRect->bottom = ptCurrent.y;
		if (bConstrain)
			ConstrainSelection(lpSelectRect, AspectX, AspectY, bFromCenter);
		if (bFromCenter)
			{
			lpSelectRect->left  = ptCenter.x - dx/2;
			lpSelectRect->top   = ptCenter.y - dy/2;
			lpSelectRect->right  = ptCenter.x + dx - dx/2;
			lpSelectRect->bottom   = ptCenter.y + dy - dy/2;
			}
		GetCenter(lpSelectRect, &ptCenter);
		drect.left = OldRect.left - lpSelectRect->left;
		drect.right = OldRect.right - lpSelectRect->right;
		drect.top = OldRect.top - lpSelectRect->top;
		drect.bottom = OldRect.bottom - lpSelectRect->bottom;
		PatBlt(hDC, lpSelectRect->left, lpSelectRect->bottom,
			OldRect.right - lpSelectRect->left,
			drect.bottom, DSTINVERT);
		PatBlt(hDC, lpSelectRect->right, OldRect.top,
			drect.right,
			lpSelectRect->bottom - OldRect.top, DSTINVERT);
		PatBlt(hDC, OldRect.left, OldRect.top,
			lpSelectRect->right-OldRect.left,
			drect.top, DSTINVERT);
		PatBlt(hDC, OldRect.left, lpSelectRect->top,
			drect.left,
			OldRect.bottom-lpSelectRect->top, DSTINVERT);
		break;
	}

ptAnchor = ptSaved;
if (fDoc)
	{
	LPIMAGE lpImage;

	lpImage = GetImagePtr(hWnd);
	DisplayInfo(ptCurrent.x, ptCurrent.y, lpSelectRect);
	StatusOfRectangle( lpSelectRect, fScale, 
				FrameResolution(ImgGetBaseEditFrame(lpImage)) );
	}
else
	StatusOfRectangle( lpSelectRect, fScale, 1000 );
if ( fNeedDC )
	ReleaseDC(hWnd, hDC);
}

/***********************************************************************/
// Gets center of rect
/***********************************************************************/
static void GetCenter(LPRECT lpRect, LPPOINT lpPoint)
/***********************************************************************/
{
int dx, dy;

dx = lpRect->right - lpRect->left;
dy = lpRect->bottom - lpRect->top;
// choose new center
lpPoint->x = lpRect->left + dx/2;
lpPoint->y = lpRect->top + dy/2;
}


/***********************************************************************/
static void MoveInsideRect( LPRECT lpRect, LPRECT lpBoundRect )
/***********************************************************************/
{ // Assumes that lpBoundRect is Ordered and that lpRect may not be
// Also assumes that lpRect fits within lpBundRect
int x, y, dx, dy, l, r, t, b;

if ( (l = lpRect->left) > (r = lpRect->right) )
	{ x = l; l = r; r = x; }
if ( (x = lpBoundRect->left - l) > 0 ) // too far left
	dx = x; // push it to right
else
if ( (x = lpBoundRect->right - r) < 0 ) // too far right
	dx = x; // push it to left
else
	dx = 0;

if ( (t = lpRect->top) > (b = lpRect->bottom) )
	{ y = t; t = b; b = y; }
if ( (y = lpBoundRect->top - t) > 0 ) // too far up
	dy = y; // push it down
else
if ( (y = lpBoundRect->bottom - b) < 0 ) // too far down
	dy = y; // push it up
else
	dy = 0;

OffsetRect( lpRect, dx, dy );
}


/***********************************************************************/
void InvertSelection( HWND hWnd, HDC hDC, LPRECT lpRect, int fFlags)
/* PURPOSE: Inverts the current selection					*/
/***********************************************************************/
{
HBRUSH hOldBrush;
short OldROP;
BOOL fNeedDC;
int x, y, dx, dy, type, cx, cy;
RECT ClientRect, SelectRect;
static RECT LastSelectRect;
static int LastFlags;
BOOL fDoc;

if ( fFlags && lpRect )
	{ // Saved for the display hook
	LastSelectRect = *lpRect;
	LastFlags = fFlags;
	}
else
	{ // Called from the display hook
	fFlags = LastFlags;
	}

type = fFlags & SL_TYPE;
SelectRect = LastSelectRect;
if ( fDoc = IsDoc( hWnd ) )
	{
	if (type == SL_LINE)
		{
		File2DisplayEx(hWnd, (LPINT)&SelectRect.left, (LPINT)&SelectRect.top, YES );
		File2DisplayEx(hWnd, (LPINT)&SelectRect.right, (LPINT)&SelectRect.bottom, YES );
		}
	else
		{
   	OrderRect(&SelectRect, &SelectRect);
		File2DispRectExact(hWnd, &SelectRect, &SelectRect);	
		}
	}

if ( fNeedDC = (!hDC) )
	hDC = GetDC(hWnd);

switch (type)
	{
	case SL_BOX:
	OldROP = SetROP2(hDC, R2_NOT);
	MoveTo( hDC, SelectRect.left, SelectRect.top );
	LineTo( hDC, SelectRect.right, SelectRect.top );
	LineTo( hDC, SelectRect.right, SelectRect.bottom );
	LineTo( hDC, SelectRect.left, SelectRect.bottom );
	LineTo( hDC, SelectRect.left, SelectRect.top );
	SetROP2(hDC, OldROP);
	break;

	case SL_BOXHANDLES:
	OldROP = SetROP2(hDC, R2_NOT);
	MoveTo( hDC, SelectRect.left, SelectRect.top );
	LineTo( hDC, SelectRect.right, SelectRect.top );
	LineTo( hDC, SelectRect.right, SelectRect.bottom );
	LineTo( hDC, SelectRect.left, SelectRect.bottom );
	LineTo( hDC, SelectRect.left, SelectRect.top );
	SetROP2(hDC, OldROP);
	DrawHandles(hDC, &SelectRect);
	break;

	case SL_GRID:
	OldROP = SetROP2(hDC, R2_NOT);
	GetClientRect( hWnd, &ClientRect );
	dx = abs( RectWidth( &SelectRect ) );
	dy = abs( RectHeight( &SelectRect ) );
	if ( dx > CLOSENESS && dy > CLOSENESS )
		{
		x = min( SelectRect.left, SelectRect.right );
		cx = x;
		Image2Control(&cx, &cy);
		while ( cx >= ClientRect.left )
			{
			MoveTo( hDC, cx, ClientRect.top );
			LineTo( hDC, cx, ClientRect.bottom );
			x -= dx;
			cx = x;
			Image2Control(&cx, &cy);
			}
		x = max( SelectRect.left, SelectRect.right );
		cx = x;
		Image2Control(&cx, &cy);
		while ( cx <= ClientRect.right )
			{
			MoveTo( hDC, cx, ClientRect.top );
			LineTo( hDC, cx, ClientRect.bottom );
			x += dx;
			cx = x;
			Image2Control(&cx, &cy);
			}
		y = min( SelectRect.top, SelectRect.bottom );
		cy = y;
		Image2Control(&cx, &cy);
		while ( cy >= ClientRect.top )
			{
			MoveTo( hDC, ClientRect.left, cy );
			LineTo( hDC, ClientRect.right, cy );
			y -= dy;
			cy = y;
			Image2Control(&cx, &cy);
			}
		y = max( SelectRect.top, SelectRect.bottom );
		cy = y;
		Image2Control(&cx, &cy);
		while ( cy <= ClientRect.bottom )
			{
			MoveTo( hDC, ClientRect.left, cy );
			LineTo( hDC, ClientRect.right, cy );
			y += dy;
			cy = y;
			Image2Control(&cx, &cy);
			}
		}
	else
		{
		Image2Control((LPINT)&SelectRect.left, (LPINT)&SelectRect.top);
		Image2Control((LPINT)&SelectRect.right, (LPINT)&SelectRect.bottom);
		MoveTo( hDC, SelectRect.left, SelectRect.top );
		LineTo( hDC, SelectRect.right, SelectRect.top );
		LineTo( hDC, SelectRect.right, SelectRect.bottom );
		LineTo( hDC, SelectRect.left, SelectRect.bottom );
		LineTo( hDC, SelectRect.left, SelectRect.top );
		}
	SetROP2(hDC, OldROP);
	break;

	case SL_LINE:
	OldROP = SetROP2(hDC, R2_NOT);
	MoveTo( hDC, SelectRect.left, SelectRect.top );
	LineTo( hDC, SelectRect.right, SelectRect.bottom );
	SetROP2(hDC, OldROP);
	break;

	case SL_ELLIPSE:
	OldROP = SetROP2(hDC, R2_NOT);
	hOldBrush = (HBRUSH)SelectObject( hDC, GetStockObject(NULL_BRUSH) );
	Ellipse( hDC, SelectRect.left, SelectRect.top,
		SelectRect.right, SelectRect.bottom );
	SetROP2(hDC, OldROP);
	SelectObject( hDC, hOldBrush );
	break;

	case SL_BLOCK:
	PatBlt(hDC, SelectRect.left, SelectRect.top,
		SelectRect.right - SelectRect.left,
		SelectRect.bottom - SelectRect.top, DSTINVERT);
	break;
	}

if ( fNeedDC )
	ReleaseDC(hWnd, hDC);
}


/***********************************************************************/
void EndSelection( HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags,
                     BOOL bRemove )
/* PURPOSE: End selection of region					*/
/***********************************************************************/
{
BOOL fDoc;

if ( bRemove )
	InvertSelection( hWnd, hDC, lpSelectRect, fFlags );
if ( fDoc = IsDoc( hWnd ) )
	SetDisplayHook( hWnd, lpLastDisplayHook );
if (fDoc)
	DisplayInfo(lpSelectRect->right, lpSelectRect->bottom, NULL);
OrderRect( lpSelectRect, lpSelectRect );
MessageStatus( 0 );
}


/***********************************************************************/
void OrderRect( LPRECT lpSrcRect, LPRECT lpDstRect )
/***********************************************************************/
{
int tmp;

*lpDstRect = *lpSrcRect;
if ( lpDstRect->right < lpDstRect->left )
	{
	tmp = lpDstRect->right;
	lpDstRect->right = lpDstRect->left;
	lpDstRect->left = tmp;
	}
if ( lpDstRect->bottom < lpDstRect->top )
	{
	tmp = lpDstRect->bottom;
	lpDstRect->bottom = lpDstRect->top;
	lpDstRect->top = tmp;
	}
}


//***********************************************************************/
//	Constrain the slelection to the AspectX&AspectY.
//	If bCenter then it will try to keep the center constant.
//	Otherwise it will keep the upper left corner constant.
//***********************************************************************/
void ConstrainSelection( LPRECT lpRect, long AspectX, long AspectY,
                           BOOL bCenter )
//***********************************************************************/
{
	int dx, dy, x, y;
	int sx, sy;
	POINT End, Start;
	
	if ( !AspectX || !AspectY )
		return;
	Start.x = lpRect->left;
	Start.y = lpRect->top;
	End.x = lpRect->right;
	End.y = lpRect->bottom;
	x = End.x - Start.x + 1;
	y = End.y - Start.y + 1;
	sx = (x>=0) ? 1:-1;
	sy = (y>=0) ? 1:-1;
	x = abs(x);
	y = abs(y);
	ScaleToFit(&x, &y, (int)AspectX, (int)AspectY);
	x *= sx;
	y *= sy;
	if (bCenter)
	{
		dx = (x-End.x+Start.x+1)/2;
		dy = (y-End.y+Start.y+1)/2;
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


/***********************************************************************/
VOID ConstrainHandles( POINT ptCurrent, LPRECT lpSelectRect )
/***********************************************************************/
{
int tmp;

if ( abs ( ptCurrent.y - lpSelectRect->bottom ) <= CLOSENESS )
	{
	if ( abs (ptCurrent.x - lpSelectRect->right ) <= CLOSENESS)
		Constrain = 0;
	else
	if ( abs (ptCurrent.x - lpSelectRect->left ) <= CLOSENESS)
		{
		Constrain = 0;
		tmp = lpSelectRect->left;
		lpSelectRect->left = lpSelectRect->right;
		lpSelectRect->right = tmp;
		}
	else
	if ( abs (ptCurrent.x - ( ( lpSelectRect->left + lpSelectRect->right+1)/2) ) <= CLOSENESS)
		Constrain ^= CONSTRAINY;
	}
else
if ( abs ( ptCurrent.y - lpSelectRect->top ) <= CLOSENESS )
	{
	if ( abs (ptCurrent.x - lpSelectRect->right ) <= CLOSENESS)
		Constrain = 0;
	else
	if ( abs (ptCurrent.x - lpSelectRect->left ) <= CLOSENESS)
		{
		Constrain = 0;
		tmp = lpSelectRect->left;
		lpSelectRect->left = lpSelectRect->right;
		lpSelectRect->right = tmp;
		}
	else
	if ( abs (ptCurrent.x - ( ( lpSelectRect->left + lpSelectRect->right+1)/2) ) <= CLOSENESS)
		Constrain ^= CONSTRAINY;
	if (Constrain != (CONSTRAINY | CONSTRAINX))
		{
		tmp = lpSelectRect->top;
		lpSelectRect->top = lpSelectRect->bottom;
		lpSelectRect->bottom = tmp;
		}
	}
else
if ( abs ( ptCurrent.x - lpSelectRect->right ) <= CLOSENESS )
	{
	if ( abs (ptCurrent.y - ( ( lpSelectRect->top + lpSelectRect->bottom+1)/2) ) <= CLOSENESS)
		Constrain ^= CONSTRAINX;
	}
else
if ( abs ( ptCurrent.x - lpSelectRect->left ) <= CLOSENESS )
	{
	if ( abs (ptCurrent.y - ( ( lpSelectRect->top + lpSelectRect->bottom+1)/2) ) <= CLOSENESS)
		{
		Constrain ^= CONSTRAINX;
		tmp = lpSelectRect->left;
		lpSelectRect->left = lpSelectRect->right;
		lpSelectRect->right = tmp;
		}
	}
}


/***********************************************************************/
static void DrawHandles( HDC hDC, LPRECT lpSelectRect )
/***********************************************************************/
{
RECT hRect, sRect;
HPEN hOldPen;

hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));

sRect.top = min(lpSelectRect->top, lpSelectRect->bottom);
sRect.bottom = max(lpSelectRect->top, lpSelectRect->bottom);
sRect.left = min(lpSelectRect->left, lpSelectRect->right);
sRect.right = max(lpSelectRect->left, lpSelectRect->right);

hRect.top = sRect.top - HSIZE;
hRect.bottom = sRect.top + HSIZE + 1;
hRect.left = sRect.left - HSIZE;
hRect.right = sRect.left + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = sRect.top - HSIZE;
hRect.bottom = sRect.top + HSIZE + 1;
hRect.left = sRect.right - HSIZE;
hRect.right = sRect.right + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = sRect.bottom - HSIZE;
hRect.bottom = sRect.bottom + HSIZE + 1;
hRect.left = sRect.right - HSIZE;
hRect.right = sRect.right + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = sRect.bottom - HSIZE;
hRect.bottom = sRect.bottom + HSIZE + 1;
hRect.left = sRect.left - HSIZE;
hRect.right = sRect.left + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = ((sRect.top+sRect.bottom+1)/2)-HSIZE;
hRect.bottom = ((sRect.top+sRect.bottom+1)/2)+HSIZE+1;
hRect.left = sRect.left - HSIZE;
hRect.right = sRect.left + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = sRect.top - HSIZE;
hRect.bottom = sRect.top + HSIZE + 1;
hRect.left = ((sRect.left+sRect.right+1)/2)-HSIZE;
hRect.right = ((sRect.left+sRect.right+1)/2)+HSIZE+1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = ((sRect.top+sRect.bottom+1)/2)-HSIZE;
hRect.bottom = ((sRect.top+sRect.bottom+1)/2)+HSIZE+1;
hRect.left = sRect.right - HSIZE;
hRect.right = sRect.right + HSIZE + 1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);
hRect.top = sRect.bottom - HSIZE;
hRect.bottom = sRect.bottom + HSIZE + 1;
hRect.left = ((sRect.left+sRect.right+1)/2)-HSIZE;
hRect.right = ((sRect.left+sRect.right+1)/2)+HSIZE+1;
PatBlt(hDC, hRect.left, hRect.top, hRect.right-hRect.left,
		hRect.bottom-hRect.top, DSTINVERT);

SelectObject(hDC, hOldPen);
}


/************************************************************************/
void CDisplayHook::DisplayHookProc( HWND hWnd, HDC hDC, LPRECT lpRect )
/************************************************************************/
{
InvertSelection( hWnd, hDC, NULL/*&SelectRect*/, 0/*Flags*/ );
if ( lpLastDisplayHook )
	lpLastDisplayHook->DisplayHookProc( hWnd, hDC, lpRect );
}

/************************************************************************/
static LPRECT GetBoundingRect( HWND hWnd )
/************************************************************************/
{
static RECT BoundRect;

if ( IsDoc(hWnd) )
	{
	LPIMAGE lpImage;

	lpImage = GetImagePtr(hWnd);
	BoundRect.left = BoundRect.top = 0;
	BoundRect.right = lpImage->npix-1;
	BoundRect.bottom = lpImage->nlin-1;
	}
else
	GetClientRect( hWnd, &BoundRect );
return( &BoundRect );
}


