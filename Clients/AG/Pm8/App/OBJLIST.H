/*
// $Header: /PM8/App/OBJLIST.H 1     3/03/99 6:08p Gbeddow $
//
// Definitions for an object list.
//
// $Log: /PM8/App/OBJLIST.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 5     2/26/99 3:42p Johno
// Added CopyGuides() to copy guide information from another list
// 
// 4     2/26/99 2:37p Johno
// Added SetPageProperties() for copying page properties with other
// objects when a page copy is added to a document
// 
// 3     2/17/98 11:42a Dennis
// Added Page Properties object
// 
// 2     11/14/97 11:38a Hforman
// added guide arrays to list
// 
//    Rev 1.0   14 Aug 1997 15:23:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:45:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:06   FRED
// Initial revision.
// 
//    Rev 2.1   18 Aug 1995 15:06:02   JAY
// Photo CD; graphic images, etc.
// 
//    Rev 2.0   07 Feb 1995 15:48:46   JAY
// Initial revision.
// 
//    Rev 1.0   16 Dec 1993 16:25:48   JAY
//  
*/

#ifndef __OBJLIST_H__
#define __OBJLIST_H__

#include "objdb.h"
#include "pageprop.h"

/*
// An object list.
// This is basically just a list, but I wanted it to exist just in case.
*/

typedef class ObjectList far &ObjectListRef;

class ObjectList : public List
{
protected:
	ObjectDatabasePtr database;

/*
// The information written preceding an object entry in an object list
// when written to a device.
*/

   typedef struct
   {
	   DB_OBJECT_TYPE		type;
	   ST_MAN_SIZE			size;
   } OBJECT_HEADER;

   // DWORD arrays of guide positions
   Array            m_HorizGuides;   // y positions of horizontal guides
   Array            m_VertGuides;    // x positions of vertical guides

   CPageProperties   m_PageProperties;

public:
/*
// The constructor for an object list.
*/

	ObjectList(ObjectDatabasePtr owner);

/*
// Assignment operator.
*/

	ERRORCODE assign(const ObjectList far &s);
	ObjectListRef operator = (const ObjectList far &s)
		{ assign(s); return *this; }

/*
// get_database()
//
// Return the database this object list is based on.
*/

	ObjectDatabasePtr get_database(VOID)
		{ return database; }

/*
// first_object()
//
// Return the first object in the list.
*/

	PageObjectPtr first_object() { return (PageObjectPtr)first_element(); }

/*
// last_object()
//
// Return the last object in the list.
*/

	PageObjectPtr last_object() { return (PageObjectPtr)last_element(); }

/*
// destroy_objects()
//
// Destroy all objects in the list.
*/

	void destroy_objects(void);

/*
// Get Guide Arrays
*/
   Array*   GetHorizGuides()  { return &m_HorizGuides; }
   Array*   GetVertGuides()   { return &m_VertGuides; }

	void
	CopyGuides(ObjectList *p)
	{
		m_HorizGuides.assign(*p->GetHorizGuides());	
		m_VertGuides.assign(*p->GetVertGuides());	
	}
/*
// Get Page Properties
*/
   CPageProperties * GetPageProperties() 
   { 
		return &m_PageProperties; 
	}

	void SetPageProperties(CPageProperties *p)
	{
		m_PageProperties.CopyOf(p);
	}
/*
// read()
//
// Read an object list from a StorageDevice.
*/

	ERRORCODE read(StorageDevicePtr device);

/*
// write()
//
// Write an object list to a StorageDevice.
*/

	ERRORCODE write(StorageDevicePtr device);

/*
// size()
//
// Size an object list for a StorageDevice.
*/

	ST_MAN_SIZE size(StorageDevicePtr device);

};

typedef ObjectList far *ObjectListPtr;

#endif						/* #ifndef __OBJLIST_H__ */

