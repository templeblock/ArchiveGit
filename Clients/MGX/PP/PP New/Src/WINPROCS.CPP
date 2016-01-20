//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "bsrutil.h"
#include <time.h>
#include "id.h"
#include "ppafx.h"
#include "pickpal.h"
#include "svrdoc.h"
#include "svrview.h"
#include "ipframe.h"
#include "browser.h"
#include "ctlutil.h"
extern "C"
{
#include "dyndlg.h"
}

// DEFINE the Following to have Gray Chiseled Dialog Look
#define CHISELED


// LOCAL prototypes
LOCAL BOOL Combo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);

/***********************************************************************/
BOOL WINPROC EXPORT DlgComboProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Combo_OnInitDialog);  

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, 0);
	}
}

/************************************************************************/
LOCAL BOOL Combo_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
RECT rect;

// Get the combo box height from a hidden text item
GetWindowRect( GetDlgItem(hDlg, IDOK), &rect );
Control.ComboHeight = RectHeight( &rect );
ShowWindow(hDlg, SW_HIDE); // make sure its invisible
AstralDlgEnd( hDlg, TRUE );
return(TRUE);
}

// LOCAL prototypes
LOCAL BOOL Herald_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);

/***********************************************************************/
BOOL WINPROC EXPORT DlgHeraldProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Herald_OnInitDialog);  

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_HERALD);
	}
}

/************************************************************************/
LOCAL BOOL Herald_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
STRING szString;

CenterPopup( hDlg );
if (GetDlgItem(hDlg, IDC_SERIALNO))
	{
	GetDefaultString( "SN", "", szString, sizeof(szString) );
	SetDlgItemText( hDlg, IDC_SERIALNO, szString );
	}

GetDefaultString( "User", "", szString, sizeof(szString) );
SetDlgItemText( hDlg, IDC_CUSTOMER, szString );

GetDefaultString( "Company", "", szString, sizeof(szString) );
SetDlgItemText( hDlg, IDC_COMPANY, szString );
return(TRUE);
}

// LOCAL prototypes
LOCAL BOOL Status_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Status_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Status_OnPaint(HWND hDlg);
LOCAL void Status_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);

/***********************************************************************/
BOOL WINPROC EXPORT DlgStatusProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Status_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Status_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_PAINT, Status_OnPaint);
	HANDLE_MSG(hDlg, WM_LBUTTONDOWN, Status_OnLButtonDown);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_STATUS);
	}
}

/***********************************************************************/
LOCAL void Status_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
FORWARD_WM_LBUTTONDOWN(GetParent(hWindow), fDoubleClick, x, y, keyFlags, SendMessage );
}

/************************************************************************/
LOCAL BOOL Status_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
return(TRUE);
}

/***********************************************************************/
LOCAL void Status_OnPaint(HWND hDlg)
/***********************************************************************/
{
PAINTSTRUCT ps;
HDC hDC;

hDC = BeginPaint( hDlg, &ps );
EndPaint( hDlg, &ps );
MessageStatus( 0 );
}

void OnStatusMask()
   {
   LPIMAGE lpImage;

	lpImage = GetActiveImage();
	if (lpImage)
		{
		CMD_PARMS parms;

		DeactivateTool();
		ProcessCommand(lpImage->lpCmdList, IDS_CMD_INVERTMASK, &parms);
		}
   }
extern void OnStatusInfo();
extern void OnStatusShield(int id);
extern void OnStatusPalette(int id);
extern void OnStatusMaskEditAlpha(int id);
extern void OnViewSlime(int id);

/************************************************************************/
LOCAL void Status_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
    case IDC_STATUSINFO:
    OnStatusInfo();
	break;

	case IDC_STATUSSHIELD:
    OnStatusShield(id);
	break;

	case IDC_STATUSPALETTE:
    OnStatusPalette(id);
	break;

	case IDC_STATUSMASK:
    OnStatusMask();
	break;

	case IDC_MASKEDITALPHA:
    if( Control.Retail )
        OnStatusMaskEditAlpha(id);
	break;

	case IDC_VIEWSLIME:
    OnViewSlime(id);
	break;

    default:
	break;
    }
}

/***********************************************************************/
void UpdateStatusBar( BOOL bDummy1, BOOL bDummy2, BOOL bMask, BOOL bShield )
/***********************************************************************/
{
HWND hStatus, hWnd;
ITEMID id;
LPSTR lpString;
BOOL Bool;
LPMASK lpMask;
LPIMAGE lpImage;

if ( !(hStatus = AstralDlgGet(IDD_STATUS)) )
	return;

lpImage = GetActiveImage();
if ( bMask && (hWnd = GetDlgItem( hStatus, IDC_STATUSMASK )) )
	{
	lpMask = ImgGetMask( lpImage );
	id = ( ( lpMask && lpMask->bInvertState) ? IDS_OUTSIDE : IDS_INSIDE);
	if ( AstralStr( id, &lpString ) )
		SetWindowText( hWnd, lpString );
	}
if ( bShield && (hWnd = GetDlgItem( hStatus, IDC_STATUSSHIELD )) )
	{
	id = (ColorMask.On ? IDS_ON : IDS_OFF);
	if ( AstralStr( id, &lpString ) )
		SetWindowText( hWnd, lpString );
	}
// Set the info icon
ControlEnable( hStatus, IDC_STATUSINFO, lpImage != NULL );

// Set the mask invert icon
if (GetDlgItem(hStatus, IDC_STATUSMASK))
	{
	lpMask = ImgGetMask( lpImage );
	ControlEnable( hStatus, IDC_STATUSMASK, lpMask != NULL );
	Bool = ( lpMask && lpMask->bInvertState );
	CheckDlgButton( hStatus, IDC_STATUSMASK, Bool );
	}

// Set the mask edit alpha icon
if (GetDlgItem(hStatus, IDC_MASKEDITALPHA))
	{
	ControlEnable( hStatus, IDC_MASKEDITALPHA, lpImage != NULL );
	CheckDlgButton( hStatus, IDC_MASKEDITALPHA,
		lpImage && ImgInMaskEditMode(lpImage) );
	}

// Set the view slime icon
ControlEnable( hStatus, IDC_VIEWSLIME, lpImage != NULL );
CheckDlgButton( hStatus, IDC_VIEWSLIME,
	lpImage && (ImgGetViewMode(lpImage) == VM_SLIME) );

// Set the shield icon
hWnd = AstralDlgGet( IDD_SHIELDFLOAT );
Bool = ( hWnd && IsWindowVisible( hWnd ) );
CheckDlgButton( hStatus, IDC_STATUSSHIELD, Bool );

// Set the palette icon
hWnd = AstralDlgGet( IDD_PALETTE );
Bool = ( hWnd && IsWindowVisible( hWnd ) );
CheckDlgButton( hStatus, IDC_STATUSPALETTE, Bool );
}


// LOCAL prototypes
LOCAL BOOL Warning_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Warning_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgWarningProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Warning_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Warning_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_WARNING);
	}
}

/************************************************************************/
LOCAL BOOL Warning_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
return(TRUE);
}

/************************************************************************/
LOCAL void Warning_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDOK:
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL TrialDaysLeft_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void TrialDaysLeft_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
/***********************************************************************/
BOOL WINPROC EXPORT DlgTrialDaysLeftProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam )
/***********************************************************************/
{
    switch (msg)
    {
        HANDLE_DLGRET(hDlg, WM_INITDIALOG, TrialDaysLeft_OnInitDialog);  
	    HANDLE_DLGMSG(hDlg, WM_COMMAND, TrialDaysLeft_OnCommand);

	    default:
	    return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_TRIALDAYSLEFT );
   }
}

/************************************************************************/
LOCAL BOOL TrialDaysLeft_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
    char szBuffer[512];
    char szFormat[256];

    CenterPopup( hDlg );

    if( AstralStrEx( IDS_TRIALDAYSLEFT, szFormat, sizeof( szFormat )))
    {
        wsprintf( szBuffer, szFormat, Control.wDaysLeft );
        SetDlgItemText( hDlg, IDC_TRIALDAYSLEFT, szBuffer );
    }
    return( TRUE );
}

/************************************************************************/
LOCAL void TrialDaysLeft_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
    switch (id)
	{
	    case IDOK:
    	    AstralDlgEnd( hDlg, TRUE );
        default:
	        break;
    }
}

LOCAL BOOL TrialExpired_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void TrialExpired_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
/***********************************************************************/
BOOL WINPROC EXPORT DlgTrialExpiredProc( HWND hDlg, UINT msg, WPARAM wParam,
                              LPARAM lParam )
/***********************************************************************/
{
    switch (msg)
	{
	    HANDLE_DLGRET(hDlg, WM_INITDIALOG, TrialExpired_OnInitDialog);  
	    HANDLE_DLGMSG(hDlg, WM_COMMAND, TrialExpired_OnCommand);

	    default:
    	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_TRIALEXPIRED);
	}
}

/************************************************************************/
LOCAL BOOL TrialExpired_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
    CenterPopup( hDlg );
    return(TRUE);
}

/************************************************************************/
LOCAL void TrialExpired_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
    switch (id)
    {
	    case IDOK:
    	    AstralDlgEnd( hDlg, TRUE );
        default:
            break;
    }
}

/***********************************************************************/
void ImageMemory( HWND hDlg, ITEMID idControl, LFIXED fWidth,
                  LFIXED fHeight, int iResolution, int iDepth )
/***********************************************************************/
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
     pix <= MAX_IMAGE_WIDTH && lin <= MAX_IMAGE_HEIGHT ) // and valid width and height...
	{
	SizeToAscii( dwSize, szString );
	SetDlgItemText( hDlg, idControl, szString );
	}
else
	SetDlgItemText( hDlg, idControl, "*******" );
}


