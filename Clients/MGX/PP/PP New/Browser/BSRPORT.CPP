// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// handles dbf & thumbnail import/ export for Browser 

#include "pp.h"
#include <malloc.h>
#include "id.h"
#include "bsrtypes.h"
#include "bsrutil.h"
#include "bsrport.h"
#include "bsrdlgs.h"
#include "handlebs.h"
#include "browser.h"
#include "bsrid.h"
#include "bsritems.h"
#include "cthmmgr.h"
#include "dbfmgr.h"
#include "thmmgr.h"

#define MIN_STACK_SIZE 1024
extern LPBSRSTATE lpBsr;

// structure for commony used dada in dialogs
typedef struct _portdata {
	FNAME szDir;
	BOOL bCopyThumbs;
	BOOL bSubdirs;
	BOOL bCopyFiles;
	BOOL bOldCopyThumbs;
	LPDBFLL lpDBFLL;
} PORTDATA, FAR *LPPORTDATA;

typedef struct _movedata {
	int idMoveType;
	BOOL bDeleteOriginal;
} MOVEDATA, FAR *LPMOVEDATA;


//********************Static functioons****************************************
LOCAL BOOL ImportDir(LPTSTR lpDir,BOOL bSubdirs);  
LOCAL BOOL ExportDir(LPTSTR lpDir,BOOL bSubdirs, BOOL bCopyThumbs);  
LOCAL BOOL	delete_all(LPTSTR lpPat);
LOCAL BOOL MoveSelectedThumbs(LPTSTR lpDir, BOOL bDeleteOriginal);
LOCAL BOOL ExportAlb(LPDBF lpSrcDBF, LPTSTR lpDstDir, BOOL bCopyFiles, BOOL bCopyThumbs);
LOCAL BOOL ImportAlb(HWND hDlg, LPTSTR lpSrcDir, LPTSTR lpFileDst, BOOL bCopyFiles, BOOL bCopyThumbs);
LOCAL BOOL DoImportAlb(HWND hDlg, LPDBF lpDBF, LPTSTR lpFileDst, BOOL bCopyFiles, BOOL bCopyThumbs);
 
 
LOCAL BOOL DirExport_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void DirExport_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);
 
//************************************************************************
BOOL WINPROC DlgDirExportProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, DirExport_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, DirExport_OnCommand);  

		HANDLE_DLGMSG(hDlg, WM_DESTROY, DWL_USER_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_CREATE, DWL_USER_OnCreate);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_DIREXPORT);
	}
}

//************************************************************************/
LOCAL BOOL DirExport_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************/
{
	LPPORTDATA lpData;

	if (!(lpData = (LPPORTDATA)Alloc(sizeof(PORTDATA))))
		return(FALSE);
	SetWindowLong(hDlg, DWL_USER, (long)lpData);

	CenterPopup(hDlg);
	lpData->bCopyThumbs = FALSE;
	lpData->bSubdirs = FALSE;
	CheckDlgButton(hDlg,IDC_BSR_PORTSUBDIR,lpData->bSubdirs);
	CheckDlgButton(hDlg,IDC_BSR_PORTCOPYTHMS,lpData->bCopyThumbs);
	CURRENTDIR(lpData->szDir, sizeof(lpData->szDir));
	SetDlgItemText(hDlg, IDC_BSR_PORTDIR, lpData->szDir);
	return(0);
}
		

//************************************************************************/
LOCAL void DirExport_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************/
{
	LPPORTDATA lpData;
	WORD wNext;
	LPTSTR lp;

	if (!(lpData = (LPPORTDATA)GetParentBsrState(hDlg, TRUE)))
		return;	

	switch(id)
	{
		case IDC_BSR_PORTSUBDIR:
			lpData->bSubdirs = !lpData->bSubdirs;
			CheckDlgButton(hDlg,IDC_BSR_PORTSUBDIR,lpData->bSubdirs);
		break;	
		case IDC_BSR_PORTCOPYTHMS:
			lpData->bCopyThumbs = !lpData->bCopyThumbs;
			CheckDlgButton(hDlg,IDC_BSR_PORTCOPYTHMS,lpData->bCopyThumbs);
		break;	
		case IDCANCEL:
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDOK:
			FixPath(lpData->szDir);

			lp = MyCharNextN( lpData->szDir, 1 );

			if( IsDBCSLeadByte(( BYTE )*lp ))
				wNext = *(( LPWORD )lp );
			else
				wNext = *lp;

			if( *lpData->szDir && wNext == _T(':'))
			{
				AstralCursor(IDC_WAIT);
				ExportDir(lpData->szDir,lpData->bSubdirs,lpData->bCopyThumbs);  
				AstralCursor(NULL);
				AstralDlgEnd(hDlg, id);
			}
		break;
		default:
		break;
	}  // switch command
}
	
	
LOCAL BOOL DirImport_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void DirImport_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

//************************************************************************
BOOL WINPROC DlgDirImportProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, DirImport_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, DirImport_OnCommand);  

		HANDLE_DLGMSG(hDlg, WM_DESTROY, DWL_USER_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_CREATE, DWL_USER_OnCreate);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_DIRIMPORT);
	}
}

//************************************************************************/
LOCAL BOOL DirImport_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************/
{
	LPPORTDATA lpData;
	
	if (!(lpData = (LPPORTDATA)Alloc(sizeof(PORTDATA))))
		return(FALSE);
	SetWindowLong(hDlg, DWL_USER, (long)lpData);

	CenterPopup(hDlg);
	lpData->bSubdirs = FALSE;
	CheckDlgButton(hDlg,IDC_BSR_PORTSUBDIR,lpData->bSubdirs);
	CURRENTDIR(lpData->szDir, sizeof(lpData->szDir));
	SetDlgItemText(hDlg, IDC_BSR_PORTDIR, lpData->szDir);

	return(0);
}

