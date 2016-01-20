//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/USERSET.CPP 1     3/03/99 6:13p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/USERSET.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 3     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     9/15/98 2:49p Jayn
// Conversion of DriveManager to COM format.
// Added UserSettings.
// 
// 1     9/15/98 11:45a Jayn
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pmw.h"
#include "userset.h"
#include "inifile.h"

/////////////////////////////////////////////////////////////////////////////
// Interface map for CPMUserSettings.

BEGIN_INTERFACE_MAP(CPMUserSettings, CCmdTarget)
	INTERFACE_PART(CPMUserSettings, IID_IUserSettings, UserSettings)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IUnknown interface for CPMUserSettings.
// This is standard stuff brought over from the docs.

STDMETHODIMP_(ULONG) CPMUserSettings::XUserSettings::AddRef()
{
	METHOD_PROLOGUE(CPMUserSettings, UserSettings)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CPMUserSettings::XUserSettings::Release()
{
	METHOD_PROLOGUE(CPMUserSettings, UserSettings)
	return pThis->ExternalRelease();
}

STDMETHODIMP CPMUserSettings::XUserSettings::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
	METHOD_PROLOGUE(CPMUserSettings, UserSettings)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IUserSettings interface for CPMUserSettings.

//
// Read a string from user settings.
// pszDefault can be NULL (which defaults to the empty string).
//

STDMETHODIMP_(CString) CPMUserSettings::XUserSettings::GetString(LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszDefault, int nSettingType)
{
	switch (nSettingType)
	{
		default:
		{
			// Not a good value for setting.
			ASSERT(FALSE);
			// Fall through to...
		}
		case IUserSettings::AnySetting:
		{
			// Return either (user overrides global).
			return GET_PMWAPP()->OurGetProfileString(pszSection, pszEntry, pszDefault);
		}
		case IUserSettings::UserSetting:
		{
			return AfxGetApp()->GetProfileString(pszSection, pszEntry, pszDefault);
		}
		case IUserSettings::GlobalSetting:
		{
			CIniFile IniFile(GET_PMWAPP()->m_csMainIni);
			return IniFile.GetString(pszSection, pszEntry, pszDefault);
		}
	}
}

//
// Write a string to user settings.
//

STDMETHODIMP_(void) CPMUserSettings::XUserSettings::WriteString(LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszString, int nSettingType)
{
	switch (nSettingType)
	{
		default:
		{
			// Not a valid value for writing.
			ASSERT(FALSE);
			// Fall through to...
		}
		case IUserSettings::UserSetting:
		{
			AfxGetApp()->WriteProfileString(pszSection, pszEntry, pszString);
			break;
		}
		case IUserSettings::GlobalSetting:
		{
			CIniFile IniFile(GET_PMWAPP()->m_csMainIni);
			IniFile.WriteString(pszSection, pszEntry, pszString);
			break;
		}
	}
}

//
// Read an int from user settings.
//

STDMETHODIMP_(int) CPMUserSettings::XUserSettings::GetInteger(LPCSTR pszSection, LPCSTR pszEntry, int nDefault, int nSettingType)
{
	switch (nSettingType)
	{
		default:
		{
			// Not a good value for setting.
			ASSERT(FALSE);
			// Fall through to...
		}
		case IUserSettings::AnySetting:
		{
			// Return either (user overrides global).
			return GET_PMWAPP()->OurGetProfileInt(pszSection, pszEntry, nDefault);
		}
		case IUserSettings::UserSetting:
		{
			return AfxGetApp()->GetProfileInt(pszSection, pszEntry, nDefault);
		}
		case IUserSettings::GlobalSetting:
		{
			CIniFile IniFile(GET_PMWAPP()->m_csMainIni);
			return IniFile.GetInteger(pszSection, pszEntry, nDefault);
		}
	}
}

//
// Write an int to user settings.
//

STDMETHODIMP_(void) CPMUserSettings::XUserSettings::WriteInteger(LPCSTR pszSection, LPCSTR pszEntry, int nValue, int nSettingType)
{
	switch (nSettingType)
	{
		default:
		{
			// Not a valid value for writing.
			ASSERT(FALSE);
			// Fall through to...
		}
		case IUserSettings::UserSetting:
		{
			AfxGetApp()->WriteProfileInt(pszSection, pszEntry, nValue);
			break;
		}
		case IUserSettings::GlobalSetting:
		{
			CIniFile IniFile(GET_PMWAPP()->m_csMainIni);
			IniFile.WriteInteger(pszSection, pszEntry, nValue);
			break;
		}
	}
}
