#ifndef __LICENSE_H__
#define __LICENSE_H__

#pragma hdrstop

// License.h : main header file for the LICENSE DLL
//
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/LICENSE.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 3     8/20/98 4:13p Jayn
// LICENSE.DAT filename now sent from the app.
// Now supports exclusion filters.
// 
// 2     7/14/98 5:49p Psasse
// include files follow same path as in SourceSafe
// Deletion of ambiguous Corrupt file Message Box
// 
//    Rev 1.0   14 Aug 1997 15:36:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:24   Fred
// Initial revision.
// 
//    Rev 1.2   13 Jun 1997 11:31:00   Jay
// Initialize now returns error codes
// 
//    Rev 1.1   10 Jun 1997 15:44:22   Fred
// Cleanup
// 
//    Rev 1.0   10 Jun 1997 14:54:56   Fred
// Initial revision.
// 
//    Rev 1.1   20 May 1997 16:08:22   johno
// Changes for PrintMaster integration
// 
// 
//    Rev 1.0   20 May 1997 10:41:04   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"				// main symbols
#include "inifile.h"
#include "instdat.h"					// CInstallationData
#include "prod.h"						// CProductInformation
#include "..\..\msreg\intl\intldll.h"	// CInternationalInformationDll and CInternationalData
#include "licinfo.h"



/////////////////////////////////////////////////////////////////////////////
// CLicenseApp
// See Upgrade.cpp for the implementation of this class
//

class CLicenseApp : public CWinApp
{
public:
	CLicenseApp();
	~CLicenseApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicenseApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CLicenseApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void InitOffer(int nProductLevel, DWORD dwPPN);

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

	const CString& ParentPartNumber(void) const
		{ return m_csParentPartNumber; }


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

	void UpdateLicenseFlags();//prepare flags to disable items that have already been ordered.

	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

/*
// Set the return code.
*/

	void SetUpgradeResult(LICENSERESULT lResult)
		{ m_lResult = lResult; }

	LICENSERESULT GetUpgradeResult(void)
		{ return m_lResult; }

	//AE stuff.  Making the DLL generic
	LICENSERESULT GetEncryptionInfo(char FAR * szFileName,structEncryptedFileInfo * pinfo);
	LICENSERESULT IsLicensed(char FAR * szFileName,structEncryptedFileInfo * info);
   LICENSERESULT IsLicensedByCode(char FAR * szCode);
   BOOL CanClose();
	void BuildDataFile(BOOL bReallyBuild);
	LICENSERESULT Initialize(LPCSTR szDllPath, LPCSTR szLicenseDat, LPCSTR szOrderFile, LPCSTR szMsregIniFile, LPCSTR szFilters);
	//CString GetPath(){return m_csPath;}
	CString GetOrderFile(){return m_csOrderFile;}
	LICENSERESULT Upgrade(HWND hWnd, EVENT_NOTIFY_FUNCTION pNotify, int nProductLevel, DWORD dwPPN);
	const char FAR * GetLastDllError();

	const CStringArray& GetFilters(void) {return m_csaFilters;}

protected:
	//CString m_csWorkingDirectory;

	CIniFile m_UserIniFile;
	CIniFile m_MsRegIniFile;

	CString m_csApplicationName;
	CString m_csParentPartNumber;

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
	LICENSERESULT m_lResult;

	// Dll object used to access international information.
	CInternationalInformationDll* m_pInternationalInformationDll;
	
	// Cached international information.
	CInternationalData* m_pInternationalData;

	CEncryptedFileInfoTable * m_pEncryptedFileInfoTable;
	LICENSERESULT InitializeDatabase();
	//CString m_csPath;//path to where the upgrade files are.
	CString m_csDllPath;//where this and other needed dlls reside (like greenleaf stuff).  also contains the file where the hidden content is stored (ms.dat).
	CString m_csOrderFile;//file where the order info resides (*.ord) i.e. hierarchy, pricing of products.
	CString m_csMsregIniFile;//MSREG.INI
	CString m_csError;
	CStringArray m_csaFilters;	// filters to exclude items from the order form
	LICENSERESULT SaveDatabase();
	void BumpRunCnt(char FAR * szFileName,structEncryptedFileInfo * pinfo);

	
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

#define	GetApp()	((CLicenseApp*)AfxGetApp())

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



#ifndef WIN32
DWORD GetPrivateProfileSectionNames(char FAR *,int,CString);  
DWORD GetPrivateProfileSection(char FAR *,char FAR *,int,CString);  
#endif


/////////////////////////////////////////////////////////////////////////////

#endif

#pragma hdrstop
