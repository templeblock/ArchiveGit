// OEAddin.cpp : Implementation of COEAddin
#include "stdafx.h"
#include "OEAddin.h"
#include "JMToolbar.h"
//#include "HelperFunctions.h"

// COEAddin
CString COEAddin::m_strHTMLText = "";

#define TOOLBAR_INIT		0x0001
#define TOOLBAR_COMPLETE	0x0002

static UINT IDT_IELOAD_TIMER	= ::RegisterWindowMessage("Creata Mail IE Load Timer");
static UINT IDT_REFRESH_TIMER	= ::RegisterWindowMessage("Creata Mail Refresh Timer");

CComPtr<IOEAddin> g_spOEAddin;
HWND g_hOEAddin;

/////////////////////////////////////////////////////////////////////////////
COEAddin::COEAddin()
{
	SetClient(CLIENT_TYPE_EXPRESS);
	SetExpress(true);
	m_Download.SetAppInfo(this->m_spClientSite, CLIENT_TYPE_EXPRESS);
	m_bRefreshTimerInUse		= false;
	m_bWaitingForCompose		= false;
	m_hActiveCompose			= NULL;
	m_hRefreshCompose			= NULL;
	m_hActiveIE					= NULL;
	m_Usage.SetMethod(METH_EXPRESS);
}

/////////////////////////////////////////////////////////////////////////////
COEAddin::~COEAddin()
{
	if (m_bRefreshTimerInUse)
		KillTimer(IDT_REFRESH_TIMER); 

#ifdef _DEBUG
	::MessageBox(NULL, "COEAddin Destroyed", g_szAppName, MB_OK);
#endif _DEBUG
}

//=============================================================================
//	CleanUp
//=============================================================================
void COEAddin::CleanUp()
{
	// Free all toolbar pointers.
	EraseAll();

	if (IsWindow())
	{
		DestroyWindow();
		g_hOEAddin = NULL;
	}
}


//=============================================================================
//	COEAddin::Popup
//=============================================================================
void COEAddin::Popup(int iType, int iButtonIndex, HWND hwndRebar, LPCTSTR pQueryString)
{
	CFuncLog log(g_pLog, "COEAddin::Popup()");

	// If not registered, ignore button clicks and show Register prompt
	if (!VerifyUserStatus())
		return;

	// Find the toolbar info associated with this Rebar.
	OEInfoType TBInfo;
	if (!GetToolbarFromRebar(hwndRebar, &TBInfo))
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

	CComQIPtr<IOEAddin> pAddIn = this;
	CComQIPtr<IDispatch> pDispatch = pAddIn;

	if (iType == POPUP_DIALOG)
	{
		//HWND hWndParent = ::GetDesktopWindow();
		CWindow TempWindow;
		TempWindow.Attach(hwndRebar);
		m_hTopParent = TempWindow.GetTopLevelParent();
		TempWindow.Detach();
		m_HTMLDialog.Popup(m_hTopParent, pDispatch, strURL);
		m_AutoUpdate.Start(true/*bSilent*/);
	}
	else
	if (iType == POPUP_MENU)
	{
		RECT rect;
		_ASSERTE(NULL != TBInfo.pToolbar);
		if (TBInfo.pToolbar->GetItemRect(iButtonIndex, &rect))
		{
			POINT ptTemp;
			ptTemp.x = rect.left;
			ptTemp.y = rect.bottom;
			HWND hWndParent = TBInfo.pToolbar->m_hWnd;
			::ClientToScreen(hWndParent, &ptTemp);
			m_HTMLMenu.Popup(hWndParent, pDispatch, ptTemp.x, ptTemp.y, strURL);
		}
	}
}

//=============================================================================
//	LaunchMail
//=============================================================================
bool COEAddin::LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject)
{
	CFuncLog log(g_pLog, "COEAddin::LaunchMail()");
	CString strHTMLText;
	if (strDownloadSrc.IsEmpty())
		strHTMLText = strDownloadDst;
	else
		strHTMLText = ReadHTML(strDownloadSrc, strDownloadDst, false/*bPreview*/);

	m_strHTMLText = strHTMLText;

	

	// If active compose window exists then write html to this one.
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
		// Wait for compose window to launch.
		m_bWaitingForCompose = true;
			
		CString strUrl = "/mailurl:mailto:";
		strUrl += (strSubject.IsEmpty() ? "" : "?subject=" + strSubject);
		CString strTargetPath = "msimn.exe";
		GetAppPath(strTargetPath);
		int iResult = (int)::ShellExecute(NULL, "open", strTargetPath, strUrl, NULL, SW_SHOWNORMAL);
		if (iResult <= 32)
		{
			::MessageBox(NULL, "Failed to launch Outlook Express", "LaunchOE", MB_OK);
			return false;
		}

		bSuccess = true;
	}
	
	return bSuccess;
}

