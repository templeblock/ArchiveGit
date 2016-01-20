#include "stdafx.h"
#include "ctp.h"
#include "Selection.h"

#define StatusOfRectangle(pRect, fScale, iResolution)
#define MessageStatus(x)

//////////////////////////////////////////////////////////////////////
CSelection::CSelection()
{
}

//////////////////////////////////////////////////////////////////////
CSelection::~CSelection()
{
}

// Static prototypes
static void MoveInsideRect(RECT* pRect, RECT* lpBoundRect);
static void DrawHandles(HDC hDC, RECT* lpSelectRect);

static RECT* GetBoundingRect(HWND hWnd);
static void GetCenter(RECT* pRect, POINT* pPoint);

static int Constrain, Mode;
static POINT ptCenter;
static POINT ptAnchor;
static double fScale;

#define CONSTRAINX 1
#define CONSTRAINY 2
#define DRAG_MODE 1
#define MOVE_MODE 2
#define HSIZE 2

//////////////////////////////////////////////////////////////////////
void CSelection::StartSelection(HWND hWnd, HDC hDC, RECT* lpSelectRect, int fFlags, POINT ptCurrent, double fStatusScale)
{
	int minx, maxx, miny, maxy;
	RECT* pRect;
	bool fDoc = false;

	if (fDoc)
	{
//j		lpLastDisplayHook = SetDisplayHook(hWnd, &DisplayHook);
//j		Display2File(hWnd, (LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y);
	}
	else
//j	if ((fFlags & SL_TYPE) == SL_GRID)
//j	{
//j		Control2Image((LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y);
//j	}

	fScale = fStatusScale;

	if (!(fFlags & SL_NOLIMIT))
	{
//j		RECT rBound;
//j		if ((fFlags & SL_TYPE) == SL_GRID)
//j			pRect = GetGridBoundRect(&rBound);
//j		else
			pRect = GetBoundingRect(hWnd);
		ptCurrent.x = bound(ptCurrent.x, pRect->left, pRect->right);
		ptCurrent.y = bound(ptCurrent.y, pRect->top, pRect->bottom);
	}

	ptAnchor = ptCurrent;

	Mode = DRAG_MODE;
	if (fFlags & SL_SPECIAL)
	{ // Setup a selection rectangle at the current location
		Constrain = 0;
		lpSelectRect->left = ptCurrent.x;
		lpSelectRect->top = ptCurrent.y;
		lpSelectRect->right = ptCurrent.x;
		lpSelectRect->bottom = ptCurrent.y;
		InvertSelection(hWnd, hDC, lpSelectRect, fFlags);
		GetCenter(lpSelectRect, &ptCenter);
		return;
	}

	// Modify the existing selection rectangle
	// First clip it to the window's client area
	if (!(fFlags & SL_NOLIMIT))
	{
//j		if ((fFlags & SL_TYPE) == SL_GRID)
//j		 	IntersectRect(lpSelectRect, lpSelectRect, GetGridBoundRect(&rBound));
//j		else
			IntersectRect(lpSelectRect, lpSelectRect, GetBoundingRect(hWnd));
	}

	// Figure out which side or corner to drag
	// The bottom right point is always the one that moves
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
			if (abs(ptCurrent.y - lpSelectRect->bottom) <= CLOSENESS)
				Constrain ^= CONSTRAINY;
			else
			if (abs(ptCurrent.y - lpSelectRect->top) <= CLOSENESS)
			{
				Constrain ^= CONSTRAINY;
				int tmp = lpSelectRect->top;
				lpSelectRect->top = lpSelectRect->bottom;
				lpSelectRect->bottom = tmp;
			}
			if (abs(ptCurrent.x - lpSelectRect->right) <= CLOSENESS)
				Constrain ^= CONSTRAINX;
			else
			if (abs(ptCurrent.x - lpSelectRect->left) <= CLOSENESS)
			{
				Constrain ^= CONSTRAINX;
				int tmp = lpSelectRect->left;
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
//j		LPIMAGE lpImage = GetImagePtr(hWnd);
//j		DisplayInfo(ptCurrent.x, ptCurrent.y, lpSelectRect);
//j		StatusOfRectangle(lpSelectRect, fScale, FrameResolution(ImgGetBaseEditFrame(lpImage)));
	}
	else
		StatusOfRectangle(lpSelectRect, fScale, 1000);
}

