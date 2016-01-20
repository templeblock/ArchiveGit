//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "ctlutil.h"
#include "picker.h"
#include "macros.h"

#define GWL_COLORMAP 	0
#define GWW_PALINDEX	(GWL_COLORMAP+sizeof(long))
#define WINDOW_EXTRA	(GWW_PALINDEX+sizeof(WORD))

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
return(PalPicker_RegisterClass(hInstance, ClassScope));
}
#define PalPicker_Control DlgEdit_Control
extern "C"{
LONG WINPROC PalPicker_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
}
#else
LONG WINPROC PalPicker_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam );
#endif

/***********************************************************************/
/***********************************************************************/
// PALPICKER CONTROL
/***********************************************************************/
/***********************************************************************/
LOCAL void PalPicker_OnDestroy(HWND hWindow);
LOCAL void PalPicker_OnPaint(HWND hWindow);
LOCAL void PalPicker_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void PalPicker_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void PalPicker_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void PalPicker_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void PalPicker_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void PalPicker_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void PalPicker_OnKey(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void PalPicker_Draw( HWND hWindow, HDC hDC );
LOCAL  void PalPicker_DrawPatch( HDC hDC, LPRECT lpRect,
								BYTE value, FRMTYPEINFO TypeInfo, LPTR lpBuf );
LOCAL  void PalPicker_DrawFocus( HWND hWindow, HDC hDC );
LOCAL  void PalPicker_GetRect( HWND hWindow, int r, int c, LPRECT lpRect);
LOCAL  void PalPicker_Select( HWND hWindow, HDC hDC, int iSelect, BOOL on );
LOCAL  int PalPicker_GetSelect( HWND hWindow, int x, int y );
LOCAL  void PalPicker_NewSelect( HWND hWindow, int iOldSelect, int iNewSelect );

static BOOL PalPicker_bTrack;

/***********************************************************************/
void PalPicker_Init(HWND hWindow, LPCOLORMAP lpColorMap, int iSelect)
/***********************************************************************/
{
int r, c;
RECT SelectRect;

SetWindowLong(hWindow, GWL_COLORMAP, (long)lpColorMap);
SetWindowWord(hWindow, GWW_PALINDEX, iSelect);
if (lpColorMap)
	{
	InvalidateRect(hWindow, NULL, TRUE);
	}
else
	{
	r = iSelect / 16;
	c = iSelect % 16;
	PalPicker_GetRect(hWindow, r, c, &SelectRect);
	InvalidateRect(hWindow, &SelectRect, TRUE);
	}
}

/***********************************************************************/
LPCOLORMAP PalPicker_GetColorMap(HWND hWindow)
/***********************************************************************/
{
LPCOLORMAP lpColorMap;

lpColorMap = (LPCOLORMAP)GetWindowLong(hWindow, GWL_COLORMAP);
return(lpColorMap);
}

/***********************************************************************/
int PalPicker_GetIndex(HWND hWindow)
/***********************************************************************/
{
return(GetWindowWord(hWindow, GWW_PALINDEX));
}

BOOL PalPicker_RegisterClass (
HINSTANCE hInstance, UINT ClassScope)
{
WNDCLASS WndClass;

/* Setup and define an window class for an image window */
WndClass.hIcon			= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= MGXCTL_PALPICKER;
WndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance		= hInstance;
WndClass.style			= ClassScope | CS_DBLCLKS;
WndClass.lpfnWndProc	= PalPicker_Control;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= WINDOW_EXTRA;

return(RegisterClass( &WndClass ));
}

BOOL PalPicker_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)MGXCTL_PALPICKER, hInstance));
}

/***********************************************************************/
#ifdef DLGEDIT
extern "C" {
#endif
LONG WINPROC EXPORT PalPicker_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_PALETTECHANGED, Control_OnPaletteChanged);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);

	HANDLE_MSG(hWindow, WM_PAINT, PalPicker_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, PalPicker_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, PalPicker_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, PalPicker_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, PalPicker_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SETFOCUS, PalPicker_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, PalPicker_OnKillFocus);
	HANDLE_MSG(hWindow, WM_DESTROY, PalPicker_OnDestroy);
	HANDLE_MSG(hWindow, WM_KEYDOWN, PalPicker_OnKey);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}
#ifdef DLGEDIT
}
#endif

/***********************************************************************/
LOCAL void PalPicker_OnDestroy(HWND hWindow)
/***********************************************************************/
{
if ( !PalPicker_bTrack )
	return;
ReleaseCapture();
PalPicker_bTrack = NO;
}

