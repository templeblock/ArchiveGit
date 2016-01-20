//®PL1¯ (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern LPBLTPROCESS ImgPixelProc;


// Note to all GDI programmers:
// GetClientRect() returns a rectangle whose rightmost and bottom most pixels
// cannot be drawn into.  FrameRect() and FillRect() compensate for this
// problem automatically, but MoveToEx() and Lineto() do not (and cannot)
// Use GetDrawClientRect if you want the actual rectangle to draw in
// and you plan to use MoveToEx() LineTo instead of FrameRect() and FillRect()
// ....WHAT A PAIN!!!

// Static prototypes
static LPFRAME GetImageFrame( HWND hWnd, LPINT lpbMustClose );
//static LPOBJECT GetImageObject( HWND hWnd );
static void ConstrainRatio( LPINT w, LPINT h, int width, int height );
static void MakeRatio( LPINT w, LPINT h, int width, int height );
static long HandleNonClientHit( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
static void DrawIconControl(HDC hDC, HWND hWnd, LPRECT lpRect, BOOL bSelected,
		BOOL bTrackInRect );
static void DrawObjListControl( HDC hDC, HWND hWnd, HBITMAP hBitmapOut, LPOBJECT lpBase,
						LPRECT lpDestRect, int dx, int dy, BOOL bShadows );
static void ResizeDIBControl( HWND hControl );
static void DrawBoxFrame( HDC hDC, LPRECT lpRect );

extern	HINSTANCE	hInstAstral;
extern	HWND		hWndAstral;
extern	HINSTANCE	hCurrentDLL;
extern	BOOL		bPaintAppActive;
static	LRESULT		lReturn;

/***********************************************************************/
long WINPROC EXPORT IconControl(
/***********************************************************************/
HWND	  hWindow,
unsigned  message,
WPARAM    wParam,
LPARAM    lParam)
{
RECT ClientRect;
PAINTSTRUCT ps;
HDC hDC;
BOOL bTrack, bInRect, bSelected, bDown, bHasFocus;

switch ( message )
    {
    case WM_SETFONT:
	SetWindowWord(hWindow, GWW_FONT, wParam);
	if (lParam)
		InvalidateRect(hWindow, NULL, TRUE);
	break;

    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_PAINT:
	if ( SuperPaint( hWindow ) )
		break; // Printing in progress... (SuperBlt not reentrant)
	hDC = BeginPaint( hWindow, &ps );
	GetDrawClientRect( hWindow, &ClientRect );
	bSelected = GetWindowWord(hWindow, GWW_STATE);
	bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
	bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
	DrawIconControl(hDC, hWindow, &ClientRect, bSelected, bTrack && bInRect );
	EndPaint( hWindow, &ps );
	break;

    case WM_KEYDOWN:
	if ( StyleOn( hWindow, WS_NOTENABLED ) )
		break;
	if ( wParam != VK_SPACE )
		break;

    case WM_LBUTTONDOWN:
	if ( StyleOn( hWindow, WS_NOTENABLED ) )
		break;
	if (IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK))
		break;
	SetCapture( hWindow ); SetWordBit(hWindow, GWW_STATUS, ST_TRACK, ON);
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	SetWordBit(hWindow, GWW_STATUS, ST_INRECT, ON);
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	SoundStartResource( "icons", NO/*bLoop*/, NULL/*hInstance*/ );
	if ( StyleOn( hWindow, WS_TOOL ) )
		goto UP;
	break;

    case WM_KEYUP:
	if ( wParam != VK_SPACE )
		break;

    case WM_LBUTTONUP:
	UP:
	if ( !IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
		break;
	
	ReleaseCapture(); SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);

	// see if button was last drawn down
	bHasFocus = ( GetFocus() == hWindow );
	bTrack = IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK);
	bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
	bSelected = GetWindowWord(hWindow, GWW_STATE);
	bDown = ( bSelected || (bTrack && bInRect && bHasFocus) );

	if ( IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT) )
		{
		SendMessage( GetParent(hWindow), WM_COMMAND,
			GET_WINDOW_ID(hWindow), 0L );
		}
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case WM_LBUTTONDBLCLK:
	if ( StyleOn( hWindow, WS_NOTENABLED ) )
		break;
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GET_WINDOW_ID(hWindow), 1L );
	break;

    case WM_MOUSEMOVE:
	if ( !IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
		break;
	GetClientRect( hWindow, &ClientRect );
	bInRect = IsWordBitSet(hWindow, GWW_STATUS, ST_INRECT);
	if ( bInRect == !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
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
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( StyleOn( hWindow, WS_NOTENABLED ) )
		break;
	if ( !StyleOn( hWindow, WS_TABSTOP ) )
		break;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case BM_SETSTATE:
    case BM_SETCHECK:
	if ( wParam == GetWindowWord( hWindow, GWW_STATE ) )
		break;
	SetWindowWord( hWindow, GWW_STATE, wParam );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case WM_ENABLE:
    case WM_SETTEXT:
	lReturn = DefWindowProc( hWindow, message, wParam, lParam );
	if (!wParam) // disabled?
		{
		if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
			{
			ReleaseCapture();
			SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
			}
		}
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
	return(lReturn);

    case WM_DESTROY:
	if ( IsWordBitSet(hWindow, GWW_STATUS, ST_TRACK) )
		{
		ReleaseCapture();
		SetWordBit(hWindow, GWW_STATUS, ST_TRACK, OFF);
		}
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
    }

return( TRUE );
}


/***********************************************************************/
static void DrawIconControl(HDC hDC, HWND hWnd, LPRECT lpRect, BOOL bSelected,
		BOOL bTrackInRect )
