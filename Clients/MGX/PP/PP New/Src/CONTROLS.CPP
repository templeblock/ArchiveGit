// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "macros.h"
#include "visual.h"
#include "ctllib.h"
#include "ctxhelp.h"
#include "ppmodal.h"
#include "dlgsmap.h"

// Note to all GDI programmers:
// GetClientRect() returns a rectangle whose rightmost and bottom most pixels
// cannot be drawn into.  FrameRect() and FillRect() compensate for this
// problem automatically, but MoveTo() and Lineto() do not (and cannot)
// Use GetDrawClientRect if you want the actual rectangle to draw in
// and you plan to use MoveTo() LineTo instead of FrameRect() and FillRect()
// ....WHAT A PAIN!!!

#ifndef C_CODE
extern "C" {
#endif
extern BYTE Matrix[256];
#ifndef C_CODE
}
#endif
#define IS_STYLE(dwStyle, theStyle) (((dwStyle) & 0x0FL) == (theStyle))


// LOCAL prototypes for utility functions
LOCAL LPOBJECT GetImageObject( HWND hWnd );
LOCAL void ConstrainRatio( LPINT w, LPINT h, int width, int height );
LOCAL void MakeRatio( LPINT w, LPINT h, int width, int height );
LOCAL void DrawObjListControl( HDC hDC, HWND hWnd, LPOBJECT lpBase,
						LPRECT lpDestRect, int dx, int dy, BOOL bShadows,
						LPVOID lpCmsXform);

typedef void ( FAR *TOOLPROC )( HWND, HDC, int, int, UINT );

/***********************************************************************/
/***********************************************************************/
// LEVER CONTROL
/***********************************************************************/
/***********************************************************************/

LOCAL void Lever_OnPaint(HWND hWindow);
LOCAL void Lever_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Lever_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Lever_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Lever_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Lever_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Lever_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void Lever_Draw( HDC hDC, LPRECT lpRect, int iOldValue, int iValue, COLOR Color );

static RECT Lever_ActiveRect;

/***********************************************************************/
LONG WINPROC EXPORT Lever_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Control_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_DESTROY, Control_OnDestroy);

	HANDLE_MSG(hWindow, WM_PAINT, Lever_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Lever_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Lever_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Lever_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Lever_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Lever_OnKillFocus);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL void Lever_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
int iValue, iOldValue;
BOOL Bool, bHasFocus;
HRGN hRgn;
HDC hDC;
RECT ClientRect;

hDC = BeginPaint( hWindow, &ps );
GetClientRect( hWindow, &ClientRect );

// Draw the focus rectangle
bHasFocus = ( GetFocus() == hWindow );
FrameRect( hDC, &ClientRect,
	( HBRUSH )GetStockObject( bHasFocus ? BLACK_BRUSH : LTGRAY_BRUSH ) );
// Draw the box frame
InflateRect( &ClientRect, -1, -1 );
FrameRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(BLACK_BRUSH) );
InflateRect( &ClientRect, -1, -1 );
Lever_ActiveRect = ClientRect;
SelectClipRgn( hDC, hRgn = CreateRectRgnIndirect( &Lever_ActiveRect ) );
DeleteObject( hRgn );

// The int holds the 2 color percentages from -100 to 100
iOldValue = INT_MAX;
iValue = GetDlgItemInt( GetParent(hWindow),
	GetDlgCtrlID(hWindow), &Bool, YES );
Lever_Draw( hDC, &Lever_ActiveRect, iOldValue, iValue,
	GetWindowLong( hWindow, GWL_LEVERCOLOR ) );
GrayWindow( hDC, hWindow, WS_NOTENABLED );
EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Lever_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
int iValue, iOldValue;
BOOL Bool;
int xMax, yMax;
HRGN hRgn;

if ( Control_bTrack )
	return;
SetCapture( hWindow ); Control_bTrack = TRUE;
if ( GetFocus() != hWindow )
	SetFocus( hWindow );
Control_shDC = GetDC( hWindow );
SelectClipRgn( Control_shDC, hRgn = CreateRectRgnIndirect( &Lever_ActiveRect ) );
DeleteObject( hRgn );
xMax = RectWidth( &Lever_ActiveRect ) - 1;
yMax = RectHeight( &Lever_ActiveRect ) - 1;
if ( xMax > yMax )
	{
	iValue = bound( x-Lever_ActiveRect.left, 0, xMax );
	iValue = -100 + FMUL( 200, FGET( iValue, xMax ) );
	}
else	{
	iValue = bound( y-Lever_ActiveRect.top, 0, yMax );
	iValue = -100 + FMUL( 200, FGET( iValue, yMax ));
	iValue = -iValue;
	}
iOldValue = GetDlgItemInt( GetParent(hWindow),
	GetDlgCtrlID(hWindow), &Bool, YES );
if ( iValue == iOldValue )
	return;
Lever_Draw( Control_shDC, &Lever_ActiveRect, iOldValue, iValue,
	GetWindowLong( hWindow, GWL_LEVERCOLOR ) );
SetDlgItemInt( GetParent(hWindow), GetDlgCtrlID(hWindow),
	iValue, YES );
FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0, SendMessage );
}

/***********************************************************************/
LOCAL void Lever_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
int iValue, iOldValue;
BOOL Bool;
HRGN hRgn;
HDC hDC;

if ( Control_bTrack )
	return;
hDC = GetDC( hWindow );
SelectClipRgn( hDC, hRgn = CreateRectRgnIndirect( &Lever_ActiveRect ) );
DeleteObject( hRgn );
iValue = 0;
iOldValue = GetDlgItemInt( GetParent(hWindow),
	GetDlgCtrlID(hWindow), &Bool, YES );
Lever_Draw( hDC, &Lever_ActiveRect, iOldValue, iValue,
	GetWindowLong( hWindow, GWL_LEVERCOLOR ) );
ReleaseDC( hWindow, hDC );
SetDlgItemInt( GetParent(hWindow), GetDlgCtrlID(hWindow),
	iValue, YES );
FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0, SendMessage );
}

/***********************************************************************/
LOCAL void Lever_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
int iValue, iOldValue;
BOOL Bool;
int xMax, yMax;

if ( !Control_bTrack )
	return;
xMax = RectWidth( &Lever_ActiveRect ) - 1;
yMax = RectHeight( &Lever_ActiveRect ) - 1;
if ( xMax > yMax )
	{
	iValue = bound( x-Lever_ActiveRect.left, 0, xMax );
	iValue = -100 + FMUL( 200, FGET( iValue, xMax ) );
	}
else	
	{
	iValue = bound( y-Lever_ActiveRect.top, 0, yMax );
	iValue = -100 + FMUL( 200, FGET( iValue, yMax ));
	iValue = -iValue;
	}
iOldValue = GetDlgItemInt( GetParent(hWindow),
	GetDlgCtrlID(hWindow), &Bool, YES );
if ( iValue == iOldValue )
	return;
Lever_Draw( Control_shDC, &Lever_ActiveRect, iOldValue, iValue,
	GetWindowLong( hWindow, GWL_LEVERCOLOR ) );
SetDlgItemInt( GetParent(hWindow), GetDlgCtrlID(hWindow),
	iValue, YES );
FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0, SendMessage );
}

/***********************************************************************/
LOCAL void Lever_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
Lever_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
LOCAL void Lever_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
Lever_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
LOCAL void Lever_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
HDC hDC;
RECT ClientRect;

if ( !StyleOn( hWindow, WS_TABSTOP ) )
	return;
// Draw the focus rectangle
hDC = GetDC( hWindow );
GetClientRect( hWindow, &ClientRect );
FrameRect( hDC, &ClientRect,
		( HBRUSH )GetStockObject( fSetFocus ? BLACK_BRUSH : WHITE_BRUSH ) );
ReleaseDC( hWindow, hDC );
}

/***********************************************************************/
void Lever_Draw( HDC hDC, LPRECT lpRect, int iOldValue, int iValue,
                     COLOR Color )
/***********************************************************************/
{
int nx, ox, mx;
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

// The iValue also holds the values from -100 to 100
iValue = bound( iValue, -100, 100 );
nx = lpRect->left + FMUL( 100+iValue, FGET( RectWidth(lpRect)-1, 200 ) );

// The iOldValue holds the values from -100 to 100
bOKtoClip = ( iOldValue != INT_MAX );
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
	FillRect( hDC, &rect, ( HBRUSH )GetStockObject(WHITE_BRUSH) );

rPaint.left = mx;
rPaint.right = max(nx,mx);
if ( IntersectRect( &rect, &rClip, &rPaint ) )
	FillRect( hDC, &rect, ( HBRUSH )GetStockObject(WHITE_BRUSH) );

rPaint.left = max(nx,mx);
rPaint.right = lpRect->right;
if ( IntersectRect( &rect, &rClip, &rPaint ) )
	{
	FillRect( hDC, &rect, hBrush = CreateSolidBrush(rgb2) );
	DeleteObject( hBrush );
	}
}

/***********************************************************************/
/***********************************************************************/
// ICON CONTROL
/***********************************************************************/
/***********************************************************************/

LOCAL BOOL Icon_OnEraseBkgnd(HWND hWindow, HDC hDC);
LOCAL void Icon_OnPaint(HWND hWindow);
LOCAL void Icon_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Icon_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Icon_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Icon_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Icon_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Icon_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Icon_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void Icon_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Icon_OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Icon_OnTimer(HWND hwnd, UINT id);
LOCAL void Icon_OnEnable(HWND hWindow, BOOL fEnable);
LOCAL void Icon_OnDestroy(HWND hWindow);
LOCAL BOOL Icon_OnDragQuery(HWND hWindow, WORD wState, int x, int y);
LOCAL BOOL Icon_OnDragAccept(HWND hWindow, LPDRAG lpDrag);
LOCAL void Icon_Draw(HDC hDC, HWND hWnd, LPRECT lpRect, BOOL bSelected,
		BOOL Control_bTrackInRect );
LOCAL void DrawObjectControl( HDC hDC, HWND hWnd, LPOBJECT lpObject, LPRECT lpDestRect, int dx, int dy, BOOL bShadows, BOOL bDark  );
LOCAL void ObjectBeginDragAndDrop(HWND hWnd);
LOCAL void ObjectMoveDragAndDrop(HWND hWnd, int x, int y);
LOCAL void ObjectEndDragAndDrop(HWND hWnd);

/***********************************************************************/
LONG WINPROC EXPORT Icon_Control(	HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
	HANDLE_MSG(hWindow, BM_SETSTATE, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK, Control_OnSetCheck);
	HANDLE_MSG(hWindow, BM_GETCHECK, Control_OnGetCheck);

	#ifndef WINDOWS95_HACK
	HANDLE_MSG(hWindow, BM_SETSTATE16, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK16, Control_OnSetCheck);
	HANDLE_MSG(hWindow, BM_GETCHECK16, Control_OnGetCheck);
	#endif

	HANDLE_MSG(hWindow, WM_ERASEBKGND, Icon_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_PAINT, Icon_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Icon_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Icon_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Icon_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Icon_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Icon_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Icon_OnKillFocus);
	HANDLE_MSG(hWindow, WM_KEYDOWN, Icon_OnKeyDown);
	HANDLE_MSG(hWindow, WM_KEYUP, Icon_OnKeyUp);
	HANDLE_MSG(hWindow, WM_TIMER, Icon_OnTimer);
	HANDLE_MSG(hWindow, WM_ENABLE, Icon_OnEnable);
	HANDLE_MSG(hWindow, WM_DESTROY, Icon_OnDestroy);
	HANDLE_MSG(hWindow, WM_DRAGQUERY, Icon_OnDragQuery);
	HANDLE_MSG(hWindow, WM_DRAGACCEPT, Icon_OnDragAccept);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL Icon_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
RECT ClientRect;

GetClientRect( hWindow, &ClientRect );
EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_BTN );
return(TRUE);
}

/***********************************************************************/
LOCAL void Icon_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
RECT ClientRect;
HDC hDC;
BOOL Control_bTrack, bInRect, bSelected;

if ( SuperPaint( hWindow ) )
	return; // Printing in progress... (SuperBlt not reentrant)
hDC = BeginPaint( hWindow, &ps );
GetClientRect( hWindow, &ClientRect );
bSelected = GetWindowWord(hWindow, GWW_STATE);
Control_bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
Icon_Draw(hDC, hWindow, &ClientRect, bSelected, Control_bTrack && bInRect );
EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Icon_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if (GetWindowLong(hWindow, GWL_ID) == IDC_STATUSTEXT)
	{
	FORWARD_WM_LBUTTONDOWN(GetParent(hWindow), fDoubleClick, x, y, keyFlags, SendMessage );
	return;
	}
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
if (IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK))
	return;
SetCapture( hWindow ); SetWordBit(hWindow, GWW_STATUS, ST_TRACK, ON);
if ( GetFocus() != hWindow )
	SetFocus( hWindow );
SetWordBit(hWindow, GWW_STATUS, ST_INRECT, ON);
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
if ( StyleOn( hWindow, WS_TOOL ) )
	{
	Icon_OnLButtonUp(hWindow, 0, 0, 0);
	return;
	}
if ( StyleOn( hWindow, WS_ONTIMER ) )
	{
	// this message could cause the control to be disabled
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
	if ( !StyleOn( hWindow, WS_NOTENABLED ) )
		if (SetTimer(hWindow, 1, 100, NULL))
			SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, ON);
	}
}

