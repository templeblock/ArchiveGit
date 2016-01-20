 /*=======================================================================*\

	FRMFILES.C - File management routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>

#ifndef WIN32
#include <stdio.h>
#include <errno.h>
#include <io.h>
#endif

#ifdef _MAC
#include <stdio.h>
#endif

#include "framelib.h"
//#include <sys/types.h>
//#include <sys/stat.h>		  
#include <time.h>

#ifdef WIN32
static BOOL AttribMatchWin32(DWORD dwAttribWin32, DWORD dwAttrib);
static void SetFindDataWin32(LPFINDDATA lpFind, LPWIN32_FIND_DATA fd);
static void SetFileAttribWin32(DWORD dwAttribWin32, LPDWORD lpAttrib);
#else
#include <ctype.h>
#include <dos.h>
#include <errno.h>
#include <direct.h>
static void SetFindDataWin16(LPFINDDATA lpFind, struct find_t *ft);
static void SetFileAttribWin16(unsigned AttribWin16, LPDWORD lpAttrib);
#endif

#ifndef WIN32
/***********************************************************************/
//	return error codes from winerror.h (swiped from WIN32 sdk)
/***********************************************************************/
DWORD GetLastError(void)
/***********************************************************************/
{
int SysError;

SysError = errno;
if (SysError == ENOENT)
	return(ERROR_FILE_NOT_FOUND);
else
	return(0);
}
#endif

/***********************************************************************/
//	return FILE_HANDLE_INVALID if error
/***********************************************************************/
FILE_HANDLE FileOpen(LPSTR lpFileName, DWORD dwMode)
/***********************************************************************/
{
FILE_HANDLE handle;

#ifdef WIN32
DWORD dwAccess, dwCreate, dwAttribs;

dwAttribs = FILE_ATTRIBUTE_NORMAL;
dwAccess = 0;
if (dwMode & FO_READ)
	dwAccess |= GENERIC_READ;
if (dwMode & FO_WRITE)
	dwAccess |= GENERIC_WRITE;
dwCreate = 0;
if (dwMode & FO_CREATE)
	dwCreate |= CREATE_ALWAYS;
else
	dwCreate |= OPEN_EXISTING;
if (dwMode & FO_HIDDEN)
	dwAttribs = FILE_ATTRIBUTE_HIDDEN;
handle = CreateFile(lpFileName, dwAccess, 0, NULL, dwCreate,
					dwAttribs, NULL);
if (handle == INVALID_HANDLE_VALUE)
	dwCreate = GetLastError();
#else
int mode;

mode = 0;
if (dwMode & FO_CREATE)
{
	if (dwMode & FO_HIDDEN)
		mode = 2;
	handle = _lcreat( lpFileName, mode);
}
else
{
	if (dwMode & FO_READ && dwMode & FO_WRITE)
		mode |= READ_WRITE;
	else
		{
		if (dwMode & FO_WRITE)
			mode |= WRITE;
		if (dwMode & FO_READ)
			mode |= READ;
		}
 	handle = _lopen( lpFileName, mode);
}
#endif
return(handle);
}

/***********************************************************************/
// returns FALSE for failure
/***********************************************************************/
BOOL FileClose(FILE_HANDLE handle)
/***********************************************************************/
{
#ifdef WIN32
return(CloseHandle(handle));
#else
return(_lclose(handle) == 0);
#endif
}

/***********************************************************************/
// returns FILE_DWERR for failure
/***********************************************************************/
DWORD FileRead(FILE_HANDLE handle, LPVOID lpBuf, DWORD dwBytes)
/***********************************************************************/
{
#ifdef WIN32
DWORD dwBytesRead;
if (!ReadFile(handle, lpBuf, dwBytes, &dwBytesRead, NULL))
	return(FILE_DWERR);
return(dwBytesRead);
#else
return((DWORD)_lread(handle, lpBuf, (UINT)dwBytes));
#endif
}

/***********************************************************************/
// returns the number of bytes written
// returns FILE_DWERR for failure
/***********************************************************************/
DWORD FileWrite(FILE_HANDLE handle, LPVOID lpBuf, DWORD dwBytes)
/***********************************************************************/
{
#ifdef WIN32
DWORD dwBytesWritten;
if (!WriteFile(handle, lpBuf, dwBytes, &dwBytesWritten, NULL))
	return(FILE_DWERR);
return(dwBytesWritten);
#else
UINT ret;
ret =_lwrite(handle, lpBuf, (UINT)dwBytes);
if (ret == HFILE_ERROR)
	return(FILE_DWERR);
return(ret);
#endif
}

/***********************************************************************/
// Sets the EOF marker to the current position
// returns FALSE failure
/***********************************************************************/
BOOL FileTruncate(FILE_HANDLE handle)
/***********************************************************************/
{
#ifdef WIN32
return(SetEndOfFile(handle));
#else
UINT ret;
ret =_lwrite(handle, NULL, 0);
return(ret == HFILE_ERROR);
#endif
}


/***********************************************************************/
// returns FILE_DWERR for failure
/***********************************************************************/
DWORD FileSeek(FILE_HANDLE handle, long lAmount, int dir)
/***********************************************************************/
{
#ifdef WIN32
DWORD dwMoveMethod;

if (dir == SEEK_CUR)
	dwMoveMethod = FILE_CURRENT;
else 
if (dir == SEEK_END)
	dwMoveMethod = FILE_END;
else
	dwMoveMethod = FILE_BEGIN;
return(SetFilePointer(handle, lAmount, NULL, dwMoveMethod));
#else
return(_llseek(handle, lAmount, dir));
#endif
}

