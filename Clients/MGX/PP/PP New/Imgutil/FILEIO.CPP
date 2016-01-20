/*=======================================================================*\

	FILES.C - File management routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef WIN32
#include <stdio.h>
#include <errno.h>
#include <io.h>
#endif

#include "imgutil.h"
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
static UINT Get31ExtendedDriveType (int Drive);
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
FILE_HANDLE FileOpen(LPTSTR lpFileName, DWORD dwMode)
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
    int mode = 0;

    CharUpper( lpFileName );
    AnsiToOem( lpFileName, lpFileName );

    if( dwMode & FO_CREATE )
    {
	    if( dwMode & FO_HIDDEN )
		    mode = 2;
	    handle = _lcreat( lpFileName, mode );
    }
    else
    {
	    if( dwMode & FO_READ && dwMode & FO_WRITE )
		    mode |= READ_WRITE;
	    else
	    {
		    if( dwMode & FO_WRITE )
			    mode |= WRITE;
		    if( dwMode & FO_READ )
			    mode |= READ;
	    }
 	    handle = _lopen( lpFileName, mode );
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
return(ret != HFILE_ERROR);
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
BOOL FileExists( LPTSTR lpFileName )
/************************************************************************/
{
	BOOL fExists;

#ifdef WIN32
	fExists = GetFileAttributes(lpFileName) != 0xFFFFFFFF;
#else
    int ret;
    FNAME szFile;
	
    lstrcpy( szFile, lpFileName );
    CharUpper( szFile );
    AnsiToOem( szFile, szFile );

    ret = _access( szFile, 0 );
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
BOOL FilePermission( LPTSTR lpFileName, int mode)
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

	lstrcpy( szFile, lpFileName );
	CharUpper( szFile );
	AnsiToOem( szFile, szFile );
	return( _access( szFile, mode ) == 0 );
#endif
}

/************************************************************************/
BOOL FileDelete( LPTSTR lpFileName )
/************************************************************************/
{
#ifdef WIN32
	return(DeleteFile(lpFileName));
#else
	FNAME szFile;

	lstrcpy( szFile, lpFileName );
	CharUpper( szFile );
	AnsiToOem( szFile, szFile );

	return (remove(szFile) == 0);
#endif
}