/***********************************************************************/
LOCAL void Icon_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
BOOL Control_bTrack, bInRect, bSelected, bDown, bHasFocus;

if ( !IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
	return;

ReleaseCapture(); SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TIMERON) )
	{
	KillTimer( hWindow, 1 );
	SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, OFF);
	}
if (IsWordBitSet(hWindow, GWW_STATUS, ST_DRAGANDDROP) )
	{
	ObjectEndDragAndDrop(hWindow);
	return;
	}

// see if button was last drawn down
bHasFocus = ( GetFocus() == hWindow );
Control_bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
bSelected = GetWindowWord(hWindow, GWW_STATE);
bDown = ( bSelected || (Control_bTrack && bInRect && bHasFocus) );

if ( IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT)
	&& !StyleOn(hWindow, WS_ONTIMER) )
	{
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
	}
bSelected = GetWindowWord(hWindow, GWW_STATE);
//	if (bDown && !bSelected)
	{
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	}
}

/***********************************************************************/
LOCAL void Icon_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 1 );
}

/***********************************************************************/
LOCAL void Icon_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
RECT ClientRect;
POINT pt;
BOOL bInRect, bSelected;

if ( !IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
	return;
if (IsWordBitSet(hWindow, GWW_STATUS, ST_DRAGANDDROP) )
	{
	ObjectMoveDragAndDrop(hWindow, x, y);
	return;
	}
GetClientRect( hWindow, &ClientRect );
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
pt.x = x; pt.y = y;
if ( bInRect == !PtInRect( &ClientRect, pt ) )
	{
	bInRect = !bInRect;
	SetWordBit(hWindow, GWW_STATUS, ST_INRECT, bInRect);
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	if (!bInRect)
		{
		bSelected = GetWindowWord(hWindow, GWW_STATE);
		if (StyleOn(hWindow, IS_OBJECT) && bSelected)
			ObjectBeginDragAndDrop(hWindow);
		}
	}
else
	SetWordBit(hWindow, GWW_STATUS, ST_INRECT, bInRect);
}

/***********************************************************************/
LOCAL void Icon_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
Icon_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
LOCAL void Icon_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
Icon_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
LOCAL void Icon_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
if ( !StyleOn( hWindow, WS_TABSTOP ) )
	return;
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
}

/***********************************************************************/
LOCAL void Icon_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
if ( vk != VK_SPACE )
	return;
Icon_OnLButtonDown(hWindow, FALSE, 0, 0, 0);
}

/***********************************************************************/
LOCAL void Icon_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
if ( vk != VK_SPACE )
	return;
Icon_OnLButtonUp(hWindow, 0, 0, 0);
}

/***********************************************************************/
LOCAL void Icon_OnTimer(HWND hWindow, UINT id)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
}

/***********************************************************************/
LOCAL void Icon_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
if (!fEnable) // disabled?
	{
	if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TIMERON) )
		{
		KillTimer( hWindow, 1 );
		SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, OFF);
		}
	if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
		{
		ReleaseCapture();
		SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
		}
	}
Control_OnEnable(hWindow, fEnable);
}

/***********************************************************************/
LOCAL void Icon_OnDestroy(HWND hWindow)
/***********************************************************************/
{
if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TIMERON) )
	{
	KillTimer( hWindow, 1 );
	SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, OFF);
	}
if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
	{
	ReleaseCapture();
	SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
	}
}

/***********************************************************************/
LOCAL BOOL Icon_OnDragQuery(HWND hWindow, WORD wState, int x, int y)
/**********************************************************************/
{
BOOL bDrag;

bDrag = IsWordBitSet(hWindow, GWW_STATUS, ST_DRAGANDDROP);
return((wState == DAD_ID && !bDrag));
}

/***********************************************************************/
LOCAL BOOL Icon_OnDragAccept(HWND hWindow, LPDRAG lpDrag)
/***********************************************************************/
{
return(TRUE);
}

/***********************************************************************/
LOCAL void Icon_Draw(HDC hDC, HWND hWnd, LPRECT lpRect, BOOL bSelected,
		BOOL Control_bTrackInRect )
/***********************************************************************/
{
int         x, y, i;
HICON       hResource = NULL;
HBITMAP     hBitmap;
BOOL        bHasFocus, bDown, bEnabled, bShadows, bHasText, bBorderOnly;
ITEMID      id;
DWORD       dwStyle;
STRING      szString;
HGDIOBJ     hFont;
TEXTMETRIC  tm;
int 		offset;

if (Control.fNewLook)
	offset = 1;
else
	offset = 2;

dwStyle = GetWindowLong( hWnd, GWL_STYLE );
// The item id is the icon id, unless its an active tool
bHasFocus = ( GetFocus() == hWnd );
bEnabled  = !( dwStyle & WS_NOTENABLED );
bShadows  = !( dwStyle & WS_NOSHADOWS );
bHasText  =  ( dwStyle & WS_TEXT );
bDown = ( bSelected || (Control_bTrackInRect && bHasFocus) );
bBorderOnly = (dwStyle & WS_BORDERONLY);

// Draw the box interior
if ( !bShadows )
	{
	if ( bEnabled )
		EraseControlBackground( hDC, hWnd, lpRect, CTLCOLOR_BTN );
	}
else
if ( !bDown || !bEnabled || bBorderOnly || Control.fNewLook)
	FillRect( hDC, lpRect, DrawTools.GetBtnFaceBrush() );
else	HilightRect( hDC, lpRect, DrawTools.GetBtnGrayBrush() );

// Compute the client rect center
if ( !bHasText )
	x = ( lpRect->right + lpRect->left + 1 ) / 2;
else	x =   lpRect->right - 16; // All the way to the right
y = ( lpRect->bottom + lpRect->top + 1 ) / 2;
if ( bDown && bEnabled && !bBorderOnly)
	{ x += offset; y += offset; }

if ( dwStyle & WS_BITMAP )
	{ // Draw the Bitmap
	id = GetDlgCtrlID( hWnd );
	if ( hBitmap =LoadBitmap(PictPubApp.GetResourceHandle(),MAKEINTRESOURCE(id)))
		{
		DrawBitmap( hDC, x, y, hBitmap );
		DeleteObject( hBitmap );
		}
	}
else
if ( dwStyle & WS_IMAGE )
	{ // Draw the Image
	i = ( bDown && bEnabled ? offset : 0 );
	DrawImageControl( hDC, hWnd, NULL, NULL, i, i, bShadows );
	}
else
if ( dwStyle & IS_OBJECT )
	{ // Draw the Object
	i = ( bDown && bEnabled ? offset : 0 );
	DrawObjectControl( hDC, hWnd, NULL, NULL, i, i, bShadows, !(!bDown || !bEnabled || bBorderOnly));
	}
else
	{ // Draw the icon
	if ( id = GetDlgCtrlID( hWnd ) )
		hResource = LoadIcon( (id > LAST_ICON ? 0 : hInstIconLib ),
			MAKEINTRESOURCE(id) );
	if ( !hResource && (id = GetWindowWord( hWnd, GWW_ICONID )) )
		hResource = LoadIcon( hInstIconLib, MAKEINTRESOURCE(id) );
	if ( hResource )
		DrawIcon( hDC, x-16, y-16, hResource);
	else
		{
		id = GetDlgCtrlID( hWnd );
		if ( hBitmap = MgxLoadBitmap(hInstIconLib,MAKEINTRESOURCE(id)))
			{
			MgxDrawBitmap( hDC, x, y, hBitmap, bEnabled );
			DeleteObject( hBitmap );
			}
		}
	}

if ( bHasText )
	{ // Draw the text left justified
	if ( !(hFont = GetStockObject( Window.iLabelFont )) )
		hFont = (HFONT)GetWindowLong( hWnd, GWL_FONT );
	SelectObject( hDC, hFont );
	GetTextMetrics( hDC, &tm );
	y = ( RectHeight(lpRect) - (tm.tmHeight + tm.tmInternalLeading) ) / 2;
	GetWindowText( hWnd, szString, sizeof(szString) );
	if ( bDown ) 
		i = offset; 
	else 
		i = 0;
	ColorText( hDC, 6+i, y+i, szString, lstrlen(szString),
	GetSysColor( (bEnabled || hResource) ?
		COLOR_WINDOWTEXT : COLOR_BTNSHADOW ));
	}

// Draw the box frame
if ( bBorderOnly )
	{
	DrawBoxFrame(hDC, lpRect);
	if (bDown)
		{							 
		InflateRect(lpRect, -1, -1);
		InvertRect(hDC, lpRect);
		}
	}
else
if ( bShadows )
	{
	DrawSculptedBox( hDC, lpRect, bHasFocus && (dwStyle & WS_TABSTOP),
		bDown && bEnabled, YES );
	}
if ( hResource && !bEnabled && !(dwStyle & WS_NOSHADOWS) )
	GrayWindow( hDC, hWnd, WS_NOTENABLED );
}

/***********************************************************************/
LOCAL void DrawObjectControl( HDC hDC, HWND hWnd, LPOBJECT lpObject, LPRECT lpDestRect, int dx, int dy, BOOL bShadows, BOOL bDark )
/***********************************************************************/
{
int iImageWidth, iImageHeight, yAmount, iOldMode;
RECT SourceRect, DestRect;
BOOL bMustClose;
STRING szText;
TEXTMETRIC metrics;
COLORREF lOldColor;
LPFRAME lpDataFrame;

bMustClose = NO;

if ( lpDestRect && !IsRectEmpty(lpDestRect) )
	DestRect = *lpDestRect;
else
	{
	GetClientRect( hWnd, &DestRect );
	}

if ( !lpObject )
	lpObject = GetImageObject( hWnd );
if ( !lpObject)
	{ // use text if style is on
	if (!StyleOn(hWnd, WS_TEXT))
		return;
	if (!GetWindowText(hWnd,szText,sizeof(szText)))
		return;
	GetTextMetrics(hDC, &metrics);
	iOldMode = SetBkMode( hDC, TRANSPARENT );
	lOldColor = SetTextColor(hDC, RGB(255,0,0));
	yAmount = (RectHeight(&DestRect)-(2*metrics.tmHeight)) / 2;
	DestRect.top += yAmount;
	DrawText( hDC, szText, -1, &DestRect,
		DT_NOPREFIX | DT_WORDBREAK | DT_CENTER);
	SetBkMode( hDC, iOldMode );
	SetTextColor( hDC, lOldColor ); 
	return;
	}

// get data frame of object
if ( !(lpDataFrame = ObjGetEditFrame(lpObject)) )
	return;
iImageWidth  = FrameXSize(lpDataFrame);
iImageHeight = FrameYSize(lpDataFrame);

SourceRect.left = SourceRect.top = 0;
SourceRect.right  = iImageWidth - 1;
SourceRect.bottom = iImageHeight - 1;

WindowsToAstralRect(&DestRect);
if ( !StyleOn( hWnd, IS_STRETCHABLE ) )
	ComputeImageControlRect( hWnd, &DestRect, iImageWidth, iImageHeight, bShadows );
OffsetRect( &DestRect, dx, dy );

// Draw the Object
DrawObject( hDC, lpObject, &SourceRect, &DestRect, bDark ? DRAWOBJ_BUTTON_DOWN:DRAWOBJ_LIGHT_GRAY);
if ( lpDestRect )
	*lpDestRect = DestRect;
}

static LPDRAG lpMyDrag;
/***********************************************************************/
LOCAL void ObjectBeginDragAndDrop(HWND hWnd)
/***********************************************************************/
{
LPIMAGE lpImage;

if (!(lpImage = GetActiveImage()))
	return;
if (ImgCountSelObjects(lpImage, NULL) != 1)
	return;
lpMyDrag = DragAndDropBegin(hWnd, DAD_ID, NULL, NULL, NULL, NULL);
SetWordBit(hWnd, GWW_STATUS, ST_DRAGANDDROP, YES);
}

/***********************************************************************/
LOCAL void ObjectMoveDragAndDrop(HWND hWnd, int x, int y)
/***********************************************************************/
{
if (lpMyDrag)
	DragAndDropMove(lpMyDrag, x, y);
}

/***********************************************************************/
LOCAL void ObjectEndDragAndDrop(HWND hWnd)
/***********************************************************************/
{
HWND hDragWnd;
int id;

if (lpMyDrag)
	{
	SetWordBit(hWnd, GWW_STATUS, ST_DRAGANDDROP, NO);
	hDragWnd = lpMyDrag->hDragWnd;
	if (DragAndDropEnd(lpMyDrag))
		{
		id = GetDlgCtrlID(hDragWnd);
		SEND_WM_COMMAND( GetParent(hWnd), GetDlgCtrlID(hWnd), hWnd, id );
		}
	lpMyDrag = NULL;
	}
}


/***********************************************************************/
/***********************************************************************/
// LABELED FRAME CONTROL
//	Same as icon control except that it may have a frame AND a bitmap or text
//		at the same time.
// 	If there is a bitmap then it takes its name from the text of the window
// 	send a WM_IMAGESPACE to get the size of the space for the frame
/***********************************************************************/
/***********************************************************************/
LOCAL void LabeledFrame_OnPaint(HWND hWindow);
LOCAL void LabeledFrame_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void LabeledFrame_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void LabeledFrame_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void LabeledFrame_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void LabeledFrame_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void LabeledFrame_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void LabeledFrame_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void LabeledFrame_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void LabeledFrame_OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void LabeledFrame_OnTimer(HWND hwnd, UINT id);
LOCAL void LabeledFrame_OnEnable(HWND hWindow, BOOL fEnable);
LOCAL void LabeledFrame_OnDestroy(HWND hWindow);
LOCAL LONG LabeledFrame_OnImageSpace(HWND hWindow);
LOCAL LONG LabeledFrame_Draw(HDC hDC, HWND hWnd, LPRECT lpRect, BOOL bSelected, BOOL Control_bTrackInRect, BOOL bGetSize);

