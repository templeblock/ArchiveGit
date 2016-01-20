//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "gradctl.h"
#include "ppafx.h"

// Static prototypes
static void UpdatePickerControls( HWND hDlg, WPARAM wParam, int iPickerIs, LPCOLORINFO lpColorInfo, LPCOLORINFO lpOldColorInfo );
static void SetPickerValues( HWND hDlg, WPARAM wParam, int iPickerIs, LPCOLORINFO lpColorInfo );
LOCAL void picker_set_gradient_control(HWND hDlg, int idControl, int iMode);
static int  GetPickerValues( HWND hDlg, WPARAM wParam, LPCOLORINFO lpColorInfo );
static void SetPickerPoint( HWND hWindow, int x, int y );
static void GetPickerPoint( HWND hWindow, LPINT lpx, LPINT lpy );
static BOOL DrawColorPicker( HDC hDC, LPRECT lpRect, int iPickerIs, LPCOLORINFO lpColorInfo, LPRECT lpRepairRect, LPRECT lpInvalidRect );
static void DrawColorPickerCross( HWND hWindow, BOOL on );

static COLORINFO  OriginalColor;
static COLORINFO  AltOriginalColor;
static COLORINFO  AltPickerColor;
static COLORINFO  PickerColor;
static COLORSPACE PickerSpace;
static BOOL bHaveAlternate;
static BOOL bAlternateActive;
static int iPickerIs = 0;

typedef struct PICKERVALUES_tag
{
	int   Mode;
	HSLS  hsl;
	RGBS  rgb;
	CMYKS cmyk;
} PICKERVALUES;

static PICKERVALUES PickerValues;

#define GWW_PICKER_MODE   0
#define GWW_PICKER_VALUE1 2
#define GWW_PICKER_VALUE2 4
#define GWW_PICKER_VALUE3 6
#define GWW_PICKER_VALUE4 8

#define TOSLIDE(n)	((int)( ((400L * (n)) + 127) / 255 ))
#define FROMSLIDE(n) ((int)( ((255L * (n)) +  200) / 400 ))
#define HUETOSLIDE(n)	((int)( ((400L * (n)) + 125) / 251L ))
#define SLIDETOHUE(n) ((int)( ((251L * (n)) +  200) / 400 ))

/***********************************************************************/
BOOL ColorPicker( LPCOLORINFO lpColor, LPCOLORINFO lpAltColor )
/***********************************************************************/
{
	HWND hWnd, hWndFocus;

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

	// Show the Color Picker Dialog
	if ( !AstralDlg( 
		NO, PictPubApp.GetResourceHandle(), 
		hWnd, IDD_COLORPICKER, 
		DlgColorPickerProc ) )
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
LOCAL BOOL ColorPicker_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void ColorPicker_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL BOOL ColorPicker_OnQueryNewPalette(HWND hDlg);

static BOOL   Bool;
static int i, iMode;
static COLOR  Color;

/***********************************************************************/
BOOL WINPROC EXPORT DlgColorPickerProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, ColorPicker_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, ColorPicker_OnCommand);
	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, ColorPicker_OnQueryNewPalette);

	HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, Dialog_OnPaletteChanged);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_COLORPICKER);
	}
}

/***********************************************************************/
LOCAL BOOL ColorPicker_OnQueryNewPalette(HWND hDlg)
/***********************************************************************/
{
return(HandleQueryNewPalette(hDlg, YES, YES));
}

/************************************************************************/
LOCAL BOOL ColorPicker_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
RECT rArea;
LPSTR lpString;

CenterPopup( hDlg );

if (!bHaveAlternate)
	ShowControl(hDlg, IDC_ACTIVE_SELECT, IDC_ALTERNATE_SELECT, NO, NO);

GetWindowRect(hDlg, &rArea);
ScreenToClient(GetParent(hDlg), (LPPOINT)&rArea.left);
ScreenToClient(GetParent(hDlg), (LPPOINT)&rArea.right);
if (bHaveAlternate)
{
	CheckDlgButton(hDlg,
		IDC_ACTIVE_SELECT, !bAlternateActive);
	CheckDlgButton(hDlg,
		IDC_ALTERNATE_SELECT, bAlternateActive);
}

if (Control.DisplayPixelValue)
	SetDlgItemTextMultiple(hDlg, IDC_PERCENT0, IDC_PERCENT6, "");


if (iPickerIs < 0) iPickerIs = 0;
if (iPickerIs > IDC_PICK_K-IDC_PICK_H) iPickerIs = IDC_PICK_K - IDC_PICK_H;

SetPickerValues( hDlg, IDC_PICKER, iPickerIs, &PickerColor );
picker_set_gradient_control(hDlg, IDC_PICK_GRADIENT, PickerValues.Mode);

if ( AstralStr( IDS_PICKER_IS_H+iPickerIs, &lpString ) )				
	SetDlgItemText( hDlg, IDC_PICK_LABEL, lpString );

CopyRGB(&PickerColor.rgb, &Color);

if (GetDlgItem(hDlg, IDC_PICK_PATCH1))
	SetWindowLong( GetDlgItem( hDlg,
			 IDC_PICK_PATCH1 ), GWL_ACTIVE, Color );
if (GetDlgItem(hDlg, IDC_PICK_2PATCH1))
	SetWindowLong( GetDlgItem( hDlg,
			IDC_PICK_2PATCH1 ), GWL_ACTIVE, Color );
if (GetDlgItem(hDlg, IDC_PICK_PATCH2))
	SetWindowLong( GetDlgItem( hDlg,
			 IDC_PICK_PATCH2 ), GWL_ACTIVE, Color );
if (GetDlgItem(hDlg, IDC_PICK_2PATCH2))
	SetWindowLong( GetDlgItem( hDlg,
			 IDC_PICK_2PATCH2 ), GWL_ACTIVE, Color );

CheckComboItem( hDlg, 
	IDC_PICKER_IS, 
	IDC_PICKER_IS_H, 
#ifdef IDC_PICK_Y
	IDC_PICKER_IS_K,
#else
	IDC_PICKER_IS_B,
#endif
	iPickerIs+IDC_PICKER_IS_H );


