#include "stdafx.h"
#include "Ctp.h"
#include "AGPage.h"
#include "AGLayer.h"
#include "AGDC.h"
#include "Image.h"
#include "HtmlDialog.h"
#include <fstream>
#include "MyAtlUrl.h"
#include "RegKeys.h"
#include "ConvertDibToJpg.h"
#include "Spell.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

// Global
CString g_szAppName;

/////////////////////////////////////////////////////////////////////////////
CCtp::CCtp() :
	m_pAutoUpdate(NULL)
{
#ifdef _DEBUG
	// Add _CRTDBG_ALLOC_MEM_DF	if you want to enable debug heap allocations and use of memory block type identifiers, such as _CLIENT_BLOCK.
	// Add _CRTDBG_CHECK_CRT_DF	if you want the c-runtime memory leaks dumped
	// Add _CRTDBG_LEAK_CHECK_DF  if you want to perform automatic leak checking at program exit via a call to _CrtDumpMemoryLeaks and generate an error report if the application failed to free all the memory it allocated
	// Add _CRTDBG_CHECK_ALWAYS_DF if you want to call _CrtCheckMemory at every allocation and deallocation request
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif _DEBUG

	m_hWndBrowser = NULL;
	m_pBlankPanel = NULL;
	m_pInfoPanel = NULL;
	m_pDocWindow = NULL;
	m_pAutoUpdate = NULL;
	m_nSelectedPanel = 0;
	m_bAutoPrint = false;

	m_pPrintPanel = NULL;
	m_pPagePanel = NULL;
	for (int nIndex = 0; nIndex < NUM_PANELS; nIndex++)
		m_pToolsPanel[nIndex] = NULL;

	COLORREF ColorText = ::GetSysColor(COLOR_WINDOWTEXT); //j RGB(0,0,0)
	COLORREF ColorBgDialog = ::GetSysColor(COLOR_BTNFACE);
	COLORREF ColorBgControl = ::GetSysColor(COLOR_WINDOW);

	m_ColorScheme.m_ColorTextDialog		= ColorText;
	m_ColorScheme.m_ColorTextStatic		= ColorText;
	m_ColorScheme.m_ColorTextMsgBox		= ColorText;
	m_ColorScheme.m_ColorTextEdit		= ColorText;
	m_ColorScheme.m_ColorTextListbox	= ColorText;
	m_ColorScheme.m_ColorTextButton		= ColorText;
	m_ColorScheme.m_ColorBgDialog		= ColorBgDialog;
	m_ColorScheme.m_ColorBgStatic		= ColorBgDialog;
	m_ColorScheme.m_ColorBgMsgBox		= ColorBgDialog;
	m_ColorScheme.m_ColorBgEdit			= ColorBgControl;
	m_ColorScheme.m_ColorBgListbox		= ColorBgControl;
	m_ColorScheme.m_ColorBgButton		= ColorBgControl;
	m_ColorScheme.m_ColorBgScrollbar	= ColorBgDialog;

	
	m_bWindowOnly = true; // Some ATL thing?
	
}

/////////////////////////////////////////////////////////////////////////////
CCtp::~CCtp()
{
	Free();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CCtp::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_Download.SetAppInfo(this->m_spClientSite, 1);

	m_pAutoUpdate = new CAutoUpdate(this);

	g_szAppName.Format(IDS_MSGBOXTITLE);

	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	::SetClassLong(m_hWnd, GCL_STYLE, ::GetClassLong(m_hWnd, GCL_STYLE) | CS_DBLCLKS);

	// Since this sets the caption text and icons for the browser window, do it as soon as possible
	HWND hWndBrowser = GetBrowser();
	if (hWndBrowser && AutoPrintEnabled())
		::ShowWindow(hWndBrowser, SW_MINIMIZE);

	CMessageBox::SetDefaults(hWndBrowser, g_szAppName, "MessageBox.htm");

	RECT rClient = {0, 0, 0, 0};
	::GetClientRect(m_hWnd, &rClient);

	HRESULT hr = ::RegisterDragDrop(m_hWnd, this);
	if (m_pDocWindow = new CDocWindow(this))
	{ // Maintained to be the full size of the client area under the control panel (when shown)
		RECT rect = {0, 0, rClient.right, rClient.bottom};
		m_pDocWindow->Create(m_hWnd, rect, "", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, WS_EX_ACCEPTFILES);
		m_pDocWindow->SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE);

		if (m_strSrcURL.IsEmpty())
			m_pDocWindow->DrawProgressDlg(""/*"Waiting to open a project..."*/);
		else
			m_pDocWindow->DrawProgressDlg("Your project is loading.\nPlease wait...");
	}

	{ // attempt 2
	// Create all control panels as hidden
	if (m_pToolsPanel[DEFAULT_PANEL] = new CToolsPanelContainer(this))
		m_pToolsPanel[DEFAULT_PANEL]->Create();

	if (m_pToolsPanel[TEXT_PANEL] = new CTextPanelContainer(this))
		m_pToolsPanel[TEXT_PANEL]->Create();

	if (m_pToolsPanel[IMAGE_PANEL] = new CImagePanelContainer(this))
		m_pToolsPanel[IMAGE_PANEL]->Create();

	if (m_pToolsPanel[GRAPHICS_PANEL] = new CGraphicsPanelContainer(this))
		m_pToolsPanel[GRAPHICS_PANEL]->Create();

	if (m_pToolsPanel[CALENDAR_PANEL] = new CCalPanelContainer(this))
		m_pToolsPanel[CALENDAR_PANEL]->Create();
	}

	if (m_pPrintPanel = new CToolsPanelPrint(this))
		m_pPrintPanel->Create(m_hWnd);

	{ // attempt 1
	// Create all control panels as hidden
	//if (m_pToolsPanel[DEFAULT_PANEL] = new CToolsCtlPanel(this))
	//	m_pToolsPanel[DEFAULT_PANEL]->Create(m_hWnd);

	//if (m_pToolsPanel[TEXT_PANEL] = new CToolsPanelText1(this))
	//	m_pToolsPanel[TEXT_PANEL]->Create(m_hWnd);

	//if (m_pToolsPanel[IMAGE_PANEL] = new CToolsPanelImage1(this))
	//	m_pToolsPanel[IMAGE_PANEL]->Create(m_hWnd);

	//if (m_pToolsPanel[GRAPHICS_PANEL] = new CToolsPanelGraphics1(this))
	//	m_pToolsPanel[GRAPHICS_PANEL]->Create(m_hWnd);

	//if (m_pToolsPanel[CALENDAR_PANEL] = new CToolsPanelCal1(this))
	//	m_pToolsPanel[CALENDAR_PANEL]->Create(m_hWnd);
	}

	{ // the original stuff
	//// Create all control panels as hidden
	//if (m_pToolsPanel[DEFAULT_PANEL] = new CToolsCtlPanel(this))
	//	m_pToolsPanel[DEFAULT_PANEL]->Create(m_hWnd);

	//if (m_pToolsPanel[TEXT_PANEL] = new CToolsCtlPanelText(this))
	//	m_pToolsPanel[TEXT_PANEL]->Create(m_hWnd);

	//if (m_pToolsPanel[IMAGE_PANEL] = new CToolsCtlPanelImage(this))
	//	m_pToolsPanel[IMAGE_PANEL]->Create(m_hWnd);

	//if (m_pToolsPanel[GRAPHICS_PANEL] = new CToolsCtlPanelGraphics(this))
	//	m_pToolsPanel[GRAPHICS_PANEL]->Create(m_hWnd);

	//if (m_pToolsPanel[CALENDAR_PANEL] = new CToolsCtlPanelCal(this))
	//	m_pToolsPanel[CALENDAR_PANEL]->Create(m_hWnd);
	}

	if (m_pPagePanel = new CPagePanel(this))
		m_pPagePanel->Create(m_hWnd);

	// Create as hidden: Maintained to sit to the right of the control panel (when shown)
	if (m_pBlankPanel = new CBlankPanel())
	{ 
		RECT rect = {0, 0, 1, 1};
		m_pBlankPanel->Create(m_hWnd, rect, "", WS_CHILD, WS_EX_NOPARENTNOTIFY);
		m_pBlankPanel->SetWindowPos(HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE);
	}

	// Create as hidden: Maintained to sit under the control panel (when shown)
	if (m_pInfoPanel = new CInfoPanel(this))
		m_pInfoPanel->Create(m_hWnd);

	ActivateBrowser();

	
	bool bStart = true;
	
	//SrcDownload(bStart);

	//DWORD dwMajor = 0;
	//DWORD dwMinor = 0;
	//AtlGetCommCtrlVersion(&dwMajor, &dwMinor);
	//bool bUsesVisualThemes = (dwMajor >= 6);
