#ifndef __BSC2_H_
#define __BSC2_H_

template <class T>
class CBindStatusCallback2 : public CBindStatusCallback<T>
{
public:
	STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
	{
		ATLTRACE(_T("CBindStatusCallback2::OnProgress"));
		return m_pT->OnProgress( ulProgress, ulProgressMax, ulStatusCode, szStatusText );
	}

	static HRESULT Download(T* pT, ATL_PDATAAVAILABLE pFunc, BSTR bstrURL, IUnknown* pUnkContainer = NULL, BOOL bRelative = FALSE)
	{
		CComObject<CBindStatusCallback2<T> > *pbsc;
		HRESULT hRes = CComObject<CBindStatusCallback2<T> >::CreateInstance(&pbsc);
		if (FAILED(hRes))
			return hRes;
		return pbsc->StartAsyncDownload(pT, pFunc, bstrURL, pUnkContainer, bRelative);
	}
};

#endif //__BSC2_H_
