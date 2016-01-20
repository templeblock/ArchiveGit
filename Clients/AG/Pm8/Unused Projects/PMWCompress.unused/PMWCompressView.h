// PMWCompressView.h : interface of the CPMWCompressView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PMWCOMPRESSVIEW_H__2A823AED_C7DE_11D1_8680_0060089672BE__INCLUDED_)
#define AFX_PMWCOMPRESSVIEW_H__2A823AED_C7DE_11D1_8680_0060089672BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "filcmprs.h"

class CPMWCompressView : public CScrollView
{
	// The file compression info
	CFileCompress m_FileCompress;

	// Text display
	int m_nDrop;
	CPoint m_TextLoc;
	CString m_csComplete;

protected: // create from serialization only
	CPMWCompressView();
	DECLARE_DYNCREATE(CPMWCompressView)

// Attributes
public:
	CPMWCompressDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPMWCompressView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPMWCompressView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL DoFileDialog(enum OperateType operation);
	void PressFiles();

// Generated message map functions
protected:
	//{{AFX_MSG(CPMWCompressView)
	afx_msg void OnCompress();
	afx_msg void OnDecompress();
	afx_msg void OnWriteHeader();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PMWCompressView.cpp
inline CPMWCompressDoc* CPMWCompressView::GetDocument()
   { return (CPMWCompressDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PMWCOMPRESSVIEW_H__2A823AED_C7DE_11D1_8680_0060089672BE__INCLUDED_)
