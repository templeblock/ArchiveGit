#if !defined(AFX_MODIFYDIALOG_H__004A9664_A6B3_11D1_9EE4_006008947D80__INCLUDED_)
#define AFX_MODIFYDIALOG_H__004A9664_A6B3_11D1_9EE4_006008947D80__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ModifyDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModifyDialog dialog

class CModifyDialog : public CDialog
{
// Construction
public:
	CModifyDialog(BOOL	allowmodify, CWnd* pParent = NULL);   // standard constructor

	BOOL m_allowmodify;
// Dialog Data
	//{{AFX_DATA(CModifyDialog)
	enum { IDD = IDD_MODIFY };
	CButton	m_bClear;
	CButton	m_bOk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModifyDialog)
	afx_msg void OnClear();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYDIALOG_H__004A9664_A6B3_11D1_9EE4_006008947D80__INCLUDED_)
