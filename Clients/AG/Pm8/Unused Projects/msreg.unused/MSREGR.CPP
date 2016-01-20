/*
// DLL testing program.
//
// $Workfile: MSREGR.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSREGR.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.12   22 Mar 1996 16:56:36   FRED
// Window title
// 
//    Rev 1.11   14 Mar 1996 15:49:58   JAY
// Now calls SetForegroundWindow() under 32-bit build.
// 
//    Rev 1.10   12 Mar 1996 17:24:04   JAY
// Fixed last two revs.
// 
//    Rev 1.9   12 Mar 1996 17:14:04   JAY
// Took out the last change.
// 
//    Rev 1.8   12 Mar 1996 16:07:48   JAY
// 
//    Rev 1.7   12 Mar 1996 12:45:08   JAY
// Now looks for a previous instance (via the window class and title).
// 
//    Rev 1.6   27 Dec 1995 16:26:12   FRED
// Switched back to WS_OVERLAPPED
// 
//    Rev 1.5   27 Dec 1995 15:43:02   FRED
// MSREGR
// 
//    Rev 1.4   21 Dec 1995 15:45:58   FRED
// Fixed bug with no icon in 16 bit MSRUN
// 
//    Rev 1.3   19 Dec 1995 13:09:44   FRED
// Uses Icon
// 
//    Rev 1.2   14 Dec 1995 17:56:40   FRED
// Dummy setarg and setenv
// 
//    Rev 1.1   22 Nov 1995 14:02:06   FRED
// Added a space before register options
// 
//    Rev 1.0   22 Nov 1995 13:07:16   JAY
// Initial revision.
*/

#include "msregrrc.h"
#include "msreg.h"
#include "msregent.h"
#include "msregini.h"
#include <stdlib.h>
#include <string.h>

HINSTANCE m_hInstance;
char m_szWorkingDirectory[_MAX_PATH];
char m_szMsregIni[_MAX_PATH];
char m_szWindowTitle[_MAX_PATH];

static void SetWorkingVariables(void);

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
// Main entry point.
*/

int PASCAL far WinMain(HINSTANCE hInstance,
							  HINSTANCE hPrevInstance,
							  LPSTR lpCmdLine,
							  int nCmdShow)
{
	REGRESULT Result = REGRESULT_Error;
	
/*
// Establish our global environment.
*/

	m_hInstance = hInstance;

/*
// Compute our working variables.
*/

	SetWorkingVariables();

/*
// See if a previous instance is running.
// We use this generic method since hPrevInstance is unusable under 32 bits.
// We need the window anyway, so we just do it.
//
// If we find a window and it has a last active popup, we use it.
// If there is no window, then we just proceed on, being the first.
// If there is a window, but it has no last active popup, then the previous
// instance has spawned its MSFREE component and is waiting. We pass control
// down again to allow the code to bring THAT window to the front.
*/

	HWND hPrevWnd;

	if ((hPrevWnd = FindWindow("MSREGR", m_szWindowTitle)) != NULL)
	{
		HWND hWndLastActive = GetLastActivePopup(hPrevWnd);

		if ((hWndLastActive != NULL) && (hWndLastActive != hPrevWnd))
		{
			if (IsIconic(hPrevWnd))
			{
				ShowWindow(hPrevWnd, SW_SHOWNORMAL);
			}
			else
			{
#ifdef WIN32
				SetForegroundWindow(hPrevWnd);
#endif
				BringWindowToTop(hPrevWnd);
#ifdef WIN32
				SetForegroundWindow(hWndLastActive);
#endif
				BringWindowToTop(hWndLastActive);
			}
			return 0;
		}
	}

/*
// Give ourselves a window so we can show an icon.
*/

	if (hPrevInstance == NULL)
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = DefWindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hInstance;
		wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_MSRUN));
		wc.hCursor = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "MSREGR";
		
		RegisterClass(&wc);
	}

	HWND hWnd = NULL;
	
	hWnd = CreateWindow(
		"MSREGR",
		m_szWindowTitle,
		WS_OVERLAPPED,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		HWND_DESKTOP,
		NULL,
		m_hInstance,
		NULL);
		
/*
// Load the library.
*/

	char cbLibraryName[_MAX_PATH];

	lstrcpy(cbLibraryName, m_szWorkingDirectory);
#ifdef WIN32
	lstrcat(cbLibraryName, "MSREG32.DLL");
#else
	lstrcat(cbLibraryName, "MSREG16.DLL");
#endif

	UINT wOldSem = SetErrorMode(SEM_NOOPENFILEERRORBOX);
	HINSTANCE hLibrary = LoadLibrary(cbLibraryName);
	SetErrorMode(wOldSem);

#ifndef WIN32
	if ((UINT)hLibrary < (UINT)HINSTANCE_ERROR)
	{
		hLibrary = NULL;
	}
#endif

	if (hLibrary != NULL)
	{
	/*
	// Get the entry point.
	*/

#ifdef WIN32
		FN_REGSENDCOMMAND pRegSendCommand = (FN_REGSENDCOMMAND)GetProcAddress(hLibrary, "RegSendCommand");
#else
		FN_REGSENDCOMMAND pRegSendCommand = (FN_REGSENDCOMMAND)GetProcAddress(hLibrary, "REGSENDCOMMAND");
#endif

		if (pRegSendCommand != NULL)
		{
			Result = pRegSendCommand(hWnd, REGCOMMAND_Register " /C /A", 0, NULL);
		}

		FreeLibrary(hLibrary);
	}

   if (hWnd != NULL)
   {
		DestroyWindow(hWnd);
	}

	return (int)Result;
}

static void SetWorkingVariables(void)
{
/*
// Setup the working directory.
*/

	::GetModuleFileName(m_hInstance, m_szWorkingDirectory, sizeof(m_szWorkingDirectory)-1);

/* Skip the drive. */

	char* ptr;
	if ((ptr = strchr(m_szWorkingDirectory, ':')) == NULL)
	{
		ptr = m_szWorkingDirectory;
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

/* Make sure this is terminated correctly. */

	int length = lstrlen(m_szWorkingDirectory);
	if (length != 0 && m_szWorkingDirectory[length-1] != '\\' && m_szWorkingDirectory[length-1] != ':')
	{
		m_szWorkingDirectory[length++] = '\\';
		m_szWorkingDirectory[length] = '\0';
	}

/* Get the full name of the MSREG.INI file. */
	
	lstrcpy(m_szMsregIni, m_szWorkingDirectory);
	lstrcat(m_szMsregIni, "MSREG.INI");
	
/* Get the title to use for the main window. */

	char szBonusShort[_MAX_PATH];

	GetPrivateProfileString(
		MSREGINI_Section_Configuration,
		"BonusShort",
		"",
		szBonusShort,
		sizeof(szBonusShort)-1,
		m_szMsregIni);

	if (strlen(szBonusShort) != 0)
	{
		strcpy(m_szWindowTitle, "Register for ");
		strcat(m_szWindowTitle, szBonusShort);
		strcat(m_szWindowTitle, "!");
	}
	else
	{
		strcpy(m_szWindowTitle, "Register!");
	}
}

