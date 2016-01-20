/*
// $Workfile: CALLFAIL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/CALLFAIL.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.4   13 Dec 1995 12:34:40   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.3   12 Dec 1995 17:56:30   FRED
// Modem Only Version
// 
//    Rev 1.2   05 Dec 1995 13:02:32   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.1   28 Nov 1995 08:46:30   FRED
// Phone Switch
// 
//    Rev 1.0   22 Nov 1995 08:51:54   FRED
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:52   FRED
//  
*/ 

// callfail.h : header file
//

class CCallFailedDialog;

#ifndef __CALLFAIL_H__
#define __CALLFAIL_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCallFailedDialog dialog

class CCallFailedDialog : public CBaseDialog
{
// Construction
public:
	CCallFailedDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCallFailedDialog)
	enum { IDD = IDD_CALL_FAILED_EPM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	static UINT DialogIds[];

	static int DialogId(void)
		{ return GetApp()->PickDialog(DialogIds); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCallFailedDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCallFailedDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
