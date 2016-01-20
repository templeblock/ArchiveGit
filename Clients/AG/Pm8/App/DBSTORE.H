/*
// Storage device and temp file definitions.
// Oct. 12, 1992 - Jay N. : Initial creation.
//
// $Header: /PM8/Download/DBSTORE.H 1     3/03/99 6:15p Gbeddow $
//
// $Log: /PM8/Download/DBSTORE.H $
// 
// 1     3/03/99 6:15p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:30   Fred
// Initial revision.
// 
//    Rev 1.1   11 Apr 1997 17:09:40   dennisp
// Added get_name() for retrieving device name
// 
//    Rev 1.0   14 Mar 1996 13:48:04   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:14   FRED
// Initial revision.
// 
//    Rev 2.2   13 May 1995 11:14:22   JAY
//  
// 
//    Rev 2.1   11 May 1995 09:38:22   JAY
// Moved huge_read to base class.
// 
//    Rev 2.0   07 Feb 1995 15:48:28   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:34:08   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.0   03 Nov 1994 12:56:30   FRED
//  
// 
//    Rev 1.1   01 Nov 1994 09:59:40   JAY
// New data manager routines
// 
//    Rev 1.0   25 Oct 1994 16:25:12   JAY
// Initial revision.
// 
//    Rev 1.2   13 Jul 1994 11:58:52   JAY
// Changed some routines to (correctly) take LPCSTRs.
// 
//    Rev 1.1   09 Jun 1994 11:09:42   JAY
// Added lock() method;
// 
//    Rev 1.0   08 Feb 1994 09:29:32   JAY
// Initial revision.
*/

#ifndef __DBSTORE_H__
#define __DBSTORE_H__

#include "error.h"

/*
// Seek mode for a storage device.
// These numbers are defined to correspond with the standard library numbers
// (for obvious reasons of ease of implementation of derived classes).
*/

enum ST_DEV_SEEK_MODE
{
	ST_DEV_SEEK_SET=0,
	ST_DEV_SEEK_CUR=1,
	ST_DEV_SEEK_END=2
};

/*
// A position in a storage device.
*/

typedef long ST_DEV_POSITION;

#define	POSITION_UNKNOWN		((ST_DEV_POSITION)-1)
#define	POSITION_UNALLOCATED	((ST_DEV_POSITION)0)
#define	POSITION_MAX			((ST_DEV_POSITION)0x7FFFFFFFL)

/*
// The size of an I/O transaction with a storage device.
*/

typedef USHORT ST_DEV_IO_SIZE;

/*
// A storage device.
// StorageDevice is an abstract class which merely defines a programming
// interface. It contains no code or data.
// Flash! The "move()" method contains code at this level.
// Flash, flash! The new string routines also contain code here.
*/

class StorageDevice
{
public:

/*
// The destructor for the StorageDevice class.
// The destructor is virtual so that it can be overloaded by derived classes.
*/

	virtual ~StorageDevice() { }

/*
// initialize()
//
// Create a blank storage device.
// If successful, the storage device will have no data in it.
*/

	virtual ERRORCODE initialize() = 0;

   virtual LPCSTR		get_name() { return NULL; }
/*
// read()
//
// Read a sequence of bytes at the current I/O position into an in-memory
// buffer. The I/O position is left just past the last byte read.
// If we read fewer than the requested bytes and the "size_read" pointer
// is NULL (or default), an error is returned. If a non-NULL pointer is
// passed for "size_read", no error is reported if the full read is not
// satisfied. It then becomes the caller's responsibility to check the amount
// actually read.
*/

	virtual ERRORCODE read(LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read = NULL) = 0;

/*
// huge_read
//
// Read a (possibly huge) sequence of bytes. See "read" for more details.
// This is defined in this class since it can be implemented using the
// normal read function.
*/

	virtual ERRORCODE huge_read(LPVOID ptr, DWORD size, DWORD *size_read = NULL);

/*
// write()
//
// Write a sequence of bytes to the current I/O position from an in-memory
// buffer. The I/O position is left just past the last byte written.
*/

