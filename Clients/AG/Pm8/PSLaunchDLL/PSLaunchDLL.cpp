// PSLaunchDLL.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PSLaunchDLL.h"
ASSERTNAME

#include <winspool.h>

// Framework support
#include "ApplicationGlobals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static _fInitializedGlobals = FALSE;

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchDLLApp

BEGIN_MESSAGE_MAP(CPSLaunchDLLApp, CWinApp)
	//{{AFX_MSG_MAP(CPSLaunchDLLApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchDLLApp construction

CPSLaunchDLLApp::CPSLaunchDLLApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPSLaunchDLLApp object

CPSLaunchDLLApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchDLLApp initialization

BOOL CPSLaunchDLLApp::InitInstance()
{
	AfxEnableControlContainer( );

	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();

	// Initilaize DDE
	//
	m_dwDDEInstance = 0;

	if (::DdeInitialize( &m_dwDDEInstance, (PFNCALLBACK) &DdeCallback, APPCMD_CLIENTONLY, 0 ))
		return FALSE;

	if (!RApplication::m_gsApplicationGlobals)
	{
		RApplication::m_gsApplicationGlobals = new RApplicationGlobals( NULL );
		_fInitializedGlobals = TRUE;
	}

	return TRUE;
}

int CPSLaunchDLLApp::ExitInstance() 
{
	::DdeUninitialize( m_dwDDEInstance );

	if (_fInitializedGlobals)
		delete RApplication::m_gsApplicationGlobals;

	return CWinApp::ExitInstance();
}

// ****************************************************************************
//
//  Function Name:	DdeCallback
//
//  Description:		Registered with DDE to receive client DDE requests.
//						
//	Parameters
//    uType				transaction type
//    uFmt				clipboard data format
//    hconv				handle to the conversation
//    hsz1				handle to a string
//    hsz2				handle to a string
//    hdata				handle to a global memory object
//    dwData1			transaction-specific data
//    dwData2			transaction-specific data
//
//  Returns:			Depends on uType (see WIN32 documentation)
//
//  Exceptions:		None
//
// ****************************************************************************
//
#pragma warning( disable : 4100 )	// unreferenced parameters

HDDEDATA CALLBACK CPSLaunchDLLApp::DdeCallback( UINT uType, UINT uFmt, HCONV hconv, 
											  HSZ hsz1, HSZ hsz2, HDDEDATA hdata, 
											  DWORD dwData1, DWORD dwData2 )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	
	// filter the incoming messages
	switch ( uType )
	{
		case XTYP_ADVSTART:				// Connect requests
		case XTYP_CONNECT:
			return (HDDEDATA) TRUE;
			break;

		case XTYP_ADVREQ:				// Data
		case XTYP_REQUEST:
		case XTYP_WILDCONNECT:
			return (HDDEDATA) NULL;
			break;

		case XTYP_ADVDATA:				// we should have valid data, 
		case XTYP_EXECUTE:				// fall through to interpret the XTYP_EXECUTE data.
		case XTYP_POKE:
			break;

		case XTYP_ADVSTOP:				// DDE notifications
		case XTYP_CONNECT_CONFIRM:
		case XTYP_DISCONNECT:
		case XTYP_ERROR:
		case XTYP_MONITOR:
		case XTYP_REGISTER:
		case XTYP_XACT_COMPLETE:
		case XTYP_UNREGISTER:
			return (HDDEDATA) TRUE;
			break;

		default:						// undocumented DDE commands
			ASSERT( FALSE );
			return (HDDEDATA) TRUE;
		break;
	}

	return (HDDEDATA)  DDE_FACK;
}

/////////
// @mfunc <c CPSLaunchDLLApp> public function sets printer paper and Orientation
//          
void CPSLaunchDLLApp::SetPrintFormat(
    short sPaper,       // @parm DMPAPER_* (see Windows' DEVMODE documentation)
    short sOrient)      // @parm DMORIENT_LANDSCAPE or DMORIENT_PORTRAIT
{
    // Get default printer settings.
    PRINTDLG   pd;
    pd.lStructSize = (DWORD) sizeof(PRINTDLG);

    if (GetPrinterDeviceDefaults(&pd))
    {
        // Lock memory handle.
        DEVMODE FAR* pDevMode = (DEVMODE FAR*)::GlobalLock(m_hDevMode);

        if (pDevMode)
        {
            // Change printer settings in here.
            pDevMode->dmFields      = DM_ORIENTATION | DM_PAPERSIZE;
            pDevMode->dmPaperSize   = sPaper;
            pDevMode->dmOrientation = sOrient;

            LPDEVNAMES lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
            LPTSTR lpszDriverName = (LPTSTR )lpDevNames + lpDevNames->wDriverOffset;
            LPTSTR lpszDeviceName = (LPTSTR )lpDevNames + lpDevNames->wDeviceOffset;
            LPTSTR lpszPortName   = (LPTSTR )lpDevNames + lpDevNames->wOutputOffset;
 
            HANDLE hPrinter;

            // See SDK help for ::DocumentProperties for detailed explanation.
            ::OpenPrinter(lpszDeviceName, &hPrinter, NULL);
            ::DocumentProperties(NULL, hPrinter, lpszDeviceName,
                                 pDevMode, pDevMode, DM_IN_BUFFER | DM_OUT_BUFFER);
            ::ClosePrinter(hPrinter);
            ::GlobalUnlock(m_hDevNames);

            // Unlock memory handle.
            ::GlobalUnlock(m_hDevMode);
        }
    }
}

CString CPSLaunchDLLApp::FindModulePath()
{
    // Get the current application directory
    char szAppPath [_MAX_PATH];
    char szDrive   [_MAX_DRIVE];
    char szDir     [_MAX_DIR];

    ::GetModuleFileName( NULL, szAppPath, _MAX_PATH);
    ::_splitpath( szAppPath, szDrive, szDir, NULL, NULL );

    CString strPath;
    strPath = CString(szDrive);
    strPath += CString(szDir);

    return strPath;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

STDAPI DllCreateInstance(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	IClassFactory* pFactory = NULL;
	HRESULT hr = DllGetClassObject( rclsid, IID_IClassFactory, (void **) &pFactory );

	if (S_OK == hr)
	{
		hr = pFactory->CreateInstance( NULL, riid, ppv );
		pFactory->Release();
	}

	return hr;
}

