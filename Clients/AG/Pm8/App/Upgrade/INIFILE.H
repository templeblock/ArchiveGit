/*
// $Workfile: INIFILE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/INIFILE.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:40   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:50   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:44   JAY
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
// Long manipulation.
*/

	long GetLong(LPCSTR pszSection, LPCSTR pszEntry, long lDefault);
	BOOL WriteLong(LPCSTR pszSection, LPCSTR pszEntry, long lValue);
	BOOL WriteLong(LPCSTR pszSection, LPCSTR pszEntry, long lValue, long lDefault);

/*
// Entry removal.
*/

	BOOL RemoveEntry(LPCSTR pszSection, LPCSTR pszEntry);

/*
// Section removal.
*/

	BOOL RemoveSection(LPCSTR pszSection);

protected:
	CString m_csName;
};

#endif					// #ifndef __INIFILE_H__
