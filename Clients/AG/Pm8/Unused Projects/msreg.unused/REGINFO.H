/*
// $Workfile: REGINFO.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/REGINFO.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.4   04 Oct 1996 15:29:56   FRED
// Uses international DLL (INTLxx.DLL)
// 
//    Rev 1.3   26 Jun 1996 07:03:24   FRED
// International Registration
// 
//    Rev 1.2   11 Mar 1996 15:34:22   FRED
// Printed registration form
// 
//    Rev 1.1   05 Dec 1995 13:02:32   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:51:52   FRED
// Initial revision.
// 
//    Rev 1.4   20 Nov 1995 16:25:38   FRED
// Removed Title, added Email
// 
//    Rev 1.3   26 Oct 1995 10:27:44   FRED
//  
*/ 

// reginfo.h : header file
//

class CRegistrationEdit;
class CRegistrationInformationDialog;

#ifndef __REGINFO_H__
#define __REGINFO_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CNameList

class CNameList : public CStringArray
{
public:
	CNameList(LPCSTR pszNames = NULL, char cSeparator = '|');
	virtual ~CNameList();
public:
	virtual void SetNames(LPCSTR pszNames, char cSeparator = '|');
protected:
	BOOL GetNextName(CString& csName, CString& csNames, char cSeparator);
};

/////////////////////////////////////////////////////////////////////////////
// CCountryList Combo Box

class CCountryList : public CComboBox
{
public:
	CCountryList();
	virtual ~CCountryList();

protected:
	// Generated message map functions
	//{{AFX_MSG(CCountryList)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Fill(WORD wCountryId = 0);
	WORD GetCountryId(void);
	void ReturnGoesToNextControl(BOOL fReturnGoesToNextControl)
		{ m_fReturnGoesToNextControl = fReturnGoesToNextControl; }

protected:
	BOOL m_fReturnGoesToNextControl;
};

/////////////////////////////////////////////////////////////////////////////
// CRegistrationEdit window

class CRegistrationEdit : public CEdit
{
// Construction
public:
	CRegistrationEdit();

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CRegistrationEdit();

protected:
	// Generated message map functions
	//{{AFX_MSG(CRegistrationEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CRegistrationInformationDialog dialog

class CRegistrationInformationDialog : public CBaseDialog
{
// Construction
public:
	CRegistrationInformationDialog(UINT uID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegistrationInformationDialog)
	enum { IDD = IDD_REGISTRATION_INFORMATION };
	CRegistrationEdit	m_EmailEdit;
	CRegistrationEdit	m_PhoneEdit;
	CCountryList m_CountryList;
	CRegistrationEdit	m_ZipEdit;
	CRegistrationEdit	m_StateEdit;
	CRegistrationEdit	m_CityEdit;
	CRegistrationEdit	m_Address2Edit;
	CRegistrationEdit	m_Address1Edit;
	CRegistrationEdit	m_CompanyEdit;
	CRegistrationEdit	m_LastNameEdit;
	CRegistrationEdit	m_FirstNameEdit;
	CString	m_csFirstName;
	CString	m_csLastName;
	CString	m_csCompany;
	CString	m_csAddress1;
	CString	m_csAddress2;
	CString	m_csCity;
	CString	m_csZip;
	CString	m_csState;
	WORD     m_wCountryId;
	CString	m_csCountry;
	CString	m_csPhone;
	CString	m_csEmail;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegistrationInformationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegistrationInformationDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDialoutprefix();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual int ReportMissingRequiredFields(LPCSTR pszRequiredFields);
	virtual int ReportInvalidPhoneNumber(void);
};

#endif
