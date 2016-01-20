// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
********************************************************************************
********************************************************************************

		 Astral.cpp    Astral picture publishing application    

********************************************************************************
********************************************************************************

*/

#define MAIN
/****** Drag-and-drop support - copied from windows.h (3.1)  ****************/
#define WM_DROPFILES        0x0233

#include "pp.h"
#include "initguid.h"
#include "ppafx.h"
#include "id.h"
#include "twain.h"
#include "dca_glue.h"
#include "dca_acq.h"
#include "plugin.h"
#include "custools.h"
#include "ipframe.h"
#include "svrdoc.h"
#include "svrview.h"
#include "ctxhelp.h"
#include "ppimport.h"
#include "objfloat.h"
#include "bomb.h"
#include "monitor.h"
#include "pptbars.h"
#include "commctrl.h"
#include "defaults.h"

#define _FASTINIT

/* ********************************* History ******************************** */

/* ******************************** Constants ******************************* */

// These identifiers are generated to be statistically unique and represent the
// types of objects the we can 'server' up
DEFINE_GUID(OLE_CLSID_PP, 0xBC7A9620L, 0x3913, 0x101B, 0x96, 0x49, 0x02, 0x00, 0x80, 0xA3, 0x8E, 0x74);
#define PP_GUIDID_STR _T("{BC7A9620-3913-101B-9649-020080A38E74}")

/* ********************************** Macros ******************************** */

/* ********************************** Types ********************************* */

struct CLIPBOARDFILE
{
	ULONG	ClipboardID;
	FNAME	szFileName;
	int		nReferenceCount;
};
// following typedefs extracted from Chicago BetaSDK (winuser.h)
typedef struct tagWNDCLASSEXA {
    UINT        cbSize;
    /* Win 3.x */
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCSTR      lpszMenuName;
    LPCSTR      lpszClassName;
    /* Win 4.0 */
    HICON       hIconSm;
} WNDCLASSEXA, *PWNDCLASSEXA, NEAR *NPWNDCLASSEXA, FAR *LPWNDCLASSEXA;
typedef struct tagWNDCLASSEXW {
    UINT        cbSize;
    /* Win 3.x */
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCWSTR     lpszMenuName;
    LPCWSTR     lpszClassName;
    /* Win 4.0 */
    HICON       hIconSm;
} WNDCLASSEXW, *PWNDCLASSEXW, NEAR *NPWNDCLASSEXW, FAR *LPWNDCLASSEXW;
#ifdef UNICODE
typedef WNDCLASSEXW WNDCLASSEX;
typedef PWNDCLASSEXW PWNDCLASSEX;
typedef NPWNDCLASSEXW NPWNDCLASSEX;
typedef LPWNDCLASSEXW LPWNDCLASSEX;
#else
typedef WNDCLASSEXA WNDCLASSEX;
typedef PWNDCLASSEXA PWNDCLASSEX;
typedef NPWNDCLASSEXA NPWNDCLASSEX;
typedef LPWNDCLASSEXA LPWNDCLASSEX;
#endif // UNICODE

/* ******************************* Local Data ******************************* */

CPictPubApp PictPubApp;       
                              
BEGIN_MESSAGE_MAP(CPictPubApp,CWinApp)
	ON_COMMAND(IDC_SWITCHACTIVE_COLOR, OnSwitchActiveColor)
		ON_UPDATE_COMMAND_UI(IDC_SWITCHACTIVE_COLOR, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDC_REVERT_COLORSWATCH, OnRevertColorSwatch)
		ON_UPDATE_COMMAND_UI(IDC_REVERT_COLORSWATCH, OnUpdateAlwaysEnabled)

	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(IDM_PASTEFROMFILE, OnPasteFromFile)
		ON_UPDATE_COMMAND_UI(IDM_PASTEFROMFILE, OnUpdatePasteFromFile)
	ON_COMMAND(IDM_TOOLSHOW, OnToolShow)
		ON_UPDATE_COMMAND_UI(IDM_TOOLSHOW, OnUpdateToolShow)
	ON_COMMAND(IDM_ABOUT, OnAbout)
		ON_UPDATE_COMMAND_UI(IDM_ABOUT, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_PASTEASNEW, OnPasteAsNew)
		ON_UPDATE_COMMAND_UI(IDM_PASTEASNEW, OnUpdatePasteAsNew)
	ON_COMMAND(IDM_MACROPLAY, OnMacroPlay)
		ON_UPDATE_COMMAND_UI(IDM_MACROPLAY, OnUpdateMacroPlay)
	ON_COMMAND(IDM_MACROPLAYBATCH, OnMacroPlayBatch)
		ON_UPDATE_COMMAND_UI(IDM_MACROPLAYBATCH, OnUpdateMacroPlay)
	ON_COMMAND(IDM_MACRORECORD, OnMacroRecord)
		ON_UPDATE_COMMAND_UI(IDM_MACRORECORD, OnUpdateMacroPlay)
	ON_COMMAND(IDM_MACROSTOP, OnMacroStop)
		ON_UPDATE_COMMAND_UI(IDM_MACROSTOP, OnUpdateMacroStop)
	ON_COMMAND(IDM_MACRO_EDIT, OnMacroEdit)
		ON_UPDATE_COMMAND_UI(IDM_MACRO_EDIT, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_EXIT, OnExit)
		ON_UPDATE_COMMAND_UI(IDM_EXIT, OnUpdateExit)
	ON_COMMAND(ID_APP_EXIT, OnExit)
		ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateExit)
	ON_COMMAND(IDM_NEW, OnNew)
		ON_UPDATE_COMMAND_UI(IDM_NEW, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_OPEN, OnOpen)
		ON_UPDATE_COMMAND_UI(IDM_OPEN, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_REVERT, OnRevert)
		ON_UPDATE_COMMAND_UI(IDM_REVERT, OnUpdateRevert)
	ON_COMMAND(IDM_IMPORT, OnImport)
		ON_UPDATE_COMMAND_UI(IDM_IMPORT, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_SCANSETUP, OnScanSetup)
		ON_UPDATE_COMMAND_UI(IDM_SCANSETUP, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_MONITORMAP, OnMonitorMap)
		ON_UPDATE_COMMAND_UI(IDM_MONITORMAP, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_PRINTSETUP, OnPrintSetup)
		ON_UPDATE_COMMAND_UI(IDM_PRINTSETUP, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_CALIBRATESCAN, OnCalibrateScan)
		ON_UPDATE_COMMAND_UI(IDM_CALIBRATESCAN, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_CALIBRATEPRINT, OnCalibratePrint)
		ON_UPDATE_COMMAND_UI(IDM_CALIBRATEPRINT, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_SCRATCHPAD, OnScratchPad)
		ON_UPDATE_COMMAND_UI(IDM_SCRATCHPAD, OnUpdateScratchPad)
	ON_COMMAND(IDM_STATUSSHOW, OnStatusShow)
		ON_UPDATE_COMMAND_UI(IDM_STATUSSHOW, OnUpdateStatusShow)
	ON_COMMAND(IDM_TASKMANSHOW, OnTaskManShow)
		ON_UPDATE_COMMAND_UI(IDM_TASKMANSHOW, OnUpdateTaskManShow)
	ON_COMMAND(IDM_OBJ_MANAGER, OnObjManager)
		ON_UPDATE_COMMAND_UI(IDM_OBJ_MANAGER, OnUpdateObjManager)
	ON_COMMAND(ID_WINDOW_ARRANGE, OnArrangeIcons)
		ON_UPDATE_COMMAND_UI(ID_WINDOW_ARRANGE, OnUpdateNeedImage)
	ON_COMMAND(ID_WINDOW_CASCADE, OnCascade)
		ON_UPDATE_COMMAND_UI(ID_WINDOW_CASCADE, OnUpdateNeedImage)
	ON_COMMAND(ID_WINDOW_TILE_HORZ, OnTile)
		ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_HORZ, OnUpdateNeedImage)
	ON_COMMAND(IDM_BHINTSSHOW, OnBHintsShow)
		ON_UPDATE_COMMAND_UI(IDM_BHINTSSHOW, OnUpdateBHintsShow)
	ON_COMMAND(IDM_SHINTSSHOW, OnSHintsShow)
		ON_UPDATE_COMMAND_UI(IDM_SHINTSSHOW, OnUpdateSHintsShow)
	ON_COMMAND(IDM_WHINTSSHOW, OnWHintsShow)
		ON_UPDATE_COMMAND_UI(IDM_WHINTSSHOW, OnUpdateWHintsShow)
	ON_COMMAND(IDM_SHOWINFO, OnShowInfo)
		ON_UPDATE_COMMAND_UI(IDM_SHOWINFO, OnUpdateShowInfo)
	ON_COMMAND(IDM_SHOWZOOMBOX, OnShowZoomBox)
		ON_UPDATE_COMMAND_UI(IDM_SHOWZOOMBOX, OnUpdateShowZoomBox)
	ON_COMMAND(IDM_CLOSEALL, OnCloseAll)
		ON_UPDATE_COMMAND_UI(IDM_CLOSEALL, OnUpdateNeedImage)
	ON_COMMAND(IDM_SAVEPOSITIONS, OnSavePositions)
		ON_UPDATE_COMMAND_UI(IDM_SAVEPOSITIONS, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_SIZEUP, OnSizeUp)
		ON_UPDATE_COMMAND_UI(IDM_SIZEUP, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_SIZEDOWN, OnSizeDown)
		ON_UPDATE_COMMAND_UI(IDM_SIZEDOWN, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_SHAPEUP, OnShapeUp)
		ON_UPDATE_COMMAND_UI(IDM_SHAPEUP, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_SHAPEDOWN, OnShapeDown)
		ON_UPDATE_COMMAND_UI(IDM_SHAPEDOWN, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_PREF, OnPref)
		ON_UPDATE_COMMAND_UI(IDM_PREF, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_HELP_CONTEXTMODE, OnHelpContextMode)
		ON_UPDATE_COMMAND_UI(IDM_HELP_CONTEXTMODE, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_HELP_CONTEXT, OnHelpContext)
		ON_UPDATE_COMMAND_UI(IDM_HELP_CONTEXT, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_HELP_INDEX, OnHelpIndex)
		ON_UPDATE_COMMAND_UI(IDM_HELP_INDEX, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_HELP_USING, OnHelpUsing)
		ON_UPDATE_COMMAND_UI(IDM_HELP_USING, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_HELP_README, OnHelpReadMe)
		ON_UPDATE_COMMAND_UI(IDM_HELP_README, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDM_HELP_HELPONHELP, OnHelpOnHelp)
		ON_UPDATE_COMMAND_UI(IDM_HELP_HELPONHELP, OnUpdateAlwaysEnabled)
	ON_COMMAND(ID_WINDOW_NEW, OnNewWindow)
		ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW, OnUpdateNewWindow)
	ON_COMMAND(IDM_ACQUIRE, OnAcquire)
		ON_UPDATE_COMMAND_UI(IDM_ACQUIRE, OnUpdateAcquire)
	ON_COMMAND(IDM_SENDMAIL, OnSendMail)
		ON_UPDATE_COMMAND_UI(IDM_SENDMAIL,OnUpdateSendMail)


	ON_COMMAND(IDC_STATUSINFO, OnStatusInfo)
		ON_UPDATE_COMMAND_UI(IDC_STATUSINFO, OnUpdateNeedImage)
	ON_COMMAND(IDC_STATUSSHIELD, OnShield)
		ON_UPDATE_COMMAND_UI(IDC_STATUSSHIELD, OnUpdateAlwaysEnabled)
	ON_COMMAND(IDC_STATUSPALETTE, OnPalette)
		ON_UPDATE_COMMAND_UI(IDC_STATUSPALETTE, OnUpdatePalette)
	ON_COMMAND(IDC_STATUSMASK, OnStatusMask)
		ON_UPDATE_COMMAND_UI(IDC_STATUSMASK, OnUpdateNeedImage)
	ON_COMMAND(IDC_MASKEDITALPHA, OnEditAlpha)
		ON_UPDATE_COMMAND_UI(IDC_MASKEDITALPHA, OnUpdateNeedImage)
	ON_COMMAND(IDC_VIEWSLIME, OnViewSlime)
		ON_UPDATE_COMMAND_UI(IDC_VIEWSLIME, OnUpdateNeedImage)

	ON_UPDATE_COMMAND_UI(IDM_SAVE,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_SAVEAS,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_REVERT,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_CLOSE,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_PRINT,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_UNDO,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_APPLY,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_CMDLIST_UNDO,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_CMDLIST_REDO,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_CMDLIST_EDIT,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_CUT,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_COPY,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_COPYTOFILE,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_PASTE,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_PASTEFROMFILE,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_DELETE,OnUpdateNeedImage)
	ON_UPDATE_COMMAND_UI(IDM_RULERSHOW,OnUpdateNeedImage)
END_MESSAGE_MAP()


LPTSTR lpszAppClass = NULL;
LPTSTR lpszMDIChildClass = NULL;
LPTSTR lpszImageClass = NULL;

WINDOW Window;
APP Control;
VIEW View;
NAMES Names;
MAPPING Mapping;
COLORMASK ColorMask;
RETOUCH Retouch;
DRAW Draw;
VIGNETTE Vignette;
FILL Fill;
EDIT Edit;
PAGE Page;
STYLE PrintStyle;
HTSETTINGS Halftone;
SEPARATE Separate;
CALIBRATE Calibrate;
MASKS Mask;
STITCH Stitch;
LPTR LineBuffer[4];
LPBLT lpBltScreen;
HWND hZoomWindow;
CONVERT Convert;
SCANS Scans;
TEXTURE Texture;
QTONE Qtone;
BROWSER Browser;
EXTBROWSER ExtBrowser;
LPBRUSHSETTINGS lpBrushSettings;
int RetouchBrushCurrent;
TOOL Tool;
EPSOPTIONS EPSOptions;
TIFFOPTIONS TIFFOptions;
TGAOPTIONS TGAOptions;
JPEGOPTIONS JPEGOptions;
PPFFOPTIONS PPFFOptions;
PPFFOPENOPTIONS PPFFOpenOptions;
AVIOPTIONS AVIOptions;
TOOLSETTINGS Common;
MACRO Macro;
PALETTESETTINGS PaletteSettings;
HINTS Hints;
BOOL fShowPalette;
BOOL fShowRulers;
MAP Map1;
MAP Map2;
MAP Map3;
MAP Map4;
MAP Map5;
BYTE Step1[CALPOINTS];
BYTE Step2[CALPOINTS];
BYTE Step3[CALPOINTS];
BYTE Step4[CALPOINTS];
#ifdef WIN32
SYSTEM_INFO SystemInfo;
#endif

#define EXE_NAME_MAX_SIZE 128
#define COLOR_PATCH_CLASS 1
#define RIBBON_ICON_CLASS 2

