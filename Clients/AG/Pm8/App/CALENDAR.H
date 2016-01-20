/*
// CALENDAR.H
//
// Calendar Object Definitions.
//
// $Header: /PM8/App/CALENDAR.H 1     3/03/99 6:03p Gbeddow $
//
// $Log: /PM8/App/CALENDAR.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 3     12/01/97 8:46a Jstoner
// changed yearly calendar default style settings
 * 
 * 2     10/31/97 3:55p Jstoner
 * multi page calendar fixes
// 
//    Rev 1.0   14 Aug 1997 15:18:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:50   Fred
// Initial revision.
// 
//    Rev 1.1   26 Jun 1996 09:06:18   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:43:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:36   FRED
// Initial revision.
// 
//    Rev 2.1   15 Sep 1995 14:04:12   JAY
// More 32-bit changes.
// 
//    Rev 2.0   07 Feb 1995 15:48:20   JAY
// Initial revision.
// 
//    Rev 1.2   17 Mar 1994 10:20:54   JAY
// Added static functions to return a month or weekday name.
// 
//    Rev 1.1   05 Jan 1994 08:08:26   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:58   JAY
//  
// 
*/

#ifndef __CALENDAR_H__
#define __CALENDAR_H__

#include "pagedefs.h"
#include "text.h"
#include "objlist.h"
#include "calinfo.h"

/*
// The calendar params structure.
*/

