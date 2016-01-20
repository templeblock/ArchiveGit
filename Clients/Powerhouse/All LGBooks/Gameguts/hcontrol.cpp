#include <windows.h>
#include "proto.h"
#include "control.h"
#include "sound.h"
#include "scene.h"
#include "commonid.h"

static BOOL bTrack, bInRect;

/***********************************************************************/
BOOL Hotspot_Register( HINSTANCE hInstance )
/***********************************************************************/
{
	WNDCLASS WndClass;
	WndClass.hCursor		= LoadCursor( hInstance, MAKEINTRESOURCE(IDC_HAND_POINT_CURSOR) );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"hot";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= hInstance;
	WndClass.style			= 0;
	WndClass.lpfnWndProc	= HotspotControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 16;

	return( RegisterClass( &WndClass ) );
}

/***********************************************************************/
LOCAL BOOL Hotspot_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	LPLONG lpSwitches;
	STRING szString;
	int iTimedSeconds;

	GetWindowText( hWindow, szString, sizeof(STRING) );
	GetStringParm( szString, 0/*nIndex*/, ',', szString );
	if ( lpSwitches = ExtractSwitches( szString ) )
		{
		if ( iTimedSeconds = (int)GetSwitchValue( 't', lpSwitches ) )
			{ // Use the window handle as the timer id
			SetTimer( hWindow, (WORD)hWindow/*TimerID*/, 1000*iTimedSeconds, NULL );
			}
		FreeUp( (LPTR)lpSwitches );
		}
	return TRUE;
}

/***********************************************************************/
LOCAL void Hotspot_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	KillTimer( hWindow, (WORD)hWindow/*TimerID*/ );
	if ( bTrack )
	{
		ReleaseCapture();
		bTrack = NO;
	}
}

/***********************************************************************/
LOCAL void Hotspot_OnTimer(HWND hWindow, UINT id)
/***********************************************************************/
{ // Timer went off, so execute the same as in WM_LBUTTONDOWN
	STRING szScene;

	KillTimer( hWindow, id );
	GetWindowText( hWindow, szScene, sizeof(szScene)-1 );
	if ( !(*szScene) )
		FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 0, SendMessage);
	else
		GetApp()->GotoScene( GetParent(hWindow), latoi(szScene) );
}

/***********************************************************************/
LOCAL UINT Hotspot_OnGetDlgCode(HWND hWindow, LPMSG lpmsg)
/***********************************************************************/
{
	return( DLGC_WANTARROWS );
}

/***********************************************************************/
LOCAL BOOL Hotspot_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
LOCAL UINT Hotspot_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
	UINT uReturn = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if ( SHIFT && (uReturn == HTCLIENT) )
		uReturn = HTCAPTION;
	return( uReturn );
}

/***********************************************************************/
LOCAL void Hotspot_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
	InvalidateRect( hWindow, NULL, TRUE );
}

/***********************************************************************/
LOCAL void Hotspot_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;

	if ( !(hDC = BeginPaint( hWindow, &ps )) )
		return;

	LPSCENE lpScene = CScene::GetScene( GetParent(hWindow) );
	if ( lpScene )
	{
		RECT rect = ps.rcPaint;
		LONG lReturn = GetWindowLong( hWindow, GWL_DATAPTR);		
		if (lReturn)
		{
			HBRUSH hBrush = CreateSolidBrush(lReturn);
			HDC hDC = GetDC(hWindow);
			RECT rctControl;
			GetClientRect(hWindow, &rctControl);
			FillRect(hDC, &rctControl, hBrush);
			ReleaseDC(hWindow, hDC);
		}
		else
		{
			MapWindowPoints( hWindow, GetParent(hWindow), (LPPOINT)&rect, 2 );
			lpScene->Paint( hDC, &rect, (LPPOINT)&ps.rcPaint );
		}
	}

	EndPaint( hWindow, &ps );
}