/************************ Global Data ***********************************/

// The global handles 
HINSTANCE   hInstIMELib = NULL;		// handle used as flag for IME support    
HINSTANCE   hInstStringLib;
HINSTANCE   hInstHintLib;   
HINSTANCE   hInstIconLib = NULL;   
HACCEL  	hAccelTable;
BOOL    	fAppActive;
BOOL    	fCursorInActiveWindow;
RECT		rFrame;

TCHAR szAppName[MAX_STR_LEN];

/* *************************** Private Functions **************************** */

static BOOL NEAR PASCAL SetRegKey(LPCSTR lpszKey, LPCSTR lpszValue)
{
	if (::RegSetValue(HKEY_CLASSES_ROOT, lpszKey, REG_SZ,
		  lpszValue, lstrlen(lpszValue)) != ERROR_SUCCESS)
	{
		TRACE1(_T("Warning: registration database update failed for key '%Fs'\n"),
			lpszKey);
		return FALSE;
	}
	return TRUE;
}

static BOOL NEAR PASCAL SetRegExt(LPCSTR lpszFileExt, LPCSTR lpszProgId, 
	BOOL bAlwaysOverwrite = FALSE)
{
	BOOL ret = FALSE;
	TCHAR szBuff[80];
	long int cb = 79;
	HKEY hkey;

	// Purge any _T("PictPub.Image") attempts to get a file extension;  Replace these
	// with _T("PictPub.Image.5")
	if (!bAlwaysOverwrite)
	{
		// if extension is found and it has _T("PictPub.Image")
		// or _T("PictPub.Image.5") in it the delete it.
		if (RegOpenKey(HKEY_CLASSES_ROOT,lpszFileExt,&hkey) == ERROR_SUCCESS)
		{
			if ((RegQueryValue(hkey,NULL,szBuff,&cb) == ERROR_SUCCESS) &&
				((lstrcmpi(_T("PictPub.Image"),szBuff) == 0) ||
				 (lstrcmpi(_T("PictPub.Image.5"),szBuff) == 0)))
			{
			 	RegCloseKey(hkey);
				RegDeleteKey(HKEY_CLASSES_ROOT,lpszFileExt);
			}
			else
			 	RegCloseKey(hkey);
		}
	}

	if ((RegOpenKey(HKEY_CLASSES_ROOT,
		lpszFileExt,&hkey) != ERROR_SUCCESS) || bAlwaysOverwrite )
		ret = SetRegKey(lpszFileExt, lpszProgId);

	return ret;
}

static void RegDocIcon(CString progPath,CString keyDescription, 
	CString regkey, CString Iconindex)
{
	CString key,value;

	key = regkey;
	value = keyDescription;
	(void)SetRegKey(key,value);
	key = regkey + _T("\\DefaultIcon");
	value = progPath + _T(","); 
	value += Iconindex; 
	(void)SetRegKey(key,value);
	key = regkey + _T("\\shell\\open\\command");
	value = progPath + _T(" %1");
	(void)SetRegKey(key,value);
	key = regkey + _T("\\shell\\print\\command");
	value = progPath + _T(" /p %1");
	(void)SetRegKey(key,value);
}

#ifndef _DESIGNERFIX
void PatchRegDBFix(CString strFileTypeId)		
{
	CString strOld50ProgID = _T("PictPub.Image");
	CString strCurVer = strOld50ProgID + _T("\\CurVer");
	CString strCLSID = strOld50ProgID + _T("\\CLSID");
	HKEY hkey;
	HKEY hkey2;
	HKEY hkey3;
	TCHAR szBuffVersion[80];
	TCHAR szBuffCLSID[80];
	long int cb1 = 0;
	long int cb2 = 0;
	BOOL bOverwrite = TRUE;
	WORD wChar;
	// if _T("PictPub.Image") ProgID entry exists then purge it  -- 
	// Remember maintenance release for 5.0 now uses _T("PictPub.Image.5").
	if (RegOpenKey(HKEY_CLASSES_ROOT,strOld50ProgID,&hkey) == ERROR_SUCCESS)
	{
		// if CurVersion or CLSID information is present save it
		// (ie.) this case might occur if the original version of 
		// pp50 was re-run ( pre-maintenance rel )

		cb1 = 80;
		cb2 = 80;
		
		if (RegOpenKey(HKEY_CLASSES_ROOT, strCurVer,&hkey2) == ERROR_SUCCESS)
		{
			if (RegQueryValue(hkey2,NULL,szBuffVersion,&cb1) == ERROR_SUCCESS)
			{
				if (RegOpenKey(HKEY_CLASSES_ROOT,strCLSID,&hkey3) == ERROR_SUCCESS)
				{
					RegQueryValue(hkey3,NULL,szBuffCLSID,&cb2);
				 	RegCloseKey(hkey3);
					//--> Validate Version Number here !!
					short i = 0;
					float Version = (float)0.0;

					while (szBuffVersion[i])
					{							  
			   			if( IsDBCSLeadByte(( BYTE )*( szBuffVersion + i )))
						{
							wChar = *(( LPWORD )( szBuffVersion + i ));
							i += 2;
						}	
						else
							wChar = szBuffVersion[i++];

						if( wChar == _T('.'))
						{
							while (szBuffVersion[i])
							{
			   					if( IsDBCSLeadByte(( BYTE )*( szBuffVersion +i )))
								{
									wChar = *(( LPWORD )( szBuffVersion + i ));
									i += 2;
								}	
								else
									wChar = szBuffVersion[i++];

								if( wChar == _T('.'))
								{
									if (szBuffVersion[i])
										sscanf((LPCSTR)(szBuffVersion+i), 
										"%f", &Version);
									break;
								}
							}
							break;
						}
					}
					if (Version > 5.0)
						bOverwrite = FALSE;
				}
			 	RegCloseKey(hkey2);
			}
		}
		
		// delete the existing record
		RegDeleteKey(HKEY_CLASSES_ROOT,strOld50ProgID);
	 	RegCloseKey(hkey);
	}
		
	// write/rewrite the record with OUR Version info and CLSID 
	if (bOverwrite)
	{
		CString str;
		str = strOld50ProgID;
		(void)SetRegKey(strOld50ProgID,_T(""));
		str = strOld50ProgID;
		str += _T("\\CLSID");
		(void)SetRegKey(str,PP_GUIDID_STR);
		str = strOld50ProgID;
		str += _T("\\CurVer");
		(void)SetRegKey(str,strFileTypeId);
	}
	else
	{
		// rewrite the record with Origianl Version info and CLSID 
		// if available
		if (cb2 || cb1)
		{
			CString str;
			str = strOld50ProgID;
			(void)SetRegKey(strOld50ProgID,NULL);
			if (cb2)
			{
				str = strOld50ProgID;
				str += _T("\\CLSID");
				(void)SetRegKey(str,szBuffCLSID);
			}
			if (cb1)
			{
				str = strOld50ProgID;
				str += _T("\\CurVer");
				(void)SetRegKey(str,szBuffVersion);
			}
		}
	}
}
#endif //_DESIGNERFIX

static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LONG lParam)
{
  TCHAR szWndTitle[16];

  ::GetWindowText(hWnd, szWndTitle, 9);
  *MyCharNextN( szWndTitle, 9 ) = _T('\0');
  if (!lstrcmp((LPCSTR)lParam, szWndTitle))
  {
    SetActiveWindow(hWnd);
    return 0;
  }
  else
    return 1;
}

/* ***************************** Public Functions ************************* */
/***********************************************************************/
BOOL SetupCursor( HWND hWnd, UINT HitCode, ITEMID idWnd )
/***********************************************************************/
{
POINT CursorPos;
BOOL bHasHelp, bContextHelp;
WORD id;
int wClassExtra;
CServerView *pActiveView;
TCHAR szClassName[20];
HWND hFocus = GetFocus();

// set bubble hinting state
if( hFocus )
{
    GetClassName( hFocus, szClassName, sizeof( szClassName ));
    if( !StringsEqual( szClassName, _T("EDIT") ) && !StringsEqual( szClassName, _T("COMBOBOX") ) &&
        !LBUTTON && !MBUTTON && !RBUTTON )
    {
        if( Hints.fBubbleHintsOn && !HelpContext.BubbleHintsOn( HL_BUBBLE_QUERY ))     
        {
            HelpContext.BubbleHintsOn( YES );           // reset state
            HelpContext.SetHintState( _T("") );             // flush current hint
        }
    }
    else
    {
        if( Hints.fBubbleHintsOn && HelpContext.BubbleHintsOn( HL_BUBBLE_QUERY ))     
        {
            HelpContext.SetHintState( _T("") );             // flush current hint
            HelpContext.BubbleHintsOn( NO );            // reset state
        }
    }
}


pActiveView = PictPubApp.GetActiveView();
if ( !pActiveView || !pActiveView->GetSafeHwnd()  || hWnd == hZoomWindow)
    { // if its any old (non-image) window, use whatever cursor it wants...
    fCursorInActiveWindow = NO;
	DisplayInfo(INT_MIN, INT_MIN, NULL);
    DisplayBrush( 0, 0, 0, OFF );
    bContextHelp = InContextHelpMode();
    wClassExtra = GetClassExtra(hWnd);
    if ( wClassExtra == COLOR_PATCH_CLASS && !bContextHelp )
        return( FALSE ); // hint line handle in the control
    if ( GetWindowTask(PictPubApp.Get_hWndAstral()) != GetWindowTask(hWnd) )
        return( FALSE ); // If it's not our app, get out
    if ( hWnd == PictPubApp.Get_hWndAstral() )
        id = IDS_READY; // If it's the app itself, use _T("Ready") hint
    else
		{
		id = 0;
		if (StyleOn(hWnd, WS_CHILD))
			{
			id = GetDlgCtrlID(hWnd);
			if (id == -1) // special check for static controls without id's
				id = idWnd;
			}
    	if ( !id )  // If no id for the window
        	if ( !(id = idWnd) && !bContextHelp ) // and no id passed in,
            	return( FALSE );          // use the default cursor
		}
        // see if special ribbon icon class - a bit of a hack, but so what!!!
    if ( wClassExtra == RIBBON_ICON_CLASS ) // Do the hint line
    {
        HelpContext.OnHints( hWnd, id + IDC_RIBBONSTART );
        bHasHelp = HintLineSpecial( id, IDC_RIBBONSTART );
    }
    else
    {
        HelpContext.OnHints( hWnd, id );
        bHasHelp = HintLine( id );
    }
    if ( bContextHelp )
        { // if in _T("help") mode, use the proper help cursor...
        id = ( bHasHelp ? ID_APPHELP : ID_NOHELP );
        SetCursor( LoadCursor( PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(id) ) );
        return( TRUE ); // means we have taken care of cursor
        }
    return( FALSE ); // use the default cursor
    }

// It's an image window
if ( HitCode != HTCLIENT )
    {
    fCursorInActiveWindow = NO;
    DisplayBrush( 0, 0, 0, OFF );

    HelpContext.OnHints( hWnd, 0 );
    HintLine( 0 );

    return( FALSE ); // use the default cursor
    }

if ( !fAppActive || pActiveView->GetSafeHwnd() != hWnd )
    { // if the app is not active, or the window's inactive use the arrow...
    fCursorInActiveWindow = NO;
    DisplayBrush( 0, 0, 0, OFF );
    SetCursor( LoadCursor( NULL, IDC_ARROW ) );
    return( TRUE ); // means we have taken care of cursor
    }

// Its the active image...
GetCursorPos( &CursorPos );
ScreenToClient( hWnd, &CursorPos );
if ( pActiveView->m_lpDisplay != NULL)
	{
	if ( !IsOnImage(hWnd, CursorPos.x, CursorPos.y ) )
    	{ // if not on the image display area, use the arrow...
    	fCursorInActiveWindow = NO;
    	DisplayBrush( 0, 0, 0, OFF );
    	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
    	return( TRUE ); // means we have taken care of cursor
    	}
	}

fCursorInActiveWindow = YES;
// The active image is busy, so let progress stuff handle the cursor
#ifdef WIN32
if (Control.UseThreading)
    {
	CServerDoc *pDoc = pActiveView->GetDocument();
	if (pDoc)
		{
		CImage *pImage = pDoc->GetImage();
		LPCMDLIST lpCmdList = pDoc->GetCmdList();
    	if (lpCmdList && (!pImage || pImage->IsBusy()))
        	{
        	SetClockCursor(&lpCmdList->ThreadData.Progress, FALSE);
        	return(TRUE);
	        }
		}
    }
#endif

if ( !Tool.bActive )
{
    HelpContext.OnHints( hWnd, 0 );
    HintLineSpecial( Tool.id, IDC_IMAGESTART );
}
if ( Window.hCursor != Window.hNullCursor )
    DisplayBrush( 0, 0, 0, OFF );

return( FALSE ); // use the default cursor
}


/***********************************************************************/
//	BOOL fFastbits- allow fastbits to open a new fastbit
/***********************************************************************/
void CloseImage( BOOL fCloseAll, LPIMAGE lpCloseImage, HWND hWnd, BOOL fRegenrating /*FALSE*/ )
/***********************************************************************/
{
int idFileType;
FNAME szFileName;
LPFRAME lpFastbitsPreview;
RECT rFastbitsGrid;
BOOL fFastBits = FALSE;

// Destroy the image window
if (fCloseAll)
    { // Close all image windows
    POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
    while (TemplatePos)
        {
        CDocTemplate *pTemplate = PictPubApp.GetNextDocTemplate (TemplatePos);
        pTemplate->CloseAllDocuments(NO);
        }
    }
else
if ( lpCloseImage )
    { // Close all image windows related to a specific image pointer
	
	// dont do fastbits if regerating
	if (!fRegenrating)
	    fFastBits = (lpCloseImage->DocumentType == IMG_DOCPIECE);
    if (fFastBits)
        {
        idFileType = lpCloseImage->FileType;
        lstrcpy( szFileName, lpCloseImage->CurFile );
        lpFastbitsPreview = lpCloseImage->lpFastbitsPreview;
        rFastbitsGrid = lpCloseImage->rFastbitsGrid;
        lpCloseImage->lpFastbitsPreview = NULL;
        }

     CServerDoc *pDoc = PictPubApp.GetDocument(lpCloseImage);
     if (pDoc)
        pDoc->OnClosePPDocumentEx(fRegenrating);
    }
else
    { // Close only the active image window
    if (!hWnd) // in no window passed in, get the active window
     if ( !(hWnd = GetActiveDoc()) )
         return;
    lpCloseImage = (LPIMAGE)GetImagePtr ( hWnd );
	if (!fRegenrating)
    	fFastBits = (lpCloseImage->DocumentType == IMG_DOCPIECE) &&
         		       !AnotherView(lpCloseImage);
    if (fFastBits)
        {
        idFileType = lpCloseImage->FileType;
        lstrcpy( szFileName, lpCloseImage->CurFile );
        lpFastbitsPreview = lpCloseImage->lpFastbitsPreview;
        rFastbitsGrid = lpCloseImage->rFastbitsGrid;
        lpCloseImage->lpFastbitsPreview = NULL;
        }
    CFrameWnd *pFrameWnd = (CFrameWnd *)CWnd::FromHandle(hWnd);
    if (pFrameWnd)
        pFrameWnd->DestroyWindow();
    }
if (fFastBits)
    PictPubApp.OpenDocumentFileImmediate (szFileName,OM_FASTBITS);
UpdateStatusBar( YES, YES, YES, NO );
}