//=============================================================================
//	LaunchFeedbackMail
//=============================================================================
bool COEAddin::LaunchFeedbackMail(DWORD dwCmd)
{
	CFuncLog log(g_pLog, "COEAddin::LaunchFeedbackMail()");
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
bool COEAddin::EmbedHTMLAtEnd(const CString& strHTMLText)
{
	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(m_hActiveCompose);
	return EmbedHTMLAtEndBase(spDoc, strHTMLText);
}

//=============================================================================
//	EmbedHTMLAtSelection
//=============================================================================
bool COEAddin::EmbedHTMLAtSelection(const CString& strHTMLText)
{
	CFuncLog log(g_pLog, "COEAddin::EmbedHTMLAtSelection()");
	// Get a pointer to the doc and embed the HTML
	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(m_hActiveCompose);
	return EmbedHTMLAtSelectionBase(strHTMLText, spDoc);
}

//=============================================================================
//	IsComposeWindow
//=============================================================================
bool COEAddin::IsComposeWindow(HWND hwndCompose)
{
	if (!::IsWindow(hwndCompose))
		return false;

	TCHAR szClass[100];
	::GetClassName(hwndCompose, szClass, sizeof(szClass) - 1);
	
	if (_tcscmp(szClass, OE_COMPOSEWINDOW_CLASS))
			return false;

	return true;
}

//=============================================================================
//	SetSubject
//=============================================================================
bool COEAddin::SetSubject(HWND hwndCompose, CString &szSubject)
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
bool COEAddin::GetTitle(HWND hwndCompose, CString &szSubject)
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

// ====================================================
//	GetRebarTopParent - determines if rebar belongs
//						to OE's browser, ATH_Note (oe compose)
//						or IEFrame (Hotmail compose)
//						class.
// ====================================================
UINT COEAddin::GetRebarTopParent(HWND hRebar, HWND& hTopParent)
{
	CFuncLog log(g_pLog, "COEAddin::GetRebarTopParent()");
	UINT Owner = 0;
	if (::IsWindow(hRebar))
	{
		CWindow TempWindow;
		TempWindow.Attach(hRebar);
		HWND hParent = TempWindow.GetParent();
		hTopParent = TempWindow.GetTopLevelParent();
		TempWindow.Detach();

		TCHAR szClass[40];

		// Only interested in rebars that are children
		// of Sizable Rebars or WorkerW rebars (IE).
		if (hParent)
		{
			::GetClassName(hParent, szClass, sizeof(szClass) - 1);
			if (_tcscmp(szClass, OE_SIZABLEREBARWND_CLASS))
			{
				if (_tcscmp(szClass, IE_WORKERW_CLASS))
					return 0;
			}
		}

		// Now that we checked pedigree, let's find the top parent.
		if (hTopParent)
		{
			::GetClassName(hTopParent, szClass, sizeof(szClass) - 1);
			if(!_tcscmp(szClass, OE_BROWSER_CLASS))
			{
				Owner = OWNER_OEBROWSER;
			}
			else if (!_tcscmp(szClass, OE_COMPOSEWINDOW_CLASS))
			{
				Owner = OWNER_COMPOSE;
			}
			else if (!_tcscmp(szClass, IE_IEFRAME_CLASS))
			{
				Owner = OWNER_IEFRAME;
			}
		}
	}

	return Owner;
}

//=============================================================================
//	GetToolbarFromRebar
//=============================================================================
BOOL COEAddin::GetToolbarFromRebar(HWND hwndRebar, OEInfoType* pToolbarInfo)
{
	CFuncLog log(g_pLog, "COEAddin::GetToolbarFromRebar()");
	if (!hwndRebar)
		return false;
	if (m_mapOEInfo.empty())
		return false;

	OEInfoMapType::iterator itr;
	itr = m_mapOEInfo.find(hwndRebar);
	if (itr == m_mapOEInfo.end())
		return false;

	*pToolbarInfo = *(itr->second);
	return true;
}

//=============================================================================
//	GetToolbarFromHandle
//=============================================================================
BOOL COEAddin::GetToolbarFromHandle(HWND hwndToolbar, OEInfoType* pToolbarInfo)
{
	CFuncLog log(g_pLog, "COEAddin::GetToolbarFromHandle()");
	OEInfoMapType::iterator itr;
	itr = m_mapOEInfo.begin();

	bool bFound = false;
	while (!bFound && itr != m_mapOEInfo.end())
	{
		if (((itr->second)->pToolbar->m_hWnd) == hwndToolbar)
		{
			*pToolbarInfo = *(itr->second);
			bFound = true;
		}
		itr++;
	}
	return bFound;
}

//=============================================================================
//	GetToolbarFromTopParent
//=============================================================================
BOOL COEAddin::GetToolbarFromTopParent(HWND hwndTopParent, OEInfoType* pToolbarInfo)
{
	CFuncLog log(g_pLog, "COEAddin::GetToolbarFromTopParent()");
	OEInfoMapType::iterator itr;
	itr = m_mapOEInfo.begin();

	bool bFound = false;
	while(!bFound && itr != m_mapOEInfo.end())
	{
		if (((itr->second)->hTopParent) == hwndTopParent)
		{
			*pToolbarInfo = *(itr->second);
			bFound = true;
		}
		itr++;
	}
	return bFound;
}

//=============================================================================
//	EraseAll
//=============================================================================
void COEAddin::EraseAll()
{
	CFuncLog log(g_pLog, "COEAddin::EraseAll()");
	// Remove all toolbars then clear map.
	OEInfoMapType::iterator itr;
	itr = m_mapOEInfo.begin();

	BOOL bError = false;
	while(!m_mapOEInfo.empty() && itr != m_mapOEInfo.end() && !bError)
	{
		bError = RemoveEntry(itr->first);
		itr++;
	}
		
	m_mapOEInfo.clear();
}

//=============================================================================
//	HideAll
//=============================================================================
void COEAddin::HideAll()
{
	CFuncLog log(g_pLog, "COEAddin::HideAll()");
	// HIDE all toolbars. 
	OEInfoMapType::iterator itr;
	itr = m_mapOEInfo.begin();

	while(!m_mapOEInfo.empty() && itr != m_mapOEInfo.end())
	{
		(itr->second)->pToolbar->ShowBand(FALSE);
		itr++;
	}
}

//=============================================================================
//	RemoveEntry
//=============================================================================
BOOL COEAddin::RemoveEntry(HWND hwndRebar)
{
	CFuncLog log(g_pLog, "COEAddin::RemoveEntry()");
	// Look up hwndRebar in the OE info map. If found delete the toolbar.
	if (!hwndRebar)
		return false;

	OEInfoMapType::iterator itr;
	itr = m_mapOEInfo.find(hwndRebar);

	if (itr == m_mapOEInfo.end())
		return false;

	_ASSERTE(NULL != (itr->second)->pToolbar);

	
	delete ((itr->second)->pToolbar);
	delete (itr->second);
	m_mapOEInfo.erase(itr);
	return true;
}

//================================================
// CreateToolbar
//================================================
BOOL COEAddin::CreateToolbar(HWND hwndParent, HWND hwndRebar, UINT OwnerClass)
{
	CFuncLog log(g_pLog, "COEAddin::CreateToolbar()");
	// Look up hwndRebar in the OE info map to see 
	// if there is a JMToolbar associated with this 
	// Rebar. If not create one.

	OEInfoMapType::iterator itrOEInfo;
	itrOEInfo = m_mapOEInfo.find(hwndRebar);

	if (itrOEInfo != m_mapOEInfo.end())
		return false;

	OEInfoType* pOEInfo = new OEInfoType;
	pOEInfo->hTopParent = hwndParent;
	pOEInfo->pToolbar =  new CJMToolbar();
	pOEInfo->uStatus = TOOLBAR_INIT;

	if (!pOEInfo->pToolbar->CreateJMToolbar(hwndParent, hwndRebar, OwnerClass))
	{
		delete pOEInfo->pToolbar;
		delete pOEInfo;
		pOEInfo->pToolbar = NULL;
		return false;
	}

	m_mapOEInfo[hwndRebar] = pOEInfo;
	if (OwnerClass == OWNER_COMPOSE)
	{
		m_hActiveCompose = pOEInfo->hTopParent;
	}

	return true;
}

//================================================
// OnOERebar
//================================================
// This handler is called every time a Rebar for OE Browser 
// OE Compose, or IE window is created.
LRESULT  COEAddin::OnOERebar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hRebar = (HWND)lParam;
	if (!::IsWindow(hRebar))
		return 0;

	CFuncLog log(g_pLog, "COEAddin::OnOERebar()");

	HWND hTopParent = NULL;
	UINT uOwnerClass = GetRebarTopParent(hRebar, hTopParent);
	if (uOwnerClass)
	{	
		// Create and add Toolbar to OE Browser, OE Compose or 
		// IE compose window.
		CreateToolbar(hTopParent, hRebar, uOwnerClass);
	}

	return 0;
}

