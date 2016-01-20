// UpdateObj.h : Declaration of the CUpdateObj

#pragma once
#include "resource.h"       // main symbols
#include "AgOutlookAddIn.h"
#include "_IUpdateObjEvents_CP.H"
#include "Async.h"
#include "Authenticate.h"


// CUpdateObj

class ATL_NO_VTABLE CUpdateObj : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUpdateObj, &CLSID_UpdateObj>,
	public CComControl<CUpdateObj>,
	public IConnectionPointContainerImpl<CUpdateObj>,
	public IDispatchImpl<IUpdateObj, &IID_IUpdateObj, &LIBID_AgOutlookAddInLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public CProxy_IUpdateObjEvents<CUpdateObj>,
	public CAsync

{
public:
	CUpdateObj();
	~CUpdateObj();
	
	DECLARE_REGISTRY_RESOURCEID(IDR_UPDATEOBJ)


	BEGIN_COM_MAP(CUpdateObj)
		COM_INTERFACE_ENTRY(IUpdateObj)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CUpdateObj)
		CONNECTION_POINT_ENTRY(__uuidof(_IUpdateObjEvents))
	END_CONNECTION_POINT_MAP()

	BEGIN_MSG_MAP(CUpdateObj)
			CHAIN_MSG_MAP(CComControl<CUpdateObj>)
			DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		// Set download info.
		m_Download.SetAppInfo(this->m_spClientSite, CLIENT_TYPE_UPDATE);
		return S_OK;
	}

	void FinalRelease() 
	{
	}

public:

	STDMETHOD(DoAutoUpdate)(OLE_HANDLE hwndOwner, VARIANT_BOOL Silent);

protected:

	virtual void AutoUpdateDone(DWORD dwResult);

};

OBJECT_ENTRY_AUTO(__uuidof(UpdateObj), CUpdateObj)
