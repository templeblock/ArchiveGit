#pragma once

#include "resource.h"
#include "BaseTab.h"

// CRedEyeTab dialog

class CRedEyeTab : public CBaseTab<CRedEyeTab>
{
public:
	CRedEyeTab();
	virtual ~CRedEyeTab();

	BEGIN_MSG_MAP_EX(CRedEyeTab)
		COMMAND_HANDLER_EX(IDC_IME_APPLY, BN_CLICKED, OnApply)
		COMMAND_HANDLER_EX(IDC_IME_RESET, BN_CLICKED, OnReset)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(CBaseTab<CRedEyeTab>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DDX_MAP(CRedEyeTab)
	END_DDX_MAP()

	enum { IDD = IDD_IME_REDEYE_TAB };

protected:
	virtual BOOL OnInitDialog(HWND hWnd, LPARAM lParam);

private:
	void OnEnterTab(bool bFirstTime = false);
	void OnLeaveTab();
	void OnApply(UINT uCode, int nID, HWND hwndCtrl);
	void OnReset(UINT uCode, int nID, HWND hwndCtrl);
	void OnTimer(UINT nIDEvent, TIMERPROC TimerProc);
};
