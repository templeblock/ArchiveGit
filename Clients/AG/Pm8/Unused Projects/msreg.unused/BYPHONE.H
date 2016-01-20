/*
// $Workfile: BYPHONE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/BYPHONE.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   05 Dec 1995 13:02:34   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:52:00   FRED
// Initial revision.
// 
//    Rev 1.0   01 Nov 1995 11:55:24   FRED
// Added Register by Phone
*/ 

// byphone.h : header file
//

class CRegisterByPhoneDialog;

#ifndef __BYPHONE_H__
#define __BYPHONE_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CRegisterByPhoneDialog dialog

class CRegisterByPhoneDialog : public CBaseDialog
{
// Construction
public:
	CRegisterByPhoneDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterByPhoneDialog)
	enum { IDD = IDD_REGISTER_BY_PHONE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterByPhoneDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterByPhoneDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