/***********************************************************************/
LONG WINPROC EXPORT LabeledFrame_Control(	HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
	HANDLE_MSG(hWindow, BM_SETSTATE, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK, Control_OnSetCheck);
	HANDLE_MSG(hWindow, BM_GETCHECK, Control_OnGetCheck);

	#ifndef WINDOWS95_HACK
	HANDLE_MSG(hWindow, BM_SETSTATE16, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK16, Control_OnSetCheck);
	HANDLE_MSG(hWindow, BM_GETCHECK16, Control_OnGetCheck);
	#endif

	HANDLE_MSG(hWindow, WM_PAINT, LabeledFrame_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, LabeledFrame_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, LabeledFrame_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, LabeledFrame_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, LabeledFrame_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SETFOCUS, LabeledFrame_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, LabeledFrame_OnKillFocus);
	HANDLE_MSG(hWindow, WM_KEYDOWN, LabeledFrame_OnKeyDown);
	HANDLE_MSG(hWindow, WM_KEYUP, LabeledFrame_OnKeyUp);
	HANDLE_MSG(hWindow, WM_TIMER, LabeledFrame_OnTimer);
	HANDLE_MSG(hWindow, WM_ENABLE, LabeledFrame_OnEnable);
	HANDLE_MSG(hWindow, WM_DESTROY, LabeledFrame_OnDestroy);
	HANDLE_MSG(hWindow, WM_IMAGESPACE, LabeledFrame_OnImageSpace);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL LabeledFrame_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
RECT ClientRect;

GetClientRect( hWindow, &ClientRect );
EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_BTN );
return(TRUE);
}

/***********************************************************************/
LOCAL void LabeledFrame_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
RECT ClientRect;
HDC hDC;
BOOL Control_bTrack, bInRect, bSelected;

if ( SuperPaint( hWindow ) )
	return; // Printing in progress... (SuperBlt not reentrant)
hDC = BeginPaint( hWindow, &ps );
GetClientRect( hWindow, &ClientRect );
bSelected = GetWindowWord(hWindow, GWW_STATE);
Control_bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
LabeledFrame_Draw(hDC, hWindow, &ClientRect, bSelected, Control_bTrack && bInRect, NO );
EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void LabeledFrame_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
if (IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK))
	return;
SetCapture( hWindow ); SetWordBit(hWindow, GWW_STATUS, ST_TRACK, ON);
if ( GetFocus() != hWindow )
	SetFocus( hWindow );
SetWordBit(hWindow, GWW_STATUS, ST_INRECT, ON);
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
if ( StyleOn( hWindow, WS_TOOL ) )
	{
	LabeledFrame_OnLButtonUp(hWindow, 0, 0, 0);
	return;
	}
if ( StyleOn( hWindow, WS_ONTIMER ) )
	{
	// this message could cause the control to be disabled
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
	if ( !StyleOn( hWindow, WS_NOTENABLED ) )
		if (SetTimer(hWindow, 1, 100, NULL))
			SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, ON);
	}
}

/***********************************************************************/
LOCAL void LabeledFrame_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
BOOL Control_bTrack, bInRect, bSelected, bDown, bHasFocus;

if ( !IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
	return;

ReleaseCapture(); SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TIMERON) )
	{
	KillTimer( hWindow, 1 );
	SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, OFF);
	}

// see if button was last drawn down
bHasFocus = ( GetFocus() == hWindow );
Control_bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
bSelected = GetWindowWord(hWindow, GWW_STATE);
bDown = ( bSelected || (Control_bTrack && bInRect && bHasFocus) );

if ( IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT)
	&& !StyleOn(hWindow, WS_ONTIMER) )
	{
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
	}
bSelected = GetWindowWord(hWindow, GWW_STATE);
//	if (bDown && !bSelected)
	{
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	}
}

/***********************************************************************/
LOCAL void LabeledFrame_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 1 );
}

/***********************************************************************/
LOCAL void LabeledFrame_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
RECT ClientRect;
POINT pt;
BOOL bInRect;

if ( !IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
	return;
GetClientRect( hWindow, &ClientRect );
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
pt.x = x; pt.y = y;
if ( bInRect == !PtInRect( &ClientRect, pt ) )
	{
	bInRect = !bInRect;
	SetWordBit(hWindow, GWW_STATUS, ST_INRECT, bInRect);
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	}
else
	SetWordBit(hWindow, GWW_STATUS, ST_INRECT, bInRect);
}

/***********************************************************************/
LOCAL void LabeledFrame_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
LabeledFrame_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
LOCAL void LabeledFrame_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
LabeledFrame_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
LOCAL void LabeledFrame_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
if ( !StyleOn( hWindow, WS_TABSTOP ) )
	return;
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
}

/***********************************************************************/
LOCAL void LabeledFrame_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
if ( vk != VK_SPACE )
	return;
LabeledFrame_OnLButtonDown(hWindow, FALSE, 0, 0, 0);
}

/***********************************************************************/
LOCAL void LabeledFrame_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
if ( vk != VK_SPACE )
	return;
LabeledFrame_OnLButtonUp(hWindow, 0, 0, 0);
}

/***********************************************************************/
LOCAL void LabeledFrame_OnTimer(HWND hWindow, UINT id)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
}

/***********************************************************************/
LOCAL void LabeledFrame_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
if (!fEnable) // disabled?
	{
	if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TIMERON) )
		{
		KillTimer( hWindow, 1 );
		SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, OFF);
		}
	if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
		{
		ReleaseCapture();
		SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
		}
	}
Control_OnEnable(hWindow, fEnable);
}

/***********************************************************************/
LOCAL void LabeledFrame_OnDestroy(HWND hWindow)
/***********************************************************************/
{
if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TIMERON) )
	{
	KillTimer( hWindow, 1 );
	SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, OFF);
	}
if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
	{
	ReleaseCapture();
	SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
	}
}

/***********************************************************************/
LOCAL LONG LabeledFrame_OnImageSpace(HWND hWindow)
/***********************************************************************/
{
HDC hDC;
BOOL Control_bTrack, bInRect, bSelected;
RECT ClientRect;
LONG ret;

GetClientRect( hWindow, &ClientRect );
bSelected = GetWindowWord(hWindow, GWW_STATE);
Control_bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
hDC = GetDC(hWindow);
ret = LabeledFrame_Draw(hDC, hWindow, &ClientRect, bSelected, 
			Control_bTrack && bInRect, YES);
ReleaseDC(hWindow, hDC);
return(ret);
}

//***********************************************************************/
//	if bGetSize then returns width & height packed into a long
//***********************************************************************/
LOCAL LONG LabeledFrame_Draw(HDC hDC, HWND hWnd, LPRECT lpRect, BOOL bSelected, BOOL Control_bTrackInRect, BOOL bGetSize)
//***********************************************************************/
{
	int         x, y, dx, dy, i, BitmapHeight, TextHeight;
	HICON       hIcoResource;
	HBITMAP     hResource;
	BOOL        bHasFocus, bDown, bEnabled, bShadows, bHasText, bHasBitmap;
	ITEMID      id;
	DWORD       dwStyle;
	STRING      szString;
	HGDIOBJ     hFont;
	TEXTMETRIC  tm;
	BITMAP      Data;
	RECT        Rect, rDraw;
	LPFRAME     lpFrame;

	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	// The item id is the icon id, unless its an active tool
	bHasFocus = ( GetFocus() == hWnd );
	bEnabled  = !( dwStyle & WS_NOTENABLED );
	bShadows  = !( dwStyle & WS_NOSHADOWS );
	bHasText  =  ( dwStyle & WS_TEXT );
	bHasBitmap = ( dwStyle & WS_BITMAP );
	bDown = ( bSelected || (Control_bTrackInRect && bHasFocus) );
	
	// Draw the box interior
	if (!bGetSize)
	{
		if ( !bShadows )
			{
			if ( bEnabled )
				EraseControlBackground( hDC, hWnd, lpRect, CTLCOLOR_BTN );
			}
		else
		if ( !bDown || !bEnabled )
			FillRect( hDC, lpRect, DrawTools.GetBtnFaceBrush() );
		else	HilightRect( hDC, lpRect, DrawTools.GetBtnGrayBrush() );
	}
	
	// Get bitmap if any 
	hResource = NULL;
	BitmapHeight = 0;
	if (bHasBitmap)
	{
		GetWindowText( hWnd, szString, sizeof(szString) );
	   	hResource =LoadBitmap(PictPubApp.GetResourceHandle(),szString);
		GetObject( hResource, sizeof(BITMAP), (LPTSTR)&Data );
		BitmapHeight = Data.bmHeight;
	}
	// Get Text if any
	TextHeight = 0;
	if ( bHasText )
	{ // Draw the text left justified
		if ( !(hFont = GetStockObject( Window.iLabelFont )) )
			hFont = (HFONT)GetWindowLong( hWnd, GWL_FONT );
		SelectObject( hDC, hFont );
		GetTextMetrics( hDC, &tm );
		TextHeight = tm.tmHeight + tm.tmInternalLeading;
		GetWindowText( hWnd, szString, sizeof(szString));
	}
	
	// Compute the client rect center
	x = ( lpRect->right + lpRect->left + 1 ) / 2;
	y = ( lpRect->bottom - TextHeight - BitmapHeight
				 + lpRect->top + 1 ) / 2;
	if ( bDown && bEnabled)
		{ x += 2; y += 2; }
	
	if ( dwStyle & WS_IMAGE )
	{ // Draw the Image
		Rect = *lpRect;
		Rect.bottom -= TextHeight+BitmapHeight;
		if (bGetSize)
			return(MAKELONG(Rect.right-Rect.left+1, Rect.bottom-Rect.top+1));
		i = ( bDown && bEnabled ? 2 : 0 );
		lpFrame = (LPFRAME)GetWindowLong( hWnd, GWL_IMAGE ); // Normally a frame pointer
		if (lpFrame)
		{
			rDraw.top = rDraw.left = 0;
			rDraw.bottom = FrameYSize(lpFrame)-1;
			rDraw.right = FrameXSize(lpFrame)-1;
			dx = RectWidth(&Rect) - RectWidth(&rDraw);
			dy = RectHeight(&Rect) - RectHeight(&rDraw);
			if ( dx < 0   || dy < 0)	
			{
				// scale and draw
				DrawImageControl( hDC, hWnd, NULL, &Rect, i, i, bShadows );
			}
			else
			{	
				// draw directly
				Rect.bottom = Rect.top+rDraw.bottom;	
				Rect.right = Rect.left+rDraw.right;	
				OffsetRect(&Rect, dx/2, dy/2);
				DrawFrameEx( lpFrame, hDC, &rDraw, &Rect, NULL );
			}
		}	// end if frame
	}	// end if IMAGE
	else
		{ // Draw the icon
		hIcoResource = NULL;
		if ( id = GetDlgCtrlID( hWnd ) )
			hIcoResource = LoadIcon( (id > LAST_ICON ? 0 : hInstIconLib ),
				MAKEINTRESOURCE(id) );
		if ( !hIcoResource && hInstIconLib && (id = GetWindowWord( hWnd, GWW_ICONID )) )
			hIcoResource = LoadIcon( hInstIconLib, MAKEINTRESOURCE(id) );
		if ( hIcoResource )
			DrawIcon( hDC, x-16, y-16, hIcoResource);
		}
	// DRAW BITMAP	
	if ( bHasBitmap && hResource )
		{ // Draw the Bitmap
			y = lpRect->bottom - TextHeight - BitmapHeight/2;
			DrawBitmap( hDC, x, y, hResource );
			DeleteObject( hResource );
			hResource = NULL;
		}
	// DRAW TEXT
	if ( bHasText )
		{ // Draw the text left justified
		SelectObject( hDC, hFont );
		y = lpRect->bottom - TextHeight;
		if ( bDown ) i = 2; else i = 0;
		ColorText( hDC, 6+i, y+i, szString, lstrlen(szString),
			GetSysColor( (bEnabled || hResource) ?
				COLOR_WINDOWTEXT : COLOR_BTNSHADOW ));
		}
	
	// Draw the box frame
	if ( bShadows )
		{
		DrawSculptedBox( hDC, lpRect, bHasFocus && (dwStyle & WS_TABSTOP),
			bDown && bEnabled, YES );
		}
	if ( hResource && !bEnabled && !(dwStyle & WS_NOSHADOWS) )
	GrayWindow( hDC, hWnd, WS_NOTENABLED );
	return(0L);
}

/***********************************************************************/
/***********************************************************************/
// VISUAL FRAME CONTROL
//	Same as icon control except that it may have a frame AND a bitmap or text
//		at the same time.
// 	If there is a bitmap then it takes its name from the text of the window
// 	send a WM_IMAGESPACE to get the size of the space for the frame
//	For visual calibration dialogs (visual.c)
// 	Should have the GWL_LPVISUAL filled.
// 	send a WM_IMAGESPACE to get the size of the space for the frame
/***********************************************************************/
/***********************************************************************/

