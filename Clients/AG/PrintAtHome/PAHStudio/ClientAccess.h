// ClientAccess.h : Declaration of the CClientAccess

#pragma once
#include "resource.h"       // main symbols



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// IClientAccess
[
	object,
	uuid("D869010E-FB38-48F5-BF06-DB757BBE30C2"),
	dual,	helpstring("IClientAccess Interface"),
	pointer_default(unique)
]
__interface IClientAccess : IDispatch
{
	[id(1), helpstring("method SetClientObject")] HRESULT SetClientObject([in] IUnknown* punkClient);
};



// CClientAccess

[
	coclass,
	default(IClientAccess),
	threading(apartment),
	vi_progid("PAHStudio.ClientAccess"),
	progid("PAHStudio.ClientAccess.1"),
	version(1.0),
	uuid("26B3E5A6-B47E-481A-9A1D-0D98603152E5"),
	helpstring("ClientAccess Class")
]
class ATL_NO_VTABLE CClientAccess :
	public IClientAccess
{
public:
	CClientAccess()
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
	STDMETHOD(SetClientObject)(IUnknown* punkClient);
};

