// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// handles items in Image Browser dlg

#include "pp.h"
#include <time.h> 
#ifndef WIN32
#include <malloc.h>
#endif
#include "id.h"
#include "bsrtypes.h"
#include "bsrutil.h"
#include "handlebs.h"
#include "browser.h"
#include "bsrid.h"
#include "bsrdlgs.h"
#include "bsritems.h"
#include "dbfmgr.h"
#include "cthmmgr.h"
#include "thmmgr.h"
#include "search.h"
#include "ppafx.h"
#include "pcdui.h"

// temporary .. delete later
void testdbfmgr(void);
void testthmmgr(void);


#define MIN_STACK_SIZE 1024

//**************************Externs******************************************/
extern HHOOK prevKeyHook;
extern LPBSRSTATE lpBsr;
extern BOOL bImported;
extern HANDLE hAccelTable;

// for printing thumbs
extern HDC hPrinterDC;
extern BOOL fAbortPrint;	/* abort sending job to spooler? */
extern BOOL BandingDevice;
extern short   BandInfoDevice;
extern BOOL bNextBandReached;
extern ABORTPROC lpAbortTest;
extern RECT rBand;
extern RECT rPrintBand;
extern int iEscErr;
extern DOCINFO PrinterDocInfo;


//********************Static prototypes****************************************/
LOCAL void HandleSaveOK(HWND hDlg);
LOCAL void HandleLoadOK(HWND hDlg);
LOCAL BOOL PromptSaveIntoAlbum(HWND hDlg, LPTSTR szAlbum, LPFILELOC lpFile, LPINT lpPromptFor);
LOCAL BOOL HandleFileNameOK(HWND hDlg, LPFILELOC lpOutFile, LPINT lpbPrompt);
LOCAL BOOL changeCWD(LPTSTR lpPath);
LOCAL BOOL ChangeAlbum(HWND hDlg, LPTSTR lpAlbum, BOOL bUpdateFileSpec);
LOCAL BOOL VerifySaveFile(BOOL bPromptOnFileExists);
LOCAL BOOL VerifyLoadFiles(void);
LOCAL BOOL	TextBoxMakeFullPath(LPTSTR lpFullName, LPTSTR lpFile);
LOCAL void	RenameDirectoryThumbs(LPTSTR szDir, LPTSTR szNewDir);
LOCAL LPTSTR GetSelDir(HWND hDlg, LPTSTR lpDir);
LOCAL BOOL delete_dir(HWND hDlg, LPTSTR lpDir);


//**************************Functions**************************************/


//************************************************************************/
//	Handles File->CreateDirectory menu function
//************************************************************************/
BOOL CreateDir(HWND hDlg)
//************************************************************************/
{
	FNAME szDir;
	
	if (!CURRENTDIR(szDir,sizeof(szDir)))
		szDir[0] = _T('\0');
	// get new name
	FixPath(szDir);
	RemovePathBackslash(szDir);
//	Lowercase(szDir);
	while (TRUE)
	{
		if (GetDialogString(hDlg, szDir, NULL, szDir, IDD_GETNEWDIR) != IDOK)
			return(0);
		FixPath(szDir);
		RemovePathBackslash(szDir);
//		Lowercase(szDir);
		if (IsValidFileName(filename(szDir))) 
			break;
		Message(IDS_INVALIDNAME, (LPTSTR)filename(szDir));
	}
		
	if (Browser.ConfirmOnCreateDir)
		if (AstralOKCancel(IDS_CONFIRMNEWDIR, (LPTSTR)szDir) != IDOK)
				return(FALSE);
	if (!MkDir(szDir))
		Message(IDS_ENEWDIR, (LPTSTR)szDir);
	FillDirectoryList(hDlg);
	return(FALSE);
}

//************************************************************************/
//	Handles File->RenameDirectory menu function
//************************************************************************/
BOOL RenameDir(HWND hDlg)
//************************************************************************/
{
	FNAME szDir, szNewDir, szCurDir, szFullDir;
	int len;
	BOOL bRet;
	DWORD error;
	
	if (!GetSelDir(hDlg, szDir))
		szDir[0] = _T('\0');
	if (!CURRENTDIR(szCurDir,sizeof(szCurDir)))
		szCurDir[0] = _T('\0');
	// get new name
	FixPath(szDir);
	RemovePathBackslash(szDir);
//	Lowercase(szDir);
	lstrcpy(szNewDir, filename(szDir));
	while (TRUE)
	{
		if (GetDialogString(hDlg, szNewDir, szDir, szNewDir, IDD_GETRENAMEDIR) != IDOK)
			return(0);
		if (StringsEqual(szNewDir, filename(szDir)))
			return(0);
		if (IsValidFileName(szNewDir))
			break;
		Message(IDS_INVALIDNAME, (LPTSTR)szNewDir);
	}
	BSRInvalidateDriveIDs();
	FixFileName(szNewDir);
//	Lowercase(szNewDir);
	lstrcpy(szFullDir, szDir);
	stripfile(szFullDir);
	lstrcat(szFullDir, szNewDir);
	if (Browser.ConfirmOnRenameDir)
		if (AstralOKCancel(IDS_CONFIRMRENAMEDIR, (LPTSTR)szDir, (LPTSTR)szFullDir) != IDOK)
			return(0);
	if (szCurDir[0])
		CHDIR(_T("\\"));
	bRet = RenameFile(szDir, filename(szFullDir));
	if (!bRet)
	{
		error = GetLastError();
		Message(IDS_ERENAMEDIR, (LPTSTR)szDir, (LPTSTR)szFullDir);
	}
	else
		RenameDirectoryThumbs(szDir, szFullDir);
	if (szCurDir[0])
	{
		len = lstrlen(szDir);
		if (StringsEqualN(szCurDir, szDir, len))
		{
			// current path needs to be changed
			lstrcpy(szDir, szFullDir);
			lstrcat(szDir, (LPTSTR)szCurDir + len);
			lstrcpy(szCurDir, szDir);
		}
		if (CHDIR(szCurDir))
			FillItems(hDlg, NO, NO);
	}
	FillDirectoryList(hDlg);
	return(TRUE);
}

//************************************************************************/
//	Handles File->DeleteDirectory menu function
//************************************************************************/
BOOL DeleteDir(HWND hDlg)
//************************************************************************/
{
	FNAME szDir, szCurDir;
	int len;
	
	if (!GetSelDir(hDlg, szDir))
		return(0);
	if (!CURRENTDIR(szCurDir,sizeof(szCurDir)))
		szCurDir[0] = _T('\0');
	FixPath(szDir);
	RemovePathBackslash(szDir);
//	Lowercase(szDir);
	if (Browser.ConfirmOnDeleteDir)
		if (AstralOKCancel(IDS_CONFIRMDELETEDIR, (LPTSTR)szDir) != IDOK)
			return(FALSE);
	if (szCurDir[0])
		CHDIR(_T("\\"));
	delete_dir(hDlg, szDir);
	if (szCurDir[0])
	{
		len = lstrlen(szDir);
		if (StringsEqualN(szCurDir, szDir, len))
			lstrcpy(szCurDir, stripfile(szDir));	
		RemovePathBackslash(szCurDir);
		CHDIR(szCurDir);
	}
	FillItems(hDlg, YES, NO);
//	FillDirectoryList(hDlg);
	return(TRUE);
}

//************************************************************************/
LOCAL BOOL delete_dir(HWND hDlg, LPTSTR lpDir)
//************************************************************************/
{
	FNAME szFile;
	FILELOC File;
	ENTRY entry;
	FINDDATA afile;
	int ret, num;
	BOOL bYesToAll;

#ifndef WIN32
	if ( stackavail() < MIN_STACK_SIZE )
	{
		MessageBeep(-1);		// ***ERROR -- real error
		return(TRUE);
	}
#endif
	
	lstrcpy(szFile, lpDir);
	lstrcat(szFile, _T("\\*.*"));
	// check for files
	bYesToAll = FALSE;
	if (Dos_FindFirst(szFile, ATTRIB_NORMAL | ATTRIB_HIDDEN, &afile))
	{
		do
		{
			stripfile(szFile);
			lstrcat(szFile, afile.szName);
			if (!bYesToAll)
			{
				// ask user
				ret = (int)AstralYesToAllString(hDlg, IDD_CONF_FILEDEL, szFile, YES);
	 			if (ret == IDC_BSR_NO)
					continue;
				else if (ret == IDC_YESTOALL)
					bYesToAll = TRUE;
				else if (ret != IDOK)
					return(FALSE);
			}
			// delete file
			if (!FileDelete(szFile))
			{
				if (AstralOKCancel(IDS_EDELETE, (LPTR)szFile) != IDOK)
					return(FALSE);
			}
			else
			{
				// Delete Thumbnail
				lstrcpy(File.szFile, szFile);
				CLEARVOLUME(File.volume);
				if (FindEntry(&File, &entry, TRUE, NO))
					if(!CompleteDeleteEntry(&entry))
						Message(IDS_EDELETETHUMBNAIL);
			}
		}
		while (Dos_FindNext(&afile));
	}
	lstrcpy(szFile, lpDir);
	lstrcat(szFile, _T("\\*.*"));	 
	// check for subdirs
	bYesToAll = FALSE;
	// get count
	num = 1;
	if (Dos_FindFirst(szFile, ATTRIB_SUBDIR, &afile))
		while (Dos_FindNext(&afile))
			num++;
	if (Dos_FindFirst(szFile, ATTRIB_SUBDIR, &afile))
	{
		do
		{
			num--;
			if (afile.szName[0] == _T('.'))
				continue;
			lstrcpy(szFile, lpDir);
			lstrcat(szFile, _T("\\"));
			lstrcat(szFile, afile.szName);
			if (!bYesToAll)
			{
				// ask user
				ret = (int)AstralYesToAllString(hDlg, IDD_CONF_DIRDEL, szFile, num);
				if (ret == IDC_BSR_NO)
					continue;
				else if (ret == IDC_YESTOALL)
					bYesToAll = TRUE;
				else if (ret != IDOK)
					return(FALSE);
			}
			// delete the subdir
			if (!delete_dir(hDlg, szFile))
				return(FALSE);
		}
		while (Dos_FindNext(&afile));
	}
	lstrcpy(szFile, lpDir);
	if (!RmDir(szFile))
		Message(IDS_EDELETEDIR, (LPTSTR)lpDir);
	return(TRUE);
}

//************************************************************************/
//	Handle Files->Move To Album menu item
//************************************************************************/
BOOL MoveFilesToAlb(HWND hDlg)
//************************************************************************/
{
	FILELOC File;
	FNAME 	szAlbum;
	LPDBFLL lpDBFLL;
	BOOL result;
	CHOOSEALBUM chooseAlbum;

	BSRInvalidateDriveIDs();
	lpChooseAlbum = &chooseAlbum;	
	if (DIRMODE)
	{
		chooseAlbum.lpChooseTitle = NULL;
		chooseAlbum.lpChooseText = NULL;
		chooseAlbum.lpSkipAlbum = NULL;
		chooseAlbum.lpSelectedAlbum = lpBsr->OpenState.szAlbum;
//		EnableWindow(hDlg, FALSE);
		result = (BOOL)AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_CHOOSEALBUM, DlgChooseAlbumProc);
//		EnableWindow(hDlg, TRUE);
		SetFocus(hDlg);
		if (!result)	
			return(0);
		lstrcpy(szAlbum, chooseAlbum.szAlbum);
		// it is now the selected album!
		lstrcpy(lpBsr->OpenState.szAlbum, chooseAlbum.szAlbum);
	}
	else	 
	{
		// Album mode
		chooseAlbum.lpChooseTitle = NULL;
		chooseAlbum.lpChooseText = NULL;
		chooseAlbum.lpSkipAlbum = lpBsr->OpenState.szAlbum;
		chooseAlbum.lpSelectedAlbum = NULL;
//		EnableWindow(hDlg, FALSE);
		result = (BOOL)AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_CHOOSEALBUM, DlgChooseAlbumProc);
