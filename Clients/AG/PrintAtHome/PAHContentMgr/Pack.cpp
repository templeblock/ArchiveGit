// Pack.cpp : Implementation of CPack

#include "stdafx.h"
#include "Pack.h"
#include "Repository.h"
#include "TestDlg.h"



// CPack
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CPack::ProcessPurchase(BSTR szPackUrl)
{
	// Test Code
	if (CString(szPackUrl).IsEmpty())
	{
		CTestDlg TestDlg;
		if (TestDlg.DoModal() != IDOK)
			return S_OK;
		szPackUrl = CComBSTR(TestDlg.m_szText);
	}
	

	CRepository Repository;
	CString szURL(szPackUrl);
	if (szURL.IsEmpty())
	{
		::MessageBox(NULL, _T("Url to XML file is invalid!"), _T("Error"), MB_OK|MB_ICONSTOP);
		return S_OK;
	}

	if (!Repository.LoadXML(szURL))
	{
		::MessageBox(NULL, _T("Loading XML failed!"), _T("Error"), MB_OK|MB_ICONSTOP);
		return S_OK;
	}

	if (!Repository.ProcessXMLPack())
	{
		::MessageBox(NULL, _T("Processing XML failed!"), _T("Error"), MB_OK|MB_ICONSTOP);
		return S_OK;
	}

	if (!Repository.DownloadFiles())
	{
		::MessageBox(NULL, _T("Downloading files failed!"), _T("Error"), MB_OK|MB_ICONSTOP);
		return S_OK;
	}

	::MessageBox(NULL, _T("Please wait. Download in progress!"), _T("PAH"), MB_OK);
	
	return S_OK;
}
