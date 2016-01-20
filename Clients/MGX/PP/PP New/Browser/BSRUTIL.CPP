// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

// utility functions for Image Browser dlg

#include "pp.h"
#include <time.h>
#include <dos.h>
#include <ctype.h>
#include "id.h"
#include "bsrtypes.h"
#include "browser.h"
#include "bsrid.h"
#include "bsrutil.h"
#include "dbfmgr.h"
#include "cthmmgr.h"
#include "pcdui.h"

#define MAX_SEL_ITEMS 400
#define NUM_DRIVES 26

//**************************Externs******************************************/
extern LPBSRSTATE lpBsr;
extern BOOL bImported;

//****************************Global Vars***************************************/
//****************************Static Vars***************************************/
LOCAL LONG getVolLabel(int drive);


//************************************************************************/
//************************************************************************/
//	ACCESS AND USE OF ALBUM LIST
//************************************************************************/
//************************************************************************/

//**************************************************************************/
//	Creates the Album list based on currently available albums
//	Returns the number of albums
//**************************************************************************/
int CreateAlbList()
//**************************************************************************/
{
	int num;
	FINDDATA afile;
	LPDBFLL	node;

	if (lpBsr->lpAlbList != NULL)
		FreeAlbList();
	lpBsr->lpAlbList = NULL;

	// get all albums
	num = 0;
	DBFBeginInfo(&afile, ID_ALBUM);
	// get node memory
	node = (LPDBFLL)Alloc(sizeof(DBFLL));
	if(!node)
		return(0);
	while (DBFNextInfo(&(node->dbf), &afile,NO, YES))
	{
		AddToAlbList(node);
		num++;
		node = (LPDBFLL)Alloc(sizeof(DBFLL));
		if(!node)
			return(0);
	}	// while
	FreeUp((LPTR)node);
	return(num);
}

//**************************************************************************/
// sets up album list.  Must be called before any album list routies are used.
// CALL BEFORE CreateAlbList.
//**************************************************************************/
void PrepareAlbList()
//**************************************************************************/
{
	lpBsr->lpAlbList = NULL;
}

//**************************************************************************/
//	Frees the Albums in list
//**************************************************************************/
void FreeAlbList()
//**************************************************************************/
{
	LPDBFLL lpnext;

	FirstAlb();
	while(lpBsr->lpAlbList)
	{

		lpnext = lpBsr->lpAlbList->next;
		DBFClose(&lpBsr->lpAlbList->dbf);
		FreeUp((LPTR)lpBsr->lpAlbList);
		lpBsr->lpAlbList = lpnext;
	}
}

//************************************************************************/
//	Moves album pointer to first album in list.
//	Returns pointer to LPDBFLL structure for first album.
//	Returns NULL if there is no first album.
//	Album List MUST be set up first
//************************************************************************/
LPDBFLL FirstAlb()
{
	LPDBFLL node;

	node = lpBsr->lpAlbList;
	while(node)
	{
		lpBsr->lpAlbList = node;
		node = lpBsr->lpAlbList->previous;
	}
	return(lpBsr->lpAlbList);
}

//************************************************************************/
//	Moves album pointer to next album in list.
// 	Returns the LPDBFLL for that node.
//	Returns NULL if there is no next album.
//	Album List MUST be set up first
//************************************************************************/
LPDBFLL GetNextAlb()
{
	if (!lpBsr->lpAlbList)
		return(NULL);
	if (!lpBsr->lpAlbList->next)
		return (NULL);

	lpBsr->lpAlbList = lpBsr->lpAlbList->next;
	return(lpBsr->lpAlbList);
}

//************************************************************************/
//	Moves album pointer to previous album in list.
// 	Returns the LPDBFLL for that node.
//	Returns NULL if there is no previous album.
//	Album List MUST be set up first
//************************************************************************/
LPDBFLL GetPreviousAlb()
{
	if (!lpBsr->lpAlbList)
		return(NULL);
	if (!lpBsr->lpAlbList->previous)
		return (NULL);

	lpBsr->lpAlbList = lpBsr->lpAlbList->previous;
	return(lpBsr->lpAlbList);
}

//************************************************************************/
// 	Returns the LPDBFLL for current album node.
//	Returns NULL if there are no albums.
//	Album List MUST be set up first
//************************************************************************/
LPDBFLL GetAlb()
{
	if (!lpBsr->lpAlbList)
		return(NULL);
	return(lpBsr->lpAlbList);
}

//************************************************************************/
//	Finds album with specified name.
//	Returns its LPDBFLL or NULL if none was found
//************************************************************************/
LPDBFLL FindAlbum(LPTSTR lpAlbum)
//************************************************************************/
{
	LPDBFLL node;

	node = FirstAlb();
	while(node)
	{
		if(StringsEqual(lpBsr->lpAlbList->dbf.info.szName, lpAlbum))
			return(lpBsr->lpAlbList);
		node = GetNextAlb();
	}
	return(NULL);
}

//************************************************************************/
//	Adds the given dbfll to the list.
// 	NOTE: the memory for the dbfll will be freed by FreeAlbList!
//************************************************************************/
void AddToAlbList(LPDBFLL lpAlb)
{
	LPDBFLL node;
	if (!lpBsr->lpAlbList)
	{
		lpBsr->lpAlbList = lpAlb;
		lpBsr->lpAlbList->previous = NULL;
		lpBsr->lpAlbList->next = NULL;
		return;
	}
	node = lpBsr->lpAlbList->next;
	lpBsr->lpAlbList->next = lpAlb;
	lpAlb->previous = lpBsr->lpAlbList;
	lpAlb->next = node;
	if (node)
		node->previous = lpAlb;
}

//************************************************************************/
//	Removes the current album in the album list from the list.
//	Frees the memory associated with the album.
//************************************************************************/
void RemoveAlbumFromList(void)
//************************************************************************/
{
	LPDBFLL node;

	if (!lpBsr->lpAlbList)
		return;
	if (node = lpBsr->lpAlbList->previous)
		node->next = lpBsr->lpAlbList->next;
	if (lpBsr->lpAlbList->next)
	{
		lpBsr->lpAlbList->next->previous = lpBsr->lpAlbList->previous;
		if (!node)
			node = lpBsr->lpAlbList->next;
	}
	FreeUp((LPTR)lpBsr->lpAlbList);
	lpBsr->lpAlbList = node;
}

//************************************************************************
// Returns the currently selected album.  
// Returns NULL for failure or no selected album.
//************************************************************************
LPDBFLL GetSelectedAlbum(HWND hDlg)
//************************************************************************
{
	FNAME szAlbum;
	HWND hBox;
	int index;
	
	if (DIRMODE)
		return(NULL);
	hBox = ALBLIST(hDlg);
	index = ListBox_GetCurSel(hBox);
	if (index == LB_ERR)
		return(NULL);
	if (ListBox_GetText(hBox, index, (LPTSTR)szAlbum) == LB_ERR)
		return(NULL);
	ExtractStringID(szAlbum);
	// get the album
	return(FindAlbum(szAlbum));
}

//************************************************************************/
//************************************************************************/
//	ACCESS AND USE OF FILE LIST
//************************************************************************/
//************************************************************************/

//************************************************************************/
// Sets up the storage needed to use the file list routines.
//	MUST be called before any of them are used.
//************************************************************************/
void PrepareFileList()
//************************************************************************/
{
	lpBsr->SelBuf = NULL;
	lpBsr->max_sel_items = 2*MAX_SEL_ITEMS;
	while(!lpBsr->SelBuf)
	{
		lpBsr->max_sel_items /= 2;
		lpBsr->SelBuf = (int FAR *)Alloc(lpBsr->max_sel_items * sizeof(int));
	}
	if (lpBsr->max_sel_items < 1)
		lpBsr->SelBuf = NULL;
}

//************************************************************************/
// Frees up the storage needed to used by the file list routines.
//************************************************************************/
void FreeFileList()
//************************************************************************/
{
	if (lpBsr->SelBuf)
		FreeUp((LPTR)lpBsr->SelBuf);
}

//************************************************************************/
// Sets up the data needed to read the list of selected files
// Returns the number of selected files
// sets up so that GetNextFile will give first file
//************************************************************************/
int OpenFileList(HWND hDlg)
//************************************************************************/
{
	lpBsr->hFileCombo = FILELIST(hDlg);
	if (!lpBsr->SelBuf)
	{
		lpBsr->NumSelBufItems = 0;
		return(0);
	}
	lpBsr->NumSelBufItems = ParseListBoxMessage(lpBsr->OpenState.bMultiListBox, 
			lpBsr->hFileCombo,	LB_GETSELITEMS, lpBsr->max_sel_items, (long)lpBsr->SelBuf);
	if (lpBsr->NumSelBufItems == LB_ERR)
		lpBsr->NumSelBufItems = 0;
	lpBsr->CurrentSelBufItem = -1;
	return(lpBsr->NumSelBufItems);
}

//************************************************************************/
LPTSTR GetFirstFileEntry(LPTSTR lpFileEntry)
//************************************************************************/
{
	lpBsr->CurrentSelBufItem = 0;
	if (lpBsr->CurrentSelBufItem >= lpBsr->NumSelBufItems)
		return(NULL);
	if (ListBox_GetText(lpBsr->hFileCombo, lpBsr->SelBuf[lpBsr->CurrentSelBufItem], lpFileEntry) == LB_ERR)
		return (NULL);
	return(lpFileEntry);
}

//************************************************************************/
LPTSTR GetNextFileEntry(LPTSTR lpFileEntry)
//************************************************************************/
{
	lpBsr->CurrentSelBufItem++;
	if (lpBsr->CurrentSelBufItem < 0 )
	{							    
		lpBsr->CurrentSelBufItem = 0;
		return(NULL);
	}
	if (lpBsr->CurrentSelBufItem >= lpBsr->NumSelBufItems)
	{
		lpBsr->CurrentSelBufItem = lpBsr->NumSelBufItems-1;
		return(NULL);
	}
	if (ListBox_GetText(lpBsr->hFileCombo, lpBsr->SelBuf[lpBsr->CurrentSelBufItem], lpFileEntry) == LB_ERR)
		return (NULL);
	return(lpFileEntry);
}

//************************************************************************/
LPTSTR GetCurrentFileEntry(LPTSTR lpFileEntry)
//************************************************************************/
{
	if (lpBsr->CurrentSelBufItem < 0 )
	{
		lpBsr->CurrentSelBufItem = 0;
		return(NULL);
	}
	if (lpBsr->CurrentSelBufItem >= lpBsr->NumSelBufItems)
	{
		lpBsr->CurrentSelBufItem = lpBsr->NumSelBufItems-1;
		return(NULL);
	}
	if (ListBox_GetText(lpBsr->hFileCombo, lpBsr->SelBuf[lpBsr->CurrentSelBufItem], lpFileEntry) == LB_ERR)
		return (NULL);
	return(lpFileEntry);
}

