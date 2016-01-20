/*
// $Workfile: INSTFORK.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/INSTFORK.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:33:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:54   FRED
// Initial revision.
// 
//    Rev 1.1   26 Jan 1996 08:27:46   FRED
// Added Autoplay support to installer.
// 
//    Rev 1.0   15 Jan 1996 12:34:44   JAY
// Initial revision.
*/

/*
// This is the dual installation system's fork point (ouch!).
// This determines whether it is running on a 16 or 32-bit system and
// spawns the appopriate installation program.
*/

#include <windows.h>
#include <stdlib.h>
#include <string.h>

static HINSTANCE m_hInstance = NULL;
char m_szWorkingDirectory[_MAX_PATH];
char m_szExecPath[_MAX_PATH];

static void AppendSlashToPath(LPSTR pszPath);
static BOOL SetWorkingVariables(LPCSTR pszCommandLine);
static BOOL IsWindows95(void);
static BOOL Is32BitEnvironment(void);
static int RunInstall(BOOL f32Bit, LPCSTR pszCommandLine);

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
		nResult = RunInstall(Is32BitEnvironment(), pszCommandLine);
	}
	return nResult;
}

static int RunInstall(BOOL f32Bit, LPCSTR pszCommandLine)
{
	int nResult = 0;

	lstrcpy(m_szExecPath, m_szWorkingDirectory);
	AppendSlashToPath(m_szExecPath);
	strcat(m_szExecPath, "INSTALL\\INST");
	strcat(m_szExecPath, f32Bit ? "32" : "16");
	strcat(m_szExecPath, ".EXE ");
	strcat(m_szExecPath, pszCommandLine);

	UINT uReturn = WinExec(m_szExecPath, SW_SHOW);

	if (uReturn < 32)
	{
		char szMessage[256];
		wsprintf(szMessage, "Error %u launching %s.", uReturn, m_szExecPath);
		MessageBox(NULL, szMessage, NULL, MB_OK);
	}
	else
	{
		nResult = 1;
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
	   
	return TRUE;
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

static BOOL IsWindowsNT(void)
{
	return (GetWinFlags() & 0x4000) != 0;				// WF_WINNT
}

static BOOL IsWindows95(void)
{
	WORD wWinVer;

	wWinVer = LOWORD(GetVersion());
	wWinVer = (((WORD)(LOBYTE(wWinVer)))<<8)|((WORD)(HIBYTE(wWinVer)));

	return (wWinVer >= 0x35f);
}

static BOOL Is32BitEnvironment(void)
{
	return IsWindowsNT() || IsWindows95();
}

