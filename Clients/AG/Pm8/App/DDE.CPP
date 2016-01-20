/*
// $Workfile: DDE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/DDE.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:04   Fred
// Initial revision.
// 
//    Rev 1.3   11 Sep 1996 13:01:06   Fred
// pmAddItem do takes arguments for xpos, ypos, and default directory
// 
//    Rev 1.2   30 Aug 1996 19:33:24   Fred
// Works with UNC and long file names
// 
//    Rev 1.1   23 Apr 1996 08:25:20   Jay
//  
// 
//    Rev 2.7   19 Apr 1996 15:50:00   FRED
// Removes old group
// 
//    Rev 2.6   12 Feb 1996 10:36:32   JAY
// Upgrade Icons
// 
//    Rev 2.5   12 Feb 1996 08:40:06   JAY
// Added param to pmOpen.
// 
//    Rev 2.4   17 Oct 1995 22:01:58   FRED
//  
// 
//    Rev 2.3   17 Oct 1995 15:42:26   JAY
// Fixes
// 
//    Rev 2.2   13 Oct 1995 14:19:58   FRED
// 32 bit changes
// 
//    Rev 2.1   08 Feb 1995 13:36:12   JAY
// Reverted. New series.
// 
//    Rev 1.0   17 Jun 1994 14:58:40   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include <dde.h>
#include <string.h>

#define	MAX_DDE_STRING_LENGTH	(1024)

char	szProgman[]	= "PROGMAN";
HWND	hWndDDE		= NULL;
HWND	hWndServer	= NULL;  	// Currently active DDE conversation
BOOL	fInitiate	= FALSE;
BOOL	fAck			= FALSE;
HWND	hWndProgman	= NULL;		// global handle of progman window
HGLOBAL hData = NULL;

HWND pmOpen(LPCSTR szName, BOOL fDeleteGroup);
BOOL pmClose(LPCSTR szName);
BOOL pmDeleteGroup(LPCSTR lpszName);
BOOL pmReplaceItem(LPCSTR lpszName);
BOOL pmAddItem(LPCSTR szName, LPCSTR szCmd, LPCSTR szIconName, WORD wIconNumber);
BOOL pmDeleteItem(LPCSTR lpszName);
BOOL pmGetGroupData(CString& csResult, LPCSTR pszGroupName = NULL);

BOOL ddeInit(HINSTANCE hInst, HINSTANCE hPrevInst);
void ddeShutdown(void);
#ifdef WIN32
LRESULT FAR PASCAL ddeWindowProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);
#else
LRESULT FAR PASCAL _export ddeWindowProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);
#endif

static HWND ddeInitiate(LPCSTR szApplication, LPCSTR szTopic);
static BOOL ddeTerminate(HWND hWnd);
static HWND ddeSendInitiate(ATOM aApplication, ATOM aTopic);
static BOOL ddeWait(HWND hWnd);
static BOOL ddeExec(HWND hWnd, LPCSTR szCmd);
static BOOL ddeRequest(HWND hWnd, LPCSTR szItem);

/*
 * pmOpen()		- Open a existing ProgMan Group File -- or create a new one.
 *
 *		szName			- Name of the group to create
 *		fDeleteGroup	- Whether to delete the group first
 *
 * RETURNS: progman DDE window handle
 *
 */

HWND pmOpen(LPCSTR szName, BOOL fDeleteGroup)
{
	char buf[MAX_DDE_STRING_LENGTH];

	if (hWndProgman == NULL)
	{
		if ((hWndProgman = ddeInitiate(szProgman, szProgman)) == NULL)
		{
			return NULL;
		}

		EnableWindow(hWndProgman, FALSE);
	}

	if (fDeleteGroup)
	{
		wsprintf(buf, "[DeleteGroup(%s)]", szName);
		ddeExec(hWndProgman, buf);
	}

	wsprintf(buf, "[CreateGroup(%s)]", szName);
	ddeExec(hWndProgman, buf);

	return hWndProgman;
}


/*
 * pmClose()	- Close a Group file opened with pmOpen()
 *					  The header will be updated to reflect any canges made.
 *
 *		szName	- Name of the group to close
 *
 * RETURNS: TRUE if successful FALSE otherwise
 *
 */

BOOL pmClose(LPCSTR /*szName*/)
{
	if (hWndProgman != NULL)
	{
		EnableWindow(hWndProgman, TRUE);

		ddeExec(hWndProgman, "[ExitProgman(1)]");

		hWndServer = hWndProgman = NULL;
	}

	return TRUE;
}


