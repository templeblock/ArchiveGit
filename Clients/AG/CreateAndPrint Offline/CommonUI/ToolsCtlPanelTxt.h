#pragma once

#include "ToolsCtlPanelCommon.h"

class CCtp;

class CToolsCtlPanelText : public CToolsCtlPanelCommon
{
public:
	CToolsCtlPanelText(CCtp* pMainWnd);
	virtual ~CToolsCtlPanelText();

	enum { IDD = IDD_TOOLSPANEL_TEXT };

public:
	void SetFonts();
	void UpdateControls();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTextSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnHorzJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnVertJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBoldItalicUnderline(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	int GetCurrentTextSize() { return m_iTextSize; };

public:
	BEGIN_MSG_MAP(CCtlPanelText)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		COMMAND_ID_HANDLER(IDC_FILL_COLOR, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_FILL_COLOR2, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_FILL_COLOR3, OnColorComboChanged)
		COMMAND_ID_HANDLER(IDC_LINE_COLOR, OnColorComboChanged)

		COMMAND_ID_HANDLER(IDC_FILL_TYPE, OnFillType)
		COMMAND_ID_HANDLER(IDC_LINE_SIZE, OnLineWidth)

		COMMAND_ID_HANDLER(IDC_FONT, OnFont)
		COMMAND_ID_HANDLER(IDC_TEXT_SIZE, OnTextSize)

		COMMAND_ID_HANDLER(IDC_BOLD, OnBoldItalicUnderline)
		COMMAND_ID_HANDLER(IDC_ITALIC, OnBoldItalicUnderline)
		COMMAND_ID_HANDLER(IDC_UNDERLINE, OnBoldItalicUnderline)

		COMMAND_ID_HANDLER(IDC_LEFT, OnHorzJust)
		COMMAND_ID_HANDLER(IDC_CENTER, OnHorzJust)
		COMMAND_ID_HANDLER(IDC_RIGHT, OnHorzJust)

		COMMAND_ID_HANDLER(IDC_TOP, OnVertJust)
		COMMAND_ID_HANDLER(IDC_MIDDLE, OnVertJust)
		COMMAND_ID_HANDLER(IDC_BOTTOM, OnVertJust)

		//REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CToolsCtlPanelCommon)
	END_MSG_MAP()

protected:
	void UpdateTextControls(CAGText* pText);
	void InitToolCtrls();
	void InitToolTips();
	void SetTextSize(int iSize);
	void SetFontControl(LOGFONT* pFont, bool bClearIfBad);
	void UpdateCalendarTextControls(CAGSymCalendar* pCalendarSym);
	void UpdateTextAlignCtrls(CAGSymCalendar* pCalSym);

protected:
	CBmpButton m_ButtonBold;
	CBmpButton m_ButtonItalic;
	CBmpButton m_ButtonUnderline;
	CBmpButton m_ButtonLeft;
	CBmpButton m_ButtonCenter;
	CBmpButton m_ButtonRight;
	CBmpButton m_ButtonTop;
	CBmpButton m_ButtonMiddle;
	CBmpButton m_ButtonBottom;

	CCombo m_ComboFonts;
	CCombo m_ComboTextSize;

	int m_iTextSize;
};