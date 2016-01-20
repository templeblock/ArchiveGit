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

WORD wDrawShapeStyle;

/***********************************************************************/
BOOL FAR PASCAL DlgTextProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
WORD wMsg;
BOOL Bool;
int i, nLength, index;
//STRING buf;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	GetFonts( hDlg );
	SendDlgItemMessage( hDlg, IDC_TEXT_FACES, CB_RESETCONTENT, 0, 0L );
	for (i=0; i<Text.FontCount; i++)
		{	/* displays available fonts */
		SendDlgItemMessage( hDlg, IDC_TEXT_FACES, CB_ADDSTRING,
			NULL, (long)(LPTR)Text.FontList[i] );
		Text.nFont = SendDlgItemMessage( hDlg, IDC_TEXT_FACES,
			CB_SETCURSEL, 0, 0L );
//		dbg("Text.FontList[%d] = %ls", i, (LPTR)Text.FontList[i]);
		}
//	GetSizes( hDlg, 0 );
//	for (i=0; i<Text.SizeCount; i++)
//		{	/* displays font sizes	*/
//		sprintf( buf, "%d", Text.SizeList[i]);
//		SendDlgItemMessage( hDlg, IDC_TEXT_SIZES, CB_ADDSTRING,
//			0, (long)(LPTR)buf );
//		Text.nSize = SendDlgItemMessage( hDlg, IDC_TEXT_SIZES,
//			CB_SETCURSEL, 0, 0L );
//		}
	SendDlgItemMessage( hDlg, IDC_TEXT_STRING, EM_LIMITTEXT,
		sizeof(Text.TextString)-1, 0L );
	InitDlgItemSpin( hDlg, IDC_TEXT_SIZE, Text.Size, NO, 2, 248 );
	SetDlgItemText( hDlg, IDC_TEXT_STRING, Text.TextString );
	CheckDlgButton( hDlg, IDC_TEXT_ITALIC, Text.Italic );
	CheckDlgButton( hDlg, IDC_TEXT_UNDERLINE, Text.Underline );
	CheckDlgButton( hDlg, IDC_TEXT_STRIKEOUT, Text.Strikeout );
	CheckDlgButton( hDlg, IDC_TEXT_BOLD, Text.Weight );
//	CheckRadioButton( hDlg, IDC_TEXT_LIGHT, IDC_TEXT_BOLD,
//		Text.Weight );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_TEXT:
		SaveToolPreferences( wParam );
		break;

	    case IDC_TEXT_STRING:
		nLength = GetDlgItemText(hDlg, wParam, Text.TextString,
			sizeof(Text.TextString) );
		Text.TextString[nLength] ='\0';
		break;

	    case IDC_TEXT_SIZE:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Text.Size = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		break;

//	    case IDC_TEXT_LIGHT:
//	    case IDC_TEXT_NORMAL:
//	    case IDC_TEXT_BOLD:
//		CheckRadioButton( hDlg, IDC_TEXT_LIGHT, IDC_TEXT_BOLD, wParam );
//		Text.Weight = wParam;
//		break;

	    case IDC_TEXT_BOLD:
		Text.Weight = !Text.Weight;
		CheckDlgButton( hDlg, wParam, Text.Weight );
		break;

	    case IDC_TEXT_ITALIC:
		Text.Italic = !Text.Italic;
		CheckDlgButton( hDlg, wParam, Text.Italic );
		break;

	    case IDC_TEXT_UNDERLINE:
		Text.Underline = !Text.Underline;
		CheckDlgButton( hDlg, wParam, Text.Underline );
		break;

	    case IDC_TEXT_STRIKEOUT:
		Text.Strikeout = !Text.Strikeout;
		CheckDlgButton( hDlg, wParam, Text.Strikeout );
		break;

	    case IDC_TEXT_FACES:
		wMsg = HIWORD(lParam);
		if ( wMsg != CBN_SELCHANGE && wMsg != CBN_DBLCLK )
			break;
//		SetDlgItemText(hDlg, IDC_TEXT_FONT, szFileName);
//		SendDlgItemMessage(hDlg, IDC_TEXT_FONT, EM_SETSEL,
//			NULL, MAKELONG(0, 0x7fff));
		Text.nFont = SendDlgItemMessage( hDlg, IDC_TEXT_FACES, CB_GETCURSEL,
			0, 0L );
//		dbg("nFont = %d face = %ls", Text.nFont,
//			(LPTR)Text.FontList[Text.nFont]);
		if ( Text.nFont == CB_ERR )
			break;
//		SendDlgItemMessage( hDlg, IDC_TEXT_SIZES, CB_RESETCONTENT,
//			0, 0L );
//		GetSizes( hDlg, Text.nFont );
//		for ( i=0; i<Text.SizeCount; i++ )
//			{
//			sprintf( buf, "%d", Text.SizeList[i] );
//			SendDlgItemMessage( hDlg, IDC_TEXT_SIZES, CB_ADDSTRING,
//				0, (long)(LPTR)buf );
//			Text.nSize = SendDlgItemMessage( hDlg, IDC_TEXT_SIZES,
//				CB_SETCURSEL, 0, 0L );
//			if ( Text.nSize == CB_ERR )
//				break;
//			}
		if ( wMsg == CBN_DBLCLK )
			SendMessage( hDlg, WM_COMMAND, IDOK, 0L );
		break;

	    case IDOK:
		Text.nFont = SendDlgItemMessage( hDlg, IDC_TEXT_FACES,
			CB_GETCURSEL, 0, 0L );
		if ( Text.nFont == CB_ERR )
			{
			Print( "No font selected" );
			break;
			}
