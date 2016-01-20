// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// handles sub dialogs brought up Browser dlg

#include "pp.h"
#include <errno.h> 
#include <time.h> 
#include "id.h"
#include "bsrid.h"
#include "bsrtypes.h"
#include "bsrutil.h"
#include "handlebs.h"
#include "browser.h"
#include "bsritems.h"
#include "dbfmgr.h"
#include "cthmmgr.h"
#include "bsrdlgs.h"
#include "ppafx.h"


//****************************Globals****************************************
LPCHOOSEALBUM lpChooseAlbum;

//****************************Externals****************************************
extern LPBSRSTATE lpBsr;
extern BOOL bImported;

BOOL WINPROC DlgGetBsrStringProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

//********************Static prototypes****************************************/
LOCAL BOOL VerifyPrefs(BROWSER FAR *lpPref);
LOCAL LPCTHMNAIL FillFileInfo(HWND hInfo, LPFILELOC lpFile);
LOCAL void FillAlbInfo(HWND hInfo, LPDBF lpDBF);
LOCAL BOOL CheckFileSaveDescr(HWND hDlg, BOOL bSaveDescr);
LOCAL BOOL FileInfoSaveDescr(HWND hInfo, LPFILELOC lpFile);
LOCAL BOOL CheckAlbSaveDescr(HWND hDlg, BOOL bSaveDescr);
LOCAL BOOL AlbInfoSaveDescr(HWND hInfo, LPDBF lpDBF);
LOCAL void ClearImageWindow(HWND hWindow, int border);
LOCAL BOOL CopyFilesProc(LPTSTR lpSrcFile, LPTSTR lpDstFile, long lData, int iCount);
LOCAL BOOL CopyFileLocProc(LPFILELOC lpSrcFile, LPFILELOC lpDstFile, HWND hDlg);
LOCAL BOOL MoveFilesProc(LPTSTR lpSrcFile, LPTSTR lpDstFile, long lData, int iCount);
LOCAL BOOL MoveFileLocProc(LPFILELOC lpSrcFile, LPFILELOC lpDstFile, HWND hDlg);
LOCAL BOOL DeleteFilesProc(LPTSTR lpSrcFile, LPTSTR lpDstFile, long lData, int iCount);
LOCAL BOOL DeleteFileLocProc(LPFILELOC lpFile, HWND hDlg);
LOCAL void FillAlbums(HWND hDlg);

LOCAL void Prefs_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);
LOCAL BOOL Prefs_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);

LOCAL BOOL ChooseAlb_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void ChooseAlb_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

LOCAL BOOL CopyFiles_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void CopyFiles_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

LOCAL BOOL CopyFile_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void CopyFile_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

LOCAL BOOL MoveFile_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void MoveFile_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

LOCAL BOOL MoveFiles_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void MoveFiles_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

LOCAL BOOL DeleteFile_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void DeleteFile_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

LOCAL BOOL DeleteFiles_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void DeleteFiles_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

LOCAL BOOL RenameFile_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void RenameFile_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

LOCAL BOOL CleanThm_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void CleanThm_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

LOCAL BOOL DeleteVirtual_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void DeleteVirtual_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

LOCAL BOOL GetBsrString_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void GetBsrString_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);


// for BsrPref Dialog
static BROWSER newPref;

// for FileInfo Dialog
typedef struct _fileinfodlg{
	LPCTHMNAIL lpSaveThm;	// so it can be freed
	BOOL bSaveDescr;
} FILEINFODLG, FAR *LPFILEINFODLG;

typedef struct _albinfodlg{
	BOOL bSaveDescr;
	int idAlbum;
} ALBINFODLG, FAR *LPALBINFODLG;

// for CopyFile Dialog
static FNAME CopyFile_szDir;

// for MoveFile Dialog
static FNAME MoveFile_szDir;

// for DeleteFile Dialog
static FNAME DeleteFile_szDir;

// for CleanupThumbs Dialog
static int CleanThm_idWhat;
static BOOL CleanThm_bCleanSubDir;

static LPTSTR lpSrcText;	
static LPTSTR lpDstText;	
static LPTSTR lpInitText;	

static int iFilesProcessed;
static int iFilesToBeProcessed;
static BOOL bAbort, bCancelled, bImageOpen, bConfirm, bConfirmOverwrite;

static int dialogId;	// for SetupCursor

// This has to stay here because MGXPS.DRV is not yet fully WIN3.1!
#define NOTWIN31

					  
//***************************** Functions  *******************************/

//************************************************************************/
//	For Browser preferences dlg
//************************************************************************/
BOOL WINPROC DlgBsrPrefProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, Prefs_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, Prefs_OnCommand);  
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_OPEN);
	}
}	
		
//************************************************************************/
LOCAL BOOL Prefs_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************/
{
	HWND hControl;
 	newPref = Browser;
	CenterPopup(hDlg);
	if (hControl = GetDlgItem(hDlg,IDC_BSR_THMDIR))
	{
		SetWindowText( hControl, newPref.ThumbnailDir);
		Edit_LimitText( hControl, sizeof(newPref.ThumbnailDir));
	}
	if (hControl = GetDlgItem(hDlg,IDC_BSR_DBFINFODIR))
	{
		SetWindowText( hControl, newPref.DBFInfoDir);
		Edit_LimitText( hControl, sizeof(newPref.DBFInfoDir));
	}
	if (hControl = GetDlgItem(hDlg,IDC_BSR_ALBFILEDIR))
	{
		SetWindowText( hControl, newPref.AlbumFileDir);
		Edit_LimitText( hControl, sizeof(newPref.AlbumFileDir));
	}
	CheckDlgButton(hDlg,IDC_BSR_SAVEPATH,newPref.OKtoSavePath);
	CheckDlgButton(hDlg,IDC_BSR_AUTOTHM,newPref.AutoThumbnails);
	CheckDlgButton(hDlg,IDC_BSR_CREATE_ON_SAVE,newPref.CreateThumbnailOnSave);
	CheckDlgButton(hDlg,IDC_BSR_CONF_DELETE 	,newPref.ConfirmOnDelFile);
	CheckDlgButton(hDlg,IDC_BSR_CONF_MOVE		,newPref.ConfirmOnMoveFile);
	CheckDlgButton(hDlg,IDC_BSR_CONF_COPY		,newPref.ConfirmOnCopyFile);
	CheckDlgButton(hDlg,IDC_BSR_CONF_RENAME		,newPref.ConfirmOnRenameFile);
	return(0);
}
		
//************************************************************************/
LOCAL void Prefs_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************/
{
	switch(id)
	{
		case IDC_BSR_THMDIR:
			if ( GetFocus() != hwndCtl)
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			GetDlgItemText(hDlg, id, newPref.ThumbnailDir,
				sizeof(newPref.ThumbnailDir));
			FixPath( newPref.ThumbnailDir );
		break;
		case IDC_BSR_DBFINFODIR:	   
			if ( GetFocus() != hwndCtl)
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			GetDlgItemText(hDlg, id, newPref.DBFInfoDir,
				sizeof(newPref.DBFInfoDir));
			FixPath( newPref.DBFInfoDir);
		break;
		case IDC_BSR_ALBFILEDIR:	   
			if ( GetFocus() != hwndCtl)
				break;
			if ( codeNotify != EN_CHANGE )
				break;
			GetDlgItemText(hDlg, id, newPref.AlbumFileDir,
				sizeof(newPref.AlbumFileDir));
			FixPath( newPref.AlbumFileDir );
		break;
		case IDC_BSR_SAVEPATH:	   
			newPref.OKtoSavePath= !newPref.OKtoSavePath;
			CheckDlgButton(hDlg,id,newPref.OKtoSavePath);
		break;
	   	case IDC_BSR_AUTOTHM:	   
			newPref.AutoThumbnails= !newPref.AutoThumbnails;
			CheckDlgButton(hDlg,id,newPref.AutoThumbnails);
		break;
	   	case IDC_BSR_CREATE_ON_SAVE:	   
			newPref.CreateThumbnailOnSave= !newPref.CreateThumbnailOnSave;
			CheckDlgButton(hDlg,id,newPref.CreateThumbnailOnSave);
		break;
		case IDC_BSR_CONF_DELETE:
			newPref.ConfirmOnDelFile	= !newPref.ConfirmOnDelFile;
			newPref.ConfirmOnDeleteDir	= newPref.ConfirmOnDelFile;
			newPref.ConfirmOnDeleteAlbum = newPref.ConfirmOnDelFile;
			CheckDlgButton(hDlg,id,newPref.ConfirmOnDelFile);
		break;
		case IDC_BSR_CONF_MOVE:
			newPref.ConfirmOnMoveFile	= !newPref.ConfirmOnMoveFile;
			newPref.ConfirmOnAddFile = newPref.ConfirmOnMoveFile;
			CheckDlgButton(hDlg,id,newPref.ConfirmOnMoveFile);
		break;
		case IDC_BSR_CONF_COPY:  
			newPref.ConfirmOnCopyFile	= !newPref.ConfirmOnCopyFile;
			CheckDlgButton(hDlg,id,newPref.ConfirmOnCopyFile);
		break;
		case IDC_BSR_CONF_RENAME:
			newPref.ConfirmOnRenameFile	= !newPref.ConfirmOnRenameFile;
			newPref.ConfirmOnRenameDir = newPref.ConfirmOnRenameFile;
			newPref.ConfirmOnRenameAlbum = newPref.ConfirmOnRenameFile;
			CheckDlgButton(hDlg,id,newPref.ConfirmOnRenameFile);
		break;
		case IDC_BSR_SAVEPREF:
			if (!VerifyPrefs(&newPref))
				break;
			Browser = newPref;
			SaveBrowserPrefs();
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDOK:
			if (!VerifyPrefs(&newPref))
				break;
			Browser = newPref;
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, FALSE);
		break;
		default:
		break;
	}  // switch command
}
	
//************************************************************************
//	Makes sure the entries in the preferences are valid
//************************************************************************
LOCAL BOOL VerifyPrefs(BROWSER FAR *lpPref)
{
	if (!CheckDir(lpPref->ThumbnailDir))
	{
		Message(IDS_EINVALIDDIR, (LPTSTR)lpPref->ThumbnailDir);
		return(FALSE);
	}
	if (!CheckDir(lpPref->DBFInfoDir))
	{
		Message(IDS_EINVALIDDIR, (LPTSTR)lpPref->DBFInfoDir);
		return(FALSE);
	}
	if (!CheckDir(lpPref->AlbumFileDir))
	{
		Message(IDS_EINVALIDDIR, (LPTSTR)lpPref->AlbumFileDir);
		return(FALSE);
	}
	return(TRUE);
}
		
//************************************************************************/
//	For Browser File Info dlg
//************************************************************************/
BOOL WINPROC DlgBsrFileInfoProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************/
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, FileInfo_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_CLOSE, FileInfo_OnClose);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, FileInfo_OnCommand);
		HANDLE_DLGMSG(hDlg, WM_CONTROLENABLE, FileInfo_OnControlEnable);
		
		HANDLE_DLGMSG(hDlg, WM_DESTROY, DWL_USER_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_CREATE, DWL_USER_OnCreate);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_BSR_FILEINFO);
	}  
}
	
//************************************************************************
BOOL FileInfo_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	FILELOC File;
	HWND hControl;
	LPFILEINFODLG lpData;
	
	if (!GetParentBsrState(hDlg))
		return(FALSE);
	if (!(lpData = (LPFILEINFODLG)Alloc(sizeof(FILEINFODLG))))
		return(FALSE);
	SetWindowLong(hDlg, DWL_USER, (long)lpData);
	
	lpData->lpSaveThm = NULL;
	lpData->bSaveDescr = FALSE;
	
	CenterPopup(hDlg);
	if (OpenFileList(GetParent(hDlg)) <= 0)
	{
		AstralDlgEnd(hDlg, FALSE);
		return(FALSE);
	}
		
	if (!GetFirstFile(&File))
	{
		AstralDlgEnd(hDlg, FALSE);
		return(FALSE);
	}
	if (hControl = GetDlgItem( hDlg, IDC_INFO_THUMBNAIL ))
		SetWindowLong( hControl, GWL_IMAGE, (LONG)NULL );
	if (hControl = GetDlgItem( hDlg, IDC_INFO_FILEDESCR ))
		Edit_LimitText( hControl, sizeof(DESC)-1);
	ControlEnable(hDlg, IDC_INFO_PREVIOUS, IsPreviousFileName());
	ControlEnable(hDlg, IDC_INFO_NEXT, IsNextFileName());
	// fill info
	SEND_WM_CONTROLENABLE(hDlg, 0L);
	return(TRUE);
}

