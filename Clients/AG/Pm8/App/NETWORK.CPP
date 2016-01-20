/*
// $Header: /PM8/App/NETWORK.CPP 1     3/03/99 6:08p Gbeddow $
//
// Network support code for Windows.
//
// $Log: /PM8/App/NETWORK.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 3     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:22:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:20   Fred
// Initial revision.
// 
//    Rev 1.5   14 Jul 1997 11:17:32   Fred
// Removed some obsolete ifdefs
// 
//    Rev 1.4   11 Jun 1997 15:49:46   johno
// Add on support
// 
// 
//    Rev 1.3   06 Nov 1996 14:47:34   johno
// Moved strings to resource DLL
// 
//    Rev 1.2   05 Nov 1996 17:31:18   Jay
// New network stuff
// 
//    Rev 1.1   24 May 1996 16:12:50   Fred
// TRACEx
// 
//    Rev 1.0   14 Mar 1996 13:33:10   Jay
// Initial revision.
// 
//    Rev 1.1   13 Mar 1996 14:52:10   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.0   16 Feb 1996 12:29:40   FRED
// Initial revision.
// 
//    Rev 2.5   20 Sep 1995 09:13:38   JAY
// New code for 32-bit version.
// 
//    Rev 2.4   13 Sep 1995 11:48:04   JAY
// New 32-bit changes
// 
//    Rev 2.3   25 Aug 1995 18:40:54   JAY
// Some debug messages.
// 
//    Rev 2.2   08 Aug 1995 19:02:48   FRED
//  
// 
//    Rev 2.1   08 Feb 1995 13:37:52   JAY
// Reverted. New series.
// 
//    Rev 1.10   30 Nov 1994 16:59:18   JAY
//  
// 
//    Rev 1.9   16 Sep 1994 14:10:54   JAY
// User is given name "Unknown" if Micro Center version.
// 
//    Rev 1.8   04 Aug 1994 14:30:06   JAY
// Accidently removed NEW_NETWORK defined.
// 
//    Rev 1.7   04 Aug 1994 08:30:38   JAY
// Large model and 32-bit changes
// 
//    Rev 1.6   08 Jun 1994 14:45:12   JAY
// Now correctly gets the user name under Windows for Workgroups.
// 
//    Rev 1.5   23 May 1994 09:06:56   JAY
// Now includes pathmgr.h (with system.h before it).
// 
//    Rev 1.4   12 May 1994 07:37:32   JAY
// Now uses path manager instead of APP working_directory.
// 
//    Rev 1.3   25 Jan 1994 12:56:36   JAY
//  
// 
//    Rev 1.2   19 Jan 1994 17:09:20   JAY
//  
// 
//    Rev 1.1   17 Dec 1993 17:19:00   JAY
//  
//
//   Rev 1.0   16 Dec 1993 16:28:22   JAY
// 
*/

#include "stdafx.h"
#include "pmw.h"

#include "license.h"

#define NEW_NETWORK

#ifdef NEW_NETWORK
#ifndef _WIN32
#include "winnet.h"
#include "multinet.h"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>

#include "netid.h"
#include "netcount.h"
#include "pmwcfg.h"

/* Just what is a FILENAME anyway? Well, here's your answer... */

typedef char FILENAME[_MAX_PATH];

/*
// Some installation file names.
*/

#define	INST_DAT_FILE_NAME				"PMINST.DAT"
#define	LOCK_FILE_NAME						"NETL.PM"
#define	USERS_DAT_FILE_NAME				"PMUSERS.DAT"

/*
// The network user name.
// If the user name is unknown, TRUE is returned; otherwise, FALSE is returned.
*/

