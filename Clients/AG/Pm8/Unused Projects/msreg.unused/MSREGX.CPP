/*
// $Workfile: MSREGX.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSREGX.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.22   06 Jun 1997 16:56:34   Fred
// CR5 is canned prefix, new reg code algorithm based on PPN
// 
//    Rev 1.21   15 Oct 1996 11:53:40   FRED
// Frees INTLnn.DLL
// 
//    Rev 1.20   04 Oct 1996 15:29:54   FRED
// Uses international DLL (INTLxx.DLL)
// 
//    Rev 1.19   01 Oct 1996 10:38:16   FRED
//  
// 
//    Rev 1.18   30 Aug 1996 09:19:36   FRED
// Fixed bad reg code crash
// 
//    Rev 1.17   26 Jun 1996 07:04:12   FRED
// International Registration
// 
//    Rev 1.16   29 Mar 1996 14:02:10   JAY
// Extensions for file-based BMPs.
// 
//    Rev 1.15   22 Mar 1996 15:15:48   FRED
// Default configuration is EPMF
// 
//    Rev 1.14   11 Mar 1996 16:31:16   FRED
// New registration packet
// 
//    Rev 1.13   11 Mar 1996 15:34:26   FRED
// Printed registration form
// 
//    Rev 1.12   13 Dec 1995 12:34:38   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.11   12 Dec 1995 17:56:28   FRED
// Modem Only Version
// 
//    Rev 1.10   07 Dec 1995 11:10:12   JAY
// Added some limited variable access code for 16-bit version.
// 
//    Rev 1.9   05 Dec 1995 13:11:14   JAY
// 
//    Rev 1.8   05 Dec 1995 13:03:26   JAY
// Event notification.
// 
//    Rev 1.7   30 Nov 1995 10:20:14   FRED
//  
// 
//    Rev 1.6   28 Nov 1995 16:13:46   FRED
// No Free variations
// 
//    Rev 1.5   28 Nov 1995 13:13:16   JAY
// Now traps WinHelp()
// 
//    Rev 1.4   28 Nov 1995 08:46:30   FRED
// Phone Switch
// 
//    Rev 1.3   22 Nov 1995 13:51:10   FRED
// Added pszIniFileName paramater to Register().
// 
//    Rev 1.2   22 Nov 1995 13:34:18   FRED
// Call InstallBonus() in INSTALL.CPP.
// 
// 
//    Rev 1.1   22 Nov 1995 10:26:04   FRED
// Merged in bonus changes from PMW version
// 
//    Rev 1.0   22 Nov 1995 08:52:12   FRED
// Initial revision.
// 
//    Rev 1.1   16 Nov 1995 10:31:26   FRED
//  
// 
//    Rev 1.0   15 Nov 1995 17:19:38   FRED
// Initial revision.
// 
//    Rev 1.10   31 Oct 1995 14:39:46   FRED
// TestMsregDll command does skip stuff
// 
//    Rev 1.9   31 Oct 1995 13:46:40   FRED
//  
// 
//    Rev 1.8   31 Oct 1995 12:01:48   FRED
//  
// 
//    Rev 1.7   30 Oct 1995 13:38:12   FRED
// 
//    Rev 1.6   30 Oct 1995 13:35:56   JAY
// Added register for progress bar.
// 
//    Rev 1.5   30 Oct 1995 12:11:50   FRED
//  
// 
//    Rev 1.4   26 Oct 1995 10:27:56   FRED
//  
*/ 

// MSREGDLL.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "msregx.h"

#include "bmpctl.h"
#include "cwmfctl.h"
#include "cprogbar.h"
#include "whereru.h"
#include "register.h"
#include "congrats.h"
#include "badcode.h"
#include "msregini.h"
#include "util.h"
#include "install.h"

#ifndef NO_FREE_BONUS
#include "regbonus.h"
#include "key.h"
#else
#include "already.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Exported entry points.
extern "C" REGENTRYPOINT Register(HWND hWnd, DWORD dwData, LPCSTR pszIniFileName, EVENT_NOTIFY_FUNCTION pNotifyFunction);

/////////////////////////////////////////////////////////////////////////////
// CMsregDllApp

BEGIN_MESSAGE_MAP(CMsregDllApp, CWinApp)
	//{{AFX_MSG_MAP(CMsregDllApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsregDllApp construction

