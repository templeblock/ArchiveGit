// CalDateD.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "CalDateD.h"
#include "util.h"

#ifdef LOCALIZE
#include "clocale.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern SHORT days_in_a_month(SHORT year, SHORT month);
extern WEEKDAY first_day_of_a_month(SHORT year, MONTH month);
extern short GetWeeksInMonth(short year, short month); // located in caldoc.cpp

/////////////////////////////////////////////////////////////////////////////
// CCalendarDateDlg dialog


PUBLIC INT DecrementMonth(int month)
{
	if (month == 0)
		month = 11;
	else 
		month--;
	return month;
}

PUBLIC INT IncrementMonth(int month)
{
	if (month == 11)
		month = 0;
	else 
		month++;
	return month;
}

CCalendarDateDlg::CCalendarDateDlg(CALENDAR_INFO_REF info, BOOL bFull, DWORD flags, COLORREF cr, short n_o_c, CWnd* pParent /*=NULL*/)
: CPmwDialogColor(CCalendarDateDlg::IDD, pParent, cr)
{
	m_info = info;

	//{{AFX_DATA_INIT(CCalendarDateDlg)
	m_iMonth = info.month;
	m_iWeek = info.week;
	m_iYear = info.year;
	m_iNumObjects = n_o_c;
	//}}AFX_DATA_INIT
		
	m_bFull = bFull;
	m_WizFlags = flags; // WIZBUT_BACK | WIZBUT_NEXT | WIZBUT_CANCEL | WIZBUT_HELP;
}


void CCalendarDateDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialogColor::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarDateDlg)
	DDX_Control(pDX, IDC_CALWHEN_EDNUMBER, m_ctlEdNumber);
	DDX_Control(pDX, IDC_YEAR_LABEL, m_ctlYearText);
	DDX_Control(pDX, IDC_WEEK_LABEL, m_ctlWeekText);
	DDX_Control(pDX, IDC_SPIN_YEAR, m_ctlSpinYear);
	DDX_Control(pDX, IDC_SPIN_EDNUMBER, m_ctlSpinNumber);
	DDX_Control(pDX, IDC_MONTH_LABEL, m_ctlMonthText);
	DDX_Control(pDX, IDC_CALWHEN_WEEK, m_ctlWeek);
	DDX_Control(pDX, IDC_CALWHEN_TNUMBER, m_ctlNumOfX);
	DDX_Control(pDX, IDC_CALWHEN_NOW, m_btnNow);
	DDX_Control(pDX, IDC_CALWHEN_MONTH, m_ctlMonth);
	DDX_CBIndex(pDX, IDC_CALWHEN_MONTH, m_iMonth);
	DDX_CBIndex(pDX, IDC_CALWHEN_WEEK, m_iWeek);
	DDX_Text(pDX, IDC_EDIT_YEAR, m_iYear);
	DDV_MinMaxInt(pDX, m_iYear, 1, 9999);
	DDX_Text(pDX, IDC_CALWHEN_EDNUMBER, m_iNumObjects);
	DDV_MinMaxInt(pDX, m_iNumObjects, 1, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCalendarDateDlg, CPmwDialogColor)
	//{{AFX_MSG_MAP(CCalendarDateDlg)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_BN_CLICKED(IDC_FINISH, OnFinish)
	ON_BN_CLICKED(IDC_CALWHEN_NOW, OnCalwhenNow)
	ON_CBN_SELCHANGE(IDC_CALWHEN_MONTH, OnSelchangeCalwhenMonth)
	ON_EN_CHANGE(IDC_EDIT_YEAR, OnChangeEditYear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarDateDlg message handlers

BOOL CCalendarDateDlg::OnInitDialog() 
{
	CPmwDialogColor::OnInitDialog();
	
	m_ctlSpinYear.SetRange (1, 9999);
//	m_ctlSpinYear.SetPos (m_info.year);

	m_ctlSpinNumber.SetRange (1, 10);
//	m_ctlSpinNumber.SetPos (n_o_c);

	SetupDialogControls();

	CreateWizzardButtons ();
	EnableWizardButtons (m_WizFlags);
	
	UpdateData(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCalendarDateDlg::OnBack() 
{
   EndDialog (IDC_BACK);
}

void CCalendarDateDlg::OnFinish() 
{
   OnOK ();
}

void CCalendarDateDlg::OnCalwhenNow() 
{
	SYSTEMTIME Now;
	GetLocalTime(&Now);

	m_info.month = (MONTH)Now.wMonth-1;
	m_info.year = Now.wYear;

/* Compute the week. */

	m_info.week = 0;

	int weekday = Now.wDayOfWeek;
	int day = Now.wDay-1;			// 0 based

	while (weekday < day)
	{
		m_info.week++;
		day -= 7;
	}

	m_iYear = m_info.year;
	m_iMonth = m_info.month;
	m_iWeek = m_info.week;

	UpdateData(FALSE);	

	if (m_info.style.type == WEEKLY)
		update_week_selection();
}

void CCalendarDateDlg::SetupDialogControls()
{

	switch (m_info.style.type)
	{
		case YEARLY:
			{
				m_ctlWeekText.ShowWindow(SW_HIDE);
				m_ctlWeek.ShowWindow(SW_HIDE);
				m_ctlMonth.ShowWindow(SW_HIDE);
				m_ctlMonthText.ShowWindow(SW_HIDE);
				m_ctlNumOfX.SetWindowText(GET_PMWAPP()->GetResourceStringPointer(IDS_NumberOfYears));
				break;
			}
		case MONTHLY:
			{
				m_ctlWeekText.ShowWindow(SW_HIDE);
				m_ctlWeek.ShowWindow(SW_HIDE);
				m_ctlNumOfX.SetWindowText(GET_PMWAPP()->GetResourceStringPointer(IDS_NumberOfMonths));
				break;
			}
		case WEEKLY:
			{
				m_ctlNumOfX.SetWindowText(GET_PMWAPP()->GetResourceStringPointer(IDS_NumberOfWeeks));
				update_week_selection();
				break;
			}
	};

	if (!m_bFull)
	{
		m_ctlNumOfX.ShowWindow(SW_HIDE);
		m_ctlEdNumber.ShowWindow(SW_HIDE);
		m_ctlSpinNumber.ShowWindow(SW_HIDE);
	}
	UpdateData(TRUE);
}

void CCalendarDateDlg::update_week_selection()
{
#ifndef LOCALIZE
	UINT m_n_id[] =
	{
   IDS_JANUARY,
   IDS_FEBRUARY,
   IDS_MARCH,
   IDS_APRIL,
   IDS_MAY,
   IDS_JUNE,
   IDS_JULY,
   IDS_AUGUST,
   IDS_SEPTEMBER,
   IDS_OCTOBER,
   IDS_NOVEMBER,
   IDS_DECEMBER
	};
#endif

	SHORT week_save = m_iWeek;
	UpdateData(TRUE);
	m_iWeek = week_save;

	SHORT month = m_iMonth;
	SHORT year = m_iYear;

	/* This is a weekly calendar. Build the weeks for this month. */

	m_ctlWeek.ResetContent();

	SHORT weekday = first_day_of_a_month(year, (MONTH)month);
	SHORT days = days_in_a_month(year, month);
	
	CString strCurMonth;
	CString strNextMonth;
	
	for (SHORT day = 0; day < days; )
	{

#ifdef LOCALIZE
		strCurMonth = CurrentLocale.AbbrevMonthName((MONTH)month);
		strNextMonth = CurrentLocale.AbbrevMonthName((MONTH)month);
#else
		strCurMonth.LoadString (m_n_id[(MONTH)month]);
		strNextMonth.LoadString (m_n_id[(MONTH)month]);
		// truncate for abbreviation
		strCurMonth= strCurMonth.Left(3);
		strNextMonth = strNextMonth.Left(3);
#endif

		day++;
		SHORT last_day = day + (6 - weekday);
		short endday;
		short startday;

		if ((day + 6) > last_day) // if first day of month not on sunday
		{
			WEEKDAY first_day = first_day_of_a_month(year, (MONTH)(month));
			short extra_days = (short)first_day;

			
			short pmonth = DecrementMonth(month);
			short pyear;
			if (pmonth == 11)
				pyear = year - 1;
			else 
				pyear = year;
			
			short prevdays = days_in_a_month(pyear, pmonth);
			startday = (prevdays - extra_days) + 1; 
			endday = day;

#ifdef LOCALIZE
			strCurMonth = CurrentLocale.AbbrevMonthName((MONTH)pmonth);
#else
			strCurMonth.LoadString(m_n_id[(MONTH)(pmonth)]);
			strCurMonth = strCurMonth.Left(3);
#endif
		}
		else if (last_day > days) // fix week at end of month
		{
			endday = last_day - days;

#ifdef LOCALIZE
			strNextMonth = CurrentLocale.AbbrevMonthName((MONTH)IncrementMonth(month));
#else
			strNextMonth.LoadString (m_n_id[IncrementMonth(month)]);
			strNextMonth = strNextMonth.Left(3);
#endif
			startday = day;
		}
		else 
		{
			endday = last_day;
			startday = day;
		}

		
		CHAR buffer[50];

		// "%s %d to %s %d"
		sprintf(buffer, GET_PMWAPP()->GetResourceStringPointer(IDS_DayToDayFormat),
					strCurMonth, startday,
 					strNextMonth, endday);

		m_ctlWeek.AddString(buffer);

		
		weekday = 0;
		day = last_day;
	}

	/* Set the current selection. */

	if (m_iWeek == -1)
	{
		m_iWeek = 0;
	}
	m_ctlWeek.SetCurSel(m_iWeek);
}


void CCalendarDateDlg::OnSelchangeCalwhenMonth() 
{
	UpdateData(TRUE);
	update_week_selection();	
}


void CCalendarDateDlg::get_exit_info(CALENDAR_INFO_REF info)
{

	if (info.month != m_iMonth)
	{
		info.month = (MONTH)m_iMonth;
	}
	
	if (info.year != m_iYear)
	{
		info.year = m_iYear;
	}

	if (m_iWeek == -1)
	{
		m_iWeek = 0;
	}

	if (info.style.type == WEEKLY)
	if (info.week != m_iWeek)
	{
		if (first_day_of_a_month(m_iYear, (MONTH)m_iMonth) == SUNDAY || m_iWeek > 0)
			info.week = m_iWeek;
		else
		{
			info.month = DecrementMonth(m_iMonth);
			if (info.month == 11)
				info.year--;
			info.week = GetWeeksInMonth(info.year, info.month);
			//info.week = m_iWeek + 1;
		}
	}

	
}


void CCalendarDateDlg::OnChangeEditYear() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPmwDialogColor::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	if (m_ctlYearText.m_hWnd != NULL)
	{
		update_week_selection();
		//UpdateData(TRUE);
	}
	
}