//j	CMessageBox::Message(String("CommCtrl version %ld.%ld", dwMajor, dwMinor));

	CRegKey regkey;
	if (regkey.Create(REGKEY_APPROOT, REGKEY_APP) == ERROR_SUCCESS)
	{ // Create the brand specific desktop icons
		//CString strShortcutName = String(IDS_MSGBOXTITLE) + " Home";

		//bool bBlueMountain = IsBlueMountain();
		//CString strCpHost = GetContextCpHost();
		//if (strCpHost.Find("http://") < 0)
		//	strCpHost = String("http://www.%s.com/cnp/", (bBlueMountain ? "bluemountain" : "americangreetings"));

		//CString strTarget = strCpHost + "index.pd";
		//

		//char strModuleFileName[MAX_PATH];
		//strModuleFileName[0] = '\0';
		//GetModuleFileName(_AtlBaseModule.GetResourceInstance(), strModuleFileName, MAX_PATH);
		//LPCSTR strIconFile = strModuleFileName;
		//
		//int iIconIndex = (bBlueMountain ? 1 : 0);

		//// Bump this version value whenever you want to force all user to get the icons again
		//#define CURRENT_SHORTCUT_VERSION 2 
		//DWORD dwShortcutVersion = 0;
		//bool bOK = (regkey.QueryDWORDValue(REGVAL_SHORTCUT_VERSION, dwShortcutVersion) == ERROR_SUCCESS);
		//bool bCreate = (dwShortcutVersion != CURRENT_SHORTCUT_VERSION);
		//
		//WriteInternetShortcut(strShortcutName, strTarget, strIconFile, iIconIndex, bCreate, CSIDL_COMMON_DESKTOPDIRECTORY);
		//WriteInternetShortcut(strShortcutName, strTarget, strIconFile, iIconIndex, bCreate, CSIDL_FAVORITES);
		//WriteInternetShortcut(strShortcutName, strTarget, strIconFile, iIconIndex, bCreate, CSIDL_COMMON_STARTMENU);
		//WriteInternetShortcut(strShortcutName, strTarget, strIconFile, iIconIndex, bCreate, CSIDL_COMMON_PROGRAMS);

		//bOK = (regkey.SetDWORDValue(REGVAL_SHORTCUT_VERSION, CURRENT_SHORTCUT_VERSION) == ERROR_SUCCESS);
	}

	//// Fix the shell registry entries to be host specific
	//if (regkey.Open(HKEY_CLASSES_ROOT, REGVAL_SHELL_OPEN) == ERROR_SUCCESS)
	//{
	//	CString strCpHost = GetContextCpHost();
	//	CString strOpenCommand = "\"C:\\Program Files\\Internet Explorer\\iexplore.exe\" -nohome \"";
	//	strOpenCommand += strCpHost;
	//	strOpenCommand += (strCpHost.Find("http://") < 0 ? "cporder.htm" : "cporder.pd");
	//	strOpenCommand += "?product=%1";
	//	CString strPrintCommand = strOpenCommand;
	//	strOpenCommand += "\"";
	//	strPrintCommand += "/Print\"";

	//	bool bOK = (regkey.SetStringValue(NULL, strOpenCommand) == ERROR_SUCCESS);

	//	if (regkey.Open(HKEY_CLASSES_ROOT, REGVAL_SHELL_PRINT) == ERROR_SUCCESS)
	//		bool bOK = (regkey.SetStringValue(NULL, strPrintCommand) == ERROR_SUCCESS);
	//}


	//Fire_PluginReady(0/*lValue*/);
	//bool bRet = m_pDocWindow->CallJavaScript(_T("PahPluginReady")); // TEST - jhc
	::PostMessage(m_pDocWindow->m_hWnd, UWM_PLUGINREADY, NULL, NULL);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CCtp::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Free();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CCtp::Free()
{
	::RevokeDragDrop(m_hWnd);

	if (m_pPagePanel)
	{
		m_pPagePanel->DestroyWindow();
		delete m_pPagePanel;
		m_pPagePanel = NULL;
	}

	if (m_pPrintPanel)
	{
		m_pPrintPanel->DestroyWindow();
		delete m_pPrintPanel;
		m_pPrintPanel = NULL;
	}

	for (int nIndex = 0; nIndex < NUM_PANELS; nIndex++)
	{
		if (m_pToolsPanel[nIndex])
		{
			//m_pToolsPanel[nIndex]->DestroyWindow();
			m_pToolsPanel[nIndex]->Free();
			delete m_pToolsPanel[nIndex];
			m_pToolsPanel[nIndex] = NULL;
		}
	}

	if (m_pBlankPanel)
	{
		m_pBlankPanel->DestroyWindow();
		delete m_pBlankPanel;
		m_pBlankPanel = NULL;
	}

	if (m_pInfoPanel)
	{
		m_pInfoPanel->DestroyWindow();
		delete m_pInfoPanel;
		m_pInfoPanel = NULL;
	}

	if (m_pDocWindow)
	{
		m_pDocWindow->DestroyWindow();
		delete m_pDocWindow;
		m_pDocWindow = NULL;
	}

	if (m_pAutoUpdate)
	{
		delete m_pAutoUpdate;
		m_pAutoUpdate = NULL;
	}

	m_Extend.Unload();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CCtp::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (!hDC)
	{
		bHandled = false;
		return S_OK;
	}

	// Say we handled it, but do nothing; OnPaint takes care of everything
	bHandled = true;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CCtp::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	InPlaceActivate(OLEIVERB_UIACTIVATE);

	return S_OK;
}

#ifdef NOTUSED //j
/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::OnDraw(ATL_DRAWINFO& di)
{
	return Draw(di.hdcDraw);
}
#endif NOTUSED //j

/////////////////////////////////////////////////////////////////////////////
LRESULT CCtp::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT PaintStruct;
	HDC hPaintDC = ::BeginPaint(m_hWnd, &PaintStruct);
	if (!hPaintDC)
		return E_FAIL;

#ifdef NOTUSED //j
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, (WPARAM)hPaintDC, 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		RECT rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		::DrawIcon(hPaintDC, x, y, m_hIcon);
	}
#endif NOTUSED //j

	::EndPaint(m_hWnd, &PaintStruct);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CCtp::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pDocWindow)
		return S_OK;

	// wParam values: SIZE_MAXIMIZED, SIZE_MINIMIZED, SIZE_RESTORED
	if (wParam != SIZE_RESTORED)
		return S_OK;

	// Get the new client size
	int iWidth = LOWORD(lParam);
	int iHeight = HIWORD(lParam);

	ConfigureWindows(iWidth, iHeight);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCtp::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return m_ColorScheme.OnCtlColor(uMsg, wParam);
}

