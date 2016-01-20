#include "stdafx.h"
#include "Ctp.h"
#include "AGDoc.h"
#include "CalendarLayoutDlg.h"
#include "CalSupport.h"
#include ".\calendarlayoutdlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCalendarLayoutDlg::CCalendarLayoutDlg(CCtp* pCtp)
{
	m_pCtp = pCtp;
	m_iCurrYrStyle = m_iYrStyle = CALSTYLES::YR_UPPERCASE_YYYY;
	m_iCurrWkStyle = m_iWkStyle = CALSTYLES::WK_ABBR_LOWERCASE;

	m_iCurrTitleSize = m_iTitleSize = CALSTYLES::TITLESIZE_1;
	m_iCurrWkSize = m_iWkSize = CALSTYLES::WEEKSIZE_1;

	m_bCurrWeekStart = m_bWeekStart = CALSTYLES::SUNDAY_START;

	m_bCurrHideBoxes = m_bHideBoxes = false;
	m_bCurrHideAllBoxes = m_bHideAllBoxes = false;

	m_iCurrCalMonth = m_iCalMonth = 1;
	m_iCurrCalYear = m_iCalYear = 2004;

	//m_hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
}

//////////////////////////////////////////////////////////////////////
CCalendarLayoutDlg::~CCalendarLayoutDlg()
{
	//DeleteObject(m_hbrBackground);
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();
	Initialize();

	return IDOK;
}

