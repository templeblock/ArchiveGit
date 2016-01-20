#include <windows.h>
#include "proto.h"
#include "control.h"
#include "mmsystem.h"
#include "..\jungle\jungleid.h"

//#define USE_MM_TIMER

#define VDP_TIMER_ID 0xFEED
#define A_SPECIAL 1073741824 // same as 0x40000000L

typedef void (FAR *VDP_SETPROC)( HWND hWindow, BOOL bOn );
typedef void (FAR *VDP_DRAWPROC)( HWND hWindow );

static VDP_SETPROC lpSetProc;
static VDP_DRAWPROC lpDrawProc;
static UINT idDrawTimer;
static UINT uDrawTimerDelay;
static DWORD dwLastDrawTime = 0;
//static DWORD dwCount = 0;


/***********************************************************************/
void Video_DrawProcInstall( HWND hWindow, VDP_SETPROC lpNewSetProc, VDP_DRAWPROC lpNewDrawProc, UINT wTimeDelay )
/***********************************************************************/
{
	if ( lpSetProc == lpNewSetProc && lpDrawProc == lpNewDrawProc )
		return;

	if ( lpSetProc )
	{
		#ifdef USE_MM_TIMER
		KillMMTimer(hWindow, idDrawTimer);
		#else
		KillTimer( hWindow, VDP_TIMER_ID );
		#endif
		(*lpSetProc)( hWindow, OFF );
	}

	if ( lpDrawProc = lpNewDrawProc )
	{
		#ifdef USE_MM_TIMER
		uDrawTimerDelay = wTimeDelay;
		idDrawTimer = SetMMTimer(hWindow, uDrawTimerDelay);
		dwLastDrawTime = timeGetTime();
		#else
		idDrawTimer = SetTimer( hWindow, VDP_TIMER_ID, wTimeDelay, NULL );
		#endif
	}

	if ( lpSetProc = lpNewSetProc )
		(*lpSetProc)( hWindow, ON );
}

/***********************************************************************/
BOOL Video_DrawProcDraw( HWND hWindow, ITEMID idTimer )
/***********************************************************************/
{
	if ( idTimer != idDrawTimer )
		return( NO );

	if ( !lpDrawProc )
		return( NO );

	#ifdef USE_MM_TIMER
	#define TIMER_CLOSENESS 10 // 10 milliseconds
	// make sure we have at least uTimerDelay-TIMER_CLOSENESS milliseconds
	// between times when we draw.  If we don't then the
	// video plays very choppy on slow machines.
	DWORD dwTime = timeGetTime();
	if ((dwTime - dwLastDrawTime) < (uDrawTimerDelay-TIMER_CLOSENESS))
		return(NO);
//	STRING szString;
//	if ((dwCount % 33) == 0)
//		Debug( "delta = %ld\n", dwTime-dwLastDrawTime );
//	++dwCount;
	dwLastDrawTime = dwTime;
	#endif

	(*lpDrawProc)( hWindow );
	return( YES );
}

static HDC hDrawDC, hHotspotDC, hInstinctDC;
//static HPEN hPen1, hPen2, hOldPen;
static HBITMAP hHotspotBitmap, hInstinctBitmap;
static HBITMAP hOldHotspotBitmap, hOldInstinctBitmap;
static iHotspotWidth, iHotspotHeight, iInstinctWidth, iInstinctHeight;