//************************************************************************/
LPTSTR GetPreviousFileEntry(LPTSTR lpFileEntry)
//************************************************************************/
{
	lpBsr->CurrentSelBufItem--;
	if (lpBsr->CurrentSelBufItem < 0 )
	{
		lpBsr->CurrentSelBufItem = 0;
		return(NULL);
	}
	if (lpBsr->CurrentSelBufItem >= lpBsr->NumSelBufItems)
	{
		lpBsr->CurrentSelBufItem = lpBsr->NumSelBufItems-1;
		return(NULL);
	}
	if (ListBox_GetText(lpBsr->hFileCombo, lpBsr->SelBuf[lpBsr->CurrentSelBufItem], lpFileEntry) == LB_ERR)
		return (NULL);
	return(lpFileEntry);
}

//************************************************************************/
LPTSTR GetNumFileEntry(int num, LPTSTR lpFileEntry)
//************************************************************************/
{
	lpBsr->CurrentSelBufItem = num;
	if (lpBsr->CurrentSelBufItem < 0 )
	{
		lpBsr->CurrentSelBufItem = -1;
		return(NULL);
	}
	if (lpBsr->CurrentSelBufItem >= lpBsr->NumSelBufItems)
	{
		lpBsr->CurrentSelBufItem = lpBsr->NumSelBufItems-1;
		return(NULL);
	}
	if (ListBox_GetText(lpBsr->hFileCombo, lpBsr->SelBuf[lpBsr->CurrentSelBufItem], lpFileEntry) == LB_ERR)
		return (NULL);
	return(lpFileEntry);
}

//************************************************************************/
// Gets the first filename out of file list.  Must call OpenFileList first.
// returns: a pointer to lpString, or NULL if there is no file
//************************************************************************/
LPFILELOC GetFirstFile(LPFILELOC lpFileLocation)
//************************************************************************/
{
	FENTRY	szFileEntry;

	if (!GetFirstFileEntry(szFileEntry))
		return(NULL);
	LBEntryToFileLocation(szFileEntry, lpFileLocation);
	return(lpFileLocation);
}

//************************************************************************/
// Gets the next filename out of file list.  Must call OpenFileList first.
// returns: a pointer to lpString, or NULL if there is no next file
//************************************************************************/
LPFILELOC GetNextFile(LPFILELOC lpFileLocation)
//************************************************************************/
{
	FENTRY	szFileEntry;
						  
	if (!GetNextFileEntry(szFileEntry))
		return(NULL);
	LBEntryToFileLocation(szFileEntry, lpFileLocation);
	return(lpFileLocation);
}

//************************************************************************/
// Gets the current filename out of file list.  Must call OpenFileList first.
// returns: a pointer to lpString, or NULL if there is no next file
//************************************************************************/
LPFILELOC GetCurrentFile(LPFILELOC lpFileLocation)
//************************************************************************/
{
	FENTRY	szFileEntry;

	if (!GetCurrentFileEntry(szFileEntry))
		return(NULL);
	LBEntryToFileLocation(szFileEntry, lpFileLocation);
	return(lpFileLocation);
}

//************************************************************************/
// Gets the filename out of file list with the given index.  Must call OpenFileList first.
// returns: a pointer to lpString, or NULL if there is file.
// Use -1 to reset to begining of list.
//************************************************************************/
LPFILELOC GetNumFile(int num, LPFILELOC lpFileLocation)
//************************************************************************/
{
	FENTRY	szFileEntry;

	if (!GetNumFileEntry(num, szFileEntry))
		return(NULL);
	LBEntryToFileLocation(szFileEntry, lpFileLocation);
	return(lpFileLocation);
}

//************************************************************************/
// Gets the previos filename out of file list.  Must call OpenFileList first.
// returns: a pointer to lpString, or NULL if there is no previous file
//************************************************************************/
LPFILELOC GetPreviousFile(LPFILELOC lpFileLocation)
//************************************************************************/
{
	FENTRY	szFileEntry;

	if (!GetPreviousFileEntry(szFileEntry))
		return(NULL);
	LBEntryToFileLocation(szFileEntry, lpFileLocation);
	return(lpFileLocation);
}

//************************************************************************/
// Gets the first filename out of file list.  Must call OpenFileList first.
// returns: a pointer to lpString, or NULL if there is no file
//************************************************************************/
LPTSTR GetFirstFileName(LPTSTR lpString)
//************************************************************************/
{
	FENTRY	szFileEntry;

	if (!GetFirstFileEntry(szFileEntry))
		return(NULL);
	LBEntryToFile(szFileEntry);
	lstrcpy(lpString, szFileEntry);
	return(lpString);
}

//************************************************************************/
// Gets the next filename out of file list.  Must call OpenFileList first.
// returns: a pointer to lpString, or NULL if there is no next file
//************************************************************************/
LPTSTR GetNextFileName(LPTSTR lpString)
//************************************************************************/
{
	FENTRY	szFileEntry;

	if (!GetNextFileEntry(szFileEntry))
		return(NULL);
	LBEntryToFile(szFileEntry);
	lstrcpy(lpString, szFileEntry);
	return(lpString);
}

//************************************************************************/
// Gets the current filename out of file list.  Must call OpenFileList first.
// returns: a pointer to lpString, or NULL if there is no next file
//************************************************************************/
LPTSTR GetCurrentFileName(LPTSTR lpString)
//************************************************************************/
{
	FENTRY	szFileEntry;

	if (!GetCurrentFileEntry(szFileEntry))
		return(NULL);
	LBEntryToFile(szFileEntry);
	lstrcpy(lpString, szFileEntry);
	return(lpString);
}

//************************************************************************/
// Gets the previos filename out of file list.  Must call OpenFileList first.
// returns: a pointer to lpString, or NULL if there is no previous file
//************************************************************************/
LPTSTR GetPreviousFileName(LPTSTR lpString)
//************************************************************************/
{
	FENTRY	szFileEntry;

	if (!GetPreviousFileEntry(szFileEntry))
		return(NULL);
	LBEntryToFile(szFileEntry);
	lstrcpy(lpString, szFileEntry);
	return(lpString);
}

//************************************************************************/
// Must call OpenFileList first.
// returns: TRUE if there is a next file in the list
//************************************************************************/
BOOL IsNextFileName(void)
//************************************************************************/
{
	if (lpBsr->CurrentSelBufItem == -1 && lpBsr->NumSelBufItems > 0)
		return(TRUE);
	if (lpBsr->CurrentSelBufItem+1 >= lpBsr->NumSelBufItems || lpBsr->CurrentSelBufItem < 0)
		return(FALSE);
	return(TRUE);
}

//************************************************************************/
// Must call OpenFileList first.
// returns: TRUE if there is a Previous file in the list
//************************************************************************/
BOOL IsPreviousFileName(void)
//************************************************************************/
{
	if (lpBsr->CurrentSelBufItem-1 >= lpBsr->NumSelBufItems || lpBsr->CurrentSelBufItem < 1)
		return(FALSE);
	return(TRUE);
}

//************************************************************************
// 	Uses the master file list box to create an array of file names.
//	Fills lpBsr->OpenState.lpFileList, lpBsr->OpenState.lpFileListVolumes,
//		lpBsr->OpenState.numFiles.
//	Returns FALSE failure.
// 	Extended Browser aware. (Doesn't get file volumes).
//************************************************************************
BOOL MakeFinalList(HWND hDlg)
//************************************************************************
{
	int i, numfiles;
	DWORD size, element, avail;
	FNAME FAR *lpNames;
	LPVOLUMEID lpVolumes;
	FILELOC File;

	numfiles = OpenFileList(hDlg);
	if (numfiles == 0)
	{
		lpBsr->OpenState.numFiles = 0;	   
		return(FALSE);
	}
	element = sizeof(FNAME);
	if (!EXTENDED)
		element += sizeof(VOLUMEID);
	// calculate size of array
	size = element * numfiles;
	// get memory
	lpNames = (FNAME FAR *)Alloc(size);
	if (!lpNames)
	{
		// try to compromise (should we post an error?)
		avail = AvailableMemory()/(2*element);
		avail *= element;
		size = Min(size,avail);
		lpNames = (FNAME FAR *)Alloc(size);
		if (!lpNames)
		{
			lpBsr->OpenState.numFiles = -1;
			return(FALSE);
		}
		numfiles = size/element;
	}
	lpBsr->OpenState.lpFileList = lpNames;
	if (!EXTENDED)
	{
		lpVolumes = (LPVOLUMEID)(&lpNames[numfiles]);
		lpBsr->OpenState.lpFileListVolumes = lpVolumes;
	}
	else
		lpBsr->OpenState.lpFileListVolumes = NULL;
	// load the strings
	for (i=0; i<numfiles; i++)
	{
		if (!EXTENDED)
		{
			// get name and volume
			if (!GetNextFile(&File))
				break;
			lstrcpy(*(lpNames++),File.szFile);
			*(lpVolumes++) = File.volume; 
		}
		else
		{
			// just get the string
			if (!GetNextFileEntry(File.szFile))
				break;
			lstrcpy(*(lpNames++),File.szFile);
		}
	}
	lpBsr->OpenState.numFiles = i;
	return(TRUE);
}

//************************************************************************
//************************************************************************
//	PACKING/UNPACKING LBENTRY STRINGS
//************************************************************************
//************************************************************************

//************************************************************************/
//	Takes an entry from the file list box.
// 	Turns it into full path and name.
// 	Returns TRUE if sucessful.
// 	Extended Browser aware.
//************************************************************************/
BOOL LBEntryToFile(LPTSTR lpString)
//************************************************************************/
{
	LPTSTR lptr;
	FNAME szName;
	LPTSTR lpvolume;

	if (!lpString)
		return(NULL);

	// handle extended name case		
	if (EXTENDED)
	{
		// extended name
		if (lpBsr->OpenState.lpGetFileProc)
			if((lpBsr->OpenState.lpGetFileProc)(szName, lpString, lpBsr->OpenState.wExtType))
			{
				lstrcpy(lpString, szName);
				return(TRUE);
			}
		lpString[0] = _T('\0');
		return(FALSE);
	}
		
	// strip volume
	lpvolume = lStrChr(lpString, _T(';'));
	if (lpvolume)
		*lpvolume = _T('\0');
	if (lptr = lStrChr(lpString, _T(',')))
		*lptr++ = _T('\0');
	else
		lptr = lpString+lstrlen(lpString);
	lstrcpy(szName, lpString);
	lstrcpy(lpString, lptr);
	lstrcat(lpString, szName);
	return(TRUE);
}

//************************************************************************/
//	Takes an entry from the file list box.
// 	Strips off all but the file name.
// 	Returns TRUE if sucessful.
// 	Extended Browser aware.
//************************************************************************/
BOOL LBEntryToFileName(LPTSTR lpString)
//************************************************************************/
{
	LPTSTR lptr;

	if (!lpString)
		return(FALSE);
	if (EXTENDED)
		return(TRUE);
	if (lptr = lStrChr(lpString, _T(',')))
		*lptr = _T('\0');
	return(TRUE);
}

