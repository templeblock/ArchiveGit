//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "routines.h"

#ifdef _MAC

// Mac include files
#include <macname1.h>
#include <resource.h>
#include <files.h>
#undef StartSound
#include <macos\sound.h>
#include <macos\memory.h>
// End Mac include files

char szWinIniName[] = { "SystemDefs" };


BOOL PlayWaveSound(LPCSTR lpszSoundName, DWORD dwSize, UINT uFlags )
{
/*
	Here's the scoop on Macintosh sounds and this function:
	This function takes a pointer to a WAV formated sound.  It replaces
	the WAV header with a Macintosh Format 1 'snd' resource header,
	then tells the Mac to play the sound.
	
	The structures used here are loosely described in Inside Macintosh
	Volume V page 491-493.  This is using Format 1 'snd' resource format.
	Check out <macos\sound.h> for the data structures used here.
	
	The WAV file format is described in Multimedia Programmer's Reference
	pages 8-22 through 8-27.

// Structures:	
	struct SndListResource
	{
	 short format;
	 short numModifiers;
	 ModRef modifierPart[1];					// This is a variable length array
	 short numCommands;
	 SndCommand commandPart[1];					// This is a variable length array
	 char dataPart[1];							// This is a variable length array
	};

	struct ModRef
	{
	 unsigned short modNumber;
	 long modInit;
	};

	struct SndCommand
	{
	 unsigned short cmd;
	 short param1;
	 long param2;
	};

	struct SoundHeader
	{
	 Ptr samplePtr;								// if NIL then samples are in sampleArea
	 unsigned long length;						// length of sound in bytes
	 Fixed sampleRate;							// sample rate for this sound
	 unsigned long loopStart;					// start of looping portion
	 unsigned long loopEnd;						// end of looping portion
	 unsigned char encode;						// header encoding
	 unsigned char baseFrequency;				// baseFrequency value
	 char sampleArea[1];
	};
*/

	SndListPtr lpSndRes = (SndListPtr)lpszSoundName;
	Handle hSnd;
	OSErr Err;
	SoundHeader* lpSoundHeader;
	
	hSnd = (Handle)&lpSndRes;
	
	if (dwSize == 0)
	{
		MessageBeep(0);
		return FALSE;
	}

// Start the Format 1 'snd' header here.	
	lpSndRes->format = 1;

// Modifiers (1 modifier @6bytes)	
	lpSndRes->numModifiers = 1;
	lpSndRes->modifierPart[0].modNumber = 5;
	lpSndRes->modifierPart[0].modInit   = 0;

// Commands (1 command @8bytes)
	lpSndRes->numCommands = 1;
	lpSndRes->commandPart[0].cmd    = 0x8051;	// SoundCmd from page V-492
	lpSndRes->commandPart[0].param1 = 0;
	lpSndRes->commandPart[0].param2 = 20;
	// 20 = 6 bytes offset + 1 modifier @6bytes + 1 command @8bytes	 

// Sound Data Table
	lpSoundHeader = (SoundHeader*)&lpSndRes->commandPart[1];
	lpSoundHeader->samplePtr     = 0;
	lpSoundHeader->length        = dwSize - 
									(sizeof(SndListResource) + 
									 sizeof(SoundHeader));
	lpSoundHeader->sampleRate    = rate11khz;
	lpSoundHeader->loopStart     = 0;
	lpSoundHeader->loopEnd       = 0;
	lpSoundHeader->encode        = 0;
	lpSoundHeader->baseFrequency = 0x3C;	// 3C came from hacking a Mac file.
	
// Hit it Hal...
	Err = SndPlay(NULL, hSnd, FALSE);
	return Err;
}

BOOL APIENTRY sndPlaySound( LPCSTR lpszSoundName, UINT uFlags )
{
	return FALSE;
}

WINBASEAPI BOOL WINAPI WriteProfileStringA(LPCSTR lpSection, LPCSTR lpEntry, LPCSTR lpString)
{
	return WritePrivateProfileString(lpSection, lpEntry, lpString, szWinIniName);
}

WINBASEAPI DWORD WINAPI GetProfileStringA(
	LPCSTR lpSection,
    LPCSTR lpEntry,
    LPCSTR lpDefault,
    LPSTR lpReturnedString,
    DWORD nSize)
{
	return GetPrivateProfileString(
		lpSection, 
		lpEntry, 
		lpDefault, 
		lpReturnedString,
		nSize, 
		szWinIniName);
}

WINBASEAPI UINT WINAPI GetProfileIntA(
	LPCSTR lpSection, 
	LPCSTR lpEntry, 
	INT nDefault)
{
	return GetPrivateProfileInt(
		lpSection, 
		lpEntry, 
		nDefault, 
		szWinIniName);
}

