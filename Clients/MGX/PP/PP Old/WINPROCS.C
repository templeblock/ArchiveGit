// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern HANDLE hInstAstral;
extern HWND hWndAstral;

static wMapType;

/***********************************************************************/
BOOL FAR PASCAL DlgHeraldProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
static BOOL bTrack;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	SetTimer( hDlg, bTrack = TRUE, 3000, NULL );
	break;

    case WM_TIMER:
    case WM_LBUTTONDOWN:
    case WM_COMMAND:
    case WM_CLOSE:
	if ( !bTrack )
		break;
	KillTimer( hDlg, bTrack );
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgStatusProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
RECT rect;
TEXTMETRIC tm;
HDC hDC;
int dy, dxShield, dxMask, dxInfo, x;
LPTR lpString;

switch (msg)
    {
    case WM_INITDIALOG:
	hDC = GetDC( hDlg );
	SelectObject( hDC, Window.hHelv10Font );
	if ( AstralStr( IDS_NONE, &lpString ) )
		x = GetTextExtent( hDC, lpString, lstrlen(lpString) );
	else	x = 50;
	dxShield = 6 + x + 32;
	if ( AstralStr( IDS_OUTSIDE, &lpString ) )
		x = GetTextExtent( hDC, lpString, lstrlen(lpString) );
	else	x = 50;
	dxMask   = 6 + x + 32;
	dxInfo = 6 + 22 + 32;
	GetTextMetrics( hDC, &tm );
	dy = tm.tmHeight /*(tmAscent+tmDescent)*/ + tm.tmInternalLeading + 4;
	ReleaseDC( hDlg, hDC );

	GetClientRect( hWndAstral, &rect );
	InflateRect( &rect, 1, 1 );
	rect.top = rect.bottom - dy;
	MoveWindow( hDlg,
		rect.left, rect.top, /* New position */
		RectWidth( &rect ), RectHeight( &rect ), NO /* New size */ );

	// Convert the rectangle to client coordinates for moving the controls
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	rect.left = rect.top = 0;

	// Adjust the rectangle so that the controls share the status border
	rect.top -= 1;
	rect.bottom -= 2;
	rect.left -= 1;
	rect.right -= 2;
	rect.left = rect.right - dxShield;
	MoveWindow( GetDlgItem(hDlg,IDC_STATUSSHIELD), rect.left, rect.top,
		RectWidth( &rect ), RectHeight( &rect ), NO /* New size */ );
	rect.right = rect.left;
	rect.left = rect.right - dxMask;
	MoveWindow( GetDlgItem(hDlg,IDC_STATUSMASK), rect.left, rect.top,
		RectWidth( &rect ), RectHeight( &rect ), NO /* New size */ );
	rect.right = rect.left;
	rect.left = rect.right - dxInfo;
	MoveWindow( GetDlgItem(hDlg,IDC_STATUSINFO), rect.left, rect.top,
		RectWidth( &rect ), RectHeight( &rect ), NO /* New size */ );
	rect.right = rect.left;
	rect.left = -1;
	MoveWindow( GetDlgItem(hDlg,IDC_STATUSTEXT), rect.left, rect.top,
		RectWidth( &rect ), RectHeight( &rect ), NO /* New size */ );
	UpdateStatusBar( YES, YES, YES, YES );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

//    case WM_LBUTTONDBLCLK:
//	UpdateStatusBar( NO, YES, YES, YES );
//	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	switch (wParam)
	    {
	    case IDC_STATUSINFO:
		AstralDlg( NO, hInstAstral, hDlg, IDD_IMAGEINFO, DlgInfoProc);
		break;

	    case IDC_STATUSSHIELD:
		ColorMask.Off = !ColorMask.Off;
		UpdateStatusBar( NO, NO, NO, YES );
//		if ( lParam == 1 || (!ColorMask.Mask && !ColorMask.Off) )
		if ( lParam == 1 )
			{ // If dbl click or no active bits and just turned on
			AstralDlg( YES, hInstAstral, hWndAstral,
				IDD_SHIELDFLOAT, DlgShieldFloatProc );
			break;
			}
		break;

	    case IDC_STATUSMASK:
		Mask.IsOutside = !Mask.IsOutside;
		UpdateStatusBar( NO, NO, YES, NO );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
void UpdateStatusBar( bMemory, bCache, bMask, bShield )
/***********************************************************************/
BOOL bMemory, bCache, bMask, bShield;
{
HWND hStatus, hWnd;
int iCount, Shield;
WORD id;
long l;
LPTR lp, lpString;
STRING szString;
BOOL UseEMS;

if ( !(hStatus = AstralDlgGet(IDD_STATUS)) )
	return;
#ifdef UNUSED
if ( bMemory && (hWnd = GetDlgItem( hStatus, IDC_STATUSMEMORY )) )
	{
	l = (ConventionalMemory()+512) / 1024L;
	lpString = szString;
	wsprintf( lpString, "%ldK", l );
	EMSstatus( &Control.EMStotal, &Control.EMSavail );
	if ( Control.EMStotal )
		{
		lpString += lstrlen(lpString);
		UseEMS = ( !Control.EMSdisable && Control.EMStotal &&
			Control.EMSavail);
		wsprintf( lpString, " + %uKexp", Control.EMSavail );
		}
	SetWindowText( hWnd, szString );
	InvalidateRect( hWnd, NULL, FALSE );
	UpdateWindow( hWnd );
	}
if ( bCache && (hWnd = GetDlgItem( hStatus, IDC_STATUSCACHE )) )
	{
	if ( frame_set( NULL ) )
		{
		lpString = szString;
		if ( !AstralStr( IDS_INMEMORY, &lp ) )
			return;
		wsprintf( lpString, lp, frame_inmemory() );
		}
	else	{
		if ( !AstralStr( IDS_NOIMAGE, &lpString ) )
			return;
		}
	SetWindowText( hWnd, lpString );
	InvalidateRect( hWnd, NULL, FALSE );
	UpdateWindow( hWnd );
	}
#endif
if ((bMemory || bCache) && (hWnd = GetDlgItem( hStatus, IDC_STATUSINFO )) )
	{
	ControlEnable( hStatus, IDC_STATUSINFO, lpImage != NULL );
	InvalidateRect( hWnd, NULL, FALSE );
	UpdateWindow( hWnd );
	}
if ( bMask && (hWnd = GetDlgItem( hStatus, IDC_STATUSMASK )) )
	{
	ControlEnable( hStatus, IDC_STATUSMASK, mask_active() );
	id = (Mask.IsOutside ? IDS_OUTSIDE : IDS_INSIDE);
	if ( !AstralStr( id, &lpString ) )
		return;
	SetWindowText( hWnd, lpString );
	InvalidateRect( hWnd, NULL, FALSE );
	UpdateWindow( hWnd );
	}
if ( bShield && (hWnd = GetDlgItem( hStatus, IDC_STATUSSHIELD )) )
	{
	if ( ColorMask.Off )
		{
		if ( !AstralStr( IDS_NONE, &lpString ) )
			return;
		}
	else	{
		iCount = 0;
		Shield = ColorMask.Mask;
		while ( Shield )
			{
			iCount += ( Shield & 1 );
			Shield >>= 1;
			}
		lpString = itoa(iCount,szString,10);
		}
	SetWindowText( hWnd, lpString );
	InvalidateRect( hWnd, NULL, FALSE );
	UpdateWindow( hWnd );
	}
}


/***********************************************************************/
BOOL FAR PASCAL DlgWarningProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;
{
int i;
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDOK:
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgAboutProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;
{
int i;
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
//	blink_readme (hDlg,TRUE);
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

//    case WM_TIMER:
//	display_cast (hDlg,TRUE);
//	blink_readme (hDlg,FALSE);
//	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case ABOUT_CAST:
//		if ( HIWORD(lParm) != BN_DISABLE)
//			break;
//		display_cast (hDlg,FALSE);
//		SetTimer (hDlg,NULL,CASTPAUSE,NULL);
		break;

	    case IDC_DESIGNER:
	    case IDC_CHARISMA:
	    case IDC_DRAWPLUS:
	    case IDC_CLIPART:
	    case IDC_TECHSUPT:
		AstralDlg( NO, hInstAstral, hWndAstral, wParam, DlgAboutProc );
		break;

	    case IDOK:
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgPasteProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;
{
int i, Opacity;
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	RightPopup( hDlg );
	CheckDlgButton( hDlg, IDC_PASTEANDBLEND, Mask.PasteBlend );
	CheckDlgButton( hDlg, wParam, Control.SmartPaste );
	InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Mask.Opacity), NO,
		0, 100 );
	SetScrollBar( hDlg, IDC_TRANSPARENT_SCROLL,
			TOPERCENT(255-Mask.Opacity), 0, 100 );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	switch ( GetDlgCtrlID( HIWORD( lParam )) )
	    {
	    case IDC_TRANSPARENT_SCROLL:
		i = HandleScrollBar( hDlg,
			IDC_TRANSPARENT_SCROLL, wParam, lParam, 0, 100 );
		Opacity = 255-TOGRAY(i);
		SetDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Opacity),
			YES );
		if (Mask.Opacity != Opacity && wParam != SB_THUMBTRACK)
			{
			Mask.Opacity = Opacity;
			PasteRedisplay();
			}
		break;
	    default:
		HandleMiniScroll( hDlg, wParam, lParam );
		break;
	    }
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_PASTEANDBLEND:
		Mask.PasteBlend = !Mask.PasteBlend;
		CheckDlgButton( hDlg, wParam, Mask.PasteBlend );
		break;

	    case IDC_SMARTPASTE:
		Control.SmartPaste = !Control.SmartPaste;
		CheckDlgButton( hDlg, wParam, Control.SmartPaste );
		break;
		
	    case IDC_TRANSPARENT:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Mask.Opacity = 255-TOGRAY(i);
		SetScrollBar( hDlg, IDC_TRANSPARENT_SCROLL,
			TOPERCENT(255-Mask.Opacity), 0, 100 );
		break;

	    case IDOK:
		AstralDlgEnd( hDlg, TRUE );
		if (Mask.Opacity != Opacity)
			PasteRefresh();
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgMapProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;
{
BOOL Bool;
int i, x, y;
STRING szString;
FNAME szFileName, szExtName;
static WORD wChannel, wTool;
static MAP *lpMap; //since this is a static pointer to a static structure,
// don't make it a far pointer because the data segment might move

switch (msg)
    {
    case WM_INITDIALOG:
	RightPopup( hDlg );
	ResetAllMaps();
	CheckComboItem( hDlg, IDC_CHANNELS, IDC_DO_ALL, IDC_DO_YELLOW,
		wChannel = IDC_DO_ALL );
	lpMap = ResetMapChannel( hDlg, wChannel );
	lstrcpy( Names.Saved, Names.Map );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Map, IDN_MAP );
	CheckComboItem( hDlg, IDC_MAPTOOL, IDC_MAPTOOL1, IDC_MAPTOOL2,
		IDC_MAPTOOL1 );
	ActivateMapControls( hDlg, IDC_MAPTOOL1, &wTool, NO );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_CHANNELS:
//	    case IDC_DO_ALL:
//	    case IDC_DO_RED:
//	    case IDC_DO_GREEN:
//	    case IDC_DO_BLUE:
//	    case IDC_DO_CYAN:
//	    case IDC_DO_MAGENTA:
//	    case IDC_DO_YELLOW:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		wChannel = wParam;
		lpMap = ResetMapChannel( hDlg, wChannel );
		break;

	    case IDC_PREVIEW:
		MapPreview();
		break;

	    case IDC_RESETALL:
		ResetAllMaps();
		lpMap = ResetMapChannel( hDlg, wChannel );
		AnimateMap( lpMap );
		UndoMapPreview( IDCANCEL );
		break;

	    case IDC_RESET:
		ResetMap( lpMap, MAPPOINTS );
		lpMap = ResetMapChannel( hDlg, wChannel );
		AnimateMap( lpMap );
		if (Previewing())
			MapPreview();
		break;

	    case IDOK:
		for ( i=0; i<10; i++ )
			if ( lpMap->Pnt[i].x > lpMap->Pnt[i+1].x )
				break;
		if ( i != 10 )
			{
			Message( IDS_EMAPASC );
			break;
			}
		if ( !lpImage )
			{
			AstralDlgEnd( hDlg, TRUE );
			break;
			}
		ApplyMap(IDS_UNDOMAP);
		ResetAllMaps();
		AnimateMap( NULL );
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		lstrcpy( Names.Map, Names.Saved );
		if ( !lpImage )
			{
			AstralDlgEnd( hDlg, FALSE );
			break;
			}
		ResetAllMaps();
		AnimateMap( NULL );
		UndoMapPreview( IDCANCEL );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_MAPTOOL:
//	    case IDC_MAPTOOL1:
//	    case IDC_MAPTOOL2:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		UndoMapPreview( NO );
		for ( i=0; i<10; i++ )
			if ( lpMap->Pnt[i].x > lpMap->Pnt[i+1].x )
				break;
		if ( i != 10 )
		{
			Message( IDS_EMAPASC );
			break;
			}
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		ActivateMapControls( hDlg, wParam, &wTool, YES );
		break;

//	    case IDC_EXTNAMES:
//		if ( HIWORD(lParam) != CBN_SELCHANGE )
//			break;
//		GetExtName( hDlg, wParam, IDC_FILENAME, Names.Map,
//			IDN_MAP, HIWORD(lParam) );
//		if ( !LookupExtFile( Names.Map, szFileName, IDN_MAP ) )
//			break;
//		if ( !LoadMap( &MasterMap, &RedMap, &GreenMap, &BlueMap,
//		     szFileName ) )
//			break;
//		lpMap = ResetMapChannel( hDlg, wChannel );
//		UndoMapPreview( NO );
//		AnimateMap( lpMap );
//		break;

//	    case IDC_MAPEXTMANAGE:
//		PopupMenu( hDlg, wParam, IDC_FILENAME );
//		break;

	    case IDC_MAPLOAD:
		if ( !PromptForExtName( hDlg, Names.Map, szFileName, IDN_MAP ) )
			break;
		if ( !LoadMap( &MasterMap, &RedMap, &GreenMap, &BlueMap,
		     szFileName ) )
			break;
//		InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Map,
//			IDN_MAP );
		lpMap = ResetMapChannel( hDlg, wChannel );
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    case IDC_MAPSAVE:
		szExtName[0] = '\0';
		if ( !PromptForExtName( hDlg, szExtName, szFileName, IDN_MAP ) )
			break;
		if ( !SaveMap( &MasterMap, &RedMap, &GreenMap, &BlueMap,
		     szFileName ) )
			break;
		if ( !AddExtFile( szExtName, szFileName, IDN_MAP ) )
			{
			unlink( szFileName );
			break;
			}
		lstrcpy( Names.Map, szExtName );
		InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Map,
			IDN_MAP );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Map,
			IDN_MAP, wParam, NO );
		break;

	    case IDC_MAPNEGATE:
		lpMap->Negate = !lpMap->Negate;
		CheckDlgButton( hDlg, IDC_MAPNEGATE, lpMap->Negate );
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    case IDC_MAP:
		i = LOWORD(lParam);
		if ( i >= 0 )
			{
			x = TOPERCENT( lpMap->Pnt[i].x );
			y = TOPERCENT( lpMap->Pnt[i].y );
			SetDlgItemInt( hDlg, IDC_MAP0IN+i, x, NO );
			SetDlgItemInt( hDlg, IDC_MAP0OUT+i, y, NO );
			UndoMapPreview( NO );
			AnimateMap( lpMap );
			wsprintf( szString, "%d.", i+1 );
			SetDlgItemText( hDlg, IDC_MAPSTEP, szString );
			}
		else	{// Just a notification of the x coordinate
			x = TOPERCENT( HIWORD(lParam) );
			y = TOPERCENT( 255-lpMap->Lut[255-HIWORD(lParam)] );
			SetDlgItemText( hDlg, IDC_MAPSTEP, "" );
			}
		wsprintf( szString, "%d %%", x );
		SetDlgItemText( hDlg, IDC_MAPIN, szString );
		wsprintf( szString, "%d %%", y );
		SetDlgItemText( hDlg, IDC_MAPOUT, szString );
		break;

	    case IDC_MAP0IN:
	    case IDC_MAP1IN:
	    case IDC_MAP2IN:
	    case IDC_MAP3IN:
	    case IDC_MAP4IN:
	    case IDC_MAP5IN:
	    case IDC_MAP6IN:
	    case IDC_MAP7IN:
	    case IDC_MAP8IN:
	    case IDC_MAP9IN:
	    case IDC_MAP10IN:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		lpMap->Pnt[wParam-IDC_MAP0IN].x = TOGRAY(i);
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    case IDC_MAP0OUT:
	    case IDC_MAP1OUT:
	    case IDC_MAP2OUT:
	    case IDC_MAP3OUT:
	    case IDC_MAP4OUT:
	    case IDC_MAP5OUT:
	    case IDC_MAP6OUT:
	    case IDC_MAP7OUT:
	    case IDC_MAP8OUT:
	    case IDC_MAP9OUT:
	    case IDC_MAP10OUT:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		lpMap->Pnt[wParam-IDC_MAP0OUT].y = TOGRAY(i);
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
MAP *ResetMapChannel( hDlg, wChannel )
/***********************************************************************/
HWND hDlg;
WORD wChannel;
{
int i;
MAP *lpMap; //since this is a static pointer to a static structure,
// don't make it a far pointer because the data segment might move

lpMap = ActivateChannel( wChannel );
SetDlgItemInt( hDlg, IDC_MAP, lpMap->Points, NO );
SetWindowLong( GetDlgItem( hDlg, IDC_MAP ), 0, (long)(WORD)lpMap->Pnt );
AstralControlPaint( hDlg, IDC_MAP );
SetDlgItemText( hDlg, IDC_MAPSTEP, "" );
SetDlgItemText( hDlg, IDC_MAPIN,   "" );
SetDlgItemText( hDlg, IDC_MAPOUT,  "" );
SetDlgItemInt( hDlg, IDC_MAP0IN,  TOPERCENT(lpMap->Pnt[0].x), NO );
SetDlgItemInt( hDlg, IDC_MAP0OUT, TOPERCENT(lpMap->Pnt[0].y), NO );
SetDlgItemInt( hDlg, IDC_MAP1IN,  TOPERCENT(lpMap->Pnt[1].x), NO );
SetDlgItemInt( hDlg, IDC_MAP1OUT, TOPERCENT(lpMap->Pnt[1].y), NO );
SetDlgItemInt( hDlg, IDC_MAP2IN,  TOPERCENT(lpMap->Pnt[2].x), NO );
SetDlgItemInt( hDlg, IDC_MAP2OUT, TOPERCENT(lpMap->Pnt[2].y), NO );
SetDlgItemInt( hDlg, IDC_MAP3IN,  TOPERCENT(lpMap->Pnt[3].x), NO );
SetDlgItemInt( hDlg, IDC_MAP3OUT, TOPERCENT(lpMap->Pnt[3].y), NO );
SetDlgItemInt( hDlg, IDC_MAP4IN,  TOPERCENT(lpMap->Pnt[4].x), NO );
SetDlgItemInt( hDlg, IDC_MAP4OUT, TOPERCENT(lpMap->Pnt[4].y), NO );
SetDlgItemInt( hDlg, IDC_MAP5IN,  TOPERCENT(lpMap->Pnt[5].x), NO );
SetDlgItemInt( hDlg, IDC_MAP5OUT, TOPERCENT(lpMap->Pnt[5].y), NO );
SetDlgItemInt( hDlg, IDC_MAP6IN,  TOPERCENT(lpMap->Pnt[6].x), NO );
SetDlgItemInt( hDlg, IDC_MAP6OUT, TOPERCENT(lpMap->Pnt[6].y), NO );
SetDlgItemInt( hDlg, IDC_MAP7IN,  TOPERCENT(lpMap->Pnt[7].x), NO );
SetDlgItemInt( hDlg, IDC_MAP7OUT, TOPERCENT(lpMap->Pnt[7].y), NO );
SetDlgItemInt( hDlg, IDC_MAP8IN,  TOPERCENT(lpMap->Pnt[8].x), NO );
SetDlgItemInt( hDlg, IDC_MAP8OUT, TOPERCENT(lpMap->Pnt[8].y), NO );
SetDlgItemInt( hDlg, IDC_MAP9IN,  TOPERCENT(lpMap->Pnt[9].x), NO );
SetDlgItemInt( hDlg, IDC_MAP9OUT, TOPERCENT(lpMap->Pnt[9].y), NO );
SetDlgItemInt( hDlg, IDC_MAP10IN,  TOPERCENT(lpMap->Pnt[10].x), NO );
SetDlgItemInt( hDlg, IDC_MAP10OUT, TOPERCENT(lpMap->Pnt[10].y), NO );
CheckDlgButton( hDlg, IDC_MAPNEGATE, lpMap->Negate );
return( lpMap );
}