/***********************************************************************/
DWORD FileLastError()
/***********************************************************************/
{
#ifdef WIN32
return(GetLastError());
#else
return(0);
#endif
}

/************************************************************************/
BOOL FileExists( LPSTR lpFileName )
/************************************************************************/
{
BOOL fExists;

#ifdef WIN32
fExists = GetFileAttributes(lpFileName) != 0xFFFFFFFF;
#else
int ret;
FNAME szFile;
	
AnsiToOem(lpFileName, szFile);
ret = _access(szFile, 0);
fExists = ret == 0;
#endif

return(fExists);
}

/************************************************************************/
// 	00 = exists
//	02 = write
//	04 = read
//	06 = read/write
/************************************************************************/
BOOL FilePermission( LPSTR lpFileName, int mode)
/************************************************************************/
{
#ifdef WIN32
	DWORD dwAttrib;

	dwAttrib = GetFileAttributes(lpFileName);
	// see if we go an error - we assume this means file does not exist
	if (dwAttrib == 0xFFFFFFFF)
		return(FALSE);
	// see if just an existence check
	if (!mode)
		return(TRUE);
	// see if it's a normal file cause they have read/write access
	if (dwAttrib & FILE_ATTRIBUTE_NORMAL)
		return(TRUE);
	// see if we're checking for write permission and it's readonly
	if ((mode & 2) && (dwAttrib & FILE_ATTRIBUTE_READONLY))
		return(FALSE);
	return(TRUE);
#else
	FNAME szFile;

	AnsiToOem(lpFileName, szFile);
	return(_access(szFile, mode) == 0);
#endif
}

/************************************************************************/
BOOL FileDelete( LPSTR lpFileName )
/************************************************************************/
{
#ifdef WIN32
	return(DeleteFile(lpFileName));
#else
	FNAME szFile;

	AnsiToOem(lpFileName, szFile);
	return (remove(szFile) == 0);
#endif
}


/************************************************************************/
LPSTR FileTimeToString( LPFILETIMES lpTime, LPSTR lpString )
/************************************************************************/
{
#ifdef WIN32
FILETIMES LocalTime;
SYSTEMTIME SystemTime;

FileTimeToLocalFileTime(lpTime, &LocalTime);
FileTimeToSystemTime(&LocalTime, &SystemTime);
wsprintf(lpString, "%d/%d/%d",
		SystemTime.wMonth, SystemTime.wDay, SystemTime.wYear);
#else
*lpString = '\0';
#endif
return(lpString);
}

/************************************************************************/
BOOL FileDate( LPSTR lpFileName, LPFILETIMES lpTime )
/************************************************************************/
{
FILE_HANDLE fh;
BOOL fRet;

if ((fh = FileOpen(lpFileName, FO_READ)) == FILE_HANDLE_INVALID)
	return(FALSE);

#ifdef WIN32
fRet = GetFileTime(fh, NULL, NULL, lpTime);
#else
{
unsigned date, time;

fRet = _dos_getftime(fh, &date, &time) == 0;
lpTime->date = date;
lpTime->time = time;
}
#endif

FileClose(fh);
return(fRet);
}

/************************************************************************/
long FileSize( LPSTR lpFileName )
/************************************************************************/
{
FILE_HANDLE fh;
long lSize;

if ((fh = FileOpen(lpFileName, FO_READ)) == FILE_HANDLE_INVALID)
	return(0L);
lSize = FileSeek(fh, 0L, SEEK_END);
FileClose(fh);
return(lSize);
}

#ifndef WIN32
/************************************************************************/
BOOL CopyFile( LPSTR lpInFile, LPSTR lpOutFile, BOOL fFailExists )
/************************************************************************/
{
if (fFailExists && FileExists(lpOutFile))
	return(FALSE);
return(FileCopy(lpInFile, lpOutFile));
}
#endif

/************************************************************************/
BOOL FileCopy( LPSTR lpInFile, LPSTR lpOutFile )
/************************************************************************/
{
#ifdef WIN32
return(CopyFile(lpInFile, lpOutFile, FALSE));
#else
int i, ifp, ofp;
BYTE Buffer[2048];

if ((ifp = _lopen( lpInFile, OF_READ )) < 0)
	return( FALSE );
if ((ofp = _lcreat( lpOutFile, 0 )) < 0)
	{
	_lclose( ifp );
	return( FALSE );
	}

i = sizeof(Buffer);
while( 1 )
	{
	if ( (i = _lread(ifp, Buffer, i)) <= 0 )
		{
		_lclose( ifp );
		_lclose( ofp );
		return( i == 0 );
		}
	if ( _lwrite(ofp, Buffer, i) != i )
		{
		_lclose( ifp );
		_lclose( ofp );
		return( FALSE );
		}
	}
#endif
}

//************************************************************************/
// same as stat (fills stat struct) but does NOT give the user an
//	error if drive can not be accessed.
//************************************************************************/
int SafeStat(LPSTR lpFile, struct _stat FAR *buffer)
//************************************************************************/
{
#ifdef WIN32
	return(0);
#else
	UINT errmode;									    
	int ret;
	struct _stat info;
	FNAME szFile;

	AnsiToOem(lpFile, szFile);
	errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
	ret = _stat(szFile, &info);
	copy((LPTR)&info, (LPTR)buffer, sizeof(info));
	SetErrorMode(errmode);
	return(ret);
#endif
}


