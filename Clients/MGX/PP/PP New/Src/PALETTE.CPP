//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

LOCAL UINT Palette_OnGetDlgCode(HWND hWindow, LPMSG lpMsg);
LOCAL BOOL Palette_OnDragQuery(HWND hWindow, WORD wState, int x, int y);
LOCAL BOOL Palette_OnDragAccept(HWND hWindow, LPDRAG lpDrag);
LOCAL void Palette_OnPaint(HWND hWindow);
LOCAL void Palette_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Palette_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Palette_HandleButtonUp(HWND hWindow, int x, int y, UINT keyFlag, BOOL fRightButton);
LOCAL void Palette_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Palette_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Palette_OnRButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Palette_HandleButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags, BOOL fRightButton);
LOCAL void Palette_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Palette_HandleMouseMove(HWND hWindow, int x, int y, UINT keyFlags, BOOL fMouseMove);
LOCAL void Palette_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
LOCAL void Palette_OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
LOCAL BOOL Palette_HandleScroll( HWND hWnd, BOOL fVScroll, UINT code, int pos);
LOCAL void Palette_OnKey(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Palette_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Palette_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Palette_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void Palette_OnSelectPalette(HWND hWindow, int iSelect);
LOCAL void Palette_OnSetPalette(HWND hWindow, LPPALETTE lpPalette, int iSelect);
LOCAL void Palette_OnDestroy(HWND hWindow);
LOCAL void Palette_Paint( HDC hDC, HWND hWnd );
LOCAL void Palette_Select( HDC hDC, HWND hWnd, int iSelect, BOOL on );
LOCAL BOOL Palette_SelectRect(HWND hWnd, int iSelect, LPRECT lpSelectRect);
LOCAL int Palette_GetEntry( HWND hWnd, int x, int y, BOOL fClosest );
LOCAL void Palette_SelectEntry( HWND hWnd, int iSelect );
LOCAL void Palette_AutoScroll(HWND hWindow, LPINT lpX, LPINT lpY, LPRECT lpRect);
LOCAL void Palette_GetRowCol( HWND hWnd, int iSelect, LPINT lpRow, LPINT lpCol );

static BOOL bHasFocus, bTrack, fLastIn;
static WORD wMode;
static POINT StartPos;
static LPDRAG lpMyDrag;

/***********************************************************************/
LONG WINPROC EXPORT Palette_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_PALETTECHANGED, Control_OnPaletteChanged);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);

	HANDLE_MSG(hWindow, WM_GETDLGCODE, Palette_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_DRAGQUERY, Palette_OnDragQuery);
	HANDLE_MSG(hWindow, WM_DRAGACCEPT, Palette_OnDragAccept);
	HANDLE_MSG(hWindow, WM_PAINT, Palette_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Palette_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_RBUTTONUP, Palette_OnRButtonUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Palette_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Palette_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_RBUTTONDOWN, Palette_OnRButtonDown);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Palette_OnMouseMove);
	HANDLE_MSG(hWindow, WM_HSCROLL, Palette_OnHScroll);
	HANDLE_MSG(hWindow, WM_VSCROLL, Palette_OnVScroll);
	HANDLE_MSG(hWindow, WM_KEYDOWN, Palette_OnKey);
	HANDLE_MSG(hWindow, WM_SELECTPALETTE, Palette_OnSelectPalette);
	HANDLE_MSG(hWindow, WM_SETPALETTE, Palette_OnSetPalette);
	HANDLE_MSG(hWindow, WM_DESTROY, Palette_OnDestroy);
	
	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL UINT Palette_OnGetDlgCode(HWND hWindow, LPMSG lpMsg)
/***********************************************************************/
{
return( DLGC_WANTALLKEYS );
}

/***********************************************************************/
LOCAL BOOL Palette_OnDragQuery(HWND hWindow, WORD wState, int x, int y)
/**********************************************************************/
{
LPPALETTE lpPalette;

lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (!lpPalette)
	return(FALSE);
return(wState == DAD_RGB);
}

