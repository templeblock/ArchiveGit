#include <windows.h>
#include "proto.h"
#include "control.h"
#include "sound.h"
#include "mmsystem.h"
#include "scene.h"
#include "toon.h"
#include "wing.h"

#define HANDLE_SM_GETPOSITION(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)SM_GETPOSITION, wParam, lParam))
#define HANDLE_SM_SETPOSITION(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)SM_SETPOSITION, wParam, lParam))

typedef struct MeterContInfo
{
	RECT	rBitmap;
	PDIB	pdibBack;
	long	Min;	
	long	Max;
	long	lPosition;
	int		State;
	COLORREF rgbColor;
} METERCONTINFO, FAR * LPMETERCONTINFO;

/***********************************************************************/
BOOL Meter_Register( HINSTANCE hInstance )
/***********************************************************************/
{
	WNDCLASS WndClass;
	WndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"meter";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= hInstance;
	WndClass.style			= 0;
	WndClass.lpfnWndProc	= MeterControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 16;

	return( RegisterClass( &WndClass ) );
}

//************************************************************************
long Meter_Set( HWND hWnd, ITEMID id, long lValue )
//************************************************************************
{
	if ( lValue > METER_MAX )
		lValue = METER_MAX;
	if ( lValue < 0 )
		lValue = 0;
	SendDlgItemMessage( hWnd, id, SM_SETPOSITION, 0, lValue );
	return( lValue );
}

/***********************************************************************/
static void Meter_Init(LPMETERCONTINFO lpInfo)
/***********************************************************************/
{
	lpInfo->pdibBack = NULL;
	lpInfo->Min = 0;
	lpInfo->Max = METER_MAX;
	lpInfo->lPosition = 0;
	lpInfo->State = 0;
	lpInfo->rgbColor = RGB(255,255,0);
}

/***********************************************************************/
static void Meter_UpdateText(HWND hWindow, LPMETERCONTINFO lpInfo)
/***********************************************************************/
{
	long lPercent;
	STRING szString;

	lPercent = (100L * lpInfo->lPosition) / (lpInfo->Max - lpInfo->Min);
	wsprintf(szString, "%ld%%", lPercent);
	SetWindowText(hWindow, szString);
}

/***********************************************************************/
static BOOL Meter_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPMETERCONTINFO lpInfo;
	if ( (lpInfo = (LPMETERCONTINFO)AllocX(sizeof(METERCONTINFO), GMEM_ZEROINIT|GMEM_SHARE) ))
	{
		Meter_Init(lpInfo);
		Meter_UpdateText(hWindow, lpInfo);
		SetWindowLong(hWindow, GWL_DATAPTR, (long)lpInfo);
	}
	else
		return FALSE;

	return TRUE;
}

