// Broker.h : Declaration of the CBroker

#pragma once
#include "resource.h"       // main symbols

typedef bool (CALLBACK * FNBROKERCALLBACK)(WPARAM wParam, LPARAM lParam, IDispatch * pDisp);


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// IBroker
[
	object,
	uuid("AB2ED144-E898-4477-8ABA-D827F2010B61"),
	dual,	helpstring("IBroker Interface"),
	pointer_default(unique)
]
__interface IBroker : IDispatch
{
	[id(1), helpstring("method SetDocument")] HRESULT SetDocument([in] IDispatch* pDisp);
	[id(2), helpstring("method CallJavaScript")] HRESULT CallJavaScript([in] VARIANT varFunctionName, [in] VARIANT varArg1, [out] VARIANT_BOOL* bSuccess);
};


// _IBrokerEvents
[
	dispinterface,
	uuid("2CBBE568-51BB-4C9B-89B7-7686ABA70950"),
	helpstring("_IBrokerEvents Interface")
]
__interface _IBrokerEvents
{
};


// CBroker

[
	coclass,
	default(IBroker, _IBrokerEvents),
	threading(apartment),
	event_source(com),
	vi_progid("PAHBrokerManager.Broker"),
	progid("PAHBrokerManager.Broker.1"),
	version(1.0),
	uuid("ADAD7888-8E81-491F-9791-967F36AFF832"),
	helpstring("Broker Class")
]
class ATL_NO_VTABLE CBroker :
	public IBroker
{
public:
	CBroker()
	{
	}

	__event __interface _IBrokerEvents;


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	bool Initialize(FNBROKERCALLBACK fnCallback, LPARAM lParam, WPARAM wParam);

protected:
	void PluginLoaded(VARIANT varValue);

public:
	STDMETHOD(SetDocument)(IDispatch* pDisp);
	STDMETHOD(CallJavaScript)(VARIANT varFunctionName, VARIANT varArg1, VARIANT_BOOL* bSuccess);

protected:
	CComPtr<IHTMLDocument2> m_spDoc;
	FNBROKERCALLBACK m_fnCallback;
	LPARAM m_lParam;
	WPARAM m_wParam;
};

