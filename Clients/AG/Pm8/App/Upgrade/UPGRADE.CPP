// Upgrade.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Upgrade.h"

#include "bmpctl.h"
#include "cwmfctl.h"
#include "cprogbar.h"
#include "util.h"
#include "msregini.h"
#include "salestax.h"

#include "upgdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpgradeApp

BEGIN_MESSAGE_MAP(CUpgradeApp, CWinApp)
	//{{AFX_MSG_MAP(CUpgradeApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpgradeApp construction

CUpgradeApp::CUpgradeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_pEventNotification = NULL;

	m_pInternationalInformationDll = NULL;
	m_pInternationalData = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUpgradeApp object

CUpgradeApp theApp;

BOOL CUpgradeApp::InitInstance() 
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

	//
	// Setup the correct INI file names.
	//

	if (!SetupIniFiles())
	{
		return FALSE;
	}
	
/*
// Get the "application name" from MSREG.INI (SetupIniFile must have been called).
*/

	if (!SetupApplicationName())
	{
		return FALSE;
	}

	// Allocate the object used for computing international information.
	TRY
	{
		m_pInternationalInformationDll = new CInternationalInformationDll(m_csWorkingDirectory);
	}
	END_TRY

	if (m_pInternationalInformationDll == NULL)
	{
		AfxMessageBox(IDS_CANT_LOAD_INTL_DLL);
		return FALSE;
	}

	// We can now get the sales tax information. Requires international data
	// and ini files to be set up.
	SalesTaxList.Init();

	return TRUE;
}

/*
// Figure out our "working" directory. It's the one we were loaded from.
*/

BOOL CUpgradeApp::SetupWorkingDirectory(void)
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

extern "C" UPGENTRYPOINT Upgrade(HWND hWnd, EVENT_NOTIFY_FUNCTION pNotify, int nProductLevel, DWORD dwPPN, DWORD dwKey)
{
	/*****************************************************************************
	* For the moment, we are limiting the use of this DLL to one user at a time. *
	* It could be extended for multiple simultaneous clients in the future.      *
	*****************************************************************************/

	/*
	// Lock temporary maps.
	*/

	AfxLockTempMaps();

	/*
	// Init the offer.
	*/

	GetApp()->InitOffer(nProductLevel, dwPPN, dwKey);

	/*
	// Plug in the event notification function.
	*/

	GetApp()->SetEventNotification(pNotify);

	/*
	// Default result code.
	*/

	GetApp()->SetUpgradeResult(UPGRESULT_NotUpgraded);

	/*
	// Do the upgrade dialog.
	*/

	CUpgradeDialog Dialog(CWnd::FromHandle(hWnd));
	Dialog.DoModal();

	/*
	// Free any international data that has been allocate
	*/

	GetApp()->FreeInternationalData();

	/*
	// Unlock temporary maps.
	*/

	AfxUnlockTempMaps();

	/*
	// And go home...
	*/

	return GetApp()->GetUpgradeResult();
}

/*
// Initialize the offer.
*/

void CUpgradeApp::InitOffer(int nProductLevel, DWORD dwPPN, DWORD dwKey)
{
	m_Offer.Init(m_csWorkingDirectory, nProductLevel, dwPPN);
/*
// 3/14/96 JN
// If we need to correct the existing bug as far as empty offer codes go,
// this is the line we need to add.
// 3/15/96 JN
// This is a go!
*/
	m_InstallationData.Init(m_Offer.OfferCode(), dwKey);
//	m_InstallationData.OfferCode(m_Offer.OfferCode());
}

/*
// Set up the INI file name member to point to the correct file.
// Also, setup the other necessary INI files.
*/

BOOL CUpgradeApp::SetupIniFiles(void)
{
	CString csIniName;
	CString csWindowsDirectory;
	BOOL fResult = FALSE;

	TRY
	{
	/*
	// Set the MSREG.INI file name.
	*/

		Util::ConstructPath(csIniName, m_csWorkingDirectory, "MSREG.INI");
		m_MsRegIniFile.Name(csIniName);

	/*
	// Set the MSREGUSR.INI file name.
	*/

		GetWindowsDirectory(csWindowsDirectory.GetBuffer(256), 256-1);
		csWindowsDirectory.ReleaseBuffer();

		Util::ConstructPath(csIniName, csWindowsDirectory, "MSREGUSR.INI");
		m_UserIniFile.Name(csIniName);

		fResult = TRUE;
	}
	END_TRY

	if (!fResult)
	{
		AfxMessageBox(IDS_CANT_INTIALIZE_INI_NAMES);
	}

	return fResult;
}