LOCAL UINT32 STAT_TIME(FILETIMES filetime)
{
#ifdef WIN32
	SYSTEMTIME systime;
	tm tmdata;
	UINT32 time;
	
	if (!FileTimeToSystemTime(&filetime, &systime))
		return(0);
	tmdata.tm_year = systime.wYear;
	tmdata.tm_mon = systime.wMonth;
	tmdata.tm_mday = systime.wDay;
	tmdata.tm_wday = systime.wDayOfWeek;
	tmdata.tm_hour = systime.wHour;
	tmdata.tm_min = systime.wMinute;
	tmdata.tm_sec = systime.wSecond;
	tmdata.tm_isdst = NO;
	time = mktime(&tmdata);
	if (time == -1L)
		return(0);
	return(time);
#else
	return(0);
#endif
}

//************************************************************************/
// Gets _stat kind of time & size.
// Any entry can be NULL. 
// Does NOT give the user an error if drive can not be accessed.
// Returns FALSE for Error.
//************************************************************************/
BOOL SafeFileStats(LPSTR lpFile, LPUINT32 lpSize, LPUINT32 lpCTime, LPUINT32 lpMTime, LPUINT32 lpATime)
//************************************************************************/
{
//#ifdef WIN32
#ifdef UNUSED

//#define STAT_TIME(filetime)  
//	(filetime.dwHighDateTime*429L+filetime.dwHighDateTime>>1L+(filetime.dwLowDateTime/10000000L))
	
	FILE_HANDLE handle;
	FILETIMES ftc, ftm, fta;
	UINT errmode;									    

	errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
	
	handle = CreateFile(lpFile, GENERIC_READ, 0, NULL, OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
		goto ExitFalse;
	if (lpSize)
		*lpSize = GetFileSize(handle, NULL);
	if (lpCTime||lpMTime||lpATime)
		if (!GetFileTime(handle, &ftc, &ftm, &fta))
			goto ExitFalse;
	FileClose(handle);
	if (lpCTime)
	{
		*lpCTime = STAT_TIME(ftc);
		if (!*lpCTime)
			*lpCTime = STAT_TIME(ftm);
		if (!*lpCTime)
			*lpCTime = STAT_TIME(fta);
	}
	if (lpMTime)
	{
		*lpMTime = STAT_TIME(ftm);
		if (!*lpMTime)
			*lpMTime = STAT_TIME(fta);
	}
	if (lpATime)
		*lpATime = STAT_TIME(fta);

#else
	UINT errmode;									    
	int ret;
	FNAME szFile;
	struct _stat info;

	errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
	
#ifdef WIN32
	lstrcpy(szFile, lpFile);
#else
	AnsiToOem(lpFile, szFile);
#endif
	ret = _stat(szFile, &info);
	if (ret)
		goto ExitFalse;
	if (lpSize)
		*lpSize = info.st_size;
	if (lpCTime)
		*lpCTime = info.st_ctime;
	if (lpMTime)
		*lpMTime = info.st_mtime;
	if (lpATime)
		*lpATime = info.st_atime;
#endif
	SetErrorMode(errmode);
	return(TRUE);

ExitFalse:
	SetErrorMode(errmode);
	return(FALSE);
}



/************************************************************************/
BOOL SafeFileExists( LPSTR lpFileName )
/************************************************************************/
{
UINT errmode;
BOOL fExists;

errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
fExists = FileExists(lpFileName);
SetErrorMode(errmode);

return(fExists);
}


/***********************************************************************/
VOID FixFileName( LPSTR lpPathName )
/***********************************************************************/
{
// dont do any work for now (NTFS)
#ifndef WIN32
LPSTR lpFileName;
STRING szFileName, szExtension;

lstrcpy( szExtension, extension(lpPathName) );
lstrcpy( szFileName, lpPathName );
stripext( szFileName );
lpFileName = filename( szFileName );
if ( lstrlen(lpFileName) > 8 )
	lpFileName[8] = '\0';
if ( lstrlen(szExtension) > 4 )
	szExtension[4] = '\0';
lstrcpy( lpPathName, szFileName );
lstrcat( lpPathName, szExtension );
#endif
}

/************************************************************************/

LPSTR stripdir( LPSTR lpPathName )
{
LPSTR lp;

lp = lpPathName;
lp += lstrlen( lpPathName );
while( lp >= lpPathName && *lp != '\\' && *lp != ':' )
	lp--;
lp++;
return( lp );
}

/************************************************************************/

LPSTR filename( LPSTR lpPathName )
{
LPSTR lp;

lp = lpPathName;
lp += lstrlen( lpPathName );
while( lp >= lpPathName && *lp != '\\' && *lp != ':' )
	lp--;
lp++;
return( lp );
}


/************************************************************************/
LPSTR stripfile( LPSTR lpPath )
/************************************************************************/
{
LPSTR lp;

if ( lp = filename( lpPath ) )
	*lp = '\0';
return( lpPath );
}


/************************************************************************/

LPSTR stripext( LPSTR lpPath )
{
LPSTR lp;

if ( lp = extension( lpPath ) )
	*lp = '\0';
return( lpPath );
}


/************************************************************************/
LPSTR FixPath( LPSTR lpPath )
/************************************************************************/
{
int length;

if ( !(length = lstrlen(lpPath)) )
	return( lpPath );
while ( length && (lpPath[length-1] == '\n' || lpPath[length-1] == '\r') )
	lpPath[--length] = '\0';
if ( !length )
	return( lpPath );
#ifdef _MAC	
if ( lpPath[length-1] != ':' )
	lstrcat( lpPath, ":" );
#else
if ( lpPath[length-1] != '\\' )
	lstrcat( lpPath, "\\" );
#endif
return( lpPath );
}

/************************************************************************/

LPSTR extension( LPSTR lpPath )
{
LPSTR lp;
int len;

len = lstrlen(lpPath);
lp = lpPath+len;
while (len--)
{
	if (*--lp == '.')
		return(lp);
	else if (*lp == '\\')
		break;
}
return(lpPath + lstrlen(lpPath)); // null string
}


/************************************************************************/
LPSTR GetCurrentDir( LPSTR lpBuffer, int maxlen)
/************************************************************************/
{
UINT  uErrorMode;

// Turn off Windows' Error Box
uErrorMode = SetErrorMode( SEM_FAILCRITICALERRORS );

#ifdef WIN32
if (!GetCurrentDirectory(maxlen, lpBuffer))
	lpBuffer = NULL;
#else
{
FNAME szCurrentDir;

szCurrentDir[0] = '\0';
if (!_getcwd(szCurrentDir, MAX_FNAME_LEN))
	lpBuffer = NULL;
else
	OemToAnsi(szCurrentDir, lpBuffer);
}
#endif

// Turn Windows' Error Box back on
SetErrorMode( uErrorMode );

return(lpBuffer);
}


/************************************************************************/
BOOL SetCurrentDir(LPSTR lpCurrentDir)
/************************************************************************/
{
#ifdef WIN32
return(SetCurrentDirectory(lpCurrentDir));
#else
FNAME szCurrentDir;
int status, length;

AnsiToOem(lpCurrentDir, szCurrentDir);
if (szCurrentDir[1] == ':')
	{
	status = _chdrive(tolower(szCurrentDir[0]) - 'a' + 1);
	if (status != 0)
		return(FALSE);
	lstrcpy(szCurrentDir, lpCurrentDir+2);
	}
length = lstrlen(szCurrentDir);
while (length && szCurrentDir[length-1] == '\\')
	{
	szCurrentDir[length-1] = '\0';
	--length;
	}
status = _chdir(szCurrentDir);
return(status == 0);
#endif
}


/************************************************************************/
// lpdwTotal or lpdwFree can be NULL
/************************************************************************/
BOOL DiskSpaceEx( LPSTR lpPath, LPDWORD lpdwTotal, LPDWORD lpdwFree )
/************************************************************************/
{
#ifdef WIN32
DWORD dwSectorsPerCluster, dwBytesPerSector, dwFreeClusters, dwClusters; 
FNAME szRoot;
LPSTR lpRoot;

// get root directory of path
lpRoot = GetRootDirectory(lpPath, szRoot);
if (!GetDiskFreeSpace(lpRoot, &dwSectorsPerCluster, &dwBytesPerSector,
						&dwFreeClusters, &dwClusters))
	return(FALSE);
if (lpdwTotal)
	*lpdwTotal = dwClusters * dwSectorsPerCluster * dwBytesPerSector;
if (lpdwFree)
	*lpdwFree = dwFreeClusters * dwSectorsPerCluster * dwBytesPerSector;
#else
BYTE Device;
struct diskfree_t DiskFree;

Device = *lpPath++;
if ( *lpPath != ':' )
	Device = 0; // The current drive
else
if ( Device >= 'a' && Device <= 'z' )
	Device -= ( 'a' - 1 );
else
if ( Device >= 'A' && Device <= 'Z' )
	Device -= ( 'A' - 1 );

if ( _dos_getdiskfree( Device, &DiskFree ) )
	return( 0 );
if (lpdwTotal)
	*lpdwTotal = (DWORD)DiskFree.total_clusters *
				DiskFree.sectors_per_cluster * 
				DiskFree.bytes_per_sector;
if (lpdwFree)
	*lpdwFree = (DWORD)DiskFree.avail_clusters *
				DiskFree.sectors_per_cluster * 
				DiskFree.bytes_per_sector;
#endif
return(TRUE);
}


/************************************************************************/
DWORD DiskSpace( LPSTR lpPath )
/************************************************************************/
{
	DWORD dwFree;

	if (!DiskSpaceEx(lpPath,NULL,&dwFree ))
		return(0);
	return(dwFree);
}

//************************************************************************/
UINT GetExtendedDriveType(int i)
//************************************************************************/
{
#ifdef WIN32
	FNAME szPath;

	szPath[0] = i+'a';
	szPath[1] = ':';
	szPath[2] = '\\';
	szPath[3] = '\0';
	return(GetDriveType(szPath));
#else
	return(GetDriveType(i));
#endif
}


/************************************************************************/
LPSTR GetRootDirectory(LPSTR lpPath, LPSTR lpRoot)
/************************************************************************/
{
lstrcpy(lpRoot, lpPath);
lpPath = lpRoot;
while (*lpPath && (*lpPath != '\\'))
	++lpPath;
if (!(*lpPath))
	return(NULL);
*(lpPath+1) = '\0';
return(lpRoot);
}

/************************************************************************/
void WildcardDelete( LPSTR lpPath, LPSTR lpName )
/************************************************************************/
{
BOOL fRet;
FINDDATA afile;
STRING szFilename;

//chdir( lpPath );
lstrcpy( szFilename, lpPath );
lstrcat( szFilename, lpName );
fRet = Dos_FindFirst( szFilename, ATTRIB_NORMAL, &afile );
while ( fRet )
	{
	if ( !(afile.dwAttrib & ATTRIB_SUBDIR) )
		{ // If this is a not subdirectory
		lstrcpy( szFilename, lpPath );
		lstrcat( szFilename, afile.szName );
//		Print("Deleting %ls", (LPSTR)szFilename );
		FileDelete( szFilename );
		}
	fRet = Dos_FindNext( &afile );
	}
Dos_FindClose(&afile);
}


/************************************************************************/
BOOL GetTempFilename(LPSTR lpTempName, LPSTR lpExtension)
/************************************************************************/
{
long    lTicks, lNum;
char    szNum[10];
FNAME   szTemp;

lTicks = GetTickCount();
lstrcpy(szTemp, lpTempName);
while (TRUE)
	{
	lNum = lTicks % 999999L;
	wsprintf(szNum, "%06ld", lNum);
	szNum[6] = '\0';
	lstrcpy(lpTempName, szTemp);
	lstrcat(lpTempName, szNum);
	lstrcat(lpTempName, lpExtension);
	if (!FileExists(lpTempName))
		break;
	++lTicks;
	}
return(TRUE);
}


/************************************************************************/
LPSTR GetVolumnID(LPSTR lpPath, LPSTR lpVolumn)
/************************************************************************/
{
#ifdef WIN32
FNAME szRoot;
LPSTR lpRoot;
DWORD dwSerialNumber, dwComponentLength, dwFlags;

lpRoot = GetRootDirectory(lpPath, szRoot);
if (!GetVolumeInformation(lpRoot, lpVolumn, 12, &dwSerialNumber,
						&dwComponentLength, &dwFlags, NULL, 0))
	*lpVolumn = '\0'; 
#else
int retc;
struct find_t afile;
FNAME   szWildCard;

lstrcpy(szWildCard, lpPath);
stripfile(szWildCard);

// get volumn name for this disk
szWildCard[3] = '\0';
lstrcat(szWildCard, "*.*");
retc = _dos_findfirst( szWildCard, _A_VOLID, &afile );
if (retc)
	lpVolumn[0] = '\0';
else
	{
	afile.name[11] = '\0';
	OemToAnsi(afile.name, lpVolumn);
	}
#endif
return(lpVolumn);
}

/************************************************************************/
BOOL GetVolumeSerialNumber(LPSTR lpPath, LPDWORD lpSerialNumber)
/************************************************************************/
{
#ifdef WIN32
FNAME szRoot;
LPSTR lpRoot;
DWORD dwComponentLength, dwFlags;

lpRoot = GetRootDirectory(lpPath, szRoot);
if (!GetVolumeInformation(lpRoot, NULL, 0, lpSerialNumber,
						&dwComponentLength, &dwFlags, NULL, 0))
	{
	*lpSerialNumber = 0;
	return(FALSE);
	}
return(TRUE);
#else
int retc;
struct find_t afile;
FNAME   szWildCard;

lstrcpy(szWildCard, lpPath);
stripfile(szWildCard);

// get volumn name for this disk
szWildCard[3] = '\0';
lstrcat(szWildCard, "*.*");
retc = _dos_findfirst( szWildCard, _A_VOLID, &afile );
if (retc)
	{
	*lpSerialNumber = 0;
	return(FALSE);
	}
else
	{
	*lpSerialNumber = MAKELONG(afile.wr_date, afile.wr_time);
	return(TRUE);
	}
#endif
}

/************************************************************************/
BOOL EnumFiles(LPSTR lpSrcName, LPSTR lpDstName, L4PROC lpEnumFunc, long lData)
/************************************************************************/
{
FNAME   szSource, szDest, szSrcPath, szDstPath;
int     iCount;
BOOL	fRet;
DWORD	dwAttrib;
BOOL    bDestDir, bContinue;
FINDDATA afile;

AnsiToOem(lpSrcName, szSource);
if (!MakeFullPath(szSource))
	return(0);
fRet = Dos_GetFileAttr(szSource, &dwAttrib);
if (fRet && (dwAttrib & ATTRIB_SUBDIR))
	{
	FixPath(szSource);
	lstrcat(szSource, "*.*");
	}
bDestDir = NO;
szDest[0] = '\0';
if (lpDstName)
	{
	AnsiToOem(lpDstName, szDest);
	if (!MakeFullPath(szDest))
		return(0);
	fRet = Dos_GetFileAttr(szDest, &dwAttrib);
	if (fRet)
		bDestDir = dwAttrib & ATTRIB_SUBDIR;
	if (bDestDir)
		FixPath(szDest);
	}

fRet = Dos_FindFirst( szSource, ATTRIB_NORMAL, &afile );
stripfile(szSource);
iCount = 0;
while ( fRet )
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
#ifndef WIN32
			OemToAnsi(szSrcPath, szSrcPath);
			OemToAnsi(szDstPath, szDstPath);
#endif
			bContinue = (*lpEnumFunc)((LPSTR)szSrcPath,
					(LPSTR)szDstPath, lData, iCount);
			if (!bContinue)
				return(iCount+1);
			}
		++iCount;
		}
