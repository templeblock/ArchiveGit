// PSLaunchCtrl.cpp : Implementation of CPSLaunchCtrl
#include "stdafx.h"
//#include "PSLaunchDLL.h"
#include "PSLaunchCtrl.h"
#include "PSLaunchData.h"

#include <afxdlgs.h>

#include "PSLaunchDlg.h"
#include "CommonTypes.h"

CProjectInfo _ProjectInfo;

IMPLEMENT_DYNCREATE(CPSLaunchCtrl, CCmdTarget)

/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPSLaunchCtrl, "PSLaunch.PSLaunchCtrl.1",
	0x8DC39CD3, 0x4D90, 0x11d2, 0xBD, 0x04, 0x00, 0x60, 0x08, 0x47, 0x3d, 0xd0)

/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_IPSLaunchCtrl =
		{ 0x8DC39CD2, 0x4D90, 0x11d2, { 0xBD, 0x04, 0x00, 0x60, 0x08, 0x47, 0x3d, 0xd0 } };

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchCtrl::CPSLaunchCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CPSLaunchCtrl

BOOL CPSLaunchCtrl::CPSLaunchCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPSLaunchCtrl

CPSLaunchCtrl::CPSLaunchCtrl()
{
}

CPSLaunchCtrl::~CPSLaunchCtrl()
{
}

#if 0
// ****************************************************************************
//
//  Function Name:	AlreadyRunning
//
//  Description:	Tries to connect to an instance of the PSD DDE Server. If 	
//					the connect succeeds then we are already running.
//
//  Returns:		TRUE if another instance of the servers is running.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL CPSLaunchCtrl::AlreadyRunning( HSZ hServName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	DWORD dwDDEInstance = ((CPSLaunchDLLApp *) AfxGetApp())->GetDDEInstance();

	 // use Windows ANSI code page
	HSZ hTopicName = ::DdeCreateStringHandle( dwDDEInstance, _T("SetForegroundWindow"), CP_WINANSI );

	// could not create string, ASSUME that another instance is running.
	if (hTopicName == 0L)
		return TRUE;

	// attempt to connect to the server
	HCONV hConv = ::DdeConnect( dwDDEInstance, hServName, hTopicName, NULL );

	// free topic name
	::DdeFreeStringHandle( dwDDEInstance, hTopicName );

	if (hConv)
	{
		::DdeDisconnect( hConv );
		return TRUE;   // instance already running.
	}

	// instance was not found.
	return FALSE;
}
#endif

// ****************************************************************************
//
//  Function Name:	CPSLaunchCtrl::FindAppWindow( )
//
//  Description:	Tries to find a top-level window with the 
//                  specified property
//
//  Returns:		S_OK if successful; S_FALSE otherwise
//
//  Exceptions:		None
//
// ****************************************************************************
//
HWND CPSLaunchCtrl::FindAppWindow( const CString& strPropName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	HWND hWnd = ::GetTopWindow( NULL );

	while (hWnd)
	{
		if (::GetProp( hWnd, strPropName ))
			return hWnd;

		hWnd = ::GetNextWindow( hWnd, GW_HWNDNEXT );
	}

	return NULL;
}

BEGIN_INTERFACE_MAP(CPSLaunchCtrl, CCmdTarget)
    INTERFACE_PART(CPSLaunchCtrl, IID_IPSLaunchCtrl, LaunchCtrl)
END_INTERFACE_MAP()

ULONG CPSLaunchCtrl::XLaunchCtrl::AddRef( )
{
	METHOD_PROLOGUE(CPSLaunchCtrl, LaunchCtrl);
	return pThis->ExternalAddRef();
}

ULONG CPSLaunchCtrl::XLaunchCtrl::Release( )
{
	METHOD_PROLOGUE(CPSLaunchCtrl, LaunchCtrl);
	return pThis->ExternalRelease();
}

