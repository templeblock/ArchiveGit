#include "pp.h"
#include "ppafx.h"
#include "id.h"
#include "data.h"
#include "ppmodal.h"
#include "dlgjpeg.h"
// globals needed for setup import options
BOOL bDefaultsRead = NO;
IMPORTSETUP TempSetup;
FNAME szExportProfile;
IMPORTSETUP ImportSetup;
static LPIMAGE Save_lpImage;

// local functions for import dialog
BOOL SetupImportOptions( int );
void SaveImportSetup( void );
void UpdateProfile( IMPORTSETUP );
int GetBitmapType( int );
int GetColorMode( HDC );
ITEMID HandleColorModeCombo( HWND, ITEMID, UINT );
BOOL WINPROC EXPORT DlgEpsOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam );
BOOL WINPROC EXPORT DlgTiffOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam );
BOOL WINPROC EXPORT DlgTGAOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam );
BOOL WINPROC EXPORT DlgJPEGOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam );
BOOL WINPROC EXPORT DlgPP4OptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam );
BOOL WINPROC EXPORT DlgPPFFOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam );
BOOL WINPROC EXPORT DlgAVIOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam );
BOOL WINPROC EXPORT ImportDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

//***********************************************************************
//	returns FALSE for cancel.
// 	Note: AVI options reqire some prep.
//***********************************************************************
BOOL SetupSaveOptions(int idnFileType, BOOL bAlways, LPIMAGE lpImage)
//***********************************************************************
{
	BOOL bRet;
	BOOL old;
	CJPEGOptionsDlg dlg;
	
	switch (idnFileType)
	{
		case IDN_DCS:
			if (!Control.Retail)
				return(FALSE);
			if (!bAlways && !EPSOptions.bDCSPromptUser)
				return(TRUE);													    
			old = EPSOptions.bEPSPromptUser;
			EPSOptions.bEPSPromptUser = EPSOptions.bDCSPromptUser;
			bRet = (BOOL)AstralDlgParam(NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_DCSOPTIONS, DlgEpsOptionsProc, (LPARAM)lpImage);
			EPSOptions.bDCSPromptUser = EPSOptions.bEPSPromptUser;
			EPSOptions.bEPSPromptUser = old;
		break;
		case IDN_EPS:
			if (!bAlways && !EPSOptions.bEPSPromptUser)
				return(TRUE);													    
			bRet = (BOOL)AstralDlgParam(NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_EPSOPTIONS, DlgEpsOptionsProc, (LPARAM)lpImage);
		break;
		case IDN_TIFF:
			if (!bAlways && !TIFFOptions.bTIFFPromptUser)
				return(TRUE);
			bRet = (BOOL)AstralDlgParam(NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_TIFFOPTIONS, DlgTiffOptionsProc, (LPARAM)lpImage);
		break;
		case IDN_TARGA:
			if (!bAlways && !TGAOptions.bTGAPromptUser)
				return(TRUE);
			bRet = (BOOL)AstralDlgParam(NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_TGAOPTIONS, DlgTGAOptionsProc, (LPARAM)lpImage);
		break;
		case IDN_JPEG:
			if (!bAlways && !JPEGOptions.bJPEGPromptUser)
				return(TRUE);
			bRet = dlg.DoModal();
		break;
		case IDN_PP:
		case IDN_PPCOMPOUND:
			if (!bAlways && !PPFFOptions.bPPFFPromptUser)
				return(TRUE);
			bRet = (BOOL)AstralDlgParam(NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(),
					IDD_PPFFOPTIONS, DlgPPFFOptionsProc,
					(LPARAM)lpImage);
		break;
		case IDN_PP4:
			if (!bAlways && !PPFFOptions.bPPFFPromptUser)
				return(TRUE);
			bRet = (BOOL)AstralDlgParam(NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(),
					IDD_PP4OPTIONS, DlgPP4OptionsProc,
					(LPARAM)lpImage);
		break;
		case IDN_AVI:
			if (!bAlways && !AVIOptions.bAVIPromptUser)
				return(TRUE);
			bRet = (BOOL)AstralDlgParam(NO, PictPubApp.GetResourceHandle(), PictPubApp.Get_hWndAstral(), IDD_AVIOPTIONS, DlgAVIOptionsProc, (LPARAM)lpImage);
		break;
		default:			
			return(TRUE);
	}
	if (bRet)
		SaveSaveOptions();
	return(bRet);
}

