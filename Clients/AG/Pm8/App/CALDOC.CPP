// caldoc.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"

#include <time.h>

#include "pmwdoc.h"
#include "pmwview.h"
#include "caldoc.h"
#include "action.h"

#include "grafobj.h"
#include "frameobj.h"
#include "dateobj.h"
#include "calobj.h"
#include "ccommand.h"
#include "util.h"
#include "ctxp.h"
#include "pagerec.h"
#include "calrec.h"
#include "paper.h"
#include "calprogd.h"

extern SHORT days_in_a_month(SHORT year, SHORT month);
extern WEEKDAY first_day_of_a_month(SHORT year, MONTH month);
extern void ReplaceWord(CFrameObject* pObject, char *pOldWord, char *pNewWord, BOOL fOverwrite);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// helper function returns the number of weeks in a month
//
PUBLIC SHORT GetWeeksInMonth(short year, short month)
{
	short days = days_in_a_month(year, month);
	// count number of sundays in the month to determine the number of weeks in that month
	WEEKDAY test = first_day_of_a_month(year, (MONTH)month);
	short today = 1; 
	short us = DAYS_IN_FULL_WEEK - test; // how many days until the next sunday
	
	if (us < 7) // if first day of week isnt sunday, add days until
		today += us;
	
	short numwks = 0;

	for (int i=today; i <= days; i+=7)
	{
		numwks++;
	}

	return numwks;
}


// helper function which calculates the increment (or decrement) for calendar year/month/week
// made it global so that viewcmd.cpp could use it easier
// bWhere = TRUE = insert at end of series, FALSE = insert at begining of series
PUBLIC VOID IncrementCalendarDate(CALENDAR_INFO * ciCalInfo, BOOL bWhere) 
{
	short type = ciCalInfo->style.type;

	
	BOOL bUpdateMonth = FALSE;
	
	if (type == WEEKLY)
	{
		short weeks = 0;
		if (bWhere == TRUE) // at end
		{
			weeks = GetWeeksInMonth(ciCalInfo->year, ciCalInfo->month);
			
			// check to see if first week of month is 0 or 1
			if (first_day_of_a_month(ciCalInfo->year, (MONTH)(ciCalInfo->month)) == SUNDAY)
				weeks--;
			
			if (ciCalInfo->week < weeks)
			{
				ciCalInfo->week++;
			}
			else
			{	
				if (first_day_of_a_month(ciCalInfo->year, (MONTH)(ciCalInfo->month + 1)) == SUNDAY)
					ciCalInfo->week = 0;
				else
					ciCalInfo->week = 1;

				bUpdateMonth = TRUE;
			}
		}
		else  // add at beginning
		{
			short firstweek;
			if (first_day_of_a_month(ciCalInfo->year, (MONTH)ciCalInfo->month) == SUNDAY)
				firstweek = 0;
			else
				firstweek = 1;

			if (ciCalInfo->week == firstweek)
			{
				weeks = GetWeeksInMonth(ciCalInfo->year, ciCalInfo->month - 1);
				// check to see if first week of month is 0 or 1
				if (first_day_of_a_month(ciCalInfo->year, (MONTH)(ciCalInfo->month - 1)) == SUNDAY)
					weeks--;
				ciCalInfo->week = weeks;
				bUpdateMonth = TRUE;
			}
			else
				ciCalInfo->week--;
		}
	}
	
	
	//BOOL bUpdateYear = FALSE;

	if( (type == MONTHLY || type == YEARLY) || bUpdateMonth)
	{
		// calculate date offset
		if (bWhere == TRUE)  // insert at end
		{
			if (ciCalInfo->month < 11) // if its not december
				ciCalInfo->month++; // increment month
			else
			{
				ciCalInfo->month = 0; // reset month
//				bUpdateYear = TRUE;
				if (type != YEARLY)
					ciCalInfo->year++; // increment year
			}
		}
		else // insert at beginning
		{
			if (ciCalInfo->month > 0) // if its january
				ciCalInfo->month--; // decrement month	
			else
			{
				ciCalInfo->month = 11; // reset month
//				bUpdateYear = TRUE;
				if (type != YEARLY)
					ciCalInfo->year--;  // decrement year
			}
		}
	}

	/*if (type == YEARLY || bUpdateYear) 
	{
		if (bWhere)
			ciCalInfo->year++;
		else
			ciCalInfo->year--;
	}*/
}
/////////////////////////////////////////////////////////////////////////////
// CCalendarDoc

IMPLEMENT_SERIAL(CCalendarDoc, CPmwDoc, 0 /* schema number*/ )

/*
// The calendar project panel.
*/

PRIVATE PANEL_INFO
calendar_panels[] =
{
	{
		{    0,    0, 1000, 1000 },
		{    0,    0,    0,    0 },
		0,
		1
	}
};

/*
// The calendar project info.
*/

ProjectInfo CCalendarDoc::StandardInfo =
{
	PROJECT_TYPE_Calendar,
	0,
	PORTRAIT,
	sizeof(calendar_panels)/sizeof(calendar_panels[0]),
	IDS_CALENDAR_FRAME_LAYOUT,
	IDS_CALENDAR_GRAPHIC_LAYOUT,
	IDS_CALENDAR_DUPLICATE_LAYOUT,
	calendar_panels
};

CCalendarDoc::CCalendarDoc()
{
	info = StandardInfo;
	m_pPendingCommand = NULL;
	m_number_of_calendars = -1;
	m_picture_style = -1;
}

