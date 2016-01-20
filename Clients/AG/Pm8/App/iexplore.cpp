/*
// $Workfile: iexplore.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/iexplore.cpp $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 3     7/16/98 2:51p Dennis
// Fix for detecting if internet explorer is installed.
// 
// 2     3/26/98 10:29a Fredf
// Changes to install program to install Internet Explorer 4.0 and the Web
// Publishing Wizard.
*/

#include <stdafx.h>
#include "iexplore.h"
#include "util.h"

// Helper function that gets browser version from SHDOCVW.DLL
// Code lifted from: http://www/microsoft.com/msdn/sdk/inetsdk/help/itt/ieprog/licensing.htm

typedef struct _DllVersionInfo
{
	DWORD cbSize; 
	DWORD dwMajorVersion;		// Major version 
	DWORD dwMinorVersion;		// Minor version 
	DWORD dwBuildNumber;			// Build number 
	DWORD dwPlatformID;			// DLLVER_PLATFORM_* 
} DLLVERSIONINFO;	// Platform IDs for DLLVERSIONINFO 

#define DLLVER_PLATFORM_WINDOWS		0x00000001      // Windows 95 
#define DLLVER_PLATFORM_NT				0x00000002      // Windows NT

typedef HRESULT (CALLBACK* DLLGETVERSIONPROC)(DLLVERSIONINFO *);  

HRESULT GetBrowserVersion(LPDWORD pdwMajor, LPDWORD pdwMinor, LPDWORD pdwBuild, LPDWORD pdwSubBuild)
{
	*pdwMajor = 0;
	*pdwMinor = 0;
	*pdwBuild = 0;
	*pdwSubBuild = 0;

	HINSTANCE hBrowser;

	if (IsBadWritePtr(pdwMajor, sizeof(DWORD))
	 || IsBadWritePtr(pdwMinor, sizeof(DWORD))
	 || IsBadWritePtr(pdwBuild, sizeof(DWORD))
	 || IsBadWritePtr(pdwSubBuild, sizeof(DWORD)))
	{
		return E_INVALIDARG;
	}
	
	// Load the DLL.
	hBrowser = LoadLibrary(TEXT("shdocvw.dll"));
	if (hBrowser)
	{
		HRESULT hr = S_OK;

      // You must get this function explicitly.
		DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hBrowser, TEXT("DllGetVersion"));
      if (pDllGetVersion != NULL)
		{
			DLLVERSIONINFO dvi;
			ZeroMemory(&dvi, sizeof(dvi));
			dvi.cbSize = sizeof(dvi);

			hr = (*pDllGetVersion)(&dvi);
			if (SUCCEEDED(hr))
			{
	         *pdwMajor = dvi.dwMajorVersion;
				*pdwMinor = dvi.dwMinorVersion;
				*pdwBuild = dvi.dwBuildNumber;
				*pdwSubBuild = 0;
         }
		}
		else
		{
			// if GetProcAddress failed, there is a problem with the DLL
	      hr = E_FAIL;
		}
		
		FreeLibrary(hBrowser);
		return hr;
	}

	return E_FAIL;
}

///////////////////////////////////////////////////////////////////////////
// CInternetExplorerVersion

CInternetExplorerVersion::CInternetExplorerVersion()
{
	Invalidate();
}

CInternetExplorerVersion::~CInternetExplorerVersion()
{
}

void CInternetExplorerVersion::Invalidate(void)
{
	m_uMajorVersion = 0;
	m_uMinorVersion = 0;
	m_uBuild = 0;
	m_uSubBuild = 0;
}

CString CInternetExplorerVersion::GetVersionString(void)
{
	CString csVersion;
	csVersion.Format("%u.%u.%u.%u",
		GetMajorVersion(),
		GetMinorVersion(),
		GetBuild(),
		GetSubBuild());
	return csVersion;
}

BOOL CInternetExplorerVersion::IsVersion3(void) const
{
	BOOL fResult = FALSE;

	if (IsValid())
	{
		fResult = (GetMajorVersion() == 4) && (GetMinorVersion() == 70);
	}

	return fResult;
}

BOOL CInternetExplorerVersion::IsVersion4(void) const
{
	BOOL fResult = FALSE;

	if (IsValid())
	{
		fResult = (GetMajorVersion() == 4) && (GetMinorVersion() >= 71);
	}

	return fResult;
}

///////////////////////////////////////////////////////////////////////////
// CInternetExplorerHelper

CInternetExplorerHelper::CInternetExplorerHelper()
{
}

CInternetExplorerHelper::~CInternetExplorerHelper()
{
}

const CInternetExplorerVersion& CInternetExplorerHelper::GetVersion(BOOL fRefresh /*=FALSE*/)
{
	if (!m_Version.IsValid() || fRefresh)
	{
		m_Version.Invalidate();

		UINT uMajorVersion = 0;
		UINT uMinorVersion = 0;
		UINT uBuild = 0;
		UINT uSubBuild = 0;

		CString csVersion, csBuild;

      // If Build Key does not exist, Explorer is not installed
		if (!Util::GetRegistryString(
				HKEY_LOCAL_MACHINE,
				"Software\\Microsoft\\Internet Explorer",
				"Build",
				csBuild))
            {
               return m_Version;
            }

		if (Util::GetRegistryString(
				HKEY_LOCAL_MACHINE,
				"Software\\Microsoft\\Internet Explorer",
				"Version",
				csVersion))
		{
			sscanf(csVersion, "%u.%u.%u.%u", &uMajorVersion, &uMinorVersion, &uBuild, &uSubBuild);
		}
		else
		{
			DWORD dwMajorVersion;
			DWORD dwMinorVersion;
			DWORD dwBuild;
			DWORD dwSubBuild;
			HRESULT hResult = GetBrowserVersion(&dwMajorVersion, &dwMinorVersion, &dwBuild, &dwSubBuild);
			if (SUCCEEDED(hResult))
			{
				uMajorVersion = dwMajorVersion;
				uMinorVersion = dwMinorVersion;
				uBuild = dwBuild;
				uSubBuild = dwSubBuild;
			}
		}

		m_Version.SetMajorVersion(uMajorVersion);
		m_Version.SetMinorVersion(uMinorVersion);
		m_Version.SetBuild(uBuild);
		m_Version.SetSubBuild(uSubBuild);
	}

	TRACE("Explorer Version = %s\n", (LPCSTR)m_Version.GetVersionString());

	return m_Version;
}
