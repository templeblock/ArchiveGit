#pragma once

#include "ToolsCtlPanelCommon.h"

class CCtp;

class CToolsPanelText2 : public CToolsCtlPanelCommon
{
public:
	CToolsPanelText2(CCtp* pMainWnd);
	virtual ~CToolsPanelText2();

	enum { IDD = IDD_BOTTOOLSPANEL_TEXT };

public:
	void UpdateControls();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CToolsPanelText2)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)

		COMMAND_ID_HANDLER(IDC_FILL_TYPE, OnFillType)
		COMMAND_ID_HANDLER(IDC_LINE_SIZE, OnLineWidth)

		COMMAND_ID_HANDLER(IDC_FILL_COLOR, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_FILL_COLOR2, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_FILL_COLOR3, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_LINE_COLOR, OnColorComboChanged)

		CHAIN_MSG_MAP(CToolsCtlPanelCommon)
	END_MSG_MAP()

protected:
	//void InitToolCtrls();
	//void InitToolTips();
	void UpdateTextControls(CAGText* pText);
	void UpdateCalendarTextControls(CAGSymCalendar* pCalendarSym);

protected:
};