#include "stdafx.h"
#include "resource.h"
#include "AddressBookDialog.h"
#include "RegKeys.h"
#include <wininet.h>

static const int DEFAULT_WIDTH	= 500;
static const int DEFAULT_HEIGHT = 425;
static LPCTSTR DEFAULT_HOST = _T("http://www.bluemountain.com");

/////////////////////////////////////////////////////////////////////////////
CAddressBookDialog::CAddressBookDialog()
{
	m_pDisp = NULL;
	m_pBrowser = NULL;
	m_bShowOnInit = false;
	m_bWaiting = false;
	m_bBrowserError = false;
	m_dwResultStatus = 0;
	m_dwMode = -1;
	SetURL(DEFAULT_HOST);
}

/////////////////////////////////////////////////////////////////////////////
CAddressBookDialog::~CAddressBookDialog()
{
}

/////////////////////////////////////////////////////////////////////////////
bool CAddressBookDialog::Initialize(HWND hWndParent, IDispatch* pDispatch, bool bShow)
{
	if (::IsWindow(m_hWnd))
		return false;

	if (!pDispatch)
		return false;

	if (!hWndParent)
		hWndParent = ::GetDesktopWindow();

	m_pDisp = pDispatch;
	m_bShowOnInit = bShow;
	Create(hWndParent);
	//HICON hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(),	(LPCTSTR)MAKEINTRESOURCE(IDI_LOGO), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED);
	//SetIcon(hIcon, true/*bBigIcon*/);
	//hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(),	(LPCTSTR)MAKEINTRESOURCE(IDI_LOGO), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED);
	//SetIcon(hIcon, false/*bBigIcon*/);

	// Check registry for latest Host url and
	// Browse, Usage and Upgrade pages. If values
	// are empty in registry the hard coded defaults
	// are used.
	CString szBaseURL;
	ReadRegistryString(REGVAL_ADDRESSBOOK_URL, szBaseURL);
	SetURL(szBaseURL);
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CAddressBookDialog::Load(LPCTSTR lpszURL)
{
	if (!::IsWindow(m_hWnd))
		return;

	CString szURL = GetURL();
	if (szURL != lpszURL)
		SetURL(lpszURL, TRUE/*bNavigate*/);
}

/////////////////////////////////////////////////////////////////////////////
void CAddressBookDialog::SetSize(int iHeight, int iWidth)
{
	SetWindowPos(NULL, 0, 0, iWidth, iHeight, SWP_NOMOVE|SWP_NOZORDER);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAddressBookDialog::Show(HWND hWndParent, BOOL bShow, int iWidth, int iHeight, LPCTSTR szURL)
{
	// Creates window if it does not exist.
	Initialize(hWndParent/*hWndParent*/, m_pDisp/*pDispatch*/, false/*bShow*/);
	
	if (!::IsWindow(m_hWnd))
		return false;

	int nCmd = SW_HIDE;

	if (bShow)
	{
		nCmd = SW_SHOW;
		int nWidth = iWidth;
		int nHeight = iHeight;
		if (!nWidth)
			nWidth = DEFAULT_WIDTH;
		if (!nHeight)
			nHeight = DEFAULT_HEIGHT;

		SetSize(nHeight, nWidth);

		if (!CString(szURL).IsEmpty())
			Load(szURL);
	}
	
	return ShowWindow(nCmd);
}

/////////////////////////////////////////////////////////////////////////////////////////////
void CAddressBookDialog::SetDispatch(IDispatch* pDisp)
{
	m_pDisp = pDisp;
}


/////////////////////////////////////////////////////////////////////////////////////////////
bool CAddressBookDialog::DisplayMessage(DWORD dwMsgId)
{
	CString szMsg;
	szMsg.LoadString(dwMsgId);
	MessageBox(szMsg, g_szAppName, MB_ICONWARNING | MB_OK | MB_HELP | MB_TOPMOST);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
BOOL CAddressBookDialog::SetURL(LPCTSTR lpszURL, bool bNavigate)
{
	m_szURL = lpszURL;

	if (bNavigate && m_pBrowser != NULL)
	{
		CComVariant varFlags = navNoHistory |  navNoReadFromCache | navNoWriteToCache;
		CComVariant varTargetFrameName = "_self";
		HRESULT hr = m_pBrowser->Navigate(CComBSTR(lpszURL), 
									&varFlags, 
									&varTargetFrameName, 
									NULL/*&varPostData*/, 
									NULL/*&varHeaders*/);
		if (FAILED(hr))
			return FALSE;
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
CString CAddressBookDialog::GetURL()
{
	if (m_pBrowser == NULL)
		return m_szURL;

	CComBSTR bstrURL;
	HRESULT hr = m_pBrowser->get_LocationURL(&bstrURL);
	return CString(bstrURL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool CAddressBookDialog::ShowPage(IWebBrowser2* pWB, UINT nId)
{
	HRESULT hr = S_OK;
	CComPtr<IWebBrowser2> spWB(pWB);
	if (!spWB)
		return false;

	HINSTANCE hInst = _AtlBaseModule.GetResourceInstance();
	TCHAR szModuleFileName[MAX_PATH];
	if (!GetModuleFileName(hInst, szModuleFileName, MAX_PATH))
		return false;
	
	//Add the IE Res protocol
	CString szResourceURL;
	szResourceURL.Format(_T("res://%s/%s"), szModuleFileName, _T("Unavailable.htm"));
		
	CComVariant varFlags = navNoHistory;
	CComVariant varTargetFrameName = "_self";
	hr = pWB->Navigate(CComBSTR(szResourceURL), &varFlags, &varTargetFrameName, NULL/*&varPostData*/, NULL/*&varHeaders*/);
	if (FAILED(hr))
		return false;
	

	return true;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAddressBookDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DlgResize_Init(false/*bAddGripper*/, true/*bUseMinTrackSize*/, WS_CLIPCHILDREN/*dwForceStyle*/);
	
	// Create the web browser control
	m_BrowserWnd = GetDlgItem(IDC_EXPLORER1);
	if (!::IsWindow(m_BrowserWnd.m_hWnd))
		return TRUE;

	HRESULT hr = m_BrowserWnd.QueryControl(&m_pBrowser);
	ATLASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return TRUE;

	if (m_pDisp)
		hr = m_BrowserWnd.SetExternalDispatch(m_pDisp);
	ATLASSERT(SUCCEEDED(hr));

	// Allow dialog boxes such as security alerts.
	if (m_pBrowser)
	{
		m_pBrowser->put_Silent(VARIANT_FALSE);
		Connect(m_pBrowser); // Connect DWebBrowserEvents
	}

	return TRUE;  // Let the system set the focus
}

/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CAddressBookDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pBrowser = NULL;
	if (::IsWindow(m_hWnd))
		DestroyWindow();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CAddressBookDialog::NavigateError(IDispatch *pDisp, VARIANT *URL, VARIANT *TargetFrameName, VARIANT *StatusCode, VARIANT_BOOL *Cancel)
{
	CComVariant varStatusCode(*StatusCode);
	DWORD  dwStatusCode =  varStatusCode.lVal;
	if (dwStatusCode >= HTTP_STATUS_BAD_REQUEST)
	{
		//*Cancel = VARIANT_TRUE; // CANCEL default error page
		//ShowPage(m_pBrowser, IDR_UNAVAILABLE_HTML);
		m_bBrowserError = true;
		m_bWaiting = false;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAddressBookDialog::BeforeNavigate2(IDispatch *pDisp, VARIANT *url, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *Cancel)
{
	m_bWaiting = true;
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CAddressBookDialog::NavigateComplete2(IDispatch *pDisp, VARIANT *URL)
{
	if (m_bWaiting)
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CAddressBookDialog::DocumentComplete(IDispatch *pDisp, VARIANT *URL)
{
	if (m_bWaiting)
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CAddressBookDialog::DocumentReallyComplete(IDispatch *pDisp, VARIANT *URL)
{
	m_bWaiting = false;
	m_bBrowserError = false;
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}
/////////////////////////////////////////////////////////////////////////////
void CAddressBookDialog::ProgressChange(long Progress, long ProgressMax)
{
	if (m_bWaiting)
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CAddressBookDialog::DownloadBegin()
{
	if (m_bWaiting)
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}