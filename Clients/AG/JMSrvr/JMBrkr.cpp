#include "stdafx.h"
#include "JMBrkr.h"
#include "Utility.h"
#include "AboutDlg.h"
#include "version.h"
#include "regkeys.h"
#include ".\jmbrkr.h"
#include "OptimizeDlg.h"

#define UPDATE_RESOURCES	0x00000004	// Indicates that the file is a resource DLL to be loaded after download

const int ACCESS_ID = 0xBABE;
typedef HRESULT (STDAPICALLTYPE *LPFNDLLREGISTERSERVER)(void);
typedef HRESULT (STDAPICALLTYPE *LPFNDLLUNREGISTERSERVER)(void);
/////////////////////////////////////////////////////////////////////////////////////////////
CJMBrkr::CJMBrkr() : m_OptPropsheet (this, _T("Creata Mail Settings"), 0)
{
	m_hJMHookInst = NULL;
	m_bUseCBTHook = false;

	RefreshRegistration();
}

/////////////////////////////////////////////////////////////////////////////////////////////
CJMBrkr::~CJMBrkr()
{
	if (m_RegisterDialog.m_hWnd)
		m_RegisterDialog.DestroyWindow();

	m_Clients.clear();
}

//=============================================================================
//	UtilityCallback
//=============================================================================
void CALLBACK CJMBrkr::UtilityCallback(UINT uMsg, LPARAM lParam1, LPARAM lParam2)
{
	CJMBrkr* pJMBrkr = (CJMBrkr*)lParam1;
	if (!pJMBrkr)
		return;

	if (UWM_SHUTDOWNALL == uMsg)
		pJMBrkr->Shutdown();
	else if (UWM_OPTIONS == uMsg)
		pJMBrkr->ManageOptions();
}

/////////////////////////////////////////////////////////////////////////////////////////////
void CJMBrkr::Shutdown()
{
	if (::IsWindow(m_RegisterDialog.m_hWnd))
		m_RegisterDialog.DestroyWindow();

	if (::IsWindow(m_UtilWnd.m_hWnd))
		m_UtilWnd.DestroyWindow();

	m_Clients.clear();

	if (m_hJMHookInst)
	{
		// Call Shutdown from JMHook.dll
		typedef  BOOL(* PFNSHUTDOWN)();
		PFNSHUTDOWN pfShutdown = (PFNSHUTDOWN)::GetProcAddress(m_hJMHookInst, _T("ForceShutdown"));
		if (pfShutdown) 
			(pfShutdown)(); 
	}

	RemoveHook();

	exit(EXIT_SUCCESS);
}

//================================================
// ProcessExecuteCommand
//================================================
bool CJMBrkr::ProcessExecuteCommand(int iCmd)
{
	bool bSuccess = false;
	switch (iCmd)
	{
		// Register AgoutlookAdin
		case CMD_REGISTERADDIN:
		{
			CString szPath;
			if (!m_Authenticate.ReadInstallPath(szPath))
				return S_OK;

#ifdef _DEBUG
			szPath = _T("C:\\My Projects\\AgApplications\\AgOutlookAddIn\\Debug\\");
#endif _DEBUG

			szPath += _T("AgOutlookAddIn.dll");

			HINSTANCE hInst = ::LoadLibrary(szPath);
			if (!hInst)
			{
				GetError(_T("CJMBrkr::ExecuteCommand[CMD_REGISTERADDIN]"));
			}
			else
			{
				LPFNDLLREGISTERSERVER lpfnRegServer;
				lpfnRegServer = (LPFNDLLREGISTERSERVER) ::GetProcAddress(hInst, _T("DllRegisterServer"));
				if (!lpfnRegServer)
				{
					GetError(_T("CJMBrkr::ExecuteCommand[CMD_REGISTERADDIN], lpfnRegServer"));
				}
				else
				{
					HRESULT hr = (*lpfnRegServer)();
					bSuccess = SUCCEEDED(hr);
				}
			}

			::FreeLibrary(hInst);
			break;
		}

		// Unregister AgoutlookAdin
		case CMD_UNREGISTERADDIN:
		{
			CString szPath;
			if (!m_Authenticate.ReadInstallPath(szPath))
				return S_OK;

#ifdef _DEBUG
			szPath = _T("C:\\My Projects\\AgApplications\\AgOutlookAddIn\\Debug\\");
#endif _DEBUG

			szPath += _T("AgOutlookAddIn.dll");

			HINSTANCE hInst = ::LoadLibrary(szPath);
			if (!hInst)
			{
				GetError(_T("CJMBrkr::ExecuteCommand[CMD_UNREGISTERADDIN]"));
			}
			else
			{
				LPFNDLLUNREGISTERSERVER lpfnUnregServer;
				lpfnUnregServer = (LPFNDLLUNREGISTERSERVER) ::GetProcAddress (hInst, _T("DllUnregisterServer"));
				if (!lpfnUnregServer)
				{
					GetError(_T("CJMBrkr::ExecuteCommand[CMD_UNREGISTERADDIN]"));
				}
				else
				{
					HRESULT hr = (*lpfnUnregServer)();
					bSuccess = SUCCEEDED(hr);
				}
			}

			::FreeLibrary(hInst);
			break;
		}
		// Launch Optimzation Wizard
		case CMD_OPTIMIZATIONWIZARD:
		{
			LaunchOptimizationWizard();
			break;
		}
		// Launch Creata Mail Options/Settings dialog
		case CMD_CMSETTINGS:
		{
			ManageOptions();
			break;
		}
		case CMD_CMLAUNCH:
		{
			PostMessagebyClientType(CLIENT_TYPE_AOL, UWM_LAUNCH, 0, 0);
		}
		default: 
			break;
	}

	return bSuccess;
}