/***********************************************************************/
LOCAL BOOL Palette_OnDragAccept(HWND hWindow, LPDRAG lpDrag)
/***********************************************************************/
{
LPPALETTE lpPalette;
LPCOLORINFO lpColorInfo;
int iCurrent, iSelect;

lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (!lpPalette)
	return(FALSE);
lpColorInfo = (LPCOLORINFO)lpDrag->lpData;
iCurrent = (int)GetWindowLong(hWindow, GWL_SELECT);
if (lpDrag->hWnd == hWindow)
	{
	iSelect = Palette_GetEntry(hWindow, lpDrag->pos.x, lpDrag->pos.y, NO);
	if (iSelect < 0)
		return(FALSE);
	MovePaletteColor(GetParent(hWindow), iCurrent, iSelect);
	}
else
	{
	iSelect = Palette_GetEntry(hWindow, lpDrag->pos.x, lpDrag->pos.y, YES);
	if (iSelect < 0)
		iSelect = 0;
	if (iSelect >= lpPalette->iColors)
		{
		iSelect = lpPalette->iColors-1;
		InsertPaletteColor(GetParent(hWindow), iSelect, lpColorInfo, NO);
		}
	else
		InsertPaletteColor(GetParent(hWindow), iSelect, lpColorInfo, YES);
	}
return(TRUE);
}

/***********************************************************************/
LOCAL void Palette_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
HDC hDC;
RECT ClientRect;
LPPALETTE lpPalette;

if ( SuperPaint( hWindow ) )
	return; // Printing in progress... (SuperBlt not reentrant)
hDC = BeginPaint( hWindow, &ps );
GetClientRect( hWindow, &ClientRect );
lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (lpPalette)
	{
	EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_CUSTOM );
	if (bHasFocus)
		{
		InflateRect(&ClientRect, -1, -1);
		FrameRect( hDC,&ClientRect,(HBRUSH)GetStockObject(BLACK_BRUSH));
		}
	Palette_Paint(hDC, hWindow);
	Palette_Select(hDC, hWindow, (int)GetWindowLong(hWindow, GWL_SELECT), ON);
	}
else
	{
	EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_STATIC );
	}
EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Palette_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Palette_HandleButtonUp(hWindow, x, y, keyFlags, NO);
}

/***********************************************************************/
LOCAL void Palette_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Palette_HandleButtonUp(hWindow, x, y, keyFlags, YES);
}

/***********************************************************************/
LOCAL void Palette_HandleButtonUp(HWND hWindow, int x, int y, UINT keyFlags,
								BOOL fRightButton)
/***********************************************************************/
{
if ( !bTrack )
	return;
ReleaseCapture(); bTrack = FALSE;
if (lpMyDrag)
	{
	DragAndDropEnd(lpMyDrag);
	lpMyDrag = NULL;
	}
}

/***********************************************************************/
LOCAL void Palette_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
LPPALETTE lpPalette;

lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (!lpPalette || !lpPalette->iColors)
	return;
FORWARD_WM_COMMAND(GetParent(hWindow), IDC_PALETTE, hWindow, 1, SendMessage);
}

/***********************************************************************/
LOCAL void Palette_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Palette_HandleButtonDown(hWindow, fDoubleClick, x, y, keyFlags, NO);
}

/***********************************************************************/
LOCAL void Palette_OnRButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Palette_HandleButtonDown(hWindow, fDoubleClick, x, y, keyFlags, YES);
}

/***********************************************************************/
LOCAL void Palette_HandleButtonDown(HWND hWindow, BOOL fDoubleClick,
									int x, int y, UINT keyFlags,
									BOOL fRightButton)
/***********************************************************************/
{
if ( bTrack )
	return;
SetCapture( hWindow ); bTrack = TRUE;
if ( GetFocus() != hWindow )
	SetFocus( hWindow );
wMode = (fRightButton ? 2 : 0);
StartPos.x = x; StartPos.y = y;
Palette_HandleMouseMove(hWindow, x, y, keyFlags, NO);
}

/***********************************************************************/
LOCAL void Palette_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
Palette_HandleMouseMove(hWindow, x, y, keyFlags, YES);
}

/***********************************************************************/
LOCAL void Palette_HandleMouseMove(HWND hWindow, int x, int y, UINT keyFlags,
									BOOL fMouseMove)