/***********************************************************************/
void VDP_HotspotSet( HWND hWindow, BOOL bOn )
/***********************************************************************/
{
	BITMAP bm;

	if ( bOn )
	{
//		static HWND hWndTest = CreateWindow(
//			"edit",				// lpClassName
//			NULL,				// lpWindowName
//			WS_POPUP|
//			WS_VISIBLE,			// dwStyle
///			300, 75,			// position of window
//			65, 15,				// size of window
//			hWindow,			// hWindowParent (null for tiled)
//			NULL,				// hMenu (null = class menu)
//			GetWindowInstance(hWindow),// hInstance
//			NULL				// lpParam
//			);

		if ( !hDrawDC )
			if ( !(hDrawDC = GetDC(hWindow)) )
				return;
		hHotspotDC = CreateCompatibleDC(hDrawDC);
		hHotspotBitmap = LoadBitmap(GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_HOTSPOT));
		GetObject(hHotspotBitmap, sizeof(BITMAP), &bm);
		iHotspotWidth = bm.bmWidth;
		iHotspotHeight = bm.bmHeight;
		hOldHotspotBitmap = (HBITMAP)SelectObject(hHotspotDC, hHotspotBitmap);

		hInstinctDC = CreateCompatibleDC(hDrawDC);
  		hInstinctBitmap = LoadBitmap(GetApp()->GetInstance(), MAKEINTRESOURCE(IDI_INSTINCT));
		GetObject(hInstinctBitmap, sizeof(BITMAP), &bm);
		iInstinctWidth = bm.bmWidth;
		iInstinctHeight = bm.bmHeight;
		hOldInstinctBitmap = (HBITMAP)SelectObject(hInstinctDC, hInstinctBitmap);
//		hPen1 = CreatePen( PS_SOLID, 3, RGB(  0,  0,0) );
//		hPen2 = CreatePen( PS_SOLID, 1, RGB(255,255,0) );
//		hOldPen = (HPEN)SelectObject( hDrawDC, hPen1 );
	}
	else
	{
		if (hHotspotDC)
		{
			SelectObject(hHotspotDC, hOldHotspotBitmap);
			DeleteDC(hHotspotDC);
			hHotspotDC = NULL;
		}
		if (hInstinctDC)
		{
			SelectObject(hInstinctDC, hOldInstinctBitmap);
			DeleteDC(hInstinctDC);
			hInstinctDC = NULL;
		}
		if (hHotspotBitmap)
		{
			DeleteObject(hHotspotBitmap);
			hHotspotBitmap = NULL;
		}
		if (hInstinctBitmap)
		{
			DeleteObject(hInstinctBitmap);
			hInstinctBitmap = NULL;
		}
//		SelectObject( hDrawDC, hOldPen );
//		DeleteObject( hPen1 );
//		DeleteObject( hPen2 );
		ReleaseDC( hWindow, hDrawDC );
		hDrawDC = NULL;
	}
}

/***********************************************************************/
void VDP_HotspotDraw( HWND hWindow )
/***********************************************************************/
{
	LPVIDEO lpVideo = (LPVIDEO)GetWindowLong (hWindow, GWL_DATAPTR);
	if (!lpVideo)
		return;

	if ( lpVideo->lCurrentShot <= 0 )
		return;

	LPSHOT lpShotOrig = &lpVideo->lpAllShots[lpVideo->lCurrentShot - 1];
	if (!lpShotOrig)
		return;

	RECT rWindow;
	GetClientRect(hWindow, &rWindow);
	int iUnitWidth = (rWindow.right - rWindow.left) / 10;
	int iUnitHeight = (rWindow.bottom - rWindow.top) / 10;

	// note: this will not draw a hotspot of 0000
	long lFrame = 0;
	LPSHOT lpShot;

	//TryLeft: // Try the left event
	lpShot = lpShotOrig;
	if ( lpShot->bLeftIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryRight;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lLeftShot, lpShot->lLeftCount, lFrame )) )
				goto TryRight;
	}
	if ( lpShot->lLeftHotspot && !(lpShot->fHotspotDisabled & 1) )
		VDP_HotspotDrawOne( lpShot->lLeftHotspot, !(lpShot->lFlags & A_SPECIAL), iUnitWidth, iUnitHeight );

	TryRight: // Try the right event
	lpShot = lpShotOrig;
	if ( lpShot->bRightIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryUp;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lRightShot, lpShot->lRightCount, lFrame )) )
				goto TryUp;
	}
	if ( lpShot->lRightHotspot && !(lpShot->fHotspotDisabled & 2) )
		VDP_HotspotDrawOne( lpShot->lRightHotspot, !(lpShot->lFlags & A_SPECIAL), iUnitWidth, iUnitHeight );

	TryUp: // Try the up event
	lpShot = lpShotOrig;
	if ( lpShot->bUpIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryDown;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lUpShot, lpShot->lUpCount, lFrame )) )
				goto TryDown;
	}
	if ( lpShot->lUpHotspot && !(lpShot->fHotspotDisabled & 4) )
		VDP_HotspotDrawOne( lpShot->lUpHotspot, !(lpShot->lFlags & A_SPECIAL), iUnitWidth, iUnitHeight );

	TryDown: // Try the down event
	lpShot = lpShotOrig;
	if ( lpShot->bDownIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryHome;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lDownShot, lpShot->lDownCount, lFrame )) )
				goto TryHome;
	}
	if ( lpShot->lDownHotspot && !(lpShot->fHotspotDisabled & 8) )
		VDP_HotspotDrawOne( lpShot->lDownHotspot, !(lpShot->lFlags & A_SPECIAL), iUnitWidth, iUnitHeight );

	TryHome: // Try the home event
	lpShot = lpShotOrig;
	if ( lpShot->bHomeIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryDone;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lHomeShot, lpShot->lHomeCount, lFrame )) )
				goto TryDone;
	}
	if ( lpShot->lHomeHotspot && !(lpShot->fHotspotDisabled & 16) )
	VDP_HotspotDrawOne( lpShot->lHomeHotspot, !(lpShot->lFlags & A_SPECIAL), iUnitWidth, iUnitHeight );

	TryDone:
		;
}

