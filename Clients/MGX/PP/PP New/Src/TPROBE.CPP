// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "pptbars.h"
#include "svrview.h"

// Static prototypes
static COLOR GetColor( LPCOLORINFO lpColor, COLORSPACE ColorSpace );
static COLORSPACE GetFrameTypeColorSpace( FRMDATATYPE DataType );

// Probe tool statics
static RECT SelectRect;

// LOCAL prototypes
LOCAL BOOL Probe_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void Probe_OnDocActivated(HWND hDlg);
LOCAL void Probe_OnControlEnable(HWND hDlg, int fFlags);
LOCAL void Probe_HandleEnable(HWND hDlg, int fFlags);
LOCAL void Probe_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgProbeProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, Probe_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, Probe_OnCommand);
	HANDLE_DLGMSG(hDlg, WM_DOCACTIVATED, Probe_OnDocActivated);
	HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, Probe_OnControlEnable);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PROBE);
	}
}

/************************************************************************/
LOCAL BOOL Probe_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CheckComboItem( hDlg, IDC_PROBEMETHOD, IDC_PROBEPOINT,
		IDC_PROBERECT, ColorMask.ProbeMethod );
return(FALSE);
}

/************************************************************************/
LOCAL void Probe_OnDocActivated(HWND hDlg)
/************************************************************************/
{
Probe_HandleEnable(hDlg, 0);
}

/************************************************************************/
LOCAL void Probe_OnControlEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
Probe_HandleEnable(hDlg, fFlags);
}

/************************************************************************/
LOCAL void Probe_HandleEnable(HWND hDlg, int fFlags)
/************************************************************************/
{
}

/************************************************************************/
LOCAL void Probe_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_PROBE:
	if ( !SaveToolPreferences( id ) )
		break;
	break;

	case IDC_PROBEMETHOD:
   case IDC_PROBEPOINT:
	case IDC_PROBERECT:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	ColorMask.ProbeMethod = id;
	break;

   default:
	break;
   }
}

/************************************************************************/
void ProbeProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option )
/************************************************************************/
{
LPRECT lpRect;
COLORINFO ProbeColor;
POINT pt;
static BOOL fDoArea;
int    xx,yy;

switch (msg)
    {
    case WM_CREATE:	// The first mouse down message
	if (!IsOnImage(hWindow, x, y))
		break;
	AstralUpdateWindow(hWindow);
	Tool.bActive = YES;
	break;

    case WM_LBUTTONDOWN:
	fDoArea = (ColorMask.ProbeMethod == IDC_PROBERECT) ^ SHIFT;
	if ( fDoArea )
		{
		AstralSetRectEmpty( &SelectRect );
		pt.x = x; pt.y = y;
		StartSelection( hWindow, NULL, &SelectRect, SL_BOX | SL_SPECIAL,
			pt, 0L );
		}
	else
		{
		Display2File(hWindow,&x, &y);
		SelectRect.left = SelectRect.right = x;
		SelectRect.top = SelectRect.bottom = y;
		if ( Measure( GetImagePtr(hWindow), &SelectRect, &ProbeColor ) )
			{
			ColorStatus( &ProbeColor, 0L, NULL, YES );
			SetActiveColor( &ProbeColor, ProbeColor.ColorSpace, FALSE );
			}
		}
	break;

    case WM_LBUTTONUP:
	Tool.bActive = NO;
	if ( fDoArea )
		{
		EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
		if ( Measure( GetImagePtr(hWindow), &SelectRect, &ProbeColor ) )
			{
			ColorStatus( &ProbeColor, 0L, NULL, YES );
			SetActiveColor( &ProbeColor, ProbeColor.ColorSpace, FALSE );
			}
		}
	break;

    case WM_MOUSEMOVE:	// sent when ToolActive is on
	// Bound the point to the window
	lpRect = &GetDisplayPtr(hWindow)->DispRect;
	x = bound( x, lpRect->left, lpRect->right );
	y = bound( y, lpRect->top, lpRect->bottom );
	if ( fDoArea )
		{
		pt.x = x; pt.y = y;
		UpdateSelection( hWindow, NULL, &SelectRect, SL_BOX,
			pt, CONSTRAINASPECT, 1L, 1L, MOVEKEY||Window.fRButtonDown,FROMCENTER);
		}
	else
		{
      xx = x; yy = y;
		Display2File(hWindow,&xx, &yy);
		SelectRect.left = SelectRect.right = xx;
		SelectRect.top = SelectRect.bottom = yy;
		if ( Measure( GetImagePtr(hWindow), &SelectRect, &ProbeColor ) )
			{
			ColorStatus( &ProbeColor, 0L, NULL, YES );
			SetActiveColor( &ProbeColor, ProbeColor.ColorSpace, FALSE );
			}
		DisplayInfo( x, y, NULL );
		}
	break;

    case WM_LBUTTONDBLCLK:
	break;

    case WM_DESTROY:	// The cancel operation message
        if ( !Tool.bActive )
		break;
	Tool.bActive = NO;
	if ( !fDoArea )
		break;
	EndSelection( hWindow, NULL, &SelectRect, SL_BOX, YES );
	break;
    }
}