//************************************************************************
void FileInfo_OnClose(HWND hDlg)
//************************************************************************
{
	LPFILEINFODLG lpData;
	
	if (!(lpData = (LPFILEINFODLG)GetParentBsrState(hDlg, TRUE)))
	{
		lpData->bSaveDescr = CheckFileSaveDescr(hDlg, lpData->bSaveDescr);
		if (lpData->lpSaveThm)
			CTHMClose(lpData->lpSaveThm);
	}
	AstralDlgEnd(hDlg, FALSE);
}
	
//************************************************************************
void FileInfo_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	FILELOC File;
	LPFILEINFODLG lpData;
	
	if (!(lpData = (LPFILEINFODLG)GetParentBsrState(hDlg, TRUE)))
		return;	

	switch(id)
	{
		case IDC_INFO_THUMBNAIL:
		break;
		case IDC_INFO_NEXT:
			lpData->bSaveDescr = CheckFileSaveDescr(hDlg, lpData->bSaveDescr);
			if (GetNextFile(&File))
			{
				// fill info
				SEND_WM_CONTROLENABLE(hDlg, 0L);
				ControlEnable(hDlg, IDC_INFO_PREVIOUS, IsPreviousFileName());
				ControlEnable(hDlg, IDC_INFO_NEXT, IsNextFileName());
			}
		break;
		case IDC_INFO_FILEDESCR:
			if (codeNotify != EN_CHANGE)
				break;
			// text changed... allow save description list box
			lpData->bSaveDescr = TRUE;
		break;
		case IDC_INFO_PREVIOUS:
			lpData->bSaveDescr = CheckFileSaveDescr(hDlg, lpData->bSaveDescr);
			if (GetPreviousFile(&File))
			{
				// fill info
				SEND_WM_CONTROLENABLE(hDlg, 0L);
				ControlEnable(hDlg, IDC_INFO_PREVIOUS, IsPreviousFileName());
				ControlEnable(hDlg, IDC_INFO_NEXT, IsNextFileName());
			}
		break;
		case IDC_INFO_UPDATE:
			if (GetCurrentFile(&File))
			{
				lpData->bSaveDescr = CheckFileSaveDescr(hDlg, lpData->bSaveDescr);
				if (lpData->lpSaveThm)
				{
					CTHMClose(lpData->lpSaveThm);
					lpData->lpSaveThm = NULL;
				}
				UpdateCreateThumb(&File, VL_EXIST | VL_CLEAN | VL_ESCAPABLE, YES); 
				// fill info
				SEND_WM_CONTROLENABLE(hDlg, 0L);
			}
		break;
		case IDOK:
			lpData->bSaveDescr = CheckFileSaveDescr(hDlg, lpData->bSaveDescr);
			if (lpData->lpSaveThm)
				CTHMClose(lpData->lpSaveThm);
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDCANCEL:
			if (lpData->lpSaveThm)
				CTHMClose(lpData->lpSaveThm);
			AstralDlgEnd(hDlg, TRUE);
		break;
		default:
		break;
	}  // switch command
}

//************************************************************************
void FileInfo_OnControlEnable(HWND hDlg, int fFlags)
//************************************************************************
{
	LPFILEINFODLG lpData;
	FILELOC File;
	
	if (!(lpData = (LPFILEINFODLG)GetParentBsrState(hDlg, TRUE)))
		return;	
	if (GetCurrentFile(&File))
	{
		if (lpData->lpSaveThm)
			CTHMClose(lpData->lpSaveThm);
		lpData->lpSaveThm = FillFileInfo(hDlg, &File);
	}
}

BOOL AlbInfo_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
void AlbInfo_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);
void AlbInfo_OnClose(HWND hDlg);

//************************************************************************/
//	For Browser Album Info dlg
//************************************************************************/
BOOL WINPROC DlgBsrAlbInfoProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************/
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, AlbInfo_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, AlbInfo_OnCommand);
		HANDLE_DLGMSG(hDlg, WM_CLOSE, AlbInfo_OnClose);
		
		HANDLE_DLGMSG(hDlg, WM_DESTROY, DWL_USER_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_CREATE, DWL_USER_OnCreate);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_BSR_ALBINFO);
	}  
}
	
//************************************************************************
BOOL AlbInfo_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	int index;
	LPDBFLL lpDBFLL;
	HWND hBox;
	LPALBINFODLG lpData;
	STRING szAlbum;
	
	if (!GetParentBsrState(hDlg))
		goto ExitError;
	if (!(lpData = (LPALBINFODLG)Alloc(sizeof(ALBINFODLG))))
		goto ExitError;
	SetWindowLong(hDlg, DWL_USER, (long)lpData);
	
	CenterPopup(hDlg);
	lpData->bSaveDescr = FALSE;
	if (!(lpDBFLL = GetSelectedAlbum(lpBsr->OpenState.hBrowser)))
		if (!(lpDBFLL = FindAlbum(lpBsr->OpenState.szAlbum)))
			goto ExitError;
	hBox = ALBLIST(lpBsr->OpenState.hBrowser);
	index = ListBox_GetCount(hBox);
	if (index < 0)
		lpData->idAlbum = 0;
	else
	{
		lpData->idAlbum = ListBox_FindString(hBox, -1, lpDBFLL->dbf.info.szName);
		while(lpData->idAlbum > LB_ERR)
		{
			if (ListBox_GetText(hBox,  lpData->idAlbum, (LONG)(LPTSTR)szAlbum) != LB_ERR)
			{
				ExtractStringID(szAlbum);
				if (StringsEqual(lpDBFLL->dbf.info.szName, szAlbum))
					break;
			}
			lpData->idAlbum = ListBox_FindString(hBox, lpData->idAlbum, lpDBFLL->dbf.info.szName);
		}
		if(lpData->idAlbum <= LB_ERR)
			lpData->idAlbum = 0;
		else if (!(lpDBFLL = FindAlbum(szAlbum)))
			goto ExitError;
	}
	if (hBox = GetDlgItem( hDlg, IDC_ALBINFO_DESCR ))
		Edit_LimitText( hBox, sizeof(FNAME)-1);
	ControlEnable(hDlg, IDC_ALBINFO_PREVIOUS, lpData->idAlbum>0);
	ControlEnable(hDlg, IDC_ALBINFO_NEXT, lpData->idAlbum<index-1);
	FillAlbInfo(hDlg, &lpDBFLL->dbf);
	return(TRUE);
ExitError:
	AstralDlgEnd(hDlg, FALSE);
	return(FALSE);
}

//************************************************************************
void AlbInfo_OnClose(HWND hDlg)
//************************************************************************
{
	LPALBINFODLG lpData;
	
	if (!(lpData = (LPALBINFODLG)GetParentBsrState(hDlg, TRUE)))
		lpData->bSaveDescr = CheckAlbSaveDescr(hDlg, lpData->bSaveDescr);
	AstralDlgEnd(hDlg, FALSE);
}

//************************************************************************
void AlbInfo_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	int index, idNewAlb;
	LPDBFLL lpDBFLL;
	STRING szAlbum;
	HWND hBox;
	LPALBINFODLG lpData;
	
	if (!(lpData = (LPALBINFODLG)GetParentBsrState(hDlg, TRUE)))
		return;	
	switch(id)
	{
		case IDC_ALBINFO_NEXT:
		case IDC_ALBINFO_PREVIOUS:
			hBox = ALBLIST(lpBsr->OpenState.hBrowser);
			index = ListBox_GetCount(hBox);
			if (index<0)
				break;
			idNewAlb = lpData->idAlbum + ((id == IDC_ALBINFO_NEXT) ? 1:-1);
			if (lpData->idAlbum < 0 || lpData->idAlbum >= index)
				break;
			if (ListBox_GetText(hBox,  idNewAlb, (LONG)(LPTSTR)szAlbum) == LB_ERR)
				break;
			ExtractStringID(szAlbum);
			lpData->bSaveDescr = CheckAlbSaveDescr(hDlg, lpData->bSaveDescr);
			// get the album
			if (!(lpDBFLL = FindAlbum(szAlbum)))
				break;
			lpData->idAlbum = idNewAlb;
			FillAlbInfo(hDlg, &lpDBFLL->dbf);
			ControlEnable(hDlg, IDC_ALBINFO_PREVIOUS, lpData->idAlbum>0);
			ControlEnable(hDlg, IDC_ALBINFO_NEXT, lpData->idAlbum<index-1);
		break;
			
		case IDC_ALBINFO_DESCR:
			if (codeNotify != EN_CHANGE)
				break;
			// text changed... allow save description list box
			lpData->bSaveDescr=TRUE;
		break;
		case IDOK:
		case IDCANCEL:
			lpData->bSaveDescr = CheckAlbSaveDescr(hDlg, lpData->bSaveDescr);
			AstralDlgEnd(hDlg, TRUE);
		break;
		default:
		break;
	}  // switch command
}


//************************************************************************/
// 	Allows user to choose an album.
//	Returns true if successful.
//	Puts result in  lpChooseAlbum->szAlbum.
//	Callår should fill lpChooseAlbum structre before calling.
//************************************************************************
BOOL WINPROC DlgChooseAlbumProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, ChooseAlb_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, ChooseAlb_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_CHOOSEALBUM);
	}  
}
		
//************************************************************************
LOCAL BOOL ChooseAlb_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	CenterPopup(hDlg);
	if (lpChooseAlbum->lpChooseTitle)
		SetWindowText(hDlg, lpChooseAlbum->lpChooseTitle);
	if (lpChooseAlbum->lpChooseText)
		SetDlgItemText(hDlg, IDC_CHOOSEALBUM_TEXT, lpChooseAlbum->lpChooseText); 
	if (lpChooseAlbum->lpSelectedAlbum)
		if( *( lpChooseAlbum->lpSelectedAlbum ) == _T('\0'))
			lpChooseAlbum->lpSelectedAlbum = NULL;
	if (lpChooseAlbum->lpSkipAlbum)
		if( *( lpChooseAlbum->lpSkipAlbum ) == _T('\0'))
			lpChooseAlbum->lpSkipAlbum = NULL;
	FillAlbums(hDlg);
	return(TRUE);
}

//************************************************************************
LOCAL void ChooseAlb_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	HWND hAlbBox;
	FNAME szAlbum, szTemp;
	int index, ret;
	LPTSTR lpTemp;

	switch(id)
	{
		case IDC_EXTMANAGE:
			// get selected album
			*szAlbum = _T('\0');
			hAlbBox = GetDlgItem(hDlg, IDC_BSR_ALBUMCOMBO);
			if (hAlbBox)
			{
				index = ComboBox_GetCurSel(hAlbBox);
				if (index != CB_ERR)
					ComboBox_GetLBText(hAlbBox,  index, (long)(LPTSTR)szAlbum);
			}
			SetDlgItemText(hDlg, IDC_FILENAME, szAlbum);
			PopupMenuEx( hDlg, id, IDC_ALBMANAGE, IDC_FILENAME );
		break;
		case IDC_ADDEXT:
			// dont want to change open album
			ret = NewAlbum(hDlg, NO);
			if (ret)
			{
				// make new album selected in our combo
				lpTemp = lpChooseAlbum->lpSelectedAlbum;
				lstrcpy(szTemp, lpBsr->OpenState.szAlbum);
				lstrcpy(lpBsr->OpenState.szAlbum, szAlbum);
				lpChooseAlbum->lpSelectedAlbum = szTemp;
				FillAlbList(lpBsr->OpenState.hBrowser);
				FillAlbums(hDlg);
				lpChooseAlbum->lpSelectedAlbum = lpTemp;
			}
			SetFocus(hDlg);
		break;
		case IDC_DELETEEXT:
			GetDlgItemText(hDlg, IDC_FILENAME, szAlbum, sizeof(szAlbum));
			ret = DeleteAlbum(lpBsr->OpenState.hBrowser, szAlbum, NO);
			EnableWindow(hDlg, TRUE);
			if (ret)
			{
				FillAlbList(lpBsr->OpenState.hBrowser);
				FillAlbums(hDlg);
			}
			SetFocus(hDlg);
		break;
		case IDC_RENAMEEXT:
			GetDlgItemText(hDlg, IDC_FILENAME, szAlbum, sizeof(szAlbum));
			ret = RenameAlbum(hDlg, szAlbum, NO);
			EnableWindow(hDlg, TRUE);
			if (ret)
			{
				FillAlbList(lpBsr->OpenState.hBrowser);
				lpTemp = lpChooseAlbum->lpSelectedAlbum;
				lpChooseAlbum->lpSelectedAlbum = szAlbum;
				FillAlbums(hDlg);
				lpChooseAlbum->lpSelectedAlbum = lpTemp;
			}
			SetFocus(hDlg);
		break;
		case IDOK:
			// get selected album
			hAlbBox = GetDlgItem(hDlg, IDC_BSR_ALBUMCOMBO);
			if (hAlbBox)
			{
				index = ComboBox_GetCurSel(hAlbBox);
				if (index != CB_ERR)
					if (ComboBox_GetLBText(hAlbBox,  index, (long)lpChooseAlbum->szAlbum) != CB_ERR)
					{
						AstralDlgEnd(hDlg, TRUE);
						break;
					}
			}
			AstralDlgEnd(hDlg, FALSE);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, FALSE);
		break;
		default:
		break;
	}  // switch command
}
		
