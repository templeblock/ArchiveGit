/*
// $Workfile: REGINFO.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/REGINFO.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:40   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:58   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/REGINFO.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.1   14 Oct 1996 13:34:54   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:41:04   Fred
// Initial revision.
// 
//    Rev 1.1   01 Mar 1996 13:44:36   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.0   02 Feb 1996 08:58:58   JAY
// Initial revision.
// 
//    Rev 1.4   18 Dec 1995 10:31:22   FRED
// Updates order form tax stuff along with reginfo
// 
//    Rev 1.3   15 Dec 1995 17:58:24   FRED
// Forces tax on CA modem orders.
// 
//    Rev 1.2   05 Dec 1995 12:12:26   FRED
// Order by Modem
// 
//    Rev 1.1   29 Nov 1995 11:25:14   JAY
//  
// 
//    Rev 1.0   29 Nov 1995 10:27:40   JAY
// Initial revision.
// 
//    Rev 1.3   26 Oct 1995 10:27:44   FRED
//  
*/ 

// reginfo.h : header file
//

class CNameList;
class CCountryList;
class CRegistrationEdit;
class CRegistrationInformationDialog;

#ifndef __REGINFO_H__
#define __REGINFO_H__

class CUpgradeDialog;

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
	enum { IDD = IDD_ORDER_FORM_INFORMATION };
	CRegistrationEdit	m_ExpirationDateEdit;
	CRegistrationEdit	m_CardNumberEdit;
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
	CString	m_csState;
	CString	m_csZip;
	CString	m_csPhone;
	CString	m_csEmail;
	CString	m_csCardNumber;
	CString	m_csExpirationDate;
	int		m_nPaymentMethod;
	//}}AFX_DATA
	
	WORD m_wCountryId;
	CString m_csCountry;
	
	long m_lSubTotal;
	long m_lTax;
	long m_lShipping;
	CUpgradeDialog* m_pOrderForm;
	
	BOOL m_fModemOrder;

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
	
protected:
	void UpdatePaymentAmount(void);
};

#endif
