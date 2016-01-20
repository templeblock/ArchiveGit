#pragma once
#include "stdafx.h"
#include "HelperFunctions.h"

typedef void (CALLBACK * UTILITYCALLBACK)(UINT uMsg, LPARAM lParam1, LPARAM lParam2);

class CUtilityWindow :public CWindowImpl<CUtilityWindow>
{
public:
	CUtilityWindow(void);
	virtual ~CUtilityWindow(void);
	bool Initialize(HWND hWnd, UTILITYCALLBACK pCallBack, LPARAM lParam1, LPARAM lParam2 = NULL, bool bShow=false);

public:
	BEGIN_MSG_MAP(CUtilityWindow)
		MESSAGE_HANDLER(UWM_OPTIONS, OnRelayMessage)
		MESSAGE_HANDLER(UWM_PREFERENCES, OnRelayMessage)
		MESSAGE_HANDLER(UWM_SHUTDOWNALL, OnRelayMessage)
		MESSAGE_HANDLER(UWM_UPDATEADDINS, OnRelayMessage)
	END_MSG_MAP()

	LRESULT OnRelayMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	

protected:
	UTILITYCALLBACK m_pCallBack;
	HWND m_hCallbackWnd;
	LPARAM m_lParam1;
	LPARAM m_lParam2;
};
