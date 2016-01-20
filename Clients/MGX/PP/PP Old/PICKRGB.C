// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern HANDLE hInstAstral;
static RGBS PickerRGB;

/***********************************************************************/
BOOL ColorPicker( lpRGB )
/***********************************************************************/
LPRGB lpRGB;
{
int ret;
HWND hWnd, hWndFocus;

hWndFocus = GetFocus();
if ( !(hWnd = GetParent(hWndFocus)) )
	hWnd = hWndFocus;
PickerRGB = *lpRGB;
ret = AstralDlg( NO, hInstAstral, hWnd, IDD_COLORPICKER, DlgColorPickerProc );
if ( ret == IDCANCEL )
	return( FALSE );
*lpRGB = PickerRGB;
SetFocus( hWndFocus );
return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgColorPickerProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
int i;
long Color;
RGBS rgbOld;
HSLS hslOld;
static RGBS rgb;
static HSLS hsl;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	rgb = PickerRGB;
	RGBtoHSL( rgb.red, rgb.green, rgb.blue, &hsl );
// void RGBtoCMYK(color, c, m, y, k, cLut, mLut, yLut, kLut)
	CopyRGB(&rgb, &Color);
	SetWindowLong( GetDlgItem( hDlg, IDC_PICK_PATCH1 ), 0, Color );
	CopyRGB(&rgb, &Color);
	SetWindowLong( GetDlgItem( hDlg, IDC_PICK_PATCH2 ), 0, Color );
	SetScrollBar( hDlg, IDC_PICK_HUELEVEL, TOANGLE(hsl.hue), 0, 360 );
	SetWindowLong( GetDlgItem( hDlg, IDC_PICKER ), 0,
		MAKELONG(hsl.sat,hsl.lum) );
	SetDlgItemInt( hDlg, IDC_PICKER, hsl.hue, NO );
	InitDlgItemSpin( hDlg, IDC_PICK_H, TOANGLE(hsl.hue), NO, 0, 360 );
	InitDlgItemSpin( hDlg, IDC_PICK_S, TOPERCENT(hsl.sat), NO, 0, 100 );
	InitDlgItemSpin( hDlg, IDC_PICK_L, TOPERCENT(hsl.lum), NO, 0, 100 );
	InitDlgItemSpin( hDlg, IDC_PICK_R, TOPERCENT(rgb.red), NO, 0, 100 );
	InitDlgItemSpin( hDlg, IDC_PICK_G, TOPERCENT(rgb.green), NO, 0, 100 );
	InitDlgItemSpin( hDlg, IDC_PICK_B, TOPERCENT(rgb.blue), NO, 0, 100 );
	InitDlgItemSpin( hDlg, IDC_PICK_C, TOPERCENT(255-rgb.red), NO, 0, 100 );
	InitDlgItemSpin( hDlg, IDC_PICK_M, TOPERCENT(255-rgb.green), NO, 0,100);
	InitDlgItemSpin( hDlg, IDC_PICK_Y, TOPERCENT(255-rgb.blue), NO, 0, 100);
	InitDlgItemSpin( hDlg, IDC_PICK_K, TOPERCENT(0), NO, 0, 100 );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	switch ( GetDlgCtrlID( HIWORD( lParam )) )
	    {
	    case IDC_PICK_HUELEVEL:
		rgbOld = rgb;
		hslOld = hsl;
		i = HandleScrollBar( hDlg, IDC_PICK_HUELEVEL,
			wParam, lParam, 0, 360 );
		hsl.hue = FROMANGLE(i);
		HSLtoRGB( hsl.hue, hsl.sat, hsl.lum, &rgb );
		wParam = IDC_PICK_HUELEVEL;
		goto Update;
		break;
	    default:
		HandleMiniScroll( hDlg, wParam, lParam );
	    }
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	rgbOld = rgb;
	hslOld = hsl;
	switch (wParam)
	    {
	    case IDC_PICKER:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		hsl.sat = LOWORD(lParam);
		hsl.lum = HIWORD(lParam);
		HSLtoRGB( hsl.hue, hsl.sat, hsl.lum, &rgb );
//	void RGBtoCMYK(color, c, m, y, k, cLut, mLut, yLut, kLut)
		break;

	    case IDC_PICK_R:
	    case IDC_PICK_G:
	    case IDC_PICK_B:
	    case IDC_PICK_H:
	    case IDC_PICK_S:
	    case IDC_PICK_L:
	    case IDC_PICK_C:
	    case IDC_PICK_M:
	    case IDC_PICK_Y:
	    case IDC_PICK_K:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
//		if ( wParam != GetWindowWord( GetFocus(), GWW_ID ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		if ( wParam == IDC_PICK_H )
			{
			while ( i > 360 ) i -= 360;
			while ( i < 0 )   i += 360;
			hsl.hue = FROMANGLE(i);
			}
		else	{
			if ( i > 100 )
				SetDlgItemSpin( hDlg, wParam, (i = 100), NO );
			i = TOGRAY(i);
			if ( wParam == IDC_PICK_R ) rgb.red   = i; else
			if ( wParam == IDC_PICK_G ) rgb.green = i; else
			if ( wParam == IDC_PICK_B ) rgb.blue  = i; else
			if ( wParam == IDC_PICK_S ) hsl.sat   = i; else
			if ( wParam == IDC_PICK_L ) hsl.lum   = i; else
			if ( wParam == IDC_PICK_C ) rgb.red   = 255-i; else
			if ( wParam == IDC_PICK_M ) rgb.green = 255-i; else
			if ( wParam == IDC_PICK_Y ) rgb.blue  = 255-i;
			}
		if ( wParam == IDC_PICK_H || wParam == IDC_PICK_S ||
		     wParam == IDC_PICK_L )
			HSLtoRGB( hsl.hue, hsl.sat, hsl.lum, &rgb );
		else	RGBtoHSL( rgb.red, rgb.green, rgb.blue, &hsl );
		break;

	    case IDOK:
		PickerRGB = rgb;
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;
	    }

	Update:
	// Check to see if changes will force us to update other controls
	if ( hslOld.hue != hsl.hue )
		{
		if ( wParam != IDC_PICK_H )
			SetDlgItemSpin( hDlg, IDC_PICK_H, TOANGLE(hsl.hue),
				NO );
		if ( wParam != IDC_PICK_HUELEVEL )
			SetScrollBar( hDlg, IDC_PICK_HUELEVEL, TOANGLE(hsl.hue),
				0, 360 );
		if ( wParam != IDC_PICKER )
			{
			SetDlgItemInt( hDlg, IDC_PICKER, hsl.hue, NO );
			AstralControlRepaint( hDlg, IDC_PICKER );
			}
		}
	if ( hslOld.sat != hsl.sat )
		if ( wParam != IDC_PICK_S )
		   SetDlgItemSpin( hDlg, IDC_PICK_S, TOPERCENT(hsl.sat), NO );
	if ( hslOld.lum != hsl.lum )
		if ( wParam != IDC_PICK_L )
		   SetDlgItemSpin( hDlg, IDC_PICK_L, TOPERCENT(hsl.lum), NO );
	if ( hslOld.sat != hsl.sat || hslOld.lum != hsl.lum )
		{ // There was an hsl change
		if ( wParam != IDC_PICKER )
			{
			DrawColorPickerCross( GetDlgItem( hDlg, IDC_PICKER ),
			    GetWindowLong( GetDlgItem(hDlg, IDC_PICKER), 0 ),
			    OFF );
			SetWindowLong( GetDlgItem( hDlg, IDC_PICKER ), 0,
				MAKELONG(hsl.sat,hsl.lum) );
			DrawColorPickerCross( GetDlgItem( hDlg, IDC_PICKER ),
			    GetWindowLong( GetDlgItem(hDlg, IDC_PICKER), 0 ),
			    ON );
			}
		}
	if ( rgbOld.red != rgb.red )
		{
		if ( wParam != IDC_PICK_R )
		   SetDlgItemSpin( hDlg, IDC_PICK_R, TOPERCENT(rgb.red), NO );
		if ( wParam != IDC_PICK_C )
		   SetDlgItemSpin( hDlg, IDC_PICK_C, TOPERCENT(255-rgb.red),NO);
		}
	if ( rgbOld.green != rgb.green )
		{
		if ( wParam != IDC_PICK_G )
		   SetDlgItemSpin( hDlg, IDC_PICK_G, TOPERCENT(rgb.green), NO );
		if ( wParam != IDC_PICK_M )
		   SetDlgItemSpin( hDlg, IDC_PICK_M,TOPERCENT(255-rgb.green),0);
		}
	if ( rgbOld.blue != rgb.blue )
		{
		if ( wParam != IDC_PICK_B )
		    SetDlgItemSpin( hDlg, IDC_PICK_B, TOPERCENT(rgb.blue), NO );
		if ( wParam != IDC_PICK_Y )
		    SetDlgItemSpin( hDlg, IDC_PICK_Y,TOPERCENT(255-rgb.blue),0);
		}
	if ( rgb.red   != rgbOld.red   ||
	     rgb.green != rgbOld.green ||
	     rgb.blue  != rgbOld.blue )
		{ // There was an rgb change
		CopyRGB(&rgb, &Color);
		SetWindowLong( GetDlgItem( hDlg, IDC_PICK_PATCH1 ), 0, Color );
		AstralControlRepaint( hDlg, IDC_PICK_PATCH1 );
		}
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
long FAR PASCAL PickerControl( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND	   hWindow;
unsigned   message;
WORD	   wParam;
long	   lParam;
{
RECT ClientRect;
PAINTSTRUCT ps;
HDC hDC;
int Hue, x, y, dx, dy;
BOOL Bool, bHasFocus;
long lValue;
static long lLastValue;
static BOOL bTrack;

switch ( message )
    {
    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	GetClientRect( hWindow, &ClientRect );
	EraseControlBackground( wParam /*hDC*/, hWindow, &ClientRect,
		CTLCOLOR_STATIC );
	break;

    case WM_PAINT:
	if ( LBUTTON ) // Ignore this message if the button is down
		return( FALSE );
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );

	bHasFocus = ( GetFocus() == hWindow );

	// Draw the focus rectangle
	FrameRect( hDC, &ClientRect,
		(bHasFocus ? Window.hBlackBrush : Window.hWhiteBrush) );
	InflateRect( &ClientRect, -1, -1 );

	// Draw the box frame
	FrameRect( hDC, &ClientRect, Window.hBlackBrush );
	InflateRect( &ClientRect, -1, -1 );

	// Draw the color picker
	Hue = GetDlgItemInt( GetParent(hWindow),
		GetWindowWord(hWindow, GWW_ID), &Bool, NO );
	DrawColorPicker( hDC, &ClientRect, Hue, &ps.rcPaint );
	DrawColorPickerCross( hWindow, GetWindowLong( hWindow, 0 ), ON );
	EndPaint( hWindow, &ps );
	break;

    case WM_LBUTTONDOWN:
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	lLastValue = GetWindowLong( hWindow, 0 );
	GetClientRect( hWindow, &ClientRect );
	InflateRect(&ClientRect, -2, -2);
	ClientRect.bottom -= 1;
	ClientRect.right -= 1;
	y = (int)HIWORD(lParam) - ClientRect.top;
	dy = ClientRect.bottom - ClientRect.top;
	y = FMUL( 255, FGET( y, dy ) );
	x = (int)LOWORD(lParam) - ClientRect.left;
	dx = ClientRect.right - ClientRect.left;
	x = FMUL( 255, FGET( x, dx ) );
	y = bound(y, 0, 255);
	x = bound(x, 0, 255);
	lValue = MAKELONG( x, y );
	SetWindowLong( hWindow, 0, lValue );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), lValue );
	break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
	DrawColorPickerCross( hWindow, lLastValue, OFF );
	DrawColorPickerCross( hWindow, GetWindowLong( hWindow, 0 ), ON );
	break;

    case WM_MOUSEMOVE:
	if ( !bTrack )
//		{
//		MessageStatus( GetWindowWord(hWindow, GWW_ID) );
		break;
//		}
	GetClientRect( hWindow, &ClientRect );
	InflateRect(&ClientRect, -2, -2);
	ClientRect.bottom -= 1;
	ClientRect.right -= 1;
	y = (int)HIWORD(lParam) - ClientRect.top;
	dy = ClientRect.bottom - ClientRect.top;
	y = FMUL( 255, FGET( y, dy ) );
	x = (int)LOWORD(lParam) - ClientRect.left;
	dx = ClientRect.right - ClientRect.left;
	x = FMUL( 255, FGET( x, dx ) );
	y = bound(y, 0, 255);
	x = bound(x, 0, 255);
	lValue = MAKELONG( x, y );
	SetWindowLong( hWindow, 0, lValue );
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GetWindowWord(hWindow, GWW_ID), lValue );
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
		break;
	// Draw the focus rectangle
	hDC = GetDC( hWindow );
	GetClientRect( hWindow, &ClientRect );
	bHasFocus = ( message == WM_SETFOCUS );
	FrameRect( hDC, &ClientRect,
		(bHasFocus ? Window.hBlackBrush : Window.hWhiteBrush) );
	ReleaseDC( hWindow, hDC );
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
void DrawColorPicker( hDC, lpRect, Hue, lpRepairRect )
/***********************************************************************/
HDC hDC;
LPRECT lpRect;
int Hue;
LPRECT lpRepairRect;
{
int x, y, dx, dy;
BYTE lum, sat;
FIXED xScale, yScale;
LPRGB lpRGB, lpRGBWork;
RECT rRepair;

if (lpRepairRect)
    rRepair = *lpRepairRect;
else
    rRepair = *lpRect;
lpRect->right--;
lpRect->bottom--;
IntersectRect(&rRepair, lpRect, &rRepair);

if ( !(lpRGB = (LPRGB)Alloc((long)RectWidth(&rRepair)*3L)) )
    return;

dx = RectWidth( lpRect );
dy = RectHeight( lpRect );
StartSuperBlt( hDC, &BltScreen, &rRepair, 3 /*depth*/, 10, 0, 0 );
lpRect->right++;
lpRect->bottom++;

xScale = FGET( 255, dx-1 );
yScale = FGET( 255, dy-1 );
for ( y=rRepair.top; y<=rRepair.bottom; y++ )
	{
	lpRGBWork = lpRGB;
	lum = FMUL( y-lpRect->top, yScale );
	for ( x=rRepair.left; x<=rRepair.right; x++, lpRGBWork++ )
		{
		sat = FMUL( x-lpRect->left, xScale );
		HSLtoRGB( (BYTE)Hue, sat, lum, lpRGBWork );
		}
	SuperBlt( (LPTR)(lpRGB) );
	}
SuperBlt( NULL );
FreeUp((LPTR)lpRGB);
}


