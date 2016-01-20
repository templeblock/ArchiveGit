//
// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "bsrtypes.h"
#include "dbfmgr.h"
#include "bsrutil.h"
#include "macros.h"

// defines for DBFBeginInfo, DBFNextInfo etc.
#define INVALIDATE_FINDDATA(fdata) \
		((fdata).szName[0] = _T('\0'))
#define ISVALID_FINDDATA(fdata) \
		((fdata).szName[0] != _T('\0'))

	 	
//***********************************Static Vars***************************************
static int filesOpen = 0;		

// ***FIX can be done another way (by looking at ext)
static WORD InfoType;			// for DBFNextInfo.		

//***********************************Static Prototypes***************************************
LOCAL LPDBF DBF_open_dbf(LPTSTR lpName, LPVOLUMEID lpVolume, LPDBF lpOutDBF, BOOL bWrite, WORD idType, int drive);
LOCAL LPDBF DBF_new_dbf(LPTSTR lpName, LPVOLUMEID lpVolume, LPDBF lpOutDBF, WORD idType);
LOCAL BOOL	DBF_goto_first(LPDBF lpDBF);
LOCAL BOOL	DBF_flush(LPDBF lpDBF);
LOCAL BOOL DBF_read_info(LPDBFINFO lpInfo, FILE_HANDLE hfile, BOOL bSeek);
LOCAL BOOL DBF_write_info(LPDBFINFO lpInfo, FILE_HANDLE hfile, BOOL bSeek);
LOCAL BOOL DBF_read_entry(LPDBF lpDBF, LPENTRY lpEntry);
LOCAL BOOL DBF_write_entry(LPDBF lpDBF, LPENTRY lpEntry);

void testdbfmgr(void)
{
	if (filesOpen == 0)
	 	return;
	MessageBeep(-1);
	MessageBeep(-1);
	MessageBeep(-1);
	MessageBeep(-1);
}

LOCAL FILE_HANDLE lOpen(LPTSTR lptr, int i)
{
	FILE_HANDLE ret;
	ret = FileOpen(lptr, i);
	if (ret != FILE_HANDLE_INVALID)
		filesOpen++;
	return(ret);
}

LOCAL BOOL lClose(FILE_HANDLE hf)
{
	BOOL ret;
	if (ret = FileClose(hf))
		filesOpen--;
	return(ret);
}

LOCAL FILE_HANDLE lCreat(LPTSTR lptr, int i)
{
	FILE_HANDLE ret;
	ret = FileOpen(lptr, FO_CREATE|i);
	if (ret != FILE_HANDLE_INVALID)
		filesOpen++;
	return(ret);
}

//**************************************************************************
// 	NOTE: ALL ROUTINES WORK ON OPEN DBF's ONLY
//**************************************************************************


//**************************************************************************
// 	opens the dbf for the album and puts the result in lpOutDBF
// 	returns lpOutDBF if successful, or NULL for failure  
//**************************************************************************
LPDBF 		DBFOpenAlbum(LPTSTR lpAlbum, LPDBF lpOutDBF, BOOL bWrite)
//**************************************************************************
{
	VOLUMEID Volume;
	
	CLEARVOLUME(Volume);
	return(DBF_open_dbf(lpAlbum, &Volume, lpOutDBF, bWrite, ID_ALBUM, 0 /*not used*/));
}	

//**************************************************************************
// 	Opens the specified dbf file. 
//	lpDBFile is the name of the DBF file.
//	Sets up lpOutDBF structure.
// 	Returns lpOutDBF if successful, NULL otherwise.
//**************************************************************************
LPDBF		DBFOpenByFile(LPTSTR lpDBFile, LPDBF lpOutDBF, BOOL bWrite)
//**************************************************************************
{
	FILE_HANDLE hfDBF;

	hfDBF = lOpen(lpDBFile,(bWrite ? (FO_WRITE|FO_READ):(FO_READ)));
	if ( hfDBF == FILE_HANDLE_INVALID)
		goto ExitFalse;
	// read header
	if (!DBF_read_info(&(lpOutDBF->info), hfDBF, NO/*need to seek*/))
		goto ExitFalse;
		
	// must be compatible 
	if (lpOutDBF->info.fDBFType & PP4DBFTYPE)
	{
		lpOutDBF->EntrySize = 2*80+4*sizeof(LONG);		// ENTRY size for pp40
		lpOutDBF->PathSize = 80;						// FNAME size for pp40
	}
	else if (lpOutDBF->info.fDBFType & DBFTYPE)
	{
		lpOutDBF->EntrySize = DBFENTRYSIZE;
		lpOutDBF->PathSize = sizeof(FNAME);
	}
	else 
		goto ExitFalse;
		
	// Pack the structure
	lstrcpy(lpOutDBF->szFile, lpDBFile);
	lpOutDBF->bOpen = TRUE;
	lpOutDBF->bWrite = bWrite;
	lpOutDBF->file = hfDBF;
	lpOutDBF->pos = -1;	// not ready to read any entry
	return(lpOutDBF);
	
ExitFalse:
	lpOutDBF->bOpen = FALSE;
	lpOutDBF->file = FILE_HANDLE_INVALID;
	if (hfDBF != FILE_HANDLE_INVALID)
		lClose(hfDBF);
	return(NULL);
}
	
//**************************************************************************
// 	opens the dbf for the file (by directory) and puts the result in lpOutDBF.
// 	Can take a (full)file name or just a path(must end in '\') 
// 	returns lpOutDBF if successful, or NULL for failure.
//**************************************************************************
LPDBF		DBFOpenDirectory(LPFILELOC lpFile, LPDBF lpOutDBF, BOOL bWrite)
//**************************************************************************
{
	FNAME szPath;
	
	lstrcpy(szPath, lpFile->szFile);
	stripfile(szPath);
	return(DBF_open_dbf(szPath, &lpFile->volume, lpOutDBF, bWrite, ID_DIRECTORY, szPath[0]));
}

