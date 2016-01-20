//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern	HWND		hWndAstral;
extern	HWND		hClientAstral;
extern	HINSTANCE	hInstAstral;
extern	HINSTANCE	hCurrentDLL;

#define TOOL_YPOS		380
#define TOOL_XPOS		0

/***********************************************************************/
BOOL WINPROC EXPORT DlgMainProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
RECT rApp, rMain;
int x, y, flag;

switch (msg)
    {
    case WM_INITDIALOG:
	RepositionDialogControls(hDlg);
	RepositionDialogBox(hDlg);
	DisableSavebits (hDlg);
	// Position the window in the center of the application window
	GetClientRect( hWndAstral, &rApp );
	GetWindowRect( hDlg, &rMain );
	x = (RectWidth (&rApp)  - 640) / 2;
	y = (RectHeight (&rApp) - 480) / 2;
	flag = ( IsWindowVisible(hDlg) ? 0: SWP_NOREDRAW );
	SetWindowPos( hDlg, NULL, x, y, 0, 0, // New location
		flag | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
	LayoutMDIClient( hDlg );
	PaletteInit( hDlg );

	PostMessage( hDlg, WM_COMMAND, IDC_CRAYON, 0L );
	break;

    case WM_DESTROY:
	PaletteClose();
	break;

	case WM_QUERYNEWPALETTE:
	return(HandleQueryNewPalette(hDlg, NO, NO));

	case WM_PALETTECHANGED:
	if (IsOurWindow((HWND)wParam))
		InvalidateRect( hDlg, NULL, FALSE );
//		SendMessageToChildren(hDlg, msg, wParam, lParam);
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_MAIN ) );

    case WM_ERASEBKGND:
	break; // handle ERASEBKGND and do nothing; PAINT covers everything

    case WM_PAINT:
	LayoutPaintMain( hDlg );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE|2 );
	break;

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_MENUSELECT:
	HintLine( wParam );
	break;

    case WM_COMMAND:
	switch (wParam)
	    {
	    case IDC_CRAYON:
	    case IDC_MARKER:
	    case IDC_PAINT:
	    case IDC_FILL:
	    case IDC_SHAPES:
	    case IDC_LINES:
	    case IDC_TEXT:
	    case IDC_TRANSFORMER:
	    case IDC_STAMP:
	    case IDC_ERASER:
	    case IDC_TOY:
	    case IDC_EFFECTS:
	    case IDC_GALLERY:
		ActivateTool( wParam );
		break;

	    case IDC_CUSTOMVIEW:
		if ( !lpImage )
			break;
		if ( lpImage->lpDisplay->ViewPercentage != 100 )
			{
			ViewAll();
			break;
			}
		ActivateTool( wParam );
		break;

		case IDC_GOBACK:
		StopAnimation();
		DeactivateTool();
	    if (idCurrentRoom)
		{ // if somewhere to go back to...
			// if coming from a DLL room
			if (idCurrentRoom < RM_FIRST || idCurrentRoom > RM_LAST)
			{
				GoRoom (hCurrentDLL, idCurrentRoom, TRUE);
	        	break;
			}
	        GoRoom (hInstAstral, -1, FALSE);
	        break;
		}
		return( DoCommand( hDlg, IDM_EXIT, lParam ) );

		case IDM_UNDO:
		DeactivateTool();
		DoCommand( hDlg, wParam, lParam );
		break;

		default:
		DeactivateTool();
		DoCommand( hDlg, wParam, lParam );
	    }

	// Give the focus back to the image (for text especially)
	if ( lpImage && (GetFocus() != lpImage->hWnd) )
		SetFocus( lpImage->hWnd );
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
void ColorInit( HWND hDlg )
/***********************************************************************/
{
COLOR ColorActive, ColorAlternate;
RGBS rgb;
HWND hControl;
static BOOL bInit;
// Set the active and alternate colors
if ( !bInit )
	{
	rgb.red = rgb.green = rgb.blue = 0; // the random color indicator
	SetActiveRGB( &rgb, NO );
	rgb.red = 255; rgb.green = 0; rgb.blue = 0; // red
	SetAlternateRGB( &rgb, NO );
	bInit = YES;
	}
if ( !(hControl = GetDlgItem( hDlg, IDC_ACTIVECOLOR )) )
	return;
GetActiveRGB( &rgb ); // also sets the random color
CopyRGB( &rgb, &ColorActive );
SetWindowLong( hControl, GWL_ACTIVE, ColorActive );
GetAlternateRGB( &rgb );
CopyRGB( &rgb, &ColorAlternate );
SetWindowLong( hControl, GWL_ALTERNATE, ColorAlternate );
}