//************************************************************************/
LOCAL void DirImport_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************/
{
	LPPORTDATA lpData;
	WORD wNext;
	LPTSTR lp;
	
	if (!(lpData = (LPPORTDATA)GetParentBsrState(hDlg, TRUE)))
		return;	
		
	switch(id)
	{
		case IDC_BSR_PORTSUBDIR:
			lpData->bSubdirs = !lpData->bSubdirs;
			CheckDlgButton(hDlg,IDC_BSR_PORTSUBDIR,lpData->bSubdirs);
		break;	
		case IDCANCEL:
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDOK:
			FixPath(lpData->szDir);

			lp = MyCharNextN( lpData->szDir, 1 );

			if( IsDBCSLeadByte(( BYTE )*lp ))
				wNext = *(( LPWORD )lp );
			else
				wNext = *lp;

			if (*lpData->szDir && wNext == _T(':'))
			{
				AstralCursor(IDC_WAIT);
				ImportDir(lpData->szDir,lpData->bSubdirs);  
				AstralCursor(NULL);
				AstralDlgEnd(hDlg, id);
			}
		default:
		break;
	}  // switch command
}

//************************************************************************
//************************************************************************
LOCAL BOOL ImportDir(LPTSTR lpDir,BOOL bSubdirs)
//************************************************************************
{
	FNAME szSrcDBF, szDstDBF;
	FILELOC File;
	DBFS dbf, DupDbf;
	FINDDATA afile;
	int ret;
	WORD wSrcDrive, wDstDrive;
	ENTRY entry;
    LPVOID lp;
	
#ifndef WIN32	  // CALFIX
	if ( stackavail() < MIN_STACK_SIZE )
	{
		MessageBeep(-1);		// ***ERROR -- real error
		return(TRUE);
	}
#endif
	// Check directory for valid dbf's
	lstrcpy(szSrcDBF, (LPTSTR)lpDir);
	lstrcat(szSrcDBF, _T("*"));
	lstrcat(szSrcDBF, DBFILE_EXT);
	if (!Dos_FindFirst(szSrcDBF, ATTRIB_NORMAL | ATTRIB_READONLY, &afile))
		goto DoSubdirs;

	lstrcpy(szSrcDBF, (LPTSTR)lpDir);
	lstrcat(szSrcDBF, afile.szName);
	if (!DBFOpenByFile(szSrcDBF, &dbf, NO))
		goto DoSubdirs; //***ERROR

    if( IsDBCSLeadByte(( BYTE )*lpDir )) 
        wDstDrive = MBToUpperW( lpDir );
    else
        wDstDrive = MBToUpper( lpDir );

    lp = dbf.info.szName;

    if( IsDBCSLeadByte( *(( LPBYTE )lp ))) 
        wSrcDrive = MBToUpperW(( LPTSTR )lp );
    else
        wSrcDrive = MBToUpper(( LPTSTR )lp );

	// check for existing dbf's	for the directory
	lstrcpy(File.szFile, dbf.info.szName);
	File.volume = dbf.info.volume;

    lp = File.szFile;
    if( IsDBCSLeadByte(( BYTE )*lpDir )) 
        *(( LPWORD )lp ) = wDstDrive;
    else
        *(( LPBYTE )lp ) = LOBYTE( wDstDrive );

	DBFClose(&dbf);
	if (DBFOpenDirectory(&File, &DupDbf, NO))
	{
		// there is a duplicate dbf
		if (AstralOKCancel(IDS_DIRIMPORT_DUPLICATE, (LPTR)File.szFile) != IDOK)
		{
			DBFClose(&DupDbf);
			goto DoSubdirs;
		}
		// delete it
		lstrcpy(File.szFile, DupDbf.szFile);
		DBFClose(&DupDbf);
		FileDelete(File.szFile);
	}
	//	get new file name
	lstrcpy(File.szFile, dbf.info.szName);
	File.volume = dbf.info.volume;

    lp = File.szFile;
    if( IsDBCSLeadByte(( BYTE )*lpDir )) 
        *(( LPWORD )lp ) = wDstDrive;
    else
        *(( LPBYTE )lp ) = LOBYTE( wDstDrive );

	if (!GetNewDBFName(szDstDBF, Browser.DBFInfoDir, File.szFile,ID_DIRECTORY))
		goto DoSubdirs;		// ***ERROR if can not get temp or copy

	// copy it
	if (!CopyFile(szSrcDBF,szDstDBF, NO))
		goto DoSubdirs;		// ***ERROR if can not get temp or copy

	if (wDstDrive == wSrcDrive)
		goto DoSubdirs; 	

	// change references to wSrcDrive into wDstDrive.
	if (!DBFOpenByFile(szDstDBF,&dbf,YES))
		goto DoSubdirs; 	//****ERROR

    lp = dbf.info.szName;

    if( IsDBCSLeadByte(( BYTE )*lpDir )) 
        *(( LPWORD )lp ) = wDstDrive;
    else
        *(( LPBYTE )lp ) = LOBYTE( wDstDrive );

	if (!DBFPutInfo(&dbf))
	{
		// Unrecovrrable 
		DBFClose(&dbf);
		goto DoSubdirs; 	//***ERROR
	}

	DBFGetEntryNum(&dbf, -1, &entry);
	while(DBFGetNextEntry(&dbf, &entry))
	{
        lp = entry.Thumb.szFile;
		// copy thumbs not already on the drive
        if( IsDBCSLeadByte( *(( LPBYTE )lp )))
		{
		    if( MBToUpperW(( LPTSTR )lp ) == wSrcDrive )
			    *(( LPWORD )lp ) = wDstDrive;
		}
        else
        {
		    if( MBToUpper(( LPTSTR )lp ) == wSrcDrive )
			    *(( LPBYTE )lp ) = LOBYTE( wDstDrive );
        }

        lp = entry.File.szFile;
		// copy thumbs not already on the drive
        if( IsDBCSLeadByte( *(( LPBYTE )lp )))
        {
		    if( MBToUpperW(( LPTSTR )lp ) == wSrcDrive )
			    *(( LPWORD )lp ) = wDstDrive;
        }
        else
        {
		    if( MBToUpper(( LPTSTR )lp ) == wSrcDrive )
			    *(( LPBYTE )lp ) = LOBYTE( wDstDrive );
        }

		if( !DBFChangeCurrentEntry( &dbf, &entry ))
		{
			// Unrecovrrable 
			DBFClose( &dbf );
			goto DoSubdirs;         //***ERROR
		}
	}
	DBFClose(&dbf);

DoSubdirs:
	if (!bSubdirs)
		return(TRUE);
	lstrcpy(szSrcDBF, (LPTSTR)lpDir);
	lstrcat(szSrcDBF, _T("*.*"));
	ret = Dos_FindFirst(szSrcDBF, ATTRIB_SUBDIR, &afile);
	while(ret)
	{
		if ((afile.dwAttrib & ATTRIB_SUBDIR) && afile.szName[0] != _T('.'))
		{
			// found one, nowsearch it
			lstrcpy(szSrcDBF, (LPTSTR)lpDir);
			lstrcat(szSrcDBF, afile.szName);
			FixPath(szSrcDBF);
			ret = ImportDir(szSrcDBF, YES);
			if(!ret)
				return(FALSE);
		}
		ret = Dos_FindNext(&afile);
	}
		
	return(TRUE);
}