/*
// pmDeleteGroup()
//
//		Delete an existing Program Manager group
*/

BOOL pmDeleteGroup(LPCSTR lpszName)
{
	char buf[MAX_DDE_STRING_LENGTH];

	if (hWndProgman == NULL)
	{
		if ((hWndProgman = ddeInitiate(szProgman, szProgman)) == 0)
		{
			return FALSE;
		}

		EnableWindow(hWndProgman, FALSE);
	}

	wsprintf(buf, "[DeleteGroup(%s)]", lpszName);
	ddeExec(hWndProgman, buf);

	EnableWindow(hWndProgman, TRUE);
	ddeExec(hWndProgman, "[ExitProgman(1)]");
	hWndServer = hWndProgman = NULL;

	return TRUE;
}


/*
// pmReplaceItem()
//
//		Replace an existing Program Manager item
*/

BOOL pmReplaceItem(LPCSTR lpszName)
{
	char buf[MAX_DDE_STRING_LENGTH];

	wsprintf(buf, "[ReplaceItem(%s)]", lpszName);
	return ddeExec(hWndProgman, buf);
}

/*
 * pmAddItem() - Append a new item to a Group file opened with pmOpen()
 *
 *		szName		- Name of item to add
 *		szCmd			- Item's Command line
 *		szIconName	- Name of executable containing icon
 *		wIconNumber	- Index number of icon
 *
 * RETURNS: TRUE if successful FALSE otherwise
 *
 */

BOOL pmAddItem(LPCSTR szName, LPCSTR szCmd, LPCSTR szIconName, WORD wIconNumber, int nXPos /*=-1*/, int nYPos /*=-1*/, LPCSTR pszDefaultDirectory /*=NULL*/)
{
	char buf[MAX_DDE_STRING_LENGTH];

	if (pszDefaultDirectory == NULL)
	{
		pszDefaultDirectory = "";
	}

	wsprintf(buf, "[AddItem(%s,%s,%s,%d,%d,%d,%s)]",
		szCmd, szName, (szIconName == NULL ? szCmd : szIconName), wIconNumber, nXPos, nYPos, pszDefaultDirectory);

	return ddeExec(hWndProgman, buf);
}

BOOL pmDeleteItem(LPCSTR lpszName)
{
	char buf[MAX_DDE_STRING_LENGTH];

	wsprintf(buf, "[DeleteItem(%s)]", lpszName);
	return ddeExec(hWndProgman, buf);
}

BOOL pmGetGroupData(CString& csResult, LPCSTR pszGroupName /*=NULL*/)
{
	BOOL fResult = FALSE;
	
	csResult.Empty();
	
	hData = NULL;
	
	if (!hWndProgman)
	{
		if ((hWndProgman = ddeInitiate(szProgman, szProgman)) == 0)
		{
			return FALSE;
		}

		EnableWindow(hWndProgman, FALSE);
	}
	
	if (pszGroupName == NULL)
	{
		pszGroupName = "GROUPS";	// Get all group names
	}
	
	ddeRequest(hWndProgman, pszGroupName);
	
	if (hData != NULL)
	{
		DDEDATA* pData = (DDEDATA*)GlobalLock(hData);
		if (pData != NULL)
		{
			TRY
			{
				csResult = (LPCSTR)(pData->Value);
				fResult = TRUE;
			}
			END_TRY
			
			GlobalUnlock(hData);
			pData = NULL;
		}
		
		GlobalFree(hData);
		hData = NULL;
	}
	
	EnableWindow(hWndProgman, TRUE);
	ddeExec(hWndProgman, "[ExitProgman(1)]");
	hWndServer = hWndProgman = NULL;
	
	return fResult;
}

/*
// ddeInit(hInst, hPrevInst)
//
// Description:
//
//		This is called when the application is first loaded into
//		memory.  It performs all initialization that doesn't need
//		to be done once per instance.
//
// Arguments:
//
//		hInst			-	instance handle of current instance
//		hPrevInst	-	instance handle of previous instance
//
// Returns:
//
//		TRUE if successful, FALSE if not
*/