//		EnableWindow(hDlg, TRUE);
		SetFocus(hDlg);
		if (!result)	
			return(0);
		lstrcpy(szAlbum, chooseAlbum.szAlbum);
	}
	// try to find album
	if (szAlbum[0] == _T('\0'))
		return(0);
	lpDBFLL = FindAlbum(szAlbum);
	if (!lpDBFLL)	
		return(0);	// ***ERROR ??
	ProgressBegin(1,0,hDlg);
	// scan through selected files and add to album
 	OpenFileList(hDlg);
	while(GetNextFile(&File))
		AddToAlbum(&(lpDBFLL->dbf), &File, YES);  // ***ERROR if fails?
	ProgressEnd();
	return(0);
}

//************************************************************************/
//	Handles Files->Delete menu item.
//	Deletes files from disk if in Directory mode.
//************************************************************************/
BOOL DeleteFiles(HWND hDlg)
//************************************************************************/
{

	int num;
	
	num = OpenFileList(hDlg);
	if (num <= 1)
		AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_DELETE_FILE, DlgDeleteFileProc);
	else	// more than one file
		AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_DELETE_FILES, DlgDeleteFilesProc);
	SetUpSearchSpec(hDlg, NULL);
	FillFileList(hDlg, YES);
	EnableOpenSaveButton(hDlg);
	EnableInfoButton(hDlg);
	SetDriveSizeText(hDlg);
	return(FALSE);
}
	
//************************************************************************/
//	Handles Album->Remove Files menu item.
//	Removes files from album in album mode.
//************************************************************************/
BOOL RemoveFilesFromAlb(HWND hDlg)
//************************************************************************/
{
	FNAME szAlbum;
	FILELOC File;
	LPDBFLL	lpDBFLL;
	LPDBF	lpDBF;

	if (!ALBMODE)
		return(FALSE);
	// Only virtual delete working yet	
	lstrcpy(szAlbum, lpBsr->OpenState.szAlbum);
	if (szAlbum[0] == _T('\0'))
		return(FALSE);
	if (!OpenFileList(hDlg))
		return(FALSE);
	// open the album
	if (!(lpDBFLL = FindAlbum(szAlbum)))	
		return(FALSE);					//***ERROR?
	if (!DeleteVirtualDialog(hDlg, szAlbum))
		return(FALSE);
	lpDBF = &(lpDBFLL->dbf);
	if (!DBFReOpen(lpDBF, YES))
		return(FALSE);
	
	// delete every selected item
	OpenFileList(hDlg);
	while (GetNextFile(&File))
		DBFDeleteEntry(lpDBF, &File);	//***ERROR check for errors?
	DBFClose(lpDBF);
	SetUpSearchSpec(hDlg, NULL);
	FillFileList(hDlg, NO);
	SetDriveSizeText(hDlg);
	return(FALSE);
}

//************************************************************************/
//	Handles Files->Copy menu item.
//	Copy files physically if in Directory mode.
//	Copy files physically if in album mode.
//************************************************************************/
BOOL CopyFiles(HWND hDlg)
//************************************************************************/
{
	
	int num;
	
	num = OpenFileList(hDlg);
	if (num <= 1)
		AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_COPY_FILE, DlgCopyFileProc);
	else	
		AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_COPY_FILES, DlgCopyFilesProc);
	SetUpSearchSpec(hDlg, NULL);
	FillFileList(hDlg, YES);
	SetDriveSizeText(hDlg);
	return(FALSE);
}

//************************************************************************/
//	Handles Files->Move menu item.
//	Copy files physically if in Directory mode.
//	Copy files physically if in album mode.
//************************************************************************/
BOOL MoveFiles(HWND hDlg)
//************************************************************************/
{
	
	int num;
	
	num = OpenFileList(hDlg);
	if (num <= 1)
		AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MOVE_FILE, DlgMoveFileProc);
	else	// more than one file
		AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_MOVE_FILES, DlgMoveFilesProc);
	SetUpSearchSpec(hDlg, NULL);
	FillFileList(hDlg, YES);
	return(FALSE);
}

//************************************************************************/
//	Handles Files->Rename menu item.
//	Renames files physically.
//************************************************************************/
BOOL RenameFiles(HWND hDlg)
//************************************************************************/
{
	FNAME szDest, szFullDest;
	FILELOC File;
	ENTRY entry;
	int num;
	DWORD dwError;
	
	BSRInvalidateDriveIDs();
	num = OpenFileList(hDlg);
	if (num != 1)
		return(0);
	//	get file name
	if (!GetFirstFile(&File))
		return(0);
//	Lowercase(File.szFile);
	if (!VolumeCheck(&File, NULL))
	{
		//***ERROR - should Prompt user for disk
		return(0);
	}
	// get new name
	
	lstrcpy(szDest, filename(File.szFile));
	while (TRUE)
	{
		if (!RenameFileDialog(hDlg, File.szFile, szDest))
			return(FALSE);
		if (IsValidFileName(filename(szDest)))
			break;
		Message(IDS_INVALIDNAME, filename(szDest));
	}
		
	FixFileName(szDest);
//	Lowercase(szDest);
	lstrcpy(szFullDest, File.szFile);
	stripfile(szFullDest);
//	Lowercase(szFullDest);
	lstrcat(szFullDest, filename(szDest));
	if (StringsEqual(filename(szDest), filename(File.szFile)))
	{
		Message(IDS_ERENAME_SELF, (LPTSTR)szFullDest);
		return(FALSE);
	}
	if (FileExists(szFullDest))
	{
		if (AstralOKCancel(IDS_OVERWRITE, (LPTSTR)szFullDest) != IDOK)
			return(FALSE);
		if (!FileDelete(szFullDest))
		{
			Message(IDS_ECANTDELETE, (LPTSTR)szFullDest);
			return(FALSE);
		}
	}
	else if (Browser.ConfirmOnRenameFile)
			if (AstralOKCancel(IDS_CONFIRMRENAME, (LPTSTR)File.szFile, (LPTSTR)szFullDest) != IDOK)
				return(FALSE);
	// rename the file 
	lstrcpy(szDest, filename(szDest));
	if (!RenameFile(File.szFile, szDest))
		goto RenameErr;
	// rename the thumbnail entries
	if (FindEntry(&File, &entry, FALSE, YES))
	{
		lstrcpy(entry.File.szFile, szFullDest);
		entry.File.volume = File.volume;
		if (!ChangeEntry(&File, &entry))
			Message(IDS_ERENAMETHUMBNAIL);
	}
	SetUpSearchSpec(hDlg, NULL);
	FillFileList(hDlg, YES);
	return(FALSE);
RenameErr:
	dwError = GetLastError();
	if (dwError == ERROR_BAD_PATHNAME||
			dwError == ERROR_FILE_NOT_FOUND)
		Message(IDS_EACCES, (LPTSTR)szFullDest);
	else 
		Message(IDS_ERENAME, File.szFile, szFullDest);
	SetUpSearchSpec(hDlg, NULL);
	FillFileList(hDlg, YES);
	return(FALSE);
}

//************************************************************************/
//	Handles Files->Find menu item.
// 	Puts user in directory mode at the location of the first file selected
//************************************************************************/
BOOL GoToFile(HWND hDlg)
//************************************************************************/
{
	FILELOC File;
	FNAME szPath;
	FNAME szNewPath;
	BOOL bRet;
	
	if (!ALBMODE)
		return(0);
	if (!OpenFileList(hDlg))
		return(0);
	if (!GetFirstFile(&File))
		return(0);
	if (!VolumeCheck(&File, NULL))
	{
		//***ERROR - should warn user or prompt for disk
		return(FALSE);
	}
	lstrcpy(szPath, File.szFile);
	stripfile(szPath);
	FixPath(szPath);
	// If not a root then strip off last 
	RemovePathBackslash(szPath);
	lpBsr->OpenState.ViewAlbums = !lpBsr->OpenState.ViewAlbums;
	// be sure the path was found
	bRet = ChangeDir(hDlg, szPath, YES);
	CURRENTDIR(szNewPath, sizeof(FNAME));
	if (!StringsEqual(szPath, szNewPath))
	{
		lpBsr->OpenState.ViewAlbums = !lpBsr->OpenState.ViewAlbums;
		return(0);
	}
	if (!bRet)
	{
		// if bRet then it was already done in ChangeDir
		FillDirectoryList(hDlg);
		FillFileList(hDlg, NO);
	}
	ShowAlbumWindow(hDlg, lpBsr->OpenState.ViewAlbums);
	return(0);
}

//************************************************************************/
//	Handles Album->New menu function
//************************************************************************/
BOOL NewAlbum(HWND hDlg, BOOL bFill)
//************************************************************************/
{
	STRING szAlbName;
	LPDBFLL node;
	BOOL bDup;

	if (GetDialogString(hDlg, NULL, NULL, szAlbName, IDD_NEW_ALB) != IDOK)
		return(0);
	// is it valid 
	if (!CheckAlbumName(szAlbName))
	{
		Message(IDS_BADALBNAME);
		return(0);
	}	
	// does it already exist? 
	node = (LPDBFLL)Alloc(sizeof(DBFLL));
	if(!node)
		return(0);	// ***ERROR
	if (!DBFNewAlbum(szAlbName, &(node->dbf), &bDup))
	{
		// Could not create new album
		FreeUp((LPTSTR)node);
		return(0); 	// ***ERROR
	}	
	DBFClose(&(node->dbf));
	if (bDup)
	{
		// Album already exitsts
		FreeUp((LPTSTR)node);
		Message(IDS_ALB_EXISTS, (LPTSTR)szAlbName);
		return(0); 	
	}	
	// its a new album
	// Add to list
	AddToAlbList(node);
	// make active alb
	lstrcpy(lpBsr->OpenState.szAlbum, szAlbName);
	// update list/combo box
	if (bFill)
		FillItems(lpBsr->OpenState.hBrowser, ALBMODE ? YES:NO, NO);
	return(TRUE);	
}

