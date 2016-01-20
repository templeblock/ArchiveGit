#pragma once
#include "resource.h"
#include <shlguid.h>	// IID_IWebBrowser2, DIID_DWebBrowserEvents2
#include <mshtml.h>		// DOM interfaces
#include <exdispid.h>	// DISPID_DOCUMENTCOMPLETE, etc.
#ifdef TEST
	#include <mshtmdid.h>	// DISPID_HTMLELEMENTEVENTS2_ONMOUSEOVER, etc.
#endif TEST

//#define DocHostUIHandler
//#define TEST

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// IBrowserHelper
[
	object,
	uuid("34D909E0-1FD5-49BF-9A77-0CA6788A303F"),
	dual,
	helpstring("IBrowserHelper Interface"),
	pointer_default(unique)
]
__interface IBrowserHelper : public IDispatch
{
	// Methods
	[id(1)] HRESULT LaunchMobilizeWindow([in]VARIANT varDispWindow, [in]BSTR bstrURL, [out, retval]BSTR* plResult);
	[id(2)] HRESULT LaunchMobilizePopup([in]VARIANT varDispWindow, [in]BSTR bstrURL, [in]BSTR bstrTargetName, [in]BSTR bstrFeatures, [out, retval]BSTR* plResult);
};

// CBrowserHelper

[
	coclass,
	default(IBrowserHelper),
	threading(apartment),
	vi_progid("a3GPlugin.BrowserHelper"),
	progid("a3GPlugin.BrowserHelper.1"),
	version(1.0),
	uuid("D29F1EC9-E53A-4A05-8A8C-5BB2E58E3E3D"),
	helpstring("BrowserHelper Class")
]
class ATL_NO_VTABLE CBrowserHelper :
	public IBrowserHelper,
	public IOleCommandTarget, // To interact with Internet Explorer as an application
	public IDispatchImpl<DWebBrowserEvents2, &__uuidof(DWebBrowserEvents2), &LIBID_SHDocVw, /*wMajor*/1, /*wMinor*/ 1>,
	public IDispEventImpl<1, CBrowserHelper, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>,
#ifdef TEST
	public IDispatchImpl<HTMLElementEvents2, &__uuidof(HTMLElementEvents2), &LIBID_MSHTML, /*wMajor*/1, /*wMinor*/ 0>,
	public IDispEventImpl<2, CBrowserHelper, &DIID_HTMLElementEvents2, &LIBID_MSHTML, 1, 0>,
#endif TEST
#ifdef DocHostUIHandler
	public IDocHostUIHandler, // To access the IE context menu
