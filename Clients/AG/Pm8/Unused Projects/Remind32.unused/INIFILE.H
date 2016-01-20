/*
// $Workfile: INIFILE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/INIFILE.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:52   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:22   Fred
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

protected:
	CString m_csName;
};

#endif					// #ifndef __INIFILE_H__
