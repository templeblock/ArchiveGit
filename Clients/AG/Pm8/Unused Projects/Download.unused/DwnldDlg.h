// DwnldDlg.h : header file
//

#if !defined(AFX_DWNLDDLG_H__42B2C467_61F5_11D2_9862_00A0246D4780__INCLUDED_)
#define AFX_DWNLDDLG_H__42B2C467_61F5_11D2_9862_00A0246D4780__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CDownloadDlg dialog

class CDownloadDlg : public CDialog
{
// Construction
public:
	CDownloadDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDownloadDlg)
	enum { IDD = IDD_DOWNLOAD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDownloadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDownloadDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWNLDDLG_H__42B2C467_61F5_11D2_9862_00A0246D4780__INCLUDED_)
