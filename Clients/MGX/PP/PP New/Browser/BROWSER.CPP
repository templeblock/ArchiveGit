// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "id.h"
#include "browser.h"
#include "bsrport.h"
#include "bsrid.h"
#include "bsrtypes.h"
#include "handlebs.h"
#include "bsritems.h"
#include "bsrutil.h"
#include "bsrdlgs.h"
#include "thmbsr.h"
#include "cthmmgr.h"
#include "extbsr.h"
#include "ppafx.h"
#include "bsrcom.h"

/***************************Static prototypes******************************/
LOCAL void ResizeOpenDlg(HWND hDlg, BOOL bViewThumbs);
//LOCAL void ViewThumbs(HWND hDlg, BOOL bViewThumbs);
//LOCAL void CalculateOpenDlgSize(HWND hDlg);
LOCAL HWND CopyWindowEx( HWND hWnd, HWND hNewParent);
LOCAL void set_BSRkey_hook( LPBSRSTATE lpBsr);
LOCAL VOID clr_BSRkey_hook(void);

/***************************Static vars******************************/

/***************************Gloabal vars******************************/
// global state vars
LPBSRSTATE lpBsr;
BOOL bImported = FALSE;

// global state vars for last initialized dialog
// used for keyboard procs to ensure processing in correct context
LPBSRSTATE lpKeyHookBsr = NULL;
HHOOK prevKeyHook = NULL;
HOOKPROC KeyProc = NULL;		// key proc (to allow FreeProcInstance) 

