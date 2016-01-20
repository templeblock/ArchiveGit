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

extern HINSTANCE hInstAstral;

/***********************************************************************/
BOOL WINPROC EXPORT DlgLinesProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
STRING szOptions;
LPFRAME lpFrame;

switch (msg)
    {
    case WM_INITDIALOG:
	SetControlFont (hDlg, IDC_LINES_CATEGORY);
	RibbonInit( hDlg );
	ColorInit( hDlg );
	iTotalCategories = GetExtNameCount( IDN_CUSTOM );
	hOptions = OptionLoadLibrary( hDlg, IDC_LINES_CATEGORY, 11/*iCount*/,
		IDN_CUSTOM, iCategory, iTotalCategories );
	OptionGetString( hOptions, 0, szOptions );
	SetWindowText( GetDlgItem( hDlg, IDC_LINES_CATEGORY ), szOptions );
	CheckRadioButton( hDlg, IDC_LINES_T1, IDC_LINES_T10,
		iTool+IDC_LINES_T1 );
	// Fall thru...

    case WM_CONTROLENABLE:
    case WM_DOCACTIVATED:
	Retouch.hBrush = CreateSimpleBrush( Retouch.hBrush );
	SetSimpleBrushStrokeMethod( Retouch.hBrush, SM_SIMPLE_DDA );
	SetSimpleBrushSpacing( Retouch.hBrush, 100 );
	OptionGetString( hOptions, iTool+1, szOptions );
	lpFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
	SetSimpleBrushSource( Retouch.hBrush, lpFrame, szOptions );
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, Tool.idRibbon ) );

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

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	switch (wParam)
	    {
		case IDC_ACTIVECOLOR:
		case IDC_COLORS_LINES:
			ColorCommand( hDlg, wParam, lParam );
			break;

		case IDC_LINES_CATEGORY:
		if ( hOptions != hInstAstral )
			FreeLibrary( hOptions );
		iCategory++;
		hOptions = OptionLoadLibrary( hDlg, IDC_LINES_CATEGORY, 11/*iCount*/,
			IDN_CUSTOM, iCategory, iTotalCategories );
		OptionGetString( hOptions, 0, szOptions );
		SetWindowText( GetDlgItem( hDlg, IDC_LINES_CATEGORY ), szOptions );
		OptionGetString( hOptions, iTool+1, szOptions );
		lpFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
		SetSimpleBrushSource( Retouch.hBrush, lpFrame, szOptions );
		break;

		case IDC_LINES_T1:
		case IDC_LINES_T2:
		case IDC_LINES_T3:
		case IDC_LINES_T4:
		case IDC_LINES_T5:
		case IDC_LINES_T6:
		case IDC_LINES_T7:
		case IDC_LINES_T8:
		case IDC_LINES_T9:
		case IDC_LINES_T10:
		CheckRadioButton( hDlg, IDC_LINES_T1, IDC_LINES_T10, wParam );
		iTool = wParam - IDC_LINES_T1;
		OptionGetString( hOptions, iTool+1, szOptions );
		lpFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
		SetSimpleBrushSource( Retouch.hBrush, lpFrame, szOptions );
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
int LinesProc( HWND hWindow, LPARAM lParam, UINT msg )
/************************************************************************/
{
int x, y, fx, fy;
LPOBJECT lpObject;
RGBS rgb;
LPFRAME lpFrame;
STRING szOptions;
static RGBS rgbLast;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Display2File(&x, &y);
	if ( !ImgSelectObjectType( lpImage, x, y,
		YES/*bAllowBase*/, YES/*bAllowMovable*/, NO/*bAllowPlayable*/ ) )
			break;
	if (!Retouch.hBrush)
		break;
	GetActiveRGB( &rgb );
	if ( rgb.red != rgbLast.red || rgb.green != rgbLast.green ||
		 rgb.blue != rgbLast.blue )
		{ // if a color change, restuff the brush source
		OptionGetString( hOptions, iTool+1, szOptions );
		lpFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
		SetSimpleBrushSource( Retouch.hBrush, lpFrame, szOptions );
		rgbLast = rgb;
		}
	Tool.bActive = YES;
	break;

	case WM_SETCURSOR:
	return( SetToolCursor( hWindow, NULL/*idMoveObject*/, ID_PLAY_OBJECT ) );

	case WM_ACTIVATE:
	if (!lParam) // a deactivate
		{
		if (Retouch.hBrush)
			DestroySimpleBrush(Retouch.hBrush);
		Retouch.hBrush = NULL;
		}
	else
		{ // an activate ( to re-create brush)
		if ( Tool.hRibbon )
			SendMessage( Tool.hRibbon, WM_CONTROLENABLE, 1, 0L );
		}
	break;

    case WM_LBUTTONDOWN:
	Tool.bActive = NO;
	if (ImgCountSelObjects(lpImage, NULL) == 1)
		lpObject = ImgGetSelObject(lpImage, NULL);
	else
		{
		fx = x;
		fy = y;
		Display2File( &fx, &fy );
		lpObject = ImgFindSelObject(lpImage, fx, fy, NO);
		}
	if (!lpObject)
		break;
	if (!ImgEditInit(lpImage, ET_OBJECT, UT_DATA, lpObject))
		break;

	if (!SimpleBrushBegin(hWindow, &lpObject->Pixmap,
		lpObject->rObject.left, lpObject->rObject.top, Retouch.hBrush))
	    break;
	ImgEditedObject(lpImage, lpObject, IDS_UNDOCHANGE, NULL);
	Tool.bActive = SimpleBrushActive();

	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x;
	fy = y;
	Display2File( &fx, &fy );
	SimpleBrushStroke(fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	SimpleBrushEnd(NO);
	Tool.bActive = NO;
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	SimpleBrushEnd(NO);
	Tool.bActive = NO;
	break;
    }

return(TRUE);
}

