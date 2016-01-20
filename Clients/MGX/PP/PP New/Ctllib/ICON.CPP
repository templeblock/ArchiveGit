// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"
#include "ctltools.h"
#include "icon.h"
#include "macros.h"

#ifdef DLGEDIT
void Control_GetClassInfo(LPTSTR lpClassName, LPWORD lpWidth, LPWORD lpHeight, LPDWORD lpStyle)
{
lstrcpy(lpClassName, MGXCTL_ICON);
*lpWidth = 40;
*lpHeight = 12;
*lpStyle = 0;
}
BOOL Control_RegisterClass(HINSTANCE hInstance, UINT ClassScope)
{
return(Icon_RegisterClass(hInstance, ClassScope));
}
#define MgxIcon_Control DlgEdit_Control
extern "C"{
LONG WINPROC MgxIcon_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
}
#else
LONG WINPROC MgxIcon_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
#endif

#define WINDOW_EXTRA (GWW_ICONID+sizeof(WORD))

/* GWW_STATUS possible values */
#define ST_TRACK		0x0001
#define ST_INRECT		0x0002
#define ST_TIMERON		0x0004

/* void Cls_OnSetState(HWND hwnd, BOOL fState) */
#define HANDLE_BM_SETSTATE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (BOOL)wParam), 0L)
/* void Cls_OnSetCheck(HWND hwnd, BOOL fCheck) */
#define HANDLE_BM_SETCHECK(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (BOOL)wParam), 0L)
#ifndef WINDOWS95_HACK
#define BM_SETSTATE16 BTNM_SETSTATE16
#define BM_SETCHECK16 BTNM_SETCHECK16
/* void Cls_OnSetState(HWND hwnd, BOOL fState) */
#define HANDLE_BM_SETSTATE16(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (BOOL)wParam), 0L)
/* void Cls_OnSetCheck(HWND hwnd, BOOL fCheck) */
#define HANDLE_BM_SETCHECK16(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (BOOL)wParam), 0L)
#endif

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
LOCAL void Icon_Draw(HDC hDC, HWND hWnd, LPRECT lpRect, BOOL bSelected,
		BOOL Control_bTrackInRect );

BOOL Icon_RegisterClass (
HINSTANCE hInstance, UINT ClassScope)
{
WNDCLASS WndClass;

/* Setup and define the window class for 3 dimensional icons */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= MGXCTL_ICON;
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS|ClassScope;
WndClass.lpfnWndProc	= MgxIcon_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= WINDOW_EXTRA;
return(RegisterClass( &WndClass ));
}

BOOL Icon_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)MGXCTL_ICON, hInstance));
}

/***********************************************************************/
LONG WINPROC EXPORT MgxIcon_Control(	HWND hWindow, UINT message,
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
	#ifndef WINDOWS95_HACK
	HANDLE_MSG(hWindow, BM_SETSTATE16, Control_OnSetState);
	HANDLE_MSG(hWindow, BM_SETCHECK16, Control_OnSetCheck);
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
Ctl_EraseBackground( hWindow, hDC, &ClientRect, CTLCOLOR_BTN );
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
	FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0, SendMessage );
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

// see if button was last drawn down
bHasFocus = ( GetFocus() == hWindow );
Control_bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
bSelected = GetWindowWord(hWindow, GWW_STATE);
bDown = ( bSelected || (Control_bTrack && bInRect && bHasFocus) );

if ( IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT)
	&& !StyleOn(hWindow, WS_ONTIMER) )
	{
	FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0, SendMessage );
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
FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 1, SendMessage );
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
		bSelected = GetWindowWord(hWindow, GWW_STATE);
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
FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0, SendMessage );
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
LOCAL void Icon_Draw(HDC hDC, HWND hWnd, LPRECT lpRect, BOOL bSelected,
		BOOL Control_bTrackInRect )
/***********************************************************************/
{
int         x, y, i;
HICON       hResource;
HBITMAP     hBitmap;
BOOL        bHasFocus, bDown, bEnabled, bShadows, bHasText, bBorderOnly;
ITEMID      id;
DWORD       dwStyle;
STRING      szString;
HGDIOBJ     hFont;
TEXTMETRIC  tm;

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
		Ctl_EraseBackground( hWnd, hDC, lpRect, CTLCOLOR_BTN );
	}
else
if ( !bDown || !bEnabled || bBorderOnly)
		FillRect( hDC, lpRect, DrawTools.GetBtnFaceBrush() );
else	HilightRect( hDC, lpRect, DrawTools.GetBtnHilightBrush() );

// Compute the client rect center
if ( !bHasText )
	x = ( lpRect->right + lpRect->left + 1 ) / 2;
else	x =   lpRect->right - 16; // All the way to the right
y = ( lpRect->bottom + lpRect->top + 1 ) / 2;
if ( bDown && bEnabled && !bBorderOnly)
	{ x += 2; y += 2; }

if ( dwStyle & WS_BITMAP )
	{ // Draw the Bitmap
	id = GetDlgCtrlID( hWnd );
	if ( hBitmap =LoadBitmap(GetWindowInstance(hWnd),MAKEINTRESOURCE(id)))
		{
		DrawBitmap( hDC, x, y, hBitmap );
		DeleteObject( hBitmap );
		}
	}
else
	{ // Draw the icon
	hResource = NULL;
	if ( id = GetDlgCtrlID( hWnd )  && Ctl_GetIconInstance())
		hResource = LoadIcon( Ctl_GetIconInstance(),
			MAKEINTRESOURCE(id) );
	if ( !hResource && (id = GetWindowWord( hWnd, GWW_ICONID )) && Ctl_GetIconInstance() )
		hResource = LoadIcon( Ctl_GetIconInstance(), MAKEINTRESOURCE(id) );
	if ( hResource )
		DrawIcon( hDC, x-16, y-16, hResource);
	}

if ( bHasText )
	{ // Draw the text left justified
	if ( !(hFont = GetStockObject( Ctl_GetLabelFont() )) )
		hFont = (HFONT)GetWindowLong( hWnd, GWL_FONT );
	SelectObject( hDC, hFont );
	GetTextMetrics( hDC, &tm );
	y = ( RectHeight(lpRect) - (tm.tmHeight + tm.tmInternalLeading) ) / 2;
	GetWindowText( hWnd, szString, sizeof(szString) );
	if ( bDown ) i = 2; else i = 0;
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


