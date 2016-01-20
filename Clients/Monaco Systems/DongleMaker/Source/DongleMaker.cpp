// DongleMaker.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DongleMaker.h"
#include "DongleMakerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDongleMakerApp

BEGIN_MESSAGE_MAP(CDongleMakerApp, CWinApp)
	//{{AFX_MSG_MAP(CDongleMakerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDongleMakerApp construction

CDongleMakerApp::CDongleMakerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDongleMakerApp object

CDongleMakerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDongleMakerApp initialization

BOOL CDongleMakerApp::InitInstance()
{
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
	m_pszProfileName = _tcsdup(_T("DongleMaker\\1.0"));

	// All profile reading and writing should use the version as the "section"
	// e.g. WriteProfileString("Settings\\SubHeading", "entry", "name");

	CDongleMakerDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else
	if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
