#pragma once
#include "Utility.h"
#include "ProgressDialog.h"

// The following code does not like the 'new' debug macro, so save it undefine it, and restore it
#pragma push_macro("new")
#undef new
	#include <strstream>
	#include <vector>
#pragma pop_macro("new")
using namespace std;

struct DOWNLOAD
{
	CString strSrcURL;
	CString strDstPath;
	int iData;
	BYTE* pData;
	DWORD dwSize;
};

typedef vector<DOWNLOAD> DOWNLOADLIST;
typedef bool (CALLBACK * FNDOWNLOADCALLBACK)(LPARAM lParam, void* pDownload);

#define DL_PROMPTUSER 1
#define DL_KILLINPROGRESS 2

class CDownload
{
public:
	CDownload();
	~CDownload();
	void SetAppInfo(IOleClientSite* pClientSite, int iAppType);
	bool Init(int iFileType, FNDOWNLOADCALLBACK fnCallback, LPARAM lParam, DWORD dwFlags = DL_PROMPTUSER);
	void AddFile(CString& strSrcURL, CString& strDstPath, int iData);
	bool Start(bool bGoOnline, LPCTSTR szProgressTitle = NULL);
	bool Next();
	HRESULT OnData(CString& strSrcURL, BYTE* pBytes, DWORD dwSize);
	HRESULT OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText);
	bool InProgress()
		{ return (m_iFileType >= 0); }

protected:
	void FileData(BYTE* pBytes, DWORD dwLen);
	void FileEnd(bool bError);
	void Free();
	void FileSave();
	void FreeSavedData();

protected:
	IOleClientSite* m_pClientSite;
	int m_iAppType;
	int m_iFileType;
	FNDOWNLOADCALLBACK m_fnCallback;
	LPARAM m_lParam;
	BYTE* m_pData;
	DWORD m_dwSize;
	DOWNLOADLIST m_List;
	CProgressDialog m_ProgressDialog;
	bool m_bShowProgress;
	bool m_bGoOnline;

	BYTE* m_pDataSaved;
	DWORD m_dwSizeSaved;
	FNDOWNLOADCALLBACK m_fnCallbackSaved;
};

// The only reason for duplicating ATL's CBindStatusCallback class was to utilize a static OnData function
// Otherwise, this is the same class definition
// All variations from the ATL CBindStatusCallback are indicated with //a comments
template <class TT, int nBindFlags = BINDF_ASYNCHRONOUS | BINDF_ASYNCSTORAGE | BINDF_GETNEWESTVERSION | BINDF_NOWRITECACHE>
class ATL_NO_VTABLE CBindStatusCallback2 :
	public CComObjectRootEx<typename TT::_ThreadModel::ThreadModelNoCS>,
	public IBindStatusCallback
{
//a	typedef void (TT::*ATL_PDATAAVAILABLE)(CBindStatusCallback2<TT, nBindFlags>* pbsc, BYTE* pBytes, DWORD dwSize);
	typedef void (*ATL_PDATAAVAILABLE)(CString& strSrcURL, BYTE* pBytes, DWORD dwSize);

public:
	typedef CBindStatusCallback2<TT, nBindFlags> thisClass;

	BEGIN_COM_MAP(thisClass)
		COM_INTERFACE_ENTRY(IBindStatusCallback)
	END_COM_MAP()

	CBindStatusCallback2()
	{
//a		m_pT = NULL;
//a		m_pFunc = NULL;
		m_pDownload = NULL; //a
	}
	~CBindStatusCallback2()
	{
		ATLTRACE(atlTraceControls,2,_T("~CBindStatusCallback\n"));
	}

	STDMETHOD(OnStartBinding)(DWORD dwReserved, IBinding *pBinding)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnStartBinding\n"));
		m_spBinding = pBinding;
		return S_OK;
	}

	STDMETHOD(GetPriority)(LONG *pnPriority)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::GetPriority"));
		HRESULT hr = S_OK;
		if (pnPriority)
			*pnPriority = THREAD_PRIORITY_NORMAL;
		else
			hr = E_INVALIDARG;
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
		if (m_pDownload)
		{
			HRESULT hr = m_pDownload->OnProgress(ulProgress, ulProgressMax, ulStatusCode, szStatusText);
			if (FAILED(hr))
				m_spBinding->Abort();
			return  hr;
		}
		return S_OK;
	}

	STDMETHOD(OnStopBinding)(HRESULT hresult, LPCWSTR szError)
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

		// Pass NULL as the array of bytes to signify the end.
		// Pass the HRESULT for the dwSize parameter
