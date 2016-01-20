// $Header: /PM8/Toolbars/PrintShop/BARCOMBO.H 1     3/03/99 6:35p Gbeddow $
//
// barcombo.h : Combo box and edit classes for use on dialog bar.
//
// $Log: /PM8/Toolbars/PrintShop/BARCOMBO.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
// 1     2/24/99 12:24p Ddusich
// Print Shop Toolbar DLL
// 
// 1     1/20/99 9:55a Mwilson
// 
// 1     12/11/98 11:04a Mwilson
// 
// 4     3/27/98 10:33a Rlovejoy
// Renamed UpdateMRU() function.
// 
// 3     3/25/98 12:07p Rlovejoy
// Shuffling of message handlers.
// 
// 2     3/24/98 3:22p Rlovejoy
// Changes for CBarMRUCombo class.
// 
//    Rev 1.0   14 Aug 1997 15:18:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:28:30   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:32:22   FRED
// Initial revision.
// 
//    Rev 1.1   29 Mar 1995 09:15:34   JAY
// Lots of changes
// 
//    Rev 1.0   15 Mar 1995 11:06:42   JAY
// Initial revision.
// 

#ifndef __BARCOMBO_H__
#define __BARCOMBO_H__

#include <afxadv.h>

class CBarEdit;
class CBarCombo;

/////////////////////////////////////////////////////////////////////////////
// CBarEdit window

class CBarEdit : public CEdit
{
// Construction
public:
	CBarEdit();

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CBarEdit();

protected:
	CBarCombo* ComboBox(void)
		{ return (CBarCombo*)GetParent(); }

	virtual WNDPROC* GetSuperWndProcAddr(void);

	// Generated message map functions
	//{{AFX_MSG(CBarEdit)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBarCombo window

class CBarCombo : public CComboBox
{
	INHERIT(CBarCombo, CComboBox)

// Construction
public:
	CBarCombo();

// Attributes
public:

	BOOL HasFocus(void);

// Operations
public:

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Implementation
public:
	virtual ~CBarCombo();

	BOOL EditKeyStroke(UINT nChar, UINT nRepCnt, UINT nFlags);

	void OnEditKillFocus(CWnd* pNewWnd);
	void OnEditSetFocus(CWnd* pOldWnd);

	CWnd* GetEdit(void)
		{ return &m_Edit; }

protected:
	CBarEdit m_Edit;

	HWND m_hOldWnd;
	CString m_SavedState;
	BOOL m_fAccepted;

	virtual void SaveState(void);								// At "gain focus" time.
	virtual void AbortState(BOOL fSetFocus = TRUE);				// At "forget it" time.
	virtual void ConfirmState(void);							// At "accept" time.

	virtual WNDPROC* GetSuperWndProcAddr(void);

	// Generated message map functions
	//{{AFX_MSG(CBarCombo)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg void OnSelChange();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CBarMRUCombo window

class CBarMRUCombo : public CBarCombo
{
	INHERIT(CBarMRUCombo, CBarCombo)
	int m_nCurSel;

// Construction
public:
	CBarMRUCombo(UINT nStart, LPCTSTR lpszSection,
		LPCTSTR lpszEntryFormat, int nSize,
		int nMaxDispLen = AFX_ABBREV_FILENAME_LEN);

// Attributes
public:

// Operations
public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void UpdateMRU();

// Implementation
public:
	virtual CBarMRUCombo::~CBarMRUCombo();

protected:
	CRecentFileList m_RecentItemList;
	int m_nMRUItems;

	// Generated message map functions
	//{{AFX_MSG(CBarMRUCombo)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif					// #ifndef __BARCOMBO_H__