//================================================
// RemoveClient
//================================================
bool CJMBrkr::RemoveClient(HWND hwnd)
{
	bool bFound = false;
	CLIENTLIST::iterator itr;
	itr = m_Clients.begin();
	while (!bFound && !m_Clients.empty() && itr != m_Clients.end())
	{
		if (hwnd == (*itr).hwnd)
		{
			m_Clients.erase(itr);
			bFound = true;

			if (CLIENT_TYPE_AOL == (*itr).Type)
			{
				DWORD dwStatus = 0;
				m_Authenticate.ReadDWORDStatus(REGVAL_AOL_STATUS, dwStatus);
				dwStatus &= ~CLIENT_TYPE_AOL;
				m_Authenticate.WriteDWORDStatus(REGVAL_AOL_STATUS, dwStatus);
			}
		}
		itr++;
	}
	return bFound;
}
//================================================
// RefreshIERegistration
//================================================
bool CJMBrkr::RefreshRegistration()
{
	if (m_Authenticate.IsIEEnabled())
		EnableClient(CLIENT_TYPE_IE);

	if (m_Authenticate.IsOutlookEnabled())
		EnableClient(CLIENT_TYPE_OUTLOOK);

	if (m_Authenticate.IsExpressEnabled())
		EnableClient(CLIENT_TYPE_EXPRESS);

	if (m_Authenticate.IsAOLEnabled())
		EnableClient(CLIENT_TYPE_AOL);

	return true;
}
//================================================
// BroadcastMessage
//================================================
BOOL CJMBrkr::BroadcastMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_Clients.empty())
		return false;

	bool bMessageSent = false;
	CLIENTLIST::iterator itr;
	itr = m_Clients.begin();
	HWND hwnd;
	while (itr != m_Clients.end())
	{
		hwnd =(*itr).hwnd;
		if (::IsWindow(hwnd))
		{
			::PostMessage(hwnd, uMsg, wParam, lParam);
			bMessageSent = true;
		}
		itr++;
	}
	return bMessageSent;
}
//================================================
// SendMessagebyClientType
//================================================
BOOL CJMBrkr::PostMessagebyClientType(UINT uType, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_Clients.empty())
		return false;

	bool bMessageSent = false;
	CLIENTLIST::iterator itr;
	itr = m_Clients.begin();
	HWND hwnd;
	UINT uClient;
	while (itr != m_Clients.end() && !bMessageSent)
	{
		uClient = (*itr).Type;
		if (uType == uClient)
		{
			hwnd =(*itr).hwnd;
			if (::IsWindow(hwnd))
			{
				::PostMessage(hwnd, uMsg, wParam, lParam);
				bMessageSent = true;
			}
		}

		itr++;
	}
	return bMessageSent;
}
//================================================
// RemoveHook
//================================================
bool CJMBrkr::RemoveHook()
{
	if (m_hJMHookInst)
	{	
		// Remove Hook
		typedef BOOL(* PFNREMOVEHOOK)();
		PFNREMOVEHOOK pfRemoveHook = (PFNREMOVEHOOK)::GetProcAddress(m_hJMHookInst, _T("RemoveSHELLHook"));
		if (pfRemoveHook)
			(*pfRemoveHook)(); 

		// Free the JMHook DLL module.
		if(::FreeLibrary(m_hJMHookInst))
			m_hJMHookInst = NULL;
	}
	return true;
}
//================================================
// InstallHook
//================================================
bool CJMBrkr::InstallHook()
{
	// Leave if hook already installed
	if (NULL != m_hJMHookInst)
		return false;

	// Explicitly load JMHook.dll
	m_hJMHookInst = ::LoadLibrary( _T("JMHOOK.DLL") );
	if (m_hJMHookInst)
	{	
		// Setup hook to detect OE launch
		typedef  BOOL(* PFNINSTALLHOOK)(bool bUseCBT);
		PFNINSTALLHOOK pfInstallHook = (PFNINSTALLHOOK)::GetProcAddress(m_hJMHookInst, _T("InstallSHELLHook"));
		if (pfInstallHook) 
			(*pfInstallHook)(m_bUseCBTHook);
	}
	return true;
}
//================================================
// HandleUpgrade
//================================================
void CJMBrkr::ShowUpgradePage(int iSource)
{		
	CFuncLog log(g_pLog, "CJMBrkr::ShowUpgradePage()");
	CString szUrl;
	CString szUpgrade;
	m_Authenticate.ReadHost(szUrl);
	m_Authenticate.ReadUpgradePage(szUpgrade, iSource);
	if (szUpgrade.Find("http") >= 0)
		szUrl = szUpgrade;
	else
		szUrl += szUpgrade;

	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}
