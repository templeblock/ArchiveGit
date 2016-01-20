#if !defined(AFX_SERVERACCOUNTDLG_H__4ECA5A07_BB35_11D1_9909_00A0240C7400__INCLUDED_)
#define AFX_SERVERACCOUNTDLG_H__4ECA5A07_BB35_11D1_9909_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ServerAccountDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CServerAccountDlg dialog

class CServerAccountDlg : public CDialog
{
// Construction
public:
	CServerAccountDlg(int nType, CWnd* pParent = NULL);   // standard constructor
	UINT VerifyPasswordFormat(); 
	UINT VerifyAccountFormat(); 
	UINT VerifyPassword(); 

	void FromAccountInfo(const CServerAccountInfo& AccountInfo);
	void ToAccountInfo(CServerAccountInfo& AccountInfo) const;

	int m_eType;

	enum AcctTypes
		{NewAcct,
		  ExistingAcct,
		  UpdateAcct
		};

	// Dialog Data
	//{{AFX_DATA(CServerAccountDlg)
	enum { IDD = IDD_SERVERACCOUNTDIALOG };
	CEdit	m_ecStreet2;
	CEdit	m_ecStreet1;
	CStatic	m_stLastName;
	CStatic	m_stEMail;
	CStatic	m_stZipCode;
	CStatic	m_stVerifyPassword;
	CStatic	m_stStreet;
	CStatic	m_stState;
	CStatic	m_stPhone;
	CStatic	m_stPassword;
	CStatic	m_stFirstName;
	CStatic	m_stCountry;
	CStatic	m_stCity;
	CStatic	m_stAccount;
	CEdit	m_ecAccountID;
	CEdit	m_ecZipCode;
	CEdit	m_ecState;
	CEdit	m_ecPhone;
	CEdit	m_ecLastName;
	CEdit	m_ecFirstName;
	CEdit	m_ecEMail;
	CEdit	m_ecCountry;
	CEdit	m_ecCity;
	CEdit	m_ecPassword;
	CEdit	m_ecVerifyPassword;
	CString	m_strCity;
	CString	m_strCountry;
	CString	m_strEMail;
	CString	m_strFirstName;
	CString	m_strLastName;
	CString	m_strPhone;
	CString	m_strState;
	CString	m_strZipCode;
	CString	m_strPassword;
	CString	m_strVerifyPassword;
	CString	m_strAccountID;
	CString	m_strStreet1;
	CString	m_strStreet2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerAccountDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CServerAccountDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeCity();
	afx_msg void OnChangeCountry();
	afx_msg void OnChangeEmail();
	afx_msg void OnChangeFirstName();
	afx_msg void OnChangeLastName();
	afx_msg void OnChangePassword();
	afx_msg void OnChangePhone();
	afx_msg void OnChangeState();
	afx_msg void OnChangeVerifyPassword();
	afx_msg void OnChangeZipcode();
	afx_msg void OnChangeAccountID();
	afx_msg void OnChangeStreet1Edit();
	afx_msg void OnChangeStreet2Edit();
	afx_msg void OnKillFocusPasswordEdit();
	afx_msg void OnKillFocusVerifyPasswordEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERACCOUNTDLG_H__4ECA5A07_BB35_11D1_9909_00A0240C7400__INCLUDED_)
