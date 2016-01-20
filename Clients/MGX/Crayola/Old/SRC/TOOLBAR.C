//®FD1¯®PL1¯®TP0¯®BT0¯ (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;


/***********************************************************************/
WORD ActivateTool( WORD idTool )
/***********************************************************************/
{
// never, and I mean never activate a null tool - (what exactly do you mean by that?)
if (!idTool)
	return(Tool.idLast);

DeactivateTool();
if ( idTool != Tool.id )
	{ // if a different tool, cleanup and save it
	if ( Tool.lpToolProc )
		ActivateProc( NULL, 0L );
	Tool.idLast = Tool.id;
	}

clr( (LPTR)&Tool.Prim, sizeof(Tool.Prim) );
clr( (LPTR)&Tool.Alt, sizeof(Tool.Alt) );
Tool.bPassRButton = NO;

switch ( Tool.id = idTool )
    {
//    case 1:
//	Tool.Prim.lpToolProc = DotToDotToolProc;
//	Tool.Prim.idCursor = ID_ARROW;
//	break;
//
//    case 2:
//	Tool.Prim.lpToolProc = PaintBallToolProc;
//	Tool.Prim.idCursor = ID_ARROW;
//	break;

    case IDC_GALLERY:
	Tool.Prim.lpRibbonProc = DlgGalleryProc;
	Tool.Prim.idRibbon = IDD_GALLERY;
	Tool.Prim.lpToolProc   = GalleryProc;
	Tool.Prim.idCursor = ID_NO_ACTION;
	break;

    case IDC_TRANSFORMER:
	Tool.Prim.lpRibbonProc = DlgTransformerProc;
	Tool.Prim.idRibbon = IDD_TRANSFORMER;
	Tool.Prim.lpToolProc   = TransformerProc;
	Tool.Prim.idCursor = ID_NO_ACTION;
	break;

    case IDC_PAINT:
	Tool.Prim.lpRibbonProc = DlgPaintProc;
	Tool.Prim.idRibbon = IDD_PAINT;
	Tool.Prim.lpToolProc   = PaintProc;
	Tool.Prim.idCursor = ID_NULL;
	Tool.Prim.fConstrain = YES;
	Tool.Prim.fAutoScrollSetPos = YES;
	SelectRetouchBrush( Tool.id );
#ifdef _MAC
	Tool.Prim.idCursor = ID_ARROW;
#endif	
	break;

    case IDC_CRAYON:
	Tool.Prim.lpRibbonProc = DlgCrayonProc;
	Tool.Prim.idRibbon = IDD_CRAYON;
	Tool.Prim.lpToolProc   = CrayonProc;
	Tool.Prim.idCursor = ID_NULL;
	Tool.Prim.fConstrain = YES;
	Tool.Prim.fAutoScrollSetPos = YES;
	SelectRetouchBrush( Tool.id );
#ifdef _MAC
	Tool.Prim.idCursor = ID_ARROW;
#endif	
	break;

    case IDC_MARKER:
	Tool.Prim.lpRibbonProc = DlgMarkerProc;
	Tool.Prim.idRibbon = IDD_MARKER;
	Tool.Prim.lpToolProc   = MarkerProc;
	Tool.Prim.idCursor = ID_NULL;
	Tool.Prim.fConstrain = YES;
	Tool.Prim.fAutoScrollSetPos = YES;
	SelectRetouchBrush( Tool.id );
#ifdef _MAC
	Tool.Prim.idCursor = ID_ARROW;
#endif	
	break;

    case IDC_LINES:
	Tool.Prim.lpRibbonProc = DlgLinesProc;
	Tool.Prim.idRibbon = IDD_LINES;
	Tool.Prim.lpToolProc   = LinesProc;
	Tool.Prim.idCursor = ID_LINES;
	SelectRetouchBrush( Tool.id );
	Tool.Prim.fConstrain = YES;
	Tool.Prim.fAutoScrollSetPos = YES;
	SelectRetouchBrush( Tool.id );
	break;

    case IDC_SHAPES:
	Tool.Prim.lpRibbonProc = DlgDrawShapesProc;
	Tool.Prim.idRibbon = IDD_SHAPES;
	Tool.Prim.lpToolProc   = DrawShapesProc;
	Tool.Prim.idCursor = ID_SHAPES;
	Tool.Prim.fConstrain = YES;
	SelectRetouchBrush( Tool.id );
	Draw.FillInterior = FALSE;
	Draw.DrawExterior = TRUE;
	break;

    case IDC_ERASER:
	Tool.Prim.lpRibbonProc = DlgEraserProc;
	Tool.Prim.lpToolProc   = EraserProc;
	Tool.Prim.idRibbon = IDD_ERASER;
	Tool.Prim.idCursor = ID_NULL;
	Tool.Prim.fConstrain = YES;
	Tool.Prim.fAutoScrollSetPos = YES;
	SelectRetouchBrush( Tool.id );
	break;

    case IDC_STAMP:
	Tool.Prim.lpRibbonProc = DlgStampProc;
	Tool.Prim.idRibbon = IDD_STAMP;
	Tool.Prim.lpToolProc   = StampProc;
	Tool.Prim.idCursor = ID_NO_ACTION;
	break;

    case IDC_FILL:
	Tool.Prim.lpRibbonProc = DlgFillProc;
	Tool.Prim.idRibbon = IDD_FILL;
	Tool.Prim.lpToolProc   = FillProc;
	Tool.Prim.idCursor = ID_FILL;
	break;

    case IDC_TOY:
	Tool.Prim.lpRibbonProc = DlgToyProc;
	Tool.Prim.idRibbon = IDD_TOY;
	Tool.Prim.lpToolProc   = ToyProc;
	Tool.Prim.idCursor = ID_NO_ACTION;
	break;

    case IDC_EFFECTS:
	Tool.Prim.lpRibbonProc = DlgEffectsProc;
	Tool.Prim.idRibbon = IDD_EFFECTS;
	Tool.Prim.lpToolProc   = EffectsProc;
	Tool.Prim.idCursor = ID_EFFECTS;
	break;

//	case IDC_CUSTOMVIEW:
//	Tool.Prim.lpToolProc   = CustomProc;
//	Tool.Prim.idCursor = ID_CUSTOMVIEW;
//	break;

    case IDC_TEXT:
	Tool.Prim.lpRibbonProc = DlgTextProc;
	Tool.Prim.idRibbon = IDD_TEXT;
	Tool.Prim.lpToolProc   = TextProc;
	Tool.Prim.idCursor = ID_TEXT;
	break;

    default:
	Tool.Prim.lpRibbonProc = NULL;
	Tool.Prim.idRibbon = NULL;
	Tool.Prim.lpToolProc   = NULL;
	Tool.Prim.idCursor = ID_NO_ACTION;
	break;
    }

InstallTool( NO ); // Bank in the primary tool
if ( Tool.lpToolProc )
	ActivateProc( NULL, 1L );
return( Tool.idLast );
}