//***********************************************************************
BOOL GetSaveOptions(int idnFileType, LPPVOID lppOptions)
//***********************************************************************
{
	BOOL fRet;
	LPVOID lpOptions;

	fRet = TRUE;
	lpOptions = NULL;

	switch (idnFileType)
	{
		case IDN_DCS:
		case IDN_EPS:
			lpOptions = Alloc(sizeof(EPSOPTIONS));
			if (!lpOptions)
			{
				Message(IDS_EMEMALLOC);
				fRet = FALSE;
				break;
			}
			copy((LPTR)&EPSOptions, (LPTR)lpOptions, sizeof(EPSOPTIONS));
		break;
		case IDN_TIFF:
			lpOptions = Alloc(sizeof(TIFFOPTIONS));
			if (!lpOptions)
			{
				Message(IDS_EMEMALLOC);
				fRet = FALSE;
				break;
			}
			copy((LPTR)&TIFFOptions, (LPTR)lpOptions, sizeof(TIFFOPTIONS));
		break;
		case IDN_TARGA:
			lpOptions = Alloc(sizeof(TGAOPTIONS));
			if (!lpOptions)
			{
				Message(IDS_EMEMALLOC);
				fRet = FALSE;
				break;
			}
			copy((LPTR)&TGAOptions, (LPTR)lpOptions, sizeof(TGAOPTIONS));
		break;
		case IDN_JPEG:
			lpOptions = Alloc(sizeof(JPEGOPTIONS));
			if (!lpOptions)
			{
				Message(IDS_EMEMALLOC);
				fRet = FALSE;
				break;
			}
			copy((LPTR)&JPEGOptions, (LPTR)lpOptions, sizeof(JPEGOPTIONS));
		break;
		case IDN_PP:
		case IDN_PPCOMPOUND:
		case IDN_PP4:
			lpOptions = Alloc(sizeof(PPFFOPTIONS));
			if (!lpOptions)
			{
				Message(IDS_EMEMALLOC);
				fRet = FALSE;
				break;
			}
			copy((LPTR)&PPFFOptions, (LPTR)lpOptions, sizeof(PPFFOPTIONS));
		break;
		case IDN_AVI:
			lpOptions = Alloc(sizeof(AVIOPTIONS));
			if (!lpOptions)
			{
				Message(IDS_EMEMALLOC);
				fRet = FALSE;
				break;
			}
			copy((LPTR)&AVIOptions, (LPTR)lpOptions, sizeof(AVIOPTIONS));
		break;
		default:			
			break;
	}
*lppOptions = lpOptions;
return(fRet);
}

BOOL SetupImportOptions( int idFileType )
{
    // get user input from previous session, first time through
    if( !bDefaultsRead )
    {
        GetDefInt( ImportSetup.Import_Anti_Alias, TRUE );
        GetDefInt( ImportSetup.Import_Resolution, 75 );
        GetDefInt( ImportSetup.Import_BitmapMode, IDC_IMAGE_24BIT );

        bDefaultsRead = TRUE;
    }

    if( !( AstralDlg( NO, ::AfxGetInstanceHandle(), ::AfxGetMainWnd()->GetSafeHwnd(), IDD_IMPORTSETUP, ImportDlgProc )))
        return FALSE;

    return TRUE;
}

// LOCAL prototypes
LOCAL BOOL EpsOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void EpsOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
static EPSOPTIONS EpsOptions_Save;

/***********************************************************************/
BOOL WINPROC EXPORT DlgEpsOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, EpsOptions_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, EpsOptions_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_EPSOPTIONS);
	}
}

/************************************************************************/
LOCAL BOOL EpsOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
EpsOptions_Save = EPSOptions;

CheckComboItem(hDlg, IDC_PREVIEWTYPE, IDC_PREVIEWNONE, IDC_PREVIEW24, EPSOptions.PreviewBPP);
CheckDlgButton(hDlg, IDC_EPSCOMPOSITE, EPSOptions.CompositeBPP);
SetDlgItemText( hDlg, IDC_STYLENAME, Names.PrintStyle );

EPSOptions.ClipPath[0] = '\0';
InitExtNameN( hDlg, IDC_EXTNAMES, IDC_FILENAME,
			EPSOptions.ClipPath, IDN_SHAPE, IDS_NONE, NO);

CheckDlgButton(hDlg, IDC_PROMPTFOROPTIONS, EPSOptions.bEPSPromptUser);
return(TRUE);
}

/************************************************************************/
LOCAL void EpsOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
FNAME szFileName;
STRING szNone;