CMsregDllApp::CMsregDllApp()
{
#ifndef WIN32
	m_hEventTask = 0;
#endif
	m_pEventNotification = NULL;

	m_pInternationalInformationDll = NULL;
	m_pInternationalData = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMsregDllApp object

CMsregDllApp theApp;

BOOL CMsregDllApp::InitInstance() 
{
	if (!CWinApp::InitInstance())
	{
		return FALSE;
	}

	CBitmapControl::RegisterMyClass();
	CWindowsMetaFileControl::RegisterMyClass();
	CProgressBar::RegisterMyClass();
	
/*
// Set default dialog background to be gray.
*/

	SetDialogBkColor();
	
/*
// Compute the working directory.
*/

	if (!SetupWorkingDirectory())
	{
		return FALSE;
	}

	CBitmapControl::SetWorkingDirectory(m_csWorkingDirectory);

	// Allocate the object used for computing international information.
	TRY
	{
		m_pInternationalInformationDll = new CInternationalInformationDll(m_csWorkingDirectory);
	}
	END_TRY

	if (m_pInternationalInformationDll == NULL)
	{
		AfxMessageBox(IDS_CantLoadInternationalInformationDll);
		return FALSE;
	}

	return TRUE;
}

int CMsregDllApp::ExitInstance()
{
	// Free object used for computing international information.
	delete m_pInternationalInformationDll;
	m_pInternationalInformationDll = NULL;
	
	return 0;
}

/*
// Figure out our "working" directory. It's the one we were loaded from.
*/

BOOL CMsregDllApp::SetupWorkingDirectory(void)
{
	BOOL fResult = FALSE;

	CString csModuleFileName;

	m_csWorkingDirectory.Empty();

	TRY
	{
	/*
	// Setup the working directory.
	*/

		fResult = (::GetModuleFileName(AfxGetInstanceHandle(), csModuleFileName.GetBuffer(_MAX_PATH+1), _MAX_PATH+1-1) != 0);
		csModuleFileName.ReleaseBuffer();

		if (fResult)
		{
			Util::SplitPath(csModuleFileName, &m_csWorkingDirectory, NULL);
			fResult = TRUE;
		}
	}
	END_TRY

	return fResult;
}

/*
// Preload the application name from MSREG.INI for quick access.
*/

BOOL CMsregDllApp::SetupApplicationName(void)
{
	m_csApplicationName = m_IniFile.GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_Application, "");
	return !m_csApplicationName.IsEmpty();
}

/*
// Set up the full path names of MSREG.INI and MSREGUSR.INI.
*/

