// banndoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBannerDoc document

class CBannerDoc : public CPmwDoc
{
	DECLARE_SERIAL(CBannerDoc)
protected:
	CBannerDoc();			// protected constructor used by dynamic creation

	CFrameObject* banner_frame;

// Attributes
public:

	static ProjectInfo StandardInfo;

// Operations
public:

	BOOL OnNewDocument();
	BOOL OnOpenDocument(const char *pszPathName);


	virtual CFrameObject* get_main_frame(VOID);
	virtual BOOL resize_project(CFrameObject* frame = NULL);

	virtual PMGPageObject* PasteBackground(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds, BOOL fDuplicate = TRUE);

	// Move to next or previous object.
	virtual void SelectNextObject(void);
	virtual BOOL CanSelectNextObject(void);
	virtual void SelectPreviousObject(void);
	virtual BOOL CanSelectPreviousObject(void);

// Implementation
protected:
	virtual ~CBannerDoc();
	// Generated message map functions
protected:
	//{{AFX_MSG(CBannerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

