#pragma once

#include "ToolsCtlPanelCommon.h"

class CCtp;

class CToolsPanelText1 : public CToolsCtlPanelCommon
{
public:
	CToolsPanelText1(CCtp* pMainWnd);
	virtual ~CToolsPanelText1();

	enum { IDD = IDD_TOPTOOLSPANEL_TEXT };

public:
	void SetFonts();
	void UpdateControls();
	void ActivateNewDoc();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTextSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnHorzJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnVertJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBoldItalicUnderline(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSpellCheck(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	//LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	int GetCurrentTextSize() { return m_iTextSize; };

public:
	BEGIN_MSG_MAP(CToolsPanelText1)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)

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

		COMMAND_ID_HANDLER(IDC_SPELLCHECK, OnSpellCheck)

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

	void FillFontSizeCombo();

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
	CBmpButton m_ButtonSpellChk;

	CCombo m_ComboFonts;
	CCombo m_ComboTextSize;

	int m_iTextSize;
};