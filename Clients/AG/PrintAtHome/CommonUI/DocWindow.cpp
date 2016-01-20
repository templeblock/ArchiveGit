#include "stdafx.h"
#include "Ctp.h"
#include "DocWindow.h"
#include "AGDC.h"
#include "AGPage.h"
#include "AGLayer.h"
#include "Track.h"
#include "RegKeys.h"
#include "Version.h"
#include "PAH_Constants.h"
#include <fstream>

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

#define CONTROL (GetAsyncKeyState(VK_CONTROL)<0)
#define SHIFT (GetAsyncKeyState(VK_SHIFT)<0)
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

#define DEFAULT_SHADOW_SIZE INCHES(0.05)

LPCTSTR CDocWindow::TEMP_ENVELOPES_DIR   = _T("Envelopes");
LPCTSTR CDocWindow::TEMP_CARDS_DIR       = _T("Cards");

// Because this has to be static, it is shared between instances
CDocWindow* CDocWindow::g_pDocWindow;

//////////////////////////////////////////////////////////////////////
CDocWindow::CDocWindow(CCtp* pCtp) :
	m_Undo(this),
	m_Print(this),
	m_Select(this),
	m_Highlight(this)
{
	m_pCtp = pCtp;
	m_pAGDoc = NULL;
	m_hKeyboardHook = NULL;
	m_pClientDC = NULL;
	m_pProgressDlg = NULL;
	m_ShadowSize.cx = 0;
	m_ShadowSize.cy = 0;
	m_ptCascade.x = 0;
	m_ptCascade.y = 0;
	SetRect(&m_PageRect, 0, 0, 0, 0);
	m_bHasFocus = false;
	m_iZoom = 0;
	m_fZoomScale = 1.0;
	m_bEditMasterLayer = false;
	m_bShowHiddenSymbols = false;
	m_bIgnoreSetCursor = false;
	m_pHelpWindow = NULL;
	m_hUpdateInfoThread = NULL;
	m_pEnvelopeWizard = NULL;
	m_pWebPage = NULL;

    InitializeLinkedDoc();

	m_bMetricMode = !!m_pCtp->GetContextMetric();
	m_bSpecialsEnabled = false;

	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{
		DWORD dwLength = MAX_PATH;
		char strValue[MAX_PATH];
		if (regkey.QueryStringValue(REGVAL_SPECIALSENABLED, strValue, &dwLength) == ERROR_SUCCESS)
			m_bSpecialsEnabled = !!atoi(strValue);

		DWORD dwValue = false;
		if (regkey.QueryDWORDValue(REGVAL_METRIC, dwValue) == ERROR_SUCCESS) 
		{
			bool bMetricMode = !!dwValue;
			if (CMessageBox::Message(String("You have activated an override to always set METRIC MODE to %s.\n\nWould you like to cancel it and use the site default?", (bMetricMode ? "ON" : "OFF")), MB_YESNO) == IDNO)
				m_bMetricMode = bMetricMode;
			else
				regkey.DeleteValue(REGVAL_METRIC);
		}
	}

	CAGDocSheetSize::SetMetric(m_bMetricMode);
}

