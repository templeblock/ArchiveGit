/*
// $Workfile: NETID.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
*/

#include "stdafx.h"
#include "netid.h"

/*
// The text in the header of the id file.
*/

static char BASED_CODE cbFileHeader[] =
"PrintMaster Gold network user id file. (c) MicroLogic Software, Inc. 1997\x0d\x0a\x1a";

#define	HEADER_PAD	32				/* 32 blanks */

#define	TOTAL_HEADER_SIZE		(sizeof(cbFileHeader)-1+HEADER_PAD)

//
// Create the user id file.
//

NETID_STATUS UserIDFile::Create(LPCSTR pszName)
{
	NETID_STATUS Status = NetIDError_Create;

	// Attempt to create the file.
	if (m_cfFile.Open(pszName, CFile::modeReadWrite | CFile::modeCreate | CFile::shareExclusive))
	{
		// Created the file.
		// Write the "empty" state.

		TRY
		{
			// Write the file header.
			m_cfFile.Write(cbFileHeader, sizeof(cbFileHeader)-1);

			// Now write the pad.
			char buffer[HEADER_PAD];
			memset(buffer, 0, sizeof(buffer));
			m_cfFile.Write(buffer, sizeof(buffer));

			// We have totally succeeded. Go home happy.
			Status = NetIDStatus_OK;
		}
		CATCH_ALL(e)
		{
			// Remove our failure.
			TRY
			{
				// Make sure the file is closed before we zap it.
				m_cfFile.Close();
				// Bye bye!
				CFile::Remove(pszName);
			}
			END_TRY
		}
		END_CATCH_ALL
	}

	// Return our resulting status.
	return Status;
}

//
// Delay for a second.
//

static void WaitASecond(void)
{
	ULONG ulStart = GetTickCount(), ulNow;

	do
	{
		ulNow = GetTickCount();
	} while (ulNow >= ulStart && ulNow < ulStart + 1000);
}

//
// The constructor for the user id file.
//

UserIDFile::UserIDFile()
{
	m_nUserCount = 0;
}

//
// The destructor for the user id file.
//

UserIDFile::~UserIDFile()
{
}

//
// Initialize the user ID file. Open or create it.
//

NETID_STATUS UserIDFile::Initialize(LPCSTR pszName)
{
	ASSERT(pszName != NULL);
	if (pszName == NULL)
	{
		return NetIDError_BadName;
	}

	// Init status to some default.
	NETID_STATUS Status = NetIDError_FileInUse;

/*
// Try to open the user file.
// This involves an arduous retry process in case somebody else currently
// has the file open.
*/

	for (int nRetries = 3; nRetries--; )
	{
		// Try to open the file.
		CFileException e;

		if (m_cfFile.Open(pszName, CFile::modeReadWrite | CFile::shareExclusive, &e))
		{
			// File is opened! Success!
			Status = NetIDStatus_OK;
			break;
		}

		if (e.m_cause == CFileException::fileNotFound)
		{
			// The file does not yet exist! Create it.
			Status = Create(pszName);
			break;
		}

		// Now...
		WaitASecond();
	}

	// OK status.
	if (Status == NetIDStatus_OK)
	{
		// The file has been opened.
		// Count how many users there are.
		long l;
		TRY
		{
			l = m_cfFile.GetLength() - TOTAL_HEADER_SIZE;
			m_nUserCount = (int)(l/USER_NAME_LENGTH);
		}
		CATCH_ALL(e)
		{
			m_nUserCount = -1;
		}
		END_CATCH_ALL

		if (m_nUserCount < 0 || (long)m_nUserCount*USER_NAME_LENGTH != l)
		{
			m_cfFile.Close();
			Status = NetIDError_Corrupt;
		}
	}

	return Status;
}

/*
// Return the id for a user name.
// The name is assumed to be non-blank.
//
// If the name is not in the file, it is appended to the file.
// On error, a NETID_STATUS value is returned.
*/

int UserIDFile::GetUserID(LPCSTR pszName)
{
	if (pszName[0] == '\0')
	{
		// Good old "No name".
		return 0;
	}

	char name_buffer[USER_NAME_LENGTH];

	// Seek in the file and look at all the names.

	TRY
	{
		m_cfFile.Seek(TOTAL_HEADER_SIZE, CFile::begin);

		// Go through all names.

		for (int i = 0; i++ < m_nUserCount; )
		{
			if (m_cfFile.Read(name_buffer, sizeof(name_buffer)) != sizeof(name_buffer))
			{
				AfxThrowFileException(CFileException::endOfFile);
			}

			// See if the names match.
			if (stricmp(name_buffer+1, pszName) == 0)
			{
				// They do! Return the index.
				return i;
			}
		}
	
		// We did not find the name.
		// Attempt to write it to the end of the file.
		// The name is written zero-padded as a Pascal string.

		// Build the name.
		memset(name_buffer, 0, sizeof(name_buffer));
		strcpy(name_buffer+1, pszName);
		name_buffer[0] = (CHAR)(UCHAR)strlen(pszName);

		// Write the name.
		m_cfFile.Write(name_buffer, sizeof(name_buffer));
	}
	CATCH_ALL(e)
	{
		return NetIDError_Corrupt;
	}
	END_CATCH_ALL

	// The name was added successfully.
	// Update and return the new id.

	return ++m_nUserCount;
}

/*
// Get the user's id.
// On error, a NETID_STATUS value is returned.
*/

int GetUserID(LPCSTR pName, LPCSTR pUserFile, int *pUserCount)
{
	int nID = 0;				// Index of no-name.
	*pUserCount = 1;

	UserIDFile File;

	if ((nID = File.Initialize(pUserFile)) == NetIDStatus_OK)
	{
		// We have the file.
		*pUserCount = File.GetUserCount();
		nID = File.GetUserID(pName);
	}

	return nID;
}
