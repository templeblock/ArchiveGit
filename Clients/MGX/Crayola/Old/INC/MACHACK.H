// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#ifndef _MACHACK_H_
#define _MACHACK_H_

#ifdef _MAC

// ----------------------------------------------------------------------
BOOL APIENTRY sndPlaySound( LPCSTR lpszSoundName, UINT uFlags );

#define SND_SYNC       0x00000000   // play synchronously (default)
#define SND_ASYNC      0x00000001   // play asynchronously

#define SND_NODEFAULT  0x00000002   // silence not default, if sound not found

#define SND_MEMORY     0x00000004   // lpszSoundName points to a memory file
#define SND_ALIAS      0x00010000   // name is a WIN.INI [sounds] entry
#define SND_FILENAME   0x00020000   // name is a file name
#define SND_RESOURCE   0x00040004   // name is a resource name or atom
#define SND_ALIAS_ID   0x00110000   // name is a WIN.INI [sounds] entry identifier

#define SND_ALIAS_ID   0x00110000   // name is a WIN.INI [sounds] entry identifier
#define SND_ALIAS_START  0      // must be > 4096 to keep strings in same section of resource file

#define SND_LOOP       0x00000008   // loop the sound until next sndPlaySound

// ----------------------------------------------------------------------

WINBASEAPI BOOL WINAPI WriteProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString);
WINBASEAPI DWORD WINAPI GetProfileStringA(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize);
WINBASEAPI UINT WINAPI GetProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault);
WINUSERAPI BOOL WINAPI SetCursorPos(int X, int Y);
WINBASEAPI BOOL WINAPI SetCurrentDirectoryA(LPCSTR lpPathName);
WINBASEAPI BOOL WINAPI RemoveDirectoryA(LPCSTR lpPathName);
WINBASEAPI HINSTANCE WINAPI LoadLibraryA(LPCSTR lpLibFileName);
WINBASEAPI BOOL WINAPI FreeLibrary(HINSTANCE hLibModule);
WINBASEAPI UINT WINAPI GetWindowsDirectoryA(LPSTR lpBuffer, UINT uSizew);
WINBASEAPI void WINAPI InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
WINBASEAPI void WINAPI EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
WINBASEAPI void WINAPI LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
WINBASEAPI void WINAPI DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
WINBASEAPI BOOL WINAPI GetVolumeInformationA(
	LPCSTR lpRootPathName,
    LPSTR lpVolumeNameBuffer,
    DWORD nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPSTR lpFileSystemNameBuffer,
    DWORD nFileSystemNameSize);
WINBASEAPI HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName);
WINBASEAPI DWORD WINAPI GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer);
WINBASEAPI BOOL WINAPI GetDiskFreeSpaceA(
    LPCSTR lpRootPathName,
    LPDWORD lpSectorsPerCluster,
    LPDWORD lpBytesPerSector,
    LPDWORD lpNumberOfFreeClusters,
    LPDWORD lpTotalNumberOfClusters);
WINBASEAPI UINT WINAPI GetDriveTypeA(LPCSTR lpRootPathName);
WINBASEAPI BOOL WINAPI FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData);
WINBASEAPI BOOL WINAPI FileTimeToLocalFileTime(CONST FILETIME *lpFileTime, LPFILETIME lpLocalFileTime);
WINUSERAPI BOOL WINAPI DlgDirSelectExA(HWND hDlg, LPSTR lpString, int nCount, int nIDListBox);
WINUSERAPI int WINAPI DlgDirListA(HWND hDlg, LPSTR lpPathSpec, int nIDListBox, int nIDStaticPath, UINT uFileType);
WINBASEAPI BOOL WINAPI CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);
WINGDIAPI BOOL  WINAPI AfxAnimatePalette(HPALETTE, UINT, UINT, CONST PALETTEENTRY *);

// ----------------------------------------------------------------------

BOOL PlayWaveSound(LPCSTR lpszSoundName, DWORD dwSize, UINT uFlags);
void MakePascalString(LPSTR lpDest, LPSTR lpSrc);
void MakeCStyleString(LPSTR lpDest, LPSTR lpSrc, int iMaxLen);
HANDLE LoadMacResource(HINSTANCE hinst, LPSTR lpszName, LPSTR lpszType);
HANDLE LoadMacResourceID(HINSTANCE hinst, UINT ID, LPSTR lpszType);
void ReleaseMacResource(HANDLE hRes);
DWORD GetMacResourceSize(HANDLE hRes);
DWORD MakeResType(LPSTR lpType);
DWORD MacDiskSpace(LPSTR lpPath);

#endif // _MAC

int LoadOSString(		// OS independant string loader.	
HINSTANCE	hinst,
UINT 		idResource,
LPSTR 		lpszBuffer,
int 		cbBuffer);
DWORD OSDiskSpace(LPSTR lpPath);

// ----------------------------------------------------------------------

#endif // _MACHACK_H_

