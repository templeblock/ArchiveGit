/*
// $Header: /PM8/App/FDIRREC.CPP 1     3/03/99 6:05p Gbeddow $
//
// File directory record routines.
//
// $Log: /PM8/App/FDIRREC.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:47:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:26   FRED
// Initial revision.
// 
//    Rev 2.1   01 Aug 1995 12:59:06   JAY
// Only does read locks when allowable.
// 
//    Rev 2.0   07 Feb 1995 15:47:42   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:38:36   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   07 Jul 1994 11:26:24   JAY
// Fixed a near/far problems with NULL.
// 
//    Rev 1.1   20 Jun 1994 13:24:52   JAY
// Misc. debug messages.
// 
//    Rev 1.0   09 Jun 1994 11:13:06   JAY
// Initial revision.
*/

#include "stdafx.h"

#include "filedb.h"

#include "fdirrec.h"
#include "flistrec.h"

/*****************************************************************************/
/*                          FileDirectory record                             */
/*****************************************************************************/

/*
// The creator for a file list record.
*/

ERRORCODE FileDirectoryRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	FileDirectoryPtr file_dir;

/* Create the new file directory record. */

	if ((file_dir = new FileDirectoryRecord(number, type, owner, where)) == NULL)
	{
		return ERRORCODE_Memory;
	}

	*record = file_dir;

	return ERRORCODE_None;
}

/*
// The constructor for a file list record.
*/

FileDirectoryRecord::FileDirectoryRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
}

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

