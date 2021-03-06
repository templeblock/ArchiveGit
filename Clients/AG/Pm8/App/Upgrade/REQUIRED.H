/*
// $Workfile: REQUIRED.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright � 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/REQUIRED.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:56   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:41:04   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:52   JAY
// Initial revision.
// 
//    Rev 1.1   30 Jan 1996 08:10:06   FRED
// Lists only empty required fields.
// Valdiates credit card numbers.
// 
// 
//    Rev 1.0   29 Nov 1995 10:27:46   JAY
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
	CRequiredInformationDialog(LPCSTR pszRequiredFieldNames, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRequiredInformationDialog)
	enum { IDD = IDD_REQUIRED_INFORMATION };
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