//************************************************************************/
LOCAL void FillAlbums(HWND hDlg)
//************************************************************************/
{
	HWND hAlbBox;
	LPDBFLL alb;
	FNAME szAlbum;
	BOOL  bActive;
	int index, num;
	
	// Fill albums combo box
	hAlbBox = GetDlgItem(hDlg, IDC_BSR_ALBUMCOMBO);
	if (!hAlbBox)
		return;
	ComboBox_ResetContent( hAlbBox);
	index = 0;
	bActive = FALSE;
	alb = FirstAlb();
	while (alb)
	{
		lstrcpy(szAlbum, alb->dbf.info.szName);
		index = ComboBox_AddString(hAlbBox, szAlbum);
		// select current string
		if (lpChooseAlbum->lpSelectedAlbum)
			if (StringsEqual(szAlbum, lpChooseAlbum->lpSelectedAlbum) && index != CB_ERR)
				bActive = TRUE;
		alb = GetNextAlb();
	}
	// deselect/remove album
	if (lpChooseAlbum->lpSkipAlbum)
	{
		index = ComboBox_FindString(hAlbBox, -1, lpChooseAlbum->lpSkipAlbum);
		if (index != CB_ERR)
			index = ComboBox_DeleteString(hAlbBox, index);
	}
	// be sure there are some items
	num = ComboBox_GetCount(hAlbBox);
	if (!num)
		return;
	// be sure one is selected
	if (bActive)
		ListBox_SelectString(hAlbBox, -1, lpChooseAlbum->lpSelectedAlbum);
	else
	{
		// if there is no selected then select first one
		ComboBox_SetCurSel(hAlbBox, 0);
	}
}

//***********************************************************************
BOOL RenameFileDialog(HWND hDlg, LPTSTR lpSrcString, LPTSTR lpDstString)
//***********************************************************************
{
	lpSrcText = lpSrcString;
	lpDstText = lpDstString;
	return((BOOL)AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_RENAME_FILE, DlgRenameFileProc));
}

//***********************************************************************
BOOL DeleteVirtualDialog(HWND hDlg, LPTSTR lpAlbString)
//***********************************************************************
{
	lpSrcText = lpAlbString;
	return((BOOL)AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_DELETE_VIRTUAL, DlgDeleteVirtualProc));
}

//***********************************************************************
//	Gets a string: 
//		Fills IDC_BSR_TEXT  with lpSrcSrting.  
//		Fills IDC_DST_TEXT 	width lpInitString.
//		On return lpDstString will have the result of IDC_DST_TEXT.
//***********************************************************************
int GetDialogString(HWND hParent, LPTSTR lpInitString, LPTSTR lpSrcString, LPTSTR lpDstString, int idDlg)
//***********************************************************************
{
	lpSrcText = lpSrcString;
	lpDstText = lpDstString;
	lpInitText = lpInitString;
	dialogId = idDlg;
	return((int)AstralDlg(NO, PictPubApp.GetResourceHandle(), hParent, idDlg, DlgGetBsrStringProc));
}

//***********************************************************************
// 	Used for Copy and Move more than one file.  Point lpDstText to a FNAME.
//***********************************************************************
BOOL WINPROC DlgCopyFilesProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, CopyFiles_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, CopyFiles_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_COPY_FILES);
	}  
}

//************************************************************************
LOCAL BOOL CopyFiles_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	int num, nBytes;
	FNAME szFile;
	FNAME szFileList;
	LPTSTR lpFileName;
	FNAME szDir;
	
	CenterPopup(hDlg);
	num = OpenFileList(lpBsr->OpenState.hBrowser);
	if (!num)
	{
		AstralDlgEnd(hDlg, FALSE);
		return(FALSE);
	}	
	nBytes = sizeof( szFileList ) - 2;
	*szFileList = _T('\0');

	while( GetNextFileName( szFile ) && nBytes > 0 )
	{
		lpFileName = filename( szFile );
		szFile[ nBytes ] = _T('\0');
		nBytes -= ( lstrlen( lpFileName ) * sizeof( TCHAR ));
		lstrcat(szFileList, lpFileName);
		nBytes -= sizeof( TCHAR );
		lstrcat( szFileList, _T(","));
	}
	StripTrailer( szFileList, _T(','));
	// get dst directory
	if( ALBMODE )
		lstrcpy(szDir, Browser.AlbumFileDir);
	else 
		CURRENTDIR(szDir, sizeof(szDir));
	FixPath(szDir);
	// fill items	
	if (ALBMODE)
		SetDlgItemText(hDlg, IDC_BSR_TEXT, _T("---"));
	else
		SetDlgItemText(hDlg, IDC_BSR_TEXT, szDir);

	SetDlgItemText(hDlg, IDC_FILESRC, szFileList);
	SetDlgItemText(hDlg, IDC_FILEDST, szDir);
	ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
	ShowDlgItem(hDlg, IDC_TOLABEL, NO);
	iFilesProcessed = iFilesToBeProcessed = 0;
	bAbort = NO;
	return(TRUE);
}		

//************************************************************************
LOCAL void CopyFiles_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	FNAME szFile, szDstText;
	FILELOC DstFile, SrcFile;

	switch(id)
	{
		case IDOK:
			if (iFilesToBeProcessed > 0)
				break;
			if (!GetDlgItemText(hDlg, IDC_FILEDST, szDstText,sizeof(FNAME)))
		   	{
				Message(IDS_EINVALIDDIR, (LPTSTR)_T(""));
				break;
		  	}		
			// get dest dir
			// verify files
			// dest must be a directory
			MakeFullPath(szDstText);
			FixPath(szDstText);
			lstrcpy(szFile, szDstText);
			RemovePathBackslash(szFile);
			if (!IsDirectory(szFile))
			{
				Message(IDS_EINVALIDDIR, (LPTSTR)szDstText);
				break;
			}
			// multiple file copy
			iFilesToBeProcessed = OpenFileList(lpBsr->OpenState.hBrowser);
			if (!iFilesToBeProcessed)
			{
				Message(IDS_NOSOURCEFILES);
				break;
			}
			bCancelled = NO;
			ShowDlgItem(hDlg, IDC_FROMLABEL, YES);
			ShowDlgItem(hDlg, IDC_TOLABEL, YES);
			ProgressBegin(1,0, hDlg);
			BSRInvalidateDriveIDs();
			bConfirm = Browser.ConfirmOnCopyFile;
			bConfirmOverwrite = YES;
			GetDriveVolume(szDstText, &DstFile.volume, YES);
			while(GetNextFile(&SrcFile))
			{
				lstrcpy(DstFile.szFile, szDstText);
				lstrcat(DstFile.szFile, filename(SrcFile.szFile));
				if (!CopyFileLocProc(&SrcFile, &DstFile, hDlg))
					break;
			}
			ProgressEnd();
			ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
			ShowDlgItem(hDlg, IDC_TOLABEL, NO);
			if ( bCancelled )
				iFilesToBeProcessed = 0;
			else	
				AstralDlgEnd( hDlg, iFilesProcessed > 0 );
		break;	
		case IDCANCEL:
			if (iFilesToBeProcessed)
			{
				bAbort = YES;
				break;
			}
			AstralDlgEnd( hDlg, iFilesProcessed > 0 );
		break;
		default:
		break;
	}  // switch command
}

//***********************************************************************
// 	Used for Copy.  
//	NEW VERSION
//***********************************************************************
BOOL WINPROC DlgCopyFileProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, CopyFile_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, CopyFile_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_COPY_FILE);
	}  
}

//************************************************************************
LOCAL BOOL CopyFile_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	int num;
	FNAME szFile;
	 
	CenterPopup(hDlg);
	num = OpenFileList(lpBsr->OpenState.hBrowser);
	// get source file and directory
	if (num <= 0)
	{
		lstrcpy(szFile, lpBsr->OpenState.szSearchSpec);		
		if (ALBMODE)
			lstrcpy(CopyFile_szDir, Browser.AlbumFileDir);
		else 
			CURRENTDIR(CopyFile_szDir, sizeof(CopyFile_szDir));
	}
	else
	{
		GetFirstFileName(szFile);	
		lstrcpy(CopyFile_szDir, szFile);
		stripfile(CopyFile_szDir);
	}
	FixPath(CopyFile_szDir);
	// fill items
	SetDlgItemText(hDlg, IDC_BSR_TEXT, CopyFile_szDir);
	SetDlgItemText(hDlg, IDC_FILESRC, filename(szFile));
	SetDlgItemText(hDlg, IDC_FILEDST, filename(szFile));
	ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
	ShowDlgItem(hDlg, IDC_TOLABEL, NO);
	iFilesProcessed = iFilesToBeProcessed = 0;
	bAbort = NO;
	return(0);
}

//************************************************************************
LOCAL void CopyFile_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	FNAME szFile, szSrcText, szDstText;
	BOOL bWild;

	switch(id)
	{
		case IDOK:
			if (iFilesToBeProcessed > 0)
				break;
			if (!GetDlgItemText(hDlg, IDC_FILESRC, szSrcText,sizeof(FNAME)))
		   	{
				Message(IDS_BADFILE, (LPTSTR)_T(""));
				break;
		  	}		
			if (!GetDlgItemText(hDlg, IDC_FILEDST, szDstText,sizeof(FNAME)))
		   	{
				Message(IDS_BADFILE, (LPTSTR)_T(""));
				break;
		  	}		
			// get source file
			lstrcpy(szFile, szSrcText);
			stripfile(szFile);
			if( *szFile == _T('\0'))
			{
				// append to directory
				FixFileName(szSrcText);
				lstrcpy(szFile, CopyFile_szDir);
				lstrcat(szFile, szSrcText);
				lstrcpy(szSrcText, szFile);
			}
			// get dest file
			lstrcpy(szFile, szDstText);
			stripfile(szFile);
			if( *szFile == _T('\0'))
			{
				// append to directory from source file
//				FixFileName(szDstText);
				lstrcpy(szFile, szSrcText);
				stripfile(szFile);
				lstrcat(szFile, szDstText);
				lstrcpy(szDstText, szFile);
			}
			lstrcpy(szFile, szDstText);
			RemovePathBackslash(szFile);
			if (!IsValidFileName(szFile))
			{
				Message(IDS_INVALIDNAME, (LPTSTR)szFile);
				break;
			}
			MakeFullPath(szDstText);
			MakeFullPath(szSrcText);
			// verify files
			lstrcpy(szFile, szSrcText);
			bWild = lStrChr(szFile, _T('*')) || lStrChr(szFile, _T('?'));
			if (bWild)
			{
				// wildcard, check directory only
				stripfile(szFile);
				RemovePathBackslash(szFile);
				if (!IsDirectory(szFile))
				{
					Message(IDS_EINVALIDDIR, (LPTSTR)szFile);
					break;
				}
			}
			else 
			if (!FileExists(szFile))
			{
					Message(IDS_BADFILE, (LPTSTR)szFile);
					break;
			}
			if (bWild)
			{
				// dest must be a directory
				FixPath(szDstText);
				RemovePathBackslash(szDstText);
				if (!IsDirectory(szDstText))
				{
					Message(IDS_EINVALIDDIR, (LPTSTR)szDstText);
					break;
				}
			}
			// single file copy
			iFilesToBeProcessed = EnumFiles(szSrcText, szDstText, NULL, (long)(LPVOID)hDlg);
			if (!iFilesToBeProcessed)
			{
				Message(IDS_NOSOURCEFILES, (LPTSTR)szSrcText);
				break;
			}
			bCancelled = NO;
			ShowDlgItem(hDlg, IDC_FROMLABEL, YES);
			ShowDlgItem(hDlg, IDC_TOLABEL, YES);
			ProgressBegin(1,0,hDlg);
			BSRInvalidateDriveIDs();
			bConfirm = Browser.ConfirmOnCopyFile;
			bConfirmOverwrite = YES;
			EnumFiles(szSrcText, szDstText, CopyFilesProc, (long)(LPVOID)hDlg);
			ProgressEnd();
			ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
			ShowDlgItem(hDlg, IDC_TOLABEL, NO);
			if ( bCancelled )
				iFilesToBeProcessed = 0;
			else
				AstralDlgEnd( hDlg, iFilesProcessed > 0 );
			break;	

		case IDCANCEL:
			if (iFilesToBeProcessed)
			{
				bAbort = YES;
				break;
			}
			AstralDlgEnd( hDlg, iFilesProcessed > 0 );
			break;

		default:
			break;
	}  // switch command
}

