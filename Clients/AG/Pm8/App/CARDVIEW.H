// cardview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardView view

class CCardDoc;

class CCardView : public CPmwView
{
	DECLARE_DYNCREATE(CCardView)
protected:
	CCardView();			// protected constructor used by dynamic creation

	VOID check_card_panel(CCmdUI* pCmdUI, SHORT panel);

// Attributes

public:

	virtual BOOL CanInsertPictures(void);
	int PacificTimeCorrection(short zone);
	BOOL ConvertToPST(tm* when, int timecorrection);
	
// Operations
public:

	VOID set_card_panel_view(SHORT panel);		// Adds command as well

	virtual void SetPanel(int nPanel, BOOL fUpdateMenu = TRUE);

	CCardDoc *GetCardDocument()
		{ return (CCardDoc *)GetDocument(); }

public:
	// Gadget interface.
	virtual BOOL GadgetVisible(UINT uID);
	virtual BOOL GadgetEnabled(UINT uID, UINT uPart);
	virtual void GadgetClicked(UINT uID, UINT uPart, CWnd* pGadget);
	virtual void GadgetText(UINT uID, CString& csText);
	virtual LPCSTR GadgetMenuName(UINT uID, UINT uPart);

	// Sentiment support.
	virtual void PasteSentiment(const CMlsStringArray& csaSentiment);

// Implementation
protected:
	virtual ~CCardView();
	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();
	virtual void CreateEditDesktop(void);
	BOOL CheckFonts(CCardDoc* pDoc);

	BOOL DumpPanel(int nPanel, LPCSTR pszGIFName, POINT MaxDims);
	UINT CheckForValidConnection();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCardView)
	afx_msg void OnCardViewback();
	afx_msg void OnUpdateCardViewback(CCmdUI* pCmdUI);
	afx_msg void OnCardViewcard();
	afx_msg void OnUpdateCardViewcard(CCmdUI* pCmdUI);
	afx_msg void OnCardViewfront();
	afx_msg void OnUpdateCardViewfront(CCmdUI* pCmdUI);
	afx_msg void OnCardViewinside();
	afx_msg void OnUpdateCardViewinside(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBackgroundSelect(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePicturesSelect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTextSelect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPasteSpecial(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInsertObject(CCmdUI* pCmdUI);
	afx_msg void OnCardServer();
	afx_msg void OnUpdateCardServer(CCmdUI* pCmdUI);
	afx_msg void OnPersonalDelivery();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CHalfCardView

class CHalfCardView : public CCardView
{
	DECLARE_DYNCREATE(CHalfCardView)
protected:
	CHalfCardView();			// protected constructor used by dynamic creation
// Attributes

public:

// Operations
public:
// Implementation
	virtual SHORT OnPrintDocument(PrintContext &pc,
							int32 aCurrentDocument, int32 aNumDocuments);
	virtual CPmwPrint* CreatePrintDialog(void);

protected:
	virtual ~CHalfCardView();
	// Generated message map functions
protected:
	//{{AFX_MSG(CHalfCardView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CNoteCardView

class CNoteCardView : public CCardView
{
	DECLARE_DYNCREATE(CNoteCardView)
protected:
	CNoteCardView();			// protected constructor used by dynamic creation
// Attributes

public:

// Operations
public:
// Implementation
protected:
	virtual ~CNoteCardView();
	// Generated message map functions
protected:
	//{{AFX_MSG(CNoteCardView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