//**************************************************************************
//	Opens a DBF previously closed by DBFClose, pointed to by lpDBF.
// 	Returns lpDBF if successful or lpDBF is already open, and NULL if failure.
//**************************************************************************
LPDBF		DBFReOpen(LPDBF lpDBF, BOOL bWrite)
//**************************************************************************
{
	FILE_HANDLE  hfile;
	if (!lpDBF)
		return(NULL);
	if (lpDBF->bOpen && (!lpDBF->bWrite || bWrite))
		return(lpDBF);
	if (lpDBF->bOpen)
		lClose(lpDBF->file);
	hfile = lOpen(lpDBF->szFile,(bWrite ? (FO_WRITE|FO_READ):(FO_READ)));
	if (hfile == FILE_HANDLE_INVALID) 
		return(NULL);
	lpDBF->bOpen = TRUE;
	lpDBF->bWrite = bWrite;
	lpDBF->file = hfile;
	return(lpDBF);
}

//**************************************************************************
// Returns the entry position of lpFile in lpDBF, or -1 if it is not a member.
// Sets entry pointer to begining of dbf.
//**************************************************************************
int			DBFFindEntry(LPDBF lpDBF, LPFILELOC lpFile)
//**************************************************************************
{
	BOOL ok;
	int count;
	ENTRY	entry;
	
	if (!lpDBF)
		return(-1);
	if (!lpDBF->bOpen)
		return(-1);
	// goto begining of entries
	if (!DBF_goto_first(lpDBF))
		return(-1);
	lpDBF->pos = -1;
	// search entires
	ok = DBF_read_entry(lpDBF, &entry);
	count = 0;
	lpDBF->pos++;
	while (ok)
	{
		if (EQFILELOC(entry.File, *lpFile))
			return(count);
		ok = DBF_read_entry(lpDBF, &entry);
		count++;
	}
	// not a member
	return(-1);
}


//**************************************************************************
// Gets the entry with the file name lpFile, and puts it into lpOutEntry.
// Sets entry pointer to entry after the one found.
// Returns lpOutEntry if successful, and NULL if fails.
// If it fails lpOutEntry is unchanged.
//**************************************************************************
LPENTRY		DBFGetEntry(LPDBF lpDBF, LPFILELOC lpFile, LPENTRY lpOutEntry)
//**************************************************************************
{
	BOOL ok;
	ENTRY entry;
	
	// goto begining of entries
	if (!DBF_goto_first(lpDBF))
		return(NULL);
	// search entires
	ok = DBF_read_entry(lpDBF, &entry);
	lpDBF->pos++;
	while (ok)
	{
		if (EQFILELOC(entry.File, *lpFile))
		{
			*lpOutEntry = entry;
			return(lpOutEntry);
		}
		ok = DBF_read_entry(lpDBF, &entry);
		lpDBF->pos++;
	}
	lpDBF->pos = -1;	// reset pos
	return(NULL);
}

//**************************************************************************
// Gets the entry and puts it into lpOutEntry.
// first entry is num=0.
// If num = -1, sets entry pointer to begining of file (still returns lpOutEntry).
// Sets entry pointer to entry after the one found.
// Returns lpOutEntry if successful, and NULL if fails.
//**************************************************************************
LPENTRY		DBFGetEntryNum(LPDBF lpDBF, int num, LPENTRY lpOutEntry)
//**************************************************************************
{
	BOOL ok;
	LONG pos;

	if (!lpDBF)
		return(NULL);
	if (!lpDBF->bOpen)
		return(NULL);
	lpDBF->pos = -1;	// assume failure
	if (num < 0)
		return(lpOutEntry);		// pos = -1 so it is setup to read begining 
	pos = lpDBF->info.lData + (long)num*lpDBF->EntrySize;
	if (pos != FileSeek(lpDBF->file, pos, 0 /*begining*/))
		return(NULL);
	ok = DBF_read_entry(lpDBF, lpOutEntry);
	if (!ok)
		return(NULL);
	// success
	lpDBF->pos = num+1;
	return(lpOutEntry);		
}

//**************************************************************************
// Gets the next entry and puts it into lpOutEntry.
// Future calls to GetNextEntry etc. will work from this new position.(or begining if failure)
// Returns lpOutEntry if successful, and NULL if fails.
//**************************************************************************
LPENTRY		DBFGetNextEntry(LPDBF lpDBF, LPENTRY lpOutEntry)
//**************************************************************************
{
	BOOL ok;
	
	if (!lpDBF)
		return(NULL);
	if (!lpDBF->bOpen)
		return(NULL);
	// if pos not valid then start from start
	if (lpDBF->pos < 0)
		if (!DBF_goto_first(lpDBF))
			return(NULL);
	ok = DBF_read_entry(lpDBF, lpOutEntry);
	if (!ok)
	{
		lpDBF->pos = -1;		
		return(NULL);
	}
	// success
	lpDBF->pos++;
	return(lpOutEntry);		
}

//**************************************************************************
// Replaces the current entry with lpEntry.
// The current entry is specified by calls to DBFGetEntryNum,
//		DBFGetNextEntry, etc.
// Returns TRUE if successful.
// Failure may result inreseting of entry pointer.
//**************************************************************************
BOOL		DBFChangeCurrentEntry(LPDBF lpDBF, LPENTRY lpEntry)
//**************************************************************************
{
	LONG  bytes;
	BOOL ok;
	int num;
	ENTRY entry; 
	
	if (!lpDBF)
		return(FALSE);
	if (!lpDBF->bOpen)
		return(FALSE);
	if (lpDBF->pos < 0)
		return(FALSE);
	// convert to 50 format
	if (!DBFConvert(lpDBF, YES/*save state*/, YES/*msg*/))
		return(FALSE);
	num = lpDBF->pos;
	lpDBF->pos = -1;		
	// backup
	bytes = FileSeek(lpDBF->file, -(int)lpDBF->EntrySize, 1 /*current pos*/);
	if (bytes == FILE_DWERR || bytes < lpDBF->info.lData)
		return(FALSE);
	// write
	ok = DBF_write_entry(lpDBF, lpEntry);
	if (!ok)
		return(FALSE);
	if (!DBF_flush(lpDBF))
		return(FALSE);
	// get old position back
	return(DBFGetEntryNum(lpDBF,num-1,&entry)!=NULL);
}

