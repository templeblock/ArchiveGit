// LumDemoW32DLLDlg.h : header file
//

#if !defined(AFX_LUMDEMOW32DLLDLG_H__AA26F7D5_F706_11D1_8E31_00403321D5C4__INCLUDED_)
#define AFX_LUMDEMOW32DLLDLG_H__AA26F7D5_F706_11D1_8E31_00403321D5C4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CLumDemoW32DLLDlg dialog

class CLumDemoW32DLLDlg : public CDialog
{
// Construction
public:
	CLumDemoW32DLLDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLumDemoW32DLLDlg)
	enum { IDD = IDD_LUMDEMOW32DLL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLumDemoW32DLLDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLumDemoW32DLLDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpenPod();
	virtual void OnOK();
	afx_msg void OnSetNumFields();
	afx_msg void OnMeasFreq();
	afx_msg void OnUnits();
	afx_msg void OnMeasure();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LUMDEMOW32DLLDLG_H__AA26F7D5_F706_11D1_8E31_00403321D5C4__INCLUDED_)
