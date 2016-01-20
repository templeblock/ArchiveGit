// (c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "bsrtypes.h"


#define BSR_ERROR -1
#define BSR_TRUE 1
#define BSR_FALSE 0

#define THISDISK '$'
typedef BOOL (FAR * BSRENUMPROC)(LPTSTR, LPTSTR, long, int);

//	ACCESS AND USE OF ALBUM LIST
int CreateAlbList();
void PrepareAlbList();
void FreeAlbList();
LPDBFLL FirstAlb();
LPDBFLL GetNextAlb();
LPDBFLL GetPreviousAlb();
LPDBFLL GetAlb();
LPDBFLL FindAlbum(LPTSTR lpAlbum);
void AddToAlbList(LPDBFLL lpAlb);
void RemoveAlbumFromList(void);

LPDBFLL GetSelectedAlbum(HWND hDlg);

//	ACCESS AND USE OF FILE LIST
void PrepareFileList();
void FreeFileList();
int OpenFileList(HWND hDlg);
LPFILELOC GetFirstFile(LPFILELOC lpFileLocation);
LPFILELOC GetNextFile(LPFILELOC lpFileLocation);
LPFILELOC GetCurrentFile(LPFILELOC lpFileLocation);
LPFILELOC GetNumFile(int num, LPFILELOC lpFileLocation);
LPFILELOC GetPreviousFile(LPFILELOC lpFileLocation);
LPTSTR GetFirstFileEntry(LPTSTR lpFileEntry);
LPTSTR GetNextFileEntry(LPTSTR lpFileEntry);
LPTSTR GetCurrentFileEntry(LPTSTR lpFileEntry);
LPTSTR GetPreviousFileEntry(LPTSTR lpFileEntry);
LPTSTR GetNumFileEntry(int num, LPTSTR lpFileEntry);
LPTSTR GetFirstFileName(LPTSTR lpString);
LPTSTR GetNextFileName(LPTSTR lpString);
LPTSTR GetCurrentFileName(LPTSTR lpString);
LPTSTR GetPreviousFileName(LPTSTR lpString);
BOOL IsNextFileName(void);
BOOL IsPreviousFileName(void);
BOOL MakeFinalList(HWND hDlg);

//	PACKING/UNPACKING LBENTRY STRINGS
BOOL LBEntryToFile(LPTSTR lpString);
BOOL LBEntryToFileName(LPTSTR lpString);
LPTSTR FileLocationToLBEntry(LPFILELOC lpFile, LPTSTR lpString);
BOOL LBEntryToFileLocation(LPTSTR lpString, LPFILELOC lpFile);

//	.INI FILE PATH AND EXTENSION ROUTINES
BOOL LookupExtension(ITEMID idLocalFileType, LPTSTR lpExtension );
void LoadFileLocations();
BOOL GetFileLocation(ITEMID idFileType, LPTSTR lpPath );
BOOL SetFileLocation(ITEMID idFileType, LPTSTR lpPath );

//	SEARCHING AND USING THUMBNAIL & DBF STRUCTURES
LPENTRY FindEntry(LPFILELOC lpFile, LPENTRY lpOutEntry, BOOL bMustBeValid, BOOL bSearchAlbums);
int 	FindOrCreateThumb(LPFILELOC lpFile, LPENTRY lpOutEntry, BOOL bCreateWithThumb);
int  	GuessFileFromAlbum(LPTSTR lpAlbum, LPTSTR lpFile, LPFILELOC lpFileOut);
BOOL	AddToAlbum(LPDBF lpDBF, LPFILELOC lpFile, BOOL bUpdateInfo);
BOOL 	UpdateCreateThumb(LPFILELOC lpFile, WORD level, BOOL bCreateWithThumb);
BOOL 	ChangeThumb(LPFILELOC lpThumb, LPFILELOC lpDst);
BOOL	ChangeEntry(LPFILELOC lpFile, LPENTRY lpEntry);
BOOL	InstallEntry(LPENTRY lpEntry);
BOOL 	CleanupDBF(LPDBF lpDFB, WORD level);
BOOL	CompleteDeleteEntry(LPENTRY lpEntry);

//	DRIVE TYPE AND VOLUME UTILITIES
BOOL SetupDriveInfoMem(BOOL bFill);
void FreeUpDriveInfoMem(void);
UINT BSRGetDriveType(int drive);
UINT BSRSetDriveType(int drive, UINT type);
void BSRInvalidateDriveIDs(void);
BOOL VolumeCheck(LPFILELOC lpFile, LPVOLUMEID lpVolume);
LPVOLUMEID GetDriveVolume(LPTSTR lpName, LPVOLUMEID lpVolume, BOOL bAll);
LPVOLUMEID DriveVolume(int drive, LPVOLUMEID lpVolume, BOOL bAll);
BOOL EqualVolume(LPVOLUMEID lpVol1, LPVOLUMEID lpVol2);
BOOL StrongEqualVolume(int drive, LPVOLUMEID lpVol1, LPVOLUMEID lpVol2);
LONG getOurMark(int drive);
LONG putOurMark(int drive);

