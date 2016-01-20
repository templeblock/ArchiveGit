/*
// $Workfile: PHONENUM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PHONENUM.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:52   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:41:00   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:50   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:46   JAY
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:52   FRED
//  
*/ 

// phonenum.h : header file
//

class CPhoneNumberDialog;

#ifndef __PHONENUM_H__
#define __PHONENUM_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CPhoneNumberDialog dialog

class CPhoneNumberDialog : public CBaseDialog
{
// Construction
public:
	CPhoneNumberDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPhoneNumberDialog)
	enum { IDD = IDD_PHONE_NUMBER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhoneNumberDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPhoneNumberDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
