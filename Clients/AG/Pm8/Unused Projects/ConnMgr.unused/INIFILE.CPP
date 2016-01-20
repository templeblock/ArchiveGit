/*
// $Workfile: INIFILE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/ConnMgr/INIFILE.CPP $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 3     2/26/99 10:08a Johno
// Moved AppendNamedApplicationRegistryPath() from PMW.CPP to INIFILE.CPP
// 
// 2     4/27/98 5:29p Fredf
// Improved bar positioning.
// 
//    Rev 1.1   16 Oct 1997 11:47:20   hal
// modified GetWindowPosition() -- no longer does a ShowWindow(), just positions the window.
// 
//    Rev 1.0   14 Aug 1997 15:21:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:48   Fred
// Initial revision.
// 
//    Rev 1.5   21 Apr 1997 14:49:10   Jay
// Adde GetSectionKeys
// 
//    Rev 1.4   02 Jul 1996 10:56:38   Fred
// Sender Information
// 
//    Rev 1.3   21 Jun 1996 09:29:24   Fred
// Unsigned integer support
// 
//    Rev 1.2   10 Jun 1996 08:16:48   Fred
// GetWindowPosition() and WriteWindowPosition().
// 
//    Rev 1.1   07 May 1996 12:43:46   Jay
// From PMW2
// 
//    Rev 1.1   06 May 1996 12:01:08   FRED
// Dynamically configurable project support
// 
//    Rev 1.0   29 Jan 1996 16:53:18   JAY
// Variable configuration in INSTALL and PMW.
// 
//    Rev 1.0   19 Dec 1995 15:28:12   JAY
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


// GetWindowPosition() and WriteWindowPosition() were taken from
// PMW.CPP and modified to work with any INI file.

static const char BASED_CODE ENTRY_Valid[] = "Valid";
static const char BASED_CODE ENTRY_Maximized[] = "Maximized";
static const char BASED_CODE ENTRY_X0[] = "X0";
static const char BASED_CODE ENTRY_Y0[] = "Y0";
static const char BASED_CODE ENTRY_Width[] = "Width";
static const char BASED_CODE ENTRY_Height[] = "Height";
static const char BASED_CODE ENTRY_ScreenWidth[] = "ScreenWidth";
static const char BASED_CODE ENTRY_ScreenHeight[] = "ScreenHeight";

// Load a window's position from the user's INI file,
// and, optionally, move the window to that position.
BOOL CIniFile::GetWindowPosition(LPCSTR pszSection, CWnd* pWnd, BOOL* pfIsMaximized /*=NULL*/, LPRECT pRect /*=NULL*/)
{
	BOOL fResult = FALSE;

	BOOL fIsMaximized = FALSE;
	int nX0 = 0;
	int nY0 = 0;
	int nXSize = 0;
	int nYSize = 0;

	// Validate section.
   if ((pszSection != NULL) && GetInteger(pszSection, ENTRY_Valid, FALSE))
   {
	   // Check if the window was maximized
		if (GetInteger(pszSection, ENTRY_Maximized, TRUE))
		{
			fIsMaximized = TRUE;
		}

		// Get the current screen size.
		int nScreenXSize = GetSystemMetrics(SM_CXSCREEN);
		int nScreenYSize = GetSystemMetrics(SM_CYSCREEN);

		// Get all the old information. Use appropriate defaults.
		int nOldScreenXSize = GetInteger(pszSection, ENTRY_ScreenWidth, nScreenXSize);
		int nOldScreenYSize = GetInteger(pszSection, ENTRY_ScreenHeight, nScreenYSize);

		// Get the window dimensions.
		nXSize = GetInteger(pszSection, ENTRY_Width, 640);
		nYSize = GetInteger(pszSection, ENTRY_Height, 480);
		nX0 = GetInteger(pszSection, ENTRY_X0, (nOldScreenXSize-nXSize)/2);
		nY0 = GetInteger(pszSection, ENTRY_Y0, (nOldScreenYSize-nYSize)/2);

		// Scale window to the new screen size.
		if (nScreenXSize != nOldScreenXSize)
		{
			nX0 = MulDiv(nX0, nScreenXSize, nOldScreenXSize);
			nXSize = MulDiv(nXSize, nScreenXSize, nOldScreenXSize);
		}

		if (nScreenYSize != nOldScreenYSize)
		{
			nY0 = MulDiv(nY0, nScreenYSize, nOldScreenYSize);
			nYSize = MulDiv(nYSize, nScreenYSize, nOldScreenYSize);
		}

		// Make sure we fit on the screen.
		if (nXSize > nScreenXSize)	nXSize = nScreenXSize;
		if (nYSize > nScreenYSize)	nYSize = nScreenYSize;

		// Set the position. Force it to be entirely on the screen.
		if (nX0 < 0)							nX0 = 0;
		if (nY0 < 0)							nY0 = 0;
		if (nX0 + nXSize > nScreenXSize)	nX0 = nScreenXSize - nXSize;
		if (nY0 + nYSize > nScreenYSize)	nY0 = nScreenYSize - nYSize;

		// Move the window if the caller wants.
		if (pWnd != NULL)
		{
			if (fIsMaximized)
			{
				// Maximize the window.
            pWnd->ShowWindow(SW_SHOWMAXIMIZED);
			}
			else
			{
				// Position the window here.
				pWnd->MoveWindow(nX0, nY0, nXSize, nYSize);
			}
		}

		fResult = TRUE;
	}

	// Return the size and position if the caller requested.
	if (pfIsMaximized != NULL)
	{
		*pfIsMaximized = fIsMaximized;
	}
	if (pRect != NULL)
	{
		pRect->left = nX0;
		pRect->top = nY0;
		pRect->right = pRect->left+nXSize;
		pRect->bottom = pRect->top+nYSize;
	}

	return fResult;
}

// Save a window's position to the INI file.
void CIniFile::WriteWindowPosition(LPCSTR pszSection, CWnd* pWnd)
{
	if (!pWnd->IsIconic())
	{
		WriteInteger(pszSection, ENTRY_Maximized, pWnd->IsZoomed());

		CRect crBounds;
		pWnd->GetWindowRect(&crBounds);

		WriteInteger(pszSection, ENTRY_X0, crBounds.left);
		WriteInteger(pszSection, ENTRY_Y0, crBounds.top);
		WriteInteger(pszSection, ENTRY_Width, crBounds.Width());
		WriteInteger(pszSection, ENTRY_Height, crBounds.Height());
		WriteInteger(pszSection, ENTRY_ScreenWidth, GetSystemMetrics(SM_CXSCREEN));
		WriteInteger(pszSection, ENTRY_ScreenHeight, GetSystemMetrics(SM_CYSCREEN));
		WriteInteger(pszSection, ENTRY_Valid, TRUE);
	}
}
// JRO Appends a string in the form "application name\version number"
// for use with Windows registry
// This is intended for launching other application
// in the PrintMaster / PrintShop family
// The functionality is based on PrintShop code, using PrintMaster functions
BOOL
CIniFile::AppendNamedApplicationRegistryPath(CString &RegPath)
{
	CString	Temp1, Temp2;
	
	Temp1 = GetString("Product Identification", "product");
	if (Temp1.IsEmpty())
		return FALSE;
	Temp2 = GetString("Product Identification", "version");
	if (Temp2.IsEmpty())
		return FALSE;

	RegPath += Temp1;
	RegPath += "\\";
	RegPath += Temp2;
	
	return TRUE;
}
