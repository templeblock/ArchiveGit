/*
// $Workfile: INIFILE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/INIFILE.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   01 Oct 1996 10:39:52   FRED
//  
// 
//    Rev 1.1   26 Jun 1996 07:03:22   FRED
// International Registration
// 
//    Rev 1.0   22 Nov 1995 08:51:50   FRED
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
