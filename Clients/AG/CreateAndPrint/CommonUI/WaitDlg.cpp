#include "stdafx.h"
#include "WaitDlg.h"
#include "Ctp.h"

//////////////////////////////////////////////////////////////////////
CWaitDlg::CWaitDlg(CCtp* pCtp)
{
	m_pCtp = pCtp;
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
	if (m_pCtp)
		return m_pCtp->OnCtlColor(uMsg, wParam, lParam, bHandled);

	return NULL;
}

//////////////////////////////////////////////////////////////////////
LRESULT CWaitDlg::OnCancel(WORD, WORD, HWND, BOOL&)
{
	return S_OK;
}

