//
// $Header: /PM8/App/BrdrObj.h 1     3/03/99 6:03p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997-1998 Mindscape, Inc. All rights reserved.
//
// Border object definitions.
//
// $Log: /PM8/App/BrdrObj.h $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 10    7/25/98 1:45p Jayn
// Added get_refresh_bound.
// 
// 9     2/20/98 8:03p Hforman
// 
// 8     2/19/98 6:44p Hforman
// background color, etc.
// 
// 7     2/17/98 7:26p Hforman
// interrupting, background color, etc.
// 
// 6     2/12/98 6:02p Hforman
// add param to ShowBorderEditor()
// 
// 5     2/12/98 1:14p Hforman
// added Undo capabilities, other cleanup and fixes
// 
// 4     1/09/98 6:52p Hforman
// 
// 3     1/09/98 11:05a Hforman
// interim checkin
// 
// 2     1/06/98 7:05p Hforman
// work-in-progress
// 
// 1     12/24/97 1:08p Hforman
//

#ifndef __BRDROBJ_H__
#define __BRDROBJ_H__

#include "borders.h"

// forward declarations
class CBEObject;


// stuff used for i/o
struct BorderObjectRecord
{
	int nDataLength;	// length of BEObject data
};

class BorderObject : public RectPageObject
{
	INHERIT(BorderObject, RectPageObject)

private:
	// Private constructor (call the create method)
	BorderObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);
	~BorderObject();

public:

	// The creator for this type of object.
	// Note that this is static so that we can register it with the database.
	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner,
									VOIDPTR creation_data, PageObjectPtr far *object);

	// Show the Border Editor
	int ShowBorderEditor(BOOL fCreating = FALSE);

	// Update an object into a redisplay context.
	virtual UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip,
											UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

	// Get/Set the properties for this object.
	virtual void GetObjectProperties(CObjectProperties& Properties);
	virtual BOOL SetObjectProperties(CObjectProperties& Properties);

	// Recalc after a size or movement.
	virtual VOID calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0);

	// Destroy this object.
	// The object is being removed from the database.
	// Any and all sub-objects must be removed from the database as well.
	// This should not FREE anything (in the memory sense); that's the job of the
	// destructor.
	virtual VOID destroy();

	virtual ERRORCODE read_data(StorageDevicePtr device);
	virtual ERRORCODE write_data(StorageDevicePtr device);

	virtual ST_MAN_SIZE size_data(StorageDevicePtr device);
	virtual BOOL original_dims(PPNT_PTR p);

	virtual BOOL PtIsOpaque(PPNT p, RedisplayContextPtr rc);

	// Assign an object to this.
	virtual ERRORCODE assign(PageObjectRef sobject);

	// Undo State functions
	virtual PageObjectState* CreateState();
	virtual void SaveState(PageObjectState* pState);
	virtual void RestoreState(PageObjectState* pState);

	virtual VOID get_refresh_bound(PBOX_PTR refresh_bound, RedisplayContextPtr rc = NULL);

	// restore old border after an edit
	void	RestoreBorder(CBEObject* pBEObject);
	CBEObject*	GetBEObject()	{ return m_pBEObject; }

protected:
	void SetObjectSize();
	void DrawBackgroundColor(RedisplayContext& rc);

	BorderObjectRecord		m_brecord;
	CBEObject*					m_pBEObject;	// the Border Editor object (owned by DLL)

};

// our UpdateState class
class BorderUpdateState : public UpdateState
{
	friend class BorderObject;
public:
				BorderUpdateState();
	virtual	~BorderUpdateState();

	void*		m_pBEDrawState;	// DLL's saved info (owned by DLL)
};


#endif	// #ifndef __BRDROBJ_H__
