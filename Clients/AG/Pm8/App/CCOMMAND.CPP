/*
// $Header: /PM8/App/CCOMMAND.CPP 1     3/03/99 6:03p Gbeddow $
//
// Calendar command handlers.
//
// $Log: /PM8/App/CCOMMAND.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     11/18/97 11:10a Jstoner
// enhanced calendar menu items to deal with yearly and/or multipage
// calendar projects
// 
//    Rev 1.0   14 Aug 1997 15:18:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:28   Fred
// Initial revision.
// 
//    Rev 1.3   04 Sep 1996 18:11:24   Jay
// Spell-check fixes.
// 
//    Rev 1.2   26 Jun 1996 09:07:30   Jay
//  
// 
//    Rev 1.1   05 Jun 1996 08:21:42   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:25:46   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:04   FRED
// Initial revision.
// 
//    Rev 1.1   05 Sep 1995 14:21:14   JAY
// Fixed undo of date modifications and freeing of objects.
// 
//    Rev 1.0   09 Mar 1995 08:28:40   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "grafobj.h"
#include "frameobj.h"
#include "calobj.h"
#include "dateobj.h"
#include "ccommand.h"
#include "ctxp.h"
#include "clip.h"
#include "caldoc.h"
/////////////////////////////////////////////////////////////////////////////
// CCalendarDateState

CCalendarDateState::CCalendarDateState()
{
	m_fGraphicInUse = TRUE;
	m_fFrameInUse = TRUE;
	m_pGraphic = NULL;
	m_pFrame = NULL;
}

CCalendarDateState::~CCalendarDateState()
{
	if (!m_fGraphicInUse)
	{
		delete m_pGraphic;
	}
	if (!m_fFrameInUse)
	{
		delete m_pFrame;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCmdCalendarDate

CCmdCalendarDate::CCmdCalendarDate(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
}

CCmdCalendarDate::~CCmdCalendarDate()
{
}

static void UpdateOnPageState(CCalendarDateState* pState)
{
	if (pState->m_pFrame != NULL)
	{
		pState->m_pFrame->OnPage(pState->m_fFrameInUse);
	}
	if (pState->m_pGraphic != NULL)
	{
		pState->m_pGraphic->OnPage(pState->m_fGraphicInUse);
	}
}

/*
// Before the change.
*/

BOOL CCmdCalendarDate::Before(CPmwDoc* pDoc, CalendarObject* pCalendar, int nDate)
{
	m_pDoc = pDoc;
	m_pCalendar = pCalendar;
	m_nDate = nDate;

	SaveState(&m_BeforeState);
	return TRUE;
}

/*
// After the change. May return FALSE if no change happened.
*/

BOOL CCmdCalendarDate::After(void)
{
	SaveState(&m_AfterState);

	m_BeforeState.m_fFrameInUse = SameFrame();
	m_BeforeState.m_fGraphicInUse = SameGraphic();
	UpdateOnPageState(&m_BeforeState);

	return TRUE;
}

/*
// Undo this command.
*/

BOOL CCmdCalendarDate::UndoIt(void)
{
	m_AfterState.m_fFrameInUse = SameFrame();
	m_AfterState.m_fGraphicInUse = SameGraphic();
	UpdateOnPageState(&m_AfterState);

	return RestoreState(&m_BeforeState);
}

/*
// Do this command.
*/

BOOL CCmdCalendarDate::DoIt(void)
{
	m_BeforeState.m_fFrameInUse = SameFrame();
	m_BeforeState.m_fGraphicInUse = SameGraphic();
	UpdateOnPageState(&m_BeforeState);

	return RestoreState(&m_AfterState);
}

/*
// Save a state.
*/

BOOL CCmdCalendarDate::SaveState(CCalendarDateState* pState)
{
	// Remember the graphic at this point.
	pState->m_pGraphic = m_pCalendar->get_date_graphic(m_nDate);

	// Remember the frame at this point.
	pState->m_pFrame = m_pCalendar->get_date_text(m_nDate);

	// The objects in question are now in use.
	pState->m_fFrameInUse = TRUE;
	pState->m_fGraphicInUse = TRUE;

	UpdateOnPageState(pState);

	// We succeeded.
	return TRUE;
}