#endif DocHostUIHandler
	public IObjectWithSiteImpl<CBrowserHelper> // To become a BHO
{
public:
	CBrowserHelper();
	~CBrowserHelper();
	static const TCHAR* GetObjectFriendlyName() 
	{
		return _T("Mobilize web content to your 3Guppies locker!");
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	BEGIN_SINK_MAP(CBrowserHelper)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
#ifdef TEST
		SINK_ENTRY_EX(2, DIID_HTMLElementEvents2, DISPID_HTMLELEMENTEVENTS2_ONMOUSEOVER, OnClick)
#endif TEST
	END_SINK_MAP()

private:
	void ProcessFlashItems();

#ifdef TEST
	bool ConnectEvents(IHTMLElement* pElement);
	CComVariant CallJavaScript(CComBSTR& bstrJavascript);
	IDispatch* WINAPI CreateWebEvtHandler(HWND hwnd, IHTMLDocument2* htmlDoc2, DWORD extraData, long id, IUnknown *obj, void *userdata);
#endif TEST

public:
	// IBrowserHelper
	HRESULT IBrowserHelper::LaunchMobilizeWindow(VARIANT varDispWindow, BSTR bstrURL, BSTR* plResult);
	HRESULT IBrowserHelper::LaunchMobilizePopup(VARIANT varDispWindow, BSTR bstrURL, BSTR bstrTargetName, BSTR bstrFeatures, BSTR* plResult);

	// IObjectWithSite
	STDMETHODIMP IObjectWithSite::SetSite(IUnknown* pUnkSite);

#ifdef DocHostUIHandler
	HRESULT ShowContextMenu(DWORD dwID, POINT* ppt, IUnknown* punkOleCommandTarget);

	// IDocHostUIHandler
	// ShowContextMenu Called by MSHTML to display a shortcut menu
	STDMETHODIMP IDocHostUIHandler::ShowContextMenu(DWORD dwID, POINT* ppt, IUnknown* punkOleCommandTarget, IDispatch* pdispObject)
	{
		HRESULT hr = S_FALSE; // Default behavior
		switch (dwID)
		{
			case CONTEXT_MENU_DEFAULT:
			{
				return ShowContextMenu(dwID, ppt, punkOleCommandTarget);
			}

			case CONTEXT_MENU_IMAGE:
			case CONTEXT_MENU_CONTROL:
			case CONTEXT_MENU_TABLE:
			case CONTEXT_MENU_TEXTSELECT:
			case CONTEXT_MENU_ANCHOR:
			case CONTEXT_MENU_UNKNOWN:
			case CONTEXT_MENU_IMGDYNSRC:
			case CONTEXT_MENU_IMGART:
			case CONTEXT_MENU_DEBUG:
			case CONTEXT_MENU_VSCROLL:
			case CONTEXT_MENU_HSCROLL:
			default:
			{
				break;
			}
		}

		return hr; // S_OK = no default context menu is displayed.
	}

	// ShowUI Called by MSHTML to enable the host to replace MSHTML menus and toolbars
	STDMETHODIMP IDocHostUIHandler::ShowUI(DWORD dwID, IOleInPlaceActiveObject* pActiveObject, IOleCommandTarget* pCommandTarget, IOleInPlaceFrame* pFrame, IOleInPlaceUIWindow* pDoc)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->ShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
		return S_FALSE;
	}

	// GetHostInfo Called by MSHTML to retrieve the user interface (UI) capabilities of the application that is hosting MSHTML
	STDMETHODIMP IDocHostUIHandler::GetHostInfo(DOCHOSTUIINFO* pInfo)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->GetHostInfo(pInfo);
		return E_NOTIMPL;
	}

	// HideUI Called when MSHTML removes its menus and toolbars
	STDMETHODIMP IDocHostUIHandler::HideUI(void)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->HideUI();
		return E_NOTIMPL;
	}

	// UpdateUI Called by MSHTML to notify the host that the command state has changed
	STDMETHODIMP IDocHostUIHandler::UpdateUI(void)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->UpdateUI();
		return E_NOTIMPL;
	}

	// EnableModeless Called by the MSHTML implementation of IOleInPlaceActiveObject::EnableModeless. Also called when MSHTML displays a modal UI
	STDMETHODIMP IDocHostUIHandler::EnableModeless(BOOL fEnable)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->EnableModeless(fEnable);
		return E_NOTIMPL;
	}

	// OnDocWindowActivate Called by the MSHTML implementation of IOleInPlaceActiveObject::OnDocWindowActivate
	STDMETHODIMP IDocHostUIHandler::OnDocWindowActivate(BOOL fActivate)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->OnDocWindowActivate(fActivate);
		return E_NOTIMPL;
	}

	// OnFrameWindowActivate Called by the MSHTML implementation of IOleInPlaceActiveObject::OnFrameWindowActivate
	STDMETHODIMP IDocHostUIHandler::OnFrameWindowActivate(BOOL fActivate)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->OnFrameWindowActivate(fActivate);
		return E_NOTIMPL;
	}

	// ResizeBorder Called by the MSHTML implementation of IOleInPlaceActiveObject::ResizeBorder
	STDMETHODIMP IDocHostUIHandler::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow* pUIWindow, BOOL fFrameWindow)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->ResizeBorder(prcBorder, pUIWindow, fFrameWindow);
		return E_NOTIMPL;
	}

	// TranslateAccelerator Called by MSHTML when IOleInPlaceActiveObject::TranslateAccelerator or IOleControlSite::TranslateAccelerator is called
	STDMETHODIMP IDocHostUIHandler::TranslateAccelerator(LPMSG lpMsg, const GUID* pguidCmdGroup, DWORD nCmdID)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
		return E_NOTIMPL;
	}

	// GetOptionKeyPath Called by the WebBrowser Control to retrieve a registry subkey path that overrides the default Internet Explorer registry settings
	STDMETHODIMP IDocHostUIHandler::GetOptionKeyPath(LPOLESTR* pchKey, DWORD dw)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->GetOptionKeyPath(pchKey, dw);
		if (!IsBadWritePtr(pchKey, sizeof(*pchKey))) *pchKey = NULL;
		return E_NOTIMPL;
	}

	// GetDropTarget Called by MSHTML when it is used as a drop target. This method enables the host to supply an alternative IDropTarget interface
	STDMETHODIMP IDocHostUIHandler::GetDropTarget(IDropTarget* pDropTarget, IDropTarget** ppDropTarget)
	{
		if (m_spDefaultDocHostUIHandler)
		{
			HRESULT hr = m_spDefaultDocHostUIHandler->GetDropTarget(pDropTarget, ppDropTarget);
			// Returning S_FALSE seems to disable DragNDrop, while DragNDrop is by default on
			// Changing return code to E_FAIL seems to fix things. 
			return (hr == S_FALSE ? E_FAIL : hr);
		}

		if (!IsBadWritePtr(ppDropTarget, sizeof(*ppDropTarget))) *ppDropTarget = NULL;
		return E_NOTIMPL;
	}

	// GetExternal Called by MSHTML to obtain the host's IDispatch interface
	STDMETHODIMP IDocHostUIHandler::GetExternal(IDispatch** ppDispatch)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->GetExternal(ppDispatch);
		if (!IsBadWritePtr(ppDispatch, sizeof(*ppDispatch))) *ppDispatch = NULL;
		return E_NOTIMPL;
	}

	// TranslateUrl Called by MSHTML to give the host an opportunity to modify the URL to be loaded
	STDMETHODIMP IDocHostUIHandler::TranslateUrl(DWORD dwTranslate, OLECHAR* pchURLIn, OLECHAR** ppchURLOut)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->TranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
		if (!IsBadWritePtr(ppchURLOut, sizeof(*ppchURLOut))) *ppchURLOut = NULL;
		return E_NOTIMPL;
	}

	// FilterDataObject Called by MSHTML to allow the host to replace the MSHTML data object
	STDMETHODIMP IDocHostUIHandler::FilterDataObject(IDataObject* pDO, IDataObject** ppDORet)
	{
		if (m_spDefaultDocHostUIHandler)
			return m_spDefaultDocHostUIHandler->FilterDataObject(pDO, ppDORet);
		if (!IsBadWritePtr(ppDORet, sizeof(*ppDORet))) *ppDORet = NULL;
		return E_NOTIMPL;
	}
