//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern HINSTANCE hInstAstral;
extern HWND hWndAstral;

static int iFontOffset;
static BOOL bTextBegun;

/***********************************************************************/
BOOL WINPROC EXPORT DlgTextProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
int i;
WORD wKey;
HFONT hFont;
STRING szString;

switch (msg)
    {
    case WM_INITDIALOG:
	RibbonInit( hDlg );
	ColorInit( hDlg );
	GetFonts( hDlg );
	CheckRadioButton( hDlg, IDC_TEXT_T1, IDC_TEXT_T10, Text.nFont+IDC_TEXT_T1 );
	Text.Size = 48;
	InitSlide( hDlg, IDC_TEXT_SIZE, Text.Size, 24, 200 );
	Text.Angle = 0;
	InitSlide( hDlg, IDC_TEXT_ANGLE, 0, -2, 2 );
	ControlEnable(hDlg, IDC_TEXT_ANGLE, TextCanRotate(hDlg, Text.nFont));
	for ( i=0; i<max(10,Text.nFont); i++ )
		{
		hFont = CreateFont(
				/* Height */ 45,
				/* Width */ 0,
				/* Escapement */ 0,
				/* Orientation */ 0,
				/* Weight */ FW_NORMAL,
				/* Italic */ FALSE,
				/* UnderLine */ FALSE,
				/* StrikeOut */ FALSE,
				/* CharSet */ Text.lpFontList[i].fdCharSet,
				/* Output Precis */ OUT_CHARACTER_PRECIS,
				/* Clip Precis */ CLIP_DEFAULT_PRECIS,
				/* Quality */ PROOF_QUALITY,
				/* PitchAndFamily */ Text.lpFontList[i].fdPitchAndFamily,
				/* Face */ Text.lpFontList[i].fdFaceName );
		if ( hFont )
			SendDlgItemMessage( hDlg, IDC_TEXT_T1+i, WM_SETFONT, (WPARAM)hFont, 0L );
		}
	if ( lpImage )
		{
		bTextBegun = TextBegin( lpImage->hWnd, 10/*x*/, 40/*y*/ );
		Tool.bActive = YES;
		SetFocus( lpImage->hWnd );
		}
	else
		bTextBegun = NO;
	// fall thru

    case WM_CONTROLENABLE:
    case WM_DOCACTIVATED:
	break;

	case WM_DESTROY:
	if ( bTextBegun )
		{
		if ( lpImage )
			{
			TextEnd( lpImage->hWnd, NO/*fCancel*/ );
			Tool.bActive = NO;
			AstralUpdateWindow( lpImage->hWnd );
			}
		bTextBegun = NO;
		}
	for ( i=0; i<max(10,Text.nFont); i++ )
		{
		if ( hFont = (HFONT)SendDlgItemMessage( hDlg, IDC_TEXT_T1+i, WM_GETFONT, (WPARAM)0, 0L ) )
			DeleteObject( hFont );
		}
	if ( Text.lpFontList )
		{
		FreeUp((LPTR)Text.lpFontList);
		Text.lpFontList = NULL;
		}
	break;

    case WM_CHAR:
	if ( !lpImage || !bTextBegun )
		return( FALSE );
	TextSetFocus();
	wKey = (WORD)lParam;
	TextKeystroke( lpImage->hWnd, wKey );
	if ( wKey == VK_RETURN )
		{
		TextAnchor( lpImage->hWnd );
		RandomizeActiveColor();
		break;
		}
	szString[0] = '_';
	szString[1] = wKey;
	szString[2] = '\0';
	Lowercase( szString );
	SoundStartResource( szString, NO/*bLoop*/, NULL/*hInstance*/ );
	break;

    case WM_KEYDOWN:
	if ( !lpImage || !bTextBegun )
		return( FALSE );
	wKey = lParam;
	switch (wKey)
		{
		case VK_UP:
			TextRMove( lpImage->hWnd, 0, -1 );
			break;
		case VK_DOWN:
			TextRMove( lpImage->hWnd, 0, 1);
			break;
		case VK_LEFT:
			TextRMove( lpImage->hWnd, -1, 0);
			break;
		case VK_RIGHT:
			TextRMove( lpImage->hWnd, 1, 0);
			break;
		default:
			break;
		}
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_TEXT ) );

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
	return( OwnerDraw( hDlg, msg, lParam, NO ) );

    case WM_CTLCOLOR:
// We don't get this message
//	Print( "%u", HIWORD(lParam) );
//	if ( HIWORD(lParam) == CTLCOLOR_BTN )
//		{
//		SetTextColor( (HDC)wParam, RGB(0,0,255) ); // Blue text...
//		SetBkColor( (HDC)wParam, RGB(255,255,255) ); // on a white background
//		return( GetStockObject(WHITE_BRUSH) );
//		}
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	switch (wParam)
	    {
		case IDC_ACTIVECOLOR:
		case IDC_COLORS_TEXT:
		ColorCommand( hDlg, wParam, lParam );
		TextSetFocus();
		if (bTextBegun)
			TextFontChanged(lpImage->hWnd);
		break;

		case IDC_TEXT_T1:
		case IDC_TEXT_T2:
		case IDC_TEXT_T3:
		case IDC_TEXT_T4:
		case IDC_TEXT_T5:
		case IDC_TEXT_T6:
		case IDC_TEXT_T7:
		case IDC_TEXT_T8:
		case IDC_TEXT_T9:
		case IDC_TEXT_T10:
		CheckRadioButton( hDlg, IDC_TEXT_T1, IDC_TEXT_T10, wParam );
		Text.nFont = wParam - IDC_TEXT_T1;
		ControlEnable(hDlg, IDC_TEXT_ANGLE, TextCanRotate(hDlg, Text.nFont));
		TextSetFocus();
		if (bTextBegun)
			TextFontChanged(lpImage->hWnd);
		break;

	    case IDC_TEXT_SIZE:
		Text.Size = HandleSlide( hDlg, (ITEMID)wParam, (UINT)lParam, NULL );
		Text.Size = bound( Text.Size, 16, 216 );
		if ( !LBUTTON )
			TextSetFocus();
		if (bTextBegun)
			TextFontChanged(lpImage->hWnd);
		break;

	    case IDC_TEXT_ANGLE:
		i = HandleSlide( hDlg, (ITEMID)wParam, (UINT)lParam, NULL ); // -2 to 2
		Text.Angle = (i * 45);
		if ( Text.Angle < 0 )
			Text.Angle += 360;
		i = (Text.Angle + 22) / 45; // down to 0-8
		if ( i > 4 )
				i = bound( i - 8, -2, 0 );
		else	i = bound( i, 0, 2 );
		SetSlide( hDlg, wParam, i );
		if ( !LBUTTON )
			TextSetFocus();
		if (bTextBegun)
			TextFontChanged(lpImage->hWnd);
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
int TextProc(
/************************************************************************/
HWND 	hWindow,
LPARAM 	lParam,
UINT 	msg)
{
int x, y;
BOOL fCancel = NO;
WORD wKey;
STRING szString;
static BOOL bTrackObject;
static int xMove, yMove;

if ( bTrackObject )
	{
	ToyProc( hWindow, lParam, msg );
	bTrackObject = Tool.bActive;
	Tool.bActive = YES;
	return( TRUE );
	}

x = LOWORD( lParam );
y = HIWORD( lParam );
switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	Tool.bActive = YES;
	break; // Never get sent since Tool.bActive is already set in the dialog

    case WM_LBUTTONDOWN:
	// Call the sticker mover tool first to see if he wants to take over
	Tool.bActive = NO;
	ToyProc( hWindow, lParam, WM_CREATE );
	if ( bTrackObject = Tool.bActive )
		{
		ToyProc( hWindow, lParam, msg );
		break;
		}
	if ( bTextBegun )
		Tool.bActive = YES;
	xMove = x;
	yMove = y;
//	TextMove(hWindow, x, y);
	break;

    case WM_LBUTTONUP:
	break;

	case WM_SETCURSOR:
	return( SetToolCursor( hWindow, ID_MOVE_OBJECT, ID_PLAY_OBJECT ) );

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	if ( Tool.bActive && Window.fLButtonDown )
		TextOffset(hWindow, x - xMove, y - yMove);
	xMove = x;
	yMove = y;
	break;

    case WM_KILLFOCUS:
	break;

    case WM_SETFOCUS:
//	TextDisplayCaret( hWindow, NULL );
	break;

    case WM_CHAR:
	wKey = (WORD)lParam;
	TextKeystroke(hWindow, wKey);
	if ( wKey == VK_RETURN )
		{
		TextAnchor( hWindow );
		RandomizeActiveColor();
		break;
		}
	szString[0] = '_';
	szString[1] = wKey;
	szString[2] = '\0';
	Lowercase( szString );
	SoundStartResource( szString, NO/*bLoop*/, NULL/*hInstance*/ );
	break;

    case WM_KEYDOWN:
	wKey = lParam;
	switch (wKey)
		{
		case VK_UP:
			TextRMove(hWindow, 0, -1);
			break;
		case VK_DOWN:
			TextRMove(hWindow, 0, 1);
			break;
		case VK_LEFT:
			TextRMove(hWindow, -1, 0);
			break;
		case VK_RIGHT:
			TextRMove(hWindow, 1, 0);
			break;
		default:
			break;
		}
	break;

    case WM_DESTROY:	// The cancel and DeactivateTool() message
	fCancel = (lParam == 1L);
	if ( fCancel )
		{ // an "escape" cancel
		TextKeystroke( hWindow, VK_ESCAPE );
		}
	else
		{ // a deactivate
		TextKeystroke( hWindow, VK_RETURN );
		TextAnchor( hWindow );
		RandomizeActiveColor();
		HideCaret( hWindow );
		DestroyCaret();
		AstralUpdateWindow( hWindow );
		}
	break;

    case WM_LBUTTONDBLCLK:
	break;
    }
return(TRUE);
}


/************************************************************************/
static void TextSetFocus( void )
/************************************************************************/
{
// This tool should always try to give the image window focus,
// so the kid can type at any time
if ( lpImage && (GetFocus() != lpImage->hWnd) )
	SetFocus( lpImage->hWnd );
}