switch(iPickerIs+IDC_PICK_H)
{
	case IDC_PICK_H :
		i = PickerColor.hsl.hue;
		iMode = IDC_PICK_H;
	break;

	case IDC_PICK_S :
		i = PickerColor.hsl.sat;
		iMode = IDC_PICK_S;
	break;

	case IDC_PICK_L :
		i = PickerColor.hsl.lum;
		iMode = IDC_PICK_L;
	break;

	case IDC_PICK_R :
		i = PickerColor.rgb.red;
		iMode = IDC_PICK_R;
	break;

	case IDC_PICK_G :
		i = PickerColor.rgb.green;
		iMode = IDC_PICK_G;
	break;

	case IDC_PICK_B :
		i = PickerColor.rgb.blue;
		iMode = IDC_PICK_B;
	break;

	case IDC_PICK_C :
		i = PickerColor.cmyk.c;
		iMode = IDC_PICK_C;
	break;

	case IDC_PICK_M :
		i = PickerColor.cmyk.m;
		iMode = IDC_PICK_M;
	break;

	case IDC_PICK_Y :
		i = PickerColor.cmyk.y;
		iMode = IDC_PICK_Y;
	break;

	case IDC_PICK_K :
		i = PickerColor.cmyk.k;
		iMode = IDC_PICK_K;
	break;
}

if (iMode == IDC_PICK_H)
	InitSlide( hDlg, IDC_PICK_LEVEL, HUETOSLIDE(i), 0, 400 );
else
	InitSlide( hDlg, IDC_PICK_LEVEL, TOSLIDE(i), 0, 400 );

InitDlgItemSpin( hDlg, IDC_PICK_H, TOANGLE(  PickerColor.hsl.hue),   NO, 0, 359 );
InitDlgItemSpin( hDlg, IDC_PICK_S, TOPERCENT(PickerColor.hsl.sat),   NO, 0, 100 );
InitDlgItemSpin( hDlg, IDC_PICK_L, TOPERCENT(PickerColor.hsl.lum),   NO, 0, 100 );
InitDlgItemSpin( hDlg, IDC_PICK_R, PIX2DISP(PickerColor.rgb.red),   NO, 0, MAXPIXDISP );
InitDlgItemSpin( hDlg, IDC_PICK_G, PIX2DISP(PickerColor.rgb.green), NO, 0, MAXPIXDISP );
InitDlgItemSpin( hDlg, IDC_PICK_B, PIX2DISP(PickerColor.rgb.blue),  NO, 0, MAXPIXDISP );

#ifdef IDC_PICK_Y
InitDlgItemSpin( hDlg, IDC_PICK_C, PIX2DISP(PickerColor.cmyk.c), NO, 0, MAXPIXDISP );
InitDlgItemSpin( hDlg, IDC_PICK_M, PIX2DISP(PickerColor.cmyk.m), NO, 0, MAXPIXDISP );
InitDlgItemSpin( hDlg, IDC_PICK_Y, PIX2DISP(PickerColor.cmyk.y), NO, 0, MAXPIXDISP );
InitDlgItemSpin( hDlg, IDC_PICK_K, PIX2DISP(PickerColor.cmyk.k), NO, 0, MAXPIXDISP );
#endif


return(TRUE);
}

/************************************************************************/
LOCAL void ColorPicker_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
COLORINFO OldColor, temp;