//************************************************************************/
// Converts a file and path to the form used in the FILELIST box.
// the form is fname,path;volume(part1).volume(pat2)
// 	If there is no path then the current path is used.
// 	lpFile is returned.
//************************************************************************/
LPTSTR FileLocationToLBEntry(LPFILELOC lpFile, LPTSTR lpString)
//************************************************************************/
{
	FENTRY szWorkString;
	LPTSTR lptr, lpStr;
	int space;

	// get file name
	lstrcpy(lpString, lpFile->szFile);
	lpStr = lpString;
	lstrcpy(szWorkString, lpStr);
	lstrcpy(lpStr, lptr=filename(szWorkString));
	// add ,
	space = lstrlen(lpStr);
	lpStr += space++;
	*lpStr++ = _T(',');

	// add path
	if (lptr == (LPTSTR)szWorkString)
	{	//No path, use cwd
		space = MAX_FENTRY_LEN - space - 8;  // leave space for ,index\0
		CURRENTDIR(szWorkString, space);
		FixPath(szWorkString);
		lstrcpy(lpStr, szWorkString);
	}
	else
	{	// use path given
		*lptr = _T('\0');
		lstrcpy(lpStr, szWorkString);
	}
	
	// add ;
	space = lstrlen(lpStr);
	lpStr += space++;
	*lpStr++ = _T(';');
	
	// add volume
	wsprintf( lpStr, "%ld", lpFile->volume.OurMark);
	// add .
	space = lstrlen(lpStr);
	lpStr += space++;
	*lpStr = _T('.'); lpStr = MyCharNext( lpStr );
	wsprintf( lpStr, "%ld", lpFile->volume.VolLabel);
	
	return(lpString);
}

//************************************************************************/
//	Takes an entry from the file list box.
// 	Turns it into full path and name.
// 	Returns TRUE if sucessful.
// 	Extended Browser aware.
//************************************************************************/
BOOL LBEntryToFileLocation(LPTSTR lpString, LPFILELOC lpFile)
//************************************************************************/
{
	LPTSTR lppath, lpvolume, lptr;

	if (!lpString)
		return(FALSE);
		
	// handle extended name case		
	if (EXTENDED)
	{
		// extended name
		CLEARVOLUME(lpFile->volume);
		if (lpBsr->OpenState.lpGetFileProc)
			if((lpBsr->OpenState.lpGetFileProc)(lpFile->szFile, lpString, lpBsr->OpenState.wExtType))
				return(TRUE);
		lpFile->szFile[0] = _T('\0');
		return(FALSE);
	}
		
	// get file path
	lppath = lStrChr(lpString, _T(','));
	// get volume
	lpvolume = lStrChr(lpString, _T(';'));
	
	// get volume
	if (lpvolume)
	{
		lpvolume = MyCharNext( lpvolume );
		*MyCharPrev( lpString, lpvolume ) = _T('\0');
		lptr = lpvolume;
		lpvolume = lStrChr(lptr, _T('.'));
		if (lpvolume)
		{
			lpvolume = MyCharNext( lpvolume );
			*MyCharPrev( lpString, lpvolume ) = _T('\0');
			lpFile->volume.VolLabel = Ascii2Long(lpvolume);
		}	
		lpFile->volume.OurMark = Ascii2Long(lptr);
	}
	else
		CLEARVOLUME(lpFile->volume);
	
	// get path
	if (lppath)
	{
		lppath = MyCharNext( lppath );
		*MyCharPrev( lpString, lppath ) = _T('\0');
		lstrcpy(lpFile->szFile, lppath);
	}
	else
		lpFile->szFile[0] = _T('\0'); 
	// get name
	lstrcat(lpFile->szFile, lpString);
	
	return(TRUE);
}

//************************************************************************
//************************************************************************
//	.INI FILE PATH AND EXTENSION ROUTINES
//************************************************************************
//************************************************************************

//************************************************************************/
//	Given a LOCAL (IDT_) type sets the string to be the extension.
//	DO NOT use this as a real extension unless a 'real' file type is
//		used ie: TIFF, JPG etc.  not  ALLFILES etc.
//************************************************************************/
BOOL LookupExtension(ITEMID idLocalFileType, LPTSTR lpExtension )
/************************************************************************/
{
	LPTSTR lpFileType;

	if( idLocalFileType == IDT_ALLFILES )
		idLocalFileType = IDS_ALLFILES;
	else
	if( idLocalFileType == IDT_ALLIMAGES )
		idLocalFileType = IDS_ALLIMAGES;
	else
	if( idLocalFileType == IDT_ALLIMPORTFILES )
		idLocalFileType = IDS_ALLIMPORTFILES;
	else
	if( idLocalFileType == IDT_ALLIMPORTIMAGES )
		idLocalFileType = IDS_ALLIMPORTIMAGES;
		
	if (!AstralStr(idLocalFileType, &lpFileType))
	{
	    if( bImported )
		    lstrcpy( lpExtension, _T(".AI") );
	    else    
	    	lstrcpy( lpExtension, _T(".TIF") );
	}

	lstrcpy( lpExtension, lpFileType );
	return(YES);
}

//************************************************************************/
// Loads file paths into temp file paths in the .INI
//************************************************************************/
void LoadFileLocations()
/************************************************************************/
{
	FNAME szFileName, szExtension;
	int i;
	int items[] =
		{IDT_PPCOMPOUND, IDT_PP, IDT_TIFF, IDT_TARGA, IDT_GIF, IDT_BMP, \
        IDT_DIB, IDT_PCX, IDT_JPEG, IDT_PHOTOCD, IDT_SCITEX, IDT_AVI,   \
        IDT_PP4, IDT_ALLFILES, IDT_AI, IDT_CDR, IDT_CGM, IDT_DRW, IDT_WMF };

	for ( i=0; i<sizeof(items)/sizeof(int); i++ )
	{
		if ( !LookupExtension( items[i], szExtension ) )
			continue;
		lstrcpy (szExtension, extension(szExtension));
		GetDefaultString(szExtension, _T(""), szFileName, sizeof(FNAME) );
		lstrcat( szExtension, _T("temp") );
		PutDefaultString( szExtension, szFileName );
	}
}

//************************************************************************/
//	gets the default path for a given type	(local IDT_ type)
//************************************************************************/
BOOL GetFileLocation(ITEMID idFileType, LPTSTR lpPath )
//************************************************************************/
{
	FNAME szExtension, szFileName, szTemp;

	if ( !LookupExtension( idFileType, szExtension ) )
		return( NO );

	/* Find out where the user was last, so we can change over to that directory */
	lstrcpy( szTemp, szExtension );
	lstrcat( szTemp, _T("temp") );
	GetDefaultString( szTemp, szExtension, szFileName, sizeof(FNAME) );
	stripfile( szFileName );
	FixPath( szFileName );
	if (idFileType == IDT_PHOTOCD)
		GetPCDLocation(szFileName);
	lstrcat( szFileName, szExtension );
	lstrcpy( lpPath, szFileName );
	return( YES );
}

//************************************************************************/
//	Saves the path into the ini as a temp path.
//	If Browser.OKtoSavePath then also saves as a perminant path.
//************************************************************************/
BOOL SetFileLocation(ITEMID idFileType, LPTSTR lpPath )
//************************************************************************/
{
	FNAME szFileName, szExtension;

	if ( !LookupExtension( idFileType, szExtension ) )
		return( NO );

	/* Whatever disk and directory the user changed to, */
	/* store it away so we come back to the same place next time */
	lstrcpy( szFileName, lpPath );
	stripfile( szFileName );
	FixPath( szFileName );
	lstrcat( szFileName, szExtension );
	if ( Browser.OKtoSavePath )
		PutDefaultString( szExtension, szFileName );
	lstrcat( szExtension, _T("temp") );
	PutDefaultString( szExtension, szFileName );
	return( TRUE );
}

//************************************************************************
//************************************************************************
//	SEARCHING AND USING THUMBNAIL & DBF STRUCTURES
//************************************************************************
//************************************************************************

//************************************************************************/
// 	Returns the entry for the file if an entry exists.
//	Returns NULL otherwise.
// 	if bMustBeValid == TRUE then only entries with a thumbnail field will be
//		returned.
//	Searches the file's directory dbf, and ALL album dbf's.
//************************************************************************/
LPENTRY FindEntry(LPFILELOC lpFile, LPENTRY lpOutEntry, BOOL bMustBeValid, BOOL bSearchAlbums)
//************************************************************************/
{
	ENTRY entry;
	DBFS dbf;
	BOOL bSuccess = FALSE;
	
	FINDDATA find;
	FILELOC File;

	File = *lpFile;
	entry = *lpOutEntry;
	// check directory dbf
	if (DBFOpenDirectory(&File, &dbf, NO))
	{
		// we know we have the correct volume
		CLEARVOLUME(File.volume);
		if (DBFGetEntry(&dbf, &File, &entry) != NULL)
		{
			bSuccess = TRUE;
			if (entry.Thumb.szFile[0] != _T('\0'))
				goto FoundEntry;
		}
		DBFClose(&dbf);
	}

	if ((!bSuccess || bMustBeValid) && bSearchAlbums)
 	{
 		//check all album dbf's
		DBFBeginInfo(&find, ID_ALBUM);
		while(DBFNextInfo(&dbf, &find,NO, NO))
		{
			if (DBFGetEntry(&dbf, &File, &entry) != NULL)
			{
				bSuccess = TRUE;
				if (entry.Thumb.szFile[0] != _T('\0'))
					goto FoundEntry;
			}
			DBFClose(&dbf);
		}
	}
	
	// no valid entry found
	if (!bSuccess || bMustBeValid)
		return(NULL);
	*lpOutEntry = entry;
	return(lpOutEntry);

FoundEntry:
	DBFClose(&dbf);
	*lpOutEntry = entry;
	return(lpOutEntry);
}

//************************************************************************/
// 	Sets lpOutEntry for the file IF a VALID entry exists. (thumb must exist).
//	Otherwise it will create and install a minimal thumbnail.
//	Returns BSR_TRUE if it needed to create a new thumbnail.
//		returns BSR_FALSE if it didn't need to create a thumbnail.
//		returns BSR_ERROR if there was an error (ie not available) or it was canceled.
// 	If a new thumb is created (and a new dbf) then lpOutEntry->File.volume
//		may be more accurate than lpFile->volume.
//	If a new thumb is created and bCreateWithThumb then it also creates a preview.  
//		if the preview creation fails then it deletes the new thumb.
//************************************************************************/
int 	FindOrCreateThumb(LPFILELOC lpFile, LPENTRY lpOutEntry, BOOL bCreateWithThumb)
//************************************************************************/
{
	LPCTHMNAIL lpThm;

 	// Is there already an entry with a thumbnail?
	if (FindEntry(lpFile, lpOutEntry, TRUE, ALBMODE))
	{
		// can we open it
		if (lpThm = CTHMOpen(lpOutEntry))
		{
			CTHMClose(lpThm);
			return(BSR_FALSE);
		}
		// cant open, is there a thumb at all?
		else 
		{
			// do we have the right volume 
			if (!VolumeCheck(&lpOutEntry->Thumb, NULL))
				return(BSR_ERROR);
			// if the file is not gone
			if (FileExists(lpOutEntry->Thumb.szFile))
				return(BSR_ERROR);
		}
	}
	// Something failed
	// a valid entry doesn't exist
	// Create entry and thumb
	lpOutEntry->File = *lpFile;
	// should determine if local or not!
	lpThm = CTHMCreate(lpOutEntry, TRUE);
	if (!lpThm)
		return(BSR_ERROR);
	if (bCreateWithThumb)	
		if (!CTHMReCreateFrame(lpThm, &lpOutEntry->File, YES))
		{
			// delete it
			FileDelete(lpThm->thm.Thumb.szFile);
			CTHMClose(lpThm);
			return(BSR_ERROR);
		}
	CTHMClose(lpThm);
	if (!InstallEntry(lpOutEntry))
		return(BSR_ERROR);
	return(BSR_TRUE);
}

