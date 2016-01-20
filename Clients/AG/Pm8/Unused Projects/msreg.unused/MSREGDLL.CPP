/*
// $Workfile: MSREGDLL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSREGDLL.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.17   24 Oct 1997 12:13:36   Fred
// Fixed bug in registration code comparison
// 
//    Rev 1.16   16 Jul 1997 13:46:14   Fred
// Fixed register by modem bug
// 
//    Rev 1.15   18 Jun 1997 08:49:28   Fred
// Merged in Sean's changes for NGI
// 
//    Rev 1.15   09 Jun 1997 16:28:40   Fred
// Switched registration code to last four digits of PPN reversed
// 
//	(don't have the MicroLogic PVCS config, so can't check into it!)
//    Rev 1.14	03 JUN 1997  20:27:00	SBS
//	Added DoRuns and support functions.  This wraps the TimeToRegister()
//	function (if DoRuns returns TRUE, then registration can run (or still
//	check for skips/time), otherwise, some fixed number of registration
//	runs has occurred, and so we no longer will request the user to register).
//	This functionality was demanded by the National Geographic project --
//	they are the first title (besides PrintMaster) to utilize registration
//	from the app (rather than AutoRUN).
//
//    Rev 1.13   27 Dec 1995 15:44:08   FRED
// Looks for DLL in current directory.
// 
// 
//    Rev 1.12   14 Dec 1995 11:07:22   FRED
// Changes for VC++ 4.0
// 
//    Rev 1.11   11 Dec 1995 16:30:58   JAY
// Win32s stuff
// 
//    Rev 1.10   07 Dec 1995 11:09:56   JAY
// Added some limited variable access code for 16-bit version.
// 
//    Rev 1.9   07 Dec 1995 10:43:18   FRED
// Strips slashes from DLLPath
// 
//    Rev 1.8   06 Dec 1995 13:41:10   JAY
// Now tries to validate application name in MSREG.INI.
// 
//    Rev 1.7   05 Dec 1995 13:03:32   JAY
// Event notification.
// 
//    Rev 1.6   28 Nov 1995 16:13:48   FRED
// No Free variations
// 
//    Rev 1.5   28 Nov 1995 10:26:02   FRED
//  
// 
//    Rev 1.4   28 Nov 1995 10:23:10   FRED
// Fixed int/long mismatch
// 
//    Rev 1.3   28 Nov 1995 10:20:34   FRED
// Mirrored key stuff.
// 
//    Rev 1.2   28 Nov 1995 10:04:50   FRED
// IsBonusEnabled and GetRegistrationData
// 
//    Rev 1.1   22 Nov 1995 13:06:58   JAY
//  
// 
//    Rev 1.0   22 Nov 1995 08:52:08   FRED
// Initial revision.
// 
//    Rev 1.16   16 Nov 1995 13:24:26   JAY
// 
//    Rev 1.15   16 Nov 1995 13:05:02   JAY
// Defined our own ioinit replacement to reduce code size even more.
// 
//    Rev 1.14   16 Nov 1995 12:31:30   FRED
//  
// 
//    Rev 1.13   16 Nov 1995 12:24:08   FRED
//  
// 
//    Rev 1.12   16 Nov 1995 11:00:38   FRED
//  
// 
//    Rev 1.11   16 Nov 1995 10:31:20   FRED
//  
// 
//    Rev 1.7   23 Oct 1995 12:12:24   JAY
// Changed "registered" section from "Register" to "BDSServer".
// 
//    Rev 1.7   23 Oct 1995 12:12:16   JAY
// 
//    Rev 1.6   23 Oct 1995 11:09:26   JAY
// Nothing.
// 
//    Rev 1.5   20 Oct 1995 17:21:34   JAY
// Support for registration skipping.
// 
//    Rev 1.4   20 Oct 1995 14:56:26   JAY
// More small changes
// 
//    Rev 1.3   20 Oct 1995 11:42:16   JAY
// Got rid of OutputDebugStrings.
// 
//    Rev 1.2   20 Oct 1995 11:39:06   JAY
// More changes.
// 
//    Rev 1.1   20 Oct 1995 10:59:34   JAY
// Fixes.
// 
//    Rev 1.0   20 Oct 1995 08:34:18   JAY
// Initial revision.
*/

