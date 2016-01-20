#pragma once

#define COMBO_NOTIFCATION_FLAG 0xC000

class CColorCombo;

struct COLORENTRY {
	COLORREF Color;
	LPCSTR Name;
};

struct COLORCOMBOENTRY {
	CColorCombo* pColorCombo;
	COLORCOMBOENTRY* pNext;
};

class CColorCombo : public CWindowImpl<CColorCombo, CComboBox>
{
public:
	CColorCombo();
	virtual ~CColorCombo();
	void InitDialog(HWND hWndCtrl, bool bDefaultColors = true, bool bAllowOffOption = false);
	bool GetAfterCloseUp();
	COLORREF GetColor();
	void SelectColor(COLORREF Color, bool bCustomColorsOnly, bool bSetCurSel = true);

public:
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnComboCommand(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

private:
	static COLORREF m_SavedColor;
	static COLORCOMBOENTRY* m_pComboList;
	static const COLORENTRY m_DefaultColors[];

private:
	int FindColor(COLORREF Color, bool bCustomColorsOnly);
	int AddColorUserEntry(COLORREF Color);
	int AddColor(COLORREF Color);
	void AddComboToList();
	void RemoveComboFromList();
	LPCSTR GetColorName(COLORREF Color);
	bool PopupColorPicker(COLORREF& Color);

	BEGIN_MSG_MAP(CColorCombo)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem);
		REFLECTED_COMMAND_RANGE_HANDLER(0/*idFirst*/, 9999/*idLast*/, OnComboCommand)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

protected:
	COLORREF m_Color;
	bool m_bFocus;
	bool m_bAfterCloseUp;
	WORD m_wLastNotifyCode;
};
