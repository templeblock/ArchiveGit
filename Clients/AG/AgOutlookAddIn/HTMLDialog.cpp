#include "stdafx.h"
#include "HTMLDialog.h"
#include "MsAddInBase.h"
#include "Internet.h"
#include "FunctionObject.h"

#define ID_HTMLCONTROL 9999
/////////////////////////////////////////////////////////////////////////////
CHTMLDialog::CHTMLDialog(CMsAddInBase* pAddin)
{
	m_pAddin = pAddin;
	m_pDispatch = NULL;
	m_bBrowserError = false;
	m_bWaiting = false;
	m_hBrowserWnd = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CHTMLDialog::~CHTMLDialog()
{
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::Popup(HWND hWndParent, IDispatch* pDispatch, LPCSTR pstrURL, RECT *pRect, bool bActivate)
{
	CString strURL = pstrURL;

	// Determine if cached line-list needs to be refreshed.
	m_pAddin->GetSmartCache().CheckLineListCache();

	if (IsWindow())
	{
		SetWindowText(g_szAppName);
		if (bActivate)
			BringWindowToTop();
		ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);

	//j Since the dialog may have navigated to another URL on its own,
	//j this check doesn't always work - so just set the URL every time 
	//j	if (GetURL() != strURL)
	//j		SetURL(strURL, true/*bNavigate*/);

		// If this url conatins a new content type then we navigate to it. 
		CString szNewContentType = ExtractContentTypeFromUrl(pstrURL);
		if (szNewContentType != m_szCurrentContentType)
		{
			ShowPage(m_pBrowser, PAGE_WAIT);
			SetURL(strURL, true/*bNavigate*/);
		}

	//j	// Do not do a SetParent(hWndParent) - doesn't work
	//j	SetWindowLong(GWL_HWNDPARENT, (long)hWndParent);

		return;
	}

	// Create the HTML dialog
	// Find the Outlook app window
	// Walk up the parent chain to find a window with a caption; this should be the app
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
	SetURL(strURL, false/*bNavigate*/);
	SetDispatch(pDispatch);

	if (pRect == NULL)
		Create(hWndParent);
	else
		Create(hWndParent, *pRect);


	HICON hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(), (LPCTSTR)MAKEINTRESOURCE(IDI_LOGO), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED);
	SetIcon(hIcon, true/*bBigIcon*/);
	hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(), (LPCTSTR)MAKEINTRESOURCE(IDI_LOGO), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED);
	SetIcon(hIcon, false/*bBigIcon*/);

	//ShowPage(m_pBrowser, PAGE_WAIT);
	SetURL(strURL, true/*bNavigate*/);
}
/////////////////////////////////////////////////////////////////////////////
CString CHTMLDialog::ExtractContentTypeFromUrl(LPCTSTR szUrl)
{
	CString szType(szUrl);
	szType.MakeLower();
	int iIndex = szType.Find(_T("?type="));
	if (iIndex < 0)
		return "";

	// Extract type value (3 characters after '=', i.e. "?type=smi")
	CString szTemp = szType.Mid(iIndex+6, 3);
	return szTemp;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Restore the saved window size and position
	RECT rect = {0, 0, DEFAULT_BROWSE_WIDTH , DEFAULT_BROWSE_HEIGHT};
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		DWORD dwLength = sizeof(rect);
		RECT rectLocation = rect;
		if (regkey.QueryBinaryValue(REGVAL_LOCATION, &rectLocation, &dwLength) == ERROR_SUCCESS && dwLength == sizeof(rectLocation))
			rect = rectLocation;
	}

	//j Someday, clip the rect to the display
	HWND hwndInsertAfter = NULL;
	CAuthenticate Auth;
	if (Auth.IsTopMost())
		hwndInsertAfter = HWND_TOPMOST;
	SetWindowPos(hwndInsertAfter, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOACTIVATE);

	GetClientRect(&rect);

	ModifyStyleEx(WS_EX_APPWINDOW|WS_EX_CONTROLPARENT/*dwRemove*/, 0/*dwAdd*/);

	// Create the web browser control
	CString szPath;
	GetPagePath(PAGE_WAIT, szPath);

	CAxWindow wnd;
	m_hBrowserWnd = wnd.Create(m_hWnd, rect, szPath, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0/*WS_EX_CLIENTEDGE*/, ID_HTMLCONTROL);
	HRESULT hr = wnd.SetExternalDispatch(m_pDispatch);
	if (SUCCEEDED(hr))
	{
		hr = wnd.QueryControl(&m_pBrowser);
		if (m_pBrowser)
			hr = m_pBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);
	}

	Connect(m_pBrowser);
	DragAcceptFiles(false);
	wnd.DragAcceptFiles(false);


	//Wait(1000);

	return 1;  // Let the system set the focus
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialog::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_hWnd || !m_pDispatch)
		return S_OK;

	// wParam values: SIZE_MAXIMIZED, SIZE_MINIMIZED, SIZE_RESTORED
	if (wParam == SIZE_MINIMIZED)
		return S_OK;

	// Get the new client size
	int iWidth = LOWORD(lParam);
	int iHeight = HIWORD(lParam);

	HWND hWndBrowseControl = GetDlgItem(ID_HTMLCONTROL);
	if (hWndBrowseControl)
	{ // Maintained to be the full size of the client area
		RECT rect = {0, 0, iWidth, iHeight};
		::SetWindowPos(hWndBrowseControl, NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialog::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = false;

	if (!m_pAddin->GetSmartCache().IsUseSmartCache())
		return S_OK;

	if (!m_hWnd || !m_pDispatch)
		return S_OK;

	bool bActivate = (LOWORD(wParam) != WA_INACTIVE);
	m_pAddin->GetSmartCache().SetRestoreSyncMode(bActivate);
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialog::OnCMActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (LOWORD(wParam) != WA_INACTIVE)
	{
		
		HWND hwnd = (HWND)wParam;
		HWND hParent = ::GetParent(hwnd);
		//ShowWindow(SW_SHOWNOACTIVATE);
		::SetWindowPos(m_hWnd, hParent, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	}
			
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialog::OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	char szDroppedFile[MAX_PATH];
	HDROP hDrop = (HDROP)wParam;
	int iFile = -1; // pass in -1 to get the number of files dropped, otherwise the file index
	int nFiles = DragQueryFile(hDrop, iFile, szDroppedFile, sizeof(szDroppedFile));
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialog::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	#define CONTROLKEY (GetAsyncKeyState(VK_CONTROL)<0)
	if (!lParam || CONTROLKEY)
		return S_OK;

	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	lpMMI->ptMinTrackSize.x =  DEFAULT_BROWSE_WIDTH;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Store the window location in the registry
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		WINDOWPLACEMENT placement;
		placement.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(&placement);
		regkey.SetBinaryValue(REGVAL_LOCATION, &placement.rcNormalPosition, sizeof(RECT));
	}

	DestroyWindow();
	m_hWnd = NULL;
	m_pDispatch = NULL;
	m_szURL = "";
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::Close()
{
	if (m_hWnd && IsWindow())
		SendMessage(WM_CLOSE);
}
/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::UpdateSize(WPARAM wParam, LPARAM lParam)
{
	if (IsWindow())
	{
		// wParam values: SIZE_MAXIMIZED, SIZE_MINIMIZED, SIZE_RESTORED
		if (wParam == SIZE_MINIMIZED)
			ShowWindow(SW_MINIMIZE);
		else
			ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);
	}
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialog::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = false;

	WINDOWPOS* pWindowPos = (WINDOWPOS*)lParam;
	if (!pWindowPos)
		return S_OK;

//j	if (GetLastActivePopup() != m_hWnd)
//j		pWindowPos->flags |= SWP_NOZORDER; // Always retain the Z-order
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::SetDispatch(IDispatch* pDispatch)
{
	m_pDispatch = pDispatch;
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::SetURL(LPCSTR pstrURL, bool bNavigate)
{
	// Set the SyncMode BEFORE we navigate to the page
	if (m_pAddin->GetSmartCache().IsUseSmartCache())
		m_pAddin->GetSmartCache().SetSyncMode();

	m_szURL = pstrURL;

	if (bNavigate && m_pBrowser != NULL)
	{
		CComVariant varFlags = navNoHistory;
		CComVariant varTargetFrameName = "_self";
		HRESULT hr = m_pBrowser->Navigate(CComBSTR(pstrURL), &varFlags, &varTargetFrameName, NULL/*&varPostData*/, NULL/*&varHeaders*/);
	}
}

/////////////////////////////////////////////////////////////////////////////
CString CHTMLDialog::GetURL()
{
	if (m_pBrowser == NULL)
		return m_szURL;

	CComBSTR bstrURL;
	HRESULT hr = m_pBrowser->get_LocationURL(&bstrURL);
	return CString(bstrURL);
}
/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::BeforeNavigate2(IDispatch *pDisp, VARIANT *url, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *Cancel)
{
	m_bWaiting = true;
	m_bBrowserError = false;
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::NavigateComplete2(IDispatch *pDisp, VARIANT *URL)
{
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::DocumentComplete(IDispatch *pDisp, VARIANT *URL)
{
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}
/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::DocumentReallyComplete(IDispatch *pDisp, VARIANT *URL)
{
	if (!m_bBrowserError)
		m_szCurrentContentType = ExtractContentTypeFromUrl(CString(URL->bstrVal));
	
	m_bWaiting = false;
	m_bBrowserError = false;
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}
/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::NavigateError(IDispatch *pDisp, VARIANT *URL, VARIANT *TargetFrameName, VARIANT *StatusCode, VARIANT_BOOL *Cancel)
{
	CComVariant varStatusCode(*StatusCode);
	DWORD  dwStatusCode =  varStatusCode.lVal;
	if (!m_bBrowserError && dwStatusCode >= HTTP_STATUS_BAD_REQUEST)
	{
		*Cancel = VARIANT_TRUE; // CANCEL default error page
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
		ShowPage(m_pBrowser, PAGE_UNAVAILABLE);
		m_bBrowserError = true;	
		m_bWaiting = false;
		

		// Add error status code to title. Only first error is added.
		CString szTitle;
		GetWindowText(szTitle);
		if (szTitle.Find("error") < 0)
		{
			szTitle += String(" (error: %X)", dwStatusCode);
			SetWindowText(szTitle);
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::ProgressChange(long Progress, long ProgressMax)
{
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::DownloadBegin()
{
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CHTMLDialog::DownloadComplete()
{
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}