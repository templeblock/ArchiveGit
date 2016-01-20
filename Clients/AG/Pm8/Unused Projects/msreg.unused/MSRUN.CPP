/*
// $Workfile: MSRUN.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSRUN.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.11   10 Mar 1997 16:13:02   Jay
// Now uses "CD" variable in section.
// 
//    Rev 1.10   10 Sep 1996 12:56:58   FRED
// %W and %N use GetShortPathName() instead of quoting they will work with 16-bit apps
// 
//    Rev 1.9   22 Mar 1996 15:25:32   JAY
// Now uses window title to handle "multiple data segment" failure.
// 
//    Rev 1.8   12 Mar 1996 15:12:16   JAY
// Now allows SetWorkingDirectory under 32-bit.
// 
//    Rev 1.7   12 Mar 1996 13:37:18   JAY
// New 32-bit stuff.
// 
//    Rev 1.6   12 Mar 1996 13:11:34   JAY
// Now looks for the running instance if WinExec returns error 16
// (multiple data segs error).
// 
//    Rev 1.5   10 Feb 1996 09:10:50   FRED
// Appends extra command line parameters to exec name
// 
//    Rev 1.4   02 Feb 1996 10:03:30   JAY
// Added "UseDLLPath" to allow turning off the prepending of DLLPath.
// 
//    Rev 1.3   29 Jan 1996 14:24:08   FRED
// Created global atom
// 
//    Rev 1.2   27 Dec 1995 15:45:00   FRED
// MSRUN
*/

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <shellapi.h>
#include <dos.h>
#include "msregini.h"
#include "drivemgr.h"
#include "globatom.h"

// This makes the EXE smaller.

extern "C"
{
	void _setenvp(void)
	{
	}
	
	void _setargv(void)
	{
	}
};

// Global variables.

static HINSTANCE m_hInstance = NULL;
char m_szWorkingDirectory[_MAX_PATH];
char m_szMsregIni[_MAX_PATH];
char m_szLaunchName[_MAX_PATH];
char m_szName[_MAX_PATH];
char m_szExec[_MAX_PATH];
BOOL m_fUseAssociation;
BOOL m_fSetWorkingDirectory;
BOOL m_fTryNativeBitness;
BOOL m_fUseDLLPath;
char m_szAppName[_MAX_PATH];
char m_szCDName[_MAX_PATH];			// "CDName" variable
char m_szCD[_MAX_PATH];					// "CD" variable
char m_szDefaultPath[_MAX_PATH];
char m_szFullPath[_MAX_PATH];
char m_szFullName[_MAX_PATH];

// Global Atom used to signal that we (or someone we launched) is running.

CGlobalAtom AutoRunAtom;

// Forward references.

static BOOL SetWorkingVariables(LPCSTR pszCommandLine);
static long AccessFile(LPCSTR pName, int nMode = 0);
static BOOL FileExists(LPCSTR pszFileName);
static BOOL LocateFile(void);
static void SetFullName(LPCSTR pszPath, BOOL fAppendDefaultPath = TRUE);
static BOOL IsOurHomeCD(void);
static BOOL PromptForHomeCD(void);
static void BuildExecName(LPSTR pszExecName, LPCSTR pszTemplate);
static void AppendSlashToPath(LPSTR pszPath);
static void RemoveSlashFromPath(LPSTR pszPath);
static BOOL ActivatePreviousInstance(LPSTR pszPath);

// Main entry point.

