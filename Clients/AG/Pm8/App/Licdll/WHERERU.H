/*
// $Workfile: WHERERU.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/WHERERU.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:46   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:41:04   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/WHERERU.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Oct 1996 13:35:22   Fred
// Initial revision.
// 
//    Rev 1.0   11 Oct 1996 15:33:16   FRED
// Initial revision.
*/ 

class CWhereAreYouDialog;

#ifndef __WHERERU_H__
#define __WHERERU_H__

#include "reginfo.h"
#include "\msreg\intl\intldll.h"
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
