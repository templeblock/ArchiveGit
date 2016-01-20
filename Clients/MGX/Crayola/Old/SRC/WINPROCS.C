//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include <time.h>
#include "id.h"
#include "data.h"
#include "routines.h"

//char * itoa (int, char *, int);
//int atoi (char *);

extern HINSTANCE hInstAstral;
extern HWND hWndAstral;
extern SAVE Save;

/***********************************************************************/
BOOL WINPROC EXPORT DlgComboProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
RECT rect;

switch (msg)
    {
    case WM_INITDIALOG:
	// Get the combo box height from a hidden text item
	GetWindowRect( GetDlgItem(hDlg, IDOK), &rect );
	Control.ComboHeight = RectHeight( &rect );
	AstralDlgEnd( hDlg, TRUE );
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
BOOL WINPROC EXPORT DlgHeraldProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
switch (msg)
    {
    case WM_INITDIALOG:
	CenterPopup( hDlg );
//	SetCapture( hDlg );
	break;

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

//	case WM_KEYDOWN:
//	case WM_LBUTTONDOWN:
//	case WM_RBUTTONDOWN:
//	ReleaseCapture();
//	AstralDlgEnd( hDlg, TRUE );
//	break;

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
void UpdateStatusBar( BOOL bDummy1, BOOL bDummy2, BOOL bMask, BOOL bShield )
/***********************************************************************/
{
}


/***********************************************************************/
BOOL WINPROC EXPORT DlgWarningProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
switch (msg)
    {
    case WM_INITDIALOG:
	SET_CLASS_HBRBACKGROUND(hDlg, ( HBRUSH )GetStockObject(LTGRAY_BRUSH));
	CenterPopup( hDlg );
	break;

    case WM_CLOSE:
	AstralDlgEnd( hDlg, NO );
	break;

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_COMMAND:
	switch (wParam)
	    {
	    case IDOK:
		AstralDlgEnd( hDlg, YES );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, NO );
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

/***********************************************************************/
VOID DoSizeInits( HWND hDlg )
/***********************************************************************/
{
LPFRAME lpFrame;

/* Be careful to set all values before doing a SetDlg... with them */
/* The Edit structure is a temorary holding area for sizing dialog boxes */
Edit.SaveUnits = Control.Units;
Edit.Distortable = NO;
Edit.ScaleX = Edit.ScaleY = 100;
if ( lpImage )
	{
	lpFrame = ImgGetBaseEditFrame(lpImage);
	Edit.Crop.left = Edit.Crop.top = 0;
	Edit.Crop.right = lpImage->npix - 1;
	Edit.Crop.bottom = lpImage->nlin - 1;
	Edit.Resolution = Edit.CropResolution = Edit.OrigResolution =
				FrameResolution(lpFrame);
	Edit.Depth = FrameDepth(lpFrame);
	}
else
	{
	Edit.Resolution = Edit.CropResolution = 75;
	Edit.Depth = 1;
	Edit.MaintainSize = NO;
	}
Edit.Angle = 0;
Edit.Width  = FGET( CROP_WIDTH, Edit.CropResolution );
Edit.Height = FGET( CROP_HEIGHT, Edit.CropResolution );
CheckDlgButton( hDlg, IDC_DISTORT, Edit.Distortable);
CheckDlgButton( hDlg, IDC_SMARTSIZE, Edit.SmartSize );
CheckDlgButton( hDlg, IDC_MAINTAINSIZE, Edit.MaintainSize );
ControlEnable(hDlg, IDC_DISTORT, !Edit.MaintainSize);
ControlEnable(hDlg, IDC_MAINTAINSIZE, !Edit.Distortable );
ControlEnable( hDlg, IDC_SMARTSIZE, !Edit.Distortable );
InitDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES, 0L, FGET(9999,1) );
InitDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES, 0L, FGET(9999,1) );
InitDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, YES, 1, 10000 );
InitDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, YES, 1, 10000 );
InitDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO, 1, 10000 );
ImageMemory( hDlg, IDC_MEMORY, Edit.Width, Edit.Height, Edit.Resolution,
	Edit.Depth );
CheckComboItem(hDlg, IDC_PREF_UNITS, IDC_PREF_UNITFIRST,
	IDC_PREF_UNITLAST, Control.Units);
}


