#include "stdafx.h"
#include "CpDesktop.h"
#include "CpDialog.h"
#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only CCpDesktop object
CCpDesktop theApp;

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCpDesktop, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CCpDesktop::CCpDesktop()
{
	CoInitialize(NULL);
}

/////////////////////////////////////////////////////////////////////////////
CCpDesktop::~CCpDesktop()
{
	CoUninitialize();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCpDesktop::InitInstance()
{
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
	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);

	AfxEnableControlContainer();

	// Change the registry key under which our settings are stored
	SetRegistryKey(_T("American Greetings/CpDesktop"));

	CCpDialog dlg;
	m_pMainWnd = &dlg;
	CSplashWnd::ShowSplashScreen(GetSplashId(), NULL, 5000L);
	INT_PTR nResponse = dlg.DoModal();

	// Since the dialog has been closed, return false so that we exit the application,
	// rather than start the application's message pump.
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
