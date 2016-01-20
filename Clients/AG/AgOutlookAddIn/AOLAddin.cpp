// AOLAddin.cpp : Implementation of CAOLAddin
#include "stdafx.h"
#include "AOLAddin.h"
#include "messagebox.h"


// CAOLAddin
// CAOLAddin
CString CAOLAddin::m_strHTMLText = "";

static UINT IDT_IELOAD_TIMER	= ::RegisterWindowMessage("Creata Mail IE Load Timer");

CComPtr<IAOLAddin> g_spAOLAddin;
HWND g_hAOLAddin;
WNDPROC g_pfAOLWndProc	= NULL;
WNDPROC g_pfAOLComposeWndProc = NULL;

/////////////////////////////////////////////////////////////////////////////
CAOLAddin::CAOLAddin()
{
	SetClient(CLIENT_TYPE_AOL);
	SetExpress(false);
	m_Download.SetAppInfo(this->m_spClientSite, CLIENT_TYPE_AOL);
	m_bWaitingForCompose		= false;
	m_hActiveCompose			= NULL;
	m_hActiveIE					= NULL;
	m_Usage.SetMethod(METH_AOL);
#ifdef _DEBUG
	//::MessageBox(NULL, "CAOLAddin Contructed", g_szAppName, MB_OK);
#endif _DEBUG
}

/////////////////////////////////////////////////////////////////////////////
CAOLAddin::~CAOLAddin()
{
	//g_spAOLAddin				= NULL;
#ifdef _DEBUG
	//::MessageBox(NULL, "CAOLAddin Destroyed", g_szAppName, MB_OK);
#endif _DEBUG
}