//================================================
// LaunchOptimizationWizard
//================================================
BOOL CJMBrkr::LaunchOptimizationWizard()
{
	COptimizeDlg dlgOptimize;
	dlgOptimize.DoModal();

	return true;
}
//================================================
// DisableClient
//================================================
BOOL CJMBrkr::DisableClient(HWND hwnd, DWORD dwType)
{
	// If mail client is running tell it to cleanup.
	if (::IsWindow(hwnd))
		::PostMessage(hwnd, UWM_REMOVEADDIN,0,0);

	// Unregister mail client.
	::DisableClient(dwType);

	return true;
}
//================================================
// ShowFeedbackPage
//================================================
BOOL CJMBrkr::ShowFeedbackPage()
{		
	CString szUrl;
	m_Authenticate.ReadHost(szUrl);
	szUrl += DEFAULT_FEEDBACK;
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
	return true;
}
//================================================
// CheckForUpdates
//================================================
BOOL CJMBrkr::CheckForUpdates()
{
	HRESULT hr=S_OK;

	if (!m_spUpdateObj)
	{
		hr = m_spUpdateObj.CoCreateInstance(CComBSTR("AgOutlookAddIn.UpdateObj"), NULL);
		if (FAILED(hr))
			return false;
		hr = DispEventAdvise(m_spUpdateObj, &DIID__IUpdateObjEvents);
	}

	if (FAILED(hr))
		return false;

	hr = m_spUpdateObj->DoAutoUpdate((OLE_HANDLE)m_OptPropsheet.m_hWnd, false);
	if (FAILED(hr))
		return false;

	return true;
}
//================================================
// OnAutoUpdateComplete
//================================================
void STDMETHODCALLTYPE CJMBrkr::OnAutoUpdateComplete(DWORD dwResult)
{
	if (dwResult & UPDATE_RESOURCES)
		BroadcastMessage(UWM_UPDATERESOURCE, 0, 0);

	if (m_spUpdateObj)
	{
		HRESULT hr = DispEventUnadvise(m_spUpdateObj);
		m_spUpdateObj.Release();
		m_spUpdateObj = NULL;
	}

	if (dwResult && ::IsWindow(m_OptPropsheet.m_hWnd))
		m_OptPropsheet.ShowWindow(SW_MINIMIZE);

	return;
}
//================================================
// ManageOptions
//================================================
BOOL CJMBrkr::ManageOptions()
{
	if (::IsWindow(m_OptPropsheet.m_hWnd))
	{
		m_OptPropsheet.ShowWindow(::IsIconic(m_OptPropsheet.m_hWnd) ? SW_RESTORE : SW_SHOWNORMAL);
		m_OptPropsheet.SetActiveWindow();
		m_OptPropsheet.SetFocus();
		return true;
	}

	HWND hParent = GetDesktopWindow();
	m_OptPropsheet.LoadPages();
    m_OptPropsheet.Create(hParent);
	if (::IsWindow(m_OptPropsheet.m_hWnd))
		::SetWindowPos(m_OptPropsheet.m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	return true;
}


//================================================
// InterfaceSupportsErrorInfo
//================================================
STDMETHODIMP CJMBrkr::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IJMBrkr
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

//================================================
// OnTimer
//================================================
// This handler is called every time a timer windows message is received 
//
LRESULT CJMBrkr::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bAnimate)
		return S_OK;

	WORD Icon[] = { IDI_LOGO16 };
	
	// Update the icon
	SetShellIcon(Icon[m_CurrentIcon]);

	// Move onto the next icon
	int iCount = sizeof(Icon) / sizeof(WORD);
	if (++m_CurrentIcon >= iCount)
		m_CurrentIcon = 0;

	return S_OK;
}