BOOL CMsregDllApp::SetupIniFile(LPCSTR pszIniFileName)
{
	CString csWindowsDirectory;

	TRY
	{
		if (pszIniFileName != NULL)
		{
			m_IniFile.Name(pszIniFileName);
		}
		else
		{
			m_IniFile.Name(WorkingDirectory() + MSREGINI_IniName);
		}

		GetWindowsDirectory(csWindowsDirectory.GetBuffer(256), 256-1);
		csWindowsDirectory.ReleaseBuffer();
		m_UserIniFile.Name(csWindowsDirectory + "\\" + MSREGUSRINI_IniName);
	}
	CATCH_ALL(e)
	{
		csWindowsDirectory.Empty();
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;
}

/*
// Preload the configuration flags from MSREG.INI for quick access.
*/

BOOL CMsregDllApp::SetupConfiguration(void)
{
	m_nConfiguration = -1;
	int nRegistrationCodeNumber = 0;

	CInternationalData* pInternationalData = GetInternationalData();

	// Make sure modem is available.
	ASSERT(pInternationalData->m_Registration.m_Modem.Exists());
	if (pInternationalData->m_Registration.m_Modem.Exists())
	{
		// They can modem stuff in.
		m_nConfiguration = 0;

		if (pInternationalData->m_Registration.m_Phone.Exists())
		{
			// They can call in.
			m_nConfiguration += 1;
		}

		if (pInternationalData->m_Registration.m_Mail.Exists())
		{
			// They can mail and fax stuff in.
			m_nConfiguration += 2;
		}

		// 6/6/97 (FF) Registration code prefix is always CR5. The code number
		// is no longer used to distinguish between configurations. This switch
		// to CR5 corresponds to the change in the registration code matching the
		// reverse of the last four digits of the Parent Part Number.

		nRegistrationCodeNumber = 5;

#if 0
		switch (m_nConfiguration)
		{
			case 0:	nRegistrationCodeNumber = 5;	break;
			case 2:	nRegistrationCodeNumber = 4;	break;
			case 3:	nRegistrationCodeNumber = 3;	break;	
			default:	nRegistrationCodeNumber = 0;	break;
		}
#endif

	}

	// Set the registration prefix string.
	TRY
	{
		ASSERT(nRegistrationCodeNumber != 0);
		m_csRegistrationPrefix.Format("CR%d", nRegistrationCodeNumber);
	}
	END_TRY

	return (nRegistrationCodeNumber != 0) && (m_nConfiguration >= 0);
}

/*
// Is the user registered?
*/

BOOL CMsregDllApp::UserIsRegistered(void)
{
	return UserIniFile().GetInteger(ApplicationName(), MSREGUSRINI_Application_Registered, 0);
}

/*
// Mark the user as registered.
*/

BOOL CMsregDllApp::RegisterUser(void)
{
	// Make sure they're marked as registered.
	GetApp()->UserIniFile().WriteInteger(GetApp()->ApplicationName(), MSREGUSRINI_Application_Registered, 1);
	return TRUE;
}

#ifndef NO_FREE_BONUS
/*
// Is the user Bonus Enabled?
*/

BOOL CMsregDllApp::UserIsBonusEnabled(void)
{
	BOOL fResult = FALSE;

	CString csRegistrationCode;

	TRY
	{
		csRegistrationCode = UserIniFile().GetString(ApplicationName(), MSREGUSRINI_Application_RegistrationCode);

		if (csRegistrationCode == MODEM_REGISTRATION_CODE)
		{
			fResult = TRUE;
		}
		else
		{
			if (!csRegistrationCode.IsEmpty())
			{
				// 6/6/97 (FF) Switched to scheme where registration code is last four digits of
				// Parent Part Number reversed.
				CString csParentPartNumber;
				TRY
				{
					csParentPartNumber = GetApp()->IniFile().GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_ParentPartNumber);
					if (csParentPartNumber.GetLength() >= 4)
					{
						csParentPartNumber.MakeReverse();
						csParentPartNumber = csParentPartNumber.Left(4);
						Util::Trim(csRegistrationCode);
						if (csRegistrationCode == csParentPartNumber)
						{
							fResult = TRUE;
						}
					}
				}
				END_TRY

#if 0
				unsigned long lKey = (unsigned long)IniFile().GetLong(MSREGINI_Section_Configuration, MSREGINI_Configuration_Key, 0);
				if (lKey == KeyFromString(csRegistrationCode))
				{
					fResult = TRUE;
				}
#endif

			}
		}
	}
	END_TRY

	return fResult;
}

/*
// Attempt to "bonus enable" the user.
*/

BOOL CMsregDllApp::BonusEnableUser(LPCSTR pszRegistrationCode, CWnd* pParent /*=NULL*/)
{
	UserIniFile().WriteString(ApplicationName(), MSREGUSRINI_Application_RegistrationCode, pszRegistrationCode);

	// Check if the user entered the correct string.
	if (UserIsBonusEnabled())
	{
		// Make sure they're marked as registered.
		GetApp()->UserIniFile().WriteInteger(GetApp()->ApplicationName(), MSREGUSRINI_Application_Registered, 1);

		// Thank the user!
		CCongratulationsDialog CongratsDialog(pParent);
		CongratsDialog.DoModal();

		return TRUE;
	}
	else
	{
		// Bad registration code. Complain.
		CBadRegistrationCodeDialog Dialog(pParent);
		Dialog.DoModal();
		
		return FALSE;
	}
}
#endif

/*
// Run the register dialog.
*/