//////////////////////////////////////////////////////////////////////
void CSelection::UpdateSelection(HWND hWnd, HDC hDC, RECT* lpSelectRect, int fFlags, POINT ptCurrent, bool bConstrain, long AspectX, long AspectY, bool bMove, bool bFromCenter)
{
	RECT* pRect;
	bool fNeedDC;
	int dx, dy;
	POINT ptSaved;
	RECT OldRect, drect;
	bool fDoc = false;

	if (Mode == MOVE_MODE)
		bMove = true;
	else
	if (Constrain == (CONSTRAINX | CONSTRAINY)) // It hasn't been started yet
		StartSelection(hWnd, hDC, lpSelectRect, fFlags, ptCurrent, fScale);

	if (fDoc)
	{
//j		Display2File(hWnd, (LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y);
	}
//j	else
//j	if ((fFlags & SL_TYPE) == SL_GRID)
//j		Control2Image((LPINT)&ptCurrent.x, (LPINT)&ptCurrent.y);

	if (!(fFlags & SL_NOLIMIT))
	{ // Constrain point to window client area
//j		RECT rBound;
//j		if ((fFlags & SL_TYPE) == SL_GRID)
//j			pRect = GetGridBoundRect(&rBound);
//j		else
			pRect = GetBoundingRect(hWnd);
		ptCurrent.x = bound(ptCurrent.x, pRect->left, pRect->right);
		ptCurrent.y = bound(ptCurrent.y, pRect->top, pRect->bottom);
	}

	ptSaved = ptCurrent;

	if (!bMove)
	{
		// Constrain point in x-direction based on where grabbed
		if (Constrain & CONSTRAINX)
			ptCurrent.x = lpSelectRect->right;

		// Constrain point in y-direction based on where grabbed
		if (Constrain & CONSTRAINY)
			ptCurrent.y = lpSelectRect->bottom;
	}

	if (fNeedDC = (!hDC))
		hDC = GetDC(hWnd);

	switch (fFlags & SL_TYPE)
	{
		case SL_BOX:
		case SL_BOXHANDLES:
		case SL_GRID:
		case SL_LINE:
		case SL_ELLIPSE:
		{
			InvertSelection(hWnd, hDC, lpSelectRect, fFlags);
			if (bMove)
			{
				dx = ptCurrent.x - ptAnchor.x;
				dy = ptCurrent.y - ptAnchor.y;
				lpSelectRect->right  += dx;
				lpSelectRect->bottom += dy;
				lpSelectRect->top	 += dy;
				lpSelectRect->left	 += dx;
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
				lpSelectRect->left	 = ptCenter.x - dx/2;
				lpSelectRect->top	 = ptCenter.y - dy/2;
				lpSelectRect->right  = ptCenter.x + dx - dx/2;
				lpSelectRect->bottom = ptCenter.y + dy - dy/2;
			}
			GetCenter(lpSelectRect, &ptCenter);
			// Keep the new rectangle inside the window's client area
			if (!(fFlags & SL_NOLIMIT))
			{
//j				if ((fFlags & SL_TYPE) == SL_GRID)
//j					MoveInsideRect(lpSelectRect, GetGridBoundRect(&rBound));
//j				else
					MoveInsideRect(lpSelectRect, GetBoundingRect(hWnd));
			}
			InvertSelection(hWnd, hDC, lpSelectRect, fFlags);
			break;
		}
		case SL_BLOCK:
		{
			OldRect = *lpSelectRect;
			dx = ptCurrent.x - lpSelectRect->right;
			dy = ptCurrent.y - lpSelectRect->bottom;
			lpSelectRect->right = ptCurrent.x;
			lpSelectRect->bottom = ptCurrent.y;
			if (bConstrain)
				ConstrainSelection(lpSelectRect, AspectX, AspectY, bFromCenter);
			if (bFromCenter)
			{
				lpSelectRect->left	 = ptCenter.x - dx/2;
				lpSelectRect->top	 = ptCenter.y - dy/2;
				lpSelectRect->right  = ptCenter.x + dx - dx/2;
				lpSelectRect->bottom = ptCenter.y + dy - dy/2;
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
	}

	ptAnchor = ptSaved;
	if (fDoc)
	{
//j		LPIMAGE lpImage = GetImagePtr(hWnd);
//j		DisplayInfo(ptCurrent.x, ptCurrent.y, lpSelectRect);
//j		StatusOfRectangle(lpSelectRect, fScale, FrameResolution(ImgGetBaseEditFrame(lpImage)));
	}
	else
		StatusOfRectangle(lpSelectRect, fScale, 1000);
	if (fNeedDC)
		ReleaseDC(hWnd, hDC);
}

//////////////////////////////////////////////////////////////////////
static void GetCenter(RECT* pRect, POINT* pPoint)
{
	int dx, dy;

	dx = pRect->right - pRect->left;
	dy = pRect->bottom - pRect->top;
	// choose new center
	pPoint->x = pRect->left + dx/2;
	pPoint->y = pRect->top + dy/2;
}


//////////////////////////////////////////////////////////////////////
static void MoveInsideRect(RECT* pRect, RECT* lpBoundRect)
{ // Assumes that lpBoundRect is Ordered and that pRect may not be
// Also assumes that pRect fits within lpBundRect
	int x, y, dx, dy, l, r, t, b;

	if ((l = pRect->left) > (r = pRect->right))
		{ x = l; l = r; r = x; }
	if ((x = lpBoundRect->left - l) > 0) // too far left
		dx = x; // push it to right
	else
	if ((x = lpBoundRect->right - r) < 0) // too far right
		dx = x; // push it to left
	else
		dx = 0;

	if ((t = pRect->top) > (b = pRect->bottom))
		{ y = t; t = b; b = y; }
	if ((y = lpBoundRect->top - t) > 0) // too far up
		dy = y; // push it down
	else
	if ((y = lpBoundRect->bottom - b) < 0) // too far down
		dy = y; // push it up
	else
		dy = 0;

	OffsetRect(pRect, dx, dy);
}


//////////////////////////////////////////////////////////////////////
void CSelection::InvertSelection(HWND hWnd, HDC hDC, RECT* pRect, int fFlags)
{
	short OldROP;
	bool fNeedDC;
	RECT SelectRect;
	static RECT LastSelectRect;
	static int LastFlags;
	bool fDoc = false;
	int type;

	if (pRect->right - pRect->left > 50)
		int i = 0;

	if (fFlags && pRect)
	{ // Saved for the display hook
		LastSelectRect = *pRect;
		LastFlags = fFlags;
	}
	else
	{ // Called from the display hook
		fFlags = LastFlags;
	}

	type = fFlags & SL_TYPE;
	SelectRect = LastSelectRect;
	if (fDoc)
	{
//j		if (type == SL_LINE)
//j		{
//j			File2DisplayEx(hWnd, (LPINT)&SelectRect.left, (LPINT)&SelectRect.top, true);
//j			File2DisplayEx(hWnd, (LPINT)&SelectRect.right, (LPINT)&SelectRect.bottom, true);
//j		}
//j		else
//j		{
//j			OrderRect(&SelectRect, &SelectRect);
//j			File2DispRectExact(hWnd, &SelectRect, &SelectRect); 
//j		}
	}

	if (fNeedDC = (!hDC))
		hDC = GetDC(hWnd);

	switch (type)
	{
		case SL_BOX:
		{
			OldROP = SetROP2(hDC, R2_NOT);
			MoveToEx(hDC, SelectRect.left, SelectRect.top, NULL);
			LineTo(hDC, SelectRect.right, SelectRect.top);
			LineTo(hDC, SelectRect.right, SelectRect.bottom);
			LineTo(hDC, SelectRect.left, SelectRect.bottom);
			LineTo(hDC, SelectRect.left, SelectRect.top);
			SetROP2(hDC, OldROP);
			break;
		}

		case SL_BOXHANDLES:
		{
			OldROP = SetROP2(hDC, R2_NOT);
			MoveToEx(hDC, SelectRect.left, SelectRect.top, NULL);
			LineTo(hDC, SelectRect.right, SelectRect.top);
			LineTo(hDC, SelectRect.right, SelectRect.bottom);
			LineTo(hDC, SelectRect.left, SelectRect.bottom);
			LineTo(hDC, SelectRect.left, SelectRect.top);
			SetROP2(hDC, OldROP);
			DrawHandles(hDC, &SelectRect);
			break;
		}

	#ifdef NOTUSED //j
		case SL_GRID:
		{
			RECT ClientRect;
			int x, y, dx, dy, cx, cy;

			OldROP = SetROP2(hDC, R2_NOT);
			GetClientRect(hWnd, &ClientRect);
			dx = abs(RectWidth(&SelectRect));
			dy = abs(RectHeight(&SelectRect));
			if (dx > CLOSENESS && dy > CLOSENESS)
			{
				x = min(SelectRect.left, SelectRect.right);
				cx = x;
				Image2Control(&cx, &cy);
				while (cx >= ClientRect.left)
				{
					MoveToEx(hDC, cx, ClientRect.top, NULL);
					LineTo(hDC, cx, ClientRect.bottom);
					x -= dx;
					cx = x;
					Image2Control(&cx, &cy);
				}
				x = max(SelectRect.left, SelectRect.right);
				cx = x;
				Image2Control(&cx, &cy);
				while (cx <= ClientRect.right)
				{
					MoveToEx(hDC, cx, ClientRect.top, NULL);
					LineTo(hDC, cx, ClientRect.bottom);
					x += dx;
					cx = x;
					Image2Control(&cx, &cy);
				}
				y = min(SelectRect.top, SelectRect.bottom);
				cy = y;
				Image2Control(&cx, &cy);
				while (cy >= ClientRect.top)
				{
					MoveToEx(hDC, ClientRect.left, cy, NULL);
					LineTo(hDC, ClientRect.right, cy);
					y -= dy;
					cy = y;
					Image2Control(&cx, &cy);
				}
				y = max(SelectRect.top, SelectRect.bottom);
				cy = y;
				Image2Control(&cx, &cy);
				while (cy <= ClientRect.bottom)
				{
					MoveToEx(hDC, ClientRect.left, cy, NULL);
					LineTo(hDC, ClientRect.right, cy);
					y += dy;
					cy = y;
					Image2Control(&cx, &cy);
				}
			}
			else
			{
				Image2Control((LPINT)&SelectRect.left, (LPINT)&SelectRect.top);
				Image2Control((LPINT)&SelectRect.right, (LPINT)&SelectRect.bottom);
				MoveToEx(hDC, SelectRect.left, SelectRect.top, NULL);
				LineTo(hDC, SelectRect.right, SelectRect.top);
				LineTo(hDC, SelectRect.right, SelectRect.bottom);
				LineTo(hDC, SelectRect.left, SelectRect.bottom);
				LineTo(hDC, SelectRect.left, SelectRect.top);
			}
			SetROP2(hDC, OldROP);
			break;
		}
	#endif NOTUSED //j

		case SL_LINE:
		{
			OldROP = SetROP2(hDC, R2_NOT);
			MoveToEx(hDC, SelectRect.left, SelectRect.top, NULL);
			LineTo(hDC, SelectRect.right, SelectRect.bottom);
			SetROP2(hDC, OldROP);
			break;
		}

		case SL_ELLIPSE:
		{
			OldROP = SetROP2(hDC, R2_NOT);
			HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));
			Ellipse(hDC, SelectRect.left, SelectRect.top,
				SelectRect.right, SelectRect.bottom);
			SetROP2(hDC, OldROP);
			SelectObject(hDC, hOldBrush);
			break;
		}

		case SL_BLOCK:
		{
			PatBlt(hDC, SelectRect.left, SelectRect.top,
				SelectRect.right - SelectRect.left,
				SelectRect.bottom - SelectRect.top, DSTINVERT);
			break;
		}
	}

	if (fNeedDC)
		ReleaseDC(hWnd, hDC);
}


