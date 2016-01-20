/*
// $Workfile: LINEWID.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LINEWID.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:38:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:44   FRED
// Initial revision.
// 
//    Rev 1.2   28 Aug 1995 15:26:02   FRED
// None/NoNone cases for line width selection.
// 
//    Rev 1.1   19 Aug 1995 17:49:26   FRED
// Maximum line sizes
// 
//    Rev 1.0   11 May 1995 08:52:08   FRED
// Initial revision.
*/ 

class CChooseLineWidthDialog;

#ifndef _LINEWID_H_
#define _LINEWID_H_

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CChooseLineWidthDialog dialog

class CChooseLineWidthDialog : public CPmwDialog
{
// Construction
public:
	CChooseLineWidthDialog(int nLineWidth, int nMaximumLineWidth, BOOL fAllowNone = TRUE, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CChooseLineWidthDialog)
	enum { IDD = IDD_CHOOSE_LINE };
	CEdit	m_LineWidthEdit;
	//}}AFX_DATA

	int m_nLineWidth;
	int m_nMaximumLineWidth;
	BOOL m_fAllowNone;
	
protected:
	BOOL m_fLineWidthChanged;
	
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CChooseLineWidthDialog)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual void OnOK();
	afx_msg void OnChangeLineWidth();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusLineWidth();
	afx_msg LRESULT OnValidateLineWidth(WPARAM wParam, LPARAM lParam);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnSetfocusLineWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
protected:
	void Width(int nWidth);
};

#endif
