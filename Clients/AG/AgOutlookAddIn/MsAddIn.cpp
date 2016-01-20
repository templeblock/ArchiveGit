#include "stdafx.h"
#include "MsAddIn.h"
#include "WindowSubclass.h"
#include "Utility.h"
#include "HelperFunctions.h"

static UINT IDT_IELOAD_TIMER	= ::RegisterWindowMessage("Creata Mail IE Load Timer");
#define TOOLBAR_NAME "Creata Mail"

/////////////////////////////////////////////////////////////////////////////
// CMsAddIn
_ATL_FUNC_INFO OnButtonClickInfo		= {CC_STDCALL, VT_EMPTY, 2, {VT_DISPATCH,VT_BYREF | VT_BOOL}};
_ATL_FUNC_INFO OnAppItemSendInfo		= {CC_STDCALL, VT_EMPTY, 2, {VT_DISPATCH,VT_BYREF | VT_BOOL}};
_ATL_FUNC_INFO OnNewInspectorInfo		= {CC_STDCALL, VT_EMPTY, 1, {VT_DISPATCH}};
//j_ATL_FUNC_INFO OnAppNewMailInfo		= {CC_STDCALL, VT_EMPTY, 0, {VT_NULL}};

/////////////////////////////////////////////////////////////////////////////
CMsAddIn::CMsAddIn()
{
	SetClient(CLIENT_TYPE_OUTLOOK);
	SetExpress(false);
	m_bConnected = false;
	m_dwButtonEventCookie = 0xFEFEFEFE;
	m_Download.SetAppInfo(this->m_spClientSite, CLIENT_TYPE_OUTLOOK);
	m_PointLastClick.x = 0;
	m_PointLastClick.y = 0;
	m_dwTimeLastClick = 0;
	m_Usage.SetMethod(METH_OUTLOOK);
}

/////////////////////////////////////////////////////////////////////////////
CMsAddIn::~CMsAddIn()
{
	if (IsWindow())
		DestroyWindow();
#ifdef _DEBUG
	::MessageBox(NULL, "CMsAddIn Destroyed", g_szAppName, MB_OK);
#endif _DEBUG
}