/***********************************************************************/
LOCAL void Meter_OnDraw(HWND hWindow, HDC hDC, LPRECT lpRect, BOOL fHighlight)
/***********************************************************************/
{
	RECT	rSrcArea, rDstArea, rControlArea;
	POINT	ptDst;
	PDIB	pdibSrc, pdibDst, pdibSrc2, pdibTmp;
	UINT	id;
	LPOFFSCREEN lpOffScreen;
	LPSCENE lpScene;
	HDC		hWinGDC;
	HPALETTE hLoadPal;

	lpScene = CScene::GetScene( GetParent(hWindow) );
	if (!lpScene)
		return;

	LPMETERCONTINFO lpInfo;
	if (!(lpInfo = (LPMETERCONTINFO)GetWindowLong(hWindow, GWL_DATAPTR) ))
		return;

	rSrcArea = * lpRect;
	rDstArea = * lpRect;
	rControlArea = * lpRect;

	lpOffScreen = lpScene->GetOffScreen();
	if (lpOffScreen)
	{
		MapWindowPoints(hWindow, GetParent(hWindow), (LPPOINT)& rDstArea, 2);
		if (!(pdibSrc = lpOffScreen->GetReadOnlyDIB() ) )
			return;
		if (!(pdibDst = lpOffScreen->GetWritableDIB() ) )
			return;
		hWinGDC = lpOffScreen->GetDC();
		hLoadPal = GetApp()->m_hPal;
	}
	else
	{
		return;
	}

	RECT	rClient;
	GetClientRect(hWindow, & rClient);
	MapWindowPoints(hWindow, GetParent(hWindow), (LPPOINT)& rClient, 2 );

	// Get the control style
	DWORD dwStyle = GetWindowStyle(hWindow);
	BOOL bVert = (dwStyle & MC_VERT) ? TRUE : FALSE ;

	// Get the control id
	if (!(id = GetWindowWord(hWindow, GWW_ICONID) ) )
		id = GET_WINDOW_ID(hWindow);

	// Refresh the background,
	// compressed DIBs must use GDI copying (lose transparency ability)
	if (pdibSrc->GetCompression() == BI_RLE8 ||
		pdibDst->GetCompression() == BI_RLE8)
	{
		pdibSrc->DCBlt(hWinGDC,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top );
	}
	else
	{
		pdibSrc->DibBlt(pdibDst,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
						rDstArea.left, rDstArea.top,
						rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
						NO /*bTransparent*/ );
	}

	// Load the resource
	if (pdibSrc = CDib::LoadDibFromResource(GetWindowInstance(hWindow),
		MAKEINTRESOURCE(id), hLoadPal, (dwStyle & BS_MASK) != 0) )
	{
		if (pdibSrc2 = CDib::LoadDibFromResource(GetWindowInstance(hWindow),
			MAKEINTRESOURCE(id + 1), hLoadPal, (dwStyle & BS_MASK) != 0) )
		{
			int iBitmapWidth = pdibSrc->GetWidth();
			int iBitmapHeight = pdibSrc->GetHeight();

			rSrcArea.left = 0;
			rSrcArea.top = 0;
			rSrcArea.right = rSrcArea.left + iBitmapWidth;
			rSrcArea.bottom = rSrcArea.top + iBitmapHeight;
			rDstArea = rClient;
			rDstArea.right = rDstArea.left + iBitmapWidth;
			rDstArea.bottom = rDstArea.top + iBitmapHeight;

			// Get the position width
			int iPosWidth;
			if (bVert)
			{
				iPosWidth = (int)( (float)iBitmapHeight * 
					((float)lpInfo->lPosition / ((float)lpInfo->Max - (float)lpInfo->Min)));
				iPosWidth = iBitmapHeight - iPosWidth;
			}
			else
			{
				iPosWidth = (int)( (float)iBitmapWidth *
					((float)lpInfo->lPosition / ((float)lpInfo->Max - (float)lpInfo->Min)));
			}
			// compressed DIBs must use GDI copying (lose transparency ability)
			if (pdibSrc->GetCompression() == BI_RLE8 ||
				pdibDst->GetCompression() == BI_RLE8 )
			{
				pdibSrc->DCBlt(hWinGDC,
					rDstArea.left, rDstArea.top,
					rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
					rSrcArea.left, rSrcArea.top,
					rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top);
			}
			else
			{
				RGBTRIPLE rgb;
				LPRGBTRIPLE lpRGB = NULL;
				if (dwStyle & BS_MASK)
				{
					STRING szColor;
					GetWindowText(hWindow, szColor, sizeof(szColor));
					AsciiRGB(szColor, & rgb);
					if (fHighlight)
					{
						// this relies on the fact that AsciiRGB replaces commas
						// with NULL terminators
						LPTSTR lp = szColor + lstrlen(szColor) + 1; // passed red
						lp += lstrlen(lp) + 1; // passed green
						lp += lstrlen(lp) + 1; // passed blue to higlight color
						AsciiRGB(lp, & rgb);
					}
					lpRGB = & rgb;
				}
				// Render the first bitmap
				pdibTmp = pdibSrc;
				pdibTmp->DibBlt(pdibDst,
					rDstArea.left, rDstArea.top,
					rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
					rSrcArea.left, rSrcArea.top,
					rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top,
					FALSE /*bTransparent*/, lpRGB);				
			}

			// Adjust the source and dest rect's and render the 2nd bitmap
			
			if (bVert)
			{	
				//rDstArea.top = rDstArea.top;// iPosWidth;
				rDstArea.bottom = rDstArea.top + iPosWidth;
				//rSrcArea.top = rSrcArea.bottom - iPosWidth;
				rSrcArea.bottom = iPosWidth;
				}
			else
				{
				rDstArea.right = rDstArea.left + iPosWidth;
				rSrcArea.right = rSrcArea.left + iPosWidth;
				}
			// compressed DIBs must use GDI copying (lose transparency ability)
			if (pdibSrc->GetCompression() == BI_RLE8 ||
				pdibDst->GetCompression() == BI_RLE8 )
			{
				pdibSrc->DCBlt(hWinGDC,
					rDstArea.left, rDstArea.top,
					rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
			rSrcArea.left, rSrcArea.top,
					rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top);
			}
			else
			{
				RGBTRIPLE rgb;
				LPRGBTRIPLE lpRGB = NULL;
				if (dwStyle & BS_MASK)
				{
					STRING szColor;
					GetWindowText(hWindow, szColor, sizeof(szColor));
					AsciiRGB(szColor, & rgb);
					if (fHighlight)
					{
						// this relies on the fact that AsciiRGB replaces commas
						// with NULL terminators
						LPTSTR lp = szColor + lstrlen(szColor) + 1; // passed red
						lp += lstrlen(lp) + 1; // passed green
						lp += lstrlen(lp) + 1; // passed blue to higlight color
						AsciiRGB(lp, & rgb);
					}
					lpRGB = & rgb;
				}
				// Render the first bitmap
				pdibTmp = pdibSrc2;
				pdibTmp->DibBlt(pdibDst,
					rDstArea.left, rDstArea.top,
					rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
					rSrcArea.left, rSrcArea.top,
					rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top,
					FALSE /*bTransparent*/, lpRGB);

			}
		}
		delete pdibSrc;
		delete pdibSrc2;
	}

	ptDst.x = rControlArea.left;
	ptDst.y = rControlArea.top;
	if (lpOffScreen)
	{
		lpOffScreen->DrawRect(hDC, & rClient, & ptDst);
	}
}

