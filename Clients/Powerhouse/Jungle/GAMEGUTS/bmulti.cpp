#include <windows.h>
#include "proto.h"
#include "control.h"

#define HANDLE_MB_SETSTATE(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)MB_SETSTATE, wParam, lParam))
#define HANDLE_MB_GETSTATE(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)MB_GETSTATE, wParam, lParam))

typedef struct BmultiContInfo
{
	int		State;	// Initially 0 = normal state
	int		NumDIBs;
	int		Incr;
	BOOL	bTrack;
	BOOL	bInRect;
} BMULTICONTINFO, FAR * LPBMULTICONTINFO;

static HWND hLastWnd;

/***********************************************************************/
static BOOL Bmulti_OnCreate (HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPBMULTICONTINFO lpInfo;
	if (! (lpInfo = (LPBMULTICONTINFO)AllocX (sizeof(BMULTICONTINFO), GMEM_ZEROINIT|GMEM_SHARE)) )
		return FALSE;

	lpInfo->State = 0;
	lpInfo->NumDIBs = 8;
	lpInfo->Incr = 5;
	lpInfo->bTrack = FALSE;
	lpInfo->bInRect = FALSE;
	SetWindowLong (hWindow, GWL_DATAPTR, (long)lpInfo);
	return TRUE;
}

