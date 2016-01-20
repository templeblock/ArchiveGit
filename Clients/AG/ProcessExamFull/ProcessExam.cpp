#include "stdafx.h"
#include "ProcessExam.h"
#include "ProcessExamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CProcessExamApp theApp;

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CProcessExamApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CProcessExamApp::CProcessExamApp()
{
	m_iExitCode = 0;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProcessExamApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("ProcessExam"));

	CProcessExamDlg dlg;
	dlg.ShutdownAll(); // Shutdown creatamail

	int nMatches = dlg.ProcessWalk(TA_NONE, true/*bSilent*/);
	if (!nMatches)
	{
		m_iExitCode = 0;
		return false;
	}

	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK || nResponse == IDCANCEL)
	{
		int nMatches = dlg.ProcessWalk(TA_NONE, true/*bSilent*/);
		if (nMatches)
			m_iExitCode = -1;
		return false;
	}

	// Since the dialog has been closed, return false so that we exit the
	//  application, rather than start the application's message pump.
	return false;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CProcessExamApp::ExitInstance()
{
	return m_iExitCode;
}