BOOL GetNetworkUserName(LPSTR lpszNetworkUserName)
{
	*lpszNetworkUserName = 0;

	/*
	// Attempt to get the user name from the Windows network driver.
	*/

#ifndef _WIN32
	/*
	// If this is Windows for Workgroups, select the primary network.
	*/

	UINT uCaps = WNetGetCaps(WNNC_NET_TYPE);
	TRACE1("WNetGetCaps(WNNC_NET_TYPE) %04x\n\r", uCaps);

	UINT uUser = WNetGetCaps(WNNC_USER);
	TRACE1("WNetGetCaps(WNNC_USER) %04x\n\r", uUser);
	
	if (uCaps & WNNC_NET_MultiNet)
	{
		TRACE0("MultiNet\n\r");
	/*
	// If multiple-network bit set, check low byte for
	// Windows for Workgroups bit.
	*/
		if (LOBYTE(uCaps) & WNNC_SUBNET_WinWorkgroups)
		{
			TRACE0("Windows for Workgroups\n\r");
		/*
		// Windows for Workgroups is loaded.
		// Load WFWNET.DRV so we can call the multi functions.
		// We use the name "WFWNET.DRV" explicitly because, if we were
		// linking with the import library for these functions, it would be
		// hard-bound to WFWNET.
		*/

			HMODULE hDriver;

			UINT uPrevSem = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
			hDriver = ::LoadLibrary("WFWNET.DRV");
			::SetErrorMode(uPrevSem);

			if (hDriver >= (HINSTANCE)32)
			{
				WORD (API *lpMNetNetworkEnum)(HANDLE FAR *hNetwork);
				WORD (API *lpMNetGetNetInfo)(HANDLE hNetwork,
													LPWORD lpwNetInfo,
													LPSTR lpszButton,
													LPINT lpcbButton,
													LPHANDLE lphInstance);
				WORD (API *lpMNetSetNextTarget)(HANDLE);

				if ((*(FARPROC *)&lpMNetNetworkEnum = ::GetProcAddress(hDriver, MAKEINTRESOURCE(33))) != NULL
					 && (*(FARPROC *)&lpMNetGetNetInfo = ::GetProcAddress(hDriver, MAKEINTRESOURCE(37))) != NULL
					 && (*(FARPROC *)&lpMNetSetNextTarget = ::GetProcAddress(hDriver, MAKEINTRESOURCE(38))) != NULL)
				{
					HANDLE hNetwork;
					WORD wNetInfo;
					char szButton[80];
					int cButton;
					HANDLE hinst;

					hNetwork =  0;
					while (lpMNetNetworkEnum(&hNetwork) == WN_SUCCESS)
					{
						cButton = 80;

						if (lpMNetGetNetInfo(hNetwork,
												 &wNetInfo,
												 szButton,
												 &cButton,
												 &hinst) != WN_SUCCESS)
						{
							break;
						}

					/* If this is the primary network, select it! */
						if (wNetInfo == MNM_NET_PRIMARY)
						{
							lpMNetSetNextTarget(hNetwork);
							break;
						}
					}
				}
			/* Free the library. */
				::FreeLibrary(hDriver);
			}
		}
	}
#endif					// #ifndef _WIN32

	/*
	// Call WNetGetUser() to get the user name.
	*/

	TRACE0("Call WNetGetUser\n\r");
	int nResult;
#ifdef WIN32
#ifdef _MAC
   // TODO: MAC -- what about this network support?
   if (FALSE)
#else // _MAC
	unsigned long ulBufferSize = 48;
	if ((nResult = WNetGetUser(NULL, lpszNetworkUserName, &ulBufferSize)) != WN_SUCCESS)
#endif // else _MAC
#else // WIN32
	int nBufferSize = 48;
	if ((nResult = WNetGetUser(lpszNetworkUserName, &nBufferSize)) != WN_SUCCESS)
#endif // WIN32
	{
		TRACE1("WNetGetUser Failed (%d)\n\r", nResult);
		/*
		// The call failed, clear the string.
		*/

		*lpszNetworkUserName = 0;
	}

	TRACE1("WNetGetUser returned |%s|\n\r", lpszNetworkUserName);

	/*
	// If we still do not have a name, then use the name
	// specified by the environment variable MLSUSER.
	*/

	if (*lpszNetworkUserName == 0)
	{
		LPSTR pszEnvironmentUserName;

		if ((pszEnvironmentUserName = getenv("MLSUSER")) != NULL)
		{
			strcpy(lpszNetworkUserName, pszEnvironmentUserName);
		}
	}

	if (*lpszNetworkUserName != 0)
	{
		return FALSE;
	}

   strcpy(lpszNetworkUserName, GET_PMWAPP()->GetResourceStringPointer (IDS_UNKNOWN_USER));
	return TRUE;
}

BOOL CPmwApp::GetNetworkUserId(LPSTR lplzNetworkUserName, LPSTR szMessage)
{
	CString csUserFileName;

	*szMessage = '\0';

	csUserFileName = GetPathManager()->ExpandPath(USERS_DAT_FILE_NAME);

	m_nUserID = GetUserID(lplzNetworkUserName+1, csUserFileName, &m_nUserIDCount);

	if (m_nUserID < 0)
	{
		switch (m_nUserID)
		{
			case NetIDError_FileInUse:
			{
				// JRO sprintf(szMessage, "The network user file, %s, is currently in use.", (LPCSTR)csUserFileName);
            sprintf(szMessage, GET_PMWAPP()->GetResourceStringPointer (IDS_NET_IN_USE_FILE), (LPCSTR)csUserFileName);
				break;
			}
			case NetIDError_Corrupt:
			{
				// JRO sprintf(szMessage, "The network user file, %s, is corrupted.", (LPCSTR)csUserFileName);
            sprintf(szMessage, GET_PMWAPP()->GetResourceStringPointer (IDS_NET_FILE_BAD), (LPCSTR)csUserFileName);
				break;
			}
			case NetIDError_Create:
			{
				// JRO sprintf(szMessage, "Unable to create the network user file, %s.", (LPCSTR)csUserFileName);
            sprintf(szMessage, GET_PMWAPP()->GetResourceStringPointer (IDS_NET_FILE_NO_MAKE), (LPCSTR)csUserFileName);
				break;
			}
			default:
			{
				// Should not get here.
				ASSERT(FALSE);
				break;
			}
		}
	}

#ifdef DEBUGON
	TRACE("ID: %d Count: %d\n", m_user_id, m_user_id_count);
#endif

	return (*szMessage == 0);
}

