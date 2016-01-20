/*
// CALINFO.H
//
// Calendar Info Definitions.
//
// $Header: /PM8/App/CALINFO.H 1     3/03/99 6:03p Gbeddow $
//
// $Log: /PM8/App/CALINFO.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 4     11/25/97 8:54a Jstoner
// added new calendar type - cal with pic on same page
 * 
 * 3     10/31/97 3:55p Jstoner
 * multi page calendar fixes
// 
//    Rev 1.2   26 Oct 1997 18:57:40   JOE
// modified calendar desktop to handle picture/calendar
// 
//    Rev 1.1   17 Oct 1997 17:32:26   JOE
// multi page calendar modifications
// 
//    Rev 1.0   14 Aug 1997 15:18:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:00   FRED
// Initial revision.
// 
//    Rev 2.2   15 Sep 1995 14:04:12   JAY
// More 32-bit changes.
// 
//    Rev 2.1   01 Aug 1995 12:59:48   JAY
// Changed typedef to straight "struct name".
// 
//    Rev 2.0   07 Feb 1995 15:48:22   JAY
// Initial revision.
// 
//    Rev 1.1   05 Jan 1994 08:08:58   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:23:48   JAY
// Initial revision.
// 
//    Rev 1.11   05 Apr 1993 10:05:06   JAY
// Moved choose_week() prototype under section requiring TREE.H.
// 
//    Rev 1.10   02 Apr 1993 17:02:38   JAY
// Added choose_week().
// Added a parameter to choose_date().
// Removed erroneous global 'info'.
// 
//    Rev 1.9   31 Mar 1993 16:00:14   JAY
// Added parameter to choose_date() (which tree to use)/.
// 
//    Rev 1.8   19 Mar 1993 16:28:18   JAY
// More calendar stuff.
// 
//    Rev 1.7   18 Mar 1993 15:27:34   JAY
// New calendar structure with Groups.
// 
//    Rev 1.6   23 Feb 1993 13:57:24   MACDUFF
// __INC_TREE
// 
//    Rev 1.5   23 Feb 1993 12:12:46   JAY
// More choose date stuff. Delete date stuff. Rebuild calendar stuff.
// 
//    Rev 1.4   19 Feb 1993 16:30:40   JAY
// More fun calendar code (moving toward date enhancement).
// Title editing.
// 
//    Rev 1.3   18 Feb 1993 16:38:12   JAY
// Moved calendar building routines to calbuild.c
// 
//    Rev 1.2   18 Feb 1993 16:00:26   JAY
// Stable version before the addition of enhanced dates, style, etc.
*/

#ifndef __CALINFO_H__
#define __CALINFO_H__

#include "pagedefs.h"

#define LINE_THICKNESS	(PAGE_RESOLUTION/72)

#define MAX_WEEKS		6						/* maximum number of weeks in month */

#define	MINIMUM_LABEL_POINT_SIZE				10

/*
// The months of the year.
// January is 0. December is 11.
*/

typedef enum
{
	NO_MONTH = -1,
	JANUARY,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER,
	MONTHS_IN_YEAR
} MONTH;

/*
// The days of the week.
// Sunday is 0. Saturday is 6.
*/

typedef enum
{
	NO_DAY = -1,
	SUNDAY,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
	DAYS_IN_FULL_WEEK
} WEEKDAY;

/*
// Enums for the various possible parameters we can choose for text.
*/

typedef enum
{
	CALSTYLE_LAST = -1,						/* Whatever we picked last time. */
	CALSTYLE_LINES,
	CALSTYLE_LABEL,
	CALSTYLE_NAMES,
	CALSTYLE_OUTLINE,
	CALSTYLE_COLOR,
	CALSTYLE_SHADOW
} CALSTYLE_ASPECT;

/*
// Types of calendars.
*/

typedef enum
{
	MONTHLY,
	WEEKLY,
	GENERIC,
	YEARLY
} CALENDAR_TYPE;

/*
// Line part of calendar style.
*/

typedef enum
{
	CAL_LINES_NONE,
	CAL_LINES_HORIZONTAL,
	CAL_LINES_ALL
} CAL_LINE_STYLE;

/*
// Weekday name part of calendar style.
*/

typedef enum
{
	WEEKDAY_STYLE_INITIALS,
	WEEKDAY_STYLE_ABBREV,
	WEEKDAY_STYLE_FULL
} WEEKDAY_STYLE;

/*
// Shadow part of calendar style.
*/

