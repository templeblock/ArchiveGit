#include "stdafx.h"
#include "HTMLDialogEx.h"
#include "WebPage.h"

CHTMLDialogEx* CHTMLDialogEx::g_pHtmlDialog = NULL;

/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CHTMLDialogEx::BrowserKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!g_pHtmlDialog)
		return false;

	HHOOK hHook = g_pHtmlDialog->GetKeyboardHook();
	if (!hHook)
		return false;

	if (nCode < 0)
		return ::CallNextHookEx(hHook, nCode, wParam, lParam);

	UINT nChar = wParam; // The virtual key code
	if (nCode < 0 || !g_pHtmlDialog->IsActive())
	{ 
		// This key belongs to the browser
		if (nChar >= VK_F1 && nChar <= VK_F12 && nCode == HC_ACTION) // Steal F7-F12 from the browser
			g_pHtmlDialog->SetFocus();
		else
			return ::CallNextHookEx(hHook, nCode, wParam, lParam);
	}
	
	WORD nRepeatCount = LOWORD(lParam);
	WORD wFlags = HIWORD(lParam);
	bool bKeyDown	=  !(wFlags & KF_UP);
	bool bRepeat	= !!(wFlags & KF_REPEAT);
	bool bAltDown	= !!(wFlags & KF_ALTDOWN);

	switch (nChar)
	{
		case VK_DELETE:
		case VK_TAB:
		case VK_DOWN:
		case VK_UP:
		case VK_RIGHT:
		case VK_LEFT:
		case VK_RETURN:
		{
			CComPtr<IWebBrowser2> spBrowser;
			spBrowser = g_pHtmlDialog->GetBrowser();
			if (spBrowser)
			{
				DWORD dwMsg = WM_KEYUP;
				if (bKeyDown)
					dwMsg = WM_KEYDOWN;

				CComPtr<IOleInPlaceActiveObject> spIOIPAO;
				spIOIPAO = spBrowser;
				HRESULT hr;
				if (spIOIPAO)
				{
					DWORD dwPos = GetMessagePos();
					POINT pt;
					pt.x = LOWORD(dwPos);
					pt.y = HIWORD(dwPos);
					MSG msg;
					HWND hwnd = g_pHtmlDialog->GetBrowserWindow();
					msg.hwnd  = hwnd;
					msg.message = dwMsg;
					msg.wParam = wParam;
					msg.lParam = lParam;
					msg.time = GetMessageTime();
					msg.pt = pt;
					hr = spIOIPAO->TranslateAccelerator(&msg);
				}

				spIOIPAO = NULL;
				spBrowser = NULL;
			}
		}
	}

	return ::CallNextHookEx(hHook, nCode, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
CHTMLDialogEx::CHTMLDialogEx()
{
	Init();	
}

//////////////////////////////////////////////////////////////////////
CHTMLDialogEx::CHTMLDialogEx(const CString& strURL, BOOL bRes, HWND hWnd)
{
	Init();

	m_hWndParent = hWnd;

	if (bRes)
		ResourceToURL(strURL);
	else
		m_strURL = strURL;
}

/////////////////////////////////////////////////////////////////////////////
CHTMLDialogEx::~CHTMLDialogEx()
{
	if (IsWindow())
		DestroyWindow();

	Free();
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::RegisterCallback(PFNBROWSERCALLBACK pfnBrowserCallback, LPARAM lParam, LPCTSTR szValue)
{
	if (!pfnBrowserCallback)
	{
		m_pfnBrowserCallback = NULL;
		return;
	}

	m_pfnBrowserCallback = pfnBrowserCallback;
	m_pCallbackParam = lParam;
	m_szID = szValue;
}

//////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::UnregisterCallback()
{
	m_pfnBrowserCallback = NULL;
	m_pCallbackParam = 0;
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::DoModeless(HWND hWndParent)
{
	if (IsWindow())
	{
		BringWindowToTop();
		ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);
		return;
	}

	while (hWndParent)
	{
		DWORD dwStyle = ::GetWindowLong(hWndParent, GWL_STYLE);
		bool bIsApp = ((dwStyle & WS_CAPTION) == WS_CAPTION);
		if (bIsApp)
			break;

		hWndParent = ::GetParent(hWndParent);
	}
	
	if (hWndParent)
	{
		// Sanity check; it should have a title
		char strTitle[MAX_PATH];
		strTitle[0] = '\0';
		::GetWindowText(hWndParent, strTitle, sizeof(strTitle));
		if (!strTitle[0])
			hWndParent = NULL;
	}

	// Create the dialog
	m_bModal = false;
	m_hWndParent = hWndParent;
	Create(hWndParent);
}

///////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::Init()
{
	m_pDispatch = NULL;
	m_pfnBrowserCallback = NULL;
	m_pCallbackParam = NULL;
	m_hKeyboardHook = NULL;
	g_pHtmlDialog = NULL;
	m_cx = 600;
	m_cy = 400;
	m_xpos = 0;
	m_ypos = 0;
	m_bModal = true;
	m_bHasFocus = false;
	m_bIsActive = false;
	m_nShowCmd = SW_SHOW;
	m_hWndParent = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::Free()
{
	UnregisterCallback();

	m_pDispatch = NULL;
	m_strURL = "";

	if (m_spBrowser)
        Disconnect();

	m_spBrowser = NULL;
	m_BrowserWnd = NULL;
	g_pHtmlDialog = NULL;

	if (m_hKeyboardHook)
	{
		::UnhookWindowsHookEx(m_hKeyboardHook);
		m_hKeyboardHook = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyleEx(WS_EX_APPWINDOW|WS_EX_CONTROLPARENT/*dwRemove*/, 0/*dwAdd*/);

	DlgResize_Init(false/*bAddGripper*/, true/*bUseMinTrackSize*/, WS_CLIPCHILDREN/*dwForceStyle*/);
		
	// Create the web browser control
	m_BrowserWnd = GetDlgItem(IDC_EXPLORER1);
	if (!::IsWindow(m_BrowserWnd.m_hWnd))
		return TRUE;

	HRESULT hr = m_BrowserWnd.QueryControl(&m_spBrowser);
	ATLASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return IDOK;

	if (m_pDispatch)
		hr = m_BrowserWnd.SetExternalDispatch(m_pDispatch);
	ATLASSERT(SUCCEEDED(hr));
	
	if (m_spBrowser)
	{
		m_spBrowser->put_Silent(VARIANT_FALSE); // Allow dialog boxes such as security alerts.
		m_spBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);
		Connect(m_spBrowser); // Connect DWebBrowserEvents
	}

	SetURL(m_strURL, true/*bNavigate*/);
	SetWindowText(m_szTitle);
	SetWindowPos(NULL, m_xpos, m_ypos, m_cx, m_cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
	
	if (m_pfnBrowserCallback)
		m_pfnBrowserCallback(m_hWnd, WM_INITDIALOG, m_pCallbackParam, m_szID);
	
	// Set keyboard hook to catch keyboard strokes from browser
	m_hKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD, CHTMLDialogEx::BrowserKeyboardProc, NULL, GetCurrentThreadId());
	g_pHtmlDialog = this;

	ShowWindow(m_nShowCmd);

	return IDOK;  // Let the system set the focus
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	g_pHtmlDialog = this;
	m_bHasFocus = true;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bHasFocus = false;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool bActivate = (LOWORD(wParam) != WA_INACTIVE);
	m_bIsActive = bActivate;
	if (m_bIsActive)
		g_pHtmlDialog = this;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pfnBrowserCallback)
		m_pfnBrowserCallback(m_hWnd, WM_CLOSE, m_pCallbackParam, m_szID);
	
	if (m_bModal)
	{
		EndDialog(IDOK);
	}
	else
	{
		if (::IsWindow(m_hWnd))
			DestroyWindow();
	}	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pfnBrowserCallback)
		m_pfnBrowserCallback(m_hWnd, WM_DESTROY, m_pCallbackParam, m_szID);
	
	Free();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::SetWindowSize(int cx, int cy)
{
	m_cx = cx;
	m_cy = cy;
	if (IsWindow())
		SetWindowPos(NULL, 0, 0, m_cx, m_cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::SetWindowPosition(int x, int y)
{
	m_xpos = x;
	m_ypos = y;
	if (IsWindow())
		SetWindowPos(NULL, m_xpos, m_ypos, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::SetDispatch(IDispatch* pDispatch)
{
	if (IsWindow())
		return;

	m_pDispatch = pDispatch;
}

///////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::SetURL(LPCSTR pstrURL, bool bNavigate)
{
	m_strURL = pstrURL;

	if (bNavigate && m_spBrowser != NULL)
	{
		CComVariant varFlags = navNoHistory |  navNoReadFromCache | navNoWriteToCache;
		CComVariant varTargetFrameName = "_self";
		HRESULT hr = m_spBrowser->Navigate(CComBSTR(pstrURL), 
									&varFlags, 
									&varTargetFrameName, 
									NULL/*&varPostData*/, 
									NULL/*&varHeaders*/);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::SetWindowTitle(LPCTSTR szTitle)
{
	m_szTitle = szTitle;
	if (IsWindow())
		SetWindowText(szTitle);
}

/////////////////////////////////////////////////////////////////////////////
bool CHTMLDialogEx::SetWindowShowCommand(int nCmd)
{
	m_nShowCmd = nCmd;
	if (!IsWindow())
		return false;

	ShowWindow(m_nShowCmd);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::TitleChange(BSTR Text)
{
	CString strText = Text;

	if (strText.Left(7) == "file://" || strText.Left(7) == "http://")
		strText.Empty();

	if (strText.IsEmpty())
		return;

	SetWindowText(strText);
}

/////////////////////////////////////////////////////////////////////////////
bool CHTMLDialogEx::CallJavaScript(LPCTSTR szFunction, LPCTSTR szArg1)
{
	if (NULL == m_spBrowser)
		return false;

	CComPtr<IDispatch> spDisp;
	HRESULT hr = m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2> spHtmlDoc(spDisp);
	if (!spHtmlDoc)
		return false;

	CWebPage WebPage;
	
	if (!WebPage.SetDocument(spHtmlDoc))
		return false;

	if (CString(szArg1).IsEmpty())
	{
		if (!WebPage.CallJScript(szFunction))
			return false;
	}
	else
	{
		if (!WebPage.CallJScript(szFunction, szArg1))
			return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::ResourceToURL(const LPCTSTR szURL)
{
	HMODULE hModule = _AtlBaseModule.GetResourceInstance();

	LPTSTR lpszModule = new TCHAR[MAX_PATH];
	if (GetModuleFileName(hModule, lpszModule, MAX_PATH))
		m_strURL.Format(_T("res://%s/%s"), lpszModule, szURL);
	
	delete [] lpszModule;
}
