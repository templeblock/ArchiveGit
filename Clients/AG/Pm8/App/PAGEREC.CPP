/*
// $Header: /PM8/App/PAGEREC.CPP 1     3/03/99 6:08p Gbeddow $
//
// Page record routines.
//
// $Log: /PM8/App/PAGEREC.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:34   Fred
// Initial revision.
// 
//    Rev 1.1   26 Jul 1996 11:38:56   Jay
// Page stuff
// 
//    Rev 1.0   14 Mar 1996 13:45:58   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:10   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:00   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:34:02   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   01 Jun 1994 16:35:18   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.1   05 Jan 1994 08:05:42   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:24:56   JAY
// 
*/

#include "stdafx.h"
#include "pagerec.h"
#include "pmgobj.h"

/*****************************************************************************/
/*                              Page record                                  */
/*****************************************************************************/

/*
// The creator for a page record.
*/

ERRORCODE PageRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	if ((*record = (DatabaseRecordPtr)(new PageRecord(number, type, owner, where))) == NULL)
	{
		return ERRORCODE_Memory;
	}
	return ERRORCODE_None;
}

/*
// The constructor for a project preview record.
*/

PageRecord::PageRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where),
				  object_list((ObjectDatabasePtr)owner)
{
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE PageRecord::ReadData(StorageDevicePtr device)
{
/* Just read the object list. */

	return object_list.read(device);
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE PageRecord::WriteData(StorageDevicePtr device)
{
/* Just write the object list. */

	return object_list.write(device);
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE PageRecord::SizeofData(StorageDevicePtr device)
{
/* Just size the object list. */

	return object_list.size(device);
}

/*
// Assign method.
*/

ERRORCODE PageRecord::assign(DatabaseRecordRef srecord)
{
/* Assign the base record first. */
	return DatabaseRecord::assign(srecord);
}

void PageRecord::FixupPageNumbers(void)
{
	for (PMGPageObject* pObject = (PMGPageObject*)objects()->first_object();
		  pObject != NULL;
		  pObject = (PMGPageObject*)pObject->next_object())
	{
		pObject->Page(Id());
	}
}