//=============================================================================
//	CleanUp
//=============================================================================
void CAOLAddin::CleanUp()
{
	if (IsWindow())
	{
		DestroyWindow();
		g_hAOLAddin = NULL;
		//g_spAOLAddin = NULL;
	}
}
//=========================================================================
// AOLComposeWndProc
//=========================================================================
LRESULT CALLBACK AOLComposeWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
    {
		case WM_PARENTNOTIFY:
		{
			if (LOWORD(wParam) == WM_LBUTTONDOWN)
			{
				POINT pt;
				pt.x = LOWORD(lParam);
				pt.y = HIWORD(lParam);

				HWND hChild = ChildWindowFromPoint(hwnd, pt);
				if (hChild)
				{
					TCHAR szTitle[MAX_PATH];
					::GetWindowText(hChild, szTitle, sizeof(szTitle));
					CString szCaption = szTitle;
					if (szCaption.Find(AOL_SMILEY_TITLE) >= 0)
					{
						if (::IsWindow(g_hAOLAddin))
							::PostMessage(g_hAOLAddin, UWM_AOLSMILEY, (WPARAM)hwnd, lParam);
					}
					else if (szCaption.Find(AOL_SENDNOW_TITLE) >= 0)
					{
						if (::IsWindow(g_hAOLAddin))
							::SendMessage(g_hAOLAddin, UWM_MAILSEND, (WPARAM)hwnd, lParam);
					}
				}
			}
			break;
		}

		case WM_DESTROY:
		{
			RestoreWindowProc(hwnd, g_pfAOLComposeWndProc); 
			LRESULT lres = CallWindowProc(g_pfAOLComposeWndProc, hwnd, uMsg, wParam, lParam);
			g_pfAOLComposeWndProc = NULL;
			return lres;
		}
		
		default:
			break;
	}

	return CallWindowProc(g_pfAOLComposeWndProc, hwnd, uMsg, wParam, lParam);
}
//=========================================================================
// AOLWndProc
//=========================================================================
LRESULT CALLBACK AOLWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
    {
		case WM_CLOSE:
		{
			/*if (InSendMessage())
				ReplyMessage(TRUE);*/

			ATLTRACE2( atlTraceGeneral, 4, "AOLWndProc: WM_CLOSE \n");
			break;	
		}

		case WM_DESTROY:
		{
			::SendMessage(g_hAOLAddin, UWM_SHUTDOWN, 0, 0);
			RestoreWindowProc(hwnd, g_pfAOLWndProc);
			LRESULT lres = CallWindowProc(g_pfAOLWndProc, hwnd, uMsg, wParam, lParam);
			g_pfAOLWndProc = NULL;
			return lres;
		}

		case WM_NCDESTROY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "AOLWndProc: WM_NOTIFY \n");
			break;
		}

		case WM_SIZE:
		{
			//::PostMessage(g_hAOLAddin, UWM_UPDATESIZE, wParam, lParam);
			break;
		}

		case WM_ACTIVATE:
		{
			/*if (::IsWindow(g_hAOLAddin))
				::PostMessage(g_hAOLAddin, UWM_CMACTIVATE, wParam, lParam);*/
			break;
		}
		
		default:
			break;
	}

	return CallWindowProc(g_pfAOLWndProc, hwnd, uMsg, wParam, lParam);
}
//=============================================================================
//	CAOLAddin::HandleShutdown
//=============================================================================
void CAOLAddin::HandleShutdown()
{
	ServerUnregister(m_hWnd);
	CMsAddInBase::HandleShutdown();

	return;
}
//=============================================================================
//	CAOLAddin::Popup
//=============================================================================
void CAOLAddin::Popup(int iType, int iButtonIndex, HWND hwndRebar, LPCTSTR pQueryString, bool bActivate)
{
	CFuncLog log(g_pLog, "CAOLAddin::Popup()");

	// If not registered, ignore button clicks and show Register prompt
	if (!VerifyUserStatus())
		return;

	

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

	CComQIPtr<IAOLAddin> pAddIn = this;
	CComQIPtr<IDispatch> pDispatch = pAddIn;

	if (iType == POPUP_DIALOG)
	{
		//HWND hWndParent = ::GetDesktopWindow();
		m_HTMLDialog.Popup(m_hTopParent, pDispatch, strURL, NULL, bActivate);
		m_AutoUpdate.Start(true/*bSilent*/);
	}
}
//=============================================================================
//	DisplayMessage
//=============================================================================
int CAOLAddin::DisplayMessage(DWORD dwMsgID, UINT uType)
{
	CString szMsg;
	szMsg.LoadString(dwMsgID);
	
	CMessagebox Msg;
	return Msg.ShowMessagebox(m_hWnd, szMsg, g_szAppName, uType);
}
//=============================================================================
//	LaunchMail
//=============================================================================
bool CAOLAddin::LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject)
{
	CFuncLog log(g_pLog, "CAOLAddin::LaunchMail()");
	CString strHTMLText;
	if (strDownloadSrc.IsEmpty())
		strHTMLText = strDownloadDst;
	else
		strHTMLText = ReadHTML(strDownloadSrc, strDownloadDst, false/*bPreview*/);

	m_strHTMLText = strHTMLText;

	

	// If active compose window exists then write html to this one.
	UpdateComposeWindowHandle();
	bool bSuccess = false;
	if (bAllowReuse && ::IsWindow(m_hActiveCompose))
	{
		IHTMLDocument2Ptr spDoc = GetHTMLDocument2(m_hActiveCompose);
		if (spDoc != NULL && IsDocInEditMode(spDoc))
		{
			// Wait for the document to become ready
			if (!IsDocLoadedWait(spDoc))
				return false;

			// Make sure the Add_a_Photo dialog is not up
			if (m_PhotoManager.HasOpenDialog())
				return false;

			// Check Security policies
			if (!VerifySecurity(spDoc, m_strHTMLText))
			{
				m_strHTMLText.Empty();
				return false;
			}

			// Reset current hooks.
			DeactivateJazzyMailDocument(spDoc);

			// Identify non Creata Mail text to preserve, such as signatures
			MarkNonJazzyMailTextForPreservation(spDoc);

			// Extract any html that will be retained from current message
			CString strPrevHtmlText;
			GetHtmlText(spDoc, strPrevHtmlText);

			// Write the html to message window
			if (WriteHtmlToDocument(CComBSTR(m_strHTMLText), spDoc))
			{					
				// Write saved html text from previous message
				SetHtmlText(spDoc, strPrevHtmlText);

				// Setup edit fields to identify which fields are editble 
				// by user and update text insertion point
				SetDocEditFields(spDoc);

				// Activate the new document to set hooks
				ActivateJazzyMailDocument(spDoc, true);
				bSuccess = true;
			}

			m_strHTMLText.Empty();
		}
		
	}

	// If unable to use the active compose window then create a new mail message
	// If just setting stationery default do not launch new mail
	if (!bSuccess && m_wLastCommand != ID_CONTENT_SETDEFAULT)
	{
		CString szMsg;
		szMsg.LoadString(IDS_AOL_COMPOSE_WARN);
		::MessageBox(NULL, szMsg, g_szAppName, MB_OK|MB_ICONINFORMATION);
		bSuccess = false;
	}
	
	return bSuccess;
}