/***********************************************************************/
void ColorCommand( HWND hDlg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
POINT pt;
RECT rect;
//COLORINFO ColorInfo, AltColorInfo;
//COLORSPACE ColorSpace;
COLOR Color;
RGBS rgb;
HWND hWnd;
int i, rows, cols;
static int iStartColor;

switch ( wParam )
	{
    case IDC_ACTIVECOLOR:
	// The "2" message is sent from the probe and the palette to
	// force an update; they leave the color in the active RGB
	if ( lParam == 2 )
		{
		GetActiveRGB( &rgb );
		CopyRGB( &rgb, &Color );
		SetWindowLong( GetDlgItem( hDlg, wParam ), GWL_ACTIVE, Color );
		GetAlternateRGB( &rgb );
		CopyRGB( &rgb, &Color );
		SetWindowLong( GetDlgItem( hDlg, wParam ), GWL_ALTERNATE, Color );
		AstralControlRepaint( hDlg, wParam );
		break;
		}

	// The "4" message is sent from the palette to force an
	// update; they leave the color in the alternate rgb
	if ( lParam == 4 )
		{
		GetAlternateRGB( &rgb );
		CopyRGB( &rgb, &Color );
		SetWindowLong( GetDlgItem( hDlg, wParam ), GWL_ALTERNATE, Color );
		AstralControlRepaint( hDlg, wParam );
		break;
		}
	SoundStartResource( "color", NO/*bLoop*/, NULL/*hInstance*/ );
	break;

	case IDC_COLORS_CRAYON:
	case IDC_COLORS_MARKER:
    //***************************************************************//**
    // Code similar to the following is also used for color hints    //**
    // See also ASTRAL.C SetupCursor() [mpd]                         //**
    //***************************************************************//**
	hWnd = GetDlgItem( hDlg, wParam );                               //**
	GetWindowRect( hWnd, &rect );                                    //**
	GetCursorPos( &pt );                                             //**
	pt.x -= rect.left;                                               //**
	pt.y -= rect.top;                                                //**
	OffsetRect( &rect, -rect.left, -rect.top );                      //**
	#ifdef STUDIO                                                    //**
		rows = 2; cols = 12;	// 2 x 12 color layout               //**
	#else                                                            //**
		rows = 1; cols = 16;	// 1 x 16 color layout               //**
	#endif                                                           //**
    if (pt.x >= rect.right/cols*cols) // compensate for round-off err
        pt.x = rect.right/cols*cols - 1;
    if (pt.y >= rect.bottom/rows*rows)
        pt.y = rect.bottom/rows*rows - 1;
	i  = (pt.y / (rect.bottom/rows));                                //**
	i *= cols;                                                       //**
	i += (pt.x / (rect.right/cols));                                 //**
	#ifndef STUDIO                                                   //**
	// remap the index                                               //**
		if ( i == 10 )	i = 11;		else                             //**
		if ( i == 11 )	i = 23;		else                             //**
		if ( i == 12 )	i = 10;		else                             //**
		if ( i == 13 )	i = 22;		else                             //**
		if ( i == 14 )	i = 13;		else                             //**
		if ( i == 15 )	i = 12;                                      //**
	#endif                                                           //**
	PaletteSelect( iStartColor + i, SHIFT );                         //**
	break;                                                           //**
                                                                     //**
	case IDC_COLORS_PAINT:                                           //**
	case IDC_COLORS_TEXT:                                            //**
	case IDC_COLORS_LINES:                                           //**
	case IDC_COLORS_FILL:                                            //**
	case IDC_COLORS_SHAPES:                                          //**
	hWnd = GetDlgItem( hDlg, wParam );                               //**
	GetWindowRect( hWnd, &rect );                                    //**
	GetCursorPos( &pt );                                             //**
	pt.x -= rect.left;                                               //**
	pt.y -= rect.top;                                                //**
	OffsetRect( &rect, -rect.left, -rect.top );                      //**
	#ifdef STUDIO                                                    //**
		rows = 3; cols = 8;		// 3 x 8 color layout                //**
	#else                                                            //**
		rows = 2; cols = 8;		// 2 x 8 color layout                //**
	#endif                                                           //**
    if (pt.x >= rect.right/cols*cols) // compensate for round-off err
        pt.x = rect.right/cols*cols - 1;
    if (pt.y >= rect.bottom/rows*rows)
        pt.y = rect.bottom/rows*rows - 1;
	i  = (pt.y / (rect.bottom/rows));                                //**
	i *=cols;                                                        //**
	i += (pt.x / (rect.right/cols));                                 //**
	// remap the index                                               //**
	if ( i == 1 )	i = 12;		else                                 //**
	if ( i == 2 )	i = 13;		else                                 //**
	if ( i == 3 )	i = 1;		else                                 //**
	if ( i == 4 )	i = 22;		else                                 //**
	if ( i == 5 )	i = 10;		else                                 //**
	if ( i == 6 )	i = 23;		else                                 //**
	if ( i == 7 )	i = 11;		else                                 //**
	if ( i >= 8 && i <= 15 ) i -= 6; else // 2nd row, the primaries  //**
	if ( i >= 16 && i <= 23 ) i -= 2; // 3rd row, the secondaries    //**
	PaletteSelect( iStartColor + i, SHIFT );                         //**
	break;                                                           //**
    //***************************************************************//**
    //***************************************************************//**
	}
}