int PASCAL far WinMain(HINSTANCE hInstance,
							  HINSTANCE hPrevInstance,
							  LPSTR pszCommandLine,
							  int nCmdShow)
{
	int nResult = 0;
	
	// Establish our global environment.

	m_hInstance = hInstance;

	// Compute our working variables.

	if (SetWorkingVariables(pszCommandLine))
	{
		// Look for the file we are suppose to run.
	
		if (LocateFile())
		{
			// We found the file, run it.
			RemoveSlashFromPath(m_szFullPath);
			
			UINT uReturn;
			char cbExecName[_MAX_PATH];
			
			if ((!m_fUseAssociation)
			 || (uReturn = (UINT)ShellExecute(
			 									NULL,
												NULL,
												m_szFullName,
												NULL,
												m_szFullPath,
												SW_SHOWNORMAL)) < 32)
			{
				// No association or action.
				BuildExecName(cbExecName, m_szExec);

#if 0
				char szMessage[256];
				wsprintf(szMessage, "Exec |%s|", cbExecName);
				MessageBox(NULL, szMessage, NULL, MB_OK);
#endif

#ifdef WIN32
				/* 32-bit launch. */
				STARTUPINFO StartupInfo;
				PROCESS_INFORMATION ProcessInformation;

				StartupInfo.cb = sizeof(StartupInfo);
				StartupInfo.lpReserved = NULL;
				StartupInfo.lpDesktop = NULL;
				StartupInfo.lpTitle = NULL;
				StartupInfo.dwX = 0;
				StartupInfo.dwY = 0;
				StartupInfo.dwXSize = 0;
				StartupInfo.dwYSize = 0;
				StartupInfo.dwXCountChars = 0;
				StartupInfo.dwYCountChars = 0;
				StartupInfo.dwFillAttribute = 0;
				StartupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK
												| STARTF_USESHOWWINDOW;
				StartupInfo.wShowWindow = SW_SHOWNORMAL;
				StartupInfo.cbReserved2 = 0;
				StartupInfo.lpReserved2 = NULL;
				StartupInfo.hStdInput = NULL;
				StartupInfo.hStdOutput = NULL;
				StartupInfo.hStdError = NULL;

				if (!CreateProcess(NULL,
										cbExecName,
										NULL,
										NULL,
										FALSE,
										0,
										NULL,
										m_fSetWorkingDirectory ? m_szFullPath : NULL,
										&StartupInfo,
										&ProcessInformation))
				{
					uReturn = GetLastError();
					if (uReturn == ERROR_SINGLE_INSTANCE_APP)
#else
				/* 16-bit launch. */
				if (m_fSetWorkingDirectory)
				{
					// Force the working directory.
					UINT uDrives;
					_dos_setdrive(toupper(m_szFullPath[0])-'A'+1, &uDrives);
					_chdir(m_szFullPath);
				}
				uReturn = WinExec(cbExecName, SW_SHOWNORMAL);
				if (uReturn < 32)
				{
					if (uReturn == 16)
#endif
					{
					/*
					// Could not launch another instance because of multiple data segments.
					// Try and find the previous instance's window.
					*/
						if (ActivatePreviousInstance(cbExecName))
						{
							nResult = 1;
						}
						else
						{
							MessageBox(NULL, "The program is already running.", NULL, MB_OK);
						}
					}
					else
					{
						char szMessage[256];
						wsprintf(szMessage, "Error %u launching %s.", uReturn, cbExecName);
						MessageBox(NULL, szMessage, NULL, MB_OK);
					}
				}
				else
				{
					nResult = 1;
				}
			}
			else
			{
			/* Launched by association. */
				nResult = 1;
			}
		}
	}
	
	return (int)nResult;
}

// Initialize the main variables used by this program.

static BOOL SetWorkingVariables(LPCSTR pszCommandLine)
{
	// Setup the working directory.

	if (GetModuleFileName(m_hInstance, m_szWorkingDirectory, sizeof(m_szWorkingDirectory)-1) == 0)
	{
		return FALSE;
	}
	
	// Skip the drive.
	
	char* ptr1;
	if ((ptr1 = strchr(m_szWorkingDirectory, ':')) == NULL)
	{
		ptr1 = m_szWorkingDirectory;
	}
	else
	{
		ptr1++; 
	}
	
	// Skip all directories.
	
	char *ptr2;
	while ((ptr2 = strchr(ptr1, '\\')) != NULL)
	{
		ptr1 = ptr2+1;
	}
	*ptr1 = '\0';		// Knock off the final name.

	// Make sure this is terminated correctly with trailing backslash.
	
	AppendSlashToPath(m_szWorkingDirectory);
	
	// Compute the name of the MSREG.INI file to use.
	lstrcpy(m_szMsregIni, m_szWorkingDirectory);
	lstrcat(m_szMsregIni, "MSREG.INI");

	// Now that we know where MSREG.INI is, get the name to use for
	// the global atom used to signal our presence. If there is no
	// atom name specified, then no atom is created. This is used to
	// prevent auto-run from running if we need to ask for the CD and
	// the user inserts the CD.

	static char szAtomName[_MAX_PATH];

	GetPrivateProfileString(
		MSREGINI_Section_Configuration,
		MSREGINI_Configuration_SkipAutoRunAtom,
		"",
		szAtomName,
		sizeof(szAtomName),
		m_szMsregIni);

	if (lstrlen(szAtomName) > 0)
	{
		AutoRunAtom.Name(szAtomName);
		AutoRunAtom.Create();			// If it fails.. oh, well.
	}
   	
	// Get the name of the configuration to launch. It should be the
	// first and only parameter. Leading spaces are ignored.
	
	ptr1 = (LPSTR)pszCommandLine;
	ptr2 = m_szLaunchName;
	while (*ptr1 == ' ')
	{
		ptr1++;
	}
	while ((*ptr1 != ' ') && (*ptr1 != '\0'))
	{
		*(ptr2++) = *(ptr1++);
	}
	*ptr2 = '\0';
		
	// Make sure we have a name.
		
	if (strlen(m_szLaunchName) == 0)
	{
		return FALSE;
	}
	
	// Get our default CD name.

	GetPrivateProfileString(
		MSREGINI_Section_Configuration,
		MSREGINI_Configuration_CDName,
		"the Mindscape CD",
		m_szCDName,
		sizeof(m_szCDName)-1,
		m_szMsregIni);

	// Read the parameters for the given launch configuration.
		
	if (GetPrivateProfileString(m_szLaunchName, "Name", "", m_szName, sizeof(m_szName)-1, m_szMsregIni) == 0)
	{
		return FALSE;
	}
	
	GetPrivateProfileString(m_szLaunchName, "CD", m_szCDName, m_szCD, sizeof(m_szCD)-1, m_szMsregIni);

	GetPrivateProfileString(m_szLaunchName, "Exec", "%N", m_szExec, sizeof(m_szExec)-1, m_szMsregIni);

	// Append any extra parameters onto the end of the launch string.
	strcat(m_szExec, ptr1);

	m_fUseAssociation = GetPrivateProfileInt(m_szLaunchName, "UseAssociation", FALSE, m_szMsregIni);
	m_fSetWorkingDirectory = GetPrivateProfileInt(m_szLaunchName, "SetWorkingDirectory", TRUE, m_szMsregIni);
   m_fTryNativeBitness = GetPrivateProfileInt(m_szLaunchName, "TryNativeBitness", TRUE, m_szMsregIni);
   m_fUseDLLPath = GetPrivateProfileInt(m_szLaunchName, "UseDLLPath", TRUE, m_szMsregIni);

	// Get the application name.
			
	if (GetPrivateProfileString(
			MSREGINI_Section_Configuration,
			MSREGINI_Configuration_Application,
			"",
			m_szAppName,
			sizeof(m_szAppName)-1,
			m_szMsregIni) == 0)
	{
		return FALSE;
	}

	// Get our base DLL path.

	GetPrivateProfileString(
		MSREGINI_Section_Configuration,
		MSREGINI_Configuration_DllPath,
		"MSREG",
		m_szDefaultPath,
		sizeof(m_szDefaultPath)-1,
		m_szMsregIni);
									
	// Strip off any leading or trailing black slashes from the DLL path.
	
	int i = 0;
	int l = lstrlen(m_szDefaultPath);
	
	while (m_szDefaultPath[i] == '\\')
	{
		i++;
	}
	if (i > 0)
	{
		for (int j = 0; i <= l; i++, j++)
		{
			m_szDefaultPath[j] = m_szDefaultPath[i];
		}
	}
	
	i = lstrlen(m_szDefaultPath)-1;
	while ((i >= 0) && (m_szDefaultPath[i] == '\\'))
	{
		i--;
	}
	m_szDefaultPath[i+1] = '\0';
	
	// Everything seems to have worked.

#if 0
	char szMessage[256];

	wsprintf(szMessage, "Working Directory = |%s|\n", m_szWorkingDirectory);
	::MessageBox(NULL, szMessage, NULL, MB_OK);

	wsprintf(szMessage, "MSREG.INI = |%s|\n", m_szMsregIni);
	::MessageBox(NULL, szMessage, NULL, MB_OK);

	wsprintf(szMessage, "Launch = |%s|\n", m_szLaunchName);
	::MessageBox(NULL, szMessage, NULL, MB_OK);

	wsprintf(szMessage, "Name = |%s|\n", m_szName);
	::MessageBox(NULL, szMessage, NULL, MB_OK);

	wsprintf(szMessage, "Exec = |%s|\n", m_szExec);
	::MessageBox(NULL, szMessage, NULL, MB_OK);

	wsprintf(szMessage, "Use Association = %d\n", m_fUseAssociation);
	::MessageBox(NULL, szMessage, NULL, MB_OK);

	wsprintf(szMessage, "Set Working Directory = %d\n", m_fSetWorkingDirectory);
	::MessageBox(NULL, szMessage, NULL, MB_OK);

	wsprintf(szMessage, "Try Native Bitness = %d\n", m_fTryNativeBitness);
	::MessageBox(NULL, szMessage, NULL, MB_OK);

	wsprintf(szMessage, "Application = |%s|\n", m_szAppName);
	::MessageBox(NULL, szMessage, NULL, MB_OK);

	wsprintf(szMessage, "CD = |%s|\n", m_szCD);
	::MessageBox(NULL, szMessage, NULL, MB_OK);

	wsprintf(szMessage, "Default Path = |%s|\n", m_szDefaultPath);
	::MessageBox(NULL, szMessage, NULL, MB_OK);
#endif

	return TRUE;
}


// Private version of access().

static long AccessFile(LPCSTR pszName, int nMode)
{
	long lRet;

#ifdef _WIN32

	// 32-bit version
	
	WIN32_FIND_DATA ffd;
	HANDLE hSearch;

	if ((hSearch = FindFirstFile(pszName, &ffd)) == INVALID_HANDLE_VALUE)
	{
		lRet = GetLastError();
	}
	else
	{
		lRet = NO_ERROR;
		if ((nMode & 2) && (ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
		{
			// We want to be able to write to it but we can't.
			lRet = ERROR_ACCESS_DENIED;
		}
		FindClose(hSearch);
	}
	
#else

	// 16-bit version
	
	struct _find_t buffer;

	// Preload the struct.

	lstrcpy(buffer.name, "Nope!");
	buffer.attrib = -1;
	buffer.size = -1L;

	int nAccess = _A_NORMAL;

	if (!(nMode & 2))
	{
		// Doesn't require write permission.
		nAccess |= _A_RDONLY;
	}

	lRet = (long)_dos_findfirst(pszName, nAccess, &buffer);

	if (lRet == 0x12)
	{
		// "No more files" == "file not found"
		lRet = 2;
	}

	if (lRet == 0)
	{
		// Let's be REALLY sure we found the file, that this is not just a bogus
		// return value (it happens!).
		// We compare the name in the find buffer to the base name of what
		// we were looking for.
		
		int nNameLength = lstrlen(pszName);
		int nBufferLength = lstrlen(buffer.name);

		if ((nNameLength < nBufferLength)
		 || (lstrcmpi(pszName + nNameLength - nBufferLength, buffer.name) != 0))
		{
			// Names don't match! We didn't really find the file.
			lRet = 2;		// Not found.
		}
	}
#endif

	return lRet;
}

// Does a file exist?

static BOOL FileExists(LPCSTR pszFileName)
{
	UINT uPrevSem = SetErrorMode(SEM_FAILCRITICALERRORS);
	BOOL fExists = AccessFile(pszFileName) == 0;
	SetErrorMode(uPrevSem);
	return fExists;
}

// Locate a file.
// This will prompt the user for the CD if necessary.

static BOOL LocateFile(void)
{
	CDriveManager DriveManager;

	if (DriveManager.Open())
	{
		DriveManager.InitDrives();

		// Look in the current directory first.

		SetFullName(m_szWorkingDirectory, FALSE);

		if (IsOurHomeCD())
		{
			return TRUE;
		}

		// Look relative to current directory first.

		if (m_fUseDLLPath)
		{
			SetFullName(m_szWorkingDirectory);

			if (IsOurHomeCD())
			{
				return TRUE;
			}
		}

		// Look in root directories of drives.

		SetFullName("?:\\", m_fUseDLLPath);

		// We need to search in a number of places for the home CD.
	
		for (;;)
		{
			// Search on all CD-ROM drives.

			char& cDrive = m_szFullName[0];			// Cheat a little...

			for (cDrive = 'A'; cDrive <= 'Z'; cDrive++)
			{
				DriveType Type = DriveManager.GetDriveType(cDrive);
				m_szFullPath[0] = cDrive;

				if ((Type == DT_CDROM) && IsOurHomeCD())
				{
					return TRUE;
				}
			}

			// Search on all remote drives.

			for (cDrive = 'A'; cDrive <= 'Z'; cDrive++)
			{
				DriveType Type = DriveManager.GetDriveType(cDrive);
				m_szFullPath[0] = cDrive;

				if ((Type == DT_Remote) && IsOurHomeCD())
				{
					return TRUE;
				}
			}

			// Search on all non-remote, non-CD >= 'C'.

			for (cDrive = 'C'; cDrive <= 'Z'; cDrive++)
			{
				DriveType Type = DriveManager.GetDriveType(cDrive);
				m_szFullPath[0] = cDrive;

				if ((Type != DT_CDROM) && (Type != DT_Remote) && IsOurHomeCD())
				{
					return TRUE;
				}
			}

			// The volume could not be found. Ask the user to put it in a drive.
	
			if (!PromptForHomeCD())
			{
				// User said forget it.
				break;
			}
		}
		
		DriveManager.Close();
	}
	
	return FALSE;
}

static void SetFullName(LPCSTR pszPath, BOOL fAppendDefaultPath /*=TRUE*/)
{
	// Setup a full path name for the file for which we are searching.
	lstrcpy(m_szFullPath, pszPath);
	
	if (fAppendDefaultPath)
	{
		AppendSlashToPath(m_szFullPath);
		lstrcat(m_szFullPath, m_szDefaultPath);
	}
	
	lstrcpy(m_szFullName, m_szFullPath);
	AppendSlashToPath(m_szFullName);
	lstrcat(m_szFullName, m_szName);
}

// Does the file pointed to by m_szFullName exist on "our" CD?
// If the file is found, then its name may have been changed with
// a "16" or "32" at the end.

static BOOL IsOurHomeCD(void)
{
	// Make a local copy of the file name since we will be
	// changing it.
	
	static char szName[_MAX_PATH];
	lstrcpy(szName, m_szFullName);
	
	// Does the file exist?

#if 0
	char szMessage[256];
	wsprintf(szMessage, "Check |%s|", szName);
	MessageBox(NULL, szMessage, NULL, MB_OK);
#endif

	if (!FileExists(szName))
	{
		if (!m_fTryNativeBitness)
		{
			return FALSE;
		}
		
		// The current name does not exist, try appending "16" or "32" to the
		// end and try again.
		
		int n = lstrlen(szName)-1;
		BOOL fFoundExtension = FALSE;
		
		while (n >= 0)
		{
			if (szName[n] == '.')
			{
				fFoundExtension = TRUE;
				break;
			}
			
			if ((szName[n] == '\\') || (szName[n] == ':'))
			{
				break;
			}
			
			n--;
		}
		
		if (!fFoundExtension)
		{
			n = lstrlen(szName);
		}
		
		szName[n] = '\0';
#ifdef WIN32
		lstrcat(szName, "32");
#else
		lstrcat(szName, "16");
#endif
		lstrcat(szName, &(m_szFullName[n]));

#if 0
		wsprintf(szMessage, "Check |%s|", szName);
		MessageBox(NULL, szMessage, NULL, MB_OK);
#endif

		if (!FileExists(szName))
		{
			return FALSE;
		}
	}
	
	// Build a path to a possible MSREG.INI in that directory for verification.
	
	char cbMsregName[_MAX_PATH];
	lstrcpy(cbMsregName, m_szFullPath);
	AppendSlashToPath(cbMsregName);
	lstrcat(cbMsregName, "MSREG.INI");

	char cbAppEntry[_MAX_PATH];

	// It's OK if there is no such entry OR the entry is our name.

	if (GetPrivateProfileString(
			MSREGINI_Section_Configuration,
			MSREGINI_Configuration_Application,
			"",
			cbAppEntry,
			sizeof(cbAppEntry)-1,
			cbMsregName) > 0)
	{
		if (lstrcmpi(cbAppEntry, m_szAppName) != 0)
		{
			return FALSE;
		}
	}
	
	// The file was found, remember its name.

	lstrcpy(m_szFullName, szName);

#if 0
	wsprintf(szMessage, "Found |%s|", m_szFullName);
	MessageBox(NULL, szMessage, NULL, MB_OK);
#endif
	
	return TRUE;
}

// Ask the user to put in the home CD.

static PromptForHomeCD(void)
{
	// Build the message string.

	char szMessage[256];
	lstrcpy(szMessage, "Please insert ");
	lstrcat(szMessage, m_szCD);
	lstrcat(szMessage, "\ninto any drive and press OK.\n\nPress Cancel if the CD is not available.");

	// Put up the message box.

	if (MessageBox(
			NULL,
			szMessage,
			"Insert CD",
			MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
	{
		return TRUE;
	}

	// Otherwise, just cancel for now.

	return FALSE;
}


// Build the exec name.
// This will fill in various user-specified arguments.
//
// %N - Add the file name.
// %W - Add the working directory for the MSRUN program.

static void BuildExecName(LPSTR pszExecName, LPCSTR pszTemplate)
{
	while (*pszTemplate != '\0')
	{
		char ch;

		switch (ch = *pszTemplate++)
		{
			case '%':
			{
				// Get next character to evaluate the escape.

				ch = *pszTemplate++;

				switch (toupper(ch))
				{
					case 'N':
					{
						// File name.
						char szShortName[_MAX_PATH+1];
#ifdef WIN32
						GetShortPathName(m_szFullName, szShortName, sizeof(szShortName)-1);
#else
						strcpy(szShortName, m_szFullName);
#endif
						lstrcpy(pszExecName, szShortName);
						pszExecName += lstrlen(pszExecName);
						break;
					}
					case 'W':
					{
						// Working directory.
						char szShortName[_MAX_PATH+1];
#ifdef WIN32
						GetShortPathName(m_szWorkingDirectory, szShortName, sizeof(szShortName)-1);
#else
						strcpy(szShortName, m_szWorkingDirectory);
#endif
						lstrcpy(pszExecName, szShortName);
						pszExecName += lstrlen(pszExecName);
						break;
					}
					default:
					{
						*(pszExecName++) = ch;
						break;
					}
				}
				break;
			}
			default:
			{
				*(pszExecName++) = ch;
				break;
			}
		}
	}
	
	*pszExecName = '\0';
}

// Make sure a path has a backslash on the end.

static void AppendSlashToPath(LPSTR pszPath)
{
	int nLength = lstrlen(pszPath);
	if ((nLength != 0)
	 && (pszPath[nLength-1] != '\\')
	 && (pszPath[nLength-1] != ':'))
	{
		pszPath[nLength++] = '\\';
		pszPath[nLength] = '\0';
	}
}

/////////////////////////////////////////////////////////////////////////////
// Code for determining the window of a previous instance.

// Make sure a path does not have a backslash on the end.

static void RemoveSlashFromPath(LPSTR pszPath)
{
	int nLength = lstrlen(pszPath);
	if ((nLength > 1)
	 && (pszPath[nLength-1] == '\\')
	 && (pszPath[nLength-2] != ':'))
	{
		pszPath[nLength-1] = '\0';
	}
}

struct EnumData
{
	LPCSTR pFilename;
	HWND hWndFound;
};

// Callback function for EnumWindows()
#ifdef WIN32
BOOL CALLBACK WndEnumProc(HWND hWnd, LPARAM lParam)
#else
BOOL __far __pascal __export WndEnumProc(HWND hWnd, LPARAM lParam)
#endif
{
	EnumData* pData = (EnumData*)lParam;

	HINSTANCE hInst;        // hInstance of current window being examined
	char szFilename[_MAX_PATH];

// The following two lines were in the existing code, but it caused a problem
// (as acknowledged in the article this was taken from): if the window had a
// popup message box, the window was disabled and was not found. So I thought
// it would be good to allow disabled windows to be found.
// Then the GetLastActivePopup() will give us the window to use.
//
//	if (!IsWindowEnabled(hWnd))   // Don't bother if disabled HWND
//		return TRUE;                        // continue enumeration

	if (!IsWindowVisible(hWnd))   // Don't bother if invisible HWND
		return TRUE;                        // continue enumeration

	// Get the filename of the task/module that owns this window
#ifdef WIN32
	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
#else
	HINSTANCE hInstance = (HINSTANCE)GetWindowWord(hWnd, GWW_HINSTANCE);
#endif
	GetModuleFileName(hInstance, szFilename, sizeof(szFilename));

	// Does the filename for this HWND match what was passed to ShellExecute?
	// If not, continue the enumeration
	if (lstrcmpi(szFilename, pData->pFilename) == 0)
	{
		// We found a window for the first instance of the program.
		pData->hWndFound = hWnd;

		return FALSE;   // Stop the enumeration. We found the right window
	}

	return TRUE;
}

static HWND GetTheOwningWindow(HWND hWnd)
{
    HWND hWndTemp = hWnd;

    while (hWndTemp = GetParent(hWnd))  // Walk up the parent window chain
        hWnd = hWndTemp;                // to find the ultimate parent HWND

    hWndTemp = hWnd;    // Start with ultimate parent window

    // Walk up the owning window chain to find the ultimate owning window
    while ( hWndTemp = GetWindow(hWnd, GW_OWNER) )
        hWnd = hWndTemp;

    return hWnd;        // Return the ultimate owning window
}

static void BringFirstInstanceToTop(HWND hWnd)
{
	HWND hWndLastActive;

	hWnd = GetTheOwningWindow(hWnd);
	hWndLastActive = GetLastActivePopup(hWnd);

	if ( IsIconic(hWnd) )
	{
		ShowWindow(hWnd, SW_SHOWNORMAL);
	}
	else
	{
#ifdef WIN32
		SetForegroundWindow(hWnd);
#endif
		BringWindowToTop(hWnd);
		if ( hWndLastActive && (hWndLastActive != hWnd) )
		{
#ifdef WIN32
			SetForegroundWindow(hWndLastActive);
#endif
			BringWindowToTop( hWndLastActive );
		}
	}
}

static BOOL ActivatePreviousInstance(LPSTR pszPath)
{
#ifdef WIN32
	char szWindowTitle[_MAX_PATH];

	if (GetPrivateProfileString(m_szLaunchName, "WindowTitle", "", szWindowTitle, sizeof(szWindowTitle)-1, m_szMsregIni) == 0)
	{
		return FALSE;
	}

	HWND hWnd = FindWindow(NULL, szWindowTitle);
	if (hWnd != NULL)
	{
		BringFirstInstanceToTop(hWnd);
		return TRUE;
	}
#else
	char Filename[_MAX_PATH];

	strcpy(Filename, pszPath);
	char* pSpace = strchr(Filename, ' ');
	if (pSpace != NULL)
	{
	/* Truncate it there. Assume that it's an argument. */
		*pSpace = '\0';
	}

	EnumData Data;
	Data.pFilename = Filename;
	Data.hWndFound = NULL;

	EnumWindows((WNDENUMPROC)WndEnumProc, (long)&Data);

	if (Data.hWndFound != NULL)
	{
		/* Bring the first instance's window to the front (see below) */
		BringFirstInstanceToTop(Data.hWndFound);
		return TRUE;
	}
#endif
	return FALSE;
}