/***********************************************************************/
{
LPPALETTE lpPalette;
int iCurrent, iSelect;
RECT ClientRect, SelectRect;
HDC hDC;
POINT CursorPt;

// doing Drag and Drop?
if (lpMyDrag)
	{
	iSelect = Palette_GetEntry(hWindow, x, y, NO);
	GetDrawClientRect(hWindow, &ClientRect);
	InflateRect(&ClientRect, -3, -3);
	if (fLastIn && iSelect < 0)
		Palette_AutoScroll(hWindow, &x, &y, &ClientRect);
	CursorPt.x = x; CursorPt.y = y;
	++ClientRect.right; ++ClientRect.bottom;
	fLastIn = PtInRect(&ClientRect, CursorPt);
	DragAndDropMove(lpMyDrag, x, y);
	return;
	}

// do we current have a palette?
lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (!lpPalette || !lpPalette->iColors)
	return;

// get selection based on mouse location
iSelect = Palette_GetEntry(hWindow, x, y, NO);
// get current selection
iCurrent = (int)GetWindowLong(hWindow, GWL_SELECT);

// if not tracking just make inform dialog of user browsing
// if tracking in current selection, inform dialog of user browsing
if (!bTrack || (iSelect == iCurrent && fMouseMove))
	{
	// need to save iSelect somewhere for browsing
	FORWARD_WM_COMMAND(GetParent(hWindow), IDC_PALETTE, hWindow, 3, SendMessage);
	return;
	}

// if we have moved out of current selection start drag and drop
if (SHIFT && bTrack && iSelect != iCurrent && fMouseMove)
	{
	Palette_SelectRect(hWindow, iCurrent, &SelectRect);
	ClientToScreen(hWindow, &StartPos);
	ClientToScreen(hWindow, (LPPOINT)&SelectRect.left);
	ClientToScreen(hWindow, (LPPOINT)&SelectRect.right);
	lpMyDrag = DragAndDropBegin(hWindow, DAD_RGB,
		(LPTR)&lpPalette->lpColors[iCurrent],
		&StartPos, NULL, NULL);
	fLastIn = YES;
	return;
	}
// mouse down in entry, may or may not be a new entry
if (iSelect >= 0)
	{
	if (iSelect != iCurrent)
		{
		hDC = GetDC(hWindow);
		Palette_Select(hDC, hWindow, iCurrent, OFF);
		Palette_Select(hDC, hWindow, iSelect, ON);
		ReleaseDC(hWindow, hDC);
		SetWindowLong(hWindow, GWL_SELECT, (LONG)iSelect);
		}
	FORWARD_WM_COMMAND(GetParent(hWindow), IDC_PALETTE, hWindow, wMode, SendMessage);
	}
else
	{
	GetDrawClientRect(hWindow, &ClientRect);
	InflateRect(&ClientRect, -3, -3);
	Palette_AutoScroll(hWindow, &x, &y, &ClientRect);
	iSelect = Palette_GetEntry(hWindow, x, y, NO);
	if (iSelect >= 0 && iSelect != iCurrent)
		{
		hDC = GetDC(hWindow);
		Palette_Select(hDC, hWindow, iCurrent, OFF);
		Palette_Select(hDC, hWindow, iSelect, ON);
		ReleaseDC(hWindow, hDC);
		SetWindowLong(hWindow, GWL_SELECT, (LONG)iSelect);
		FORWARD_WM_COMMAND(GetParent(hWindow), IDC_PALETTE, hWindow, wMode, SendMessage);
		}
	}
}

/***********************************************************************/
LOCAL void Palette_OnHScroll(HWND hWindow, HWND hwndCtl, UINT code, int pos)
/***********************************************************************/
{
Palette_HandleScroll(hWindow, NO, code, pos);
}

/***********************************************************************/
LOCAL void Palette_OnVScroll(HWND hWindow, HWND hwndCtl, UINT code, int pos)
/***********************************************************************/
{
Palette_HandleScroll(hWindow, YES, code, pos);
}

