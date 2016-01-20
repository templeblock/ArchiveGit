// Upgrade.h : main header file for the UPGRADE DLL
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "inifile.h"
#include "instdat.h"			// CInstallationData
#include "prod.h"				// CProductInformation
#include "intldll.h"	// CInternationalInformationDll and CInternationalData

// Cheat a little with the 'long' here. Should be 'UPGRESULT'...

#ifdef WIN32
#define UPGENTRYPOINT __declspec(dllexport) long FAR PASCAL
#else
#define UPGENTRYPOINT long FAR PASCAL _export
#endif

#include "iupgrade.h"		// EVENT_NOTIFY_FUNCTION, etc.

/////////////////////////////////////////////////////////////////////////////
// CUpgradeApp
// See Upgrade.cpp for the implementation of this class
//

class CUpgradeApp : public CWinApp
{
public:
	CUpgradeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpgradeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CUpgradeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void InitOffer(int nProductLevel, DWORD dwPPN, DWORD dwKey);

/*
// Get the user data IniFile.
*/

	CIniFile& UserIniFile(void)
		{ return m_UserIniFile; }

/*
// Get the MSREG.INI IniFile.
*/

	CIniFile& MsRegIniFile(void)
		{ return m_MsRegIniFile; }

/*
// Get the application name.
*/

	const CString& ApplicationName(void) const
		{ return m_csApplicationName; }

/*
// Get the installation data.
*/

	CInstallationData* InstallationData(void)
		{ return &m_InstallationData; }

/*
// Get the offer information.
*/

	COfferInformation* Offer(void)
		{ return &m_Offer; }

/*
// Customer tax info.
*/

	BOOL UserNeedsTax(void);
	void UserNeedsTax(BOOL fNeedsTax);

/*
// Set the event notification function.
*/

	void SetEventNotification(EVENT_NOTIFY_FUNCTION pEventNotification);

/*
// Do event notification.
*/

	void EventNotify(int nEvent, HWND hWnd, int nID, DWORD dwData = 0);

	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

/*
// Set the return code.
*/

	void SetUpgradeResult(UPGRESULT lResult)
		{ m_lResult = lResult; }

	UPGRESULT GetUpgradeResult(void)
		{ return m_lResult; }

protected:
	CString m_csWorkingDirectory;

	CIniFile m_UserIniFile;
	CIniFile m_MsRegIniFile;

	CString m_csApplicationName;

/*
// Installation data.
*/

	CInstallationData m_InstallationData;

/*
// Offer information.
*/
	
	COfferInformation m_Offer;

	BOOL SetupWorkingDirectory(void);
	BOOL SetupIniFiles(void);
	BOOL SetupApplicationName(void);

	EVENT_NOTIFY_FUNCTION m_pEventNotification;
	UPGRESULT m_lResult;

	// Dll object used to access international information.
	CInternationalInformationDll* m_pInternationalInformationDll;
	
	// Cached international information.
	CInternationalData* m_pInternationalData;
	
public:
	CInternationalData* GetInternationalData(void)
		{ return m_pInternationalData; }
	void SetInternationalData(CInternationalData* pGetInternationalData)
		{ m_pInternationalData = pGetInternationalData; }
	void FreeInternationalData(void)
		{ delete m_pInternationalData; m_pInternationalData = NULL; }
	
	// Get various phone numbers.
	CString GetModemNumber(BOOL fIncludePrefix = FALSE)
		{ return MakePhoneNumber(GetInternationalData(), &GetInternationalData()->m_InstantSales.m_Modem, fIncludePrefix); }
	CString GetPhoneNumber(BOOL fIncludePrefix = FALSE)
		{ return MakePhoneNumber(GetInternationalData(), &GetInternationalData()->m_InstantSales.m_Phone, fIncludePrefix); }
	CString GetFaxNumber(BOOL fIncludePrefix = FALSE)
		{ return MakePhoneNumber(GetInternationalData(), &GetInternationalData()->m_InstantSales.m_Fax, fIncludePrefix); }
	CString GetHelpNumber(BOOL fIncludePrefix = FALSE)
		{ return MakePhoneNumber(GetInternationalData(), &GetInternationalData()->m_InstantSales.m_Help, fIncludePrefix); }
	CString MakePhoneNumber(CInternationalData* pInternationalData, CInternationalData::PhoneNumber* pPhoneNumber, BOOL fIncludePrefix = FALSE);
	CString MakeMailAddress(CInternationalData* pInternationalData, CInternationalData::MailAddress* pMailAddress);
	
	// Wrappers for CInternationalData methods.
	BOOL GetInternationalData(WORD wCountryId, CInternationalData* pData);
	BOOL FillCountryComboBox(HWND hComboBox, WORD wSelectCountryId = 0);
	BOOL GetCountryName(WORD wCountryId, CString& csCountryName);
	BOOL GetWebSite(WORD wCountryId, CString& csWebSite);
	BOOL GetTaxInformation(CString& csTaxInformation);
};

#define	GetApp()	((CUpgradeApp*)AfxGetApp())

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyControl

class CReadOnlyControl : public CWnd
{
// Construction
public:
	CReadOnlyControl();

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CReadOnlyControl();

protected:
	// Generated message map functions
	//{{AFX_MSG(CReadOnlyControl)
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT uHitTest, UINT uMessage);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
};

/////////////////////////////////////////////////////////////////////////////
