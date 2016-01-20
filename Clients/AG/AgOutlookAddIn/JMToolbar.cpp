#include "StdAfx.h"
#include "Resource.h"
#include "HelperFunctions.h"
#include "JMToolbar.h"

const UINT ID_OEMAIL_SEND = 0x09ddb; //Identifier for Outlook Express Send button.

UINT ID_JMTOOLBAR	= ::RegisterWindowMessage("Creata Mail Toolbar");
UINT IDT_REBAR_TIMER = ::RegisterWindowMessage("Creata Mail Rebar Timer");

WNDPROC g_pfOutlookExWndProc	= NULL;
WNDPROC g_pfComposeWndProc		= NULL;
WNDPROC g_pfRebarWndProc		= NULL;
WNDPROC g_pfToolbarWndProc		= NULL;
WNDPROC g_pfSysTab32WndProc		= NULL;

extern CComPtr<IOEAddin> g_spOEAddin;
extern HWND g_hOEAddin;

//=========================================================================
// OutlookExWndProc
//=========================================================================
LRESULT CALLBACK OutlookExWndProc(HWND hwndOutlookEx, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
    {
		case WM_CLOSE:
		{
			if (InSendMessage())
				ReplyMessage(TRUE); 
			SendMessage(g_hOEAddin, UWM_TBDESTROY, (WPARAM)TYPE_TOPPARENT, (LPARAM)hwndOutlookEx);
			ATLTRACE2( atlTraceGeneral, 4, "OutlookExWndProc: WM_CLOSE \n");
			break;	
		}

		case WM_DESTROY:
		{
			g_spOEAddin->OnShutDown();
			RestoreWindowProc(hwndOutlookEx, g_pfOutlookExWndProc); 
			ATLTRACE2( atlTraceGeneral, 4, "OutlookExWndProc: WM_NOTIFY \n");
			break;
		}

		case WM_NCDESTROY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "OutlookExWndProc: WM_NOTIFY \n");
			break;
		}
		
		default:
			break;
	}

	return CallWindowProc(g_pfOutlookExWndProc, hwndOutlookEx, uMsg, wParam, lParam);
}

//=========================================================================
// ComposeWndProc
//=========================================================================
LRESULT CALLBACK ComposeWndProc(HWND hwndCompose, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
    {
		case WM_ACTIVATE:
		{
			if (g_spOEAddin == NULL)
				break;

			if (LOWORD(wParam) != WA_INACTIVE)
			{
				g_spOEAddin->SetActiveCompose((OLE_HANDLE)hwndCompose, WA_ACTIVE);
			}
			
			ATLTRACE2( atlTraceGeneral, 4, "ComposeWndProc: WM_ACTIVATE \n");
			break;
		}

		case WM_CLOSE:
		{
			// Do all cleanup during the destroy, since the user could cancel the close
			ATLTRACE2( atlTraceGeneral, 4, "ComposeWndProc: WM_CLOSE \n");
			break;	
		}

		case WM_DESTROY:
		{
			if (g_spOEAddin == NULL)
				break;

			g_spOEAddin->SetActiveCompose((OLE_HANDLE)hwndCompose, WA_INACTIVE);
			if (InSendMessage())
				ReplyMessage(TRUE); 
			SendMessage(g_hOEAddin, UWM_TBDESTROY, (WPARAM)TYPE_TOPPARENT, (LPARAM)hwndCompose);
			ATLTRACE2( atlTraceGeneral, 4, "ComposeWndProc: WM_NOTIFY \n");
			break;
		}

		case WM_NCDESTROY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "ComposeWndProc: WM_NOTIFY \n");
			break;
		}
		
		default:
			break;
	}

	return CallWindowProc(g_pfComposeWndProc, hwndCompose, uMsg, wParam, lParam);
}