BOOL CCalendarDoc::OnNewDocument()
{
/*
// Prompt the user for the type and when of this calendar.
*/
	CPmwApp* pApp = GET_PMWAPP();

	CALENDAR_CREATE_STRUCT ccs;

	CALENDAR_INFO info;

	ASSERT(pApp->last_new_info.m_pCalendarInfo != NULL);
#if 0
	if (pApp->last_new_info.m_pCalendarInfo == NULL)
	{
	/* Compute "now". */

		SYSTEMTIME Now;
		GetLocalTime(&Now);

		info.set_default_style(MONTHLY);
		info.month = (MONTH)Now.wMonth-1;
		info.year = Now.wYear;

		ccs.picture_style = m_picture_style = CAL_PICTURE_NONE;
		m_number_of_calendars = 1;

	/* Compute the week. */

		info.week = 0;

		int weekday = Now.wDayOfWeek;
		int day = Now.wDay-1;			// 0 based

		while (weekday < day)
		{
			info.week++;
			day -= 7;
		}

		info.first_day_of_week = SUNDAY;
		info.last_day_of_week = SATURDAY;

		CCalendarWhenDlg cdlg(info, TRUE);

		if (cdlg.DoModal() != IDOK)
		{
			return FALSE;
		}	

	/*
	// Get the info the user agreed to.
	*/
		cdlg.get_exit_info(info);
	}
	else
#endif
	{
		info = *(pApp->last_new_info.m_pCalendarInfo);
		m_picture_style = pApp->last_new_info.m_PictureStyle;
		m_number_of_calendars = pApp->last_new_info.m_NumberOfCalendars;
	}

/*
// Do the new document thing.
*/

	if (!CPmwDoc::OnNewDocument())
		return FALSE;

/*
// Create the default calendar object.
*/

	// get orientation information
	short orientation = pApp->last_new_info.orientation;
	
	ccs.picture_style = m_picture_style;// = pApp->last_new_info.m_PictureStyle;
	
	CFrameObject * pFrame = NULL;
	char text [] = {"Front \nCover \nPage"};
	
	CTextStyle Style(get_database(), TRUE);
	Style.SetDefault();
	Style.Size(MakeFixed(48));
	Style.BaseSize(MakeFixed(48));
	Style.UpdateFontMetrics();


	DWORD dwAtPage = CurrentPageIndex();
	AdjustPageSize();
	
	// add in cover page
	if (m_picture_style == CAL_PICTURE_TOP || m_picture_style == CAL_PICTURE_LEFT)
	{
		pFrame = CreateFrame (&(get_text_bound(dwAtPage)), text, ALIGN_left, ALIGN_top,
			get_current_panel(), &Style);

		if (pFrame != NULL)
		{
			append_object(pFrame);
			refresh_object(pFrame);
			pFrame->Page(GetPage(dwAtPage));
		}
		AddPage(++dwAtPage); // increment and add page
	}


	ccs.info = &info;
	CalcCalendarSize(&ccs.bound);
		
	ccs.full_build = TRUE;
	
	short numcals = m_number_of_calendars = pApp->last_new_info.m_NumberOfCalendars;

	CreateCalendarInfoRecord();

	BeginWaitCursor();
	DWORD startpage = dwAtPage;
	int numsteps = numcals;
	CString strDlgTitle;
	switch (info.style.type)
	{
		case YEARLY:
			{
				numsteps = (numcals * 12);
				strDlgTitle = "Creating yearly calendar...";

				// default style settings for yearly calendar
				info.set_default_style(YEARLY);
				break;
			}
		case MONTHLY:
			{
				strDlgTitle = "Creating monthly calendar...";
				break;
			}
		case WEEKLY:
			{
				strDlgTitle = "Creating weekly calendar...";
				break;
			}
	};


	BOOL fSuccess = FALSE;
	CCalendarProgressDlg dlg (++numsteps, strDlgTitle);

	dlg.Create(IDD_CALENDAR_PROGRESS);

	dlg.ShowWindow(SW_SHOW);
	dlg.Step();
	do {

		ObjectList ol (NULL);
		CalendarObject* pCalendarObject;
		// create yearly calendar set
		if (info.style.type == YEARLY)
		{
			CreateYearlyCalendar (&ccs, &ol, &dlg, m_picture_style == CAL_PICTURE_SAME);
			fSuccess = TRUE;
		}
		// create montly/weekly calendar object
		else if ((pCalendarObject = get_database()->create_calendar_object(&ccs)) != NULL)
		{
			fSuccess = TRUE;
			pCalendarObject->set_panel(MAIN_CALENDAR_PANEL);
			pCalendarObject->add_flags(OBJECT_FLAG_locked);
//			pCalendarObject->SetNumberOfCalendars(m_number_of_calendars);
			pCalendarObject->SetPictureStyle(m_picture_style);
			pCalendarObject->set_bound(ccs.bound);
			//append_object(pCalendarObject);
			ol.append(pCalendarObject);
			dlg.Step();
		}

		// if calendar has a picure, create a text frame to 
		// remind the user to add a picture.
		if (m_picture_style != CAL_PICTURE_NONE)
		{
			CString calstr;
			// create text frame for pictue page
			if (m_picture_style == CAL_PICTURE_TOP || m_picture_style == CAL_PICTURE_LEFT)
				calstr = "Place picture here \n\n To place a picture on the cover page, go to page 1";
			else
				calstr = "\nPlace picture here";

			char caltext[100];
			sprintf (caltext, (LPCTSTR)calstr);

			pFrame = CreateFrame (&(get_text_bound(dwAtPage)), caltext, ALIGN_left, ALIGN_top,
				get_current_panel(), &Style);
			if (pFrame) {
				ol.append(pFrame);
				pFrame->Page(GetPage(dwAtPage));
			}
		}
		
		
		PageRecord* pNewPage = (PageRecord*)get_database()->get_record(GetPage(dwAtPage), NULL, RECORD_TYPE_Page);
		if (pNewPage != NULL)
		{
			get_database()->CopyObjectList(pNewPage->objects(), &ol, pNewPage->Id());
			
			pNewPage->release(TRUE);
		}

		
		numcals--; // decrement month counter
		
		if (numcals > 0)
		{
			AddPage(++dwAtPage); // increment and add page
			if (info.style.type != YEARLY)
				IncrementCalendarDate(&info, TRUE); // adding calendar to end of doc.
			else 
				info.year++; // just increment year
			ccs.info = &info; 
		}
		
	} while (numcals > 0);  // end loop if all calendars have been created


	// maybe add back cover page
	if (m_picture_style == CAL_PICTURE_TOP || m_picture_style == CAL_PICTURE_LEFT)
	{
		pFrame = NULL;
		char bptext [] = "Back \nCover \nPage";
		AddPage(++dwAtPage); // increment and add page
		
		PageRecord* pNewPage = (PageRecord*)get_database()->get_record(GetPage(dwAtPage), NULL, RECORD_TYPE_Page);
		if (pNewPage != NULL)
		{
			pFrame = CreateFrame (&(get_text_bound(dwAtPage+1)), bptext, ALIGN_left, ALIGN_top,
				get_current_panel(), &Style);
			
			if (pFrame != NULL)
			{
				pNewPage->objects()->append(pFrame);
				refresh_object(pFrame);
				pNewPage->release(TRUE);
				pFrame->Page(GetPage(dwAtPage));
			}
		}
	}
	
	dlg.DestroyWindow();
	EndWaitCursor();

	LoadPage(GetPage(startpage));	
	
	return fSuccess;
}

void CCalendarDoc::GoToFirstPage()
{
	//for templates only
	DWORD startpage = 0;
	if (m_picture_style == CAL_PICTURE_TOP || m_picture_style == CAL_PICTURE_LEFT)
	{
		startpage = 1;  //this gives a preview of page 2 and 3
	}
	LoadPage(GetPage(startpage));	
}

