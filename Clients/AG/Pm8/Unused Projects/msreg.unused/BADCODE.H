/*
// $Workfile: BADCODE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/BADCODE.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.3   04 Oct 1996 15:29:50   FRED
// Uses international DLL (INTLxx.DLL)
// 
//    Rev 1.2   30 Aug 1996 09:19:36   FRED
// Fixed bad reg code crash
// 
//    Rev 1.1   05 Dec 1995 13:02:34   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 10:26:48   FRED
// Initial revision.
// 
//    Rev 1.1   14 Nov 1995 09:40:12   JAY
// Base dialog supporting events.
*/ 

#ifndef __BADCODE_H__
#define __BADCODE_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CBadRegistrationCodeDialog dialog

class CBadRegistrationCodeDialog : public CBaseDialog
{
// Construction
public:
	CBadRegistrationCodeDialog(CWnd* pParent = NULL);   // standard constructor

	UINT DialogId(void)
		{ return GetApp()->GetInternationalData() != NULL ? IDD_BAD_REGISTRATION_CODE : IDD_BAD_REGISTRATION_CODE_NO_PHONE; }

// Dialog Data
	//{{AFX_DATA(CBadRegistrationCodeDialog)
	enum { IDD = IDD_BAD_REGISTRATION_CODE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBadRegistrationCodeDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBadRegistrationCodeDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