#include <windows.h>
#include <string.h>
#include <time.h>
#ifndef WIN32
#include <dos.h>
#endif

#ifndef WIN32
#include <stdlib.h>
#endif

#ifdef WIN32
#define REGENTRYPOINT __declspec(dllexport) REGRESULT FAR PASCAL
#else
#define REGENTRYPOINT REGRESULT FAR PASCAL _export
#endif

#include "msreg.h"
#include "msregini.h"
#include "msregent.h"
#include "command.h"
#include "drivemgr.h"

HINSTANCE m_hInstance;
char m_cbWorkingDirectory[_MAX_PATH];
char m_cbIniName[_MAX_PATH];
char m_cbUserIniName[_MAX_PATH];
char m_cbDllName[_MAX_PATH];
char m_cbDllPath[_MAX_PATH];
char m_cbAppName[_MAX_PATH];
char m_cbCD[_MAX_PATH];
char m_cbDllDefaultPath[_MAX_PATH];

#ifndef WIN32
HTASK m_hEventTask = 0;			// The task that owns the notify.
#endif
EVENT_NOTIFY_FUNCTION m_pEventNotification = NULL;

#define SECONDS_IN_DAY	(60L*60L*24L)

// Forward references.
static BOOL UserIsRegistered(void);
static BOOL UserIsBonusEnabled(void);
static REGRESULT RunRegister(HWND hWnd, DWORD dwData, LPCSTR pszDllPath);
static void SetWorkingVariables(void);
static void SetDllPathName(LPCSTR pszPath, BOOL fAddDefault = TRUE);
static long AccessFile(LPCSTR pName, int mode = 0);
static BOOL FileExists(LPCSTR pFileName);
static BOOL IsOurHome(void);
static BOOL LocateDllFile(void);
static BOOL PromptForHomeCD(void);
static unsigned long KeyFromString(LPCSTR pString);

// Register skipping code.
static BOOL TimeToRegister(void);		// Main
static BOOL AnySkipsLeft(void);
static BOOL AnyTimeLeft(void);
static BOOL DoRuns(void);

// Run count management.
static void ResetRunsLeft(void);
static int GetRunsLeft(void);
static int GetRunsCount(void);

// Skip count management.
static void ResetSkips(void);
static int GetSkipsLeft(void);
static int GetSkipCount(void);

// Skip time management.
static void ResetLastTime(void);
static long GetLastTime(void);
static long GetTimeInterval(void);

// INI management.
static void RemoveUserEntry(LPCSTR pszSection, LPCSTR pszEntry);
static void WriteUserProfileInt(LPCSTR pszSection, LPCSTR pszEntry, int nValue, int nDefault);
static long GetProfileLong(LPCSTR pszSection, LPCSTR pszEntry, long lDefault, LPCSTR pszIniName);
static void WriteUserProfileLong(LPCSTR pszSection, LPCSTR pszEntry, long lValue, long lDefault);

extern "C"
{
	void _setenvp(void)
	{
	}
	
	void _setargv(void)
	{
	}
};

/*
// Send a register command.
*/

