/*
// $Workfile: UPGDLL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGDLL.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 3     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     1/21/98 4:25p Fredf
// Year 2000 compliance.
// 
//    Rev 1.0   14 Aug 1997 15:26:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:54   Fred
// Initial revision.
// 
//    Rev 1.3   12 Jun 1997 16:46:58   Fred
// Changes for 4.0 instant upgrade
// 
//    Rev 1.2   07 May 1996 12:45:10   Jay
// From PMW2
// 
//    Rev 1.4   24 Apr 1996 16:44:46   JAY
// New demo support
// 
//    Rev 1.3   28 Mar 1996 10:51:32   FRED
//  
// 
//    Rev 1.2   27 Mar 1996 14:59:58   JAY
// Fixed upgrade parent window problem
// 
//    Rev 1.1   12 Feb 1996 14:16:30   JAY
// Added ProductLevel param to Upgrade() call.
// 
//    Rev 1.0   02 Feb 1996 13:10:28   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "pmwini.h"
#include "upgdll.h"
#include "util.h"
#include "pmwcfg.h"
#include "inifile.h"

#ifndef WIN32
#include <auxdata.h>			// for _AfxGetSafeOwner()
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpgradeDLL

CUpgradeDLL::CUpgradeDLL()
{
	m_hLibrary = NULL;
	m_Upgrade = NULL;
}

CUpgradeDLL::~CUpgradeDLL()
{
	Shutdown();
}

BOOL CUpgradeDLL::Startup(void)
{
	Shutdown();

/*
// Generate the library name.
*/
	CString csLibName;

	TRY
	{
#ifdef WIN32
		csLibName = GetGlobalPathManager()->ExpandPath("[[H]]\\MSREG\\IUPG32.DLL");
#else
		csLibName = GetGlobalPathManager()->ExpandPath("[[H]]\\MSREG\\IUPG16.DLL");
#endif
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL

#ifdef WIN32
	CString csPath;
	Util::SplitPath(csLibName, &csPath, NULL);
	Util::RemoveBackslashFromPath(csPath);
	char cbCWD[_MAX_PATH];

	GetCurrentDirectory(sizeof(cbCWD), cbCWD);
	if (!csPath.IsEmpty())
	{
		SetCurrentDirectory(csPath);
	}
#endif

	UINT wOldSem = SetErrorMode(SEM_NOOPENFILEERRORBOX);

	if ((m_hLibrary = LoadLibrary(csLibName)) < (HINSTANCE)HINSTANCE_ERROR)
	{
#ifdef WIN32
		DWORD dwLastError = GetLastError();
		TRACE("Error %lu\n", dwLastError);
#endif
		m_hLibrary = NULL;
	}

	SetErrorMode(wOldSem);

#ifdef WIN32
	SetCurrentDirectory(cbCWD);
#endif

	GetEntryPoints();

	return IsValid();
}

void CUpgradeDLL::Shutdown(void)
{
	if (m_hLibrary >= (HINSTANCE)HINSTANCE_ERROR)
	{
		FreeLibrary(m_hLibrary);
		m_hLibrary = NULL;
	}

	m_Upgrade = NULL;
}

/*
// Set up the entry points.
*/

void CUpgradeDLL::GetEntryPoints(void)
{
	if (m_hLibrary != NULL)
	{
#ifdef WIN32
		m_Upgrade = (FN_UPGRADE)GetProcAddress(m_hLibrary, "Upgrade");
#else
		m_Upgrade = (FN_UPGRADE)GetProcAddress(m_hLibrary, "UPGRADE");
#endif
	
		if (m_Upgrade == NULL)
		{
			Shutdown();
		}
	}
}

UPGRESULT CUpgradeDLL::Upgrade(CWnd* pWnd /*=NULL*/)
{
	if (!IsValid() || m_Upgrade == NULL)
	{
		return UPGRESULT_Error;
	}

	// Get the parent part number.
	CString csMsregIni = GetGlobalPathManager()->ExpandPath("MSREG.INI");
	CIniFile MsregIniFile(csMsregIni);

	DWORD dwPPN = MsregIniFile.GetLong("Configuration", "ParentPartNumber", 0);
	if (dwPPN == 0)
	{
		return UPGRESULT_Error;
	}

	// Use the creation time of PMINST.DAT as our upgrade time.
	CString csTimeStampFile;
	TRY
	{
		csTimeStampFile = GetGlobalPathManager()->ExpandPath("PMINST.DAT");
	}
	END_TRY

	FILETIME CreateTime;
	if ((csTimeStampFile.IsEmpty())
	 || (!Util::GetFileTime(csTimeStampFile, &CreateTime, NULL, NULL)))
	{
		return UPGRESULT_Error;
	}

	// Do the upgrade.
	HWND hWnd;
#ifdef WIN32
	hWnd = CWnd::GetSafeOwner(pWnd)->GetSafeHwnd();
#else
	hWnd = _AfxGetSafeOwner(pWnd);
#endif
	return m_Upgrade(
		hWnd,
		EventNotify,
		GetConfiguration()->ProductLevel(),
		dwPPN,
		CreateTime.dwHighDateTime ^ CreateTime.dwLowDateTime);
}

#ifdef WIN32
void PASCAL CUpgradeDLL::EventNotify(int nEvent, HWND hWndCurrent, int nID, DWORD dwData)
#else
void PASCAL __export CUpgradeDLL::EventNotify(int nEvent, HWND hWndCurrent, int nID, DWORD dwData)
#endif
{
	CWnd* pWnd = (hWndCurrent == NULL) ? NULL : CWnd::FromHandle(hWndCurrent);

	switch (nEvent)
	{
		case UPGRADE_EVENT_DialogStartup:
		{
			standard_dialog_startup(pWnd, nID);
			break;
		}
		case UPGRADE_EVENT_DialogShutdown:
		{
			standard_dialog_shutdown(pWnd, nID);
			break;
		}
		case UPGRADE_EVENT_DialogActivate:
		{
			standard_dialog_activate(nID, WA_ACTIVE, NULL, FALSE);
			break;
		}
		case UPGRADE_EVENT_DialogDeactivate:
		{
			standard_dialog_activate(nID, WA_INACTIVE, NULL, FALSE);
			break;
		}
		case UPGRADE_EVENT_Help:
		{
			AfxGetApp()->WinHelp(dwData, nID);
			break;
		}
		default:
		{
			break;
		}
	}
}