/***********************************************************************/
LOCAL BOOL SetupMeterControlInfo(HWND hControl, LPINT lpDX, LPINT lpDY)
/***********************************************************************/
{
	ITEMID idControl;
	LPBITMAPINFO lpDIB;
	HGLOBAL hResource;

	LPMETERCONTINFO lpInfo;
	if (!(lpInfo = (LPMETERCONTINFO)GetWindowLong(hControl, GWL_DATAPTR) ) )
		return FALSE;

	if (!(idControl = GetWindowWord(hControl, GWW_ICONID)))
		idControl = GET_WINDOW_ID(hControl);

	if (!(hResource = (HGLOBAL)FindResource(GetWindowInstance(hControl),
		MAKEINTRESOURCE(idControl), RT_BITMAP) ) )
		return(FALSE);

	if (!(hResource = LoadResource(GetWindowInstance (hControl), (HRSRC)hResource) ) )
		return (FALSE);

	if (! (lpDIB = (LPBITMAPINFO)LockResource (hResource) ) )
		return (FALSE);

	// Make sure that the DIB is uncompressed
	if (lpDX)
		* lpDX = (int)lpDIB->bmiHeader.biWidth;
	if (lpDY)
		* lpDY = (int)abs (lpDIB->bmiHeader.biHeight);

	// Set up the DIB rectangle
	if (lpInfo)
	{
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
LOCAL void Meter_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPMETERCONTINFO lpInfo;
	if (! (lpInfo = (LPMETERCONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;

	// Delete the background bmp
	if (lpInfo->pdibBack)
	{
		delete lpInfo->pdibBack;
		lpInfo->pdibBack = NULL;
	}

	FreeUp (lpInfo);
}

/***********************************************************************/
LOCAL void Meter_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
	int dx, dy;

	LPMETERCONTINFO lpInfo;
	if (! (lpInfo = (LPMETERCONTINFO)GetWindowLong (hWindow, GWL_DATAPTR) ) )
		return;

	if (SetupMeterControlInfo (hWindow, & dx, & dy) )
	{
		// Delete any existing background bmp
		if (lpInfo->pdibBack)
		{
			delete lpInfo->pdibBack;
			lpInfo->pdibBack = NULL;
		}
		// Instantiate a new background bmp
		lpInfo->pdibBack = new CDib();
		if (lpInfo->pdibBack)
		{
//			lpInfo->pdibBack->Create (8, cx, cy);
			lpInfo->pdibBack->Create (8, dx, dy);
		}
		ResizeControl (hWindow, dx, dy);
//		ResizeControl (hWindow, cx, cy);

		FORWARD_WM_SIZE (hWindow, state, dx, dy, DefWindowProc);
	}
	else
	{
		FORWARD_WM_SIZE (hWindow, state, cx, cy, DefWindowProc);
	}
}

/***********************************************************************/
LOCAL void Meter_OnSetFont(HWND hWindow, HFONT hFont, BOOL fRedraw)
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
LOCAL UINT Meter_OnGetDlgCode(HWND hWindow, LPMSG lpmsg)
/***********************************************************************/
{
	return( DLGC_WANTARROWS );
}

/***********************************************************************/
LOCAL BOOL Meter_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
LOCAL UINT Meter_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
	UINT uReturn = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if ( SHIFT && (uReturn == HTCLIENT) )
		uReturn = HTCAPTION;
	return( uReturn );
}

/***********************************************************************/
LOCAL void Meter_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
}

/***********************************************************************/
LOCAL void Meter_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;

	if (hDC = BeginPaint (hWindow, & ps))
	{
		Meter_OnDraw (hWindow, hDC, & ps.rcPaint, FALSE);
		EndPaint (hWindow, & ps);
	}
}

