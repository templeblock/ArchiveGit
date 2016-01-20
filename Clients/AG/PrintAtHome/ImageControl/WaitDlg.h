#pragma once

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg

class CDocWindow;

class CWaitDlg : public CDialogImpl<CWaitDlg>
{
public:
	CWaitDlg(CDocWindow* pDocWindow);
	~CWaitDlg();

	enum { IDD = IDD_WAITDLG };

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	CDocWindow* m_pDocWindow;

public:
	BEGIN_MSG_MAP(CWaitDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()
};