NextFile:
	fRet = Dos_FindNext( &afile );
	}
Dos_FindClose(&afile);
return(iCount);
}

/************************************************************************/
LPSTR MakeFullPath(LPSTR lpPath)
/************************************************************************/
{
FNAME   szTemp, szPath;
int     index, i, j, len;
char    n, c;

lstrcpy(szPath, lpPath);

// strip out combinations of '.\' and '.' and '..'
len = lstrlen(szPath);
for (i = 0, j = 0; i < len;)
	{
	c = szPath[i];
	n = szPath[i+1];
	if (c == '.' && (n == '\\' || n == '\0'))
		i += 2;
	else if (c == '.' && n == '.')
		{
		szTemp[j] = c;
		++i;
		++j;
		szTemp[j] = n;
		++i;
		++j;
		}
	else
		{
		szTemp[j] = c;
		++i;
		++j;
		}
	}
szTemp[j] = '\0';
lstrcpy(szPath, szTemp);

// make sure we have a disk designator which is either a
// drive letter followed by a colon or a server name\share name
// which begins with double backslash
if (szPath[1] != ':' &&
	!(szPath[0] == '\\' && szPath[1] == '\\'))
	{
#ifdef WIN32
	GetCurrentDirectory(sizeof(szTemp), szTemp);
	if (szTemp[1] == ':') // do we have a drive letter
		szTemp[2] = '\0';
	else if (szTemp[0] == '\\' && szTemp[1] == '\\')
		{
		i = 2;
		while (szTemp[i] && szTemp[i] != '\\')
			++i;
		if (szTemp[i] == '\\')
			szTemp[i+1] = '\0';
		else
			szTemp[0] = '\0';		
		}
	else
		szTemp[0] = '\0';
#else
	int drive;

	drive = _getdrive() - 1;
	szTemp[0] = 'A' + drive;
	szTemp[1] = ':';
	szTemp[2] = '\0';
#endif
	lstrcat(szTemp, szPath);
	lstrcpy(szPath, szTemp);
	}

// strip of any trailing backslashes - should be a full path
// to a directory or a file, must be more than "drive:\"
// if the third and last character is a "\", leave it there
// it is specifying the root directory
if (lstrlen(szPath) > 3)
	{
	index = lstrlen(szPath) - 1;
	while (index >= 0 && szPath[index] == '\\')
		{
		szPath[index] = '\0';
		--index;
		}
	}

// make sure we have a full path not just a relative path

// BROKEN
#ifndef WIN32
if (lstrlen(szPath) < 3 || (szPath[2] != '\\' && szPath[1] == ':'))
	{
	int drive;

	drive = toupper(szPath[0]) - 'A' + 1;
// get current working directory on drive specified
	if (_getdcwd(drive, szTemp, MAX_FNAME_LEN))
		{
		if (lstrlen(szPath) > 2) // any thing worthwhile in szPath
			{
			FixPath(szTemp);
			lstrcat(szTemp, szPath+2);
			}
		lstrcpy(szPath, szTemp);
		}
	}
#endif

// strip out combinations of '..'
len = lstrlen(szPath);
for (i = 0, j = 0; i < len;)
	{
	c = szPath[i];
	n = szPath[i+1];
	if (c == '.' && n == '.')
		{ // backup one directory level
		i += 2; // toss away ".."
		if (szPath[i] == '\\') // toss away next '\'
			++i;
		--j;
// get to last '\'
		while (j >= 0 && szTemp[j] != '\\')
			{
			szTemp[j] = '\0';
			--j;
			}
// get rid of last '\' if it exists
		if (j >= 0)
			szTemp[j] = '\0';
// remove last directory level
		while (j >= 0 && szTemp[j] != '\\')
			{
			szTemp[j] = '\0';
			--j;
			}
		if (j >= 0) // we found a '\'
			++j; // leave it there
		if (j < 3)  // backed too far back? need 'drive:\'
			return(NULL);
		}
	else
		{
		szTemp[j] = c;
		++i;
		++j;
		}
	}
szTemp[j] = '\0';
lstrcpy(szPath, szTemp);

if (lstrlen(szPath) > 2)
	{
	lstrcpy(lpPath, szPath);
	return(lpPath);
	}
return(NULL);
}


