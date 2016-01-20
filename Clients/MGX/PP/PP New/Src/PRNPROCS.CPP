//®PL1¯®FD1¯®TP0¯®BT0¯®RM255¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "limits.h"
#include "prnstyle.h"
#include "ppmodal.h"	
#include "dlgsmap.h"

#define GRIDSIZE 64

// Stati c prototypes
static void InitPrintTypeControls( LPIMAGE lpImage, HWND hDlg );
static void DisplayControls( HWND hDlg, BOOL bPrintNumeric );
static void CopyNumericToVisual( HWND hDlg );
static void CopyVisualToNumeric( HWND hDlg );
static BOOL SetupVisualPageSizes( HWND hDlg );
static void DoPrintSizeInits(LPIMAGE lpImage, HWND hDlg);
static void DoPrintSizeControls( HWND hDlg, ITEMID id, UINT codeNotify, BOOL bResAdjust );
static void SetSizeMinMax(HWND hDlg);

static LFIXED lfPageWidth, lfPageHeight;
static int iPrResX = 100;
static int iPrResY = 100;
static LFIXED lfOriginalWidth, lfOriginalHeight;
static UINT			idTimer;

// LOCAL prototypes
LOCAL BOOL Print_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Print_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static BOOL bInit = FALSE;
static BOOL bScatter;
static BOOL Print_bPostScript;
static WORD SepFlags;
static WORD wChannel;
static MAP *lpMap; //since this is a static pointer to a static structure,
static DLGSECTION	printsec;

static int Print_iTabTable[] =
	{
	IDD_PRINTVISUAL,
	IDD_PRINTNUMERIC
	};

/***********************************************************************/
BOOL WINPROC EXPORT DlgPrintProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Print_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Print_OnCommand);

	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);
	HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, Dialog_OnPaletteChanged);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PRINT);
	}
}

/************************************************************************/
LOCAL BOOL Print_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	BOOL fGray;
	LPSTR lpUnit;
	STRING szDescription;
	HWND hControl;
	LPIMAGE lpImage;

	lpImage = GetActiveImage();
	if ( !bInit ) // Only executed when dialog comes up (not at reset time)
	{
		AstralCursor(IDC_WAIT);
		CenterPopup( hDlg );

		// Initialise the dialog section.
		InitDlgSection(&printsec, ID_SECTION0, hDlg, IDC_PANE0, NULL);
		// Set threshold so that a category is always discarded when the user
		// selects another.
		SetDlgSectionThreshold(&printsec, 2, 2);

		int nItems = sizeof(Print_iTabTable)/sizeof(int);

		for( int i = 0; i < nItems; ++i)
		{
			// add the halftone category
			AddDlgSectionCategory(	&printsec,
									PictPubApp.GetResourceHandle(),
									MAKEINTRESOURCE(Print_iTabTable[i]),
									Print_iTabTable[i],
					    			DSCF_DISCARDABLE|DSCF_PRELOAD,
									NULL);
		}

		bScatter = Page.ScatterPrint;
		if ( AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES,
			&lpUnit ))
		{
			SetDlgItemText( hDlg, IDC_LEFTLABEL, lpUnit );
			SetDlgItemText( hDlg,IDC_TOPLABEL, lpUnit ); 
		}
		CheckComboItem( hDlg, IDC_PRINTVIEW, IDC_PRINTVIEWVISUAL,
			IDC_PRINTVIEWNUMERIC, IDC_PRINTVIEWVISUAL + Page.PrintNumeric );
		if ( !Names.Printer[0] )
			GetDefaultPrinter( Names.Printer );
		DeviceDescription( Names.Printer, szDescription );
		SetDlgItemText( hDlg, IDC_PRINTER, szDescription );
		SetDlgItemText( hDlg, IDC_PRINTSTYLE, Names.PrintStyle );

		fGray = FrameType(ImgGetBaseEditFrame(lpImage)) <= FDT_GRAYSCALE;
		if (fGray)
			Page.OutputType = IDC_PRINT_GRAY;

		SepFlags = ( fGray ? K_SEP : Page.SepFlags );
		CheckDlgButton( hDlg, IDC_DO_CYAN, (SepFlags & C_SEP) );
		CheckDlgButton( hDlg, IDC_DO_MAGENTA, (SepFlags & M_SEP) );
		CheckDlgButton( hDlg, IDC_DO_YELLOW, (SepFlags & Y_SEP) );
		CheckDlgButton( hDlg, IDC_DO_BLACK, (SepFlags & K_SEP) );
		InitPrintTypeControls( lpImage, hDlg );
	}
	else
		Page.ScatterPrint = bScatter;

	if ( hControl = GetDlgItem( hDlg, IDC_PRINTVIEWIMAGE ) )
	{
		if ( lpImage )
			SetWindowLong( hControl, GWL_IMAGE, (long)ImgGetBase(lpImage) );
		SetParent( hControl, GetDlgItem( hDlg, IDC_PRINTVIEWPAGE ) );
	}

	if ( (hControl = GetDlgItem( hDlg, IDC_PRINTVIEWPAGE )) &&
	 	(hControl = GetDlgItem( hControl, IDC_PRINTVIEWIMAGE )) )
	{
		SetLongBit(hControl, GWL_STYLE, IS_HASCHANGED, FALSE );
		SetLongBit(hControl, GWL_STYLE, IS_RATIO_CONSTRAIN, YES );
		SetLongBit(hControl, GWL_STYLE, IS_LIMITSIZE, TRUE );
	}

	DoPrintSizeInits( lpImage, hDlg );
	SetupVisualPageSizes( hDlg );
	SetSizeMinMax( hDlg );
	CopyNumericToVisual( hDlg );
	DisplayControls( hDlg, Page.PrintNumeric );
	bInit = YES;
	AstralCursor(NULL);
	return(TRUE);
}

/************************************************************************/
LOCAL void Print_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool, bOKColor;
STRING szDescription;
LPIMAGE lpImage;

lpImage = GetActiveImage();
switch (id)
	{
	case IDC_DISTORT:
	case IDC_HEIGHT:
	case IDC_WIDTH:
	case IDC_SCALEX:
	case IDC_SCALEY:
		DoPrintSizeControls( hDlg, id, codeNotify, YES );
	break;

	case IDC_CENTERED:
		Page.Centered = !Page.Centered;
		CheckDlgButton( hDlg, IDC_CENTERED, Page.Centered );
		ControlEnable( hDlg, IDC_OFFSETX, !Page.Centered );
		ControlEnable( hDlg, IDC_OFFSETY, !Page.Centered );
	break;

	case IDC_OFFSETX:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		Page.OffsetX = GetDlgItemSpinFixed( hDlg, IDC_OFFSETX,
			&Bool, YES );
	break;

	case IDC_OFFSETY:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		Page.OffsetY = GetDlgItemSpinFixed( hDlg, IDC_OFFSETY,
			&Bool, YES );
	break;

	case IDC_PRINT_TYPE:
//	case IDC_PRINT_GRAY:
//	case IDC_PRINT_COLOR:
//	case IDC_PRINT_COLORGRAY:
//	case IDC_PRINT_BLACKSEPS:
//	case IDC_PRINT_COLORSEPS:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		Page.OutputType = id;
		bOKColor = ( Page.OutputType == IDC_PRINT_BLACKSEPS ||
			Page.OutputType == IDC_PRINT_COLORSEPS );
		ShowControl( hDlg, IDC_DO_CYAN, IDC_DO_BLACK, bOKColor, NO );
		ShowControl( hDlg, IDC_DO_CPATCH, IDC_DO_KPATCH, bOKColor, NO );
	break;

	case IDC_DO_CYAN:
		SepFlags ^= C_SEP;
		CheckDlgButton( hDlg, id, SepFlags & C_SEP );
	break;

	case IDC_DO_MAGENTA:
		SepFlags ^= M_SEP;
		CheckDlgButton( hDlg, id, SepFlags & M_SEP );
	break;

	case IDC_DO_YELLOW:
		SepFlags ^= Y_SEP;
		CheckDlgButton( hDlg, id, SepFlags & Y_SEP );
	break;

	case IDC_DO_BLACK:
		SepFlags ^= K_SEP;
		CheckDlgButton( hDlg, id, SepFlags & K_SEP );
	break;

	case IDC_SCATTER:
		Page.ScatterPrint = !Page.ScatterPrint;
		CheckDlgButton( hDlg, id, Page.ScatterPrint );
	break;

	case IDC_BINARY_PS: // Postscript only
		Page.BinaryPS = !Page.BinaryPS;
		CheckDlgButton( hDlg, id, Page.BinaryPS );
	break;

	case IDC_PRINTER_SCREENING:
		Page.UsePrinterScreening = !Page.UsePrinterScreening;
		//if (Page.UsePrinterScreening && Print_bPostScript)
		//	{
		//	if (AstralOKCancel(IDS_SCREENWARNING) != IDOK)
		//		Page.UsePrinterScreening = !Page.UsePrinterScreening;
		//	}
		CheckDlgButton( hDlg, id, Page.UsePrinterScreening );
	break;

	case IDC_USEPRINTSCALING: // Postscript only
		Page.PrintScaling = !Page.PrintScaling;
		if (Page.PrintScaling && !Print_bPostScript)
			Message(IDS_PSONLY);
		CheckDlgButton( hDlg, id, Page.PrintScaling );
	break;

	case IDC_PRINTVIEW:
//	case IDC_PRINTVIEWVISUAL:
//	case IDC_PRINTVIEWNUMERIC:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		Bool = id - IDC_PRINTVIEWVISUAL;
		if ( Page.PrintNumeric == Bool )
			break;
		Page.PrintNumeric = Bool;
		if ( (hControl = GetDlgItem( hDlg, IDC_PRINTVIEWPAGE)) &&
			 (hControl = GetDlgItem( hControl, IDC_PRINTVIEWIMAGE)) )
			{
			if (Page.PrintNumeric && StyleOn(hControl, IS_HASCHANGED))
				CopyVisualToNumeric( hDlg );
			else
			if (!Page.PrintNumeric)
				{
				SetLongBit(hControl, GWL_STYLE, IS_RATIO_CONSTRAIN,
					!Edit.Distortable);
				SetLongBit(hControl, GWL_STYLE, IS_HASCHANGED, FALSE);
				SetLongBit(hControl, GWL_STYLE, IS_LIMITSIZE, TRUE);
				CopyNumericToVisual( hDlg );
				}
			}
		DisplayControls( hDlg, Page.PrintNumeric );
	break;

	case IDC_PRINTSETUP:
		// Bring up the printer selection dialog
		EnableWindow(hDlg, FALSE);
		AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PRINTSETUP,
			DlgPrintSetupProc);
		EnableWindow(hDlg, TRUE);
		DeviceDescription( Names.Printer, szDescription );
		SetDlgItemText( hDlg, IDC_PRINTER, szDescription );
		SetDlgItemText( hDlg, IDC_PRINTSTYLE, Names.PrintStyle );
		SetupVisualPageSizes( hDlg );
		SetSizeMinMax(hDlg);
		InitPrintTypeControls( lpImage, hDlg );
	break;

	case IDC_PRINTRESET:
		SendMessage( hDlg, WM_INITDIALOG, 0, 0L );
	break;

	case IDC_PRINTTESTPAGE:
		if ( !lpImage )
			break;
		EnableF1Help( NO );
		PrintTest( hDlg, filename(lpImage->CurFile), lpImage );
		EnableF1Help( YES );
	break;

	case IDOK:
		if ( !lpImage )
			break;
		EnableF1Help( NO );
		if ( !Page.PrintNumeric &&
			( hControl = GetDlgItem( hDlg, IDC_PRINTVIEWPAGE ) ) &&
			( hControl = GetDlgItem( hControl, IDC_PRINTVIEWIMAGE ) ) &&
			( StyleOn( hControl, IS_HASCHANGED ) ) )
				CopyVisualToNumeric( hDlg );

		// About to print, so load up the print style
		LoadPrintStyle( Names.PrintStyle );

		if ( !Names.Printer[0] )
			GetDefaultPrinter( Names.Printer );

		/* Print the current image file */
		if ( Page.OutputType == IDC_PRINT_BLACKSEPS ||
			Page.OutputType == IDC_PRINT_COLORSEPS )
			{
			Page.SepFlags = SepFlags;
			}
		SetUnitResolution( 0 );
		EnableF1Help( YES );
		AstralDlgEnd( hDlg, TRUE );
		bInit = FALSE;
	break;

	case IDCANCEL:
		SetUnitResolution( 0 );
		AstralDlgEnd( hDlg, FALSE );
		bInit = FALSE;
	break;

   default:
	break;
   }
}