//************************************************************************/
//	Searches the album for a file with filename == lpFile.
// 	Returns the number of matches found.
//	lpOut will hold the last match found (if any are found);
//	The album must be in the album list.
//************************************************************************/
int  	GuessFileFromAlbum(LPTSTR lpAlbum, LPTSTR lpFile, LPFILELOC lpFileOut)
//************************************************************************/
{
	LPDBFLL	lpDBFLL;
	LPDBF	lpDBF;
	ENTRY	entry;
	int 	num;

	lpDBFLL = FindAlbum(lpAlbum);
	if (lpDBFLL)
		lpDBF = &(lpDBFLL->dbf);
	else
		return(0); 	// no album
	DBFReOpen(lpDBF, NO);
	// search all entries
	num = 0;
	DBFGetEntryNum(lpDBF, -1, &entry);
	while(DBFGetNextEntry(lpDBF, &entry))
	{
		if (StringsEqual(filename(entry.File.szFile), lpFile))
		{
			num++;
			*lpFileOut = entry.File;
		}
	}
	DBFClose(lpDBF);
	return(num);
}

//************************************************************************/
// 	If file is areaady in the album then does nothing.
//	If file is not in album && file already has thumb
//		then adds file & thm to album
//	If file is not in album && file has no thumb
//		then created a 'minimal' thumb  & adds to album (and directory).
//	If a thm is created then an entry will also be put into the Directory DBF.
//	If there is any error will return FALSE.
// 	IF bUpdateInfo 	== YES then the thm info will be updated (info only).
// 					== MAYBE then info will be updated only if thm is CREATED.
//					== NO then no info will be updated or created. (file doesnt have to exist).
// 	Returns False for a severe error.
//************************************************************************/
BOOL	AddToAlbum(LPDBF lpDBF, LPFILELOC lpFile, BOOL bUpdateInfo)
//************************************************************************/
{
	ENTRY	entry;
	LPENTRY lpEntry;
	LPCTHMNAIL lpThm;
	int  	ret;

	DBFReOpen(lpDBF, NO);
	// Is file is already in album?
	lpEntry = DBFGetEntry(lpDBF, lpFile, &entry);
	DBFClose(lpDBF);
	lpThm = NULL;
	if (lpEntry && entry.Thumb.szFile[0] != _T('\0'))
		lpThm = CTHMOpen(&entry);
	if (!lpThm)
	{
		// Must Find or create a thumb
		ret = FindOrCreateThumb(lpFile, &entry, NO);
		if (ret == BSR_ERROR)
			return(FALSE);
		if (ret == BSR_TRUE)
			bUpdateInfo = (bUpdateInfo == NO) ? NO:YES;	   // created thumb
		// Save out the new album entry
		DBFReOpen(lpDBF, YES);
		ret = DBFPutEntry(lpDBF, &entry, NULL);
		DBFClose(lpDBF);
		if (!ret)
			return(FALSE);
		lpThm = CTHMOpen(&entry);
		if (!lpThm)
			return(FALSE);
	}
	ret = TRUE;
	if (bUpdateInfo == YES)
		ret = CTHMReCreateEasyInfo(lpThm, &entry.File, YES);
	CTHMClose(lpThm);
	return(ret);
}

//************************************************************************/
//	Uses validation level to decide how to update the thumbnail (if any)
//		 for the file. (See browser.h for validation levels).
//	May create a new thumbnail, update an old thumb, etc.
//	Handles:	VL_EXIST, and calls CTHMValidate: VL_NOTIME, VL_CLEAN, VL_DATAONLY, VL_ESCAPABLE.
//	returns TRUE if the file is going to need updating.
//************************************************************************/
BOOL 	UpdateCreateThumb(LPFILELOC lpFile, WORD level, BOOL bCreateWithThumb)
//************************************************************************/
{
	ENTRY entry;
	LPCTHMNAIL lpThm;
	int 	ret;

	if (level & VL_NONE)
		return(TRUE);
	if ((level & VL_ESCAPABLE) && SAFECANCEL)
		return(FALSE);
	if (level & VL_EXIST)
	{
		// find and create if doesnt exist
	   	if (FindOrCreateThumb(lpFile, &entry, (level & VL_DATAONLY)? FALSE:bCreateWithThumb) == BSR_ERROR)
			return(FALSE);
		lpThm = CTHMOpen(&entry);
		if (!lpThm)
			return(FALSE);
	}
	else 
	{
		// only proceed if a thumb already exists
		if (!FindEntry(lpFile, &entry, TRUE, ALBMODE))
			return(TRUE);
		// can we open it
		lpThm = CTHMOpen(&entry);
		if (!lpThm)
			return(TRUE);
	}
	// We now have a thumbnail
	ret = CTHMValidate(lpThm, &entry.File, level);
	if (!ret)
		return(FALSE);
	if (ret == -1)
	{
		// must delete thmbnail and update all dbf's
		CompleteDeleteEntry(&entry);
		return(TRUE);
	}
	return(TRUE);
}

//************************************************************************/
//	Installs the entry into the appropraite Directory DBF.
//	Uninstalls the entry for lpFile from its Directory DBF.
// 	Replaces any entries in albums to lpFile with the lpEntry.
//	Returns FALSE for major failure.
//************************************************************************/
BOOL	ChangeEntry(LPFILELOC lpFile, LPENTRY lpEntry)
//************************************************************************/
{
	LPDBF lpDBF;
	DBFS dbf;
	BOOL ret;
	ENTRY tempEntry;
	FINDDATA find;

	// delete old entry from Directory DBF
	lpDBF = DBFOpenDirectory(lpFile, &dbf, YES);
	if (lpDBF)
	{
		DBFDeleteEntry(lpDBF, lpFile);
		DBFClose(lpDBF);
	}
	// Put new entry into Directory DBF
	lpDBF = DBFOpenDirectory(&lpEntry->File, &dbf, YES);
	if (!lpDBF)
	{
		lpDBF = DBFNewDirectory(&lpEntry->File, &dbf, TRUE);
		lpBsr->szLastDirDBF[0] = _T('\0');
		if (!lpDBF)
			return (FALSE);
	}
	ret = DBFPutEntry(lpDBF, lpEntry, NULL);
	DBFClose(lpDBF);
	if (!ret)
		return(FALSE);
	// Find all duplicates  in albums and replace
	DBFBeginInfo(&find, ID_ALBUM);
	while(DBFNextInfo(&dbf, &find,YES, NO))
	{
		if (DBFGetEntry(&dbf, lpFile, &tempEntry))
			DBFPutEntry(lpDBF, lpEntry, lpFile);
		DBFClose(&dbf);
	}
	return(TRUE);
}

//************************************************************************/
//	Replaces any referance to THUMBNAIL FILE lpThumb in the dbf's & 
//		albums with referances to lpDst.
//	Returns FALSE for major failure.
//************************************************************************/
BOOL 	ChangeThumb(LPFILELOC lpThumb, LPFILELOC lpDst)
//************************************************************************/
{
	FINDDATA afile;
	DBFS dbf;
	int ret;
	ENTRY entry;
	
	// change entry in one Directory DBF
	DBFBeginInfo(&afile, ID_DIRECTORY);
	while(DBFNextInfo(&dbf, &afile,YES, NO))
	{
		DBFGetEntryNum(&dbf, -1, &entry);
		while(DBFGetNextEntry(&dbf, &entry))
			if (EQFILELOC(entry.Thumb, *lpThumb))
			{
				// found one now change it	
				entry.Thumb = *lpDst;
				ret = DBFChangeCurrentEntry(&dbf, &entry);
				DBFClose(&dbf);
				if (!ret)
					return(FALSE);
				goto ChangeAlbums;
			}
		DBFClose(&dbf);
	}

ChangeAlbums:
	// change entry in all Albums
	DBFBeginInfo(&afile, ID_ALBUM);
	while(DBFNextInfo(&dbf, &afile,YES, NO))
	{
		DBFGetEntryNum(&dbf, -1, &entry);
		while(DBFGetNextEntry(&dbf, &entry))
			if (EQFILELOC(entry.Thumb, *lpThumb))
			{
				// found one now change it	
				entry.Thumb = *lpDst;
				ret = DBFChangeCurrentEntry(&dbf, &entry);
				if (!ret)
					return(FALSE);
			}
		DBFClose(&dbf);
	}
	return(TRUE);
}

//************************************************************************/
//	Installs the entry into the appropraite Directory DBF.
//	Replaces any confilting Entries in the Directory, or in any album.
//	Returns FALSE for major failure.
//************************************************************************/
BOOL	InstallEntry(LPENTRY lpEntry)
//************************************************************************/
{
	LPDBF lpDBF;
	DBFS dbf;
	BOOL ret;
	ENTRY tempEntry;
	FINDDATA find;

	// Put entry into Directory DBF
	lpDBF = DBFOpenDirectory(&lpEntry->File, &dbf, YES);
	if (!lpDBF)
	{
		lpDBF = DBFNewDirectory(&lpEntry->File, &dbf, TRUE);
		lpBsr->szLastDirDBF[0] = _T('\0');
		if (!lpDBF)
			return (FALSE);
	}
	ret = DBFPutEntry(lpDBF, lpEntry, NULL);
	DBFClose(lpDBF);
	if (!ret)
		return(FALSE);
	// Find all duplicates  in albums and replace
	DBFBeginInfo(&find,ID_ALBUM);
	while(DBFNextInfo(&dbf, &find,YES, NO))
	{
		if (DBFGetEntry(&dbf, &lpEntry->File, &tempEntry))
			DBFPutEntry(lpDBF, lpEntry, NULL);
		DBFClose(&dbf);
	}
	return(TRUE);
}

//************************************************************************
//	verifies all entries in the dbf.
// 	dbf should be open on call.
//	returns FALSE if there was a major failure.
//************************************************************************
BOOL 	CleanupDBF(LPDBF lpDBF, WORD level)
//************************************************************************
{
	int ret, num;
	ENTRY entry;
	LPCTHMNAIL lpThm;
	
	if (!DBFReOpen(lpDBF, YES))
		return(FALSE);
	DBFGetEntryNum(lpDBF, num = -1, &entry);
	while(DBFGetNextEntry(lpDBF, &entry))
	{
		num++;
		lpThm = CTHMOpen(&entry);
		if (!lpThm)
			continue;
		// We now have a thumbnail
		ret = CTHMValidate(lpThm, &entry.File, level);
		if (!ret)
			continue;
		if (ret == -1)
		{
			// must delete thmbnail and update all dbf's
			DBFClose(lpDBF);
			CompleteDeleteEntry(&entry);
			if (!DBFReOpen(lpDBF, YES))
				return(FALSE);
			if (!DBFGetEntryNum(lpDBF,--num, &entry))
				break;
		}
	}
	return(TRUE);
}

