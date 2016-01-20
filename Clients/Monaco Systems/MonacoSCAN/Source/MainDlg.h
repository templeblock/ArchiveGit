// MainDlg.h : header file
//

#if !defined(AFX_MAINDLG_H__F09703D0_364F_11D1_BAE5_200604C10000__INCLUDED_)
#define AFX_MAINDLG_H__F09703D0_364F_11D1_BAE5_200604C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Target.h"

/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

class CMainDlg : public CDialog
{
// Construction
public:
	CMainDlg(CWnd* pParent = NULL);	// standard constructor
	~CMainDlg();	// standard destructor

	void GetReference(CString* pszFileName);
	void GetImage(CString* pszFileName);
	void EnableControl( int id, BOOL bEnable );
	void SetControlCompletion( int id, BOOL bComplete, CString* pszExtraText = NULL );
	void SetDone(int id, BOOL bDone);
	BOOL GetReferenceFileName( CString& szFileName );
	BOOL GetImageFileName( CString& szFileName );

// Dialog Data
	//{{AFX_DATA(CMainDlg)
	enum { IDD = IDD_MAIN };
	CTarget m_Target;
	CString	m_szReferenceFile;
	CString	m_szImageFile;
	CPoint m_ptCornerUR;
	CPoint m_ptCornerUL;
	CPoint m_ptCornerLR;
	CPoint m_ptCornerLL;
	CPoint	m_ptOutputUR;
	CPoint	m_ptOutputUL;
	CPoint	m_ptOutputLR;
	CPoint	m_ptOutputLL;
	HGLOBAL m_hDib;
	HICON m_hIcon;
	BOOL m_bImageDone;
	BOOL m_bReferenceDone;
	//}}AFX_DATA

// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMainDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnReference();
	afx_msg void OnImage();
	afx_msg void OnCrop();
	afx_msg void OnCalibrate();
	afx_msg void OnProfileSave();
	afx_msg void OnReferenceFileSelected();
	afx_msg void OnImageFileSelected();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDLG_H__F09703D0_364F_11D1_BAE5_200604C10000__INCLUDED_)
