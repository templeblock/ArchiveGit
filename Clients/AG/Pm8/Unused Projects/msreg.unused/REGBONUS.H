/*
// $Workfile: REGBONUS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/REGBONUS.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   13 Dec 1995 12:35:20   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.1   05 Dec 1995 13:02:34   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 10:26:48   FRED
// Initial revision.
// 
//    Rev 1.1   14 Nov 1995 09:40:12   JAY
// Base dialog supporting events.
*/ 

#ifndef __REGBONUS_H__
#define __REGBONUS_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CRegisterForBonusDialog dialog

class CRegisterForBonusDialog : public CBaseDialog
{
// Construction
public:
	CRegisterForBonusDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRegisterForBonusDialog)
	enum { IDD = IDD_REGISTER_FOR_BONUS_EPM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegisterForBonusDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRegisterForBonusDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnRegisterNow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
