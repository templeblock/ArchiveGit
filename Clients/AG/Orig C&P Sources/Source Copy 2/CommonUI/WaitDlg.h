#ifndef __WAITDLG_H_
#define __WAITDLG_H_

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg
class CWaitDlg : 
	public CDialogImpl<CWaitDlg>
{
public:
	CWaitDlg();
	~CWaitDlg();

	enum { IDD = IDD_WAITDLG };

BEGIN_MSG_MAP(CWaitDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
END_MSG_MAP()

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

#endif //__WAITDLG_H_
