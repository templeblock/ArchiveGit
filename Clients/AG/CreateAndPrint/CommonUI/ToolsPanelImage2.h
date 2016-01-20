#pragma once

#include "ToolsCtlPanel.h"

class CCtp;

class CToolsPanelImage2 : public CToolsCtlPanel
{
public:
	CToolsPanelImage2(CCtp* pMainWnd);
	virtual ~CToolsPanelImage2();

public:
	enum { IDD = IDD_BOTTOOLSPANEL_IMAGE };
	void UpdateControls();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnIMEToolbar(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);


public:
	BEGIN_MSG_MAP(CToolsPanelImage2)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_IME_COLORART_CHECK, OnIMEToolbar)
		COMMAND_ID_HANDLER(IDC_IME_CROP_CHECK, OnIMEToolbar)
		COMMAND_ID_HANDLER(IDC_IME_FLIP_CHECK, OnIMEToolbar)
		COMMAND_ID_HANDLER(IDC_IME_CONTRAST_CHECK, OnIMEToolbar)
		COMMAND_ID_HANDLER(IDC_IME_TINT_CHECK, OnIMEToolbar)
		COMMAND_ID_HANDLER(IDC_IME_REDEYE_CHECK, OnIMEToolbar)
		COMMAND_ID_HANDLER(IDC_IME_BORDERS_CHECK, OnIMEToolbar)
		COMMAND_ID_HANDLER(IDC_IME_EFFECTS_CHECK, OnIMEToolbar)
		CHAIN_MSG_MAP(CToolsCtlPanel)
	END_MSG_MAP()

protected:
	void InitToolCtrls();
	void InitToolTips();
	void ResetToolbarButtons();

protected:
	CBmpButton m_ButtonColor;
	CBmpButton m_ButtonCrop;
	CBmpButton m_ButtonFlip;
	CBmpButton m_ButtonContrast;
	CBmpButton m_ButtonTint;
	CBmpButton m_ButtonRedeye;
	CBmpButton m_ButtonBorder;
	CBmpButton m_ButtonEffects;

};