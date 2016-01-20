#pragma once

#include "ToolsCtlPanelCommon.h"

class CCtp;

class CToolsCtlPanelGraphics : public CToolsCtlPanelCommon
{
public:
	CToolsCtlPanelGraphics(CCtp* pMainWnd);
	virtual ~CToolsCtlPanelGraphics();

public:
	enum { IDD = IDD_TOOLSPANEL_GRAPHIC };

	void UpdateControls();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShapeNameComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	int GetCurrentShape() { return m_iCurrentShape; };

public:
	BEGIN_MSG_MAP(CToolsCtlPanelGraphics)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		//MESSAGE_HANDLER(WM_NOTIFY, OnNotify)

		COMMAND_ID_HANDLER(IDC_SHAPE_NAME, OnShapeNameComboChanged)

		//COMMAND_ID_HANDLER(IDC_FILL_COLOR, OnColorComboChanged)
		//COMMAND_ID_HANDLER(IDC_FILL_COLOR2, OnColorComboChanged)
		//COMMAND_ID_HANDLER(IDC_FILL_COLOR3, OnColorComboChanged)
		//COMMAND_ID_HANDLER(IDC_LINE_COLOR, OnColorComboChanged)

		COMMAND_ID_HANDLER(IDC_FILL_TYPE, OnFillType)
		COMMAND_ID_HANDLER(IDC_LINE_SIZE, OnLineWidth)

		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CToolsCtlPanelCommon)
	END_MSG_MAP()

protected:
	void SetShapeName(LPCSTR pShapeName);

protected:
	int m_iCurrentShape;

	CCombo m_ComboShapeName;
};