#pragma once

#include "resource.h"
#include "ColorScheme.h"

class CCtp;

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg

class CProgressDlg : public CDialogImpl<CProgressDlg>
{
public:
	CProgressDlg(CCtp* pMainWnd);
	~CProgressDlg();
	void SetProgressText(CString strText);
	void CenterInParent();

	enum { IDD = IDD_PROGRESSDLG };

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CProgressDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

private:
	CCtp* m_pCtp;
	CColorScheme m_ColorScheme;
};
