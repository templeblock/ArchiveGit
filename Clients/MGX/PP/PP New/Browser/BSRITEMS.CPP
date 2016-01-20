//
// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// handles items in Image Browser dlg

#include "pp.h"
#include <time.h>
#include "id.h"
#include "macros.h"
#include "bsrtypes.h"
#include "handlebs.h"
#include "browser.h"
#include "bsrid.h"
#include "bsritems.h"
#include "bsrutil.h"
#include "dbfmgr.h"
#include "cthmmgr.h"
#include "bsrdlgs.h"


//**************************Externs******************************************/
extern LPBSRSTATE lpBsr;
extern BOOL bImported;
//****************************Static Vars***************************************/
static RDRDEF Readers[] = {
//   CT  LA  SP  P4  P8  24  32 OPN SAV SV1 SV8 S24 S32 OPTIONS
"", YES,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES,YES,YES, NO, // IDT_ALLFILES
"", YES,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES,YES,YES, NO, // IDT_ALLIMAGES
"", YES,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES,YES,YES,YES, // IDT_PPCOMPOUND
"", YES,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES,YES,YES,YES, // IDT_PP
"", YES,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES,YES,YES,YES, // IDT_TIFF
"", YES,NO , NO, NO,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES, // IDT_TARGA
"", YES,YES,YES, NO,YES, NO, NO,YES,YES,YES,YES,YES,YES, NO, // IDT_GIF
"", YES,YES,YES,YES,YES,YES, NO,YES,YES,YES,YES,YES,YES, NO, // IDT_BMP
"", YES,YES,YES,YES,YES,YES, NO,YES,YES,YES,YES,YES,YES, NO, // IDT_DIB
"", YES,YES,YES, NO,YES,YES, NO,YES,YES,YES,YES,YES,YES, NO, // IDT_PCX
"", YES,YES,YES, NO,YES,YES, NO, NO,YES,YES,YES,YES,YES,YES, // IDT_EPS
"",  NO, NO, NO, NO, NO, NO,YES, NO,YES, NO, NO, NO,YES,YES, // IDT_DCS
"JPEG_DLL.DLL",
    YES, NO, NO, NO, NO,YES, NO,YES,YES,YES,YES,YES,YES,YES, // IDT_JPEG
"", YES, NO, NO, NO, NO,YES, NO,YES, NO, NO, NO, NO, NO, NO, // IDT_PHOTOCD
"", NO,  NO, NO, NO, NO,NO, YES,YES,YES, NO, YES, NO,YES, NO, // IDT_SCITEX
"", YES,YES,YES, NO,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES, // IDT_AVI
"", YES,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES,YES,YES,YES, // IDT_PP4
};

static RDRDEF ImportReaders[] = {
//   CT  LA  SP  P4  P8  24  32 OPN SAV SV1 SV8 S24 S32 COMPRESS
"", YES,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES,YES,YES, NO, // IDT_ALLIMPORTIMAGES, IDT_ALLIMPORTFILES
"", YES,YES,YES, NO,YES,YES,YES,YES,YES,YES,YES,YES,YES,YES, // IDT_TIFF
};

//********************Static prototypes****************************************/
LOCAL WORD DriveName( int iDrive, LPTSTR lpName );
LOCAL LPTSTR RemoveDots( LPTSTR lpFileName );
LOCAL void AddSpaces(LPTSTR lpString,int iCount );
LOCAL int ODCFindItem(HWND hCombo,int id);
LOCAL void fill_album_filelist(LPDBF album, LPTSTR lpSpec, HWND hFileList);
LOCAL void fill_dir_filelist(HWND hFileList, LPTSTR lpDir, LPTSTR lpSpec);
LOCAL BOOL StoreSelectedFiles(HWND hDlg);
LOCAL BOOL RestoreSelectedFiles(HWND hDlg);
LOCAL BOOL CheckGotoPCD(void);

//************************************************************************/
//	Fills combo with all drives.  Displays current drive
//************************************************************************/
void FillDrivesCombo(HWND hDlg)
//************************************************************************/
{
	int i, count, drive;
	WORD id;
	WORD type;
	STRING szString;
	HWND hControl;

	if (!(hControl = GetDlgItem(hDlg, IDC_BSR_DRIVE)))
		return;

	drive = CURRENTDRIVE;
	count = 0;
	ComboBox_ResetContent(hControl);
	for ( i=0; i<=26; i++ )
	{
		if ( !(type = DriveName(i, szString )))
			continue;
		switch(type)
		{
			case DRIVE_REMOVABLE:	 
				id = IDC_PATHICON_FLOPPY;	
			break;	
			case DRIVE_RAMDISK:			 
				id = IDC_PATHICON_RAMDRIVE;
			break;	
			case DRIVE_REMOTE:		 
				id = IDC_PATHICON_NET;
			break;	
			case DRIVE_CDROM:		 
				id = IDC_PATHICON_CDDRIVE;	
			break;	
			case DRIVE_FIXED:		 
			default:
				id = IDC_PATHICON_FIXED;		
			break;	
		}
		if( type > 1 )
			{
			CombineStringID( szString, id);
			ComboBox_AddString(hControl, Lowercase(szString));
			if ( i == drive )
				ComboBox_SetCurSel( hControl, count);
			count++;
			}
	}
}

//************************************************************************/
// fils directory list based on current working directory
//	fills path text.
//************************************************************************/
BOOL FillDirectoryList(HWND hDlg)
//************************************************************************/
{
	HWND hCombo;
	int i, count;
	STRING szString, szDirName;
	int retc, iMaxSubDirs;
	FINDDATA dir;
	WORD wChar;
    LPTSTR lp, lpDir;

	hCombo = DIRLIST(hDlg);

	// this is done first because CD drives require access before CURRENTDIR will work.
	retc = Dos_FindFirst( _T("*.*"), ATTRIB_SUBDIR, &dir );
	
	if (!CURRENTDIR( szString, MAX_STR_LEN ))	//***ERROR?? could not read cwd
	{	
		ListBox_ResetContent(hCombo);
		SetDlgItemText(hDlg, IDC_BSR_PATH_TEXT, (LPTSTR)_T(""));
		return(FALSE);
	}
    // skip over drive identifier
    lpDir = MyCharNextN( szString, 3 );
	lp = lpDir;
	iMaxSubDirs = 32;

	SetWindowRedraw (hCombo,FALSE);
	ListBox_ResetContent(hCombo);
	AddSpaces( szDirName, iMaxSubDirs-- ); // Add spaces to defeat the sort
	lstrcat( szDirName, _T("\\") );
	CombineStringID( szDirName, IDC_PATHICON_ROOT );
	ListBox_AddString(hCombo, szDirName );
	count = 0;

	while( *lp )
	{
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wChar = *(( LPWORD )lp );
		else
			wChar = *lp;

		if( wChar != _T('\\') )
		{
			lp = MyCharNext( lp );
			continue;
		}
        // Overwrite the slash and skip past it
        lp = MyCharNext( lp );
        *( MyCharPrev( lpDir, lp )) = _T('\0');

		AddSpaces( szDirName, iMaxSubDirs-- ); // Add spaces to defeat the sort
		lstrcat( szDirName, lpDir );
		CombineStringID( szDirName, IDC_PATHICON_OPEN );
		ListBox_AddString( hCombo, szDirName );
		count++;
		lpDir = lp;
	}

	if ( lp > lpDir )
	{
		AddSpaces( szDirName, iMaxSubDirs-- ); // Add spaces to defeat the sort
		lstrcat( szDirName, lpDir );
		CombineStringID( szDirName, IDC_PATHICON_ACTIVE );
		ListBox_AddString(hCombo, szDirName);
		count++;
	}

	for ( i=0; ; i++ )
	{
		if ( !retc )
			break;

		if( IsDBCSLeadByte(( BYTE )*( dir.szName )))
			wChar = *(( LPWORD )( dir.szName ));
		else
			wChar = *( dir.szName );

		if ( dir.dwAttrib & ATTRIB_SUBDIR && wChar != _T('.') )	// If this is a subdirectory
		{
			lstrcpy( szDirName, dir.szName );
			CombineStringID( szDirName, IDC_PATHICON_CLOSED );
			ListBox_AddString(hCombo, szDirName);
		}
		retc = Dos_FindNext( &dir );
	}
	SetPathText(hDlg);
	SetWindowRedraw (hCombo,TRUE);
	return(TRUE);
}

