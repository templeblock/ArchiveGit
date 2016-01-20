/*
// $Workfile: INTL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/intl/INTL.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/14/98 4:07p Jayn
// 
//    Rev 1.3   07 Oct 1996 14:03:26   FRED
// Added GetTaxInformation()
// 
//    Rev 1.2   04 Oct 1996 15:06:54   FRED
// Added GetWebSite()
// 
//    Rev 1.1   04 Oct 1996 10:03:52   FRED
// Added GetCountryName(); Country Id's to data
// 
//    Rev 1.0   03 Oct 1996 10:31:04   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "intl.h"
#include "country.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIntlApp

BEGIN_MESSAGE_MAP(CIntlApp, CWinApp)
	//{{AFX_MSG_MAP(CIntlApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntlApp construction

CIntlApp::CIntlApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// GetInternationalData Entry Point

#ifdef WIN32
extern "C" _declspec(dllexport) BOOL FAR PASCAL GetInternationalData(WORD wCountryId, LPCSTR pszIniFile, CInternationalData* pInternationalData)
#else
extern "C" BOOL FAR PASCAL _export GetInternationalData(WORD wCountryId, LPCSTR pszIniFile, CInternationalData* pInternationalData)
#endif
{
	return CInternationalInformation::GetInternationalData(wCountryId, pszIniFile, pInternationalData);
}

/////////////////////////////////////////////////////////////////////////////
// FillCountryComboBox Entry Point

#ifdef WIN32
extern "C" _declspec(dllexport) BOOL FAR PASCAL FillCountryComboBox(HWND hComboBox, WORD wSelectCountryId /*=0*/)
#else
extern "C" BOOL FAR PASCAL _export FillCountryComboBox(HWND hComboBox, WORD wSelectCountryId /*=0*/)
#endif
{
	BOOL fResult = FALSE;

	// Fill the specified combo box with the country names from the resource file.
	// The data for each item is set to the country ID number.

	CComboBox ComboBox;
	CString csCountry;

	TRY
	{
		ComboBox.Attach(hComboBox);

		ComboBox.SetRedraw(FALSE);

		if (wSelectCountryId == 0)
		{
			wSelectCountryId = ID_UnitedStatesOfAmerica;
		}

		int nSelectIndex = 0;

		int nCount = CCountryTableEntry::GetCountryTableEntryCount();
		for (int nCountry = 0; nCountry < nCount; nCountry++)
		{
			CCountryTableEntry* pCountry = CCountryTableEntry::GetCountryEntryByIndex(nCountry);
			if (pCountry != NULL)
			{
				csCountry.LoadString(pCountry->GetId());
				int nComboIndex = ComboBox.AddString(csCountry);
				if (nComboIndex != CB_ERR)
				{
					ComboBox.SetItemData(nComboIndex, (DWORD)(pCountry->GetId()));
					if (pCountry->GetId() == wSelectCountryId)
					{
						nSelectIndex = nComboIndex;
					}
				}
			}
		}

		if (nSelectIndex != 0)
		{
			ComboBox.SetCurSel(nSelectIndex);
		}

		ComboBox.SetRedraw(TRUE);
		ComboBox.Invalidate();

		ComboBox.Detach();

		fResult = TRUE;
	}
	CATCH_ALL(e)
	{
		if (ComboBox.m_hWnd != NULL)
		{
			ComboBox.SetRedraw(TRUE);
			ComboBox.Detach();
		}

		fResult = FALSE;
	}
	END_CATCH_ALL

	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// GetCountryName Entry Point

#ifdef WIN32
extern "C" _declspec(dllexport) int FAR PASCAL GetCountryName(WORD wCountryId, LPSTR pszCountryName /*=NULL*/)
#else
extern "C" int FAR PASCAL _export GetCountryName(WORD wCountryId, LPSTR pszCountryName /*=NULL*/)
#endif
{
	int nResult = 0;

	CString csName;
	TRY
	{
		// Load the country name.
		csName.LoadString(wCountryId);

		if (pszCountryName == NULL)
		{
			// Return required length (including NULL).
			nResult = csName.GetLength()+1;
		}
		else
		{
			// Assume buffer is large enough for name plus NULL.
			strcpy(pszCountryName, csName);
		}
	}
	END_TRY

	return nResult;
}

/////////////////////////////////////////////////////////////////////////////
// GetWebSite Entry Point

#ifdef WIN32
extern "C" _declspec(dllexport) int FAR PASCAL GetWebSite(WORD wCountryId, LPCSTR pszIniFile, LPSTR pszWebSite /*=NULL*/)
#else
extern "C" int FAR PASCAL _export GetWebSite(WORD wCountryId, LPCSTR pszIniFile, LPSTR pszWebSite /*=NULL*/)
#endif
{
	int nResult = 0;

	CString csWebSite;
	TRY
	{
		// Load the web site name.
		CInternationalInformation::GetWebSite(wCountryId, pszIniFile, csWebSite);

		if (pszWebSite== NULL)
		{
			// Return required length (including NULL).
			nResult = csWebSite.GetLength()+1;
		}
		else
		{
			// Assume buffer is large enough for name plus NULL.
			strcpy(pszWebSite, csWebSite);
		}
	}
	END_TRY

	return nResult;
}

/////////////////////////////////////////////////////////////////////////////
// GetTaxInformation Entry Point

#ifdef WIN32
extern "C" _declspec(dllexport) int FAR PASCAL GetTaxInformation(LPCSTR pszIniFile, LPSTR pszTaxInformation /*=NULL*/)
#else
extern "C" int FAR PASCAL _export GetTaxInformation(LPCSTR pszIniFile, LPSTR pszTaxInformation /*=NULL*/)
#endif
{
	int nResult = 0;

	CString csTaxInformation;
	TRY
	{
		// Load the tax information string.
		CInternationalInformation::GetTaxInformation(pszIniFile, csTaxInformation);

		if (pszTaxInformation == NULL)
		{
			// Return required length (including NULL).
			nResult = csTaxInformation.GetLength()+1;
		}
		else
		{
			// Assume buffer is large enough for name plus NULL.
			strcpy(pszTaxInformation, csTaxInformation);
		}
	}
	END_TRY

	return nResult;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIntlApp object

CIntlApp theApp;