/************************************************************************/
LPTSTR FileTimeToString( LPFILETIMES lpTime, LPTSTR lpString )
/************************************************************************/
{
#ifdef WIN32
FILETIME LocalTime;
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
BOOL FileDate( LPTSTR lpFileName, LPFILETIMES lpTime )
/************************************************************************/
{
FILE_HANDLE fh;
BOOL fRet;

CharUpper( lpFileName );
if ((fh = FileOpen(lpFileName, FO_READ)) == FILE_HANDLE_INVALID)
	return(FALSE);

#ifdef WIN32
fRet = GetFileTime(fh, NULL, NULL, lpTime);
#else
unsigned date, time;

fRet = _dos_getftime(fh, &date, &time) == 0;
lpTime->date = date;
lpTime->time = time;
#endif

FileClose(fh);
return(fRet);
}

/************************************************************************/
long FileSize( LPTSTR lpFileName )
/************************************************************************/
{
FILE_HANDLE fh;
long lSize;

CharUpper( lpFileName );
if ((fh = FileOpen(lpFileName, FO_READ)) == FILE_HANDLE_INVALID)
	return(0L);

lSize = FileSeek(fh, 0L, SEEK_END);
FileClose(fh);
return(lSize);
}

#ifndef WIN32
/************************************************************************/
BOOL CopyFile( LPTSTR lpInFile, LPTSTR lpOutFile, BOOL fFailExists )
/************************************************************************/
{
	CharUpper(lpInFile);
    CharUpper(lpOutFile);
	if (fFailExists && FileExists(lpOutFile))
		return(FALSE);
	return(FileCopy(lpInFile, lpOutFile));
}
#endif

/************************************************************************/
BOOL FileCopy( LPTSTR lpInFile, LPTSTR lpOutFile )
/************************************************************************/
{
#ifdef WIN32
	return(CopyFile(lpInFile, lpOutFile, FALSE));
#else
    int i, ifp, ofp, size;
    LPTR lpBuffer;
    FNAME szInFile, szOutFile;

    CharUpper( lpInFile );
    CharUpper( lpOutFile );

    lstrcpy( szInFile, lpInFile );
    lstrcpy( szOutFile, lpOutFile );

    AnsiToOem( szInFile, szInFile );
    AnsiToOem( szOutFile, szOutFile );

    size = 16384;
    while( !( lpBuffer = Alloc( size )))
    {
	    size/=2;
	    if( size < 2048 )
		    return( FALSE );
    }

    size -= 256; // just in case... we seem to be having a problem with the size of the buffer

    if(( ifp = _lopen( szInFile, OF_READ )) < 0 )
	    return( FALSE );

    if(( ofp = _lcreat( szOutFile, 0 )) < 0 )
    {
	    _lclose( ifp );
	    FreeUp( lpBuffer );
	    return( FALSE );
    }

    while( TRUE )
    {
	    if(( i = _lread( ifp, lpBuffer, size )) <= 0 )
	    {
		    FreeUp( lpBuffer );
		    _lclose( ifp );
		    _lclose( ofp );
		    return( i == 0 );
	    }
	    if(  _lwrite( ofp, lpBuffer, i ) != i )
	    {
		    FreeUp( lpBuffer );
		    _lclose( ifp );
		    _lclose( ofp );
		    return( FALSE );
	    }
    }
#endif
}

#ifdef UNUSED
//************************************************************************/
// same as stat (fills stat struct) but does NOT give the user an
//	error if drive can not be accessed.
//************************************************************************/
int SafeStat(LPTSTR lpFile, struct _stat FAR *buffer)
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

LOCAL UINT32 STAT_TIME(FILETIME filetime)
{
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
}
#endif

//************************************************************************/
// Gets _stat kind of time & size.
// Any entry can be NULL. 
// Does NOT give the user an error if drive can not be accessed.
// Returns FALSE for Error.
//************************************************************************/
BOOL SafeFileStats(LPTSTR lpFile, LPUINT32 lpSize, LPUINT32 lpCTime, LPUINT32 lpMTime, LPUINT32 lpATime)
//************************************************************************/
{
//#ifdef WIN32
#ifdef UNUSED

//#define STAT_TIME(filetime)  
//	(filetime.dwHighDateTime*429L+filetime.dwHighDateTime>>1L+(filetime.dwLowDateTime/10000000L))
	
	FILE_HANDLE handle;
	FILETIME ftc, ftm, fta;
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
	lstrcpy( szFile, lpFile );
	CharUpper( szFile );
	AnsiToOem( szFile, szFile );
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
BOOL SafeFileExists( LPTSTR lpFileName )
/************************************************************************/
{
UINT errmode;
BOOL fExists;

CharUpper( lpFileName );
errmode = SetErrorMode(SEM_FAILCRITICALERRORS);
fExists = FileExists(lpFileName);
SetErrorMode(errmode);

return(fExists);
}

/***********************************************************************/
VOID FixFileName( LPTSTR lpPathName )
/***********************************************************************/
{
#ifndef WIN32
// makes sure we have 8.3 file name for windows 3.1, OK for WIN32
    int i;
    LPTSTR lpFileName;
    STRING szFileName, szExtension;

    CharUpper( lpPathName );
    lstrcpy( szExtension, extension( lpPathName ));
    lstrcpy( szFileName, lpPathName );

    stripext( szFileName );
    lpFileName = filename( szFileName );

    if( lstrlen( lpFileName ) > 8 )
	    *( lpFileName + 8 ) = '\0';
    if( lstrlen( szExtension ) > 4 )
    	*( szExtension + 4 ) = '\0';

    lstrcpy( lpPathName, szFileName );
    lstrcat( lpPathName, szExtension );
#endif
}

/************************************************************************/
LPTSTR stripdir( LPTSTR lpPathName )
/************************************************************************/
{
    LPTSTR lp = lpPathName;
    LPTSTR lpFileName = lp;
	WORD  wChar;

    while( *lp )
    {
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wChar = *(( LPWORD )lp );
		else
			wChar = *lp;

        if( wChar == '\\' || wChar == ':' )
            lpFileName = MyCharNext( lp );

        lp = MyCharNext( lp );
    }
    return( lpFileName );
}

/************************************************************************/
LPTSTR filename( LPTSTR lpPathName )
/************************************************************************/
{
    LPTSTR lp = lpPathName;
    LPTSTR lpFileName = lp;
	WORD  wChar;

    while( *lp )
    {
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wChar = *(( LPWORD )lp );
		else
			wChar = *lp;

        if( wChar == '\\' || wChar == ':' )
            lpFileName = MyCharNext( lp );

        lp = MyCharNext( lp );
    }
    return( lpFileName );
}

/************************************************************************/
LPTSTR stripfile( LPTSTR lpPath )
/************************************************************************/
{
	LPTSTR lp;

	if( lp = filename( lpPath ))
		*lp = '\0';
	return( lpPath );
}

/************************************************************************/
LPTSTR stripext( LPTSTR lpPath )
/************************************************************************/
{
	LPTSTR lp;

	if ( lp = extension( lpPath ) )
		*lp = '\0';
	return( lpPath );
}

/************************************************************************/
LPTSTR FixPath( LPTSTR lpPath )
/************************************************************************/
{
    LPTSTR lp = lpPath;
	WORD wChar;

    // search for end of string
    while( *lp )
    {
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wChar = *(( LPWORD )lp );
		else
			wChar = *lp;

        if( wChar == '\n' || wChar == '\r' )
        {
            *lp = '\0';     
            break;
        }
        lp = MyCharNext( lp );
    }

    lp = MyCharPrev( lpPath, lp );

	if( IsDBCSLeadByte(( BYTE )*lp ))
		wChar = *(( LPWORD )lp );
	else
		wChar = *lp;

    if( wChar != '\\' )   // if last character is not a backslash
    {
        lp = MyCharNext( lp ); *lp = '\\';
        lp = MyCharNext( lp ); *lp = '\0';
    }
    return( lpPath );
}

/************************************************************************/
LPTSTR extension( LPTSTR lpPath )
/************************************************************************/
{
    LPTSTR lp = lpPath;
	WORD wChar;

    while( *lp )
    {
		if( IsDBCSLeadByte(( BYTE )*lp ))
			wChar = *(( LPWORD )lp );
		else
			wChar = *lp;

	    if( wChar == '.' )
		    return( lp );   // pointer to extension

        lp = MyCharNext( lp );
    }
    return( lp );           // NULL string
}

/************************************************************************/
LPTSTR GetCurrentDir( LPTSTR lpBuffer, int maxlen)
/************************************************************************/
{
	UINT  uErrorMode;

	// Turn off Windows' Error Box
	uErrorMode = SetErrorMode( SEM_FAILCRITICALERRORS );

#ifdef WIN32
	if (!GetCurrentDirectory(maxlen, lpBuffer))
		lpBuffer = NULL;
#else
    FNAME szCurrentDir;

    *szCurrentDir = '\0';
    if( !_getcwd( szCurrentDir, Min( sizeof( FNAME ), maxlen )))
	    lpBuffer = NULL;
    else
    {
        lstrcpyn( lpBuffer, szCurrentDir, Min( sizeof( FNAME ), maxlen ));
	    OemToAnsi( lpBuffer, lpBuffer );
	    CharUpper( lpBuffer );
    }
#endif

// Turn Windows' Error Box back on
SetErrorMode( uErrorMode );
return(lpBuffer);
}

/************************************************************************/
BOOL SetCurrentDir(LPTSTR lpCurrentDir)
/************************************************************************/
{
#ifdef WIN32
	return( SetCurrentDirectory( lpCurrentDir ));
#else
    int status;
	WORD wChar;
    LPTSTR lp = lpCurrentDir;

	// check 2nd character to see if drive letter in string
	if( IsDBCSLeadByte(( BYTE )*MyCharNext( lp )))
		wChar = *(( LPWORD )MyCharNext( lp ));
	else
		wChar = *MyCharNext( lp );

    if( wChar == ':' ) 		// get drive designation
    {
        if( IsDBCSLeadByte(( BYTE )*lp ))
            *(( LPWORD )lp ) = wChar = MBToLowerW( lp );
        else
            *lp = wChar = MBToLower( lp );

        AnsiToOem( lp, lp );
   	    status = _chdrive( wChar - 'a' + 1 );
	    if( status != 0 )
		    return( FALSE );
        OemToAnsi( lp, lp );

	    lp = MyCharNextN( lp, 2 );    // skip over drive designation
    }

    LPTSTR lpPtr = lp;
    // look ahead to find last character in string
    while( *( MyCharNext( lpPtr )))
        lpPtr = MyCharNext( lpPtr );

    // wack trailing backslashes    
	if( IsDBCSLeadByte(( BYTE )*lpPtr ))
		wChar = *(( LPWORD )lpPtr );
	else
		wChar = *lpPtr;

    while( TRUE )
    {
		if( IsDBCSLeadByte(( BYTE )*lpPtr ))
			wChar = *(( LPWORD )lpPtr );
		else
			wChar = *lpPtr;

		// leave backslash if first character
		if(( wChar != '\\' ) || ( lpPtr == lp ))
			break;

        *lpPtr = '\0'; lpPtr = MyCharPrev( lp, lpPtr );
    }

    AnsiToOem( lp, lp );
    status = _chdir( lp );

    return( status == 0 );
#endif
}

/************************************************************************/
// lpdwTotal or lpdwFree can be NULL
/************************************************************************/
BOOL DiskSpaceEx( LPTSTR lpPath, LPDWORD lpdwTotal, LPDWORD lpdwFree )
/************************************************************************/
{
#ifdef WIN32
	DWORD dwSectorsPerCluster, dwBytesPerSector, dwFreeClusters, dwClusters; 
	FNAME szRoot;
	LPTSTR lpRoot;

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
    WORD Device, wChar;
    struct diskfree_t DiskFree;

    if( IsDBCSLeadByte(( BYTE )*lpPath ))
        Device = *(( LPWORD )lpPath );
    else
        Device = *lpPath;

    if( IsDBCSLeadByte(( BYTE )*MyCharNext( lpPath )))
        wChar = *(( LPWORD )MyCharNext( lpPath ));
    else
        wChar = *MyCharNext( lpPath );

    if( wChar != ':' )
	    Device = 0; // The current drive
    else
    if( Device >= 'a' && Device <= 'z' )
	    Device -= ( 'a' - 1 );
    else
    if( Device >= 'A' && Device <= 'Z' )
	    Device -= ( 'A' - 1 );

    if( _dos_getdiskfree( Device, &DiskFree ))
	    return( 0 );

    if( lpdwTotal )
	    *lpdwTotal = ( DWORD )DiskFree.total_clusters *
    			    		  DiskFree.sectors_per_cluster * 
				    		  DiskFree.bytes_per_sector;

    if( lpdwFree )
	    *lpdwFree = ( DWORD )DiskFree.avail_clusters *
				   	 		 DiskFree.sectors_per_cluster * 
				    		 DiskFree.bytes_per_sector;
#endif
	return( TRUE );
}

/************************************************************************/
DWORD DiskSpace( LPTSTR lpPath )
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

	*szPath = i + 'a';
	*MyCharNextN( szPath, 1 ) = ':';
	*MyCharNextN( szPath, 2 ) = '\\';
	*MyCharNextN( szPath, 3 ) = '\0';
	return( GetDriveType( szPath ));
#else
	// try using windows call first... due to Windows 3.11 & WFW
	// bug trying to get type from dismounted drives
//	if (GetDriveType(i) == DRIVE_REMOTE)
//		return(DRIVE_REMOTE);
	return(Get31ExtendedDriveType (i));
#endif
}

#ifndef WIN32

/************************* STUFF FROM MGXUTIL ******************************/

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */
extern void FAR PASCAL DOS3Call (void);
#ifdef __cplusplus
}            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

