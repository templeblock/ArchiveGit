#include "stdafx.h"
#include "MessageBox.h"
#include "JMExplorerBand.h"
#include ".\messagebox.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
CMessagebox::CMessagebox()
{
	m_uType = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
CMessagebox::~CMessagebox(void)
{
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////
int CMessagebox::ShowMessagebox(HWND hwndParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	m_szText = lpText;
	m_szCaption = lpCaption;
	m_uType = uType;
	m_hParent = hwndParent;

	return DoModal(hwndParent);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CMessagebox::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CButton btnHelp = GetDlgItem(IDHELP);
	CButton btnOK = GetDlgItem(IDOK);
	CButton btnYes = GetDlgItem(IDYES);
	CButton btnNo = GetDlgItem(IDNO);
	CButton btnClose = GetDlgItem(IDCLOSE);
	btnHelp.ShowWindow(SW_HIDE);
	btnOK.ShowWindow(SW_HIDE);
	btnYes.ShowWindow(SW_HIDE);
	btnNo.ShowWindow(SW_HIDE);
	btnClose.ShowWindow(SW_HIDE);
	if (m_uType & MB_HELP)
	{
		btnHelp.ShowWindow(SW_SHOW);
		btnOK.ShowWindow(SW_SHOW);
	}
	else if (m_uType & MB_YESNO)
	{
		btnYes.ShowWindow(SW_SHOW);
		btnNo.ShowWindow(SW_SHOW);
	}
	else
	{
		btnClose.ShowWindow(SW_SHOW);
	}

	
	SetWindowText(m_szCaption);
	HWND hWnd = GetDlgItem(IDC_MSG_TEXT);
	if (hWnd)
		::SetWindowText(hWnd, m_szText);

	CenterWindow();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CMessagebox::OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDOK);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CMessagebox::OnBnClickedHelpButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (::IsWindow(m_hParent))
		::PostMessage(m_hParent, UWM_HELP, 0, 0);

	EndDialog(IDHELP);
	return 0;
}

LRESULT CMessagebox::OnBnClickedCloseButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCLOSE);
	return 0;
}

LRESULT CMessagebox::OnBnClickedYesButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDYES);
	return 0;
}

LRESULT CMessagebox::OnBnClickedNoButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDNO);
	return 0;
}
