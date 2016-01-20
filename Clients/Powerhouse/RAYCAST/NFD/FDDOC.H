// fddoc.h : interface of the CFdDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CFdDoc : public CDocument
{
protected: // create from serialization only
	CFdDoc();
	DECLARE_DYNCREATE(CFdDoc)

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CFdDoc();
	virtual void Serialize(CArchive& ar);	// overridden for document i/o
	virtual BOOL OnOpenDocument(const char* pszPathName);
	virtual BOOL OnSaveDocument(const char* pszPathName);
#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext& dc) const;
#endif
protected:
	virtual	BOOL	OnNewDocument();

// Generated message map functions
protected:
	//{{AFX_MSG(CFdDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
