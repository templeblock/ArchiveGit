/*
// $Header: /PM8/App/CALOBJ.H 1     3/03/99 6:03p Gbeddow $
//
// Calendar object definitions.
//
// $Log: /PM8/App/CALOBJ.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 3     9/03/98 5:06p Jayn
// Added calendar pages now propagate the label and number font styles.
 * 
 * 2     10/31/97 3:55p Jstoner
 * multi page calendar fixes
// 
//    Rev 1.1   21 Oct 1997 15:13:54   JOE
// multi-page calendar mods
// 
//    Rev 1.0   14 Aug 1997 15:18:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:52   Fred
// Initial revision.
// 
//    Rev 1.2   08 Jul 1996 08:49:02   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.1   26 Jun 1996 09:06:20   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:42:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:05:54   FRED
// Initial revision.
// 
//    Rev 2.1   09 Mar 1995 08:26:52   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:48:22   JAY
// Initial revision.
// 
//    Rev 1.5   09 Jan 1995 16:26:44   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.4   01 Jun 1994 16:35:36   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.3   17 Feb 1994 09:22:32   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.2   01 Feb 1994 14:02:32   JAY
// Added a get_refresh_bounds() method to override the (not-working)
// GroupObject method.
// 
//    Rev 1.1   05 Jan 1994 08:08:56   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:26:06   JAY
//  
*/


#ifndef __CALOBJ_H__
#define __CALOBJ_H__

#include "calendar.h"
#include "grpobj.h"


class CFrameObject;
class CTextStyle;

struct CALENDAR_CREATE_STRUCT
{
	CALENDAR_CREATE_STRUCT()
		: m_pLabelFontStyle(NULL), m_pNumberFontStyle(NULL)
		{}

	CALENDAR_INFO_PTR info;
	PBOX bound;
	BOOL full_build;
	short picture_style;
	CTextStyle* m_pLabelFontStyle;
	CTextStyle* m_pNumberFontStyle;
};
typedef CALENDAR_CREATE_STRUCT far *CALENDAR_CREATE_STRUCT_PTR;

FARCLASS CalendarObject : public GroupObject
{
private:
/*
// Calendar information.
*/

	Calendar calendar;

/*
// The currently selected date.
*/

	SHORT current_date;

/*
// Private constructor.
// Call the create method.
*/

	CalendarObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, short ps);

/*
// See if a point is within the object of a group.
*/

	BOOL pt_in_group(GroupObjectPtr group, PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object);

public:

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

/*
// Update an object into a redisplay context.
*/

	UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

/*
// read_data()
//
// This is a method invoked by the read method to handle object-specific data.
*/

	ERRORCODE read_data(StorageDevicePtr device);

/*
// write_data()
//
// This is a method invoked by the write method to handle object-specific data.
*/

	ERRORCODE write_data(StorageDevicePtr device);

/*
// size_data()
//
// This is a method invoked by the size method to handle object-specific data.
*/

	ST_MAN_SIZE size_data(StorageDevicePtr device);

/*
// Fix a weekly calendar's date numbers.
*/

	VOID fix_weekly(VOID);

/*
// Recalc after a size or movement.
*/

	virtual VOID calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0);

/*
// Do a group calc.
// This has additional parameters from the normal calc method.
// Should this have a different name or just overload calc()?
// (I personally prefer a different name because it seems clearer.)
*/

	virtual VOID group_calc(PBOX_PTR current_group_bound,
 									PBOX_PTR org_group_bound,
 									FLAGS group_flags,
 									ROTATION_COMPONENT_PTR group_rotc);
/*
// Destroy this object.
// The object is being removed from the database.
// Any and all sub-objects must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

/*
// Assign an object to this.
*/

	virtual ERRORCODE assign(PageObjectRef sobject);

/*
// Return whether a point is in an object or not.
*/

	virtual BOOL pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object);

/*
// Return the refresh bounds for this object.
*/

	virtual VOID get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc = NULL);

/************************************/
/* Useful/necessary calendar stuff. */
/************************************/

/*
// Get the calendar in this object.
*/

	CalendarRef get_calendar(VOID)
		{ return calendar; }