WINUSERAPI BOOL WINAPI SetCursorPos(int X, int Y)
{
	return FALSE;
}

WINBASEAPI BOOL WINAPI SetCurrentDirectoryA(LPCSTR lpPathName)
{
	return FALSE;
}

WINBASEAPI BOOL WINAPI RemoveDirectoryA(LPCSTR lpPathName)
{
	return FALSE;
}

WINBASEAPI HINSTANCE WINAPI LoadLibraryA(LPCSTR lpLibFileName)
{
	BOOL 		bResult;
	int			err;
	HINSTANCE 	hInst;
	FNAME		sLibName;
	
	err = 0;
	MakePascalString(sLibName, (LPSTR)lpLibFileName);	
	hInst = (HINSTANCE)OpenResFile((ConstStr255Param)sLibName);
	bResult = (hInst != (HINSTANCE)-1);
	if (!bResult)
		err = ResError();
		
	return bResult ? hInst : 0;
}

WINBASEAPI BOOL WINAPI FreeLibrary(HINSTANCE hLibModule)
{
	if (hLibModule != (HINSTANCE)-1)
		CloseResFile((short)hLibModule);
	return TRUE;
}

WINBASEAPI UINT WINAPI GetWindowsDirectoryA(LPSTR lpBuffer, UINT uSize)
{
	return 0;
}

WINBASEAPI void WINAPI InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
}

WINBASEAPI void WINAPI EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
}

WINBASEAPI void WINAPI LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
}

WINBASEAPI void WINAPI DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
}

WINBASEAPI BOOL WINAPI GetVolumeInformationA(
	LPCSTR lpRootPathName,
    LPSTR lpVolumeNameBuffer,
    DWORD nVolumeNameSize,
    LPDWORD lpVolumeSerialNumber,
    LPDWORD lpMaximumComponentLength,
    LPDWORD lpFileSystemFlags,
    LPSTR lpFileSystemNameBuffer,
    DWORD nFileSystemNameSize)
{
	return FALSE;
}

WINBASEAPI HMODULE WINAPI GetModuleHandleA(LPCSTR lpModuleName)
{
	return (HMODULE)0;
}

WINBASEAPI DWORD WINAPI GetCurrentDirectoryA(DWORD nBufferLength, LPSTR lpBuffer)
{
	return 0;
}


WINBASEAPI BOOL WINAPI GetDiskFreeSpaceA(
    LPCSTR lpRootPathName,
    LPDWORD lpSectorsPerCluster,
    LPDWORD lpBytesPerSector,
    LPDWORD lpNumberOfFreeClusters,
    LPDWORD lpTotalNumberOfClusters)
{
	return FALSE;
}

			  WINBASEAPI UINT WINAPI GetDriveTypeA(LPCSTR lpRootPathName)
{
	return 0;
}

WINBASEAPI BOOL WINAPI FindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
	return FALSE;
}

WINBASEAPI BOOL WINAPI FileTimeToLocalFileTime(CONST FILETIME *lpFileTime, LPFILETIME lpLocalFileTime)
{
	return FALSE;
}

WINUSERAPI BOOL WINAPI DlgDirSelectExA(HWND hDlg, LPSTR lpString, int nCount, int nIDListBox)
{
	return FALSE;
}

WINUSERAPI int WINAPI DlgDirListA(HWND hDlg, LPSTR lpPathSpec, int nIDListBox, int nIDStaticPath, UINT uFileType)
{
	return 0;
}

WINBASEAPI BOOL WINAPI CreateDirectoryA(LPCSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	return FALSE;
}

WINGDIAPI BOOL  WINAPI AfxAnimatePalette(HPALETTE hPalette, UINT u1, UINT u2, CONST PALETTEENTRY *pPaletteEntry)
{
	return FALSE;
}


void MakePascalString(LPSTR lpDest, LPSTR lpSrc)
{
// Change the C String into a Pascal string.
// count string
	lpDest[0] = lstrlen(lpSrc);
	lstrcpy(&lpDest[1], lpSrc);
}


void MakeCStyleString(LPSTR lpDest, LPSTR lpSrc, int iMaxLen)
{
// Change the Pascal count string into a zero terminated C style string.
	BYTE i;
	BYTE iLen = (BYTE)lpSrc[0];	// count byte is first in Pascal string.
	iLen = min(iLen, iMaxLen);
	
	for (i=0; i<iLen; i++)
		lpDest[i] = lpSrc[i+1];
	
	lpDest[iLen] = 0;
}


DWORD MakeResType(LPSTR lpType)
{
// All Mac resources are 4 character packed DWORDs.
// Example: 'MENU', 'DLOG'.
// Turn this string into a DWORD

	DWORD dwType = 0;
	dwType = *((DWORD *)lpType);
	return dwType;
}


