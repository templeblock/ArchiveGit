#include "stdafx.h"
#include "RedEyeTab.h"
#include "MainDlg.h"

/////////////////////////////////////////////////////////////////////////////
CRedEyeTab::CRedEyeTab()
{
}

/////////////////////////////////////////////////////////////////////////////
CRedEyeTab::~CRedEyeTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRedEyeTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	SetMsgHandled(false);

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnEnterTab(bool bFirstTime)
{
	bool bOK = DoCommand(_T("RedEyeTab"), Str(_T("%d"), EDIT_START));
	EnableButtons(/*Apply*/false, /*Reset*/false);
	SetTimer(TAB_TIMER, 1000);
}

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnLeaveTab()
{
	bool bOK = DoCommand(_T("RedEyeTab"), Str(_T("%d"), EDIT_END));
	KillTimer(TAB_TIMER);
}

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnApply(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("RedEyeTab"), Str(_T("%d"), EDIT_APPLY));
	OnEnterTab();
	GoBack(); // added
}

/////////////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnReset(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("RedEyeTab"), Str(_T("%d"), EDIT_RESET));
	OnEnterTab();
}

//////////////////////////////////////////////////////////////////////
void CRedEyeTab::OnTimer(UINT nIDEvent, TIMERPROC TimerProc)
{
	if (nIDEvent == TAB_TIMER && GetCommand(_T("GetCanUndo")))
		EnableButtons(/*fEnableApply*/true, /*fEnableReset*/true);
}