//**************************************************************************/
// fills file list box
// 		Directory mode:
//			based on current directory, szSearchSpec, wFileAttr.
//		Album mode:
//			based on current (lpBsr->OpenState.szAlbum, _T("") for no album)
//			Note: the album MUST be in album list.
//	If bMaintainSelect then it will TRY to re-select any selected items.
//**************************************************************************/
void FillFileList(HWND hDlg, BOOL bMaintainSelect)
//************************************************************************/
{
	HWND hFileList;
	int i, topindex;
	LPDBF album;
	LPDBFLL albumll;
	FNAME szSpec;
	FNAME szDir;
	
	hFileList = FILELIST(hDlg);
	BSRInvalidateDriveIDs();

	if (bMaintainSelect)
	{
		topindex = ListBox_GetTopIndex(hFileList);
		StoreSelectedFiles(hDlg);
	}
	SetWindowRedraw (hFileList, FALSE);
	if(DIRMODE)
	{
		//	Directory case
		// fill temporary file list
		CURRENTDIR(szDir, sizeof(szDir));
		FixPath(szDir);										 
		ListBox_ResetContent(hFileList);
		lstrcpy(szSpec, lpBsr->OpenState.szSearchSpec);

        if( bImported )
        {
		    if(lpBsr->OpenState.idFileType == IDT_ALLIMPORTIMAGES )
		    {
			    // use all image types search specs
			    for( i=IDT_FIRSTIMPORTREAL; i<= IDT_LASTIMPORTREAL; i++ )
			    {
				    SetExtension( szSpec, i, YES );
				    fill_dir_filelist( hFileList, szDir, szSpec );
			    }
		    }
		    else 
		    {
			    // Just use the search spec
			    fill_dir_filelist(hFileList, szDir, lpBsr->OpenState.szSearchSpec);
		    }
        }
        else
        {
		    if (lpBsr->OpenState.idFileType == IDT_ALLIMAGES)
		    {
			    // use all image types search specs
			    for (i=IDT_FIRSTREAL; i<= IDT_LASTREAL; i++)
			    {
				    SetExtension(szSpec, i, YES);
				    fill_dir_filelist(hFileList, szDir, szSpec);
			    }
		    }
		    else 
		    {
			    // Just use the search spec
			    fill_dir_filelist(hFileList, szDir, lpBsr->OpenState.szSearchSpec);
		    }
        }

	} // if directory mode
    // imported files do not support albums
	else	// album mode
    if( !bImported )
	{
		// try to find album
		if (lpBsr->OpenState.szAlbum[0] == _T('\0'))
			goto ClearList;
		if (!(albumll = FindAlbum(lpBsr->OpenState.szAlbum)))
			goto ClearList;
		album = &(albumll->dbf);
		if  (!DBFReOpen(album, NO))
			goto ClearList;
		ListBox_ResetContent(hFileList);
		// fill list
		lstrcpy(szSpec, lpBsr->OpenState.szSearchSpec);
		if (lpBsr->OpenState.idFileType == IDT_ALLIMAGES)
		{
			// use all image types search specs
			for (i=IDT_FIRSTREAL; i<= IDT_LASTREAL; i++)
			{
				SetExtension(szSpec,i, YES);
				fill_album_filelist(album, szSpec, hFileList);
			}
		}
		else 
		{
			// Just use the search spec
			fill_album_filelist(album, lpBsr->OpenState.szSearchSpec, hFileList);
		}
		// close album
		DBFClose(album);
	}
	if (bMaintainSelect)
	{
		RestoreSelectedFiles(hDlg);
		ListBox_SetTopIndex(hFileList, topindex);
	}
	SetWindowRedraw (hFileList, TRUE);
	SetFileListInfo(hDlg);
	return;

ClearList:
	SetWindowRedraw (hFileList, TRUE);
	ListBox_ResetContent(hFileList);
	SetFileListInfo(hDlg);
	return;
}

//**************************************************************************/
//	Stores the complete strings for every selected item in the file list 
//		into LineBuffer[0].
//**************************************************************************/
LOCAL BOOL StoreSelectedFiles(HWND hDlg)
//**************************************************************************/
{
	LPFENTRY lpItems;
	int i;
	
	lpItems = (LPFENTRY)LineBuffer[0];
	lpItems[0][0] = '\0';
	if (!OpenFileList(hDlg))
		return(FALSE);
	GetNumFileEntry(-1, lpItems[0]);
	i = 0;
	while(GetNextFileEntry(lpItems[i]))
		i++;				    
	lpItems[i][0] = '\0';
	return(TRUE);
}

//**************************************************************************/
// Sets all items in LineBuffer[0] to selected in the file list.
//**************************************************************************/
LOCAL BOOL RestoreSelectedFiles(HWND hDlg)
//**************************************************************************/
{
	LPFENTRY lpItems;
	int i, count;
	HWND hBox;
	
	lpItems = (LPFENTRY)LineBuffer[0];
	if (!(hBox = FILELIST(hDlg)))
		return(FALSE);

	count = 0;
	while(lpItems[0][0])
	{
		count++;
		i= ListBox_FindString(hBox, -1, lpItems);
		if (i>=0)
			ParseListBoxMessage(lpBsr->OpenState.bMultiListBox, hBox, LB_SETSEL, TRUE, MAKELONG(i,0));
		if (count>200)
			return(FALSE);  // just in case
		lpItems++;	
	}
	return(TRUE);
}

