#pragma once
#include "afxwin.h"


// CXMLDialog dialog

class CXMLDialog : public CDialog
{
	DECLARE_DYNAMIC(CXMLDialog)

public:
	CXMLDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CXMLDialog();

// Dialog Data
	enum { IDD = IDD_XML_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedUnpackFiles();
	afx_msg void OnBnClickedSrcBrowse();
	afx_msg void OnBnClickedDstBrowse();
	afx_msg void OnBnClickedSchBrowse();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

public:
	void GetSrcPath(CString& strPath) { strPath = m_strSrcPath; }
	void GetDstPath(CString& strPath) { strPath = m_strDstPath; }
	void GetSchemaPath(CString& strFile) { strFile = m_strSchPath; }
	int  GetTestFileCount() { return m_FileCount; }
	BOOL GetXmlValidation() { return m_bValidateXml; }
	BOOL GetUnpackFiles() { return m_bUnpackFiles; }

protected:
	CString GetInitialDir();
	CString GetInitialDir(CString strPath);
	void GetPath(CString& strPath, CString strTitle);
	void GetFileName(CString& strFile, CString strTitle);

	CString m_strSrcPath;
	CString m_strDstPath;
	CString m_strSchPath;

public:
	CEdit m_edtSrc;
	CEdit m_edtDest;
	CEdit m_edtSchPath;

	UINT m_FileCount;
	BOOL m_bValidateXml;
	BOOL m_bUnpackFiles;
};
