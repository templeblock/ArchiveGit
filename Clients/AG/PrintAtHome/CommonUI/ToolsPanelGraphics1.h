#pragma once

#include "ToolsCtlPanelCommon.h"

class CCtp;

class CToolsPanelGraphics1 : public CToolsCtlPanelCommon
{
public:
	CToolsPanelGraphics1(CCtp* pMainWnd);
	virtual ~CToolsPanelGraphics1();

public:
	enum { IDD = IDD_TOPTOOLSPANEL_GRAPHIC };

	void UpdateControls();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShapeNameComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	int GetCurrentShape() { return m_iCurrentShape; };

public:
	BEGIN_MSG_MAP(CToolsPanelGraphics1)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		//MESSAGE_HANDLER(WM_NOTIFY, OnNotify)

		COMMAND_ID_HANDLER(IDC_SHAPE_NAME, OnShapeNameComboChanged)

		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CToolsCtlPanelCommon)
	END_MSG_MAP()

protected:
	void SetShapeName(LPCSTR pShapeName);

protected:
	int m_iCurrentShape;

	CCombo m_ComboShapeName;
};