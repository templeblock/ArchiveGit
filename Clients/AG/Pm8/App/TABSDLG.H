/*
// $Header: /PM8/App/TABSDLG.H 1     3/03/99 6:12p Gbeddow $
//
// Tabs dialog
//
// $Log: /PM8/App/TABSDLG.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:10   Fred
// Initial revision.
// 
//    Rev 1.2   21 Jun 1996 17:50:46   Jay
//  
// 
//    Rev 1.1   20 Jun 1996 15:39:48   Jay
// Now handles multiple paragraph tabs correctly.
// 
//    Rev 1.0   18 Jun 1996 16:16:16   Jay
// Initial revision.
*/

#ifndef __TABSDLG_H__
#define __TABSDLG_H__

#include "pmwdlg.h"
#include "text.h"

class CTabArray;

// Tabsdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabsDialog dialog

class CTabsDialog : public CPmwDialog
{
// Construction
public:
	CTabsDialog(CTabArray* pTabs, BOOL fMultiple, CWnd* pParent = NULL);   // standard constructor

	CTabArray* GetTabs(void);

// Dialog Data
	//{{AFX_DATA(CTabsDialog)
	enum { IDD = IDD_TABS };
	CListBox	m_TabStops;
	//}}AFX_DATA

	// Is this for multiple paragraphs?
	BOOL m_fMultiple;

	double	m_dDefaultTabSize;
	BOOL		m_fDefaultTabSize;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CTextTab m_NewTab;
	CTabArray m_Tabs;
	CDWordArray m_cdaTabStopsToClear;

	CTextTab* GetCurrentTab(void);
	void UpdateCurrentTab(BOOL fUpdatePosition);
	void UpdateTabList(void);
	void UpdateButtons(void);
	void AddTabStop(CTextTab* pTab);
	void SelectTab(CFixed Position, BOOL fUpdate = TRUE);
	CFixed GetPosition(BOOL* pfEmpty = NULL);

	void AddTabStopClear(CFixed Position);

	// Generated message map functions
	//{{AFX_MSG(CTabsDialog)
	afx_msg void OnClear();
	afx_msg void OnClearAll();
	afx_msg void OnSelchangeTabStops();
	virtual BOOL OnInitDialog();
	afx_msg void OnSetTab();
	afx_msg void OnTabAlignment();
	afx_msg void OnChangePosition();
	afx_msg void OnTabLeader();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif					// #ifndef __TABSDLG_H__