//************************************************************************/
//	Album->Rename menu item.
//	Renames lpAlbum, if lpAlbum == NULL then renames the currently selected album. 
//  Recomfirms the rename, if Browser.ConfirmOnRenameAlbum == TRUE.
//************************************************************************/
BOOL RenameAlbum(HWND hDlg, LPTSTR lpAlbum, BOOL bFill)
//************************************************************************/
{
	FNAME szAlbum, szNewName;
	BOOL done;
	LPDBFLL	lpDBFLL;
	LPDBFLL	lpNewDBFLL;
	LPDBF	lpDBF;
	
	if (lpAlbum)
		lpDBFLL = FindAlbum(lpAlbum);
	else
		lpDBFLL = GetSelectedAlbum(lpBsr->OpenState.hBrowser);
	if (!lpDBFLL)
		return(FALSE);
	lstrcpy(szAlbum, lpDBFLL->dbf.info.szName);
	do 
	{
		if (GetDialogString(hDlg, szAlbum, szAlbum, szNewName, IDD_RENAMEALB) != IDOK)
			return(0);
		done =  CheckAlbumName(szNewName);
		if (!done)
			Message(IDS_BADALBNAME);
		else
		{	// check for duplicate
			lpNewDBFLL = FindAlbum(szNewName);
			if (lpNewDBFLL)
				if (lpNewDBFLL != lpDBFLL)
				{	// its not a case variation on the old name
					done = FALSE;
					Message(IDS_ALB_EXISTS, (LPTSTR)szNewName);
				}
		}
		
	} while (!done);
	
	if (Browser.ConfirmOnRenameAlbum)
		if (AstralOKCancel(IDS_RENAMEALB_CONF, (LPTSTR)szAlbum, (LPTSTR)szNewName) != IDOK)
			return(0);
	
	// get the album
	lpDBF = &(lpDBFLL->dbf);
	// do actual rename
	DBFReOpen(lpDBF, YES);
	if (!DBFUpdateInfo(lpDBF))
		return(0);		//***ERROR
	lstrcpy(lpDBF->info.szName, szNewName);
	if (!DBFPutInfo(lpDBF))
		return(0);		//***ERROR
	DBFClose(lpDBF);
	
	if(StringsEqual(lpBsr->OpenState.szAlbum, szAlbum))
		lstrcpy(lpBsr->OpenState.szAlbum, szNewName);
	if (lpAlbum)
		lstrcpy(lpAlbum, szNewName);
	// update display
	if (bFill)
	{
		FillAlbList(lpBsr->OpenState.hBrowser);
		SetPathText(lpBsr->OpenState.hBrowser);	
	}
	return(TRUE);			
}

//************************************************************************/
//	Album->Delete menu item.
//	Deletes lpAlbum, if lpAlbum == NULL then deleteds the currently selected album. 
//	Prompts the user if Browser.ConfirmOnDeleteAlbum == TRUE.
//************************************************************************/
BOOL DeleteAlbum(HWND hDlg, LPTSTR lpAlbum, BOOL bFill)
//************************************************************************/
{
	LPDBFLL	lpDBFLL;
	
	// get the album
	if (lpAlbum)
		lpDBFLL = FindAlbum(lpAlbum);
	else
		lpDBFLL = GetSelectedAlbum(hDlg);
	if (!lpDBFLL)
		return(FALSE);
	if (!lpDBFLL)
		return(0);	
	// confirm
	if (Browser.ConfirmOnDeleteAlbum)
		if (AstralOKCancel(IDS_DELETEALB, (LPTSTR)lpDBFLL->dbf.info.szName) != IDOK)
			return(0);
	// delete the file
	if (!FileDelete(lpDBFLL->dbf.szFile))
		return(0);	// ***ERROR??
	// remove from album list
	RemoveAlbumFromList();
	if (bFill)
		FillItems(hDlg, ALBMODE ? YES : NO, NO);
	return(TRUE);
}

//************************************************************************/
//	Edit->SelectAll or Edit->SelectNone Menu Item
//************************************************************************/
BOOL SelectFiles(HWND hDlg, BOOL bAll)
//************************************************************************/
{
	ChangeFileSel(hDlg, LB_SETSEL, bAll, -1L);
	SetFileListInfo(hDlg);
	return(0);
}

//************************************************************************/
//	Edit->Invert Selection Menu Item
//************************************************************************/
BOOL InvertSelectFiles(HWND hDlg)
//************************************************************************/
{
	int num;
	HWND hBox;
	BOOL bSelect;

	hBox = FILELIST(hDlg);
	num = ListBox_GetCount(hBox);
	if (num == LB_ERR)
		return(0);
	for (num--;num>=0;num--)
	{
		bSelect = !ListBox_GetSel(hBox, num);
		ChangeFileSel(hDlg, LB_SETSEL, bSelect, num);
	}
	SetFileListInfo(hDlg);
	return(0);
}

//************************************************************************/
//	File->Search Menu Item
//************************************************************************/
void Search(HWND hDlg)
//************************************************************************/
{
	LPDBFLL node, lpDBFLL;
	int result;

// setup data structure
	lstrcpy(FileSearch.szSelectedAlb, lpBsr->OpenState.szAlbum);
	CURRENTDIR(FileSearch.szSearchDir, sizeof(FileSearch.szSearchDir));
	FileSearch.hBrowser = hDlg;
	
	// search
	result = (int)AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_BSR_SEARCH, DlgSearchProc );
	SetFocus(hDlg);
	if (result == IDCANCEL || !FileSearch.dbfResult.szFile[0])
		return;
	
	// Hook in new album
	node = (LPDBFLL)Alloc(sizeof(DBFLL));
	if(!node)
		return;	// ***ERROR	no results will be shown
	node->dbf = FileSearch.dbfResult;
	// Remove any old search results
	lpDBFLL = FindAlbum(Browser.SearchAlbum);
	if (lpDBFLL)
	{	
		// FOUND OLD SEARCH RESULTS
		if (!FileDelete(lpDBFLL->dbf.szFile))
			Message(IDS_SEARCH_ERROR); 
		// remove from album list
		RemoveAlbumFromList();
	}
	// rename new search results
	DBFReOpen(&node->dbf, YES);
	if (!DBFUpdateInfo(&node->dbf))
		Message(IDS_SEARCH_ERROR); 
	lstrcpy(node->dbf.info.szName, Browser.SearchAlbum);
	if (!DBFPutInfo(&node->dbf))
		Message(IDS_SEARCH_ERROR); 
	DBFClose(&node->dbf);
	// Add new search results to list
	AddToAlbList(node);
	// make active alb
	lstrcpy(lpBsr->OpenState.szAlbum, node->dbf.info.szName);
	// update list/combo box
	lpBsr->OpenState.ViewAlbums = TRUE;
	SetDirectoryFromDef(hDlg);
    if( bImported )
    	lpBsr->OpenState.idFileType = IDT_ALLIMPORTFILES;
    else
    	lpBsr->OpenState.idFileType = IDT_ALLFILES;
	FillItems(hDlg, YES, NO);
	ShowAlbumWindow(hDlg, lpBsr->OpenState.ViewAlbums);
	return;	
}								    

//************************************************************************/
//	Thumbnails->Cleanup Thumbs
//************************************************************************/
BOOL CleanupThumbs(HWND hDlg)
//************************************************************************/
{
	if (AstralDlg(NO, PictPubApp.GetResourceHandle(), hDlg, IDD_CLEANUP_THUMBS, DlgCleanupThumbsProc))
	{
		SetUpSearchSpec(hDlg, NULL);
		FillFileList(hDlg, YES);
		SetDriveSizeText(hDlg);
	}
	return(FALSE);
}

//************************************************************************/
// Call to setup memory etc.
// Extended Browser aware.
// Returns an LPBSRSTATE, with lpOpenState copied into it.
//************************************************************************/
BOOL BrowserDlgBegin(HWND hDlg)
//************************************************************************/
{
	lpBsr->OpenState.hBrowser = hDlg;
	// setup thumb cache
	CTHMInitCache();
	
	
	// Setup INTERNATIONAL DATE TIME
	SetupInternational();

	// Setup thumbnail search dbf
	CLEAR_DIR_DBF_CACHE;	
	
	// setup search info
	FileSearch.idSearchBy = IDC_SEARCH_BYNAME;
	FileSearch.idSearchIn = IDC_SEARCH_ALLALB;
	FileSearch.szSearchString[0] = _T('\0');

	if (!SetupDriveInfoMem(TRUE))
		return(FALSE);
	BSRInvalidateDriveIDs();
	
	// setup SelBuf
	PrepareFileList();
	
	// do stuff for regular browser only
	if (!EXTENDED)
	{
		// setup title bar & ok button
		if (hDlg)
		{
			SetDlgItemText(hDlg,IDC_OK, lpBsr->OpenState.szOK);
			SetWindowText(hDlg, lpBsr->OpenState.szTitle);
		}
		// setup album list
		PrepareAlbList();
		CreateAlbList();
	}
	lpBsr->OpenState.bInitialized = TRUE;
	return(TRUE);
}

//************************************************************************/
// Extended Browser aware.
//************************************************************************/
void BrowserDlgEnd(HWND hDlg)
//************************************************************************/
{
	
	lpBsr->OpenState.bInitialized = FALSE;
	// do stuff for regular browser only
	if (!EXTENDED)
	{
		// free up AlbList
		FreeAlbList();

	}
	// free up SelBuf 
	FreeFileList();
	testthmmgr();
	testdbfmgr();
	// free up thumb cache memory
	CTHMEndCache();
	// Free up drive volume/type info
	FreeUpDriveInfoMem();
}

//************************************************************************/
void SetupItems(HWND hDlg)
//************************************************************************/
{
	lpBsr->OpenState.bNavigated = FALSE;
	// Go to specified or saved directory
	SetDirectoryFromDef(hDlg);
	FillDrivesCombo(hDlg);
	return;
}

//************************************************************************/
//	Does not fill file info boxes.  
// 	These are only filled when needed.
// 	if (!bMain) then the Album or Directory list is NOT filled (depeding on mode).
//************************************************************************/
void FillItems(HWND hDlg, BOOL bMain, BOOL bMaintainFileSelect)		 
//************************************************************************/
{
	if (DIRMODE)
	{
		if (bMain)
			if (!FillDirectoryList(hDlg))
				// error with drive
				ClearItems(hDlg);
		FillAlbList(hDlg);
	}	
	else if (bMain)
		FillAlbList(hDlg);
	if (DIRMODE)
		SetDriveSizeText(hDlg);
	FillFileType(hDlg);
	SetUpSearchSpec(hDlg, NULL); // will set file name text
	FillFileList(hDlg, bMaintainFileSelect);
	FillDataType(hDlg);

    if( Control.Retail )
    	FillOpenMode(hDlg);

	EnableDataTypes(hDlg);
	EnableCompress(hDlg);
	EnableOpenSaveButton(hDlg);
	EnableInfoButton(hDlg);
}

//************************************************************************/
// 	Sets all items without accessing disk
//************************************************************************/
void ClearItems(HWND hDlg)
//************************************************************************/
{
	HWND hWnd;
	hWnd = DIRLIST(hDlg);
	ListBox_ResetContent(hWnd);
	SetDlgItemText(hDlg, IDC_BSR_PATH_TEXT, (LPTSTR)_T(""));	
	hWnd = GetDlgItem(DIRWINDOW(hDlg), IDC_BSR_ALBUMCOMBO);
	ListBox_ResetContent(hWnd);
	SetDlgItemText(hDlg, IDC_BSR_PATH_TEXT, (LPTSTR)_T(""));	
	SetDlgItemText(DIRWINDOW(hDlg), IDC_BSR_DRIVESIZE, (LPTSTR)_T(""));
	lpBsr->bFileNameChange = TRUE;
	SetDlgItemText(FILEWINDOW(hDlg),IDC_BSR_FILENAME, (LPTSTR)_T(""));
	lpBsr->bFileNameChange = FALSE;
	FillFileType(hDlg);
	FillDataType(hDlg);
}

