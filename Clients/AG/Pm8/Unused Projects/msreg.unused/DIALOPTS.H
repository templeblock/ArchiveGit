/*
// $Workfile: DIALOPTS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/DIALOPTS.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   26 Jun 1996 07:03:26   FRED
// International Registration
// 
//    Rev 1.1   05 Dec 1995 13:02:32   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:51:54   FRED
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:50   FRED
//  
*/ 

// dialopts.h : header file
//

class CDialingOptionsDialog;

#ifndef __DIALOPTS_H__
#define __DIALOPTS_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDialingOptionsDialog dialog

class CDialingOptionsDialog : public CBaseDialog
{
// Construction
public:
	CDialingOptionsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialingOptionsDialog)
	enum { IDD = IDD_DIALING_OPTIONS };
	CString	m_csPrefix;
	BOOL	m_fDial1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialingOptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialingOptionsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangePrefix();
	afx_msg void OnDial1();
	afx_msg void OnDoubleclickedDial1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void RedrawDialingNumber(void);
};

#endif