extern "C" REGENTRYPOINT Register(HWND hWnd, DWORD dwData, LPCSTR pszIniName, EVENT_NOTIFY_FUNCTION pNotifyFunction)
{
	long lResult = REGRESULT_CommandError;

	GetApp()->SetEventNotification(pNotifyFunction);

	AfxLockTempMaps();

	TRY
	{
	/*
	// Generate the MSREG.INI file name.
	// GetWorkingDirectory() is guaranteed to have a trailing backslash,
	// so we just append the INI name.
	*/

		if (!GetApp()->SetupIniFile(pszIniName))
		{
			AfxThrowUserException();
		}

	/*
	// Get the "application name" from MSREG.INI (SetupIniFile must have been called).
	*/

		if (!GetApp()->SetupApplicationName())
		{
			AfxThrowUserException();
		}

#ifndef NO_FREE_BONUS
		// If the user is not already bonus enabled, then ask them to register
		// or give them a place to enter their code (register by mail).

		if (!GetApp()->UserIsBonusEnabled())
		{
			// Run the register dialog.
			UINT nResult = IDC_REGISTER_NOW;

			CRegisterForBonusDialog Dialog(hWnd != NULL ? CWnd::FromHandle(hWnd) : NULL);
			nResult = Dialog.DoModal();

			if (nResult == IDC_REGISTER_NOW)
			{
				// First, find out just where this user is located.
				CWhereAreYouDialog WhereAreYou(hWnd != NULL ? CWnd::FromHandle(hWnd) : NULL);
				if (WhereAreYou.DoModal() == IDOK)
				{
					// Save the registration parameters.
					GetApp()->SetInternationalData(WhereAreYou.m_pInternationalData);
					ASSERT(GetApp()->GetInternationalData() != NULL);
					if (GetApp()->GetInternationalData() != NULL)
					{
						TRACE("Country = %d\n", GetApp()->GetInternationalData()->m_wCountry);

						// Determine the configuration.
						if (!GetApp()->SetupConfiguration())
						{
							AfxThrowUserException();
						}

						// Ok, the user has confessed. Let them register.
						CRegisterDialog Dialog(hWnd != NULL ? CWnd::FromHandle(hWnd) : NULL);
						Dialog.DoModal();
					}
				}

				// Get rid of the registration parameters.
				delete GetApp()->GetInternationalData();
				GetApp()->SetInternationalData(NULL);
			}

			// Return the registered status of the user.
			lResult = GetApp()->UserIsRegistered()
								? REGRESULT_NowRegistered
								: REGRESULT_NotRegistered;
		}
		else
		{
			// User is already registered.
			lResult = REGRESULT_AlreadyRegistered;
		}

		// If the user is "bonus enabled", then install the bonus.
		if (GetApp()->UserIsBonusEnabled())
		{
			InstallBonus(hWnd, GetApp()->WorkingDirectory());
		}
#else
		BOOL fRunRegister = TRUE;

		// If the user is already registered, tell him about it!

		if (GetApp()->UserIsRegistered())
		{
			CAlreadyRegisteredDialog Dialog(hWnd != NULL ? CWnd::FromHandle(hWnd) : NULL);
			if (Dialog.DoModal() != IDOK)
			{
				lResult = REGRESULT_AlreadyRegistered;
				fRunRegister = FALSE;
			}
		}

		// Run the register dialog.

		if (fRunRegister)
		{
			// First, find out just where this user is located.
			CWhereAreYouDialog WhereAreYou(hWnd != NULL ? CWnd::FromHandle(hWnd) : NULL);
			if (WhereAreYou.DoModal() == IDOK)
			{
				// Ok, the user has confessed. Let them register.
				CRegisterDialog Dialog(hWnd != NULL ? CWnd::FromHandle(hWnd) : NULL);
				Dialog.DoModal();
			}

			// Return the registered status of the user.
			lResult = GetApp()->UserIsRegistered()
								? REGRESULT_NowRegistered
								: REGRESULT_NotRegistered;
		}
#endif
	}
	CATCH_ALL(e)
	{
		ASSERT(FALSE);
		lResult = REGRESULT_Error;
	}
	END_CATCH_ALL

	AfxUnlockTempMaps();

	GetApp()->SetEventNotification(NULL);

	return lResult;
}

BOOL CMsregDllApp::RegistrationProductCode(CString& csCode)
{
	BOOL fResult = FALSE;

	CString csPrefix;
	CString csParentPartNumber;

	TRY
	{
		csParentPartNumber = GetApp()->IniFile().GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_ParentPartNumber);
		csCode = m_csRegistrationPrefix;
		if (!csCode.IsEmpty())
		{
			csCode += '-';
		}
		csCode += csParentPartNumber;

		fResult = TRUE;
	}
	END_TRY

	return fResult;
}

