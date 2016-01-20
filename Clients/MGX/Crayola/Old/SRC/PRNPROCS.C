//®PL1¯®FD1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

//***********************************************************************
void DoPrintSizeInits( void )
//***********************************************************************
{
int depth;
LPFRAME lpFrame;

if ( !lpImage || !(lpFrame = ImgGetBaseEditFrame(lpImage)) )
	return;

if ( !Names.Printer[0] )
	GetDefaultPrinter( Names.Printer );
// Load up the print style
LoadPrintStyle( Names.PrintStyle );


/* Be careful to set all values before doing a SetDlg... with them */
/* The Edit structure is a temorary holding area for sizing dialog boxes */
Edit.Distortable = NO;
Edit.ScaleX = Edit.ScaleY = 100;
if ( lpImage )
	{
	Edit.Crop.left   = Edit.Crop.top = 0;
	Edit.Crop.right  = FrameXSize(lpFrame) - 1;
	Edit.Crop.bottom = FrameYSize(lpFrame) - 1;
#ifndef CRAYOLA
	Edit.Resolution  = Edit.CropResolution = 50;  // force a certain size
#else
	Edit.Resolution  = Edit.CropResolution = FrameResolution(lpFrame);
#endif
	Edit.Depth       = FrameDepth(lpFrame);
	if (Edit.Depth == 0) Edit.Depth = 1;
	}
else
	{
	Edit.Resolution = Edit.CropResolution = 50;
	Edit.Depth = 1;
	}
Edit.Angle       = 0;
Edit.Width       = FGET( CROP_WIDTH,  Edit.CropResolution );
Edit.Height      = FGET( CROP_HEIGHT, Edit.CropResolution );

// Stuff the Page structure
Page.Centered    = YES;
Page.OffsetX     = 0;
Page.OffsetY     = 0;
depth = FrameDepth( ImgGetBaseEditFrame(lpImage) );
if (depth == 0) depth = 1;
if (depth == 1)
		Page.OutputType = IDC_PRINT_GRAY;
else	Page.OutputType = IDC_PRINT_COLOR;
}

//***********************************************************************
BOOL WINPROC EXPORT DlgPrintSetupProc(
//***********************************************************************
HWND   hDlg,
UINT   msg,
WPARAM wParam,
LPARAM lParam)
{
BOOL Bool;
static LPPAGE lpPageBackup;

switch (msg)
	{
	case WM_INITDIALOG:
		SET_CLASS_HBRBACKGROUND(hDlg, ( HBRUSH )GetStockObject(LTGRAY_BRUSH));
		AstralCursor(IDC_WAIT);
		CenterPopup( hDlg );
		if ( !Names.Printer[0] )
			GetDefaultPrinter( Names.Printer );
		lstrcpy( Names.Saved, Names.Printer );
		InitPrinterListBox( hDlg, Names.Printer );
		CheckComboItem( hDlg, IDC_PRINTERTYPES, IDC_PRINTER_IS_MONO,
			IDC_PRINTER_IS_RGB, Page.Type );
		CheckDlgButton( hDlg, IDC_BINARY_PS, Page.BinaryPS );
		if ( lpPageBackup = (LPPAGE)Alloc( (long)sizeof(PAGE) ) )
			*lpPageBackup = Page;
		// Gray out the Postcript-only controls
		Bool = IsPostScript();
		ControlEnable( hDlg, IDC_BINARY_PS, Bool );
		AstralCursor(NULL);
	break;

	case WM_SETCURSOR:
		return( SetupCursor( wParam, lParam, IDD_PRINTSETUP ) );

	case WM_MEASUREITEM:
	case WM_DRAWITEM:
		return( OwnerDraw( hDlg, msg, lParam, NO ) );

	case WM_CTLCOLOR:
		return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
			HIWORD(lParam) ) );

	case WM_MENUSELECT:
		HintLine( wParam );
	break;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDC_PRINTSELECT:
			if ( HIWORD(lParam) == LBN_SELCHANGE )
				{
				GetPrinterListBox( hDlg, Names.Printer );
				// Gray out the Postcript-only controls
				Bool = IsPostScript();
				ControlEnable( hDlg, IDC_BINARY_PS, Bool );
				}
			if ( HIWORD(lParam) != LBN_DBLCLK )
				break;
			// else fall through to setup printer

		case IDC_PRINTSETUP:
			EnableWindow( hDlg, NO );
			SetupPrinter( hDlg, Names.Printer );
			EnableWindow( hDlg, YES );
			SetFocus( hDlg );
		break;

	    case IDC_PRINTERTYPES:
//	    case IDC_PRINTER_IS_MONO:
//	    case IDC_PRINTER_IS_CMYK:
//	    case IDC_PRINTER_IS_RGB:
			if ( !(wParam = HandleCombo( hDlg, wParam, lParam )) )
				break;
			Page.Type = wParam;
		break;

		case IDC_BINARY_PS: // Postscript only
//			Page.BinaryPS = !Page.BinaryPS;
//			CheckDlgButton( hDlg, wParam, Page.BinaryPS );
			Page.BinaryPS = (BOOL)SendDlgItemMessage (hDlg, IDC_BINARY_PS,
		  	  BM_GETCHECK, 0, 0L);
		break;

		case IDOK:
			AstralCursor(IDC_WAIT);
			LoadPrintStyle( Names.PrintStyle );
			if ( lpPageBackup )
				{
				FreeUp( (LPTR)lpPageBackup );
				lpPageBackup = NULL;
				}

			if ( !IsPostScript() &&
				( !StringsEqualID( PrintStyle.HalftoneName, IDS_NONE ) ||
				  !StringsEqualID( Separate.BGName, IDS_NONE ) ) )
			{
				AstralCursor(NULL);
				Message( IDS_EBADSTYLE );
			}

			AstralCursor(NULL);

			AstralDlgEnd( hDlg, YES|2 );
		break;

		case IDCANCEL:
			lstrcpy( Names.Printer, Names.Saved );
			LoadPrintStyle( Names.PrintStyle );
			if ( lpPageBackup )
				{
				Page = *lpPageBackup;
				FreeUp( (LPTR)lpPageBackup );
				lpPageBackup = NULL;
				}
			AstralDlgEnd( hDlg, YES|2 );
		break;

		default:
			return( FALSE );
		break;
		}
	break;

	default:
		return( FALSE );
	break;
	}

return( TRUE );
}


// Default macros
#define GetPrivateStr(K,V,STR) GetPrivateProfileString("-",K,STR,V,sizeof(V),szFileName)
#define GetPrivateInt(K,V,INT) (V=GetPrivateProfileInt("-",K,INT,szFileName))
#define GetPrivateFix(K,V,INT) { char t[40]; GetPrivateProfileString("-",K,itoa(INT,t,10),t,sizeof(t),szFileName); V=AsciiFixed(t); }
#define PutPrivateStr(K,V) WritePrivateProfileString("-",K,V,szFileName)
#define PutPrivateInt(K,V) { char t[40]; WritePrivateProfileString("-",K,itoa(V,t,10),szFileName); }
#define PutPrivateFix(K,V) { char t[40]; WritePrivateProfileString("-",K,FixedAscii(V,t,-1),szFileName); }

/***********************************************************************/
BOOL LoadPrintStyle( LPSTR lpExtName )
/***********************************************************************/
{
BOOL bRet;
LPSTR lp;
FNAME szFileName;

szFileName[0] = '\0';
if ( !(bRet = LookupExtFile( lpExtName, szFileName, IDN_STYLE )) )
	{
	if ( AstralStr( IDS_UNTITLED, &lp ) )
		lstrcpy( lpExtName, lp );
	}

GetPrivateStr( "HalftoneName",  PrintStyle.HalftoneName, "" );
GetPrivateStr( "SeparateName",  PrintStyle.SeparateName, "" );
GetPrivateStr( "CalibrateName", PrintStyle.CalibrateName, "" );

LoadHalftoneStyle( PrintStyle.HalftoneName );
LoadSeparateStyle( PrintStyle.SeparateName );
LoadCalibrateStyle( PrintStyle.CalibrateName );
return( bRet );
}


