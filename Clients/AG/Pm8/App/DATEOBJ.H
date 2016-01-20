/*
// $Header: /PM8/App/DATEOBJ.H 1     3/03/99 6:04p Gbeddow $
//
// Calendar date object definitions.
//
// $Log: /PM8/App/DATEOBJ.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:28   Fred
// Initial revision.
// 
//    Rev 1.3   16 Jul 1997 17:56:26   Jay
// Calendar date improvements
// 
//    Rev 1.2   21 Feb 1997 15:14:40   Jay
// New mode handling; crop mode support
// 
//    Rev 1.1   26 Jun 1996 09:06:22   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:45:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:54   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:26   JAY
// Initial revision.
// 
//    Rev 1.4   01 Jun 1994 16:35:34   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.3   17 Feb 1994 09:22:42   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.2   10 Feb 1994 15:14:24   MACDUFF
// Toggle functions now return flag indicating whether they drew anything.
// 
//    Rev 1.1   05 Jan 1994 08:08:18   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:48   JAY
//  
*/

#ifndef __DATEOBJ_H__
#define __DATEOBJ_H__

#include "calendar.h"

class CalendarDateObject : public CFrameObject
{
private:
	CalendarDateObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

	VOID get_enhanced_bound(CalendarRef calendar, PBOX_PTR ebound);

public:

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

/*
// Toggle select marks for an object.
// We override this to give the selection box a little more thickness.
*/

	virtual BOOL toggle_selects(RedisplayContextPtr rc, FLAGS which);

/*
// Return the refresh bounds for this object.
*/

	virtual VOID get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc = NULL);

/*
// Return whether a point is in an object or not.
*/

	virtual BOOL pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object);

/*
// Return whether a point is over an object's move area or not.
*/

	virtual BOOL pt_on_move_area(PPNT p, RedisplayContextPtr rc);

/*
// Move an object a certain amount.
// This override exists to handle the end_drag state (which does a date copy).
*/

	virtual PMG_OBJECT_ACTION move_object(DRAG_STATE state, PPNT vector, PPNT current_xy, LPVOID far *data = NULL, BOOL fPanning = FALSE);

/*
// Primary action.
// This is what the object wants to have happen when the user clicks on it.
// The default is to SELECT the object (once the object is selected,
// movement and resizing are possible).
//
// On entry data points to a variable of LPVOID. This variable gets any
// specific data that goes with the action. This data should not be ignored
// in certain cases (like EDIT actions) because the object may be a place-
// holder for another object. This variable will contain the address of the
// object ('this') on entry, since this is a good default.
//
// If a NULL pointer is passed, this means that the object is not the only
// one selected (like with a shift click or multiple select). The action
// may be different in this case.
*/

	virtual PMG_OBJECT_ACTION primary_action(LPVOID far *data = NULL);

/*
// Secondary action.
// This is what the object wants to have happen when the user double-clicks
// on it. The default is to do nothing.
//
// On entry data points to a variable of LPVOID. This variable gets any
// specific data that goes with the action. This data should not be ignored
// in certain cases (like EDIT actions) because the object may be a place-
// holder for another object. This variable will contain the address of the
// object ('this') on entry, since this is a good default.
//
// If a NULL pointer is passed, this means that the object is not the only
// one selected (like with a shift click or multiple select). The action
// may be different in this case.
*/

	virtual PMG_OBJECT_ACTION secondary_action(LPVOID far *data = NULL);

/*
// Get the properties for this object.
*/

	virtual void GetObjectProperties(CObjectProperties& Properties);

/*
// Set the properties for this object.
*/

	virtual BOOL SetObjectProperties(CObjectProperties& Properties);

/*
// Layout an enhanced graphic.
*/

	VOID layout_enhanced_graphic(GraphicObjectPtr object, CalendarRef calendar);

/*
// Layout an enhanced text frame.
*/

	VOID layout_enhanced_frame(CFrameObject* object, CalendarRef calendar);
};

#endif						/* #ifndef __DATEOBJ_H__ */
