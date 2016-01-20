#include "windows.h"
#include "proto.h"
#include "control.h"

typedef void (FAR *VDP_SETPROC)( HWND hWindow, BOOL bOn );
typedef void (FAR *VDP_DRAWPROC)( HWND hWindow );

//Video_DrawProcInstall( hWindow, BumpSet, BumpDraw, 33 );
//Video_DrawProcInstall( hWindow, CrackSet, CrackDraw, 33 );

/***********************************************************************/
static void StuffCrack( LPCRACK lpCrack, POINT pt1, BOOL bxpos, BOOL bypos,
						BOOL bxlty, int iMaxLen )
/***********************************************************************/
{
	if ( !lpCrack )
		return;

	lpCrack->pt1 = pt1;
	lpCrack->bxpos = bxpos;
	lpCrack->bypos = bypos;
	lpCrack->bxlty = bxlty;
	lpCrack->lpNext = NULL;
	int rx = GetRandomNumber( iMaxLen );
	int ry = GetRandomNumber( iMaxLen );
	int r;
	if ( bxlty != (rx < ry) ) { r = rx; rx = ry; ry = r; }
	lpCrack->pt2.x = pt1.x + ( bxpos ? rx : -rx );
	lpCrack->pt2.y = pt1.y + ( bypos ? ry : -ry );
	lpCrack->iMaxLen = iMaxLen-1;
//	r = min( rx, ry );
//	lpCrack->iMaxLen = r;
}

/***********************************************************************/
static void AddCrack( LPCRACK lpCrack )
/***********************************************************************/
{
	if ( !lpCrack )
		return;

	while ( lpCrack )
	{
		if ( !lpCrack->lpNext )
			break;
		lpCrack = lpCrack->lpNext;
	}

	if ( lpCrack->iMaxLen < 2 )
		return;

	if ( !(lpCrack->lpNext = (LPCRACK)Alloc( sizeof(CRACK) )) )
		return;

	StuffCrack( lpCrack->lpNext, lpCrack->pt2, lpCrack->bxpos, lpCrack->bypos,
		lpCrack->bxlty, lpCrack->iMaxLen );
}

/***********************************************************************/
static void FreeCracks( LPCRACK lpCrack )
/***********************************************************************/
{
	while ( lpCrack )
	{
		LPCRACK lpNextCrack = lpCrack->lpNext;
		FreeUp( (LPTR)lpCrack );
		lpCrack = lpNextCrack;
	}
}

static CRACK Crack1, Crack2, Crack3, Crack4, Crack5, Crack6, Crack7, Crack8;

//************************************************************************
static void InitCracks( HWND hWindow, BOOL bInit )
//************************************************************************
{
	if ( !bInit )
	{
		FreeCracks( Crack1.lpNext ); Crack1.lpNext = NULL;
		FreeCracks( Crack2.lpNext ); Crack2.lpNext = NULL;
		FreeCracks( Crack3.lpNext ); Crack3.lpNext = NULL;
		FreeCracks( Crack4.lpNext ); Crack4.lpNext = NULL;
		FreeCracks( Crack5.lpNext ); Crack5.lpNext = NULL;
		FreeCracks( Crack6.lpNext ); Crack6.lpNext = NULL;
		FreeCracks( Crack7.lpNext ); Crack7.lpNext = NULL;
		FreeCracks( Crack8.lpNext ); Crack8.lpNext = NULL;
	}
	else
	{
		RECT rWindow;
		GetClientRect( hWindow, &rWindow );
		POINT pt;
		pt.x = (rWindow.right - rWindow.left) / 2;
		pt.y = (rWindow.bottom - rWindow.top) / 2;
		StuffCrack( &Crack1, pt,  NO/*x+*/,	 NO/*y+*/,	NO/*x<y*/, 30 );
		StuffCrack( &Crack2, pt,  NO/*x+*/,	 NO/*y+*/, YES/*x<y*/, 30 );
		StuffCrack( &Crack3, pt,  NO/*x+*/, YES/*y+*/,	NO/*x<y*/, 30 );
		StuffCrack( &Crack4, pt,  NO/*x+*/, YES/*y+*/, YES/*x<y*/, 30 );
		StuffCrack( &Crack5, pt, YES/*x+*/,	 NO/*y+*/,	NO/*x<y*/, 30 );
		StuffCrack( &Crack6, pt, YES/*x+*/,	 NO/*y+*/, YES/*x<y*/, 30 );
		StuffCrack( &Crack7, pt, YES/*x+*/, YES/*y+*/,	NO/*x<y*/, 30 );
		StuffCrack( &Crack8, pt, YES/*x+*/, YES/*y+*/, YES/*x<y*/, 30 );
	}
}

