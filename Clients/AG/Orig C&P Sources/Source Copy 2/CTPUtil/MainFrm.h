// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__64AE5209_3509_11D3_9331_0080C6F796A1__INCLUDED_)
#define AFX_MAINFRM_H__64AE5209_3509_11D3_9331_0080C6F796A1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AGPage.h"

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void CreateThumb(const CAGPage *pPage, int nMaxSize,
	  const char *pszFileName);
	void WriteBMP(const char *pszFileName, const BITMAPINFO *pbmi,
	  const BYTE *pBits);

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnBatchPrint();
	afx_msg void OnBuildThumbs();
	afx_msg void OnCardList();
	afx_msg void OnPrintOptions();
	afx_msg void OnUpdatePrintBoth(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrintDefault(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrintQuarter(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrintSingle(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int m_nPrintOption;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__64AE5209_3509_11D3_9331_0080C6F796A1__INCLUDED_)
