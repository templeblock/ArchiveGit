/*
// $Workfile: BASEDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/BASEDLG.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:35:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:10   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:12   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
//    Rev 1.0   08 Sep 1996 09:40:42   Fred
// Initial revision.
// 
//    Rev 1.1   02 Feb 1996 13:44:02   FRED
// Added palette support to bitmap control
// 
//    Rev 1.0   02 Feb 1996 08:59:10   JAY
// Initial revision.
// 
//    Rev 1.0   05 Dec 1995 13:03:06   JAY
// Initial revision.
*/

#ifndef __BASEDLG_H__
#define __BASEDLG_H__

/////////////////////////////////////////////////////////////////////////////
// CBaseDialog dialog

class CBaseDialog : public CDialog
{
// Construction
public:
	CBaseDialog(UINT nID, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBaseDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseDialog)
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBaseDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	//}}AFX_MSG
	afx_msg LRESULT OnHelpHitTest(WPARAM, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	UINT m_nID;
};
#endif
