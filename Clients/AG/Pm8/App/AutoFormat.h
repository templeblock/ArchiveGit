#if !defined(AFX_AUTOFORMAT_H__49EFB462_E995_11D1_B87A_006097BAD988__INCLUDED_)
#define AFX_AUTOFORMAT_H__49EFB462_E995_11D1_B87A_006097BAD988__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AutoFormat.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoFormat dialog

class CAutoFormat : public CDialog
{
// Construction
public:
	CAutoFormat(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoFormat)
	enum { IDD = IDD_AUTO_FORMAT };
	CListBox	m_list;
	CComboBox	m_tableformat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoFormat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoFormat)
	afx_msg void OnSelchangeTableFormat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOFORMAT_H__49EFB462_E995_11D1_B87A_006097BAD988__INCLUDED_)