LOCAL void VisualFrame_OnPaint(HWND hWindow);
LOCAL void VisualFrame_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void VisualFrame_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void VisualFrame_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void VisualFrame_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void VisualFrame_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void VisualFrame_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void VisualFrame_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void VisualFrame_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void VisualFrame_OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void VisualFrame_OnTimer(HWND hwnd, UINT id);
LOCAL void VisualFrame_OnEnable(HWND hWindow, BOOL fEnable);
LOCAL void VisualFrame_OnDestroy(HWND hWindow);
LOCAL LONG VisualFrame_OnImageSpace(HWND hWindow);
LOCAL LONG VisualFrame_Draw(HDC hDC, HWND hWnd, LPRECT lpRect, BOOL bSelected, BOOL Control_bTrackInRect, BOOL bGetSize);

/***********************************************************************/
LONG WINPROC EXPORT VisualFrame_Control(	HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
	HANDLE_MSG(hWindow, BM_SETSTATE, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK, Control_OnSetCheck);
	HANDLE_MSG(hWindow, BM_GETCHECK, Control_OnGetCheck);
	#ifndef WINDOWS95_HACK
	HANDLE_MSG(hWindow, BM_SETSTATE16, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK16, Control_OnSetCheck);
	HANDLE_MSG(hWindow, BM_GETCHECK16, Control_OnGetCheck);
	#endif

	HANDLE_MSG(hWindow, WM_PAINT, VisualFrame_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, VisualFrame_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, VisualFrame_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, VisualFrame_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, VisualFrame_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SETFOCUS, VisualFrame_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, VisualFrame_OnKillFocus);
	HANDLE_MSG(hWindow, WM_KEYDOWN, VisualFrame_OnKeyDown);
	HANDLE_MSG(hWindow, WM_KEYUP, VisualFrame_OnKeyUp);
	HANDLE_MSG(hWindow, WM_TIMER, VisualFrame_OnTimer);
	HANDLE_MSG(hWindow, WM_ENABLE, VisualFrame_OnEnable);
	HANDLE_MSG(hWindow, WM_DESTROY, VisualFrame_OnDestroy);
	HANDLE_MSG(hWindow, WM_IMAGESPACE, VisualFrame_OnImageSpace);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL VisualFrame_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
RECT ClientRect;

GetClientRect( hWindow, &ClientRect );
EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_BTN );
return(TRUE);
}

/***********************************************************************/
LOCAL void VisualFrame_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
RECT ClientRect;
HDC hDC;
BOOL Control_bTrack, bInRect, bSelected;

if ( SuperPaint( hWindow ) )
	return; // Printing in progress... (SuperBlt not reentrant)
hDC = BeginPaint( hWindow, &ps );
GetClientRect( hWindow, &ClientRect );
bSelected = GetWindowWord(hWindow, GWW_STATE);
Control_bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
VisualFrame_Draw(hDC, hWindow, &ClientRect, bSelected, Control_bTrack && bInRect, NO );
EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void VisualFrame_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
if (IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK))
	return;
SetCapture( hWindow ); SetWordBit(hWindow, GWW_STATUS, ST_TRACK, ON);
if ( GetFocus() != hWindow )
	SetFocus( hWindow );
SetWordBit(hWindow, GWW_STATUS, ST_INRECT, ON);
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
if ( StyleOn( hWindow, WS_TOOL ) )
	{
	VisualFrame_OnLButtonUp(hWindow, 0, 0, 0);
	return;
	}
if ( StyleOn( hWindow, WS_ONTIMER ) )
	{
	// this message could cause the control to be disabled
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
	if ( !StyleOn( hWindow, WS_NOTENABLED ) )
		if (SetTimer(hWindow, 1, 100, NULL))
			SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, ON);
	}
}

/***********************************************************************/
LOCAL void VisualFrame_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
BOOL Control_bTrack, bInRect, bSelected, bDown, bHasFocus;

if ( !IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
	return;

ReleaseCapture(); SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TIMERON) )
	{
	KillTimer( hWindow, 1 );
	SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, OFF);
	}

// see if button was last drawn down
bHasFocus = ( GetFocus() == hWindow );
Control_bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
bSelected = GetWindowWord(hWindow, GWW_STATE);
bDown = ( bSelected || (Control_bTrack && bInRect && bHasFocus) );

if ( IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT)
	&& !StyleOn(hWindow, WS_ONTIMER) )
	{
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
	}
bSelected = GetWindowWord(hWindow, GWW_STATE);
//	if (bDown && !bSelected)
	{
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	}
}

/***********************************************************************/
LOCAL void VisualFrame_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 1 );
}

/***********************************************************************/
LOCAL void VisualFrame_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
RECT ClientRect;
POINT pt;
BOOL bInRect;

if ( !IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
	return;
GetClientRect( hWindow, &ClientRect );
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
pt.x = x; pt.y = y;
if ( bInRect == !PtInRect( &ClientRect, pt ) )
	{
	bInRect = !bInRect;
	SetWordBit(hWindow, GWW_STATUS, ST_INRECT, bInRect);
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	}
else
	SetWordBit(hWindow, GWW_STATUS, ST_INRECT, bInRect);
}

/***********************************************************************/
LOCAL void VisualFrame_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
VisualFrame_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
LOCAL void VisualFrame_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
VisualFrame_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
LOCAL void VisualFrame_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
if ( !StyleOn( hWindow, WS_TABSTOP ) )
	return;
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
}

/***********************************************************************/
LOCAL void VisualFrame_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
if ( vk != VK_SPACE )
	return;
VisualFrame_OnLButtonDown(hWindow, FALSE, 0, 0, 0);
}

/***********************************************************************/
LOCAL void VisualFrame_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
if ( vk != VK_SPACE )
	return;
VisualFrame_OnLButtonUp(hWindow, 0, 0, 0);
}

/***********************************************************************/
LOCAL void VisualFrame_OnTimer(HWND hWindow, UINT id)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
}

/***********************************************************************/
LOCAL void VisualFrame_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
if (!fEnable) // disabled?
	{
	if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TIMERON) )
		{
		KillTimer( hWindow, 1 );
		SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, OFF);
		}
	if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
		{
		ReleaseCapture();
		SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
		}
	}
Control_OnEnable(hWindow, fEnable);
}

/***********************************************************************/
LOCAL void VisualFrame_OnDestroy(HWND hWindow)
/***********************************************************************/
{
if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TIMERON) )
	{
	KillTimer( hWindow, 1 );
	SetWordBit(hWindow, GWW_STATUS, ST_TIMERON, OFF);
	}
if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
	{
	ReleaseCapture();
	SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
	}
}

/***********************************************************************/
LOCAL LONG VisualFrame_OnImageSpace(HWND hWindow)
/***********************************************************************/
{
HDC hDC;
BOOL Control_bTrack, bInRect, bSelected;
LONG ret;
RECT ClientRect;

GetClientRect( hWindow, &ClientRect );
bSelected = GetWindowWord(hWindow, GWW_STATE);
Control_bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
hDC = GetDC(hWindow);
ret = VisualFrame_Draw(hDC, hWindow, &ClientRect, bSelected, 
			Control_bTrack && bInRect, YES);
ReleaseDC(hWindow, hDC);
return(ret);
}

//***********************************************************************/
//	if bGetSize then returns width & height packed into a long.
// 	bSelected, Control_bTrackInRect, bGetSize are NOT CURRENTLY USED.
//***********************************************************************/
LOCAL LONG VisualFrame_Draw(HDC hDC, HWND hWnd, LPRECT lpRect, BOOL bSelected, BOOL Control_bTrackInRect, BOOL bGetSize)
//***********************************************************************/
{
	int id, xpos, ypos, dx, dy, width, height;
	BOOL bHasFocus, bDown, bEnabled, bShadows;
	DWORD dwStyle;
	RECT Rect;
  	CVisual* lpVisual;

	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	// The item id is the icon id, unless its an active tool
	// THESE ARE NOT CURRENTLY USED
	bHasFocus = ( GetFocus() == hWnd );
	bEnabled  = !( dwStyle & WS_NOTENABLED );
	bShadows  = !( dwStyle & WS_NOSHADOWS );
	bDown = ( bSelected || (Control_bTrackInRect && bHasFocus) );
	
	// Draw the box interior
	// Draw the Image
	Rect = *lpRect;
	if (bGetSize)
		return(MAKELONG(Rect.right-Rect.left+1, Rect.bottom-Rect.top+1));

	lpVisual = (CVisual*)GetWindowLong( hWnd, GWL_LPVISUAL);
	if (lpVisual)
	{
		// center image
		if (lpVisual->m_lpBasisFrame)
		{
			width = FrameXSize(lpVisual->m_lpBasisFrame);
			width = Min(width, RectWidth(&Rect));
			height = FrameYSize(lpVisual->m_lpBasisFrame);
			height = Min(height, RectHeight(&Rect));
			dx = (RectWidth(&Rect) - width)/2;
			dy = (RectHeight(&Rect) - height)/2;
			SetRect(&Rect, dx, dy, dx+width, dy+height);
		}
		id = GetDlgCtrlID(hWnd);
		if (GetPosFromID(id,&xpos,&ypos));
			(lpVisual->DrawVisual)(hDC, &Rect, xpos, ypos);
	}
	return(0L);
}

/***********************************************************************/
/***********************************************************************/
// COLOR CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL void Color_OnPaint(HWND hWindow);
LOCAL void Color_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Color_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Color_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Color_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Color_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Color_OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);

static BOOL Color_bInRect;

/***********************************************************************/
LONG WINPROC EXPORT Color_Control(	HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
	HANDLE_MSG(hWindow, WM_DESTROY, Control_OnDestroy);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Control_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Control_OnKillFocus);
	HANDLE_MSG(hWindow, WM_PALETTECHANGED, Control_OnPaletteChanged);
	HANDLE_MSG(hWindow, BM_SETSTATE, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK, Control_OnSetCheck);
	HANDLE_MSG(hWindow, BM_GETCHECK, Control_OnGetCheck);
	#ifndef WINDOWS95_HACK
	HANDLE_MSG(hWindow, BM_SETSTATE16, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK16, Control_OnSetCheck);
	HANDLE_MSG(hWindow, BM_GETCHECK16, Control_OnGetCheck);
	#endif

	HANDLE_MSG(hWindow, WM_PAINT, Color_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Color_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Color_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Color_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Color_OnMouseMove);
	HANDLE_MSG(hWindow, WM_KEYDOWN, Color_OnKeyDown);
	HANDLE_MSG(hWindow, WM_KEYUP, Color_OnKeyUp);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL void Color_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
RECT        ClientRect;
HDC         hDC;
BOOL        bHasFocus, bSelected, bDown, bSuperBlt, bShadows;
RGBS        rgb;
COLOR       Color;
COLORINFO   ColorInfo;
DWORD       dwStyle;
TCHAR       tmp[MAX_STR_LEN];
WORD 		wChar;

if ( SuperPaint( hWindow ) )
	return; // Printing in progress... (SuperBlt not reentrant)
hDC = BeginPaint( hWindow, &ps );
GetClientRect( hWindow, &ClientRect );
bSelected = GetWindowWord(hWindow, GWW_STATE );

dwStyle = GetWindowLong( hWindow, GWL_STYLE );
bShadows = ((dwStyle & WS_NOSHADOWS) == 0);
bHasFocus = ( GetFocus() == hWindow );
bDown = ( bSelected || (Control_bTrack && Color_bInRect && bHasFocus) );

// Compute the color to paint
GetWindowText( hWindow, tmp, sizeof(tmp) );

if( IsDBCSLeadByte(( BYTE )*tmp ))
	wChar = *(( LPTSTR )tmp );
else	
	wChar = *tmp;

if( wChar )
	{
	AsciiRGB( MyCharNextN( tmp, 1 ), &rgb );
	// The first character indicates _T('h') for HSL or _T('r') for RGB
	if( wChar == _T('H') || wChar == _T('h') )
		HSLtoRGB( rgb.red, rgb.green, rgb.blue, &rgb );
	// If lower case, use a Windows brush to draw the patch,
	// otherwise use SuperBlt
	bSuperBlt = ( wChar < _T('a') || wChar > _T('z') );
	}
else	
	{
	bSuperBlt = YES;
	Color = GetWindowLong( hWindow, GWL_ACTIVE );
	CopyRGB( &Color, &rgb );
	ColorInfo.rgb = rgb;
	SetColorInfo( &ColorInfo, &ColorInfo, CS_RGB );
	}

// Draw the color patch
if (bShadows)
	InflateRect( &ClientRect, -1, -1 );
if ( !bDown )
	DrawColorPatch( hDC, &ClientRect, &rgb, bSuperBlt );
else 
	HilightRect( hDC, &ClientRect, DrawTools.GetBtnGrayBrush() );

if (bShadows)
	{
	InflateRect( &ClientRect, 1, 1 );

	// Draw the box frame
	if ( WS_NOTENABLED & dwStyle )
		FrameRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(BLACK_BRUSH) );
	else	
		DrawSculptedBox( hDC, &ClientRect, bHasFocus && (WS_TABSTOP & dwStyle), bDown, NO );
	}

EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Color_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( Control_bTrack )
	return;
SetCapture( hWindow ); Control_bTrack = TRUE;
if ( GetFocus() != hWindow )
	SetFocus( hWindow );
Color_bInRect = YES;
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
}

/***********************************************************************/
LOCAL void Color_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( !Control_bTrack )
	return;
ReleaseCapture(); Control_bTrack = FALSE;
InvalidateRect( hWindow, NULL, FALSE );
UpdateWindow( hWindow );
if ( Color_bInRect )
	SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0 );
}

