/*
// $Workfile: REQUIRED.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/REQUIRED.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.6   26 Jun 1996 07:03:26   FRED
// International Registration
// 
//    Rev 1.5   11 Mar 1996 15:34:26   FRED
// Printed registration form
// 
//    Rev 1.4   29 Jan 1996 09:54:22   FRED
// Lists only required fields
// 
//    Rev 1.3   13 Dec 1995 12:34:36   FRED
// E, EM, EP, and EMP configurations
// 
//    Rev 1.2   12 Dec 1995 17:55:36   FRED
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

// required.h : header file
//

class CRequiredInformationDialog;

#ifndef __REQUIRED_H__
#define __REQUIRED_H__

#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CRequiredInformationDialog dialog

class CRequiredInformationDialog : public CBaseDialog
{
// Construction
public:
	CRequiredInformationDialog(const LPCSTR pszRequiredFieldNames, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRequiredInformationDialog)
	enum { IDD = IDD_REQUIRED_INFORMATION_EM_EPM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRequiredInformationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRequiredInformationDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CString m_csRequiredFieldNames;
};

#endif
