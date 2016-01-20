//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes
static void display_cast( HWND hDlg, LPSTR lp, int bScroll );

extern HINSTANCE hInstAstral;

/***********************************************************************/
BOOL WINPROC EXPORT DlgAboutProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
STRING szString;
static LPSTR lpString;
static BOOL bTrack;
static int bScroll;

switch (msg)
    {
    case WM_INITDIALOG:
	SET_CLASS_HBRBACKGROUND(hDlg, ( HBRUSH )GetStockObject(LTGRAY_BRUSH));
	CenterPopup( hDlg );

	GetDefaultString( "SN", "", szString, sizeof(szString) );
	SetDlgItemText( hDlg, IDC_SERIALNO, szString );

	GetDefaultString( "User", "", szString, sizeof(szString) );
	SetDlgItemText( hDlg, IDC_CUSTOMER, szString );

	GetDefaultString( "Company", "", szString, sizeof(szString) );
	SetDlgItemText( hDlg, IDC_COMPANY, szString );

	GetDefaultString( "VersionName", "", szString, sizeof(szString) );
	SetDlgItemText( hDlg, IDC_VERSION_NAME, szString );
	lpString = LineBuffer[0];
	if ( !GetTextBlockData( IDC_CASTTEXT, lpString, 16384 ) )
		GetWindowText( hDlg, (LPSTR)lpString, 16384 );
	bScroll = NO;
	SetTimer( hDlg, bTrack = TRUE, 1, NULL );
	break;

    case WM_TIMER:
	display_cast( GetDlgItem(hDlg, IDC_CAST), lpString, bScroll );
	bScroll = YES;
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_ABOUT ) );

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	switch (wParam)
	    {
	    case IDOK:
	    case IDCANCEL:
		if ( bTrack )
			KillTimer( hDlg, bTrack );
		bTrack = NO;
		AstralDlgEnd( hDlg, NO|2 );
		break;

	    default:
		return( FALSE );
	    }
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}


/************************************************************************/
static void display_cast( HWND hWnd, LPSTR lpString, BOOL bScroll )
/************************************************************************/
{ //  Display a scrolling list of the Cast, beginning with a new player each
//  time.
TEXTMETRIC SysCharSize;
HDC hDC;
RECT Area;
LPSTR lpNext, lpCastName;
HRGN hUpdate;
int SaveLevel, WndHeight, CastHeight, OldBkMode;
DWORD OldTextColor;
HBRUSH hBrush;
HFONT hFont, hOldFont;
RECT Update, rDummy;
BYTE c;
static short CastOfs = -1;
#define SCROLLCOUNT 1

if ( !hWnd )
	return;
if ( !(hDC = GetDC( hWnd )) )
	return;

GetTextMetrics( hDC, &SysCharSize );

OldBkMode = SetBkMode( hDC,TRANSPARENT );
OldTextColor = SetTextColor( hDC, GetSysColor( COLOR_WINDOWTEXT ) );
hBrush = CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );
if ( !(hFont = (HFONT)SendMessage( hWnd, WM_GETFONT, 0, 0L )) )
	hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
hOldFont = (HFONT)SelectObject(hDC, hFont);

GetClientRect( hWnd, &Area );

CastHeight = 0;
lpNext = lpString;
while ( lpNext )
	{
	CastHeight += SysCharSize.tmHeight;
	lpNext = lstrchr( lpNext + 1, '\n' );
	}

if (bScroll)
	{
	Area.top += SCROLLCOUNT; 
	ScrollDC( hDC, 0, -SCROLLCOUNT, &Area, NULL, NULL, &Update );
	Area.top -= SCROLLCOUNT; 
	SaveLevel = SaveDC( hDC );
	}
else
	{
	Update = Area;
	}

hUpdate = CreateRectRgn( Update.left, Update.top, Update.right,
	Update.bottom );
SelectClipRgn( hDC, hUpdate );

WndHeight = (Area.bottom / SysCharSize.tmHeight) * SysCharSize.tmHeight;
if ((WndHeight - (CastOfs -= SCROLLCOUNT)) >= CastHeight)
	CastOfs = WndHeight;
Area.top = CastOfs;
lpNext = lpString;
while ( lpNext )
	{
	Area.bottom = Area.top + SysCharSize.tmHeight;
	lpCastName = lpNext;
	if ( lpNext = lstrchr( lpNext, '\n') )
		{
		c = *lpNext;
		*lpNext = '\0';
		}
	if ( IntersectRect( &rDummy, &Area, &Update ) )
		{
		FillRect( hDC, &Area, hBrush );
		DrawText( hDC, (LPCSTR)lpCastName, -1, &Area,
			(*lpCastName == ' ' ? DT_CENTER : 0 ) |
			DT_NOPREFIX | DT_SINGLELINE );
		}
	if ( lpNext )
		*lpNext++ = c;
	if ((Area.top = Area.bottom) > WndHeight )
		Area.top -= CastHeight;
	}

if (bScroll)
	RestoreDC( hDC, SaveLevel );

DeleteObject( hUpdate );
DeleteObject( hBrush );
SetTextColor( hDC, OldTextColor );
SetBkMode( hDC, OldBkMode );
if ( hOldFont )
	SelectObject( hDC, hOldFont );
ReleaseDC( hWnd, hDC );
}
