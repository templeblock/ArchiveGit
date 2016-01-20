//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern BOOL bHexBrush;              // used by hexcircle brush with virt keys

#define ZIGZAG_BRUSHSIZE 2

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaintProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
int i;
BOOL Bool;
ITEMID id;
static int iTool;

switch (msg)
    {
    case WM_INITDIALOG:
	RibbonInit( hDlg );
	ColorInit( hDlg );
	CheckRadioButton( hDlg, IDC_PAINT_T1, IDC_PAINT_T10,
		iTool+IDC_PAINT_T1 );
	Retouch.Opacity = 128;
	Retouch.Opacity = bound( Retouch.Opacity, 255-TOGRAY(25), 255-TOGRAY(75) );
	InitSlide( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Retouch.Opacity), 25, 75 );
	#ifdef STUDIO
		InitSlide( hDlg, IDC_BRUSHSIZE, Retouch.BrushSize, 1, MAX_BRUSH_SIZE );
	#else
		switch (Control.iAdventBrush)
		{
			case BRUSH_SIZE_SMALL:	Retouch.BrushSize = 20; break;
			case BRUSH_SIZE_MEDIUM:	Retouch.BrushSize = 25; break;
			case BRUSH_SIZE_LARGE: 	Retouch.BrushSize = 30; break;
			default:				Retouch.BrushSize = 25; break;
		}
//		Retouch.BrushSize = 25;
	#endif
	// Fall thru...

    case WM_CONTROLENABLE:
    case WM_DOCACTIVATED:
	if ( !(Retouch.hBrush = CreateDefaultBrush( Retouch.hBrush )) )
		break;
	SetBrushOptions( Retouch.hBrush, iTool );
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

    case WM_MEASUREITEM:
    case WM_DRAWITEM:
	id = ((LPDRAWITEMSTRUCT)lParam)->CtlID;
	Bool = (id == IDC_BRUSHSHAPE);
	return( OwnerDraw( hDlg, msg, lParam, Bool ) );

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	switch (wParam)
	    {
		case IDC_ACTIVECOLOR:
		case IDC_COLORS_PAINT:
			ColorCommand( hDlg, wParam, lParam );
			break;

		case IDC_PAINT_T1:
		case IDC_PAINT_T2:
		case IDC_PAINT_T3:
		case IDC_PAINT_T4:
		case IDC_PAINT_T5:
		case IDC_PAINT_T6:
		case IDC_PAINT_T7:
		case IDC_PAINT_T8:
		case IDC_PAINT_T9:
		case IDC_PAINT_T10:
		CheckRadioButton( hDlg, IDC_PAINT_T1, IDC_PAINT_T10, wParam );
		iTool = wParam - IDC_PAINT_T1;
   		SetBrushOptions( Retouch.hBrush, iTool );
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

	    case IDC_TRANSPARENT:
		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
			break;
		i = HandleSlide( hDlg, (ITEMID)wParam, (UINT)lParam, NULL );
		Retouch.Opacity = 255-TOGRAY(i);
		SetMgxBrushOpacity(Retouch.hBrush, Retouch.Opacity);
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
static void SetBrushOptions( HMGXBRUSH hBrush, int iTool )
/************************************************************************/
{
	SetMgxBrushOverlap( Retouch.hBrush, NO );
	SetMgxBrushShape( Retouch.hBrush, IDC_BRUSHCIRCLE, NULL );
	SetSimpleBrushSpacing( Retouch.hBrush, 25 );
	SetMgxBrushStyle( Retouch.hBrush, IDC_BRUSHSOLID );
	SetSimpleBrushFade( Retouch.hBrush, 0 );
	SetMgxBrushFeather( Retouch.hBrush, 0 );
    SetMgxBrushSourceFunction( Retouch.hBrush, SF_SOLID_COLOR );
    SetMgxBrushWeight( Retouch.hBrush, 0 );
	SetMgxBrushMergeMode( Retouch.hBrush, MM_MULTIPLY );
    bHexBrush = NO;
    switch( iTool )
    {
	    case 0:
            break;

	    case 1:
			SetMgxBrushFeather( Retouch.hBrush, 75 );
            break;

	    case 2:
			SetSimpleBrushFade( Retouch.hBrush, 75 );
            break;

	    case 3:
			SetSimpleBrushSpacing( Retouch.hBrush, 100 );
            break;

	    case 4:
            bHexBrush = YES;    
        	SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHHEXCIRCLE );
            // brushsize must be even to maintain precision
            if(( Retouch.BrushSize % 2 ) && ( Retouch.BrushSize > 6 ))
            {
                Retouch.BrushSize++;
           	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
                SetCommonBrushSize( Retouch.BrushSize );
            }
            SetSimpleBrushSpacing( Retouch.hBrush, 50 );
            break;

	    case 5:
			SetMgxBrushStyle( Retouch.hBrush, IDC_BRUSHSCATTER );
            break;

	    case 6:
			SetMgxBrushFeather( Retouch.hBrush, 75 );
			SetMgxBrushStyle( Retouch.hBrush, IDC_BRUSHSCATTER );
            break;

	    case 7:
			SetSimpleBrushFade( Retouch.hBrush, 75 );
			SetMgxBrushStyle( Retouch.hBrush, IDC_BRUSHSCATTER );
            break;
//          SetMgxBrushSourceFunction( Retouch.hBrush, SF_SMEAR_IMAGE );
//        	SetMgxBrushMergeMode( Retouch.hBrush, MM_NORMAL );
//          SetMgxBrushWeight( Retouch.hBrush, 5 );
//        	SetSimpleBrushSpacing( Retouch.hBrush, 0 );
            break;

	    case 8:
			SetSimpleBrushSpacing( Retouch.hBrush, 100 );
			SetMgxBrushStyle( Retouch.hBrush, IDC_BRUSHSCATTER );
            break;

	    case 9:
//          SetMgxBrushSourceFunction( Retouch.hBrush, SF_SMEAR_IMAGE );
//        	SetMgxBrushMergeMode( Retouch.hBrush, MM_NORMAL );
//			SetMgxBrushFeather( Retouch.hBrush, 50 );
//          SetMgxBrushWeight( Retouch.hBrush, 5 );
//        	SetSimpleBrushSpacing( Retouch.hBrush, 0 );

            bHexBrush = YES;    
        	SetSimpleBrushShape( Retouch.hBrush, IDC_BRUSHHEXCIRCLE );
            // brushsize must be even to maintain precision
            if(( Retouch.BrushSize % 2 ) && ( Retouch.BrushSize > 6 ))
            {
                Retouch.BrushSize++;
           	    SetSimpleBrushSize( Retouch.hBrush, Retouch.BrushSize );
                SetCommonBrushSize( Retouch.BrushSize );
            }
            SetSimpleBrushSpacing( Retouch.hBrush, 50 );
			SetMgxBrushStyle( Retouch.hBrush, IDC_BRUSHSCATTER );
            break;

        default:
            break;
    }
}

