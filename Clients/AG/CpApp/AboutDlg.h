#pragma once
#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
class CAboutDlg : public CDialog
{
public:
	CAboutDlg(CWnd* pParentWnd, LPCSTR pstrVersion = NULL);

	enum { IDD = IDD_ABOUTBOX };

	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();

protected:
	DECLARE_MESSAGE_MAP()

private:
	CString m_strVersion;
};
