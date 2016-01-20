/*
// $Header: /PM8/App/RWSEM.H 1     3/03/99 6:11p Gbeddow $
//
// Read/Write Semaphore definitions.
//
// $Log: /PM8/App/RWSEM.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:48   Fred
// Initial revision.
// 
//    Rev 1.2   17 Jul 1996 14:10:06   Jay
// Database bookkeeping
// 
//    Rev 1.1   17 Jul 1996 12:53:28   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.0   14 Mar 1996 13:45:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:20   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:58   JAY
// Initial revision.
// 
//    Rev 1.2   30 Nov 1994 16:33:32   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.0   03 Nov 1994 12:56:50   FRED
//  
// 
//    Rev 1.0   25 Oct 1994 16:24:02   JAY
// Initial revision.
// 
//    Rev 1.1   20 Jun 1994 13:18:02   JAY
// Added drop_all_locks(), get_shared_read_count(), and read() methods.
// 
//    Rev 1.0   09 Jun 1994 11:13:38   JAY
// Initial revision.
*/

#ifndef __RWSEM_H__
#define __RWSEM_H__

#include "dbstore.h"

#ifdef _DEBUG
#define RWSEM_STATISTICS
#endif

/*
// A read/write semaphore.
*/

class RW_Semaphore
{
private:

/* The data written to file. */
	struct SEM_RECORD
	{
		WORD wMods;
		WORD wReadLocks;
	} m_Record;

/* How many times have we "modify locked" this? */
	WORD m_wModifyLocks;

/* How many times have we "read locked" this? */
	WORD m_wReadLocks;

/* Flag to record whether the last lock was of a changed semaphore. */
	BOOL m_fChanged;

/*
// Lock the semaphore file area before we do anything to it.
// This will actually read the current semaphore values.
*/

	ERRORCODE Lock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fExclusive = FALSE);

/*
// Unlock the file area of a semaphore.
// We will write our value if necessary.
*/

	ERRORCODE Unlock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL update, BOOL fExclusive = FALSE);

public:
/*
// Construct a read/write semaphore.
*/

	RW_Semaphore();

/*
// Modify lock a R/W semaphore.
*/

	ERRORCODE ModifyLock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fExclusive = FALSE);

/*
// Modify unlock a R/W semaphore.
*/

	ERRORCODE ModifyUnlock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL modified, BOOL fExclusive = FALSE);

/*
// Read lock a R/W semaphore.
*/

	ERRORCODE ReadLock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fExclusive = FALSE);

/*
// Read unlock a R/W semaphore.
*/

	ERRORCODE ReadUnlock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fExclusive = FALSE);

/*
// Drop all locks on this semaphore.
*/

	ERRORCODE DropAllLocks(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fExclusive = FALSE);

/*
// Has the semaphore been updated?
// This should be called after a lock (modify or read).
*/

	BOOL Changed()
		{ return m_fChanged; }

/*
// Return the size of the semaphore when written to file.
*/

	static int Size(void)
		{ return sizeof(struct SEM_RECORD); }

/*
// Return the modify lock count.
*/

	WORD GetModifyLocks() const
		{ return m_wModifyLocks; }

/*
// Return the read lock count.
*/

	WORD GetReadLocks() const
		{ return m_wReadLocks; }

/*
// Return the full lock count on this semaphore.
*/

	WORD GetSharedReadLocks() const
		{ return m_Record.wReadLocks; }

/*
// Initialize a semaphore in the file.
*/

	ERRORCODE Init(StorageDevicePtr device, ST_DEV_POSITION position);

/*
// Just read the semaphore file area.
*/

	ERRORCODE Read(StorageDevicePtr device, ST_DEV_POSITION position);


#ifdef RWSEM_STATISTICS
	static long m_lReadLock;
	static long m_lReadLockSkip;
	static long m_lReadUnLock;
	static long m_lReadUnLockSkip;
#endif
};

#endif				/* #ifndef __RWSEM_H__ */