/***********************************************************************/
void ActivateMapControls( hDlg, wTool, lpOldTool, bDelay )
/***********************************************************************/
HWND hDlg;
WORD wTool;
LPWORD lpOldTool;
BOOL bDelay;
{
int i;
BOOL bShow, bFirstTimeHide;

if ( bDelay && wTool == *lpOldTool )
	return;
if ( !wTool )
	wTool = *lpOldTool;

AstralCursor( IDC_WAIT );
bShow = NO;
Top:
bFirstTimeHide = (!bShow && !bDelay);
if ( *lpOldTool == IDC_MAPTOOL1 || bFirstTimeHide )
	ShowControl( hDlg, IDC_MAP, IDC_MAPOUT, bShow, bDelay );
if ( *lpOldTool == IDC_MAPTOOL2 || bFirstTimeHide )
	ShowControl( hDlg, IDC_MAPSTEP0, IDC_MAP10OUT, bShow, bDelay );
if ( !bShow )
	{
	bShow = YES;
	*lpOldTool = wTool;
	goto Top;
	}
ShowControl( NULL, NULL, NULL, NULL, NULL );
SetDlgItemText( hDlg, IDC_MAPSTEP, "" );
SetDlgItemText( hDlg, IDC_MAPIN,   "" );
SetDlgItemText( hDlg, IDC_MAPOUT,  "" );
AstralCursor( NULL );
}


/***********************************************************************/
BOOL FAR PASCAL DlgContBritProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL	 Bool;
static WORD wChannel;
static MAP *lpMap; //since this is a static pointer to a static structure,
// don't make it a far pointer because the data segment might move

switch (msg)
    {
    case WM_INITDIALOG:
	RightPopup( hDlg );
	ResetAllMaps();
	CheckComboItem( hDlg, IDC_CHANNELS, IDC_DO_ALL, IDC_DO_YELLOW,
		wChannel = IDC_DO_ALL );
	lpMap = ResetCBChannel( hDlg, wChannel );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_CHANNELS:
//	    case IDC_DO_ALL:
//	    case IDC_DO_RED:
//	    case IDC_DO_GREEN:
//	    case IDC_DO_BLUE:
//	    case IDC_DO_CYAN:
//	    case IDC_DO_MAGENTA:
//	    case IDC_DO_YELLOW:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		wChannel = wParam;
		lpMap = ResetCBChannel( hDlg, wChannel );
		break;

	    case IDC_PREVIEW:
		MapPreview();
		break;

	    case IDC_RESETALL:
		ResetAllMaps();
		lpMap = ResetCBChannel( hDlg, wChannel );
		AnimateMap( lpMap );
		UndoMapPreview( IDCANCEL );
		break;

	    case IDC_RESET:
		ResetMap( lpMap, MAPPOINTS );
		lpMap = ResetCBChannel( hDlg, wChannel );
		AnimateMap( lpMap );
		if (Previewing())
			MapPreview();
		break;

	    case IDOK:
		if ( !lpImage )
			{
			AstralDlgEnd( hDlg, TRUE );
			break;
			}
		ApplyMap(IDS_UNDOCONTBRIT); // ApplyMap always undoes the Preview
		ResetAllMaps();
		AnimateMap( NULL );
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		if ( !lpImage )
			{
			AstralDlgEnd( hDlg, FALSE );
			break;
			}
		ResetAllMaps();
		AnimateMap( NULL );
		UndoMapPreview( IDCANCEL );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_CONTBRIT:
		if ( GetFocus() != GetDlgItem( hDlg, IDC_CONTBRIT ) )
			break;
		lpMap->Brightness = (int)LOWORD(lParam);
		lpMap->Contrast = (int)HIWORD(lParam);
		SetDlgItemSpin( hDlg, IDC_CONTRAST, lpMap->Contrast, YES );
		SetDlgItemSpin( hDlg, IDC_BRIGHTNESS, lpMap->Brightness, YES );
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    case IDC_CONTRAST:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		lpMap->Contrast = (int)GetDlgItemSpin( hDlg, IDC_CONTRAST,
			&Bool, YES );
		SetWindowLong( GetDlgItem( hDlg, IDC_CONTBRIT ), 0,
			MAKELONG(lpMap->Brightness,lpMap->Contrast) );
		AstralControlPaint( hDlg, IDC_CONTBRIT );
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    case IDC_BRIGHTNESS:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		lpMap->Brightness = (int)GetDlgItemSpin( hDlg, IDC_BRIGHTNESS,
			&Bool, YES );
		SetWindowLong( GetDlgItem( hDlg, IDC_CONTBRIT ), 0,
			MAKELONG(lpMap->Brightness,lpMap->Contrast) );
		AstralControlPaint( hDlg, IDC_CONTBRIT );
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    case IDC_MAPNEGATE:
		lpMap->Negate = !lpMap->Negate;
		CheckDlgButton( hDlg, IDC_MAPNEGATE, lpMap->Negate );
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    default:
		return( FALSE );
		break;
	    }

    default:
	return( FALSE );
	break;
    }

return( TRUE );
}


/***********************************************************************/
MAP *ResetCBChannel( hDlg, wChannel )
/***********************************************************************/
HWND hDlg;
WORD wChannel;
{
MAP *lpMap; //since this is a static pointer to a static structure,
// don't make it a far pointer because the data segment might move

lpMap = ActivateChannel( wChannel );
SetWindowLong( GetDlgItem( hDlg, IDC_CONTBRIT ), 0,
	MAKELONG(lpMap->Brightness,lpMap->Contrast) );
InitDlgItemSpin( hDlg, IDC_CONTRAST, lpMap->Contrast, YES, -100, 100 );
InitDlgItemSpin( hDlg, IDC_BRIGHTNESS, lpMap->Brightness, YES, -100, 100 );
CheckDlgButton( hDlg, IDC_MAPNEGATE, lpMap->Negate );
AstralControlPaint( hDlg, IDC_CONTBRIT );
return( lpMap );
}


/***********************************************************************/
MAP *ResetPosterChannel( hDlg, wChannel )
/***********************************************************************/
HWND hDlg;
WORD wChannel;
{
MAP *lpMap; //since this is a static pointer to a static structure,
// don't make it a far pointer because the data segment might move

lpMap = ActivateChannel( wChannel );
SetScrollBar( hDlg, IDC_MAPLEVELS_SCROLL, lpMap->Levels, 0, 255 );
SetDlgItemInt( hDlg, IDC_MAPLEVELS, lpMap->Levels, NO );
SetScrollBar( hDlg, IDC_MAPTHRESH_SCROLL, lpMap->Threshold, 0, 100 );
SetDlgItemInt( hDlg, IDC_MAPTHRESH, lpMap->Threshold, NO );
CheckDlgButton( hDlg, IDC_MAPNEGATE, lpMap->Negate );
AstralControlPaint( hDlg, IDC_MAPLEVELS_SCROLL );
AstralControlPaint( hDlg, IDC_MAPTHRESH_SCROLL );
return( lpMap );
}


/***********************************************************************/
MAP *ActivateChannel( wChannel )
/***********************************************************************/
WORD wChannel;
{
BOOL bReverse;
MAP *lpMap; //since this is a static pointer to a static structure,
// don't make it a far pointer because the data segment might move

switch ( wChannel )
    {
    case IDC_DO_ALL:
	lpMap = &MasterMap; bReverse = NO; lpMap->Color = LMAP;
	break;
    case IDC_DO_RED:
	lpMap = &RedMap;   bReverse = (lpMap->Color == CMAP); lpMap->Color=RMAP;
	break;
    case IDC_DO_GREEN:
	lpMap = &GreenMap; bReverse = (lpMap->Color == MMAP); lpMap->Color=GMAP;
	break;
    case IDC_DO_BLUE:
	lpMap = &BlueMap;  bReverse = (lpMap->Color == YMAP); lpMap->Color=BMAP;
	break;
    case IDC_DO_CYAN:
	lpMap = &RedMap;   bReverse = (lpMap->Color == RMAP); lpMap->Color=CMAP;
	break;
    case IDC_DO_MAGENTA:
	lpMap = &GreenMap; bReverse = (lpMap->Color == GMAP); lpMap->Color=MMAP;
	break;
    case IDC_DO_YELLOW:
	lpMap = &BlueMap;  bReverse = (lpMap->Color == BMAP); lpMap->Color=YMAP;
	break;
    default:
	return( lpMap );
    }

if ( bReverse )
	InvertMap( lpMap );
return( lpMap );
}


/***********************************************************************/
BOOL FAR PASCAL DlgPosterizeProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL	 Bool;
static WORD wChannel;
static MAP *lpMap; //since this is a static pointer to a static structure,
// don't make it a far pointer because the data segment might move

