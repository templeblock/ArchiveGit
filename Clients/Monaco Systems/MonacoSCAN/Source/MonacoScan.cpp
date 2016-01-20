// MonacoScan.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "MonacoScan.h"
#include "MainDlg.h"
#include "RegisterServer.h"
#include "Splash.h"
#include "monacoeve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	//{{AFX_MSG_MAP(CApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApp construction
CApp::CApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CApp object
CApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CApp initialization
BOOL CApp::InitInstance()
{
	//check scanner	key
	if(IsMonacoSCAN())
		mhasDongle = TRUE;
	else
		mhasDongle = FALSE;

	mhasDongle = TRUE;
	if(!mhasDongle)
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

	// Change the main registry key under which our settings are stored.
	SetRegistryKey( _T("Monaco Systems") );
	// Note: MFC automatically puts the name of the app as another level,
	// unless you use the code below to reset the default m_pszProfileName
	// First free the string allocated by MFC at CWinApp startup.
	// The string is allocated before InitInstance is called.
	free((void*)m_pszProfileName);
	// The CWinApp destructor will free the memory.
	m_pszProfileName = _tcsdup(_T("MonacoSCAN\\1.0"));

	// All profile reading and writing should use the version as the "section"
	// e.g. WriteProfileString("Settings\\SubHeading", "entry", "name");

	// Register any ActiveX controls
	BeginWaitCursor();
	char szFolder[255];
	GetSystemDirectory(szFolder, sizeof(szFolder));
	CString szPath;
	szPath = szFolder;
	szPath += "\\imw32o30.ocx";
	RegisterServer((LPCSTR)szPath, FALSE/*bSilent*/, FALSE/*bUnregister*/);
	szPath = szFolder;
	szPath += "\\label.ocx";
	RegisterServer((LPCSTR)szPath, FALSE/*bSilent*/, FALSE/*bUnregister*/);
	EndWaitCursor();

	CMainDlg dlg;
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
// CApp commands

BOOL CApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}