//***********************************************************************
static void InitPrintTypeControls( LPIMAGE lpImage, HWND hDlg )
//***********************************************************************
{
BOOL bOKColor;
BOOL fGray;

Print_bPostScript = IsPostScript();
fGray = FrameType(ImgGetBaseEditFrame(lpImage)) <= FDT_GRAYSCALE;

bOKColor = (!fGray && Page.Type != IDC_PRINTER_IS_RGB);
ControlEnable( hDlg, IDC_DO_CYAN, bOKColor );
ControlEnable( hDlg, IDC_DO_MAGENTA, bOKColor );
ControlEnable( hDlg, IDC_DO_YELLOW, bOKColor );

bOKColor = (!fGray && Page.Type != IDC_PRINTER_IS_RGB );
ControlEnable( hDlg, IDC_DO_BLACK, bOKColor );

bOKColor = (!fGray && Page.Type != IDC_PRINTER_IS_MONO);
ControlEnable( hDlg, IDC_PRINT_COLOR, bOKColor );

bOKColor = (!fGray && Page.Type == IDC_PRINTER_IS_CMYK &&
	Print_bPostScript);

ControlEnable( hDlg, IDC_PRINT_COLORGRAY, bOKColor );
ControlEnable( hDlg, IDC_PRINT_COLORSEPS, bOKColor );

bOKColor = (!fGray && Page.Type != IDC_PRINTER_IS_RGB &&
	Print_bPostScript);

if ( !Control.Retail )
	{
	ControlEnable( hDlg, IDC_PRINT_COLORSEPS, NO );
	ControlEnable( hDlg, IDC_PRINT_BLACKSEPS, NO );
	}
else
    ControlEnable( hDlg, IDC_PRINT_BLACKSEPS, bOKColor );

if ( !IsWindowEnabled( GetDlgItem( hDlg, Page.OutputType ) ) )
	Page.OutputType = 
		(fGray || Page.Type == IDC_PRINTER_IS_MONO ) ?
		IDC_PRINT_GRAY : IDC_PRINT_COLOR;

bOKColor = ( Page.OutputType == IDC_PRINT_BLACKSEPS ||
	Page.OutputType == IDC_PRINT_COLORSEPS );

ShowControl( hDlg, IDC_DO_CYAN, IDC_DO_BLACK, bOKColor, NO );
ShowControl( hDlg, IDC_DO_CPATCH, IDC_DO_KPATCH, bOKColor, NO );

CheckComboItem( hDlg, IDC_PRINT_TYPE, IDC_PRINT_GRAY,
	Control.Retail ? IDC_PRINT_COLORSEPS : IDC_PRINT_COLORGRAY,
	Page.OutputType );

CheckDlgButton(hDlg, IDC_USEPRINTSCALING, Page.PrintScaling);
CheckDlgButton( hDlg, IDC_BINARY_PS, Page.BinaryPS );
CheckDlgButton( hDlg, IDC_PRINTER_SCREENING, Page.UsePrinterScreening );

ControlEnable(hDlg, IDC_USEPRINTSCALING, Print_bPostScript);
ControlEnable( hDlg, IDC_BINARY_PS, Print_bPostScript );
ControlEnable( hDlg, IDC_PRINTER_SCREENING, !Print_bPostScript);
}

//***********************************************************************
static void DisplayControls( HWND hDlg, BOOL bPrintNumeric )
//***********************************************************************
{
	SelectDlgSectionCategory(&printsec, Print_iTabTable[bPrintNumeric ? 1 : 0]);
}

//***********************************************************************
static void CopyVisualToNumeric( HWND hDlg )
//***********************************************************************
{
HWND hPageControl, hImageControl, hControl, hWndFocus;
int px, py, ix, iy;
int iScaleX, iScaleY;
RECT rPage, rImage;

if ( !(hPageControl = GetDlgItem( hDlg, IDC_PRINTVIEWPAGE )) )
	return;

if ( !(hImageControl = GetDlgItem( hPageControl, IDC_PRINTVIEWIMAGE )) )
	return;

GetClientRect( hPageControl, &rPage );
px = RectWidth( &rPage );
py = RectHeight( &rPage );
GetClientRect( hImageControl, &rImage );
ix = RectWidth( &rImage );
iy = RectHeight( &rImage );

ClientToScreen( hImageControl, (LPPOINT)&rImage.left );
ClientToScreen( hImageControl, (LPPOINT)&rImage.right );
ScreenToClient( hPageControl, (LPPOINT)&rImage.left );
ScreenToClient( hPageControl, (LPPOINT)&rImage.right );

iScaleX = FMUL( 100, FGET( lfPageWidth  * ix, lfOriginalWidth * px ) );
if ( abs( (int)iScaleX - (int)Edit.ScaleX ) > 0 )
	{
	SetDlgItemSpin( hDlg, IDC_SCALEX, iScaleX, NO );
	hWndFocus = SetFocus( hControl = GetDlgItem( hDlg, IDC_SCALEX ) );
	SendMessage( hDlg, WM_COMMAND, IDC_SCALEX, MAKELONG(hControl,EN_CHANGE) );
	SetFocus( hWndFocus );
	}

iScaleY = FMUL( 100, FGET( lfPageHeight * iy, lfOriginalHeight * py ) );
if ( abs( (int)iScaleY - (int)Edit.ScaleY ) > 0 )
	{
	SetDlgItemSpin( hDlg, IDC_SCALEY, iScaleY, NO );
	hWndFocus = SetFocus( hControl = GetDlgItem( hDlg, IDC_SCALEY ) );
	SendMessage( hDlg, WM_COMMAND, IDC_SCALEY, MAKELONG(hControl,EN_CHANGE) );
	SetFocus( hWndFocus );
	}

Page.Centered = NO;
CheckDlgButton( hDlg, IDC_CENTERED, Page.Centered );
ControlEnable( hDlg, IDC_OFFSETX, !Page.Centered );
ControlEnable( hDlg, IDC_OFFSETY, !Page.Centered );

Page.OffsetX = ( lfPageWidth * rImage.left ) / px;
Page.OffsetY = ( lfPageHeight * rImage.top ) / py;
SetDlgItemSpinFixed( hDlg, IDC_OFFSETX, Page.OffsetX, YES );
SetDlgItemSpinFixed( hDlg, IDC_OFFSETY, Page.OffsetY, YES );
}

