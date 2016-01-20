#include "stdafx.h"
#include "WaitDlg.h"
#include "DocWindow.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CWaitDlg::CWaitDlg(CDocWindow* pDocWindow)
{
	m_pDocWindow = pDocWindow;
}

//////////////////////////////////////////////////////////////////////
CWaitDlg::~CWaitDlg()
{
}

//////////////////////////////////////////////////////////////////////
LRESULT CWaitDlg::OnInitDialog(UINT, WPARAM, LPARAM, BOOL&)
{
	CenterWindow();
	return IDOK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CWaitDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pDocWindow)
		return m_pDocWindow->OnCtlColor(uMsg, wParam, lParam, bHandled);

	return NULL;
}

//////////////////////////////////////////////////////////////////////
LRESULT CWaitDlg::OnCancel(WORD, WORD, HWND, BOOL&)
{
	return S_OK;
}