HRESULT CPSLaunchCtrl::XLaunchCtrl::QueryInterface( REFIID iid, void** ppvObj )
{
	METHOD_PROLOGUE(CPSLaunchCtrl, LaunchCtrl);
    return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

// ****************************************************************************
//
//  Function Name:	CPSLaunchCtrl::DoFileNew( )
//
//  Description:	Displays the project picker dialog, and then
//					launches the appropriate application base on
//                  the users selections 
//
//  Returns:		S_OK if successful; S_FALSE otherwise
//
//  Exceptions:		None
//
// ****************************************************************************
//
STDMETHODIMP CPSLaunchCtrl::XLaunchCtrl::DoFileNew(HWND hwndParent, EAppType eCallingApp, EResult* pResult)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	METHOD_PROLOGUE(CPSLaunchCtrl, LaunchCtrl);

	HRESULT hResult = S_OK;

	try
	{
		int nResult = -1;

		// Do the launcher dialog
		//

		if (hwndParent)
		{
			// Launch the dialog with the parent window
			//
			CWnd* pWnd = CWnd::FromHandle( hwndParent );
			
			CPSLaunchDlg dlg( pWnd );
			nResult = dlg.DoModal();
		}
		else
		{
			// Launch the dialog without a parent window
			//
			CPSLaunchDlg dlg;
			nResult = dlg.DoModal();
		}

		*pResult = (EResult) nResult;

		// Launch the appropriate application
		//
		if (IDCANCEL != nResult)
		{
			EAppType eLaunchApp;

			if (!_ProjectInfo.GetProjectApp( eLaunchApp ))
			{
			#ifdef _DEBUG
				::MessageBox( hwndParent, "ERROR: Project not linked to an application!", 
					"Error", MB_OK | MB_ICONINFORMATION );
			#endif

				throw E_FAIL;
			}

			// Map application type to application name.
			//
			CString strAppName;
			if (!strAppName.LoadString( IDS_APP_START + eLaunchApp ))
				throw E_FAIL;

			try 
			{
				if (eLaunchApp != eCallingApp)
				{
					// Let the caller know a different 
					// app is being launched
					*pResult = kResultOtherApp;

					CString strPath;
					::GetModuleFileName( NULL, strPath.GetBuffer( _MAX_PATH ), _MAX_PATH );
					strPath.ReleaseBuffer();

					int nPos = strPath.ReverseFind( '\\' );
					strPath = strPath.Left( nPos - 1 );

					::SetCurrentDirectory( strPath );

					// Launch a new instance of the application.
					CString strExe = strAppName + _T( ".exe" );
//					strExe += " /" + strProjName + " /TALL";

					SECURITY_ATTRIBUTES sa = { sizeof( SECURITY_ATTRIBUTES ), NULL, FALSE };
					HANDLE hFile = ::CreateFileMapping(  
						(HANDLE)0xFFFFFFFF,			// handle to file to map
						&sa,						// optional security attributes
						PAGE_READWRITE,				// protection for mapping object
						0,							// high-order 32 bits of object size
						sizeof( _ProjectInfo ),		// low-order 32 bits of object size
						"LAUNCHDATA" );				// name of file-mapping object;

					LPVOID lpVoid = ::MapViewOfFile(
						hFile,						// file-mapping object to map into address space
						FILE_MAP_WRITE,				// access mode
						0,							// high-order 32 bits of file offset
						0,							// low-order 32 bits of file offset
						0 );						// number of bytes to map);

					memcpy( lpVoid, &_ProjectInfo, sizeof( _ProjectInfo ) );
					::UnmapViewOfFile( lpVoid );

					HANDLE hEvent = ::CreateEvent( &sa, FALSE, FALSE, "LaunchEvent" );


//					CString strTemp;
//					strTemp.Format( " /LAUNCHDATA:%u", hData );
					strExe += " /LAUNCHDATA";

					STARTUPINFO tStartupInfo;
					PROCESS_INFORMATION tProcessInformation;

					memset( &tStartupInfo, 0, sizeof( tStartupInfo ) );
					tStartupInfo.cb = sizeof(STARTUPINFO);

					if (!::CreateProcess(
							NULL,
							(LPTSTR)(LPCTSTR) strExe,
							NULL,
							NULL,
							TRUE,
							NORMAL_PRIORITY_CLASS,
							NULL,
							NULL,
							&tStartupInfo,
							&tProcessInformation )
						)
					{
//						throw E_FAIL;
					}

					::WaitForInputIdle( tProcessInformation.hProcess, INFINITE );
					::WaitForSingleObject( hEvent, 5000 );

					// Remove our reference to the process handle
					::CloseHandle( tProcessInformation.hProcess );
					::CloseHandle( tProcessInformation.hThread );

					::CloseHandle( hFile );
					::CloseHandle( hEvent );
				}


				// Attempt to connect to the server
				//
/*				HSZ   hTopicName = ::DdeCreateStringHandle( dwDDEInstance, _T("NewData"), CP_WINANSI );
				if (!hTopicName) throw E_FAIL;

				HCONV hConv = ::DdeConnect( dwDDEInstance, hServName, hTopicName, NULL );

				if (hConv)
				{
					AFX_MANAGE_STATE(AfxGetAppModuleState())

					// Send the XTYP_EXECUTE command
					DWORD dwResult;
					HDDEDATA hData = ::DdeClientTransaction(
						(LPBYTE) &_ProjectInfo,
						(DWORD) sizeof( _ProjectInfo ),	// length of data
						hConv,							// handle to conversation
						0L,								// handle to item name string
						0L,								// clipboard data format
						XTYP_EXECUTE,					// transaction type
						(DWORD) 5000,					// time-out duration
						(LPDWORD) &dwResult 			// pointer to transaction result
						);		

					// clean-up the data
					::DdeFreeDataHandle( hData );
					::DdeDisconnect( hConv );

					// the server should have processed the command.
					ASSERT( dwResult == DDE_FACK );
					TRACE( "dwResult = %d, DdeGetLastError = %d\n", dwResult, ::DdeGetLastError( dwDDEInstance ) );

				}
				else
				{
					hResult = E_FAIL;
				}

				::DdeFreeStringHandle( dwDDEInstance , hTopicName );
*/
			}
			catch (HRESULT hr)
			{
				// free server name
//				::DdeFreeStringHandle( dwDDEInstance , hServName );
				hResult = hr;

				throw hr;
			}

			// free server name
//			::DdeFreeStringHandle( dwDDEInstance , hServName );

		}
		else
		{
			*pResult = kResultCancel;
		}
	}
	catch (...)
	{
		hResult = E_FAIL;
	}

	return hResult;
}

STDMETHODIMP CPSLaunchCtrl::XLaunchCtrl::DoFileOpen(HWND hwndParent, EAppType eAppType, EResult* pResult)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CFileDialog dlg( TRUE );
	int nResult = dlg.DoModal();

	if (IDOK == nResult)
	{
		*pResult = kResultOK;

		// Launch app if different then calling app
	}
	else
	{
		*pResult = kResultCancel;
	}

	return S_OK;
}

STDMETHODIMP CPSLaunchCtrl::XLaunchCtrl::GetData(IPSLaunchData** ppObj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	CPSLaunchData* pData = new CPSLaunchData( _ProjectInfo );
	IUnknown* pIUnknown  = pData->GetIUnknown();

	return pIUnknown->QueryInterface( IID_IPSLaunchData, (void **) ppObj );
}
