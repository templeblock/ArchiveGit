#pragma once

#include "ToolsCtlPanel.h"

class CCtp;

class CToolsCtlPanelImage : public CToolsCtlPanel
{
public:
	CToolsCtlPanelImage(CCtp* pMainWnd);
	virtual ~CToolsCtlPanelImage();

public:
	enum { IDD = IDD_TOOLSPANEL_DEFAULT };
	void UpdateControls();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CToolsCtlPanelImage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CToolsCtlPanel)
	END_MSG_MAP()
};