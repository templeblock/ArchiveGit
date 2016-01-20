//
// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// handles file search for Browser dlg

#include "pp.h"
#include <errno.h> 
#ifndef WIN32
#include <malloc.h>
#endif
#include "search.h"
#include "bsrtypes.h"
#include "bsrutil.h"
#include "handlebs.h"
#include "browser.h"
#include "bsrid.h"
#include "bsritems.h"
#include "dbfmgr.h"
#include "thmmgr.h"

#define MIN_STACK_SIZE 1024


//********************Global vars****************************************/
// ***FIX - put into DWL_USER
SEARCHSPEC FileSearch;

//********************Static prototypes****************************************/
LOCAL int DoSearch(HWND hDlg);
LOCAL int SearchDBF(LPDBF lpDBF, BOOL bByKey);
LOCAL BOOL CheckKey(LPENTRY lpEntry, LPTSTR lpKey, BOOL bCase);
LOCAL BOOL CheckName(LPTSTR lpFile, LPTSTR lpSpec);
LOCAL int 	SearchDirectory(LPTSTR lpPath);
LOCAL int	SearchDirectoryForKey(LPTSTR lpPath);
LOCAL int SearchCursor(void);
LOCAL void	SetSearchButton(HWND hDlg, BOOL bSearching);
LOCAL void SetSearchedTypeText(HWND hDlg, BOOL bDir);

//************************************************************************/
//**************************Functions*************************************/
//************************************************************************/
LOCAL BOOL Search_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) ;
LOCAL void Search_OnClose(HWND hDlg);
LOCAL void Search_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);
//************************************************************************/
// 	Proc for image browser file search dialog
//************************************************************************/
BOOL WINPROC DlgSearchProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, Search_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_CLOSE, Search_OnClose);
		HANDLE_DLGMSG(hDlg, WM_COMMAND, Search_OnCommand);  
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_BSR_SEARCH);
	}
}	
	
//************************************************************************/
LOCAL BOOL Search_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************/
{
	CenterPopup(hDlg);
	FileSearch.hSearch = hDlg;
	FileSearch.dbfResult.info.szName[0] = _T('\0');
	FileSearch.dbfResult.szFile[0] = _T('\0');
	FileSearch.bCaseSensitive = FALSE;
	SetDlgItemText(hDlg,IDC_SEARCH_STRING, FileSearch.szSearchString);
	CheckRadioButton(hDlg,IDC_SEARCH_BYFIRST,IDC_SEARCH_BYLAST, FileSearch.idSearchBy);
	CheckRadioButton(hDlg,IDC_SEARCH_SELECTEDALB,IDC_SEARCH_FROM_DIRECTORY, FileSearch.idSearchIn);
	CheckDlgButton(hDlg,IDC_SEARCH_CASE,FileSearch.bCaseSensitive);
	ControlEnable(hDlg, IDC_SEARCH_CASE, FileSearch.idSearchBy != IDC_SEARCH_BYNAME); 
	SetDlgItemText(hDlg, IDC_SEARCH_DIRECTORY, FileSearch.szSearchDir);
	ControlEnable(hDlg, IDC_SEARCH_DIRECTORY, FileSearch.idSearchIn == IDC_SEARCH_FROM_DIRECTORY);
	SetDlgItemInt(hDlg, IDC_SEARCH_NUMFOUND, 0, FALSE);
	SetDlgItemInt(hDlg, IDC_SEARCH_NUMSEARCHED, 0, FALSE);
	SetDlgItemText(hDlg,IDC_SEARCH_SEARCHING, _T(""));
	SetSearchButton(hDlg, FALSE);
	ControlEnable(hDlg, IDC_VIEW, FALSE);
	ControlEnable(hDlg, IDC_SEARCH_SEARCH, FileSearch.szSearchString[0] != _T('\0'));
	return(0);
}

//************************************************************************
LOCAL void Search_OnClose(HWND hDlg)
//************************************************************************
{
	GetDlgItemText(hDlg, IDC_SEARCH_STRING, 
			FileSearch.szSearchString, sizeof(FileSearch.szSearchString));
	GetDlgItemText(hDlg, IDC_SEARCH_DIRECTORY, 
			FileSearch.szSearchDir, sizeof(FileSearch.szSearchDir));
	AstralDlgEnd(hDlg, IDC_VIEW);
}