OldColor = PickerColor;
switch (id)
	{
	case IDC_ACTIVE_SELECT:
	case IDC_ALTERNATE_SELECT:
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
	UpdatePickerControls( hDlg, id, iPickerIs,
			&PickerColor, &OldColor );
	SetPickerValues( hDlg, IDC_PICKER, iPickerIs,  &PickerColor );
	CopyRGB(&OriginalColor.rgb, &Color);
	if (GetDlgItem(hDlg, IDC_PICK_PATCH2))
		{
		SetWindowLong( GetDlgItem( hDlg,
			IDC_PICK_PATCH2 ), GWL_ACTIVE, Color );
		AstralControlRepaint( hDlg,
			IDC_PICK_PATCH2 );
		}
	if (GetDlgItem(hDlg, IDC_PICK_2PATCH2))
		{
		SetWindowLong( GetDlgItem( hDlg,
			IDC_PICK_2PATCH2 ), GWL_ACTIVE, Color );
		AstralControlRepaint( hDlg,
			IDC_PICK_2PATCH2 );
		}
	break;

	case IDC_PICK_LEVEL:
	if ( GetFocus() != GetDlgItem( hDlg, id ) )
		break;
	i = HandleSlide( hDlg, id, codeNotify, NULL );
	SetSlide( hDlg, id, i );
	if ((iPickerIs+IDC_PICK_H) == IDC_PICK_H)
		i = SLIDETOHUE(i);
	else
		i = FROMSLIDE(i);
	switch(iPickerIs + IDC_PICK_H)
	{
	case IDC_PICK_H :
		PickerColor.hsl.hue = i;
		SetColorInfo( &PickerColor, &PickerColor, CS_HSL );
	break;

	case IDC_PICK_S :
		PickerColor.hsl.sat = i;
		SetColorInfo( &PickerColor, &PickerColor, CS_HSL );
	break;

	case IDC_PICK_L :
		PickerColor.hsl.lum = i;
		SetColorInfo( &PickerColor, &PickerColor, CS_HSL );
	break;

	case IDC_PICK_R :
		PickerColor.rgb.red   = i;
		SetColorInfo( &PickerColor, &PickerColor, CS_RGB );
	break;

	case IDC_PICK_G :
		PickerColor.rgb.green = i;
		SetColorInfo( &PickerColor, &PickerColor, CS_RGB );
	break;

	case IDC_PICK_B :
		PickerColor.rgb.blue  = i;
		SetColorInfo( &PickerColor, &PickerColor, CS_RGB );
	break;

	case IDC_PICK_C :
		PickerColor.cmyk.c = i;
		SetColorInfo( &PickerColor, &PickerColor, CS_CMYK );
	break;

	case IDC_PICK_M :
		PickerColor.cmyk.m = i;
		SetColorInfo( &PickerColor, &PickerColor, CS_CMYK );
	break;

	case IDC_PICK_Y :
		PickerColor.cmyk.y = i;
		SetColorInfo( &PickerColor, &PickerColor, CS_CMYK );
	break;

	case IDC_PICK_K :
		PickerColor.cmyk.k = i;
		SetColorInfo( &PickerColor, &PickerColor, CS_CMYK );
	break;
	}

	UpdatePickerControls( hDlg, IDC_PICK_LEVEL, iPickerIs,
		&PickerColor, &OldColor );

//	if ( codeNotify != SN_VALUECHANGING )
		SetPickerValues( hDlg, IDC_PICKER, iPickerIs,
					&PickerColor );
	break;

	case IDC_PICKER:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;

		GetPickerValues( hDlg, IDC_PICKER, &PickerColor );
		UpdatePickerControls( hDlg, id, iPickerIs,
			&PickerColor, &OldColor );
		SetPickerValues( hDlg, IDC_PICKER, iPickerIs,  &PickerColor );
	break;

	case IDC_PICK_PATCH2:
	case IDC_PICK_2PATCH2:
		PickerColor = OriginalColor;
		UpdatePickerControls( hDlg, id, iPickerIs, &PickerColor, &OldColor );
		SetPickerValues( hDlg, IDC_PICKER, iPickerIs,  &PickerColor );
	break;

	case IDC_PICKER_IS:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		iPickerIs = id - IDC_PICKER_IS_H;

		UpdatePickerControls( hDlg, id, iPickerIs,
			&PickerColor, &OldColor );
		SetPickerValues( hDlg, IDC_PICKER, iPickerIs, &PickerColor );
	break;

	case IDC_PICK_H:
	case IDC_PICK_S:
	case IDC_PICK_L:
	case IDC_PICK_R:
	case IDC_PICK_G:
	case IDC_PICK_B:

#ifdef IDC_PICK_C
	case IDC_PICK_C:
	case IDC_PICK_M:
	case IDC_PICK_Y:
	case IDC_PICK_K:
#endif
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		if ( codeNotify != EN_CHANGE )
			break;

		i = GetDlgItemSpin( hDlg, id, &Bool, NO );

		switch(id)
		{
			case IDC_PICK_H :
				PickerColor.hsl.hue = FROMANGLE(i);
				SetColorInfo( &PickerColor, &PickerColor, CS_HSL );
			break;

			case IDC_PICK_S :
				PickerColor.hsl.sat = TOGRAY(i);
				SetColorInfo( &PickerColor, &PickerColor, CS_HSL );
			break;

			case IDC_PICK_L :
				PickerColor.hsl.lum = TOGRAY(i);
				SetColorInfo( &PickerColor, &PickerColor, CS_HSL );
			break;

			case IDC_PICK_R :
				PickerColor.rgb.red   = DISP2PIX(i);
				SetColorInfo( &PickerColor, &PickerColor, CS_RGB );
			break;

			case IDC_PICK_G :
				PickerColor.rgb.green = DISP2PIX(i);
				SetColorInfo( &PickerColor, &PickerColor, CS_RGB );
			break;

			case IDC_PICK_B :
				PickerColor.rgb.blue  = DISP2PIX(i);
				SetColorInfo( &PickerColor, &PickerColor, CS_RGB );
			break;

			case IDC_PICK_C :
				PickerColor.cmyk.c = DISP2PIX(i);
				SetColorInfo( &PickerColor, &PickerColor, CS_CMYK );
			break;

			case IDC_PICK_M :
				PickerColor.cmyk.m = DISP2PIX(i);
				SetColorInfo( &PickerColor, &PickerColor, CS_CMYK );
			break;

			case IDC_PICK_Y :
				PickerColor.cmyk.y = DISP2PIX(i);
				SetColorInfo( &PickerColor, &PickerColor, CS_CMYK );
			break;

			case IDC_PICK_K :
				PickerColor.cmyk.k = DISP2PIX(i);
				SetColorInfo( &PickerColor, &PickerColor, CS_CMYK );
			break;
		}

		UpdatePickerControls( hDlg, id, iPickerIs,
			&PickerColor, &OldColor );

		if ( !LBUTTON )
			SetPickerValues( hDlg, IDC_PICKER, iPickerIs, &PickerColor );
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

/***********************************************************************/
static void UpdatePickerControls( HWND hDlg, WPARAM wParam, int iPickerIs,
                                    LPCOLORINFO lpColorInfo,
                                    LPCOLORINFO lpOldColorInfo )
/***********************************************************************/
{
	long Color;

	if ((lpColorInfo->hsl.hue != lpOldColorInfo->hsl.hue) &&
		(wParam != IDC_PICK_H))
		SetDlgItemSpin( hDlg, IDC_PICK_H, TOANGLE(  lpColorInfo->hsl.hue),   NO );

	if ((lpColorInfo->hsl.sat != lpOldColorInfo->hsl.sat) &&
		(wParam != IDC_PICK_S))
		SetDlgItemSpin( hDlg, IDC_PICK_S, TOPERCENT(lpColorInfo->hsl.sat),   NO );

	if ((lpColorInfo->hsl.lum != lpOldColorInfo->hsl.lum) &&
		(wParam != IDC_PICK_L))
		SetDlgItemSpin( hDlg, IDC_PICK_L, TOPERCENT(lpColorInfo->hsl.lum),   NO );

	if ((lpColorInfo->rgb.red != lpOldColorInfo->rgb.red) &&
		(wParam != IDC_PICK_R))
		SetDlgItemSpin( hDlg, IDC_PICK_R, PIX2DISP(lpColorInfo->rgb.red),   NO );

	if ((lpColorInfo->rgb.green != lpOldColorInfo->rgb.green) &&
		(wParam != IDC_PICK_G))
		SetDlgItemSpin( hDlg, IDC_PICK_G, PIX2DISP(lpColorInfo->rgb.green), NO );

	if ((lpColorInfo->rgb.blue != lpOldColorInfo->rgb.blue) &&
		(wParam != IDC_PICK_B))
		SetDlgItemSpin( hDlg, IDC_PICK_B, PIX2DISP(lpColorInfo->rgb.blue),  NO );

#ifdef IDC_PICK_Y
	if ((lpColorInfo->cmyk.c != lpOldColorInfo->cmyk.c) &&
		(wParam != IDC_PICK_C))
		SetDlgItemSpin( hDlg, IDC_PICK_C, PIX2DISP(lpColorInfo->cmyk.c), NO );

	if ((lpColorInfo->cmyk.m != lpOldColorInfo->cmyk.m) &&
		(wParam != IDC_PICK_M))
		SetDlgItemSpin( hDlg, IDC_PICK_M, PIX2DISP(lpColorInfo->cmyk.m), NO );

	if ((lpColorInfo->cmyk.y != lpOldColorInfo->cmyk.y) &&
		(wParam != IDC_PICK_Y))
		SetDlgItemSpin( hDlg, IDC_PICK_Y, PIX2DISP(lpColorInfo->cmyk.y), NO );

	if ((lpColorInfo->cmyk.k != lpOldColorInfo->cmyk.k) &&
		(wParam != IDC_PICK_K))
		SetDlgItemSpin( hDlg, IDC_PICK_K, PIX2DISP(lpColorInfo->cmyk.k), NO );
#endif

	CopyRGB(&lpColorInfo->rgb, &Color);
	if (GetDlgItem(hDlg, IDC_PICK_PATCH1))
		{
		SetWindowLong( GetDlgItem( hDlg,
					IDC_PICK_PATCH1 ), GWL_ACTIVE, Color );
		AstralControlRepaint( hDlg, IDC_PICK_PATCH1 );
		}
	if (GetDlgItem(hDlg, IDC_PICK_2PATCH1))
		{
		SetWindowLong( GetDlgItem( hDlg,
					 IDC_PICK_2PATCH1 ), GWL_ACTIVE, Color );
		AstralControlRepaint( hDlg, IDC_PICK_2PATCH1 );
		}
}

/***********************************************************************/
static void SetPickerValues( HWND hDlg, WPARAM wParam, int iPickerIs,
                              LPCOLORINFO lpColorInfo )
/***********************************************************************/
{
	HWND hWnd;
	COLORINFO OldColor;

	int  iMode, iOldMode;
	LPSTR lpString;
	BOOL bModeChange, bValueChange, bCrossChange;

	GetPickerValues( hDlg, wParam, &OldColor );

	hWnd     = GetDlgItem( hDlg, wParam );
	iOldMode = PickerValues.Mode;

	PickerColor = *lpColorInfo;

	if ( iPickerIs+IDC_PICK_H >= IDC_PICK_H && iPickerIs+IDC_PICK_H <= IDC_PICK_K)
		iMode = iPickerIs;
	else
		iMode = iOldMode;

	if ( (bModeChange = (iMode != iOldMode)) )
		PickerValues.Mode = iMode;

	switch ( iMode+IDC_PICK_H )
	{
		case IDC_PICK_H :
			bValueChange = ( lpColorInfo->hsl.hue != OldColor.hsl.hue );
			bCrossChange = ( lpColorInfo->hsl.sat != OldColor.hsl.sat || 
				lpColorInfo->hsl.lum != OldColor.hsl.lum );
			PickerValues.hsl = lpColorInfo->hsl;
			if ( bValueChange || bModeChange )
				SetSlide( hDlg, IDC_PICK_LEVEL, HUETOSLIDE(lpColorInfo->hsl.hue) );
		break;

		case IDC_PICK_S :
			bValueChange = ( lpColorInfo->hsl.sat != OldColor.hsl.sat );
			bCrossChange = ( lpColorInfo->hsl.hue != OldColor.hsl.hue || 
				lpColorInfo->hsl.lum != OldColor.hsl.lum );
			PickerValues.hsl = lpColorInfo->hsl;
			if ( bValueChange || bModeChange )
				SetSlide( hDlg, IDC_PICK_LEVEL, TOSLIDE(lpColorInfo->hsl.sat ));
		break;

		case IDC_PICK_L :
			bValueChange = ( lpColorInfo->hsl.lum != OldColor.hsl.lum );
			bCrossChange = ( lpColorInfo->hsl.hue != OldColor.hsl.hue || 
				lpColorInfo->hsl.sat != OldColor.hsl.sat );
			PickerValues.hsl = lpColorInfo->hsl;
			if ( bValueChange || bModeChange )
				SetSlide( hDlg, IDC_PICK_LEVEL, TOSLIDE(lpColorInfo->hsl.lum) );
		break;

		case IDC_PICK_R :
			bValueChange = ( lpColorInfo->rgb.red != OldColor.rgb.red );
			bCrossChange = ( lpColorInfo->rgb.green != OldColor.rgb.green || 
				lpColorInfo->rgb.blue != OldColor.rgb.blue );
			PickerValues.rgb = lpColorInfo->rgb;
			if ( bValueChange || bModeChange )
				SetSlide( hDlg, IDC_PICK_LEVEL, TOSLIDE(lpColorInfo->rgb.red) );
		break;

		case IDC_PICK_G :
			bValueChange = ( lpColorInfo->rgb.green != OldColor.rgb.green );
			bCrossChange = ( lpColorInfo->rgb.red != OldColor.rgb.red || 
				lpColorInfo->rgb.blue != OldColor.rgb.blue );
			PickerValues.rgb = lpColorInfo->rgb;
			if ( bValueChange || bModeChange )
				SetSlide( hDlg, IDC_PICK_LEVEL, TOSLIDE(lpColorInfo->rgb.green) );
		break;

		case IDC_PICK_B :
			bValueChange = ( lpColorInfo->rgb.blue != OldColor.rgb.blue );
			bCrossChange = ( lpColorInfo->rgb.red != OldColor.rgb.red ||
				lpColorInfo->rgb.green != OldColor.rgb.green );
			PickerValues.rgb = lpColorInfo->rgb;
			if ( bValueChange || bModeChange )
				SetSlide( hDlg, IDC_PICK_LEVEL, TOSLIDE(lpColorInfo->rgb.blue) );
		break;

#ifdef IDC_PICK_Y
		case IDC_PICK_C :
			bValueChange = ( lpColorInfo->cmyk.c != OldColor.cmyk.c );
			bCrossChange = (
				lpColorInfo->cmyk.m != OldColor.cmyk.m ||
				lpColorInfo->cmyk.y != OldColor.cmyk.y ||
				lpColorInfo->cmyk.k != OldColor.cmyk.k );
			PickerValues.cmyk = lpColorInfo->cmyk;
			if ( bValueChange || bModeChange )
				SetSlide( hDlg, IDC_PICK_LEVEL, TOSLIDE(lpColorInfo->cmyk.c) );
		break;

		case IDC_PICK_M :
			bValueChange = ( lpColorInfo->cmyk.m != OldColor.cmyk.m );
			bCrossChange = (
				lpColorInfo->cmyk.c != OldColor.cmyk.c ||
				lpColorInfo->cmyk.y != OldColor.cmyk.y ||
				lpColorInfo->cmyk.k != OldColor.cmyk.k );
			PickerValues.cmyk = lpColorInfo->cmyk;
			if ( bValueChange || bModeChange )
				SetSlide( hDlg, IDC_PICK_LEVEL, TOSLIDE(lpColorInfo->cmyk.m) );
		break;

		case IDC_PICK_Y :
			bValueChange = ( lpColorInfo->cmyk.y != OldColor.cmyk.y );
			bCrossChange = (
				lpColorInfo->cmyk.c != OldColor.cmyk.c ||
				lpColorInfo->cmyk.m != OldColor.cmyk.m ||
				lpColorInfo->cmyk.k != OldColor.cmyk.k );
			PickerValues.cmyk = lpColorInfo->cmyk;
			if ( bValueChange || bModeChange )
				SetSlide( hDlg, IDC_PICK_LEVEL, TOSLIDE(lpColorInfo->cmyk.y) );
		break;

		case IDC_PICK_K :
			bValueChange = ( lpColorInfo->cmyk.k != OldColor.cmyk.k );
			bCrossChange = (
				lpColorInfo->cmyk.c != OldColor.cmyk.c || 
				lpColorInfo->cmyk.m != OldColor.cmyk.m || 
				lpColorInfo->cmyk.y != OldColor.cmyk.y );
			PickerValues.cmyk = lpColorInfo->cmyk;
			if ( bValueChange || bModeChange )
				SetSlide( hDlg, IDC_PICK_LEVEL, TOSLIDE(lpColorInfo->cmyk.k) );
		break;
#endif // IDC_PICK_Y
	}

	if ( bModeChange || bValueChange )
	{
		AstralControlRepaint( hDlg, wParam );

		if ( AstralStr( IDS_PICKER_IS_H+iPickerIs, &lpString ) )
			SetDlgItemText( hDlg, IDC_PICK_LABEL, lpString );

		PickerValues.Mode = iMode;
		
		// Update picher controls
		picker_set_gradient_control(hDlg, IDC_PICK_GRADIENT, PickerValues.Mode);
		
		AstralControlRepaint( hDlg, IDC_PICK_GRADIENT );
	}
	else if ( bCrossChange )
	{
		DrawColorPickerCross( hWnd, OFF );
		DrawColorPickerCross( hWnd, ON  );
	}
}

/***********************************************************************/
//	Sets up the gradient control based on given mode 
/***********************************************************************/
LOCAL void picker_set_gradient_control(HWND hDlg, int idControl, int iMode)
/***********************************************************************/
{
	
	HWND hGrad;
	RGBS rgb1, rgb2;
	HSLS hsl1, hsl2;
	
	hGrad = GetDlgItem(hDlg, idControl);
	
	switch(iMode+IDC_PICK_H )
	{
		case IDC_PICK_H :
			hsl1.hue = 0;
			hsl2.hue = MAX_HUES-1;
			hsl1.sat = hsl2.sat = 255;
			hsl1.lum = hsl2.lum = 127;
			Gradient_SetHSL(hGrad, &hsl1, &hsl2);
			Gradient_SetType(hGrad, GT_HSL);																	  
		break;

		case IDC_PICK_S :
			hsl1.hue = hsl2.hue = 0;
			hsl1.sat = 0;
			hsl2.sat = 255;
			hsl1.lum = hsl2.lum = 127;
			Gradient_SetHSL(hGrad, &hsl1, &hsl2);
			Gradient_SetType(hGrad, GT_HSL);																	  
		break;

		case IDC_PICK_L :
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = rgb2.green = rgb2.blue = 255;
			Gradient_SetRGB(hGrad, &rgb1, &rgb2);
			Gradient_SetType(hGrad, GT_RGB);																	  
		break;

		case IDC_PICK_R :
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.red = 255;
			rgb2.green = rgb2.blue = 0;
			Gradient_SetRGB(hGrad, &rgb1, &rgb2);
			Gradient_SetType(hGrad, GT_RGB);																	  
		break;

		case IDC_PICK_G :
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.green = 255;
			rgb2.red = rgb2.blue = 0;
			Gradient_SetRGB(hGrad, &rgb1, &rgb2);
			Gradient_SetType(hGrad, GT_RGB);																	  
		break;

		case IDC_PICK_B :
			rgb1.red = rgb1.green = rgb1.blue = 0;
			rgb2.blue = 255;
			rgb2.red = rgb2.green = 0;
			Gradient_SetRGB(hGrad, &rgb1, &rgb2);
			Gradient_SetType(hGrad, GT_RGB);																	  
		break;

		case IDC_PICK_C :
			rgb1.red = rgb1.green = rgb1.blue = 255;
			rgb2.red = 0;
			rgb2.green = rgb2.blue = 255;
			Gradient_SetRGB(hGrad, &rgb1, &rgb2);
			Gradient_SetType(hGrad, GT_RGB);																	  
		break;

		case IDC_PICK_M :
			rgb1.red = rgb1.green = rgb1.blue = 255;
			rgb2.green = 0;
			rgb2.red = rgb2.blue = 255;
			Gradient_SetRGB(hGrad, &rgb1, &rgb2);
			Gradient_SetType(hGrad, GT_RGB);																	  
		break;

		case IDC_PICK_Y :
			rgb1.red = rgb1.green = rgb1.blue = 255;
			rgb2.blue = 0;
			rgb2.red = rgb2.green = 255;
			Gradient_SetRGB(hGrad, &rgb1, &rgb2);
			Gradient_SetType(hGrad, GT_RGB);																	  
		break;

		case IDC_PICK_K :
			rgb1.red = rgb1.green = rgb1.blue = 255;
			rgb2.red = rgb2.green = rgb2.blue = 0;
			Gradient_SetRGB(hGrad, &rgb1, &rgb2);
			Gradient_SetType(hGrad, GT_RGB);																	  
		break;
	}
}


/***********************************************************************/
static int GetPickerValues( HWND hDlg, WPARAM wParam, LPCOLORINFO lpColorInfo )
/***********************************************************************/
{
	switch( PickerValues.Mode+IDC_PICK_H )
	{
		case IDC_PICK_H :
		case IDC_PICK_S :
		case IDC_PICK_L :
			lpColorInfo->hsl = PickerValues.hsl;
			SetColorInfo( lpColorInfo, lpColorInfo, CS_HSL );
		break;

		case IDC_PICK_R :
		case IDC_PICK_G :
		case IDC_PICK_B :
			lpColorInfo->rgb = PickerValues.rgb;
			SetColorInfo( lpColorInfo, lpColorInfo, CS_RGB );
		break;

		case IDC_PICK_C :
		case IDC_PICK_M :
		case IDC_PICK_Y :
		case IDC_PICK_K :
			lpColorInfo->cmyk = PickerValues.cmyk;
			SetColorInfo( lpColorInfo, lpColorInfo, CS_CMYK );
		break;
	}

	return( PickerValues.Mode );
}

/***********************************************************************/
/***********************************************************************/
// PICKER CONTROL
/***********************************************************************/
/***********************************************************************/

LOCAL void Picker_OnDestroy(HWND hWindow);
LOCAL void Picker_OnPaint(HWND hWindow);
LOCAL void Picker_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
LOCAL void Picker_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Picker_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
LOCAL void Picker_OnSetFocus(HWND hWindow, HWND hwndOldFocus);
LOCAL void Picker_OnKillFocus(HWND hWindow, HWND hwndNewFocus);
LOCAL void Picker_HandleFocus(HWND hWindow, BOOL fSetFocus);
LOCAL void Picker_Draw( HWND hWindow );

static BOOL Picker_bTrack;

/***********************************************************************/
LONG WINPROC EXPORT Picker_Control(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
switch ( message )
	{
	HANDLE_MSG(hWindow, WM_GETDLGCODE, Control_OnGetDlgCode);
	HANDLE_MSG(hWindow, WM_PALETTECHANGED, Control_OnPaletteChanged);
	HANDLE_MSG(hWindow, WM_ERASEBKGND, Control_OnEraseBkgnd);

	HANDLE_MSG(hWindow, WM_PAINT, Picker_OnPaint);
	HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Picker_OnLButtonDown);
	HANDLE_MSG(hWindow, WM_LBUTTONUP, Picker_OnLButtonUp);
	HANDLE_MSG(hWindow, WM_MOUSEMOVE, Picker_OnMouseMove);
	HANDLE_MSG(hWindow, WM_SETFOCUS, Picker_OnSetFocus);
	HANDLE_MSG(hWindow, WM_KILLFOCUS, Picker_OnKillFocus);
	HANDLE_MSG(hWindow, WM_DESTROY, Picker_OnDestroy);

	default:
	return Control_DefProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL void Picker_OnDestroy(HWND hWindow)
/***********************************************************************/
{
if ( !Picker_bTrack )
	return;
ReleaseCapture();
Picker_bTrack = NO;
}

/***********************************************************************/
LOCAL void Picker_OnPaint(HWND hWindow)
/***********************************************************************/
{
Picker_Draw( hWindow );
}

/***********************************************************************/
LOCAL void Picker_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
RECT ClientRect;
int px, py, pdx, pdy;
long lValue;

if ( Picker_bTrack )
	return;

SetCapture( hWindow ); Picker_bTrack = TRUE;

if ( GetFocus() != hWindow )
	SetFocus( hWindow );

GetWindowRect( hWindow, &ClientRect );
ClipCursor( &ClientRect );
DrawColorPickerCross( hWindow, OFF );
GetClientRect( hWindow, &ClientRect );
InflateRect(&ClientRect, -1, -1);
ClientRect.bottom -= 1;
ClientRect.right -= 1;
py = y - ClientRect.top;
pdy = ClientRect.bottom - ClientRect.top;
py = FMUL( 255, FGET( py, pdy ) );
px = x - ClientRect.left;
pdx = ClientRect.right - ClientRect.left;
px = FMUL( 255, FGET( px, pdx ) );
py = bound(py, 0, 255);
px = bound(px, 0, 255);
SetPickerPoint( hWindow, px, py );
lValue = MAKELONG( px, py );
FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow),
				hWindow, lValue, SendMessage );
}