/***********************************************************************/
LOCAL void PalPicker_OnPaint(HWND hWindow)
/***********************************************************************/
{
PAINTSTRUCT ps;
HDC hDC;

hDC = BeginPaint( hWindow, &ps );
PalPicker_Draw( hWindow, hDC );
PalPicker_DrawFocus( hWindow, hDC);
PalPicker_Select(hWindow, hDC, GetWindowWord(hWindow, GWW_PALINDEX), ON);
EndPaint(hWindow, &ps);
}

/***********************************************************************/
LOCAL void PalPicker_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
RECT ClientRect;
int iSelect, iOldSelect;

if ( GetFocus() != hWindow )
	SetFocus( hWindow );

iSelect = PalPicker_GetSelect(hWindow, x, y);
if (iSelect < 0)
	return;

if (fDoubleClick)
	{
	FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow),
				hWindow, fDoubleClick, SendMessage );
	return;
	}

if ( PalPicker_bTrack )
	return;

SetCapture( hWindow ); PalPicker_bTrack = TRUE;

GetWindowRect( hWindow, &ClientRect );
ClipCursor( &ClientRect );

iOldSelect = GetWindowWord(hWindow, GWW_PALINDEX);
if (iSelect != iOldSelect)
	{
	PalPicker_NewSelect(hWindow, iOldSelect, iSelect);
	FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow),
				hWindow, fDoubleClick, SendMessage );
	}
}

/***********************************************************************/
LOCAL void PalPicker_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( !PalPicker_bTrack )
	return;
ReleaseCapture(); PalPicker_bTrack = FALSE;
ClipCursor( NULL );
}

/***********************************************************************/
LOCAL void PalPicker_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
int iSelect, iOldSelect;

if ( !PalPicker_bTrack )
	return;

iSelect = PalPicker_GetSelect(hWindow, x, y);
if (iSelect < 0)
	return;
iOldSelect = GetWindowWord(hWindow, GWW_PALINDEX);
if (iSelect != iOldSelect)
	{
	PalPicker_NewSelect(hWindow, iOldSelect, iSelect);
	FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow),
				hWindow, 0, SendMessage );
	}
}

/***********************************************************************/
LOCAL void PalPicker_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
PalPicker_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
LOCAL void PalPicker_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
PalPicker_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
LOCAL void PalPicker_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
HDC hDC;

// Draw the focus rectangle
hDC = GetDC( hWindow );
PalPicker_DrawFocus(hWindow, hDC);
ReleaseDC( hWindow, hDC );
}

/***********************************************************************/
LOCAL void PalPicker_OnKey(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
LPCOLORMAP lpColorMap;
int iCurrent, r, c, iSelect, n;

if (vk == VK_UP || vk == VK_DOWN ||
    vk == VK_LEFT || vk == VK_RIGHT ||
    vk == VK_RETURN)
	{
	// get info all keystrokes need
	lpColorMap = PalPicker_GetColorMap(hWindow);
	if (lpColorMap)
		n = lpColorMap->NumEntries;
	else
		n = 256;

	iCurrent = (int)GetWindowWord(hWindow, GWW_PALINDEX);
	r = iCurrent / 16;
	c = iCurrent % 16;
	iSelect = iCurrent;
	}

switch (vk)
	{
	case VK_UP:
		--r;
		if (r < 0)
			break;
		iSelect = (r * 16) + c;		
		break;

	case VK_DOWN:
		++r;
		if (r >= 16)
			break;
		iSelect = (r * 16) + c;		
		break;

	case VK_LEFT:
		--c;
		if (c < 0)
			break;
		iSelect = (r * 16) + c;		
		break;

	case VK_RIGHT:
		++c;
		if (c >= 16)
			break;
		iSelect = (r * 16) + c;		
		break;

	case VK_RETURN:
		FORWARD_WM_COMMAND(GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, TRUE, SendMessage);
		break;
	default:
		return;
	}
if (iSelect != iCurrent)
	{
	PalPicker_NewSelect( hWindow, iCurrent, iSelect );
	FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow),
				hWindow, 0, SendMessage );
	}
}

/***********************************************************************/
LOCAL  void PalPicker_DrawFocus( HWND hWindow, HDC hDC )
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
LOCAL  void PalPicker_NewSelect( HWND hWindow, int iOldSelect, int iNewSelect )
/***********************************************************************/
{
HDC hDC;

hDC = GetDC(hWindow);
PalPicker_Select(hWindow, hDC, iOldSelect, OFF);
PalPicker_Select(hWindow, hDC, iNewSelect, ON);
ReleaseDC(hWindow, hDC);
SetWindowWord(hWindow, GWW_PALINDEX, iNewSelect);
}

