// SerialNumberDlg.h : header file
//

#if !defined(AFX_SerialNumberDLG_H__CD41E48B_D443_11D1_9EE4_006008947D80__INCLUDED_)
#define AFX_SerialNumberDLG_H__CD41E48B_D443_11D1_9EE4_006008947D80__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "SerialNumber.h"
#include <afxcmn.h>

/////////////////////////////////////////////////////////////////////////////
// CSerialNumberDlg dialog

class CSerialNumberDlg : public CDialog
{
// Construction
public:
	CSerialNumberDlg(CSerialNumber* pSerialNumber, CString& pApplication, CWnd* pParent = NULL);	// standard constructor
	~CSerialNumberDlg();	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSerialNumberDlg)
	CEdit m_Message;
	CString	m_szApplication;
	CString m_szSerialNumber;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialNumberDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CSerialNumber* m_pSerialNumber;

	// Generated message map functions
	//{{AFX_MSG(CSerialNumberDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeSerialNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SerialNumberDLG_H__CD41E48B_D443_11D1_9EE4_006008947D80__INCLUDED_)