/***********************************************************************/
LOCAL void Hotspot_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( SHIFT )
		return;
	if ( bTrack )
		return;
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	bInRect = YES;
}

/***********************************************************************/
LOCAL void Hotspot_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( !bTrack )
		return;
	ReleaseCapture(); bTrack = FALSE;
	if ( bInRect )
	{
		STRING szScene;

		GetWindowText( hWindow, szScene, sizeof(szScene)-1 );
		if ( !(*szScene) )
			FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 0, SendMessage);
		else
			GetApp()->GotoScene( GetParent(hWindow), latoi(szScene) );
	}
}

/***********************************************************************/
LOCAL void Hotspot_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
//	STRING szScene;
//
//	GetWindowText( hWindow, szScene, sizeof(szScene)-1 );
//	if ( !(*szScene) )
//		FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 1, SendMessage);
//	else
//		GetApp()->GotoScene( GetParent(hWindow), latoi(szScene) );
}

#ifdef _DEBUG
/***********************************************************************/
LOCAL void Hotspot_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( CONTROL )
		PrintWindowCoordinates( hWindow );
}
#endif

/***********************************************************************/
LOCAL void Hotspot_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	RECT ClientRect;
	POINT pt;

	if ( !bTrack )
		return;
	GetClientRect( hWindow, &ClientRect );
	pt.x = x;
	pt.y = y;
	if ( bInRect == !PtInRect( &ClientRect, pt ) )
		bInRect = !bInRect;
}

/***********************************************************************/
void Hotspot_OnSetState(HWND hWindow, BOOL fState)
/***********************************************************************/
{
	SetWindowWord( hWindow, GWW_STATE, fState );
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
}

/***********************************************************************/
void Hotspot_OnSetColor(HWND hWindow, COLORREF crColor, BOOL bNoDisplay )
/***********************************************************************/
{
	SetWindowLong( hWindow, GWL_DATAPTR, crColor);	
	if ( bNoDisplay )
		return;
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow( hWindow );
}

/***********************************************************************/
COLORREF Hotspot_OnGetColor(HWND hWindow)
/***********************************************************************/
{
	LONG lReturn;
	lReturn = GetWindowLong( hWindow, GWL_DATAPTR);		
	return((COLORREF)lReturn);
}

/***********************************************************************/
LONG WINPROC EXPORT HotspotControl(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, Hotspot_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Hotspot_OnDestroy);
		HANDLE_MSG(hWindow, WM_TIMER, Hotspot_OnTimer);
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Hotspot_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Hotspot_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_NCHITTEST, Hotspot_OnNCHitTest);
		HANDLE_MSG(hWindow, WM_MOVE, Hotspot_OnMove);
		HANDLE_MSG(hWindow, WM_PAINT, Hotspot_OnPaint);
		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Hotspot_OnLButtonDown);
		HANDLE_MSG(hWindow, WM_LBUTTONUP, Hotspot_OnLButtonUp);
		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Hotspot_OnLButtonDblClk);
		#ifdef _DEBUG
		HANDLE_MSG(hWindow, WM_RBUTTONUP, Hotspot_OnRButtonUp);
		#endif
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Hotspot_OnMouseMove);
		HANDLE_MSG(hWindow, BM_SETSTATE, Hotspot_OnSetState);
		HANDLE_MSG(hWindow, BM_SETCHECK, Hotspot_OnSetState);
		HANDLE_MSG(hWindow, BM_SETCOLOR, Hotspot_OnSetColor);
		HANDLE_MSG(hWindow, BM_GETCOLOR, Hotspot_OnGetColor);
		#ifdef WIN32 // needed for Windows 95 cause it still sends some 16-bit messages
		HANDLE_MSG(hWindow, BM_SETSTATE16, Hotspot_OnSetState);
		HANDLE_MSG(hWindow, BM_SETCHECK16, Hotspot_OnSetState); 
		#endif

		default:
		return DefWindowProc( hWindow, message, wParam, lParam );
	}
}