/***********************************************************************/
LOCAL void Palette_OnKey(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
int iRowIncr, iColIncr, iSelect, iCurrent, iRows, iCols, iStart;
LPPALETTE lpPalette;
RECT ClientRect;

if (vk == VK_UP || vk == VK_DOWN ||
    vk == VK_LEFT || vk == VK_RIGHT ||
    vk == VK_RETURN)
	{
	// get info all keystrokes need
	lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
	if (!lpPalette || !lpPalette->iColors)
		return;

	iCurrent = (int)GetWindowLong(hWindow, GWL_SELECT);
	GetChipLayout(hWindow, &ClientRect, &iRows, &iCols, &iRowIncr, &iColIncr,
		 &iStart);
	}

switch (vk)
	{
	case VK_DELETE:
		SendMessage(GetParent(hWindow), WM_COMMAND, IDC_PALDELETEITEM, 0L);
		break;
//	case VK_INSERT:
//		SendMessage(GetParent(hWindow), WM_COMMAND,
//			SHIFT ? IDC_PALAPPEND : IDC_PALINSERT, 0L);
//		break;
	case VK_UP:
		iSelect = iCurrent - iRowIncr;
		if (iSelect < 0)
			break;
		Palette_SelectEntry(hWindow, iSelect);
		break;

	case VK_DOWN:
		iSelect = iCurrent + iRowIncr;
		if (iSelect >= lpPalette->iColors)
			break;
		Palette_SelectEntry(hWindow, iSelect);
		break;

	case VK_LEFT:
		iSelect = iCurrent - iColIncr;
		if (iSelect < 0)
			break;
		Palette_SelectEntry(hWindow, iSelect);
		break;

	case VK_RIGHT:
		iSelect = iCurrent + iColIncr;
		if (iSelect >= lpPalette->iColors)
			break;
		Palette_SelectEntry(hWindow, iSelect);
		break;

	case VK_RETURN:
		iCurrent = (int)GetWindowLong(hWindow, GWL_SELECT);
		FORWARD_WM_COMMAND(GetParent(hWindow), IDC_PALETTE, hWindow, 1, SendMessage);
		break;

	default:
		break;
	}
}

/***********************************************************************/
LOCAL void Palette_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
Palette_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
LOCAL void Palette_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
Palette_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
LOCAL void Palette_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
LPPALETTE lpPalette;
RECT ClientRect;
HDC hDC;
HBRUSH hBrush;

if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
	return;
bHasFocus = fSetFocus;
lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (!lpPalette)
	return;
hDC = GetDC( hWindow );
GetClientRect( hWindow, &ClientRect );
InflateRect(&ClientRect, -1, -1);
hBrush = GetBackgroundBrush( hDC, hWindow, CTLCOLOR_CUSTOM );
FrameRect( hDC, &ClientRect,
	(bHasFocus ? (HBRUSH)GetStockObject(BLACK_BRUSH) : hBrush) );
ReleaseDC( hWindow, hDC );
}

/***********************************************************************/
LOCAL void Palette_OnSelectPalette(HWND hWindow, int iSelect)
/***********************************************************************/
{
Palette_SelectEntry(hWindow, iSelect);
}

/***********************************************************************/
LOCAL void Palette_OnSetPalette(HWND hWindow, LPPALETTE lpPalette, int iSelect)
/***********************************************************************/
{
int iColors, iCount;
int iRows, iRowIncr, iCols, iColIncr, iStart;
RECT ClientRect;
BOOL fSetScrollPos;

SetWindowLong(hWindow, GWL_PALETTE, (long)lpPalette);
// Turn off scroll bars to start
SetScrollRange(hWindow, SB_HORZ, 0, 0, YES);
SetScrollRange(hWindow, SB_VERT, 0, 0, YES);
if ( !lpPalette )
	{
	InvalidateRect(hWindow, NULL, TRUE);
	return;
	}
if ( !(iColors = lpPalette->iColors) )
	iColors = 1;
fSetScrollPos = (iSelect < 0);
if ( iSelect < 0 )
	iSelect = 0;
else
if (iSelect >= iColors)
	iSelect = iColors - 1;
SetWindowLong(hWindow, GWL_SELECT, (LONG)iSelect);
if (lpPalette->iColors)
	{
	FORWARD_WM_COMMAND(GetParent(hWindow), IDC_PALETTE, hWindow, 3, SendMessage);
	}
GetChipLayout(hWindow, &ClientRect, &iRows, &iCols, &iRowIncr, &iColIncr,
	&iStart);
if ( iRowIncr > 1 )
	{ // Fill left to right, row at a time - i.e., vertical scroll bars
	iCount = (iColors + (iCols-1)) / iCols;
	if ( iCount > iRows )
		{ // Add a vertical scroll bar - start with a dummy range
		SetScrollRange(hWindow, SB_VERT, 0, 1, NO);
		// Since we added a scroll bar, GetChipLayout() should
		// be called again to update the values
		GetChipLayout(hWindow, &ClientRect, &iRows, &iCols,
			&iRowIncr, &iColIncr, &iStart);
		// Set the proper scroll range
		iCount = (iColors + (iCols-1)) / iCols;
		SetScrollRange(hWindow, SB_VERT, 0, iCount-iRows, YES);
		if ( !fSetScrollPos )
			SetScrollPos(hWindow, SB_VERT, 0, YES);
		}
	}
else
if ( iColIncr > 1 )
	{ // Fill top to bottom, col at a time - horizontal scroll bars
	iCount = (iColors + (iRows-1)) / iRows;
	if ( iCount > iCols )
		{ // Add a horizontal scroll bar - start with a dummy range
		SetScrollRange(hWindow, SB_HORZ, 0, 1, NO);
		// Since we added a scroll bar, GetChipLayout() should
		// be called again to update the values
		GetChipLayout(hWindow, &ClientRect, &iRows, &iCols,
			&iRowIncr, &iColIncr, &iStart);
		// Set the proper scroll range
		iCount = (iColors + (iRows-1)) / iRows;
		SetScrollRange(hWindow, SB_HORZ, 0, iCount-iCols, YES);
		if ( !fSetScrollPos )
			SetScrollPos(hWindow, SB_HORZ, 0, YES);
		}
	}
InvalidateRect(hWindow, NULL, YES);
}

