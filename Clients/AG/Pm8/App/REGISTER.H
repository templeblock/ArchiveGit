/*
// $Workfile: REGISTER.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/REGISTER.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:30:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:10   FRED
// Initial revision.
// 
//    Rev 2.7   05 Jan 1996 17:21:28   JAY
// Calling convention fix for notify function.
// 
//    Rev 2.6   05 Dec 1995 14:39:18   JAY
// 
//    Rev 2.5   17 Nov 1995 13:14:14   JAY
// Added GetRegistrationData command.
// 
//    Rev 2.4   15 Nov 1995 10:30:06   JAY
// Fixes.
// 
//    Rev 2.3   14 Nov 1995 09:43:00   JAY
// Event notification
// 
//    Rev 2.2   10 Nov 1995 17:38:58   JAY
// New registration code
// 
//    Rev 2.1   17 Aug 1995 07:21:34   FRED
//  
*/ 

#ifndef __REGISTER_H__
#define __REGISTER_H__

#include "pmwdlg.h"
#include "msreg.h"

/////////////////////////////////////////////////////////////////////////////
// CRegisterDLL class

class CRegisterDLL
{
	typedef REGRESULT (FAR PASCAL *FN_REGSENDCOMMAND)(HWND hWnd, LPCSTR pszCommand, DWORD dwData, LPCSTR pszMsregxDllPath);
public:
	CRegisterDLL();
	virtual ~CRegisterDLL();

	BOOL Startup(void);
	void Shutdown(void);

	REGRESULT Register(void);
	REGRESULT RegisterBonus(void);
	REGRESULT IsRegistered(void);
	REGRESULT IsBonusEnabled(void);
	REGRESULT GetRegistrationData(LPCSTR pField, CString& csData);

	REGRESULT RegSendCommand(LPCSTR pCommand, DWORD dwData = 0);
	BOOL IsValid(void) const
		{ return m_hLibrary != 0; }

#ifdef WIN32
	static void PASCAL EventNotify(int nEvent, HWND hWndCurrent, int nID, DWORD dwData);
#else
	static void PASCAL __export EventNotify(int nEvent, HWND hWndCurrent, int nID, DWORD dwData);
#endif

protected:
	void GetEntryPoints(void);

	FN_REGSENDCOMMAND m_RegSendCommand;
	HINSTANCE m_hLibrary;
};

/////////////////////////////////////////////////////////////////////////////
// CUnregisteredItemsDialog dialog

class CUnregisteredItemsDialog : public CPmwDialog
{
// Construction
public:
	CUnregisteredItemsDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUnregisteredItemsDialog)
	enum { IDD = IDD_UNREGISTERED_ITEMS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CUnregisteredItemsDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CRegisterDLL RegisterDLL;

#endif