switch (id)
	{
	case IDOK:
		if (!LookupExtFile( EPSOptions.ClipPath, szFileName, IDN_SHAPE ))
			{
			if ( AstralStrEx( IDS_NONE, szNone, sizeof(szNone) ) )
				lstrcpy( EPSOptions.ClipPath, szNone );
			}
		AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
		EPSOptions = EpsOptions_Save;
		AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_EXTNAMES:
		GetExtNameN( hDlg, id, IDC_FILENAME, EPSOptions.ClipPath,
					IDN_SHAPE, codeNotify, IDS_NONE, NO);
	break;

	case IDC_EXTMANAGE:
		PopupMenu( hDlg, id, IDC_FILENAME );
	break;

	case IDC_ADDEXT:
	case IDC_DELETEEXT:
	case IDC_RENAMEEXT:
		ExtNameManager( hDlg, IDC_EXTNAMES, IDC_FILENAME, EPSOptions.ClipPath,
			IDN_SHAPE, id, NO );
	break;

	case IDC_PREVIEWTYPE:
		if ( !(id = HandleCombo( hDlg, id, codeNotify )) )
			break;
		EPSOptions.PreviewBPP = id;
	break;

	case IDC_EPSCOMPOSITE:
		EPSOptions.CompositeBPP = !EPSOptions.CompositeBPP;
		CheckDlgButton(hDlg, IDC_EPSCOMPOSITE, EPSOptions.CompositeBPP);
	break;

	case IDC_PROMPTFOROPTIONS:
		EPSOptions.bEPSPromptUser = !EPSOptions.bEPSPromptUser;
		CheckDlgButton(hDlg, id, EPSOptions.bEPSPromptUser);
	break;

	default:
	break;
   }
}

// LOCAL prototypes
LOCAL BOOL TiffOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void TiffOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static TIFFOPTIONS TiffOptions_Save;

/***********************************************************************/
BOOL WINPROC EXPORT DlgTiffOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, TiffOptions_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, TiffOptions_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_TIFFOPTIONS);
	}
}

/************************************************************************/
LOCAL BOOL TiffOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup(hDlg);
TiffOptions_Save = TIFFOptions;
CheckDlgButton(hDlg, IDC_PROMPTFOROPTIONS, TIFFOptions.bTIFFPromptUser);
CheckDlgButton(hDlg,IDC_TIFFCOMPRESS,TIFFOptions.TIFFCompress);
CheckDlgButton(hDlg,IDC_TIFFDIFF,TIFFOptions.TIFFDiff);
CheckDlgButton(hDlg,IDC_TIFFSAVEMASK,TIFFOptions.bSaveMask);

ControlEnable(hDlg, IDC_TIFFDIFF, TIFFOptions.TIFFCompress);
if( Control.Retail )
    CheckDlgButton(hDlg, IDC_CMS_SAVE, TIFFOptions.bWritePT);

Save_lpImage = (LPIMAGE)lParam;
if (Save_lpImage)
	{
	FRMTYPEINFO TypeInfo;

	ImgGetTypeInfo(Save_lpImage, &TypeInfo);
    if( Control.Retail )    
    	ControlEnable(hDlg, IDC_CMS_SAVE, IsDstPTSelected( &TypeInfo.ptInfo ));
	ControlEnable(hDlg, IDC_TIFFSAVEMASK, ImgGetMask(Save_lpImage) != NULL);
	}
return(TRUE);
}

/************************************************************************/
LOCAL void TiffOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_CMS_SAVE:
        if( Control.Retail )  
        {
		    TIFFOptions.bWritePT = !TIFFOptions.bWritePT;
		    CheckDlgButton(hDlg, id, TIFFOptions.bWritePT);
        }
	break;
	case IDC_TIFFCOMPRESS:
		TIFFOptions.TIFFCompress = !TIFFOptions.TIFFCompress;
		CheckDlgButton(hDlg,id,TIFFOptions.TIFFCompress);
		ControlEnable(hDlg, IDC_TIFFDIFF, TIFFOptions.TIFFCompress);
	break;
	case IDC_TIFFDIFF:
		TIFFOptions.TIFFDiff = !TIFFOptions.TIFFDiff;
		CheckDlgButton(hDlg,id,TIFFOptions.TIFFDiff);
	break;
	case IDC_TIFFSAVEMASK:
		TIFFOptions.bSaveMask = !TIFFOptions.bSaveMask;
		CheckDlgButton(hDlg,id,TIFFOptions.bSaveMask);
	break;
	case IDC_PROMPTFOROPTIONS:
		TIFFOptions.bTIFFPromptUser = !TIFFOptions.bTIFFPromptUser;
		CheckDlgButton(hDlg, id, TIFFOptions.bTIFFPromptUser);
	break;
	case IDOK:
		AstralDlgEnd(hDlg, TRUE);
	break;
	case IDCANCEL:
		TIFFOptions = TiffOptions_Save;
		AstralDlgEnd(hDlg, FALSE);
	break;

	default:
	break;
   }
}


// LOCAL prototypes
LOCAL BOOL TGAOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void TGAOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static TGAOPTIONS TGAOptions_Save;

/***********************************************************************/
BOOL WINPROC EXPORT DlgTGAOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, TGAOptions_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, TGAOptions_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_TGAOPTIONS);
	}
}