//************************************************************************
LOCAL void Search_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************
{
	int ret;
	
	switch(id)
	{
		case IDC_SEARCH_BYKEY:
		case IDC_SEARCH_BYNAME:
			CheckRadioButton(hDlg, IDC_SEARCH_BYFIRST, 
						IDC_SEARCH_BYLAST, FileSearch.idSearchBy = id);
			ControlEnable(hDlg, IDC_SEARCH_CASE, FileSearch.idSearchBy != IDC_SEARCH_BYNAME); 
		break;
		case IDC_SEARCH_CASE:
			FileSearch.bCaseSensitive = !FileSearch.bCaseSensitive;
			CheckDlgButton(hDlg,IDC_SEARCH_CASE,FileSearch.bCaseSensitive);
		break;
		case IDC_SEARCH_SELECTEDALB:
		case IDC_SEARCH_ALLALB:			
		case IDC_SEARCH_WHOLEDRIVE:
		case IDC_SEARCH_FROM_DIRECTORY:
				CheckRadioButton(hDlg, IDC_SEARCH_SELECTEDALB,
						IDC_SEARCH_FROM_DIRECTORY, FileSearch.idSearchIn=id);
				ControlEnable(hDlg, IDC_SEARCH_DIRECTORY, 
						FileSearch.idSearchIn == IDC_SEARCH_FROM_DIRECTORY);
		break;			
		case IDC_SEARCH_STRING:
			if (codeNotify != EN_CHANGE)
				break;
			ControlEnable(hDlg, IDC_SEARCH_SEARCH, TRUE);
		break;		
		case IDC_SEARCH_SEARCH:
			// fill in structure
			GetDlgItemText(hDlg, IDC_SEARCH_STRING, 
					FileSearch.szSearchString, sizeof(FileSearch.szSearchString));

            CharUpper(FileSearch.szSearchString);

			GetDlgItemText(hDlg, IDC_SEARCH_DIRECTORY, 
					FileSearch.szSearchDir, sizeof(FileSearch.szSearchDir));

            CharUpper(FileSearch.szSearchDir);

			FileSearch.numFound = 0;
			FileSearch.numSearched = 0;
			SetSearchButton(hDlg, TRUE);
			ret = DoSearch(hDlg);
			SetSearchButton(hDlg, FALSE);
			ControlEnable(hDlg, IDC_VIEW, FileSearch.numFound > 0);
			if (ret != IDCANCEL)
			{
				if (FileSearch.numFound > 0)
				{
					if (AstralOKCancel(IDS_SEARCH_DONE)==IDOK)
						Search_OnCommand(hDlg, IDC_VIEW, GetDlgItem(hDlg,IDC_VIEW), 0);
				}
				else
					Message(IDS_SEARCH_FAILED);
				break;
			}
		// fall throught
		case IDCANCEL:
			if (FileSearch.dbfResult.szFile[0] != _T('\0'))
				FileDelete(FileSearch.dbfResult.szFile);
		   	FileSearch.dbfResult.szFile[0] = _T('\0');
		// fall throught
		case IDC_VIEW:
			GetDlgItemText(hDlg, IDC_SEARCH_STRING, 
					FileSearch.szSearchString, sizeof(FileSearch.szSearchString));
			GetDlgItemText(hDlg, IDC_SEARCH_DIRECTORY, 
					FileSearch.szSearchDir, sizeof(FileSearch.szSearchDir));
			AstralDlgEnd(hDlg, IDC_VIEW);
		break;
		default:
		break;
	}  // switch command
}

