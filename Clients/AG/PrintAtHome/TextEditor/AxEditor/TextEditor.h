// TextEditor.h : Declaration of the CTextEditor
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "AxEditor.h"
#include "Draw.h"
#include "_ITextEditorEvents_CP.h"

#define dwSAFETY_OPTIONS	INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA

// CTextEditor
class ATL_NO_VTABLE CTextEditor : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTextEditor, &CLSID_TextEditor>,
	public CComControl<CTextEditor>,
	public IDispatchImpl<ITextEditor, &IID_ITextEditor, &LIBID_AxEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CTextEditor>,
	public IPersistStorageImpl<CTextEditor>,
	public IPersistPropertyBagImpl<CTextEditor>,
	public IOleControlImpl<CTextEditor>,
	public IOleObjectImpl<CTextEditor>,
	public IOleInPlaceActiveObjectImpl<CTextEditor>,
	public IViewObjectExImpl<CTextEditor>,
	public IOleInPlaceObjectWindowlessImpl<CTextEditor>,
	public IConnectionPointContainerImpl<CTextEditor>,
	public IProvideClassInfo2Impl<&CLSID_TextEditor, &__uuidof(_ITextEditorEvents), &LIBID_AxEditorLib>,
	public IPropertyNotifySinkCP<CTextEditor>,
	public CProxy_ITextEditorEvents<CTextEditor>, 
	public IQuickActivateImpl<CTextEditor>,
	public IDataObjectImpl<CTextEditor>,
	public IObjectSafetyImpl<CTextEditor, dwSAFETY_OPTIONS>
{
public:

DECLARE_REGISTRY_RESOURCEID(IDR_TEXTEDITOR)
DECLARE_NOT_AGGREGATABLE(CTextEditor)
DECLARE_OLEMISC_STATUS(
	OLEMISC_RECOMPOSEONRESIZE | 
	OLEMISC_CANTLINKINSIDE | 
	OLEMISC_INSIDEOUT | 
	OLEMISC_ACTIVATEWHENVISIBLE | 
	OLEMISC_SETCLIENTSITEFIRST
)

BEGIN_COM_MAP(CTextEditor)
	COM_INTERFACE_ENTRY(ITextEditor)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(IPersistPropertyBag)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

BEGIN_PROP_MAP(CTextEditor)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CTextEditor)
	CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
	CONNECTION_POINT_ENTRY(__uuidof(_ITextEditorEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CTextEditor)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	CHAIN_MSG_MAP(CComControl<CTextEditor>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()

	CTextEditor()
	{
		this->m_bWindowOnly = 1;
		m_pDraw = NULL;
	}

	~CTextEditor()
	{
	}

// Handler prototypes:
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

  LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] = 
		{
			&IID_ITextEditor,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IObjectSafety
	STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD* pdwSupportedOptions, DWORD* pdwEnabledOptions);
	STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);

// ITextEditor
	STDMETHOD(SetColorGradientMode)(USHORT SweepMode);
	STDMETHOD(SetFont)(BSTR FontName);
	STDMETHOD(IncrementLineHeight)(void);
	STDMETHOD(DecrementLineHeight)(void);
	STDMETHOD(IncrementCharWidth)(void);
	STDMETHOD(DecrementCharWidth)(void);
	STDMETHOD(SetFontSize)(USHORT FontSize);
	STDMETHOD(SetCurveType)(USHORT CurveType);
	STDMETHOD(SetImageSize)(FLOAT Width, FLOAT Height);
	STDMETHOD(SetString)(BSTR LineString, USHORT Index);
	STDMETHOD(SaveImage)(BSTR FileName, USHORT ImageType);
	STDMETHOD(RefreshPreviewWindow)(void);
	STDMETHOD(SetColor1)(LONG Color1);
	STDMETHOD(SetColor2)(LONG Color2);
	STDMETHOD(SetPreviewBkColor)(LONG Clr);
	STDMETHOD(DeleteString)(USHORT Index);
	STDMETHOD(SetImageBkColor)(LONG Clr);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

private:
	CStringDraw *m_pDraw;
	Color* m_CurrPrvBkColor;
public:
	STDMETHOD(SetWhiteBkgdTransparent)(VARIANT_BOOL bkgdTransparent);
};

//OBJECT_ENTRY_AUTO(__uuidof(TextEditor), CTextEditor)