//=========================================================================
// SysTab32WndProc
//=========================================================================
LRESULT CALLBACK SysTab32WndProc(HWND hwndSysTab32, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	ATLTRACE2( atlTraceGeneral, 4, "SysTab32WndProc : uMsg %X\n", uMsg);
	switch (uMsg)
    {
		case WM_PARENTNOTIFY :
		{
			ATLTRACE2( atlTraceGeneral, 4, "WM_PARENTNOTIFY : WM_COMMAND \n");
			break;
		}

		case WM_PAINT:
		{
			ATLTRACE2( atlTraceGeneral, 4, "WM_PAINT: WM_COMMAND \n");
			break;
		}

		case WM_ENABLE:
		{
			ATLTRACE2( atlTraceGeneral, 4, "WM_ENABLE: WM_COMMAND \n");
			break;
		}

		case WM_KILLFOCUS:
		{
			ATLTRACE2( atlTraceGeneral, 4, "WM_KILLFOCUS: WM_COMMAND \n");
			break;
		}

		case WM_COMMAND:
		{
			ATLTRACE2( atlTraceGeneral, 4, "IEServerWndProc: WM_COMMAND \n");
			break;
		}

		case WM_NOTIFY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "IEServerWndProc: WM_NOTIFY \n");
			break;
		}

		case WM_CREATE:
		{
			ATLTRACE2( atlTraceGeneral, 4, "IEServerWndProc: WM_CREATE \n");
			break;
		}

		case WM_LBUTTONUP:
		{
			PostMessage(g_hOEAddin, UWM_REFRESH, 0, (LPARAM)hwndSysTab32);
			ATLTRACE2( atlTraceGeneral, 4, "IEServerWndProc: WM_ACTIVATE \n");
			break;
		}

		case WM_NCACTIVATE:
		{
			ATLTRACE2( atlTraceGeneral, 4, "WM_NCACTIVATE: WM_COMMAND \n");
			break;
		}

		case WM_CLOSE:
		{
			ATLTRACE2( atlTraceGeneral, 4, "IEServerWndProc: WM_CLOSE \n");
			break;
		}

		case WM_DESTROY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "IEServerWndProc: WM_NOTIFY \n");
			break;
		}

		case WM_NCDESTROY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "IEServerWndProc: WM_NOTIFY \n");
			break;
		}

		default:
			break;
	}

	return CallWindowProc(g_pfSysTab32WndProc, hwndSysTab32, uMsg, wParam, lParam);
}

//=============================================================================
//	RebarWindowProc
//=============================================================================
LRESULT CALLBACK RebarWindowProc(HWND hwndRebar, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uiMsg)
    {
		case WM_CREATE:
		{
			ATLTRACE2( atlTraceGeneral, 4, "RebarWindowProc: WM_CREATE \n");
			break;
		}

		case WM_TIMER:
		{
			if (IDT_REBAR_TIMER == wParam)
			{
				::PostMessage(g_hOEAddin, UWM_MOVEBAND, 0, (LPARAM)hwndRebar);
				::KillTimer(hwndRebar, IDT_REBAR_TIMER);
			}
			break;
		}

		case WM_CLOSE:
		{	
			ATLTRACE2( atlTraceGeneral, 4, "RebarWindowProc: WM_CLOSE \n");
			break;
		}

		case WM_DESTROY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "RebarWindowProc: WM_DESTROY \n");	
			break;
		}

		case WM_NCDESTROY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "RebarWindowProc: WM_NCDESTROY:  \n");
			break;
		}

		case WM_NOTIFY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "RebarWindowProc: WM_NOTIFY \n ((LPNMHDR) lParam)->code X\n", ((LPNMHDR)lParam)->code);
			switch (((LPNMHDR)lParam)->code)
			{ 
				case TTN_NEEDTEXTA:
				{ // Return the tooltip text.
					LPTOOLTIPTEXTA lpToolTipText = (LPTOOLTIPTEXTA)lParam;
					int idButton = lpToolTipText->hdr.idFrom;
					int iOffset = idButton - ID_BTN1_CMD;
					int idString = IDS_BTN1_TIP + iOffset;
					CString strText;
					strText.LoadString(idString);
					lpToolTipText->lpszText = (LPSTR)(LPCSTR)strText;
					return 0; // if we break, the tooltips don't work
				}

				case TTN_NEEDTEXTW:
				{ // Return the tooltip text.
					LPTOOLTIPTEXTW lpToolTipText = (LPTOOLTIPTEXTW)lParam;
					int idButton = lpToolTipText->hdr.idFrom;
					int iOffset = idButton - ID_BTN1_CMD;
					int idString = IDS_BTN1_TIP + iOffset;
					CString strText;
					strText.LoadString(idString);
					lpToolTipText->lpszText = CComBSTR(strText);
					return 0; // if we break, the tooltips don't work
				}

				default: 
					break; 
			}

			break;
		}

		case WM_COMMAND:
		{
			if (g_spOEAddin == NULL)
				break;

			ATLTRACE2( atlTraceGeneral, 4, "RebarWindowProc: WM_COMMAND \n");
			BOOL bSuccess; 
			UINT uCode = HIWORD(wParam);
			UINT uID = LOWORD(wParam);
			
			switch (uID)
			{
				case ID_BTN1_CMD:
				case ID_BTN2_CMD:
				case ID_BTN3_CMD:
				case ID_BTN4_CMD:
				case ID_BTN5_CMD:
				case ID_BTN6_CMD:
				case ID_BTN7_CMD:
				case ID_BTN8_CMD:
				case ID_BTN9_CMD:
				case ID_BTN10_CMD:
				case ID_BTN11_CMD:
				{
					g_spOEAddin->OnOEButtonClick(uID, (OLE_HANDLE)hwndRebar, (VARIANT_BOOL *)&bSuccess);
					break;
				}
				case ID_OEMAIL_SEND:
				{
					g_spOEAddin->OnItemSend();
					break;
				}
				default:
					break;
			} 
			break;
		}

		default: 
			break; 
	}
	
	return CallWindowProc(g_pfRebarWndProc, hwndRebar, uiMsg, wParam, lParam);
}

