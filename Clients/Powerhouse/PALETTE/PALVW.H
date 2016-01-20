// palvw.h : interface of the CPaletteView class
//
/////////////////////////////////////////////////////////////////////////////

class CPaletteView : public CView
{
protected: // create from serialization only
	CPaletteView();
	DECLARE_DYNCREATE(CPaletteView)

// Attributes
public:
	CPaletteDoc* GetDocument();

// Operations
public:

// Implementation
public:
	virtual ~CPaletteView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual void OnUpdate(CView* pSender,
                          LPARAM lHint,
                          CObject* pHint);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CPaletteView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG	// debug version in palvw.cpp
inline CPaletteDoc* CPaletteView::GetDocument()
   { return (CPaletteDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