extern "C" REGENTRYPOINT RegSendCommand(HWND hWnd, LPCSTR pszCommand, DWORD dwData, LPCSTR pszMsregxDllPath)
{
	// Setup the working variables.
	SetWorkingVariables();

	// Parse the command.
	CCommandParser Command;
	if (!Command.Init(pszCommand))
	{
		return REGRESULT_CommandError;
	}

	// Branch on main command.
	if (lstrcmpi(Command.Command(), REGCOMMAND_Register) == 0)
	{
	/*
	// See if we are registered.
	*/

		if (!Command.Option('A') && UserIsRegistered())
		{
		/* User is already registered. */
			return REGRESULT_AlreadyRegistered;
		}

		if (Command.Option('C') || TimeToRegister())
		{
			/*
			// Run the register code.
			*/

			return RunRegister(hWnd, dwData, pszMsregxDllPath);
		}

		/*
		// Return the registered status of the user.
		*/
		return UserIsRegistered()
							? REGRESULT_NowRegistered
							: REGRESULT_NotRegistered;
	}
	else if (lstrcmpi(pszCommand, REGCOMMAND_IsRegistered) == 0)
	{
		/*
		// Return the registered status of the user.
		*/
		return UserIsRegistered()
							? REGRESULT_AlreadyRegistered
							: REGRESULT_NotRegistered;
	}
#ifndef NO_FREE_BONUS
	else if (lstrcmpi(pszCommand, REGCOMMAND_IsBonusEnabled) == 0)
	{
		/*
		// Return the "bonus enabled" status of the user.
		*/
		return UserIsBonusEnabled()
							? REGRESULT_AlreadyRegistered
							: REGRESULT_NotRegistered;
	}
#endif
	else if (lstrcmpi(pszCommand, REGCOMMAND_SetEventNotify) == 0)
	{
#ifdef WIN32
		m_pEventNotification = (EVENT_NOTIFY_FUNCTION)dwData;
#else
		HTASK hTask = GetCurrentTask();
		if (m_hEventTask == 0 || m_hEventTask == hTask)
		{
			m_hEventTask = hTask;
			m_pEventNotification = (EVENT_NOTIFY_FUNCTION)dwData;
		}
#endif
		return (dwData == 0)
					? REGRESULT_NotRegistered
					: REGRESULT_NowRegistered;
	}
	else if (lstrcmpi(pszCommand, REGCOMMAND_GetRegistrationData) == 0)
	{
		if (!UserIsRegistered())
		{
			return REGRESULT_NotRegistered;
		}

		REGDATA_GetRegistrationData* pData = (REGDATA_GetRegistrationData*)dwData;
		if ((pData == NULL)
		 || (pData->m_pszField == NULL)
		 || (pData->m_pszData == NULL)
		 || (pData->m_nData <= 1))
		{
			return REGRESULT_Error;
		}

		GetPrivateProfileString(
			MSREGUSRINI_Section_CustomerInfo,
			pData->m_pszField,
			"",
			pData->m_pszData,
			pData->m_nData,
			m_cbUserIniName);

		return REGRESULT_AlreadyRegistered;
	}

/*
// They passed us something bad.
*/

	return REGRESULT_CommandError;
}

/*
// Is it time to register?
*/

static BOOL TimeToRegister(void)
{
	/* Check skips first since it always needs to happen. */
	if (AnySkipsLeft() || AnyTimeLeft())
	{
		return FALSE;
	}

	// Okay, skips/time related bypass not in effect -- how about
	// runcount limit (as in "only ask three times").
	// performed AFTER skip testing for predictability - thus we can say
	// "prompt only every 3 days, but then only do that four times".

	if ( DoRuns() )
	{
		// Either runs not tracked, or we're still enabled as of this run.

		return TRUE;
	}

	// If no runs left, we don't register.
	return FALSE;
}


/*
// Do we have any runs left?
*/

static BOOL DoRuns(void)
{
	// Runs is a count of times to ACTUALLY prompt.
	// 0 (or not set) means that runs isn't used.
	// Otherwise, a number represents how many times to check, and after
	// that number of checks, we don't prompt for registration.

	int nRunsCount = GetRunsCount();

	if (nRunsCount != 0)
	{
		int nRunsLeft = GetRunsLeft();

		/* We are running... */

		if ( nRunsLeft == -2 )
		{
			// the runs counter hasn't been initialized.  Do so
			ResetRunsLeft();

			// and set the default count for the following code.
			nRunsLeft = nRunsCount;
		}

		if (nRunsLeft == -1)
		{
			/* No more runs.  Don't do registration! */
			return FALSE;
		}
		else
		{
			/* Decrement run count, but still count registration */
			// NOTE: this writeUserprofileInt function is MicroLogic code,
			// and apparently if passed 0 as the value to write, DELETES
			// the key (which isn't good for us).  So if the value to be
			// written is 0 (== no more runs), we set it to -1 (which is
			// what we now use above to check if no runs left).
			
			WriteUserProfileInt( m_cbAppName,
								 MSREGUSRINI_Application_RunsLeft,
								 ( ( nRunsLeft - 1 ) == 0 ) ? ( -1 ) : ( nRunsLeft - 1 ),
								 0);
			return TRUE;
		}
	}

	// Runs not tracked, so we always have runs available.
	return TRUE;
}

/*
// Reset the runs count. (not actually implemented in the interface)
// Set the number of runs left to the runs count (if it's not zero).
*/