//************************************************************************
//	
//************************************************************************
LOCAL BOOL ExportDir(LPTSTR lpDir,BOOL bSubdirs, BOOL bCopyThumbs)
//***********************************************************************
{
	FILELOC Src, Dst;
	FNAME Temp;
	DBFS dbf;
	ENTRY entry;
	LPDBF lpdbf = NULL;
	FINDDATA afile;
	int ret;
	WORD wDrive;
	
#ifndef WIN32	// CALFIX
	if ( stackavail() < MIN_STACK_SIZE )
	{
		MessageBeep(-1);		// ***ERROR -- real error
		return(TRUE);
	}
#endif
	
	// Check directory for valid dbf's
	lstrcpy(Dst.szFile, lpDir);
	GetDriveVolume(lpDir, &Dst.volume, YES);
	
	// get a dbf to export
	if (!(lpdbf = DBFOpenDirectory(&Dst, &dbf, NO)))
		goto DoSubdirs;

	lstrcpy(Src.szFile, dbf.szFile); 
	DBFClose(&dbf);

	// check for exising directory for dbf files && thumbs(if copy thumbs)
	lstrcpy(Temp, lpDir);
	lstrcat(Temp, _T("*"));
	lstrcat(Temp, DBFILE_EXT);

	ret = Dos_FindFirst(Temp, ATTRIB_NORMAL, &afile);
	if (ret)
	{
		if (AstralOKCancel(IDS_DIREXPORT_DUPLICATE, (LPTR)lpDir) != IDOK)
			return(FALSE);
		// delete dbf's
		delete_all(Temp);
	}
	if (bCopyThumbs)
	{
		lstrcpy(Temp, lpDir);
		lstrcat(Temp, _T("*"));
		lstrcat(Temp, THUMBNAIL_EXT);

		ret = Dos_FindFirst(Temp, ATTRIB_NORMAL, &afile);
		if (ret)
		{
			ret = AstralConfirm(IDS_DIREXPORT_DUPTHUMBS, (LPTR)lpDir);
			if (ret == IDCANCEL)
				return(FALSE);
			else 
			if (ret == IDYES)
				delete_all(Temp);
		}
	}
	// copy the dbf	
	if (GetNewDBFName(Dst.szFile,Dst.szFile,lpDir,ID_DIRECTORY))
		if (!CopyFile(Src.szFile, Dst.szFile, NO))
			goto DoSubdirs; //***ERROR

	if (!bCopyThumbs)
		goto DoSubdirs; 

	// copy thumbs (if needed) 
	// open the dest dbf
	if (!DBFOpenByFile(Dst.szFile,&dbf,YES))
		goto DoSubdirs; //****ERROR

    if( IsDBCSLeadByte(( BYTE )*( Dst.szFile ))) 
        wDrive = MBToUpperW( Dst.szFile );
    else
        wDrive = MBToUpper( Dst.szFile );

	Src = Dst;  // includes volume
	DBFGetEntryNum(&dbf, -1, &entry);
	while(DBFGetNextEntry(&dbf, &entry))
	{
		// copy thumbs not already on the drive
        LPVOID lp = entry.Thumb.szFile;

        if(( IsDBCSLeadByte( *(( LPBYTE )lp )) && MBToUpperW(( LPTSTR )lp ) != wDrive ) ||
		     MBToUpper(( LPTSTR )lp ) != wDrive )
		{
			//copy it
			stripfile( Src.szFile );
			if( !GetTempFilename( Src.szFile, THUMBNAIL_EXT ))
				continue; 	    //***ERROR

			if( !CopyFile( entry.Thumb.szFile, Src.szFile, NO ))
				continue; 	    //***ERROR

			// update the dbf entry
			entry.Thumb = Src;
			if( !DBFChangeCurrentEntry( &dbf, &entry ))
			{
				// Unrecovrrable 
				DBFClose( &dbf );
				goto DoSubdirs; 
			}
		}
	}
	DBFClose(&dbf);
DoSubdirs:
	if (!bSubdirs)
		return(TRUE);

	lstrcpy(Src.szFile, lpDir);
	lstrcat(Src.szFile, _T("*.*"));
	ret = Dos_FindFirst(Src.szFile, ATTRIB_SUBDIR, &afile);
	while(ret)
	{
		WORD wChar;
		if( IsDBCSLeadByte(( BYTE )*afile.szName ))
			wChar = *(( LPWORD )afile.szName );
		else
			wChar = *afile.szName;

		if(( afile.dwAttrib & ATTRIB_SUBDIR) && wChar != _T('.') )
		{
			// found one, nowsearch it
			lstrcpy(Src.szFile, lpDir);
			lstrcat(Src.szFile, afile.szName);
			FixPath(Src.szFile);

			ret = ExportDir(Src.szFile, YES, bCopyThumbs);
			if(!ret)
				return(FALSE);
		}
		ret = Dos_FindNext(&afile);
	}
	return(TRUE);
}

//************************************************************************
//************************************************************************
LOCAL BOOL	delete_all(LPTSTR lpPat)
//************************************************************************
{
	int ret;
	FNAME szFile;
	FINDDATA afile;
	
	lstrcpy(szFile, lpPat);
	ret = Dos_FindFirst(szFile, ATTRIB_NORMAL, &afile);
	while (ret)
	{
		stripfile(szFile);	
		lstrcat(szFile, afile.szName);
		FileDelete(szFile);
		ret = Dos_FindNext(&afile);
	}
	return(TRUE);
}

LOCAL BOOL ThumbMove_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void ThumbMove_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

