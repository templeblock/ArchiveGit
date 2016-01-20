#pragma once

#include "resource.h"

class CCtp;
class CAGDoc;

class CCalendarLayoutDlg : public CDialogImpl<CCalendarLayoutDlg>
{
public:

	CCalendarLayoutDlg(CCtp* pCtp);
	~CCalendarLayoutDlg();

	enum { IDD = IDD_CALENDAR_LAYOUT };

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	void SetCalendarMonth(int iMonth) { m_iCalMonth = iMonth; }
	void SetCalendarYear(int iYear) { m_iCalYear = iYear; }

	UINT GetYearStyle()      { return m_iCurrYrStyle; }
	UINT GetWeekStyle()      { return m_iCurrWkStyle; }
	bool GetHideBoxes()		 { return m_bCurrHideBoxes; }
	bool GetHideAllBoxes()   { return m_bCurrHideAllBoxes; }
	UINT GetTitleSize()      { return m_iCurrTitleSize; }
	UINT GetDayOfWkSize()    { return m_iCurrWkSize; }
	bool GetWeekStart()      { return m_bCurrWeekStart; }

	void SetYearStyle(UINT yearStyle) { m_iYrStyle = yearStyle; }
	void SetWeekStyle(UINT weekStyle) { m_iWkStyle = weekStyle; }
	void SetHideBoxes(bool bHide)	  { m_bHideBoxes = bHide; }
	void SetHideAllBoxes(bool bHide)  { m_bHideAllBoxes = bHide; }
	void SetTitleSize(UINT tSize)     { m_iTitleSize = tSize; }
	void SetDayOfWkSize(UINT wkSize)  { m_iWkSize = wkSize; }
	void SetWeekStart(bool bOn)		  { m_bWeekStart = bOn; }

public:
	BEGIN_MSG_MAP(CCalendarLayoutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_STYLE_MONTHYEAR, CBN_SELCHANGE, OnCbnSelchangeStyleMonthyear)
		COMMAND_HANDLER(IDC_STYLE_WEEK, CBN_SELCHANGE, OnCbnSelchangeStyleWeek)
		COMMAND_HANDLER(IDC_UNUSEDBOXES_INVISIBLE, BN_CLICKED, OnBnClickedUnusedboxesInvisible)
		COMMAND_HANDLER(IDC_ALLBOXES, BN_CLICKED, OnBnClickedAllboxes)
		COMMAND_HANDLER(IDC_LASTROW_BOXES, BN_CLICKED, OnBnClickedLastrowBoxes)
		COMMAND_HANDLER(IDC_TITLE_SIZE, CBN_SELCHANGE, OnCbnSelchangeTitleSize)
		COMMAND_HANDLER(IDC_WEEK_SIZE, CBN_SELCHANGE, OnCbnSelchangeWeekSize)
		COMMAND_HANDLER(IDC_SUNDAY_START, BN_CLICKED, OnBnClickedSundayStart)
		COMMAND_HANDLER(IDC_MONDAY_START, BN_CLICKED, OnBnClickedMondayStart)
	END_MSG_MAP()

private:
	void StuffCombos();
	void Initialize();
	void InitCombo(UINT msg, UINT cmpVal = 0);

private:
	CCtp* m_pCtp;

	UINT m_iCurrYrStyle;
	UINT m_iYrStyle;
	UINT m_iCurrWkStyle;
	UINT m_iWkStyle;
	UINT m_iCurrTitleSize;
	UINT m_iTitleSize;
	UINT m_iCurrWkSize;
	UINT m_iWkSize;

	bool m_bWeekStart;
	bool m_bCurrWeekStart;

	bool m_bCurrHideBoxes;
	bool m_bHideBoxes;
	bool m_bCurrHideAllBoxes;
	bool m_bHideAllBoxes;

	int m_iCurrCalMonth;
	int m_iCalMonth;
	int m_iCurrCalYear;
	int m_iCalYear;

	//HBRUSH m_hbrBackground;

public:
	LRESULT OnCbnSelchangeStyleMonthyear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCbnSelchangeStyleWeek(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedUnusedboxesInvisible(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedAllboxes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedLastrowBoxes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCbnSelchangeTitleSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCbnSelchangeWeekSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedSundayStart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedMondayStart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCtlColorDlg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
