#pragma once

#define COMBO_NOTIFCATION_FLAG 0xC000

class CCombo;

class CCombo : public CWindowImpl<CCombo, CComboBox>
{
public:
	CCombo();
	virtual ~CCombo();
	void InitDialog(HWND hWndCtrl);
	bool GetAfterCloseUp();

public:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnComboCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	BEGIN_MSG_MAP(CCombo)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		REFLECTED_COMMAND_RANGE_HANDLER(0/*idFirst*/, 9999/*idLast*/, OnComboCommand)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

protected:
	bool m_bEditable;
	bool m_bAfterCloseUp;
	WORD m_wLastNotifyCode;
};
