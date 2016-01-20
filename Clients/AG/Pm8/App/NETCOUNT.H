/*
// $Header: /PM8/App/NETCOUNT.H 1     3/03/99 6:08p Gbeddow $
//
// Network count definitions.
//
// $Log: /PM8/App/NETCOUNT.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:42   Fred
// Initial revision.
// 
//    Rev 1.1   05 Nov 1996 17:31:22   Jay
// New network stuff
// 
//    Rev 1.0   14 Mar 1996 13:29:10   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:02   FRED
// Initial revision.
// 
//    Rev 1.0   20 Sep 1995 09:13:12   JAY
// Initial revision.
// 
//    Rev 1.0   12 Jul 1993 16:46:42   JAY
// Initial revision.
*/

#ifndef __NETCOUNT_H__
#define __NETCOUNT_H__

typedef enum
{
	NetCount_LockingUnavailable	= -2,
	NetCount_FileUnavailable		= -1,
	NetCount_OK							= 0
} NET_COUNT_STATUS;

#define	MAXIMUM_FILE_LOCKS		256
#define	FILE_LOCK_TEST_AREA		MAXIMUM_FILE_LOCKS

int NetworkUserStartup(int nUserId,
							  int nUserCount,
							  int nAvailableLocks,
							  LPCSTR pLockFileName,
							  CFile& cfLockFile);

#endif								/* #ifndef __NETCOUNT_H__ */