/************************************************************************/
BOOL Dos_FindFirst(LPSTR match, DWORD dwAttrib, LPFINDDATA lpFind)
/************************************************************************/
{
#ifdef WIN32
WIN32_FIND_DATA fd;

if (dwAttrib & ATTRIB_VOLID)
{
	if (!GetVolumeInformation(match, lpFind->szName, 
			sizeof(lpFind->szName), NULL, NULL, NULL, NULL, 0))
		return(FALSE);
	lpFind->dwFindAttrib = dwAttrib;
	lpFind->hFindFile = INVALID_HANDLE_VALUE;
	return(TRUE);
}

lpFind->hFindFile = FindFirstFile(match, &fd);
if (lpFind->hFindFile == INVALID_HANDLE_VALUE)
	return(FALSE);
while (!AttribMatchWin32(fd.dwFileAttributes, dwAttrib))
	{
	if (!FindNextFile(lpFind->hFindFile, &fd))
		{
		FindClose(lpFind->hFindFile);
		return(FALSE);
		}
	}
SetFindDataWin32(lpFind, &fd);
lpFind->dwFindAttrib = dwAttrib;
return(TRUE);
#else
unsigned ret;
FNAME szMatch;
//struct find_t ft;
unsigned dos_attrib;

dos_attrib = _A_NORMAL;
if (dwAttrib & ATTRIB_SUBDIR)
	dos_attrib |= _A_SUBDIR;
if (dwAttrib & ATTRIB_READONLY)
	dos_attrib |= _A_RDONLY;
if (dwAttrib & ATTRIB_SYSTEM)
	dos_attrib |= _A_SYSTEM;
if (dwAttrib & ATTRIB_HIDDEN)
	dos_attrib |= _A_HIDDEN;
if (dwAttrib & ATTRIB_ARCHIVE)
	dos_attrib |= _A_ARCH;
if (dwAttrib & ATTRIB_VOLID)
	dos_attrib |= _A_VOLID;
AnsiToOem(match, szMatch);
ret = _dos_findfirst(szMatch,dos_attrib, &lpFind->ft);
if (!ret)
	{
	SetFindDataWin16(lpFind, &lpFind->ft);
	return(TRUE);
	}
return(FALSE);
#endif
}