//***********************************************************************
LOCAL BOOL CopyFilesProc(LPTSTR lpSrcFile, LPTSTR lpDstFile, long lData, int iCount)
//***********************************************************************
{
	FILELOC SrcFile, DstFile;
	
	lstrcpy(SrcFile.szFile, lpSrcFile);
	GetDriveVolume(SrcFile.szFile, &SrcFile.volume, YES);
	lstrcpy(DstFile.szFile, lpDstFile);
	GetDriveVolume(DstFile.szFile, &DstFile.volume, YES);
	return(CopyFileLocProc(&SrcFile, &DstFile, (HWND)lData));
}

//***********************************************************************
LOCAL BOOL CopyFileLocProc(LPFILELOC lpSrcFile, LPFILELOC lpDstFile, HWND hDlg)
//***********************************************************************
{
	FNAME	szFromPath, szToPath;
	int	ret;
	MSG	msg;
	ULONG time;
	BOOL bLast;
	
	
	bLast = (iFilesProcessed+1 >= iFilesToBeProcessed);	 
	if (iFilesToBeProcessed > 1)
		{
		while ( !bAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			if ( !IsDialogMessage(hDlg, &msg))
				{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}
		if (bAbort)
			return(FALSE);
		}
	++iFilesProcessed;
	if (!VolumeCheck(lpSrcFile, NULL))
		return(TRUE);		//***ERROR  prompt user for disk
	if (!VolumeCheck(lpDstFile, NULL))
		return(TRUE);		//***ERROR	prompt user for disk
	lstrcpy(szFromPath, lpSrcFile->szFile);
	lstrcpy(szToPath, lpDstFile->szFile);
//	Lowercase(szFromPath);
//	Lowercase(szToPath);
	FixFileName( szFromPath );
	FixFileName( szToPath );
	if (StringsEqual(szToPath, szFromPath))
		if (AstralOKCancel(IDS_ECOPY_SELF, (LPTSTR)szFromPath) != IDOK)
			return(FALSE);
		else
			return(TRUE);
	if (bConfirm)
	{
		ret = AstralYesToAllStrings(hDlg, IDD_CONF_COPY, szFromPath, szToPath, !bLast);
		if (ret == IDC_YESTOALL)
			bConfirm = NO;
		else if (ret == IDCANCEL)
			return(FALSE);
		else if (ret != IDOK)
			return(TRUE);	
	}	
	if (FileExists(szToPath))
	{
		if (bConfirmOverwrite)
		{
			ret = AstralYesToAllString(hDlg, IDD_CONF_REPLACE, szToPath, !bLast);
			if (ret == IDC_YESTOALL)
				bConfirmOverwrite = NO;
			else if (ret == IDCANCEL)
			{
				bCancelled = YES;
				return(FALSE);
			}
			else if (ret != IDOK)
				return(TRUE);	
		}				 
		FileDelete(szToPath);
	}
	SetDlgItemTextNow(hDlg, IDC_FROMSTATUS, szFromPath);
	SetDlgItemTextNow(hDlg, IDC_TOSTATUS, szToPath);
	if (!CopyFile1(szFromPath, szToPath, YES, NO))
	{
		SetDlgItemTextNow(hDlg, IDC_FROMSTATUS, _T(" "));
		SetDlgItemTextNow(hDlg, IDC_TOSTATUS, _T(" "));
		if (AstralAffirm(IDS_ECOPY, (LPTSTR)szFromPath, (LPTSTR)szToPath))
			return(TRUE);
		bCancelled = YES;
		return(FALSE);
	}
	
	if (!SafeFileStats(szToPath, NULL, NULL ,&time,NULL))
		time = 0L;
	GetDriveVolume(szFromPath, &lpDstFile->volume, YES);
	CopyThumbnail(lpSrcFile, lpDstFile, time);

	SetDlgItemTextNow(hDlg, IDC_FROMSTATUS, _T(" "));
	SetDlgItemTextNow(hDlg, IDC_TOSTATUS, _T(" "));
	return(TRUE);
}


//***********************************************************************
// 	Used for Move.  
//	NEW VERSION
//***********************************************************************
BOOL WINPROC DlgMoveFileProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, MoveFile_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, MoveFile_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MOVE_FILE);
	}  
}


//************************************************************************
LOCAL BOOL MoveFile_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	int num;
	FNAME szFile;
	
	CenterPopup(hDlg);
	num = OpenFileList(lpBsr->OpenState.hBrowser);
	// get source file and directory
	if (num <= 0)
	{
		lstrcpy(szFile, lpBsr->OpenState.szSearchSpec);		
		if (ALBMODE)
			lstrcpy(MoveFile_szDir, Browser.AlbumFileDir);
		else 
			CURRENTDIR(MoveFile_szDir, sizeof(MoveFile_szDir));
	}
	else
	{
		GetFirstFileName(szFile);	
		lstrcpy(MoveFile_szDir, szFile);
		stripfile(MoveFile_szDir);
	}
	FixPath(MoveFile_szDir);
	// fill items
	SetDlgItemText(hDlg, IDC_BSR_TEXT, MoveFile_szDir);
	SetDlgItemText(hDlg, IDC_FILESRC, filename(szFile));
	SetDlgItemText(hDlg, IDC_FILEDST, filename(szFile));
	ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
	ShowDlgItem(hDlg, IDC_TOLABEL, NO);
	iFilesProcessed = iFilesToBeProcessed = 0;
	bAbort = NO;
	return(0);
}

//************************************************************************
LOCAL void MoveFile_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	FNAME szFile, szSrcText, szDstText;
	BOOL bWild;
	
	switch(id)
	{
		case IDOK:
			if (iFilesToBeProcessed > 0)
				break;
			if (!GetDlgItemText(hDlg, IDC_FILESRC, szSrcText,sizeof(FNAME)))
		   	{
				Message(IDS_BADFILE, (LPTSTR)_T(""));
				break;
		  	}		
			MakeFullPath(szSrcText);
			if (!GetDlgItemText(hDlg, IDC_FILEDST, szDstText,sizeof(FNAME)))
		   	{
				Message(IDS_BADFILE, (LPTSTR)_T(""));
				break;
		  	}		
			// get source file
			lstrcpy(szFile, szSrcText);
			stripfile(szFile);
			if( *szFile == _T('\0'))
			{
				// append to directory
				FixFileName(szSrcText);
				lstrcpy(szFile, MoveFile_szDir);
				lstrcat(szFile, szSrcText);
				lstrcpy(szSrcText, szFile);
			}
			// get dest file
			lstrcpy(szFile, szDstText);
			stripfile(szFile);
			if( *szFile == _T('\0'))
			{
				// append to directory from source file
				FixFileName(szDstText);
				lstrcpy(szFile, szSrcText);
				stripfile(szFile);
				lstrcat(szFile, szDstText);
				lstrcpy(szDstText, szFile);
			}
			lstrcpy(szFile, szDstText);
			RemovePathBackslash(szFile);
			if (!IsValidFileName(szFile))
			{
				Message(IDS_INVALIDNAME, (LPTSTR)szFile);
				break;
			}
			MakeFullPath(szDstText);
			MakeFullPath(szSrcText);
			// verify files
			lstrcpy(szFile, szSrcText);
			bWild = lStrChr(szFile, _T('*')) || lStrChr(szFile, _T('?'));
			if (bWild)
			{
				// wildcard, check directory only
				stripfile(szFile);
				RemovePathBackslash(szFile);
				if (!IsDirectory(szFile))
				{
					Message(IDS_EINVALIDDIR, (LPTSTR)szFile);
					break;
				}
			}
			else if (!FileExists(szFile))
			{
					Message(IDS_BADFILE, (LPTSTR)szFile);
					break;
			}
			if (bWild)
			{
				// dest must be a directory
				FixPath(szDstText);
				RemovePathBackslash(szDstText);
				if (!IsDirectory(szDstText))
				{
					Message(IDS_EINVALIDDIR, (LPTSTR)szDstText);
					break;
				}
			}
			WORD wSrc, wDst;
			if( IsDBCSLeadByte(( BYTE )*szSrcText ))
			{
				wSrc = *(( LPWORD )szSrcText );
		 	    wSrc = MBToUpperW(( LPTSTR )&wSrc );
			}
			else
			{
				wSrc = *szSrcText;
		 	    wSrc = MBToUpper(( LPTSTR )&wSrc );
			}

			if( IsDBCSLeadByte(( BYTE )*szDstText ))
			{
				wDst = *(( LPWORD )szDstText );
		 	    wDst = MBToUpperW(( LPTSTR )&wDst );
			}
			else
			{
				wDst = *szDstText;
		 	    wDst = MBToUpper(( LPTSTR )&wDst );
			}

			if( wSrc != wDst )
			{
				Message( IDS_ECOPYCROSSDRIVE );
				break;
			}
			// single file copy
			iFilesToBeProcessed = EnumFiles(szSrcText, szDstText, NULL, (long)(LPVOID)hDlg);
			if (!iFilesToBeProcessed)
			{
				Message(IDS_NOSOURCEFILES, (LPTSTR)szSrcText);
				break;
			}
			bCancelled = NO;
			ShowDlgItem(hDlg, IDC_FROMLABEL, YES);
			ShowDlgItem(hDlg, IDC_TOLABEL, YES);
			ProgressBegin(1,0,hDlg);
			bConfirm = Browser.ConfirmOnMoveFile;
			bConfirmOverwrite = YES;
			EnumFiles(szSrcText, szDstText, MoveFilesProc, (long)(LPVOID)hDlg);
			ProgressEnd();
			ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
			ShowDlgItem(hDlg, IDC_TOLABEL, NO);
			if ( bCancelled )
				iFilesToBeProcessed = 0;
			else	AstralDlgEnd( hDlg, iFilesProcessed > 0 );
		break;	
		case IDCANCEL:
			if (iFilesToBeProcessed)
			{
				bAbort = YES;
				break;
			}
			AstralDlgEnd( hDlg, iFilesProcessed > 0 );
		break;
		default:
		break;
	}  // switch command
}


//***********************************************************************
// 	Used for Move.  
//	NEW VERSION
//***********************************************************************
BOOL WINPROC DlgMoveFilesProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, MoveFiles_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, MoveFiles_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_MOVE_FILES);
	}  
}

