// VDCrusher.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include "VDCrusher.h"

//////////////////////////////////////////////////////////////////////
class CVDCrusherModule : public CAtlDllModuleT< CVDCrusherModule >
{
public :
	DECLARE_LIBID(LIBID_VDCrusherLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_VDCRUSHER, "{B0A7838B-A92D-4748-B1BA-448063344675}")
};

CVDCrusherModule _AtlModule;

//////////////////////////////////////////////////////////////////////
// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
    return _AtlModule.DllMain(dwReason, lpReserved); 
}

//////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}

//////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

//////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}

//////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}
