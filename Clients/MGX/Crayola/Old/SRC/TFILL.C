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
static void DoFill( HWND hWindow, int x, int y );

static int iTool, iCategory, iTotalCategories;
static HINSTANCE hOptions;
static LPFRAME lpFillFrame;

extern HINSTANCE hInstAstral;
extern HWND hWndAstral;

/***********************************************************************/
BOOL WINPROC EXPORT DlgFillProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
STRING szOptions;

switch (msg)
    {
    case WM_INITDIALOG:
	SetControlFont (hDlg, IDC_FILL_CATEGORY);
	RibbonInit( hDlg );
	ColorInit( hDlg );
	iTotalCategories = GetExtNameCount( IDN_TEXTURE );
	hOptions = OptionLoadLibrary( hDlg, IDC_FILL_CATEGORY, 11/*iCount*/,
		IDN_TEXTURE, iCategory, iTotalCategories );
	OptionGetString( hOptions, 0, szOptions );
	SetWindowText( GetDlgItem( hDlg, IDC_FILL_CATEGORY ), szOptions );
	CheckRadioButton( hDlg, IDC_FILL_T1, IDC_FILL_T10,
		iTool+IDC_FILL_T1 );
	// fall thru...

	case WM_CONTROLENABLE:
	case WM_DOCACTIVATED:
	if ( lpFillFrame )
		FrameClose( lpFillFrame );
	lpFillFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_FILL ) );

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
		case IDC_COLORS_FILL:
			ColorCommand( hDlg, wParam, lParam );
			break;

		case IDC_FILL_CATEGORY:
		if ( hOptions != hInstAstral )
			FreeLibrary( hOptions );
		iCategory++;
		hOptions = OptionLoadLibrary( hDlg, IDC_FILL_CATEGORY, 11/*iCount*/,
			IDN_TEXTURE, iCategory, iTotalCategories );
		OptionGetString( hOptions, 0, szOptions );
		SetWindowText( GetDlgItem( hDlg, IDC_FILL_CATEGORY ), szOptions );
		if ( lpFillFrame )
			FrameClose( lpFillFrame );
		lpFillFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
		break;

		case IDC_FILL_T1:
		case IDC_FILL_T2:
		case IDC_FILL_T3:
		case IDC_FILL_T4:
		case IDC_FILL_T5:
		case IDC_FILL_T6:
		case IDC_FILL_T7:
		case IDC_FILL_T8:
		case IDC_FILL_T9:
		case IDC_FILL_T10:
		CheckRadioButton( hDlg, IDC_FILL_T1, IDC_FILL_T10, wParam );
		iTool = wParam - IDC_FILL_T1;
		if ( lpFillFrame )
			FrameClose( lpFillFrame );
		lpFillFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
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
int FillProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
int x, y;

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
	DoFill( hWindow, x, y );
	Tool.bActive = YES;
	break;

	case WM_SETCURSOR:
	return( SetToolCursor( hWindow, NULL/*idMoveObject*/, ID_PLAY_OBJECT ) );

    case WM_LBUTTONDOWN:
	break;

    case WM_LBUTTONUP:
	Tool.bActive = NO;
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
	Tool.bActive = NO;
	break;
    }
return(TRUE);
}