typedef struct
{
    BYTE    DriveNumber;
    BYTE    DeviceDriverUnit;
    WORD    BytesPerSector;
    BYTE    SectorsPerCluster;
    BYTE    ShiftFactor;
    WORD    ReservedBootSectors;
    BYTE    FatCopies;
    WORD    RootDirEntries;
    WORD    FirstDataSector;
    WORD    HighestCluster;
    BYTE    SectorsPerFat;
    WORD    RootDirStartingSector;
    DWORD   DeviceDriverAddress;
    BYTE    Media2and3;         // media descriptor here in DOS 2.x and 3.x
    BYTE    Media4;             // media descriptor here in DOS 4.x
    DWORD   NextDeviceParamBlock;
} PARAMBLOCK, far *LPPARAMBLOCK;

static LPPARAMBLOCK GetParamBlockPtr (int DriveNumber)
{
    LPPARAMBLOCK lpParamBlock = NULL;

    _asm    mov     ah,032h         // Get pointer to drive parameter block
    _asm    mov     dl,byte ptr DriveNumber
    _asm    inc     dl              // 0 means current disk for int 32h
    _asm    push    ds
    DOS3Call ();
    _asm    mov     dx,ds
    _asm    pop     ds
    _asm    cmp     al,0FFh
    _asm    je      dcDone
    _asm    mov     word ptr lpParamBlock[0],bx
    _asm    mov     word ptr lpParamBlock[2],dx
dcDone:
    return lpParamBlock;
}