/***********************************************************************/
void VDP_HotspotDrawOne( long lHotspot, BOOL bNormal, int iUnitWidth, int iUnitHeight )
/***********************************************************************/
{
//	POINT pts[9];
	POINT pt;
	RECT rHotspot;
	char szBuff[8];

 	if ( !lHotspot )
		return;

	if ( !hDrawDC )
		return;

	if ( lHotspot < 0 )
		lHotspot = -lHotspot;

	wsprintf (szBuff, "%04ld", lHotspot);
	rHotspot.left	= (int)(szBuff [0] - '0') * iUnitWidth;
	rHotspot.right	= (int)(szBuff [1] - '0' + 1) * iUnitWidth;
	rHotspot.top	= (int)(szBuff [2] - '0') * iUnitHeight;
	rHotspot.bottom = (int)(szBuff [3] - '0' + 1) * iUnitHeight;

	pt.x = (rHotspot.right + rHotspot.left) / 2;
	pt.y = (rHotspot.bottom + rHotspot.top) / 2;

	if ( bNormal )
	{
		pt.x -= iHotspotWidth/2;
		pt.y -= iHotspotHeight/2;
		BitBlt(hDrawDC, pt.x, pt.y, iHotspotWidth, iHotspotHeight, hHotspotDC, 0, 0, SRCCOPY);
		//for ( int i=0; i<2; i++ )
		//{
		//	if ( i == 0 )
		//		SelectObject( hDrawDC, hPen1 );
		//	else
		//		SelectObject( hDrawDC, hPen2 );
		//
		//	// Note: we add 1 to the 1 pixel LineTo() only since it doesn't draw the last pixel
		//	pts[0].x = pt.x-3; pts[0].y = pt.y-2;
		//	pts[1].x = pt.x-3; pts[1].y = pt.y-3;
		//	pts[2].x = pt.x-2; pts[2].y = pt.y-4;
		//	pts[3].x = pt.x+2; pts[3].y = pt.y-4;
		//	pts[4].x = pt.x+3; pts[4].y = pt.y-3;
		//	pts[5].x = pt.x+3; pts[5].y = pt.y-2;
		//	pts[6].x = pt.x+1; pts[6].y = pt.y;
		//	pts[7].x = pt.x; pts[7].y = pt.y;
		//	pts[8].x = pt.x; pts[8].y = pt.y+3-i;
		//	Polyline(hDrawDC, pts, 9);
		//
		//	//MoveToEx (hDrawDC, pt.x-3, pt.y-2, NULL );
		//	//LineTo (hDrawDC, pt.x-3, pt.y-3 );
		//	//LineTo (hDrawDC, pt.x-2, pt.y-4 );
		//	//LineTo (hDrawDC, pt.x+2, pt.y-4 );
		//	//LineTo (hDrawDC, pt.x+3, pt.y-3 );
		//	//LineTo (hDrawDC, pt.x+3, pt.y-2 );
		//	//LineTo (hDrawDC, pt.x+1, pt.y );
		//	//LineTo (hDrawDC, pt.x, pt.y );
		//	//LineTo (hDrawDC, pt.x, pt.y+3-i );
		//
		//	// top-right to bottom-left
		//	pts[0].x = pt.x; pts[0].y = pt.y+4;
		//	pts[1].x = pt.x; pts[1].y = pt.y+5+i;
		//	Polyline(hDrawDC, pts, 2);
		//	//MoveToEx (hDrawDC, pt.x, pt.y+4, NULL );
		//	//LineTo (hDrawDC, pt.x, pt.y+5+i );
		//}
	}
	else
	{
		pt.x -= iInstinctWidth/2;
		pt.y -= iInstinctHeight/2;
		BitBlt(hDrawDC, pt.x, pt.y, iInstinctWidth, iInstinctHeight, hInstinctDC, 0, 0, SRCCOPY);
		//for ( int i=0; i<2; i++ )
		//{
		//	if ( i == 0 )
		//		SelectObject( hDrawDC, hPen1 );
		//	else
		//		SelectObject( hDrawDC, hPen2 );
		//
		//	// Note: we add 1 to the 1 pixel LineTo() only since it doesn't draw the last pixel
	 	//	// top-left to bottom-right
		//	pts[0].x = pt.x-4; pts[0].y = pt.y-4;
		//	pts[1].x = pt.x+4+i; pts[1].y = pt.y+4+i;
		//	Polyline(hDrawDC, pts, 2);
		//	//MoveToEx (hDrawDC, pt.x-4, pt.y-4, NULL );
		//	//LineTo (hDrawDC, pt.x+4+i, pt.y+4+i);
		//	// top to bottom
		//	pts[0].x = pt.x; pts[0].y = pt.y-6;
		//	pts[1].x = pt.x; pts[1].y = pt.y+6+i;
		//	Polyline(hDrawDC, pts, 2);
		//	//MoveToEx (hDrawDC, pt.x, pt.y-6, NULL );
		//	//LineTo (hDrawDC, pt.x, pt.y+6+i);
		//	// left to right
		//	pts[0].x = pt.x-6; pts[0].y = pt.y;
		//	pts[1].x = pt.x+6+i; pts[1].y = pt.y;
		//	Polyline(hDrawDC, pts, 2);
		//	//MoveToEx (hDrawDC, pt.x-6, pt.y, NULL );
		//	//LineTo (hDrawDC, pt.x+6+i, pt.y);
		//	// top-right to bottom-left
		//	pts[0].x = pt.x+4; pts[0].y = pt.y-4;
		//	pts[1].x = pt.x-4-i; pts[1].y = pt.y+4+i;
		//	Polyline(hDrawDC, pts, 2);
		//	//MoveToEx (hDrawDC, pt.x+4, pt.y-4, NULL );
		//	//LineTo (hDrawDC, pt.x-4-i, pt.y+4+i);
		//}
	}
}

/***********************************************************************/
void VDP_GridSet( HWND hWindow, BOOL bOn )
/***********************************************************************/
{
	if ( bOn )
	{
		if ( !hDrawDC )
			if ( !(hDrawDC = GetDC(hWindow)) )
				return;
	}
	else
	{
		ReleaseDC( hWindow, hDrawDC );
		hDrawDC = NULL;
	}
}

/***********************************************************************/
void VDP_GridDraw( HWND hWindow )
/***********************************************************************/
{
	RECT rWindow;
	int iWidth, iHeight, iDelta;
	
	GetClientRect( hWindow, &rWindow );
	iWidth	= RectWidth( &rWindow );
	iHeight = RectHeight( &rWindow );
	iDelta = iWidth/10;
	for ( int x=iDelta; x<iWidth; x+=iDelta )
	{
		MoveToEx( hDrawDC, x, rWindow.top, NULL );
		LineTo( hDrawDC, x, rWindow.bottom );
	}
	iDelta = iHeight/10;
	for ( int y=iDelta; y<iHeight; y+=iDelta )
	{
		MoveToEx( hDrawDC, rWindow.left, y, NULL );
		LineTo( hDrawDC, rWindow.right, y );	
	}
}