static COLORINFO ActiveColor;
static COLORINFO AlternateColor;
BOOL      ActiveColorIsActive = TRUE;

/************************************************************************/
void SetColorInfo( LPCOLORINFO lpSrcColor, LPCOLORINFO lpDstColor,
                     COLORSPACE ColorSpace )
/************************************************************************/
{
	if (ColorSpace < CS_GRAY || ColorSpace > CS_CMYK)
		ColorSpace = CS_GRAY;
	lpDstColor->ColorSpace = ColorSpace;

	switch(ColorSpace)
	{
		// Original Color Space is Gray
		case CS_GRAY :
		{
			unsigned char Gray = lpSrcColor->gray;

			// Set up the Gray Color Entry
			lpDstColor->gray = Gray;

			// Set up the RGB Color Entry
			lpDstColor->rgb.red   = Gray;
			lpDstColor->rgb.green = Gray;
			lpDstColor->rgb.blue  = Gray;

			// Set up the CMYK Color Entry
			RGBtoCMYK(Gray, Gray, Gray, &lpDstColor->cmyk);

			// Set up the HSL Color Entry
			RGBtoHSL(Gray, Gray, Gray, &lpDstColor->hsl);
		}
		break;

		case CS_HSL  :
		{
			HSLS hsl;

			hsl.hue = lpSrcColor->hsl.hue;
			hsl.sat = lpSrcColor->hsl.sat;
			hsl.lum = lpSrcColor->hsl.lum;

			// Set up the HSL Color Entry
			lpDstColor->hsl.hue = hsl.hue;
			lpDstColor->hsl.sat = hsl.sat;
			lpDstColor->hsl.lum = hsl.lum;

			// Set up the RGB Color Entry
			HSLtoRGB(
				hsl.hue,
				hsl.sat,
				hsl.lum,
				&lpDstColor->rgb);

			// Set up the GRAY Color Entry
			lpDstColor->gray = TOLUM(
				lpDstColor->rgb.red,
				lpDstColor->rgb.green,
				lpDstColor->rgb.blue);

			// Set up the CMYK Color Entry
			RGBtoCMYK(
				lpDstColor->rgb.red,
				lpDstColor->rgb.green,
				lpDstColor->rgb.blue,
				&lpDstColor->cmyk);
		}
		break;

		case CS_RGB  :
		{
			RGBS rgb;

			rgb.red   = lpSrcColor->rgb.red;
			rgb.green = lpSrcColor->rgb.green;
			rgb.blue  = lpSrcColor->rgb.blue;

			// Set up the RGB Color Entry
			lpDstColor->rgb.red   = rgb.red;
			lpDstColor->rgb.green = rgb.green;
			lpDstColor->rgb.blue  = rgb.blue;

			// Set up the GRAY Color Entry
			lpDstColor->gray = TOLUM(
				lpDstColor->rgb.red,
				lpDstColor->rgb.green,
				lpDstColor->rgb.blue);

			// Set up the HSL Color Entry
			RGBtoHSL(
				rgb.red,
				rgb.green,
				rgb.blue,
				&lpDstColor->hsl);

			// Set up the CMYK Color Entry
			RGBtoCMYK(rgb.red, rgb.green, rgb.blue, &lpDstColor->cmyk);
		}
		break;

		case CS_CMYK :
		{
			CMYKS cmyk;

			cmyk.c = lpSrcColor->cmyk.c;
			cmyk.m = lpSrcColor->cmyk.m;
			cmyk.y = lpSrcColor->cmyk.y;
			cmyk.k = lpSrcColor->cmyk.k;

			// Set up the CMYK Color Entry
			lpDstColor->cmyk.c = cmyk.c;
			lpDstColor->cmyk.m = cmyk.m;
			lpDstColor->cmyk.y = cmyk.y;
			lpDstColor->cmyk.k = cmyk.k;

			// Set up the RGB Color Entry
			CMYKtoRGB(
				cmyk.c,
				cmyk.m,
				cmyk.y,
				cmyk.k,
				&lpDstColor->rgb);

			// Set up the GRAY Color Entry
			lpDstColor->gray = TOLUM(
				lpDstColor->rgb.red,
				lpDstColor->rgb.green,
				lpDstColor->rgb.blue);

			// Set up the HSL Color Entry
			RGBtoHSL(
				lpDstColor->rgb.red,
				lpDstColor->rgb.green,
				lpDstColor->rgb.blue,
				&lpDstColor->hsl);
		}
		break;
	}

#ifdef UNUSED
	// Force to gray if it really is gray
	if (ColorSpace != CS_GRAY && ColorSpace != CS_CMYK)
		if (lpDstColor->rgb.red == lpDstColor->rgb.green && 
			lpDstColor->rgb.red == lpDstColor->rgb.blue)
			lpDstColor->ColorSpace = CS_GRAY;
#endif // UNUSED

}

