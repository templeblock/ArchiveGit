// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "pointutl.h"

#define HALF_HANDLE_SIZE 3

#define SE_SELECTED	0x8000

#define MAXDEPTH 12

// what does 'moved' mean?
#define CLOSE(x1,y1,x2,y2) \
	(abs((x1)-(x2)) < 4 && abs((y1)-(y2)) < 4)

#define PTEQ(p1, p2) \
	((p1).x == (p2).x && (p1).y == (p2).y)
	
#define StartInvert(hDC, bOn) \
	SetROP2( hDC, R2_NOT )
#define EndInvert(hDC, iOldRop) \
	SetROP2( hDC, iOldRop )

	
// LOCALS For 'ForEach' functions
LOCAL BOOL copy_edit_poly_list(LPCEditPoly lpBasePoly, LPCEditPolyList lpPolyList);
LOCAL BOOL close_poly(LPCPoly lpPoly, LONG bClosed);

LOCAL void make_curve(LPCEditPoly lpPoly);
LOCAL void draw_handle(LPPOINT lpPt, HDC hDC, BOOL on, BOOL bSel);
LOCAL void draw_bez_handle(LPPOINT lpPt, HDC hDC, BOOL on);
LOCAL BOOL setup_display(LPTFORM FAR *lppOutTForm, LPTFORM lpInTForm, HDC FAR* lpHDC, HWND hWnd);
LOCAL void make_curve(LPCEditPoly lpPoly);
LOCAL void make_corner(LPCEditPoly lpPoly);
LOCAL BOOL is_linear(LPPOINT p1, LPPOINT p2, LPPOINT p3);
LOCAL void rotate_point(LPPOINT lpOut, LPPOINT lpIn, LPPOINT lpOld,LPPOINT lpNew,LPPOINT lpBase);
LOCAL WORD is_handle(LPCPolyList lpPolyList, LPSEPOS lpPos);
LOCAL WORD is_selected(LPCPolyList lpPolyList, LPSEPOS lpPos);
LOCAL void move_range(LPCShapeEdit lpShapeEdit, LPCEditPoly lpPoly, int from, int to, int dx, int dy, int bMoveHandles);
LOCAL int delete_poly_point(LPCEditPoly lpPoly);
LOCAL int forward_whole(LPCPoly lpPoly, int index);
LOCAL int backup_whole(LPCPoly lpPoly, int index);
LOCAL BOOL get_delayed_render(LPCShapeEdit lpEditShape, LPSERENDER lpRender, LPCEditPoly lpPoly, int from, int to, LPTFORM lpTForm = NULL);
LOCAL void do_delayed_render(LPCShapeEdit lpEditShape, LPSERENDER lpRender, BOOL on, HDC hDC = NULL);
LOCAL BOOL fill_render(LPSERENDER lpRender);
LOCAL void free_render(LPSERENDER lpRender);

//=======================================================================
//=======================================================================
CShapeEdit::CShapeEdit()
//=======================================================================
{
	hWnd = NULL;
	lpPolyList = NULL;
	Position = NULL;
	bClosed = FALSE;
	Function = IDC_MOVEPOINTS;
	bLDown = FALSE;
	bSelect = FALSE;
	RenderOff.lpBaseLines = NULL;
	RenderOff.iMaxNumLines = 0;
	RenderOff.lpHandles = NULL;
	RenderOff.iMaxNumHandles = 0;
	RenderOff.lpBezPairs = NULL;
	RenderOff.iMaxNumBezPairs = 0;
	RenderOn.lpBaseLines = NULL;
	RenderOn.iMaxNumLines = 0;
	RenderOn.lpHandles = NULL;
	RenderOn.iMaxNumHandles = 0;
	RenderOn.lpBezPairs = NULL;
	RenderOn.iMaxNumBezPairs = 0;
}

//=======================================================================
//=======================================================================
CShapeEdit::~CShapeEdit()	
//=======================================================================
{
	if (lpPolyList)
		FreePolyList(lpPolyList);
	lpPolyList = NULL;
	// must end selection to restore display hook
	if (bSelect)
		EndSelection(hWnd, NULL, &rSelect, SL_BOX, YES);
	bSelect = FALSE;
	hWnd = NULL;
	Position = NULL;
	bClosed = FALSE;
	bLDown = FALSE;
	free_render(&RenderOff);
	free_render(&RenderOn);
}


//=======================================================================
//	BOOL CShapeEdit::Init(HWND hWnd, LPCPoly lpPoly)
//=======================================================================
BOOL CShapeEdit::Init(HWND hWindow, LPCPoly lpPoly)
//=======================================================================
{
	LPCEditPoly lpNewPoly;
	hWnd = hWindow;
	// free any existing poly list
	if (lpPolyList)
		FreePolyList(lpPolyList);
	// create a new poly list
	if (!(lpPolyList = new CEditPolyList)) goto ExitMemory;
	// create polygon to work on
	if (!(lpNewPoly = new CEditPoly))  goto ExitMemory;
	if (!(Position = lpPolyList->AddTail(lpNewPoly)))
	{
		delete(lpNewPoly);
		goto ExitMemory;
	}
	// copy polygon
	if (lpPoly)
		if (!lpPoly->Copy(lpNewPoly))
			 goto ExitMemory;
	
	if (!RenderOff.iMaxNumHandles)
	{
		if (!fill_render(&RenderOff))
			 goto ExitMemory;
			
	}
	if (!RenderOn.iMaxNumHandles)
		if (!fill_render(&RenderOn))
			 goto ExitMemory;
	
	return(TRUE);

ExitMemory:
	this->~CShapeEdit();
	return(FALSE);
}

//=======================================================================
//	BOOL CShapeEdit::Init(HWND hWnd, LPCPolyList lpList)
//=======================================================================
BOOL CShapeEdit::Init(HWND hWindow, LPCPolyList lpList)
//=======================================================================
{
	LPCEditPoly lpNewPoly;
	
	hWnd = hWindow;
	// free any existing poly list
	if (lpPolyList)
		FreePolyList(lpPolyList);
	// create a new poly list
	if (!(lpPolyList = new CEditPolyList)) goto ExitMemory;
	// add polygons
	if (lpList && !lpList->IsEmpty())
	{
		// Copy Poly's
		if (!lpList->ForEach1((LPLISTPROC1)copy_edit_poly_list, (LONG)lpPolyList))
			goto ExitMemory;
		if (!(Position = lpPolyList->GetTailPosition()))
			goto ExitMemory;
	}
	else
	{
		// create New polygon to work on
		if (!(lpNewPoly = new CEditPoly))  goto ExitMemory;
		if (!(Position = lpPolyList->AddTail(lpNewPoly)))
		{
			delete(lpNewPoly);
			goto ExitMemory;
		}
	}
	
	if (!RenderOff.iMaxNumHandles)
	{
		if (!fill_render(&RenderOff))
			 goto ExitMemory;
			
	}
	if (!RenderOn.iMaxNumHandles)
		if (!fill_render(&RenderOn))
			 goto ExitMemory;
	
	
	return(TRUE);
	

ExitMemory:
	this->~CShapeEdit();
	return(FALSE);
}

//=======================================================================
LOCAL BOOL copy_edit_poly_list(LPCEditPoly lpBasePoly, LPCEditPolyList lpPolyList)
//=======================================================================
{
	LPCEditPoly lpNewPoly;
	
	if (!(lpNewPoly = new CEditPoly))
		return(FALSE);
	if (!lpPolyList->AddTail(lpNewPoly))
	{
		delete(lpNewPoly);
		return(FALSE);
	}
	if (!lpBasePoly->Copy(lpNewPoly))
		return(FALSE);
	return(TRUE);
}


//=======================================================================
//	void CShapeEdit::CloseShape(BOOL bClosed)
//=======================================================================
void CShapeEdit::CloseShape(BOOL bClosed)
//=======================================================================
{
	if (lpPolyList)
		lpPolyList->ForEach1((LPLISTPROC1)close_poly, (LONG)bClosed);
	this->bClosed = bClosed;
}

//=======================================================================
LOCAL BOOL close_poly(LPCPoly lpPoly, LONG bClosed)
//=======================================================================
{
	lpPoly->ClosePoly(bClosed);
	return(TRUE);
}

//=======================================================================
//	LPCPolyList CShapeEdit::Done()
//	
//		Retunrs a poly list which was the result of the editing.
//		If there are no points it returns NULL.
//		It is up to the caller to free the PolyList and all the Polys.
//=======================================================================
LPCPolyList CShapeEdit::Done()
//=======================================================================
{
	LPCPoly lpPoly;
	LPCEditPolyList lpList;
	
	if (!lpPolyList)
		return(NULL);
	if (lpPolyList->IsEmpty())
		goto ExitNone;
	if (lpPolyList->GetCount() > 1)
		goto ExitOk;
	if (!(lpPoly = (LPCEditPoly)lpPolyList->GetHeadPtr()))
		goto ExitNone;
	if (lpPoly->NumPoints() < 1)
		goto ExitNone;
ExitOk:
	lpList = lpPolyList;
	lpPolyList = NULL;
	this->~CShapeEdit();
	return(lpList);

ExitNone:
	this->~CShapeEdit();
	return(NULL);
}


//************************************************************************
// BOOL CShapeEdit::AddPolyList(LPCPolyList lpPolyList)
//
// DESCRIPTION:
//		Adds the polygons in lpPolyList to the shape edit structure.
//		The shape edit structure should be initialized.
// RETURNS:
//		FALSE if there is a failure.
//************************************************************************
BOOL CShapeEdit::AddPolyList(LPCPolyList lpList)
//************************************************************************
{	
	if (!lpPolyList || !lpList || lpList->IsEmpty())
		return(FALSE);
	
	// Undraw
	Draw(NULL, OFF);

	// add polygons
	// Copy Poly's
	if (!lpList->ForEach1((LPLISTPROC1)copy_edit_poly_list, (LONG)lpPolyList))
	{
		// Draw
		Draw(NULL, ON);
		return(FALSE);
	}

	// Draw
	Draw(NULL, ON);
	return(TRUE);
}


//************************************************************************
//	LPCEditPolyList CShapeEdit::PeekPolyList()
// DESCRIPTION:
//			Returns the current poly list.  
//			This list SHOULD NOT BE MODIFIED!
// RETURNS:
//			The current poly list.
//************************************************************************
LPCEditPolyList CShapeEdit::PeekPolyList()
//************************************************************************
{
	return(lpPolyList);
}


//=======================================================================
//	VOID CShapeEdit::SetBaseFunction(int function)
//=======================================================================
VOID CShapeEdit::SetBaseFunction(int iFunction)
//=======================================================================
{
	Function = iFunction;
}

//=======================================================================
//	int CShapeEdit::GetExtendedFunction()
//=======================================================================
int CShapeEdit::GetExtendedFunction()
//=======================================================================
{
	BOOL control = CONTROL;
	BOOL shift = SHIFT;
	int newFunction = Function;

	switch (Function)
	{
		case IDC_MOVEPOINTS:
			if (control)
				newFunction = IDC_DELPOINTS;
			else if (shift)
				newFunction = IDC_ADDPOINTS;
		break;
		case IDC_ADDPOINTS:
			if (control)
				newFunction = IDC_DELPOINTS;
			else if (shift)
				newFunction = IDC_MOVEPOINTS;
		break;			 
		case IDC_DELPOINTS:
			if (shift)
				newFunction = IDC_ADDPOINTS;
			else if (control)
				newFunction = IDC_MOVEPOINTS;
		break;
	}
	return(newFunction);
}