static void ResetRunsLeft(void)
{
	/* Set the number of runs.  Not so much as RESET, as in set the first
	 * time through. */
	WriteUserProfileInt(m_cbAppName, MSREGUSRINI_Application_RunsLeft, GetRunsCount(), 0);
}

static int GetRunsLeft(void)
{
	// Returns -2 if the runs hasn't been initialized.
	return GetPrivateProfileInt(m_cbAppName,
										 MSREGUSRINI_Application_RunsLeft,
										 -2,
										 m_cbUserIniName);
}

static int GetRunsCount(void)
{
	return GetPrivateProfileInt(MSREGINI_Section_Configuration,
										 MSREGINI_Configuration_RunsCount,
										 0,
										 m_cbIniName);
}


/*
// Do we have any skips left?
*/

static BOOL AnySkipsLeft(void)
{
	int nSkipCount = GetSkipCount();

	if (nSkipCount != 0)
	{
		int nSkipsLeft = GetSkipsLeft();

	/* We are skipping. */

		if (nSkipsLeft == 0)
		{
		/* No more skips. Reset the count. */
			ResetSkips();
		}
		else
		{
		/* Skip this time. */
			WriteUserProfileInt(m_cbAppName, MSREGUSRINI_Application_SkipsLeft, nSkipsLeft-1, 0);
			return TRUE;
		}
	}
/* No skips left. */
	return FALSE;
}

/*
// Is there any time left to skip?
*/

static BOOL AnyTimeLeft(void)
{
	long lInterval = GetTimeInterval();
	if (lInterval == 0)
	{
	// Nothing to skip.
		return FALSE;
	}

	time_t lNow;
	time(&lNow);

	time_t lLastTime = GetLastTime();
	lInterval = lInterval*SECONDS_IN_DAY;

	if (lNow >= lLastTime + lInterval)
	{
	/* Time to register! */
		ResetLastTime();
		return FALSE;
	}

/* Keep skipping until time. */

	return TRUE;
}

/*
// Reset the skips count.
// Set the number of skips left to the skip count (if it's not zero).
*/

static void ResetSkips(void)
{
/* Set the number of skips. */
	WriteUserProfileInt(m_cbAppName, MSREGUSRINI_Application_SkipsLeft, GetSkipCount(), 0);
}

static int GetSkipsLeft(void)
{
	return GetPrivateProfileInt(m_cbAppName,
										 MSREGUSRINI_Application_SkipsLeft,
										 0,
										 m_cbUserIniName);
}

static int GetSkipCount(void)
{
	return GetPrivateProfileInt(MSREGINI_Section_Configuration,
										 MSREGINI_Configuration_SkipCount,
										 0,
										 m_cbIniName);
}

static void ResetLastTime(void)
{
	time_t lNow;
	time(&lNow);

	WriteUserProfileLong(m_cbAppName, MSREGUSRINI_Application_LastTime, lNow, 0);
}

static long GetLastTime(void)
{
	return GetProfileLong(m_cbAppName, MSREGUSRINI_Application_LastTime, 0, m_cbUserIniName);
}

static long GetTimeInterval(void)
{
	return GetProfileLong(MSREGINI_Section_Configuration, MSREGINI_Configuration_TimeInterval, 0, m_cbIniName);
}

/*
// Is the user registered?
*/

static BOOL UserIsRegistered(void)
{
	return GetPrivateProfileInt(m_cbAppName,
										 MSREGUSRINI_Application_Registered,
										 0,
										 m_cbUserIniName);
}

#ifndef NO_FREE_BONUS
/*
// Is the user Bonus Enabled?
*/

