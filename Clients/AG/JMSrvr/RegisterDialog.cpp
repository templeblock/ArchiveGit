#include "stdafx.h"
#include "RegisterDialog.h"
#include "Email.h"
#include "HelperFunctions.h"
#include "HTTPClient.h"
#include "Internet.h"
#include "SecurityMgr.h"

static UINT IDT_PROMPT_TIMER = ::RegisterWindowMessage("Creata Mail Register Prompt Timer");
static UINT IDT_AUTH_TIMER = ::RegisterWindowMessage("Creata Mail Authenticate Timer");

static const int DEFAULT_WIDTH	= 500;
static const int DEFAULT_HEIGHT = 425;

/////////////////////////////////////////////////////////////////////////////
CRegisterDialog::CRegisterDialog()
{
	m_pAddIn = NULL;
	m_pBrowser = NULL;
	m_bShowOnInit = false;
	m_bRegistrationInProgress = false;
	m_bWaiting = false;
	m_bBrowserError = false;
	m_dwResultStatus = 0;
	m_dwMode = -1;
	SetBaseURL (DEFAULT_HOST);
}

/////////////////////////////////////////////////////////////////////////////
CRegisterDialog::~CRegisterDialog()
{
}

/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::IsRegistrationInProgress()
{
	return m_bRegistrationInProgress;
}

/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::Initialize(HWND hWndParent, IDispatch* pDispatch, bool bShow)
{
	if (::IsWindow(m_hWnd))
		return false;

	if (!pDispatch)
		return false;

	if (!hWndParent)
		hWndParent = ::GetDesktopWindow();

	m_pAddIn = pDispatch;
	m_bShowOnInit = bShow;
	Create(hWndParent);
	HICON hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(),	(LPCTSTR)MAKEINTRESOURCE(IDI_LOGO), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED);
	SetIcon(hIcon, true/*bBigIcon*/);
	hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(),	(LPCTSTR)MAKEINTRESOURCE(IDI_LOGO), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED);
	SetIcon(hIcon, false/*bBigIcon*/);

	// Check registry for latest Host url and
	// Browse, Usage and Upgrade pages. If values
	// are empty in registry the hard coded defaults
	// are used.
	CString szBaseURL;
	m_Authenticate.ReadHost(szBaseURL);
	SetBaseURL(szBaseURL);
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::Load(LPCTSTR pstrURL)
{
	if (!::IsWindow(m_hWnd))
		return;

	CString strURL = GetURL();
	if (strURL != pstrURL)
		SetURL(pstrURL, TRUE/*bNavigate*/);
}

/////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::SetSize(int iHeight, int iWidth)
{
	SetWindowPos(NULL, 0, 0, iWidth, iHeight, SWP_NOMOVE|SWP_NOZORDER);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CRegisterDialog::Show(BOOL bShow, int iWidth, int iHeight)
{
	// Creates window if it does not exist.
	Initialize(NULL/*hWndParent*/, m_pAddIn/*pDispatch*/, false/*bShow*/);
	
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
	}
	
	return ShowWindow(nCmd);
}

/////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::ShowHelp()
{
	CString szHelp;
	m_Authenticate.ReadHelpPage(szHelp);
	CString szUrl;
	m_Authenticate.ReadHost(szUrl);
	szUrl += szHelp;
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}

/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::ShowRegisterPrompt()
{
	if (!::IsWindow(m_hWnd))
		return false;

	if (m_pBrowser == NULL)
		return false;

	// Display register prompt.
	CString szMsg;
	CString szCaption;
	szMsg.LoadString(IDS_REG_PROMPT);
	szCaption.LoadString(IDS_REG_TITLE);
	INT nResponse = MessageBox(szMsg, szCaption, MB_OKCANCEL | MB_ICONINFORMATION | MB_TOPMOST);
	if (nResponse == IDOK)
		Register(MODE_FIRSTRUN);

	return true;				
}