///***********************************************************************/
//VOID DoSizeControls( HWND hDlg, ITEMID id, UINT codeNotify, BOOL bResAdjust )
///***********************************************************************/
//{
//    BOOL Bool;
//    long x, y;
//    int npix, nlin;
//    HWND hWnd, hControl;
//    WORD idUnits;
//    RECT rArea;
//    LFIXED scale;
//    LPIMAGE lpImage;
//    FRMDATATYPE Type;
//
//    lpImage = GetActiveImage();
//    ImgGetInfo(lpImage, NULL, NULL, NULL, &Type);
//    npix = CROP_WIDTH;
//    nlin = CROP_HEIGHT;
//
//    switch( id )
//    {
//	    case IDC_SMARTSIZE:
//	        Edit.SmartSize = !Edit.SmartSize;
//	        CheckDlgButton( hDlg, IDC_SMARTSIZE, Edit.SmartSize );
//        break;
//
//	    case IDC_MAINTAINSIZE:
//	        Edit.MaintainSize = !Edit.MaintainSize;
//	        if (Edit.MaintainSize)
//		        Edit.Distortable = NO;
//
//	        ControlEnable(hDlg, IDC_DISTORT, !Edit.MaintainSize);
//	        ControlEnable(hDlg, IDC_SMARTSIZE, !Edit.MaintainSize);
//	        Bool = !Edit.MaintainSize || Control.Units != IDC_PREF_UNITPIXELS;
//	        ControlEnable(hDlg, IDC_WIDTH, Bool);
//	        ControlEnable(hDlg, IDC_HEIGHT, Bool);
//	        ControlEnable(hDlg, IDC_SCALEX, Bool);
//	        ControlEnable(hDlg, IDC_SCALEY, Bool);
//
//	        if (Edit.MaintainSize)
//	        {
//		        Edit.Resolution = Edit.CropResolution = Edit.OrigResolution;
//		        Edit.Width  = FGET( npix, Edit.CropResolution );
//		        Edit.Height = FGET( nlin, Edit.CropResolution );
//		        SetUnitResolution( Edit.CropResolution );
//		        SetDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO );
//		        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
//		        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
//	        }
//	        CheckDlgButton( hDlg, IDC_MAINTAINSIZE, Edit.MaintainSize );
//	        CheckDlgButton( hDlg, IDC_SMARTSIZE, !Edit.MaintainSize && Edit.SmartSize );
//	        break;
//
//	    case IDC_PREF_UNITS:
//	        if ( !(idUnits = HandleCombo( hDlg, id, codeNotify )) )
//		        break;
//
//	        Control.Units = idUnits;
//	        SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
//	        CheckComboItem(hDlg, IDC_PREF_UNITS, IDC_PREF_UNITFIRST,
//		        IDC_PREF_UNITLAST, Control.Units);
//
//	        Bool = !Edit.MaintainSize || Control.Units != IDC_PREF_UNITPIXELS;
//	        ControlEnable(hDlg, IDC_WIDTH, Bool);
//	        ControlEnable(hDlg, IDC_HEIGHT, Bool);
//	        ControlEnable(hDlg, IDC_SCALEX, Bool);
//	        ControlEnable(hDlg, IDC_SCALEY, Bool);
//
//	        if (Edit.MaintainSize && Control.Units == IDC_PREF_UNITPIXELS)
//	        {
//		        Edit.CropResolution = Edit.Resolution;
//		        SetUnitResolution( Edit.CropResolution );
//		        Edit.Width  = FGET( npix, Edit.CropResolution );
//		        Edit.Height = FGET( nlin, Edit.CropResolution );
//		        y = FMUL( Edit.CropResolution, Edit.Height );
//		        Edit.ScaleY = ((y*100L) + nlin/2) / nlin;
//		        SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
//		        x = FMUL( Edit.CropResolution, Edit.Width );
//		        Edit.ScaleX = ((x*100L) + npix/2) / npix;
//		        SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
//	        }
//	       	SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
//	       	SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
//	        SetUnitLabels(hDlg);
//	        break;
//
//        case IDC_DISTORT:
//	        Edit.Distortable = !Edit.Distortable;
//	        CheckDlgButton( hDlg, IDC_DISTORT, Edit.Distortable );
//	        if (Edit.Distortable)
//		        Edit.MaintainSize = NO;
//	        ControlEnable(hDlg, IDC_MAINTAINSIZE, !Edit.MaintainSize);
//	        if ( Edit.ScaleX != Edit.ScaleY )
//	        {
//		        Edit.ScaleX = Edit.ScaleY;
//		        x = (((long)npix * Edit.ScaleX)+50L)/100L;
//		        Edit.Width  = FGET( x, Edit.CropResolution );
//		        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
//		        SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
//	        }
//	        CheckDlgButton( hDlg,IDC_SMARTSIZE,Edit.SmartSize && !Edit.Distortable);
//	        ControlEnable( hDlg, IDC_SMARTSIZE, !Edit.Distortable && CANDOTRANSPARENCY(Type));
//	        ControlEnable(hDlg, IDC_MAINTAINSIZE, !Edit.Distortable && lpImage);
//	        if ( Page.PrintNumeric )
//		        break;
//	        if ( !(hWnd = GetDlgItem( hDlg, IDC_PRINTVIEWPAGE)) )
//		        break;
//	        if ( !(hControl = GetDlgItem( hWnd, IDC_PRINTVIEWIMAGE)) )
//		        break;
//	        SetLongBit(hControl, GWL_STYLE, IS_RATIO_CONSTRAIN,
//		        !Edit.Distortable );
//	        if (!Edit.Distortable)
//	        {
//		        GetWindowRect(hControl, &rArea);
//		        ScreenToClient(hWnd, (LPPOINT)&rArea.left);
//		        ScreenToClient(hWnd, (LPPOINT)&rArea.right);
//		        MoveWindow(hControl, rArea.left, rArea.top,
//			        RectWidth(&rArea)-1, RectHeight(&rArea)-1, TRUE);
//	        }
//	        break;
//
//        case IDC_HEIGHT:
//	        if ( GetFocus() != GetDlgItem( hDlg, id ) )
//		        break;
//	        if ( codeNotify != EN_CHANGE )
//		        break;
//	        Edit.Height = GetDlgItemSpinFixed( hDlg, IDC_HEIGHT, &Bool, YES);
//	        if (Edit.MaintainSize)
//	        {
//		        scale = FGET(FUNITY, Edit.Height);
//		        Edit.Resolution = FMUL(nlin, scale);
//		        SetUnitResolution( Edit.Resolution );
//		        SetDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO );
//	        }
//	        // y = number of new pixels x 100
//	        y = FMUL( Edit.CropResolution, Edit.Height );
//	        // calculate scale factor = (new pixels * 100) / old pixels
//	        Edit.ScaleY = ((y * 100L) + nlin/2) / nlin;
//	        SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
//	        if ( !Edit.Distortable )
//	        {
//		        Edit.ScaleX = Edit.ScaleY;
//				x = (((long)npix * (long)y) + (nlin/2))/ (long)nlin;
//		        Edit.Width  = FGET( x, Edit.CropResolution );
//		        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
//		        SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
//	        }
//	        ImageMemory( hDlg, IDC_MEMORY,
//			        Edit.MaintainSize ? FGET(npix,1) : Edit.Width,
//			        Edit.MaintainSize ? FGET(nlin,1) : Edit.Height,
//			        Edit.MaintainSize ? 1 : Edit.Resolution, Edit.Depth );
//	        break;
//
//        case IDC_WIDTH:
//	        if ( GetFocus() != GetDlgItem( hDlg, id ) )
//		        break;
//	        if ( codeNotify != EN_CHANGE )
//		        break;
//	        Edit.Width = GetDlgItemSpinFixed( hDlg, IDC_WIDTH, &Bool, YES );
//	        if (Edit.MaintainSize)
//	        {
//		        scale = FGET(FUNITY, Edit.Width);
//		        Edit.Resolution = FMUL(npix, scale);
//		        SetUnitResolution( Edit.Resolution );
//		        SetDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO );
//	        }
//	        x = FMUL( Edit.CropResolution, Edit.Width );
//	        Edit.ScaleX = ((x * 100L) + npix/2L) / (long)npix;
//	        SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
//	        if ( !Edit.Distortable )
//	        {
//				Edit.ScaleY = Edit.ScaleX;
//				y = (((long)nlin * (long)x) + (npix/2))/ (long)npix;
//		        Edit.Height  = FGET( y, Edit.CropResolution );
//		        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
//		        SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
//	        }
//	        ImageMemory( hDlg, IDC_MEMORY,
//			        Edit.MaintainSize ? FGET(npix,1) : Edit.Width,
//			        Edit.MaintainSize ? FGET(nlin,1) : Edit.Height,
//			        Edit.MaintainSize ? 1 : Edit.Resolution, Edit.Depth );
//	        break;
//
//        case IDC_RES:
//	        if ( GetFocus() != GetDlgItem( hDlg, id ) )
//		        break;
//	        if ( codeNotify != EN_CHANGE )
//		        break;
//	        Edit.Resolution = GetDlgItemSpin( hDlg, IDC_RES, &Bool, NO );
//	        if ( Edit.Resolution <= 0 )
//		        Edit.Resolution = 2;
//	        if (Edit.MaintainSize)
//	        {
//		        Edit.Width = FGET(npix, Edit.Resolution);
//		        Edit.Height = FGET(nlin, Edit.Resolution);
//		        SetUnitResolution( Edit.Resolution );
//		        if (Control.Units != IDC_PREF_UNITPIXELS)
//		        {
//			        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
//			        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
//			        y = FMUL( Edit.CropResolution, Edit.Height );
//			        Edit.ScaleY = ((y*100L) + nlin/2) / nlin;
//			        SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
//			        x = FMUL( Edit.CropResolution, Edit.Width );
//			        Edit.ScaleX = ((x*100L) + npix/2) / npix;
//			        SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
//		        }
//	        }
//	
//	        if ( Control.Units == IDC_PREF_UNITPIXELS )
//            {
//	            if ( bResAdjust )
//                { // Extract the values again (inch values changed)
//			        Edit.CropResolution = Edit.Resolution;
//			        SetUnitResolution( Edit.Resolution );
//			        SetFocus( hWnd = GetDlgItem( hDlg, IDC_HEIGHT ) );
//					FORWARD_WM_COMMAND(hDlg, IDC_HEIGHT, hWnd, EN_CHANGE,
//									   SendMessage);
//			        SetFocus( hWnd = GetDlgItem( hDlg, IDC_WIDTH ) );
//					FORWARD_WM_COMMAND(hDlg, IDC_WIDTH, hWnd, EN_CHANGE,
//									   SendMessage);
//			        SetFocus( GetDlgItem( hDlg, id ) );
//		        }
//	            else
//		        { // Change the edit box
//			        SetUnitResolution( Edit.Resolution );
//			        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES);
//			        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
//		        }
//            }
//	        ImageMemory( hDlg, IDC_MEMORY,
//			        Edit.MaintainSize ? FGET(npix,1) : Edit.Width,
//			        Edit.MaintainSize ? FGET(nlin,1) : Edit.Height,
//			        Edit.MaintainSize ? 1 : Edit.Resolution, Edit.Depth );
//	        break;
//
//        case IDC_SCALEX:
//	        if ( GetFocus() != GetDlgItem( hDlg, id ) )
//		        break;
//	        if ( codeNotify != EN_CHANGE )
//		        break;
//	        Edit.ScaleX = GetDlgItemSpin( hDlg, IDC_SCALEX, &Bool, NO );
//	        x = (((long)npix * Edit.ScaleX)+50L)/100L;
//	        Edit.Width  = FGET( x, Edit.CropResolution );
//	        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES );
//	        if (Edit.MaintainSize)
//	        {
//		        scale = FGET(FUNITY, Edit.Width);
//		        Edit.Resolution = FMUL(npix, scale);
//		        SetUnitResolution( Edit.Resolution );
//		        SetDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO );
//	        }
//	        if ( !Edit.Distortable )
//	        {
//		        Edit.ScaleY = Edit.ScaleX;
//		        y = (((long)nlin * Edit.ScaleY)+50)/100L;
//		        Edit.Height  = FGET( y, Edit.CropResolution );
//		        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
//		        SetDlgItemSpin( hDlg, IDC_SCALEY, Edit.ScaleY, NO );
//	        }
//	        ImageMemory( hDlg, IDC_MEMORY,
//			        Edit.MaintainSize ? FGET(npix,1) : Edit.Width,
//			        Edit.MaintainSize ? FGET(nlin,1) : Edit.Height,
//			        Edit.MaintainSize ? 1 : Edit.Resolution, Edit.Depth );
//	        break;
//
//        case IDC_SCALEY:
//	        if ( GetFocus() != GetDlgItem( hDlg, id ) )
//		        break;
//	        if ( codeNotify != EN_CHANGE )
//		        break;
//	        Edit.ScaleY = GetDlgItemSpin( hDlg, IDC_SCALEY, &Bool, NO );
//	        y = (((long)nlin * Edit.ScaleY)+50)/100L;
//	        Edit.Height = FGET( y, Edit.CropResolution );
//	        SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Edit.Height, YES );
//	        if (Edit.MaintainSize)
//	        {
//		        scale = FGET(FUNITY, Edit.Height);
//		        Edit.Resolution = FMUL(nlin, scale);
//		        SetUnitResolution( Edit.Resolution );
//		        SetDlgItemSpin( hDlg, IDC_RES, Edit.Resolution, NO );
//	        }
//	        if ( !Edit.Distortable )
//	        {
//		        Edit.ScaleX = Edit.ScaleY;
//		        x = (((long)npix * Edit.ScaleX)+50L)/100L;
//		        Edit.Width  = FGET( x, Edit.CropResolution );
//		        SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Edit.Width, YES);
//		        SetDlgItemSpin( hDlg, IDC_SCALEX, Edit.ScaleX, NO );
//	        }
//	        ImageMemory( hDlg, IDC_MEMORY,
//			        Edit.MaintainSize ? FGET(npix,1) : Edit.Width,
//			        Edit.MaintainSize ? FGET(nlin,1) : Edit.Height,
//			        Edit.MaintainSize ? 1 : Edit.Resolution, Edit.Depth );
//	        break;
//
//        case IDC_MEMORY:
//        default:
//    	    break;
//    }
//}