BOOL CUpgradeApp::SetupApplicationName(void)
{
	m_csApplicationName = m_MsRegIniFile.GetString(MSREGINI_Section_Configuration, MSREGINI_Configuration_Application, "");
	return !m_csApplicationName.IsEmpty();
}

BOOL CUpgradeApp::UserNeedsTax(void)
{
	return UserIniFile().GetInteger(MSREGUSRINI_Section_CustomerInfo,
											  MSREGUSRINI_CustomerInfo_NeedsTax,
											  FALSE);
}

void CUpgradeApp::UserNeedsTax(BOOL fNeedsTax)
{
	UserIniFile().WriteInteger(MSREGUSRINI_Section_CustomerInfo,
										MSREGUSRINI_CustomerInfo_NeedsTax,
										fNeedsTax);
}

/*
// Notify the user (of this DLL) of some event.
*/

void CUpgradeApp::EventNotify(int nEvent, HWND hWnd, int nID, DWORD dwData /*=0*/)
{
	if (m_pEventNotification != NULL)
	{
		m_pEventNotification(nEvent, hWnd, nID, dwData);
	}
}

void CUpgradeApp::WinHelp(DWORD dwData, UINT nCmd /*=HELP_CONTEXT*/)
{
	EventNotify(UPGRADE_EVENT_Help, NULL, nCmd, dwData);
}

/*
// Set the event notification function.
*/

void CUpgradeApp::SetEventNotification(EVENT_NOTIFY_FUNCTION pEventNotification)
{
	m_pEventNotification = pEventNotification;
}

// Make a phone number.

CString CUpgradeApp::MakePhoneNumber(CInternationalData* pInternationalData, CInternationalData::PhoneNumber* pPhoneNumber, BOOL fIncludePrefix /*=FALSE*/)
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

// Make a mailing address string.

CString CUpgradeApp::MakeMailAddress(CInternationalData* pInternationalData, CInternationalData::MailAddress* pMailAddress)
{
	CString csAddress;
	CNameList csAddressList;
	TRY
	{
		csAddressList.SetNames(pMailAddress->m_szAddress);
		for (int nLine = 0; nLine < csAddressList.GetSize(); nLine++)
		{
			if (!csAddress.IsEmpty())
			{
				csAddress += '\n';
			}
			csAddress += csAddressList.ElementAt(nLine);
		}
	}
	END_TRY

	return csAddress;
}

// Wrappers for CInternationalData methods.

BOOL CUpgradeApp::GetInternationalData(WORD wCountryId, CInternationalData* pData)
{
	BOOL fResult = FALSE;
	if (m_pInternationalInformationDll != NULL)
	{
		fResult = m_pInternationalInformationDll->GetInternationalData(wCountryId, MsRegIniFile().Name(), pData);
	}
	return fResult;
}

BOOL CUpgradeApp::FillCountryComboBox(HWND hComboBox, WORD wSelectCountryId /*=0*/)
{
	BOOL fResult = FALSE;
	if (m_pInternationalInformationDll != NULL)
	{
		fResult = m_pInternationalInformationDll->FillCountryComboBox(hComboBox, wSelectCountryId);
	}
	return fResult;
}

BOOL CUpgradeApp::GetCountryName(WORD wCountryId, CString& csCountryName)
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

BOOL CUpgradeApp::GetWebSite(WORD wCountryId, CString& csWebSite)
{
	BOOL fResult = FALSE;
	if (m_pInternationalInformationDll != NULL)
	{
		TRY
		{
			fResult = m_pInternationalInformationDll->GetWebSite(wCountryId, MsRegIniFile().Name(), csWebSite);
		}
		END_TRY
	}
	return fResult;
}

BOOL CUpgradeApp::GetTaxInformation(CString& csTaxInformation)
{
	BOOL fResult = FALSE;
	if (m_pInternationalInformationDll != NULL)
	{
		TRY
		{
			fResult = m_pInternationalInformationDll->GetTaxInformation(MsRegIniFile().Name(), csTaxInformation);
		}
		END_TRY
	}
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyControl

WNDPROC CReadOnlyControl::m_pfnSuperWndProc = NULL;

CReadOnlyControl::CReadOnlyControl()
{
}

CReadOnlyControl::~CReadOnlyControl()
{
}

BEGIN_MESSAGE_MAP(CReadOnlyControl, CWnd)
	//{{AFX_MSG_MAP(CReadOnlyControl)
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyControl message handlers

int CReadOnlyControl::OnMouseActivate(CWnd* pDesktopWnd, UINT uHitTest, UINT uMessage)
{
	return MA_NOACTIVATEANDEAT;
}

BOOL CReadOnlyControl::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage)
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return TRUE;
}
