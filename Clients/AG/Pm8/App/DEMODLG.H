/*
// $Workfile: DEMODLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DEMODLG.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:32   Fred
// Initial revision.
// 
//    Rev 1.1   15 May 1996 08:45:50   Jay
// From PMW2
// 
//    Rev 1.1   13 May 1996 13:16:04   FRED
// Trial Version
// 
//    Rev 1.0   25 Jan 1996 09:20:16   JAY
//  
*/ 

class CDemoVersionDialog;

#ifndef __DEMODLG_H__
#define __DEMODLG_H__

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDemoVersionDialog dialog

class CDemoVersionDialog : public CPmwDialog
{
// Construction
public:
	CDemoVersionDialog(LPCSTR lpszMessage, LPCSTR lpszTitle, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDemoVersionDialog)
	enum { IDD = IDD_DEMO_VERSION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CString m_csMessage;
	CString m_csTitle;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CDemoVersionDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
