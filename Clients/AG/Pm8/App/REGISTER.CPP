/*
// $Workfile: REGISTER.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/REGISTER.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     6/21/98 12:33p Cboggio
// Allow Registration to be removed for localization
// 
//    Rev 1.0   14 Aug 1997 15:25:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:34:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:48   FRED
// Initial revision.
// 
//    Rev 2.9   05 Jan 1996 17:21:38   JAY
// Calling convention fix for notify function.
// 
//    Rev 2.8   05 Dec 1995 14:39:14   JAY
// 
//    Rev 2.7   17 Nov 1995 13:14:14   JAY
// Added GetRegistrationData command.
// 
//    Rev 2.6   15 Nov 1995 10:30:10   JAY
// Fixes.
// 
//    Rev 2.5   14 Nov 1995 09:42:58   JAY
// Event notification
// 
//    Rev 2.4   10 Nov 1995 17:38:58   JAY
// New registration code
// 
//    Rev 2.3   22 Aug 1995 19:22:02   FRED
//  
// 
//    Rev 2.2   17 Aug 1995 07:21:32   FRED
//  
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "pmwini.h"
#include "register.h"
#include "util.h"

#ifdef LOCALIZE
#include "pmwcfg.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterDLL

CRegisterDLL::CRegisterDLL()
{
	m_hLibrary = NULL;
	m_RegSendCommand = NULL;
}

CRegisterDLL::~CRegisterDLL()
{
	Shutdown();
}

BOOL CRegisterDLL::Startup(void)
{
	Shutdown();

/*
// Generate the library name.
*/
	CString csLibName;

	TRY
	{
#ifdef WIN32
		csLibName = GetGlobalPathManager()->ExpandPath("MSREG32.DLL");
#else
		csLibName = GetGlobalPathManager()->ExpandPath("MSREG16.DLL");
#endif
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL

	UINT wOldSem = SetErrorMode(SEM_NOOPENFILEERRORBOX);

	if ((m_hLibrary = LoadLibrary(csLibName)) < (HINSTANCE)HINSTANCE_ERROR)
	{
		m_hLibrary = NULL;
	}

	SetErrorMode(wOldSem);

	GetEntryPoints();

	// Setup our event notify.
	RegSendCommand(REGCOMMAND_SetEventNotify, (DWORD)EventNotify);

	return IsValid();
}

void CRegisterDLL::Shutdown(void)
{
	if (m_hLibrary >= (HINSTANCE)HINSTANCE_ERROR)
	{
		FreeLibrary(m_hLibrary);
		m_hLibrary = NULL;
	}

	m_RegSendCommand = NULL;
}

/*
// Set up the entry points.
// This is used by both Startup methods.
*/

void CRegisterDLL::GetEntryPoints(void)
{
	if (m_hLibrary != NULL)
	{
#ifdef WIN32
		m_RegSendCommand = (FN_REGSENDCOMMAND)GetProcAddress(m_hLibrary, "RegSendCommand");
#else
		m_RegSendCommand = (FN_REGSENDCOMMAND)GetProcAddress(m_hLibrary, "REGSENDCOMMAND");
#endif
	
		if (m_RegSendCommand == NULL)
		{
			Shutdown();
		}
	}
}

REGRESULT CRegisterDLL::RegSendCommand(LPCSTR pCommand, DWORD dwData /*=0*/)
{
	if (!IsValid() || m_RegSendCommand == NULL)
	{
		return REGRESULT_Error;
	}
	return m_RegSendCommand(AfxGetMainWnd()->GetSafeHwnd(),
								 pCommand,
								 dwData,
								 NULL);
}

REGRESULT CRegisterDLL::Register(void)
{
#ifdef LOCALIZE
	if (GetConfiguration()->RemoveRegistration())
		return TRUE;
#endif
	return RegSendCommand(REGCOMMAND_Register);
}

REGRESULT CRegisterDLL::RegisterBonus(void)
{
#ifdef LOCALIZE
	if (GetConfiguration()->RemoveRegistration())
		return TRUE;
#endif
	return RegSendCommand(REGCOMMAND_Register " /C /A");
}

REGRESULT CRegisterDLL::IsRegistered(void)
{
	return RegSendCommand(REGCOMMAND_IsRegistered);
}

REGRESULT CRegisterDLL::IsBonusEnabled(void)
{
#ifdef LOCALIZE
	if (GetConfiguration()->RemoveRegistration())
		return REGRESULT_AlreadyRegistered;
#endif
	return RegSendCommand(REGCOMMAND_IsBonusEnabled);
}

REGRESULT CRegisterDLL::GetRegistrationData(LPCSTR pField, CString& csData)
{
	const int nSize = 128;
	REGRESULT Result;
	TRY
	{
		REGDATA_GetRegistrationData Data;
		Data.m_pszField = pField;
		Data.m_pszData = csData.GetBuffer(nSize);
		Data.m_nData = nSize;

		Result = RegSendCommand(REGCOMMAND_GetRegistrationData, (DWORD)&Data);
		csData.ReleaseBuffer();
	}
	CATCH_ALL(e)
	{
		Result = REGRESULT_Error;
	}
	END_CATCH_ALL

	return Result;
}

#ifdef WIN32
void PASCAL CRegisterDLL::EventNotify(int nEvent, HWND hWndCurrent, int nID, DWORD dwData)
#else
void PASCAL __export CRegisterDLL::EventNotify(int nEvent, HWND hWndCurrent, int nID, DWORD dwData)
#endif
{
	CWnd* pWnd = (hWndCurrent == NULL) ? NULL : CWnd::FromHandle(hWndCurrent);

	switch (nEvent)
	{
		case EVENT_DialogStartup:
		{
			standard_dialog_startup(pWnd, nID);
			break;
		}
		case EVENT_DialogShutdown:
		{
			standard_dialog_shutdown(pWnd, nID);
			break;
		}
		case EVENT_DialogActivate:
		{
			standard_dialog_activate(nID, WA_ACTIVE, NULL, FALSE);
			break;
		}
		case EVENT_DialogDeactivate:
		{
			standard_dialog_activate(nID, WA_INACTIVE, NULL, FALSE);
			break;
		}
		case EVENT_Help:
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

/////////////////////////////////////////////////////////////////////////////
// The Registration DLL.

CRegisterDLL RegisterDLL;

/////////////////////////////////////////////////////////////////////////////
// CUnregisteredItemsDialog dialog

CUnregisteredItemsDialog::CUnregisteredItemsDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CUnregisteredItemsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUnregisteredItemsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CUnregisteredItemsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnregisteredItemsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUnregisteredItemsDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CUnregisteredItemsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUnregisteredItemsDialog message handlers

BOOL CUnregisteredItemsDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
