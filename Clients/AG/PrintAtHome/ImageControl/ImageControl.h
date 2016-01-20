#pragma once
#include "resource.h"
#include <atlctl.h>
#include "ColorScheme.h"
#include "Label.h"

// IImageControl
[
	object,
	uuid(750976F5-86CA-4D8F-9103-EA07FC9DB611),
	dual,
	helpstring("IImageControl Interface"),
	pointer_default(unique)
]
__interface IImageControl : public IDispatch
{
	// Methods
	[id(1)] HRESULT DoCommand([in]BSTR strCommand, [in]BSTR strValue, [out, retval]long* plResult);
	[id(2)] HRESULT GetCommand([in]BSTR strCommand, [out,retval]long* plResult);

	// Properties
	[propget, bindable, requestedit, id(DISPID_HWND)]			HRESULT HWND([out, retval]long* pHWND);
	[propput, bindable, requestedit, id(DISPID_BACKCOLOR)]		HRESULT BackColor([in]OLE_COLOR clr);
	[propget, bindable, requestedit, id(DISPID_BACKCOLOR)]		HRESULT BackColor([out,retval]OLE_COLOR* pclr);
	[propput, bindable, requestedit, id(DISPID_BACKSTYLE)]		HRESULT BackStyle([in]long style);
	[propget, bindable, requestedit, id(DISPID_BACKSTYLE)]		HRESULT BackStyle([out,retval]long* pstyle);
	[propput, bindable, requestedit, id(DISPID_BORDERCOLOR)]	HRESULT BorderColor([in]OLE_COLOR clr);
	[propget, bindable, requestedit, id(DISPID_BORDERCOLOR)]	HRESULT BorderColor([out, retval]OLE_COLOR* pclr);
	[propput, bindable, requestedit, id(DISPID_BORDERSTYLE)]	HRESULT BorderStyle([in]long style);
	[propget, bindable, requestedit, id(DISPID_BORDERSTYLE)]	HRESULT BorderStyle([out, retval]long* pstyle);
	[propput, bindable, requestedit, id(DISPID_BORDERWIDTH)]	HRESULT BorderWidth([in]long width);
	[propget, bindable, requestedit, id(DISPID_BORDERWIDTH)]	HRESULT BorderWidth([out, retval]long* width);
	[propput, bindable, requestedit, id(DISPID_BORDERVISIBLE)]	HRESULT BorderVisible([in]VARIANT_BOOL vbool);
	[propget, bindable, requestedit, id(DISPID_BORDERVISIBLE)]	HRESULT BorderVisible([out, retval]VARIANT_BOOL* pbool);
	[propput, bindable, requestedit, id(DISPID_VALID)]			HRESULT Valid([in]VARIANT_BOOL vbool);
	[propget, bindable, requestedit, id(DISPID_VALID)]			HRESULT Valid([out, retval]VARIANT_BOOL* pbool);
};

// _IImageControlEvents
[
	uuid("FE2FC027-8743-4F05-90F3-BBE79D22F4CE"),
	dispinterface,
	helpstring("_IImageControlEvents Interface")
]
__interface _IImageControlEvents
{
};

// CImageControl

#define dwSAFETY_OPTIONS	INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA

class CDocWindow;

[
	coclass,
	control,
	default(IImageControl, _IImageControlEvents),
	threading(apartment),
	vi_progid("ImageControl.ImageControl"),
	progid("ImageControl.ImageControl.1"),
	version(1.0),
	uuid("266D1DBF-1107-4529-A48F-0FFBAE7351D1"),
	helpstring("ImageControl Class"),
	event_source(com),
	registration_script("control.rgs")
]
class ATL_NO_VTABLE CImageControl :
	public CStockPropImpl<CImageControl, IImageControl>, //new
	public IPersistStreamInitImpl<CImageControl>,
	public IOleControlImpl<CImageControl>,
	public IOleObjectImpl<CImageControl>,
	public IOleInPlaceActiveObjectImpl<CImageControl>,
	public IViewObjectExImpl<CImageControl>,
	public IOleInPlaceObjectWindowlessImpl<CImageControl>,
	public IPersistStorageImpl<CImageControl>,
	public IQuickActivateImpl<CImageControl>,
	public IDataObjectImpl<CImageControl>,
	public IObjectSafetyImpl<CImageControl, dwSAFETY_OPTIONS>,
	public IDropTarget,
	public IDropSource,
	public CComControl<CImageControl>
