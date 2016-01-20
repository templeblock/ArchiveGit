#pragma once

#include "ToolsCtlPanelCommon.h"

class CCtp;

class CToolsPanelCal2 : public CToolsCtlPanelCommon
{
public:
	CToolsPanelCal2(CCtp* pMainWnd);
	virtual ~CToolsPanelCal2();

	enum { IDD = IDD_BOTTOOLSPANEL_CALENDAR };

public:
	void UpdateControls();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CToolsPanelCal2)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)

		COMMAND_ID_HANDLER(IDC_FILL_COLOR, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_FILL_COLOR2, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_FILL_COLOR3, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_LINE_COLOR, OnColorComboChanged)

		COMMAND_ID_HANDLER(IDC_LINE_SIZE, OnLineWidth)

		CHAIN_MSG_MAP(CToolsCtlPanelCommon)
	END_MSG_MAP()

protected:
};