//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

static int iTool, iCategory, iTotalCategories;
static HINSTANCE hOptions;
static BOOL bAllowColor;
static LPOBJECT lpPrevious;

extern HINSTANCE hInstAstral;

/***********************************************************************/
BOOL WINPROC EXPORT DlgStampProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
LPOBJECT lpObject;
STRING szOptions;

switch (msg)
    {
    case WM_INITDIALOG:
	SetControlFont (hDlg, IDC_STAMP_CATEGORY);
	RibbonInit( hDlg );
	iTotalCategories = GetExtNameCount( IDN_STAMP );
	hOptions = OptionLoadLibrary( hDlg, IDC_STAMP_CATEGORY, 17/*iCount*/,
		IDN_STAMP, iCategory, iTotalCategories );
	OptionGetString( hOptions, 0, szOptions );
	SetWindowText( GetDlgItem( hDlg, IDC_STAMP_CATEGORY ), szOptions );
	CheckDlgButton( hDlg, IDC_STAMP_SW1, bAllowColor );
	lpPrevious = NULL;
	// Select the top object
	if ( lpObject = ImgGetNextObject( lpImage, NULL/*lpObject*/, NO/*fBottomUp*/, NO/*fLoop*/ ) )
		ImgChangeSelection( lpImage, lpObject, YES/*fSelect*/, NO/*fExtendedSel*/ );
	// fall thru...

	case WM_CONTROLENABLE:
	case WM_DOCACTIVATED:
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_STAMP ) );

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_PAINT:
	LayoutPaint( hDlg );
	break;

    case WM_DESTROY:
	if ( hOptions != hInstAstral )
		FreeLibrary( hOptions );
	return( FALSE );

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_MEASUREITEM:
    case WM_DRAWITEM:
	return( OwnerDraw( hDlg, msg, lParam, NO ) );

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	switch (wParam)
	    {
		case IDC_STAMP_CATEGORY:
		if ( hOptions != hInstAstral )
			FreeLibrary( hOptions );
		iCategory++;
		hOptions = OptionLoadLibrary( hDlg, IDC_STAMP_CATEGORY, 17/*iCount*/,
			IDN_STAMP, iCategory, iTotalCategories );
		OptionGetString( hOptions, 0, szOptions );
		SetWindowText( GetDlgItem( hDlg, IDC_STAMP_CATEGORY ), szOptions );
		break;

		case IDC_STAMP_T1:
		case IDC_STAMP_T2:
		case IDC_STAMP_T3:
		case IDC_STAMP_T4:
		case IDC_STAMP_T5:
		case IDC_STAMP_T6:
		case IDC_STAMP_T7:
		case IDC_STAMP_T8:
		case IDC_STAMP_T9:
		case IDC_STAMP_T10:
		case IDC_STAMP_T11:
		case IDC_STAMP_T12:
		case IDC_STAMP_T13:
		case IDC_STAMP_T14:
		case IDC_STAMP_T15:
		case IDC_STAMP_T16:
		case IDC_STAMP_T17:
		case IDC_STAMP_T18:
		case IDC_STAMP_T19:
		case IDC_STAMP_T20:
		if ( !lpImage )
			break;
		if ( lpPrevious )
			{
			DeactivateTool();
			ImgDeleteSelObjects( lpImage );
			}
		iTool = wParam - IDC_STAMP_T1 + 1;
		ProgressBegin(1, IDS_PROGPASTE);
		lpPrevious = AddObjectFromTiffResource( hOptions,
			(LPSTR)MAKEINTRESOURCE( iTool ), bAllowColor );
		// Select the new object
		if ( lpPrevious )
			ImgChangeSelection( lpImage, lpPrevious, YES/*fSelect*/, NO/*fExtendedSel*/ );
		ProgressEnd();
		break;

	    case IDC_STAMP_SW1:
		bAllowColor = !bAllowColor;
		CheckDlgButton( hDlg, wParam, bAllowColor );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}


/************************************************************************/
int StampProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
if ( msg == WM_MOUSEMOVE )	// sent when ToolActive is on
	lpPrevious = NULL; // since an object is being moved, it will be kept
return( ToyProc( hWindow, lParam, msg ) );
}