/***********************************************************************/
LOCAL void Bmulti_Paint(HWND hWindow, HDC hDC, LPRECT lpRect)
/***********************************************************************/
{
	BOOL bHasFocus, bSelected, bDown;
	DWORD dwStyle;
	RECT rSrcArea, rDstArea;
	POINT ptDst;
	PDIB pdibSrc, pdibDst;
	UINT id;
	HDC hWinGDC;
	HPALETTE hLoadPal;	
	BOOL	fUseScene = TRUE;

	LPBMULTICONTINFO lpInfo;
	if (! (lpInfo = (LPBMULTICONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;

	rSrcArea = *lpRect;
	rDstArea = *lpRect;

	LPSCENE lpScene = CScene::GetScene(GetParent(hWindow));
	if ( !lpScene )
		return;

	if ( lpScene->GetDIBs( &pdibSrc, &pdibDst, &hWinGDC ) )
	{
		if ( !pdibDst )
			return;
		hLoadPal = GetApp()->m_hPal;
		MapWindowPoints( hWindow, GetParent(hWindow), (LPPOINT)&rDstArea, 2 );
	}
	else
		return;

	bSelected = GetWindowWord (hWindow, GWW_STATE);
	dwStyle = GetWindowLong (hWindow, GWL_STYLE);
	bHasFocus = (GetFocus() == hWindow);
	bDown = ( bSelected || (lpInfo->bTrack && lpInfo->bInRect && bHasFocus) );

	// Draw the Bitmap
	if ( !(id = GetWindowWord( hWindow, GWW_ICONID )) )
		id = GET_WINDOW_ID( hWindow );
	id += lpInfo->State;

	// Repair the dirty bitmap with the clean bitmap
	if ( pdibSrc && pdibDst )
	{
		if ( pdibSrc->GetCompression() == BI_RLE8 || pdibDst->GetCompression() == BI_RLE8)
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
	}

	if (pdibSrc = CDib::LoadDibFromResource( GetWindowInstance(hWindow),
		MAKEINTRESOURCE(id), hLoadPal, (dwStyle & BS_MASK) != 0 ) )
	{ // Load the resource
			// Do highlighting andf transparency
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
		if ( pdibSrc->GetCompression() == BI_RLE8 || pdibDst->GetCompression() == BI_RLE8 )
		{ // compressed DIBs must use GDI copying (lose transparency ability)
			pdibSrc->DCBlt( hWinGDC,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
						rSrcArea.left, rSrcArea.top,
						rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top );
		}
		else
		{
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
	if (fUseScene)
		lpScene->Paint( hDC, &rDstArea, &ptDst );

}

/***********************************************************************/
LOCAL void Bmulti_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;

	if ( !(hDC = BeginPaint( hWindow, &ps )) )
		return;
	Bmulti_Paint(hWindow, hDC, &ps.rcPaint);
	EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Bmulti_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPBMULTICONTINFO lpInfo;
	if (! (lpInfo = (LPBMULTICONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;
	if (lpInfo->bTrack)
	{
		ReleaseCapture ();
		lpInfo->bTrack = FALSE;
	}
	FreeUp (lpInfo);
}

/***********************************************************************/
LOCAL BOOL Bmulti_ComputeSize( HWND hControl, LPINT lpDX, LPINT lpDY )
/***********************************************************************/
{
	ITEMID idControl;
	LPBITMAPINFO lpDIB;
	HGLOBAL hResource;

	if ( !(idControl = GetWindowWord( hControl, GWW_ICONID )) )
		idControl = GET_WINDOW_ID( hControl );

	if ( !(hResource = (HGLOBAL)FindResource (GetWindowInstance(hControl), MAKEINTRESOURCE(idControl),
		RT_BITMAP) ) )
		return( NO );

	if ( !(hResource = LoadResource (GetWindowInstance(hControl), (HRSRC)hResource) ) )
		return( NO );

	if ( !(lpDIB = (LPBITMAPINFO)LockResource (hResource) ) )
		return( NO );

	// Make sure that the DIB is uncompressed
	if ( lpDX )
		*lpDX = (int)lpDIB->bmiHeader.biWidth;
	if ( lpDY )
		*lpDY = (int)abs(lpDIB->bmiHeader.biHeight);
	UnlockResource( hResource );
	FreeResource( hResource );
	return( YES );
}

/***********************************************************************/
static BOOL Bmulti_OnSizeOrPosChange( HWND hWindow, LPWINDOWPOS lpWindowPos )
/***********************************************************************/
{
	if ( lpWindowPos->flags & SWP_NOSIZE )
		return( YES );

	int cx, cy;
	if ( !Bmulti_ComputeSize( hWindow, &cx, &cy ) )
		return( YES );

	if ( lpWindowPos->cx == cx && lpWindowPos->cy == cy )
		return( YES );

	lpWindowPos->cx = cx;
	lpWindowPos->cy = cy;
	return( NO );
}

/***********************************************************************/
LOCAL void Bmulti_OnSetFont(HWND hWindow, HFONT hFont, BOOL fRedraw)
/***********************************************************************/
{
	SetWindowLong(hWindow, GWL_FONT, (long)(LPTR)hFont);
	if (fRedraw)
		InvalidateRect(hWindow, NULL, TRUE);
}

/***********************************************************************/
LOCAL UINT Bmulti_OnGetDlgCode(HWND hWindow, LPMSG lpmsg)
/***********************************************************************/
{
	return( DLGC_WANTARROWS );
}

/***********************************************************************/
LOCAL BOOL Bmulti_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
LOCAL UINT Bmulti_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
	UINT uReturn = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if ( SHIFT && (uReturn == HTCLIENT) )
		uReturn = HTCAPTION;
	return( uReturn );
}

/***********************************************************************/
LOCAL void Bmulti_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
	InvalidateRect( hWindow, NULL, FALSE );
}

/***********************************************************************/
LOCAL void Bmulti_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	DWORD dwStyle;
	if ( SHIFT )
		return;
	LPBMULTICONTINFO lpInfo;
	if (! (lpInfo = (LPBMULTICONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;
	if (lpInfo->bTrack)
		return;

	SetCapture (hWindow);
	lpInfo->bTrack = TRUE;

	if ( GetFocus () != hWindow )
		SetFocus (hWindow);

	lpInfo->bInRect = YES;

	// Increment the state to pick up the next bmp
	lpInfo->State++;
	if (lpInfo->State >= lpInfo->NumDIBs)
		lpInfo->State = 0;

	hLastWnd = hWindow ;
	dwStyle = GetWindowLong(hWindow, GWL_STYLE);
	InvalidateRect(hWindow, NULL, FALSE);
}

/***********************************************************************/
LOCAL void Bmulti_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	DWORD dwStyle;
	LPBMULTICONTINFO lpInfo;
	if (! (lpInfo = (LPBMULTICONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;

	if (! lpInfo->bTrack)
		return;

	ReleaseCapture();
	lpInfo->bTrack = FALSE;
	dwStyle = GetWindowLong(hWindow, GWL_STYLE);
	InvalidateRect(hWindow, NULL, FALSE);
	if (lpInfo->bInRect)
		FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 0, SendMessage);
}

/***********************************************************************/
LOCAL void Bmulti_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 1, SendMessage);
}

#ifdef _DEBUG
/***********************************************************************/
LOCAL void Bmulti_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( CONTROL )
		PrintWindowCoordinates( hWindow );
}
#endif

/***********************************************************************/
LOCAL void Bmulti_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	RECT ClientRect;
	POINT pt;
	DWORD dwStyle;

	LPBMULTICONTINFO lpInfo;
	if (! (lpInfo = (LPBMULTICONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;

	if (! lpInfo->bTrack)
		return;

	GetClientRect( hWindow, &ClientRect );
	pt.x = x;
	pt.y = y;
	if (lpInfo->bInRect == ! PtInRect (& ClientRect, pt) )
	{
		lpInfo->bInRect = ! lpInfo->bInRect;
		dwStyle = GetWindowLong(hWindow, GWL_STYLE);
		InvalidateRect(hWindow, NULL, FALSE);
	}
}

/***********************************************************************/
LOCAL void Bmulti_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
	FORWARD_WM_ENABLE(hWindow, fEnable, DefWindowProc);
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
}

/***********************************************************************/
LOCAL void Bmulti_OnPaletteChanged(HWND hWindow, HWND hwndPaletteChange)
/***********************************************************************/
{
	InvalidateRect(hWindow, NULL, FALSE);
}

/***********************************************************************/
LOCAL long Bmulti_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	long lResult = 0L;
	LPBMULTICONTINFO lpInfo;
	lpInfo = (LPBMULTICONTINFO)GetWindowLong (hWindow, GWL_DATAPTR);

	if (lpInfo)
	{
		switch (msg)
		{
			case MB_GETSTATE:
				lResult = lpInfo->State * lpInfo->Incr;
				break;

			case MB_SETSTATE:
				lpInfo->State = (int)wParam;
				InvalidateRect (hWindow, NULL, FALSE);
				break;

			default:
				break;
		 }
	}
	return lResult;
}

/***********************************************************************/
LONG WINPROC EXPORT BmultiControl(HWND hWindow, UINT message,
			  WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, Bmulti_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Bmulti_OnDestroy);
		HANDLE_MSG(hWindow, WM_WINDOWPOSCHANGING, Bmulti_OnSizeOrPosChange);
		HANDLE_MSG(hWindow, WM_SETFONT, Bmulti_OnSetFont);
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Bmulti_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Bmulti_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_NCHITTEST, Bmulti_OnNCHitTest);
		HANDLE_MSG(hWindow, WM_MOVE, Bmulti_OnMove);
		HANDLE_MSG(hWindow, WM_PAINT, Bmulti_OnPaint);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Bmulti_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Bmulti_OnLButtonUp);
		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Bmulti_OnLButtonDblClk);
		#ifdef _DEBUG
		HANDLE_MSG(hWindow, WM_RBUTTONUP, Bmulti_OnRButtonUp);
		#endif
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Bmulti_OnMouseMove);
		HANDLE_MSG(hWindow, WM_ENABLE, Bmulti_OnEnable);
		HANDLE_MSG(hWindow, WM_PALETTECHANGED, Bmulti_OnPaletteChanged);
		HANDLE_MSG(hWindow, MB_GETSTATE, Bmulti_OnUserMsg);
		HANDLE_MSG(hWindow, MB_SETSTATE, Bmulti_OnUserMsg);

		default:
			return DefWindowProc( hWindow, message, wParam, lParam );
	}
}