/***********************************************************************/
LOCAL void Palette_OnDestroy(HWND hWindow)
/***********************************************************************/
{
if ( bTrack )
	{
	ReleaseCapture();
	bTrack = NO;
	}
}

/***********************************************************************/
LOCAL void Palette_Paint( HDC hDC, HWND hWindow )
/***********************************************************************/
{
RECT ClientRect;
LPPALETTE lpPalette;
int iRowIncr, iColIncr, iWidth, iHeight, iRows, iCols;
int iStart, iEntry, r, c, xCount, yCount;
LPRGB lpRGB, lpLine;
LFIXED xrate, yrate;
BLTSESSION BltSession;
FRMTYPEINFO TypeInfo;

// get pointer to palette information
lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (!lpPalette)
	return;

GetClientRect(hWindow, &ClientRect);
FrameRect(hDC, &ClientRect, (HBRUSH)GetStockObject(BLACK_BRUSH) );

if (!lpPalette->iColors)
	return;

// get ClientRect and allocate buffer for SuperBlt
GetChipLayout(hWindow, &ClientRect, &iRows, &iCols, &iRowIncr, &iColIncr, &iStart);

iWidth = RectWidth(&ClientRect);
iHeight = RectHeight(&ClientRect);

lpRGB = (LPRGB)Alloc((long)iWidth*3L);
if (!lpRGB)
	return;

yrate = FGET(iHeight, iRows);
xrate = FGET(iWidth, iCols);
FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
StartSuperBlt( &BltSession, hDC, NULL, lpBltScreen, &ClientRect, TypeInfo,
	10, 0, 0, YES, NULL, NULL );
for (r = 0; r < iRows; ++r)
	{
	yCount = FMUL(r+1, yrate) - FMUL(r, yrate);
	set24(lpRGB, iWidth, RGB(255,255,255));
	SuperBlt(&BltSession, (LPTR)lpRGB);
	yCount -= 2;
	while (--yCount >= 0)
		{
		iEntry = iStart + (r * iRowIncr);
		lpLine = lpRGB;
		set24(lpLine, iWidth, RGB(255,255,255));
		for (c = 0; c < iCols; ++c)
			{
			xCount = FMUL(c+1, xrate) - FMUL(c, xrate);

			if (iEntry < lpPalette->iColors)
				set24(lpLine+1, xCount-2, 
					RGB2long(lpPalette->lpColorInfo[iEntry].rgb));
			lpLine += xCount;
			iEntry += iColIncr;
			}
		SuperBlt(&BltSession, (LPTR)lpRGB);
		}
	set24(lpRGB, iWidth, RGB(255,255,255));
	SuperBlt(&BltSession, (LPTR)lpRGB);
	}
SuperBlt(&BltSession, NULL);
FreeUp((LPTR)lpRGB);
}

