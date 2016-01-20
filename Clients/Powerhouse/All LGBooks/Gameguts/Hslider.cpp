#include <windows.h>
#include "proto.h"
#include "control.h"
#include "sound.h"
#include "mmsystem.h"
#include "scene.h"
#include "commonid.h"

#define SM_GETPOSITION	(WM_USER + 700)
#define SM_SETPOSITION	(WM_USER + 701)
#define HANDLE_SM_GETPOSITION(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)SM_GETPOSITION, wParam, lParam))
#define HANDLE_SM_SETPOSITION(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)SM_SETPOSITION, wParam, lParam))

typedef struct HSliderInfo
{
	RECT	rSlider;
	RECT	rBitmap;
	long	Min;	
	long	Max;
	long	Position;
	int		Step;
	int		iCenter;
	int		iSlideWidth;
	int		iTrackWidth;
	int		iTicks;
	BOOL	bTrack;
	BOOL	bInRect;
	BOOL	bFocus;
	COLORREF rgbBackgnd;
} HSLIDERINFO, FAR * LPHSLIDERINFO;

static HWND hLastWnd ;

/***********************************************************************/
BOOL HSlider_Register( HINSTANCE hInstance )
/***********************************************************************/
{
	WNDCLASS WndClass;
	WndClass.hCursor		= LoadCursor( hInstance, MAKEINTRESOURCE(IDC_HAND_POINT_CURSOR) );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"hslider";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= hInstance;
	WndClass.style			= 0;
	WndClass.lpfnWndProc	= HSliderControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 16;

	return( RegisterClass( &WndClass ) );
}

/***********************************************************************/
LOCAL void Slider_Init (LPHSLIDERINFO lpInfo)
/***********************************************************************/
{
	lpInfo->Min			= 0;
	lpInfo->Max			= 100;
	lpInfo->Position	= 0;
	lpInfo->Step		= 10;
	lpInfo->iSlideWidth = 14;
	lpInfo->iTrackWidth = 4;
	lpInfo->iTicks		= 10;
	lpInfo->bTrack		= FALSE;
	lpInfo->bInRect		= FALSE;
	lpInfo->bFocus		= FALSE;
	lpInfo->rgbBackgnd	= RGB(0, 0, 255);
}

/***********************************************************************/
static BOOL Slider_OnCreate (HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPHSLIDERINFO lpInfo;
	if ( (lpInfo = (LPHSLIDERINFO)AllocX( sizeof(HSLIDERINFO), GMEM_ZEROINIT|GMEM_SHARE )) )
	{
		Slider_Init (lpInfo) ;
		SetWindowLong( hWindow, GWL_DATAPTR, (long)lpInfo );
	}
	else
		return FALSE;

	return TRUE;
}