static BOOL IsRamDisk (int DriveNumber)
{
    LPPARAMBLOCK lpParamBlock = GetParamBlockPtr (DriveNumber);

    return lpParamBlock && lpParamBlock->FatCopies==1;
}

static BOOL IsCDROM (int DriveNumber)
{
    BOOL bCDfound = FALSE;

    _asm    mov     ax,01500h
    _asm    xor     bx,bx
    _asm    int     02Fh
    _asm    or      bx,bx
    _asm    jz      Done
    _asm    mov     ax,0150bh
    _asm    mov     cx,DriveNumber
    _asm    int     02Fh
    _asm    or      ax,ax
    _asm    jz      Done
    bCDfound = TRUE;
Done:
    return bCDfound;
}

/***************************************************************************
 *
 *        Name:  Get31ExtendedDriveType
 *
 *     Purpose:  Determines the type of the specified drive.  Extends
 *               standard Windows GetDriveType by identifying RAM drives and
 *               CD ROM drives.
 *
 *  Parameters:  Drive - Drive Number to check (A=0,B=1,etc...)
 *                       If Drive is an alphabetic character,
 *                       then it will be automatically converted to
 *                       the correct drive number (e.g. 'B' is same as 1,
 *                       'c' is same as 2).
 *
 *     Returns:  0 for invalid or unknown drive, or one of the following:
 *               DRIVE_REMOVABLE        // Same as Windows
 *               DRIVE_FIXED            // Same as Windows
 *               DRIVE_REMOTE           // Same as Windows
 *               DRIVE_CDROM            // Extended Type
 *               DRIVE_RAM              // Extended Type
 *
 **************************************************************************/
static UINT Get31ExtendedDriveType (int Drive)
{
    UINT DriveType;

    if (Drive>='A' && Drive<='Z')
        Drive -= 'A';
    else 
	if (Drive>='a' && Drive<='z')
        Drive -= 'a';

    DriveType = GetDriveType (Drive);
    switch (DriveType)
    {
    case DRIVE_FIXED:
        if (IsRamDisk (Drive))  DriveType = DRIVE_RAMDISK;
        break;
    case DRIVE_REMOTE:
        if (IsCDROM (Drive))    DriveType = DRIVE_CDROM;
        break;
    }
    return DriveType;
}
/******************** END STUFF FROM MGXUTIL ****************************/

#endif

/************************************************************************/
LPTSTR GetRootDirectory(LPTSTR lpPath, LPTSTR lpRoot)
/************************************************************************/
{
	int i = 1; 	// assume drive:\ name 
	WORD wCurrent, wNext;

	lstrcpy( lpRoot, lpPath );
	lpPath = lpRoot;

	// for "Drive:\...\..\" paths, the root dir is "drive:\"
	// for "\\machinename\resourcename\...\...\" UNC names, the root dir is "\\machinename\resourcename\"

    if( IsDBCSLeadByte(( BYTE )*lpPath ))
        wCurrent = *(( LPWORD )lpPath );
    else
        wCurrent = *lpPath;

    if( IsDBCSLeadByte(( BYTE )*MyCharNext( lpPath )))
        wNext = *(( LPWORD )MyCharNext( lpPath ));
    else
        wNext = *MyCharNext( lpPath );

	if( wCurrent == '\\' && wNext == '\\')		// UNC name
		i = 4;				

	while( i-- )
	{
		while( wCurrent && ( wCurrent != '\\' ))
		{
			lpPath = MyCharNext( lpPath );
    		if( IsDBCSLeadByte(( BYTE )*lpPath ))
        		wCurrent = *(( LPWORD )lpPath );
    		else
        		wCurrent = *lpPath;
		}

		if( !wCurrent )
			return( NULL );

		if( i )
			lpPath = MyCharNext( lpPath );
	}
	*MyCharNext( lpPath ) = '\0';
	return( lpRoot );
}