BOOL CPmwApp::NetworkStartup(char *szMessage)
{
	BOOL fSuccess = FALSE;

	*szMessage = '\0';

	CString csLockFileName;
	CString csDataFileName;

	csLockFileName = GetPathManager()->ExpandPath(LOCK_FILE_NAME);
	csDataFileName = GetPathManager()->ExpandPath(INST_DAT_FILE_NAME);

	for (;;)
	{
		BOOL again = FALSE;

		m_nMaxUsers = m_installation_info.bNetCount;
		m_nUserCount = NetworkUserStartup(m_nUserID,
													 m_nUserIDCount,
													 m_nMaxUsers,
													 csLockFileName,
													 GET_PMWAPP()->m_cfLockFile);

		if (m_nUserCount < 0)
		{
			switch (-m_nUserCount)
			{
				case 1:	// NetResult_NoLocksAvailable
				{
					CLicenseExceededDialog edlg;

					if (edlg.DoModal() == IDOK)
					{
						BYTE old_count = m_installation_info.bNetCount;

					/* They want the license dialog. */

						CLicenseDlg ldlg(&m_installation_info);

						ldlg.DoModal();

						if (m_installation_info.bNetCount > old_count)
						{
						/* They added some licenses! Let's try again. */
							again = TRUE;
						}
					}
					break;
				}

				case 2:	// NetResult_UserIdAlreadyLocked
				{
					// JRO wsprintf(szMessage, "User '%s' is already using %s.", (LPCSTR)m_user_name, (LPCSTR)AfxGetAppName());
               wsprintf(szMessage, GET_PMWAPP()->GetResourceStringPointer (IDS_NET_USER_USING), (LPCSTR)m_csUserName, (LPCSTR)AfxGetAppName());
					break;
				}

				case 3:	// NetResult_ProblemAccessingFiles
				{
					// JRO wsprintf(szMessage, "Cannot access '%s' and '%s'.", (LPCSTR)csLockFileName, (LPCSTR)csDataFileName);
               wsprintf(szMessage, GET_PMWAPP()->GetResourceStringPointer (IDS_NET_NO_ACCESS), (LPCSTR)csLockFileName, (LPCSTR)csDataFileName);
					break;
				}

				case 4:	// NetResult_FileLockingNotAvailable
				{
					fSuccess = TRUE;
					break;
				}

				default:
				{
					// JRO wsprintf(szMessage, "Unknown network support error (#%d).", m_nUserCount);
               wsprintf(szMessage, GET_PMWAPP()->GetResourceStringPointer (IDS_NET_ERROR_UNKNOWN), m_nUserCount);
					break;
				}
			}
		}
		else
		{
			fSuccess = TRUE;
		}

		if (!again)
		{
			break;
		}
	}

#ifdef DEBUGON
	TRACE("Handle: %d Maximum: %d Current %d\n\r", m_lock_file_handle, m_max_users, m_user_count);
#endif

	return fSuccess;
}

BOOL CPmwApp::initialize_network()
{
	char lzNetworkUserName[64];
	char szMessage[256] = { '\0'};
	BOOL started_up = FALSE;
	BOOL unknown;

	if (GetConfiguration()->IsAddOn () == TRUE)
   {
      //m_user_name = "Unknown";
      m_csUserName = GET_PMWAPP()->GetResourceStringPointer (IDS_UNKNOWN_USER);
      m_nUserID = 0;
      return TRUE;
   }

   unknown = GetNetworkUserName(&(lzNetworkUserName[1]));
	lzNetworkUserName[0] = (char)strlen(&(lzNetworkUserName[1]));

	if (lzNetworkUserName[0] == 0)
	{
		// JRO strcpy(szMessage, "Cannot get network user name.");
      strcpy(szMessage, GET_PMWAPP()->GetResourceStringPointer (IDS_NET_NO_NAME));
	}
	else
	{
	/* Copy over the user name. */

		m_csUserName = lzNetworkUserName+1;

		if (GetNetworkUserId(unknown ? "" : lzNetworkUserName, szMessage))
		{
			if (NetworkStartup(szMessage))
			{
				started_up = TRUE;
			}
		}
	}

/* See if we have seized the moment (or not). */

	if (!started_up && *szMessage != '\0')
	{
	/* Maybe some other time? */
		::MessageBox(0, szMessage, AfxGetAppName(),
 						MB_OK | MB_ICONSTOP | MB_TASKMODAL);
	}
	return started_up;
}