//***********************************************************************
static void CopyNumericToVisual( HWND hDlg )
//***********************************************************************
{
HWND hPageControl, hImageControl;
int x, y, dx, dy, px, py;
RECT rPage;
WORD wShowFlag;

if ( !(hPageControl = GetDlgItem( hDlg, IDC_PRINTVIEWPAGE )) )
	return;

if ( !(hImageControl = GetDlgItem( hPageControl, IDC_PRINTVIEWIMAGE )) )
	return;

GetClientRect( hPageControl, &rPage );
px = RectWidth( &rPage );
dx = FMUL( px, FGET( Edit.Width, lfPageWidth ) );
py = RectHeight( &rPage );
dy = FMUL( py, FGET( Edit.Height, lfPageHeight ) );

if ( Page.Centered )
	{
	x = (px - dx) / 2;
	y = (py - dy) / 2;
	}
else
	{
	x  = FMUL( px, FGET( Page.OffsetX, lfPageWidth ) );
	y  = FMUL( py, FGET( Page.OffsetY, lfPageHeight ) );
	}

if ( !IsWindowVisible( hImageControl ) )
	wShowFlag = (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
else
	wShowFlag = (SWP_NOZORDER | SWP_NOACTIVATE);

SetWindowPos( hImageControl, NULL, x, y, dx-1, dy-1, wShowFlag );
}

//***********************************************************************
static BOOL SetupVisualPageSizes( HWND hDlg )
//***********************************************************************
{
HDC hPrinterDC;
int dxPrinter, dyPrinter;
POINT ptDimensions;
RECT rect;
int x, y, dx, dy, ndx, ndy;
WORD wShowFlag;
HWND hControl;

if ( !(hPrinterDC = GetPrinterDC()) )
	return( FALSE );

dxPrinter = GetDeviceCaps( hPrinterDC, HORZRES );
dyPrinter = GetDeviceCaps( hPrinterDC, VERTRES );
iPrResX = GetDeviceCaps( hPrinterDC, LOGPIXELSX );
iPrResY = GetDeviceCaps( hPrinterDC, LOGPIXELSY );
lfPageWidth = FGET( dxPrinter, iPrResX );
lfPageHeight = FGET( dyPrinter, iPrResY );

/* Get the physical page size */
if ( Escape( hPrinterDC, GETPHYSPAGESIZE,
	NULL, NULL,(LPSTR)&ptDimensions ) < 0)
	{
	ptDimensions.x = dxPrinter;
	ptDimensions.y = dyPrinter;
	}

if ( !(hControl = GetDlgItem( hDlg, IDC_PRINTVIEWEXT )) )
	return( FALSE );

GetWindowRect( hControl, &rect );
ScreenToClient( hDlg, (LPPOINT)&rect.left );
ScreenToClient( hDlg, (LPPOINT)&rect.right );

dx = ndx = RectWidth( &rect );
dy = ndy = RectHeight( &rect );
ScaleToFit( &ndx, &ndy, ptDimensions.x, ptDimensions.y );
x = rect.left + ( ( dx - ndx ) / 2 );
y = rect.top  + ( ( dy - ndy ) / 2 );

if ( !(hControl = GetDlgItem( hDlg, IDC_PRINTVIEWPHYS )) )
	return( FALSE );

if ( !IsWindowVisible( hControl ) )
	wShowFlag = (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
else
	wShowFlag = (SWP_NOZORDER | SWP_NOACTIVATE);

SetWindowPos( hControl, NULL, x, y, ndx, ndy, wShowFlag );

dx = ndx;
dy = ndy;
ndx = FMUL( dx, FGET( dxPrinter, ptDimensions.x ) );
ndy = FMUL( dy, FGET( dyPrinter, ptDimensions.y ) );
x += ( ( dx - ndx ) / 2 );
y += ( ( dy - ndy ) / 2 );

if ( !(hControl = GetDlgItem( hDlg, IDC_PRINTVIEWPAGE )) )
	return( FALSE );

if ( !IsWindowVisible( hControl ) )
	wShowFlag = (SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
else
	wShowFlag = (SWP_NOZORDER | SWP_NOACTIVATE);

SetWindowPos( hControl, NULL, x, y, ndx, ndy, wShowFlag );

DeleteDC( hPrinterDC );
return( TRUE );
}

//***********************************************************************
static void DoPrintSizeInits( LPIMAGE lpImage, HWND hDlg )
//***********************************************************************
{
LPSTR lpUnit;
LPFRAME lpFrame;

if ( !lpImage || !(lpFrame = ImgGetBaseEditFrame(lpImage)) )
	return;

/* Be careful to set all values before doing a SetDlg... with them */
/* The Edit structure is a temorary holding area for sizing dialog boxes */
Edit.Distortable = NO;
Edit.ScaleX = Edit.ScaleY = 100;
if ( lpImage )
	{
	Edit.Crop.left   = Edit.Crop.top = 0;
	Edit.Crop.right  = FrameXSize(lpFrame) - 1;
	Edit.Crop.bottom = FrameYSize(lpFrame) - 1;
	Edit.Resolution  = Edit.CropResolution = FrameResolution(lpFrame);
	Edit.Depth       = FrameDepth(lpFrame);
	if (Edit.Depth == 0) Edit.Depth = 1;
	}
else
	{
	Edit.Resolution = Edit.CropResolution = 75;
	Edit.Depth = 1;
	}
Edit.Angle       = 0;
Edit.Width       = FGET( (Edit.Crop.right-Edit.Crop.left-1),  Edit.CropResolution );
Edit.Height      = FGET( (Edit.Crop.bottom-Edit.Crop.top-1), Edit.CropResolution );
Page.Centered    = YES;
lfOriginalWidth  = Edit.Width;
lfOriginalHeight = Edit.Height;
Page.OffsetX     = 0;
Page.OffsetY     = 0;

CheckDlgButton( hDlg, IDC_CENTERED,  Page.Centered );
CheckDlgButton( hDlg, IDC_SCATTER,   Page.ScatterPrint );
CheckDlgButton( hDlg, IDC_DISTORT,   Edit.Distortable );

InitDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES, 0L, FGET(9999,1) );
InitDlgItemSpinFixed( hDlg, IDC_WIDTH,  Edit.Width,  YES, 0L, FGET(9999,1) );

InitDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, YES, 1, 10000 );
InitDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, YES, 1, 10000 );

InitDlgItemSpinFixed( hDlg, IDC_OFFSETX, Page.OffsetX, YES,
	-FGET(9999,1), FGET(9999,1) );

InitDlgItemSpinFixed( hDlg, IDC_OFFSETY, Page.OffsetY, YES,
	-FGET(9999,1), FGET(9999,1) );

if ( AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES, &lpUnit ) )
	{
	SetDlgItemText( hDlg, IDC_WIDTHLABEL,  lpUnit );
	SetDlgItemText( hDlg, IDC_HEIGHTLABEL, lpUnit );
	}
// enable/disable controls
ControlEnable(  hDlg, IDC_OFFSETX,   !Page.Centered );
ControlEnable(  hDlg, IDC_OFFSETY,   !Page.Centered );
}

//***********************************************************************
static void SetSizeMinMax(HWND hDlg)
//***********************************************************************
{
#define FMAX 0x7fffffff

LFIXED lfmin, lfmax;
long min, max;
int cropH, cropW;
BOOL Bool;

cropH = (Edit.Crop.bottom-Edit.Crop.top-1);
cropW = (Edit.Crop.right-Edit.Crop.left-1);

// Height
lfmin = FGET(1, iPrResY);
lfmax = lfPageHeight*2;
SetDlgItemSpinFixedLimits( hDlg, IDC_HEIGHT,   lfmin, lfmax);
SetDlgItemSpinFixedLimits( hDlg, IDC_OFFSETY, -lfmax, lfPageHeight);

// ScaleY
max = FMUL( (100L*Edit.CropResolution), lfmax );
max = (max + cropH/2) / cropH;
min = FMUL( (100L*Edit.CropResolution), lfmin );
min = (min + cropH/2) / cropH;
min = mbound(min,0, INT_MAX); 
max = mbound(max,0, INT_MAX); 
SetDlgItemSpinLimits( hDlg, IDC_SCALEY, (int)min, (int)max);

// Width
lfmin = FGET(1, iPrResX);         // must be at least 1 pixel
lfmax = lfPageWidth*2;
SetDlgItemSpinFixedLimits( hDlg, IDC_WIDTH, lfmin, lfmax);
SetDlgItemSpinFixedLimits( hDlg, IDC_OFFSETX, -lfmax, lfPageWidth);

// ScaleX
max = FMUL( (100L*Edit.CropResolution), lfmax );
max = (max + cropW/2) / cropW;
min = FMUL( (100L*Edit.CropResolution), lfmin );
min = (min + cropW/2) / cropW;
min = mbound(min,0, INT_MAX); 
max = mbound(max,0, INT_MAX); 
SetDlgItemSpinLimits( hDlg, IDC_SCALEX, (int)min, (int)max);

Edit.Height  = GetDlgItemSpinFixed(hDlg, IDC_HEIGHT,  &Bool, YES);
Edit.Width   = GetDlgItemSpinFixed(hDlg, IDC_WIDTH,   &Bool, YES);
Edit.ScaleX  = GetDlgItemSpin(     hDlg, IDC_SCALEX,  &Bool, NO);
Edit.ScaleY  = GetDlgItemSpin(     hDlg, IDC_SCALEY,  &Bool, NO);
Page.OffsetX = GetDlgItemSpinFixed(hDlg, IDC_OFFSETX, &Bool, YES);
Page.OffsetY = GetDlgItemSpinFixed(hDlg, IDC_OFFSETY, &Bool, YES);
}