static BOOL UserIsBonusEnabled(void)
{
	BOOL fResult = FALSE;

	char szRegistrationCode[256];

	GetPrivateProfileString(
		m_cbAppName,
		MSREGUSRINI_Application_RegistrationCode,
		"",
		&szRegistrationCode[0],
		sizeof(szRegistrationCode)-1,
		m_cbUserIniName);

	if (lstrcmp(szRegistrationCode, MODEM_REGISTRATION_CODE) == 0)
	{
		return TRUE;
	}

	int nRegistrationCodeLength = lstrlen(szRegistrationCode);
	if (nRegistrationCodeLength > 0)
	{
		// 6/6/97 (FF) Switched to scheme where registration code is last four digits of
		// Parent Part Number reversed.
		char szParentPartNumber[256];
		GetPrivateProfileString(
			MSREGINI_Section_Configuration,
			MSREGINI_Configuration_ParentPartNumber,
			"",
			&(szParentPartNumber[0]),
			sizeof(szParentPartNumber)-1,
			m_cbIniName);

		int nParentPartNumberLength = lstrlen(szParentPartNumber);
		if (nParentPartNumberLength >= 4)
		{
			for (int nRegistrationCodeIndex = 0; nRegistrationCodeIndex < nRegistrationCodeLength; nRegistrationCodeIndex++)
			{
				if (szRegistrationCode[nRegistrationCodeIndex] != ' ')
				{
					break;
				}
			}
			if (nRegistrationCodeIndex+4 == nRegistrationCodeLength)
			{
				fResult = TRUE;
				int nParentPartNumberIndex = nParentPartNumberLength-1;
				for (int i = 0; i < 4; i++)
				{
					if (szRegistrationCode[nRegistrationCodeIndex+i] != szParentPartNumber[nParentPartNumberIndex-i])
					{
						fResult = FALSE;
						break;
					}
				}
			}
		}

#if 0
		unsigned long lKey = (unsigned long)GetProfileLong(MSREGINI_Section_Configuration, MSREGINI_Configuration_Key, 0, m_cbIniName);
		if (lKey == KeyFromString(szRegistrationCode))
		{
			fResult = TRUE;
		}
#endif
	}

	return fResult;
}
#endif

static REGRESULT RunRegister(HWND hWnd, DWORD dwData, LPCSTR pszDllPath)
{
	REGRESULT lResult = REGRESULT_Error;
/*
// See if we need to search for the DLL to load.
*/

	BOOL fSearch = TRUE;
	if (pszDllPath != NULL)
	{
		SetDllPathName(pszDllPath, FALSE);
		if (IsOurHome())
		{
			fSearch = FALSE;
		}
	}

/*
// Search for the DLL if we don't have it yet.
*/

	if (fSearch)
	{
	/*
	// We need to locate the DLL to run.
	// Call the appropriate function.
	*/
		if (!LocateDllFile())
		{
		/*
		// We could not locate the DLL (the user cancelled out).
		// Just return REGRESULT_Error.
		*/
			return lResult;
		}
	}

/*
// We have the DLL path.
// Now load it.
*/

#ifdef WIN32
	char cbCWD[_MAX_PATH];

	GetCurrentDirectory(sizeof(cbCWD), cbCWD);
	SetCurrentDirectory(m_cbDllPath);
#endif

	UINT wOldSem = SetErrorMode(SEM_NOOPENFILEERRORBOX);
	HINSTANCE hLibrary = LoadLibrary(m_cbDllName);
	SetErrorMode(wOldSem);

#ifndef WIN32
	if ((UINT)hLibrary < (UINT)HINSTANCE_ERROR)
	{
		hLibrary = NULL;
	}
#endif
	if (hLibrary != NULL)
	{
#ifdef WIN32
		FN_REGISTER pRegister = (FN_REGISTER)GetProcAddress(hLibrary, "Register");
#else
		FN_REGISTER pRegister = (FN_REGISTER)GetProcAddress(hLibrary, "REGISTER");
#endif

		if (pRegister != NULL)
		{
			lResult = pRegister(hWnd,
									  dwData,
									  m_cbIniName,
#ifndef WIN32
									  GetCurrentTask() == m_hEventTask
											? m_pEventNotification
											: NULL);
#else
											m_pEventNotification);
#endif
		}

		FreeLibrary(hLibrary);
	}

#ifdef WIN32
	SetCurrentDirectory(cbCWD);
#endif
	return lResult;
}

static void AppendSlashToPath(LPSTR pPath)
{
	/* Make sure this is terminated correctly. */

	int length = lstrlen(pPath);
	if (length != 0 && pPath[length-1] != '\\' && pPath[length-1] != ':')
	{
		pPath[length++] = '\\';
		pPath[length] = '\0';
	}
}