//**************************************************************************/
// fills album list box	based on ALREADY CREATED ALBUM LIST (lpAlbList)
// 		Album mode:
//			fills list box.
//			fills path text.
//		Directory mode:
//			fill combo box.
//**************************************************************************/
void FillAlbList(HWND hDlg)
//************************************************************************/
{
	HWND hAlbBox;
	LPDBFLL alb;
	FNAME szAlbum;
	FNAME szSelect;
	BOOL  bActive, bFound;
	WORD index, num;

	if (ALBMODE)
	{
		// Fill actual album list box
		hAlbBox = ALBLIST(hDlg);
		if (!hAlbBox)
			return;
		SetWindowRedraw (hAlbBox,FALSE);
		ListBox_ResetContent(hAlbBox);
		alb = FirstAlb();
		bActive = FALSE;
		bFound = FALSE;
		*szSelect = _T('\0');

		// go throgh all albums
		while (alb)
		{
			lstrcpy(szAlbum, alb->dbf.info.szName);
			if (StringsEqual(szAlbum, lpBsr->OpenState.szAlbum))
				bActive = TRUE;
			CombineStringID(szAlbum, (WORD)(bActive ?  IDC_ICON_ALBUMOPEN: IDC_ICON_ALBUMCLOSED));
			index = ListBox_AddString(hAlbBox, szAlbum);
			if (bActive)
			{
				bActive = FALSE;
				lstrcpy(szSelect, szAlbum);
			}
			alb = GetNextAlb();
		}
		// be sure an album is opened
		if (szSelect[0] != _T('\0'))
			ListBox_SelectString(hAlbBox, -1, szSelect);
		else
		{
			// open and select first item
			index = ListBox_GetCount(hAlbBox);
			if (index != 0 && index != LB_ERR)
			{
				index = ListBox_GetTopIndex(hAlbBox);
				ListBox_SetCurSel(hAlbBox, index);
				ListBox_GetText(hAlbBox, index, szAlbum);
				ExtractStringID(szAlbum);
				CombineStringID(szAlbum, IDC_ICON_ALBUMOPEN);
				ChangeListBoxItem(hAlbBox, index, (LPTSTR)szAlbum);
				ExtractStringID(szAlbum);
				lstrcpy(lpBsr->OpenState.szAlbum, szAlbum);
			}
			else
				lstrcpy(lpBsr->OpenState.szAlbum, _T(""));
		}
		SetWindowRedraw (hAlbBox,TRUE);
		SetPathText(hDlg);
		return;
	}  	// if album mode
	else
	{
		// fill Album Combo box
		hAlbBox = GetDlgItem(DIRWINDOW(hDlg), IDC_BSR_ALBUMCOMBO);
		if (!hAlbBox)
			return;
		ComboBox_ResetContent(hAlbBox);
		index = 0;
		bActive = FALSE;
		alb = FirstAlb();
		while (alb)
		{
			lstrcpy(szAlbum, alb->dbf.info.szName);
			index = ComboBox_AddString(hAlbBox, szAlbum);

			// select current string
			if (StringsEqual(szAlbum, lpBsr->OpenState.szAlbum) && index != CB_ERR)
				bActive = TRUE;
			alb = GetNextAlb();
		}
		// be sure one is selected
		if (bActive)
			ListBox_SelectString(hAlbBox, -1, lpBsr->OpenState.szAlbum);
		else
		{
			// if there is no selected then select last one
			num = ComboBox_GetCount(hAlbBox);
			if (!num)
				return;
			ComboBox_SetCurSel(hAlbBox, num-1);
			index = ComboBox_GetCurSel(hAlbBox);
			if (index == CB_ERR)
				return;
		  	ComboBox_GetLBText(hAlbBox, index, szAlbum);
			ExtractStringID(szAlbum);
			lstrcpy(lpBsr->OpenState.szAlbum, szAlbum);
		}
		return;
	}	// else (directory mode)
}

//**************************************************************************/
// fills File Type Combo box: based on lpBsr->OpenState.idFileType
//**************************************************************************/
void FillFileType(HWND hDlg)
//**************************************************************************/
{
	HWND hControl, hWnd;
	int numItems, i, idIndex, depth;
	STRING szString;
	BOOL bEnable, bNewSelect;

	hWnd = FILEWINDOW(hDlg);

    if( bImported )
    {
    	CheckComboItem(hWnd, IDC_BSR_FILETYPE, IDT_FIRSTIMPORTFILETYPE,
	    		IDT_LASTIMPORTFILETYPE, lpBsr->OpenState.idFileType);
    }
    else
    {
    	CheckComboItem(hWnd, IDC_BSR_FILETYPE, IDT_FIRSTFILETYPE,
	    		IDT_LASTFILETYPE, lpBsr->OpenState.idFileType);
    }

	hControl = GetDlgItem(hWnd, IDC_BSR_FILETYPE);
	numItems = ComboBox_GetCount( hControl);

	// Remove bad items and deselect others
	depth = Bound(lpBsr->OpenState.frameDepth, 0, NUM_DEPTHS-1);
	bNewSelect = FALSE;

    if( bImported )
    {
	    for( i=IDT_FIRSTIMPORTFILETYPE; i<=IDT_LASTIMPORTFILETYPE; i++ )
	    {
		    // check if this is a valid type for the operation.
		    bEnable = lpBsr->OpenState.bSaving ?
			    ImportReaders[(ULONG)((( i - IDT_FIRSTIMPORTFILETYPE ) < ( IDT_FIRSTIMPORTREAL - IDT_FIRSTIMPORTFILETYPE )) ? 0 : 1 )].bCanSave:
			    ImportReaders[(ULONG)((( i - IDT_FIRSTIMPORTFILETYPE ) < ( IDT_FIRSTIMPORTREAL - IDT_FIRSTIMPORTFILETYPE )) ? 0 : 1 )].bOpen;

		    if (!bEnable)
		    {
			    // take off the list
			    idIndex = ODCFindItem(hControl, i);
			    ControlEnable(hWnd, i, bEnable); // so it doesnt get selected
			    if (i == lpBsr->OpenState.idFileType)
				    bNewSelect = TRUE;
			    if (idIndex >= 0)
				    ComboBox_DeleteString(hControl, idIndex);
			    continue;
		    }
		    if (lstrlen(ImportReaders[(ULONG)((( i - IDT_FIRSTIMPORTFILETYPE ) < ( IDT_FIRSTIMPORTREAL - IDT_FIRSTIMPORTFILETYPE )) ? 0 : 1 )].szDLL))
			    {
			    lstrcpy(szString, Control.ProgHome);
			    lstrcat(szString, ImportReaders[(ULONG)((( i - IDT_FIRSTIMPORTFILETYPE ) < ( IDT_FIRSTIMPORTREAL - IDT_FIRSTIMPORTFILETYPE )) ? 0 : 1 )].szDLL);
			    bEnable = FileExists(szString);
			    if (i == lpBsr->OpenState.idFileType && !bEnable)
				    bNewSelect = TRUE;
			    }
		    if (bEnable && lpBsr->OpenState.bSaving && !ImportReaders[((( i - IDT_FIRSTIMPORTFILETYPE ) < ( IDT_FIRSTIMPORTREAL - IDT_FIRSTIMPORTFILETYPE )) ? 0 : 1 )].bSave[depth])
			    bEnable = FALSE;
		    if (i == lpBsr->OpenState.idFileType && !bEnable)
			    bNewSelect = TRUE;
		    ControlEnable(hWnd, i, bEnable);
	    }
    }
    else
    {
	    for ( i=IDT_FIRSTFILETYPE; i<=IDT_LASTFILETYPE; i++ )
	    {
		    // check if this is a valid type for the operation.
		    bEnable = lpBsr->OpenState.bSaving ?
			    Readers[(ULONG)i - IDT_FIRSTFILETYPE].bCanSave:
			    Readers[(ULONG)i - IDT_FIRSTFILETYPE].bOpen;
		    if (!Control.Retail && i == IDT_DCS)
			    bEnable = FALSE;
		    if (!bEnable)
		    {
			    // take off the list
			    idIndex = ODCFindItem(hControl, i);
			    ControlEnable(hWnd, i, bEnable); // so it doesnt get selected
			    if (i == lpBsr->OpenState.idFileType)
				    bNewSelect = TRUE;
			    if (idIndex >= 0)
				    ComboBox_DeleteString(hControl, idIndex);
			    continue;
		    }
		    if (lstrlen(Readers[(ULONG)i-(WORD)IDT_FIRSTFILETYPE].szDLL))
			    {
			    lstrcpy(szString, Control.ProgHome);
			    lstrcat(szString, Readers[(ULONG)i-(WORD)IDT_FIRSTFILETYPE].szDLL);
			    bEnable = FileExists(szString);
			    if (i == lpBsr->OpenState.idFileType && !bEnable)
				    bNewSelect = TRUE;
			    }
		    if (bEnable && lpBsr->OpenState.bSaving && !Readers[(ULONG)i-(WORD)IDT_FIRSTFILETYPE].bSave[depth])
			    bEnable = FALSE;
		    if (i == lpBsr->OpenState.idFileType && !bEnable)
			    bNewSelect = TRUE;
		    ControlEnable(hWnd, i, bEnable);
	    }
    }
	if( bNewSelect )
	{
        if( bImported )
    		lpBsr->OpenState.idFileType = IDT_AI;
        else
    		lpBsr->OpenState.idFileType = IDT_TIFF;
	}

	idIndex = ODCFindItem(hControl, lpBsr->OpenState.idFileType);
	idIndex = idIndex >= 0 ? idIndex:0;
	if (ComboBox_GetCurSel( hControl) != idIndex)
		ComboBox_SetCurSel( hControl, idIndex);
}