/***********************************************************************/
VOID SetUnitLabels(HWND hDlg)
/***********************************************************************/
{
int idCtl;
LPSTR lpUnit;

if ( !AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES, &lpUnit ) )
	return;

for (idCtl = IDC_FIRSTLABEL; idCtl <= IDC_LASTLABEL; ++idCtl)
	{
	if (GetDlgItem(hDlg, idCtl))
		SetDlgItemText( hDlg, idCtl, lpUnit );
	}
}

/***********************************************************************/
// LOCAL prototypes
LOCAL BOOL New_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void New_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void New_OnClose(HWND hDlg);

static LFIXED fSavedWidth, fSavedHeight;
static int iSavedResolution, iSavedDepth;

/***********************************************************************/
BOOL WINPROC EXPORT DlgNewProc( HWND hDlg, UINT msg, WPARAM wParam,
                           LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, New_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, New_OnCommand);

	HANDLE_DLGMSG(hDlg, WM_CLOSE, New_OnClose);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_NEW);
	}
}

/************************************************************************/
LOCAL BOOL New_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
int incr;
long Color;

CenterPopup( hDlg );
SetUnitResolution( Control.NewResolution );
if ( (incr = Control.NewDepth) > 1 )
	incr--;
if( Control.Retail )
{
    CheckComboItem( hDlg, IDC_NEWTYPE, IDC_NEWLINEART, IDC_NEWCMYKCOLOR,
	    IDC_NEWLINEART + incr );
}
else
{
    CheckComboItem( hDlg, IDC_NEWTYPE, IDC_NEWLINEART, IDC_NEWCOLOR,
	    IDC_NEWLINEART + incr );
}
InitDlgItemSpinFixed( hDlg, IDC_HEIGHT, Control.NewHeight, YES, 0L,
	FGET(9999,1) );
InitDlgItemSpinFixed( hDlg, IDC_WIDTH, Control.NewWidth, YES, 0L,
	FGET(9999,1) );
InitDlgItemSpin( hDlg, IDC_RES, Control.NewResolution, NO, 1, 10000 );
ImageMemory( hDlg, IDC_MEMORY, Control.NewWidth, Control.NewHeight,
	Control.NewResolution, Control.NewDepth );
Edit.SaveUnits = Control.Units;
CheckComboItem(hDlg, IDC_PREF_UNITS, IDC_PREF_UNITFIRST,
	IDC_PREF_UNITLAST, Control.Units);
SetUnitLabels(hDlg);
fSavedWidth = Control.NewWidth;
fSavedHeight = Control.NewHeight;
iSavedDepth = Control.NewDepth;
iSavedResolution = Control.NewResolution;

Edit.FillColor.gray = 255;
SetColorInfo(&Edit.FillColor, &Edit.FillColor, CS_GRAY);
CopyRGB( &Edit.FillColor.rgb, &Color );
SetWindowLong( GetDlgItem(hDlg, IDC_FILLINCOLOR), GWL_ACTIVE, Color );
ControlEnable( hDlg, IDC_CMS_ENABLED, Control.CMSEnabled &&
	( Control.NewDepth == FDT_RGBCOLOR ||
	Control.NewDepth == FDT_CMYKCOLOR ) );
cmsInfoInit( &ColorManager.FileNew );
return(TRUE);
}

/************************************************************************/
LOCAL void New_OnClose(HWND hDlg)
/************************************************************************/
{
SetUnitResolution( 0 );
Dialog_OnClose(hDlg);
}

/************************************************************************/
LOCAL void New_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int 		idUnits;
BOOL 		Bool;
STRING 	szString;
long 		Color;
CMSINFO	cmsInfo;

CCmsFileNewDlg	FileDlg( &cmsInfo, CWnd::FromHandle( PictPubApp.Get_hWndAstral() ) );


