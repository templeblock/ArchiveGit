/*
// $Workfile: TESTDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/intl/TESTDLG.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/07/98 4:21p Jayn
// 
//    Rev 1.2   07 Oct 1996 14:03:24   FRED
// Added GetTaxInformation()
// 
//    Rev 1.1   04 Oct 1996 10:03:54   FRED
// Added GetCountryName(); Country Id's to data
// 
//    Rev 1.0   03 Oct 1996 10:31:02   FRED
// Initial revision.
*/

#ifndef __TESTDLG_H__
#define __TESTDLG_H__

#include "intldll.h"

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

class CTestDlg : public CDialog
{
// Construction
public:
	CTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestDlg)
	enum { IDD = IDD_TEST_DIALOG };
	CComboBox	m_CountryList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeCountryList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void UpdateCountryInformation(void);
	LPCSTR FormatPhoneNumber(CInternationalInformationDll& Dll, LPCSTR pszIniFile, CInternationalData::PhoneNumber* pData);
	LPCSTR FormatMailAddress(CInternationalInformationDll& Dll, LPCSTR pszIniFile, CInternationalData::MailAddress* pData);
};

#endif