HANDLE LoadMacResource(HINSTANCE hinst, LPSTR lpszName, LPSTR lpszType)
{
// Load a Macintosh resource by name.
	HANDLE 	hRes;
	STRING	sName;
	int		err;
	ResType Type;

// Did this come here via MAKEINTRESOURCE()
// Is this name actually an integer.
	if ((DWORD)lpszName < 32768)
		return LoadMacResourceID(hinst, (UINT)(DWORD)lpszName, lpszType);

// Get the resource by name.	
	UseResFile((short)hinst);		// bring this lib to the forefront
	Type = MakeResType(lpszType);
	MakePascalString(sName, lpszName);
	hRes = GetNamedResource(Type, (ConstStr255Param)sName);
	if (!hRes)
		err = ResError();
	if (hRes)
	{
		LoadResource((Handle)hRes);
		HLock((Handle)hRes);
	}
	return hRes;
}


HANDLE LoadMacResourceID(HINSTANCE hinst, UINT ID, LPSTR lpszType)
{
// Load a Macintosh resource by ID.
	HANDLE 	hRes;
	int		err;
	ResType Type;
	
	UseResFile((short)hinst);		// bring this lib to the forefront
	Type = MakeResType(lpszType);
	hRes = GetResource(Type, ID);
	if (!hRes)
		err = ResError();
	if (hRes)
	{
		LoadResource((Handle)hRes);
		HLock((Handle)hRes);
	}
	return hRes;
}


void ReleaseMacResource(HANDLE hRes)
{
	if (hRes)
	{
		HUnlock((Handle)hRes);
		ReleaseResource((Handle)hRes);
	}
}


DWORD GetMacResourceSize(HANDLE hRes)
{
	int		err;
	DWORD dwSize = SizeResource((Handle)hRes);	// returns -1 if bad resource.
	if (dwSize == -1)
	{
		err = ResError();
		dwSize = 0;
	}
	return dwSize;
}



DWORD MacDiskSpace(LPSTR lpPath)
{
	DWORD	dwFree = 0;
	STRING	sVolName;
	short	vRefNum;
	OSErr	err;
	
	err = GetVInfo(0, (StringPtr)&sVolName, &vRefNum, (long *)&dwFree);
	if (err != noErr)
		 return 0;
	else return dwFree;
}




#endif // _MAC

//------------Everything from here down is shared Mac/PC stuff------------




int LoadOSString(		// OS independant string loader.	
HINSTANCE	hinst,
UINT 		idResource,
LPSTR 		lpszBuffer,
int 		cbBuffer)
{
/*
	Macintosh String tables explained - by Ray White.
	
	Mac string tables are stored in 'STR#' resources.
	The ID of the resource is obtained by this formula:
		----- STR# ID = WindowsID / 16strings + 1 ----
	The STR# resource holds up to 16 pascal strings in the following format:
		0x0010 count string, count string, count string, etc.
	The string offset within the STR# resource is this formula:
		----- WindowsID - ((STR# ID - 1) * 16) ----
	Having the string offset you can walk through the pascal strings
	of the buffer returned by the GetResource() Handle.
	
	Example: Given: WindowsID = 1000 --> STR# = 63, String offset = 9
*/

#ifdef _MAC
	Handle 	hStr;
	LPSTR	lpStr;
	int		iLen;
	UINT	uStrID;
	int		iOffset;
	BYTE 	iCount;
	
	// STR# ID = WindowsID / 16strings + 1
	uStrID = idResource / 16 + 1;
	hStr = (Handle)LoadMacResourceID(hinst, uStrID, "STR#");
	if (hStr)
	{
		lpStr = *hStr;
		if (lpStr)
		{
			// Skip over 0x0010 - The 16 string indicator
			lpStr += 2;
			
			// WindowsID - ((STR# ID - 1) * 16)
			iOffset = idResource - ((uStrID - 1) * 16);
			
			// Skip over each Pascal count string.
			while (iOffset > 0)
			{
				iCount = *(BYTE far *)lpStr;
				lpStr += iCount;	// the number to skip
				lpStr += 1;			// the count byte is also skipped.
				iOffset--;
			} 
		
			MakeCStyleString(lpszBuffer, lpStr, cbBuffer);
			iLen = lstrlen(lpszBuffer);
			
			ReleaseMacResource(hStr);
			return iLen;
		}
	}
	return 0;
#else
	return LoadString(hinst, idResource, lpszBuffer, cbBuffer);
#endif 
}


DWORD OSDiskSpace(LPSTR lpPath)
{
#ifdef _MAC
	return MacDiskSpace(lpPath);
#else
	return DiskSpace(lpPath);
#endif
}



