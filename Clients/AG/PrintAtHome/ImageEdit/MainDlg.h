#pragma once
#include "ResizeDlg.h"
#include "TabControl.h"
#include "ImageControlWrapper.h"
#include "ButtonST.h"

// CMainDlg dialog
class CMainDlg : public CResizeDlg
{
// Construction
public:
	CMainDlg(CWnd* pParent = NULL);	// standard constructor
	~CMainDlg();
	bool DoCommand(LPCTSTR strCommand, LPCTSTR strValue);
	long GetCommand(LPCTSTR strCommand);
	CImageControl& GetImageControl();

// Dialog Data
	enum { IDD = IDD_IMAGEEDIT_DIALOG };

protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	// Generated message map functions
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStartOver();
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnZoomFull();
	afx_msg void OnApply();

private:
	HICON m_hIcon;
	bool bInit;
	CTabControl m_TabControl;
	CImageControl m_ImageControl;
	CButtonST m_btnZoomFull;
//j	CBitmapButton m_btnZoomFull;
//j	CButton m_btnZoomFull;
};