//================================================
// OnClose
//================================================
LRESULT CJMBrkr::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Shutdown();
	return S_OK;
}

//================================================
// OnUserCommand
//================================================
// This handler is called every time there is a mouse movement.
LRESULT  CJMBrkr::OnUserCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	if (lParam == WM_LBUTTONUP || lParam == WM_RBUTTONUP)
	{
		
		
//		// Show the popup menu.  The return code is the menu item the user
//		// selected, or zero if they didn't make a choice (i.e. by clicking 
//		// outside of the popup menu).
		WORD cmd = ShowPopupMenu(IDR_POPUP_MENU2);
		DWORD dwResult = 100;
		CString szUrl;
		m_Authenticate.ReadHost(szUrl);
		switch (cmd)
		{
			case ID_TRAY_MYACCOUNT:
			{
				//HandleMyAccount();
				break;
			}

			case ID_TRAY_UPGRADENOW:
			{
				ShowUpgradePage(SOURCE_TRAYICON);
				break;
			}

			case ID_TRAY_HOME:
			{
				//HandleHome();
				break;
			}
		
			case ID_TRAY_NEWEMAIL:
			{
				//HandleNewMail();
				break;
			}
			case ID_TRAY_FEEDBACK:
			{
				ShowFeedbackPage();
				break;
			}

			case ID_TRAY_HELP:
			{
				if (GetKeyState(VK_CONTROL) < 0)
					ProcessExecuteCommand(CMD_REGISTERADDIN);
				else if (GetKeyState(VK_SHIFT) < 0)
					ProcessExecuteCommand(CMD_UNREGISTERADDIN);
				else
					m_OptPropsheet.HandleHelp();
				
				break;
			}
			case ID_TRAY_SETTINGS:
			{
				if (GetKeyState(VK_CONTROL) < 0)
					Shutdown();
				else
					ManageOptions();
			
				break;
			}
			case ID_TRAY_LAUNCH:
			{
				ProcessExecuteCommand(CMD_CMLAUNCH);
				break;
			}
			case ID_TRAY_ABOUT:
			{
				/*CAboutDlg dlgAbout(VER_PRODUCT_VERSION_STR);
				dlgAbout.DoModal();*/
				break;
			}
//
//#ifdef NOTUSED
//			case ID_HELP_TOPICS:
//			{
//				break;
//			}
//
//			case ID_HELP_FAQ:
//			{
//				break;
//			}
//#endif NOTUSED
//
//			case ID_HELP_FEEDBACK:
//			{
//				szUrl += DEFAULT_FEEDBACK;
//				dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
//				break;
//			}
			
			default:
				break;
		}

		if (dwResult <= 32)
			::MessageBox(NULL, "System tray failure", "System Tray", MB_OK);
	}

	return S_OK;
}

//================================================
// Visible
//================================================
STDMETHODIMP CJMBrkr::Visible(VARIANT_BOOL bVisible)
{
	BOOL bShow = bVisible;
	SetShellVisible(bShow);
	return S_OK;
}

//================================================
// Animate
//================================================
STDMETHODIMP CJMBrkr::Animate(VARIANT_BOOL bAnimate, UINT nDuration)
{
	m_bAnimate = (BOOL)bAnimate;

    // Update the timer in the shell
    SetShellTimer(m_bAnimate, nDuration);

	return S_OK;
}

//================================================
// SetText
//================================================
STDMETHODIMP CJMBrkr::SetText(BSTR szText)
{
    // Immediately update the tip text
    SetShellTipText(CString(szText));
	return S_OK;
}