//**************************************************************************
// 	Will change the data in an entry if an entry already 
//		exists for the file lpOldFile.
//	Otherwise it will add the entry to the dbf.
// 	May call DBFUpdateInfo on lpDBF.
// 	lpOldFile can be NULL (lpEntry->szFile will be used)
// 	Resets DBF entry pointer to begining. 
//	Returns TRUE if successful.
//	volume is the volume id for the entry to be replaced.
// 	If volume == -1L then volume is ignored.
//**************************************************************************
BOOL 		DBFPutEntry(LPDBF lpDBF, LPENTRY lpEntry, LPFILELOC lpOldFile)
//**************************************************************************
{
	ENTRY CmpEntry;
	LONG  bytes;
	BOOL ok;
	FILELOC SearchFile;
	
	// convert to 50 format
	if (!DBFConvert(lpDBF, NO/*save state*/, YES/*msg*/))
		return(FALSE);
	
	if (lpOldFile)
		SearchFile = *lpOldFile;
	else
		SearchFile = lpEntry->File;
			
	if (!DBFGetEntryNum(lpDBF, -1, &CmpEntry))
		return(FALSE);
	while(DBFGetNextEntry(lpDBF, &CmpEntry))
		if (EQFILELOC(CmpEntry.File, SearchFile))
		{	// found match now overwrite it
			// Backup	
			lpDBF->pos = -1;
			bytes = FileSeek(lpDBF->file, -(int)lpDBF->EntrySize, 1 /*current pos*/);
			if (bytes == FILE_DWERR || bytes < lpDBF->info.lData)
				return(FALSE);
			// write
			ok = DBF_write_entry(lpDBF, lpEntry);
			if (!ok)
				return(FALSE);
			return(DBF_flush(lpDBF));
		}
	return(DBFAddEntry(lpDBF, lpEntry));
}

//**************************************************************************
//	Will add the entry to the dbf.  
// 	DOES NOT CHECK TO SEE IF ENTRY IS ALREADY IN THE DBF...
//		... be careful (See DBFPutEntry).
//	Will call DBFUpdateInfo on lpDBF.
// 	Resets DBF entry pointer to begining. 
//	Returns TRUE if successful.
//**************************************************************************
BOOL 		DBFAddEntry(LPDBF lpDBF, LPENTRY lpEntry)
//**************************************************************************
{
	LONG  bytes;		
	
	// convert to 50 format
	if (!DBFConvert(lpDBF, NO/*save state*/, YES/*msg*/))
		return(FALSE);
	// ensure that we have current info.numEntry
	if (!DBFUpdateInfo(lpDBF))
		return(FALSE);
	// end of file is calculated as insurance against a previous error
	bytes = lpDBF->info.lData + lpDBF->info.numEntry*(long)lpDBF->EntrySize;
	if (bytes != FileSeek(lpDBF->file, bytes, 0 /*begining*/))
		return(FALSE);
	// write
	if (!DBF_write_entry(lpDBF, lpEntry))
		return(FALSE);
	// change and write numEntry
	lpDBF->info.numEntry++;
	if (!DBF_write_info(&(lpDBF->info), lpDBF->file, YES/*need to seek*/))
		return(FALSE); 		// VERY BAD NEWS!
	return(DBF_flush(lpDBF));
}								 

//**************************************************************************
//	Deletes the entry associated with the given file name from the dbf.
//	Will call DBFUpdateInfo on lpDBF.
//	Returns DBF_SUCCESS if suceessfull.
//	Returns DBF_NOENTRY if the entry could not be found.
//	Returns DBF_FAILURE for genral failure.
//**************************************************************************
int			DBFDeleteEntry(LPDBF lpDBF, LPFILELOC lpFile)
//**************************************************************************
{
	ENTRY	entry;
	LONG	end, remain, bytes;
	LPTSTR	lpBuf;

	if (!lpDBF)
		return(DBF_FAILURE);
	if (!lpDBF->bOpen)
		return(DBF_FAILURE);
		
	// be sure we have current info
	if (!DBFUpdateInfo(lpDBF))
		return(FALSE);

	// goto begining of entries
	DBFGetEntryNum(lpDBF, -1, &entry);
	// search entires
	while (DBFGetNextEntry(lpDBF, &entry))
	{
		if (EQFILELOC(entry.File, *lpFile))
		{
			// found it, now backup and delete
			bytes = FileSeek(lpDBF->file, 0, 1 /*current pos*/);
			if (bytes == FILE_DWERR)
				return(DBF_FAILURE);
			end = FileSeek(lpDBF->file, 0, 2 /*eof*/);
			if (end == FILE_DWERR)
				return(DBF_FAILURE);
			// copy entries down
			remain = end - bytes;
			lpBuf = (LPTSTR)LineBuffer[0];
			if (remain <= 0)
				if(FileSeek(lpDBF->file, bytes-lpDBF->EntrySize, 0 /*begining*/) == FILE_DWERR)
					return(DBF_FAILURE);
			while(remain > 0)
			{
				if (FileSeek(lpDBF->file, bytes, 0 /*begining*/) == FILE_DWERR)
					return(DBF_FAILURE);
				// read data
				end = FileRead(lpDBF->file, lpBuf, (UINT)Min(remain, 0xff00));
				if (end == FILE_DWERR)
					return(DBF_FAILURE);
				// write data
				if(FileSeek(lpDBF->file, bytes-lpDBF->EntrySize, 0 /*begining*/) == FILE_DWERR)
					return(DBF_FAILURE);
				if ((long)FileWrite(lpDBF->file, lpBuf, (UINT)end) != end)
					return(DBF_FAILURE);
				remain =- end;
			}	// while remain >0
			// truncate file
			if (!FileTruncate(lpDBF->file))
				return(DBF_FAILURE);
			// change and write numEntry
			if (lpDBF->info.numEntry)
				lpDBF->info.numEntry--;
			if (!DBF_write_info(&(lpDBF->info), lpDBF->file, YES/*need to seek*/))
				return(DBF_FAILURE); // VERY BAD NEWS!
			if (!DBF_flush(lpDBF))
				return(DBF_FAILURE);
			return(DBF_SUCCESS);	
		}	// if found memeber
	}	// while searching through members
	// not a member
	return(DBF_NOENTRY);
}