CString CMsregDllApp::RegistrationProductCode(void)
{
	CString csProductCode;
	
	TRY
	{
		if (!RegistrationProductCode(csProductCode))
		{
			csProductCode.Empty();
		}
	}
	CATCH_ALL(e)
	{
		csProductCode.Empty();
	}
	END_CATCH_ALL
	
	return csProductCode;
}

/*
// Notify the user (of this DLL) of some event.
*/

void CMsregDllApp::EventNotify(int nEvent, HWND hWnd, int nID, DWORD dwData /*=0*/)
{
	if (m_pEventNotification != NULL
#ifndef WIN32
		 && GetCurrentTask() == m_hEventTask
#endif
		 )
	{
		m_pEventNotification(nEvent, hWnd, nID, dwData);
	}
}

void CMsregDllApp::WinHelp(DWORD dwData, UINT nCmd /*=HELP_CONTEXT*/)
{
	EventNotify(EVENT_Help, NULL, nCmd, dwData);
}

/*
// Set the event notification function.
*/

void CMsregDllApp::SetEventNotification(EVENT_NOTIFY_FUNCTION pEventNotification)
{
#ifdef WIN32
	m_pEventNotification = pEventNotification;
#else
	HTASK hTask = GetCurrentTask();

	if (m_hEventTask == 0 || m_hEventTask == hTask)
	{
		m_hEventTask = (pEventNotification == NULL) ? 0: hTask;
		m_pEventNotification = pEventNotification;
	}
#endif
}

CString CMsregDllApp::MakePhoneNumber(CInternationalData* pInternationalData, CInternationalData::PhoneNumber* pPhoneNumber, BOOL fIncludePrefix /*=FALSE*/)
{
	CString csNumber;
	TRY
	{
		if (fIncludePrefix)
		{
			csNumber += GetApp()->UserIniFile().GetString(MSREGUSRINI_Section_DialingOptions, MSREGUSRINI_DialingOptions_Prefix, DEFAULT_PREFIX);
			int nLength = csNumber.GetLength();
			if (nLength > 0 && csNumber[nLength-1] != ',')
			{
				csNumber += ',';
			}
		}
			
		csNumber += pPhoneNumber->m_szPrefix;

		// Process "Dial 1" option if 
		if ((pPhoneNumber->m_fUnitedStatesLocal)
		 && (pInternationalData->m_fUnitedStatesLocal))
		{
			if (UserIniFile().GetInteger(MSREGUSRINI_Section_DialingOptions, MSREGUSRINI_DialingOptions_DialOne, DEFAULT_DIAL1))
			{
				csNumber += "1-";
			}
		}

		csNumber += pPhoneNumber->m_szNumber;
	}
	END_TRY

	return csNumber;
}

// Wrappers for CInternationalData methods.

BOOL CMsregDllApp::GetInternationalData(WORD wCountryId, CInternationalData* pData)
{
	BOOL fResult = FALSE;
	if (m_pInternationalInformationDll != NULL)
	{
		fResult = m_pInternationalInformationDll->GetInternationalData(wCountryId, IniFile().Name(), pData);
	}
	return fResult;
}

BOOL CMsregDllApp::FillCountryComboBox(HWND hComboBox, WORD wSelectCountryId /*=0*/)
{
	BOOL fResult = FALSE;
	if (m_pInternationalInformationDll != NULL)
	{
		fResult = m_pInternationalInformationDll->FillCountryComboBox(hComboBox, wSelectCountryId);
	}
	return fResult;
}

BOOL CMsregDllApp::GetCountryName(WORD wCountryId, CString& csCountryName)
{
	BOOL fResult = FALSE;
	if (m_pInternationalInformationDll != NULL)
	{
		TRY
		{
			fResult = m_pInternationalInformationDll->GetCountryName(wCountryId, csCountryName);
		}
		END_TRY
	}
	return fResult;
}

BOOL CMsregDllApp::GetWebSite(WORD wCountryId, CString& csWebSite)
{
	BOOL fResult = FALSE;
	if (m_pInternationalInformationDll != NULL)
	{
		TRY
		{
			fResult = m_pInternationalInformationDll->GetWebSite(wCountryId, IniFile().Name(), csWebSite);
		}
		END_TRY
	}
	return fResult;
}
