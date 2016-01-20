/*
// SPLTARRY.H
//
//		Copyright (c) 1994 MicroLogic Software, Inc.
//		Code to manage arrays that, for compatibility reasons, are split
// into two sections in a file.  This happens when the size of a record
// stored as an array member increases.  Since the generic Array object
// does not store the element size in the file, we must store the new
// data (at the end of the record) in a separate part of the file.
//
// $Header: /PM8/App/SPLTARRY.CPP 1     3/03/99 6:11p Gbeddow $
//
// $Log: /PM8/App/SPLTARRY.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:46:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:28   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:06   JAY
// Initial revision.
// 
//    Rev 1.2   30 Nov 1994 16:34:38   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.1   04 Aug 1994 07:33:26   JAY
// Large model and 32-bit changes
// 
//    Rev 1.0   16 Mar 1994 16:03:18   MACDUFF
// Initial revision.
*/

#include "stdafx.h"

#include <string.h>
#include "error.h"
#include "spltarry.h"


ERRORCODE SplitArray::read(StorageDevicePtr device)
{
	ERRORCODE error = ERRORCODE_None;

	Array OldArray(OldSize());
	if ((error = OldArray.read(device)) != ERRORCODE_None)
	{
		return error;
	}

	if ((error = set_array_size(OldArray.count())) != ERRORCODE_None)
	{
		return error;
	}

	LPBYTE lpOldData, lpNewData;

	lpOldData = (LPBYTE)OldArray.get_element(0);
	lpNewData = (LPBYTE)data;

	for (int i = 0; i < OldArray.count(); i++)
	{
		memcpy(lpNewData, lpOldData, OldSize());
		memset(lpNewData + OldSize(), 0, SizeDifference());
		lpNewData += NewSize();
		lpOldData += OldSize();
	}
	return error;
}

ERRORCODE SplitArray::write(StorageDevicePtr device)
{
	ERRORCODE error = ERRORCODE_None;

	Array OldArray(OldSize());
	if ((error = OldArray.set_array_size(count())) != ERRORCODE_None)
	{
		return error;
	}

	LPBYTE lpOldData, lpNewData;

	lpOldData = (LPBYTE)OldArray.get_element(0);
	lpNewData = (LPBYTE)data;

	for (int i = 0; i < OldArray.count(); i++)
	{
		memcpy(lpOldData, lpNewData, OldSize());
		lpNewData += NewSize();
		lpOldData += OldSize();
	}

	error = OldArray.write(device);
	return error;
}

ST_MAN_SIZE SplitArray::size(StorageDevicePtr device)
{
	// This will have to change when we write record size to the new data area
	
	// Size is size of regular array, plus size of element count for second array, plus
	// size of overhead for second array.  This overhead is determined by sizing an empty
	// array.
	Array a(0);
	return Array::size(device) + sizeof (m_nOldElemSize) + a.size(device);
}	


void SplitArray::set_new_data_defaults()
{
}	

ERRORCODE SplitArray::read_new_data(StorageDevicePtr device)
{
	ERRORCODE error = ERRORCODE_None;

	SHORT nStoredElemSize;

	if ((error = device->read(&nStoredElemSize, sizeof (nStoredElemSize))) != ERRORCODE_None)
	{
		return error;
	}

	Array NewArray(nStoredElemSize);
	if ((error = NewArray.read(device)) != ERRORCODE_None)
	{
		return error;
	}

	if (NewArray.count() != count())
	{
		return ERRORCODE_TypeMismatch;
	}

	LPBYTE lpSrcData, lpDestData;
	lpDestData = (LPBYTE)data + OldSize();
	lpSrcData = (LPBYTE)NewArray.get_element(0);

	for (int i = 0; i < count(); i++)
	{
		memcpy(lpDestData, lpSrcData, nStoredElemSize);
		lpDestData += NewSize();
		lpSrcData += nStoredElemSize;
	}
	return error;
}

ERRORCODE SplitArray::write_new_data(StorageDevicePtr device)
{
	ERRORCODE error = ERRORCODE_None;
	SHORT nStoredElemSize = SizeDifference();

	if ((error = device->write(&nStoredElemSize, sizeof (nStoredElemSize))) != ERRORCODE_None)
	{
		return error;
	}

	Array NewArray(nStoredElemSize);
	if ((error = NewArray.set_array_size(count())) != ERRORCODE_None)
	{
		return error;
	}

	LPBYTE lpSrcData, lpDestData;
	lpSrcData = (LPBYTE)data + OldSize();
	lpDestData = (LPBYTE)NewArray.get_element(0);

	for (int i = 0; i < count(); i++)
	{
		memcpy(lpDestData, lpSrcData, nStoredElemSize);
		lpDestData += nStoredElemSize;
		lpSrcData += NewSize();
	}

	error = NewArray.write(device);
	return error;
}

