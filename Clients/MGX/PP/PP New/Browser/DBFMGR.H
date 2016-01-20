// (c) Copyright 1992 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#ifndef __dbfmgr
#define __dbfmgr

// routines to manage dbf's	 (DATA BASE FILES) a link between files and thumbnails

#include "bsrtypes.h"

#define	DBF_NOENTRY	-1
#define DBF_FAILURE	1
#define DBF_SUCCESS 0
 	
// 			BASE Routines
void 		testdbfmgr(void);
LPDBF 		DBFOpenAlbum(LPTSTR lpAlbum, LPDBF lpOutDBF, BOOL bWrite);
LPDBF		DBFOpenByFile(LPTSTR lpDBFile, LPDBF lpOutDBF, BOOL bWrite);
LPDBF		DBFOpenDirectory(LPFILELOC lpFile, LPDBF lpOutDBF, BOOL bWrite);
LPDBF		DBFReOpen(LPDBF lpDBF, BOOL bWrite);
int			DBFFindEntry(LPDBF lpDBF, LPFILELOC lpFile);
LPENTRY		DBFGetEntry(LPDBF lpDBF, LPFILELOC lpFile, LPENTRY lpOutEntry);
LPENTRY		DBFGetEntryNum(LPDBF lpDBF, int num, LPENTRY lpOutEntry);
LPENTRY		DBFGetNextEntry(LPDBF lpDBF, LPENTRY lpOutEntry);
BOOL		DBFChangeCurrentEntry(LPDBF lpDBF, LPENTRY lpEntry);
BOOL 		DBFPutEntry(LPDBF lpDBF, LPENTRY lpEntry, LPFILELOC lpOldFile);
BOOL 		DBFAddEntry(LPDBF lpDBF, LPENTRY lpEntry);
int			DBFDeleteEntry(LPDBF lpDBF, LPFILELOC lpFile);
BOOL 		DBFPutInfo(LPDBF lpDBF);
BOOL 		DBFUpdateInfo(LPDBF lpDBF);
LPDBF		DBFNewAlbum(LPTSTR lpAlbum, LPDBF lpOutDBF, BOOL far *bDuplicate);
LPDBF		DBFNewDirectory(LPFILELOC lpFile, LPDBF lpOutDBF, BOOL bInDrive);
BOOL		DBFClose(LPDBF lpDBF);
BOOL 		GetNewDBFName(LPTSTR lpResult, LPTSTR lpDBFPath, LPTSTR lpDBFName, int idType);
BOOL	 	DBFConvert(LPDBF lpDBF, BOOL bSaveState, BOOL bMsg);

// routines to get all albumbs
void 	DBFBeginInfo(LPFINDDATA lpafile, WORD idType);
void 	DBFBeginDirInfo(LPFINDDATA lpafile, WORD drive);
LPDBF 	DBFNextInfo(LPDBF lpOutDBF, LPFINDDATA lpafile, BOOL bWrite, BOOL bClose);

#endif
