/*
// $Header: /PM8/App/RECARRAY.CPP 1     3/03/99 6:11p Gbeddow $
//
// Document record array routines.
//
// $Log: /PM8/App/RECARRAY.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:14   Fred
// Initial revision.
// 
//    Rev 1.1   15 Aug 1996 16:00:52   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:46:10   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:20   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:04   JAY
// Initial revision.
// 
//    Rev 1.1   30 Nov 1994 16:33:12   JAY
// stdafx.h; merged database code from album
//
//   Rev 1.0   16 Dec 1993 16:24:48   JAY
// 
*/

#include "stdafx.h"
#include "recarray.h"

int RecordArray::find(DB_RECORD_NUMBER lRecord)
{
	int i;
	DB_RECORD_NUMBER* r;

	for (i = 0, r = (DB_RECORD_NUMBER*)data; i < element_count; i++, r++)
	{
		if (*r == lRecord)
		{
			return i;
		}
	}
	return -1;
}

/*
// Remove a record number from the array.
*/

ERRORCODE RecordArray::remove(DB_RECORD_NUMBER record)
{
	// Find the record.
	int i = find(record);

	// If we didn't find it, we can't delete it.
	if (i == -1)
	{
		return ERRORCODE_BadParameter;
	}
	else
	{
		// Delete it!
		return delete_element(i);
	}
}

/*
// Get a record from the array.
*/

DB_RECORD_NUMBER RecordArray::get(SHORT which)
{
	DB_RECORD_NUMBER far *n;

	if ((n = (DB_RECORD_NUMBER far *)get_element(which)) == NULL)
	{
		return 0;
	}
	return *n;
}