//////////////////////////////////////////////////////////////////////
CDocWindow::~CDocWindow()
{
	Free();
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	::SetClassLong(m_hWnd, GCL_STYLE, ::GetClassLong(m_hWnd, GCL_STYLE) | CS_DBLCLKS);

	m_pClientDC = new CAGClientDC(m_hWnd);
	m_hKeyboardHook = ::SetWindowsHookEx(WH_KEYBOARD, CDocWindow::CtpKeyboardProc, NULL, GetCurrentThreadId());
	g_pDocWindow = this;

	if (m_pClientDC)
	{ // Calculate the shadow size for the first time
		m_ShadowSize.cx = DEFAULT_SHADOW_SIZE;
		m_ShadowSize.cy = DEFAULT_SHADOW_SIZE;

		// Convert the shadow size (it will end up being about 4 pixels)
		m_pClientDC->GetDeviceMatrix().Transform((POINT*)&m_ShadowSize, 1, false);
	}

	EliminateScrollbars(true/*bRedraw*/);

	Clipboard.Register(m_hWnd);


    // Initialize file menu
	BOOL bOK = m_menuFile.LoadMenu(IDR_POPUPMENU_FILE);
	if (!bOK)
		return E_FAIL;

	m_popupFile = m_menuFile.GetSubMenu(0);
	if (!m_popupFile.IsMenu())
		return E_FAIL;
	

    // Setup MRU
	static CMenuHandle MenuMru = m_popupFile.GetSubMenu(9);
	m_mruList.SetMenuHandle(MenuMru);
	m_mruList.ReadFromRegistry(REGKEY_APP);
	m_mruList.SetMaxEntries(10);


	// Connect to WebBrowser
	GetWebBrowserInterface(&m_spBrowser);
	/*CComPtr<IDispatch> spDisp;
	HRESULT hr;
	if (m_spBrowser)
	{
		SHANDLE hwnd;
		hr = m_spBrowser->get_HWND(&hwnd);
		if (::IsWindow(hwnd))
			::PostMessage(hwnd, UWM_PLUGINREADY, NULL, NULL);
	}*/

	//::PostMessage(HWND_BROADCAST, UWM_PLUGINREADY, NULL, (LPARAM)m_hWnd);

#ifdef NOTUSED //j
	m_ToolTip.Create(m_hWnd);
	m_ToolTip.Activate(true);
	DWORD c = m_ToolTip.GetDelayTime(TTDT_AUTOPOP); // How long if remains stationary
	DWORD a = m_ToolTip.GetDelayTime(TTDT_INITIAL); // before coming up
	DWORD b = m_ToolTip.GetDelayTime(TTDT_RESHOW); // between reappearances
//j	m_ToolTip.SetDelayTime(TTDT_INITIAL, 250);

	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hinst = _AtlBaseModule.GetResourceInstance();
	ti.hwnd = m_hWnd;
	ti.uId = 0;
	::GetClientRect(ti.hwnd, &ti.rect);
	ti.lpszText = "Hello!";
	m_ToolTip.AddTool(&ti);
#endif NOTUSED //j
	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    Close(true/*bExit*/);
	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Free();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::Free()
{
	//Disconnect();
	/*delete m_pWebPage;
	m_pWebPage = NULL;*/

	Close(true/*bExit*/);
    CleanupLinkedDoc(true/*bExit*/);

	g_pDocWindow = NULL;

	if (m_hKeyboardHook)
	{
		::UnhookWindowsHookEx(m_hKeyboardHook);
		m_hKeyboardHook = NULL;
	}

	if (m_pProgressDlg)
	{
		m_pProgressDlg->DestroyWindow();
		delete m_pProgressDlg;
		m_pProgressDlg = NULL;
	}

	if (m_pClientDC)
	{
		delete m_pClientDC;
		m_pClientDC = NULL;
	}

	if (m_hUpdateInfoThread)
	{
		::TerminateThread(m_hUpdateInfoThread, 0/*dwExitCode*/);
		::CloseHandle(m_hUpdateInfoThread);
		m_hUpdateInfoThread = NULL;
	}

    if (m_popupFile)
        m_popupFile.DestroyMenu();

    if (m_menuFile)
        m_menuFile.DestroyMenu();
}
/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::CallJavaScript(LPCTSTR szFunction, LPCTSTR szArg1)
{
	if (NULL == m_spBrowser)
		return false;

	CComPtr<IHTMLDocument2> spHtmlDoc;
	CWebPage WebPage;
	if (!GetHtmlDoc(&spHtmlDoc))
		return false;

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
/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::GetWebBrowserInterface(IWebBrowser2 ** pBrowser)
{
	CComQIPtr<IServiceProvider> spServiceProvider(m_pCtp->m_spClientSite);
	if (NULL == spServiceProvider)
		return NULL;
	
	HRESULT hr = spServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void**)pBrowser);
	if (FAILED(hr))
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::GetHtmlDoc(IHTMLDocument2 ** pDoc)
{	
	CComPtr<IDispatch> spDisp;
	HRESULT hr = m_spBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2> spDoc(spDisp);
	if (FAILED(hr))
		return false;
	
	*pDoc = spDoc.p;
	(*pDoc)->AddRef();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::BeforeNavigate2(IDispatch *pDisp, VARIANT *url, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *Cancel)
{
	delete m_pWebPage;
	m_pWebPage = NULL;
	return;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::DocumentComplete(IDispatch *pDisp, VARIANT *URL)
{
	/*if (NULL != m_pWebPage)
		return;

	CComPtr<IHTMLDocument2> spHtmlDoc;
	if (GetHtmlDoc(&spHtmlDoc))
	{
		m_pWebPage = new CWebPage;
		m_pWebPage->SetDocument(spHtmlDoc);
	}*/
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{ // Process a window 'hide' message as the logical OnClose()
	bHandled = false;

	bool bBeingHidden = !wParam;
	if (!bBeingHidden)
		return S_OK;

//j	if (!m_pAGDoc)
//j		return S_OK;

//j	return Close(true/*bExit*/);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (!hDC)
	{
		bHandled = false;
		return S_OK;
	}

	// We handle it, but do nothing here - take care of it when we paint
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return m_pCtp->OnCtlColor(uMsg, wParam, lParam, bHandled);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//j OnPaint() has palette problems on 256 color displays
//j	maybe select the m_pClientDC's palette into hPaintDC
	PAINTSTRUCT PaintStruct;
	HDC hPaintDC = ::BeginPaint(m_hWnd, &PaintStruct);
	if (!hPaintDC)
		return E_FAIL;

	if (!m_pAGDoc)
	{
		::FillRect(hPaintDC, &PaintStruct.rcPaint, CAGBrush(BROWSER_BACKGOUND_COLOR));

		// Draw the text
		int iOldMode = ::SetBkMode(hPaintDC, TRANSPARENT);
		COLORREF lOldColor = ::SetTextColor(hPaintDC, RGB(0,0,0)); //j ::GetSysColor(COLOR_WINDOWTEXT)

		RECT rect;
		GetClientRect(&rect);
		CString strVersion = String(IDS_MSGBOXTITLE) + " " + VER_PRODUCT_VERSION_STR;
		::DrawText(hPaintDC, strVersion, lstrlen(strVersion), &rect, DT_RIGHT|DT_BOTTOM|DT_SINGLELINE|DT_NOPREFIX);

		::SetBkMode(hPaintDC, iOldMode);
		::SetTextColor(hPaintDC, lOldColor);

		DrawProgressDlg("You may now open a saved project\nor close this workspace window");
		::EndPaint(m_hWnd, &PaintStruct);
		return E_FAIL;
	}

	if (!m_pClientDC)
	{
		::EndPaint(m_hWnd, &PaintStruct);
		return E_FAIL;
	}

	// If the off-screen is 'dirty', redraw the page into it
	if (m_pClientDC->GetDirty())
	{
		HDC hDC = m_pClientDC->GetHDC();

		// Draw the document background and 'furniture'
		{
			// Calculate the border rectangle
			RECT rPageRect = m_PageRect;
			::InflateRect(&rPageRect, 1, 1); // Add the border pixel

			// Before drawing anything, open up the offscreen clipping to include a border pixel and drop shadow
			HRGN hClipRegion = m_pClientDC->SaveClipRegion();
			RECT rClipRect = rPageRect;
			rClipRect.right += m_ShadowSize.cx;
			rClipRect.bottom += m_ShadowSize.cy;
			m_pClientDC->SetClipRect(rClipRect, false/*bIncludeRawDC*/);

			// Draw the page background and the border pixel
			::FillRect(hDC, &rPageRect, CAGBrush(WHITE_BRUSH, eStock));
			::FrameRect(hDC, &rPageRect, CAGBrush(192,192,192)); // Light gray

			// Draw the drop shadow on the right
			RECT r;
			SetRect(&r,
				rPageRect.right,
				rPageRect.top + m_ShadowSize.cy,
				rPageRect.right + m_ShadowSize.cx,
				rPageRect.bottom + m_ShadowSize.cy);
			::FillRect(hDC, &r, CAGBrush(LTGRAY_BRUSH, eStock));

			// Draw the drop shadow on the bottom
			SetRect(&r,
				rPageRect.left + m_ShadowSize.cx,
				rPageRect.bottom,
				rPageRect.right + m_ShadowSize.cx,
				rPageRect.bottom + m_ShadowSize.cy);
			::FillRect(hDC, &r, CAGBrush(LTGRAY_BRUSH, eStock));

			m_pClientDC->RestoreClipRegion(hClipRegion);
		}
				
		CAGPage* pPage = m_pAGDoc->GetCurrentPage();

		// Draw the page
		if (pPage)
			pPage->Draw(*m_pClientDC, GetHiddenFlag());

		// Draw the non-printable margins for this page
		RECT Margins = {0,0,0,0};
		m_Print.GetMargins(m_pAGDoc, Margins);
		Margins.left   = dtoi(m_pClientDC->GetViewToDeviceMatrix().TransformWidth(Margins.left));
		Margins.top    = dtoi(m_pClientDC->GetViewToDeviceMatrix().TransformHeight(Margins.top));
		Margins.right  = dtoi(m_pClientDC->GetViewToDeviceMatrix().TransformWidth(Margins.right));
		Margins.bottom = dtoi(m_pClientDC->GetViewToDeviceMatrix().TransformHeight(Margins.bottom));
		CAGBrush Brush(245,245,245);
		RECT r;
		r = m_PageRect; r.right = r.left + Margins.left;
		::FillRect(hDC, &r, Brush);
		r = m_PageRect; r.left = r.right - Margins.right;
		::FillRect(hDC, &r, Brush);
		r = m_PageRect; r.bottom = r.top + Margins.top;
		::FillRect(hDC, &r, Brush);
		r = m_PageRect; r.top = r.bottom - Margins.bottom;
		::FillRect(hDC, &r, Brush);

		// Draw the selected symbol
		m_Select.Paint(hDC);
		m_Highlight.Paint(hDC);

		// Draw the grid
		if (pPage)
		{
			SIZE PageSize = {0,0};
			pPage->GetPageSize(PageSize);
			m_Grid.Draw(hDC, m_pClientDC->GetViewToDeviceMatrix(), PageSize);
		}

		m_pClientDC->SetDirty(false);
	}

	// Blt from the offscreen to the screen
	m_Select.ShowTextSelection(hPaintDC, false/*bCaretOnly*/, false);
	m_pClientDC->Blt(hPaintDC, PaintStruct.rcPaint);
	m_Select.ShowTextSelection(hPaintDC, false/*bCaretOnly*/, true);

	::EndPaint(m_hWnd, &PaintStruct);

	return S_OK;
}

// EVENTS
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nChar = wParam;

	// See if the select object wants the event
	if (m_Select.KeyChar(nChar))
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nChar = wParam;

	// See if the select object wants the event
	if (m_Select.KeyDown(nChar))
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nChar = wParam;

	// See if the select object wants the event
	if (m_Select.KeyUp(nChar))
		return S_OK;

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	// See if the select object wants the event
	if (m_Select.ButtonDblClick(x, y))
		return S_OK;


	CallJavaScript(_T("OnCommand"), _T("TB_FILE")); // TEST - jhc
	
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool bTakingFocus = (GetFocus() != m_hWnd);
	if (bTakingFocus)
		SetFocus();

	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	SetCapture();

	if (!m_pAGDoc)
		return E_FAIL;

	if (!m_pClientDC)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return E_FAIL;

	POINT pt = {x, y};
	m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(pt);
	CAGSym* pSym = pActiveLayer->FindSymbolByPoint(pt, ST_ANY, GetHiddenFlag());

	if (pSym && pSym->IsCalendar())
	{ // make sure the main panel is selected
		CAGSymCalendar* pCalendar = (CAGSymCalendar*)pSym;
		if (pCalendar->GetMode() != -1)
		{
			pCalendar->SetMode(-1);
			m_pCtp->SelectPanel(CALENDAR_PANEL);
		}
	}

	// See if the select object wants the event
	if (m_Select.ButtonDown(x, y, pSym))
		return S_OK;

	if (!pSym)
	{
		SymbolUnselect(true/*bClearPointer*/);
		return E_FAIL;
	}

	POINT ClickPoint = {x, y};
	m_Select.SymbolSelect(pSym, &ClickPoint);

	// Give the newly selected symbol the down event so it can be moved
	if (!(m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled())))
		m_Select.ButtonDown(x, y, pSym);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	// See if the select object wants the event
	if (m_Select.MouseMove(x, y))
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	ReleaseCapture();

	// See if the select object wants the event
	if (m_Select.ButtonUp(x, y))
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CMenu Menu;
	bool bOK = !!Menu.LoadMenu(SpecialsEnabled() ? IDR_POPUPMENU_EX : IDR_POPUPMENU);
	if (!bOK)
		return E_FAIL;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return E_FAIL;

	// Make sure the menu items are set correctly
	bool bAllowNewPages = (m_pAGDoc ? (m_pAGDoc->AllowNewPages() | (SpecialsEnabled() ? m_pAGDoc->NeedsCardBack() : false)) : false);
	bool bSymSelected = m_Select.SymSelected();

	Popup.EnableMenuItem(1/*Add*/,              MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(2/*Zoom*/,             MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(3/*Transform*/,        MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(4/*Position*/,         MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(5/*Layers*/,           MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(6/*Select*/,           MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(7/*Size*/,             MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(8/*Page*/,             MF_BYPOSITION | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(29/*Convert to JPG*/,  MF_BYPOSITION | ((m_pAGDoc && m_Select.SymIsImage() && m_Select.GetImgNativeType() != "JPG") ? MF_ENABLED : MF_GRAYED));

	Popup.EnableMenuItem(IDC_DOC_NEW,           MF_BYCOMMAND | MF_ENABLED);
	Popup.EnableMenuItem(IDC_DOC_OPEN,          MF_BYCOMMAND | MF_ENABLED);
	Popup.EnableMenuItem(IDC_DOC_CLOSE,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SAVE,          MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SAVEAS,        MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SAVEAS_PDF,    MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SAVEAS_XML,    MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_PRINT,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_PRINT_PREVIEW, MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_CONVERT,       MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_PROPERTIES,    MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));

	Popup.EnableMenuItem(IDC_DOC_PAGEADD,       MF_BYCOMMAND | (m_pAGDoc && bAllowNewPages ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_PAGEDELETE,    MF_BYCOMMAND | (m_pAGDoc && bAllowNewPages ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_PAGEPREV,      MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_PAGENEXT,      MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
		
	Popup.EnableMenuItem(IDC_DOC_ZOOMIN,        MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ZOOMOUT,       MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ZOOMFULL,      MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ZOOM1,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ZOOM2,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ZOOM3,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ZOOM4,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ZOOM5,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));

	Popup.EnableMenuItem(IDC_DOC_BACKONE,       MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_FORWARDONE,    MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_BRINGTOFRONT,  MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_PUSHTOBACK,    MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));

	Popup.EnableMenuItem(IDC_DOC_POSCENTER,     MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_POSCENTERH,    MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_POSCENTERV,    MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_POSLEFT,       MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_POSRIGHT,      MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_POSTOP,        MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_POSTOPLEFT,    MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_POSTOPRIGHT,   MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_POSBOTTOM,     MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_POSBOTTOMLEFT, MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_POSBOTTOMRIGHT,MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));

	Popup.EnableMenuItem(IDC_DOC_MAXIMIZE,      MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_MAXWIDTH,      MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_MAXHEIGHT,     MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));

	Popup.EnableMenuItem(IDC_DOC_ADDIMAGE,      MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ADDAPHOTO,     MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_FLIP,          MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_MIRROR,        MF_BYCOMMAND | (m_pAGDoc && !(m_Select.SymIsText() || m_Select.SymIsCalendar()) ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ROTATECW,      MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ROTATECCW,     MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_LOCKED,		MF_BYCOMMAND | (m_pAGDoc && m_Select.SymIsImage() ? MF_ENABLED : MF_GRAYED));
	Popup.CheckMenuItem(IDC_DOC_LOCKED,		    MF_BYCOMMAND | (m_pAGDoc && m_Select.SymIsImage() && m_Select.SymIsLocked() ? MF_CHECKED : MF_UNCHECKED));
	Popup.EnableMenuItem(IDC_DOC_TRANSPARENT,   MF_BYCOMMAND | (m_pAGDoc && m_Select.SymIsImage() ? MF_ENABLED : MF_GRAYED));
	Popup.CheckMenuItem(IDC_DOC_TRANSPARENT,    MF_BYCOMMAND | (m_pAGDoc && m_Select.SymIsImage() && (m_Select.GetImageSym()->GetTransparentColor() != CLR_NONE) ? MF_CHECKED : MF_UNCHECKED));

	Popup.EnableMenuItem(IDC_DOC_ADDTEXTBOX,    MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SPELLCHECK,    MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_DOWNLOADFONTS, MF_BYCOMMAND | MF_ENABLED);
	Popup.EnableMenuItem(IDC_DOC_DOWNLOADSPELLCHECK, MF_BYCOMMAND | MF_ENABLED);

	Popup.EnableMenuItem(IDC_DOC_ADDSHAPE,      MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ADDCALENDAR,   MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));

	Popup.EnableMenuItem(IDC_CONVERT_TO_JPG,    MF_BYCOMMAND | (m_pAGDoc && m_Select.SymIsImage() ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_MASTERLAYER,   MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.CheckMenuItem(IDC_DOC_MASTERLAYER,    MF_BYCOMMAND | (m_bEditMasterLayer ? MF_CHECKED : MF_UNCHECKED));
	Popup.EnableMenuItem(IDC_DOC_METRIC,        MF_BYCOMMAND | MF_ENABLED);
	Popup.CheckMenuItem(IDC_DOC_METRIC,         MF_BYCOMMAND | (m_bMetricMode ? MF_CHECKED : MF_UNCHECKED));
	Popup.EnableMenuItem(IDC_DOC_GRID,          MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.CheckMenuItem(IDC_DOC_GRID,           MF_BYCOMMAND | (m_Grid.IsOn() ? MF_CHECKED : MF_UNCHECKED));
	Popup.EnableMenuItem(IDC_DOC_CUT,           MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_COPY,          MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_PASTE,         MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_DELETE,        MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_TRANSFORM,     MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_CROP,          MF_BYCOMMAND | (m_pAGDoc && m_Select.SymIsImage() ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_UNDO,          MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_REDO,          MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SELECTALL,     MF_BYCOMMAND | (m_pAGDoc && (m_Select.SymIsText() || (m_Select.SymIsAddAPhoto() && SpecialsEnabled())) ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SELECTPREV,    MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_SELECTNEXT,    MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_GIVEGIFT,          MF_BYCOMMAND | (m_pAGDoc && m_pAGDoc->NeedsCardBack() ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_ARTGALLERY,    MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_DOC_HIDDEN,		MF_BYCOMMAND | (m_pAGDoc && bSymSelected ? MF_ENABLED : MF_GRAYED));
	Popup.CheckMenuItem(IDC_DOC_HIDDEN,			MF_BYCOMMAND | (m_pAGDoc && bSymSelected && m_Select.GetSym()->IsHidden() ? MF_CHECKED : MF_UNCHECKED));
	Popup.EnableMenuItem(IDC_DOC_SHOWHIDDEN,	MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.CheckMenuItem(IDC_DOC_SHOWHIDDEN,		MF_BYCOMMAND | (m_bShowHiddenSymbols ? MF_CHECKED : MF_UNCHECKED));
	Popup.EnableMenuItem(IDC_FILEDOWNLOAD,      MF_BYCOMMAND | (m_pAGDoc ? MF_ENABLED : MF_GRAYED));
	Popup.EnableMenuItem(IDC_ABOUT,             MF_BYCOMMAND | MF_ENABLED);

	bool bAllowEnvelopeWizard = (m_pAGDoc && m_pAGDoc->AllowEnvelopeWizard());
	Popup.EnableMenuItem(IDC_DOC_ENVELOPE_WIZ,  MF_BYCOMMAND | (bAllowEnvelopeWizard || m_pEnvelopeWizard ? MF_ENABLED : MF_GRAYED));

	POINT point;
	::GetCursorPos(&point);
	m_bIgnoreSetCursor = true;
	Popup.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, m_hWnd);
	Menu.DestroyMenu();
	m_bIgnoreSetCursor = false;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bIgnoreSetCursor)
	{
		bHandled = false;
		return E_FAIL;
	}

	POINT pts;
	::GetCursorPos(&pts);
	::ScreenToClient(m_hWnd, &pts);
	RECT rect;
	GetClientRect(&rect);
	if (pts.x < rect.left || pts.x >= rect.right ||
		pts.y < rect.top  || pts.y >= rect.bottom)
	{
		bHandled = false;
		return E_FAIL;
	}

	int x = pts.x;
	int y = pts.y;

	if (!m_pAGDoc)
		return E_FAIL;

	if (!m_pClientDC)
		return E_FAIL;

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (!pPage)
		return E_FAIL;

	CAGLayer* pActiveLayer = pPage->GetActiveLayer();
	if (!pActiveLayer)
		return E_FAIL;

	POINT pt = {x, y};
	m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(pt);
	CAGSym* pSym = pActiveLayer->FindSymbolByPoint(pt, ST_ANY, GetHiddenFlag());

	// See if the select object wants the event
	if (m_Select.SetCursor(x, y, pSym))
		return S_OK;

	if (!pSym)
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	else
	if (pSym->IsText() || (pSym->IsAddAPhoto() && SpecialsEnabled()))
		::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
	else
	if (pSym->IsCalendar())
		::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(ID_HAND_CAL)));
	else
	if (pSym->IsImage())
		::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(ID_HAND_IM)));
	else
	if (pSym->IsLine())
		::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(ID_HAND_LI)));
	else
	if (pSym->IsGraphic())
	{
		bool bIsUnfilled = (((CAGSymGraphic*)pSym)->GetFillType() == FT_None);
		if (pSym->IsEllipse())
			::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(bIsUnfilled ? ID_HAND_UE : ID_HAND_FE)));
		else
		if (pSym->IsRectangle())
			::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(bIsUnfilled ? ID_HAND_UR : ID_HAND_FR)));
		else
			::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(bIsUnfilled ? ID_HAND_UR : ID_HAND_FR)));
	}
	else
	{
		::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(ID_HAND)));
	}

