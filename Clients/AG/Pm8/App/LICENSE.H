// license.h : header file
//

#include "pmwdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg dialog

class CLicenseDlg : public CPmwDialog
{
// Construction
public:
	CLicenseDlg(PINSTALLATIONINFO pInfo, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLicenseDlg)
	enum { IDD = IDD_NETWORK_LICENSE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	PINSTALLATIONINFO m_pInfo;
// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void show_licensed_users();

	// Generated message map functions
	//{{AFX_MSG(CLicenseDlg)
	afx_msg void OnClickedLicenseAdd();
	afx_msg void OnClickedLicenseInstall();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedLicenseUsers();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CAddLicenseDialog dialog

class CAddLicenseDialog : public CPmwDialog
{
// Construction
public:
	CAddLicenseDialog(int max_users, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAddLicenseDialog)
	enum { IDD = IDD_NETWORK_ADD };
	int		m_new_users;
	//}}AFX_DATA

	int m_max_users;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CAddLicenseDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CInstallLicenseDialog dialog

class CInstallLicenseDialog : public CPmwDialog
{
// Construction
public:
	CInstallLicenseDialog(PINSTALLATIONINFO pInfo, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInstallLicenseDialog)
	enum { IDD = IDD_NETWORK_INSTALL };
	//}}AFX_DATA

	PINSTALLATIONINFO m_pInfo;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CInstallLicenseDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CLicenseRequestDialog dialog

class CLicenseRequestDialog : public CPmwDialog
{
// Construction
public:
	CLicenseRequestDialog(LPSTR sn, LPSTR lrc, CWnd* pParent = NULL);	// standard constructor

	LPSTR m_sn;
	LPSTR m_lrc;

// Dialog Data
	//{{AFX_DATA(CLicenseRequestDialog)
	enum { IDD = IDD_LICENSE_REQUEST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CLicenseRequestDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CLicenseExceededDialog dialog

class CLicenseExceededDialog : public CPmwDialog
{
// Construction
public:
	CLicenseExceededDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLicenseExceededDialog)
	enum { IDD = IDD_LICENSE_EXCEEDED };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CLicenseExceededDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