/***********************************************************************/
void DeactivateTool()
/***********************************************************************/
{
if ( Tool.bActive && Tool.lpToolProc )
	if ( lpImage && lpImage->hWnd )
		DestroyProc( lpImage->hWnd, 0L );
}

/***********************************************************************/
void InstallTool(BOOL fAlternate)
/***********************************************************************/
{
WORD idCursor, idRibbon;
BOOL fDisplaysInfo, fSetCursor, fConstrain, fAutoScrollSetPos;
DLGPROC lpRibbon;
LPTOOLPROC lpTool;
HWND	hWnd;

if (fAlternate)
	{
	lpTool   = Tool.Alt.lpToolProc;
	idCursor = Tool.Alt.idCursor;
	idRibbon = Tool.Alt.idRibbon;
	lpRibbon = Tool.Alt.lpRibbonProc;
	fSetCursor = Tool.Alt.fSetCursor;
	fDisplaysInfo = Tool.Alt.fDisplaysInfo;
	fConstrain = Tool.Alt.fConstrain;
	fAutoScrollSetPos = Tool.Alt.fAutoScrollSetPos;
	}
else
	{
	lpTool   = Tool.Prim.lpToolProc;
	idCursor = Tool.Prim.idCursor;
	idRibbon = Tool.Prim.idRibbon;
	lpRibbon = Tool.Prim.lpRibbonProc;
	fSetCursor = Tool.Prim.fSetCursor;
	fDisplaysInfo = Tool.Prim.fDisplaysInfo;
	fConstrain = Tool.Prim.fConstrain;
	fAutoScrollSetPos = Tool.Prim.fAutoScrollSetPos;
	}

Tool.bAltTool = fAlternate;
Tool.lpToolProc = lpTool;
Tool.fSetCursor = fSetCursor;
Tool.fDisplaysInfo = fDisplaysInfo;
Tool.fConstrain = fConstrain;
Tool.fAutoScrollSetPos = fAutoScrollSetPos;

SetWindowCursor(idCursor);

// close the old Ribbon (if any)
if ( hWnd = AstralDlgGet( Tool.idRibbon ) )
	AstralDlgEnd(hWnd, TRUE|2);

// Show the new Ribbon (if any) and if successful,
if ( idRibbon && idRibbon != Tool.idRibbon )
	{
	if ( hWnd = AstralDlgGet(IDD_MAIN) )
		{
		ToolMover( hWnd, Tool.id );
		if ( lpRibbon )
			AstralDlg( YES|2, hInstAstral, hWnd, idRibbon, lpRibbon );
		}
	// Move the new ribbon to the bottom of the Z order
	Tool.idRibbon = idRibbon;
	if ( Tool.hRibbon = AstralDlgGet( Tool.idRibbon ) )
		SetWindowPos( Tool.hRibbon, HWND_BOTTOM, 0, 0, 0, 0,
		    SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE );
	}
}


