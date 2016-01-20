#include <windows.h>
#include "proto.h"
#include "control.h"
#include "sound.h"
#include "mmsystem.h"
#include "scene.h"
#include "toon.h"
#include "wing.h"

#define SM_SETSTATE (WM_USER + 100)
#define HANDLE_SM_SETSTATE(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)SM_SETSTATE, wParam, lParam))

// DIBCONTINFO State settings
//	0 = default setting, bitmap ID same as control ID
//	1 = second bitmap, ID + 1 (i.e., raised)
//	2 = third bitmap, ID + 2 (i.e.,	 depressed)
typedef struct DIBContInfo
{
	BOOL	bTrack;
	BOOL	bInRect;
} DIBCONTINFO, FAR * LPDIBCONTINFO;

/***********************************************************************/
LOCAL void Bitmap_PlaySound( HWND hWindow, UINT msg )
/***********************************************************************/
{
	ITEMID id;
	if ( !(id = GetWindowWord( hWindow, GWW_ICONID )) )
		if ( !(id = GET_WINDOW_ID( hWindow )) )
			return;

	HWND hVideoWnd;
	if ( hVideoWnd = FindClassDescendent( GetParent(hWindow), "video" ) )
	{	// Set the audio off
		FORWARD_WM_KEYDOWN( hVideoWnd, '1', 0/*cRepeat*/, 0/*flags*/, SendMessage);
	}

	LPSCENE lpScene = CScene::GetScene(GetParent(hWindow));
	if ( lpScene && lpScene->GetSound())
		lpScene->GetSound()->StartResourceID(id, NO/*bLoop*/, -1/*iChannel*/, GetWindowInstance(hWindow), YES/*bWait*/ );

//this method doesn't pause the video nearly as bad as the above method of playing the sounds
//	STRING szAlias;
//	HMCI hWAV;
//	wsprintf( szAlias, "Alias%ld", (LPSTR)hWindow );
//	if ( hWAV = MCIOpen( GetApp()->m_hDeviceWAV, "che.wav", szAlias ) )
//	{
//		MCIPlay( hWAV, NULL/*Wait*/ );
//		MCIClose( hWAV );
//	}

	if (hVideoWnd)
		FORWARD_WM_KEYDOWN( hVideoWnd, '2', 0/*cRepeat*/, 0/*flags*/, SendMessage);
}

/***********************************************************************/
static BOOL Bitmap_OnCreate (HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPDIBCONTINFO lpInfo;
	if ( !(lpInfo = (LPDIBCONTINFO)AllocX( sizeof(DIBCONTINFO), GMEM_ZEROINIT|GMEM_SHARE )) )
		return FALSE;

	lpInfo->bTrack = FALSE;
	lpInfo->bInRect = FALSE;
	SetWindowLong( hWindow, GWL_DATAPTR, (long)lpInfo );
	SetWindowWord( hWindow, GWW_ICONID, 0 );
	SetWindowWord( hWindow, GWW_STATE, 0 );
	return TRUE;
}

