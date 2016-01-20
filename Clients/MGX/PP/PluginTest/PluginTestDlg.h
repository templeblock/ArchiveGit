// PluginTestDlg.h : header file
//

#if !defined(AFX_PLUGINTESTDLG_H__5DDFD74B_7F8E_11D2_91E7_00A02459C447__INCLUDED_)
#define AFX_PLUGINTESTDLG_H__5DDFD74B_7F8E_11D2_91E7_00A02459C447__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CPluginTestDlg dialog

typedef enum {
	PIT_FILTER,
	PIT_EXPORT,
    PIT_ACQUIRE
} PLUGIN_TYPE;

#include "CProgressDlg.h"

class CPluginTestDlg : public CDialog
{
// Construction
public:
	CPluginTestDlg(CWnd* pParent = NULL);	// standard constructor
	~CPluginTestDlg();	// standard destructor

	BOOL AddFilter( CString& lpFilter, CString& lpFileName,
			bool bOldStyle, PLUGIN_TYPE piType, int iListIndex );
	void FreeUpPluginList();

// Dialog Data
	//{{AFX_DATA(CPluginTestDlg)
	enum { IDD = IDD_PLUGINTEST_DIALOG };
	CEdit m_editSearchPath;
	CListBox m_listPlugins;
	CButton m_btnSearch;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPluginTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	static CProgressDlg* pProgressDlg;
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPluginTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSearch();
	afx_msg void OnAcquire();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINTESTDLG_H__5DDFD74B_7F8E_11D2_91E7_00A02459C447__INCLUDED_)