/***********************************************************************/
void CrackSet( HWND hWindow, BOOL bOn )
/***********************************************************************/
{
	InitCracks( hWindow, bOn );
}

/***********************************************************************/
static void CrackDraw( HDC hDC, LPCRACK lpCrack )
/***********************************************************************/
{  
    HPEN hCrackPen = CreatePen( PS_SOLID, 1, RGB(255,255,255) );
    
    HPEN hOldPen = (HPEN)SelectObject( hDC, hCrackPen );
	while ( lpCrack )
	{
		MoveToEx( hDC, lpCrack->pt1.x, lpCrack->pt1.y, NULL );
		LineTo( hDC, lpCrack->pt2.x, lpCrack->pt2.y );
		lpCrack = lpCrack->lpNext;
	}    
	
	SelectObject( hDC, hOldPen ); 
}

//************************************************************************
void CALLBACK CrackDrawDCProc( HDC hDC, HWND hWindow )
//************************************************************************
{
	CrackDraw( hDC, &Crack1 );
	CrackDraw( hDC, &Crack2 );
	CrackDraw( hDC, &Crack3 );
	CrackDraw( hDC, &Crack4 );
	CrackDraw( hDC, &Crack5 );
	CrackDraw( hDC, &Crack6 );
	CrackDraw( hDC, &Crack7 );
	CrackDraw( hDC, &Crack8 );

	if ( SHIFT )
	{
		AddCrack( &Crack1 );
		AddCrack( &Crack2 );
		AddCrack( &Crack3 );
		AddCrack( &Crack4 );
		AddCrack( &Crack5 );
		AddCrack( &Crack6 );
		AddCrack( &Crack7 );
		AddCrack( &Crack8 );
		while ( SHIFT )
			;
	}
}

/***********************************************************************/
void CrackDraw( HWND hWindow )
/***********************************************************************/
{
	HDC hDC;
	if ( !(hDC = GetDC( hWindow )) )
		return;
	CrackDrawDCProc( hDC, hWindow );
	ReleaseDC( hWindow, hDC );
}

static RECT SrcRect, DstRect;
static int iCount;

/***********************************************************************/
void BumpDraw( HWND hWindow )
/***********************************************************************/
{
	LPVIDEO lpVideo;
	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	RECT SRect = SrcRect;
	RECT DRect = DstRect;
	int iZoomFactor = (int)GetSwitchValue( 'z', lpVideo->lpSwitches );
	if ( !iCount )
	{
		SRect.top += 10;
		DRect = SRect;
		ScaleRect( &DRect, iZoomFactor );
		iCount = 1;
	}
	else
	{
		SRect.top -= 10;
		DRect = SRect;
		ScaleRect( &DRect, iZoomFactor );
		iCount = 0;
	}
	MCIClip( lpVideo->hDevice, &SRect, NULL/*&DRect*/, /*0*/iZoomFactor );
}

/***********************************************************************/
void BumpSet( HWND hWindow, BOOL bOn )
/***********************************************************************/
{
	LPVIDEO lpVideo;
	if ( !(lpVideo = (LPVIDEO)GetWindowLong( hWindow, GWL_DATAPTR )) )
		return;

	if ( !bOn )
	{
		MCIGetSrcRect( lpVideo->hDevice, &SrcRect );
		MCIGetDstRect( lpVideo->hDevice, &DstRect );
	}
	else
	{
		MCIClip( lpVideo->hDevice, &SrcRect, &DstRect, 0/*iZoomFactor*/ );
		iCount = 0;
	}
}
