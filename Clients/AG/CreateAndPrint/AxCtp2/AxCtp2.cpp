#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "AxCtp2.h"
#include "AxCtp2_i.c"
#include "Ctp.h"
#include "scappint.h"
#include "Msiquery.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_Ctp, CCtp)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
void RegisterSystemVersion()
{
	TCHAR szSysPath[MAX_PATH];
	::GetSystemDirectory(szSysPath, sizeof(szSysPath));

	CString szParam = _T("/y ");
	szParam += _T("AxCtp2.dll");
	CString szTarget = _T("msiexec.exe");
	int iResult = (int)::ShellExecute(NULL, _T("open"), szTarget, szParam, szSysPath, SW_SHOWNORMAL);
	ATLASSERT(iResult > 32);

}
/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_Module.Init(ObjectMap, hInstance, &LIBID_AXCTP2Lib);
		DisableThreadLibraryCalls(hInstance);
		SCENG_Init();
		::InitCommonControls();
	}
	else
	if (dwReason == DLL_PROCESS_DETACH)
	{
		_Module.Term();
		SCENG_Fini();
	}
	return TRUE;	// ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(true);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	_Module.UnregisterServer(true);
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
void WINAPI HandleUninstall(MSIHANDLE hInstall)
{
	DllUnregisterServer();
	RegisterSystemVersion(); 
}
/////////////////////////////////////////////////////////////////////////////
void WINAPI InstallComplete(MSIHANDLE hInstall)
{
	// Open IE with appropo url.
	CString szUrl;
	szUrl.LoadString(IDS_BETASTART_URL);
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}