/************************************************************************/
BOOL AnotherView(LPIMAGE lpTheImage)
/************************************************************************/
{
return(NumImageWindows(lpTheImage) > 1);
}

/***********************************************************************/
LPTSTR GetUntitledName(LPTSTR szUntitled)
/***********************************************************************/
{
TCHAR num[10];
int iNum;

AstralStrEx( IDS_UNTITLED, szUntitled, MAX_FNAME_LEN );

#ifdef WIN32
if (Control.UseThreading)
	EnterCriticalSection(&Control.UntitledCriticalSection);
#endif

iNum = ++Control.UntitledNo;

#ifdef WIN32
if (Control.UseThreading)
	LeaveCriticalSection(&Control.UntitledCriticalSection);
#endif

_itoa( iNum, num, 10 );
lstrcat( (LPTSTR)szUntitled, (LPTSTR)num );
return( (LPTSTR)szUntitled );
}

/***********************************************************************/
BOOL HandleQueryNewPalette( HWND hWnd, BOOL fSpecialPal, BOOL fColorPal )
/***********************************************************************/
{
HPALETTE    hPal;
HDC     hDC;
LPIMAGE lpImage;
LPTR        lpPaletteLUT;
PALETTE_TYPE PaletteType;
LPPALETTEENTRY lpPalette;
int nEntries;


if (!lpBltScreen) // should never happen - not initialized yet
    return(FALSE);

lpImage = GetActiveImage();
if (fSpecialPal)
    // window needs a specific display palette which
    // may be different than palette for active image
    {
    if (fColorPal)
        {
        hPal = lpBltScreen->hColorPal;
        lpPaletteLUT = lpBltScreen->lpColorPaletteLUT;
        PaletteType = lpBltScreen->ColorPaletteType;
        lpPalette = lpBltScreen->ColorPalette;
        nEntries = lpBltScreen->nColorEntries;
        }
    else
        {
        hPal = lpBltScreen->hGrayPal;
        lpPaletteLUT = lpBltScreen->lpGrayPaletteLUT;
        PaletteType = lpBltScreen->GrayPaletteType;
        lpPalette = lpBltScreen->GrayPalette;
        nEntries = lpBltScreen->nGrayEntries;
        }
    }
else if (lpImage)
    // use active image palette if there is an active image
    {
    hPal = lpImage->hPal;
    lpPaletteLUT = lpImage->lpPaletteLUT;
    PaletteType = lpImage->PaletteType;
    lpPalette = lpImage->Palette;
    nEntries = lpBltScreen->nEntries;
    }
else if (lpBltScreen->hColorPal)
    // always use color display palette if no active image
    {
    hPal = lpBltScreen->hColorPal;
    lpPaletteLUT = lpBltScreen->lpColorPaletteLUT;
    PaletteType = lpBltScreen->ColorPaletteType;
    lpPalette = lpBltScreen->ColorPalette;
    nEntries = lpBltScreen->nColorEntries;
    }
else
    hPal = NULL;
SetNewPalette(lpBltScreen, PaletteType, hPal, lpPaletteLUT, lpPalette, nEntries,
				FALSE /* fApplyGamma */);
if (!hPal)
    return(FALSE);
hDC = GetDC(hWnd);
SuperBltSetupPalette(lpBltScreen, hDC, hPal, TRUE);
ReleaseDC(hWnd, hDC);
return(TRUE);
}


/***********************************************************************/
BOOL IsOurWindow( HWND hWnd )
/***********************************************************************/
{
#ifdef WIN32
DWORD dwWindowProcessID;

GetWindowThreadProcessId(hWnd, &dwWindowProcessID);
return(GetCurrentProcessId() == dwWindowProcessID);
#else
return(GetCurrentTask() == GetWindowTask(hWnd));
#endif
}


//************************************************************************
BOOL WackWindows( HINSTANCE hInst )
//************************************************************************
{
WNDENUMPROC  lpProc, lpProc1;

if ( !(lpProc = (WNDENUMPROC)MakeProcInstance( (FARPROC)CallBkProc, hInst )) )
    return( FALSE );
if ( !(lpProc1 = (WNDENUMPROC)MakeProcInstance( (FARPROC)CallBkProc1, hInst )) )
    return( FALSE );

EnumWindows( lpProc, ( LPARAM )lpProc1 );
FreeProcInstance( (FARPROC)lpProc );
FreeProcInstance( (FARPROC)lpProc1 );
return( TRUE );
}


//************************************************************************
BOOL CALLBACK CallBkProc( HWND hWnd, LPARAM lParam )
//************************************************************************
{
TCHAR  aClassName[128];
RECT  rect;

GetClassName( hWnd, aClassName, sizeof( aClassName ) );

if ( !lstrcmpi( aClassName, _T("MDICLIENT") ) )
    {
    GetClientRect( hWnd, &rect );
    InvalidateRect( hWnd, &rect, TRUE );
    }
else    EnumChildWindows( hWnd, ( WNDENUMPROC )lParam, 0 );
return( TRUE );
}


//************************************************************************
BOOL CALLBACK CallBkProc1( HWND hWnd, LPARAM lParam )
//************************************************************************
{
TCHAR  aClassName[128];
RECT  rect;

GetClassName( hWnd, aClassName, sizeof( aClassName ) );

if ( !lstrcmpi( aClassName, _T("MDICLIENT") ) )
    {
    GetClientRect( hWnd, &rect );
    InvalidateRect( hWnd, &rect, TRUE );
    }
return( TRUE );
}

/***********************************************************************/
void SetShowHideMenus( WORD idMenu, WORD idDialogBox, HMENU hMenu)
/***********************************************************************/
{
HWND hWnd;
BOOL fShow;

if (!hMenu)
   hMenu = ::GetMenu(PictPubApp.Get_hWndAstral());

if (!hMenu) // still don't have it!
   return;
	
if (idDialogBox == IDD_OBJFLOAT)
   hWnd =  ObjFloat.GetSafeHwnd();
else
   hWnd = AstralDlgGet(idDialogBox);

if (hWnd && ::IsWindowVisible( hWnd ) )
    fShow = NO;
else    fShow = YES;
ShowHideMenu( idMenu, fShow, hMenu);
}


/***********************************************************************/
void ShowHideMenu( WORD idMenu, BOOL fShow, HMENU hMenu)
/***********************************************************************/
{
LPTSTR lpString;
WORD idString;

switch( idMenu )
    {
    case IDM_TOOLSHOW:
    idString = IDS_SHOW_TOOLS;
    break;
    case IDM_STATUSSHOW:
    idString = IDS_SHOW_STATUS;
    break;
    case IDM_RULERSHOW:
    idString = IDS_SHOW_RULERS;
    break;
    case IDM_HIDEMARQUEE:
    idString = IDS_SHOW_MASK;
	break;
    case IDM_OBJ_HIDEMARQUEE:
    idString = IDS_SHOW_MARQUEE;
    break;
    case IDM_SHOWINFO:
    idString = IDS_SHOW_INFO;
    break;
    case IDM_OBJ_MANAGER:
    idString = IDS_SHOW_OBJ;
    break;
    case IDM_TASKMANSHOW:
    idString = IDS_SHOW_TASKMAN;
    break;
    case IDM_BHINTSSHOW:
    idString = IDS_SHOW_BHINT;
    break;
    case IDM_SHINTSSHOW:
    idString = IDS_SHOW_SHINT;
    break;
    case IDM_WHINTSSHOW:
    idString = IDS_SHOW_WHINT;
    break;
    case IDC_STATUSPALETTE:
    idString = IDS_SHOW_PALETTE;
    break;

    default:
    return;
    }

idString += (!fShow);
if ( !AstralStr( idString, &lpString ) )
    return;
ModifyMenu( hMenu, idMenu, MF_BYCOMMAND | MF_STRING, idMenu, lpString );
}


/***********************************************************************/
void OnUpdateSetShowHideMenus( WORD idMenu, WORD idDialogBox, CCmdUI* pCmdUI )
/***********************************************************************/
{
HWND hWnd;
BOOL fShow;

if (idDialogBox == IDD_OBJFLOAT)
   hWnd =  ObjFloat.GetSafeHwnd();
else
   hWnd = AstralDlgGet( idDialogBox );

if (hWnd && ::IsWindowVisible( hWnd ))
    fShow = NO;
else    
    fShow = YES;
OnUpdateShowHideMenu( idMenu, fShow, pCmdUI );
}

/***********************************************************************/
void OnUpdateShowHideMenu( WORD idMenu, BOOL fShow, CCmdUI* pCmdUI )
/***********************************************************************/
{
    LPTSTR lpString;
    WORD idString;

    switch( idMenu )
    {
        case IDM_TOOLSHOW:
        idString = IDS_SHOW_TOOLS;
        break;
        case IDM_STATUSSHOW:
        idString = IDS_SHOW_STATUS;
        break;
        case IDM_RULERSHOW:
        idString = IDS_SHOW_RULERS;
        break;
        case IDM_HIDEMARQUEE:
        idString = IDS_SHOW_MASK;
	    break;
        case IDM_OBJ_HIDEMARQUEE:
        idString = IDS_SHOW_MARQUEE;
        break;
        case IDM_SHOWINFO:
        idString = IDS_SHOW_INFO;
        break;
        case IDM_OBJ_MANAGER:
        idString = IDS_SHOW_OBJ;
        break;
        case IDM_TASKMANSHOW:
        idString = IDS_SHOW_TASKMAN;
        break;
        case IDM_BHINTSSHOW:
        idString = IDS_SHOW_BHINT;
        break;
        case IDM_SHINTSSHOW:
        idString = IDS_SHOW_SHINT;
        break;
        case IDM_WHINTSSHOW:
        idString = IDS_SHOW_WHINT;
        break;
        case IDC_STATUSPALETTE:
        idString = IDS_SHOW_PALETTE;
        break;

        default:
        return;
    }

    idString += ( !fShow );
    if( !AstralStr( idString, &lpString ))
        return;
    pCmdUI->SetText( lpString );
}


//************************************************************************
HMENU AstralGetSubMenu( HMENU hMenu, int nPos )
//************************************************************************
{
    // adjust for no object menu
    if( !Control.Retail )
    {
        if( nPos > IDM_OBJECTMENU )
            --nPos;
    }

    HWND hMDIWindow = NULL;
    HWND hView = GetActiveDoc();
    if( hView )
    {
	    CWnd *pWnd = CWnd::FromHandle( hView );
	    ASSERT(pWnd && pWnd->IsKindOf( RUNTIME_CLASS( CServerView )));
	    hMDIWindow = pWnd->GetParentFrame()->GetSafeHwnd();
    }

    // adjust for mdi cookie
    if( hMDIWindow && IsZoomed( hMDIWindow ))
        ++nPos;

    return( GetSubMenu( hMenu, nPos ));
}

/***********************************************************************/
// CPPOleTemplateServer methods
/***********************************************************************/

CCmdTarget* CPPOleTemplateServer::OnCreateObject()
{
	if (!m_bResourcesLoaded)
	{
		// Load All the resources that typically are loaded at startuptime in
		// the default MFC SetServerInfo method off CDocTemplate 

		ASSERT(m_pDocTemplate);
		CPPMultiDocTemplate* pDocTemplate = (CPPMultiDocTemplate*)m_pDocTemplate;
		UINT nIDOleEmbedding = pDocTemplate->m_nIDOleEmbedding; 
		UINT nIDOleInPlaceServer = pDocTemplate->m_nIDOleInPlaceServer;

		if (nIDOleEmbedding != 0)
		{
			// load menu to be used while editing an embedding (as a server)
			HINSTANCE hInst = AfxGetResourceHandle();
			hInst  = (::FindResource(hInst, 
				MAKEINTRESOURCE(nIDOleEmbedding), RT_MENU)) ? 
					hInst : NULL;

			if (hInst)
			{
				pDocTemplate->m_hMenuEmbedding = 
					::LoadMenu(hInst, MAKEINTRESOURCE(nIDOleEmbedding));
				pDocTemplate->m_hAccelEmbedding = 
					::LoadAccelerators(hInst, MAKEINTRESOURCE(nIDOleEmbedding));
			}
		}
	
		if (nIDOleInPlaceServer != 0)
		{
			// load menu to be used while editing in-place (as a server)
			HINSTANCE hInst = AfxGetResourceHandle();
			hInst  = (::FindResource(hInst, 
				MAKEINTRESOURCE(nIDOleInPlaceServer), RT_MENU)) ? 
					hInst : NULL;

			if (hInst)
			{
				pDocTemplate->m_hMenuInPlaceServer = ::LoadMenu(hInst,
					MAKEINTRESOURCE(nIDOleInPlaceServer));
				pDocTemplate->m_hAccelInPlaceServer = ::LoadAccelerators(hInst,
					MAKEINTRESOURCE(nIDOleInPlaceServer));
			}
		}

		m_bResourcesLoaded = TRUE;
	}

	return COleTemplateServer::OnCreateObject();
}

/***********************************************************************/
// CPictPubApp Methods
/***********************************************************************/

CPictPubApp::CPictPubApp ()
{
	bPPAppFileIo = FALSE; 
	m_pActiveView = NULL;
	m_pClipboardFileList = new CPtrList();
	m_pMail= NULL;
	m_dwMainThreadId = GetCurrentThreadId();
#ifdef _FASTINIT
    m_pInitThread = NULL; 
    m_bCustoolsLoaded = FALSE;
#endif //_FASTINIT
	m_bInPlaceActive = FALSE;
	ASSERT(m_pClipboardFileList);
}  

CPictPubApp::~CPictPubApp ()
{
    m_bMailInitDone = FALSE;

#ifdef _FASTINIT
    if (m_pInitThread)
        delete m_pInitThread;
#endif //_FASTINIT

	if (m_pClipboardFileList)
	{
		CLIPBOARDFILE *pClipboardFile;
		POSITION pos = m_pClipboardFileList->GetHeadPosition();
		while (pos)
		{
			pClipboardFile = (CLIPBOARDFILE*)m_pClipboardFileList->GetNext(pos);		
			if (pClipboardFile)
			{
				if (!pClipboardFile->nReferenceCount)
					FileDelete(pClipboardFile->szFileName);
				delete pClipboardFile;
			}
		}
		delete m_pClipboardFileList;
	}
}


