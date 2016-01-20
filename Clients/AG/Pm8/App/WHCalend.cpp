// $Workfile: WHCalend.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
//
// Revision History:
//
// $Log: /PM8/App/WHCalend.cpp $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 3     12/23/98 1:51p Psasse
// Code clean up
// 
// 2     9/22/98 2:51p Hforman
// changed file names so all With Help files are consistent
// 
// 1     9/22/98 1:44p Hforman
// changed name from cwcalend.cpp
// 
// 9     7/25/98 4:32p Rlovejoy
// Check for success of UpdateData().
// 
// 8     6/04/98 2:11p Rlovejoy
// Make sure all title strings are maintained.
// 
// 7     6/02/98 2:31p Rlovejoy
// Code to keep 'picture to the side' bitmaps consistent.
// 
// 6     6/01/98 11:28p Hforman
// change CWHCalendarPicturePage baseclass to CWHelpPropertyPage
// 
// 5     6/01/98 8:25p Hforman
// new UpdatePreview() functionality
// 
// 4     5/31/98 3:28p Rlovejoy
// Display the correct strings in view.
// 
// 3     5/29/98 7:05p Psasse
// get rid of m_nPageOffset once and for all
// 
// 2     5/28/98 2:10p Rlovejoy
// Alpha calendars!
// 
// 1     5/26/98 3:05p Rlovejoy
// Initial creation.

#include "stdafx.h"
//#include <assert.h>
#include "resource.h"
#include "pmwtempl.h"
#include "pmwdoc.h"
#include "caldoc.h"
#include "calprogd.h"
#include "pagerec.h"
#include "utils.h"

#include "WHlpCmn.h"
#include "WHlpView.h"
#include "WHCalend.h"

CHAR *suffix_for_day(SHORT day);

/////////////////////////////////////////////////////////////////////////////
// CWCalendarPageContainer

CWCalendarPageContainer::CWCalendarPageContainer(CWHelpWizPropertySheet* pSheet) :
	CWHelpPageContainer(pSheet)
{
	// Build property pages for each step in the wizard process
	// NOTE: need to allocate separate CWHelpPropertyPage's because
	// property sheet won't allow you to add a pointer to the same property
	// page twice.
	m_aPages.Add(new CWHelpCommonPropertyPage(this, IDe_CALENDAR_CALENDAR_TYPE));
	m_aPages.Add(new CWHCalendarWhenPage(this, IDe_CALENDAR_ENTER_TIME));
	m_aPages.Add(new CWHCalendarPicturePage(this, IDe_CALENDAR_PICTURE_LOC));
	m_aPages.Add(new CWHelpCommonPropertyPage(this, IDe_CALENDAR_CATEGORY));
	m_aPages.Add(new CWHelpCommonPropertyPage(this, IDe_CALENDAR_STYLE));
	m_aPages.Add(new CWCalendarView(this, IDe_CALENDAR_HERE_IT_IS));

	m_PictureStyle = CAL_PICTURE_TOP;
}

CWCalendarPageContainer::~CWCalendarPageContainer()
{

}

void CWCalendarPageContainer::RetrieveDlgInfo(UINT nIDContainerItem, void* stDlgInfo)
{
	//UINT nIndex = nIDContainerItem - m_nPageOffset;

	//m_pSheet->GetDlgStrings((CStringArray*)stDlgInfo);
}

void CWCalendarPageContainer::StoreUserInfo(UINT nIDContainerItem, int nChoice)
{
	static CALENDAR_TYPE TypeTable[] = {
		MONTHLY, WEEKLY, YEARLY
	};
	
	switch (nIDContainerItem) {
	case IDe_CALENDAR_CALENDAR_TYPE:
		SetTimeType(TypeTable[nChoice]);
		break;
	case IDe_CALENDAR_PICTURE_LOC:
		SetPictureStyle((CAL_PICTURE_STYLE)nChoice);
		break;
	default:
		break;
	}
}