//j C&P has but we don't
//j	public CComObjectRootEx<CComSingleThreadModel>,
//j	public IPersistPropertyBagImpl<CCtp>,
//j	public IConnectionPointContainerImpl<CCtp>, //j
//j	public IProvideClassInfo2Impl<&CLSID_Ctp, &DIID__ICtpEvents, &LIBID_AXCTPLib>, //j
//j	public IPropertyNotifySinkCP<CCtp>, //j
//j	public CProxy_ICtpEvents<CCtp>, //j
//j	public IPersistFile, // for CImageExtractor
//j	public IExtractImage2, // for CImageExtractor
{
public:
	CImageControl();
	HRESULT FinalConstruct();
	void FinalRelease();

	void ConfigureWindows(int iWidth = NULL, int iHeight = NULL);

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	__event __interface _IImageControlEvents;

	// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_OPAQUE)

	// IObjectSafety
	STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD* pdwSupportedOptions, DWORD* pdwEnabledOptions);
	STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);

	// IDropTarget
	virtual STDMETHODIMP Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect); 
	virtual STDMETHODIMP DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect); 
	virtual STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect); 
	virtual STDMETHODIMP DragLeave(void); 
	void DoDragDrop();

	// IDropSource
	virtual STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed,  DWORD grfKeyState);
	virtual STDMETHODIMP GiveFeedback(DWORD dwEffect);

	// IImageControl
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT OnDraw(ATL_DRAWINFO& di);
	LRESULT OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT DoCommand(BSTR strCommand, BSTR strValue, long* plResult);
	HRESULT GetCommand(BSTR strCommand, long* plResult);

private:
	CString g_szAppName;
	CDocWindow* m_pDocWindow;
	CColorScheme m_ColorScheme;

public:
	OLE_COLOR m_clrBackColor;
	void OnBackColorChanged()
	{
		ATLTRACE(_T("OnBackColorChanged\n"));
	}

	LONG m_nBackStyle;
	void OnBackStyleChanged()
	{
		ATLTRACE(_T("OnBackStyleChanged\n"));
	}

	OLE_COLOR m_clrBorderColor;
	void OnBorderColorChanged()
	{
		ATLTRACE(_T("OnBorderColorChanged\n"));
	}

	LONG m_nBorderStyle;
	void OnBorderStyleChanged()
	{
		ATLTRACE(_T("OnBorderStyleChanged\n"));
	}

	BOOL m_bBorderVisible;
	void OnBorderVisibleChanged()
	{
		ATLTRACE(_T("OnBorderVisibleChanged\n"));
	}

	LONG m_nBorderWidth;
	void OnBorderWidthChanged()
	{
		ATLTRACE(_T("OnBorderWidthChanged\n"));
	}

	BOOL m_bValid;
	void OnValidChanged()
	{
		ATLTRACE(_T("OnValidChanged\n"));
	}

	DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE | OLEMISC_CANTLINKINSIDE | OLEMISC_INSIDEOUT | 
		OLEMISC_ACTIVATEWHENVISIBLE |OLEMISC_SETCLIENTSITEFIRST)

	BEGIN_PROP_MAP(CImageControl)
		PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
		PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
		PROP_ENTRY("BackColor", DISPID_BACKCOLOR, CLSID_StockColorPage)
		PROP_ENTRY("BackStyle", DISPID_BACKSTYLE, CLSID_NULL)
		PROP_ENTRY("BorderColor", DISPID_BORDERCOLOR, CLSID_StockColorPage)
		PROP_ENTRY("BorderStyle", DISPID_BORDERSTYLE, CLSID_NULL)
		PROP_ENTRY("BorderVisible", DISPID_BORDERVISIBLE, CLSID_NULL)
		PROP_ENTRY("BorderWidth", DISPID_BORDERWIDTH, CLSID_NULL)
		PROP_ENTRY("Valid", DISPID_VALID, CLSID_NULL)
		// Example entries
		// PROP_ENTRY("Property Description", dispid, clsid)
		 PROP_PAGE(CLSID_StockColorPage)
	END_PROP_MAP()

	BEGIN_MSG_MAP(CImageControl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		CHAIN_MSG_MAP(CComControl<CImageControl>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
};