//************************************************************************/
//	Brings up the open/save dlg.
//	To use fill an OPENSTATE structure and pass it in.
//	If the routine exits TRUE then the structure will contain the result.
//	This routine will update .INI entries. 
//************************************************************************/
BOOL DoOpenDlg( HWND hParent, LPOPENSTATE lpState, BOOL bUseSingleSelect /* FALSE */)
/************************************************************************/
{
	int 				iReturn, iCancelOK;
	WORD 				type;
	LPTSTR 			lpExtension;
	int 				drive, i, j;
	OPENSTATE 		OpenState;
	FNAME				szPath, szFile;
	TCHAR				string[256];
	WORD 				wChar;
   HKEY		   	hKey;
   DWORD		   	dwNew;
   DWORD		   	dwNewSize = sizeof( dwNew );
   DWORD       	dwType;
   CMgxFileDialog	OpenDlg( !lpState->bSaving, Control.WinVer == WV_WINDOWS95 );

	static	ITEMID	arOpenFileType[] = { IDT_PPCOMPOUND ,IDT_PP, IDT_TIFF, IDT_BMP,
    	IDT_TARGA, IDT_GIF, IDT_DIB, IDT_PCX, IDT_JPEG, IDT_PHOTOCD, IDT_SCITEX,
		IDT_PP4 };

	static	ITEMID	arImportFileType[] = { IDT_PPCOMPOUND ,IDT_PP, IDT_TIFF, IDT_BMP,
    	IDT_TARGA, IDT_GIF, IDT_DIB, IDT_PCX, IDT_JPEG, IDT_PHOTOCD, IDT_SCITEX,
		IDT_PP4 };

	clr((LPTR)&OpenState, sizeof(OpenState));
	
	// for CMS
	cmsInfoInit( &OpenState.cmsInfo );
	lpState->cmsInfo = OpenState.cmsInfo;

	bImported = OpenState.bImported = lpState->bImported;

	// setup local state
	OpenState.bSaving       = lpState->bSaving;
	OpenState.idDataType    = lpState->idDataType;
	OpenState.frameDepth    = lpState->frameDepth;

	if ((OpenState.wFileAttr = lpState->wFileAttr) == -1)
		OpenState.wFileAttr = DDL_READONLY;
	
	// copy items from browsre
    if( bImported )
    	OpenState.ViewAlbums = FALSE;
    else
    	OpenState.ViewAlbums = Browser.ViewAlbums;

	OpenState.ViewThumbnails = Browser.ViewThumbnails;
	OpenState.AutoThumbnails = Browser.AutoThumbnails;
	
    if( bImported )
	    type = bound(Browser.bsrImpFileType, IDT_FIRSTIMPORTFILETYPE, IDT_LASTIMPORTFILETYPE);
    else
	    type = bound(Browser.bsrFileType, IDT_FIRSTFILETYPE, IDT_LASTFILETYPE);

	if (lpState->idFileType)
		type = LocalFileType(lpState->idFileType);

	OpenState.idFileType = type;

	if( IsDBCSLeadByte(( BYTE )*lpState->szFileSpec ))
		wChar = *(( LPWORD )( lpState->szFileSpec ));
	else
		wChar = *( lpState->szFileSpec );

	if( wChar )
	{
		if ( type == IDT_ALLIMPORTIMAGES || type == IDT_ALLIMAGES || !AstralStr(type, &lpExtension))
			lstrcpy( OpenState.szFileSpec, _T("*.*"));
		else 
		{
			lstrcpy(OpenState.szFileSpec, _T("*"));
			lstrcat(OpenState.szFileSpec, lpExtension);
		}
	}
	else
		lstrcpy(OpenState.szFileSpec, lpState->szFileSpec);

	lstrcpy(OpenState.szNewFileSpec, OpenState.szFileSpec);
	lstrcpy(OpenState.szPath, lpState->szPath);
	lstrcpy(OpenState.szTitle, lpState->szTitle);
	lstrcpy(OpenState.szOK, lpState->szOK);
	
	if( IsDBCSLeadByte(( BYTE )*lpState->szAlbum ))
		wChar = *(( LPWORD )( lpState->szAlbum ));
	else
		wChar = *( lpState->szAlbum );

	if( wChar == _T('\0'))
		lstrcpy(OpenState.szAlbum, Browser.LastAlbum);
	else
		lstrcpy(OpenState.szAlbum, lpState->szAlbum);

	OpenState.bInitialized = FALSE;
	OpenState.OpenMode = OM_NORMAL;
	OpenState.bMultiListBox =  !bUseSingleSelect;
	
	OpenState.bExtended = FALSE;
	drive = CURRENTDRIVE;

	if (OpenState.bSaving && !bImported )
	{
	 	OpenState.idThumbDialog = IDD_THUMBSAVEAS;
	 	OpenState.idFileDialog = IDD_SAVEAS;
	}
	else
	{
        if( bImported )					 
        {
    	 	OpenState.idThumbDialog = bUseSingleSelect ? 
				IDD_THUMBIMPORT_SS : IDD_THUMBIMPORT;
    	 	OpenState.idFileDialog = bUseSingleSelect ? 
				IDD_IMPORT_SS : IDD_IMPORT;
        }
        else
        {
    	 	OpenState.idThumbDialog =bUseSingleSelect ? 
				IDD_THUMBOPEN_SS : IDD_THUMBOPEN;
    	 	OpenState.idFileDialog = bUseSingleSelect ? 
				IDD_OPEN_SS : IDD_OPEN;
        }
	}
	
	iReturn = OpenState.ViewThumbnails ? OpenState.idThumbDialog:  OpenState.idFileDialog;
                                   
	OpenDlg.SetThumbDialog( OpenState.idThumbDialog );
                                
   lpState->lpFileList = ( FNAME  FAR *)Alloc( MAX_STR_LEN * sizeof( FNAME ) );
   lpState->lpFileListVolumes = ( LPVOLUMEID )Alloc( sizeof( VOLUMEID ) );

   OpenDlg.m_ofn.lStructSize = sizeof( OPENFILENAME );
   OpenDlg.m_ofn.hwndOwner   = hParent;
   OpenDlg.m_ofn.hInstance   = NULL;

   OpenDlg.m_ofn.lpstrFilter = bImported ?
   	_T("AI\0*.ai\0CDR\0*.cdr\0CGM\0*.cgn\0DRW\0*.drw\0WMF\0*.wmf\0") :
   	_T("PPF\0*.ppf\0PP5\0*.pp5\0TIFF\0*.tif\0BMP\0*.bmp\0Targa\0*.tga\0GIF\0*.gif\0DIB\0*.dib\0PCX\0*.pcx\0JPEG\0*.jpg\0PCD\0*.pcd\0Scitec CT\0*.sct\0PP4\0*.pp4\0\0");

   OpenDlg.m_ofn.lpstrCustomFilter = NULL;
   OpenDlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT;
   
	bImported ?
   	RegCreateKeyEx( HKEY_CLASSES_ROOT, _T("ImportFileType"), 0, _T("Volatile"), REG_OPTION_NON_VOLATILE,	
   			KEY_ALL_ACCESS, NULL, &hKey, &dwNew ) :
   	RegCreateKeyEx( HKEY_CLASSES_ROOT, _T("OpenFileType"), 0, _T("Volatile"), REG_OPTION_NON_VOLATILE,	
   			KEY_ALL_ACCESS, NULL, &hKey, &dwNew );

   if( dwNew == REG_CREATED_NEW_KEY )
   	dwNew = 0;
   else
		bImported ?
   		RegQueryValueEx( HKEY_CLASSES_ROOT, _T("ImportFileType"), 0, &dwType, ( LPBYTE )&dwNew, &dwNewSize ) :
   		RegQueryValueEx( HKEY_CLASSES_ROOT, _T("OpenFileType"), 0, &dwType, ( LPBYTE )&dwNew, &dwNewSize );

   OpenDlg.m_ofn.nFilterIndex = ( int )dwNew;

	OpenDlg.m_ofn.lpstrInitialDir = OpenState.szPath;

	// Do actual Dialog
	while( iReturn == OpenState.idFileDialog || iReturn == OpenState.idThumbDialog )
		{
		if( iReturn == OpenState.idThumbDialog )
			{
			OpenState.ViewThumbnails = TRUE;
			iReturn = (int)AstralDlgParam(NO, PictPubApp.GetResourceHandle(),
				hParent, iReturn, DlgOpenProc, (long)&OpenState ) ;
			}
		else
			{
			OpenState.ViewThumbnails = FALSE;
			iCancelOK = OpenDlg.DoModal( );
			iReturn = OpenDlg.GetThumbState( );
			GetCurrentDirectory( sizeof( string ), string );
			lstrcat( lstrcpy( OpenState.szPath, string ), _T("\\") );
			}
		}

  	Browser.ViewThumbnails = OpenState.ViewThumbnails;
	
	if( !OpenDlg.GetThumbState( ) )
   	{													 
		if( iCancelOK == IDCANCEL )
 			return( FALSE );

		GetCurrentDirectory( sizeof( string ), string );
		lstrcpy( lpState->szPath, string );

		iReturn = IDOK;

		Browser.ViewThumbnails = FALSE;

   	// Save Last Selection
		bImported ?
   		RegSetValueEx( HKEY_CLASSES_ROOT, _T("ImportFileType"), 0, REG_DWORD, ( LPBYTE )&OpenDlg.m_ofn.nFilterIndex, sizeof( dwNew ) ) :
   		RegSetValueEx( HKEY_CLASSES_ROOT, _T("OpenFileType"), 0, REG_DWORD, ( LPBYTE )&OpenDlg.m_ofn.nFilterIndex, sizeof( dwNew ) );

		// setup return values
		lpState->lpFileListVolumes->OurMark = -1;
   	lpState->lpFileListVolumes->VolLabel = -1;
		OpenState.numFiles = 0;

		if( lstrlen( OpenDlg.m_ofn.lpstrFileTitle ) )
		{
   		lstrcpy( *( lpState->lpFileList ) , OpenDlg.m_ofn.lpstrFile );
			OpenState.numFiles++;
		}
		else
		{
			lstrcpyn( szPath, OpenDlg.m_ofn.lpstrFile, OpenDlg.m_ofn.nFileOffset );
			lstrcat( szPath, _T("\\" ));
			i = OpenDlg.m_ofn.nFileOffset;

			WORD wChar;
			LPTSTR psz;

			if( Control.WinVer != WV_WINDOWS95 )
			{
				while( TRUE )
				{
					psz = MyCharNextN( OpenDlg.m_ofn.lpstrFile, i );
					if( IsDBCSLeadByte(( BYTE )*psz ))
						wChar = *(( LPWORD )psz );
					else
						wChar = *psz;

					if( !wChar )
						break;

					j = 0;							 
					while( wChar )
					{
						if( wChar == _T(' ')) 
						{
							i++;
							break;
						}
						else
						{
							psz = MyCharNextN( szFile, j );
							if( IsDBCSLeadByte(( BYTE )wChar ))
								*(( LPWORD )psz ) = wChar;
							else
								*psz = wChar;
							i++; j++;
						}

						psz = MyCharNextN( OpenDlg.m_ofn.lpstrFile, i );

						if( IsDBCSLeadByte(( BYTE )*psz ))
							wChar = *(( LPWORD )psz );
						else
							wChar = *psz;
					}

					*MyCharNextN( szFile, j ) = _T('\0');

					lstrcpy( lpState->lpFileList[OpenState.numFiles], szPath );
					lstrcat( lpState->lpFileList[OpenState.numFiles], szFile );
					OpenState.numFiles++;
				}
			}
			else
			{
				while( lstrlen( &OpenDlg.m_ofn.lpstrFile[i] ) )
				{
					j = 0;							 
					while( szFile[j++] = OpenDlg.m_ofn.lpstrFile[i++] )
						;

					lstrcpy( lpState->lpFileList[OpenState.numFiles], szPath );
					lstrcat( lpState->lpFileList[OpenState.numFiles], szFile );
					OpenState.numFiles++;
				}
			}
		}
		lpState->numFiles = OpenState.numFiles;
   	lpState->idFileType =
			AppFileType( bImported ?
				arImportFileType[OpenDlg.m_ofn.nFilterIndex-1] :     
				arOpenFileType[OpenDlg.m_ofn.nFilterIndex-1] );     
		lpState->idDataType = OpenState.idDataType;
   	lpState->OpenMode = OpenDlg.GetOpenMode( ); 
		lpState->cmsInfo = OpenState.cmsInfo;
	}		   
	else
	{
		lpKeyHookBsr = NULL;
		CHDRIVE(drive);
		// save browser state
		if (iReturn == IDOK && OpenState.numFiles >= 1)
		{
			if (!OpenState.ViewAlbums)
				SetFileLocation(OpenState.idFileType, OpenState.lpFileList[0]);
			if (!OpenState.bSaving)
			{
	    		if( bImported )
					Browser.bsrImpFileType = OpenState.idFileType;
				else
					Browser.bsrFileType = OpenState.idFileType;
			}
			if( *OpenState.szAlbum != _T('\0'))
				lstrcpy(Browser.LastAlbum, OpenState.szAlbum);
		}	

		// always save this state
   		if( !bImported )
   			Browser.ViewAlbums = OpenState.ViewAlbums;

		// setup return values
		lpState->lpFileList = OpenState.lpFileList;
		lpState->lpFileListVolumes = OpenState.lpFileListVolumes;
		lpState->numFiles = OpenState.numFiles;
   	lpState->idFileType = AppFileType( OpenState.idFileType );     
		lpState->idDataType = OpenState.idDataType;
   	lpState->OpenMode = OpenState.OpenMode;
		lpState->cmsInfo = OpenState.cmsInfo;
   	}
	return(iReturn);		
}