//=============================================================================
//	LaunchFeedbackMail
//=============================================================================
bool CAOLAddin::LaunchFeedbackMail(DWORD dwCmd)
{
	CFuncLog log(g_pLog, "CAOLAddin::LaunchFeedbackMail()");
	CString strUrl = "/mailurl:mailto:tsoltis@ag.com?cc=jmccurdy@ag.com&subject=Creata Mail%20";
	strUrl += (dwCmd == ID_OPTIONS_GIVEFEEDBACK ? "Feedback" : "Problem%20Report");
	CString strTargetPath = "msimn.exe";
	GetAppPath(strTargetPath);
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", strTargetPath, strUrl, NULL, SW_SHOWNORMAL);
	return true;
}

//=============================================================================
//	EmbedHTMLAtEnd
//=============================================================================
bool CAOLAddin::EmbedHTMLAtEnd(const CString& strHTMLText)
{
	UpdateComposeWindowHandle();
	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(m_hActiveCompose);
	return EmbedHTMLAtEndBase(spDoc, strHTMLText);
}

//=============================================================================
//	EmbedHTMLAtSelection
//=============================================================================
bool CAOLAddin::EmbedHTMLAtSelection(const CString& strHTMLText)
{
	CFuncLog log(g_pLog, "CAOLAddin::EmbedHTMLAtSelection()");

	UpdateComposeWindowHandle();

	// Get a pointer to the doc and embed the HTML
	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(m_hActiveCompose);
	return EmbedHTMLAtSelectionBase(strHTMLText, spDoc);
}
//=============================================================================
//	EmbedHTMLAtSelection
//=============================================================================
void CAOLAddin::UpdateComposeWindowHandle()
{
	// Find taget compose window. For example, there may be multiple compose windows.  
	HWND hwndIEServer = NULL;
	HWND hwndCompose = NULL;
	GetIEServerWindow(m_hTopParent, hwndIEServer, false);
	if (hwndIEServer)
	{
		if (FindComposeWindow(hwndIEServer, hwndCompose))
			m_hActiveCompose = hwndCompose;
	}
	::BringWindowToTop(m_hActiveCompose);
}
//=============================================================================
//	IsComposeWindow
//=============================================================================
bool CAOLAddin::IsComposeWindow(HWND hwndCompose)
{
	if (!::IsWindow(hwndCompose))
		return false;

	TCHAR szClass[100];
	::GetClassName(hwndCompose, szClass, sizeof(szClass) - 1);
	
	if (_tcscmp(szClass, AOL_COMPOSEWINDOW_CLASS))
			return false;

	return true;
}
//=============================================================================
//	FindComposeWindow
//=============================================================================
bool CAOLAddin::FindComposeWindow(HWND hwndIEServer, HWND& hwndCompose)
{
	if (!::IsWindow(hwndIEServer))
		return false;

    // Find compose window by iterating thru parents starting with IE server window.
    // If a parent's title matches the compose title then it is the compose window.
    HWND hwndParent;
    HWND hwnd = hwndIEServer;
    bool bSuccess = false;
	TCHAR szTitle[MAX_PATH];
	CString szCaption;
    while (!bSuccess && NULL != hwnd)
    {
        hwndParent = ::GetParent(hwnd);
       
        // Get window title
		if (::GetWindowText(hwndParent, szTitle, sizeof(szTitle)))
        {
			szCaption = szTitle; 
			if (szCaption.Find(AOL_COMPOSE_TITLE) >= 0		|| 
				szCaption.Find(AOL_REPLYCOMPOSE_TITLE) >= 0 || 
				szCaption.Find(AOL_FWDCOMPOSE_TITLE) >= 0)
            {
                hwndCompose = hwndParent;
                bSuccess = true;
            }
        }

        hwnd = hwndParent;
    }

	return bSuccess;
}

