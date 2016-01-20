/*
// $Workfile: UPGRADLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADLG.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:24   Fred
// Initial revision.
// 
//    Rev 1.1   15 May 1996 08:46:48   Jay
// From PMW2
// 
//    Rev 1.2   13 May 1996 13:15:52   FRED
// Trial Version
// 
//    Rev 1.1   21 Feb 1996 11:18:18   FRED
// Upgrade dialogs which explain feature deltas.
// 
//    Rev 1.0   12 Feb 1996 09:45:26   FRED
//  
*/ 

class CNeedToUpgradeDialog;

#ifndef __UPGRADLG_H__
#define __UPGRADLG_H__

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CNeedToUpgradeDialog dialog

class CNeedToUpgradeDialog : public CPmwDialog
{
// Construction
public:
	CNeedToUpgradeDialog(LPCSTR pszMessage, LPCSTR pszTitle, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNeedToUpgradeDialog)
	enum { IDD = IDD_NEED_TO_UPGRADE_FROM_CLASSIC };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CString m_csMessage;
	CString m_csTitle;

	UINT DialogId(LPCSTR pszMessage);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNeedToUpgradeDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
