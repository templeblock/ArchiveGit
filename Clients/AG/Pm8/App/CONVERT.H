// convert.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConvertingDialog dialog

class CConvertingDialog : public CDialog
{
// Construction
public:
	CConvertingDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CConvertingDialog)
	enum { IDD = IDD_CONVERTING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CConvertingDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