//************************************************************************
LOCAL BOOL MoveFiles_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	int num, nBytes;
	FNAME szFile;
	FNAME szFileList;
	LPTSTR lpFileName;
	FNAME szDir;
	
	CenterPopup(hDlg);
	num = OpenFileList(lpBsr->OpenState.hBrowser);
	if (!num)
	{
		AstralDlgEnd(hDlg, FALSE);
		return(FALSE);
	}	

	nBytes = sizeof(szFileList) - 2;

	*szFileList = _T('\0');

	while( GetNextFileName( szFile ) && nBytes > 0 )
	{
		lpFileName = filename( szFile );
		szFile[ nBytes ] = _T('\0');
		nBytes -= ( lstrlen( lpFileName ) * sizeof( TCHAR ));
		lstrcat(szFileList, lpFileName);
		nBytes -= sizeof( TCHAR );
		lstrcat( szFileList, _T(","));
	}

	StripTrailer(szFileList, _T(','));
	// get dst directory
	if (ALBMODE)
		lstrcpy(szDir, Browser.AlbumFileDir);
	else 
		CURRENTDIR(szDir, sizeof(szDir));
	FixPath(szDir);
	// fill items	
	if (ALBMODE)
		SetDlgItemText(hDlg, IDC_BSR_TEXT, _T("---"));
	else
		SetDlgItemText(hDlg, IDC_BSR_TEXT, szDir);
	SetDlgItemText(hDlg, IDC_FILESRC, szFileList);
	SetDlgItemText(hDlg, IDC_FILEDST, szDir);
	ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
	ShowDlgItem(hDlg, IDC_TOLABEL, NO);
	iFilesProcessed = iFilesToBeProcessed = 0;
	bAbort = NO;
	return(TRUE);
}	
	
//************************************************************************
LOCAL void MoveFiles_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	FNAME szFile, szDstText;
	FILELOC DstFile, SrcFile;
	BOOL bYTA, ret;

	switch(id)
	{
		case IDOK:
			if (iFilesToBeProcessed > 0)
				break;
			if (!GetDlgItemText(hDlg, IDC_FILEDST, szDstText,sizeof(FNAME)))
		   	{
				Message(IDS_EINVALIDDIR, (LPTSTR)_T(""));
				break;
		  	}		
			// get dest dir
			// verify files
			// dest must be a directory
			// dest must be a directory
			MakeFullPath(szDstText);
			FixPath(szDstText);
			lstrcpy(szFile, szDstText);
			RemovePathBackslash(szFile);
			if (!IsDirectory(szFile))
			{
				Message(IDS_EINVALIDDIR, (LPTSTR)szDstText);
				break;
			}
			// multiple file copy
			iFilesToBeProcessed = OpenFileList(lpBsr->OpenState.hBrowser);
			if (!iFilesToBeProcessed)
			{
				Message(IDS_NOSOURCEFILES);
				break;
			}
			bCancelled = NO;
			ShowDlgItem(hDlg, IDC_FROMLABEL, YES);
			ShowDlgItem(hDlg, IDC_TOLABEL, YES);
			ProgressBegin(1,0,hDlg);
			BSRInvalidateDriveIDs();
			bConfirm = Browser.ConfirmOnMoveFile;
			bConfirmOverwrite = YES;
			GetDriveVolume(szDstText, &DstFile.volume, YES);
			bYTA=FALSE;

			WORD wSrc, wDst;
			while( GetNextFile(&SrcFile ))
			{
				lstrcpy(DstFile.szFile, szDstText);
				lstrcat(DstFile.szFile, filename(SrcFile.szFile));

				if( IsDBCSLeadByte(( BYTE )*( SrcFile.szFile )))
				{
					wSrc = *(( LPWORD )SrcFile.szFile );
		 	    	wSrc = MBToUpperW(( LPTSTR )&wSrc );
				}
				else
				{
					wSrc = *( SrcFile.szFile );
		 	    	wSrc = MBToUpper(( LPTSTR )&wSrc );
				}

				if( IsDBCSLeadByte(( BYTE )*( DstFile.szFile )))
				{
					wDst = *(( LPWORD )DstFile.szFile );
		 	    	wDst = MBToUpperW(( LPTSTR )&wDst );
				}
				else
				{
					wDst = *( DstFile.szFile );
		 	    	wDst = MBToUpper(( LPTSTR )&wDst );
				}

				if( wSrc != wDst )
				{
					if (bYTA)
					{
						++iFilesProcessed;
						continue;
					}
					ret = AstralYesToAllStrings(hDlg, IDD_ECOPYCROSSDRIVE, SrcFile.szFile, DstFile.szFile, !(iFilesProcessed+1 >= iFilesToBeProcessed));
					++iFilesProcessed;
					if (ret == IDC_YESTOALL)
						bYTA = TRUE;
					else if (ret == IDCANCEL)
						break;
					continue;
				}
				if( !MoveFileLocProc( &SrcFile, &DstFile, hDlg ))
					break;
			}
			ProgressEnd();
			ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
			ShowDlgItem(hDlg, IDC_TOLABEL, NO);
			if ( bCancelled )
				iFilesToBeProcessed = 0;
			else	
				AstralDlgEnd( hDlg, iFilesProcessed > 0 );
		break;	
		case IDCANCEL:
			if (iFilesToBeProcessed)
			{
				bAbort = YES;
				break;
			}
			AstralDlgEnd( hDlg, iFilesProcessed > 0 );
		break;
		default:
		break;
	}  // switch command
}

//***********************************************************************
LOCAL BOOL MoveFilesProc(LPTSTR lpSrcFile, LPTSTR lpDstFile, long lData, int iCount)
//***********************************************************************
{
	FILELOC SrcFile, DstFile;
	
	lstrcpy(SrcFile.szFile, lpSrcFile);
	GetDriveVolume(SrcFile.szFile, &SrcFile.volume, YES);
	lstrcpy(DstFile.szFile, lpDstFile);
	GetDriveVolume(DstFile.szFile, &DstFile.volume, YES);
	return(MoveFileLocProc(&SrcFile, &DstFile, (HWND)lData));
}

//***********************************************************************
LOCAL BOOL MoveFileLocProc(LPFILELOC lpSrcFile, LPFILELOC lpDstFile, HWND hDlg)
//***********************************************************************
{
	FNAME	szFromPath, szToPath;
	int		ret;
	MSG		msg;
	ENTRY 	entry;
	BOOL 	bLast;
	
	bLast = (iFilesProcessed+1 >= iFilesToBeProcessed);
	if (iFilesToBeProcessed > 1)
		{
		while ( !bAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			if ( !IsDialogMessage(hDlg, &msg))
				{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}
		if (bAbort)
			return(FALSE);
		}
	++iFilesProcessed;
	if (!VolumeCheck(lpSrcFile, NULL))
		return(TRUE);		//***ERROR  prompt user for disk
	if (!VolumeCheck(lpDstFile, NULL))
		return(TRUE);		//***ERROR	prompt user for disk
	lstrcpy(szFromPath, lpSrcFile->szFile);
	lstrcpy(szToPath, lpDstFile->szFile);
//	Lowercase(szFromPath);
//	Lowercase(szToPath);
	if (StringsEqual(szToPath, szFromPath))
		if (AstralOKCancel(IDS_EMOVE_SELF, (LPTSTR)szFromPath) != IDOK)
			return(FALSE);
		else
			return(TRUE);
	if (bConfirm)
	{
		ret = AstralYesToAllStrings(hDlg, IDD_CONF_MOVE, szFromPath, szToPath, !bLast);
		if (ret == IDC_YESTOALL)
			bConfirm = NO;
		else if (ret == IDCANCEL)
			return(FALSE);
		else if (ret != IDOK)
			return(TRUE);	
	}	
	
	if (FileExists(szToPath))
	{
		if (bConfirmOverwrite)
		{
			ret = AstralYesToAllString(hDlg, IDD_CONF_REPLACE, szToPath, !bLast);
			if (ret == IDC_YESTOALL)
				bConfirmOverwrite = NO;
			else if (ret == IDCANCEL)
			{
				bCancelled = YES;
				return(FALSE);
			}
			else if (ret != IDOK)
				return(TRUE);	
		}				 
		FileDelete(szToPath);
	}
	SetDlgItemTextNow(hDlg, IDC_FROMSTATUS, szFromPath);
	SetDlgItemTextNow(hDlg, IDC_TOSTATUS, szToPath);
	if (!RenameFile(szFromPath, szToPath))
	{
		SetDlgItemTextNow(hDlg, IDC_FROMSTATUS, _T(" "));
		SetDlgItemTextNow(hDlg, IDC_TOSTATUS, _T(" "));
		if (AstralAffirm(IDS_EMOVE, (LPTSTR)szFromPath, (LPTSTR)szToPath))
			return(TRUE);
		bCancelled = YES;
		return(FALSE);
	}
	
		// copy the file 
	if (FindEntry(lpSrcFile, &entry, FALSE, YES))
	{
		lstrcpy(entry.File.szFile, szToPath);
		entry.File.volume = lpSrcFile->volume;
		if (!ChangeEntry(lpSrcFile, &entry))
			Message(IDS_EMOVETHUMBNAIL, (LPTSTR)lpSrcFile);
	}

	SetDlgItemTextNow(hDlg, IDC_FROMSTATUS, _T(" "));
	SetDlgItemTextNow(hDlg, IDC_TOSTATUS, _T(" "));
	return(TRUE);
}


//***********************************************************************
// 	Used for Delete.  
//	NEW VERSION
//***********************************************************************
BOOL WINPROC DlgDeleteFileProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, DeleteFile_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, DeleteFile_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_DELETE_FILE);
	}  
}

//************************************************************************
LOCAL BOOL DeleteFile_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	int num;
	FNAME szFile;
	
	CenterPopup(hDlg);
	num = OpenFileList(lpBsr->OpenState.hBrowser);
	// get source file and directory
	if (num <= 0)
	{
		lstrcpy(szFile, lpBsr->OpenState.szSearchSpec);		
		if (ALBMODE)
			lstrcpy(DeleteFile_szDir, Browser.AlbumFileDir);
		else 
			CURRENTDIR(DeleteFile_szDir, sizeof(DeleteFile_szDir));
	}
	else
	{
		GetFirstFileName(szFile);	
		lstrcpy(DeleteFile_szDir, szFile);
		stripfile(DeleteFile_szDir);
	}
	FixPath(DeleteFile_szDir);
	// fill items
	SetDlgItemText(hDlg, IDC_BSR_TEXT, DeleteFile_szDir);
	SetDlgItemText(hDlg, IDC_FILESRC, filename(szFile));
	ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
	iFilesProcessed = iFilesToBeProcessed = 0;
	bAbort = NO;
	return(TRUE);
}	
	
//************************************************************************
LOCAL void DeleteFile_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	FNAME szFile, szSrcText;
	BOOL bWild;
	
	switch(id)
	{
		case IDOK:
			if (iFilesToBeProcessed > 0)
				break;
			if (!GetDlgItemText(hDlg, IDC_FILESRC, szSrcText,sizeof(FNAME)))
		   	{
				Message(IDS_BADFILE, (LPTSTR)_T(""));
				break;
		  	}		
			// get source file
			lstrcpy(szFile, szSrcText);
			stripfile(szFile);
			if( *szFile == _T('\0'))
			{
				// append to directory
				FixFileName(szSrcText);
				lstrcpy(szFile, DeleteFile_szDir);
				lstrcat(szFile, szSrcText);
				lstrcpy(szSrcText, szFile);
			}
			lstrcpy(szFile, szSrcText);
			RemovePathBackslash(szFile);
			if (!IsValidFileName(szFile))
			{
				Message(IDS_INVALIDNAME, (LPTSTR)szFile);
				break;
			}
			MakeFullPath(szSrcText);
			// verify files
			lstrcpy(szFile, szSrcText);

			bWild = MBStrchr(szFile, _T('*')) || MBStrchr(szFile, _T('?'));
			if (bWild)
			{
				// wildcard, check directory only
				stripfile(szFile);
				RemovePathBackslash(szFile);
				if (!IsDirectory(szFile))
				{
					Message(IDS_EINVALIDDIR, (LPTSTR)szFile);
					break;
				}
				
			}
			else if (!FileExists(szFile))
			{
					Message(IDS_BADFILE, (LPTSTR)szFile);
					break;
			}
			iFilesToBeProcessed = EnumFiles(szSrcText, NULL, NULL, (long)(LPVOID)hDlg);
			if (!iFilesToBeProcessed)
			{
				Message(IDS_NOSOURCEFILES, (LPTSTR)szSrcText);
				break;
			}
			bCancelled = NO;
			ShowDlgItem(hDlg, IDC_FROMLABEL, YES);
			ProgressBegin(1,0,hDlg);
			bConfirm = Browser.ConfirmOnDelFile;
			EnumFiles(szSrcText, NULL, DeleteFilesProc, (long)(LPVOID)hDlg);
			ProgressEnd();
			ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
			if ( bCancelled )
				iFilesToBeProcessed = 0;
			else	AstralDlgEnd( hDlg, iFilesProcessed > 0 );
		break;	
		case IDCANCEL:
			if (iFilesToBeProcessed)
			{
				bAbort = YES;
				break;
			}
			AstralDlgEnd( hDlg, iFilesProcessed > 0 );
		break;
		default:
		break;
	}  // switch command
}