//**************************************************************************
// 	Writes the lpDBF->info to the dbf file.
//	sets info->numEntry to that of the file BEFORE writing 
//		to ensure that it is not changed to an old value.
// 	Resets DBF entry pointer to begining. 
// 	Returns TRUE if successful.
//**************************************************************************
BOOL 		DBFPutInfo(LPDBF lpDBF)
//**************************************************************************
{
	DBFINFO info;
	
	if (!lpDBF)
		return(FALSE);
	if (!lpDBF->bOpen)
		return(FALSE);

	lpDBF->pos = -1;	
	// get current info so that numEntry is not changed
	if (!DBF_read_info(&info, lpDBF->file, YES/*need to seek*/))
		return(FALSE);	
	// 	set numEntry on info to  write
	lpDBF->info.numEntry = info.numEntry;
	if (!DBF_write_info(&(lpDBF->info), lpDBF->file, YES/*need to seek*/))
		return(FALSE); 		// BAD NEWS!
	return(DBF_flush(lpDBF));
}

//**************************************************************************
//  Updates lpDBF->info from the dbf file.
// 	Resets DBF entry pointer to begining. 
// 	If it fails then lpDBF->info is unchanged.
// 	Returns TRUE if successful.
//**************************************************************************
BOOL 		DBFUpdateInfo(LPDBF lpDBF)
//**************************************************************************
{
	DBFINFO info;
	
	if (!lpDBF)
		return(FALSE);
	if (!lpDBF->bOpen)
		return(FALSE);

	lpDBF->pos = -1;	
	if (!DBF_read_info(&info, lpDBF->file, YES/*need to seek*/))
		return(FALSE);	
	lpDBF->info = info;
	return(TRUE);
}	


//**************************************************************************
//	Creates a new (open) Album DBF in Browser.DBFInfoDir.  Sets bDuplicate = FALSE.
//	Puts it into lpOutDBF.
// 	If an album with this name already exists then it uses a copy of that.
// 		And sets bDuplicate =  TRUE;
//	Sets DBF entry pointer to begining.
//	Returns lpOutDBF if successful, NULL otherwise.	
//**************************************************************************
LPDBF		DBFNewAlbum(LPTSTR lpAlbum, LPDBF lpOutDBF, BOOL far *bDuplicate)
//**************************************************************************
{
	VOLUMEID 	Volume;

	// does one already exist?
	if (DBFOpenAlbum(lpAlbum, lpOutDBF, YES))
	{
		*bDuplicate = TRUE;
		return(lpOutDBF);
	}
	// get new dbf
	*bDuplicate = FALSE;
	CLEARVOLUME(Volume);
	if (DBF_new_dbf(lpAlbum, &Volume, lpOutDBF, ID_ALBUM) == NULL)
		return(NULL);
	if (!DBF_flush(lpOutDBF))
	{	// delet it
		
		FileDelete(lpOutDBF->szFile);
		return(FALSE);
	}
	return(lpOutDBF);
}

//**************************************************************************
//	Creates a new (open) Directory DBF in Browser.DBFInfoDir for 
//		the directory and volume given in lpFile.
// 	Puts it into lpOutDBF.
// 	If a dbf for that dir name already exists then it uses a copy of that.
//	lpName may have a file name in it. 
// 	if bInDrive== TRUE then it will Try to add our mark to any 
//		writable & removable media (if not already there) and will change lpFile.
//	Sets DBF entry pointer to begining.
//	Returns lpOutDBF if successful, NULL otherwise.	
//**************************************************************************
LPDBF		DBFNewDirectory(LPFILELOC lpFile, LPDBF lpOutDBF, BOOL bInDrive)
//**************************************************************************
{
	FNAME szPath;
	FILELOC File;
	int drive, type;
	
	// does one already exist?
	if (DBFOpenDirectory(lpFile, lpOutDBF, YES))
		return(lpOutDBF);
	// Mark Drive if we can
	if (lpFile->volume.OurMark == VOLUME_ANY && bInDrive)
	{
		drive = lpFile->szFile[0];
		drive = WITHIN(drive, _T('a'),_T('z')) ? drive-_T('a'): 
				WITHIN(drive, _T('A'),_T('Z')) ? drive-_T('A'):drive;
		if (WITHIN(drive, 0, 25)) 
		{		
			type = BSRGetDriveType(drive);
			if (type == DRIVE_REMOVABLE)
			{
				lpFile->volume.OurMark =  getOurMark(drive);
				if (lpFile->volume.OurMark == VOLUME_ANY)
					lpFile->volume.OurMark =  putOurMark(drive);
			}	
		}
	}
	// get new dbf
	File = *lpFile;
	lstrcpy(szPath, lpFile->szFile);
	stripfile(szPath);

	if (DBF_new_dbf(szPath, &lpFile->volume, lpOutDBF, ID_DIRECTORY) == NULL)
		return(NULL);
	if (!DBF_flush(lpOutDBF))
	{	// delet it
		FileDelete(lpOutDBF->szFile);
		return(NULL);
	}
	return(lpOutDBF);
}

