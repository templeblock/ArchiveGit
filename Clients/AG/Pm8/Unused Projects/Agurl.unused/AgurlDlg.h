// AgurlDlg.h : header file
//

#if !defined(AFX_AGURLDLG_H__91212F9A_14AB_11D2_B06E_00A024EA69C0__INCLUDED_)
#define AFX_AGURLDLG_H__91212F9A_14AB_11D2_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CAgurlDlg dialog

class CAgurlDlg : public CDialog
{
// Construction
public:
	CAgurlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CAgurlDlg)
	enum { IDD = IDD_AGURL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgurlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAgurlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGURLDLG_H__91212F9A_14AB_11D2_B06E_00A024EA69C0__INCLUDED_)
