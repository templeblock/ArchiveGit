/*
// SPLTARRY.H
//
//		Copyright (c) 1994 MicroLogic Software, Inc.
//		Code to manage arrays that, for compatibility reasons, are split
// into two sections in a file.
//
// $Header: /PM8/App/SPLTARRY.H 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/SPLTARRY.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:45:18   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:22   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:58   JAY
// Initial revision.
// 
//    Rev 1.0   16 Mar 1994 16:03:16   MACDUFF
// Initial revision.
*/

#ifndef _SPLTARRY_H
#define _SPLTARRY_H

#include "array.h"
#include "storeman.h"					/* We need storage devices */

FARCLASS SplitArray : public Array
{
	SHORT m_nOldElemSize;

	SHORT OldSize() const { return m_nOldElemSize; }
	SHORT NewSize() const { return element_size; }
	SHORT SizeDifference() const { return NewSize() - OldSize(); }
public:

	// Constructor

	SplitArray(SHORT elem_size, SHORT old_elem_size, SHORT first_element = 0) : Array(elem_size, first_element)
		{ m_nOldElemSize = old_elem_size; }

	// Overrides of Array class
	ERRORCODE read(StorageDevicePtr device);
	ERRORCODE write(StorageDevicePtr device);
	ST_MAN_SIZE size(StorageDevicePtr device);

	// New methods
	virtual void set_new_data_defaults();
	ERRORCODE read_new_data(StorageDevicePtr device);
	ERRORCODE write_new_data(StorageDevicePtr device);
};

	


#endif