//***********************************************************************
// 	Used for Delete.  
//	NEW VERSION
//***********************************************************************
BOOL WINPROC DlgDeleteFilesProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, DeleteFiles_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, DeleteFiles_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_DELETE_FILES);
	}  
 }
 
//************************************************************************
LOCAL BOOL DeleteFiles_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
 	int num, nBytes;
	FNAME szFile;
	FNAME szFileList;
	LPTSTR lpFileName;
	FNAME szDir;
	
	CenterPopup(hDlg);
	num = OpenFileList(lpBsr->OpenState.hBrowser);
	if (!num)
	{
		AstralDlgEnd(hDlg, FALSE);
		return(FALSE);
	}	

	nBytes = sizeof(szFileList) - 2;
	*szFileList = _T('\0');

	while( GetNextFileName( szFile ) && nBytes > 0 )
	{
		lpFileName = filename( szFile );
		szFile[ nBytes ] = _T('\0');
		nBytes -= ( lstrlen( lpFileName ) * sizeof( TCHAR ));
		lstrcat(szFileList, lpFileName);
		nBytes -= sizeof( TCHAR );
		lstrcat( szFileList, _T(","));
	}

	StripTrailer(szFileList, _T(','));
	// get dst directory
	if (ALBMODE)
		lstrcpy(szDir, Browser.AlbumFileDir);
	else 
		CURRENTDIR(szDir, sizeof(szDir));
	FixPath(szDir);
	// fill items	
	if (ALBMODE)
		SetDlgItemText(hDlg, IDC_BSR_TEXT, _T("---"));
	else
		SetDlgItemText(hDlg, IDC_BSR_TEXT, szDir);
	SetDlgItemText(hDlg, IDC_FILESRC, szFileList);
	ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
	iFilesProcessed = iFilesToBeProcessed = 0;
	bAbort = NO;
	return(TRUE);
}

//************************************************************************
LOCAL void DeleteFiles_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	FILELOC SrcFile;
	
	switch(id)
	{
		case IDOK:
			if (iFilesToBeProcessed > 0)
				break;
			// multiple file copy
			iFilesToBeProcessed = OpenFileList(lpBsr->OpenState.hBrowser);
			if (!iFilesToBeProcessed)
			{
				Message(IDS_NOSOURCEFILES);
				break;
			}
			bCancelled = NO;
			ShowDlgItem(hDlg, IDC_FROMLABEL, YES);
			ProgressBegin(1,0,hDlg);
			BSRInvalidateDriveIDs();
			bConfirm = Browser.ConfirmOnDelFile;
			while(GetNextFile(&SrcFile))
			{
				if (!DeleteFileLocProc(&SrcFile, hDlg))
					break;
			}
			ProgressEnd();
			ShowDlgItem(hDlg, IDC_FROMLABEL, NO);
			if ( bCancelled )
				iFilesToBeProcessed = 0;
			else	AstralDlgEnd( hDlg, iFilesProcessed > 0 );
		break;	
		case IDCANCEL:
			if (iFilesToBeProcessed)
			{
				bAbort = YES;
				break;
			}
			AstralDlgEnd( hDlg, iFilesProcessed > 0 );
		break;
		default:
		break;
	}  // switch command
}

//***********************************************************************
LOCAL BOOL DeleteFilesProc(LPTSTR lpSrcFile, LPTSTR lpDstFile, long lData, int iCount)
//***********************************************************************
{
	FILELOC SrcFile;
	
	lstrcpy(SrcFile.szFile, lpSrcFile);
	GetDriveVolume(SrcFile.szFile, &SrcFile.volume, YES);
	return(DeleteFileLocProc(&SrcFile, (HWND)lData));
}

//***********************************************************************
LOCAL BOOL DeleteFileLocProc(LPFILELOC lpFile, HWND hDlg)
//***********************************************************************
{
	FNAME	szFile;
	int	ret;
	MSG	msg;
	ENTRY entry;
	BOOL bLast;
	
	bLast = (iFilesProcessed+1 >= iFilesToBeProcessed);
	if (iFilesToBeProcessed > 1)
		{
		while ( !bAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			if ( !IsDialogMessage(hDlg, &msg))
				{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}
		if (bAbort)
			return(FALSE);
		}
	++iFilesProcessed;
	if (!VolumeCheck(lpFile, NULL))
		return(TRUE);		//***ERROR  prompt user for disk
	lstrcpy(szFile, lpFile->szFile);
//	Lowercase(szFile);
	if (bConfirm)
	{
		ret = AstralYesToAllString(hDlg, IDD_CONF_FILEDEL, szFile, !bLast);
		if (ret == IDC_YESTOALL)
			bConfirm = NO;
		else if (ret == IDCANCEL)
			return(FALSE);
		else if (ret != IDOK)
			return(TRUE);	
	}	
	
	SetDlgItemTextNow(hDlg, IDC_FROMSTATUS, szFile);
	// delete file
	if (!FileDelete(szFile))
	{
		SetDlgItemTextNow(hDlg, IDC_FROMSTATUS, _T(" "));
		if (AstralAffirm(IDS_EDELETE, (LPTSTR)szFile))
			return(TRUE);
		bCancelled = YES;
		return(FALSE);
	}
	// Delete Thumbnail
	if (FindEntry(lpFile, &entry, TRUE, ALBMODE))
		if(!CompleteDeleteEntry(&entry))
			Message(IDS_EDELETETHUMBNAIL);
	++iFilesProcessed;
	
	SetDlgItemTextNow(hDlg, IDC_FROMSTATUS, _T(" "));
	return(TRUE);
}


//***********************************************************************
//***********************************************************************
BOOL WINPROC DlgRenameFileProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, RenameFile_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, RenameFile_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_RENAME_FILE);
	}  
}

//************************************************************************
LOCAL BOOL RenameFile_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	FNAME szDir;
	
	CenterPopup(hDlg);
	lstrcpy(szDir, lpSrcText);
	stripfile(szDir);
	SetDlgItemText(hDlg, IDC_BSR_TEXT, szDir);
	SetDlgItemText(hDlg, IDC_FILESRC, filename(lpSrcText));
	SetDlgItemText(hDlg, IDC_FILEDST, lpDstText);
	return(TRUE);
}

//************************************************************************
LOCAL void RenameFile_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	switch(id)
	{
		case IDC_BSR_THMDIR:
		break;
		case IDOK:
			if (lpDstText)
				if (!GetDlgItemText(hDlg, IDC_FILEDST, lpDstText, sizeof(FNAME)))
				{
					AstralDlgEnd(hDlg, FALSE);
					break;
				}
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, FALSE);
		break;
		default:
		break;
	}  // switch command
}
	
//***********************************************************************
//***********************************************************************
BOOL WINPROC DlgCleanupThumbsProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, CleanThm_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, CleanThm_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_DELETE_FILE);
	}  
}

//************************************************************************
LOCAL BOOL CleanThm_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	LPDBFLL lpDBFLL;
	FNAME szDir, szAlb;
	
	CenterPopup(hDlg);
	CleanThm_idWhat = IDC_CLEAN_DIR;
	CleanThm_bCleanSubDir = FALSE;
	CheckRadioButton(hDlg,IDC_CLEAN_DIR,IDC_CLEAN_ALL, CleanThm_idWhat);
	CheckDlgButton(hDlg,IDC_CLEAN_SUBDIR, CleanThm_bCleanSubDir);
	ControlEnable(hDlg, IDC_CLEAN_DIR_TEXT,CleanThm_idWhat==IDC_CLEAN_DIR);
	ControlEnable(hDlg, IDC_CLEAN_SUBDIR,CleanThm_idWhat==IDC_CLEAN_DIR);
	ControlEnable(hDlg, IDC_CLEAN_ALB_TEXT,CleanThm_idWhat==IDC_CLEAN_ALB);
	// setup directory and album
	if (ALBMODE)
	{
		lstrcpy(szDir, Browser.AlbumFileDir);
		if (!(lpDBFLL = GetSelectedAlbum(lpBsr->OpenState.hBrowser)))
			if (!(lpDBFLL = FindAlbum(lpBsr->OpenState.szAlbum)))
				lpDBFLL = GetAlb();
		if (!lpDBFLL)
			*szAlb = _T('\0');
		else
			lstrcpy(szAlb, lpDBFLL->dbf.info.szName);
	}
	else 
	{
		CURRENTDIR(szDir, sizeof(szDir));
		*szAlb = _T('\0');
	}
	FixPath(szDir);
	SetDlgItemText(hDlg, IDC_CLEAN_DIR_TEXT, szDir);
	SetDlgItemText(hDlg, IDC_CLEAN_ALB_TEXT, szAlb);
	return(TRUE);
}

//************************************************************************
LOCAL void CleanThm_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	LPDBFLL lpDBFLL;
	FNAME szDir, szAlb;
	int len;
	DBFS dbf;
	VOLUMEID volume;
	FINDDATA afile;
	BOOL bContinue, bFound;
	
	switch(id)
	{
		case IDC_CLEAN_DIR:
		case IDC_CLEAN_ALL:
		case IDC_CLEAN_ALB:
			CheckRadioButton(hDlg,IDC_CLEAN_DIR,IDC_CLEAN_ALL, CleanThm_idWhat=id);
			ControlEnable(hDlg, IDC_CLEAN_DIR_TEXT,CleanThm_idWhat==IDC_CLEAN_DIR);
			ControlEnable(hDlg, IDC_CLEAN_SUBDIR,CleanThm_idWhat==IDC_CLEAN_DIR);
			ControlEnable(hDlg, IDC_CLEAN_ALB_TEXT,CleanThm_idWhat==IDC_CLEAN_ALB);
		break;
		case IDC_CLEAN_SUBDIR:
			CleanThm_bCleanSubDir = !CleanThm_bCleanSubDir;
			CheckDlgButton(hDlg,id, CleanThm_bCleanSubDir);
		break;
		case IDOK:
			bContinue = FALSE;
			switch(CleanThm_idWhat)
			{	
				case IDC_CLEAN_DIR:
					GetDlgItemText(hDlg, IDC_CLEAN_DIR_TEXT, szDir, sizeof(szDir));
					MakeFullPath(szDir);
					if (!IsDirectory(szDir))
					{
						Message(IDS_EINVALIDDIR, (LPTSTR)szDir);
						bContinue = TRUE;
						break;
					}
					FixPath(szDir);
					len = lstrlen(szDir);
					if( len )
						len -= sizeof( TCHAR );
					ProgressBegin(1,0,hDlg);
					BSRInvalidateDriveIDs();
					// do directories
					DBFBeginDirInfo(&afile, szDir[0]);
					while(DBFNextInfo(&dbf, &afile,YES, NO))
					{
						if (CleanThm_bCleanSubDir)
							bFound = StringsEqualN(szDir, dbf.info.szName, len);
						else
							bFound = StringsEqual(szDir, dbf.info.szName);
						if (bFound)	    
						{
							GetDriveVolume(dbf.info.szName, &volume, NO);
							if (EqualVolume(&dbf.info.volume,  &volume))
								CleanupDBF(&dbf, VL_DATAONLY|VL_CLEAN);	//***ERROR if fails??
						}
						DBFClose(&dbf);
					} 
					ProgressEnd();
				break;
				case IDC_CLEAN_ALB:
					GetDlgItemText(hDlg, IDC_CLEAN_ALB_TEXT, szAlb, sizeof(szAlb));
					lpDBFLL	= FindAlbum(szAlb);
					if (!lpDBFLL)
					{
						Message(IDS_BADALBNAME);
						bContinue = TRUE;
						break;
					}
					ProgressBegin(1,0,hDlg);
					CleanupDBF(&lpDBFLL->dbf, VL_DATAONLY|VL_CLEAN);	//***ERROR if fails??
					ProgressEnd();
				break;
				case IDC_CLEAN_ALL:
					ProgressBegin(1,0,hDlg);
					// do albums
					DBFBeginInfo(&afile, ID_ALBUM);
					while(DBFNextInfo(&dbf, &afile,YES, NO)) 
					{
						CleanupDBF(&dbf, VL_DATAONLY|VL_CLEAN);	//***ERROR if fails??
						DBFClose(&dbf);
					} 
					BSRInvalidateDriveIDs();
					// do directories
					DBFBeginInfo(&afile, ID_DIRECTORY);
					while(DBFNextInfo(&dbf, &afile,YES, NO))
					{
						GetDriveVolume(dbf.info.szName, &volume, NO);
						if (EqualVolume(&dbf.info.volume,  &volume))
							CleanupDBF(&dbf, VL_DATAONLY|VL_CLEAN);	//***ERROR if fails??
						DBFClose(&dbf);
					} 
					ProgressEnd();
				break;	// end handle clean all
			}
			if (!bContinue)
				AstralDlgEnd(hDlg, TRUE);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, FALSE);
		break;
		default:
		break;
	}  // switch command
}

