/*
// $Header: /PM8/App/OLEOBJ.H 1     3/03/99 6:08p Gbeddow $
//
// OLE object definitions.
//
// $Log: /PM8/App/OLEOBJ.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     3/24/98 10:52a Jayn
// More serif object. New thumbnail sizes.
// 
//    Rev 1.0   14 Aug 1997 15:23:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:29:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:12   FRED
// Initial revision.
// 
//    Rev 1.3   15 Nov 1995 15:23:12   JAY
// OLE fixes, etc.
// 
//    Rev 1.2   28 Sep 1995 12:13:14   JAY
// More drag/drop stuff
// 
//    Rev 1.1   27 Sep 1995 15:26:32   JAY
// New OLE code
// 
//    Rev 1.0   29 Mar 1995 09:18:58   JAY
// Initial revision.
*/

#ifndef __OLEOBJ_H__
#define __OLEOBJ_H__

typedef struct
{
	PBOX m_Bound;
	PPNT m_OriginalSize;
	COleClientItem* m_pItem;
} OLEOBJECT_CREATE_STRUCT;

struct OleObjectRecord
{
	DWORD		m_dwItemNumber;	/* Serial number within this document. */
	PPNT		m_OriginalSize;
};

class OleObjectState : public RectPageObjectState
{
public:
	OleObjectRecord	m_ORecord;
};

FARCLASS OleObject : public RectPageObject
{
	INHERIT(OleObject, RectPageObject)
private:
	OleObjectRecord m_orecord;
	COleClientItem* m_pItem;

/*
// Private constructor.
// Call the create method.
*/

	OleObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

public:

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

/*
// Get the OLE client item for this object.
*/

	COleClientItem* Item(void) const;

/*
// Set the OLE client item for this object.
*/

	void Item(COleClientItem* pItem);

/*
// Get the OLE client item number for this object.
*/

	DWORD ItemNumber(void) const;

/*
// Set the object's original size.
*/

	void SetOriginalSize(const PPNT& Size);

/*
// Update an object into a redisplay context.
*/

	UpdateStatePtr update(RedisplayContextPtr rc, PBOX_PTR extent, LPRECT clip, UpdateStatePtr ustate, REFRESH_TYPE refresh_type);

/*
// Recalc after a size or movement.
*/

	virtual VOID calc(PBOX_PTR panel_world = NULL, FLAGS panel_flags = 0);

/*
// Destroy this object.
// The object is being removed from the database.
// Any and all sub-objects must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

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
// Get the original dimensions of an object.
// This is used primarily for proportional stuff.
// If the object has none, it should return FALSE.
*/

	virtual BOOL original_dims(PPNT_PTR p);

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
// Get the properties for this object.
*/

	virtual void GetObjectProperties(CObjectProperties& Properties);

/*
// Set the properties for this object.
*/

	virtual BOOL SetObjectProperties(CObjectProperties& Properties);

};

inline COleClientItem* OleObject::Item(void) const
	{ return m_pItem; }
inline void OleObject::Item(COleClientItem* pItem)
	{ m_pItem = pItem; m_orecord.m_dwItemNumber = pItem->m_dwItemNumber; }
inline DWORD OleObject::ItemNumber(void) const
	{ return m_orecord.m_dwItemNumber; }

#endif					// #ifndef __OLEOBJ_H__