BOOL ddeInit(HINSTANCE hInst, HINSTANCE hPrevInst)
{
	WNDCLASS rClass;

	if (hPrevInst == NULL)
	{
		rClass.hCursor			= NULL;
		rClass.hIcon			= NULL;
		rClass.lpszMenuName	= NULL;
		rClass.lpszClassName	= "ddeClass";
		rClass.hbrBackground	= NULL;
		rClass.hInstance		= hInst;
		rClass.style			= 0;
		rClass.lpfnWndProc	= (WNDPROC)ddeWindowProc;
		rClass.cbClsExtra		= 0;
		rClass.cbWndExtra		= 0;

		if (!RegisterClass(&rClass))
		{	
			return FALSE;
		}
	}

	/*
	// Create a window to handle our DDE mesage traffic.
	*/

	hWndDDE = CreateWindow("ddeClass", NULL, 0L, 0, 0, 0, 0,
							NULL,		/* no parent */
							NULL,		/* use class menu */
							hInst,	/* handle to window instance */
							NULL);	/* no params to pass on */

	return (BOOL)hWndDDE;
}


/*
// ddeShutdown()
//
// Description:
//
//		This is called when the application is terminating.
//		It cleans up any objects including destroying the
//		window created for DDE communication.
*/

void ddeShutdown(void)
{
	if (hWndDDE != NULL)
	{
		DestroyWindow(hWndDDE);
		hWndDDE = NULL;
	}
}

/*
// ddeWindowProc(hWnd, wMessage, wParam, lParam)
//
// Description:
//
// Arguments:
//
//		hWnd			window handle for the parent window
//		wMessage		message number
//		wParam		message-dependent
//		lParam		message-dependent
//
// Returns:
//
//		0 if processed, nonzero if ignored
*/

#ifdef WIN32
LRESULT FAR PASCAL ddeWindowProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
	switch (wMessage)
	{
		case WM_DDE_TERMINATE:
		{
			ddeTerminate((HWND)wParam);
			return 0;
		}

		case WM_DDE_ACK:
		{
			if (fInitiate)
			{
				// From WM_DDE_INITIATE
				hWndServer = (HWND)wParam;
				GlobalDeleteAtom(LOWORD(lParam));
				GlobalDeleteAtom(HIWORD(lParam));
			}
			
			else
			{
				// From WM_DDE_EXECUTE and other messages
				UINT uLow;
				UINT uHigh;
				UnpackDDElParam(wMessage, lParam, &uLow, &uHigh);
				FreeDDElParam(wMessage, lParam);
				fAck = ((DDEACK*)(&uLow))->fAck;
			}
			return 0;
		}

		case WM_DDE_DATA:
		{
			UINT uLow;
			UINT uHigh;
			UnpackDDElParam(wMessage, lParam, &uLow, &uHigh);
			FreeDDElParam(wMessage, lParam);
			hData = (HGLOBAL)uLow;
			GlobalDeleteAtom((ATOM)uHigh);

#if 0
			if (hData != NULL)
			{
				DDEDATA* pData = (DDEDATA*)GlobalLock(hData);
				if (pData != NULL)
				{
					unsigned short fRelease = pData->fRelease;
					unsigned short fAckReq = pData->fAckReq;
					GlobalUnlock(hData);
					if (!fRelease || fAckReq)
					{
						AfxMessageBox("Ugh!");
					}
				}
			}
#endif

			return 0;
		}
	}

	return DefWindowProc(hWnd, wMessage, wParam, lParam);
}
#else
LRESULT FAR PASCAL _export ddeWindowProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
	switch (wMessage)
	{
		case WM_DDE_TERMINATE:
		{
			ddeTerminate((HWND)wParam);
			return 0;
		}

		case WM_DDE_ACK:
		{
			if (fInitiate)
			{
				hWndServer = (HWND)wParam;
				GlobalDeleteAtom(LOWORD(lParam));
				GlobalDeleteAtom(HIWORD(lParam));
			}
			
			else
			{
				fAck = (LOWORD(lParam) & 0x8000);
			}

			return 0;
		}

		case WM_DDE_DATA:
		{
			hData = (HGLOBAL)LOWORD(lParam);
			GlobalDeleteAtom(HIWORD(lParam));

#if 0
			if (hData != NULL)
			{
				DDEDATA* pData = (DDEDATA*)GlobalLock(hData);
				if (pData != NULL)
				{
					unsigned short fRelease = pData->fRelease;
					unsigned short fAckReq = pData->fAckReq;
					GlobalUnlock(hData);
					if (!fRelease || fAckReq)
					{
						AfxMessageBox("Ugh!");
					}
				}
			}
#endif

			return 0;
		}
	}

	return DefWindowProc(hWnd, wMessage, wParam, lParam);
}
#endif