/************************************************************************/
LOCAL BOOL TGAOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup(hDlg);
TGAOptions_Save = TGAOptions;
CheckDlgButton(hDlg, IDC_PROMPTFOROPTIONS, TGAOptions.bTGAPromptUser);
CheckDlgButton(hDlg,IDC_TIFFSAVEMASK,TGAOptions.bSaveMask);

Save_lpImage = (LPIMAGE)lParam;
if (Save_lpImage)
	{
	FRMTYPEINFO TypeInfo;

	ImgGetTypeInfo(Save_lpImage, &TypeInfo);
	ControlEnable(hDlg, IDC_TIFFSAVEMASK,
		(ImgGetMask(Save_lpImage) != NULL) && (TypeInfo.DataType == FDT_RGBCOLOR));
	}
return(TRUE);
}

/************************************************************************/
LOCAL void TGAOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_TIFFSAVEMASK:
		TGAOptions.bSaveMask = !TGAOptions.bSaveMask;
		CheckDlgButton(hDlg,id,TGAOptions.bSaveMask);
	break;
	case IDC_PROMPTFOROPTIONS:
		TGAOptions.bTGAPromptUser = !TGAOptions.bTGAPromptUser;
		CheckDlgButton(hDlg, id, TGAOptions.bTGAPromptUser);
	break;
	case IDOK:
		AstralDlgEnd(hDlg, TRUE);
	break;
	case IDCANCEL:
		TGAOptions = TGAOptions_Save;
		AstralDlgEnd(hDlg, FALSE);
	break;

	default:
	break;
   }
}


// LOCAL prototypes
LOCAL BOOL PP4Options_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PP4Options_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void PP4Options_Enable(HWND hDlg);

static PPFFOPTIONS PPFFOptions_Save;

/***********************************************************************/
BOOL WINPROC EXPORT DlgPP4OptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PP4Options_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PP4Options_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PP4OPTIONS);
	}
}

/************************************************************************/
LOCAL BOOL PP4Options_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup(hDlg);
PPFFOptions_Save = PPFFOptions;
CheckDlgButton(hDlg, IDC_PROMPTFOROPTIONS, PPFFOptions.bPPFFPromptUser);
CheckDlgButton(hDlg,IDC_PPFFCOMPRESS,PPFFOptions.PPFFCompress);
CheckDlgButton(hDlg,IDC_PPFFSAVEMASK,PPFFOptions.bSaveMask);
if( Control.Retail )    
    CheckDlgButton(hDlg, IDC_CMS_SAVE, PPFFOptions.bWritePT);

Save_lpImage = (LPIMAGE)lParam;

PP4Options_Enable(hDlg);
return(TRUE);
}

/************************************************************************/
LOCAL void PP4Options_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_CMS_SAVE:
        if( Control.Retail )  
            {
		    PPFFOptions.bWritePT = !PPFFOptions.bWritePT;
		    CheckDlgButton(hDlg, id, PPFFOptions.bWritePT);
            }
	break;
	case IDC_PPFFCOMPRESS:
		PPFFOptions.PPFFCompress = !PPFFOptions.PPFFCompress;
		CheckDlgButton(hDlg,id,PPFFOptions.PPFFCompress);
	break;
	case IDC_PPFFSAVEMASK:
		PPFFOptions.bSaveMask = !PPFFOptions.bSaveMask;
		CheckDlgButton(hDlg,id,PPFFOptions.bSaveMask);
	break;
	case IDC_PROMPTFOROPTIONS:
		PPFFOptions.bPPFFPromptUser = !PPFFOptions.bPPFFPromptUser;
		CheckDlgButton(hDlg, id, PPFFOptions.bPPFFPromptUser);
	break;
	case IDOK:
		AstralDlgEnd(hDlg, TRUE);
	break;
	case IDCANCEL:
		PPFFOptions = PPFFOptions_Save;
		AstralDlgEnd(hDlg, FALSE);
	break;
   default:
	break;
   }
}

/************************************************************************/
LOCAL void PP4Options_Enable(HWND hDlg)
/************************************************************************/
{
if (Save_lpImage)
	{
	ControlEnable(hDlg, IDC_PPFFSAVEMASK, ImgGetMask(Save_lpImage) != NULL);
    if( Control.Retail )    
		{
		FRMTYPEINFO TypeInfo;

		ImgGetTypeInfo(Save_lpImage, &TypeInfo);
    	ControlEnable(hDlg, IDC_CMS_SAVE, IsDstPTSelected( &TypeInfo.ptInfo ));
		}
	}
}

// LOCAL prototypes
LOCAL BOOL PPFFOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void PPFFOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);
LOCAL void PPFFOptions_Enable(HWND hDlg);

/***********************************************************************/
BOOL WINPROC EXPORT DlgPPFFOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, PPFFOptions_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, PPFFOptions_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PPFFOPTIONS);
	}
}