//***********************************************************************
static void DoPrintSizeControls( HWND hDlg, ITEMID id, UINT codeNotify,
								BOOL bResAdjust )
//***********************************************************************
{
BOOL Bool;
long x, y;
HWND hWnd, hControl;
RECT rArea;

static BOOL fUpdateOthers = TRUE;       // flag to prvent feedback loops

switch( id )
{
	case IDC_DISTORT:
		Edit.Distortable = !Edit.Distortable;
		CheckDlgButton( hDlg, IDC_DISTORT, Edit.Distortable );
		if ( Edit.ScaleX != Edit.ScaleY )
		{
			Edit.ScaleX = SetDlgItemSpin(hDlg, IDC_SCALEX,
				Edit.ScaleY, NO);
			if (Edit.ScaleX != Edit.ScaleY)
			{
				Edit.ScaleY = SetDlgItemSpin(hDlg, IDC_SCALEY,
					Edit.ScaleX, NO);
				y = ((long)(Edit.Crop.bottom-Edit.Crop.top-1) * Edit.ScaleY);
				Edit.Height  = FGET( y, 100L*Edit.CropResolution );
				SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
			}
			x = ((long)(Edit.Crop.right-Edit.Crop.left-1) * Edit.ScaleX);
			Edit.Width  = FGET( x, 100L*Edit.CropResolution );
			SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES);
		}

		if ( Page.PrintNumeric )
			break;

		if ( !(hWnd = GetDlgItem( hDlg, IDC_PRINTVIEWPAGE)) )
			break;

		if ( !(hControl = GetDlgItem( hWnd, IDC_PRINTVIEWIMAGE)) )
			break;

		SetLongBit(hControl, GWL_STYLE, IS_RATIO_CONSTRAIN,
			!Edit.Distortable );

		if (!Edit.Distortable)
		{
			GetWindowRect(hControl, &rArea);
			ScreenToClient(hWnd, (LPPOINT)&rArea.left);
			ScreenToClient(hWnd, (LPPOINT)&rArea.right);
			MoveWindow(hControl, rArea.left, rArea.top,
			RectWidth(&rArea)-1, RectHeight(&rArea)-1, TRUE);
		}
	break;

	case IDC_HEIGHT:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;
		Edit.Height = GetDlgItemSpinFixed( hDlg, IDC_HEIGHT, &Bool, YES );

		if (!fUpdateOthers)
			return;

		fUpdateOthers = FALSE;

		y = FMUL( (100L*Edit.CropResolution), Edit.Height );
		Edit.ScaleY = (y + (Edit.Crop.bottom-Edit.Crop.top-1)/2) / (Edit.Crop.bottom-Edit.Crop.top-1);
		SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );

		if ( !Edit.Distortable  && ( Edit.ScaleX != Edit.ScaleY ))
		{
			Edit.ScaleX = SetDlgItemSpin(hDlg, IDC_SCALEX,
				Edit.ScaleY, NO);
			if (Edit.ScaleX != Edit.ScaleY)
			{
				Edit.ScaleY = SetDlgItemSpin(hDlg, IDC_SCALEY,
					Edit.ScaleX, NO);
				y = ((long)(Edit.Crop.bottom-Edit.Crop.top-1) * Edit.ScaleY);
				Edit.Height  = FGET( y, 100L*Edit.CropResolution );
				SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES);
			}
			x = ((long)(Edit.Crop.right-Edit.Crop.left-1) * Edit.ScaleX);
			Edit.Width  = FGET( x, 100L*Edit.CropResolution );
			SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES);
		}
		fUpdateOthers = TRUE;
	break;

	case IDC_WIDTH:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;

		if ( codeNotify != EN_CHANGE )
			break;

		Edit.Width = GetDlgItemSpinFixed( hDlg, IDC_WIDTH, &Bool, YES );

		if (!fUpdateOthers)
			return;

		fUpdateOthers = FALSE;

		x = FMUL( (100L*Edit.CropResolution), Edit.Width );
		Edit.ScaleX = (x + (Edit.Crop.right-Edit.Crop.left-1)/2) / (Edit.Crop.right-Edit.Crop.left-1);
		SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );

		if ( !Edit.Distortable  && ( Edit.ScaleX != Edit.ScaleY ))
		{
			Edit.ScaleY = SetDlgItemSpin(hDlg, IDC_SCALEY,
				Edit.ScaleX, NO);
			if (Edit.ScaleY != Edit.ScaleX)
			{
				Edit.ScaleX = SetDlgItemSpin(hDlg, IDC_SCALEX,
					Edit.ScaleY, NO);
				x = ((long)(Edit.Crop.right-Edit.Crop.left-1) * Edit.ScaleX);
				Edit.Width  = FGET( x, 100L*Edit.CropResolution );
				SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES);
			}
			y = ((long)(Edit.Crop.bottom-Edit.Crop.top-1) * Edit.ScaleY);
			Edit.Height  = FGET( y, 100L*Edit.CropResolution );
			SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES);
		}
		fUpdateOthers = TRUE;
	break;

	case IDC_SCALEX:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;

		if ( codeNotify != EN_CHANGE )
			break;

		Edit.ScaleX = GetDlgItemSpin( hDlg, IDC_SCALEX, &Bool, NO );

		if (!fUpdateOthers)
			return;

		fUpdateOthers = FALSE;

		x = ((long)(Edit.Crop.right-Edit.Crop.left-1) * Edit.ScaleX);
		Edit.Width  = FGET( x, 100L*Edit.CropResolution );
		SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES);

		if ( !Edit.Distortable && ( Edit.ScaleX != Edit.ScaleY ))
		{
			Edit.ScaleY = SetDlgItemSpin(hDlg, IDC_SCALEY,
				Edit.ScaleX, NO);
			if (Edit.ScaleY != Edit.ScaleX)
			{
				Edit.ScaleX = SetDlgItemSpin(hDlg, IDC_SCALEX,
					Edit.ScaleY, NO);
				x = ((long)(Edit.Crop.right-Edit.Crop.left-1) * Edit.ScaleX);
				Edit.Width  = FGET( x, 100L*Edit.CropResolution );
				SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
			}
			y = ((long)(Edit.Crop.bottom-Edit.Crop.top-1) * Edit.ScaleY);
			Edit.Height  = FGET( y, 100L*Edit.CropResolution );
			SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES);
		}
		fUpdateOthers = TRUE;
	break;

	case IDC_SCALEY:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;

		if ( codeNotify != EN_CHANGE )
			break;

		Edit.ScaleY = GetDlgItemSpin( hDlg, IDC_SCALEY, &Bool, NO );
 
		if (!fUpdateOthers)
			return;

		fUpdateOthers = FALSE;

		y = ((long)(Edit.Crop.bottom-Edit.Crop.top-1) * Edit.ScaleY);
		Edit.Height = FGET( y, 100L*Edit.CropResolution );
		SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );

		if ( !Edit.Distortable && ( Edit.ScaleX != Edit.ScaleY ))
		{
			Edit.ScaleX = SetDlgItemSpin(hDlg, IDC_SCALEX,
				Edit.ScaleY, NO);
			if (Edit.ScaleX != Edit.ScaleY)
			{
				Edit.ScaleY = SetDlgItemSpin(hDlg, IDC_SCALEY,
					Edit.ScaleX, NO);
				y = ((long)(Edit.Crop.bottom-Edit.Crop.top-1) * Edit.ScaleY);
				Edit.Height  = FGET( y, 100L*Edit.CropResolution );
				SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES);
			}
			x = ((long)(Edit.Crop.right-Edit.Crop.left-1) * Edit.ScaleX);
			Edit.Width  = FGET( x, 100L*Edit.CropResolution );
			SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES);
		}
		fUpdateOthers = TRUE;
	break;

	default:
	break;
	}
}

// LOCAL prototypes
LOCAL BOOL PrintSetup_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PrintSetup_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static LPPAGE lpPageBackup;
static STRING SavedPrintStyle;

/***********************************************************************/
BOOL WINPROC EXPORT DlgPrintSetupProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PrintSetup_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PrintSetup_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PRINTSETUP);
	}
}

/************************************************************************/
LOCAL BOOL PrintSetup_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
    AstralCursor(IDC_WAIT);
    CenterPopup( hDlg );
    if ( !Names.Printer[0] )
	    GetDefaultPrinter( Names.Printer );
    lstrcpy( SavedPrintStyle, Names.PrintStyle );
    lstrcpy( Names.Saved, Names.Printer );
    InitPrinterListBox( hDlg, Names.Printer );
    InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.PrintStyle,
	    IDN_STYLE );
    CheckComboItem( hDlg, IDC_PRINTERTYPES, IDC_PRINTER_IS_MONO,
	    IDC_PRINTER_IS_RGB, Page.Type );
    if ( lpPageBackup = (LPPAGE)Alloc( (long)sizeof(PAGE) ) )
	    *lpPageBackup = Page;
    AstralCursor(NULL);
    return(TRUE);
}


void UpdateTheWorld()
   {
	POSITION TemplatePos = PictPubApp.GetFirstDocTemplate ();
	while (TemplatePos)
	   {
	   CDocTemplate *pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
	   POSITION DocPos = pTemplate->GetFirstDocPosition();
	   while (DocPos)
	      {
		   CServerDoc *pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
		   if (pDoc)
	         pDoc->UpdateAllViews(NULL);
         }
      }
   }