/************************************************************************/
void SetActiveColorString( LPSTR lpString, COLORSPACE ColorSpace )
/************************************************************************/
{
COLORINFO ColorInfo;

SetColorInfoString( lpString, &ColorInfo, ColorSpace );
SetColorInfo( &ColorInfo, &ActiveColor, ColorSpace );
}

/************************************************************************/
void SetAlternateColorString( LPSTR lpString, COLORSPACE ColorSpace )
/************************************************************************/
{
COLORINFO ColorInfo;

SetColorInfoString( lpString, &ColorInfo, ColorSpace );
SetColorInfo( &ColorInfo, &AlternateColor, ColorSpace );
}

/************************************************************************/
void SetColorInfoString( LPSTR lpString, LPCOLORINFO lpColorInfo,
	COLORSPACE ColorSpace )
/************************************************************************/
{
int a, b, c, d;

AsciiToInt4( lpString, &a, &b, &c, &d );
switch( ColorSpace )
	{
	case CS_HSL:
		lpColorInfo->hsl.hue = a;
		lpColorInfo->hsl.sat = b;
		lpColorInfo->hsl.lum = c;
		break;

	case CS_RGB:
		lpColorInfo->rgb.red   = a;
		lpColorInfo->rgb.green = b;
		lpColorInfo->rgb.blue  = c;
		break;

	case CS_CMYK:
		lpColorInfo->cmyk.c = a;
		lpColorInfo->cmyk.m = b;
		lpColorInfo->cmyk.y = c;
		lpColorInfo->cmyk.k = d;
		break;

	default:
	case CS_GRAY:
		lpColorInfo->gray = a;
		break;

	}
}

/************************************************************************/
COLORSPACE GetActiveColorString( LPSTR lpString )
/************************************************************************/
{
return( GetColorInfoString( lpString, &ActiveColor ) );
}

/************************************************************************/
COLORSPACE GetAlternateColorString( LPSTR lpString )
/************************************************************************/
{
return( GetColorInfoString( lpString, &AlternateColor ) );
}