//	BASIC UTILITIES
P_STR CurrentDirEx(int drive, LPTSTR lpPath, int size);
BOOL ChangeDrive(int drive);
BOOL CopyLB(HWND hLBDst, HWND hLBSrc);
LPTSTR SizeToAscii(DWORD dwSize, LPTSTR lpString );
void SetupInternational(void);
LPTSTR	FormatDate(struct tm *time, LPTSTR lpDate);
LPTSTR	FormatTime(struct tm *time, LPTSTR lpTime);
LPTSTR DriveSize( int iDrive, LPTSTR lpSize );
BOOL IsDirectory (LPTSTR lpName);
LPTSTR RemovePathBackslash(LPTSTR lpPath);
BOOL IsValidFileName(LPTSTR lpPath);
void SetExtension(LPTSTR lpFile, ITEMID idType, BOOL bReal);
void AddExtension(LPTSTR lpFile, ITEMID idType, BOOL bReal);
int GetTypeFromExt(LPTSTR lpFile, int iDefault);
long ParseListBoxMessage(BOOL bMultiSelection, HWND hListBox, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL ChangeListBoxItem(HWND hBox, int item, LPTSTR lpString);
BOOL CheckAlbumName(LPTSTR lpString);
BOOL BSREnumFiles(LPTSTR lpSrcName, LPTSTR lpDstName, BSRENUMPROC lpEnumFunc, long lData);
BOOL CopyFile1(LPTSTR lpInFile, LPTSTR lpOutFile, BOOL bClock, BOOL bEscapable );

// Browser state 
LPBSRSTATE GetBsrStateStruct();
void FreeBsrStateStruct(LPBSRSTATE lpBsrState);
long GetParentBsrState(HWND hDlg, BOOL bGetMine = FALSE);

BOOL DWL_USER_OnCreate(HWND hDlg, CREATESTRUCT FAR* lpCreateStruct);
void DWL_USER_OnDestroy(HWND hDlg);


#ifdef WIN32
#define SAFECANCEL \
	(SPACE ? TRUE:(lpBsr->bDisableCancel = FALSE))
#else
#define SAFECANCEL \
	(SPACE ? TRUE:(CANCEL ? (lpBsr->bDisableCancel = TRUE):FALSE))
#endif
	
// Our Mark has first proiory
#define CLEARVOLUME(vol) \
	(vol.OurMark = vol.VolLabel = -1)
	
// checks the entry's file and the files volume
#define EQFILELOC(File1, File2) \
	(EqualVolume(&(File1).volume,&(File2).volume) && \
	StringsEqual((File1).szFile, (File2).szFile))

// set the given FileLoc structue
#define SETFILELOC(FileLoc, szFile, volume) \
	{lstrcpy((FileLoc).szFile,szFile);(FileLoc).volume = volume;}
	
// copy the given FileLoc structues
#define COPYFILELOC(FileLocDst, FileLocSrc) \
	(FileLocDst = FileLocSrc)

//	{lstrcpy((FileLocDst).szFile,(FileLocSrc).szFile); 
//	(FileLocDst).volume = (FileLocDst).volume;}
	
// defines
#define CURRENTDIR(dir,len) GetCwd( dir,len)
#define CHDIR(dir) (ChDir((LPTSTR)(dir)))
#define CURRENTDRIVE (GetDrive())
#define CHDRIVE(drive) (SetDrive(drive))

// File IO
#define GETSHORT(result, shtptr) \
	(result=(INT16)*shtptr++)
#define GETWORD(result, shtptr) \
	(result=(WORD)*shtptr++)
#define GETLONG(result, shtptr) \
	{wLo = *shtptr++; wHi = *shtptr++;result = MAKELONG(wLo, wHi);}
// NOTE: to be safe len should be even!
#define GETBYTES(result, len, shtptr) \
	{copy((LPTR)shtptr, (LPTR)result, len); shtptr = (LPWORD)((LPTR)shtptr + len);}

#define PUTSHORT(val, shtptr) \
	(*shtptr++ = (INT16)val)
#define PUTWORD(val, shtptr) \
	(*shtptr++ = (WORD)val)
#define PUTLONG(val, shtptr) \
	{*shtptr++ = LOWORD(val);*shtptr++ = HIWORD(val);}
#define PUTBYTES(result, len, shtptr) \
	{copy((LPTR)result, (LPTR)shtptr, len); shtptr = (LPWORD)((LPTR)shtptr + len);}

//	CurrentDirEx(CURRENTDRIVE, dir, len)

#define FILELISTID (lpBsr->OpenState.ViewThumbnails ? IDC_BSR_THUMBS : IDC_BSR_FILELIST)
#define OPENMODEWINDOW(hDlg) hDlg
#define FILEWINDOW(hDlg) hDlg
#define THUMBWINDOW(hDlg) hDlg
#define TEXTFILELISTWINDOW(hDlg) hDlg
#define DIRWINDOW(hDlg) hDlg
#define ALBWINDOW(hDlg) hDlg
#define IMAGETYPEWND(hDlg) hDlg
#define DIRLIST(hDlg) \
	GetDlgItem(hDlg, IDC_BSR_DIRECTORIES)
#define ALBLIST(hDlg) \
	GetDlgItem(hDlg, IDC_BSR_ALBUMS)
#define FILELIST(hDlg) \
	GetDlgItem(hDlg, FILELISTID)
#define TEMPLIST(hDlg) \
	GetDlgItem(hDlg,IDC_BSR_TEMPLIST)
	

#define ALBMODE lpBsr->OpenState.ViewAlbums
#define DIRMODE (!lpBsr->OpenState.ViewAlbums)

#define EXTENDED (lpBsr->OpenState.bExtended)
	
#define THMMODE lpBsr->OpenState.ViewThumbnails
#define AUTOTHUMBS lpBsr->OpenState.AutoThumbnails
	
#define CLEAR_DIR_DBF_CACHE (lpBsr->szLastDirDBF[0] = '\0')