//=========================================================================
// ToolbarWndProc
//=========================================================================
LRESULT CALLBACK ToolbarWndProc(HWND hwndToolbar, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
    {
		case WM_DESTROY:
		{
			//if (InSendMessage())
			//	ReplyMessage(TRUE); 
			//SendMessage(g_hOEAddin, UWM_TBDESTROY, (WPARAM)TYPE_TOOLBAR, (LPARAM)hwndToolbar);
			ATLTRACE2( atlTraceGeneral, 4, "ToolbarWndProc: WM_DESTROY \n");
			break;
		}

		case WM_NCDESTROY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "ToolbarWndProc: WM_NCDESTROY:  \n");	
			break;
		}

		case WM_COMMAND:
		{
			ATLTRACE2( atlTraceGeneral, 4, "ToolbarWndProc: WM_COMMAND \n");
			break;
		}

		case WM_NOTIFY:
		{
			ATLTRACE2( atlTraceGeneral, 4, "ToolbarWndProc: WM_NOTIFY \n");
			break;
		}

		case WM_CLOSE:
		{
			ATLTRACE2( atlTraceGeneral, 4, "ToolbarWndProc: WM_CLOSE \n");
			break;
		}

		default:
			break;
	}

	return CallWindowProc(g_pfToolbarWndProc, hwndToolbar, uMsg, wParam, lParam);
}

//=========================================================================
// CJMToolbar
//=========================================================================
CJMToolbar::CJMToolbar(void)
{
	m_bInitialized = FALSE;
	m_bToolbarCreated = FALSE;
	m_uOwnerClass = 0;
	m_uTimerID = 0;
	m_hTopParent = NULL;
	m_hRebarWnd = NULL;
	m_hToolbar = NULL;
	ATLTRACE2( atlTraceGeneral, 4, "CJMToolbar: Construct\n");
}

//=========================================================================
// ~CJMToolbar
//=========================================================================
CJMToolbar::~CJMToolbar(void)
{
	if (Cleanup())
		ATLTRACE2( atlTraceGeneral, 4, "CJMToolbar::Cleanup() FAILED: ~CJMToolbar()");
	else
		ATLTRACE2( atlTraceGeneral, 4, "CJMToolbar::Cleanup() SUCCEEDED: ~CJMToolbar()");
}