//a		(m_pT->*m_pFunc)(this, NULL, hresult);
		return m_pDownload->OnData(strSrcURL, NULL, hresult); //a
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

	STDMETHOD(OnDataAvailable)(DWORD grfBSCF, DWORD dwSize, FORMATETC* pFormatetc, STGMEDIUM *pstgmed)
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

//a						(m_pT->*m_pFunc)(this, pBytes, dwActuallyRead);
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

	STDMETHOD(OnObjectAvailable)(REFIID riid, IUnknown* punk)
	{
		ATLTRACE(atlTraceControls,2,_T("CBindStatusCallback::OnObjectAvailable"));
		return S_OK;
	}

	HRESULT _StartAsyncDownload(BSTR bstrURL, IUnknown* pUnkContainer, BOOL bRelative)
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

//a	HRESULT StartAsyncDownload(TT* pT, ATL_PDATAAVAILABLE pFunc, BSTR bstrURL, IUnknown* pUnkContainer = NULL, BOOL bRelative = FALSE)
	HRESULT StartAsyncDownload(CDownload* pDownload, BSTR bstrURL, IUnknown* pUnkContainer = NULL, BOOL bRelative = FALSE)
	{
//a		m_pT = pT;
//a		m_pFunc = pFunc;
		m_pDownload = pDownload; //a
		return  _StartAsyncDownload(bstrURL, pUnkContainer, bRelative);
	}

//a	static HRESULT Download(TT* pT, ATL_PDATAAVAILABLE pFunc, BSTR bstrURL, IUnknown* pUnkContainer = NULL, BOOL bRelative = FALSE)
//a	{
//a		CComObject<CBindStatusCallback2<TT, nBindFlags> > *pbsc;
//a		HRESULT hRes = CComObject<CBindStatusCallback2<TT, nBindFlags> >::CreateInstance(&pbsc);
//a		if (FAILED(hRes))
//a			return hRes;
//a		return pbsc->StartAsyncDownload(pT, pFunc, bstrURL, pUnkContainer, bRelative);
//a	}

	static HRESULT Download(BSTR bstrSrcURL, CDownload* pDownload, IOleClientSite* pClientSite) //a
	{
		CComObject<CBindStatusCallback2<TT, nBindFlags> >* pbsc = NULL;
		HRESULT hResult = CComObject<CBindStatusCallback2<TT, nBindFlags> >::CreateInstance(&pbsc);
		if (FAILED(hResult))
			return hResult;

	//	if (!pClientSite)
	//		CMessageBox::Message(String("Error downloading %s.  No client site.", CString(bstrSrcURL)));

		bool bRelative = (CString(bstrSrcURL).Find(':') < 0);
		hResult = pbsc->StartAsyncDownload(pDownload, bstrSrcURL, pClientSite, bRelative);
		if (SUCCEEDED(hResult))
			return hResult;

		CString strError = HRString(hResult);
		CMessageBox::Message(String("%s (0x%lx)\nError while trying to download %s.", strError, hResult, CString(bstrSrcURL)));
		return hResult;
	}

private:
	CComPtr<IMoniker> m_spMoniker;
	CComPtr<IBindCtx> m_spBindCtx;
	CComPtr<IBinding> m_spBinding;
	CComPtr<IStream> m_spStream;
//a	TT* m_pT;
//a	ATL_PDATAAVAILABLE m_pFunc;
	CDownload* m_pDownload; //a
	DWORD m_dwTotalRead;
	DWORD m_dwAvailableToRead;
};