/***********************************************************************/
static void LayoutMDIClient( HWND hWindow )
/***********************************************************************/
{
CLIENTCREATESTRUCT ccs;
int x, y, dx, dy;
LPSTR lp;

// Create the MDI client based on a pre-defined size
if ( AstralStr( IDS_POS_CLIENT, &lp ) )
	AsciiToInt4( lp, &x, &y, &dx, &dy );
else
	{
	x = 10; y = 10; dx = 480; dy = 360;
	}

if ( hClientAstral )
	{
	SetWindowPos( hClientAstral, NULL, x, y, 0, 0, // New location
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
	}
else
	{
	// Find window menu where the MDI documents will be listed
	ccs.hWindowMenu = NULL; // Don't want MDI client names in the menu
	ccs.idFirstChild = NULL;
//	ccs.hWindowMenu = AstralGetSubMenu(GetMenu(hWndAstral), IDM_FILEMENU);
//	ccs.idFirstChild = IDM_WINDOWCHILD;
	hClientAstral = CreateWindow ("mdiclient",
		NULL,
		// the MDIS_ALLCHILDSTYLES style allows us to override
		// the default MDI child window styles imposed by Windows.
		MDIS_ALLCHILDSTYLES |
		WS_CHILD | WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS,
		x, y, dx, dy,
		hWindow,
		(HMENU)0xCAC,
		hInstAstral,
		(LPSTR)&ccs);
	ShowWindow( hClientAstral, SW_SHOW );
	}
}


/***********************************************************************/
static void LayoutPaintMain( HWND hWindow )
/***********************************************************************/
{
HDC hDC;
RECT rPaint, rScreen;
PAINTSTRUCT ps;
RGBS rgb;
#ifdef WIN32		
		bool bSuperBlt = FALSE;
#else
		bool bSuperBlt = TRUE;
#endif

#define SETRGB(rgb,r,g,b) { rgb.red = r; rgb.green = g; rgb.blue = b; }

HintLine (0);
hDC = BeginPaint( hWindow, &ps );

// Does it intersect the tool bar
SetRect( &rScreen, 500, 0, 640, 380 );
if ( IntersectRect( &rPaint, &ps.rcPaint, &rScreen ) )
	{
	#ifdef STUDIO
		SETRGB( rgb, 153, 102, 255 );
	#else
		SETRGB( rgb, 255, 204, 51 );
	#endif
	DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt/*bSuperBlt*/ );
	}

// Does it intersect the option bar
SetRect( &rScreen, 0, 380, 640, 480 );
if ( IntersectRect( &rPaint, &ps.rcPaint, &rScreen ) )
	{
	#ifdef STUDIO
		SETRGB( rgb, 102, 153, 255 );
	#else
		SETRGB( rgb, 0, 153, 51 );
	#endif
	DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt/*bSuperBlt*/ );
	}