//************************************************************************
//	MUST CALL OpenFileList BEFORE OPENING THIS DLG
//************************************************************************
BOOL WINPROC DlgThumbMoveProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, ThumbMove_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, ThumbMove_OnCommand);  

		HANDLE_DLGMSG(hDlg, WM_DESTROY, DWL_USER_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_CREATE, DWL_USER_OnCreate);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_THMMOVE);
	}
}

//************************************************************************/
LOCAL BOOL ThumbMove_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************/
{
	LPMOVEDATA lpData;
	FNAME szDir;
	
	if (!(lpData = (LPMOVEDATA)Alloc(sizeof(MOVEDATA))))
		return(FALSE);
	SetWindowLong(hDlg, DWL_USER, (long)lpData);

	CenterPopup(hDlg);
	ControlEnable(hDlg, IDC_BSR_THMREMOTE, DIRMODE);
	CheckDlgButton(hDlg,IDC_BSR_DELORIGINAL,lpData->bDeleteOriginal=TRUE);
	CheckRadioButton(hDlg,IDC_BSR_THMLOCAL,IDC_BSR_THMOTHER, lpData->idMoveType = IDC_BSR_THMOTHER);
	SetDlgItemText(hDlg, IDC_BSR_LOCALDIR, Browser.ThumbnailDir);

	LPTSTR lp = Browser.ThumbnailDir;
	WORD wChar;

	if( IsDBCSLeadByte(( BYTE )*lp ))
		wChar = MBToLowerW( lp );
	else	
		wChar = MBToLower( lp );

	if (CURRENTDRIVE + _T('a') == wChar )
	{
		lstrcpy(szDir, Browser.ThumbnailDir);
		if (DIRMODE)
			ControlEnable(hDlg, IDC_BSR_THMREMOTE, FALSE);
	}
	else
	{
		lstrcpy(szDir, _T("A:"));
		*szDir += CURRENTDRIVE;
   		lstrcat(szDir, Browser.DriveThumbnailDir);
		if (DIRMODE)
			SetDlgItemText(hDlg, IDC_BSR_REMOTEDIR, szDir);
	}
	SetDlgItemText(hDlg, IDC_BSR_PORTDIR, szDir);
	ControlEnable(hDlg, IDC_BSR_PORTDIR, lpData->idMoveType == IDC_BSR_THMOTHER);
	
	return(0);
}


//************************************************************************/
LOCAL void ThumbMove_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************/
{
	LPMOVEDATA lpData;
	FNAME szDir;
	WORD wChar;
	LPTSTR lp;
	
	if (!(lpData = (LPMOVEDATA)GetParentBsrState(hDlg, TRUE)))
		return;	
	switch(id)
	{
		case IDC_BSR_THMLOCAL:
		case IDC_BSR_THMREMOTE:	
		case IDC_BSR_THMOTHER:	
			CheckRadioButton(hDlg,IDC_BSR_THMLOCAL,
					IDC_BSR_THMOTHER, lpData->idMoveType = id);
			ControlEnable(hDlg, IDC_BSR_PORTDIR, lpData->idMoveType == IDC_BSR_THMOTHER);
		break;
		case IDC_BSR_DELORIGINAL:
			lpData->bDeleteOriginal=!lpData->bDeleteOriginal;
			CheckDlgButton(hDlg,IDC_BSR_DELORIGINAL,lpData->bDeleteOriginal);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, TRUE);
		break;
		case IDOK:
			// setup destination
			switch (lpData->idMoveType)
			{
				case IDC_BSR_THMLOCAL:
					lstrcpy(szDir, Browser.ThumbnailDir);
				break;
				case IDC_BSR_THMREMOTE:	
					lstrcpy(szDir, _T("A:"));
					*szDir += CURRENTDRIVE;
   					lstrcat(szDir, Browser.DriveThumbnailDir);
				break;
				case IDC_BSR_THMOTHER:	
					GetDlgItemText(hDlg, IDC_BSR_PORTDIR, szDir, sizeof(FNAME));
				break;
				default:
					AstralDlgEnd(hDlg, IDCANCEL);
				break;
			}
			MakeFullPath(szDir);
			if (!IsValidFileName(szDir))
			{
				Message(IDS_INVALIDNAME, (LPTR)szDir);
				break;
			}
			if (!CheckDir(szDir))
			{
				Message(IDS_EINVALIDDIR, (LPTR)szDir);
				break;
			}
			FixPath(szDir);

			lp = MyCharNextN( szDir, 1 );
			if( IsDBCSLeadByte(( BYTE )*lp ))
				wChar = *(( LPWORD )lp );
			else
				wChar = *lp;

			if( *szDir && wChar == _T(':'))
			{
				AstralCursor(IDC_WAIT);
				MoveSelectedThumbs(szDir, lpData->bDeleteOriginal);  
				AstralCursor(NULL);
				AstralDlgEnd(hDlg, id);
			}
		break;
		default:
		break;
	}  // switch command
}
	
//************************************************************************
//************************************************************************
LOCAL BOOL MoveSelectedThumbs(LPTSTR lpDir, BOOL bDeleteOriginal)
//************************************************************************
{
	FILELOC File;
	FILELOC Dst;
	VOLUMEID volume;
	ENTRY entry;
	
	BSRInvalidateDriveIDs();
	GetDriveVolume(lpDir, &volume, YES);
	if (!GetFirstFile(&File))
		return(TRUE);
	do 
	{
		// find the thumb
		if (!FindEntry(&File, &entry, YES, ALBMODE))
			continue;
		// check for thumb file
		if (!VolumeCheck(&entry.Thumb, NULL))
			continue;
		// Get dest file name
		Dst.volume = volume;
		lstrcpy(Dst.szFile, lpDir);
		if (!GetTempFilename(Dst.szFile, THUMBNAIL_EXT))
		{
			MessageBeep(-1);	//***ERROR
			return(FALSE);
		}
		// copy the thumb
		if (!CopyFile(entry.Thumb.szFile, Dst.szFile, FALSE))
		{
			BSRInvalidateDriveIDs();
			Message(IDS_ECOPYTHUMBNAIL);
			continue;
		}
	   	// update dbf's
		if (!ChangeThumb(&entry.Thumb, &Dst))
		{
			BSRInvalidateDriveIDs();
			Message(IDS_ECOPYTHUMBNAIL);
			continue;
		}
	   	// delete original
		if (bDeleteOriginal)
			if (!FileDelete(entry.Thumb.szFile))
			{
				BSRInvalidateDriveIDs();
				continue;				//***ERROR
			}
	} while(GetNextFile(&File));
	return(TRUE);
}