//**************************************************************************
// 	Closes DBF.
// 	DBF may be reopened using DBFReOpen.
//	Returns TRUE if successful.
//**************************************************************************
BOOL		DBFClose(LPDBF lpDBF)
//**************************************************************************
{
	if (!lpDBF->bOpen)
		return(TRUE);
	if (!lClose(lpDBF->file))
		return(FALSE);
	lpDBF->pos = -1;
	lpDBF->file = 0;
	lpDBF->bOpen = FALSE;
	return(TRUE);	
}

//**************************************************************************
//	Gets a file name for a new dbf.
//	the result goes int lpResult (if successful).
//	lpDBFPath is the directory for the new dbf.
//	lpDBFName is the album or directory name.
//	idType should be ID_ALBUM or ID_DIRECTORY.
//	Returns false for failure.
//**************************************************************************
BOOL 		GetNewDBFName(LPTSTR lpResult, LPTSTR lpDBFPath, LPTSTR lpDBFName, int idType)
//**************************************************************************
{
	FNAME szExt;
	int len;
	
	lstrcpy(lpResult, lpDBFPath);
	MakeFullPath(lpResult);
	FixPath(lpResult);
	if (idType == ID_ALBUM)
		lstrcpy(szExt, ALBUM_EXT);
	else
	{
		MakeFullPath(lpDBFName);
		lstrcpy(szExt, DBFILE_EXT);
		len = lstrlen(lpResult);
		lpResult[len] = lpDBFName[0];
		lpResult[len+1] = _T('\0');
	}
	if (!GetTempFilename(lpResult, szExt))
		return(FALSE); 
	return(TRUE);
}

//**************************************************************************
// 	Routines to get the info on All dbf's of a given type 
//**************************************************************************

//**************************************************************************
//	Call this to setup, before calling DBFNextAlbumInfo.
// Pass in an unitialized FINDDATA structure.
//**************************************************************************
void 		DBFBeginInfo(LPFINDDATA lpafile, WORD idType)
{
	FNAME szDBF;
	FINDDATA afile;
	
	// create dbf search string;
	lstrcpy(szDBF, Browser.DBFInfoDir);
	lstrcat(szDBF, _T("*"));
	if (idType == ID_ALBUM)
		lstrcat(szDBF, ALBUM_EXT);
	else
		lstrcat(szDBF, DBFILE_EXT);
	
	// get first dbf
	afile = *lpafile;	
	if (!Dos_FindFirst(szDBF, ATTRIB_NORMAL, &afile))
		INVALIDATE_FINDDATA(afile);
	*lpafile = afile;	
	InfoType = idType;
	
}

//**************************************************************************
//	Call this to setup, before calling DBFNextAlbumInfo for DIRECTORIES only.
//	Use DBFBeginInfo if the drive is not known.
//  Pass in an unitialized FINDDATA structure.
//	Can use DBFBeginInfo if drive is not known
//	drive can be a number (0 based) or a letter.
//**************************************************************************
void 		DBFBeginDirInfo(LPFINDDATA lpafile, WORD drive)
{
	FNAME szDBF;
	FINDDATA afile;
	int len;
	
	if (drive < 26)
		drive += _T('A');
	
	// create dbf search string;
	lstrcpy(szDBF, Browser.DBFInfoDir);
	len = lstrlen(( LPTSTR )szDBF );
	szDBF[len] = drive;
	szDBF[len+1] = _T('\0');
	lstrcat(szDBF, _T("*"));
	lstrcat(szDBF, DBFILE_EXT);
	 
	// get first dbf
	afile = *lpafile;	
	if (!Dos_FindFirst(szDBF, ATTRIB_NORMAL, &afile))
		INVALIDATE_FINDDATA(afile);
	*lpafile = afile;	
	InfoType = ID_DIRECTORY;
	
}

//**************************************************************************
//  Gets the info on the next dbf.  Call DBFBeginInfo first.
//	Pass in the FINDDATA structure from DBFBeginInfo.
//	Returns NULL for failure.
// 	If (!bClose) then leaves the dbf open (for reading only).
//**************************************************************************
LPDBF 		DBFNextInfo(LPDBF lpOutDBF, LPFINDDATA lpafile, BOOL bWrite, BOOL bClose)
//**************************************************************************
{

	BOOL found;
	FILE_HANDLE hfDBF;
	FNAME szDBF;
	FINDDATA afile;
	
	// are there any more dbf's?
	if (!ISVALID_FINDDATA(*lpafile))
		return(NULL);	
		
	// for near far , must have local find_t
	afile = *lpafile;	
	// check from current dbf for valid album
	hfDBF = FILE_HANDLE_INVALID;
	found = FALSE;
	while (!found)
	{
		if (afile.dwAttrib & ATTRIB_SUBDIR)
			goto NextFile;
		// check if correct dbf
		lstrcpy(szDBF, Browser.DBFInfoDir);
		lstrcat(szDBF, afile.szName);
		hfDBF = lOpen(szDBF, (bWrite ? (FO_WRITE|FO_READ):(FO_READ)));
		if ( hfDBF == FILE_HANDLE_INVALID)
			goto NextFile;
		// read header
		if (!DBF_read_info(&(lpOutDBF->info), hfDBF, NO/*need to seek*/))
			goto NextFile;
		// must be compatible 
		if (!(lpOutDBF->info.fDBFType & DBFTYPE))
			if (!(lpOutDBF->info.fDBFType & PP4DBFTYPE))
				goto NextFile;
		// must be an correct type
		if (lpOutDBF->info.idType != InfoType)
			goto NextFile;
		found = TRUE;
NextFile:
		// close old file  (if needed)
		if (hfDBF != FILE_HANDLE_INVALID && (bClose || !found))
		{ // close file
			lClose(hfDBF);
			hfDBF = FILE_HANDLE_INVALID;
		}
		// find next
		if (!Dos_FindNext(&afile))
		{ 	// failure 
			INVALIDATE_FINDDATA(afile);
			if (!found)
				return(NULL);
		}
	}	// while !found

	// this is it!	Pack the structure
	lstrcpy(lpOutDBF->szFile, szDBF);
	lpOutDBF->bOpen = !bClose;
	lpOutDBF->bWrite = bWrite;
	lpOutDBF->file = hfDBF;	// FILE_HANDLE_INVALID if not open
	lpOutDBF->pos = -1;	// not ready to read any entry
	if (lpOutDBF->info.fDBFType & PP4DBFTYPE)
	{
		lpOutDBF->EntrySize = 2*80+4*sizeof(LONG);		// ENTRY size for pp40
		lpOutDBF->PathSize = 80;						// FNAME size for pp40
	}
	else // (lpOutDBF->info.fDBFType & DBFTYPE)
	{
		lpOutDBF->EntrySize = DBFENTRYSIZE;
		lpOutDBF->PathSize = sizeof(FNAME);
	}
	*lpafile = afile;	
	return(lpOutDBF);
}