/***********************************************************************/
LOCAL void Palette_Select( HDC hDC, HWND hWindow, int iSelect, BOOL on )
/***********************************************************************/
{
RECT SelectRect;
int x1, x2, y1, y2;
short OldROP;
HPEN hOldPen;

if (!Palette_SelectRect(hWindow, iSelect, &SelectRect))
	return;

y1 = SelectRect.top;
y2 = SelectRect.bottom;
x1 = SelectRect.left;
x2 = SelectRect.right;

if (on)
	hOldPen = (HPEN)SelectObject( hDC, GetStockObject(BLACK_PEN) );
else
	hOldPen = (HPEN)SelectObject( hDC, GetStockObject(WHITE_PEN) );
MoveTo(hDC, x1, y1);
LineTo(hDC, x2, y1);
LineTo(hDC, x2, y2);
LineTo(hDC, x1, y2);
LineTo(hDC, x1, y1);
SelectObject(hDC, hOldPen);

OldROP = SetROP2(hDC, R2_NOT);
MoveTo(hDC, x1+1, y1+1);
LineTo(hDC, x2-1, y1+1);
LineTo(hDC, x2-1, y2-1);
LineTo(hDC, x1+1, y2-1);
LineTo(hDC, x1+1, y1+1);
SetROP2(hDC, OldROP);
}

/***********************************************************************/
LOCAL BOOL Palette_SelectRect(HWND hWindow, int iSelect, LPRECT lpSelectRect)
/***********************************************************************/
{
RECT ClientRect;
int iRowIncr, iColIncr, iWidth, iHeight, iRows, iCols;
int iStart, iEntry, r, c;
LFIXED xrate, yrate;
LPPALETTE lpPalette;

lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (!lpPalette || !lpPalette->iColors)
	return(FALSE);

// get ClientRect and allocate buffer for SuperBlt
GetChipLayout(hWindow, &ClientRect, &iRows, &iCols, &iRowIncr, &iColIncr, &iStart);

iWidth = RectWidth(&ClientRect);
iHeight = RectHeight(&ClientRect);

yrate = FGET(iHeight, iRows);
xrate = FGET(iWidth, iCols);

for (r = 0; r < iRows; ++r)
	{
	iEntry = iStart + (r * iRowIncr);
	for (c = 0; c < iCols; ++c)
		{
		if (iEntry == iSelect)
			{
			lpSelectRect->top = ClientRect.top + FMUL(r, yrate);
			lpSelectRect->bottom = ClientRect.top + FMUL(r+1, yrate) - 1;
			lpSelectRect->left = ClientRect.left + FMUL(c, xrate);
			lpSelectRect->right = ClientRect.left + FMUL(c+1, xrate) - 1;
			return(TRUE);
			}
		iEntry += iColIncr;
		}
	}
return(FALSE);
}

/***********************************************************************/
LOCAL int Palette_GetEntry( HWND hWindow, int x, int y, BOOL fClosest )
/***********************************************************************/
{
LPPALETTE lpPalette;
RECT ClientRect;
int iRowIncr, iColIncr, iWidth, iHeight, iRows, iCols;
int iStart, iEntry, r, c;
int x1, x2, y1, y2, iMinDist, iDist, dx, dy, iClosest;
LFIXED xrate, yrate;

lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (!lpPalette || !lpPalette->iColors)
	return(-1);

// get ClientRect and allocate buffer for SuperBlt
GetChipLayout(hWindow, &ClientRect, &iRows, &iCols, &iRowIncr, &iColIncr, &iStart);

iWidth = RectWidth(&ClientRect);
iHeight = RectHeight(&ClientRect);

yrate = FGET(iHeight, iRows);
xrate = FGET(iWidth, iCols);

x -= ClientRect.left;
y -= ClientRect.top;
iMinDist = INT_MAX;
iClosest = -1;
for (r = 0; r < iRows; ++r)
	{
	y1 = FMUL(r, yrate);
	y2 = FMUL(r+1, yrate) - 1;
	dy = min(abs(y-y1), abs(y-y2));
	iEntry = iStart + (r * iRowIncr);
	for (c = 0; c < iCols; ++c)
		{
		if (iEntry < lpPalette->iColors || fClosest)
			{
			x1 = FMUL(c, xrate);
			x2 = FMUL(c+1, xrate) - 1;
			if (y >= y1 && y <= y2 &&
			    x >= x1 && x <= x2)
				return(iEntry);
			else if (fClosest)
				{
				dx = min(abs(x-x1), abs(x-x2));
				iDist = min(dx, dy);
				if (iDist < iMinDist)
					{
					iClosest = iEntry;
					iMinDist = iDist;
					}
				}
			}
		iEntry += iColIncr;
		}
	}
return(iClosest);
}

