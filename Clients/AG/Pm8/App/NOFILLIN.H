/*
// $Workfile: NOFILLIN.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/NOFILLIN.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:29:18   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:08   FRED
// Initial revision.
// 
//    Rev 1.0   23 Aug 1995 07:41:30   FRED
// Initial revision.
*/

class CNoFillInFieldsDialog;

#ifndef __NOFILLIN_H__
#define __NOFILLIN_H__

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CNoFillInFieldsDialog dialog

class CNoFillInFieldsDialog : public CPmwDialog
{
// Construction
public:
	CNoFillInFieldsDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNoFillInFieldsDialog)
	enum { IDD = IDD_NO_FILL_IN_FIELDS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CNoFillInFieldsDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
