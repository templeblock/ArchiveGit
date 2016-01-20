//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "paledit.h"

/***********************************************************************/
BOOL PaletteEdit( LPIMAGE lpImage )
/***********************************************************************/
{
PALETTEEDIT_PARMS parms;
LPCOLORMAP lpColorMap;
FRMTYPEINFO TypeInfo;
	 
if (!lpImage)
	return(FALSE);
ImgGetTypeInfo(lpImage, &TypeInfo);
if (TypeInfo.DataType != FDT_PALETTECOLOR)
	return(FALSE);
lpColorMap = PaletteEditor(TypeInfo.ColorMap);
if (lpColorMap)
	{
	parms.fReMap = PaletteSettings.fReMap;
	parms.DitherType = PaletteSettings.DitherType;
	parms.lpColorMap = lpColorMap;
	ProcessCommand(lpImage->lpCmdList, IDS_CMD_PALETTEEDIT, &parms);
	return(TRUE);
	}
return(FALSE);
}

/***********************************************************************/
BOOL PaletteEditImage(LPIMAGE lpImage, LPPALETTEEDIT_PARMS lpParms)
/***********************************************************************/
{
FRMTYPEINFO TypeInfo;

ImgGetTypeInfo(lpImage, &TypeInfo);
if (TypeInfo.DataType != FDT_PALETTECOLOR)
	{
	Message(IDS_NOTPALETTECOLOR);
	return(FALSE);
	}
	
lpParms->Common.StatusCode= ApplyColorMap(lpImage, lpParms->lpColorMap, lpParms->fReMap,
					lpParms->DitherType, lpParms->Common.idDirty);
if (lpParms->Common.StatusCode == SC_SUCCESS)
	lpParms->Common.UpdateType = UT_DATATYPE;
return(lpParms->Common.StatusCode == SC_SUCCESS);	
}

/***********************************************************************/
LPCOLORMAP PaletteEditor( LPCOLORMAP lpColorMap )
/***********************************************************************/
{
	HWND hWnd, hWndFocus;
	LPCOLORMAP lpNewColorMap;

	lpNewColorMap = FrameCopyColorMap(lpColorMap);
	if (!lpNewColorMap)
		{
		Message(IDS_EMEMALLOC);
		return(NULL);
		}
	
	// always allow user to edit 256 entries
	//lpNewColorMap->NumEntries = 256;

	// Get the proper parent window
	hWndFocus = GetFocus();

	if ( !(hWnd = GetParent(hWndFocus)) )
		hWnd = hWndFocus;

	// Show the Palette Editor Dialog
	if ( !AstralDlgParam( 
		NO, PictPubApp.GetResourceHandle(), 
		hWnd, IDD_PALETTEEDITOR, 
		DlgPaletteEditorProc,
		(LPARAM)lpNewColorMap) )
	{
		FrameDestroyColorMap(lpNewColorMap);
		return( NULL );
	}

	SetFocus( hWndFocus );

	return(lpNewColorMap);
}

// LOCAL prototypes
LOCAL BOOL PaletteEditor_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PaletteEditor_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL BOOL PaletteEditor_OnQueryNewPalette(HWND hDlg);

static LPCOLORMAP PickColorMap;

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaletteEditorProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PaletteEditor_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PaletteEditor_OnCommand);
	HANDLE_DLGRET(hDlg, WM_QUERYNEWPALETTE, PaletteEditor_OnQueryNewPalette);

	HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, Dialog_OnPaletteChanged);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTEEDITOR);
	}
}

/***********************************************************************/
LOCAL BOOL PaletteEditor_OnQueryNewPalette(HWND hDlg)
/***********************************************************************/
{
return(HandleQueryNewPalette(hDlg, YES, YES));
}

/************************************************************************/
LOCAL BOOL PaletteEditor_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
ITEMID id;

CenterPopup( hDlg );

// lParam - lpColorMap pointer
PickColorMap = (LPCOLORMAP)lParam;
PalPicker_Init(GetDlgItem(hDlg, IDC_PALETTEPICKER), PickColorMap, 0);
CheckRadioButton(hDlg, IDC_PALETTEREPLACE, IDC_PALETTEREMAP,
		PaletteSettings.fReMap ? IDC_PALETTEREMAP : IDC_PALETTEREPLACE);
if (PaletteSettings.DitherType == DT_SCATTER)
	id = IDC_DITHERSCATTERED;
else
if (PaletteSettings.DitherType == DT_PATTERN)
	id = IDC_DITHERPATTERN;
else
	id = IDC_DITHERNONE;
CheckComboItem( hDlg, IDC_DITHERTYPES, IDC_DITHERNONE,
	IDC_DITHERSCATTERED, id );

ControlEnable(hDlg, IDC_DITHERTYPES, PaletteSettings.fReMap);
ControlEnable(hDlg, IDOK, FALSE);

return(TRUE);
}

/************************************************************************/
LOCAL void PaletteEditor_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
COLORINFO NewColor;
LPCOLORMAP lpColorMap;
int iSelect;
EXTNAME szPaletteName;
FNAME szFileName;
int i;
BOOL fRet;