//************************************************************************/
//************************************************************************/
LOCAL int DoSearch(HWND hDlg)
//************************************************************************/
{
	FNAME szAlbum;
	long i;
	LPDBFLL lpDBFLL;
	BOOL bDup, bKey;
	int result;
	
	// get rid of any old search results
	if (FileSearch.dbfResult.szFile[0] != _T('\0'))
		FileDelete(FileSearch.dbfResult.szFile);
	// get the new album name for search results
	lStrCpyN(szAlbum, Browser.SearchAlbum, sizeof(FNAME)-10);
	i = 0;
	while (FindAlbum(szAlbum))
	{
		i++;
		wsprintf(szAlbum,"%s%lu", (LPTR)Browser.SearchAlbum,i);
	}
	// create the new album
	if (!DBFNewAlbum(szAlbum, &FileSearch.dbfResult, &bDup))
		return(0); //***ERROR-  will not be able to search
	
	// search
	BSRInvalidateDriveIDs();
	AstralCursor(IDC_WAIT);
	DBFReOpen(&FileSearch.dbfResult, YES);
	bKey = (FileSearch.idSearchBy != IDC_SEARCH_BYNAME);
	switch(FileSearch.idSearchIn)
	{
		case IDC_SEARCH_SELECTEDALB:
			SetSearchedTypeText(hDlg, NO);	// files searched
			if(!(lpDBFLL = FindAlbum(FileSearch.szSelectedAlb)))
				goto ExitCancel;//***ERROR		
			result = SearchDBF(&lpDBFLL->dbf, bKey);
			if (result == IDC_VIEW || result == IDC_SEARCH_SEARCH)
				goto ExitOK;
			else
				goto ExitCancel;
		break;
		case IDC_SEARCH_ALLALB:
			SetSearchedTypeText(hDlg, NO);	// files searched
			lpDBFLL = FirstAlb();
			while(lpDBFLL)
			{
				result = SearchDBF(&lpDBFLL->dbf, bKey);
				if (result == IDC_SEARCH_SEARCH)
					goto ExitOK;
				if (result == IDCANCEL)
					goto ExitCancel;
				lpDBFLL = GetNextAlb();
			}
			goto ExitOK;
		break;
		case IDC_SEARCH_FROM_DIRECTORY:
			if (bKey)
			{
				SetSearchedTypeText(hDlg, NO);	// files searched
				result = SearchDirectoryForKey(FileSearch.szSearchDir);
			}
			else
			{
				SetSearchedTypeText(hDlg, YES);	// directories searched
				result = SearchDirectory(FileSearch.szSearchDir);
			}
			if (result == IDC_VIEW || result == IDC_SEARCH_SEARCH)
				goto ExitOK;
			else
				goto ExitCancel;
		break;
		default:
			goto ExitCancel;
		break;
	}
ExitOK:
	AstralCursor(NULL);
	DBFClose(&FileSearch.dbfResult);			 
	return(IDC_VIEW);

ExitCancel:
	DBFClose(&FileSearch.dbfResult);			 
	FileDelete(FileSearch.dbfResult.szFile);
	FileSearch.dbfResult.szFile[0] = _T('\0');
	FileSearch.dbfResult.info.szName[0] = _T('\0');
	AstralCursor(NULL);
	return(IDCANCEL);
}

//************************************************************************/
// Searches all files in a dbf for either a keyword or file name.
//	Returns	IDC_VIEW, IDCANCEL, or IDC_SEARCH_SEARCH
//************************************************************************/
LOCAL int SearchDBF(LPDBF lpDBF, BOOL bByKey)
//************************************************************************/
{
	ENTRY entry;
	BOOL bMatch;
	int whole, part;
	int ret;
	
	// setup satatus
	SetDlgItemIntNow(FileSearch.hSearch, IDC_SEARCH_NUMFOUND, FileSearch.numFound, FALSE);
	FileSearch.numSearched++;
	SetDlgItemIntNow(FileSearch.hSearch, IDC_SEARCH_NUMSEARCHED, FileSearch.numSearched, FALSE);
	SetDlgItemTextNow(FileSearch.hSearch, IDC_SEARCH_SEARCHING, lpDBF->info.szName);
	
	AstralCursor(IDC_WAIT);
	// Setup dbf
	DBFReOpen(lpDBF, NO);
	whole = lpDBF->info.numEntry;
	part = 0;
	DBFGetEntryNum(lpDBF, -1, &entry);
	// search all entries
	while(DBFGetNextEntry(lpDBF, &entry))
	{
		ret = SearchCursor();
		if (ret != IDC_VIEW)
			goto Exit;
		if (bByKey)
			bMatch = CheckKey(&entry, 
						FileSearch.szSearchString, 
						FileSearch.bCaseSensitive);
		else										   
			bMatch = CheckName(entry.File.szFile, FileSearch.szSearchString);
		SetDlgItemIntNow(FileSearch.hSearch, IDC_SEARCH_NUMSEARCHED, ++FileSearch.numSearched, FALSE);
		if (bMatch)
		{
			ret = SearchCursor();
			if (ret != IDC_VIEW)
				goto Exit;
			if (DBFPutEntry(&FileSearch.dbfResult, &entry, NULL))
				SetDlgItemIntNow(FileSearch.hSearch, IDC_SEARCH_NUMFOUND, FileSearch.dbfResult.info.numEntry, FALSE);
			else	
			if (AstralOKCancel(IDS_SEARCH_EWRITEDBF,(LPTR)entry.File.szFile)!=IDOK)
			{
				ret = IDC_SEARCH_SEARCH;
				goto Exit;
			}
		}
	}
	FileSearch.numFound = FileSearch.dbfResult.info.numEntry;
	ret = IDC_VIEW;
Exit:
	DBFClose(lpDBF);
	AstralCursor(NULL);
	return(ret);
}

