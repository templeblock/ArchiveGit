#include "stdafx.h"
#include "FileTab.h"
#include "MainDlg.h"

/////////////////////////////////////////////////////////////////////////////
CFileTab::CFileTab()
{
}

/////////////////////////////////////////////////////////////////////////////
CFileTab::~CFileTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CFileTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);
	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CFileTab::OnEnterTab(bool bFirstTime)
{
}

/////////////////////////////////////////////////////////////////////////////
void CFileTab::OnLeaveTab()
{
}

/////////////////////////////////////////////////////////////////////////////
void CFileTab::OnImageOpen(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("FileTab"), _T("Open"));
}

/////////////////////////////////////////////////////////////////////////////
void CFileTab::OnImageSave(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("FileTab"), _T("Save"));
}

/////////////////////////////////////////////////////////////////////////////
void CFileTab::OnImageClose(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("FileTab"), _T("Close"));
}

