// progdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUninstallProgressDialog dialog

class CUninstallProgressDialog : public CDialog
{
// Construction
public:
	CUninstallProgressDialog(CWnd* pParent = NULL);   // standard constructor
	BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);

// Dialog Data
	//{{AFX_DATA(CUninstallProgressDialog)
	enum { IDD = IDD_PROGRESS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUninstallProgressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUninstallProgressDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetProgress(LPCSTR pszAction = NULL, LPCSTR pszItem = NULL);
	void SetAction(LPCSTR pszAction = NULL);
	void SetItem(LPCSTR pszItem = NULL);
};
