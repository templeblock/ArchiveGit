#pragma once

#include "ToolsCtlPanelCommon.h"

class CCtp;

class CToolsPanelGraphics2 : public CToolsCtlPanelCommon
{
public:
	CToolsPanelGraphics2(CCtp* pMainWnd);
	virtual ~CToolsPanelGraphics2();

public:
	enum { IDD = IDD_BOTTOOLSPANEL_GRAPHIC };

	void UpdateControls();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CToolsPanelGraphics2)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)

		COMMAND_ID_HANDLER(IDC_FILL_COLOR, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_FILL_COLOR2, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_FILL_COLOR3, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_LINE_COLOR, OnColorComboChanged)

		COMMAND_ID_HANDLER(IDC_FILL_TYPE, OnFillType)
		COMMAND_ID_HANDLER(IDC_LINE_SIZE, OnLineWidth)

		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CToolsCtlPanelCommon)
	END_MSG_MAP()

};