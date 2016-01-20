// Pack.h : Declaration of the CPack

#pragma once
#include "resource.h"       // main symbols
#include "Download.h"
#include "PAHCollection.h"
#include "PAHProject.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// IPack
[
	object,
	uuid("300F4EEA-6570-481D-9DF1-E3F1B01ADB5E"),
	dual,	helpstring("IPack Interface"),
	pointer_default(unique)
]
__interface IPack : IDispatch
{
	[id(1), helpstring("method ProcessPurchase"), helpcontext(1001)] HRESULT ProcessPurchase([in] BSTR szPackUrl);
};


///////////////////////////////////////////////////////////////////////////////
// CPack

[
	coclass,
	default(IPack),
	threading(apartment),
	vi_progid("PAHContentMgr.Pack"),
	progid("PAHContentMgr.Pack.1"),
	version(1.0),
	uuid("372E2874-34CF-44E8-B55D-73AFCB07AE84"),
	helpstring("Pack Class")
]
///////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE CPack :
	public IPack
{
public:
	CPack()
	{
	}


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

public:
	STDMETHOD(ProcessPurchase)(BSTR szPackUrl);
};

