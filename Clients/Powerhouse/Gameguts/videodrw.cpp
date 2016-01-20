#include <windows.h>
#include "proto.h"
#include "control.h"
#include "mmsystem.h"
#include "commonid.h"

//#define USE_MM_TIMER

#define VDP_TIMER_ID 0xFEED

typedef void (FAR *VDP_SETPROC)( HWND hWindow, BOOL bOn );
typedef void (FAR *VDP_DRAWPROC)( HWND hWindow );

static VDP_SETPROC lpSetProc;
static VDP_DRAWPROC lpDrawProc;
static UINT idDrawTimer;
static UINT uDrawTimerDelay;
static DWORD dwLastDrawTime = 0;


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
	dwLastDrawTime = dwTime;
	#endif

	(*lpDrawProc)( hWindow );
	return( YES );
}

static HDC hDrawDC, hHotspotDC, hInstinctDC;
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
		VDP_HotspotDrawOne( lpShot->lLeftHotspot, lpShot->lFlags, iUnitWidth, iUnitHeight );

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
		VDP_HotspotDrawOne( lpShot->lRightHotspot, lpShot->lFlags, iUnitWidth, iUnitHeight );

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
		VDP_HotspotDrawOne( lpShot->lUpHotspot, lpShot->lFlags, iUnitWidth, iUnitHeight );

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
		VDP_HotspotDrawOne( lpShot->lDownHotspot, lpShot->lFlags, iUnitWidth, iUnitHeight );

	TryHome: // Try the home event
	lpShot = lpShotOrig;
	if ( lpShot->bHomeIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryIns;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lHomeShot, lpShot->lHomeCount, lFrame )) )
				goto TryIns;
	}
	if ( lpShot->lHomeHotspot && !(lpShot->fHotspotDisabled & 16) )
	VDP_HotspotDrawOne( lpShot->lHomeHotspot, lpShot->lFlags, iUnitWidth, iUnitHeight );

	TryIns: // Try the ins event
	lpShot = lpShotOrig;
	if ( lpShot->bInsIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryDel;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lInsShot, lpShot->lInsCount, lFrame )) )
				goto TryDel;
	}
	if ( lpShot->lInsHotspot && !(lpShot->fHotspotDisabled & 32) )
		VDP_HotspotDrawOne( lpShot->lInsHotspot, lpShot->lFlags, iUnitWidth, iUnitHeight );

	TryDel: // Try the del event
	lpShot = lpShotOrig;
	if ( lpShot->bDelIsSubShot )
	{
		if ( !lFrame ) // We need it now...
			if ( !(lFrame = MCIGetPosition( lpVideo->hDevice )) )
				goto TryDone;
		if ( !(lpShot = Video_FindSubShot( lpVideo,
			lpShot->lDelShot, lpShot->lDelCount, lFrame )) )
				goto TryDone;
	}
	if ( lpShot->lDelHotspot && !(lpShot->fHotspotDisabled & 64) )
		VDP_HotspotDrawOne( lpShot->lDelHotspot, lpShot->lFlags, iUnitWidth, iUnitHeight );

	TryDone:
		;
}

/***********************************************************************/
void VDP_HotspotDrawOne( long lHotspot, long lFlags, int iUnitWidth, int iUnitHeight )
/***********************************************************************/
{
	if ( lFlags & FLAGBIT(A_NO_HOTSPOT) )
		return;

	int iDrawType;
	if ( lFlags & FLAGBIT(A_SPECIAL) )
		iDrawType = 2;
	else
		iDrawType = 1;
	
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
	rHotspot.top	= (int)(szBuff [1] - '0') * iUnitHeight;
	rHotspot.right	= (int)(szBuff [2] - '0' + 1) * iUnitWidth;
	rHotspot.bottom = (int)(szBuff [3] - '0' + 1) * iUnitHeight;

	pt.x = (rHotspot.right + rHotspot.left) / 2;
	pt.y = (rHotspot.bottom + rHotspot.top) / 2;

	if ( iDrawType == 1 )
	{
		pt.x -= iHotspotWidth/2;
		pt.y -= iHotspotHeight/2;
		BitBlt(hDrawDC, pt.x, pt.y, iHotspotWidth, iHotspotHeight, hHotspotDC, 0, 0, SRCCOPY);
	}
	else
	if ( iDrawType == 2 )
	{
		pt.x -= iInstinctWidth/2;
		pt.y -= iInstinctHeight/2;
		BitBlt(hDrawDC, pt.x, pt.y, iInstinctWidth, iInstinctHeight, hInstinctDC, 0, 0, SRCCOPY);
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
