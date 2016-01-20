//
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// This .c file should provides API for extened brower
// The Extended browser provides list box and thumbnail magament
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

extern LPBSRSTATE lpBsr;
			 
LOCAL void handle_file_list(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify);
LOCAL BOOL WINPROC dlg_ext_bsr_proc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

//************************************************************************
// 		ExtBsr_Init
// PARAMETERS:
//					HWND hDlg- The dialog box.
//					LPOPENSTATE lpState- Standard open info.
//					LPEXTSTATE LPEXTSTATE- EXTENDED OPEN INFO (CALLBACKS ETC)
// DESCRIPTION:
//					Call on WM_INIT for any dialog using extended browser.
// RETURNS:
//				False if there was a failure (probably memory).
//************************************************************************
BOOL ExtBsr_Init( HWND hDlg, LPEXTSTATE lpExtState, void FAR *lpExtUserData)
//************************************************************************
{
	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (lpBsr) return(FALSE);
	
	// setup BROWSERSTATE struct
	lpBsr = GetBsrStateStruct();
	if (!lpBsr)
		return(FALSE);
	SetWindowLong(hDlg, DWL_USER, (LONG)lpBsr);
	
	lpBsr->OpenState.bSaving = FALSE;
	lpBsr->OpenState.bExtended = TRUE;
	lpBsr->OpenState.bInitialized = FALSE;
	lpBsr->OpenState.hBrowser = hDlg;
	
   	lpBsr->OpenState.wExtType = lpExtState->wExtType;
   	lpBsr->OpenState.lpGetFileProc = lpExtState->lpGetFileProc;
   	lpBsr->OpenState.lpGetPreview = lpExtState->lpGetPreview;
   	lpBsr->OpenState.bMultiListBox = lpExtState->bMultiListBox;

	lpBsr->OpenState.ViewAlbums = FALSE;
	lpBsr->OpenState.ViewThumbnails = lpExtState->ViewThumbnails;
	lpBsr->OpenState.AutoThumbnails = lpExtState->AutoThumbnails;
	lpBsr->OpenState.lpExtUserData = lpExtUserData;
	
	if (!BrowserDlgBegin(hDlg))
		return(FALSE);
	EnableOpenSaveButton(hDlg);
	EnableInfoButton(hDlg);
	return(TRUE);
}

//************************************************************************
// 		ExtBsr_Done
// DESCRIPTION:
//			Call to free up all memory associated with ext open.
//			Will automaticly be called on WM_DESTROY.
//************************************************************************
void ExtBsr_Done( HWND hDlg)
//************************************************************************
{
	// clean up
	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return;
	SetWindowLong(hDlg, DWL_USER, 0L);
	BrowserDlgEnd(hDlg);
	if (lpBsr->lpOpenState)
		*lpBsr->lpOpenState = lpBsr->OpenState;
	FreeBsrStateStruct(lpBsr);
	lpBsr = NULL;
}

//************************************************************************
// 
// PARAMETERS:
//				LPINT lpNum- Filled with number of items selected.
// DESCRIPTION:
//				Call to get the list of selected items.
// RETURNS:
//				An array of FNAMEs of size lpNum, which gives the extended
//				names of selected items.  NULL if there are none, 
//				or for failure.
//				NOTE: caller must free up the returned array.
//************************************************************************
FNAME *ExtBsr_GetSelection( HWND hDlg, LPINT lpNum)
//************************************************************************
{
	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return(NULL);
		
	if (!MakeFinalList(hDlg))
	{
		*lpNum = 0;	
		return(NULL);
	}
	*lpNum = lpBsr->OpenState.numFiles;
	return(lpBsr->OpenState.lpFileList);
}

// LOCAL prototypes


/***********************************************************************/
void ExtBsr_OnPaletteChanged(HWND hDlg, HWND hwndPaletteChange)
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
// DESCRIPTION:
//			Call for default processing in extended browser.
// RETURNS:
//			TRUE if processing should continue
//************************************************************************
BOOL ExtBsr_OnInitDialog(HWND hDlg, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	SetDlgItemText(hDlg, IDC_BSR_FILENAME, _T(""));
	return(TRUE);
}


//************************************************************************
// DESCRIPTION:
//			Call for default processing in extended browser.
// RETURNS:
//			TRUE if processing should continue
//************************************************************************
BOOL ExtBsr_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	ExtBsr_Done(hDlg);
	return(TRUE);
}


//************************************************************************
// DESCRIPTION:
//			Call for default processing in extended browser.
// RETURNS:
//			TRUE if processing should continue.
//			FALSE if the message was processed.
//************************************************************************
BOOL ExtBsr_OnDrawItem(HWND hDlg, const DRAWITEMSTRUCT FAR *lpDrawItem)
//************************************************************************
{
	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return(TRUE);
	
	switch (lpDrawItem->CtlID)
	{
		case IDC_BSR_FILELIST:
			FileListDraw(hDlg, (LPDRAWITEMSTRUCT)lpDrawItem); 
			return(FALSE);
		break;
		case IDC_BSR_THUMBS:
			DrawThumb(hDlg, (LPDRAWITEMSTRUCT)lpDrawItem); 
			SAFECANCEL;
			return(FALSE);
		break;
	}
	return(TRUE);
}