//************************************************************************/
//	Brings up the mini open/save dlg.
//	To use fill an OPENSTATE structure and pass it in.
//	If the routine exits TRUE then the structure will contain the result.
//	This routine will update .INI entries. 
//************************************************************************/
BOOL DoMiniOpenDlg( HWND hParent, LPOPENSTATE lpState)
/************************************************************************/
{
	int iReturn;
	WORD type;
	BOOL OKtoSavePath;
	OPENSTATE OpenState;
	int drive;
	
	OKtoSavePath = Browser.OKtoSavePath;
	Browser.OKtoSavePath = FALSE;
	
	OpenState.ViewAlbums = FALSE;
	OpenState.ViewThumbnails = FALSE;
	OpenState.AutoThumbnails = FALSE;
	
	// setup defaults for mini 
	OpenState.bSaving = FALSE;
	OpenState.idDataType = IDC_SAVELA;
	OpenState.frameDepth = 1;
	if ((OpenState.wFileAttr = lpState->wFileAttr) == -1)
		OpenState.wFileAttr = DDL_READONLY;

    if( bImported )
        type = IDT_NOIMPORTFILETYPE;
    else
     	type = IDT_NOFILETYPE;

	OpenState.idFileType = type;
	if( *( lpState->szAlbum ) == _T('\0'))
	OpenState.bInitialized = FALSE;
	OpenState.OpenMode = OM_NORMAL;
	OpenState.bMultiListBox = FALSE;
	// setup passed in values
	if( *( lpState->szFileSpec ) == _T('\0'))
		lstrcpy( OpenState.szFileSpec, _T("*.*" ));
	else
		lstrcpy(OpenState.szFileSpec, lpState->szFileSpec);
	lstrcpy(OpenState.szNewFileSpec, OpenState.szFileSpec);
	lstrcpy(OpenState.szPath, lpState->szPath);
	lstrcpy(OpenState.szTitle, lpState->szTitle);
	lstrcpy(OpenState.szOK, lpState->szOK);

	OpenState.bExtended = FALSE;
   	OpenState.idThumbDialog = IDD_MINI_OPEN;
	OpenState.idFileDialog = IDD_MINI_OPEN;
	
	drive = CURRENTDRIVE;
	iReturn = (int)AstralDlgParam(NO, PictPubApp.GetResourceHandle(), hParent, IDD_MINI_OPEN, DlgOpenProc, (long)&OpenState);
	CHDRIVE(drive);

	lpKeyHookBsr = NULL;
	// setup return values
	lpState->lpFileList = OpenState.lpFileList;
	lpState->lpFileListVolumes = OpenState.lpFileListVolumes;
	lpState->numFiles = OpenState.numFiles;
	lpState->idFileType = AppFileType(OpenState.idFileType);
	lpState->idDataType = OpenState.idDataType;
	lpState->OpenMode = OpenState.OpenMode;

	Browser.OKtoSavePath = OKtoSavePath;
	
	return(iReturn);		
}

LOCAL void Browser_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange);
LOCAL BOOL Browser_OnInitDialog(HWND hDlg, HWND hDlgFocus, LPARAM lParam);
LOCAL void Browser_OnShowWindow(HWND hDlg, BOOL fShow, UINT status);
LOCAL void Browser_OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
LOCAL void Browser_OnActivateApp(HWND hwnd, BOOL fActivate, HTASK htaskActDeact);
LOCAL BOOL Browser_OnNCActivate(HWND hDlg, BOOL fActive, HWND hwndActDeact, BOOL fMinimized);
LOCAL void Browser_OnDestroy(HWND hDlg);

LOCAL void Browser_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR *lpDrawItem);
LOCAL void Browser_OnMeasureItem(HWND hDlg, LPMEASUREITEMSTRUCT lpMeasureItem);
LOCAL void Browser_OnClose(HWND hDlg);
LOCAL void Browser_OnInitMenu(HWND hDlg, HMENU hMenu);
LOCAL void Browser_OnCommand(HWND hDlg, int id, HWND hDlgCtl, UINT codeNotify);

			 
//************************************************************************
BOOL WINPROC EXPORT DlgOpenProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/************************************************************************/
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, Browser_OnInitDialog);  
//		HANDLE_DLGMSG(hDlg, WM_SHOWWINDOW, Browser_OnShowWindow);
		HANDLE_DLGMSG(hDlg, WM_MEASUREITEM, Browser_OnMeasureItem);
		HANDLE_DLGMSG(hDlg, WM_ACTIVATE, Browser_OnActivate);
// CALFIX      HANDLE_DLGMSG(hDlg, WM_ACTIVATEAPP, Browser_OnActivateApp);
		HANDLE_DLGRET(hDlg, WM_NCACTIVATE, Browser_OnNCActivate);  
		HANDLE_DLGMSG(hDlg, WM_DRAWITEM, Browser_OnDrawItem);
		HANDLE_DLGMSG(hDlg, WM_CLOSE, Browser_OnClose);
		HANDLE_DLGMSG(hDlg, WM_INITMENU, Browser_OnInitMenu);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, Browser_OnCommand);
		HANDLE_DLGRET(hDlg, WM_DESTROY, Browser_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_PALETTECHANGED, Browser_OnPaletteChanged);

	    default:
		{
			lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
			if (lpBsr)
				return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, THMMODE ? lpBsr->OpenState.idThumbDialog:  lpBsr->OpenState.idFileDialog);
			else
				return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MINI_OPEN);
		}
	}  
}

/***********************************************************************/
LOCAL void Browser_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange)
/***********************************************************************/
{
	if (IsOurWindow(hwndPaletteChange))
	{
		lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
		if (lpBsr)
		{
	 		ITEMID idFileList = FILELISTID;
			HWND hFileList = GetDlgItem(hDlg, idFileList);
			if (hFileList)
				InvalidateRect(hFileList, NULL, TRUE);
		}
	}
}

//************************************************************************
LOCAL BOOL Browser_OnInitDialog(HWND hDlg, HWND hDlgFocus, LPARAM lParam)
//************************************************************************
{
	SetWindowLong(hDlg, DWL_USER, 0L);
	if (!lParam)
		return(TRUE);
	lpBsr = GetBsrStateStruct();
	if (!lpBsr)
		return(TRUE);
	lpBsr->OpenState = *(LPOPENSTATE)lParam;
	lpBsr->lpOpenState = (LPOPENSTATE)lParam;
	SetWindowLong(hDlg, DWL_USER, (LONG)lpBsr);

	// Color Manager - Enable/Disable CMS button
    if( Control.Retail )
    	EnableWindow( GetDlgItem( hDlg, IDC_CMS ), Control.CMSEnabled );
  	EnableWindow( GetDlgItem( hDlg, IDC_BSR_NETWORK ), YES );
	
	CenterPopup(hDlg);
	BrowserDlgBegin(hDlg);
	if (!lpBsr)
		AstralDlgEnd( hDlg, FALSE);
//	CalculateOpenDlgSize(hDlg);
	// show proper parts of dlg and resize
	ShowAlbumWindow(hDlg, lpBsr->OpenState.ViewAlbums);
//	if (THMMODE)
//	{
//		lpBsr->OpenState.ViewThumbnails = !lpBsr->OpenState.ViewThumbnails;
//		ResizeOpenDlg(hDlg, TRUE);
//	}
	SetupItems(hDlg);
	FillItems(hDlg, YES, NO);
	return( FALSE );
}
		
//************************************************************************
LOCAL void Browser_OnShowWindow(HWND hDlg, BOOL fShow, UINT status)
//***********************************       *************************************
{
//	HWND hWindow;
//	if (!fShow)
//		return;
//	hWindow = GetDlgItem(hDlg, IDC_BSR_FILENAME);
//	if (hWindow)
//	{
//		SetFocus(hWindow);
//		SendMessage(hWindow, EM_SETSEL, 0, MAKELONG(0,-1));
//	}
}		

/***************************************************************************/
LOCAL void Browser_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	// clean up
	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return;
	clr_BSRkey_hook();
	SetWindowLong(hDlg, DWL_USER, 0L);
	BrowserDlgEnd(hDlg);
	if (lpBsr->lpOpenState)
		*lpBsr->lpOpenState = lpBsr->OpenState;
	FreeBsrStateStruct(lpBsr);
	lpBsr = NULL;
}
		
//************************************************************************
LOCAL void Browser_OnActivate(HWND hDlg, UINT state, HWND hwndActDeact, BOOL fMinimized)
//************************************************************************
{

	// get the global state variables for the given dialog.
	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return;
	
	if (state == WA_ACTIVE || state == WA_CLICKACTIVE)
		set_BSRkey_hook( lpBsr);
	else if (state == WA_INACTIVE)// disable
		clr_BSRkey_hook();
}
	