/***********************************************************************/
LOCAL void Picker_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
if ( !Picker_bTrack )
	return;
ReleaseCapture(); Picker_bTrack = FALSE;
ClipCursor( NULL );
DrawColorPickerCross( hWindow, ON );
}

/***********************************************************************/
LOCAL void Picker_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
RECT ClientRect;
int px, py, pdx, pdy;
long lValue;

if ( !Picker_bTrack )
	return;
GetClientRect( hWindow, &ClientRect );
InflateRect(&ClientRect, -1, -1);
ClientRect.bottom -= 1;
ClientRect.right  -= 1;
py = y - ClientRect.top;
pdy = ClientRect.bottom - ClientRect.top;
py = FMUL( 255, FGET( py, pdy ) );
px = x - ClientRect.left;
pdx = ClientRect.right - ClientRect.left;
px = FMUL( 255, FGET( px, pdx ) );
py = bound(py, 0, 255);
px = bound(px, 0, 255);
SetPickerPoint( hWindow, px, py );
lValue = MAKELONG( px, py );
FORWARD_WM_COMMAND( GetParent(hWindow), GetDlgCtrlID(hWindow),
				hWindow, lValue, SendMessage );
}

/***********************************************************************/
LOCAL void Picker_OnSetFocus(HWND hWindow, HWND hwndOldFocus)
/***********************************************************************/
{
Picker_HandleFocus(hWindow, TRUE);
}