//************************************************************************
// 	Deletes the actual thumbnail. (if there is one)
//	Removes the entry from all dbf's.
//	Returns FALSE if there was a failure (ie thumb not available).
//************************************************************************
BOOL	CompleteDeleteEntry(LPENTRY lpEntry)
//************************************************************************
{
	LPDBF lpDBF;
	DBFS dbf;
	int ret;
	FINDDATA find;
	LPCTHMNAIL lpThm;

	ret = TRUE;
	
	// Check volume
	if (VolumeCheck(&lpEntry->Thumb, NULL))
	{
		// Be sure thumb is not cached!!!
		if (lpThm = CTHMOpenCached(&lpEntry->Thumb))
			CTHMPurge(lpThm);
		
		// try to Delete thumbnail file
		FileDelete(lpEntry->Thumb.szFile);
	}
	// Remove entry from Directory DBF
	lpDBF = DBFOpenDirectory(&lpEntry->File, &dbf, YES);
	if (lpDBF)
	{
		if (DBFDeleteEntry(&dbf, &lpEntry->File) == DBF_FAILURE)
			ret = FALSE;
		DBFClose(lpDBF);
	}
	// Find all duplicates  in albums and remove
	DBFBeginInfo(&find,ID_ALBUM);
	while(DBFNextInfo(&dbf, &find, YES, NO))
	{
		if (DBFDeleteEntry(&dbf, &lpEntry->File) == DBF_FAILURE)
			ret = FALSE;
		DBFClose(&dbf);
	}
	return(ret);
}

//************************************************************************
//************************************************************************
//	DRIVE TYPE AND VOLUME UTILITIES
//************************************************************************
//************************************************************************

//************************************************************************/
//	Sets up memory needed to hold drive types and volume id's
//************************************************************************/
BOOL SetupDriveInfoMem(BOOL bFill)
//************************************************************************/
{
	int i;
	
	lpBsr->DriveTypes = (UINT FAR *)Alloc(NUM_DRIVES*(sizeof(*lpBsr->DriveTypes) + sizeof(VOLUMEID)));
	if (!lpBsr->DriveTypes)
	{
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}
	lpBsr->DriveVolumes = (LPVOLUMEID)(&lpBsr->DriveTypes[NUM_DRIVES]);
	for (i=0;i<NUM_DRIVES;i++)
	{
		if (bFill)
			lpBsr->DriveTypes[i] = GetExtendedDriveType(i);
		else
			lpBsr->DriveTypes[i] = DRIVE_REMOVABLE;
	}
	return(TRUE);
}

//************************************************************************/
//	Frees Up memory needed to hold drive types and volume id's
//************************************************************************/
void FreeUpDriveInfoMem(void)
//************************************************************************/
{
	if (lpBsr->DriveTypes)
		FreeUp((LPTSTR)lpBsr->DriveTypes);
	lpBsr->DriveTypes = NULL;
//	lpBsr->DriveVolumes = NULL;
}

//************************************************************************/
// 	Returns the extended drive type of the drive.
//	drive can be 0-25 a-z or A-Z.
//************************************************************************/
UINT BSRGetDriveType(int drive)
//************************************************************************/
{
	if (WITHIN(drive, 0, 25))
		return(lpBsr->DriveTypes[drive]);
	else if (WITHIN(drive, _T('a'),_T('z')))
		drive -= _T('a');
	else if (WITHIN(drive, _T('A'),_T('Z')))
		drive -= _T('A');
	else 
		return(0);
	return(lpBsr->DriveTypes[drive]);
}

//************************************************************************
// 	Sets the extended drive type of the drive for later use.
//	drive can be 0-25 a-z or A-Z.
//************************************************************************
UINT BSRSetDriveType(int drive, UINT type)
{
	if (WITHIN(drive, 0, 25))
		return(lpBsr->DriveTypes[drive]=type);
	else if (WITHIN(drive, _T('a'),_T('z')))
		drive -= _T('a');
	else if (WITHIN(drive, _T('A'),_T('Z')))
		drive -= _T('A');
	else 
		return(0);
	return(lpBsr->DriveTypes[drive]=type);
}


//************************************************************************
// 	 Be Sure Drive Types Filled First!!!
//	 Causes the next call to GetDriveVolume or VolumeCheck to 
//		actually get the volume for each drive.
//************************************************************************
void BSRInvalidateDriveIDs(void)
//************************************************************************
{
	int i;
	
	for (i=0; i<NUM_DRIVES; i++)
	{
		if (lpBsr->DriveTypes[i] != DRIVE_CDROM && lpBsr->DriveTypes[i] != DRIVE_REMOVABLE)
		{
			lpBsr->DriveVolumes[i].OurMark = VOLUME_ANY;
			lpBsr->DriveVolumes[i].VolLabel = VOLUME_ANY;
		}
		else
		{
			lpBsr->DriveVolumes[i].OurMark = VOLUME_INVALID;
			lpBsr->DriveVolumes[i].VolLabel = VOLUME_INVALID;
		}
	}
}


//***********************************************************************
//	Given a file location check if an acceptable disk is available.
//	Returns TRUE if the volume is available.
//	If true then lpVolume will be the actual volume found.
//***********************************************************************
BOOL VolumeCheck(LPFILELOC lpFile, LPVOLUMEID lpVolume)
//************************************************************************
{
	VOLUMEID volume;
	
	if (!lpVolume && 
			lpFile->volume.OurMark == VOLUME_ANY &&
			lpFile->volume.VolLabel == VOLUME_ANY)
		return(TRUE);
	GetDriveVolume(lpFile->szFile, &volume, YES);
	if (lpVolume)
		*lpVolume = volume;
	return(EqualVolume(&lpFile->volume, &volume));
}

//************************************************************************
//	Get the volume for the disk in the drive pointed to by lpName
// 	if bAll then both OurMark & VolLabel are gotten. (needed for comparison)
//	if !bAll then only OurMark is gotten (unless its not available).
//************************************************************************
LPVOLUMEID GetDriveVolume(LPTSTR lpName, LPVOLUMEID lpVolume, BOOL bAll)
//************************************************************************
{
	int drive;
	
	drive = lpName[0];
	if (WITHIN(drive, _T('a'),_T('z')))
		drive -= _T('a');
	else if (WITHIN(drive, _T('A'),_T('Z')))
		drive -= _T('A');
	else 
	{
		MessageBeep(-1);
		CLEARVOLUME((*lpVolume));
		return(lpVolume);
	}
	return(DriveVolume(drive, lpVolume, bAll));
}

//************************************************************************
// 	Returns the volume of the drive.
//	drive can be 0-25 a-z or A-Z.
// 	if bAll then both OurMark & VolLabel are gotten. (needed for comparison)
//	if !bAll then only OurMark is gotten (unless its not available).
//************************************************************************
LPVOLUMEID DriveVolume(int drive, LPVOLUMEID lpVolume, BOOL bAll)
//************************************************************************
{
	// is it a bad drive
	if (!WITHIN(drive, 0, 25))
	{
		CLEARVOLUME((*lpVolume));
		return(lpVolume);
	}
	// is it non removable
	if (lpBsr->DriveTypes[drive] != DRIVE_CDROM && 
				lpBsr->DriveTypes[drive] != DRIVE_REMOVABLE)
	{
		CLEARVOLUME((*lpVolume));
		return(lpVolume);
	}
	// do we have valid Our Mark?
	if (lpBsr->DriveVolumes[drive].OurMark == VOLUME_INVALID)
		lpBsr->DriveVolumes[drive].OurMark = getOurMark(drive);
	lpVolume->OurMark = lpBsr->DriveVolumes[drive].OurMark;
	// do we need VolLabel?
	if (lpVolume->OurMark == VOLUME_ANY || bAll)
	{
		if (lpBsr->DriveVolumes[drive].VolLabel == VOLUME_INVALID)
			lpBsr->DriveVolumes[drive].VolLabel = getVolLabel(drive);
		lpVolume->VolLabel = lpBsr->DriveVolumes[drive].VolLabel;
	}
	else
		lpVolume->VolLabel = VOLUME_ANY;
	return(lpVolume);
}

//************************************************************************
// 	Returns True if the volumes are compatable
//************************************************************************
BOOL EqualVolume(LPVOLUMEID lpVol1, LPVOLUMEID lpVol2)
//************************************************************************
{
	if (lpVol1->OurMark == VOLUME_ANY || lpVol2->OurMark == VOLUME_ANY)
	{
		// check Volume Labels
		if (lpVol1->VolLabel == VOLUME_ANY ||
			 	lpVol2->VolLabel == VOLUME_ANY ||
			 	lpVol1->VolLabel == lpVol2->VolLabel)
			return(TRUE);
	   	return(FALSE);		
	}
	// have actual Our Mark
	if (lpVol1->OurMark == lpVol2->OurMark)
		return(TRUE);
	return(FALSE);
}

//************************************************************************
// 	Returns True if the volumes are (almost) certainly the same volume.
//	Use for actions which are very dangerous.
//	drive can be 0-25 a-z or A-Z.
//************************************************************************
BOOL StrongEqualVolume(int drive, LPVOLUMEID lpVol1, LPVOLUMEID lpVol2)
//************************************************************************
{
	int type;
	type = BSRGetDriveType(drive);
	if (type == DRIVE_FIXED || type == DRIVE_REMOTE || type == DRIVE_RAMDISK)
		return(EqualVolume(lpVol1, lpVol2));
	if (lpVol1->OurMark == lpVol2->OurMark && lpVol1->OurMark!=VOLUME_ANY && lpVol2->OurMark!=VOLUME_ANY)
		return(TRUE);
	if (lpVol1->VolLabel==1 || lpVol1->VolLabel==VOLUME_ANY)
		return(FALSE);
	if (lpVol2->VolLabel==1 || lpVol2->VolLabel==VOLUME_ANY)
		return(FALSE);
	if(lpVol1->VolLabel == lpVol2->VolLabel)
			return(TRUE);
	return(FALSE);		
}

//************************************************************************
// 	Tries to read the mark file we put at the root.
//	Returns VOLUME_ANY for failure
//************************************************************************
LONG getOurMark(int drive)
//************************************************************************
{
	ULONG time;
	FNAME szMarker;
	
	*szMarker = _T('a')+ drive;
	*MyCharNextN( szMarker, 1 ) = _T(':');
	*MyCharNextN( szMarker, 2 ) = _T('\\');
	lstrcpy( MyCharNextN( szMarker, 3 ), Browser.DiskMarker );
	if (!SafeFileStats(szMarker, NULL, NULL ,&time,NULL))
		return(VOLUME_ANY);
	return(time);
}

//************************************************************************
// 	Tries to put our marker file at the root.
//	Returns the new marker value.
//	Returns VOLUME_ANY for failure.
//************************************************************************
LONG putOurMark(int drive)
//************************************************************************
{
	ULONG time;
	FNAME szMarker;
	FILE_HANDLE file;
	
	*szMarker = _T('a')+drive;
	*MyCharNextN( szMarker, 1 ) = _T(':');
	*MyCharNextN( szMarker, 2 ) = _T('\\');
	lstrcpy( MyCharNextN( szMarker, 3 ), Browser.DiskMarker);
	// we only new an empty file for the time stamp
  	file = FileOpen(szMarker,FO_CREATE|FO_HIDDEN);
		return(VOLUME_ANY);
	FileClose(file);
		return(VOLUME_ANY);
	if (!SafeFileStats(szMarker, NULL, NULL ,&time,NULL))
		return(VOLUME_ANY);
	if (lpBsr->DriveVolumes)
		lpBsr->DriveVolumes[drive].OurMark = time;
	return(time);
}