//=======================================================================
//	VOID CShapeEdit::MouseDown(int x, int y)
//=======================================================================
VOID CShapeEdit::MouseDown(int x, int y)
//=======================================================================
{
	POINT pt;
	BOOL bLineHit;
	
	// setup common point down vars
   	bLDown = TRUE;
	bSelect = FALSE;
	bDoSelect = FALSE;
	bMoveOnePoint = FALSE;
	ptDown.x = x;
	ptDown.y = y;
	ptLast = ptDown;
	bPointHit = find_hit(x,y, &PointHit);
	
	// do function specific stuff
	switch (downFunction = GetExtendedFunction())
	{
		case IDC_MOVEPOINTS:
			if (bPointHit)
			{
				if (is_handle(lpPolyList, &PointHit))
					start_move_handle(&PointHit);
				else if (!is_selected(lpPolyList, &PointHit))
				{
					select_point(&PointHit, ON, FALSE);
					bMoveOnePoint = TRUE;
				}
				else 
					bDoSelect = TRUE;	
				break;
			}
			bDoSelect = TRUE;	
		break;	
		case IDC_DELPOINTS:
			if (bPointHit)
			{
				if (is_handle(lpPolyList, &PointHit))
				{
					start_move_handle(&PointHit);
					downFunction = IDC_MOVEPOINTS;
				}
				else 
				{
					delete_point(&PointHit);
					bLDown = FALSE;
				}
			}
		break;	
		case IDC_ADDPOINTS:
			// are we on a point?
			bLineHit = FALSE;
			if (!bPointHit)
				bLineHit = find_close_line(x, y, &PointHit);
			if (bLineHit)
			{
				// We have add point
				bPointHit = TRUE;
				// add a point
				pt = ptDown;
				Display2File(hWnd, (LPINT)&pt.x, (LPINT)&pt.y);
				add_point(&pt, &PointHit);
				select_point(&PointHit, ON, FALSE);
				bMoveOnePoint = TRUE;
				break;
			}
			// really move points with shift (could still be an add...thanks Craig)
			downFunction = IDC_MOVEPOINTS;
			if (bPointHit && is_handle(lpPolyList, &PointHit))
				start_move_handle(&PointHit);
			else if (bPointHit)
				downFunction = IDC_MOVEADD;
			else
				bDoSelect = TRUE;
		break;
		default:
			bLDown = FALSE;
		break;
	}
}


//=======================================================================
//	VOID CShapeEdit::MouseMove(int x, int y)
//=======================================================================
VOID CShapeEdit::MouseMove(int x, int y)
//=======================================================================
{
	POINT pt, delta; 
	int fn;
	
	if (!bLDown)
		return;
	if (bSelect)
	{
		pt.x = x;
		pt.y = y;
		UpdateSelection( hWnd, NULL, &rSelect, SL_BOX,
				pt, NO, 1L, 1L, MOVEKEY||Window.fRButtonDown,NO);
		return;
	
	}
	switch (downFunction)
	{
		case IDC_MOVEADD:
			if (!CLOSE(x,y, ptDown.x, ptDown.y))
			{
				pt.x = x;
				pt.y = y;
				Display2File(hWnd, (LPINT)&pt.x, (LPINT)&pt.y);
				if (!add_point(&pt, &PointHit))
					bLDown = FALSE;
				else	
					select_point(&PointHit, ON, FALSE);
				bMoveOnePoint = TRUE;
				downFunction = IDC_ADDPOINTS;
			}
		break;
		case IDC_ADDPOINTS:
		case IDC_MOVEPOINTS:
			if (bPointHit)
			{
				// determine amount to move
				delta.x = x;
				delta.y = y;
				pt = ptLast;
				ptLast = delta;
				Display2File(hWnd, (LPINT)&pt.x, (LPINT)&pt.y);
				Display2File(hWnd, (LPINT)&delta.x, (LPINT)&delta.y);
				delta.x -= pt.x;
				delta.y -= pt.y;
				if (!delta.x && !delta.y)
					break;
				// move point(s)
				if (Window.fRButtonDown)
				{
					move_shape(PointHit.pos, delta.x, delta.y, TRUE);
					if (is_handle(lpPolyList, &PointHit))
						start_move_handle(&PointHit);
				}
				else if (is_handle(lpPolyList, &PointHit))
					move_handle(delta.x, delta.y, !SHIFT, &PointHit);
				else if (bMoveOnePoint)
					move_point(delta.x, delta.y, TRUE, &PointHit);
				else
					move_selected(delta.x, delta.y, TRUE);
				break;
			}
			// dont select on add points
			if (fn == IDC_ADDPOINTS)
				break;
		// Fall through if just going to drag a rect
		case IDC_DELPOINTS:
			// have we move enough?
			if (!CLOSE(x,y, ptDown.x, ptDown.y))
			{
				// begin selection 
				bSelect = TRUE;
				pt.x = x;
				pt.y = y;
				StartSelection( hWnd, NULL, &rSelect, SL_BOX | SL_SPECIAL,
					pt, 0L );
			}
		break;
		default:
		break;
	}
}

				
//=======================================================================
//	VOID CShapeEdit::MouseUp(int x, int y)
//=======================================================================
VOID CShapeEdit::MouseUp(int x, int y)
//=======================================================================
{
	BOOL bMoved;
	BOOL bGotRect = FALSE;
	
	if (bSelect)
	{
		// selection made
		EndSelection(hWnd, NULL, &rSelect, SL_BOX, YES);
		bSelect = FALSE;
		bGotRect = TRUE;
	}
	
	if (!bLDown)
		return;
	switch (downFunction)
	{
		case IDC_MOVEADD:
			// unsucessful move/add
			bDoSelect = TRUE;
		// Fall through
		case IDC_MOVEPOINTS:
			if (!bDoSelect)
				break;
			if (bPointHit && is_handle(lpPolyList, &PointHit))
				break;
			bMoved = !CLOSE(x,y, ptDown.x, ptDown.y);
			if (bGotRect)
				select_rect(&rSelect, SHIFT);
			else if (bPointHit && !bMoved)
			{
				select_point(&PointHit, SHIFT ? MAYBE:YES, SHIFT);
			}
			else if (!bPointHit && !SHIFT)
			{
				// non move click on empty space
				select_all(FALSE);
			}
		break;
		case IDC_DELPOINTS:
			if (bGotRect)
				delete_rect(&rSelect);
		break;
		default:
		break;
	}
	bLDown = FALSE;
}

//=======================================================================
//	VOID CShapeEdit::KeyDown(WORD wKey)
//=======================================================================
VOID CShapeEdit::KeyDown(WORD wKey)
//=======================================================================
{
	int x,y;
	LPCEditPoly lpPoly, lpSelPoly;
	POINT pt;
	POSITION pos;
	
	switch (wKey)
	{
		case VK_DELETE:
			delete_selected();
		break;
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			x = (wKey==VK_LEFT) ? -1:((wKey==VK_RIGHT) ? 1:0);
			y = (wKey==VK_UP) ? -1:((wKey==VK_DOWN) ? 1:0);
			move_selected(x, y, TRUE);
		break;
		case VK_F2:
		case VK_TAB:
			// find first selected polygon
			lpSelPoly = NULL;
			pos = lpPolyList->GetHeadPosition();
			while (lpPoly = (LPCEditPoly)lpPolyList->GetNextPtr(pos))
			{
				// go through points
				lpPoly->GetPoint(-1);
				while (lpPoly->ListNextPoint(&pt))
				{
					if (lpPoly->GetFlags() & SE_SELECTED)
					{
						lpSelPoly = lpPoly;
						break;
					}
				}
				if (lpSelPoly)
					break;
			}
			select_all(SHIFT ? MAYBE:YES, lpSelPoly);
	 	break;
	 	case 'L':
			MakeCorners();
	 	break;				
	 	case 'C':
			MakeCurves();
	 	break;				
	}

}

//=======================================================================
//	VOID CShapeEdit::KeyUp(WORD wKey)
//=======================================================================
VOID CShapeEdit::KeyUp(WORD wKey)
//=======================================================================
{
}

//=======================================================================
//	VOID CShapeEdit::MakeCurves()
//	 	Make selected points into curves
//=======================================================================
VOID CShapeEdit::MakeCurves()
//=======================================================================
{
	POSITION pos;
	LPCEditPoly lpPoly;
	POINT pt;
	TFORM tform;
	LPTFORM lpTForm = NULL;
	HDC hDC = NULL;
	
	if (!lpPolyList)
		return;
	
	// Setup display TForm & hDC
	setup_display(&lpTForm, &tform, &hDC, hWnd);
	
	// go through polygons
	pos = lpPolyList->GetHeadPosition();
	while (lpPoly = (LPCEditPoly)lpPolyList->GetNextPtr(pos))
	{
		// undraw shape
//		draw_range(lpPoly, 0, -1, OFF, hDC, lpTForm);
		get_delayed_render(this, &RenderOff, lpPoly, 0, -1, lpTForm);
		// go through points
		lpPoly->GetPoint(-1);
		while (lpPoly->ListNextPoint(&pt))
		{
			// work on selected points
			if (!(lpPoly->GetFlags() & SE_SELECTED))
				continue;
			make_curve(lpPoly);
		}
		// redraw shape
//		draw_range(lpPoly, 0, -1, ON, hDC, lpTForm);
		get_delayed_render(this, &RenderOn, lpPoly, 0, -1, lpTForm);
		do_delayed_render(this, &RenderOff, OFF, hDC);
		do_delayed_render(this, &RenderOn, ON, hDC);
	}
	SelectClipRgn(hDC, NULL);
}

//=======================================================================
//	VOID CShapeEdit::MakeCorners()
//	 	Make selected points into corners.
//=======================================================================
VOID CShapeEdit::MakeCorners()
//=======================================================================
{                       
  	POSITION pos;
	LPCEditPoly lpPoly;
	POINT pt;
	TFORM tform;
	LPTFORM lpTForm = NULL;
	HDC hDC = NULL;
	
	if (!lpPolyList)
		return;
	
	// Setup display TForm & hDC
	setup_display(&lpTForm, &tform, &hDC, hWnd);
	
	// go through polygons
	pos = lpPolyList->GetHeadPosition();
	while (lpPoly = (LPCEditPoly)lpPolyList->GetNextPtr(pos))
	{
		// undraw shape
//		draw_range(lpPoly, 0, -1, OFF, hDC, lpTForm);
		// go through points
		get_delayed_render(this, &RenderOff, lpPoly, 0, -1, lpTForm);
		lpPoly->GetPoint(-1);
		while (lpPoly->ListNextPoint(&pt))
		{
			// work on selected points
			if (!(lpPoly->GetFlags() & SE_SELECTED))
				continue;
			make_corner(lpPoly);
		}
		// redraw shape
//		draw_range(lpPoly, 0, -1, ON, hDC, lpTForm);
		get_delayed_render(this, &RenderOn, lpPoly, 0, -1, lpTForm);
		do_delayed_render(this, &RenderOff, OFF, hDC);
		do_delayed_render(this, &RenderOn, ON, hDC);
	}
	SelectClipRgn(hDC, NULL);
}

