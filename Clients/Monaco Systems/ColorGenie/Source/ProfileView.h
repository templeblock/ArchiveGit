// ProfileView.h : interface of the CProfileView class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef PROFILE_VIEW_H
#define PROFILE_VIEW_H

#include "ProfileSheet.h"

class CProfileView : public CView
{
protected: // create from serialization only
	CProfileView();
	DECLARE_DYNCREATE(CProfileView)

public:
	//{{AFX_DATA(CProfileView)
		// NOTE: the ClassWizard will add data members here
	HWND m_hWndFocus;
	//}}AFX_DATA

// Attributes
public:
	CProfileDoc* GetDocument();
	BOOL SaveFocusControl();
	inline CProfileSheet* GetPropertySheet(void) { return m_pPropSheet; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProfileView)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnActivateFrame(UINT nState, CFrameWnd* /*pFrameWnd*/);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CProfileView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CPropertyPage* m_pPageCalibrate;

protected:

	// property sheet is wired to MDI child frame and is not displayed
	CProfileSheet* m_pPropSheet;

	// one page for each menu so we can initialize controls
	// using OnInitDialog
	CPropertyPage* m_pPageToneAdjust;
	CPropertyPage* m_pPageColorData;
	CPropertyPage* m_pPageSettings;
	CPropertyPage* m_pPageTuning;

// Generated message map functions
protected:
	//{{AFX_MSG(CProfileView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ProfileView.cpp
inline CProfileDoc* CProfileView::GetDocument()
   { return (CProfileDoc*)m_pDocument; }
#endif

#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
