/*
// $Header: /PM8/App/GRPOBJ.H 1     3/03/99 6:05p Gbeddow $
//
// Group object definitions.
//
// $Log: /PM8/App/GRPOBJ.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     2/04/98 5:05p Jayn
// Added OnPage()
// 
//    Rev 1.0   14 Aug 1997 15:21:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:04   Fred
// Initial revision.
// 
//    Rev 1.6   18 Jul 1997 14:50:12   Jay
// Now saves states for contained objects.
// 
//    Rev 1.5   25 Feb 1997 12:57:22   Jay
// Cropping and extensions
// 
//    Rev 1.4   23 Jan 1997 11:05:46   Jay
// New popup palettes
// 
//    Rev 1.3   26 Jul 1996 11:38:54   Jay
// Page stuff
// 
//    Rev 1.2   18 Jul 1996 11:10:10   Jay
// Better flow around objects.
// 
//    Rev 1.1   12 Jul 1996 16:47:52   Jay
// Text wrap
// 
//    Rev 1.0   14 Mar 1996 13:43:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:38   FRED
// Initial revision.
// 
//    Rev 2.1   15 Feb 1995 17:03:12   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:40   JAY
// Initial revision.
// 
//    Rev 1.6   04 Nov 1994 12:23:04   JAY
// Added destroy_objects().
// 
//    Rev 1.5   01 Jun 1994 16:35:30   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.4   17 Feb 1994 09:22:38   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.3   28 Jan 1994 17:11:36   JAY
//  
// 
//    Rev 1.2   19 Jan 1994 17:08:16   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:08:30   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:26:08   JAY
//  
*/

#ifndef __GRPOBJ_H__
#define __GRPOBJ_H__

#include "objlist.h"

typedef struct
{
	PBOX original_bound;
} GROUP_CREATE_STRUCT, far *GROUP_CREATE_STRUCT_PTR;

struct GroupObjectRecord
{
	PBOX	original_bound;
};

/*
// The class for saving a GroupObject's state.
*/

class GroupObjectState : public RectPageObjectState
{
public:
	// The record for this group.
	GroupObjectRecord	m_GRecord;

	// States for the contained objects.
	CObjectStateArray m_States;
};

class GroupObject : public RectPageObject
{
	INHERIT(GroupObject, RectPageObject)
private:
	GroupObjectRecord grecord;

	ObjectList list;

/*
// Invoke the calc method of each object in the group.
*/

	VOID near recalc_group(VOID);

protected:
/*
// Private constructor.
// Call the create method.
*/

	GroupObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

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
// Update all the child object parent pointers.
*/

	VOID adopt_children(VOID);

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

/***************************/
/* Group-specific methods. */
/***************************/

	ObjectListPtr object_list(VOID) { return &list; }

	VOID set_original_bound(PBOX bound) { grecord.original_bound = bound; }
	PBOX get_original_bound(void) { return grecord.original_bound; }

/*
// Establish a group from the list of objects.
// This makes a group current with respect to the objects in its list.
// This is psuedo-equivalent to ungrouping and then regrouping a group.
//
// This is used when objects have been added to a group's list after the
// group has been formed.
*/

	VOID establish(BOOL compute_bound = TRUE);

	// Set the page for this object.
	virtual void Page(DB_RECORD_NUMBER lPage);

/************************************/
/* Generally useful object methods. */
/************************************/

/*
// Destroy all objects in this group.
*/

	void destroy_objects(void);

/*
// Destroy this object.
// The object is being removed from the database.
// Any and all sub-objects must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

/*
// Recalc after a size or movement.
*/

	virtual VOID calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0);

/*
// Flip an object left to right.
*/

	virtual VOID xflip(BOOL fChangeAngle = TRUE);

/*
// Flip an object top to bottom.
*/

	virtual VOID yflip(BOOL fChangeAngle = TRUE);

/*
// Notify an object that its image has changed.
*/

	virtual VOID changed_image(VOID);

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
// Assign an object to this.
*/

	virtual ERRORCODE assign(PageObjectRef sobject);

/*
// Get the original dimensions of this group.
*/

	virtual BOOL original_dims(PPNT_PTR p);

/*
// Return the refresh bounds for this object.
*/

	virtual VOID get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc = NULL);

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
// Set whether the object is on or off the page at the moment.
*/

	virtual void OnPage(BOOL fOnPage);

/*
// Get the properties for this object.
*/

	virtual void GetObjectProperties(CObjectProperties& Properties);

/*
// Set the properties for this object.
*/

	virtual BOOL SetObjectProperties(CObjectProperties& Properties);

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
	virtual void SetFlowAround(long lFlowAround);
};

/*
// A group object update state.
*/

class GroupUpdateState : public UpdateState
{
	friend class GroupObject;
private:
	PMGPageObjectPtr object;
	UpdateStatePtr object_state;
public:
	~GroupUpdateState();
};

typedef GroupUpdateState far *GroupUpdateStatePtr;

#endif					// #ifndef __GRPOBJ_H__
