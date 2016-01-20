/*
// $Workfile: CANTREG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/CANTREG.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   12 Dec 1995 17:57:58   FRED
// Initial revision.
*/ 

// cantreg.h : header file
//

class CCantRegisterDialog;

#ifndef __CANTREG_H__
#define __CANTREG_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCantRegisterDialog dialog

class CCantRegisterDialog : public CBaseDialog
{
// Construction
public:
	CCantRegisterDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCantRegisterDialog)
	enum { IDD = IDD_CANT_REGISTER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCantRegisterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCantRegisterDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