//=============================================================================
//	SetSubject
//=============================================================================
bool CAOLAddin::SetSubject(HWND hwndCompose, CString &szSubject)
{
	bool bSuccess = false;

	if (!IsComposeWindow(hwndCompose))
		return bSuccess;

	CWindow Wnd;
	Wnd.Attach(hwndCompose);
	if (Wnd.SetWindowText(szSubject))
		bSuccess = true;

	Wnd.Detach();

	return bSuccess;

}

//=============================================================================
//	GetTitle
//=============================================================================
bool CAOLAddin::GetTitle(HWND hwndCompose, CString &szSubject)
{
	bool bSuccess = false;

	if (!IsComposeWindow(hwndCompose))
		return bSuccess;

	CWindow Wnd;
	Wnd.Attach(hwndCompose);
	CComBSTR bsTitle;
	if (Wnd.GetWindowText(&bsTitle))
	{
		szSubject = bsTitle;
		bSuccess = true;
	}

	Wnd.Detach();

	return bSuccess;
}

//================================================
//   CAOLAddin::LoadPage()
//================================================
bool CAOLAddin::LoadPage(int nCat, bool bActivate)
{
	DWORD dwCat = DEFAULT_CAT;

	if (nCat < 0 || nCat > LAST_CAT_USED)
		m_Authenticate.ReadDWORDStatus(REGVAL_LAST_AOL_CAT, dwCat);
	else
		dwCat = (DWORD) nCat;

	Popup(NULL, 0, NULL, ITEM_INFO[dwCat].szQuery, bActivate);

	return true;
}
//================================================
//   CAOLAddin::SubclassAOLCompose()
//================================================
bool CAOLAddin::SubclassAOLCompose()
{
	if (!::IsWindow(m_hActiveCompose))
		return false;


	if (!g_pfAOLComposeWndProc)
		g_pfAOLComposeWndProc = SetWindowProc(m_hActiveCompose, AOLComposeWndProc);

	return true;
}
//================================================
// OnIEServer
//================================================
// This handler is called every time an Internet Explorer_Server window is created.
LRESULT CAOLAddin::OnIEServer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hwndIEServer = (HWND)lParam;
	if (!::IsWindow(hwndIEServer))
		return 0;

	CFuncLog log(g_pLog, "CAOLAddin::OnIEServer()");

    HWND hwndCompose = NULL;
    if (!FindComposeWindow(hwndIEServer, hwndCompose))
        return 0;


	m_hActiveIE = hwndCompose;
    m_hActiveCompose = hwndCompose;

	SetTimer(IDT_IELOAD_TIMER, 300, NULL);
	
	return 0;
}


