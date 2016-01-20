/*
// $Workfile: IATTDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/IATTDLG.H $
// 
// 1     3/03/99 6:06p Gbeddow
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

class CAttWorldNetServiceDialog;

#ifndef __IATTDLG_H__
#define __IATTDLG_H__

#include "ddb.h"

class CAttWorldNetServiceDialog : public CDialog
{
	INHERIT(CAttWorldNetServiceDialog, CDialog)

// Construction
public:
	CAttWorldNetServiceDialog(CWnd* pParent = NULL);   // standard constructor
	~CAttWorldNetServiceDialog();

// Dialog Data
	//{{AFX_DATA(CAttWorldNetServiceDialog)
	enum { IDD = IDD_ATTWORLDNETSERVICE };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttWorldNetServiceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAttWorldNetServiceDialog)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CDDB *m_pDDB;
};

#endif
