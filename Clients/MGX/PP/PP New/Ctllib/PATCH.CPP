#include "ctlutil.h"
#include "ctltools.h"
#include "patch.h"
#include "patch.rh"
#include "framelib.h"
#include "macros.h"


/************************************************************************
			PATCH CONTROL OVERVIEW

The patch control draws a color patch button. 
The color can be set by setting the controls text. 
ie: "h0,125,255" is an hsl color.   "r128,0,128" is an rgb color.
If R or H (capital) are used then superblt is used.  Otherwise GDI is used.
The	Patch_SetRGBColor can also be used.  In which case the style bit PS_USEGDI
determines whenter GDI or superblt are used.
If the WS_NOTENABLED style is set there is no 'button face'.
************************************************************************/


long WINPROC Patch_Control( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam );
LOCAL void draw_color_patch( HDC hDC, LPRECT lpRect, LPRGB lpRGB, BOOL bSuperBlt );

#define GWL_PATCH_COLOR 0
#define PATCH_EXTRA	4
#define PATCH_CLASS 0

/* void Cls_OnSetState(HWND hwnd, BOOL fState) */
#define HANDLE_BM_SETSTATE(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (BOOL)wParam), 0L)
/* void Cls_OnSetCheck(HWND hwnd, BOOL fCheck) */
#define HANDLE_BM_SETCHECK(hwnd, wParam, lParam, fn) \
    ((fn)(hwnd, (BOOL)wParam), 0L)



//************************************************************************
//		Patch_SetRGBColor
// DESCRIPTION:
//		Sets the color of the given patch.  Sets the text of the control 
//		to "".
//************************************************************************
VOID Patch_SetRGBColor(HWND hPatch, BYTE red, BYTE green, BYTE blue)
//************************************************************************
{	
	LONG l;
	RGBS rgb;
	
	rgb.red = red;
	rgb.green = green;
	rgb.blue = blue;
	l = RGB2long(rgb);
	SetWindowLong(hPatch, GWL_PATCH_COLOR, l);
	SetWindowText(hPatch, _T(""));
}



//************************************************************************
//		Patch_UseGDI
// DESCRIPTION:
//		Sets the control to use GDI or superblt
//************************************************************************
void Patch_UseGDI(HWND hPatch, BOOL bUseGDI)
//************************************************************************
{
	SetLongBit(hPatch, GWL_STYLE, PS_USEGDI, bUseGDI);
}


//************************************************************************
BOOL Patch_RegisterClass (HINSTANCE hInstance, UINT ClassScope)
//************************************************************************
{
	WNDCLASS WndClass;					   
	
	WndClass.hIcon		= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPTSTR)_T("patch");
	WndClass.hCursor	= NULL;
	WndClass.hbrBackground	= NULL;
	WndClass.hInstance	= hInstance;
	WndClass.style		= CS_DBLCLKS | ClassScope;
	WndClass.lpfnWndProc	= Patch_Control;
	WndClass.cbClsExtra	= PATCH_CLASS;
	WndClass.cbWndExtra	= PATCH_EXTRA;

	return(RegisterClass( &WndClass ));
}

BOOL Patch_UnRegisterClass (HINSTANCE hInstance)
{
    return (UnregisterClass ((LPTSTR)_T("patch"), hInstance));
}


/***********************************************************************/
/***********************************************************************/
// COLOR CONTROL
/***********************************************************************/
/**********************************************************************/

