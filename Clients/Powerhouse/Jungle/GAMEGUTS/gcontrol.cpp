#include <windows.h>
#include <math.h>
#include "proto.h"
#include "control.h"
#include "sound.h"
#include "mmsystem.h"
#include "scene.h"

#define SM_GETPOSITION	(WM_USER + 700)
#define SM_SETPOSITION	(WM_USER + 701)
#define HANDLE_SM_GETPOSITION(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)SM_GETPOSITION, wParam, lParam))
#define HANDLE_SM_SETPOSITION(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)SM_SETPOSITION, wParam, lParam))

typedef struct GaugeInfo
{
	long	Min;	
	long	Max;
	long	Position;
	int		ArmLength;
	BOOL	bTrack;
	BOOL	bInRect;
	BOOL	bFocus;
	COLORREF rgbArmColor;
//	COLORREF rgbCircleColor;
//	COLORREF rgbEndCircleColor;
} HGAUGEINFO, FAR * LPHGAUGEINFO;


//************************************************************************
long Gauge_Set( HWND hWnd, ITEMID id, long lValue )
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
LOCAL void Gauge_Init (LPHGAUGEINFO lpInfo)
/***********************************************************************/
{
	lpInfo->Min	  	= 0;
	lpInfo->Max	  	= METER_MAX;
	lpInfo->Position	= 0;
	lpInfo->bTrack	= FALSE;
	lpInfo->bInRect	= FALSE;
	lpInfo->bFocus	= FALSE;
	lpInfo->rgbArmColor = RGB(255, 0, 0);
//	lpInfo->rgbEndCircleColor = RGB(204, 255, 0);
//	lpInfo->rgbCircleColor = RGB(204, 255, 0);
}

/***********************************************************************/
static BOOL Gauge_OnCreate (HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPHGAUGEINFO lpInfo;
	if ( (lpInfo = (LPHGAUGEINFO)AllocX (sizeof(HGAUGEINFO), GMEM_ZEROINIT|GMEM_SHARE )) )
	{
		Gauge_Init (lpInfo) ;
		SetWindowLong (hWindow, GWL_DATAPTR, (long)lpInfo);
	}
	else
		return FALSE;
	
	return TRUE;
}

/***********************************************************************/
LOCAL LPHGAUGEINFO Gauge_GetInfo (HWND hWindow)
/***********************************************************************/
{
	return ((LPHGAUGEINFO)GetWindowLong (hWindow, GWL_DATAPTR));
}

/***********************************************************************/
LOCAL void GetEndPoint (LPHGAUGEINFO lpInfo, POINT ptOrigin, LPINT iX, LPINT iY)
/***********************************************************************/
{
	if (! lpInfo)
		return;
	
	if (lpInfo->Position == 0)
	{
		* iX = ptOrigin.x - lpInfo->ArmLength;
		* iY = ptOrigin.y;
		return;
	}
	else
	if (lpInfo->Position == lpInfo->Max)
	{
		* iX = ptOrigin.x + lpInfo->ArmLength;
		* iY = ptOrigin.y;
		return;
	}
	
	int iAngle = (int)(((float)lpInfo->Position / (float)lpInfo->Max) * (float)180);
	float radianAngle = ((float)iAngle * (float)3.141592654) / (float)180;
	int iEndY = (int)( (float)ptOrigin.y - ((float)lpInfo->ArmLength * (float)sin (radianAngle)) ) ;
	int iEndX = (int)( (float)ptOrigin.x - ((float)lpInfo->ArmLength * (float)cos (radianAngle)) ) ;
	
	* iX = iEndX;
	* iY = iEndY;
}

/***********************************************************************/
LOCAL void Gauge_Paint (HWND hWindow, HDC hDC, LPRECT lpRect, BOOL fHighlight)
/***********************************************************************/
{
	if ( !hDC )
		return;

	LPSCENE lpScene = CScene::GetScene( GetParent(hWindow) );
	if ( lpScene )
	{
		RECT rect = *lpRect;
		MapWindowPoints( hWindow, GetParent(hWindow), (LPPOINT)&rect, 2 );
		lpScene->Paint( hDC, &rect, (LPPOINT)lpRect );
	}

	LPHGAUGEINFO lpInfo;
	if ( !(lpInfo = Gauge_GetInfo (hWindow) ) )
		return;

	RECT  rClient;
	GetClientRect (hWindow, & rClient);
	int iX, iY;
	int iXCenter = rClient.left + ((rClient.right - rClient.left) / 2);
	POINT ptOrigin;
	ptOrigin.x = iXCenter;
	ptOrigin.y = rClient.bottom - 2;

	HPEN hPen = CreatePen (PS_SOLID, 2, lpInfo->rgbArmColor);
	HPEN hOldPen = (HPEN)SelectObject (hDC, hPen);

	// Position to the arm origin and draw the arm
	MoveToEx (hDC, ptOrigin.x, ptOrigin.y, NULL);
	GetEndPoint (lpInfo, ptOrigin, & iX, & iY);
	LineTo (hDC, iX, iY);
	SelectObject (hDC, hOldPen);
	DeleteObject (hPen);

//	HBRUSH hBrush, hBrushOld;
//	LOGBRUSH lb;
//	// Draw a circle at the origin
//	lb.lbStyle = BS_SOLID;
//	lb.lbColor = lpInfo->rgbCircleColor;
//	lb.lbHatch = 0;
//	hBrush = CreateBrushIndirect (& lb);
//	hBrushOld = (HBRUSH)SelectObject (hDC, hBrush);
//	Ellipse (hDC, ptOrigin.x - 4, ptOrigin.y - 4, ptOrigin.x + 4, ptOrigin.y + 4);
//	SelectObject (hDC, hBrushOld);
//	DeleteObject (hBrush);

//	// Draw a circle at the end of the arm
//	lb.lbStyle = BS_SOLID;
//	lb.lbColor = lpInfo->rgbEndCircleColor;
//	lb.lbHatch = 0;
//	hBrush = CreateBrushIndirect (& lb);
//	hBrushOld = (HBRUSH)SelectObject (hDC, hBrush);
//	Ellipse (hDC, iX - 3, iY - 3, iX + 3, iY + 3);
//	SelectObject (hDC, hBrushOld);
//	DeleteObject (hBrush);
}

