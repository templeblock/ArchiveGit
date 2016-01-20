// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

/************************************************************************/
//	FOR OLD 3.1 THUMBNAILS
/************************************************************************/

#include "pp.h"
#include "id.h"
#include "thmmgr.h"
#include "oldthm.h"
#include "bsrutil.h"
			  
#define OLDDBFILE_EXT	_T(".dbf")

extern LPBSRSTATE lpBsr;


/************************************************************************/
/* Image Database Constants						*/
/************************************************************************/
#define THUMBNAIL_SIZE 		64
#define IMAGEDB_VERSION 	1

#define IMAGEDB_OK			0
#define IMAGEDB_BADFORMAT	1
#define IMAGEDB_DELETED		2

/************************************************************************/
/* Image Database Header definition					*/
/************************************************************************/
typedef struct _imagehdr
	{
	WORD	wVersion;		// Version number
	TCHAR	szVolumn[12];		// Volumn ID
	TCHAR	szDir[80];		// Directory name
	WORD	wRecordSize;		// Record size
	WORD	wEntries;		// Entries in DB file
} IMAGEHDR;
typedef IMAGEHDR far *LPIMAGEHDR;

/************************************************************************/
/* Image Database Record definition					*/
/************************************************************************/
typedef struct _imagedbrec
	{
	TCHAR szFileName[14];    // The image filename
	TCHAR szThumbnail[14]; // The thumbnail name
	WORD	wState;	   // The state of this entry
	WORD	wWidth;	   // The images width
	WORD	wHeight;   // The images height
	WORD	wRes;	   // Resolution of the image
	WORD	wDataType; // The images data type
	WORD	wFileType; // The images file type
} IMAGEDBREC;
typedef IMAGEDBREC far *LPIMAGEDBREC;


/************************************************************************/
/* Image Database Local Functions					*/
/************************************************************************/
LOCAL  BOOL ImageDBDirectory(LPTSTR,LPTSTR);
LOCAL  LPTSTR ImageDBCurrentDir(LPTSTR);
LOCAL  LPTSTR ImageDBThumbDir(LPTSTR);
LOCAL  FILE_HANDLE ImageDBGetFile(LPTSTR, LPIMAGEHDR, LPTSTR);
LOCAL void	ImageDBDeleteAllThumbnails(FILE_HANDLE, LPIMAGEHDR);


/************************************************************************/
BOOL OldImageDBGetThumb(LPTSTR lpFileName, LPTSTR lpDBFDir, LPTHMNAIL lpThm, BOOL bGetThumbnail)
/************************************************************************/
{
	FILE_HANDLE	dbh;
	int	i;
	IMAGEHDR hdr;
	IMAGEDBREC rec;
	FNAME	szName, szThumbnail, szDBFile;
	BOOL    bMessageEnable, bCursorEnable;
	ULONG size, time;
	
	// these elements are NOT filled
	lpThm->bDescr = NO;
	lpThm->szDescr[0] = _T('\0');
	lpThm->bSpecial = TRUE;
	lpThm->info.fileDepth = 1;
	lpThm->info.ctime = 0L;
	lpThm->info.time = 0L;
	lpThm->info.fileSize = 0L;
	lpThm->info.thmtime = 0L;
	lpThm->info.lThumb = 0L;
	lpThm->info.lDescr = 0L;
	lpThm->info.lFree = 0L;	
	
	lstrcpy(lpBsr->OldThumbnailDir, lpDBFDir);
	if ((dbh = ImageDBGetFile(lpFileName, &hdr, szDBFile)) == FILE_HANDLE_INVALID)
		return(FALSE);
	lstrcpy(szName, stripdir(lpFileName));
	for (i = 0; i < hdr.wEntries; ++i)
	{
		if (FileRead(dbh, (LPTSTR)&rec, hdr.wRecordSize) != hdr.wRecordSize)
	 	{
			FileClose(dbh);
			return(FALSE);
	 	}
		if (rec.wState == IMAGEDB_DELETED)
			continue;
		if (StringsEqual(rec.szFileName, szName))
		{
			if (lstrlen(rec.szThumbnail) != 0 && bGetThumbnail)
			{
				bMessageEnable = MessageEnable(NO);
				bCursorEnable = AstralCursorEnable(NO);
				ImageDBCurrentDir(szThumbnail);
				lstrcat(szThumbnail, rec.szThumbnail);
				CReadImage read(szThumbnail);
				lpThm->frame = read.ReadFrame();
				MessageEnable(bMessageEnable);
				AstralCursorEnable(bCursorEnable);
				CLEARVOLUME(lpThm->Thumb.volume);
				lstrcpy(lpThm->Thumb.szFile,szThumbnail); 
			}
			else
			{
				lpThm->frame = NULL;
				CLEARVOLUME(lpThm->Thumb.volume);
				lpThm->Thumb.szFile[0] = _T('\0');
			}
			lpThm->info.fileXSize = rec.wWidth;
			lpThm->info.fileYSize = rec.wHeight;
			lpThm->info.fileRes = rec.wRes;
			lpThm->info.dataType = rec.wDataType/*-IDC_FIRSTDATATYPE*/;
			lpThm->info.fileType = rec.wFileType;
			if (SafeFileStats(lpFileName, &size, NULL, &time, NULL))
			{
				lpThm->info.time = time;
				lpThm->info.fileSize = size;
			}
			FileClose(dbh);
			return(TRUE);
		}
	}
	FileClose(dbh);
	return(FALSE);
}