static HWND ddeInitiate(LPCSTR szApplication, LPCSTR szTopic)
{
	ATOM aApplication;
	ATOM aTopic;
	HWND hWnd;

	TRACE("ddeInitiate - Application %Fs, Topic %Fs\n", szApplication, szTopic);

	aApplication = GlobalAddAtom(szApplication);
	aTopic       = GlobalAddAtom(szTopic);

	/*
	// Try to start a conversation with the requested application.
	*/

	if ((hWnd = ddeSendInitiate(aApplication, aTopic)) == NULL)
	{
		/*
		// Perhaps the application is not running, try to EXEC it.
		*/

		if (WinExec(szApplication, SW_SHOWNORMAL) < 32)
		{
			return NULL;
		}

		hWnd = ddeSendInitiate(aApplication, aTopic);
	}

	GlobalDeleteAtom(aApplication);
	GlobalDeleteAtom(aTopic);

	return hWnd;
}


static BOOL ddeTerminate(HWND hWnd)
{
	TRACE("ddeTerminate\n");

#ifdef WIN32
	PostMessage(hWnd, WM_DDE_TERMINATE, (WPARAM)hWndDDE, 0);
#else
	PostMessage(hWnd, WM_DDE_TERMINATE, (WORD)hWndDDE, 0L);
#endif

	if (hWnd == hWndServer)
	{
		hWndServer = NULL;
	}

	return TRUE;
}


static HWND ddeSendInitiate(ATOM aApplication, ATOM aTopic)
{
	fInitiate = TRUE;

	SendMessage((HWND)(-1), WM_DDE_INITIATE, (WPARAM)hWndDDE, MAKELONG(aApplication, aTopic));

	fInitiate = FALSE;

	return hWndServer;
}


static BOOL ddeWait(HWND hWnd)
{
	MSG    rMsg;
	BOOL   fResult;
	
	fResult = FALSE;

	TRACE("ddeWait");

#ifndef WIN32
	LockData(0);
#endif

	while (GetMessage(&rMsg, NULL, WM_DDE_FIRST, WM_DDE_LAST))
	{
		TranslateMessage(&rMsg);
		DispatchMessage(&rMsg);

		if (rMsg.wParam == (WPARAM)hWnd)	/* DDE message from proper window */
		{
			switch (rMsg.message)
			{
				case WM_DDE_ACK:
				{
					fResult = fAck;
					goto exit;
				}

				case WM_DDE_DATA:
				{
					fResult = TRUE;
					goto exit;
				}
			}
		}
	}

exit:

#ifndef WIN32
	UnlockData(0);
#endif

	TRACE("\n");

	return fResult;
}


static BOOL ddeExec(HWND hWnd, LPCSTR szCmd)
{
	HGLOBAL hCmd;
	BOOL   wResult;

	TRACE("ddeExec:%Fs\n", szCmd);

	wResult = FALSE;

	if ((hCmd = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, (LONG)lstrlen(szCmd)+1)) != NULL)
	{
		LPSTR lpCmd;

		if ((lpCmd = (LPSTR)GlobalLock(hCmd)) != NULL)
		{
			lstrcpy(lpCmd, szCmd);

			GlobalUnlock(hCmd);

#ifdef WIN32
			PostMessage(hWnd, WM_DDE_EXECUTE, (WPARAM)hWndDDE, (LPARAM)hCmd);
#else
			PostMessage(hWnd, WM_DDE_EXECUTE, (WORD)hWndDDE, MAKELONG(NULL, hCmd));
#endif

			wResult = ddeWait(hWnd);
		}

		GlobalFree(hCmd);
	}

	return wResult;
}

static BOOL ddeRequest(HWND hWnd, LPCSTR szItem)
{
	BOOL   wResult = FALSE;

	TRACE("ddeRequest:%Fs\n", szItem);

	hData = NULL;
	
	ATOM aItem = GlobalAddAtom(szItem);
	if (aItem != NULL)
	{
		PostMessage(hWnd, WM_DDE_REQUEST, (WPARAM)hWndDDE, MAKELPARAM(CF_TEXT, aItem));

		wResult = ddeWait(hWnd);
	}

	return hData != NULL;
}
