// JMExplorerBand.cpp : Implementation of CJMExplorerBand

#include "stdafx.h"
#include "JMExplorerBand.h"
#include "HelperFunctions.h"
#include "FunctionObject.h"
#include "CM_Constants.h"
#include "JMToolband.h"
#include "hotmail.h"
#include "yahoomail.h"
#include "messagebox.h"
#include <Exdispid.h>

const WCHAR TITLE_CJMExplorerBand[] = L"Creata Mail";

const DWORD CLIENT_ID_HOTMAIL		= 0x0001;
const DWORD CLIENT_ID_YAHOOMAIL		= 0x0002;

static UINT IDT_NEWDOC_TIMER	= ::RegisterWindowMessage("Creata Mail IE New Doc Timer");

// CJMExplorerBand

//=============================================================================
//	Constructors/Destructors
//=============================================================================
CJMExplorerBand::CJMExplorerBand()
{
	SetClient(CLIENT_TYPE_IE);
	m_spSite			= NULL;
    m_hWnd				= NULL;
    m_hwndParent		= NULL;
	m_pDispatch			= NULL;
	m_spWebBrowser		= NULL;
	m_spJMBrowser		= NULL;
	m_pMailClient		= NULL;
	m_lpDocCompleteDisp = NULL;

    m_bFocus		= false;
	m_bShow			= false;

	m_dwCookie		= 0;
    m_dwViewMode	= 0;
    m_dwBandID		= 0;
	m_ClientID		= 0;
}
CJMExplorerBand::~CJMExplorerBand()
{
	ClientCleanup();
	m_spSite			= NULL;
	m_pDispatch			= NULL;
	m_spWebBrowser		= NULL;
	m_spJMBrowser		= NULL;
	m_pMailClient		= NULL;


}
//=============================================================================
//	UtilityCallback
//=============================================================================
void CALLBACK CJMExplorerBand::UtilityCallback(LPARAM lParam1, LPARAM lParam2)
{
	CJMExplorerBand* pExplorerBand = (CJMExplorerBand*)lParam1; 
	if (!pExplorerBand)
		return;

	pExplorerBand->OnShutdownAll();
}
//=============================================================================
//	ClientCleanup
//=============================================================================
void CJMExplorerBand::ClientCleanup()
{
	if (!m_pMailClient)
		return;

	// Undo potential send event operations. For example, this will clear
	// usage send count. This ensures that a compose message that has Creata Mail
	// content does not erroneously include usage send count when IE is closed.
	// NOTE: if a message was sent prior to IE being closed then the correct usage
	// send count would already have been captured.
	m_pMailClient->UndoSendEvent();

	delete m_pMailClient;

	return;
}
//=============================================================================
//	SetURL
//=============================================================================
void CJMExplorerBand::SetURL(LPCSTR pstrURL, bool bNavigate)
{
	m_strURL = pstrURL;

	if (bNavigate && m_spJMBrowser != NULL)
	{
		CComVariant varFlags = navNoHistory;
		CComVariant varTargetFrameName = "_self";
		HRESULT hr = m_spJMBrowser->Navigate(CComBSTR(pstrURL), &varFlags, &varTargetFrameName, NULL/*&varPostData*/, NULL/*&varHeaders*/);
	}
}
//=============================================================================
//	GetURL
//=============================================================================
CString CJMExplorerBand::GetURL()
{
	if (m_spJMBrowser == NULL)
		return m_strURL;

	CComBSTR bstrURL;
	HRESULT hr = m_spJMBrowser->get_LocationURL(&bstrURL);
	return CString(bstrURL);
}
//=============================================================================
//	SyncMailClient
//=============================================================================
// Both CJMExplorerBand and the web mail client are derived from CMSAddinBase. 
// However, Only CJMExplorerband is capable of handling downloads since it is 
// the COM object. The mail clients manage the other tasks such as writing html, 
// usage and etc thru the underlying CMSAddinBase base class. Some day it will 
// make sense to break CMSAddinBase into separate distinct components that 
// encapsulate specific behavior such as download. But for now we need to pass 
// the mail clients any info resulting from download - we do that here.
//
bool CJMExplorerBand::SyncMailClient()
{
	if (!IsMailClientValid())
		return false;

	m_pMailClient->SetLastCommand(m_wLastCommand);
	m_pMailClient->SetDownloadParms(m_DownloadParms);
	m_pMailClient->GetPreviewDialog().SetAddin(this);		// CJMExplorerBand needs to handle download.

	return true;
}
//=============================================================================
//	LaunchMail
//=============================================================================
bool CJMExplorerBand::LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject)
{
	CFuncLog log(g_pLog, "CJMExplorerBand::LaunchMail()");
	if (!m_pMailClient)
		HandleNewDoc(m_spWebBrowser);
	
	if (!SyncMailClient())
		return false;

	return m_pMailClient->LaunchMail(bAllowReuse, strDownloadSrc, strDownloadDst, strSubject);
}
//=============================================================================
//	LaunchFeedbackMail
//=============================================================================
bool CJMExplorerBand::LaunchFeedbackMail(DWORD dwCmd)
{
	if (!m_pMailClient)
		return false;

	return  m_pMailClient->LaunchFeedbackMail(dwCmd);
}
//=============================================================================
//	EmbedHTMLAtEnd
//=============================================================================
bool CJMExplorerBand::EmbedHTMLAtEnd(const CString& strHTMLText)
{
	if (!m_pMailClient)
		HandleNewDoc(m_spWebBrowser);
	
	if (!SyncMailClient())
		return false;

	return m_pMailClient->EmbedHTMLAtEnd(strHTMLText);
}