LOCAL BOOL AlbumExport_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void AlbumExport_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

//***********************************************************************
//***********************************************************************
int WINPROC DlgAlbumExportProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, AlbumExport_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, AlbumExport_OnCommand);  

		HANDLE_DLGMSG(hDlg, WM_DESTROY, DWL_USER_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_CREATE, DWL_USER_OnCreate);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_ALBEXPORT);
	}
}

//************************************************************************/
LOCAL BOOL AlbumExport_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************/
{
	LPPORTDATA lpData;
	
	if (!(lpData = (LPPORTDATA)Alloc(sizeof(PORTDATA))))
		return(FALSE);
	SetWindowLong(hDlg, DWL_USER, (long)lpData);

	CenterPopup(hDlg);
	lpData->bCopyThumbs = lpData->bOldCopyThumbs = TRUE;
	lpData->bCopyFiles = FALSE;
	CheckDlgButton(hDlg,IDC_BSR_PORTCOPYFILES,lpData->bCopyFiles);
	CheckDlgButton(hDlg,IDC_BSR_PORTCOPYTHMS,lpData->bCopyThumbs);
	ControlEnable(hDlg, IDC_BSR_PORTCOPYTHMS,!lpData->bCopyFiles);
	if (!(lpData->lpDBFLL = GetSelectedAlbum(lpBsr->OpenState.hBrowser)))
		if (!(lpData->lpDBFLL = FindAlbum(lpBsr->OpenState.szAlbum)))
		{
			AstralDlgEnd(hDlg, FALSE);
			return(FALSE);
		}
	SetDlgItemText(hDlg, IDC_BSR_PORTALB, (LPTSTR)(lpData->lpDBFLL->dbf.info.szName));
	SetDlgItemText(hDlg, IDC_BSR_PORTALBDIR, (LPTSTR)_T("A:\\"));

	return(0);
}
		
//************************************************************************/
LOCAL void AlbumExport_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************/
{
	LPPORTDATA lpData;
	FNAME szDir;
	
	if (!(lpData = (LPPORTDATA)GetParentBsrState(hDlg, TRUE)))
		return;	
	switch(id)
	{
		case IDC_BSR_PORTCOPYTHMS:
			lpData->bOldCopyThumbs = lpData->bCopyThumbs = !lpData->bCopyThumbs;
			CheckDlgButton(hDlg,IDC_BSR_PORTCOPYTHMS,lpData->bCopyThumbs);
		break;	
		case IDC_BSR_PORTCOPYFILES:
			lpData->bCopyFiles = !lpData->bCopyFiles;
			CheckDlgButton(hDlg,id,lpData->bCopyFiles);
			if (!lpData->bCopyFiles)
				lpData->bCopyThumbs = lpData->bOldCopyThumbs;
			else
				lpData->bCopyThumbs = TRUE;
			CheckDlgButton(hDlg,IDC_BSR_PORTCOPYTHMS,lpData->bCopyThumbs);
			ControlEnable(hDlg, IDC_BSR_PORTCOPYTHMS,!lpData->bCopyFiles);
		break;	
		case IDOK:
			GetDlgItemText(hDlg, IDC_BSR_PORTALBDIR, szDir, sizeof(FNAME));
			RemovePathBackslash(szDir);
			if (!IsValidFileName(szDir))
			{
				Message(IDS_INVALIDNAME, (LPTR)szDir);
				break;
			}
			MakeFullPath(szDir);
			if (!CheckDir(szDir))
			{
				Message(IDS_EINVALIDDIR, (LPTR)szDir);
				break;
			}
			AstralCursor(IDC_WAIT);
			FixPath(szDir);
			ExportAlb(&lpData->lpDBFLL->dbf,szDir,lpData->bCopyFiles,lpData->bCopyThumbs);  
			DBFClose(&lpData->lpDBFLL->dbf);
			AstralCursor(NULL);
			AstralDlgEnd(hDlg, id);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, FALSE);
		break;
		default:
		break;
	}  // switch command
}

LOCAL BOOL AlbumImport_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam);
LOCAL void AlbumImport_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify);

//***********************************************************************
int WINPROC DlgAlbumImportProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//***********************************************************************
{
	switch (msg)
	{
		HANDLE_DLGRET(hDlg, WM_INITDIALOG, AlbumImport_OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, AlbumImport_OnCommand);  

		HANDLE_DLGMSG(hDlg, WM_DESTROY, DWL_USER_OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_CREATE, DWL_USER_OnCreate);
	    default:
			return Dialog_HandleDefaults(hDlg, msg, wParam, lParam, IDD_ALBIMPORT);
	}
}

//************************************************************************/
LOCAL BOOL AlbumImport_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam) 
//************************************************************************/
{
	LPPORTDATA lpData;
	
	if (!(lpData = (LPPORTDATA)Alloc(sizeof(PORTDATA))))
		return(FALSE);
	SetWindowLong(hDlg, DWL_USER, (long)lpData);

	CenterPopup(hDlg);
	lpData->bCopyFiles = FALSE;
	lpData->bCopyThumbs = lpData->bOldCopyThumbs = TRUE;
	CheckDlgButton(hDlg,IDC_BSR_PORTCOPYTHMS, lpData->bCopyThumbs);
	CheckDlgButton(hDlg,IDC_BSR_PORTCOPYFILES, lpData->bCopyFiles);
	ControlEnable(hDlg, IDC_BSR_PORTFILEDIR,lpData->bCopyFiles);
	ControlEnable(hDlg, IDC_BSR_PORTCOPYTHMS,!lpData->bCopyFiles);
	SetDlgItemText(hDlg, IDC_BSR_PORTFILEDIR, Browser.AlbumFileDir);			
	SetDlgItemText(hDlg, IDC_BSR_PORTALBDIR, (LPTSTR)_T("A:\\"));

	return(0);
}