BOOL CPictPubApp::OnIdle(
long lCount)
{
	LPIMAGE lpImage;

#ifdef _FASTINIT
    if ( !m_bCustoolsLoaded)
    {
        if ( m_pInitThread && m_pInitThread->HintLoadComplete(FALSE))
        {
            PictPubApp.GetCustomBoxes()->Load((CFrameWnd*)m_pMainWnd);

            if( Hints.fStatusHintsOn )
                MessageStatus( IDS_WELCOME, (LPTSTR)szAppName );

            m_bCustoolsLoaded = TRUE;
        }
    }
#endif // _FASTINIT

	BOOL bMore = CWinApp::OnIdle(lCount);
	if (bMore)
		return(bMore);

	// see if we need to display a marquee for this image
	// the active image could be in-place in another app
	if (Control.WinVer != WV_WIN32S)
	{
		DWORD result;

		result = MsgWaitForMultipleObjects(0, NULL, FALSE, 20, QS_ALLINPUT);
		lpImage = GetActiveImage();
		if (lpImage && ImgDisplayMarquees(lpImage))
		{
			if (result == WAIT_TIMEOUT)
				Marquee(NO);
			bMore = TRUE;
		}
	}
	else
	{
		if (lpImage && ImgDisplayMarquees(lpImage))
		{
			static long lLastTickCount;
			long lTickCount;
	
			lpImage = GetActiveImage();

			// Get the document so we can determine if it's in-place
			// We draw marquee if we are in-place or the app is active
	    	CServerDoc *pDoc = PictPubApp.GetDocument(lpImage);
			if (pDoc &&	(fAppActive || pDoc->IsInPlaceActive()) )
			{
				lTickCount = GetTickCount();
				if ((lTickCount-lLastTickCount) > 20)
				{
					lLastTickCount = lTickCount;
					Marquee(NO);
				}
				bMore = TRUE;
			}
		}
	}
	return(bMore);
}

void CPictPubApp::DocumentChanged (
LPIMAGE lpImage /* NULL */)
// Set the Modified Flag and for the Document that contains the lpImage.
// If lpImage is NULL  then the method will use the current Active Window    
{       
    CMDIChildWnd *pActiveWnd = ((CMDIFrameWnd*)PictPubApp.m_pMainWnd)->MDIGetActive();  
    if (lpImage == NULL)
    {
        if (pActiveWnd) 
        {
            CServerDoc *pDoc = (CServerDoc *)pActiveWnd->GetActiveDocument(); 
            if (pDoc && pDoc->m_lpImage)
            {
				// TED - the following line is commented out so that we
 				// can always know from an lpImage whether the image
				// has been modified, set it to true just in case
                //pDoc->m_lpImage->fChanged = FALSE;
				pDoc->m_lpImage->fChanged = TRUE;
                pDoc->SetModifiedFlag(TRUE);
            } 
        }        
    }    
    else 
    if (lpImage->fChanged)
    {       
        CServerDoc *pDoc;
        POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
        while (TemplatePos)
        {
            CDocTemplate *pTemplate = PictPubApp.GetNextDocTemplate (TemplatePos);
            POSITION DocPos = pTemplate->GetFirstDocPosition();
            while (DocPos)
            {
                pDoc = (CServerDoc*)pTemplate->GetNextDoc (DocPos);
                if (pDoc && pDoc->m_lpImage && (pDoc->m_lpImage == lpImage))
                {
					// TED - the following line is commented out so that we
 					// can always know from an lpImage whether the image
					// has been modified
                    //pDoc->m_lpImage->fChanged = FALSE;
                    pDoc->SetModifiedFlag(TRUE);            
                }
            }
        }
    }
}    

POSITION CPictPubApp::GetFirstDocTemplate ()
{
    return (PictPubApp.m_templateList.GetHeadPosition());
}

CDocTemplate * CPictPubApp::GetNextDocTemplate (POSITION &pos)
{
    ASSERT(pos != NULL);
    return ((CDocTemplate*)PictPubApp.m_templateList.GetNext(pos));
}

CServerDoc * CPictPubApp::GetDocument (LPIMAGE lpImage)
{
    //  NOTE!!  lpImage should now contain a pointer to the Document that
    // contains it and it is much more efficient thatn this method
    
    // make sure we have an image coming in
    if (!lpImage)
        return(NULL);

    if (lpImage->pServerDoc != NULL)
        return ((CServerDoc*)lpImage->pServerDoc);      

    // lpImage's pointer back to it's containing ServerDoc is now yet initialized
    POSITION TemplatePos = PictPubApp.GetFirstDocTemplate ();
    while (TemplatePos)
    {
        CDocTemplate *pTemplate = PictPubApp.GetNextDocTemplate (TemplatePos);    
        POSITION DocPos = pTemplate->GetFirstDocPosition();
        while (DocPos)
        {
            CServerDoc *pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
            if (pDoc)
            {
                if (pDoc->m_lpImage == lpImage)
                    return(pDoc);
            }
        }
    }
    // never found one???
    return(NULL);
}

CServerDoc * CPictPubApp::GetDocument (LPTSTR lpFileName)
{
    // make sure we have a file name coming in
    if (!lpFileName)
        return(NULL);

    POSITION TemplatePos = PictPubApp.GetFirstDocTemplate ();
    while (TemplatePos)
    {
        CDocTemplate *pTemplate = PictPubApp.GetNextDocTemplate (TemplatePos);    
        POSITION DocPos = pTemplate->GetFirstDocPosition();
        while (DocPos)
        {
            CServerDoc *pDoc = (CServerDoc *)pTemplate->GetNextDoc (DocPos);    
            if (pDoc && pDoc->m_lpImage)
            {
                if (StringsEqual(pDoc->m_lpImage->CurFile, lpFileName))
                    return(pDoc);
            }
        }
    }
    // never found one???
    return(NULL);
}

ATOM CPictPubApp::MyRegisterClass( CONST WNDCLASS *lpwc, UINT IconSmID /* 0 */)
{
    HANDLE hMod = GetModuleHandle(_T("USER32"));
    if (hMod != NULL)  // 32 bit BUT is it Chicago API?
    {
          typedef ATOM (FAR WINAPI * REGCLASSPROC)(CONST WNDCLASSEX *);
          REGCLASSPROC proc;
#if defined (UNICODE)
        proc = 
           (REGCLASSPROC)GetProcAddress((struct HINSTANCE__*)hMod,
                _T("RegisterClassExW"));
#else
        proc = 
            (REGCLASSPROC)GetProcAddress((struct HINSTANCE__*)hMod,
                _T("RegisterClassExA"));
#endif
        if (proc != NULL)
        {
            ATOM retAtom;
            WNDCLASSEX wcex;
            HINSTANCE hSmIconRes = Get_hInstAstral();

            wcex.cbSize = sizeof (WNDCLASSEX);
            wcex.style = lpwc->style ;
            wcex.lpfnWndProc = lpwc->lpfnWndProc ;
            wcex.cbClsExtra = lpwc->cbClsExtra ;
            wcex.cbWndExtra = lpwc->cbWndExtra ;
            wcex.hInstance = lpwc->hInstance ;
            wcex.hIcon = lpwc->hIcon ;
            wcex.hCursor = lpwc->hCursor ;
            wcex.hbrBackground = lpwc->hbrBackground ;
            wcex.lpszMenuName = lpwc->lpszMenuName ;
            wcex.lpszClassName = lpwc->lpszClassName ;
            if (IconSmID == 0)
				IconSmID = IDC_PP16X16ICON;
            wcex.hIconSm = ::LoadIcon(hSmIconRes, MAKEINTRESOURCE(IconSmID));
            if (retAtom = (*proc) ((CONST WNDCLASSEX *)&wcex))	// might be merely stubbed out if fails
                return (retAtom);
        }
    }
    return (RegisterClass(lpwc));
}

BOOL CPictPubApp::AstralInit( )
{
    ASSERT(hInstIconLib != NULL); // needs to be loaded by now;
	WNDCLASS WndClass;
	lpszAppClass = ID_APPCLASS;
	WndClass.lpszClassName = lpszAppClass;
	WndClass.hIcon		   = ::LoadIcon( hInstIconLib, MAKEINTRESOURCE(IDC_ABOUTAPP));
	WndClass.hbrBackground =  (Window.hBGBrush ? Window.hBGBrush : (HBRUSH)(COLOR_APPWORKSPACE+1) );
	WndClass.lpszMenuName  = NULL;
	WndClass.hCursor       = ::LoadCursor( NULL, IDC_ARROW );
	WndClass.hInstance	   = ::AfxGetInstanceHandle();
	WndClass.style		   = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	WndClass.lpfnWndProc   = AfxWndProc;
	WndClass.cbClsExtra	   = 0;
	WndClass.cbWndExtra	   = 0;
	if (!MyRegisterClass( &WndClass,0))
		return (FALSE);

	// if exists a previous instance of PP then activate it.
	if (PreviousInstance())
		return FALSE;

	/* Initialize Ole Libraries	*/
    if( Control.Retail )
    {
        if (!AfxOleInit())
        {                   
            AfxMessageBox(IDS_E_OLEINITFAILED);
            return FALSE;
        }
    }

    lpszMDIChildClass = _T("PP50MDICHILD");
    WndClass.lpszClassName = lpszMDIChildClass;
    WndClass.hIcon		   = NULL;
    WndClass.hbrBackground =  (Window.hBGBrush ? Window.hBGBrush : (HBRUSH)(COLOR_APPWORKSPACE+1) );
    WndClass.lpszMenuName  = NULL;
    WndClass.hCursor       = NULL;
    WndClass.hInstance	   = ::AfxGetInstanceHandle();
    WndClass.style		   = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
    WndClass.lpfnWndProc   = AfxWndProc;
    WndClass.cbClsExtra	   = 0;
    WndClass.cbWndExtra	   = 0;
    if (!MyRegisterClass( &WndClass, IDC_DOC16X16ICON ))
    	return (FALSE);

    RegisterAllClasses( GetResourceHandle() );

    // ------------------------------------------------
    // Create, Add and Register All Document templates
    // ------------------------------------------------
    // ========== 1) PP Format ===========
    CPPMultiDocTemplate* pDocTemplate;
    pDocTemplate = new CPPMultiDocTemplate(IDR_PPSVRTYPE_PP,
            RUNTIME_CLASS(CServerDoc),
            RUNTIME_CLASS(CPPMDIChildWnd),    
            RUNTIME_CLASS(CServerView));
	pDocTemplate->OurSetServerInfo(IDR_PPSVRTYPE_SRVR_EMB,IDR_PPSVRTYPE_SRVR_IP,
	 	RUNTIME_CLASS(CInPlaceFrame));
    AddDocTemplate(pDocTemplate);                          
    if( Control.Retail )
    {
        m_PPserver.ConnectTemplate(OLE_CLSID_PP, pDocTemplate, FALSE);
        COleTemplateServer::RegisterAll();
    }

    return( TRUE );
}

void CPictPubApp::CloseDocumentFile(
LPIMAGE lpImage)
{   
    ASSERT(lpImage);   
    
    //Locate the document that owns the lpImage and set it as modified
    CServerDoc *pDoc = GetDocument (lpImage);
    if (pDoc)
        pDoc->CloseDocumentFile();
}
    
CDocument* CPictPubApp::NewDocumentFile( 
LPIMAGE lpImage)
{                      
    ASSERT(lpImage);
    return (this->OpenDocumentFile(lpImage->CurFile,lpImage));    
} 
       
CDocument* CPictPubApp::OpenDocumentFile( 
// Routine to Open a Document File using the correct type of Document Template.   
// This match is made by comparing the FileName extension against the list
// of registered templates and their file extensions.
//
// NOTE :: This routine assumes that lpszFileName is a fully qualified filename
LPCSTR lpszFileName,
LPIMAGE lpImage,
BOOL bMakeVisible)
{        
	FNAME szFileName;
              
    ASSERT(lpImage);   

	lstrcpy(szFileName, lpszFileName);
	if (lpImage->DocumentType == IMG_DOCPIECE)
	{
		// setup title
		lstrcpy(szFileName,_T("FastBits"));
		if (lpImage->fUntitled)
		{
			// already untitled
			lstrcat(szFileName,lpImage->CurFile);
			lstrcpy(lpImage->CurFile, lpImage->ImageName);
		}
		else // first version of this image
		{
			lpImage->fUntitled = YES;
			GetUntitledName(szFileName);
		}
	}

    // find the highest confidence
    POSITION pos = m_templateList.GetHeadPosition();
    CPPMultiDocTemplate::Confidence bestMatch = CPPMultiDocTemplate::noAttempt;
    CPPMultiDocTemplate* pBestTemplate = NULL;
    CDocument* pOpenDocument = NULL;

    while (pos != NULL)
    {
        CPPMultiDocTemplate* pTemplate = 
            (CPPMultiDocTemplate*)m_templateList.GetNext(pos);
        ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));

        CDocTemplate::Confidence match;
        ASSERT(pOpenDocument == NULL);
        match = pTemplate->MatchDocType(szFileName, pOpenDocument);
        if (match > bestMatch)
        {
            bestMatch = match;
            pBestTemplate = pTemplate;
        }
        if (match == CDocTemplate::yesAlreadyOpen)
            break;      // stop here
    }

    if (pOpenDocument != NULL)
    {
        POSITION pos = pOpenDocument->GetFirstViewPosition();
        if (pos != NULL)
        {
            CView* pView = pOpenDocument->GetNextView(pos); // get first one
            ASSERT_VALID(pView);
            CFrameWnd* pFrame = pView->GetParentFrame();
            if (pFrame != NULL)
                pFrame->ActivateFrame();
            else
                TRACE0(_T("Error: Can not find a frame for document to activate"));
            CFrameWnd* pAppFrame;
            if (pFrame != (pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd))
            {
                ASSERT(pAppFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)));
                pAppFrame->ActivateFrame();
            }
        }
        else
        {
            TRACE0(_T("Error: Can not find a view for document to activate"));
        }
        return pOpenDocument;
            // file already open (even if we can't activate it)
    }

    if (pBestTemplate == NULL)
    {
        TRACE0(_T("Error: can't open document\n"));
        return FALSE;
    }


    bPPAppFileIo = TRUE; 
    pOpenDocument = pBestTemplate->OpenDocumentFile(lpImage,szFileName,bMakeVisible);
    bPPAppFileIo = FALSE; 

    return (pOpenDocument);    
} 

    
CDocument* CPictPubApp::OpenDocumentFile( 
// Routine to Open a Document File using the correct type of Document Template.   
// This match is made by comparing the FileName extension against the list
// of registered templates and their file extensions.
//
// NOTE :: This routine assumes that lpszFileName is a fully qualified filename
LPCSTR lpszFileName)
{
	FNAME szFileName;

	lstrcpy(szFileName, lpszFileName);
	return(AstralImageLoadBG(GetPPFileType(szFileName), szFileName, TRUE, NULL));
}

