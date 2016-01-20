#pragma once

#include "ToolsCtlPanel.h"

class CCtp;

class CToolsPanelImage2 : public CToolsCtlPanel
{
public:
	CToolsPanelImage2(CCtp* pMainWnd);
	virtual ~CToolsPanelImage2();

public:
	enum { IDD = IDD_BOTTOOLSPANEL_DEFAULT };
	void UpdateControls();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CToolsPanelImage2)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CToolsCtlPanel)
	END_MSG_MAP()
};