//************************************************************************
// 	Tries to read the DOS volume label for the drive.
//	Returns a long based on the volume label
//	Returns VOLUME_ANY for failure.
//************************************************************************
LOCAL LONG getVolLabel(int drive)
//************************************************************************
{
	LONG sum;
	TCHAR szString[10];
	TCHAR szLabel[15];
	LPTR lpLabel;
	UINT errmode;
	
	errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
	lstrcpy(szString, _T("a:\\*.*"));
	szString[0] += drive;
	GetVolumnID(szString,szLabel);
	SetErrorMode(errmode);
	if (!szLabel[0])
		return(1);

	sum = 0;
	lpLabel = ( LPTR )szLabel;
	while( *lpLabel )
	{
		if( IsDBCSLeadByte(( BYTE )*(( LPTSTR )lpLabel )))
			sum += *(( LPWORD )lpLabel );
		else
			sum += *lpLabel;
		lpLabel = ( LPTR )MyCharNext(( LPTSTR )lpLabel );
	}
	if( sum == VOLUME_INVALID )
		sum = 0L;
	return( sum );
}

//************************************************************************
//************************************************************************
//	UTILITIES
//************************************************************************
//************************************************************************

//************************************************************************/
//	Transfers contents and selection from one multiple selection
//		list box to another
// 	Must be a list box with strings	of no more than 160 chars
//************************************************************************/
BOOL CopyLB(HWND hLBDst, HWND hLBSrc)
/************************************************************************/
{
	int i, num, ret;
	int bSel;
	TCHAR szString[160];

	SetWindowRedraw(hLBSrc, FALSE);
	SetWindowRedraw(hLBDst, FALSE);
	ListBox_ResetContent(hLBDst);
	num = ListBox_GetCount(hLBSrc);
	if (num == LB_ERR) goto Fail;
	for (i=0; i<num; i++)
	{
		bSel = ListBox_GetSel(hLBSrc, 0);
		if (bSel == LB_ERR) goto Fail;
		ret = ListBox_GetText(hLBSrc, 0, szString);
		if (ret == LB_ERR) goto Fail;
		ret = ListBox_DeleteString(hLBSrc, 0);
		if (ret == LB_ERR) goto Fail;
		ret = ListBox_InsertString (hLBDst, -1, szString);
		if (ret == LB_ERR) goto Fail;
		ret = ListBox_SetSel(hLBDst, bSel, ret);
		if (ret == LB_ERR) goto Fail;
	}
	ListBox_ResetContent(hLBSrc);
	SetWindowRedraw(hLBSrc, TRUE);
	SetWindowRedraw(hLBDst, TRUE);
	return(TRUE);

Fail:
	SetWindowRedraw(hLBSrc, TRUE);
	SetWindowRedraw(hLBDst, TRUE);
	return(FALSE);
}

/************************************************************************/
LPTSTR SizeToAscii(DWORD dwSize, LPTSTR lpString )
/************************************************************************/
{
	int l;
	TCHAR num[32];
	WORD wChar;
	DWORD dwDivisor;
	LPTSTR lpLabel;

	#define GIG (1L<<30)
	#define MEG (1L<<20)
	#define KB (1L<<10)

	if ( dwSize < KB )
	{
		dwSize;
		itoa( (int)dwSize, num, 10 );
		lstrcpy( lpString, num );
		if ( AstralStr( IDS_BYTES, &lpLabel ) )
			lstrcat( lpString, lpLabel );
		return( lpString );
	}

	if ( dwSize >= GIG )
	{
		dwDivisor = GIG;
		if ( !AstralStr( IDS_GIGABYTES, &lpLabel ) )
			lpLabel = _T("");
	}
	else
	if ( dwSize >= MEG )
	{
		dwDivisor = MEG;
		if ( !AstralStr( IDS_MEGABYTES, &lpLabel ) )
			lpLabel = _T("");
	}
	else
	//if ( dwSize >= KB )
	{
		dwDivisor = KB;
		if ( !AstralStr( IDS_KILOBYTES, &lpLabel ) )
			lpLabel = _T("");
	}

	dwSize += (dwDivisor/20);
	dwSize /= (dwDivisor/10);
	itoa(( int )dwSize, num, 10 );
	l = MBStrlen( num );

	LPTSTR lp = MyCharNextN( num, l-1 );	// get the decimal place value
	if( IsDBCSLeadByte(( BYTE )*lp ))
		wChar = *((LPWORD )lp );
	else
		wChar = *lp;

	if ( l > 3 || wChar == _T('0') ) 			// has 4 significant digits or no decimal value
		*MyCharNextN( num, l-1 ) = _T('\0'); 	// Don't do any decimal places
	else	
	{
		*MyCharNextN( num, l-1 ) = _T('.');

		if( IsDBCSLeadByte( LOBYTE( wChar )))
			*(( LPWORD )MyCharNextN( num, l )) = wChar;
		else
			*MyCharNextN( num, l ) = LOBYTE( wChar );

		*MyCharNextN( num, l+1 ) = _T('\0');
	}
	lstrcpy( lpString, num );
	lstrcat( lpString, lpLabel );
	return( lpString );
}

//************************************************************************/
// Call once before using FormatDate or FormatTime
//************************************************************************/
void SetupInternational(void)
//************************************************************************/
{
	STRING szName;
	
	lstrcpy(szName, _T("intl"));
	lpBsr->iDate = GetProfileInt(szName, _T("iDate"), 0);
	lpBsr->iTime = GetProfileInt(szName, _T("iTime"), 0);
	GetProfileString(szName, _T("sDate"), _T("/"), lpBsr->sDate, 2 );
	GetProfileString(szName, _T("sTime"), _T(":"), lpBsr->sTime, 2 );
	GetProfileString(szName, _T("s1159"), _T("AM"), lpBsr->sAMPM[0], 5 );
	GetProfileString(szName, _T("s2359"), _T("PM"), lpBsr->sAMPM[1], 5 );
}

#define YEAR (time->tm_year % 100)
#define MONTH (time->tm_mon+1)
#define DAY (time->tm_mday)
//************************************************************************/
LPTSTR	FormatDate(struct tm *time, LPTSTR lpDate)
//************************************************************************/
{
	STRING szDate;
	
	wsprintf(szDate, "%d%s%02d%s%02d",
			lpBsr->iDate == 1 ? DAY	: lpBsr->iDate == 2 ? YEAR	: MONTH	, (LPTSTR)lpBsr->sDate,
			lpBsr->iDate == 1 ? MONTH	: lpBsr->iDate == 2 ? MONTH: DAY	, (LPTSTR)lpBsr->sDate,
			lpBsr->iDate == 1 ? YEAR	: lpBsr->iDate == 2 ? DAY 	: YEAR);
	lstrcpy (lpDate, szDate);
	return(lpDate);
}

//************************************************************************/
// call SetupInternational before using
//************************************************************************/
LPTSTR	FormatTime(struct tm *time, LPTSTR lpTime)
//************************************************************************/
{
	STRING szTime;
	
	if (!strftime(szTime, sizeof(STRING), "%X", time))
		return(NULL);

//	if (lpBsr->iTime == 1)
//		wsprintf(szTime, "%02d%s%02d%s%02d",
//					time->tm_hour, (LPTSTR)lpBsr->sTime,
//					time->tm_min, (LPTSTR)lpBsr->sTime, 
//					time->tm_sec);
//	else
//		wsprintf(szTime, "%d%s%02d%s%02d%s",
//					(time->tm_hour%12) ? (time->tm_hour%12):12,  (LPTSTR)lpBsr->sTime,
//					time->tm_min, (LPTSTR)lpBsr->sTime,
//					time->tm_sec,
//					(LPTSTR)lpBsr->sAMPM[time->tm_hour/12]);
	lstrcpy (lpTime, szTime);
	return(lpTime);
}

//************************************************************************
LPTSTR DriveSize( int iDrive, LPTSTR lpSize )
//************************************************************************
{
	TCHAR szString[MAX_STR_LEN];
	DWORD dwFree, dwTotal;
	WORD wType;
	LPTSTR lpSlash;

	wType = BSRGetDriveType(iDrive);
	if( iDrive != CURRENTDRIVE &&
	  ( wType == DRIVE_REMOVABLE || wType == DRIVE_CDROM ))
		*lpSize = _T('\0');
	else
	{
		*szString = iDrive+_T('A');
		*MyCharNextN( szString, 1 ) = _T(':');
		*MyCharNextN( szString, 2 ) = _T('\\');
		*MyCharNextN( szString, 3 ) = _T('\0');
		if (!DiskSpaceEx(szString, &dwTotal, &dwFree ))
		{
			*lpSize = _T('\0');
			return(lpSize);
		}
		SizeToAscii( dwFree, szString );
		lstrcpy( lpSize, szString );

		if( AstralStr( IDS_SLASH, &lpSlash ))
			lstrcat( lpSize, lpSlash );

		SizeToAscii( dwTotal, szString );
		lstrcat( lpSize, szString );
	}
	return( lpSize );
}

//************************************************************************/
// 	Gets the current directory with a special cas for cd drives
// 	cd drives require access befor CURRENTDIR will work
//************************************************************************/
P_STR CurrentDirEx(int drive, LPTSTR lpPath, int size)
{
	FNAME szPath;
	P_STR pret;
	FINDDATA vol;
//	UINT errmode;
	
	szPath[0] = _T('\0');
	if (BSRGetDriveType(drive) == DRIVE_CDROM)
		if(!Dos_FindFirst( _T("\\*.*"), ATTRIB_SUBDIR | ATTRIB_READONLY, &vol ))
			return(NULL);
//	errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
	pret = CURRENTDIR(szPath, size); 
//	SetErrorMode(errmode);
	if (!pret)
		return(NULL);
	lstrcpy(lpPath, szPath);
	return(pret);
}		
													
//************************************************************************/
//	This provides a way to tell if the drive was changed properly.
//	Because it is so hard to tell if a cd drive has a disk... IE. getcwd & setdrive
//		dont fail etc.  
//************************************************************************/
BOOL ChangeDrive(int drive)
//************************************************************************/
{
#ifdef WIN32
	return(CHDRIVE(drive));
#else
	FINDDATA vol;
	unsigned newdrive;
//	int val;

	_dos_setdrive(drive+1,&newdrive);
	_dos_getdrive(&newdrive);
	if (drive != (newdrive-1))
		return(FALSE);
//	val is no longer valid !!
#ifdef BROKEN
	if(val = Dos_FindFirst( _T("\\*.*"), ATTRIB_SUBDIR | ATTRIB_READONLY, &vol ))
		if (val != 18)	// if there are no files but there is a disk then will get 18
			return(FALSE);
	return(TRUE);
#endif
	if (BSRGetDriveType(drive) == DRIVE_CDROM)
		return(Dos_FindFirst( _T("\\*.*"), ATTRIB_SUBDIR | ATTRIB_READONLY, &vol ));
	return(TRUE);
#endif
}