//=============================================================================
//	EmbedHTMLAtSelection
//=============================================================================
bool CJMExplorerBand::EmbedHTMLAtSelection(const CString& strHTMLText)
{
	CFuncLog log(g_pLog, "CJMExplorerBand::EmbedHTMLAtSelection()");
	if (!m_pMailClient)
		HandleNewDoc(m_spWebBrowser);
	
	if (!SyncMailClient())
		return false;

	return m_pMailClient->EmbedHTMLAtSelection(strHTMLText);
}
//=============================================================================
//	DownloadComplete
//=============================================================================
bool CJMExplorerBand::DownloadComplete()
{
	CFuncLog log(g_pLog, "CJMExplorerBand::DownloadComplete()");

	if (!SyncMailClient())
		return false;

	return m_pMailClient->DownloadComplete();
}
//=============================================================================
//	DisplayMessage
//=============================================================================
int CJMExplorerBand::DisplayMessage(DWORD dwMsgID, UINT uType)
{
	CString szMsg;
	szMsg.LoadString(dwMsgID);
	
	CMessagebox Msg;
	return Msg.ShowMessagebox(m_hWnd, szMsg, g_szAppName, uType);
}
//=============================================================================
//   CJMExplorerBand::ShowHelp() 
//=============================================================================
void CJMExplorerBand::ShowHelp(DWORD dwHelpID)
{
	CFuncLog log(g_pLog, "CJMExplorerBand::ShowHelp()");

	if (!m_spJMBrowser)
		return;

	CString szHelp;
	if (!dwHelpID)
		m_Authenticate.ReadHelpPage(szHelp);
	else
		szHelp.LoadString(dwHelpID);
	
	CString szUrl;
	m_Authenticate.ReadHost(szUrl);
	szUrl += szHelp;

	CComVariant varFlags = navOpenInNewWindow;
	CComVariant varTargetFrameName = "_BLANK";
	HRESULT hr = m_spJMBrowser->Navigate(CComBSTR(szUrl), &varFlags, &varTargetFrameName, NULL/*&varPostData*/, NULL/*&varHeaders*/);
	//DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}

//=============================================================================
//   CJMExplorerBand::RegisterAndCreateWindow() 
//=============================================================================

BOOL CJMExplorerBand::RegisterAndCreateWindow()
{  
	// If window already exists we're done;
    if(m_hWnd)
		return FALSE;

	//Can't create a child window without a parent.
    if(!m_hwndParent)
        return FALSE;
 
    //If the window class has not been registered, then do so.
    WNDCLASS wc;
    if(!GetClassInfo(g_hInst, JMEB_CLASS_NAME, &wc))
    {
        ZeroMemory(&wc, sizeof(wc));
        wc.style          = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
        wc.lpfnWndProc    = (WNDPROC)WndProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = 0;
        wc.hInstance      = g_hInst;
        wc.hIcon          =  NULL;
        wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground  = (HBRUSH)CreateSolidBrush(RGB(0, 0, 192));
        wc.lpszMenuName   = NULL;//MAKEINTRESOURCE(IDR_OPTIONSMENU);
        wc.lpszClassName  = JMEB_CLASS_NAME;
      
		//If RegisterClass fails, CreateWindow below will fail.
        if(!RegisterClass(&wc))
		{
			return false;
        }
    }

    //Create the window. The WndProc will set m_hWnd.
	RECT  rc;
	::GetClientRect(m_hwndParent, &rc);
    CreateWindowEx(0,
                   JMEB_CLASS_NAME,
                   NULL,
                   WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER,
                   rc.left,
                   rc.top,
                   rc.right - rc.left,
                   rc.bottom - rc.top,
                   m_hwndParent,
                   NULL,
                   g_hInst,
                   (LPVOID)this);
        
 
    
    return (NULL != m_hWnd);
}
//=============================================================================
//   CJMExplorerBand::LoadPage()
//=============================================================================
bool CJMExplorerBand::LoadPage()
{
	DWORD dwIECat = DEFAULT_CAT;
	m_Authenticate.ReadDWORDStatus(REGVAL_DEFAULT_IECAT, dwIECat);
	if (dwIECat > LAST_CAT_USED)
		dwIECat = LAST_CAT_USED;

	if (dwIECat == LAST_CAT_USED)
		m_Authenticate.ReadDWORDStatus(REGVAL_LAST_IE_CAT, dwIECat);

	if (dwIECat >= LAST_CAT_USED)
		dwIECat = DEFAULT_CAT;

	Popup(NULL, 0, NULL, ITEM_INFO[dwIECat].szQuery);

	return true;
}
//=============================================================================
//   CJMExplorerBand::Popup()
//=============================================================================
bool CJMExplorerBand::Popup(int iType, int iButtonIndex, HWND hwndRebar, LPCTSTR pQueryString)
{
	CFuncLog log(g_pLog, "CJMExplorerBand::Popup()");


	CString strURL = m_Authenticate.GetBaseUrl();
	strURL += m_Authenticate.GetBrowsePage();
	strURL += pQueryString;
	strURL += "&";
	CString szPair;
	m_Authenticate.GetMemberStatusQueryString(szPair);
	strURL += szPair;

	// Add pagination string
	m_Authenticate.GetPaginationQueryString(szPair);
	strURL += "&";
	strURL += szPair;

	// Determine if cached line-list needs to be refreshed.
	m_SmartCache.CheckLineListCache();

	if (m_hWnd && !::IsWindow(m_BrowserWnd.m_hWnd))
	{
		// Create the jazzymail browser control
		HRESULT hr = S_OK;
		RECT rc = {0, 0, DEFAULT_BROWSE_WIDTH , DEFAULT_BROWSE_HEIGHT};
		::GetClientRect(m_hwndParent, &rc);
		HWND hBrowser = m_BrowserWnd.Create(m_hWnd, rc, strURL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0/*WS_EX_CLIENTEDGE*/);
		m_spJMBrowser = NULL;
		hr = m_BrowserWnd.QueryControl(&m_spJMBrowser);
		if (FAILED(hr))
		{
			GetError(_T("Popup"));		
			return false;
		}

		

		// Set External dispatch for server to call back.
		CComQIPtr<IJMExplorerBand> pAddIn = this;
		CComQIPtr<IDispatch> pDispatch = pAddIn;
		hr = m_BrowserWnd.SetExternalDispatch(pDispatch);

		
		// Set download info.
		m_Download.SetAppInfo(this->m_spClientSite, CLIENT_TYPE_IE);
		
		SetURL(strURL, false/*bNavigate*/);
		m_AutoUpdate.Start(true/*bSilent*/);

	}
	else
	{
		SetURL(strURL, true/*bNavigate*/);
	}

	return true;
	
}
//=============================================================================
//   CJMExplorerBand::IsMailClientValid() 
//=============================================================================
bool CJMExplorerBand::IsMailClientValid(bool bShowMessage)
{
	if (!m_pMailClient)
	{
		if (bShowMessage)
			DisplayMessage(IDS_GENERAL_WARN, MB_OK  | MB_HELP /*| MB_TOPMOST*/);
		return false;
	}

	return true;
}
//=============================================================================
//   CJMExplorerBand::IsHotmailCompose() 
//=============================================================================
bool CJMExplorerBand::IsHotmailCompose(IHTMLDocument2* pDoc)
{
	HRESULT hr = S_OK;
	CComQIPtr<IHTMLDocument2> spDoc2(pDoc);
	if (!spDoc2)
		return false;

	if (spDoc2 == NULL)
		return false;

	CComPtr<IHTMLWindow2> spWnd;
	hr = spDoc2->get_parentWindow(&spWnd);
	if (FAILED(hr) || spWnd == NULL) 
		return false;

	IHTMLLocationPtr spLoc;
	hr = spWnd->get_location(&spLoc);
	if (FAILED(hr) || spLoc == NULL) 
		return false;

	CComBSTR bstrHRef;
	hr = spLoc->get_href(&bstrHRef);
	if (FAILED(hr)) 
		return false;

	CString szHRef(bstrHRef);
	szHRef.MakeLower();
	if (szHRef.Find(_T("hotmail")) < 0)
		return false;

	if (szHRef.Find(_T("compose")) < 0 && szHRef.Find(_T("premail")) < 0)
		return false;

	CComPtr<IHTMLElement> spElem;
	if (!GetElementByTagAndAttribute(spDoc2,_T("textarea"), _T("name"), _T("body"), spElem))
		return false;

	return true;
}
//=============================================================================
//   CJMExplorerBand::IsYahooCompose()
//=============================================================================
bool CJMExplorerBand::IsYahooCompose(IWebBrowser2 * pWB, IHTMLDocument2* pDoc, DWORD &dwVer)
{
	CComQIPtr<IHTMLDocument2> spDoc2(pDoc);
	if (!spDoc2)
		return false;

	if (spDoc2 == NULL)
		return false;


	CComPtr<IHTMLWindow2> spWnd;
	HRESULT hr = spDoc2->get_parentWindow(&spWnd);
	if (FAILED(hr) || spWnd == NULL) 
		return false;

	IHTMLLocationPtr spLoc;
	hr = spWnd->get_location(&spLoc);
	if (FAILED(hr) || spLoc == NULL) 
		return false;

	CComBSTR bstrHRef;
	hr = spLoc->get_href(&bstrHRef);
	if (FAILED(hr)) 
		return false;

	CString szHRef(bstrHRef);
	szHRef.MakeLower();
	if (szHRef.Find(_T("yahoo")) < 0)
		return false;

	if (szHRef.Find(_T("mail")) < 0)
		return false;


	if (GetKeyState(VK_CONTROL) < 0)
		return true;


	if (szHRef.Find(_T("compose")) < 0)
	{
		dwVer = YAHOO_VERSION_02;

		CWebMailBase * pWebMail = new CYahooMail(this);
		pWebMail->SetWebmailVersion(dwVer);
		pWebMail->SetWebbrowser(pWB);
		bool bComposeView = pWebMail->IsComposeView();
		delete pWebMail;
		if (!bComposeView)
			return false;
	
	}
	else
	{
		dwVer = YAHOO_VERSION_01;
	}

	
	return true;
}

//=============================================================================
//   CJMExplorerBand::HandleNewDoc() 
//=============================================================================
bool CJMExplorerBand::HandleNewDoc(IWebBrowser2 * pWB)
{
	CFuncLog log(g_pLog, "CJMExplorerBand::HandleNewDoc()");

	// Get doc associated with web browser.
	HRESULT hr;
	CComPtr<IWebBrowser2> spWB(pWB);
	if (!spWB)
		return false;

	CComPtr<IDispatch> spDocDisp;
	hr = spWB->get_Document(&spDocDisp);
	if (!spDocDisp)
		return false;

	CComQIPtr<IHTMLDocument2> spDoc2(spDocDisp);
	if (!spDoc2)
		return false;


	// Check if doc is hotmail or yahoo compose window.
	bool bNew = false;
	DWORD dwVer;
	// Hotmail compose?
	if (IsHotmailCompose(spDoc2))
	{
		m_bShow = true;
		if (m_ClientID != CLIENT_ID_HOTMAIL)
		{
			bNew = true;
			m_ClientID = CLIENT_ID_HOTMAIL;
			delete m_pMailClient;
			m_pMailClient = new CHotmail(this);
		}
	}
	// Yahoo mail compose?
	else if (IsYahooCompose(spWB, spDoc2, dwVer))
	{
		m_bShow = true;
		if (m_ClientID != CLIENT_ID_YAHOOMAIL)
		{
			bNew = true;
			m_ClientID = CLIENT_ID_YAHOOMAIL;
			delete m_pMailClient;
			m_pMailClient = new CYahooMail(this);
			m_pMailClient->SetWebmailVersion(dwVer);

		}
	}
	// If not compose window then delete mail client.
	else if (m_pMailClient)
	{
		// If this was NOT a send event then undo send actions - 
		// i.e. reset Usage Send Count to zero for each product.
		if (!m_pMailClient->IsSendEvent(spDoc2))
			m_pMailClient->UndoSendEvent();

		delete m_pMailClient;
		m_pMailClient = NULL;
		m_ClientID = 0;
	}
	
	// Update mail client.
	if (m_pMailClient)
	{
		m_pMailClient->SetWebbrowser(spWB);	
		if (bNew)
			m_pMailClient->OnNewMessage();	
	}


	// If commanded to show explorer bar and its hidden then show it.
	// Otherwise, if commanded to hide it and its visible then hide it.
	BOOL bWindowVisible = ::IsWindowVisible(m_hWnd);
	if (m_bShow && (!::IsWindow(m_hWnd) || !bWindowVisible))
	{
		CComVariant varCLSID = CComVariant(CLSID_JMEXPLORERBAND_STRING);
		CComVariant varShow = CComVariant(true);
		CComVariant varSize = VT_NULL;
		hr = spWB->ShowBrowserBar(&varCLSID, &varShow, &varSize);
	}
	else if (!m_bShow && bWindowVisible)
	{
		CComVariant varCLSID = CComVariant(CLSID_JMEXPLORERBAND_STRING);
		CComVariant varShow = CComVariant(false);
		CComVariant varSize = VT_NULL;
		hr = spWB->ShowBrowserBar(&varCLSID, &varShow, &varSize);
	}
	
	if (FAILED(hr))
			return false;


	return true;
}
//=============================================================================
//   CJMExplorerBand::HandleBeforeNavigate() 
//=============================================================================
bool CJMExplorerBand::HandleBeforeNavigate()
{
	// Treat as potential send event. If it turns out this is not a send event
	// adjustments are made in HandleNewDoc(). This is necessary because the only
	// way to detect a send event is after the message is sent but it would be to
	// late to capture data from document since it is gone. So for example, we get
	// usage Send Count now and if it turns out (in HandleNewDoc()) this was not 
	// a send event then we clear the usage Send Count.
	if (!m_pMailClient)
		return false;

	m_pMailClient->OnSendEvent();

	return true;
}
//////////////////////////////////////////////////////////////////////////////
bool CJMExplorerBand::HandleBrowserOptionsButton()
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleBrowserOptionsButton()");

	WORD wDisableFlags = OPTIONS_DISABLE_NONE;
	wDisableFlags |= (m_pMailClient ? OPTIONS_DISABLE_NONE : OPTIONS_DISABLE_CLEAR);
	WORD wCommand = TrackOptionsMenu(wDisableFlags);

	if (!wCommand)
		return false;


	bool bUseClient = false;
	switch (wCommand)
	{
		case ID_OPTIONS_CHECKFORUPDATES:
		case ID_OPTIONS_SIGNIN:
		case ID_OPTIONS_MYACCOUNT:
		case ID_OPTIONS_SHOWTRAYICON:
		case ID_OPTIONS_GIVEFEEDBACK:
		case ID_OPTIONS_UPGRADENOW:
		case ID_OPTIONS_JAZZYMAILHOME:
		case ID_OPTIONS_HELPCENTER:
		case ID_OPTIONS_ABOUTJAZZYMAIL:
		case ID_OPTIONS_OPTIMIZE:
		{
			bUseClient = false;
			break;
		}
		case ID_OPTIONS_CLEARSTATIONERY:
		case ID_OPTIONS_CLEARDEFAULTSTATIONERY:
		case ID_FILE_SMARTCACHE:
		case ID_OPTIONS_REFERAFRIEND:
		{
			bUseClient = true;
			break;
		}
	}

	bool bSuccess = false;
	if (m_pMailClient && bUseClient)
		bSuccess = m_pMailClient->HandleOptionsCommand(wCommand, SOURCE_TOOLBAR);
	else if (!m_pMailClient && wCommand == ID_OPTIONS_REFERAFRIEND)
		DisplayMessage(IDS_REFERFRIEND_WARN, MB_ICONINFORMATION | MB_OK | MB_TOPMOST);
	else
		bSuccess = HandleOptionsCommand(wCommand, SOURCE_TOOLBAR);

	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IPersistStream implementations
// 
// This is only supported to allow the desk band to be dropped on the 
// desktop and to prevent multiple instances of the desk band from showing 
// up in the shortcut menu. This desk band doesn't actually persist any data.
//

//=============================================================================
//   CJMExplorerBand::GetClassID()
//=============================================================================

STDMETHODIMP CJMExplorerBand::GetClassID(LPCLSID pClassID)
{
    *pClassID = CLSID_JMExplorerBand;
    
    return S_OK;
}

//=============================================================================
//   CJMExplorerBand::IsDirty() 
//=============================================================================

STDMETHODIMP CJMExplorerBand::IsDirty(void)
{
    return S_FALSE;
}

//=============================================================================
//   CJMExplorerBand::Load() 
//=============================================================================

STDMETHODIMP CJMExplorerBand::Load(LPSTREAM pStream)
{
    return S_OK;
}

//=============================================================================
//   CJMExplorerBand::Save() 
//=============================================================================

STDMETHODIMP CJMExplorerBand::Save(LPSTREAM pStream, BOOL fClearDirty)
{
    return S_OK;
}

//=============================================================================
//   CJMExplorerBand::GetSizeMax() 
//=============================================================================

STDMETHODIMP CJMExplorerBand::GetSizeMax(ULARGE_INTEGER *pul)
{
    return E_NOTIMPL;
}
///////////////////////////////////////////////////////////////////////////
//
// IOleWindow Implementation
//

//=============================================================================
//   CJMExplorerBand::GetWindow()
//=============================================================================

STDMETHODIMP CJMExplorerBand::GetWindow(HWND *phWnd)
{
    *phWnd = m_hWnd;

    return S_OK;
}

//=============================================================================
//   CJMExplorerBand::ContextSensitiveHelp() 
//=============================================================================

STDMETHODIMP CJMExplorerBand::ContextSensitiveHelp(BOOL fEnterMode)
{
    return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////
//
// IDockingWindow Implementation
//

//=============================================================================
//   CJMExplorerBand::ShowDW()  
//=============================================================================

STDMETHODIMP CJMExplorerBand::ShowDW(BOOL fShow)
{
    if(m_hWnd)
    {
		m_bShow = fShow;

		if (m_bShow)
			::ShowWindow(m_hWnd, SW_SHOW);
		else
			::ShowWindow(m_hWnd, SW_HIDE);
    }

    return S_OK;
}

//=============================================================================
//   CJMExplorerBand::CloseDW()   
//=============================================================================

STDMETHODIMP CJMExplorerBand::CloseDW(DWORD dwReserved)
{
    ShowDW(FALSE);

	if (!m_bSrvrShutdown)
		ServerUnregister(m_hWnd);

    
	if(::IsWindow(m_hWnd))
		::DestroyWindow(m_hWnd);
    
    m_hWnd = NULL;
       
    return S_OK;
}

//=============================================================================
//   CJMExplorerBand::ResizeBorderDW() 
//=============================================================================

STDMETHODIMP CJMExplorerBand::ResizeBorderDW(LPCRECT prcBorder, 
                                          IUnknown* punkSite, 
                                          BOOL fReserved)
{
    // This method is never called for Band Objects.
    return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IInputObject Implementation
//

//=============================================================================
//   CJMExplorerBand::UIActivateIO()  
//=============================================================================

STDMETHODIMP CJMExplorerBand::UIActivateIO(BOOL fActivate, LPMSG pMsg)
{
    if(fActivate)
		::SetFocus(m_hWnd);

    return S_OK;
}

//=============================================================================
//   CJMExplorerBand::HasFocusIO()
//   
//   If this window or one of its decendants has the focus, return S_OK. Return 
//   S_FALSE if neither has the focus.
//
//=============================================================================
STDMETHODIMP CJMExplorerBand::HasFocusIO(void)
{
    if(m_bFocus)
        return S_OK;
    
    return S_FALSE;
}

//=============================================================================
//   CJMExplorerBand::TranslateAcceleratorIO()
//   
//   If the accelerator is translated, return S_OK or S_FALSE otherwise.
//
//=============================================================================
STDMETHODIMP CJMExplorerBand::TranslateAcceleratorIO(LPMSG pMsg)
{
    return S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IObjectWithSite implementations
//
//=============================================================================
//   CJMExplorerBand::SetSite()
//=============================================================================
STDMETHODIMP CJMExplorerBand::SetSite(IUnknown *punkSite)
{
    HRESULT hr;

	//If a site is being held, release it.
    if(m_spSite)
		m_spSite = NULL;

    //If punkSite is not NULL, a new site is being set.
    if(punkSite)
    {
        //Get the parent window.
        CComPtr<IOleWindow>  spOleWindow;

        m_hwndParent = NULL;

        hr = punkSite->QueryInterface(IID_IOleWindow, (LPVOID*)&spOleWindow);
        if (SUCCEEDED(hr))
		{
            spOleWindow->GetWindow(&m_hwndParent);
            spOleWindow = NULL;
        }

        if(!m_hwndParent)
            return E_FAIL;

		// Minimum of Explorer 4 required
        CComQIPtr<IServiceProvider> spServiceProvider = punkSite;
		if (m_spWebBrowser)
			m_spWebBrowser = NULL;
		
        if (FAILED(spServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void**)&m_spWebBrowser)))
            return E_FAIL;


        if(!RegisterAndCreateWindow())
            return E_FAIL;


        //Get and keep the IInputObjectSite pointer.
        hr = punkSite->QueryInterface(IID_IInputObjectSite, (LPVOID*)&m_spSite);
		if (FAILED(hr))
            return E_FAIL;

		hr = Connect();
		if (FAILED(hr))
			return E_FAIL;

    }
	else  //If punkSite is NULL, explorer bar is being unloaded.
	{
		Disconnect();
	}
	
    return S_OK;
}

//=============================================================================
//   CJMExplorerBand::GetSite()
//=============================================================================
STDMETHODIMP CJMExplorerBand::GetSite(REFIID riid, LPVOID *ppvReturn)
{
    *ppvReturn = NULL;
    
    if(m_spSite)
        return m_spSite->QueryInterface(riid, ppvReturn);
    
    return E_FAIL;
}
//=============================================================================
//   CJMExplorerBand::Connect()
//=============================================================================
HRESULT CJMExplorerBand::Connect()
{
	HRESULT hr;
	hr = AtlAdvise(m_spWebBrowser, (IDispatch*)this, __uuidof(DWebBrowserEvents2), &m_dwCookie);
	return hr; 
}
//=============================================================================
//   CJMExplorerBand::Disconnect()
//=============================================================================
HRESULT CJMExplorerBand::Disconnect()
{
	HRESULT hr;
	hr = AtlUnadvise(m_spWebBrowser, __uuidof(DWebBrowserEvents2), m_dwCookie);
	m_dwCookie = 0;
	return hr; 
}

//=============================================================================
//   CJMExplorerBand::Invoke()
//=============================================================================
STDMETHODIMP CJMExplorerBand::Invoke(DISPID dispidMember, REFIID riid, 
								LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, 
								VARIANT* pvarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	switch (dispidMember)
	{
		/////////////////////////////////////////////////////////
		// The parameters for this DISPID are as follows:
		// [0]: Cancel flag  - VT_BYREF|VT_BOOL
		// [1]: IDispatch* - Pointer to an IDispatch interface. 
		//		You can set this parameter to the IDispatch of 
		//		a WebBrowser Control that you've created. When 
		//		you pass back an IDispatch like this, MSHTML will 
		//		use the control you've given it to open the link.
		//
	case DISPID_NEWWINDOW2:
		break;

		///////////////////////////////////////////////////////////
		// The parameters for this DISPID are as follows:
		// [0]: Cancel flag  - VT_BYREF|VT_BOOL
		// [1]: HTTP headers - VT_BYREF|VT_VARIANT
		// [2]: Address of HTTP POST data  - VT_BYREF|VT_VARIANT 
		// [3]: Target frame name - VT_BYREF|VT_VARIANT 
		// [4]: Option flags - VT_BYREF|VT_VARIANT
		// [5]: URL to navigate to - VT_BYREF|VT_VARIANT
		// [6]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event. 
		// 
		// User clicked a link or launched the browser.
		//
	case DISPID_BEFORENAVIGATE2:
		{
			HandleBeforeNavigate();
		}
		break;
		
		///////////////////////////////////////////////////////////
		// The parameters for this DISPID:
		// [0]: URL navigated to - VT_BYREF|VT_VARIANT
		// [1]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event. 
		//
		// Fires after a navigation to a link is completed on either 
		// a window or frameSet element.
		//
	case DISPID_NAVIGATECOMPLETE2:
		{
			// Check if m_lpDocCompleteDisp is NULL. If NULL, that means it is
			// the top level NavigateComplete2. Save the LPDISPATCH
			if (!m_lpDocCompleteDisp)
			{
				VARIANTARG varDisp = pDispParams->rgvarg[1];
				m_lpDocCompleteDisp = varDisp.pdispVal;
			}

		}
		break;
		
		///////////////////////////////////////////////////////////
		// The parameters for this DISPID:
		// [0]: New status bar text - VT_BSTR
		//
	case DISPID_STATUSTEXTCHANGE:
		break;
		
		///////////////////////////////////////////////////////////
		// The parameters for this DISPID:
		// [0]: Maximum progress - VT_I4
		// [1]: Amount of total progress - VT_I4
		//
	case DISPID_PROGRESSCHANGE:
		break;

		///////////////////////////////////////////////////////////
		// The parameters for this DISPID:
		// [0]: URL navigated to - VT_BYREF|VT_VARIANT
		// [1]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event. 
		//
		// Fires when a document has been completely loaded and initialized.
		// Unreliable -- currently, the DWebBrowserEvents2::DocumentComplete 
		// does not fire when the IWebBrowser2::Visible property of the 
		// WebBrowser Control is set to false (see Q259935).  Also, multiple 
		// DISPID_DOCUMENTCOMPLETE events can be fired before the final 
		// READYSTATE_COMPLETE (see Q180366).
		//
	case DISPID_DOCUMENTCOMPLETE:
		{
			VARIANTARG varDisp = pDispParams->rgvarg[1];
			if (m_lpDocCompleteDisp && m_lpDocCompleteDisp == varDisp.pdispVal)
			{
				// if the LPDISPATCH are same, that means
				// it is the final DocumentComplete. Reset m_lpDocCompleteDisp
				m_lpDocCompleteDisp = NULL;

				
				// Now handle the final nav event.
				if (!m_spWebBrowser)
					return E_POINTER;

				// Need to wait a minimum period of time to allow mail clients to
				// complete their initialization. For example, Yahoo and Hotmail clear the
				// compose area shortly after doc is complete. This causes the Creata Mail
				// default stationery to be erased - so we wait a little while to avoid.
				::SetTimer(m_hWnd, IDT_NEWDOC_TIMER, 300, NULL);
			}

		}
		break;
		
		///////////////////////////////////////////////////////////
		// No parameters
		//
		// Fires when a navigation operation is beginning.
		//
	case DISPID_DOWNLOADBEGIN:
		break;
		
		///////////////////////////////////////////////////////////
		// No parameters
		//
		// Fires when a navigation operation finishes, is halted, or fails.
		//
	case DISPID_DOWNLOADCOMPLETE:
		break;
		
		///////////////////////////////////////////////////////////
		// The parameters for this DISPID:
		// [0]: Enabled state - VT_BOOL
		// [1]: Command identifier - VT_I4
		//
	case DISPID_COMMANDSTATECHANGE:
		break;
		
		///////////////////////////////////////////////////////////
		// The parameters for this DISPID:
		// [0]: Document title - VT_BSTR
		// [1]: An object that evaluates to the top-level or frame
		//      WebBrowser object corresponding to the event.
		//
	case DISPID_TITLECHANGE:
		break;
		
		///////////////////////////////////////////////////////////
		// The parameters for this DISPID:
		// [0]: Name of property that changed - VT_BSTR
		//
	case DISPID_PROPERTYCHANGE:
		break;

		///////////////////////////////////////////////////////////
		// No parameters
		//
		// The BHO docs in MSDN say to use DISPID_QUIT, but this is an error.
		// The BHO never gets a DISPID_QUIT and thus the browser connection
		// never gets unadvised and so the BHO never gets the FinalRelease().
		// This is bad.  So use DISPID_ONQUIT instead and everything is cool --
		// EXCEPT when you exit the browser when viewing a page that launches
		// a popup in the onunload event!  In that case the BHO is already
		// unadvised so it does not intercept the popup.  So the trick is to
		// navigate to a known page (I used the about:blank page) that does 
		// not have a popup in the onunload event before unadvising.
		//
	case DISPID_ONQUIT:
		{
			Disconnect();
		}
		break;

	default:
		{
			IDispatchImpl<IJMExplorerBand, &IID_IJMExplorerBand, &LIBID_AgOutlookAddInLib, /*wMajor =*/ 1, /*wMinor =*/ 0>::Invoke(dispidMember, riid, lcid, wFlags, pDispParams, pvarResult, pExcepInfo, puArgErr);
		}
		break;
   }

   
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////
//
// IDeskBand implementation
//
//=============================================================================
//   CExplorerBar::GetBandInfo() 
//=============================================================================

STDMETHODIMP CJMExplorerBand::GetBandInfo(DWORD dwBandID, 
                                       DWORD dwViewMode, 
                                       DESKBANDINFO* pdbi)
{
    if(pdbi)
    {
        m_dwBandID = dwBandID;
        m_dwViewMode = dwViewMode;
    
        if(pdbi->dwMask & DBIM_MINSIZE)
        {
            pdbi->ptMinSize.x = MIN_SIZE_X;
            pdbi->ptMinSize.y = MIN_SIZE_Y;
        }
    
        if(pdbi->dwMask & DBIM_MAXSIZE)
        {
            pdbi->ptMaxSize.x = -1;
            pdbi->ptMaxSize.y = -1;
        }
    
        if(pdbi->dwMask & DBIM_INTEGRAL)
        {
            pdbi->ptIntegral.x = 1;
            pdbi->ptIntegral.y = 1;
        }
    
        if(pdbi->dwMask & DBIM_ACTUAL)
        {
            pdbi->ptActual.x = 0;
            pdbi->ptActual.y = 0;
        }
    
        if(pdbi->dwMask & DBIM_TITLE)
        {
			wcscpy_s(pdbi->wszTitle, _countof(pdbi->wszTitle), TITLE_CJMExplorerBand);
        }
    
        if(pdbi->dwMask & DBIM_MODEFLAGS)
        {
            pdbi->dwModeFlags = DBIMF_VARIABLEHEIGHT;
        }
       
        if(pdbi->dwMask & DBIM_BKCOLOR)
        {
            //Use the default background color by removing this flag.
            pdbi->dwMask &= ~DBIM_BKCOLOR;
        }
    
        return S_OK;
    }
    
    return E_INVALIDARG;
}

///////////////////////////////////////////////////////////////////////////
//
// private method implementations
//
////////////////////////////////////////////////////////////////////////////

//=============================================================================
//   CJMExplorerBand::WndProc()
//=============================================================================
LRESULT CALLBACK CJMExplorerBand::WndProc(HWND hWnd, 
                                       UINT uMessage, 
                                       WPARAM wParam, 
                                       LPARAM lParam)
{
	CJMExplorerBand *pThis = (CJMExplorerBand*)::GetWindowLong(hWnd,GWL_USERDATA);
    switch (uMessage)
    {
        case WM_NCCREATE:
        {
            LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
            pThis = (CJMExplorerBand*)(lpcs->lpCreateParams);
			::SetWindowLong(hWnd, GWL_USERDATA, (LONG)pThis);
    
            //set the window handle
            pThis->m_hWnd = hWnd;
        }
        break;
       
		case WM_CREATE:
			return pThis->OnCreate(lParam);

        case WM_PAINT:
            return pThis->OnPaint();

		case WM_SIZE :
			return pThis->OnSize(wParam, lParam);
       
        case WM_COMMAND:
            return pThis->OnCommand(wParam, lParam);

        case WM_SETFOCUS:
            return pThis->OnSetFocus();

        case WM_KILLFOCUS:
            return pThis->OnKillFocus();

		case WM_ERASEBKGND:
			return S_OK;

		case WM_TIMER:
			return pThis->OnTimer(wParam, lParam);
    }

	if (uMessage == UWM_SHUTDOWNALL)
		return pThis->OnShutdownAll();
	else if (uMessage == UWM_UPDATERESOURCE)
		return pThis->OnUpdateResource();
	else if (uMessage == UWM_HELP)
		return pThis->OnHelp();


	return ::DefWindowProc(hWnd, uMessage, wParam, lParam);
}

//=============================================================================
//   CExplorerBar::OnCreate() 
//=============================================================================
LRESULT CJMExplorerBand::OnCreate(LPARAM lParam)
{
	UTILITYCALLBACK pCallBack = (UTILITYCALLBACK)CJMExplorerBand::UtilityCallback;
	m_UtilWnd.Initialize(m_hWnd, pCallBack, (LPARAM)this); 

	// Save reference to window handle
	m_hClientWnd = m_hWnd;

	// Register this mail client with JMSrvr.
	ServerRegister(m_hWnd, CLIENT_TYPE_IE);
	
    return S_OK;
}

//=============================================================================
//   CExplorerBar::OnPaint() 
//=============================================================================
LRESULT CJMExplorerBand::OnPaint(void)
{
	if (m_BrowserWnd)
		m_BrowserWnd.RedrawWindow();
    return S_OK;
}
//=============================================================================
//   CExplorerBar::OnSize() 
//=============================================================================
LRESULT CJMExplorerBand::OnSize(WPARAM wParam, LPARAM lParam)
{
	if (!m_hWnd)
		return S_OK;

	// wParam values: SIZE_MAXIMIZED, SIZE_MINIMIZED, SIZE_RESTORED
	if (wParam == SIZE_MINIMIZED)
		return S_OK;

	// Get the new client size
	int iWidth = LOWORD(lParam);
	int iHeight = HIWORD(lParam);

	if (m_BrowserWnd)
	{ // Maintained to be the full size of the client area
		RECT rect = {0, 0, iWidth, iHeight};
		m_BrowserWnd.SetWindowPos(NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		//::SetWindowPos(m_BrowserWnd, NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	
    return S_OK;
}
//=============================================================================
//   CExplorerBar::OnCommand() 
//=============================================================================
LRESULT CJMExplorerBand::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return S_OK;
}

//=============================================================================
//   FocusChange() 
//=============================================================================
void CJMExplorerBand::FocusChange(BOOL bFocus)
{
    m_bFocus = bFocus;
    
    //inform the input object site that the focus has changed
    if(m_spSite)
    {
        m_spSite->OnFocusChangeIS((IDockingWindow*)this, bFocus);
    }
}

//=============================================================================
//   OnSetFocus()  
//=============================================================================

LRESULT CJMExplorerBand::OnSetFocus(void)
{
    FocusChange(TRUE);
	LoadPage();

    return S_OK;
}

//=============================================================================
//   OnKillFocus() 
//=============================================================================

LRESULT CJMExplorerBand::OnKillFocus(void)
{
    FocusChange(FALSE);
    
    return S_OK;
}
//=============================================================================
//   OnTimer() 
//=============================================================================
LRESULT CJMExplorerBand::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if (IDT_NEWDOC_TIMER == wParam) 
	{
		::KillTimer(m_hWnd, IDT_NEWDOC_TIMER);
		HandleNewDoc(m_spWebBrowser);
	}
	
	return S_OK;
}
//=============================================================================
//  OnShutdownAll() 
//=============================================================================
LRESULT CJMExplorerBand::OnShutdownAll()
{
	m_bSrvrShutdown = true;
	HandleShutdown();
	if (m_pMailClient)
		m_pMailClient->HandleShutdown();

    if (::IsWindow(m_UtilWnd.m_hWnd))
		m_UtilWnd.DestroyWindow();

	//exit(EXIT_SUCCESS);

	return S_OK;
}
//=============================================================================
//    OnHelp()
//=============================================================================
bool CJMExplorerBand::OnHelp()
{
    if (m_ClientID == CLIENT_ID_HOTMAIL)
		ShowHelp(IDS_FAQ_HOTMAIL_URL);
	else if (m_ClientID == CLIENT_ID_YAHOOMAIL)
		ShowHelp(IDS_FAQ_YAHOO_URL);
	else
		ShowHelp(IDS_FAQ_GENERAL_URL);

	return true;
}
//=============================================================================
//	OnUpdateResource
//=============================================================================
LRESULT CJMExplorerBand::OnUpdateResource()
{
	AutoUpdateDone(UPDATE_RESOURCES);
	return S_OK;
}
//=============================================================================
//	Download
//=============================================================================
STDMETHODIMP CJMExplorerBand::Download(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice, VARIANT varExternalLink)
{
	CFuncLog log(g_pLog, "CJMExplorerBand::Download()");
	
	// If not registered, show Register prompt
	if (!VerifyUserStatus())
		return S_OK;

	// If we don't have a valid web mail client, check
	// to see if we can obtain one.
	if (!m_pMailClient)
		HandleNewDoc(m_spWebBrowser);

	// Display warning if still no mail client, i.e. no compose window.
	if (!IsMailClientValid(true))
		return S_OK;

	return m_pMailClient->HandleDownloadRequest(varProductId, varName, varDownload, varFlags, varWidth, varHeight, varFlashPath, varPathPrice, varExternalLink);
}
//=============================================================================
//	Download2
//=============================================================================
STDMETHODIMP CJMExplorerBand::Download2(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath,  VARIANT varPathPrice)
{
	CFuncLog log(g_pLog, "CJMExplorerBand::Download2()");

	// If not registered, show Register prompt
	if (!VerifyUserStatus())
		return S_OK;
	
	// If we don't have a valid web mail client, check
	// to see if we can obtain one.
	if (!m_pMailClient)
		HandleNewDoc(m_spWebBrowser);
	
	// Display warning if still no mail client, i.e. no compose window.
	if (!IsMailClientValid(true))
		return S_OK;

	return m_pMailClient->HandleDownloadRequest(varProductId, varName, varDownload, varFlags, varWidth, varHeight, varFlashPath, varPathPrice, CComVariant(CComBSTR("")));
}

//=============================================================================
//	ExecuteCommand
//=============================================================================
STDMETHODIMP CJMExplorerBand::ExecuteCommand(VARIANT varCmdId)
{
	CFuncLog log(g_pLog, "CJMExplorerBand::ExecuteCommand()");
	
	ProcessExecuteCommand(varCmdId.intVal);

	return S_OK;
}