/************************************************************************/
void WildcardDelete( LPTSTR lpPath, LPTSTR lpName )
/************************************************************************/
{
    BOOL fRet;
    FINDDATA afile;
    STRING szFilename;

    lstrcpy( szFilename, lpPath );
    lstrcat( szFilename, lpName );

    fRet = Dos_FindFirst( szFilename, ATTRIB_NORMAL, &afile );
    while( fRet )
    {
	    if( !( afile.dwAttrib & ATTRIB_SUBDIR ))    // If this is a not subdirectory
	    { 
		    lstrcpy( szFilename, lpPath );
		    lstrcat( szFilename, afile.szName );
		    FileDelete( szFilename );
	    }
	    fRet = Dos_FindNext( &afile );
    }
    Dos_FindClose( &afile );
}

/************************************************************************/
BOOL GetTempFilename(LPTSTR lpTempName, LPTSTR lpExtension)
/************************************************************************/
{
    long    lTicks, lNum;
    TCHAR   szNum[10];
    FNAME   szTemp;

    lTicks = GetTickCount();
    lstrcpy( szTemp, lpTempName );

    while( TRUE )
    {
	    lNum = lTicks % 999999L;
	    wsprintf( szNum, "%06ld", lNum );

	    LPTSTR lp = MyCharNextN(( LPTSTR )szNum, 6 );
        if( lp )
            *lp = '\0';

	    lstrcpy( lpTempName, szTemp );
	    lstrcat( lpTempName, szNum );
	    lstrcat( lpTempName, lpExtension );

	    if( !FileExists( lpTempName ))
		    break;
	    ++lTicks;
    }
    return( TRUE );
}

/************************************************************************/
LPTSTR GetVolumnID(LPTSTR lpPath, LPTSTR lpVolumn)
/************************************************************************/
{
#ifdef WIN32
    FNAME szRoot;
    LPTSTR lpRoot;
    DWORD dwSerialNumber, dwComponentLength, dwFlags;

    lpRoot = GetRootDirectory( lpPath, szRoot );
    if( !GetVolumeInformation( lpRoot, lpVolumn, 12, &dwSerialNumber, 
						    &dwComponentLength, &dwFlags, NULL, 0 ))
	    *lpVolumn = '\0'; 
#else
    int retc;
    struct find_t afile;
    FNAME   szWildCard;

    lstrcpy( szWildCard, lpPath );
    stripfile( szWildCard );

    LPTSTR lp = szWildCard;
    for( int i = 0; i < 3; i++ )
    {
        if( *lp )
            lp = MyCharNext( lp );
        else
            break;
    }
    *lp = '\0';

    lstrcat( szWildCard, "*.*" );

    AnsiToOem( szWildCard, szWildCard );

    // get volumn name for this disk
    retc = _dos_findfirst( szWildCard, _A_VOLID, &afile );
    if( retc )
	    *lpVolumn = '\0';
    else
    {
	    OemToAnsi( afile.name, lpVolumn );
	    *MyCharNextN( lpVolumn, 11 ) = '\0';
    }
#endif
    return( lpVolumn );
}

/************************************************************************/
BOOL GetVolumeSerialNumber(LPTSTR lpPath, LPDWORD lpSerialNumber)
/************************************************************************/
{
#ifdef WIN32
    FNAME szRoot;
    LPTSTR lpRoot;
    DWORD dwComponentLength, dwFlags;

    lpRoot = GetRootDirectory( lpPath, szRoot );
    if( !GetVolumeInformation( lpRoot, NULL, 0, lpSerialNumber, 
						    &dwComponentLength, &dwFlags, NULL, 0 ))
    {
	    *lpSerialNumber = 0;
	    return( FALSE );
    }
    return( TRUE );
#else
    int retc;
    struct find_t afile;
    FNAME   szWildCard;

    lstrcpy( szWildCard, lpPath );
    stripfile( szWildCard );

    LPTSTR lp = szWildCard;  
    for( int i = 0; i < 3; i++ )
    {
        if( *lp )
            lp = MyCharNext( lp );
        else
            break;
    }
    *lp = '\0';

    lstrcat( szWildCard, "*.*" );

    AnsiToOem( szWildCard, szWildCard );

    // get volumn name for this disk
    retc = _dos_findfirst( szWildCard, _A_VOLID, &afile );
    if( retc )
	{
	    *lpSerialNumber = 0;
	    return( FALSE );
	}
    else
	{
	    *lpSerialNumber = MAKELONG( afile.wr_date, afile.wr_time );
	    return( TRUE );
	}
#endif
}

