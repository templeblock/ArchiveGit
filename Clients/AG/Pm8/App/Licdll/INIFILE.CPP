/*
// $Workfile: INIFILE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/INIFILE.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:48   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:41:38   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/INIFILE.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:28   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:02   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:36   JAY
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:54   FRED
//  
// 
//    Rev 1.0   25 Oct 1995 08:23:02   JAY
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
