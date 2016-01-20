// (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"
#include "stext.h"
#include "stext.rh"
#include "ctltools.h"

LONG WINPROC EXPORT StaticText_Control(	HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
LOCAL void DrawLOCALBox( HDC hDC, LPRECT lpRect, BOOL bRaised, BOOL bDouble );

/* Static Text Control */
#define STATICTEXT_EXTRA		GWW_START

BOOL StaticText_RegisterClass (HINSTANCE hInstance, UINT ClassScope)
{
	WNDCLASS WndClass;

	/* Setup and define the window class labeled frame control */
	/* Setup and define the window class for a joystick */
	WndClass.hIcon		= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPTSTR)_T("stext");
	WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
	WndClass.hbrBackground	= NULL;
	WndClass.hInstance	= hInstance;
	WndClass.style		= CS_DBLCLKS | ClassScope;
	WndClass.lpfnWndProc	= StaticText_Control;
	WndClass.cbClsExtra	= 0;
	WndClass.cbWndExtra	= STATICTEXT_EXTRA;

	return(RegisterClass( &WndClass ));
}

BOOL StaticText_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)_T("stext"), hInstance));
}


/***********************************************************************/
/***********************************************************************/
// STATICTEXT CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL void StaticText_OnPaint(HWND hWindow);
LOCAL UINT StaticText_OnGetDlgCode(HWND hWindow, LPMSG lpMsg);

#define IS_STYLE(dwStyle, theStyle) (((dwStyle) & 0x0FL) == (theStyle))

/***********************************************************************/
LONG WINPROC EXPORT StaticText_Control(	HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_ENABLE, Control_OnEnable);
	HANDLE_MSG(hWindow, WM_SETTEXT, Control_OnSetText);
	HANDLE_MSG(hWindow, WM_SETFONT, Control_OnSetFont);

	HANDLE_MSG(hWindow, WM_PAINT, StaticText_OnPaint);
	HANDLE_MSG(hWindow, WM_GETDLGCODE, StaticText_OnGetDlgCode);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL void StaticText_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT PS;
HDC         hDC;
DWORD       dwStyle;
WORD        wFormat;
HGDIOBJ     hFont, hOldFont;
RECT        ClientRect;						 
STRING      szString;
int         nOldBkMode;

if ( !(hDC = BeginPaint (hWindow,&PS)) )
	return;

dwStyle = GetWindowLong( hWindow, GWL_STYLE );

GetWindowText(hWindow, szString, MAX_STR_LEN);

hOldFont = NULL;
if (dwStyle & SS_THINFONT)
	hOldFont = SelectObject(hDC, GetStockObject( Ctl_GetLabelFont() ) );
else
	{
	hFont = (HFONT)GetWindowLong(hWindow, GWL_FONT);
	if (hFont)
		hOldFont = SelectObject(hDC, hFont );
	}
	

GetClientRect(hWindow, &ClientRect);
Ctl_EraseBackground(hWindow, hDC, &ClientRect, CTLCOLOR_STATIC );

if (lstrlen(szString) > 0)
	{
	nOldBkMode = SetBkMode(hDC, TRANSPARENT);
	if (IS_STYLE(dwStyle, SS_RIGHT))
		wFormat = DT_RIGHT;
	else
	if (IS_STYLE(dwStyle, SS_CENTER))
		wFormat = DT_CENTER;
	else
		wFormat = DT_LEFT;
	if (dwStyle & SS_VCENTER)
		wFormat |= DT_VCENTER;
	wFormat |= DT_SINGLELINE;
	DrawText(hDC, szString, -1, &ClientRect, wFormat | DT_WORDBREAK );
	SetBkMode(hDC, nOldBkMode);
	}

if ( hOldFont )
	SelectObject(hDC, hOldFont);

if (dwStyle & SS_THINBEVEL)
	DrawLOCALBox( hDC, &ClientRect, YES, NO );

EndPaint (hWindow,&PS);
}

/***********************************************************************/
LOCAL UINT StaticText_OnGetDlgCode(HWND hWindow, LPMSG lpMsg)
/***********************************************************************/
{
return( DLGC_STATIC );
}

/***********************************************************************/
LOCAL void DrawLOCALBox( HDC hDC, LPRECT lpRect, BOOL bRaised, BOOL bDouble )
/***********************************************************************/
{
HGDIOBJ  hOldPen, hPen1, hPen2;
RECT     Rect;
POINT    p;

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

if ( bRaised )
	{ hPen1 = DrawTools.GetBtnHilightPen(); hPen2 = DrawTools.GetBtnShadowPen(); }
else	{ hPen1 = DrawTools.GetBtnShadowPen(); hPen2 = DrawTools.GetBtnHilightPen(); }

// Draw the upper left border
hOldPen = SelectObject( hDC, hPen1 );
MoveToEx( hDC, Rect.left, Rect.bottom, &p);
LineTo( hDC, Rect.left, Rect.top );
LineTo( hDC, Rect.right, Rect.top );
// Draw the drop shadow
SelectObject( hDC, hPen2 );
LineTo( hDC, Rect.right, Rect.bottom );
LineTo( hDC, Rect.left, Rect.bottom );
InflateRect( &Rect, -1, -1 );

if ( bDouble )
	{
	// Draw the upper left border
	SelectObject( hDC, hPen1 );
	MoveToEx( hDC, Rect.left, Rect.bottom, &p);
	LineTo( hDC, Rect.left, Rect.top );
	LineTo( hDC, Rect.right, Rect.top );
	// Draw the drop shadow
	SelectObject( hDC, hPen2 );
	LineTo( hDC, Rect.right, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.bottom );
	InflateRect( &Rect, -1, -1 );
	}

SelectObject( hDC, hOldPen );
}
