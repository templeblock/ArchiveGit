/*
// $Header: /PM8/App/LINEOBJ.H 1     3/03/99 6:07p Gbeddow $
//
// $Log: /PM8/App/LINEOBJ.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 2     7/16/98 3:09p Rlovejoy
// Added group_calc().
// 
//    Rev 1.0   14 Aug 1997 15:22:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:30   Fred
// Initial revision.
// 
//    Rev 1.4   21 Feb 1997 15:11:34   Jay
// New mode handling; crop mode support
// 
//    Rev 1.3   15 Jul 1996 11:34:28   Jay
//  
// 
//    Rev 1.2   12 Jul 1996 16:47:54   Jay
// Text wrap
// 
//    Rev 1.1   08 Jul 1996 08:49:04   Jay
// Bigger handles. Better pt_in_object
// 
//    Rev 1.0   14 Mar 1996 13:47:14   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:12   FRED
// Initial revision.
// 
//    Rev 1.2   01 Aug 1995 12:59:40   JAY
// Added correct name.
// 
//    Rev 1.1   10 May 1995 13:14:00   JAY
// Now it's really a line.
// 
//    Rev 1.0   09 May 1995 08:07:04   JAY
// Initial revision.
// 
//    Rev 1.0   04 May 1995 17:15:40   JAY
// Initial revision.
*/

class LineObject;
class LineObjectState;

#ifndef __LINEOBJ_H__
#define __LINEOBJ_H__

#include "drawobj.h"

struct LineObjectRecord
{
};

class LineObjectState : public DrawingObjectState
{
public:
	LineObjectRecord	m_LineRecord;
};

class LineObject : public DrawingObject
{
	INHERIT(LineObject, DrawingObject)
private:
	LineObjectRecord m_LineRecord;

/*
// Private constructor.
// Call the create method.
*/

	LineObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

public:

	~LineObject(void);

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

/*
// Toggle select marks for an object.
*/

	virtual BOOL toggle_selects(RedisplayContextPtr rc, FLAGS which);

/*
// Update an object into a redisplay context.
*/

	UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

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
// Return whether a point is in an object or not.
*/

	virtual BOOL pt_in_object(PPNT p, RedisplayContextPtr rc, PMGPageObjectPtr far *sub_object);

/*
// Return whether a box is over a handle or not.
*/

	virtual OBJECT_HANDLE box_on_handle(PBOX_PTR box, RedisplayContextPtr rc);

/*
// Move an object's handle.
*/

	virtual PMG_OBJECT_ACTION move_handle(DRAG_STATE state, PPNT pos, OBJECT_HANDLE handle, FLAGS shift_status, LPVOID far *data = NULL, BOOL fCropping = FALSE);

/*
// Return the refresh bounds for this object.
*/

	virtual VOID get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc = NULL);

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
// Get an object's name.
*/

	virtual BOOL get_name(LPSTR buffer, SHORT size, SHORT max_width);

/*
// Assign an object to this.
*/

	virtual ERRORCODE assign(PageObjectRef sobject);

/*
// Create a state for saving.
// We return a pointer to a new object. This object needs to be freed by
// the caller at some point.
*/

	virtual PageObjectState* CreateState(void);

/*
// Save our state.
*/

	virtual void SaveState(PageObjectState* pState);

/*
// Restore our state.
// The state passed in is NOT deleted by us. It is still the responsibility
// of the caller (or whomever).
*/

	virtual void RestoreState(PageObjectState* pState);

/*
// Add the outline for this object to the passed array.
//
// If pClipBox is not NULL, it is assumed that an intersect test has
// already been performed with this box against the object bound.
*/

	virtual void AddOutline(COutlinePath& Path, PBOX* pClipBox = NULL);

/*
// How does text flow around us?
*/

	virtual long GetFlowAround(void);

protected:
	virtual VOID calc_bounds(PCOORD_PTR the_corner_x, PCOORD_PTR the_corner_y, PCOORD_PTR opp_corner_x, PCOORD_PTR opp_corner_y, PPNT pos, FLAGS key);
};

#endif					// #ifndef __LINEOBJ_H__