/************************************************************************/
LOCAL BOOL PPFFOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup(hDlg);
PPFFOptions_Save = PPFFOptions;
CheckDlgButton(hDlg, IDC_PROMPTFOROPTIONS, PPFFOptions.bPPFFPromptUser);
CheckDlgButton(hDlg,IDC_PPFFCOMPRESS,PPFFOptions.PPFFCompress);
CheckDlgButton(hDlg,IDC_PPFFSAVEMASK,PPFFOptions.bSaveMask);
CheckDlgButton(hDlg,IDC_PPFFSAVEPROXY,PPFFOptions.bSaveProxy);
if (!PPFFOptions.bSaveProxy)
	PPFFOptions.bSaveCmdList = TRUE;
CheckDlgButton(hDlg,IDC_PPFFSAVECMDLIST,PPFFOptions.bSaveCmdList);
CheckDlgButton(hDlg,IDC_PPFFSAVELINK, PPFFOptions.bSaveLink);
if( Control.Retail )    
    CheckDlgButton(hDlg, IDC_CMS_SAVE, PPFFOptions.bWritePT);

Save_lpImage = (LPIMAGE)lParam;

PPFFOptions_Enable(hDlg);
return(TRUE);
}

/************************************************************************/
LOCAL void PPFFOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
switch (id)
	{
	case IDC_CMS_SAVE:
        if( Control.Retail )  
            {
		    PPFFOptions.bWritePT = !PPFFOptions.bWritePT;
		    CheckDlgButton(hDlg, id, PPFFOptions.bWritePT);
            }
	break;
	case IDC_PPFFCOMPRESS:
		PPFFOptions.PPFFCompress = !PPFFOptions.PPFFCompress;
		CheckDlgButton(hDlg,id,PPFFOptions.PPFFCompress);
	break;
	case IDC_PPFFSAVEMASK:
		PPFFOptions.bSaveMask = !PPFFOptions.bSaveMask;
		CheckDlgButton(hDlg,id,PPFFOptions.bSaveMask);
	break;
	case IDC_PPFFSAVEPROXY:
		PPFFOptions.bSaveProxy = !PPFFOptions.bSaveProxy;
		CheckDlgButton(hDlg,id,PPFFOptions.bSaveProxy);
		if (!PPFFOptions.bSaveProxy)
			PPFFOptions.bSaveCmdList = TRUE;
		CheckDlgButton(hDlg,IDC_PPFFSAVECMDLIST,PPFFOptions.bSaveCmdList);
		PPFFOptions_Enable(hDlg);
	break;
	case IDC_PPFFSAVECMDLIST:
		PPFFOptions.bSaveCmdList = !PPFFOptions.bSaveCmdList;
		CheckDlgButton(hDlg,id,PPFFOptions.bSaveCmdList);
		PPFFOptions_Enable(hDlg);
	break;
	case IDC_PPFFSAVELINK:
		PPFFOptions.bSaveLink = !PPFFOptions.bSaveLink;
		CheckDlgButton(hDlg,id,PPFFOptions.bSaveLink);
	break;
	case IDC_PROMPTFOROPTIONS:
		PPFFOptions.bPPFFPromptUser = !PPFFOptions.bPPFFPromptUser;
		CheckDlgButton(hDlg, id, PPFFOptions.bPPFFPromptUser);
	break;
	case IDOK:
		AstralDlgEnd(hDlg, TRUE);
	break;
	case IDCANCEL:
		PPFFOptions = PPFFOptions_Save;
		AstralDlgEnd(hDlg, FALSE);
	break;
   default:
	break;
   }
}

/************************************************************************/
LOCAL void PPFFOptions_Enable(HWND hDlg)
/************************************************************************/
{
if (Save_lpImage)
	{
	FRMTYPEINFO TypeInfo;

	ImgGetTypeInfo(Save_lpImage, &TypeInfo);
	int iCount = ListGetCount(&Save_lpImage->lpCmdList->UndoPacketList);
	// gray save link if we don't have just a load in the command list
	ControlEnable(hDlg, IDC_PPFFSAVELINK, iCount == 1);
	ControlEnable(hDlg, IDC_PPFFSAVECMDLIST, iCount > 1 && PPFFOptions.bSaveProxy);
	ControlEnable(hDlg, IDC_PPFFSAVEMASK, ImgGetMask(Save_lpImage) &&
					(!PPFFOptions.bSaveCmdList || iCount <= 1));
	ControlEnable(hDlg, IDC_PPFFSAVEPROXY, iCount > 1 && PPFFOptions.bSaveCmdList);
    if( Control.Retail )    
    	ControlEnable(hDlg, IDC_CMS_SAVE, IsDstPTSelected( &TypeInfo.ptInfo ));
	}
else
	{
	// if not saving the proxy, you must save the command list
	ControlEnable(hDlg, IDC_PPFFSAVECMDLIST, PPFFOptions.bSaveProxy);
	ControlEnable(hDlg, IDC_PPFFSAVEPROXY, PPFFOptions.bSaveCmdList);

	}
}