//=======================================================================
//	VOID CShapeEdit::Draw(HDC hDC, BOOL on)
//=======================================================================
VOID CShapeEdit::Draw(HDC hDC, BOOL on)
//=======================================================================
{
	POSITION pos;
	LPCEditPoly lpPoly;
	TFORM tform;
	
	if (!lpPolyList || lpPolyList->IsEmpty())
		return;
	pos = lpPolyList->GetHeadPosition();
	if (!pos)
		return;
	TInit(&tform);
	File2DispTFormEx(hWnd, &tform, ISDRAWTOOL(Tool.id)/*YES*/);
	while(lpPoly = (LPCEditPoly)lpPolyList->GetNextPtr(pos))
		draw_range(lpPoly, 0, -1, on, hDC, &tform);
}	

//=======================================================================
//	VOID CShapeEdit::GetBounds(LPRECT lpRect)
//=======================================================================
VOID CShapeEdit::GetBounds(LPRECT lpRect)
//=======================================================================
{
	PolyListGetBounds(lpPolyList, lpRect);
}


//=======================================================================
// 		PRIVATE HELPER FUNCTIONS
//=======================================================================

//=======================================================================
//	Sets lpFound to be the point under x,y (screen coords)
//	Retruns TRUE if there was a point.
//=======================================================================
BOOL CShapeEdit::find_hit(int x, int y, LPSEPOS lpFound)
//=======================================================================
{
	LPCEditPoly lpPoly;
	POINT pt;
	TFORM tform;
	WORD wHandle;
	int index;
	int xmin, xmax, ymin, ymax;
	
	xmin = x-HALF_HANDLE_SIZE;
	xmax = x+HALF_HANDLE_SIZE;
	ymin = y-HALF_HANDLE_SIZE;
	ymax = y+HALF_HANDLE_SIZE;
	// prepare to read screen coords
	TInit(&tform);
	File2DispTFormEx(hWnd, &tform, ISDRAWTOOL(Tool.id)/*YES*/);
	// go through polygons
	lpFound->pos = lpPolyList->GetHeadPosition();
	while (lpPoly = (LPCEditPoly)lpPolyList->GetAtPtr(lpFound->pos))
	{
		// go through points
		lpPoly->GetPoint(-1);
		while (lpPoly->ListNextPoint(&pt))
		{
			// convert to screen coords
			Transformer(&tform, &pt, &pt.x, &pt.y);
			// is it a hit?
			if (WITHIN(pt.x, xmin, xmax) && WITHIN(pt.y, ymin, ymax))
			{
				// can only grab visible handles
				wHandle = lpPoly->IsHandle();
				if (wHandle == 1)
				{
					index = lpPoly->CurrentIndex()-1;
					if (index < 0)
						index = lpPoly->NumPoints()-1;
					if (!(lpPoly->GetFlags(index) & SE_SELECTED))
						continue;
				}
				if (wHandle == 2)
					if (!(lpPoly->GetFlags(lpPoly->CurrentIndex()+1) & SE_SELECTED))
						continue;
				// if its a selected bezier end point give precidence to control point
				if (!wHandle && 
					(lpPoly->GetFlags() & SE_SELECTED) &&
					(lpPoly->IsCurveEndPoint() & POLY_CURVE_BEGIN))
				{
					lpPoly->NextPoint(&pt);
					if (!WITHIN(pt.x, xmin, xmax) || !WITHIN(pt.y, ymin, ymax))
						lpPoly->PreviousPoint();
				}
				// found one!
				lpFound->num = lpPoly->CurrentIndex();
				return(TRUE);
			}	
		}
		lpPolyList->GetNextPtr(lpFound->pos);
	}
	return(FALSE);
}

//=======================================================================
//	Sets lpFound to be the first point in the line segment that comes 
//		close(est) to x,y (screen coords).
// 	If there are no polygons, then it set lpFound->pos = NULL.
// 	If there are no polygons with points, then it set lpFound->num = -1.
//	Retruns TRUE if there was such a line.
//=======================================================================
BOOL CShapeEdit::find_close_line(int x, int y, LPSEPOS lpFound)
//=======================================================================
{
	LPCEditPoly lpPoly;
	POSITION pos;
	POINT pt, last, test;
	POINT bezier[4];
	TFORM tform;
	int i,j, count;
	long d1, d2, d3, lastlen, t;
	RECT rBound;
	int best = 4;	// best dist
	long bestsq = 16;	// best dist^2
	BOOL bPointsExist = FALSE;
				 
	lpFound->pos = NULL;
	// prepare to read screen coords
	TInit(&tform);
	File2DispTFormEx(hWnd, &tform, ISDRAWTOOL(Tool.id) /*YES*/);
	// go through polygons
	pos = lpPolyList->GetHeadPosition();
	if (!pos)
	{
		lpFound->pos = NULL;
		lpFound->num = -1;
		return(TRUE);
	}
	while (lpPoly = (LPCEditPoly)lpPolyList->GetAtPtr(pos))
	{
		// go through points
		if (!bPointsExist)
			bPointsExist = lpPoly->NumPoints();
		lpPoly->GetPoint(-1);
		if (!lpPoly->NextPoint(&pt))
		{
		 	lpPolyList->GetNextPtr(pos);
		 	continue;
		}
		Transformer(&tform, &pt, &pt.x, &pt.y);
		last = pt;
		count = lpPoly->NumPoints() + (lpPoly->IsClosed() ? 1:0);
		for (i=0;i<count;i++)
		{
			if (lpPoly->PointType() & POLY_CURVE_HANDLE1)
			{
				// get bezier
				bezier[0] = last;
				bezier[1] = pt;
				lpPoly->NextPoint(&bezier[2]);	// 2nd handle
				Transformer(&tform, &bezier[2], &bezier[2].x, &bezier[2].y);
				i++;
				lpPoly->NextPoint(&bezier[3]); 	// end point
				Transformer(&tform, &bezier[3], &bezier[3].x, &bezier[3].y);
				i++;
				last = bezier[3];	
				//	quick check using bounding rect
				// get bounding rect expanded by best
				AstralSetRectEmpty(&rBound);
				rBound.left = rBound.right = bezier[0].x;
				rBound.top = rBound.bottom = bezier[0].y;
				for (j=1;j<4;j++)
				{
					if (bezier[j].x < rBound.left) rBound.left = bezier[j].x;
					if (bezier[j].x > rBound.right) rBound.right = bezier[j].x;
					if (bezier[j].y < rBound.top) rBound.top = bezier[j].y;
					if (bezier[j].y > rBound.bottom) rBound.bottom = bezier[j].y;
				}
				rBound.left -= best;
				rBound.right += best;
				rBound.top -= best;
				rBound.bottom += best;
				// is it in the rect?
				if (!(rBound.left <= x &&	rBound.right >= x &&
						rBound.top <= y && rBound.bottom >= y))
					goto Continue;					
				pt.x = x; pt.y = y;
				// check bezier
				BezierClosest(&test, &pt, bezier);
				test.x -= x;
				test.y -= y;
				d1 = DistanceSq(&test);
				if (d1 < bestsq)
				{
					// FOUND!
					lpFound->pos = pos;
					lpFound->num = i-3;
					bestsq = d1;
					best = lsqrt( bestsq );
				}
			}	// endif is curve
			else
			{
				//	quick check using bounding rect
				// get bounding rect expanded by best
				if (pt.x > last.x)
					{rBound.left = last.x-best; rBound.right = pt.x+best;}
				else
					{rBound.left = pt.x-best; rBound.right = last.x+best;}
				if (pt.y > last.y)
					{rBound.top = last.y-best; rBound.bottom = pt.y+best;}
				else
					{rBound.top = pt.y-best; rBound.bottom = last.y+best;}
				// is it in the rect?
				if (!(rBound.left <= x &&	rBound.right >= x &&
						rBound.top <= y && rBound.bottom >= y))
					goto Skip;					
				// DO REAL TEST
				// 'translate' all so pt=(0,0)
				last.x -= pt.x;
				test.x = x - pt.x;
				last.y -= pt.y;
				test.y = y - pt.y;
				// dist to p1 is just |test|
				d1 = DistanceSq(&test);
				// dist to last
				d2 = DistanceSq(&test, &last);
				// get min of the endpoint distances 
				d1 = Min(d1,d2);
				// perpendicular distence to line = (TEST*~LAST)/|LAST|
				lastlen = DistanceSq(&last);
				if (lastlen)
				{
					d3 = test.y*(LONG)last.x-test.x*(LONG)last.y;
					d3 *= d3;
					d3 /= lastlen;
					// should we bother to continue?
					if (bestsq >= d1 && bestsq >= d3)
						goto Skip;
					// is d3 a possiblity?
					if (d1  > d3)
					{
						// is d3 a point between pt&last?
						// get t such that t*LAST = perpendicular projection 
						// 		of TEST onto LAST.  If WITHIN(t,0,1), then we 
						//		have a valid intersection.
						// t = (LAST*TEST)/(LAST*LAST)
						t = DotProduct(&test, &last);
						// if its valid then its the best fit
						if (WITHIN (t,0,lastlen))
							d1 = d3;
					}
				}
				if (d1 < bestsq)
				{
					// FOUND!
					lpFound->pos = pos;
					lpFound->num = (i>0)? i-1:i;
					bestsq = d1;
					best = lsqrt( bestsq );
				}
Skip:				
				last = pt;	
			}	// end else is curve
Continue:			
	    	if (!lpPoly->NextPoint(&pt))
				break;	// for
			Transformer(&tform, &pt, &pt.x, &pt.y);
		}	// end for points
		lpPolyList->GetNextPtr(pos);
	}	// end while Poly
	if (lpFound->pos)
		return(TRUE);
	if (!bPointsExist)
	{
		lpFound->pos = lpPolyList->GetHeadPosition();
		lpFound->num = -1;
		return(TRUE);
	}
	return(FALSE);
}