/***********************************************************************/
LOCAL void Color_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 1 );
}

/***********************************************************************/
LOCAL void Color_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
RGBS rgb;
COLOR Color;
COLORINFO ColorInfo;
POINT pt;
RECT ClientRect;

if ( !Control_bTrack )
	{
	if ( StyleOn( hWindow, WS_NOTENABLED ) )
		return;
	Color = GetWindowLong( hWindow, GWL_ACTIVE );
	CopyRGB( &Color, &rgb );
	ColorInfo.rgb = rgb;
	SetColorInfo( &ColorInfo, &ColorInfo, CS_RGB );
    // color and palette picker controls are a special case
    int id = GetDlgCtrlID( hWindow );

    if( id == IDC_PICK_PATCH2 ||
        id == IDC_PICK_2PATCH2 )
    { 
        if( Hints.fStatusHintsOn )
            HintLine( id );
    }
    else
    	ColorStatus( &ColorInfo, 0L, NULL, YES );
	return;
	}
GetClientRect( hWindow, &ClientRect );
pt.x = x; pt.y = y;
if ( Color_bInRect == !PtInRect( &ClientRect, pt ) )
	{
	Color_bInRect = !Color_bInRect;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	}
}

/***********************************************************************/
LOCAL void Color_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
if ( StyleOn( hWindow, WS_NOTENABLED ) )
	return;
if ( vk == VK_SPACE )
	Color_OnLButtonDown(hWindow, FALSE, 0, 0, 0);
}

/***********************************************************************/
LOCAL void Color_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
if ( vk == VK_SPACE )
	Color_OnLButtonUp(hWindow, 0, 0, 0);
}

/***********************************************************************/
void DrawColorPatch( HDC hDC, LPRECT lpRect, LPRGB lpRGB, BOOL bSuperBlt )
/***********************************************************************/
{
HBRUSH hBrush;
long Color;

Color = RGB(lpRGB->red, lpRGB->green, lpRGB->blue);

if ( bSuperBlt )
	{
	lpRect->right--;
	lpRect->bottom--;
	if (!AstralIsRectEmpty(lpRect))
		SuperFill( hDC, lpBltScreen, lpRect, 0, 0, Color, NULL );
	lpRect->right++;
	lpRect->bottom++;
	}
else	{

	FillRect( hDC, lpRect, hBrush = CreateSolidBrush( Color ) );

	DeleteObject( hBrush );
	}
}

/***********************************************************************/
/***********************************************************************/
// COLOR 2 CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL BOOL Color2_OnEraseBkgnd(HWND hWindow, HDC hDC);
LOCAL void Color2_OnPaint(HWND hWindow);
LOCAL void Color2_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Color2_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Color2_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Color2_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Color2_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Color2_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Color2_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void Color2_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Color2_OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL COLOR Color2_OnGetColor(HWND hWindow, BOOL fAlternate);

extern BOOL ActiveColorIsActive;

/***********************************************************************/
LONG WINPROC EXPORT Color2_Control(	HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
	HANDLE_MSG(hWindow, WM_DESTROY, Control_OnDestroy);
	HANDLE_MSG(hWindow, WM_PALETTECHANGED, Control_OnPaletteChanged);
	HANDLE_MSG(hWindow, BM_SETSTATE, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK, Control_OnSetCheck);
	HANDLE_MSG(hWindow, BM_GETCHECK, Control_OnGetCheck);
	#ifndef WINDOWS95_HACK
	HANDLE_MSG(hWindow, BM_SETSTATE16, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK16, Control_OnSetCheck);
	HANDLE_MSG(hWindow, BM_GETCHECK16, Control_OnGetCheck);
	#endif

	HANDLE_MSG(hWindow, WM_PAINT, Color2_OnPaint);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Color2_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Color2_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Color2_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Color2_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Color2_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Color2_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Color2_OnKillFocus);
	HANDLE_MSG(hWindow, WM_GETCOLOR, Color2_OnGetColor);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL Color2_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
RECT ClientRect;

GetClientRect( hWindow, &ClientRect );
EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_BTN );
return(TRUE);
}

/***********************************************************************/
LOCAL void Color2_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
RECT ClientRect, SavedRect;
HDC hDC;
BOOL bHasFocus, bSelected;
BOOL bDrawingActive;
RGBS rgb;
COLOR Color;

if ( SuperPaint( hWindow ) )
	return; // Printing in progress... (SuperBlt not reentrant)
hDC = BeginPaint( hWindow, &ps );
GetClientRect( hWindow, &ClientRect );

bSelected = ActiveColorIsActive;
bHasFocus = ( GetFocus() == hWindow );

// Draw the box frame
FrameRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(BLACK_BRUSH) );
InflateRect( &ClientRect, -1, -1 );

// Draw the box interior
//if (message == WM_INITDIALOG)
//{
//	FillRect( hDC, &ClientRect, DrawTools.GetBtnFaceBrush() );
//}
InflateRect( &ClientRect, -1, -1 );

//	Take out the focus rect for now
//	if ( bHasFocus ) // Draw the focus rectangle
//		FrameRect( hDC, &ClientRect, GetStockObject(BLACK_BRUSH) );
//	else	FrameRect( hDC, &ClientRect, GetStockObject(WHITE_BRUSH) );
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
FrameRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(BLACK_BRUSH) );
InflateRect( &ClientRect, -1, -1 );

Color = SendMessage(hWindow, WM_GETCOLOR, bDrawingActive?0:1, 0L);

CopyRGB( &Color, &rgb );

// Draw the color patch
DrawColorPatch( hDC, &ClientRect, &rgb, YES );

// If we just drew the active color, we're done
if ( bDrawingActive )
	{
	GrayWindow( hDC, hWindow, WS_NOTENABLED );
	EndPaint( hWindow, &ps );
	return;
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
}

/***********************************************************************/
LOCAL void Color2_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
POINT pt;
BOOL bSelected;
RECT ClientRect;


if ( Control_bTrack )
	return;
SetCapture( hWindow );
Control_bTrack = TRUE;
if ( GetFocus() != hWindow )
	SetFocus( hWindow );

// Compute the rectangle of the active color
GetClientRect( hWindow, &ClientRect );
pt.x = x; pt.y = y;
if ( !PtInRect( &ClientRect, pt ) )
	return;
bSelected = ActiveColorIsActive;
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

if ( PtInRect( &ClientRect, pt ) )
	return;

// Otherwise, switch the active and alternate color values and redraw
SwapActiveAlternate();
UpdateWindow( hWindow );
}

/***********************************************************************/
LOCAL void Color2_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( !Control_bTrack )
	return;
ReleaseCapture(); Control_bTrack = FALSE;
}

/***********************************************************************/
LOCAL void Color2_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 1 );
}

/***********************************************************************/
LOCAL void Color2_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
    POINT pt;
    RECT ClientRect, AltClientRect;
    BOOL bSelected, Bool;

    // Compute the rectangle of the active color
    GetClientRect( hWindow, &ClientRect );
    pt.x = x; pt.y = y;
    if ( !PtInRect( &ClientRect, pt ) )
	    return;
    bSelected = ActiveColorIsActive;
    InflateRect( &ClientRect, -3, -3 );

    AltClientRect = ClientRect;

    if ( !bSelected ) // The active color is higher up on the screen
	{
	    AltClientRect.right -= RectWidth(&ClientRect)/3;
	    AltClientRect.top += RectHeight(&ClientRect)/3;
	    ClientRect.left += RectWidth(&ClientRect)/3;
	    ClientRect.bottom -= RectHeight(&ClientRect)/3;
	}
    else	
    {  // The active color is lower on the screen
	    AltClientRect.left += RectWidth(&ClientRect)/3;
	    AltClientRect.bottom -= RectHeight(&ClientRect)/3;
	    ClientRect.right -= RectWidth(&ClientRect)/3;
	    ClientRect.top += RectHeight(&ClientRect)/3;
	}

    if (!Control_bTrack)
    {
        int Bubble;

        if ( Bool = PtInRect( &ClientRect, pt ) )
        {
            Bubble = IDS_ACTIVECOLOR;
  		    Color2Status( GetActiveColorInfo(), TRUE );
	    }
	    else 
        if( PtInRect( &AltClientRect, pt ))    
        {
            Bubble = IDS_ALTERNATECOLOR;
	        Color2Status( GetAlternateColorInfo(), FALSE );
	    }
        else
        {
            Bubble = IDS_COLORSWATCH;
            if( Hints.fStatusHintsOn )                                    
                HintLine( IDS_COLORSWATCH );
            else
                Status( _T("") );
        }
        if( Hints.fBubbleHintsOn )
            HelpContext.OnHints( hWindow, Bubble );
    }

    // If we are in the active color, then get out without any messages
    if ( PtInRect( &ClientRect, pt ) )
	    return;
    UpdateWindow( hWindow );
}

/***********************************************************************/
LOCAL void Color2_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
Color2_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
LOCAL void Color2_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
Color2_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
LOCAL void Color2_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
HDC   hDC;
RECT  ClientRect;

if ( !StyleOn( hWindow, WS_TABSTOP ) )
	return;
// Draw the focus rectangle
hDC = GetDC( hWindow );
GetClientRect( hWindow, &ClientRect );
InflateRect( &ClientRect, -1, -1 );
FrameRect( hDC, &ClientRect,
	( HBRUSH )GetStockObject( fSetFocus ? BLACK_BRUSH : WHITE_BRUSH ) );
ReleaseDC( hWindow, hDC );
}

/***********************************************************************/
LOCAL COLOR Color2_OnGetColor(HWND hWindow, BOOL fAlternate)
/***********************************************************************/
{
long     Color;
RGBS     rgb;
BYTE     bw;
BOOL     bSelected;
LPIMAGE  lpImage;
FRMTYPEINFO TypeInfo;

if (!fAlternate)
   GetActiveRGB(&rgb);
else
   GetAlternateRGB(&rgb);
CopyRGB( &rgb, &Color );

if (lpImage = GetActiveImage())
	{
	ImgGetTypeInfo(lpImage, &TypeInfo);
	if (TypeInfo.DataType == FDT_LINEART)
		{
      bSelected = ActiveColorIsActive;
		bw = fAlternate == 0 ? 255 : 0;
		if (bSelected)
			bw ^= 255;
		rgb.red = rgb.green = rgb.blue = bw;
		}
	else
	if (TypeInfo.DataType == FDT_PALETTECOLOR)
		{
		BYTE index;

		index = FrameGetNearestIndex(TypeInfo.ColorMap, &rgb);
		rgb = TypeInfo.ColorMap->RGBData[index];
		}
	else
		{
		// get cache pixel for rgb
		FrameRGB2Pixel(ImgGetBaseEditFrame(lpImage), &rgb, ( long *)&Color);

		if (FrameType(ImgGetBaseEditFrame(lpImage)) == FDT_GRAYSCALE)
			frame_getRGB(&Color, 1, &rgb); // get rgb for cache pixel
		}
	Color = RGB2long(rgb);		// get Color for rgb
	}
return(Color);
}

/***********************************************************************/
/***********************************************************************/
// HUE CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL void Hue_OnPaint(HWND hWindow);
LOCAL void Hue_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Hue_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Hue_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Hue_Draw( HDC hDC, LPRECT lpRect, int hue, int hueDefault, BOOL bHasFocus );

static int Hue_hueDefault;

/***********************************************************************/
LONG WINPROC EXPORT Hue_Control(	HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Control_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_DESTROY, Control_OnDestroy);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Control_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Control_OnKillFocus);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
	HANDLE_MSG(hWindow, WM_PALETTECHANGED, Control_OnPaletteChanged);

	HANDLE_MSG(hWindow, WM_PAINT, Hue_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Hue_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Hue_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Hue_OnMouseMove);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL void Hue_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
long lValue;
int hue;
HDC hDC;
BOOL bHasFocus;
RECT ClientRect;

hDC = BeginPaint( hWindow, &ps );
GetClientRect( hWindow, &ClientRect );

// The window long holds the hue and the hue default
lValue = GetWindowLong( hWindow, GWL_HUECOLOR );
hue = bound( (int)LOWORD(lValue), 0, MAX_HUES-1 );
Hue_hueDefault = bound( (int)HIWORD(lValue), 0, MAX_HUES-1 );
bHasFocus = ( GetFocus() == hWindow );
Hue_Draw( hDC, &ClientRect, hue, Hue_hueDefault, bHasFocus );
GrayWindow( hDC, hWindow, WS_NOTENABLED );
EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Hue_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
long lValue;
int dy, hue;
HDC hDC;
RECT ClientRect;
BOOL bHasFocus;

if ( Control_bTrack )
	return;
SetCapture( hWindow ); Control_bTrack = TRUE;
if ( GetFocus() != hWindow )
	SetFocus( hWindow );
GetClientRect( hWindow, &ClientRect );
// The window long holds the hue and the hue default
lValue = GetWindowLong( hWindow, GWL_HUECOLOR );
Hue_hueDefault = bound( (int)HIWORD(lValue), 0, MAX_HUES-1 );
y = bound( y, ClientRect.top, ClientRect.bottom );
y -= ClientRect.top;
dy = ClientRect.bottom - ClientRect.top;
hue = Hue_hueDefault + FMUL( MAX_HUES-1, FGET( y, dy ) );
while ( hue < 0 )
	hue += MAX_HUES;
while ( hue >= MAX_HUES )
	hue -= MAX_HUES;