/***********************************************************************/
LOCAL void Picker_OnKillFocus(HWND hWindow, HWND hwndNewFocus)
/***********************************************************************/
{
Picker_HandleFocus(hWindow, FALSE);
}

/***********************************************************************/
LOCAL void Picker_HandleFocus(HWND hWindow, BOOL fSetFocus)
/***********************************************************************/
{
HDC hDC;
RECT ClientRect;

if ( !(GetWindowLong( hWindow, GWL_STYLE ) & WS_TABSTOP) )
	return;
// Draw the focus rectangle
hDC = GetDC( hWindow );
GetClientRect( hWindow, &ClientRect );
FrameRect( hDC, &ClientRect,
		(HBRUSH)GetStockObject( fSetFocus ? BLACK_BRUSH : WHITE_BRUSH ) );
ReleaseDC( hWindow, hDC );
}

/***********************************************************************/
LOCAL  void Picker_Draw( HWND hWindow )
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;
	//BOOL bHasFocus;
	RECT ClientRect, rInvalid;
	int iPickerIs;
	COLORINFO ColorInfo;

	hDC = BeginPaint( hWindow, &ps );
	GetClientRect( hWindow, &ClientRect );

	// Draw the box frame
	FrameRect( hDC, &ClientRect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
	InflateRect( &ClientRect, -1, -1 );

	// Draw the color picker
	iPickerIs = GetPickerValues( GetParent(hWindow),
		GetDlgCtrlID(hWindow), &ColorInfo );
		
	if (!DrawColorPicker( hDC, &ClientRect, iPickerIs, &ColorInfo, &ps.rcPaint, &rInvalid ))
	{
		// We aren't done yet...
		EndPaint( hWindow, &ps );

		// Invalidate the remaining rectangle
		rInvalid.right++;	// make into a Windows rectangle
		rInvalid.bottom++;
		InvalidateRect(hWindow, &rInvalid, NO);
	}
	else
	{
		EndPaint( hWindow, &ps );
		DrawColorPickerCross( hWindow, ON );
	}
}

