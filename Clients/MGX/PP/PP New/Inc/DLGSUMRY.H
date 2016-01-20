// dlgsumry.h : header file
//
#include "propsht.h"
#include "id.h"

/////////////////////////////////////////////////////////////////////////////
// CSummaryStreamPage dialog

class CSummaryStreamPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CSummaryStreamPage)

// Construction
public:
	CSummaryStreamPage();
	~CSummaryStreamPage();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSummaryStreamPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSummaryStreamPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeSummary();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void HandleOK();
	virtual void HandleCancel();

// Dialog Data
	//{{AFX_DATA(CSummaryStreamPage)
	enum { IDD = IDD_SUMMDLG };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	CString	m_SummAuthor;
	CString	m_SummComments;
	CString	m_SummKeywords;
	CString	m_SummSubject;
	CString	m_SummTitle;
	CString	m_SummLastAuthor;
	CString	m_SummCreated;
	CString	m_SummLastSaved;
	CString	m_SummLastPrinted;
	//}}AFX_DATA
};
/////////////////////////////////////////////////////////////////////////////
// CGeneralSummaryPage dialog

class CGeneralSummaryPage : public CPPPropertyPage
{
	DECLARE_DYNCREATE(CGeneralSummaryPage)

// Construction
public:
	CGeneralSummaryPage();
	~CGeneralSummaryPage();

// Dialog Data
	//{{AFX_DATA(CGeneralSummaryPage)
	enum { IDD = IDD_IMAGEINFO };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGeneralSummaryPage)
	//}}AFX_VIRTUAL

// Implementation 
protected:
	virtual BOOL OnInitDialog();
	virtual void HandleOK();
	virtual void HandleCancel();
};

/////////////////////////////////////////////////////////////////////////////
// CSummaryPropSheet

class CSummaryPropSheet : public CPPPropertySheet
{
	DECLARE_DYNCREATE(CSummaryPropSheet)

// Construction
public:
	CSummaryPropSheet(CServerDoc *pDocument, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT nIDHelp = 0);
	CSummaryPropSheet(CServerDoc *pDocument,LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT nIDHelp = 0);

// Attributes
public:

// Operations
public:
    CServerDoc *GetDocument() {return m_pDocument;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSummaryPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSummaryPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSummaryPropSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	UINT GetSelectPage();
	void CommonConstruct();
	CServerDoc *m_pDocument;
    BOOL m_bHasSummaryStream;

// Dialog Data
protected:
	CSummaryStreamPage 	m_SummaryStreamPage;
	CGeneralSummaryPage	m_GeneralSummaryPage;
	static int			m_nLastPage;
};

/////////////////////////////////////////////////////////////////////////////