//**************************************************************************/
// fills Data Type Combo box: based on lpBsr->OpenState.idDataType
//**************************************************************************/
void FillDataType(HWND hDlg)
//**************************************************************************/
{
	HWND hWnd;

	hWnd = IMAGETYPEWND(hDlg);
	CheckComboItem(hWnd, IDC_BSR_DATATYPE,
			IDC_FIRSTDATATYPE, IDC_LASTDATATYPE, lpBsr->OpenState.idDataType);
}

//**************************************************************************/
// fills Open Mode Combo box: based on lpBsr->OpenState.OpenMode
//**************************************************************************/
void FillOpenMode(HWND hDlg)
//**************************************************************************/
{
	CheckComboItem(OPENMODEWINDOW(hDlg),
			IDC_BSR_OPENMODE,
			IDC_BSR_FIRSTOPENMODE, 
			IDC_BSR_LASTOPENMODE,
			lpBsr->OpenState.OpenMode+IDC_BSR_FIRSTOPENMODE);
}

//**************************************************************************/
//	Enables the proper data types based on file type
//**************************************************************************/
void EnableDataTypes(HWND hDlg)
//**************************************************************************/
{
	HWND hControl, hWnd;
	int idFile, i, idOkType;
	BOOL bEnable, bNewDataType;

	hWnd = IMAGETYPEWND(hDlg);
	hControl = GetDlgItem(hWnd, IDC_BSR_DATATYPE);

    if( bImported )
    {
	    idFile = Bound(lpBsr->OpenState.idFileType, IDT_FIRSTIMPORTFILETYPE, IDT_LASTIMPORTFILETYPE );
        idFile = (( idFile - IDT_FIRSTIMPORTFILETYPE ) < ( IDT_FIRSTIMPORTREAL - IDT_FIRSTIMPORTFILETYPE )) ? 0 : 1;
	    bNewDataType = FALSE;
	    idOkType = -1;

	    for (i=IDC_FIRSTDATATYPE; i<=IDC_LASTDATATYPE; i++)
	    {
		    bEnable = ImportReaders[idFile].bTypes[i-IDC_FIRSTDATATYPE];
		    ControlEnable(hWnd, i, bEnable);
		    if (bEnable && (!bNewDataType || (bNewDataType && idOkType==-1)))
			    idOkType = i;
		    if (!bEnable && i == lpBsr->OpenState.idDataType)
			    bNewDataType = TRUE;
	    }
	    if (bNewDataType && idOkType != 0)
	    {
		    lpBsr->OpenState.idDataType = idOkType;
		    CheckComboItem(hWnd, IDC_BSR_DATATYPE,
			    IDC_FIRSTDATATYPE, IDC_LASTDATATYPE, lpBsr->OpenState.idDataType);
	    }
    }
    else
    {
	    idFile = Bound(lpBsr->OpenState.idFileType, IDT_FIRSTFILETYPE, IDT_LASTFILETYPE);
	    idFile = idFile - IDT_FIRSTFILETYPE;
	    bNewDataType = FALSE;
	    idOkType = -1;
	    for (i=IDC_FIRSTDATATYPE; i<=IDC_LASTDATATYPE; i++)
	    {
		    bEnable = Readers[idFile].bTypes[i-IDC_FIRSTDATATYPE];
		    ControlEnable(hWnd, i, bEnable);
		    if (bEnable && (!bNewDataType || (bNewDataType && idOkType==-1)))
			    idOkType = i;
		    if (!bEnable && i == lpBsr->OpenState.idDataType)
			    bNewDataType = TRUE;
	    }
	    if (bNewDataType && idOkType != 0)
	    {
		    lpBsr->OpenState.idDataType = idOkType;
		    CheckComboItem(hWnd, IDC_BSR_DATATYPE,
			    IDC_FIRSTDATATYPE, IDC_LASTDATATYPE, lpBsr->OpenState.idDataType);
	    }
    }
}

//**************************************************************************/
//	Enables/Disables the compression button based on file type
//**************************************************************************/
void EnableCompress(HWND hDlg)
//**************************************************************************/
{
    if( bImported )
    {
	    ControlEnable(hDlg, IDC_BSR_COMPRESS,
		    ImportReaders[((( lpBsr->OpenState.idFileType - IDT_FIRSTIMPORTFILETYPE ) < ( IDT_FIRSTIMPORTREAL - IDT_FIRSTIMPORTFILETYPE )) ? 0 : 1 )].bCompressed );
    }
    else
    {
	    ControlEnable(hDlg, IDC_BSR_COMPRESS,
		    Readers[lpBsr->OpenState.idFileType-IDT_FIRSTFILETYPE].bCompressed);
    }
}

//**************************************************************************/
//	Enables/Disables the OK button based on number of files selected
//**************************************************************************/
void EnableOpenSaveButton(HWND hDlg)
//**************************************************************************/
{
	HWND hFileCombo;
	int num;
	BOOL bEnable;
	FNAME text;
	WORD wChar;

	hFileCombo = FILELIST(hDlg);
	num = ParseListBoxMessage(lpBsr->OpenState.bMultiListBox, 
				hFileCombo, LB_GETSELCOUNT, 0, 0L);
	if (lpBsr->OpenState.bSaving || EXTENDED)
		bEnable = (num <=1) || (num == LB_ERR);
	else
		bEnable = TRUE;
   	if (num==0)
		if (GetDlgItemText(FILEWINDOW(hDlg),IDC_BSR_FILENAME,text, sizeof(text)))
		{
			if( IsDBCSLeadByte(( BYTE )*text ))
				wChar = *(( LPWORD )text );
			else
				wChar = *text;

			// is it an obviously invalid string
			if( wChar == _T('\0') )
				bEnable = FALSE;
			else 
			if( wChar == _T('.') )
				bEnable = FALSE;
			else 
			if (lStrChr(text, _T('*')))
				bEnable = FALSE;
			else 
			if (lStrChr(text, _T(',')))
				bEnable = FALSE;
		}
		else
			bEnable = FALSE;
	ControlEnable(hDlg, IDC_OK, bEnable);
}