//***********************************************************************
LOCAL BOOL PrintThumbs_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void PrintThumbs_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);
//***********************************************************************
//***********************************************************************
BOOL WINPROC DlgPrintThumbsProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, PrintThumbs_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, PrintThumbs_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_PRINT_THUMBS);
	}  
}

static BOOL bSelectedOnly;
static LFIXED PTwidth, PTheight;
static BOOL bPTChanging;

//************************************************************************
LOCAL BOOL PrintThumbs_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	CenterPopup(hDlg);
	Browser.bPTRows = (Browser.bPTRows != FALSE);
	CheckRadioButton(hDlg,IDC_PT_USEROWS,IDC_PT_USECOLUMNS, IDC_PT_USECOLUMNS-Browser.bPTRows);
	CheckDlgButton( hDlg, IDC_PT_SELECTED, bSelectedOnly = FALSE);
	if (!SetupPrintThumbs(&PTwidth, &PTheight))
	{
	 	AstralDlgEnd(hDlg, FALSE);
	 	return(FALSE);
	}
	Browser.PTNumRows = Bound(Browser.PTNumRows, 1, 99);
	Browser.PTNumCols = Bound(Browser.PTNumCols, 1, 99);
	bPTChanging = TRUE;
	InitDlgItemSpin( hDlg, IDC_PT_ROWS, Browser.PTNumRows, YES, 1, 99);
	InitDlgItemSpin( hDlg, IDC_PT_COLUMNS, Browser.PTNumCols, YES, 1, 99);
	bPTChanging = FALSE;
	if (Browser.bPTRows)
		InitDlgItemSpin( hDlg, IDC_PT_ROWS, Browser.PTNumRows, YES, 1, 99);
	else
	 	InitDlgItemSpin( hDlg, IDC_PT_COLUMNS, Browser.PTNumCols, YES, 1, 99);
	ControlEnable(hDlg, IDC_PT_COLUMNS, !Browser.bPTRows);
	ControlEnable(hDlg, IDC_PT_ROWS, Browser.bPTRows);
	return(TRUE);
}		

//************************************************************************
LOCAL void PrintThumbs_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	BOOL bOldChanging, Bool;
	
	switch(id)
	{
		case IDC_PT_USEROWS:
		case IDC_PT_USECOLUMNS:
			Browser.bPTRows = (id == IDC_PT_USEROWS);
			CheckRadioButton(hDlg,IDC_PT_USEROWS,IDC_PT_USECOLUMNS, IDC_PT_USECOLUMNS-Browser.bPTRows);
			ControlEnable(hDlg, IDC_PT_ROWS, Browser.bPTRows);
			ControlEnable(hDlg, IDC_PT_COLUMNS, !Browser.bPTRows);
			SetDlgItemSpin( hDlg, IDC_PT_COLUMNS, Browser.PTNumCols, NO );
			SetDlgItemSpin( hDlg, IDC_PT_ROWS, Browser.PTNumRows, NO );
		break;
		case IDC_PT_ROWS: 
		case IDC_PT_COLUMNS:
			if (bPTChanging)
				break;
			bOldChanging = bPTChanging;
			bPTChanging = TRUE;
			Browser.PTNumRows =  GetDlgItemSpin( hDlg, IDC_PT_ROWS, &Bool, NO);
			Browser.PTNumCols =  GetDlgItemSpin( hDlg, IDC_PT_COLUMNS, &Bool, NO);
			if (Browser.bPTRows)
			{
				Browser.PTNumCols = (Browser.PTNumRows*PTwidth)/PTheight;
				Browser.PTNumCols = Bound(Browser.PTNumCols, 1, 99);
				SetDlgItemSpin( hDlg, IDC_PT_COLUMNS, Browser.PTNumCols, NO );
			}
			else
			{
				Browser.PTNumRows = (Browser.PTNumCols*PTheight)/PTwidth;
				Browser.PTNumRows = Bound(Browser.PTNumRows, 1, 99);
				SetDlgItemSpin( hDlg, IDC_PT_ROWS, Browser.PTNumRows, NO );
			}
			bPTChanging = bOldChanging;
		break;
		case IDC_PT_SELECTED:
			bSelectedOnly = !bSelectedOnly;
			CheckDlgButton( hDlg, id, bSelectedOnly);
		break;
		case IDOK:
			PrintThumbs(hDlg, bSelectedOnly);
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, FALSE);
		break;
		default:
		break;
	}  // switch command
}

//***********************************************************************
//***********************************************************************
BOOL WINPROC DlgDeleteVirtualProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, DeleteVirtual_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, DeleteVirtual_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_DELETE_VIRTUAL);
	}  
}

//************************************************************************
LOCAL BOOL DeleteVirtual_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	int num, nBytes;
	FNAME szFile, szFileList;
	LPTSTR lpFileName;

	CenterPopup(hDlg);
	num = OpenFileList(lpBsr->OpenState.hBrowser);
	if (!num)
	{
		AstralDlgEnd(hDlg, FALSE);
		return(FALSE);
	}	
	nBytes = sizeof(szFileList) - 2;
	*szFileList = _T('\0');

	while( GetNextFileName( szFile ) && nBytes > 0 )
	{
		lpFileName = filename( szFile );
		szFile[ nBytes ] = _T('\0');
		nBytes -= ( lstrlen( lpFileName ) * sizeof( TCHAR ));
		lstrcat(szFileList, lpFileName);
		nBytes -= sizeof( TCHAR );
		lstrcat( szFileList, _T(","));
	}

	StripTrailer(szFileList, _T(','));
	// fill items	   
	SetDlgItemText(hDlg, IDC_BSR_TEXT, lpSrcText);
	SetDlgItemText(hDlg, IDC_FILESRC, szFileList);
	return(TRUE);
}		

//************************************************************************
LOCAL void DeleteVirtual_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	switch(id)
	{
		case IDOK:
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, FALSE);
		break;
		default:
		break;
	}  // switch command
}
	
//***********************************************************************
//***********************************************************************
BOOL WINPROC DlgGetBsrStringProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{						    
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, GetBsrString_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, GetBsrString_OnCommand);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_DELETE_FILE);
	}  
}

//************************************************************************
LOCAL BOOL GetBsrString_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************
{
	CenterPopup(hDlg);
	if (lpSrcText)
		SetDlgItemText(hDlg, IDC_BSR_TEXT, lpSrcText);
	if (lpInitText)
		SetDlgItemText(hDlg, IDC_DST_TEXT, lpInitText);
	return(TRUE);
}	

//************************************************************************
LOCAL void GetBsrString_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	switch(id)
	{
		case IDOK:
			if (lpDstText)
				if (!GetDlgItemText(hDlg, IDC_DST_TEXT, lpDstText, sizeof(FNAME)))
				{
					AstralDlgEnd(hDlg, FALSE);
					return;
				}
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, FALSE);
		break;
		default:
		break;
	}  // switch command
}
	