#ifdef HTMLDOC//j
	// Use the HTML window to display status
	IHTMLWindow2* pHTMLWin = m_pCtp->GetHTMLWin();
	if (strStatus && pHTMLWin)
		pHTMLWin->put_status(CComBSTR(strStatus));

	IHTMLDocument2* pHTMLDoc = m_pCtp->GetHTMLDoc();
	if (pHTMLDoc)
	{
		CComBSTR strTitle;
		pHTMLDoc->get_title(&strTitle);
		strTitle = "Jim's Test";
		pHTMLDoc->put_title(strTitle);
	}
#endif HTMLDOC//j

	return S_OK;
}

#ifdef SYSCOLORTEST //j
	#define MyB(rgb)	((BYTE)(rgb))
	#define MyG(rgb)	((BYTE)(((WORD)(rgb)) >> 8))
	#define MyR(rgb)	((BYTE)((rgb)>>16))
	#define MYRGB(hex)	RGB(MyR(hex), MyG(hex), MyB(hex))

	COLORREF NewColor[] = {
		MYRGB(0x4682B4), // steelblue
		MYRGB(0x041690), // royalblue
		MYRGB(0x6495ED), // cornflowerblue
		MYRGB(0xB0C4DE), // lightsteelblue
		MYRGB(0x7B68EE), // mediumslateblue
	};
	int NewIndex[] = {
		COLOR_BTNFACE,
		COLOR_BTNSHADOW,
		COLOR_BTNTEXT,
		COLOR_BTNHIGHLIGHT,
		COLOR_MENU,
	};
	COLORREF OldColor[25];
	int OldIndex[25];
	bool bBackup;
#endif SYSCOLORTEST //j

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bHasFocus = false;

#ifdef SYSCOLORTEST
	// Restore the system colors
	if (bBackup)
	{
		int iCount = sizeof(OldColor)/sizeof(COLORREF);
		::SetSysColors(iCount, OldIndex, OldColor);
	}
#endif SYSCOLORTEST

	if (m_Select.KillFocus())
		return S_OK;

	return S_OK;
}