BOOL CCmdCalendarDate::RestoreState(CCalendarDateState* pState)
{
	// Attach our graphic.
	m_pCalendar->make_date_graphic(pState->m_pGraphic, m_nDate, FALSE);

	// Attach our frame.
	m_pCalendar->make_date_frame(pState->m_pFrame, m_nDate, FALSE);

	// The objects in question are now in use.
	pState->m_fFrameInUse = TRUE;
	pState->m_fGraphicInUse = TRUE;

	UpdateOnPageState(pState);

	// The document has been modified.
	m_pDoc->SetModifiedFlag();

/* Bring back the context of our date. */

	CalendarDateObjectPtr pDate = m_pCalendar->get_date_object(m_nDate);

	m_pCalendar->set_current_date(m_nDate);

	m_pDoc->deselect_all();
	m_pDoc->select_object(pDate);
	m_pDoc->refresh_object(pDate);

	// We succeeded.
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdCalendarStyle

CCmdCalendarStyle::CCmdCalendarStyle(WORD wCmdID /*=0*/)
	: CCommand(wCmdID)
{
}

CCmdCalendarStyle::~CCmdCalendarStyle()
{
}

/*
// Before the change.
*/

BOOL CCmdCalendarStyle::Before(CPmwDoc* pDoc, CalendarObject* pCalendar)
{
	m_pDoc = pDoc;
	m_pCalendar = pCalendar;

	return SaveInfo(&m_BeforeInfo);
}

/*
// After the change. May return FALSE if no change happened.
*/

BOOL CCmdCalendarStyle::After(void)
{
	return SaveInfo(&m_AfterInfo);
}

/*
// Undo this command.
*/

BOOL CCmdCalendarStyle::UndoIt(void)
{
	return RestoreInfo(m_BeforeInfo);
}

/*
// Do this command.
*/

BOOL CCmdCalendarStyle::DoIt(void)
{
	return RestoreInfo(m_AfterInfo);
}

/*
// Save a calendar's info.
*/

BOOL CCmdCalendarStyle::SaveInfo(CALENDAR_INFO* pInfo)
{
	// Remember the calendar info.
	*pInfo = m_pCalendar->get_calendar().get_info();

	// We succeeded.
	return TRUE;
}

BOOL CCmdCalendarStyle::RestoreInfo(const CALENDAR_INFO& Info)
{
	// Don't let anything be selected.
	m_pDoc->deselect_all();

	// Restore the calendar info.
	m_pCalendar->get_calendar().get_info() = Info;

	// Rebuild the calendar.
	//m_pDoc->refresh_object(m_pCalendar);

	m_pDoc->BeginWaitCursor();

	((CCalendarDoc*)m_pDoc)->ChangeCalendarDate(Info);
	//m_pCalendar->rebuild(FALSE);
	m_pDoc->EndWaitCursor();

	//m_pDoc->refresh_object(m_pCalendar);

	// The document has been modified.
	m_pDoc->SetModifiedFlag();

	// We succeeded.
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdCalendarTextStyle

CCmdCalendarTextStyle::CCmdCalendarTextStyle(WORD wCmdID /*=0*/)
	: CCommand(wCmdID), m_BeforeStyle(NULL), m_AfterStyle(NULL)
{
}

/*
// Before the change.
*/

BOOL CCmdCalendarTextStyle::Before(CPmwDoc* pDoc, CalendarObject* pCalendar, const CTextStyle& Style)
{
	m_pDoc = pDoc;
	m_pCalendar = pCalendar;
	m_BeforeStyle = Style;

	return TRUE;
}

/*
// After the change. May return FALSE if no change happened.
*/

BOOL CCmdCalendarTextStyle::After(const CTextStyle& Style)
{
	m_AfterStyle = Style;

	return TRUE;
}

/*
// Undo this command.
*/

BOOL CCmdCalendarTextStyle::UndoIt(void)
{
	return RestoreStyle(m_BeforeStyle);
}

/*
// Do this command.
*/

BOOL CCmdCalendarTextStyle::DoIt(void)
{
	return RestoreStyle(m_AfterStyle);
}

/////////////////////////////////////////////////////////////////////////////
// CCmdNumberStyle

CCmdNumberStyle::CCmdNumberStyle()
	: CCmdCalendarTextStyle(IDCmd_DateNumberFont)
{
}

BOOL CCmdNumberStyle::RestoreStyle(const CTextStyle& Style)
{
	m_pCalendar->get_calendar().SetNumberFontStyle(Style);
	((CCalendarDoc*)m_pDoc)->ChangeNumberFontStyle(Style);
	m_pDoc->SetModifiedFlag();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdWeekdayStyle

CCmdWeekdayStyle::CCmdWeekdayStyle()
	: CCmdCalendarTextStyle(IDCmd_WeekdayNameFont)
{
}

BOOL CCmdWeekdayStyle::RestoreStyle(const CTextStyle& Style)
{
	m_pCalendar->get_calendar().SetLabelFontStyle(Style);
	((CCalendarDoc*)m_pDoc)->ChangeLabelFontStyle(Style);
	m_pDoc->SetModifiedFlag();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCmdCalendarWhen

CCmdCalendarWhen::CCmdCalendarWhen()
	: CCmdCalendarStyle(IDCmd_CalendarWhen)
{
	m_BeforeRecord = 0;
	m_AfterRecord = 0;
}

CCmdCalendarWhen::~CCmdCalendarWhen()
{
// Only get the database if we have a record to delete.
	if (m_BeforeRecord != 0 || m_AfterRecord != 0)
	{
	// We need to clean up. Get the database.
		PMGDatabase* pDatabase;
		if ((pDatabase = CCommand::GetUndoDatabase()) != NULL)
		{
		// Handle the before record.
			if (m_BeforeRecord != 0)
			{
				pDatabase->delete_record(m_BeforeRecord, RECORD_TYPE_Paragraph);
			}
		// Handle the after record.
			if (m_AfterRecord != 0)
			{
				pDatabase->delete_record(m_AfterRecord, RECORD_TYPE_Paragraph);
			}
		}
	}
}

/*
// Before the change.
*/

BOOL CCmdCalendarWhen::Before(CPmwDoc* pDoc, CalendarObject* pCalendar)
{
	return INHERITED::Before(pDoc, pCalendar) && SaveTitleText(&m_BeforeRecord);
}

/*
// After the change. May return FALSE if no change happened.
*/

BOOL CCmdCalendarWhen::After(void)
{
	return INHERITED::After()
				&& SaveTitleText(&m_AfterRecord);
}

/*
// Undo this command.
*/

BOOL CCmdCalendarWhen::UndoIt(void)
{
	return /*RestoreTitleText(m_BeforeRecord) &&*/ INHERITED::UndoIt();
}

/*
// Do this command.
*/

BOOL CCmdCalendarWhen::DoIt(void)
{
	return /*RestoreTitleText(m_AfterRecord) && */INHERITED::DoIt();
}

/*
// Save the title text.
*/

BOOL CCmdCalendarWhen::SaveTitleText(DB_RECORD_NUMBER* pRecord)
{
/*
// Just to be sure in case of failure or vacuous success.
*/

	*pRecord = 0;

/*
// Get the database to save our text into.
*/

	PMGDatabasePtr pDatabase;

	if ((pDatabase = GetUndoDatabase()) == NULL)
	{
		return FALSE;
	}

#if 0
/*
// Get the title frame.
// If this fails (and I can't say why it would), it may mean there is no
// title text. So we don't have anything to do! So we were successful.
*/
	FrameObjectPtr pFrameObject;

	if ((pFrameObject = m_pCalendar->get_title_frame()) == NULL)
	{
		return TRUE;			// Nothing to do. So we did it.
	}

/*
// Setup a txp to use to copy text to the database.
*/

	CTxp Txp(pDatabase);

	Txp.Init(pFrameObject, 0);

/* Do the copy. */

	Txp.SelectAll();					/* We want to copy all the text. */
	Txp.CalcStyle();
	return (CopyTextToDatabase(&Txp, pDatabase, pRecord) == ERRORCODE_None);
#endif
	return TRUE;
}

/*
// Restore the title text.
*/

BOOL CCmdCalendarWhen::RestoreTitleText(DB_RECORD_NUMBER Record)
{
/*
// If there is no data to restore, we are done.
*/

	if (Record == 0)
	{
		return TRUE;
	}

/*
// Get the database to restore our text from.
*/

	PMGDatabasePtr pDatabase;

	if ((pDatabase = GetUndoDatabase()) == NULL)
	{
		return FALSE;
	}

#if 0
/*
// Get the title frame.
// If this fails (and I can't say why it would), it means that we were unable
// to get the title text; yet, we have a record that says we USED to have
// title text. This is a bad error.
*/
	FrameObjectPtr pFrameObject;

	if ((pFrameObject = m_pCalendar->get_title_frame()) == NULL)
	{
		return FALSE;			// We couldn't do what we needed to do.
	}

/*
// Setup a txp to use to paste text from the database.
*/

	CTxp Txp(pDatabase);

	Txp.Init(pFrameObject, 0);

/*
// Delete all existing text from the frame.
// Our pasted text takes its place.
*/

	Txp.SelectAll();					/* We want to copy all the text. */
	if (Txp.SomethingSelected())
	{
		Txp.DeleteText(1);
	}

/* Do the paste to get our new text. */

	Txp.CalcStyle();
	return (PasteTextFromParagraph(&txp, pDatabase, Record) == ERRORCODE_None);
#endif
	return TRUE;
}