//================================================
// Cleanup
//================================================
BOOL CJMToolbar::Cleanup(void)
{
	BOOL bSuccess = FALSE;

	// Remove band from rebar. This has to be 
	// performed before the rebar receives the 
	// WM_DESTROY following WM_CLOSE. This is 
	// because of a problem on Windows XP machines
	// - common control bug/feature.
	bSuccess = RemoveBand();
	_ASSERTE(bSuccess);

	// If toolbar window still around remove.
	if (IsWindow())
	{
		SetParent(NULL);
		Detach();
	}
	
	// Destroy toolbar's image list.
	if (m_himlBtns)
	{
		ImageList_Destroy(m_himlBtns);
		m_himlBtns = NULL;
	}
	
	return bSuccess;
}

//================================================
// GetBandIndex
//================================================
void CJMToolbar::GetBandIndex(INT &iBandIndex)
{
	if (!::IsWindow(m_hRebarWnd))
		return;

	iBandIndex = -1;
	UINT uBandCount = SendMessage(m_hRebarWnd, RB_GETBANDCOUNT, 0, 0);
	REBARBANDINFO rbBand;

	// Setup Band to retrieve just the hwndChild.
	rbBand.cbSize = sizeof(REBARBANDINFO);  // Required
	rbBand.fMask  =	RBBIM_CHILD	;

	UINT u=0;
	while (u < uBandCount)
	{
		if (SendMessage(m_hRebarWnd, RB_GETBANDINFO, (WPARAM)u, (LPARAM)&rbBand))
		{
			if (rbBand.hwndChild == m_hToolbar)
			{
				iBandIndex = u;
			}
		}
		u++;
	}
}

//================================================
// RemoveBand
//================================================
BOOL CJMToolbar::RemoveBand()
{
	if (!::IsWindow(m_hRebarWnd))
		return FALSE;

	INT iBandIndex = -1;
	GetBandIndex(iBandIndex);
	if (iBandIndex < 0)
		return FALSE;

	if (!::SendMessage(m_hRebarWnd, RB_SHOWBAND, iBandIndex, FALSE))
		return FALSE;

	REBARBANDINFO rbBand;
	rbBand.cbSize = sizeof(REBARBANDINFO);  // Required
	rbBand.fMask  =	RBBIM_CHILD;
	rbBand.hwndChild = NULL;
	::SendMessage(m_hRebarWnd, RB_SETBANDINFO, (WPARAM)iBandIndex, (LPARAM)&rbBand); 
	return ::SendMessage(m_hRebarWnd, RB_DELETEBAND, (WPARAM)iBandIndex, 0);  
}

