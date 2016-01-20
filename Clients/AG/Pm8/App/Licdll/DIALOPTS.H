/*
// $Workfile: DIALOPTS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/DIALOPTS.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:20   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:36   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/DIALOPTS.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.1   14 Oct 1996 13:34:46   Fred
// International Version
// 
//    Rev 1.0   08 Sep 1996 09:40:48   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:48   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:44   JAY
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
