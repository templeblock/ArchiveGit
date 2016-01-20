#include "stdafx.h"
#include "PreviewDialog.h"
#include "HelperFunctions.h"
#include "MsAddInBase.h"
#include "FunctionObject.h"
#include "FlashManager.h"
#include "Authenticate.h"
#include "Regkeys.h"
#include "HelperFunctions.h"

#define USE_TEMP_FILE true
#define DEFAULT_PREVIEW_WIDTH 620
#define DEFAULT_PREVIEW_HEIGHT 620
static const DISPID DISPID_DRAGSTART = 0x8001;
static UINT IDT_DOCLOAD_TIMER = ::RegisterWindowMessage("Creata Mail Doc Load Timer");

/////////////////////////////////////////////////////////////////////////////
CPreviewDialog::CPreviewDialog(CMsAddInBase* pAddin)
{
	m_pAddin = pAddin;
	m_hWnd = NULL;
	m_pDownloadParms = new DownloadParms();
}

/////////////////////////////////////////////////////////////////////////////
CPreviewDialog::~CPreviewDialog(void)
{
	m_spCurrentDoc = NULL;
	m_pBrowser = NULL;

	if (m_pDownloadParms)
		delete m_pDownloadParms;

	if (!m_strTempFile.IsEmpty())
		::DeleteFile(m_strTempFile);
}
/////////////////////////////////////////////////////////////////////////////
void CPreviewDialog::SetAddin(CMsAddInBase* pAddin)
{
	m_pAddin = pAddin;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPreviewDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "CPreviewDialog::OnInitDialog");
	DlgResize_Init();

	// Restore the saved window size and position
	RECT rect = {0, 0, DEFAULT_PREVIEW_WIDTH, DEFAULT_PREVIEW_HEIGHT};
	bool bMove = false;
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		DWORD dwSize = sizeof(rect);
		RECT rectLocation = rect;
		if (regkey.QueryBinaryValue(REGVAL_PREVIEW_LOCATION, &rectLocation, &dwSize) == ERROR_SUCCESS && dwSize == sizeof(rectLocation))
		{
			rect = rectLocation;
			bMove = true;
			log.LogString(LOG_INFO, log.FormatString("regkey.Open(REGKEY_APPROOT, REGKEY_APP == == ERROR_SUCCESS, rectLocation == %d, dwSize == %d", rectLocation, dwSize));
		}
	}

	//j Someday, clip the rect to the display
	SetWindowPos(NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), (bMove ? 0 : SWP_NOMOVE) | SWP_NOZORDER | SWP_NOACTIVATE);
	if (!bMove)
		CenterWindow();
	
 	// Create the web browser control
	m_BrowserWnd = GetDlgItem(IDC_EXPLORER);
	if (!::IsWindow(m_BrowserWnd.m_hWnd))
		return TRUE;

	HRESULT hr = m_BrowserWnd.QueryControl(&m_pBrowser);
	ATLASSERT(SUCCEEDED(hr));

	log.LogString(LOG_INFO, log.FormatString(" hr = m_BrowserWnd.QueryControl(&m_pBrowser) hr==%[X], m_pBrowser== %X", hr, m_pBrowser));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPreviewDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	CFuncLog log(g_pLog, "CPreviewDialog::OnClose");

	// Reset callbacks
	SetMousedownCallback(m_spCurrentDoc, true);
	SetBodySelectCallback(m_spCurrentDoc, true);
	SetKeyPressCallback(m_spCurrentDoc, true);
	SetDragStartCallback(m_spCurrentDoc, true);
	m_spCurrentDoc = NULL;
	
	// Store the window location in the registty
	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		WINDOWPLACEMENT placement;
		placement.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(&placement);
		regkey.SetBinaryValue(REGVAL_PREVIEW_LOCATION, &placement.rcNormalPosition, sizeof(RECT));
	}

	// Need to release m_pBrowser since CPreviewDialog may still 
	// be around even though window is destroyed,
	// i.e. m_pBrowser doesn't go out of scope. 
	m_pBrowser = NULL;

	DestroyWindow();
	m_hWnd = NULL;
	m_strURL = "";
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CPreviewDialog::Close()
{
	CFuncLog log(g_pLog, "CPreviewDialog::Close()");

	if (m_hWnd && ::IsWindow(m_hWnd))
		SendMessage(WM_CLOSE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPreviewDialog::Display(HWND hWndParent, DownloadParms& Parms, int iWidth, int iHeight)
{
	CFuncLog log(g_pLog, "CPreviewDialog::Display()");

	// Make a copy of the download parms
	*m_pDownloadParms = Parms;
	m_pDownloadParms->iSource = SOURCE_PREVIEW;
	
	// Display Preview - create if necessary.
	if (!::IsWindow(m_hWnd))
	{
		Create(hWndParent);
		HICON hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(),	(LPCTSTR)MAKEINTRESOURCE(IDI_LOGO), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED);
		SetIcon(hIcon, true/*bBigIcon*/);
		hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(),	(LPCTSTR)MAKEINTRESOURCE(IDI_LOGO), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED);
		SetIcon(hIcon, false/*bBigIcon*/);
	}

	if (m_pDownloadParms->bPayingMember)
	{
		//Show Insert button
		CButton btnInsert = GetDlgItem(ID_CONTENT_SEND);
		btnInsert.ShowWindow(SW_SHOW);

		// Show Add to Favorites
		CButton btnAddToFavorites = GetDlgItem(ID_CONTENT_ADDTOFAVORITES); 
		btnAddToFavorites.ShowWindow(SW_HIDE);//btnAddToFavorites.ShowWindow(SW_SHOW); - jhc

		// Hide Upgrade
		CButton btnUpgrade = GetDlgItem(ID_OPTIONS_UPGRADENOW);
		btnUpgrade.ShowWindow(SW_HIDE);
	}
	else
	if (m_pDownloadParms->bFreeContent)
	{
		//Show Insert button
		CButton btnInsert = GetDlgItem(ID_CONTENT_SEND);
		btnInsert.ShowWindow(SW_SHOW);

		// Hide Add to Favorites
		CButton btnAddToFavorites = GetDlgItem(ID_CONTENT_ADDTOFAVORITES); 
		btnAddToFavorites.ShowWindow(SW_HIDE);//btnAddToFavorites.ShowWindow(SW_SHOW); - jhc

		// Show Upgrade
		CButton btnUpgrade = GetDlgItem(ID_OPTIONS_UPGRADENOW);
		btnUpgrade.ShowWindow(SW_SHOW);
	}
	else
	{
		// Hide Insert button
		CButton btnInsert = GetDlgItem(ID_CONTENT_SEND);
		btnInsert.ShowWindow(SW_HIDE);

		// Hide Add to Favorites
		CButton btnAddToFavorites = GetDlgItem(ID_CONTENT_ADDTOFAVORITES);
		btnAddToFavorites.ShowWindow(SW_HIDE);

		// Show Upgrade
		CButton btnUpgrade = GetDlgItem(ID_OPTIONS_UPGRADENOW);
		btnUpgrade.ShowWindow(SW_SHOW);
	}

	// If height and width specified change size of window.
	if (iWidth && iHeight)
		SetWindowPos(HWND_TOP, 0, 0, iWidth, iHeight, SWP_NOMOVE);

	// Load the blank Preview document.
	Load("about:blank");
	
	BringWindowToTop();
	ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPreviewDialog::AddHtmlToPreview(LPCTSTR pstrHtml)
{
	CFuncLog log(g_pLog, "CPreviewDialog::AddHtmlToPreview()");

	m_strHtml = pstrHtml;

	if (USE_TEMP_FILE)
	{
		if (m_strTempFile.IsEmpty())
			m_strTempFile = RandomFileName("jm", ".htm");
		if (m_pAddin->WriteHTMLFile(m_strTempFile, pstrHtml))
			Load(m_strTempFile);
	}

	SetTimer(IDT_DOCLOAD_TIMER, 200);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CPreviewDialog::Load(LPCTSTR pstrURL)
{
	CFuncLog log(g_pLog, "CPreviewDialog::Load()");

	if (!::IsWindow(m_hWnd))
		return false;

	if (m_strURL == pstrURL)
		return false;

	if (m_pBrowser == NULL)
		return false;

	m_strURL = pstrURL;

	if (m_spCurrentDoc)
	{
		// Reset callbacks
		SetMousedownCallback(m_spCurrentDoc, true);
		SetBodySelectCallback(m_spCurrentDoc, true);
		SetKeyPressCallback(m_spCurrentDoc, true);
		SetDragStartCallback(m_spCurrentDoc, true);
		m_spCurrentDoc = NULL;
	}

	CComVariant varURL = CComBSTR(pstrURL);
	CComVariant varFlags = navNoHistory;
	CComVariant varTargetFrameName = CComBSTR("_self");
	HRESULT hr = m_pBrowser->Navigate2(&varURL, &varFlags, &varTargetFrameName, NULL/*&varPostData*/, NULL/*&varHeaders*/);
	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
void CPreviewDialog::ShowMenu()
{
	CFuncLog log(g_pLog, "CPreviewDialog::ShowMenu()");

	if (!m_pAddin)
		return;

	CMenu Menu;
	Menu.LoadMenu(m_pDownloadParms->bPayingMember || m_pDownloadParms->bFreeContent ? IDR_PREVIEW_MENU1 : IDR_PREVIEW_MENU2);
	if (!Menu.IsMenu())
		return;
  
	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return;

	Popup.EnableMenuItem(ID_CONTENT_SEND, MF_BYCOMMAND | MF_ENABLED);
	Popup.EnableMenuItem(ID_OPTIONS_UPGRADENOW, MF_BYCOMMAND | MF_ENABLED);
	Popup.EnableMenuItem(ID_PREV_CLOSE, MF_BYCOMMAND | MF_ENABLED);

	POINT Point;
	GetCursorPos(&Point);
	WORD wCommand = Popup.TrackPopupMenu (TPM_RETURNCMD|TPM_NONOTIFY|TPM_TOPALIGN|TPM_LEFTALIGN|TPM_RIGHTBUTTON, Point.x, Point.y, m_hWnd );
	Menu.DestroyMenu();
	  
	if (!wCommand)
		return;

	switch (wCommand)
	{
		case ID_CONTENT_SEND:
		{
			bool bOK = m_pAddin->HandleDownloadCommand(ID_CONTENT_SEND, m_pDownloadParms);
			Close();
			break;
		}
		
		case ID_CONTENT_ADDTOFAVORITES:
		{
			bool bOK = m_pAddin->HandleDownloadCommand(ID_CONTENT_ADDTOFAVORITES, m_pDownloadParms);
			Close();
			break;
		}
		
		case ID_OPTIONS_UPGRADENOW:
		{
			bool bOK = m_pAddin->HandleDownloadCommand(ID_OPTIONS_UPGRADENOW, m_pDownloadParms);
			Close();
			break;
		}

		case ID_PREV_CLOSE:
		{
			Close();
			break;
		}

		default:
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPreviewDialog::OnBnClickedButton1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "CPreviewDialog::OnBnClickedButton1()");

	bool bOK = m_pAddin->HandleDownloadCommand(ID_CONTENT_SEND, m_pDownloadParms);
	Close();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPreviewDialog::OnBnClickedButton2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "CPreviewDialog::OnBnClickedButton2()");

	bool bOK = m_pAddin->HandleDownloadCommand(ID_CONTENT_ADDTOFAVORITES, m_pDownloadParms);
	Close();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPreviewDialog::OnBnClickedButton3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "CPreviewDialog::OnBnClickedButton3()");

	Close();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPreviewDialog::OnBnClickedButton4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "CPreviewDialog::OnBnClickedButton4()");

	bool bOK = m_pAddin->HandleDownloadCommand(ID_OPTIONS_UPGRADENOW, m_pDownloadParms);
	Close();
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
BOOL CPreviewDialog::SetMousedownCallback(IHTMLDocument2 *pDoc, BOOL bReset)
{
	CFuncLog log(g_pLog, "CPreviewDialog::SetMousedownCallback()");

	CComQIPtr<IHTMLDocument2> spDoc2(pDoc);
	if (!spDoc2)
		return FALSE;
	
	if (bReset)
	{
		VARIANT varNull; 
		varNull.vt = VT_NULL;
		HRESULT hr = spDoc2->put_onmousedown(varNull);
		if (FAILED(hr))
			return FALSE;
	}
	else
	{
		VARIANT varFO;
		varFO.vt = VT_DISPATCH;
		varFO.pdispVal = 
		CFunctionObject<CPreviewDialog>::CreateEventFunctionObject
										(this, 
										&CPreviewDialog::PreviewCallback,
										spDoc2,
										NULL,
										DISPID_MOUSEDOWN);
	
		HRESULT hr = spDoc2->put_onmousedown(varFO);
		if (FAILED(hr))
			return FALSE;
	}
		
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CPreviewDialog::SetBodySelectCallback(IHTMLDocument2 *pDoc, BOOL bReset)
{
	CFuncLog log(g_pLog, "CPreviewDialog::SetBodySelectCallback()");

	CComQIPtr<IHTMLDocument2> spDoc2(pDoc);
	if (!spDoc2)
		return FALSE;
		
	if (bReset)
	{
		VARIANT varNull; 
		varNull.vt = VT_NULL;
		HRESULT hr = spDoc2->put_onselectstart(varNull);
		if (FAILED(hr))
			return FALSE;
	}
	else
	{
		VARIANT varFO;
		varFO.vt = VT_DISPATCH;
		varFO.pdispVal = 
		CFunctionObject<CPreviewDialog>::CreateEventFunctionObject
										(this, 
										&CPreviewDialog::PreviewCallback,
										spDoc2,
										NULL,
										DISPID_SELSTART);
	
		HRESULT hr = spDoc2->put_onselectstart(varFO);
		if (FAILED(hr))
			return FALSE;
	}
		
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CPreviewDialog::SetKeyPressCallback(IHTMLDocument2 *pDoc, BOOL bReset)
{
	CFuncLog log(g_pLog, "CPreviewDialog::SetKeyPressCallback()");

	CComQIPtr<IHTMLDocument2> spDoc2(pDoc);
	if (!spDoc2)
		return FALSE;

		
	if (bReset)
	{
		VARIANT varNull; 
		varNull.vt = VT_NULL;
		HRESULT hr = spDoc2->put_onkeypress(varNull);
		if (FAILED(hr))
			return FALSE;
	}
	else
	{
		VARIANT varFO;
		varFO.vt = VT_DISPATCH;
		varFO.pdispVal = 
		CFunctionObject<CPreviewDialog>::CreateEventFunctionObject
										(this, 
										&CPreviewDialog::PreviewCallback,
										spDoc2,
										NULL,
										DISPID_KEYPRESS);
	
		HRESULT hr = spDoc2->put_onkeypress(varFO);
		if (FAILED(hr))
			return FALSE;
	}
		
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CPreviewDialog::SetDragStartCallback(IHTMLDocument2 *pDoc, BOOL bReset)
{
	CFuncLog log(g_pLog, "CPreviewDialog::SetDragStartCallback()");

	CComQIPtr<IHTMLDocument2> spDoc2(pDoc);
	if (!spDoc2)
		return FALSE;

		
	if (bReset)
	{
		VARIANT varNull; 
		varNull.vt = VT_NULL;
		HRESULT hr = spDoc2->put_ondragstart(varNull);
		if (FAILED(hr))
			return FALSE;
	}
	else
	{
		VARIANT varFO;
		varFO.vt = VT_DISPATCH;
		varFO.pdispVal = 
		CFunctionObject<CPreviewDialog>::CreateEventFunctionObject
										(this, 
										&CPreviewDialog::PreviewCallback,
										spDoc2,
										NULL,
										DISPID_DRAGSTART);
	
		HRESULT hr = spDoc2->put_ondragstart(varFO);
		if (FAILED(hr))
			return FALSE;
	}
		
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
void CPreviewDialog::PreviewCallback(IDispatch * pDisp1, IDispatch * pDisp2, DISPID id, VARIANT* pVarResult)
{
	CFuncLog log(g_pLog, "CPreviewDialog::PreviewCallback()");

	if (!pDisp1)
		return;

	if (id == DISPID_MOUSEDOWN)			
	{
		CComQIPtr<IHTMLDocument2> spDoc(pDisp1);
		if (!spDoc)
			return;

		CComPtr<IHTMLWindow2> spWnd;
		HRESULT hr = spDoc->get_parentWindow(&spWnd);
		if (FAILED(hr) || !spWnd)
			return;
		
		CComPtr<IHTMLEventObj> spEventObj;
		hr = spWnd->get_event(&spEventObj);
		if (FAILED(hr) || !spEventObj)
			return;
		
		long lVal;
		spEventObj->get_button(&lVal);

		if ( lVal == 2) // 2 = Right button is pressed 		
			ShowMenu();
	}
	else if (id == DISPID_SELSTART)
	{
		CComQIPtr<IHTMLDocument2> spDoc(pDisp1);
		if (!spDoc)
			return;

		CComPtr<IHTMLWindow2> spWnd;
		HRESULT hr = spDoc->get_parentWindow(&spWnd);
		if (FAILED(hr) || !spWnd)
			return;
		
		CComPtr<IHTMLEventObj> spEventObj;
		hr = spWnd->get_event(&spEventObj);
		if (FAILED(hr) || !spEventObj)
			return;

		// Disable select operation
		VARIANT var;
		var.boolVal = false;
		var.vt = VT_BOOL;
		spEventObj->put_returnValue(var);
	}

	else if (id == DISPID_KEYPRESS)
	{
		CComQIPtr<IHTMLDocument2> spDoc(pDisp1);
		if (!spDoc)
			return;

		CComPtr<IHTMLWindow2> spWnd;
		HRESULT hr = spDoc->get_parentWindow(&spWnd);
		if (FAILED(hr) || !spWnd)
			return;
		
		CComPtr<IHTMLEventObj> spEventObj;
		hr = spWnd->get_event(&spEventObj);
		if (FAILED(hr) || !spEventObj)
			return;

		// Disable key press operation
		VARIANT var;
		var.boolVal = false;
		var.vt = VT_BOOL;
		spEventObj->put_returnValue(var);
	}
	else if (id == DISPID_DRAGSTART)
	{
		CComQIPtr<IHTMLDocument2> spDoc(pDisp1);
		if (!spDoc)
			return;

		CComPtr<IHTMLWindow2> spWnd;
		HRESULT hr = spDoc->get_parentWindow(&spWnd);
		if (FAILED(hr) || !spWnd)
			return;
		
		CComPtr<IHTMLEventObj> spEventObj;
		hr = spWnd->get_event(&spEventObj);
		if (FAILED(hr) || !spEventObj)
			return;

		// Disable drag operation
		VARIANT var;
		var.boolVal = false;
		var.vt = VT_BOOL;
		spEventObj->put_returnValue(var);
	}
}

//////////////////////////////////////////////////////////////////////////////
bool CPreviewDialog::HandleDocLoadComplete(IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CPreviewDialog::HandleDocLoadComplete()");

	if (spDoc == NULL)
		return false;

	// Write html to Preview document.
	if (!USE_TEMP_FILE)
		if (!m_pAddin->WriteHtmlToDocument(CComBSTR(m_strHtml), spDoc))
			return false;

	// Save reference to current document.
	m_spCurrentDoc = spDoc;

	// Disable flash menu.
	CFlashManager Flash;
	Flash.ShowMenu(m_spCurrentDoc, FALSE);

	// Set up callback to handle context menu - right click, drag and key press
	SetMousedownCallback(m_spCurrentDoc, false);
	SetBodySelectCallback(m_spCurrentDoc, false);
	SetKeyPressCallback(m_spCurrentDoc, false);
	SetDragStartCallback(m_spCurrentDoc, false);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPreviewDialog::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CFuncLog log(g_pLog, "CPreviewDialog::OnTimer()");

	if (m_pBrowser == NULL)
		return 0;

	// Get IHTMLDocument2 interface
	IDispatchPtr spDisp;
	HRESULT hr = m_pBrowser->get_Document(&spDisp);
	if (FAILED(hr))
		return 0;

	IHTMLDocument2Ptr spDoc = spDisp;
	if (spDoc == NULL)
		return 0;

	if (!IsDocLoaded(spDoc))
		return 0;

	KillTimer(IDT_DOCLOAD_TIMER);
	HandleDocLoadComplete(spDoc);

	return 0;
}