//================================================
// OnIEServer
//================================================
// This handler is called every time an Internet Explorer_Server window is created.
LRESULT COEAddin::OnIEServer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hwndIEServer = (HWND)lParam;
	if (!::IsWindow(hwndIEServer))
		return 0;

	CFuncLog log(g_pLog, "COEAddin::OnIEServer()");

	CWindow TempWindow;
	TempWindow.Attach(hwndIEServer);
	HWND hwndCompose = TempWindow.GetTopLevelParent();
	if (!hwndCompose)
		return 0;

	TCHAR szClass[100];
	::GetClassName(hwndCompose, szClass, sizeof(szClass) - 1);
	TempWindow.Detach();

	// Handle Hotmail  - commented out until hotmail/yahoo mail
	// solution given go ahead.
	/*if (0 == _tcscmp(szClass, IE_IEFRAME_CLASS))
	{
		SetTimer(IDT_IELOAD_TIMER, 500, NULL);
		m_hActiveIE = hwndCompose;
	}*/

	if (_tcscmp(szClass, OE_COMPOSEWINDOW_CLASS))
			return 0;


	SetTimer(IDT_IELOAD_TIMER, 300, NULL);
	m_hActiveIE = hwndCompose;
	
	return 0;
}

//================================================
// OnTBDestroy
//================================================
LRESULT  COEAddin::OnTBDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "COEAddin::OnTBDestroy()");

	HWND hwnd = (HWND)lParam;
	UINT uType = (UINT)wParam;
	OEInfoType Info;
	
	HandleOnItemClose();

	BOOL bFound = false;
	if (uType == TYPE_REBAR)
	{
		bFound = GetToolbarFromRebar(hwnd, &Info);
	}
	else
	if (uType == TYPE_TOOLBAR)
	{
		// Remove toolbar only if it is not owned by OE Browser window.
		// OE Browser's toolbar is removed as a result of a WM_CLOSE.
		// This is due to a Window XP bug/feature. 
		if (GetToolbarFromHandle(hwnd, &Info))
		{
			UINT uOwner = Info.pToolbar->GetOwnerClass();
			if (uOwner != OWNER_OEBROWSER && uOwner != OWNER_COMPOSE)
				bFound = true;
		}
	}
	else
	if (uType == TYPE_TOPPARENT)
	{
		bFound = GetToolbarFromTopParent(hwnd, &Info);
	}
	
	if (bFound)
	{
		UINT uOwner = Info.pToolbar->GetOwnerClass();
		if (uOwner == OWNER_COMPOSE)
		{
			// Shutdown document handling
			IHTMLDocument2Ptr spDoc = GetHTMLDocument2(Info.hTopParent);
			DeactivateJazzyMailDocument(spDoc);
		}

		// Remove toolbar and info since window is being destroyed.
		bHandled = TRUE;
		HWND hwndRebar = Info.pToolbar->GetRebarHandle();
		if (hwndRebar)
			RemoveEntry(hwndRebar);
	}

	return 0;
}

