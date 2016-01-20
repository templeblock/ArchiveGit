// paldoc.h : interface of the CPaletteDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CPaletteDoc : public CDocument
{
protected: // create from serialization only
	CPaletteDoc();
	DECLARE_DYNCREATE(CPaletteDoc)

// Attributes
public:

// Operations
public:
	CDIBPal* GetPalette()
		{return m_pPal;}

// Implementation
public:
	virtual ~CPaletteDoc();
	virtual void Serialize(CArchive& ar);	// overridden for document i/o
#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext& dc) const;
#endif
protected:
	virtual	BOOL	OnNewDocument();

private:
	CDIBPal* m_pPal;

// Generated message map functions
protected:
	//{{AFX_MSG(CPaletteDoc)
	afx_msg void OnFileFromDIB();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