//////////////////////////////////////////////////////////////////////
void CSelection::EndSelection(HWND hWnd, HDC hDC, RECT* lpSelectRect, int fFlags, bool bRemove)
{
	bool fDoc = false;

	if (bRemove)
		InvertSelection(hWnd, hDC, lpSelectRect, fFlags);
	if (fDoc)
	{
//j		SetDisplayHook(hWnd, lpLastDisplayHook);
	}
	if (fDoc)
	{
//j		DisplayInfo(lpSelectRect->right, lpSelectRect->bottom, NULL);
	}
	OrderRect(lpSelectRect, lpSelectRect);
	MessageStatus(0);
}


//////////////////////////////////////////////////////////////////////
void CSelection::OrderRect(RECT* lpSrcRect, RECT* lpDstRect)
{
	*lpDstRect = *lpSrcRect;
	if (lpDstRect->right < lpDstRect->left)
	{
		int tmp = lpDstRect->right;
		lpDstRect->right = lpDstRect->left;
		lpDstRect->left = tmp;
	}
	if (lpDstRect->bottom < lpDstRect->top)
	{
		int tmp = lpDstRect->bottom;
		lpDstRect->bottom = lpDstRect->top;
		lpDstRect->top = tmp;
	}
}


//////////////////////////////////////////////////////////////////////
//	Constrain the selection to the AspectX & AspectY.
//	If bCenter then it will try to keep the center constant.
//	Otherwise it will keep the upper left corner constant.
//////////////////////////////////////////////////////////////////////
void CSelection::ConstrainSelection(RECT* pRect, long AspectX, long AspectY, bool bCenter)
{
	if (!AspectX || !AspectY)
		return;

	int x = WIDTH(*pRect);
	int y = HEIGHT(*pRect);
	int sx = (x >= 0 ? 1 : -1);
	int sy = (y >= 0 ? 1 : -1);
	x = abs(x);
	y = abs(y);
	ScaleToFit(&x, &y, (int)AspectX, (int)AspectY, true/*bUseSmallerFactor*/);
	x *= sx;
	y *= sy;
	if (bCenter)
	{
		pRect->left -= ((x - WIDTH(*pRect)) / 2);
		pRect->top  -= ((y - HEIGHT(*pRect)) / 2);
	}

	pRect->right  = pRect->left + x;
	pRect->bottom = pRect->top  + y;
}