/***********************************************************************/
{
int x, y, i, dyText;
HBITMAP hBitmap;
HICON hIcon;
BOOL bHasFocus, bDown, bEnabled, bShadows, bHasText, bBorderOnly, bTwoFaced;
ITEMID id;
DWORD dwStyle;
STRING szString;
HFONT hFont;
TEXTMETRIC tm;
HINSTANCE hInstance;
BITMAP bm;

dwStyle = GetWindowLong( hWnd, GWL_STYLE );
bHasFocus = ( GetFocus() == hWnd );
bEnabled  = !( dwStyle & WS_NOTENABLED );
bShadows  = !( dwStyle & WS_NOSHADOWS );
bHasText  =  ( dwStyle & WS_TEXT );
bDown = ( bSelected || (bTrackInRect && bHasFocus) );
bBorderOnly = (dwStyle & WS_BORDERONLY);
bTwoFaced = (dwStyle & WS_TWOFACED);

// Draw the box interior
if ( !bTwoFaced )
	{
	if ( !bShadows )
		{
		if ( bEnabled )
			EraseControlBackground( hDC, hWnd, lpRect, CTLCOLOR_BTN );
		}
	else
	if ( !bDown || !bEnabled || bBorderOnly)
			FillRect( hDC, lpRect, Window.hButtonBrush );
	else	HilightRect( hDC, lpRect, Window.hHilightBrush );
	}

if ( !bHasText )
	dyText = 0;
else
	{ // Draw the text left justified
	if ( !(hFont = (HFONT)GetWindowWord( hWnd, GWW_FONT )) )
		hFont = (HFONT)GetStockObject( Window.iLabelFont );
	SelectObject( hDC, hFont );
	GetTextMetrics( hDC, &tm );
	dyText = (tm.tmHeight + tm.tmInternalLeading) + 3;
	}

// Compute the client rect center
x = ( lpRect->right + lpRect->left + 1 ) / 2;
y = ( lpRect->bottom + lpRect->top + 1 ) / 2 - dyText;
if ( bDown && bEnabled && !bBorderOnly && !bTwoFaced )
	{ x += 2; y += 2; }

if ( dwStyle & WS_IMAGE )
	{ // Draw the Image
	i = ( bDown && bEnabled ? 2 : 0 );
	DrawImageControl( hDC, hWnd, NULL, NULL, i, i, bShadows );
	}
else
	{ // Draw an icon or a bitmap
	if ( !(hInstance = (HINSTANCE)GetWindowWord( hWnd, GWW_ICONINST )) )
		hInstance = hInstAstral;
	if ( !(id = GetWindowWord( hWnd, GWW_ICONID )) )
		id = GET_WINDOW_ID( hWnd );
	if ( bTwoFaced && bSelected )
		id++;
	hBitmap = NULL;
	hIcon = NULL;
	if ( dwStyle & WS_BITMAP )
		{ // Draw the bitmap
		if ( hBitmap = DibResource2Bitmap( hInstance, id ) )
			{
			GetObject( hBitmap, sizeof(BITMAP), (LPSTR)&bm );
			if ( x < bm.bmWidth/2 )  x = bm.bmWidth/2;
			if ( y < bm.bmHeight/2 ) y = bm.bmHeight/2;
			DrawBitmap( hDC, hBitmap, x, y );
			DeleteObject( hBitmap );
			}
		}
	else
		{ // Draw the icon
		if ( hIcon = LoadIcon( (id > LAST_ICON ? 0 : hInstance ),
			MAKEINTRESOURCE(id) ) )
			DrawIcon( hDC, x-16, y-16, hIcon );
		}
	}

if ( bHasText )
	{ // Draw the text left justified
	y = ( RectHeight(lpRect) - dyText );
	GetWindowText( hWnd, szString, sizeof(szString) );
	if ( bDown ) i = 2; else i = 0;
	ColorText( hDC, 6+i, y+i, szString, lstrlen(szString),
	GetSysColor( (bEnabled || hIcon) ?
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
if ( hIcon && !bEnabled && !(dwStyle & WS_NOSHADOWS) )
	GrayWindow( hDC, hWnd, WS_NOTENABLED );
}


/***********************************************************************/
long WINPROC EXPORT ToyControl(
/***********************************************************************/
HWND	  hWindow,
unsigned  message,
WPARAM    wParam,
LPARAM    lParam)
{
RECT ClientRect;
PAINTSTRUCT ps;
HDC hDC;
BOOL bHasFocus, bSelected, bDown;
RGBS rgb;
COLOR Color;
DWORD dwStyle, dwReturn;
ITEMID id;
LPRGB lpReplaceRGB;
int cx, cy;
HBITMAP hBMobject, hBMmask;
static BOOL bTrack, bInRect;
//COLORINFO ColorInfo;

switch ( message )
    {
    case WM_PALETTECHANGED:
	InvalidateRect(hWindow, NULL, FALSE);
	break;

    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

	case WM_SIZE:
    if (IsButtonAnimated(hWindow))
        ResizeAnimatedButton(hWindow);
    else
	    ResizeDIBControl( hWindow );
	return( DefWindowProc( hWindow, message, wParam, lParam ) );

    case WM_PAINT:
	if ( SuperPaint( hWindow ) )
		break; // Printing in progress... (SuperBlt not reentrant)
	hDC = BeginPaint( hWindow, &ps );
	bSelected = GetWindowWord(hWindow, GWW_STATE );

	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	bHasFocus = ( GetFocus() == hWindow );
	bDown = ( bSelected || (bTrack && bInRect && bHasFocus) );

	// See if there is a replacement color specified
	if ( Color = GetWindowLong( hWindow, GWL_ALTERNATE ) )
		{
		CopyRGB( &Color, &rgb );
		lpReplaceRGB = &rgb;
		}
	else
		lpReplaceRGB = NULL;

	// Draw the Bitmap
    hBMobject = 0;
    if (IsButtonAnimated(hWindow))
		{
        hBMobject = AnimateButtonGetBitmap(hWindow);
		}
    else
	    {
	    if ( !(id = GetWindowWord( hWindow, GWW_ICONID )) )
			{
		    id = GET_WINDOW_ID( hWindow );
			}
		// support for loading toy bitmaps from add-on dll's
		if (! bPaintAppActive && hCurrentDLL != NULL)
			{
	    	hBMobject = DibResource2Bitmap (hCurrentDLL, id);
			}
		else
			{
	    	hBMobject = DibResource2Bitmap (hInstAstral, id);
			}
	    }
    if (hBMobject)
		{
		GetClientRect( hWindow, &ClientRect );
		cx = ( ClientRect.right  + ClientRect.left + 1 ) / 2;
		cy = ( ClientRect.bottom + ClientRect.top  + 1 ) / 2;
		if ( hBMmask = DrawTransDIB( hDC, hBMobject, NULL/*hBMmask*/, cx, cy ) )
			DeleteObject( hBMmask );
		DeleteObject( hBMobject );
		}

	EndPaint( hWindow, &ps );
	break;

    case WM_KEYDOWN:
	if ( StyleOn( hWindow, WS_NOTENABLED ) )
		break;
	if ( wParam != VK_SPACE )
		break;

    case WM_NCHITTEST:
	dwReturn = DefWindowProc( hWindow, message, wParam, lParam );
	if ( SHIFT && (dwReturn == HTCLIENT) )
		dwReturn = HTCAPTION;
	return( dwReturn );

    case WM_LBUTTONDOWN:
    if (IsButtonAnimated(hWindow))
    {
        AnimateButtonDoAnimation(hWindow);
        UpdateWindow (hWindow); // flush any paint messages that might
                                // interfere with bubble help (mpd)
    }
	if ( bTrack )
		break;
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	bInRect = YES;
    if (!IsButtonAnimated(hWindow))
    {
		SoundStartResource( "icons", NO/*bLoop*/, NULL/*hInstance*/ );
	}
	break;

    case WM_KEYUP:
	if ( wParam != VK_SPACE )
		break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
//	if ( CONTROL )
//		{
//		GetWindowRect( hWindow, &ClientRect );
//		Print("(%d,%d)", ClientRect.left, ClientRect.top );
//		break;
//		}
	if ( bInRect )
	    SendMessage( GetParent(hWindow), WM_COMMAND,
			GET_WINDOW_ID(hWindow), 0L );
	break;

    case WM_LBUTTONDBLCLK:
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GET_WINDOW_ID(hWindow), 1L );
	break;

    case WM_MOUSEMOVE:
	if ( !bTrack )
		break;
	GetClientRect( hWindow, &ClientRect );
	if ( bInRect == !PtInRect( &ClientRect, MAKEPOINT(lParam) ) )
		{
		bInRect = !bInRect;
		}
	break;

    case WM_SETCURSOR:
	SetCursor (LoadCursor (hInstAstral, MAKEINTRESOURCE(ID_HAND02)) );
    SetupCursor( wParam, lParam, GetDlgCtrlID (hWindow) ); // do the hint!
    return TRUE; // cursor has been handled!

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !StyleOn( hWindow, WS_TABSTOP ) )
		break;
	break;

    case BM_SETSTATE:
    case BM_SETCHECK:
	if ( wParam == GetWindowWord( hWindow, GWW_STATE ) )
		break;
	SetWindowWord( hWindow, GWW_STATE, wParam );
	break;

    case WM_ENABLE:
    case WM_SETTEXT:
	lReturn = DefWindowProc( hWindow, message, wParam, lParam );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
	return(lReturn);

    case WM_DESTROY:
	if ( !bTrack )
		break;
	ReleaseCapture();
	bTrack = NO;
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
    }

return( TRUE );
}

/***********************************************************************/
static void ResizeDIBControl( HWND hControl )
/***********************************************************************/
{
ITEMID idControl;
HWND hDlg;
LPBITMAPINFO lpDIB;
HGLOBAL hResource;
int cx, cy, dx, dy;
RECT Rect;
STRING szString;
BYTE c;
HINSTANCE hInst;

if ( !(idControl = GetWindowWord( hControl, GWW_ICONID )) )
	idControl = GET_WINDOW_ID( hControl );
hDlg = GetParent( hControl );

if (! bPaintAppActive && hCurrentDLL != NULL)
	hInst = hCurrentDLL;
else
	hInst = hInstAstral;

if ( !(hResource = (HGLOBAL)FindResource (hInst, MAKEINTRESOURCE(idControl),
	RT_BITMAP) ) )
	return;

if ( !(hResource = LoadResource (hInst, (HRSRC)hResource) ) )
	return;

if ( !(lpDIB = (LPBITMAPINFO)LockResource (hResource) ) )
	return;

// Make sure that the DIB is uncompressed
dx = lpDIB->bmiHeader.biWidth;
dy = lpDIB->bmiHeader.biHeight;
UnlockResource( hResource );
FreeResource( hResource );

GetWindowRect( hControl, &Rect );
Rect.right--; Rect.bottom--;
if ( dx == RectWidth(&Rect) && dy == RectHeight(&Rect) )
	return;

/* Convert control's coordinates to be relative to parent */
ScreenToClient( hDlg, (LPPOINT)&Rect.left );
ScreenToClient( hDlg, (LPPOINT)&Rect.right );

// See if a pixel location is specified
GetWindowText( hControl, szString, sizeof(szString) );
if ( c = szString[0] )
	{
	AsciiToInt2( szString, &cx, &cy );
	Rect.left = cx;
	Rect.top  = cy;
	}
else
if ( StyleOn( hControl, SS_CENTER ) )
	{
	Rect.left = (Rect.left + Rect.right + 1 - dx) / 2;
	Rect.top  = (Rect.top + Rect.bottom + 1 - dy) / 2;
	}

Rect.right  = Rect.left + dx - 1;
Rect.bottom = Rect.top + dy - 1;
SetWindowPos( hControl, 0, /* Z order ignored */
	Rect.left, Rect.top, /* New position */
	dx+1, dy+1, /* New size */
	SWP_NOZORDER );
}

/***********************************************************************/
long WINPROC EXPORT Color2Control(
/***********************************************************************/
HWND 	hWindow,
UINT 	message,
WPARAM 	wParam,
LPARAM 	lParam)
{
RECT ClientRect, SavedRect;
PAINTSTRUCT ps;
HDC hDC;
BOOL Bool;
BOOL bDrawingActive;
RGBS rgb;
COLOR Color;
BYTE bw;
int depth;
static BOOL bTrack, bInRect;

switch ( message )
    {
    case WM_PALETTECHANGED:
	InvalidateRect(hWindow, NULL, FALSE);
	break;

    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_PAINT:
	if ( SuperPaint( hWindow ) )
		break; // Printing in progress... (SuperBlt not reentrant)
	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );

	// Adjust the client rect for drawing the alternate color patch
	SavedRect = ClientRect;
	if ( StyleOn( hWindow, WS_TWOFACED ) )
		{
		// The alternate color is higher up on the screen
		ClientRect.left += RectWidth(&ClientRect)/5;
		ClientRect.bottom -= RectHeight(&ClientRect)/5;
		bDrawingActive = NO;
		}
	else
		bDrawingActive = YES;

	DrawColor:

	// Draw the box frame
	FrameRect( hDC, &ClientRect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
	InflateRect( &ClientRect, -1, -1 );

	// Draw the color patch
	Color = SendMessage(hWindow, WM_GETCOLOR, bDrawingActive?0:1, 0L);
	CopyRGB( &Color, &rgb );
	DrawColorPatch( hDC, &ClientRect, &rgb, YES );

	// If we just drew the active color, we're done
	if ( bDrawingActive )
		{
		GrayWindow( hDC, hWindow, WS_NOTENABLED );
		EndPaint( hWindow, &ps );
		break;
		}

	// Adjust the client rect for drawing the active color patch
	bDrawingActive = YES;
	ClientRect = SavedRect;
	// The active color is lower on the screen
	ClientRect.right -= RectWidth(&ClientRect)/5;
	ClientRect.top += RectHeight(&ClientRect)/5;
	// Go back and paint the active color
	goto DrawColor;

    case WM_LBUTTONDOWN:
	if ( bTrack )
		break;
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	bInRect = YES;
	// Switch the active and alternate color values and redraw
	Color = GetWindowLong( hWindow, GWL_ACTIVE );
	SetWindowLong( hWindow, GWL_ACTIVE, GetWindowLong(hWindow, GWL_ALTERNATE) );
	SetWindowLong( hWindow, GWL_ALTERNATE, Color );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	SoundStartResource( "icons", NO/*bLoop*/, NULL/*hInstance*/ );
	SwapActiveAlternate();
	break;

    case WM_LBUTTONUP:
	if ( !bTrack )
		break;
	ReleaseCapture(); bTrack = FALSE;
	if ( bInRect )
	    SendMessage( GetParent(hWindow), WM_COMMAND,
			GET_WINDOW_ID(hWindow), 0L );
	break;

    case WM_LBUTTONDBLCLK:
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GET_WINDOW_ID(hWindow), 1L );
	break;

    case WM_MOUSEMOVE:
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	if ( !StyleOn( hWindow, WS_TABSTOP ) )
		break;
	// Draw the focus rectangle
	break;

    case BM_SETSTATE:
    case BM_SETCHECK:
	if ( wParam == GetDlgItemInt( GetParent(hWindow),
	     GET_WINDOW_ID(hWindow), &Bool, NO ) )
		break;
	SetDlgItemInt( GetParent(hWindow), GET_WINDOW_ID(hWindow),
		wParam, NO );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	break;

    case WM_ENABLE:
    case WM_SETTEXT:
	lReturn = DefWindowProc( hWindow, message, wParam, lParam );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
	return(lReturn);

    case WM_GETCOLOR: // wParam - 0 = ActiveColor 1 = AlternateColor
	Color = GetWindowLong( hWindow, (!wParam ? GWL_ACTIVE : GWL_ALTERNATE) );
	if (lpImage)
		{
		if (depth = FrameDepth(ImgGetBaseEditFrame(lpImage)))
			{
			CopyRGB(&Color, &rgb);		// get rgb for Color
			// get cache pixel for rgb
			FrameRGB2Pixel(ImgGetBaseEditFrame(lpImage), &rgb, ( long *)&Color);

			if (FrameType(ImgGetBaseEditFrame(lpImage)) == FDT_GRAYSCALE)
				frame_getRGB(&Color, depth, &rgb); // get rgb for cache pixel
			}
		else
			{
			bw = wParam == 0 ? 0 : 255;
			bw ^= 255;
			rgb.red = rgb.green = rgb.blue = bw;
			}
			Color = RGB2long(rgb);		// get Color for rgb
		}
	return(Color);

    case WM_DESTROY:
	if ( !bTrack )
		break;
	ReleaseCapture();
	bTrack = NO;
	break;

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
    }

return( TRUE );
}

