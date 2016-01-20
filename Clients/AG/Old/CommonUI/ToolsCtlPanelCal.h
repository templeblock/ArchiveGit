#pragma once

#include "ToolsCtlPanelCommon.h"

class CCtp;

class CToolsCtlPanelCal : public CToolsCtlPanelCommon
{
public:
	CToolsCtlPanelCal(CCtp* pMainWnd);
	virtual ~CToolsCtlPanelCal();

	enum { IDD = IDD_TOOLSPANEL_CALENDAR };

public:
	void UpdateControls();

	UINT GetYearStyle() { return m_iYearStyle; }
	UINT GetWeekStyle() { return m_iWeekStyle; }
	UINT GetTitleSize()  { return m_iTitleSize; }
	UINT GetDayOfWkSize(){ return m_iDayOfWkSize; }
	bool GetWeekStart() { return m_iWeekStart; }

	void SetYearStyle(UINT yearStyle) { m_iYearStyle = yearStyle; }
	void SetWeekStyle(UINT weekStyle) { m_iWeekStyle = weekStyle; }
	void SetTitleSize(UINT titleSize) { m_iTitleSize = titleSize; }
	void SetDayOfWkSize(UINT wkSize)  { m_iDayOfWkSize = wkSize; }
	void SetWeekStart(bool bOn)   { m_iWeekStart = bOn; }

	bool GetHideBoxes() { return m_iHideBoxes; }
	bool GetHideAllBoxes() { return m_iHideAllBoxes; }
	void SetHideBoxes(bool bHide) { m_iHideBoxes = bHide; }
	void SetHideAllBoxes(bool bHide) { m_iHideAllBoxes = bHide; }

	int GetCurrentCalendarMonth() { return m_iCalendarMonth; };
	int GetCurrentCalendarYear() { return m_iCalendarYear; };

public:
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnTransparent(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCalendarLayout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCalendarYear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCalendarMonth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCalendarStyleMonth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCalendarStyleWeek(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCalendarStyleDays(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

public:
	BEGIN_MSG_MAP(CToolsCtlPanelCal)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		//COMMAND_ID_HANDLER(IDC_TRANS_COLOR_1, OnTransparent)
		//COMMAND_ID_HANDLER(IDC_TRANS_COLOR_2, OnTransparent)
		//COMMAND_ID_HANDLER(IDC_TRANS_COLOR_3, OnTransparent)
		COMMAND_ID_HANDLER(IDC_LINE_SIZE, OnLineWidth)

		COMMAND_ID_HANDLER(IDC_CALENDAR_LAYOUT, OnCalendarLayout)
		COMMAND_ID_HANDLER(IDC_CALENDAR_YEAR, OnCalendarYear)
		COMMAND_ID_HANDLER(IDC_CALENDAR_MONTH, OnCalendarMonth)
		COMMAND_ID_HANDLER(IDC_CALENDAR_STYLEMONTH, OnCalendarStyleMonth)
		COMMAND_ID_HANDLER(IDC_CALENDAR_STYLEWEEK, OnCalendarStyleWeek)
		COMMAND_ID_HANDLER(IDC_CALENDAR_STYLEDAYS, OnCalendarStyleDays)

		//MESSAGE_HANDLER(WM_NOTIFY, OnNotify);

		CHAIN_MSG_MAP(CToolsCtlPanelCommon)
	END_MSG_MAP()

protected:
	void SetCalendarYear(int iYear);
	void SetCalendarMonth(int iMonth);
	void UpdateFillCtrls(WORD wID);

protected:
	UINT m_iYearStyle;
	UINT m_iWeekStyle;
	UINT m_iTitleSize;
	UINT m_iDayOfWkSize;
	bool m_iWeekStart;

	int m_iCalendarMonth;
	int m_iCalendarYear;

	bool m_iHideBoxes;
	bool m_iHideAllBoxes;

	CCombo m_ComboCalendarYear;
	CCombo m_ComboCalendarMonth;

	CBmpButton m_ButtonMonth;
	CBmpButton m_ButtonWeek;
	CBmpButton m_ButtonDays;
	CBmpButton m_ButtonOptions;

	//CBmpButton m_ButtonClr1;
	//CBmpButton m_ButtonClr2;
	//CBmpButton m_ButtonClr3;
};