/************************************************************************/
int PaintProc( HWND hWindow, LPARAM lParam, UINT msg )
/************************************************************************/
{
int x, y, fx, fy;
LPOBJECT lpObject;
RGBS rgb;

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
	Tool.bActive = YES;
	GetActiveRGB( &rgb ); // if painting with white change to normal merge mode
	if ( rgb.red == 255 && rgb.green == 255 && rgb.blue == 255 ) // if white...
		SetMgxBrushMergeMode( Retouch.hBrush, MM_NORMAL );
	else
		SetMgxBrushMergeMode( Retouch.hBrush, MM_MULTIPLY );
	break;

	case WM_SETCURSOR:
	return( SetToolCursor( hWindow, NULL/*idMoveObject*/, ID_PLAY_OBJECT ) );

	case WM_ACTIVATE:
	if (!lParam) // a deactivate
		{
		if (Retouch.hBrush)
			DestroyMgxBrush(Retouch.hBrush);
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

#ifdef ADVENT
	switch (Control.iAdventBrush)
	{
		case BRUSH_SIZE_SMALL:	Retouch.BrushSize = 20; break;
		case BRUSH_SIZE_MEDIUM:	Retouch.BrushSize = 25; break;
		case BRUSH_SIZE_LARGE: 	Retouch.BrushSize = 30; break;
		default:				Retouch.BrushSize = 25; break;
	}
	SetMgxBrushSize (Retouch.hBrush, Retouch.BrushSize);
#endif
	if (!MgxBrushBegin(hWindow, &lpObject->Pixmap,
		lpObject->rObject.left, lpObject->rObject.top, Retouch.hBrush))
	    break;
	ImgEditedObject(lpImage, lpObject, IDS_UNDOCHANGE, NULL);
	Tool.bActive = MgxBrushActive();

	/* fall through to WM_MOUSEMOVE */

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	fx = x;
	fy = y;
	Display2File( &fx, &fy );
	MgxBrushStroke(fx, fy, fx, fy, x, y);
	break;

    case WM_LBUTTONUP:
	MgxBrushEnd(NO);
	Tool.bActive = NO;
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	MgxBrushEnd(NO);
	Tool.bActive = NO;
	break;
    }

return(TRUE);
}