/***********************************************************************/
void DrawColorPickerCross( hWindow, lValue, on )
/***********************************************************************/
HWND hWindow;
long lValue;
BOOL on;
{
int x, y, ldelta, dx, dy, sdelta;
BOOL Bool;
RECT ClientRect, rRepair;
HPEN hOldPen;
HDC hDC;

hDC = GetDC(hWindow);
GetClientRect( hWindow, &ClientRect );
InflateRect(&ClientRect, -2, -2);
ClientRect.bottom -= 1;
ClientRect.right -= 1;
dy = ClientRect.bottom - ClientRect.top;
dx = ClientRect.right - ClientRect.left;
y = FMUL( (int)HIWORD(lValue), FGET( dy, 255 ) ) + ClientRect.top;
x = FMUL( (int)LOWORD(lValue), FGET( dx, 255 ) ) + ClientRect.left;
ldelta = 7;
sdelta = 1;
if (on)
	{
	hOldPen = SelectObject(hDC, Window.hBlackPen);

	// Draw the horizontal dash
	MoveTo( hDC, x-ldelta, y-1 );
	LineTo( hDC, x-sdelta, y-1 );
	MoveTo( hDC, x-ldelta, y );
	LineTo( hDC, x-sdelta, y );
	MoveTo( hDC, x-ldelta, y+1 );
	LineTo( hDC, x-sdelta, y+1 );

	MoveTo( hDC, x+ldelta, y-1 );
	LineTo( hDC, x+sdelta, y-1 );
	MoveTo( hDC, x+ldelta, y );
	LineTo( hDC, x+sdelta, y );
	MoveTo( hDC, x+ldelta, y+1 );
	LineTo( hDC, x+sdelta, y+1 );

	// Draw the vertical dash
	MoveTo( hDC, x-1, y-ldelta );
	LineTo( hDC, x-1, y-sdelta );
	MoveTo( hDC, x,   y-ldelta );
	LineTo( hDC, x,   y-sdelta );
	MoveTo( hDC, x+1, y-ldelta );
	LineTo( hDC, x+1, y-sdelta );

	MoveTo( hDC, x-1, y+ldelta );
	LineTo( hDC, x-1, y+sdelta );
	MoveTo( hDC, x,   y+ldelta );
	LineTo( hDC, x,   y+sdelta );
	MoveTo( hDC, x+1, y+ldelta );
	LineTo( hDC, x+1, y+sdelta );

	SelectObject(hDC, hOldPen);
	}
else	{
	rRepair.top = y - ldelta;
	rRepair.bottom = y + ldelta + 1;
	rRepair.left = x - ldelta;
	rRepair.right = x + ldelta + 1;
	InvalidateRect(hWindow, &rRepair, FALSE);
	UpdateWindow( hWindow );
	}

ReleaseDC(hWindow, hDC);
}