#endif DocHostUIHandler

	// IOleCommandTarget
 	STDMETHODIMP IOleCommandTarget::QueryStatus(const GUID* pguidCmdGroup, ULONG cCmds, OLECMD* prgCmds, OLECMDTEXT* pCmdText)
	{
		if (m_spDefaultOleCommandTarget)
			return m_spDefaultOleCommandTarget->QueryStatus(pguidCmdGroup, cCmds, prgCmds, pCmdText);
		return E_NOTIMPL;
	}

	STDMETHODIMP IOleCommandTarget::Exec(const GUID* pguidCmdGroup, DWORD nCmdID, DWORD nCmdExecOpt, VARIANTARG* pvaIn, VARIANTARG* pvaOut)
	{
		if (m_spDefaultOleCommandTarget)
			return m_spDefaultOleCommandTarget->Exec(pguidCmdGroup, nCmdID, nCmdExecOpt, pvaIn, pvaOut);
		return E_NOTIMPL;
	}

public:
#ifdef TEST
	// HTMLElementEvents2
	void STDMETHODCALLTYPE OnClick(IHTMLEventObj *pEvtObj)
	{
		return;
	}
#endif TEST

	// DWebBrowserEvents2
	void STDMETHODCALLTYPE OnDocumentComplete(IDispatch* pDisp, VARIANT* pvarURL); 
	//void STDMETHODCALLTYPE StatusTextChange(BSTR Text);
	//void STDMETHODCALLTYPE ProgressChange(long Progress, long ProgressMax);
	//void STDMETHODCALLTYPE CommandStateChange(long Command, BOOL Enable);
	//void STDMETHODCALLTYPE DownloadBegin();
	//void STDMETHODCALLTYPE DownloadComplete();
	//void STDMETHODCALLTYPE TitleChange(BSTR Text);
	//void STDMETHODCALLTYPE PropertyChange(BSTR szProperty);
	//void STDMETHODCALLTYPE BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
	//void STDMETHODCALLTYPE NewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	//void STDMETHODCALLTYPE NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL);
	//void STDMETHODCALLTYPE DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	//void STDMETHODCALLTYPE OnQuit();
	//void STDMETHODCALLTYPE OnVisible(BOOL Visible);
	//void STDMETHODCALLTYPE OnToolBar(BOOL ToolBar);
	//void STDMETHODCALLTYPE OnMenuBar(BOOL MenuBar);
	//void STDMETHODCALLTYPE OnStatusBar(BOOL StatusBar);
	//void STDMETHODCALLTYPE OnFullScreen(BOOL FullScreen);
	//void STDMETHODCALLTYPE OnTheaterMode(BOOL TheaterMode);
	//void STDMETHODCALLTYPE WindowSetResizable(BOOL Resizable);
	//void STDMETHODCALLTYPE WindowSetLeft(long Left);
	//void STDMETHODCALLTYPE WindowSetTop(long Top);
	//void STDMETHODCALLTYPE WindowSetWidth(long Width);
	//void STDMETHODCALLTYPE WindowSetHeight(long Height);
	//void STDMETHODCALLTYPE WindowClosing(BOOL IsChildWindow, BOOL* Cancel);
	//void STDMETHODCALLTYPE ClientToHostWindow(long* CX, long* CY);
	//void STDMETHODCALLTYPE SetSecureLockIcon(long SecureLockIcon);
	//void STDMETHODCALLTYPE FileDownload(BOOL* Cancel);
	//void STDMETHODCALLTYPE NavigateError(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Frame, VARIANT* StatusCode, BOOL* Cancel);
	//void STDMETHODCALLTYPE PrintTemplateInstantiation(LPDISPATCH pDisp);
	//void STDMETHODCALLTYPE PrintTemplateTeardown(LPDISPATCH pDisp);
	//void STDMETHODCALLTYPE UpdatePageStatus(LPDISPATCH pDisp, VARIANT* nPage, VARIANT* fDone);
	//void STDMETHODCALLTYPE PrivacyImpactedStateChange(BOOL bImpacted);
	//void STDMETHODCALLTYPE NewWindow3(LPDISPATCH* ppDisp, BOOL* Cancel, unsigned long dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);

