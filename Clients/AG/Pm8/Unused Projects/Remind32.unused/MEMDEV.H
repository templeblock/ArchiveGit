/*
// A simple memory-based device (read-only).
//
// $Header: /PM8/REMIND32/MEMDEV.H 1     3/03/99 6:26p Gbeddow $
//
// $Log: /PM8/REMIND32/MEMDEV.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:54   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:43:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:24   FRED
// Initial revision.
// 
//    Rev 2.1   18 Aug 1995 15:06:02   JAY
// Photo CD; graphic images, etc.
// 
//    Rev 2.0   07 Feb 1995 15:48:44   JAY
// Initial revision.
// 
//    Rev 1.0   10 Jan 1995 15:41:30   JAY
// Initial revision.
*/

#ifndef __MEMDEV_H__
#define __MEMDEV_H__

#include "dbstore.h"				/* Define our base clase. */
#include "array.h"				/* We use an array. */

class MemoryDevice : public StorageDevice
{
public:
/*
// Constructor.
*/

	MemoryDevice(BOOL fOwnsPieces = FALSE);

/*
// Destructor.
*/

	virtual ~MemoryDevice();

/*
// Add a piece to this memory file.
// The pointer is NOT managed in any way by this object (e.g. it is not
// freed here - unless fOwnsMemory above is TRUE).
*/

	void AddPiece(LPVOID pData, DWORD dwDataSize);

////////////////////////////////////////////////////////
// StorageDevice overriden methods.

/*
// initialize()
//
// Create a blank storage device.
// If successful, the storage device will have no data in it.
*/

	virtual ERRORCODE initialize();

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

	virtual ERRORCODE read(LPVOID ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read = NULL);

/*
// write()
//
// Write a sequence of bytes to the current I/O position from an in-memory
// buffer. The I/O position is left just past the last byte written.
*/

	virtual ERRORCODE write(LPVOID ptr, ST_DEV_IO_SIZE size);

/*
// seek()
//
// Set the I/O position to a specfied location.
*/

	virtual ERRORCODE seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode);

/*
// tell()
//
// Return the current I/O position.
*/

	virtual ERRORCODE tell(ST_DEV_POSITION far *position);

/*
// length()
//
// Return the length of the data in the device.
*/

	virtual ERRORCODE length(ST_DEV_POSITION far *size);

/*
// truncate()
//
// Truncate the device data at the current I/O position.
*/

	virtual ERRORCODE truncate();

/*
// flush()
//
// Do anything necessary to make sure a storage device is consistent and clean.
// The state of the I/O position is considered undefined after this call.
*/

	virtual ERRORCODE flush();

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

	virtual ERRORCODE io_limits(ST_DEV_POSITION limit_start = POSITION_UNKNOWN, ST_DEV_POSITION limit_end = POSITION_UNKNOWN);
protected:
	Array m_Pieces;

	ST_DEV_POSITION m_Position;
	ST_DEV_POSITION m_Length;
	BOOL m_fOwnsPieces;
};

struct MEMORY_DEVICE_PIECE
{
	ST_DEV_POSITION m_Start;
	LPVOID m_pData;
	long m_lSize;
};

#endif				/* #ifndef __MEMDEV_H__ */
