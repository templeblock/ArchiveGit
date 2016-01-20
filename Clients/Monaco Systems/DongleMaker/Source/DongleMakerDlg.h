// DongleMakerDlg.h : header file
//

#if !defined(AFX_DongleMakerDLG_H__CD41E48B_D443_11D1_9EE4_006008947D80__INCLUDED_)
#define AFX_DongleMakerDLG_H__CD41E48B_D443_11D1_9EE4_006008947D80__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CDongleMakerDlg dialog

#include "Dongle.h"

class CDongleMakerDlg : public CDialog
{
// Construction
public:
	CDongleMakerDlg(CWnd* pParent = NULL);	// standard constructor
	~CDongleMakerDlg();	// standard constructor

	BOOL AddToTree(AppLock* appLock); 
	void SetupLimitCountUI(AppLock* applock);
	AppLock* GetSelectedApp(HTREEITEM hSelected, LPDWORD pdwState, LPDWORD pdwParam);
	void CheckTreeItem(HTREEITEM hItem, BOOL bOn);
	 
// Dialog Data
	//{{AFX_DATA(CDongleMakerDlg)
	enum { IDD = IDD_DongleMaker_DIALOG };
	CTreeCtrl m_AppTree;
	CString	m_serialNumber;
	CString	m_AppName;
	CString m_versions;
	CString m_features;
	CString m_algorithm;
	CString m_apassword;
	CButton m_unlimited;
	CButton m_limited_uses;
	CButton m_limited_days;
	DWORD m_count;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDongleMakerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CDongle* m_pDongle;
	CImageList m_imagelist;
	HTREEITEM m_selected;
	int m_iSavedCount;

	// Generated message map functions
	//{{AFX_MSG(CDongleMakerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnReadDongle();
	afx_msg void OnProgramDongle();
	afx_msg void OnUnlimited();
	afx_msg void OnLimitedUses();
	afx_msg void OnLimitedDays();
	afx_msg void OnChangeCount();
	afx_msg void OnAppTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAppTreeDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnOrder();
	afx_msg void OnTestDongle();
	afx_msg void OnTestSerialNumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DongleMakerDLG_H__CD41E48B_D443_11D1_9EE4_006008947D80__INCLUDED_)
