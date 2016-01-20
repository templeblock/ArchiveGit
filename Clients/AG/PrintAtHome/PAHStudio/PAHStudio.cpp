// PAHStudio.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PAHStudio.h"
#include "PAHStudioDlg.h"
#include <initguid.h>
#include "PAHStudio_i.c"
#include "CFileTypeAccess.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPAHStudioApp


class CPAHStudioModule :
	public CAtlMfcModule
{
public:
	DECLARE_LIBID(LIBID_PAHStudio);
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_PAHStudio, "{8FA90952-A463-4FF8-B749-468E0330C4C1}");};

CPAHStudioModule _AtlModule;

BEGIN_MESSAGE_MAP(CPAHStudioApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPAHStudioApp construction

CPAHStudioApp::CPAHStudioApp()
{
	EnableHtmlHelp();

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPAHStudioApp object

CPAHStudioApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x8B85EE30, 0x7D0F, 0x4337, { 0x8C, 0x24, 0x71, 0xDF, 0xC1, 0xE9, 0xC8, 0x50 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

/////////////////////////////////////////////////////////////////////////////
// CPAHStudioApp initialization
BOOL CPAHStudioApp::InitInstance()
{
	// We use a GUID here, so that we are sure the string is 100% unique
	if ( CSingleInstance::Create ( _T("2F699E18-48A9-4fd3-AC00-29D899A570BD") ) == FALSE )
		return FALSE ;
	
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	// Parse command line for automation or reg/unreg switches.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	#if !defined(_WIN32_WCE) || defined(_CE_DCOM)
	// Register class factories via CoRegisterClassObject().
	if (FAILED(_AtlModule.RegisterClassObjects(CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE)))
		return FALSE;
	#endif // !defined(_WIN32_WCE) || defined(_CE_DCOM)

	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Register class factories via CoRegisterClassObject().
		COleTemplateServer::RegisterAll();
	}
	// App was launched with /Unregserver or /Unregister switch.  Remove
	// entries from the registry.
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		_AtlModule.UpdateRegistryAppId(FALSE);
		_AtlModule.UnregisterServer(TRUE);
		COleObjectFactory::UpdateRegistryAll(FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
		return FALSE;
	}
	// App was launched standalone or with other switches (e.g. /Register
	// or /Regserver).  Update registry entries, including typelibrary.
	else
	{
		COleObjectFactory::UpdateRegistryAll();
		_AtlModule.UpdateRegistryAppId(TRUE);
		_AtlModule.RegisterServer(TRUE);
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
		if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister)
			return FALSE;
	}


	// Register File Associations
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szPath, sizeof(szPath));
	CString szAppPath = szPath;

	CFileTypeAccess FTA;

	FTA.SetExtension(_T("pah"));

	// Open - just pass file path in quotes on command line
	CString csTempText;
	csTempText  = szAppPath;
	csTempText += " \"%1\"";
	FTA.SetShellOpenCommand(csTempText);
	FTA.SetDocumentShellOpenCommand(csTempText);

	FTA.SetDocumentClassName(_T("Pah.Document"));

	// use first icon in program
	csTempText  = szAppPath;
	csTempText += ",0";
	FTA.SetDocumentDefaultIcon(csTempText);

	FTA.RegSetAllInfo();


	//::MessageBox(NULL, _T("Test"), _T("PAH"), MB_OK);

	CPAHStudioDlg dlg;
	m_pMainWnd = &dlg;
	dlg.SetFile(cmdInfo.m_strFileName);
	INT_PTR nResponse = dlg.DoModal();
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
BOOL CPAHStudioApp::ExitInstance(void)
{
#if !defined(_WIN32_WCE) || defined(_CE_DCOM)
	_AtlModule.RevokeClassObjects();
#endif
	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
void CPAHStudioApp::WakeUp ( LPCTSTR aCommandLine ) const
// Description: virtual override for wake up, so we can process the command line
{
	// Call parent class to handle the basic functionality
	CSingleInstance::WakeUp ( aCommandLine ) ;	

	//// Send new command line to the dialog
	//CPAHStudioDlg* PAHStudioDlg = (CPAHStudioDlg*) m_pMainWnd ;
	//if ( CPAHStudioDlg )
	//{
	//	lSingletonDlg -> SetCommandLine ( aCommandLine ) ;
	//}
}
[
	module(unspecified, uuid = "{8B85EE30-7D0F-4337-8C24-71DFC1E9C850}", name = "PAHStudioLib")
];
/////////////////////////////////////////////////////////////////////////////
LPCTSTR CPAHStudioApp::GetHomeFolder()
{
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szPath, sizeof(szPath));
	CString szAppPath = szPath;

	int i = szAppPath.ReverseFind(_T('\\'));
	CString szHomeFolder = szAppPath.Left(i + 1);

	szAppPath.MakeLower();
	int iDebug = szAppPath.Find(_T("\\debug\\"));
	if (iDebug > 0)
		szHomeFolder = szHomeFolder.Left(iDebug + 1);
	int iRelease = szAppPath.Find(_T("\\release\\"));
	if (iRelease > 0)
		szHomeFolder = szHomeFolder.Left(iRelease + 1);

	return szHomeFolder;
}