//************************************************************************/
// 	searches for the keyword(s) in the description for the entry
//	Returns true fro a match.
//	Note: USED non cached thumbnails
//************************************************************************/
LOCAL BOOL CheckKey(LPENTRY lpEntry, LPTSTR lpKey, BOOL bCase)
//************************************************************************/
{
	LPTHMNAIL lpThm;
	LPDESC lpDesc;
	TCHAR szSpec[sizeof(FNAME)+2];
	BOOL ret;
	
	lpThm = (LPTHMNAIL)Alloc(sizeof(THMNAIL));
	if (!lpThm)
		return(FALSE); //***ERROR
	if (!(lpThm = THMOpen(lpThm, lpEntry)))
		return(FALSE);
	ret = FALSE;
	if (lpDesc = THMGetDescription(lpThm))
	{
		lstrcpy(szSpec, _T("*"));
		lstrcat(szSpec, lpKey);
		lstrcat(szSpec, _T("*"));
		ret = MatchWildcard((LPTSTR)lpDesc, (LPTSTR)szSpec, bCase ? 0:MW_CASE);
	}
	THMClose(lpThm);
	return(ret);	
}

//************************************************************************/
// 	checks the file name against the spec.
//	Returns true for a match.
//************************************************************************/
LOCAL BOOL CheckName(LPTSTR lpFile, LPTSTR lpSpec)
//************************************************************************/
{
	int i;
	FNAME szSpec;
	
	lstrcpy(szSpec, lpSpec);
	StripTrailer(szSpec, _T(' '));
	i = MBStrlen(szSpec);
	if( i > 1 )
	{
		WORD wFirst, wNext;

		LPTSTR psz = MyCharNextN( szSpec, i-1 );
		if( IsDBCSLeadByte(( BYTE )*psz ))
			wNext = *(( LPWORD )psz );
		else
			wNext = *psz;

		psz = MyCharPrev( szSpec, psz );
		if( IsDBCSLeadByte(( BYTE )*psz ))
			wFirst = *(( LPWORD )psz );
		else
			wFirst = *psz;
		
		if( wFirst == _T('.') && wNext == _T('*') )
			*psz = _T('\0'); 
	}	
	i = MatchWildcard((LPTSTR)filename((LPTSTR)lpFile), (LPTSTR)lpSpec, MW_CASE);
	return(i);	
}