/***********************************************************************/
static void SetPickerPoint( HWND hWindow, int x, int y )
/***********************************************************************/
{
	switch( PickerValues.Mode+IDC_PICK_H )
	{
		case IDC_PICK_H :
			PickerValues.hsl.sat = x;
			PickerValues.hsl.lum = y;
		break;

		case IDC_PICK_S :
			PickerValues.hsl.hue = (x*251L)/255L;
			PickerValues.hsl.lum = y;
		break;

		case IDC_PICK_L :
			PickerValues.hsl.hue = (x*251L)/255L;
			PickerValues.hsl.sat = y;
		break;

		case IDC_PICK_R :
			PickerValues.rgb.green = x;
			PickerValues.rgb.blue  = y;
		break;

		case IDC_PICK_G :
			PickerValues.rgb.red   = x;
			PickerValues.rgb.blue  = y;
		break;

		case IDC_PICK_B :
			PickerValues.rgb.red   = x;
			PickerValues.rgb.green = y;
		break;

		case IDC_PICK_C :
			PickerValues.cmyk.m = x;
			PickerValues.cmyk.y = y;
		break;

		case IDC_PICK_M :
			PickerValues.cmyk.c = x;
			PickerValues.cmyk.y = y;
		break;

		case IDC_PICK_Y :
		case IDC_PICK_K :
			PickerValues.cmyk.c = x;
			PickerValues.cmyk.m = y;
		break;
	}
}