/************************************************************************/
LOCAL void PrintSetup_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_PRINTSELECT:
		if ( codeNotify == LBN_SELCHANGE )
			{
			GetPrinterListBox( hDlg, Names.Printer );
			}
		if ( codeNotify != LBN_DBLCLK )
			break;
		// else fall through to setup printer

	case IDC_PRINTSETUP:
		EnableWindow( hDlg, NO );
		SetupPrinter( hDlg, Names.Printer );
		EnableWindow( hDlg, YES );
		SetFocus( hDlg );
	break;

	case IDC_EXTNAMES:
		if ( codeNotify == LBN_SELCHANGE )
			{
			GetExtName( hDlg, id, IDC_FILENAME,
				Names.PrintStyle, IDN_STYLE, codeNotify );
			}
		if ( codeNotify != LBN_DBLCLK )
			break;
		// else fall through to setup print style

	case IDC_SETUPSTYLE:
	{
		CString szString;

		szString.LoadString(IDS_EDITPRINTSTYLE);
		szString += Names.PrintStyle;
		CPrintStylePropSheet PrintStyleSheet(szString, AfxGetMainWnd(), IDD_PRINTSTYLE);
		PrintStyleSheet.DoModal();
		InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, Names.PrintStyle,
			IDN_STYLE );
	}
	break;

	case IDC_EXTMANAGE:
		PopupMenuEx( hDlg, id, IDC_EXTMANAGE, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			Names.PrintStyle, IDN_STYLE, id, NO );
	break;

	case IDC_PRINTERTYPES:
//	case IDC_PRINTER_IS_MONO:
//	case IDC_PRINTER_IS_CMYK:
//	case IDC_PRINTER_IS_RGB:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		Page.Type = id;
	break;

	case IDOK:
		AstralCursor(IDC_WAIT);
		LoadPrintStyle( Names.PrintStyle );

		if ( !IsPostScript() &&
			( Halftone.DoHalftoning ||
			  !StringsEqualID( Separate.BGName, IDS_NONE ) ) )
			{
			AstralCursor(NULL);
			if (AstralOKCancel( IDS_EBADSTYLE ) == IDCANCEL)
				break;
			}

		if ( lpPageBackup )
			{
			FreeUp( (LPTR)lpPageBackup );
			lpPageBackup = NULL;
			}

		if( Control.CMSEnabled && PrintStyle.fUseCMS )
			{
			if( ( Page.Type == IDC_PRINTER_IS_CMYK &&
					Separate.cmsInfo.dst.DataType != FDT_CMYKCOLOR ) ||
					( Page.Type == IDC_PRINTER_IS_RGB &&
					Separate.cmsInfo.dst.DataType != FDT_RGBCOLOR ) )
				{
				AstralCursor(NULL);
				Message( IDS_EBADPT );
				break;
				}
			}
		if( !Control.CMSEnabled && PrintStyle.fUseCMS )
			{
			AstralCursor(NULL);
			Message( IDS_CMS_EBADCOMBO );
			}

		AstralCursor(NULL);
        UpdateTheWorld();
		AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
		lstrcpy( Names.PrintStyle, SavedPrintStyle );
		lstrcpy( Names.Printer, Names.Saved );
		LoadPrintStyle( Names.PrintStyle );
		if ( lpPageBackup )
			{
			Page = *lpPageBackup;
			FreeUp( (LPTR)lpPageBackup );
			lpPageBackup = NULL;
			}
		AstralDlgEnd( hDlg, FALSE );
        UpdateTheWorld();
	break;

   default:
	break;
   }
}

// This dialog is the black generation file editor; edit Separate.BGName

// LOCAL prototypes
//LOCAL BOOL BlackGenerate_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
//LOCAL void BlackGenerate_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
//
//MAPVALUES BG_Values;
//
///***********************************************************************/
//BOOL WINPROC EXPORT DlgBlackGenerateProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
///***********************************************************************/
//{
//switch (msg)
//	{
//	HANDLE_DLGRET(hDlg, WM_INITDIALOG, BlackGenerate_OnInitDialog);  
//	HANDLE_DLGMSG(hDlg, WM_COMMAND, BlackGenerate_OnCommand);
//
//	default:
//	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_BLACKGENERATE);
//	}
//}
//
///************************************************************************/
//LOCAL BOOL BlackGenerate_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
///************************************************************************/
//{
//	CenterPopup( hDlg );
//
//	// set up to edit the black generation map
//	SetDlgItemText( hDlg, IDC_FNAME, Separate.BGName );
//	BlackMap = Separate.BGMap;
//	BG_Values.wChannel = IDC_CAL_BLACK;
//	HandleMapInit(hDlg, &BG_Values, NULL, FDT_GRAYSCALE, FALSE);
//
//	return(TRUE);
//}
//
///************************************************************************/
//LOCAL void BlackGenerate_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
///************************************************************************/
//{
//int i;
//FNAME szFileName, szExtName;
//
//switch (id)
//	{
//	case IDOK:
//		for ( i=0; i<BG_Values.pMap->Points-1; i++ )
//			{
//			if ( BG_Values.pMap->Pnt[i].x > BG_Values.pMap->Pnt[i+1].x )
//				break;
//			}
//		if ( i != (BG_Values.pMap->Points-1) )
//			{
//			Message( IDS_EMAPASC );
//			break;
//			}
//		lstrcpy( szExtName, Separate.BGName );
//		if ( !PromptForExtName( hDlg, szExtName, szFileName, IDN_BGMAP ) )
//			break;
//		if ( !SaveMap( BG_Values.pMap, NULL, NULL, NULL, szFileName ) )
//			break;
//		if ( !AddExtFile( szExtName, szFileName, IDN_BGMAP ) )
//			{
//			FileDelete( szFileName );
//			break;
//			}
//		lstrcpy( Separate.BGName, szExtName );
//		Separate.BGMap = *(BG_Values.pMap);
//		AstralDlgEnd( hDlg, TRUE );
//	break;
//
//	case IDCANCEL:
//		// Load the old separation file from disk
////		LoadSeparateStyle( PrintStyle.SeparateName );
//		AstralDlgEnd( hDlg, FALSE );
//	break;
//
//   default:
//		if (HandleMapCommand(hDlg, id, codeNotify, &BG_Values, NULL))
//			ControlEnable(hDlg, IDOK, BG_Values.fModified);
//	break;
//   }
//}
//
//#ifdef UNUSED
//// This dialog is the print calibration file editor; edit PrintStyle.CalibrateName
//
//// LOCAL prototypes
//LOCAL BOOL GrayBalance_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
//LOCAL void GrayBalance_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
//
//MAPVALUES GrayBalance_Values;
//
///***********************************************************************/
//BOOL WINPROC EXPORT DlgGrayBalanceProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
///***********************************************************************/
//{
//	switch (msg)
//	{
//		HANDLE_DLGRET(hDlg, WM_INITDIALOG, GrayBalance_OnInitDialog);  
//		HANDLE_DLGMSG(hDlg, WM_COMMAND, GrayBalance_OnCommand);
//
//		default:
//		return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_GRAYBALANCEEDIT);
//	}
//}
//
///************************************************************************/
//LOCAL BOOL GrayBalance_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
///************************************************************************/
//{
//	int x, y;
//	FNAME szFileName;
//
//	CenterPopup( hDlg );
//
//	GrayBalance_Values.fModified = FALSE;
//
//	// remove or add percents
//	SetAllDlgItemsText(hDlg, IDC_PERCENT, Mapping.Map_bPercent ? (LPSTR)"%":(LPSTR)"");
//
//	// initialize the map conrol 
//	if (Mapping.Map_bGrid)
//		x = y = GRIDSIZE;
//	else
//		x = y = 0;	
//	Map_Init(	GetDlgItem( hDlg, IDC_MAP ), 
//				x, // gridX 
//				y, // gridX 
//				1.0, // gamma 
//				11, // MaxPoints
//				0); // styles
//
//	// ensure that the correct window is showing
//	GrayBalance_Values.bMap_Visual = NO;
//	MapActivatePane(hDlg, IDC_MAPTOOL3, &GrayBalance_Values);
//
//	// set maps to defaults
//	GrayBalance_Values.wChannel = IDC_DO_ALL;
//	HandleResetAll(hDlg, IDC_RESETALL, 0, &GrayBalance_Values, NULL);
//
//	if ( LookupExtFile(Calibrate.BalName, szFileName, IDN_MAP) )
//		LoadMap( &MasterMap, &RedMap, &GreenMap, &BlueMap, &BlackMap, szFileName );
//
//	GrayBalance_Values.pMap = ResetMapChannel( hDlg, &GrayBalance_Values );
//
//	ChannelsCombo_Setup(hDlg, IDC_CHANNELS, FDT_CMYKCOLOR, GrayBalance_Values.wChannel);
//
//	ControlEnable(hDlg, IDOK, GrayBalance_Values.fModified);
//
//	return(TRUE);
//}
//
///************************************************************************/
//LOCAL void GrayBalance_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
///************************************************************************/
//{
//	switch (id)
//		{
//		case IDOK:
//			if ( !AstralDlg( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MAPSAVE,
//		 		DlgMapSaveProc ) )
//				break;
//			lstrcpy(Calibrate.BalName, Names.ColorMap);
//			AstralDlgEnd( hDlg, TRUE );
//		break;
//
//		case IDCANCEL:
//			AstralDlgEnd( hDlg, FALSE );
//		break;
//
//	   	default:
//			if (HandleMapCommand(hDlg, id, codeNotify, &GrayBalance_Values, NULL))
//				ControlEnable(hDlg, IDOK, GrayBalance_Values.fModified);
//		break;
//   	}
//}
//#endif