hDC = GetDC( hWindow );
bHasFocus = ( GetFocus() == hWindow );
Hue_Draw( hDC, &ClientRect, hue, Hue_hueDefault, bHasFocus );
ReleaseDC( hWindow, hDC );
SetWindowLong( hWindow, GWL_HUECOLOR, MAKELONG(hue,Hue_hueDefault) );
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow,
				MAKELONG(hue,Hue_hueDefault) );
}

/***********************************************************************/
LOCAL void Hue_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
long lValue;
int hue;
HDC hDC;
RECT ClientRect;
BOOL bHasFocus;

if ( Control_bTrack )
	return;
GetClientRect( hWindow, &ClientRect );
// The window long holds the hue and the hue default
lValue = GetWindowLong( hWindow, GWL_HUECOLOR );
Hue_hueDefault = bound( (int)HIWORD(lValue), 0, MAX_HUES-1 );
hue = Hue_hueDefault;
hDC = GetDC( hWindow );
bHasFocus = ( GetFocus() == hWindow );
Hue_Draw( hDC, &ClientRect, hue, Hue_hueDefault, bHasFocus );
ReleaseDC( hWindow, hDC );
SetWindowLong( hWindow, GWL_HUECOLOR, MAKELONG(hue,Hue_hueDefault) );
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow,
				MAKELONG(hue,Hue_hueDefault) );
}

/***********************************************************************/
LOCAL void Hue_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
long lValue;
int dy, hue;
HDC hDC;
RECT ClientRect;
BOOL bHasFocus;

if ( !Control_bTrack )
	return;
GetClientRect(hWindow, &ClientRect);

// The window long holds the hue and the hue default
lValue = GetWindowLong( hWindow, GWL_HUECOLOR );
Hue_hueDefault = bound( (int)HIWORD(lValue), 0, MAX_HUES-1 );
y = bound( y, ClientRect.top, ClientRect.bottom );
y -= ClientRect.top;
dy = ClientRect.bottom - ClientRect.top;
hue = Hue_hueDefault + FMUL( MAX_HUES-1, FGET( y, dy ) );
while ( hue < 0 )
	hue += MAX_HUES;
while ( hue >= MAX_HUES )
	hue -= MAX_HUES;
hDC = GetDC( hWindow );
bHasFocus = ( GetFocus() == hWindow );
Hue_Draw( hDC, &ClientRect, hue, Hue_hueDefault, bHasFocus );
ReleaseDC( hWindow, hDC );
SetWindowLong( hWindow, GWL_HUECOLOR, MAKELONG(hue,Hue_hueDefault) );
SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow,
				MAKELONG(hue,Hue_hueDefault) );
}

/***********************************************************************/
LOCAL void Hue_Draw( HDC hDC, LPRECT lpRect, int hue, int hueDefault,
                        BOOL bHasFocus )
/***********************************************************************/
{
int y, dy, ySave;
RGBS rgb;

HSLtoRGB( (BYTE)hue, 255, 127, &rgb );

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
FillRect( hDC, lpRect, ( HBRUSH )GetStockObject(WHITE_BRUSH) );
lpRect->bottom = ySave;

// Draw the color portion of the bar
ySave = lpRect->top;
lpRect->top = y;
// Can't use SuperBlt to draw the patches because the lut changes
DrawColorPatch( hDC, lpRect, &rgb, NO );

// Draw the focus rectangle
if ( bHasFocus )
	{
	lpRect->right--;
//	lpRect->bottom--;
	FrameRect( hDC, lpRect, ( HBRUSH )GetStockObject(BLACK_BRUSH) );
	lpRect->right++;
//	lpRect->bottom++;
	}
lpRect->top = ySave;
}

/***********************************************************************/
/***********************************************************************/
// TEXTBLOCK CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL void TextBlock_OnPaint(HWND hWindow);

/***********************************************************************/
LONG WINPROC EXPORT TextBlock_Control(	HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);

	HANDLE_MSG(hWindow, WM_PAINT, TextBlock_OnPaint);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL void TextBlock_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT PS;
HDC         hDC, hPDC;
DWORD       Style;
BOOL        bCenter;
TCHAR       WindowText[2048];
RECT        Area;
short       OldBkMode;
HGDIOBJ     hOldFont, hFont;

if ( !(hDC = BeginPaint (hWindow,&PS)) )
	return;
Style = GetWindowLong (hWindow,GWL_STYLE);
if ( !TextBlock_GetData( GetDlgCtrlID( hWindow ),
     WindowText, sizeof(WindowText) ) )
	GetWindowText( hWindow, WindowText, sizeof(WindowText) );

bCenter = (Style & SS_CENTER);
OldBkMode = SetBkMode (hDC,TRANSPARENT);
if ( hFont = (HFONT)GetWindowLong(hWindow, GWL_FONT) )
	hOldFont = SelectObject(hDC, hFont);
else	
	{
	hPDC = GetDC( GetParent(hWindow) );
	hFont = SelectObject( hPDC, GetStockObject( GetSystemMetrics( SM_DBCSENABLED ) ? SYSTEM_FONT:ANSI_VAR_FONT ));
	SelectObject( hPDC, hFont );
	hOldFont = SelectObject( hDC, hFont );
	ReleaseDC( GetParent(hWindow), hPDC );
	}
GetClientRect (hWindow,&Area);
DrawText( hDC, WindowText, -1, &Area,
	DT_WORDBREAK | (bCenter ? DT_CENTER : 0) );
SetBkMode (hDC,OldBkMode);
if (hOldFont)
	SelectObject (hDC,hOldFont);
EndPaint (hWindow,&PS);
}

/***********************************************************************/
BOOL TextBlock_GetData( int id, LPTSTR lpData, int iSize )
/***********************************************************************/
{
HGLOBAL hTxtBlk;
LPTSTR 	lpTxtBlk;

if ( !(hTxtBlk = FindResource( PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(id), RT_RCDATA )) )
	return( NO );
if ( !(hTxtBlk = LoadResource( PictPubApp.GetResourceHandle(), ( HRSRC )hTxtBlk )) )
	return( NO );
if ( !(lpTxtBlk = ( LPTSTR )LockResource( hTxtBlk )) )
	{
	FreeResource( hTxtBlk );
	return( NO );
	}

iSize = min( lstrlen(lpTxtBlk), iSize-1 );
copy( ( LPTR )lpTxtBlk, (LPTR)lpData, iSize );
lpData[iSize] = _T('\0');
UnlockResource( hTxtBlk );
FreeResource( hTxtBlk );
return( YES );
}



/***********************************************************************/
/***********************************************************************/
// PANEL CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL void Panel_OnPaint(HWND hWindow);
LOCAL void Panel_CheckPosition(HWND hWindow);
LOCAL void Panel_OnShowWindow(HWND hWindow, BOOL fShow, UINT status);
LOCAL void Panel_DrawBox( HDC hDC, LPRECT lpRect, BOOL bRaised, BOOL bDouble, LPRECT lpClientRect );

/***********************************************************************/
LONG WINPROC EXPORT Panel_Control(	HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);

	HANDLE_MSG(hWindow, WM_PAINT, Panel_OnPaint);
	HANDLE_MSG(hWindow, WM_SHOWWINDOW, Panel_OnShowWindow);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

#define CLOSE 5
#define CLOSETO(n1, n2) (abs((n1)-(n2)) <= CLOSE)

/***********************************************************************/
LOCAL void Panel_OnShowWindow(HWND hWindow, BOOL fShow, UINT status)
/***********************************************************************/
{
	if (fShow)
		Panel_CheckPosition(hWindow);
	FORWARD_WM_SHOWWINDOW(hWindow, fShow, status, Control_DefProc);
}

/***********************************************************************/
LOCAL void Panel_CheckPosition(HWND hWindow)
/***********************************************************************/
{
	HWND hOther;
	RECT rPanel, rOther, rOld;
	int dx, dy, cx, cy;
	char szClass[20];

	GetWindowRect(hWindow, &rPanel);
	dx = rPanel.right-rPanel.left;
	dy = rPanel.bottom-rPanel.top;
	// don't do small panels
	if (dx < 5 || dy < 5)
		return;

	hOther = hWindow;
	while (hOther = GetWindow(hOther, GW_HWNDPREV))
	{
		GetClassName(hOther, szClass, sizeof(szClass));
		if( !StringsEqual( szClass, _T("panel")))
			break;
		GetWindowRect(hOther, &rOther);
		rOld = rPanel;

		if (CLOSETO(rPanel.right, rOther.left))
			rPanel.right = rOther.left;
		else
		if (CLOSETO(rPanel.left, rOther.right))
			rPanel.left = rOther.right;

		if (CLOSETO(rPanel.bottom, rOther.top))
			rPanel.bottom = rOther.top;
		else
		if (CLOSETO(rPanel.top, rOther.bottom))
			rPanel.top = rOther.bottom;

		if (EqualRect(&rPanel, &rOld))
			continue;

		cx = rPanel.right - rPanel.left;
		cy = rPanel.bottom - rPanel.top;
		ScreenToClient(GetParent(hWindow), (LPPOINT)&rPanel.left);
		SetWindowPos(hWindow, NULL, rPanel.left, rPanel.top, cx, cy,
						SWP_NOACTIVATE|SWP_NOZORDER);
	}
}

/***********************************************************************/
LOCAL void Panel_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
HDC         hDC;
DWORD       Style;
BOOL        bCenter;
RECT        Area;
short       OldBkMode;
STRING      WindowText;
HGDIOBJ     hOldPen;
HGDIOBJ     hOldFont;

if ( !(hDC = BeginPaint (hWindow,&ps)) )
	return;

GetClientRect( hWindow, &Area );

EraseControlBackground( hDC, hWindow, &Area, CTLCOLOR_BTN );

Style = GetWindowLong (hWindow,GWL_STYLE);
GetWindowText( hWindow, WindowText, sizeof(WindowText) );

if (lstrlen(WindowText))
	{
	OldBkMode = SetBkMode (hDC,TRANSPARENT);
 	bCenter = YES;
	hOldFont = SelectObject( hDC, GetStockObject( GetSystemMetrics( SM_DBCSENABLED ) ? SYSTEM_FONT:ANSI_VAR_FONT ));
	GetClientRect (hWindow,&Area);
	DrawText( hDC, WindowText, -1, &Area,
		DT_WORDBREAK | (bCenter ? DT_CENTER : 0) );
	SetBkMode (hDC,OldBkMode);
	if (hOldFont)
		SelectObject (hDC,hOldFont);
	}

if (Style & PS_BLACKLINE)
	{
	hOldPen = SelectObject(hDC, GetStockObject(BLACK_PEN));
	MoveTo(hDC, Area.left, Area.top);
	LineTo(hDC, Area.right, Area.top);
	SelectObject(hDC, hOldPen);
	++Area.top;
	}

if (Style & PS_VDIVIDER)
	{
	// draw the drop shadow
	hOldPen = SelectObject( hDC, DrawTools.GetBtnShadowPen() );
	MoveTo( hDC, Area.left, Area.top );
	LineTo( hDC, Area.left, Area.bottom );

	// draw the white
	SelectObject( hDC, DrawTools.GetBtnHilightPen() );
	MoveTo( hDC, Area.left+1, Area.top );
	LineTo( hDC, Area.left+1, Area.bottom );

	SelectObject( hDC, hOldPen );
	}
else
if (Style & PS_HDIVIDER)
	{
	// draw the drop shadow
	hOldPen = SelectObject( hDC, DrawTools.GetBtnShadowPen() );
	MoveTo( hDC, Area.left, Area.top );
	LineTo( hDC, Area.right, Area.top );

	// draw the white
	SelectObject( hDC, DrawTools.GetBtnHilightPen() );
	MoveTo( hDC, Area.left, Area.top+1 );
	LineTo( hDC, Area.right, Area.top+1 );

	SelectObject( hDC, hOldPen );
	}
else
	{
	RECT rClient;
	HWND hDialog;
	DWORD dwDialogStyle;
	
	hDialog = GetParent(hWindow);
	dwDialogStyle = GetWindowLong( hDialog, GWL_STYLE );
	if (Control.fNewLook && (dwDialogStyle & WS_DLGFRAME) )
		{
		GetClientRect(hDialog, &rClient);
		ClientToScreen(hDialog, (LPPOINT)&rClient.left);
		ClientToScreen(hDialog, (LPPOINT)&rClient.right);
		ScreenToClient(hWindow, (LPPOINT)&rClient.left);
		ScreenToClient(hWindow, (LPPOINT)&rClient.right);
		Panel_DrawBox( hDC, &Area, YES, NO, &rClient );
		}
	else
		Panel_DrawBox( hDC, &Area, YES, NO, NULL );
	}

EndPaint (hWindow,&ps);
}

/***********************************************************************/
void Panel_DrawBox( HDC hDC, LPRECT lpRect, BOOL bRaised, BOOL bDouble,
						LPRECT lpClientRect)
