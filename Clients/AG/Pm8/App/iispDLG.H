/*
// $Workfile: iispDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/iispDLG.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     3/26/98 10:29a Fredf
// Changes to install program to install Internet Explorer 4.0 and the Web
// Publishing Wizard.
// 
// 1     3/25/98 2:45p Fredf
// 
//    Rev 1.0   14 Aug 1997 15:21:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:08   Fred
// Initial revision.
// 
//    Rev 1.0   20 Aug 1996 18:21:02   Fred
// AT&T Installation
*/

class CIspDialog;

#ifndef __IISPDLG_H__
#define __IISPDLG_H__

#include "ddb.h"

class CIspDialog : public CDialog
{
	INHERIT(CIspDialog, CDialog)

// Construction
public:
	CIspDialog(CWnd* pParent = NULL);   // standard constructor
	~CIspDialog();

// Dialog Data
	//{{AFX_DATA(CIspDialog)
	enum { IDD = IDD_INSTALL_ISP };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIspDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIspDialog)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CDDB *m_pDDB;
};

#endif