/************************************************************************/
BOOL Dos_FindNext(LPFINDDATA lpFind)
/************************************************************************/
{
#ifdef WIN32
WIN32_FIND_DATA fd;

if (!lpFind->hFindFile)
	return(FALSE);
if (!FindNextFile(lpFind->hFindFile, &fd))
	return(FALSE);
while (!AttribMatchWin32(fd.dwFileAttributes, lpFind->dwFindAttrib))
	{
	if (!FindNextFile(lpFind->hFindFile, &fd))
		return(FALSE);
	}
SetFindDataWin32(lpFind, &fd);
return(TRUE);
#else
unsigned ret;
//struct find_t ft;

ret = _dos_findnext(&lpFind->ft);
if (!ret)
	{
	SetFindDataWin16(lpFind, &lpFind->ft);
	return(TRUE);
	}
return(FALSE);
#endif
}

/************************************************************************/
BOOL Dos_FindClose(LPFINDDATA lpFind)
/************************************************************************/
{
#ifdef WIN32
if (lpFind->hFindFile)
	return(FindClose(lpFind->hFindFile));
else
	return(FALSE);
#else
return(TRUE);
#endif
}

#ifdef WIN32
/************************************************************************/
static void SetFindDataWin32(LPFINDDATA lpFind, LPWIN32_FIND_DATA fd)
/************************************************************************/
{
lstrcpy(lpFind->szName, fd->cFileName);
SetFileAttribWin32(fd->dwFileAttributes, &lpFind->dwAttrib);
lpFind->dwHighSize = fd->nFileSizeHigh;
lpFind->dwLowSize = fd->nFileSizeLow;
lpFind->ftLastWriteTime = fd->ftLastWriteTime;
}