public:
	HWND GetWndTab() { return m_hWndTab; }
	WNDPROC GetOldWndProc() { return m_fnOldWndProc; }
	bool OnInitMenuPopup(HMENU hMenu);
	static int GetNumTabs() { return m_nTabs; }
	static HBITMAP GetMenuBitmap() { return m_hMenuBitmap; }
	static CBrowserHelper* GetBrowserHelper(HWND hWnd) { return CBrowserHelper::m_arrBrowserHelper.Lookup(hWnd); }

private:
	bool SaveWindowContext(HWND hWnd);
	bool RemoveWindowContext();

private:
	HWND m_hWndTab;
	WNDPROC m_fnOldWndProc;
	static int m_nTabs;
	static HBITMAP m_hMenuBitmap;
	static CSimpleMap<HWND, CBrowserHelper*> m_arrBrowserHelper;

private:
	CComQIPtr<IWebBrowser2> m_spWebBrowser2;
	CComQIPtr<IHTMLDocument2> m_spHTMLDocument2;
#ifdef DocHostUIHandler
	CComQIPtr<IDocHostUIHandler> m_spDefaultDocHostUIHandler;
#endif DocHostUIHandler
	CComQIPtr<IOleCommandTarget> m_spDefaultOleCommandTarget;
	bool m_bAdvisedDWebBrowserEvents2;
#ifdef TEST
	bool m_bAdvisedHTMLElementEvents2;
#endif TEST
};
