#if !defined(AFX_TYPECMBO_H__21371141_C234_11D1_8680_0060089672BE__INCLUDED_)
#define AFX_TYPECMBO_H__21371141_C234_11D1_8680_0060089672BE__INCLUDED_

#include "pmw.h"
#include "barcombo.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TypeCmbo.h : header file
//

#define MAX_FONTS_MRU	(6)

class CFontPreviewWnd;
/////////////////////////////////////////////////////////////////////////////
// CTypefaceCombo window

class CTypefaceCombo : public CBarMRUCombo
{
	INHERIT(CTypefaceCombo, CBarMRUCombo)

	// Preview window creation
	int m_nTimer;
	BOOL m_bShowPreview;
	DRAWITEMSTRUCT m_DrawItemStruct;	// info on list item to be previewed

	// Preview window
	CFontPreviewWnd* m_pPreviewWnd;

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
	void DestroyPreview();
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
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CFontPreviewWnd


class CFontPreviewWnd : public CWnd
{
	int m_nFontID;

// Construction
public:
	CFontPreviewWnd(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(CFontPreviewWnd)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontPreviewWnd)
	//}}AFX_VIRTUAL

	void SetFontID(int nFont)
	{
		m_nFontID = nFont;
	}
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFontPreviewWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TYPECMBO_H__21371141_C234_11D1_8680_0060089672BE__INCLUDED_)
