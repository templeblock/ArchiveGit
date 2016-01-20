// BrowserHelperObj.h : Declaration of the CBrowserHelperObj

#pragma once
#include "resource.h"       // main symbols
#include "AgOutlookAddIn.h"

static LPCTSTR CLSID_BROWSERHELPEROBJ_STRING = _T("{9FEA5BDA-695A-417B-AA31-B54A06570053}");


// CBrowserHelperObj

class ATL_NO_VTABLE CBrowserHelperObj : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBrowserHelperObj, &CLSID_BrowserHelperObj>,
	public IObjectWithSiteImpl<CBrowserHelperObj>,
	public IDispatchImpl<IBrowserHelperObj, &IID_IBrowserHelperObj, &LIBID_AgOutlookAddInLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CBrowserHelperObj();
	virtual ~CBrowserHelperObj();
	

DECLARE_REGISTRY_RESOURCEID(IDR_BROWSERHELPEROBJ)

DECLARE_NOT_AGGREGATABLE(CBrowserHelperObj)

BEGIN_COM_MAP(CBrowserHelperObj)
	COM_INTERFACE_ENTRY(IBrowserHelperObj)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

// IObjectWithSite
public:
	STDMETHOD(SetSite)(IUnknown* pUnkSite);

private:
	CComPtr<IWebBrowser2> m_spWebBrowser2;
	bool m_bVisible;
	bool m_bLoaded;


};

OBJECT_ENTRY_AUTO(__uuidof(BrowserHelperObj), CBrowserHelperObj)