/***********************************************************************/
LOCAL BOOL Palette_HandleScroll( HWND hWindow, BOOL fVScroll, UINT code, int pos)
/************************************************************************/
{
int Value, Position, MinPos, MaxPos, xChipSize, yChipSize;
int Type, iRows, iCols, iRowIncr, iColIncr, iStart;
RECT ClientRect;
LPPALETTE lpPalette;

// get pointer to palette information
lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (!lpPalette || !lpPalette->iColors)
	return(FALSE);

if ( fVScroll )
	{
	if ( !PaletteVertical() )
		return( FALSE );
	Type = SB_VERT;
	}
else
	{
	if ( PaletteVertical() )
		return( FALSE );
	Type = SB_HORZ;
	}

Value = Position = GetScrollPos(hWindow, Type);
GetScrollRange(hWindow, Type, &MinPos, &MaxPos);
GetChipLayout(hWindow, &ClientRect, &iRows, &iCols, &iRowIncr, &iColIncr, &iStart);

switch (code)
	{
	case SB_LINEUP:
		Value -= 1;
		break;
	case SB_LINEDOWN:
		Value += 1;
		break;
	case SB_PAGEUP:
		if (Type == SB_VERT)
				Value -= (iRows-1);
		else	Value -= (iCols-1);
		Value = bound(Value, MinPos, MaxPos);
		break;
	case SB_PAGEDOWN:
		if (Type == SB_VERT)
				Value += (iRows-1);
		else	Value += (iCols-1);
		Value = bound(Value, MinPos, MaxPos);
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		Value = pos;
		break;
	}

if ( Value == Position || Value < MinPos || Value > MaxPos)
	return(FALSE);

SetScrollPos(hWindow, Type, Value, YES);
GetChipSize(&xChipSize, &yChipSize);
Value -= Position;
Value = -Value;
ClientRect.bottom++; ClientRect.right++;
if (Type == SB_VERT)
   {
//   xChipSize--;
   Value = (Value * xChipSize);
   }
else
   {
//   yChipSize--;
   Value = (Value * yChipSize);
   }
if (Type == SB_VERT)
	ScrollWindow( hWindow, 0, Value, &ClientRect, &ClientRect );
else
	ScrollWindow( hWindow, Value, 0, &ClientRect, &ClientRect );
return(TRUE);
}

/***********************************************************************/
LOCAL void PaletteKeystrokes( HWND hWindow, WORD wKey )
/***********************************************************************/
{
}

/***********************************************************************/
LOCAL void Palette_SelectEntry( HWND hWindow, int iSelect )
/***********************************************************************/
{
int iRowIncr, iColIncr, iCurrent, iRows, iCols, iStart;
int iFirstRow, iLastRow, iFirstCol, iLastCol, iCol, iRow;
RECT ClientRect;
HDC hDC;
LPPALETTE lpPalette;

// get info all keystrokes need
lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE);
if (!lpPalette)
	return;

iCurrent = (int)GetWindowLong(hWindow, GWL_SELECT);
GetChipLayout(hWindow, &ClientRect, &iRows, &iCols, &iRowIncr, &iColIncr, &iStart);
iFirstCol = GetScrollPos(hWindow, SB_HORZ);
iLastCol = iFirstCol + iCols - 1;
iFirstRow = GetScrollPos(hWindow, SB_VERT);
iLastRow = iFirstRow + iRows - 1;

hDC = GetDC(hWindow);
Palette_Select(hDC, hWindow, iCurrent, OFF);
Palette_Select(hDC, hWindow, iSelect, ON);
ReleaseDC(hWindow, hDC);
SetWindowLong(hWindow, GWL_SELECT, (LONG)iSelect);
FORWARD_WM_COMMAND(GetParent(hWindow), IDC_PALETTE, hWindow, 0, SendMessage);
Palette_GetRowCol(hWindow, iSelect, &iRow, &iCol);
if (iRow < iFirstRow)
	iFirstRow = iRow;
else if (iRow > iLastRow)
	iFirstRow += (iRow-iLastRow);
if (iCol < iFirstCol)
	iFirstCol = iCol;
else if (iCol > iLastCol)
	iFirstCol += (iCol-iLastCol);
