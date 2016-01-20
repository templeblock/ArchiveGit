// Uninsdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUninstallDlg dialog

class CUninstallDlg : public CDialog
{
// Construction
public:
	CUninstallDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUninstallDlg)
	enum { IDD = IDD_UNINSTALL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUninstallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CUninstallDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnContinue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
