/*
// $Workfile: MSREGX.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSREGX.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.14   15 Oct 1996 11:53:40   FRED
// Frees INTLnn.DLL
// 
//    Rev 1.13   04 Oct 1996 15:29:54   FRED
// Uses international DLL (INTLxx.DLL)
// 
//    Rev 1.12   26 Jun 1996 07:05:24   FRED
// International Registration
// 
//    Rev 1.11   11 Mar 1996 16:31:18   FRED
// New registration packet
// 
//    Rev 1.10   11 Mar 1996 15:34:26   FRED
// Printed registration form
// 
//    Rev 1.9   13 Dec 1995 12:34:38   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.8   12 Dec 1995 17:55:34   FRED
// Modem Only Version
// 
//    Rev 1.7   07 Dec 1995 11:10:12   JAY
// Added some limited variable access code for 16-bit version.
// 
//    Rev 1.6   05 Dec 1995 13:03:34   JAY
// Event notification.
// 
//    Rev 1.5   28 Nov 1995 16:13:48   FRED
// No Free variations
// 
//    Rev 1.4   28 Nov 1995 13:13:22   JAY
// Now traps WinHelp()
// 
//    Rev 1.3   28 Nov 1995 08:46:32   FRED
// Phone Switch
// 
//    Rev 1.2   22 Nov 1995 13:51:10   FRED
// Added pszIniFileName paramater to Register().
// 
//    Rev 1.1   22 Nov 1995 10:26:04   FRED
// Merged in bonus changes from PMW version
// 
//    Rev 1.0   22 Nov 1995 08:52:00   FRED
// Initial revision.
// 
//    Rev 1.2   16 Nov 1995 10:57:56   FRED
//  
// 
//    Rev 1.1   16 Nov 1995 10:31:34   FRED
//  
// 
//    Rev 1.0   15 Nov 1995 17:19:34   FRED
// Initial revision.
// 
//    Rev 1.4   31 Oct 1995 12:01:48   FRED
//  
// 
//    Rev 1.3   26 Oct 1995 10:27:52   FRED
//  
*/ 

// MSREGX.h : main header file for MSREGXnn.DLL
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

class CMsregDllApp;

#ifndef __MSREGX_H__
#define __MSREGX_H__

#include "resource.h"
#include "intl\intldll.h"
#include "inifile.h"

#ifdef WIN32
#define REGENTRYPOINT __declspec(dllexport) REGRESULT FAR PASCAL
#else
#define REGENTRYPOINT REGRESULT FAR PASCAL _export
#endif

/*
// Define alternate entry point protocol for dll itself.
*/

#include "msreg.h"

/////////////////////////////////////////////////////////////////////////////
// CMsregDllApp
// See MSREGDLL.cpp for the implementation of this class
//

class CMsregDllApp : public CWinApp
{
public:
	CMsregDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMsregDllApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMsregDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CIniFile m_IniFile;
	CIniFile m_UserIniFile;
	CString m_csWorkingDirectory;
	CString m_csApplicationName;
	int m_nConfiguration;
	CInternationalInformationDll* m_pInternationalInformationDll;
	CInternationalData* m_pInternationalData;
	CString m_csRegistrationPrefix;

#ifndef WIN32
	HTASK m_hEventTask;
#endif
	EVENT_NOTIFY_FUNCTION m_pEventNotification;

	BOOL SetupWorkingDirectory(void);

	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);

public:
	BOOL SetupApplicationName(void);
	BOOL SetupIniFile(LPCSTR pszIniFileName);
	BOOL SetupConfiguration(void);

	/*
	// Set the event notification function.
	*/

	void SetEventNotification(EVENT_NOTIFY_FUNCTION pEventNotification);

	/*
	// Do event notification.
	*/

	void EventNotify(int nEvent, HWND hWnd, int nID, DWORD dwData = 0);

	/*
	// Get the IniFile.
	*/

	CIniFile& IniFile(void)
		{ return m_IniFile; }

	/*
	// Get the user data IniFile.
	*/

	CIniFile& UserIniFile(void)
		{ return m_UserIniFile; }

	/*
	// Get the working directory.
	*/

	const CString& WorkingDirectory(void) const
		{ return m_csWorkingDirectory; }

	/*
	// Get the application name.
	*/

	const CString& ApplicationName(void) const
		{ return m_csApplicationName; }

	/*
	// Get the current configuration.
	*/

	int Configuration(void) const
		{ return m_nConfiguration; }
		
	/*
	// Pick a dialog, any dialog.
	*/

	enum
	{
		CONFIGURATION_E		=	0,
		CONFIGURATION_EP		=	1,
		CONFIGURATION_EM		=	2,
		CONFIGURATION_EPM		=	3
	};

	UINT PickDialog(const UINT* pIdArray) const
		{ return pIdArray[Configuration()]; }

	/*
	// Find out if the user has registered.
	*/

	BOOL UserIsRegistered(void);

	/*
	// Mark the user as registered.
	*/

	BOOL RegisterUser(void);

	/*
	// Get the registration parameters for the user.
	*/

	CInternationalData* GetInternationalData(void)
		{ return m_pInternationalData; }
	void SetInternationalData(CInternationalData* pInternationalData)
		{ m_pInternationalData = pInternationalData; }

#ifndef NO_FREE_BONUS	
	/*
	// Find out if the user has enabled the bonus.
	*/

	BOOL UserIsBonusEnabled(void);

	/*
	// Attempt to make the user bonus enabled.
	*/

	BOOL BonusEnableUser(LPCSTR pszRegistrationCode, CWnd* pParent = NULL);
#endif

	/*
	// Get the product code number to use for registration: CRn-ppn.
	*/

	BOOL RegistrationProductCode(CString& csCode);
	CString RegistrationProductCode(void);

	CString GetModemNumber(BOOL fIncludePrefix = FALSE)
		{ return MakePhoneNumber(GetInternationalData(), &GetInternationalData()->m_Registration.m_Modem, fIncludePrefix); }
	CString GetPhoneNumber(BOOL fIncludePrefix = FALSE)
		{ return MakePhoneNumber(GetInternationalData(), &GetInternationalData()->m_Registration.m_Phone, fIncludePrefix); }
	CString GetFaxNumber(BOOL fIncludePrefix = FALSE)
		{ return MakePhoneNumber(GetInternationalData(), &GetInternationalData()->m_Registration.m_Fax, fIncludePrefix); }
	CString GetHelpNumber(BOOL fIncludePrefix = FALSE)
		{ return MakePhoneNumber(GetInternationalData(), &GetInternationalData()->m_Registration.m_Help, fIncludePrefix); }
	CString MakePhoneNumber(CInternationalData* pInternationalData, CInternationalData::PhoneNumber* pPhoneNumber, BOOL fIncludePrefix = FALSE);

	// Wrappers for CInternationalData methods.
	BOOL GetInternationalData(WORD wCountryId, CInternationalData* pData);
	BOOL FillCountryComboBox(HWND hComboBox, WORD wSelectCountryId = 0);
	BOOL GetCountryName(WORD wCountryId, CString& csCountryName);
	BOOL GetWebSite(WORD wCountryId, CString& csWebSite);
};

/*
// A little helper function to get the app.
*/

#define GetApp()		((CMsregDllApp*)AfxGetApp())

/////////////////////////////////////////////////////////////////////////////

#endif