Palette_HandleScroll(hWindow, YES, SB_THUMBPOSITION, iFirstRow);
Palette_HandleScroll(hWindow, NO, SB_THUMBPOSITION, iFirstCol);
}

/***********************************************************************/
LOCAL void Palette_AutoScroll(HWND hWindow, LPINT lpX, LPINT lpY, LPRECT lpRect)
/***********************************************************************/
{
BOOL	fScrolled;
POINT	CursorPt;

if (*lpX < lpRect->left)
	{
	fScrolled = Palette_HandleScroll(hWindow, NO, SB_LINEUP, 0);
	if (fScrolled)
		*lpX = lpRect->left;
	}
else if (*lpX > lpRect->right)
	{
	fScrolled = Palette_HandleScroll(hWindow, NO, SB_LINEDOWN, 0);
	if (fScrolled)
		*lpX = lpRect->right;
	}
else if (*lpY < lpRect->top)
	{
	fScrolled = Palette_HandleScroll(hWindow, YES, SB_LINEUP, 0);
	if (fScrolled)
		*lpY = lpRect->top;
	}
else if (*lpY > lpRect->bottom)
	{
	fScrolled = Palette_HandleScroll(hWindow, YES, SB_LINEDOWN, 0);
	if (fScrolled)
		*lpY = lpRect->bottom;
	}
else
	fScrolled = NO;
if (fScrolled)
	{
	UpdateWindow(hWindow);
	CursorPt.x = *lpX;
	CursorPt.y = *lpY;
	ClientToScreen(hWindow, &CursorPt);
	SetCursorPos(CursorPt.x, CursorPt.y);
	}
}

/***********************************************************************/
void GetChipLayout( HWND hWindow, LPRECT lpRect, LPINT lpRows, LPINT lpCols, LPINT lpRowIncr, LPINT lpColIncr, LPINT lpStart )
/***********************************************************************/
{
int xChipSize, yChipSize, iWidth, iHeight, xPos, yPos, iColors;
LPPALETTE lpPalette;

if ( !(lpPalette = (LPPALETTE)GetWindowLong(hWindow, GWL_PALETTE)) )
	return;
if ( (iColors = lpPalette->iColors) <= 0 )
	iColors = 1;

GetDrawClientRect(hWindow, lpRect);
InflateRect(lpRect, -3, -3);
iWidth = RectWidth(lpRect);
iHeight = RectHeight(lpRect);

xPos = GetScrollPos(hWindow, SB_HORZ);
yPos = GetScrollPos(hWindow, SB_VERT);

GetChipSize( &xChipSize, &yChipSize );
*lpRows = iHeight / yChipSize;
*lpCols = iWidth / xChipSize;
if ( PaletteVertical() )
	{ // Fill left to right, row at a time
	*lpRowIncr = *lpCols;
	*lpColIncr = 1;
	*lpStart = (yPos * (*lpCols)) + xPos;
	// Too ensure proper scrolling, make the height an exact multiple
	lpRect->bottom = lpRect->top + (*lpRows * yChipSize) - 1;
	}
else
	{ // Fill top to bottom, row at a time
	*lpRowIncr = 1;
	*lpColIncr = *lpRows;
	*lpStart = (xPos * (*lpRows)) + yPos;
	// Too ensure proper scrolling, make the width an exact multiple
	lpRect->right = lpRect->left + (*lpCols * xChipSize) - 1;
	}
}

/***********************************************************************/
void GetChipSize( LPINT lpDx, LPINT lpDy )
/***********************************************************************/
{
GetScrollBarSize(lpDx, lpDy);
*lpDx += 1;
*lpDy += 1;
}

/***********************************************************************/
LOCAL void Palette_GetRowCol( HWND hWindow, int iSelect, LPINT lpRow, LPINT lpCol )
/***********************************************************************/
{
int iRows, iRowIncr, iCols, iColIncr, iStart;
RECT ClientRect;

GetChipLayout( hWindow, &ClientRect, &iRows, &iCols, &iRowIncr, &iColIncr,
	&iStart );

if (iRowIncr > 1)
	{
	*lpRow = iSelect / iRowIncr;
	*lpCol = iSelect - ((*lpRow) * iRowIncr);
	}
else
	{
	*lpCol = iSelect / iColIncr;
	*lpRow = iSelect - ((*lpCol) * iColIncr);
	}
}

