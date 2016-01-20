/*
// $Workfile: NETWORK.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/NETWORK.CPP $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:32   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:55:58   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "remind.h"

#include "netid.h"
#include "netcount.h"

#include <stdio.h>
#include <stdlib.h>

// Some installation file names.
#define	USERS_DAT_FILE_NAME	"PMUSERS.DAT"

BOOL CRemindApp::InitNetwork(void)
{
	BOOL fResult = FALSE;
	
	TRY
	{
		// Initialize global network information.
		m_csNetworkUserName.Empty();
		m_nNetworkUserId = -1;
		m_nNetworkUserCount = 0;
		
		// Get the user name.
		LPSTR pszNetworkUserName = m_csNetworkUserName.GetBuffer(64);
		BOOL fUnknown = GetNetworkUserName(pszNetworkUserName);
		m_csNetworkUserName.ReleaseBuffer();
		if (m_csNetworkUserName.GetLength() > 0)
		{
			// Look up the name to get the id.
			char lzNetworkUserName[64];
			strcpy(lzNetworkUserName+1, m_csNetworkUserName);
			lzNetworkUserName[0] = (char)(m_csNetworkUserName.GetLength());
			char szMessage[256] = "";
			if (GetNetworkUserId(fUnknown ? "" : lzNetworkUserName, szMessage))
			{
				fResult = TRUE;
			}
		}
	}
	END_TRY
	
	return fResult;
}

// The network user name.
// If the user name is unknown, TRUE is returned; otherwise, FALSE is returned.
BOOL CRemindApp::GetNetworkUserName(LPSTR lpszNetworkUserName)
{
	*lpszNetworkUserName = 0;

	// Attempt to get the user name from the Windows network driver.

	// Call WNetGetUser() to get the user name.
	TRACE0("Call WNetGetUser\n");
	int nResult;
	
	unsigned long ulBufferSize = 48;
	if ((nResult = WNetGetUser(NULL, lpszNetworkUserName, &ulBufferSize)) != WN_SUCCESS)
	{
		TRACE1("WNetGetUser Failed (%d)\n", nResult);
		// The call failed, clear the string.
		*lpszNetworkUserName = 0;
	}

	TRACE1("WNetGetUser returned |%s|\n", lpszNetworkUserName);

	// If we still do not have a name, then use the name
	// specified by the environment variable MLSUSER.
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
	
	::LoadString(AfxGetResourceHandle(), IDS_UNKNOWN, lpszNetworkUserName, 48);
	
	return TRUE;
}

BOOL CRemindApp::GetNetworkUserId(LPSTR lplzNetworkUserName, LPSTR pszMessage)
{
	*pszMessage = '\0';

	CString csUserFileName;
	CString csMessage;
	
	TRY
	{
		csUserFileName = PmwDirectory;
		csUserFileName += USERS_DAT_FILE_NAME;

		// Cheat for now!
		m_nNetworkUserId = GetUserID(lplzNetworkUserName+1, csUserFileName, &m_nNetworkUserCount);

		if (m_nNetworkUserId < 0)
		{
			switch (m_nNetworkUserId)
			{
				case NetIDError_FileInUse:
				{
					csMessage.LoadString(IDS_NETWORK_FILE_IN_USE);
					break;
				}
				case NetIDError_Corrupt:
				{
					csMessage.LoadString(IDS_NETWORK_FILE_CORRUPTED);
					break;
				}
				case NetIDError_Create:
				{
					csMessage.LoadString(IDS_NETWORK_FILE_CANT_CREATE);
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
	}
	END_TRY

	strcpy(pszMessage, csMessage);

   TRACE("ID: %d Count: %d\n", m_nNetworkUserId, m_nNetworkUserCount);

	return *pszMessage == 0;
}