//************************************************************************/
// Searches all files in a directory and all subdirectories for a
//		 file name.
//	Returns	IDC_VIEW, IDCANCEL, or IDC_SEARCH_SEARCH
//************************************************************************/
LOCAL int SearchDirectory(LPTSTR lpPath)
//************************************************************************/
{
	FINDDATA afile;
	FILELOC FileFound;
	int ret;

#ifndef WIN32
	// are we too deep?
	if ( stackavail() < MIN_STACK_SIZE )
	{
		MessageBeep(-1);		// ***ERROR -- real error
		return(IDC_VIEW);
	}
#endif
	
	lstrcpy(FileFound.szFile, lpPath);
	FixPath(FileFound.szFile);
	
	AstralCursor(IDC_WAIT);
	// set important data
	SetDlgItemIntNow(FileSearch.hSearch, IDC_SEARCH_NUMFOUND, FileSearch.numFound, FALSE);
	FileSearch.numSearched++;
	SetDlgItemIntNow(FileSearch.hSearch, IDC_SEARCH_NUMSEARCHED, FileSearch.numSearched, FALSE);
	SetDlgItemTextNow(FileSearch.hSearch, IDC_SEARCH_SEARCHING, FileFound.szFile);
	
	// check for abort
	ret = SearchCursor();
	if (ret != IDC_VIEW)
	{
		goto Exit;
	}
	
	// search for files first
	// check length ... just in case
	if ((MAX_FNAME_LEN-13) < lstrlen(FileFound.szFile))
	{
		MessageBeep(-1);		// ***ERROR -- real error
		goto Exit;
	}
	lstrcat(FileFound.szFile, FileSearch.szSearchString);
	ret = Dos_FindFirst(FileFound.szFile, ~ATTRIB_SUBDIR, &afile);
	while(ret)
	{
		// found one, now put in dbf
		ret = SearchCursor();
		if (ret != IDC_VIEW)
			goto Exit;
		lstrcpy(FileFound.szFile, lpPath);
		FixPath(FileFound.szFile);
		if (MAX_FNAME_LEN < lstrlen(FileFound.szFile) + lstrlen(afile.szName))
		{
			MessageBeep(-1);		// ***ERROR -- real error
			goto Exit;
		}
		lstrcat(FileFound.szFile, afile.szName);
		GetDriveVolume(FileFound.szFile, &FileFound.volume, NO);
		if (AddToAlbum(&FileSearch.dbfResult, &FileFound, MAYBE))
			SetDlgItemIntNow(FileSearch.hSearch, IDC_SEARCH_NUMFOUND, ++(FileSearch.numFound), FALSE);
		else	
			if (AstralOKCancel(IDS_SEARCH_EWRITEDBF,(LPTSTR)FileFound.szFile)!=IDOK)
			{
				ret = IDC_SEARCH_SEARCH;
				goto Exit;
			}
		ret = Dos_FindNext(&afile);
	}
	// search all subdirectories
	lstrcpy(FileFound.szFile, lpPath);
	FixPath(FileFound.szFile);
	lstrcat(FileFound.szFile, _T("*.*"));
	ret = Dos_FindFirst(FileFound.szFile, ATTRIB_SUBDIR, &afile);
	while(ret)
	{
		WORD wChar;
		if( IsDBCSLeadByte(( BYTE )*afile.szName ))
			wChar = *(( LPWORD )afile.szName );
		else
			wChar = *afile.szName;

		if ((afile.dwAttrib & ATTRIB_SUBDIR) && wChar != _T('.'))
		{
			// found one, now search it
			lstrcpy(FileFound.szFile, lpPath);
			FixPath(FileFound.szFile);
			if(( MAX_FNAME_LEN - ( 13 * sizeof( TCHAR ))) < lstrlen( FileFound.szFile ) + lstrlen( afile.szName ))
			{
				MessageBeep(-1);		// ***ERROR -- real error
				ret = IDC_VIEW;
				goto Exit;
			}
			lstrcat(FileFound.szFile, afile.szName);
			ret = SearchDirectory(FileFound.szFile);
			if(ret != IDC_VIEW)
				goto Exit; // was aborted		
		}
		ret = Dos_FindNext(&afile);
	}
	ret = IDC_VIEW;
Exit:
	AstralCursor(NULL);
	return(ret);
}


