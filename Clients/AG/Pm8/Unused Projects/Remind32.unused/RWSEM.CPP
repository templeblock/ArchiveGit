/*
// $Header: /PM8/REMIND32/RWSEM.CPP 1     3/03/99 6:27p Gbeddow $
//
// Read/write semaphore routines.
//
// $Log: /PM8/REMIND32/RWSEM.CPP $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:38   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:02   Fred
// Initial revision.
// 
//    Rev 1.2   18 Jul 1996 11:10:28   Jay
// Bug fixes
// 
//    Rev 1.1   17 Jul 1996 12:53:28   Jay
// Speed ups. New 'exclusive' algorithm
// 
//    Rev 1.0   14 Mar 1996 13:46:26   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:34   FRED
// Initial revision.
// 
//    Rev 2.2   02 Aug 1995 10:58:52   JAY
// Removed an over-zealous change I mad last time.
// 
//    Rev 2.1   02 Aug 1995 07:46:52   JAY
// Fixed a bug in ModifyLock.
// 
//    Rev 2.0   07 Feb 1995 15:48:06   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:33:22   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.0   03 Nov 1994 12:54:12   FRED
//  
// 
//    Rev 1.0   25 Oct 1994 16:24:00   JAY
// Initial revision.
// 
//    Rev 1.2   20 Jun 1994 15:34:32   JAY
// Now inits a sem with a random mod_count.
// 
//    Rev 1.1   20 Jun 1994 13:18:52   JAY
// Added drop_all_locks(), get_shared_read_count(), and read() methods.
// Added retry_count in lock for read_unlock().
// 
//    Rev 1.0   09 Jun 1994 11:13:38   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "rwsem.h"

#ifdef RWSEM_STATISTICS
long RW_Semaphore::m_lReadLock = 0;
long RW_Semaphore::m_lReadLockSkip = 0;
long RW_Semaphore::m_lReadUnLock = 0;
long RW_Semaphore::m_lReadUnLockSkip = 0;
#endif

/*
// Consructor for a read/write semaphore.
*/

RW_Semaphore::RW_Semaphore()
{
	m_Record.wMods =
		m_Record.wReadLocks =
		m_wModifyLocks =
		m_wReadLocks = 0;
}

/*
// Read the file area for a semaphore.
*/

ERRORCODE RW_Semaphore::Read(StorageDevicePtr device, ST_DEV_POSITION position)
{
	ERRORCODE error;

	if ((error = device->seek(position, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
		error = device->read(&m_Record, sizeof(m_Record));
	}

	return error;
}

/*
// Lock the file area of a semaphore.
// The semaphore may be busy.
*/

ERRORCODE RW_Semaphore::Lock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fExclusive /*=FALSE*/)
{
	if (fExclusive)
	{
		return ERRORCODE_None;
	}
	else
	{
		ERRORCODE error;

		// Do the lock.
		if ((error = device->lock(position, sizeof(m_Record), FALSE)) != ERRORCODE_None)
		{
			return error;
		}

		// Now read the values so we can know the current state of the semaphore.

		return Read(device, position);
	}
}

/*
// Unlock the file area of a semaphore.
// We will write our value if necessary.
*/

ERRORCODE RW_Semaphore::Unlock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fUpdate, BOOL fExclusive /*=FALSE*/)
{
	if (fExclusive)
	{
		return ERRORCODE_None;
	}
	else
	{
		if (fUpdate)
		{
		/* Now read the values so we can know the current state of the semaphore. */

			if (device->seek(position, ST_DEV_SEEK_SET) == ERRORCODE_None)
			{
				device->write(&m_Record, sizeof(m_Record));
			}
		}

	/* Do the lock. */

		return device->lock(position, sizeof(m_Record), TRUE);
	}
}

/*
// Modify lock a R/W semaphore.
*/

ERRORCODE RW_Semaphore::ModifyLock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fExclusive /*=FALSE*/)
{
/* If we're already locked, just update our internal count. */

	if (m_wModifyLocks != 0)
	{
		m_wModifyLocks++;
		m_fChanged = FALSE;
		return ERRORCODE_None;
	}

/* Remember the current mod count. */

	WORD current_mod_count = m_Record.wMods;

/* We need to lock. */

	ERRORCODE error;
	if ((error = Lock(device, position, fExclusive)) != ERRORCODE_None)
	{
		return error;
	}

//	if (m_wReadLocks != m_Record.wReadLocks)
	if ((m_wReadLocks == 0 && m_Record.wReadLocks > 0)
			|| (m_wReadLocks > 0 && m_Record.wReadLocks > 1))
	{
	/* Someone else is reading this. */
		Unlock(device, position, FALSE, fExclusive);
	/*
	// 8/2/95 (JN)
	// We are going to return an error code here.
	// This means that the caller is not going to checked Changed() (in fact,
	// we haven't even set m_fChanged yet).
	// And yet, m_Record.wMods has been updated indicating that things are
	// synced when, in fact, they're not. We need to set m_Record.wMods back
	// to what it originally was so that the next ReadLock or ModifyLock will
	// cause whatever data this is associated with to be flagged as changed.
	*/
		m_Record.wMods = current_mod_count;

		return ERRORCODE_Busy;
	}

/* Flag whether we changed or not. */

	m_fChanged = current_mod_count != m_Record.wMods;
	
	m_wModifyLocks++;

	return ERRORCODE_None;
}