/************************************************************************/
COLORSPACE GetColorInfoString( LPSTR lpString, LPCOLORINFO lpColorInfo )
/************************************************************************/
{
switch( lpColorInfo->ColorSpace )
	{
	case CS_HSL:
		Int4ToAscii( lpString, lpColorInfo->hsl.hue, lpColorInfo->hsl.sat,
			lpColorInfo->hsl.lum, 0 );
		break;

	case CS_RGB:
		Int4ToAscii( lpString, lpColorInfo->rgb.red, lpColorInfo->rgb.green,
			lpColorInfo->rgb.blue, 0 );
		break;

	case CS_CMYK:
		Int4ToAscii( lpString, lpColorInfo->cmyk.c, lpColorInfo->cmyk.m,
			lpColorInfo->cmyk.y, lpColorInfo->cmyk.k );
		break;
	default:
		lpColorInfo->ColorSpace = CS_GRAY;
	case CS_GRAY:
		Int4ToAscii( lpString, lpColorInfo->gray, 0, 0, 0 );
		break;
	}

return( lpColorInfo->ColorSpace );
}

/************************************************************************/
void RevertColorSwatch(void)
/************************************************************************/
   {
   ActiveColorIsActive = TRUE;
	ActiveColor.gray = 0x00;
	SetColorInfo(&ActiveColor, &ActiveColor, CS_GRAY);
	AlternateColor.gray = 0xFF;
	SetColorInfo(&AlternateColor, &AlternateColor , CS_GRAY);
   PictPubApp.GetCustomBoxes()->InvalidateColorPatches(TRUE);
   }

/************************************************************************/
void SwapActiveAlternate(void)
/************************************************************************/
{
COLORINFO TempColor = ActiveColor;

ActiveColorIsActive = !ActiveColorIsActive;

ActiveColor = AlternateColor;
AlternateColor = TempColor;
PictPubApp.GetCustomBoxes()->InvalidateColorPatches(TRUE);
}

/************************************************************************/
void SetActiveColor( LPCOLORINFO lpColor, COLORSPACE ColorSpace,
                        BOOL bToolBar )
/************************************************************************/
{


SetColorInfo( lpColor, &ActiveColor, ColorSpace );
if (!bToolBar)
	PictPubApp.GetCustomBoxes()->NotifyColorPatches(2);
}

/************************************************************************/
void SetActiveRGB(LPRGB lpRGB, BOOL bToolBar)
/************************************************************************/
{


ActiveColor.rgb = *lpRGB;
SetColorInfo( (LPCOLORINFO)&ActiveColor, (LPCOLORINFO)&ActiveColor, CS_RGB );
if (!bToolBar)
   PictPubApp.GetCustomBoxes()->NotifyColorPatches(2);
}

/************************************************************************/
COLORSPACE GetActiveColor( LPCOLORINFO lpColor )
/************************************************************************/
{
*lpColor = ActiveColor;

return(ActiveColor.ColorSpace);
}

/************************************************************************/
COLORSPACE GetActiveColorFromTypeInfo( FRMTYPEINFO TypeInfo, LPCOLORINFO lpColor )
/************************************************************************/
{
COLORSPACE Space;

Space = GetActiveColorFromType(TypeInfo.DataType, lpColor);
GetColorFromTypeInfo(TypeInfo, lpColor);
return(Space);
}

/************************************************************************/
COLORSPACE GetAlternateColorFromTypeInfo( FRMTYPEINFO TypeInfo, LPCOLORINFO lpColor )
/************************************************************************/
{
COLORSPACE Space;

Space = GetAlternateColorFromType(TypeInfo.DataType, lpColor);
GetColorFromTypeInfo(TypeInfo, lpColor);
return(Space);
}

/************************************************************************/
LPCOLORINFO GetColorFromTypeInfo(FRMTYPEINFO TypeInfo, LPCOLORINFO lpColor)
/************************************************************************/
{
if (lpColor)
	{
	if (TypeInfo.DataType == FDT_PALETTECOLOR)
		lpColor->index = FrameGetNearestIndex(TypeInfo.ColorMap, &lpColor->rgb);
	}
return(lpColor);
}

/************************************************************************/
COLORSPACE GetActiveColorFromType( FRMDATATYPE Type, LPCOLORINFO lpColor )
/************************************************************************/
{
if (Type == FDT_LINEART)
	{
   if (ActiveColorIsActive)
		lpColor->gray = 0x00;
   else
		lpColor->gray = 0xFF;
	SetColorInfo(lpColor, lpColor, CS_GRAY);
	}
else
	*lpColor = ActiveColor;

return(lpColor->ColorSpace);
}