CDocument* CPictPubApp::OpenDocumentFile( 
// Routine to Open a Document File using the correct type of Document Template.   
// This match is made by comparing the FileName extension against the list
// of registered templates and their file extensions.
//
// NOTE :: This routine assumes that lpszFileName is a fully qualified filename
LPCSTR lpszFileName,
OPENINFO *pOpenInfo,
BOOL bMakeVisible)
{                      
    ASSERT(pOpenInfo != NULL);   

    // find the highest confidence
    POSITION pos = m_templateList.GetHeadPosition();
    CPPMultiDocTemplate::Confidence bestMatch = CPPMultiDocTemplate::noAttempt;
    CPPMultiDocTemplate* pBestTemplate = NULL;
    CDocument* pOpenDocument = NULL;

    while (pos != NULL)
    {
        CPPMultiDocTemplate* pTemplate = 
            (CPPMultiDocTemplate*)m_templateList.GetNext(pos);
        ASSERT(pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));

        CDocTemplate::Confidence match;
        ASSERT(pOpenDocument == NULL);
        match = pTemplate->MatchDocType(lpszFileName, pOpenDocument);
        if (match > bestMatch)
        {
            bestMatch = match;
            pBestTemplate = pTemplate;
        }
        if (match == CDocTemplate::yesAlreadyOpen)
            break;      // stop here
    }

    if (pOpenDocument != NULL)
    {
        POSITION pos = pOpenDocument->GetFirstViewPosition();
        if (pos != NULL)
        {
            CView* pView = pOpenDocument->GetNextView(pos); // get first one
            ASSERT_VALID(pView);
            CFrameWnd* pFrame = pView->GetParentFrame();
            if (pFrame != NULL)
                pFrame->ActivateFrame();
            else
                TRACE0(_T("Error: Can not find a frame for document to activate"));
            CFrameWnd* pAppFrame;
            if (pFrame != (pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd))
            {
                ASSERT(pAppFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)));
                pAppFrame->ActivateFrame();
            }
        }
        else
        {
            TRACE0(_T("Error: Can not find a view for document to activate"));
        }
        return pOpenDocument;
            // file already open (even if we can't activate it)
    }

    if (pBestTemplate == NULL)
    {
        TRACE0(_T("Error: can't open document\n"));
        return FALSE;
    }


    bPPAppFileIo = TRUE; 
    pOpenDocument = pBestTemplate->OpenDocumentFile(lpszFileName,pOpenInfo,bMakeVisible);
    bPPAppFileIo = FALSE; 

    return (pOpenDocument);    
} 
    
CDocument * CPictPubApp::OpenDocumentFileImmediate (
LPCSTR lpszFileName,
OPEN_MODE OpenMode,
LPCMSINFO pCmsInfo)
{
    LPIMAGE lpImage = NULL;

    switch (OpenMode)
    {
    case OM_NORMAL:
        {
        CReadImage read((LPTSTR)lpszFileName);
        lpImage = read.ReadImage();
        }    
        break;    
    case OM_FASTBITS:
        {     
        DoFastbitsLoad( 0/*FileType*/, (LPTSTR)lpszFileName, pCmsInfo, NULL, NULL, YES);
        }
        break;
    case OM_LOWRES:                                
    default :
        ASSERT(FALSE);  // not implemented
        break;                
    };   

    if (lpImage)        
        return (OpenDocumentFile(lpszFileName,lpImage)); 
    else
        return (FALSE);
}

BOOL CPictPubApp::ActivatePreviousInstance()
{
  return !::EnumWindows(EnumWindowsProc, (LONG)(LPCSTR)AfxGetAppName());
}

BOOL CPictPubApp::PreviousInstance()
{
	if (!::FindWindow(lpszAppClass,NULL))
		return FALSE;

	if (!ActivatePreviousInstance())
	{
		AfxMessageBox(IDS_EALREADYRUNNING);
		return TRUE;
	}

	return FALSE;
}