// This dialog is the print calibration file editor; edit PrintStyle.CalibrateName

// LOCAL prototypes
//LOCAL BOOL CalPrintEdit_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
//LOCAL void CalPrintEdit_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
//
//MAPVALUES CalPrintEdit_Values;
//
///***********************************************************************/
//BOOL WINPROC EXPORT DlgCalPrintEditProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
///***********************************************************************/
//{
//	switch (msg)
//	{
//		HANDLE_DLGRET(hDlg, WM_INITDIALOG, CalPrintEdit_OnInitDialog);  
//		HANDLE_DLGMSG(hDlg, WM_COMMAND, CalPrintEdit_OnCommand);
//
//		default:
//		return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CALIBRATEPRINTEDIT);
//	}
//}
//
///************************************************************************/
//LOCAL BOOL CalPrintEdit_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
///************************************************************************/
//{
//	int x, y;
//
//	CenterPopup( hDlg );
//
//	// set up to edit the calibration map
//	MasterMap = Calibrate.CalMap[CAL_MASTER];
//	RedMap    = Calibrate.CalMap[CAL_CYAN];    
//	GreenMap 	= Calibrate.CalMap[CAL_MAGENTA];	
//	BlueMap  	= Calibrate.CalMap[CAL_YELLOW];
//	BlackMap = Calibrate.CalMap[CAL_BLACK];
//
//	CalPrintEdit_Values.wChannel = IDC_CAL_MASTER;
// 	HandleMapInit(hDlg, &CalPrintEdit_Values, GetActiveDoc(), FDT_CMYKCOLOR, FALSE);
//
//	CheckComboItem( hDlg, IDC_CAL_CHANNELS, IDC_CAL_MASTER, IDC_CAL_BLACK,
//				CalPrintEdit_Values.wChannel );
//	MapSetGradients(hDlg, TRUE, CalPrintEdit_Values.wChannel);
//
//	return(TRUE);
//}
//
///************************************************************************/
//LOCAL void CalPrintEdit_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
///************************************************************************/
//{
//	int i, map;
//
//	switch (id)
//		{
//		case IDC_CAL_CHANNELS:
//	//  case IDC_CAL_MASTER:
//	//	case IDC_CAL_BLACK:
//	//	case IDC_CAL_CYAN:
//	//	case IDC_CAL_MAGENTA:
//	//	case IDC_CAL_YELLOW:
//			if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
//				break;
//			CalPrintEdit_Values.wChannel = id;
//			CalPrintEdit_Values.pMap = ResetMapChannel( hDlg, &CalPrintEdit_Values );
//			MapSetGradients(hDlg, TRUE, CalPrintEdit_Values.wChannel);
//		break;
//
//		case IDOK:
//		Calibrate.CalMap[CAL_MASTER] = MasterMap;
//		Calibrate.CalMap[CAL_CYAN] = RedMap;
//		Calibrate.CalMap[CAL_MAGENTA] = GreenMap;
//		Calibrate.CalMap[CAL_YELLOW] = BlueMap;
//		Calibrate.CalMap[CAL_BLACK] = BlackMap;
//		for (map = 0; map < CAL_MAPS; ++map)
//			{
//			for ( i=0; i<Calibrate.CalMap[map].Points-1; i++ )
//					if ( Calibrate.CalMap[map].Pnt[i].x >
//					 	Calibrate.CalMap[map].Pnt[i+1].x )
//						break;
//				{
//				if ( i != (Calibrate.CalMap[map].Points-1) )
//					{
//					Message( IDS_EMAPASC );
//					break;
//					}
//				}
//			if ( i != (Calibrate.CalMap[map].Points-1) )
//				break;
//			}
//			if ( !SaveCalibrateStyle( hDlg, PrintStyle.CalibrateName ) )
//				break;
//			AstralDlgEnd( hDlg, TRUE );
//		break;
//
//		case IDCANCEL:
//			// Load the old calibration file from disk
//			LoadCalibrateStyle( PrintStyle.CalibrateName );
//			AstralDlgEnd( hDlg, FALSE );
//		break;
//
//	   	default:
//			if (HandleMapCommand(hDlg, id, codeNotify, &CalPrintEdit_Values, NULL))
//				ControlEnable(hDlg, IDOK, CalPrintEdit_Values.fModified);
//		break;
//   	}
//}
////////////////////////////////////CBlackGenerateDlg////////////////////////////////////////

IMPLEMENT_DYNCREATE(CBlackGenerateDlg, CPPMapDlg)

CBlackGenerateDlg::CBlackGenerateDlg(CServerView* pView, CWnd* pParent)
:CPPMapDlg(pView, CBlackGenerateDlg::IDD, pParent)
{

}

BOOL CBlackGenerateDlg::OnInitDialog()
{
	CenterWindow();

	// set up to edit the black generation map
	SetDlgItemText(IDC_FNAME, Separate.BGName );
	m_BlackMap = Separate.BGMap;
	m_MapValues.wChannel = IDC_CAL_BLACK;
	HandleMapInit(FDT_GRAYSCALE, FALSE);
	return CPPMapDlg::OnInitDialog();
}

BOOL CBlackGenerateDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);

	if(id != IDOK && id != IDCANCEL)
		if (HandleMapCommand(id, codeNotify))
			ControlEnable(GetSafeHwnd(), IDOK, m_MapValues.fModified);
	return CWnd::OnCommand(wParam, lParam);
}

void CBlackGenerateDlg::OnCancel()
{
	CPPMapDlg::OnCancel();
}

void CBlackGenerateDlg::OnOK()
{

int i;
FNAME szFileName, szExtName;

	for ( i=0; i<m_MapValues.pMap->Points-1; i++ )
	{
		if ( m_MapValues.pMap->Pnt[i].x > m_MapValues.pMap->Pnt[i+1].x )
		{
			EndDialog(FALSE);
			CPPMapDlg::OnOK();
		}
	}
	if ( i != (m_MapValues.pMap->Points-1) )
	{
		Message( IDS_EMAPASC );
		EndDialog(FALSE);
		CPPMapDlg::OnOK();
	}
	lstrcpy( szExtName, Separate.BGName );
	if ( !PromptForExtName( GetSafeHwnd(), szExtName, szFileName, IDN_BGMAP ) )
	{
		EndDialog(FALSE);
		CPPMapDlg::OnOK();
	}

	if ( !SaveMap( m_MapValues.pMap, NULL, NULL, NULL, szFileName ) )
	{
		EndDialog(FALSE);
		CPPMapDlg::OnOK();
	}
	if ( !AddExtFile( szExtName, szFileName, IDN_BGMAP ) )
		{
		FileDelete( szFileName );
		EndDialog(FALSE);
		CPPMapDlg::OnOK();
		}
	lstrcpy( Separate.BGName, szExtName );
	Separate.BGMap = *(m_MapValues.pMap);
	CPPMapDlg::OnOK();	
}

///////////////////////////////////CCalibratePrintEditDlg////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CCalibratePrintEditDlg, CPPMapDlg)

CCalibratePrintEditDlg::CCalibratePrintEditDlg(CServerView* pView, CWnd* pParent)
:CPPMapDlg(pView, CCalibratePrintEditDlg::IDD, pParent)
{

}

BOOL CCalibratePrintEditDlg::OnInitDialog()
{

	CenterWindow();
	// set up to edit the calibration map
	m_MasterMap = Calibrate.CalMap[CAL_MASTER];
	m_RedMap    = Calibrate.CalMap[CAL_CYAN];    
	m_GreenMap 	= Calibrate.CalMap[CAL_MAGENTA];	
	m_BlueMap  	= Calibrate.CalMap[CAL_YELLOW];
	m_BlackMap = Calibrate.CalMap[CAL_BLACK];

	m_MapValues.wChannel = IDC_CAL_MASTER;
 	HandleMapInit(FDT_CMYKCOLOR, FALSE);

	CheckComboItem( GetSafeHwnd(), IDC_CAL_CHANNELS, IDC_CAL_MASTER, IDC_CAL_BLACK,
				m_MapValues.wChannel );
	MapSetGradients();
	return CPPMapDlg::OnInitDialog();
}

BOOL CCalibratePrintEditDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int i, map;
	int id = LOWORD(wParam);
	WORD codeNotify = HIWORD(wParam);
	HWND hDlg = GetSafeHwnd();

	switch (id)
		{
		case IDC_CAL_CHANNELS:
			if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
				break;
			m_MapValues.wChannel = id;
			m_MapValues.pMap = ResetMapChannel();
			MapSetGradients();
		break;

		case IDOK:
		Calibrate.CalMap[CAL_MASTER] = m_MasterMap;
		Calibrate.CalMap[CAL_CYAN] = m_RedMap;
		Calibrate.CalMap[CAL_MAGENTA] = m_GreenMap;
		Calibrate.CalMap[CAL_YELLOW] = m_BlueMap;
		Calibrate.CalMap[CAL_BLACK] = m_BlackMap;
		for (map = 0; map < CAL_MAPS; ++map)
			{
			for ( i=0; i<Calibrate.CalMap[map].Points-1; i++ )
					if ( Calibrate.CalMap[map].Pnt[i].x >
					 	Calibrate.CalMap[map].Pnt[i+1].x )
						break;
				{
				if ( i != (Calibrate.CalMap[map].Points-1) )
					{
					Message( IDS_EMAPASC );
					break;
					}
				}
			if ( i != (Calibrate.CalMap[map].Points-1) )
				break;
			}
			if ( !SaveCalibrateStyle( hDlg, PrintStyle.CalibrateName ) )
				break;
			EndDialog(TRUE);
		break;


	   	default:
			if (HandleMapCommand(id, codeNotify))
				ControlEnable(hDlg, IDOK, m_MapValues.fModified);
		break;
   	}