//************************************************************************/
//	Shows Directory or Album Pane
//************************************************************************/
void ShowAlbumWindow(HWND hDlg, BOOL bViewAlbums)
//************************************************************************/
{
	HMENU hMenu, hOldMenu; 

    if( bImported )
	    return;     // albums not supported
	
	hOldMenu = GetMenu(hDlg);

	if (bViewAlbums)
	{
		// bring Albums to front
		ShowPane(hDlg, IDC_PANE0, IDC_PANE1, NO);
		ShowPane(hDlg, IDC_PANE1, IDC_PANE2, YES);
		if (hOldMenu)
		{
            hMenu = LoadMenu(PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(MENU_1));
			if (hMenu)
			{
				SetMenu(hDlg, hMenu);
				DestroyMenu(hOldMenu);
			}
		}
//		if (hWindow = GetFocus())
//			if (hWindow = GetParent(hWindow))
//				if (hWindow == hDir)
//					SetFocus(hAlb);
	}
	else 
	{
		// bring Directories to front
		ShowPane(hDlg, IDC_PANE1, IDC_PANE2, NO);
		ShowPane(hDlg, IDC_PANE0, IDC_PANE1, YES);

		if (hOldMenu)
		{
			// delete album menu
			// add album menu
   			hMenu = LoadMenu(PictPubApp.GetResourceHandle(), MAKEINTRESOURCE(MENU_1));

			if( hMenu )
			{
				DeleteMenu(hMenu, IDM_BSR_ALB_MENU, MF_BYPOSITION);		
				SetMenu(hDlg, hMenu);
				DestroyMenu(hOldMenu);
			}
		}
//		if (hWindow = GetFocus())
//			if (hWindow = GetParent(hWindow))
//				if (hWindow == hAlb)
//					SetFocus(DIRLIST(hDlg));
	}
}

//************************************************************************/
//	Handles Ok or return in dlg
//************************************************************************/
void HandleOK(HWND hDlg)
//************************************************************************/
{
	BSRInvalidateDriveIDs();
	if (lpBsr->OpenState.bSaving)
	{
		HandleSaveOK(hDlg);
		return;
	}
	else	// Load dlg
	{
		HandleLoadOK(hDlg);
		return;   
	}
}

//************************************************************************/
//	setsup the search spec and file name text.
//	if lpPath != NULL, then it will put lpPath up in the text box (with search spec)
//************************************************************************/
void SetUpSearchSpec(HWND hDlg, LPTSTR lpPath)
//************************************************************************/
{
	FNAME szName;
	TCHAR szTemp[100]; 
	LPTSTR lpWild;
	
	if (GetDlgItemText(FILEWINDOW(hDlg), IDC_BSR_FILENAME, szName, sizeof(FNAME)) <= 0)
		lstrcpy(szName, lpBsr->OpenState.szFileSpec);
	if (lStrChr(szName, _T(',')))
		lstrcpy(szName, lpBsr->OpenState.szFileSpec);
	lstrcpy(szTemp, szName);
	lstrcpy(szName, filename(szTemp));
	lstrcpy(lpBsr->OpenState.szNewFileSpec, szName);
	lpWild = lStrChr(szName, _T('*'));

	// if its a file name then make into wildcard
	if (!lpWild || lpWild > (LPTSTR)extension(szName))
	{
		SetExtension(szName,lpBsr->OpenState.idFileType, NO);
		if (lpPath)
			if (*lpPath)
			{
				lstrcpy(szTemp,lpPath);
            	FixPath(szTemp);
				lstrcat(szTemp, szName);
				lstrcpy(szName, szTemp);
			}
		SetFileNameText(hDlg, szName, TRUE);
		lstrcpy(szName, _T("*.*"));			

        if( bImported )
        {
		    if( lpBsr->OpenState.idFileType != IDT_ALLIMPORTIMAGES )
			    SetExtension( szName, lpBsr->OpenState.idFileType, NO );
        }
        else
        {
		    if( lpBsr->OpenState.idFileType != IDT_ALLIMAGES )
			    SetExtension( szName, lpBsr->OpenState.idFileType, NO );
        }
		lstrcpy(lpBsr->OpenState.szSearchSpec, szName);
	}	
	else 
	{
        if( bImported )
        {
		    if (lpBsr->OpenState.idFileType == IDT_ALLIMPORTIMAGES)
		    {
			    SetExtension(szName,IDT_ALLIMPORTFILES, NO);
			    lstrcpy(lpBsr->OpenState.szNewFileSpec, szName);
			    lstrcpy(lpBsr->OpenState.szSearchSpec, szName);
			    // for display only
			    SetExtension(szName,lpBsr->OpenState.idFileType, NO);
		    }
		    else
		    {
			    SetExtension(szName,lpBsr->OpenState.idFileType, NO);
			    lstrcpy(lpBsr->OpenState.szNewFileSpec, szName);
			    lstrcpy(lpBsr->OpenState.szSearchSpec, szName);
		    }
        }
        else
        {
		    if (lpBsr->OpenState.idFileType == IDT_ALLIMAGES)
		    {
			    SetExtension(szName,IDT_ALLFILES, NO);
			    lstrcpy(lpBsr->OpenState.szNewFileSpec, szName);
			    lstrcpy(lpBsr->OpenState.szSearchSpec, szName);
			    // for display only
			    SetExtension(szName,lpBsr->OpenState.idFileType, NO);
		    }

		    else
		    {
			    SetExtension(szName,lpBsr->OpenState.idFileType, NO);
			    lstrcpy(lpBsr->OpenState.szNewFileSpec, szName);
			    lstrcpy(lpBsr->OpenState.szSearchSpec, szName);
		    }
        }

		if (lpPath)
			if (*lpPath)
			{
				lstrcpy(szTemp,lpPath);
            	FixPath(szTemp);
				lstrcat(szTemp, szName);
				lstrcpy(szName, szTemp);
			}
		SetFileNameText(hDlg, szName, TRUE);
	}
}

//************************************************************************/
//	Changes the current working directory based on lpBsr->OpenState.bNavigated,
//		lpBsr->OpenState.szPath, and lpBsr->OpenState.idFileType.
//	Does NOTHING in ALBUM mode.
//	Returns TRUE if directory actually changed.
//	Does NOT change Directory listing, but MAY change drive combo.
//************************************************************************/
BOOL SetDirectoryFromDef(HWND hDlg)
//************************************************************************/
{
	FNAME szPath, szNewPath;
	int drive;
	BOOL bSuccess, bTried;
	UINT errmode;
	int i, type;
	
	
	if (ALBMODE)
		return(FALSE);
	
	errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
	drive = CURRENTDRIVE;
	szPath[0] =  _T('\0');
	if (ChangeDrive(drive))
		if (!CURRENTDIR(szPath, sizeof(szPath)))
			szPath[0] =  _T('\0');
	SetErrorMode(errmode);
	
	bTried = FALSE;
	// Use szPath if there is one
	if (lpBsr->OpenState.szPath[0] != _T('\0'))
	{
		stripfile(lpBsr->OpenState.szPath);
		FixPath(lpBsr->OpenState.szPath);
		// are we already there?
		if (StringsEqual(lpBsr->OpenState.szPath, szPath))
		{
			lpBsr->OpenState.szPath[0] = _T('\0');
			return(FALSE);
		}
		bSuccess = changeCWD(lpBsr->OpenState.szPath);
		lpBsr->OpenState.szPath[0] = _T('\0');		
		if (bSuccess)
			goto Done;
		bTried = TRUE;
		// if lpBsr->OpenState.szPath fails,  use file type
	}
	// use file type default directory
	if (lpBsr->OpenState.bNavigated)
		return(FALSE);
	GetFileLocation(lpBsr->OpenState.idFileType, szNewPath);
	if (szNewPath[0] != _T('\0'))
	{
		stripfile(szNewPath);
		FixPath(szNewPath);
		if (bTried)
			errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
		bSuccess = changeCWD(szNewPath);
		if (bTried)
			SetErrorMode(errmode);
	}
	else
		bSuccess = FALSE;
Done:
	if (!bSuccess)
	{
		// try staying at old dir
		errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
		bSuccess = ChangeDrive(drive);
		SetErrorMode(errmode);
	}		  
	if (!bSuccess)
	{
	// try first fixed or net drive
		for(i=0; i<26; i++)
		{
			type = GetExtendedDriveType(i);
			if (type == DRIVE_FIXED || type == DRIVE_REMOTE)
			{
				if (ChangeDrive(i))
					break;
			}	
		}
	}
   	if (drive != CURRENTDRIVE || !bSuccess)
   	{
   		FillDrivesCombo(hDlg);
   		SetDriveSizeText(hDlg);
   	}
   	return(bSuccess);
}
//************************************************************************/
//	Updates/Creates all thumbs that are currently selected in the file list
//	based on the valiadtion level (level)
//************************************************************************/
void UpdateFileThumbs(HWND hDlg, WORD level)
//************************************************************************/
{
	FILELOC File;
	int num;
	
	BSRInvalidateDriveIDs();
	if (!OpenFileList(hDlg))
		return;
	num = 0;
	while(GetNextFile(&File))
		if (level & VL_NOTIME)
			num++;
		else
			num += UpdateCreateThumb(&File, VL_CHECK, YES); 
	if (!OpenFileList(hDlg))
		return;
	ProgressBegin(num, 0, hDlg);
	while(GetNextFile(&File) && (!(level&VL_ESCAPABLE) || !SAFECANCEL))
		UpdateCreateThumb(&File, level, YES); 
	ProgressEnd();
}

//************************************************************************/
//	Deletes all thumbs that are currently selected in the file list.
//************************************************************************/
void DeleteFileThumbs(HWND hDlg)
//************************************************************************/
{
	FILELOC File; 
	ENTRY entry;
	LPCTHMNAIL lpThm;
	BOOL ret;
	
	BSRInvalidateDriveIDs();
	if (!OpenFileList(hDlg))
		return;
	if (AstralOKCancel(IDS_CONF_DELETETHM) != IDOK)
		return;
	while(GetNextFile(&File))
	{
	 	// Delete Thumbnail
		if (FindEntry(&File, &entry, TRUE, ALBMODE))
			if (lpThm = CTHMOpen(&entry))
			{
				ret = CTHMDeleteFrame(lpThm);
				CTHMClose(lpThm);
				if (!ret)
					if (AstralOKCancel(IDS_EDELETETHUMBNAIL) == IDCANCEL)
						break;
			}
	}
}

//*************************** LOCAL  FUNCTIONS ***************************/

//************************************************************************/
//	Sets the drive and current directory according to the specified path.
//	Path must be path only, no file name.
//	Returns TRUE if completely successful.
// 	Updates no controls.
//************************************************************************/
LOCAL BOOL changeCWD(LPTSTR lpPath)
//************************************************************************/
{
	FNAME szDir;
	int drive;
	WORD wFirst, wNext;
	
	// delete backslash if not root

	lstrcpy(szDir, lpPath);

	if( IsDBCSLeadByte(( BYTE )*szDir ))
		wFirst = *(( LPWORD )szDir );
	else
		wFirst = *szDir;

	if( IsDBCSLeadByte(( BYTE )*MyCharNext( szDir )))
		wNext = *(( LPWORD )MyCharNext( szDir ));
	else
		wNext = *MyCharNext( szDir );

	if( MBStrlen( szDir) != 1 || wFirst != _T('\\'))
		StripTrailer(szDir, _T('\\'));
	
	// change drive
	if( MBStrlen( szDir ) > 1 && wNext == _T(':'))
	{
		drive = wFirst;

		if ( drive >= _T('A') && drive <= _T('Z') )
			drive -= _T('A');
		else 
			drive -= _T('a');

		if (!ChangeDrive(drive))
			return(FALSE);

		// go to root
		if( MBStrlen(szDir) == 2)
			lstrcat(szDir, _T("\\"));	
	}
	// change directory
	return (CHDIR(szDir));
}