// Does it intersect the cavas
SetRect( &rScreen, 0, 0, 500, 380 );
if ( IntersectRect( &rPaint, &ps.rcPaint, &rScreen ) )
	{
	SETRGB( rgb, 204, 204, 204 );
	DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt/*bSuperBlt*/ );

	// Does it intersect the top border
	SetRect( &rScreen, 0, 0, 500, 10 );
	if ( IntersectRect( &rPaint, &ps.rcPaint, &rScreen ) )
		{
		SetRect( &rPaint, 0, 0, 500, 1 );
		SETRGB( rgb,   0,  51, 153 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right--; rPaint.bottom++;
		SETRGB( rgb,  51, 102, 204 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right--; rPaint.bottom++;
		SETRGB( rgb, 102, 153, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right--; rPaint.bottom++;
		SETRGB( rgb, 153, 204, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right--; rPaint.bottom++;
		SETRGB( rgb, 204, 255, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right--; rPaint.bottom++;
		SETRGB( rgb, 255, 204, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right--; rPaint.bottom++;
		SETRGB( rgb, 204, 153, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right--; rPaint.bottom++;
		SETRGB( rgb, 153, 102, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right--; rPaint.bottom++;
		SETRGB( rgb, 102,  51, 204 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right--; rPaint.bottom++;
		SETRGB( rgb,  51,   0, 151 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		}

	// Does it intersect the left border
	SetRect( &rScreen, 0, 0, 10, 380 );
	if ( IntersectRect( &rPaint, &ps.rcPaint, &rScreen ) )
		{
		SetRect( &rPaint, 0, 0, 1, 380 );
		SETRGB( rgb,   0,  51, 153 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right++; rPaint.bottom--;
		SETRGB( rgb,  51, 102, 204 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right++; rPaint.bottom--;
		SETRGB( rgb, 102, 153, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right++; rPaint.bottom--;
		SETRGB( rgb, 153, 204, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right++; rPaint.bottom--;
		SETRGB( rgb, 204, 255, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right++; rPaint.bottom--;
		SETRGB( rgb, 255, 204, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right++; rPaint.bottom--;
		SETRGB( rgb, 204, 153, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right++; rPaint.bottom--;
		SETRGB( rgb, 153, 102, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right++; rPaint.bottom--;
		SETRGB( rgb, 102,  51, 204 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top++; rPaint.right++; rPaint.bottom--;
		SETRGB( rgb,  51,   0, 151 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		}

	// Does it intersect the right border
	SetRect( &rScreen, 490, 0, 500, 380 );
	if ( IntersectRect( &rPaint, &ps.rcPaint, &rScreen ) )
		{
		SetRect( &rPaint, 499, 0, 500, 380 );
		SETRGB( rgb,   0,  51, 153 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left--; rPaint.top++; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb,  51, 102, 204 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left--; rPaint.top++; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 102, 153, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left--; rPaint.top++; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 153, 204, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left--; rPaint.top++; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 204, 255, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left--; rPaint.top++; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 255, 204, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left--; rPaint.top++; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 204, 153, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left--; rPaint.top++; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 153, 102, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left--; rPaint.top++; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 102,  51, 204 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left--; rPaint.top++; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb,  51,   0, 151 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		}

	// Does it intersect the bottom border
	SetRect( &rScreen, 0, 370, 500, 380 );
	if ( IntersectRect( &rPaint, &ps.rcPaint, &rScreen ) )
		{
		SetRect( &rPaint, 0, 379, 500, 380 );
		SETRGB( rgb,   0,  51, 153 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top--; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb,  51, 102, 204 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top--; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 102, 153, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top--; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 153, 204, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top--; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 204, 255, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top--; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 255, 204, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top--; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 204, 153, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top--; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 153, 102, 255 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top--; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb, 102,  51, 204 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		rPaint.left++; rPaint.top--; rPaint.right--; rPaint.bottom--;
		SETRGB( rgb,  51,   0, 151 ); DrawColorPatch( hDC, &rPaint, &rgb, bSuperBlt );
		}
	}

EndPaint( hWindow, &ps );
}


/***********************************************************************/
void LayoutPaint( HWND hWindow )
/***********************************************************************/
{
PAINTSTRUCT ps;

HintLine (0);
BeginPaint( hWindow, &ps );
EndPaint( hWindow, &ps );
}


/***********************************************************************/
void RibbonInit( HWND hDlg )
/***********************************************************************/
{
// These functions are used to convert pixels back to dlg units.
// The reason for this is that we want to type PIXEL coordinates into the RC
// file.  Windows converts these to pixels, we convert them back to the
// values typed into the RC statement.

	RepositionDialogBox(hDlg);
	RepositionDialogControls(hDlg);
	SetWindowPos( hDlg, 0, TOOL_XPOS, TOOL_YPOS, 0, 0,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE );
}