/////////////////////////////////////////////////////////////////////////////
void CCtp::CreateBackPage()
{
	//return; // Disabled for time being - JHC
	
	::MessageBox(NULL, "Debug", "", MB_OK);
	CAGDoc* pAGDoc = (!m_pDocWindow ? NULL : m_pDocWindow->GetDoc());
	if (!pAGDoc)
		return;

	if (!pAGDoc->NeedsCardBack())
		return;

	CAGPage* pPage = pAGDoc->GetPage(3);
	if (!pPage)
		return;

	CAGLayer* pMasterLayer = pPage->GetMasterLayer();
	if (!pMasterLayer)
		return;

	CAGLayer* pUserLayer = pPage->GetUserLayer();
	if (!pUserLayer)
		return;

	// Only add the card back symbols if both layers are empty
	if (!pMasterLayer->IsEmpty() || !pUserLayer->IsEmpty())
		return;

	CString strCpHost = GetContextCpHost();
	bool bBlueMountain = IsBlueMountain();

	SIZE PageSize = {0,0};
	pPage->GetPageSize(PageSize);
	int xCenter = PageSize.cx / 2;
	int yCenter = PageSize.cy / 2;
	RECT ImageRect = {0, 0, 0, 0};

	RECT rMargins = { INCHES(0.375), INCHES(0.375), PageSize.cx - INCHES(0.375), PageSize.cy - INCHES(0.375)};

	// The Message text
	RECT MsgRect = {0, 0, INCHES(3.25), INCHES(1.0)};
	::OffsetRect(&MsgRect, xCenter - WIDTH(MsgRect)/2, rMargins.top);

	// The Copyright text
	long lWidth = INCHES(2.5);
	long lHeight = INCHES(0.375);
	RECT CopyrightRect = {0, 0, lWidth, lHeight};
	::OffsetRect(&CopyrightRect, xCenter - WIDTH(CopyrightRect)/2, rMargins.bottom - HEIGHT(CopyrightRect));

	// The Create & Print logo
	RECT LogoRect;
	long lSize = INCHES(1.75);
	::SetRect(&ImageRect, 0, 0, lSize, lSize);
	int idSiteLogo = (bBlueMountain ? IDR_BMLOGO : IDR_AGLOGO);
	CAGSymImage* pSymLogo = GetImageFromResource(idSiteLogo, &ImageRect);
	if (pSymLogo)
	{
		pSymLogo->SetID(IDR_AGLOGO);
		ImageRect = pSymLogo->GetDestRect();
		RECT Rect = {0, 0, WIDTH(ImageRect), HEIGHT(ImageRect)};
		LogoRect = Rect;
	}

	// The Sponsor logo
	RECT SponsorLogoRect;
	CAGSymImage* pSymSponsorLogo = NULL;
	
	
	int iMessageFontHeight = POINTUNITS(14);
	int iCopyrightFontHeight = POINTUNITS(9);

	// Make any location or size tweaks
	switch (pAGDoc->GetDocType())
	{
		case DOC_HALFCARD:
		{
			int delta = INCHES(0.125);
			if (pPage->IsPortrait())
			{
				::OffsetRect(&MsgRect, 0, INCHES(0.25)); // tweak down
			}
			else
			{
				::OffsetRect(&MsgRect, 0, delta); // tweak down
				::OffsetRect(&CopyrightRect, 0, -delta); // tweak up
			}

			break;
		}

		case DOC_NOTECARD:
		{
			int delta = INCHES(0.125);
			if (pPage->IsPortrait())
			{
			}
			else
			{
				::OffsetRect(&CopyrightRect, 0, -delta); // tweak up
			}

			break;
		}

		case DOC_QUARTERCARD:
		case DOC_CDBOOKLET:
		{
			break;
		}
	}

	bool bIsApp = IsApplication();
	if (pSymLogo)
	{
		::OffsetRect(&LogoRect, xCenter - WIDTH(LogoRect)/2, CopyrightRect.top - HEIGHT(LogoRect));
		pSymLogo->SetDestRect(LogoRect);
		if (bIsApp)
			pUserLayer->AddSymbol(pSymLogo);
		else
			pMasterLayer->AddSymbol(pSymLogo);
	}

	if (pSymSponsorLogo)
	{
		int yOffset = (PageSize.cy - HEIGHT(SponsorLogoRect)) / 7;
		::OffsetRect(&SponsorLogoRect, xCenter - WIDTH(SponsorLogoRect)/2, yCenter - HEIGHT(SponsorLogoRect)/2 - yOffset);
		pSymSponsorLogo->SetDestRect(SponsorLogoRect);
		if (bIsApp)
			pUserLayer->AddSymbol(pSymSponsorLogo);
		else
			pMasterLayer->AddSymbol(pSymSponsorLogo);
	}

	LOGFONT Font;
	memset(&Font, 0, sizeof(Font));
	Font.lfWeight = FW_NORMAL;
	Font.lfCharSet = ANSI_CHARSET; //j DEFAULT_CHARSET
	lstrcpy(Font.lfFaceName, "Arial");

	CAGSymText* pText = new CAGSymText(ST_TEXT);
	if (pText)
	{
		pText->SetID(IDR_CARDBACK_COPYRIGHT);
		pText->Create(CopyrightRect);
		pText->SetVertJust(eVertBottom);

		Font.lfHeight = -iCopyrightFontHeight;

		CString strURL = strCpHost;
		int iPos = strURL.Find("http://");
		if (iPos >= 0)
			strURL = strURL.Mid(iPos + 7);
		else
			strURL = "www.americangreetings.com";
		iPos = strURL.Find("/");
		if (iPos >= 0)
			strURL = strURL.Left(iPos);

		CString pRawText = strURL + "\n";
		pRawText += "© AG.com, Inc.";

#ifdef NOTUSED //j
		"www.americangreetings.com"
		"www.bluemountain.com"
		"www.bluemountaincards.co.uk"
		"msn.bluemountaincards.co.uk"
#endif NOTUSED //j

		// Initialize the type specs
		CAGSpec* pSpecs[] = { new CAGSpec(
			Font,			// LOGFONT& Font
			eRagCentered,	// eTSJust HorzJust
			0,				// short sLineLeadPct
			LT_None,		// int LineWidth
			CLR_NONE,		// COLORREF LineColor
			FT_Solid,		// FillType Fill
			RGB(0,0,0),		// COLORREF FillColor
			CLR_NONE)		// COLORREF FillColor2
		};

		const int pOffsets[] = {0};
		pText->SetText(pRawText, 1, pSpecs, pOffsets);
		if (bIsApp)
			pUserLayer->AddSymbol(pText);
		else
			pMasterLayer->AddSymbol(pText);
	}

	pText = new CAGSymText(ST_TEXT);
	if (pText)
	{
		pText->SetID(IDR_CARDBACK_MESSAGE);
		pText->Create(MsgRect);
		pText->SetVertJust(eVertTop);

		Font.lfHeight = -iMessageFontHeight;
		CString strUser = GetContextUser();
		if (strUser.IsEmpty())
			strUser = "(sender's name)";
	
		strUser = "\n" + strUser;
		CString strRawText = "Created just for you by\n" + strUser;

		// Initialize the type specs
		CAGSpec* pSpecs[] = { new CAGSpec(
			Font,			// LOGFONT& Font
			eRagCentered,	// eTSJust HorzJust
			0,				// short sLineLeadPct
			LT_None,		// int LineWidth
			CLR_NONE,		// COLORREF LineColor
			FT_Solid,		// FillType Fill
			RGB(0,0,0),		// COLORREF FillColor
			CLR_NONE)		// COLORREF FillColor2
		};

		const int pOffsets[] = {0};
		pText->SetText(strRawText, 1, pSpecs, pOffsets);
		pUserLayer->AddSymbol(pText);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCtp::ShowHideCardBack(bool bShow)
{
	CAGDoc* pAGDoc = (!m_pDocWindow ? NULL : m_pDocWindow->GetDoc());
	if (!pAGDoc)
		return;

	if (!pAGDoc->NeedsCardBack())
		return;

	CAGPage* pPage = pAGDoc->GetPage(3);
	if (!pPage)
		return;

	int nLayers = pPage->GetNumLayers();
	for (int nLayer = 0; nLayer < nLayers; nLayer++)
	{
		CAGLayer* pLayer = pPage->GetLayer(nLayer);
		if (!pLayer)
			continue;

		int nSymbols = pLayer->GetNumSymbols();
		for (int nSymbol = 0; nSymbol < nSymbols; nSymbol++)
		{
			CAGSym* pSym = pLayer->GetSymbol(nSymbol);
			if (pSym)
				pSym->SetDeleted(!bShow);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
CAGSymImage* CCtp::GetImageFromResource(int nID, const RECT* pMaxBounds)
{
	CAGDoc* pAGDoc = (!m_pDocWindow ? NULL : m_pDocWindow->GetDoc());
	if (!pAGDoc)
		return NULL;

	// Get the image from the resource
	CImage Image(_AtlBaseModule.GetResourceInstance(), nID, "GIF");
	BITMAPINFOHEADER* pDIB = Image.GetDibPtr();
	if (!pDIB)
		return NULL;

	RECT MaxBounds = {0, 0, 0, 0};
	if (pMaxBounds)
		MaxBounds = *pMaxBounds;
	else
	{
		CAGPage* pPage = pAGDoc->GetCurrentPage();
		if (pPage)
		{
			// Get the page size minus the non-printable margins
			SIZE PageSize = {0,0};
			pPage->GetPageSize(PageSize);
			RECT PageRect = {0, 0, PageSize.cx, PageSize.cy};
			RECT Margins = {0,0,0,0};
			m_pDocWindow->m_Print.GetMargins(pAGDoc, Margins);
			PageRect.left   += Margins.left;
			PageRect.top    += Margins.top;
			PageRect.right  -= Margins.right;
			PageRect.bottom -= Margins.bottom;
			MaxBounds = PageRect;
		}
	}

	CAGSymImage* pSymImage = new CAGSymImage(pDIB, MaxBounds);
	if (pSymImage)
		pSymImage->SetID(nID);

	return pSymImage;
}

/////////////////////////////////////////////////////////////////////////////
static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	TCHAR buffer[100];
	::GetClassName(hWnd, (LPTSTR)&buffer, sizeof(buffer));
	if (_tcscmp(buffer, _T("Internet Explorer_Server")))
		return true;

	*(HWND*)lParam = hWnd;
	return false;
};

/////////////////////////////////////////////////////////////////////////////
HWND CCtp::GetBrowser()
{
	if (!m_hWnd)
	{
		m_hWndBrowser = NULL;
		return NULL;
	}

	if (m_hWndBrowser)
		return m_hWndBrowser;

	// Walk up the parent chain to find a window with a caption; this should be the browser
	m_hWndBrowser = m_hWnd;
	while (m_hWndBrowser = ::GetParent(m_hWndBrowser))
	{
		DWORD dwStyle = ::GetWindowLong(m_hWndBrowser, GWL_STYLE);
		bool bIsBrowser = ((dwStyle & WS_CAPTION) == WS_CAPTION);
		if (bIsBrowser)
			break;
	}
	
	if (!m_hWndBrowser)
		return NULL;

	// Sanity check; it should have a title
	char szTitle[MAX_PATH];
	szTitle[0] = '\0';
	::GetWindowText(m_hWndBrowser, szTitle, sizeof(szTitle));
	CString strTitle = szTitle;
	if (strTitle.IsEmpty())
	{
		m_hWndBrowser = NULL;
		return NULL;
	}
	
	// If need be, eliminate 1.) the URL from the front of the window title and 
	// 2.) the name of the browser from the end of the window title
	bool bIsWorkspace = (strTitle.Find("Workspace") >= 0); // Create & Print Workspace
	int iFirstSeparator = strTitle.Find(" - ");
	if (iFirstSeparator >= 0)
	{
		int iSecondSeparator = strTitle.Find(" - ", iFirstSeparator + 1);
		if (iSecondSeparator >= 0)
		{
			int iStart = iFirstSeparator + 3;
			strTitle = strTitle.Mid(iStart);
			iFirstSeparator = iSecondSeparator - iStart;
		}

		strTitle = strTitle.Left(iFirstSeparator);
		::SetWindowText(m_hWndBrowser, strTitle);
	}

	// Change the browser's icons to our logo icon
	bool bBlueMountain = IsBlueMountain();
	int idSiteIcon = (bBlueMountain ? IDI_BMICON : IDI_AGICON);
	HICON hIcon = ::LoadIcon(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(idSiteIcon));
	::SendMessage(m_hWndBrowser, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	::SendMessage(m_hWndBrowser, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

	return m_hWndBrowser;
}

/////////////////////////////////////////////////////////////////////////////
void CCtp::ActivateBrowser()
{
	HWND hWndBrowser = GetBrowser();
	if (!hWndBrowser)
		return;

	// Make it the top most window
	::SetWindowPos(hWndBrowser, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	::SetForegroundWindow(hWndBrowser);
}

/////////////////////////////////////////////////////////////////////////////
void CCtp::ActivateNewDoc()
{
	if (m_pDocWindow)
		m_pDocWindow->ActivateNewDoc();

	if (m_pPagePanel)
		m_pPagePanel->ActivateNewDoc();

	if (m_pPrintPanel)
		m_pPrintPanel->ActivateNewDoc();

	for (int nIndex = 0; nIndex < NUM_PANELS; nIndex++ )
	{
		if (m_pToolsPanel[nIndex])
			m_pToolsPanel[nIndex]->ActivateNewDoc();
	}

	ConfigureWindows();

	if (m_pToolsPanel[m_nSelectedPanel])
		m_pToolsPanel[m_nSelectedPanel]->UpdateControls();
}

/////////////////////////////////////////////////////////////////////////////
void CCtp::ConfigureWindows(int iWidth, int iHeight)
{
	RECT rClient = {0, 0, iWidth, iHeight};
	if (!iWidth && !iHeight)
		::GetClientRect(m_hWnd, &rClient);

	RECT rPrintPanel = {0, 0, 0, 0};
	RECT rPagePanel = {0, 0, 0, 0};
	RECT rToolsPanel = {0, 0, 0, 0};
	RECT rBlankPanel = {0, 0, 0, 0};
	RECT rInfoPanel = {0, 0, 0, 0};

	CAGDoc* pAGDoc = (!m_pDocWindow ? NULL : m_pDocWindow->GetDoc());
	bool bSelectedPanelVisible = (m_pToolsPanel[m_nSelectedPanel] != NULL && pAGDoc);
	int nSelectedPanelShow = (bSelectedPanelVisible ? SW_SHOW : SW_HIDE);

	// New Tools Panel
	// maintained to sit to the left and is the first one
	for (int nIndex = 0; nIndex < NUM_PANELS; nIndex++)
	{
		if (m_pToolsPanel[nIndex])
		{
			m_pToolsPanel[nIndex]->ShowWindow(nIndex == m_nSelectedPanel ? nSelectedPanelShow : SW_HIDE);
			::GetWindowRect(m_pToolsPanel[nIndex]->GetTopToolPanel()->m_hWnd, &rToolsPanel);
			//bool bSmallToolsPanel = WIDTH(rToolsPanel) < WIDTH(rClient);
			//RECT rect = {0, 0, (bSmallToolsPanel ? WIDTH(rClient) : WIDTH(rToolsPanel)), HEIGHT(rToolsPanel)};
			//m_pToolsPanel[nIndex]->SetWindowPos(NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOZORDER | SWP_NOACTIVATE);
			RECT rect = {0, 0, WIDTH(rToolsPanel), HEIGHT(rToolsPanel)};
			m_pToolsPanel[nIndex]->SetWindowsPos(rect);
			rToolsPanel = rect;
		}
	}

	if (m_pPrintPanel)
	{
		m_pPrintPanel->ShowWindow(nSelectedPanelShow);
		::GetWindowRect(m_pPrintPanel->m_hWnd, &rPrintPanel);
		RECT rect = {rToolsPanel.right, 0, rClient.right, HEIGHT(rPrintPanel)};
		m_pPrintPanel->SetWindowPos(NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOZORDER | SWP_NOACTIVATE);
		rPrintPanel = rect;
	}

	if (m_pPagePanel)
	{ // Maintained to sit to the left of the control panel (when shown)
		m_pPagePanel->ShowWindow(nSelectedPanelShow);
		::GetWindowRect(m_pPagePanel->m_hWnd, &rPagePanel);
		RECT rect = {0, rToolsPanel.bottom*2, WIDTH(rPagePanel), (rToolsPanel.bottom*2) + HEIGHT(rPagePanel)};
		m_pPagePanel->SetWindowPos(NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOZORDER | SWP_NOACTIVATE);
		rPagePanel = rect;
	}

	if (m_pBlankPanel)
	{ // Maintained to sit to the right of the control panel (when shown)
		m_pBlankPanel->ShowWindow(nSelectedPanelShow);
		RECT rect = {rPagePanel.right, rPagePanel.top, rClient.right, rPagePanel.bottom};
		m_pBlankPanel->SetWindowPos(NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOZORDER | SWP_NOACTIVATE);
		rBlankPanel = rect;
	}

	if (m_pInfoPanel)
	{ // Maintained to sit to under the control panel (when shown)
		m_pInfoPanel->ShowWindow(nSelectedPanelShow);
		::GetWindowRect(m_pInfoPanel->m_hWnd, &rInfoPanel);
		RECT rect = {0, rPagePanel.bottom, rClient.right, rPagePanel.bottom + HEIGHT(rInfoPanel)};
		m_pInfoPanel->SetWindowPos(NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOZORDER | SWP_NOACTIVATE);
		rInfoPanel = rect;
	}

	if (m_pDocWindow)
	{ // Maintained to be the full size of the client area under the info panel
		int y = 0;
		if (bSelectedPanelVisible)
			y = (m_pDocWindow->SpecialsEnabled() ? rInfoPanel.bottom : rPagePanel.bottom);
		RECT rect = {0, y, rClient.right, rClient.bottom};
		m_pDocWindow->SetWindowPos(NULL, rect.left, rect.top, WIDTH(rect), HEIGHT(rect), SWP_NOZORDER | SWP_NOACTIVATE);
		m_pDocWindow->SetupPageView(true/*bSetScrollbars*/, false/*bOffset*/);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCtp::SelectPanel(UINT nPanel)
{
	m_nSelectedPanel = nPanel;

	bool bSelectedPanelVisible = (m_pToolsPanel[m_nSelectedPanel] != NULL);
	int nSelectedPanelShow = (bSelectedPanelVisible ? SW_SHOW : SW_HIDE);

	for (int nIndex = 0; nIndex < NUM_PANELS; nIndex++)
	{
		if (m_pToolsPanel[nIndex])
		{
			int nCmdShow = (nIndex == m_nSelectedPanel ? nSelectedPanelShow : SW_HIDE);
			m_pToolsPanel[nIndex]->ShowWindow(nCmdShow);
		}
	}

	if (m_pToolsPanel[m_nSelectedPanel])
		m_pToolsPanel[m_nSelectedPanel]->UpdateControls();
}

/////////////////////////////////////////////////////////////////////////////
CPagePanel* CCtp::GetPagePanel()
{ 
	return m_pPagePanel;
}

/////////////////////////////////////////////////////////////////////////////
CInfoPanel* CCtp::GetInfoPanel()
{ 
	return m_pInfoPanel;
}

/////////////////////////////////////////////////////////////////////////////
CToolsPanelContainer* CCtp::GetCtlPanel(UINT nPanel)
{ 
	if (nPanel == SELECTED_PANEL)
		return m_pToolsPanel[m_nSelectedPanel];

	return m_pToolsPanel[nPanel];
}

///////////////////////////////////////////////////////////////////////////////
//CToolsCtlPanel* CCtp::GetCtlPanel(UINT nPanel)
//{ 
//	if (nPanel == SELECTED_PANEL)
//		return m_pToolsPanel[m_nSelectedPanel];
//
//	return m_pToolsPanel[nPanel];
//}
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::get_Context(BSTR* bstrContext)
{
	return GetContext(bstrContext);
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::GetContext(BSTR* bstrContext)
{
	*bstrContext = CComBSTR(m_strContext).Copy();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::put_Context(BSTR bstrContext)
{
	return PutContext(bstrContext);
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::PutContext(BSTR bstrContext)
{
	// Each field in the context string must be unescaped as 
	// they are extracted from the string in GetOptionStringValue()
	m_strContext = CString(bstrContext);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::get_Src(BSTR* bstrSrcURL)
{
	return GetSrc(bstrSrcURL);
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::GetSrc(BSTR* bstrSrcURL)
{
	*bstrSrcURL = CComBSTR(m_strSrcURL).Copy();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::put_Src(BSTR bstrSrcURL)
{
	return PutSrc(bstrSrcURL);
}
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCtp::LoadSrc(VARIANT varSrc)
{
	return PutSrc(varSrc.bstrVal);;
}
/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::PutSrc(BSTR bstrSrcURL)
{
	CString strSrcURL(bstrSrcURL);
	int nPos = strSrcURL.Find("/Print");
	if (nPos >= 0)
	{
		strSrcURL = strSrcURL.Left(nPos);
		m_bAutoPrint = true;
	}

	UnescapeUrl(strSrcURL, m_strSrcURL);
	return SrcDownload(true/*bStart*/);
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::PutSrcURL(BSTR bstrSrcURL)
{
	UnescapeUrl(CString(bstrSrcURL), m_strSrcURL);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::SrcDownload(bool bStart)
{
	// Download the source document if the plugin window has been created
	// If not, the source document will be downloaded in the create method
	if (!m_pDocWindow)
		return S_OK;

	// If we were opened without a source, prompt the user for a file and open it
	if (m_strSrcURL.IsEmpty())
	{
		m_strSrcURL = m_pDocWindow->GetDocFileName();
		if (m_strSrcURL.IsEmpty())
		{
			// If the user cancels, lets clean things up and close up shop
			Free();
			AutoCloseBrowser();
			return E_FAIL;
		}
	}

	if (!m_Download.Init(0/*iFileType*/, CCtp::MyProjectDownloadCallback, (LPARAM)this))
		return S_OK;

	m_Download.AddFile(m_strSrcURL, CString(""), NULL);
	if (bStart)
		m_Download.Start(false/*bGoOnline*/);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::GetUsageData(BSTR* pstrUsageData)
{
	CString strURLData;

	strURLData = "memnum=" + GetContextMemberNumber();
	strURLData += "&";
	strURLData += "iteminfo=" + GetContextItemInfo();
	strURLData += "&";
	strURLData += "item="     + GetDBItem();
	strURLData += "&";
	strURLData += "copies=1";
	CComBSTR(strURLData).CopyTo(pstrUsageData);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::FYSClose()
{
	if (m_pDocWindow)
		m_pDocWindow->CloseModelessDlg();

	if (AutoPrintEnabled())
		AutoCloseBrowser();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::FYSTransfer(BSTR OrderId, BSTR CorrelationId, BSTR FYSInfo, BSTR* pstrError)
{
	if (!m_pDocWindow)
		return S_OK;

	CString strError;
	m_pDocWindow->DoFYSDataTransfer(OrderId, CorrelationId, FYSInfo, strError);
	CComBSTR(strError).CopyTo(pstrError);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::FileDownload(BSTR bstrSrcURL)
{ // The graphics file download function called by the Art Gallery's HTML dialog
	CString strSrcURL(bstrSrcURL);
	if (strSrcURL.IsEmpty())
		return S_OK;

	char szTempPath[MAX_PATH];
	::GetTempPath(sizeof(szTempPath), szTempPath);
	CString strDstPath = CString(szTempPath) + CString(_T("AgDownloads\\"));
	::CreateDirectory(strDstPath, NULL);
	int i = max(strSrcURL.ReverseFind('/'), strSrcURL.ReverseFind('\\'));
	strDstPath += strSrcURL.Mid(i+1);

	if (!m_Download.Init(1/*iFileType*/, CCtp::MyGraphicsDownloadCallback, (LPARAM)this))
		return S_OK;

	m_Download.AddFile(strSrcURL, strDstPath, NULL);
	m_Download.Start(true/*bGoOnline*/);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CCtp::FontListDownload(FONTDOWNLOADLIST& FontList)
{
	if (!m_Download.Init(1/*iFileType*/, CCtp::MyFontDownloadCallback, (LPARAM)this))
		return;

	CString strImgHost = GetContextImgHost();
	CString strSrcURL = strImgHost + "cp/fonts/";
	while (!FontList.empty())
	{
		int index = FontList[0];
		FontList.erase(FontList.begin() + 0);

		const char* pstrFontName = m_FontList.GetFontFileName(index);
		if (pstrFontName)
			m_Download.AddFile(strSrcURL + pstrFontName, CString(""), index);
	}

	m_Download.Start(true/*bGoOnline*/);
}

/////////////////////////////////////////////////////////////////////////////
bool CCtp::BeginSpellingDownload(CString& strDstPath)
{ 
	// The spelling download is initiated if the spelling dll and the dictionaries are not on the system
	CString strFileDLL;
	CString strFileMainLex1;
	CString strFileMainLex2;
	CString strFileUserLex;
	CSpell Spell(CAGDocSheetSize::GetMetric());
	Spell.GetFileNames(strFileDLL, strFileMainLex1, strFileMainLex2, strFileUserLex);

	if (!m_Download.Init(1/*iFileType*/, CCtp::MySpellingDownloadCallback, (LPARAM)this))
		return false;

	CString strImgHost = GetContextImgHost();
	CString strSrcURL = strImgHost + "cp/spell/";
	m_Download.AddFile(strSrcURL + strFileDLL,      strDstPath + strFileDLL, NULL);
	m_Download.AddFile(strSrcURL + strFileMainLex1, strDstPath + strFileMainLex1, NULL);
	m_Download.AddFile(strSrcURL + strFileMainLex2, strDstPath + strFileMainLex2, NULL);
	m_Download.AddFile(strSrcURL + strFileUserLex,  strDstPath + strFileUserLex, NULL);
	m_Download.Start(true/*bGoOnline*/);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CCtp::MyProjectDownloadCallback(LPARAM lParam, void* pDownloadVoid)
{
	CCtp* pCtp = (CCtp*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (!pDownload) // The final callback when there are no more files to download
	{
		pCtp->CreateBackPage();
		pCtp->ActivateBrowser();
		pCtp->ActivateNewDoc();
		return true;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CCtp::AutoCloseBrowser()
{
	// Lets close down the app
	::PostMessage(GetBrowser(), WM_CLOSE, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CCtp::MyFontDownloadCallback(LPARAM lParam, void* pDownloadVoid)
{
	CCtp* pCtp = (CCtp*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (!pDownload) // The final callback when there are no more files to download
	{
		// Update our own list of installed fonts
		pCtp->GetFontList().UpdateInstalledFontList();

		// Notify all the other application new fonts have been added
		::SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);

		if (pCtp->GetCtlPanel(TEXT_PANEL))
			((CTextPanelContainer*)pCtp->GetCtlPanel(TEXT_PANEL))->SetFonts();

		return true;
	}

	pCtp->GetFontList().InstallFont(pDownload->pData, pDownload->dwSize, pDownload->iData);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CCtp::MyGraphicsDownloadCallback(LPARAM lParam, void* pDownloadVoid)
{
	CCtp* pCtp = (CCtp*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (!pDownload) // The final callback when there are no more files to download
		return true;

	// Rename the file so it will be picked up by the directory monitor
	CString strDstPath = pDownload->strDstPath;
	CString strNewFileName = strDstPath + CString(_T(".agd"));
	if (!::MoveFile(strDstPath, strNewFileName)) // if the rename doesn't work...
		::DeleteFile(strDstPath); // get rid of the file

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CCtp::MySpellingDownloadCallback(LPARAM lParam, void* pDownloadVoid)
{
	CCtp* pCtp = (CCtp*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (!pDownload) // The final callback when there are no more files to download
	{
		CSpell Spell(CAGDocSheetSize::GetMetric());
		if (!Spell.Init(true/*bCheckFiles*/))
		{
			CMessageBox::Message("The spell check library failed to load.");
			return true;
		}

		CMessageBox::Message("The spell check library has been downloaded successfully.\n\nClick OK to begin checking your project.");
		if (pCtp)
			pCtp->m_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_SPELLCHECK);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetHomeFolder()
{
	char szAppPath[MAX_PATH];
	::GetModuleFileName(_AtlBaseModule.GetResourceInstance(), szAppPath, sizeof(szAppPath));

	CString strAppPath = szAppPath;
	int i = strAppPath.ReverseFind('\\');
	CString strHomeFolder = strAppPath.Left(i + 1);

	strAppPath.MakeLower();
	int iDebug = strAppPath.Find("\\debug\\");
	if (iDebug > 0)
		strHomeFolder = strHomeFolder.Left(iDebug + 1);
	int iRelease = strAppPath.Find("\\release\\");
	if (iRelease > 0)
		strHomeFolder = strHomeFolder.Left(iRelease + 1);

	return strHomeFolder;
}

/////////////////////////////////////////////////////////////////////////////
bool CCtp::IsApplication()
{
	CString strApp = GetContextApp();
	return (strApp.Find("app") >= 0);
}

/////////////////////////////////////////////////////////////////////////////
bool CCtp::IsAIM()
{
	CString strCpHost = GetContextCpHost();
	return (strCpHost.Find("aim.") >= 0);
}

/////////////////////////////////////////////////////////////////////////////
bool CCtp::IsBlueMountain()
{
	CString strCpHost = GetContextCpHost();
	if (strCpHost.Find("bluemountain") >= 0)
		return true;
	if (IsApplication() && strCpHost.Find("AppBm") >= 0)
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CCtp::IsAmericanGreetings()
{
	CString strCpHost = GetContextCpHost();
	if (strCpHost.Find("americangreetings") >= 0)
	{
		if (strCpHost.Find("msn.") >= 0)
			return false;
		return true;
	}
	//if (IsApplication() && strCpHost.Find("AppAg") >= 0)
	//	return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CCtp::IsDevelopmentServer()
{
	CString strCpHost = GetContextCpHost();
	return ((strCpHost.Find("work.") >= 0) ||
			(strCpHost.Find("stage.") >= 0) ||
			(strCpHost.Find("dev.") >= 0));
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetServerType()
{
	CString strCpHost = GetContextCpHost();
	strCpHost.MakeLower();

	if (strCpHost.Find("www.") >= 0) return "PROD";
	if (strCpHost.Find("stage.") >= 0) return "STAGE";
	if (strCpHost.Find("work.") >= 0) return "WORK";
	if (strCpHost.Find("dev.") >= 0) return "DEV";

	return "APP";
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetOptionStringValue(CString& strOptionString, CString& strNameTarget)
{
	CString strOptions = strOptionString;
	
	if (strOptions.IsEmpty())
		return CString("");

	strOptions += _T('&');

	int iStart = strOptions.Find(_T('?')) + 1;
	while (iStart >= 0)
	{
		int iEnd = strOptions.Find(_T('&'), iStart);
		if (iEnd <= iStart)
			break;
		int iPair = strOptions.Find(_T('='), iStart);
		if (iPair > iStart && iPair < iEnd)
		{
			CString strName = strOptions.Mid(iStart, iPair-iStart);
			strName.TrimLeft();
			strName.TrimRight();
			strName.MakeLower();
			if (strName == strNameTarget)
			{
				iPair++;
				CString strValue = strOptions.Mid(iPair, iEnd-iPair);
				strValue.TrimLeft();
				strValue.TrimRight();
				UnescapeUrl(strValue, strValue);
				return strValue;
			}
		}

		iStart = iEnd + 1;
	}

	return CString("");
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetContextApp()
{
	return GetOptionStringValue(m_strContext, CString(_T("app")));
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetContextUser()
{
	return GetOptionStringValue(m_strContext, CString(_T("user")));
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetContextMemberNumber()
{
	return GetOptionStringValue(m_strContext, CString(_T("memnum")));
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetContextMemberStatus()
{
	return GetOptionStringValue(m_strContext, CString(_T("memstat")));
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetContextItemInfo()
{
	return GetOptionStringValue(m_strContext, CString(_T("iteminfo")));
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetContextCallProc()
{
	return GetOptionStringValue(m_strContext, CString(_T("callproc")));
}
/////////////////////////////////////////////////////////////////////////////
int CCtp::GetContextRemotePrintStatus()
{
	CString szValue = GetOptionStringValue(m_strContext, CString(_T("remote")));
	if (szValue.IsEmpty())
		return -1;

	return atoi(szValue);
}
/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetContextContentStatus()
{
	CString Price = "0.0";
	CString ItemInfo = GetContextItemInfo();
	if (ItemInfo != "")
	{
		ItemInfo = ItemInfo.Mid(ItemInfo.Find("%7")+3);
		ItemInfo = ItemInfo.Mid(ItemInfo.Find("%7")+3);
		ItemInfo = ItemInfo.Mid(ItemInfo.Find("%7")+3);
		Price = ItemInfo.Left(ItemInfo.Find("%7"));
	}

	return Price;
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetContextProductNumber()
{
	CString ItemInfo = GetContextItemInfo();
	return ItemInfo.Left(ItemInfo.Find("%7"));
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetContextCpHost()
{
	CString strOption = GetOptionStringValue(m_strContext, CString(_T("cphost")));
	if (strOption.IsEmpty())
		strOption = "http://www.americangreetings.com/cnp/";
	else
		strOption += "/";

	return strOption;
}

/////////////////////////////////////////////////////////////////////////////
CString CCtp::GetContextImgHost()
{
	CString strCpHost = GetContextCpHost();
	bool bAOL = (strCpHost.Find("aol") >= 0);

	CString strOption = GetOptionStringValue(m_strContext, CString(_T("imghost")));
	if (strOption.IsEmpty() || bAOL)
		strOption = "http://www.imgag.com/";
	else
		strOption += "/";

	return strOption;
}

/////////////////////////////////////////////////////////////////////////////
int CCtp::GetContextLicense()
{
	// The license option can either be None (0), Disney (283) or Undefined (-1)
	CString szValue = GetOptionStringValue(m_strContext, CString(_T("license")));
	if (szValue.IsEmpty())
		return -1;

	return atoi(szValue);
}

/////////////////////////////////////////////////////////////////////////////
int CCtp::GetContextMetric()
{
	// The metric option can either be true (1), false (0), or content specific (-1)
	return atoi(GetOptionStringValue(m_strContext, CString(_T("metric"))));
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::GetVerse(BSTR bstrFilePath, BSTR* bstrVerse)
{
	bool bNeedNewDoc = (bstrFilePath && CComBSTR(bstrFilePath) != "");
	CAGDoc* pAGDoc = (bNeedNewDoc ? new CAGDoc() : (!m_pDocWindow ? NULL : m_pDocWindow->GetDoc()));
	if (!pAGDoc)
	{
		*bstrVerse = CComBSTR("").Copy(); // "Error creating new project"
		return S_OK;
	}

	if (bNeedNewDoc)
	{
		ifstream In(CString(bstrFilePath), ios::in | /*ios::nocreate |*/ ios::binary);

		bool bAdjusted = false;
		if (!pAGDoc->Read(In, bAdjusted))
		{
			*bstrVerse = CComBSTR("").Copy(); // "Error reading project file"
			if (bNeedNewDoc)
				delete pAGDoc;
			return S_OK;
		}
	}

	if (!pAGDoc->NeedsCardBack())
	{
		*bstrVerse = CComBSTR("").Copy(); // "Error: Not a card project"
		if (bNeedNewDoc)
			delete pAGDoc;
		return S_OK;
	}

	bool bCoverText = false;
	bool bInsideText = false;
	CString strProjectText;
	CString strCoverText(_T("<b>Cover Verse:</b><br><br>"));
	CString strDivide(_T("<br>"));
	CString strInsideText(_T("<b>Inside Verse:</b><br><br>"));
	int nPages = pAGDoc->GetNumPages();
	for (int nPage = 0; nPage < 3/*nPages*/; nPage++)
	{
		CAGPage* pPage = pAGDoc->GetPage(nPage);
		if (!pPage)
			continue;

		int nLayers = pPage->GetNumLayers();
		for (int nLayer = 0; nLayer < nLayers; nLayer++)
		{
			CAGLayer* pLayer = pPage->GetLayer(nLayer);
			if (!pLayer)
				continue;

			int nSymbols = pLayer->GetNumSymbols();
			for (int nSymbol = 0; nSymbol < nSymbols; nSymbol++)
			{
				CAGSym* pSym = pLayer->GetSymbol(nSymbol);
				if (!pSym || pSym->IsHidden() || !pSym->IsText())
					continue;

				CAGSymText* pText = static_cast<CAGSymText*>(pSym);
				if (!pText)
					continue;

				// Ignore special cardback text symbols
				if (pText->GetID() == IDR_CARDBACK_COPYRIGHT)
					continue;

				CString strText = pText->ExtractText();
				if (strText.IsEmpty())
					continue;
				if (strText.Find('©') >= 0)
					continue;

				strText.Replace('\r', '\n');
				strText.Replace("\n", "<br>");
				if (nPage == 0 && !bCoverText)
				{
					strProjectText = strCoverText;
					bCoverText = true;
				}
				if (nPage > 0 && !bInsideText)
				{
					if (!strProjectText.IsEmpty())
						strProjectText += strDivide;
					strProjectText += strInsideText;
					bInsideText = true;
				}
				
				strProjectText += strText;
				strProjectText += strDivide;
			}
		}
	}

	*bstrVerse = CComBSTR(strProjectText).Copy();

	if (bNeedNewDoc)
		delete pAGDoc;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::GetAttributes(BSTR bstrFilePath, BSTR* bstrAttributes)
{
	bool bNeedNewDoc = (bstrFilePath && CComBSTR(bstrFilePath) != "");
	CAGDoc* pAGDoc = (bNeedNewDoc ? new CAGDoc() : (!m_pDocWindow ? NULL : m_pDocWindow->GetDoc()));
	if (!pAGDoc)
	{
		*bstrAttributes = CComBSTR("").Copy(); // "Error creating new project"
		return S_OK;
	}

	if (bNeedNewDoc)
	{
		ifstream In(CString(bstrFilePath), ios::in | /*ios::nocreate |*/ ios::binary);

		bool bAdjusted = false;
		if (!pAGDoc->Read(In, bAdjusted))
		{
			*bstrAttributes = CComBSTR("").Copy(); // "Error reading project file"
			if (bNeedNewDoc)
				delete pAGDoc;
			return S_OK;
		}
	}

	// Save the metric attribute
	bool bMetric = CAGDocSheetSize::GetMetric();
	{
		SIZE OrigPageSize = {0,0};
		pAGDoc->GetOrigPageSize(OrigPageSize);

		CAGDocSheetSize::SetMetric(true);
		SIZE ClosestSizeMetric = OrigPageSize;
		bool bNeedsAdjustment = CPaperTemplates::AdjustSize(pAGDoc->GetDocType(), ClosestSizeMetric);
		ClosestSizeMetric.cx -= OrigPageSize.cx;
		ClosestSizeMetric.cy -= OrigPageSize.cy;

		CAGDocSheetSize::SetMetric(false);
		SIZE ClosestSizeUS = OrigPageSize;
		bNeedsAdjustment = CPaperTemplates::AdjustSize(pAGDoc->GetDocType(), ClosestSizeUS);
		ClosestSizeUS.cx -= OrigPageSize.cx;
		ClosestSizeUS.cy -= OrigPageSize.cy;

		// Restore the metric attribute
		CAGDocSheetSize::SetMetric(bMetric);

		bMetric = (abs(ClosestSizeMetric.cx) + abs(ClosestSizeMetric.cy) < 
				   abs(ClosestSizeUS.cx) + abs(ClosestSizeUS.cy));
	}

	CString strAttributes;
	strAttributes.Format("metric=%d", bMetric);
	*bstrAttributes = CComBSTR(strAttributes).Copy();

	if (bNeedNewDoc)
		delete pAGDoc;
	return S_OK;
}

#include "Image.h"
#include "ScaleImage.h"
/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::ProcessImage(BSTR bstrFilePath, int nMaxKB, int nQuality, BSTR* bstrFilePathNew)
{
	// Set the return value in case we leave early
	*bstrFilePathNew = CComBSTR(bstrFilePath).Copy();

	// Create an image object
	CString strFilePath = CString(bstrFilePath);
	CImage Image(strFilePath);
	
	// Get a Dib pointer from the image
	BITMAPINFOHEADER* pDibSrc = Image.GetDibPtr();
	if (!pDibSrc)
		return S_OK;

	// Copy the source Dib header into a destination Dib
	BITMAPINFOHEADER DibDst = *pDibSrc;

	// Define a maximum rectangle
	int iMaxWidth = 400;
	int iMaxHeight = 400;

	// Scale the Dib to fit within the maximum rectangle
	double fxScale = (!DibDst.biWidth ? 1.0 : (double)iMaxWidth / DibDst.biWidth);
	double fyScale = (!DibDst.biHeight ? 1.0 : (double)iMaxHeight / DibDst.biHeight);
	double fScale = min(fxScale, fyScale);

	// Make it fit
	DibDst.biWidth  = (long)(fScale * DibDst.biWidth);
	DibDst.biHeight = (long)(fScale * DibDst.biHeight);
	DibDst.biSizeImage = 0;

	// Allocate memory for the new Dib
	BITMAPINFOHEADER* pDibDst = (BITMAPINFOHEADER*)malloc(DibSize(&DibDst));
	if (!pDibDst)
		return S_OK;

	// Copy the Dib header
	*pDibDst = DibDst;

	// Resize the Dib
	ScaleImage(pDibSrc, pDibDst);

	// Generate a temporary Jpg file
	CString strFilePathNew = RandomFileName("ag", ".jpg");

	// Convert the Dib in memory to a Jpg file
	bool bOK = ConvertDibToJpg(pDibDst, nQuality, strFilePathNew);
	if (bOK)
		*bstrFilePathNew = CComBSTR(strFilePathNew).Copy();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::DoCommand(BSTR bstrCommand, long* plResult)
{
	if (!plResult)
		return E_FAIL;

	if (!m_pDocWindow)
		return E_FAIL;

	if (!m_hWnd)
	{
		RECT rect = {50, 50, 750, 550};
		DWORD dwStyle = WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		DWORD dwExStyle = 0;
		UINT nId = 0;
		LPVOID lpCreateParam = NULL;
		HWND hWndParent = GetDesktopWindow();
		Create(hWndParent, rect, g_szAppName, dwStyle, dwExStyle, nId, lpCreateParam);
	}

	CString strCommand = bstrCommand;
	strCommand.MakeUpper();
	*plResult = m_pDocWindow->DoCommand(strCommand);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CCtp::GetCommand(BSTR bstrCommand, long* plResult)
{
	if (!plResult)
		return E_FAIL;

	if (!m_pDocWindow)
		return E_FAIL;

	// The return value of this function is a set of bitwise flags about the command
	// Bit 1 = Enabled
	// Bit 2 = Checked

	CString strCommand = bstrCommand;
	strCommand.MakeUpper();
	*plResult = m_pDocWindow->GetCommand(strCommand);
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// IViewObjectEx
HRESULT CCtp::GetViewStatus(DWORD* pdwStatus)
{
	ATLTRACE(_T("IViewObjectExImpl::GetViewStatus\n"));
	*pdwStatus = 0;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IObjectSafety
HRESULT CCtp::GetInterfaceSafetyOptions(REFIID riid, DWORD* pdwSupportedOptions, DWORD* pdwEnabledOptions)
{
	if (!pdwSupportedOptions || !pdwEnabledOptions)
		return E_POINTER;

	HRESULT hr = S_OK;
	if (riid == IID_IDispatch)
	{
		*pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
		*pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
	}
	else
	{
		*pdwSupportedOptions = 0;
		*pdwEnabledOptions = 0;
		hr = E_NOINTERFACE;
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// IObjectSafety
HRESULT CCtp::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
{
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IDropTarget interface
STDMETHODIMP CCtp::Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	if (!pDataObject)
		return S_OK;

	if (!m_pDocWindow)
		return S_OK;

	if (m_pDocWindow->m_pEnvelopeWizard)
	{
		m_pDocWindow->m_pEnvelopeWizard->FinishImageDrop(true/*bDrop*/, pt);
		return S_OK;
	}

	CComPtr<IEnumFORMATETC> pEnumFmt;
	HRESULT hr = pDataObject->EnumFormatEtc(DATADIR_GET, &pEnumFmt);
	TCHAR szBuf[100];
	FORMATETC Format1;
	while (pEnumFmt->Next(1, &Format1, NULL) == S_OK)
	{
		::GetClipboardFormatName(Format1.cfFormat, szBuf, sizeof(szBuf));
	}

	STGMEDIUM Medium;
	::ZeroMemory(&Medium, sizeof(Medium));
	FORMATETC Format;
	::ZeroMemory(&Format, sizeof(Format));
	Format.dwAspect = DVASPECT_CONTENT;
	Format.lindex  = -1;
	Format.tymed = TYMED_HGLOBAL;

	Format.cfFormat = CF_HDROP;
	hr = pDataObject->GetData(&Format, &Medium);
	if (FAILED(hr))
	{
		Format.cfFormat = ::RegisterClipboardFormat((LPSTR)"FileGroupDescriptor");
		hr = pDataObject->GetData(&Format, &Medium);
		if (FAILED(hr))
		{
			Format.cfFormat = ::RegisterClipboardFormat((LPSTR)"FileContents");
			hr = pDataObject->GetData(&Format, &Medium);
			if (FAILED(hr))
			{
				Format.cfFormat = CF_HDROP;
				Format.tymed = TYMED_FILE;
				hr = pDataObject->GetData(&Format, &Medium);
				if (FAILED(hr))
				return S_OK;
			}
		}
	}

	if (Format.cfFormat == ::RegisterClipboardFormat((LPSTR)"FileGroupDescriptor")) // Test for Kodak?
	{
		HGLOBAL hMemory = Medium.hGlobal;
		if (!hMemory)
			return S_OK;

		FILEGROUPDESCRIPTOR* FileGroupDescriptor = (FILEGROUPDESCRIPTOR*)::GlobalLock(hMemory);
		FILEDESCRIPTOR& FileDescriptor = FileGroupDescriptor->fgd[0];
		::GlobalUnlock(hMemory);
	}
	else
	if (Format.cfFormat == ::RegisterClipboardFormat((LPSTR)"FileContents")) // Test for Kodak?
	{
		HGLOBAL hMemory = Medium.hGlobal;
		if (!hMemory)
			return S_OK;

		char* pMemory = (char*)::GlobalLock(hMemory);
		::GlobalUnlock(hMemory);
	}
	else
	if (Format.cfFormat == CF_HDROP)
	{
		if (Format.tymed & TYMED_HGLOBAL)
		{
			// Get the HDROP data from the data object
			HGLOBAL hMemory = Medium.hGlobal;
			if (!hMemory)
				return S_OK;

			HDROP hDrop = (HDROP)::GlobalLock(hMemory);
			if (!hDrop)
			{
				::GlobalUnlock(hMemory);
				return S_OK;
			}

			// Read in the list of files here...
			char szFileName[MAX_PATH];
			int iFilesDropped = ::DragQueryFile(hDrop, -1, NULL, 0);
			for (int i = 0; i < iFilesDropped; i++)
			{
				int iStringLength = ::DragQueryFile(hDrop, i, NULL, 0);
				::DragQueryFile(hDrop, i, szFileName, MAX_PATH);

				CString strFileName = szFileName;
				CString strExtension = strFileName;
				strExtension = strExtension.Right(4);
				strExtension.MakeLower();
				bool bCtpFile = (strExtension == ".ctp");

				LRESULT lResult = S_OK;
				if (bCtpFile)
				{
					lResult = m_pDocWindow->OpenDoc(strFileName);
					break;
				}
				else
				{
					m_pDocWindow->DoDropImage(strFileName, pt);
//					lResult = m_pDocWindow->AddImage(strFileName, 0/*iOffset*/, pt.x, pt.y, true);
				}
			}

			::GlobalUnlock(hMemory);
		}
		else
		if (Format.tymed & TYMED_FILE)
		{
			if (!Medium.lpszFileName)
				return S_OK;

			CString strFileName = Medium.lpszFileName;
			CString strExtension = strFileName;
			strExtension = strExtension.Right(4);
			strExtension.MakeLower();
			bool bCtpFile = (strExtension == ".ctp");

			LRESULT lResult = S_OK;
			if (bCtpFile)
				lResult = m_pDocWindow->OpenDoc(strFileName);
			else
			{
				m_pDocWindow->DoDropImage(strFileName, pt);
//				lResult = m_pDocWindow->AddImage(strFileName, 0/*iOffset*/, pt.x, pt.y, true);
			}
		}
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCtp::DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) 
{
	*pdwEffect = DROPEFFECT_COPY;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCtp::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	*pdwEffect = DROPEFFECT_COPY;

	m_pDocWindow->DoDragOver(pt);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCtp::DragLeave(void)
{
	m_pDocWindow->DoDragLeave();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CCtp::DoDragDrop()
{
	DWORD dwEffect;
	::DoDragDrop(this, this, DROPEFFECT_COPY, &dwEffect);
}

/////////////////////////////////////////////////////////////////////////////
// IDropSource interface
STDMETHODIMP CCtp::QueryContinueDrag(BOOL fEscapePressed,  DWORD grfKeyState)
{
//j	::MessageBox(NULL, "QueryContinueDrag", "", MB_OK);
	if (!(grfKeyState & MK_LBUTTON))
		return DRAGDROP_S_DROP;

	if (fEscapePressed)
	{
		if (m_pDocWindow && m_pDocWindow->m_pEnvelopeWizard)
		{
			//Clean up from drag
			POINTL pt;
			m_pDocWindow->m_pEnvelopeWizard->FinishImageDrop(false/*bDrop*/, pt);
		}
		return DRAGDROP_S_CANCEL;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCtp::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

/////////////////////////////////////////////////////////////////////////////
// IShellExtInit interface
STDMETHODIMP CCtp::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID)
{
//j	::MessageBox(NULL, "Initialize", "", MB_OK);
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP     hDrop;

    // Look for CF_HDROP data in the data object.
    if (FAILED(pDataObj->GetData(&fmt, &stg)))
    {
        // Nope! Return an "invalid argument" error back to Explorer.
        return E_INVALIDARG;
    }

    // Get a pointer to the actual data.
    hDrop = (HDROP)::GlobalLock(stg.hGlobal);

    // Make sure it worked.
    if (!hDrop)
        return E_INVALIDARG;

    // Sanity check – make sure there is at least one filename.
	UINT uNumFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
    if (!uNumFiles)
    {
        GlobalUnlock(stg.hGlobal);
        ReleaseStgMedium(&stg);
        return E_INVALIDARG;
     }

	HRESULT hr = S_OK;

    // Get the name of the first file and store it
    if (!DragQueryFile(hDrop, 0, m_szPrintFile, MAX_PATH))
        hr = E_INVALIDARG;

    ::GlobalUnlock(stg.hGlobal);
    ReleaseStgMedium(&stg);

    return hr;
}

#ifdef NOTUSED //j
/////////////////////////////////////////////////////////////////////////////
// IContextMenu interface

#define PRINT_COMMAND_OFFSET 0

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCtp::QueryContextMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
//j	::MessageBox(NULL, "QueryContextMenu", "", MB_OK);
	// If the flags	include	CMF_DEFAULTONLY	then we	shouldn't do anything.
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd + PRINT_COMMAND_OFFSET, _T("Print card	project"));
	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCtp::GetCommandString(UINT  idCmd, UINT  uFlags, UINT* pwReserved, LPSTR pszName, UINT  cchMax)
{
//j	::MessageBox(NULL, "GetCommandString", "", MB_OK);
	USES_CONVERSION;

	// Check idCmd,	it must	be 0 since we have only	one	menu item.
	if (idCmd)
		return E_INVALIDARG;

	// If Explorer is asking for a help	string,	copy our string	into the
	// supplied	buffer.
	if (uFlags & GCS_HELPTEXT)
	{
		LPCTSTR	szText = _T("This is the simple	shell extension's help");

		if (uFlags & GCS_UNICODE)
		{
			// We need to cast pszName to a	Unicode	string,	and	then use the Unicode string copy API.
			lstrcpynW((LPWSTR) pszName,	T2CW(szText), cchMax);	
		}
		else
		{
			// Use the ANSI	string copy	API	to return the help string.
			lstrcpynA(pszName, T2CA(szText), cchMax);
		}

		return S_OK;
	}

	return E_INVALIDARG;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCtp::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
//j	::MessageBox(NULL, "InvokeCommand", "", MB_OK);
	// If lpVerb really	points to a	string,	ignore this	function call and bail out.
	if (HIWORD(pCmdInfo->lpVerb))
		return E_INVALIDARG;

	// Get the command index - the only	valid one is 0.
	switch (LOWORD(pCmdInfo->lpVerb))
	{
		case PRINT_COMMAND_OFFSET:
		{
			CString	szFilePath = CString("http://www.americangreetings.com/cnp/cporder.pd?product=") + m_szPrintFile + CString("/Print");
			ShellExecute(m_hWnd, "open", szFilePath,  NULL,	NULL, SW_SHOWNORMAL);
			break;
		}

		default:
			return E_INVALIDARG;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCtp::HandleMenuMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//j	::MessageBox(NULL, "HandleMenuMsg", "", MB_OK);
	ATLTRACE(_T("CCtp ---> IContextMenu2 interface method HandleMenuMsg() called!\n"));
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CCtp::HandleMenuMsg2(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
{
//j	::MessageBox(NULL, "HandleMenuMsg2", "", MB_OK);
	ATLTRACE(_T("CCtp ---> IContextMenu3 interface method HandleMenuMsg2() called!\n"));
	return S_OK;
}
#endif NOTUSED //j