//**************************************************************************/
//	Enables/Disables the info button based whether any files/albums
//		are selected
//**************************************************************************/
void EnableInfoButton(HWND hDlg)
//**************************************************************************/
{
	int num;
	BOOL bEnable;

	// check file list box
	num = ParseListBoxMessage(lpBsr->OpenState.bMultiListBox,
				FILELIST(hDlg), LB_GETSELCOUNT, 0, 0L);
	bEnable = (num != 0 && num != LB_ERR);
	// check album list box
	if (!bEnable && ALBMODE)
	{
		num = ListBox_GetCurSel(ALBLIST(hDlg));
		bEnable = (num != LB_ERR);
	}
	ControlEnable(hDlg, IDC_STATUSINFO, bEnable);
    EnableMenuItem( GetMenu( hDlg ), IDC_STATUSINFO, bEnable ? MF_ENABLED : MF_GRAYED );
}

//**************************************************************************/
// fills path text box
// 		Directory mode:
//			based on current directory.
//		Album mode:
//			name of current album. (based on lpBsr->OpenState.szAlbum.)
//**************************************************************************/
void SetPathText(HWND hDlg)
//**************************************************************************/
{
	FNAME szPath;
	LPTSTR  lpString;

	if (DIRMODE)
	{
		if (!CURRENTDIR(szPath, MAX_FNAME_LEN))
			lstrcpy(szPath, _T(""));
		Lowercase(szPath);
		SetDlgItemText(hDlg, IDC_BSR_PATH_TEXT, (LPTSTR)szPath);
	}
	else	// album mode
	{
		if( lpBsr->OpenState.szAlbum[0] == _T('\0') )	// no album
		{
			if (AstralStr(IDS_NOALB, &lpString))
				SetDlgItemText(hDlg, IDC_BSR_PATH_TEXT, lpString);
			else
				SetDlgItemText(hDlg, IDC_BSR_PATH_TEXT, _T("----"));
		}
		else
			SetDlgItemText(hDlg, IDC_BSR_PATH_TEXT, (LPTSTR)lpBsr->OpenState.szAlbum);
	}
}

//**************************************************************************/
// fills drive size text box
// 		Directory mode:
//			based on current directory.
//**************************************************************************/
void SetDriveSizeText(HWND hDlg)
//**************************************************************************/
{
   	int drive;
	FNAME szString;

	drive = CURRENTDRIVE;
	hDlg = DIRWINDOW(hDlg);
	SetDlgItemText(hDlg, IDC_BSR_DRIVESIZE, DriveSize(drive,szString) );
}

//**************************************************************************/
// fills file name text box
// if !bSaveSel then all items are deselected from file list box
//**************************************************************************/
void SetFileNameText(HWND hDlg, LPTSTR lpString, BOOL bSaveSel)
//**************************************************************************/
{
	lpBsr->bFileNameChange = bSaveSel;
	SetDlgItemText(FILEWINDOW(hDlg),IDC_BSR_FILENAME, lpString);
	lpBsr->bFileNameChange = FALSE;
}

//**************************************************************************/
//	Sets the file size, date and time (if possible), based on file list box.
//	Returns TRUE if file size is displayed.
//**************************************************************************/
BOOL SetFileListInfo(HWND hDlg)
//**************************************************************************/
{
	UINT32 size, fsize;
	ULONG time;
	struct tm *tminfo;
	int num;
	STRING	szSize, szDate, szTime;
	FNAME szFile;
	FILELOC File;
	BOOL ret, okVolume;
	LPTSTR lpBlank;
	ENTRY entry;
	LPCTHMNAIL lpThm;

	ret = FALSE;
	num = OpenFileList(hDlg);
	// initialize strings
	if (!AstralStr(IDS_BSR_BLANK, &lpBlank))
	{
		lstrcpy(szFile, _T("-----"));
		lpBlank = szFile;
	}
	lstrcpy(szSize, lpBlank);
	lstrcpy(szDate, lpBlank);
	lstrcpy(szTime, lpBlank);
	if (!num)
		goto Done;
	if (num == 1)
	{	// get size date and time
		if (!GetNextFile(&File))
			goto Done;
		if (ALBMODE)
		{
			BSRInvalidateDriveIDs();
			okVolume = VolumeCheck(&File, NULL);
		}
		else 
			okVolume = TRUE;
		if (!okVolume || !SafeFileStats(File.szFile,&size, NULL, &time, NULL))
		{
			if (FindEntry(&File, &entry, TRUE, ALBMODE))
			{
				lpThm = CTHMOpen(&entry);
				if (lpThm)
				{
					size = lpThm->thm.info.fileSize;
					time = lpThm->thm.info.time;
					CTHMClose(lpThm);
				}
				else
					goto Done;
			}
			else
				goto Done;
		}
		ret = TRUE;
		SizeToAscii(size, szSize);
		tminfo = localtime((PLONG)&time);
		if (tminfo)
		{
			FormatDate(tminfo, szDate);
			FormatTime(tminfo, szTime);
		}
	}
	else // more than one, get size only
	{
		size = 0;
		ret = TRUE;
		while (GetNextFile(&File))
		{
			if (ALBMODE)
			{
				BSRInvalidateDriveIDs();
				okVolume = VolumeCheck(&File, NULL);
			}
			else 
				okVolume = TRUE;
			if (okVolume && SafeFileStats(File.szFile,&fsize, NULL, NULL, NULL))
				size += fsize;
			else if (FindEntry(&File, &entry, TRUE, ALBMODE))	// try thumbnails
			{
			 	lpThm = CTHMOpen(&entry);
				if (lpThm)
				{
					size += lpThm->thm.info.fileSize;
					CTHMClose(lpThm);
				}
			}
		}
		SizeToAscii(size, szSize);
	}
Done:
	SetDlgItemText(hDlg, IDC_BSR_FILESIZE, (LPTSTR)szSize);
	SetDlgItemText(hDlg, IDC_BSR_FILEDATE, (LPTSTR)szDate);
	SetDlgItemText(hDlg, IDC_BSR_FILETIME, (LPTSTR)szTime);
	return(ret);
}

//************************************************************************/
BOOL HandleDrives(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify)
//************************************************************************/
{
	WORD index;
	int oldDrive, drive;
	HWND hCombo;
	FNAME szString;

	if (codeNotify != CBN_SELCHANGE)
		return(FALSE);

	// Handle selection changed
	hCombo = GetDlgItem(DIRWINDOW(hDlg), IDC_BSR_DRIVE);
	index = ComboBox_GetCurSel(hCombo);
	if (index == LB_ERR)
		return(FALSE);
	ComboBox_GetLBText(hCombo, index, szString);

	// change drive
	oldDrive = CURRENTDRIVE;

    if( IsDBCSLeadByte(( BYTE )*szString ))
        drive = ( int )( *(( LPWORD )szString ) - _T('a') );
    else
    	drive = szString[0] - _T('a');			 

	if (!ChangeDrive(drive))
	{
		// error changing drive, leave as is
		CHDRIVE(oldDrive);		// bad cwd, reset drive
		FillDrivesCombo(hDlg);
		return(FALSE);
	}
	// check validity of new drive
	if (!CurrentDirEx( drive, szString, sizeof(szString)))
	{
		CHDRIVE(oldDrive);		// bad cwd, reset drive
		FillDrivesCombo(hDlg);
	}	
	if (oldDrive != CURRENTDRIVE)
		lpBsr->OpenState.bNavigated = TRUE;
	// special case photo_cd
	CheckGotoPCD();
	// update the items
	FillItems(hDlg, YES, NO);
	return(FALSE);
}

