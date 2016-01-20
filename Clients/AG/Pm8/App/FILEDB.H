/*
// $Header: /PM8/App/FILEDB.H 1     3/03/99 6:05p Gbeddow $
//
// Definitions for PMG database record objects.
//
// $Log: /PM8/App/FILEDB.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:46   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:32   JAY
// Initial revision.
// 
//    Rev 1.3   07 Jul 1994 11:26:44   JAY
// Fixed a near/far problems with NULL.
// 
//    Rev 1.2   20 Jun 1994 13:25:58   JAY
// Added override for reinitialize().
// Made set_maximum_size() physically try and reduce the file size if necessary.
// 
//    Rev 1.1   09 Jun 1994 11:11:22   JAY
// Main implementation.
// 
//    Rev 1.0   01 Jun 1994 16:44:18   JAY
// Initial revision.
*/

#ifndef __FILEDB_H__
#define __FILEDB_H__

#include "database.h"
#include "file.h"

/*
// Record types for a FileDatabase.
*/

typedef enum
{
	FRT_FileList = 1,
	FRT_FileDirectory,
	FRT_EmbeddedFile
} FILE_RECORD_TYPE;

/*
// Additional record classes we use.
*/

/* fdirrec.h */

FARCLASS FileDirectoryRecord;
typedef FileDirectoryRecord far *FileDirectoryPtr;
typedef FileDirectoryRecord far &FileDirectoryRef;
typedef const FileDirectoryRecord far *FileDirectoryCPtr;
typedef const FileDirectoryRecord far &FileDirectoryCRef;

/* flistrec.h */

FARCLASS FileListRecord;
typedef FileListRecord far *FileListPtr;
typedef FileListRecord far &FileListRef;
typedef const FileListRecord far *FileListCPtr;
typedef const FileListRecord far &FileListCRef;

/*
// A database for holding embedded files.
*/

FARCLASS FileDatabase : public Database
{
private:

	ERRORCODE m_last_error;

	ST_DEV_POSITION m_maximum_size;

	ERRORCODE create_file_directory(void);

public:

/*
// initialize() [Our override for initialize()]
// 
// Initialize the database. The start of the database in the storage
// device is passed as a parameter.
// 
*/

	virtual ERRORCODE initialize(ST_DEV_POSITION position);

/*
// reinitialize() [Our override for reinitialize()]
*/

	virtual ERRORCODE reinitialize(void);

/*
// Create a file directory record.
*/

	FileDirectoryPtr create_file_directory_record(VOIDPTR creation_data = NULL)
		{ return (FileDirectoryPtr)new_record(FRT_FileDirectory, creation_data); }

/*
// Create a file list record.
*/

	FileListPtr create_file_list_record(VOIDPTR creation_data = NULL)
		{ return (FileListPtr)new_record(FRT_FileList, creation_data); }

/*
// Create an embedded file record.
*/

	EmbeddedFilePtr create_embedded_file_record(VOIDPTR creation_data = NULL)
		{ return (EmbeddedFilePtr)new_record(FRT_EmbeddedFile, creation_data); }

/*
// Create a file database.
*/

	FileDatabase(StorageDevicePtr device);

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
// Prep a file (if the image is in the database).
*/

	ERRORCODE prep_file(LPCSTR name, SHORT file_type, StorageFilePtr file);

/*
// Set the maximum size for this database.
// This value is only used when embedding a new file.
*/

	ERRORCODE set_maximum_size(ST_DEV_POSITION max_size = POSITION_MAX, BOOL adjust_storage = FALSE);

/*
// Get the maximum size for this database.
*/

	ST_DEV_POSITION get_maximum_size(void) const
		{ return m_maximum_size; }

/*
// Delete the oldest embedded file record in the database.
*/

	ERRORCODE delete_oldest_record(void);

};

typedef FileDatabase far *FileDatabasePtr;
typedef FileDatabase far &FileDatabaseRef;

/*
// Hard-coded database record numbers for the base record.
// This record MUST have this record number.
// (This is easily maintained by creating it first.)
*/

#define FILEDB_DIRECTORY_RECORD			1

#endif						/* #ifndef __FILEDB_H__ */