//************************************************************************/
// 	Returns TRUE if the path is a valid directory
//************************************************************************/
BOOL CheckDir(LPTSTR lpPath)
//************************************************************************/
{
	FNAME szPath;			  

	lstrcpy(szPath, lpPath);
	MakeFullPath(szPath);
	FixPath(szPath);
	RemovePathBackslash(szPath);
	return(IsDirectory(szPath));
}

//************************************************************************/
BOOL IsDirectory (LPTSTR lpName)
//************************************************************************/
{
    TCHAR SaveDir [MAX_PATH];
    BOOL bResult = FALSE;

    if (GetCurrentDir(SaveDir, sizeof(SaveDir)))
    {
        bResult = SetCurrentDir (lpName);
        SetCurrentDir (SaveDir);
    }
    return(bResult);
}

//************************************************************************/
// Checks if the file name on the path is valid
//************************************************************************/
BOOL IsValidFileName(LPTSTR lpPath)
//************************************************************************/
{
	FNAME szFile, szOld;
	LPTSTR lptr;
	WORD wChar;
	
	lstrcpy(szFile, filename(lpPath));
 	RemovePathBackslash(szFile);
	lstrcpy(szOld, szFile);
	FixFileName(szFile);
	lptr = szFile;
	while( TRUE )
	{
		if( !( *lptr ))
			break;

		if( IsDBCSLeadByte(( BYTE )*lptr ))
			wChar = *(( LPWORD )lptr );
		else
			wChar = *lptr;

		if( wChar == _T('*') || 
			wChar == _T('?') ||
			wChar == _T(' ') ||
			wChar == _T(':') ||
			wChar == _T(';') ||
			wChar == _T('+') ||
			wChar == _T(','))
			return( FALSE );

		lptr = MyCharNext( lptr );
	}
	return(StringsEqual(szFile, szOld));
	
}

//************************************************************************/
// 	Removes the trailing backslash from the path unless it is the root
//************************************************************************/
LPTSTR RemovePathBackslash(LPTSTR lpPath)
//************************************************************************/
{
	int len;
	WORD wFirst, wNext; 

	StripTrailer( lpPath, _T(' ') );
	len = MBStrlen( lpPath );

	if( len <= 1 )
	{
		if( IsDBCSLeadByte(( BYTE )*lpPath ))
			wFirst = *(( LPWORD )lpPath );
		else
			wFirst = *lpPath;
		if( wFirst != _T('\\') )
			return( lpPath );
	}
	else
	if( len == 3 )
	{
		LPTSTR lp = MyCharNextN( lpPath, 1 );
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wFirst = *(( LPWORD )lp );
		else
			wFirst = *lp;

		lp = MyCharNextN( lpPath, 2 );
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wNext = *(( LPWORD )lp );
		else
			wNext = *lp;

		if( wFirst == _T(':') && wNext == _T('\\') )
			return( lpPath );
	}
	StripTrailer( lpPath,_T('\\') );
	return( lpPath );
}

//************************************************************************/
//	Takes a global file type (IDN_) and returns the equivelent
//		local file type (IDT_).
//************************************************************************/
WORD LocalFileType(ITEMID idAppType)
//************************************************************************/
{
	switch(idAppType)
	{
		case IDN_SCITEX:
			return (IDT_SCITEX);
		case IDN_TIFF:
			return (IDT_TIFF);
		case IDN_TARGA:
			return(IDT_TARGA);
		case IDN_GIF:
			return(IDT_GIF);
		case IDN_BMP:
			return(IDT_BMP);
		case IDN_DIB:
			return(IDT_DIB);
		case IDN_PCX:
			return(IDT_PCX);
		case IDN_EPS:
			return(IDT_EPS);
		case IDN_DCS:
			return(IDT_DCS);
		case IDN_JPEG:
			return(IDT_JPEG);
		case IDN_PHOTOCD:
			return(IDT_PHOTOCD);
		case IDN_AVI:
			return(IDT_AVI);
		case IDN_PP4:
			return(IDT_PP4);
		case IDN_PP:
			return(IDT_PP);
		case IDN_PPCOMPOUND:
			return(IDT_PPCOMPOUND);
      case IDN_AI:                  
			return(IDT_AI);
      case IDN_CDR:                 
			return(IDT_CDR);
      case IDN_CGM:                 
			return(IDT_CGM);
      case IDN_DRW:                 
			return(IDT_DRW);
      case IDN_WMF:                 
			return(IDT_WMF);
		case IDN_NOFILETYPE:
			return(IDT_NOFILETYPE);
		case IDN_NOIMPORTFILETYPE:
			return(IDT_NOIMPORTFILETYPE);
		default:
            if( bImported )
    	        return(IDT_ALLIMPORTFILES);
            else
			    return(IDT_ALLFILES);
	}
    if( bImported )
    	return(IDT_ALLIMPORTFILES);
    else
    	return(IDT_ALLFILES);
}

//************************************************************************/
//	Takes a local file type (IDT_) and returns the equivelent
//		global file type (IDN_).
//	Since there are more local types, any without matches go to IDN_TIFF
//************************************************************************/
WORD AppFileType(ITEMID idLocalType)
//************************************************************************/
{
	switch (idLocalType)
	{
		case IDT_SCITEX:
			return (IDN_SCITEX);
		case IDT_TIFF:
			return (IDN_TIFF);
		case IDT_TARGA:
			return (IDN_TARGA);
		case IDT_GIF:
			return (IDN_GIF);
		case IDT_BMP:
			return (IDN_BMP);
		case IDT_DIB:
			return (IDN_DIB);
		case IDT_PCX:
			return (IDN_PCX);
		case IDT_EPS:
			return (IDN_EPS);
		case IDT_DCS:
			return (IDN_DCS);
		case IDT_JPEG:
			return (IDN_JPEG);
		case IDT_PHOTOCD:
			return (IDN_PHOTOCD);
		case IDT_AVI:
			return (IDN_AVI);
		case IDT_PP4:
			return (IDN_PP4);
		case IDT_PP:
			return (IDN_PP);
		case IDT_PPCOMPOUND:
			return (IDN_PPCOMPOUND);
      case IDT_AI:                  
			return(IDN_AI);
      case IDT_CDR:                 
			return(IDN_CDR);
      case IDT_CGM:                 
			return(IDN_CGM);
      case IDT_DRW:                 
			return(IDN_DRW);
      case IDT_WMF:                 
			return(IDN_WMF);
		case IDT_NOFILETYPE:
			return(IDN_NOFILETYPE);
		case IDT_NOIMPORTFILETYPE:
			return(IDN_NOIMPORTFILETYPE);
		default:
		    if( bImported )
		    	return(IDN_AI);
		    else
		    	return(IDN_TIFF);
	}
    if( bImported )
    	return(IDN_AI);
    else
    	return(IDN_TIFF);
}


//************************************************************************/
//	sets the file extension based on an IDT_ file type 
// 	if bReal is true then only a single, non wildcard ext will be set.
//	Will not exceed sizeof(FNAME).
//************************************************************************/
void SetExtension(LPTSTR lpFile, ITEMID idType, BOOL bReal)
/************************************************************************/
{
	LPTSTR lpNewExt, lpExt;
	int len, i;
	FNAME szFile;

	//	get extension
	if (!bReal && ( idType == IDT_NOFILETYPE || idType == IDT_NOIMPORTFILETYPE ))
    {
		// use the file spec's extension
		lpExt = extension(lpFile);
		len = lstrlen(lpFile) - lstrlen(lpExt) + 1;
		lpNewExt = extension(lpBsr->OpenState.szFileSpec);
		lStrCpyN(lpExt, lpNewExt, sizeof(FNAME) - len);
		return;
	}
	if (!bReal && ( idType == IDT_ALLIMAGES || idType == IDT_ALLIMPORTIMAGES ))
	{
		// all images use list
		lstrcpy(szFile, lpFile);
		lpFile[0] = _T('\0');
		len = 0;
		for (i = 0; i<3;i++)
		{
            if( idType == IDT_ALLIMAGES )
			    SetExtension(szFile, i+IDT_FIRSTREAL, YES);
            else
			    SetExtension(szFile, i+IDT_FIRSTIMPORTREAL, YES);

			len += lstrlen(szFile)+1;
			if (len-7 > MAX_FNAME_LEN)
				break;
			lstrcat(lpFile, szFile);
			lstrcat(lpFile, _T(","));
		}
		StripTrailer(lpFile, _T(','));
		lstrcat(lpFile, _T("..."));
		return;				  
	}
    if( bImported )
    {
	    if( bReal && ( idType<IDT_FIRSTIMPORTREAL || idType > IDT_LASTIMPORTREAL ))
		    idType = IDT_AI;
	    else 
        if( !bReal && ( idType<IDT_FIRSTIMPORTFILETYPE || idType > IDT_LASTIMPORTFILETYPE ))
		    idType = IDT_ALLIMPORTFILES;
    }
    else
    {
	    if (bReal && (idType<IDT_FIRSTREAL || idType > IDT_LASTREAL))
		    idType = IDT_PP;
	    else if (!bReal && (idType<IDT_FIRSTFILETYPE || idType > IDT_LASTFILETYPE))
		    idType = IDT_ALLFILES;
    }
	// special case all images
	i = AstralStr(IDT_ALLIMAGES, &lpExt);
	if (i && StringsEqual(lpFile, lpExt))
		lpExt = &lpFile[1];	
	else
		lpExt = extension(lpFile);
	len = lstrlen(lpFile) - lstrlen(lpExt) + 1;

	if (!AstralStr(idType, &lpNewExt))
	{
		if (!bReal)
			lStrCpyN(lpExt, _T(".*"), sizeof(FNAME) - len);
		else
        {
            if( bImported )
    			lStrCpyN(lpExt, _T(".AI"), sizeof(FNAME) - len);
            else
    			lStrCpyN(lpExt, _T(".TIF"), sizeof(FNAME) - len);
        }
	}
	else
		lStrCpyN(lpExt, lpNewExt, sizeof(FNAME) - len);
}

//************************************************************************/
//	Sets the file extension based on an IDT_ file type.
//	If there is already a valid file type extension then it is replaced.
//		Otherwide the extension is added.
// 	If bReal is true then only a single, non wildcard ext will be set.
//	Will not exceed sizeof(FNAME).
//************************************************************************/
void AddExtension(LPTSTR lpFile, ITEMID idType, BOOL bReal)
/************************************************************************/
{
	int type;
    LPTSTR pszDot;
	
	// is there a valid extension?
	type = GetTypeFromExt(lpFile, 0);
	if (type)
	{
		SetExtension(lpFile, idType, bReal);
		return;
	}

	// ok add extension

    pszDot = MBStrrchr( lpFile, _T('.') );
    if( pszDot )
        *pszDot = _T('\0'); // remove invalid extension       

    if( bImported )
    	lstrcat(lpFile, _T(".AI"));
    else
    	lstrcat(lpFile, _T(".TIF"));

	SetExtension(lpFile, idType, bReal);
}