static void SetWorkingVariables(void)
{
/*
// Setup the working directory.
*/

	::GetModuleFileName(m_hInstance, m_cbWorkingDirectory, sizeof(m_cbWorkingDirectory)-1);

/* Skip the drive. */

	char* ptr;
	if ((ptr = strchr(m_cbWorkingDirectory, ':')) == NULL)
	{
		ptr = m_cbWorkingDirectory;
	}
	else
	{
		ptr++; 
	}

/* Skip all directories . */

	char *ptr2;
	while ((ptr2 = strchr(ptr, '\\')) != NULL)
	{
		ptr = ptr2+1;
	}
	*ptr = '\0';			/* Knock off the final name. */

	AppendSlashToPath(m_cbWorkingDirectory);

	char buffer[_MAX_PATH+1];

/*
// Setup the MSREG.INI file name.
*/

	lstrcpy(m_cbIniName, m_cbWorkingDirectory);
	lstrcat(m_cbIniName, "MSREG.INI");

/*
// Setup the MSREGUSR.INI file name.
*/

	GetWindowsDirectory(buffer, sizeof(buffer)-1);

	/* Make sure this is terminated correctly. */

	AppendSlashToPath(buffer);

	lstrcpy(m_cbUserIniName, buffer);
	lstrcat(m_cbUserIniName, "MSREGUSR.INI");

/*
// Get the application name.
*/

	GetPrivateProfileString(
		MSREGINI_Section_Configuration,
		MSREGINI_Configuration_Application,
		"",
		m_cbAppName,
		sizeof(m_cbAppName)-1,
		m_cbIniName);

/*
// Get our CD name.
*/

	GetPrivateProfileString(MSREGINI_Section_Configuration,
									MSREGINI_Configuration_CDName,
									"the Mindscape CD",
									m_cbCD, sizeof(m_cbCD)-1, m_cbIniName);

/*
// Get our base DLL path.
*/

	GetPrivateProfileString(MSREGINI_Section_Configuration,
									MSREGINI_Configuration_DllPath,
									"MSREG",
									m_cbDllDefaultPath, sizeof(m_cbDllDefaultPath)-1, m_cbIniName);
									
	// Strip off any leading or trailing black slashes.
	int i = 0;
	int l = lstrlen(m_cbDllDefaultPath);
	
	while (m_cbDllDefaultPath[i] == '\\')
	{
		i++;
	}
	if (i > 0)
	{
		for (int j = 0; i <= l; i++, j++)
		{
			m_cbDllDefaultPath[j] = m_cbDllDefaultPath[i];
		}
	}
	
	i = lstrlen(m_cbDllDefaultPath)-1;
	while ((i >= 0) && (m_cbDllDefaultPath[i] == '\\'))
	{
		i--;
	}
	m_cbDllDefaultPath[i+1] = '\0';
}

static void SetDllPathName(LPCSTR pszPath, BOOL fAddDefault /*=TRUE*/)
{
/*
// Setup the MSREGXxx.DLL file name.
*/
	lstrcpy(m_cbDllPath, pszPath);
	if (fAddDefault)
	{
		AppendSlashToPath(m_cbDllPath);
		lstrcat(m_cbDllPath, m_cbDllDefaultPath);
	}
	lstrcpy(m_cbDllName, m_cbDllPath);
	AppendSlashToPath(m_cbDllName);
#ifdef WIN32
	lstrcat(m_cbDllName, "MSREGX32.DLL");
#else
	lstrcat(m_cbDllName, "MSREGX16.DLL");
#endif
}

/////////////////////////////////////////////////////////////////////////////
// INI file reading and writing.

/*
// Remove a setting.
*/

static void RemoveUserEntry(LPCSTR pszSection, LPCSTR pszEntry)
{
	WritePrivateProfileString(pszSection, pszEntry, NULL, m_cbUserIniName);
}

static void WriteUserProfileInt(LPCSTR pszSection, LPCSTR pszEntry, int nValue, int nDefault)
{
	if (nValue == nDefault)
	{
		RemoveUserEntry(pszSection, pszEntry);
	}
	else
	{
		char Buffer[40];
		itoa(nValue, Buffer, 10);

		WritePrivateProfileString(pszSection, pszEntry, Buffer, m_cbUserIniName);
	}
}