/***********************************************************************/
LOCAL void Meter_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
	FORWARD_WM_ENABLE(hWindow, fEnable, DefWindowProc);
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
}

/***********************************************************************/
LOCAL void Meter_OnPaletteChanged(HWND hWindow, HWND hwndPaletteChange)
/***********************************************************************/
{
	InvalidateRect(hWindow, NULL, FALSE);
	UpdateWindow(hWindow);
}

/***********************************************************************/
LOCAL long Meter_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	long lResult = 0L;
	LPMETERCONTINFO lpInfo;
	lpInfo = (LPMETERCONTINFO)GetWindowLong (hWindow, GWL_DATAPTR);

	if (lpInfo)
	{
		switch (msg)
		{
			case SM_GETPOSITION:
				lResult = lpInfo->lPosition;
				break;

			case SM_SETPOSITION:
				lpInfo->lPosition = lParam;
				// Enforce min/max range
				if (lpInfo->lPosition < lpInfo->Min)
					lpInfo->lPosition = lpInfo->Min;
				if (lpInfo->lPosition > lpInfo->Max)
					lpInfo->lPosition = lpInfo->Max;
				Meter_UpdateText(hWindow, lpInfo);
				InvalidateRect (hWindow,NULL,FALSE);
				UpdateWindow( hWindow );
				break;

			default:
				break;
		}
	}
	return lResult;
}

//************************************************************************
long Meter_PostSet( HWND hWnd, ITEMID id, long lValue )
//************************************************************************
{
	HWND hItem;

	if ( lValue > METER_MAX )
		lValue = METER_MAX;
	if ( lValue < 0 )
		lValue = 0;
	hItem = GetDlgItem( hWnd, id );
	PostMessage( hItem, SM_SETPOSITION, 0, lValue );
	return( lValue );
}

/***********************************************************************/
LONG WINPROC EXPORT MeterControl(HWND hWindow, UINT message,
			WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (message)
	{
		HANDLE_MSG(hWindow, WM_CREATE, Meter_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Meter_OnDestroy);
		HANDLE_MSG(hWindow, WM_SIZE, Meter_OnSize);
		HANDLE_MSG(hWindow, WM_SETFONT, Meter_OnSetFont);
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Meter_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Meter_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_NCHITTEST, Meter_OnNCHitTest);
		HANDLE_MSG(hWindow, WM_MOVE, Meter_OnMove);
		HANDLE_MSG(hWindow, WM_PAINT, Meter_OnPaint);
		HANDLE_MSG(hWindow, WM_ENABLE, Meter_OnEnable);
		HANDLE_MSG(hWindow, WM_PALETTECHANGED, Meter_OnPaletteChanged);
		HANDLE_MSG(hWindow, SM_GETPOSITION, Meter_OnUserMsg);
		HANDLE_MSG(hWindow, SM_SETPOSITION, Meter_OnUserMsg);

		default:
			return DefWindowProc (hWindow, message, wParam, lParam);
	}
}