//=============================================================================
//	OnShutdown
//=============================================================================
LRESULT CAOLAddin::OnForceShutdown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CFuncLog log(g_pLog, "CAOLAddin::OnForceShutdown()");
	HandleShutdown();
	return S_OK;
}
//=============================================================================
//	OnShutdownAll
//=============================================================================
LRESULT CAOLAddin::OnShutdownAll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bSrvrShutdown = true;
	HandleShutdown();
	return S_OK;
}
//=============================================================================
//	OnUpdateResource
//=============================================================================
LRESULT CAOLAddin::OnUpdateResource(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	AutoUpdateDone(UPDATE_RESOURCES);
	return S_OK;
}
//=============================================================================
//	OnRemoveAddin
//=============================================================================
LRESULT CAOLAddin::OnRemoveAddin(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_Authenticate.IsExpressEnabled())
		return S_OK;

	m_bSrvrShutdown = false;
	HandleShutdown();
	return S_OK;
}
//=============================================================================
//	OnLaunch
//=============================================================================
LRESULT CAOLAddin::OnLaunch(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LoadPage();
	return S_OK;
}
//=============================================================================
//	OnHelp
//=============================================================================
LRESULT CAOLAddin::OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CFuncLog log(g_pLog, "CAOLAddin::OnHelp()");
	ShowHelp(IDS_FAQ_GENERAL_URL);
	return S_OK;
}
//=============================================================================
//   OnUpdateSize()
//=============================================================================
LRESULT CAOLAddin::OnUpdateSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	m_HTMLDialog.UpdateSize(wParam, lParam);
	return S_OK;
}
//=============================================================================
//   OnCMActivate()
//=============================================================================
LRESULT CAOLAddin::OnCMActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	::PostMessage(m_HTMLDialog.m_hWnd, UWM_CMACTIVATE, wParam, lParam);
	return S_OK;
}
//=============================================================================
//   OnAOLSmiley()
//=============================================================================
LRESULT CAOLAddin::OnAOLSmiley(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (::IsWindow(m_HTMLDialog.m_hWnd))
		return S_OK;
	
	LoadPage(1 /* Smileys*/, false /* Activate */);

	HWND hwnd = (HWND)wParam;
	if (::IsWindow(hwnd))
		::SetWindowPos(hwnd, m_HTMLDialog.m_hWnd, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	

	return S_OK;
}
//=============================================================================
//   OnMailSend()
//=============================================================================
LRESULT CAOLAddin::OnMailSend(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	HWND hCompose = (HWND)wParam;
	if (!::IsWindow(hCompose))
		return S_OK;
	
	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(hCompose);
	HandleOnItemSend(spDoc);
	return S_OK;
}
//=============================================================================
//	OnTimer
//=============================================================================
LRESULT CAOLAddin::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CFuncLog log(g_pLog, "CAOLAddin::OnTimer()");
	if (IDT_IELOAD_TIMER == wParam) 
	{
		if (!::IsWindow(m_hActiveIE))
			return 0;

		SubclassAOLCompose();
		
        // Load Creata Mail browser page
        LoadPage();

		// If the document is not loaded yet (or it is NULL), keep checking
		IHTMLDocument2Ptr spDoc = GetHTMLDocument2(m_hActiveIE);
		if (!IsDocLoaded(spDoc))
			return 0;
		
		// The document is loaded, stop the timer
		KillTimer(IDT_IELOAD_TIMER);

		// Will we be adding content?
		bool bNeedToAddContent = (m_bWaitingForCompose && !m_strHTMLText.IsEmpty());
		bool bBackgroundContent = (bNeedToAddContent &&
					(m_DownloadParms.iCategory != TYPE_CLIPART && 
					 m_DownloadParms.iCategory != TYPE_SMILEYS &&
					 m_DownloadParms.iCategory != TYPE_SOUNDS));

		// Will we be adding stationery?
		CString szTitle;
		GetTitle(m_hActiveIE, szTitle);
		bool bNewMessage = (szTitle.CompareNoCase(AOL_COMPOSE_TITLE) == 0);
		bool bNeedToAddStationery = (m_StationeryMgr.NeedToAdd() && bNewMessage);

		// Will we be changing the document?
		bool bNeedToChange = bNeedToAddStationery || bNeedToAddContent;
		if (!bNeedToChange)
			return 0;

		// Check Security policies
		bool bSecurityPassed = VerifySecurity(spDoc, m_strHTMLText);
		if (!bSecurityPassed)
		{
			m_strHTMLText.Empty();
			return 0;
		}

		// Identify non Creata Mail text to preserve, such as signatures
		MarkNonJazzyMailTextForPreservation(spDoc);
		
		// Add the default stationery
		if (bNeedToAddStationery && !bBackgroundContent)
		{
			AddDefaultStationery(spDoc);
		}
			
		// Add the Creata Mail content. If the content is embedded type,
		// then we simply insert the html. Otherwise, we get any current
		// html text that may be present (i.e. Signatures) then write the new
		// html to doc (overwritting previous html) then we set edit fields to 
		// which ones are user editable as well as update the text insertion point. 
		if (bNeedToAddContent)
		{
			if (!bBackgroundContent)
			{
				// Restore any text that was retained from previous message
				if (!SetHtmlText(spDoc, m_strHTMLText))
					return 0;
			}
			else 
			{
				// Extract any html that will be retained from current message
				CString strPrevHtmlText;
				GetHtmlText(spDoc, strPrevHtmlText);

				// Write the html to message window
				if (WriteHtmlToDocument(CComBSTR(m_strHTMLText), spDoc))
				{
					// Write saved html text from previous message
					SetHtmlText(spDoc, strPrevHtmlText);

					// Setup edit fields to identify which fields are editble 
					// by user and update text insertion point
					SetDocEditFields(spDoc);
				}
			}

			m_strHTMLText.Empty();
		}
		
		// Activate the new document to set hooks
		ActivateJazzyMailDocument(spDoc, m_bWaitingForCompose);
	}
	

	return 0;
}