//		CurrentFont = Text.nFont;
//		Text.nSize = SendDlgItemMessage( hDlg, IDC_TEXT_SIZES,
//			CB_GETCURSEL, 0, 0L );
//		if ( Text.nSize == CB_ERR )
//			{
//			Print( "No size selected" );
//			break;
//			}
//		CurrentSize = Text.nSize;
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgPaintProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
int Transparency;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	lstrcpy( Names.Saved, Texture.Name );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Texture.Name,
		IDN_TEXTURE );
	CheckDlgButton( hDlg, IDC_TEXTUREFLIPX, Texture.fHorzFlip );
	CheckDlgButton( hDlg, IDC_TEXTUREFLIPY, Texture.fVertFlip );
	CheckComboItem( hDlg, IDC_BRUSHSHAPE, IDC_BRUSHCIRCLE, IDC_BRUSHCUSTOM,
		Retouch.BrushShape );
	CheckComboItem( hDlg, IDC_PRESSURE, IDC_PRESSLIGHT, IDC_PRESSHEAVY,
		Retouch.Pressure );
	CheckRadioButton( hDlg, IDC_BRUSHSOLID, IDC_BRUSHSCATTER,
		Retouch.BrushStyle );
	CheckComboItem( hDlg, IDC_CIRSTYLE, IDC_CIRDRAWEDGE, IDC_CIRDRAWANDFILL,
		wDrawShapeStyle );
	CheckComboItem( hDlg, IDC_SQUSTYLE, IDC_SQUDRAWEDGE, IDC_SQUDRAWANDFILL,
		wDrawShapeStyle - IDC_CIRDRAWEDGE + IDC_SQUDRAWEDGE );
	CheckComboItem( hDlg, IDC_FRESTYLE, IDC_FREDRAWEDGE, IDC_FREDRAWANDFILL,
		wDrawShapeStyle - IDC_CIRDRAWEDGE + IDC_FREDRAWEDGE );
	InitDlgItemSpin( hDlg, IDC_FUZZY, Retouch.FuzzyPixels, NO,
		0, MAX_BRUSH_SIZE);
	Transparency = (99 * (255 - Retouch.Opacity)) / 255;
	InitDlgItemSpin( hDlg, IDC_TRANSPARENT, Transparency, NO, 0, 100 );
	InitDlgItemSpin( hDlg, IDC_BRUSHSIZE, Retouch.BrushSize, NO,
		0, MAX_BRUSH_SIZE);
	InitDlgItemSpin( hDlg, IDC_FADEOUT, Retouch.Fadeout, YES, -100, 100 );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_MEASUREITEM:
	MeasurePopupBrush( hDlg, (LPMEASUREITEMSTRUCT)lParam );
	break;

    case WM_DRAWITEM:
	DrawPopupBrush( (LPDRAWITEMSTRUCT)lParam );
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_PAINT:
	    case IDC_SPRAY:
	    case IDC_CLONE:
	    case IDC_TEXTURE:
	    case IDC_SMEAR:
	    case IDC_SHARP:
	    case IDC_SMOOTH:
	    case IDC_LIGHTEN:
	    case IDC_DARKEN:
	    case IDC_PENCIL:
	    case IDC_DRAWSQUARE:
	    case IDC_DRAWCIRCLE:
	    case IDC_DRAWFREE:
		SaveToolPreferences( wParam );
		break;

	    case IDC_PRESSURE:
//	    case IDC_PRESSLIGHT:
//	    case IDC_PRESSMEDIUM:
//	    case IDC_PRESSHEAVY:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Retouch.Pressure = wParam;
		AstralControlPaint( hDlg, IDC_BRUSHICON );
		break;

	    case IDC_BRUSHSHAPE:
//	    case IDC_BRUSHCIRCLE:
//	    case IDC_BRUSHSQUARE:
//	    case IDC_BRUSHHORIZONTAL:
//	    case IDC_BRUSHVERTICAL:
//	    case IDC_BRUSHSLASH:
//	    case IDC_BRUSHBACKSLASH:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		if ( wParam != IDC_BRUSHCUSTOM )
			{
			if (lpImage)
		    		DisplayBrush(0, 0, 0, OFF);
			Retouch.BrushShape = wParam;
		    	SetupMask();
		    	if (lpImage && Window.hCursor == Window.hNullCursor)
				DisplayBrush(lpImage->hWnd, 32767, 32767, ON);
			}
		else	{
			if ( !AstralDlg( NO, hInstAstral, hDlg, IDD_CUSTOMBRUSH,
			     DlgCustomBrushProc ) )
				break;
			if (lpImage)
		    		DisplayBrush(0, 0, 0, OFF);
			if (!SetupCustomBrushMask())
				break;
			}
		AstralControlPaint( hDlg, IDC_BRUSHICON );
		break;

	    case IDC_BRUSHSOLID:
	    case IDC_BRUSHSCATTER:
		CheckRadioButton( hDlg, IDC_BRUSHSOLID, IDC_BRUSHSCATTER,
			wParam );
		Retouch.BrushStyle = wParam;
		AstralControlPaint( hDlg, IDC_BRUSHICON );
		break;

	    case IDC_BRUSHSIZE:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		if (lpImage)
	    		DisplayBrush(0, 0, 0, OFF);
		Retouch.BrushSize = GetDlgItemSpin( hDlg, IDC_BRUSHSIZE,
			&Bool, NO );
		AstralControlPaint( hDlg, IDC_BRUSHICON );
	    	SetupMask();
	    	if (lpImage && Window.hCursor == Window.hNullCursor)
			DisplayBrush(lpImage->hWnd, 32767, 32767, ON);
		break;

	    case IDC_FUZZY:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Retouch.FuzzyPixels = GetDlgItemSpin( hDlg, IDC_FUZZY,
			&Bool, NO );
		break;

	    case IDC_TRANSPARENT:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Transparency = GetDlgItemSpin( hDlg, IDC_TRANSPARENT,
			&Bool, NO );
		Retouch.Opacity = (255 * (99 - Transparency)) / 99;
		break;

	    case IDC_FADEOUT:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Retouch.Fadeout = GetDlgItemSpin( hDlg, wParam, &Bool, YES );
		break;

	    case IDC_TEXTUREFLIPX:
		Texture.fHorzFlip = !Texture.fHorzFlip;
		CheckDlgButton( hDlg, IDC_TEXTUREFLIPX, Texture.fHorzFlip);
		break;

	    case IDC_TEXTUREFLIPY:
		Texture.fVertFlip = !Texture.fVertFlip;
		CheckDlgButton( hDlg, IDC_TEXTUREFLIPY, Texture.fVertFlip);
		break;

	    case IDC_CIRDRAWEDGE:
	    case IDC_CIRFILLINTERIOR:
	    case IDC_CIRDRAWANDFILL:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		wDrawShapeStyle = wParam;
		break;

	    case IDC_SQUDRAWEDGE:
	    case IDC_SQUFILLINTERIOR:
	    case IDC_SQUDRAWANDFILL:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		wDrawShapeStyle = wParam - IDC_SQUDRAWEDGE + IDC_CIRDRAWEDGE;
		break;

	    case IDC_FREDRAWEDGE:
	    case IDC_FREFILLINTERIOR:
	    case IDC_FREDRAWANDFILL:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		wDrawShapeStyle = wParam - IDC_FREDRAWEDGE + IDC_CIRDRAWEDGE;
		break;

	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Texture.Name,
			IDN_TEXTURE, HIWORD(lParam) );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Texture.Name,
			IDN_TEXTURE, wParam, NO );
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgFreeProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL	 Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	CheckComboItem( hDlg, IDC_MASKMETHOD, IDC_FREEHAND,
		IDC_AUTOMASK, Mask.AutoMask + IDC_FREEHAND );
	InitDlgItemSpin( hDlg, IDC_AUTOSEARCHDEPTH, Mask.AutoSearchDepth, NO,
		1, 5 );
	InitDlgItemSpin( hDlg, IDC_AUTOMINEDGE, Mask.AutoMinEdge, NO,
		1, 50 );
	ControlEnable( hDlg, IDC_AUTOSEARCHDEPTH, Mask.AutoMask );
	ControlEnable( hDlg, IDC_AUTOMINEDGE, Mask.AutoMask );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_FREE:
		SaveToolPreferences( wParam );
		break;

	    case IDC_MASKMETHOD:
//	    case IDC_FREEHAND:
//	    case IDC_AUTOMASK:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Mask.AutoMask = wParam - IDC_FREEHAND;
		ControlEnable( hDlg, IDC_AUTOSEARCHDEPTH, Mask.AutoMask );
		ControlEnable( hDlg, IDC_AUTOMINEDGE, Mask.AutoMask );
		break;

	    case IDC_AUTOSEARCHDEPTH:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Mask.AutoSearchDepth = GetDlgItemSpin( hDlg,IDC_AUTOSEARCHDEPTH,
			&Bool, NO );
		break;

	    case IDC_AUTOMINEDGE:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Mask.AutoMinEdge = GetDlgItemSpin( hDlg, IDC_AUTOMINEDGE,
			&Bool, NO );
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgShieldProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	CheckComboItem( hDlg, IDC_PROBEMETHOD, IDC_PROBEPOINT,
		IDC_PROBERECT, ColorMask.ShieldMethod );
	SetupShield();
	AstralDlg( YES, hInstAstral, hWndAstral, IDD_SHIELDFLOAT,
		DlgShieldFloatProc );
	SendMessage( AstralDlgGet(IDD_SHIELDFLOAT), WM_NCPAINT, 0, 0L );
	break;

    case WM_DESTROY:
	SendMessage( AstralDlgGet(IDD_SHIELDFLOAT), WM_NCPAINT, 0, 0L );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	SendMessage( AstralDlgGet(IDD_SHIELDFLOAT), WM_NCPAINT, 0, 0L );
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_SHIELD:
		SaveToolPreferences( wParam );
		break;

	    case IDC_PROBEMETHOD:
//	    case IDC_PROBEPOINT:
//	    case IDC_PROBERECT:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		ColorMask.ShieldMethod = wParam;
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgShieldFloatProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
HWND hWnd;
int i;
BOOL Bool, active;
BYTE mask;
long Color;
RECT rMask;

switch (msg)
    {
    case WM_INITDIALOG:
	RightPopup( hDlg );
	for (i=0; i<NUM_COLORMASKS; i++)
		{
		InitDlgItemSpin( hDlg, IDC_MASKRANGE1 + i,
			ColorMask.MaskRange[i], NO, 0, 100 );
		active = ColorMask.Mask & (1 << i);
		CheckDlgButton( hDlg, IDC_MASKACTIVE1 + i, active );
		CopyRGB(&ColorMask.MaskColor[i], &Color);
		SetWindowLong( GetDlgItem( hDlg, IDC_MASKCOLOR1+i ), 0,
			Color );
		}
	CheckRadioButton( hDlg, IDC_SHIELD1, IDC_SHIELD8,
		ColorMask.MaskShield + IDC_SHIELD1 );
	CheckComboItem( hDlg, IDC_SHIELDSELECT, IDC_MASKINCLUDE,IDC_MASKEXCLUDE,
		ColorMask.Include ? IDC_MASKINCLUDE : IDC_MASKEXCLUDE);
	SetupShield();
	UpdateStatusBar( NO, NO, NO, YES );
	ControlEnable( hDlg, IDC_PREVIEW, ColorMask.Mask );
	ControlEnable( hDlg, IDC_RESET, NO );
	break;

    case WM_NCPAINT:
	Bool = ( Control.Function == IDC_SHIELD );
	ShowControl( hDlg, IDC_SHIELDLABEL1, IDC_SHIELD8, Bool, YES );
	ShowControl( hDlg, IDC_SHIELDNUMLABEL1, IDC_SHIELDNUM8, !Bool, YES );
	ShowControl( NULL, NULL, NULL, NULL, NULL );
	return( FALSE );
	break;

    case WM_CLOSE:
	UpdateStatusBar( NO, NO, NO, YES );
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
	    case IDC_SHIELD1:
	    case IDC_SHIELD2:
	    case IDC_SHIELD3:
	    case IDC_SHIELD4:
	    case IDC_SHIELD5:
	    case IDC_SHIELD6:
	    case IDC_SHIELD7:
	    case IDC_SHIELD8:
		ColorMask.MaskShield = wParam - IDC_SHIELD1;
		CheckRadioButton( hDlg, IDC_SHIELD1, IDC_SHIELD8,
			IDC_SHIELD1 + ColorMask.MaskShield );
		break;

	    case IDC_MASKCOLOR1:
	    case IDC_MASKCOLOR2:
	    case IDC_MASKCOLOR3:
	    case IDC_MASKCOLOR4:
	    case IDC_MASKCOLOR5:
	    case IDC_MASKCOLOR6:
	    case IDC_MASKCOLOR7:
	    case IDC_MASKCOLOR8:
		i = wParam - IDC_MASKCOLOR1;
		if ( !ColorPicker( &ColorMask.MaskColor[i] ) )
			break;
		CopyRGB( &ColorMask.MaskColor[i], &Color );
		SetWindowLong( GetDlgItem( hDlg, wParam ), 0, Color );
		AstralControlRepaint( hDlg, wParam );
		SetupShield();
		break;

	    case IDC_MASKRANGE1:
	    case IDC_MASKRANGE2:
	    case IDC_MASKRANGE3:
	    case IDC_MASKRANGE4:
	    case IDC_MASKRANGE5:
	    case IDC_MASKRANGE6:
	    case IDC_MASKRANGE7:
	    case IDC_MASKRANGE8:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		ColorMask.MaskRange[wParam-IDC_MASKRANGE1] =
			GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		SetupShield();
	    	break;

	    case IDC_MASKACTIVE1:
	    case IDC_MASKACTIVE2:
	    case IDC_MASKACTIVE3:
	    case IDC_MASKACTIVE4:
	    case IDC_MASKACTIVE5:
	    case IDC_MASKACTIVE6:
	    case IDC_MASKACTIVE7:
	    case IDC_MASKACTIVE8:
		mask = 1 << (wParam - IDC_MASKACTIVE1);
		ColorMask.Mask ^= mask;
		active = ColorMask.Mask & mask;
		CheckDlgButton( hDlg, wParam, active );
		SetupShield();
		if ( active )
			ColorMask.Off = NO;
		UpdateStatusBar( NO, NO, NO, YES );
		break;

	    case IDC_SHIELDSELECT:
//	    case IDC_MASKINCLUDE:
//	    case IDC_MASKEXCLUDE:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		ColorMask.Include = ( wParam == IDC_MASKINCLUDE );
		break;

	    case IDC_PREVIEW:
		ShowShieldPreview();
		break;

	    case IDC_RESET:
		ShowShieldReset();
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgVignetteProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
int i;
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	CheckComboItem( hDlg, IDC_VIGNETTETYPE, IDC_VIGLINEAR, IDC_VIGRECTANGLE,
		Mask.VignetteType );
	InitDlgItemSpin( hDlg, IDC_VIGREPEAT, Mask.VigRepeatCount, NO,
		0, 99 );
	CheckComboItem( hDlg, IDC_VIGTRANSITION,
		IDC_VIGHARDTRANSITION, IDC_VIGSOFTTRANSITION,
		IDC_VIGHARDTRANSITION + Mask.VigSoftTransition );
	ControlEnable( hDlg, IDC_VIGTRANSITION, Mask.VigRepeatCount > 1 );
	InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Mask.Opacity), NO,
		0, 100 );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_VIGNETTE:
		SaveToolPreferences( wParam );
		break;

	    case IDC_VIGNETTETYPE:
//	    case IDC_VIGLINEAR:
//	    case IDC_VIGRADIAL:
//	    case IDC_VIGCIRCLE:
//	    case IDC_VIGELLIPSE:
//	    case IDC_VIGSQUARE:
//	    case IDC_VIGRECTANGLE:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Mask.VignetteType = wParam;
		break;

	    case IDC_VIGREPEAT:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Mask.VigRepeatCount = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		ControlEnable( hDlg, IDC_VIGTRANSITION,
			Mask.VigRepeatCount > 1 );
		break;

	    case IDC_VIGTRANSITION:
//	    case IDC_VIGSOFTTRANSITION:
//	    case IDC_VIGHARDTRANSITION:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Mask.VigSoftTransition = wParam - IDC_VIGHARDTRANSITION;
		break;

	    case IDC_TRANSPARENT:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Mask.Opacity = 255-TOGRAY(i);
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgEllipticalProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	SetDlgItemFixed( hDlg, IDC_HCONSTRAIN, Mask.ConstrainCircleHeight, NO );
	SetDlgItemFixed( hDlg, IDC_WCONSTRAIN, Mask.ConstrainCircleWidth, NO );
	CheckComboItem( hDlg, IDC_ASPECTRATIO, IDC_UNCONSTRAIN, IDC_CONSTRAIN,
		IDC_UNCONSTRAIN + Mask.ConstrainCircleAspect );
	ControlEnable( hDlg, IDC_HCONSTRAIN, Mask.ConstrainCircleAspect );
	ControlEnable( hDlg, IDC_WCONSTRAIN, Mask.ConstrainCircleAspect );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_CIRCLE:
		SaveToolPreferences( wParam );
		break;

	    case IDC_ASPECTRATIO:
//	    case IDC_UNCONSTRAIN:
//	    case IDC_CONSTRAIN:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Mask.ConstrainCircleAspect = wParam - IDC_UNCONSTRAIN;
		ControlEnable( hDlg, IDC_HCONSTRAIN,Mask.ConstrainCircleAspect);
		ControlEnable( hDlg, IDC_WCONSTRAIN,Mask.ConstrainCircleAspect);
		break;

	    case IDC_HCONSTRAIN:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Mask.ConstrainCircleHeight = GetDlgItemFixed( hDlg,
			wParam, &Bool, NO );
		break;

	    case IDC_WCONSTRAIN:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Mask.ConstrainCircleWidth = GetDlgItemFixed( hDlg,
			wParam, &Bool, NO );
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgRectangularProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	SetDlgItemFixed( hDlg, IDC_HCONSTRAIN, Mask.ConstrainRectHeight, NO );
	SetDlgItemFixed( hDlg, IDC_WCONSTRAIN, Mask.ConstrainRectWidth, NO );
	CheckComboItem( hDlg, IDC_ASPECTRATIO, IDC_UNCONSTRAIN, IDC_CONSTRAIN,
		IDC_UNCONSTRAIN + Mask.ConstrainRectAspect );
	ControlEnable( hDlg, IDC_HCONSTRAIN, Mask.ConstrainRectAspect );
	ControlEnable( hDlg, IDC_WCONSTRAIN, Mask.ConstrainRectAspect );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_SQUARE:
		SaveToolPreferences( wParam );
		break;

	    case IDC_ASPECTRATIO:
//	    case IDC_UNCONSTRAIN:
//	    case IDC_CONSTRAIN:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Mask.ConstrainRectAspect = wParam - IDC_UNCONSTRAIN;
		ControlEnable( hDlg, IDC_HCONSTRAIN, Mask.ConstrainRectAspect );
		ControlEnable( hDlg, IDC_WCONSTRAIN, Mask.ConstrainRectAspect );
		break;

	    case IDC_HCONSTRAIN:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Mask.ConstrainRectHeight = GetDlgItemFixed( hDlg,
			wParam, &Bool, NO );
		break;

	    case IDC_WCONSTRAIN:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		Mask.ConstrainRectWidth = GetDlgItemFixed( hDlg,
			wParam, &Bool, NO );
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgMagicProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	InitDlgItemSpin( hDlg, IDC_FLOODRANGE, Mask.WandRange, NO, 0, 100 );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_MAGICWAND:
		SaveToolPreferences( wParam );
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_FLOODRANGE:
		Mask.WandRange = GetDlgItemSpin( hDlg, IDC_FLOODRANGE,
			&Bool, NO );
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
BOOL FAR PASCAL DlgFloodProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	InitDlgItemSpin( hDlg, IDC_FLOODRANGE, Retouch.FloodRange, NO, 0, 100 );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_FLOOD:
		SaveToolPreferences( wParam );
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_FLOODRANGE:
		Retouch.FloodRange = GetDlgItemSpin( hDlg, IDC_FLOODRANGE,
			&Bool, NO );
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
BOOL FAR PASCAL DlgTransformerProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	CheckComboItem( hDlg, IDC_TRANSFUNCTION, IDC_MASKONLY,
		IDC_MASKPLUSIMAGE, IDC_MASKONLY + Mask.TransformImage );
	CheckComboItem( hDlg, IDC_ORIGINAL, IDC_RETAINIMAGE, IDC_REMOVEIMAGE,
		IDC_RETAINIMAGE + Mask.RemoveImage );
	ControlEnable(hDlg, IDC_ORIGINAL, Mask.TransformImage);
	CheckComboItem( hDlg, IDC_TRANSMODE, IDC_TRANSSCALE, IDC_TRANSROTATE,
		IDC_TRANSSCALE + Mask.Rotate );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_TRANSFORMER:
		SaveToolPreferences( wParam );
		break;

	    case IDC_TRANSFUNCTION:
///	    case IDC_MASKONLY:
//	    case IDC_MASKPLUSIMAGE:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Mask.TransformImage = wParam - IDC_MASKONLY;
		ControlEnable(hDlg, IDC_ORIGINAL, Mask.TransformImage);
		break;

	    case IDC_ORIGINAL:
//	    case IDC_RETAINIMAGE:
//	    case IDC_REMOVEIMAGE:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Mask.RemoveImage = wParam - IDC_RETAINIMAGE;
		break;

	    case IDC_TRANSMODE:
///	    case IDC_TRANSSCALE:
//	    case IDC_TRANSROTATE:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Mask.Rotate = wParam - IDC_TRANSSCALE;
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgPointerProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	CheckComboItem( hDlg, IDC_POINTERFUNCTION, IDC_MOVEPOINTS,
		IDC_DELPOINTS, Mask.PointerFunction );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_POINTER:
		SaveToolPreferences( wParam );
		break;

	    case IDC_POINTERFUNCTION:
//	    case IDC_MOVEPOINTS:
//	    case IDC_ADDPOINTS:
//	    case IDC_DELETEPOINTS:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Mask.PointerFunction = wParam;
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgTintFillProc( hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
int i;
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Mask.Opacity), NO,
		0, 100 );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_TINTFILL:
		SaveToolPreferences( wParam );
		break;

	    case IDC_TRANSPARENT:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Mask.Opacity = 255-TOGRAY(i);
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgTextureFillProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
int i;
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	lstrcpy( Names.Saved, Texture.Name );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Texture.Name,
		IDN_TEXTURE );
	CheckDlgButton( hDlg, IDC_TEXTUREFLIPX, Texture.fHorzFlip);
	CheckDlgButton( hDlg, IDC_TEXTUREFLIPY, Texture.fVertFlip);
	InitDlgItemSpin( hDlg, IDC_TRANSPARENT, TOPERCENT(255-Mask.Opacity), NO,
		0, 100 );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_TEXTUREFILL:
		SaveToolPreferences( wParam );
		break;

	    case IDC_TEXTUREFLIPX:
		Texture.fHorzFlip = !Texture.fHorzFlip;
		CheckDlgButton( hDlg, IDC_TEXTUREFLIPX, Texture.fHorzFlip);
		break;

	    case IDC_TEXTUREFLIPY:
		Texture.fVertFlip = !Texture.fVertFlip;
		CheckDlgButton( hDlg, IDC_TEXTUREFLIPY, Texture.fVertFlip);
		break;

	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Texture.Name,
			IDN_TEXTURE, HIWORD(lParam) );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Texture.Name,
			IDN_TEXTURE, wParam, NO );
		break;

	    case IDC_TRANSPARENT:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemSpin( hDlg, wParam, &Bool, NO );
		Mask.Opacity = 255-TOGRAY(i);
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
BOOL FAR PASCAL DlgViewProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
int x, y, res, ratio;
DWORD dXY;
FIXED fixed;
HDC hDC;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	CheckComboItem( hDlg, IDC_CUSTOMFUNCTION, IDC_ZOOMIN, IDC_ZOOMOUT,
		Control.ZoomOut + IDC_ZOOMIN );
	CheckDlgButton( hDlg, IDC_ZOOMWINDOW, Control.ZoomWindow );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_CUSTOMVIEW:
		SaveToolPreferences( wParam );
		break;

	    case IDC_CUSTOMFUNCTION:
//	    case IDC_ZOOMIN:
//	    case IDC_ZOOMOUT:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		Control.ZoomOut = wParam - IDC_ZOOMIN;
		break;

	    case IDC_ZOOMWINDOW:
		Control.ZoomWindow = !Control.ZoomWindow;
		CheckDlgButton( hDlg, IDC_ZOOMWINDOW, Control.ZoomWindow );
		break;

	    case IDC_VIEWLAST:
		if ( !lpImage )
			break;
		RevertLastView();
		break;

	    case IDC_VIEWFULL:
		SetFullClientRect( hWndAstral );
	    case IDC_VIEWALL:
		if (!lpImage)
		    break;
		SaveLastView();
		lpImage->lpDisplay->FileRect.top = 0;
		lpImage->lpDisplay->FileRect.left = 0;
		lpImage->lpDisplay->FileRect.bottom = lpImage->nlin - 1;
		lpImage->lpDisplay->FileRect.right = lpImage->npix - 1;
		lpImage->lpDisplay->FileRate = 0;
		lpImage->lpDisplay->ViewPercentage = 0;
		dXY = GetWinXY(lpImage->hWnd);
		x = ( lpImage->lpDisplay->FileRect.left 
		    + lpImage->lpDisplay->FileRect.right ) / 2;
		y = ( lpImage->lpDisplay->FileRect.top 
		    + lpImage->lpDisplay->FileRect.bottom ) / 2;
		FullViewWindow(lpImage->hWnd, LOWORD(dXY), HIWORD(dXY), x, y,
			YES);
		break;

	    case IDC_VIEW1TO1:
		if ( !lpImage )
			break;
		hDC = GetDC( hWndAstral );
		x = GetDeviceCaps( hDC, HORZRES ); // in pixels
		ReleaseDC( hWndAstral, hDC );
		fixed = FGET( UNITY, Control.ScreenWidth );
		res = FMUL( x, fixed ); // in pixels/inch
		if ( !res )
			break;
		ratio = ( 100L * res ) / lpImage->xres;
		x = ( lpImage->lpDisplay->FileRect.left 
		    + lpImage->lpDisplay->FileRect.right ) / 2;
		y = ( lpImage->lpDisplay->FileRect.top 
		    + lpImage->lpDisplay->FileRect.bottom ) / 2;
		Zoom( x, y, ratio, NO, YES );
		break;

	    case IDOK:
//		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
//		AstralDlgEnd( hDlg, FALSE );
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
BOOL FAR PASCAL DlgProbeProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	StretchPopup( hDlg );
	CheckComboItem( hDlg, IDC_PROBEMETHOD, IDC_PROBEPOINT,
		IDC_PROBERECT, ColorMask.ProbeMethod );
	AstralDlg( YES, hInstAstral, hWndAstral, IDD_PALETTE,
		DlgPaletteProc );
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

    case WM_PAINT:
	DlgOptionPaint(hDlg);
	break;

    case WM_HSCROLL:
    case WM_VSCROLL:
	HandleMiniScroll( hDlg, wParam, lParam );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_PROBE:
		SaveToolPreferences( wParam );
		break;

	    case IDC_PROBEMETHOD:
///	    case IDC_PROBEPOINT:
//	    case IDC_PROBERECT:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		wParam += SendDlgItemMessage( hDlg,wParam,CB_GETCURSEL,0,0L );
		wParam++;
		ColorMask.ProbeMethod = wParam;
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
BOOL FAR PASCAL DlgCopyToFileProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
LPFRAME lpFrame;
LPMASK lpMask;
FNAME szFileName, szMaskName;
LPTR lpString;
int idCtl, idString;
static int iMode = IDN_CLIPBOARD, iSaveMode;
static FNAME szCopyToName;
int DataType;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	lpFrame = frame_set(NULL);
	if (lpFrame && lpFrame->WriteMask)
		{
		if (RectWidth(&lpFrame->WriteMask->rMask) > MAX_BRUSH_SIZE ||
		    RectHeight(&lpFrame->WriteMask->rMask) > MAX_BRUSH_SIZE)
			{
			ControlEnable( hDlg, IDC_COPYTOBRUSH, NO );
			if (iMode == IDN_CUSTOM)
				iMode = IDN_CLIPBOARD;
			}
		}
	lstrcpy( Names.Saved, szCopyToName );
	if (iMode == IDN_TEXTURE)
		{
		idCtl = IDC_COPYTOTEXTURE;
		}
	else if (iMode == IDN_CUSTOM)
		{
		idCtl = IDC_COPYTOBRUSH;
		}
	else
		{
		idCtl = IDC_COPYTOCLIP;
		}
	AstralStr(idCtl, &lpString);
	SetDlgItemText(hDlg, IDC_COPYTONAME, lpString);
	CheckRadioButton( hDlg, IDC_COPYTOCLIP, IDC_COPYTOBRUSH,
		idCtl);
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, szCopyToName,
		iMode );
	iSaveMode = iMode;
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
		if ( !lpImage )
			break;
		if ( !(lpFrame = frame_set(NULL)) )
			break;
		if ( !(lpMask = lpFrame->WriteMask) )
			break;
		if ( LookupExtFile(szCopyToName, szFileName, iMode) )
			{
			if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
			    (LPTR)szCopyToName) == IDCANCEL)
				break;
			}
		if (!GetExtFile(szCopyToName, NULL, iMode,
		    szFileName))
			break;
		if (iMode == IDN_CLIPBOARD)
			if (!GetExtFile(szCopyToName, NULL, IDN_MASK, szMaskName))
				break;
		if (iMode == IDN_CUSTOM)
			DataType = IDC_SAVECT;
		else
			DataType = lpImage->DataType;
		if (!AstralFrameSave(szFileName, lpFrame, &lpMask->rMask,
		    DataType, IDC_SAVEBMP, NO))
			break;
		if (iMode == IDN_CLIPBOARD)
			{
			if (!WriteMask(szMaskName, lpMask, TRUE))
				{
				unlink(szFileName);
				break;
				}
			if (!AddExtFile(szCopyToName, szMaskName, IDN_MASK))
				{
				unlink(szFileName);
				unlink(szMaskName);
				break;
				}
			}
		if (!AddExtFile(szCopyToName, szFileName, iMode))
			{
			DeleteExtFile(szCopyToName, IDN_MASK, YES);
			unlink(szFileName);
			unlink(szMaskName);
			break;
			}
		if (iMode == IDN_CLIPBOARD)
			lstrcpy(Names.Clipboard, szCopyToName);
		AstralDlgEnd( hDlg, TRUE);
		break;

	    case IDCANCEL:
		lstrcpy( szCopyToName, Names.Saved );
		iMode = iSaveMode;
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			szCopyToName, iMode, HIWORD(lParam) );
		break;

	    case IDC_CLIPEXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			szCopyToName, iMode, wParam, YES );
		break;

	    case IDC_COPYTOCLIP:
	    case IDC_COPYTOTEXTURE:
	    case IDC_COPYTOBRUSH:
		if (wParam == IDC_COPYTOTEXTURE)
			iMode = IDN_TEXTURE;
		else if (wParam == IDC_COPYTOBRUSH)
			iMode = IDN_CUSTOM;
		else
			iMode = IDN_CLIPBOARD;
		AstralStr(wParam, &lpString);
		SetDlgItemText(hDlg, IDC_COPYTONAME, lpString);
		CheckRadioButton( hDlg, IDC_COPYTOCLIP, IDC_COPYTOBRUSH,
		wParam );
		InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, szCopyToName,
		iMode );
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
BOOL FAR PASCAL DlgSmoothProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
FNAME szFileName;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	lstrcpy( Names.Saved, Names.Smooth );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Smooth,
		IDN_SMOOTH );
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
		if ( !LookupExtFile( Names.Smooth, szFileName, IDN_SMOOTH ) )
			break;
		if ( !lpImage )
			break;
		if ( !CacheInit( NO, YES ))
			break;
		AstralDlgEnd( hDlg, FilterImage(szFileName) );
		break;

	    case IDCANCEL:
		lstrcpy( Names.Smooth, Names.Saved );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Smooth, IDN_SMOOTH, HIWORD(lParam) );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Smooth,
			IDN_SMOOTH, wParam, NO );
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
BOOL FAR PASCAL DlgSharpProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
FNAME szFileName;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	lstrcpy( Names.Saved, Names.Sharp );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Sharp, IDN_SHARP );
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
		if ( !LookupExtFile( Names.Sharp, szFileName, IDN_SHARP ) )
			break;
		if ( !lpImage )
			break;
		if ( !CacheInit( NO, YES ))
			break;
		AstralDlgEnd( hDlg, FilterImage(szFileName) );
		break;

	    case IDCANCEL:
		lstrcpy( Names.Sharp, Names.Saved );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Sharp, IDN_SHARP, HIWORD(lParam) );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Sharp,
			IDN_SHARP, wParam, NO );
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
BOOL FAR PASCAL DlgEdgeProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
FNAME szFileName;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	lstrcpy( Names.Saved, Names.Edge );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Edge, IDN_EDGE );
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
		if ( !LookupExtFile( Names.Edge, szFileName, IDN_EDGE ) )
			break;
		if ( !lpImage )
			break;
		if ( !CacheInit( NO, YES ))
			break;
		AstralDlgEnd( hDlg, FilterImage(szFileName) );
		break;

	    case IDCANCEL:
		lstrcpy( Names.Edge, Names.Saved );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Edge, IDN_EDGE, HIWORD(lParam) );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Edge,
			IDN_EDGE, wParam, NO );
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
BOOL FAR PASCAL DlgSpecialProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
FNAME szFileName;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	lstrcpy( Names.Saved, Names.Special );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Special,
		IDN_SPECIAL );
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
		if ( !LookupExtFile( Names.Special, szFileName, IDN_SPECIAL ) )
			break;
		if ( !lpImage )
			break;
		if ( !CacheInit( NO, YES ))
			break;
		AstralDlgEnd( hDlg, FilterImage(szFileName) );
		break;

	    case IDCANCEL:
		lstrcpy( Names.Special, Names.Saved );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Special, IDN_SPECIAL, HIWORD(lParam) );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Special,
			IDN_SPECIAL, wParam, NO );
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
BOOL FAR PASCAL DlgMaskLoadProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
FNAME szFileName;
LPFRAME lpFrame;
LPSHAPE lpShape;
LPMASK lpMask;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	lstrcpy( Names.Saved, Names.Mask );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Mask, IDN_MASK );
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

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDOK:
		if (!(lpFrame = frame_set(NULL)))
			break;
		if ( !LookupExtFile( Names.Mask, szFileName, IDN_MASK ) )
			break;
		if (!(lpShape = ReadMask(szFileName)))
			break;
		lpMask = BuildEdges(lpFrame->WriteMask, lpShape);
		mask_link(lpMask, WRITE_MASK);
		BuildMarquee();
		Marquee(YES);
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		lstrcpy( Names.Mask, Names.Saved );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Mask, IDN_MASK, HIWORD(lParam) );
		break;

	    case IDC_CLIPEXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Mask,
			IDN_MASK, wParam, NO );
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
BOOL FAR PASCAL DlgMaskSaveProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
FNAME szFileName;
LPFRAME lpFrame;
LPMASK lpMask;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	lstrcpy( Names.Saved, Names.Mask );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Mask, IDN_MASK );
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

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.Mask, IDN_MASK, HIWORD(lParam) );
		break;

	    case IDC_CLIPEXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Mask,
			IDN_MASK, wParam, NO );
		break;

	    case IDOK:
		if ( LookupExtFile(Names.Mask, szFileName, IDN_MASK) )
			{
			if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
			    (LPTR)Names.Mask) == IDCANCEL)
				break;
			}
		if ( !GetExtFile(Names.Mask, NULL, IDN_MASK, szFileName) )
			break;
		if ( !(lpFrame = frame_set(NULL)) )
			break;
		if ( !(lpMask = lpFrame->WriteMask) )
			break;
		if (!WriteMask(szFileName, lpMask, FALSE))
			break;
		if (!AddExtFile(Names.Mask, szFileName, IDN_MASK))
			{
			unlink(szFileName);
			break;
			}
		AstralDlgEnd( hDlg, TRUE );
		break;

	    case IDCANCEL:
		lstrcpy( Names.Mask, Names.Saved );
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
BOOL FAR PASCAL DlgApplyUndoProc(hDlg, msg, wParam, lParam)
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

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_APPLY:
		SendMessage(hWndAstral, WM_COMMAND, IDM_APPLY, 0L);
		AstralDlgEnd( hDlg, TRUE );
		break;
		
	    case IDC_UNDO:
		SendMessage(hWndAstral, WM_COMMAND, IDM_UNDO, 0L);
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
}