#ifdef UNUSED
/************************************************************************/
BOOL ImageDBDeleteEntry(lpFileName)
/************************************************************************/
LPTSTR	lpFileName;
{
	FILE_HANDLE	dbh;
	int	i;
	IMAGEHDR hdr;
	IMAGEDBREC rec;
	FNAME szName, szThumbnail, szDBFile;
	long lOffset;
	WORD wEntries;
	
	if ((dbh = ImageDBGetFile(lpFileName, &hdr, szDBFile, NO)) == FILE_HANDLE_INVALID)
		return(FALSE);
	if (fReadOnly)
		{
		FileClose(dbh);
		return(FALSE);
		}
	lstrcpy(szName, stripdir(lpFileName));
	for (i = 0; i < hdr.wEntries; ++i)
		{
		if (FileRead(dbh, (LPTSTR)&rec, hdr.wRecordSize) != hdr.wRecordSize)
			{
			Message(IDS_EREADTHUMBNAIL, (LPTSTR)szDBFile);
			FileClose(dbh);
			return(FALSE);
			}
		if (rec.wState == IMAGEDB_DELETED)
			continue;
		if (StringsEqual(rec.szFileName, szName))
			{
			if (lstrlen(rec.szThumbnail))
				{
				ImageDBCurrentDir(szThumbnail);
				lstrcat(szThumbnail, rec.szThumbnail);
				if (FileExists(szThumbnail))
					FileDelete(szThumbnail);
				}
			rec.wState = IMAGEDB_DELETED;
			lOffset = hdr.wRecordSize;
			FileSeek(dbh, -lOffset, 1);  // backup one record
			if (FileWrite(dbh, (LPTSTR)&rec, hdr.wRecordSize) !=
				 hdr.wRecordSize)
				{
				Message(IDS_EWRITETHUMBNAIL, (LPTSTR)szDBFile);
				FileClose(dbh);
				return(FALSE);
				}
			FileClose(dbh);
			ImageDBExist(lpFileName, &wEntries);
			if (!wEntries)
				FileDelete(szDBFile);
			return(TRUE);
			}
		}
	FileClose(dbh);
	return(FALSE);
}
#endif // UNUSED

#ifdef UNUSED
/************************************************************************/
BOOL ImageDBExist(lpPath, lpEntries)
/************************************************************************/
LPTSTR	lpPath;
LPWORD	lpEntries;
{
	FILE_HANDLE	dbh;
	int i;
	IMAGEHDR hdr;
	IMAGEDBREC rec;
	int wEntries;
	FNAME	szDBFile;
	
	*lpEntries = 0;
	if ((dbh = ImageDBGetFile(lpPath, &hdr, szDBFile, NO)) == FILE_HANDLE_INVALID)
		return(FALSE);
	wEntries = 0;
	for (i = 0; i < hdr.wEntries; ++i)
		{
		if (FileRead(dbh, (LPTSTR)&rec, hdr.wRecordSize) != hdr.wRecordSize)
			{
			Message(IDS_EREADTHUMBNAIL, (LPTSTR)szDBFile);
			FileClose(dbh);
			return(FALSE);
			}
		if (rec.wState == IMAGEDB_DELETED)
			continue;
		++wEntries;
		}
	FileClose(dbh);
	*lpEntries = wEntries;
	return(TRUE);
}

#endif // UNUSED

#ifdef UNUSED
/************************************************************************/
void ImageDBFileEnum(lpEnumProc)
/************************************************************************/
LPROC	lpEnumProc;
{
	int retc;
	FINDDATA afile;
	FNAME	szPath;
	FNAME	szWildCard;
	FNAME	szFileName;
	FILE_HANDLE	dbh;
	IMAGEHDR hdr;
	int	iAction;
	
	ImageDBThumbDir(szPath);
	lstrcpy(szWildCard, szPath);
	lstrcat(szWildCard, _T("*"));
	lstrcat(szWildCard, OLDDBFILE_EXT);
	
	retc = Dos_FindFirst( szWildCard, ATTRIB_NORMAL, &afile );
	while ( retc )
		{
		if ( !(afile.dwAttrib & ATTRIB_SUBDIR) &&
		     !(afile.dwAttrib & ATTRIB_READONLY) )
			{ // If this is a not subdirectory
			lstrcpy( szFileName, szPath );
			lstrcat( szFileName, afile.szName );
			if ((dbh = FileOpen(szFileName, FO_READ|FO_WRITE)) != FILE_HANDLE_INVALID)
				{
				if (FileRead(dbh, (LPTSTR)&hdr,
					sizeof(IMAGEHDR)) == sizeof(IMAGEHDR))
					{
					iAction = (*lpEnumProc)(
							(LPTSTR)hdr.szVolumn,
							(LPTSTR)hdr.szDir,
							hdr.wEntries);
					if (iAction < 0)
						{
						FileClose(dbh);
						return;
						}
					if (iAction) // Delete
						{
						ImageDBDeleteAllThumbnails(dbh, &hdr);
						FileClose(dbh);
						FileDelete(szFileName);
						}
					}
				else
					{
					FileClose(dbh);
					}
				}
			}
		retc = Dos_FindNext( &afile );
		}
}
#endif // UNUSED