//////////////////////////////////////////////////////////////////////
void CSelection::ConstrainHandles(POINT ptCurrent, RECT* lpSelectRect)
{
	if (abs (ptCurrent.y - lpSelectRect->bottom) <= CLOSENESS)
	{
		if (abs (ptCurrent.x - lpSelectRect->right) <= CLOSENESS)
			Constrain = 0;
		else
		if (abs (ptCurrent.x - lpSelectRect->left) <= CLOSENESS)
		{
			Constrain = 0;
			int tmp = lpSelectRect->left;
			lpSelectRect->left = lpSelectRect->right;
			lpSelectRect->right = tmp;
		}
		else
		if (abs (ptCurrent.x - ((lpSelectRect->left + lpSelectRect->right+1)/2)) <= CLOSENESS)
			Constrain ^= CONSTRAINY;
	}
	else
	if (abs (ptCurrent.y - lpSelectRect->top) <= CLOSENESS)
	{
		if (abs (ptCurrent.x - lpSelectRect->right) <= CLOSENESS)
			Constrain = 0;
		else
		if (abs (ptCurrent.x - lpSelectRect->left) <= CLOSENESS)
		{
			Constrain = 0;
			int tmp = lpSelectRect->left;
			lpSelectRect->left = lpSelectRect->right;
			lpSelectRect->right = tmp;
		}
		else
		if (abs (ptCurrent.x - ((lpSelectRect->left + lpSelectRect->right+1)/2)) <= CLOSENESS)
			Constrain ^= CONSTRAINY;
		if (Constrain != (CONSTRAINY | CONSTRAINX))
		{
			int tmp = lpSelectRect->top;
			lpSelectRect->top = lpSelectRect->bottom;
			lpSelectRect->bottom = tmp;
		}
	}
	else
	if (abs (ptCurrent.x - lpSelectRect->right) <= CLOSENESS)
	{
		if (abs (ptCurrent.y - ((lpSelectRect->top + lpSelectRect->bottom+1)/2)) <= CLOSENESS)
			Constrain ^= CONSTRAINX;
	}
	else
	if (abs (ptCurrent.x - lpSelectRect->left) <= CLOSENESS)
	{
		if (abs (ptCurrent.y - ((lpSelectRect->top + lpSelectRect->bottom+1)/2)) <= CLOSENESS)
		{
			Constrain ^= CONSTRAINX;
			int tmp = lpSelectRect->left;
			lpSelectRect->left = lpSelectRect->right;
			lpSelectRect->right = tmp;
		}
	}
}


//////////////////////////////////////////////////////////////////////
static void DrawHandles(HDC hDC, RECT* lpSelectRect)
{
	RECT hRect, sRect;
	HPEN hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));

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

//////////////////////////////////////////////////////////////////////
static RECT* GetBoundingRect(HWND hWnd)
{
	static RECT BoundRect;
	bool fDoc = false;

	if (fDoc)
	{
//j		LPIMAGE lpImage = GetImagePtr(hWnd);
//j		BoundRect.left = BoundRect.top = 0;
//j		BoundRect.right = lpImage->npix;
//j		BoundRect.bottom = lpImage->nlin;
	}
	else
		GetClientRect(hWnd, &BoundRect);

	return &BoundRect;
}
