// spdoc.h : interface of the CShowPalDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CShowPalDoc : public CDocument
{
protected: // create from serialization only
    CShowPalDoc();
    DECLARE_DYNCREATE(CShowPalDoc)

// Attributes
public:
    CDIBPal* GetPalette() {return m_pPal;}

// Operations
public:

// Implementation
public:
    virtual ~CShowPalDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
protected:
    virtual BOOL    OnNewDocument();

private:
    CDIBPal* m_pPal;

// Generated message map functions
protected:
    //{{AFX_MSG(CShowPalDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