#ifdef UNUSED
/************************************************************************/
LOCAL void ImageDBDeleteAllThumbnails(dbh, lpHdr)
/************************************************************************/
FILE_HANDLE	dbh;
LPIMAGEHDR lpHdr;
{
	int	i;
	IMAGEDBREC rec;
	FNAME	szThumbnail;
	
	for (i = 0; i < lpHdr->wEntries; ++i)
		{
		if (FileRead(dbh, (LPTSTR)&rec, lpHdr->wRecordSize) != lpHdr->wRecordSize)
			break;
		if (rec.wState == IMAGEDB_DELETED)
			continue;
		if (lstrlen(rec.szThumbnail))
			{
			ImageDBThumbDir(szThumbnail);
			lstrcat(szThumbnail, rec.szThumbnail);
			if (FileExists(szThumbnail))
				FileDelete(szThumbnail);
			}
		}
}
#endif // UNUSED

/************************************************************************/
LOCAL LPTSTR ImageDBThumbDir(LPTSTR lpDirectory)
/************************************************************************/
{
	lstrcpy(lpDirectory, lpBsr->OldThumbnailDir);
	FixPath(lpDirectory);
	return(lpDirectory);
}

/************************************************************************/
LOCAL LPTSTR ImageDBCurrentDir(LPTSTR lpDirectory)
/************************************************************************/
{
	lstrcpy(lpDirectory, lpBsr->szOldDBDir);
	return(lpDirectory);
}

/************************************************************************/
LOCAL BOOL ImageDBDirectory(LPTSTR lpPath, LPTSTR lpDirectory)
/************************************************************************/
{
	FNAME szPath;
	
	ImageDBThumbDir(szPath);
	lstrcpy(lpDirectory, szPath);
	return(TRUE);
}

/************************************************************************/
LOCAL FILE_HANDLE ImageDBGetFile(LPTSTR lpFileName, LPIMAGEHDR lpHdr, LPTSTR lpDBFile)
/************************************************************************/
{
	int retc;
	FINDDATA afile;
	FNAME	szPath;
	FNAME	szWildCard;
	FNAME	szFileName;
	FNAME	szDir;
	TCHAR	szVolumn[12];
	FILE_HANDLE	dbh;
	BOOL	fUseDriveThumbs;
	
	// get directory which contains file
	lstrcpy(szDir, lpFileName);
	stripfile(szDir);
	
	// get volumn name for this disk
	GetVolumnID(szDir, szVolumn);
	
	// see if this disk has its own thumbnail directory
	fUseDriveThumbs = ImageDBDirectory(lpFileName, szPath);
	
	lstrcpy(szWildCard, szPath);
	lstrcat(szWildCard, _T("*"));
	lstrcat(szWildCard, OLDDBFILE_EXT);
	retc = Dos_FindFirst( szWildCard, ATTRIB_NORMAL, &afile );
	
	// setup the database directory to be used with these database files
	// and eventually the opened database file
	lstrcpy(lpBsr->szOldDBDir, szPath);
	while ( retc )
	{
		if ( !(afile.dwAttrib & ATTRIB_SUBDIR) )
		{ // If this is a not subdirectory
			lstrcpy( szFileName, szPath );
			lstrcat( szFileName, afile.szName );
			dbh = FileOpen(szFileName, (afile.dwAttrib & ATTRIB_READONLY) ? FO_READ : (FO_READ|FO_WRITE));
			if (dbh != FILE_HANDLE_INVALID)
			{
				if (FileRead(dbh, (LPTSTR)lpHdr,
						sizeof(IMAGEHDR)) == sizeof(IMAGEHDR))
				{
					// if using a drives own thumbnails, set it
					// up so we ignore drive letter and volumn id
					if (fUseDriveThumbs)
					{
						lpHdr->szDir[0] = szDir[0];
						lstrcpy(lpHdr->szVolumn, szVolumn);
					}
					if (StringsEqual(szDir, lpHdr->szDir) &&
						    StringsEqual(szVolumn, lpHdr->szVolumn))
					{
						if (lpDBFile)
							lstrcpy(lpDBFile, szFileName);
						return(dbh);
					}
				}
				FileClose(dbh);
			}
		}
		retc = Dos_FindNext( &afile );
	}
	return(FILE_HANDLE_INVALID);
}
