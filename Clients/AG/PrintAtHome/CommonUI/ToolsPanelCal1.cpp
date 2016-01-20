#include "stdafx.h"
#include "Ctp.h"
#include "ToolsPanelCal1.h"
#include "CalendarLayoutDlg.h"
#include "CalSupport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsPanelCal1::CToolsPanelCal1(CCtp* pMainWnd) : CToolsCtlPanelCommon(pMainWnd)
{
	m_nResID = IDD;
	m_iYearStyle = CALSTYLES::YR_UPPERCASE_YYYY;
	m_iWeekStyle = CALSTYLES::WK_ABBR_LOWERCASE;
	m_iTitleSize = CALSTYLES::TITLESIZE_1;
	m_iDayOfWkSize = CALSTYLES::WEEKSIZE_1;
	m_iWeekStart = CALSTYLES::SUNDAY_START;
	m_iCalendarMonth = 1;
	m_iCalendarYear = 2004;
}

//////////////////////////////////////////////////////////////////////
CToolsPanelCal1::~CToolsPanelCal1()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelCal1::UpdateControls()
{
	if (m_nResID != IDD_TOPTOOLSPANEL_CALENDAR)
		return;

	CAGSym* pSym = m_pCtp->GetDocWindow()->GetSelectedItem();
	if (!pSym)
		return;

	if (!pSym->IsCalendar())
		return;

	CAGSymCalendar* pCalSym = (CAGSymCalendar*)pSym;

	SetCalendarMonth(pCalSym->GetMonth());
	SetCalendarYear(pCalSym->GetYear());

	SetHideBoxes(pCalSym->GetHideBoxes());
	SetHideAllBoxes(pCalSym->GetHideAllBoxes());

	SetYearStyle(pCalSym->GetYearStyle());
	SetWeekStyle(pCalSym->GetWeekStyle());

	SetTitleSize(pCalSym->GetTitleSize());
	SetDayOfWkSize(pCalSym->GetDayOfWkSize());

	SetWeekStart(pCalSym->GetWeekStart());
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelCal1::SetCalendarYear(int iYear)
{
	if (iYear == -1)
	{
		m_ComboCalendarYear.SetCurSel(-1);
		return;
	}

	m_iCalendarYear = iYear;

	// Find a matching year in the combobox
	int nItems = m_ComboCalendarYear.GetCount();
	int nItemBefore = 0;
	int nItem = 0;
	for (nItem = 0; nItem < nItems; nItem++)
	{
		int iItemData = (int)m_ComboCalendarYear.GetItemData(nItem);
		if (m_iCalendarYear == iItemData)
			break;

		if (m_iCalendarYear > iItemData)
			nItemBefore = nItem + 1;
	}

	if (nItem >= nItems)
	{
		CString strText;
		strText.Format("%d", m_iCalendarYear);
		if (nItemBefore < nItems)
			nItem = m_ComboCalendarYear.InsertString(nItemBefore, strText);
		else
			nItem = m_ComboCalendarYear.AddString(strText);
		m_ComboCalendarYear.SetItemData(nItem, m_iCalendarYear);
	}
	
	m_ComboCalendarYear.SetCurSel(nItem);
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelCal1::SetCalendarMonth(int iMonth)
{
	if (iMonth == -1)
	{
		m_ComboCalendarMonth.SetCurSel(-1);
		return;
	}

	m_iCalendarMonth = iMonth;
	int nItems = m_ComboCalendarMonth.GetCount();
	iMonth--; // convert from a combo index to a month
	m_ComboCalendarMonth.SetCurSel((int)iMonth < nItems ? iMonth : -1);
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelCal1::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool bCalendarPanel = (m_nResID == IDD_TOPTOOLSPANEL_CALENDAR);
	if (bCalendarPanel)
	{
		m_ButtonMonth.InitDialog(GetDlgItem(IDC_CALENDAR_STYLEMONTH), IDR_CAL_BTNMONTH);
		m_ButtonWeek.InitDialog(GetDlgItem(IDC_CALENDAR_STYLEWEEK), IDR_CAL_BTNWEEK);
		m_ButtonDays.InitDialog(GetDlgItem(IDC_CALENDAR_STYLEDAYS), IDR_CAL_BTNDAYS);
		m_ButtonOptions.InitDialog(GetDlgItem(IDC_CALENDAR_LAYOUT), IDR_CAL_BTNOPTIONS);

		m_ComboCalendarMonth.InitDialog(GetDlgItem(IDC_CALENDAR_MONTH));
		LPCSTR Months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
		for (int i=0; i<12; i++)
		{
			int nItem = m_ComboCalendarMonth.AddString(Months[i]);
		}

		m_ComboCalendarMonth.SetCurSel(-1);

		m_ComboCalendarYear.InitDialog(GetDlgItem(IDC_CALENDAR_YEAR));
		for (int i=2000; i<=2020; i++)
		{
			CString strText;
			strText.Format("%d", i);
			int nItem = m_ComboCalendarYear.AddString(strText);
			m_ComboCalendarYear.SetItemData(nItem, i);
		}

		m_ComboCalendarYear.SetCurSel(-1);
	}

	CToolsCtlPanel::InitToolTips();

	bHandled = true;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//void CToolsPanelCal1::UpdateFillCtrls(WORD wID)
//{
//	if (wID == IDC_TRANS_COLOR_1)
//	{
//		m_ColorBtnFill.SetVisible(false);
//		m_ColorBtnFill.Invalidate();
//	}
//	else
//	if (wID == IDC_TRANS_COLOR_2)
//	{
//		m_ColorBtnFill2.SetVisible(false);
//		m_ColorBtnFill2.Invalidate();
//	}
//	else
//	if (wID == IDC_TRANS_COLOR_3)
//	{
//		m_ColorBtnFill3.SetVisible(false);
//		m_ColorBtnFill3.Invalidate();
//	}
//}

//////////////////////////////////////////////////////////////////////
//LRESULT CToolsPanelCal1::OnTransparent(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
//{
//	if (wID == IDC_TRANS_COLOR_1)
//	{
//		m_CurrentFillColor = CLR_NONE;
//		m_ColorBtnFill.SetVisible(false);
//		m_pCtp->GetDocWindow()->OnDocChangeFillColor(m_CurrentFillColor);
//	}
//	else
//	if (wID == IDC_TRANS_COLOR_2)
//	{
//		m_CurrentFillColor2 = CLR_NONE;
//		m_ColorBtnFill2.SetVisible(false);
//		m_pCtp->GetDocWindow()->OnDocChangeFillColor2(m_CurrentFillColor2);
//	}
//	else
//	if (wID == IDC_TRANS_COLOR_3)
//	{
//		m_CurrentFillColor3 = CLR_NONE;
//		m_ColorBtnFill3.SetVisible(false);
//		m_pCtp->GetDocWindow()->OnDocChangeFillColor3(m_CurrentFillColor3);
//	}
//
//	UpdateFillCtrls(wID);
//
//	return S_OK;
//}
//
//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelCal1::OnCalendarLayout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CCalendarLayoutDlg dlg(m_pCtp);
	dlg.SetYearStyle(m_iYearStyle);
	dlg.SetWeekStyle(m_iWeekStyle);
	dlg.SetHideBoxes(m_iHideBoxes);
	dlg.SetHideAllBoxes(m_iHideAllBoxes);
	dlg.SetCalendarYear(m_iCalendarYear);
	dlg.SetCalendarMonth(m_iCalendarMonth);
	dlg.SetTitleSize(m_iTitleSize);
	dlg.SetDayOfWkSize(m_iDayOfWkSize);
	dlg.SetWeekStart(m_iWeekStart);

	dlg.DoModal();

	m_iYearStyle = dlg.GetYearStyle();
	m_iWeekStyle = dlg.GetWeekStyle();
	m_iHideBoxes = dlg.GetHideBoxes();
	m_iHideAllBoxes = dlg.GetHideAllBoxes();
	m_iTitleSize = dlg.GetTitleSize();
	m_iDayOfWkSize = dlg.GetDayOfWkSize();
	m_iWeekStart = dlg.GetWeekStart();

	m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARSTYLECHANGE);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelCal1::OnCalendarYear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!(wNotifyCode & COMBO_NOTIFCATION_FLAG))
	{ // Let the combo class take care of it
		bHandled = false;
		return S_OK;
	}

	wNotifyCode &= ~COMBO_NOTIFCATION_FLAG;

	int nItem = -1;

	if (wNotifyCode == CBN_EDITUPDATE)
	{
		nItem = 0;
		char szYear[20];
		szYear[0] = '\0';
		m_ComboCalendarYear.GetWindowText(szYear, sizeof(szYear));
		m_iCalendarYear = atoi(szYear);
		{
			 if (strlen(szYear) != 4)
				 return S_OK;

			 if (m_ComboCalendarYear.FindString(0, szYear) == CB_ERR)
			 {
				int nCount = m_ComboCalendarYear.GetCount();  
				int nIndex = ((m_iCalendarYear < (int)m_ComboCalendarYear.GetItemData(0))?0:nCount);
				nIndex = m_ComboCalendarYear.InsertString(nIndex, szYear);
				m_ComboCalendarYear.SetItemData(nIndex, m_iCalendarYear);
			 }
		}
	}
	else
	{
		nItem = m_ComboCalendarYear.GetCurSel();
		m_iCalendarYear = (int)m_ComboCalendarYear.GetItemData(nItem);
	}

	if (nItem >= 0)
		m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARYEARCHANGE);

	if (m_ComboCalendarYear.GetAfterCloseUp())
		m_pCtp->GetDocWindow()->SetFocus();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelCal1::OnCalendarMonth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!(wNotifyCode & COMBO_NOTIFCATION_FLAG))
	{ // Let the combo class take care of it
		bHandled = false;
		return S_OK;
	}

	wNotifyCode &= ~COMBO_NOTIFCATION_FLAG;

	int iMonth = m_ComboCalendarMonth.GetCurSel();
	if (iMonth >= 0)
	{
		m_iCalendarMonth = ++iMonth; // convert from a month to a combo index
	}

	m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_CALENDARMONTHCHANGE);

	if (m_ComboCalendarMonth.GetAfterCloseUp())
		m_pCtp->GetDocWindow()->SetFocus();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelCal1::OnCalendarStyleMonth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CAGSym* pSym = m_pCtp->GetDocWindow()->GetSelectedItem();
	if (!pSym->IsCalendar())
		return S_OK;

	CAGSymCalendar* pCalendarSym = (CAGSymCalendar*)pSym;
	pCalendarSym->SetMode(0);
	m_pCtp->SelectPanel(TEXT_PANEL);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelCal1::OnCalendarStyleWeek(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CAGSym* pSym = m_pCtp->GetDocWindow()->GetSelectedItem();
	if (!pSym->IsCalendar())
		return S_OK;

	CAGSymCalendar* pCalendarSym = (CAGSymCalendar*)pSym;
	pCalendarSym->SetMode(1);
	m_pCtp->SelectPanel(TEXT_PANEL);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelCal1::OnCalendarStyleDays(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CAGSym* pSym = m_pCtp->GetDocWindow()->GetSelectedItem();
	if (!pSym->IsCalendar())
		return S_OK;

	CAGSymCalendar* pCalendarSym = (CAGSymCalendar*)pSym;
	pCalendarSym->SetMode(2);
	m_pCtp->SelectPanel(TEXT_PANEL);
	return S_OK;
}