return CWnd::OnCommand(wParam, lParam);
}

void CCalibratePrintEditDlg::OnCancel()
{
	LoadCalibrateStyle( PrintStyle.CalibrateName );
	CPPMapDlg::OnCancel();
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
int	Hi, Lo, i, DotGainDefault;

szFileName[0] = '\0';
if ( !(bRet = LookupExtFile( lpExtName, szFileName, IDN_STYLE )) )
	{
	if ( AstralStr( IDS_UNTITLED, &lp ) )
		lstrcpy( lpExtName, lp );
	}

GetPrivateStr( "HalftoneName",  PrintStyle.HalftoneName, "" );
GetPrivateStr( "SeparateName",  PrintStyle.SeparateName, "" );
GetPrivateStr( "CalibrateName", PrintStyle.CalibrateName, "" );

// Separation settings
GetPrivateInt( "UseCMS",		PrintStyle.fUseCMS, FALSE );

GetPrivateInt( "DataTypeHi",	Hi, -1 );
GetPrivateInt( "DataTypeLo",	Lo, -1 );
Separate.cmsInfo.dst.DataType = ( FRMDATATYPE )MAKELONG( Lo, Hi );

GetPrivateInt( "ToRCSHi", 		Hi, 0 );
GetPrivateInt( "ToRCSLo", 		Lo, 0 );
Separate.cmsInfo.dst.ptInfo.toRCS = MAKELONG( Lo, Hi );

GetPrivateInt( "FrRCSHi", 		Hi, 0 );
GetPrivateInt( "FrRCSLo", 		Lo, 0 );
Separate.cmsInfo.dst.ptInfo.frRCS = MAKELONG( Lo, Hi );
GetPrivateInt( "MfromR", 		Separate.MfromR, 0 );
GetPrivateInt( "YfromG", 		Separate.YfromG, 0 );
GetPrivateInt( "CfromB", 		Separate.CfromB, 0 );
GetPrivateInt( "UCR",    		Separate.UCR, 25 );
GetPrivateStr( "BGName", 		Separate.BGName, "" );
GetPrivateInt( "BlackLimit",    Separate.BlackLimit, 100 );
GetPrivateInt( "TotalInk",    	Separate.TotalInk, 300 );
GetPrivateInt( "SatBoost",    	Separate.SatBoost, 15 );
Separate.DoInkCorrect = TRUE;
if (PrintStyle.SeparateName[0])
	LoadSeparateStyle( PrintStyle.SeparateName );

// Halftone settings
GetPrivateInt( "Dot",     		Halftone.DotShape, 0 ); Halftone.DotShape += IDC_CIRCLEDOT;
GetPrivateFix( "FreqC",   		Halftone.ScreenRuling[0], 53 );
GetPrivateFix( "FreqM",   		Halftone.ScreenRuling[1], 53 );
GetPrivateFix( "FreqY",   		Halftone.ScreenRuling[2], 53 );
GetPrivateFix( "FreqK",   		Halftone.ScreenRuling[3], 53 );
GetPrivateFix( "AngleC",  		Halftone.ScreenAngle[0], 15 );
GetPrivateFix( "AngleM",  		Halftone.ScreenAngle[1], 75 );
GetPrivateFix( "AngleY",  		Halftone.ScreenAngle[2], 0 );
GetPrivateFix( "AngleK",  		Halftone.ScreenAngle[3], 45 );
GetPrivateInt( "DoHalftoning",  Halftone.DoHalftoning, TRUE );

// Calibration settings
if (LoadCalibrateStyle( PrintStyle.CalibrateName ))
	DotGainDefault = 0;
else
	DotGainDefault = 25;
GetPrivateInt( "DotGain",    		Calibrate.DotGain, DotGainDefault );
GetPrivateInt( "DoCalOnConvert",  	Calibrate.DoCalOnConvert, FALSE );
GetPrivateInt( "Hilight", 		 	Calibrate.Hilight, 0 );
GetPrivateInt( "Shadow",  			Calibrate.Shadow, 255 );

// Extras settings
GetPrivateInt( "TrimMarks",  	PrintStyle.TrimMarks, Page.TrimMarks );
GetPrivateInt( "RegMarks",  	PrintStyle.RegMarks, Page.RegMarks );
if( Control.Retail )
    GetPrivateInt( "Labels",  		PrintStyle.Labels, Page.Labels );
GetPrivateInt( "StepScale",  	PrintStyle.StepScale, Page.StepScale );
GetPrivateInt( "Negative",  	PrintStyle.Negative, Page.Negative );
GetPrivateInt( "EmulsionDown",  PrintStyle.EmulsionDown, Page.EmulsionDown );

// if somebody already has a halftone style, then use it
if (PrintStyle.HalftoneName[0])
	LoadHalftoneStyle( PrintStyle.HalftoneName );

LoadBlackGeneration(&Separate);

// setups seperation stuff
// SHOULD WARN USER HERE IF MULTITHREAD
InitPrintStyle(&Separate, &Calibrate);

return( bRet );
}

/***********************************************************************/
BOOL LoadBlackGeneration( LPSEPARATE lpSeparate )
/***********************************************************************/
{
	LPSTR lp;
	FNAME szFileName;

	if ( !LookupExtFile( lpSeparate->BGName, szFileName, IDN_BGMAP ) )
		{
		if ( AstralStr( IDS_NONE, &lp ) )
			lstrcpy( lpSeparate->BGName, lp );
		}

	if ( !(lpSeparate->DoBlackAndUCR = !StringsEqualID( lpSeparate->BGName, IDS_NONE )) )
		szFileName[0] = '\0';

	ResetMap( &lpSeparate->BGMap, Mapping.Map_nPoints, NO );
	lpSeparate->BGMap.iCurves = Mapping.Map_bCurves ? Mapping.Map_CurveType:0;
	MakeMap( &lpSeparate->BGMap );

	if ( szFileName[0] )
		LoadMap( &lpSeparate->BGMap, NULL, NULL, NULL, szFileName );
	return(TRUE);
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
GetPrivateInt( "Hilight", Calibrate.Hilight, 0 );
GetPrivateInt( "Shadow",  Calibrate.Shadow, 255 );

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
GetPrivateInt( "UCR",    Separate.UCR, 25 );
GetPrivateStr( "BGName", Separate.BGName, "" );

return( Separate.DoInkCorrect );
}

/***********************************************************************/
BOOL LoadCalibrateStyle( LPSTR lpExtName )
/***********************************************************************/
{
LPSTR lp;
FNAME szFileName;
int i;

if ( !LookupExtFile( lpExtName, szFileName, IDN_CALMAP ) )
	{
	if ( AstralStr( IDS_NONE, &lp ) )
		lstrcpy( lpExtName, lp );
	}

if ( !(Calibrate.DoCalibration = !StringsEqualID( lpExtName, IDS_NONE )) )
	szFileName[0] = '\0';

for (i = 0; i < CAL_MAPS; ++i)
{
	ResetMap( &Calibrate.CalMap[i], Mapping.Map_nPoints, NO );
	Calibrate.CalMap[i].iCurves = Mapping.Map_bCurves ? Mapping.Map_CurveType:0;
	MakeMap( &Calibrate.CalMap[i]);
}

if ( szFileName[0] )
	LoadCalMap( &Calibrate.CalMap[CAL_MASTER], &Calibrate.CalMap[CAL_CYAN],
				&Calibrate.CalMap[CAL_MAGENTA],	&Calibrate.CalMap[CAL_YELLOW],
				&Calibrate.CalMap[CAL_BLACK], szFileName );

return( Calibrate.DoCalibration );
}


/***********************************************************************/
BOOL LoadScanCalibrate( LPSTR lpExtName )
/***********************************************************************/
{
LPSTR lp;
FNAME szFileName;
int i;

if ( !LookupExtFile( lpExtName, szFileName, IDN_SCANMAP ) )
	{
	if ( AstralStr( IDS_NONE, &lp ) )
		lstrcpy( lpExtName, lp );
	}

if ( !(Scans.DoCalibration = !StringsEqualID( lpExtName, IDS_NONE )) )
	szFileName[0] = '\0';

for (i = 0; i < CALSCAN_MAPS; ++i)
{
	ResetMap( &Scans.CalMap[i], Mapping.Map_nPoints, NO );
	Scans.CalMap[i].iCurves = Mapping.Map_bCurves ? Mapping.Map_CurveType:0;
	MakeMap( &Scans.CalMap[i]);
}

if ( szFileName[0] )
	LoadMap(&Scans.CalMap[CALSCAN_GRAY], &Scans.CalMap[CALSCAN_RED],
			 &Scans.CalMap[CALSCAN_GREEN], &Scans.CalMap[CALSCAN_BLUE],
			 szFileName );

return( Scans.DoCalibration );
}