BOOL CPictPubApp::InitInstance ()
{
    WORD wHeight, wWidth;
    HWND hDlg = NULL;
    FNAME szFileName;
    int x, y, dx, dy;
    HDC hDC;
    DWORD dwVersion;
    BOOL bPrint = FALSE;
	BOOL bRunningEmbedded = FALSE;
	BOOL bRunningAutomated = FALSE;
	HINSTANCE hResDll;
	CPINFO	CPInfo;		//	Windows-defined structure for code page info
	BOOL fDBCS;


	Enable3dControls();
	InitCommonControls();

    /* Get the path of the file being executed (needed by preferences) */
    GetModuleFileName( m_hInstance, Control.ProgHome, sizeof(Control.ProgHome)-1);
    stripfile( Control.ProgHome );
    FixPath( Control.ProgHome );

    /************************************************************************/
    /* Load Resource DLL                         							*/
    /************************************************************************/
	lstrcpy(szFileName, Control.ProgHome);
	lstrcat(szFileName, _T("ppres.dll"));
    if ( !( hResDll = (HINSTANCE)AstralLoadLibrary( szFileName ) ) )
        return( FALSE );

	// tell MFC about our resources
	AfxSetResourceHandle(hResDll);

    /************************************************************************/
    /* Load String Library                      							*/
    /************************************************************************/
	lstrcpy(szFileName, Control.ProgHome);
	lstrcat(szFileName, _T("string.dll"));
    if ( !( hInstStringLib = (HINSTANCE)AstralLoadLibrary( szFileName ) ) )
        return( FALSE );

#ifndef _FASTINIT				   
    /************************************************************************/
    /* Load Hint Library                      								*/
    /************************************************************************/
	lstrcpy(szFileName, Control.ProgHome);
	lstrcat(szFileName, _T("hint.dll"));
    if( !( hInstHintLib = (HINSTANCE)AstralLoadLibrary( szFileName )))
        return( FALSE );
#endif // _FASTINIT				   

	/************************************************************************/
	/* Load and Install the Application name into the Foundation			*/
	/* NOTE: Normally this could be done in the CPictPubApp Constructor,	*/
	/* however since we store our strings in a separate DLL, we must wait   */
	/* now.																	*/
	/************************************************************************/
    AstralStrEx(IDS_APPNAME, szAppName, MAX_STR_LEN);
 	this->m_pszAppName = (LPCSTR)&szAppName;

	// must be done after
	//m_pszProfileName = SetupPreferencePath();

	// uncomment the line below to use registry for prefs
	SetRegistryKey(_T("Micrografx"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	// Check current code page to determine if DBCS enabled
	if( GetCPInfo( CP_ACP, &CPInfo ))
		fDBCS = ( CPInfo.MaxCharSize > 1 );
	else
		fDBCS =FALSE;

    Window.iLabelFont = GetDefaultInt( _T("LabelSet"), fDBCS ? SYSTEM_FONT:ANSI_VAR_FONT );

    /************************************************************************/
    /* Load IME Library if running on double-byte windows.   				*/
	/* If not found, don't use it and keep going							*/
    /************************************************************************/
	if( fDBCS )
	{
		int ErrorMode = SetErrorMode( SEM_NOOPENFILEERRORBOX );
		hInstIMELib = ( HINSTANCE )AstralLoadLibrary( _T("WINNLS.DLL") );
		SetErrorMode( ErrorMode );
	}

    Control.Crippled = NO;

    // initialize bubble help
    HelpContext.InitBubbleHints();      // register window

    #ifdef WIN32
    LPTHREADDATA lpThreadData;
    STARTUPINFO StartupInfo;
    
    GetSystemInfo(&SystemInfo);
    GetStartupInfo(&StartupInfo);
//CALFIX    if (cmdShow == SW_SHOWDEFAULT)
//CALFIX        cmdShow = StartupInfo.wShowWindow;
//CALFIX    if (lpszCmdLine && lStrChr(lpszCmdLine, _T(' ')))
//CALFIX        lpszCmdLine = lStrChr(lpszCmdLine, _T(' ')) + 1;
    #endif
    dwVersion = GetVersion();
	if (dwVersion < 0x80000000L)
		Control.WinVer = WV_NT;
	else
    if (LOBYTE(LOWORD(dwVersion)) < 4)
		Control.WinVer = WV_WIN32S;
	else
        Control.WinVer = WV_WINDOWS95;
	Control.fNewLook = Control.WinVer == WV_WINDOWS95;
    
    /* Load the global data structures with memory related info */
    LoadAllPreferences(YES);
    
	lstrcpy(szFileName, Control.ProgHome);
	if (Control.UseGrayScaleIcons)
		lstrcat(szFileName, _T("gico.dll"));
	else
		lstrcat(szFileName, _T("cico.dll"));

    if( !( hInstIconLib = (HINSTANCE)AstralLoadLibrary( szFileName )))
   	    return( FALSE );

    /************************************************************************/
    /* Initialization for all instances                                     */
    /************************************************************************/
    // set this flag before calling AstralInit
#ifndef OEM
    Control.Retail = TRUE;
#else
    Control.Retail = FALSE;     // LE version
#endif

    if ( !AstralInit() )
        return( FALSE );

    bRunningEmbedded = RunEmbedded();
	bRunningAutomated = RunAutomated();

    GetDefInt( Control.ScreenColor, NO );
    GetDefRGB( Control.rgbBackground, _T("0,0,0") );
    if ( Control.ScreenColor )
    {
        Window.hBGBrush = CreateSolidBrush( RGB(
            Control.rgbBackground.red,
            Control.rgbBackground.green,
            Control.rgbBackground.blue ) );
    }

    /* Load strings from the string table */
    
    /* Get the handles to the keyboard accelerators */
    hAccelTable = LoadAccelerators( PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(IDR_PPMAIN) );
    
    /* Size the application window based on the screen size */
    wWidth = GetSystemMetrics( SM_CXSCREEN );
    wHeight = GetSystemMetrics( SM_CYSCREEN );
    
    GetDefaultString( _T("xyApp"), _T("0,0,0,0"), szFileName, sizeof(szFileName) );
    AsciiToInt4( szFileName, &x, &y, &dx, &dy );
    if ( !(x + y+ dx + dy ) )
    {
        x = y = 1;
        dx = wWidth-2;
        dy = wHeight-2;
    }
    x = bound( x, 0, wWidth-32 );
    y = bound( y, 0, wHeight-32 );
    dx = bound( dx, 32, wWidth-x );
    dy = bound( dy, 32, wHeight-y );
    rFrame.left = x; 
	rFrame.top = y;
    rFrame.right = x + dx; 
	rFrame.bottom = y + dy;
    
    /* Create the main application window */
    CMDIFrame * pMainFrame = new CMDIFrame ;
    if ( !(pMainFrame->Create(
        lpszAppClass,                           /** lpClassName **/
        szAppName,                              /** lpWindowName **/
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | FWS_ADDTOTITLE,  /** dwStyle **/
        (RECT)rFrame,                           /** Initial pos and size of window **/
        NULL,                                   /** Parent Window Handle **/
        MAKEINTRESOURCE(IDR_PPMAIN)             /** Menu String **/
        )) )
    {
        return( FALSE );
    }

	// save menu to use when no active MDI child window is present
	ASSERT(pMainFrame->m_hWnd != NULL);
	pMainFrame->m_hMenuDefault = ::GetMenu(pMainFrame->m_hWnd);
	if (pMainFrame->m_hMenuDefault == NULL)
		TRACE0(_T("Warning: CMDIFrame without a default menu\n"));

	/* Set the global application window handle */
	m_pMainWnd = pMainFrame;

	// tedr - lets stick with default behavior so things get grayed
	//pMainFrame->m_bAutoMenuEnable = FALSE; 
	      // FALSE => menu items without handlers will NOT be disabled

	// Enable file Manager drag/drop and DDE Execute DDEExecute Open 
	m_pMainWnd->DragAcceptFiles();
	EnableShellOpen();

    /* Register our own external clipboard data format */
    Control.cfImage5Ptr = RegisterClipboardFormat(_T("_PIMAGE5_"));
    Control.cfImage = RegisterClipboardFormat(_T("MGX_IMAGE2"));
    Control.cfImagePP4 = RegisterClipboardFormat(_T("MGX_IMAGE"));

    if ( ( m_nCmdShow == SW_NORMAL || m_nCmdShow == SW_SHOW )
        && GetDefaultInt( _T("Maximized"), YES ) )
        m_nCmdShow = SW_MAXIMIZE;

    //  get highlights midtones and shadows maps
    if (!SetupHMSMaps(PictPubApp.GetResourceHandle(), IDC_BAND_ALL))
    {
        Message( IDS_ERESMISSING );
        return(0);
    }
   
    // Allocate the Linebuffer buffers (used for scanning as well)
    if ( !(Control.lpSegment = GetBuffers16Kx4( LineBuffer, YES )) )
    {
        if ( !(Control.lpSegment = GetBuffers16Kx4( LineBuffer, NO )) )
        {
            Message( IDS_EMEMALLOC );
            return( FALSE );
        }
    }
    
    /* Initialize all of the palettes and map structures */
    hDC = GetDC( AfxGetMainWnd()->GetSafeHwnd() );
    lpBltScreen = InitSuperBlt( hDC, 0, NULL, YES, YES );
    ReleaseDC( AfxGetMainWnd()->GetSafeHwnd(), hDC );
    if (!lpBltScreen)
    {
        Message( IDS_EMEMALLOC );
        return(0);
    }
	 SetDisplayGamma(lpBltScreen);
    // initialize the control library 
    if (!Ctl_Init( GetResourceHandle(), 0, Window.iLabelFont, lpBltScreen, hInstIconLib, Control.fNewLook ))
        return(0);

    
    #ifdef WIN32
    if (Control.UseThreading)
    {
        Control.ThreadDataTlsIndex = TlsAlloc();
        // initialiize progress pointer
        lpThreadData = (LPTHREADDATA)Alloc((long)sizeof(THREADDATA));
        if (lpThreadData)
        {
            clr((LPTR)lpThreadData, sizeof(THREADDATA));
            lpThreadData->ThreadInfo.pThread = AfxGetThread();
            lstrcpy(lpThreadData->ThreadInfo.szThreadName, _T("Application"));
            CursorDataInit(&lpThreadData->CursorData);
            // Initialize the progress data 
            ProgressInit(&lpThreadData->Progress);
        }
        TlsSetValue(Control.ThreadDataTlsIndex, lpThreadData);
        InitializeCriticalSection(&Control.StatusCriticalSection);
        InitializeCriticalSection(&Control.UntitledCriticalSection);
    }
    #endif
    
    // Allocate the brush settings buffer
    lpBrushSettings = (LPBRUSHSETTINGS)Alloc ((long) (sizeof(BRUSHSETTINGS) *
                                     TOTAL_BRUSHES));
    if ( !lpBrushSettings )
    {
        Message( IDS_EMEMALLOC );
        return(0);
    }
    
    if (!SetupBrushBuffers(YES))
    {
        Message( IDS_EMEMALLOC );
        return(0);
    }
    
    /* Put the application's name up in lights! */
	SetWindowText( AfxGetMainWnd()->GetSafeHwnd(), szAppName );

#ifdef BOMB
    // if this is a 30-day version, see if there is any time left

    if( !( FreeTrialStartup( Control.wDaysLeft,     // if all locks valid, set to number of days left.
                             Control.wTrialsLeft,   // if all locks valid, set to number of trials left.
                             Control.bIntruder,     // set to TRUE if intruder detected.
                             Control.bFirstTime ))) // set to TRUE if first time on system.
    {
		AstralDlg( NO, m_hInstance, AfxGetMainWnd()->GetSafeHwnd(), IDD_TRIALEXPIRED, DlgTrialExpiredProc );
        return FALSE;
    }
	AstralDlg( NO, m_hInstance, AfxGetMainWnd()->GetSafeHwnd(), IDD_TRIALDAYSLEFT, DlgTrialDaysLeftProc );
#else
    CleanLocks();       // remove locks from system, if previously set
#endif

	if (!bRunningEmbedded)
	{
#ifndef _DESIGNERFIX
		// Clear the MGX.INI EditorInProgress Hack for using PP as Editor
		// of choice in Designer. 
		// === MUST Use ONLY when NOT Running Embedded 
		// Otherwise state will be incorrect when		
		// COleServerDoc::OnNewDocument checks it. ===
		DesignerEditorInProgress ();  
#endif //_DESIGNERFIX

		/* Display the application window */
		ShowWindow( AfxGetMainWnd()->GetSafeHwnd(), m_nCmdShow );
		if ( m_nCmdShow != SW_HIDE )
		    UpdateWindow( AfxGetMainWnd()->GetSafeHwnd() );
	    if ( m_nCmdShow == SW_NORMAL || m_nCmdShow == SW_MAXIMIZE || m_nCmdShow == SW_SHOW )
	    {
			if (NeedSPAWarning())
				AstralDlg( NO, PictPubApp.GetResourceHandle(), AfxGetMainWnd()->GetSafeHwnd(), IDD_WARNING, DlgWarningProc );

			lstrcpy( szFileName, Control.ProgHome );
			lstrcat( szFileName, _T("LOGO.TIF") );
	        if ( FileExists( szFileName ) &&
	            ( !m_lpCmdLine || ( m_lpCmdLine && *m_lpCmdLine == _T('\0') ) ) )
	        {
	            if ( hDlg = AstralDlg( YES, PictPubApp.GetResourceHandle(), 
					AfxGetMainWnd()->GetSafeHwnd(), IDD_HERALD, DlgHeraldProc ) )
	                    UpdateWindow( hDlg );
	        }
	    }
	}

    // Used to set the height of combo boxes to be the same as edit boxes
    AstralDlg( NO, PictPubApp.GetResourceHandle(), AfxGetMainWnd()->GetSafeHwnd(), IDD_COMBO, DlgComboProc );

    /* Load the global data structures with any default values */
    LoadAllPreferences(NO);
    
#ifndef _FASTINIT
	// add and/or delete items to menus ( any one-time ( permanent ) menu changes )
    // setup IDR_PPMAIN
    SetupAppMainMenu( CMenu::FromHandle( GetMenu( pMainFrame->m_hWnd )));

    // setup IDR_PPSVRTYPE_PP
    POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
    if (TemplatePos)
    {
        CPPMultiDocTemplate *pTemplate = (CPPMultiDocTemplate *)PictPubApp.GetNextDocTemplate (TemplatePos);
		SetupAppSharedMenu(CMenu::FromHandle(pTemplate->m_hMenuShared));
	}

    // Destroy the herald, if it's up
    if ( hDlg = AstralDlgGet(IDD_HERALD) )
        AstralDlgEnd( hDlg, TRUE );
#endif //_FASTINIT


#ifndef _FASTINIT
	// Main Toolbox (and other if applicable) must be loaded regardless of inplace
	// or not.
    PictPubApp.GetCustomBoxes()->Load((CFrameWnd*)m_pMainWnd);
#endif // _FASTINIT

    if (!bRunningEmbedded)
	{
		UpdateRegInfo();
	    Window.fHasZoom = NO;
	    hZoomWindow = NULL;   
	    if ( GetDefaultInt( _T("upShield"), NO ) )
	        AstralDlg( YES, PictPubApp.GetResourceHandle(), AfxGetMainWnd()->GetSafeHwnd(), IDD_SHIELDFLOAT, DlgShieldFloatProc );
		if ( GetDefaultInt( _T("upPalette"), NO ) )
		{
		    fShowPalette = TRUE;
		    AstralDlg( YES, PictPubApp.GetResourceHandle(), AfxGetMainWnd()->GetSafeHwnd(), IDD_PALETTE, DlgPaletteProc );
		}
		else
		    fShowPalette = FALSE;
		ShowHideMenu( IDC_STATUSPALETTE, !fShowPalette, ::GetMenu( PictPubApp.Get_hWndAstral()));

	    if(( GetDefaultInt( _T("upObjects"), NO )) && Control.Retail )
	        ObjFloat.Create();

	    if ( GetDefaultInt( _T("upInfo"), NO ) )
	        AstralDlg( YES, PictPubApp.GetResourceHandle(), AfxGetMainWnd()->GetSafeHwnd(), IDD_INFOFLOAT, DlgInfoFloatProc );
	    if ( GetDefaultInt( _T("upClipBrowser"), NO ) )
		{
    		 AstralDlg( YES, GetResourceHandle(), Get_hWndAstral(),
			 	ExtBrowser.PasteViewThumbnails ? 
				IDD_PASTEFROMFILE_THUMB:IDD_PASTEFROMFILE,DlgPasteFromFileProc);
		}
	}
	else
	{
	    Window.fHasZoom = GetDefaultInt( _T("upZoom"), NO );
	    hZoomWindow = NULL;
	}

    #ifdef WIN32
    if ( Control.UseThreading && GetDefaultInt( _T("upTaskMan"), NO ) )
        AstralDlg( YES, PictPubApp.GetResourceHandle(), AfxGetMainWnd()->GetSafeHwnd(), IDD_TASKMANAGER, DlgTaskManagerProc );
    #endif
    
    // Bring up the status (hint line) window
    UpdateStatusBar( YES, YES, YES, YES );
    UpdateWindow(AstralDlgGet(IDD_STATUS));
    DisableSavebits( AstralDlgGet(IDD_STATUS) );
    
#ifndef _FASTINIT    
    // Put up the welcome message, if status hints turned on
    if( Hints.fStatusHintsOn )
        MessageStatus( IDS_WELCOME, (LPTSTR)szAppName );
#endif //_FASTINIT    
    
    // Bring up the default tool ribbon
    ActivateTool( IDC_SQUARE ); // make sure the tool gets depressed
    
    // Force the MDI client to be updated to not include status and ribbon windows
    SendMessage( AfxGetMainWnd()->GetSafeHwnd(), WM_SIZE, 0, 0L );
    
    if (Control.UseTablet)
        Control.UseTablet = TabletStart();
    
#ifndef _FASTINIT    
    /* This is TWAIN Stuff */
    {
        TW_IDENTITY identity;
    
        identity.Id = (TW_UINT32)AfxGetMainWnd()->GetSafeHwnd();
        identity.Version.MajorNum = 3;
        identity.Version.MinorNum = 1;
        identity.Version.Language = TWLG_USA;
        identity.Version.Country  = TWCY_USA;
        lstrcpy(identity.Version.Info,  _T("3.1 Beta Release"));
    
        identity.ProtocolMajor    = TWON_PROTOCOLMAJOR;
        identity.ProtocolMinor    = TWON_PROTOCOLMINOR;
        identity.SupportedGroups  = DG_CONTROL | DG_IMAGE;
        lstrcpy(identity.Manufacturer,  _T("MicroGrafX Inc."));
        lstrcpy(identity.ProductFamily, _T("Windows Apps"));
        lstrcpy(identity.ProductName,   szAppName );
        
        DCInitialize( &identity, AfxGetMainWnd()->GetSafeHwnd() );
    
    } /* End of TWAIN Stuff */
                  
#endif // _FASTINIT

// Init Color Management 
CMSInit();

// Load Color Management 
// Color Management Must be Initialized by the Main App Thread because all
// worker threads must be children of the initialization thread.

if( Control.bUseCMS )
	Control.CMSEnabled = CMSOpen( YES );
else
   Control.CMSEnabled = NO;

#ifdef _FASTINIT
// Destroy the herald, if it's up
if ( hDlg = AstralDlgGet(IDD_HERALD) )
    AstralDlgEnd( hDlg, TRUE );
#endif //_FASTINIT

    if (!bRunningEmbedded)
    {                      
		// process the Command Line here. 
	    HandleCommandLine( AfxGetMainWnd()->GetSafeHwnd(), m_lpCmdLine, &bPrint );

	    /* If the caller passed in a file name, try to load it or print it */
	    if ( bPrint ) // The message loop takes care of the exit
    		SendMessage( AfxGetMainWnd()->GetSafeHwnd(), WM_COMMAND, IDM_EXIT, 0L );

        SetFocus( AfxGetMainWnd()->GetSafeHwnd() );
    }

	// install the Windows Keyboard Hook for F1 Help Processing
	StartF1Help();

	if ( !bPrint && !bRunningEmbedded && Names.StartMacro[0] && !StringsEqualID( Names.StartMacro, IDS_NONE ) )
		if ( LookupExtFile( Names.StartMacro, szFileName, IDN_MACRO ) )
		{
	        PlayMacro(NULL, szFileName, 1, NO, NULL, NULL, 0, Names.StartMacro);
		}

#ifdef BOMB
    // see if someone tried to bypass our locks
    if( !( FreeTrialFailSafe()))
    {
		AstralDlg( NO, PictPubApp.GetResourceHandle(), AfxGetMainWnd()->GetSafeHwnd(), IDD_TRIALEXPIRED, DlgTrialExpiredProc );
        return FALSE;
    }
#endif

#ifdef _FASTINIT
m_pInitThread  = new CInitializationThread(this);
if (m_pInitThread)
{
    if (!m_pInitThread->StartThread())
    {
        delete m_pInitThread;
        m_pInitThread = NULL;
    }
}
#endif // _FASTINIT

    return TRUE;
}


void CPictPubApp::OnSysColorChange()
{
	DrawTools.OnSysColorChange();
	OCToolList->UpdateSysColors();
}


//************************************************************************
// DESCRIPTION:  Override.... so we can do extra cleanup/prompting
//
//************************************************************************
BOOL CPictPubApp::SaveAllModified()
//************************************************************************
{
    BOOL Bool, fHasZoom;
    HWND hWnd;

    // Check to see if the palette needs to be saved
    if ( !PaletteDlgClose(NO) )
        return(FALSE);

    // deactivate the current tool, this must be done here
    // because the deactivation may cause an image to become modified
    DeactivateTool();

	if (!CWinApp::SaveAllModified())
		return(FALSE);

    // turn off bubble hints timer
    HelpContext.SetHintWindows();  
    //Clean up the TWAIN SourceManager
    DCTerminate();

    // take care of things here that cannot be taken care of
    // in do_freeup() because it is too late
    fHasZoom = Window.fHasZoom;     // Closing images will set this to False

    // Save window states - this is done here because the main app
    // is hidden when OnDestroy() is called and that causes
    // IsWindowVisible() to always return FALSE
    CMDIFrameWnd *pMDIWnd = (CMDIFrameWnd*)PictPubApp.m_pMainWnd;  
	if (pMDIWnd)
	    PutDefaultInt( _T("Maximized"), pMDIWnd->IsZoomed() );
//    Bool = ( (hWnd = AstralDlgGet(IDD_TOOLS)) && ::IsWindowVisible(hWnd) );
//    PutDefaultInt( _T("upTools"), Bool );
    Bool = ( (hWnd = AstralDlgGet(IDD_SHIELDFLOAT)) && ::IsWindowVisible(hWnd) );
    PutDefaultInt( _T("upShield"), Bool );
	// there are 2 possible clip browsers
    Bool = ( (hWnd = AstralDlgGet(IDD_PASTEFROMFILE)) && ::IsWindowVisible(hWnd) );
	if (!Bool)
    	Bool = ( (hWnd = AstralDlgGet(IDD_PASTEFROMFILE_THUMB)) && ::IsWindowVisible(hWnd) );
    PutDefaultInt( _T("upClipBrowser"), Bool );
    Bool = ( (hWnd = AstralDlgGet(IDD_PALETTE)) && ::IsWindowVisible(hWnd) );
    PutDefaultInt( _T("upPalette"), Bool );
    Bool = ( (hWnd = ObjFloat.GetSafeHwnd()) && ::IsWindowVisible(hWnd) );
    PutDefaultInt( _T("upObjects"), Bool );
    Bool = ( (hWnd = AstralDlgGet(IDD_INFOFLOAT)) && ::IsWindowVisible(hWnd) );
    PutDefaultInt( _T("upInfo"), Bool );
    Bool = ( (hWnd = AstralDlgGet(IDD_TASKMANAGER)) && ::IsWindowVisible(hWnd) );
    PutDefaultInt( _T("upTaskMan"), Bool );
    PutDefaultInt( _T("upZoom"), fHasZoom );

	return(TRUE);
}

//************************************************************************
//	Note: never called if app is closing due to windows closing
//************************************************************************
BOOL CPictPubApp::ExitInstance ()
{
	// -----------------------------------------------------------------
	// Logoff the Mail System is logged on
	// -----------------------------------------------------------------
	if (m_pMail)
	{
		if (m_pMail->IsLoggedOn())
			m_pMail->Logoff();
		delete m_pMail; // this cause the library to unload as well.
		m_pMail = NULL;
	}

    // -----------------------------------------------------------------
    // Delete clipboard formats.  If they have not been rendered before
    // now (ie.)CPictPubApp::SaveAllModified(), then it is too late here
    // -----------------------------------------------------------------
    COleDataSource *pDataSource = COleDataSource::GetClipboardOwner();
    if (pDataSource)
       pDataSource->Empty();

	// Deinstall the Windows Keyboard hook for F1 Help 
	EndF1Help();

    // turn off bubble hints timer
    HelpContext.SetHintWindows();   

    /* Clean up the TWAIN Source Manager */
    DCTerminate();

    /* Clean up the color management system */
	 if( Control.CMSEnabled )
	 	CMSOpen( NO );

    // clean up the control library
    Ctl_Cleanup(m_hInstance);

//**************************************************************************
// Stuff moved from mdiframe::on_freeup
//**************************************************************************
    /* Free up any DOS memory */
    GetBuffers16Kx4( NULL, YES );
    
    // Delete any GDI objects that might be hanging around
    if ( Window.hBGBrush )
        DeleteObject( Window.hBGBrush );

    // free up the current brush and all brush related memory
    DestroyMgxBrush(Retouch.hBrush);
    if ( lpBrushSettings )
        FreeUp( (LPTR) lpBrushSettings );
    SetupBrushBuffers(NO);

    // free up tonal range maps
    FreeupHMSMaps();

    // free up superblt stuff
    if (lpBltScreen)
        EndSuperBlt(lpBltScreen);

    // FreeUp the ext name paths
    LoadAllExtNamePaths( YES );

    // free up the jpeg library that's always kept around
    // NULL to free up grabber library
    LoadGrabberLibrary(NULL);

	// Free IME lib, if loaded
	if( hInstIMELib )
	{
	    FreeLibrary( hInstIMELib );
    	hInstIMELib = NULL;
	}

    //	Free Hint Library 
	FreeLibrary( hInstHintLib );
    hInstHintLib = NULL;

    //	Free Icon Library 
	FreeLibrary( hInstIconLib );
    hInstIconLib = NULL;

    //  Free String Library last in case anybody else needs it
    FreeLibrary( hInstStringLib );
    hInstStringLib = NULL;
//**************************************************************************
// Stuff moved from mdiframe::on_freeup
//**************************************************************************

	return CWinApp::ExitInstance();
}

CPPToolList *CPictPubApp::GetCustomBoxes()
{
	return (((CMDIFrame *)m_pMainWnd)->m_pListCustTools);		
}

HWND CPictPubApp::Get_hActiveWindow(BOOL bUseAfxGetMainWnd/*TRUE*/)
/* Returns the HANDLE to the current Active View if it exists. Else it returns
   NULL */
{
    return GetActiveView()->GetSafeHwnd();
}

HWND CPictPubApp::Get_hClientAstral()
/* Returns the MDI Client Window. */
{
	CWnd *pWnd = ::AfxGetMainWnd();
	HWND hWnd = NULL;
	ASSERT(pWnd);
	if (pWnd)
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrame)))
		{
			hWnd = ((CMDIFrame*)pWnd)->m_hWndMDIClient;
		}
		else
		if (pWnd->IsKindOf(RUNTIME_CLASS(CInPlaceFrame)))
		{
      hWnd = ((CInPlaceFrame*)pWnd)->GetClientFrameWnd();      
		}
	}
	ASSERT(hWnd);
	return (hWnd);
}

