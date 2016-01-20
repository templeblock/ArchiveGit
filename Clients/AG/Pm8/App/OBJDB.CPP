/*
// $Header: /PM8/App/OBJDB.CPP 1     3/03/99 6:08p Gbeddow $
//
// Object database routines.
// This file contains methods for an object database.
//
// $Log: /PM8/App/OBJDB.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:43:50   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:30   FRED
// Initial revision.
// 
//    Rev 2.1   15 Feb 1995 17:03:08   JAY
// Added states to objects.
// 
//    Rev 2.0   07 Feb 1995 15:47:56   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:33:14   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   04 Aug 1994 07:32:34   JAY
// Large model and 32-bit changes
// 
//    Rev 1.1   01 Jun 1994 16:38:38   JAY
// Moved another new_object() method here.
// Added 'type' parameter to object construct/create methods.
//
//   Rev 1.0   16 Dec 1993 16:24:54   JAY
// 
*/

#include "stdafx.h"

#include <string.h>

#include "objdb.h"

/*****************************************************************************/
/*                          Object Database methods                          */
/*****************************************************************************/

ObjectDatabase::ObjectDatabase(StorageDevicePtr device) : Database(device)
{
/* Wipe the creator methods clean. */

	memset(object_creation_method, 0, sizeof(object_creation_method));
}

/*
// Register a creation method with the database.
*/

ERRORCODE ObjectDatabase::register_object_creation_method(DB_OBJECT_TYPE type, PAGE_OBJECT_CREATOR method)
{
/* Validate the type. */

	if (type < 0 || type >= OBJECT_TYPES_PER_DATABASE)
	{
		return ERRORCODE_IllegalType;
	}

/* Do the register. */

	object_creation_method[type] = method;

	return ERRORCODE_None;
}

/*
// Create a new page object.
*/

ERRORCODE ObjectDatabase::new_object(DB_OBJECT_TYPE type, VOIDPTR creation_data, PageObjectPtr far *record)
{
/* Validate the type. */

	if (type < 0 || type >= OBJECT_TYPES_PER_DATABASE)
	{
		return ERRORCODE_IllegalType;
	}

/* See if a creation method has been registered. */

	PAGE_OBJECT_CREATOR creator;

	if ((creator = object_creation_method[type]) == NULL)
	{
		return ERRORCODE_NotRegistered;
	}

/* Invoke the creator. */

	return creator(type, this, creation_data, record);
}

/*
// Another way to create an object.
*/

PageObjectPtr ObjectDatabase::new_object(DB_OBJECT_TYPE type, VOIDPTR creation_data)
{
	PageObjectPtr object;

	if ((m_last_error = new_object(type, creation_data, &object)) != ERRORCODE_None)
	{
		object = NULL;
	}

	return object;
}

/*
// Duplicate an object.
*/

PageObjectPtr PageObject::duplicate(ObjectDatabasePtr dup_database, LPERRORCODE lperror)
{
	PageObjectPtr object;
	ERRORCODE error;

	if (dup_database == NULL)
	{
		dup_database = database;
	}

	if ((error = dup_database->new_object(my_type, NULL, &object)) == ERRORCODE_None)
	{
		error = object->assign(*this);
	}
	if (lperror != NULL)
	{
		*lperror = error;
	}
	return object;
}

/*
// The base object assignment method.
*/

ERRORCODE PageObject::assign(PageObjectRef sobject)
{
// Nothing to do. Sorry to disappoint you!
	return ERRORCODE_None;
}

PageObjectState* PageObject::CreateState(void)
{
	return new PageObjectState;
}

void PageObject::SaveState(PageObjectState* pState)
{
// Nothing to save!
}

void PageObject::RestoreState(PageObjectState* pState)
{
// Nothing to restore!
}
