/*
// $Workfile: INIFILE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/ConnMgr/INIFILE.H $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 3     2/26/99 10:08a Johno
// Moved AppendNamedApplicationRegistryPath() from PMW.CPP to INIFILE.CPP
// 
// 2     4/27/98 5:30p Fredf
// Improved bar positioning.
// 
//    Rev 1.1   16 Oct 1997 11:45:54   hal
// modify params to GetWindowPosition()
// 
//    Rev 1.0   14 Aug 1997 15:21:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:16   Fred
// Initial revision.
// 
//    Rev 1.5   21 Apr 1997 14:49:10   Jay
// Adde GetSectionKeys
// 
//    Rev 1.4   02 Jul 1996 10:56:38   Fred
// Sender Information
// 
//    Rev 1.3   21 Jun 1996 09:29:22   Fred
// Unsigned integer support
// 
//    Rev 1.2   10 Jun 1996 08:16:48   Fred
// GetWindowPosition() and WriteWindowPosition().
// 
//    Rev 1.1   07 May 1996 12:43:46   Jay
// From PMW2
// 
//    Rev 1.1   06 May 1996 12:01:10   FRED
// Dynamically configurable project support
// 
//    Rev 1.0   29 Jan 1996 16:53:34   JAY
// Variable configuration in INSTALL and PMW.
// 
//    Rev 1.0   19 Dec 1995 15:28:54   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:40   JAY
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:50   FRED
//  
// 
//    Rev 1.0   25 Oct 1995 08:23:02   JAY
// Initial revision.
*/

class CIniFile;

#ifndef __INIFILE_H__
#define __INIFILE_H__

class CIniFile
{
public:
/*
// Construction.
*/
	CIniFile();
	CIniFile(LPCSTR pFileName);

/*
// Get a list of key names for a section.
*/

	LPSTR GetSectionKeys(LPCSTR pszSection);

/*
// Name access.
*/

	void Name(LPCSTR pFileName);
	LPCSTR Name(void) const;

/*
// String manipulation.
*/

	CString GetString(LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszDefault = NULL);
	BOOL WriteString(LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszValue);

/*
// Integer manipulation.
*/

	int GetInteger(LPCSTR pszSection, LPCSTR pszEntry, int nDefault);
	BOOL WriteInteger(LPCSTR pszSection, LPCSTR pszEntry, int nValue);
	BOOL WriteInteger(LPCSTR pszSection, LPCSTR pszEntry, int nValue, int nDefault);

/*
// Unsigned integer manipulation.
*/

	UINT GetUnsignedInteger(LPCSTR pszSection, LPCSTR pszEntry, UINT uDefault);
	BOOL WriteUnsignedInteger(LPCSTR pszSection, LPCSTR pszEntry, UINT uValue);
	BOOL WriteUnsignedInteger(LPCSTR pszSection, LPCSTR pszEntry, UINT uValue, UINT uDefault);

/*
// Long manipulation.
*/

	long GetLong(LPCSTR pszSection, LPCSTR pszEntry, long lDefault);
	BOOL WriteLong(LPCSTR pszSection, LPCSTR pszEntry, long lValue);
	BOOL WriteLong(LPCSTR pszSection, LPCSTR pszEntry, long lValue, long lDefault);

/*
// Unsigned long (DWORD) manipulation.
*/

	DWORD GetDword(LPCSTR pszSection, LPCSTR pszEntry, DWORD dwDefault);
	BOOL WriteDword(LPCSTR pszSection, LPCSTR pszEntry, DWORD dwValue);
	BOOL WriteDword(LPCSTR pszSection, LPCSTR pszEntry, DWORD dwValue, DWORD dwDefault);

/*
// Entry removal.
*/

	BOOL RemoveEntry(LPCSTR pszSection, LPCSTR pszEntry);

/*
// Section removal.
*/

	BOOL RemoveSection(LPCSTR pszSection);

/*
// Window position save/restore.
*/

	BOOL GetWindowPosition(LPCSTR pszSection, CWnd* pWnd, BOOL* pfIsMaximized = NULL, LPRECT pRect = NULL);
	void WriteWindowPosition(LPCSTR pszSection, CWnd* pWnd);
	// Gets a string from an ini file for the registry like PrintShop
	BOOL AppendNamedApplicationRegistryPath(CString &RegPath);
protected:
	CString m_csName;
};

#endif					// #ifndef __INIFILE_H__
