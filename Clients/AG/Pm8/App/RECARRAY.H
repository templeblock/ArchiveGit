/*
// Record array definitions.
//
// $Header: /PM8/App/RECARRAY.H 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/RECARRAY.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:42   Fred
// Initial revision.
// 
//    Rev 1.1   15 Aug 1996 16:00:52   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:43:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:38   FRED
// Initial revision.
// 
//    Rev 2.1   19 Apr 1995 16:04:54   JAY
// Removed some symbols.
// 
//    Rev 2.0   07 Feb 1995 15:48:54   JAY
// Initial revision.
// 
//    Rev 1.1   10 Jun 1994 09:59:26   JAY
// Added PVCS header.
// Added Ptr and Ref typedefs.
*/

#ifndef __RECARRAY_H__
#define __RECARRAY_H__

#include "array.h"

/*
// An array of records.
*/

FARCLASS RecordArray : public Array
{
public:

/*
// The constructor for the record array.
*/

	RecordArray() : Array(sizeof(DB_RECORD_NUMBER)) {}

/*
// insert()
//
//	Insert a record at a position in the array.
*/

	ERRORCODE insert(DB_RECORD_NUMBER record, SHORT where) { return insert_element(&record, where); }

/*
// add()
//
// Add a record to the end of the array.
*/

	ERRORCODE add(DB_RECORD_NUMBER record) { return insert(record, element_count); }

/*
// get()
//
// Get a record number.
*/

	DB_RECORD_NUMBER get(SHORT which);

/*
// find()
//
// Find a record in the array.
*/

	int find(DB_RECORD_NUMBER record);

/*
// remove()
//
// Remove a record from the array.
*/

	ERRORCODE remove(DB_RECORD_NUMBER record);
};

typedef RecordArray far &RecordArrayRef;

#endif					/* #ifndef __RECARRAY_H__ */
