#pragma once

#include "ToolsCtlPanel.h"

class CCtp;

class CToolsPanelImage1 : public CToolsCtlPanel
{
public:
	CToolsPanelImage1(CCtp* pMainWnd);
	virtual ~CToolsPanelImage1();

public:
	enum { IDD = IDD_TOPTOOLSPANEL_DEFAULT };
	void UpdateControls();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CToolsPanelImage1)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CToolsCtlPanel)
	END_MSG_MAP()
};