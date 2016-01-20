#include "stdafx.h"
#include "MonacoView.h"
//#include "MainFrame.h"
#include "RegisterServer.h"
#include "MonacoViewDlg.h"
#include "Splash.h"
#include "monacoeve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMonacoViewApp

BEGIN_MESSAGE_MAP(CMonacoViewApp, CWinApp)
	//{{AFX_MSG_MAP(CMonacoViewApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CMonacoViewApp::CMonacoViewApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMonacoViewApp object
CMonacoViewApp theApp;

/////////////////////////////////////////////////////////////////////////////
BOOL CMonacoViewApp::InitInstance()
{
	// Check dongle
	if (1) //(IsMonacoVIEW())
		mhasDongle = TRUE;
	else
		mhasDongle = FALSE;

	if (!mhasDongle)
	{
		AfxMessageBox("Can't find the programed key, please make sure the key is programed correctly and attached to the parallel port.");

		return FALSE;
	}

	// CG: The following block was added by the Splash Screen component.
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);

		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
	}
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Monaco Systems"));
	// Note: MFC automatically puts the name of the app as another level,
	// unless you use the code below to reset the default m_pszProfileName
	// First free the string allocated by MFC at CWinApp startup.
	// The string is allocated before InitInstance is called.
	free((void*)m_pszProfileName);
	// The CWinApp destructor will free the memory.
	m_pszProfileName = _tcsdup(_T("MonacoVIEW\\1.0"));

	// All profile reading and writing should use the version as the "section"
	// e.g. WriteProfileString("Settings\\SubHeading", "entry", "name");

	// Register any ActiveX controls
	BeginWaitCursor();
	char szFolder[255];
	GetSystemDirectory(szFolder, sizeof(szFolder));
	CString szPath;
	szPath = szFolder;
	szPath += "\\label.ocx";
	RegisterServer((LPCSTR)szPath, FALSE/*bSilent*/, FALSE/*bUnregister*/);
	EndWaitCursor();

	MonacoViewDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMonacoViewApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}
