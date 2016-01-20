// Issues:
// How to detect Flash object with video
// ProcessFlashItems still can't infuse javascript into the page
// OnDocumentComplete doesn't get called after a refresh and you would lose the Flash DOM changes

#include "stdafx.h"
#include "BrowserHelper.h"
#include <mshtmcid.h> // Standard IE context menu id's
#include <atlrx.h> // For CAtlRegExp<>

#ifdef NOTUSED
enum MyNewBrowserNavConstants
{
	navOpenInNewTab = 0x800,
	navOpenInBackgroundTab = 0x1000,
};
#endif NOTUSED

#ifdef DocHostUIHandler
	#define IDR_BROWSE_CONTEXT_MENU		24641	// more <mshtmcid.h>
	#define IDM_EXTRA_ITEM				6047	// more <mshtmcid.h>
	#define IDR_FORM_CONTEXT_MENU		24640
	#define SHDVID_GETMIMECSETMENU		27
	#define SHDVID_ADDMENUEXTENSIONS	53
#endif DocHostUIHandler

// Initialize static variables held for the entire class
int CBrowserHelper::m_nTabs = 0;
HBITMAP CBrowserHelper::m_hMenuBitmap = NULL;
CSimpleMap<HWND, CBrowserHelper*> CBrowserHelper::m_arrBrowserHelper;