LOCAL void Patch_OnPaint(HWND hWindow);
LOCAL void Patch_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Patch_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Patch_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Patch_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Patch_OnKeyDown(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
LOCAL void Patch_OnKeyUp(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);

static BOOL Patch_bInRect;

/***********************************************************************/
LONG WINPROC EXPORT Patch_Control(	HWND hWindow, UINT message,
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
	
		HANDLE_MSG(hWindow, WM_PAINT, Patch_OnPaint);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Patch_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Patch_OnLButtonUp);
		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Patch_OnLButtonDblClk);
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Patch_OnMouseMove);
		HANDLE_MSG(hWindow, WM_KEYDOWN, Patch_OnKeyDown);
		HANDLE_MSG(hWindow, WM_KEYUP, Patch_OnKeyUp);

		default:
		return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL void Patch_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	RECT        ClientRect;
	HDC         hDC;
	BOOL        bHasFocus, bDown, bSuperBlt;
	RGBS        rgb;
	TCHAR       tmp[MAX_STR_LEN];
	WORD 		wChar;
	COLOR       Color;
	DWORD       dwStyle;

	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );

	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	bHasFocus = ( GetFocus() == hWindow );
	bDown = ( (Control_bTrack && Patch_bInRect && bHasFocus) );

	// Compute the color to paint
	GetWindowText( hWindow, tmp, sizeof( tmp ));

	if( IsDBCSLeadByte(( BYTE )*tmp ))
		wChar = *(( LPWORD )tmp );
	else
		wChar = *tmp;

	if( wChar )
	{
		AsciiRGB( MyCharNextN( tmp, 1 ), &rgb );

		// The first character indicates 'h' for HSL or 'r' for RGB
		if ( wChar == 'H' || wChar == 'h' )
			HSLtoRGB( rgb.red, rgb.green, rgb.blue, &rgb );

		// If lower case, use a Windows brush to draw the patch,
		// otherwise use SuperBlt
		bSuperBlt = ( wChar < 'a' || wChar > 'z' );
	}
	else
	{
		Color = GetWindowLong( hWindow, GWL_PATCH_COLOR);
		CopyRGB( &Color, &rgb );
		bSuperBlt = (dwStyle & PS_USEGDI)==0;
	}

	// Draw the color patch
	InflateRect( &ClientRect, -1, -1 );
	if ( !bDown )
		draw_color_patch( hDC, &ClientRect, &rgb, bSuperBlt );
	else	
		HilightRect( hDC, &ClientRect, DrawTools.GetBtnHilightBrush() );
	InflateRect( &ClientRect, 1, 1 );

	// Draw the box frame
	if ( WS_NOTENABLED & dwStyle )
		FrameRect( hDC, &ClientRect, ( HBRUSH )GetStockObject(BLACK_BRUSH) );
	else	
		DrawSculptedBox( hDC, &ClientRect, bHasFocus && (WS_TABSTOP & dwStyle), bDown, NO );

	EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Patch_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( Control_bTrack )
		return;
	SetCapture( hWindow ); Control_bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	Patch_bInRect = YES;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
}

/***********************************************************************/
LOCAL void Patch_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( !Control_bTrack )
		return;
	ReleaseCapture(); Control_bTrack = FALSE;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
	if ( Patch_bInRect )
		FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 0, SendMessage);
}

/***********************************************************************/
LOCAL void Patch_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow), hWindow, 1, SendMessage);
}

/***********************************************************************/
LOCAL void Patch_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	POINT pt;
	RECT ClientRect;

	if ( !Control_bTrack )
		return;
	GetClientRect( hWindow, &ClientRect );
	pt.x = x; pt.y = y;
	if ( Patch_bInRect == !PtInRect( &ClientRect, pt ) )
	{
		Patch_bInRect = !Patch_bInRect;
		InvalidateRect( hWindow, NULL, FALSE );
		UpdateWindow( hWindow );
	}
}

/***********************************************************************/
LOCAL void Patch_OnKeyDown(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
	if ( StyleOn( hWindow, WS_NOTENABLED ) )
		return;
	if ( vk == VK_SPACE )
		Patch_OnLButtonDown(hWindow, FALSE, 0, 0, 0);
}

/***********************************************************************/
LOCAL void Patch_OnKeyUp(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
	if ( vk == VK_SPACE )
		Patch_OnLButtonUp(hWindow, 0, 0, 0);
}

/***********************************************************************/
LOCAL void draw_color_patch( HDC hDC, LPRECT lpRect, LPRGB lpRGB, BOOL bSuperBlt )
/***********************************************************************/
{
	HBRUSH hBrush;
	long Color;

	Color = RGB(lpRGB->red, lpRGB->green, lpRGB->blue);
	    
	#ifndef _WINDLL
	if ( bSuperBlt )
	{
		lpRect->right--;
		lpRect->bottom--;
		SuperFill( hDC, Ctl_GetBltScreen(), lpRect, 0, 0, Color, NULL );
		lpRect->right++;
		lpRect->bottom++;
	}
	else
	#endif
	{

		FillRect( hDC, lpRect, hBrush = CreateSolidBrush( Color ) );
		DeleteObject( hBrush );
	}
}