/***********************************************************************/
void ImageMemory(
/***********************************************************************/
HWND 	hDlg,
ITEMID 	idControl,
LFIXED 	fWidth,
LFIXED	fHeight,
int 	iResolution,
int		iDepth)
{
DWORD pix, lin, dwSize;
STRING szString;
BOOL bOverflow;

if ( iDepth == 2 )
	iDepth = 3;
pix = FMUL( iResolution, fWidth );
lin = FMUL( iResolution, fHeight );
if (iDepth > 0)
	{
	dwSize = pix * lin * iDepth;
	bOverflow = ( pix && ((dwSize / pix) != (lin * iDepth)) );
	}
else
	{
	dwSize = (pix * lin) / 8;
	bOverflow = ( pix && ((dwSize / pix) != (lin / 8)) );
	}

if ( !bOverflow && // if no overflow....
     pix < 32768L && lin < 32768L ) // and valid width and height...
	{
	SizeToAscii( dwSize, szString );
	SetDlgItemText( hDlg, idControl, szString );
	}
else
	SetDlgItemText( hDlg, idControl, "*******" );
}


/***********************************************************************/
VOID DoSizeControls(
/***********************************************************************/
HWND 	hDlg,
WPARAM 	wParam,
LPARAM 	lParam,
BOOL 	bResAdjust)
{
BOOL Bool;
long x, y;
int npix, nlin;
HWND hWnd, hControl;
WORD idUnits;
RECT rArea;
LFIXED scale;

npix = CROP_WIDTH;
nlin = CROP_HEIGHT;
switch( wParam )
    {
	case IDC_MAINTAINSIZE:
	Edit.MaintainSize = !Edit.MaintainSize;
	CheckDlgButton( hDlg, IDC_MAINTAINSIZE, Edit.MaintainSize );
	if (Edit.MaintainSize)
		{
		Edit.Resolution = Edit.CropResolution = Edit.OrigResolution;
		Edit.Width  = FGET( npix, Edit.CropResolution );
		Edit.Height = FGET( nlin, Edit.CropResolution );
		SetUnitResolution( Edit.CropResolution );
		SetDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO );
		SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
		SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
		}
	Bool = !Edit.MaintainSize || Control.Units != IDC_PREF_UNITPIXELS;
	ControlEnable(hDlg, IDC_WIDTH, Bool);
	ControlEnable(hDlg, IDC_HEIGHT, Bool);
	if (Edit.MaintainSize)
		Edit.Distortable = NO;
	ControlEnable(hDlg, IDC_DISTORT, !Edit.MaintainSize);
	break;

	case IDC_PREF_UNITS:
	if ( !(idUnits = HandleCombo( hDlg, wParam, lParam )) )
		break;
	Control.Units = idUnits;
	CheckComboItem(hDlg, IDC_PREF_UNITS, IDC_PREF_UNITFIRST,
		IDC_PREF_UNITLAST, Control.Units);
	if (Edit.MaintainSize && Control.Units == IDC_PREF_UNITPIXELS)
		{
		Edit.CropResolution = Edit.Resolution;
		SetUnitResolution( Edit.CropResolution );
		Edit.Width  = FGET( npix, Edit.CropResolution );
		Edit.Height = FGET( nlin, Edit.CropResolution );
		y = FMUL( (100L*Edit.CropResolution), Edit.Height );
		Edit.ScaleY = (y + nlin/2) / nlin;
		SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
		x = FMUL( (100L*Edit.CropResolution), Edit.Width );
		Edit.ScaleX = (x + npix/2) / npix;
		SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
		}
	SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
	SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
	Bool = !Edit.MaintainSize || Control.Units != IDC_PREF_UNITPIXELS;
	ControlEnable(hDlg, IDC_WIDTH, Bool);
	ControlEnable(hDlg, IDC_HEIGHT, Bool);
	ControlEnable(hDlg, IDC_SCALEX, Bool);
	ControlEnable(hDlg, IDC_SCALEY, Bool);
	break;

    case IDC_DISTORT:
	Edit.Distortable = !Edit.Distortable;
	CheckDlgButton( hDlg, IDC_DISTORT, Edit.Distortable );
	if (Edit.Distortable)
		Edit.MaintainSize = NO;
	ControlEnable(hDlg, IDC_MAINTAINSIZE, !Edit.MaintainSize);
	if ( Edit.ScaleX != Edit.ScaleY )
		{
		Edit.ScaleX = Edit.ScaleY;
		x = ((long)npix * Edit.ScaleX);
		Edit.Width  = FGET( x, 100L*Edit.CropResolution );
		SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
		SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
		}
	CheckDlgButton( hDlg,IDC_SMARTSIZE,Edit.SmartSize && !Edit.Distortable);
	ControlEnable( hDlg, IDC_SMARTSIZE, !Edit.Distortable );
	ControlEnable(hDlg, IDC_MAINTAINSIZE, !Edit.Distortable && lpImage);
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
	if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		break;
	if ( HIWORD(lParam) != EN_CHANGE )
		break;
	Edit.Height = GetDlgItemSpinFixed( hDlg, IDC_HEIGHT, &Bool, YES);
	if (Edit.MaintainSize)
		{
		scale = FGET(UNITY, Edit.Height);
		Edit.Resolution = FMUL(nlin, scale);
		SetUnitResolution( Edit.Resolution );
		SetDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO );
		}
	// y = number of new pixels x 100
	y = FMUL( (100L*Edit.CropResolution), Edit.Height );
	// calculate scale factor = (new pixels * 100) / old pixels
	Edit.ScaleY = (y + nlin/2) / nlin;
	SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
	if ( !Edit.Distortable )
		{
		Edit.ScaleX = Edit.ScaleY;
		x = ((long)npix * Edit.ScaleX);
		Edit.Width  = FGET( x, 100L*Edit.CropResolution );
		SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
		SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
		}
	ImageMemory( hDlg, IDC_MEMORY,
			Edit.MaintainSize ? FGET(npix,1) : Edit.Width,
			Edit.MaintainSize ? FGET(nlin,1) : Edit.Height,
			Edit.MaintainSize ? 1 : Edit.Resolution, Edit.Depth );
	break;

    case IDC_WIDTH:
	if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		break;
	if ( HIWORD(lParam) != EN_CHANGE )
		break;
	Edit.Width = GetDlgItemSpinFixed( hDlg, IDC_WIDTH, &Bool, YES );
	if (Edit.MaintainSize)
		{
		scale = FGET(UNITY, Edit.Width);
		Edit.Resolution = FMUL(npix, scale);
		SetUnitResolution( Edit.Resolution );
		SetDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO );
		}
	x = FMUL( (100L*Edit.CropResolution), Edit.Width );
	Edit.ScaleX = (x + npix/2) / npix;
	SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
	if ( !Edit.Distortable )
		{
		Edit.ScaleY = Edit.ScaleX;
		y = ((long)nlin * Edit.ScaleY);
		Edit.Height  = FGET( y, 100L*Edit.CropResolution );
		SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
		SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
		}
	ImageMemory( hDlg, IDC_MEMORY,
			Edit.MaintainSize ? FGET(npix,1) : Edit.Width,
			Edit.MaintainSize ? FGET(nlin,1) : Edit.Height,
			Edit.MaintainSize ? 1 : Edit.Resolution, Edit.Depth );
	break;

    case IDC_RES:
	if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		break;
	if ( HIWORD(lParam) != EN_CHANGE )
		break;
	Edit.Resolution = GetDlgItemSpin( hDlg, IDC_RES, &Bool, NO );
	if ( Edit.Resolution <= 0 )
		Edit.Resolution = 2;
	if (Edit.MaintainSize)
		{
		Edit.Width = FGET(npix, Edit.Resolution);
		Edit.Height = FGET(nlin, Edit.Resolution);
		SetUnitResolution( Edit.Resolution );
		if (Control.Units != IDC_PREF_UNITPIXELS)
			{
			SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
			SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
			y = FMUL( (100L*Edit.CropResolution), Edit.Height );
			Edit.ScaleY = (y + nlin/2) / nlin;
			SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
			x = FMUL( (100L*Edit.CropResolution), Edit.Width );
			Edit.ScaleX = (x + npix/2) / npix;
			SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
			}
		}
	
	if ( Control.Units == IDC_PREF_UNITPIXELS )
	    {
	    if ( bResAdjust )
			{ // Extract the values again (inch values changed)
			Edit.CropResolution = Edit.Resolution;
			SetUnitResolution( Edit.Resolution );
			SetFocus( hWnd = GetDlgItem( hDlg, IDC_HEIGHT ) );
			SendMessage( hDlg, WM_COMMAND, IDC_HEIGHT,
			MAKELONG(hWnd,EN_CHANGE) );
			SetFocus( hWnd = GetDlgItem( hDlg, IDC_WIDTH ) );
			SendMessage( hDlg, WM_COMMAND, IDC_WIDTH,
				MAKELONG(hWnd,EN_CHANGE) );
			SetFocus( GetDlgItem( hDlg, wParam ) );
			}
	    else
			{ // Change the edit box
			SetUnitResolution( Edit.Resolution );
			SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES);
			SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
			}
	    }
	ImageMemory( hDlg, IDC_MEMORY,
			Edit.MaintainSize ? FGET(npix,1) : Edit.Width,
			Edit.MaintainSize ? FGET(nlin,1) : Edit.Height,
			Edit.MaintainSize ? 1 : Edit.Resolution, Edit.Depth );
	break;

    case IDC_SCALEX:
	if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		break;
	if ( HIWORD(lParam) != EN_CHANGE )
		break;
	Edit.ScaleX = GetDlgItemSpin( hDlg, IDC_SCALEX, &Bool, NO );
	x = ((long)npix * Edit.ScaleX);
	Edit.Width  = FGET( x, 100L*Edit.CropResolution );
	SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
	if (Edit.MaintainSize)
		{
		scale = FGET(UNITY, Edit.Width);
		Edit.Resolution = FMUL(npix, scale);
		SetUnitResolution( Edit.Resolution );
		SetDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO );
		}
	if ( !Edit.Distortable )
		{
		Edit.ScaleY = Edit.ScaleX;
		y = ((long)nlin * Edit.ScaleY);
		Edit.Height  = FGET( y, 100L*Edit.CropResolution );
		SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
		SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
		}
	ImageMemory( hDlg, IDC_MEMORY,
			Edit.MaintainSize ? FGET(npix,1) : Edit.Width,
			Edit.MaintainSize ? FGET(nlin,1) : Edit.Height,
			Edit.MaintainSize ? 1 : Edit.Resolution, Edit.Depth );
	break;

    case IDC_SCALEY:
	if ( GetFocus() != GetDlgItem( hDlg, wParam ) )
		break;
	if ( HIWORD(lParam) != EN_CHANGE )
		break;
	Edit.ScaleY = GetDlgItemSpin( hDlg, IDC_SCALEY, &Bool, NO );
	y = ((long)nlin * Edit.ScaleY);
	Edit.Height = FGET( y, 100L*Edit.CropResolution );
	SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
	if (Edit.MaintainSize)
		{
		scale = FGET(UNITY, Edit.Height);
		Edit.Resolution = FMUL(nlin, scale);
		SetUnitResolution( Edit.Resolution );
		SetDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO );
		}
	if ( !Edit.Distortable )
		{
		Edit.ScaleX = Edit.ScaleY;
		x = ((long)npix * Edit.ScaleX);
		Edit.Width  = FGET( x, 100L*Edit.CropResolution );
		SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES);
		SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
		}
	ImageMemory( hDlg, IDC_MEMORY,
			Edit.MaintainSize ? FGET(npix,1) : Edit.Width,
			Edit.MaintainSize ? FGET(nlin,1) : Edit.Height,
			Edit.MaintainSize ? 1 : Edit.Resolution, Edit.Depth );
	break;

    case IDC_MEMORY:
    default:
	break;
    }
}