switch (id)
	{
	case IDC_CMS_ENABLED:
		cmsInfo.src.DataType = ( FRMDATATYPE )Control.NewDepth;
		FileDlg.DoModal();
		ColorManager.FileNew = cmsInfo;
	break;

	case IDC_PREF_UNITS:
	if ( !(idUnits = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Control.Units = idUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	CheckComboItem(hDlg, IDC_PREF_UNITS, IDC_PREF_UNITFIRST,
		IDC_PREF_UNITLAST, Control.Units);
	SetDlgItemSpinFixed( hDlg, IDC_WIDTH, Control.NewWidth, YES );
	SetDlgItemSpinFixed( hDlg, IDC_HEIGHT, Control.NewHeight, YES );
	SetUnitLabels(hDlg);
	break;

	case IDC_HEIGHT:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Control.NewHeight = GetDlgItemSpinFixed( hDlg, id, &Bool, YES );
	ImageMemory( hDlg, IDC_MEMORY, Control.NewWidth, Control.NewHeight,
		Control.NewResolution, Control.NewDepth );
	break;

	case IDC_WIDTH:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Control.NewWidth = GetDlgItemSpinFixed( hDlg, id, &Bool, YES);
	ImageMemory( hDlg, IDC_MEMORY, Control.NewWidth, Control.NewHeight,
		Control.NewResolution, Control.NewDepth );
	break;

	case IDC_RES:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Control.NewResolution = GetDlgItemSpin( hDlg, id, &Bool, NO );
	SetUnitResolution( Control.NewResolution );
	if ( Control.Units == IDC_PREF_UNITPIXELS )
		{ // Extract the values again (inch values changed)
		Control.NewHeight = GetDlgItemSpinFixed( hDlg, IDC_HEIGHT, &Bool,
			YES);
		Control.NewWidth = GetDlgItemSpinFixed( hDlg, IDC_WIDTH, &Bool,
			YES );
		}
	ImageMemory( hDlg, IDC_MEMORY, Control.NewWidth, Control.NewHeight,
		Control.NewResolution, Control.NewDepth );
	break;

	case IDC_NEWTYPE:
//	case IDC_NEWLINEART:
//	case IDC_NEWGRAY:
//	case IDC_NEWCOLOR:
//	case IDC_NEWCMYKCOLOR:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Control.NewDepth = id - IDC_NEWLINEART;
	if ( Control.NewDepth > 1 )
		Control.NewDepth++;
	ImageMemory( hDlg, IDC_MEMORY, Control.NewWidth, Control.NewHeight,
		Control.NewResolution, Control.NewDepth );
	ControlEnable( hDlg, IDC_CMS_ENABLED, Control.CMSEnabled &&
		( Control.NewDepth == FDT_RGBCOLOR ||
		Control.NewDepth == FDT_CMYKCOLOR ) );
	break;

	case IDC_FILLINCOLOR:
	if (codeNotify != 1)
		break;
	if (Control.NewDepth == FDT_LINEART) 
		{
		if (Edit.FillColor.gray == 255)
			Edit.FillColor.gray = 0;
		else
			Edit.FillColor.gray = 255;
		SetColorInfo(&Edit.FillColor, &Edit.FillColor, CS_GRAY);
		}
	else if (Control.NewDepth == FDT_GRAYSCALE) 
	   {
	   if (!PalettePicker(&Edit.FillColor, NULL, NULL))
	      break;
	   }
	else if (!ColorPicker(&Edit.FillColor, NULL))
	   break;
	CopyRGB( &Edit.FillColor.rgb, &Color );
	SetWindowLong( GetDlgItem(hDlg, IDC_FILLINCOLOR), GWL_ACTIVE, Color );
	AstralControlRepaint(hDlg, IDC_FILLINCOLOR);
	break;

	case IDOK:
	// Check to see if the memory has overflowed
	GetDlgItemText( hDlg, IDC_MEMORY, szString, sizeof(STRING) );
	if ( szString[0] == '*' )
		{
		Message( IDS_ETOOWIDE );
		break;
		}
	Control.Units = Edit.SaveUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	SetUnitResolution( 0 );
	if( Control.CMSEnabled && IsDstPTSelected( &ColorManager.FileNew.dst.ptInfo ) )
		{
		if( ( Control.NewDepth == FDT_CMYKCOLOR &&
				ColorManager.FileNew.dst.DataType != FDT_CMYKCOLOR ) ||
				( Control.NewDepth == FDT_RGBCOLOR &&
				ColorManager.FileNew.dst.DataType != FDT_RGBCOLOR ) )
			{
			Message( IDS_EBADFILETYPE );
			break;
			}
		}
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	Control.NewWidth = fSavedWidth;
	Control.NewHeight = fSavedHeight;
	Control.NewDepth = iSavedDepth;
	Control.NewResolution = iSavedResolution;
	Control.Units = Edit.SaveUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	SetUnitResolution( 0 );
	AstralDlgEnd( hDlg, FALSE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL Pref_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Pref_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void Pref_OnClose(HWND hDlg);
LOCAL void Pref_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem);
LOCAL void Pref_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem);
LOCAL HBRUSH Pref_OnCtlColor(HWND hDlg, HDC hDC, HWND hControl, int wType);
LOCAL void SetIconInterfaceType (BOOL bUseColor);

// Maps a (tab) to a Dialog ID
static int Pref_iTabTable[] =
	{
	IDD_PREFMEMORY,
	IDD_PREFUNDO,
	IDD_PREFOBJECTS,
	IDD_PREFSCRATCHPAD,
	IDD_PREFPLUGIN,
	IDD_PREFUNITS,
	IDD_PREFMISC,
	IDD_LOCATIONS
	};

static int Pref_iTabTableOEM[] =
	{
	IDD_PREFMEMORY,
	IDD_PREFUNDO,
	IDD_PREFSCRATCHPAD,
	IDD_PREFPLUGIN,
	IDD_PREFUNITS,
	IDD_PREFMISC,
	IDD_LOCATIONS
	};

static ITEMID FileType_Table[] =
	{
	IDT_PPCOMPOUND,		
	IDT_PP,		
	IDT_TIFF,	
	IDT_TARGA, 	
	IDT_GIF, 	
	IDT_BMP, 	
	IDT_DIB, 	
	IDT_PCX, 	
	IDT_JPEG, 	
	IDT_SCITEX, 
	IDT_PP4
	};

static ITEMID FileType_TableOEM[] =
	{
	IDT_PPCOMPOUND,		
	IDT_PP,		
	IDT_TIFF,	
	IDT_TARGA, 	
	IDT_GIF, 	
	IDT_BMP, 	
	IDT_DIB, 	
	IDT_PCX, 	
	IDT_JPEG, 	
	IDT_PP4
	};

// Memory Group
static FNAME szRamDisk;
static int MainMemMin, MainMemFactor;
static BOOL LineArtAsGray;
// Scratchpad Group
static int ScratchpadWidth, ScratchpadHeight;
static BOOL IsScratchpadColor;
// Undo Group
static BOOL Undo, UndoMasks, UndoObjects, UseApply, DoCommandList;
// Miscellaneous Group
static FNAME szPouchPath, szMacro;
static int UseTablet;
static int MaskTint;
static BOOL IndependentSettings;
static BOOL PasteAtRealSize;
static BOOL UseGrayScaleIcons;
static BOOL DisableAutoScroll;
static BOOL DisableDither;
static ITEMID DefaultFileType;


// Units Group
static WORD Units;
static LFIXED ScreenWidth;
static BOOL UseRulers;
static BOOL DisplayPixelValue;
// Plugin Group
static FNAME szPlugin1, szPlugin2;
static BOOL UsePlugins;
// Object Group
static BOOL bObjects, bObjectMasks;
// Files Group
static STRING sz;
struct FILEPATH
{ 	
	FNAME Clipbits; 
	FNAME Printstyles;
	FNAME Brushes; 
	FNAME Palettes;
	FNAME Textures; 
	FNAME Devices;
	FNAME Effects; 
	FNAME Macros;
	FNAME Importfilters;
};

FILEPATH *pFilePath;


static DLGSECTION	prefsec;

static ITEMID iPrefGroup    = 0;
static ITEMID iLastGroup    = -1;

/***********************************************************************/
LOCAL void SetIconInterfaceType (BOOL bUseColor)
/***********************************************************************/
{
	ASSERT (hInstIconLib != NULL);
	FNAME szFileName;

	if (bUseColor)
		Control.UseGrayScaleIcons = FALSE;
	else
		Control.UseGrayScaleIcons = TRUE;
	
	FreeLibrary (hInstIconLib);
	lstrcpy(szFileName,Control.ProgHome);
	if (Control.UseGrayScaleIcons)
		lstrcat (szFileName, "gico.dll");
	else
		lstrcat (szFileName, "cico.dll");
	if (hInstIconLib = (HINSTANCE)AstralLoadLibrary (szFileName))
	{
		// switch out the current Icon Library
		Ctl_SetIconInstance (hInstIconLib);
		// Notify the children of the Astral to repaint.
		CServerView *pView = PictPubApp.GetActiveView();
		if (pView && pView->GetDocument() && 
			pView->GetDocument()->IsInPlaceActive())
			::InvalidateRect (NULL,(LPRECT)NULL,TRUE);
		else
		{
			HWND hWnd = ::GetWindow (PictPubApp.Get_hWndAstral(),GW_CHILD);
			while (hWnd != NULL)
			{
				::InvalidateRect (hWnd,(LPRECT)NULL,TRUE);
				hWnd = ::GetWindow (hWnd,GW_HWNDNEXT);
			}
			HWND hWndAstral = PictPubApp.Get_hWndAstral();
			hWnd = ::GetWindow (hWndAstral,GW_HWNDFIRST);
			while (hWnd != NULL)
			{
				if ((GetWindow(hWnd, GW_OWNER) == hWndAstral) &&
					(hWnd != hWndAstral))
					::InvalidateRect (hWnd,(LPRECT)NULL,TRUE);
				hWnd = ::GetWindow (hWnd,GW_HWNDNEXT);
			}
		}
	}
	ASSERT (hInstIconLib != NULL);
}

/***********************************************************************/
static void UpdatePrefs(HWND hDlg, ITEMID iPrefGroup)
/***********************************************************************/
{
	ITEMID idCategory;

	if (iPrefGroup == iLastGroup)
		return;
	
	iLastGroup = iPrefGroup;

    if( Control.Retail )
    	idCategory = Pref_iTabTable[iPrefGroup];
    else
    	idCategory = Pref_iTabTableOEM[iPrefGroup];

	SelectDlgSectionCategory(&prefsec, idCategory);

	if (idCategory == IDD_PREFMEMORY)
	{
		CheckDlgButton( hDlg, IDC_PREF_LINEART, Control.LineArtAsGray);
		SetDlgItemText( hDlg, IDC_PREF_RAMDISK, Control.RamDisk );
		InitDlgItemSpin( hDlg, IDC_PREF_MEMMIN,    Control.MainMemMin,    NO, 0, 16384 );
		SetDlgItemSpinStep( hDlg, IDC_PREF_MEMMIN, 64L );
		InitDlgItemSpin( hDlg, IDC_PREF_MEMFACTOR, Control.MainMemFactor, NO, 1, 100 );
	}
	else
	if (idCategory == IDD_PREFSCRATCHPAD)
	{
		CheckComboItem(  hDlg, IDC_PREF_SCRATCHTYPE,
					IDC_PREF_SCRATCHGRAY, IDC_PREF_SCRATCHCOLOR,
					Control.IsScratchpadColor + IDC_PREF_SCRATCHGRAY);
		InitDlgItemSpin( hDlg, IDC_PREF_SCRATCHX, Control.ScratchpadWidth,  NO, 1, 500 );
		InitDlgItemSpin( hDlg, IDC_PREF_SCRATCHY, Control.ScratchpadHeight, NO, 1, 500 );
	}
	else
	if (idCategory == IDD_PREFUNDO)
	{
		ITEMID id;

		if ( Control.NoUndo )
				id = IDC_PREF_NOUNDO;
		else	id = (Control.UseApply + IDC_PREF_AUTOAPPLY);
		CheckComboItem(  hDlg, IDC_PREF_UNDOTYPE, IDC_PREF_NOUNDO, IDC_PREF_MANUALAPPLY, id );

		if (GetDlgItem(hDlg, IDC_PREF_UNDOMODE))
			CheckComboItem(  hDlg, IDC_PREF_UNDOMODE,
							IDC_PREF_IMAGEUNDO, IDC_PREF_OBJECTUNDO,
							Control.UndoObjects + IDC_PREF_IMAGEUNDO );
		CheckDlgButton( hDlg, IDC_PREF_MASKUNDO, !Control.UndoMasks );
		CheckDlgButton( hDlg, IDC_PREF_DISABLECMDLIST, !Control.DoCommandList );
		ControlEnable( hDlg, IDC_PREF_MASKUNDO, !Control.NoUndo );
	}
	else
	if (idCategory == IDD_PREFMISC)
	{
		SetDlgItemText(  hDlg, IDC_PREF_POUCH, Control.PouchPath );
		CheckComboItem(  hDlg, IDC_PREF_TABLET, IDC_PREF_TABLETUNUSED, IDC_PREF_TABLETBOTH, Control.UseTablet+IDC_PREF_TABLETUNUSED );
        if( Control.Retail )
        {
    		if (GetDlgItem(hDlg, IDC_PREF_STARTMACRO))
	    		InitExtNameN( hDlg, IDC_PREF_STARTMACRO, IDC_FILENAME, Names.StartMacro, IDN_MACRO, IDS_NONE, NO );
        }
		CheckComboItem(  hDlg, IDC_PREF_MASKTINT, IDC_PREF_MASKTINT1, IDC_PREF_MASKTINT6, Control.MaskTint + IDC_PREF_MASKTINT1);
		CheckDlgButton( hDlg, IDC_PREF_TOOLSETTINGS, Control.IndependentSettings);
		CheckDlgButton( hDlg, IDC_CMS_ENABLED, Control.CMSEnabled);
		CheckDlgButton( hDlg, IDC_PREF_PASTEATREALSIZE, Mask.PasteAtRealSize);
		CheckDlgButton( hDlg, IDC_PREF_USEGRAYSCALEICONS, Control.UseGrayScaleIcons);
		CheckDlgButton( hDlg, IDC_PREF_DISABLEAUTOSCROLL, Control.DisableAutoScroll);
		CheckDlgButton( hDlg, IDC_PREF_DISABLEDITHER, Vignette.DisableDither);
        if( Control.Retail )
        {
		    TableCheckComboItem(hDlg, IDC_PREF_FILETYPE, FileType_Table,
					    sizeof(FileType_Table)/sizeof(ITEMID),
					    LocalFileType(Control.DefaultFileType),
					    FALSE);
        }
        else
        {
		    TableCheckComboItem(hDlg, IDC_PREF_FILETYPE, FileType_TableOEM,
					    sizeof(FileType_TableOEM)/sizeof(ITEMID),
					    LocalFileType(Control.DefaultFileType),
					    FALSE);
        }
	}
	else
	if (idCategory == IDD_PREFUNITS)
	{
		LPSTR lpUnit;

		CheckComboItem( hDlg, IDC_PREF_UNITS, IDC_PREF_UNITINCHES, IDC_PREF_UNITPIXELS, Control.Units );
		ControlEnable(  hDlg, IDC_PREF_SCREENWIDTH, Control.Units != IDC_PREF_UNITPIXELS );
		CheckDlgButton( hDlg, IDC_PREF_USERULERS, View.UseRulers );
		CheckDlgButton( hDlg, IDC_PREF_DISPLAYPIXELS,
			 		!Control.DisplayPixelValue );
		if ( Control.Units != IDC_PREF_UNITPIXELS )
			AstralSetDlgItemFixed( hDlg, IDC_PREF_SCREENWIDTH,
				Control.ScreenWidth, NO );
		else
			SetDlgItemInt( hDlg, IDC_PREF_SCREENWIDTH,
				GetSystemMetrics( SM_CXSCREEN ), NO );
		if ( AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES, &lpUnit ))
			SetDlgItemText( hDlg, IDC_WIDTHLABEL, lpUnit );
	}
	else
	if (idCategory == IDD_PREFPLUGIN)
	{
		SetDlgItemText( hDlg, IDC_PREF_PLUGINPATH1, Control.PluginPath1);
		SetDlgItemText( hDlg, IDC_PREF_PLUGINPATH2, Control.PluginPath2);
		CheckDlgButton( hDlg, IDC_PREF_USEPLUGINS,  Control.UsePlugins);
	}
	else
	if (idCategory == IDD_PREFOBJECTS)
	{
		CheckDlgButton( hDlg, IDC_PREF_OBJECTS, Control.MultipleObjects );
		//	if (!Control.MultipleObjects)
		//		Control.UseMaskAndObjects = FALSE;
		CheckDlgButton( hDlg, IDC_PREF_OBJECTMASKS, Control.UseMaskAndObjects );
		ControlEnable(  hDlg, IDC_PREF_OBJECTMASKS, Control.MultipleObjects );
	}
	else
	if (idCategory == IDD_LOCATIONS)
	{
			SetDlgItemText( hDlg, IDC_LOCATE_CLIPBITS, Lowercase(pFilePath->Clipbits) );
			SetDlgItemText( hDlg, IDC_LOCATE_PRNSTYLES, Lowercase(pFilePath->Printstyles) );
			SetDlgItemText( hDlg, IDC_LOCATE_BRUSHES, Lowercase(pFilePath->Brushes) );
			SetDlgItemText( hDlg, IDC_LOCATE_PALETTES, Lowercase(pFilePath->Palettes) );
			SetDlgItemText( hDlg, IDC_LOCATE_TEXTURES, Lowercase(pFilePath->Textures) );
			SetDlgItemText( hDlg, IDC_LOCATE_DEVICES, Lowercase(pFilePath->Devices) );
			SetDlgItemText( hDlg, IDC_LOCATE_FILTERS, Lowercase(pFilePath->Effects) );
            if( Control.Retail )
            {
			    SetDlgItemText( hDlg, IDC_LOCATE_MACROS, Lowercase(pFilePath->Macros) );
		        SetDlgItemText( hDlg, IDC_LOCATE_IMPORTFILTERS, Lowercase(pFilePath->Importfilters));
            }
	}
}

/***********************************************************************/
BOOL WINPROC EXPORT DlgPrefProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Pref_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Pref_OnCommand);

	HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, Pref_OnMeasureItem);
	HANDLE_DLGMSG(hDlg, WM_DRAWITEM, Pref_OnDrawItem);
	HANDLE_DLGMSG(hDlg, WM_CLOSE, Pref_OnClose);
	HANDLE_CONTROL_COLORS(hDlg, Pref_OnCtlColor);

