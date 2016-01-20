#pragma once

class CDownload;

class CMyBindStatusCallback :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IBindStatusCallback
{
public:
	BEGIN_COM_MAP(CMyBindStatusCallback)
		COM_INTERFACE_ENTRY(IBindStatusCallback)
	END_COM_MAP()

	CMyBindStatusCallback()
	:m_pDownload(NULL),
	 nBindFlags(BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE)
	{
	}

	~CMyBindStatusCallback()
	{
		ATLTRACE(atlTraceControls,2,_T("~CBindStatusCallback\n"));
	}

	STDMETHOD(OnStartBinding)(DWORD dwReserved, IBinding *pBinding);
	STDMETHOD(GetPriority)(LONG *pnPriority)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::GetPriority"));
		HRESULT hr = S_OK;
		if (pnPriority)
			*pnPriority = THREAD_PRIORITY_NORMAL;

		return S_OK;
	}

	STDMETHOD(OnLowResource)(DWORD dwReserved)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnLowResource"));
		return S_OK;
	}

	STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnProgress"));
		//if (m_pDownload)
		//{
		//	HRESULT hr = m_pDownload->OnProgress(ulProgress, ulProgressMax, ulStatusCode, szStatusText);
		//	if (FAILED(hr))
		//		m_spBinding->Abort();
		//	return  hr;
		//}
		return S_OK;
	}

	STDMETHOD(GetBindInfo)(DWORD *pgrfBINDF, BINDINFO *pbindInfo)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::GetBindInfo\n"));

		if (pbindInfo==NULL || pbindInfo->cbSize==0 || pgrfBINDF==NULL)
			return E_INVALIDARG;

		*pgrfBINDF = nBindFlags;

		ULONG cbSize = pbindInfo->cbSize;		// remember incoming cbSize
		memset(pbindInfo, 0, cbSize);			// zero out structure
		pbindInfo->cbSize = cbSize;				// restore cbSize
		pbindInfo->dwBindVerb = BINDVERB_GET;	// set verb
		return S_OK;
	}

	STDMETHOD(OnObjectAvailable)(REFIID riid, IUnknown* punk)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnObjectAvailable"));
		return S_OK;
	}

	STDMETHOD(StartAsyncDownload)(CDownload* pDownload, BSTR bstrURL, IUnknown* pUnkContainer = NULL, BOOL bRelative = FALSE)
	{
		m_pDownload = pDownload; //a
		return  _StartAsyncDownload(bstrURL, pUnkContainer, bRelative);
	}

	STDMETHOD(OnStopBinding)(HRESULT hresult, LPCWSTR szError);
	STDMETHOD(OnDataAvailable)(DWORD grfBSCF, DWORD dwSize, FORMATETC* pFormatetc, STGMEDIUM *pstgmed);
	HRESULT _StartAsyncDownload(BSTR bstrURL, IUnknown* pUnkContainer, BOOL bRelative);
	static HRESULT Download(BSTR bstrSrcURL, CDownload* pDownload, IOleClientSite* pClientSite = NULL);

private:
	CComPtr<IMoniker> m_spMoniker;
	CComPtr<IBindCtx> m_spBindCtx;
	CComPtr<IBinding> m_spBinding;
	CComPtr<IStream> m_spStream;

	CDownload* m_pDownload; //a
	DWORD m_dwTotalRead;
	DWORD m_dwAvailableToRead;

	int nBindFlags;
};
