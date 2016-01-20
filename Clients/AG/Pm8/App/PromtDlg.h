#if !defined(AFX_PROMTDLG_H__024155A4_0C32_11D2_B06E_00A024EA69C0__INCLUDED_)
#define AFX_PROMTDLG_H__024155A4_0C32_11D2_B06E_00A024EA69C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PromtDlg.h : header file
//
#define UM_CDROM_DETECTED WM_USER + 75

/////////////////////////////////////////////////////////////////////////////
// CPromptDlg dialog

class CPromptDlg : public CDialog
{
// Construction
public:
	CPromptDlg(CString csTitle, CString csMessage, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPromptDlg)
	enum { IDD = IDD_PROMPT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPromptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString m_csTitle;
	CString m_csMessage;

	// Generated message map functions
	//{{AFX_MSG(CPromptDlg)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnCDDetected(LPARAM lParam, WPARAM wParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROMTDLG_H__024155A4_0C32_11D2_B06E_00A024EA69C0__INCLUDED_)