// LOCAL prototypes
LOCAL BOOL AVIOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam);
LOCAL void AVIOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify);

static AVIOPTIONS AVIOptions_Save;

/***********************************************************************/
BOOL WINPROC EXPORT DlgAVIOptionsProc( HWND hDlg, UINT msg, WPARAM wParam,
                                 LPARAM lParam )
/***********************************************************************/
{
switch (msg)
	{
	HANDLE_DLGRET(hDlg, WM_INITDIALOG, AVIOptions_OnInitDialog);  
	HANDLE_DLGMSG(hDlg, WM_COMMAND, AVIOptions_OnCommand);

	default:
	return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_AVIOPTIONS);
	}
}

/************************************************************************/
LOCAL BOOL AVIOptions_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
CenterPopup( hDlg );
AVIOptions_Save = AVIOptions;

AVIOptions.iAVIPosType = bound(AVIOptions.iAVIPosType, 0, 2);
CheckRadioButton( hDlg, IDC_AVI_ADD_END, IDC_AVI_REPLACE,
				AVIOptions.iAVIPosType+IDC_AVI_ADD_END);
AVIOptions.iAVISizingType = bound(AVIOptions.iAVISizingType, 0, 1);
CheckRadioButton( hDlg, IDC_AVI_CLIP, IDC_AVI_SIZE,
				AVIOptions.iAVISizingType+IDC_AVI_CLIP);

InitDlgItemSpin(hDlg, IDC_AVI_POSI, AVIOptions.AVIPosition+1, NO, 1, Max(2, AVIOptions.AVILastPosition+1));
InitDlgItemSpin(hDlg, IDC_AVI_POSR, AVIOptions.AVIPosition+1, NO, 1, Max(2, AVIOptions.AVILastPosition+1));

ControlEnable(hDlg, IDC_AVI_POSI, 
		(AVIOptions.iAVIPosType+IDC_AVI_ADD_END) == IDC_AVI_INSERT);
ControlEnable(hDlg, IDC_AVI_POSR, 
		(AVIOptions.iAVIPosType+IDC_AVI_ADD_END) == IDC_AVI_REPLACE);
		
CheckDlgButton(hDlg, IDC_PROMPTFOROPTIONS, AVIOptions.bAVIPromptUser);
return(TRUE);
}

/************************************************************************/
LOCAL void AVIOptions_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
BOOL Bool;
int val;

switch (id)
	{
	case IDOK:
		AstralDlgEnd( hDlg, TRUE );
	break;

	case IDCANCEL:
		AVIOptions = AVIOptions_Save;
		AstralDlgEnd( hDlg, FALSE );
	break;

	case IDC_AVI_ADD_END:
	case IDC_AVI_INSERT:
	case IDC_AVI_REPLACE:
		AVIOptions.iAVIPosType = id - IDC_AVI_ADD_END;
		CheckRadioButton( hDlg, IDC_AVI_ADD_END, IDC_AVI_REPLACE,
						AVIOptions.iAVIPosType+IDC_AVI_ADD_END);
		ControlEnable(hDlg, IDC_AVI_POSI, 
				(AVIOptions.iAVIPosType+IDC_AVI_ADD_END) == IDC_AVI_INSERT);
		ControlEnable(hDlg, IDC_AVI_POSR, 
				(AVIOptions.iAVIPosType+IDC_AVI_ADD_END) == IDC_AVI_REPLACE);
	break;

	case IDC_AVI_CLIP:
	case IDC_AVI_SIZE:
		AVIOptions.iAVISizingType = id - IDC_AVI_CLIP;
		CheckRadioButton( hDlg, IDC_AVI_CLIP, IDC_AVI_SIZE,
						AVIOptions.iAVISizingType+IDC_AVI_CLIP);
	break;

	case IDC_AVI_POSI:
		val = GetDlgItemSpin(hDlg, id, &Bool, NO );
		if (!Bool)
			break;
		AVIOptions.AVIPosition = val-1;
		if (val != GetDlgItemSpin(hDlg, IDC_AVI_POSR, &Bool, NO ))
			if (Bool)
				SetDlgItemSpin(hDlg, IDC_AVI_POSR, val, NO );
	break;
	
	case IDC_AVI_POSR:
		val = GetDlgItemSpin(hDlg, id, &Bool, NO );
		if (!Bool)
			break;
		AVIOptions.AVIPosition = val-1;
		if (val != GetDlgItemSpin(hDlg, IDC_AVI_POSI, &Bool, NO ))
			if (Bool)
				SetDlgItemSpin(hDlg, IDC_AVI_POSI, val, NO );
	break;

	case IDC_PROMPTFOROPTIONS:
		AVIOptions.bAVIPromptUser = !AVIOptions.bAVIPromptUser;
		CheckDlgButton(hDlg, id, AVIOptions.bAVIPromptUser);
	break;
   default:
	break;
   }
}