/***********************************************************************/
BOOL FAR PASCAL DlgScanPrintProc(hDlg, msg, wParam, lParam)
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

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_CALIBSCAN:
	    case IDC_CALIBPRINT:
		AstralDlgEnd( hDlg, wParam );
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
}


/***********************************************************************/
BOOL FAR PASCAL DlgCalibrateScanProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
int i;
FNAME szFileName, szExtName;
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	RightPopup( hDlg );
	lstrcpy( Names.Saved, Names.ScanMap );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.Map, IDN_SCANMAP );
	if ( !LookupExtFile( Names.ScanMap, szFileName, IDN_SCANMAP ) )
		; //break;
	if ( !LoadMap( &MasterMap, &RedMap, &GreenMap, &BlueMap,
	     szFileName ) )
		; //break;
	LoadMeasurements( hDlg );
	EnableMenuItem( GetMenu( hWndAstral ), IDM_CALIBRATE, MF_GRAYED );
	break;

    case WM_CLOSE:
	EnableMenuItem( GetMenu( hWndAstral ), IDM_CALIBRATE, MF_ENABLED );
	AstralDlgEnd( hDlg, TRUE ); // Kill the dialog, don't just hide it
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_EXTNAMES:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		GetExtName( hDlg, wParam, IDC_FILENAME, Names.ScanMap,
			IDN_SCANMAP, HIWORD(lParam) );
		if ( !LookupExtFile( Names.ScanMap, szFileName, IDN_SCANMAP ) )
			break;
		if ( !LoadMap( &MasterMap, &RedMap, &GreenMap, &BlueMap,
		     szFileName ) )
			break;
		LoadMeasurements( hDlg );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.ScanMap,
			IDN_SCANMAP, wParam, NO );
		break;

	    case IDC_RMEASURE1:
	    case IDC_RMEASURE2:
	    case IDC_RMEASURE3:
	    case IDC_RMEASURE4:
	    case IDC_RMEASURE5:
	    case IDC_RMEASURE6:
	    case IDC_RMEASURE7:
	    case IDC_RMEASURE8:
	    case IDC_RMEASURE9:
	    case IDC_RMEASURE10:
	    case IDC_RMEASURE11:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		RedMap.Pnt[wParam-IDC_RMEASURE1].x = TOGRAY(i);
		break;

	    case IDC_GMEASURE1:
	    case IDC_GMEASURE2:
	    case IDC_GMEASURE3:
	    case IDC_GMEASURE4:
	    case IDC_GMEASURE5:
	    case IDC_GMEASURE6:
	    case IDC_GMEASURE7:
	    case IDC_GMEASURE8:
	    case IDC_GMEASURE9:
	    case IDC_GMEASURE10:
	    case IDC_GMEASURE11:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		GreenMap.Pnt[wParam-IDC_GMEASURE1].x = TOGRAY(i);
		break;

	    case IDC_BMEASURE1:
	    case IDC_BMEASURE2:
	    case IDC_BMEASURE3:
	    case IDC_BMEASURE4:
	    case IDC_BMEASURE5:
	    case IDC_BMEASURE6:
	    case IDC_BMEASURE7:
	    case IDC_BMEASURE8:
	    case IDC_BMEASURE9:
	    case IDC_BMEASURE10:
	    case IDC_BMEASURE11:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		BlueMap.Pnt[wParam-IDC_BMEASURE1].x = TOGRAY(i);
		break;

	    case IDC_XMEASURE1:
	    case IDC_XMEASURE2:
	    case IDC_XMEASURE3:
	    case IDC_XMEASURE4:
	    case IDC_XMEASURE5:
	    case IDC_XMEASURE6:
	    case IDC_XMEASURE7:
	    case IDC_XMEASURE8:
	    case IDC_XMEASURE9:
	    case IDC_XMEASURE10:
	    case IDC_XMEASURE11:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		MasterMap.Pnt[wParam-IDC_XMEASURE1].x = TOGRAY(i);
		break;

	    case IDC_MEASURE:
		MeasureTestStrip();
		break;

	    case IDC_RESET:
		ResetAllMaps();
		LoadMeasurements( hDlg );
		break;

	    case IDC_SAVE:
//		szExtName[0] = '\0';
		if ( !PromptForExtName( hDlg, szExtName,szFileName,IDN_SCANMAP))
			break;
		if ( !SaveMap( &MasterMap, &RedMap, &GreenMap, &BlueMap,
		     szFileName ) )
			break;
		if ( !AddExtFile( szExtName, szFileName, IDN_SCANMAP ) )
			{
			unlink( szFileName );
			break;
			}
		lstrcpy( Names.ScanMap, szExtName );
		InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.ScanMap,
			IDN_SCANMAP );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }
}


/***********************************************************************/
BOOL FAR PASCAL DlgCalibratePrintProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
int i;
FNAME szFileName, szExtName;
BOOL Bool;