// _IDTExtensibility2
/////////////////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE CMsAddIn::OnConnection(IDispatch* Application, ext_ConnectMode ConnectMode, IDispatch* AddInInst, SAFEARRAY** custom)
{
	CFuncLog log(g_pLog, "CMsAddin::OnConnection()");
	m_pOutlook = (Outlook::_ApplicationPtr)Application;
	if (m_pOutlook == NULL)
		return E_FAIL;
	Outlook::_ExplorerPtr spExplorer = GetActiveExplorer();
	if (spExplorer == NULL)
		return E_FAIL;

	// get the CommandBars interface that represents Outlook's toolbars & menu items
	Office::_CommandBarsPtr spCmdBars = spExplorer->GetCommandBars();
	if (spCmdBars == NULL)
		return E_FAIL;

	if (m_ToolbarList.size() != 0)
		m_ToolbarList.clear();

	// Create the main toolbar
	if (!ToolbarCreate(spCmdBars, TOOLBAR_NAME, NULL/*Outlook::_InspectorPtr*/))
		return E_FAIL;

	m_bConnected = true;
	
	m_pInspectors = m_pOutlook->GetInspectors();

	// Setup the event sinks for the application
	HRESULT hr = S_OK;
	hr = AppItemSendEvents::DispEventAdvise((IDispatch*)m_pOutlook);
	hr = InspectorsEvents::DispEventAdvise((IDispatch*)m_pInspectors);
//j	hr = AppNewMailEvents::DispEventAdvise((IDispatch*)m_pOutlook);

	ShowTrayIcon();
	VerifyUserStatus();
	
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE CMsAddIn::OnAddInsUpdate(SAFEARRAY** custom)
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE CMsAddIn::OnStartupComplete(SAFEARRAY** custom)
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE CMsAddIn::OnBeginShutdown(SAFEARRAY** custom)
{
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE CMsAddIn::OnDisconnection(ext_DisconnectMode RemoveMode, SAFEARRAY** custom)
{
	CFuncLog log(g_pLog, "CMsAddin::OnDisconnection()");

#ifdef _DEBUG
	::MessageBox(NULL, "OnDisconnection", g_szAppName, MB_OK);
#endif _DEBUG

	HandleShutdown();

	if (!m_bConnected)
		return S_OK;

	// Tear down the event sinks for the application
	HRESULT hr = S_OK;
	hr = AppItemSendEvents::DispEventUnadvise((IDispatch*)m_pOutlook);
	hr = InspectorsEvents::DispEventUnadvise((IDispatch*)m_pInspectors);
//j	hr = AppNewMailEvents::DispEventUnadvise((IDispatch*)m_pOutlook);

	if (m_ToolbarList.size() != 0)
		m_ToolbarList.clear();

	m_bConnected = false;

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddIn::Popup(int iType, Office::_CommandBarButton* Ctrl, LPCSTR pQueryString)
{
	CFuncLog log(g_pLog, "CMsAddin::Popup()");

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

	CComQIPtr<IMsAddIn> pAddIn = this;
	CComQIPtr<IDispatch> pDispatch = pAddIn;

	if (iType == POPUP_DIALOG)
	{
		//HWND hWndParent = ::GetDesktopWindow();
		m_HTMLDialog.Popup(m_hTopParent, pDispatch, strURL);
		m_AutoUpdate.Start(true/*bSilent*/);
	}
	else
	if (iType == POPUP_MENU)
	{
		Office::_CommandBarButtonPtr spCmdButton(Ctrl); // the button that raised the event
		POINT Point = { spCmdButton->GetLeft(), spCmdButton->GetTop() };
		HWND hWndParent = ::WindowFromPoint(Point);
		Point.y += spCmdButton->GetHeight();
		m_HTMLMenu.Popup(hWndParent, pDispatch, Point.x, Point.y, strURL);
	}
}

/////////////////////////////////////////////////////////////////////////////
void STDMETHODCALLTYPE CMsAddIn::OnButtonClick(Office::_CommandBarButton* Ctrl, VARIANT_BOOL* CancelDefault)
{
	CFuncLog log(g_pLog, "CMsAddin::OnButtonClick()");
	Office::_CommandBarButtonPtr spCmdButton(Ctrl); // the button that raised the event

	// Note:  Even though only one button was clicked, this function is called once for each toolbar
	// Since there is now way to differentiate one from the other, we will have look at the cursor point
	// and bail if it is exactly the same as the last call
	POINT Point;
	::GetCursorPos(&Point);
	RECT Rect = {0,0,0,0};
	try { // This could fail if the button is not visible
	Rect.left   = spCmdButton->GetLeft();
	Rect.top    = spCmdButton->GetTop();
	Rect.right  = Rect.left + spCmdButton->GetWidth();
	Rect.bottom = Rect.top + spCmdButton->GetHeight();
	} catch (...) { // So default the button rect to the click point
	Rect.left   = Point.x;
	Rect.top    = Point.y;
	Rect.right  = Point.x+1;
	Rect.bottom = Point.y+1;
	}

	bool bCursorOutsideButton = !::PtInRect(&Rect, Point);
	bool bLastClickIsOld = (::timeGetTime() - m_dwTimeLastClick > 1000/*1 second*/);
	bool bPointIsSame = (m_PointLastClick.x == Point.x && m_PointLastClick.y == Point.y);
	bool bBail = (bCursorOutsideButton || (bPointIsSame && !bLastClickIsOld));
	if (bBail)
		return;

	POINT Point2 = { spCmdButton->GetLeft(), spCmdButton->GetTop() };
	HWND hWndCommandBar = ::WindowFromPoint(Point2);
	CWindow TempWindow;
	TempWindow.Attach(hWndCommandBar);
	m_hTopParent = TempWindow.GetTopLevelParent();
	TempWindow.Detach();
//j	HWND hWndCommandBarDock = ::GetParent(hWndCommandBar);
//j	HWND hWndMyExplorer = ::GetParent(hWndCommandBarDock);
//j
//j	Outlook::_InspectorPtr pInspector = GetActiveInspector();
//j	IOleWindowPtr pOleWindow = pInspector;
//j	if (pOleWindow == NULL)
//j	{
//j		Outlook::_ExplorerPtr spExplorer = GetActiveExplorer();
//j		pOleWindow = spExplorer;
//j	}
//j
//j	HWND hWnd = NULL;
//j	if (pOleWindow)
//j		pOleWindow->GetWindow(&hWnd);
//j		
//j	if (hWnd != hWndMyExplorer)
//j		return;

	int index = spCmdButton->GetIndex();
	CComBSTR strTag = spCmdButton->GetTag().GetBSTR();

	if (strTag == "1")
		Popup(POPUP_DIALOG, Ctrl, "?type=sta");
	else
	if (strTag == "2")
		Popup(POPUP_DIALOG, Ctrl, "?type=smi");
	else
	if (strTag == "3")
		Popup(POPUP_DIALOG, Ctrl, "?type=clp");
	else
	if (strTag == "4")
		Popup(POPUP_DIALOG, Ctrl, "?type=pho");
	else
	if (strTag == "5")
		Popup(POPUP_DIALOG, Ctrl, "?type=inv");
	else
	if (strTag == "6")
		Popup(POPUP_DIALOG, Ctrl, "?type=ann");
	else
	if (strTag == "7")
		Popup(POPUP_DIALOG, Ctrl, "?type=snd");
	else
	if (strTag == "8")
		Popup(POPUP_DIALOG, Ctrl, "?type=fav");
	else
	if (strTag == "9")
		HandleECardsButton();
	else if (strTag == "10")
		HandleGamesButton();
	else if (strTag == "11")
		HandleOptionsButton();

	m_dwTimeLastClick = ::timeGetTime();
	::GetCursorPos(&m_PointLastClick);
	*CancelDefault = VARIANT_TRUE;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddIn::ToolbarCreate(Office::_CommandBarsPtr& pCommandBars, LPCSTR pName, Outlook::_InspectorPtr pInspector)
{
	CFuncLog log(g_pLog, "CMsAddin::ToolbarCreate()");
	// Add a new toolband to Outlook to which we'll add our buttons; position it below all toolbands 
	CComPtr<Office::CommandBar> spNewCmdBar;
	try
	{
		spNewCmdBar = pCommandBars->Add(CComVariant(pName), CComVariant(Office::msoBarTop)/*vPos*/, CComVariant(VARIANT_FALSE)/*vMenuBar*/, CComVariant(VARIANT_TRUE)/*vTemp*/);
	}
	catch (...)
	{
	}

	if (spNewCmdBar == NULL)
		return false;


	CToolbar* pToolbar = new CToolbar(pInspector);
	if (!pToolbar)
		return false;

	m_ToolbarList.push_back(pToolbar);

	// Get the toolband's CommandBarControls
	Office::CommandBarControlsPtr spBarControls = spNewCmdBar->GetControls();

	for (int i=0; i<NUM_OF_BTNS; i++)
	{
		if (i== 7) //j Skip the Favorties button for now
			continue;

		Office::_CommandBarButtonPtr spCmdButton = ToolbarCreateButton(spBarControls, i);
		if (spCmdButton)
		{
			pToolbar->AddButton(spCmdButton);

			// Setup the event sinks for the button
			ButtonClickEvents::m_dwEventCookie = 0xFEFEFEFE;
			HRESULT hr = ButtonClickEvents::DispEventAdvise((IDispatch*)spCmdButton);
			m_dwButtonEventCookie = ButtonClickEvents::m_dwEventCookie; // save the event cookie
		}
	}

	spNewCmdBar->PutVisible(VARIANT_TRUE); 

	// After the toolbar is made visible, find the window handle so we can subclass it
	IOleWindowPtr pOleWindow = pInspector;
	if (pOleWindow == NULL)
	{
		Outlook::_ExplorerPtr spExplorer = GetActiveExplorer();
		pOleWindow = spExplorer;
	}
	
	HWND hWnd = NULL;
	if (pOleWindow)
		pOleWindow->GetWindow(&hWnd);

//j	if (!hWnd)
//j		hWnd = GetChildWindow(hWnd, NULL, pName);
	
	if (hWnd)
	{
		CWindowSubclass* pWnd = new CWindowSubclass();
		SUBCLASSCALLBACK pCallBack = (SUBCLASSCALLBACK)CMsAddIn::MyItemClose;
		LPARAM lParam1 = (LPARAM)(void*)this;
		LPARAM lParam2 = (LPARAM)(void*)pInspector;
		pWnd->InitDialog(hWnd, pCallBack, lParam1, lParam2);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
Office::_CommandBarButtonPtr CMsAddIn::ToolbarCreateButton(Office::CommandBarControlsPtr& pCommandBar, int index)
{
	CFuncLog log(g_pLog, "CMsAddin::ToolbarCreateButton()");
	CComVariant varEmpty(DISP_E_PARAMNOTFOUND, VT_ERROR);
	Office::CommandBarControlPtr spCmdControl = pCommandBar->Add(
		CComVariant(Office::msoControlButton)/*vType*/, varEmpty/*vBuiltInId*/, varEmpty/*vParameter*/, varEmpty/*vBefore*/, CComVariant(VARIANT_TRUE)/*vTemporary*/); 

	UINT idBitmap = IDB_BTN1 + index;
	CComBSTR strCaption;
	strCaption.LoadString(IDS_BTN1 + index);
	CComBSTR strToolTip;
	strToolTip.LoadString(IDS_BTN1_TIP + index);
	CString strTag;
	strTag.Format("%d", index + 1);

	Office::_CommandBarButtonPtr spCmdButton = spCmdControl;
	spCmdButton->PutStyle(Office::msoButtonIconAndCaption);
	spCmdButton->PutCaption((BSTR)strCaption); 
	spCmdButton->PutEnabled(VARIANT_TRUE);
	spCmdButton->PutTooltipText((BSTR)strToolTip); 
	spCmdButton->PutTag((BSTR)CComBSTR(strTag)); 
	spCmdButton->PutVisible(VARIANT_TRUE);

	HBITMAP hBitmap2 = (HBITMAP)::LoadImage(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(idBitmap), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
	HBITMAP hBitmap = ReplaceBitmapColor(hBitmap2, CLR_DEFAULT, ::GetSysColor(COLOR_BTNFACE));
	if (hBitmap)
		::DeleteObject(hBitmap2);
	else
		hBitmap = hBitmap2;

	if (!hBitmap)
		return spCmdButton;

	HBITMAP hSavedBitmap = NULL;
	HANDLE hSavedData = ::GetClipboardData(CF_BITMAP);
	if (hSavedData)
		hSavedBitmap = (HBITMAP)::CopyImage(hSavedData, IMAGE_BITMAP, 0/*cx*/, 0/*cy*/, 0/*uiFlags*/);

	// Put the bitmap on the clipboard
	::OpenClipboard(NULL);
	::SetClipboardData(CF_BITMAP, (HANDLE)hBitmap);
	::CloseClipboard();

	try { // This could fail if there is anything wrong with the bitmap on the clipboard
		HRESULT hr = spCmdButton->PasteFace();
	} catch (...) {
	}

	// Restore the original clipboard
	::OpenClipboard(NULL);
	::SetClipboardData(CF_BITMAP, hSavedBitmap);
	::CloseClipboard();

	return spCmdButton;
}

/////////////////////////////////////////////////////////////////////////////
CToolbar* CMsAddIn::ToolbarFind(void* pInspector)
{
	CFuncLog log(g_pLog, "CMsAddin::ToolbarFind()");
	int iSize = m_ToolbarList.size();
	for (int i=0; i<iSize; i++)
	{
		CToolbar* pToolbar = m_ToolbarList[i];
		if (pToolbar && pToolbar->GetInspector() == pInspector)
			return pToolbar;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddIn::ToolbarDestroy(void* pInspector)
{
	CFuncLog log(g_pLog, "CMsAddin::ToolbarDestroy()");
	CToolbar* pToolbar = ToolbarFind(pInspector);
	if (!pToolbar)
		return;

	// Tear down the event sinks for the buttons
	UINT nSize = pToolbar->NumButtons();
	for (UINT nIndex = 0; nIndex < nSize; nIndex++)
	{
		Office::_CommandBarButtonPtr spCmdButton = pToolbar->GetButton(nIndex);
		if (spCmdButton != NULL)
		{
			ButtonClickEvents::m_dwEventCookie = m_dwButtonEventCookie; // restore the event cookie
			HRESULT hr = ButtonClickEvents::DispEventUnadvise((IDispatch*)spCmdButton);
		}
	}

	// Remove the toolbar from the list
	int iSize = m_ToolbarList.size();
	for (int i=0; i<iSize; i++)
	{
		if (pToolbar == m_ToolbarList[i])
		{
			m_ToolbarList.erase(m_ToolbarList.begin() + i);
			break;
		}
	}

	delete pToolbar;
}
/////////////////////////////////////////////////////////////////////////////
bool CMsAddIn::ShowInspectorToolbars(bool bShow)
{
	CComPtr<Outlook::_Inspectors> spInspectors = m_pOutlook->GetInspectors();
	if (spInspectors == NULL)
		return false;

	int iSize = spInspectors->GetCount();
	bool bSuccess = false;
	for (int i=1; i<=iSize; i++)
	{
		
		CComPtr<Outlook::_Inspector> spInspector = spInspectors->Item(CComVariant(i));
		if (spInspector == NULL)
			break;

		CComPtr<Office::_CommandBars> spCmdBars = spInspector->GetCommandBars();
		if (spCmdBars == NULL)
			break;

		int iCount = spCmdBars->GetCount();
		for (int j=1; j<=iCount; j++)
		{
			CComPtr<Office::CommandBar> spCmdBar = spCmdBars->GetItem(CComVariant(j));
			CComBSTR Name = (BSTR)spCmdBar->GetName();
			if (Name == TOOLBAR_NAME)
			{
				spCmdBar->PutVisible((VARIANT_BOOL)bShow);
				bSuccess = true;
			}
		}		
	}

	return bSuccess;
}
/////////////////////////////////////////////////////////////////////////////
bool CMsAddIn::ShowExplorerToolbars(bool bShow)
{
	CComPtr<Outlook::_Explorers> spExplorers = m_pOutlook->GetExplorers();
	if (spExplorers == NULL)
		return false;

	int iSize = spExplorers->GetCount();
	bool bSuccess = false;
	for (int i=1; i<=iSize; i++)
	{
		CComPtr<Outlook::_Explorer> spExplorer = spExplorers->Item(CComVariant(i));
		if (spExplorer == NULL)
			break;

		CComPtr<Office::_CommandBars> spCmdBars = spExplorer->GetCommandBars();
		if (spCmdBars == NULL)
			break;

		int iCount = spCmdBars->GetCount();
		for (int j=1; j<=iCount; j++)
		{
			CComPtr<Office::CommandBar> spCmdBar = spCmdBars->GetItem(CComVariant(j));
			CComBSTR Name = (BSTR)spCmdBar->GetName();
			if (Name == TOOLBAR_NAME)
			{
				spCmdBar->PutVisible((VARIANT_BOOL)bShow);
				bSuccess = true;
			}
		}
		
	}

	return bSuccess;
}
/////////////////////////////////////////////////////////////////////////////
void STDMETHODCALLTYPE CMsAddIn::OnNewInspector(_Inspector* Inspector)
{
	CFuncLog log(g_pLog, "CMsAddin::OnNewInspector()");

	if (m_Security.FixupEmailEditor(true/*bSilent*/, false/*bAllowChange*/))
		return;

	Outlook::_InspectorPtr pInspector(Inspector);
	Outlook::_MailItemPtr spMailItem = GetMailItem(pInspector);
	if (pInspector == NULL || spMailItem == NULL)
		return;
	
	Office::_CommandBarsPtr spCmdBars = pInspector->GetCommandBars();
	if (spCmdBars == NULL)
		return;

	bool bFound = false;
	int iCount = spCmdBars->GetCount();
	for (int i=1; i<=iCount; i++)
	{
		Office::CommandBarPtr spCmdBar = spCmdBars->GetItem(CComVariant(i));
		CComBSTR Name = (BSTR)spCmdBar->GetName();
		if (Name == TOOLBAR_NAME)
		{
			bFound = true;
			log.LogString(LOG_INFO, log.FormatString(" if (Name == TOOLBAR_NAME):  iCount== %d", iCount));
			break;
		}
	}
	
	if (!bFound)
	{
		// Create the compose window toolbar
		if (!ToolbarCreate(spCmdBars, TOOLBAR_NAME, pInspector))
		{
			log.LogString(LOG_ERROR, "ToolbarCreate FAILURE");
			return;
		}
	}

	if (IsInspectorInEditMode(pInspector))
	{
		log.LogString(LOG_INFO, " if (IsInspectorInEditMode(pInspector)):  TRUE ");
		SetTimer(IDT_IELOAD_TIMER, 500, NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CALLBACK CMsAddIn::MyItemClose(LPARAM lParam1, LPARAM lParam2)
{
	CFuncLog log(g_pLog, "CMsAddin::MyItemClose()");
	CMsAddIn* pAddIn = (CMsAddIn*)lParam1;
	if (!pAddIn)
		return;

	pAddIn->HandleOnItemClose();

	// If there is an inspector, shutdown document handling
	Outlook::_Inspector* pInspector = NULL;
	try
	{
		pInspector = (Outlook::_Inspector*)lParam2;
		pAddIn->ToolbarDestroy(pInspector);
		if (pInspector)
		{ //j There is a problem here - the spDoc always returns NULL because the mailitem is gone by the time we get a destroy
			IHTMLDocument2Ptr spDoc = pAddIn->GetHTMLDocument2(pInspector, false/*bConvert*/);
			pAddIn->DeactivateJazzyMailDocument(spDoc);
		}
	}
	catch (...)
	{
		int i = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
void STDMETHODCALLTYPE CMsAddIn::OnAppItemSend(IDispatch* pDispatch, VARIANT_BOOL* CancelDefault)
{
	CFuncLog log(g_pLog, "CMsAddin::OnAppItemSend()");
	// Called when any item is sent; after OnItemSend
	Outlook::_MailItemPtr spMailItem = pDispatch;
	if (spMailItem == NULL)
		return;

	Outlook::_InspectorPtr pInspector = spMailItem->GetGetInspector();
	if (pInspector == NULL)
		return;

	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(pInspector, false/*bConvert*/);
	HandleOnItemSend(spDoc);
}

/////////////////////////////////////////////////////////////////////////////
Outlook::_ExplorerPtr CMsAddIn::GetActiveExplorer()
{
	CFuncLog log(g_pLog, "CMsAddin::GetActiveExplorer()");
	Outlook::_ExplorerPtr spExplorer = m_pOutlook->ActiveExplorer();
	if (spExplorer == NULL)
		return NULL;

	return spExplorer;
}

/////////////////////////////////////////////////////////////////////////////
Outlook::_InspectorPtr CMsAddIn::GetActiveInspector()
{
	CFuncLog log(g_pLog, "CMsAddin::GetActiveInspector()");
	Outlook::_InspectorPtr pInspector = m_pOutlook->ActiveInspector();
	if (pInspector == NULL)
		return NULL;

	return pInspector;
}

/////////////////////////////////////////////////////////////////////////////
Outlook::_MailItemPtr CMsAddIn::GetMailItem(Outlook::_InspectorPtr pInspector)
{
	CFuncLog log(g_pLog, "CMsAddin::GetMailItem()");
	if (pInspector == NULL)
		return NULL;
		
	IDispatchPtr spDisp = pInspector->GetCurrentItem();
	if (spDisp == NULL)
		return NULL;

	Outlook::_MailItemPtr pMailItem = spDisp;
	return pMailItem;
}

// Called from a Web page
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CMsAddIn::Download(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice, VARIANT varExternalLink)
{
	CFuncLog log(g_pLog, "CMsAddin::Download()");
	return HandleDownloadRequest(varProductId, varName, varDownload, varFlags, varWidth, varHeight, varFlashPath, varPathPrice, varExternalLink);
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CMsAddIn::Download2(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice)
{
	CFuncLog log(g_pLog, "CMsAddin::Download2()");
	return HandleDownloadRequest(varProductId, varName, varDownload, varFlags, varWidth, varHeight, varFlashPath, varPathPrice, CComVariant(CString("")));
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CMsAddIn::ExecuteCommand(VARIANT varCmdId)
{
	CFuncLog log(g_pLog, "CMsAddIn::ExecuteCommand()");
	
	ProcessExecuteCommand(varCmdId.intVal);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddIn::LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject)
{
	CFuncLog log(g_pLog, "CMsAddIn::LaunchMail");

	if (m_Security.FixupEmailEditor(false/*bSilent*/, true/*bAllowChange*/))
		return false;
	
	CString strHTMLText;
	if (strDownloadSrc.IsEmpty())
		strHTMLText = strDownloadDst;
	else
		strHTMLText = ReadHTML(strDownloadSrc, strDownloadDst, false/*bPreview*/);

	m_strHTMLText = strHTMLText;

	// See if we can reuse the active compose window
	bool bSuccess = false;
	Outlook::_InspectorPtr pInspector = GetActiveInspector();
	Outlook::_MailItemPtr spMailItem = GetMailItem(pInspector);
	if (bAllowReuse && pInspector != NULL && spMailItem != NULL && IsInspectorInEditMode(pInspector))
	{ // The active compose window is editable and has no subject, so it is a target for the Creata Mail content
		log.LogString(LOG_INFO, " PREEXECUTE: GetHTMLDocument2(pInspector, true/*bConvert*/)");
		IHTMLDocument2Ptr spDoc = GetHTMLDocument2(pInspector, true/*bConvert*/);
		log.LogString(LOG_INFO, " POSTEXECUTE: GetHTMLDocument2(pInspector, true/*bConvert*/)");
		if (spDoc != NULL)
		{
			// Wait for the document to become ready
			if (!IsDocLoadedWait(spDoc))
				return false;

			// Make sure the Add_a_Photo dialog is not up
			if (m_PhotoManager.HasOpenDialog())
			{
				log.LogString(LOG_WARNING, log.FormatString(" FAILURE: if (m_PhotoManager.HasOpenDialog())"));
				return false;
			}

			// Check Security policies
			if (!VerifySecurity(spDoc, m_strHTMLText))
			{
				log.LogString(LOG_WARNING, log.FormatString(" FAILURE: if (!VerifySecurity(spDoc, m_strHTMLText))"));
				m_strHTMLText.Empty();
				return false;
			}

			// Reset current hooks.
			DeactivateJazzyMailDocument(spDoc);
			log.LogString(LOG_INFO, " DeactivateJazzyMailDocument(spDoc): COMPLETED" );

			// Identify non JazzyMail text to preserve, such as signatures
			MarkNonJazzyMailTextForPreservation(spDoc);
			log.LogString(LOG_INFO, " MarkNonJazzyMailTextForPreservation(spDoc): COMPLETED" );

			// Extract any html that will be retained from current message
			CString strPrevHtmlText;
			GetHtmlText(spDoc, strPrevHtmlText);
			log.LogString(LOG_INFO, " GetHtmlText(spDoc, strPrevHtmlText): COMPLETED" );

			// Write the html to message window
			if (WriteHtmlToDocument(CComBSTR(m_strHTMLText), spDoc))
			{
				log.LogString(LOG_INFO, " WriteHtmlToDocument(CComBSTR(m_strHTMLText), spDoc): COMPLETED" );

				// Restore any text that was retained from current message
				SetHtmlText(spDoc, strPrevHtmlText);

				log.LogString(LOG_INFO, " SetHtmlText(spDoc, strPrevHtmlText): COMPLETED" );

				// Setup edit fields to identify which fields are editble 
				// by user and update text insertion point
				SetDocEditFields(spDoc);

				log.LogString(LOG_INFO, " SetDocEditFields(spDoc): COMPLETED" );

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
		Outlook::_ApplicationPtr pOutlook = m_pOutlook;

		// Logon. Doesn't hurt if you are already running and logged on
		_NameSpacePtr pOutlookNamespace = pOutlook->GetNamespace((BSTR)CComBSTR("MAPI"));
		CComVariant varOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
		HRESULT hr = pOutlookNamespace->Logon(varOptional, varOptional, varOptional, varOptional);

		log.LogString(LOG_INFO, log.FormatString(" HRESULT hr = pOutlookNamespace->Logon(varOptional, varOptional, varOptional, varOptional): COMPLETED, hr == [%X]",hr));

		// Create a new mail message
		IDispatchPtr pCreateDispatch = pOutlook->CreateItem(Outlook::olMailItem);
		spMailItem = pCreateDispatch; 
		log.LogString(LOG_INFO, log.FormatString(" IDispatchPtr pCreateDispatch = pOutlook->CreateItem(Outlook::olMailItem): COMPLETED, pCreateDispatch == %X", pCreateDispatch));

	//j	spMailItem->PutTo((BSTR)CComBSTR(""));
		spMailItem->PutSubject((BSTR)CComBSTR(strSubject));
		log.LogString(LOG_INFO, " spMailItem->PutSubject((BSTR)CComBSTR(strSubject)): COMPLETED" );

		spMailItem->PutVotingResponse((BSTR)CComBSTR(TOOLBAR_NAME));
		log.LogString(LOG_INFO, " spMailItem->PutVotingResponse((BSTR)CComBSTR(TOOLBAR_NAME)): COMPLETED" );

		// Send the message!  This causes the OnNewInspector event to be triggered
		hr = spMailItem->Display(CComVariant((long)false/*bModal*/));
		log.LogString(LOG_INFO, " hr = spMailItem->Display(CComVariant((long)false/*bModal*/)): COMPLETED" );

		hr = pOutlookNamespace->Logoff();
		log.LogString(LOG_INFO, log.FormatString(" hr = pOutlookNamespace->Logoff(): COMPLETED, hr == [%X]",hr));

		bSuccess = true;
	}

	return bSuccess;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddIn::LaunchFeedbackMail(DWORD dwCmd)
{
	Outlook::_ApplicationPtr pOutlook = m_pOutlook;

	// Logon. Doesn't hurt if you are already running and logged on
	_NameSpacePtr pOutlookNamespace = pOutlook->GetNamespace((BSTR)CComBSTR("MAPI"));
	CComVariant varOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	HRESULT hr = pOutlookNamespace->Logon(varOptional, varOptional, varOptional, varOptional);

	// Create a new mail message
	IDispatchPtr pCreateDispatch = pOutlook->CreateItem(Outlook::olMailItem);
	Outlook::_MailItemPtr spMailItem = pCreateDispatch;

	CString strSubject = "Creata Mail ";
	strSubject += (dwCmd == ID_OPTIONS_GIVEFEEDBACK ? "Feedback" : "Problem Report");
	spMailItem->PutTo((BSTR)CComBSTR("tsoltis@ag.com"));
	spMailItem->PutCC((BSTR)CComBSTR("jmccurdy@ag.com"));
	spMailItem->PutSubject((BSTR)CComBSTR(strSubject));

	// Send the message!  This causes the OnNewInspector event to be triggered
	hr = spMailItem->Display(CComVariant((long)false/*bModal*/));

	hr = pOutlookNamespace->Logoff();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddIn::EmbedHTMLAtEnd(const CString& strHTMLText)
{
	CFuncLog log(g_pLog, "CMsAddin::EmbedHTMLAtEnd()");
	Outlook::_InspectorPtr pInspector = GetActiveInspector();
	if (pInspector == NULL)
		return false;

	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(pInspector, false/*bConvert*/);
	return EmbedHTMLAtEndBase(spDoc, strHTMLText);
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddIn::EmbedHTMLAtSelection(const CString& strHTMLText)
{
	CFuncLog log(g_pLog, "CMsAddin::EmbedHTMLAtSelection()");
	Outlook::_InspectorPtr pInspector = GetActiveInspector();
	if (pInspector == NULL)
		return false;

	if (!IsInspectorInEditMode(pInspector))
		return false;

	if (m_Security.FixupEmailEditor(false/*bSilent*/, true/*bAllowChange*/))
		return true; // So we don't call LaunchMail

	// Get a pointer to the doc and embed the HTML
	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(pInspector, true/*bConvert*/);
	return EmbedHTMLAtSelectionBase(strHTMLText, spDoc);
}

/////////////////////////////////////////////////////////////////////////////
IHTMLDocument2Ptr CMsAddIn::GetHTMLDocument2(Outlook::_InspectorPtr pInspector, bool bConvert)
{
	CFuncLog log(g_pLog, "CMsAddin::GetHTMLDocument2()");
	Outlook::_MailItemPtr spMailItem = GetMailItem(pInspector);
	if (pInspector == NULL || spMailItem == NULL)
	{
		log.LogString(LOG_WARNING, "if (pInspector == NULL || spMailItem == NULL): TRUE");
		return NULL;
	}

	// Get a pointer to the doc
#ifdef NOTUSED
	IDispatchPtr spDocDisp = pInspector->GetHTMLEditor();
	IHTMLDocument2Ptr spDoc = (IHTMLDocument2Ptr)spDocDisp;
#else
	IOleWindowPtr pOleWindow = pInspector;
	if (pOleWindow == NULL)
		return NULL;

	HWND hWnd = NULL;
	pOleWindow->GetWindow(&hWnd);
	if (!hWnd)
		return NULL;

	IHTMLDocument2Ptr spDoc = CMsAddInBase::GetHTMLDocument2(hWnd);
#endif
	if (spDoc != NULL)
	{
		log.LogString(LOG_INFO, "if (spDoc != NULL): TRUE");
		return spDoc;
	}

	if (!bConvert)
	{
		log.LogString(LOG_WARNING, "if (!bConvert): TRUE");
		return NULL;
	}

	CComBSTR bstrBody;
	CString strBody;
#ifdef NOTUSED
	bstrBody = (BSTR)spMailItem->GetBody();
	strBody = bstrBody;
#else
	CComPtr<ITextDocument> spTextDocument = GetPlainTextDocument(hWnd);
#endif NOTUSED

	if (!spTextDocument)
		return NULL;

	CComPtr<ITextRange> spTextRange;
	HRESULT hr = spTextDocument->Range(0, 1, &spTextRange);
	if (!spTextRange)
		return NULL;

	long lDelta=1;
	while (lDelta > 0)
	{
		hr = spTextRange->MoveEnd(tomStory, 1, &lDelta);
		if (FAILED(hr))
			return NULL;
	}
	
	hr = spTextRange->GetText(&bstrBody);
	spTextRange = NULL;
	spTextDocument = NULL;
	if (FAILED(hr))
		return NULL;

	strBody = bstrBody;
	Replace(strBody, "\r", "<br>");

	CString strNewBody;
	strNewBody += "<html><body><div>";
	strNewBody += strBody;
	strNewBody += "</div></body></html>";
	log.LogString(LOG_INFO, "PREEXECUTE: spMailItem->PutHTMLBody((BSTR)CComBSTR(strNewBody))");
	spMailItem->PutHTMLBody((BSTR)CComBSTR(strNewBody));
	log.LogString(LOG_INFO, "POSTEXECUTE: spMailItem->PutHTMLBody((BSTR)CComBSTR(strNewBody))");

#ifdef NOTUSED
	spDocDisp = pInspector->GetHTMLEditor();
	spDoc = (IHTMLDocument2Ptr)spDocDisp;
#else
	spDoc = CMsAddInBase::GetHTMLDocument2(hWnd);
#endif

	if (spDoc == NULL)
	{
		log.LogString(LOG_WARNING, "if (spDoc == NULL): TRUE");
		return NULL;
	}

	// Wait for the document to become ready
	bool bOK = IsDocLoadedWait(spDoc);

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in GetHTMLDocument2 during convert to HTML", g_szAppName, MB_OK);
#endif _DEBUG

	return spDoc;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddIn::IsInspectorInEditMode(Outlook::_InspectorPtr pInspector)
{
	CFuncLog log(g_pLog, "CMsAddin::IsInspectorInEditMode()");
	Outlook::_MailItemPtr spMailItem = GetMailItem(pInspector);
	if (pInspector == NULL || spMailItem == NULL)
		return false;

	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(pInspector, false/*bConvert*/);
	if (spDoc)
		return IsDocInEditMode(spDoc);

	// Must be a plain text document
	VARIANT_BOOL bSent = spMailItem->GetSent();
	return !bSent;
}

////////////////////////////////////////////////////////////////////////////
LRESULT CMsAddIn::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "CMsAddIn::OnTimer()");

	if (IDT_IELOAD_TIMER != wParam)
		return 0;

	Outlook::_InspectorPtr pInspector = GetActiveInspector();
	Outlook::_MailItemPtr spMailItem = GetMailItem(pInspector);
	if (pInspector == NULL || spMailItem == NULL)
	{ // If this fails, stop the timer and get out
		KillTimer(IDT_IELOAD_TIMER);
		log.LogString(LOG_ERROR, log.FormatString(" FAILURE: if (pInspector == NULL || spMailItem == NULL), pInspector=%X, spMailItem=%X", pInspector, spMailItem));
		return 0;
	}

	// If the document is not loaded yet (and its not plain text), keep checking
	IHTMLDocument2Ptr spDoc = GetHTMLDocument2(pInspector, false/*bConvert*/);
	if (!(IsDocLoaded(spDoc) || spDoc == NULL))
		return 0;

	// The document is loaded (or it is plain text), stop the timer
	KillTimer(IDT_IELOAD_TIMER);

	// Did we launch this compose window?
	log.LogString(LOG_INFO, " PREEXECUTE: CComBSTR strAttribute = (BSTR)spMailItem->GetVotingResponse()");
	CComBSTR strAttribute = (BSTR)spMailItem->GetVotingResponse();
	log.LogString(LOG_INFO, " POSTEXECUTE: CComBSTR strAttribute = (BSTR)spMailItem->GetVotingResponse()");
	bool bLaunchedByJazzyMail = (strAttribute == TOOLBAR_NAME);
	if (bLaunchedByJazzyMail)
	{
		log.LogString(LOG_INFO, " PREEXECUTE: spMailItem->PutVotingResponse((BSTR)CComBSTR(""))");
		spMailItem->PutVotingResponse((BSTR)CComBSTR(""));
		log.LogString(LOG_INFO, " POSTEXECUTE: spMailItem->PutVotingResponse((BSTR)CComBSTR(""))");
	}

	// Will we be adding content?
	bool bNeedToAddContent = (bLaunchedByJazzyMail && !m_strHTMLText.IsEmpty());
	bool bBackgroundContent = (bNeedToAddContent &&
			  (m_DownloadParms.iCategory != TYPE_CLIPART && 
			   m_DownloadParms.iCategory != TYPE_SMILEYS &&
			   m_DownloadParms.iCategory != TYPE_SOUNDS));

#ifdef _DEBUG
	// Will we be adding stationery?
	COleDateTime FileDate;
	CComBSTR bstrDate;
	if (spDoc)
	{
		log.LogString(LOG_INFO, " PREEXECUTE: Get file date");
		HRESULT hr = S_OK;
		hr = spDoc->get_fileSize(&bstrDate);
		hr = spDoc->get_lastModified(&bstrDate);
		hr = spDoc->get_fileModifiedDate(&bstrDate);
		hr = spDoc->get_fileUpdatedDate(&bstrDate);
		hr = spDoc->get_fileCreatedDate(&bstrDate);
		FileDate.ParseDateTime(CString(bstrDate), 0/*dwFlags*/);
		log.LogString(LOG_INFO, " POSTEXECUTE: Get file date");
	}

	bool bNewMessage = false;
	const DATE DateNone = 949998; // 1/1/4501
	DATE Date = spMailItem->GetCreationTime();
	if (Date == DateNone)
		bNewMessage = true;
	else
	{
		FileDate = COleDateTime(Date);
		int fm = FileDate.GetMonth();
		int fd = FileDate.GetDay();
		int fy = FileDate.GetYear();

		COleDateTime Now = COleDateTime::GetCurrentTime();
		int nm = Now.GetMonth();
		int nd = Now.GetDay();
		int ny = Now.GetYear();
		if (Now > FileDate)
		{
			COleDateTimeSpan Span = (Now - FileDate);
			double fMinsOld = Span.GetTotalMinutes();
			bNewMessage = (fMinsOld <= 1);
		}
	}
#endif _DEBUG

	// Will we be adding stationery?
	CComBSTR bstrSubject = (BSTR)spMailItem->GetSubject();
	CString strSubject = bstrSubject;
	bool bNeedToAddStationery = (m_StationeryMgr.NeedToAdd() && !bBackgroundContent && strSubject.IsEmpty());

	// Will we be changing the document?
	bool bNeedToChange = bNeedToAddStationery || bNeedToAddContent;
	if (!bNeedToChange)
	{
		log.LogString(LOG_INFO, " if (!bNeedToChange): TRUE" );
		return 0;
	}

	// If this is a plain text document, we must convert it now
	if (spDoc == NULL) 
	{
		spDoc = GetHTMLDocument2(pInspector, true/*bConvert*/);
		if (spDoc == NULL) 
		{
			log.LogString(LOG_INFO, " GetHTMLDocument2(pInspector, true/*bConvert*/) = NULL" );
			return 0;
		}
	}

	// Check Security policies
	bool bSecurityPassed = VerifySecurity(spDoc, m_strHTMLText);
	if (!bSecurityPassed)
	{
		log.LogString(LOG_WARNING, " if (!bSecurityPassed): TRUE" );
		m_strHTMLText.Empty();
		return 0;
	}

	// Identify non JazzyMail text to preserve, such as signatures
	MarkNonJazzyMailTextForPreservation(spDoc);
	log.LogString(LOG_INFO, " MarkNonJazzyMailTextForPreservation(spDoc): COMPLETED" );

	// Add the default stationery
	if (bNeedToAddStationery)
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
			{
				log.LogString(LOG_ERROR, " if (!SetHtmlText(spDoc, m_strHTMLText)): TRUE" );
				return 0;
			}
			log.LogString(LOG_INFO, " if (!SetHtmlText(spDoc, m_strHTMLText)): FALSE" );
		}
		else 
		{
			// Extract any html that will be retained from current message
			CString strPrevHtmlText;
			GetHtmlText(spDoc, strPrevHtmlText);
			log.LogString(LOG_INFO, " GetHtmlText(spDoc, strPrevHtmlText): COMPLETED" );

			// Write the html to message window
			if (WriteHtmlToDocument(CComBSTR(m_strHTMLText), spDoc))
			{
				log.LogString(LOG_INFO, " WriteHtmlToDocument(CComBSTR(m_strHTMLText), spDoc): COMPLETED" );
				
				// Restore any text that was retained from current message
				SetHtmlText(spDoc, strPrevHtmlText);

				log.LogString(LOG_INFO, " SetHtmlText(spDoc, strPrevHtmlText): COMPLETED" );

				// Setup edit fields to identify which fields are editble 
				// by user and update text insertion point
				SetDocEditFields(spDoc);

				log.LogString(LOG_INFO, " SetDocEditFields(spDoc): COMPLETED" );
			}
		}

		m_strHTMLText.Empty();
	}

	// Activate the new document to set hooks
	ActivateJazzyMailDocument(spDoc, bLaunchedByJazzyMail);
	return 0;
}

////////////////////////////////////////////////////////////////////////////
LRESULT CMsAddIn::OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CFuncLog log(g_pLog, "CMsAddin::OnHelp()");
	ShowHelp();
	return 0;
}
////////////////////////////////////////////////////////////////////////////
LRESULT CMsAddIn::OnUpdateResource(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	AutoUpdateDone(UPDATE_RESOURCES);
	return S_OK;
}
////////////////////////////////////////////////////////////////////////////
LRESULT CMsAddIn::OnShutdownAll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bSrvrShutdown = true;
	HandleShutdown();
	return 0;
}
////////////////////////////////////////////////////////////////////////////
LRESULT CMsAddIn::OnRemoveAddin(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (m_Authenticate.IsOutlookEnabled())
		return S_OK;

	m_bSrvrShutdown = false;
	ShowInspectorToolbars(false);
	ShowExplorerToolbars(false);
	HandleShutdown();
	return S_OK;
}
#ifdef NOTUSED
/////////////////////////////////////////////////////////////////////////////
bool CopyButtonMaskToClipboard(HBITMAP hbmMask, HDC hdcTarget)
{
	// Get the BITMAPHEADERINFO for the mask
	BITMAPINFO uBIH;
	uBIH.bmiHeader.biSize = 40;
	::GetDIBits(hdcTarget, hbmMask, 0, 0, NULL, &uBIH, DIB_RGB_COLORS);

	// Make sure it is a mask image.
	if (uBIH.bmiHeader.biBitCount != 1)
		return false;
	if (uBIH.bmiHeader.biSizeImage < 1)
		return false;

	// Open the clipboard.
	if (!::OpenClipboard(NULL))
		return false;

	// Get the cf for button face and mask.
	long cfBtnFace = ::RegisterClipboardFormat("Toolbar Button Face");
	long cfBtnMask = ::RegisterClipboardFormat("Toolbar Button Mask");
	    
	// Open DIB on the clipboard and make a copy of it for the button face.
	HANDLE hMemTmp = ::GetClipboardData(CF_DIB);
	if (hMemTmp)
	{
		SIZE_T cbSize = ::GlobalSize(hMemTmp);
		HGLOBAL hGMemFace = ::GlobalAlloc(GMEM_SHARE|GMEM_MOVEABLE, cbSize);
		if (hGMemFace)
		{
			LPVOID lpData = ::GlobalLock(hMemTmp);
			LPVOID lpData2 = ::GlobalLock(hGMemFace);
			::CopyMemory(lpData2, lpData, cbSize);
			::GlobalUnlock(hGMemFace);
			::GlobalUnlock(hMemTmp);
	    
			if (!::SetClipboardData(cfBtnFace, hGMemFace))
				::GlobalFree(hGMemFace);
		}
	}

	// Now get the mask bits and the rest of the header.
	// Create a temp buffer to hold the bitmap bits.
	BYTE* arrBMDataBuffer = new BYTE[uBIH.bmiHeader.biSizeImage + 4];
	::GetDIBits(hdcTarget, hbmMask, 0, uBIH.bmiHeader.biSizeImage, arrBMDataBuffer, &uBIH, DIB_RGB_COLORS);
	    
	// Copy them to global memory and set it on the clipboard.
	HGLOBAL hGMemMask = ::GlobalAlloc(GMEM_SHARE|GMEM_MOVEABLE, uBIH.bmiHeader.biSizeImage + 50);
	if (hGMemMask)
	{
		BYTE* lpData = (BYTE*)::GlobalLock(hGMemMask);
		::CopyMemory(lpData, &uBIH, 48);
		::CopyMemory(lpData + 48, arrBMDataBuffer, uBIH.bmiHeader.biSizeImage);
		::GlobalUnlock(hGMemMask);
	    
		if (!::SetClipboardData(cfBtnMask, hGMemMask))
			::GlobalFree(hGMemMask);
	}

	delete [] arrBMDataBuffer;
	::CloseClipboard();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
HBITMAP CreateButtonMask(HBITMAP hbmSource, COLORREF clrMask, HDC hdcTarget, HPALETTE hPal)
{
	// Get some information about the bitmap handed to us.
	BITMAP uBM;
	::GetObject(hbmSource, sizeof(BITMAP), &uBM);

	// Check the size of the bitmap given.
	if (uBM.bmWidth < 1 || uBM.bmWidth > 30000)
		return NULL;
	if (uBM.bmHeight < 1 || uBM.bmHeight > 30000)
		return NULL;

	// Create a compatible DC, load the palette and the bitmap.
	HDC hdcSource = ::CreateCompatibleDC(hdcTarget);
	HPALETTE hpalSourceOld = ::SelectPalette(hdcSource, hPal, true);
	::RealizePalette(hdcSource);
	HBITMAP hbmSourceOld = (HBITMAP)::SelectObject(hdcSource, hbmSource);

	// Create a black and white mask the same size as the image.
	HBITMAP hbmMask = ::CreateBitmap(uBM.bmWidth, uBM.bmHeight, 1, 1, NULL);
	if (!hbmMask)
	{
		::DeleteDC(hdcSource);
		return NULL;
	}

	// Create a compatble DC for it and load it.
	HDC hdcMask = ::CreateCompatibleDC(hdcTarget);
	HBITMAP hbmMaskOld = (HBITMAP)::SelectObject(hdcMask, hbmMask);

	// All you need to do is set the mask color as the background color
	// on the source picture, and set the forground color to white, and
	// then a simple BitBlt will make the mask for you.
	::SetBkColor(hdcSource, clrMask);
	::SetTextColor(hdcSource, RGB(255,255,255));
	::BitBlt(hdcMask, 0, 0, uBM.bmWidth, uBM.bmHeight, hdcSource, 0, 0, SRCCOPY);

	// Clean up the memory DCs.
	::SelectObject(hdcMask, hbmMaskOld);
	::DeleteDC(hdcMask);

	::SelectObject(hdcSource, hbmSourceOld);
	::SelectObject(hdcSource, hpalSourceOld);
	::DeleteDC(hdcSource);

	return hbmMask;
}

/////////////////////////////////////////////////////////////////////////////
// create a mask based on the bitmap provided and copy both to the clipboard
// The first parameter is a Picture object. The second is the transparentcolor
bool CopyBitmapAsButtonFace(IPicture* pPicture, OLE_COLOR clrMaskColor)
{
	if (!pPicture)
		return false;

	short Type = 0;
	HRESULT hr = pPicture->get_Type(&Type);
	if (Type != PICTYPE_BITMAP)
		return false;

	HBITMAP hBitmap = NULL;
	hr = pPicture->get_Handle((OLE_HANDLE*)&hBitmap);
	if (!hBitmap)
		return false;
		
	//DEBUG
	BITMAP uBM;
	::GetObject(hBitmap, sizeof(BITMAP), &uBM);

	// Get the DC for the display device we are on.
	HDC hdcScreen = ::GetDC(NULL);
	HPALETTE hPal = NULL;
	hr = pPicture->get_hPal((OLE_HANDLE*)&hPal);
	bool bDeletePal = false;
	if (!hPal)
	{
		hPal = ::CreateHalftonePalette(hdcScreen);
		bDeletePal = true;
	}

	// Translate the OLE_COLOR value to a GDI COLORREF value based on the palette.
	COLORREF clrMask = NULL;
	::OleTranslateColor(clrMaskColor, hPal, &clrMask);
	    
	// Create a mask based on the image handed in (hbmButtonMask is the result).
	bool bOK = false;
	HBITMAP hbmButtonMask = CreateButtonMask(hBitmap, clrMask, hdcScreen, hPal);
	if (hbmButtonMask)
	{
		// Copy the bitmap to the clipboard (for the CF_DIB)
		::SetClipboardData(CF_DIB, (HANDLE)hBitmap);
//j		SetClipboardData(pPicture, CF_DIB);

		// Now copy the button mask
		bOK = CopyButtonMaskToClipboard(hbmButtonMask, hdcScreen);
	}

	// Delete the mask and clean up (a copy is left on the clipboard)
	::DeleteObject(hbmButtonMask);
	if (bDeletePal)
		::DeleteObject(hPal);
	::ReleaseDC(NULL, hdcScreen);
	
	return bOK;
}
#endif NOTUSED