//************************************************************************/
//	Handles getting save file list.
//	Exits the dialog if sucessful.
//************************************************************************/
LOCAL void HandleSaveOK(HWND hDlg)
//************************************************************************/
{
	FILELOC File;
	int bPrompt = TRUE;

	// check selection box
	lpBsr->OpenState.numFiles = ParseListBoxMessage( lpBsr->OpenState.bMultiListBox,
					FILELIST( hDlg ), LB_GETSELCOUNT, 0, 0L );

	// too many files
	if( lpBsr->OpenState.numFiles > 1 )
	{
		Message( IDS_SAVE_TOOMANY );
		return;
	}
	if( lpBsr->OpenState.numFiles == 1 )
	{	
        // return the one file
		if( !MakeFinalList( hDlg ))     // should not happen
		{	
			MessageBeep( MB_ICONASTERISK );
			return;
		}
		goto Verify;
	}

	// there are no selected files
	// Get file name box entry	(sets bPrompt)
	if( !HandleFileNameOK( hDlg, &File, &bPrompt ))
		return;

	// got a valid file name
	if( !IsValidFileName( File.szFile ))
	{
		Message( IDS_INVALIDNAME, ( LPTSTR )File.szFile );
		return;
	}

	lpBsr->OpenState.numFiles = 1;
	if( !( lpBsr->OpenState.lpFileList = ( FNAME FAR * )Alloc( sizeof( FNAME ) + sizeof( VOLUMEID ))))
	{
		Message( IDS_EMEMALLOC );
		return;
	}
	
	lpBsr->OpenState.lpFileListVolumes = ( LPVOLUMEID )( lpBsr->OpenState.lpFileList[1] );
	lpBsr->OpenState.lpFileListVolumes[0] = File.volume; 
	lstrcpy( lpBsr->OpenState.lpFileList[0], File.szFile );

Verify:
	if( VerifySaveFile( bPrompt ))
		AstralDlgEnd( hDlg, TRUE );
	else 
		( FreeUp(( LPTSTR )lpBsr->OpenState.lpFileList ));
	return;
}

//************************************************************************/
//	Handles getting load file list.
//	Exits the dialog if sucessful.
//************************************************************************/
LOCAL void HandleLoadOK(HWND hDlg)
//************************************************************************/
{
	FILELOC File;
	int bDummy;

	// check selection box
	lpBsr->OpenState.numFiles = ParseListBoxMessage(lpBsr->OpenState.bMultiListBox,
					                FILELIST(hDlg),LB_GETSELCOUNT,0, 0L);
     
	if( lpBsr->OpenState.numFiles > 0 )
	{
		if (!MakeFinalList(hDlg))
		{
			MessageBeep(MB_ICONASTERISK);
			return;
		}
		goto Verify;
	}
	// no selection, use text box
	if (!HandleFileNameOK(hDlg, &File, &bDummy))
		return;
	// got a valid file name
	lpBsr->OpenState.numFiles = 1;
	if (!(lpBsr->OpenState.lpFileList = (FNAME FAR *)Alloc(sizeof(FNAME)+sizeof(VOLUMEID))))
	{
		Message(IDS_EMEMALLOC);
		return;
	}
	lpBsr->OpenState.lpFileListVolumes = (LPVOLUMEID)(lpBsr->OpenState.lpFileList[1]);
	lpBsr->OpenState.lpFileListVolumes[0] = File.volume; 
	lstrcpy(lpBsr->OpenState.lpFileList[0], File.szFile);

Verify:

	if (VerifyLoadFiles())
		AstralDlgEnd( hDlg, TRUE);
	else
	 	FreeUp((LPTSTR)lpBsr->OpenState.lpFileList);
	return;
}
	
//************************************************************************/
//	Handles the file name text box when OK or return are hit.
//	File list shold be checked for selected items first.
//	It will do one of the following:
//		*Return TRUE with lpFile containing a valid FULL path&file.
//			(file name length NOT checked)
//		*Return FALSE and change directories.
//		*Return FALSE and update file list contents to reflect new search string
//	lpFile must be as large as a FNAME.
//************************************************************************/
LOCAL BOOL HandleFileNameOK(HWND hDlg, LPFILELOC lpOutFile, LPINT lpbPrompt)
//************************************************************************/
{
	int	num;
	DWORD attrib;
	FNAME szFullName;
	FNAME szAlbum;
	LPTSTR lpName;
	LPDBFLL lpDBFLL;
	HWND hWnd;
	
	*lpbPrompt = TRUE;
	// get string
	if (!GetDlgItemText(FILEWINDOW(hDlg), IDC_BSR_FILENAME, 
					lpOutFile->szFile, sizeof(FNAME)))
	{
		MessageBeep(MB_ICONASTERISK);
		return(FALSE);
	}	
	CharUpper( lpOutFile->szFile );
	// is it a list
	if (lStrChr(lpOutFile->szFile, _T(',')) && !lStrChr(lpOutFile->szFile, _T('\\')))
		goto BadName;
	if (ALBMODE)
	{
		lstrcpy(szFullName, lpOutFile->szFile); 
		lstrcpy(szAlbum, _T(""));
		// does contain an album??
		StripTrailer(szFullName, _T(' '));
		lpName = lStrChr(szFullName, _T('\\'));
		if (lpName)
		{
			// got an album
			*lpName++ = _T('\0');
			lstrcpy(szAlbum, szFullName);
			lstrcpy(szFullName, lpName);
			if (lStrChr(szAlbum, _T('*')))
				goto BadName;
		 }
		// wildcard??
		if (lStrChr(szFullName, _T('*')))
		{
			lstrcpy(lpBsr->OpenState.szSearchSpec, szFullName);
			SetFileNameText(hDlg, lpBsr->OpenState.szSearchSpec, TRUE);
			if (!ChangeAlbum(hDlg, szAlbum, NO))
			{	// album does not exist
				SetUpSearchSpec(hDlg, NULL);
				FillFileList(hDlg, NO);
				goto BadName;	// ***ERROR use differeent error for album???
			}
			if (szAlbum[0] == _T('\0'))
				FillFileList(hDlg, NO); 	//not updated because album not changed
			return(FALSE);
		}
		// must be a file name or just an album
		if (szAlbum[0] != _T('\0'))
		{
			hWnd = GetFocus();
			if (hWnd == ALBLIST(hDlg))
				if ((num = ListBox_GetCurSel(hWnd)) != LB_ERR)
				{
					SetFileNameText(hDlg, szFullName, TRUE);
					ChangeAlbum(hDlg, szAlbum, YES);
					SetUpSearchSpec(hDlg, NULL);
					FillFileList(hDlg, NO);
					return(FALSE);
				}
			if (!ChangeAlbum(hDlg, szAlbum, YES))
				{	// album does not exist
					SetUpSearchSpec(hDlg, NULL);
					FillFileList(hDlg, NO);
					goto BadName;	// ***ERROR use differeent error for album???
				}
		}
		SetFileNameText(hDlg, _T(""), YES);
		if (szFullName[0] == _T('\0'))
			return(FALSE);		//just an ALBUM
		// is there was no match in the album then use current
		if (szAlbum[0] == _T('\0'))
			lstrcpy(szAlbum, lpBsr->OpenState.szAlbum);
		if (lpBsr->OpenState.bSaving)
		{
            if( bImported )
            {
			    if (!WITHIN(lpBsr->OpenState.idFileType, IDT_FIRSTIMPORTREAL, IDT_LASTIMPORTREAL))
				    lpBsr->OpenState.idFileType = GetTypeFromExt(szFullName, IDT_AI);	
            }
            else
            {
			    if (!WITHIN(lpBsr->OpenState.idFileType, IDT_FIRSTREAL, IDT_LASTREAL))
				    lpBsr->OpenState.idFileType = GetTypeFromExt(szFullName, IDT_TIFF);	
            }

			AddExtension(szFullName, lpBsr->OpenState.idFileType, YES);
			lstrcpy(lpOutFile->szFile, Browser.AlbumFileDir);
			lstrcat(lpOutFile->szFile, szFullName);
			// must add to the album 
			// Prompt user if there are any special problems
			if (!PromptSaveIntoAlbum(hDlg, szAlbum, lpOutFile, lpbPrompt))
				return(FALSE);
			// AddToAlbum will not do anything if file already in album
			lpDBFLL = FindAlbum(szAlbum);
			GetDriveVolume(lpOutFile->szFile, &lpOutFile->volume, YES);
			if (lpDBFLL)	// ***ERROR IF NOT??
				if(!AddToAlbum(&(lpDBFLL->dbf), lpOutFile, NO)) 		
					return(FALSE); // ***ERROR  failed to add to album		
			return(TRUE);
		}
		// load mode
		num = GuessFileFromAlbum(szAlbum, szFullName, lpOutFile);
		if (num < 1)
			goto BadName;
		if (num > 1)
			{
				Message(IDS_NAME_AMBIGUOUS);
				return(FALSE);
			}
		return(TRUE);
	}  // if ALBMODE
	
	else //DIRMODE
	{
		// Make full path
		if (!TextBoxMakeFullPath(szFullName, lpOutFile->szFile))
			goto BadName;
		// wildcard
		if (lStrChr(lpOutFile->szFile, _T('*')) || lStrChr(lpOutFile->szFile, _T('?')))
		{
			if (lpOutFile->szFile != filename(lpOutFile->szFile))
			{	// file and path with wildcard
//				lstrcpy(lpBsr->OpenState.szSearchSpec, filename(szFullName));
				SetFileNameText(hDlg, filename(szFullName), TRUE);
				if (!ChangeDir(hDlg, stripfile(szFullName), NO))
					FillItems(hDlg, YES, NO);
				return(FALSE);
			}
			else
			{  // just a file with wildcard
				lstrcpy(lpBsr->OpenState.szSearchSpec, lpOutFile->szFile);
				FillFileList(hDlg, NO);
				return(FALSE);
			}
		}
		// is it a directory??
		if (Dos_GetFileAttr(szFullName, &attrib))
		{ // got stat
			if (attrib & ATTRIB_SUBDIR)
			{	// its a directory
				SetFileNameText(hDlg, _T(""), YES);
				if (!ChangeDir(hDlg, szFullName, YES))
					FillItems(hDlg, YES, NO);
				return(FALSE);
			}
		}
		// check if we just hit a direcory 
		hWnd = GetFocus();
		if (hWnd == DIRLIST(hDlg))
			if ((num = ListBox_GetCurSel(hWnd)) != LB_ERR)
			{
				SetFileNameText(hDlg, filename(szFullName), TRUE);
				if (!ChangeDir(hDlg, stripfile(szFullName), NO))
					FillItems(hDlg, YES, NO);
				return(FALSE);
			}
		// must be a file name
		if (lpBsr->OpenState.bSaving)
		{
            if( bImported )
            {
			    if (!WITHIN(lpBsr->OpenState.idFileType, IDT_FIRSTIMPORTREAL, IDT_LASTIMPORTREAL))
				    lpBsr->OpenState.idFileType = GetTypeFromExt(szFullName, IDT_AI);	
            }
            else
            {
			    if (!WITHIN(lpBsr->OpenState.idFileType, IDT_FIRSTREAL, IDT_LASTREAL))
				    lpBsr->OpenState.idFileType = GetTypeFromExt(szFullName, IDT_TIFF);	
            }

			AddExtension(szFullName,lpBsr->OpenState.idFileType, YES);
		}
		lstrcpy(lpOutFile->szFile, szFullName);
		GetDriveVolume(lpOutFile->szFile, &lpOutFile->volume, NO);
		return(TRUE);
	}	// else (DIRMODE)
	
BadName:
	Message(IDS_BADNAME);
	return(FALSE);
}