//////////////////////////////////////////////////////////////////////
void CCalendarLayoutDlg::Initialize()
{
	m_iCurrYrStyle = m_iYrStyle;
	m_iCurrWkStyle = m_iWkStyle;
	m_bCurrHideBoxes = m_bHideBoxes;
	m_bCurrHideAllBoxes = m_bHideAllBoxes;
	m_iCurrCalMonth = m_iCalMonth;
	m_iCurrCalYear = m_iCalYear;
	m_iCurrTitleSize = m_iTitleSize;
	m_iCurrWkSize = m_iWkSize;
	m_bCurrWeekStart = m_bWeekStart;

	StuffCombos();
	InitCombo(IDC_STYLE_MONTHYEAR, m_iCurrYrStyle);
	InitCombo(IDC_STYLE_WEEK, m_iCurrWkStyle);
	InitCombo(IDC_TITLE_SIZE, m_iCurrTitleSize);
	InitCombo(IDC_WEEK_SIZE, m_iCurrWkSize);

	if (m_bCurrHideBoxes)
	{
		CheckDlgButton(IDC_UNUSEDBOXES_INVISIBLE, BST_CHECKED);
		::EnableWindow(GetDlgItem(IDC_LASTROW_BOXES), true);
		::EnableWindow(GetDlgItem(IDC_ALLBOXES), true);
	}
	else
	{
		CheckDlgButton(IDC_UNUSEDBOXES_INVISIBLE, BST_UNCHECKED);
		::EnableWindow(GetDlgItem(IDC_LASTROW_BOXES), false);
		::EnableWindow(GetDlgItem(IDC_ALLBOXES), false);
	}

	bool bSunStart = (m_bCurrWeekStart==CALSTYLES::SUNDAY_START);
	CheckRadioButton(IDC_ALLBOXES, IDC_LASTROW_BOXES, (m_bCurrHideAllBoxes?IDC_ALLBOXES:IDC_LASTROW_BOXES));
	CheckRadioButton(IDC_MONDAY_START, IDC_SUNDAY_START, (bSunStart?IDC_SUNDAY_START:IDC_MONDAY_START));
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//if (uMsg == WM_CTLCOLORDLG)
	//{
	//	return (long) m_hbrBackground;
	//}

	if (m_pCtp)
		return m_pCtp->OnCtlColor(uMsg, wParam, lParam, bHandled);

	return NULL;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int iSel;

	iSel = SendDlgItemMessage(IDC_STYLE_MONTHYEAR, CB_GETCURSEL, 0, 0);
	m_iCurrYrStyle = SendDlgItemMessage(IDC_STYLE_MONTHYEAR, CB_GETITEMDATA, iSel, 0);

	iSel = SendDlgItemMessage(IDC_STYLE_WEEK, CB_GETCURSEL, 0, 0);
	m_iCurrWkStyle = SendDlgItemMessage(IDC_STYLE_WEEK, CB_GETITEMDATA, iSel, 0);

	iSel = SendDlgItemMessage(IDC_TITLE_SIZE, CB_GETCURSEL, 0, 0);
	m_iCurrTitleSize = SendDlgItemMessage(IDC_TITLE_SIZE, CB_GETITEMDATA, iSel, 0);

	iSel = SendDlgItemMessage(IDC_WEEK_SIZE, CB_GETCURSEL, 0, 0);
	m_iCurrWkSize = SendDlgItemMessage(IDC_WEEK_SIZE, CB_GETITEMDATA, iSel, 0);

	m_bCurrHideBoxes = (IsDlgButtonChecked(IDC_UNUSEDBOXES_INVISIBLE) == BST_CHECKED);
	m_bCurrHideAllBoxes = (IsDlgButtonChecked(IDC_ALLBOXES) == BST_CHECKED);
	m_bCurrWeekStart = !(IsDlgButtonChecked(IDC_SUNDAY_START) == BST_CHECKED);

	EndDialog(IDOK);
	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_iCurrYrStyle = m_iYrStyle;
	m_iCurrWkStyle = m_iWkStyle;
	m_bCurrHideBoxes = m_bHideBoxes;
	m_bCurrHideAllBoxes = m_bHideAllBoxes;
	m_iCurrCalMonth = m_iCalMonth;
	m_iCurrCalYear = m_iCalYear;
	m_iCurrWkSize = m_iWkSize;
	m_iCurrTitleSize = m_iTitleSize;
	m_bCurrWeekStart = m_bWeekStart;

	EndDialog(IDCANCEL);
	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnBnClickedSundayStart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here 
	CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1*)(m_pCtp->GetCtlPanel(SELECTED_PANEL)->GetTopToolPanel());
	if (!pTopCalToolsPanel)
		return 0;

	m_bCurrWeekStart = !(IsDlgButtonChecked(IDC_SUNDAY_START) == BST_CHECKED);
	pTopCalToolsPanel->SetWeekStart(m_bCurrWeekStart);
	m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARSTYLECHANGE);

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnBnClickedMondayStart(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here CToolsCtlPanelCalendar
	CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1*)(m_pCtp->GetCtlPanel(SELECTED_PANEL)->GetTopToolPanel());
	if (!pTopCalToolsPanel)
		return 0;

	m_bCurrWeekStart = (IsDlgButtonChecked(IDC_MONDAY_START) == BST_CHECKED);
	pTopCalToolsPanel->SetWeekStart(m_bCurrWeekStart);
	m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARSTYLECHANGE);

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnCbnSelchangeStyleMonthyear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here 
	CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1 *)(m_pCtp->GetCtlPanel(SELECTED_PANEL)->GetTopToolPanel());
	if (!pTopCalToolsPanel)
		return 0;

	int nItem = SendDlgItemMessage(IDC_STYLE_MONTHYEAR, CB_GETCURSEL, 0, 0);
	m_iCurrYrStyle = SendDlgItemMessage(IDC_STYLE_MONTHYEAR, CB_GETITEMDATA, nItem, 0);
	pTopCalToolsPanel->SetYearStyle(m_iCurrYrStyle);
	if (nItem >= 0)
		m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARSTYLECHANGE);

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnCbnSelchangeStyleWeek(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here 
	CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1*)(m_pCtp->GetCtlPanel(SELECTED_PANEL)->GetTopToolPanel());
	if (!pTopCalToolsPanel)
		return 0;

	int nItem = SendDlgItemMessage(IDC_STYLE_WEEK, CB_GETCURSEL, 0, 0);
	m_iCurrWkStyle = SendDlgItemMessage(IDC_STYLE_WEEK, CB_GETITEMDATA, nItem, 0);
	pTopCalToolsPanel->SetWeekStyle(m_iCurrWkStyle);
	if (nItem >= 0)
		m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARSTYLECHANGE);

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnCbnSelchangeTitleSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here 
	CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1 *)(m_pCtp->GetCtlPanel(SELECTED_PANEL)->GetTopToolPanel());
	if (!pTopCalToolsPanel)
		return 0;

	int nItem = SendDlgItemMessage(IDC_TITLE_SIZE, CB_GETCURSEL, 0, 0);
	m_iCurrTitleSize = SendDlgItemMessage(IDC_TITLE_SIZE, CB_GETITEMDATA, nItem, 0);
	pTopCalToolsPanel->SetTitleSize(m_iCurrTitleSize);
	if (nItem >= 0)
		m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARSTYLECHANGE);

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnCbnSelchangeWeekSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here 
	CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1 *)(m_pCtp->GetCtlPanel(SELECTED_PANEL)->GetTopToolPanel());
	if (!pTopCalToolsPanel)
		return 0;

	int nItem = SendDlgItemMessage(IDC_WEEK_SIZE, CB_GETCURSEL, 0, 0);
	m_iCurrWkSize = SendDlgItemMessage(IDC_WEEK_SIZE, CB_GETITEMDATA, nItem, 0);
	pTopCalToolsPanel->SetDayOfWkSize(m_iCurrWkSize);
	if (nItem >= 0)
		m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARSTYLECHANGE);

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnBnClickedUnusedboxesInvisible(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here 
	CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1 *)(m_pCtp->GetCtlPanel(SELECTED_PANEL)->GetTopToolPanel());
	if (!pTopCalToolsPanel)
		return 0;

	m_bCurrHideBoxes = (IsDlgButtonChecked(IDC_UNUSEDBOXES_INVISIBLE) == BST_CHECKED);
	m_bCurrHideAllBoxes = (IsDlgButtonChecked(IDC_ALLBOXES) == BST_CHECKED);

	if (m_bCurrHideBoxes)
	{
		::EnableWindow(GetDlgItem(IDC_LASTROW_BOXES), true);
		::EnableWindow(GetDlgItem(IDC_ALLBOXES), true);
	}
	else
	{
		::EnableWindow(GetDlgItem(IDC_LASTROW_BOXES), false);
		::EnableWindow(GetDlgItem(IDC_ALLBOXES), false);
	}

	pTopCalToolsPanel->SetHideBoxes(m_bCurrHideBoxes);
	pTopCalToolsPanel->SetHideAllBoxes(m_bCurrHideAllBoxes);
	m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARSTYLECHANGE);

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnBnClickedAllboxes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here
	CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1 *)(m_pCtp->GetCtlPanel(SELECTED_PANEL)->GetTopToolPanel());
	if (!pTopCalToolsPanel)
		return 0;

	m_bCurrHideAllBoxes = (IsDlgButtonChecked(IDC_ALLBOXES) == BST_CHECKED);
	pTopCalToolsPanel->SetHideAllBoxes(m_bCurrHideAllBoxes);
	m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARSTYLECHANGE);

	return 0;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCalendarLayoutDlg::OnBnClickedLastrowBoxes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// TODO: Add your control notification handler code here 
	CToolsPanelCal1* pTopCalToolsPanel = (CToolsPanelCal1 *)(m_pCtp->GetCtlPanel(SELECTED_PANEL)->GetTopToolPanel());
	if (!pTopCalToolsPanel)
		return 0;

	m_bCurrHideAllBoxes = !(IsDlgButtonChecked(IDC_LASTROW_BOXES) == BST_CHECKED);
	pTopCalToolsPanel->SetHideAllBoxes(m_bCurrHideAllBoxes);
	m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARSTYLECHANGE);

	return 0;
}