//************************************************************************/
BOOL HandleDirectories(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify)
//************************************************************************/
{
	WORD index;
	int i;
	ITEMID type;
	HWND hCombo;
	FNAME szDir, szFile;   

	switch (codeNotify)
	{
		case LBN_DBLCLK:
		 	hCombo = GetDlgItem(DIRWINDOW(hDlg), IDC_BSR_DIRECTORIES);
			index = ListBox_GetCurSel(hCombo);
			if (index == LB_ERR)
				return(FALSE);
			ListBox_GetText(hCombo,  index, (long)(LPTSTR)szDir);
			// determine the new curent dir
	  		type = ExtractStringID(szDir);
			if ( type == IDC_PATHICON_ACTIVE )
				return(FALSE);		// already current dir
			// build new path
			*szFile = _T('\0');
			for ( i=0; i<=index; i++ )
			{
				ListBox_GetText( hCombo, i,	(long)(LPTSTR)szDir);
				type = ExtractStringID( szDir );
				if ( type == IDC_PATHICON_CLOSED && i != index )
					continue;
				lstrcat( szFile, SkipSpaces(szDir) );
				if ( type != IDC_PATHICON_ROOT && i != index )
					lstrcat( szFile, _T("\\") );
			}
			CHDIR(szFile);
			BSRInvalidateDriveIDs();
			lpBsr->OpenState.bNavigated = TRUE;
			FillItems(hDlg, YES, NO);
			return(FALSE);
		break;	// case LBN_DBLCLK
		case LBN_SELCHANGE:
			hCombo = GetDlgItem(DIRWINDOW(hDlg), IDC_BSR_DIRECTORIES);
			// Handle selection changed
			index = ListBox_GetCurSel(hCombo);
			if (index == LB_ERR)
				return(FALSE);
			ListBox_GetText(hCombo,  index, (long)(LPTSTR)szDir);
			// determine the new file entry
			type = ExtractStringID(szDir);
		   	if ( type == IDC_PATHICON_OPEN )
				i = 0; 		// Must start building the path from the root
			else if ( type == IDC_PATHICON_ACTIVE )
				i = 9999;  	// Don't build any path - we're there
			else
				i = index; // OK to build a relative path
			szFile[0] = _T('\0');
			for ( ; i<=index; i++ )
			{
				ListBox_GetText( hCombo, i, (long)(LPTSTR)szDir);
				type = ExtractStringID( szDir );
				if ( type == IDC_PATHICON_CLOSED && i != index )
					continue;
				lstrcat( szFile, SkipSpaces(szDir) );
				if ( type != IDC_PATHICON_ROOT )
					lstrcat( szFile, _T("\\") );
			}
			// new file name text
			SetUpSearchSpec(hDlg, szFile);
			HandleFileName(hDlg, 0, NULL, EN_CHANGE);
			return(FALSE);
		break; // case  LBN_SELCHANGE
		default:
			return(FALSE);
		break;
	} // switch
	return(FALSE);
}

//************************************************************************/
//	Album list Box
//************************************************************************/
BOOL HandleAlbumList(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify)
//************************************************************************/
{
	WORD index;
	int i;
	ITEMID type;
	HWND hCombo;
	FNAME szAlbum;

	switch (codeNotify)
	{
		case LBN_DBLCLK:
		 	hCombo = ALBLIST(hDlg);
			index = ListBox_GetCurSel(hCombo);
			if (index == LB_ERR)
				return(FALSE);
			// get the album
			ListBox_GetText(hCombo,  index, (long)(LPTSTR)szAlbum);
			// determine the new curent dir
	  		type = ExtractStringID(szAlbum);
			if ( type == IDC_ICON_ALBUMOPEN)
			{
				FillItems(hDlg, NO, YES);
				return(FALSE);		// already current dir
			}
			SetWindowRedraw (hCombo, TRUE);
			// Make it current
			lstrcpy (lpBsr->OpenState.szAlbum, szAlbum);
			// set it to open
			CombineStringID(szAlbum, IDC_ICON_ALBUMOPEN);
			ChangeListBoxItem(hCombo, index, (LPTSTR)szAlbum);
			// set all other items to closed
			i = ListBox_GetCount(hCombo);
			for (i--; i>=0; i--)
			{
				if (ListBox_GetText(hCombo, i, (long)(LPTSTR)szAlbum) != LB_ERR)
				{
					type = ExtractStringID(szAlbum);
					if (i != index && type != IDC_ICON_ALBUMCLOSED)
					{
						CombineStringID(szAlbum, IDC_ICON_ALBUMCLOSED);
						ChangeListBoxItem(hCombo, i, (LPTSTR)szAlbum);
					}	// if
				}	// if
			} // for
			
			// reselect item
			ListBox_SetCurSel(hCombo, index);
			SetWindowRedraw (hCombo, TRUE);
			SetPathText(hDlg);	
			FillItems(hDlg, NO, NO);
			return(FALSE);
		break; //LBN_DBLCLK
		case LBN_SELCHANGE:
			// just update text box
		 	hCombo = ALBLIST(hDlg);
			index = ListBox_GetCurSel(hCombo);
			if (index == LB_ERR)
				return(FALSE);
			ListBox_GetText(hCombo,  index, (long)(LPTSTR)szAlbum);
			// new file name text
	  		type = ExtractStringID(szAlbum);
			SetUpSearchSpec(hDlg, szAlbum);
			HandleFileName(hDlg, 0, NULL, EN_CHANGE);
			EnableInfoButton(hDlg);
			return(FALSE);
		break; // case  LBN_SELCHANGE
			default:
			return(FALSE);
		break;
	} // switch
	return(FALSE);
}

//************************************************************************/
//	Album Combo Box
//************************************************************************/
BOOL HandleAlbumCombo(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify)
//************************************************************************/
{
	STRING szAlbum;
	HWND hCombo;
	WORD index;
	int wMsg;

	wMsg = codeNotify;
	if (wMsg != CBN_SELCHANGE)
		return(0);
	hCombo = GetDlgItem(DIRWINDOW(hDlg), IDC_BSR_ALBUMCOMBO);
	if (!hCombo)
		return(0);
	index = ComboBox_GetCurSel(hCombo);
	if (index == CB_ERR)
		return(0);
	if (ComboBox_GetLBText(hCombo, index, szAlbum) == CB_ERR)
		return(0);
	lstrcpy(lpBsr->OpenState.szAlbum, szAlbum);
	return(0);
}

