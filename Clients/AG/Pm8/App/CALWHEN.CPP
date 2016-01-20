// calwhen.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"

#include <time.h>

#include "system.h"
#include "calinfo.h"
#include "calwhen.h"
#include "colordef.h"
#include "util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern SHORT days_in_a_month(SHORT year, SHORT month);
extern WEEKDAY first_day_of_a_month(SHORT year, MONTH month);

/////////////////////////////////////////////////////////////////////////////
// CCalendarWhenDlg dialog

CALENDAR_INFO  CCalendarWhenDlg::StaticInfo;

CCalendarWhenDlg::CCalendarWhenDlg(CALENDAR_INFO_REF info, BOOL full_dialog, DWORD WizFlags, COLORREF cr, CWnd* pParent /*=NULL*/)
	: CPmwDialogColor(CCalendarWhenDlg::IDD, pParent, cr)
{
	type = (CALENDAR_TYPE)info.style.type;

   Afx_Data_Init ();

	m_type = info.style.type;

	m_full_dialog = full_dialog;
	vars_from_info(info);
	m_WizFlags = WizFlags;
}

CCalendarWhenDlg::CCalendarWhenDlg (BOOL full_dialog, DWORD WizFlags, COLORREF cr, CWnd* pParent /*=NULL*/)
	: CPmwDialogColor(CCalendarWhenDlg::IDD, pParent, cr)
{
	type = (CALENDAR_TYPE)StaticInfo.style.type;

   Afx_Data_Init ();

	m_type = StaticInfo.style.type;

	m_full_dialog = full_dialog;
	vars_from_info(StaticInfo);
	m_WizFlags = WizFlags;
}

void
CCalendarWhenDlg::Afx_Data_Init (void)
{
	//{{AFX_DATA_INIT(CCalendarWhenDlg)
	m_week = -1;
	m_numMonths = 1;
	//}}AFX_DATA_INIT
}

void CCalendarWhenDlg::vars_from_info(CALENDAR_INFO_REF info)
{
	m_month = info.month;
	m_year = info.year;
	m_week = info.week;
}

void CCalendarWhenDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarWhenDlg)
	DDX_Control(pDX, IDC_SPIN_EDNUMBER, m_ctlSpinNum);
	DDX_CBIndex(pDX, IDC_CALWHEN_MONTH, m_month);
	DDX_Text(pDX, IDC_CALWHEN_YEAR, m_year);
	DDV_MinMaxInt(pDX, m_year, 1, 9999);
	DDX_Radio(pDX, IDC_CALWHEN_MONTHLY, m_type);
	DDX_CBIndex(pDX, IDC_CALWHEN_WEEK, m_week);
	DDX_Text(pDX, IDC_CALWHEN_EDNUMBER, m_numMonths);
	DDV_MinMaxInt(pDX, m_numMonths, 1, 24);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCalendarWhenDlg, CPmwDialogColor)
	//{{AFX_MSG_MAP(CCalendarWhenDlg)
	ON_BN_CLICKED(IDC_CALWHEN_NOW, OnCalwhenNow)
	ON_BN_CLICKED(IDC_CALWHEN_MONTHLY, OnClickedType)
	ON_CBN_SELCHANGE(IDC_CALWHEN_MONTH, OnMonthChange)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_BN_CLICKED(IDC_CALWHEN_WEEKLY, OnClickedType)
	ON_BN_CLICKED(IDC_CALWHEN_GENERIC, OnClickedType)
	ON_EN_KILLFOCUS(IDC_CALWHEN_YEAR, OnMonthChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarWhenDlg message handlers

BOOL CCalendarWhenDlg::get_exit_info(CALENDAR_INFO_REF info)
{
	BOOL changed = FALSE;
	if (info.month != m_month)
	{
		info.month = (MONTH)m_month;
		changed = TRUE;
	}
	if (info.year != m_year)
	{
		info.year = m_year;
		changed = TRUE;
	}

	if (m_week == -1)
	{
		m_week = 0;
	}
	if (info.week != m_week)
	{
		info.week = m_week;
		changed = TRUE;
	}

	if (m_full_dialog)
	{
		info.style.type = (CALENDAR_TYPE)m_type;
	}

	return changed;
}

/*
// Set the dialog entries to "now".
*/

void CCalendarWhenDlg::OnCalwhenNow()
{
	CALENDAR_INFO info;

/* Compute "now". */

	SYSTEMTIME Now;
	GetLocalTime(&Now);

	info.month = (MONTH)Now.wMonth-1;
	info.year = Now.wYear;

/* Compute the week. */

	info.week = 0;

	int weekday = Now.wDayOfWeek;
	int day = Now.wDay-1;			// 0 based

	while (weekday < day)
	{
		info.week++;
		day -= 7;
	}

	UpdateData(TRUE);
	vars_from_info(info);
	UpdateData(FALSE);
}

BOOL CCalendarWhenDlg::OnInitDialog()
{
	CPmwDialogColor::OnInitDialog();

	OnClickedType();				// Handle the weekly selection

/* Come to grips with your limitations... */

	CEdit *edit;

	if ((edit = (CEdit *)GetDlgItem(IDC_CALWHEN_YEAR)) != NULL)
	{
		edit->LimitText(4);
	}

	if (!m_full_dialog)
	{
		enable_child(IDC_CALWHEN_MONTHLY, FALSE);
		enable_child(IDC_CALWHEN_WEEKLY, FALSE);
		enable_child(IDC_CALWHEN_GENERIC, FALSE);
		enable_child(IDC_CALWHEN_TTYPE, FALSE);
		enable_child(IDC_CALWHEN_TNUMBER, FALSE);
		enable_child(IDC_CALWHEN_EDNUMBER, FALSE);
		enable_child(IDC_SPIN_EDNUMBER, FALSE);
	}

   CreateWizzardButtons ();
   EnableWizardButtons (m_WizFlags);

	// set range on selection of number of months
	m_ctlSpinNum.SetRange (1, 24);
	m_ctlSpinNum.SetPos(1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCalendarWhenDlg::OnClickedType()
{
	update_week_selection(TRUE);
	update_number_selection();
}

VOID CCalendarWhenDlg::enable_child(UINT id, BOOL enable)
{
	CWnd *child;

	if ((child = GetDlgItem(id)) != NULL)
	{
		child->ShowWindow(enable ? SW_SHOW : SW_HIDE);
		child->EnableWindow(enable);
	}
}

void CCalendarWhenDlg::OnMonthChange()
{
/*
// The month or year has changed.
// Build the "week" strings.
*/

	update_week_selection(FALSE);
}

/*
// Return a suffix for a day number.
*/

PUBLIC CHAR *
suffix_for_day(SHORT day)
{
	switch (day)
	{
		case 1:
		case 21:
		case 31:
		{
			return "st";
		}
		case 2:
		case 22:
		{
			return "nd";
		}
		case 3:
		case 23:
		{
			return "rd";
		}
		default:
		{
			return "th";
		}
	}
}

VOID CCalendarWhenDlg::update_week_selection(BOOL type_change)
{
/* See if we're on the weekly selection. */

	BOOL weekly = (GetCheckedRadioButton(IDC_CALWHEN_MONTHLY, IDC_CALWHEN_GENERIC) 
							== IDC_CALWHEN_WEEKLY);

	if (type_change)
	{
		enable_child(IDC_CALWHEN_WEEK, weekly);
		enable_child(IDC_CALWHEN_TWEEK, weekly);
	}

	CComboBox *list;

	if ((list = (CComboBox *)GetDlgItem(IDC_CALWHEN_WEEK)) != NULL)
	{
		if (weekly)
		{
			SHORT week_save = m_week;
			UpdateData(TRUE);
			m_week = week_save;

			SHORT month = m_month;
			SHORT year = m_year;

		/* This is a weekly calendar. Build the weeks for this month. */

			list->ResetContent();

			SHORT weekday = first_day_of_a_month(year, (MONTH)month);
			SHORT days = days_in_a_month(year, month);

			for (SHORT day = 0; day < days; )
			{
				day++;
				SHORT last_day = day + (6 - weekday);

				if (last_day > days)
				{
					last_day = days;
				}

				CHAR buffer[45];

				sprintf(buffer, "%d%s to %d%s",
 							day, suffix_for_day(day),
 							last_day, suffix_for_day(last_day));

				list->AddString(buffer);

				weekday = 0;
				day = last_day;
			}

		/* Set the current selection. */

			if (m_week == -1)
			{
				m_week = 0;
			}
			list->SetCurSel(m_week);
		}
		else
		{
		/*
 		// This is not a weekly calendar. If we changed types, remove all
		// week strings.
 		*/
			if (type_change)
			{
				list->ResetContent();
			}
		}
	}
}

void CCalendarWhenDlg::OnBack() 
{
   EndDialog (IDC_BACK);
}

void CCalendarWhenDlg::update_number_selection ()
{

	BOOL monthly = (GetCheckedRadioButton(IDC_CALWHEN_MONTHLY, IDC_CALWHEN_GENERIC) 
							== IDC_CALWHEN_MONTHLY);

	enable_child(IDC_CALWHEN_EDNUMBER, monthly);
	enable_child(IDC_CALWHEN_TNUMBER, monthly);
	enable_child(IDC_SPIN_EDNUMBER, monthly);
	
}