//**************************************************************************
// Converts a dbf to 50 format.
// This is normally done on an as needed basis by the other DBF routines.
// if bSaveState==TRUE then the file pointer is returned to its current pos.
// if bMsg then the user is made aware that a dbf is being converted.
// Returns FALSE if there is an error.
//**************************************************************************
BOOL 		DBFConvert(LPDBF lpDBF, BOOL bSaveState, BOOL bMsg)
//**************************************************************************
{
	int pos;
	BOOL bOpen;
	BOOL bWrite;
	DBFS dbf;
	ENTRY entry;
	
	if (!lpDBF)
		return(FALSE);
	
	if (lpDBF->info.fDBFType & DBFTYPE)
		return(TRUE); 
		
	if (bMsg)
		MessageBox(NULL, &_T("Converting DBF to 5.0 format"), &_T("Notice"), MB_OK|MB_TASKMODAL);
		
	// get old data
	pos = lpDBF->pos;
	bOpen = lpDBF->bOpen;
	bWrite = lpDBF->bWrite;
	
	// open source 
	if (DBFReOpen(lpDBF, NO) == NULL)
		return(FALSE); // no conversion
	
	// create dst
	if (DBF_new_dbf(lpDBF->info.szName, 
					&lpDBF->info.volume, 
					&dbf, 
					lpDBF->info.idType) == NULL)
		return(FALSE);	// no conversion
	
	// copy over entries
	DBF_goto_first(&dbf);
	DBF_goto_first(lpDBF);
	while(DBFGetNextEntry(lpDBF,&entry))
	{
		// DBFPutEntry
		if (!DBF_write_entry(&dbf, &entry))
			goto ExitBadError; 
		dbf.info.numEntry++;
	}
	
	// write updated dbf info
	if (!DBF_write_info(&(dbf.info), dbf.file, YES/*seek*/))
		goto ExitBadError; 
	if (!DBF_flush(&dbf))
		goto ExitBadError; 
	
	DBFClose(lpDBF);
	DBFClose(&dbf);

	// rename new dbf (if can)
	if (FileDelete(lpDBF->szFile))
		if (RenameFile(dbf.szFile, lpDBF->szFile))
			lstrcpy(dbf.szFile,lpDBF->szFile);			
	
	// update album list if needed
	// Probably should do this elsewhere.
	if (lpDBF->info.idType == ID_ALBUM)
		CreateAlbList();

	// copy over all info and restore state
	*lpDBF = dbf;
	if (bOpen)
		DBFReOpen(lpDBF, bWrite);
	if (bOpen && bSaveState && pos > 0)
		if (!DBFGetEntryNum(lpDBF, pos-1, &entry))
			return(FALSE);	// not severe
	return(TRUE);
	
ExitBadError:
	if (!bOpen)
		DBFClose(lpDBF);
	DBFClose(&dbf);
	FileDelete(dbf.szFile);
	return(FALSE);
	
}


//**************************************************************************
//************************SATATIC ROUTINES**********************************
//**************************************************************************


//**************************************************************************
// 	Opens the dbf with proper name and type.
//	Sets DBF entry pointer to the begining (GetNext will get first entry).
// 	Returns lpOutDBF if successful, or NULL for failure.
// 	If *lpVolume == -1L then volume is ignored.
//	Drive is used for ID_DIRECTORY type only.
//**************************************************************************
LOCAL LPDBF DBF_open_dbf(LPTSTR lpName, LPVOLUMEID lpVolume, LPDBF lpOutDBF, BOOL bWrite, WORD idType, int drive)
//**************************************************************************
{
	FINDDATA afile;
	DBFS dbf;
	BOOL bFound = FALSE;
	
	if (idType == ID_DIRECTORY)
		DBFBeginDirInfo(&afile, (WORD)drive);
	else
		DBFBeginInfo(&afile, idType);
	while(DBFNextInfo(lpOutDBF, &afile,bWrite, NO))
	{
		if (StringsEqual(lpName, lpOutDBF->info.szName))
		{
			if (idType == ID_DIRECTORY)
			{
				if (StrongEqualVolume(lpName[0], lpVolume, &lpOutDBF->info.volume))
					return(lpOutDBF); 	// This is the one
				else 
				{
					DBFClose(lpOutDBF);
					dbf = *lpOutDBF;
					bFound = TRUE;
					continue;
				}
			}
			else if (EqualVolume(lpVolume, &lpOutDBF->info.volume))
				return(lpOutDBF); 	// This is the one
		}
		DBFClose(lpOutDBF);
	}
	if (bFound)
	{
		*lpOutDBF = dbf;
		return(DBFReOpen(lpOutDBF, bWrite));
	}	
	return(NULL);
}

