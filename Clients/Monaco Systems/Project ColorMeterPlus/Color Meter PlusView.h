// Color Meter PlusView.h : interface of the CColorMeterPlusView class
//
/////////////////////////////////////////////////////////////////////////////

class CColorMeterPlusView : public CView
{
protected: // create from serialization only
	CColorMeterPlusView();
	DECLARE_DYNCREATE(CColorMeterPlusView)

// Attributes
public:
	CColorMeterPlusDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorMeterPlusView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorMeterPlusView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CColorMeterPlusView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Color Meter PlusView.cpp
inline CColorMeterPlusDoc* CColorMeterPlusView::GetDocument()
   { return (CColorMeterPlusDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
