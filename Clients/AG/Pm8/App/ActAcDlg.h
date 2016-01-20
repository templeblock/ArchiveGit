#if !defined(AFX_ACTACDLG_H__2EB86D01_BF37_11D1_9909_00A0240C7400__INCLUDED_)
#define AFX_ACTACDLG_H__2EB86D01_BF37_11D1_9909_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ActAcDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAccountAccessDlg dialog

class CAccountAccessDlg : public CDialog
{
// Construction
public:
	CAccountAccessDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_fNew;

// Dialog Data
	//{{AFX_DATA(CAccountAccessDlg)
	enum { IDD = IDD_ACCOUNTACCESSDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccountAccessDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAccountAccessDlg)
	afx_msg void OnAccessExistingAccount();
	afx_msg void OnCreateAccount();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTACDLG_H__2EB86D01_BF37_11D1_9909_00A0240C7400__INCLUDED_)