/*
// Find an object with a certain panel, date and type.
// If date is -1, current_date is used.
*/

	PMGPageObjectPtr find_object(SHORT panel, SHORT date, PMG_OBJECT_TYPE type);

/*
// Find a particular group in a calendar.
*/

	GroupObjectPtr find_group(SHORT panel)
		{ return calendar.find_group(panel); }

/*
// Return the number of days in the calendar.
//
// This is the number of days actually existing in the calendar object. So if
// this is a weekly calendar, the number will be 7 or less.
*/

	SHORT days_in_calendar(VOID);

/*
// Return the date number of the first day in the calendar.
*/

	SHORT first_day(VOID);

/*
// Return the date object for a particular date.
// This object is the one with the number and is never edited directly
// (but its panel number can be vital for accessing other things).
//
// The date number passed should be between 0 and the number of days in the
// month (see above function) or such a number plus DATE_PANEL (CALINFO.H).
//
// The date object returned is the DATE frame for this date or NULL if the
// number passed is invalid.
*/

	CalendarDateObjectPtr get_date_object(SHORT date = -1);

/*
// Set the currnt date.
// This registers the internal "selected" variable so other functions will
// have a default value.
//
// The date number passed should be between 0 and the number of days in the
// month (see above function) or such a number plus DATE_PANEL (CALINFO.H).
//
// The frame object returned is the DATE frame for this date or NULL if the
// number passed is invalid.
*/

	CalendarDateObjectPtr set_current_date(SHORT date);
	
/*
// Get the current date.
*/

	SHORT get_current_date(VOID)
		{ return current_date; }

/*
// Return the graphic for a particular date.
//
// The date number passed should be between 0 and the number of days in the
// month (see above function) or such a number plus DATE_PANEL (CALINFO.H).
//
// NULL is returned if the date is invalid or if the date has no graphic. (If
// you need to be able to distinguish this case, you could always call
// get_date_frame() first to validate!)
*/

	GraphicObjectPtr get_date_graphic(SHORT date = -1);

/*
// Return the text box for a particular date. (This is the one the user edits).
//
// The date number passed should be between 0 and the number of days in the
// month (see above function) or such a number plus DATE_PANEL (CALINFO.H).
//
// If the date has no text frame, one will be created (if 'create' is TRUE).
//
// NULL is returned if the date is invalid or if the date has no text frame
// and one could not be created.
*/

	CFrameObject* get_date_text(SHORT date = -1, BOOL create = FALSE);

/*
// Get the title frame.
*/

	CFrameObject* get_title_frame(VOID);

/*
// Delete the text for a date.
*/

	VOID delete_date_text(SHORT date = -1, BOOL fDelete = TRUE);

/*
// Delete the graphic for a date.
*/

	VOID delete_date_graphic(SHORT date = -1, BOOL fDelete = TRUE);

/*
// Make a new graphic into a date graphic.
// This will free any old graphic if there is one.
*/

	VOID make_date_graphic(GraphicObjectPtr object, SHORT date = -1, BOOL fDeleteOld = TRUE);

/*
// Make a new frame into a date frame.
// This will free any old frame if there is one.
*/

	VOID make_date_frame(CFrameObject* object, SHORT date = -1, BOOL fDeleteOld = TRUE);

/*
// Rebuild the calendar.
// The title may be preserved if desired.
*/

	VOID rebuild(BOOL reset_title);

/*
// Return the date object under a certain point.
*/

	PMGPageObjectPtr FindDate(PPNT pPnt, SHORT* pPanel);

/*
// Return the number of calendars
*/

//	short GetNumberOfCalendars () { return calendar.GetNumberOfCalendars(); }

/*
// Return the style of picture on the calendar
*/
	short GetPictureStyle() { return calendar.GetPictureStyle(); }

/*
// set the picture style
*/
	void SetPictureStyle (short style) { calendar.SetPictureStyle(style); }

/*
// set the number of calendars
*/
//	void SetNumberOfCalendars (short noc) { calendar.SetNumberOfCalendars(noc); }

};

#endif					// #ifndef __CALOBJ_H__