FARCLASS Calendar
{
private:
/*
// The calendar info is the information stored in the database record.
*/

	struct
	{
		CALENDAR_INFO	m_Info;
		WORD			m_wVersion;
		//SHORT			m_picture_style;    // CAL_PIC_STYLE
		//SHORT			m_number_of_calendars;	// number of monthly calendars desired
	} m_Record;

/* Calendar temporal dimension variables. */

	WEEKDAY	first_day;
	WEEKDAY	last_day;
	WEEKDAY	skewed_first_day;
	WEEKDAY	skewed_last_day;
	SHORT		days_in_week;
	SHORT		days_in_month;
	SHORT		weeks_in_month;
	SHORT		first_day_of_month;
	SHORT		last_day_of_month;

/* Calendar layout dimension variables. */

	PBOX		full_bound;
	PBOX		label;
	PBOX		bound;
	PCOORD	cell_width;
	PCOORD	cell_height;
	PCOORD	line_thickness;
	PCOORD	shadow_thickness;
	PCOORD	heading_gap;
	SHORT		heading_size;
	BOOL		has_border;
	BOOL		has_label;
	BOOL		has_shadow;

	COLOR		shadow_color;
	SHORT		shadow_pattern;

	CTextStyle		m_LabelFontStyle;
	CTextStyle		m_NumberFontStyle;

	short					m_picture_style;
/*
// The bound for the entire calendar.
// Normally the full page extent.
*/

	PBOX				calendar_bound;

/*
// Calculate a monthly calendar.
*/

	void calculate_monthly_calendar(short);

/*
// Calculate a weekly calendar.
*/

	void calculate_weekly_calendar(void);

/*
// The database and object list used for building a calendar.
*/

	ObjectListPtr list;
	PMGDatabasePtr database;

/*
// Add a group object to a calendar.
// This can only be used by CALBUILD.CPP since it's NEAR.
*/

	GroupObjectPtr near add_group(SHORT panel, FLAGS flags);

/*
// Ensure all calendar groups exist.
// This can only be used by CALBUILD.CPP since it's NEAR.
*/

	BOOL near build_calendar_groups(GroupObjectPtr *skeleton_group,
								GroupObjectPtr *date_group,
								GroupObjectPtr *graphic_group,
								GroupObjectPtr *text_group);

/*
// Add a line (pattern) object to a calendar.
// This can only be used by CALBUILD.CPP since it's NEAR.
*/

	ERRORCODE near add_line(PBOX_PTR box, SHORT pattern, COLOR color, SHORT panel, ObjectListPtr list);

/*
// Add a box object to a calendar.
// This can only be used by CALBUILD.CPP since it's NEAR.
*/

	ERRORCODE near add_box(PBOX_PTR box, SHORT pattern, COLOR color, SHORT panel, PCOORD line_thickness, ObjectListPtr list);
                                   
/*
// Add a title object to a calendar.
// This can only be used by CALBUILD.CPP since it's NEAR.
*/

	ERRORCODE near add_title(ObjectListPtr l, CTextStyle* pStyle);

/*
// Build a monthly calendar.
*/

	ERRORCODE build_monthly(CTextStyle* pTitleStyle);

/*
// Build a weekly calendar.
*/

	ERRORCODE build_weekly(CTextStyle* pTitleStyle);

/*
// Delete all the objects in a calendar group.
*/

	void delete_group(SHORT panel);

/*
// Delete the skeleton group.
// The title may be preserved in this case (and 'style' gets its style).
*/

	void delete_skeleton_group(BOOL reset_title, CTextStyle* pStyle);

/*
// Rebuild the dates for a panel.
*/

	void rebuild_dates(GroupObjectPtr group);

/*
// Do the actual building of a calendar.
*/

	ERRORCODE calc_and_build(CTextStyle* pTitleStyle);


	void calc_calendar_size();


public:

/*
// Constructor.
*/

	Calendar(BOOL temporary = FALSE, PMGDatabasePtr db = NULL)
 			: database(db), list(NULL),
 			  m_NumberFontStyle(db, temporary),
 			  m_LabelFontStyle(db, temporary)
 		{ bound.x0 = bound.x1 = 0; }

/*
// When being destroyed.
*/

	void destroy(void);

/*
// Add a frame object to a calendar.
*/

	ERRORCODE add_frame(PBOX_PTR box, LPSTR text, ALIGN_TYPE align, VERT_ALIGN_TYPE valign, SHORT panel, FLAGS flags, FLAGS sel_flags, ObjectListPtr list, DB_OBJECT_TYPE type, CTextStyle* style = NULL);

/*
// Set the object list used by this calendar.
*/

	void set_object_list(ObjectListPtr l, BOOL set_database = TRUE);

/*
// Set the calendar bound.
*/

	void set_bound(PBOX bound)
		{ calendar_bound = bound; }

/*
// Set the calendar info.
*/

	void set_info(CALENDAR_INFO_REF infop)
		{ m_Record.m_Info = infop; }

/*
// Get the calendar info.
*/

	CALENDAR_INFO_REF get_info(void)
		{ return m_Record.m_Info; }

/*
// Get the calendar type.
*/

	CALENDAR_TYPE get_type(void)
		{ return (CALENDAR_TYPE)get_info().style.type; }

/*
// Return the calculated line thickness for this calendar.
*/

	PCOORD get_line_thickness(void)
		{ return line_thickness; }

/*
// Get the label bound.
*/

	PBOX_REF get_label_bound(void)
		{ return label; }

/*
// Get cell dimensions.
*/

	PPNT get_cell_dimensions(void);

/*
// Get the label font style.
*/

	CTextStyle& LabelFontStyle(void)
		{ return m_LabelFontStyle; }

/*
// Set the label font style.
*/

	void SetLabelFontStyle(const CTextStyle& Style, BOOL update_all = TRUE);

/*
// Get the number font style.
*/

	CTextStyle& NumberFontStyle(void)
		{ return m_NumberFontStyle; }

/*
// Set the number font style.
*/

	void SetNumberFontStyle(const CTextStyle& style, BOOL update_all = TRUE);

/*
// Calculate the calendar from the info.
*/

	void calculate(void);

/*
// Build the calendar from the calculated params.
//
// All created objects are placed in the passed list.
// The database used is the one owning the list.
*/

	ERRORCODE build(BOOL full_build, CTextStyle* pTitleStyle = NULL);

/*
// Find a particular group in a calendar.
*/

	GroupObjectPtr find_group(SHORT panel);

/*
// Find an object.
*/

	PMGPageObjectPtr find_object(SHORT group_panel, SHORT panel, PMG_OBJECT_TYPE type);

/*
// Get a date object.
// The date must be valid (or you get NULL!).
*/

	CalendarDateObjectPtr get_date_object(SHORT date);

/*
// Get the number of days in the calendar.
*/

	SHORT get_days_in_calendar(void)
		{ return days_in_month; }

/*
// Get the first day of the month.
*/

	SHORT get_first_day(void)
		{ return first_day_of_month; }

/*
// Get the style for a frame.
*/

	ERRORCODE get_frame_style(CTextStyle* pStyle, SHORT group_panel, SHORT panel_start);

/*
// Rebuild the calendar.
*/

	void rebuild(BOOL reset_title);

/*
// Read the data for this calendar.
*/

	ERRORCODE read(StorageDevicePtr device);

/*
// Write the data for this calendar.
*/

	ERRORCODE write(StorageDevicePtr device);

/*
// Size the data for this calendar.
*/

	ST_MAN_SIZE size(StorageDevicePtr device);

/*
// Return the name of a month.
*/

	static CHAR *get_month_name(MONTH month);

/*
// Return the name of a weekday.
*/

	static CHAR *get_weekday_name(WEEKDAY weekday);

/*
// Return the number of calendars
*/

//	short GetNumberOfCalendars () { return m_Record.m_number_of_calendars; }

/*
// Return the style of picture on the calendar
*/
	short GetPictureStyle() { return m_picture_style; }

/*
// SetPictureStyle
*/

	void SetPictureStyle(short style) { m_picture_style = style; }

/*
// Set number of calendars
*/

//	void SetNumberOfCalendars(short noc) { m_Record.m_number_of_calendars = noc; }


};

typedef Calendar far *CalendarPtr;
typedef Calendar far &CalendarRef;

#endif					// #ifndef __CALENDAR_H__
