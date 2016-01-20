/*
// $Workfile: PHONENUM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/PHONENUM.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.5   26 Jun 1996 07:04:08   FRED
// International Registration
// 
//    Rev 1.4   11 Mar 1996 15:34:24   FRED
// Printed registration form
// 
//    Rev 1.3   13 Dec 1995 12:34:34   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.2   12 Dec 1995 17:55:34   FRED
// Modem Only Version
// 
//    Rev 1.1   05 Dec 1995 13:02:32   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:51:56   FRED
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