/***********************************************************************/
BOOL LoadHalftoneStyle( LPSTR lpExtName )
/***********************************************************************/
{
LPSTR lp;
FNAME szFileName;

if ( !LookupExtFile( lpExtName, szFileName, IDN_HALFTONE ) )
	{
	if ( AstralStr( IDS_NONE, &lp ) )
		lstrcpy( lpExtName, lp );
	}

if ( !(Halftone.DoHalftoning = !StringsEqualID( lpExtName, IDS_NONE )) )
	szFileName[0] = '\0';

GetPrivateInt( "Dot",     Halftone.DotShape, 0 ); Halftone.DotShape += IDC_CIRCLEDOT;
GetPrivateFix( "FreqC",   Halftone.ScreenRuling[0], 53 );
GetPrivateFix( "FreqM",   Halftone.ScreenRuling[1], 53 );
GetPrivateFix( "FreqY",   Halftone.ScreenRuling[2], 53 );
GetPrivateFix( "FreqK",   Halftone.ScreenRuling[3], 53 );
GetPrivateFix( "AngleC",  Halftone.ScreenAngle[0], 15 );
GetPrivateFix( "AngleM",  Halftone.ScreenAngle[1], 75 );
GetPrivateFix( "AngleY",  Halftone.ScreenAngle[2], 0 );
GetPrivateFix( "AngleK",  Halftone.ScreenAngle[3], 45 );
GetPrivateInt( "Hilight", Halftone.Hilight, 0 );
GetPrivateInt( "Shadow",  Halftone.Shadow, 255 );

return( Halftone.DoHalftoning );
}


/***********************************************************************/
BOOL LoadSeparateStyle( LPSTR lpExtName )
/***********************************************************************/
{
LPSTR lp;
FNAME szFileName;

if ( !LookupExtFile( lpExtName, szFileName, IDN_SEPARATE ) )
	{
	if ( AstralStr( IDS_NONE, &lp ) )
		lstrcpy( lpExtName, lp );
	}

if ( !(Separate.DoInkCorrect = !StringsEqualID( lpExtName, IDS_NONE )) )
	szFileName[0] = '\0';

GetPrivateInt( "MfromR", Separate.MfromR, 0 );
GetPrivateInt( "YfromG", Separate.YfromG, 0 );
GetPrivateInt( "CfromB", Separate.CfromB, 0 );
GetPrivateInt( "UCR",    Separate.UCR, 50 );
GetPrivateStr( "BGName", Separate.BGName, "" );

if ( !LookupExtFile( Separate.BGName, szFileName, IDN_BGMAP ) )
	{
	if ( AstralStr( IDS_NONE, &lp ) )
		lstrcpy( Separate.BGName, lp );
	}

if ( !(Separate.DoBlackAndUCR = !StringsEqualID( Separate.BGName, IDS_NONE )) )
	szFileName[0] = '\0';

ResetMap( &Separate.BGMap, MAPPOINTS, NO );
if ( szFileName[0] )
	LoadMap( &Separate.BGMap, NULL, NULL, NULL, szFileName );

return( Separate.DoInkCorrect );
}


/***********************************************************************/
BOOL LoadCalibrateStyle( LPSTR lpExtName )
/***********************************************************************/
{
LPSTR lp;
FNAME szFileName;

if ( !LookupExtFile( lpExtName, szFileName, IDN_CALMAP ) )
	{
	if ( AstralStr( IDS_NONE, &lp ) )
		lstrcpy( lpExtName, lp );
	}

if ( !(Calibrate.DoCalibration = !StringsEqualID( lpExtName, IDS_NONE )) )
	szFileName[0] = '\0';

ResetMap( &Calibrate.CalMap[0], CALPOINTS, NO );
Calibrate.CalMap[0].bStretch = YES;
ResetMap( &Calibrate.CalMap[1], CALPOINTS, NO );
Calibrate.CalMap[1].bStretch = YES;
ResetMap( &Calibrate.CalMap[2], CALPOINTS, NO );
Calibrate.CalMap[2].bStretch = YES;
ResetMap( &Calibrate.CalMap[3], CALPOINTS, NO );
Calibrate.CalMap[3].bStretch = YES;

if ( szFileName[0] )
	LoadMap( &Calibrate.CalMap[0], &Calibrate.CalMap[1],
			 &Calibrate.CalMap[2], &Calibrate.CalMap[3], szFileName );

return( Calibrate.DoCalibration );
}