HWND CPictPubApp::Get_hWndAstral(BOOL bUseAfxGetMainWnd/*TRUE*/)
/* Returns the Main App's hWnd */
{
	HWND hWnd = NULL;
	if (bUseAfxGetMainWnd)
	{
		if (::AfxGetMainWnd())
			hWnd = ::AfxGetMainWnd()->GetSafeHwnd();
	}
	else
	{
		if (m_pMainWnd)
			hWnd = m_pMainWnd->GetSafeHwnd();
	}

	return hWnd;
};

HINSTANCE CPictPubApp::Get_hInstAstral(BOOL bUseAfxGetInstanceHandle/*TRUE*/)
{
	return m_hInstance;
}

HINSTANCE CPictPubApp::GetResourceHandle()
{
	return AfxGetResourceHandle();
}

CServerView *CPictPubApp::GetActiveView()
{
return(m_pActiveView);
}

BOOL CPictPubApp::IsActiveDocInPlaceActive()
{
	return m_bInPlaceActive;
	//BOOL bInPlaceActive = FALSE;
	//CServerView *pActiveView = GetActiveView();
	//
	//if (pActiveView && pActiveView->GetDocument())
	//	bInPlaceActive = pActiveView->GetDocument()->IsInPlaceActive();
	//
	//return bInPlaceActive;
}

void CPictPubApp::SetActiveView(CServerView *pViewWnd)
{
	m_pActiveView = pViewWnd;
}

BOOL CPictPubApp::SetClipboardFileName(LPTSTR lpFileName, DWORD ClipboardID)
{
	ASSERT(m_pClipboardFileList);
	ASSERT(lpFileName);

	CLIPBOARDFILE *pNewClipboardFile = NULL;

	TRY
	{											    
		pNewClipboardFile = new CLIPBOARDFILE;
		ASSERT(pNewClipboardFile);

		CMDIFrame *pMainWnd = (CMDIFrame *)m_pMainWnd;

		pNewClipboardFile->ClipboardID = ClipboardID;
		lstrcpy(pNewClipboardFile->szFileName, lpFileName);
		pNewClipboardFile->nReferenceCount = 0;

		m_pClipboardFileList->AddTail(pNewClipboardFile);
		return(TRUE);
	}
	CATCH_ALL(e)
	{
		if (pNewClipboardFile)
			delete pNewClipboardFile;
		return(FALSE);
	}
	END_CATCH_ALL
}

LPTSTR CPictPubApp::GetClipboardFileName(LPTSTR lpFileName)
{
	ASSERT(m_pClipboardFileList);
	ASSERT(lpFileName);

	POSITION pos = m_pClipboardFileList->GetHeadPosition();
	CLIPBOARDFILE *pClipboardFile;

	CMDIFrame *pMainWnd = (CMDIFrame *)m_pMainWnd;
	ULONG ClipboardID = pMainWnd->GetClipboardID();

	while (pos)
	{
		pClipboardFile = (CLIPBOARDFILE*)m_pClipboardFileList->GetNext(pos);		
		if (pClipboardFile && (pClipboardFile->ClipboardID == ClipboardID))
		{
			lstrcpy(lpFileName, pClipboardFile->szFileName);
			return(lpFileName);
		}
	}
	return (NULL);
}

void CPictPubApp::AddClipboardFileReference(LPTSTR lpFileName)
{
	ASSERT(m_pClipboardFileList);
	ASSERT(lpFileName);

	POSITION pos = m_pClipboardFileList->GetHeadPosition();
	CLIPBOARDFILE *pClipboardFile;

	while (pos)
	{
		pClipboardFile = (CLIPBOARDFILE*)m_pClipboardFileList->GetNext(pos);		
		if (pClipboardFile &&
			StringsEqual(pClipboardFile->szFileName, lpFileName))
		{
			pClipboardFile->nReferenceCount += 1;
			break;
		}
	}
}

void CPictPubApp::SetupAppMainMenu( CMenu *pMenu )
{
	AddFileMainMenuItems( GetSubMenu( pMenu, IDM_FILEMENU ));
}

void CPictPubApp::SetupAppSharedMenu( CMenu *pMenu )
{
	AddFileSharedMenuItems( GetSubMenu( pMenu, IDM_FILEMENU ));
	AddImageMenuItems( GetSubMenu( pMenu, IDM_IMAGEMENU ));

    if( Control.Retail )
    	EnableObjectMenu( pMenu );
}

void CPictPubApp::AddFileMainMenuItems( CMenu *pFileMenu )
{
	HMENU hFileMenu = pFileMenu->GetSafeHmenu();

	// Make any one-time ( permanent ) menu changes.
    // delete menu items from the bottom up.
    //SetupAdobeFileImportMenu( hFileMenu, Control.Retail ? IDM_IMPORTPOPUP : IDM_IMPORTPOPUP - IDM_OEMOFFSET );  // Add the Photoshop import plugins

   //	SetupRunMenus( hFileMenu, Control.Retail ? IDM_RUNPOPUP : IDM_RUNPOPUP - IDM_OEMOFFSET,
   //            	   IDM_RUN0, 10, IDS_RUNTASKLIST );

    //SetupImportMenu( hFileMenu, IDM_IMPORT );
}

void CPictPubApp::AddFileSharedMenuItems( CMenu *pFileMenu )
{
	HMENU hFileMenu = pFileMenu->GetSafeHmenu();

    // Make any one-time ( permanent ) menu changes.
    // delete menu items from the bottom up.
    //SetupAdobeFileExportMenu( hFileMenu, Control.Retail ? IDM_EXPORTPOPUP : IDM_EXPORTPOPUP - IDM_OEMOFFSET );  // Add the Photoshop export plugins
	//
    //SetupAdobeFileImportMenu( hFileMenu, Control.Retail ? IDM_IMPORTPOPUP : IDM_IMPORTPOPUP - IDM_OEMOFFSET );  // Add the Photoshop import plugins
	//
	//SetupRunMenus( hFileMenu, Control.Retail ? IDM_RUNPOPUP : IDM_RUNPOPUP - IDM_OEMOFFSET,
    //           	   IDM_RUN0, 10, IDS_RUNTASKLIST );
	//
    //SetupImportMenu( hFileMenu, IDM_IMPORT );
}

void CPictPubApp::AddImageMenuItems( CMenu *pImageMenu )
{
	HMENU hImageMenu = pImageMenu->GetSafeHmenu();

    // CALFIX SetupPluginMenus( hImageMenu ); // Add the Gallery effects plugins
    SetupAdobeImageMenu( hImageMenu );      // Add the Photoshop plugins
}

void CPictPubApp::EnableObjectMenu( CMenu *pMenu )
{
    if( Control.MultipleObjects )
		pMenu->EnableMenuItem( IDM_OBJECTMENU, MF_ENABLED|MF_BYPOSITION );
	else
		pMenu->EnableMenuItem( IDM_OBJECTMENU, MF_GRAYED|MF_BYPOSITION );

	CWnd *pWnd = AfxGetMainWnd();
	if (pWnd)
		pWnd->DrawMenuBar();
}

CMenu *CPictPubApp::GetSubMenu( CMenu *pMenu, int nPos )
{
    // adjust for no object menu
    if( !Control.Retail )
    {
        if( nPos > IDM_OBJECTMENU )
            --nPos;
    }

    BOOL fMaximized;

    CMDIFrameWnd *pFrame = ( CMDIFrameWnd * )AfxGetMainWnd();
    if( pFrame )
    {
	    CMDIChildWnd *pWnd = pFrame->MDIGetActive( &fMaximized );

	    // adjust for mdi cookie
	    if( pWnd && fMaximized )
    	    ++nPos;
    }
    return( pMenu->GetSubMenu( nPos ));
}

BOOL CPictPubApp::OnCmdMsg(UINT nID, int nCode, void* pExtra,
      AFX_CMDHANDLERINFO* pHandlerInfo)
   {
   if (PictPubApp.GetCustomBoxes()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
      return(TRUE);

   return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
   }

void CPictPubApp::UpdateRegInfo()
{
	ASSERT(PictPubApp.m_templateList.GetCount() == 1); // should only be one for now...

	// Register other file types and data formats we can serve
	CString strFileExt,strFileTypeId;
	POSITION TemplatePos = PictPubApp.GetFirstDocTemplate();
	if (TemplatePos)
	{
	    CPPMultiDocTemplate *pTemplate = (CPPMultiDocTemplate *)PictPubApp.GetNextDocTemplate (TemplatePos);
		if (pTemplate->GetDocString(strFileTypeId,
			   CDocTemplate::regFileTypeId) && !strFileTypeId.IsEmpty())
		{
			CString progPath;
			GetModuleFileName( m_hInstance, progPath.GetBuffer(255), 255);
			progPath.ReleaseBuffer();

#ifndef _DESIGNERFIX
			PatchRegDBFix(strFileTypeId);		
#endif //_DESIGNERFIX
		    if (Control.Retail)
			{
    	   		m_PPserver.UpdateRegistry(OAT_INPLACE_SERVER,NULL,NULL); // embedding server for now...
			}

			// this registers those File Types found in the template document list
			RegisterShellFileTypes(); 

			//-------------------------------------------
			// Register icons for the various document types ( used by Chicago Explorer )     
			// FORMAT in Reg DataBase example:
			// ROOT\\ppfile = MGX Picture Publisher Image
			//		 	   \\DefaultIcon = c:\pp50\pp50.exe,1
			//			   \\open\\command = c:\pp50\pp50.exe %1
			//			   \\print\\command = c:\pp50\pp50.exe /p %1
			//-------------------------------------------
			RegDocIcon(progPath,_T("MGX Picture Publisher Image"),_T("ppfile"),_T("1"));
			RegDocIcon(progPath,_T("MGX Picture Publisher Editable Image"),_T("ppotherfile"),_T("2"));
			// Always overwrite ppf, pp5, tif and pp4
			SetRegExt(_T(".ppf"),_T("ppfile"),TRUE);
			SetRegExt(_T(".pp5"),_T("ppfile"),TRUE);
			SetRegExt(_T(".pp4"),_T("ppfile"),TRUE);
			SetRegExt(_T(".tif"),_T("ppotherfile"),TRUE);
			// if the following extensions don't exist, then take 'em
			SetRegExt(_T(".pcx"),_T("ppotherfile"),FALSE);
			SetRegExt(_T(".gif"),_T("ppotherfile"),FALSE);
			SetRegExt(_T(".bmp"),_T("ppotherfile"),FALSE);
			SetRegExt(_T(".jpg"),_T("ppotherfile"),FALSE);
			SetRegExt(_T(".dcs"),_T("ppotherfile"),FALSE);
			SetRegExt(_T(".eps"),_T("ppotherfile"),FALSE);
			SetRegExt(_T(".pcd"),_T("ppotherfile"),FALSE);
			SetRegExt(_T(".tga"),_T("ppotherfile"),FALSE);

			// ------------------------------------------------------------
			// Add Set/Get formats supported
			// ------------------------------------------------------------
			CString strSetGet;
			CString strSetGetValue;

			strSetGet = strFileTypeId;
			strSetGet += _T("\\protocol\\StdFileEditing\\SetDataFormats");
			strSetGetValue = _T("CF_DIB, CF_BITMAP");
			(void)SetRegKey(strSetGet,strSetGetValue);
	
			strSetGet = strFileTypeId;
			strSetGet += _T("\\protocol\\StdFileEditing\\RequestDataFormats");
			strSetGetValue = _T("CF_DIB");
			(void)SetRegKey(strSetGet,strSetGetValue);
	
			strSetGet = _T("CLSID\\{BC7A9620-3913-101B-9649-020080A38E74}\\DataFormats\\GetSet\\1");
			strSetGetValue = _T("2,1,16,2");	//CF_BITMAP,DVS_CONTENT,TYMED_GDI,SET DIRECTION ONLY
			(void)SetRegKey(strSetGet,strSetGetValue);
			strSetGet = _T("CLSID\\{BC7A9620-3913-101B-9649-020080A38E74}\\DataFormats\\GetSet\\0");
			strSetGetValue = _T("8,1,1,3");	//CF_DIB,DVS_CONTENT,TYMED_GLOBAL,SET/GET DIRECTION
			(void)SetRegKey(strSetGet,strSetGetValue);
	
			// ------------------------------------------------------------
			// Write Conversion and Emulation entries to PP40 processing
			// ------------------------------------------------------------
			CString strConvert;
			CString strConvertValue (_T("BitmapImage")); // PP40's Progid
			strConvert = _T("CLSID\\{BC7A9620-3913-101B-9649-020080A38E74}\\Conversion\\Readwritable\\Main");
			(void)SetRegKey(strConvert, strConvertValue);
			strConvert = _T("CLSID\\{BC7A9620-3913-101B-9649-020080A38E74}\\Conversion\\Readable\\Main");
			(void)SetRegKey(strConvert, strConvertValue);

			// ------------------------------------------------------------
			// Write shell printing to registry
			// ------------------------------------------------------------
			CString strPrint;
			progPath += _T(" /p %1");
			strPrint = strFileTypeId;
			strPrint += _T("\\Shell\\Print\\Command");
			(void)SetRegKey(strPrint, progPath);
		}
	}
}

// INI strings are not localized
static const TCHAR szSoftware[] = _T("Software");
// determine number of elements in an array (not bytes)
#define _countof(array) (sizeof(array)/sizeof(array[0]))

#define ID_SECTION ID_SETTINGS
// uncomment line below to put settings in pp.ini
//#define ID_SECTION ID_APPL

#ifndef _MAC
// returns key for HKEY_LOCAL_MACHINE\_T("Software")\RegistryKey\ProfileName
// creating it if it doesn't exist
// responsibility of the caller to call RegCloseKey() on the returned HKEY
HKEY CPictPubApp::GetLocalRegistryKey()
{
	ASSERT(m_pszRegistryKey != NULL);
	ASSERT(m_pszProfileName != NULL);

	HKEY hAppKey = NULL;
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSoftware, 0, KEY_WRITE|KEY_READ,
		&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dw;
		if (RegCreateKeyEx(hSoftKey, m_pszRegistryKey, 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
			&hCompanyKey, &dw) == ERROR_SUCCESS)
		{
			RegCreateKeyEx(hCompanyKey, m_pszProfileName, 0, REG_NONE,
				REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
				&hAppKey, &dw);
		}
	}
	if (hSoftKey != NULL)
		RegCloseKey(hSoftKey);
	if (hCompanyKey != NULL)
		RegCloseKey(hCompanyKey);

	return hAppKey;
}

