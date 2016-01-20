/*
// $Workfile: COMMAND.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/COMMAND.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   22 Nov 1995 08:52:12   FRED
// Initial revision.
// 
//    Rev 1.3   16 Nov 1995 14:16:38   JAY
// 
//    Rev 1.2   16 Nov 1995 13:24:24   JAY
// 
//    Rev 1.1   16 Nov 1995 12:24:06   FRED
//  
// 
//    Rev 1.0   16 Nov 1995 10:31:26   FRED
//  
*/

#include <windows.h>
#include "command.h"
#include <string.h>

///////////////////////////////////////////////////////
// class CommandParser

static char UpChar(char ch)
{
	if ((ch >= 'a') && (ch <= 'z'))
	{
		ch += 'A'-'a';
	}

	return ch;
}

CCommandParser::CCommandParser(void)
{
	m_pszCommand = NULL;
	m_pszOptions = NULL;
}

CCommandParser::~CCommandParser(void)
{
	delete [] m_pszCommand;
	delete [] m_pszOptions;
}

BOOL CCommandParser::Init(LPCSTR pszString)
{
	BOOL fResult = FALSE;

	delete [] m_pszCommand;
	m_pszCommand = NULL;

	delete [] m_pszOptions;
	m_pszOptions = NULL;

	if (pszString != NULL)
	{
		// Skip white space.
		while (*pszString == ' ')
		{
			pszString++;
		}

		// Collect command.
		char szBuffer[128];
		int i = 0;

		while ((i < sizeof(szBuffer)-1)
			 && (*pszString != '\0')
			 && (*pszString != ' ')
			 && (*pszString != '/'))
		{
			szBuffer[i++] = *(pszString++);
		}

		// Save a copy of the command.
		if ((i > 0) && (i < sizeof(szBuffer)-1))
		{
			szBuffer[i++] = '\0';
			m_pszCommand = new char[i];

			if (m_pszCommand != NULL)
			{
				lstrcpy(m_pszCommand, szBuffer);

				// Collect the options.
				i = 0;

				while ((i < sizeof(szBuffer)-1)
					 && (*pszString != '\0'))
				{
					if (*pszString == '/')
					{
						if (*(pszString+1) != '\0')
						{
							szBuffer[i++] = UpChar(*(++pszString));
						}
					}

					pszString++;
				}

				fResult = TRUE;

				// Save a copy of the options.
				if (i > 0)
				{
					fResult = FALSE;

					if (i < sizeof(szBuffer)-1)
					{
						szBuffer[i++] = '\0';
						m_pszOptions = new char[i];

						if (m_pszOptions != NULL)
						{
							lstrcpy(m_pszOptions, szBuffer);
							fResult = TRUE;
						}
					}
				}
			}
		}
	}

	return fResult;
}

LPCSTR CCommandParser::Command(void)
{
	return m_pszCommand;
}

BOOL CCommandParser::Option(char cOption)
{
	BOOL fResult = FALSE;

	if (m_pszOptions != NULL)
	{
		fResult = strchr(m_pszOptions, UpChar(cOption)) != NULL;
	}

	return fResult;
}