//************************************************************************
LOCAL void Browser_OnActivateApp(HWND hDlg, BOOL fActivate, HTASK htaskActDeact)
//************************************************************************
{
	// get the global state variables for the given dialog.
	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return;
	
	if (fActivate)
		set_BSRkey_hook( lpBsr);
	else 
		clr_BSRkey_hook();
}

//************************************************************************
LOCAL BOOL Browser_OnNCActivate(HWND hDlg, BOOL fActive, HWND hwndActDeact, BOOL fMinimized)
//************************************************************************
{
	// get the global state variables for the given dialog.
	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return(FALSE);
	
	if (fActive)
		set_BSRkey_hook( lpBsr);
	else 
		clr_BSRkey_hook();
	return(FALSE);
}		

//************************************************************************
LOCAL void Browser_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR *lpDrawItem)
//************************************************************************
{
	// get the global state variables for the given dialog.
 	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return;

	switch (lpDrawItem->CtlID)
	{
		case IDC_BSR_DIRECTORIES:
			DrawItem( hDlg, lpDrawItem, OD_LOWERCASE );
		break;
		case IDC_BSR_FILELIST:
			FileListDraw(hDlg, (LPDRAWITEMSTRUCT)lpDrawItem); 
		break;
		case IDC_BSR_THUMBS:
			DrawThumb(hDlg, (LPDRAWITEMSTRUCT)lpDrawItem); 
			SAFECANCEL;
		break;
		default:
			DrawItem( hDlg, lpDrawItem, 0 );
	}
}

//************************************************************************
LOCAL void Browser_OnMeasureItem(HWND hDlg, LPMEASUREITEMSTRUCT lpMeasureItem)
//************************************************************************
{
	switch (lpMeasureItem->CtlID)
	{
		case IDC_BSR_DRIVE:
		case IDC_BSR_DIRECTORIES:
 		case IDC_BSR_ALBUMS:
			MeasureItem( hDlg, lpMeasureItem, YES );
		break;
		case IDC_BSR_FILETYPE:
		case IDC_BSR_DATATYPE:
		case IDC_BSR_OPENMODE:
			MeasureItem( hDlg, lpMeasureItem, NO );
		break;
		case IDC_BSR_FILELIST:
			FileListMeasure(hDlg,lpMeasureItem); 
		break;
		case IDC_BSR_THUMBS:
			MeasureThumb(hDlg,lpMeasureItem); 
		break;
	}
}

//************************************************************************
LOCAL void Browser_OnClose(HWND hDlg)
//************************************************************************
{
	// get the global state variables for the given dialog.
 	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return;
	
	AstralDlgEnd( hDlg, FALSE);
}

//************************************************************************
LOCAL void Browser_OnInitMenu(HWND hDlg, HMENU hMenu)
//************************************************************************
{
#define STATE(bool) ((bool) ? MF_ENABLED : MF_GRAYED)
	
	BOOL   bAlbMode, bOneFile, bFiles, bAlb, bAlbSel, bDirSel;		
//	HANDLE hMenu;
	int 	num;
	LPTSTR	lpString;
	
	// get the global state variables for the given dialog.
 	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return;
		
	num = ParseListBoxMessage(lpBsr->OpenState.bMultiListBox, 
				FILELIST(hDlg), LB_GETSELCOUNT, 0, 0L);
	bAlbMode = ALBMODE;
	bOneFile = (num == 1); 
	bFiles = (num > 0);
	if (bAlbMode)
		bAlbSel = (ListBox_GetCurSel(ALBLIST(hDlg)) != LB_ERR);
	else 
		bAlbSel = FALSE;
	bAlb = (GetAlb()!=NULL) ? TRUE:FALSE;
	if (!bAlbMode)
		bDirSel = (ListBox_GetCurSel(GetDlgItem(DIRWINDOW(hDlg), IDC_BSR_DIRECTORIES)) != LB_ERR);
	else 
		bDirSel = FALSE;
  		
	EnableMenuItem(hMenu, IDM_BSR_RENAME, STATE(bOneFile));
	EnableMenuItem(hMenu, IDM_BSR_FIND, STATE(bOneFile && bAlbMode));
	EnableMenuItem(hMenu, IDM_BSR_REMOVEFROMALB, STATE(bFiles && bAlbMode));
	EnableMenuItem(hMenu, IDM_BSR_ADDTOALB, STATE(bFiles));
	EnableMenuItem(hMenu, IDM_BSR_CONVERT, STATE(bFiles && FALSE));
	EnableMenuItem(hMenu, IDM_BSR_DIRIMPORT, STATE(!bAlbMode));
	EnableMenuItem(hMenu, IDM_BSR_DIREXPORT, STATE(!bAlbMode));
	EnableMenuItem(hMenu, IDM_BSR_RENAMEDIR, STATE(bDirSel));
	EnableMenuItem(hMenu, IDM_BSR_DELETEDIR, STATE(bDirSel));
	EnableMenuItem(hMenu, IDM_BSR_UNDO, STATE(FALSE));
	EnableMenuItem(hMenu, IDM_BSR_ALBRENAME, STATE(bAlbSel));
	EnableMenuItem(hMenu, IDM_BSR_ALBDELETE, STATE(bAlbSel));
	EnableMenuItem(hMenu, IDM_BSR_ALBINFO, STATE(bAlbSel));
//	if (AstralStr(bAlbMode ? IDS_VIEWDIR : IDS_VIEWALB, &lpString))
//		ModifyMenu( hMenu, IDM_BSR_ALBVIEW, MF_BYCOMMAND | MF_STRING, IDM_BSR_ALBVIEW, lpString );
	if (AstralStr(/*THMMODE ? */ IDS_VIEWFILES/* : IDS_VIEWTHUMBS*/, &lpString))
		ModifyMenu( hMenu, IDM_BSR_VIEWTHUMBS, MF_BYCOMMAND | MF_STRING, IDM_BSR_VIEWTHUMBS, lpString );
	EnableMenuItem(hMenu, IDM_BSR_THMCREATE, STATE(bFiles));
	EnableMenuItem(hMenu, IDM_BSR_THMUPDATE, STATE(bFiles));
	EnableMenuItem(hMenu, IDM_BSR_PRINTTHUMBS, STATE(bFiles));
	EnableMenuItem(hMenu, IDM_BSR_THMDEL, STATE(bFiles && !AUTOTHUMBS));
	EnableMenuItem(hMenu, IDM_BSR_THMMOVE, STATE(bFiles));
//	EnableMenuItem(hMenu, IDM_BSR_, STATE());
//	Should normally be available;
//	EnableMenuItem(hMenu, IDM_BSR_SEARCH, STATE(FALSE));
//	EnableMenuItem(hMenu, IDM_BSR_, STATE());
}
		
