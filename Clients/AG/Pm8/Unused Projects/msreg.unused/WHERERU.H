/*
// $Workfile: WHERERU.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright � 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/WHERERU.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:28p Jayn
// 
//    Rev 1.1   04 Oct 1996 15:31:06   FRED
// Uses international DLL (INTLxx.DLL)
// 
//    Rev 1.0   30 Aug 1996 07:55:14   FRED
// Initial revision.
*/ 

class CWhereAreYouDialog;

#ifndef __WHERERU_H__
#define __WHERERU_H__

#include "reginfo.h"
#include "intl\intldll.h"
#include "basedlg.h"

/////////////////////////////////////////////////////////////////////////////
// CWhereAreYouDialog dialog

class CWhereAreYouDialog : public CBaseDialog
{
// Construction
public:
	CWhereAreYouDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWhereAreYouDialog)
	enum { IDD = IDD_WHERE_ARE_YOU };
	//}}AFX_DATA
	CInternationalData* m_pInternationalData;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWhereAreYouDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWhereAreYouDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CCountryList m_CountryList;
};

#endif
