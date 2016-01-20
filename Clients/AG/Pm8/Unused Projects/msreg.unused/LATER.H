/*
// $Workfile: LATER.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/LATER.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   05 Dec 1995 13:02:32   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:51:52   FRED
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:42   FRED
//  
*/ 

// later.h : header file
//

class CRegisterLaterDialog;

#ifndef __LATER_H__
#define __LATER_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CRegisterLaterDialog dialog

class CRegisterLaterDialog : public CBaseDialog
{
// Construction
public:
	CRegisterLaterDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterLaterDialog)
	enum { IDD = IDD_REGISTER_LATER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterLaterDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterLaterDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