//************************************************************************
LOCAL void Browser_OnCommand(HWND hDlg, int id, HWND hDlgCtl, UINT codeNotify) 
//************************************************************************
{
	BOOL retc;

	// get the global state variables for the given dialog.
 	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return;

	switch(id)
	{
		case IDC_BSR_NETWORK:
			if( WNetConnectionDialog( hDlg, RESOURCETYPE_DISK ) == NO_ERROR )
				{
				FreeUpDriveInfoMem( );
				SetupDriveInfoMem( YES );
				FillDrivesCombo( hDlg );
				}
		break;
		case IDC_BSR_DRIVE:
			HandleDrives(hDlg, id, hDlgCtl, codeNotify);   
		break;
		case IDC_BSR_DIRECTORIES:
			HandleDirectories(hDlg, id, hDlgCtl, codeNotify);   
		break;
		case IDC_BSR_THUMBS:
		case IDC_BSR_FILELIST:
			HandleFileList(hDlg, id, hDlgCtl, codeNotify);
		break;
		case IDC_BSR_ALBUMCOMBO:
			HandleAlbumCombo(hDlg, id, hDlgCtl, codeNotify);
		break;
		case IDC_BSR_ALBUMS:
			HandleAlbumList(hDlg, id, hDlgCtl, codeNotify);
		break;
		case IDC_BSR_FILETYPE:
			HandleFileType(hDlg, id, hDlgCtl, codeNotify);
		break;
		case IDC_BSR_DATATYPE:
			HandleDataType(hDlg, id, hDlgCtl, codeNotify);
		break;
		case IDC_BSR_OPENMODE:
            if( Control.Retail )
    			HandleOpenMode(hDlg, id, hDlgCtl, codeNotify );
		break;
		case IDC_BSR_FILENAME:
			HandleFileName(hDlg, id, hDlgCtl, codeNotify);
		break;
		
		case IDC_BSR_PATH_TEXT:
//			if (ALBMODE)
//				break;
//			wMsg = HIWORD(lParam);
//			if ( wMsg == BN_DOUBLECLICKED )
//			{
//				if (!GetDlgItemText(FILEWINDOW(hDlg), IDC_BSR_FILENAME, 
//						szDir, sizeof(szDir)))
//				{
//					MessageBeep(MB_ICONASTERISK);
//					break;
//				}				
//				if (*filename(szDir))
//				{
//					lstrcpy(lpBsr->OpenState.szSearchSpec, filename(szDir));
//					SetFileNameText(hDlg, filename(szDir), TRUE);
//				}
//				if (!ChangeDir(hDlg, stripfile(szDir), NO))
//					FillItems(hDlg, YES, NO);
//				 break;
//			}
//			if ( wMsg != BN_CLICKED )
//				break;
//			// new file name text
// 			GetDlgItemText(hDlg, wParam, szDir, sizeof(szDir));
//			if ( !szDir[0] )
//				break;
//			FixPath( szDir );
//			SetUpSearchSpec(hDlg, szDir);
//			HandleFileName(hDlg, 0, MAKELONG(0, EN_CHANGE));
//		break;

		case IDM_BSR_VIEWTHUMBS:
			lpBsr->OpenState.ViewThumbnails = FALSE; //!lpBsr->OpenState.ViewThumbnails;
			AstralDlgEnd( hDlg, THMMODE ? lpBsr->OpenState.idThumbDialog:  lpBsr->OpenState.idFileDialog);
		break;
		case IDM_BSR_ALBVIEW:
			lpBsr->OpenState.ViewAlbums = !lpBsr->OpenState.ViewAlbums;
			SetDirectoryFromDef(hDlg);
			FillItems(hDlg, YES, NO);
			ShowAlbumWindow(hDlg, lpBsr->OpenState.ViewAlbums);
		break;
																			                     
		case IDM_BSR_SEARCH:
			Search(hDlg);
		break;
		case IDM_BSR_INFO:
			if (OpenFileList(hDlg) > 0)
			{
				EnableWindow(hDlg, FALSE);
				AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_BSR_FILEINFO, DlgBsrFileInfoProc );
				EnableWindow(hDlg, TRUE);
				SetFocus(hDlg);
			}
		break;
		case IDC_BSR_COMPRESS:
			EnableWindow(hDlg, FALSE);
				SetupSaveOptions(AppFileType(lpBsr->OpenState.idFileType), YES);
			EnableWindow(hDlg, TRUE);
			SetFocus(hDlg);
		break;
		case IDC_STATUSINFO:
			if (OpenFileList(hDlg) > 0)
			{
				EnableWindow(hDlg, FALSE);
				AstralDlg(NO, PictPubApp.GetResourceHandle(),hDlg, IDD_BSR_FILEINFO, DlgBsrFileInfoProc );
				EnableWindow(hDlg, TRUE);
				SetFocus(hDlg);
				break;
			}
			// Otherwise Fall through to IDM_BSR_ALBINFO
		case IDM_BSR_ALBINFO:
			if (!ALBMODE)
				return;
			EnableWindow(hDlg, FALSE);
			AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_BSR_ALBINFO, DlgBsrAlbInfoProc );
			EnableWindow(hDlg, TRUE);
			SetFocus(hDlg);
		break;

        // Color Manager Stuff
		case IDC_CMS:
            if( Control.Retail )
            {
				CCmsFileOpenDlg	OpenDlg( &lpBsr->OpenState.cmsInfo, CWnd::FromHandle( hDlg ) );
				OpenDlg.DoModal();
            }
		break;
		case IDM_BSR_DELETE:
			DeleteFiles(hDlg);	 
		break;
		case IDM_BSR_FIND:
			GoToFile(hDlg);
		break;
		case IDM_BSR_RENAME:
			RenameFiles(hDlg);
		break;
		case IDM_BSR_COPY:
			CopyFiles(hDlg);
		break;
		case IDM_BSR_MOVE:
			MoveFiles(hDlg);
		break;
 		case IDM_BSR_REMOVEFROMALB:
			RemoveFilesFromAlb(hDlg);
		break;
		case IDM_BSR_ALBDELETE:
			(DeleteAlbum(hDlg, NULL, YES));
		break;
		case IDM_BSR_ALBRENAME:
			(RenameAlbum(hDlg, NULL, YES));
		break;
		case IDM_BSR_ALBNEW:
			(NewAlbum(hDlg, YES));
		break;
		case IDM_BSR_ALBEXPORT:								  
			AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_ALBEXPORT, DlgAlbumExportProc);
			FillItems(hDlg, YES, YES);
		break;
		case IDM_BSR_ALBIMPORT:
			AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_ALBIMPORT, DlgAlbumImportProc);
			FillItems(hDlg, YES, YES);
		break;
		case IDM_BSR_DELETEDIR:
			(DeleteDir(hDlg));
		break;
		case IDM_BSR_RENAMEDIR:
			(RenameDir(hDlg));
		break;
		case IDM_BSR_CREATEDIR:
			(CreateDir(hDlg));
		break;
		case IDM_BSR_SELECTALL:
			(SelectFiles(hDlg, YES));
		break;
		case IDM_BSR_DIREXPORT:
			EnableWindow(hDlg, FALSE);
			AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_DIREXPORT,  DlgDirExportProc);
			EnableWindow(hDlg, TRUE);
			SetFocus(hDlg);
		break;
		case IDM_BSR_DIRIMPORT:
			EnableWindow(hDlg, FALSE);
			AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_DIRIMPORT,  DlgDirImportProc);
			EnableWindow(hDlg, TRUE);
			SetFocus(hDlg);
			*( lpBsr->szLastDirDBF ) = _T('\0');
			FillItems(hDlg, NO, NO);
		break;
		case IDM_BSR_SELECTNONE:
			SelectFiles(hDlg, NO);
		break;
		case IDM_BSR_INVERTSELECT:
			InvertSelectFiles(hDlg);
		break;
		case IDM_BSR_ADDTOALB:
			MoveFilesToAlb(hDlg);
		break;
		case IDM_BSR_PREFERENCES:
			retc = Browser.AutoThumbnails;
			AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_BSR_PREFS, DlgBsrPrefProc );
			if (!retc && Browser.AutoThumbnails)
				FillFileList(hDlg, YES);
		break;
		case IDM_BSR_THMCREATE:
			if (SHIFT)
				UpdateFileThumbs(hDlg, VL_EXIST | VL_CLEAN | VL_ESCAPABLE);
			else
				UpdateFileThumbs(hDlg, VL_EXIST | VL_CLEAN | VL_NOTIME | VL_ESCAPABLE);
			FillItems(hDlg, NO, YES);
			SAFECANCEL;
		break;
		case IDM_BSR_THMUPDATE:
			if (SHIFT)
				UpdateFileThumbs(hDlg, VL_EXIST | VL_CLEAN | VL_NOTIME | VL_ESCAPABLE);
			else
				UpdateFileThumbs(hDlg, VL_EXIST | VL_CLEAN | VL_ESCAPABLE);
			FillItems(hDlg, NO, YES);
			SAFECANCEL;
		break;
		case IDM_BSR_THMDEL:
			DeleteFileThumbs(hDlg);
			FillItems(hDlg, NO, YES);
		break;
		case IDM_BSR_PRINTTHUMBS:
			AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_PRINT_THUMBS, DlgPrintThumbsProc);
		break;
		case IDM_BSR_CLEANUP:
			CleanupThumbs(hDlg);
		break;
		case IDM_BSR_THMMOVE:
			EnableWindow(hDlg, FALSE);
			OpenFileList(hDlg);
			AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_THMMOVE,  DlgThumbMoveProc);
			EnableWindow(hDlg, TRUE);
			SetFocus(hDlg);
			FillItems(hDlg, NO, YES);
		break;
	    case IDOK:
		case IDC_OK:
			HandleOK(hDlg);
		break;
	    case IDCANCEL:
			if (lpBsr->bDisableCancel)
			{
			 	lpBsr->bDisableCancel = FALSE;
			 	break;
			}
		// Fall through
	    case IDC_CANCEL:
			AstralDlgEnd( hDlg, FALSE);
		break;
	    default:
		break;
	}	// switch WM_COMMAND
}	    