// Local prototypes
LOCAL BOOL Import_OnInitDialog( HWND hDlg, HWND hWndFocus, LPARAM lParam );
LOCAL void Import_OnCommand( HWND hDlg, int id, HWND hControl, UINT codeNotify );

//////////////////////////////////////////////////////////////////////////////
BOOL WINPROC EXPORT ImportDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//////////////////////////////////////////////////////////////////////////////
{
    switch( msg )
	{
    	HANDLE_DLGRET( hDlg, WM_INITDIALOG, Import_OnInitDialog );  
	    HANDLE_DLGMSG( hDlg, WM_COMMAND, Import_OnCommand );
    	default:
    	    return Dialog_HandleDefaults( hDlg, msg, wParam, lParam, IDD_IMPORTSETUP );
	}
}

//////////////////////////////////////////////////////////////////////////////
LOCAL BOOL Import_OnInitDialog( HWND hDlg, HWND hWndFocus, LPARAM lParam )
//////////////////////////////////////////////////////////////////////////////
{
    int idMode;

    CenterPopup( hDlg );
    
    HDC hDC = GetDC( NULL );
    idMode = GetColorMode( hDC );   // get current display driver setting
    ReleaseDC( NULL, hDC );

    // disable unavailable modes
    switch( idMode )
    {
	    case IDC_IMAGE_COLOR16:
            ControlEnable( hDlg, IDC_IMAGE_COLOR256, FALSE );     
            ControlEnable( hDlg, IDC_IMAGE_GRAYSCALE256, FALSE );  // fall through

	    case IDC_IMAGE_COLOR256:                       
             ControlEnable( hDlg, IDC_IMAGE_24BIT, FALSE );
        default:
            break;
    }
    if( ImportSetup.Import_BitmapMode > idMode )
        ImportSetup.Import_BitmapMode = idMode; 

    if( ImportSetup.Import_BitmapMode != IDC_IMAGE_24BIT && 
        ImportSetup.Import_BitmapMode != IDC_IMAGE_GRAYSCALE256 &&
        ImportSetup.Import_BitmapMode != IDC_IMAGE_GRAYSCALE16 )
    {
        CheckDlgButton( hDlg, IDC_ANTIALIAS, FALSE );
        ControlEnable( hDlg, IDC_ANTIALIAS, FALSE );
    }
    else
        CheckDlgButton( hDlg, IDC_ANTIALIAS, ImportSetup.Import_Anti_Alias );

    CheckComboItem( hDlg, IDC_IMAGE_TYPE, IDC_IMAGE_FIRST,
	    IDC_IMAGE_LAST, ImportSetup.Import_BitmapMode );

    InitDlgItemSpin( hDlg, IDC_EDITRES, ImportSetup.Import_Resolution, YES, 0, 400 );

    TempSetup = ImportSetup;

    return( TRUE );
}

//////////////////////////////////////////////////////////////////////////////
LOCAL void Import_OnCommand( HWND hDlg, int id, HWND hControl, UINT codeNotify )
//////////////////////////////////////////////////////////////////////////////
{
    BOOL Bool;
    int idType;

    switch( id )
	{
        case IDC_IMAGE_TYPE:
	        if( !( idType = HandleColorModeCombo( hDlg, id, codeNotify )))
		        break;
            TempSetup.Import_BitmapMode = idType;

            if( TempSetup.Import_BitmapMode == IDC_IMAGE_24BIT || 
                TempSetup.Import_BitmapMode == IDC_IMAGE_GRAYSCALE256 ||
                TempSetup.Import_BitmapMode == IDC_IMAGE_GRAYSCALE16 )
            {
                ControlEnable( hDlg, IDC_ANTIALIAS, TRUE );
                CheckDlgButton( hDlg, IDC_ANTIALIAS, TempSetup.Import_Anti_Alias );
            }
            else
            {
                CheckDlgButton( hDlg, IDC_ANTIALIAS, FALSE );
                ControlEnable( hDlg, IDC_ANTIALIAS, FALSE );
            }
            break;

        case IDC_EDITRES:
	        if( GetFocus() != GetDlgItem( hDlg, id ))
		        break;
	        if( codeNotify != EN_CHANGE )
		        break;
            TempSetup.Import_Resolution = GetDlgItemSpin( hDlg, id, &Bool, YES );
            break;

        case IDC_ANTIALIAS:
            TempSetup.Import_Anti_Alias = !TempSetup.Import_Anti_Alias;
            CheckDlgButton( hDlg, IDC_ANTIALIAS, TempSetup.Import_Anti_Alias );
            break;

        case IDOK:
	        AstralDlgEnd( hDlg, TRUE );
            ImportSetup = TempSetup;
            UpdateProfile( ImportSetup );
        	break;

    	case IDCANCEL:
	        AstralDlgEnd( hDlg, FALSE );
        	break;

        default:
            break;
    }
}