//************************************************************************/
LOCAL void AlbumImport_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
//************************************************************************/
{
	LPPORTDATA lpData;
	FNAME szFilesDir, szDir;
	BOOL bRet;
	
	if (!(lpData = (LPPORTDATA)GetParentBsrState(hDlg, TRUE)))
		return;	
	switch(id)
	{
		case IDC_BSR_PORTCOPYFILES:
			lpData->bCopyFiles = !lpData->bCopyFiles;
			CheckDlgButton(hDlg,id,lpData->bCopyFiles);
			if (!lpData->bCopyFiles)
				lpData->bCopyThumbs = lpData->bOldCopyThumbs;
			else
				lpData->bCopyThumbs = TRUE;
			CheckDlgButton(hDlg,IDC_BSR_PORTCOPYTHMS,lpData->bCopyThumbs);
			ControlEnable(hDlg, IDC_BSR_PORTCOPYTHMS,!lpData->bCopyFiles);
			ControlEnable(hDlg, IDC_BSR_PORTFILEDIR,lpData->bCopyFiles);
		break;
		case IDC_BSR_PORTCOPYTHMS:
			lpData->bOldCopyThumbs = lpData->bCopyThumbs = !lpData->bCopyThumbs;
			CheckDlgButton(hDlg,IDC_BSR_PORTCOPYTHMS, lpData->bCopyThumbs);
		break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_BSR_PORTALBDIR, szDir, sizeof(FNAME));
			MakeFullPath(szDir);
			if (!CheckDir(szDir))
			{
				Message(IDS_EINVALIDDIR, (LPTR)szDir);
				break;
			}
			if (lpData->bCopyFiles)
			{
				GetDlgItemText(hDlg, IDC_BSR_PORTFILEDIR, szFilesDir, sizeof(FNAME));
				RemovePathBackslash(szFilesDir);
				if (!IsValidFileName(szFilesDir))
				{
					Message(IDS_INVALIDNAME, (LPTR)szFilesDir);
					break;
				}
				MakeFullPath(szFilesDir);
				if (!CheckDir(szFilesDir))
				{
					Message(IDS_EINVALIDDIR, (LPTR)szFilesDir);
					break;
				}
			}
			FixPath(szDir);
			FixPath(szFilesDir);
			AstralCursor(IDC_WAIT);
			bRet = ImportAlb(hDlg, szDir,szFilesDir, lpData->bCopyFiles,lpData->bCopyThumbs);
			AstralCursor(NULL);
			if (!bRet)
				break;	
			AstralDlgEnd(hDlg, id);
		break;
		case IDCANCEL:
			AstralDlgEnd(hDlg, FALSE);
		break;
		default:
		break;
	}  // switch command
}

