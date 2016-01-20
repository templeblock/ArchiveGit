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
// $Log: /PM8/msreg/intl/COUNTRY.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/07/98 4:20p Jayn
// 
//    Rev 1.3   07 Oct 1996 14:03:22   FRED
// Added GetTaxInformation()
// 
//    Rev 1.2   04 Oct 1996 15:06:56   FRED
// Added GetWebSite()
// 
//    Rev 1.1   04 Oct 1996 10:03:50   FRED
// Added GetCountryName(); Country Id's to data
// 
//    Rev 1.0   03 Oct 1996 10:31:00   FRED
// Initial revision.
*/

class CNameList;
class CCountryList;
class CCountryTableEntry;
class CInternationalOfficeData;
class CInternationalInformation;

#ifndef __COUNTRY_H__
#define __COUNTRY_H__

#include "inifile.h"

/////////////////////////////////////////////////////////////////////////////
// CNameList

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
// CCountryList

class CCountryList : public CNameList
{
public:
	CCountryList(LPCSTR pszCountries = NULL, char cSeparator = '|');
	virtual ~CCountryList();
public:
	virtual void SetNames(LPCSTR pszNames, char cSeparator = '|');
	BOOL FindCountry(WORD wId);
	BOOL FindCountry(LPCSTR pszId);

protected:
	BOOL m_fAllCountries;
};

/////////////////////////////////////////////////////////////////////////////
// CCountryTableEntry
// An entry in the country table.

class CCountryTableEntry
{
public:
	WORD m_wCountry;
	LPCSTR m_pszCountryCode;		// "to" code
	LPCSTR m_pszDialingPrefix;		// "from" code

public:
	WORD GetId(void)
		{ return m_wCountry; }
	LPCSTR GetCountryCode(void)
		{ return m_pszCountryCode; }
	LPCSTR GetDialingPrefix(void)
		{ return m_pszDialingPrefix; }

public:
	static int GetCountryTableEntryCount(void);
	static CCountryTableEntry* GetCountryEntryByIndex(int nIndex);
	static CCountryTableEntry* GetCountryEntryById(WORD wId);
	static CCountryTableEntry* GetCountryEntryById(LPCSTR pszId);
	BOOL IsUnitedStatesLocal(void);
};

/////////////////////////////////////////////////////////////////////////////
// CInternationalOfficeData

class CInternationalOfficeData
{
public:
	// Structure to hold information on a phone number.
	struct PhoneNumber
	{
		LPSTR m_pszNumber;
		BOOL m_fFree;
		void Clear(void);
		void Free(void);
		void Load(LPCSTR pszFullOfficeName, CIniFile& IniFile, LPCSTR pszPrefix);
		LPCSTR GetNumber(void)
			{ return m_pszNumber; }
		BOOL Exists(void)
			{ return m_pszNumber != NULL; }
		BOOL IsFree(void)
			{ return m_fFree; }
	};

	// Structure to hold registration information for a specific set of countries.
	struct CountryRegistration
	{
		LPSTR m_pszCountries;
		PhoneNumber m_ModemNumber;
		PhoneNumber m_PhoneNumber;
		PhoneNumber m_FaxNumber;
		PhoneNumber m_HelpNumber;
		LPSTR m_pszMailAddress;
		void Clear(void);
		void Free(void);
		void Load(LPCSTR pszFullOfficeName, CIniFile& IniFile, LPCSTR pszPrefix);
	};

	// Structure to hold local and foreign registration information.
	struct Registration
	{
		CountryRegistration m_Local;
		CountryRegistration m_Foreign;
		void Clear(void);
		void Free(void);
		void Load(LPCSTR pszFullOfficeName, CIniFile& IniFile, LPCSTR pszPrefix);
	};

	// Structure to hold instant sales information for a specific set of countries.
	struct CountryInstantSales
	{
		LPSTR m_pszCountries;
		PhoneNumber m_ModemNumber;
		PhoneNumber m_PhoneNumber;
		PhoneNumber m_FaxNumber;
		PhoneNumber m_HelpNumber;
		LPSTR m_pszMailAddress;
		void Clear(void);
		void Free(void);
		void Load(LPCSTR pszFullOfficeName, CIniFile& IniFile, LPCSTR pszPrefix);
	};