/***********************************************************************/
void DrawColorPatch(
/***********************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
LPRGB 	lpRGB,
BOOL 	bSuperBlt)
{
HBRUSH hBrush;
long Color;

Color = RGB(lpRGB->red, lpRGB->green, lpRGB->blue);

if ( bSuperBlt )
	{
	lpRect->right--;
	lpRect->bottom--;
	SuperFill( hDC, lpBltScreen, lpRect, 0, 0, Color, NULL );
	lpRect->right++;
	lpRect->bottom++;
	}
else
	{
	FillRect( hDC, lpRect, hBrush = CreateSolidBrush( Color ) );
	DeleteObject( hBrush );
	}
}

/***********************************************************************/
void SetControlValue(
/***********************************************************************/
HWND 	hDlg,
int 	nIDDlgItem,
int		nIndex,
long 	lValue,
BOOL 	fNotify)
{
HWND hDlgItem;

if ( !(hDlgItem = GetDlgItem( hDlg, nIDDlgItem )) )
	return;
if ( fNotify )
	SendMessage( hDlgItem, WM_SETCONTROL, nIndex, lValue);
SetWindowLong( hDlgItem, nIndex*4, lValue );
}

/***********************************************************************/
long GetControlValue(
/***********************************************************************/
HWND 	hDlg,
int		nIDDlgItem,
int		nIndex)
{
HWND hDlgItem;

if ( !(hDlgItem = GetDlgItem( hDlg, nIDDlgItem )) )
	return(0L);
return(GetWindowLong( GetDlgItem( hDlg, nIDDlgItem ), nIndex*4));
}


/***********************************************************************/
long WINPROC EXPORT TextBlockControl(
/***********************************************************************/
HWND 	hWindow,
UINT 	message,
WPARAM 	wParam,
LPARAM 	lParam)
{
PAINTSTRUCT PS;
HDC hDC, hPDC;
DWORD Style;
BOOL bCenter;
char WindowText[2048];
RECT Area;
short OldBkMode;
HFONT hOldFont, hFont;

switch ( message )
    {
    case WM_SETFONT:
	SetWindowWord(hWindow, GWW_FONT, wParam);
	if (lParam)
		InvalidateRect(hWindow, NULL, TRUE);
	break;

    case WM_GETDLGCODE:
	return( DLGC_WANTARROWS );

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_PAINT:
	if ( !(hDC = BeginPaint (hWindow,&PS)) )
		break;
	Style = GetWindowLong (hWindow,GWL_STYLE);
	if ( !GetTextBlockData( GET_WINDOW_ID( hWindow ),
	     WindowText, sizeof(WindowText) ) )
		GetWindowText( hWindow, WindowText, sizeof(WindowText) );

	bCenter = (Style & SS_CENTER);
	OldBkMode = SetBkMode (hDC,TRANSPARENT);
	if ( hFont = (HFONT)GetWindowWord(hWindow, GWW_FONT) )
		hOldFont = (HFONT)SelectObject(hDC, hFont);
	else	{
		hPDC = GetDC( GetParent(hWindow) );
		hFont = (HFONT)SelectObject( hPDC,
			GetStockObject(ANSI_VAR_FONT) );
		SelectObject( hPDC, hFont );
		hOldFont = (HFONT)SelectObject( hDC, hFont );
		ReleaseDC( GetParent(hWindow), hPDC );
		}
	GetClientRect (hWindow,&Area);
	DrawText( hDC, WindowText, -1, &Area,
		DT_WORDBREAK | (bCenter ? DT_CENTER : 0) );
	SetBkMode (hDC,OldBkMode);
	if (hOldFont)
		SelectObject (hDC,hOldFont);
	EndPaint (hWindow,&PS);
	break;

    case WM_LBUTTONDOWN:
	break;

    case WM_LBUTTONUP:
	break;

    case WM_MOUSEMOVE:
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_SETFOCUS:
    case WM_KILLFOCUS:
	break;

    case WM_ENABLE:
    case WM_SETTEXT:
	lReturn = DefWindowProc( hWindow, message, wParam, lParam );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
	return(lReturn);

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
    }

return( TRUE );
}


/***********************************************************************/
BOOL GetTextBlockData( int id, LPSTR lpData, int iSize)
{
HGLOBAL hTxtBlk;
LPSTR lpTxtBlk;

if ( !(hTxtBlk = (HGLOBAL)FindResource( hInstAstral, MAKEINTRESOURCE(id), RT_RCDATA )) )
	return( NO );
if ( !(hTxtBlk = LoadResource( hInstAstral, (HRSRC)hTxtBlk )) )
	return( NO );
if ( !(lpTxtBlk = (LPSTR)LockResource( hTxtBlk )) )
	{
	FreeResource( hTxtBlk );
	return( NO );
	}

iSize = min( lstrlen(lpTxtBlk), iSize-1 );
copy( (LPTR)lpTxtBlk, (LPTR)lpData, iSize );
lpData[iSize] = '\0';
UnlockResource( hTxtBlk );
FreeResource( hTxtBlk );
return( YES );
}


/***********************************************************************/
void DrawStaticBox(
/***********************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
BOOL 	bRaised,
BOOL	bDouble)
{
HPEN hOldPen, hPen1, hPen2;
RECT Rect;

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

if ( bRaised )
	{ hPen1 = (HPEN)GetStockObject(WHITE_PEN); hPen2 = Window.hShadowPen; }
else	{ hPen1 = Window.hShadowPen; hPen2 = (HPEN)GetStockObject(WHITE_PEN); }

// Draw the upper left border
hOldPen = (HPEN)SelectObject( hDC, hPen1 );
MoveToEx( hDC, Rect.left, Rect.bottom, NULL );
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
	MoveToEx( hDC, Rect.left, Rect.bottom, NULL );
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


/***********************************************************************/
static void DrawBoxFrame( HDC hDC, LPRECT lpRect )
/***********************************************************************/
{
HPEN hOldPen;
RECT Rect;

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

// Draw the box frame
hOldPen = (HPEN)SelectObject( hDC, GetStockObject(BLACK_PEN) );
MoveToEx( hDC, Rect.left, Rect.top, NULL );
LineTo( hDC, Rect.right, Rect.top );
LineTo( hDC, Rect.right, Rect.bottom );
LineTo( hDC, Rect.left, Rect.bottom );
LineTo( hDC, Rect.left, Rect.top );
SelectObject(hDC, hOldPen);
}

/***********************************************************************/
void DrawSculptedBox(
/***********************************************************************/
HDC 	hDC,
LPRECT 	lpRect,
BOOL 	bFocus,
BOOL	bDown,
BOOL	bDouble)
{
HPEN hOldPen;
RECT Rect;

Rect = *lpRect;
Rect.right--;
Rect.bottom--;

// Draw the box frame
hOldPen = (HPEN)SelectObject( hDC, GetStockObject(BLACK_PEN) );
MoveToEx( hDC, Rect.left+1, Rect.top, NULL );
LineTo( hDC, Rect.right, Rect.top );
MoveToEx( hDC, Rect.right, Rect.top+1, NULL );
LineTo( hDC, Rect.right, Rect.bottom );
MoveToEx( hDC, Rect.right-1, Rect.bottom, NULL );
LineTo( hDC, Rect.left, Rect.bottom );
MoveToEx( hDC, Rect.left, Rect.bottom-1, NULL );
LineTo( hDC, Rect.left, Rect.top );
InflateRect( &Rect, -1, -1 );

if ( bFocus )
	{ // Draw the focus rectangle
	MoveToEx( hDC, Rect.left, Rect.top, NULL );
	LineTo( hDC, Rect.right, Rect.top );
	LineTo( hDC, Rect.right, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.top );
	InflateRect( &Rect, -1, -1 );
	}

// Draw the upper left border
if ( bDouble )
	{
	SelectObject( hDC, GetStockObject( bDown ? BLACK_PEN : WHITE_PEN ) );
	MoveToEx( hDC, Rect.left, Rect.bottom, NULL );
	LineTo( hDC, Rect.left, Rect.top );
	LineTo( hDC, Rect.right, Rect.top );
	if ( bDown )
		{
		LineTo( hDC, Rect.right, Rect.top+1 );
		Rect.left++;
		Rect.top++;
		}
	else	{ // Draw the drop shadow
		SelectObject( hDC, Window.hShadowPen );
		LineTo( hDC, Rect.right, Rect.bottom );
		LineTo( hDC, Rect.left, Rect.bottom );
		InflateRect( &Rect, -1, -1 );
		}
	}

SelectObject( hDC, ( bDown ? Window.hShadowPen : GetStockObject(WHITE_PEN) ) );
MoveToEx( hDC, Rect.left, Rect.bottom, NULL );
LineTo( hDC, Rect.left, Rect.top );
LineTo( hDC, Rect.right, Rect.top );
if ( bDown )
	{
	LineTo( hDC, Rect.right, Rect.top+1 );
	Rect.left++;
	Rect.top++;
	}
else	{ // Draw the drop shadow
	SelectObject( hDC, Window.hShadowPen );
	LineTo( hDC, Rect.right, Rect.bottom );
	LineTo( hDC, Rect.left, Rect.bottom );
	InflateRect( &Rect, -1, -1 );
	}

SelectObject( hDC, hOldPen );
}


/***********************************************************************/
long WINPROC EXPORT ImageControl(
/***********************************************************************/
HWND		hWindow,
unsigned	message,
WPARAM		wParam,
LPARAM		lParam)
{
PAINTSTRUCT ps;
HDC hDC;
LPFRAME lpFrame;
LPOBJECT lpBase;
BOOL bMustClose;
int x, y, w, h, iImageWidth, iImageHeight;
long l;
WINDOWPOS FAR *lpWinPos;
HWND hParent;
RECT ClientRect, rWindow, DestRect;
MINMAXINFO FAR* lpmmi;
LPTOOLPROC lpToolProc;
static BOOL bTrack;

switch ( message )
    {
    case WM_PALETTECHANGED:
	InvalidateRect(hWindow, NULL, FALSE);
	break;

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_LBUTTONDOWN:
	if ( bTrack )
		break;
	SoundStartResource( "icons", NO/*bLoop*/, NULL/*hInstance*/ );
	if ( lpToolProc = (LPTOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ) )
		{
		(*lpToolProc)( hWindow, lParam, message );
		SetCapture( hWindow );
		bTrack = TRUE;
		}
	break;

    case WM_MOUSEMOVE:
//	if ( !bTrack )
//		break;
	if ( lpToolProc = (LPTOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ) )
		(*lpToolProc)( hWindow, lParam, message );
	break;

//    case WM_SETCURSOR:
//	if ( lpToolProc = (LPTOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ) )
//		(*lpToolProc)( hWindow, lParam, message );
//	break;

    case WM_LBUTTONUP:
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GET_WINDOW_ID(hWindow), 0L );
	if ( !bTrack )
		break;
	if ( lpToolProc = (LPTOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ) )
		{
		(*lpToolProc)( hWindow, lParam, message );
		ReleaseCapture();
		bTrack = FALSE;
		}
	break;

    case WM_LBUTTONDBLCLK:
	SendMessage( GetParent(hWindow), WM_COMMAND,
		GET_WINDOW_ID(hWindow), 1L );
	if ( lpToolProc = (LPTOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ) )
		(*lpToolProc)( hWindow, lParam, message );
	break;

	case WM_GETMINMAXINFO:
	  	if (!StyleOn(hWindow, IS_LIMITSIZE))
			break;
		if (!(hParent = GetParent(hWindow)))
				return(TRUE);
		GetClientRect(hParent, &ClientRect);
		lpmmi = (MINMAXINFO FAR*)lParam;
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
		return(FALSE);
	break;

	case WM_SIZE:
		SetLongBit(hWindow, GWL_STYLE, IS_HASCHANGED, TRUE);
		return( DefWindowProc( hWindow, message, wParam, lParam ) );

	case WM_MOVE:
		SetLongBit(hWindow, GWL_STYLE, IS_HASCHANGED, TRUE);
		return( DefWindowProc( hWindow, message, wParam, lParam ) );

    case WM_WINDOWPOSCHANGING:
	if ( !StyleOn(hWindow, IS_RATIO_CONSTRAIN) )
		return( DefWindowProc( hWindow, message, wParam, lParam ) );
	// constrain window to have same aspect ratio as its frame
	if (StyleOn(hWindow, WS_OBJECT))
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
	lpWinPos = (WINDOWPOS FAR*)lParam;
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
		return(0);
  	GetClientRect(hParent, &ClientRect);
	GetWindowRect(hWindow, &rWindow);
 	lpWinPos->x = Bound(lpWinPos->x, -w+2, ClientRect.right-2);
	lpWinPos->y = Bound(lpWinPos->y, -h+2, ClientRect.bottom-2);
	return( FALSE );
	break;

    case WM_PAINT:
	if ( SuperPaint( hWindow ) )
		break; // Printing in progress... (SuperBlt not reentrant)
	hDC = BeginPaint( hWindow, &ps );
    DestRect = ps.rcPaint;
	if (StyleOn(hWindow, WS_OBJECT))
		DrawObjListControl( hDC, hWindow, NULL, NULL, &DestRect, 0, 0, NO );
	else
		DrawImageControl( hDC, hWindow, NULL, &DestRect, 0, 0, NO );
	if ( lpToolProc = (LPTOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ) )
		{
		l = MAKELONG( DestRect.left, DestRect.top );
		(*lpToolProc)( hWindow, (LPARAM)l, WM_MOVE );
		l = MAKELONG( RectWidth(&DestRect), RectHeight(&DestRect) );
		(*lpToolProc)( hWindow, (LPARAM)l, WM_SIZE );
		(*lpToolProc)( hWindow, (LPARAM)(LPTR)hDC, message );
		}
	EndPaint( hWindow, &ps );
	break;

    case WM_NCHITTEST:
	if ( StyleOn( hWindow, IS_MOVEABLE ) )
			return( HandleNonClientHit( hWindow, message, wParam, lParam ));
	else	return( DefWindowProc( hWindow, message, wParam, lParam ) );

    case WM_ENABLE:
    case WM_SETTEXT:
	lReturn = DefWindowProc( hWindow, message, wParam, lParam );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
	return(lReturn);

	// change to fix the word puzzle room and it's funky edit control
	// and focus problems.
	case WM_KEYUP:
	case WM_CHAR:
    case WM_KEYDOWN:
	if (lpToolProc = (LPTOOLPROC)GetWindowLong( hWindow, GWL_IMAGE_TOOLPROC ))
		(*lpToolProc)( hWindow, (LPARAM)wParam, message );
	return(0L);
	case WM_GETDLGCODE:
	return (DLGC_WANTALLKEYS);

    default:
	return( DefWindowProc( hWindow, message, wParam, lParam ) );
    }

return( TRUE );
}


/***********************************************************************/
void DrawImageControl(
/***********************************************************************/
HDC 	hDC,
HWND 	hWnd,
LPFRAME lpFrame,
LPRECT 	lpDestRect,
int 	dx, 
int		dy,
BOOL 	bShadows)
{
int iImageWidth, iImageHeight, yAmount, iOldMode;
RECT SourceRect, DestRect, DisplayRect, ImageRect;
BOOL bMustClose;
LFIXED scale;
STRING szText;
TEXTMETRIC metrics;
COLORREF lOldColor;

bMustClose = NO;

if ( lpDestRect && !IsRectEmpty(lpDestRect) )
	DestRect = *lpDestRect;
else
	GetDrawClientRect( hWnd, &DestRect );

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

if ( !StyleOn( hWnd, IS_STRETCHABLE ) )
	ComputeImageControlRect( hWnd, iImageWidth, iImageHeight, &DisplayRect,
		bShadows );
else
	GetDrawClientRect( hWnd, &DisplayRect );

AstralIntersectRect( &DestRect, &DestRect, &DisplayRect );
SourceRect = DestRect;
OffsetRect( &SourceRect, -DisplayRect.left, -DisplayRect.top );
scale = FGET( iImageWidth, RectWidth(&DisplayRect) );
SourceRect.left   = FMUL( SourceRect.left, scale );
SourceRect.right  = FMUL( SourceRect.right, scale );
scale = FGET( iImageHeight, RectHeight(&DisplayRect) );
SourceRect.top    = FMUL( SourceRect.top, scale );
SourceRect.bottom = FMUL( SourceRect.bottom, scale );

SetRect( &ImageRect, 0, 0, iImageWidth-1, iImageHeight-1 );
AstralIntersectRect( &SourceRect, &SourceRect, &ImageRect );

// Draw the image
OffsetRect( &DestRect, dx, dy );
DrawFrame( lpFrame, hDC, &SourceRect, &DestRect, NULL );
if ( bMustClose )
	FrameClose( lpFrame );
if ( lpDestRect )
	*lpDestRect = DestRect;
}


/***********************************************************************/
void DrawObjListControl( HDC hDC, HWND hWnd, HBITMAP hBitmapOut,  LPOBJECT lpBase,
						LPRECT lpDestRect, int dx, int dy, BOOL bShadows )
/***********************************************************************/
{
int iImageWidth, iImageHeight, yAmount, iOldMode;
RECT SourceRect, DestRect, DisplayRect, ImageRect;
LPFRAME lpFrame;
LFIXED scale;
STRING szText;
TEXTMETRIC metrics;
COLORREF lOldColor;

if ( lpDestRect && !IsRectEmpty(lpDestRect) )
	DestRect = *lpDestRect;
else
	GetDrawClientRect( hWnd, &DestRect );

if ( !lpBase )
	lpBase = (LPOBJECT)GetWindowLong( hWnd, GWL_IMAGE ); 
if ( !lpBase && hWnd)
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

if (hWnd)
{
	if ( !StyleOn( hWnd, IS_STRETCHABLE ) )
		ComputeImageControlRect( hWnd, iImageWidth, iImageHeight, &DisplayRect,
			bShadows );
	else
		GetDrawClientRect( hWnd, &DisplayRect );
}		

AstralIntersectRect( &DestRect, &DestRect, &DisplayRect );
SourceRect = DestRect;
OffsetRect( &SourceRect, -DisplayRect.left, -DisplayRect.top );
scale = FGET( iImageWidth, RectWidth(&DisplayRect) );
SourceRect.left   = FMUL( SourceRect.left, scale );
SourceRect.right  = FMUL( SourceRect.right, scale );
scale = FGET( iImageHeight, RectHeight(&DisplayRect) );
SourceRect.top    = FMUL( SourceRect.top, scale );
SourceRect.bottom = FMUL( SourceRect.bottom, scale );

SetRect( &ImageRect, 0, 0, iImageWidth-1, iImageHeight-1 );
AstralIntersectRect( &SourceRect, &SourceRect, &ImageRect );

// Draw the image
OffsetRect( &DestRect, dx, dy );
DrawObjList( hDC, hBitmapOut, lpBase, &SourceRect, &DestRect);
if ( lpDestRect )
	*lpDestRect = DestRect;
}

#define LINES_PER_BLT 20
static LPTR lpBuffer;
static long lBufSize;

/************************************************************************/
int DrawObjList( HDC hDC, HBITMAP hBitmapOut, LPOBJECT lpBase, LPRECT lpSrcRect, LPRECT lpDstRect)
/************************************************************************/
{
int y, yline, ystart, ylast, DstWidth, SrcWidth, count, sx, depth, PaintWidth;
LFIXED yrate, xrate, yoffset;
RECT rPaint, rSource, rDest;
long lSizeNeeded;
LPTR lpDataBuf;
long dx, dy;
LPRECT lpRepairRect;
BLTSESSION BltSession;
LPFRAME lpFrame;
FRMDATATYPE Type;
int iBltLines;
#ifdef STATIC16  // Only in the new framelib
FRMTYPEINFO TypeInfo;
#endif

iBltLines = (hBitmapOut ? 0 : LINES_PER_BLT);
if (!lpBase)
	return(FALSE);

lpFrame = ObjGetEditFrame(lpBase);

Type = FrameType(lpFrame);
depth = FrameDepth( lpFrame );
if (depth == 0) depth = 1;
rDest = *lpDstRect;
rSource = *lpSrcRect;
lpRepairRect = NULL;
DstWidth = RectWidth( &rDest );
SrcWidth = RectWidth( &rSource );
lSizeNeeded = (long)DstWidth * depth;
if (lSizeNeeded > lBufSize)
	{
	if (lpBuffer)
		FreeUp(lpBuffer);
	lBufSize = lSizeNeeded;
	if ( !(lpBuffer = Alloc(lBufSize)) )
		{
		lBufSize = 0;
		return(FALSE);
		}
	}

lpDataBuf = Alloc((long)SrcWidth*depth);
if (!lpDataBuf)
	return(FALSE);
	
if ( !lpRepairRect )
	rPaint = rDest;
else
	{
	rPaint = *lpRepairRect;
	BoundRect( &rPaint, rDest.left, rDest.top, rDest.right, rDest.bottom );
	}

sx = rSource.left;
yrate = FGET( RectHeight(&rSource), RectHeight(&rDest) );
xrate = FGET( RectWidth(&rSource), RectWidth(&rDest) );
ystart = rSource.top;
ylast = -1;

dx = rPaint.left - rDest.left;
dy = rPaint.top - rDest.top;

yoffset = (dy*yrate)+((long)yrate>>1);
PaintWidth = RectWidth(&rPaint);
count = dx + PaintWidth;

#ifdef STATIC16 // Only in the new framelib
FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
StartSuperBlt( &BltSession, hDC, hBitmapOut, lpBltScreen, &rPaint, TypeInfo,
	iBltLines, 0, 0, YES, ImgPixelProc );
#else
StartSuperBlt( &BltSession, hDC, hBitmapOut, lpBltScreen, &rPaint, depth,
	iBltLines, 0, 0, YES, ImgPixelProc );
#endif
for ( y=rPaint.top; y<=rPaint.bottom; y++ )
	{
#ifdef WIN32
	yline = ystart + WHOLE( yoffset );
#else
	yline = ystart + HIWORD( yoffset );
#endif
	yoffset += yrate;
	if ( yline != ylast )
		{
		ylast = yline;
		ImgGetLine(NULL, lpBase, sx, yline, SrcWidth, lpDataBuf);
		FrameSample( lpFrame, lpDataBuf, 0, lpBuffer, (WORD)dx, count,
				xrate );
		}
	SuperBlt( &BltSession, lpBuffer );
	}
SuperBlt( &BltSession, NULL );
FreeUp(lpDataBuf);
return( TRUE );
}


HBITMAP Image2Bitmap(HWND hWnd)
{
// Capture the image into a bitmap.
// This function works just like WM_PAINT except the results go into a bitmap.
	HBITMAP hNewBitmap;
	HBITMAP hOld;
	HDC		hDC;
	RECT	rWnd;
	LPOBJECT lpObject = ImgGetBase(lpImage);

// Get the rect to copy.	
	if (hWnd)
		 GetClientRect(hWnd, &rWnd);
	else rWnd = lpObject->rObject;

	hDC = GetDC(hWnd);
	hNewBitmap = CreateCompatibleBitmap(
		hDC, 
		rWnd.right  - rWnd.left, 
		rWnd.bottom - rWnd.top);

// Draw the image into the memory DC.	
	DrawObjListControl(
		hDC, 
		hWnd,
		hNewBitmap,
		lpObject,
		&rWnd, 
		0, 
		0, 
		FALSE);

	ReleaseDC(hWnd, hDC);
	
	return hNewBitmap;
}


/***********************************************************************/
static void ComputeImageControlRect( HWND hWnd, int iImageWidth, int iImageHeight, LPRECT lpRect, BOOL bShadows )
/***********************************************************************/
{
RECT ClientRect;
int Width, Height, OldWidth, OldHeight;

GetDrawClientRect( hWnd, &ClientRect );

if (bShadows)
	InflateRect( &ClientRect, -3, -3); // adjust for beveling

Width = OldWidth = RectWidth( &ClientRect );
Height = OldHeight = RectHeight( &ClientRect );
ScaleToFit( &Width, &Height, iImageWidth, iImageHeight );
lpRect->left = ClientRect.left + ( ( OldWidth - Width ) / 2 );
lpRect->right = lpRect->left + Width - 1;
lpRect->top  = ClientRect.top  + ( ( OldHeight - Height ) / 2 );
lpRect->bottom = lpRect->top + Height - 1;
}


/***********************************************************************/
static LPFRAME GetImageFrame( HWND hWnd, LPINT lpbMustClose )
/***********************************************************************/
{
DWORD lValue;
STRING szString;
FNAME szFileName;
LPFRAME lpFrame;
LPIMAGE lpBaseImage;
int FileType; 
int DataType;
int nImage;

*lpbMustClose = NO;
lValue = GetWindowLong( hWnd, GWL_IMAGE ); // Normally a frame pointer
if ( !lValue )
	{ // Check the window name to see if it references an image
	GetWindowText( hWnd, szString, sizeof(szString) );
	if ( !szString[0] )
		return( NULL );
	lstrcpy( szFileName, Control.ProgHome );
	lstrcat( szFileName, szString );
	lpFrame = AstralFrameLoad( szFileName, -1, &DataType, &FileType );
	*lpbMustClose = YES;
	}
else
if ( lValue < 256 )
	{ // Its a numeric reference to an open image document
	nImage = lValue - 1;
	if ( nImage >= NumDocs() )
		return( NULL );
	lpBaseImage = (LPIMAGE)GetWindowLong( GetDoc(nImage), GWL_IMAGEPTR );
	lpFrame = ImgGetBaseEditFrame(lpBaseImage);
	}
else	lpFrame = (LPFRAME)lValue;

return( lpFrame );
}


/***********************************************************************/
void EraseControlBackground(
/***********************************************************************/
HDC 	hDC,
HWND 	hWnd,
LPRECT 	lpRect,
WORD 	wType)
{
HBRUSH hBrush;

hBrush = GetBackgroundBrush( hDC, hWnd, wType );
FillRect( hDC, lpRect, hBrush );
}


/***********************************************************************/
HBRUSH GetBackgroundBrush(
/***********************************************************************/
HDC 	hDC,
HWND 	hWnd,
WORD 	wType)
{
HBRUSH hBrush;

hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLOR, (WPARAM)hDC, (LPARAM)MAKELONG (hWnd,wType));
if (!hBrush)
	{
	hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	SetBkColor (hDC,GetSysColor (COLOR_WINDOW));
        SetTextColor (hDC,GetSysColor (COLOR_WINDOWTEXT));
	}
return( hBrush );
}


