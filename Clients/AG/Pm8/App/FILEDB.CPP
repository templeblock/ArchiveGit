/*
// $Header: /PM8/App/FILEDB.CPP 1     3/03/99 6:05p Gbeddow $
//
// "File database" object routines.
// This file contains methods for "file database" objects
//
// $Log: /PM8/App/FILEDB.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:24   Fred
// Initial revision.
// 
//    Rev 1.2   08 Jan 1997 11:15:06   dennisp
// Disabled workspace as shareable by changing contructor flag from TRUE to FALSE.
// 
//    Rev 1.1   11 Apr 1996 12:11:24   FRED
// Copied from PMW2
// 
//    Rev 2.2   03 Apr 1996 09:05:34   FRED
// Flushes cache when full
// 
//    Rev 2.1   01 Aug 1995 12:59:06   JAY
// Only does read locks when allowable.
// 
//    Rev 2.0   07 Feb 1995 15:47:44   JAY
// Initial revision.
// 
//    Rev 1.5   30 Nov 1994 16:38:24   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.4   07 Jul 1994 11:26:44   JAY
// Fixed a near/far problems with NULL.
// 
//    Rev 1.3   20 Jun 1994 13:26:42   JAY
// Added override for reinitialize().
// Made set_maximum_size() physically try and reduce the file size if necessary.
// 
//    Rev 1.2   10 Jun 1994 09:59:58   JAY
// prep_file of EmbeddedFileRecord handles releasing itself in all cases.
// 
//    Rev 1.1   09 Jun 1994 11:11:28   JAY
// Main implementation.
// 
//    Rev 1.0   01 Jun 1994 16:44:20   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "filedb.h"

#include "fdirrec.h"
#include "flistrec.h"
#include "efilerec.h"

/*****************************************************************************/
/*                          File Database methods                            */
/*****************************************************************************/

FileDatabase::FileDatabase(StorageDevicePtr device) : Database(device, FALSE)
{
/* Record creation methods. */

	register_creation_method(FRT_FileDirectory, FileDirectoryRecord::create);
	register_creation_method(FRT_FileList, FileListRecord::create);
	register_creation_method(FRT_EmbeddedFile, EmbeddedFileRecord::create);

/* Max out our size for now. */

	m_maximum_size = POSITION_MAX;
}

/*
// initialize() [Our override for initialize()]
// 
// Initialize the database. The start of the database in the storage
// device is passed as a parameter.
// 
*/

ERRORCODE FileDatabase::initialize(ST_DEV_POSITION position)
{
	ERRORCODE error;

/*
// Do the regular init first.
*/

	if ((error = Database::initialize(position)) == ERRORCODE_None)
	{
		error = create_file_directory();
	}

	return error;
}

/*
// reinitialize()
*/

ERRORCODE FileDatabase::reinitialize(void)
{
	ERRORCODE error;

/*
// Do the regular reinit first.
*/

	if ((error = Database::reinitialize()) == ERRORCODE_None)
	{
		if (create_file_directory() != ERRORCODE_None)
		{
			initialized = FALSE;
			error = ERRORCODE_NotInitialized;
		}
	}

	return error;
}

/*
// Create the file directory.
*/

ERRORCODE FileDatabase::create_file_directory()
{
/*
// Create our file directory.
*/
	FileDirectoryPtr fdir;

/* Create the file directory. */

	if ((fdir = create_file_directory_record()) != NULL)
	{
		fdir->release();
	}
	else
	{
		return last_creation_error();
	}
	return ERRORCODE_None;
}

/*
// Find a file.
//
// The record number of the embedded file is returned in the variable pointed
// to by 'lprecord'.
*/

ERRORCODE FileDatabase::find_file(LPCSTR name, SHORT file_type, DB_RECORD_NUMBER far *lprecord)
{
	FileDirectoryPtr fdir;
	ERRORCODE error;

	if ((fdir = (FileDirectoryPtr)get_record(FILEDB_DIRECTORY_RECORD, &error, FRT_FileDirectory, TRUE)) != NULL)
	{
		error = fdir->find_file(name, file_type, lprecord);
		fdir->release_read_only();
	}
	return error;
}

