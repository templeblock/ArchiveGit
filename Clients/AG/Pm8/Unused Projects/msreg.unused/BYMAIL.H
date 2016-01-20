/*
// $Workfile: BYMAIL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/BYMAIL.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.3   26 Jun 1996 07:05:20   FRED
// International Registration
// 
//    Rev 1.2   11 Mar 1996 15:34:34   FRED
// Printed registration form
// 
//    Rev 1.1   05 Dec 1995 13:02:30   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:51:52   FRED
// Initial revision.
// 
//    Rev 1.2   01 Nov 1995 11:55:18   FRED
// Added Register by Phone
// 
//    Rev 1.1   26 Oct 1995 10:27:42   FRED
//  
*/ 

// bymail.h : header file
//

class CRegisterByMailFormDialog;
class CSendFormDialog;

#ifndef __BYMAIL_H__
#define __BYMAIL_H__

#include "reginfo.h"
#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CRegisterByMailFormDialog dialog

class CRegisterByMailFormDialog : public CRegistrationInformationDialog
{
// Construction
public:
	CRegisterByMailFormDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterByMailFormDialog)
	enum { IDD = IDD_REGISTER_BY_MAIL_EPMF };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterByMailFormDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterByMailFormDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPrintForm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual int ReportMissingRequiredFields(LPCSTR pszRequiredFields);
	virtual int ReportInvalidPhoneNumber(void);

	BOOL PrintRegistrationForm(void);
};

/////////////////////////////////////////////////////////////////////////////
// CSendFormDialog dialog

class CSendFormDialog : public CBaseDialog
{
// Construction
public:
	CSendFormDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSendFormDialog)
	enum { IDD = IDD_SEND_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendFormDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendFormDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