//************************************************************************/
// 	Should be called instead of SendMessage whenever File List selection is
//	Being changed (so it can deselct directories or albums.)
//	(One exception is when desecting all).
//	Call with LB_SETSEL or LB_SELITEMRANGE only.
//	hDlg is the Main Dialog!
//************************************************************************/
BOOL ChangeFileSel(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
//************************************************************************/
{
	HWND hBox;
	BOOL bSel;

	hBox = FILELIST(hDlg);
	switch(uMsg)
	{
		case LB_SETSEL:
		case LB_SELITEMRANGE:
			bSel = wParam;
		break;
		default:
			return(LB_ERR);
		break;
	}
	ParseListBoxMessage(lpBsr->OpenState.bMultiListBox, hBox, uMsg, wParam, lParam);
		// deselect directory or album
	if (bSel)
		ListBox_SetCurSel(ALBMODE ? ALBLIST(hDlg) : DIRLIST(hDlg), -1);
	EnableOpenSaveButton(hDlg);
	EnableInfoButton(hDlg);
	return (bSel);
}

//************************************************************************/
BOOL HandleFileList(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify)
//************************************************************************/
{
	int len, num;
	FNAME szFile, szFileName;
	LPTSTR lpFileName;
	HWND hDir;

	switch (codeNotify)
	{
		case LBN_DBLCLK:
			FORWARD_WM_COMMAND(hDlg, IDOK, 0, 0, SendMessage);
			return(0);
		break;
		case LBN_SELCHANGE:
		case LBN_SELCANCEL:
			EnableOpenSaveButton(hDlg);
			EnableInfoButton(hDlg);

			//build up file name
			num = OpenFileList(hDlg);
			if (!num)	// no selected items, reset file text
			{	
				SetUpSearchSpec(hDlg, NULL);
				SetFileListInfo(hDlg);
				return(FALSE);
			}

			len = sizeof(szFileName) - 2;
			*szFileName = _T('\0');
			while (GetNextFileName(szFile) && len > 0)
			{
				lpFileName = filename(szFile);
				szFile[len] = _T('\0');
				len -= lstrlen(lpFileName);
				if (len>0) 
				{
					lstrcat(szFileName, lpFileName);
					lstrcat(szFileName, _T(","));
					len--;
				}
			}
			StripTrailer(szFileName, _T(','));

			// Set File Name text box
			SetFileNameText(hDlg, szFileName, TRUE);

			// sel file date/time etc
			SetFileListInfo(hDlg);

			// clear album selection
			hDir = ALBMODE ? ALBLIST(hDlg): DIRLIST(hDlg);
			if (hDir)
				ListBox_SetCurSel(hDir, -1);
			return(FALSE);
		break;
		default:
			return(FALSE);
		break;
	}
	return(FALSE);
}

//************************************************************************/
BOOL HandleFileType(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify)
//************************************************************************/
{
	WORD type = HandleCombo(FILEWINDOW(hDlg),id,codeNotify);
	if (!type)
		return(FALSE);
	lpBsr->OpenState.idFileType = type;

    if( type == IDT_ALLFILES || type == IDT_ALLIMAGES || type == IDT_ALLIMPORTIMAGES )
	{
		lstrcpy(lpBsr->OpenState.szSearchSpec, _T("*.*"));
		SetFileNameText(hDlg, _T(""), TRUE);
	}

	SetUpSearchSpec(hDlg, NULL);
	// special case photo_cd
	if (CheckGotoPCD())
		FillItems(hDlg, YES,NO);
	else if (SetDirectoryFromDef(hDlg))
		FillItems(hDlg, YES, NO);
	else
		FillFileList(hDlg, NO);
	EnableDataTypes(hDlg);
	EnableCompress(hDlg);
	return(FALSE);
}

//************************************************************************/
BOOL HandleDataType(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify)
//************************************************************************/
{
	WORD type;

	type = HandleCombo(IMAGETYPEWND(hDlg),id,codeNotify);
	if (!type)
		return(FALSE);
	lpBsr->OpenState.idDataType = type;
	return(FALSE);
}

//************************************************************************/
BOOL HandleOpenMode(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify)
//************************************************************************/
{
	WORD type;

	type = HandleCombo(OPENMODEWINDOW(hDlg),id,codeNotify);
	if (!type)
		return(FALSE);
	lpBsr->OpenState.OpenMode = (OPEN_MODE)(type-IDC_BSR_FIRSTOPENMODE);
	return(FALSE);
}

//************************************************************************/
BOOL HandleFileName(HWND hDlg, INT id, HWND hDlgCtl, UINT codeNotify)
//************************************************************************/
{
	if (codeNotify != EN_CHANGE)
		return(FALSE);
	// text changed... clear list box

	if (!lpBsr->bFileNameChange)
	{
		ChangeFileSel(hDlg, LB_SETSEL, FALSE, -1L);
		SetFileListInfo(hDlg);
	}
	return(FALSE);
}

//************************************************************************/
// Handles WM_MEASUREITEM Messages to FILELIST box
//************************************************************************/
BOOL FileListMeasure(HWND hDlg, LPMEASUREITEMSTRUCT lpMeasure)
//************************************************************************/
{
	HWND hList;
	HFONT hFont, hOldFont;
	HDC hDC;
	TEXTMETRIC tm;

	hList = GetDlgItem(TEXTFILELISTWINDOW(hDlg), IDC_BSR_FILELIST);
	if ( !(hFont = GetWindowFont(hList)) )
		hFont = ( HFONT )GetStockObject( GetSystemMetrics( SM_DBCSENABLED ) ? SYSTEM_FONT:ANSI_VAR_FONT );
	hDC = GetDC(hList);
	hOldFont = (HFONT)SelectObject(hDC, hFont);
	GetTextMetrics(hDC, &tm);
	SelectObject(hDC, hOldFont);
	ReleaseDC(hList, hDC);
	lpMeasure->itemHeight = tm.tmHeight;
	// Changing the width never seems to do anything for list boxes!!!!
	lpMeasure->itemWidth = 50; // doesn't matter
	return(TRUE);
}

//************************************************************************/
// Handles WM_DRAW Messages to FILELIST box
//************************************************************************/
BOOL FileListDraw(HWND hDlg, LPDRAWITEMSTRUCT lpDraw)
//************************************************************************/
{
	HWND hList;
	HDC hDC;
	RECT rect;
	FENTRY szString;
	BOOL bHilighted, bFocus;
	HBRUSH hBrush;
	int iOldMode;
	COLORREF lOldColor;

	if ( lpDraw->itemAction == ODA_FOCUS )
	{
		DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
		return(TRUE);
	}
	hDC = lpDraw->hDC;
	rect = lpDraw->rcItem;
	hList = FILELIST(hDlg);

	// get string
	ListBox_GetText( hList,  lpDraw->itemID, (long)((LPTSTR)szString) );
#ifndef WIN32
	AnsiLower(szString);
#endif
	// GET FILE NAME PART
	LBEntryToFileName(szString);
	bHilighted = (lpDraw->itemState & (ODS_SELECTED | ODS_CHECKED));
	bFocus = (lpDraw->itemState & ODS_FOCUS);
	// Draw the box interior
	hBrush = CreateSolidBrush(
	GetSysColor( bHilighted ? COLOR_HIGHLIGHT : COLOR_WINDOW ) );
	FillRect( hDC, &rect, hBrush );
	DeleteObject( hBrush );
	// Give some leading space
	rect.left += 2;
	// Draw the text
	iOldMode = SetBkMode( hDC, TRANSPARENT );
	lOldColor = SetTextColor( hDC,
			GetSysColor( bHilighted ? COLOR_HIGHLIGHTTEXT :	COLOR_WINDOWTEXT));
	DrawText( hDC, szString, -1, &rect,
			DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	SetBkMode( hDC, iOldMode );
	SetTextColor( hDC, lOldColor );
	if ( bFocus )
		DrawFocusRect( lpDraw->hDC, &lpDraw->rcItem );
	return(TRUE);
}

//************************************************************************/
//***************************STATIC ROUTINES******************************/
//************************************************************************/

/***********************************************************************/
LOCAL LPTSTR RemoveDots( LPTSTR lpFileName )
/***********************************************************************/
{
	WORD    wChar;
	LPTSTR	lpFile = lpFileName;
    LPVOID  lp;

	lp = lpFile;

	while( *lpFile )
	{
        BOOL fDBCS = IsDBCSLeadByte(( BYTE )*lpFile );
        if( fDBCS )
            wChar = *(( LPWORD )lpFile );
        else
            wChar = *lpFile;

        lpFile = MyCharNext( lpFile );

		if( wChar != _T('.') )
        {
            if( fDBCS )
    			*(( LPWORD )lp ) = wChar;
            else
                *(( LPBYTE )lp ) = LOBYTE( wChar );
            lp = MyCharNext(( LPTSTR )lp );
        }
	}
	*(( LPTSTR )lp ) = _T('\0');
	return( lpFileName );
}

/************************************************************************/
LOCAL WORD DriveName( int iDrive, LPTSTR lpName )
/************************************************************************/
{
	STRING	szString;
	WORD	wType;
	DWORD	dwUniversalNameInfoSize;
	UNIVERSAL_NAME_INFO	UniversalNameInfo;

	*lpName = _T('a') + iDrive;
	*MyCharNextN( lpName, 1 ) = _T(':');
	*MyCharNextN( lpName, 2 ) = _T('\0');

	wType = BSRGetDriveType(iDrive);
	if (!Browser.UseVolumeLabels)
		return(wType);
		
//	lstrcpy( szString, lpName );
//	lstrcat( szString, _T("\\*.*") ); /* wildcard search of root dir */

//	lstrcat( lpName, _T(" [") );
	if (!wType)
		return(wType);
	if ( wType == DRIVE_REMOTE )
		{
		dwUniversalNameInfoSize = sizeof( szString );
		UniversalNameInfo.lpUniversalName = szString;
		if( WNetGetUniversalName( lpName, UNIVERSAL_NAME_INFO_LEVEL, 
			&UniversalNameInfo, &dwUniversalNameInfoSize ) == NO_ERROR )
			{
			lstrcat( lpName, _T(" ") );
			lstrcat( lpName, UniversalNameInfo.lpUniversalName );
			}
		else
			{
			lstrcat( lpName, _T(" ") );
			LPTSTR lp;
			AstralStr( IDS_UNKNOWN, &lp );
			lstrcat( lpName, lp );
			}
		}

	return( wType );
}


/************************************************************************/
LOCAL void AddSpaces(LPTSTR lpString,int iCount )
/************************************************************************/
{
	while ( --iCount >= 0 )
		*lpString++ = _T(' ');
	*lpString = _T('\0');
}

//***********************************************************************/
// Returns the index of the item in the owner draw combo with that id
// returns -1 if none found.
//***********************************************************************/
LOCAL int ODCFindItem(HWND hCombo,int id)
/***********************************************************************/
{
	STRING szItem;
	int i, numItems;

	numItems = ComboBox_GetCount(hCombo);
	for (i=0; i<numItems;i++)
	{
		ComboBox_GetLBText( hCombo,i, szItem);
	 	if(ExtractStringID(szItem) == id)
			return(i);
	}
	return(-1);
}

//***********************************************************************/
// 	Fills the given filelist with items in the album matching the spec
//***********************************************************************/
LOCAL void fill_album_filelist(LPDBF album, LPTSTR lpSpec, HWND hFileList)
//***********************************************************************/
{
	FENTRY szFileEntry;
	FNAME szSpec;
	int i;
	ENTRY entry;
	WORD wFirst, wNext;
	LPTSTR lp;
		
	*szFileEntry = _T('\0');
	// adjust search spec for albums
	lstrcpy(szSpec, lpSpec);
	StripTrailer(szSpec, _T(' '));

	i = MBStrlen( szSpec );
	if( i > 1 )
	{
		lp = MyCharNextN( szSpec, i-1 );
	    if( IsDBCSLeadByte(( BYTE )*lp ))
			wNext = *(( LPWORD )lp );
		else
			wNext = *lp;

		lp = MyCharNextN( szSpec, i-2 );
	    if( IsDBCSLeadByte(( BYTE )*lp ))
			wFirst = *(( LPWORD )lp );
		else
			wFirst = *lp;

		if( wFirst == _T('.') && wNext == _T('*') )
			*lp = _T('\0');		// szSpec[i-2]
	}

	// fill list with all entries that match
	DBFGetEntryNum(album, -1, &entry);
	while(DBFGetNextEntry(album, &entry))
		if (MatchWildcard((LPTSTR)filename(entry.File.szFile), (LPTSTR)szSpec, MW_CASE))
		{
			FileLocationToLBEntry(&entry.File, szFileEntry);
			ListBox_AddString(hFileList, szFileEntry);
		}
}

//***********************************************************************/
// 	Fills the given filelist with items in the direcory matching the spec
//***********************************************************************/
LOCAL void fill_dir_filelist(HWND hFileList, LPTSTR lpDir, LPTSTR lpSpec)
//***********************************************************************/
{
	FINDDATA file;
	FENTRY szFileEntry;
	FILELOC File;
	int retc;
	
	GetDriveVolume(lpDir, &File.volume, YES);
	*File.szFile = _T('\0');

	retc = Dos_FindFirst(lpSpec, ATTRIB_NORMAL | ATTRIB_READONLY, &file);
	while(retc)
	{
		lstrcpy(File.szFile, lpDir);	
		lstrcat(File.szFile, file.szName);
		FileLocationToLBEntry(&File, szFileEntry);
		ListBox_AddString(hFileList, szFileEntry);
		retc = Dos_FindNext(&file);
	}
}

//***********************************************************************
// Special case photo cd will go to directory photo_cd|images
// Returns TRUE if the dir was changed.
//***********************************************************************
LOCAL BOOL CheckGotoPCD(void)
//***********************************************************************
{
	FNAME szPath, szNewPath;
	int drive;
	UINT errmode;
	
	if (ALBMODE)
		return(FALSE);
	if (lpBsr->OpenState.idFileType != IDT_PHOTOCD)
		return(FALSE);
	drive = CURRENTDRIVE;
	if (BSRGetDriveType(drive) != DRIVE_CDROM)
		return(FALSE);

	errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
	if (!CURRENTDIR(szPath, sizeof(szPath)))
		lstrcpy(szPath, _T(""));
	SetErrorMode(errmode);
	
	// usethe photo CD directory if it exists
	lstrcpy(szNewPath, _T("\\photo_cd\\images"));
	MakeFullPath(szNewPath);
	if (!IsDirectory(szNewPath))
		return(FALSE);
	// already there??
	if (StringsEqual(szNewPath, szPath))
		return(FALSE);
    // change it
	CHDIR(szNewPath);
   	return(TRUE);
}