#define DO_HIGHLIGHT

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::DoDropImage(LPCSTR strFileName, POINTL Pt)
{
#ifdef DO_HIGHLIGHT
	CAGSym* pDragSym = GetSymUnderPt(Pt);
	m_Highlight.DoHighlight(Pt.x, Pt.y, pDragSym, true);
#endif DO_HIGHLIGHT
	AddImage(strFileName, 0/*iOffset*/, Pt.x, Pt.y, true);
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::DoDragOver(POINTL Pt)
{
#ifdef DO_HIGHLIGHT
	if (m_pEnvelopeWizard)
		return;

	CAGSym* pDragSym = GetSymUnderPt(Pt);
	m_Highlight.DoHighlight(Pt.x, Pt.y, pDragSym, false);
#endif DO_HIGHLIGHT
}

///////////////////////////////////////////////////////////////////////////////
void CDocWindow::DoDragLeave()
{
#ifdef DO_HIGHLIGHT
	if (m_pEnvelopeWizard)
		return;

	m_Highlight.DoHighlight(0, 0, NULL, false);
#endif DO_HIGHLIGHT
}

///////////////////////////////////////////////////////////////////////////////
CAGSym* CDocWindow::GetSymUnderPt(POINTL Pt)
{
	CAGSym* pSym = NULL;
	POINT pt;
	pt.x = Pt.x;
	pt.y = Pt.y;

	::ScreenToClient(m_hWnd, &pt);
	m_pClientDC->GetViewToDeviceMatrix().Inverse().Transform(pt);

	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (pPage)
	{
		CAGLayer* pActiveLayer = pPage->GetActiveLayer();
		if (pActiveLayer)
		{
			pSym = pActiveLayer->FindSymbolByPoint(pt, ST_ANY, GetHiddenFlag());
			return pSym;
		}
	}

	return pSym;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bHasFocus = true;
	g_pDocWindow = this;

#ifdef SYSCOLORTEST
	// Backup the system colors
	if (!bBackup)
	{
		for (int i = 0 ; i < sizeof(OldColor)/sizeof(COLORREF); i++)
		{
			int iIndex = COLOR_SCROLLBAR + i;
			OldIndex[i] = iIndex;
			OldColor[i] = ::GetSysColor(iIndex);
		}

		bBackup = true;
	}

	// Set the system colors
	if (bBackup)
	{
		int iCount = sizeof(NewColor)/sizeof(COLORREF);
		SetSysColors(iCount, NewIndex, NewColor);
	}
#endif SYSCOLORTEST

	if (m_Select.SetFocus())
		return S_OK;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT ScrollCode = LOWORD(wParam);
	UINT TrackCode = HIWORD(wParam);
	// SB_LINERIGHT, SB_LINEDOWN	Scrolls one line right/down
	// SB_LINELEFT, SB_LINEUP		Scrolls one line up/left
	// SB_PAGERIGHT, SB_PAGEDOWN	Scrolls one page right/down
	// SB_PAGELEFT, SB_PAGEUP		Scrolls one page left/up
	// SB_LEFT, SB_TOP				Scrolls to the start
	// SB_RIGHT, SB_BOTTOM			Scrolls to the end
	// SB_ENDSCROLL					Ends scroll
	// SB_THUMBPOSITION				The user has dragged the scroll box (thumb) and released the mouse button
	//								The high-order word indicates the position of the scroll box at the end of the drag operation
	// SB_THUMBTRACK				The user is dragging the scroll box
	//								This message is sent repeatedly until the user releases the mouse button
	//								The high-order word indicates the position that the scroll box has been dragged to

	int nMin = 0;
	int nMax = 0;
	if (uMsg == WM_HSCROLL)
	{
		GetScrollRange(SB_HORZ, &nMin, &nMax);
		int nPage = GetScrollPage(SB_HORZ);
		int nPos = GetScrollPos(SB_HORZ);
		switch (ScrollCode)
		{
			case SB_LINERIGHT:
			case SB_LINELEFT:
			{
				if (SB_LINERIGHT == ScrollCode)
					nPos += 100;
				else
					nPos -= 100;
				break;
			}
			case SB_PAGERIGHT:
			case SB_PAGELEFT:
			{
				if (SB_PAGERIGHT == ScrollCode)
					nPos += nPage;
				else
					nPos -= nPage;
				break;
			}
			case SB_RIGHT:
			case SB_LEFT:
			{
				if (SB_RIGHT == ScrollCode)
					nPos = nMax;
				else
					nPos = nMin;

				break;
			}
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
			{
				nPos = TrackCode;
				break;
			}

			default:
				return S_OK;
		}

		if (nPos > nMax + 1 - nPage)
			nPos = nMax + 1 - nPage;
		if (nPos < nMin)
			nPos = nMin;

		SetScrollPos(SB_HORZ, nPos, true);
	}
	else
	if (uMsg == WM_VSCROLL)
	{
		GetScrollRange(SB_VERT, &nMin, &nMax);
		int nPage = GetScrollPage(SB_VERT);
		int nPos = GetScrollPos(SB_VERT);
		switch (ScrollCode)
		{
			case SB_LINEDOWN:
			case SB_LINEUP:
			{
				if (SB_LINEDOWN == ScrollCode)
					nPos += 100;
				else
					nPos -= 100;
				break;
			}
			case SB_PAGEDOWN:
			case SB_PAGEUP:
			{
				if (SB_PAGEDOWN == ScrollCode)
					nPos += nPage;
				else
					nPos -= nPage;
				break;
			}
			case SB_BOTTOM:
			case SB_TOP:
			{
				if (SB_BOTTOM == ScrollCode)
					nPos = nMax;
				else
					nPos = nMin;

				break;
			}
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
			{
				nPos = TrackCode;
				break;
			}

			default:
				return S_OK;
		}

		if (nPos > nMax + 1 - nPage)
			nPos = nMax + 1 - nPage;
		if (nPos < nMin)
			nPos = nMin;

		SetScrollPos(SB_VERT, nPos, true);
	}

	SetupPageView(false/*bSetScrollbars*/, false/*bOffset*/);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_Select.Timer(wParam))
		return S_OK;

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDocWindow::OnPluginReady(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CallJavaScript(_T("PahPluginReady"));	
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
void CDocWindow::UpdateInfo()
{
	if (m_hUpdateInfoThread)
	{
		::TerminateThread(m_hUpdateInfoThread, 0/*dwExitCode*/);
		::CloseHandle(m_hUpdateInfoThread);
		m_hUpdateInfoThread = NULL;
	}

	if (!SpecialsEnabled())
		return;

	if (!m_pAGDoc)
		return;

	if (!m_pClientDC)
		return;

	DWORD dwThreadID = 0;
	m_hUpdateInfoThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UpdateInfoProc, (void*)this, 0, &dwThreadID);
}

/////////////////////////////////////////////////////////////////////////////
DWORD WINAPI CDocWindow::UpdateInfoProc(void* pVoid)
{
	CDocWindow* pDocWindow = (CDocWindow*)pVoid;
	if (!pDocWindow)
		return 0;

	return pDocWindow->UpdateInfoProc();
}

/////////////////////////////////////////////////////////////////////////////
DWORD CDocWindow::UpdateInfoProc()
{
	if (!m_pAGDoc)
		return 0;

	CInfoPanel* pInfoPanel = m_pCtp->GetInfoPanel();
	if (!pInfoPanel)
		return 0;

	CAGSym* pSym = m_Select.GetSym();
	CString strInfo;
	if (!pSym || pSym->IsHidden(GetHiddenFlag()))
	{
		SIZE PageSize = {0,0};
		CAGPage* pPage = m_pAGDoc->GetCurrentPage();
		if (pPage)
			pPage->GetPageSize(PageSize);

		CString strFullDocPath;
		CString strPath;
		CString strFileName;
		m_pAGDoc->GetFileName(strFullDocPath, strPath, strFileName);

		strInfo.Format(" %s\t%s\tName %s       Width Height %s\" %s\"       Version %s",
			(CAGDocSheetSize::GetMetric() ? "METRIC" : "USA"),
			CAGDocTypes::Name(m_pAGDoc->GetDocType()),
			strFileName,
			dtoa(DINCHES(PageSize.cx), 3),
			dtoa(DINCHES(PageSize.cy), 3),
			VER_PRODUCT_VERSION_STR);
	}
	else
	{
		CString strType = pSym->GetTypeName();
		RECT DestRect = pSym->GetDestRectTransformed();

		strInfo.Format(" %s\t%s\tWidth Height %s\" %s\"       Left Top %s\" %s\"       Right Bottom %s\" %s\"",
			(CAGDocSheetSize::GetMetric() ? "METRIC" : "USA"),
			strType,
			dtoa(DINCHES(WIDTH(DestRect)), 3),
			dtoa(DINCHES(HEIGHT(DestRect)), 3),
			dtoa(DINCHES(DestRect.left), 3),
			dtoa(DINCHES(DestRect.top), 3),
			dtoa(DINCHES(DestRect.right), 3),
			dtoa(DINCHES(DestRect.bottom), 3));

		if (pSym->IsImage())
		{
			CAGSymImage* pImage = (CAGSymImage*)pSym;
			BITMAPINFOHEADER* pDIB = pImage->GetDib();
			if (pDIB)
			{
				CString strImageInfo;
				strImageInfo.Format("\r\n\t\tBit Depth %ld       Pixels %d x %d       Resolution %s",
					DibBitCount(pDIB),
					DibWidth(pDIB),
					DibHeight(pDIB),
					dtoa((double)DibWidth(pDIB) / (DINCHES(WIDTH(DestRect))), 0)); //j DibResolutionX(pDIB));
				strInfo += strImageInfo;

				CString strNativeType = pImage->GetNativeType();
				DWORD dwCompressedSize = pImage->GetCompressedSize();
				if (!dwCompressedSize)
				{
					// This operation is slow, so let's output what we have for now
					pInfoPanel->UpdateInfo(strInfo);

					CAGDocIO DocIO((ostream*)NULL);
					DocIO.Write(pDIB, DibSize(pDIB));
					DocIO.Close();
					dwCompressedSize = DocIO.GetOutputCount();
					pImage->SetCompressedSize(dwCompressedSize);
				}

				CString strFileSize;
				strFileSize.Format("       Size %dK       Compression %s to 1 %s",
					dtoi((double)dwCompressedSize / 1024),
					dtoa((double)DibSize(pDIB) / dwCompressedSize, 1),
					strNativeType);
				strInfo += strFileSize;

				COLORREF TransparentColor = pImage->GetTransparentColor();
				CString strTransparent;
				if (TransparentColor != CLR_NONE)
				{
					strTransparent.Format("       Transparent RGB %d %d %d",
						GetRValue(TransparentColor),
						GetGValue(TransparentColor),
						GetBValue(TransparentColor));
					strInfo += strTransparent;
				}
			}
		}
	}
	
	pInfoPanel->UpdateInfo(strInfo);
	return 0;
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::DisplaySavedFileSize()
{
	if (m_hUpdateInfoThread)
	{
		::TerminateThread(m_hUpdateInfoThread, 0/*dwExitCode*/);
		::CloseHandle(m_hUpdateInfoThread);
		m_hUpdateInfoThread = NULL;
	}

	if (!m_pAGDoc)
		return;

	if (!m_pClientDC)
		return;

	DWORD dwThreadID = 0;
	m_hUpdateInfoThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DisplaySavedFileSizeProc, (void*)this, 0, &dwThreadID);
}

/////////////////////////////////////////////////////////////////////////////
DWORD WINAPI CDocWindow::DisplaySavedFileSizeProc(void* pVoid)
{
	CDocWindow* pDocWindow = (CDocWindow*)pVoid;
	if (!pDocWindow)
		return 0;

	return pDocWindow->DisplaySavedFileSizeProc();
}

/////////////////////////////////////////////////////////////////////////////
DWORD CDocWindow::DisplaySavedFileSizeProc()
{
	if (!m_pAGDoc)
		return 0;

	if (SpecialsEnabled() && m_pAGDoc->NeedsCardBack())
		m_pCtp->ShowHideCardBack(false/*bShow*/);

	DWORD dwCompressedSize = m_pAGDoc->GetCompressedSize();

	CString strInfo;
	strInfo.Format("This project will be %ldK (%ld bytes) when saved.", dtoi((double)dwCompressedSize / 1024), dwCompressedSize);
	CMessageBox::Message(strInfo);
	
	if (SpecialsEnabled() && m_pAGDoc->NeedsCardBack())
		m_pCtp->ShowHideCardBack(true/*bShow*/);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::PageInvalidate()
{
	if (!m_pClientDC)
		return;

	RECT rPageRect = m_PageRect;
	::InflateRect(&rPageRect, 1, 1); // Add the border pixel
	rPageRect.right += m_ShadowSize.cx; // Add the drop shadow
	rPageRect.bottom += m_ShadowSize.cy;
	::InvalidateRect(m_hWnd, &rPageRect, false);
	m_pClientDC->SetDirty(true);
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::Repaint(const RECT& Rect)
{
	RECT rect = m_PageRect;
	::InflateRect(&rect, 1, 1); // Add the border pixel
	::IntersectRect(&rect, &rect, &Rect);
	::InvalidateRect(m_hWnd, &rect, false);
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::SymbolInvalidate(CAGSym* pSym)
{
	if (!m_pClientDC)
		return;

	if (!pSym)
		return;

	CAGMatrix MatrixSaved = m_pClientDC->SetSymbolMatrix(pSym->GetMatrix());

	RECT DestRect = pSym->GetDestRect();
	SIZE Size = {0,0};
	pSym->GetDestInflateSize(Size);
	if (!Size.cx && !Size.cy)
		m_pClientDC->GetSymbolToDeviceMatrix().Transform(DestRect);
	else
	{
		pSym->GetMatrix().Transform(DestRect);
		::InflateRect(&DestRect, Size.cx, Size.cy);
		m_pClientDC->GetViewToDeviceMatrix().Transform(DestRect);
	}

	// In case the symbol is being transformed, add in the handle size
	::InflateRect(&DestRect, H_SIZE, H_SIZE);

	// If this is an image that should cover to the edge, invalidate the whole page
	if (m_pClientDC->GetCoverAllowed() && pSym->IsImage() && ((CAGSymImage*)pSym)->DoCoverDraw())
	{
		DestRect = m_pClientDC->GetViewRect();
		m_pClientDC->GetViewToDeviceMatrix().Transform(DestRect);
	}

	Repaint(DestRect);

	m_pClientDC->SetDirty(true);

	m_pClientDC->SetSymbolMatrix(MatrixSaved);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::IsProgressDlgUp()
{
	return m_pProgressDlg != NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::DrawProgressDlg(LPCSTR pMessage, bool bCreateIfNeeded)
{
	if (!bCreateIfNeeded && !m_pProgressDlg)
		return;

	if (pMessage)
	{
		if (*pMessage == '\0') // Does the caller want the default message?
			pMessage = "Waiting to open a project...";
		if (!m_pProgressDlg)
		{
			if (!::IsWindow(m_hWnd))
				return;
			m_pProgressDlg = new CProgressDlg(m_pCtp);
			if (!m_pProgressDlg)
				return;
			m_pProgressDlg->Create(m_hWnd);
		}

		m_pProgressDlg->SetProgressText(pMessage);
		m_pProgressDlg->CenterInParent();
		UpdateWindow(); // Update the entire DocWindow
	}
	else
	{
		if (m_pProgressDlg)
		{
			m_pProgressDlg->DestroyWindow();
			delete m_pProgressDlg;
			m_pProgressDlg = NULL;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::ActivateNewDoc()
{
	EliminateScrollbars(true/*bRedraw*/);

	DrawProgressDlg(NULL);

	m_Grid.On(false);
	m_bEditMasterLayer = false;
	m_bShowHiddenSymbols = false;

	if (GetFocus() != m_hWnd)
		SetFocus();

	m_Print.ActivateNewDoc();

	ActivateNewPage();
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::ActivateNewPage()
{
	if (!m_pAGDoc)
		return;

	SymbolUnselect(true/*bClearPointer*/);

	// Unzoom the view (if not a developer)
	if (!SpecialsEnabled())
	{
		m_iZoom = 0;
		m_fZoomScale = 1.0;
	}

	SetupPageView(true/*bSetScrollbars*/, false/*bOffset*/);

#ifdef NOTUSED //j
	// Select the first text symbol we can find
	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (pPage)
	{
		CAGLayer* pActiveLayer = pPage->GetActiveLayer();
		if (pActiveLayer)
		{
			pSym = pActiveLayer->FindFirstSymbolByType(ST_TEXT, GetHiddenFlag());
			if (pSym)
				m_Select.SymbolSelect(pSym);
		}
	}
#endif NOTUSED //j

	UpdateInfo();

	// Start the cascade point all over again
	m_ptCascade.x = 0;
	m_ptCascade.y = 0;
}

/////////////////////////////////////////////////////////////////////////////
void CDocWindow::SetupPageView(bool bSetScrollbars, bool bOffset)
{
	#define WHITE_BORDER_X 10
	#define WHITE_BORDER_Y 10

	if (!m_hWnd)
		return;

	if (!m_pAGDoc)
		return;

	if (!m_pClientDC)
		return;

	// Suspend any selection tracking
	m_Select.SuspendTracking();

	// Clear the old page from the offscreen DC
	m_pClientDC->Clear();

	// Invalidate the old page rect (m_PageRect) before we change it
	PageInvalidate();

	// Now figure out how much window area we have for the document display
	// This code assumes that the window has no non-client borders
	RECT WndRect;
	GetWindowRect(&WndRect);
	::OffsetRect(&WndRect, -WndRect.left, -WndRect.top);
	WndRect.right -= GetSystemMetrics(SM_CXVSCROLL);
	WndRect.bottom -= GetSystemMetrics(SM_CYHSCROLL);
	
	// Subtract some white space
	WndRect.left   += WHITE_BORDER_X;
	WndRect.top    += WHITE_BORDER_Y;
	WndRect.right  -= WHITE_BORDER_X;
	WndRect.bottom -= WHITE_BORDER_Y;

	// Convert to Document coordinates
	m_pClientDC->GetDeviceMatrix().Inverse().Transform(WndRect);

	int xVisible = WIDTH(WndRect);
	int yVisible = HEIGHT(WndRect);

	// Time to get the page rectangle
	SIZE PageSize = {0,0};
	CAGPage* pPage = m_pAGDoc->GetCurrentPage();
	if (pPage)
		pPage->GetPageSize(PageSize);
	SIZE PageSizeEx = {PageSize.cx + DEFAULT_SHADOW_SIZE, PageSize.cy + DEFAULT_SHADOW_SIZE};

	// Fit the extended page into the available window space
	double fxPageScale = (double)xVisible / PageSizeEx.cx;
	double fyPageScale = (double)yVisible / PageSizeEx.cy;
	double fFitInWindowScale = min(fxPageScale, fyPageScale);
	if (!fFitInWindowScale) fFitInWindowScale = 1.0;

	// Default the scroll position to the center of the current page
	int xPos = -1;
	int yPos = -1;

	// Special setting to indicate a zoom into the center of the SELECTED OBJECT
	if (m_fZoomScale == -1.0 && m_Select.SymSelected())
	{
		RECT DestRect = m_Select.GetSym()->GetDestRectTransformed();
		int iSymWidth = WIDTH(DestRect);
		if (!iSymWidth) iSymWidth = 1;
		int iSymHeight = HEIGHT(DestRect);
		if (!iSymHeight) iSymHeight = 1;
		double fxSymScale = (double)xVisible / iSymWidth;
		double fySymScale = (double)yVisible / iSymHeight;
		double fFitSymScale = min(fxSymScale, fySymScale);
		m_fZoomScale = fFitSymScale / fFitInWindowScale;
		m_iZoom = dtoi(m_fZoomScale - 1.0);
		
		// Set the (x,y) position to be the center of the symbol
		xPos = (DestRect.left + DestRect.right) / 2;
		yPos = (DestRect.top + DestRect.bottom) / 2;
	}
	
	// Special setting to indicate a zoom into the width of the FULL PAGE
	if (m_fZoomScale <= 0)
	{
		m_fZoomScale = fxPageScale / fFitInWindowScale;
		m_iZoom = dtoi(m_fZoomScale - 1.0);
	}

	// Compute the scale and adjust the visible area
	double fScale = fFitInWindowScale * m_fZoomScale;
	xVisible = dtoi((double)xVisible / fScale);
	yVisible = dtoi((double)yVisible / fScale);

	// If the (x,y) position was set to the symbol center, adjust it by 1/2 the visible area
	if (xPos >= 0 || yPos >= 0)
	{
		xPos -= xVisible / 2;
		yPos -= yVisible / 2;
	}

	// Setup the scrollbars
	if (bSetScrollbars)
		SetupScrollbars(xPos, yPos, PageSizeEx.cx, PageSizeEx.cy, xVisible, yVisible, true/*bRedraw*/);

	// Get the updated position
	xPos = GetScrollPos(SB_HORZ);
	yPos = GetScrollPos(SB_VERT);

	// Calculate a view matrix
	CAGMatrix ViewMatrix;
	ViewMatrix.Translate(-xPos, -yPos);
	ViewMatrix.Scale(fScale, fScale);
	ViewMatrix.Translate(WndRect.left, WndRect.top);
	m_pClientDC->SetViewingMatrix(ViewMatrix);

	// Setup our page view rectangle
	m_pClientDC->SetClipToView(PageSize, true/*bIncludeRawDC*/, &m_PageRect);

	// Invalidate the new page rect (m_PageRect) now that we've changed it
	PageInvalidate();

	// Resume any selection tracking
	m_Select.ResumeTracking();
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::SetupScrollbars(int xPos, int yPos, int xSize, int ySize, int xVisible, int yVisible, bool bRedraw)
{
	// If no (x,y) position was passed in, set the position to be the center of the current page
	if (xPos < 0 && yPos < 0)
	{
		xPos = GetScrollPos(SB_HORZ) + GetScrollPage(SB_HORZ)/2 - xVisible/2;
		yPos = GetScrollPos(SB_VERT) + GetScrollPage(SB_VERT)/2 - yVisible/2;
	}

	// Set the scroll range
	SetScrollRange(SB_HORZ, 0, xSize-1, false);
	SetScrollRange(SB_VERT, 0, ySize-1, false);

	// Set the scroll page size
	SetScrollPage(SB_HORZ, xVisible, false);
	SetScrollPage(SB_VERT, yVisible, false);

	// Reset the scroll position and be sure it is valid
	ResetScrollPos(SB_HORZ, xPos, bRedraw);
	ResetScrollPos(SB_VERT, yPos, bRedraw);
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::EliminateScrollbars(bool bRedraw)
{
	// Set the scroll range
	SetScrollRange(SB_HORZ, 0, 0/*xSize-1*/, false);
	SetScrollRange(SB_VERT, 0, 0/*ySize-1*/, false);

	// Set the scroll page size
	SetScrollPage(SB_HORZ, 0/*xVisible*/, false);
	SetScrollPage(SB_VERT, 0/*yVisible*/, false);

	// Set the scroll position
	SetScrollPos(SB_HORZ, 0/*xPos*/, bRedraw);
	SetScrollPos(SB_VERT, 0/*yPos*/, bRedraw);
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::ResetScrollPos(int nBar, int nPos, bool bRedraw)
{
	int nMin = 0;
	int nMax = 0;
	GetScrollRange(nBar, &nMin, &nMax);
	if (nPos < nMin)
		nPos = nMin;
	int nPage = GetScrollPage(nBar);
	if (nPos > nMax + 1 - nPage)
		nPos = nMax + 1 - nPage;
	SetScrollPos(nBar, nPos, bRedraw);
}

//////////////////////////////////////////////////////////////////////
void CDocWindow::SetScrollPage(int nBar, UINT nPage, bool bRedraw)
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE;
	si.nPage = nPage;
	SetScrollInfo(nBar, &si, bRedraw);
}

//////////////////////////////////////////////////////////////////////
UINT CDocWindow::GetScrollPage(int nBar)
{
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE;
	si.nPage = 0;
	GetScrollInfo(nBar, &si);
	return si.nPage;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::DoCommand(const CString& strCommand)
{
	if (!m_hWnd)
		return (strCommand == "CLOSE" ? true : false);

	LRESULT lResult = S_FALSE;
	     if (strCommand == "NEW")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_NEW);
	else if (strCommand == "OPEN")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_OPEN);
	else if (strCommand == "CLOSE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_CLOSE);
	else if (strCommand == "SAVE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_SAVE);
	else if (strCommand == "SAVEAS")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_SAVEAS);
	else if (strCommand == "SAVEASPDF")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_SAVEAS_PDF);
	else if (strCommand == "SAVEASXML")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_SAVEAS_XML);
	else if (strCommand == "PRINT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_PRINT);
	else if (strCommand == "HELP")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_HELP);
	else if (strCommand == "CONVERT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_CONVERT);
	else if (strCommand == "PROPERTIES")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_PROPERTIES);
	else if (strCommand == "EXTRAS")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_EXTRAS);

	else if (strCommand == "PAGEADD")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_PAGEADD);
	else if (strCommand == "PAGEDELETE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_PAGEDELETE);
	else if (strCommand == "PAGEPREV")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_PAGEPREV);
	else if (strCommand == "PAGENEXT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_PAGENEXT);
	
	else if (strCommand == "ZOOMIN")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ZOOMIN);
	else if (strCommand == "ZOOMOUT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ZOOMOUT);
	else if (strCommand == "ZOOMFULL")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ZOOMFULL);
	else if (strCommand == "ZOOM1")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ZOOM1);
	else if (strCommand == "ZOOM2")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ZOOM2);
	else if (strCommand == "ZOOM3")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ZOOM3);
	else if (strCommand == "ZOOM4")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ZOOM4);
	else if (strCommand == "ZOOM5")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ZOOM5);

	else if (strCommand == "BACKONE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_BACKONE);
	else if (strCommand == "FORWARDONE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_FORWARDONE);
	else if (strCommand == "BRINGTOFRONT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_BRINGTOFRONT);
	else if (strCommand == "PUSHTOBACK")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_PUSHTOBACK);

	else if (strCommand == "POSCENTER")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSCENTER);
	else if (strCommand == "POSCENTERH")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSCENTERH);
	else if (strCommand == "POSCENTERV")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSCENTERV);
	else if (strCommand == "POSLEFT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSLEFT);
	else if (strCommand == "POSRIGHT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSRIGHT);
	else if (strCommand == "POSTOP")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSTOP);
	else if (strCommand == "POSBOTTOM")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSBOTTOM);
	else if (strCommand == "POSTOPLEFT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSTOPLEFT);
	else if (strCommand == "POSTOPRIGHT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSTOPRIGHT);
	else if (strCommand == "POSBOTTOMLEFT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSBOTTOMLEFT);
	else if (strCommand == "POSBOTTOMRIGHT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_POSBOTTOMRIGHT);

	else if (strCommand == "IMAGEMAXIMIZE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_MAXIMIZE);
	else if (strCommand == "MAXIMIZE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_MAXIMIZE);
	else if (strCommand == "MAXWIDTH")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_MAXWIDTH);
	else if (strCommand == "MAXHEIGHT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_MAXHEIGHT);

	else if (strCommand == "ADDIMAGE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ADDIMAGE);
	else if (strCommand == "ADDAPHOTO")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ADDAPHOTO);
	else if (strCommand == "FLIP")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_FLIP);
	else if (strCommand == "MIRROR")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_MIRROR);
	else if (strCommand == "ROTATECW")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ROTATECW);
	else if (strCommand == "ROTATECCW")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ROTATECCW);
	else if (strCommand == "LOCKED")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_LOCKED);
	else if (strCommand == "TRANSPARENT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_TRANSPARENT);

	else if (strCommand == "ADDTEXTBOX")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ADDTEXTBOX);
	else if (strCommand == "SPELLCHECK")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_SPELLCHECK);
	else if (strCommand == "DOWNLOADFONTS")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_DOWNLOADFONTS);
	else if (strCommand == "DOWNLOADSPELLCHECK")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_DOWNLOADSPELLCHECK);

	else if (strCommand == "ADDSHAPE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ADDSHAPE);
	else if (strCommand == "ADDCALENDAR")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ADDCALENDAR);

	else if (strCommand == "CONVERTJPG")
		lResult = SendMessage(WM_COMMAND, IDC_CONVERT_TO_JPG);
	else if (strCommand == "MASTERLAYER")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_MASTERLAYER);
	else if (strCommand == "METRIC")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_METRIC);
	else if (strCommand == "GRID")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_GRID);
	else if (strCommand == "CUT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_CUT);
	else if (strCommand == "COPY")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_COPY);
	else if (strCommand == "PASTE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_PASTE);
	else if (strCommand == "DELETE")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_DELETE);
	else if (strCommand == "TRANSFORM")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_TRANSFORM);
	else if (strCommand == "CROP")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_CROP);
	else if (strCommand == "UNDO")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_UNDO);
	else if (strCommand == "REDO")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_REDO);
	else if (strCommand == "SELECTALL")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_SELECTALL);
	else if (strCommand == "SELECTPREV")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_SELECTPREV);
	else if (strCommand == "SELECTNEXT")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_SELECTNEXT);
	else if (strCommand == "ARTGALLERY")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ARTGALLERY);
	else if (strCommand == "GIVEGIFT")
		lResult = SendMessage(WM_COMMAND, IDC_GIVEGIFT);
	else if (strCommand == "ENVELOPEWIZ")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_ENVELOPE_WIZ);
    else if (strCommand == "MATCHINGCARD")
		lResult = SendMessage(WM_COMMAND, IDC_DOC_MATCHING_CARD);

    // Toolbar buttons
    else if (strCommand == "TB_FILE")
		lResult = SendMessage(WM_COMMAND, IDC_TB_FILE);
    else if (strCommand == "TB_EDIT")
		lResult = SendMessage(WM_COMMAND, IDC_TB_EDIT);
    else if (strCommand == "TB_VIEW")
		lResult = SendMessage(WM_COMMAND, IDC_TB_VIEW);
    else if (strCommand == "TB_FORMAT")
		lResult = SendMessage(WM_COMMAND, IDC_TB_FORMAT);
    else if (strCommand == "TB_TOOLS")
		lResult = SendMessage(WM_COMMAND, IDC_TB_TOOLS);
    else if (strCommand == "TB_HELP")
		lResult = SendMessage(WM_COMMAND, IDC_TB_HELP);

	else
	{
		CMessageBox::Message(String("Error: '%s' is not a legal command.", strCommand));
	}

	// Always take back the focus after an external command
	SetFocus();

	return (lResult == S_OK ? true : false);
}

