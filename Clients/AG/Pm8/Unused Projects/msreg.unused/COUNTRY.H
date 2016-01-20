/*
// $Workfile: COUNTRY.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/COUNTRY.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   01 Oct 1996 10:38:14   FRED
//  
// 
//    Rev 1.1   10 Jul 1996 13:02:10   FRED
// Fixes to international changes
// 
//    Rev 1.0   28 Jun 1996 10:22:34   FRED
// Initial revision.
*/

class CNameList;
class CCountryNameList;
class CRegistrationOfficeData;
class CCountryRegistrationParameters;
class CCountryTableEntry;
class CCountryList;

#ifndef __COUNTRY_H__
#define __COUNTRY_H__

#include "inifile.h"

/////////////////////////////////////////////////////////////////////////////
// CStringList
// Utility string list class.

class CNameList : public CStringArray
{
public:
	CNameList(LPCSTR pszNames = NULL, char cSeparator = '|');
	virtual ~CNameList();
public:
	virtual void SetNames(LPCSTR pszNames, char cSeparator = '|');
protected:
	BOOL GetNextName(CString& csName, CString& csNames, char cSeparator);
};

/////////////////////////////////////////////////////////////////////////////
// CCountryNameList
// List of countries.

class CCountryNameList : public CNameList
{
public:
	CCountryNameList(LPCSTR pszCountries = NULL, char cSeparator = '|');
	virtual ~CCountryNameList();
public:
	virtual void SetNames(LPCSTR pszNames, char cSeparator = '|');
	BOOL FindCountry(LPCSTR pszCountry);

protected:
	BOOL m_fAllCountries;
};

/////////////////////////////////////////////////////////////////////////////
// CRegistrationOfficeData
// Base data for an office. Can be statically allocated or
// loaded from an INI file.

class CRegistrationOfficeData
{
public:
	LPSTR m_pszCountry;

public:
	LPSTR m_pszLocalCountries;
	LPSTR m_pszLocalMailAddress;
	LPSTR m_pszLocalModemNumber;
	BOOL m_fLocalModemFree;
	LPSTR m_pszLocalPhoneNumber;
	BOOL m_fLocalPhoneFree;
	LPSTR m_pszLocalFaxNumber;
	BOOL m_fLocalFaxFree;
	LPSTR m_pszLocalHelpNumber;
	BOOL m_fLocalHelpFree;

public:
	LPSTR m_pszForeignCountries;
	LPSTR m_pszForeignMailAddress;
	LPSTR m_pszForeignModemNumber;
	BOOL m_fForeignModemFree;
	LPSTR m_pszForeignPhoneNumber;
	BOOL m_fForeignPhoneFree;
	LPSTR m_pszForeignFaxNumber;
	BOOL m_fForeignFaxFree;
	LPSTR m_pszForeignHelpNumber;
	BOOL m_fForeignHelpFree;

public:
	BOOL Load(LPCSTR pszOfficeName, CIniFile& IniFile);
	void Free(void);
	LPSTR NewString(const CString& csString);
	void FreeString(LPSTR pszString);
};

/////////////////////////////////////////////////////////////////////////////
// CCountryRegistrationParameters
// Registration parameters for a given country.

class CCountryRegistrationParameters
{
public:
	CCountryRegistrationParameters();
	virtual ~CCountryRegistrationParameters();

public:
	// Create registration data. searches for best deal.
	static CCountryRegistrationParameters* Create(CCountryTableEntry* pCountry, CRegistrationOfficeData* pData = NULL);
	static CCountryRegistrationParameters* Create(CCountryTableEntry* pCountry, CIniFile& IniFile);
	static void GetWebSite(LPCSTR pszCountryName, CString& csWebSite);

public:
	CCountryTableEntry* m_pCountry;
	CCountryTableEntry* m_pOfficeCountry;
	BOOL m_fIsLocal;
	CNameList m_MailAddress;
	CString m_csModemNumber;
	CString m_csPhoneNumber;
	CString m_csFaxNumber;
	CString m_csHelpNumber;
	BOOL m_fModemFree;
	BOOL m_fPhoneFree;
	BOOL m_fFaxFree;
	BOOL m_fHelpFree;
	CString m_csDialingPrefix;
	CString m_csWebSite;

protected:
	// Data for default office (United States).
	static CRegistrationOfficeData DefaultOfficeData;

public:
	static CRegistrationOfficeData* GetDefaultOfficeData(void)
		{ return &DefaultOfficeData; }
};

/////////////////////////////////////////////////////////////////////////////
// CCountryTableEntry
// An entry in the country table.

class CCountryTableEntry
{
public:
	LPCSTR m_pszName;					// name
	LPCSTR m_pszCountryCode;		// "to" code
	LPCSTR m_pszDialingPrefix;		// "from" code
public:
	LPCSTR GetName(void)
		{ return m_pszName; }
	LPCSTR GetCountryCode(void)
		{ return m_pszCountryCode; }
	LPCSTR GetDialingPrefix(void)
		{ return m_pszDialingPrefix; }

public:
	static CCountryTableEntry* GetCountryEntryByName(LPCSTR pszName);
	BOOL IsUnitedStatesLocal(void);
	CCountryRegistrationParameters* CreateRegistrationParameters(CIniFile& IniFile);
};

/////////////////////////////////////////////////////////////////////////////
// CCountryList Combo Box

class CCountryList : public CComboBox
{
public:
	CCountryList();
	virtual ~CCountryList();

protected:
	// Generated message map functions
	//{{AFX_MSG(CCountryList)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Fill(LPCSTR pszInitialCountry = NULL);
	CCountryTableEntry* GetCountryTableEntry(int nIndex = -1);
	BOOL SelectByName(LPCSTR pszName);
	void ReturnGoesToNextControl(BOOL fReturnGoesToNextControl)
		{ m_fReturnGoesToNextControl = fReturnGoesToNextControl; }

protected:
	BOOL m_fReturnGoesToNextControl;
};

#endif