void CWCalendarPageContainer::ModifyPreviewName(UINT nIDContainerItem, CString& csName)
{
	// If we're after the picture location page, make sure we see the picture
	// to the side if picture style is set to CAL_PICTURE_LEFT
	if (nIDContainerItem > IDe_CALENDAR_PICTURE_LOC)
	{
		if (GetPictureStyle() == CAL_PICTURE_LEFT)
		{
			csName.SetAt(9, 'L');
		}
	}
}

extern SHORT days_in_a_month(SHORT year, SHORT month);
extern WEEKDAY first_day_of_a_month(SHORT year, MONTH month);

/////////////////////////////////////////////////////////////////////////////
// CCalendarWhenPage dialog

CWHCalendarWhenPage::CWHCalendarWhenPage(CWHelpPageContainer* pContainer, UINT nIDContainerItem)
	: CWHelpPropertyPage(pContainer, nIDContainerItem, CWHCalendarWhenPage::IDD)
{
   Afx_Data_Init();

	ComputeNow();
	vars_from_info();
}

void CWHCalendarWhenPage::Afx_Data_Init (void)
{
	//{{AFX_DATA_INIT(CCalendarWhenPage)
	m_week = -1;
	m_numMonths = 1;
	//}}AFX_DATA_INIT
}

void CWHCalendarWhenPage::vars_from_info()
{
	m_month = m_ChangingCalendarInfo.month;
	m_year = m_ChangingCalendarInfo.year;
	m_week = m_ChangingCalendarInfo.week;
}