//************************************************************************
//	returns the appropriate file type based on the file name given.
//	default is tiff.
//************************************************************************
int GetTypeFromExt(LPTSTR lpFile, int iDefault)
//************************************************************************
{
	FNAME szTest;
	int id;
	
    if( bImported )
    {
	    for(id = IDT_FIRSTIMPORTREAL; id <= IDT_LASTIMPORTREAL ; id++)
	    {
		    lstrcpy(szTest, lpFile);
		    SetExtension(szTest, (WORD)id, YES);
		    if (StringsEqual(szTest, lpFile))
			    return(id);
	    }
    }
    else
    {
	    for(id = IDT_FIRSTREAL; id <= IDT_LASTREAL ; id++)
	    {
		    lstrcpy(szTest, lpFile);
		    SetExtension(szTest, (WORD)id, YES);
		    if (StringsEqual(szTest, lpFile))
			    return(id);
	    }
    }
	return(iDefault);
}

//************************************************************************
//	Allows single selection list boxes to be handled like multi-selection
//		list boxes.  
//	Parameters same as windows 3.1 SendMessage (except bMultiSelection)
//	Send ONLY the Following multi selection messages:
//		LB_GETSELCOUNT, LB_GETSELITEMS, LB_SELITEMRANGE, LB_SETSEL
//************************************************************************
long ParseListBoxMessage(BOOL bMultiSelection, HWND hListBox, UINT uMsg, WPARAM wParam, LPARAM lParam)
//************************************************************************
{
	int item;
	
	switch (uMsg)
	{
		case LB_GETSELCOUNT:
			if (bMultiSelection)
				return (ListBox_GetSelCount(hListBox)); 
			item = ListBox_GetCurSel(hListBox);
			if (item == LB_ERR)
				return(0L);
			return(1L);
		case LB_GETSELITEMS:
			if (bMultiSelection)
				return (ListBox_GetSelItems(hListBox, wParam, lParam)); 
			if (wParam < 1 || !lParam)
				return(LB_ERR);
			item = ListBox_GetCurSel(hListBox);
			if (item == LB_ERR)
				return(LB_ERR);
			*(LPINT)lParam = item;
			return(1L);
		case LB_SELITEMRANGE:
			if (bMultiSelection)
				return (ListBox_SelItemRange(hListBox, wParam/*fSel*/, (int)LOWORD(lParam), (int)HIWORD(lParam))); 
			if (!wParam)
			{
				ListBox_SetCurSel(hListBox, -1);
				return(0);
			}	
			// select first item 
			item = ListBox_SetCurSel(hListBox, (int)LOWORD(lParam));
			return(item);
		case LB_SETSEL:
			if (bMultiSelection)
				return (ListBox_SetSel(hListBox, wParam, lParam)); 
			if (!wParam || HIWORD(lParam) == -1)
			{
				ListBox_SetCurSel(hListBox, -1);
				return(0);
			}	
			item = ListBox_SetCurSel(hListBox, lParam);
			return(item);
		default:
			return(LB_ERR);
	}
}

//************************************************************************
// 	changes the list box items string, without changing the index.
//	Returns FALSE if there are any problems.
//************************************************************************
BOOL ChangeListBoxItem(HWND hBox, int item, LPTSTR lpString)
//************************************************************************/
{
	int res;

	res = ListBox_DeleteString(hBox, item);
	if (res == LB_ERR)
		return(FALSE);
	res = ListBox_InsertString(hBox, item, lpString);
	if (res == LB_ERR)
		return(FALSE);
	if (res == LB_ERRSPACE)
	{	// TRY SOMETHING AT LEAST
		ListBox_InsertString(hBox, item, (LPTSTR)_T(""));
		return(FALSE);
	}
	return(TRUE);
}

//************************************************************************/
// 	Takes an album name.
//	Fixes it up to be acceptable if it can.
// 	Returns TRUE if the result is a valid Album Name.
//************************************************************************/
BOOL CheckAlbumName(LPTSTR lpString)
// Returns TRUE if character is not in range 0x00 - 0x1f || 0x7f
//************************************************************************/
{
	LPTSTR lp = lpString;

	while( *lp )
    {
		if( iscntrl( *lp ))
			return( FALSE );
        lp = MyCharNext( lp );
    }
	return( TRUE );
}

/************************************************************************/
BOOL BSREnumFiles(LPTSTR lpSrcName, LPTSTR lpDstName, BSRENUMPROC lpEnumFunc, long lData)
/************************************************************************/
{
FNAME   szSource, szDest, szSrcPath, szDstPath;
int     iCount;
unsigned ret;
DWORD attrib;
BOOL    bDestDir, bContinue;
FINDDATA afile;

lstrcpy(szSource, lpSrcName);
if (!MakeFullPath(szSource))
	return(0);
if (!lStrChr(szSource,_T('*')) && !lStrChr(szSource,_T('?')))
	{
	ret = Dos_GetFileAttr(szSource, &attrib);
	if (ret && (attrib & ATTRIB_SUBDIR))
		{
		FixPath(szSource);
		lstrcat(szSource, _T("*.*"));
		}
	}
bDestDir = NO;
szDest[0] = _T('\0');
if (lpDstName)
	{
	lstrcpy(szDest, lpDstName);
	if (!MakeFullPath(szDest))
		return(0);
	if (!lStrChr(szDest,_T('*')) && !lStrChr(szDest,_T('?')))
		{
		ret = Dos_GetFileAttr(szDest, &attrib);
		if (ret)
			bDestDir = attrib & ATTRIB_SUBDIR;
		if (bDestDir)
			FixPath(szDest);
		}
	}

ret = Dos_FindFirst( szSource, ATTRIB_NORMAL, &afile );
stripfile(szSource);
iCount = 0;
while ( ret )
	{
	if ( !(afile.dwAttrib & ATTRIB_SUBDIR) )
		{ // If this is a not subdirectory
		lstrcpy( szSrcPath, szSource );
		lstrcat( szSrcPath, afile.szName );
		lstrcpy( szDstPath, szDest);
		if (bDestDir)
			{
			lstrcat(szDstPath, afile.szName);
			if (StringsEqual(szSrcPath, szDstPath))
				goto    NextFile;
			}
		if (lpEnumFunc)
			{
			bContinue = (*lpEnumFunc)((LPTSTR)szSrcPath,
					(LPTSTR)szDstPath, lData, iCount);
			if (!bContinue)
				return(iCount+1);
			}
		++iCount;
		}
NextFile:
	ret = Dos_FindNext( &afile );
	}
return(iCount);
}

//************************************************************************
BOOL CopyFile1(LPTSTR lpInFile, LPTSTR lpOutFile, BOOL bClock, BOOL bEscapable )
//************************************************************************
{
	int i;
	FILE_HANDLE ifp, ofp;					
	TCHAR Buffer[2048];
	int	nTotal, nCount;
	
	nTotal = FileSize(lpInFile) / 2048;
	lstrcpy( Buffer, lpInFile );
	if ((ifp = FileOpen( Buffer, FO_READ )) == FILE_HANDLE_INVALID)
		return( FALSE );
	lstrcpy( Buffer, lpOutFile );
	if ((ofp = FileOpen( Buffer, FO_CREATE|FO_READ|FO_WRITE )) == FILE_HANDLE_INVALID)
		{
		FileClose( ifp );
		return( FALSE );
		}
	
	i = sizeof(Buffer);
	nCount = 0;
	while( 1 )
		{
		if (bClock)
			{
			if (AstralClockCursor(nCount, nTotal, bEscapable))
				{
				FileClose( ifp );
				FileClose( ofp );
				lstrcpy( Buffer, lpOutFile );
				FileDelete(Buffer);
				return( FALSE );
				}
			}
		if ( (i = FileRead(ifp, Buffer, i)) <= 0 )
			{
			FileClose( ifp );
			FileClose( ofp );
			return( i == 0 );
			}
		if ( FileWrite(ofp, Buffer, i) != i )
			{
			FileClose( ifp );
			FileClose( ofp );
			lstrcpy( Buffer, lpOutFile );
			FileDelete(Buffer);
			return( FALSE );
			}
		++nCount;
		}
}

//************************************************************************
//		GetBsrSateStruct
// DESCRIPTION:
//		Allocates memory for a BSRSTATE.
//		Fills any default values.
// RETURNS:
//		A pointer to a BSRSTATE structure. NULL for failure.
//************************************************************************
LPBSRSTATE GetBsrStateStruct()
//************************************************************************
{
	LPBSRSTATE lpBsrState;
	
	lpBsrState = (LPBSRSTATE)Alloc(sizeof(BSRSTATE));
	if (!lpBsrState)
		return(NULL);
	lpBsrState->OpenState.bInitialized = FALSE;
	lpBsrState->lpOpenState = NULL;
	lpBsrState->bDisableCancel = NO;
	lpBsrState->SelBuf = NULL;	// for the file list...MUST BE SET UP
	lpBsrState->lpAlbList = NULL; 	// current album in album double linked list
	lpBsrState->DriveTypes = NULL;	// cached drive types
	lpBsrState->DriveVolumes = NULL;	// cached drive volumes
	lpBsrState->sDate[0] = _T('\0');		// for international date& time
	lpBsrState->sTime[0] = _T('\0');		// for international date& time
	lpBsrState->sAMPM[0][0] = _T('\0');	// for international date& time
	lpBsrState->sAMPM[1][0] = _T('\0');	// for international date& time
	return(lpBsrState);
}

//************************************************************************
//		FreeBsrSateStruct
// DESCRIPTION:
//		Frees memory for a BSRSTATE.
//************************************************************************
void FreeBsrStateStruct(LPBSRSTATE lpBsrState)
//************************************************************************
{
	if (lpBsrState)
		FreeUp(lpBsrState);
}

//***********************************************************************
//		GetParentBsrState
// PARAMETERS:
//			HWND hDlg- the child dialog
//			BOOL bGetMine- get child's user data also?
// DESCRIPTION:
//			Sets lpBsr to be the the parents lpBsr.
// RETURNS:
//			NULL if fails.
//			lpBsr if bGetMine==FALSE.
//			hDlg's DWL_USER if bGetMine = TRUE;
//************************************************************************
long GetParentBsrState(HWND hDlg, BOOL bGetMine)
//************************************************************************
{				 
	HWND hParent = GetParent(hDlg);
	
	if (!hParent)
		return(0L);
	lpBsr = (LPBSRSTATE)GetWindowLong(hParent, DWL_USER);
	if (!lpBsr)	
		return(0L);
	if (!bGetMine)
		return((LONG)lpBsr);
	return(GetWindowLong(hDlg, DWL_USER));	
}

//************************************************************************
BOOL DWL_USER_OnCreate(HWND hDlg, CREATESTRUCT FAR* lpCreateStruct)
/***************************************************************************/
{
	SetWindowLong(hDlg, DWL_USER, 0L);
	return(TRUE);
}

/***************************************************************************/
void DWL_USER_OnDestroy(HWND hDlg)
/***************************************************************************/
{
	LPTR lptr;
	
	lptr = (LPTR)GetWindowLong(hDlg, DWL_USER);
	if (lptr)
		FreeUp(lptr);
}