//================================================
// CreateJMToolbar
//================================================
HWND CJMToolbar::CreateJMToolbar (HWND hwndTopParent, HWND hwndRebar, UINT OwnerClass)
{
	if (m_hToolbar || !::IsWindow(hwndRebar))
		return m_hToolbar;

	m_hTopParent = hwndTopParent;
	m_uOwnerClass = OwnerClass;
	m_bInitialized = TRUE;

	REBARBANDINFO rbBand;
	memset(&rbBand, 0, sizeof(rbBand));

	// Ensure that the common control DLL is loaded. 
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_COOL_CLASSES|ICC_BAR_CLASSES;
	if (!InitCommonControlsEx(&icex))
	{
		MessageBox(_T("Common Controls dll not found.\n See technical support for help!"),
				    _T("Error"), MB_OK | MB_ICONSTOP); 

		return NULL;
	}
	
	// Create a toolbar. 
	m_hToolbar = CreateWindowEx(
//j		WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR | WS_EX_TOOLWINDOW | TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_HIDECLIPPEDBUTTONS | TBSTYLE_EX_MIXEDBUTTONS,
		0,
			TOOLBARCLASSNAME, g_szAppName, 
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | 
		TBSTYLE_TRANSPARENT | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_LIST | 
//j		TBSTYLE_EX_HIDECLIPPEDBUTTONS | TBSTYLE_EX_MIXEDBUTTONS | 
		CCS_NODIVIDER | CCS_ADJUSTABLE | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_TOP, 
		0, 0, 0, 0, hwndRebar, 
		(HMENU)ID_JMTOOLBAR, _AtlBaseModule.GetResourceInstance(), NULL); 

	HICON hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(),	(LPCTSTR)MAKEINTRESOURCE(IDI_LOGO), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR|LR_SHARED);
	::SendMessage(m_hToolbar, WM_SETICON, true/*bBigIcon*/, (LPARAM)hIcon);
	hIcon = (HICON)::LoadImage(_AtlBaseModule.GetResourceInstance(),	(LPCTSTR)MAKEINTRESOURCE(IDI_LOGO), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_SHARED);
	::SendMessage(m_hToolbar, WM_SETICON, false/*bBigIcon*/, (LPARAM)hIcon);

	if (!m_hToolbar)
		return NULL;

	m_hRebarWnd = hwndRebar;

	// Send the TB_BUTTONSTRUCTSIZE message, which is required for 
	// backward compatibility. 
	SendMessage(m_hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0); 

	// Get the height of the toolbar.
	DWORD dwBtnSize = (DWORD)SendMessage(m_hToolbar, TB_GETBUTTONSIZE, 0,0);
	
	ATLTRACE2( atlTraceGeneral, 4, "CreateJMToolbar \n dwBtnSize = %d\n", dwBtnSize );

	// Set values unique to the band with the toolbar.
	rbBand.cbSize = sizeof(REBARBANDINFO);  // Required
	rbBand.fMask  =	RBBIM_COLORS		| 
					RBBIM_CHILD			| 
					RBBIM_ID			| 
					RBBIM_CHILDSIZE		| 
					RBBIM_IDEALSIZE		|
					RBBIM_STYLE;
	rbBand.fStyle =	RBBS_CHILDEDGE		| 
					RBBS_FIXEDBMP		| 
					RBBS_BREAK			| 
					RBBS_USECHEVRON;

	rbBand.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbBand.clrBack = GetSysColor(COLOR_BTNFACE);

	rbBand.hwndChild  = m_hToolbar;
	rbBand.wID		  = ID_JMTOOLBAR;
	rbBand.cxMinChild = LOWORD(dwBtnSize);
	rbBand.cyMinChild = HIWORD(dwBtnSize);
	rbBand.cx         = 250;
	rbBand.cxIdeal	  = 250;

	ATLTRACE2( atlTraceGeneral, 4, "CreateToolbar: LOWORD(dwBtnSize) = %d\n HIWORD(dwBtnSize) = %d\n ", LOWORD(dwBtnSize), HIWORD(dwBtnSize));

	// Add the band that has the toolbar.
	if (! SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand))
	{
		ATLTRACE2( atlTraceGeneral, 4, "JMToolbar failed to be inserted!");
	}

	// Add buttons to toolbar
	if (!AddButtons(m_hToolbar))
	{
		ATLTRACE2( atlTraceGeneral, 4, "CreateJMToolbar failed: AddButtons()");
	}

	_ASSERTE(::IsWindow(m_hToolbar));
	if (!::IsWindow(m_hToolbar))
		return m_hToolbar;

	Attach(m_hToolbar);
	SetParent(hwndRebar);
	g_pfRebarWndProc = SetWindowProc(hwndRebar, RebarWindowProc); // subclass Rebar's WinProc
	g_pfToolbarWndProc = SetWindowProc(m_hToolbar, ToolbarWndProc); // subclass Toolbar's WinProc

	// Subclass the OE Browser or the Compose Window (ATH_Note) windowproc.
	if(m_uOwnerClass == OWNER_OEBROWSER)
	{
		g_pfOutlookExWndProc = SetWindowProc(m_hTopParent, OutlookExWndProc);
	}
	else
	if (m_uOwnerClass == OWNER_COMPOSE)
	{
		g_pfComposeWndProc = SetWindowProc(m_hTopParent, ComposeWndProc);

		// Also subclass the SysTabControl32 windowproc.
		HWND hSysTab32 = GetChildWindow(m_hTopParent, OE_SYSTAB32_CLASS);
		if (::IsWindow(hSysTab32))
			g_pfSysTab32WndProc = SetWindowProc(hSysTab32, SysTab32WndProc);
	}

	// Start timer to allow any other third party bands to be added. 
	// If any are added before the timer expires then the band 
	// containing the toolbar is moved to the last band.
	//::SetTimer(m_hRebarWnd, IDT_REBAR_TIMER, 15000, NULL);
		
	return m_hToolbar;
}

