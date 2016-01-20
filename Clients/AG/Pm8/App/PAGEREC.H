/*
// $Header: /PM8/App/PAGEREC.H 1     3/03/99 6:08p Gbeddow $
//
// Page record definitions.
//
// $Log: /PM8/App/PAGEREC.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:56   Fred
// Initial revision.
// 
//    Rev 1.1   26 Jul 1996 11:38:56   Jay
// Page stuff
// 
//    Rev 1.0   14 Mar 1996 13:45:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:12   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:50   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:35:10   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   01 Jun 1994 16:35:18   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.1   05 Jan 1994 08:08:18   JAY
// Restructuring, etc.
// 
//    Rev 1.0   16 Dec 1993 16:25:50   JAY
//  
*/

#ifndef __PAGEREC_H__
#define __PAGEREC_H__

#include "objlist.h"

/*
// The page record.
*/

FARCLASS PageRecord : public DatabaseRecord
{
private:
/*
// The list of objects on this page.
*/

	ObjectList	object_list;

/*
// The constructor for this record.
// This is private. Use the create method.
*/

	PageRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where);

/*
// Assign method.
*/

	virtual ERRORCODE assign(DatabaseRecordRef record);

public:

/*
// The creator for this type of record.
// Note that this is static so that we can register it with the database.
*/

	static ERRORCODE create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record);

/*
// Return a pointer to our list.
*/

	ObjectListPtr objects(VOID) { return &object_list; }

/*
// Fixup all objects to have the correct page.
*/

	void FixupPageNumbers(void);

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

#endif					// #ifndef __PAGEREC_H__
