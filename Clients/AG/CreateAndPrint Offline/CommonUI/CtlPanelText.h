// CtlPanelText.h: interface for the CCtlPanelText class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CtlPanel.h"
#include "Ctp.h"

class CCtlPanelText : public CCtlPanel  
{
public:
	CCtlPanelText(CCtp* pMainWnd);
	virtual ~CCtlPanelText();

	void UpdateControls();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHorzJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnVertJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

protected:
	void UpdateCalendarTextControls(CAGSymCalendar* pCalendarSym);
	void UpdateTextAlignCtrls(CAGSymCalendar* pCalSym);

public:
	BEGIN_MSG_MAP(CCtlPanelText)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		COMMAND_ID_HANDLER(IDC_LEFT, OnHorzJust)
		COMMAND_ID_HANDLER(IDC_CENTER, OnHorzJust)
		COMMAND_ID_HANDLER(IDC_RIGHT, OnHorzJust)

		COMMAND_ID_HANDLER(IDC_TOP, OnVertJust)
		COMMAND_ID_HANDLER(IDC_MIDDLE, OnVertJust)
		COMMAND_ID_HANDLER(IDC_BOTTOM, OnVertJust)

		CHAIN_MSG_MAP(CCtlPanel)
	END_MSG_MAP()
};