void 
CCalendarDoc::ClipBlankPanels(PBOX* pBound)
{
	if (CurrentPageIndex() == 0 || CurrentPageIndex() == NumberOfPages()-1)
	{
		// Set our internal paper to this.
		// Why do we set this every time?
		*m_pPaperInfo = *(GET_PMWAPP()->GetPaperInfo(get_orientation()));

		PPNT new_dims = m_pPaperInfo->GetProjectDims();

		switch (m_picture_style)
		{
			case CAL_PICTURE_TOP:
			{
				// Pick the bottom half.
				pBound->y0 = pBound->y1 - new_dims.y;
				break;
			}
			case CAL_PICTURE_LEFT:
			{
				// Pick the right half.
				pBound->x0 = pBound->x1 - new_dims.x;
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

void 
CCalendarDoc::GetPreviewBound(PBOX* pBound)
{
	INHERITED::GetPreviewBound(pBound);
	ClipBlankPanels(pBound);
}

void 
CCalendarDoc::GetPanelBoundForPreview(PBOX* pBound)
{
	INHERITED::GetPanelBoundForPreview(pBound);
	ClipBlankPanels(pBound);
}
// open calendar doc
BOOL CCalendarDoc::OnOpenDocument(const char *pszPathName)
{                                       

	if (!CPmwDoc::OnOpenDocument(pszPathName))
		return FALSE;

	EnsureCalendarInfoRecord();

	size_to_paper();
	
	SYSTEMTIME Now;
	GetLocalTime(&Now);
	
	CalendarObject * pObj = get_current_calendar(TRUE);
	if (pObj != NULL)
	{
		Calendar cal = pObj->get_calendar();
		CALENDAR_INFO ci = cal.get_info();
		
		if ((ci.year < Now.wYear) && (m_wDocumentFlags == 0))
			GET_PMWAPP()->ShowHelpfulHint("CalChangeDate");
	}

	
	DWORD dwCurPage = CurrentPageIndex();

	PMGPageObjectPtr object;

	for (int x = 0; x < (signed)NumberOfPages(); x++)
	{
		GotoPage(x);

		for (object = (PMGPageObjectPtr)object_list()->first_object();
						object != NULL;
 						object = (PMGPageObjectPtr)object->next_object())
		{
			object->Page(CurrentPageRecord());
		}

	}

	GotoPage(dwCurPage);

	return TRUE;
}

CCalendarDoc::~CCalendarDoc()
{

}

BEGIN_MESSAGE_MAP(CCalendarDoc, CPmwDoc)
	//{{AFX_MSG_MAP(CCalendarDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarDoc commands

/*
// obsolete description
// Return the calendar object for the currently selected date object
// (if there is one).
// If use_global is TRUE, then the calendar object remembered in the
// CCalendarDoc structure is used if nothing else is appropriate.
*/
// current description
// returns the first calendar object found on a page
// returns NULL if no calendar is found
// if use_global is TRUE, returns the first calendar in the project 
// if no calendar is found on the current page.
//

CalendarObjectPtr CCalendarDoc::get_current_calendar(BOOL use_global)
{
	CalendarObjectPtr object;
	DWORD dwAtPage = CurrentPageIndex();
	PageObject* pObject = NULL;// (PageObject*)pPage->objects()->first_object();
	BOOL bRestore = FALSE;
	pObject = find_calendar_object(dwAtPage);
	
	if (use_global)
	{
		if (pObject == NULL && m_picture_style != CAL_PICTURE_NONE) // no calendar was found on that page.
		{
			pObject = find_calendar_object(1);
			bRestore = TRUE; // page changed
		}
	}

	// going to assume it finds a calendar somewhere
	if (pObject != NULL && pObject->type() == OBJECT_TYPE_Calendar)
		object = (CalendarObject*)pObject;

	// restore current page
	if (bRestore)
		LoadPage(GetPage(dwAtPage));

	return object;
}


// steps thru objects on page and returns first calendar object found
// returns NULL if no calendar objects are found
CalendarObject * CCalendarDoc::find_calendar_object(DWORD dwPage)
{
	PageRecord* pPage = NULL;
	PMGDatabase* pDatabase = get_database(); // current database
	PageObject * pRet = NULL;
	//LoadPage(GetPage(dwPage));	
	pPage = (PageRecord*)pDatabase->get_record(GetPage(dwPage), NULL, RECORD_TYPE_Page);
	ASSERT(pPage != NULL);
	pRet = (PageObject*)pPage->objects()->first_object();
	
	// if the passed object is not null and is not a calendar object:
	if (pRet != NULL && pRet->type() != OBJECT_TYPE_Calendar)
	{
		// if this is not a calendar object look for one!
		if (pRet->type() != OBJECT_TYPE_Calendar)	
		{
			do 
			{
				pRet = pRet->next_object();
				if (pRet == NULL)
					break;
				if (pRet->type() == OBJECT_TYPE_Calendar)
					break;
			} while (pRet != NULL);
		}
		
	}
	// else 

	if (pPage)
		pPage->release(TRUE);

	//return the calendar object found
	return (CalendarObject*)pRet;
}

PMGPageObject* CCalendarDoc::PasteBackground(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds, BOOL fDuplicate /*=TRUE*/)
{
	ASSERT(fDuplicate);
	return PasteBackgroundAsPicture(pObject, pOldWorld, pBounds);
}

PMGPageObject* CCalendarDoc::CopyObject(PMGPageObjectPtr pObject, ObjectListPtr pObjects)
{
	PMGPageObject* pNewObject = NULL;

	switch (pObject->type())
	{
		case OBJECT_TYPE_CalendarDate:
		{
		/* Copy any date objects to the list. */
			SHORT nDate = pObject->get_panel();
			PMGPageObjectPtr pSubObject;
			CalendarObject* pCalendarObject = (CalendarObject*)pObject->get_container();
			ASSERT(pCalendarObject != NULL);
			if (pCalendarObject != NULL)
			{
			/* First, we copy any graphic. This should be underneath. */

				if ((pSubObject = pCalendarObject->get_date_graphic(nDate)) != NULL)
				{
					CPmwDoc::CopyObject(pSubObject, pObjects);
				}

			/* Next, we copy any text frame. This should be on top. */

				if ((pSubObject = pCalendarObject->get_date_text(nDate)) != NULL)
				{
					CPmwDoc::CopyObject(pSubObject, pObjects);
				}
			}
			// We leave pNewObject NULL in this case.
			// We only need it for linked frames.
			break;
		}
		default:
		{
			pNewObject = CPmwDoc::CopyObject(pObject, pObjects);
			break;
		}
	}
	return pNewObject;
}

/*
// High-level entry for pasting an object.
*/

PMGPageObject* CCalendarDoc::PasteObject(PageObjectPtr pObject, PBOX* pOldWorld, PBOX* pBounds, BOOL fDuplicate /*=TRUE*/)
{
	CalendarDateObjectPtr pDate;

	if (get_date_selected() == -1
		 || (pDate = (CalendarDateObject*)selected_objects()) == NULL)
	{
		return CPmwDoc::PasteObject(pObject, pOldWorld, pBounds, fDuplicate);
	}
	else
	{
		SHORT nDate = pDate->get_panel();
		CalendarObject* pCalendarObject = (CalendarObject*)pDate->get_container();
		ERRORCODE error = ERRORCODE_None;

		switch (pObject->type())
		{
			case OBJECT_TYPE_Background:
			case OBJECT_TYPE_Graphic:
			{
				GraphicObjectPtr pGraphic;
				if (pObject->type() == OBJECT_TYPE_Background)
				{
					ASSERT(fDuplicate);
					pGraphic = ConvertBackgroundToGraphic((BackgroundObjectPtr)pObject);
					if (pGraphic == NULL)
					{
						error = get_database()->last_creation_error();
					}
				}
				else
				{
					pGraphic = (GraphicObjectPtr)DoDuplicateObject(pObject, NULL, NULL, fDuplicate, &error);
				}

				if (pGraphic != NULL)
				{
					pCalendarObject->make_date_graphic(pGraphic, nDate, m_pPendingCommand == NULL);
					refresh_object(pDate);
					select_object(pDate);
				}
				break;
			}
			case OBJECT_TYPE_Frame:
			{
				CFrameObject* pFrame = (CFrameObject*)DoDuplicateObject(pObject, NULL, NULL, fDuplicate, &error);

				if (pFrame != NULL)
				{
					pCalendarObject->make_date_frame(pFrame, nDate, m_pPendingCommand == NULL);
					refresh_object(pDate);
					select_object(pDate);
				}
				break;
			}
			default:
			{
			/* Can't paste such an animal. */
				break;
			}
		}

		// We don't care about this.
		return NULL;
	}
}

void CCalendarDoc::PrePaste(void)
{
	CalendarDateObject* pDate;

	if (get_date_selected() == -1
		 || (pDate = (CalendarDateObject*)selected_objects()) == NULL)
	{
	// No date selected. Do normal paste.
		CPmwDoc::PrePaste();
	}
	else
	{
	/* We need to create an undo command for this action. */
		if (m_pPendingCommand != NULL)
		{
			od("*** m_pPendingCommand is not NULL! ***\r\n");
			delete m_pPendingCommand;
		}
		m_pPendingCommand = new CCmdCalendarDate(IDCmd_PasteObjects);

		CalendarObject* pCalendarObject = (CalendarObject*)pDate->get_container();
		int nDate = pDate->get_panel();

		if (!m_pPendingCommand->Before(this, pCalendarObject, nDate))
		{
			delete m_pPendingCommand;
			m_pPendingCommand = NULL;
		}
	}
}

/*
// Handle the completion of a successful paste.
// If we are pasting to a date, we need to wrap up and add the pending undo
// command (if we were successful in allocating one).
*/

void CCalendarDoc::PostPaste(void)
{
	if (get_date_selected() == -1)
	{
	// No date selected. Do normal post paste.
		CPmwDoc::PostPaste();
	}
	else
	{
	/* Finish up the paste command. */
		if (m_pPendingCommand != NULL)
		{
			if (m_pPendingCommand->After())
			{
				AddCommand(m_pPendingCommand);
			}
			else
			{
				delete m_pPendingCommand;
			}

		/* Command should always be NULL after this. */

			m_pPendingCommand = NULL;
		}

	/* Document is modified. */

		SetModifiedFlag();
	}
}

/*
// Abort a paste.
// We need to clean up any pending command if we are pasting to a date.
*/

void CCalendarDoc::AbortPaste(void)
{
	if (get_date_selected() == -1)
	{
	/* Not doing anything with a date. Pass it on. */
		CPmwDoc::AbortPaste();
	}
	else
	{
	/* Clean up any command we created. */
		delete m_pPendingCommand;
		m_pPendingCommand = NULL;
	}
}

void CCalendarDoc::SelectNextObject(void)
{
	if (CanSelectNextObject())
	{
		BOOL fSet = TRUE;
		PMGPageObject* pObject = selected_objects();
		CalendarObject* pCalendarObject = get_current_calendar();

		do
		{
			if (pObject == NULL)
			{
				// Nothing selected. Select first object.
				pObject = (PMGPageObject*)object_list()->first_object();
			}
			else if (pObject->type() == OBJECT_TYPE_CalendarDate)
			{
				ChangeDate(1);
				fSet = FALSE;
			}
			else if (pObject->next_selected_object() == NULL
						&& !(pObject->get_flags() & OBJECT_FLAG_grouped))
			{
				// One object selected. Select the next object.
				pObject = (PMGPageObject*)pObject->next_object();
			}
			else
			{
				// More than one object selected. Reset.
				pObject = NULL;
			}
		} while (pObject == pCalendarObject);

		if (fSet)
		{
			// Select the new object.
			deselect_all();
			if (pObject != NULL)
			{
				select_object(pObject);
			}
		}
	}
}

BOOL CCalendarDoc::CanSelectNextObject(void)
{
	PMGPageObject* pObject = selected_objects();
	CalendarObject* pCalendarObject = get_current_calendar();

	return object_list()->first_object() != pCalendarObject
			|| pCalendarObject->next_object() != NULL
			|| (pObject != NULL
					&& pObject->type() == OBJECT_TYPE_CalendarDate
					&& pObject->next_object() != NULL);
}

void CCalendarDoc::SelectPreviousObject(void)
{
	if (CanSelectPreviousObject())
	{
		BOOL fSet = TRUE;
		PMGPageObject* pObject = selected_objects();
		CalendarObject* pCalendarObject = get_current_calendar();

		do
		{
			if (pObject == NULL)
			{
				// Nothing selected. Select last object.
				pObject = (PMGPageObject*)object_list()->last_object();
			}
			else if (pObject->type() == OBJECT_TYPE_CalendarDate)
			{
				// Handle the date thing.
				ChangeDate(-1);
				fSet = FALSE;
			}
			else if (pObject->next_selected_object() == NULL
						&& !(pObject->get_flags() & OBJECT_FLAG_grouped))
			{
				// One object selected. Select previous object.
				pObject = (PMGPageObject*)pObject->previous_object();
			}
			else
			{
				// More than one object selected. Reset.
				pObject = NULL;
			}
		} while (pObject == pCalendarObject);

		// Select the new object.
      if (fSet)
      {
		   deselect_all();
		   if (pObject != NULL)
		   {
			   select_object(pObject);
		   }
      }
	}
}

BOOL CCalendarDoc::CanSelectPreviousObject(void)
{
	PMGPageObject* pObject = selected_objects();
	CalendarObject* pCalendarObject = get_current_calendar();

	return object_list()->first_object() != pCalendarObject
			|| pCalendarObject->next_object() != NULL
			|| (pObject != NULL
					&& pObject->type() == OBJECT_TYPE_CalendarDate
					&& pObject->previous_object() != NULL);
}

void CCalendarDoc::ChangeDate(int nCount)
{
	PMGPageObject* pObject = selected_objects();
	CalendarObjectPtr calobj = (CalendarObjectPtr)pObject->get_container();
	int nCurrentDate = calobj->get_current_date();
	int nDate = nCurrentDate + nCount;

	PMGPageObjectPtr pNewObject;

	if ((pNewObject = (PMGPageObjectPtr)calobj->set_current_date(nDate)) == NULL)
	{
		calobj->set_current_date(nCurrentDate);
	}
	else if (pNewObject != pObject)
	{
		deselect_object(pObject);
		select_object(pNewObject);
	}
}


BOOL CCalendarDoc::CanPageNext(void)
{
	// We allow the user to click off the last page since that
	// brings up the add pages dialog.
	return CurrentPageIndex() < NumberOfPages();
}

void CCalendarDoc::DoPageNext(void)
{
	DWORD dwCurrentPage = CurrentPageIndex();
	if (dwCurrentPage < NumberOfPages()-1)
	{
		LoadPage(GetPage(dwCurrentPage+1));
	}
	else
	{
		// Ask the user if they want to add some pages.
		Util::SendWmCommand(AfxGetMainWnd(), ID_ADD_CALENDAR_PAGE, NULL, 0);
	}
}

// make page large enough to hold calendar + picture
void CCalendarDoc::AdjustPageSize()
{
	// If just loaded, make sure we have our calendar info.
	EnsureCalendarInfoRecord();

	// Get the app's global (printer defined) paper info.
	CPmwApp *pApp = (CPmwApp *)AfxGetApp();

	// Set our internal paper to this.
	*m_pPaperInfo = *(pApp->GetPaperInfo(get_orientation()));

	PPNT new_dims = m_pPaperInfo->GetProjectDims();
	PBOX Margins = m_pPaperInfo->GetPageMargins();

/* Get the paper size. */

	if (get_orientation() == LANDSCAPE)
	{
		PCOORD t = new_dims.x;
		new_dims.x = new_dims.y;
		new_dims.y = t;
	}
	// get current page dimensions
	PPNT dims = get_dimensions();

	switch (m_picture_style)
		{
		case CAL_PICTURE_NONE:  // no extra space is needed
		case CAL_PICTURE_SAME:
			break;
		case CAL_PICTURE_TOP:
			{
				new_dims.y *= 2;
				new_dims.y += Margins.y0 + Margins.y1;
				break;
			}
		case CAL_PICTURE_LEFT:
			{
				new_dims.x *= 2;
				new_dims.x += Margins.x0 + Margins.x1;
				break;
			}
	};
	
	if (dims.x != new_dims.x || dims.y != new_dims.y)
	{
		set_dimensions(new_dims);
		//set_paper_width(0);
		//set_paper_height(0);

		SizeAllPages(dims, new_dims);
		set_portion();
		reset_views();
	}
}

void CCalendarDoc::RecalcPageSize()
{
	// Get the app's global (printer defined) paper info.
	CPmwApp *pApp = (CPmwApp *)AfxGetApp();

	// Set our internal paper to this.
	*m_pPaperInfo = *(pApp->GetPaperInfo(get_orientation()));

	PPNT new_dims = m_pPaperInfo->GetProjectDims();
	PBOX Margins = m_pPaperInfo->GetPageMargins();

	// Get the paper size.
	if (get_orientation() == LANDSCAPE)
	{
		PCOORD t = new_dims.x;
		new_dims.x = new_dims.y;
		new_dims.y = t;
	}
	// get current page dimensions
	PPNT dims = get_dimensions();

	switch (m_picture_style)
		{
		case CAL_PICTURE_NONE:  // no extra space is needed
		case CAL_PICTURE_SAME:
			break;
		case CAL_PICTURE_TOP:
			{
				new_dims.y *= 2;
				new_dims.y += Margins.y0 + Margins.y1;
				break;
			}
		case CAL_PICTURE_LEFT:
			{
				new_dims.x *= 2;
				new_dims.x += Margins.x0 + Margins.x1;
				break;
			}
	};
	
	if (dims.x != new_dims.x || dims.y != new_dims.y)
	{
		set_dimensions(new_dims);
	}
}

void CCalendarDoc::GetPageDims(PPNT& bottom, PPNT& top)
{
	// Get the app's global (printer defined) paper info.
	CPmwApp *pApp = (CPmwApp *)AfxGetApp();

	// Set our internal paper to this.
	*m_pPaperInfo = *(pApp->GetPaperInfo(get_orientation()));

	top = m_pPaperInfo->GetProjectDims();
	if (get_orientation() == LANDSCAPE)
	{
		PCOORD t = top.x;
		top.x = top.y;
		top.y = t;
	}
	bottom = get_dimensions();
}

void CCalendarDoc::SizeCurrentPage(PPNT OldSize, PPNT NewSize)
{
	CPmwDoc::SizeCurrentPage(OldSize, NewSize);

	// Calculate the calendar size.
	PBOX CalendarBound;
	CalcCalendarSize(&CalendarBound);

	// If we have a calendar, force it to be where the calendar should be.
	// This is a hack to fix broken documents.
	PMGPageObject* pObject;
	for (pObject = objects(); pObject != NULL; pObject = (PMGPageObject*)pObject->next_object())
	{
		if (pObject->type() == OBJECT_TYPE_Calendar)
		{
			CALENDAR_INFO ci = ((CalendarObject*)pObject)->get_calendar().get_info();
			if (ci.style.type != YEARLY)
			{
				((CalendarObject*)pObject)->set_bound(CalendarBound);
				pObject->calc();
			}
		}
	}
}

// gets picture style of current calendar
// returns CAL_PICTURE_NONE if no calendar is found
short CCalendarDoc::GetPictureStyle()
{
	return m_picture_style;
}

void CCalendarDoc::SetPictureStyle(short picture_style)
{
	CalendarInfoRecord * pRec = NULL;
	pRec = (CalendarInfoRecord*)get_database()->get_record (document->get_info_record(), NULL, RECORD_TYPE_CalendarInfo);

	if(pRec != NULL)
	{
		m_picture_style = picture_style;
		pRec->record.picture_style = picture_style;
		pRec->release(TRUE);
	}
}

void CCalendarDoc::CalcCalendarSize(PBOX_PTR box)
{

	PPNT dimensions = get_dimensions();	
	PBOX Margins = GetPaperInfo()->GetPageMargins();

	switch (m_picture_style)
	{
		case CAL_PICTURE_NONE:
			{
				box->x0 = 0;
				box->y0 = 0;
				box->x1 = dimensions.x;
				box->y1 = dimensions.y;
				break;
			}
		case CAL_PICTURE_TOP:
			{
				box->x0 = 0;
				box->y0 = dimensions.y / 2 + Margins.y0;
				box->x1 = dimensions.x;
				box->y1 = dimensions.y;
				break;
			}
		case CAL_PICTURE_LEFT:
			{
				box->x0 = dimensions.x / 2 + Margins.x0;
				box->y0 = 0;
				box->x1 = dimensions.x;
				box->y1 = dimensions.y;
				break;
			}
		case CAL_PICTURE_SAME:
			{
				if (get_orientation() == LANDSCAPE)
				{
					int incx = dimensions.x / 10;
					box->x0 =  incx * 4;
					box->y0 = 0;
				}
				else 
				{
					int incy = dimensions.y / 10;
					box->x0 = 0;
					box->y0 =  incy * 4;
				}

				box->x1 = dimensions.x;
				box->y1 = dimensions.y;
				break;
			}
	};
		
}

PBOX CCalendarDoc::get_text_bound (DWORD dwPage)
{
	PBOX box;
	PPNT dimensions = get_dimensions();	

	DWORD dwNumPages = NumberOfPages();

	BOOL bEnd = dwPage == dwNumPages;

	long xmar, ymar;
	xmar = dimensions.x / 10;
	ymar = dimensions.y / 10;

	PBOX Margins = GetPaperInfo()->GetPageMargins();

	switch (m_picture_style)
	{
		case CAL_PICTURE_TOP:
		{
			if (dwPage == 0 || bEnd)
			{
				box.x0 = 0 + xmar;
				box.y0 = (dimensions.y / 2) + Margins.y0 + ymar;	
				box.x1 = dimensions.x - xmar;
				box.y1 = dimensions.y - ymar;	
			}
			else
			{
				box.x0 = 0 + xmar;
				box.y0 = 0 + ymar;
				box.x1 = dimensions.x - xmar;
				box.y1 = (dimensions.y / 2) - Margins.y1 - ymar;
			}
			break;
		}
		case CAL_PICTURE_LEFT:
		{
			if (dwPage == 0 || bEnd)
			{
				box.x0 = (dimensions.x / 2) + Margins.x0 + xmar;
				box.x1 = dimensions.x - xmar;
				box.y0 = 0 + ymar;	
				box.y1 = dimensions.y - ymar;	
			}
			else
			{
				box.x0 = 0 + xmar;
				box.x1 = (dimensions.x / 2) - Margins.x1 - xmar;
				box.y0 = 0 + ymar;
				box.y1 = dimensions.y - ymar;
			}
			break;
		}
		case CAL_PICTURE_SAME:
		{
			long hxmar, hymar;
			hxmar = xmar/2;
			hymar = ymar/2;

			box.x0 = hxmar;
			box.y0 = hymar;
			
			if (get_orientation() == LANDSCAPE)
			{
				box.x1 = xmar * 3 + hxmar;
				box.y1 = ymar * 9 + hymar;
			}
			else
			{
				box.x1 = xmar * 9 + hxmar;
				box.y1 = ymar * 3 + hymar;	
			}

		}
	};
	return box;
}

PBOX CCalendarDoc::GetPicturePageBound(DWORD dwPage)
{
	return get_text_bound(dwPage);
}

CFrameObject * CCalendarDoc::CreateFrame(PBOX_PTR bound, LPSTR text, ALIGN_TYPE align, VERT_ALIGN_TYPE valign, SHORT panel, CTextStyle* style)
{
	FRAMEOBJ_CREATE_STRUCT fcs;
	CFrameObject* pObject;
	
/* Fill out the frame object creation structure. */

	fcs.bound = *bound;
	fcs.alignment = align;
	fcs.vert_alignment = valign;
	fcs.flags = 0;

/* Create the object. */

	if ((pObject = (CFrameObject*)get_database()->new_object(OBJECT_TYPE_Frame, &fcs)) == NULL)
	{
		return pObject;
	}
	pObject->set_panel(panel);
	
	CTxp Txp(get_database());

	/* Stick in some text. */
	Txp.Init(pObject, 0);
	Txp.SetHorizontalAlignment(align);

	if (text == NULL || *text == '\0')
	{
		// No text. Set the base paragraph style.
		pObject->SetTextStyle(style);
	}
	else
	{
		Txp.Style(*style);
		Txp.InsertString(text);
	}

	return pObject;
}

void CCalendarDoc::CreateCalendarInfoRecord ()
{

	CalendarInfoRecord * pRec = NULL;
	pRec = (CalendarInfoRecord*)get_database()->new_record (RECORD_TYPE_CalendarInfo, NULL);

	if(pRec != NULL)
	{
		pRec->record.picture_style = m_picture_style;
		pRec->record.number_of_calendars = m_number_of_calendars;
		document->set_info_record(pRec->Id());
		pRec->release();
	}
}


void CCalendarDoc::GetCalendarInfoRecord ()
{

	CalendarInfoRecord * pRec = NULL;
	pRec = (CalendarInfoRecord*)get_database()->get_record (document->get_info_record(), NULL, RECORD_TYPE_CalendarInfo);

	if(pRec != NULL)
	{
		m_picture_style = pRec->record.picture_style;
		m_number_of_calendars = pRec->record.number_of_calendars;
		pRec->release();
	}
	else // assume this is an old (< version 5) calendar project
	{
		m_picture_style = CAL_PICTURE_NONE;
		m_number_of_calendars = 1;
	}
}

// For loaded documents.
void CCalendarDoc::EnsureCalendarInfoRecord(void)
{
	if (m_number_of_calendars <= 0)
	{
		if (document->get_info_record() == 0)
		{
			m_number_of_calendars = 1;
			m_picture_style = CAL_PICTURE_NONE;
			CreateCalendarInfoRecord();
		}
		else
		{
			GetCalendarInfoRecord();
		}
	}
	ASSERT(m_number_of_calendars > 0);
}

void CCalendarDoc::size_to_paper()
{
	AdjustPageSize();
	//CPmwDoc::size_to_paper();
}

short CCalendarDoc::NumberOfPrinterPages()
{
	short pgs = (short)NumberOfPages();
	if (m_picture_style == CAL_PICTURE_TOP || m_picture_style == CAL_PICTURE_LEFT)
		pgs = ((pgs-2) * 2) + 2; // add cover pages and pictures 
	return pgs;
}

BOOL CCalendarDoc::CreateYearlyCalendar (CALENDAR_CREATE_STRUCT* ccs,  ObjectList* ol, CDialog * pDlg, BOOL bSwap)
{
	
	PPNT increment;
	short rows;
	short cols;

	// copy calendar bounds
	PBOX bound = ccs->bound;
	
	CALENDAR_INFO * oldinfo = ccs->info;
	PBOX oldbound = ccs->bound;
	CALENDAR_INFO newinfo = *(ccs->info);
	
//	newinfo.style.type = MONTHLY;
	newinfo.month = 0; // start in january
	ccs->info = &newinfo;

	if (get_orientation() == LANDSCAPE)
	{
		rows = 4;
		cols = 3;
	}
	else // portrait
	{
		rows = 3;
		cols = 4;
	}

	if (bSwap)
	{
		short m = rows;
		rows = cols;
		cols = m;
	}

	increment.x = (bound.x1 - bound.x0) / rows;
	increment.y = (bound.y1 - bound.y0) / cols;

	PBOX calbox;

	for (int col = 0; col < cols; col++)
	//for (int row = 0; row < rows; row++)
	{
		for (int row = 0; row < rows; row++)
		//for (int col = 0; col < cols; col++)
		{
			calbox.x0 = bound.x0 + (increment.x * row);
			calbox.x1 = bound.x0 + (increment.x * (row+1));
			calbox.y0 = bound.y0 + (increment.y * col);
			calbox.y1 = bound.y0 + (increment.y * (col+1));

			CalendarObject * pObject;

			ccs->bound = calbox;

			if ((pObject = get_database()->create_calendar_object(ccs)) != NULL)
			{
				pObject->set_panel(MAIN_CALENDAR_PANEL);
				pObject->add_flags(OBJECT_FLAG_locked);
				pObject->SetPictureStyle(m_picture_style);
				pObject->set_bound(ccs->bound);
				ol->append(pObject);

				IncrementCalendarDate(ccs->info, TRUE); 
				
				if (pDlg)
					((CCalendarProgressDlg*)pDlg)->Step();					
			}
			else 
			{
				// restore info
				ccs->info = oldinfo;
				ccs->bound = oldbound;
				return FALSE;
			}
		}
	}
	
	// restore info
	ccs->info = oldinfo;
	ccs->bound = oldbound;
	return TRUE;

}

// calculates bounding box for printer page and returns axis flip flag 
FLAGS CCalendarDoc::CalcPagePrintSourceBox (PBOX_PTR box, short page)
{
	FLAGS flags = NULL;

	BOOL bEven = !(page % 2);
	
	BOOL bLast = (page == (short)NumberOfPrinterPages());

	PPNT dimensions = get_dimensions();	
	PBOX Margins = GetPaperInfo()->GetPageMargins();

	switch (m_picture_style)
	{
		case CAL_PICTURE_NONE:
		case CAL_PICTURE_SAME:
			{
				box->x0 = 0;
				box->y0 = 0;
				box->x1 = dimensions.x;
				box->y1 = dimensions.y;
				break;
			}
		case CAL_PICTURE_TOP:
			{
				if (bEven && !bLast)
				{
					// top half of page
					box->x0 = 0;
					box->y0 = 0;//dimensions.y / 2;
					box->x1 = dimensions.x;
					box->y1 = dimensions.y / 2 - Margins.y1;
					flags = OBJECT_FLAG_yflipped;
				}
				else
				{
					// bottom half of page
					box->x0 = 0;
					box->y0 = dimensions.y / 2 + Margins.y0;
					box->x1 = dimensions.x;
					box->y1 = dimensions.y;
				//	if (bLast)
					//	flags = OBJECT_FLAG_yflipped;
				}
							
				break;
			}
		case CAL_PICTURE_LEFT:
			{
				if (bEven && !bLast)
				{
					// right half of page
					box->x0 = 0; //dimensions.x / 2;
					box->y0 = 0;
					box->x1 = dimensions.x / 2 - Margins.x1;
					box->y1 = dimensions.y;
					flags = OBJECT_FLAG_xflipped;
				}
				else
				{
					// right half of page
					box->x0 = dimensions.x / 2 + Margins.x0;
					box->y0 = 0;
					box->x1 = dimensions.x;
					box->y1 = dimensions.y;
					//if (bLast)
					//	flags = OBJECT_FLAG_xflipped;
				}
				break;
			}
	};


	return flags;
}


void CCalendarDoc::ReplaceCalendarInfo(CALENDAR_INFO & info, CalendarObjectPtr pObject)
{
	if (!object_selected(pObject))
		deselect_all();

	// save old info
	CALENDAR_INFO oldinfo = pObject->get_calendar().get_info();
	pObject->get_calendar().get_info() = info;
	
	// rebuild the calendar
	refresh_object(pObject);

	BOOL bFullBuild = TRUE;
	CFrameObject* frameobj;

	if ((frameobj = pObject->get_title_frame()) != NULL)
	{
		// replace the month name
		if (oldinfo.month != info.month)
		{
			ReplaceWord(frameobj,
				Calendar::get_month_name((MONTH)oldinfo.month),
				Calendar::get_month_name((MONTH)info.month),
				FALSE);
		}

		if (oldinfo.year != info.year)
		{
			char old_year[8];
			char new_year[8];

			itoa(oldinfo.year, old_year, 10);
			itoa(info.year, new_year, 10);

			ReplaceWord(frameobj,
				old_year,
				new_year,
				TRUE);
		}
		bFullBuild = FALSE;
	}

	pObject->rebuild(bFullBuild);
	
	refresh_object(pObject);

}

void CCalendarDoc::ChangeCalendarDate(CALENDAR_INFO info)
{
	// step thru and update all calendars on each page of project
	DWORD dwCurPage = CurrentPageIndex();
	DWORD dwNumPages = NumberOfPages();
	BOOL bUpdate = FALSE;

	int numsteps = dwNumPages;

	if (info.style.type == YEARLY)
		numsteps *= 12;

	CString strDlgTitle = "Changing calendar(s)...";

	CCalendarProgressDlg dlg (++numsteps, strDlgTitle);
	dlg.Create(IDD_CALENDAR_PROGRESS);
	dlg.ShowWindow(SW_SHOW);
	dlg.Step();

	BeginWaitCursor();
	for (DWORD i = 0; i < dwNumPages; i++)
	{
		LoadPage (GetPage(i));
		CalendarObject * pObject = find_calendar_object(i);

		if (pObject == NULL)
		{
			// This is an empty page. Include it in the progress.
			dlg.Step();
		}
		else
		{
			while (pObject != NULL) 
			{
				if (pObject->type() == OBJECT_TYPE_Calendar)
				{
					// increment date for monthly/weekly calendar
					if (info.style.type != YEARLY)
					{
						ReplaceCalendarInfo(info, pObject);
						IncrementCalendarDate (&info, TRUE);
					}
					else 
					{
						// save old calendar's month info, but update year info
						Calendar c = ((CalendarObject*)pObject)->get_calendar();
						CALENDAR_INFO ci = c.get_info();
						info.month = ci.month;
						info.week = ci.week;
						ReplaceCalendarInfo(info, pObject);
						bUpdate = TRUE;
					}
					dlg.Step();
				}
				pObject = (CalendarObject*)pObject->next_object();
			}
		}

		// if yearly calendar, update year now.
		if (info.style.type == YEARLY && bUpdate)
			info.year++;
	}
	dlg.DestroyWindow();
	EndWaitCursor();

	// set original page back
	LoadPage (GetPage(dwCurPage));
}

void CCalendarDoc::ChangeLabelFontStyle(CTextStyle style)
{
	// step thru and update all calendars on each page of project
	DWORD dwCurPage = CurrentPageIndex();
	DWORD dwNumPages = NumberOfPages();

	int numsteps = dwNumPages;
	
	CALENDAR_INFO info = get_current_calendar(TRUE)->get_calendar().get_info();
	
	if (info.style.type == YEARLY)
		numsteps *= 12;

	CString strDlgTitle = "Changing font style...";
	CCalendarProgressDlg dlg (++numsteps, strDlgTitle);
	dlg.Create(IDD_CALENDAR_PROGRESS);
	dlg.ShowWindow(SW_SHOW);
	dlg.Step();

	BeginWaitCursor();
	for (DWORD i = 0; i < dwNumPages; i++)
	{
		LoadPage (GetPage(i));
		PageObject * pObject = find_calendar_object(i);
		
		while (pObject != NULL) 
		{
			if (pObject->type() == OBJECT_TYPE_Calendar)
			{
				CalendarRef calendar = ((CalendarObject*)pObject)->get_calendar();
				calendar.SetLabelFontStyle(style);
				dlg.Step();
			}
			pObject = pObject->next_object();
		} 
	}
	dlg.DestroyWindow();
	EndWaitCursor();

	// set original page back
	LoadPage (GetPage(dwCurPage));
}

void CCalendarDoc::ChangeNumberFontStyle(const CTextStyle& style)
{
	// step thru and update all calendars on each page of project
	DWORD dwCurPage = CurrentPageIndex();
	DWORD dwNumPages = NumberOfPages();

	int numsteps = dwNumPages;
	
	CALENDAR_INFO info = get_current_calendar(TRUE)->get_calendar().get_info();
	if (info.style.type == YEARLY)
		numsteps *= 12;

	CString strDlgTitle = "Changing font style...";
	CCalendarProgressDlg dlg (++numsteps, strDlgTitle);
	dlg.Create(IDD_CALENDAR_PROGRESS);
	dlg.ShowWindow(SW_SHOW);
	dlg.Step();

	BeginWaitCursor();
	for (DWORD i = 0; i < dwNumPages; i++)
	{
		LoadPage (GetPage(i));
		PageObject * pObject = find_calendar_object(i);
		
		while (pObject != NULL) 
		{
			if (pObject->type() == OBJECT_TYPE_Calendar)
			{
				CalendarRef calendar = ((CalendarObject*)pObject)->get_calendar();
				calendar.SetNumberFontStyle(style);
				dlg.Step();
			}
			pObject = pObject->next_object();
		} 
	}
	dlg.DestroyWindow();
	EndWaitCursor();

	// set original page back
	LoadPage (GetPage(dwCurPage));
}

// flips objects inside box according to the axis specified in Flags
void CCalendarDoc::FlipPage(PBOX& world, FLAGS Flags)
{
   ObjectList* pObjects = object_list();

   PMGPageObjectPtr pObject;
   //PBOX world;
	
	 //CalcPagePrintSourceBox (&world, nPage);
   //get_panel_world(&world, nPanel);
	 

   for (pObject = (PMGPageObjectPtr)pObjects->last_object();
               pObject != NULL;
               pObject = (PMGPageObjectPtr)pObject->previous_object())
   {
   /* To alternate position. */

      if (Flags & OBJECT_FLAG_xflipped)
      {
         pObject->xflip();
      }
      if (Flags & OBJECT_FLAG_yflipped)
      {
         pObject->yflip();
      }

      if (pObject->get_layout() <= 0)
      {
         PBOX bound = ((RectPageObjectPtr)pObject)->get_unrotated_bound();
         PCOORD tmp;

         if (Flags & OBJECT_FLAG_xflipped)
         {
            tmp = world.x1 - (bound.x0 - world.x0);
            bound.x0 = world.x1 - (bound.x1 - world.x0);
            bound.x1 = tmp;
         }

         if (Flags & OBJECT_FLAG_yflipped)
         {
            tmp = world.y1 - (bound.y0 - world.y0);
            bound.y0 = world.y1 - (bound.y1 - world.y0);
            bound.y1 = tmp;
         }

         ((RectPageObjectPtr)pObject)->set_unrotated_bound(bound);
      }
      calc_object(pObject);
   }
   //panel_group[nPanel]->establish(FALSE);
   //RemoveGroupBits(panel_group[nPanel]);
}



void CCalendarDoc::GetObjectPageDims(PBOX * pBox, PageObject * pObject)
{
	// get object dims
	PBOX objbound = ((RectPageObject*)pObject)->get_unrotated_bound();

	// get page dims
	PPNT dimensions = get_dimensions();	

//	PBOX MarginBox = GetPaperInfo()->GetPageMargins();

//	CFixedRect Margins = GetPaperInfo()->Margins();
//  MarginBox.x0 = MulFixed(Margins.Top, PAGE_RESOLUTION);
//  MarginBox.y0 = MulFixed(Margins.Left, PAGE_RESOLUTION);
//  MarginBox.x1 = MulFixed(Margins.Bottom, PAGE_RESOLUTION);
//  MarginBox.y1 = MulFixed(Margins.Right, PAGE_RESOLUTION);

	// find center of object
	PPNT objcenter;

	objcenter.x = ((objbound.x1 - objbound.x0) / 2) + objbound.x0;
	objcenter.y = ((objbound.y1 - objbound.y0) / 2) + objbound.y0;

	// figure out which half of the page it's on
	//if (get_orientation() == LANDSCAPE)
	//{	
	
	switch (m_picture_style)
		{
		case CAL_PICTURE_NONE:
		case CAL_PICTURE_SAME:
			{
				pBox->x0 = 0;
				pBox->y0 = 0;
				pBox->x1 = dimensions.x;
				pBox->y1 = dimensions.y;
				break;
			}
		case CAL_PICTURE_TOP:
			{
				if (objcenter.y < dimensions.y/2)
				{
					CalcPagePrintSourceBox(pBox, 2); // get top half box
//					pBox->y1 -= MarginBox.x1;
				}
				else 
				{
					CalcPagePrintSourceBox(pBox, 3); // get bottom half box
//					pBox->y0 += MarginBox.x0;
				}
				break;
			}
		case CAL_PICTURE_LEFT:
			{
				if (objcenter.x < dimensions.x/2)
				{
					CalcPagePrintSourceBox(pBox, 2); // left side
//					pBox->x1 -= MarginBox.y1;
				}
				else
				{
					CalcPagePrintSourceBox(pBox, 3); // right side
//					pBox->x0 += MarginBox.y0;
				}
				break;
			}
	};

	// return that box


}
