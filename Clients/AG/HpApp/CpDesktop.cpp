#include "stdafx.h"
#include "CpDesktop.h"
#include "CpDialog.h"
#include "Splash.h"
#include "VersionInfo.h"
#include "RegKeys.h"
#include <initguid.h>
#include "HPapp_i.c"
#include ".\cpdesktop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only CCpDesktop object

CCpDesktopModule _AtlModule;
CString g_szAppName;
CCpDesktop theApp;

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCpDesktop, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CCpDesktop::CCpDesktop()
{
}

/////////////////////////////////////////////////////////////////////////////
CCpDesktop::~CCpDesktop()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCpDesktop::InitInstance()
{
	AfxOleInit();
	// we are a single instance app
    if (!FirstInstance())
        return(false);

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// CG: The following block was added by the Splash Screen component.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Register class factories via CoRegisterClassObject().
	if (FAILED(_AtlModule.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE)))
		return FALSE;
	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Don't show the main window
		return TRUE;
	}
	// App was launched with /Unregserver or /Unregister switch.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		_AtlModule.UpdateRegistryAppId(FALSE);
		_AtlModule.UnregisterServer(TRUE);
		return FALSE;
	}
	// App was launched with /Register or /Regserver switch.
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister)
	{
		_AtlModule.UpdateRegistryAppId(TRUE);
		_AtlModule.RegisterServer(TRUE);
		return FALSE;
	}

	g_szAppName = "HP Design Studio";
	{
		char szModule[MAX_PATH];
		char szVersion[MAX_PATH];

		::GetModuleFileName(AfxGetInstanceHandle(), szModule, sizeof(szModule));
		CString strFilePath(szModule);
		GetFileVersion(strFilePath, szVersion);

		// if fileversion is less or equal to the registry just return 
		// else update the registry
		if (!RegistryUpToDate(REGVAL_HPAPP_VERSION, szVersion))
		{
			// register this version of the App
			CRegKey regkey;

			regkey.Open(REGKEY_APPROOT, REGKEY_APP);
			regkey.SetStringValue(REGVAL_HPAPP_VERSION, szVersion);
		}
	}

	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);

	AfxEnableControlContainer();

	// Change the registry key under which our settings are stored
	SetRegistryKey(_T("American Greetings/CpApp"));

	CCpDialog dlg;
	m_pMainWnd = &dlg;
#ifndef _DEBUG
	CSplashWnd::ShowSplashScreen(GetSplashId(), NULL, 5000L);
#endif
	INT_PTR nResponse = dlg.DoModal();

	// Since the dialog has been closed, return false so that we exit the application,
	// rather than start the application's message pump.
	return false;
}

/////////////////////////////////////////////////////////////////////////////
bool CCpDesktop::RegistryUpToDate(CString strKeyName, CString strFileVersion)
{
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, REGKEY_APP) != ERROR_SUCCESS)
		return false;

	DWORD dwSize = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(strKeyName, szBuffer, &dwSize) == ERROR_SUCCESS)
	{
		CString strRegVersion = szBuffer;
		DWORD dwRegVersion = Version(strRegVersion);

		return Version(strFileVersion) <= dwRegVersion;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCpDesktop::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
bool CCpDesktop::FirstInstance()
{
	// Determine if another window with our class name or caption exists...
	CString strCaption;
	strCaption.LoadString(GetTitleId());
	CWnd* pWndApp = CWnd::FindWindow(NULL, strCaption);
	if (!pWndApp)
		return true;

	// An instance of the app exists
	// Does it have any popups?
	CWnd* pPopupWnd = pWndApp->GetLastActivePopup();

	// Bring the main window to the top
	pWndApp->BringWindowToTop();
	pWndApp->SetForegroundWindow();

	// If iconic, restore the main window
	if (pWndApp->IsIconic())
		pWndApp->ShowWindow(SW_RESTORE);

	// If there was an active popup, bring it along too
	if (pPopupWnd && (pPopupWnd != pWndApp))
	{
		pPopupWnd->BringWindowToTop();
		pPopupWnd->SetForegroundWindow();
	}

	return false;
}

BOOL CCpDesktop::ExitInstance(void)
{
	_AtlModule.RevokeClassObjects();
	return CWinApp::ExitInstance();
}