//************************************************************************
// DESCRIPTION:
//			Call for default processing in extended browser.
// RETURNS:
//			TRUE if processing should continue.
//			FALSE if the message was processed.
//************************************************************************
BOOL ExtBsr_OnMeasureItem(HWND hDlg, LPMEASUREITEMSTRUCT lpMeasureItem)
//************************************************************************
{
	switch (lpMeasureItem->CtlID)
	{
		case IDC_BSR_FILELIST:
			FileListMeasure(hDlg,lpMeasureItem); 
			return(FALSE);
		break;
		case IDC_BSR_THUMBS:
			MeasureThumb(hDlg,lpMeasureItem); 
			return(FALSE);
		break;
	}
	return(TRUE);
}

//************************************************************************
// DESCRIPTION:
//			Call for default processing in extended browser.
// RETURNS:
//			TRUE if processing should continue.
//			FALSE if the message was processed.
//************************************************************************
BOOL ExtBsr_OnCommand(HWND hDlg, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	HWND hList;

	lpBsr = (LPBSRSTATE)GetWindowLong(hDlg, DWL_USER);
	if (!lpBsr)	return(TRUE);
	switch (id)
	{
	
		case IDC_BSR_THUMBS:
		case IDC_BSR_FILELIST:
			handle_file_list(hDlg, id, hControl, codeNotify);
		break;
		case IDM_BSR_THMUPDATE:
			if (SHIFT)
				UpdateFileThumbs(hDlg, VL_EXIST | VL_CLEAN | VL_NOTIME | VL_ESCAPABLE);
			else
				UpdateFileThumbs(hDlg, VL_EXIST | VL_CLEAN | VL_ESCAPABLE);
			hList = GetDlgItem(hDlg,FILELISTID);
		    InvalidateRect (hList,NULL,FALSE);
    		UpdateWindow (hList); 
		break;
	   	default:
			return(TRUE);
		break;
	}
	return(FALSE);
}

//************************************************************************
//		ExtBsrInfo_GetCurrentName
// PARAMETERS:
//				LPTSTR lpName- FNAME buffer to put name into.
//				HWND hDlg- Info dialog.
// DESCRIPTION:
//			While in info dialog, call this to get current extended name.
// RETURNS:
//			lpName or NULL for failure
//************************************************************************
LPTSTR ExtBsrInfo_GetCurrentName(HWND hDlg, LPTSTR lpName)
//************************************************************************
{
	FNAME File;
	
	if (!GetParentBsrState(hDlg))
		return(NULL);
	if (!GetCurrentFileEntry(File))
		return(NULL);
	lstrcpy(lpName, File);
	return(lpName);
}


//************************************************************************
// 		ExtBsrInfo_HandleDefaults
// DESCRIPTION:
//			Call to handle common parts of info dialog.
//************************************************************************
BOOL WINPROC ExtBsrInfo_HandleDefaults( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam, ITEMID idDialog)
//************************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, ExtBsrInfo_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_CLOSE, ExtBsrInfo_OnClose);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, ExtBsrInfo_OnCommand);
		HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, ExtBsrInfo_OnControlEnable);
	    default:	  
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, idDialog);
	}  
}

//************************************************************************
BOOL ExtBsrInfo_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	 return(FileInfo_OnInitDialog(hDlg,hwndFocus,lParam));
}

//************************************************************************
void ExtBsrInfo_OnClose(HWND hDlg) 
//************************************************************************
{
	FileInfo_OnClose(hDlg);
}

//************************************************************************
void ExtBsrInfo_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify) 
//************************************************************************
{
	FileInfo_OnCommand( hDlg, id, hwndCtl, codeNotify);
}

//************************************************************************
void ExtBsrInfo_OnControlEnable(HWND hDlg, int fFlags) 
//************************************************************************
{
	FileInfo_OnControlEnable( hDlg, fFlags);
}

//************************************************************************
BOOL ExtBsrInfo_OnCreate(HWND hDlg, CREATESTRUCT FAR* lpCreateStruct) 
//************************************************************************
{
	return(DWL_USER_OnCreate( hDlg, lpCreateStruct));
}

//************************************************************************
void ExtBsrInfo_OnDestroy(HWND hDlg) 
//************************************************************************
{
	DWL_USER_OnDestroy(hDlg);
}

//************************************************************************
// 		handle_file_list
// DESCRIPTION:
//		Called in respone to notification message from file list box
//************************************************************************
LOCAL void handle_file_list(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify)
//************************************************************************/
{
	int len, num;
	FNAME szFile, szFileName;
	LPTSTR lpFileName;

	switch (codeNotify)
	{
		case LBN_DBLCLK:
			FORWARD_WM_COMMAND(hDlg, IDOK, 0, 0, SendMessage);
		break;
		case LBN_SELCHANGE:
		case LBN_SELCANCEL:
			EnableOpenSaveButton(hDlg);
			EnableInfoButton(hDlg);
			//build up file name
			num = OpenFileList(hDlg);
			if (!num)
			{	// no selected items, reset file text
				SetDlgItemText(hDlg, IDC_BSR_FILENAME, _T(""));
				break;
			}
			len = sizeof(szFileName) - 2;
			szFileName[0] = _T('\0');
			while (GetNextFileEntry(szFile) && len > 0 )
			{
				lpFileName = szFile;
				szFile[len] = _T('\0');
				len -= lstrlen(lpFileName);
				if (len>0) 
				{
					lstrcat(szFileName, lpFileName);
					len--;
					lstrcat(szFileName, _T(","));
				}
			}
			StripTrailer(szFileName, _T(','));
			// Set File Name text box
			SetDlgItemText(hDlg, IDC_BSR_FILENAME, szFileName);
		break;
	}
}
