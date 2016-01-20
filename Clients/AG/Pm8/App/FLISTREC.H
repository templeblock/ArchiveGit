/*
// $Header: /PM8/App/FLISTREC.H 1     3/03/99 6:05p Gbeddow $
//
// Definitions for file list record (used in file database).
//
// $Log: /PM8/App/FLISTREC.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:48   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:34   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:34:08   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   04 Aug 1994 07:33:06   JAY
// Large model and 32-bit changes
// 
//    Rev 1.1   07 Jul 1994 11:26:22   JAY
// Fixed a near/far problems with NULL.
// 
//    Rev 1.0   09 Jun 1994 11:12:44   JAY
// Initial revision.
*/

#ifndef __FLISTREC_H__
#define __FLISTREC_H__

#include "array.h"

/*
// A file array element. One is used per file entry in the database.
*/

typedef struct
{
	DB_RECORD_NUMBER	record_number;				/* Embedded file record #. */
	WORD					hash;							/* Hash of file name. */
	DWORD					last_access;				/* Time of last access. */
} FILE_ARRAY_ELEMENT, far *FILE_ARRAY_ELEMENT_PTR;

/*
// An array of these file elements.
//
// This array is sorted by hash value.
*/

FARCLASS FileArray : public Array
{
public:
/*
// Constructor.
*/
	FileArray() : Array(sizeof(FILE_ARRAY_ELEMENT))
		{}

/*
// Get a file in the array.
*/

	FILE_ARRAY_ELEMENT_PTR get_file(SHORT which)
		{ return (FILE_ARRAY_ELEMENT_PTR)get_element(which); }
};

/*
// A file list record.
*/

FARCLASS FileListRecord : public DatabaseRecord
{
private:

	typedef struct
	{
	/* Filled in before the search. */

		FileListRecord far *this_record;
		LPCSTR search_name;
		WORD search_hash;

	/* Filled in during the search. */

		FILE_ARRAY_ELEMENT_PTR last_searched_element;
		int last_result;
	} FILE_SEARCH_KEY;

/*
// The file array.
*/

	FileArray m_file_array;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	FileListRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// The comparison function for use with bsearch() and the necessary global
// variables.
*/

	static int file_element_compare(PCVOID key, PCVOID elem2);

/*
// Actually find a file. This function needs a key which is filled in with
// various arcane information (that the caller may want to use!).
*/

	ERRORCODE do_find_file(LPCSTR name, DB_RECORD_NUMBER far *lprecord, FILE_SEARCH_KEY *key);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all embedded files referenced must be removed from the database as
// well. This should not FREE anything (in the memory sense); that's the job
// of the destructor.
*/

	virtual VOID destroy(VOID);

/*
// Find a file.
//
// The record number of the embedded file is returned in the variable pointed
// to by 'lprecord'.
*/

	ERRORCODE find_file(LPCSTR name, DB_RECORD_NUMBER far *lprecord);

/*
// Add a file (embed it).
// If the file is already in the list, it is merely returned without error.
//
// The record number of the embedded file is returned in the variable pointed
// to by 'lprecord' if 'lprecord' is not NULL.
*/

	ERRORCODE add_file(LPCSTR name, DB_RECORD_NUMBER far *lprecord = NULL);

/*
// Find the oldest file in the file list.
*/

	FILE_ARRAY_ELEMENT_PTR get_oldest_file(SHORT *pindex);

/*
// Delete the specified file from the file list.
*/

	ERRORCODE delete_file(SHORT index);

/***************************************************/
/* Methods used exclusively by the StorageManager. */
/***************************************************/

/*
// ReadData()
//
// Read the record.
*/

	ERRORCODE	ReadData(StorageDevicePtr device);

/*
// WriteData()
//
// Write the record.
*/

	ERRORCODE	WriteData(StorageDevicePtr device);

/*
// SizeofData()
//
// Return the size of the record.
*/

	ST_MAN_SIZE SizeofData(StorageDevicePtr device);
};

#endif						/* #ifndef __FLISTREC_H__ */