//////////////////////////////////////////////////////////////////////////////
void UpdateProfile( IMPORTSETUP ImportSetup )
//  convert and write out current import values to export filter profile
//////////////////////////////////////////////////////////////////////////////
{
    int AntiAlias = FALSE;

    if( ImportSetup.Import_BitmapMode == IDC_IMAGE_24BIT || 
        ImportSetup.Import_BitmapMode == IDC_IMAGE_GRAYSCALE256 ||
        ImportSetup.Import_BitmapMode == IDC_IMAGE_GRAYSCALE16 )
    {
        AntiAlias = ImportSetup.Import_Anti_Alias;
    }    
    
    PutPrivInt( szExportProfile, "Standard Options", Size Mode, 602 );
    PutPrivInt( szExportProfile, "Standard Options", Bitmap Mode, GetBitmapType( ImportSetup.Import_BitmapMode ));

    // resolution is doubled for anti-aliasing but cannot exceed 400
    ImportSetup.Import_Resolution = Bound( ImportSetup.Import_Resolution, 32, AntiAlias ? 200:400 ); 

    PutPrivInt( szExportProfile, "Standard Options", X Resolution, ImportSetup.Import_Resolution << ( AntiAlias ? 1:0 ));
    PutPrivInt( szExportProfile, "Standard Options", Y Resolution, ImportSetup.Import_Resolution << ( AntiAlias ? 1:0 ));
}

//////////////////////////////////////////////////////////////////////////////
void SaveImportSetup()
//  write out current import values to PP.INI
//////////////////////////////////////////////////////////////////////////////
{
    if( !bDefaultsRead )
        return;

    PutDefInt( ImportSetup.Import_Anti_Alias, ImportSetup.Import_Anti_Alias );
    PutDefInt( ImportSetup.Import_Resolution, ImportSetup.Import_Resolution );
    PutDefInt( ImportSetup.Import_BitmapMode, ImportSetup.Import_BitmapMode );
}

//////////////////////////////////////////////////////////////////////////////
int GetBitmapType( int mode )
//  returns code to specify for bitmap type in export filter profile.
//////////////////////////////////////////////////////////////////////////////
{
    switch( mode )
    {
        case IDC_IMAGE_LINEART:   
            return( LINE_ART );
        case IDC_IMAGE_GRAYSCALE16: 
            return( GRAY_SCALE16 );
        case IDC_IMAGE_GRAYSCALE256: 
            return( GRAY_SCALE256 );
        case IDC_IMAGE_COLOR16:  
            return( COLOR_16 );
        case IDC_IMAGE_COLOR256:  
            return( COLOR_256 );
        case IDC_IMAGE_24BIT:     
        default:
           return( COLOR_24BIT );
    }
}

//////////////////////////////////////////////////////////////////////////////
int GetColorMode( HDC hDC )
//////////////////////////////////////////////////////////////////////////////
{
    int nBits       = GetDeviceCaps( hDC, BITSPIXEL );
    int nPlanes     = GetDeviceCaps( hDC, PLANES );

    nBits *= nPlanes;
    if( nBits <= 4 )
	    return( IDC_IMAGE_COLOR16 );
    else 
    if( nBits <= 8 )
	    return( IDC_IMAGE_COLOR256 );
    else
	    return( IDC_IMAGE_24BIT );
}

//////////////////////////////////////////////////////////////////////////////
ITEMID HandleColorModeCombo( HWND hDlg, ITEMID idCombo, UINT codeNotify )
//////////////////////////////////////////////////////////////////////////////
{
    ITEMID idItem;
    HWND hControl, hItem;

    #define OKITEM( id ) \
        (( hItem = GetDlgItem( hDlg, id )) && IsWindowEnabled( hItem ))

    if( codeNotify != CBN_SELCHANGE && codeNotify != CBN_DBLCLK )
	    return( FALSE );

    if( !( hControl = GetDlgItem( hDlg, idCombo )))
	    return( FALSE );

    idItem = idCombo + 1;
    idItem += ComboBox_GetCurSel( hControl ); 

    if( !OKITEM( idItem ))  // Mode can't be selected, so select next lowest enabled item
    {   
	    for( idItem = idItem - 1; idItem > idCombo; idItem-- )
	    {
		    if( !OKITEM( idItem ))
			    continue;
		    ComboBox_SetCurSel( hControl, idItem - ( idCombo + 1 ));
		    break;
	    }
	    return( FALSE );
    }
    return( idItem );
}


