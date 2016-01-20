// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "ctxhelp.h"

// LOCAL prototypes
LOCAL BOOL About_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void About_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void About_OnTimer(HWND hDlg, UINT id);
LOCAL void blink_readme( HWND hDlg, int bReset );
LOCAL void display_cast( HWND hDlg, LPTSTR lp, int bScroll );

// LOCAL storage
static LPTSTR  lpString;
static BOOL    bTrack;
static int     bScroll;

/***********************************************************************/
BOOL WINPROC EXPORT DlgAboutProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, About_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, About_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_TIMER, About_OnTimer);  

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_ABOUT);
	}
}

/************************************************************************/
LOCAL BOOL About_OnInitDialog( HWND hDlg, HWND hWndFocus, LPARAM lParam )
/************************************************************************/
{
STRING szString;

CenterPopup( hDlg );

if (GetDlgItem(hDlg, IDC_SERIALNO))
	{
	GetDefaultString( _T("SN"), _T(""), szString, sizeof(szString) );
	SetDlgItemText( hDlg, IDC_SERIALNO, szString );
	}

GetDefaultString( _T("User"), _T(""), szString, sizeof(szString) );
SetDlgItemText( hDlg, IDC_CUSTOMER, szString );

GetDefaultString( _T("Company"), _T(""), szString, sizeof(szString) );
SetDlgItemText( hDlg, IDC_COMPANY, szString );

GetDefaultString( _T("VersionName"), _T(""), szString, sizeof(szString) );
SetDlgItemText( hDlg, IDC_VERSION_NAME, szString );
lpString = ( LPTSTR )LineBuffer[0];
if ( !TextBlock_GetData( IDC_CASTTEXT, lpString, 16384 ) )
	GetWindowText( hDlg, lpString, 16384 );
blink_readme( GetDlgItem( hDlg, READ_ME ), TRUE );
bScroll = NO;
SetTimer( hDlg, bTrack = TRUE, 2, NULL );
return(TRUE);
}

/************************************************************************/
LOCAL void About_OnTimer( HWND hDlg, UINT id )
/************************************************************************/
{
blink_readme( GetDlgItem(hDlg, READ_ME), FALSE );
display_cast( GetDlgItem(hDlg, IDC_CAST), lpString, bScroll );
bScroll = YES;
}

/************************************************************************/
LOCAL BOOL About_OnSetCursor( HWND hDlg, HWND hWndCursor, UINT codeHitTest,
							UINT msg )
/************************************************************************/
{
    CPoint pt;
    HWND hChild;

    // check for disabled control
    GetCursorPos( &pt );
    if( hChild = MyChildWindowFromPoint( hWndCursor, pt ))
    {
        hWndCursor = hChild;
    }
    return( SetupCursor( hWndCursor, codeHitTest, IDD_ABOUT ));
}

/************************************************************************/
LOCAL void About_OnCommand( HWND hDlg, int id, HWND hControl, UINT codeNotify )
/************************************************************************/
{
switch (id)
	{
	case IDC_ABOUTAPP:
	PopupWindow(hDlg, id, id, DlgDriversProc);
	break;

	case IDC_MGX:
	case IDC_PICTPUB:
   case IDC_PHOTOMAGIC:
   case IDC_DESIGNER:
   case IDC_CHARISMA:
   case IDC_DRAWPLUS:
   case IDC_ORGCHART:
   case IDC_ABCFLOWCHART:
   case IDC_SNAPGRAFX:
   case IDC_CLIPART:
   case IDC_TECHSUPT:
   case IDC_PHOTOCREDITS:
	if ( bTrack )
		KillTimer( hDlg, bTrack );
	bTrack = NO;
	EnableWindow(hDlg, FALSE);
	AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, id, DlgWarningProc );
	EnableWindow(hDlg, TRUE);
	bScroll = NO;
	SetTimer( hDlg, bTrack = TRUE, 2, NULL );
	break;

	case IDOK:
	case IDCANCEL:
	if ( bTrack )
		KillTimer( hDlg, bTrack );
	bTrack = NO;
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

/************************************************************************/
LOCAL void blink_readme( HWND hWnd, BOOL bReset )
/************************************************************************/
{ //  Blink a window on and off by every time a number of steps in completed.
static short nSteps;
#define README_STEPS    12

if ( !hWnd )
	return;
if ( bReset )
	nSteps = 0;
else
if ( ++nSteps >= README_STEPS )
	{
	nSteps = 0;
	ShowWindow( hWnd, IsWindowVisible(hWnd) ? SW_HIDE : SW_SHOWNA );
	}
}


/************************************************************************/
LOCAL void display_cast( HWND hWnd, LPTSTR lpString, BOOL bScroll )
// Display a scrolling list of the Cast, beginning with a new player each time.
/************************************************************************/
{ 
	TEXTMETRIC   	SysCharSize;
	HDC          	hDC;
	RECT         	Area;
	LPTSTR       	lpNext, lpCastName;
	HRGN         	hUpdate;
	int          	SaveLevel, WndHeight, CastHeight, OldBkMode;
	DWORD        	OldTextColor;
	HBRUSH       	hBrush;
	HGDIOBJ      	hFont, hOldFont;
	RECT         	Update, rDummy;
	WORD         	wChar;
	BOOL	   	 	fDBCS, fCenter;
	static short 	CastOfs = -1;

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
		hFont = GetStockObject( GetSystemMetrics( SM_DBCSENABLED ) ? SYSTEM_FONT:ANSI_VAR_FONT );
	hOldFont = SelectObject(hDC, hFont);

	GetClientRect( hWnd, &Area );

	CastHeight = 0;
	lpNext = lpString;
	while ( lpNext )
	{
		CastHeight += SysCharSize.tmHeight;
		lpNext = lStrChr( MyCharNextN( lpNext, 1 ), _T('\n') );
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

	hUpdate = CreateRectRgn(Update.left,Update.top,Update.right,Update.bottom);
	SelectClipRgn( hDC, hUpdate );

	WndHeight = (1 + Area.bottom / SysCharSize.tmHeight) * SysCharSize.tmHeight;
	if ((WndHeight - (CastOfs -= SCROLLCOUNT)) >= CastHeight)
		CastOfs = WndHeight;
	Area.top = CastOfs;

	lpNext = lpString;
	while ( lpNext )
	{
		Area.bottom = Area.top + SysCharSize.tmHeight;

		lpCastName = lpNext;
		if( lpNext = lStrChr( lpNext, _T('\n') ))
		{
			if( IsDBCSLeadByte(( BYTE )*lpNext ))
			{
				fDBCS = TRUE;
				wChar =  *(( LPWORD )lpNext );
			}
			else
			{
				fDBCS = FALSE;
				wChar = *lpNext;
			}
			*lpNext = _T('\0');
		}

		if( IntersectRect( &rDummy, &Area, &Update ))
		{
			FillRect( hDC, &Area, hBrush );

			if( IsDBCSLeadByte(( BYTE )*lpCastName ))
				fCenter = ( *(( LPWORD )lpCastName ) == _T(' ')	);		
			else
				fCenter = ( *lpCastName == _T(' ') );		

			DrawText( hDC, lpCastName, -1, &Area, 
				( fCenter ? DT_CENTER : 0 ) | DT_NOPREFIX | DT_SINGLELINE );
		}

		if( lpNext )
		{
			if( fDBCS )
				*(( LPWORD )lpNext ) = wChar;
			else
				*lpNext = LOBYTE( wChar );

			lpNext = MyCharNext( lpNext );
		}

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