/***********************************************************************/
{
HGDIOBJ	hOldPen, hPen1, hPen2;
RECT	Rect, rClient;
BOOL	fParentBeveled, fNoDraw, fNoDrawNext;

Rect = *lpRect;
WindowsToAstralRect(&Rect);
if (fParentBeveled = (lpClientRect != NULL))
	{
	rClient = *lpClientRect;
	WindowsToAstralRect(&rClient);
	}

if ( bRaised )
	{ hPen1 = DrawTools.GetBtnHilightPen(); hPen2 = DrawTools.GetBtnShadowPen(); }
else	{ hPen1 = DrawTools.GetBtnShadowPen(); hPen2 = DrawTools.GetBtnHilightPen(); }

// Draw the upper left border
hOldPen = SelectObject( hDC, hPen1 );
MoveTo( hDC, Rect.left, Rect.bottom );

fNoDraw = fParentBeveled && CLOSETO(Rect.left, rClient.left);
fNoDrawNext = fParentBeveled && CLOSETO(Rect.top, rClient.top);
if (fNoDraw)
	MoveTo( hDC, Rect.left, Rect.top );
else
	{
	// if we aren't going to draw next line then we need
	// to draw this line 1 pixel farther
	if (fNoDrawNext)
		LineTo( hDC, Rect.left, Rect.top-1 );
	else
		LineTo( hDC, Rect.left, Rect.top );
	}

fNoDraw = fNoDrawNext;
fNoDrawNext = fParentBeveled && CLOSETO(Rect.right, rClient.right);
if (fNoDraw)
	MoveTo( hDC, Rect.right, Rect.top );
else
	{
	// if we aren't going to draw next line then we need
	// to draw this line 1 pixel farther
	if	(fNoDrawNext)
		LineTo( hDC, Rect.right+1, Rect.top );
	else
		LineTo( hDC, Rect.right, Rect.top );
	}

// Draw the drop shadow
SelectObject( hDC, hPen2 );

fNoDraw = fNoDrawNext;
fNoDrawNext = fParentBeveled && CLOSETO(Rect.bottom, rClient.bottom);
if	(fNoDraw)
	MoveTo( hDC, Rect.right, Rect.bottom );
else
	{
	// if we aren't going to draw next line then we need
	// to draw this line 1 pixel farther
    if (fNoDrawNext)
		LineTo( hDC, Rect.right, Rect.bottom+1 );
	else
		LineTo( hDC, Rect.right, Rect.bottom );
	}

fNoDraw = fNoDrawNext;
fNoDrawNext = fParentBeveled && CLOSETO(Rect.left, rClient.left);
if	(fNoDraw)
	MoveTo( hDC, Rect.left, Rect.bottom );
else
	{
    if (fNoDrawNext)
		LineTo( hDC, Rect.left-1, Rect.bottom );
	else
		LineTo( hDC, Rect.left, Rect.bottom );
	}

// double disabled for now
//if ( bDouble )
//	{
//	InflateRect( &Rect, -1, -1 );
//	// Draw the upper left border
//	SelectObject( hDC, hPen1 );
//	MoveTo( hDC, Rect.left, Rect.bottom );
//	LineTo( hDC, Rect.left, Rect.top );
//	LineTo( hDC, Rect.right, Rect.top );
//	// Draw the drop shadow
//	SelectObject( hDC, hPen2 );
//	LineTo( hDC, Rect.right, Rect.bottom );
//	LineTo( hDC, Rect.left, Rect.bottom );
//	InflateRect( &Rect, -1, -1 );
//	}

SelectObject( hDC, hOldPen );
}

/***********************************************************************/
/***********************************************************************/
// IMAGE CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL void Image_OnPaint(HWND hWindow);
LOCAL void Image_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Image_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Image_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Image_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Image_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Image_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Image_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void Image_OnGetMinMaxInfo(HWND hWindow, LPMINMAXINFO lpMinMaxInfo);
LOCAL void Image_OnSize(HWND hWindow, UINT state, int cx, int cy);
LOCAL void Image_OnMove(HWND hWindow, int x, int y);
LOCAL BOOL Image_OnWindowPosChanging(HWND hWindow, LPWINDOWPOS lpwpos);
LOCAL UINT Image_OnNCHitTest(HWND hwnd, int x, int y);
LOCAL UINT HandleNonClientHit( HWND hWindow, int x, int y );
LOCAL LPFRAME GetImageFrame( HWND hWnd, LPINT lpbMustClose );
LOCAL BOOL Image_OnEraseBkgnd(HWND hWindow, HDC hDC);

/***********************************************************************/
LONG WINPROC EXPORT Image_Control(	HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_DESTROY, Control_OnDestroy);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);
//	HANDLE_MSG(hWindow, WM_SETFOCUS, Control_OnSetFocus);
//	HANDLE_MSG(hWindow, WM_KILLFOCUS, Control_OnKillFocus);
	HANDLE_MSG(hWindow, WM_PALETTECHANGED, Control_OnPaletteChanged);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Image_OnEraseBkgnd);

	HANDLE_MSG(hWindow, WM_PAINT, Image_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Image_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Image_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Image_OnLButtonDblClk);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Image_OnMouseMove);
	HANDLE_MSG(hWindow, WM_GETMINMAXINFO, Image_OnGetMinMaxInfo);
	HANDLE_MSG(hWindow, WM_SIZE, Image_OnSize);
	HANDLE_MSG(hWindow, WM_MOVE, Image_OnMove);
	HANDLE_MSG(hWindow, WM_WINDOWPOSCHANGING, Image_OnWindowPosChanging);
	HANDLE_MSG(hWindow, WM_NCHITTEST, Image_OnNCHitTest);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL void Image_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
HDC         hDC;
RECT        DestRect;
TOOLPROC    lpToolProc;

if ( SuperPaint( hWindow ) )
	return; // Printing in progress... (SuperBlt not reentrant)
hDC = BeginPaint( hWindow, &ps );
if (!IsRectEmpty(&ps.rcPaint))
	{
	LPVOID lpCmsXform = NULL;
	LPIMAGE lpImage;

	if( StyleOn( hWindow, IS_ACTIVEIMAGECMS ) )
		{
		LPIMAGE lpImage = GetActiveImage();
		if (lpImage)
			lpCmsXform = lpImage->m_cmsXform;
		}
	else
	if( StyleOn( hWindow, IS_CMS ) )
		lpCmsXform = ColorManager.CmsXform;

	SetRectEmpty( &DestRect );
	if (StyleOn(hWindow, IS_OBJECT))
		DrawObjListControl(hDC, hWindow, NULL, &DestRect, 0, 0, NO, lpCmsXform);
	else
		DrawImageControl( hDC, hWindow, NULL, &DestRect, 0, 0, NO, lpCmsXform );
	if ( !IsRectEmpty(&DestRect) && (lpToolProc = (TOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC )) )
		{
		(*lpToolProc)( hWindow, NULL, DestRect.left, DestRect.top, WM_MOVE );
		(*lpToolProc)( hWindow, NULL, RectWidth(&DestRect), RectHeight(&DestRect), WM_SIZE );
		(*lpToolProc)( hWindow, hDC, 0, 0, WM_PAINT );
		}
	}
EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Image_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
TOOLPROC lpToolProc;

ASSERT(Control_bTrack == 0);
if ( Control_bTrack )
	return;
if (hWindow != GetFocus())
	SetFocus(hWindow);
if ( lpToolProc = (TOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ) )
	{
	(*lpToolProc)( hWindow, NULL, x, y, WM_LBUTTONDOWN );
	SetCapture( hWindow );
	Control_bTrack = TRUE;
	}
}

/***********************************************************************/
LOCAL void Image_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
TOOLPROC lpToolProc;

SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0);
if ( !Control_bTrack )
	return;
if ( lpToolProc = (TOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ) )
	{
	(*lpToolProc)( hWindow, NULL, x, y, WM_LBUTTONUP );
	ReleaseCapture();
	Control_bTrack = FALSE;
	}
}

/***********************************************************************/
LOCAL void Image_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
TOOLPROC lpToolProc;

SEND_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 1);
if ( lpToolProc = (TOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ) )
	(*lpToolProc)( hWindow, NULL, x, y, WM_LBUTTONDBLCLK );
}

/***********************************************************************/
LOCAL void Image_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
TOOLPROC lpToolProc;

if ( lpToolProc = (TOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ) )
	(*lpToolProc)( hWindow, NULL, x, y, WM_MOUSEMOVE );
}

/***********************************************************************/
LOCAL void Image_OnGetMinMaxInfo(HWND hWindow, LPMINMAXINFO lpmmi)
/***********************************************************************/
{
RECT ClientRect;
HWND hParent;

if (!StyleOn(hWindow, IS_LIMITSIZE))
	return;
if (!(hParent = GetParent(hWindow)))
	return;
GetClientRect(hParent, &ClientRect);
lpmmi->ptMaxSize.x = RectWidth(&ClientRect);
lpmmi->ptMaxSize.y = RectHeight(&ClientRect);
lpmmi->ptMaxPosition.x = 0;
lpmmi->ptMaxPosition.y = 0;
lpmmi->ptMinTrackSize.x = 1;
lpmmi->ptMinTrackSize.y = 1;
if (!StyleOn(hWindow, IS_RATIO_CONSTRAIN))
	{
  	lpmmi->ptMaxTrackSize.x = 2*RectWidth(&ClientRect);
	lpmmi->ptMaxTrackSize.y = 2*RectHeight(&ClientRect);
	}
}

/***********************************************************************/
LOCAL void Image_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
SetLongBit(hWindow, GWL_STYLE, IS_HASCHANGED, TRUE);
FORWARD_WM_SIZE(hWindow, state, cx, cy, Control_DefProc);
}

/***********************************************************************/
LOCAL void Image_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
SetLongBit(hWindow, GWL_STYLE, IS_HASCHANGED, TRUE);
FORWARD_WM_MOVE(hWindow, x, y, Control_DefProc);
}

/***********************************************************************/
LOCAL BOOL Image_OnWindowPosChanging(HWND hWindow, LPWINDOWPOS lpWinPos)
/***********************************************************************/
{
LPFRAME lpFrame;
LPOBJECT lpBase;
BOOL bMustClose;
int x, y, w, h, iImageWidth, iImageHeight;
HWND hParent;
RECT ClientRect, rWindow;

if ( !StyleOn(hWindow, IS_RATIO_CONSTRAIN) )
	return FORWARD_WM_WINDOWPOSCHANGING(hWindow, lpWinPos, Control_DefProc);
// constrain window to have same aspect ratio as its frame
if (StyleOn(hWindow, IS_OBJECT))
	{
	lpBase = (LPOBJECT)GetWindowLong( hWindow, GWL_IMAGE );
	if (lpBase)
		lpFrame = ObjGetEditFrame(lpBase);
	else
		lpFrame = NULL;
	}
else
	lpFrame = GetImageFrame( hWindow, &bMustClose );		
if ( !lpFrame )
	{
	iImageWidth = 1;
	iImageHeight = 1;
	}
else
	{
	iImageWidth  = FrameXSize(lpFrame);
	iImageHeight = FrameYSize(lpFrame);
	}
GetWindowRect(hWindow, &rWindow);
x= RectWidth(&rWindow)-1;
y = RectHeight(&rWindow)-1;
w = lpWinPos->cx;
h = lpWinPos->cy;
hParent = GetParent(hWindow);
if (w != x && h != y)
	ConstrainRatio(&w,&h, iImageWidth, iImageHeight);
else if (w != x && h == y)
	MakeRatio(&w, &h, iImageWidth, iImageHeight);
else if (w == x && h != y)
	MakeRatio(&h, &w, iImageHeight, iImageWidth);
else
	ConstrainRatio(&w,&h, iImageWidth, iImageHeight);

if (StyleOn(hWindow, IS_LIMITSIZE) && hParent)
	{
	x=w;
	y=h;
	GetClientRect(hParent, &ClientRect);
	w = Bound(w, 0,  2*RectWidth(&ClientRect)-1);
	h = Bound(h, 0,  2*RectHeight(&ClientRect)-1);
	if (x != w || y != h)
		ConstrainRatio(&w,&h, iImageWidth,
		iImageHeight);	// always makes smaller
	}

lpWinPos->cx = w;
lpWinPos->cy = h;
GetWindowRect(hWindow, &rWindow);
lpWinPos->flags = lpWinPos->flags | SWP_DRAWFRAME;
// be sure we are still visible
if (!hParent)
	return(FALSE);
GetClientRect(hParent, &ClientRect);
GetWindowRect(hWindow, &rWindow);
lpWinPos->x = Bound(lpWinPos->x, -w+2, ClientRect.right-2);
lpWinPos->y = Bound(lpWinPos->y, -h+2, ClientRect.bottom-2);
return( FALSE );
}

/***********************************************************************/
LOCAL UINT Image_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
if ( StyleOn( hWindow, IS_MOVEABLE ) )
	return( HandleNonClientHit( hWindow, x, y ));
else
	return FORWARD_WM_NCHITTEST(hWindow, x, y, Control_DefProc);
}

/***********************************************************************/
LOCAL BOOL Image_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
RECT ClientRect;

GetClientRect( hWindow, &ClientRect );
EraseControlBackground( hDC, hWindow, &ClientRect, CTLCOLOR_BTN);
return(TRUE);
}

/***********************************************************************/
void DrawImageControl( HDC hDC, HWND hWnd, LPFRAME lpFrame, LPRECT lpDestRect,
                           int dx, int dy, BOOL bShadows,
						   LPVOID lpCmsXform)