/***********************************************************************/
BOOL SavePrintStyle( HWND hDlg, LPSTR lpExtName )
/***********************************************************************/
{
LPSTR lp;
FNAME szFileName, szNewExtName;
char	pt[128];

lstrcpy( szNewExtName, lpExtName );
if ( StringsEqualID( szNewExtName, IDS_NONE ) && AstralStr( IDS_UNTITLED, &lp ) )
	lstrcpy( szNewExtName, lp );

szFileName[0] = '\0';
if ( !PromptForExtName( hDlg, szNewExtName, szFileName, IDN_STYLE ) )
	return( FALSE );
if ( !szFileName[0] )
	return( FALSE );

PutPrivateStr( "HalftoneName", "" );
PutPrivateStr( "SeparateName", "" );
PutPrivateStr( "CalibrateName", PrintStyle.CalibrateName );
PutPrivateInt( "UseCMS",		PrintStyle.fUseCMS );

// separation settings
PutPrivateInt( "DataTypeHi",	HIWORD( Separate.cmsInfo.dst.DataType ) );
PutPrivateInt( "DataTypeLo",	LOWORD( Separate.cmsInfo.dst.DataType ) );
PutPrivateInt( "ToRCSHi", 		HIWORD( Separate.cmsInfo.dst.ptInfo.toRCS ) );
PutPrivateInt( "ToRCSLo", 		LOWORD( Separate.cmsInfo.dst.ptInfo.toRCS ) );
PutPrivateInt( "FrRCSHi", 		HIWORD( Separate.cmsInfo.dst.ptInfo.frRCS ) );
PutPrivateInt( "FrRCSLo", 		LOWORD( Separate.cmsInfo.dst.ptInfo.frRCS ) );
PutPrivateInt( "MfromR", 		Separate.MfromR );
PutPrivateInt( "YfromG", 		Separate.YfromG );
PutPrivateInt( "CfromB", 		Separate.CfromB );
PutPrivateInt( "UCR",    		Separate.UCR );
PutPrivateStr( "BGName", 		Separate.BGName );
PutPrivateInt( "BlackLimit",    Separate.BlackLimit );
PutPrivateInt( "TotalInk",    	Separate.TotalInk );
PutPrivateInt( "SatBoost",    	Separate.SatBoost );


// calibration settings
PutPrivateInt( "DotGain",    	Calibrate.DotGain );
PutPrivateInt( "DoCalOnConvert",Calibrate.DoCalOnConvert );
PutPrivateInt( "Hilight", 		Calibrate.Hilight );
PutPrivateInt( "Shadow",  		Calibrate.Shadow );

// halftone settings
PutPrivateInt( "Dot",     Halftone.DotShape - IDC_CIRCLEDOT );
PutPrivateFix( "FreqC",   Halftone.ScreenRuling[0] );
PutPrivateFix( "FreqM",   Halftone.ScreenRuling[1] );
PutPrivateFix( "FreqY",   Halftone.ScreenRuling[2] );
PutPrivateFix( "FreqK",   Halftone.ScreenRuling[3] );
PutPrivateFix( "AngleC",  Halftone.ScreenAngle[0] );
PutPrivateFix( "AngleM",  Halftone.ScreenAngle[1] );
PutPrivateFix( "AngleY",  Halftone.ScreenAngle[2] );
PutPrivateFix( "AngleK",  Halftone.ScreenAngle[3] );
PutPrivateInt( "DoHalftoning",  Halftone.DoHalftoning );

// extras settings
PutPrivateInt( "TrimMarks",  PrintStyle.TrimMarks );
PutPrivateInt( "RegMarks",  PrintStyle.RegMarks );
PutPrivateInt( "Labels",  PrintStyle.Labels );
PutPrivateInt( "StepScale",  PrintStyle.StepScale );
PutPrivateInt( "Negative",  PrintStyle.Negative );
PutPrivateInt( "EmulsionDown",  PrintStyle.EmulsionDown );

if ( !AddExtFile( szNewExtName, szFileName, IDN_STYLE ) )
	{
	FileDelete( szFileName );
	return( FALSE );
	}
lstrcpy( lpExtName, szNewExtName );

return( TRUE );
}


/***********************************************************************/
BOOL SaveHalftoneStyle( HWND hDlg, LPSTR lpExtName )
/***********************************************************************/
{
LPSTR lp;
FNAME szFileName, szNewExtName;

lstrcpy( szNewExtName, lpExtName );
if ( StringsEqualID( szNewExtName, IDS_NONE ) && AstralStr( IDS_UNTITLED, &lp ) )
	lstrcpy( szNewExtName, lp );

szFileName[0] = '\0';
if ( !PromptForExtName( hDlg, szNewExtName, szFileName, IDN_HALFTONE ) )
	return( FALSE );
if ( !szFileName[0] )
	return( FALSE );

PutPrivateInt( "Dot",     Halftone.DotShape - IDC_CIRCLEDOT );
PutPrivateFix( "FreqC",   Halftone.ScreenRuling[0] );
PutPrivateFix( "FreqM",   Halftone.ScreenRuling[1] );
PutPrivateFix( "FreqY",   Halftone.ScreenRuling[2] );
PutPrivateFix( "FreqK",   Halftone.ScreenRuling[3] );
PutPrivateFix( "AngleC",  Halftone.ScreenAngle[0] );
PutPrivateFix( "AngleM",  Halftone.ScreenAngle[1] );
PutPrivateFix( "AngleY",  Halftone.ScreenAngle[2] );
PutPrivateFix( "AngleK",  Halftone.ScreenAngle[3] );
PutPrivateInt( "Hilight", Calibrate.Hilight );
PutPrivateInt( "Shadow",  Calibrate.Shadow );

if ( !AddExtFile( szNewExtName, szFileName, IDN_HALFTONE ) )
	{
	FileDelete( szFileName );
	return( FALSE );
	}
lstrcpy( lpExtName, szNewExtName );
return( TRUE );
}


/***********************************************************************/
BOOL SaveSeparateStyle( HWND hDlg, LPSTR lpExtName )
/***********************************************************************/
{
LPSTR lp;
FNAME szFileName, szNewExtName;

lstrcpy( szNewExtName, lpExtName );
if ( StringsEqualID( szNewExtName, IDS_NONE ) && AstralStr( IDS_UNTITLED, &lp ) )
	lstrcpy( szNewExtName, lp );

szFileName[0] = '\0';
if ( !PromptForExtName( hDlg, szNewExtName, szFileName, IDN_SEPARATE ) )
	return( FALSE );
if ( !szFileName[0] )
	return( FALSE );

PutPrivateInt( "MfromR", Separate.MfromR );
PutPrivateInt( "YfromG", Separate.YfromG );
PutPrivateInt( "CfromB", Separate.CfromB );
PutPrivateInt( "UCR",    Separate.UCR );
PutPrivateStr( "BGName", Separate.BGName );

if ( !AddExtFile( szNewExtName, szFileName, IDN_SEPARATE ) )
	{
	FileDelete( szFileName );
	return( FALSE );
	}
lstrcpy( lpExtName, szNewExtName );
return( TRUE );
}


/***********************************************************************/
BOOL SaveCalibrateStyle( HWND hDlg, LPSTR lpExtName )
/***********************************************************************/
{
LPSTR lp;
FNAME szFileName, szNewExtName;

lstrcpy( szNewExtName, lpExtName );
if ( StringsEqualID( szNewExtName, IDS_NONE ) && AstralStr( IDS_UNTITLED, &lp ) )
	lstrcpy( szNewExtName, lp );

szFileName[0] = '\0';
if ( !PromptForExtName( hDlg, szNewExtName, szFileName, IDN_CALMAP ) )
	return( FALSE );
if ( !szFileName[0] )
	return( FALSE );

if ( !SaveMap( &Calibrate.CalMap[CAL_MASTER], &Calibrate.CalMap[CAL_CYAN],
		&Calibrate.CalMap[CAL_MAGENTA],	&Calibrate.CalMap[CAL_YELLOW],
		&Calibrate.CalMap[CAL_BLACK], szFileName ) )
		return( FALSE );
if ( !AddExtFile( szNewExtName, szFileName, IDN_CALMAP ) )
	{
	FileDelete( szFileName );
	return( FALSE );
	}
lstrcpy( lpExtName, szNewExtName );
Calibrate.DoCalibration = TRUE;
return( TRUE );
}


/***********************************************************************/
BOOL SaveScanCalibrate( HWND hDlg, LPSTR lpExtName )
/***********************************************************************/
{
LPSTR lp;
FNAME szFileName, szNewExtName;

lstrcpy( szNewExtName, lpExtName );
if ( StringsEqualID( szNewExtName, IDS_NONE ) && AstralStr( IDS_UNTITLED, &lp ) )
	lstrcpy( szNewExtName, lp );

szFileName[0] = '\0';
if ( !PromptForExtName( hDlg, szNewExtName, szFileName, IDN_SCANMAP ) )
	return( FALSE );
if ( !szFileName[0] )
	return( FALSE );

if ( !SaveMap( &Scans.CalMap[CALSCAN_GRAY], &Scans.CalMap[CALSCAN_RED], &Scans.CalMap[CALSCAN_GREEN],
	&Scans.CalMap[CALSCAN_BLUE], szFileName ) )
		return( FALSE );

if ( !AddExtFile( szNewExtName, szFileName, IDN_SCANMAP ) )
	{
	FileDelete( szFileName );
	return( FALSE );
	}
lstrcpy( lpExtName, szNewExtName );
return( TRUE );
}
