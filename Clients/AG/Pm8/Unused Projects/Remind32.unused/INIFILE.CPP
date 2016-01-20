/*
// $Workfile: INIFILE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/INIFILE.CPP $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:30   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:55:54   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "inifile.h"

/*
// Construction.
*/

CIniFile::CIniFile()
{
}

CIniFile::CIniFile(LPCSTR pFileName)
{
	Name(pFileName);
}

/*
// Name access.
*/

/* Set the name. */

void CIniFile::Name(LPCSTR pFileName)
{
	m_csName = pFileName;
}

/* Get the name. */

LPCSTR CIniFile::Name(void) const
{
	return m_csName;
}

/*
// Get a list of key names for a section.
*/

LPSTR CIniFile::GetSectionKeys(LPCSTR pszSection)
{
	LPSTR pszKeys = NULL;

	int nStartSize;
	int nIncrementSize;
	int nMaxSize;

#ifdef WIN32
	nStartSize = 0x00000400L;
	nIncrementSize = 0x00000400L;
	nMaxSize = 0x7fffffffL;
#else
	nStartSize = 0x00000400L;
	nIncrementSize = 0x00000400L;
	nMaxSize = 0x00007fffL;
#endif
	TRY
	{
		int nBufferSize = nStartSize;
		while ((pszKeys == NULL) && (nBufferSize <= nMaxSize))
		{
			// Try the current buffer size.
			pszKeys = new char[nBufferSize];

			// Should never happen, but...
			if (pszKeys == NULL)
			{
				break;
			}

			DWORD dwResult = GetPrivateProfileString(pszSection, NULL, "", pszKeys, nBufferSize, m_csName);
			if (dwResult >= (DWORD)nBufferSize-2)
			{
				// Buffer is too small.
				delete pszKeys;
				pszKeys = NULL;
				nBufferSize += nIncrementSize;
			}
		}
	}
	CATCH_ALL(e)
	{
		delete pszKeys;
		pszKeys = NULL;
	}
	END_CATCH_ALL

	return pszKeys;
}

/*
// Strings manipulation.
*/

/* Get a string. */

CString CIniFile::GetString(LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszDefault)
{
	if (pszDefault == NULL)
	{
		pszDefault = "";
	}

	if (m_csName.GetLength() == 0)
	{
		return pszDefault;
	}

	char sz[_MAX_PATH];
	::GetPrivateProfileString(pszSection, pszEntry, pszDefault, sz, sizeof(sz), m_csName);

	return sz;
}

/* Write a string. */

BOOL CIniFile::WriteString(LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszValue)
{
	return ::WritePrivateProfileString(pszSection, pszEntry, pszValue, m_csName);
}

/*
// Integer manipulation.
*/

/* Get an integer. */

int CIniFile::GetInteger(LPCSTR pszSection, LPCSTR pszEntry, int nDefault)
{
	if (m_csName.GetLength() == 0)
	{
		return nDefault;
	}
	return ::GetPrivateProfileInt(pszSection, pszEntry, nDefault, m_csName);
}

/* Write an integer. */

BOOL CIniFile::WriteInteger(LPCSTR pszSection, LPCSTR pszEntry, int nValue)
{
	char Buffer[40];
	itoa(nValue, Buffer, 10);

	return WriteString(pszSection, pszEntry, Buffer);
}

/* Write an integer with default removal. */

BOOL CIniFile::WriteInteger(LPCSTR pszSection, LPCSTR pszEntry, int nValue, int nDefault)
{
	if (nValue == nDefault)
	{
		return RemoveEntry(pszSection, pszEntry);
	}
	else
	{
		return WriteInteger(pszSection, pszEntry, nValue);
	}
}

/*
// Unsigned integer manipulation.
*/

/* Get an unsigned integer. */

UINT CIniFile::GetUnsignedInteger(LPCSTR pszSection, LPCSTR pszEntry, UINT uDefault)
{
	if (m_csName.GetLength() == 0)
	{
		return uDefault;
	}
	return (UINT)::GetPrivateProfileInt(pszSection, pszEntry, uDefault, m_csName);
}

/* Write an integer. */

BOOL CIniFile::WriteUnsignedInteger(LPCSTR pszSection, LPCSTR pszEntry, UINT uValue)
{
	char Buffer[40];
	ultoa((DWORD)uValue, Buffer, 10);

	return WriteString(pszSection, pszEntry, Buffer);
}

/* Write an integer with default removal. */

BOOL CIniFile::WriteUnsignedInteger(LPCSTR pszSection, LPCSTR pszEntry, UINT uValue, UINT uDefault)
{
	if (uValue == uDefault)
	{
		return RemoveEntry(pszSection, pszEntry);
	}
	else
	{
		return WriteInteger(pszSection, pszEntry, uValue);
	}
}

/*
// Long manipulation.
*/

/* Get a long integer. */

long CIniFile::GetLong(LPCSTR pszSection, LPCSTR pszEntry, long lDefault)
{
	if (m_csName.GetLength() != 0)
	{
		CString csValue = GetString(pszSection, pszEntry);
		if (csValue != "")
		{
			lDefault = atol(csValue);
		}
	}
	return lDefault;
}

/* Write a long integer. */

BOOL CIniFile::WriteLong(LPCSTR pszSection, LPCSTR pszEntry, long lValue)
{
	char Buffer[40];
	ltoa(lValue, Buffer, 10);

	return WriteString(pszSection, pszEntry, Buffer);
}

/* Write a long integer with default removal. */

BOOL CIniFile::WriteLong(LPCSTR pszSection, LPCSTR pszEntry, long lValue, long lDefault)
{
	if (lValue == lDefault)
	{
		return RemoveEntry(pszSection, pszEntry);
	}
	else
	{
		return WriteLong(pszSection, pszEntry, lValue);
	}
}

/*
// Unsigned long (DWORD) manipulation.
*/

/* Get a DWORD. */

DWORD CIniFile::GetDword(LPCSTR pszSection, LPCSTR pszEntry, DWORD dwDefault)
{
	if (m_csName.GetLength() != 0)
	{
		CString csValue = GetString(pszSection, pszEntry);
		if (!csValue.IsEmpty())
		{
			dwDefault = strtoul(csValue, NULL, 10);
		}
	}
	return dwDefault;
}

/* Write a DWORD. */

BOOL CIniFile::WriteDword(LPCSTR pszSection, LPCSTR pszEntry, DWORD dwValue)
{
	char Buffer[40];
	ultoa(dwValue, Buffer, 10);

	return WriteString(pszSection, pszEntry, Buffer);
}

/* Write a DWORD with default removal. */

BOOL CIniFile::WriteDword(LPCSTR pszSection, LPCSTR pszEntry, DWORD dwValue, DWORD dwDefault)
{
	if (dwValue == dwDefault)
	{
		return RemoveEntry(pszSection, pszEntry);
	}
	else
	{
		return WriteDword(pszSection, pszEntry, dwValue);
	}
}

/*
// Entry removal.
*/

BOOL CIniFile::RemoveEntry(LPCSTR pszSection, LPCSTR pszEntry)
{
	return WriteString(pszSection, pszEntry, NULL);
}

/*
// Section removal.
*/

BOOL CIniFile::RemoveSection(LPCSTR pszSection)
{
	return WriteString(pszSection, NULL, NULL);
}
