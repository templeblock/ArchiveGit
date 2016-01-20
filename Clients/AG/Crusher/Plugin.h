// Plugin.h : Declaration of the CPlugin
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "Crusher.h"

// CPlugin

#define dwSAFETY_OPTIONS	INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA

class ATL_NO_VTABLE CPlugin : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IPlugin, &IID_IPlugin, &LIBID_CrusherLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CPlugin>,
	public IOleControlImpl<CPlugin>,
	public IOleObjectImpl<CPlugin>,
	public IOleInPlaceActiveObjectImpl<CPlugin>,
	public IViewObjectExImpl<CPlugin>,
	public IOleInPlaceObjectWindowlessImpl<CPlugin>,
	public IObjectSafetyImpl<CPlugin, dwSAFETY_OPTIONS>,
	public CComCoClass<CPlugin, &CLSID_Plugin>,
	public CComControl<CPlugin>
{
public:

	CPlugin()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_PLUGIN)
	DECLARE_NOT_AGGREGATABLE(CPlugin)
	DECLARE_OLEMISC_STATUS(
		/*0x00001*/ OLEMISC_RECOMPOSEONRESIZE | 
		/*0x00010*/ OLEMISC_CANTLINKINSIDE | 
		/*0x00080*/ OLEMISC_INSIDEOUT | 
		/*0x00100*/ OLEMISC_ACTIVATEWHENVISIBLE | 
		/*0x00400*/ OLEMISC_INVISIBLEATRUNTIME | //j
		/*0x20000*/ OLEMISC_SETCLIENTSITEFIRST
	)

	BEGIN_COM_MAP(CPlugin)
		COM_INTERFACE_ENTRY(IPlugin)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IViewObjectEx)
		COM_INTERFACE_ENTRY(IViewObject2)
		COM_INTERFACE_ENTRY(IViewObject)
		COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceObject)
		COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
		COM_INTERFACE_ENTRY(IObjectSafety)
		COM_INTERFACE_ENTRY(IOleControl)
		COM_INTERFACE_ENTRY(IOleObject)
		COM_INTERFACE_ENTRY(IPersistStreamInit)
		COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	END_COM_MAP()

	BEGIN_PROP_MAP(CPlugin)
		PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
		PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
		// Example entries
		// PROP_ENTRY("Property Description", dispid, clsid)
		// PROP_PAGE(CLSID_StockColorPage)
	END_PROP_MAP()

	BEGIN_MSG_MAP(CPlugin)
		CHAIN_MSG_MAP(CComControl<CPlugin>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

// Handler prototypes:
//	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//	LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IObjectSafety
	STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD* pdwSupportedOptions, DWORD* pdwEnabledOptions)
	{
		if (!pdwSupportedOptions || !pdwEnabledOptions)
			return E_POINTER;

		HRESULT hr = S_OK;
		if (riid == IID_IDispatch)
		{
			*pdwEnabledOptions = *pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
		}
		else
		{
			*pdwEnabledOptions = *pdwSupportedOptions = 0;
			hr = E_NOINTERFACE;
		}
		return hr;
	}

	STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
	{
		return S_OK;
	}

// IPlugin
	STDMETHOD(Version)(BSTR* pbstrVersion);
	STDMETHOD(SpellCheck)(BSTR bstrText, int iFlags, BSTR* pbstrChangedText);
	STDMETHOD(UploadImages)(BSTR bstrURL, BSTR bstrForm, int iMaxWidth, int iMaxHeight, int nQuality, int bSilent, BSTR* pbstrResult);
	STDMETHOD(ResizeJPG)(BSTR bstrFilePath, int iMaxWidth, int iMaxHeight, int nQuality, int bSilent, BSTR* pbstrFilePathNew);
	STDMETHOD(ColorPicker)(int iFormat, BSTR* pbstrColor);
	STDMETHOD(ColorPicker2)(int iFormat, BSTR* pbstrColor);

private:
	HRESULT UploadForm(const CString& strURL, const CString& strFormDataPairs, int iMaxWidth, int iMaxHeight, int nQuality, int bSilent, CString& strReturn);
	HGLOBAL PrepareFormData(const CString& strFormDataPairs, int iMaxWidth, int iMaxHeight, int nQuality, int bSilent);

public:
	HRESULT OnDraw(ATL_DRAWINFO& di)
	{
		RECT& rc = *(RECT*)di.prcBounds;
		// Set Clip region to the rectangle specified by di.prcBounds
		HRGN hRgnOld = NULL;
		if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
			hRgnOld = NULL;
		bool bSelectOldRgn = false;

		HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
		if (hRgnNew)
			bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);

		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("Crusher Plugin");
		TextOut(di.hdcDraw, 
			(rc.left + rc.right) / 2, 
			(rc.top + rc.bottom) / 2, 
			pszText, 
			lstrlen(pszText));

		if (bSelectOldRgn)
			SelectClipRgn(di.hdcDraw, hRgnOld);

		return S_OK;
	}


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}
};

OBJECT_ENTRY_AUTO(__uuidof(Plugin), CPlugin)