/***********************************************************************/
{
int iImageWidth, iImageHeight, yAmount, iOldMode, iWidth, iHeight;
RECT SourceRect, DestRect;
BOOL bMustClose;
STRING szText;
TEXTMETRIC metrics;
COLORREF lOldColor;

bMustClose = NO;

if ( lpDestRect && !IsRectEmpty(lpDestRect) )
	DestRect = *lpDestRect;
else
	{
	GetClientRect( hWnd, &DestRect );
	WindowsToAstralRect(&DestRect);
	}

if ( !lpFrame )
	lpFrame = GetImageFrame( hWnd, &bMustClose ); 
if ( !lpFrame)
	{ // use text if style is on
	if (!StyleOn(hWnd, WS_TEXT))
		return;
	if (!GetWindowText(hWnd,szText,sizeof(szText)))
		return;
	GetTextMetrics(hDC, &metrics);
	iOldMode = SetBkMode( hDC, TRANSPARENT );
	lOldColor = SetTextColor(hDC, RGB(255,0,0));
	yAmount = (RectHeight(&DestRect)-(2*metrics.tmHeight)) / 2;
	DestRect.top += yAmount;
	DrawText( hDC, szText, -1, &DestRect,
		DT_NOPREFIX | DT_WORDBREAK | DT_CENTER);
	SetBkMode( hDC, iOldMode );
	SetTextColor( hDC, lOldColor ); 
	return;
	}

iImageWidth  = FrameXSize(lpFrame);
iImageHeight = FrameYSize(lpFrame);

SourceRect.left = SourceRect.top = 0;
SourceRect.right  = iImageWidth - 1;
SourceRect.bottom = iImageHeight - 1;

if ( !StyleOn( hWnd, IS_STRETCHABLE ) )
	ComputeImageControlRect( hWnd, &DestRect, iImageWidth, iImageHeight,
							bShadows );
if (StyleOn(hWnd, IS_NOSAMPLING))
	{
	iWidth = RectWidth(&DestRect);
	if (iWidth < iImageWidth)
		SourceRect.right = SourceRect.left + iWidth;
	iHeight = RectHeight(&DestRect);
	if (iHeight < iImageHeight)
		SourceRect.bottom = SourceRect.top + iHeight;
	}
OffsetRect( &DestRect, dx, dy );

// Draw the image
DrawFrameEx( lpFrame, hDC, &SourceRect, &DestRect, NULL, lpCmsXform );

if ( bMustClose )
	FrameClose( lpFrame );
if ( lpDestRect )
	*lpDestRect = DestRect;
}

//************************************************************************
//		Image_SetFrame
// DESCRIPTION:
//		Call to set the image displayed by an image control.
//		Repaints the control.
//************************************************************************
void Image_SetFrame(HWND hControl, LPFRAME lpFrame)
//************************************************************************
{					
	LONG lValue;
	LPFRAME lpOldFrame;
	BOOL bErase;
	LONG width, height;
	LFIXED fRatio1, fRatio2;
	
	bErase = FALSE;
	if (!hControl)
		return;
	lValue = GetWindowLong( hControl, GWL_IMAGE ); // Normally a frame pointer
	if (lpFrame)
	{
		if (lValue > 255)
		{
			lpOldFrame = (LPFRAME)lValue;
			width  = FrameXSize(lpOldFrame);
			height = FrameYSize(lpOldFrame);
			fRatio1 = FGET(height, width);
			width  = FrameXSize(lpFrame);
			height = FrameYSize(lpFrame);
			fRatio2 = FGET(height, width);
			if (fRatio1 != fRatio2)
				bErase = TRUE;	// change in size
		}
//		else
//			bErase = TRUE;
	}
	else if (lValue)
		bErase = TRUE;		// no new frame
	SetWindowLong(hControl, GWL_IMAGE, (LONG)lpFrame);
//	if (bErase)
//	{	
//		// since erase background normally does nothing
//		hDC = GetDC( hControl );
//		GetClientRect( hControl, &ClientRect );
//		EraseControlBackground( hDC, hControl, &ClientRect, CTLCOLOR_BTN);
//		ReleaseDC( hControl, hDC );
//	}
	InvalidateRect( hControl, NULL, bErase );
}
	
/***********************************************************************/
void DrawObjListControl( HDC hDC, HWND hWnd, LPOBJECT lpBase,
						LPRECT lpDestRect, int dx, int dy, BOOL bShadows,
						LPVOID lpCmsXform)
/***********************************************************************/
{
int iImageWidth, iImageHeight, yAmount, iOldMode;
RECT SourceRect, DestRect;
LPFRAME lpFrame;
STRING szText;
TEXTMETRIC metrics;
COLORREF lOldColor;

if ( lpDestRect && !IsRectEmpty(lpDestRect) )
	DestRect = *lpDestRect;
else
	{
	GetClientRect( hWnd, &DestRect );
	}

if ( !lpBase )
	lpBase = (LPOBJECT)GetWindowLong( hWnd, GWL_IMAGE ); 
if ( !lpBase)
	{ // use text if style is on
	if (!StyleOn(hWnd, WS_TEXT))
		return;
	if (!GetWindowText(hWnd,szText,sizeof(szText)))
		return;
	GetTextMetrics(hDC, &metrics);
	iOldMode = SetBkMode( hDC, TRANSPARENT );
	lOldColor = SetTextColor(hDC, RGB(255,0,0));
	yAmount = (RectHeight(&DestRect)-(2*metrics.tmHeight)) / 2;
	DestRect.top += yAmount;
	DrawText( hDC, szText, -1, &DestRect,
		DT_NOPREFIX | DT_WORDBREAK | DT_CENTER);
	SetBkMode( hDC, iOldMode );
	SetTextColor( hDC, lOldColor ); 
	return;
	}

lpFrame = ObjGetEditFrame(lpBase);
iImageWidth  = FrameXSize(lpFrame);
iImageHeight = FrameYSize(lpFrame);

SourceRect.left = SourceRect.top = 0;
SourceRect.right  = iImageWidth - 1;
SourceRect.bottom = iImageHeight - 1;

if ( !StyleOn( hWnd, IS_STRETCHABLE ) )
	ComputeImageControlRect( hWnd, &DestRect, iImageWidth, iImageHeight,
							bShadows );

OffsetRect( &DestRect, dx, dy );

// Draw the image
DrawObjList( hDC, lpBase, &SourceRect, &DestRect, lpCmsXform);

if ( lpDestRect )
	*lpDestRect = DestRect;
}

/***********************************************************************/
void ComputeImageControlRect( HWND hWnd, LPRECT lpRect, int iImageWidth, int iImageHeight, BOOL bShadows )
/***********************************************************************/
{
int Width, Height, OldWidth, OldHeight;

if (bShadows)
	InflateRect(lpRect, -3, -3); // adjust for beveling
Width = OldWidth = RectWidth( lpRect );
Height = OldHeight = RectHeight( lpRect );
ScaleToFit( &Width, &Height, iImageWidth, iImageHeight );
if (StyleOn(hWnd, IS_NOSAMPLING))
	{
	if (Width > iImageWidth)
		Width = iImageWidth;
	if (Height > iImageHeight)
		Height = iImageHeight;
	}

lpRect->left += ( ( OldWidth - Width ) / 2 );
lpRect->right = lpRect->left + Width - 1;
lpRect->top  += ( ( OldHeight - Height ) / 2 );
lpRect->bottom = lpRect->top + Height - 1;
}

/***********************************************************************/
LOCAL LPFRAME GetImageFrame( HWND hWnd, LPINT lpbMustClose )
/***********************************************************************/
{
DWORD lValue;
STRING szString;
FNAME szFileName;
LPFRAME lpFrame;

*lpbMustClose = NO;
lValue = GetWindowLong( hWnd, GWL_IMAGE ); // Normally a frame pointer
if ( !lValue )
	{ 
	// is it a text frame?
	if (StyleOn(hWnd, WS_TEXT))
		return(NULL);
	// Check the window name to see if it references an image
	GetWindowText( hWnd, szString, sizeof(szString) );

	if( !( *szString ))
		return( NULL );

	lstrcpy( szFileName, Control.ProgHome );
	lstrcat( szFileName, szString );
	CReadImage read(szFileName);
	lpFrame = read.ReadFrame();
	*lpbMustClose = YES;
	}
else
if ( lValue < 256 )
	{ // Its a numeric reference to an open image document
	ASSERT(FALSE);
//	LPIMAGE lpBaseImage;
//	nImage = lValue - 1;
//	nDocs = PictPubApp.GetDocCount();
//    if (nImage >= nDocs)
//        return (NULL);
//
//	lpFrame = ImgGetBaseEditFrame(lpBaseImage);
	}
else	
	lpFrame = (LPFRAME)lValue;

return( lpFrame );
}

/***********************************************************************/
LOCAL LPOBJECT GetImageObject( HWND hWnd )
/***********************************************************************/
{
DWORD lValue;
LPOBJECT lpObject;
LPIMAGE lpImage;
int nObject;

lValue = GetWindowLong( hWnd, GWL_IMAGE ); // Normally an Object pointer
if ( !lValue )
	{ // Check the window name to see if it references an image
	lpObject = NULL;
//	GetWindowText( hWnd, szString, sizeof(szString) );
//	if ( !szString[0] )
//		return( NULL );
//	lstrcpy( szFileName, Control.ProgHome );
//	lstrcat( szFileName, szString );
// CReadImage read(szFileName);
// lpFrmae = read.ReadFrame();
	}
else
if ( lValue < 256 )
	{ // Its a numeric reference to an open image document
	if (!(lpImage = GetActiveImage()))
		return(NULL);
	nObject = lValue - 1;
	if ( nObject >= ImgCountFloaters(lpImage) )
		return( NULL );
	lpObject = ImgGetObject(lpImage, nObject);
	}
else	lpObject = (LPOBJECT)lValue;

return( lpObject );
}


/***********************************************************************/
LOCAL UINT HandleNonClientHit( HWND hWindow, int x, int y )
/***********************************************************************/
{
RECT Rect, At;
UINT hit;
int dx, dy;

hit = FORWARD_WM_NCHITTEST(hWindow, x, y, Control_DefProc);
if ( hit != HTCLIENT ) // Return all non-client hits as is
	return( hit );
dx = 10;
dy = 10;
GetWindowRect( hWindow, &Rect );
// Turn client hits into moves and size hits
At.left   = ( abs( x - Rect.left   ) < dx );
At.right  = ( abs( x - Rect.right  ) < dx );
At.top    = ( abs( y - Rect.top    ) < dy );
At.bottom = ( abs( y - Rect.bottom ) < dy );

if ( At.left )
	{
	if ( At.top )		hit = HTTOPLEFT; else
	if ( At.bottom )	hit = HTBOTTOMLEFT; else
						hit = HTLEFT;
	}
else
if ( At.right )
	{
	if ( At.top )		hit = HTTOPRIGHT; else
	if ( At.bottom )	hit = HTBOTTOMRIGHT; else
						hit = HTRIGHT;
	}
else
	{
	if ( At.top )		hit = HTTOP; else
	if ( At.bottom )	hit = HTBOTTOM; else
						hit = HTCAPTION;
	}
return( hit );
}

/***********************************************************************/
void EraseControlBackground( HDC hDC, HWND hWnd, LPRECT lpRect, WORD wType )
/***********************************************************************/
{
	if( !( GetWindowLong( hWnd, GWL_STYLE ) & WS_NOBGND ) )
	{	
		HBRUSH hBrush;
		BOOL fCreatedBrush = FALSE;

		// see if control has a background of its own
		hBrush = GetBackgroundBrush( hDC, hWnd, wType );
		if ((int)hBrush-1 <= 20/*COLOR_ENDCOLORS*/)
		{
	    	hBrush = CreateSolidBrush( GetSysColor((int)hBrush-1) );
			fCreatedBrush = TRUE;
		}
    	FillRect( hDC, lpRect, hBrush );
		if (fCreatedBrush)
			DeleteObject(hBrush);
	}
}


/***********************************************************************/
HBRUSH GetBackgroundBrush( HDC hDC, HWND hWnd, WORD wType )
/***********************************************************************/
{
	HBRUSH	hBrush;
 	HWND	hParent;

	hBrush = SetClassBackground(hWnd, NULL);
	if (!hBrush)
	{
		hParent = GetParent(hWnd);
		if (hParent)
			hBrush = SetClassBackground(hWnd, NULL);
	}
	if (!hBrush)
		hBrush = (HBRUSH)(COLOR_BTNFACE+1);
//	if ((int)hBrush-1 <= 20/*COLOR_ENDCOLORS*/)
//	{
//		SetBkColor (hDC, GetSysColor ((int)hBrush-1));
//	}
//	else
//	{
//		LOGBRUSH brush;
//		int nCount = sizeof(LOGBRUSH);
//		if (GetObject(hBrush, nCount, (LPTR)&brush) == nCount)
//			SetBkColor(hDC, brush.lbColor);
//		else
//			SetBkColor (hDC, GetSysColor (COLOR_BTNFACE));
//	}
	return( hBrush );
}

// Constrains *w, *h so that they have same ratio as width, height
/***********************************************************************/
LOCAL void ConstrainRatio( LPINT w, LPINT h, int width, int height )
/***********************************************************************/
{
	LFIXED fRatio, fPreposedRatio;

	fRatio = FGET(width,height);
	fPreposedRatio	= FGET(*w,*h);
	if (fPreposedRatio > fRatio)
		*w = FMUL(*h, fRatio);
	else if (fPreposedRatio < fRatio)
		*h = FMUL(*w, FGET(height, width));
}

// Constrains *w, *h so that they have same ratio as width, height
// the *w value takes precidence
/***********************************************************************/
LOCAL void MakeRatio( LPINT w, LPINT h, int width, int height )
/***********************************************************************/
{
	LFIXED fRatio, fPreposedRatio;

	fRatio = FGET(height, width);
	fPreposedRatio	= FGET(*h,*w);
	if (fPreposedRatio != fRatio)
		*h = FMUL(*w, fRatio);
}