//================================================
// AddButtons
//================================================
BOOL CJMToolbar::AddButtons(HWND hwndToolbar)
{
	if (!hwndToolbar)
		return FALSE;

	#define BMP_CX 16
	#define BMP_CY 16

	// Create the image list for toolbar buttons.
	m_himlBtns = ImageList_Create(BMP_CX, BMP_CY, ILC_COLOR24 | ILC_MASK, NUM_OF_BTNS, 1);

	// Add button bitmaps to the image list.
	for (int i=0; i<NUM_OF_BTNS; i++)
	{
		HBITMAP hBitmap = (HBITMAP)::LoadImage(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDB_BTN1+i), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		HBITMAP hBitmapMask = ReplaceBitmapColor(hBitmap, CLR_DEFAULT, RGB(255,255,255), RGB(0,0,0));

		ImageList_Add(m_himlBtns, hBitmap, hBitmapMask);
		
		// Delete the loaded bitmap.
		DeleteObject(hBitmap);
		DeleteObject(hBitmapMask);
	}

	// Set the image list for the toolbar. 
	SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM)m_himlBtns);

	// Loop to fill the array of TBBUTTON structures. 
	TBBUTTON tbb[NUM_OF_BTNS];
	int nButtons = 0;
	for (int i=0; i<NUM_OF_BTNS; i++)
	{
		if (i == 7) //j Skip the Favorties button for now
			continue;

		// Add button strings
		CString strText;
		strText.LoadString(IDS_BTN1+i);
		strText += '\0';
		int iStrIndex = (int)SendMessage(hwndToolbar, TB_ADDSTRING, 0, (LPARAM)(LPTSTR)strText.GetBuffer());

		// Populate button structure
		tbb[nButtons].iBitmap   = i; 
		tbb[nButtons].idCommand = ID_BTN1_CMD + i;
		tbb[nButtons].fsState   = TBSTATE_ENABLED;
		tbb[nButtons].fsStyle   = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
		tbb[nButtons].dwData    = 0;
		tbb[nButtons].iString   = iStrIndex;
		nButtons++;
	}

	// Add buttons to toolbar
	return (BOOL)SendMessage(hwndToolbar, TB_ADDBUTTONS, (WPARAM)nButtons, (LPARAM)(LPTBBUTTON)&tbb);  
}

//=============================================================================
//	GetItemRect
//=============================================================================
BOOL CJMToolbar::GetItemRect(int nIndex, LPRECT lpRect) const
{
	ATLASSERT(::IsWindow(m_hWnd));
	return (BOOL)::SendMessage(m_hWnd, TB_GETITEMRECT, nIndex, (LPARAM)lpRect);
}

//=============================================================================
//	MoveToLastBand - moves band to last index of rebar
//=============================================================================
BOOL CJMToolbar::MoveToLastBand()
{	
	BOOL bSuccess = FALSE;
	INT iBandIndex = -1;
	GetBandIndex(iBandIndex);
	if (iBandIndex > -1)
	{
		UINT uBandCount = ::SendMessage(m_hRebarWnd, RB_GETBANDCOUNT, 0, 0);
		INT iLastIndex = uBandCount-1;
		if (uBandCount > 0 && iBandIndex < iLastIndex)
			 if (::SendMessage(m_hRebarWnd, RB_MOVEBAND, (WPARAM)iBandIndex, (LPARAM)iLastIndex))
				 bSuccess = TRUE;
	}
	return bSuccess;
}

//=============================================================================
//	ShowBand - hides or shows band containing toolbar
//=============================================================================
BOOL CJMToolbar::ShowBand(BOOL bShow)
{	
	BOOL bSuccess = FALSE;
	INT iBandIndex = -1;
	GetBandIndex(iBandIndex);
	if (iBandIndex > -1)
	{
		if (::SendMessage(m_hRebarWnd, RB_SHOWBAND, iBandIndex, bShow))
			 bSuccess = TRUE;
	}
	return bSuccess;
}