//================================================
// OnOERemoveToolbars
//================================================
LRESULT  COEAddin::OnOERemoveToolbars(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "COEAddin::OnOERemoveToolbars()"); 

	// LParam is the hwnd to the rebar containing the toolbar.
	// If hwndRebar is NULL then all toolbars are removed.
	HWND hwndRebar = (HWND)lParam;
	if (hwndRebar)
		RemoveEntry(hwndRebar);
	else
		CleanUp();

	return 0;
}

//================================================
// OnMoveBand
//================================================
LRESULT  COEAddin::OnMoveBand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "COEAddin::OnMoveBand()"); 
	HWND hwndRebar = (HWND)lParam;
	OEInfoType Info;		
	if (GetToolbarFromRebar(hwndRebar, &Info))
	{
		Info.pToolbar->MoveToLastBand(); 
		bHandled = true;
	}

	return 0;
}

//================================================
// OnRefresh
//================================================
LRESULT COEAddin::OnRefresh(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hwndSysTab = (HWND)lParam;

	if (!::IsWindow(hwndSysTab))
		return 0;

	CFuncLog log(g_pLog, "COEAddin::OnRefresh()"); 

	// Find Compose window - should be top parent.
	CWindow TempWindow;
	TempWindow.Attach(hwndSysTab);
	HWND hwndCompose = TempWindow.GetTopLevelParent();
	TempWindow.Detach();
	
	// Setup document handling
	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(hwndCompose);
	m_hRefreshCompose = hwndCompose;

	// Allow time for doc to load.
	if (!m_bRefreshTimerInUse)
	{
		SetTimer(IDT_REFRESH_TIMER, 300, NULL);
		m_bRefreshTimerInUse = true;
	}


	return 0;
}

