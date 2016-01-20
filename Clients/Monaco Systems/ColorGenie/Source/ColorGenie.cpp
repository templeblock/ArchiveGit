// ColorGenie.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "ColorGenie.h"
#include "RegisterServer.h"
#include "MainFrame.h"
#include "ChildFrame.h"
#include "ProfilerDoc.h"
#include "ProfileView.h"
#include "Splash.h"
#include "monacoeve.h"
#include "Winmisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	//{{AFX_MSG_MAP(CApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN_GAMUT, OnFileOpenGamut)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApp construction

/////////////////////////////////////////////////////////////////////////////
CApp::CApp()
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CApp object

CApp theApp;

BOOL	CApp::LoadCmm()
{
	McoStatus status;

	CString dirname((LPCSTR)IDS_DEFAULTS_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);
	if(!exist)	return FALSE;

	char src[256] = "\\cmyk.icc";
	char dest[256] = "\\rgb.icc";
	exist = GetDefaultFile(src);
	if(!exist)	return FALSE;

	exist = GetDefaultFile(dest);
	if(!exist)	return FALSE;

	status = mcmmConvert.LinkGetXform(src, dest, 0);

	return (status == MCO_SUCCESS);
}

/////////////////////////////////////////////////////////////////////////////
// CApp initialization

/////////////////////////////////////////////////////////////////////////////
BOOL CApp::InitInstance()
{
	//check scanner
	if(IsColorGenie())
		mhasDongle = TRUE;
	else
		mhasDongle = FALSE;

	mhasDongle = TRUE;

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
	m_pszProfileName = _tcsdup(_T("ColorGenie\\1.0"));

	// All profile reading and writing should use the version as the "section"
	// e.g. WriteProfileString("Settings\\SubHeading", "entry", "name");

	//no MRU list will be maintained
	//LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)
	LoadStdProfileSettings(9);  // Load standard INI file options (including MRU)

	// Register any ActiveX controls
	BeginWaitCursor();
	char szFolder[255];
	GetSystemDirectory(szFolder, sizeof(szFolder));
	CString szPath;
	szPath = szFolder;
	szPath += "\\msflxgrd.ocx";
	UINT i = RegisterServer((LPCSTR)szPath, FALSE/*bSilent*/, FALSE/*bUnregister*/);
	EndWaitCursor();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

//	To switch back to an SDI app,
//	1.) uncomment the folowing code,
//	2.) base CMainFrame off of CFrameWnd (not CMDIFrameWnd)
//	3.) remove the CChildFrame class (no longer needed)
//	4.) get rid of the extra menu resource IDR_ICCTYPE
//	CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(
//		IDR_MAINFRAME,
//		RUNTIME_CLASS(CProfileDoc),
//		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
//		RUNTIME_CLASS(CProfileView));
	CMultiDocTemplate* pDocTemplate = new CMultiDocTemplate(
		IDR_ICCTYPE,
		RUNTIME_CLASS(CProfileDoc),
		RUNTIME_CLASS(CChildFrame),      // custom MDI frame window
		RUNTIME_CLASS(CProfileView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	m_pMainWnd->DragAcceptFiles( TRUE );

	//load icc files for showing the reference colors
	mhasCmm	= LoadCmm();
	if(!mhasCmm)
		AfxMessageBox(IDS_ERR_NO_CMM);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CApp commands

/////////////////////////////////////////////////////////////////////////////
BOOL CApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

/////////////////////////////////////////////////////////////////////////////
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// App command to run the dialog
void CApp::OnAppAbout()
{
	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(AfxGetMainWnd());

	// Use the splash screen instead of the about dialog
	//CAboutDlg dlgAbout;
	//dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
void CApp::OnUpdateFileNew(CCmdUI* pCmdUI) 
{
	if(mhasDongle)
		pCmdUI->Enable( TRUE );	
	else
		pCmdUI->Enable( FALSE );	
}

/////////////////////////////////////////////////////////////////////////////
void CApp::OnFileOpen() 
{
	CString dirname((LPCSTR)IDS_PROFILE_DIRNAME);
	BOOL exist = SetDefaultDir((LPCSTR)dirname);
	
	CWinApp::OnFileOpen();	
}

#include "ViewGamutFile.h"

/////////////////////////////////////////////////////////////////////////////
void CApp::OnFileOpenGamut() 
{
	ViewGamutFile();
}