/***********************************************************************/
static void ToolMover( HWND hWnd, ITEMID idTool )
/***********************************************************************/
{
int by;
POINT pt;
RECT rControl, rRect;
HWND hControl;
STRING szString;
static HWND hOldControl;

//UpdateWindow( hWnd );
if ( hOldControl )
	{
	ShowWindow( hOldControl, SW_NORMAL );
	UpdateWindow( hOldControl );
	}
if ( hControl = GetDlgItem( hWnd, idTool ) )
	{
	ShowWindow( hControl, SW_HIDE );
	UpdateWindow( hWnd );
	}
hOldControl = hControl;

if ( !IsWindowVisible( hWnd ) )
	return;

GetWindowRect( hControl, &rControl );
// Compute the final destination
pt.x = 0 + ((60 - RectWidth(&rControl)) / 2);
pt.y = 380 + ((100 - RectHeight(&rControl)) / 2);
MapWindowPoints( hWnd, NULL, &pt, 1 );
by = pt.y + RectHeight(&rControl);

#ifdef _MAC
SetRect( &rRect, rControl.left, rControl.top, rControl.right, by );
wsprintf( szString, 
	"SETSPEED 38\rJUMP %d,%d\rMOVE %d,%d\rJUMP %d,%d\rMOVE %d,%d\r",
// Jump x, y	
	rRect.left, 
	rRect.top, 
// Move x, y	
	rRect.left, 
	rRect.top + (pt.y - rControl.top),
// Jump x, y	
	rControl.left,
	pt.y, 
// Move x, y
	pt.x,
	pt.y
	 );

AnimateDibID( hWnd, NULL, hInstAstral, Tool.id, NULL, szString );

#else  // _MAC
	   // Windows version
SetRect( &rRect, rControl.left, rControl.top, rControl.right, by );
wsprintf( szString, "SETSPEED 38\rJUMP %d,%d\rMOVE %d,%d\r",
	0, 0, 0, pt.y - rControl.top );
AnimateDibID( hWnd, &rRect, hInstAstral, Tool.id, NULL, szString );
SetRect( &rRect, pt.x, pt.y, rControl.right, by );
wsprintf( szString, "SETSPEED 38\rJUMP %d,%d\rMOVE %d,%d\r",
	rControl.left - pt.x, 0, 0, 0 );
AnimateDibID( hWnd, &rRect, hInstAstral, Tool.id, NULL, szString );
#endif // _MAC
}

/************************************************************************/
HINSTANCE OptionLoadLibrary( HWND hDlg, ITEMID idFirst, int iCount, ITEMID idType, int iCategory, int iTotalCategories )
/************************************************************************/
{
FNAME szFileName;
char szNum[4];
HINSTANCE hOptions;
ITEMID id, idOffset, idNew;
HWND hControl;

if ( iTotalCategories )
		iCategory %= iTotalCategories;
else	iCategory = 0;

if ( !LookupExtFile( Int2Ascii(szNum,iCategory+1), szFileName, idType ) )
	hOptions = hInstAstral;
else
	{
	if ( (hOptions = LoadLibrary( szFileName )) < (HINSTANCE)HINSTANCE_ERROR )
		hOptions = hInstAstral;
	}

if ( hOptions == hInstAstral )
	idOffset = 0;
else
	idOffset = idFirst;

for ( id = idFirst; id < idFirst+iCount; id++ )
	{
	if ( !(hControl = GetDlgItem( hDlg, id )) )
		continue;
	if ( !(idNew = id - idOffset) )
		idNew = 9999;
	SetWindowWord( hControl, GWW_ICONID, idNew );
	SetWindowWord( hControl, GWW_ICONINST, (WORD)hOptions );
	if ( IsWindowVisible(hControl) )
		InvalidateRect( hControl, NULL, FALSE );
	}
return( hOptions );
}

/************************************************************************/
BOOL OptionGetString( HINSTANCE hOptions, int iTool, LPSTR lpString )
/************************************************************************/
{
*lpString = '\0';
if ( !hOptions )
	return( FALSE );
return( LoadOSString( hOptions, iTool, lpString, sizeof(STRING)-1 ) );
}


/************************************************************************/
BOOL OptionGetFileName( HINSTANCE hOptions, int iTool, LPSTR lpFileName )
/************************************************************************/
{
char szNameFormat[16];

*lpFileName = '\0';
if ( !hOptions )
	return( FALSE );
if ( !LoadOSString( hOptions, 1, szNameFormat, sizeof(szNameFormat)-1 ) )
	return( FALSE );

lstrcpy( lpFileName, Control.PouchPath );
wsprintf( lpFileName + lstrlen(lpFileName), szNameFormat, iTool );

return( TRUE );
}

/************************************************************************/
int OptionGetCategoryNum(int iCategory, int iTotalCategories, ITEMID idType)
/************************************************************************/
{
	FNAME 		szFileName;
	char 		szNum[4];
	LPSTR		lpNum;
	char		ch;
	int 		iCategoryNum;

	if (iTotalCategories)
			iCategory %= iTotalCategories;
	else	iCategory = 0;

	if (LookupExtFile(Int2Ascii(szNum,iCategory+1), szFileName, idType))
	{
		lpNum = filename (szFileName);
		while (((ch = *lpNum) != 0) && ((ch < 0x30) || (ch > 0x39)))
			lpNum++; 
		iCategoryNum = Ascii2Int(lpNum);
	}
	else
		iCategoryNum = iCategory;
		
	return iCategoryNum; 
}


