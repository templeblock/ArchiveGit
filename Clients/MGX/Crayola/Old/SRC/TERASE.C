//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

static BOOL bExplodeAll;
static int iTool;

/***********************************************************************/
BOOL WINPROC EXPORT DlgEraserProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
STRING szString;
COLORINFO ColorStruct;

switch (msg)
    {
    case WM_INITDIALOG:
	RibbonInit( hDlg );
	iTool = 0;
	CheckRadioButton( hDlg, IDC_ERASER_T1, IDC_ERASER_T20,
		iTool+IDC_ERASER_T1 );
	InitSlide( hDlg, IDC_BRUSHSIZE, Retouch.BrushSize, 1, MAX_BRUSH_SIZE );
	#ifdef STUDIO
		bExplodeAll = NO;
	#else
		bExplodeAll = YES;
	#endif
	CheckDlgButton( hDlg, IDC_ERASER_SW1, bExplodeAll );
	// Fall thru...

    case WM_CONTROLENABLE:
    case WM_DOCACTIVATED:
	if ( !(Retouch.hBrush = CreateDefaultBrush( Retouch.hBrush )) )
		break;
	ColorStruct.ColorSpace = CS_GRAY;
	ColorStruct.gray = 255;
	SetMgxBrushColor( Retouch.hBrush, &ColorStruct );
	SetMgxBrushShape( Retouch.hBrush, IDC_BRUSHSQUARE, NULL );
	SetMgxBrushSize( Retouch.hBrush, Retouch.BrushSize );
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, Tool.idRibbon ) );

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_PAINT:
	LayoutPaint( hDlg );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	switch (wParam)
	    {
		case IDC_ERASER_T1:
		case IDC_ERASER_T2:
		case IDC_ERASER_T3:
		case IDC_ERASER_T4:
		case IDC_ERASER_T5:
		case IDC_ERASER_T6:
		case IDC_ERASER_T7:
		case IDC_ERASER_T8:
		case IDC_ERASER_T9:
		case IDC_ERASER_T10:
		case IDC_ERASER_T11:
		case IDC_ERASER_T12:
		case IDC_ERASER_T13:
		case IDC_ERASER_T14:
		case IDC_ERASER_T15:
		case IDC_ERASER_T16:
		case IDC_ERASER_T17:
		case IDC_ERASER_T18:
		case IDC_ERASER_T19:
		case IDC_ERASER_T20:
		CheckRadioButton( hDlg, IDC_ERASER_T1, IDC_ERASER_T20, wParam );
		iTool = wParam - IDC_ERASER_T1;
		if ( iTool )
			{
			wsprintf( szString, "eraser%d", iTool );
			SoundStartResource( szString, NO/*bLoop*/, NULL/*hInstance*/ );
			}
		break;

	    case IDC_BRUSHSIZE:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		if (lpImage)
	   		DisplayBrush(0, 0, 0, OFF);
		Retouch.BrushSize = HandleSlide( hDlg, (ITEMID)wParam, (UINT)lParam, NULL );
		if (Retouch.BrushSize > MAX_BRUSH_SIZE)
			Retouch.BrushSize = MAX_BRUSH_SIZE;
		if (Retouch.BrushSize < 1)
			Retouch.BrushSize = 1;
		SetMgxBrushSize( Retouch.hBrush, Retouch.BrushSize );
		SetCommonBrushSize( Retouch.BrushSize );
		if (lpImage && Window.hCursor == Window.hNullCursor)
			DisplayBrush(lpImage->hWnd, 32767, 32767, ON);
		break;

	    case IDC_ERASER_SW1:
		bExplodeAll = !bExplodeAll;
		CheckDlgButton( hDlg, wParam, bExplodeAll );
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
int EraserProc( HWND hWindow, LPARAM lParam, UINT msg )
/************************************************************************/
{
int x, y, fx, fy;
LPOBJECT lpObject;
STRING szString;
COLORINFO ColorInfo;

x = LOWORD(lParam);
y = HIWORD(lParam);
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!ONIMAGE(x, y))
		break;
	Display2File(&x, &y);
	if ( !iTool )
		{ // the eraser brush
		if ( !ImgSelectObjectType( lpImage, x, y,
			YES/*bAllowBase*/, YES/*bAllowMovable*/, YES/*bAllowPlayable*/ ) )
				break;
		}
	else
		{ // an explosion
		if ( !ImgSelectObjectType( lpImage, x, y,
			YES/*bAllowBase*/, YES/*bAllowMovable*/, YES/*bAllowPlayable*/ ) )
				break;
		wsprintf( szString, "eraser%d", iTool );
		SoundStartResource( szString, NO/*bLoop*/, NULL/*hInstance*/ );
        Explode (lpImage->hWnd,iTool,x,y);
		if ( !bExplodeAll &&
			 ImgGetSelObject( lpImage, NULL ) != ImgGetBase( lpImage ) )
			{ // delete the single floating object and get out
			ImgDeleteSelObjects( lpImage );
            InvalidateRect( hWindow, NULL, FALSE );
			break;
			}
		if ( bExplodeAll )
			{ // Delete all floating objects
			if ( lpObject = (LPOBJECT)ImgGetBase(lpImage)->lpNext )
				ImgDeleteObjects( lpImage, lpObject, NO/*bOnlyIfSelected*/ );
			}
		// clear the base object
		ShieldsSave();
		Shields( OFF );
		ColorInfo.gray   = 255;
		SetColorInfo( &ColorInfo, &ColorInfo, CS_GRAY );
		TintFill( lpImage, &ColorInfo, 255/*Opacity*/, MM_NORMAL,
			IDS_UNDOCHANGE );
		ShieldsRestore();
		break;
		}
	if (!Retouch.hBrush)
		break;
	Tool.bActive = YES;
	break;

	case WM_SETCURSOR:
	if ( !iTool )
		{ // the eraser brush
		SetWindowCursor( Tool.Prim.idCursor = ID_NULL );
		return( SetToolCursor( hWindow, NULL, NULL ) );
		}
	else
		{ // an explosion
		SetWindowCursor( Tool.Prim.idCursor = ID_ERASE );
		return( SetToolCursor( hWindow, ID_ERASE, ID_ERASE ) );
		}

	case WM_ACTIVATE:
	if (!lParam) // a deactivate
		{
		if (Retouch.hBrush)
			DestroyMgxBrush(Retouch.hBrush);
		Retouch.hBrush = NULL;
		}
	else
		{	// an activate ( to re-create brush)
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
	ImgEditedObject(lpImage, lpObject, IDS_UNDOCHANGE, NULL);

	if (!MgxBrushBegin(hWindow, &lpObject->Pixmap,
					lpObject->rObject.left, lpObject->rObject.top,
					Retouch.hBrush))
	    break;

	if ( (Tool.bActive = MgxBrushActive()) && bExplodeAll )
		{
		ShieldsSave();
		Shields( OFF );
		}

	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x; fy = y;
	Display2File(&fx, &fy);
	MgxBrushStroke(fx, fy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	MgxBrushEnd(NO);
	if ( bExplodeAll )
		ShieldsRestore();
	Tool.bActive = NO;
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	MgxBrushEnd(NO);
	if ( bExplodeAll )
		ShieldsRestore();
	Tool.bActive = NO;
	break;
    }
return(TRUE);
}