/***********************************************************************/
HBRUSH SelectBackgroundBrush(
/***********************************************************************/
HDC 	hDC,
HWND 	hWnd,
WORD 	wType)
{
HBRUSH hBrush;

hBrush = GetBackgroundBrush( hDC, hWnd, wType );
return( (HBRUSH)SelectObject( hDC, hBrush ) );
}


/***********************************************************************/
static LRESULT HandleNonClientHit( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
RECT Rect, At;
LRESULT lReturn;
int x, y, dx, dy;

lReturn = DefWindowProc( hWindow, message, wParam, lParam );
if ( lReturn != HTCLIENT ) // Return all non-client hits as is
	return( lReturn );
x = LOWORD( lParam );
y = HIWORD( lParam );
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
	if ( At.top )		lReturn = HTTOPLEFT; else
	if ( At.bottom )	lReturn = HTBOTTOMLEFT; else
						lReturn = HTLEFT;
	}
else
if ( At.right )
	{
	if ( At.top )		lReturn = HTTOPRIGHT; else
	if ( At.bottom )	lReturn = HTBOTTOMRIGHT; else
						lReturn = HTRIGHT;
	}
else
	{
	if ( At.top )		lReturn = HTTOP; else
	if ( At.bottom )	lReturn = HTBOTTOM; else
						lReturn = HTCAPTION;
	}
return( lReturn );
}

// Constrains *w, *h so that they have same ratio as width, height
/***********************************************************************/
static void ConstrainRatio( LPINT w, LPINT h, int width, int height )
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
static void MakeRatio( LPINT w, LPINT h, int width, int height )
/***********************************************************************/
{
LFIXED fRatio, fPreposedRatio;

fRatio = FGET(height, width);
fPreposedRatio	= FGET(*h,*w);
if (fPreposedRatio != fRatio)
	*h = FMUL(*w, fRatio);
}