/***********************************************************************/
BOOL WINPROC EXPORT DlgStatusBoxProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{
HDC hDC;
PAINTSTRUCT ps;
static HFONT hFont;

switch (msg)
    {
    case WM_INITDIALOG:
	SET_CLASS_HBRBACKGROUND(hDlg, ( HBRUSH )GetStockObject(LTGRAY_BRUSH));
	SetWindowPos( hDlg, HWND_TOPMOST,
		0, 0, /* Same location */
		0, 0, /* Same size */
		SWP_NOMOVE | SWP_NOSIZE /* Flags */ );
		hFont = CreateFont ( -35,			// Height
								0,				// Width
								0,				// Escapement
								0,				// Orientation
								0,				// Weight
								0,				// Italic
								0,				// Underline
								0,				// Strikeout
								0,				// Charset
								0,				// OutputPrecision
								0,				// ClipPrecision
								0,				// Quality
								0,				// PitchAndFamily
	  							"Arial" );		// Typeface Name
		if ( hFont )
			{
			SendDlgItemMessage( hDlg, IDC_STATUSTEXT, WM_SETFONT, (WPARAM)hFont, 0L );
			MessageBeep(0);
			}
	break;

    case WM_LBUTTONDBLCLK:
    case WM_CLOSE:
	AstralDlgEnd( hDlg, FALSE );
	break;

    case WM_PAINT:
	hDC = BeginPaint( hDlg, &ps );
	EndPaint( hDlg, &ps );
	MessageStatus( NULL );
	break;

    case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );

    case WM_NCHITTEST:
	return( HandleNonClientHit( hDlg, msg, wParam, lParam ));

    case WM_DESTROY:
	if ( hFont )
		DeleteObject( hFont );
	break;

    case WM_COMMAND:
//	switch (wParam)
//	    {
//	    default:
//		return( FALSE );
//	    }
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
static long HandleNonClientHit( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
RECT Rect, At;
long lReturn;
int x, y, dx, dy;

lReturn = DefWindowProc( hWindow, message, wParam, lParam );
if ( lReturn != HTCLIENT ) // Return all non-client hits as is
	return( lReturn );
x = LOWORD( lParam );
y = HIWORD( lParam );
dx = 10;
dy = 10;
GetWindowRect( hWindow, &Rect );
// Turn client hits into moves and size hits
At.left   = ( abs( x - Rect.left   ) < dx );
At.right  = ( abs( x - Rect.right  ) < dx );
At.top    = ( abs( y - Rect.top    ) < dy );
At.bottom = ( abs( y - Rect.bottom ) < dy );

if ( At.left )
	{
	if ( At.top )		lReturn = HTTOPLEFT; else
	if ( At.bottom )	lReturn = HTBOTTOMLEFT; else
						lReturn = HTLEFT;
	}
else
if ( At.right )
	{
	if ( At.top )		lReturn = HTTOPRIGHT; else
	if ( At.bottom )	lReturn = HTBOTTOMRIGHT; else
						lReturn = HTRIGHT;
	}
else
	{
	if ( At.top )		lReturn = HTTOP; else
	if ( At.bottom )	lReturn = HTBOTTOM; else
						lReturn = HTCAPTION;
	}
return( lReturn );
}

/***********************************************************************/
BOOL WINPROC EXPORT DlgPrefProc(
/***********************************************************************/
HWND 	hDlg,
UINT 	msg,
WPARAM 	wParam,
LPARAM 	lParam)
{

STRING	szBuff;
int		iTmp;

switch (msg)
    {
    case WM_INITDIALOG:
	SET_CLASS_HBRBACKGROUND(hDlg, ( HBRUSH )GetStockObject(LTGRAY_BRUSH));

	clr ((LPTR)szBuff, sizeof(STRING));
	SendDlgItemMessage (hDlg, IDC_PREF_NOTHEME, BM_SETCHECK,
	  Control.bNoTheme, 0L);
	SendDlgItemMessage (hDlg, IDC_PREF_UNDO, BM_SETCHECK,
	  Control.NoUndo, 0L);

	GetPrivateProfileString( "Drivers", "Wave", "none", szBuff,
	  sizeof(STRING), "system.ini");
	// if no wave driver or the speaker.drv installed, disable checkbox
#ifdef USEWAVEMIX
	if ( (! lstrcmpi( szBuff, "none")) ||
	     (! lstrcmpi( szBuff, "speaker.drv")) ||
	     (! lstrcmpi( szBuff, "")) )
	{
		SendDlgItemMessage (hDlg, IDC_PREF_USEWAVEMIX, BM_SETCHECK,
	  	  TRUE, 0L);
		EnableWindow ( GetDlgItem (hDlg, IDC_PREF_USEWAVEMIX), FALSE);
	}
	else
		SendDlgItemMessage (hDlg, IDC_PREF_USEWAVEMIX, BM_SETCHECK,
	  	  !Control.bUseWaveMix, 0L);
#endif // USEWAVEMIX		  

	SendDlgItemMessage (hDlg, IDC_PREF_SAVER, EM_LIMITTEXT, 3, 0L);
	SendDlgItemMessage (hDlg, IDC_PREF_ANIMATIONS, EM_LIMITTEXT, 3, 0L);
	itoa (Control.iScrnSaverTime, szBuff, 10);
	SetDlgItemText (hDlg, IDC_PREF_SAVER, szBuff);
	itoa (Control.iMaxAnimations, szBuff, 10);
	SetDlgItemText (hDlg, IDC_PREF_ANIMATIONS, szBuff);
#ifdef ADVENT
	CheckRadioButton (hDlg, IDC_PREF_SMALL, IDC_PREF_LARGE,
		Control.iAdventBrush + IDC_PREF_SMALL);
#endif
//SMS - commented out in case we decide to do something with this
//#ifdef CDBUILD
//	if (Control.bBubbleHints && Control.bAudioHints)
//		iTmp = 2;
//	else if (Control.bBubbleHints && ! Control.bAudioHints)
//		iTmp = 0;
//	else
//		iTmp = 1;
//	CheckRadioButton (hDlg, IDC_PREF_BUBBLE, IDC_PREF_BOTH,
//		iTmp + IDC_PREF_BUBBLE);
//#endif
	CenterPopup( hDlg );
	break;

    case WM_SETCURSOR:
	return( SetupCursor( wParam, lParam, IDD_PREF ) );

    case WM_CLOSE:
	AstralDlgEnd( hDlg, NO );
	break;

	case WM_MEASUREITEM:
	case WM_DRAWITEM:
	return( OwnerDraw( hDlg, msg, lParam, NO ) );

	case WM_CTLCOLOR:
	return( (BOOL)SetControlColors( (HDC)wParam, hDlg, (HWND)LOWORD(lParam),
		HIWORD(lParam) ) );
	break;

	case WM_COMMAND:

	switch (wParam)
	    {
#ifdef ADVENT
		case IDC_PREF_SMALL:
		case IDC_PREF_MEDIUM:
		case IDC_PREF_LARGE:
		Control.iAdventBrush = wParam - IDC_PREF_SMALL;
		CheckRadioButton (hDlg, IDC_PREF_SMALL, IDC_PREF_LARGE, wParam);
		break;
#endif
//SMS - commented out in case we decide to do something with this
//#ifdef CDBUILD
//		case IDC_PREF_BUBBLE:
//		case IDC_PREF_AUDIO:
//		case IDC_PREF_BOTH:
//		iTmp = wParam - IDC_PREF_BUBBLE;
//		if (iTmp == 0)
//		{
//			Control.bBubbleHints = TRUE;
//			Control.bAudioHints = FALSE;
//		}
//		else if (iTmp == 1)
//		{
//			Control.bBubbleHints = FALSE;
//			Control.bAudioHints = TRUE;
//		}
//		else
//		{
//			Control.bBubbleHints = TRUE;
//			Control.bAudioHints = TRUE;
//		}
//		CheckRadioButton (hDlg, IDC_PREF_BUBBLE, IDC_PREF_BOTH, wParam);
//		break;
//#endif
	    case IDC_PREF_SAVE:
	    case IDOK:
		Control.bNoTheme = (BOOL)SendDlgItemMessage (hDlg, IDC_PREF_NOTHEME,
		  BM_GETCHECK, 0, 0L);
		Control.NoUndo = (int)SendDlgItemMessage (hDlg, IDC_PREF_UNDO,
		  BM_GETCHECK, 0, 0L);
#ifdef USEWAVEMIX
		Control.bUseWaveMix = !(BOOL)SendDlgItemMessage (hDlg, IDC_PREF_USEWAVEMIX,
		  BM_GETCHECK, 0, 0L);
#endif // USEWAVEMIX

		clr ((LPTR)szBuff, sizeof(STRING));
		GetDlgItemText (hDlg, IDC_PREF_SAVER, szBuff, sizeof(szBuff));
		iTmp = atoi (szBuff);
		if (iTmp < 1 || iTmp > 999)
		{
			SetFocus (GetDlgItem (hDlg, IDC_PREF_SAVER));
			return FALSE;
		}
		Control.iScrnSaverTime = iTmp;
		IntermissionSetTimeout ((DWORD)iTmp * 60000);

		GetDlgItemText (hDlg, IDC_PREF_ANIMATIONS, szBuff, sizeof(szBuff));
		iTmp = atoi (szBuff);
		if (iTmp < 1 || iTmp > 30)
		{
			SetFocus (GetDlgItem (hDlg, IDC_PREF_ANIMATIONS));
			return FALSE;
		}
		Control.iMaxAnimations = iTmp;

		if (wParam == IDC_PREF_SAVE)
			SavePreferences();

		AstralDlgEnd( hDlg, YES );
		break;

	    case IDCANCEL:
		AstralDlgEnd( hDlg, YES );
		break;
    	}
	break;

    default:
	return( FALSE );
    }

return( TRUE );
}


/***********************************************************************/
void DisplayInfo(long lParam, LPRECT lpRect)
/***********************************************************************/
{
}

