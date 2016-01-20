/*
// $Workfile: INTLDLL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/INTLDLL.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:16   Fred
// Initial revision.
// 
//    Rev 1.0   09 Jul 1997 10:16:48   Fred
// Initial revision.
// 
//    Rev 1.3   07 Oct 1996 14:03:24   FRED
// Added GetTaxInformation()
// 
//    Rev 1.2   04 Oct 1996 15:06:54   FRED
// Added GetWebSite()
// 
//    Rev 1.1   04 Oct 1996 10:05:42   FRED
// Added GetCountryName(); Country Id's to data
// 
//    Rev 1.0   03 Oct 1996 10:31:04   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "intldll.h"

/////////////////////////////////////////////////////////////////////////////
// Type definitions for DLL entry points.

typedef BOOL (FAR PASCAL * PFN_GetInternationalData)(WORD wCountryId, LPCSTR pszIniFile, CInternationalData* pInternationalData);
typedef BOOL (FAR PASCAL * PFN_FillCountryComboBox)(HWND hComboBox, WORD wSelectCountryId = 0);
typedef int (FAR PASCAL * PFN_GetCountryName)(WORD wCountryId, LPSTR pszCountryName = NULL);
typedef int (FAR PASCAL * PFN_GetWebSite)(WORD wCountryId, LPCSTR pszIniFile, LPSTR pszWebSite = NULL);
typedef int (FAR PASCAL * PFN_GetTaxInformation)(LPCSTR pszIniFile, LPSTR pszTaxInformation = NULL);

/////////////////////////////////////////////////////////////////////////////
// CInternationalInformationDll
// This class which wraps INTLxx.DLL.

CInternationalInformationDll::CInternationalInformationDll(LPCSTR pszDirectory)
{
	TRY
	{
		m_csDirectory = pszDirectory;
	}
	END_TRY

	m_hDll = NULL;
}

CInternationalInformationDll::~CInternationalInformationDll()
{
	Close();
}

BOOL CInternationalInformationDll::GetInternationalData(WORD wCountryId, LPCSTR pszIniFile, CInternationalData* pInternationalData)
{
	BOOL fResult = FALSE;

	if (Open())
	{
		PFN_GetInternationalData pfnGetInternationalData = (PFN_GetInternationalData)GetProcAddress(m_hDll, "GetInternationalData");
		if (pfnGetInternationalData != NULL)
		{
			fResult = (*pfnGetInternationalData)(wCountryId, pszIniFile, pInternationalData);
		}
	}

	return fResult;
}

BOOL CInternationalInformationDll::FillCountryComboBox(HWND hComboBox, WORD wSelectCountryId /*=0*/)
{
	BOOL fResult = FALSE;

	if (Open())
	{
		PFN_FillCountryComboBox pfnFillCountryComboBox = (PFN_FillCountryComboBox)GetProcAddress(m_hDll, "FillCountryComboBox");
		if (pfnFillCountryComboBox != NULL)
		{
			fResult = (*pfnFillCountryComboBox)(hComboBox, wSelectCountryId);
		}
	}

	return fResult;
}

BOOL CInternationalInformationDll::GetCountryName(WORD wCountryId, CString& csCountryName)
{
	csCountryName.Empty();

	if (Open())
	{
		PFN_GetCountryName pfnGetCountryName = (PFN_GetCountryName)GetProcAddress(m_hDll, "GetCountryName");
		if (pfnGetCountryName != NULL)
		{
			TRY
			{
				int nLength = (*pfnGetCountryName)(wCountryId);
				if (nLength > 0)
				{
					(*pfnGetCountryName)(wCountryId, csCountryName.GetBuffer(nLength));
				}
				csCountryName.ReleaseBuffer();
			}
			END_TRY
		}
	}

	return !csCountryName.IsEmpty();
}

BOOL CInternationalInformationDll::GetWebSite(WORD wCountryId, LPCSTR pszIniFile, CString& csWebSite)
{
	csWebSite.Empty();

	if (Open())
	{
		PFN_GetWebSite pfnGetWebSite = (PFN_GetWebSite)GetProcAddress(m_hDll, "GetWebSite");
		if (pfnGetWebSite != NULL)
		{
			TRY
			{
				int nLength = (*pfnGetWebSite)(wCountryId, pszIniFile);
				if (nLength > 0)
				{
					(*pfnGetWebSite)(wCountryId, pszIniFile, csWebSite.GetBuffer(nLength));
				}
				csWebSite.ReleaseBuffer();
			}
			END_TRY
		}
	}

	return !csWebSite.IsEmpty();
}

BOOL CInternationalInformationDll::GetTaxInformation(LPCSTR pszIniFile, CString& csTaxInformation)
{
	csTaxInformation.Empty();

	if (Open())
	{
		PFN_GetTaxInformation pfnGetTaxInformation = (PFN_GetTaxInformation)GetProcAddress(m_hDll, "GetTaxInformation");
		if (pfnGetTaxInformation != NULL)
		{
			TRY
			{
				int nLength = (*pfnGetTaxInformation)(pszIniFile);
				if (nLength > 0)
				{
					(*pfnGetTaxInformation)(pszIniFile, csTaxInformation.GetBuffer(nLength));
				}
				csTaxInformation.ReleaseBuffer();
			}
			END_TRY
		}
	}

	return !csTaxInformation.IsEmpty();
}

BOOL CInternationalInformationDll::Open(void)
{
	BOOL fResult = FALSE;

	if (m_hDll != NULL)
	{
		fResult = TRUE;
	}
	else
	{
		CString csDll;
		TRY
		{
			csDll = m_csDirectory;
			if (!csDll.IsEmpty() && (csDll[csDll.GetLength()-1] != '\\'))
			{
				csDll += '\\';
			}
#ifdef WIN32
			csDll += "INTL32.DLL";
			if ((m_hDll = LoadLibrary(csDll)) != NULL)
			{
				fResult = TRUE;
			}
#else
			csDll += "INTL16.DLL";
			if ((m_hDll = LoadLibrary(csDll)) < HINSTANCE_ERROR)
			{
				m_hDll = NULL;
			}
			else
			{
				fResult = TRUE;
			}
#endif
		}
		CATCH_ALL(e)
		{
			fResult = FALSE;
		}
		END_CATCH_ALL
	}

	return fResult;
}

void CInternationalInformationDll::Close(void)
{
	if (m_hDll != NULL)
	{
		FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
}
