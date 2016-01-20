/*
// $Header: /PM8/App/PATOBJ.H 1     3/03/99 6:08p Gbeddow $
//
// Pattern object definitions.
//
// $Log: /PM8/App/PATOBJ.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:43:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:34   FRED
// Initial revision.
// 
//    Rev 2.1   15 Feb 1995 17:03:08   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:50   JAY
// Initial revision.
// 
//    Rev 1.3   22 Jul 1994 16:28:38   JAY
// Added some missing access methods.
// 
//    Rev 1.2   01 Jun 1994 16:35:26   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.1   05 Jan 1994 08:08:14   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:42   JAY
//  
*/

#ifndef __PATOBJ_H__
#define __PATOBJ_H__

typedef struct
{
	PBOX bound;
	SHORT pattern;
	COLOR color;
	COLOR bcolor;
} PATTERN_CREATE_STRUCT, far *PATTERN_CREATE_STRUCT_PTR;

struct PatternObjectRecord
{
	SHORT		operation;		/* operation for updating */
	SHORT		pattern;			/* which pattern in the PatTable to use */
	COLOR		color;			/* object color */
	COLOR		bcolor;			/* object bcolor */
};

class PatternObjectState : public RectPageObjectState
{
public:
	PatternObjectRecord	m_PRecord;
};

FARCLASS PatternObject : public RectPageObject
{
	INHERIT(PatternObject, RectPageObject)
private:
	PatternObjectRecord precord;

/*
// Private constructor.
// Call the create method.
*/

	PatternObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

public:

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

/*
// Get the pattern for this object.
*/

	SHORT get_pattern(VOID);

/*
// Get the color for this object.
*/

	COLOR get_color(VOID);

/*
// Get the black color for this object.
*/

	COLOR get_bcolor(VOID);

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
};

inline SHORT PatternObject::get_pattern(VOID)
	{ return precord.pattern; }
inline COLOR PatternObject::get_color(VOID)
	{ return precord.color; }
inline COLOR PatternObject::get_bcolor(VOID)
	{ return precord.bcolor; }

#endif					// #ifndef __PATOBJ_H__