switch (msg)
    {
    case WM_INITDIALOG:
	RightPopup( hDlg );
	ResetAllMaps();
	CheckComboItem( hDlg, IDC_CHANNELS, IDC_DO_ALL, IDC_DO_YELLOW,
		wChannel = IDC_DO_ALL );
	lpMap = ResetPosterChannel( hDlg, wChannel );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	switch ( GetDlgCtrlID( HIWORD( lParam )) )
	    {
	    case IDC_MAPLEVELS_SCROLL:
		lpMap->Levels = HandleScrollBar( hDlg,
			IDC_MAPLEVELS_SCROLL, wParam, lParam, 0, 255 );
		SetDlgItemInt( hDlg, IDC_MAPLEVELS, lpMap->Levels, NO );
		AnimateMap( lpMap );
		break;
	    case IDC_MAPTHRESH_SCROLL:
		lpMap->Threshold = HandleScrollBar( hDlg,
			IDC_MAPTHRESH_SCROLL, wParam, lParam, 0, 100 );
		SetDlgItemInt( hDlg, IDC_MAPTHRESH, lpMap->Threshold, NO );
		AnimateMap( lpMap );
		break;
	    default:
		HandleMiniScroll( hDlg, wParam, lParam );
		break;
	    }
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_CHANNELS:
//	    case IDC_DO_ALL:
//	    case IDC_DO_RED:
//	    case IDC_DO_GREEN:
//	    case IDC_DO_BLUE:
//	    case IDC_DO_CYAN:
//	    case IDC_DO_MAGENTA:
//	    case IDC_DO_YELLOW:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		wChannel = wParam;
		lpMap = ResetPosterChannel( hDlg, wChannel );
		break;

	    case IDC_PREVIEW:
		MapPreview();
		break;

	    case IDC_RESETALL:
		ResetAllMaps();
		lpMap = ResetPosterChannel( hDlg, wChannel );
		AnimateMap( lpMap );
		if (Previewing())
			MapPreview();
		break;

	    case IDC_RESET:
		ResetMap( lpMap, MAPPOINTS );
		lpMap = ResetPosterChannel( hDlg, wChannel );
		AnimateMap( lpMap );
		UndoMapPreview( IDCANCEL );
		break;

	    case IDOK:
		if ( !lpImage )
			{
			AstralDlgEnd( hDlg, TRUE );
			break;
			}
		ApplyMap(IDS_UNDOPOSTERIZE);
		ResetAllMaps();
		AnimateMap( NULL );
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		if ( !lpImage )
			{
			AstralDlgEnd( hDlg, FALSE );
			break;
			}
		ResetAllMaps();
		AnimateMap( NULL );
		UndoMapPreview( IDCANCEL );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_MAPNEGATE:
		lpMap->Negate = !lpMap->Negate;
		CheckDlgButton( hDlg, IDC_MAPNEGATE, lpMap->Negate );
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    case IDC_MAPLEVELS:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		lpMap->Levels = GetDlgItemInt( hDlg, IDC_MAPLEVELS,
			&Bool, NO );
		SetScrollBar( hDlg, IDC_MAPLEVELS_SCROLL,
			lpMap->Levels, 0, 255 );
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    case IDC_MAPTHRESH:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		lpMap->Threshold = GetDlgItemInt( hDlg, IDC_MAPTHRESH,
			&Bool, NO );
		SetScrollBar( hDlg, IDC_MAPTHRESH_SCROLL,
			lpMap->Threshold, 0, 100 );
		UndoMapPreview( NO );
		AnimateMap( lpMap );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgHueProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
int iSlot;
int hueDefault, hue;

switch (msg)
    {
    case WM_INITDIALOG:
	RightPopup( hDlg );
	ResetMap( &HueMap, MAXPOINTS );
	ResetMap( &SatMap, MAPPOINTS );
	ResetHues( hDlg );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	switch ( GetDlgCtrlID( HIWORD( lParam )) )
	    {
	    case IDC_SATURATION_SCROLL:
		SatMap.Brightness = HandleScrollBar( hDlg,
			IDC_SATURATION_SCROLL, wParam, lParam, -100, 100 );
		SetDlgItemInt( hDlg, IDC_SATURATION, SatMap.Brightness, YES );
		SatMap.Brightness = -SatMap.Brightness;
		UndoMapPreview( NO );
		AnimateMap( &SatMap );
		break;
	    default:
		HandleMiniScroll( hDlg, wParam, lParam );
		break;
	    }
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_PREVIEW:
		MapPreview();
		break;

	    case IDC_RESETALL:
	    case IDC_RESET:
		ResetMap( &HueMap, MAXPOINTS );
		ResetMap( &SatMap, MAPPOINTS );
		ResetHues( hDlg );
		AnimateMap( &HueMap );
		UndoMapPreview( IDCANCEL );
		break;

	    case IDOK:
		if ( !lpImage )
			{
			AstralDlgEnd( hDlg, TRUE );
			break;
			}
		ApplyMap(IDS_UNDOHUE);
		ResetAllMaps();
		AnimateMap( NULL );
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		if ( !lpImage )
			{
			AstralDlgEnd( hDlg, FALSE );
			break;
			}
		ResetAllMaps();
		AnimateMap( NULL );
		UndoMapPreview( IDCANCEL );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_HUE1:
	    case IDC_HUE2:
	    case IDC_HUE3:
	    case IDC_HUE4:
	    case IDC_HUE5:
	    case IDC_HUE6:
	    case IDC_HUE7:
	    case IDC_HUE8:
	    case IDC_HUE9:
	    case IDC_HUE10:
	    case IDC_HUE11:
	    case IDC_HUE12:
		iSlot = wParam - IDC_HUE1;
		if ( iSlot >= HueMap.Points )
			break;
		hue = (int)LOWORD(lParam);
		hueDefault = (int)HIWORD(lParam);
		HueMap.Pnt[iSlot].y = hue;
		UndoMapPreview( NO );
		AnimateMap( &HueMap );
		break;

	    case IDC_SATURATION:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		SatMap.Brightness = GetDlgItemInt( hDlg, IDC_SATURATION,
			&Bool, YES );
		SetScrollBar( hDlg, IDC_SATURATION_SCROLL,
			SatMap.Brightness, -100, 100 );
		SatMap.Brightness = -SatMap.Brightness;
		UndoMapPreview( NO );
		AnimateMap( &SatMap );
		break;

	    case IDC_MAPNEGATE:
		HueMap.Negate = !HueMap.Negate;
		CheckDlgButton( hDlg, IDC_MAPNEGATE, HueMap.Negate );
		UndoMapPreview( NO );
		AnimateMap( &HueMap );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
void ResetHues( hDlg )
/***********************************************************************/
HWND hDlg;
{
int i, hueDefault, hue;

SetScrollBar( hDlg, IDC_SATURATION_SCROLL, -SatMap.Brightness, -100, 100 );
SetDlgItemInt( hDlg, IDC_SATURATION, -SatMap.Brightness, YES );
for ( i=0; i<HueMap.Points; i++ )
	{
	hueDefault = FMUL( i, FGET( MAX_HUES, HueMap.Points ) );
	HueMap.Pnt[i].x = hueDefault;
	HueMap.Pnt[i].y = hueDefault;
	hue = HueMap.Pnt[i].y;
	SetWindowLong( GetDlgItem( hDlg, IDC_HUE1+i ), 0,
		MAKELONG(hue,hueDefault) );
	}
CheckDlgButton( hDlg, IDC_MAPNEGATE, HueMap.Negate );

AstralControlPaint( hDlg, IDC_SATURATION_SCROLL );
for ( i=0; i<HueMap.Points; i++ )
	AstralControlPaint( hDlg, IDC_HUE1+i );
}


/***********************************************************************/
BOOL FAR PASCAL DlgQuarterProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
int i, iHistoValue;
static LPWORD lpRHisto, lpGHisto, lpBHisto;
static WORD wChannel;
static MAP *lpMap; //since this is a static pointer to a static structure,
// don't make it a far pointer because the data segment might move

switch (msg)
    {
    case WM_INITDIALOG:
	RightPopup( hDlg );
	lpMap = ActivateChannel(IDC_DO_ALL);
	ResetMap(lpMap, QTONEPOINTS);
	Qtone.ActiveMark = 0;
	for (i = 0; i < QTONEPOINTS; ++i)
		Qtone.QTmoved[i] = NO;
	CreateHisto(frame_set(NULL), &lpRHisto, &lpGHisto, &lpBHisto);
	SetControlValue( hDlg, IDC_RHISTO, 1, (long)lpRHisto, NO);
	SetControlValue( hDlg, IDC_GHISTO, 1, (long)lpGHisto, NO);
	SetControlValue( hDlg, IDC_BHISTO, 1, (long)lpBHisto, NO);
	SetControlValue( hDlg, IDC_QUARTERTONES, 0, (long)(LPTR)lpMap, NO);
	SetHistoValues( hDlg, lpMap, lpRHisto, lpGHisto, lpBHisto, NO);
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_QUARTERTONES:
		UndoMapPreview( NO );
		Qtone.ActiveMark = HIWORD(lParam);
		ModifyQuartertones(lpMap, Qtone.ActiveMark, LOWORD(lParam));
		SetHistoValues( hDlg, lpMap, lpRHisto, lpGHisto, lpBHisto, YES);
		break;

	    case IDC_PREVIEW:
		MapPreview();
		break;

	    case IDC_RESETALL:
	    case IDC_RESET:
		ResetMap(lpMap, QTONEPOINTS);
		Qtone.ActiveMark = 0;
		for (i = 0; i < QTONEPOINTS; ++i)
			Qtone.QTmoved[i] = NO;
		SetControlValue( hDlg, IDC_QUARTERTONES, 0, (long)(LPTR)lpMap,
			YES);
		SetControlValue( hDlg, IDC_RHISTO, 0,
			(long)lpMap->Pnt[Qtone.ActiveMark].x, YES);
		SetControlValue( hDlg, IDC_GHISTO, 0,
			(long)lpMap->Pnt[Qtone.ActiveMark].x, YES);
		SetControlValue( hDlg, IDC_BHISTO, 0,
			(long)lpMap->Pnt[Qtone.ActiveMark].x, YES);
		AnimateMap( lpMap );
		UndoMapPreview( IDCANCEL );
		break;

	    case IDOK:
		if ( !lpImage )
			{
			AstralDlgEnd( hDlg, TRUE );
			break;
			}
		ApplyMap(IDS_UNDOQUARTERTONE);
		ResetAllMaps();
		AnimateMap( NULL );
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		if ( !lpImage )
			{
			AstralDlgEnd( hDlg, FALSE );
			break;
			}
		ResetAllMaps();
		AnimateMap( NULL );
		UndoMapPreview( IDCANCEL );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}

/***********************************************************************/
VOID SetHistoValues( hDlg, lpMap, lpRHisto, lpGHisto, lpBHisto, fNotify )
/***********************************************************************/
HWND	hDlg;
MAP *lpMap;
LPWORD lpRHisto, lpGHisto, lpBHisto;
BOOL fNotify;
{
int i, value;
FIXED percent;
long pixels, countR, countG, countB;

++lpRHisto; ++lpGHisto; ++lpBHisto;
value = 255-lpMap->Pnt[Qtone.ActiveMark].x;
SetControlValue( hDlg, IDC_QUARTERTONES, 0, (long)(LPTR)lpMap, fNotify);
SetControlValue( hDlg, IDC_RHISTO, 0, (long)lpMap->Pnt[Qtone.ActiveMark].x, fNotify);
SetControlValue( hDlg, IDC_GHISTO, 0, (long)lpMap->Pnt[Qtone.ActiveMark].x, fNotify);
SetControlValue( hDlg, IDC_BHISTO, 0, (long)lpMap->Pnt[Qtone.ActiveMark].x, fNotify);
SetDlgItemInt( hDlg, IDC_HISTOVALUE, 100-TOPERCENT(value), NO);
pixels = (long)lpImage->nlin * (long)lpImage->npix;
percent = 100L * FGET(lpRHisto[value], pixels);
SetDlgItemFixed( hDlg, IDC_RCOUNT, percent, NO );
percent = 100L * FGET(lpGHisto[value], pixels);
SetDlgItemFixed( hDlg, IDC_GCOUNT, percent, NO );
percent = 100L * FGET(lpBHisto[value], pixels);
SetDlgItemFixed( hDlg, IDC_BCOUNT, percent, NO );
countR = countG = countB = 0;
for (i = 0; i < value; ++i)
	{
	countR += lpRHisto[i];
	countG += lpGHisto[i];
	countB += lpBHisto[i];
	}
percent = 100L * FGET(countR, pixels);
SetDlgItemFixed( hDlg, IDC_RRIGHT, percent, NO );
percent = 100L * FGET(countG, pixels);
SetDlgItemFixed( hDlg, IDC_GRIGHT, percent, NO );
percent = 100L * FGET(countB, pixels);
SetDlgItemFixed( hDlg, IDC_BRIGHT, percent, NO );
countR = countG = countB = 0;
for (i = value + 1; i <= 255; ++i)
	{
	countR += lpRHisto[i];
	countG += lpGHisto[i];
	countB += lpBHisto[i];
	}
percent = 100L * FGET(countR, pixels);
SetDlgItemFixed( hDlg, IDC_RLEFT, percent, NO );
percent = 100L * FGET(countG, pixels);
SetDlgItemFixed( hDlg, IDC_GLEFT, percent, NO );
percent = 100L * FGET(countB, pixels);
SetDlgItemFixed( hDlg, IDC_BLEFT, percent, NO );
AnimateMap(lpMap);
}


/***********************************************************************/
VOID DoSizeInits( hDlg )
/***********************************************************************/
HWND	hDlg;
{
LPTR	lpUnit;

/* Be careful to set all values before doing a SetDlg... with them */
/* The Edit structure is a temorary holding area for sizing dialog boxes */
Edit.Distortable = NO;
Edit.ScaleX = Edit.ScaleY = 100;
if ( lpImage )
	{
	Edit.Crop.left = Edit.Crop.top = 0;
	Edit.Crop.right = lpImage->npix - 1;
	Edit.Crop.bottom = lpImage->nlin - 1;
	Edit.Resolution = Edit.CropResolution = lpImage->xres;
	Edit.Depth = DEPTH;
	}
else	{
	Edit.Resolution = Edit.CropResolution = 75;
	Edit.Depth = 1;
	}
Edit.Angle = 0;
Edit.Width  = FGET( CROP_WIDTH, Edit.CropResolution );
Edit.Height = FGET( CROP_HEIGHT, Edit.CropResolution );
CheckDlgButton( hDlg, IDC_DISTORT, Edit.Distortable );
CheckDlgButton( hDlg, IDC_SMARTSIZE, Edit.SmartSize );
SetDlgItemFixed( hDlg, IDC_HEIGHT, Edit.Height, NO );
SetDlgItemFixed( hDlg, IDC_WIDTH, Edit.Width, NO );
SetDlgItemInt( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
SetDlgItemInt( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
SetDlgItemInt( hDlg, IDC_RES, Edit.Resolution, NO );
SetDlgItemInt( hDlg, IDC_ANGLE, Edit.Angle, YES );
SetDlgItemInt( hDlg, IDC_MEMORY, AstralImageMemory(), NO );
if ( AstralStr( Control.Units, &lpUnit ) )
	{
	SetDlgItemText( hDlg, IDC_WIDTHLABEL, lpUnit );
	SetDlgItemText( hDlg, IDC_HEIGHTLABEL, lpUnit );
	}
}


/***********************************************************************/
VOID DoNewInits( hDlg )
/***********************************************************************/
HWND	hDlg;
{
LPTR	lpUnit;

/* Be careful to set all values before doing a SetDlg... with them */
/* The Edit structure is a temorary holding area for sizing dialog boxes */
Edit.Distortable = YES;
Edit.ScaleX = Edit.ScaleY = 100;
Edit.Resolution = Edit.CropResolution = 150;
Edit.Crop.left   = 0;
Edit.Crop.right  = 4*Edit.CropResolution - 1;
Edit.Crop.top    = 0;
Edit.Crop.bottom = 5*Edit.CropResolution - 1;
Edit.Depth = 1;
Edit.Width  = FGET( CROP_WIDTH, Edit.CropResolution );
Edit.Height = FGET( CROP_HEIGHT, Edit.CropResolution );
CheckDlgButton( hDlg, IDC_DISTORT, Edit.Distortable );
SetDlgItemFixed( hDlg, IDC_HEIGHT, Edit.Height, NO );
SetDlgItemFixed( hDlg, IDC_WIDTH, Edit.Width, NO );
SetDlgItemInt( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
SetDlgItemInt( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
SetDlgItemInt( hDlg, IDC_RES, Edit.Resolution, NO );
SetDlgItemInt( hDlg, IDC_MEMORY, AstralImageMemory(), NO );
if ( AstralStr( Control.Units, &lpUnit ) )
	{
	SetDlgItemText( hDlg, IDC_WIDTHLABEL, lpUnit );
	SetDlgItemText( hDlg, IDC_HEIGHTLABEL, lpUnit );
	}
}


/***********************************************************************/
int AstralImageMemory()
/***********************************************************************/
{
long pix, lin;
int memory, depth;

depth = Edit.Depth;
if ( depth == 2 )
	depth = 3;
pix = FMUL( Edit.Resolution, Edit.Width );
lin = FMUL( Edit.Resolution, Edit.Height );
memory = ((pix * lin * depth) + 512L) / 1024L;
return( memory );
}


/***********************************************************************/
VOID DoSizeControls( hDlg, wParam, lParam )
/***********************************************************************/
HWND	 hDlg;
WORD	 wParam;
long	 lParam;
{
BOOL	 Bool;
long	 x, y;

switch( wParam )
    {
    case IDC_DISTORT:
	Edit.Distortable = !Edit.Distortable;
	CheckDlgButton( hDlg, IDC_DISTORT, Edit.Distortable );
	if ( Edit.ScaleX != Edit.ScaleY )
		{
		Edit.ScaleX = Edit.ScaleY;
		x = ((long)CROP_WIDTH * Edit.ScaleX);
		Edit.Width  = FGET( x, 100L*Edit.CropResolution );
		SetDlgItemFixed( hDlg, IDC_WIDTH, Edit.Width, NO );
		AstralControlPaint( hDlg, IDC_WIDTH );
		SetDlgItemInt( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
		AstralControlPaint( hDlg, IDC_SCALEX );
		}
	break;

    case IDC_HEIGHT:
	if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		break;
	if ( HIWORD(lParam) != EN_CHANGE )
		break;
	Edit.Height = GetDlgItemFixed( hDlg, IDC_HEIGHT, &Bool, NO );
	y = FMUL( (100L*Edit.CropResolution), Edit.Height );
	Edit.ScaleY = (y + CROP_HEIGHT/2) / CROP_HEIGHT;
	SetDlgItemInt( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
	AstralControlPaint( hDlg, IDC_SCALEY );
	if ( !Edit.Distortable )
		{
		Edit.ScaleX = Edit.ScaleY;
		x = ((long)CROP_WIDTH * Edit.ScaleX);
		Edit.Width  = FGET( x, 100L*Edit.CropResolution );
		SetDlgItemFixed( hDlg, IDC_WIDTH, Edit.Width, NO );
		AstralControlPaint( hDlg, IDC_WIDTH );
		SetDlgItemInt( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
		AstralControlPaint( hDlg, IDC_SCALEX );
		}
	SetDlgItemInt( hDlg, IDC_MEMORY, AstralImageMemory(), NO );
	AstralControlPaint( hDlg, IDC_MEMORY );
	break;

    case IDC_WIDTH:
	if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		break;
	if ( HIWORD(lParam) != EN_CHANGE )
		break;
	Edit.Width = GetDlgItemFixed( hDlg, IDC_WIDTH, &Bool, NO );
	x = FMUL( (100L*Edit.CropResolution), Edit.Width );
	Edit.ScaleX = (x + CROP_WIDTH/2) / CROP_WIDTH;
	SetDlgItemInt( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
	AstralControlPaint( hDlg, IDC_SCALEX );
	if ( !Edit.Distortable )
		{
		Edit.ScaleY = Edit.ScaleX;
		y = ((long)CROP_HEIGHT * Edit.ScaleY);
		Edit.Height  = FGET( y, 100L*Edit.CropResolution );
		SetDlgItemFixed( hDlg, IDC_HEIGHT, Edit.Height, NO );
		AstralControlPaint( hDlg, IDC_HEIGHT );
		SetDlgItemInt( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
		AstralControlPaint( hDlg, IDC_SCALEY );
		}
	SetDlgItemInt( hDlg, IDC_MEMORY, AstralImageMemory(), NO );
	AstralControlPaint( hDlg, IDC_MEMORY );
	break;

    case IDC_RES:
	if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		break;
	if ( HIWORD(lParam) != EN_CHANGE )
		break;
	Edit.Resolution = GetDlgItemInt( hDlg, IDC_RES, &Bool, NO );
	SetDlgItemInt( hDlg, IDC_MEMORY, AstralImageMemory(), NO );
	AstralControlPaint( hDlg, IDC_MEMORY );
	break;

    case IDC_SCALEX:
	if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		break;
	if ( HIWORD(lParam) != EN_CHANGE )
		break;
	Edit.ScaleX = GetDlgItemInt( hDlg, IDC_SCALEX, &Bool, NO );
	x = ((long)CROP_WIDTH * Edit.ScaleX);
	Edit.Width  = FGET( x, 100L*Edit.CropResolution );
	SetDlgItemFixed( hDlg, IDC_WIDTH, Edit.Width, NO );
	AstralControlPaint( hDlg, IDC_WIDTH );
	if ( !Edit.Distortable )
		{
		Edit.ScaleY = Edit.ScaleX;
		y = ((long)CROP_HEIGHT * Edit.ScaleY);
		Edit.Height  = FGET( y, 100L*Edit.CropResolution );
		SetDlgItemFixed( hDlg, IDC_HEIGHT, Edit.Height, NO );
		AstralControlPaint( hDlg, IDC_HEIGHT );
		SetDlgItemInt( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
		AstralControlPaint( hDlg, IDC_SCALEY );
		}
	SetDlgItemInt( hDlg, IDC_MEMORY, AstralImageMemory(), NO );
	AstralControlPaint( hDlg, IDC_MEMORY );
	break;

    case IDC_SCALEY:
	if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		break;
	if ( HIWORD(lParam) != EN_CHANGE )
		break;
	Edit.ScaleY = GetDlgItemInt( hDlg, IDC_SCALEY, &Bool, NO );
	y = ((long)CROP_HEIGHT * Edit.ScaleY);
	Edit.Height = FGET( y, 100L*Edit.CropResolution );
	SetDlgItemFixed( hDlg, IDC_HEIGHT, Edit.Height, NO );
	AstralControlPaint( hDlg, IDC_HEIGHT );
	if ( !Edit.Distortable )
		{
		Edit.ScaleX = Edit.ScaleY;
		x = ((long)CROP_WIDTH * Edit.ScaleX);
		Edit.Width  = FGET( x, 100L*Edit.CropResolution );
		SetDlgItemFixed( hDlg, IDC_WIDTH, Edit.Width, NO );
		AstralControlPaint( hDlg, IDC_WIDTH );
		SetDlgItemInt( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
		AstralControlPaint( hDlg, IDC_SCALEX );
		}
	SetDlgItemInt( hDlg, IDC_MEMORY, AstralImageMemory(), NO );
	AstralControlPaint( hDlg, IDC_MEMORY );
	break;

    case IDC_MEMORY:
    default:
	break;
    }
}


/***********************************************************************/
BOOL FAR PASCAL DlgProcessProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	DoSizeInits( hDlg );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_DISTORT:
	    case IDC_HEIGHT:
	    case IDC_WIDTH:
	    case IDC_RES:
	    case IDC_SCALEX:
	    case IDC_SCALEY:
		DoSizeControls( hDlg, wParam, lParam );
		break;

	    case IDC_SMARTSIZE:
		Edit.SmartSize = !Edit.SmartSize;
		CheckDlgButton( hDlg, IDC_SMARTSIZE, Edit.SmartSize );
		break;

	    case IDOK:
		if ( FMUL( Edit.Resolution, Edit.Width ) > MAX_IMAGE_LINE )
			{
			Message( IDS_ETOOWIDE );
			break;
			}
		AstralCursor( IDC_WAIT );
		process();
		AstralCursor( NULL );
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgRotateProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
static int Angle, IsCCW;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	CheckDlgButton( hDlg, IDC_SMARTSIZE, Edit.SmartSize );
	CheckRadioButton( hDlg, IDC_CWISE, IDC_CCWISE, IDC_CWISE+IsCCW );
	InitDlgItemSpin( hDlg, IDC_ANGLE, Angle, NO, 0, 360 );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_SMARTSIZE:
		Edit.SmartSize = !Edit.SmartSize;
		CheckDlgButton( hDlg, IDC_SMARTSIZE, Edit.SmartSize );
		break;

	    case IDC_ANGLE:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Angle = (int)GetDlgItemSpin( hDlg, IDC_ANGLE, &Bool, YES );
		break;

	    case IDC_CWISE:
	    case IDC_CCWISE:
		CheckRadioButton( hDlg, IDC_CWISE, IDC_CCWISE, wParam );
		IsCCW = wParam - IDC_CWISE;
		break;

	    case IDOK:
		AstralCursor( IDC_WAIT );
		RotateImage( (IsCCW ? -Angle : Angle), Edit.SmartSize );
		AstralCursor( NULL );
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgPrintProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;
{
int ret;
BOOL Bool, bOKColor;
LPTR lpUnit;
STRING szDescription;
static WORD SepFlags;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	DoSizeInits( hDlg );
	SetDefaultPrinter( Names.Printer );
	DeviceDescription( Names.Printer, szDescription );
	SetDlgItemText( hDlg, IDC_PRINTER, szDescription );
	SetDlgItemText( hDlg, IDC_PRINTSTYLE, Names.PrintStyle );
	CheckDlgButton( hDlg, IDC_CENTERED, Page.Centered );
	SetDlgItemFixed( hDlg, IDC_OFFSETX, Page.OffsetX, NO );
	SetDlgItemFixed( hDlg, IDC_OFFSETY, Page.OffsetY, NO );
	ControlEnable( hDlg, IDC_OFFSETX, !Page.Centered );
	ControlEnable( hDlg, IDC_OFFSETY, !Page.Centered );
	CheckDlgButton( hDlg, IDC_SCATTER, Page.ScatterPrint );
	SepFlags = ( DEPTH == 1 ? K_SEP : Page.SepFlags );
	CheckDlgButton( hDlg, IDC_DO_CYAN, (SepFlags & C_SEP) );
	CheckDlgButton( hDlg, IDC_DO_MAGENTA, (SepFlags & M_SEP) );
	CheckDlgButton( hDlg, IDC_DO_YELLOW, (SepFlags & Y_SEP) );
	CheckDlgButton( hDlg, IDC_DO_BLACK, (SepFlags & K_SEP) );
	bOKColor = (DEPTH != 1 && Printer.Type != IDC_PRINTER_IS_RGB);
	ControlEnable( hDlg, IDC_DO_CYAN, bOKColor );
	ControlEnable( hDlg, IDC_DO_MAGENTA, bOKColor );
	ControlEnable( hDlg, IDC_DO_YELLOW, bOKColor );
	bOKColor = (DEPTH != 1 && Printer.Type != IDC_PRINTER_IS_RGB );
	ControlEnable( hDlg, IDC_DO_BLACK, bOKColor );

	ControlEnable( hDlg, IDC_PRINT_GRAY, YES );
	bOKColor = (DEPTH != 1 && Printer.Type != IDC_PRINTER_IS_MONO);
	ControlEnable( hDlg, IDC_PRINT_COLOR, bOKColor );
	bOKColor = (DEPTH != 1 && Printer.Type == IDC_PRINTER_IS_CMYK );
	ControlEnable( hDlg, IDC_PRINT_COLORGRAY, bOKColor );
	ControlEnable( hDlg, IDC_PRINT_COLORSEPS, bOKColor );
	bOKColor = (DEPTH != 1 && Printer.Type != IDC_PRINTER_IS_RGB );
	ControlEnable( hDlg, IDC_PRINT_BLACKSEPS, bOKColor );
	Page.Type = ( DEPTH == 1 || Printer.Type == IDC_PRINTER_IS_MONO ?
		IDC_PRINT_GRAY : IDC_PRINT_COLOR );
	CheckComboItem( hDlg, IDC_PRINT_TYPE, IDC_PRINT_GRAY,
		IDC_PRINT_COLORSEPS, Page.Type );
	bOKColor = ( Page.Type == IDC_PRINT_BLACKSEPS
		  || Page.Type == IDC_PRINT_COLORSEPS );
	ShowControl( hDlg, IDC_DO_CYAN, IDC_DO_BLACK, bOKColor, NO );
	ShowControl( hDlg, IDC_DO_CPATCH, IDC_DO_KPATCH, bOKColor, NO );
	if ( AstralStr( Control.Units, &lpUnit ) )
		{
		SetDlgItemText( hDlg, IDC_LEFTLABEL, lpUnit );
		SetDlgItemText( hDlg,IDC_TOPLABEL, lpUnit ); 
		}
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_DISTORT:
	    case IDC_HEIGHT:
	    case IDC_WIDTH:
	    case IDC_SCALEX:
	    case IDC_SCALEY:
		DoSizeControls( hDlg, wParam, lParam );
		break;

	    case IDC_CENTERED:
		Page.Centered = !Page.Centered;
		CheckDlgButton( hDlg, IDC_CENTERED, Page.Centered );
		ControlEnable( hDlg, IDC_OFFSETX, !Page.Centered );
		ControlEnable( hDlg, IDC_OFFSETY, !Page.Centered );
		break;

	    case IDC_OFFSETX:
		Page.OffsetX = GetDlgItemFixed( hDlg, IDC_OFFSETX,
			&Bool, NO );
		break;

	    case IDC_OFFSETY:
		Page.OffsetY = GetDlgItemFixed( hDlg, IDC_OFFSETY,
			&Bool, NO );
		break;

	    case IDC_PRINT_TYPE:
//	    case IDC_PRINT_GRAY:
//	    case IDC_PRINT_COLOR:
//	    case IDC_PRINT_COLORGRAY:
//	    case IDC_PRINT_BLACKSEPS:
//	    case IDC_PRINT_COLORSEPS:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Page.Type = wParam;
		bOKColor = ( Page.Type == IDC_PRINT_BLACKSEPS
			  || Page.Type == IDC_PRINT_COLORSEPS );
		ShowControl( hDlg, IDC_DO_CYAN, IDC_DO_BLACK, bOKColor, NO );
		ShowControl( hDlg, IDC_DO_CPATCH, IDC_DO_KPATCH, bOKColor, NO );
		break;

	    case IDC_DO_CYAN:
		SepFlags ^= C_SEP;
		CheckDlgButton( hDlg, wParam, SepFlags & C_SEP );
		break;

	    case IDC_DO_MAGENTA:
		SepFlags ^= M_SEP;
		CheckDlgButton( hDlg, wParam, SepFlags & M_SEP );
		break;

	    case IDC_DO_YELLOW:
		SepFlags ^= Y_SEP;
		CheckDlgButton( hDlg, wParam, SepFlags & Y_SEP );
		break;

	    case IDC_DO_BLACK:
		SepFlags ^= K_SEP;
		CheckDlgButton( hDlg, wParam, SepFlags & K_SEP );
		break;

	    case IDC_SCATTER:
		Page.ScatterPrint = !Page.ScatterPrint;
		CheckDlgButton( hDlg, wParam, Page.ScatterPrint );
		break;

	    case IDOK:
		if ( !lpImage )
			break;
		/* Print the current image file */
		AstralCursor( IDC_WAIT );
		if ( Page.Type == IDC_PRINT_BLACKSEPS
		  || Page.Type == IDC_PRINT_COLORSEPS )
			Page.SepFlags = SepFlags;
		ret = PrintFile( hDlg, stripdir(lpImage->CurFile) );
		AstralCursor( NULL );
		if ( ret )
			AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDM_PRINTSETUP:
		/* Bring up the setup dialog box for the active printer */
		AstralDlg( NO, hInstAstral, hDlg, IDD_PRINTSETUP,
			DlgPrintSetupProc);
		DeviceDescription( Names.Printer, szDescription );
		SetDlgItemText( hDlg, IDC_PRINTER, szDescription );
		SetDlgItemText( hDlg, IDC_PRINTSTYLE, Names.PrintStyle );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgPrintSetupProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;
{
STRING szFileName;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	Names.Printer[0] = '\0';
	InitPrinterListBox( hDlg, Names.Printer );
	lstrcpy( Names.Saved, Names.PrintStyle );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.PrintStyle,
		IDN_STYLE );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_PRINTSELECT:
		if ( HIWORD(lParam) == LBN_SELCHANGE )
			GetPrinterListBox( hDlg, Names.Printer );
		if ( HIWORD(lParam) == LBN_DBLCLK )
			SetupPrinter( hDlg, Names.Printer );
		break;

	    case IDC_EXTNAMES:
		if ( HIWORD(lParam) == LBN_DBLCLK )
			{
			AstralDlg( NO, hInstAstral, hDlg, IDD_PRINTSTYLE,
				DlgPrintStyleProc );
			InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
				Names.PrintStyle, IDN_STYLE );
			break;
			}
		if ( HIWORD(lParam) != LBN_SELCHANGE )
			break;
		GetExtName( hDlg, wParam, IDC_FILENAME, Names.PrintStyle,
			IDN_STYLE, HIWORD(lParam) );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.PrintStyle, IDN_STYLE, wParam, NO );
		break;

	    case IDOK:
		SetDefaultPrinter( Names.Printer );
		if ( !LookupExtFile( Names.PrintStyle, szFileName, IDN_STYLE ) )
			; //break;
		if ( !LoadPrintStyle( &Printer, szFileName ) )
			; //break;
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		lstrcpy( Names.PrintStyle, Names.Saved );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgPrintStyleProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;
{
BOOL Bool;
int i, x, y;
FNAME szFileName;
STRING szString;
static WORD wMap;
static WORD wStyle;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	szFileName[0] = '\0';
	SetWindowText( hDlg, Names.PrintStyle );
	if ( Names.PrintStyle[0] == '-' ) // If it's untitled, zap it
		{
		Names.PrintStyle[0] = '\0';
// The Print struscture should get init'd here
		}
	else	{ // otherwise load it
		if ( !LookupExtFile( Names.PrintStyle, szFileName, IDN_STYLE ) )
			; //break;
		if ( !LoadPrintStyle( &Printer, szFileName ) )
			; //break;
		}
	CheckComboItem( hDlg, IDC_PRINTERTYPES, IDC_PRINTER_IS_MONO,
		IDC_PRINTER_IS_RGB, Printer.Type );
	CheckComboItem( hDlg, IDC_STYLEOPTIONS, IDC_SETUP_HALFTONING,
		IDC_SETUP_BLACKANDUCR, IDC_SETUP_HALFTONING );
	CheckDlgButton( hDlg, IDC_TRIMMARKS, Printer.TrimMarks );
	CheckDlgButton( hDlg, IDC_REGMARKS, Printer.RegMarks );
	CheckDlgButton( hDlg, IDC_LABELS, Printer.Labels );
	CheckDlgButton( hDlg, IDC_STEPSCALE, Printer.StepScale );
	CheckDlgButton( hDlg, IDC_NEGATIVE, Printer.Negative );
	CheckDlgButton( hDlg, IDC_EMULSIONDOWN, Printer.EmulsionDown );
	CheckDlgButton( hDlg, IDC_DO_HALFTONING, Printer.DoHalftoning );
	CheckDlgButton( hDlg, IDC_DO_CALIBRATION, Printer.DoCalibration );
	CheckDlgButton( hDlg, IDC_DO_INKCORRECT, Printer.DoInkCorrect );
	CheckDlgButton( hDlg, IDC_DO_BLACKANDUCR, Printer.DoBlackAndUCR );

	// Halftoning controls
	CheckRadioButton( hDlg, IDC_CIRCLEDOT, IDC_ELLIPSEDOT,
		Printer.DotShape );
	InitDlgItemSpin( hDlg, IDC_HILIGHT, TOPERCENT(Printer.Hilight), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_SHADOW, TOPERCENT(Printer.Shadow), NO,
		0, 100 );
	SetDlgItemFixed( hDlg, IDC_SCRULING0, Printer.ScreenRuling[0], NO );
	SetDlgItemFixed( hDlg, IDC_SCRULING1, Printer.ScreenRuling[1], NO );
	SetDlgItemFixed( hDlg, IDC_SCRULING2, Printer.ScreenRuling[2], NO );
	SetDlgItemFixed( hDlg, IDC_SCRULING3, Printer.ScreenRuling[3], NO );
	SetDlgItemFixed( hDlg, IDC_ANGLE0, Printer.ScreenAngle[0], NO );
	SetDlgItemFixed( hDlg, IDC_ANGLE1, Printer.ScreenAngle[1], NO );
	SetDlgItemFixed( hDlg, IDC_ANGLE2, Printer.ScreenAngle[2], NO );
	SetDlgItemFixed( hDlg, IDC_ANGLE3, Printer.ScreenAngle[3], NO );

	// Ink correction controls
	SetWindowLong( GetDlgItem( hDlg, IDC_CORRECT_RED ), 0, RGB(255,0,0) );
	SetWindowLong( GetDlgItem( hDlg, IDC_CORRECT_GREEN ), 0, RGB(0,255,0) );
	SetWindowLong( GetDlgItem( hDlg, IDC_CORRECT_BLUE ), 0, RGB(0,0,255) );
	SetDlgItemInt( hDlg, IDC_CORRECT_RED, Printer.MfromR, YES );
	SetDlgItemInt( hDlg, IDC_CORRECT_GREEN, Printer.YfromG, YES );
	SetDlgItemInt( hDlg, IDC_CORRECT_BLUE, Printer.CfromB, YES );
	InitDlgItemSpin( hDlg, IDC_M_IN_R, bound(100+Printer.MfromR,0,100), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_Y_IN_R, bound(100-Printer.MfromR,0,100), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_Y_IN_G, bound(100+Printer.YfromG,0,100), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_C_IN_G, bound(100-Printer.YfromG,0,100), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_C_IN_B, bound(100+Printer.CfromB,0,100), NO,
		0, 100 );
	InitDlgItemSpin( hDlg, IDC_M_IN_B, bound(100-Printer.CfromB,0,100), NO,
		0, 100 );

	// Calibration controls
	lstrcpy( szFileName, Printer.CalName );
	InitExtName( hDlg, IDC_CALEXTNAMES, IDC_FILENAME, szFileName,
		IDN_CALMAP );
	SetDlgItemText( hDlg, IDC_CALEXTNAMES, Printer.CalName );
	SetDlgItemInt( hDlg, IDC_CALMAP, Printer.CalMap[wMap].Points, NO );
	SetWindowLong( GetDlgItem( hDlg, IDC_CALMAP ), 0,
		(long)(WORD)Printer.CalMap[wMap].Pnt );
	SetDlgItemText( hDlg, IDC_CALIN,  "" );
	SetDlgItemText( hDlg, IDC_CALOUT, "" );
	CheckComboItem( hDlg, IDC_CALCHANNELS, IDC_CAL_BLACK,
		IDC_CAL_YELLOW, wMap + IDC_CAL_BLACK );
//	Printer.CalMap[wMap].bStretch = YES;

	// Black generation and UCR
	lstrcpy( szFileName, Printer.BGName );
	InitExtName( hDlg, IDC_BGEXTNAMES, IDC_FILENAME, szFileName,
		IDN_BGMAP );
	SetDlgItemText( hDlg, IDC_BGEXTNAMES, Printer.BGName );
	SetDlgItemInt( hDlg, IDC_BGMAP, Printer.BGMap.Points, NO );
	SetWindowLong( GetDlgItem( hDlg, IDC_BGMAP ), 0,
		(long)(WORD)Printer.BGMap.Pnt );
	SetDlgItemText( hDlg, IDC_BGIN,  "" );
	SetDlgItemText( hDlg, IDC_BGOUT, "" );
	InitDlgItemSpin( hDlg, IDC_UCR, Printer.UCR, NO, 0, 100 );
	InitDlgItemSpin( hDlg, IDC_BOOST, 100-Printer.UCR, NO, 0, 100 );

	ActivateStyleControls( hDlg, IDC_SETUP_HALFTONING, &wStyle, NO );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_PRINTERTYPES:
//	    case IDC_PRINTER_IS_MONO:
//	    case IDC_PRINTER_IS_CMYK:
//	    case IDC_PRINTER_IS_RGB:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Printer.Type = wParam;
		break;

	    case IDC_STYLEOPTIONS:
//	    case IDC_SETUP_HALFTONING:
//	    case IDC_SETUP_CALIBRATION:
//	    case IDC_SETUP_INKCORRECT:
//	    case IDC_SETUP_BLACKANDUCR:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		ActivateStyleControls( hDlg, wParam, &wStyle, YES );
		break;

	    case IDC_DO_HALFTONING:
		Printer.DoHalftoning = !Printer.DoHalftoning;
		CheckDlgButton( hDlg, wParam, Printer.DoHalftoning );
		ControlEnable( hDlg, IDC_SETUP_HALFTONING,Printer.DoHalftoning);
		break;

	    case IDC_DO_CALIBRATION:
		Printer.DoCalibration = !Printer.DoCalibration;
		CheckDlgButton( hDlg, wParam, Printer.DoCalibration );
		ControlEnable(hDlg,IDC_SETUP_CALIBRATION,Printer.DoCalibration);
		break;

	    case IDC_DO_INKCORRECT:
		Printer.DoInkCorrect = !Printer.DoInkCorrect;
		CheckDlgButton( hDlg, wParam, Printer.DoInkCorrect );
		ControlEnable( hDlg, IDC_SETUP_INKCORRECT,Printer.DoInkCorrect);
		break;

	    case IDC_DO_BLACKANDUCR:
		Printer.DoBlackAndUCR = !Printer.DoBlackAndUCR;
		CheckDlgButton( hDlg, wParam, Printer.DoBlackAndUCR );
		ControlEnable(hDlg,IDC_SETUP_BLACKANDUCR,Printer.DoBlackAndUCR);
		break;

	    case IDC_TRIMMARKS:
		Printer.TrimMarks = !Printer.TrimMarks;
		CheckDlgButton( hDlg, wParam, Printer.TrimMarks );
		break;

	    case IDC_REGMARKS:
		Printer.RegMarks = !Printer.RegMarks;
		CheckDlgButton( hDlg, wParam, Printer.RegMarks );
		break;

	    case IDC_LABELS:
		Printer.Labels = !Printer.Labels;
		CheckDlgButton( hDlg, wParam, Printer.Labels );
		break;

	    case IDC_STEPSCALE:
		Printer.StepScale = !Printer.StepScale;
		CheckDlgButton( hDlg, wParam, Printer.StepScale );
		break;

	    case IDC_EMULSIONDOWN:
		Printer.EmulsionDown = !Printer.EmulsionDown;
		CheckDlgButton( hDlg, wParam, Printer.EmulsionDown );
		break;

	    case IDC_NEGATIVE:
		Printer.Negative = !Printer.Negative;
		CheckDlgButton( hDlg, wParam, Printer.Negative );
		break;

	    case IDC_CIRCLEDOT:
	    case IDC_SQUAREDOT:
	    case IDC_ELLIPSEDOT:
		CheckRadioButton( hDlg, IDC_CIRCLEDOT, IDC_ELLIPSEDOT,
			wParam );
		Printer.DotShape = wParam;
		break;

	    case IDC_SCRULING0:
	    case IDC_SCRULING1:
	    case IDC_SCRULING2:
	    case IDC_SCRULING3:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Printer.ScreenRuling[ wParam - IDC_SCRULING0 ] = 
			GetDlgItemFixed( hDlg, wParam, &Bool, NO );
		break;

	    case IDC_HILIGHT:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Printer.Hilight = TOGRAY(i);
		break;

	    case IDC_SHADOW:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Printer.Shadow = TOGRAY(i);
		break;

	    case IDC_ANGLE0:
	    case IDC_ANGLE1:
	    case IDC_ANGLE2:
	    case IDC_ANGLE3:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Printer.ScreenAngle[ wParam - IDC_ANGLE0 ] = 
			GetDlgItemFixed( hDlg, wParam, &Bool, NO );
		break;

	    case IDC_CORRECT_RED:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		Printer.MfromR = (int)LOWORD(lParam);
		SetDlgItemSpin( hDlg, IDC_M_IN_R,
			bound(100+Printer.MfromR,0,100), NO );
		SetDlgItemSpin( hDlg, IDC_Y_IN_R,
			bound(100-Printer.MfromR,0,100), NO );
		break;

	    case IDC_CORRECT_GREEN:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		Printer.YfromG = (int)LOWORD(lParam);
		SetDlgItemSpin( hDlg, IDC_Y_IN_G,
			bound(100+Printer.YfromG,0,100), NO );
		SetDlgItemSpin( hDlg, IDC_C_IN_G,
			bound(100-Printer.YfromG,0,100), NO );
		break;

	    case IDC_CORRECT_BLUE:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		Printer.CfromB = (int)LOWORD(lParam);
		SetDlgItemSpin( hDlg, IDC_C_IN_B,
			bound(100+Printer.CfromB,0,100), NO );
		SetDlgItemSpin( hDlg, IDC_M_IN_B,
			bound(100-Printer.CfromB,0,100), NO );
		break;

	    case IDC_M_IN_R:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Printer.MfromR = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Printer.MfromR = bound( Printer.MfromR-100, -100, 100 );
		SetDlgItemSpin( hDlg, IDC_Y_IN_R, 100-Printer.MfromR, NO );
		SetDlgItemInt( hDlg, IDC_CORRECT_RED, Printer.MfromR, YES );
		AstralControlRepaint( hDlg, IDC_CORRECT_RED );
		break;

	    case IDC_Y_IN_R:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Printer.MfromR = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Printer.MfromR = bound( 100-Printer.MfromR, -100, 100 );
		SetDlgItemSpin( hDlg, IDC_M_IN_R, 100+Printer.MfromR, NO );
		SetDlgItemInt( hDlg, IDC_CORRECT_RED, Printer.MfromR, YES );
		AstralControlRepaint( hDlg, IDC_CORRECT_RED );
		break;

	    case IDC_Y_IN_G:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Printer.YfromG = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Printer.YfromG = bound( Printer.YfromG-100, -100, 100 );
		SetDlgItemSpin( hDlg, IDC_C_IN_G, 100-Printer.YfromG, NO );
		SetDlgItemInt( hDlg, IDC_CORRECT_GREEN, Printer.YfromG, YES );
		AstralControlRepaint( hDlg, IDC_CORRECT_GREEN );
		break;

	    case IDC_C_IN_G:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Printer.YfromG = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Printer.YfromG = bound( 100-Printer.YfromG, -100, 100 );
		SetDlgItemSpin( hDlg, IDC_Y_IN_G, 100+Printer.YfromG, NO );
		SetDlgItemInt( hDlg, IDC_CORRECT_GREEN, Printer.YfromG, YES );
		AstralControlRepaint( hDlg, IDC_CORRECT_GREEN );
		break;

	    case IDC_C_IN_B:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Printer.CfromB = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Printer.CfromB = bound( 100-Printer.CfromB, -100, 100 );
		SetDlgItemSpin( hDlg, IDC_M_IN_B, 100-Printer.CfromB, NO );
		SetDlgItemInt( hDlg, IDC_CORRECT_BLUE, Printer.CfromB, YES );
		AstralControlRepaint( hDlg, IDC_CORRECT_BLUE );
		break;

	    case IDC_M_IN_B:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Printer.CfromB = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Printer.CfromB = bound( Printer.CfromB-100, -100, 100 );
		SetDlgItemSpin( hDlg, IDC_C_IN_B, 100+Printer.CfromB, NO );
		SetDlgItemInt( hDlg, IDC_CORRECT_BLUE, Printer.CfromB, YES );
		AstralControlRepaint( hDlg, IDC_CORRECT_BLUE );
		break;

	    case IDC_CALEXTNAMES:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		GetExtName( hDlg, wParam, IDC_FILENAME, Printer.CalName,
			IDN_CALMAP, HIWORD(lParam) );
		if ( !LookupExtFile( Printer.CalName, szFileName, IDN_CALMAP ) )
			break;
		LoadMap( &Printer.CalMap[0], &Printer.CalMap[1],
			 &Printer.CalMap[2], &Printer.CalMap[3], szFileName );
		SetDlgItemInt( hDlg, IDC_CALMAP,Printer.CalMap[wMap].Points,NO);
		SetWindowLong( GetDlgItem( hDlg, IDC_CALMAP ), 0,
			(long)(WORD)Printer.CalMap[wMap].Pnt );
		AstralControlPaint( hDlg, IDC_CALMAP );
		SetDlgItemText( hDlg, IDC_CALIN,  "" );
		SetDlgItemText( hDlg, IDC_CALOUT, "" );
		break;

	    case IDC_BGEXTNAMES:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		GetExtName( hDlg, wParam, IDC_FILENAME, Printer.BGName,
			IDN_BGMAP, HIWORD(lParam) );
		if ( !LookupExtFile( Printer.BGName, szFileName, IDN_BGMAP ) )
			break;
		LoadMap( &Printer.BGMap, NULL, NULL, NULL, szFileName );
		SetDlgItemInt( hDlg, IDC_BGMAP, Printer.BGMap.Points, NO );
		SetWindowLong( GetDlgItem( hDlg, IDC_BGMAP ), 0,
			(long)(WORD)Printer.BGMap.Pnt );
		AstralControlPaint( hDlg, IDC_BGMAP );
		SetDlgItemText( hDlg, IDC_BGIN,  "" );
		SetDlgItemText( hDlg, IDC_BGOUT, "" );
		break;

	    case IDC_CALEXTMANAGE:
	    case IDC_BGEXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		if ( wStyle == IDC_SETUP_CALIBRATION )
			ExtNameManager( hDlg, IDC_CALEXTNAMES, IDC_FILENAME,
				Printer.CalName, IDN_CALMAP, wParam, NO );
		else	ExtNameManager( hDlg, IDC_BGEXTNAMES, IDC_FILENAME,
				Printer.BGName, IDN_BGMAP, wParam, NO );
		break;

	    case IDC_CALCHANNELS:
//	    case IDC_CAL_BLACK:
//	    case IDC_CAL_CYAN:
//	    case IDC_CAL_MAGENTA:
//	    case IDC_CAL_YELLOW:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		wMap = wParam - IDC_CAL_BLACK;
		SetDlgItemInt( hDlg, IDC_CALMAP,Printer.CalMap[wMap].Points,NO);
		SetWindowLong( GetDlgItem( hDlg, IDC_CALMAP ), 0,
			(long)(WORD)Printer.CalMap[wMap].Pnt );
		AstralControlPaint( hDlg, IDC_CALMAP );
		SetDlgItemText( hDlg, IDC_CALIN,  "" );
		SetDlgItemText( hDlg, IDC_CALOUT, "" );
		break;

	    case IDC_CALMAP:
		// Just use the notification of the x coordinate
		x = TOPERCENT( HIWORD(lParam) );
		y = TOPERCENT( 255-Printer.CalMap[wMap].Lut[ 255-HIWORD(lParam) ] );
		wsprintf( szString, "%d %%", x );
		SetDlgItemText( hDlg, IDC_CALIN, szString );
		wsprintf( szString, "%d %%", y );
		SetDlgItemText( hDlg, IDC_CALOUT, szString );
		break;

	    case IDC_BGMAP:
		// Just use the notification of the x coordinate
		x = TOPERCENT( HIWORD(lParam) );
		y = TOPERCENT( 255-Printer.BGMap.Lut[ 255-HIWORD(lParam) ] );
		wsprintf( szString, "%d %%", x );
		SetDlgItemText( hDlg, IDC_BGIN, szString );
		wsprintf( szString, "%d %%", y );
		SetDlgItemText( hDlg, IDC_BGOUT, szString );
		break;

	    case IDC_MAPEDITBG:
		MasterMap = Printer.BGMap;
		lstrcpy( Names.Saved, Printer.BGName );
		wMapType = IDN_BGMAP;
		if ( !AstralDlg( NO, hInstAstral, hDlg, IDD_MAPEDITBG,
		     DlgMapEditProc ) )
			break;
		// MasterMap and Names.Saved are returned
		Printer.BGMap = MasterMap;
		lstrcpy( Printer.BGName, Names.Saved );
		InitExtName( hDlg, IDC_BGEXTNAMES, IDC_FILENAME,
			Printer.BGName, IDN_BGMAP );
		SetDlgItemText( hDlg, IDC_BGEXTNAMES, Printer.BGName );
		SetDlgItemInt( hDlg, IDC_BGMAP, Printer.BGMap.Points, NO );
		SetWindowLong( GetDlgItem( hDlg, IDC_BGMAP ), 0,
			(long)(WORD)Printer.BGMap.Pnt );
		AstralControlPaint( hDlg, IDC_BGMAP );
		SetDlgItemText( hDlg, IDC_BGIN,  "" );
		SetDlgItemText( hDlg, IDC_BGOUT, "" );
		break;

	    case IDC_MAPEDITPRINTCAL:
		MasterMap = Printer.CalMap[0];
		RedMap    = Printer.CalMap[1];
		GreenMap  = Printer.CalMap[2];
		BlueMap   = Printer.CalMap[3];
		lstrcpy( Names.Saved, Printer.CalName );
		wMapType = IDN_CALMAP;
		if ( !AstralDlg( NO, hInstAstral, hDlg, IDD_MAPEDITPRINTCAL,
		     DlgMapEditProc ) )
			break;
		// The color maps and Names.Saved are returned
		Printer.CalMap[0] = MasterMap;
		Printer.CalMap[1] = RedMap;
		Printer.CalMap[2] = GreenMap;
		Printer.CalMap[3] = BlueMap;
		lstrcpy( Printer.CalName, Names.Saved );
		InitExtName( hDlg, IDC_CALEXTNAMES, IDC_FILENAME,
			Printer.CalName, IDN_CALMAP );
		SetDlgItemText( hDlg, IDC_CALEXTNAMES, Printer.CalName );
		SetDlgItemInt( hDlg, IDC_CALMAP,Printer.CalMap[wMap].Points,NO);
		SetWindowLong( GetDlgItem( hDlg, IDC_CALMAP ), 0,
			(long)(WORD)Printer.CalMap[wMap].Pnt );
		AstralControlPaint( hDlg, IDC_CALMAP );
		SetDlgItemText( hDlg, IDC_CALIN,  "" );
		SetDlgItemText( hDlg, IDC_CALOUT, "" );
		break;

	    case IDC_UCR:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Printer.UCR = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		SetDlgItemSpin( hDlg, IDC_BOOST, 100-Printer.UCR, NO );
		break;

	    case IDC_BOOST:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Printer.UCR = 100 - GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		SetDlgItemSpin( hDlg, IDC_BOOST, 100-Printer.UCR, NO );
		break;

	    case IDOK:
		AstralDlgEnd( hDlg, TRUE );
		if ( !PromptForExtName( hDlg, Names.PrintStyle, szFileName,
		     IDN_STYLE ) )
			break;
		if ( !SavePrintStyle( &Printer, szFileName ) )
			break;
		if ( !AddExtFile( Names.PrintStyle, szFileName, IDN_STYLE ) )
			{
			unlink(szFileName);
			break;
			}
		break;

	    case IDCANCEL:
// Restore the original Print structure
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


#ifdef UNUSED
	InitExtName( hDlg, IDC_CALEXTNAMES, IDC_FILENAME, Printer.CalName,
		IDN_CALMAP );
	SetDlgItemText( hDlg, IDC_CALEXTNAMES, Printer.CalName );
	if ( LookupExtFile( Printer.CalName, szFileName, IDN_CALMAP ) )
		LoadMap( &Printer.CalMap[0], &Printer.CalMap[1],
			 &Printer.CalMap[2], &Printer.CalMap[3], szFileName );
	else	{
		ResetMap( &Printer.CalMap[0], MAPPOINTS );
		ResetMap( &Printer.CalMap[1], MAPPOINTS );
		ResetMap( &Printer.CalMap[2], MAPPOINTS );
		ResetMap( &Printer.CalMap[3], MAPPOINTS );
		}
	SetDlgItemInt( hDlg, IDC_CALMAP, Printer.CalMap[wMap].Points, NO );
	SetWindowLong( GetDlgItem( hDlg, IDC_CALMAP ), 0,
		(long)(WORD)Printer.CalMap[wMap].Pnt );
	SetDlgItemText( hDlg, IDC_CALIN,  "" );
	SetDlgItemText( hDlg, IDC_CALOUT, "" );
	CheckComboItem( hDlg, IDC_CALCHANNELS, IDC_CAL_BLACK,
		IDC_CAL_YELLOW, wMap + IDC_CAL_BLACK );
	SetDlgItemInt( hDlg, IDC_CAL0MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[0].x), NO );
	SetDlgItemInt( hDlg, IDC_CAL1MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[1].x), NO );
	SetDlgItemInt( hDlg, IDC_CAL2MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[2].x), NO );
	SetDlgItemInt( hDlg, IDC_CAL3MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[3].x), NO );
	SetDlgItemInt( hDlg, IDC_CAL4MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[4].x), NO );
	SetDlgItemInt( hDlg, IDC_CAL5MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[5].x), NO );
	SetDlgItemInt( hDlg, IDC_CAL6MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[6].x), NO );
	SetDlgItemInt( hDlg, IDC_CAL7MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[7].x), NO );
	SetDlgItemInt( hDlg, IDC_CAL8MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[8].x), NO );
	SetDlgItemInt( hDlg, IDC_CAL9MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[9].x), NO );
	SetDlgItemInt( hDlg, IDC_CAL10MEASURE,
		TOPERCENT(Printer.CalMap[wMap].Pnt[10].x), NO );
	Printer.CalMap[wMap].bStretch = YES;
	/* Make sure the points are in ascending order */
	for ( i=0; i<Printer.CalMap[wMap].Points-1; i++ )
		{
		if ( Printer.CalMap[wMap].Pnt[i].x
		   > Printer.CalMap[wMap].Pnt[i+1].x)
			break;
		}
	if ( i != Printer.CalMap[wMap].Points-1 )
		{
		Message( IDS_EMAPASC );
//		break;
		}
	MakeMap( &Printer.CalMap[wMap] );

	    case IDC_CAL0MEASURE:
	    case IDC_CAL1MEASURE:
	    case IDC_CAL2MEASURE:
	    case IDC_CAL3MEASURE:
	    case IDC_CAL4MEASURE:
	    case IDC_CAL5MEASURE:
	    case IDC_CAL6MEASURE:
	    case IDC_CAL7MEASURE:
	    case IDC_CAL8MEASURE:
	    case IDC_CAL9MEASURE:
	    case IDC_CAL10MEASURE:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		Printer.CalMap[wMap].Pnt[wParam-IDC_CAL0MEASURE].x = TOGRAY(i);
		break;

#endif

/***********************************************************************/
void ActivateStyleControls( hDlg, wStyle, lpOldStyle, bDelay )
/***********************************************************************/
HWND hDlg;
WORD wStyle;
LPWORD lpOldStyle;
BOOL bDelay;
{
int i;
BOOL bShow, bFirstTimeHide;

if ( bDelay && wStyle == *lpOldStyle )
	return;
if ( !wStyle )
	wStyle = *lpOldStyle;

AstralCursor( IDC_WAIT );
bShow = NO;
Top:
bFirstTimeHide = (!bShow && !bDelay);
if ( *lpOldStyle == IDC_SETUP_HALFTONING || bFirstTimeHide )
	ShowControl( hDlg, IDC_FIRST_HALFTONING, IDC_LAST_HALFTONING,
		bShow, bDelay );
if ( *lpOldStyle == IDC_SETUP_CALIBRATION || bFirstTimeHide )
	ShowControl( hDlg, IDC_FIRST_CALIBRATION, IDC_LAST_CALIBRATION,
		bShow, bDelay );
if ( *lpOldStyle == IDC_SETUP_INKCORRECT || bFirstTimeHide )
	ShowControl( hDlg, IDC_FIRST_INKCORRECT, IDC_LAST_INKCORRECT,
		bShow, bDelay );
if ( *lpOldStyle == IDC_SETUP_BLACKANDUCR || bFirstTimeHide )
	ShowControl( hDlg, IDC_FIRST_BLACKANDUCR, IDC_LAST_BLACKANDUCR,
		bShow, bDelay );
if ( !bShow )
	{
	bShow = YES;
	*lpOldStyle = wStyle;
	goto Top;
	}
ShowControl( NULL, NULL, NULL, NULL, NULL );
AstralCursor( NULL );
}


/***********************************************************************/
BOOL FAR PASCAL DlgMapEditProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND hDlg;
unsigned msg;
WORD wParam;
long lParam;
{
BOOL Bool;
int i, x, y;
STRING szString;
FNAME szFileName;
static WORD wChannel;
static MAP *lpMap; //since this is a static pointer to a static structure,
// don't make it a far pointer because the data segment might move

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	CheckComboItem( hDlg, IDC_CHANNELS, IDC_DO_ALL, IDC_DO_YELLOW,
		wChannel = IDC_DO_ALL );
	lpMap = ResetMapChannel( hDlg, wChannel );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_CHANNELS:
//	    case IDC_DO_ALL:
//	    case IDC_DO_RED:
//	    case IDC_DO_GREEN:
//	    case IDC_DO_BLUE:
//	    case IDC_DO_CYAN:
//	    case IDC_DO_MAGENTA:
//	    case IDC_DO_YELLOW:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		wChannel = wParam;
		lpMap = ResetMapChannel( hDlg, wChannel );
		break;

	    case IDC_RESETALL:
		ResetAllMaps();
		lpMap = ResetMapChannel( hDlg, wChannel );
		MakeMap( lpMap );
		break;

	    case IDC_RESET:
		ResetMap( lpMap, MAPPOINTS );
		lpMap = ResetMapChannel( hDlg, wChannel );
		MakeMap( lpMap );
		break;

	    case IDOK:
		for ( i=0; i<10; i++ )
			if ( lpMap->Pnt[i].x > lpMap->Pnt[i+1].x )
				break;
		if ( i != 10 )
			{
			Message( IDS_EMAPASC );
			break;
			}

		if ( !PromptForExtName( hDlg, Names.Saved, szFileName,wMapType))
			break;
		if ( !SaveMap( &MasterMap, &RedMap, &GreenMap, &BlueMap,
		     szFileName ) )
			break;
		if ( !AddExtFile( Names.Saved, szFileName, wMapType ) )
			{
			unlink( szFileName );
			break;
			}
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_MAP:
		i = LOWORD(lParam);
		if ( i >= 0 )
			{
			x = TOPERCENT( lpMap->Pnt[i].x );
			y = TOPERCENT( lpMap->Pnt[i].y );
			SetDlgItemInt( hDlg, IDC_MAP0IN+i, x, NO );
			SetDlgItemInt( hDlg, IDC_MAP0OUT+i, y, NO );
			MakeMap( lpMap );
			SetDlgItemText( hDlg, IDC_MAPIN, "" );
			SetDlgItemText( hDlg, IDC_MAPOUT, "" );
			}
		else	{// Just a notification of the x coordinate
			x = TOPERCENT( HIWORD(lParam) );
			y = TOPERCENT( 255-lpMap->Lut[255-HIWORD(lParam)] );
			wsprintf( szString, "%d %%", x );
			SetDlgItemText( hDlg, IDC_MAPIN, szString );
			wsprintf( szString, "%d %%", y );
			SetDlgItemText( hDlg, IDC_MAPOUT, szString );
			}
		break;

	    case IDC_MAP0IN:
	    case IDC_MAP1IN:
	    case IDC_MAP2IN:
	    case IDC_MAP3IN:
	    case IDC_MAP4IN:
	    case IDC_MAP5IN:
	    case IDC_MAP6IN:
	    case IDC_MAP7IN:
	    case IDC_MAP8IN:
	    case IDC_MAP9IN:
	    case IDC_MAP10IN:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		lpMap->Pnt[wParam-IDC_MAP0IN].x = TOGRAY(i);
		MakeMap( lpMap );
		break;

	    case IDC_MAP0OUT:
	    case IDC_MAP1OUT:
	    case IDC_MAP2OUT:
	    case IDC_MAP3OUT:
	    case IDC_MAP4OUT:
	    case IDC_MAP5OUT:
	    case IDC_MAP6OUT:
	    case IDC_MAP7OUT:
	    case IDC_MAP8OUT:
	    case IDC_MAP9OUT:
	    case IDC_MAP10OUT:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		lpMap->Pnt[wParam-IDC_MAP0OUT].y = TOGRAY(i);
		MakeMap( lpMap );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgNewProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	DoNewInits( hDlg );
	if ( !Control.ColorEnabled )
		Control.NewIsColor = NO;
	CheckDlgButton( hDlg, IDC_NEWCOLOR, Control.NewIsColor );
	ControlEnable( hDlg, IDC_NEWCOLOR, Control.ColorEnabled );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_HEIGHT:
	    case IDC_WIDTH:
	    case IDC_RES:
	    case IDC_SCALEX:
	    case IDC_SCALEY:
		DoSizeControls( hDlg, wParam, lParam );
		break;

	    case IDC_NEWCOLOR:
		Control.NewIsColor = !Control.NewIsColor;
		CheckDlgButton( hDlg, wParam, Control.NewIsColor );
		Edit.Depth = ( Control.NewIsColor ? 3 : 1 );
		SetDlgItemInt( hDlg, IDC_MEMORY, AstralImageMemory(), NO );
		AstralControlPaint( hDlg, IDC_MEMORY );
		break;

	    case IDOK:
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, FALSE );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgToolsProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
HDC hDC;
RECT rect;
BOOL fDoubleClick;
int x, y, res, ratio;
DWORD dXY;
long Color;
FIXED fixed;
static WORD idMaskFunction, idRetouchFunction, idFilterFunction,
	    idFillFunction, idDrawFunction, idViewFunction;
static HMENU hPopup;

switch (msg)
    {
    case WM_INITDIALOG:
//	GetClientRect( hWndAstral, &rect );
//	InflateRect( &rect, 1, 1 );
//	rect.right = rect.left + 33;
//	MoveWindow( hDlg,
//		rect.left, rect.top, /* New position */
//		RectWidth( &rect ), RectHeight( &rect ), NO /* New size */ );
	rect.left = rect.top = 0;
	rect.right = 32-1;
	rect.bottom = 9*33 + 40 - 2;
	SetClientRect( hDlg, &rect );
	MoveWindow( GetDlgItem(hDlg,IDC_MASK),	     -1, 0*33-1, 34, 34, NO );
	MoveWindow( GetDlgItem(hDlg,IDC_RETOUCH),    -1, 1*33-1, 34, 34, NO );
	MoveWindow( GetDlgItem(hDlg,IDC_FILTER),     -1, 2*33-1, 34, 34, NO );
	MoveWindow( GetDlgItem(hDlg,IDC_FILL),	     -1, 3*33-1, 34, 34, NO );
	MoveWindow( GetDlgItem(hDlg,IDC_DRAW),	     -1, 4*33-1, 34, 34, NO );
	MoveWindow( GetDlgItem(hDlg,IDC_CUSTOMVIEW), -1, 5*33-1, 34, 34, NO );
	MoveWindow( GetDlgItem(hDlg,IDC_ERASER),     -1, 6*33-1, 34, 34, NO );
	MoveWindow( GetDlgItem(hDlg,IDC_TEXT),	     -1, 7*33-1, 34, 34, NO );
	MoveWindow( GetDlgItem(hDlg,IDC_PROBE),      -1, 8*33-1, 34, 34, NO );
	MoveWindow( GetDlgItem(hDlg,IDC_ACTIVECOLOR),-1, 9*33-1, 34, 41, NO );
	hPopup = InitPopupTools();
	CheckMenuItem( hPopup, idMaskFunction = IDC_FREE, MF_CHECKED );
	CheckMenuItem( hPopup, idRetouchFunction = IDC_PAINT, MF_CHECKED );
	CheckMenuItem( hPopup, idFilterFunction = IDC_SHARP, MF_CHECKED );
	CheckMenuItem( hPopup, idFillFunction = IDC_VIGNETTE, MF_CHECKED );
	CheckMenuItem( hPopup, idDrawFunction = IDC_PENCIL, MF_CHECKED );
	CheckRadioButton( hDlg, IDC_FIRSTTOOL, IDC_LASTTOOL, IDC_MASK );
	ActivateTool( IDC_SQUARE );
	CopyRGB(&Palette.ActiveRGB, &Color);
	SetWindowLong( GetDlgItem( hDlg, IDC_ACTIVECOLOR ), 0, Color );
	CopyRGB(&Palette.AlternateRGB, &Color);
	SetWindowLong( GetDlgItem( hDlg, IDC_ACTIVECOLOR ), 4, Color );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_MENUSELECT:
	MessageStatus( wParam );
	break;

    case WM_SIZE:
	if ( wParam == SIZEICONIC )
		SetWindowText( hDlg, "Tools" );
	else	SetWindowText( hDlg, "" );
	break;

    case WM_MEASUREITEM:
	MeasurePopupMenu( (LPMEASUREITEMSTRUCT)lParam );
	break;

    case WM_DRAWITEM:
	DrawPopupMenu( (LPDRAWITEMSTRUCT)lParam );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	fDoubleClick = (lParam == 1);
	switch (wParam)
	    {
	    case IDC_ACTIVECOLOR:
		if ( fDoubleClick )
			{ // Bring up the color picker
			if ( !ColorPicker( &Palette.ActiveRGB ) )
				break;
			}
		// The "2" message is sent from the probe and the palette to
		// force an update; they leave the color in Palette.ActiveRGB
		if ( fDoubleClick || lParam == 2 )
			{
			CopyRGB(&Palette.ActiveRGB, &Color);
			SetWindowLong( GetDlgItem( hDlg, wParam ), 0, Color );
			AstralControlRepaint( hDlg, wParam );
			break;
			}

		Color = GetWindowLong( GetDlgItem( hDlg, wParam ), 0 );
		CopyRGB(&Color, &Palette.ActiveRGB);
		Color = GetWindowLong( GetDlgItem( hDlg, wParam ), 4 );
		CopyRGB(&Color, &Palette.AlternateRGB);
		break;

	    case IDC_MASK:
		CheckRadioButton( hDlg, IDC_FIRSTTOOL, IDC_LASTTOOL, wParam );
		PopupTools( hDlg, wParam );
		ActivateTool( idMaskFunction );
		break;

	    case IDC_RETOUCH:
		CheckRadioButton( hDlg, IDC_FIRSTTOOL, IDC_LASTTOOL, wParam );
		PopupTools( hDlg, wParam );
		ActivateTool( idRetouchFunction );
		break;

	    case IDC_FILTER:
		CheckRadioButton( hDlg, IDC_FIRSTTOOL, IDC_LASTTOOL, wParam );
		PopupTools( hDlg, wParam );
		ActivateTool( idFilterFunction );
		break;

	    case IDC_FILL:
		CheckRadioButton( hDlg, IDC_FIRSTTOOL, IDC_LASTTOOL, wParam );
		PopupTools( hDlg, wParam );
		ActivateTool( idFillFunction );
		break;

	    case IDC_DRAW:
		CheckRadioButton( hDlg, IDC_FIRSTTOOL, IDC_LASTTOOL, wParam );
		PopupTools( hDlg, wParam );
		ActivateTool( idDrawFunction );
		break;

		CheckRadioButton( hDlg, IDC_FIRSTTOOL, IDC_LASTTOOL, wParam );
		PopupTools( hDlg, wParam );
		ActivateTool( idViewFunction );
		break;

	    case IDC_CUSTOMVIEW:
	    case IDC_ERASER:
	    case IDC_TEXT:
	    case IDC_PROBE:
		CheckRadioButton( hDlg, IDC_FIRSTTOOL, IDC_LASTTOOL, wParam );
		ActivateTool( wParam );
		break;

	    case IDC_FREE:
	    case IDC_SQUARE:
	    case IDC_CIRCLE:
	    case IDC_MAGICWAND:
	    case IDC_SHIELD:
	    case IDC_TRANSFORMER:
	    case IDC_POINTER:
		CheckMenuItem( hPopup, idMaskFunction, MF_UNCHECKED );
		CheckMenuItem( hPopup, idMaskFunction = wParam, MF_CHECKED );
		ActivateTool( wParam );
		break;

	    case IDC_PAINT:
	    case IDC_SPRAY:
	    case IDC_CLONE:
	    case IDC_TEXTURE:
	    case IDC_SMEAR:
		CheckMenuItem( hPopup, idRetouchFunction, MF_UNCHECKED );
		CheckMenuItem( hPopup, idRetouchFunction = wParam, MF_CHECKED );
		ActivateTool( wParam );
		break;

	    case IDC_SHARP:
	    case IDC_SMOOTH:
	    case IDC_LIGHTEN:
	    case IDC_DARKEN:
		CheckMenuItem( hPopup, idFilterFunction, MF_UNCHECKED );
		CheckMenuItem( hPopup, idFilterFunction = wParam, MF_CHECKED );
		ActivateTool( wParam );
		break;

	    case IDC_PENCIL:
	    case IDC_DRAWSQUARE:
	    case IDC_DRAWCIRCLE:
	    case IDC_DRAWFREE:
		CheckMenuItem( hPopup, idDrawFunction, MF_UNCHECKED );
		CheckMenuItem( hPopup, idDrawFunction = wParam, MF_CHECKED );
		ActivateTool( wParam );
		break;

	    case IDC_VIGNETTE:
	    case IDC_TEXTUREFILL:
	    case IDC_TINTFILL:
	    case IDC_FLOOD:
		CheckMenuItem( hPopup, idFillFunction, MF_UNCHECKED );
		CheckMenuItem( hPopup, idFillFunction = wParam, MF_CHECKED );
		ActivateTool( wParam );
		break;
	    }
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
void SaveRetouchBrush( brush )
/***********************************************************************/
int brush;
{
    if (brush == NO_BRUSH)
	return;

    RetouchBrushes [brush].Pressure = Retouch.Pressure;
    RetouchBrushes [brush].BrushShape = Retouch.BrushShape;
    RetouchBrushes [brush].BrushStyle = Retouch.BrushStyle;
    RetouchBrushes [brush].BrushSize = Retouch.BrushSize;
    RetouchBrushes [brush].FuzzyPixels = Retouch.FuzzyPixels;
    RetouchBrushes [brush].Opacity = Retouch.Opacity;
    RetouchBrushes [brush].Fadeout = Retouch.Fadeout;
}


/***********************************************************************/
void SelectRetouchBrush( brush )
/***********************************************************************/
int brush;
{
    if (brush == NO_BRUSH)
	return;

    SaveRetouchBrush( RetouchBrushCurrent );

    Retouch.Pressure = RetouchBrushes [brush].Pressure;
    Retouch.BrushShape = RetouchBrushes [brush].BrushShape;
    Retouch.BrushStyle = RetouchBrushes [brush].BrushStyle;
    Retouch.BrushSize = RetouchBrushes [brush].BrushSize;
    Retouch.FuzzyPixels = RetouchBrushes [brush].FuzzyPixels;
    Retouch.Opacity = RetouchBrushes [brush].Opacity;
    Retouch.Fadeout = RetouchBrushes [brush].Fadeout;

    RetouchBrushCurrent = brush;
}


/***********************************************************************/
void ActivateTool( idTool )
/***********************************************************************/
WORD idTool;
{
HWND hWnd;
WORD idCursor, idOptionBox;
FARPROC lpOption;
LPROC lpTool;
BOOL Bool;

DeactivateTool();

switch ( idTool )
    {
    case IDC_FREE:
	lpOption = DlgFreeProc;
	lpTool   = FreeProc;
	idOptionBox = IDD_FREE;
	idCursor = ID_FREE;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_SQUARE:
	lpOption = DlgRectangularProc;
	lpTool   = SquareProc;
	idOptionBox = IDD_SQUARE;
	idCursor = ID_FREE;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_CIRCLE:
	lpOption = DlgEllipticalProc;
	lpTool   = CircleProc;
	idOptionBox = IDD_CIRCLE;
	idCursor = ID_FREE;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_MAGICWAND:
	lpOption = DlgMagicProc;
	lpTool   = MagicProc;
	idOptionBox = IDD_MAGICWAND;
	idCursor = ID_MAGICWAND;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_SHIELD:
	lpOption = DlgShieldProc;
	lpTool   = ShieldProc;
	idOptionBox = IDD_SHIELD;
	idCursor = ID_SHIELD;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_TRANSFORMER:
	lpOption = DlgTransformerProc;
	lpTool   = TransformerProc;
	idOptionBox = IDD_TRANSFORMER;
	idCursor = ID_ARROW;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_POINTER:
	lpOption = DlgPointerProc;
	lpTool   = PointerProc;
	idOptionBox = IDD_POINTER;
	idCursor = ID_ARROW;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_PAINT:
	lpOption = DlgPaintProc;
	lpTool   = PaintProc;
	idOptionBox = IDD_PAINT;
	idCursor = ID_NULL;
	SelectRetouchBrush (PAINT_BRUSH);
	break;
    case IDC_SPRAY:
	lpOption = DlgPaintProc;
	lpTool   = SprayProc;
	idOptionBox = IDD_SPRAY;
	idCursor = ID_NULL;
	SelectRetouchBrush (SPRAY_BRUSH);
	break;
    case IDC_CLONE:
	lpOption = DlgPaintProc;
	lpTool   = CloneProc;
	idOptionBox = IDD_CLONE;
	idCursor = ID_NULL;
	SelectRetouchBrush (CLONE_BRUSH);
	break;
    case IDC_TEXTURE:
	lpOption = DlgPaintProc;
	lpTool   = TextureProc;
	idOptionBox = IDD_TEXTURE;
	idCursor = ID_NULL;
	SelectRetouchBrush (TEXTURE_BRUSH);
	break;
    case IDC_SMEAR:
	lpOption = DlgPaintProc;
	lpTool   = SmearProc;
	idOptionBox = IDD_SMEAR;
	idCursor = ID_NULL;
	SelectRetouchBrush (SMEAR_BRUSH);
	break;
    case IDC_SHARP:
	lpOption = DlgPaintProc;
	lpTool   = SharpSmoothProc;
	idOptionBox = IDD_SHARP;
	idCursor = ID_NULL;
	SelectRetouchBrush (SHARPEN_BRUSH);
	break;
    case IDC_SMOOTH:
	lpOption = DlgPaintProc;
	lpTool   = SharpSmoothProc;
	idOptionBox = IDD_SMOOTH;
	idCursor = ID_NULL;
	SelectRetouchBrush (SMOOTH_BRUSH);
	break;
    case IDC_LIGHTEN:
	lpOption = DlgPaintProc;
	lpTool   = LightenProc;
	idOptionBox = IDD_LIGHTEN;
	idCursor = ID_NULL;
	SelectRetouchBrush (LIGHTEN_BRUSH);
	break;
    case IDC_DARKEN:
	lpOption = DlgPaintProc;
	lpTool   = DarkenProc;
	idOptionBox = IDD_DARKEN;
	idCursor = ID_NULL;
	SelectRetouchBrush (DARKEN_BRUSH);
	break;
    case IDC_VIGNETTE:
	lpOption = DlgVignetteProc;
	lpTool   = VignetteProc;
	idOptionBox = IDD_VIGNETTE;
	idCursor = ID_VIGNETTE;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_TEXTUREFILL:
	lpOption = DlgTextureFillProc;
	lpTool   = TextureFillProc;
	idOptionBox = IDD_TEXTUREFILL;
	idCursor = ID_FLOOD;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_TINTFILL:
	lpOption = DlgTintFillProc;
	lpTool   = TintFillProc;
	idOptionBox = IDD_TINTFILL;
	idCursor = ID_FLOOD;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_FLOOD:
	lpOption = DlgFloodProc;
	lpTool   = FloodProc;
	idOptionBox = IDD_FLOOD;
	idCursor = ID_MAGICWAND;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_PENCIL:
	lpOption = DlgPaintProc;
	lpTool   = FreeProc;
	idOptionBox = IDD_PENCIL;
	idCursor = ID_PENCIL;
	SelectRetouchBrush (DRAW_LINE_BRUSH);
	break;
    case IDC_DRAWFREE:
	lpOption = DlgPaintProc;
	lpTool   = FreeProc;
	idOptionBox = IDD_DRAWFREE;
	idCursor = ID_PENCIL;
	SelectRetouchBrush (DRAW_FREE_BRUSH);
	break;
    case IDC_DRAWCIRCLE:
	lpOption = DlgPaintProc;
	lpTool   = CircleProc;
	idOptionBox = IDD_DRAWCIRCLE;
	idCursor = ID_PENCIL;
	SelectRetouchBrush (DRAW_ELLIPSE_BRUSH);
	break;
    case IDC_DRAWSQUARE:
	lpOption = DlgPaintProc;
	lpTool   = SquareProc;
	idOptionBox = IDD_DRAWSQUARE;
	idCursor = ID_PENCIL;
	SelectRetouchBrush (DRAW_RECT_BRUSH);
	break;
    case IDC_CUSTOMVIEW:
	lpOption = DlgViewProc;
	lpTool   = CustomProc;
	idOptionBox = IDD_CUSTOMVIEW;
	idCursor = ID_CUSTOMVIEW;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_GRABBER:
	lpOption = NULL;
	lpTool   = GrabberProc;
	idOptionBox = NULL;
	idCursor = ID_GRABBER;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_ERASER:
	lpOption = DlgPaintProc;
	lpTool   = EraserProc;
	idOptionBox = IDD_ERASER;
	idCursor = ID_NULL;
	SelectRetouchBrush (ERASER_BRUSH);
	break;
    case IDC_TEXT:
	lpOption = DlgTextProc;
	lpTool   = TextProc;
	idOptionBox = IDD_TEXT;
	idCursor = ID_TEXT;
	SelectRetouchBrush (NO_BRUSH);
	break;
    case IDC_PROBE:
	lpOption = DlgProbeProc;
	lpTool   = ProbeProc;
	idOptionBox = IDD_PROBE;
	idCursor = ID_PROBE;
	SelectRetouchBrush (NO_BRUSH);
	break;
    default:
	lpOption = NULL;
	lpTool   = NULL;
	idOptionBox = NULL;
	idCursor = ID_ARROW;
	SelectRetouchBrush (NO_BRUSH);
	break;
    }

Control.Function = idTool;
Window.ToolActive = NO;
Window.OptionProc = lpOption;
Window.ToolProc = lpTool;
if (idCursor > LAST_CURSOR)
     Window.hCursor = LoadCursor( NULL, MAKEINTRESOURCE(idCursor) );
else Window.hCursor = LoadCursor( hInstAstral, MAKEINTRESOURCE(idCursor) );
if ( lpImage && lpImage->hWnd )
	SetClassWord( lpImage->hWnd, GCW_HCURSOR, Window.hCursor );

// Show the new option box (if any) and close the old option box (if any)
if ( idOptionBox && idOptionBox != Window.idOptionBox )
	{
	if ( hWnd = AstralDlgGet( idOptionBox ) )
		{
		SendMessage( hWnd, WM_INITDIALOG, 0, 0L );
		AstralDlgShow( idOptionBox );
		}
	else
	if ( lpOption )
		AstralDlg( YES, hInstAstral, hWndAstral, idOptionBox, lpOption);
	if ( hWnd = AstralDlgGet( Window.idOptionBox ) )
		AstralDlgEnd(hWnd, TRUE);
//	AstralDlgHide( Window.idOptionBox );
	Window.idOptionBox = idOptionBox;
	}
}


/***********************************************************************/
void DeactivateTool()
/***********************************************************************/
{
if ( Window.ModalProc)
	if ( lpImage && lpImage->hWnd )
    	    	ModalDestroyProc( lpImage->hWnd, 0L);
if ( Window.ToolActive && Window.ToolProc )
	if ( lpImage && lpImage->hWnd )
		DestroyProc( lpImage->hWnd, 0L );
}

/***********************************************************************/
BOOL FAR PASCAL DlgPaletteProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL fDoubleClick;
int i, id, Entry;
HWND hWnd;
long Color;

switch (msg)
    {
    case WM_INITDIALOG:
	id = IDC_PALETTE001;
	for ( i=0; i<MAX_USER_COLORS; i++ )
		{
		CopyRGB(&Palette.RGB[i], &Color);
		SetWindowLong( GetDlgItem( hDlg, id ), 0, Color );
		id++;
		}
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_SIZE:
	if ( wParam == SIZEICONIC )
		SetWindowText( hDlg, "Palette" );
	else	SetWindowText( hDlg, "" );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	if ( wParam < IDC_PALETTE001 || wParam > IDC_PALETTE128 )
		break;
	Entry = wParam - IDC_PALETTE001;
	if ( fDoubleClick = lParam )
		{ // Bring up the color picker
		if ( !ColorPicker( &Palette.RGB[Entry] ) )
			break;
		}
	Palette.ActiveRGB = Palette.RGB[Entry];
	CopyRGB( &Palette.ActiveRGB, &Color );
	SetWindowLong( GetDlgItem( hDlg, wParam ), 0, Color );
	AstralControlRepaint( hDlg, wParam );
	if ( hWnd = AstralDlgGet(IDD_TOOLS) )
		SendMessage( hWnd, WM_COMMAND, IDC_ACTIVECOLOR, 2L);
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgStatusBoxProc( hDlg, message, wParam, lParam )
/***********************************************************************/
HWND	 hDlg;
unsigned message;
WORD	 wParam;
long	 lParam;
{
switch (message)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	break;
    default:
  	return(FALSE);
    }
return(TRUE);
}


/***********************************************************************/
BOOL FAR PASCAL DlgPrefProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
static BOOL	Bool, EMSdisable, UseApply;
static char	RamDisk[64];
static int	MainMemMin, MainMemFactor;
static char	PouchPath[64];
static FIXED	ScreenWidth;
static WORD	Units;
static int	ScratchpadWidth, ScratchpadHeight;
static BOOL	IsScratchpadColor;
int		temp;
LPTR		lpUnit;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	EMSstatus( &Control.EMStotal, &Control.EMSavail );
	CheckDlgButton( hDlg, IDC_PREF_EMS,
		!Control.EMSdisable && Control.EMStotal );
	CheckDlgButton( hDlg, IDC_PREF_USEAPPLY, Control.UseApply );
	SetDlgItemInt( hDlg, IDC_PREF_MEMMIN, Control.MainMemMin, NO );
	SetDlgItemInt( hDlg, IDC_PREF_MEMFACTOR, Control.MainMemFactor, NO );
	SetDlgItemText( hDlg, IDC_PREF_RAMDISK, Control.RamDisk );
	SetDlgItemText( hDlg, IDC_PREF_POUCH, Control.PouchPath );
	CheckRadioButton( hDlg, IDC_PREF_UNITINCHES, IDC_PREF_UNITCM,
				Control.Units );
	SetDlgItemFixed( hDlg, IDC_PREF_SCREENWIDTH, Control.ScreenWidth, NO );
	InitDlgItemSpin( hDlg, IDC_PREF_SCRATCHX, Control.ScratchpadWidth, NO,
		0, 500 );
	InitDlgItemSpin( hDlg, IDC_PREF_SCRATCHY, Control.ScratchpadHeight, NO,
		0, 500 );
	CheckRadioButton( hDlg, IDC_PREF_SCRATCHGRAY, IDC_PREF_SCRATCHCOLOR,
		Control.IsScratchpadColor + IDC_PREF_SCRATCHGRAY );
	if ( AstralStr( Control.Units, &lpUnit ) )
		SetDlgItemText( hDlg, IDC_WIDTHLABEL, lpUnit );
	ControlEnable( hDlg, IDC_PREF_EMS, Control.EMStotal );

	EMSdisable = Control.EMSdisable;
	UseApply = Control.UseApply;
	MainMemMin = Control.MainMemMin;
	MainMemFactor = Control.MainMemFactor;
	Units = Control.Units;
	ScreenWidth = Control.ScreenWidth;
	strncpy (RamDisk, Control.RamDisk, 64);
	strncpy (PouchPath, Control.PouchPath, 64);
	ScratchpadWidth   = Control.ScratchpadWidth;
	ScratchpadHeight  = Control.ScratchpadHeight;
	IsScratchpadColor = Control.IsScratchpadColor;
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_PREF_POUCH:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		    break;
		if ( HIWORD(lParam) != EN_CHANGE )
		    break;
		GetDlgItemText(hDlg, wParam, Control.PouchPath,
			sizeof(Control.PouchPath));
		break;

	    case IDC_PREF_RAMDISK:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		    break;
		if ( HIWORD(lParam) != EN_CHANGE )
		    break;
		GetDlgItemText(hDlg, wParam, Control.RamDisk,
			sizeof(Control.RamDisk));
		break;

	    case IDC_PREF_EMS:
		Control.EMSdisable = !Control.EMSdisable;
		CheckDlgButton( hDlg, wParam, !Control.EMSdisable );
		break;

	    case IDC_PREF_USEAPPLY:
		Control.UseApply = !Control.UseApply;
		CheckDlgButton( hDlg, wParam, Control.UseApply );
		break;

	    case IDC_PREF_MEMMIN:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		    break;
		if ( HIWORD(lParam) != EN_CHANGE )
		    break;
		Control.MainMemMin =
			GetDlgItemInt( hDlg, wParam, &Bool, NO );
		break;

	    case IDC_PREF_MEMFACTOR:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		    break;
		if ( HIWORD(lParam) != EN_CHANGE )
		    break;
		Control.MainMemFactor =
			GetDlgItemInt( hDlg, wParam, &Bool, NO );
		break;

	    case IDC_PREF_SCREENWIDTH:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Control.ScreenWidth = GetDlgItemFixed( hDlg, wParam, &Bool, NO);
		break;

	    case IDC_PREF_UNITINCHES:
	    case IDC_PREF_UNITMM:
	    case IDC_PREF_UNITCM:
	    case IDC_PREF_UNITPICAS:
		Control.Units = wParam;
		CheckRadioButton( hDlg, IDC_PREF_UNITINCHES,
				IDC_PREF_UNITCM, wParam );
		if ( AstralStr( Control.Units, &lpUnit ) )
			SetDlgItemText( hDlg, IDC_WIDTHLABEL, lpUnit );
		SetDlgItemFixed( hDlg, IDC_PREF_SCREENWIDTH,
			Control.ScreenWidth, NO );
		AstralControlPaint( hDlg, IDC_WIDTHLABEL );
		AstralControlPaint( hDlg, IDC_PREF_SCREENWIDTH );
		break;

	    case IDC_PREF_SCRATCHX:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Control.ScratchpadWidth = GetDlgItemSpin( hDlg, wParam, &Bool,
			NO );
		break;

	    case IDC_PREF_SCRATCHY:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Control.ScratchpadHeight = GetDlgItemSpin( hDlg, wParam, &Bool,
			NO );
		break;

	    case IDC_PREF_SCRATCHGRAY:
	    case IDC_PREF_SCRATCHCOLOR:
		CheckRadioButton( hDlg, IDC_PREF_SCRATCHGRAY,
			IDC_PREF_SCRATCHCOLOR, wParam );
		Control.IsScratchpadColor = wParam - IDC_PREF_SCRATCHGRAY;
		break;

	    case IDC_PREF_SAVE:
	    case IDOK:
		temp = bound( Control.MainMemFactor, 10, 100 );
		if (temp != Control.MainMemFactor)
		    {
		    Control.MainMemFactor = temp;
		    SetDlgItemInt( hDlg, IDC_PREF_MEMFACTOR,
			    Control.MainMemFactor, NO );
		    SetFocus ( GetDlgItem(hDlg,IDC_PREF_MEMFACTOR) );
		    MessageBeep(0);
		    break;
		    }
		if (wParam == IDC_PREF_SAVE)
		    StoreAllPreferences( Control.ProgHome );
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		Control.EMSdisable = EMSdisable;
		Control.UseApply = UseApply;
		Control.MainMemMin = MainMemMin;
		Control.MainMemFactor = MainMemFactor;
		Control.Units = Units;
		Control.ScreenWidth = ScreenWidth;
		strncpy (Control.RamDisk, RamDisk, 64);
		strncpy (Control.PouchPath, PouchPath, 64);
		Control.ScratchpadWidth   = ScratchpadWidth;
		Control.ScratchpadHeight  = ScratchpadHeight;
		Control.IsScratchpadColor = IsScratchpadColor;
		AstralDlgEnd( hDlg, TRUE );
		break;
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL FAR PASCAL DlgInfoProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
LPTR lpString, lpUnit;
char szString[256];
char szValue[256];
char szFormat[256];
char *pFileType;
int iSize;
long l;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	SetDlgItemText(hDlg, IDC_INFOFILENAME, stripdir(lpImage->CurFile) );
        if (AstralStr(lpImage->FileType-IDC_SAVETIFF+IDS_TIFF, &lpString))
		SetDlgItemText(hDlg, IDC_INFOFILETYPE, lpString);
	if (AstralStr(lpImage->Depth+IDS_DEPTH0, &lpString))
		SetDlgItemText(hDlg, IDC_INFODATATYPE, lpString);
	if (AstralStr(lpImage->fChanged ? IDS_YES : IDS_NO, &lpString))
		SetDlgItemText(hDlg, IDC_INFOMODIFIED, lpString);
	if (AstralStr(IDS_INFOSIZE, &lpString))
		{
		lstrcpy(szFormat, lpString);
		if (AstralStr(Control.Units, &lpUnit))
			{
			Unit2String(FGET(lpImage->npix, lpImage->xres),
				szValue);
			wsprintf(szString, szFormat, lpImage->npix,
				(LPTR)szValue, lpUnit);
			SetDlgItemText(hDlg, IDC_INFOWIDTH, szString);
			Unit2String(FGET(lpImage->nlin, lpImage->yres),
				szValue);
			wsprintf(szString, szFormat, lpImage->nlin,
				(LPTR)szValue, lpUnit);
			SetDlgItemText(hDlg, IDC_INFOHEIGHT, szString);
			}
		}
	SetDlgItemInt(hDlg, IDC_INFORESOLUTION, lpImage->xres, NO);
	if (lpImage->Depth)
		iSize = ( (long)lpImage->Depth * 
			  (long)lpImage->npix  * 
			  (long)lpImage->nlin ) / 1024L;
	else	iSize = ( ((long)lpImage->npix * 
			  (long)lpImage->nlin ) +7L ) / 8192L;
	wsprintf( szString, "%dK", iSize );
	SetDlgItemText(hDlg, IDC_INFOIMAGEMEMORY, szString);
	l = (ConventionalMemory()+512) / 1024L;
	lpString = szString;
	wsprintf( lpString, "%ldK", l );
	EMSstatus( &Control.EMStotal, &Control.EMSavail );
	if ( Control.EMStotal )
		{
		lpString += lstrlen(lpString);
		wsprintf( lpString, " + %uKexp", Control.EMSavail );
		}
	SetDlgItemText(hDlg, IDC_INFOMEMORY, szString);
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP )
		Help( IDM_HELP_INDEX, (long)wParam );
	switch (wParam)
	    {
	    case IDOK:
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, TRUE );
		break;
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}
