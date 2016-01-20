/*
// $Header: /PM8/App/BACKOBJ.H 1     3/03/99 6:03p Gbeddow $
//
// Background object definitions.
//
// $Log: /PM8/App/BACKOBJ.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:42   Fred
// Initial revision.
// 
//    Rev 1.1   23 Jul 1996 13:14:00   Jay
// Backgrounds phased out -> convert to graphics
// 
//    Rev 1.0   14 Mar 1996 13:44:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:42   FRED
// Initial revision.
// 
//    Rev 2.1   15 Feb 1995 17:03:16   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:20   JAY
// Initial revision.
// 
//    Rev 1.4   01 Jun 1994 16:35:34   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.3   17 Feb 1994 09:25:14   JAY
// Added a RedisplayContext argument to get_refresh_bound() && is_opaque().
// 
//    Rev 1.2   13 Jan 1994 17:01:14   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:08:30   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:26:04   JAY
//  
*/

#ifndef __BACKOBJ_H__
#define __BACKOBJ_H__

FARCLASS BackgroundObject : public GraphicObject
{
	INHERIT(BackgroundObject, GraphicObject)
private:
/*
// Private constructor.
// Call the create method.
*/

	BackgroundObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner);

protected:

	BOOL shrink_to_fit(VOID);

public:

/*
// The creator for this type of object.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_OBJECT_TYPE type, ObjectDatabasePtr owner, VOIDPTR creation_data, PageObjectPtr far *object);

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
};

#endif					// #ifndef __BACKOBJ_H__