//**************************************************************************
//	Creates a new (open) DBF in Browser.DBFInfoDir.
//	Sets DBF entry pointer to the begining (GetNext will get first entry).
// 	DOES NOT CHECK FOR DUPLICATE DBFs.
// 	Returns lpOutDBF if successful, or NULL for failure.
//	volume should be -1 (ignored) for fixed drives.
//**************************************************************************
LOCAL LPDBF DBF_new_dbf(LPTSTR lpName, LPVOLUMEID lpVolume, LPDBF lpOutDBF, WORD idType)
//**************************************************************************
{
	FNAME	szFile;	
	
	// setup info
	lpOutDBF->info.fDBFType = DBFTYPE;
	lstrcpy(lpOutDBF->info.szName, lpName);
	lpOutDBF->info.volume = *lpVolume;
	lpOutDBF->info.moveType = DBFM_LOCAL;
	lpOutDBF->info.idType = idType;
	lpOutDBF->info.numEntry = 0;
	lpOutDBF->info.lData = DBFINFOSIZE;
	lstrcpy(lpOutDBF->info.szDescription, _T("\0"));
	// setup some of dbf data structure
	lpOutDBF->szFile[0] =  _T('\0');
	lpOutDBF->bOpen = FALSE;
	lpOutDBF->file = 0;
	lpOutDBF->pos = -1;
	lpOutDBF->EntrySize = DBFENTRYSIZE;
	lpOutDBF->PathSize = sizeof(FNAME);
	//	get new file name
	if (!GetNewDBFName(szFile,Browser.DBFInfoDir, lpName, idType))
		return(NULL); 	
	// create, open and write file
	lpOutDBF->file = lCreat(szFile, FO_READ| FO_WRITE);
	if (lpOutDBF->file == FILE_HANDLE_INVALID)
		return(NULL);
	if (!DBF_write_info(&(lpOutDBF->info), lpOutDBF->file, NO/*need to seek*/))
	{	// failure: cleanup
		FileDelete(szFile);
		return(NULL);
	}
	// setup the rest of dbf structure
	lstrcpy(lpOutDBF->szFile, szFile);
	lpOutDBF->bOpen = TRUE;
	lpOutDBF->bWrite = TRUE;
	return(lpOutDBF);
}

//**************************************************************************
//	Positions the file pointer for the DBF to the begining of first entry
//	Resets entry pointer to 0.
// 	Returns TRUE if successful
//**************************************************************************
LOCAL BOOL	DBF_goto_first(LPDBF lpDBF)
//**************************************************************************
{
	LONG lpos;
	if (!lpDBF)
		return(FALSE);
	lpDBF->pos = -1;
	lpos = lpDBF->info.lData;
	if (FileSeek(lpDBF->file,lpos, 0/*begining*/) != lpos)
		return(FALSE);
	lpDBF->pos = 0;
	return(TRUE);
}

//**************************************************************************
//	Will ensure data in an open lpDBF has been writen from the buffer to disk.
// 	Will leave the file in whatever open state it was in (if it can).
//	Will return false ONLY if the flush fails (not if re-open fails).
//**************************************************************************
LOCAL BOOL	DBF_flush(LPDBF lpDBF)
//**************************************************************************
{
	if (!lpDBF->bOpen)
		return(TRUE);	
	if (!lClose(lpDBF->file))
	{
		lpDBF->bOpen = FALSE;
		return(FALSE);
	}
	lpDBF->file = lOpen(lpDBF->szFile,(lpDBF->bWrite ? (FO_WRITE|FO_READ):(FO_READ)));
	if (lpDBF->file == FILE_HANDLE_INVALID) 
	{
		// try opening as read only
		lpDBF->file = lOpen(lpDBF->szFile,FO_READ);
		if (lpDBF->file == FILE_HANDLE_INVALID) 
		{
			lpDBF->bOpen = FALSE;
			return(FALSE);	
		}
		lpDBF->bOpen = TRUE;
		lpDBF->bWrite = FALSE;
		return(TRUE);
	}
	lpDBF->bOpen = TRUE;
	return(TRUE);
}


//**************************************************************************
//	reads the dbf info. 
//	if bSeek==FALSE then it will assume that file points to the begining of 
//		the info data.
// 	Returns False for error.
//**************************************************************************
LOCAL BOOL DBF_read_info(LPDBFINFO lpInfo, FILE_HANDLE hfile, BOOL bSeek)
//**************************************************************************
{
	WORD Data[(DBFINFOSIZE+1)/sizeof(WORD)];
	WORD wHi, wLo;
	LPWORD shtptr = &Data[0];
	
	if (bSeek)
		if (FileSeek(hfile, 0L, 0/*begining*/) != 0L)
			return(FALSE);
	// get dbf type 
	if (FileRead(hfile, shtptr, sizeof(WORD)) != sizeof(WORD))
		return(FALSE);
	lpInfo->fDBFType = *shtptr;
	// is it pp 40?
	if (lpInfo->fDBFType & PP4DBFTYPE)
		goto PP4;
	if (!(lpInfo->fDBFType & DBFTYPE))
		return(FALSE);
	// read data
	if (FileRead(hfile, shtptr, DBFINFOSIZE-sizeof(WORD)) != DBFINFOSIZE-sizeof(WORD))
		return(FALSE);
	// fill structure
	GETBYTES(lpInfo->szName, sizeof(lpInfo->szName), shtptr);
	GETLONG(lpInfo->volume.OurMark, shtptr);
	GETLONG(lpInfo->volume.VolLabel, shtptr);
	GETWORD(lpInfo->idType, shtptr);
	GETWORD(lpInfo->moveType, shtptr);
	GETWORD(lpInfo->lData, shtptr);
	GETWORD(lpInfo->numEntry, shtptr);
	GETBYTES(lpInfo->szDescription, sizeof(lpInfo->szDescription), shtptr);
	return(TRUE);

PP4:
	// read data
	if (FileRead(hfile, shtptr, PP4DBFINFOSIZE-sizeof(WORD)) != PP4DBFINFOSIZE-sizeof(WORD))
		return(FALSE);
	// fill structure
	GETBYTES(lpInfo->szName, 80, shtptr);
	GETLONG(lpInfo->volume.OurMark, shtptr);
	GETLONG(lpInfo->volume.VolLabel, shtptr);
	GETWORD(lpInfo->idType, shtptr);
	GETWORD(lpInfo->moveType, shtptr);
	GETWORD(lpInfo->lData, shtptr);
	GETWORD(lpInfo->numEntry, shtptr);
	GETBYTES(lpInfo->szDescription, 80, shtptr);
	return(TRUE);
}