	virtual ERRORCODE write(LPVOID ptr, ST_DEV_IO_SIZE size) = 0;

/*
// huge_write
//
// Read a (possibly huge) sequence of bytes. See "write" for more details.
// This is defined in this class since it can be implemented using the
// normal write function.
*/

	virtual ERRORCODE huge_write(LPVOID ptr, DWORD size, DWORD *size_write = NULL);

/*
// move()
//
// Move a sequence of bytes from one I/O position to another.
// The I/O position becomes undefined.
// Overlapping moves (moving data into the middle of the source block)
// are handled correctly.
// This routine is actually coded at this level because it can be written in
// a standard way on top of the existing methods.
*/

	virtual ERRORCODE move(ST_DEV_POSITION src, ST_DEV_POSITION dst, ST_DEV_POSITION size);

/*
// seek()
//
// Set the I/O position to a specfied location.
*/

	virtual ERRORCODE seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode) = 0;

/*
// tell()
//
// Return the current I/O position.
*/

	virtual ERRORCODE tell(ST_DEV_POSITION far *position) = 0;

/*
// length()
//
// Return the length of the data in the device.
*/

	virtual ERRORCODE length(ST_DEV_POSITION far *size) = 0;

/*
// truncate()
//
// Truncate the device data at the current I/O position.
*/

	virtual ERRORCODE truncate() = 0;

/*
// flush()
//
// Do anything necessary to make sure a storage device is consistent and clean.
// The state of the I/O position is considered undefined after this call.
*/

	virtual ERRORCODE flush() = 0;

/*
// io_limits()
//
// Set the limits for an I/O transaction.
// Call with no parameters to reset.
// It's not critical for a device to support this (though it is recommended).
//
// Limit start is the first position which can be read or written. Limit end
// is the first position after the start which can NOT be read or written
// (the coordinates are "between positions").
//
// Passing POSITION_UNKNOWN disables an end.
*/

	virtual ERRORCODE io_limits(ST_DEV_POSITION limit_start = POSITION_UNKNOWN, ST_DEV_POSITION limit_end = POSITION_UNKNOWN) = 0;

/*
// lock()
//
// Lock (or unlock) a range of bytes in a file.
*/

	virtual ERRORCODE lock(ST_DEV_POSITION range_start, ST_DEV_POSITION range_length, BOOL unlock = FALSE)
		{ return ERRORCODE_None; }

/*
// Read a string from a file.
// The string is allocated with a far heap allocate.
*/

	virtual ERRORCODE read_string(LPSTR far *string);

/*
// Write a string to a file.
*/

	virtual ERRORCODE write_string(LPCSTR string);

/*
// Return the size of a string (as it would be in the file).
*/

	virtual SHORT size_string(LPCSTR string);

/*
// Read a CString from a file.
*/

	virtual ERRORCODE read_cstring(CString& csString);

/*
// Write a CString to a file.
*/

	virtual ERRORCODE write_cstring(const CString& csCtring);

/*
// Return the size of a CString (as it would be in the file).
*/

	virtual SHORT size_cstring(const CString& csString);

/*
// Read a block of data.
// A heap allocation is done.
*/

	virtual ERRORCODE read_block(ST_DEV_IO_SIZE size, LPVOID far *block);

/*
// Read a sized record.
// The first word in the file is assumed to be the record size.
// Then comes the record.
// If the file record size is larger than the memory record size, only the
// memory record size is read, then the rest is skipped.
// If the file record size is smaller than the memory record size, only the
// file record size is read and the rest is filled with zeros.
// Either way, the size of the file record is returned if desired.
//
// The memory size of the file should not include the size word itself.
*/

	virtual ERRORCODE read_record(LPVOID record, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE far *file_size = NULL);

/*
// Write a record to file.
// The size of the record is written first followed by the data.
*/

	virtual ERRORCODE write_record(LPVOID record, ST_DEV_IO_SIZE size);

/*
// Return the size of a record.
*/

	virtual ST_DEV_IO_SIZE size_record(ST_DEV_IO_SIZE size)
		{ return size + sizeof(ST_DEV_IO_SIZE); }

};

typedef StorageDevice far *StorageDevicePtr;

#endif				/* #ifndef __DBSTORE_H__ */