//=======================================================================
//	Adds lpAddPoint (or close to it) after lpPos.
//	If sucessful sets lpPos to the new point.
//	If there is no polygon pointed to by lpPos then one is created!
//	Returns FALSE if there is a failure.
//=======================================================================
BOOL CShapeEdit::add_point(LPPOINT lpAddPoint, LPSEPOS lpPos)
//=======================================================================
{
	LPCEditPoly lpPoly;
	POINT start, pt, end;
	POINT bezier[4], newbez[4];
	WORD flags, wType;
	long a, b;
	BOOL bRet;
	double t;
	
	// get the polygon
	lpPoly = (LPCEditPoly)lpPolyList->GetAtPtr(lpPos->pos);
	if (!lpPoly)
	{
		if (!(lpPoly = new CEditPoly))
			return(FALSE);
		if (!(lpPos->pos = lpPolyList->AddTail(lpPoly)))
		{
			delete(lpPoly);
			return(FALSE);
		}
		lpPos->num = -1;
	}
	
	if (lpPos->num < 0)
	{
		// add to beginig
		// undraw lines 
//		draw_range(lpPoly,0, 1, OFF);
		get_delayed_render(this, &RenderOff, lpPoly, 0, 1);
		// add point
		lpPoly->GetPoint(-1);
		bRet = lpPoly->InsertPoint(lpAddPoint);
		// select it!
		flags = lpPoly->GetFlags();
		flags |= SE_SELECTED;
		lpPoly->SetFlags(flags);
		// draw new lines
//		draw_range(lpPoly,0, 2, ON);
		get_delayed_render(this, &RenderOn, lpPoly, 0, 2);
		do_delayed_render(this, &RenderOff, OFF);
		do_delayed_render(this, &RenderOn, ON);
		lpPos->num = 0;
		return(bRet);
	}
	
	lpPoly->GetPoint(lpPos->num, &start);
	wType = (WORD)lpPoly->PointType();
	// can't add to handles
	if (wType & POLY_CURVE_HANDLE)
		return(FALSE);
	// split curves
	if (wType & POLY_CURVE_BEGIN)
	{
		// get the bezier
		bezier[0] = start;
		lpPoly->NextPoint(&bezier[1]);	// 1st handle
		lpPoly->NextPoint(&bezier[2]);	// 2nd handle
		if (!lpPoly->NextPoint(&bezier[3]))	// end point
			return(FALSE);
		// get the parameter of the closest point 
	   	t =	BezierClosest(&start, lpAddPoint, bezier);
		// split bezier!
		SplitBezier(bezier/*in*/, bezier/*out*/, newbez, t);
		// undraw curve (and both handles)
//		draw_range(lpPoly,lpPos->num, lpPos->num+2, OFF);
		get_delayed_render(this, &RenderOff, lpPoly, lpPos->num, lpPos->num+2);
		// make changes to the shape
		lpPoly->GetPoint(lpPos->num);
		lpPoly->BezierToLine();
		lpPoly->InsertPoint(&bezier[3]);
		lpPoly->GetPoint(lpPos->num);
		bRet = lpPoly->LineToBezier(&bezier[1], &bezier[2]);
		if (!bRet)
			return(FALSE);
		NextControlPoint(lpPoly);
		bRet = lpPoly->LineToBezier(&newbez[1], &newbez[2]);
		// select it!
		flags = lpPoly->GetFlags();
		flags |= SE_SELECTED;
		lpPoly->SetFlags(flags);
		// draw new curves and all handles
//		draw_range(lpPoly,lpPos->num, lpPos->num+BEZIER_IN+1, ON);
		get_delayed_render(this, &RenderOn, lpPoly,lpPos->num, lpPos->num+BEZIER_IN+1);
		do_delayed_render(this, &RenderOff, OFF);
		do_delayed_render(this, &RenderOn, ON);
		lpPos->num += BEZIER_IN-1;
		return(bRet);
	}
	// SPLIT LINE
	if(!lpPoly->NextPoint(&end))
		end = start;
	pt = *lpAddPoint;
	if ((end.x != start.x) || (end.y != start.y) )
	{
		// translate pt & end start = (0,0)
		end.x -= start.x; end.y -= start.y;
		pt.x -= start.x; pt.y -= start.y;
		// compute pt = ((lpAddpoint*end)/(end*end))*end
		a = DotProduct(&pt, &end);
		b = DistanceSq(&end);
		pt.x = (end.x*a)/b;
		pt.y = (end.y*a)/b;
		// translate back
		pt.x += start.x; pt.y += start.y;
	}
	// undraw lines 
//	draw_range(lpPoly,lpPos->num, lpPos->num+1, OFF);
	get_delayed_render(this, &RenderOff, lpPoly, lpPos->num, lpPos->num+1);
	// add point
	bRet = !(lpPoly->GetPoint(lpPos->num, &end) == NULL);
	if (bRet)
	{
		bRet = lpPoly->InsertPoint(&pt);
		// select it!
		flags = lpPoly->GetFlags();
		flags |= SE_SELECTED;
		lpPoly->SetFlags(flags);
	}
	
	// draw new lines
//	draw_range(lpPoly,lpPos->num, lpPos->num+2, ON);
	get_delayed_render(this, &RenderOn, lpPoly, lpPos->num, lpPos->num+2);
	do_delayed_render(this, &RenderOff, OFF);
	do_delayed_render(this, &RenderOn, ON);
	if (bRet)
		lpPos->num++;
	return(bRet);
}

//=======================================================================
//	Moves all selected points by dx,dy(file coords)
//=======================================================================
void CShapeEdit::move_selected(int dx, int dy, BOOL bMoveHandles)
//=======================================================================
{
	POSITION pos;
	LPCEditPoly lpPoly;
	POINT pt;
	TFORM tform;
	BOOL bSel;
	int from, to, firstfrom, firstto;
	BOOL bFoundFirstRange = FALSE;
	LPTFORM lpTForm = NULL;
	HDC hDC = NULL;
	
	// Setup display TForm & hDC
	setup_display(&lpTForm, &tform, &hDC, hWnd);
	
	// go through polygons
	pos = lpPolyList->GetHeadPosition();
	while (lpPoly = (LPCEditPoly)lpPolyList->GetNextPtr(pos))
	{
		// undraw shape
//		draw_range(lpPoly, 0, -1, OFF, hDC, lpTForm);
//		get_delayed_render(this, &RenderOff, lpPoly, 0, -1, lpTForm);
		// go through points
		lpPoly->GetPoint(-1);
		from = -1;	// not started yet
		
		// find slected range
		while (lpPoly->ListNextPoint(&pt))
		{
			// wait for start
			bSel = (lpPoly->GetFlags() & SE_SELECTED);
			if (!bSel && lpPoly->IsHandle())
				continue;
			if (from < 0 && !bSel)
				continue;
			// is it a start?
			if (from < 0)
				from = lpPoly->CurrentIndex();
			else if (!bSel)
			{
				// end of range
				to = lpPoly->CurrentIndex()-1;
				if (!bFoundFirstRange && from == 0 && lpPoly->IsClosed())
				{
					// if its our first ans starts at 0
					//then save it to concatinate with last
					firstfrom = from;
					firstto = to;
					bFoundFirstRange = TRUE;
				}
				else
				{
					// move and draw them
					move_range(this, lpPoly, from, to, dx, dy, bMoveHandles);
				}
				// restore position
				lpPoly->GetPoint(to+1);
				from = -1;
			}
		}
		if (from >= 0)
		{
			// range all the way to the end
			// move and draw them
			if (bFoundFirstRange)
				to = firstto;
			else
				to = lpPoly->NumPoints()-1;
			move_range(this, lpPoly, from, to, dx, dy, bMoveHandles);
		}
		else if (bFoundFirstRange)
			move_range(this, lpPoly, firstfrom, firstto, dx, dy, bMoveHandles);
	}
}			


//************************************************************************
//		move_range 
// DESCRIPTION:
//			Undraws, moves, and redraws all points in the given rage (and thier handles)
//			by dx,dy.
//************************************************************************
LOCAL void move_range(LPCShapeEdit lpShapeEdit, LPCEditPoly lpPoly, int from, int to, int dx, int dy, int bMoveHandles)
//************************************************************************
{
	int drawfrom, drawto, i;	 
	POINT pt;
	WORD wType;
	BOOL bWholeShape;
	
	drawfrom = backup_whole(lpPoly, from);
	drawto = forward_whole(lpPoly, to);
	bWholeShape = FALSE;
	// be sure we dont 'cross over' on closed shapes
	if (drawfrom == drawto)
		bWholeShape = TRUE;
	else if ((from != to) &&  (drawto < to && drawfrom > from))
	{
		// crossed over by going all the way around
		bWholeShape = TRUE;
		
	}
	
	// undraw lines 
	if (bWholeShape)
		get_delayed_render(lpShapeEdit, &lpShapeEdit->RenderOff, lpPoly, 0, -1);
	else
		get_delayed_render(lpShapeEdit, &lpShapeEdit->RenderOff, lpPoly, drawfrom, drawto);
	// change points
	lpPoly->GetPoint(from-1, &pt);
	if (to >=from)
		i = to - from;
	else
		i = lpPoly->NumPoints()-from + to;
	while(i-- >= 0)
	{
		if (!lpPoly->NextPoint(&pt))
			break;
		wType = (WORD)lpPoly->PointType();
		
		// handle curves with thier base point
		if (wType & POLY_CURVE_HANDLE)
			continue;
		if (bMoveHandles)
		{
			// move point
			pt.x += dx;
			pt.y += dy;
			lpPoly->SetCurrentPoint(&pt);
			// check for handles
			if (wType & POLY_CURVE_END)
			{
				lpPoly->PreviousPoint(&pt);
				pt.x += dx;
				pt.y += dy;
				lpPoly->SetCurrentPoint(&pt);
				lpPoly->NextPoint();
			}
			if (wType & POLY_CURVE_BEGIN)
			{
				lpPoly->NextPoint(&pt);
				pt.x += dx;
				pt.y += dy;
				lpPoly->SetCurrentPoint(&pt);
				i--;
				// skip 2nd handle
				lpPoly->NextPoint();
				i--;
			}
		}
		else if (!(wType & POLY_CURVE_HANDLE))
		{
			// move point
			pt.x += dx;
			pt.y += dy;
			lpPoly->SetCurrentPoint(&pt);
		}
	}
	// draw lines 
	if (bWholeShape)
		get_delayed_render(lpShapeEdit, &lpShapeEdit->RenderOn, lpPoly, 0, -1);
	else
	 	get_delayed_render(lpShapeEdit, &lpShapeEdit->RenderOn, lpPoly, drawfrom, drawto);
 	do_delayed_render(lpShapeEdit, &lpShapeEdit->RenderOff, OFF);
 	do_delayed_render(lpShapeEdit, &lpShapeEdit->RenderOn, ON);
}


//=======================================================================
//	Moves all points in shape by dx,dy(file coords)
//=======================================================================
void CShapeEdit::move_shape(POSITION Shape, int dx, int dy, BOOL bMoveHandles)
//=======================================================================
{
	LPCEditPoly lpPoly;
	POINT pt;
	TFORM tform;
	LPTFORM lpTForm = NULL;
	HDC hDC = NULL;
	
	// get polygon
   	lpPoly = (LPCEditPoly)lpPolyList->GetAtPtr(Shape);
	if (!lpPoly)
		return;
		
	// Setup display TForm & hDC
	setup_display(&lpTForm, &tform, &hDC, hWnd);
	
	// undraw shape
//	draw_range(lpPoly, 0, -1, OFF, hDC, lpTForm);
	get_delayed_render(this, &RenderOff, lpPoly, 0, -1, lpTForm);
	
	// go through points
	lpPoly->GetPoint(-1);
	while (lpPoly->ListNextPoint(&pt))
	{
		// MOVE IT!
		if (!bMoveHandles && lpPoly->IsHandle())
			continue;
		pt.x += dx;
		pt.y += dy;
		lpPoly->SetCurrentPoint(&pt);
	}
	// redraw shape
//	draw_range(lpPoly, 0, -1, ON, hDC, lpTForm);
 	get_delayed_render(this, &RenderOn, lpPoly, 0, -1, lpTForm);
 	do_delayed_render(this, &RenderOff, OFF, hDC);
 	do_delayed_render(this, &RenderOn, ON, hDC);
	SelectClipRgn(hDC, NULL);
}