/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::Register(DWORD dwMode)
{
	bool bSuccess = true;
	m_dwMode = dwMode;

	// Initially hide window then clear web browser.
	Show(false);
	Load(_T("about:blank"));

	// Process Mode
	if (dwMode == MODE_REGPROMPT)
		bSuccess = ShowRegisterPrompt();
	else if (dwMode == MODE_FIRSTRUN)
		bSuccess = ShowRegisterPage();
	else if (dwMode == MODE_SIGNIN)
		bSuccess = ShowSignInPage();
	else if (dwMode == MODE_SILENT)
		bSuccess = StartAuthenticate();

	// If registration process is in progress then 
	// set flag. Reset after 30 seconds just in case
	// OnAuthenticate is not called by web server. This 
	// ensures proper behavior of silent authentication.
	if (bSuccess)
	{
		m_bRegistrationInProgress = true;
		SetTimer(IDT_AUTH_TIMER, 30000, NULL); // reset after 30 secs
	}

	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::ShowRegisterPage()
{
	if (!::IsWindow(m_hWnd) || m_pBrowser == NULL)
			return false;

	ShowPage(m_pBrowser, PAGE_WAIT);

	// Prepare to display authentication page.
	CString szAuthUrl; 
	if (!AuthPagePrep(szAuthUrl, true))
		return false;

	// Get a default email address
	/*CEmail Email;
	CString szEmailAddr = "";
	if (!Email.GetOutlookPopEmailAddr(szEmailAddr))
		if (!Email.GetExchangeServerEmailAddr(szEmailAddr))
			Email.GetExpressEmailAddr(szEmailAddr);*/
	
	// Get version #
	CString szVersion;
	m_Authenticate.ReadVersion(szVersion);

	// Get Source Id
	CString szSourceId;
	m_Authenticate.ReadSource(szSourceId);

	// Check if logging is requested for server.
	DWORD dwLoggingOpt=0;
	m_Authenticate.ReadServerLoggingOption(dwLoggingOpt);

	// Post authentication request.
	CHTTPClient Http(m_pBrowser);
	Http.InitilizePostArguments();
	Http.AddPostArguments(TAG_MODE, MODE_FIRSTRUN); 

	if (!szSourceId.IsEmpty())
		Http.AddPostArguments(TAG_SOURCE, szSourceId); 

	//Http.AddPostArguments(TAG_USERID, szEmailAddr);
	Http.AddPostArguments(TAG_VERSION, szVersion);
	Http.AddPostArguments(TAG_DEBUG, dwLoggingOpt);
	if (!Http.Request(szAuthUrl, CHTTPClient::RequestPostMethod))
		return false;


	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::ShowSignInPage()
{
	if (!::IsWindow(m_hWnd) || m_pBrowser == NULL)
			return false;
	
	ShowPage(m_pBrowser, PAGE_WAIT);

	// Prepare to display authentication page.
	CString szAuthUrl; 
	if (!AuthPagePrep(szAuthUrl, true))
		return false;

	// Get User id from registry
	CString szEmailAddr;
	m_Authenticate.ReadUserID(szEmailAddr);
	
	// Get version #
	CString szVersion;
	m_Authenticate.ReadVersion(szVersion);

	// Check if logging is requested for server.
	DWORD dwLoggingOpt=0;
	m_Authenticate.ReadServerLoggingOption(dwLoggingOpt);

	
	// Post authentication request.
	CHTTPClient Http(m_pBrowser);
	Http.InitilizePostArguments();
	Http.AddPostArguments(TAG_MODE, MODE_SIGNIN); 
	Http.AddPostArguments(TAG_USERID, szEmailAddr);
	Http.AddPostArguments(TAG_VERSION, szVersion);
	Http.AddPostArguments(TAG_DEBUG, dwLoggingOpt);
	if (!Http.Request(szAuthUrl, CHTTPClient::RequestPostMethod))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::SetDispatch(IDispatch* pAddIn)
{
	m_pAddIn = pAddIn;
}

/////////////////////////////////////////////////////////////////////////////////////////////
LPCTSTR CRegisterDialog::GetBaseURL()
{
	SetBaseURL(m_Authenticate.GetBaseUrl());
	return m_szBaseURL;
}

/////////////////////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::SetBaseURL(LPCTSTR szUrl)
{
	m_szBaseURL = szUrl;
	
	// if Dev host then we need to use HTTP. 
	m_szBaseURL.MakeLower();
	if (m_szBaseURL.Find("http://dev") >= 0)
		return;

	// If Host is not Dev then use HTTPS.
	m_szBaseURL.MakeLower();
	if (m_szBaseURL.Find("https") == -1)
	{
		int iCount = Replace(m_szBaseURL, "http", "https");
		ATLASSERT(iCount);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::SecureSocket()
{
	// If using secure socket we're ok.
	if (m_szBaseURL.Find("https") >= 0)
		return true;

	// Otherwise, ensure internet security permits data submission to url.
	CSecurityMgr Security;
	if (!Security.IsUrlActionAllowed(NULL, URLACTION_HTML_SUBMIT_FORMS, DEFAULT_HOSTW))
	{
		DisplayMessage(IDS_SECURITY_SUBMIT_FAIL);
		return false;
	}
		
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::DisplayMessage(DWORD dwMsgId)
{
	CString szMsg;
	szMsg.LoadString(dwMsgId);
	MessageBox(szMsg, g_szAppName, MB_ICONWARNING | MB_OK | MB_HELP | MB_TOPMOST);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRegisterDialog::SetURL(LPCTSTR pstrURL, BOOL bNavigate)
{
	m_strURL = pstrURL;

	if (bNavigate && m_pBrowser != NULL)
	{
		CComVariant varFlags = navNoHistory |  navNoReadFromCache | navNoWriteToCache;
		CComVariant varTargetFrameName = "_self";
		HRESULT hr = m_pBrowser->Navigate(CComBSTR(pstrURL), 
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
CString CRegisterDialog::GetURL()
{
	if (m_pBrowser == NULL)
		return m_strURL;

	CComBSTR bstrURL;
	HRESULT hr = m_pBrowser->get_LocationURL(&bstrURL);
	return CString(bstrURL);
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::AuthPagePrep(CString &szAuthUrl, bool bShow)
{
	// Construct auth url
	szAuthUrl = GetBaseURL();
	szAuthUrl += AUTHENTICATE_PAGE;
	
	Show(bShow);

	if (bShow)
	{
		BringWindowToTop();
		FlashWindow(TRUE);
	
		// Check secure socket to ensure form 
		// data can be submitted
		if (!SecureSocket())
			return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::ProcessOptions(LPCTSTR lpszOptions)
{
	CString szOptions = lpszOptions;

	CString szLineListDate = GetOptionStringValue(szOptions, _T("LineListDate"));
	if (!szLineListDate.IsEmpty())
		m_Authenticate.WriteLineListDate(szLineListDate);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::IsRegistrationdNeeded()
{
	DWORD dwFail = 0;
	return !m_Authenticate.IsRegistered(dwFail);
}

/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::IsAuthenticateNeeded()
{
	return m_Authenticate.IsExpired();
}
/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::StartAuthenticate()
{	
	if (m_bRegistrationInProgress)
		return false;


	// Initially, set member status to free user to handle cases
	// where authentication fails or is interrupted.
	//m_Authenticate.WriteMemberStatus(USER_STATUS_AFU);

	// Need to make sure our browser window is still around.
	// If its not around then it is created in a hidden state.
	// Otherwise, it is just hidden.
	Show(false);	
	
	// Abort if not online;
	CInternet Internet;
	if (!Internet.IsOnline(true/*bForceLANOnline*/))
		return false;
	
	// Prepare to send to authentication page.
	// Page is hidden.
	CString szAuthUrl; 
	if (!AuthPagePrep(szAuthUrl, false))
		return false;


	// Get auth info from registry.
	CString szUserID;
	DWORD dwCustNum;
	CString szVersion;
	m_Authenticate.ReadUserID(szUserID);
	m_Authenticate.ReadMemberNum(dwCustNum);
	m_Authenticate.ReadVersion(szVersion);

	// Post authentication request.
	CHTTPClient Http(m_pBrowser);
	Http.InitilizePostArguments();
	Http.AddPostArguments(TAG_MODE, MODE_SILENT); 
	Http.AddPostArguments(TAG_USERID, szUserID);
	Http.AddPostArguments(TAG_CUSTNUM, dwCustNum);
	Http.AddPostArguments(TAG_VERSION, szVersion);
	if (!Http.Request(szAuthUrl, CHTTPClient::RequestPostMethod))
		return false;

	// see the results
	bool bShowBrowser = (g_pLog ? true : false);
//#if defined _DEBUG
//	bShowBrowser = true;
//#endif

	if (bShowBrowser)
	{
		ShowWindow(SW_SHOW);
		m_pBrowser->put_Visible(true);
	}
	else
		m_pBrowser->put_Visible(false);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CRegisterDialog::OnAuthenticate(DWORD dwCustomerNum,
									 LPCTSTR szUserId, 
									 LPCTSTR szHost, 
									 LPCTSTR szUpgrade, 
									 LPCTSTR szUsage,
									 LPCTSTR szHelp,
									 LPCTSTR szBrowse,
									 LPCTSTR szOptions,
									 DWORD dwUserStatus,
									 BOOL bShow,
									 DWORD dwResultStatus)
{
	m_bRegistrationInProgress = false;

	// Server determines when to show.
	if (::IsWindow(m_hWnd))
		ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	
	m_dwResultStatus = dwResultStatus;

	// If authentication fails ensure member status is set to FREE.
	// Note: (1) for pay user server will prompt with sign-in and explanation.
	// For example, email address (UserId) does not match. (2) In addition to 
	// member status only the result status is written to registry, this will
	// help web developers diagnose problems. The other values are not written
	// to registry because there are no garauntees that the data is valid during
	// a failed authentication and we would like the user to still be functional
	// at least as a free user.
	if (HIWORD(m_dwResultStatus) != AUTH_FAILURE_NONE)
	{
		m_Authenticate.WriteMemberStatus(USER_STATUS_AFU);
		m_Authenticate.WriteAuthStatus(dwResultStatus);
		return false;
	}

	// Update base url.
	CString szTemp = szHost;
	if (!szTemp.IsEmpty())
		SetBaseURL(szHost); 


	// Save authenticate info in registry.
	m_Authenticate.WriteMemberNum(dwCustomerNum);
	m_Authenticate.WriteUserID(szUserId);
	m_Authenticate.WriteHost(szHost);
	m_Authenticate.WriteUpgradePage(szUpgrade);
	m_Authenticate.WriteUsagePage(szUsage);
	m_Authenticate.WriteBrowsePage(szBrowse);
	m_Authenticate.WriteHelpPage(szHelp);
	m_Authenticate.WriteMemberStatus(dwUserStatus);
	m_Authenticate.WriteAuthStatus(dwResultStatus);
	m_Authenticate.WriteTimeStamp();

	ProcessOptions(szOptions);

	// Verify authenticaton info saved to registry.
	// If verification fails must be techinical problem, 
	// alert user.
	DWORD dwFailure = 0;
	if (m_Authenticate.IsRegistered(dwFailure))
		return true;

	CString szMsg;
	szMsg.Format("There was a problem authenticating your account.\n\nClick 'OK' below to contact customer support.");
	int iReturn = ::MessageBox(NULL, szMsg, g_szAppName, MB_ICONERROR | MB_OKCANCEL | MB_TOPMOST);
	if (iReturn == IDCANCEL)
		return false;

	CString szUrl;
	if (m_Authenticate.ReadHost(szUrl))
	{
		szUrl += DEFAULT_FEEDBACK;
		HINSTANCE hInstance = ::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CRegisterDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

	if (m_pAddIn)
		hr = m_BrowserWnd.SetExternalDispatch(m_pAddIn);
	ATLASSERT(SUCCEEDED(hr));

	// Allow dialog boxes such as security alerts.
	if (m_pBrowser)
	{
		m_pBrowser->put_Silent(VARIANT_FALSE);
		Connect(m_pBrowser); // Connect DWebBrowserEvents
	}

	// Allow time for dialog to initialize.
	if (SUCCEEDED(hr) && m_bShowOnInit)
	{
		SetTimer(IDT_PROMPT_TIMER, 200, NULL);
		m_bShowOnInit = false;
	}

	return TRUE;  // Let the system set the focus
}

/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CRegisterDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (::IsWindow(m_hWnd))
		ShowWindow(SW_HIDE);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CRegisterDialog::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (IDT_PROMPT_TIMER == wParam) 
	{
		KillTimer(IDT_PROMPT_TIMER);
		Register(MODE_REGPROMPT);
	}
	// This timer handles cases where OnAuthenticate
	// is never (after 30 seconds) called. As precaution
	// the in progress flag is reset. Mainly applicable
	// to silent authentication.
	else if (IDT_AUTH_TIMER == wParam) 
	{
		KillTimer(IDT_AUTH_TIMER);
		m_bRegistrationInProgress = false;
		m_dwMode = -1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CRegisterDialog::OnHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ShowHelp();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::NavigateError(IDispatch *pDisp, VARIANT *URL, VARIANT *TargetFrameName, VARIANT *StatusCode, VARIANT_BOOL *Cancel)
{
	CComVariant varStatusCode(*StatusCode);
	DWORD  dwStatusCode =  varStatusCode.lVal;
	if (dwStatusCode >= HTTP_STATUS_BAD_REQUEST)
	{
		*Cancel = VARIANT_TRUE; // CANCEL default error page
		ShowPage(m_pBrowser, PAGE_UNAVAILABLE);
		m_bBrowserError = true;
		m_bWaiting = false;
	}

}
/////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::BeforeNavigate2(IDispatch *pDisp, VARIANT *url, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *Cancel)
{
	m_bWaiting = true;
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::NavigateComplete2(IDispatch *pDisp, VARIANT *URL)
{
	if (m_bWaiting)
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::DocumentComplete(IDispatch *pDisp, VARIANT *URL)
{
	if (m_bWaiting)
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::DocumentReallyComplete(IDispatch *pDisp, VARIANT *URL)
{
	m_bWaiting = false;
	m_bBrowserError = false;
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}
/////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::ProgressChange(long Progress, long ProgressMax)
{
	if (m_bWaiting)
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}
/////////////////////////////////////////////////////////////////////////////
void CRegisterDialog::DownloadBegin()
{
	if (m_bWaiting)
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
}