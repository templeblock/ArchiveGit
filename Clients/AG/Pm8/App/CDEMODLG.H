/*
// $Workfile: CDEMODLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CDEMODLG.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:00   Fred
// Initial revision.
// 
//    Rev 1.0   11 Apr 1996 13:15:00   FRED
// Initial revision.
// 
//    Rev 1.0   26 Mar 1996 12:29:02   FRED
// Demo dialog double-click on icon brings up catalog
*/

#ifndef __CDEMODLG_H__
#define __CDEMODLG_H__

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CDemoDescriptionDialog dialog

class CDemoDescriptionDialog : public CPmwDialog
{
// Construction
public:
	CDemoDescriptionDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDemoDescriptionDialog)
	enum { IDD = IDD_DEMO_DESCRIPTION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDemoDescriptionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDemoDescriptionDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
