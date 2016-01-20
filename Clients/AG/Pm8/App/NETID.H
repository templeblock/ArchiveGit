/*
// $Header: /PM8/App/NETID.H 1     3/03/99 6:08p Gbeddow $
//
// Network id definitions.
//
// $Log: /PM8/App/NETID.H $
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
//    Rev 1.0   14 Mar 1996 13:29:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:35:02   FRED
// Initial revision.
// 
//    Rev 1.0   20 Sep 1995 09:13:10   JAY
// Initial revision.
// 
//    Rev 1.0   12 Jul 1993 14:42:26   JAY
// Initial revision.
*/

#ifndef __NETID_H__
#define __NETID_H__

#define	USER_NAME_LENGTH_MAX	48
#define	USER_NAME_LENGTH		(USER_NAME_LENGTH_MAX+1)

// Possible error values for the status field.

typedef enum
{
	NetIDError_BadName	= -4,
	NetIDError_FileInUse	= -3,
	NetIDError_Create		= -2,
	NetIDError_Corrupt	= -1,
	NetIDStatus_OK			= 0,
} NETID_STATUS;

/*
// The PrintMaster Gold user id file.
*/

class UserIDFile
{
public:
	// Constructor.
	UserIDFile();

	// Destructor.
	~UserIDFile();

	// Initialize the user id file.
	NETID_STATUS Initialize(LPCSTR pszName = NULL);

	// Return the number of users.
	int GetUserCount()
		{ return m_nUserCount+1; }		/* +1 for noname */

	// Return the ID of the user identified by name.
	// The name is added if not found.
	int GetUserID(LPCSTR pszName);

protected:
	CFile m_cfFile;
	int m_nUserCount;
	NETID_STATUS Create(LPCSTR pszName);
};

extern int GetUserID(LPCSTR pName, LPCSTR pUserFile, int* pUserCount);

#endif								/* #ifndef __NETID_H__ */