	// Structure to hold local and foreign instant sales information.
	struct InstantSales
	{
		CountryInstantSales m_Local;
		CountryInstantSales m_Foreign;
		void Clear(void);
		void Free(void);
		void Load(LPCSTR pszFullOfficeName, CIniFile& IniFile, LPCSTR pszPrefix);
	};

public:
	// Name of office.
	LPSTR m_pszOffice;

	// Country containing this office.
	LPSTR m_pszCountry;

	// Tax information for office.
	LPSTR m_pszTax;

	// Registration information.
	Registration m_Registration;

	// Instant Sales information.
	InstantSales m_InstantSales;

public:
	void Clear(void);
	BOOL Load(LPCSTR pszOfficeName, CIniFile& IniFile);
	void Free(void);
};

/////////////////////////////////////////////////////////////////////////////
// CInternationalInformation

class CInternationalInformation
{
public:
	struct PhoneNumber
	{
		CString m_csOffice;
		WORD m_wCountry;
		BOOL m_fUnitedStatesLocal;
		BOOL m_fLocal;
		BOOL m_fFree;
		CString m_csNumber;
		CString m_csPrefix;
		BOOL Exists(void)
			{ return !m_csNumber.IsEmpty(); }
		void Clear(void);
		void Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, CInternationalOfficeData::PhoneNumber* pData, BOOL fLocal);
		void Copy(CInternationalData::PhoneNumber* pData);
	};

	struct MailAddress
	{
		CString m_csOffice;
		WORD m_wCountry;
		BOOL m_fUnitedStatesLocal;
		BOOL m_fLocal;
		CString m_csAddress;
		BOOL Exists(void)
			{ return !m_csAddress.IsEmpty(); }
		void Clear(void);
		void Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, LPCSTR pData, BOOL fLocal);
		void Copy(CInternationalData::MailAddress* pData);
	};

	struct Registration
	{
		PhoneNumber m_Modem;
		PhoneNumber m_Phone;
		PhoneNumber m_Fax;
		PhoneNumber m_Help;
		MailAddress m_Mail;
		void Clear(void);
		void Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, CInternationalOfficeData::Registration* pData);
		void Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, CInternationalOfficeData::CountryRegistration* pData, BOOL fLocal);
		void Copy(CInternationalData::Registration* pData);
	};

	struct InstantSales
	{
		PhoneNumber m_Modem;
		PhoneNumber m_Phone;
		PhoneNumber m_Fax;
		PhoneNumber m_Help;
		MailAddress m_Mail;
		void Clear(void);
		void Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, CInternationalOfficeData::InstantSales* pData);
		void Update(CCountryTableEntry* pCountry, CCountryTableEntry* pOfficeCountry, CInternationalOfficeData* pOfficeData, CInternationalOfficeData::CountryInstantSales* pData, BOOL fLocal);
		void Copy(CInternationalData::InstantSales* pData);
	};

public:
	CInternationalInformation();
	virtual ~CInternationalInformation();

public:
	static BOOL GetInternationalData(WORD wCountryId, LPCSTR pszIniFile, CInternationalData* pInternationalData);
	static BOOL GetWebSite(WORD wCountryId, LPCSTR pszIniFile, CString& csWebSite);
	static BOOL GetTaxInformation(LPCSTR pszIniFile, CString& csTaxInformation);
	void Update(CCountryTableEntry* pCountry, CInternationalOfficeData* pOfficeData);
	void Copy(CInternationalData* pData);

public:
	// International data.
	WORD m_wCountry;
	BOOL m_fUnitedStatesLocal;
	Registration m_Registration;
	InstantSales m_InstantSales;
	CString m_csWebSite;
	CString m_csTaxInformation;

protected:
	// Data for default office (United States).
	static CInternationalOfficeData DefaultOfficeData;

public:
	static CInternationalOfficeData* GetDefaultOfficeData(void)
		{ return &DefaultOfficeData; }
};

#endif
