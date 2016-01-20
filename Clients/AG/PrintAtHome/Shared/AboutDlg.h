#pragma once

/////////////////////////////////////////////////////////////////////////////

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
	CAboutDlg(LPCSTR pstrVersion)
	{
		m_pstrVersion = pstrVersion;
	}

	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	//////////////////////////////////////////////////////////////////////
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HWND hWnd = GetDlgItem(IDHELP);
		if (hWnd)
		{
			char szLabel[MAX_PATH];
			::GetWindowText(hWnd, szLabel, sizeof(szLabel));
			CString strLabel = szLabel;
			strLabel += m_pstrVersion;
			::SetWindowText(hWnd, strLabel);
		}

		return IDOK;
	}

	//////////////////////////////////////////////////////////////////////
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(IDOK);
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return S_OK;
	}

private:
	LPCSTR m_pstrVersion;
};
