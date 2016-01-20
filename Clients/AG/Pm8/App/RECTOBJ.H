/*
// $Header: /PM8/App/RECTOBJ.H 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/RECTOBJ.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:42   Fred
// Initial revision.
// 
//    Rev 1.2   04 Mar 1997 17:04:10   Jay
// Fixes for drawing objects, cropping, etc.
// 
//    Rev 1.1   16 Jan 1997 13:00:36   Jay
// Square original dims so shift-resize works!
// 
//    Rev 1.0   14 Mar 1996 13:48:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:30   FRED
// Initial revision.
// 
//    Rev 1.2   01 Aug 1995 12:59:40   JAY
// Added correct name.
// 
//    Rev 1.1   13 May 1995 11:14:30   JAY
//  
// 
//    Rev 1.0   04 May 1995 17:15:40   JAY
// Initial revision.
*/

class RectangleObject;
class RectangleObjectState;

#ifndef __RECTOBJ_H__
#define __RECTOBJ_H__

#include "drawobj.h"

struct RectangleObjectRecord
{
};

class RectangleObjectState : public DrawingObjectState
{
public:
	RectangleObjectRecord	m_RectRecord;
};

class RectangleObject : public DrawingObject
{
	INHERIT(RectangleObject, DrawingObject)
private:
	RectangleObjectRecord m_RectRecord;

/*
// Private constructor.
// Call the create method.
*/

	RectangleObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

public:

	~RectangleObject(void);

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

/*
// Get the path for this object.
*/

	virtual BOOL GetPath(COutlinePath& Path, const CRect& crObject, const CRect* pcrOpenObject = NULL);

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
// Get the original dimensions of this drawing object.
*/

	virtual BOOL original_dims(PPNT_PTR p);
	
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
};

#endif					// #ifndef __RECTOBJ_H__
