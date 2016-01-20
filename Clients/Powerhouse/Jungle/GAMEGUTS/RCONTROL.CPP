#include <windows.h>
#include <math.h>
#include "proto.h"
#include "control.h"
#include "sound.h"
#include "mmsystem.h"
#include "scene.h"
#include "worlds.h"

#define SM_GETPOSITION	(WM_USER + 700)
#define SM_SETPOSITION	(WM_USER + 701)
#define HANDLE_SM_GETPOSITION(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)SM_GETPOSITION, wParam, lParam))
#define HANDLE_SM_SETPOSITION(hwnd, wParam, lParam, fn) \
	((LRESULT)(fn)(hwnd, (UINT)SM_SETPOSITION, wParam, lParam))

static BOOL bTrack, bInRect;

/***********************************************************************/
static BOOL Radar_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	bTrack = FALSE;
	bInRect = FALSE;

	LPWORLD lpWorld;
	if ( lpWorld = new CWorld )
	{
		lpWorld->Init( hWindow, NULL/*idMap*/ );
		SetWindowLong( hWindow, GWL_DATAPTR, (long)lpWorld );
	}
	return TRUE;
}

/***********************************************************************/
static LPWORLD Radar_GetWorld( HWND hWindow )
/***********************************************************************/
{
	return( (LPWORLD)GetWindowLong (hWindow, GWL_DATAPTR) );
}

/***********************************************************************/
static void Radar_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	if (bTrack)
	{
		ReleaseCapture();
		bTrack = NO;
	}

	LPWORLD lpWorld;
	if ( lpWorld = Radar_GetWorld(hWindow) )
	{
		delete lpWorld;
		lpWorld = NULL;
	}
}

/***********************************************************************/
static void Radar_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
	ResizeControl( hWindow, cx, cy );
	FORWARD_WM_SIZE(hWindow, state, cx, cy, DefWindowProc);
}

/***********************************************************************/
static UINT Radar_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
	UINT uReturn = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if ( SHIFT && (uReturn == HTCLIENT) )
		uReturn = HTCAPTION;
	return( uReturn );
}

/***********************************************************************/
static void Radar_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
	InvalidateRect( hWindow, NULL, TRUE );
}

/***********************************************************************/
static void Radar_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;

	if ( !(hDC = BeginPaint (hWindow, &ps)) )
		return;

	LPWORLD lpWorld;
	if ( lpWorld = Radar_GetWorld(hWindow) )
		lpWorld->Draw( hDC, &ps.rcPaint );

	EndPaint( hWindow, &ps );
}

static POINT ptDown;
static BOOL bMoved;
/***********************************************************************/
static void Radar_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( bTrack )
		return;
	SetCapture( hWindow );
	bTrack = TRUE;
	bInRect = TRUE;
	bMoved = FALSE;

	LPWORLD lpWorld;
	if ( lpWorld = Radar_GetWorld(hWindow) )
	{
		lpWorld->GetViewLocation( &ptDown );
		ptDown.x += x;
		ptDown.y += y;
	}
}

/***********************************************************************/
static void Radar_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( !bTrack )
		return;

	ReleaseCapture();
	bTrack = FALSE;

	if ( bMoved )
		return;

	LPWORLD lpWorld;
	if ( !(lpWorld = Radar_GetWorld(hWindow) ) )
		return;

	int iZoomFactor = lpWorld->GetZoomFactor();
	if ( CONTROL )
	{
		if ( iZoomFactor == -3 )
			iZoomFactor = 6;
		else
		{
			iZoomFactor--;
			if ( !iZoomFactor )
				iZoomFactor -= 2;
		}
	}
	else
	{
		if ( iZoomFactor == 6 )
			iZoomFactor = -3;
		else
		{
			iZoomFactor++;
			if ( iZoomFactor == -1 )
				iZoomFactor += 2;
		}
	}

	lpWorld->SetZoomFactor( iZoomFactor );
	InvalidateRect( hWindow, NULL, TRUE );
//	FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 0, SendMessage);
}

/***********************************************************************/
static void Radar_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
//	FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 1, SendMessage);
}

#ifdef _DEBUG
/***********************************************************************/
static void Radar_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( CONTROL )
		PrintWindowCoordinates( hWindow );
}
#endif

/***********************************************************************/
static void Radar_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	RECT ClientRect;
	POINT pt;

	if ( !bTrack )
		return;
	
	GetClientRect(hWindow, &ClientRect);
	pt.x = x;
	pt.y = y;
	if (bInRect == ! PtInRect(&ClientRect, pt ) )
		bInRect = ! bInRect;

//	LPWORLD lpWorld;
//	if ( lpWorld = Radar_GetWorld(hWindow) )
//	{
//		POINT ptOffset;
//		ptOffset.x = ptDown.x - x;
//		ptOffset.y = ptDown.y - y;
//		lpWorld->SetViewLocation( &ptOffset, YES/*bScroll*/ );
//		bMoved = YES;
//	}
}

/***********************************************************************/
static void Radar_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
	FORWARD_WM_ENABLE( hWindow, fEnable, DefWindowProc );
	InvalidateRect( hWindow, NULL, TRUE );
	UpdateWindow( hWindow );
}

/***********************************************************************/
static void Radar_OnPaletteChanged( HWND hWindow, HWND hwndPaletteChange )
/***********************************************************************/
{
	InvalidateRect( hWindow, NULL, TRUE );
}

/***********************************************************************/
static long Radar_OnUserMsg( HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	long lResult = 0L;

	switch (msg)
	{
		case SM_GETPOSITION:
			break;

		case SM_SETPOSITION:
			break;

		default:
			break;
	}

	return lResult;
}

/***********************************************************************/
LONG WINPROC EXPORT RadarControl(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, Radar_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Radar_OnDestroy);
		HANDLE_MSG(hWindow, WM_SIZE, Radar_OnSize);
		HANDLE_MSG(hWindow, WM_NCHITTEST, Radar_OnNCHitTest);
		HANDLE_MSG(hWindow, WM_MOVE, Radar_OnMove);
		HANDLE_MSG(hWindow, WM_PAINT, Radar_OnPaint);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Radar_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Radar_OnLButtonUp);
		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Radar_OnLButtonDblClk);
		#ifdef _DEBUG
		HANDLE_MSG(hWindow, WM_RBUTTONUP, Radar_OnRButtonUp);
		#endif
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Radar_OnMouseMove);
		HANDLE_MSG(hWindow, WM_ENABLE, Radar_OnEnable);
		HANDLE_MSG(hWindow, WM_PALETTECHANGED, Radar_OnPaletteChanged);
		HANDLE_MSG(hWindow, SM_GETPOSITION, Radar_OnUserMsg);
		HANDLE_MSG(hWindow, SM_SETPOSITION, Radar_OnUserMsg);
	
		default:
		  return DefWindowProc( hWindow, message, wParam, lParam );
	}
}
