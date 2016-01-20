#include "stdafx.h"
#include "Optionspropertysheet.h"
#include "JMBrkr.h"
#include "OptimizeDlg.h"


COptionsPropertySheet::COptionsPropertySheet (CJMBrkr* pBroker, _U_STRINGorID title, UINT uStartPage, HWND hWndParent ) :
					CPropertySheetImpl<COptionsPropertySheet> ( title, uStartPage, hWndParent ), 
					m_pgGeneral(this), m_pgPreference(this), m_pgMaint(this), m_pgAbout(this)
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_psh.dwFlags |= PSH_USEICONID;
	m_psh.pszIcon = MAKEINTRESOURCE(IDI_LOGO);
	m_psh.hInstance = _AtlBaseModule.GetResourceInstance();;
       

	m_pBroker = pBroker;
}
/////////////////////////////////////////////////////////////////////////////
COptionsPropertySheet::~COptionsPropertySheet(void)
{
}
/////////////////////////////////////////////////////////////////////////////
LRESULT COptionsPropertySheet::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	
    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT COptionsPropertySheet::OnPageInit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(m_psh.hwndParent);
	//PositionWindow();
	ShowWindow(SW_SHOW);
	BringWindowToTop();
	
    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COptionsPropertySheet::PositionWindow()
{
	HWND hParent = m_psh.hwndParent;

	POINT ptCursor;
	::GetCursorPos(&ptCursor);
	
	RECT rParent;
	::GetWindowRect(hParent, &rParent);
	ClientToScreen(&rParent);

	RECT rRect;
	GetWindowRect(&rRect);
	ClientToScreen(&rRect);

	POINT ptNew;
	if (ptCursor.x < (rParent.left + rParent.right)/2)
		ptNew.x = ptCursor.x;
	else
		ptNew.x = ptCursor.x - WIDTH(rRect);

	if (ptCursor.y < (rParent.top + rParent.bottom)/2)
		ptNew.y = ptCursor.y;
	else
		ptNew.y = ptCursor.y - HEIGHT(rRect);
	
	SetWindowPos(HWND_TOPMOST, ptNew.x, ptNew.y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}
/////////////////////////////////////////////////////////////////////////////
bool COptionsPropertySheet::LoadPages()
{
	AddPage(m_pgGeneral);
	AddPage(m_pgPreference);
	AddPage(m_pgMaint);
	AddPage(m_pgAbout);
	//BOOL bSuccess = SetActivePage(1);

	return true;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COptionsPropertySheet::HandleNewMail()
{
	CString strTargetPath = "msimn.exe";
	GetAppPath(strTargetPath);
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", strTargetPath, "/MailUrl:j", NULL, SW_SHOWNORMAL);
	if (dwResult <= 32)
		::MessageBox(NULL, "New Mail Option failed!", g_szAppName, MB_OK|MB_ICONSTOP);

	return true;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COptionsPropertySheet::HandleMyAccount()
{
	CString szMyAccount;
	m_Authenticate.ReadMyAccountPage(szMyAccount);
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szMyAccount, NULL, NULL, SW_SHOWNORMAL);
	if (dwResult <= 32)
		::MessageBox(NULL, "My Account Option failed!", g_szAppName, MB_OK|MB_ICONSTOP);

	return true;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COptionsPropertySheet::HandleUpgrade()
{
	CString szUrl;
	m_Authenticate.ReadHost(szUrl);
	CString szUpgrade;
	m_Authenticate.ReadUpgradePage(szUpgrade, SOURCE_TRAYICON);
	if (szUpgrade.Find("http") >= 0)
		szUrl = szUpgrade;
	else
		szUrl += szUpgrade;

	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
	if (dwResult <= 32)
		::MessageBox(NULL, "Upgrade Option failed!", g_szAppName, MB_OK|MB_ICONSTOP);

	return true;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COptionsPropertySheet::HandleHome()
{
	CString szUrl;
	m_Authenticate.ReadHost(szUrl);
	szUrl += DEFAULT_HOME;
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
	if (dwResult <= 32)
		::MessageBox(NULL, "Home Option failed!", g_szAppName, MB_OK|MB_ICONSTOP);
	return true;
}
//////////////////////////////////////////////////////////////////////////////
BOOL COptionsPropertySheet::HandleOptimize()
{
	COptimizeDlg dlgOptimize;
	dlgOptimize.DoModal();
	return true;
}
//////////////////////////////////////////////////////////////////////////////
BOOL COptionsPropertySheet::HandleCheckForUpdates()
{
	return m_pBroker->CheckForUpdates();
}
//////////////////////////////////////////////////////////////////////////////
BOOL COptionsPropertySheet::HandleAuth(DWORD dwMode)
{
	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	HRESULT hr = m_pBroker->Register(dwMode, &bSuccess);
	return SUCCEEDED(hr) && bSuccess;
}
//////////////////////////////////////////////////////////////////////////////
BOOL COptionsPropertySheet::HandleShow(BOOL bShow)
{
	m_pBroker->Visible((VARIANT_BOOL)bShow);
	return true;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COptionsPropertySheet::HandleHelp()
{
	CString szUrl;
	m_Authenticate.ReadHost(szUrl);
	if (GetKeyState(VK_CONTROL) < 0)
		m_pBroker->ProcessExecuteCommand(CMD_REGISTERADDIN);
	else if (GetKeyState(VK_SHIFT) < 0)
		m_pBroker->ProcessExecuteCommand(CMD_UNREGISTERADDIN);
	else
	{
		CString szHelp;
		m_Authenticate.ReadHelpPage(szHelp);
		if (szHelp.Find("http") >= 0)
			szUrl = szHelp;
		else
			szUrl += szHelp;
		DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
		if (dwResult <= 32)
			::MessageBox(NULL, "Help Option failed!", g_szAppName, MB_OK|MB_ICONSTOP);
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COptionsPropertySheet::UpdateClients(DWORD dwStatus)
{
	BOOL bExpressUpdated	= false;
	BOOL bOutlookUpdated	= false;
	BOOL bIEUpdated			= false;
	BOOL bAOLUpdated		= false;
	BOOL bIEDisabled		= false;
	BOOL bAOLDisabled		= false;
	BOOL bExpressEnableRequest	= dwStatus & CLIENT_TYPE_EXPRESS;
	BOOL bOutlookEnableRequest	= dwStatus & CLIENT_TYPE_OUTLOOK;
	BOOL bIEEnableRequest		= dwStatus & CLIENT_TYPE_IE;
	BOOL bAOLEnableRequest		= dwStatus & CLIENT_TYPE_AOL;

	BOOL bExpressEnabled = m_Authenticate.IsClientRegistered(CLIENT_TYPE_EXPRESS);
	BOOL bOutlookEnabled = m_Authenticate.IsClientRegistered(CLIENT_TYPE_OUTLOOK);
	BOOL bIEEnabled      = m_Authenticate.IsClientRegistered(CLIENT_TYPE_IE);
	BOOL bAOLEnabled     = false;/*m_Authenticate.IsClientRegistered(CLIENT_TYPE_AOL);*/// Temp, Until AOL supported - JHC
	
	HWND hwnd=0;
	DWORD dwType=0;
	CLIENTLIST::iterator itr;
	itr = m_pBroker->m_Clients.begin();

	// Handle disabling clients that are enabled and currently running. 
	// Note: m_Clients is a list of active mail clients.
	while (!m_pBroker->m_Clients.empty() && itr != m_pBroker->m_Clients.end())
	{
		hwnd = (*itr).hwnd;
		dwType = (*itr).Type;
		
		// If a client is currently enabled but it should be disabled then disable it.
		if (!bExpressEnableRequest && bExpressEnabled && dwType==CLIENT_TYPE_EXPRESS) 
		{
			m_pBroker->DisableClient(hwnd, dwType);
			bExpressUpdated = true;
		} 
		if (!bOutlookEnableRequest && bOutlookEnabled && dwType==CLIENT_TYPE_OUTLOOK)
		{
			m_pBroker->DisableClient(hwnd, dwType);
			bOutlookUpdated = true;
		}
		if (!bIEEnableRequest && bIEEnabled && dwType==CLIENT_TYPE_IE)
		{
			m_pBroker->DisableClient(hwnd, dwType);
			bIEUpdated = true;
			bIEDisabled = true;
		}
		if (!bAOLEnableRequest && bAOLEnabled && dwType==CLIENT_TYPE_AOL) 
		{
			m_pBroker->DisableClient(hwnd, dwType);
			bAOLUpdated = true;
			bAOLDisabled = true;
		} 
	
		itr++;
	}

	// Handle disabling mail clients that are enabled but are NOT currently running (i.e. not in m_Clients list)
	if (!bExpressEnableRequest && bExpressEnabled && !bExpressUpdated)
		m_pBroker->DisableClient(NULL, CLIENT_TYPE_EXPRESS);
	
	if (!bOutlookEnableRequest && bOutlookEnabled && !bOutlookUpdated)
		m_pBroker->DisableClient(NULL, CLIENT_TYPE_OUTLOOK);

	if (!bIEEnableRequest && bIEEnabled && !bIEUpdated)
	{
		m_pBroker->DisableClient(NULL, CLIENT_TYPE_IE);
		bIEDisabled = true;
	}

	if (!bAOLEnableRequest && bAOLEnabled && !bAOLUpdated)
	{
		m_pBroker->DisableClient(NULL, CLIENT_TYPE_AOL);
		bAOLDisabled = true;
	}

	if (bIEDisabled)
	{
		CString szMsg;
		szMsg.LoadString(IDS_IEDISABLE_PROMPT);
		::MessageBox(m_hWnd, szMsg, g_szAppName, MB_OK | MB_ICONINFORMATION);
	}

	if (bAOLDisabled)
	{
		CString szMsg;
		szMsg.LoadString(IDS_AOLDISABLE_PROMPT);
		::MessageBox(m_hWnd, szMsg, g_szAppName, MB_OK | MB_ICONINFORMATION);
	}

	// Handle enabling of mail clients (registers mail clients).
	CString szNewClients;

	if (bOutlookEnableRequest && !bOutlookEnabled)
	{
		EnableClient(CLIENT_TYPE_OUTLOOK);
		szNewClients = _T("\tOutlook\n");
	}
	
	if (bExpressEnableRequest && !bExpressEnabled)
	{
		EnableClient(CLIENT_TYPE_EXPRESS);
		szNewClients += _T("\tOutlook Express\n");
	}

	if (bIEEnableRequest && !bIEEnabled)
	{
		EnableClient(CLIENT_TYPE_IE);
		szNewClients += _T("\tInternet Explorer\n");
	}

	if (bAOLEnableRequest && !bAOLEnabled)
	{
		EnableClient(CLIENT_TYPE_AOL);
		szNewClients += _T("\tAOL\n");
	}

	if (!szNewClients.IsEmpty())
	{
		CString szMsg;
		szMsg.LoadString(IDS_NEWCLIENTS_PROMPT);
		szMsg += szNewClients;
		::MessageBox(m_hWnd, szMsg, g_szAppName, MB_OK | MB_ICONINFORMATION);
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////
void COptionsPropertySheet::ShowContextHelp(HWND hwndCaller, int iID, int iXPos, int iYPos)
{
	HH_POPUP    hhp;
	memset(&hhp, 0, sizeof(HH_POPUP));
    hhp.cbStruct = sizeof(HH_POPUP);
    hhp.hinst = _AtlBaseModule.GetResourceInstance();

	CString szMsg;
	szMsg.LoadString(iID);
    hhp.idString = 0;
	hhp.pszText = szMsg;
    hhp.pt.x = iXPos;
    hhp.pt.y = iYPos;


    // Use default color & margins
    hhp.clrForeground = -1;
    hhp.clrBackground = -1;//RGB(255, 255, 208);	// Yellower background color
    hhp.rcMargins.left = -1;
    hhp.rcMargins.top = -1;
    hhp.rcMargins.bottom = -1;
    hhp.rcMargins.right = -1;
    hhp.pszFont = _T("MS Shell Dlg, 8, ,");
	::HtmlHelp(hwndCaller, NULL, HH_DISPLAY_TEXT_POPUP,(DWORD)&hhp);
}