//**************************************************************************
//	writes the dbf info. 
//	if bSeek==FALSE then it will assume that file points to the begining of 
//		the info data.
// 	Returns False for error.
//**************************************************************************
LOCAL BOOL DBF_write_info(LPDBFINFO lpInfo, FILE_HANDLE hfile, BOOL bSeek)
//**************************************************************************
{
	WORD Data[(DBFINFOSIZE+1)/sizeof(WORD)];
	LPWORD shtptr = &Data[0];
	
	if (bSeek)
		if (FileSeek(hfile, 0L, 0/*begining*/) != 0L)
			return(FALSE);
	// is it pp 40?
	if (lpInfo->fDBFType & PP4DBFTYPE)
		goto PP4;
	if (!(lpInfo->fDBFType & DBFTYPE))
		return(FALSE);
	// write in data
	PUTWORD (lpInfo->fDBFType, shtptr);
	PUTBYTES(lpInfo->szName, sizeof(lpInfo->szName), shtptr);
	PUTLONG (lpInfo->volume.OurMark, shtptr);
	PUTLONG (lpInfo->volume.VolLabel, shtptr);
	PUTWORD (lpInfo->idType, shtptr);
	PUTWORD (lpInfo->moveType, shtptr);
	PUTWORD (lpInfo->lData, shtptr);
	PUTWORD (lpInfo->numEntry, shtptr);
	PUTBYTES(lpInfo->szDescription, sizeof(lpInfo->szDescription), shtptr);
	// write data
	if (FileWrite(hfile, &Data[0], DBFINFOSIZE) != DBFINFOSIZE)
		return(FALSE);
	return(TRUE);

PP4:
	// write in data
	PUTWORD (lpInfo->fDBFType, shtptr);
	PUTBYTES(lpInfo->szName, 80, shtptr);
	PUTLONG (lpInfo->volume.OurMark, shtptr);
	PUTLONG (lpInfo->volume.VolLabel, shtptr);
	PUTWORD (lpInfo->idType, shtptr);
	PUTWORD (lpInfo->moveType, shtptr);
	PUTWORD (lpInfo->lData, shtptr);
	PUTWORD (lpInfo->numEntry, shtptr);
	PUTBYTES(lpInfo->szDescription, 80, shtptr);
	// write data
	if (FileWrite(hfile, &Data[0], PP4DBFINFOSIZE) != PP4DBFINFOSIZE)
		return(FALSE);
	return(TRUE);
}


//**************************************************************************
//	Reads the entry from the current file position.
//	Accounts for the different type of entries in different kinds of dbf's.
//	Returns FALSE for failure.
//**************************************************************************
LOCAL BOOL DBF_read_entry(LPDBF lpDBF, LPENTRY lpEntry)
//**************************************************************************
{
	WORD Data[(DBFENTRYSIZE+1)/2];
	LPWORD shtptr = &Data[0];
	WORD wHi, wLo;
	int bytes;
	
	// get actual size of entry
	bytes = lpDBF->EntrySize;

	// read data
	if (FileRead(lpDBF->file, shtptr, bytes) != bytes)
		return(FALSE);
	
	// fill entry
	GETBYTES(lpEntry->File.szFile, lpDBF->PathSize, shtptr);
	GETLONG (lpEntry->File.volume.OurMark, shtptr);
	GETLONG (lpEntry->File.volume.VolLabel, shtptr);
	GETBYTES(lpEntry->Thumb.szFile, lpDBF->PathSize, shtptr);
	GETLONG (lpEntry->Thumb.volume.OurMark, shtptr);
	GETLONG (lpEntry->Thumb.volume.VolLabel, shtptr);
	return(TRUE);
}
 
//**************************************************************************
//	Writes the entry from the current file position.
//	Accounts for the different type of entries in different kinds of dbf's.
//	Returns FALSE for failure.
//**************************************************************************
LOCAL BOOL DBF_write_entry(LPDBF lpDBF, LPENTRY lpEntry)
//**************************************************************************
{
	WORD Data[(DBFENTRYSIZE+1)/2];
	LPWORD shtptr = &Data[0];
	int bytes;
	
	// get actual size of entry
	bytes = lpDBF->EntrySize;

	// fill entry
	PUTBYTES(lpEntry->File.szFile, lpDBF->PathSize, shtptr);
	PUTLONG (lpEntry->File.volume.OurMark, shtptr);
	PUTLONG (lpEntry->File.volume.VolLabel, shtptr);
	PUTBYTES(lpEntry->Thumb.szFile, lpDBF->PathSize, shtptr);
	PUTLONG (lpEntry->Thumb.volume.OurMark, shtptr);
	PUTLONG (lpEntry->Thumb.volume.VolLabel, shtptr);

	// write data
	if (FileWrite(lpDBF->file, &Data[0], bytes) != bytes)
		return(FALSE);
	return(TRUE);	
}