//************************************************************************
// call after a file has been saved/changed to updtae the thumb.
//************************************************************************
void UpdateThumbnail( LPTSTR lpFile, LPFRAME lpFrame, int width, int height, 
					  LONG dataType, LONG fileType )
//************************************************************************
{
	FILELOC File;
	WORD wChar;
	ENTRY entry;
	LPCTHMNAIL lpThm = NULL;
	BSRSTATE BsrState;
	
	// get the global state variables.
 	lpBsr = &BsrState;
	
	// setup structures for volume and drive type stuff
	if( !SetupDriveInfoMem( FALSE ))
	{
	 	lpBsr = NULL;
		return;
	}

	for( wChar = 0; wChar < 26; wChar++ )
		BSRSetDriveType( wChar, DRIVE_FIXED );

	// get drive type only for the drive saving to
	if( IsDBCSLeadByte(( BYTE )*lpFile ))
		wChar = *(( LPWORD )lpFile );
	else
		wChar = *lpFile;

	if( WITHIN( wChar, _T('a'),_T('z')))
		wChar -= _T('a');
	else 
	if( WITHIN( wChar, _T('A'), _T('Z')))
		wChar -= 'A';

	BSRSetDriveType( wChar, GetExtendedDriveType( wChar ));
	BSRInvalidateDriveIDs();
	
	lstrcpy( File.szFile, lpFile );
//	Lowercase(File.szFile);

	// get volume
	DriveVolume( wChar ,&File.volume, YES );

	// Find or create the thumb
	wChar = FindOrCreateThumb( &File, &entry, NO );
	if( wChar == BSR_ERROR )
		goto Exit;

	if( !( lpThm = CTHMOpen( &entry )))
		goto Exit;

   	// re-create file size etc
   	CTHMReCreateEasyInfo( lpThm, &entry.File, YES );

	// add the frame
	CTHMPutFrame( lpThm, lpFrame, width, height, dataType, fileType, 
				  lpThm->thm.info.time );
Exit:
	if( lpThm )
		CTHMClose( lpThm );	

	FreeUpDriveInfoMem();
 	lpBsr = NULL;
}

//************************************************************************/
//	Called only by routines outside the browser.  Checks lpVolume against
//	the volume for drive indicated by lpFile.
//	lpBoolRemovable is set to true if the drive is removable (could be cahnged).
//************************************************************************/
BOOL NonBSRVolumeCheck(LPTSTR lpFile, LPVOLUMEID lpVolume, LPINT lpBoolRemovable)
//************************************************************************/
{
	WORD wType;
	FILELOC File;
	BOOL bRet;
	BSRSTATE BsrState;
	
	// get the global state variables.
 	lpBsr = &BsrState;
	
	if( !SetupDriveInfoMem( FALSE ))
	{
	 	lpBsr = NULL;
		return( TRUE );
	}
	for( wType = 0; wType < 26; wType++ )
		BSRSetDriveType( wType, DRIVE_FIXED );

	// get drive type only for the drive saving to
	if( IsDBCSLeadByte(( BYTE )*lpFile ))
		wType = *(( LPWORD )lpFile );
	else
		wType = *lpFile;

	if( WITHIN( wType , _T('a'), _T('z')))
		wType -= _T('a');
	else 
	if( WITHIN( wType, _T('A'), _T('Z')))
		wType -= _T('A');

	wType = GetExtendedDriveType( wType );

	if( lpBoolRemovable )
		*lpBoolRemovable = ( wType == DRIVE_REMOVABLE || wType == DRIVE_CDROM );

	BSRSetDriveType( wType, wType );
	BSRInvalidateDriveIDs();
	lstrcpy( File.szFile, lpFile );
	File.volume = *lpVolume;
	bRet = VolumeCheck(&File, NULL);
	FreeUpDriveInfoMem();
 	lpBsr = NULL;
	return( bRet );
}

//************************************************************************
// 		NonBSRAddToAlbum
// PARAMETERS:
//			LPTSTR lpFile- The file to add.
//		 	LPTSTR lpAlbum- The album to add it to.
// DESCRIPTION:
//			Should be called only from NON BROWSER code.
//			Add the given file to the given album.
//			Uses existing thumb, if there is one.
//			Creates a 'minimal' thumb if there is not already a thumbnail.
// RETURNS:
//			FALSE if there is a failure.
//************************************************************************
BOOL NonBSRAddToAlbum(LPTSTR lpFile, LPTSTR lpAlbum)
//************************************************************************
{
	BSRSTATE BsrState;
	LPDBFLL lpDBFLL;
	FILELOC File;
	BOOL bRet = FALSE;
	
	// get the global state variables.
 	lpBsr = &BsrState;
	lpBsr->OpenState.bExtended = FALSE;  // album, stuff needed
	if  (!BrowserDlgBegin(NULL))
		goto Exit;
	
	// Setup File name
	lstrcpy(File.szFile, lpFile);
	GetDriveVolume(File.szFile, &File.volume, NO);
	
	// find the given album
	lpDBFLL = FindAlbum(lpAlbum);
	if (!lpDBFLL)
		goto Exit;
	// Do the actual ADD 
	if(AddToAlbum(&(lpDBFLL->dbf), &File, MAYBE))
		bRet = TRUE;
Exit:
	// clean up
	BrowserDlgEnd(NULL);
 	lpBsr = NULL;
	return(bRet);
}

//************************************************************************
// 		NonBSREnumAlbum
// PARAMETERS:
//			LPENUMALBUMPROC lpCallbackProc- The callback (see browser.h)
// DESCRIPTION:
//			Should be called only from NON BROWSER code.
//			Calls the callback function for every album.
//			Stops when all albums are processed, or callback returns FALSE.
// RETURNS:
//			The number of Albums processed.
//			If lpCallbackProc==NULL then returns the total number of albums.
//************************************************9************************
int NonBSREnumAlbum(LPENUMALBUMPROC lpCallbackProc, long lData)
//************************************************************************
{
	BSRSTATE BsrState;
	LPDBFLL lpAlbum;
	FNAME szAlbum;
	int num, i = 0;
	
	// get the global state variables.
	// only used for Album list stuff...lpAlbList. 
 	lpBsr = &BsrState;
	PrepareAlbList();
	num = CreateAlbList();
	if (!lpCallbackProc)
		goto Exit;
	
	// enumerate albums
	lpAlbum = FirstAlb();
	while (lpAlbum)
	{
		lstrcpy(szAlbum, lpAlbum->dbf.info.szName);
		if (!(*lpCallbackProc)(szAlbum, lData))
			break;
		lpAlbum = GetNextAlb();
		i++;
	}
	// clean up
Exit:
	FreeAlbList();
 	lpBsr = NULL;
	return(i);
}