/***********************************************************************/
LOCAL  void PalPicker_Select( HWND hWindow, HDC hDC, int iSelect, BOOL on )
/***********************************************************************/
{
RECT SelectRect;
int x1, x2, y1, y2, r, c;
HPEN hOldPen;
POINT p;

r = iSelect / 16;
c = iSelect % 16;
PalPicker_GetRect(hWindow, r, c, &SelectRect);

InflateRect(&SelectRect, 2, 2);
y1 = SelectRect.top;
y2 = SelectRect.bottom;
x1 = SelectRect.left;
x2 = SelectRect.right;

if (on)
	hOldPen = (HPEN)SelectObject( hDC, GetStockObject(BLACK_PEN) );
else
	hOldPen = (HPEN)SelectObject( hDC, GetStockObject(WHITE_PEN) );
MoveToEx(hDC, x1, y1, &p);
LineTo(hDC, x2, y1);
LineTo(hDC, x2, y2);
LineTo(hDC, x1, y2);
LineTo(hDC, x1, y1);

//OldROP = SetROP2(hDC, R2_NOT);
MoveToEx(hDC, x1+1, y1+1, &p);
LineTo(hDC, x2-1, y1+1);
LineTo(hDC, x2-1, y2-1);
LineTo(hDC, x1+1, y2-1);
LineTo(hDC, x1+1, y1+1);
//SetROP2(hDC, OldROP);

SelectObject(hDC, hOldPen);
}

/***********************************************************************/
LOCAL  int PalPicker_GetSelect( HWND hWindow, int x, int y )
/***********************************************************************/
{
int r, c, i, n;
LPCOLORMAP lpColorMap;
POINT pt;
RECT rSelect;

lpColorMap = PalPicker_GetColorMap(hWindow);
if (lpColorMap)
	n = lpColorMap->NumEntries;
else
	n = 256;
pt.x = x;
pt.y = y;
for (i = 0; i < n; ++i)
	{
	r = i / 16;
	c = i % 16;
	PalPicker_GetRect(hWindow, r, c, &rSelect);
	if (PtInRect(&rSelect, pt))
		return(i);
	}
return(-1);
}

/***********************************************************************/
LOCAL  void PalPicker_Draw( HWND hWindow, HDC hDC )
/***********************************************************************/
{
RECT ClientRect, rPatch;
LPCOLORMAP lpColorMap;
LPTR lpData;
FRMTYPEINFO TypeInfo;
int iWidth, iHeight, c, r, n;
BYTE value;

lpColorMap = PalPicker_GetColorMap(hWindow);
if (lpColorMap)
	n = lpColorMap->NumEntries;
else
	n = 256;

// fill background with white
GetClientRect(hWindow, &ClientRect);
FillRect( hDC, &ClientRect, (HBRUSH)GetStockObject(WHITE_BRUSH) );

// frame the picker
FrameRect(hDC, &ClientRect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
InflateRect(&ClientRect, -3, -3);

WindowsToAstralRect(&ClientRect);

iWidth = RectWidth(&ClientRect);
iHeight = RectHeight(&ClientRect);

lpData = Alloc((long)iWidth);
if (!lpData)
	return;

if (lpColorMap)
	FrameSetTypeInfo(&TypeInfo, FDT_PALETTECOLOR, lpColorMap);
else
	FrameSetTypeInfo(&TypeInfo, FDT_GRAYSCALE, NULL);

value = 0;
for (r = 0; r < 16; ++r)
	{
	for (c = 0; c < 16; ++c)
		{
		PalPicker_GetRect(hWindow, r, c, &rPatch);
		PalPicker_DrawPatch(hDC, &rPatch, value, TypeInfo, lpData);
		++value;
		if (value >= n)
			break;
		}
	if (value >= n)
		break;
	}
FreeUp(lpData);
}

/***********************************************************************/
LOCAL  void PalPicker_GetRect( HWND hWindow, int r, int c, LPRECT lpRect)
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

/***********************************************************************/
LOCAL  void PalPicker_DrawPatch( HDC hDC, LPRECT lpRect,
								BYTE value, FRMTYPEINFO TypeInfo, LPTR lpBuf )
/***********************************************************************/
{
BLTSESSION BltSession;
int dx, dy;

dx = RectWidth(lpRect);
dy = RectHeight(lpRect);
set(lpBuf, dx, value);
StartSuperBlt( &BltSession, hDC, NULL, Ctl_GetBltScreen(), lpRect, TypeInfo,
	10, 0, 0, YES, NULL, NULL );
while (--dy >= 0)
	SuperBlt(&BltSession, lpBuf);
SuperBlt(&BltSession, NULL);
}