/***********************************************************************/
LOCAL void Bitmap_Paint(HWND hWindow, HDC hDC, LPRECT lpRect)
/***********************************************************************/
{
	RECT rSrcArea, rDstArea;
	POINT ptDst;
	PDIB pdibSrc, pdibDst;
	BOOL fUseScene = TRUE;
	#ifdef USE_WINTOON
	HPALETTE hDisplayPal;
	PTOON pToon;
	#endif

	rSrcArea = *lpRect;
	rDstArea = *lpRect;

	LPSCENE lpScene = CScene::GetScene(GetParent(hWindow));
	if ( !lpScene )
		return;

	HPALETTE hLoadPal;	
	HDC hWinGDC;
	if ( lpScene->GetDIBs( &pdibSrc, &pdibDst, &hWinGDC ) )
	{
		if ( !pdibDst )
			return;
		hLoadPal = GetApp()->m_hPal;
		MapWindowPoints( hWindow, GetParent(hWindow), (LPPOINT)&rDstArea, 2 );
	}
	else
	#ifdef USE_WINTOON // don't need wintoon.lib if you don't want it
	{
		//RGBQUAD rgbQuad[256];
		BITMAPINFOHEADER bmi;
		LPTR lp;

		pToon = lpScene->GetToon();
		if (!pToon)
			return;
		fUseScene = FALSE;
		MapWindowPoints( hWindow, pToon->GetWindow(), (LPPOINT)&rDstArea, 2 );
		pdibSrc = pToon->GetStageDib();
		if (!pdibSrc)
			return;

		lp = ToonGetDIBPointer(pToon->GetToonHandle(), &bmi);
		//ToonGetColors(pToon->GetToonHandle(), 0, 256, pdibSrc->GetColors());
		//bmi.biClrUsed = 256;
		//pdibDst = new CDib(&bmi, rgbQuad, lp);
		pdibDst = new CDib(pdibSrc, lp, FALSE);
		if (!pdibDst)
			return;
		hWinGDC = ToonDC(pToon->GetToonHandle());
		// to make sure we don't change the system palette
		hLoadPal = pToon->GetPalette();
		// we can't map to any colors in this palette since WinToon sets
		// PC_RESERVED in its palette
		hDisplayPal = hLoadPal;
	}
	#else
	{
		return;
	}
	#endif

	LPDIBCONTINFO lpInfo;
	if ( !(lpInfo = (LPDIBCONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;

	BOOL bHasFocus = ( GetFocus() == hWindow );
	int iState = GetWindowWord(hWindow, GWW_STATE );
	BOOL bDown = ( iState || (lpInfo->bTrack && lpInfo->bInRect && bHasFocus) );
	DWORD dwStyle = GetWindowLong( hWindow, GWL_STYLE );

	// Draw the Bitmap
	UINT id;
	if ( !(id = GetWindowWord( hWindow, GWW_ICONID )) )
		id = GET_WINDOW_ID( hWindow );

	if ( iState )
		id += iState;
	else
	if ( dwStyle & BS_DUAL )
		id += (bDown != 0);

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

	#ifdef USE_WINTOON
	else // WinToon
	{
		// this creative code is here for the following purpose:
		// there is no way to get the WinToon HPALETTE and the
		// HPALETTE WinToon uses has PC_RESERVED entries.  This means
		// no matter what palette I select and realize, colors will
		// only map to the 20 static colors.  So instead, WinToon is
		// used to draw into the area occupied by the bitmap
		// control by call GetDCEx() specifying the clipping region.

		// get toon DC with bitmap control not clipped out, also
		// the only thing clipped in
		//HRGN hRgn= CreateRectRgn(rDstArea.left, rDstArea.top, rDstArea.right, rDstArea.bottom);
		//HDC hToonDC = GetDCEx(pToon->GetWindow(), hRgn, DCX_CACHE);
		HDC hToonDC = NULL;
		//if (hToonDC)
		//{ // call WinToon to do painting for us
		//	ToonPaintDC(pToon->GetToonHandle(), hToonDC);
		//	ReleaseDC(pToon->GetWindow(), hToonDC);
		//}
		//DeleteObject(hRgn);

		if (!hToonDC)
		{ // if we ran out of DC's
			HPALETTE hOldPal;
		
			if (hDisplayPal)
			{
				// map to background palette
				hOldPal = SelectPalette(hDC, hDisplayPal, TRUE);
				RealizePalette(hDC);
			}
			//pdibDst->DCBlt( hDC,
			//	ptDst.x, ptDst.y,
			//	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
			//	rDstArea.left, rDstArea.top,
			//	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top );
			WinGBitBlt( hDC,
				ptDst.x, ptDst.y,
				rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
				hWinGDC,
				rDstArea.left, rDstArea.top);
			if (hDisplayPal)
				SelectPalette(hDC, hOldPal, TRUE);
		}
	}
	#endif
}

/***********************************************************************/
LOCAL void Bitmap_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;

	if ( !(hDC = BeginPaint( hWindow, &ps )) )
		return;
	Bitmap_Paint(hWindow, hDC, &ps.rcPaint);
	EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Bitmap_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPDIBCONTINFO lpInfo;
	if ( !(lpInfo = (LPDIBCONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;
	if (lpInfo->bTrack)
	{
		ReleaseCapture();
		lpInfo->bTrack = FALSE;
	}
	FreeUp (lpInfo);
}

/***********************************************************************/
LOCAL BOOL Bitmap_ComputeSize( HWND hControl, LPINT lpDX, LPINT lpDY )
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
static BOOL Bitmap_OnSizeOrPosChange( HWND hWindow, LPWINDOWPOS lpWindowPos )
/***********************************************************************/
{
	if ( lpWindowPos->flags & SWP_NOSIZE )
		return( YES );

	int cx, cy;
	if ( !Bitmap_ComputeSize( hWindow, &cx, &cy ) )
		return( YES );

	if ( lpWindowPos->cx == cx && lpWindowPos->cy == cy )
		return( YES );

	lpWindowPos->cx = cx;
	lpWindowPos->cy = cy;
	return( NO );
}

/***********************************************************************/
LOCAL void Bitmap_OnSetFont(HWND hWindow, HFONT hFont, BOOL fRedraw)
/***********************************************************************/
{
	SetWindowLong(hWindow, GWL_FONT, (long)(LPTR)hFont);
	if (fRedraw)
	{
		InvalidateRect(hWindow, NULL, TRUE);
		UpdateWindow(hWindow);
	}
}

/***********************************************************************/
LOCAL UINT Bitmap_OnGetDlgCode(HWND hWindow, LPMSG lpmsg)
/***********************************************************************/
{
	return( DLGC_WANTARROWS );
}

/***********************************************************************/
LOCAL BOOL Bitmap_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
LOCAL UINT Bitmap_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
	UINT uReturn = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if ( SHIFT && (uReturn == HTCLIENT) )
		uReturn = HTCAPTION;
	return( uReturn );
}

/***********************************************************************/
LOCAL void Bitmap_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
}

/***********************************************************************/
LOCAL void Bitmap_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( SHIFT )
		return;
	LPDIBCONTINFO lpInfo;
	if ( !(lpInfo = (LPDIBCONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;
	if (lpInfo->bTrack)
		return;
	SetCapture (hWindow);
	lpInfo->bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus (hWindow);
	lpInfo->bInRect = YES;
	InvalidateRect(hWindow, NULL, FALSE);
	UpdateWindow(hWindow);
}

/***********************************************************************/
LOCAL void Bitmap_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	LPDIBCONTINFO lpInfo;
	if ( !(lpInfo = (LPDIBCONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;

	if ( !lpInfo->bTrack)
		return;

	ReleaseCapture();
	lpInfo->bTrack = FALSE;
	InvalidateRect(hWindow, NULL, FALSE);
	// Don't UpdateWindow() yet since the app may want to keep it down
	if (lpInfo->bInRect)
	{
		Bitmap_PlaySound( hWindow, WM_LBUTTONDOWN );
		FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 0, SendMessage);
	}
}

/***********************************************************************/
LOCAL void Bitmap_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
//	FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 1, SendMessage);
}

#ifdef _DEBUG
/***********************************************************************/
LOCAL void Bitmap_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( CONTROL )
		PrintWindowCoordinates( hWindow );
}
#endif

/***********************************************************************/
LOCAL void Bitmap_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	RECT ClientRect;
	POINT pt;

	LPDIBCONTINFO lpInfo;
	if ( !(lpInfo = (LPDIBCONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;

	if ( !lpInfo->bTrack)
		return;

	GetClientRect( hWindow, &ClientRect );
	pt.x = x;
	pt.y = y;
	if (lpInfo->bInRect ==  !PtInRect (&ClientRect, pt) )
	{
		lpInfo->bInRect =  !lpInfo->bInRect;
		InvalidateRect(hWindow, NULL, FALSE);
		UpdateWindow(hWindow);
	}
}

/***********************************************************************/
LOCAL void Bitmap_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
	FORWARD_WM_ENABLE(hWindow, fEnable, DefWindowProc);
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
}

/***********************************************************************/
LOCAL void Bitmap_OnPaletteChanged(HWND hWindow, HWND hwndPaletteChange)
/***********************************************************************/
{
	InvalidateRect(hWindow, NULL, FALSE);
}

/***********************************************************************/
LOCAL long Bitmap_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	long lResult = 0L;
	LPDIBCONTINFO lpInfo;
	lpInfo = (LPDIBCONTINFO)GetWindowLong (hWindow, GWL_DATAPTR);

	if (lpInfo)
	{
		switch (msg)
		{
			case SM_SETSTATE:
				SetWindowWord(hWindow, GWW_STATE, (int)wParam );
				InvalidateRect (hWindow, NULL, FALSE);
				UpdateWindow(hWindow);
				break;

			default:
				break;
		 }
	}
	return 0L;
}

/***********************************************************************/
LONG WINPROC EXPORT BitmapControl(HWND hWindow, UINT message,
			  WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, Bitmap_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Bitmap_OnDestroy);
		HANDLE_MSG(hWindow, WM_WINDOWPOSCHANGING, Bitmap_OnSizeOrPosChange);
		HANDLE_MSG(hWindow, WM_SETFONT, Bitmap_OnSetFont);
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Bitmap_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Bitmap_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_NCHITTEST, Bitmap_OnNCHitTest);
		HANDLE_MSG(hWindow, WM_MOVE, Bitmap_OnMove);
		HANDLE_MSG(hWindow, WM_PAINT, Bitmap_OnPaint);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Bitmap_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Bitmap_OnLButtonUp);
		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Bitmap_OnLButtonDblClk);
		#ifdef _DEBUG
		HANDLE_MSG(hWindow, WM_RBUTTONUP, Bitmap_OnRButtonUp);
		#endif
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Bitmap_OnMouseMove);
		HANDLE_MSG(hWindow, WM_ENABLE, Bitmap_OnEnable);
		HANDLE_MSG(hWindow, WM_PALETTECHANGED, Bitmap_OnPaletteChanged);
		HANDLE_MSG(hWindow, SM_SETSTATE, Bitmap_OnUserMsg);

		default:
			return DefWindowProc( hWindow, message, wParam, lParam );
	}
}
