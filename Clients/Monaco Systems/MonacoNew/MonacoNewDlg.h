// MonacoNewDlg.h : header file
//
//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES

#if !defined(AFX_MONACONEWDLG_H__882436CD_0AEC_11D2_91E6_E0EF0DC10000__INCLUDED_)
#define AFX_MONACONEWDLG_H__882436CD_0AEC_11D2_91E6_E0EF0DC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CMonacoNewDlg dialog

class CMonacoNewDlg : public CDialog
{
// Construction
public:
	void OnWizard();
	CMonacoNewDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMonacoNewDlg)
	enum { IDD = IDD_MONACONEW_DIALOG };
	CWebBrowser2	m_browser;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonacoNewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CComboBox m_combo;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMonacoNewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOnVisibleExplorer1(BOOL Visible);
	afx_msg void OnBeforeNavigate2Explorer1(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	afx_msg void OnSetfocusCombo1();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONACONEWDLG_H__882436CD_0AEC_11D2_91E6_E0EF0DC10000__INCLUDED_)