//************************************************************************/
// Searches all files in a directory and all subdirectories for a file
//		with a thumbnail with the keyword (FileSearch.szSearchString).
//	Returns	IDC_VIEW, IDCANCEL, or IDC_SEARCH_SEARCH
//************************************************************************/
LOCAL int SearchDirectoryForKey(LPTSTR lpPath)
//************************************************************************/
{
	FINDDATA afile;
	FILELOC Path;
	DBFS dbf;
	int ret;

#ifndef WIN32	
	// are we too deep?
	if ( stackavail() < MIN_STACK_SIZE )
	{
		MessageBeep(-1);		// ***ERROR -- put real error here
		return(IDC_VIEW);
	}
#endif
	
	GetDriveVolume(lpPath, &Path.volume, YES);
	lstrcpy(Path.szFile, lpPath);
	FixPath(Path.szFile);
	
	AstralCursor(IDC_WAIT);
	// set important data
	SetDlgItemIntNow(FileSearch.hSearch, IDC_SEARCH_NUMFOUND, FileSearch.numFound, FALSE);
	FileSearch.numSearched++;
	SetDlgItemIntNow(FileSearch.hSearch, IDC_SEARCH_NUMSEARCHED, FileSearch.numSearched, FALSE);

	SetDlgItemTextNow(FileSearch.hSearch, IDC_SEARCH_SEARCHING, Path.szFile);
	
	// check for abort
	ret = SearchCursor();
	if (ret != IDC_VIEW)
		goto Exit;
	
	// Does the directory have a dbf to search?
	if (DBFOpenDirectory(&Path, &dbf, NO))
	{
		ret = SearchDBF(&dbf, YES);
		if (ret != IDC_VIEW)
			goto Exit;
	}
	// search all subdirectories
	lstrcpy(Path.szFile, lpPath);
	FixPath(Path.szFile);
	if ((MAX_FNAME_LEN-13) < lstrlen(Path.szFile))
	{
		MessageBeep(-1);		// ***ERROR -- real error
		goto Exit;
	}
	lstrcat(Path.szFile, _T("*.*"));
	ret = Dos_FindFirst(Path.szFile, ATTRIB_SUBDIR, &afile);
	while(ret)
	{
		WORD wChar;
		if( IsDBCSLeadByte(( BYTE )*afile.szName ))
			wChar = *(( LPWORD )afile.szName );
		else
			wChar = *afile.szName;

		if ((afile.dwAttrib & ATTRIB_SUBDIR) && wChar != _T('.') )
		{
			// found one, nowsearch it
			lstrcpy(Path.szFile, lpPath);
			FixPath(Path.szFile);
			lstrcat(Path.szFile, afile.szName);
			ret = SearchDirectoryForKey(Path.szFile);
			if(ret != IDC_VIEW)
				goto Exit;		// was aborted		
		}
		ret = Dos_FindNext(&afile);
	}
	ret = IDC_VIEW;
Exit:
	AstralCursor(NULL);
	return(ret);
}

//************************************************************************/
// 	Puts up a clock cursor.
// 	Returns IDC_VIEW, IDCANCES, or IDC_SEARCH_SEARCH
//************************************************************************/
LOCAL int SearchCursor(void)
//************************************************************************/
{
	HWND hWnd;
	MSG msg;
	RECT rect;
	long cancel;
	
	cancel = (CANCEL && !ALT && !CONTROL);
	if (cancel)	
	{
		while( PeekMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST,
			PM_REMOVE|PM_NOYIELD) );
		return(IDCANCEL);
	}
  	while (PeekMessage(&msg,FileSearch.hSearch, WM_MOUSEFIRST, WM_MOUSELAST, 
						PM_REMOVE | PM_NOYIELD))
	{
		if (msg.message == WM_LBUTTONDOWN)
		{
			hWnd = GetDlgItem(FileSearch.hSearch, IDC_SEARCH_SEARCH);
			GetWindowRect(hWnd, &rect);
			if (PtInRect(&rect, msg.pt))
				return(IDC_SEARCH_SEARCH);
			hWnd = GetDlgItem(FileSearch.hSearch, IDCANCEL);
			GetWindowRect(hWnd, &rect);
			if (PtInRect(&rect, msg.pt))
				return(IDCANCEL);
		}
	}
	return(IDC_VIEW);
}		
					
//************************************************************************
//	Sets the text of the search button based on bSearching
//************************************************************************
LOCAL void	SetSearchButton(HWND hDlg, BOOL bSearching)
//************************************************************************
{
	LPTSTR lpString;
	int id;
	
	id = (bSearching ? IDS_STOP_BUTTON:IDS_SEARCH_BUTTON);
	if (AstralStr(id, &lpString))
		SetDlgItemTextNow(hDlg, IDC_SEARCH_SEARCH, lpString);
}

//************************************************************************
//	sets the IDC_SEARCH_NUMSEARCHED_TEXT 
//************************************************************************
LOCAL void SetSearchedTypeText(HWND hDlg, BOOL bDir)
//************************************************************************
{
	LPTR lpString;
	int id;
	
	if (bDir)
		id = IDS_SEARCH_DIRS;
	else
		id = IDS_SEARCH_FILES;
	if (AstralStr(id,(LPTSTR *)&lpString))
		SetDlgItemTextNow(hDlg, IDC_SEARCH_NUMSEARCHED_TEXT, (LPTSTR)lpString);
	else				   
		SetDlgItemTextNow(hDlg, IDC_SEARCH_NUMSEARCHED_TEXT, (LPTSTR)_T("Searched"));
}
