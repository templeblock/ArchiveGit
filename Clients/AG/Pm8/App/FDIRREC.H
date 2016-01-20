/*
// $Header: /PM8/App/FDIRREC.H 1     3/03/99 6:05p Gbeddow $
//
// Definitions for the file directory record.
//
// $Log: /PM8/App/FDIRREC.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:42   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:32   JAY
// Initial revision.
// 
//    Rev 1.2   30 Nov 1994 16:38:52   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.1   07 Jul 1994 11:26:24   JAY
// Fixed a near/far problems with NULL.
// 
//    Rev 1.0   09 Jun 1994 11:13:12   JAY
// Initial revision.
*/

#ifndef __FDIRREC_H__
#define __FDIRREC_H__

#include "array.h"

/*
// A file list element. One is used per file type in the database.
*/

typedef struct
{
	SHORT					type;							/* File type. */
	DB_RECORD_NUMBER	record_number;				/* File list record #. */
} FILE_LIST_ELEMENT, far *FILE_LIST_ELEMENT_PTR;

/*
// An array of these file list elements.
//
// This array is not sorted (except perhaps implicitly by the order of
// addition). It is assumed that there won't be too many different *types* of
// files being added.
*/

FARCLASS FileListArray : public Array
{
public:
/*
// Constructor.
*/
	FileListArray() : Array(sizeof(FILE_LIST_ELEMENT))
		{}

/*
// Get a file list element in the array.
*/

	FILE_LIST_ELEMENT_PTR get_list(SHORT which)
		{ return (FILE_LIST_ELEMENT_PTR)get_element(which); }
};

/*
// A file database directory record.
//
// This record is the first record in a file database, and it points to the
// various file lists in which the files are stored.
*/

FARCLASS FileDirectoryRecord : public DatabaseRecord
{
private:

	FileListArray m_list_array;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	FileDirectoryRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Find a type in the list.
*/

	FILE_LIST_ELEMENT_PTR find_type(SHORT type);

/*
// Add a type to the list.
*/

	ERRORCODE add_type(SHORT type, DB_RECORD_NUMBER far *lprecord);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

	virtual VOID destroy(VOID);

/*
// Find a file.
//
// The record number of the embedded file is returned in the variable pointed
// to by 'lprecord'.
*/

	ERRORCODE find_file(LPCSTR name, SHORT file_type, DB_RECORD_NUMBER far *lprecord);

/*
// Add a file (embed it).
//
// The record number of the embedded file is returned in the variable pointed
// to by 'lprecord' if 'lprecord' is not NULL.
*/

	ERRORCODE add_file(LPCSTR name, SHORT file_type, DB_RECORD_NUMBER far *lprecord = NULL);

/*
// Delete the oldest embedded file record in the database.
*/

	ERRORCODE delete_oldest_record(void);

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

#endif						/* #ifndef __FDIRREC_H__ */
