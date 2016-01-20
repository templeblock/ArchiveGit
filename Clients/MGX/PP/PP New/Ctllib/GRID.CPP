//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"
#include "grid.h"
#include "superblt.h"
#include "macros.h"

#define GWL_GRIDLINES	0
#define WINDOW_EXTRA	sizeof( LONG )

#ifdef DLGEDIT
void Control_GetClassInfo(LPTSTR lpClassName, LPWORD lpWidth, LPWORD lpHeight, LPDWORD lpStyle)
{
lstrcpy(lpClassName, MGXCTL_SPIN);
*lpWidth = 256;
*lpHeight = 256;
*lpStyle = 0;
}
BOOL Control_RegisterClass(HINSTANCE hInstance, UINT ClassScope)
{
return(Grid_RegisterClass(hInstance, ClassScope));
}
#define Grid_Control DlgEdit_Control
extern "C"{
LONG WINPROC Grid_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
}
#else
LONG WINPROC Grid_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
#endif

/***********************************************************************/
/***********************************************************************/
// GRID CONTROL
/***********************************************************************/
/***********************************************************************/
LOCAL void Grid_OnDestroy(HWND hWindow);
LOCAL void Grid_OnPaint(HWND hWindow);
LOCAL void Grid_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Grid_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Grid_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void Grid_Draw( HWND hWindow, HDC hDC );
LOCAL  void Grid_DrawFocus( HWND hWindow, HDC hDC );
LOCAL  void Grid_GetRect( HWND hWindow, int r, int c, LPRECT lpRect);

static BOOL Grid_bTrack;

BOOL Grid_RegisterClass (
HINSTANCE hInstance, UINT ClassScope)
{
WNDCLASS WndClass;

/* Setup and define an window class for an image window */
WndClass.hIcon			= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= MGXCTL_GRID;
WndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance		= hInstance;
WndClass.style			= ClassScope | CS_DBLCLKS;
WndClass.lpfnWndProc	= Grid_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= WINDOW_EXTRA;

return(RegisterClass( &WndClass ));
}

BOOL Grid_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)MGXCTL_GRID, hInstance));
}

/***********************************************************************/
#ifdef DLGEDIT
extern "C" {
#endif
LONG WINPROC EXPORT Grid_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_PALETTECHANGED, Control_OnPaletteChanged);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);

	HANDLE_MSG(hWindow, WM_PAINT, Grid_OnPaint);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Grid_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Grid_OnKillFocus);
	HANDLE_MSG(hWindow, WM_DESTROY, Grid_OnDestroy);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}
#ifdef DLGEDIT
}
#endif

/***********************************************************************/
LOCAL void Grid_OnDestroy(HWND hWindow)
/***********************************************************************/
{
if ( !Grid_bTrack )
	return;
ReleaseCapture();
Grid_bTrack = NO;
}

/***********************************************************************/
LOCAL void Grid_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
HDC hDC;

hDC = BeginPaint( hWindow, &ps );
Grid_Draw( hWindow, hDC );
Grid_DrawFocus( hWindow, hDC);
EndPaint(hWindow, &ps);
}

/***********************************************************************/
LOCAL void Grid_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
Grid_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
LOCAL void Grid_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
Grid_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
LOCAL void Grid_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
HDC hDC;

// Draw the focus rectangle
hDC = GetDC( hWindow );
Grid_DrawFocus(hWindow, hDC);
ReleaseDC( hWindow, hDC );
}


/***********************************************************************/
LOCAL  void Grid_DrawFocus( HWND hWindow, HDC hDC )
/***********************************************************************/
{
RECT ClientRect;
BOOL fSetFocus;

if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
	return;
fSetFocus = GetFocus() == hWindow;
GetClientRect( hWindow, &ClientRect );
InflateRect( &ClientRect, -1, -1);
FrameRect( hDC, &ClientRect,
		(HBRUSH)GetStockObject( fSetFocus ? BLACK_BRUSH : WHITE_BRUSH ) );
}

/***********************************************************************/
LOCAL  void Grid_Draw( HWND hWindow, HDC hDC )
/***********************************************************************/
{
RECT  	ClientRect;
int   	iWidth, iHeight, i, x, y;
POINT		rPoint;
LFIXED	xrate, yrate;
TCHAR		lpString[32];
LONG		nLines;
COLORREF	LightGray;
HPEN		hPen;
HGDIOBJ	hOldObject;

// get number of grid lines.
SendMessage( hWindow, WM_GETTEXT, ( WPARAM )sizeof( lpString ),
	( LPARAM )&lpString );
if( !( nLines = Ascii2Int( lpString ) ) )
	return;

// fill background with white
GetClientRect(hWindow, &ClientRect);
FillRect( hDC, &ClientRect, (HBRUSH)GetStockObject(WHITE_BRUSH) );

// frame the grid
FrameRect(hDC, &ClientRect, (HBRUSH)GetStockObject(BLACK_BRUSH) );

iWidth = RectWidth(&ClientRect);
iHeight = RectHeight(&ClientRect);

xrate = FGET( iWidth, nLines );
yrate = FGET( iHeight, nLines );

LightGray = RGB( 128, 128, 128 );

hPen = CreatePen( PS_SOLID, 1, LightGray );
hOldObject = SelectObject( hDC, hPen );

for( i = 1, x = FMUL( i, xrate ); x < iWidth;
	x = FMUL( i, xrate ), i++ )
	{
	MoveToEx( hDC, x, ClientRect.top, &rPoint );
	LineTo( hDC, x, ClientRect.bottom );
	}

for( i = 1, y = FMUL( i, yrate ); y < iHeight;
	y = FMUL( i, yrate ), i++ )
	{
	MoveToEx( hDC, ClientRect.left, y, &rPoint );
	LineTo( hDC, ClientRect.right, y );
	}
SelectObject( hDC, hOldObject );
DeleteObject( hPen );

}

/***********************************************************************/
LOCAL  void Grid_GetRect( HWND hWindow, int r, int c, LPRECT lpRect)
/***********************************************************************/
{
RECT ClientRect;
int iWidth, iHeight;
LFIXED xrate, yrate;

GetClientRect(hWindow, &ClientRect);
InflateRect(&ClientRect, -3, -3);
WindowsToAstralRect(&ClientRect);

iWidth = RectWidth(&ClientRect);
iHeight = RectHeight(&ClientRect);

yrate = FGET(iHeight, 16);
xrate = FGET(iWidth, 16);

lpRect->left = FMUL(c, xrate) + 1;
lpRect->right = FMUL(c+1, xrate) - 2;
lpRect->top = FMUL(r, yrate) + 1;
lpRect->bottom = FMUL(r+1, yrate) - 2;
OffsetRect(lpRect, ClientRect.left, ClientRect.top);
}