/***********************************************************************/
LOCAL void Slider_Paint (HWND hWindow, HDC hDC, LPRECT lpRect, BOOL fHighlight)
/***********************************************************************/
{
	BOOL	bHasFocus, bSelected, bDown;
	DWORD	dwStyle;
	RECT	rSrcArea, rDstArea;
	POINT	ptDst;
	UINT	id;
	PDIB	pdibSrc, pdibDst;
	HDC		hWinGDC;

	LPHSLIDERINFO lpInfo;
	if (! (lpInfo = (LPHSLIDERINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;

	rSrcArea = *lpRect;
	rDstArea = *lpRect;
	MapWindowPoints (hWindow, GetParent(hWindow), (LPPOINT)& rDstArea, 2 );

	LPSCENE lpScene = CScene::GetScene (GetParent (hWindow) );
	if (! lpScene)
		return;

	if ( !lpScene->GetDIBs( &pdibSrc, &pdibDst, &hWinGDC ) )
		return;
	if ( !pdibSrc || !pdibDst || !hWinGDC )
		return;

	bSelected = GetWindowWord (hWindow, GWW_STATE);
	dwStyle = GetWindowStyle(hWindow);
	bHasFocus = (GetFocus () == hWindow);
	bDown = ( bSelected || (lpInfo->bTrack && lpInfo->bInRect && bHasFocus) );

	// Repair the dirty bitmap with the clean bitmap
	if ( pdibSrc->GetCompression() == BI_RLE8 ||
		 pdibDst->GetCompression() == BI_RLE8)
	{ // compressed DIBs must use GDI copying (lose transparency ability)
		pdibSrc->DCBlt( hWinGDC,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top );
	}
	else
	{
		pdibSrc->DibBlt( pdibDst,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
						NO /*bTransparent*/ );
	}

	// Draw the slider background
	RECT rSlider = lpInfo->rSlider;
	RECT rInner = lpInfo->rSlider;
	rInner.top = lpInfo->iCenter - (lpInfo->iTrackWidth / 2);
	rInner.bottom = lpInfo->iCenter + (lpInfo->iTrackWidth / 2);
	MapWindowPoints (hWindow, GetParent(hWindow), (LPPOINT)& rSlider, 2);
	MapWindowPoints (hWindow, GetParent(hWindow), (LPPOINT)& rInner, 2);

	// Turn off tick painting because using background bitmap
	#ifdef UNUSED
	HBRUSH hBrush = CreateSolidBrush (lpInfo->rgbBackgnd);
	HBRUSH hBrush2 = (HBRUSH)GetStockObject (BLACK_BRUSH);

	FillRect (hWinGDC, & rSlider, hBrush);
	FillRect (hWinGDC, & rInner, hBrush2);
	DeleteObject (hBrush);
	#endif

	RECT	rClient;
	GetClientRect (hWindow, & rClient);
	MapWindowPoints (hWindow, GetParent(hWindow), (LPPOINT)& rClient, 2 );

	// Turn off tick painting because using background bitmap
	#ifdef UNUSED
	// Draw a chiselled border
	RECT	rBorder = rClient;
	InflateRect (& rBorder, 1, 1);
	int cxButton = rBorder.right - rBorder.left;
	int cyButton = rBorder.bottom - rBorder.top;			   
	HPEN hLitePen = CreatePen (PS_SOLID, 1, RGB (255, 255, 255));
	HPEN hDarkPen = CreatePen (PS_SOLID, 1, RGB (128, 128, 128));
	HPEN hOldPen = (HPEN)SelectObject (hWinGDC, hLitePen);

	MoveToEx (hWinGDC, rBorder.left, rBorder.bottom - 1, NULL);
	LineTo (hWinGDC, rBorder.left, rBorder.top);
	LineTo (hWinGDC, rBorder.right - 1, rBorder.top);
	
	MoveToEx (hWinGDC, rBorder.left + 1, rBorder.bottom - 2, NULL);
	LineTo (hWinGDC, rBorder.left + 1, rBorder.top + 1);
	LineTo (hWinGDC, rBorder.right - 2, rBorder.top + 1);
	
	SelectObject (hWinGDC, hDarkPen);
	MoveToEx (hWinGDC, rBorder.right - 1, rBorder.top, NULL);
	LineTo (hWinGDC, rBorder.right - 1, rBorder.bottom - 1);
	LineTo (hWinGDC, rBorder.left, rBorder.bottom - 1);
	
	MoveToEx (hWinGDC, rBorder.right - 2, rBorder.top + 1, NULL);
	LineTo (hWinGDC, rBorder.right - 2, rBorder.bottom - 2);
	LineTo (hWinGDC, rBorder.left + 1, rBorder.bottom - 2);

	SelectObject (hWinGDC, hOldPen);
	DeleteObject (hLitePen);
	DeleteObject (hDarkPen);

	// If tick-marks are wanted
	if (lpInfo->iTicks)
	{
		int pw = 2; // pen width
		HPEN hTickPen = CreatePen (PS_SOLID, pw, lpInfo->rgbBackgnd);
		hOldPen = (HPEN)SelectObject (hWinGDC, hTickPen);

		int iBmpWidth = lpInfo->rBitmap.right - lpInfo->rBitmap.left;
		int wx = rClient.right - rClient.left - iBmpWidth;
		int dx = wx / lpInfo->iTicks;
		wx = wx - (dx * lpInfo->iTicks) - pw;
		int x = rClient.left + (iBmpWidth + wx + pw) / 2; // wx is now the excess
		int y1 = rClient.top + 2;
		int y2 = y1 + 10;
		for (int idx = 0; idx <= lpInfo->iTicks ; idx++)
		{
			MoveToEx (hWinGDC, x, y1, NULL);
			LineTo (hWinGDC, x, y2);
			x += dx;
		}
		SelectObject (hWinGDC, hOldPen);
		DeleteObject (hTickPen);
	}
	#endif

	rSrcArea = lpInfo->rBitmap;
	rDstArea = lpInfo->rBitmap;
	MapWindowPoints (hWindow, GetParent(hWindow), (LPPOINT)& rDstArea, 2 );

	// Get the control ID to load the bitmap resource
	if (! (id = GetWindowWord (hWindow, GWW_ICONID) ) )
		id = GET_WINDOW_ID (hWindow);

	if (pdibSrc = CDib::LoadDibFromResource (GetWindowInstance (hWindow),
		MAKEINTRESOURCE(id), GetApp()->m_hPal, (dwStyle & BS_MASK) != 0 ))
	{ // Load the resource
		if ( pdibSrc->GetCompression() == BI_RLE8 ||
			 pdibDst->GetCompression() == BI_RLE8 )
		{ // compressed DIBs must use GDI copying (lose transparency ability)
			pdibSrc->DCBlt( hWinGDC,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
						rSrcArea.left, rSrcArea.top,
						rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top );
		}
		else
		{
			RGBTRIPLE rgb;
			LPRGBTRIPLE lpRGB = NULL;
			if (dwStyle & BS_MASK)
			{
				STRING szColor;
				GetWindowText(hWindow, szColor, sizeof(szColor));
				AsciiRGB( szColor, &rgb );
				lpRGB = &rgb;
			}
			if (dwStyle & BS_HIGHLIGHT)
			{
				// Get the transparency color
				BYTE bTrans = *pdibSrc->GetXY(0, 0);

				// Get the color to replace
				LPTR lpColor = pdibSrc->GetXY(1, 0);
				BYTE bColor = *lpColor;
				*lpColor = bTrans;

				// Get the hightlight color
				LPTR lpHighlight = pdibSrc->GetXY(2, 0); 
				BYTE bHighlight = *lpHighlight;
				*lpHighlight = bTrans;

				// Replace the pixels in the DIB
				if (bDown && (bColor != bHighlight))
				{
					HPTR hp = pdibSrc->GetPtr();
					DWORD dwSize = pdibSrc->GetSizeImage();
					while ( dwSize-- )
					{
						if (*hp == bColor)
							*hp = bHighlight;
						++hp;
					}
				}
			}

			// Set slider position if needed
			if (lpInfo->Position)
			{
				int iBmpWidth = lpInfo->rBitmap.right - lpInfo->rBitmap.left;
				int iWidth = rClient.right - rClient.left - iBmpWidth;
				int x = (int)(((long)(lpInfo->Position - lpInfo->Min) * iWidth) / (lpInfo->Max - lpInfo->Min));
				rDstArea.left += x;
				if (rDstArea.left < rClient.left)
					rDstArea.left = rClient.left;
				else
				if (rDstArea.left + iBmpWidth > rClient.right)
					rDstArea.left = rClient.right - iBmpWidth;
				rDstArea.right = rDstArea.left + iBmpWidth;
			}

			pdibSrc->DibBlt( pdibDst,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
						rSrcArea.left, rSrcArea.top,
						rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top,
						(dwStyle & BS_TRANSPARENT) != 0 /*bTransparent*/, lpRGB );
		}
		delete pdibSrc;
	}

	ptDst.x = rSrcArea.left;
	ptDst.y = rSrcArea.top;
	if (lpScene)
		lpScene->Paint( hDC, &rClient, &ptDst );
}

/***********************************************************************/
LOCAL BOOL ComputeDIBControlSize (HWND hControl, LPINT lpDX, LPINT lpDY)
/***********************************************************************/
{
	ITEMID idControl;
	LPBITMAPINFO lpDIB;
	HGLOBAL hResource;

	LPHSLIDERINFO lpInfo;
	if (! (lpInfo = (LPHSLIDERINFO)GetWindowLong( hControl, GWL_DATAPTR )) )
		return FALSE;

	if (! (idControl = GetWindowWord (hControl, GWW_ICONID) ) )
		idControl = GET_WINDOW_ID (hControl);

	if (! (hResource = (HGLOBAL)FindResource (GetApp()->GetInstance(), MAKEINTRESOURCE(idControl),
	  RT_BITMAP) ) )
		return (FALSE);

	if (! (hResource = LoadResource (GetApp()->GetInstance(), (HRSRC)hResource) ) )
		return (FALSE);

	if (! (lpDIB = (LPBITMAPINFO)LockResource (hResource) ) )
		return (FALSE);

	RECT rClient;
	GetClientRect (hControl, & rClient);

	if ( lpDX )
		*lpDX = rClient.right - rClient.left;
	if ( lpDY )
		*lpDY = rClient.bottom - rClient.top;

	// Set up the DIB rectangle
	if (lpInfo)
	{
		lpInfo->iCenter = (rClient.bottom - rClient.top) / 2;

		lpInfo->rSlider.top = (rClient.bottom - rClient.top) / 2 -
								lpInfo->iSlideWidth / 2;
		lpInfo->rSlider.left = rClient.left;
		lpInfo->rSlider.right = rClient.right;
		lpInfo->rSlider.bottom = (rClient.bottom - rClient.top) / 2 +
								lpInfo->iSlideWidth / 2;

		lpInfo->rBitmap.top = 0;
		lpInfo->rBitmap.left = 0;
		lpInfo->rBitmap.right = lpInfo->rBitmap.left + (int)lpDIB->bmiHeader.biWidth;
		lpInfo->rBitmap.bottom = lpInfo->rBitmap.top + (int)abs(lpDIB->bmiHeader.biHeight);
	}
	UnlockResource (hResource);
	FreeResource (hResource);
	return (TRUE);
}

/***********************************************************************/
LOCAL void Slider_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPHSLIDERINFO lpInfo;
	if ( !(lpInfo = (LPHSLIDERINFO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;
	if (lpInfo->bTrack)
	{
		ReleaseCapture();
		lpInfo->bTrack = NO;
	}
	FreeUp (lpInfo);
}

/***********************************************************************/
LOCAL void Slider_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
	int dx, dy;

	if (ComputeDIBControlSize (hWindow, & dx, & dy))
		ResizeControl (hWindow, dx, dy);
	FORWARD_WM_SIZE(hWindow, state, cx, cy, DefWindowProc);
}

/***********************************************************************/
LOCAL void Slider_OnSetFont(HWND hWindow, HFONT hFont, BOOL fRedraw)
/***********************************************************************/
{
	SetWindowLong(hWindow, GWL_FONT, (long)(LPTR)hFont);
	if (fRedraw)
		InvalidateRect(hWindow, NULL, TRUE);
}

/***********************************************************************/
LOCAL UINT Slider_OnGetDlgCode(HWND hWindow, LPMSG lpmsg)
/***********************************************************************/
{
	return (DLGC_WANTARROWS);
}

/***********************************************************************/
LOCAL BOOL Slider_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return (TRUE);
}

/***********************************************************************/
LOCAL UINT Slider_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
	UINT uReturn = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if ( SHIFT && (uReturn == HTCLIENT) )
		uReturn = HTCAPTION;
	return (uReturn);
}

/***********************************************************************/
LOCAL void Slider_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
	InvalidateRect (hWindow, NULL, FALSE);
}

/***********************************************************************/
LOCAL void Slider_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;

	if (hDC = BeginPaint( hWindow, &ps ) )
	{
		Slider_Paint(hWindow, hDC, &ps.rcPaint, FALSE);
		EndPaint( hWindow, &ps );
	}
}

