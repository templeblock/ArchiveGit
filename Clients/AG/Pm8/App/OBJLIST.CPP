/*
// $Header: /PM8/App/OBJLIST.CPP 1     3/03/99 6:08p Gbeddow $
//
// Object list routines.
//
// $Log: /PM8/App/OBJLIST.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 4     2/17/98 11:42a Dennis
// Added Page Properties object
// 
// 3     2/10/98 5:18p Hforman
// modified read() so it just skips an object if there's an error
// 
// 2     11/14/97 11:38a Hforman
// added guide arrays to list
// 
//    Rev 1.0   14 Aug 1997 15:23:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:44:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:40   FRED
// Initial revision.
// 
//    Rev 2.1   18 Aug 1995 15:06:02   JAY
// Photo CD; graphic images, etc.
// 
//    Rev 2.0   07 Feb 1995 15:47:58   JAY
// Initial revision.
// 
//    Rev 1.2   30 Nov 1994 16:37:20   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.1   31 Jan 1994 17:42:42   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:24:54   JAY
// 
*/

#include "stdafx.h"

#include <string.h>

#include "objlist.h"
#include "pmgobj.h"

ObjectList::ObjectList(ObjectDatabasePtr owner)
       : List(0, TRUE), database(owner), m_HorizGuides(sizeof(DWORD)), m_VertGuides(sizeof(DWORD))
{
}


/*
// destroy_objects()
//
// Destroy all objects in the list.
*/

void ObjectList::destroy_objects(void)
{
	PageObjectPtr object;

/* Do all the objects in the list. */

	for (object = first_object();
							object != NULL;
							object = object->next_object())
	{
		object->destroy();
	}

	empty();
}

/*
// Read a list of objects.
*/

ERRORCODE ObjectList::read(StorageDevicePtr device)
{
/* Make sure we're empty. */

	empty();

/* Read all objects. */

	for (;;)
	{
		ERRORCODE error;
		OBJECT_HEADER header;
		PageObjectPtr object;

	/* Read the next object header. */

		if ((error = device->read(&header, sizeof(header))) != ERRORCODE_None)
		{
			return error;
		}

	/* See if this is the terminating header. */

		if (header.type == DB_NO_TYPE)
		{
		/* All done! */
			return ERRORCODE_None;
		}

		if (header.type < 0)
		{
			return ERRORCODE_IllegalType;
		}

	/* See if we can create an object of this type. */

      if (header.type == OBJECT_TYPE_Guides)
      {
         // Read the guides.
         if ((error = m_HorizGuides.read(device)) != ERRORCODE_None)
            return error;
         if ((error = m_VertGuides.read(device)) != ERRORCODE_None)
            return error;
      }
      else if(header.type == OBJECT_TYPE_PageBackground)
         {
         if ( (error = device->read_record(&m_PageProperties, sizeof(m_PageProperties))) != ERRORCODE_None)
            return error;
         }
      else
      {
         // Normal object
         ST_DEV_POSITION read_pos;
		   device->tell(&read_pos);
			error = database->new_object(header.type, NULL, &object);
		   device->seek(read_pos, ST_DEV_SEEK_SET);

		   switch (error)
		   {
			   case ERRORCODE_None:
			   {
				   // Read the object.
					ST_DEV_POSITION curPos;
					device->tell(&curPos);
					if ((error = object->read(device)) != ERRORCODE_None)
				   {
					   device->seek(header.size + curPos, ST_DEV_SEEK_SET);
						delete object;
				   }
					else
					{
					   // Add the object to the list.
					   append(object);
					}
				   break;
			   }
			   default:
			   {
				   // Skip this one. We don't know what it is.
				   device->seek(header.size, ST_DEV_SEEK_CUR);
				   break;
			   }
		   }
	   }
   }
}

/*
// Write a list of objects.
*/

ERRORCODE ObjectList::write(StorageDevicePtr device)
{
/* Write all objects. */

	PageObjectPtr object;
	OBJECT_HEADER header;
	ERRORCODE error;

   // Write the normal objects.
   for (object = first_object(); object != NULL; object = object->next_object())
	{                                   

	/* Build the header. */

		header.type = object->type();
		header.size = object->size(device);

	/* Write the object header. */

		if ((error = device->write(&header, sizeof(header))) != ERRORCODE_None
				|| (error = object->write(device)) != ERRORCODE_None)
		{
			return error;
		}
	}

   // Write the guides.
	header.type = OBJECT_TYPE_Guides;
	header.size = m_HorizGuides.size(device)+m_VertGuides.size(device);
	if ((error = device->write(&header, sizeof(header))) != ERRORCODE_None
			|| (error = m_HorizGuides.write(device)) != ERRORCODE_None
			|| (error = m_VertGuides.write(device)) != ERRORCODE_None)
	{
		return error;
	}

   // Write the Page Background Object
	header.type = OBJECT_TYPE_PageBackground;
	header.size = device->size_record(sizeof(m_PageProperties));
	if ((error = device->write(&header, sizeof(header))) != ERRORCODE_None ||
       (error = device->write_record(&m_PageProperties, sizeof(m_PageProperties))) != ERRORCODE_None)
	{
		return error;
	}

   // Write the terminating header.

	header.type = DB_NO_TYPE;
	header.size = 0;

	return device->write(&header, sizeof(header));
}

/*
// Size a list of objects.
*/

ST_MAN_SIZE ObjectList::size(StorageDevicePtr device)
{
	PageObjectPtr object;
	ST_MAN_SIZE total_size = 0;

   // Size all objects (count the header we write, too).
	for (object = first_object(); object != NULL; object = object->next_object())
	{
		total_size += sizeof(OBJECT_HEADER) + object->size(device);
	}

   // Size the guides.
   total_size += sizeof(OBJECT_HEADER) + m_HorizGuides.size(device) + m_VertGuides.size(device);

   // Size of the Page Background.
   total_size += sizeof(OBJECT_HEADER) + device->size_record(sizeof(m_PageProperties));

   // Add the size of the terminating header.
	total_size += sizeof(OBJECT_HEADER);

	return total_size;
}

/*
// Assign one list to another.
// Care must be taken to manage database transfers.
//
// Since these are both object lists, there is no need to copy the List
// portion of these guys (since they are already identical).
*/

ERRORCODE ObjectList::assign(const ObjectList far &ol)
{
	ERRORCODE error = ERRORCODE_None;

/* Empty the destination in case it's not. */

	empty();

/* Copy all elements over. */

	for (PageObjectPtr object = ((ObjectListRef)ol).first_object(); object != NULL; object = object->next_object())
	{
		PageObjectPtr new_object;

		if ((new_object = object->duplicate(database, &error)) == NULL)
		{
			break;
		}

	/* Append the new object. */

		append(new_object);
	}

   // Assign the guides.
   m_HorizGuides.assign(ol.m_HorizGuides);
   m_VertGuides.assign(ol.m_VertGuides);

	return error;
}