/////////////////////////////////////////////////////////////////////////////
DWORD CDocWindow::GetCommand(const CString& strCommand)
{
	// The return value of this function is a set of bitwise flags about the command
	// Bit 1 = Enabled
	// Bit 2 = Checked
	const long Enable = 1;
	const long Disable = 0;
	const long Checked = 2;
	const long Unchecked = 0;
	long lDocDependent = (m_pAGDoc ? Enable : Disable);
	long lSymDependent = (m_pAGDoc && m_Select.SymSelected() ? Enable : Disable);
	long lSymImageDependent = (m_pAGDoc && m_Select.SymIsImage() ? Enable : Disable);
	bool bAllowNewPages = m_pAGDoc->AllowNewPages() | (SpecialsEnabled() ? m_pAGDoc->NeedsCardBack() : 0);
	long lAllowNewPageDependent = (m_pAGDoc && bAllowNewPages ? Enable : Disable);

	LRESULT lResult = S_FALSE;
	     if (strCommand == "NEW")
		lResult = Enable;
	else if (strCommand == "OPEN")
		lResult = Enable;
	else if (strCommand == "CLOSE")
		lResult = lDocDependent;
	else if (strCommand == "SAVE")
		lResult = lDocDependent;
	else if (strCommand == "SAVEAS")
		lResult = lDocDependent;
	else if (strCommand == "SAVEASPDF")
		lResult = lDocDependent;
	else if (strCommand == "SAVEASXML")
		lResult = lDocDependent;
	else if (strCommand == "PRINT")
		lResult = lDocDependent;
	else if (strCommand == "HELP")
		lResult = Enable;
	else if (strCommand == "CONVERT")
		lResult = lDocDependent;
	else if (strCommand == "PROPERTIES")
		lResult = lDocDependent;
	else if (strCommand == "EXTRAS")
		lResult = Enable;

	else if (strCommand == "PAGEADD")
		lResult = lAllowNewPageDependent;
	else if (strCommand == "PAGEDELETE")
		lResult = lAllowNewPageDependent;
	else if (strCommand == "PAGEPREV")
		lResult = lDocDependent;
	else if (strCommand == "PAGENEXT")
		lResult = lDocDependent;
	
	else if (strCommand == "ZOOMIN")
		lResult = lDocDependent;
	else if (strCommand == "ZOOMOUT")
		lResult = lDocDependent;
	else if (strCommand == "ZOOMFULL")
		lResult = lDocDependent;
	else if (strCommand == "ZOOM1")
		lResult = lDocDependent;
	else if (strCommand == "ZOOM2")
		lResult = lDocDependent;
	else if (strCommand == "ZOOM3")
		lResult = lDocDependent;
	else if (strCommand == "ZOOM4")
		lResult = lDocDependent;
	else if (strCommand == "ZOOM5")
		lResult = lDocDependent;

	else if (strCommand == "BACKONE")
		lResult = lSymDependent;
	else if (strCommand == "FORWARDONE")
		lResult = lSymDependent;
	else if (strCommand == "BRINGTOFRONT")
		lResult = lSymDependent;
	else if (strCommand == "PUSHTOBACK")
		lResult = lSymDependent;

	else if (strCommand == "POSCENTER")
		lResult = lSymDependent;
	else if (strCommand == "POSCENTERH")
		lResult = lSymDependent;
	else if (strCommand == "POSCENTERV")
		lResult = lSymDependent;
	else if (strCommand == "POSLEFT")
		lResult = lSymDependent;
	else if (strCommand == "POSRIGHT")
		lResult = lSymDependent;
	else if (strCommand == "POSTOP")
		lResult = lSymDependent;
	else if (strCommand == "POSBOTTOM")
		lResult = lSymDependent;
	else if (strCommand == "POSTOPLEFT")
		lResult = lSymDependent;
	else if (strCommand == "POSTOPRIGHT")
		lResult = lSymDependent;
	else if (strCommand == "POSBOTTOMLEFT")
		lResult = lSymDependent;
	else if (strCommand == "POSBOTTOMRIGHT")
		lResult = lSymDependent;

	else if (strCommand == "IMAGEMAXIMIZE")
		lResult = lSymDependent;
	else if (strCommand == "MAXIMIZE")
		lResult = lSymDependent;
	else if (strCommand == "MAXWIDTH")
		lResult = lSymDependent;
	else if (strCommand == "MAXHEIGHT")
		lResult = lSymDependent;

	else if (strCommand == "ADDIMAGE")
		lResult = lDocDependent;
	else if (strCommand == "ADDAPHOTO")
		lResult = lDocDependent;
	else if (strCommand == "FLIP")
		lResult = lSymDependent;
	else if (strCommand == "MIRROR")
		lResult = lSymDependent;
	else if (strCommand == "ROTATECW")
		lResult = lSymDependent;
	else if (strCommand == "ROTATECCW")
		lResult = lSymDependent;
	else if (strCommand == "LOCKED")
		lResult = lSymImageDependent;
	else if (strCommand == "TRANSPARENT")
		lResult = lSymImageDependent;

	else if (strCommand == "ADDTEXTBOX")
		lResult = lDocDependent;
	else if (strCommand == "SPELLCHECK")
		lResult = lDocDependent;
	else if (strCommand == "DOWNLOADFONTS")
		lResult = Enable;
	else if (strCommand == "DOWNLOADSPELLCHECK")
		lResult = Enable;

	else if (strCommand == "ADDSHAPE")
		lResult = lDocDependent;
	else if (strCommand == "ADDCALENDAR")
		lResult = lDocDependent;

	else if (strCommand == "CONVERTJPG")
		lResult = lSymImageDependent;
	else if (strCommand == "MASTERLAYER")
	{
		lResult = lDocDependent;
		lResult |= (m_pAGDoc && m_bEditMasterLayer ? Checked : Unchecked);
	}
	else if (strCommand == "METRIC")
	{
		lResult |= (m_bMetricMode? Checked : Unchecked);
	}
	else if (strCommand == "GRID")
	{
		lResult = lDocDependent;
		lResult |= (m_pAGDoc && m_Grid.IsOn() ? Checked : Unchecked);
	}
	else if (strCommand == "CUT")
		lResult = lDocDependent;
	else if (strCommand == "COPY")
		lResult = lDocDependent;
	else if (strCommand == "PASTE")
		lResult = lDocDependent;
	else if (strCommand == "DELETE")
		lResult = lSymDependent;
	else if (strCommand == "TRANSFORM")
		lResult = lSymDependent;
	else if (strCommand == "CROP")
		lResult = lSymImageDependent;
	else if (strCommand == "UNDO")
		lResult = lDocDependent;
	else if (strCommand == "REDO")
		lResult = lDocDependent;
	else if (strCommand == "SELECTALL")
		lResult = lSymDependent;
	else if (strCommand == "SELECTPREV")
		lResult = lSymDependent;
	else if (strCommand == "SELECTNEXT")
		lResult = lSymDependent;
	else if (strCommand == "ARTGALLERY")
		lResult = lSymDependent;
	else if (strCommand == "GIVEGIFT")
		lResult = lSymDependent;
	else if (strCommand == "ENVELOPEWIZ")
		lResult = lSymDependent;
	else
	{
		CMessageBox::Message(String("Error: '%s' is not a legal command.", strCommand));
	}

	return lResult;
}