//=======================================================================
//	Moves the lpPos point by dx,dy (file coords)			
//	if bMoveHandles then it will move any handles with the point.
//=======================================================================
void CShapeEdit::move_point(int dx, int dy, BOOL bMoveHandles, LPSEPOS lpPos)
//=======================================================================
{
	LPCEditPoly lpPoly;
	
	lpPoly = (LPCEditPoly)lpPolyList->GetAtPtr(lpPos->pos);
	if (!lpPoly)
		return;
	move_range(this, lpPoly, lpPos->num, lpPos->num, dx, dy, bMoveHandles);
}		


//=======================================================================
// 	Call before using move_handle.
//	Sets up the angle between handles so it can be maintained.
//	Sets ptHandle1 & ptHandle2.
//=======================================================================
void CShapeEdit::start_move_handle(LPSEPOS lpPos)
//=======================================================================
{
	LPCEditPoly lpPoly;
	WORD wHandle;
	POINT pt;
	
	// get the polygon
	lpPoly = (LPCEditPoly)lpPolyList->GetAtPtr(lpPos->pos);
	if (!lpPoly)
		return;
	if (!lpPoly->GetPoint(lpPos->num, &pt))
		return;
	ptHandle1 = ptHandle2 = pt;
	wHandle = lpPoly->IsHandle();
	if (wHandle == 1)   
	{
			lpPoly->PreviousPoint();
			lpPoly->PreviousPoint(&ptHandle1); // previous handle
	}
	else if (wHandle == 2)
	{
		lpPoly->NextPoint();
		lpPoly->NextPoint(&ptHandle2); // next handle
	}
}

//=======================================================================
//	Moves the lpPos point by dx,dy (file coords)			
//	wFlages then it will lock the angle between handles.
//=======================================================================
void CShapeEdit::move_handle(int dx, int dy, WORD wFlags, LPSEPOS lpPos)
//=======================================================================
{
	LPCEditPoly lpPoly;
	POINT base, ptNew, pt;
	WORD wType;
	int from, to;
	
	lpPoly = (LPCEditPoly)lpPolyList->GetAtPtr(lpPos->pos);
	if (!lpPoly)
		return;
	// determine how much of curve wil be effected
	lpPoly->GetPoint(lpPos->num, &ptNew);
	wType = lpPoly->IsHandle();
	if (!wType)
		return;
	// keep pair up to date
	if (!wFlags)
		start_move_handle(lpPos);
	from = lpPos->num;
	to = from;
	if (wType == 1)
	{
		// first handle
		to+=2;
		if (wFlags)
			from -=2;
	}
	else
	{
		// 2nd handle
		from -=2;
		if (wFlags)
			to+=2;
	}
	
	if (from<0)
		from = lpPoly->IsClosed() ? (lpPoly->NumPoints()+from):0;
	if (to>=lpPoly->NumPoints())
		to = lpPoly->IsClosed() ? (to-lpPoly->NumPoints()):(lpPoly->NumPoints()-1);
		
	// be sure we dont 'cross over' on closed shapes
	if ((from == to ) || (to < lpPos->num && from> lpPos->num))
	{
		// crossed over by going all the way around
		// draw the whole thing
		from = 0;
		to = -1;
	}
	
	// undraw lines 
	//	draw_range(lpPoly,from, to, OFF);
	get_delayed_render(this, &RenderOff, lpPoly, from, to);
	// change point
	lpPoly->GetPoint(lpPos->num, &ptNew);
	ptNew.x += dx;
	ptNew.y += dy;
	lpPoly->SetCurrentPoint(&ptNew);
	// Move other handle
	if (wFlags)
	{
		// check for handles
		if (wType == 1)
		{
			lpPoly->PreviousPoint(&base);
			lpPoly->PreviousPoint();
			if (lpPoly->IsHandle())
			{
				rotate_point(&pt, &ptHandle1, &ptHandle2, &ptNew, &base);
				lpPoly->SetCurrentPoint(&pt);
			}
		}
		else if (wType == 2)
		{
			lpPoly->NextPoint(&base);
			lpPoly->NextPoint();
			if (lpPoly->IsHandle())
			{
				rotate_point(&pt, &ptHandle2, &ptHandle1, &ptNew, &base);
				lpPoly->SetCurrentPoint(&pt);
			}
		}
	}
	// draw lines 
//	draw_range(lpPoly,from, to, ON);
 	get_delayed_render(this, &RenderOn, lpPoly, from, to);
 	do_delayed_render(this, &RenderOff, OFF);
 	do_delayed_render(this, &RenderOn, ON);
}



//=======================================================================
//	selects all points inside lpSelect (file coords)
//	if bAdd then it will add the points to the already selected points
//=======================================================================
void CShapeEdit::select_rect(LPRECT lpSelect, BOOL bAdd)
//=======================================================================
{
	POSITION pos;
	LPCEditPoly lpPoly;
	POINT pt;
	TFORM tform;
	LPTFORM lpTForm = NULL;
	HDC hDC = NULL;
	
	if (!bAdd)
		select_all(OFF);
	
	// Setup display TForm & hDC
	setup_display(&lpTForm, &tform, &hDC, hWnd);
	
	// go through polygons
	pos = lpPolyList->GetHeadPosition();
	while (lpPoly = (LPCEditPoly)lpPolyList->GetNextPtr(pos))
	{
		// go through points
		lpPoly->GetPoint(-1);
		while (lpPoly->ListNextPoint(&pt))
			if (PtInRect(lpSelect, pt))
				select_it(lpPoly, ON, hDC, lpTForm);
	}
	SelectClipRgn(hDC, NULL);
}

//=======================================================================
//	selects the point
//	if bAdd then it will add the points to the already selected points
//	BOOL bSelect- YES for select, NO for deselect, MAYBE for invert.
//=======================================================================
void CShapeEdit::select_point(LPSEPOS lpPos, BOOL bSelect, BOOL bAdd)
//=======================================================================
{
	LPCEditPoly lpPoly;
	
	if (!bAdd)
		select_all(OFF);
	lpPoly = (LPCEditPoly)lpPolyList->GetAtPtr(lpPos->pos);
	if (!lpPoly)
		return;
	lpPoly->GetPoint(lpPos->num);
	if (bSelect == MAYBE)
		bSelect = !(lpPoly->GetFlags() & SE_SELECTED);	
	select_it(lpPoly, bSelect);
}

//=======================================================================
//	selects or deselects the current point in the polygon.
//	updates visual.
//=======================================================================
void CShapeEdit::select_it(LPCEditPoly lpPoly, BOOL bOn, HDC hDC, LPTFORM lpTForm)
//=======================================================================
{
	BOOL bSel;
	WORD flags, wType;
	POINT pt, handle;
	BOOL bOldClip = FALSE;
	TFORM tform;
	int iInvert;
	
	// can't select a handle
	wType = (WORD)lpPoly->PointType();
	if (wType & POLY_CURVE_HANDLE)
		return;
	flags = lpPoly->GetFlags();
	bSel = (flags & SE_SELECTED);	
	if ((bSel && bOn) || (!bSel && !bOn))
		return;
	// Setup display TForm & hDC
	if (!lpTForm || !hDC)
		bOldClip = setup_display(&lpTForm, &tform, &hDC, hWnd);
	if (bOn)
		flags |= SE_SELECTED;
	else
		flags &= !SE_SELECTED;
	lpPoly->SetFlags(flags);
	lpPoly->CurrentPoint(&pt);
	Transformer(lpTForm, &pt, &pt.x, &pt.y);
	
	// begin invert
	iInvert = StartInvert(hDC, OFF);
	
	draw_handle(&pt, hDC, OFF, bSel);
	draw_handle(&pt, hDC, ON, bOn);
	// are we part of a bezier?
	if (wType & POLY_CURVE_END)
	{
		// handle is previous point
		if (lpPoly->PreviousPoint(&handle))
		{
			lpPoly->NextPoint();
			Transformer(lpTForm, &handle, &handle.x, &handle.y);
//			Dline(hDC, pt.x, pt.y, handle.x, handle.y, bOn);
			MoveTo( hDC, pt.x, pt.y);
			LineTo( hDC, handle.x, handle.y);
			draw_bez_handle(&handle, hDC, bOn);
		}
	}
	if (wType & POLY_CURVE_BEGIN)
	{
		// handle is next point
		if (lpPoly->NextPoint(&handle))
		{
			lpPoly->PreviousPoint();
			Transformer(lpTForm, &handle, &handle.x, &handle.y);
// 			Dline(hDC, pt.x, pt.y, handle.x, handle.y, bOn);
			MoveTo( hDC, pt.x, pt.y);
			LineTo( hDC, handle.x, handle.y);
			draw_bez_handle(&handle, hDC, bOn);
		}
	}
	
	// done inverting
	EndInvert(hDC, iInvert);
	
	if (bOldClip)
		SelectClipRgn(hDC, NULL);
}

//=======================================================================
//	selects or deselects or inverts selection on all the points in lpPoly.  If lpPoly == NULL 
//	the it selects or deselects all the points in whole universe! (or at 
//		least the shape)
//	BOOL bSelect- YES for select, NO for deselect, MAYBE for invert.
//=======================================================================
void CShapeEdit::select_all(BOOL bSelect, LPCEditPoly lpPoly)
//=======================================================================
{

	POSITION pos;
	TFORM tform;        
	POINT pt;
	LPTFORM lpTForm = NULL;
	HDC hDC = NULL;
	BOOL bSelPoint;
	
	// Setup display TForm & hDC
	setup_display(&lpTForm, &tform, &hDC, hWnd);
	
	bSelPoint = bSelect;
	
	if (lpPoly)
	{
		// select all points in poly
		lpPoly->GetPoint(-1);
		while (lpPoly->ListNextPoint(&pt))
		{
			if (bSelect == MAYBE)
				bSelPoint = !(lpPoly->GetFlags() & SE_SELECTED);
			select_it(lpPoly, bSelPoint, hDC, lpTForm);
		}
	}
	else
	{
		// go through polygons
		pos = lpPolyList->GetHeadPosition();
		while (lpPoly = (LPCEditPoly)lpPolyList->GetNextPtr(pos))
		{
			// go through points
			lpPoly->GetPoint(-1);
			while (lpPoly->ListNextPoint(&pt))
			{
				if (bSelect == MAYBE)
					bSelPoint = !(lpPoly->GetFlags() & SE_SELECTED);
				select_it(lpPoly, bSelPoint, hDC, lpTForm);
			}
		}
	}
	SelectClipRgn(hDC, NULL);
}

