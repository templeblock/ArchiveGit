/*
// $Workfile: REGISTER.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/REGISTER.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.4   13 Dec 1995 12:34:36   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.3   12 Dec 1995 17:56:30   FRED
// Modem Only Version
// 
//    Rev 1.2   05 Dec 1995 13:02:32   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.1   28 Nov 1995 08:47:08   FRED
// Phone Switch
// 
//    Rev 1.0   22 Nov 1995 08:51:52   FRED
// Initial revision.
// 
//    Rev 1.3   01 Nov 1995 11:55:18   FRED
// Added Register by Phone
// 
//    Rev 1.2   26 Oct 1995 10:27:42   FRED
//  
*/ 

// register.h : header file
//

class CRegisterDialog;

#ifndef __REGISTER_H__
#define __REGISTER_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CRegisterDialog dialog

class CRegisterDialog : public CBaseDialog
{
// Construction
public:
	CRegisterDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterDialog)
	enum { IDD = IDD_REGISTER_EPM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	static UINT DialogIds[];

	static int DialogId(void)
		{ return GetApp()->PickDialog(DialogIds); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnRegisterNow();
	afx_msg void OnRegisterLater();
	afx_msg void OnRegisterByMail();
	afx_msg void OnRegisterByPhone();
	afx_msg void OnCantRegister();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
