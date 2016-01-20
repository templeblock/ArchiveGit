/*
// $Workfile: SPELLDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPELLDLG.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:00   Fred
// Initial revision.
// 
//    Rev 1.3   01 Aug 1996 18:20:24   Jay
// Spell check highlight.
// 
//    Rev 1.2   01 Aug 1996 14:40:46   Jay
// Spelling fixes
// 
//    Rev 1.1   31 Jul 1996 18:22:02   Jay
// Spelling, etc.
// 
//    Rev 1.0   28 Jul 1996 15:09:02   Jay
//  
*/

class CSpellingDialog;

#ifndef __SPELLDLG_H__
#define __SPELLDLG_H__

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CSpellingDialog dialog

class CSpellingDialog : public CPmwDialog
{
// Construction
public:
	CSpellingDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpellingDialog)
	enum { IDD = IDD_SPELLING };
	BOOL	m_fIgnoreNumbers;
	BOOL	m_fIgnoreUpperCase;
	CString	m_csChangeTo;
	CString	m_csWord;
	//}}AFX_DATA

	LPCSTR m_pSuggestions;
	int m_nMaxWordSize;
	BOOL m_fValidateChangeTo;
	BOOL m_fChangedChangeTo;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpellingDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpellingDialog)
	afx_msg void OnIgnore();
	afx_msg void OnIgnoreAll();
	afx_msg void OnChange();
	afx_msg void OnChangeAll();
	afx_msg void OnAdd();
	afx_msg void OnSelchangeSuggestions();
	afx_msg void OnChangeChangeTo();
	virtual BOOL OnInitDialog();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDblclkSuggestions();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	BOOL m_fInnerSet;
	void OnExit(UINT uID, BOOL fValidateChangeTo);
	void UpdateButtons(void);
};

#endif