//***********************************************************************
// Exprorts the DBF to lpDstDir.
// If bCopy Files them it will copy the files to lpDstDir.
// If bCopyThumbs then it will copy the thumbs to lpDstDir.
// You should NOT copy files and not thumbs.
//***********************************************************************
LOCAL BOOL ExportAlb(LPDBF lpSrcDBF, LPTSTR lpDstDir, BOOL bCopyFiles, BOOL bCopyThumbs)
//***********************************************************************
{
	FNAME szString;
	ENTRY entry;
	DBFS  outDBF;
	BOOL  bDuplicate, bSuccess;
	BOOL  bDBFOpen = FALSE;
	BOOL  bCursor = FALSE;
	VOLUMEID ThisVolume;
	int   total, num, ret;
	WORD wThisDrive;
	ULONG time;
	LPCTHMNAIL lpDstThm, lpSrcThm;
    LPVOID lp;

	// create a new album at the dst location
	lstrcpy(szString, Browser.DBFInfoDir);
	lstrcpy(Browser.DBFInfoDir, lpDstDir);
	bDBFOpen = (DBFNewAlbum(lpSrcDBF->info.szName, &outDBF, &bDuplicate) != NULL);
	lstrcpy(Browser.DBFInfoDir, szString);
	if (!bDBFOpen)
		goto ExitError;
	// is there aready on there 
	if (bDuplicate)
	{
		ret = AstralOKCancel(IDS_ALBUM_ALREADY_HERE, (LPTR)outDBF.info.szName, (LPTR)lpDstDir);
		if (ret != IDOK)
			goto Exit;
		ret = AstralOKCancel(IDS_DELETEALB, (LPTR)outDBF.info.szName);
		if (ret != IDOK)
			goto Exit;
		DBFClose(&outDBF);
		bDBFOpen = FALSE;
		// delete the duplicate
		if (!FileDelete(outDBF.szFile))
		{
			Message (IDS_NODELETE, (LPTR)outDBF.szFile);
			goto Exit;
		}
		// Try Again
		lstrcpy(szString, Browser.DBFInfoDir);
		lstrcpy(Browser.DBFInfoDir, lpDstDir);
		bDBFOpen = (DBFNewAlbum(lpSrcDBF->info.szName, &outDBF, &bDuplicate) != NULL);
		lstrcpy(Browser.DBFInfoDir, szString);
		if (!bDBFOpen || bDuplicate)
			goto ExitError;
	}	
	if( IsDBCSLeadByte(( BYTE )*lpDstDir ))
        wThisDrive = ( WORD )*(( LPWORD )lpDstDir );
    else
    	wThisDrive = MAKEWORD( *lpDstDir, 0 );

	DriveVolume(wThisDrive, &ThisVolume, YES);
	BSRInvalidateDriveIDs();

	if (!DBFReOpen(&outDBF, YES))
		goto ExitError;

	if (!DBFReOpen(lpSrcDBF, NO))
		goto ExitError;

	// go through items copying and updating
	total = lpSrcDBF->info.numEntry;
	bCursor = TRUE;
	ProgressBegin(1,0,lpBsr->lpOpenState->hBrowser);
	num = 0;

	while (DBFGetEntryNum(lpSrcDBF, num++, &entry))
	{
		AstralClockCursor(num-1, total, NO);
		if(bCopyFiles)
		{
			// copy the file and update entry
			lstrcpy(szString, lpDstDir);
			lstrcat(szString, filename(entry.File.szFile));
			if (bSuccess = VolumeCheck(&entry.File, NULL))
			{
				// Check for duplicate and overwrite self
				if (!StringsEqual(entry.File.szFile, szString))
				{
					if (FileExists(szString))
					{
						ret = AstralConfirm(IDS_IMPORT_FILEOVERWRITE, (LPTR)szString, (LPTR)entry.File.szFile);
						if (ret == IDNO)
							continue;
						else if (ret != IDYES)
							goto Exit;
					}
					bSuccess = CopyFile(entry.File.szFile, szString, NO);
				}
			}
			if (!bSuccess)	 
			{
				if (AstralAffirm(IDS_ECOPY, (LPTR)entry.File.szFile, (LPTR)szString) == IDYES)
					continue;
				else
					goto ExitError;
			}
			lstrcpy(entry.File.szFile, szString);
			entry.File.volume = ThisVolume;
			// Copying File without thumb is not recommended
			// Copy and update thumbs
			if (bCopyThumbs)
			{
				time = 0L;
				SafeFileStats(entry.File.szFile, NULL, NULL, &time, NULL);
				// create a new thumbnail at the dst location
				lpSrcThm = CTHMOpen(&entry);
				lstrcpy(szString, Browser.ThumbnailDir);
				lstrcpy(Browser.ThumbnailDir, lpDstDir);
				lpDstThm = CTHMCreate(&entry,YES);
				lstrcpy(Browser.ThumbnailDir, szString);
				if (bSuccess = (lpSrcThm != NULL && lpDstThm != NULL))
					bSuccess = CTHMCopyThumb(lpDstThm, lpSrcThm, time);
				if (lpDstThm)
					CTHMClose(lpDstThm);
				if (lpSrcThm)
					CTHMClose(lpSrcThm);
				if (!bSuccess)
					Message(IDS_ECOPYTHUMBNAIL);
			}
		}
		else if (bCopyThumbs)
		{
			// simply copy thumb file
			lstrcpy(szString, lpDstDir);
			if (bSuccess = VolumeCheck(&entry.Thumb, NULL))
				if (bSuccess = GetTempFilename(szString, THUMBNAIL_EXT))
					if (bSuccess = CopyFile(entry.Thumb.szFile, szString, NO))
					{
						// update entry
						lstrcpy(entry.Thumb.szFile, szString);
						entry.Thumb.volume = ThisVolume;
					}	
			if (!bSuccess)
				Message(IDS_ECOPYTHUMBNAIL);
		}

		// copy thumbs not already on the drive
        lp = entry.Thumb.szFile;
        if( IsDBCSLeadByte( *(( LPBYTE )lp )))
        {
		    if(( MBToUpperW(( LPTSTR )lp ) == wThisDrive ) &&
				 EqualVolume( &entry.Thumb.volume, &ThisVolume ))
			    *(( LPBYTE )lp ) = THISDISK;
        }
        else
        {
		    if(( MBToUpper(( LPTSTR )lp ) == wThisDrive ) &&
				 EqualVolume( &entry.Thumb.volume, &ThisVolume ))
			    *(( LPBYTE )lp ) = THISDISK;          
        }

        lp = entry.File.szFile;
        if( IsDBCSLeadByte( *(( LPBYTE )lp )))
        {
		    if(( MBToUpperW(( LPTSTR )lp ) == wThisDrive ) &&
				 EqualVolume( &entry.Thumb.volume, &ThisVolume ))
			    *(( LPBYTE )lp ) = THISDISK;
        }
        else
        {
		    if(( MBToUpper(( LPTSTR )lp ) == wThisDrive ) &&
				 EqualVolume( &entry.Thumb.volume, &ThisVolume ))
			    *(( LPBYTE )lp ) = THISDISK;          
        }

		// add it to the exported album
		DBFAddEntry(&outDBF, &entry);
	}
	ProgressEnd();
	DBFClose(&outDBF);
	return(TRUE);

ExitError:
	Message(IDS_BSR_ERROR);
Exit:
	if (bCursor)
		ProgressEnd();
	if (bDBFOpen)
		DBFClose(&outDBF);
	return(FALSE);
}

//***********************************************************************
// Imports the DBFs from lpSrcDir.
// If bCopy Files them it will copy the files to lpFileDst.
// If bCopyThumbs then it will copy the thumbs local.
// You should NOT copy files and not thumbs.
// Returns TRUE if there were any albums to import.
//***********************************************************************
LOCAL BOOL ImportAlb(HWND hDlg, LPTSTR lpSrcDir, LPTSTR lpFileDst, BOOL bCopyFiles, BOOL bCopyThumbs)
//***********************************************************************
{
	FNAME szString;
	FINDDATA afile;
	DBFS dbf;
	int ret, num;
	BOOL bConfirm = TRUE;
	

	// find all album dbf's at the dst location
	lstrcpy(szString, Browser.DBFInfoDir);
	lstrcpy(Browser.DBFInfoDir, lpSrcDir);
	num = 0;
	DBFBeginInfo(&afile, ID_ALBUM);
	while(DBFNextInfo(&dbf,&afile, NO /*bWrite*/, YES /*bClose*/))
		num++;
	if (!num)
	{
		Message(IDS_NO_ALBUMS_HERE, (LPTR)szString);
		lstrcpy(Browser.DBFInfoDir, szString);
		return(FALSE);
	}
	DBFBeginInfo(&afile, ID_ALBUM);
	while(DBFNextInfo(&dbf,&afile, NO /*bWrite*/, YES /*bClose*/))
	{
		num--;
		lstrcpy(Browser.DBFInfoDir, szString);
		if (bConfirm)
		{
			ret = AstralYesToAllString(hDlg, IDD_CONFIRM_IMPORTALB, dbf.info.szName, num);
			if (ret == IDC_BSR_NO)
				goto Continue;
			else if (ret == IDC_YESTOALL)
				bConfirm = FALSE;
			else if (ret != IDOK)
				return(TRUE);
		}
		DoImportAlb(hDlg,&dbf,lpFileDst,bCopyFiles,bCopyThumbs);
Continue:
		lstrcpy(Browser.DBFInfoDir, lpSrcDir);
	}
	lstrcpy(Browser.DBFInfoDir, szString);
	return(TRUE);
}