VOID FileDirectoryRecord::destroy(VOID)
{
	SHORT count = m_list_array.count();

	if (count != 0)
	{
		FILE_LIST_ELEMENT_PTR element = m_list_array.get_list(0);

		while (count-- != 0)
		{
			database->delete_record(element->record_number, FRT_FileList);
			element++;
		}
	}
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE FileDirectoryRecord::ReadData(StorageDevicePtr device)
{
	return m_list_array.read(device);
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE FileDirectoryRecord::WriteData(StorageDevicePtr device)
{
	return m_list_array.write(device);
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE FileDirectoryRecord::SizeofData(StorageDevicePtr device)
{
	return m_list_array.size(device);
}

/*
// Find a type in the list.
*/

FILE_LIST_ELEMENT_PTR FileDirectoryRecord::find_type(SHORT type)
{
	SHORT count;

	if ((count = m_list_array.count()) != 0)
	{
		FILE_LIST_ELEMENT_PTR element = m_list_array.get_list(0);

		while (count-- != 0)
		{
			if (element->type == type)
			{
				return element;
			}
			element++;
		}
	}
	return NULL;
}

/*
// Add a type to the list.
// If the type already exists, it is returned. Otherwise, a new list is
// created in the database. If creation fails, NULL is returned.
*/

ERRORCODE FileDirectoryRecord::add_type(SHORT type, DB_RECORD_NUMBER far *lprecord)
{
	ERRORCODE error;

/*
// See if the element already exists.
*/
	
	FILE_LIST_ELEMENT_PTR lpelem;

	if ((lpelem = find_type(type)) != NULL)
	{
	/* It already exists. Everybody's happy. */

		if (lprecord != NULL)
		{
			*lprecord = lpelem->record_number;
		}
		return ERRORCODE_None;
	}

/*
// We need to add it.
// First, create the file list record.
*/

	FileDatabasePtr filedb = (FileDatabasePtr)database;

	FileListPtr flist;

	if ((flist = filedb->create_file_list_record()) == NULL)
	{
		return filedb->last_creation_error();
	}

/*
// We got a new list record. Build the element.
*/

	FILE_LIST_ELEMENT new_element;

	new_element.type = type;
	new_element.record_number = flist->Id();

	flist->release();

/*
// Now add the element to the list array.
*/

	if ((error = m_list_array.add_element(&new_element)) != ERRORCODE_None)
	{
	/* Get rid of the new list. */
		filedb->delete_record(new_element.record_number, FRT_FileList);
	}
	else
	{
	/* Return the element. */

		if ((lpelem = m_list_array.get_list(m_list_array.count()-1)) == NULL)
		{
		/* The universe is inconsistent. */
			error = ERRORCODE_IntError;
		}
		else
		{
			if (lprecord != NULL)
			{
				*lprecord = lpelem->record_number;
			}
		}

	/* We're now modified. */

		modified();
	}

	return error;
}

/*
// Find a file.
//
// The record number of the embedded file is returned in the variable pointed
// to by 'lprecord'.
*/

ERRORCODE FileDirectoryRecord::find_file(LPCSTR name, SHORT file_type, DB_RECORD_NUMBER far *lprecord)
{
	FILE_LIST_ELEMENT_PTR list_element;

/* Find the type. */

	if ((list_element = find_type(file_type)) == NULL)
	{
		return ERRORCODE_DoesNotExist;
	}

/* Lock the list to search it. */

	ERRORCODE error;
	FileListPtr flist;

	if ((flist = (FileListPtr)database->get_record(list_element->record_number,
																  &error,
																  FRT_FileList,
																  TRUE)) == NULL)
	{
		return error;
	}

/* Search the list. */

	error = flist->find_file(name, lprecord);

/* Release the list. */

	flist->release_read_only();

/* Return whether we were succesful or not. */

	return error;
}

/*
// Add a file (embed it).
//
// The record number of the embedded file is returned in the variable pointed
// to by 'lprecord' if 'lprecord' is not NULL.
*/

ERRORCODE FileDirectoryRecord::add_file(LPCSTR name, SHORT file_type, DB_RECORD_NUMBER far *lprecord)
{
	ERRORCODE error;
	DB_RECORD_NUMBER list_rec;

/* Make sure the type is available. */

	if ((error = add_type(file_type, &list_rec)) != ERRORCODE_None)
	{
		return error;
	}

/*
// Lock the file list so we can add the file.
*/

	FileListPtr flist;

	if ((flist = (FileListPtr)database->get_record(list_rec,
																  &error,
																  FRT_FileList)) == NULL)
	{
		return error;
	}

/* Add the file to the list. */

	error = flist->add_file(name, lprecord);

/* Release the list. */

	flist->release();

/* Return whether we were successful or not. */

	return error;
}

/*
// Delete the oldest embedded file record in the database.
*/

ERRORCODE FileDirectoryRecord::delete_oldest_record(void)
{
/* Find the oldest record in the lists. */
	ERRORCODE error;
	SHORT oldest_type;
	SHORT oldest_index;
	DWORD oldest_time = (DWORD)(-1L);

/* Search through all types. */

	for (SHORT type = 0; type < m_list_array.count(); type++)
	{
		FILE_LIST_ELEMENT_PTR plist;

		if ((plist = m_list_array.get_list(type)) != NULL)
		{
			FileListPtr flist;

			if ((flist = (FileListPtr)database->get_record(plist->record_number,
																  NULL,
																  FRT_FileList)) == NULL)
			{
				continue;
			}

			FILE_ARRAY_ELEMENT_PTR pfile;
			SHORT index;

			if ((pfile = flist->get_oldest_file(&index)) != NULL)
			{
				if (pfile->last_access < oldest_time)
				{
					oldest_time = pfile->last_access;
					oldest_type = type;
					oldest_index = index;
				}
			}

			flist->release();
		}
	}

/*
// See if we found anything to delete.
*/

	if (oldest_time == (DWORD)(-1L))
	{
		return ERRORCODE_DoesNotExist;
	}

/*
// We now have the record we want to delete.
*/

	FILE_LIST_ELEMENT_PTR plist;

	od("Delete file %d of type	index %d ", oldest_index, oldest_type);

	if ((plist = m_list_array.get_list(oldest_type)) == NULL)
	{
		return ERRORCODE_IntError;
	}
	else
	{
	/* Get the record so we can delete the file. */

		FileListPtr flist;

		if ((flist = (FileListPtr)database->get_record(plist->record_number,
															  &error,
															  FRT_FileList)) != NULL)
		{
			error = flist->delete_file(oldest_index);
		/* We may someday want to delete the type if the count goes to 0. */
			flist->release();
		}
	}
	od("\r\n");
	return error;
}