//************************************************************************/
// 	Fills the File info dialog with info on the file.
// 	Returns the thumbnail (or null) used. The thumbnail MUST be kept around
//		for its preview frame.
//	If a frame is used with no thumbnail (ie Photo CD) then lpFrame will point to the frame.
//	It is the callers responsibility to free the thumbnail when it is no longer 
//		needed.
//************************************************************************/
LOCAL LPCTHMNAIL FillFileInfo(HWND hInfo, LPFILELOC lpFile)
//************************************************************************/
{
	ENTRY entry;
	LPCTHMNAIL lpThm, lpCDThm;
	FNAME szBlank, szString, szFormat, szValue, szUnit;
	LPTSTR lpBlank, lpString, lpUnit;
	int OldUnitRes;
	struct tm *tminfo;
	LPTSTR lpDescr;
	LONG time;
	LONG filesize, xsize, ysize, fileRes;
	int fileType, dataType;
	LPFRAME lpFrame;
	HWND 	hControl;
	BOOL bCDThm, ret;

	lpThm  = NULL;
	bCDThm = FALSE;
	
	// Get string to use when no info avalable 
	if (AstralStr(IDS_BSR_BLANK, &lpBlank))
		lstrcpy(szBlank, lpBlank);
	else
		lstrcpy(szBlank, _T("-----"));

	// fill name 
	SetDlgItemText(hInfo, IDC_INFO_FILENAME, lpFile->szFile);
	// use thumbnail if there is one
	if (FindEntry(lpFile, &entry, TRUE, ALBMODE))
		lpThm = CTHMOpen(&entry);
	if (!lpThm)
	{
		lpThm =	GetCDThumb(lpFile);
		bCDThm = (lpThm != NULL);
	}
	
	if (lpThm)
	{
		// DESCRIPTION
		if (!lpThm->thm.bDescr)
			CTHMGetDescription(lpThm);
		if (lpThm->thm.info.lDescr)
			lpDescr = lpThm->thm.szDescr;
		else 
			lpDescr = NULL;
		// there is a thumbnail
		// FILE SIZE
		filesize = lpThm->thm.info.fileSize;
		// TIME AND DATE
		time = lpThm->thm.info.time;
		if (!time)
			time = lpThm->thm.info.ctime;

		// IMAGE HEIGHT AND WIDTH
		xsize = lpThm->thm.info.fileXSize;	  
		ysize = lpThm->thm.info.fileYSize;

		fileRes = lpThm->thm.info.fileRes;

		// IMAGE FILE & DATA TYPE
        // if file is to be imported, use extension to determine file type 
        // before imported.
        if( bImported )
        {
            fileType = GetTypeFromExt( lpFile->szFile, 0 );
            fileType -= IDT_FIRSTIMPORTREAL;
        }
        else
    		fileType = lpThm->thm.info.fileType;

    	dataType = lpThm->thm.info.dataType;

		// PREVIEW Frame
		lpFrame = CTHMGetFrame(lpThm);
		if (!lpFrame && !bCDThm)
		{
			lpCDThm = GetCDThumb(lpFile);
			if (lpCDThm)
			{
				CTHMClose(lpThm);
				lpThm = lpCDThm;
				lpFrame = CTHMGetFrame(lpThm);
				bCDThm = TRUE;
			}
		}	
	}
	else 
	{	
		// Get info directly from file
		filesize = time = xsize = ysize = 0L;
		fileRes = 0;
        fileType = -1;   //GetTypeFromExt( lpFile->szFile, 0 );
        dataType = -1;
		lpDescr = NULL;
		if (VolumeCheck(lpFile,NULL))
			if (!SafeFileStats(lpFile->szFile, (LPUINT32)&filesize, NULL ,(LPUINT32)&time,NULL))
				time = filesize = 0L;
		// try to get old 3.1 thumbnail or photo cd thumbnail
		lpFrame = NULL;
	}

	// fill info items
	// FILE TYPE
    if( bImported )
    {
	    fileType += IDI_FIRSTIMPORTFILETYPE;
        if( fileType >= IDI_FIRSTIMPORTFILETYPE && fileType <= IDI_LASTIMPORTFILETYPE )
        {
   	        if (!AstralStr(fileType, &lpBlank))
   		        lstrcpy(szString, szBlank);
	        else
		        lstrcpy(szString, lpBlank);
        }
        else
	        lstrcpy(szString, szBlank);
    }
    else
    {
	    fileType += IDI_FIRSTFILETYPE;
        if( fileType >= IDI_FIRSTFILETYPE && fileType <= IDI_LASTFILETYPE )
        {
   	        if (!AstralStr(fileType,  &lpBlank))
   		        lstrcpy(szString, szBlank);
	        else
		        lstrcpy(szString, lpBlank);
        }
	    else
		    lstrcpy(szString, szBlank);
    }
	SetDlgItemText(hInfo, IDC_INFO_FILETYPE, szString);

	// FILE SIZE
	if (filesize)
		SizeToAscii(filesize, szString);
	else 
		lstrcpy (szString, szBlank);
	SetDlgItemText(hInfo, IDC_INFO_FILESIZE, szString);

	// TIME AND DATE
	if (time)
	{
		tminfo = localtime(&time);
		FormatDate(tminfo, szString);
		SetDlgItemText(hInfo, IDC_INFO_FILEDATE, szString);
		FormatTime(tminfo, szString);
		SetDlgItemText(hInfo, IDC_INFO_FILETIME, szString);
	}
	else
	{
		SetDlgItemText(hInfo, IDC_INFO_FILEDATE, szBlank);
		SetDlgItemText(hInfo, IDC_INFO_FILETIME, szBlank);
	}  

    // DATA TYPE
    if( bImported )
    	SetDlgItemText( hInfo, IDC_INFO_DATATYPE, szBlank );
    else
    {
        dataType += IDC_FIRSTDATATYPE;
        if( dataType >= IDC_FIRSTDATATYPE && fileType <= IDC_LASTDATATYPE )
        {
   	        if (!AstralStr(dataType, &lpBlank))
   		        lstrcpy(szString, szBlank);
	        else
		        lstrcpy(szString, lpBlank);
        }
	    else
		    lstrcpy(szString, szBlank);
	    SetDlgItemText(hInfo, IDC_INFO_DATATYPE, szString);
    }

	// IMAGE HEIGHT AND WIDTH
    if( bImported )
    {
	    SetDlgItemText(hInfo, IDC_INFO_IMAGEWIDTH, szBlank);
	    SetDlgItemText(hInfo, IDC_INFO_IMAGEHEIGHT, szBlank);
    }
    else
    {
	    ret = AstralStr(IDS_BSR_INFOSIZE, &lpString);
	    if (ret)
		    lstrcpy(szFormat, lpString);

	    if (ret &&  AstralStr( Control.Units-IDC_PREF_UNITINCHES+IDS_INCHES,
		        &lpUnit ))
	    {
		    OldUnitRes = Control.UnitRes;
		    Control.UnitRes = fileRes;
		    lstrcpy(szUnit, lpUnit);
		    if (xsize)
		    {
			    Unit2String(FGET(xsize, fileRes),
				    szValue);
			    wsprintf(szString, szFormat, xsize,
				    (LPTSTR)szValue,  (LPTSTR)szUnit);
			    SetDlgItemText(hInfo, IDC_INFO_IMAGEWIDTH, szString);
		    }
		    else
			    SetDlgItemText(hInfo, IDC_INFO_IMAGEWIDTH, szBlank);
		    if (ysize)
		    {	
			    Unit2String(FGET(ysize, fileRes),
				    szValue);
			    wsprintf(szString, szFormat, ysize,
				    (LPTSTR)szValue, (LPTSTR)szUnit);
 			    SetDlgItemText(hInfo, IDC_INFO_IMAGEHEIGHT, szString);
		    }
		    else	
			    SetDlgItemText(hInfo, IDC_INFO_IMAGEHEIGHT, szBlank);
		    Control.UnitRes = OldUnitRes;
	    }
	    else 
	    {
		    if (xsize)
			    SetDlgItemInt(hInfo, IDC_INFO_IMAGEWIDTH, (UINT)xsize, NO);
		    else 
			    SetDlgItemText(hInfo, IDC_INFO_IMAGEWIDTH, szBlank);
		    if (ysize)
			    SetDlgItemInt(hInfo, IDC_INFO_IMAGEHEIGHT, (UINT)ysize, NO);
		    else 
			    SetDlgItemText(hInfo, IDC_INFO_IMAGEHEIGHT, szBlank);
	    }
    }

    // RESOLUTION	
    if( bImported )
    {
	    SetDlgItemText( hInfo, IDC_INFO_RESOLUTION, szBlank );
    }
    else
    {
	    ret = AstralStr( IDS_BSR_INFORES, &lpString );
	    if( ret )
        {
		    lstrcpy( szFormat, lpString );

		    if( fileRes > 1 )
		    {
			    wsprintf( szString, szFormat, fileRes );
			    SetDlgItemText( hInfo, IDC_INFO_RESOLUTION, szString );
            }
		    else
		        SetDlgItemText( hInfo, IDC_INFO_RESOLUTION, szBlank );
	    }
	    else 
	    {
		    if( fileRes > 1 )
			    SetDlgItemInt( hInfo, IDC_INFO_RESOLUTION, ( UINT )fileRes, NO );
		    else 
			    SetDlgItemText( hInfo, IDC_INFO_RESOLUTION, szBlank );
	    }
    }

    // COLOR MANAGEMENT - turned off for Le version
    if( Control.Retail )
    {
        SetDlgItemText( hInfo, IDC_INFO_COLORMANAGED,
			    CmsGetPTNameFromFile( lpFile->szFile, szBlank ));
    }

	// DESCRIPTION
	if (lpDescr)
	{
		SetDlgItemText(hInfo, IDC_INFO_FILEDESCR, lpDescr);
		SetFocus(GetDlgItem(hInfo, IDOK));
	}
	else if (AstralStr(IDS_NO_DESCRIPTION, &lpDescr))
	{
		SetDlgItemText(hInfo, IDC_INFO_FILEDESCR, lpDescr);
		SetFocus(GetDlgItem(hInfo, IDC_INFO_FILEDESCR));
		if (hControl = GetDlgItem(hInfo,IDC_INFO_FILEDESCR))
			Edit_SetSel( hControl, 0, -1);
	}
	else
	{
		SetDlgItemText(hInfo, IDC_INFO_FILEDESCR, _T(""));
		SetFocus(GetDlgItem(hInfo, IDC_INFO_FILEDESCR));
	}
	//Preview Frame	
	if (hControl = GetDlgItem( hInfo, IDC_INFO_THUMBNAIL ))
	{
		SetWindowLong( hControl, GWL_IMAGE,(LONG)lpFrame);
		ClearImageWindow(hControl, 0);
		InvalidateRect( hControl, NULL, YES);
		//AstralControlPaint(hInfo, IDC_INFO_THUMBNAIL);
	}	
	return (lpThm);
}		

//************************************************************************/
//	FOR FILE INFO DIALOG
// 	if bSaveDescr then saves description (before it is lost)
//	returns new bSaveDescrValue
//************************************************************************/
LOCAL BOOL CheckFileSaveDescr(HWND hDlg, BOOL bSaveDescr)
//************************************************************************/
{
	FILELOC File;
	
 	if (bSaveDescr)
		if (GetCurrentFile(&File))
			if (FileInfoSaveDescr(hDlg, &File))
				return(FALSE);
	return(bSaveDescr);
}

//************************************************************************/
// 	Saves the File description for the file info dialog.
//	Creates a minimal thumbnail if needed.
//	returns(FALSE) if there was an error.
//************************************************************************/
LOCAL BOOL FileInfoSaveDescr(HWND hInfo, LPFILELOC lpFile)
//************************************************************************/
{
	int ret;
	ENTRY entry;
	LPCTHMNAIL lpThm, lpCDThm;
	DESC	szDescription;

	// Get the text		
	*szDescription = _T('x');
	if( !GetDlgItemText( hInfo, IDC_INFO_FILEDESCR, szDescription, sizeof( DESC )))
	{
		WORD wChar;
		if( IsDBCSLeadByte(( BYTE )*szDescription ))
			wChar = *(( LPWORD )szDescription );
		else
			wChar = *szDescription;

		if( wChar == _T('x'))
			goto Error;
	}			
	// get the thumbnail
	if (FindEntry(lpFile, &entry, TRUE, ALBMODE))
		lpThm = CTHMOpen(&entry);
	else
	{
		lpThm =	GetCDThumb(lpFile);
	}
	
	ret = FindOrCreateThumb(lpFile, &entry, NO);
	if (ret == BSR_ERROR)
		goto Error;
	lpThm = CTHMOpen(&entry);
	if (!lpThm)
		goto Error;
	if (ret == BSR_TRUE)
	{
		// get basic info
		CTHMReCreateEasyInfo(lpThm, lpFile, YES);
		// can we get info from photocd or 3.1?
		lpCDThm = GetCDThumb(lpFile);
		if (lpCDThm)
		{
            if( bImported )
            {
			    // use its info		
			    CTHMPutFrame(lpThm,
						    lpCDThm->thm.frame,
						    (int)lpCDThm->thm.info.fileXSize,
						    (int)lpCDThm->thm.info.fileYSize, 
						    lpCDThm->thm.info.dataType+IDC_FIRSTDATATYPE, 
						    lpCDThm->thm.info.fileType+IDN_FIRSTIMPORTFILETYPE, 
						    lpThm->thm.info.time);
            }
            else
            {
			    // use its info		
			    CTHMPutFrame(lpThm,
						    lpCDThm->thm.frame,
						    (int)lpCDThm->thm.info.fileXSize,
						    (int)lpCDThm->thm.info.fileYSize, 
						    lpCDThm->thm.info.dataType+IDC_FIRSTDATATYPE, 
						    lpCDThm->thm.info.fileType+IDN_FIRSTFILETYPE, 
						    lpThm->thm.info.time);
            }
			CTHMClose(lpCDThm);
		}
	}
	ret = CTHMPutDescription(lpThm, &szDescription);
	CTHMClose(lpThm);
	if (!ret)
		goto Error;
	return (TRUE);

Error:
	Message (IDS_BSR_ERROR);
	return(FALSE);
}

//************************************************************************/
//	FOR ALBUM INFO DIALOG
// 	if bSaveDescr then prompts the user to save the description 
//		(befor it is lost)
//	Updates save descr button, and returns the new bSaveDescrValue
//************************************************************************/
LOCAL BOOL CheckAlbSaveDescr(HWND hDlg, BOOL bSaveDescr)
//************************************************************************/
{
	LPDBFLL lpDBFLL;
	
 	if (bSaveDescr)
		if (lpDBFLL = GetAlb())
			if (AlbInfoSaveDescr(hDlg, &lpDBFLL->dbf))
				return(FALSE);
	return(bSaveDescr);
}

//************************************************************************/
// 	Saves the album description for the album info dialog.
//	returns(FALSE) if there was an error.
//************************************************************************/
LOCAL BOOL AlbInfoSaveDescr(HWND hInfo, LPDBF lpDBF)
//************************************************************************/
{
	FNAME szDescription;

	// Get the text		
	*szDescription = _T('x');
	if (!GetDlgItemText(hInfo, IDC_ALBINFO_DESCR, szDescription, sizeof(szDescription)))
	{
		WORD wChar;
		if( IsDBCSLeadByte(( BYTE )*szDescription ))
			wChar = *(( LPWORD )szDescription );
		else
			wChar = *szDescription;

		if( wChar == _T('x'))
			goto Error;
	}
			
	// get the thumbnail
	if (!DBFReOpen(lpDBF, YES))
		goto Error;
	lstrcpy(lpDBF->info.szDescription, szDescription);
	if (!DBFPutInfo(lpDBF))
		goto Error;
	return (TRUE);

Error:
	Message (IDS_BSR_ERROR);
	return(FALSE);
}

//***********************************************************************/
// 	Fills the Album Info for the Album Info dialog
//***********************************************************************/
LOCAL void FillAlbInfo(HWND hInfo, LPDBF lpDBF)
//***********************************************************************/
{
	if (!lpDBF)
		return;
	SetDlgItemText(hInfo, IDC_ALBINFO_NAME, lpDBF->info.szName);
	SetDlgItemInt(hInfo, IDC_ALBINFO_NUMENTRIES, lpDBF->info.numEntry, NO);
	SetDlgItemText(hInfo, IDC_ALBINFO_DESCR, lpDBF->info.szDescription);
}

/***********************************************************************/
LOCAL void ClearImageWindow(HWND hWindow, int border)
/***********************************************************************/
{
	RECT ClientRect;
	HDC hDC;
	HBRUSH hBrush;
	
	hDC = NULL;
	hBrush = NULL;
	
	if (!(hBrush = (HBRUSH)GetStockObject(GRAY_BRUSH)))
		return;
	if (!(hDC = GetDC(hWindow)))
		return;
	GetClientRect( hWindow, &ClientRect );
	InflateRect( &ClientRect, -border, -border);
	FillRect(hDC, &ClientRect, hBrush);
	ReleaseDC(hWindow, hDC);
}

