/*
// $Header: /PM8/App/NETCOUNT.CPP 1     3/03/99 6:08p Gbeddow $
//
// Network license count code.
//
// $Log: /PM8/App/NETCOUNT.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:18   Fred
// Initial revision.
// 
//    Rev 1.3   10 Jun 1997 13:34:58   Fred
// Removed more network stuff
// 
//    Rev 1.2   03 Jun 1997 09:10:24   Fred
// Fixed bug in file sharing test
// 
//    Rev 1.1   05 Nov 1996 17:31:16   Jay
// New network stuff
// 
//    Rev 1.0   14 Mar 1996 13:33:06   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:36   FRED
// Initial revision.
// 
//    Rev 1.0   20 Sep 1995 09:13:12   JAY
// Initial revision.
// 
//    Rev 1.1   13 Jul 1993 17:04:52   JAY
// Fixed an off-by-one error.
// 
//    Rev 1.0   12 Jul 1993 16:46:32   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "netcount.h"

#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <sys\locking.h>

/*
// See if file locking is available.
*/

BOOL TestFileLocking(CFile& cfFile, DWORD dwWhere)
{
	TRY
	{
		// Lock...
		cfFile.LockRange(dwWhere, 1);
		// ...and unlock.
		cfFile.UnlockRange(dwWhere, 1);
	}
	CATCH(CFileException, e)
	{
#if 0
		if (e->m_cause == CFileException::hardIO)
		{
			// This is what EINVAL is mapped to.
			// It means file locking is not available.
			return FALSE;
		}
#endif
		return FALSE;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	END_CATCH_ALL

	// File locking is available.
	return TRUE;
}

/*
// Return the number of locks on a file.
// Passed is the maximum number of locks which can exist.
// If the returned count < 0, then it is a NET_COUNT_STATUS.
*/

static int NetLockCount(LPCSTR pszName, int nMaxLocks, int nID)
{
	CFile cfFile;

	// Open the file.

	if (!cfFile.Open(pszName, CFile::modeRead | CFile::shareDenyWrite, NULL))
	{
		return NetCount_FileUnavailable;
	}

	// See if file locking is available.
	if (!TestFileLocking(cfFile, nID+FILE_LOCK_TEST_AREA))
	{
		return NetCount_LockingUnavailable;
	}

	// Now run through the file and check the locks.

	int nLocks = 0;

	for (int i = 0; i < nMaxLocks; i++)
	{
		TRY
		{
			cfFile.LockRange(i, 1);
			cfFile.UnlockRange(i, 1);
		}
		CATCH_ALL(e)
		{
			// The file must be locked at that point.
			nLocks++;
		}
		END_CATCH_ALL
	}

	return nLocks;
}

/*
// Set a user lock.
*/

static BOOL SetUserLock(LPCSTR pszName, CFile& cfFile, int nUserID)
{
	// Open the file.

	BOOL fResult = FALSE;
	if (cfFile.Open(pszName, CFile::modeRead | CFile::shareDenyWrite, NULL))
	{
		TRY
		{
			fResult = TRUE;
			cfFile.LockRange((DWORD)nUserID, 1);
		}
		CATCH_ALL(e)
		{
			// Failed. Close the file.
			cfFile.Close();
		}
		END_CATCH_ALL
	}

	return fResult;
}

/*
// Take care of the startup of a network user.
*/

int NetworkUserStartup(int nUserID, int nUserCount, int nAvailableLocks, LPCSTR pLockFileName, CFile& cfLockFile)
{
	if (nAvailableLocks == 0)
	{
		nAvailableLocks++;
	}

	// Count the network locks.
	int nNetworkUsers = NetLockCount(pLockFileName, nUserCount, nUserID);

	switch (nNetworkUsers)
	{
		case NetCount_LockingUnavailable:
		{
		/* We can't lock. That's OK. Go home. */
			return -4;
		}
		case NetCount_FileUnavailable:
		{
		/* We couldn't find the lock file. Ugh! */
			return -3;
		}
		default:
		{
			break;
		}
	}

	// See if we have exceeded our count.

	if (nNetworkUsers >= nAvailableLocks)
	{
		// 6/9/97 (FF) Removed check for maximum number of users.
#if 0
		return -1;
#endif
	}

/*
// There are enough locks.
// See if we can lock our position.
*/

	if (!SetUserLock(pLockFileName, cfLockFile, nUserID))
	{
		return -2;
	}

/*
// All is well on the home front.
// The lock is set. It will remain set until this instance of PM terminates.
// Now we can return.
*/
	return ++nNetworkUsers;
}

