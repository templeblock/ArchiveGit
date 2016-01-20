#pragma once
#include "afxwin.h"

#define TA_NONE		1
#define TA_CLOSE	2
#define TA_WAIT		4
#define TA_KILL		8

class CProcessExamDlg : public CDialog
{
public:
	CProcessExamDlg(CWnd* pParent = NULL);
	~CProcessExamDlg();

	enum { IDD = IDD_PROCESSEXAM_DIALOG };

	int ProcessWalk(int iTerminateCode, bool bSilent);
	void ShutdownAll();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	bool IsMatch(LPCSTR strModule);
	

private:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nUIEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRefreshClicked();
	afx_msg void OnCloseAllClicked();
	afx_msg void OnKillAllClicked();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnSearchChanged();
	DECLARE_MESSAGE_MAP()

private:
	bool m_bCloseAllStarted;
	UINT_PTR m_idTimer;
	HICON m_hIcon;
	CListBox m_ListBox;
	CEdit m_Search;
	int m_iTimerCount;
	int m_dx;
	int m_dy;
	CString m_strSearch;
};