/***********************************************************************/
static void GetPickerPoint( HWND hWindow, LPINT lpx, LPINT lpy )
/***********************************************************************/
{
	switch( PickerValues.Mode+IDC_PICK_H )
	{
		case IDC_PICK_H :
			*lpx = PickerValues.hsl.sat;
			*lpy = PickerValues.hsl.lum;
		break;

		case IDC_PICK_S :
			*lpx = (PickerValues.hsl.hue * 255L) / 251L;
			*lpy = PickerValues.hsl.lum;
		break;

		case IDC_PICK_L :
			*lpx = (PickerValues.hsl.hue * 255L) / 251L;
			*lpy = PickerValues.hsl.sat;
		break;

		case IDC_PICK_R :
			*lpx = PickerValues.rgb.green;
			*lpy = PickerValues.rgb.blue;
		break;

		case IDC_PICK_G :
			*lpx = PickerValues.rgb.red;
			*lpy = PickerValues.rgb.blue;
		break;

		case IDC_PICK_B :
			*lpx = PickerValues.rgb.red;
			*lpy = PickerValues.rgb.green;
		break;

		case IDC_PICK_C :
			*lpx = PickerValues.cmyk.m;
			*lpy = PickerValues.cmyk.y;
		break;

		case IDC_PICK_M :
			*lpx = PickerValues.cmyk.c;
			*lpy = PickerValues.cmyk.y;
		break;

		case IDC_PICK_Y :
		case IDC_PICK_K :
			*lpx = PickerValues.cmyk.c;
			*lpy = PickerValues.cmyk.m;
		break;
	}
}

/***********************************************************************/
static BOOL DrawColorPicker( HDC hDC, LPRECT lpRect, int iPickerIs,
                              LPCOLORINFO lpColorInfo, LPRECT lpRepairRect,
                              LPRECT lpInvalidRect )
/***********************************************************************/
{
	int iMode, x, y, dx, dy;
	int cx, cy, v;
	LPRGB lpRGBline, lpRGBWork;
	HSLS  hsl;
	RGBS  rgb;
	CMYKS cmyk;
	RECT  rRepair;
	LFIXED xScale, yScale;
	LFIXED xOffset, yOffset;
	BLTSESSION BltSession;
	FRMTYPEINFO TypeInfo;

#ifdef ADD_BLACK
	int k = lpColorInfo->cmyk.k;
#endif // ADD_BLACK

	hsl  = lpColorInfo->hsl;
	rgb  = lpColorInfo->rgb;
	cmyk = lpColorInfo->cmyk;

	iMode = iPickerIs;

	if (lpRepairRect)
		rRepair = *lpRepairRect;
	else
		rRepair = *lpRect;

	lpRect->right--;
	lpRect->bottom--;

	if (!AstralIntersectRect(&rRepair, lpRect, &rRepair))
		return(TRUE);

	if ( !(lpRGBline = (LPRGB)Alloc((long)RectWidth(&rRepair)*3L*2L)) )
		return(TRUE);

	dx = RectWidth( lpRect );
	dy = RectHeight( lpRect );

	FrameSetTypeInfo(&TypeInfo, FDT_RGBCOLOR, NULL);
	StartSuperBlt(
		&BltSession, hDC, NULL, 
		lpBltScreen, &rRepair, TypeInfo, 10, 0, 0, YES, NULL, NULL );

	lpRect->right++;
	lpRect->bottom++;

	xScale = FGET( 255, dx );
	yScale = FGET( 255, dy );

	yOffset = (yScale * rRepair.top);

	for (y=0; y<=(rRepair.bottom-rRepair.top); y++ )
	{
		lpRGBWork = lpRGBline;
		cy = WHOLE(yOffset);
		yOffset += yScale;
		xOffset = (xScale * rRepair.left);

		switch ( iMode+IDC_PICK_H )
		{
			case IDC_PICK_H :
				v = hsl.hue;

				for ( x=0; x<= (rRepair.right-rRepair.left)+1; x+=2, lpRGBWork+=2 )
				{
					cx = WHOLE(xOffset);
					xOffset += xScale;
					xOffset += xScale;

					HSLtoRGB( (BYTE)v, (BYTE)cx, (BYTE)cy, lpRGBWork );
					lpRGBWork[1] = lpRGBWork[0];
				}
			break;

			case IDC_PICK_S :
				v = hsl.sat;

				for ( x=0; x<= (rRepair.right-rRepair.left)+1; x+=2, lpRGBWork+=2 )
				{
					cx = WHOLE(xOffset);
					xOffset += xScale;
					xOffset += xScale;

					HSLtoRGB( (BYTE)((cx * 251L)/255L), (BYTE)v, (BYTE)cy, lpRGBWork );
					lpRGBWork[1] = lpRGBWork[0];
				}
			break;

			case IDC_PICK_L :
				v = hsl.lum;

				for ( x=0; x<= (rRepair.right-rRepair.left)+1; x+=2, lpRGBWork+=2 )
				{
					cx = WHOLE(xOffset);
					xOffset += xScale;
					xOffset += xScale;

					HSLtoRGB( (BYTE)((cx * 251L)/255L), (BYTE)cy, (BYTE)v, lpRGBWork );
					lpRGBWork[1] = lpRGBWork[0];
				}
			break;

			case IDC_PICK_R :
				v = rgb.red;

				for ( x=0; x<= (rRepair.right-rRepair.left); x++, lpRGBWork++ )
				{
					cx = WHOLE(xOffset);
					xOffset += xScale;

					lpRGBWork->red   = v;
					lpRGBWork->green = cx;
					lpRGBWork->blue  = cy;
				}
			break;

			case IDC_PICK_G :
				v = rgb.green;

				for ( x=0; x<= (rRepair.right-rRepair.left); x++, lpRGBWork++ )
				{
					cx = WHOLE(xOffset);
					xOffset += xScale;

					lpRGBWork->red   = cx;
					lpRGBWork->green = v;
					lpRGBWork->blue  = cy;
				}
			break;

			case IDC_PICK_B :
				v = rgb.blue;

				for ( x=0; x<= (rRepair.right-rRepair.left); x++, lpRGBWork++ )
				{
					cx = WHOLE(xOffset);
					xOffset += xScale;

					lpRGBWork->red   = cx;
					lpRGBWork->green = cy;
					lpRGBWork->blue  = v;
				}
			break;

			case IDC_PICK_C :
				cy = 255-cy;
				v  = 255-cmyk.c;

				for ( x=0; x<= (rRepair.right-rRepair.left); x++, lpRGBWork++ )
				{
					cx = 255 - WHOLE(xOffset);
					xOffset += xScale;

#ifdef ADD_BLACK
					if (k)
					{
						lpRGBWork->red   = v  - ((v  * k) >> 8);
						lpRGBWork->green = cx - ((cx * k) >> 8);
						lpRGBWork->blue  = cy - ((cy * k) >> 8);
					}
					else
#endif // ADD_BLACK

					{
						lpRGBWork->red   = v;
						lpRGBWork->green = cx;
						lpRGBWork->blue  = cy;
					}
				}
			break;

			case IDC_PICK_M :
				cy = 255-cy;
				v  = 255-cmyk.m;

				for ( x=0; x<= (rRepair.right-rRepair.left); x++, lpRGBWork++ )
				{
					cx = 255 - WHOLE(xOffset);
					xOffset += xScale;

#ifdef ADD_BLACK
					if (k)
					{
						lpRGBWork->red   = cx - ((cx * k) >> 8);
						lpRGBWork->green = v  - ((v  * k) >> 8);
						lpRGBWork->blue  = cy - ((cy * k) >> 8);
					}
					else
#endif // ADD_BLACK

					{
						lpRGBWork->red   = cx;
						lpRGBWork->green = v;
						lpRGBWork->blue  = cy;
					}
				}
			break;

			case IDC_PICK_Y :
			case IDC_PICK_K :
				cy = 255-cy;
				v  = 255-cmyk.y;

				for ( x=0; x<= (rRepair.right-rRepair.left); x++, lpRGBWork++ )
				{
					cx = 255 - WHOLE(xOffset);
					xOffset += xScale;

#ifdef ADD_BLACK
					if (k)
					{
						lpRGBWork->red   = cx - ((cx * k) >> 8);
						lpRGBWork->green = cy - ((cy * k) >> 8);
						lpRGBWork->blue  = v  - ((v  * k) >> 8);
					}
					else
#endif // ADD_BLACK

					{
						lpRGBWork->red   = cx;
						lpRGBWork->green = cy;
						lpRGBWork->blue  = v;
					}
				}
			break;

		}

//		if (lpInvalidRect && AbortUpdate())
//			break;

		SuperBlt( &BltSession, (LPTR)(lpRGBline) );
	}

	SuperBlt( &BltSession, NULL );
	FreeUp((LPTR)lpRGBline);

	// if we aborted make sure we update the Invalidation Rectangle
//	if ((y < (rRepair.bottom-rRepair.top)) && lpInvalidRect)
//	{
//		*lpInvalidRect     = rRepair;
//		lpInvalidRect->top = y+rRepair.top;
//		return(FALSE);
//	}

	return(TRUE);
}