/************************************************************************/
static void SetFileAttribWin32(DWORD dwAttribWin32, LPDWORD lpAttrib)
/************************************************************************/
{
*lpAttrib = 0;
if (dwAttribWin32 & FILE_ATTRIBUTE_NORMAL)
	*lpAttrib = ATTRIB_NORMAL;
if (dwAttribWin32 & FILE_ATTRIBUTE_DIRECTORY)
	*lpAttrib |= ATTRIB_SUBDIR;
if (dwAttribWin32 & FILE_ATTRIBUTE_READONLY)
	*lpAttrib |= ATTRIB_READONLY;
if (dwAttribWin32 & FILE_ATTRIBUTE_SYSTEM)
	*lpAttrib |= ATTRIB_SYSTEM;
if (dwAttribWin32 & FILE_ATTRIBUTE_HIDDEN)
	*lpAttrib |= ATTRIB_HIDDEN;
if (dwAttribWin32 & FILE_ATTRIBUTE_ARCHIVE)
	*lpAttrib |= ATTRIB_ARCHIVE;
}
#else
/************************************************************************/
static void SetFindDataWin16(LPFINDDATA lpFind, struct find_t *ft)
/************************************************************************/
{
OemToAnsi(ft->name, lpFind->szName);
SetFileAttribWin16((unsigned)ft->attrib, &lpFind->dwAttrib);
lpFind->dwHighSize = 0;
lpFind->dwLowSize = ft->size;
lpFind->ftLastWriteTime.time = ft->wr_time;
lpFind->ftLastWriteTime.date = ft->wr_date;
}

/************************************************************************/
static void SetFileAttribWin16(unsigned AttribWin16, LPDWORD lpAttrib)
/************************************************************************/
{
*lpAttrib = 0;
if (!AttribWin16)
	*lpAttrib = ATTRIB_NORMAL;
else
	{
	if (AttribWin16 & _A_SUBDIR)
		*lpAttrib |= ATTRIB_SUBDIR;
	if (AttribWin16 & _A_RDONLY)
		*lpAttrib |= ATTRIB_READONLY;
	if (AttribWin16 & _A_SYSTEM)
		*lpAttrib |= ATTRIB_SYSTEM;
	if (AttribWin16 & _A_HIDDEN)
		*lpAttrib |= ATTRIB_HIDDEN;
	if (AttribWin16 & _A_ARCH)
		*lpAttrib |= ATTRIB_ARCHIVE;
	}	
}
#endif

