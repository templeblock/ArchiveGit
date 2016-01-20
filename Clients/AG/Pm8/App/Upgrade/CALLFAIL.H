/*
// $Workfile: CALLFAIL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/CALLFAIL.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:32   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:44   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:44   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:46   JAY
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
	enum { IDD = IDD_CALL_FAILED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


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