/***********************************************************************/
LOCAL void Slider_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	LPHSLIDERINFO lpInfo;
	if ( !(lpInfo = (LPHSLIDERINFO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;
	if ( SHIFT )
		return;
	if ( lpInfo->bTrack )
		return;

	SetCapture( hWindow );
	lpInfo->bTrack = TRUE;

	if ( GetFocus() != hWindow )
		SetFocus( hWindow );

	lpInfo->bInRect = YES;

	RECT ClientRect;
	GetClientRect (hWindow, & ClientRect);
	
	x -= ClientRect.left;
	int iBmpWidth = lpInfo->rBitmap.right - lpInfo->rBitmap.left;
	x -= (iBmpWidth/2);
	if ( x < 0 )
		x = 0;

	int iWidth = ClientRect.right - ClientRect.left - iBmpWidth;
	if ( x > iWidth )
		x = iWidth;

	lpInfo->Position = lpInfo->Min +
		(int)(((long)x * (lpInfo->Max - lpInfo->Min)) / iWidth);

	// Send the new setting back to the parent
	FORWARD_WM_COMMAND( GetParent(hWindow), GET_WINDOW_ID(hWindow), 0,
		lpInfo->Position, SendMessage );

	InvalidateRect (hWindow, & ClientRect, TRUE);
}

/***********************************************************************/
LOCAL void Slider_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	LPHSLIDERINFO lpInfo;
	if (! (lpInfo = (LPHSLIDERINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;
	if (! lpInfo->bTrack)
		return;
	ReleaseCapture ();
	lpInfo->bTrack = FALSE;

	// Send the new setting back to the parent
	FORWARD_WM_COMMAND( GetParent(hWindow), GET_WINDOW_ID(hWindow), 1,
		lpInfo->Position, SendMessage );
}

/***********************************************************************/
LOCAL void Slider_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
}

/***********************************************************************/
LOCAL void Slider_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	RECT ClientRect;
	POINT pt;
	LPHSLIDERINFO lpInfo;

	if (! (lpInfo = (LPHSLIDERINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;
	if (! lpInfo->bTrack)
		return;

	GetClientRect (hWindow, & ClientRect);
	pt.x = x;
	pt.y = y;
	if (lpInfo->bInRect == ! PtInRect (& ClientRect, pt ) )
		lpInfo->bInRect = ! lpInfo->bInRect;

	x -= ClientRect.left;
	int iBmpWidth = lpInfo->rBitmap.right - lpInfo->rBitmap.left;
	x -= (iBmpWidth/2);
	if ( x < 0 )
		x = 0;

	int iWidth = ClientRect.right - ClientRect.left - iBmpWidth;
	if ( x > iWidth )
		x = iWidth;

	lpInfo->Position = lpInfo->Min +
		(int)(((long)x * (lpInfo->Max - lpInfo->Min)) / iWidth);

	// Send the new setting back to the parent
	FORWARD_WM_COMMAND( GetParent(hWindow), GET_WINDOW_ID(hWindow), 0,
		lpInfo->Position, SendMessage );
	InvalidateRect (hWindow, & ClientRect, TRUE);
}

/***********************************************************************/
LOCAL void Slider_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
	FORWARD_WM_ENABLE (hWindow, fEnable, DefWindowProc);
	InvalidateRect (hWindow, NULL, FALSE);
	UpdateWindow (hWindow);
}

/***********************************************************************/
LOCAL void Slider_OnPaletteChanged (HWND hWindow, HWND hwndPaletteChange)
/***********************************************************************/
{
	InvalidateRect (hWindow, NULL, FALSE);
}

/***********************************************************************/
LOCAL LPHSLIDERINFO Slider_GetInfo (HWND hWindow)
/***********************************************************************/
{
	return ((LPHSLIDERINFO)GetWindowLong (hWindow, GWL_DATAPTR));
}

/***********************************************************************/
LOCAL long Slider_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	long lResult = 0L;
	LPHSLIDERINFO lpInfo = Slider_GetInfo (hWindow);

	if (lpInfo)
	{
		switch (msg)
		{
			case SM_GETPOSITION:
				lResult = lpInfo->Position;
				break;

			case SM_SETPOSITION:
				lpInfo->Position = lParam;
				// Enforce min/max range
				if (lpInfo->Position < lpInfo->Min)
					lpInfo->Position = lpInfo->Min;
				if (lpInfo->Position > lpInfo->Max)
					lpInfo->Position = lpInfo->Max;
				InvalidateRect (hWindow,NULL,FALSE);

			default:
				break;
		}
	}
	return lResult;
}

/***********************************************************************/
LONG WINPROC EXPORT HSliderControl(HWND hWindow, UINT message,
			  WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, Slider_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Slider_OnDestroy);
		HANDLE_MSG(hWindow, WM_SIZE, Slider_OnSize);
		HANDLE_MSG(hWindow, WM_SETFONT, Slider_OnSetFont);
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Slider_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Slider_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_NCHITTEST, Slider_OnNCHitTest);
		HANDLE_MSG(hWindow, WM_MOVE, Slider_OnMove);
		HANDLE_MSG(hWindow, WM_PAINT, Slider_OnPaint);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Slider_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Slider_OnLButtonUp);
		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Slider_OnLButtonDblClk);
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Slider_OnMouseMove);
		HANDLE_MSG(hWindow, WM_ENABLE, Slider_OnEnable);
		HANDLE_MSG(hWindow, WM_PALETTECHANGED, Slider_OnPaletteChanged);
		HANDLE_MSG(hWindow, SM_GETPOSITION, Slider_OnUserMsg);
		HANDLE_MSG(hWindow, SM_SETPOSITION, Slider_OnUserMsg);

		default:
			return DefWindowProc (hWindow, message, wParam, lParam);
	}
}



