// ConnMgr.cpp : Implementation of CConnMgrApp and DLL registration.

#include "stdafx.h"
#include "resource.h"
#include "ConnMgr.h"
#include <AfxSock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CConnMgrApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x94fb2c43, 0x512e, 0x11d2, { 0x8b, 0xef, 0, 0xa0, 0xc9, 0xb1, 0x2c, 0x3d } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


////////////////////////////////////////////////////////////////////////////
// CConnMgrApp::InitInstance - DLL initialization

BOOL CConnMgrApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
      AfxSocketInit();  // Enable use of Winsock
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CConnMgrApp::ExitInstance - DLL termination

int CConnMgrApp::ExitInstance()
{
   if(m_pMainWnd)
   {
      m_pMainWnd->Detach();
      delete m_pMainWnd;
      m_pMainWnd = NULL;
   }

	return COleControlModule::ExitInstance();
}

void CConnMgrApp::SetMainWnd(HWND hwndMainWnd)
{
   if (m_pMainWnd != NULL)
      delete m_pMainWnd;
   m_pMainWnd = new CWnd;
   m_pMainWnd->Attach(hwndMainWnd);
}


//=--------------------------------------------------------------------------=
// StringFromGuidA
//=--------------------------------------------------------------------------=
// returns an ANSI string from a CLSID or GUID
//
// Parameters:
//    REFIID               - [in]  clsid to make string out of.
//    LPSTR                - [in]  buffer in which to place resultant GUID.
//
// Output:
//    int                  - number of chars written out.
//
// Notes:
//
int StringFromGuidA
(
    REFIID   riid,
    LPSTR    pszBuf
)
{
    return wsprintf((char *)pszBuf, "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1,
            riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2],
            riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);

}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