#ifndef WIN32

	case WM_SYSCOMMAND:
	if ((wParam  & 0xFFF0) == SC_KEYMENU)
		return (!FTabSwitcherAccel(hDlg, LOWORD(lParam)));
	return(FALSE);

#endif

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PREF);
	}
}


/************************************************************************/
LOCAL BOOL Pref_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );

iLastGroup = -1;

// Memory Group
lstrcpy( szRamDisk, Control.RamDisk );
MainMemMin = Control.MainMemMin;
MainMemFactor = Control.MainMemFactor;
LineArtAsGray = Control.LineArtAsGray;

// Scratchpad Group
ScratchpadWidth   = Control.ScratchpadWidth;
ScratchpadHeight  = Control.ScratchpadHeight;
IsScratchpadColor = Control.IsScratchpadColor;

// Undo Group
UseApply = Control.UseApply;
Undo = Control.NoUndo;
UndoMasks = Control.UndoMasks;
UndoObjects = Control.UndoObjects;
DoCommandList = Control.DoCommandList;

// Miscellaneous
lstrcpy( szPouchPath, Control.PouchPath );
lstrcpy(szMacro, Names.StartMacro);
UseTablet = Control.UseTablet;
MaskTint = Control.MaskTint;
IndependentSettings = Control.IndependentSettings;
PasteAtRealSize = Mask.PasteAtRealSize;
UseGrayScaleIcons = Control.UseGrayScaleIcons;
DisableAutoScroll = Control.DisableAutoScroll;
DisableDither = Vignette.DisableDither;
DefaultFileType = Control.DefaultFileType;

// Units Group
Units = Control.Units;
ScreenWidth = Control.ScreenWidth;
UseRulers = View.UseRulers;
DisplayPixelValue = Control.DisplayPixelValue;
SetUnitResolution( 1 ); // For the disabled Screen Width edit box

// PlugIn Group
UsePlugins = Control.UsePlugins;
lstrcpy( szPlugin1, Control.PluginPath1 );
lstrcpy( szPlugin2, Control.PluginPath2 );

// Object Group
bObjects = Control.MultipleObjects;
bObjectMasks = Control.UseMaskAndObjects;

// Files Group

pFilePath = (FILEPATH*) Alloc ((long)sizeof(FILEPATH));
if (!pFilePath)
	{
	// message here
	return (FALSE);
	}
else
	{
	GetExtNamePath( pFilePath->Clipbits, IDN_CLIPBOARD );
	GetExtNamePath( pFilePath->Printstyles, IDN_STYLE );
	GetExtNamePath( pFilePath->Brushes, IDN_CUSTOM );
	GetExtNamePath( pFilePath->Palettes,IDN_PALETTE );
	GetExtNamePath( pFilePath->Textures, IDN_TEXTURE );
	GetExtNamePath( pFilePath->Devices, IDN_SCANNER );
	GetExtNamePath( pFilePath->Effects, IDN_SPECIAL );
    if( Control.Retail )
        {
	    GetExtNamePath( pFilePath->Macros, IDN_MACRO );
        GetExtNamePath( pFilePath->Importfilters, IDN_IMPORTFILTERS );
        }
	}


// Initialise the dialog section.
InitDlgSection(&prefsec, ID_SECTION0, hDlg, IDC_PANE0, NULL);
// Set threshold so that a category is always discarded when the user
// selects another.
SetDlgSectionThreshold(&prefsec, 1, 1);

int nItems;
if( Control.Retail )
    nItems = sizeof(Pref_iTabTable)/sizeof(int);
else
    nItems = sizeof(Pref_iTabTableOEM)/sizeof(int);

for (int i = 0; i < nItems; ++i)
{
    if( Control.Retail )
    {
        // add the halftone category
	    AddDlgSectionCategory(	&prefsec,
							    PictPubApp.GetResourceHandle(),
							    MAKEINTRESOURCE(Pref_iTabTable[i]),
							    Pref_iTabTable[i],
					    	    DSCF_DISCARDABLE,
							    NULL);
    }
    else
    {
        // add the halftone category
	    AddDlgSectionCategory(	&prefsec,
							    PictPubApp.GetResourceHandle(),
							    MAKEINTRESOURCE(Pref_iTabTableOEM[i]),
							    Pref_iTabTableOEM[i],
					    	    DSCF_DISCARDABLE,
							    NULL);
    }
}

UpdatePrefs( hDlg, iPrefGroup );

return(TRUE);
}

/************************************************************************/
LOCAL void Pref_OnMeasureItem(HWND hDlg, MEASUREITEMSTRUCT FAR * lpMeasureItem)
/************************************************************************/
{
MeasureItem( hDlg, lpMeasureItem, NO );
}

/************************************************************************/
LOCAL void Pref_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR * lpDrawItem)
/************************************************************************/
{
DrawItem( hDlg, lpDrawItem, 0 );
}

/***********************************************************************/
LOCAL HBRUSH Pref_OnCtlColor(HWND hDlg, HDC hDC, HWND hControl, int wType)
/***********************************************************************/
{
return( Dialog_OnCtlColor( hDlg, hDC, hControl, wType) );
}

/************************************************************************/
LOCAL void Pref_OnClose(HWND hDlg)
/************************************************************************/
{
SetUnitResolution( 0 );
Dialog_OnClose(hDlg);
}

extern STRING szAppName;

void WackCmdList()
   {
	POSITION       TemplatePos = PictPubApp.GetFirstDocTemplate();
   POSITION       DocPos;
   CServerDoc     *pDoc;
   CDocTemplate   *pTemplate;

	while (TemplatePos)
	   {
		pTemplate = PictPubApp.GetNextDocTemplate(TemplatePos);
		DocPos = pTemplate->GetFirstDocPosition();
		while (DocPos)
		   {
    		pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
    		if (pDoc)
            {
            pDoc->m_lpImage->lpCmdList->fSaveCommands = FALSE;
            DestroyPacketList(&pDoc->m_lpImage->lpCmdList->PacketList);
            DestroyPacketList(&pDoc->m_lpImage->lpCmdList->UndoPacketList);
            DestroyPacketList(&pDoc->m_lpImage->lpCmdList->RedoPacketList);
            }
   		}
	   }
   }

/************************************************************************/
LOCAL void Pref_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
int temp;
LPSTR lpUnit;
BOOL Bool;
LPIMAGE lpImage;
HWND hActiveWnd;

switch (id)
	{
	case IDC_PREF_POUCH:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id, Control.PouchPath,
		sizeof(Control.PouchPath));
	FixPath( Lowercase( Control.PouchPath ) );
	break;

	case IDC_PREF_RAMDISK:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id, Control.RamDisk,
		sizeof(Control.RamDisk));
	FixPath( Lowercase( Control.RamDisk ) );
	break;

	case IDC_PREF_OBJECTS:
	Control.MultipleObjects = !Control.MultipleObjects;
	CheckDlgButton( hDlg, id, Control.MultipleObjects );
//	if (!Control.MultipleObjects)
//		Control.UseMaskAndObjects = FALSE;
//	CheckDlgButton( hDlg, IDC_PREF_OBJECTMASKS, Control.UseMaskAndObjects );
	ControlEnable(  hDlg, IDC_PREF_OBJECTMASKS, Control.MultipleObjects );
	break;

	case IDC_PREF_OBJECTMASKS:
	if (!Control.MultipleObjects)
		break;
	Control.UseMaskAndObjects = !Control.UseMaskAndObjects;
	CheckDlgButton( hDlg, id, Control.UseMaskAndObjects );
	break;

	case IDC_PREF_LINEART:
	Control.LineArtAsGray = !Control.LineArtAsGray;
	CheckDlgButton( hDlg, id, Control.LineArtAsGray );
	break;

	case IDC_PREF_USERULERS:
	View.UseRulers = !View.UseRulers;
	CheckDlgButton( hDlg, id, View.UseRulers );
	break;

	case IDC_PREF_DISPLAYPIXELS:
	Control.DisplayPixelValue = !Control.DisplayPixelValue;
	CheckDlgButton( hDlg, id, !Control.DisplayPixelValue );
	break;

	case IDC_PREF_MEMMIN:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	Control.MainMemMin = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_PREF_MEMFACTOR:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	Control.MainMemFactor = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_PREF_SCREENWIDTH:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Control.ScreenWidth = AstralGetDlgItemFixed( hDlg, id, &Bool, NO);
	break;

	case IDC_PREF_UNITS:
//	case IDC_PREF_UNITINCHES:
//	case IDC_PREF_UNITMM:
//	case IDC_PREF_UNITCM:
//	case IDC_PREF_UNITPICAS:
//	case IDC_PREF_UNITPIXELS:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Control.Units = id;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	if ( AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES,
	     &lpUnit ) )
		SetDlgItemText( hDlg, IDC_WIDTHLABEL, lpUnit );
	ControlEnable( hDlg, IDC_PREF_SCREENWIDTH,
		Control.Units != IDC_PREF_UNITPIXELS );

	if ( Control.Units != IDC_PREF_UNITPIXELS )
		AstralSetDlgItemFixed( hDlg, IDC_PREF_SCREENWIDTH, Control.ScreenWidth, NO );
	else
		SetDlgItemInt( hDlg, IDC_PREF_SCREENWIDTH, GetSystemMetrics( SM_CXSCREEN ), NO );
	break;

	case IDC_PREF_SCRATCHX:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Control.ScratchpadWidth = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_PREF_SCRATCHY:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	if ( codeNotify != EN_CHANGE )
		break;
	Control.ScratchpadHeight = GetDlgItemSpin( hDlg, id, &Bool, NO );
	break;

	case IDC_PREF_SCRATCHTYPE:
//	case IDC_PREF_SCRATCHGRAY:
//	case IDC_PREF_SCRATCHCOLOR:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Control.IsScratchpadColor = id - IDC_PREF_SCRATCHGRAY;
	break;

	case IDC_PREF_UNDOTYPE:
//	case IDC_PREF_NOUNDO:
//	case IDC_PREF_MANUALAPPLY:
//	case IDC_PREF_AUTOAPPLY:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Control.NoUndo = (id == IDC_PREF_NOUNDO);
	Control.UseApply = (id == IDC_PREF_MANUALAPPLY);
	ControlEnable( hDlg, IDC_PREF_MASKUNDO, !Control.NoUndo );
	break;

	case IDC_PREF_UNDOMODE:
//	case IDC_PREF_IMAGEUNDO:
//	case IDC_PREF_OBJECTUNDO:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	Control.UndoObjects = (id == IDC_PREF_OBJECTUNDO);
	break;

	case IDC_PREF_MASKUNDO:
	Control.UndoMasks = !Control.UndoMasks;
	CheckDlgButton( hDlg, id, !Control.UndoMasks );
	break;

	case IDC_PREF_DISABLECMDLIST:
    if( Control.Retail )
    {
    	Control.DoCommandList = !Control.DoCommandList;
	    CheckDlgButton( hDlg, id, !Control.DoCommandList );
    }
	break;

	case IDC_PREF_TABLET:
//	case IDC_PREF_TABLETUNUSED:
//	case IDC_PREF_TABLETOPACITY:
//	case IDC_PREF_TABLETSIZE:
//	case IDC_PREF_TABLETBOTH:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		Control.UseTablet = id-IDC_PREF_TABLETUNUSED;
	break;

	case IDC_PREF_STARTMACRO:
        if( Control.Retail )
        {
		    GetExtNameN( hDlg, id, IDC_FILENAME, Names.StartMacro,
			    IDN_MACRO, codeNotify, IDS_NONE, NO );
        }
	break;

	case IDC_PREF_MASKTINT:
//	case IDC_PREF_MASKTINT1:
//	case IDC_PREF_MASKTINT2:
//	case IDC_PREF_MASKTINT3:
//	case IDC_PREF_MASKTINT4:
//	case IDC_PREF_MASKTINT5:
//	case IDC_PREF_MASKTINT6:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		Control.MaskTint = id - IDC_PREF_MASKTINT1;
	break;

	case IDC_PREF_TOOLSETTINGS:
	Control.IndependentSettings = !Control.IndependentSettings;
	CheckDlgButton(hDlg, id, Control.IndependentSettings);
	break;

	case IDC_CMS_ENABLED:
	Control.CMSEnabled = CMSOpen( !Control.CMSEnabled );
	CheckDlgButton(hDlg, id, Control.CMSEnabled);
	break;

	case IDC_PREF_PASTEATREALSIZE:
	Mask.PasteAtRealSize = !Mask.PasteAtRealSize;
	CheckDlgButton( hDlg, IDC_PREF_PASTEATREALSIZE, Mask.PasteAtRealSize);
	break;

	case IDC_PREF_USEGRAYSCALEICONS:
	Control.UseGrayScaleIcons = !Control.UseGrayScaleIcons;
	CheckDlgButton( hDlg, IDC_PREF_USEGRAYSCALEICONS, Control.UseGrayScaleIcons);
	break;

	case IDC_PREF_DISABLEAUTOSCROLL:
	Control.DisableAutoScroll = !Control.DisableAutoScroll;
	CheckDlgButton( hDlg, IDC_PREF_DISABLEAUTOSCROLL, Control.DisableAutoScroll);
	break;

	case IDC_PREF_DISABLEDITHER:
	Vignette.DisableDither = !Vignette.DisableDither;
	CheckDlgButton( hDlg, IDC_PREF_DISABLEDITHER, Vignette.DisableDither);
	break;

	case IDC_PREF_FILETYPE:
    if( Control.Retail )
    {
	    if (! (id = TableHandleCombo(hDlg, FileType_Table,
					    sizeof(FileType_Table)/sizeof(ITEMID),
					    id, codeNotify)))
            break;
    }
    else
    {
	    if (! (id = TableHandleCombo(hDlg, FileType_TableOEM,
					    sizeof(FileType_TableOEM)/sizeof(ITEMID),
					    id, codeNotify)))
            break;
    }
	Control.DefaultFileType = AppFileType(id);
	break;

	case IDC_PREF_USEPLUGINS:
	Control.UsePlugins = !Control.UsePlugins;
	CheckDlgButton( hDlg, id, Control.UsePlugins );
	break;

	case IDC_PREF_PLUGINPATH1:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    	break;
		if ( codeNotify != EN_CHANGE )
		    break;
		GetDlgItemText( hDlg, id, Control.PluginPath1,
			sizeof(Control.PluginPath1));
		FixPath( Lowercase( Control.PluginPath1 ) );
	break;

	case IDC_PREF_PLUGINPATH2:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
		    break;
		if ( codeNotify != EN_CHANGE )
		    break;
		GetDlgItemText( hDlg, id, Control.PluginPath2,
			sizeof(Control.PluginPath2));
		FixPath( Lowercase( Control.PluginPath2 ) );
	break;

	case IDC_LOCATE_CLIPBITS:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id, pFilePath->Clipbits,
		sizeof(pFilePath->Clipbits));
	FixPath( Lowercase( pFilePath->Clipbits ) );
	break;

	case IDC_LOCATE_PRNSTYLES:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id, pFilePath->Printstyles,
		sizeof(pFilePath->Printstyles));
	FixPath( Lowercase( pFilePath->Printstyles ) );
	break;
	
	case IDC_LOCATE_BRUSHES:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id, pFilePath->Brushes,
		sizeof(pFilePath->Brushes));
	FixPath( Lowercase( pFilePath->Brushes ) );
	break;

	case IDC_LOCATE_PALETTES:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id,pFilePath->Palettes ,
		sizeof(pFilePath->Palettes));
	FixPath( Lowercase( pFilePath->Palettes ) );
	break;

	case IDC_LOCATE_TEXTURES:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id,pFilePath->Textures ,
		sizeof(pFilePath->Textures));
	FixPath( Lowercase( pFilePath->Textures ) );
	break;

	case IDC_LOCATE_DEVICES:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id,pFilePath->Devices ,
		sizeof(pFilePath->Devices));
	FixPath( Lowercase( pFilePath->Devices ) );
	break;

	case IDC_LOCATE_FILTERS:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id,pFilePath->Effects ,
		sizeof(pFilePath->Effects));
	FixPath( Lowercase( pFilePath->Effects ) );
	break;

	case IDC_LOCATE_MACROS:
    if( !Control.Retail )
        break;
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id,pFilePath->Macros ,
		sizeof(pFilePath->Macros));
	FixPath( Lowercase( pFilePath->Macros ) );
	break;

	case IDC_LOCATE_IMPORTFILTERS:
    if( !Control.Retail )
        break;
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
	    break;
	if ( codeNotify != EN_CHANGE )
	    break;
	GetDlgItemText( hDlg, id,pFilePath->Importfilters ,
		sizeof(pFilePath->Importfilters));
	FixPath( Lowercase( pFilePath->Importfilters ) );
	break;

	case IDC_PREF_SAVE:
	case IDOK:

   if (DoCommandList && !Control.DoCommandList)
      WackCmdList();
	temp = bound( Control.MainMemFactor, 10, 100 );
	if (temp != Control.MainMemFactor)
		{
		Control.MainMemFactor = temp;
		SetDlgItemInt( hDlg, IDC_PREF_MEMFACTOR,
			    Control.MainMemFactor, NO );
		SetFocus ( GetDlgItem(hDlg,IDC_PREF_MEMFACTOR) );
		MessageBeep(0);
		break;
		}
	if (id == IDC_PREF_SAVE)
		SavePreferences();
	if (Control.Units != Units)
		SetupAllRulers();
	if (UseRulers != View.UseRulers)
		EnableRulers(View.UseRulers);
	SetUnitResolution( 0 );
	FrameSettings( Control.RamDisk,
		Control.MainMemMin,
		Control.MainMemFactor);
	if (Control.MultipleObjects != bObjects)
		PictPubApp.EnableObjectMenu(AfxGetMainWnd()->GetMenu());
	lpImage = NULL;
	if (hActiveWnd = GetActiveDoc())
		lpImage = GetImagePtr(hActiveWnd);
	if (!Control.UseMaskAndObjects &&
		Control.UseMaskAndObjects != bObjectMasks)
		{
		Control.UseMaskAndObjects = YES;

		RemoveObjectMarquee(lpImage);
		Control.UseMaskAndObjects = NO;
		}

	if (lpImage && MaskTint != Control.MaskTint &&
		ImgGetViewMode(lpImage) == VM_SLIME)
	{
		RECT rRect;

		rRect = GetDisplayPtr(hActiveWnd)->DispRect;
		AstralToWindowsRect(&rRect);
		InvalidateRect( hActiveWnd, &rRect, FALSE );
	}
	if (!Control.IndependentSettings &&
		Control.IndependentSettings != IndependentSettings)
	{
		SetCommonSettings();
	}
	if (Control.UseTablet)
    {
		Control.UseTablet = TabletStart();
        if (!Control.UseTablet)
        {
            LPSTR    lp;

            AstralStr(IDS_NOTABLET, &lp);
            MessageBox(PictPubApp.Get_hWndAstral(), lp, szAppName, 
               MB_ICONHAND | MB_OK | MB_TASKMODAL);
        }
    }