switch (id)
	{
	case IDC_PALETTEPICKER:
		if ( GetFocus() != GetDlgItem( hDlg, id ) )
			break;
		iSelect = PalPicker_GetIndex(GetDlgItem(hDlg, id));
		if (PickColorMap)
		{
			NewColor.rgb = PickColorMap->RGBData[iSelect];
			SetColorInfo(&NewColor, &NewColor, CS_RGB);
		}
		else
		{
			NewColor.gray = iSelect;
			SetColorInfo(&NewColor, &NewColor, CS_GRAY);
		}
		if (codeNotify && PickColorMap)
			{
			if (ColorPicker( &NewColor, NULL ))
				{
				PickColorMap->RGBData[iSelect] = NewColor.rgb;
				PalPicker_Init( GetDlgItem( hDlg, IDC_PALETTEPICKER ), PickColorMap, iSelect );
				ControlEnable(hDlg, IDOK, TRUE);
				}
			}
	break;

	case IDC_PALETTELOAD:
		ControlEnable(hDlg, id, FALSE);
        fRet = (BOOL)AstralDlgParam( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PALETTELOAD,
                    DlgPaletteEditorLoadProc, (LPARAM)szPaletteName );
		ControlEnable(hDlg, id, TRUE);
		if (!fRet)
			break;
		if ( !LookupExtFile( szPaletteName, szFileName, IDN_PALETTE ) )
			break;
		if (!(lpColorMap = Palette_ReadColorMap(szFileName)))
			break;
		PickColorMap->NumEntries = lpColorMap->NumEntries;
		for (i = 0; i < PickColorMap->NumEntries; ++i)
			PickColorMap->RGBData[i] = lpColorMap->RGBData[i];
		// always let the user edit 256 entries
		PickColorMap->NumEntries = 256;
		for (; i < PickColorMap->NumEntries; ++i)
			PickColorMap->RGBData[i].red =
			PickColorMap->RGBData[i].green =
			PickColorMap->RGBData[i].blue = 0;
		FrameDestroyColorMap(lpColorMap);
		PalPicker_Init(GetDlgItem(hDlg, IDC_PALETTEPICKER), PickColorMap, 0);
		SetWindowText(hDlg, szPaletteName);
		ControlEnable(hDlg, IDOK, TRUE);
	break;

	case IDC_PALETTESAVE:
		ControlEnable(hDlg, id, FALSE);
		fRet = (BOOL)AstralDlgParam( NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PALETTESAVE,
                    DlgPaletteEditorSaveProc, (LPARAM)szPaletteName );
		ControlEnable(hDlg, id, TRUE);
		if (!fRet)
			break;
		if ( LookupExtFile(szPaletteName, szFileName, IDN_PALETTE) )
			{
			if (AstralOKCancel(IDS_OVERWRITEEXTFILE,
				    (LPSTR)szPaletteName) == IDCANCEL)
				break;
			}
		else
			{
			if ( !CreateFileName( szPaletteName, IDN_PALETTE, szFileName) )
				break;
			}
		AstralCursor(IDC_WAIT);
		if (!Palette_WriteColorMap(szFileName, PickColorMap, szPaletteName))
			{
			AstralCursor(NULL);
			break;
			}
		AstralCursor(NULL);
		if (!AddExtFile(szPaletteName, szFileName, IDN_PALETTE))
			{
			FileDelete(szFileName);
			break;
			}
		SetWindowText(hDlg, szPaletteName);
	break;

	case IDC_PALETTEREPLACE:
	case IDC_PALETTEREMAP:
		PaletteSettings.fReMap = id == IDC_PALETTEREMAP;
		CheckRadioButton(hDlg, IDC_PALETTEREPLACE, IDC_PALETTEREMAP,
			PaletteSettings.fReMap ? IDC_PALETTEREMAP : IDC_PALETTEREPLACE);
		ControlEnable(hDlg, IDC_DITHERTYPES, PaletteSettings.fReMap);
	break;

	case IDC_DITHERTYPES:
//	case IDC_DITHERPATTERN:
//	case IDC_DITHERSCATTERED:
//	case IDC_DITHERNONE:
	if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
		break;
	if ( id == IDC_DITHERPATTERN )
		PaletteSettings.DitherType = DT_PATTERN;
	else
	if ( id == IDC_DITHERSCATTERED )
		PaletteSettings.DitherType = DT_SCATTER;
	else
		PaletteSettings.DitherType = DT_NONE;
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

// LOCAL prototypes
LOCAL BOOL PaletteEditorLoad_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PaletteEditorLoad_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static LPSTR lpLoadPaletteName;

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaletteEditorLoadProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PaletteEditorLoad_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PaletteEditorLoad_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTELOAD);
	}
}

/************************************************************************/
LOCAL BOOL PaletteEditorLoad_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
lpLoadPaletteName = (LPSTR)lParam;
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpLoadPaletteName, IDN_PALETTE );
return(TRUE);
}

/************************************************************************/
LOCAL void PaletteEditorLoad_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDOK:
	case IDCANCEL:
	AstralDlgEnd( hDlg, id == IDOK );
	break;

	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		lpLoadPaletteName, IDN_PALETTE, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpLoadPaletteName,
		IDN_PALETTE, id, NO );
	break;

    default:
	break;
    }
}

// LOCAL prototypes
LOCAL BOOL PaletteEditorSave_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PaletteEditorSave_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static LPSTR lpSavePaletteName;

/***********************************************************************/
BOOL WINPROC EXPORT DlgPaletteEditorSaveProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PaletteEditorSave_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PaletteEditorSave_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PALETTESAVE);
	}
}

/************************************************************************/
LOCAL BOOL PaletteEditorSave_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
lpSavePaletteName = (LPSTR)lParam;
InitExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpSavePaletteName, IDN_PALETTE );
return(TRUE);
}

/************************************************************************/
LOCAL void PaletteEditorSave_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_EXTNAMES:
	GetExtName( hDlg, IDC_EXTNAMES, IDC_FILENAME,
		lpSavePaletteName, IDN_PALETTE, codeNotify );
	break;

	case IDC_EXTMANAGE:
	PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
	ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, lpSavePaletteName,
		IDN_PALETTE, id, NO );
	break;

	case IDOK:
	case IDCANCEL:
	AstralDlgEnd( hDlg, id == IDOK );
	break;

    default:
	break;
    }
}