/************************************************************************/
COLORSPACE GetAlternateColorFromType( FRMDATATYPE Type, LPCOLORINFO lpColor )
/************************************************************************/
{



if (Type == FDT_LINEART)
	{
   if (!ActiveColorIsActive)
		lpColor->gray = 0x00;
   else
		lpColor->gray = 0xFF;
	SetColorInfo(lpColor, lpColor, CS_GRAY);
	}
else
	*lpColor = AlternateColor;

return(lpColor->ColorSpace);
}

/************************************************************************/
LPCOLORINFO GetActiveColorInfo( void )
/************************************************************************/
{
return((LPCOLORINFO)&ActiveColor);
}

/************************************************************************/
LPRGB GetActiveRGB(LPRGB lpRGB)
/************************************************************************/
{
if (lpRGB)
	*lpRGB = ActiveColor.rgb;

return(&ActiveColor.rgb);
}

/************************************************************************/
void SetAlternateColor( LPCOLORINFO lpColor, COLORSPACE ColorSpace,
                           BOOL bToolBar )
/************************************************************************/
{

SetColorInfo( lpColor, &AlternateColor, ColorSpace );
if (!bToolBar)
	PictPubApp.GetCustomBoxes()->NotifyColorPatches(4);
}

/************************************************************************/
void SetAlternateRGB(LPRGB lpRGB, BOOL bToolBar)
/************************************************************************/
{


AlternateColor.rgb = *lpRGB;
SetColorInfo( (LPCOLORINFO)&AlternateColor, (LPCOLORINFO)&AlternateColor,
	CS_RGB );
if (!bToolBar)
   PictPubApp.GetCustomBoxes()->NotifyColorPatches(4);
}

/************************************************************************/
COLORSPACE GetAlternateColor( LPCOLORINFO lpColor )
/************************************************************************/
{
*lpColor = AlternateColor;

return(AlternateColor.ColorSpace);
}

/************************************************************************/
LPCOLORINFO GetAlternateColorInfo( void )
/************************************************************************/
{
return((LPCOLORINFO)&AlternateColor);
}

/************************************************************************/
static COLOR GetColor( LPCOLORINFO lpColor, COLORSPACE ColorSpace )
/************************************************************************/
{
COLOR Color;

switch(ColorSpace)
{
	case CS_GRAY :
		Color = lpColor->gray;
	break;

	case CS_HSL  :
		Color = 0;
		CopyRGB( &lpColor->hsl, &Color );
	break;

	case CS_RGB  :
		Color = 0;
		CopyRGB( &lpColor->rgb, &Color );
	break;

	case CS_CMYK :
		CopyCMYK( &lpColor->cmyk, &Color );
	break;
}

return(Color);
}

/************************************************************************/
LPRGB GetAlternateRGB(LPRGB lpRGB)
/************************************************************************/
{
if (lpRGB)
	*lpRGB = AlternateColor.rgb;

return(&AlternateColor.rgb);
}

/************************************************************************/
COLOR GetFrameColor( LPCOLORINFO lpColor, FRMDATATYPE DataType)
/************************************************************************/
{
COLORSPACE ColorSpace;

ColorSpace = GetFrameTypeColorSpace(DataType);
return(GetColor(lpColor, ColorSpace));
}

/************************************************************************/
COLOR GetFrameColor( LPCOLORINFO lpColor, FRMTYPEINFO TypeInfo)
/************************************************************************/
{
COLORSPACE ColorSpace;

GetColorFromTypeInfo(TypeInfo, lpColor);
if (TypeInfo.DataType == FDT_PALETTECOLOR)
	return(lpColor->index);
else
	{
	ColorSpace = GetFrameTypeColorSpace(TypeInfo.DataType);
	return(GetColor(lpColor, ColorSpace));
	}
}

/************************************************************************/
static COLORSPACE GetFrameTypeColorSpace( FRMDATATYPE DataType )
/************************************************************************/
{
	switch( DataType )
	{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
			return(CS_GRAY);
		break;

		case FDT_RGBCOLOR :
		case FDT_PALETTECOLOR :
			return(CS_RGB);
		break;

		case FDT_CMYKCOLOR :
			return(CS_CMYK);
		break;

		default:
			return(CS_RGB);
		break;
	}
}