// Write out the file location stuff

	FixPath(pFilePath->Clipbits);
	PutDefaultString( "ClipBit", pFilePath->Clipbits );
	FixPath( pFilePath->Printstyles ); 
	PutDefaultString( "PrnStyle", pFilePath->Printstyles );
	FixPath( pFilePath->Brushes ); 
	PutDefaultString( "Brush", pFilePath->Brushes );
	FixPath( pFilePath->Palettes ); 
	PutDefaultString( "Palette", pFilePath->Palettes );
	FixPath( pFilePath->Textures ); 
	PutDefaultString( "Texture", pFilePath->Textures );
	FixPath( pFilePath->Devices ); 
	PutDefaultString( "Device", pFilePath->Devices );
	FixPath( pFilePath->Effects ); 
	PutDefaultString( "Effects", pFilePath->Effects );
    if( Control.Retail )
    {
	    FixPath( pFilePath->Macros ); 
	    PutDefaultString( "Macros", pFilePath->Macros );
  	    FixPath( pFilePath->Importfilters ); 
  	    PutDefaultString( "Filters", pFilePath->Importfilters );
    }
	LoadAllExtNamePaths( NO );
	FreeUp (pFilePath);
  	AstralDlgEnd( hDlg, TRUE );
    if (UseGrayScaleIcons != Control.UseGrayScaleIcons)
        SetIconInterfaceType (!Control.UseGrayScaleIcons);
	break;

	case IDCANCEL:
	// Memory Group
	lstrcpy( Control.RamDisk, szRamDisk );
	Control.MainMemMin = MainMemMin;
	Control.MainMemFactor = MainMemFactor;
	Control.LineArtAsGray = LineArtAsGray;

	// Scratchpad Group
	Control.ScratchpadWidth   = ScratchpadWidth;
	Control.ScratchpadHeight  = ScratchpadHeight;
	Control.IsScratchpadColor = IsScratchpadColor;

	// Undo Group
	Control.UseApply = UseApply;
	Control.NoUndo = Undo;
	Control.UndoMasks = UndoMasks;
	Control.UndoObjects = UndoObjects;
	Control.DoCommandList = DoCommandList;

	// Miscellaneous Group
	lstrcpy( Control.PouchPath, szPouchPath );
	lstrcpy( Names.StartMacro, szMacro );
	Control.UseTablet = UseTablet;
	Control.MaskTint = MaskTint;
	Control.IndependentSettings = IndependentSettings;
	Mask.PasteAtRealSize = PasteAtRealSize;
	Control.UseGrayScaleIcons = UseGrayScaleIcons;
	Control.DisableAutoScroll = DisableAutoScroll;
	Vignette.DisableDither = DisableDither;
	Control.DefaultFileType = DefaultFileType;

	// Units Group
	Control.Units = Units;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	Control.ScreenWidth = ScreenWidth;
	View.UseRulers = UseRulers;
	Control.DisplayPixelValue = DisplayPixelValue;
	SetUnitResolution( 0 );

	// Plugin Group
	Control.UsePlugins = UsePlugins;
	lstrcpy( Control.PluginPath1, szPlugin1 );
	lstrcpy( Control.PluginPath2, szPlugin2 );

	// Objects Group
	Control.MultipleObjects = bObjects;
	Control.UseMaskAndObjects = bObjectMasks;

	FreeUp (pFilePath);
	AstralDlgEnd( hDlg, TRUE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL Info_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Info_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgInfoProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Info_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Info_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_IMAGEINFO);
	}
}

/************************************************************************/
LOCAL BOOL Info_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
LPSTR lpString, lpUnit;
LPOBJECT lpObject;
char szString[256], szValue[256], szFormat[256];
int iType, nEdit, nUndo;
long lSize, lFileSize, lEdit, lUndo, lObjects;
long l;
LPMASK lpMask;
LPFRAME	lpFrame;
LPIMAGE lpImage;
char szTitle[MAX_STR_LEN];

CenterPopup( hDlg );
lpImage = GetActiveImage();
if (!lpImage)
	return(TRUE);
lpFrame = ImgGetBaseEditFrame(lpImage);
lstrcpy(szTitle, lpImage->CurFile );
if (Control.WinVer == WV_WINDOWS95)
    stripext(szTitle);
SetDlgItemText(hDlg, IDC_INFOFILENAME, szTitle);
iType = IDI_TIFF + (lpImage->FileType - IDN_TIFF);
     if (AstralStr(iType, &lpString))
	SetDlgItemText(hDlg, IDC_INFOFILETYPE, lpString);
if (AstralStr(lpImage->DataType, &lpString))
	SetDlgItemText(hDlg, IDC_INFODATATYPE, lpString);
if (AstralStr(ImgChanged(lpImage) ? IDS_YES : IDS_NO, &lpString))
	SetDlgItemText(hDlg, IDC_INFOMODIFIED, lpString);
if (AstralStr(IDS_INFOSIZE, &lpString))
	{
	int OldUnits = Control.Units;

	if (Control.Units == IDC_PREF_UNITPIXELS)
		{
		Control.Units = IDC_PREF_UNITINCHES;
		SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
		}

	lstrcpy(szFormat, lpString);
	if ( AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES,
	     &lpUnit ) )
		{
		Unit2String(FGET(FrameXSize(lpFrame), FrameResolution(lpFrame)),
			szValue);
		wsprintf(szString, szFormat, FrameXSize(lpFrame),
			(LPSTR)szValue, lpUnit);
		SetDlgItemText(hDlg, IDC_INFOWIDTH, szString);
		Unit2String(FGET(FrameYSize(lpFrame), FrameResolution(lpFrame)),
			szValue);
		wsprintf(szString, szFormat, FrameYSize(lpFrame),
			(LPSTR)szValue, lpUnit);
		SetDlgItemText(hDlg, IDC_INFOHEIGHT, szString);
		}

	Control.Units = OldUnits;
	SetUnitInfo((UNIT_TYPE)(Control.Units-IDC_PREF_UNITINCHES), Control.UnitRes, Control.Points);
	}
SetDlgItemInt(hDlg, IDC_INFORESOLUTION, FrameResolution(lpFrame), NO);
lObjects = 0;
lpObject = ImgGetBase(lpImage);
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	lpFrame = ObjGetEditFrame(lpObject);
	if (lpFrame)
		lObjects += (long)FrameByteWidth(lpFrame) * (long)FrameYSize(lpFrame);
	if (lpObject->lpAlpha)
		{
		lpFrame = lpObject->lpAlpha->Pixmap.EditFrame;
		if (lpFrame)
			lObjects += (long)FrameByteWidth(lpFrame) * (long)FrameYSize(lpFrame);
		}
	}
if (AstralStr(IDS_INFOIMAGEMEMORY, &lpString))
	{
	lpFrame = ImgGetBaseEditFrame(lpImage);
	lstrcpy(szFormat, lpString);
	l = (long)FrameByteWidth(lpFrame) * (long)FrameYSize(lpFrame);
	l += lObjects; 
	lSize = (l + 512L) / 1024L;
	lFileSize = (FileSize( lpImage->CurFile ) + 512L) / 1024L;
	wsprintf( szString, szFormat, lSize, lFileSize );
	SetDlgItemText(hDlg, IDC_INFOIMAGEMEMORY, szString);
	}

lEdit = lUndo = 0;
nEdit = nUndo = 0;
lpObject = NULL;
while (lpObject = ImgGetNextObject(lpImage, lpObject, YES, NO))
	{
	if (lpFrame = ObjGetEditFrame(lpObject))
		{
		lEdit += (long)FrameInMemory(lpFrame);
		++nEdit;
		}
	if (lpFrame = ObjGetUndoFrame(lpObject))
		{
		lUndo += (long)FrameInMemory(lpFrame);
		++nUndo;
		}
	if (lpObject->lpAlpha &&
		(lpFrame = lpObject->lpAlpha->Pixmap.EditFrame))
		{
		lEdit += (long)FrameInMemory(lpFrame);
		++nEdit;
		}
	if (lpObject->lpAlpha &&
		(lpFrame = lpObject->lpAlpha->Pixmap.UndoFrame))
		{
		lUndo += (long)FrameInMemory(lpFrame);
		++nUndo;
		}
	}
if (nEdit)
	lEdit /= nEdit;
if (nUndo)
	lUndo /= nUndo;
if (AstralStr(IDS_INFOEDITPERCENT, &lpString))
	{
	lstrcpy(szFormat, lpString);
	wsprintf( szString, szFormat, (int)lEdit );
	SetDlgItemText(hDlg, IDC_INFOEDITPERCENT, szString);
	}
if (AstralStr(IDS_INFOUNDOPERCENT, &lpString))
	{
	lstrcpy(szFormat, lpString);
	wsprintf( szString, szFormat, (int)lUndo );
	SetDlgItemText(hDlg, IDC_INFOUNDOPERCENT, szString);
	}
lEdit = lUndo = 0;
if (lpMask = ImgGetMask(lpImage))
	{
	if (lpMask->Pixmap.EditFrame)
		lEdit += FrameInMemory(lpMask->Pixmap.EditFrame);
	if (lpMask->Pixmap.UndoFrame)
		lUndo += FrameInMemory(lpMask->Pixmap.UndoFrame);
	}
if (AstralStr(IDS_INFOMASKEDITPERCENT, &lpString))
	{
	lstrcpy(szFormat, lpString);
	wsprintf( szString, szFormat, (int)lEdit );
	SetDlgItemText(hDlg, IDC_INFOMASKEDITPERCENT, szString);
	}
if (AstralStr(IDS_INFOMASKUNDOPERCENT, &lpString))
	{
	lstrcpy(szFormat, lpString);
	wsprintf( szString, szFormat, (int)lUndo );
	SetDlgItemText(hDlg, IDC_INFOMASKUNDOPERCENT, szString);
	}
if (AstralStr(IDS_INFOMEMORY, &lpString))
	{
	lstrcpy(szFormat, lpString);
	l = (AvailableMemory()+512) / 1024L;
	lpString = szString;
	wsprintf( lpString, szFormat, l );
	SetDlgItemText(hDlg, IDC_INFOMEMORY, szString);
	}
SetDlgItemInt(hDlg, IDC_INFONUMOBJECTS, ImgCountObjects(lpImage)-1, NO);
if (AstralStr(IDS_INFOMEMORY, &lpString))
	{
	lstrcpy(szFormat, lpString);
	lObjects = (lObjects + 512L) / 1024L;
	wsprintf(szString, szFormat, lObjects);
	SetDlgItemText(hDlg, IDC_INFOOBJECTSMEMORY, szString);
	}
SetDlgItemText( hDlg, IDC_CMS_SRCNAME,
	CmsGetPTName( ImgGetBaseEditFrame(lpImage), szString ) );
return(TRUE);
}

/************************************************************************/
LOCAL void Info_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDOK:
	AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
	AstralDlgEnd( hDlg, TRUE );
	break;

   default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL Drivers_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);

/***********************************************************************/
BOOL WINPROC EXPORT DlgDriversProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Drivers_OnInitDialog);  

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, 0);
	}
}

/************************************************************************/
LOCAL BOOL Drivers_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
STRING szFileName, szTime, szSize;
long lSize;
int length;
FILETIMES time;

