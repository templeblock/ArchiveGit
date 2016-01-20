/*
// Calendar command support.
//
// $Workfile: CCOMMAND.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CCOMMAND.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:56   Fred
// Initial revision.
// 
//    Rev 1.1   26 Jun 1996 09:07:30   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:27:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:32:58   FRED
// Initial revision.
// 
//    Rev 1.1   05 Sep 1995 14:21:14   JAY
// Fixed undo of date modifications and freeing of objects.
// 
//    Rev 1.0   09 Mar 1995 08:28:40   JAY
// Initial revision.
*/

#ifndef __CCOMMAND_H__
#define __CCOMMAND_H__

#include "calinfo.h"

/*
// State for holding info about a calendar date.
*/

class CCalendarDateState
{
public:
	CCalendarDateState();
	virtual ~CCalendarDateState();

	BOOL m_fGraphicInUse;						// Is the graphic in use?
	BOOL m_fFrameInUse;							// Is the frame in use?
	GraphicObject* m_pGraphic;
	CFrameObject* m_pFrame;
};

/*
// Command for dealing with calendar dates.
*/

class CCmdCalendarDate : public CCommand
{
public:
	CCmdCalendarDate(WORD wCmdID = 0);
	virtual ~CCmdCalendarDate();

	// Before the change.
	virtual BOOL Before(CPmwDoc* pDoc, CalendarObject* pCalendar, int nDate);
	// After the change. May return FALSE if no change happened.
	virtual BOOL After(void);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:
	BOOL SaveState(CCalendarDateState* pState);
	BOOL RestoreState(CCalendarDateState* pState);

	BOOL SameFrame(void) const
		{ return m_BeforeState.m_pFrame == m_AfterState.m_pFrame; }
	BOOL SameGraphic(void) const
		{ return m_BeforeState.m_pGraphic == m_AfterState.m_pGraphic; }

	CPmwDoc* m_pDoc;						// Document ("mother-of-all").
	CalendarObject* m_pCalendar;		// The calendar object in question.
	int m_nDate;							// The date in question.

	CCalendarDateState m_BeforeState;	// Before state
	CCalendarDateState m_AfterState;		// After state
};

/*
// Command for dealing with calendar style changes.
*/

class CCmdCalendarStyle : public CCommand
{
public:
	CCmdCalendarStyle(WORD wCmdID = 0);
	virtual ~CCmdCalendarStyle();

	// Before the change.
	virtual BOOL Before(CPmwDoc* pDoc, CalendarObject* pCalendar);
	// After the change. May return FALSE if no change happened.
	virtual BOOL After(void);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:
	BOOL SaveInfo(CALENDAR_INFO* pInfo);
	BOOL RestoreInfo(const CALENDAR_INFO& Info);

	CPmwDoc* m_pDoc;						// Document ("mother-of-all").
	CalendarObject* m_pCalendar;		// The calendar object in question.

	CALENDAR_INFO m_BeforeInfo;		// Before state
	CALENDAR_INFO m_AfterInfo;			// After state
};

/*
// Command for dealing with calendar text styles.
*/

class CCmdCalendarTextStyle : public CCommand
{
public:
	CCmdCalendarTextStyle(WORD wCmdID = 0);

	// Before the change.
	virtual BOOL Before(CPmwDoc* pDoc, CalendarObject* pCalendar, const CTextStyle& Style);
	// After the change. May return FALSE if no change happened.
	virtual BOOL After(const CTextStyle& Style);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:
	// Override this.
	virtual BOOL RestoreStyle(const CTextStyle& Style) = 0;

	CPmwDoc* m_pDoc;						// Document ("mother-of-all").
	CalendarObject* m_pCalendar;		// The calendar object in question.

	CTextStyle m_BeforeStyle;			// Before style
	CTextStyle m_AfterStyle;			// After style
};

/*
// Command for dealing with calendar date number styles.
*/

class CCmdNumberStyle : public CCmdCalendarTextStyle
{
public:
	CCmdNumberStyle();

protected:
	virtual BOOL RestoreStyle(const CTextStyle& Style);
};

/*
// Command for dealing with calendar weekday name styles.
*/

class CCmdWeekdayStyle : public CCmdCalendarTextStyle
{
public:
	CCmdWeekdayStyle();

protected:
	virtual BOOL RestoreStyle(const CTextStyle& Style);
};

class CCmdCalendarWhen : public CCmdCalendarStyle
{
	INHERIT(CCmdCalendarWhen, CCmdCalendarStyle)
public:
	CCmdCalendarWhen();
	virtual ~CCmdCalendarWhen();

	// Before the change.
	virtual BOOL Before(CPmwDoc* pDoc, CalendarObject* pCalendar);
	// After the change. May return FALSE if no change happened.
	virtual BOOL After(void);

	// Undo this command.
	virtual BOOL UndoIt(void);
	// Do this command.
	virtual BOOL DoIt(void);

protected:

	BOOL SaveTitleText(DB_RECORD_NUMBER* pRecord);
	BOOL RestoreTitleText(DB_RECORD_NUMBER pRecord);

	DB_RECORD_NUMBER m_BeforeRecord;
	DB_RECORD_NUMBER m_AfterRecord;
};

#endif					// #ifndef __CCOMMAND_H__
