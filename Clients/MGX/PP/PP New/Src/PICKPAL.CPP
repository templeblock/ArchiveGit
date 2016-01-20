//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "pickpal.h"

static COLORINFO  OriginalColor;
static COLORINFO  AltOriginalColor;
static COLORINFO  AltPickerColor;
static COLORINFO  PickerColor;
static COLORSPACE PickerSpace;
static BOOL bHaveAlternate;
static BOOL bAlternateActive;
static LPCOLORMAP PickColorMap;

/***********************************************************************/
BOOL PalettePicker( LPCOLORINFO lpColor, LPCOLORINFO lpAltColor, LPCOLORMAP lpColorMap )
/***********************************************************************/
{
	HWND hWnd, hWndFocus;
	int iSelect;
	COLORSPACE Space;

	// Get the proper parent window
	hWndFocus = GetFocus();

	if ( !(hWnd = GetParent(hWndFocus)) )
		hWnd = hWndFocus;

	PickerColor   = *lpColor;
	OriginalColor = PickerColor;
	bAlternateActive = NO;
	if (bHaveAlternate = (lpAltColor != NULL))
	{
		AltPickerColor = *lpAltColor;
		AltOriginalColor = AltPickerColor;
	}

	PickColorMap = lpColorMap;
	// make sure the colors are in our palettes
	if (lpColorMap)
	{
		Space = CS_RGB;
		iSelect = FrameGetNearestIndex(lpColorMap, &PickerColor.rgb);
		PickerColor.rgb = lpColorMap->RGBData[iSelect];
		iSelect = FrameGetNearestIndex(lpColorMap, &OriginalColor.rgb);
		OriginalColor.rgb = lpColorMap->RGBData[iSelect];
		if (bHaveAlternate)
		{
			iSelect = FrameGetNearestIndex(lpColorMap, &AltPickerColor.rgb);
			AltPickerColor.rgb = lpColorMap->RGBData[iSelect];
			iSelect = FrameGetNearestIndex(lpColorMap, &AltOriginalColor.rgb);
			AltOriginalColor.rgb = lpColorMap->RGBData[iSelect];
		}
	}
	else
	{
		Space = CS_GRAY;
	}

	SetColorInfo(&PickerColor, &PickerColor, Space);
	SetColorInfo(&OriginalColor, &OriginalColor, Space);
	if (bHaveAlternate)
	{
		SetColorInfo(&AltPickerColor, &AltPickerColor, Space);
		SetColorInfo(&AltOriginalColor, &AltOriginalColor, Space);
	}
		
	// Show the Color Picker Dialog
	if ( !AstralDlgParam( 
		NO, PictPubApp.GetResourceHandle(), 
		hWnd, IDD_PALETTEPICKER, 
		DlgPalettePickerProc,
		(LPARAM)lpColorMap) )
	{
		return( FALSE );
	}

	// Set the new color
	if (bAlternateActive)
	{
		*lpColor = AltPickerColor;
		if (lpAltColor)
			*lpAltColor = PickerColor;
	}
	else
	{
		*lpColor = PickerColor;
		if (lpAltColor)
			*lpAltColor = AltPickerColor;
	}

	SetFocus( hWndFocus );

	return( TRUE );
}

// LOCAL prototypes
LOCAL BOOL PalettePicker_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PalettePicker_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPalettePickerProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PalettePicker_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PalettePicker_OnCommand);

	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, Dialog_OnQueryNewPalette);
	HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, Dialog_OnPaletteChanged);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTEPICKER);
	}
}

/************************************************************************/
LOCAL BOOL PalettePicker_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
COLOR Color;
LPCOLORMAP lpColorMap;
int iSelect;
STRING szColor;

CenterPopup( hDlg );

// lParam - lpColorMap pointer
lpColorMap = (LPCOLORMAP)lParam;
if (lpColorMap)
	iSelect = FrameGetNearestIndex(lpColorMap, &PickerColor.rgb);
else
	iSelect = PickerColor.gray;
if (PickColorMap)
{
	PickerColor.rgb = PickColorMap->RGBData[iSelect];
	SetColorInfo(&PickerColor, &PickerColor, CS_RGB);
}
else
{
	PickerColor.gray = iSelect;
	SetColorInfo(&PickerColor, &PickerColor, CS_GRAY);
}
PalPicker_Init(GetDlgItem(hDlg, IDC_PALETTEPICKER), lpColorMap, iSelect);