if (lstrlen(Names.Scanner))
	{
	SetDlgItemText(hDlg, IDC_SCANNAME, Names.Scanner);
	if (LookupExtFile(Names.Scanner, szFileName, IDN_SCANNER))
		{
		Lowercase( szFileName );
		SetDlgItemText(hDlg, IDC_SCANFILE, szFileName);
		if (FileDate(szFileName, &time))
			{
			FileTimeToString(&time, szTime);
			length = lstrlen(szTime);
			while (szTime[length-1] == '\r' ||
			       szTime[length-1] == '\n')
				szTime[--length] = '\0';
			lSize = FileSize(szFileName);
			wsprintf(szSize, "   %ld", lSize);
			lstrcat(szTime, szSize);
			SetDlgItemText(hDlg, IDC_SCANDATE, szTime);
			}
		}
	}
return(TRUE);
}

static FRMDATATYPE InfoType;
static BOOL fInfoDisplayed, fSizeDisplayed;

static void ClearInfo(HWND hDlg, BOOL fNotSize, BOOL fNotPos);

/***********************************************************************/
void DisplayInfo(int cx, int cy, LPRECT lpRect)
/***********************************************************************/
{
HWND hDlg;
int cur_x, cur_y;
int x, y;
int w, h;
int Resolution;
FRMDATATYPE Type;
STRING szValue;
BYTE gray;
RGBS rgb;
CMYKS cmyk;
LPFRAME lpFrame;
RECT rFile;
BOOL fDisplaySize;
HWND hActiveWnd;
LPIMAGE lpImage = NULL;

if (!(hDlg = AstralDlgGet(IDD_INFOFLOAT)))
	return;

if (hActiveWnd = GetActiveDoc())
	lpImage = GetImagePtr(hActiveWnd);
if (lpImage)
	{
	lpFrame = ImgGetBaseEditFrame(lpImage);
	Type = FrameType(lpFrame);
	}
else
	Type = FDT_RGBCOLOR;

fDisplaySize = lpRect != NULL;
if (Type != InfoType || fSizeDisplayed != fDisplaySize)
	{
	InfoType = Type;
	ShowPane(hDlg, IDC_PANE0, IDC_PANE1, TRUE /*lpRect != NULL*/);
	ShowPane(hDlg, IDC_PANE1, IDC_PANE2,
		InfoType == FDT_RGBCOLOR || InfoType == FDT_PALETTECOLOR);
	ShowPane(hDlg, IDC_PANE2, IDC_PANE3,
		InfoType == FDT_CMYKCOLOR );
	ShowPane(hDlg, IDC_PANE3, IDC_PANE4,
		InfoType == FDT_LINEART || InfoType == FDT_GRAYSCALE);

	if (lpImage)
		ClearInfo(hDlg, NO, NO);
//	GetWindowRect(hDlg, &rArea);
//	RxResizeDialog(hDlg, &rArea);
//	ScreenToClient(GetParent(hDlg), (LPPOINT)&rArea.left);
//	SetWindowPos(hDlg, NULL, rArea.left, rArea.top, 0, 0,
//			SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
//	UpdateWindow(hDlg);
	}

if (!hActiveWnd || cx == INT_MIN || cy == INT_MIN)
	{
	ClearInfo(hDlg, NO, NO);
	return;
	}

if (cx == -1 && cy == -1 && lpRect)
	{
	cur_x = lpRect->left;
	cur_y = lpRect->top;
	}
else
	{
	cur_x = cx;
	cur_y = cy;
	Display2File(hActiveWnd, &cur_x, &cur_y);
	}

if (lpRect)
	{
	OrderRect( lpRect, &rFile );
	x = rFile.left;
	y = rFile.top;
	w = RectWidth(&rFile);
	h = RectHeight(&rFile);
	}
else
	{
		x = cur_x;
		y = cur_y;
	}


#ifdef UNUSED
FileRect = GetDisplayPtr(hActiveWnd)->FileRect;

if (x < FileRect.left || x > FileRect.right ||
	y < FileRect.top || y > FileRect.bottom)
	{
	ClearInfo(hDlg, lpRect == NULL, NO);
	}
else
#endif // UNUSED
	{
	Resolution = FrameResolution(lpFrame);
	Unit2String( FGET( x, Resolution ), szValue);
	SetDlgItemTextNow(hDlg, IDC_INFO_XPOS, szValue);
	Unit2String( FGET( y, Resolution ), szValue);
	SetDlgItemTextNow(hDlg, IDC_INFO_YPOS, szValue);
	switch (InfoType)
		{
		case FDT_LINEART:
		ImgGetLine(lpImage, NULL, cur_x, cur_y, 1, &gray);
		gray = GRAY2DISP(gray);
		if (gray == 255)
			gray = 1;
		Int2Ascii(szValue, gray);
		SetDlgItemTextNow(hDlg, IDC_VALUE_GRAY, szValue);
		break;

		case FDT_GRAYSCALE:
		ImgGetLine(lpImage, NULL, cur_x, cur_y, 1, &gray);
		Int2Ascii(szValue, GRAY2DISP(gray));
		SetDlgItemTextNow(hDlg, IDC_VALUE_GRAY, szValue);
		break;

		case FDT_PALETTECOLOR:
		case FDT_RGBCOLOR:
		if (InfoType == FDT_PALETTECOLOR)
		{
			ImgGetLine(lpImage, NULL, cur_x, cur_y, 1, &gray);
			LPCOLORMAP lpColorMap = FrameGetColorMap(lpFrame);
			rgb = lpColorMap->RGBData[gray];
		}
		else
			ImgGetLine(lpImage, NULL, cur_x, cur_y, 1, (LPTR)&rgb);
		Int2Ascii(szValue, PIX2DISP(rgb.red));
		SetDlgItemTextNow(hDlg, IDC_VALUE_R, szValue);
		Int2Ascii(szValue, PIX2DISP(rgb.green));
		SetDlgItemTextNow(hDlg, IDC_VALUE_G, szValue);
		Int2Ascii(szValue, PIX2DISP(rgb.blue));
		SetDlgItemTextNow(hDlg, IDC_VALUE_B, szValue);
		break;

		case FDT_CMYKCOLOR:
		ImgGetLine(lpImage, NULL, cur_x, cur_y, 1, (LPTR)&cmyk);
		Int2Ascii(szValue, PIX2DISP(cmyk.c));
		SetDlgItemTextNow(hDlg, IDC_VALUE_C, szValue);
		Int2Ascii(szValue, PIX2DISP(cmyk.m));
		SetDlgItemTextNow(hDlg, IDC_VALUE_M, szValue);
		Int2Ascii(szValue, PIX2DISP(cmyk.y));
		SetDlgItemTextNow(hDlg, IDC_VALUE_Y, szValue);
		Int2Ascii(szValue, PIX2DISP(cmyk.k));
		SetDlgItemTextNow(hDlg, IDC_VALUE_K, szValue);
		break;

		default:
			break;
		}
	fInfoDisplayed = YES;
	}
if (lpRect)
	{
	Unit2String( FGET( w, Resolution ), szValue);
	SetDlgItemTextNow(hDlg, IDC_INFO_WIDTH, szValue);
	Unit2String( FGET( h, Resolution ), szValue);
	SetDlgItemTextNow(hDlg, IDC_INFO_HEIGHT, szValue);
	fSizeDisplayed = YES;
	}
}

/***********************************************************************/
static void ClearInfo(HWND hDlg, BOOL fNotSize, BOOL fNotPos)
/***********************************************************************/
{
if (!fInfoDisplayed)
	return;

if (!fNotPos)
	{
	switch (InfoType)
		{
		case FDT_LINEART:
		case FDT_GRAYSCALE:
			SetDlgItemText(hDlg, IDC_VALUE_GRAY, " ");
		break;

		case FDT_RGBCOLOR:
		case FDT_PALETTECOLOR:
			SetDlgItemText(hDlg, IDC_VALUE_R, " ");
			SetDlgItemText(hDlg, IDC_VALUE_G, " ");
			SetDlgItemText(hDlg, IDC_VALUE_B, " ");
		break;

		case FDT_CMYKCOLOR:
			SetDlgItemText(hDlg, IDC_VALUE_C, " ");
			SetDlgItemText(hDlg, IDC_VALUE_M, " ");
			SetDlgItemText(hDlg, IDC_VALUE_Y, " ");
			SetDlgItemText(hDlg, IDC_VALUE_K, " ");
		break;
		}
	SetDlgItemText(hDlg, IDC_INFO_XPOS, " ");
	SetDlgItemText(hDlg, IDC_INFO_YPOS, " ");
	fInfoDisplayed = NO;
	}
if (!fNotSize && fSizeDisplayed)
	{
	SetDlgItemText(hDlg, IDC_INFO_WIDTH, " ");
	SetDlgItemText(hDlg, IDC_INFO_HEIGHT, " ");
	fSizeDisplayed = NO;
	}
}

// LOCAL prototypes
LOCAL BOOL InfoFloat_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void InfoFloat_OnClose(HWND hDlg);

/***********************************************************************/
BOOL WINPROC EXPORT DlgInfoFloatProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, InfoFloat_OnInitDialog);  

	HANDLE_DLGMSG(hDlg, WM_CLOSE, InfoFloat_OnClose);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_INFOFLOAT);
	}
}

/************************************************************************/
LOCAL BOOL InfoFloat_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
if (Control.fNewLook)
	SetSmallCaption(hDlg);
RestorePopup( hDlg, "xyInfo", RightPopup );
InfoType = FDT_RGBCOLOR;
ShowPane(hDlg, IDC_PANE0, IDC_PANE1, TRUE);
ShowPane(hDlg, IDC_PANE1, IDC_PANE2,
	InfoType == FDT_RGBCOLOR || InfoType == FDT_PALETTECOLOR);
ShowPane(hDlg, IDC_PANE2, IDC_PANE3,
	InfoType == FDT_CMYKCOLOR );
ShowPane(hDlg, IDC_PANE3, IDC_PANE4,
	InfoType == FDT_LINEART || InfoType == FDT_GRAYSCALE);
return(TRUE);
}

/************************************************************************/
LOCAL void InfoFloat_OnClose(HWND hDlg)
/************************************************************************/
{
InfoType = FDT_RGBCOLOR;
Dialog_OnClose(hDlg);
}

LOCAL BOOL Ribbon_OnEraseBkgnd(HWND hWindow, HDC hDC);
LOCAL void Ribbon_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);

/***********************************************************************/
LONG WINPROC EXPORT RibbonDlgProc(	HWND hWindow, UINT message,
								WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Ribbon_OnEraseBkgnd);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Ribbon_OnLButtonDown);

	default:
	return DefDlgProc(hWindow, message, wParam, lParam);
	}
}

/***********************************************************************/
LOCAL void Ribbon_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
FORWARD_WM_LBUTTONDOWN(GetParent(hWindow), fDoubleClick, x, y, keyFlags, SendMessage );
}

/***********************************************************************/
LOCAL BOOL Ribbon_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
RECT rClient;

GetClientRect(hWindow, &rClient);
FillRect( hDC, &rClient, hBrush );
DeleteObject(hBrush);
return(TRUE);
}

