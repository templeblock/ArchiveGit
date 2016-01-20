#if !defined(AFX_TYPECMBO_H__21371141_C234_11D1_8680_0060089672BE__INCLUDED_)
#define AFX_TYPECMBO_H__21371141_C234_11D1_8680_0060089672BE__INCLUDED_

#include "barcombo.h"
#include "..\..\app\Toolbar.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TypeCmbo.h : header file
//

#define MAX_FONTS_MRU	(6)

//class CFontPreviewWnd;
/////////////////////////////////////////////////////////////////////////////
// CTypefaceCombo window

class CTypefaceCombo : public CBarMRUCombo
{
	INHERIT(CTypefaceCombo, CBarMRUCombo)

	// Preview window creation
	int m_nTimer;
	BOOL m_bShowPreview;
	DRAWITEMSTRUCT m_DrawItemStruct;	// info on list item to be previewed

// Construction
public:
	CTypefaceCombo();
	void BuildList();

// Attributes
public:

// Operations
public:
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void ShowPreview();
	void HidePreview();
	void MovePreview();
	void UpdatePreview();
	void OnKillfocus();
	virtual void UpdateMRU();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTypefaceCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTypefaceCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTypefaceCombo)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnCloseup();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CWnd* m_pMainWnd;
	SFontComboInfo m_ComboInfo;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TYPECMBO_H__21371141_C234_11D1_8680_0060089672BE__INCLUDED_)
