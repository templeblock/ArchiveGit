#pragma once

class DWebBrowserEventsImpl : public DWebBrowserEvents2
{
public:

	DWebBrowserEventsImpl();
	virtual ~DWebBrowserEventsImpl(void);

	// IUnknown methods
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppv)
	{
		*ppv = NULL;

		if (IID_IUnknown == riid || __uuidof(DWebBrowserEvents2) == riid)
		{
			*ppv = (LPUNKNOWN)(DWebBrowserEvents2*)this;
			AddRef();
			return NOERROR;
		}
		else if (IID_IOleClientSite == riid)
		{
			*ppv = (IOleClientSite*)this;
			AddRef();
			return NOERROR;
		}
		else if (IID_IDispatch == riid)
		{
			*ppv = (IDispatch*)this;
			AddRef();
			return NOERROR;
		}
		else
		{
			return E_NOTIMPL;
		}
	}
    STDMETHOD_(ULONG, AddRef)() { return 1;}
    STDMETHOD_(ULONG, Release)() { return 0;}

	// IDispatch methods
	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo) {return E_NOTIMPL;}
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) {return E_NOTIMPL;}
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames,  UINT cNames, LCID lcid,  DISPID* rgDispId) {return E_NOTIMPL;}
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult,
            EXCEPINFO *pExcepInfo, UINT *puArgErr);
	

    // Methods: 
	virtual void NewWindow2(IDispatch **ppDisp, VARIANT_BOOL *Cancel){return;}
	virtual void BeforeNavigate2(IDispatch *pDisp, VARIANT *url, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *Cancel){return;}
	virtual void NavigateComplete2(IDispatch *pDisp, VARIANT *URL){return;}
	virtual void StatusTextChange(BSTR Text){return;}
	virtual void ProgressChange(long Progress, long ProgressMax){return;}
	virtual void DocumentComplete(IDispatch *pDisp, VARIANT *URL){return;}
	virtual void DocumentReallyComplete(IDispatch *pDisp, VARIANT *URL){return;}
	virtual void DownloadBegin(){return;}
	virtual void DownloadComplete(){return;}
	virtual void CommandStateChange(long Command, VARIANT_BOOL Enable){return;}
	virtual void TitleChange(BSTR Text){return;}
	virtual void PropertyChange(BSTR szProperty){return;}
	virtual void WindowClosing(VARIANT_BOOL IsChildWindow, VARIANT_BOOL *Cancel){return;}
	virtual void FileDownload(VARIANT_BOOL *Cancel){return;}
	virtual void NavigateError(IDispatch *pDisp, VARIANT *URL, VARIANT *TargetFrameName, VARIANT *StatusCode, VARIANT_BOOL *Cancel){return;}

	virtual void OnVisible(VARIANT_BOOL Visible){return;}
	virtual void OnQuit(){Disconnect(); return;}
	

public:

	HRESULT Connect(IWebBrowser2 * pWB);
	HRESULT Disconnect();
	

protected:
	LPDISPATCH m_lpDocCompleteDisp;
	CComPtr<IWebBrowser2> m_spWebBrowser;
	DWORD m_dwCookie;

};