switch (msg)
    {
    case WM_INITDIALOG:
	RightPopup( hDlg );
	lstrcpy( Names.Saved, Names.PrintMap );
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.PrintMap,
		IDN_CALMAP );
	if ( !LookupExtFile( Names.PrintMap, szFileName, IDN_CALMAP ) )
		; //break;
	if ( !LoadMap( &MasterMap, &RedMap, &GreenMap, &BlueMap,
	     szFileName ) )
		; //break;
	LoadMeasurements( hDlg );
	EnableMenuItem( GetMenu( hWndAstral ), IDM_CALIBRATE, MF_GRAYED );
	break;

    case WM_CLOSE:
	EnableMenuItem( GetMenu( hWndAstral ), IDM_CALIBRATE, MF_ENABLED );
	AstralDlgEnd( hDlg, TRUE ); // Kill the dialog, don't just hide it
	break;

    case WM_CTLCOLOR:
	return( SetControlColors( (HDC)wParam, hDlg, LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_ERASEBKGND:
	EraseDialogBackground( wParam /*hDC*/, hDlg );
	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDC_EXTNAMES:
		if ( HIWORD(lParam) != CBN_SELCHANGE )
			break;
		GetExtName( hDlg, wParam, IDC_FILENAME, Names.PrintMap,
			IDN_CALMAP, HIWORD(lParam) );
		if ( !LookupExtFile( Names.PrintMap, szFileName, IDN_CALMAP ) )
			break;
		if ( !LoadMap( &MasterMap, &RedMap, &GreenMap, &BlueMap,
		     szFileName ) )
			break;
		LoadMeasurements( hDlg );
		break;

	    case IDC_EXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_ADDEXT:
	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.PrintMap, IDN_CALMAP, wParam, NO );
		break;

	    case IDC_RMEASURE1:
	    case IDC_RMEASURE2:
	    case IDC_RMEASURE3:
	    case IDC_RMEASURE4:
	    case IDC_RMEASURE5:
	    case IDC_RMEASURE6:
	    case IDC_RMEASURE7:
	    case IDC_RMEASURE8:
	    case IDC_RMEASURE9:
	    case IDC_RMEASURE10:
	    case IDC_RMEASURE11:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		RedMap.Pnt[wParam-IDC_RMEASURE1].x = TOGRAY(i);
		break;

	    case IDC_GMEASURE1:
	    case IDC_GMEASURE2:
	    case IDC_GMEASURE3:
	    case IDC_GMEASURE4:
	    case IDC_GMEASURE5:
	    case IDC_GMEASURE6:
	    case IDC_GMEASURE7:
	    case IDC_GMEASURE8:
	    case IDC_GMEASURE9:
	    case IDC_GMEASURE10:
	    case IDC_GMEASURE11:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		GreenMap.Pnt[wParam-IDC_GMEASURE1].x = TOGRAY(i);
		break;

	    case IDC_BMEASURE1:
	    case IDC_BMEASURE2:
	    case IDC_BMEASURE3:
	    case IDC_BMEASURE4:
	    case IDC_BMEASURE5:
	    case IDC_BMEASURE6:
	    case IDC_BMEASURE7:
	    case IDC_BMEASURE8:
	    case IDC_BMEASURE9:
	    case IDC_BMEASURE10:
	    case IDC_BMEASURE11:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		BlueMap.Pnt[wParam-IDC_BMEASURE1].x = TOGRAY(i);
		break;

	    case IDC_XMEASURE1:
	    case IDC_XMEASURE2:
	    case IDC_XMEASURE3:
	    case IDC_XMEASURE4:
	    case IDC_XMEASURE5:
	    case IDC_XMEASURE6:
	    case IDC_XMEASURE7:
	    case IDC_XMEASURE8:
	    case IDC_XMEASURE9:
	    case IDC_XMEASURE10:
	    case IDC_XMEASURE11:
//		if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
//			break;
		if ( HIWORD(lParam) != EN_CHANGE )
			break;
		i = GetDlgItemInt( hDlg, wParam, &Bool, NO );
		MasterMap.Pnt[wParam-IDC_XMEASURE1].x = TOGRAY(i);
		break;

	    case IDC_MEASURE:
		MeasureTestStrip();
		break;

	    case IDC_RESET:
		ResetAllMaps();
		LoadMeasurements( hDlg );
		break;

	    case IDC_SAVE:
//		szExtName[0] = '\0';
		if ( !PromptForExtName( hDlg, szExtName,szFileName,IDN_CALMAP))
			break;
		if ( !SaveMap( &MasterMap, &RedMap, &GreenMap, &BlueMap,
		     szFileName ) )
			break;
		if ( !AddExtFile( szExtName, szFileName, IDN_CALMAP ) )
			{
			unlink( szFileName );
			break;
			}
		lstrcpy( Names.PrintMap, szExtName );
		InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.PrintMap,
			IDN_CALMAP );
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }
}


/***********************************************************************/
void LoadMeasurements( hDlg )
/***********************************************************************/
HWND hDlg;
{
int i;

for ( i=0; i<MAPPOINTS; i++ )
	{
	SetDlgItemInt( hDlg, IDC_RMEASURE1+i,
		TOPERCENT( RedMap.Pnt[i].x), NO );
	SetDlgItemInt( hDlg, IDC_GMEASURE1+i,
		TOPERCENT( GreenMap.Pnt[i].x), NO );
	SetDlgItemInt( hDlg, IDC_BMEASURE1+i,
		TOPERCENT( BlueMap.Pnt[i].x), NO );
	SetDlgItemInt( hDlg, IDC_XMEASURE1+i,
		TOPERCENT( MasterMap.Pnt[i].x), NO );
	}
}


/***********************************************************************/
BOOL FAR PASCAL DlgCustomBrushProc(hDlg, msg, wParam, lParam)
/***********************************************************************/
HWND	 hDlg;
unsigned msg;
WORD	 wParam;
long	 lParam;
{
BOOL Bool;
FNAME szFileName;
LPFRAME lpFrame, lpOldFrame;
LPTR lpLine;
long black;
int y, bpp, type, dx, dy;
RGBS blackRGB;
static BOOL fEditBrush;

switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
	lstrcpy( Names.Saved, Retouch.CustomBrushName);
	InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Retouch.CustomBrushName,
		IDN_CUSTOM );
	CheckDlgButton( hDlg, IDC_EDIT_BRUSH, fEditBrush = FALSE);
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

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	switch (wParam)
	    {
	    case IDOK:
		if ( !LookupExtFile( Retouch.CustomBrushName, szFileName,
		     IDN_CUSTOM ) )
			{
			if (AstralOKCancel(IDS_NEWBRUSH,
			    (LPTR)Retouch.CustomBrushName) != IDOK)
				break;
			if (!(lpFrame = frame_open(1, Retouch.BrushSize,
			    Retouch.BrushSize, 100)))
				break;
			blackRGB.red = blackRGB.green = blackRGB.blue = 0;
			lpOldFrame = frame_set(lpFrame);
			black = frame_tocache(&blackRGB);
			for (y = 0; y < lpFrame->Ysize; ++y)
			    {
			    if ((lpLine = frame_ptr(0, 0, y, YES)))
				frame_setpixels(lpLine, lpFrame->Xsize, black);
			    }
			frame_set(lpOldFrame);
			if (!GetExtFile(Retouch.CustomBrushName, NULL,
			    IDN_CUSTOM, szFileName))
				{
				frame_close(lpFrame);
				break;
				}
			if (!AstralFrameSave(szFileName, lpFrame, NULL,
			    IDC_SAVECT, IDC_SAVEBMP, NO))
				{
				frame_close(lpFrame);
				break;
				}
			if (!AddExtFile(Retouch.CustomBrushName, szFileName,
			    IDN_CUSTOM))
				{
				frame_close(lpFrame);
				unlink(szFileName);
				break;
				}
			InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
				Retouch.CustomBrushName, IDN_CUSTOM );
			}
		else
		if (!(lpFrame = AstralFrameLoad(szFileName, 1, &bpp, &type)))
			break;

		if ( lpFrame->Ysize > MAX_BRUSH_SIZE ||
		     lpFrame->Xsize > MAX_BRUSH_SIZE ||
		     lpFrame->Ysize != lpFrame->Xsize )
			{
			frame_close(lpFrame);
			Print("Invalid size image for a custom brush");
			break;
			}
		AstralDlgEnd( hDlg, TRUE );
		if (fEditBrush)
			NewImageWindow( NULL, szFileName, lpFrame, IDC_SAVEBMP,
				NO, NO, IMG_BRUSH, Retouch.CustomBrushName );
		else	frame_close(lpFrame);
		break;

	    case IDCANCEL:
		lstrcpy( Retouch.CustomBrushName, Names.Saved );
		AstralDlgEnd( hDlg, FALSE );
		break;

	    case IDC_EXTNAMES:
		GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Retouch.CustomBrushName, IDN_CUSTOM, HIWORD(lParam) );
		break;

	    case IDC_CLIPEXTMANAGE:
		PopupMenu( hDlg, wParam, IDC_FILENAME );
		break;

	    case IDC_DELETEEXT:
	    case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Retouch.CustomBrushName, IDN_CUSTOM, wParam, NO );
		break;

	    case IDC_EDIT_BRUSH:
		fEditBrush = !fEditBrush;
		CheckDlgButton( hDlg, IDC_EDIT_BRUSH, fEditBrush);
		break;

	    default:
		return( FALSE );
	    }

    default:
	return( FALSE );
    }

return( TRUE );
}
