#include "stdafx.h"
#include "AboutDlg.h"
#include "ModuleVersion.h"

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg(CWnd* pParentWnd, LPCSTR pstrVersion) : CDialog(CAboutDlg::IDD, pParentWnd)
{
	m_strVersion = pstrVersion;
	if (m_strVersion.IsEmpty())
	{
		char szAppPath[MAX_PATH];
		::GetModuleFileName(AfxGetInstanceHandle(), szAppPath, sizeof(szAppPath));
		CModuleVersion ver;
		if (ver.GetFileVersionInfo(szAppPath))
		{
			char szProductversion[MAX_PATH];
			ver.GetProductVersion(szProductversion, MAX_PATH);
			m_strVersion = szProductversion;
		}
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CAboutDlg::OnInitDialog()
{
	CWnd* pWnd = GetDlgItem(IDHELP);
	if (pWnd && !m_strVersion.IsEmpty())
	{
		char szLabel[MAX_PATH];
		pWnd->GetWindowText(szLabel, sizeof(szLabel));
		CString strLabel = szLabel;
		strLabel += m_strVersion;
		pWnd->SetWindowText(strLabel);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
void CAboutDlg::OnOK()
{
	EndDialog(IDOK);
}

//////////////////////////////////////////////////////////////////////
void CAboutDlg::OnCancel()
{
	EndDialog(IDCANCEL);
}
