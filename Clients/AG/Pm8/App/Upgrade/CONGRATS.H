/*
// $Workfile: CONGRATS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/CONGRATS.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     9/01/98 12:22p Dennis
// Change Congratulations dialog to get product name from msreg.ini
// 
//    Rev 1.0   14 Aug 1997 15:41:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:34   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:46   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:46   JAY
// Initial revision.
// 
//    Rev 1.0   30 Nov 1995 15:58:32   JAY
// Initial revision.
// 
//    Rev 1.0   22 Nov 1995 08:51:52   FRED
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:42   FRED
//  
*/ 

// congrats.h : header file
//

class CCongratulationsDialog;

#ifndef __CONGRATS_H__
#define __CONGRATS_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CCongratulationsDialog dialog

class CCongratulationsDialog : public CBaseDialog
{
// Construction
public:
	CCongratulationsDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCongratulationsDialog)
	enum { IDD = IDD_CONGRATULATIONS };
	CString	m_csCongrats;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCongratulationsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCongratulationsDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
