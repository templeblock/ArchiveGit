/*
// $Header: /PM8/App/FLISTREC.CPP 1     3/03/99 6:05p Gbeddow $
//
// File list record routines.
//
// $Log: /PM8/App/FLISTREC.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:42:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:08   FRED
// Initial revision.
// 
//    Rev 2.1   01 Aug 1995 12:59:06   JAY
// Only does read locks when allowable.
// 
//    Rev 2.0   07 Feb 1995 15:47:46   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:38:18   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   04 Aug 1994 07:34:04   JAY
// Large model and 32-bit changes
// 
//    Rev 1.1   07 Jul 1994 11:26:22   JAY
// Fixed a near/far problems with NULL.
// 
//    Rev 1.0   09 Jun 1994 11:12:44   JAY
// Initial revision.
*/

#include "stdafx.h"

#include "filedb.h"

#include "flistrec.h"
#include "efilerec.h"

#include <stdlib.h>					/* For bsearch(). */
#include <string.h>
#include <time.h>

/*****************************************************************************/
/*                          FileList record                                  */
/*****************************************************************************/

/*
// The creator for a file list record.
*/

ERRORCODE FileListRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	FileListPtr file_list;

/* Create the new file list record. */

	if ((file_list = new FileListRecord(number, type, owner, where)) == NULL)
	{
		return ERRORCODE_Memory;
	}

	*record = file_list;

	return ERRORCODE_None;
}

/*
// The constructor for a file list record.
*/

FileListRecord::FileListRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
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

