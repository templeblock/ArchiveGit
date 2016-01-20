#include "StdAfx.h"
#include "TestDlg.h"
/////////////////////////////////////////////////////////////////////
CTestDlg::CTestDlg(void)
{
}

CTestDlg::~CTestDlg(void)
{
}
//////////////////////////////////////////////////////////////////////
LRESULT CTestDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SetWindowText(GetDlgItem(IDC_EDIT1), _T("C:\\My Projects\\AgApplications\\PrintAtHome\\PAHContentMgr\\pack_01.xml"));
	return 1;  // Set the focus
}

//////////////////////////////////////////////////////////////////////
LRESULT CTestDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	TCHAR szText[MAX_PATH];
	
	::GetWindowText(GetDlgItem(IDC_EDIT1), szText, MAX_PATH);
	
	m_szText = szText;
	EndDialog(IDOK);
	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CTestDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return S_OK;
}
