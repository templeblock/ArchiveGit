/*
// $Header: /PM8/App/OBJDB.H 1     3/03/99 6:08p Gbeddow $
//
// Definitions for a "page object" database.
//
// $Log: /PM8/App/OBJDB.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 3     3/20/98 6:03p Jayn
// Serif Drawing Object
// 
// 2     3/04/98 2:49p Johno
// incremented OBJECT_TYPES_PER_DATABASE to accommodate new
// PMG_OBJECT_TYPE object type
// 
//    Rev 1.0   14 Aug 1997 15:23:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:43:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:30   FRED
// Initial revision.
// 
//    Rev 2.1   15 Feb 1995 17:03:08   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:48:46   JAY
// Initial revision.
// 
//    Rev 1.1   01 Jun 1994 16:38:00   JAY
// Moved another new_object() method here.
// Added 'type' parameter to object construct/create methods.
// 
//    Rev 1.0   16 Dec 1993 16:25:42   JAY
//  
*/

#ifndef __OBJDB_H__
#define __OBJDB_H__

#include "database.h"
#include "list.h"

/*
// An object type.
// This is subject to the same restrictions as database record types
// (e.g. 0 is invalid)
*/

typedef SHORT DB_OBJECT_TYPE;

/*
// A pointer to an object database.
*/

typedef class ObjectDatabase far *ObjectDatabasePtr;

/*
// A pointer to a page object.
*/

typedef class PageObject far *PageObjectPtr;
typedef class PageObject far &PageObjectRef;

/*
// The state of an object.
*/

class PageObjectState
{
public:
	virtual ~PageObjectState() {}
};

/*
// A page object.
*/

FARCLASS PageObject : public ListElement
{
protected:
/*
// What type of object is this?
*/

	DB_OBJECT_TYPE		my_type;

/*
// A pointer to the database owning this object.
// This allows an object to manipulate records for its use.
*/

	ObjectDatabasePtr	database;

public:

	PageObject(DB_OBJECT_TYPE type, ObjectDatabasePtr owner) : my_type(type), database(owner) {}
	virtual ~PageObject() {}

/*
// previous_object()
//
// Return the previous object in the list.
*/

	PageObjectPtr previous_object() { return (PageObjectPtr)previous_element(); }

/*
// next_object()
//
// Return the next object in the list.
*/

	PageObjectPtr next_object() { return (PageObjectPtr)next_element(); }

/*
// read()
//
// This is a method defined by a class derived from this one.
// It handles the reading of any data which is specific to the derived class.
// This routine is usually called by the ObjectDatabase when reading an object
// list.
*/

	virtual	ERRORCODE read(StorageDevicePtr device) { return ERRORCODE_None; }

/*
// write()
//
// This is a method defined by a class derived from this one.
// It handles the writing of any data which is specific to the derived class.
// This routine is usually called by the ObjectDatabase when writing an object
// list.
*/

	virtual	ERRORCODE write(StorageDevicePtr device) { return ERRORCODE_None; }

/*
// size()
//
// This is a method defined by a class derived from this one.
// It returns the size of any data which is specific to the derived class.
// This routine is usually called by the ObjectDatabase when writing an object
// list.
//
// A pointer to the device is passed to allow access to the device's
// sizing routines.
*/

	virtual	ST_MAN_SIZE size(StorageDevicePtr device) { return 0; }

/*
// destroy()
//
// This routine is called when an object is being removed from a document.
// It should NOT free the object (that is done in the destructor) but should
// rather destroy any records or other objects it has direct control over and
// which are only associated with this object.
*/

	virtual VOID destroy() {}

/*
// type()
//
// Return the type of object this is.
*/

	DB_OBJECT_TYPE	type() const { return my_type; }

/*
// Duplicate this object.
// This has a standard default which calls the assign method.
*/

	virtual PageObjectPtr duplicate(ObjectDatabasePtr dup_database = NULL, LPERRORCODE lperror = NULL);

/*
// Assign method.
// This needs to duplicate all database references.
// This could ALMOST have been the assignment operator. Oh, well...
*/

	virtual ERRORCODE assign(PageObjectRef obj);

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

/*
// A PageObject creation method.
*/

typedef ERRORCODE (*PAGE_OBJECT_CREATOR)(DB_OBJECT_TYPE, ObjectDatabasePtr, VOIDPTR, PageObjectPtr far *);

#define	OBJECT_TYPES_PER_DATABASE	30

/*
// A page object database.
*/

FARCLASS ObjectDatabase : public Database
{
private:
/*
// The page object creators for this object database.
// Theoretically, each database could have its own different set of creation
// methods. This may be useful in the future.
//
// You set a creation method by calling register_object_creation_method().
*/

	PAGE_OBJECT_CREATOR		object_creation_method[OBJECT_TYPES_PER_DATABASE];

public:

	ObjectDatabase(StorageDevicePtr device);

/**************************/
/* Page Object Management */
/**************************/

/*
// register_object_creation_method()
//
// This will register a routine to create a page object of a certain type with
// the database so that the database will know who to call when a object
// needs creation.
*/

	ERRORCODE register_object_creation_method(DB_OBJECT_TYPE type, PAGE_OBJECT_CREATOR method);

/*
// new_object()
//
// Create a new object of the type passed.
// This will call the appropriate creation method to actually create
// the object. There must be an object creation method registered for this
// type. "creation_data" is specific to the type of object being created.
*/

	ERRORCODE new_object(DB_OBJECT_TYPE type, VOIDPTR creation_data, PageObjectPtr far *record);
	PageObjectPtr new_object(DB_OBJECT_TYPE type, VOIDPTR creation_data);

};

#endif						/* #ifndef __OBJDB_H__ */