//=============================================================================
//	OnHotmailCompose
//=============================================================================
LRESULT COEAddin::OnHotmailCompose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	IDispatchPtr spDisp = (IDispatch*)lParam;
	IHTMLDocument2Ptr spDoc(spDisp);
	if (spDoc == NULL)
		return 0;

	if (m_bWaitingForCompose)
	{
		WriteHtmlToDocument(CComBSTR(m_strHTMLText), spDoc);
		m_strHTMLText.Empty();
		m_bWaitingForCompose = false;
	}
	return 0;
}

//=============================================================================
//	OnShutdown
//=============================================================================
LRESULT COEAddin::OnForceShutdown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CFuncLog log(g_pLog, "COEAddin::OnForceShutdown()");
	HideAll();
	HandleShutdown();
	return S_OK;
}
//=============================================================================
//	OnShutdownAll
//=============================================================================
LRESULT COEAddin::OnShutdownAll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bSrvrShutdown = true;
	HandleShutdown();
	return S_OK;
}
//=============================================================================
//	OnUpdateResource
//=============================================================================
LRESULT COEAddin::OnUpdateResource(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	AutoUpdateDone(UPDATE_RESOURCES);
	return S_OK;
}
//=============================================================================
//	OnRemoveAddin
//=============================================================================
LRESULT COEAddin::OnRemoveAddin(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_Authenticate.IsExpressEnabled())
		return S_OK;

	m_bSrvrShutdown = false;
	HideAll();
	HandleShutdown();
	return S_OK;
}
//=============================================================================
//	OnHelp
//=============================================================================
LRESULT COEAddin::OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CFuncLog log(g_pLog, "COEAddin::OnHelp()");
	ShowHelp();
	return 0;
}
//=============================================================================
//	OnTimer
//=============================================================================
LRESULT COEAddin::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CFuncLog log(g_pLog, "COEAddin::OnTimer()");
	if (IDT_IELOAD_TIMER == wParam) 
	{
		if (!::IsWindow(m_hActiveIE))
			return 0;
		
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
		bool bNewMessage = (szTitle.CompareNoCase("New Message") == 0);
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
	else if (IDT_REFRESH_TIMER == wParam) 
	{
		IHTMLDocument2Ptr spDoc = GetHTMLDocument2(m_hRefreshCompose);
		if (!IsDocLoaded(spDoc))
				return 0;
	
		// Set Play Param for all non-wsShell flash types.
		SetFlashPlayParam(spDoc);

		// Setup document handling
		ActivateJazzyMailDocument(spDoc, false/*bSetAttribute*/);

		KillTimer(IDT_REFRESH_TIMER); 
		m_bRefreshTimerInUse = false;
	}

	return 0;
}