typedef enum
{
	CAL_SHADOW_NONE,
	CAL_SHADOW_BLACK,
	CAL_SHADOW_GRAY,
	CAL_SHADOW_LIGHT_GRAY,
	CAL_SHADOW_100,
	CAL_SHADOW_50,
	CAL_SHADOW_25
} CAL_SHADOW_STYLE;

/*
// Label part of calendar style.
*/

typedef enum
{
	CAL_LABEL_TEXT,
	CAL_LABEL_BOXED,
	CAL_LABEL_INVERTED
} CAL_LABEL_STYLE;

/*
// Outline part of calendar style.
*/

typedef enum
{
	CAL_OUTLINE_NONE,
	CAL_OUTLINE_THIN,
	CAL_OUTLINE_THICK
} CAL_OUTLINE_STYLE;

// Calendar picture location
typedef enum
{
	CAL_PICTURE_NONE,  // calendar has no picture
	CAL_PICTURE_TOP,   // calendar has picture above, on separate page
	CAL_PICTURE_LEFT,  // calendar has picture left, on separate page
	CAL_PICTURE_SAME  // calendar has picture on same page
} CAL_PICTURE_STYLE;
/*
// A calendar style.
// DANGER!  Do not change this struct, or it will break downward compatiblity!
*/

typedef struct
{
	SHORT		type;						// CALENDAR_TYPE

	SHORT		line_style;				// CAL_LINE_STYLE
	SHORT		label_style;			// CAL_LABEL_STYLE
	SHORT		outline_style;			// CAL_OUTLINE_STYLE
	SHORT		shadow_style;			// CAL_SHADOW_STYLE
	SHORT		weekday_style;			// WEEKDAY_STYLE
	COLOR		color;

} CALENDAR_STYLE;

/*
// The info structure stored for a calendar document.
// DANGER!  Do not change this struct, or it will break downward compatiblity!
*/

struct CALENDAR_INFO
{
	CALENDAR_STYLE	style;					/* style of calendar we are creating */
	SHORT				year;						/* calendar year we are creating */
	SHORT				month;					/* (MONTH) calendar month we are creating */
	SHORT				week;						/* which week (weekly only) */
	SHORT				first_day_of_week;	/* (WEEKDAY) first day in week */
	SHORT				last_day_of_week;		/* (WEEKDAY) last day in week */
	
	VOID set_default_style(CALENDAR_TYPE type);
};
typedef CALENDAR_INFO far *CALENDAR_INFO_PTR, far &CALENDAR_INFO_REF;

typedef enum
{
	SKELETON_PANEL			= 10,
	TITLE_PANEL,
	SHADOW_PANEL,
	LABEL_PANEL				= 100,		/* label structure */
	LABEL_FRAME_PANEL,					/* label object = weekday + this */
	DATE_PANEL				= 200,		/* date object panel = day + this */
	GRAPHIC_PANEL			= 300,		/* enhanced graphic panel = day + this */
	TEXT_PANEL				= 400			/* enhanced text panel = day + this */
} CALENDAR_PANEL;

/*
// A calendar event type.
*/

typedef enum
{
	CALENDAR_EVENT_ABSOLUTE,			/* Fixed month/day */
	CALENDAR_EVENT_RELATIVE				/* Relative n'th day of month */
} CALENDAR_EVENT_TYPE;

#ifdef UNUSED
/*
// A calendar event structure.
//
// If type is ABSOLUTE
//		"year" is which year (may be -1 for all years)
//		"month" is which month (may be NO_MONTH for all months)
//		"weekday" is unused (and should probably be NO_DAY)
//		"number" is which date
// If type is RELATIVE
//		"year" is which year (may be -1 for all years)
//		"month" is which month (may be NO_MONTH for all months)
//		"weekday" is which day of the week (must NOT be NO_DAY)
//		"number" is which occurrence of weekday in the month
*/

typedef struct calendar_event *CALENDAR_EVENT_PTR;
typedef struct calendar_event
{
	CALENDAR_EVENT_PTR	next;
	CALENDAR_EVENT_TYPE	type;
	SHORT						year;			/* may be -1 */
	WEEKDAY					weekday;		/* may be NO_DAY */
	MONTH						month;		/* may be NO_MONTH */
	SHORT						number;		/* either absolute date or relative "n" */
} CALENDAR_EVENT;
#endif

#define	next_weekday(weekday)	(((weekday) == SATURDAY) ? SUNDAY : (WEEKDAY)((weekday)+1))

#endif					// #ifndef __CALINFO_H__
