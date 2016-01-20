// Broker.cpp : Implementation of CBroker

#include "stdafx.h"
#include "Broker.h"
#include "WebPage.h"


// CBroker

void CBroker::PluginLoaded(VARIANT varValue)
{
	if (m_fnCallback)
		m_fnCallback(m_wParam, m_lParam, varValue.pdispVal);

}
/////////////////////////////////////////////////////////////////////////////
bool CBroker::Initialize(FNBROKERCALLBACK fnCallback, LPARAM lParam, WPARAM wParam)
{
	m_fnCallback = fnCallback;
	m_lParam = lParam;
	m_wParam = wParam;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBroker::SetDocument(IDispatch* pDisp)
{
	CComQIPtr<IHTMLDocument2> spDoc(pDisp);
	if (NULL != spDoc)
		m_spDoc = spDoc;

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CBroker::CallJavaScript(VARIANT varFunctionName, VARIANT varArg1, VARIANT_BOOL* bSuccess)
{
	*bSuccess = false;
	CWebPage WebPage;
	if (!m_spDoc)
		return S_OK;

	if (!WebPage.SetDocument(m_spDoc))
		return S_OK;

	if (CString(varArg1.bstrVal).IsEmpty())
	{
		if (!WebPage.CallJScript(CString(varFunctionName.bstrVal)))
			return S_OK;
	}
	else
	{
		if (!WebPage.CallJScript(CString(varFunctionName.bstrVal), CString(varArg1.bstrVal)))
			return S_OK;
	}

	*bSuccess = true;
	return S_OK;
}