// returns key for:
//      HKEY_LOCAL_MACHINE\_T("Software")\RegistryKey\AppName\lpszSection
// creating it if it doesn't exist.
// responsibility of the caller to call RegCloseKey() on the returned HKEY
HKEY CPictPubApp::GetLocalSectionKey(LPCTSTR lpszSection)
{
	ASSERT(lpszSection != NULL);

	HKEY hSectionKey = NULL;;
	HKEY hAppKey = GetLocalRegistryKey();
	if (hAppKey == NULL)
		return NULL;

	DWORD dw;
	RegCreateKeyEx(hAppKey, lpszSection, 0, REG_NONE,
		REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
		&hSectionKey, &dw);
	RegCloseKey(hAppKey);
	return hSectionKey;
}
#endif //!_MAC

int CPictPubApp::GetLocalProfileInt( LPTSTR lpKeyName, int nDefault )
{
	return((int)GetLocalProfileInt(ID_SECTION, stripstruct(lpKeyName), nDefault));
}

UINT CPictPubApp::GetLocalProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	int nDefault)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
#ifndef _MAC
	if (m_pszRegistryKey != NULL) // use registry
	{
		HKEY hSecKey = GetLocalSectionKey(lpszSection);
		if (hSecKey == NULL)
			return nDefault;
		DWORD dwValue;
		DWORD dwType;
		DWORD dwCount = sizeof(DWORD);
		LONG lRes = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
			(LPBYTE)&dwValue, &dwCount);
		RegCloseKey(hSecKey);
		if (lRes == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_DWORD);
			ASSERT(dwCount == sizeof(dwValue));
			return (UINT)dwValue;
		}
		return nDefault;
	}
	else
#endif
	{
		ASSERT(m_pszProfileName != NULL);
		return ::GetPrivateProfileInt(lpszSection, lpszEntry, nDefault,
			m_pszProfileName);
	}
}

UINT CPictPubApp::GetLocalProfileString( LPTSTR lpKeyName, LPTSTR lpDefault, LPTSTR lpReturnedString, int nSize)
{
	CString szReturn = GetLocalProfileString(ID_SECTION, stripstruct(lpKeyName), lpDefault);
	lstrcpy(lpReturnedString, szReturn);
	return(szReturn.GetLength());
}

CString CPictPubApp::GetLocalProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	LPCTSTR lpszDefault)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
#ifndef _MAC
	if (m_pszRegistryKey != NULL)
	{
		HKEY hSecKey = GetLocalSectionKey(lpszSection);
		if (hSecKey == NULL)
			return lpszDefault;
		CString strValue;
		DWORD dwType, dwCount;
		LONG lRes = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
			NULL, &dwCount);
		if (lRes == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_SZ);
			lRes = RegQueryValueEx(hSecKey, (LPTSTR)lpszEntry, NULL, &dwType,
				(LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount);
			strValue.ReleaseBuffer();
		}
		RegCloseKey(hSecKey);
		if (lRes == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_SZ);
			return strValue;
		}
		return lpszDefault;
	}
	else
#endif
	{
		ASSERT(m_pszProfileName != NULL);

		if (lpszDefault == NULL)
			lpszDefault = &afxChNil;    // don't pass in NULL
		TCHAR szT[_MAX_PATH];
		::GetPrivateProfileString(lpszSection, lpszEntry, lpszDefault,
			szT, _countof(szT), m_pszProfileName);
		return szT;
	}
}

#ifdef AFX_CORE3_SEG
#pragma code_seg(AFX_CORE3_SEG)
#endif

BOOL CPictPubApp::WriteLocalProfileInt( LPTSTR lpKeyName, int nDefault )
{
	return(WriteLocalProfileInt(ID_SECTION, stripstruct(lpKeyName), nDefault));
}

BOOL CPictPubApp::WriteLocalProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,
	int nValue)
{
	ASSERT(lpszSection != NULL);
	ASSERT(lpszEntry != NULL);
#ifndef _MAC
	if (m_pszRegistryKey != NULL)
	{
		HKEY hSecKey = GetLocalSectionKey(lpszSection);
		if (hSecKey == NULL)
			return FALSE;
		LONG lRes = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_DWORD,
			(LPBYTE)&nValue, sizeof(nValue));
		RegCloseKey(hSecKey);
		return (lRes == ERROR_SUCCESS) ? TRUE : FALSE;
	}
	else
#endif
	{
		ASSERT(m_pszProfileName != NULL);

		TCHAR szT[16];
		wsprintf(szT, "%d", nValue);
		return ::WritePrivateProfileString(lpszSection, lpszEntry, szT,
			m_pszProfileName);
	}
}

BOOL CPictPubApp::WriteLocalProfileFileName( LPTSTR lpKeyName, LPTSTR lpFileName)
{
	UNIVERSAL_NAME_INFO info;
	FNAME szFileName;
	DWORD dwBufSize = sizeof(szFileName);
	info.lpUniversalName = szFileName;
	DWORD dwReturn = WNetGetUniversalName(lpFileName, UNIVERSAL_NAME_INFO_LEVEL, (LPVOID)&info, &dwBufSize);
	if (dwReturn == NO_ERROR)
		lpFileName = szFileName;
	return(WriteLocalProfileString(ID_SECTION, stripstruct(lpKeyName), lpFileName));
}

BOOL CPictPubApp::WriteLocalProfileString( LPTSTR lpKeyName, LPTSTR lpString)
{
	return(WriteLocalProfileString(ID_SECTION, stripstruct(lpKeyName), lpString));
}

BOOL CPictPubApp::WriteLocalProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry,
			LPCTSTR lpszValue)
{
	ASSERT(lpszSection != NULL);
#ifndef _MAC
	if (m_pszRegistryKey != NULL)
	{
		LONG lRes;
		if (lpszEntry == NULL) //delete whole section
		{
			HKEY hAppKey = GetLocalRegistryKey();
			if (hAppKey == NULL)
				return FALSE;
			lRes = ::RegDeleteKey(hAppKey, lpszSection);
			RegCloseKey(hAppKey);
		}
		else if (lpszValue == NULL)
		{
			HKEY hSecKey = GetLocalSectionKey(lpszSection);
			if (hSecKey == NULL)
				return FALSE;
			// necessary to cast away const below
			lRes = ::RegDeleteValue(hSecKey, (LPTSTR)lpszEntry);
			RegCloseKey(hSecKey);
		}
		else
		{
			HKEY hSecKey = GetLocalSectionKey(lpszSection);
			if (hSecKey == NULL)
				return FALSE;
			lRes = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_SZ,
				(LPBYTE)lpszValue, (lstrlen(lpszValue)+1)*sizeof(TCHAR));
			RegCloseKey(hSecKey);
		}
		return (lRes == ERROR_SUCCESS) ? TRUE : FALSE;
	}
	else
#endif
	{
		ASSERT(m_pszProfileName != NULL);

		return ::WritePrivateProfileString(lpszSection, lpszEntry, lpszValue,
			m_pszProfileName);
	}
}

/***********************************************************************/
// CPPMultiDocTemplate Methods
/***********************************************************************/
CPPMultiDocTemplate::CPPMultiDocTemplate(
UINT nIDResource, 
CRuntimeClass* pDocClass,
CRuntimeClass* pFrameClass, 
CRuntimeClass* pViewClass) : 
    CMultiDocTemplate(nIDResource, pDocClass,pFrameClass, pViewClass)
{
}

void CPPMultiDocTemplate::CloseAllDocuments (BOOL bEndSession)
{
    //Delete or Render all promised clipboard formats
    COleDataSource *pDataSource = COleDataSource:: GetClipboardOwner();
    if (pDataSource)
    {
        if (!AstralAffirm(IDS_OKTORENDERCLIPBOARD))
            pDataSource->Empty();
        COleDataSource::FlushClipboard();
    }

    CMultiDocTemplate::CloseAllDocuments(bEndSession);
}

CDocument* CPPMultiDocTemplate::OpenDocumentFile(
LPIMAGE lpImage,
const TCHAR* pszPathName, 
BOOL bMakeVisible)
{
    ASSERT(lpImage);
    ASSERT(pszPathName != NULL);

    CServerDoc* pDocument = (CServerDoc*)CreateNewDocument();
    if (pDocument == NULL)
    {
        TRACE0(_T("CDocTemplate::CreateNewDocument returned NULL\n"));
        AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
        return NULL;
    }
    ASSERT_VALID(pDocument);

    BOOL bAutoDelete = pDocument->m_bAutoDelete;
    pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
    CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
    pDocument->m_bAutoDelete = bAutoDelete;
    if (pFrame == NULL)
    {
        AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
        ASSERT(pFrame == NULL);
        return NULL;
    }
    ASSERT_VALID(pFrame);
    
//---------------------------------
//==> Here's the difference between the base calls and us!!!
// we need the lpImage assigned before OnOpenDocument is called
//---------------------------------
	pDocument->SetImage(lpImage);
	pDocument->SetModifiedFlag(FALSE);  //Startoff unmodified
	pDocument->SetPathName(pszPathName,TRUE);
	// CurFile is used instead of pathname, because fastbits
	// documents are actually untitled documents with some bogus
	// untitled name set as the path name, the real path is in CurFile
	pDocument->SetTitle(filename(lpImage->CurFile));
	pDocument->RevokeAndRegister(pszPathName);
	pDocument->InitCompoundFile();
    InitialUpdateFrame(pFrame, pDocument, bMakeVisible);
    return pDocument;

}

CDocument* CPPMultiDocTemplate::OpenDocumentFile(
const TCHAR* pszPathName, 
OPENINFO *pOpenInfo,
BOOL bMakeVisible)
{
	FNAME szTitle;
	LPCMDLIST lpCmdList;

    ASSERT(pszPathName != NULL);

    CServerDoc* pDocument = (CServerDoc*)CreateNewDocument();
    if (pDocument == NULL)
    {
        TRACE0(_T("CDocTemplate::CreateNewDocument returned NULL\n"));
        AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
        return NULL;
    }
    ASSERT_VALID(pDocument);

    BOOL bAutoDelete = pDocument->m_bAutoDelete;
    pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
    CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
    pDocument->m_bAutoDelete = bAutoDelete;
    if (pFrame == NULL)
    {
        AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
        ASSERT(pFrame == NULL);
        return NULL;
    }
    ASSERT_VALID(pFrame);
    
//---------------------------------
//==> Here's the difference between the base calls and us!!!
// we need the lpImage assigned before OnOpenDocument is called
//---------------------------------
//    pDocument->m_lpImage = lpImage;
	pDocument->SetModifiedFlag(FALSE);  //Startoff unmodified
	pDocument->SetPathName(pszPathName,TRUE);
	lstrcpy(szTitle, pszPathName);
	pDocument->SetTitle(filename(szTitle));
	pDocument->RevokeAndRegister(pszPathName);
    InitialUpdateFrame(pFrame, pDocument, bMakeVisible);

	// create a command list for the load command
	if (!(lpCmdList = AddLoadCommandEx(pOpenInfo->idCommand, pOpenInfo->lpParms, FALSE)))
	{
        AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
        return NULL;
	}
	lpCmdList->PhotoCDResOverride = pOpenInfo->PhotoCDResOverride;
	// link document and command list to each other
	lpCmdList->pDocument = pDocument;
	pDocument->SetCmdList(lpCmdList);
	ExecuteCommands(lpCmdList);
	
    return pDocument;

}

void CPPMultiDocTemplate::OurSetServerInfo(UINT nIDOleEmbedding, UINT nIDOleInPlaceServer,
	CRuntimeClass* pOleFrameClass, CRuntimeClass* pOleViewClass)
// This method differs from the Foundations SetServerInfo in that we do not load
// the menu, and accelerator resources up front.  Instead we will delay this hit
// until the first OnCreateObject request.  This last method has benn overwritten
// in CPPOleTemplateServer to handle the actual resource load. 
{
	// This ROUTINE WILL PUT OFF THE LOADING OF THE RESOURCES Until the first 
	// OnCreateObject request-- save ids only here
	m_nIDOleEmbedding=nIDOleEmbedding;
	m_nIDOleInPlaceServer=nIDOleInPlaceServer;

	m_pOleFrameClass = pOleFrameClass;
	m_pOleViewClass = pOleViewClass;
	m_nIDServerResource = nIDOleInPlaceServer;
}