/***********************************************************************/
static void DrawColorPickerCross( HWND hWindow, BOOL on )
/***********************************************************************/
{
	int dx, dy;
	RECT ClientRect, rRepair;
	HPEN hOldPen;
	HDC hDC;
	int cx, cy;
	static int x, y;

#define LDELTA 7
#define SDELTA 1

	hDC = GetDC(hWindow);
	GetClientRect( hWindow, &ClientRect );
	InflateRect(&ClientRect, -1, -1); // skip over the focus rect and the border

	if ( !on )
	{ // redraw the area where the cursor was last drawn
		rRepair.left = rRepair.right = x;
		rRepair.top = rRepair.bottom = y;
		InflateRect( &rRepair, LDELTA+1, LDELTA+1 );
		DrawColorPicker( hDC, &ClientRect, iPickerIs, &PickerColor, &rRepair, NULL );
		UpdateWindow( hWindow );
	}
	else
	{
		ClientRect.bottom--;
		ClientRect.right--;
		dx = ClientRect.right - ClientRect.left;
		dy = ClientRect.bottom - ClientRect.top;
		GetPickerPoint( hWindow, &cx, &cy );
		x = FMUL( cx, FGET( dx, 255 ) ) + ClientRect.left;
		y = FMUL( cy, FGET( dy, 255 ) ) + ClientRect.top;

		hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN) );

		// Draw the horizontal dash
		MoveTo( hDC, x-LDELTA, y-1 );
		LineTo( hDC, x-SDELTA, y-1 );
		MoveTo( hDC, x-LDELTA, y );
		LineTo( hDC, x-SDELTA, y );
		MoveTo( hDC, x-LDELTA, y+1 );
		LineTo( hDC, x-SDELTA, y+1 );

		MoveTo( hDC, x+LDELTA, y-1 );
		LineTo( hDC, x+SDELTA, y-1 );
		MoveTo( hDC, x+LDELTA, y );
		LineTo( hDC, x+SDELTA, y );
		MoveTo( hDC, x+LDELTA, y+1 );
		LineTo( hDC, x+SDELTA, y+1 );

		// Draw the vertical dash
		MoveTo( hDC, x-1, y-LDELTA );
		LineTo( hDC, x-1, y-SDELTA );
		MoveTo( hDC, x,   y-LDELTA );
		LineTo( hDC, x,   y-SDELTA );
		MoveTo( hDC, x+1, y-LDELTA );
		LineTo( hDC, x+1, y-SDELTA );

		MoveTo( hDC, x-1, y+LDELTA );
		LineTo( hDC, x-1, y+SDELTA );
		MoveTo( hDC, x,   y+LDELTA );
		LineTo( hDC, x,   y+SDELTA );
		MoveTo( hDC, x+1, y+LDELTA );
		LineTo( hDC, x+1, y+SDELTA );

		SelectObject(hDC, hOldPen);
	}

	ReleaseDC(hWindow, hDC);
}


