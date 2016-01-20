// bannview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBannerView view

class CBannerView : public CPmwView
{
	DECLARE_DYNCREATE(CBannerView)
protected:
	CBannerView();			// protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Implementation
protected:
	virtual ~CBannerView();
	virtual void OnDraw(CDC* pDC);		// overridden to draw this view
	virtual void OnDraw(PrintContext* pPC);  // overridden to print this view 
	virtual void CreateEditDesktop(void);

	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

	virtual int PrintPreviewPageCount(void);

   virtual void OnInitialUpdate();

public:
	virtual int SetPrintPreviewPage(int nPage);
	// Generated message map functions
protected:
	//{{AFX_MSG(CBannerView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	int m_nPreviewPage;
};

/////////////////////////////////////////////////////////////////////////////