/************************************************************************/
BOOL ColorsEqual( LPCOLORINFO lpSrcColor, LPCOLORINFO lpDstColor)
/************************************************************************/
{
	BOOL result = TRUE;

	// Check Colorspace
	if (lpSrcColor->ColorSpace != lpDstColor->ColorSpace) result = FALSE;

	// Check Gray
	if (lpSrcColor->gray != lpDstColor->gray) result = FALSE;

	// Check RGB
	if (lpSrcColor->rgb.red   != lpDstColor->rgb.red  ) result = FALSE;
	if (lpSrcColor->rgb.green != lpDstColor->rgb.green) result = FALSE;
	if (lpSrcColor->rgb.blue  != lpDstColor->rgb.blue ) result = FALSE;

	// Check HSL
	if (lpSrcColor->hsl.hue != lpDstColor->hsl.hue) result = FALSE;
	if (lpSrcColor->hsl.sat != lpDstColor->hsl.sat) result = FALSE;
	if (lpSrcColor->hsl.lum != lpDstColor->hsl.lum) result = FALSE;

	// Check CMYK
	if (lpSrcColor->cmyk.c != lpDstColor->cmyk.c) result = FALSE;
	if (lpSrcColor->cmyk.m != lpDstColor->cmyk.m) result = FALSE;
	if (lpSrcColor->cmyk.y != lpDstColor->cmyk.y) result = FALSE;
	if (lpSrcColor->cmyk.k != lpDstColor->cmyk.k) result = FALSE;

	return(result);
}



static BOOL bProbe;
//***********************************************************************
BOOL QuickProbeStart( HWND hWnd )
//***********************************************************************
{
if ( bProbe )
	return( NO );
SetCapture( hWnd );bProbe = YES;
return( YES );
}


//***********************************************************************
BOOL QuickProbeEnd( HWND hWnd, int x, int y )
//***********************************************************************
{
POINT pt;

if ( !bProbe )
	return( NO );
if ( x > 0  && y > 0)
	{ // If probing and clicked on this window (or a child), abort the probe
	pt.x = x;
	pt.y = y;
	if ( !ChildWindowFromPoint( hWnd, pt ) )
		return( NO );
	}
ReleaseCapture(); bProbe = NO;
return( YES );
}


//***********************************************************************
BOOL QuickProbeGet( HWND hWnd, int x, int y, LPCOLORINFO lpProbeColor )
//***********************************************************************
{
POINT pt;
RECT DisplayRect;
HWND hWndMDI, hWndImage;
BOOL bRet;

if ( !bProbe )
	return( NO );

SetCursor( LoadCursor( PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(ID_PROBE) ) );
pt.x = x;
pt.y = y;
// See if the point is over an image window
// get MDI window

CServerView    *pView = PictPubApp.GetActiveView();
CServerDoc     *pDoc  = pView->GetDocument();
BOOL            InPlace = pDoc && pDoc->IsInPlaceActive();

if (!InPlace)
   {
   MapWindowPoints( hWnd, PictPubApp.Get_hClientAstral(), &pt, 1 );
   hWndMDI = ChildWindowFromPoint(PictPubApp.Get_hClientAstral(),pt);
   if (!hWndMDI)
	   return( NO );
   // get view window
   MapWindowPoints( PictPubApp.Get_hClientAstral(), hWndMDI, &pt, 1 );
   hWndImage = ChildWindowFromPoint(hWndMDI,pt);
   if (!hWndImage)
	   hWndImage = hWndMDI;
   }
else
   hWndImage = pView->GetSafeHwnd();

if ( !IsDoc(hWndImage  ) )
   return( NO );
pt.x = x;
pt.y = y;
MapWindowPoints( hWnd, hWndImage, &pt, 1 );

// If over an image, read a pixel
Display2File(hWndImage, (LPINT)&pt.x, (LPINT)&pt.y);
DisplayRect.left = DisplayRect.right = pt.x;
DisplayRect.top = DisplayRect.bottom = pt.y;

if ( bRet = Measure( GetImagePtr(hWndImage), &DisplayRect, lpProbeColor ) )
	ColorStatus( lpProbeColor, 0L, NULL, YES );
return( bRet );
}