/***********************************************************************/
LOCAL void Gauge_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	LPHGAUGEINFO lpInfo;
	if (! (lpInfo = Gauge_GetInfo (hWindow) ) )
		return;
	if (lpInfo->bTrack)
	{
		ReleaseCapture();
		lpInfo->bTrack = NO;
	}
	FreeUp (lpInfo);
}

/***********************************************************************/
LOCAL void Gauge_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
	LPHGAUGEINFO lpInfo;
	
	ResizeControl (hWindow, cx, cy);
	if ( (lpInfo = Gauge_GetInfo (hWindow) ) )
		lpInfo->ArmLength = cy - 8;
	FORWARD_WM_SIZE(hWindow, state, cx, cy, DefWindowProc);
}

/***********************************************************************/
LOCAL BOOL Gauge_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return (TRUE);
}

/***********************************************************************/
LOCAL UINT Gauge_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
	UINT uReturn = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if ( SHIFT && (uReturn == HTCLIENT) )
		uReturn = HTCAPTION;
	return (uReturn);
}

/***********************************************************************/
LOCAL void Gauge_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
	InvalidateRect (hWindow, NULL, FALSE);
}

/***********************************************************************/
LOCAL void Gauge_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;

	if ( !(hDC = BeginPaint (hWindow, &ps)) )
		return;
	Gauge_Paint( hWindow, hDC, &ps.rcPaint, FALSE );
	EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Gauge_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
}

/***********************************************************************/
LOCAL void Gauge_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
//	FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 0, SendMessage);
}

/***********************************************************************/
LOCAL void Gauge_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
//	FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 1, SendMessage);
}

/***********************************************************************/
LOCAL void Gauge_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	LPHGAUGEINFO lpInfo;
	if (! (lpInfo = Gauge_GetInfo (hWindow) ) )
		return;
	if (! lpInfo->bTrack)
		return;
	
	RECT ClientRect;
	GetClientRect (hWindow, & ClientRect);
	POINT pt;
	pt.x = x;
	pt.y = y;
	if (lpInfo->bInRect == ! PtInRect (& ClientRect, pt ) )
		lpInfo->bInRect = ! lpInfo->bInRect;
}

/***********************************************************************/
LOCAL void Gauge_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
	FORWARD_WM_ENABLE (hWindow, fEnable, DefWindowProc);
	InvalidateRect (hWindow, NULL, FALSE);
	UpdateWindow (hWindow);
}

/***********************************************************************/
LOCAL void Gauge_OnPaletteChanged (HWND hWindow, HWND hwndPaletteChange)
/***********************************************************************/
{
	InvalidateRect (hWindow, NULL, FALSE);
}

/***********************************************************************/
LOCAL long Gauge_OnUserMsg (HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
	long lResult = 0L;
	LPHGAUGEINFO lpInfo = Gauge_GetInfo (hWindow);

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
LONG WINPROC EXPORT GaugeControl(HWND hWindow, UINT message,
			  WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, Gauge_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Gauge_OnDestroy);
		HANDLE_MSG(hWindow, WM_SIZE, Gauge_OnSize);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Gauge_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_NCHITTEST, Gauge_OnNCHitTest);
		HANDLE_MSG(hWindow, WM_MOVE, Gauge_OnMove);
		HANDLE_MSG(hWindow, WM_PAINT, Gauge_OnPaint);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Gauge_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Gauge_OnLButtonUp);
		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Gauge_OnLButtonDblClk);
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Gauge_OnMouseMove);
		HANDLE_MSG(hWindow, WM_ENABLE, Gauge_OnEnable);
		HANDLE_MSG(hWindow, WM_PALETTECHANGED, Gauge_OnPaletteChanged);
		HANDLE_MSG(hWindow, SM_GETPOSITION, Gauge_OnUserMsg);
		HANDLE_MSG(hWindow, SM_SETPOSITION, Gauge_OnUserMsg);
		
		default:
		  return DefWindowProc (hWindow, message, wParam, lParam);
	}
}
