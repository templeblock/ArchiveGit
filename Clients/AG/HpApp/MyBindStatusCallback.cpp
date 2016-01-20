#include "stdafx.h"
#include "MyBindStatusCallback.h"
#include "Download.h"
#include "MessageBox.h"

HRESULT CMyBindStatusCallback::Download(BSTR bstrSrcURL, CDownload* pDownload, IOleClientSite* pClientSite) //a
{
	CComObject<CMyBindStatusCallback>* pbsc = NULL;
	HRESULT hResult = CComObject<CMyBindStatusCallback>::CreateInstance(&pbsc);
	if (FAILED(hResult))
		return hResult;

	bool bRelative = (CString(bstrSrcURL).Find(':') < 0);
	hResult = pbsc->StartAsyncDownload(pDownload, bstrSrcURL, pClientSite, bRelative);
	if (SUCCEEDED(hResult))
		return hResult;

	CString strError = HRString(hResult);
	CMessageBox::Message(String("%s (0x%lx)\nError while trying to download %s.", strError, hResult, CString(bstrSrcURL)));
	return hResult;
}

STDMETHODIMP CMyBindStatusCallback::OnStartBinding(DWORD dwReserved, IBinding *pBinding)
{
	ATLTRACE(atlTraceControls,2,_T("CMyBindStatusCallback::OnStartBinding\n"));
	m_spBinding = pBinding;
	return S_OK;
}

STDMETHODIMP CMyBindStatusCallback::OnStopBinding(HRESULT hresult, LPCWSTR szError)
{
	ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnStopBinding\n"));

	CString strSrcURL;
	if (m_spMoniker)
	{
		LPOLESTR bstrSrcURL = NULL;
		m_spMoniker->GetDisplayName(m_spBindCtx, m_spMoniker, &bstrSrcURL);
		strSrcURL = CString(bstrSrcURL);
	}

	// Release the interfaces before we make the final callback
	m_spBinding.Release(); //a Moved up before the OnData call
	m_spBindCtx.Release(); //a
	m_spMoniker.Release(); //a

	return m_pDownload->OnData(strSrcURL, NULL, hresult); //a
}

HRESULT CMyBindStatusCallback::_StartAsyncDownload(BSTR bstrURL, IUnknown* pUnkContainer, BOOL bRelative)
{
	m_dwTotalRead = 0;
	m_dwAvailableToRead = 0;
	HRESULT hr = S_OK;
	CComQIPtr<IServiceProvider, &__uuidof(IServiceProvider)> spServiceProvider(pUnkContainer);
	CComPtr<IBindHost> spBindHost;
	CComPtr<IStream> spStream;
	if (spServiceProvider)
		spServiceProvider->QueryService(SID_IBindHost, __uuidof(IBindHost), (void**)&spBindHost);

	if (spBindHost == NULL)
	{
		if (bRelative)
			return E_NOINTERFACE;  // relative asked for, but no IBindHost
		hr = CreateURLMoniker(NULL, bstrURL, &m_spMoniker);
		if (SUCCEEDED(hr))
			hr = CreateBindCtx(0, &m_spBindCtx);

		if (SUCCEEDED(hr))
			hr = RegisterBindStatusCallback(m_spBindCtx, static_cast<IBindStatusCallback*>(this), 0, 0L);
		else
			m_spMoniker.Release();

		if (SUCCEEDED(hr))
			hr = m_spMoniker->BindToStorage(m_spBindCtx, 0, __uuidof(IStream), (void**)&spStream);
	}
	else
	{
		hr = CreateBindCtx(0, &m_spBindCtx);
		if (SUCCEEDED(hr))
			hr = RegisterBindStatusCallback(m_spBindCtx, static_cast<IBindStatusCallback*>(this), 0, 0L);

		if (SUCCEEDED(hr))
		{
			if (bRelative)
				hr = spBindHost->CreateMoniker(bstrURL, m_spBindCtx, &m_spMoniker, 0);
			else
				hr = CreateURLMoniker(NULL, bstrURL, &m_spMoniker);
		}

		if (SUCCEEDED(hr))
		{
			hr = spBindHost->MonikerBindToStorage(m_spMoniker, m_spBindCtx, static_cast<IBindStatusCallback*>(this), __uuidof(IStream), (void**)&spStream);
			ATLTRACE(atlTraceControls,2,_T("Bound"));
		}
	}
	return hr;
}

STDMETHODIMP CMyBindStatusCallback::OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC* pFormatetc, STGMEDIUM *pstgmed)
{
	ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnDataAvailable\n"));
	HRESULT hr = S_OK;

	// Get the Stream passed
	if (BSCF_FIRSTDATANOTIFICATION & grfBSCF)
	{
		if (!m_spStream && pstgmed->tymed == TYMED_ISTREAM)
			m_spStream = pstgmed->pstm;
	}

	DWORD dwRead = dwSize - m_dwTotalRead; // Minimum amount available that hasn't been read
	DWORD dwActuallyRead = 0;            // Placeholder for amount read during this pull

	// If there is some data to be read then go ahead and read them
	if (m_spStream)
	{
		if (dwRead > 0)
		{
			BYTE* pBytes = NULL;
			ATLTRY(pBytes = new BYTE[dwRead + 1]);
			if (pBytes == NULL)
				return E_OUTOFMEMORY;
			hr = m_spStream->Read(pBytes, dwRead, &dwActuallyRead);
			if (SUCCEEDED(hr))
			{
				pBytes[dwActuallyRead] = 0;
				if (dwActuallyRead>0)
				{
					CString strSrcURL;
					if (m_spMoniker)
					{
						LPOLESTR bstrSrcURL = NULL;
						m_spMoniker->GetDisplayName(m_spBindCtx, m_spMoniker, &bstrSrcURL);
						strSrcURL = CString(bstrSrcURL);
					}

					m_pDownload->OnData(strSrcURL, pBytes, dwActuallyRead); //a
					m_dwTotalRead += dwActuallyRead;
				}
			}
			delete[] pBytes;
		}
	}

	if (BSCF_LASTDATANOTIFICATION & grfBSCF)
		m_spStream.Release();
	return hr;
}