//***********************************************************************
// Imports the DBF.
// If bCopy Files them it will copy the files to lpFileDst.
// If bCopyThumbs then it will copy the thumbs local.
// You should NOT copy files and not thumbs.
//***********************************************************************
LOCAL BOOL DoImportAlb(HWND hDlg, LPDBF lpDBF, LPTSTR lpFileDst, BOOL bCopyFiles, BOOL bCopyThumbs)
//***********************************************************************
{

	FNAME szNewAlbum, szString;
	ENTRY entry;
	BOOL  bSuccess;
	BOOL  bDBFOpen = FALSE;
	BOOL  bCursor = FALSE;
	VOLUMEID newSrcVol;
	VOLUMEID dstVol;
	WORD newSrcDrive, wChar;
	int   total, num, ret;
	ULONG time;
	LPCTHMNAIL lpDstThm, lpSrcThm;
	LPDBFLL node; 
	DBFS dbf;
	LPTSTR lp;
	
	if( IsDBCSLeadByte(( BYTE )*( lpDBF->szFile )))
		newSrcDrive = *(( LPWORD )( lpDBF->szFile ));
	else
		newSrcDrive = *( lpDBF->szFile );

	DriveVolume(newSrcDrive, &newSrcVol, YES);
	// check for existing album with the same name
	lstrcpy(szNewAlbum, lpDBF->info.szName);
	while (DBFOpenAlbum(szNewAlbum, &dbf, NO))
	{
		bSuccess = GetDialogString(hDlg,_T(""),lpDBF->info.szName,szNewAlbum,IDD_IMPORT_GETNEWNAME);
		if (bSuccess != IDOK)
		{
			DBFClose(lpDBF);
			return(FALSE);
		}
		if (!CheckAlbumName(szNewAlbum))
			Message(IDS_BADALBNAME);
		DBFClose(&dbf);
	}
	node = (LPDBFLL)Alloc(sizeof(DBFLL));
	if(!node)
		goto ExitError;
	// Open src album
	if (!DBFReOpen(lpDBF, NO))
		goto ExitError;
	// Open new album
	if (!DBFNewAlbum(szNewAlbum, &(node->dbf), &bSuccess/*dummy*/))
		goto ExitError;
	bDBFOpen = TRUE;
	BSRInvalidateDriveIDs();
	DriveVolume(lpFileDst[0], &dstVol, YES);
	total = lpDBF->info.numEntry;
	bCursor = TRUE;
	ProgressBegin(1,0,hDlg);
	// go through items copying files , thumbs and, updating drives.
	num = 0;
	while(DBFGetEntryNum(lpDBF, num++, &entry))
	{
		AstralClockCursor(num-1, total, NO);

		// be sure drive is updated 
		lp = entry.Thumb.szFile;
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wChar = MBToUpperW(( LPTSTR )lp );
		else	
			wChar = MBToUpper( ( LPTSTR )lp );

		if( wChar == THISDISK )
		{
		 	entry.Thumb.volume = newSrcVol;

			if( IsDBCSLeadByte( LOBYTE( newSrcDrive )))
				*(( LPWORD )entry.Thumb.szFile ) = newSrcDrive;
			else	
				*( entry.Thumb.szFile ) = LOBYTE( newSrcDrive );
		}

		lp = entry.File.szFile;
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wChar = MBToUpperW(( LPTSTR )lp );
		else	
			wChar = MBToUpper( lp );

		if( wChar == THISDISK )
		{
		 	entry.File.volume = newSrcVol;
			if( IsDBCSLeadByte( LOBYTE( newSrcDrive )))
				*(( LPWORD )entry.File.szFile ) = newSrcDrive;
			else	
				*( entry.File.szFile ) =LOBYTE( newSrcDrive );
		}

		if(bCopyFiles)
		{
			// copy the file and update entry
			lstrcpy(szString, lpFileDst);
			lstrcat(szString, filename(entry.File.szFile));
			if (bSuccess = VolumeCheck(&entry.File, NULL))
			{
				// Check for duplicate and overwrite self
				if (!StringsEqual(entry.File.szFile, szString))
				{
					if (FileExists(szString))
					{
						ret = AstralConfirm(IDS_IMPORT_FILEOVERWRITE, (LPTR)szString, (LPTR)entry.File.szFile);
						if (ret == IDNO)
							continue;
						else if (ret != IDYES)
							goto Exit;
					}
					bSuccess = CopyFile(entry.File.szFile, szString, NO);
				}
			}
			if (!bSuccess)	 
			{
				if (AstralAffirm(IDS_ECOPY, (LPTR)entry.File.szFile, (LPTR)szString))
					continue;
				else
					goto ExitError;
			}
			lstrcpy(entry.File.szFile, szString);
			entry.File.volume = dstVol;
			// Copying File without thumb is not recomened
		}
		if (bCopyThumbs)
		{
			// Copy and update thumbs
			time = 0L;
			// create a new local thumbnail if one doesn't already exist
			lpSrcThm = CTHMOpen(&entry);
			if (bCopyFiles)	
			{
				// get new time stamp
				SafeFileStats(entry.File.szFile, NULL, NULL, &time, NULL);
			}
			else if (lpSrcThm)
			 	time = lpSrcThm->thm.info.time;
			lpDstThm = NULL;
			bSuccess = (FindOrCreateThumb(&entry.File, &entry, NO) != BSR_ERROR);
			if (bSuccess)
				lpDstThm = CTHMOpen(&entry);
			if (bSuccess = (bSuccess && lpSrcThm != NULL && lpDstThm != NULL))
				bSuccess = CTHMCopyThumb(lpDstThm, lpSrcThm, time);
			if (lpDstThm) CTHMClose(lpDstThm);
			if (lpSrcThm) CTHMClose(lpSrcThm);
			if (!bSuccess)
				Message(IDS_ECOPYTHUMBNAIL);
		}
		// add it to the imported album
		DBFAddEntry(&node->dbf, &entry);
	}
	ProgressEnd();
	AddToAlbList(node);
	DBFClose(&node->dbf);
	DBFClose(lpDBF);
	return(TRUE);

ExitError:
	Message(IDS_BSR_ERROR);
Exit:
	if (bCursor)
		ProgressEnd();
	DBFClose(lpDBF);
	if (bDBFOpen)
		DBFClose(&node->dbf);
	return(FALSE);
}