/*
// Add a file (embed it).
//
// The record number of the embedded file is returned in the variable pointed
// to by 'lprecord' if 'lprecord' is not NULL.
*/

ERRORCODE FileDatabase::add_file(LPCSTR name, SHORT file_type, DB_RECORD_NUMBER far *lprecord)
{
	FileDirectoryPtr fdir;
	ERRORCODE error;

	if ((fdir = (FileDirectoryPtr)get_record(FILEDB_DIRECTORY_RECORD, &error, FRT_FileDirectory)) != NULL)
	{
		error = fdir->add_file(name, file_type, lprecord);
		fdir->release();
	}

	// New behavior:
	// When the cache gets full, we empty it entirely rather that trying
	// to search through it and remove the least recently used items.
	// Users reported that the program slowed down significantly once the
	// cache became full. The solution that seemed to work was to decrease
	// the cache size (which empties it.)
	if (error == ERRORCODE_Full)
	{
		// The database is full, empty it.
		if (reinitialize() == ERRORCODE_None)
		{
			// Attempt to add file to new (empty) database.
			if ((fdir = (FileDirectoryPtr)get_record(FILEDB_DIRECTORY_RECORD, &error, FRT_FileDirectory)) != NULL)
			{
				error = fdir->add_file(name, file_type, lprecord);
				fdir->release();
			}
		}
	}

	return error;
}

/*
// Prep a file (if the image is in the database).
//
// The file pointed to by 'file' will become based on the database device
// at the appropriate location if this is successful.
*/

ERRORCODE FileDatabase::prep_file(LPCSTR name, SHORT file_type, StorageFilePtr file)
{
	ERRORCODE error;
	DB_RECORD_NUMBER record;

/* Find the file. */

	if ((error = find_file(name, file_type, &record)) == ERRORCODE_None)
	{
	/*
	// Get the embedded file so we can base ourselves on it.
	// This is a "read-only" lock so we make it a 'const' pointer.
	*/

		EmbeddedFileCPtr efile;

		if ((efile = (EmbeddedFileCPtr)get_record(record, &error, FRT_EmbeddedFile, TRUE)) != NULL)
		{
		/* Prep the file using the embedded file method. */

			error = efile->prep_file(file, TRUE);
		}
	}
	return error;
}

/*
// Delete the oldest embedded file record in the database.
*/

ERRORCODE FileDatabase::delete_oldest_record(void)
{
#if 0
	FileDirectoryPtr fdir;
	ERRORCODE error;

	if ((fdir = (FileDirectoryPtr)get_record(FILEDB_DIRECTORY_RECORD, &error, FRT_FileDirectory)) != NULL)
	{
		error = fdir->delete_oldest_record();
		fdir->release();
	}
	return error;
#endif
	// New behavior:
	// When the cache gets full, we empty it entirely rather that trying
	// to search through it and remove the least recently used items.
	// Users reported that the program slowed down significantly once the
	// cache became full. The solution that seemed to work was to decrease
	// the cache size (which empties it.)
	return ERRORCODE_Full;
}

/*
// Set the maximum size for this database.
*/

ERRORCODE FileDatabase::set_maximum_size(ST_DEV_POSITION max_size, BOOL adjust_storage)
{
/*
// Remember the size that is requested.
*/

	m_maximum_size = max_size;

	if (adjust_storage)
	{
	/*
	// The size is set. Now try and make sure the file is less than the
	// maximum size.
	*/

		StorageDevicePtr device = get_storage_device();
		ERRORCODE error;

	/* Get the length of the device. */

		ST_DEV_POSITION length;
		if ((error = device->length(&length)) != ERRORCODE_None)
		{
			return error;
		}

	/* See if it needs to get smaller. */

		if (length < m_maximum_size)
		{
		/* The size is currently OK. */
			return ERRORCODE_None;
		}

		return reinitialize();
	}
	return ERRORCODE_None;
}