static long GetProfileLong(LPCSTR pszSection, LPCSTR pszEntry, long lDefault, LPCSTR pszIniName)
{
	char Buffer[40];

	if (GetPrivateProfileString(pszSection, pszEntry, "", Buffer, sizeof(Buffer)-1, pszIniName) > 0)
	{
		lDefault = atol(Buffer);
	}
	return lDefault;
}

static void WriteUserProfileLong(LPCSTR pszSection, LPCSTR pszEntry, long lValue, long lDefault)
{
	if (lValue == lDefault)
	{
		RemoveUserEntry(pszSection, pszEntry);
	}
	else
	{
		char Buffer[40];
		ltoa(lValue, Buffer, 10);

		WritePrivateProfileString(pszSection, pszEntry, Buffer, m_cbUserIniName);
	}
}

/*
// My version of access().
// This now returns a CFileException enum value (which may be 'none').
*/

static long AccessFile(LPCSTR pName, int mode)
{
	long lRet;

#ifdef _WIN32
	WIN32_FIND_DATA ffd;
	HANDLE hSearch;

	if ((hSearch = FindFirstFile(pName, &ffd)) == INVALID_HANDLE_VALUE)
	{
		lRet = GetLastError();
	}
	else
	{
		lRet = NO_ERROR;
		if ((mode & 2) && (ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
		{
		/* We want to be able to write to it but we can't. */
			lRet = ERROR_ACCESS_DENIED;
		}
		FindClose(hSearch);
	}
#else	
	struct _find_t buffer;

	/* Preload the struct. */

	lstrcpy(buffer.name, "Nope!");
	buffer.attrib = -1;
	buffer.size = -1L;

	int nAccess = _A_NORMAL;

	if (!(mode & 2))
	{
	/* Doesn't require write permission. */
		nAccess |= _A_RDONLY;
	}

	lRet = (long)_dos_findfirst(pName, nAccess, &buffer);

	if (lRet == 0x12)
	{
	/* "No more files" == "file not found" */
		lRet = 2;
	}

	if (lRet == 0)
	{
	/*
	// Let's be REALLY sure we found the file, that this is not just a bogus
	// return value (it happens!).
	// We compare the name in the find buffer to the base name of what
	// we were looking for.
	*/
		int nNameLength = lstrlen(pName);
		int nBufferLength = lstrlen(buffer.name);

		if (nNameLength < nBufferLength
				|| stricmp(pName + nNameLength - nBufferLength, buffer.name) != 0)
		{
		/* Names don't match! We didn't really find the file. */
			lRet = 2;		// Not found.
		}
	}
#endif

	return lRet;
}

/*
// Does a file exist?
*/

static BOOL FileExists(LPCSTR pFileName)
{
	UINT uPrevSem = ::SetErrorMode(SEM_FAILCRITICALERRORS);

	BOOL fExists = AccessFile(pFileName) == 0;

	::SetErrorMode(uPrevSem);

	return fExists;
}

/*
// Is the file pointed to by m_cbDllName our DLL?
*/

static BOOL IsOurHome(void)
{
/*
// Does the DLL exist?
*/
	if (!FileExists(m_cbDllName))
	{
		return FALSE;
	}
/*
// Build a path to a possible MSREG.INI in that directory for verification.
*/
	char cbMsregName[_MAX_PATH];
	lstrcpy(cbMsregName, m_cbDllPath);
	AppendSlashToPath(cbMsregName);
	lstrcat(cbMsregName, "MSREG.INI");

	char cbAppEntry[_MAX_PATH];

/*
// It's OK if there is no such entry OR the entry is our name.
*/

	if (GetPrivateProfileString(MSREGINI_Section_Configuration,
										 MSREGINI_Configuration_Application,
										 "", cbAppEntry, sizeof(cbAppEntry)-1,
										 cbMsregName) > 0)
	{
		if (lstrcmpi(cbAppEntry, m_cbAppName) != 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL LocateDllFile(void)
{
	CDriveManager DriveManager;

	if (DriveManager.Open())
	{
		DriveManager.InitDrives();

		//
		// Look in the current directory first.
		//

		SetDllPathName(m_cbWorkingDirectory, FALSE);

		if (IsOurHome())
		{
			return TRUE;
		}

		//
		// Look relative to the current directory first.
		//

		SetDllPathName(m_cbWorkingDirectory, TRUE);

		if (IsOurHome())
		{
			return TRUE;
		}

		//
		// Look in root directories of drives.
		//

		SetDllPathName("?:\\", TRUE);

		for (;;)
		{
		/*
		// We need to search in a number of places for the home CD.
		*/

		/* Search on all CD-ROM drives. */

			char& cDrive = m_cbDllName[0];			// Cheat a little...

			for (cDrive = 'A'; cDrive <= 'Z'; cDrive++)
			{
				DriveType Type = DriveManager.GetDriveType(cDrive);
				m_cbDllPath[0] = cDrive;

				if (Type == DT_CDROM && IsOurHome())
				{
					return TRUE;
				}
			}

		/* Search on all remote drives. */

			for (cDrive = 'A'; cDrive <= 'Z'; cDrive++)
			{
				DriveType Type = DriveManager.GetDriveType(cDrive);
				m_cbDllPath[0] = cDrive;

				if (Type == DT_Remote && IsOurHome())
				{
					return TRUE;
				}
			}

		/* Search on all non-remote, non-CD >= 'C'. */

			for (cDrive = 'C'; cDrive <= 'Z'; cDrive++)
			{
				DriveType Type = DriveManager.GetDriveType(cDrive);
				m_cbDllPath[0] = cDrive;

				if (Type != DT_CDROM && Type != DT_Remote && IsOurHome())
				{
					return TRUE;
				}
			}

		/*
		// The volume could not be found. Ask the user to put it in a drive.
		*/

			if (!PromptForHomeCD())
			{
			/* User said forget it. */
				break;
			}
		}
		DriveManager.Close();
	}
	return FALSE;
}

/*
// Ask the user to put in the home CD.
*/

static BOOL PromptForHomeCD(void)
{
//	od("Prompt for home CD volume\r\n");

/* Build the message string. */

	static char cbMessage[_MAX_PATH];

	lstrcpy(cbMessage, "Please insert ");
	lstrcat(cbMessage, m_cbCD);
	lstrcat(cbMessage,"\n"
							"into any drive, and press OK.\n"
							"\n"
							"Press Cancel if the disk is not available");

/* Put up the message box. */

	if (::MessageBox(NULL,
						  cbMessage,
						  "Insert CD",
						  MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
	{
		return TRUE;
	}

/* Otherwise, just cancel for now. */

	return FALSE;
}

#ifndef NO_FREE_BONUS
/////////////////////////////////////////////////////////////////
//
// Key/CRC stuff from KEY.CPP
//
// This code is mirrored here to avoid include file hassles.
//

//*
//* This is the lookup table used when performing the CRC calculation.
//*

static unsigned long ccitt_32[ 256 ] =
{
0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};  

//*
//* unsigned long CalculateBlock( unsigned char *buf, unsigned int cout, unsigned long crc )
//*
//* ARGUMENTS
//*   unsigned char *buf - The data to preform the CRC on.
//*   int count - The number of bytes in the buffer.
//*   unsigned long crc - The previous CRC
//*
//* DESCRIPTION
//*   Computes the 32 bit CRC of the data in a buffer.
//*
//* SIDE EFFECTS
//*
//* RETURNS
//*   A 32 bit CRC for the supplied buffer.
//*

static unsigned long CalculateBlock( const unsigned char *buffer, unsigned int count, unsigned long crc )
{
  const unsigned char *p = buffer;
  unsigned long temp1;
  unsigned long temp2;

  //*
  //* Calculate the 32 bit CRC using the table lookup method.
  //*
  while ( count-- != 0 ) {
    temp1 = ( crc >> 8 ) & 0x00FFFFFFL;
    temp2 = ccitt_32[ ( (int) crc ^ *p++ ) & 0xff ];
    crc = temp1 ^ temp2;
  }
  return( crc );
}

static unsigned long KeyFromString(LPCSTR pString)
{
	return CalculateBlock((const unsigned char*)pString, lstrlen(pString), 0x31415265L);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Library init and WEP

#ifdef WIN32
extern "C" BOOL WINAPI
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
	// Like LibMain.
		m_hInstance = hInstance;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
	// Like WEP.
	}

	return TRUE;    // ok
}
#else
extern "C"
int CALLBACK LibMain(HINSTANCE hInstance, WORD, WORD, LPSTR)
{
	m_hInstance = hInstance;
	return 1;   // ok
}
#endif
