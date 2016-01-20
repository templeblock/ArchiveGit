/*
// $Workfile: INSTALL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/INSTALL.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.11   16 Dec 1995 08:15:58   FRED
// Keeps dwInstallFinished on stack
// 
//    Rev 1.10   15 Dec 1995 16:16:36   FRED
// Fixes to RunInstall().
// 
// 
//    Rev 1.9   15 Dec 1995 14:46:24   FRED
// Does not delete window, CFrameWnd deletes itself.
// 
// 
//    Rev 1.8   14 Dec 1995 16:19:58   FRED
// If no window is passed in, RunInstall() creates a
// dummy frame window to be sure that is gets the
// broadcast message from MSFREE.
// 
// 
//    Rev 1.7   14 Dec 1995 13:46:50   FRED
// Fixed bug where FREE Fonts icon would not launch
// if user was bonus enabled.
// 
// 
//    Rev 1.6   14 Dec 1995 11:17:12   FRED
// Fixed warning
// 
//    Rev 1.5   12 Dec 1995 13:28:34   FRED
// Uses message to synch with free bonus installer
// 
//    Rev 1.4   08 Dec 1995 13:14:30   FRED
// Uses top-level window without parent for termination test
// 
//    Rev 1.3   30 Nov 1995 09:26:30   JAY
// Now re-activates the window we are re-enabling.
// 
//    Rev 1.2   22 Nov 1995 16:01:08   FRED
//  
// 
//    Rev 1.1   22 Nov 1995 14:02:48   FRED
// Only enable/disables parent window if not null
// 
//    Rev 1.0   22 Nov 1995 13:34:42   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "install.h"
#include "util.h"

#define	INSTALL_FINISHED_MESSAGE	"InstallFinishedMessage"

static UINT msgInstallFinished = RegisterWindowMessage(INSTALL_FINISHED_MESSAGE);

/*
// Pump the next message.
*/

static BOOL PumpMessage(DWORD dwInstallFinished)
{
/* Pump the message. */
	MSG Msg;

	if (GetMessage(&Msg, NULL, 0, 0))
	{
		if ((Msg.message == msgInstallFinished)
		 && (Msg.lParam == (LPARAM)dwInstallFinished))
		{
			return FALSE;
		}

		if (Msg.hwnd != NULL)
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		return TRUE;
	}
	else
	{
	/*
	// GetMessage() returned FALSE.
	// That means a WM_QUIT is pending.
	// Just break out. We don't care whether it finished or not.
	// To make sure somebody gets this, though, we post it again.
	*/
		PostQuitMessage(0);
		return FALSE;
	}
}

static BOOL RunInstall(HWND hWnd, LPCSTR pszInstall, DWORD dwInstallFinished)
{
	CFrameWnd* pWnd = NULL;

	// Allocate a window if we don't have one.
	if (hWnd == NULL)
	{
		pWnd = new CFrameWnd;
		pWnd->Create(NULL, "");
	}

	// Try to exec MSREG.EXE
	UINT uiResult = WinExec(pszInstall, SW_SHOWNORMAL);
	if (uiResult >= 32)
	{
		// Wait around for the app to finish.
		while (PumpMessage(dwInstallFinished)) ;
	}

	// Free up window if one was allocated.
	if (pWnd != NULL)
	{
		pWnd->DestroyWindow();	// CFrameWnd deletes itself
		pWnd = NULL;
	}

	return uiResult >= 32;
}

CString GetProgramName(LPCSTR pszDirectory)
{
	CString csName;

	// Attempt to determine the name of the free bonus installation program:
	//
	//	1) Try Directory\MSFREEnn.EXE first where nn is the native "bitness"
	//		of this application.
	//
	// 2) Try generic Directory\MSFREE.EXE.

	// Try name with our same "bitness".
	csName = pszDirectory;
	Util::AppendBackslashToPath(csName);
#ifdef WIN32
	csName += "msfree32.exe";
#else
	csName += "msfree16.exe";
#endif

	if (!Util::FileExists(csName))
	{
		// Try generic name.
		csName = pszDirectory;
		csName += "msfree.exe";

		if (!Util::FileExists(csName))
		{
			csName.Empty();
		}
	}

	return csName;
}

BOOL InstallBonus(HWND hWnd, LPCSTR pszDirectory)
{
	BOOL fResult = FALSE;

	CString csName;

	TRY
	{
		csName = GetProgramName(pszDirectory);
		if (!csName.IsEmpty())
		{
			if (hWnd != NULL)
			{
				EnableWindow(hWnd, FALSE);
			}

			// M=<Message Name>
			csName += " /M=" INSTALL_FINISHED_MESSAGE;

			// D=<Message Data>
			DWORD dwInstallFinished = GetTickCount() & 0x7ffffffL;
			csName += " /D=";
			char szMessageData[40];
			ltoa(dwInstallFinished, szMessageData, 10);
			csName += szMessageData;

			RunInstall(hWnd, csName, dwInstallFinished);

			if (hWnd != NULL)
			{
				EnableWindow(hWnd, TRUE);
				BringWindowToTop(hWnd);
				SetActiveWindow(hWnd);
			}
		}
	}
	CATCH_ALL(e)
	{
		csName.Empty();
		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}