//=============================================================================
//	OnAuthenticate
//=============================================================================
STDMETHODIMP CJMBrkr::OnAuthenticate(VARIANT varCustomerNum, 
									  VARIANT varUserId, 
									  VARIANT varHost, 
									  VARIANT varUpgradeUrl, 
									  VARIANT varUsageUrl,
									  VARIANT varHelp,
									  VARIANT varBrowse,
									  VARIANT varOptions,
									  VARIANT varUserStatus, 
									  VARIANT varShow, 
									  VARIANT verResultStatus)
{
	bool bSuccess = m_RegisterDialog.OnAuthenticate(varCustomerNum.intVal,
												CString(varUserId.bstrVal), 
												CString(varHost.bstrVal),
												CString(varUpgradeUrl.bstrVal), 
												CString(varUsageUrl.bstrVal),
												CString(varHelp.bstrVal),
												CString(varBrowse.bstrVal),
												CString(varOptions.bstrVal),
												varUserStatus.intVal,
												varShow.boolVal,
												verResultStatus.intVal);


	return S_OK;
}

//=============================================================================
//	Show
//=============================================================================
STDMETHODIMP CJMBrkr::Show(VARIANT varShow, VARIANT varWidth, VARIANT varHeight) 
{
	m_RegisterDialog.Show(varShow.boolVal, varWidth.intVal, varHeight.intVal);
	return S_OK;
}

//================================================
// Register
//================================================
STDMETHODIMP CJMBrkr::Register(LONG Mode, VARIANT_BOOL* bSuccess)
{
	*bSuccess = m_RegisterDialog.Register(Mode);
	return S_OK;
}
//================================================
// get_Version
//================================================
STDMETHODIMP CJMBrkr::get_Version(VARIANT* pVal)
{
	CString szVersion;
	m_Authenticate.ReadVersion(szVersion);
	*pVal = CComVariant(szVersion);

	return S_OK;
}
//================================================
// get_SignInUrl
//================================================
STDMETHODIMP CJMBrkr::get_SignInUrl(VARIANT varId, VARIANT* pVal)
{
	int iId = varId.intVal;
	if (iId != ACCESS_ID)
		return S_OK;

	CString szUrl = m_RegisterDialog.GetURL();
	*pVal = CComVariant(szUrl);

	return S_OK;
}

//================================================
// put_SignInUrl
//================================================
STDMETHODIMP CJMBrkr::put_SignInUrl(VARIANT varId, VARIANT newVal)
{
	int iId = varId.intVal;
	if (iId != ACCESS_ID)
		return S_OK;

	CString szUrl = newVal.bstrVal;
	m_RegisterDialog.SetURL(szUrl, true);

	return S_OK;
}

//================================================
// AddInRegister
//================================================
STDMETHODIMP CJMBrkr::ExecuteCommand(VARIANT varId, VARIANT_BOOL* bSuccess)
{
	*bSuccess = ProcessExecuteCommand(varId.intVal);
	return S_OK;
}

//================================================
// RegisterClient
//================================================
STDMETHODIMP CJMBrkr::RegisterClient(OLE_HANDLE Handle, VARIANT varType, OLE_HANDLE* phUtilWnd)
{
	HWND hwnd = (HWND)Handle;
	if (!::IsWindow(hwnd))
		return S_OK;

	*phUtilWnd = (OLE_HANDLE)m_UtilWnd.m_hWnd;

	ClientInfoType ci;
	ci.hwnd = hwnd;
	ci.Type = varType.intVal;
	m_Clients.push_back(ci);

	if (ci.Type == CLIENT_TYPE_AOL)
	{
		DWORD dwStatus = 0;
		m_Authenticate.ReadDWORDStatus(REGVAL_AOL_STATUS, dwStatus);
		dwStatus |= CLIENT_TYPE_AOL;
		m_Authenticate.WriteDWORDStatus(REGVAL_AOL_STATUS, dwStatus);
	}
	return S_OK;
}

//================================================
// UnregisterClient
//================================================
STDMETHODIMP CJMBrkr::UnregisterClient(OLE_HANDLE Handle)
{
	HWND hwnd = (HWND)Handle;
	RemoveClient(hwnd);
	return S_OK;
}

//================================================
// KillAll
//================================================
STDMETHODIMP CJMBrkr::KillAll(VARIANT varId)
{
	if (varId.intVal != ACCESS_ID)
		return S_OK;

	Shutdown();
	return S_OK;
}