//////////////////////////////////////////////////////////////////////
void CCalendarLayoutDlg::InitCombo(UINT msg, UINT cmpVal)
{
	int nCount = SendDlgItemMessage(msg, CB_GETCOUNT, 0, 0);
	for (int i=0; i<nCount; i++)
	{
		if ( SendDlgItemMessage(msg, CB_GETITEMDATA, i, 0) == cmpVal)
		{
			SendDlgItemMessage(msg, CB_SETCURSEL, i, 0);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CCalendarLayoutDlg::StuffCombos()
{
	int iIndex = -1;
	COleDateTime Date(m_iCurrCalYear, m_iCurrCalMonth, 1, 0,0,0);
	for (int i=CALSTYLES::YR_UPPERCASE; i<=CALSTYLES::YR_ABBR_LOWERCASE_YYYY; i++)
	{
		iIndex = SendDlgItemMessage(IDC_STYLE_MONTHYEAR, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)CCalSupport::YearStr(Date, i));
				SendDlgItemMessage(IDC_STYLE_MONTHYEAR, CB_SETITEMDATA, iIndex, i);
	}

	// Combo for Week Styles
	for (int i=CALSTYLES::WK_UPPERCASE; i<=CALSTYLES::WK_SINGLE_MIXED; i++)
	{
		iIndex = SendDlgItemMessage(IDC_STYLE_WEEK, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)CCalSupport::WeekStr(0, i));
				SendDlgItemMessage(IDC_STYLE_WEEK, CB_SETITEMDATA, iIndex, i);
	}

	// Combo for Title Sizes
	for (int x=1, i=CALSTYLES::TITLESIZE_MIN; i<=CALSTYLES::TITLESIZE_MAX; i+=CALSTYLES::TITLESIZE_INCR, x++)
	{
		iIndex = SendDlgItemMessage(IDC_TITLE_SIZE, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)String("%d", x)); //CCalSupport::TitleSizeStr(i)
				 SendDlgItemMessage(IDC_TITLE_SIZE, CB_SETITEMDATA, iIndex, i);
	}

	// Combo for DayOfWeek Size
	for (int x=1, i=CALSTYLES::WEEKSIZE_MIN; i<=CALSTYLES::WEEKSIZE_MAX; i+=CALSTYLES::WEEKSIZE_INCR, x++)
	{
		iIndex = SendDlgItemMessage(IDC_WEEK_SIZE, CB_ADDSTRING, 0, (LPARAM)(LPCSTR)String("%d", x)); //CCalSupport::DayOfWkSizeStr(i)
				 SendDlgItemMessage(IDC_WEEK_SIZE, CB_SETITEMDATA, iIndex, i);
	}
}