/*
// Modify unlock a R/W semaphore.
*/

ERRORCODE RW_Semaphore::ModifyUnlock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fModified, BOOL fExclusive /*=FALSE*/)
{
/* Do we have a lock? */

	if (m_wModifyLocks == 0)
	{
		return ERRORCODE_NotLocked;
	}

/* If a modification is indicated, update the semaphore to reflect this. */

	if (fModified)
	{
		m_Record.wMods++;
	}

/* If multiple counts are in effect, just decrement the most recent. */

	if (--m_wModifyLocks != 0)
	{
		return ERRORCODE_None;
	}

/*
// We need to unlock. We always write. The mod count may have changed, or
// some read locks may have gone away since we locked.
*/

	return Unlock(device, position, TRUE, fExclusive);
}

/*
// Read lock a R/W semaphore.
*/

ERRORCODE RW_Semaphore::ReadLock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fExclusive /*=FALSE*/)
{
	// If we already have a read lock in effect, we do not need to increment
	// the value in the file. A value of one will represent all our read
	// locks.

#ifdef RWSEM_STATISTICS
	m_lReadLock++;
#endif
	if (m_wReadLocks > 0)
	{
#ifdef RWSEM_STATISTICS
		m_lReadLockSkip++;
#endif
		m_wReadLocks++;
		m_fChanged = FALSE;
		return ERRORCODE_None;
	}

/* Remember the current mod count. */

	WORD current_mod_count = m_Record.wMods;

/*
// If we already have a modify lock, then we are in total control.
// We just need to update our internal variables which will get written
// when the modify lock is removed.
//
// Otherwise, we need to lock and unlock as we go.
*/

	if (m_wModifyLocks == 0)
	{
	/* Do the lock. */
		ERRORCODE error;
		if ((error = Lock(device, position, fExclusive)) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Update the variables. */

	m_wReadLocks++;
	m_Record.wReadLocks++;

/* Unlock if we locked. This will also write the new lock count. */

	if (m_wModifyLocks == 0)
	{
		Unlock(device, position, TRUE, fExclusive);
	}

/* Flag whether we changed or not. */

	m_fChanged = current_mod_count != m_Record.wMods;

	return ERRORCODE_None;
}

/*
// Read unlock a R/W semaphore.
*/

ERRORCODE RW_Semaphore::ReadUnlock(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fExclusive /*=FALSE*/)
{
/* A little sanity check. */

	if (m_wReadLocks == 0)
	{
		ASSERT(FALSE);
		return ERRORCODE_NotLocked;
	}

#ifdef RWSEM_STATISTICS
	m_lReadUnLock++;
#endif

	// If we have more than one read lock in effect, we do not need to decrement
	// the value in the file. A value of one will represent all our read
	// locks.

	if (m_wReadLocks > 1)
	{
#ifdef RWSEM_STATISTICS
		m_lReadUnLockSkip++;
#endif
		m_wReadLocks--;
		return ERRORCODE_None;
	}

/*
// If we already have a modify lock, then we are in total control.
// We just need to update our internal variables which will get written
// when the modify lock is removed.
//
// Otherwise, we need to lock and unlock as we go.
*/

	if (m_wModifyLocks == 0)
	{
	/*
	// Do the lock.
	// We do extensive retrying on this because if it fails, the whole
	// lock scheme will get messed up for this semaphore.
	*/
		ERRORCODE error;

		for (int retry_count = 10; retry_count-- != 0; )
		{
			if ((error = Lock(device, position, fExclusive)) != ERRORCODE_Busy)
			{
				break;
			}
		}

		if (error != ERRORCODE_None)
		{
			return error;
		}
	}

/* Update the variables. */

	m_wReadLocks--;

/* We always 0-check this guy to prevent the file from getting corrupted. */

	if (m_Record.wReadLocks != 0)
	{
		m_Record.wReadLocks--;
	}

/* Unlock if we locked. This will also write the new lock count. */

	if (m_wModifyLocks == 0)
	{
		Unlock(device, position, TRUE, fExclusive);
	}

	return ERRORCODE_None;
}

/*
// Drop all locks on this semaphore.
*/

ERRORCODE RW_Semaphore::DropAllLocks(StorageDevicePtr device, ST_DEV_POSITION position, BOOL fExclusive /*=FALSE*/)
{
	ERRORCODE error = ERRORCODE_None;

/*
// Drop all read locks.
// We do this before the modify lock because the modify locks will prevent
// our need to do a physical lock each time.
*/

	while (m_wReadLocks != 0)
	{
		if ((error = ReadUnlock(device, position, fExclusive)) != ERRORCODE_None)
		{
			return error;
		}
	}

/* Drop all modify locks. */

	while (m_wModifyLocks != 0)
	{
		if ((error = ModifyUnlock(device, position, TRUE, fExclusive)) != ERRORCODE_None)
		{
			return error;
		}
	}

	return error;
}

/*
// Initialize a semaphore in the file.
*/

ERRORCODE RW_Semaphore::Init(StorageDevicePtr device, ST_DEV_POSITION position)
{
	ERRORCODE error;

	m_Record.wMods = (WORD)::GetTickCount();		/* Semi-random. */

	if ((error = device->seek(position, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
		error = device->write(&m_Record, sizeof(m_Record));
	}
	return error;
}