//=======================================================================
//	lpArea is in file coords.
//	returns the number of points deleted.
//=======================================================================
int CShapeEdit::delete_rect(LPRECT lpArea)
//=======================================================================
{
	POSITION pos;
	LPCEditPoly lpPoly;
	POINT pt;
	int deleted = 0;
	TFORM tform;
	LPTFORM lpTForm = NULL;
	HDC hDC = NULL;
	
	// Setup display TForm & hDC
	setup_display(&lpTForm, &tform, &hDC, hWnd);
	
	// go through polygons
	pos = lpPolyList->GetHeadPosition();
	while (lpPoly = (LPCEditPoly)lpPolyList->GetNextPtr(pos))
	{
		// undraw shape
//		draw_range(lpPoly, 0, -1, OFF, hDC, lpTForm);
		get_delayed_render(this, &RenderOff, lpPoly, 0, -1, lpTForm);
		// go through points
		lpPoly->GetPoint(-1);
		lpPoly->ListNextPoint(&pt);
		while (TRUE)
		{
			if (PtInRect(lpArea, pt))
			{
				// DELETE IT!
				if (delete_poly_point(lpPoly))
				{
					deleted++;
					if (!lpPoly->CurrentPoint(&pt))
						break;
					continue;
				}
			}
			if(!lpPoly->ListNextPoint(&pt))
				break;
		}
		// redraw shape
//		draw_range(lpPoly, 0, -1, ON, hDC, lpTForm);
		get_delayed_render(this, &RenderOn, lpPoly, 0, -1, lpTForm);
		do_delayed_render(this, &RenderOff, OFF, hDC);
		do_delayed_render(this, &RenderOn, ON, hDC);
	}
	SelectClipRgn(hDC, NULL);
	return(deleted);
}

//=======================================================================
//	returns the number of points deleted.
//=======================================================================
int CShapeEdit::delete_selected()
//=======================================================================
{
	POSITION pos;
	LPCEditPoly lpPoly;
	POINT pt;
	int deleted = 0;
	TFORM tform;
	LPTFORM lpTForm = NULL;
	HDC hDC = NULL;
	
	// Setup display TForm & hDC
	setup_display(&lpTForm, &tform, &hDC, hWnd);
	
	// go through polygons
	pos = lpPolyList->GetHeadPosition();
	while (lpPoly = (LPCEditPoly)lpPolyList->GetNextPtr(pos))
	{
		// undraw shape
//		draw_range(lpPoly, 0, -1, OFF, hDC, lpTForm);
		get_delayed_render(this, &RenderOff, lpPoly, 0, -1, lpTForm);
		// go through points
		lpPoly->GetPoint(-1);
		if (!lpPoly->ListNextPoint(&pt))
			continue;
		while (TRUE)
		{
			if (lpPoly->GetFlags() & SE_SELECTED)
			{
				// DELETE IT!
				if (delete_poly_point(lpPoly))
				{
					deleted++;
					if (!lpPoly->CurrentPoint(&pt))
						break;
					continue;
				}
			}
			if(!lpPoly->ListNextPoint(&pt))
				break;
		}
		// redraw shape
//		draw_range(lpPoly, 0, -1, ON, hDC, lpTForm);
		get_delayed_render(this, &RenderOn, lpPoly, 0, -1, lpTForm);
		do_delayed_render(this, &RenderOff, OFF, hDC);
		do_delayed_render(this, &RenderOn, ON, hDC);
	}
	SelectClipRgn(hDC, NULL);
	return(deleted);
}

//=======================================================================
//=======================================================================
void CShapeEdit::delete_point(LPSEPOS lpPos)
//=======================================================================
{   
	LPCEditPoly lpPoly;
	WORD wType;
	int from, to, num;
	
	// get the polygon
	lpPoly = (LPCEditPoly)lpPolyList->GetAtPtr(lpPos->pos);
	if (!lpPoly)
		return;
	lpPoly->GetPoint(lpPos->num);
	wType = (WORD)lpPoly->PointType();
	// can't delete handle
	if (wType & POLY_CURVE_HANDLE)
		return;
	// DELETE POINT 
	lpPoly->GetPoint(lpPos->num);
	// undraw lines 
	// adjust for beziers
	from = backup_whole(lpPoly, lpPos->num);
	to = forward_whole(lpPoly, lpPos->num);
//	draw_range(lpPoly, from, to, OFF);
  	get_delayed_render(this, &RenderOff, lpPoly,from , to);
	// delete point
	lpPoly->GetPoint(lpPos->num);
	num = delete_poly_point(lpPoly);
	// draw new lines
	if (num>0)
	{
		if (from > to)
		{
			// from will change also
			from = backup_whole(lpPoly, lpPos->num);
			to = forward_whole(lpPoly, from); 
		}
		else if (from == lpPos->num)
			to = from;	// deleted first point (not closed)
		else
			to = forward_whole(lpPoly, from); 
	}
//	draw_range(lpPoly,from, to, ON);
   	get_delayed_render(this, &RenderOn, lpPoly, from, to);
   	do_delayed_render(this, &RenderOff, OFF);
   	do_delayed_render(this, &RenderOn, ON);
	return;
}


//=======================================================================
//	 Draw the range of points into the window.
//	lpTForm is the file to display tform and may be NULL.
//	hDC can also be NULL.
//	if to is -1 then the entire shape will be drawn.
//	draws complete beziers if any handle is in the range.
//=======================================================================
void CShapeEdit::draw_range(LPCEditPoly lpPoly, int from, int to, BOOL on, HDC hDC, LPTFORM lpTForm)
//=======================================================================
{
	LPPOINT lpData;
	int num, i, numraw, idone; 
	TFORM tform;
	int iInvert;
	POINT pt, last;
	BOOL bSel, lastSel;
	BOOL bOldClip = FALSE;
	
	
	// get the data
	if (from < 0) 
		from = 0;
	lpPoly->GetPoint(from);
	if (lpPoly->IsHandle())
	{
		PreviousControlPoint(lpPoly);
		from = lpPoly->CurrentIndex();
	}
	if (to == -1)
		num = -1;
	else if (from > to)
		num = lpPoly->NumPoints()-from+to;
	else
		num = to-from+1;
	
	// Setup display TForm & hDC
	if (!lpTForm || !hDC)
		bOldClip = setup_display(&lpTForm, &tform, &hDC, hWnd);

	idone = 16384; 	// storage size
	lpData = lpPoly->GetData(num, lpTForm, &numraw, (LPPOINT)LineBuffer[0], &idone);
	if (!lpData)
	{
		if (bOldClip)
			SelectClipRgn(hDC, NULL);
	   	return;
	}
		
	// begin inverting
	iInvert = StartInvert(hDC, OFF);

	// draw line
	Polyline(hDC, lpData, numraw);
	
	// draw handles
	lpPoly->GetPoint(from-1);
	if (num == -1)
		num = lpPoly->NumPoints()-from;
	else
		num++;	// to include final handle
	while(--num >= 0)
	{
		if (!lpPoly->NextPoint(&pt))
			break;
		Transformer(lpTForm, &pt, &pt.x, &pt.y);
		if (i = lpPoly->IsHandle())
		{
			if (i == 2)
			{
				// handle belongs to next point 
				bSel = lpPoly->GetFlags(lpPoly->CurrentIndex()+1) & SE_SELECTED;
				if (bSel)
					if (lpPoly->NextPoint(&last))
					{
						lpPoly->PreviousPoint();
						Transformer(lpTForm, &last, &last.x, &last.y);
//						Dline(hDC, last.x, last.y, pt.x, pt.y, on );
						MoveTo( hDC, last.x, last.y);
						LineTo( hDC, pt.x, pt.y);
						draw_bez_handle(&pt, hDC, on);
					}
			}
			else if (lastSel)
			{
//				Dline(hDC,last.x, last.y, pt.x, pt.y, on );
			  	MoveTo( hDC, last.x, last.y);
			  	LineTo( hDC, pt.x, pt.y);
				draw_bez_handle(&pt, hDC, on);
			}
		}
		else
		{
			bSel = lpPoly->GetFlags() & SE_SELECTED;
			draw_handle(&pt, hDC, on, bSel);
			last = pt;
			lastSel = bSel;
		}
	}
	
	// done inverting
	EndInvert(hDC, iInvert);
	
	// clean up
	if (lpData != (LPPOINT)LineBuffer[0])
		FreeUp(lpData);
	if (bOldClip)
		SelectClipRgn(hDC, NULL);
}


//************************************************************************
// DESCRIPTION:
// identical to draw_range, except does not do render.
// fills lpRender.  If lpRender in not large enough then returns false.
// even when returning false, a partial render can be attempted.
//************************************************************************
LOCAL BOOL get_delayed_render(LPCShapeEdit lpEditShape, LPSERENDER lpRender, LPCEditPoly lpPoly, int from, int to, LPTFORM lpTForm)
//************************************************************************
{                       
	int num, idone, numraw, i;
	BOOL bSel, lastSel;
	TFORM tform;
	POINT pt, last;
	LPPOINT lpHandles;
	int iNumHandles;
	int iMaxNumHandles;
	LPPOINT lpBezPairs;
	int iNumBezPairs;
	int iMaxNumBezPairs;
	
	
	lpRender->iNumLines = 0;
	lpRender->iNumHandles = 0;
	lpRender->iNumBezPairs = 0;
	if (lpRender->iMaxNumHandles < lpPoly->NumPoints())
		return(FALSE);
		
	// for quich access
	lpHandles = lpRender->lpHandles;
	iNumHandles = lpRender->iNumHandles;
	iMaxNumHandles = lpRender->iMaxNumHandles;
	lpBezPairs = lpRender->lpBezPairs;
	iNumBezPairs = lpRender->iNumBezPairs;
	iMaxNumBezPairs = lpRender->iMaxNumBezPairs;
	
	// be sure tform is ok
	if (!lpTForm)
		setup_display(&lpTForm, &tform, NULL, lpEditShape->hWnd);
	
	// get the data
	if (from < 0) 
		from = 0;
	lpPoly->GetPoint(from);
	if (lpPoly->IsHandle())
	{
		PreviousControlPoint(lpPoly);
		from = lpPoly->CurrentIndex();
	}
	if (to == -1)
		num = -1;
	else if (from > to)
		num = lpPoly->NumPoints()-from+to;
	else
		num = to-from+1;
		
	if (num > lpPoly->NumPoints())
		num =  lpPoly->NumPoints();
	if ((from + num) > lpPoly->NumPoints() && !lpPoly->IsClosed())
		num =  lpPoly->NumPoints() - from;
	
	// render polylines
	idone = lpRender->iMaxNumLines; 	// storage size
	lpRender->lpLines = NULL;
	if (idone)
		lpRender->lpLines = lpPoly->GetData(num, lpTForm, &numraw, lpRender->lpBaseLines, &idone);
	if (!lpRender->lpLines || idone < num)
	{
		// try again, but let GetData do alloc 
		lpRender->lpLines = lpPoly->GetData(num, lpTForm, &numraw);
		if (!lpRender->lpLines)
			goto ExitFalse;
	}
	lpRender->iNumLines = numraw;
		
	// get handles
	lpPoly->GetPoint(from-1);
	if (num == -1)
		num = lpPoly->NumPoints()-from;
	else
		num++;	// to include final handle
	while(--num >= 0)
	{
		if (!lpPoly->NextPoint(&pt))
			break;
		Transformer(lpTForm, &pt, &pt.x, &pt.y);
		if (i = lpPoly->IsHandle())
		{
			if (i == 2)
			{
				// handle belongs to next point 
				bSel = lpPoly->GetFlags(lpPoly->CurrentIndex()+1) & SE_SELECTED;
				if (bSel)
					if (lpPoly->NextPoint(&last))
					{
						lpPoly->PreviousPoint();
						Transformer(lpTForm, &last, &last.x, &last.y);
						if (iNumBezPairs >=  iMaxNumBezPairs)
							goto ExitFalse;
						lpBezPairs[iNumBezPairs*2] = last;
						lpBezPairs[(iNumBezPairs++*2)+1] = pt;
					}
			}
			else if (lastSel)
			{
				if (iNumBezPairs >=  iMaxNumBezPairs)
					goto ExitFalse;
				lpBezPairs[iNumBezPairs*2] = last;
				lpBezPairs[(iNumBezPairs++*2)+1] = pt;
			}
		}
		else
		{
			bSel = lpPoly->GetFlags() & SE_SELECTED;
			last = pt;
			lastSel = bSel;
			// mark as selected
			if (bSel)
			{
				if( pt.x < 0)
					pt.x += ((INT_MIN+1)/2);
				else
					pt.x += ((INT_MAX-1)/2);
			}
			if (iNumHandles >=  iMaxNumHandles)
				goto ExitFalse;
			lpHandles[iNumHandles++] = pt;
		}
	}

	lpRender->iNumHandles = iNumHandles;
	lpRender->iNumBezPairs = iNumBezPairs;
	return(TRUE);

ExitFalse:
	if (lpRender->lpLines && lpRender->lpLines != lpRender->lpBaseLines)
	{
		FreeUp((LPTR)lpRender->lpLines);
		lpRender->lpLines = NULL;
		lpRender->iNumLines = 0;
	}
	lpRender->iNumHandles = iNumHandles;
	lpRender->iNumBezPairs = iNumBezPairs;
	return(FALSE);
}

