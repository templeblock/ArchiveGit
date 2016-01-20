#pragma once

#include "AGPage.h"
#include "ProgDlg.h"

class CLogger;

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnBuildThumbs();
	afx_msg void OnBuildDataSheet();
	afx_msg void OnBuildXml();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	int BuildThumbs(CString& strPath, bool bJustCount, CProgressDlg& ProgDlg);
	int BuildDataSheet(CString& strPath, bool bJustCount, CProgressDlg& ProgDlg, FILE* output);
	int BuildXmls(CString& strSrcPath, CString& strDstPath, CString& strSchPath, bool bJustCount, CProgressDlg& ProgDlg, BOOL bUnpackFiles, int nTestFileCount=50);
	bool ValidateXml(CString& strXmlFile, CString& strSchemaFile, CString& strProjectName);
	void CleanUp(CString& strCtpRoot);
	void CaptureFileNames();

	CLogger* m_pLog;

	int m_cx;
	int m_cy;

public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