//************************************************************************
//	Prompts user for save over existing file etc.  
//	May CHANGE lpFile to a better location.
//	Retuns TRUE if the operation was NOT canceled.
// 	On return lpPromptFor == TRUE if the user should still be prompted 
//		for overwriting existing files.
//	Ignores volume info.
//************************************************************************
LOCAL BOOL PromptSaveIntoAlbum(HWND hDlg, LPTSTR szAlbum, LPFILELOC lpFile, LPINT lpPromptFor)
//************************************************************************
{
	FILELOC File;
	LPDBFLL lpDBFLL;
	FNAME szNewDir;
	int ret;
	
	*lpPromptFor = TRUE;
	File = *lpFile;
	CLEARVOLUME(File.volume);
	while (TRUE)
	{
		// File exists?
		if (!FileExists(File.szFile))
			return(TRUE);
		// File is in this album?
		lpDBFLL = FindAlbum(szAlbum);
		if (!lpDBFLL)
			return(FALSE);
		DBFReOpen(&lpDBFLL->dbf, NO);
		ret = DBFFindEntry(&lpDBFLL->dbf, &File);
		DBFClose(&lpDBFLL->dbf);
		if (ret != -1)
		{
			// warn user
			ret = AstralYesToAllString(hDlg, IDD_INTO_THIS_ALB, File.szFile, YES);
			if (ret == IDOK)
			{
				*lpPromptFor = FALSE;
				return(TRUE);
			}
			if (ret == IDC_BSR_NO)
				goto GetNewDir;
			return(FALSE);
		}
		// file exists in some album?
		lpDBFLL = FirstAlb();
		while(lpDBFLL)
		{
			DBFReOpen(&lpDBFLL->dbf, NO);
			ret = DBFFindEntry(&lpDBFLL->dbf, &File);
			DBFClose(&lpDBFLL->dbf);
			if (ret != -1)
			{
				// warn user
				ret = AstralYesToAllStrings(hDlg, 
									IDD_INTO_ALB, 
									File.szFile,
									lpDBFLL->dbf.info.szName, 
									YES);
				if (ret == IDOK)
				{
					*lpPromptFor = FALSE;
					return(TRUE);
				}
				if (ret == IDC_BSR_NO)
					goto GetNewDir;
				return(FALSE);
			}
			lpDBFLL= GetNextAlb();
		}
		// file exists but not in any album... just prompt like normal
		return(TRUE);
GetNewDir:
		// Prompt user for a new directory
		lstrcpy(szNewDir, File.szFile);
		stripfile(szNewDir);
		ret = GetDialogString(hDlg,szNewDir,szNewDir,szNewDir, IDD_SAVENEWDIR);
		if (ret != IDOK)
			return(FALSE);
		MakeFullPath(szNewDir);
		if(!IsDirectory(szNewDir))
		{
			Message(IDS_EINVALIDDIR, (LPTSTR)szNewDir);
			goto GetNewDir;
		}
		// get valid dir
		FixPath(szNewDir);
		File = *lpFile;
		lstrcat(szNewDir, filename(File.szFile));
		lstrcpy(File.szFile, szNewDir);
		*lpFile = File;
	}	// while TRUE
}

//************************************************************************/
//	Changes the directory, and refills file list etc.
//	if !bUpdateFileName, then the file name text box 
//		and search spec are NOT changed.
//	Otherwise they are updated.
//************************************************************************/
BOOL ChangeDir(HWND hDlg, LPTSTR lpDir, BOOL bUpdateFileSpec)
//************************************************************************/
{
	FNAME szDir, szNewDir;
	int drive, i, type;

	drive = CURRENTDRIVE;
	if (!CURRENTDIR(szDir, sizeof(FNAME)))
		szDir[0] = _T('\0');
	if (!changeCWD(lpDir))
		for(i=0; i<26; i++)
		{
			type = GetExtendedDriveType(i);
			if (type == DRIVE_FIXED || type == DRIVE_REMOTE)
				if (ChangeDrive(i))
					return(TRUE);
		}
	
	// update items	
	if (!CURRENTDIR(szNewDir, sizeof(FNAME)))
		szNewDir[0] = _T('\0');
	if (StringsEqual(szDir, szNewDir))
		return(FALSE); 					// no change
	if (drive != CURRENTDRIVE)
	{
		FillDrivesCombo(hDlg);
		SetDriveSizeText(hDlg);
	}
	if (bUpdateFileSpec)
		SetUpSearchSpec(hDlg, NULL); 		// will set file name text
	if (DIRMODE)
		FillDirectoryList(hDlg);
	FillFileList(hDlg, NO);
	return(TRUE);
}

//************************************************************************/
// 	Gets the full path of the currently selected directory.
//	returns lpDir if successful, NULL otherwise.
//************************************************************************/
LOCAL LPTSTR GetSelDir(HWND hDlg, LPTSTR lpDir)
//************************************************************************/
{
	WORD index;
	int i, id;
	FNAME szDir;
	HWND hCombo;

	hCombo = GetDlgItem(DIRWINDOW(hDlg), IDC_BSR_DIRECTORIES);
	if (!hCombo)
		return(NULL);
	// Handle selection changed
	index = ListBox_GetCurSel(hCombo);
	if (index == LB_ERR)
		return(NULL);
	if (ListBox_GetText(hCombo, index, szDir) == LB_ERR)
		return(NULL);
	*lpDir = _T('a') + CURRENTDRIVE;
	*MyCharNextN( lpDir, 1 ) = _T(':');
	*MyCharNextN( lpDir, 2 ) = _T('\0');
	for (i=0; i<=index; i++ )
	{
		ListBox_GetText( hCombo, i, szDir);
		id = ExtractStringID( szDir );
		if ( id == IDC_PATHICON_CLOSED && i != index )
			continue;
		lstrcat(lpDir, SkipSpaces(szDir) );
		if ( id != IDC_PATHICON_ROOT )
			lstrcat(lpDir, _T("\\") );
	}
	return(lpDir);
}

//************************************************************************/
//	Changes the album, and refills file list etc.
//	if !bUpdateFileName, then the file name text box 
//		and search spec are NOT changed.
//	Otherwise they are updated.
// 	If lpAlbum == "" then nothing happens, but returns TRUE 
//	Returns FALSE if the album was invalid;
//************************************************************************/
LOCAL BOOL ChangeAlbum(HWND hDlg, LPTSTR lpAlbum, BOOL bUpdateFileSpec)
//************************************************************************/
{
	LPDBFLL lpDBFLL;
	
	if (lpAlbum[0] == _T('\0'))
		return(TRUE);
	// check if valid album
	lpDBFLL = FindAlbum(lpAlbum);
	if (!lpDBFLL)
		return(FALSE);
	// change Album
	lstrcpy(lpBsr->OpenState.szAlbum, lpAlbum);
	// update items	
	FillAlbList(hDlg);
	SetPathText(hDlg);
	SetDriveSizeText(hDlg);
	if (bUpdateFileSpec)
		SetUpSearchSpec(hDlg, NULL); // will set file name text
	FillFileList(hDlg, NO);
	if (!StringsEqual(lpBsr->OpenState.szAlbum, lpAlbum))
		return(FALSE);		// was not able to change to that album
	return(TRUE);
}

//************************************************************************/
//	Verifies file name length.
//	If file already exists then verify save over.
//	Works only on first file in the list.
//	Returns TRUE if save should proceed.
//************************************************************************/
LOCAL BOOL VerifySaveFile(BOOL bPromptOnFileExists)
//************************************************************************/
{
	FNAME szName;
	
	if (lpBsr->OpenState.numFiles < 1)
		return(FALSE);
	lstrcpy(szName, lpBsr->OpenState.lpFileList[0]);
#ifndef WIN32
	// check if name too long
	LPTSTR lpExt = extension(szName);
	LPTSTR lpFname = filename(szName);
	if ((lpExt - lpFname) > 8)
	{
        FixFileName( szName );
		if (AstralOKCancel(IDS_TRUNCATE, (LPTSTR)lpBsr->OpenState.lpFileList[0], (LPTSTR)szName) != IDOK)
			return(FALSE);
		// ok to truncate
		lstrcpy(lpBsr->OpenState.lpFileList[0], szName);
	}
#endif
	// check if file exists
	if (!SafeFileExists(szName))
		return(TRUE);
	// check for read only 
	if (!FilePermission(szName, 2L /*Write*/))
	{
		Message(IDS_EWRITEPROTECT, (LPTSTR)szName);
		return(FALSE);
	}
	// verify copy over
	if (bPromptOnFileExists)	  
		if (AstralOKCancel (IDS_OVERWRITEIMAGE, (LPTSTR)szName) != IDOK)
			return(FALSE);
	return(TRUE);
}

//************************************************************************/
//	Verifies file name length.
//	Returns TRUE if load should proceed.
//	May change number or elements of file list
//************************************************************************/
LOCAL BOOL VerifyLoadFiles(void)
{
	FNAME szName;
	LPTSTR lpExt, lpFname;
	int i;
	
	if (lpBsr->OpenState.numFiles < 1)
		return(FALSE);
#ifdef WIN32
	return(TRUE);
#endif
	// check if names too long
	for (i=0; i<lpBsr->OpenState.numFiles; i++)
	{
		lstrcpy(szName, lpBsr->OpenState.lpFileList[i]);
		lpExt = extension(szName);
		lpFname = filename(szName);
		if ((lpExt - lpFname) > 8)
		{
            FixFileName( szName );
			if (AstralOKCancel(IDS_TRUNCATE, (LPTSTR)lpBsr->OpenState.lpFileList[i], (LPTSTR)szName) != IDOK)
				return(FALSE);
			// ok to truncate
			lstrcpy(lpBsr->OpenState.lpFileList[i], szName);
		}
	}
	// all ok
	return(TRUE);
}

//************************************************************************/
// for HandleFileNameOK.  Takes file name text and turns it into full path.
// Still may include wild cards etc.
//************************************************************************/
LOCAL BOOL	TextBoxMakeFullPath(LPTSTR lpFullName, LPTSTR lpFile)
//************************************************************************/
{
	FNAME szDir;
	int len;
	WORD wFirst, wNext;
	
	// make full path out of it
	StripTrailer(lpFile, _T(' '));
	len = MBStrlen( lpFile );

	if( IsDBCSLeadByte(( BYTE )*lpFile ))
		wFirst = *(( LPWORD )lpFile );
	else
		wFirst = *lpFile;

	if( IsDBCSLeadByte(( BYTE )*MyCharNext( lpFile )))
		wNext = *(( LPWORD )MyCharNext( lpFile ));
	else
		wNext = *MyCharNext( lpFile );

	if( wFirst == _T('\\') )		// absolute path just add drive
	{	
		*lpFullName = _T('a')+ CURRENTDRIVE;
		*MyCharNextN( lpFullName, 1 ) = _T(':');
		*MyCharNextN( lpFullName, 2 ) = _T('\0');
		lstrcat( lpFullName, lpFile );

		if( *( MyCharNext( lpFile )) != _T('\0') )
			StripTrailer( lpFullName, _T('\\') );
	}
	else 
    if( len > 1 && wNext == _T(':')) 	// contains drive, full path already
	{ 
		if(( wFirst > _T('z') || wFirst < _T('a') ) && ( wFirst > _T('Z') || wFirst < _T('A') ))
			// not valid drive name
			return( FALSE );

		lstrcpy(lpFullName, lpFile);
		StripTrailer(lpFullName, _T('\\'));
	}
	else  
    if (lStrChr(lpFile, _T(':')))	// has ':' but not in right place
		return( FALSE );
	else						// normal file and/or path	
	{	
		if (!CURRENTDIR(szDir, sizeof(FNAME)))
			return(FALSE);

		lstrcpy(lpFullName, szDir);
		if(( lstrlen( lpFullName ) + lstrlen( lpFile + 3 )) > sizeof( FNAME ))
			return( FALSE );

		StripTrailer(lpFullName, _T('\\'));
		lstrcat(lpFullName, _T("\\"));
		lstrcat(lpFullName, lpFile);
		StripTrailer(lpFullName, _T('\\'));
	} 
	return( TRUE );
}

