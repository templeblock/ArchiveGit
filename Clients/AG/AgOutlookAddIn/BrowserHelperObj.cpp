// BrowserHelperObj.cpp : Implementation of CBrowserHelperObj

#include "stdafx.h"
#include "BrowserHelperObj.h"
#include "JMExplorerBand.h"



// CBrowserHelperObj
CBrowserHelperObj::CBrowserHelperObj()
{
	m_spWebBrowser2 = NULL;
	m_bVisible = false;
	m_bLoaded = false;
}
CBrowserHelperObj::~CBrowserHelperObj()
{
	m_spWebBrowser2 = NULL;
}
/**************************************************************************

   CBrowserHelperObj::SetSite()
   
**************************************************************************/
STDMETHODIMP CBrowserHelperObj::SetSite(IUnknown *punkSite)
{
    HRESULT hr=S_OK;

    //If punkSite is not NULL, a new site is being set.
    if(punkSite)
    {
		if (m_spWebBrowser2)
			m_spWebBrowser2 = NULL;
		
		// Retrieve and store the IWebBrowser2 pointer 
		hr = punkSite->QueryInterface(IID_IWebBrowser2, (LPVOID*)&m_spWebBrowser2);
		if (m_spWebBrowser2 == NULL)
			return E_INVALIDARG;

		
	/*	VARIANT_BOOL bVisible = false;
		hr = m_spWebBrowser2->get_Visible(&bVisible);
		if (!bVisible)
			return S_OK;*/

		if (!m_bLoaded)
		{
			// Load explorer bar
			CComVariant varCLSID = CComVariant(CLSID_JMEXPLORERBAND_STRING);
			CComVariant varShow = CComVariant(true);
			CComVariant varSize = VT_NULL;
			hr = m_spWebBrowser2->ShowBrowserBar(&varCLSID, &varShow, &varSize);
			Sleep(300);
			m_spWebBrowser2->put_Visible(false);
			varShow = CComVariant(false);
			hr = m_spWebBrowser2->ShowBrowserBar(&varCLSID, &varShow, &varSize);
			if (SUCCEEDED(hr))
				m_bLoaded = true;
		}
		//m_spWebBrowser2->put_Visible(true);
		

    }
	
    return hr;
}