//////////////////////////////////////////////////////////////////////
bool IsVideoUrl(const CComBSTR& bstrUrl)
{
	LPCTSTR aSupportedVideoUrls[] = {
		"^.*youtube\\.com\\/watch\\?v\\=",
		"^.*myspace\\.com\\/index.cfm\\?fuseaction\\=vids",
		"^.*google\\.com\\/videoplay\\?docid\\=",
		"^.*break\\.com\\/index\\/",
		"^.*dailymotion\\.com\\/.*video",
		"^.*media\\.putfile\\.com",
//j		"^.*sevenload\\.com",
//j		"^.*metacafe\\.com",
//j		"^.*clipfish\\.de",
//j		"^.*myvideo\\.de",
	};

	CString strUrl = bstrUrl;
	CAtlRegExp<> regexp;
	int iLength = sizeof(aSupportedVideoUrls) / sizeof(aSupportedVideoUrls[0]);
	for (int i = 0; i < iLength; i++)
	{
		if (regexp.Parse(aSupportedVideoUrls[i]) != REPARSE_ERROR_OK)
			continue;
		CAtlREMatchContext<> mcUrl;
		bool bIsVideoUrl = !!regexp.Match(strUrl, &mcUrl);
		if (bIsVideoUrl)
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
CBrowserHelper::CBrowserHelper()
{	// A new browser helper object is created for each browser tab
	m_hWndTab = NULL;
	m_fnOldWndProc = NULL;
	m_bAdvisedDWebBrowserEvents2 = false;
#ifdef TEST
	m_bAdvisedHTMLElementEvents2 = false;
#endif TEST
	if (!m_nTabs && !m_hMenuBitmap)
		m_hMenuBitmap = ::LoadBitmap(_AtlBaseModule.GetModuleInstance(), MAKEINTRESOURCE(IDB_MENUITEM));
	m_nTabs++;
}

//////////////////////////////////////////////////////////////////////
CBrowserHelper::~CBrowserHelper()
{
	RemoveWindowContext();
	m_nTabs--;

	// The last tab is being closed
	if (!m_nTabs)
	{
		if (m_hMenuBitmap)
		{
			::DeleteObject(m_hMenuBitmap);
			m_hMenuBitmap = NULL;
		}

		int nSize = m_arrBrowserHelper.GetSize();
		m_arrBrowserHelper.RemoveAll();
	}
}

//////////////////////////////////////////////////////////////////////
// IBrowserHelper
HRESULT CBrowserHelper::LaunchMobilizeWindow(VARIANT varDispWindow, BSTR bstrURL, BSTR* plResult)
{
	if (!plResult)
		return E_FAIL;

	HINSTANCE hInst = ::ShellExecute(NULL/*hWnd*/, "open", CString(bstrURL), NULL/*lpParameters*/, NULL/*lpDirecory*/, SW_NORMAL/*nShowCommand*/);
	bool bOK = ((UINT)hInst > HINSTANCE_ERROR);
	*plResult = CComBSTR(bOK ? "OK" : "ERROR").Copy();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IBrowserHelper
HRESULT CBrowserHelper::LaunchMobilizePopup(VARIANT varDispWindow, BSTR bstrURL, BSTR bstrTargetName, BSTR bstrFeatures, BSTR* plResult)
{
	if (!plResult)
		return E_FAIL;

	*plResult = CComBSTR("ERROR").Copy();
	if (varDispWindow.vt != VT_DISPATCH)
		return S_OK;

	VARIANT_BOOL varReplace = VARIANT_TRUE;
	IHTMLWindow2* pHTMLWindow2 = NULL;
	CComQIPtr<IDispatch> spDispWindow = varDispWindow.pdispVal;
	CComQIPtr<IHTMLWindow2> spHTMLWindow2 = spDispWindow;
	if (spHTMLWindow2)
		spHTMLWindow2->open(bstrURL, bstrTargetName, bstrFeatures, varReplace, &pHTMLWindow2);

#ifdef NOTUSED //j
	CComPtr<IHTMLDocument2> spHTMLDocument2;
	if (pHTMLWindow2)
		pHTMLWindow2->get_document(&spHTMLDocument2);
	CComQIPtr<IOleWindow> spOleWindow = spHTMLDocument2;
	HWND hWnd = NULL;
	if (spOleWindow)
		spOleWindow->GetWindow(&hWnd);

	// Bring the newly launched window into the foreground
	if (hWnd)
	{
		// Find the top level window
		HWND hWndParent = NULL;;
		while (hWndParent = ::GetParent(hWnd))
			hWnd = hWndParent;
		::SetForegroundWindow(hWnd);
	}
#endif NOTUSED //j

	*plResult = CComBSTR("OK").Copy();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// IObjectWithSite
// Internet Explorer invokes this method exactly twice; once to establish a connection, and again as the browser is exiting
STDMETHODIMP CBrowserHelper::SetSite(IUnknown* pUnkSite)
{
	HRESULT hr = S_OK;
	if (pUnkSite)
	{
		// Cache the pointer to IWebBrowser2
		m_spWebBrowser2 = pUnkSite;

		// Register to sink events from DWebBrowserEvents2
		hr = CBrowserHelper::IDispEventSimpleImpl<1,CBrowserHelper,&DIID_DWebBrowserEvents2>::DispEventAdvise(m_spWebBrowser2);
		if (SUCCEEDED(hr))
			m_bAdvisedDWebBrowserEvents2 = true;
	}
	else
	{
		if (m_bAdvisedDWebBrowserEvents2)
		{ // Unregister event sink
			CBrowserHelper::IDispEventSimpleImpl<1,CBrowserHelper,&DIID_DWebBrowserEvents2>::DispEventUnadvise(m_spWebBrowser2);
			m_bAdvisedDWebBrowserEvents2 = false;
		}

		// Release cached pointers and other resources here
		m_spWebBrowser2.Release();
		m_spHTMLDocument2.Release();
	}

	// Return the base class implementation
	return IObjectWithSiteImpl<CBrowserHelper>::SetSite(pUnkSite);
}

//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK SubclassWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CBrowserHelper* pBrowserHelper = CBrowserHelper::GetBrowserHelper(hWnd);
	if (uMsg == WM_INITMENUPOPUP/*|| uMsg == WM_INITMENU || uMsg == WM_UNINITMENUPOPUP*/)
		bool bOK = (pBrowserHelper ? pBrowserHelper->OnInitMenuPopup((HMENU)wParam) : false);

	WNDPROC fnOldWndProc = (pBrowserHelper ? pBrowserHelper->GetOldWndProc() : NULL);
	return ::CallWindowProc(fnOldWndProc, hWnd, uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
bool CBrowserHelper::OnInitMenuPopup(HMENU hMenu)
{
	CMenuHandle menu = hMenu;
	if (!menu.IsMenu())
		return false;

	int nTargetPosition = 0;
	int nCount = menu.GetMenuItemCount();
	for (int iPosition=0; iPosition<nCount; iPosition++)
	{
		UINT id = menu.GetMenuItemID(iPosition);
		CString strMenuItem;
		int n = menu.GetMenuString(id, strMenuItem, MF_BYCOMMAND/*uFlags*/);
		bool bMobilizeFound = (strMenuItem.Find(_T("Mobilize")) >= 0);
		if (!bMobilizeFound)
			continue;

		// Check to see if the Mobilize Video! item should be removed
		bool bEnable = true;
		bool bMobilizeVideoFound = (strMenuItem.Find(_T("Video")) >= 0);
		if (bMobilizeVideoFound)
		{
			CComBSTR bstrUrl;
			if (m_spHTMLDocument2)
				m_spHTMLDocument2->get_URL(&bstrUrl);
			bEnable = IsVideoUrl(bstrUrl);
			menu.EnableMenuItem(id, MF_BYCOMMAND | (bEnable ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
			// Don't delete the menu item because it may need to be reused for a different page
			if (!bEnable)
				continue;
		}

		// If necessary, move the Mobilize item up in the menu
		if (iPosition != nTargetPosition)
		{
			MENUITEMINFO menuItemInfo;
			::ZeroMemory(&menuItemInfo, sizeof(menuItemInfo));
			menuItemInfo.cbSize = sizeof(MENUITEMINFO);
			TCHAR tcString[MAX_PATH];
			::ZeroMemory(&tcString, sizeof(tcString));
			menuItemInfo.dwTypeData = tcString;
			menuItemInfo.cch = MAX_PATH;
			menuItemInfo.fMask = MIIM_ID|MIIM_STATE|MIIM_SUBMENU|MIIM_CHECKMARKS|MIIM_TYPE|MIIM_DATA; // Others: MIIM_STRING, MIIM_BITMAP, MIIM_FTYPE
			bool bOK = !!menu.GetMenuItemInfo(iPosition, true/*bByPosition*/, &menuItemInfo);

			if (menu.DeleteMenu(id, MF_BYCOMMAND/*uFlags*/)) iPosition--;
			if (menu.InsertMenuItem(nTargetPosition/*nPosition*/, true/*bByPosition*/, &menuItemInfo)) iPosition++;
		}

		// Add the menu item bitmap
		bool bOK = !!menu.SetMenuItemBitmaps(id, MF_BYCOMMAND/*uFlags*/, CBrowserHelper::GetMenuBitmap(), NULL);

		nTargetPosition++;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CBrowserHelper::SaveWindowContext(HWND hWnd)
{
	if (!hWnd)
		return false;

	// Remove the window context in case a new document is being loaded into this window
	RemoveWindowContext();

	m_hWndTab = hWnd;
	m_fnOldWndProc = (WNDPROC)::SetWindowLongPtr(hWnd, GWL_WNDPROC, (LRESULT)SubclassWndProc);
	bool bOK = !!m_arrBrowserHelper.Add(m_hWndTab, this);
	return bOK;
}

//////////////////////////////////////////////////////////////////////
bool CBrowserHelper::RemoveWindowContext()
{
	// Set the old winproc back for this window
	if (m_hWndTab && m_fnOldWndProc)
		LRESULT lResult = ::SetWindowLongPtr(m_hWndTab, GWL_WNDPROC, (LRESULT)m_fnOldWndProc);

	bool bOK = false;
	if (m_hWndTab)
		bOK = !!m_arrBrowserHelper.Remove(m_hWndTab);

	m_hWndTab = NULL;
	m_fnOldWndProc = NULL;
	return bOK;
}

//////////////////////////////////////////////////////////////////////
void STDMETHODCALLTYPE CBrowserHelper::OnDocumentComplete(IDispatch* pDisp, VARIANT* pvarURL)
{
	HRESULT hr = S_OK;

	// Is this event associated with the top-level browser?
	CComQIPtr<IWebBrowser2> spTempWebBrowser = pDisp;
	if (!spTempWebBrowser || !m_spWebBrowser2 || !m_spWebBrowser2.IsEqualObject(spTempWebBrowser))
		return;

	// Get the current document object from the browser
	CComPtr<IDispatch> spDispDoc;
	m_spWebBrowser2->get_Document(&spDispDoc);
	CComQIPtr<IOleWindow> spOleWindow = spDispDoc;
	HWND hWnd = NULL;
	if (spOleWindow)
		spOleWindow->GetWindow(&hWnd);
	if (!hWnd)
		return;

	m_spHTMLDocument2 = spDispDoc;
	if (!m_spHTMLDocument2)
		return;

	SaveWindowContext(hWnd);

#ifdef DocHostUIHandler
	// The ICustomDoc interface is implemented by MSHTML to allow a host to set the MSHTML IDocHostUIHandler interface
	CComQIPtr<ICustomDoc> spCustomDoc = spDispDoc;
	if (spCustomDoc)
		spCustomDoc->SetUIHandler(this);
#endif DocHostUIHandler

	CComQIPtr<IOleObject> spOleObject = spDispDoc;
	if (spOleObject)
	{
		CComPtr<IOleClientSite> spClientSite;
		spOleObject->GetClientSite(&spClientSite);
		if (spClientSite)
		{
			#ifdef DocHostUIHandler
				m_spDefaultDocHostUIHandler = spClientSite;
			#endif DocHostUIHandler
			m_spDefaultOleCommandTarget = spClientSite;
		}
	}

//j	ProcessFlashItems();
}

//////////////////////////////////////////////////////////////////////
void CBrowserHelper::ProcessFlashItems()
{
	// Get the collection of items from the DOM
	CComPtr<IHTMLElementCollection> spAllItems;
	m_spHTMLDocument2->get_all(&spAllItems);
	if (!spAllItems)
		return;

	CComPtr<IDispatch> spDispItems;
    spAllItems->tags(CComVariant(CComBSTR("object")), &spDispItems);
    CComQIPtr<IHTMLElementCollection> spItems = spDispItems;
    if (!spItems)
		return;

	// Process all of the items in the collection
	long nItems = 0;
	spItems->get_length(&nItems);
	for (int i = 0; i < nItems; i++)
	{
		// Get the item out of the collection by index
		CComPtr<IDispatch> spDispItem;
		spItems->item(CComVariant(i), CComVariant(i), &spDispItem);
		CComQIPtr<IHTMLElement> spElement = spDispItem;
		if (!spElement)
			continue;

		CComBSTR bstrTagName;
		spElement->get_tagName(&bstrTagName);
		bstrTagName.ToLower();
		if (bstrTagName != "object")
			continue;

		CComPtr<IHTMLStyle> spStyle;
		spElement->get_style(&spStyle);
		if (!spStyle)
			continue;

		HRESULT hr = S_OK;
		CComVariant varFunction;
//j		hr = spElement->get_onmouseover(&varFunction);

#ifdef TEST
		bool bOK = ConnectEvents(spElement);
		CComVariant varReturn = CallJavaScript(CComBSTR("function() { alert('hi'); return; }"));
		varFunction = CreateWebEvtHandler(NULL/*hwnd*/, m_spHTMLDocument2, 0/*dwExtraData*/, 1/*lID*/, (IUnknown*)NULL, (void*)NULL/*UserData*/);
		hr = spElement->put_onmouseover(varFunction);
//j		hr = spElement->put_onmouseover(CComVariant(CComBSTR("function() { alert('hi'); return; }")));
		hr = spElement->get_onmouseover(&varFunction);
#endif TEST

#ifdef NOTUSED //j
		hr = spElement->put_onmouseover((CComBSTR("\
		function() \
		{ alert('hi'); return; \
//			if (document.all) \
//				e = event; \
//			if (e.target) \
//				source = e.target; \
//			else \
///			if (e.srcElement) \
//				source = e.srcElement; \
//			if (source.nodeType == 3) // defeat Safari bug \
//				source = source.parentNode; \
//			source.style.border = '1px solid red'; \
		} \
		")));
#endif NOTUSED //j

		// Set display="none" to hide the item
//j		spStyle->put_border(CComBSTR("1px solid red"));

		// Set display="none" to hide the item
//j		spStyle->put_display(CComBSTR("none"));
	}
}

#ifdef TEST
//////////////////////////////////////////////////////////////////////
bool CBrowserHelper::ConnectEvents(IHTMLElement* pElement)
{
	HRESULT hr = S_OK;

	// Register to sink events from HTMLElementEvents2
	hr = CBrowserHelper::IDispEventSimpleImpl<2,CBrowserHelper,&DIID_HTMLElementEvents2>::DispEventAdvise(pElement);
	if (SUCCEEDED(hr))
		m_bAdvisedHTMLElementEvents2 = true;

	// Check that this is a connectable object.
	CComQIPtr<IConnectionPointContainer> spConnectionPointContainer = pElement;
	if (!spConnectionPointContainer)
		return false;

	// Find the connection point.
	CComPtr<IConnectionPoint> spConnectionPoint;
	hr = spConnectionPointContainer->FindConnectionPoint(DIID_HTMLElementEvents2, &spConnectionPoint);
	if (!spConnectionPoint)
		return false;

	// Advise the connection point.
	// pUnk is the IUnknown interface pointer for your event sink
	DWORD dwCookie = 0;
	hr = spConnectionPoint->Advise(m_spWebBrowser2, &dwCookie);
	if (FAILED(hr))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////
CComVariant CBrowserHelper::CallJavaScript(CComBSTR& bstrJavascript)
{
	if (!m_spHTMLDocument2)
		return NULL;
	CComPtr<IHTMLWindow2> spHTMLWindow2;
	HRESULT hr = m_spHTMLDocument2->get_parentWindow(&spHTMLWindow2);
	if (!spHTMLWindow2)
		return NULL;
	CComVariant varReturn;
	hr = spHTMLWindow2->execScript(bstrJavascript, CComBSTR(_T("javascript")), &varReturn);
	return varReturn;
}

/************************ CreateWebEvtHandler() *********************
 * Creates an event handler, to be used to "attach to" some events that happpen with an element on the web page.
 *
 * hwnd - The window where messages will be sent when the event happens.
 *
 * pHTMLDocument2 - Pointer to an IHTMLDocument2 object. Objects that use the resulting event handler must be associated with this
 *			  (ie. either its parent window, itself or a child element).
 *
 * extraData -	sizeof() any application defined struct you wish prepended to the returned IDispatch. You can use
 *				GetWebExtraData() to fetch a pointer to this struct.  0 if no extra data needed.
 *
 * id -		Any numeric value of your choosing to be stored in the _IDispatchEx's "id" member. If a negative value, then the
 *			WEBPARAMS->eventStr will be set to the passed USERDATA instead of an event string.
 *
 * obj -	A pointer to any other object to be stored in the _IDispatchEx's "object" member. 0 if none.
 *
 * userdata -	If not zero, then this will be stored in the _IDispatchEx's "userdata" member.
 *
 * attachObj -	If not zero, then "userdata" is considered to be a BSTR of some event type to attach to, and "attachObj" is the
 *				
 * RETURNS: Pointer to the IDispatch if success, or 0 if an error.
 *
 * NOTE: "elem" will automatically be Release()'ed by this DLL when the _IDispatchEx is destroyed. It is also Release()'ed if this call fails.
 */
// Our _IDispatchEx struct. This is just an IDispatch with some extra fields appended to it for our use in storing extra
// info we need for the purpose of reacting to events that happen to some element on a web page.
typedef struct {
	IDispatch*		dispatchObj;	// The mandatory IDispatch.
	DWORD			refCount;		// Our reference count.
	IHTMLWindow2*	spHTMLWindow2;	// Where we store the IHTMLWindow2 so that our IDispatch's Invoke() can get it.
	HWND			hwnd;			// The window hosting the browser page. Our IDispatch's Invoke() sends messages when an event of interest occurs.
	short			id;				// Any numeric value of your choosing that you wish to associate with this IDispatch.
	unsigned short	extraSize;		// Byte size of any extra fields prepended to this struct.
	IUnknown*		object;			// Some object associated with the web page element this IDispatch is for.
	void*			userdata;		// An extra pointer.
} _IDispatchEx;

IDispatch* WINAPI CBrowserHelper::CreateWebEvtHandler(HWND hwnd, IHTMLDocument2* pHTMLDocument2, DWORD extraData, long id, IUnknown *obj, void *userdata)
{
	// Get the IHTMLWindow2. Our IDispatch's Invoke() will need it to cleanup
	CComPtr<IHTMLWindow2> spHTMLWindow2;
	pHTMLDocument2->get_parentWindow(&spHTMLWindow2);
	if (!spHTMLWindow2)
		return 0;

	CComQIPtr<IHTMLWindow3> spHTMLWindow3 = spHTMLWindow2;
	if (!spHTMLWindow3)
		return 0;

	CComVariant varDisp;

	// If he didn't pass any userdata, then we don't need the extra "userdata" member on the IDispatch
	if (!userdata && id >= 0)
		varDisp.vt -= sizeof(void *);

	// Create an IDispatch object (actually we create one of our own _IDispatchEx objects)
	// which we'll use to monitor "events" that occur to an element on a web page.
	// IE's engine will call our IDispatch's Invoke() function when it wants to inform
	// us that a specific event has occurred.
	_IDispatchEx* lpDispatchEx = NULL;;
	if (!(lpDispatchEx = (_IDispatchEx*)GlobalAlloc(GMEM_FIXED, sizeof(_IDispatchEx) + extraData + varDisp.vt)))
		return NULL;

	// Clear out the extradata area in case the caller wants that.
	::ZeroMemory(lpDispatchEx, extraData);

	// Skip past the extradata.
	lpDispatchEx = (_IDispatchEx *)((char *)lpDispatchEx + extraData);	

	// Fill in our _IDispatchEx with its VTable, and the args passed to us by the caller
//j	lpDispatchEx->dispatchObj.lpVtbl = &MyIDispatchVtbl;
	lpDispatchEx->hwnd = hwnd;
	lpDispatchEx->spHTMLWindow2 = spHTMLWindow2;
	lpDispatchEx->id = (short)id;
	lpDispatchEx->extraSize = (unsigned short)extraData;
	lpDispatchEx->object = obj;
	if (userdata)
		lpDispatchEx->userdata = userdata;

	// Now we attach our IDispatch to the "beforeunload" event so that our IDispatch's
	varDisp.vt = VT_DISPATCH;
	varDisp.pdispVal = (IDispatch *)lpDispatchEx;	

//	if (SUCCEEDED(spHTMLWindow3->attachEvent("onmouseover", (LPDISPATCH)lpDispatchEx, &varDisp)))
//	{
//		return((IDispatch*)lpDispatchEx);
//	}

	GlobalFree(((char*)lpDispatchEx - lpDispatchEx->extraSize));

	// Failure
	return 0;
}

#endif TEST

#ifdef DocHostUIHandler
//////////////////////////////////////////////////////////////////////
HRESULT CBrowserHelper::ShowContextMenu(DWORD dwID, POINT* ppt, IUnknown* punkOleCommandTarget) 
{
	CComQIPtr<IOleCommandTarget> spOleCommandTarget = punkOleCommandTarget;
	CComQIPtr<IOleWindow> spOleWindow = punkOleCommandTarget;
	HWND hWnd = NULL;
	spOleWindow->GetWindow(&hWnd);

	WNDPROC fnOldWndProc = (WNDPROC)::SetWindowLongPtr(hWnd, GWL_WNDPROC, (LRESULT)SubclassWndProc);

	HINSTANCE hinstSHDOCLC = ::LoadLibrary("shdoclc.dll");
	if (!hinstSHDOCLC)
		hinstSHDOCLC = ::LoadLibrary("shdocvw.dll");
	if (!hinstSHDOCLC)
		return S_FALSE; // Default behavior
	HMENU hMenu = LoadMenu(hinstSHDOCLC, MAKEINTRESOURCE(IDR_BROWSE_CONTEXT_MENU));
	hMenu = GetSubMenu(hMenu, dwID);
	if (!hMenu)
		return S_FALSE; // Default behavior

	// Get the language submenu
	CComVariant var;
	HRESULT hr = spOleCommandTarget->Exec(&CGID_ShellDocView, SHDVID_GETMIMECSETMENU, 0, NULL, &var);

	// Insert Shortcut Menu Extensions from registry
	CComVariant varhMenu;
	varhMenu = (int*)hMenu;
	CComVariant vardwID = dwID;
	hr = spOleCommandTarget->Exec(&CGID_ShellDocView, SHDVID_ADDMENUEXTENSIONS, 0, &varhMenu, &vardwID);

	// Show shortcut menu
	int iSelection = ::TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, ppt->x, ppt->y, 0, hWnd, (RECT*)NULL);

	// Send selected shortcut menu item command to shell
	if (iSelection)
		LRESULT lr = ::SendMessage(hWnd, WM_COMMAND, iSelection, NULL);

	FreeLibrary(hinstSHDOCLC);
	return S_OK;
}
#endif DocHostUIHandler
