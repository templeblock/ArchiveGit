// spview.h : interface of the CShowPalView class
//
/////////////////////////////////////////////////////////////////////////////

class CShowPalView : public CView
{
protected: // create from serialization only
	CShowPalView();
	DECLARE_DYNCREATE(CShowPalView)

// Attributes
public:
	CShowPalDoc* GetDocument();

// Operations
public:

// Implementation
public:
	virtual ~CShowPalView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CShowPalView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG	// debug version in spview.cpp
inline CShowPalDoc* CShowPalView::GetDocument()
   { return (CShowPalDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