//************************************************************************/
//	Copys the thumbnail for the src file(if any), and installs the 
//	Copy as the thumbnail for the destfile.
//	Returns TRUE if there was a sucessful copy.
//	Displays error warnings.
//************************************************************************/
BOOL CopyThumbnail(LPFILELOC lpSrcFile, LPFILELOC lpDstFile, ULONG time)
//************************************************************************/
{
	ENTRY entry;
	int ret;
	LPCTHMNAIL lpSrcThm = NULL;
	LPCTHMNAIL lpDstThm = NULL;
	
	// get old thumbnail
	if (!FindEntry(lpSrcFile, &entry, TRUE, ALBMODE))
		return(FALSE);
	if (!(lpSrcThm = CTHMOpen(&entry)))
		return(FALSE);
	// create new thumbnail
	ret = FindOrCreateThumb(lpDstFile, &entry, NO);
	if (ret == BSR_ERROR)
		goto ExitErr;
	if (!(lpDstThm = CTHMOpen(&entry)))
		goto ExitErr;
	if (!CTHMCopyThumb(lpDstThm, lpSrcThm, time))
	{
		CTHMPurge(lpDstThm);
		lpDstThm = NULL;
		FileDelete(lpDstFile->szFile);
		goto ExitErr;
	}
	CTHMClose(lpSrcThm);
	lpSrcThm = NULL;
	CTHMReCreateEasyInfo(lpDstThm, &entry.File, YES);	
	CTHMClose(lpDstThm);
	lpDstThm = NULL;
	return(TRUE);
	
ExitErr:
	if (lpSrcThm)
		CTHMClose(lpSrcThm);
	if (lpDstThm)
		CTHMClose(lpDstThm);
	Message(IDS_ECOPYTHUMBNAIL);
	return(FALSE);		
}

//************************************************************************/
//	Called when a directory is renamed... 
//	Updates all thumb and dbf information.
//	Assumes volume does not change.
//************************************************************************/
LOCAL void	RenameDirectoryThumbs(LPTSTR lpDir, LPTSTR lpNewDir)
//************************************************************************/
{
	FNAME szDir, szNewDir;
	FNAME szNewName, szDirFound, szDBF;
	FILELOC OldFile;
	DBFS dbf;
	ENTRY entry;
	FINDDATA afile;
	int len, newlen, num;
	VOLUMEID volume;
	BOOL ok;
	
	GetDriveVolume(lpDir, &volume, YES);
	
	lstrcpy(szDir, lpDir);
	lstrcpy(szNewDir, lpNewDir);
	
	FixPath(szDir);
	FixPath(szNewDir);
	len = lstrlen(szDir);
	newlen = lstrlen(szNewDir);
	
	if (StringsEqualN(szDir, szNewDir, Max(len, newlen)))
		return;
		
	// do directories
	DBFBeginInfo(&afile, ID_DIRECTORY);
	while(DBFNextInfo(&dbf, &afile, YES, NO))
	{
		if (!EqualVolume(&volume,&dbf.info.volume))
			continue;
	
		if (StringsEqualN(dbf.info.szName, szDir, len))
		{
			// name needs to be changed
			// copy dbf
			ok = FALSE;
			lstrcpy(szNewName, dbf.szFile);
			stripfile(szNewName);
			lstrcpy(szDirFound,dbf.info.szName); 
			lstrcpy(szDBF,dbf.szFile); 

			// change name of dbf if needed
			if (szDir[0] == szNewDir[0])
			{
				// name only needs to be changed
				lstrcpy(dbf.info.szName, szNewDir);
				lstrcat(dbf.info.szName, (LPTSTR)szDirFound + len);
				if (DBFPutInfo(&dbf))
					ok = TRUE;
			}
			else if (GetNewDBFName(szNewName, szNewName, szNewDir, ID_DIRECTORY))
			{
				DBFClose(&dbf);
				// name and drive changed
				if (CopyFile(szDBF, szNewName, TRUE))
					if (DBFOpenByFile(szNewName, &dbf,YES))
					{
						lstrcpy(dbf.info.szName, szNewDir);
						lstrcat(dbf.info.szName, (LPTSTR)szDirFound + len);
						if (DBFPutInfo(&dbf))
						{
							FileDelete(szDBF);
							ok = TRUE;
						}
						DBFClose(&dbf);
					}
				DBFReOpen(&dbf, YES);
			}
			if (!ok)
				Message(IDS_ECHANGETHUMBS);
			// change name of dbf if needed
			if (szDir[0] != szNewDir[0])
			{
				
			}
		}	
		// change any entries
		num = 0;
		while(DBFGetEntryNum(&dbf, num++, &entry))
		{
			if (StringsEqualN(entry.File.szFile, szDir, len))
			{
				// entry needs to be changed
				OldFile = entry.File;
				lstrcpy(entry.File.szFile, szNewDir);
				lstrcat(entry.File.szFile, (LPTSTR)OldFile.szFile + len);
				if (!DBFPutEntry(&dbf, &entry, &OldFile))
					Message(IDS_ECHANGETHUMBS);
			} // end if found match
		} // while got entry
		DBFClose(&dbf);
	} // while DBFNextInfo
		
	// change albums
	DBFBeginInfo(&afile, ID_ALBUM);
	while(DBFNextInfo(&dbf, &afile,YES, NO))
	{
		// change any entries
		num = 0;
		while(DBFGetEntryNum(&dbf, num++, &entry))
		{
			if (StringsEqualN(entry.File.szFile, szDir, len)
					&& EqualVolume(&volume, &entry.File.volume))
			{
				// entry needs to be changed
				OldFile = entry.File;
				lstrcpy(entry.File.szFile, szNewDir);
				lstrcat(entry.File.szFile, (LPTSTR)OldFile.szFile + len);
				if (!DBFPutEntry(&dbf, &entry, &OldFile))
					Message(IDS_ECHANGETHUMBS);
			} // end if found match
		} // while got entry
		DBFClose(&dbf);
	} // while DBFNextInfo
}	

//***********************************************************************
//	gets a 'special' thumb from photo_cd & old GraphicsWorks & 
//  Designer (3.1 type thumbs)
//***********************************************************************
LPCTHMNAIL GetCDThumb(LPFILELOC lpFile)
//***********************************************************************
{
	LPCTHMNAIL lpCThm;
	FNAME szDir;
	UINT  uErrorMode;

	if (BSRGetDriveType(lpFile->szFile[0]) != DRIVE_CDROM)
		return(NULL);
		
     lpCThm = NULL;
	// Turn off Windows' Error Box
	uErrorMode = SetErrorMode( SEM_FAILCRITICALERRORS );
	if (StringsEqual(extension(lpFile->szFile),_T(".pcd")))
	{
		if(IsPCDImage(lpFile->szFile))
		{
			// check for photo cd thumbs
			if (!FileExists(lpFile->szFile))
				goto Exit;
			if (!VolumeCheck(lpFile, NULL))
				goto Exit;
			lpCThm = CReadPCDThumb(lpFile->szFile);
		}
	}
	else if (StringsEqual(extension(lpFile->szFile),_T(".jpg")) || 
		StringsEqual(extension(lpFile->szFile),_T(".tif")))
	{
		// check for 3.1 thumbs in root:|thumbs
		szDir[0] = lpFile->szFile[0];
		lstrcpy( MyCharNext( szDir ), _T(":\\thumbs"));
		if (!IsDirectory(szDir))
			goto Exit;
		lpCThm = CReadOldThumb(lpFile->szFile, szDir);
	}
Exit:
	SetErrorMode( uErrorMode );
	return(lpCThm);
}

//***********************************************************************
LFIXED SetupPrintThumbs(LPLFIXED lpwidth, LPLFIXED lpheight)
//***********************************************************************
{
	int iPrResX, iPrResY;
	LFIXED PageWidth, PageHeight;
	extern HDC hPrinterDC;

	/* get the printer of the current printer */
	if ( !(hPrinterDC = GetPrinterDC()) )
		return(FALSE);
	/* how big the image will be on the output device */
	iPrResX = GetDeviceCaps(hPrinterDC, LOGPIXELSX);
	iPrResY = GetDeviceCaps(hPrinterDC, LOGPIXELSY);

	/* Get size of printer page */
	PageWidth  = FGET(GetDeviceCaps(hPrinterDC, HORZRES), iPrResX);
	PageHeight = FGET(GetDeviceCaps(hPrinterDC, VERTRES), iPrResY);
	
	*lpwidth = PageWidth;
	*lpheight = PageHeight;
	return(TRUE);
	
}