VOID FileListRecord::destroy(VOID)
{
	SHORT count = m_file_array.count();

	if (count != 0)
	{
		FILE_ARRAY_ELEMENT_PTR element = m_file_array.get_file(0);

		while (count-- != 0)
		{
			database->delete_record(element->record_number, FRT_EmbeddedFile);
			element++;
		}
	}
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE FileListRecord::ReadData(StorageDevicePtr device)
{
	return m_file_array.read(device);
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE FileListRecord::WriteData(StorageDevicePtr device)
{
	return m_file_array.write(device);
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE FileListRecord::SizeofData(StorageDevicePtr device)
{
	return m_file_array.size(device);
}

/*
// Generate a hash value for a name.
*/

PRIVATE WORD near
hash_name(LPCSTR s)
{
	WORD hash = 0;

	while (*s != '\0')
	{
		WORD carry;

		carry = ((hash & 0x8000L) == 0) ? 1 : 0;
		hash = (hash << 1) + *s++ + carry;
	}
	return hash;
}

/*
// The compare function for use with bsearch and the related global vars.
*/

int FileListRecord::file_element_compare(PCVOID key, PCVOID elem2)
{
	FILE_SEARCH_KEY *skey = (FILE_SEARCH_KEY *)key;
	FILE_ARRAY_ELEMENT_PTR selem2 = (FILE_ARRAY_ELEMENT_PTR)elem2;

/* Remember the last element we touched. */

	skey->last_searched_element = selem2;

	WORD hash = skey->search_hash;
	int result;

//	od("compare key %x to %x...", hash, selem2->hash);

	if (selem2->hash == hash)
	{
	/*
	// The hash values match. We need to read this entry so we can see if
	// the name matches.
	*/

		ERRORCODE error;
		EmbeddedFileCPtr efile;

		if ((efile = (EmbeddedFileCPtr)skey->this_record->database->get_record(selem2->record_number, &error, FRT_EmbeddedFile, TRUE)) == NULL)
		{
		/* Error is special case. 'last_result' gets error number. */
			skey->last_result = error;
			return 0;						/* And stop searching. */
		}

	/* Generate the return value. */

//		od("compare '%s' to '%s'", skey->search_name, efile->get_file_name());
		result = strcmp(skey->search_name, efile->get_file_name());

	/* Release the record. */

		efile->release_read_only();
	}
	else if (hash < selem2->hash)
	{
		result = -1;
	}
	else
	{
		result = 1;
	}
//	od("Result: %d\r\n:", result);
	return skey->last_result = result;
}

/*
// Peform a file find.
// 'key' is filled in with info about the search (useful in add_file).
*/

ERRORCODE FileListRecord::do_find_file(LPCSTR name, DB_RECORD_NUMBER far *lprecord, FILE_SEARCH_KEY *key)
{
#if 0
	od("Dump: ");
	for (int i = 0; i < m_file_array.count(); i++)
	{
		FILE_ARRAY_ELEMENT_PTR elem = m_file_array.get_file(i);
		od("(h:%x, r:%ld)", elem->hash, elem->record_number);
	}
	od("\r\n");
#endif
/*
// We want to perform a binary search on the array.
// The search key will be the hash value.
*/

	key->this_record = this;
	key->search_name = name;
	key->search_hash = hash_name(name);
	key->last_searched_element = NULL;

//	od("Look for file name '%s' (hash: %x)\r\n", name, key->search_hash);

	SHORT count;

	if ((count = m_file_array.count()) != 0)
	{
	/*
	// Get the first entry.
	// All entries in the array have the same selector.
	*/
		FILE_ARRAY_ELEMENT_PTR elem = m_file_array.get_file(0);

	/* Do the search. */

		void *ret = bsearch(key, elem, count,
								  sizeof(FILE_ARRAY_ELEMENT),
								  file_element_compare);

	/* Did we find it? */

		if (ret != NULL)
		{
			if (key->last_result != 0)
			{
			/* This is actually an error. */
				return (ERRORCODE)key->last_result;
			}

		/* Found it! */

			elem = (FILE_ARRAY_ELEMENT_PTR)ret;

		/* Update our time stamp. */

			elem->last_access = (DWORD)time(NULL);

			if (lprecord != NULL)
			{
				*lprecord = elem->record_number;
			}

			return ERRORCODE_None;
		}
	}

/* Not found. */

	return ERRORCODE_DoesNotExist;
}

/*
// Find a file.
//
// The record number of the embedded file is returned in the variable pointed
// to by 'lprecord'.
*/

ERRORCODE FileListRecord::find_file(LPCSTR name, DB_RECORD_NUMBER far *lprecord)
{
	FILE_SEARCH_KEY key;

	return do_find_file(name, lprecord, &key);
}

/*
// Add a file (embed it).
// If the file is already in the list, it is merely returned without error.
//
// The record number of the embedded file is returned in the variable pointed
// to by 'lprecord' if 'lprecord' is not NULL.
*/

ERRORCODE FileListRecord::add_file(LPCSTR name, DB_RECORD_NUMBER far *lprecord)
{
	ERRORCODE error;
	FILE_SEARCH_KEY key;

//	od("Add file: '%s'\r\n", name);

/*
// See if it already exists.
// The return value is either "None", "DoesNotExist" or some other error.
// Pass any value other than "DoesNotExist" back to the caller.
*/

	if ((error = do_find_file(name, lprecord, &key)) != ERRORCODE_DoesNotExist)
	{
		return error;
	}

/*
// The file does not exist, so we need to add it.
// Create the new embedded file record.
*/

	FileDatabasePtr filedb = (FileDatabasePtr)database;
	ST_DEV_POSITION max_size = filedb->get_maximum_size();
	StorageManagerPtr stman = filedb->get_storage_manager();
	BOOL deleted_record = FALSE;

	EmbeddedFilePtr efile;

	for (;;)
	{
	/* Enforce the size limit only while embedding a file. */

		stman->SetSizeLimit(max_size);
		efile = filedb->create_embedded_file_record((LPVOID)name);
		stman->SetSizeLimit();

		if (efile != NULL)
		{
		/* We got the file. */
			break;
		}

	/*
	// Get the last error to see what went wrong.
	*/

		if ((error = filedb->last_creation_error()) != ERRORCODE_Full)
		{
		/* Some other bogus error. */
			return error;
		}

	/*
	// The database has gotten full.
	// Try to kick out the oldest record to make room.
	*/

		if (filedb->delete_oldest_record() != ERRORCODE_None)
		{
		/* We're just full, that's all. */
			return error;
		}
	/* We have deleted a record. This invalidates 'key'. */
		deleted_record = TRUE;
	/* Loop back and try again... */
	}

/*
// Build the element.
*/

	FILE_ARRAY_ELEMENT new_element;

	new_element.record_number = efile->Id();
	new_element.hash = key.search_hash;				/* This should be ours. */
	new_element.last_access = (DWORD)time(NULL);

	efile->release();

/*
// Now add the element to the list array.
*/

	if (deleted_record)
	{
	/* A record was deleted. This may have invalidated 'key'. */
		if ((error = do_find_file(name, lprecord, &key)) != ERRORCODE_DoesNotExist)
		{
//			efile->release();
			database->delete_record(new_element.record_number, FRT_EmbeddedFile);
			return error;
		}
	}

/*
// Now, 'key' holds information about the last search so we can
// find exactly where to insert.
*/

	SHORT index = 0;

	if (key.last_searched_element != NULL)
	{
	/* Compute the index of the last searched element. */

		index = key.last_searched_element - m_file_array.get_file(0);

		if (key.last_result > 0)
		{
		/*
		// Insert after the last searched element since the last searched
		// element compares before this element in the sorting scheme.
		*/
			index++;
		}
	}

//	od("Insert element at index %d\r\n", index);

	if ((error = m_file_array.insert_element(&new_element, index)) != ERRORCODE_None)
	{
	/* Get rid of the new list. */
		database->delete_record(new_element.record_number, FRT_EmbeddedFile);
	}
	else
	{
	/* Return the element. */

		FILE_ARRAY_ELEMENT_PTR lpelem;

		if ((lpelem = m_file_array.get_file(m_file_array.count()-1)) == NULL)
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
// Find the oldest file in the file list.
*/

FILE_ARRAY_ELEMENT_PTR FileListRecord::get_oldest_file(SHORT *pindex)
{
	DWORD oldest_time = (DWORD)(-1);
	SHORT oldest_index = -1;
	FILE_ARRAY_ELEMENT_PTR oldest_file = NULL;

	for (SHORT i = 0; i < m_file_array.count(); i++)
	{
		FILE_ARRAY_ELEMENT_PTR pfile;

		if ((pfile = m_file_array.get_file(i)) != NULL)
		{
			if (pfile->last_access < oldest_time)
			{
				oldest_time = pfile->last_access;
				oldest_index = i;
				oldest_file = pfile;
			}
		}
	}

/* Return what we found. */

	if (pindex != NULL)
	{
		*pindex = oldest_index;
	}
	return oldest_file;
}

/*
// Delete the specified file from the file list.
*/

ERRORCODE FileListRecord::delete_file(SHORT index)
{
	FILE_ARRAY_ELEMENT_PTR pfile;

/* Get the file. */

	if ((pfile = m_file_array.get_file(index)) == NULL)
	{
		return ERRORCODE_BadParameter;
	}

/* Delete the record from the database. */

	ERRORCODE error;

	if ((error = database->delete_record(pfile->record_number, FRT_EmbeddedFile)) != ERRORCODE_None)
	{
		return error;
	}

/* Delete the element from the array. */

	modified();

	return m_file_array.delete_element(index);
}