//************************************************************************
//	Note: may free up line segment... so can only use once per call to get_delayed_render
//************************************************************************
LOCAL void do_delayed_render(LPCShapeEdit lpEditShape, LPSERENDER lpRender, BOOL on, HDC hDC)
//************************************************************************
{
	int i; 
	int iInvert;
	POINT pt;
	BOOL bOldClip = FALSE;
	LPPOINT lpHandles;
	int iNumHandles;
	
	
	// Setup display TForm & hDC
	if (!hDC)
		bOldClip = setup_display(NULL, NULL, &hDC, lpEditShape->hWnd);
	// begin inverting
	iInvert = StartInvert(hDC, OFF);

	// draw line
	if (lpRender->iNumLines && lpRender->lpLines)
	{
		Polyline(hDC, lpRender->lpLines, lpRender->iNumLines);
		if (lpRender->lpLines != lpRender->lpBaseLines)
		{
			FreeUp((LPTR)lpRender->lpLines);
			lpRender->lpLines = NULL;
			lpRender->iNumLines = 0;
		}
	}
	
	// draw handles
	iNumHandles = lpRender->iNumHandles;
	lpHandles = lpRender->lpHandles;
	for (i=0;i<iNumHandles;i++)
	{
		pt = lpHandles[i];
		if( pt.x <= ((INT_MIN+1)/2))
		{
			pt.x -= ((INT_MIN+1)/2);
		 	draw_handle(&pt, hDC, on, YES);
		}
		else if (pt.x >= ((INT_MAX-1)/2))
		{
			pt.x -= ((INT_MAX-1)/2);
		 	draw_handle(&pt, hDC, on, YES);
		}
		else
		 	draw_handle(&pt, hDC, on, NO);
	}
	
	// draw beizer handles
	iNumHandles = lpRender->iNumBezPairs;
	lpHandles = lpRender->lpBezPairs;
	for (i=0;i<iNumHandles;i++)
	{
	  	MoveTo( hDC, lpHandles[i*2].x, lpHandles[i*2].y);
	  	LineTo( hDC, lpHandles[i*2+1].x, lpHandles[i*2+1].y);
		draw_bez_handle(&lpHandles[i*2+1], hDC, on);
	}
	
	// done inverting
	EndInvert(hDC, iInvert);
	if (bOldClip)
		SelectClipRgn(hDC, NULL);
}

//=======================================================================
//=======================================================================
LOCAL BOOL fill_render(LPSERENDER lpRender)
//=======================================================================
{
	long maxmem = 32760;
	
	lpRender->lpBaseLines = NULL;
	lpRender->lpHandles = NULL;
	lpRender->lpBezPairs = NULL;
	if (!(lpRender->lpBaseLines = (LPPOINT)Alloc(maxmem)))
		goto MemError;
	lpRender->iMaxNumLines = maxmem/sizeof(POINT);
	if (!(lpRender->lpHandles = (LPPOINT)Alloc(maxmem)))
		goto MemError;
	lpRender->iMaxNumHandles = maxmem/sizeof(POINT);
	if (!(lpRender->lpBezPairs = (LPPOINT)Alloc(maxmem)))
		goto MemError;
	lpRender->iMaxNumBezPairs = maxmem/(2*sizeof(POINT));
	return(TRUE);

MemError:
free_render(lpRender);
	return(FALSE);
}

//=======================================================================
//=======================================================================
LOCAL void free_render(LPSERENDER lpRender)
//=======================================================================
{
	if (lpRender->lpLines && (lpRender->lpLines != lpRender->lpBaseLines))
		FreeUp((LPTR)lpRender->lpLines);
	lpRender->lpLines = NULL;
	if (lpRender->lpBaseLines)
		FreeUp((LPTR)lpRender->lpBaseLines);
	lpRender->lpBaseLines = NULL;
	lpRender->iMaxNumLines = 0;
	if (lpRender->lpHandles)
		FreeUp((LPTR)lpRender->lpHandles);
	lpRender->lpHandles = NULL;
	lpRender->iMaxNumHandles = 0;
	if (lpRender->lpBezPairs)
		FreeUp((LPTR)lpRender->lpBezPairs);
	lpRender->lpBezPairs = NULL;
	lpRender->iMaxNumBezPairs = 0;
}


//=======================================================================
//	hDC can not be null!
// 	Wrap with StartInvert and EndInvert
//=======================================================================
LOCAL void draw_handle(LPPOINT lpPt, HDC hDC, BOOL on, BOOL bSel)
//=======================================================================
{
	
	if (bSel)
	{
		RECT rArea;
	
		rArea.left = 	lpPt->x-HALF_HANDLE_SIZE;
		rArea.right = 	lpPt->x+HALF_HANDLE_SIZE;
		rArea.top = 	lpPt->y-HALF_HANDLE_SIZE;
		rArea.bottom = 	lpPt->y+HALF_HANDLE_SIZE;
//		Dbox(hDC, &rArea, on );
		InvertRect(hDC, &rArea);
	}
	else
	{
		POINT ptlist[5];
		ptlist[0].x = lpPt->x-HALF_HANDLE_SIZE;
		ptlist[0].y = lpPt->y-HALF_HANDLE_SIZE;
		ptlist[1].x = lpPt->x+HALF_HANDLE_SIZE;
		ptlist[1].y = ptlist[0].y;
		ptlist[2].x = ptlist[1].x;
		ptlist[2].y = lpPt->y+HALF_HANDLE_SIZE;
		ptlist[3].x = ptlist[0].x;
		ptlist[3].y = ptlist[2].y;
//		Drect(hDC, &rArea, on );
		ptlist[4] = ptlist[0];
		Polyline(hDC, ptlist, 5);
	}
}

//=======================================================================
//	hDC can not be null!
// 	Wrap with StartInvert and EndInvert
//=======================================================================
LOCAL void draw_bez_handle(LPPOINT lpPt, HDC hDC, BOOL on)
//=======================================================================
{
	POINT ptlist[5];
	
	ptlist[0].x = lpPt->x-(HALF_HANDLE_SIZE-1);
	ptlist[0].y = lpPt->y-(HALF_HANDLE_SIZE);
	ptlist[1].x = lpPt->x+(HALF_HANDLE_SIZE-1);
	ptlist[1].y = ptlist[0].y;
	ptlist[2].x = ptlist[1].x;
	ptlist[2].y = lpPt->y+(HALF_HANDLE_SIZE);
	ptlist[3].x = ptlist[0].x;
	ptlist[3].y = ptlist[2].y;
	ptlist[4] = ptlist[0];
//	Drect(hDC, &rArea, on);
  	Polyline(hDC, ptlist, 5);
	ptlist[0].x--; 
	ptlist[0].y++; 
	ptlist[1].x++; 
	ptlist[1].y++; 
	ptlist[2].x++; 
	ptlist[2].y--;
	ptlist[3].x--; 
	ptlist[3].y--; 
	ptlist[4] = ptlist[0];
//	Drect(hDC, &rArea, on);
  	Polyline(hDC, ptlist, 5);
}


//=======================================================================
//	helper function... 
//	if (*lppOutTForm == NULL)  then it sets up lpInTForm and sets 
//		*lppOutTForm=lpInTForm.
//	if (*lpHDC == NULL) then sets up *lpHDC.
//		returns TRUE if lpHDC was set up. (user should reset hDC when done)
//=======================================================================
LOCAL BOOL setup_display(LPTFORM FAR *lppOutTForm, LPTFORM lpInTForm, HDC FAR* lpHDC, HWND hWnd)
//=======================================================================
{
	if (lppOutTForm && !*lppOutTForm)
	{
		TInit(lpInTForm);
		File2DispTFormEx(hWnd,lpInTForm, ISDRAWTOOL(Tool.id)/*YES*/);
		*lppOutTForm = lpInTForm;
	}
	// be sure we have an hDC
	if (lpHDC && !*lpHDC)
	{
		LPDISPLAY lpDisplay;
		*lpHDC = Window.hDC;
		lpDisplay = GetDisplayPtr(hWnd);
		SelectClipRect(*lpHDC, &lpDisplay->DispRect, NULL);
		return(TRUE);
	}
	return(FALSE);
}

