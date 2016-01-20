// calview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCalendarView view

class CCalendarView : public CPmwView
{
	DECLARE_DYNCREATE(CCalendarView)
protected:
	CCalendarView();			// protected constructor used by dynamic creation
	
	SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);

// Attributes
public:

public:
	// Gadget interface.
	virtual BOOL GadgetVisible(UINT uID);
	virtual void GadgetText(UINT uID, CString& csText);
	virtual LPCSTR GadgetMenuName(UINT uID, UINT uPart);
// Operations
public:

// Implementation
protected:
	virtual ~CCalendarView();
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view
	virtual void CreateEditDesktop(void);
	virtual CPmwPrint* CreatePrintDialog(void);

	virtual int PrintCurrentPage(PrintContext& pc, int nPage, BOOL fMakeBlank = FALSE);
	virtual int PrintSingleSided(int nFromPage, int nToPage, PrintContext& pc);
	virtual int PrintDocumentForward(PrintContext& pc, int nSide);
	virtual int PrintDocumentBackward(PrintContext& pc, int nSide);

	virtual int PrintPreviewPageCount(void);
	virtual int SetPrintPreviewPage(int nPage);
	// Generated message map functions
protected:
	//{{AFX_MSG(CCalendarView)
		// NOTE - the ClassWizard will add and remove member functions here.
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
