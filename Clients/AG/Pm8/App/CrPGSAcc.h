#if !defined(AFX_CRPGSACC_H__EAAE57A3_E5C4_11D1_9909_00A0240C7400__INCLUDED_)
#define AFX_CRPGSACC_H__EAAE57A3_E5C4_11D1_9909_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CrPGSAcc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreatePGSAccount dialog

class CCreatePGSAccount : public CDialog
{
// Construction
public:
	CCreatePGSAccount(CWnd* pParent = NULL);   // standard constructor
	UINT VerifyPasswordFormat(); 
	UINT VerifyAccountFormat(); 
	UINT VerifyPassword(); 

// Dialog Data
	//{{AFX_DATA(CCreatePGSAccount)
	enum { IDD = IDD_CREATEPGSACCOUNT };
	CEdit	m_ecVerifyPassword;
	CEdit	m_ecPassword;
	CEdit	m_ecEMail;
	CEdit	m_ecAccountID;
	CString	m_strAccountID;
	CString	m_strEMail;
	CString	m_strPassword;
	CString	m_strVerifyPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreatePGSAccount)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreatePGSAccount)
	afx_msg void OnChangeAccountIDEdit();
	afx_msg void OnChangeEmailEdit();
	afx_msg void OnChangePasswordEdit();
	afx_msg void OnChangeVerifypasswordEdit();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRPGSACC_H__EAAE57A3_E5C4_11D1_9909_00A0240C7400__INCLUDED_)