if (!bHaveAlternate)
	ShowControl(hDlg, IDC_ACTIVE_SELECT, IDC_ALTERNATE_SELECT, NO, NO);
if (bHaveAlternate)
{
	CheckDlgButton(hDlg,
		IDC_ACTIVE_SELECT, !bAlternateActive);
	CheckDlgButton(hDlg,
		IDC_ALTERNATE_SELECT, bAlternateActive);
}

if (Control.DisplayPixelValue)
	SetDlgItemTextMultiple(hDlg, IDC_PERCENT0, IDC_PERCENT6, "");

CopyRGB(&PickerColor.rgb, &Color);
SetWindowLong( GetDlgItem( hDlg,
			 IDC_PICK_PATCH1 ), GWL_ACTIVE, Color );
SetWindowLong( GetDlgItem( hDlg,
			 IDC_PICK_PATCH2 ), GWL_ACTIVE, Color );

GetColorString(&PickerColor, PickerColor.ColorSpace, szColor);
SetDlgItemText(hDlg, IDC_PALETTECOLORVALUE, szColor);

return(TRUE);
}

/************************************************************************/
LOCAL void PalettePicker_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
COLORINFO OldColor, temp;
COLOR Color;
int iSelect;
STRING szColor;

OldColor = PickerColor;
switch (id)
	{
	case IDC_ACTIVE_SELECT:
	case IDC_ALTERNATE_SELECT:
	if (id == IDC_ACTIVE_SELECT && !bAlternateActive)
		break;
	if (id == IDC_ALTERNATE_SELECT && bAlternateActive)
		break;
	bAlternateActive = !bAlternateActive;
	CheckDlgButton(hDlg,
		IDC_ACTIVE_SELECT, !bAlternateActive);
	CheckDlgButton(hDlg,
		IDC_ALTERNATE_SELECT, bAlternateActive);
	temp = PickerColor;
	PickerColor = AltPickerColor;
	AltPickerColor = temp;
	temp = OriginalColor;
	OriginalColor = AltOriginalColor;
	AltOriginalColor = temp;
	CopyRGB(&OriginalColor.rgb, &Color);
	SetWindowLong( GetDlgItem( hDlg,
			 IDC_PICK_PATCH2 ), GWL_ACTIVE, Color );
	AstralControlRepaint( hDlg,	IDC_PICK_PATCH2 );
	CopyRGB(&PickerColor.rgb, &Color);
	SetWindowLong( GetDlgItem( hDlg,
			 IDC_PICK_PATCH1 ), GWL_ACTIVE, Color );
	AstralControlRepaint( hDlg,	IDC_PICK_PATCH1 );
	break;

	case IDC_PALETTEPICKER:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		iSelect = PalPicker_GetIndex(GetDlgItem(hDlg, id));
		if (PickColorMap)
		{
			PickerColor.rgb = PickColorMap->RGBData[iSelect];
			SetColorInfo(&PickerColor, &PickerColor, CS_RGB);
		}
		else
		{
			PickerColor.gray = iSelect;
			SetColorInfo(&PickerColor, &PickerColor, CS_GRAY);
		}
//		if (codeNotify)
//			ColorPicker( &PickerColor, NULL );
		CopyRGB(&PickerColor.rgb, &Color);
		SetWindowLong( GetDlgItem( hDlg,
			 IDC_PICK_PATCH1 ), GWL_ACTIVE, Color );
		AstralControlRepaint( hDlg,	IDC_PICK_PATCH1 );
		GetColorString(&PickerColor, PickerColor.ColorSpace, szColor);
		SetDlgItemText(hDlg, IDC_PALETTECOLORVALUE, szColor);
	break;

	case IDC_PICK_PATCH2:
		PickerColor = OriginalColor;
		CopyRGB(&PickerColor.rgb, &Color);
		SetWindowLong( GetDlgItem( hDlg,
			 IDC_PICK_PATCH1 ), GWL_ACTIVE, Color );
		AstralControlRepaint( hDlg,	IDC_PICK_PATCH1 );
	break;

	case IDOK:
		ClipCursor(NULL);
		AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
		ClipCursor(NULL);
		AstralDlgEnd( hDlg, FALSE );
	break;

	default:
	break;
   }
}