void CWHCalendarWhenPage::DoDataExchange(CDataExchange* pDX)
{
	CWHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCalendarWhenPage)
	DDX_Control(pDX, IDC_SPIN_EDNUMBER, m_ctlSpinNum);
	DDX_CBIndex(pDX, IDC_CALWHEN_MONTH, m_month);
	DDX_Text(pDX, IDC_CALWHEN_YEAR, m_year);
	DDV_MinMaxInt(pDX, m_year, 1, 9999);
	DDX_CBIndex(pDX, IDC_CALWHEN_WEEK, m_week);
	DDX_Text(pDX, IDC_CALWHEN_EDNUMBER, m_numMonths);
	DDV_MinMaxInt(pDX, m_numMonths, 1, 24);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWHCalendarWhenPage, CWHelpPropertyPage)
	//{{AFX_MSG_MAP(CCalendarWhenPage)
	ON_BN_CLICKED(IDC_CALWHEN_NOW, OnCalwhenNow)
	ON_CBN_SELCHANGE(IDC_CALWHEN_MONTH, OnMonthChange)
	ON_EN_KILLFOCUS(IDC_CALWHEN_YEAR, OnMonthChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarWhenPage message handlers

BOOL CWHCalendarWhenPage::OnInitDialog()
{
	CWHelpPropertyPage::OnInitDialog();

/* Come to grips with your limitations... */
	CEdit *edit;

	if ((edit = (CEdit *)GetDlgItem(IDC_CALWHEN_YEAR)) != NULL)
	{
		edit->LimitText(4);
	}

	// set range on selection of number of months
	m_ctlSpinNum.SetRange (1, 24);
	m_ctlSpinNum.SetPos(1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CWHCalendarWhenPage::OnSetActive()
{
	// Retrieve the time-span type for the calendar
	m_type = ((CWCalendarPageContainer*)m_pContainer)->GetTimeType();
	m_ChangingCalendarInfo.style.type = (short)m_type;

	// Enable controls based on type
	enable_child(IDC_CALWHEN_WEEK, (m_type == WEEKLY));
	enable_child(IDC_CALWHEN_TWEEK, (m_type == WEEKLY));
	enable_child(IDC_CALWHEN_MONTH, (m_type == WEEKLY || m_type == MONTHLY));
	enable_child(IDC_CALWHEN_TMONTH, (m_type == WEEKLY || m_type == MONTHLY));

	// Change text in IDC_CALWHEN_TNUMBER
	CString cs;
	switch (m_type)
	{
	case WEEKLY:
		cs.LoadString(IDS_NumberOfWeeks);
		break;
	case MONTHLY:
		cs.LoadString(IDS_NumberOfMonths);
		break;
	case YEARLY:
		cs.LoadString(IDS_NumberOfYears);
		break;
	}
	SetDlgItemText(IDC_CALWHEN_TNUMBER, cs);

	// Set up week string
	update_week_selection();

	// If this is not a weekly calendar, reset weeks
	if (m_type != WEEKLY)
	{
		CComboBox *list;
		if ((list = (CComboBox *)GetDlgItem(IDC_CALWHEN_WEEK)) != NULL)
		{
			list->ResetContent();
		}
	}

	// Set the instructional text based on calendar type
	CALENDAR_TYPE TimeType = ((CWCalendarPageContainer*)m_pContainer)->GetTimeType();
	if (TimeType == MONTHLY)
	{
		m_strTopText.LoadString(IDS_MonthlyDateSelection);
	}
	else if (TimeType == WEEKLY)
	{
		m_strTopText.LoadString(IDS_WeeklyDateSelection);
	}
	if (TimeType == YEARLY)
	{
		m_strTopText.LoadString(IDS_YearlyDateSelection);
	}
	m_strLowerText = _T("");
	SetDlgItemText(IDC_WH_TOP_TEXT, m_strTopText);
	SetDlgItemText(IDC_WH_LOWER_TEXT, m_strLowerText);
	
	// Bypass
	UpdatePreview(FALSE);
	return CPropertyPage::OnSetActive();
}

LRESULT CWHCalendarWhenPage::OnWizardNext() 
{
	// Tally up changes
	if (!UpdateData())
		return -1;

	m_ChangingCalendarInfo.month = (MONTH)m_month;
	m_ChangingCalendarInfo.year = m_year;

	if (m_week == -1)
	{
		m_week = 0;
	}
	m_ChangingCalendarInfo.week = m_week;

	// Store calendar info
	((CWCalendarPageContainer*)m_pContainer)->SetCalendarInfo(m_ChangingCalendarInfo, m_numMonths);

	return CWHelpPropertyPage::OnWizardNext();
}

LRESULT CWHCalendarWhenPage::OnWizardBack() 
{	
	return CWHelpPropertyPage::OnWizardBack();
}

/*
// Set the dialog entries to "now".
*/

void CWHCalendarWhenPage::OnCalwhenNow()
{
	ComputeNow();

	UpdateData(TRUE);
	vars_from_info();
	UpdateData(FALSE);
}

void CWHCalendarWhenPage::ComputeNow()
{
/* Compute "now". */

	SYSTEMTIME Now;
	GetLocalTime(&Now);

	m_ChangingCalendarInfo.month = (MONTH)Now.wMonth-1;
	m_ChangingCalendarInfo.year = Now.wYear;

/* Compute the week. */

	m_ChangingCalendarInfo.week = 0;

	int weekday = Now.wDayOfWeek;
	int day = Now.wDay-1;			// 0 based

	while (weekday < day)
	{
		m_ChangingCalendarInfo.week++;
		day -= 7;
	}

	m_ChangingCalendarInfo.first_day_of_week = SUNDAY;
	m_ChangingCalendarInfo.last_day_of_week = SATURDAY;

}

VOID CWHCalendarWhenPage::enable_child(UINT id, BOOL enable)
{
	CWnd *child;

	if ((child = GetDlgItem(id)) != NULL)
	{
		child->ShowWindow(enable ? SW_SHOW : SW_HIDE);
		child->EnableWindow(enable);
	}
}

void CWHCalendarWhenPage::OnMonthChange()
{
/*
// The month or year has changed.
// Build the "week" strings.
*/
	update_week_selection();
}

VOID CWHCalendarWhenPage::update_week_selection()
{
	CComboBox *list;
	if ((list = (CComboBox *)GetDlgItem(IDC_CALWHEN_WEEK)) != NULL)
	{
		if (m_type == WEEKLY)
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
	}
}

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


/////////////////////////////////////////////////////////////////////////////
// CWHCalendarPicturePage dialog

CWHCalendarPicturePage::CWHCalendarPicturePage(CWHelpPageContainer* pContainer, UINT nIDContainerItem)
	: CWHelpPropertyPage(pContainer, nIDContainerItem, CWHCalendarPicturePage::IDD)
{
	m_iAbove = 0;
	m_iPicture = 0;
}

int CWHCalendarPicturePage::FindBranchChoice()
{
	int nChoice = CAL_PICTURE_TOP;	// default to picture 'above' path
	if (((CButton*)GetDlgItem(IDC_PICTURE_YES))->GetCheck())
	{
		if (((CButton*)GetDlgItem(IDC_PICTURE_SAME))->GetCheck())
		{
			nChoice = CAL_PICTURE_SAME;
		}
	}

	return nChoice;
}

CAL_PICTURE_STYLE CWHCalendarPicturePage::FindStyleChoice()
{
	CAL_PICTURE_STYLE PicStyle = CAL_PICTURE_NONE;
	if (((CButton*)GetDlgItem(IDC_PICTURE_YES))->GetCheck())
	{
		if (((CButton*)GetDlgItem(IDC_PICTURE_SIDE))->GetCheck())
		{
			PicStyle = CAL_PICTURE_LEFT;
		}
		else if (((CButton*)GetDlgItem(IDC_PICTURE_ABOVE))->GetCheck())
		{
			PicStyle = CAL_PICTURE_TOP;
		}
		else if (((CButton*)GetDlgItem(IDC_PICTURE_SAME))->GetCheck())
		{
			PicStyle = CAL_PICTURE_SAME;
		}
	}

	return PicStyle;
}

void CWHCalendarPicturePage::DoDataExchange(CDataExchange* pDX)
{
	CWHelpPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWHCalendarPicturePage)
	DDX_Radio(pDX, IDC_PICTURE_ABOVE, m_iAbove);
	DDX_Radio(pDX, IDC_PICTURE_YES, m_iPicture);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWHCalendarPicturePage, CWHelpPropertyPage)
	//{{AFX_MSG_MAP(CWHCalendarPicturePage)
	ON_BN_CLICKED(IDC_PICTURE_YES, OnPictureYes)
	ON_BN_CLICKED(IDC_PICTURE_NO, OnPictureNo)
	ON_BN_CLICKED(IDC_PICTURE_ABOVE, OnPictureAbove)
	ON_BN_CLICKED(IDC_PICTURE_SIDE, OnPictureSide)
	ON_BN_CLICKED(IDC_PICTURE_SAME, OnPictureSame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWHCalendarPicturePage message handlers

BOOL CWHCalendarPicturePage::OnInitDialog() 
{
	CWHelpPropertyPage::OnInitDialog();
	
	UpdateCBitmap();	
	FixControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWHCalendarPicturePage::OnSetActive() 
{
	// Big-time override
	// Follow the branch
	GetSheet()->FollowBranch(this);
	UpdateCBitmap();

	return CWHelpPropertyPage::OnSetActive();
}

void CWHCalendarPicturePage::OnPictureAbove() 
{
	UpdateCBitmap();
	
}

void CWHCalendarPicturePage::OnPictureNo() 
{
	UpdateCBitmap();
	FixControls();
}

void CWHCalendarPicturePage::OnPictureSide() 
{
	UpdateCBitmap();
	
}

void CWHCalendarPicturePage::OnPictureYes() 
{
	UpdateCBitmap();
	FixControls();
}


// UpdateCBitmap()
// creates resource name based on type of calendar, then loads it

void CWHCalendarPicturePage::UpdateCBitmap()
{
	// Each calendar style is mapped to a choice
	SetCurSelection((int)FindStyleChoice());
	UpdatePreview();
}	


VOID CWHCalendarPicturePage::enable_child(UINT id, BOOL enable)
{
	CWnd *child;

	if ((child = GetDlgItem(id)) != NULL)
	{
		child->EnableWindow(enable);
	}
}

void CWHCalendarPicturePage::FixControls()
{
	UpdateData(TRUE);
	BOOL bEnable = m_iPicture == 0;
	enable_child(IDC_PICTURE_SIDE, bEnable);
	enable_child(IDC_PICTURE_ABOVE, bEnable);
	enable_child(IDC_PICTURE_SAME, bEnable);
}

void CWHCalendarPicturePage::OnPictureSame() 
{
	UpdateCBitmap();	
}

LRESULT CWHCalendarPicturePage::OnWizardNext() 
{
	// Find the style
	CAL_PICTURE_STYLE PicStyle = FindStyleChoice();
	m_pContainer->StoreUserInfo(GetSheet()->GetActiveIndex(), PicStyle);

	// Find branch choice
	int nChoice = FindBranchChoice();

	// Branch ahead in the table (only above and same exist)
	GetSheet()->FindBranchAhead(nChoice);

	return CWHelpPropertyPage::OnWizardNext();
}

LRESULT CWHCalendarPicturePage::OnWizardBack() 
{
	// Branch back in the table
	GetSheet()->FindBranchBack();

	return CWHelpPropertyPage::OnWizardBack();
}

/////////////////////////////////////////////////////////////////////////////
// CWCalendarView dialog

extern void IncrementCalendarDate(CALENDAR_INFO * ciCalInfo, BOOL bWhere);

CWCalendarView::CWCalendarView(CWHelpPageContainer* pContainer, UINT nIDContainerItem) :
	CWView(pContainer, nIDContainerItem)
{

}

BEGIN_MESSAGE_MAP(CWCalendarView, CWView)
	//{{AFX_MSG_MAP(CWCalendarView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWCalendarView message handlers

BOOL CWCalendarView::OnSetActive() 
{
	// Open the document
	if (!CWView::OnSetActive())
		return FALSE;

	// Show final text
	m_strTopText.LoadString(IDS_WH_CALENDAR_FINAL);
	UpdateData(FALSE);

	// Use calendar style to edit the document
	CCalendarDoc* pDoc = (CCalendarDoc*)m_pDoc;
	CAL_PICTURE_STYLE PicStyle = ((CWCalendarPageContainer*)m_pContainer)->GetPictureStyle();
	pDoc->GotoPage(1);

	// Retrieve the bound for the current picture page & whole calendar
	PBOX PicBound = pDoc->GetPicturePageBound(pDoc->CurrentPageIndex());

	if (PicStyle == CAL_PICTURE_NONE)
	{
		// Remove picture page, front & back cover pages
		// ASSUME there is only one picture page to begin with
		pDoc->DocumentRecord()->delete_page(m_pDoc->GetPage(0));
		pDoc->GotoPage(0);
		pDoc->DocumentRecord()->delete_page(m_pDoc->GetPage(1));
		pDoc->InvalidateLastKnownPage();
	}
	DWORD dwAtPage = pDoc->CurrentPageIndex(); // current working page

	// Change the picture style
	CalendarObject* pCalendarObject = pDoc->find_calendar_object(dwAtPage);
	pDoc->SetPictureStyle(PicStyle);
	pCalendarObject->SetPictureStyle(PicStyle);

	// Retrieve a list of objects in the current page
   ObjectList* pObjectList = pDoc->object_list();
	PMGPageObject* pObject = (PMGPageObject*)pObjectList->first_object();

	// If we're removing pictures, loop through objects and remove anything
	// that's in the picture bound
	PPNT bottom_dims, top_dims;
	pDoc->GetPageDims(bottom_dims, top_dims);
	if (PicStyle == CAL_PICTURE_NONE)
	{
		long nMove = (bottom_dims.y - top_dims.y);
		while (pObject != NULL)
		{
			PBOX bound = ((RectPageObject*)pObject)->get_unrotated_bound();
			PBOX intersect;
			
			// If the bounds intersect, remove the object
			// otherwise, shift it up for rescale
			if (IntersectBox(&intersect, &bound, &PicBound))
			{
				PMGPageObject* pNextObject = (PMGPageObject*)pObject->next_object();
				pObjectList->detach(pObject);
				pObject->destroy();
				delete pObject;

				pObject = pNextObject;
			}
			else
			{
				// Move the object up
				bound.y0 -= nMove;
				bound.y1 -= nMove;
				((RectPageObject*)pObject)->set_unrotated_bound(bound);
				pObject->calc();
				pObject = (PMGPageObject*)pObject->next_object();
			}
		}
	}
	pDoc->RecalcPageSize();

	// May need to shift pictures over
	if (PicStyle == CAL_PICTURE_LEFT)
	{
		PPNT new_bottom_dims, new_top_dims;
		pDoc->GetPageDims(new_bottom_dims, new_top_dims);
		PPNT Move;
		Move.x = (new_bottom_dims.x - bottom_dims.x);
		Move.y = (new_bottom_dims.y - bottom_dims.y);

		// Apply changes to front, calendar, and back pages
		for (int i = 0; i < (int)pDoc->NumberOfPages(); i++)
		{
			pDoc->GotoPage(i);

			// Retrieve a list of objects in the current page
			ObjectList* pObjectList = pDoc->object_list();
			PMGPageObject* pObject = (PMGPageObject*)pObjectList->first_object();
			while (pObject != NULL)
			{
				PBOX bound = ((RectPageObject*)pObject)->get_unrotated_bound();
				PBOX intersect;
				
				// If the bounds don't intersect, we need to move the object
				if (!IntersectBox(&intersect, &bound, &PicBound))
				{
					bound.x0 += Move.x;
					bound.x1 += Move.x;
					bound.y0 += Move.y;
					bound.y1 += Move.y;
					((RectPageObject*)pObject)->set_unrotated_bound(bound);
					pObject->calc();
				}
				pObject = (PMGPageObject*)pObject->next_object();
			}
		}
		pDoc->GotoPage(dwAtPage);
	}

	// Resize the preview
	m_preview.SizePreview();

	// Retrieve selected calendar attributes
	CALENDAR_INFO info;
	short numcals;
	((CWCalendarPageContainer*)m_pContainer)->GetCalendarInfo(info, numcals);

	// Merge in calendar style structure
	CALENDAR_INFO old_info = pCalendarObject->get_calendar().get_info();
	info.style = old_info.style;

	// Change the date
	pDoc->ChangeCalendarDate(info);

	// Create the remaining pages
	DWORD dwPages = numcals-1;  // number of new pages to add
	DWORD dwTotalPages = pDoc->NumberOfPages(); // total pages in project
	PMGDatabase* pDatabase = pDoc->get_database(); // current database
	PageRecord* pSourcePage = NULL;  // source page for info/possible template 
	PageRecord* pNewPage = NULL; // new page 
	BOOL bCopy = TRUE;
	BOOL bPictures = (PicStyle == CAL_PICTURE_TOP || PicStyle == CAL_PICTURE_LEFT);
	DB_RECORD_NUMBER dbPage;
		
	// decide where to insert new pages, and pull the proper page
	// get last page
	if (bPictures)  // use 2nd to last page
	{
		dwTotalPages--;
		dwAtPage  = pDoc->NumberOfPages() -1; // total pages in project;
	}
	else
		dwAtPage  = pDoc->NumberOfPages(); // total pages in project;
	dbPage = dwTotalPages - 1;

	// get calendar from template source page and get calendar info
	CALENDAR_INFO ciCalInfo;
	pObject = NULL;
	
	// get source page
	pSourcePage = (PageRecord*)pDatabase->get_record(pDoc->GetPage(dbPage), NULL, RECORD_TYPE_Page);

	if (pSourcePage != NULL) // get calendar object from page
	{
		int index = 0;
		pObject = (PMGPageObject*)pSourcePage->objects()->first_object();
		
		// loop thru and find a calendar to get properties from
		do 
		{
			if (pObject->type() == OBJECT_TYPE_Calendar)
			{
				// copy calendar info
				Calendar cal = ((CalendarObject*)pObject)->get_calendar();
				ciCalInfo = cal.get_info();
				
				// get start date from date source page
				CalendarObject* pSrc = pDoc->find_calendar_object(dbPage);
				if (pSrc != NULL)
				{
					Calendar t = pSrc->get_calendar();
					ciCalInfo.year = t.get_info().year;
					ciCalInfo.month = t.get_info().month;
					ciCalInfo.week = t.get_info().week;
				}
				break;
			}
			else 
				pObject = (PMGPageObject*)pObject->next_object();
		} 
		while (pObject != NULL);
	}
	
	DWORD dwStartPage = dwAtPage;
	int numsteps;
	
	if (ciCalInfo.style.type == YEARLY) 
		numsteps = ((int)dwPages * 12) + 1;
	else 
		numsteps = (int)dwPages +1;

	CString strDlgTitle = "Adding page(s)...";
	CCalendarProgressDlg dlg (numsteps, strDlgTitle);
	dlg.Create(IDD_CALENDAR_PROGRESS);
	dlg.ShowWindow(SW_SHOW);
	dlg.Step();
	BeginWaitCursor();
	
	// create new pages
	while (dwPages-- != 0)
	{

		// insert new page
		if (!pDoc->AddPage(dwAtPage))
		{
			break;
		}

		// Lock the new page.
		pNewPage = (PageRecord*)pDatabase->get_record(pDoc->GetPage(dwAtPage), NULL, RECORD_TYPE_Page);

		if (pNewPage != NULL)
		{
			// increment date
			if (ciCalInfo.style.type != YEARLY)
				IncrementCalendarDate(&ciCalInfo, TRUE);
			else 
			{
				ciCalInfo.year++;
			}

			if (pSourcePage != NULL) // creating a default calendar object for the page
			{
				// put a calendar on it
				CalendarObject * pNewCal = NULL;
				ObjectList ol (NULL);					
	
				if (!bCopy) // not copying, creating new calendars
				{					
					// build calendar create struct
					CALENDAR_CREATE_STRUCT ccs;
					ccs.info = &ciCalInfo;
					ccs.picture_style = pDoc->GetPictureStyle();
					//PPNT dim = pDoc->get_dimensions();
					pDoc->CalcCalendarSize(&ccs.bound);
					ccs.full_build = TRUE;
					
					if (ciCalInfo.style.type == YEARLY) 
					{
						// create and add yearly calendar to new page
						pDoc->CreateYearlyCalendar (&ccs, &ol, &dlg, ccs.picture_style == CAL_PICTURE_SAME);
						pDatabase->CopyObjectList(pNewPage->objects(), &ol, pNewPage->Id());	
					}
					else 
					{
						// create and add new weekly/monthly calendar
						pNewCal = pDatabase->create_calendar_object(&ccs);
						pNewPage->objects()->append(pNewCal);
						dlg.Step();
					}
				}
				else // copy from source page
				{
					// copy all objects from source page
					PMGPageObject* pObj = (PMGPageObject*)pSourcePage->objects()->first_object();
					do 
					{
						// copy object and append it to new list
						PMGPageObject * pDestObj = (PMGPageObject*)pObj->duplicate();
						ol.append(pDestObj);
						pObj = (PMGPageObject*)pObj->next_object();
					} 
					while (pObj != NULL);

					// and add them to the new page
					pDatabase->CopyObjectList(pNewPage->objects(), &ol, pNewPage->Id());	

					// replace calendar info and rebuild calendars
					// update calendar information to reflect new date
					pObj = (PMGPageObject*)pNewPage->objects()->first_object();
					
					// find calendar
					do 
					{
						if(pObj->type() == OBJECT_TYPE_Calendar)
						{
							if (ciCalInfo.style.type == YEARLY)
							{
								// save old calendar's month info, but update year info
								Calendar c = ((CalendarObject*)pObj)->get_calendar();
								CALENDAR_INFO ci = c.get_info();
								ciCalInfo.month = ci.month;
								ciCalInfo.week = ci.week;
							}

							pDoc->ReplaceCalendarInfo(ciCalInfo, (CalendarObject*)pObj);
							dlg.Step();								
							if (ciCalInfo.style.type != YEARLY)
								break; // should only be 1 calendar here.
						}

						pObj = (PMGPageObject*)pObj->next_object();
					} 
					while (pObj != NULL);	
				}
			}	
		}
		pNewPage->release();
		
		// increment current page, only if adding at end
		dwAtPage++;

 	} // while

	dlg.DestroyWindow();
	EndWaitCursor();
	
	// If we had a source page, release it now.
	if (pSourcePage != NULL)
	{
		pSourcePage->release();
	}
	
	return TRUE;
}
