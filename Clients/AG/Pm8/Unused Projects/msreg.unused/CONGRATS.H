/*
// $Workfile: CONGRATS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/CONGRATS.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   13 Dec 1995 12:35:20   FRED
// E, EM, EP, and EMP configurations
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

// congrats.h : header file
//

class CCongratulationsDialog;

#ifndef __CONGRATS_H__
#define __CONGRATS_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCongratulationsDialog dialog

class CCongratulationsDialog : public CBaseDialog
{
// Construction
public:
	CCongratulationsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCongratulationsDialog)
	enum { IDD = IDD_CONGRATULATIONS_EPM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	static UINT DialogIds[];

	static int DialogId(void)
		{ return GetApp()->PickDialog(DialogIds); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCongratulationsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCongratulationsDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
