// JMToolband.h : Declaration of the CJMToolband

#pragma once
#include "resource.h"       // main symbols
#include <shlobj.h>
#include "AgOutlookAddIn.h"
#include "BandToolBarReflectorCtrl.h"
#include "BandToolBarCtrl.h"


#define MIN_SIZE_X   10
#define MIN_SIZE_Y   10

static LPCTSTR CLSID_JMTOOLBAND_STRING = _T("{08868B78-71CE-43AB-BB03-3851D57C9E93}");

// CJMToolband

class ATL_NO_VTABLE CJMToolband : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CJMToolband, &CLSID_JMToolband>,
	public CComControl<CJMToolband>,
	public IDeskBand,
	public IPersistStream,
	public IInputObject,
	public IObjectWithSiteImpl<CJMToolband>,
	public IDispatchImpl<IJMToolband, &IID_IJMToolband, &LIBID_AgOutlookAddInLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CJMToolband();
	virtual ~CJMToolband();
	

DECLARE_REGISTRY_RESOURCEID(IDR_JMTOOLBAND)


BEGIN_COM_MAP(CJMToolband)
	COM_INTERFACE_ENTRY(IJMToolband)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY_IID(IID_IDeskBand, IDeskBand)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY_IID(IID_IInputObject, IInputObject)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

// IDeskBand
public:
	STDMETHOD(GetBandInfo)(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi);

// IObjectWithSite
public:
	STDMETHOD(SetSite)(IUnknown* pUnkSite);
	STDMETHOD(GetSite)(REFIID riid, void **ppvSite);

// IOleWindow
public:
	STDMETHOD(GetWindow)(HWND* phwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

// IDockingWindow
public:
	STDMETHOD(CloseDW)(unsigned long dwReserved);
	STDMETHOD(ResizeBorderDW)(const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved);
	STDMETHOD(ShowDW)(BOOL fShow);

// IPersist
public:
	STDMETHOD(GetClassID)(CLSID *pClassID);

// IPersistStream
public:
	STDMETHOD(IsDirty)(void);
	STDMETHOD(Load)(IStream *pStm);
	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize);

// IInputObject methods
public:
	STDMETHOD (UIActivateIO) (BOOL, LPMSG);
	STDMETHOD (HasFocusIO) (void);
	STDMETHOD (TranslateAcceleratorIO) (LPMSG);

// IJMExplorerBand
public:
	void FocusChange(bool bFocus);
    CBandToolBarCtrl* GetToolBarCtrl() {return  m_ReflectWnd.GetToolBar();}

private:
	BOOL RegisterAndCreateWindow();

protected:
	DWORD m_dwBandID;
	DWORD m_dwViewMode;
	BOOL m_bShow;
	BOOL m_bEnterHelpMode;
	HWND m_hWndParentRebar;
	HWND m_hWnd;
    LPCSTR m_pszVerb;
    LPCTSTR m_pwszVerb;

	CComPtr<IInputObjectSite> m_spSite;
    CComPtr<IWebBrowser2> m_spWebBrowser;

    CBandToolBarReflectorCtrl m_ReflectWnd; // Used for Reflecting Messages

};

OBJECT_ENTRY_AUTO(__uuidof(JMToolband), CJMToolband)