/************************************************************************/
static void DoFill( HWND hWindow, int x, int y )
/************************************************************************/
{
COLORINFO Color, ActiveColor, AlternateColor;
int x1, y1, x2, y2, dx, dy;
RECT rMask;
BOOL bFillTillBlack;

if ( !INFILE(x, y) )
	return;

Fill.FloodRange = 0;

bFillTillBlack = !IsSolidArea( ImgGetSelObject(lpImage, NULL), x, y );
Fill.idFillColorModel = (bFillTillBlack ? IDC_MODEL_RGB : IDC_MODEL_HL);
Fill.idFillColorModel -= IDC_FIRST_MODEL;

if ( iTotalCategories && (iCategory % iTotalCategories) )
	{
	if ( !lpFillFrame )
		lpFillFrame = TiffResource2Frame( hOptions, (LPSTR)MAKEINTRESOURCE(iTool+1) );
	if ( !lpFillFrame )
		return;
	RemoveMaskEx(lpImage, NULL/*dirty*/);
 	MgxMagicMask( lpImage, x, y, Fill.FloodRange, Fill.idFillColorModel,
		SHAPE_DEFAULT, IDS_UNDOCHANGE );
	//Texture.TextureOpacity, .TextureMergeMode
	TextureFill( lpImage, lpFillFrame, NO/*fHorzFlip*/, NO/*fVertFlip*/ );
	RemoveMaskEx(lpImage, NULL/*dirty*/);
	return;
	}

if ( !iTool )
	{
	GetActiveColorFromType( FrameType(ImgGetBaseEditFrame(lpImage)), &Color );
	MgxFlood( lpImage, x, y, Fill.FloodRange, Fill.FloodOpacity, &Color,
		Fill.FloodMergeMode, Fill.idFillColorModel );
	return;
	}

// iTool >= 1 and iTool <= 9
RemoveMaskEx(lpImage, NULL/*dirty*/);
MgxMagicMask( lpImage, x, y, Fill.FloodRange, Fill.idFillColorModel,
	SHAPE_DEFAULT, IDS_UNDOCHANGE );
ImgGetMaskRect( lpImage, &rMask );
switch ( iTool )
	{
	case 1: // top to bottom
	Vignette.Gradient = IDC_VIGLINEAR;
	Vignette.VigColorModel = IDC_MODEL_RGB - IDC_FIRST_MODEL;
	x1 = rMask.left;  y1 = rMask.top;
	x2 = rMask.left;  y2 = rMask.bottom;
	break;

	case 2: // left to right
	Vignette.Gradient = IDC_VIGLINEAR;
	Vignette.VigColorModel = IDC_MODEL_RGB - IDC_FIRST_MODEL;
	x1 = rMask.left;  y1 = rMask.top;
	x2 = rMask.right; y2 = rMask.top;
	break;

	case 3: // left-top to right-bottom
	Vignette.Gradient = IDC_VIGLINEAR;
	Vignette.VigColorModel = IDC_MODEL_RGB - IDC_FIRST_MODEL;
	x1 = rMask.left;  y1 = rMask.top;
	x2 = rMask.right; y2 = rMask.bottom;
	break;

	case 4: // right-top to left-bottom
	Vignette.Gradient = IDC_VIGLINEAR;
	Vignette.VigColorModel = IDC_MODEL_RGB - IDC_FIRST_MODEL;
	x1 = rMask.right; y1 = rMask.top;
	x2 = rMask.left;  y2 = rMask.bottom;
	break;

	case 5: // top to bottom
	Vignette.Gradient = IDC_VIGELLIPSE;
	Vignette.VigColorModel = IDC_MODEL_RGB - IDC_FIRST_MODEL;
	x1 = rMask.left;  y1 = rMask.top;
	x2 = rMask.right;  y2 = rMask.bottom;
	dx = x2 - x1;
	dy = y2 - y1;
	if ( dx > dy )
		{
		y1 = (y2 + y1)/2 - dx/2;
		y2 = y1 + dx;
		}
	else
		{
		x1 = (x2 + x1)/2 - dy/2;
		x2 = x1 + dy;
		}
	break;

	case 6: // top to bottom
	Vignette.Gradient = IDC_VIGLINEAR;
	Vignette.VigColorModel = IDC_MODEL_HSL - IDC_FIRST_MODEL;
	x1 = rMask.left;  y1 = rMask.top;
	x2 = rMask.left;  y2 = rMask.bottom;
	break;

	case 7: // left to right
	Vignette.Gradient = IDC_VIGLINEAR;
	Vignette.VigColorModel = IDC_MODEL_HSL - IDC_FIRST_MODEL;
	x1 = rMask.left;  y1 = rMask.top;
	x2 = rMask.right; y2 = rMask.top;
	break;

	case 8: // left-top to right-bottom
	Vignette.Gradient = IDC_VIGLINEAR;
	Vignette.VigColorModel = IDC_MODEL_HSL - IDC_FIRST_MODEL;
	x1 = rMask.left;  y1 = rMask.top;
	x2 = rMask.right; y2 = rMask.bottom;
	break;

	case 9: // right-top to left-bottom
	Vignette.Gradient = IDC_VIGLINEAR;
	Vignette.VigColorModel = IDC_MODEL_HSL - IDC_FIRST_MODEL;
	x1 = rMask.right; y1 = rMask.top;
	x2 = rMask.left;  y2 = rMask.bottom;
	break;
	}

//Vignette.RepeatCount, .SoftTransition, .VigColorModel, .VigOpacity,
//Vignette.VigMergeMode, .Midpoint
if ( Vignette.VigColorModel != (IDC_MODEL_HSL - IDC_FIRST_MODEL) )
	DoVignette( lpImage, x1, y1, x2, y2 );
else
	{
	GetActiveColor( &ActiveColor );
	GetAlternateColor( &AlternateColor );
	Color.hsl.hue = 0; Color.hsl.sat = 255; Color.hsl.lum = 128;
	SetActiveColor( &Color, CS_HSL, YES/*bToolbar*/ );
	Color.hsl.hue = 251; Color.hsl.sat = 255; Color.hsl.lum = 128;
	SetActiveColor( &Color, CS_HSL, YES/*bToolbar*/ );
	DoVignette( lpImage, x1, y1, x2, y2 );
	SetActiveColor( &AlternateColor, CS_RGB, YES/*bToolbar*/ );
	SetActiveColor( &ActiveColor, CS_RGB, YES/*bToolbar*/ );
	}
RemoveMaskEx(lpImage, NULL/*dirty*/);
}


/************************************************************************/
static BOOL IsSolidArea( LPOBJECT lpObject, int x, int y )
/************************************************************************/
{
int i, j, iDepth;
RECT rect;
BYTE cTarget, cArray[3*3*4];

if ( !lpObject )
	return( NO );
if ( !PixmapGetInfo( &lpObject->Pixmap, PMT_EDIT, NULL/*lpXSize*/,
	NULL/*lpYSize*/, &iDepth, NULL/*lpType*/ ) )
	return( NO );
if (!LoadSourceData( &lpObject->Pixmap, PMT_EDIT, x-1, y-1, 3, 3, cArray,
	&rect, NO/*Horz*/, NO/*Vert*/) )
	return( NO );
for ( i=0; i<iDepth; i++ )
	{
	cTarget = cArray[i];
	for ( j=1; j<(3*3); j++ )
		{
		if ( cTarget != cArray[i+(j*iDepth)] )
			return( NO );
		}
	}
return( YES );
}