#ifdef WIN32
/************************************************************************/
static BOOL AttribMatchWin32(DWORD dwAttribWin32, DWORD dwAttrib)
/************************************************************************/
{
// if it's a normal file we always return it
if (dwAttribWin32 & FILE_ATTRIBUTE_NORMAL &&
	dwAttrib & ATTRIB_NORMAL)
	return(TRUE);
// archived only is normal
if (dwAttribWin32 & FILE_ATTRIBUTE_ARCHIVE &&
	!(dwAttribWin32 & (FILE_ATTRIBUTE_DIRECTORY | 
				FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | 
				FILE_ATTRIBUTE_SYSTEM)) &&
	dwAttrib & ATTRIB_NORMAL)

	return(TRUE);

// see if it's a type we are looking for
if (dwAttribWin32 & FILE_ATTRIBUTE_ARCHIVE &&
	dwAttrib & ATTRIB_ARCHIVE)
	return(TRUE);
if (dwAttribWin32 & FILE_ATTRIBUTE_DIRECTORY &&
	dwAttrib & ATTRIB_SUBDIR)
	return(TRUE);
if (dwAttribWin32 & FILE_ATTRIBUTE_HIDDEN &&
	dwAttrib & ATTRIB_HIDDEN)
	return(TRUE);
if (dwAttribWin32 & FILE_ATTRIBUTE_READONLY &&
	dwAttrib & ATTRIB_READONLY)
	return(TRUE);
if (dwAttribWin32 & FILE_ATTRIBUTE_SYSTEM &&
	dwAttrib & ATTRIB_SYSTEM)
	return(TRUE);
return(FALSE);
}
#endif

/************************************************************************/
BOOL RenameFile(LPSTR lpSrc, LPSTR lpDst)
/************************************************************************/
{
#ifdef WIN32
	return(MoveFile(lpSrc, lpDst));
#else
	FNAME szSrc, szDst;
	
	AnsiToOem(lpSrc, szSrc);
	AnsiToOem(lpDst, szDst);
	return(rename(szSrc, szDst) == 0);
#endif
}

/************************************************************************/
LPSTR MkTemp(LPSTR lpTemplate)
/************************************************************************/
{
#ifdef WIN32
	FNAME szPath, szPrefix;

	lstrcpy(szPath, lpTemplate);
	stripfile(szPath);
	lstrcpy(szPrefix, filename(lpTemplate));
	if (GetTempFileName(szPath, szPrefix, 0, lpTemplate)) 
		return(lpTemplate);
	else
		return(NULL);
#else
	FNAME szTemplate;
	BOOL bRet;
	
	AnsiToOem(lpTemplate, szTemplate);
	bRet = _mktemp(szTemplate) != NULL;
	if (!bRet)
		return(NULL);
	OemToAnsi(szTemplate, lpTemplate);	
	return(lpTemplate);
#endif
}

/************************************************************************/
BOOL Dos_GetFileAttr(LPSTR lpFile, LPDWORD dwAttrib)
/************************************************************************/
{
#ifdef WIN32
	DWORD	dwFileAttributes;

	dwFileAttributes = GetFileAttributes(lpFile);
	if (dwFileAttributes == 0xFFFFFFFF)
		return(FALSE);
	SetFileAttribWin32(dwFileAttributes, dwAttrib);
	return(TRUE);
#else
	FNAME szFile;
	unsigned attrib;
	
	AnsiToOem(lpFile, szFile);
 	if (_dos_getfileattr(szFile, &attrib))
		return(FALSE);
	SetFileAttribWin16(attrib, dwAttrib);
	return(TRUE);
#endif
}

/************************************************************************/
BOOL MkDir(LPSTR lpDir)
/************************************************************************/
{
#ifdef WIN32
	return(CreateDirectory(lpDir, NULL));
#else
	FNAME szDir;
	
	AnsiToOem(lpDir, szDir);
	return(_mkdir(szDir) == 0);
#endif
}

/************************************************************************/
BOOL RmDir(LPSTR lpDir)
/************************************************************************/
{
#ifdef WIN32
	return(RemoveDirectory(lpDir));
#else
	FNAME szDir;
	
	AnsiToOem(lpDir, szDir);
	return(_rmdir(szDir) == 0);
#endif
}

/************************************************************************/
int ChDir(LPSTR lpDir)
/************************************************************************/
{
#ifdef WIN32
	return(SetCurrentDirectory(lpDir));
#else
	FNAME szDir;
	
	AnsiToOem(lpDir, szDir);
	return(_chdir(szDir) == 0);
#endif
}

/************************************************************************/
char *GetCwd(char *pDir, int len)
/************************************************************************/
{
#ifdef WIN32
	if (!GetCurrentDirectory(len, pDir))
		return(NULL);
	return(pDir);
#else
	P_STR ptr;
	ptr = _getcwd(pDir, len);
	if (!ptr)
		return(NULL);
	OemToAnsi(pDir, pDir);
	return(ptr);
#endif
}

/************************************************************************/
//	gets the current drive (a=0)
/************************************************************************/
int GetDrive()
/************************************************************************/
{
#ifdef WIN32
	FNAME szPath;

	if (!GetCurrentDirectory(sizeof(szPath), szPath))
		return(3);
	return(toupper(szPath[0])-'A');
#else
	return(_getdrive()-1);
#endif
}

/************************************************************************/
//	sets the current drive (a=0)
//	returns TRUE for success
/************************************************************************/
BOOL SetDrive(int drive)
/************************************************************************/
{
#ifdef WIN32
	FNAME szPath;

	szPath[0] = drive+'A';
	szPath[1] = ':';
	szPath[2] = '\0';	
	return(SetCurrentDirectory(szPath));
#else
	return(_chdrive((drive)+1) == 0);
#endif
}