//////////////////////////////////////////////////////////////////////
void AdjustArrowKeyByTransform(const CAGSym* pSym, UINT& nChar, bool bInverse)
{
	if (!pSym)
		return;

	POINT pt[2] = {0,0,0,0};
	if (nChar == VK_LEFT)
		pt[1].x = -100000;
	else
	if (nChar == VK_RIGHT)
		pt[1].x =  100000;
	else
	if (nChar == VK_UP)
		pt[1].y =  100000;
	else
	if (nChar == VK_DOWN)
		pt[1].y = -100000;
	else
		return;

	if (bInverse)
		pSym->GetMatrix().Inverse().Transform(pt, 2);
	else
		pSym->GetMatrix().Transform(pt, 2);

	int dx = pt[1].x - pt[0].x;
	int dy = pt[1].y - pt[0].y;
	int adx = abs(dx);
	int ady = abs(dy);

	bool bLeft  = (dx <= 0 && adx >  ady);
	bool bRight = (dx >  0 && adx >  ady);
	bool bDown  = (dy <= 0 && adx <= ady);
	bool bUp    = (dy >  0 && adx <= ady);

	if (bLeft)
		nChar = VK_LEFT;
	else
	if (bRight)
		nChar = VK_RIGHT;
	else
	if (bUp)
		nChar = VK_UP;
	else
	if (bDown)
		nChar = VK_DOWN;
}
/////////////////////////////////////////////////////////////////////////////
void CDocWindow::InitializeLinkedDoc()
{
    m_LinkedDoc.bModified = false;
    m_LinkedDoc.bSwitchInProgress = false;
    m_LinkedDoc.DocType = DOC_DEFAULT;
    m_LinkedDoc.szCardFilePath = "";
    m_LinkedDoc.szEnvelopeFilePath = "";
    m_LinkedDoc.szFilename = "";

    return;
}
/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::Refresh(LPCTSTR szFilename)
{
    m_pCtp->PutSrcURL(CComBSTR(szFilename));
	m_pCtp->ActivateBrowser();
	m_pCtp->ActivateNewDoc();

    return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::SwitchToEnvelope(bool bAtPrintTime)
{
    if (!m_pAGDoc)
		return false;

    if (!m_pAGDoc->AllowEnvelopeWizard())
		return false;

    AGDOCTYPE DocType = m_pAGDoc->GetDocType();
    if (DOC_ENVELOPE != DocType)
    {
        bool bIsModified =  m_LinkedDoc.bModified;

        // Save greeting card
        if (!SaveToTemp())
             return false;
        
        if (!m_LinkedDoc.szEnvelopeFilePath.IsEmpty() && !bAtPrintTime)
        {
            m_LinkedDoc.bSwitchInProgress = true;
          
            // Close current project.
            if (m_pAGDoc)
		        Close(false/*bExit*/, true /*bSilent*/);

            // Open envelope project
            OpenDoc2(m_LinkedDoc.szEnvelopeFilePath);

            if (m_pAGDoc)
                m_pAGDoc->SetModified(bIsModified);

            m_LinkedDoc.bSwitchInProgress = false;

			if (m_pEnvelopeWizard && ::IsWindow(m_pEnvelopeWizard->m_hWnd))
				m_pEnvelopeWizard->ShowWindow(SW_SHOW);
        }
        else if (m_LinkedDoc.szEnvelopeFilePath.IsEmpty())
        {
            if (m_pEnvelopeWizard)
	        {
		        if (::IsWindow(m_pEnvelopeWizard->m_hWnd))
                    m_pEnvelopeWizard->DestroyWindow();
		        delete m_pEnvelopeWizard;
		        m_pEnvelopeWizard = NULL;
	        }

            if (bAtPrintTime)
            {
	            int iResponse = CMessageBox::Message(String("Would you like to create a matching envelope for this project?"), MB_YESNO);
	            if (iResponse != IDYES)
		            return S_OK;
            }

                        
            CEnvelopeWizard* pEnvelopeWizard = new CEnvelopeWizard(m_hWnd, this, m_pAGDoc, DocType);
	        if (!pEnvelopeWizard)
		        return false;

            // Close current project.
            m_LinkedDoc.bSwitchInProgress = true;
           
            if (m_pAGDoc)
		       Close(false/*bExit*/, true /*bSilent*/);

             if (m_pAGDoc)
	        {
		        delete pEnvelopeWizard;
		        pEnvelopeWizard = NULL;
		        return false;
	        }

            m_LinkedDoc.bSwitchInProgress = false;

            m_pEnvelopeWizard = pEnvelopeWizard;
            m_pEnvelopeWizard->CreateEnvelope(&m_pAGDoc);
            Refresh(_T("Untitled Envelope"));

	        RECT EnvRect;
	        m_pEnvelopeWizard->GetWindowRect(&EnvRect);
	        RECT DocRect;
	        ::GetWindowRect(m_pCtp->GetBrowser(), &DocRect);
	        int nYOffset = (((DocRect.bottom - DocRect.top) - (EnvRect.bottom - EnvRect.top))/2);
	        m_pEnvelopeWizard->SetWindowPos(HWND_TOPMOST, (DocRect.right - (EnvRect.right - EnvRect.left)), (DocRect.top + nYOffset), 0, 0, SWP_NOSIZE);
	        m_pEnvelopeWizard->ShowWindow(SW_SHOW);
        }
    }
    else // DOC_ENVELOPE == DocType
    {
        if (!m_pAGDoc)
            return false;

        if (!bAtPrintTime && m_pEnvelopeWizard && ::IsWindow(m_pEnvelopeWizard->m_hWnd))
        {
			m_pEnvelopeWizard->ShowWindow(SW_SHOW);
        }
    }
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::HasLinkedDoc()
{
	return (m_LinkedDoc.DocType != DOC_DEFAULT);
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::SwitchToCard()
{
    // If current project is not envelope then do not switch. 
    // We assume this is already a card - in the future when we handle
    // more than two relationships.
    if (DOC_ENVELOPE != m_pAGDoc->GetDocType() || m_LinkedDoc.szCardFilePath.IsEmpty())
       return false;

    bool bIsModified =  m_LinkedDoc.bModified;

    // Save envelope
    if (!SaveToTemp())
        return false;
    
    m_LinkedDoc.bSwitchInProgress = true;
    
    // Close current project.
    if (m_pAGDoc)
	   Close(false/*bExit*/, true /*bSilent*/);

    // Open Card project
    OpenDoc2(m_LinkedDoc.szCardFilePath);

    if (m_pAGDoc)
        m_pAGDoc->SetModified(bIsModified);

    m_LinkedDoc.bSwitchInProgress = false;

    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::SaveToTemp()
{
    if (!m_pAGDoc)
        return false;
    

    CString szFullDocPath;
	CString szPath;
	CString szFileName;
	m_pAGDoc->GetFileName(szFullDocPath, szPath, szFileName); 
	if (szFileName.IsEmpty())
        return false;


    TCHAR szTempPath[MAX_PATH];
	if (ERROR_PATH_NOT_FOUND == ::GetTempPath(sizeof(szTempPath), szTempPath))
        return false;
    
   
    CString szTemp = szTempPath;
    szTemp += g_szAppName;
    szTemp += _T("\\");
    ::CreateDirectory(szTemp, NULL);
    

    szTemp += String("%d\\", _AtlBaseModule.GetResourceInstance());
    ::CreateDirectory(szTemp, NULL);
   

    // Save temp location for this instance of CP.
    m_szTempDir = szTemp;

    CString szType;
    AGDOCTYPE DocType =  m_pAGDoc->GetDocType();
    if (DocType == DOC_ENVELOPE)
        szType += TEMP_ENVELOPES_DIR;
    else
        szType += TEMP_CARDS_DIR;

    szTemp += szType;
    szTemp += _T("\\");
    ::CreateDirectory(szTemp, NULL);
    	
   
    szTemp += szFileName;
    ofstream output(szTemp, ios::out | ios::binary);
	int bOpen = output.is_open();
	if (!bOpen || !m_pAGDoc->Write(output))
	{
		CMessageBox::Message(String("Error auto saving file '%s'.", szTemp), MB_OK);
		return false;
	}		
	
    if (DocType == DOC_ENVELOPE)
    {
        m_LinkedDoc.szEnvelopeFilePath = szTemp;
    }
    else
    {
        m_LinkedDoc.szCardFilePath = szTemp;
    }
    
    // Unselect the current symbol, because all symbols are going away
	// and to ensure that the document modifed flag is up to date
	SymbolUnselect(true/*bClearPointer*/);

    m_LinkedDoc.DocType = DocType;
    m_LinkedDoc.bModified = m_pAGDoc->IsModified();
    m_LinkedDoc.szFilename = szTemp;

    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::RemoveTempFilesByType(AGDOCTYPE DocType)
{
    // Delete all CTP files in temp folder by doctype
    if (m_szTempDir.IsEmpty())
        return false;

	WIN32_FIND_DATA FileData;
	HANDLE hFind;
	CString szTempDir = m_szTempDir;
    
    if (DocType == DOC_ENVELOPE)
    {
        szTempDir += TEMP_ENVELOPES_DIR;
        m_LinkedDoc.szEnvelopeFilePath.Empty();
    }
    else 
    {
        szTempDir += TEMP_CARDS_DIR;
        m_LinkedDoc.szCardFilePath.Empty();
    }
   
    CString szFindPath(szTempDir);
	szFindPath +=  _T("\\*.*");
	hFind = FindFirstFile(szFindPath, &FileData); 
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		CString szFilePath;
		bool bDone = false;
		while (!bDone)
		{
			szFilePath = szTempDir;
			szFilePath += _T("\\");
			szFilePath += FileData.cFileName;
			DeleteFile(szFilePath);

			if (!FindNextFile(hFind, &FileData)) 
			{
				if (GetLastError() != ERROR_NO_MORE_FILES) 
					GetError("FindNextFile(hFind, &FileData)");
				bDone = true; 
			}
		}
	}

	FindClose(hFind);

	//  Delete project type temp directory only if it is empty
    if (!::RemoveDirectory(szTempDir))
	    return false;

    return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::RemoveAllTempFiles()
{
    RemoveTempFilesByType(DOC_ENVELOPE);
    RemoveTempFilesByType(DOC_HALFCARD);
    
    //  Delete temp directory only if it is empty
    if (!m_szTempDir.IsEmpty() && !::RemoveDirectory(m_szTempDir))
        return false;
	
    return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CDocWindow::CloseLinkedDoc(bool bExit)
{
    if (!m_LinkedDoc.szFilename.IsEmpty() && m_LinkedDoc.bModified)
    {
        CAGDoc AGDoc(m_LinkedDoc.DocType);
        AGDoc.SetFileName(m_LinkedDoc.szFilename);
		CString strFullDocPath;
		CString strPath;
		CString strFileName;
		AGDoc.GetFileName(strFullDocPath, strPath, strFileName);
        
        AGDOCTYPE DocType = m_LinkedDoc.DocType;
        CString szMsg  = String("The contents of '%s' has changed.\n\nDo you want to save the changes?\n", strFileName);
        if (DOC_ENVELOPE == DocType)
            szMsg  = String("The contents of your envelope project '%s' has changed.\n\nDo you want to save the changes?\n", strFileName);
           
		int iResponse = CMessageBox::Message(szMsg, (bExit ? MB_YESNO | MB_SYSTEMMODAL | MB_TOPMOST : MB_YESNOCANCEL | MB_TOPMOST));
		if (iResponse == IDCANCEL)
			return S_FALSE;

		if (iResponse == IDYES)
		{
	        CString str = m_pCtp->GetContextMemberStatus();
	        bool bPaidMember = !(str!="MCP" && str!="MCS");

	        // Card Price
	        str = m_pCtp->GetContextContentStatus();
	        bool bFreeContent = (str=="0.0");

   
	        if (!bPaidMember && !bFreeContent)
	        {
		        CMessageBox::Message("Join now to save.");
		        return S_OK;
	        }
   

		    // Get the path from the registry
		    CRegKey regkey;
		    if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
		    {
			    char szFilePath[MAX_PATH];
			    szFilePath[0] = 0;
			    DWORD nChars = MAX_PATH;
			    bool bOK = (regkey.QueryStringValue(REGVAL_MRU_CTP_PATH, szFilePath, &nChars) == ERROR_SUCCESS);
			    if (bOK)
			    {
				    szFilePath[nChars] = 0;
				    strPath = szFilePath;
			    }
		    }

		    // If there is no path in the registry, use the default
		    if (strPath.IsEmpty())
		    {
			    bool bOK = GetSpecialFolderLocation(strPath, CSIDL_COMMON_DESKTOPDIRECTORY);
			    if (!bOK)
				    bOK = GetSpecialFolderLocation(strPath, CSIDL_DESKTOPDIRECTORY);
		    }

	        char szFileName[MAX_PATH];
	        szFileName[0] = 0;
	        lstrcpy(szFileName, strFileName);

	
		    OPENFILENAMEX ofx;
		    ::ZeroMemory(&ofx, sizeof(ofx));
		    ofx.of.lStructSize = (IsEnhancedWinVersion() ? sizeof(ofx) : sizeof(ofx.of));
		    ofx.of.hwndOwner = m_pCtp->GetBrowser();
		    ofx.of.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_ENABLEHOOK;
		    ofx.of.lpstrInitialDir = strPath;
		    ofx.of.lpstrFile = szFileName;
		    ofx.of.lpfnHook = CDocWindow::OpenFileHookProc;
		    ofx.of.nMaxFile = MAX_PATH;
		    ofx.of.lpstrFilter = "Project Files (*.ctp)\0*.*ctp\0\0";
		    CString strTitle = "Save your " + g_szAppName + " project";
		    ofx.of.lpstrTitle = strTitle;
		    ofx.of.lpstrDefExt = "ctp";

		    if (::GetSaveFileName(&ofx.of))
            {
		        // Save the path in the registry
		        CRegKey regkey;
		        if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
			        bool bOK = (regkey.SetStringValue(REGVAL_MRU_CTP_PATH, StrPath(szFileName)) == ERROR_SUCCESS);

		        // Redraw the window underneath, before we get to work
		        ::UpdateWindow(m_pCtp->GetBrowser());
    	

                // Add to MRU
                CString szFile(szFileName);
                if (m_szTempDir.IsEmpty() || szFile.Find(m_szTempDir) < 0)
                {
                    m_mruList.AddToList(szFileName);
	                m_mruList.WriteToRegistry(REGKEY_APP);
                }

	            CWaitCursor Wait;

                if (!::CopyFile(m_LinkedDoc.szFilename, szFileName, false))
                {
		            CMessageBox::Message(String("Error saving to file '%s'.", szFileName), MB_OK | MB_SYSTEMMODAL);
		            return false;
                }
    	
	            m_pCtp->PutSrcURL(CComBSTR(szFileName));

				m_pCtp->Fire_FileSaved(0/*lValue*/);
              
            
            }
        }

	    CWaitCursor Wait;
	    Clipboard.NotifyDocumentClose();
	    m_pCtp->ActivateNewDoc(); // Even though the doc pointer is NULL
	    Invalidate();
	}

    m_LinkedDoc.szFilename.Empty();
    m_LinkedDoc.szCardFilePath.Empty();
    m_LinkedDoc.szEnvelopeFilePath.Empty();
    m_LinkedDoc.bModified = false;
    m_LinkedDoc.bSwitchInProgress = false;
    
    return true;
}
/////////////////////////////////////////////////////////////////////////////
void CDocWindow::CleanupLinkedDoc(bool bExit)
{
    if (!m_LinkedDoc.szFilename.IsEmpty())
            CloseLinkedDoc(bExit);

    RemoveAllTempFiles();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CDocWindow::CtpKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!g_pDocWindow)
		return false;

	HHOOK hHook = g_pDocWindow->GetKeyboardHook();
	if (!hHook)
		return false;

	UINT nChar = wParam; // The virtual key code
	// nCode == HC_ACTION(0) or HC_NOREMOVE(3)
	if (nCode < 0 || !g_pDocWindow->HasFocus())
	{ // This key belongs to the browser
		if (nChar >= VK_F1 && nChar <= VK_F12 && nCode == HC_ACTION) // Steal F7-F12 from the browser
			g_pDocWindow->SetFocus();
		else
			return ::CallNextHookEx(hHook, nCode, wParam, lParam);
	}

	if (nChar == VK_CONTROL || nChar == VK_SHIFT)
		return ::CallNextHookEx(hHook, nCode, wParam, lParam);

	WORD nRepeatCount = LOWORD(lParam);
	WORD wFlags = HIWORD(lParam);
	bool bKeyDown	=  !(wFlags & KF_UP);
	bool bRepeat	= !!(wFlags & KF_REPEAT);
	bool bAltDown	= !!(wFlags & KF_ALTDOWN);
//j	bool bMenuMode	= !!(wFlags & KF_MENUMODE);
//j	bool bExtended	= !!(wFlags & KF_EXTENDED);
//j	bool bDlgMode	= !!(wFlags & KF_DLGMODE);

	// Handle keyboard accelerators
	if (bKeyDown)
	{
		UINT idMessage = NULL;
		bool bControl = CONTROL;
		if (bControl)
		{
			if (nChar == 'A' && !bRepeat)		idMessage = IDC_DOC_SELECTALL;		else
			if (nChar == 'C' && !bRepeat)		idMessage = IDC_DOC_COPY;			else
			if (nChar == 'F' && !bRepeat)		idMessage = IDC_DOC_DOWNLOADFONTS;	else
			if (nChar == 'N' && !bRepeat && g_pDocWindow->SpecialsEnabled())
												idMessage = IDC_DOC_NEW;			else
			if (nChar == 'O' && !bRepeat)		idMessage = IDC_DOC_OPEN;			else
			if (nChar == 'P' && !bRepeat)		idMessage = IDC_DOC_PRINT;			else
			if (nChar == 'S' && !bRepeat)		idMessage = IDC_DOC_SAVE;			else
			if (nChar == 'V' && !bRepeat)		idMessage = IDC_DOC_PASTE;			else
			if (nChar == 'X' && !bRepeat)		idMessage = IDC_DOC_CUT;			else
			if (nChar == 'Z')					idMessage = (SHIFT ? IDC_DOC_REDO : IDC_DOC_UNDO);	else
			if (nChar == VK_DELETE && !bRepeat)	idMessage = IDC_DOC_DELETE;			else
				int i = 0;

			if (idMessage)
			{
				::PostMessage(g_pDocWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(idMessage, BN_CLICKED), (LPARAM)g_pDocWindow->m_hWnd);
				return true;
			}

			if (nChar == 'B' && !bRepeat)		idMessage = IDC_BOLD;				else
			if (nChar == 'I' && !bRepeat)		idMessage = IDC_ITALIC;				else
			if (nChar == 'U' && !bRepeat)		idMessage = IDC_UNDERLINE;			else
				int i = 0;

			if (idMessage)
			{
				CToolsPanelContainer* pCtlPanel = g_pDocWindow->m_pCtp->GetCtlPanel(TEXT_PANEL);
				if (pCtlPanel)
				{
					CToolsPanelText1* pTopTxtToolsPanel = (CToolsPanelText1*)pCtlPanel->GetTopToolPanel();
					if (pTopTxtToolsPanel)
					{
						pTopTxtToolsPanel->CheckDlgButton(idMessage, pTopTxtToolsPanel->IsDlgButtonChecked(idMessage) ? BST_UNCHECKED : BST_CHECKED);
						::PostMessage(pTopTxtToolsPanel->m_hWnd, WM_COMMAND, MAKEWPARAM(idMessage, BN_CLICKED), (LPARAM)pTopTxtToolsPanel->m_hWnd);
					}
				}
				return true;
			}
		}
		else
		if (bAltDown)
		{
			if (nChar == VK_UP)					idMessage = IDC_DOC_SELECTPREV;		else
			if (nChar == VK_DOWN)				idMessage = IDC_DOC_SELECTNEXT;		else
				int i = 0;

			if (idMessage)
			{
				::PostMessage(g_pDocWindow->m_hWnd, WM_COMMAND, MAKEWPARAM(idMessage, BN_CLICKED), (LPARAM)g_pDocWindow->m_hWnd);
				return true;
			}

			if (nChar == VK_LEFT)				idMessage = IDC_DOC_PAGEPREV;		else
			if (nChar == VK_RIGHT)				idMessage = IDC_DOC_PAGENEXT;		else
				int i = 0;

			if (idMessage)
			{
				CToolsPanelContainer* pCtlPanel = g_pDocWindow->m_pCtp->GetCtlPanel(SELECTED_PANEL);
				if (pCtlPanel)
				{
					CToolsPanelText1* pTopTxtToolsPanel = (CToolsPanelText1*)pCtlPanel->GetTopToolPanel();
					if (pTopTxtToolsPanel)
					{
						pTopTxtToolsPanel->CheckDlgButton(idMessage, pTopTxtToolsPanel->IsDlgButtonChecked(idMessage) ? BST_UNCHECKED : BST_CHECKED);
						::PostMessage(pTopTxtToolsPanel->m_hWnd, WM_COMMAND, MAKEWPARAM(idMessage, BN_CLICKED), (LPARAM)pTopTxtToolsPanel->m_hWnd);
					}
				}
				return true;
			}
		}

	}

	switch (nChar)
	{
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
		{
			int bHandled = true;
			UINT nChar1 = nChar;
			AdjustArrowKeyByTransform(g_pDocWindow->m_Select.GetSym(), nChar1, false/*bInverse*/);
			if (bKeyDown)
				g_pDocWindow->OnKeyDown(0, nChar1, lParam, bHandled);
			else
				g_pDocWindow->OnKeyUp(0, nChar1, lParam, bHandled);
			return true;
		}

		case VK_HOME:
		case VK_END:
		case VK_PRIOR:
		case VK_NEXT:
		{
			int bHandled = true;
			if (bRepeat)
				return true;
			if (bKeyDown)
				g_pDocWindow->OnKeyDown(0, nChar, lParam, bHandled);
			else
				g_pDocWindow->OnKeyUp(0, nChar, lParam, bHandled);
			return true;
		}

         case VK_F1: // Help
		{
			int bHandled = true;
			if (bRepeat)
				return true;
			if (bKeyDown)
				g_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_HELP);
			return true;
		}
        case VK_F3: // Free Rotate
		{
			int bHandled = true;
			if (bRepeat)
				return true;
			if (bKeyDown)
				g_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_FREEROTATE);
			return true;
		}
         case VK_F4: // Move/Resize
		{
			int bHandled = true;
			if (bRepeat)
				return true;
			if (bKeyDown)
				g_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_TRANSFORM);
			return true;
		}
		case VK_F7: // Kick off spell check
		{
			int bHandled = true;
			if (bRepeat)
				return true;
			if (bKeyDown)
				g_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_SPELLCHECK);
			return true;
		}

		case VK_F8: // Display the saved file size
		{
			int bHandled = true;
			if (bRepeat)
				return true;
			if (bKeyDown)
				g_pDocWindow->DisplaySavedFileSize();
			return true;
		}

		case VK_F9: // Zoom in on the next selection
		{
			int bHandled = true;
			if (bKeyDown)
			{
				g_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_SELECTNEXT);
				g_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_ZOOM2);
			}
			return true;
		}

		case VK_F10: // Zoom out on the cursor
		{
			int bHandled = true;
			if (bKeyDown)
				g_pDocWindow->PostMessage(WM_COMMAND, IDC_DOC_ZOOMOUT, 0xcaca);
			return true;
		}

		case VK_F11: // Zoom in on the cursor
		{
			int bHandled = true;
			if (bKeyDown)
				g_pDocWindow->PostMessage(WM_COMMAND, IDC_DOC_ZOOMIN, 0xbaba);
			return true;
		}

		case VK_F12: // Developer Features
		{
			int bHandled = true;
			if (bRepeat)
				return true;
			if (bKeyDown)
			{
				if (CONTROL && SHIFT)
				{ // Secret key to enable additional functionality
					bool bEnable = !g_pDocWindow->SpecialsEnabled();
					g_pDocWindow->SetSpecialsEnabled(bEnable);

					CRegKey regkey;
					if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
					{
						CString strValue;
						strValue.Format("%d", bEnable);
						bool bOK = (regkey.SetStringValue(REGVAL_SPECIALSENABLED, strValue) == ERROR_SUCCESS);
					}

					g_pDocWindow->m_pCtp->ConfigureWindows();

					CString strMsg = String("You have just %s the developer features.", (bEnable ? "ENABLED" : "DISABLED"));
					if (bEnable)
						strMsg += "\n\nAccess these features by right-clicking on the project and selecting from the popup menu.";
					CMessageBox::Message(strMsg);
				}
				else
				{ // Zoom full
					g_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_ZOOMFULL);
				}
			}

			return true;
		}

		case VK_TAB:
		case VK_BACK:
		{
			int bHandled = true;
			if (bKeyDown)
				g_pDocWindow->OnChar(NULL, nChar, lParam, bHandled);
			return true;
		}
	}

	return ::CallNextHookEx(hHook, nCode, wParam, lParam);
}