//=============================================================================
//	OnOEButtonClick
//=============================================================================
STDMETHODIMP COEAddin::OnOEButtonClick(ULONG ulButton_ID, OLE_HANDLE hwndRebar, VARIANT_BOOL* bSuccess)
{
	*bSuccess = true;
	CFuncLog log(g_pLog, "COEAddin::OnOEButtonClick()");
	switch (ulButton_ID)
	{
		case ID_BTN1_CMD:
			Popup(POPUP_DIALOG, 0, (HWND)hwndRebar, _T("?type=sta"));
			break;

		case ID_BTN2_CMD:
			Popup(POPUP_DIALOG, 1, (HWND)hwndRebar, _T("?type=smi"));
			break;

		case ID_BTN3_CMD:
			Popup(POPUP_DIALOG, 2, (HWND)hwndRebar, _T("?type=clp"));
			break;

		case ID_BTN4_CMD:
			Popup(POPUP_DIALOG, 3, (HWND)hwndRebar, _T("?type=pho"));
			break;

		case ID_BTN5_CMD:
			Popup(POPUP_DIALOG, 4, (HWND)hwndRebar, _T("?type=inv"));
			break;

		case ID_BTN6_CMD:
			Popup(POPUP_DIALOG, 5, (HWND)hwndRebar, _T("?type=ann"));
			break;

		case ID_BTN7_CMD:
			Popup(POPUP_DIALOG, 6, (HWND)hwndRebar, _T("?type=snd"));
			break;

		case ID_BTN8_CMD:
			Popup(POPUP_DIALOG, 7, (HWND)hwndRebar, _T("?type=fav"));
			break;

		case ID_BTN9_CMD:
			HandleECardsButton();
			break;

		case ID_BTN10_CMD:
			HandleGamesButton();
			break;

		case ID_BTN11_CMD:
			HandleOptionsButton();
			break;

		default:
			*bSuccess = false;
			break;
	}
	
	return S_OK;
}

//=============================================================================
//	OnItemSend
//=============================================================================
STDMETHODIMP COEAddin::OnItemSend()
{	
	CFuncLog log(g_pLog, "COEAddin::OnItemSend()");

	// Make sure this is a JazzMail document.
	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(m_hActiveCompose);
	HandleOnItemSend(spDoc);
	return S_OK;
}

//=============================================================================
//	OnShutDown
//=============================================================================
STDMETHODIMP COEAddin::OnShutDown()
{	
	CFuncLog log(g_pLog, "COEAddin::OnShutDown()");
	HandleShutdown();
	return S_OK;
}

//=============================================================================
//	Download
//=============================================================================
STDMETHODIMP COEAddin::Download(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice, VARIANT varExternalLink)
{
	CFuncLog log(g_pLog, "COEAddin::Download()");
	return HandleDownloadRequest(varProductId, varName, varDownload, varFlags, varWidth, varHeight, varFlashPath, varPathPrice, varExternalLink);
}

//=============================================================================
//	Download2
//=============================================================================
STDMETHODIMP COEAddin::Download2(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath,  VARIANT varPathPrice)
{
	CFuncLog log(g_pLog, "COEAddin::Download2()");
	return HandleDownloadRequest(varProductId, varName, varDownload, varFlags, varWidth, varHeight, varFlashPath, varPathPrice, CComVariant(CComBSTR("")));
}

//=============================================================================
//	RemoveToolbar
//=============================================================================
STDMETHODIMP COEAddin::RemoveToolbar(OLE_HANDLE hwndRebar, VARIANT_BOOL* bSuccess)
{
	CFuncLog log(g_pLog, "COEAddin::RemoveToolbar()");
	*bSuccess = RemoveEntry((HWND) hwndRebar);
	return S_OK;
}

//=============================================================================
//	GetToolbar
//=============================================================================
STDMETHODIMP COEAddin::GetToolbar(OLE_HANDLE hwndRebar, LONG* ToolbarInfo, VARIANT_BOOL* bSuccess)
{
	CFuncLog log(g_pLog, "COEAddin::GetToolbar()");
	*bSuccess = GetToolbarFromRebar((HWND)hwndRebar, (OEInfoType*)ToolbarInfo);
	return S_OK;
}

//=============================================================================
//	GetWindowHandle
//=============================================================================
STDMETHODIMP COEAddin::GetWindowHandle(OLE_HANDLE* hWnd)
{
	CFuncLog log(g_pLog, "COEAddin::GetWindowHandle()");
	*hWnd = (OLE_HANDLE)m_hWnd;
	return S_OK;
}

//=============================================================================
//	SetActiveCompose
//=============================================================================
STDMETHODIMP COEAddin::SetActiveCompose(OLE_HANDLE hwndCompose, UINT State)
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
STDMETHODIMP COEAddin::ExecuteCommand(VARIANT varCmdId)
{
	CFuncLog log(g_pLog, "COEAddin::ExecuteCommand()");
	
	ProcessExecuteCommand(varCmdId.intVal);

	return S_OK;
}