#ifdef UNUSED
//************************************************************************/
// sets up size for the different states of the dialog
//************************************************************************/
LOCAL void CalculateOpenDlgSize(HWND hDlg)
//************************************************************************/
{
	HWND hFiles, hThumbs;
	RECT rThumbs, rFiles, rDialog;
	int cxScreen, width, height;

	// size thumbnail listbox
	cxScreen = GetSystemMetrics(SM_CXFULLSCREEN);
	hThumbs = GetDlgItem(hDlg, IDC_BSR_THUMBS);
	hFiles = GetDlgItem(hDlg, IDC_BSR_FILELIST);
	GetWindowRect(hDlg, &rDialog);
	GetWindowRect(hThumbs, &rThumbs);
	GetWindowRect(hFiles, &rFiles);
	
	
	if (cxScreen > 1100)
		width = (cxScreen*3)/5;
	else if (cxScreen > 1000)
		width = (cxScreen*2)/3;
	else if (cxScreen > 750)
		width = (cxScreen*3)/4;
	else 
		width = (cxScreen*5)/6;

	width = width - RectWidth(&rDialog) + RectWidth(&rFiles);
	height = lpBsr->ThumbYSize*3 + GetSystemMetrics(SM_CYHSCROLL) + 1;
	// this control controls the width of the thumbnail list box

	SetWindowPos(hThumbs, NULL,0,0, width, height, 
						SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
}

#endif


//************************************************************************/
// 	Shows files or thumbs and resizes according to 
// 		sizes from CalcualteOpenDlgSize
//************************************************************************/
LOCAL void ResizeOpenDlg(HWND hDlg, BOOL bViewThumbs)
//************************************************************************/
{
	HWND hFiles, hThumbs;
	RECT rThumbs, rFiles, rDialog;
	int dx, dy;	
	
	if (THMMODE && bViewThumbs || !THMMODE && !bViewThumbs)
		return;

	hThumbs = GetDlgItem(hDlg, IDC_BSR_THUMBS);
	hFiles = GetDlgItem(hDlg, IDC_BSR_FILELIST);
	GetWindowRect(hDlg, &rDialog);
	GetWindowRect(hThumbs, &rThumbs);
	GetWindowRect(hFiles, &rFiles);

	dx = RectWidth(&rThumbs) - RectWidth(&rFiles);
	dy = RectHeight(&rThumbs) - RectHeight(&rFiles);
	if (!bViewThumbs)
	{
		dx = -dx;
		dy = -dy;
	}

	// move right hand controls
	MovePane(hDlg, IDC_BSR_DIRPANE1, IDC_BSR_DIRPANE2, dx,0);

	// Move Open/Save buttons
	MovePane(hDlg, IDC_BSR_TEMPPANE1, IDC_BSR_TEMPPANE2, 0,dy);

	
	// show/hide proper items 
	ShowDlgItem(hDlg, IDC_BSR_THUMBS, bViewThumbs );
	ShowDlgItem(hDlg, IDC_BSR_FILELIST, !bViewThumbs );

	lpBsr->OpenState.ViewThumbnails = bViewThumbs;

	// Resize Dialog
	MoveWindow(hDlg, rDialog.left, rDialog.top, RectWidth(&rDialog)+dx-1, RectHeight(&rDialog)+dy-1, YES);
}

#ifdef UNUSED
//************************************************************************/
// 	Shows files or thumbs and resizes according to 
// 		sizes from CalcualteOpenDlgSize
//************************************************************************/
LOCAL void ViewThumbs(HWND hDlg, BOOL bViewThumbs)
//************************************************************************/
{
	HWND hItem1, hItem2, hNext;
	HWND hParent, hListBox;
	HWND hNewDlg = NULL;
	BOOL bOldView;
	ITEMID  id1, id2;
	int  idNewDlg;
	RECT rect;
	
	if (lpBsr->OpenState.ViewThumbnails == bViewThumbs)
		return;
	bOldView = lpBsr->OpenState.ViewThumbnails;
	
	// get the new dialog
	idNewDlg = bViewThumbs ? lpBsr->OpenState.idThumbDialog : lpBsr->OpenState.idFileDialog;
	hParent = GetParent(hDlg);
	if (!hParent)
		return;

	hNewDlg = CreateDialogParam( PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(idNewDlg), hParent ? hParent:hDlg, DlgOpenProc, NULL);
	if (!hNewDlg)
		return; 
	
	// copy the file list
	hListBox = FILELIST(hDlg);
	SetParent(hListBox, hNewDlg);
	lpBsr->OpenState.ViewThumbnails = bViewThumbs;
	
	// Clear out any elements NOT in hNewDlg 
	// Also Clear any -1 elements
	hItem1 = GetTopWindow(hDlg);
	while (hItem1)
	{
		id1 = GetDlgCtrlID(hItem1);
		if (id1 && id1 <= SHRT_MAX)
		{
			hItem2 = GetTopWindow(hNewDlg);
			id2 = SHRT_MAX+1;
			while(hItem2)
			{
				id2 = GetDlgCtrlID(hItem2);
				if (id1 == id2)
					break;
				hItem2 = GetNextWindow(hItem2, GW_HWNDNEXT);
			}
			if (id1 == id2)
			{
				// This Item is OK!
				hItem1 = GetNextWindow(hItem1, GW_HWNDNEXT);
				continue;
			}
		}
		// the item was NOT ok.. so delete it
		hNext = GetNextWindow(hItem1, GW_HWNDNEXT);
		DestroyWindow(hItem1);
		hItem1 = hNext;
	}
	
	// Move every element in hNewDlg to hDlg that is not already in hDlg.
	// Position all remaining elements.
	hItem1 = GetTopWindow(hNewDlg);
	while (hItem1)
	{
		id1 = GetDlgCtrlID(hItem1);
		if (id1 && id1 <= SHRT_MAX)
		{
			hItem2 = GetTopWindow(hDlg);
			id2 = SHRT_MAX+1;
			while(hItem2)
			{
				id2 = GetDlgCtrlID(hItem2);
				if (id1 == id2)
					break;
				hItem2 = GetNextWindow(hItem2, GW_HWNDNEXT);
			}
			if (id1 == id2)
			{
				// This item in hDlg Should just be moved!
				GetWindowRect(hItem1, &rect);
				ScreenToClient(hNewDlg, (LPPOINT)&rect.left);
				ScreenToClient(hNewDlg, (LPPOINT)&rect.right);
				SetWindowPos(hItem2, 
					HWND_BOTTOM, 
					rect.left,
					rect.top,
					rect.right - rect.left,
					rect.bottom - rect.top,
					SWP_NOREDRAW); 
				// Now destroy it from hNewDlg!
				hNext = GetNextWindow(hItem1, GW_HWNDNEXT);
				DestroyWindow(hItem1);
				hItem1 = hNext;
				continue;
			}
		}
		// the item was NOT in hDlg so add it
		hNext = GetNextWindow(hItem1, GW_HWNDNEXT);
	   	SetParent(hItem1, hDlg);
//		SetWindowPos(hItem1, 
//			HWND_BOTTOM, 
//			0,
//			0,
//			0,
//			0,
//			SWP_NOREDRAW|SWP_NOMOVE|SWP_NOSIZE);
		hItem1 = hNext;
	}
	// Resize the entire window
	GetWindowRect(hNewDlg, &rect);
	if (hParent)
	{
		ScreenToClient(hParent, (LPPOINT)&rect.left);
		ScreenToClient(hParent, (LPPOINT)&rect.right);
	}
	SetWindowPos(hDlg, 
		0, 
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		SWP_NOMOVE|SWP_NOZORDER);
		
	hItem2 = FILELIST(hDlg);
	hItem1 = CopyWindowEx(hItem2, hDlg);
	DestroyWindow(hItem2);
   	if (!CopyLB(hItem1, hListBox))
		goto Error;
	DestroyWindow(hListBox);
		
	// Clean Up
	DestroyWindow(hNewDlg);
	
	// get needed windows
//	GetDlgItemText(hDlg, IDC_BSR_FILENAME, szString, sizeof(szString));
//	SetFileNameText(hDlg, szString, YES);
//	FillFileType(hDlg);
//	FillDataType(hDlg);
//	EnableDataTypes(hDlg);
//	EnableCompress(hDlg);
	return;

Error:
	lpBsr->OpenState.ViewThumbnails = bOldView;
	if (hNewDlg)
		DestroyWindow(hNewDlg);
	FillItems(hDlg, NO, NO);
}
#endif //UNUSED

//************************************************************************
//************************************************************************
LOCAL void set_BSRkey_hook( LPBSRSTATE lpBsr)
//************************************************************************
{
#ifndef WIN32
	if (!lpKeyHookBsr)
	{
		// Set up keyboard hook
		KeyProc =  (HOOKPROC)MakeProcInstance((FARPROC)BrowserKeyHookProc, PictPubApp.Get_hInstAstral());
		prevKeyHook =  SetWindowsHookEx(WH_KEYBOARD,
		                            KeyProc,
		                            PictPubApp.Get_hInstAstral(),
		                            GetCurrentTask());
   		lpKeyHookBsr = lpBsr;									
	}
#endif
}

//************************************************************************
LOCAL VOID clr_BSRkey_hook(void)
//************************************************************************
{
#ifndef WIN32
	    // Release keyboard hook
		lpKeyHookBsr = NULL;
	    if (prevKeyHook)
	        UnhookWindowsHookEx(prevKeyHook);
		prevKeyHook = NULL;
	    if (KeyProc)
	        FreeProcInstance((FARPROC)KeyProc);
		KeyProc = NULL;
#endif
}


#define PREVIOUS_KEY_STATE 0x40000000  // Bit 30 implies key was up
#define KEY_GOING_UP 	0x80000000  // Bit 31 implies key was being released
#ifndef WIN32
//***********************************************************************
DWORD CALLBACK EXPORT BrowserKeyHookProc(int code, UINT nVKey, LONG dwKeyData)
//***********************************************************************
{
	int num;
	BOOL  bAlbSel, bDirSel;		
	HWND hFocus;

    DWORD r = 0L;
    
	// setup global state
	if (!lpKeyHookBsr)
		goto Exit;
	lpBsr = lpKeyHookBsr;
	if (!lpBsr->OpenState.bInitialized)
		goto Exit;
		
	if (code <0)
		return(0L);
	if (nVKey == VK_ESCAPE)
	{
		if (!IsWindowEnabled(lpBsr->OpenState.hBrowser))
			goto Exit;
    	if (!(dwKeyData & (PREVIOUS_KEY_STATE | KEY_GOING_UP)))
				PostMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDCANCEL, 1L);
		return (TRUE);
	}
    if (!(dwKeyData & PREVIOUS_KEY_STATE))
    {
		if (!IsWindowEnabled(lpBsr->OpenState.hBrowser))
			goto Exit;
        switch (nVKey)
		{
            case VK_DELETE:  
				// DELETE
				num = ParseListBoxMessage(lpBsr->OpenState.bMultiListBox, 
							FILELIST(lpBsr->OpenState.hBrowser), LB_GETSELCOUNT, 0, 0L);
				bAlbSel = (ALBMODE && (ListBox_GetCurSel(ALBLIST(lpBsr->OpenState.hBrowser)) != LB_ERR));
				bDirSel = (!ALBMODE && (ListBox_GetCurSel(GetDlgItem(DIRWINDOW(lpBsr->OpenState.hBrowser), IDC_BSR_DIRECTORIES)) != LB_ERR));
				if (!CONTROL)
					if (!(hFocus = GetFocus()))
						break;
				if (num >0)
				{
					if (!CONTROL && hFocus != FILELIST(lpBsr->OpenState.hBrowser))
						break;
					SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_DELETE, 0L);
				}
				else if (bAlbSel)
				{
					if (!CONTROL && hFocus != ALBLIST(lpBsr->OpenState.hBrowser))
						break;
					SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_ALBDELETE, 0L);
				}
				else if (bDirSel)
				{
					if (!CONTROL && hFocus != DIRLIST(lpBsr->OpenState.hBrowser))
						break;
					SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_DELETEDIR, 0L);
				}   
				else 
					break;
				r = 1L;
            break;
            case _T('C'): 
				// COPY
				if (!CONTROL)
					break;
				num = ParseListBoxMessage(lpBsr->OpenState.bMultiListBox, 
							FILELIST(lpBsr->OpenState.hBrowser), LB_GETSELCOUNT, 0, 0L);
				if (num <=0)
					break;
				SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_COPY, 0L);
				r = 1L;
			break;
            case _T('I'): 
				// INFO
				if (!CONTROL)
					break;
				num = ParseListBoxMessage(lpBsr->OpenState.bMultiListBox, 
							FILELIST(lpBsr->OpenState.hBrowser), LB_GETSELCOUNT, 0, 0L);
				bAlbSel = (ALBMODE && (ListBox_GetCurSel(ALBLIST(lpBsr->OpenState.hBrowser)) != LB_ERR));
				if (bAlbSel || num>0)
					SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDC_STATUSINFO, 0L);
				r = 1L;
            break;
            case _T('N'): 
				// NEW
				if (!CONTROL)
					break;
				if(ALBMODE)
					SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_ALBNEW, 0L);
				else
					SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_CREATEDIR, 0L);
				r = 1L;
			break;
            case _T('R'): 
				// RENAME
				if (!CONTROL)
					break;
				num = ParseListBoxMessage(lpBsr->OpenState.bMultiListBox, 
							FILELIST(lpBsr->OpenState.hBrowser), LB_GETSELCOUNT, 0, 0L);
				bAlbSel = (ALBMODE && (ListBox_GetCurSel(ALBLIST(lpBsr->OpenState.hBrowser)) != LB_ERR));
				bDirSel = (!ALBMODE && (ListBox_GetCurSel(GetDlgItem(DIRWINDOW(lpBsr->OpenState.hBrowser), IDC_BSR_DIRECTORIES)) != LB_ERR));
				if (num >0)
					SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_RENAME, 0L);
				else if (bAlbSel)
					SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_ALBRENAME, 0L);
				else if (bDirSel)
					SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_RENAMEDIR, 0L);
				else 
					break;
				r = 1L;
			break;
            case _T('M'): 
				// MOVE
				if (!CONTROL)
					break;
				num = ParseListBoxMessage(lpBsr->OpenState.bMultiListBox, 
							FILELIST(lpBsr->OpenState.hBrowser), LB_GETSELCOUNT, 0, 0L);
				if (num <=0)
					break;
				SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_MOVE, 0L);
				r = 1L;
			break;
            case _T('S'): 
				// SEARCH
				if (!CONTROL)
					break;
				SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_SEARCH, 0L);
				r = 1L;
			break;
            case _T('F'): 
				// FIND ORIGINAL
				if (!CONTROL)
					break;
				if(ALBMODE)
					SendMessage(lpBsr->OpenState.hBrowser, WM_COMMAND, IDM_BSR_FIND, 0L);
				r = 1L;
			break;				
        }
	}
Exit:
    if (!r && prevKeyHook)    
		return( CallNextHookEx(prevKeyHook, code, nVKey, dwKeyData));
	else	
    	return r;
}
#endif // WIN16

/************************************************************************/
LOCAL HWND CopyWindowEx( HWND hWnd, HWND hNewParent)
/************************************************************************/
{
	STRING szClassName, szWindowName;
	RECT Rect;

	if ( !hWnd )
		return( NULL );

	GetWindowRect( hWnd, &Rect );
	GetClassName( hWnd, szClassName, sizeof(STRING) );
	GetWindowText( hWnd, szWindowName, sizeof(STRING) );
	return( CreateWindow(
		szClassName,
		szWindowName,
		GetWindowLong( hWnd, GWL_STYLE ),
		Rect.left, Rect.top, RectWidth(&Rect), RectHeight(&Rect),
		hNewParent,
		(HMENU)(GetDlgCtrlID( hWnd )+1),
		GetWindowInstance( hWnd ),
		NULL /* lpParam */ ) );
}