//=============================================================================
//	OnItemSend
//=============================================================================
STDMETHODIMP CAOLAddin::OnItemSend()
{	
	CFuncLog log(g_pLog, "CAOLAddin::OnItemSend()");

	// Make sure this is a JazzMail document.
	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(m_hActiveCompose);
	HandleOnItemSend(spDoc);
	return S_OK;
}

//=============================================================================
//	OnShutDown
//=============================================================================
STDMETHODIMP CAOLAddin::OnShutDown()
{	
	CFuncLog log(g_pLog, "CAOLAddin::OnShutDown()");
	HandleShutdown();
	return S_OK;
}

//=============================================================================
//	Download2
//=============================================================================
STDMETHODIMP CAOLAddin::Download2(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath,  VARIANT varPathPrice)
{
	CFuncLog log(g_pLog, "CAOLAddin::Download2()");
	return HandleDownloadRequest(varProductId, varName, varDownload, varFlags, varWidth, varHeight, varFlashPath, varPathPrice, CComVariant(CComBSTR("")));
}


//=============================================================================
//	GetWindowHandle
//=============================================================================
STDMETHODIMP CAOLAddin::GetWindowHandle(OLE_HANDLE* hWnd)
{
	CFuncLog log(g_pLog, "CAOLAddin::GetWindowHandle()");
	*hWnd = (OLE_HANDLE)m_hWnd;
	return S_OK;
}

//=============================================================================
//	SetActiveCompose
//=============================================================================
STDMETHODIMP CAOLAddin::SetActiveCompose(OLE_HANDLE hwndCompose, UINT State)
{
	if (!hwndCompose)
		return S_OK;

	if (State == WA_ACTIVE)
	{
		m_hActiveCompose = (HWND)hwndCompose;
	}
	else
	if (State == WA_INACTIVE)
	{
		if (m_hActiveCompose == (HWND)hwndCompose) 
			m_hActiveCompose = NULL;				
	}

	
	return S_OK;
}
//=============================================================================
//	ExecuteCommand
//=============================================================================
STDMETHODIMP CAOLAddin::ExecuteCommand(VARIANT varCmdId)
{
	CFuncLog log(g_pLog, "CAOLAddin::ExecuteCommand()");
	
	ProcessExecuteCommand(varCmdId.intVal);

	return S_OK;
}
//=============================================================================
//	SetTopParent
//=============================================================================
STDMETHODIMP CAOLAddin::SetTopParent(OLE_HANDLE hwnd)
{
	m_hTopParent = (HWND) hwnd;

	if (!g_pfAOLWndProc)
		g_pfAOLWndProc = SetWindowProc(m_hTopParent, AOLWndProc);

	return S_OK;
}