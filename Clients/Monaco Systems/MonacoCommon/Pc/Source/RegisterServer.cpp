#include "afxwin.h"
#include "ole2.h"
#include "Message.h"
#include "RegisterServer.h"

#define FAIL_ARGS	1
#define FAIL_OLE	2
#define FAIL_LOAD	3
#define FAIL_ENTRY	4
#define FAIL_REG	5

static char _szDllRegSvr[] = "DllRegisterServer";
static char _szDllUnregSvr[] = "DllUnregisterServer";

/////////////////////////////////////////////////////////////////////////////
int RegisterServer( LPCSTR pszDllName, BOOL bSilent, BOOL bUnregister )
{
	if (!pszDllName)
		return FAIL_ARGS;

	// Initialize OLE.				
	if (FAILED(OleInitialize(NULL)))
	{
		if (!bSilent)
			Message("OleInitialize failed.");
		return FAIL_OLE;
	}

	// Load the library.	
	HINSTANCE hLib = LoadLibrary(pszDllName);
	if (hLib < (HINSTANCE)HINSTANCE_ERROR)
	{
		if (!bSilent)
			Message("Failed to load needed library '%s' after error %d (0x%08lx).",
				pszDllName, GetLastError(), GetLastError());

		OleUninitialize();
		return FAIL_LOAD;
	}

	// Find the entry point.		
	LPSTR pszDllEntryPoint = (bUnregister ? _szDllUnregSvr : _szDllRegSvr);
	HRESULT (FAR STDAPICALLTYPE * lpDllEntryPoint)(void);
	(FARPROC&)lpDllEntryPoint = GetProcAddress(hLib, pszDllEntryPoint);
	if (!lpDllEntryPoint)
	{
		if (!bSilent)
			Message("Loaded library %s, but entry point %s not found",
				pszDllName, pszDllEntryPoint);

		FreeLibrary(hLib);
		OleUninitialize();
		return FAIL_ENTRY;
	}

	// Call the entry point.	
	if (FAILED((*lpDllEntryPoint)()))
	{
		if (!bSilent)
			Message("Call to entry point %s in %s failed.", pszDllEntryPoint, pszDllName);
		FreeLibrary(hLib);
		OleUninitialize();
		return FAIL_REG;
	}

	FreeLibrary(hLib);
	OleUninitialize();
	return 0;
}			