//***********************************************************************
BOOL PrintThumbs(HWND hDlg, BOOL bSelectedOnly)
//***********************************************************************
{
	int fileCount, curFile, startFile;
	BOOL aborted = FALSE;
	RECT rRect;
	FILELOC File;
	ENTRY entry;
	int curColumn, curRow;
	int pgColumns, pgRows;
	DWORD Extent;
	FNAME szFile;
	LPTSTR lpString;
	HWND hListBox;
	BOOL bRet;
	FENTRY szEntry;
	STRING   szDocName;

	LPCTHMNAIL lpThm;
	LPFRAME lpFrame;
	int edWidth, edHeight;
	int myWidth, myHeight;
	LFIXED scWidth, scHeight;
	LFIXED PageWidth, PageHeight;
	int dxPrinter, dyPrinter;
    int dx, dy;	
	PAGE oldPage;
	EDIT oldEdit;
	int iPrResX, iPrResY;
	int length;
	HPEN hPen, hOldPen;
	HBRUSH hOldBrush;
	TEXTMETRIC tm;
//	HFONT hFont, hOldFont;
	BOOL bOldHints;
	HWND hWndAbort;      /* handle to modeless abort dialog box */
    POINT   point;
    SIZE   size;

	
	bOldHints = Control.Hints;
	
	oldPage = Page;
	oldEdit = Edit;

	if (bSelectedOnly)
	{
		if (!(fileCount=OpenFileList(lpBsr->OpenState.hBrowser)))
			return(FALSE);
	}
	else
	{
		hListBox = FILELIST(lpBsr->OpenState.hBrowser);
		fileCount = ListBox_GetCount(hListBox);
		if (!fileCount || fileCount == LB_ERR) 
			return(FALSE);
	}

	/* get the printer of the current printer */
	if ( !(hPrinterDC = GetPrinterDC()) )
		return( FALSE );

	ProgressBegin(1,0,hDlg);

	hPen = CreatePen( PS_SOLID, 1, RGB(0, 0, 0) );
	hOldPen = (HPEN)SelectObject( hPrinterDC, hPen );
	hOldBrush = (HBRUSH)SelectObject( hPrinterDC, GetStockObject(WHITE_BRUSH) );

	/* Determine the banding capabilities of the printer driver */
	BandingDevice   = GetDeviceCaps(hPrinterDC, RASTERCAPS) & RC_BANDING;
	bNextBandReached = FALSE;
	BandInfoDevice = Escape(hPrinterDC, QUERYESCSUPPORT, 2,(LPTSTR)&BandInfoDevice, NULL);

	hWndAbort = NULL;
	fAbortPrint = FALSE;

	/* Set up abort dialog box */
	EnableWindow( hDlg, FALSE );
	GetClientRect( PictPubApp.Get_hWndAstral(), &rRect );
	InflateRect( &rRect, 1, 1 );

	hWndAbort = AstralDlg(YES, PictPubApp.GetResourceHandle(), hDlg, 
						IDD_ABORT, DlgPrintAbortProc);

	/* Give abort procedure to gdi to check abort print command */
	lpAbortTest = (ABORTPROC)MakeProcInstance((FARPROC) AbortTest, PictPubApp.Get_hInstAstral() );

	if ((iEscErr = SetAbortProc( hPrinterDC, lpAbortTest )) < 0)
	{
		aborted = FALSE;
		goto test_done;
	}

	Edit.Distortable = NO;
	Edit.ScaleX = Edit.ScaleY = 100;
 	Edit.Crop.left   = Edit.Crop.top = 0;
	Edit.Crop.right  = 63;
	Edit.Crop.bottom = 63;
	Edit.Resolution  = Edit.CropResolution = 1;
	Edit.Angle       = 0;
	Edit.Width       = FGET( (Edit.Crop.right-Edit.Crop.left+1),  Edit.CropResolution );
	Edit.Height      = FGET( (Edit.Crop.bottom-Edit.Crop.top+1), Edit.CropResolution );
	Page.Centered    = 0;
	
	/* Use the device resolution (in pixels per inch) to compute */
	/* how big the image will be on the output device */
	iPrResX = GetDeviceCaps(hPrinterDC, LOGPIXELSX);
	iPrResY = GetDeviceCaps(hPrinterDC, LOGPIXELSY);

	/* Get size of printer page */
	dxPrinter = GetDeviceCaps( hPrinterDC, HORZRES );
	dyPrinter = GetDeviceCaps( hPrinterDC, VERTRES );
	PageWidth  = FGET(dxPrinter, iPrResX);
	PageHeight = FGET(dyPrinter, iPrResY);

	if( !BandingDevice ) {
		/* Create a single band for the entire page */
		rPrintBand.top    = 0;
		rPrintBand.left   = 0;					   
		rPrintBand.bottom = dyPrinter-1;
		rPrintBand.right  = dxPrinter-1;
	}

	if (fileCount <= 0) 
	{
		aborted = TRUE;
		goto test_done;         
	}

	/* Find out the number of Rows and Columns of tiled images */
	pgColumns = Browser.PTNumCols;
	pgRows    = Browser.PTNumRows;

	/* Scale the image to fit the tile size */
	if (Browser.bPTRows)
	{
		scHeight = (PageHeight / pgRows);
		scWidth  = (scHeight*iPrResX)/iPrResY;
	}
	else
	{
		scWidth  = (PageWidth  / pgColumns);
		scHeight = (scWidth*iPrResY)/iPrResX;
	}	
	edWidth  = (long)((scWidth  -  8192L) * iPrResX) >> 16;
	edHeight = (long)((scHeight - 20480L) * iPrResY) >> 16;


//	dx = (scWidth/14)>16L;
//    hFont = CreateFont (0,dx,0,0,FW_DONTCARE,0,0,0,0,0,0,0,FF_DONTCARE,NULL);
//	if (hFont)
//		hOldFont = SelectObject(hPrinterDC, hFont);

	/* Get the text metric information */
	GetTextMetrics( hPrinterDC, &tm );
	dy = tm.tmHeight + tm.tmInternalLeading + 4;


	/* Send document name to spooler */
	if (ALBMODE)
		lstrcpy( szDocName, lpBsr->OpenState.szAlbum);
	else
		CURRENTDIR(szDocName, sizeof(szDocName));

	PrinterDocInfo.cbSize = sizeof(DOCINFO);
	PrinterDocInfo.lpszDocName = szDocName;
	PrinterDocInfo.lpszOutput  = NULL;      

	PrinterDocInfo.cbSize = sizeof(DOCINFO);
	PrinterDocInfo.lpszDocName = szDocName;
	PrinterDocInfo.lpszOutput  = NULL;	

	if ((iEscErr = StartDoc( hPrinterDC, &PrinterDocInfo )) < 0)
	{
		aborted = FALSE;
		goto test_done;
	}
	startFile = 0;
	// loop on pages
	do
	{	
		Page.OffsetX = 0;
		Page.OffsetY = 0;
		do
		{
			if ( BandingDevice )
			{
				/* Get the next band rectangle */
				if ((iEscErr = Escape(hPrinterDC, NEXTBAND, 
					0, NULL, (LPTSTR)&rPrintBand)) <0)
				{
					goto test_done;
				}
				bNextBandReached = TRUE;
			}
	
			if ( IsRectEmpty( &rPrintBand ) )
				break;
	
			curFile = startFile;
	
			/* Send the multiple print commands to the printer */
			for(curRow = 0; curRow < pgRows; curRow++ )
			{
				for(curColumn = 0; curColumn < pgColumns ; curColumn++)
				{
					if (bSelectedOnly)
						bRet = (GetNumFile(curFile, &File) != NULL);
					else
					{
						bRet = (ListBox_GetText(hListBox, curFile,szEntry)!=LB_ERR);
		 				if (bRet)
							LBEntryToFileLocation(szEntry, &File);
					}
						
					if (!aborted && bRet)
					{
						// make image
						lpFrame = NULL;
						lpThm = NULL;
						if (FindEntry(&File, &entry, YES, ALBMODE))
							if (lpThm = CTHMOpen(&entry))
								lpFrame = CTHMGetFrame(lpThm);

						// Make sure we put something in the abort dialog box...
						SendMessage( hWndAbort, WM_COMMAND, ID_FILENAME, (LPARAM)((LPTSTR)File.szFile) );

						if (AstralClockCursor(curFile, fileCount, TRUE))
						{
							aborted = TRUE;
							break;  
						}

						AstralCursorEnable( FALSE );
//						Control.Hints = FALSE;
						/* Draw the Outline Box */
						rRect.left = rRect.right  = 
                     (((long)curColumn * scWidth)  * iPrResX); // CALFIX >> 16;
						rRect.top  = rRect.bottom =
                     (((long)curRow    * scHeight) * iPrResY); // CALFIX >> 16;
						
                  rRect.right  += (scWidth  * iPrResX); // CALFIX >> 16;
                  rRect.bottom += (scHeight * iPrResY); // CALFIX >> 16;
						
                  MoveToEx( hPrinterDC, rRect.left,  rRect.top, &point );
						LineTo( hPrinterDC, rRect.right, rRect.top );
						LineTo( hPrinterDC, rRect.right, rRect.bottom );
						LineTo( hPrinterDC, rRect.left,  rRect.bottom );
						LineTo( hPrinterDC, rRect.left,  rRect.top );
						
						/* Print out the label */
						lstrcpy(szFile, filename(File.szFile));
						length = lstrlen(szFile);
                  Extent = GetTextExtentPoint( hPrinterDC, szFile, length, &size );
	
                  dx = size.cx; // CALFIX LOWORD(Extent);
                  dy = size.cy; // CALFIX HIWORD(Extent);
	
						ColorText( hPrinterDC, 
									(rRect.left + rRect.right-dx)/2, 
									rRect.bottom - (iPrResY/8)-(dy/2), 
									szFile, 
									length, 
									RGB(0, 0, 0));
	
						/* Move up the bottom */
						rRect.bottom -= (iPrResY/4);
						
						/* Draw the bottom of the text box */
                        MoveToEx( hPrinterDC, rRect.left,  rRect.bottom, &point );
						LineTo( hPrinterDC, rRect.right, rRect.bottom );
						
						/* Perform Our Page Setup */
						Page.Centered = FALSE;
						if (lpFrame)
						{
							// compute image bounds
							Edit.Depth = FrameDepth(lpFrame);
 							Edit.Crop.left   = Edit.Crop.top = 0;
							Edit.Crop.right  = FrameXSize(lpFrame)-1;
							Edit.Crop.bottom = FrameYSize(lpFrame)-1;
							Edit.Width       = FGET( (Edit.Crop.right-Edit.Crop.left+1),  Edit.CropResolution );
							Edit.Height      = FGET( (Edit.Crop.bottom-Edit.Crop.top+1), Edit.CropResolution );
							myWidth = edWidth;
							myHeight = edHeight;
							ScaleToFit( &myWidth, &myHeight, (int)FMUL( iPrResX, Edit.Width ),
								 (int)FMUL( iPrResY, Edit.Height ) );
							Edit.Width  = FGET(myWidth,  iPrResX);
							Edit.Height = FGET(myHeight, iPrResY);
							
							/* Compute image position on output device */
							Page.OffsetX = FGET((rRect.left+rRect.right)/2, iPrResX);
							Page.OffsetY = FGET((rRect.top+rRect.bottom)/2, iPrResY);
						
							Page.OffsetX -= (Edit.Width / 2L);
							Page.OffsetY -= (Edit.Height / 2L);
						
							
							/* Print the individual Tiled image */
							if (!PrintFile( hDlg, szFile, NO, NULL, lpFrame ))
							{
								aborted = TRUE;
							}
						}
						else
						{
							/* Print out the message */
							if (AstralStr(IDS_NOTHUMB, &lpString))
								lstrcpy(szFile, lpString);
							else
								lstrcpy(szFile, _T("----"));
							length = lstrlen(szFile);
                            Extent = GetTextExtentPoint( hPrinterDC, szFile, length, &size );
                            dx = size.cx; // CALFIX LOWORD(Extent);
                            dy = size.cy; // CALFIX HIWORD(Extent);
							ColorText( hPrinterDC, 
										(rRect.left + rRect.right-dx)/2, 
										(rRect.bottom + rRect.top-dy)/2, 
										szFile, 
										length, 
										RGB(0, 0, 0));
						}
						AstralCursorEnable( TRUE );
//						Control.Hints = bOldHints;
						if (lpThm)
							CTHMClose(lpThm);
					}
					curFile++;
				}
			}
		}
		while( BandingDevice && (!aborted) );
		
test_done:
		if (aborted)
			break;
		else
			EndPage( hPrinterDC );

		startFile = curFile;
		if (BandingDevice && (!aborted))
		{
			// to fix banding device bug
			Escape(hPrinterDC, ENDDOC,   0, NULL, 0L);
			if ((iEscErr = Escape(hPrinterDC, STARTDOC,
					lstrlen( szDocName ), (LPTSTR)szDocName, 0L)) < 0)
			{
				aborted = TRUE;
				goto test_done;
			}
		}
		if (bSelectedOnly)
			bRet = (GetNumFile(curFile, &File) != NULL);
		else
		{
			bRet = (ListBox_GetText (hListBox, curFile, (long)(LPTSTR)szEntry)!=LB_ERR);
			if (bRet)
				LBEntryToFileLocation(szEntry, &File);
		}
	}while(!aborted && bRet); 

	if (aborted)
		AbortDoc( hPrinterDC );
	else
	{
		EndDoc( hPrinterDC );
	 	if ((iEscErr = StartDoc( hPrinterDC, &PrinterDocInfo )) < 0)
	 	{
	 		aborted = TRUE;
	 		goto test_done;
	 	}
	}	


	if ( hWndAbort )
	{
		/* abort dialog box still up, so bring it down  */
		AstralDlgEnd(hWndAbort, TRUE);
		hWndAbort = NULL;
	}

	SelectObject( hPrinterDC, hOldBrush );
	SelectObject( hPrinterDC, hOldPen );
	DeleteObject( hPen );
	
//	SelectObject( hPrinterDC, hOldFont );
//	if (hFont)
//		DeleteObject(hFont);

	DeleteDC( hPrinterDC );
	hPrinterDC = NULL;

	FreeProcInstance((FARPROC)lpAbortTest );

	Page = oldPage;
	Edit = oldEdit;

	if ( aborted )
	{
		PrintErr(hDlg, SP_USERABORT);
	}
	else
	{
		if ( iEscErr < 0 )
		{
			PrintErr(hDlg, iEscErr);
		}
	}

	ProgressEnd();
	return(!aborted);
}

