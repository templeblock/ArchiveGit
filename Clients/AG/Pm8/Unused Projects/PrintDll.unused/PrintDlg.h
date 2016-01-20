#if !defined(AFX_PRINTDLG_H__1BC1296F_FEB3_11D1_B06E_00A024EA69C0__INCLUDED_)
#define AFX_PRINTDLG_H__1BC1296F_FEB3_11D1_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PrintDlg.h : header file
//

enum
{
	SUCCESS = 0,
	ERR_APP_STARTUP,
	ERR_TIME_OUT,
	ERR_PRODUCT_CODE
};

/////////////////////////////////////////////////////////////////////////////
// CPrintDlg dialog

class CPrintDlg : public CDialog
{
// Construction
public:
	CPrintDlg(CString csFilePath, CString csProductCode, CWnd* pParent = NULL);   // standard constructor
	~CPrintDlg();

// Dialog Data
	//{{AFX_DATA(CPrintDlg)
	enum { IDD = IDD_STATUS_DLG };
	CStatic	m_cMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HWND m_hPrintAppWnd;
	BOOL m_bConnectionMade;
	int	 m_nLastDDEMessage;
	CString m_csFilePath;
	CString m_csProductCode;
	HANDLE m_hStartupEvent;
	PROCESS_INFORMATION m_pi;
	HGLOBAL m_hGlobal;
	CString m_csAppPath;
	BOOL GetRegistryString(HKEY hRootKey, LPCTSTR pszSubkey, LPCTSTR pszValue, CString& csValue);

	CString m_csExeName;
	void DoPrintJob() ;
	void DdeInit();
	void StartProcess();

	// Generated message map functions
	//{{AFX_MSG(CPrintDlg)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnDDEAck(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDDETerminate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnConnectComplete();
	afx_msg void OnPrintComplete();
	afx_msg void OnStartPrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTDLG_H__1BC1296F_FEB3_11D1_B06E_00A024EA69C0__INCLUDED_)