//=======================================================================
//	Makes the current point in the given polygon into a bezier.
//	Does no visual updating.
//	Leaves the current point where it was.
//=======================================================================
LOCAL void make_curve(LPCEditPoly lpPoly)
//=======================================================================
{
	POINT pt;
	POINT handles1[2], handles2[2];
	POINT line[3];
	WORD wType;

	if (!lpPoly->CurrentPoint(&line[1]))
		return;
	wType = (WORD)lpPoly->PointType();
	// Do Nothing To Handles
	if (wType & POLY_CURVE_HANDLE)
		return;
	if (!PreviousControlPoint(lpPoly, &line[0]))
	{
		if (!NextControlPoint(lpPoly, &line[2]))
			return;	// 1 point case
		PreviousControlPoint(lpPoly, NULL);
		// 1st point case
		if (wType & POLY_CURVE_BEGIN)
			return;
		// make it a bezier
		SplitLine(&line[1],&line[2], &handles1[0], &handles1[1]);
		lpPoly->LineToBezier(&handles1[0], &handles1[1]);
		return;
	}
	NextControlPoint(lpPoly);
	if (!NextControlPoint(lpPoly, &line[2]))
	{
		// last point case
		if (wType & POLY_CURVE_END)
			return;
		if (!lpPoly->PreviousPoint(&pt))
			return;
		SplitLine(&line[0],&line[1], &handles1[0], &handles1[1]);
		if (!lpPoly->LineToBezier(&handles1[0], &handles1[1]))
		{
			lpPoly->NextPoint();
			return;
		}
		lpPoly->NextPoint(); // 1st handle
		lpPoly->NextPoint(); // 2nd handle
		lpPoly->NextPoint(); // original point
		return;
	}	
	PreviousControlPoint(lpPoly);
	// is it already a curve ?
	if ((wType&POLY_CURVE_BEGIN) && (wType&POLY_CURVE_END))
	{
		// get the points handles
		if (!lpPoly->PreviousPoint(&handles1[0]))
			return;
		lpPoly->NextPoint();
		if (!lpPoly->NextPoint(&handles1[1]))
			return;                   
		// restore current position
		if (!lpPoly->PreviousPoint(&pt))
			return;
		// is it already ok?
		if (is_linear(&handles1[0], &line[1], &handles1[1]))
			return;
	}
	// determine ideal handles 
	ComputeBezierHandles(line, handles1, handles2);
	// Make beziers
	// NEXT LINE SEGMENT
	if (wType & POLY_CURVE_BEGIN)
	{
		// adjust next handle
		if (!lpPoly->NextPoint(&pt))
			return;
		lpPoly->SetCurrentPoint(&handles2[0]);
		lpPoly->PreviousPoint();
	}
	else
	{
		// make next a bezier
		if (!lpPoly->LineToBezier(&handles2[0], &handles2[1]))
			return;
	}
	
	// PREVIOUS LINE SEGMENT
	if (wType & POLY_CURVE_END)
	{
		// adjust previous handle
		if (!lpPoly->PreviousPoint(&pt))
			return;
		lpPoly->SetCurrentPoint(&handles1[1]);
		lpPoly->NextPoint();
	}
	else
	{
		// make previous a bezier
		if (!lpPoly->PreviousPoint(&pt))
			return;
		if (!lpPoly->LineToBezier(&handles1[0], &handles1[1]))
		{
			lpPoly->NextPoint();
			return;
		}
		lpPoly->NextPoint(); // 1st handle
		lpPoly->NextPoint(); // 2nd handle
		lpPoly->NextPoint(); // original point
	}
}

//=======================================================================
//	Makes the current point in the given polygon into a corner.
//	Does no visual updating.
//	Leaves the current point where it was.
//=======================================================================
LOCAL void make_corner(LPCEditPoly lpPoly)
//=======================================================================
{
	POINT pt, dummy, end;
	WORD wType;
	BOOL bool;
	POINT handles[2];

	// is it already a line?
	wType = (WORD)lpPoly->PointType();
	if (!lpPoly->CurrentPoint(&pt))
		return;
	// do previous line?
	if (wType & POLY_CURVE_END)
	{
		// make previous sgement 'strait'
		if(PreviousControlPoint(lpPoly, &end))
		{
			NextControlPoint(lpPoly);
			SplitLine(&pt,&end, &handles[0], &handles[1]);
			if (!lpPoly->PreviousPoint(&dummy))
				return;
			lpPoly->SetCurrentPoint(&handles[0]);
			lpPoly->NextPoint();
		}
		
	}
	if (wType & POLY_CURVE_BEGIN)
	{
		// is the next point also selected?
		if(!NextControlPoint(lpPoly, &end))
			return;	// bad
		bool = (lpPoly->GetFlags() & SE_SELECTED);
		PreviousControlPoint(lpPoly);
		if (bool)
		{
			// its ok to make a real line
			lpPoly->BezierToLine();
			return;
		}
		
		// make forward sgement 'strait'
		SplitLine(&pt,&end, &handles[0], &handles[1]);
		if (!lpPoly->NextPoint(&dummy))
			return;
		lpPoly->SetCurrentPoint(&handles[0]);
		lpPoly->PreviousPoint();
	}
}


//=======================================================================
// 	Returns true if the points are colinear 
//	(and in order 1,2,3 should be added)
//=======================================================================
LOCAL BOOL is_linear(LPPOINT p1, LPPOINT p2, LPPOINT p3)
//=======================================================================
{     
	LONG l, d1, d2;
	LONG p1x,p1y,p3x,p3y;
	// any 0 length line will do
	if (PTEQ(*p1, *p2))
		return(TRUE);
	if (PTEQ(*p2, *p3))
		return(TRUE);
	// translate to p2 = (0,0)
	p1x -= p1->x-p2->x;
	p1y -= p1->y-p2->y;
	p3x -= p3->x-p2->x;
	p3y -= p3->y-p2->y;
	// must have p1*~p3 = 0 (orthoginal dot product = 0)
	l = labs(p1x*p3y - p1y*p3x);
	d1 = (LONG)sqrt(p1x*p1x + p1y*p1y);
	d2 = (LONG)sqrt(p3x*p3x + p3y*p3y);
	d1 = Min(d1,d2);
	if (!d1)
		return(FALSE);
	l /= d1;
	if (l > 1)	
		return(FALSE);
	return(TRUE);
}


//************************************************************************
// Rotates line(lpIn, lpBase) around lpBase to match the rotation of the 
//	line (lpOld,lpBase)	to which produced line (lpNew, lpBase).
//	Puts result in lpOut
//************************************************************************
LOCAL void rotate_point(LPPOINT lpOut, LPPOINT lpIn, LPPOINT lpOld,LPPOINT lpNew,LPPOINT lpBase)
//************************************************************************
{
	long x1, y1, x2, y2, u1, v1, u2, v2;
	long r, r2, sum1, sum2;
	long shift;
	
	x1 = lpIn->x - lpBase->x;
	y1 = lpIn->y - lpBase->y;
	u1 = lpOld->x - lpBase->x;
	v1 = lpOld->y - lpBase->y;
	u2 = lpNew->x - lpBase->x;
	v2 = lpNew->y - lpBase->y;
    
   	*lpOut = *lpIn;
	if( (u1==0 && v1==0) || (u2 == 0 && v2 == 0) || (x1==0 && y1==0))
		return;
	r = (u1*u1+v1*v1);
	r = (LONG)sqrt(r);
	r2 = (u2*u2+v2*v2);
	r *= (LONG)sqrt(r2);
	// be sure result can fit
	shift = 0;
	if (r>0xffff)
	{
		if (r<=0xffffff)
			shift = 8;
		else
			shift = 16;	    
	}
	r >> shift;
	sum1 = (x1*u1+y1*v1)>>shift;
	sum2 = (y1*u1-x1*v1)>>shift;
	x2= (sum1*u2-sum2*v2)/r;
	y2 = (sum1*v2+sum2*u2)/r;
	if (WITHIN(x2,-1,1) && WITHIN (y2,-1,1))
		return;
	lpOut->x = x2+lpBase->x;
	lpOut->y = y2+lpBase->y;
}


//=======================================================================
//=======================================================================
LOCAL WORD is_handle(LPCPolyList lpPolyList, LPSEPOS lpPos)
//=======================================================================
{
	LPCPoly lpPoly;
	
	// get the polygon
	lpPoly = (LPCPoly)lpPolyList->GetAtPtr(lpPos->pos);
	if (!lpPoly)
		return(FALSE);
	lpPoly->GetPoint(lpPos->num);
	return(lpPoly->IsHandle());
}

//=======================================================================
//=======================================================================
LOCAL WORD is_selected(LPCPolyList lpPolyList, LPSEPOS lpPos)
//=======================================================================
{
	LPCEditPoly lpPoly;
	
	// get the polygon
	lpPoly = (LPCEditPoly)lpPolyList->GetAtPtr(lpPos->pos);
	if (!lpPoly)
		return(FALSE);
	return(lpPoly->GetFlags(lpPos->num) & SE_SELECTED);
}

//=======================================================================
// deletes current point from poly, even if its part of bezier.  If it 
// is part of a bezier then it acts appropraitely for point editing.
// The current point is the point after those deleted.
// Returns: the number of points deleted (0 for error).
//=======================================================================
LOCAL int delete_poly_point(LPCEditPoly lpPoly)
//=======================================================================
{
	WORD wType;
	int org;
	POINT ptHandles[2];
	POINT pt;
	int del = 0;
	
	wType = (WORD)lpPoly->PointType();
	if (!wType)
	{
		// simple point
		if (lpPoly->DeletePoint())
			return(1);
		return(0);
	}
	// MUST BE BEZIER
	org = lpPoly->CurrentIndex();
	// can't delete handle
	if (wType & POLY_CURVE_HANDLE)
		return(0);
	// make any curves into line segments
	if (wType & POLY_CURVE_BEGIN)
	{
		lpPoly->NextPoint();
		if (!lpPoly->NextPoint(&ptHandles[1])) goto ExitError;
		lpPoly->PreviousPoint();
		lpPoly->PreviousPoint();
		if (!lpPoly->BezierToLine()) goto ExitError;
		del +=2;
	}
	if (wType & POLY_CURVE_END)
	{
		lpPoly->PreviousPoint();
		if (!lpPoly->PreviousPoint(&ptHandles[0])) goto ExitError;
		if (!lpPoly->PreviousPoint(&pt)) goto ExitError;
		if (!lpPoly->BezierToLine()) goto ExitError;
		del +=2;
		if (!lpPoly->NextPoint(&pt)) goto ExitError;
	}
	// delete the point
	if (!lpPoly->DeletePoint()) goto ExitError;
	// turn back into bezier if needed
	if ((wType & POLY_CURVE_BEGIN) && (wType & POLY_CURVE_END))
	{
		if (!lpPoly->PreviousPoint(&pt)) goto ExitError;
	  	if (lpPoly->LineToBezier(&ptHandles[0], &ptHandles[1]))
			del-=2;
		lpPoly->NextPoint(); // 1st handle
		lpPoly->NextPoint(); // 2nd handle
		lpPoly->NextPoint(); // end point
	}
	return(del);

ExitError:
	lpPoly->GetPoint(org);
	return(0);
}

//************************************************************************
//		backup_whole
// DESCRIPTION:
//			Finds the first non handle point behind the given point.
//			Does not change the current position.
// RETURNS:
//			The index of the point found
//************************************************************************
LOCAL int backup_whole(LPCPoly lpPoly, int index)
//************************************************************************
{
	int oldpos;
	POINT ptDummy;
	
	oldpos = lpPoly->CurrentIndex();
	if (!lpPoly->GetPoint(index, &ptDummy))
		return(index);
	if (!PreviousControlPoint(lpPoly))
		return(index);
	index = lpPoly->CurrentIndex();
	lpPoly->GetPoint(oldpos);
	return(index);
}							  

//************************************************************************
//		forward_whole
// DESCRIPTION:
//			Finds the first non handle point past the given point.
//			Does not change the current position.
// RETURNS:
//			The index of the point found
//************************************************************************
LOCAL int forward_whole(LPCPoly lpPoly, int index)
//************************************************************************
{
	int oldpos;
	POINT ptDummy;
	
	oldpos = lpPoly->CurrentIndex();
	if (!lpPoly->GetPoint(index, &ptDummy))
		return(index);
	if (!NextControlPoint(lpPoly))
		return(index);
	index = lpPoly->CurrentIndex();
	lpPoly->GetPoint(oldpos);
	return(index);
}