/************************************************************************/
BOOL EnumFiles(LPTSTR lpSrcName, LPTSTR lpDstName, L4PROC lpEnumFunc, long lData)
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
		if ( !(afile.dwAttrib & ATTRIB_SUBDIR) )	// If this is a not subdirectory
		{ 
			lstrcpy( szSrcPath, szSource );
			lstrcat( szSrcPath, afile.szName );
			lstrcpy( szDstPath, szDest);
			if (bDestDir)
			{
				lstrcat(szDstPath, afile.szName);
				if (StringsEqual(szSrcPath, szDstPath))
					goto NextFile;
			}
			if (lpEnumFunc)
			{
#ifndef WIN32
				OemToAnsi(szSrcPath, szSrcPath);
				OemToAnsi(szDstPath, szDstPath);
#endif
				bContinue = (*lpEnumFunc)((LPTSTR)szSrcPath,
						(LPTSTR)szDstPath, lData, iCount);
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
LPTSTR MakeFullPath(LPTSTR lpPath)
/************************************************************************/
{
    FNAME   szPath, szTemp;
    WORD    wCur, wNext;
    LPTSTR  lpCur, lpNext;
    LPVOID  lpTemp;

    lstrcpy( szPath, lpPath );
    lpTemp = szTemp;
    lpCur  = lpNext = szPath;

    // strip out combinations of '.\' and '.' and '..'
    while( *lpCur )
    {
        if( IsDBCSLeadByte(( BYTE )*lpCur ))
    	    wCur = *(( LPWORD )lpCur );
        else
    	    wCur = *lpCur;

        if( IsDBCSLeadByte(( BYTE )*( MyCharNext( lpNext ))))
    	    wNext = *(( LPWORD )MyCharNext( lpNext ));
        else
    	    wNext = *MyCharNext( lpNext );

	    if( wCur == '.' && ( wNext == '\\' || wNext == '\0' ))
    	    lpCur = MyCharNextN( lpCur, 2 );
	    else 
        if( wCur == '.' && wNext == '.' )
	    {
	        if( IsDBCSLeadByte( LOBYTE( wCur )))
	            *(( LPWORD )lpTemp ) = wCur;
			else
	            *(( LPTSTR )lpTemp ) = LOBYTE( wCur );

            lpTemp = MyCharNext(( LPTSTR )lpTemp );

	        if( IsDBCSLeadByte( LOBYTE( wNext )))
	            *(( LPWORD )lpTemp ) = wNext;
			else
	            *(( LPTSTR )lpTemp ) = LOBYTE( wNext );

            lpTemp = MyCharNext(( LPTSTR )lpTemp );
    	    lpCur = MyCharNextN( lpCur, 2 );
	    }
	    else
	    {
            if( IsDBCSLeadByte(( BYTE )*lpCur ))
    		    *(( LPWORD )lpTemp ) = wCur;
            else
    		    *(( LPTSTR )lpTemp ) = LOBYTE( wCur );

            lpTemp = MyCharNext(( LPTSTR )lpTemp );
            lpCur = MyCharNext( lpCur );
	    }
    }
    *(( LPTSTR )lpTemp ) = '\0';
    lstrcpy( szPath, szTemp );

    // make sure we have a disk designator which is either a drive letter followed 
    // by a colon or a server name\share name which begins with double backslash
    lpTemp = szTemp;
    lpCur  = szPath;
    lpNext = MyCharNext( lpCur );

    if( IsDBCSLeadByte(( BYTE )* lpCur ))
        wCur = *(( LPWORD )lpCur );
    else
        wCur = *lpCur;

    if( IsDBCSLeadByte(( BYTE )* lpNext ))
        wNext = *(( LPWORD )lpNext );
    else
        wNext = *lpNext;

    if( wNext != ':' && !( wCur == '\\' && wNext == '\\' ))
    {
#ifdef WIN32
	    GetCurrentDirectory( sizeof( szTemp ), szTemp );

		// get szTemp[0]
		lpTemp = szTemp;							
    	if( IsDBCSLeadByte(( BYTE )*(( LPTSTR )lpTemp )))
        	wCur = *(( LPWORD )lpTemp );
    	else
        	wCur = *(( LPTSTR )lpTemp );

		// get szTemp[1]
		lpTemp = MyCharNext(( LPTSTR )lpTemp );		
    	if( IsDBCSLeadByte(( BYTE )*(( LPTSTR )lpTemp )))
        	wNext = *(( LPWORD )lpTemp );
    	else
        	wNext = *(( LPTSTR )lpTemp );

	    if( wNext == ':' ) 						// do we have a drive letter?
		    *MyCharNextN( szTemp, 2 ) = '\0';
	    else 
		if( wCur == '\\' && wNext == '\\' )
	    {
		    int i = 2; 		// skip past backslashes

		    while( TRUE )	// look for backslash or EOS
			{
				lpTemp = MyCharNextN(( LPTSTR )lpTemp, i );
    			if( IsDBCSLeadByte(( BYTE )*(( LPTSTR )lpTemp )))
        			wCur = *(( LPWORD )lpTemp );
    			else
        			wCur = *(( LPTSTR )lpTemp );

				if( !wCur || wCur == '\\' )
					break;
			    i++;
			}
		    if( wCur == '\\' )
			    *MyCharNextN( szTemp, i+1 ) = '\0';
		    else
			    *szTemp = '\0';		
	    }
	    else
		    *szTemp = '\0';
#else
	    int drive = _getdrive() - 1;
	    *(( LPTSTR )lpTemp ) = 'A' + drive;
        lpTemp  = MyCharNext(( LPTSTR )lpTemp );
	    *(( LPTSTR )lpTemp ) = ':';
        lpTemp  = MyCharNext(( LPTSTR )lpTemp );
	    *(( LPTSTR )lpTemp ) = '\0';
#endif
	    lstrcat( szTemp, szPath );
	    lstrcpy( szPath, szTemp );
    }
    // Strip of any trailing backslashes - should be a full path to a 
    // directory or a file, must be more than "drive:\"
    // If the third and last character is a "\", leave it there as it is 
    // specifying the root directory.
    if( MBStrlen( szPath ) > 3 )
    {
        lpCur = szPath;

        // peek ahead to find end of string
        while( *( MyCharNext( lpCur )))
            lpCur = MyCharNext( lpCur );

        // start wacking trailing backslashes
	    while( TRUE )
	    {
    		if( IsDBCSLeadByte(( BYTE )*lpCur ))
        		wCur = *(( LPWORD )lpCur );
    		else
        		wCur = *lpCur;

			if( wCur != '\\' )
				break;

		    *lpCur = '\0';
            lpCur = MyCharPrev( szPath, lpCur );
            if( lpCur == szPath )
                break;
	    }
    }
    // make sure we have a full path not just a relative path
#ifndef WIN32
    lpCur = MyCharNext( szTemp );             // szPath[1]
    lpNext = MyCharNext( lpCur );             // szPath[2]

    if( IsDBCSLeadByte(( BYTE )* lpCur ))
        wCur = *(( LPWORD )lpCur );
    else
        wCur = *lpCur;

    if( IsDBCSLeadByte(( BYTE )*lpNext ))
        wNext = *(( LPWORD )lpNext );
    else
        wNext = *lpNext;

    if( MBStrlen(( LPTSTR )szPath ) < 3 || ( wCur != '\\' && wNext == ':' ))
    {
        // get current working directory on drive specified
        lpCur = ( LPTSTR )szPath;

    	if( IsDBCSLeadByte(( BYTE )* lpCur ))
		{
        	wCur = *(( LPWORD )lpCur );
			wCur = MBToUpperW(( LPTSTR )&wCur );
		}
    	else
		{
        	wCur = *lpCur;
			wCur = MBToUpper(( LPTSTR )&wCur );
		}
	    int drive = ( wCur - 'A' ) + 1;

        AnsiToOem(( LPTSTR )szTemp, ( LPTSTR )szTemp );
	    if( _getdcwd( drive, ( LPTSTR )szTemp, MAX_FNAME_LEN ))
	    {
            OemToAnsi(( LPTSTR )szTemp, ( LPTSTR )szTemp );
		    if( MBStrlen(( LPTSTR )szPath ) > 2 ) // any thing worthwhile in szPath?
		    {
			    FixPath(( LPTSTR )szTemp );
        	    lpCur = MyCharNextN(( LPTSTR )szPath, 2 );
			    lstrcat(( LPTSTR )szTemp, lpCur );  
		    }
    	    lstrcpy( szPath, ( LPTSTR )szTemp );
	    }
        else
            OemToAnsi(( LPTSTR )szTemp, ( LPTSTR )szTemp );
	}
#endif
    // strip out combinations of '..'

	int Len = MBStrlen( szPath );
	int MinLen = 0;

	if( IsDBCSLeadByte(( BYTE )*szPath ))					// szPath[0]
		wCur = *(( LPWORD )szPath );
	else
		wNext = *szPath;

	if( IsDBCSLeadByte(( BYTE )*MyCharNextN( szPath, 1 )))	// szPath[1]
		wNext = *(( LPWORD )MyCharNextN( szPath, 1 ));
	else
		wNext = *MyCharNextN( szPath, 1 );

	if( wNext == ':' )
		MinLen = 3;
#ifdef WIN32
	else 
	if( wCur == '\\' && wNext == '\\')
	{
		MinLen = 2;

		while( TRUE )
		{
			if( IsDBCSLeadByte(( BYTE )*MyCharNextN( szPath, MinLen )))
				wNext = *(( LPWORD )MyCharNextN( szPath, MinLen ));
			else
				wNext = *MyCharNextN( szPath, MinLen );

			if( MinLen >= Len || wNext == '\\' )
				break;
			MinLen++;
		}
		MinLen++;
		while( TRUE )
		{
			if( IsDBCSLeadByte(( BYTE )*MyCharNextN( szPath, MinLen )))
				wNext = *(( LPWORD )MyCharNextN( szPath, MinLen ));
			else
				wNext = *MyCharNextN( szPath, MinLen );

			if( MinLen >= Len || wNext == '\\' )
				break;
			MinLen++;
		}

		if( MinLen >= Len )
			return( NULL );
		else
			MinLen++;
	}
	else
		return( NULL );
#endif
    lpTemp = ( LPTSTR )szTemp;
    lpCur = lpNext = ( LPTSTR )szPath;

    while( *lpCur )
    {
        if( IsDBCSLeadByte(( BYTE )*lpCur ))
    	    wCur = *(( LPWORD )lpCur );
        else
    	    wCur = *lpCur;

        if( IsDBCSLeadByte(( BYTE )*( MyCharNext( lpNext ))))
    	    wNext = *(( LPWORD )MyCharNext( lpNext ));
        else
    	    wNext = *MyCharNext( lpNext );

	    if( wCur == '.' && wNext == '.' )   // backup one directory level
        {
    	    lpCur = MyCharNextN( lpCur, 2 );
        	if( IsDBCSLeadByte(( BYTE )*lpCur ))
    	    	wCur = *(( LPWORD )lpCur );
        	else
    	    	wCur = *lpCur;

            if( wCur == '\\' )
                lpCur = MyCharNext( lpCur );  // Toss away next '\'

            lpTemp = MyCharPrev(( LPTSTR )szTemp, ( LPTSTR )lpTemp );

            // get to last '\'
            while((( LPTSTR )lpTemp >= ( LPTSTR )szPath ) && *(( LPTSTR )lpTemp ) != '\\' )
            {
                *(( LPTSTR )lpTemp ) = '\0';
                if(( LPTSTR )lpTemp == ( LPTSTR )szPath )
                    break;

                lpTemp = MyCharPrev(( LPTSTR )szTemp, ( LPTSTR )lpTemp );
            }

            // get rid of last '\' if it exists
            if(( LPTSTR )lpTemp >= ( LPTSTR )szPath )
                *(( LPTSTR )lpTemp ) = '\0';

            while((( LPTSTR )lpTemp >= ( LPTSTR )szPath ) && *(( LPTSTR )lpTemp ) != '\\' )
            {
                *(( LPTSTR )lpTemp ) = '\0';

                if(( LPTSTR )lpTemp == ( LPTSTR )szPath )
                    break;
                lpTemp = MyCharPrev(( LPTSTR )szTemp, ( LPTSTR )lpTemp );
            }
            if( *(( LPTSTR )lpTemp ) == '\\' )  
                lpTemp = MyCharNext(( LPTSTR )lpTemp );    

		    if( MBStrlen(( LPTSTR )lpTemp ) < MinLen ) 		// backed too far back? need 'drive:\'
			    return( NULL );
	    }
	    else
	    {
            if( IsDBCSLeadByte(( BYTE )*lpCur ))
    		    *(( LPWORD )lpTemp ) = ( WORD )wCur;
            else
    		    *(( LPBYTE )lpTemp ) = LOBYTE( wCur );

            lpCur = MyCharNext( lpCur );
            lpTemp = MyCharNext(( LPTSTR )lpTemp );
	    }
    }
    *(( LPTSTR )lpTemp ) = '\0';

    lstrcpy( szPath, szTemp );

    if( MBStrlen(( LPTSTR )szPath ) > 2 )
    {
	    lstrcpy( lpPath, szPath );
	    return( lpPath );
    }
    return( NULL );
}

/************************************************************************/
BOOL Dos_FindFirst(LPTSTR match, DWORD dwAttrib, LPFINDDATA lpFind)
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
	unsigned ret, unsigned dos_attrib;
	FNAME szMatch;

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

	AnsiToOem( match, szMatch);
	ret = _dos_findfirst( szMatch,dos_attrib, &lpFind->ft );

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
BOOL RenameFile(LPTSTR lpSrc, LPTSTR lpDst)
/************************************************************************/
{
#ifdef WIN32
	return(MoveFile(lpSrc, lpDst));
#else
	AnsiToOem( lpSrc, lpSrc );
	AnsiToOem( lpDst, lpDst );
    BOOL result = rename( lpSrc, lpDst );
	OemToAnsi( lpSrc, lpSrc );
	OemToAnsi( lpDst, lpDst );

	return( !result );
#endif
}

/************************************************************************/
LPTSTR MkTemp(LPTSTR lpTemplate)
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
	AnsiToOem( lpTemplate, lpTemplate );
	BOOL bRet = _mktemp( lpTemplate ) != NULL;
	OemToAnsi( lpTemplate, lpTemplate );	
	if( !bRet )
		return( NULL );
	return( lpTemplate );
#endif
}

/************************************************************************/
BOOL Dos_GetFileAttr(LPTSTR lpFile, LPDWORD dwAttrib)
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
BOOL MkDir(LPTSTR lpDir)
/************************************************************************/
{
#ifdef WIN32
	return(CreateDirectory(lpDir, NULL));
#else
	FNAME szDir;
	
	lstrcpy( szDir, lpDir );
	AnsiUpper( szDir );
	AnsiToOem( szDir, szDir );
	return( _mkdir( szDir ) == 0 );
#endif
}

/************************************************************************/
BOOL RmDir(LPTSTR lpDir)
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
int ChDir(LPTSTR lpDir)
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
TCHAR *GetCwd( TCHAR *pDir, int len )
/************************************************************************/
{
#ifdef WIN32
	if (!GetCurrentDirectory(len, pDir))
		return(NULL);
	return(pDir);
#else
	P_STR ptr;
	AnsiToOem( pDir, pDir );
	ptr = _getcwd( pDir, len );
	if( !ptr )
		return( NULL );
	OemToAnsi( pDir, pDir );
	return( ptr );
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
	WORD wChar;

	if( !GetCurrentDirectory( sizeof( szPath ), szPath ))
		return( 3 );
	if( IsDBCSLeadByte(( BYTE )*szPath ))
	{
		wChar = *(( LPWORD )szPath );
		wChar = MBToUpperW(( LPTSTR )&wChar );
	}
	else
	{
		wChar = *szPath;
		wChar = MBToUpper(( LPTSTR )&wChar );
	}
	return( wChar - 'A' );
#else
	return( _getdrive() - 1 );
#endif
}

/************************************************************************/
//	sets the current drive (a=0)
//	returns TRUE for success
/************************************************************************/
BOOL SetDrive( int drive )
/************************************************************************/
{
#ifdef WIN32
	FNAME szPath;
	szPath[0] = drive+'A';
	szPath[1] = ':';
	szPath[2] = '\0';	

	*szPath = drive + 'a';
	*MyCharNextN( szPath, 1 ) = ':';
	*MyCharNextN( szPath, 2 ) = '\0';

	return( SetCurrentDirectory( szPath ));
#else
	return( _chdrive( drive + 1 ) == 0 );
#endif
}

/************************************************************************/
BOOL IsAbsolutePath(LPCTSTR lpPath)
/************************************************************************/
{
	FNAME szFileName;

	lstrcpy(szFileName, lpPath);
	MakeFullPath(szFileName);
	return(StringsEqual(szFileName, (LPTSTR)lpPath));
}

