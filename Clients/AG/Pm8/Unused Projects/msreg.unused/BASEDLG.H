/*
// $Workfile: BASEDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/BASEDLG.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   02 Feb 1996 09:51:52   FRED
// Palette Support
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
