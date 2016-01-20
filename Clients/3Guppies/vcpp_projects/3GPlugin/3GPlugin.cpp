#include "stdafx.h"
#include "resource.h"

// The module attribute causes DllMain, DllRegisterServer and DllUnregisterServer to be automatically implemented for you
[
	module(dll, uuid = "{81F18C34-F49B-402E-8E55-313D0DAA9F21}", 
	name = "a3GPlugin", 
	helpstring = "3GPlugin 1.0 Type Library",
	resource_name = "IDR_3GPLUGIN")
]
class C3GPluginModule
{
public:
// Override CAtlDllModuleT members
};

#include <Msi.h>
#include <MsiQuery.h>
#pragma comment(lib, "Msi.lib")

class CInstallFirefox
{
public:
	CInstallFirefox() {};
	~CInstallFirefox() {};

	bool Install()
	{
		CString strFile = "firefox.exe";
//j		CString strParameters = "https://addons.mozilla.org/en-US/firefox/downloads/file/17483/mobilizer-0.4-fx.xpi";
		CString strParameters = "-install-global-extension \"C:\\Program Files\\3Guppies\\FirefoxMobilizer.xpi";
		HINSTANCE hInst = ::ShellExecute(NULL/*hWnd*/, "open", strFile, strParameters, NULL/*lpDirecory*/, SW_NORMAL/*nShowCommand*/);
		bool bOK = ((UINT)hInst > HINSTANCE_ERROR);
		return bOK;
	};
};

class CInstallInternetExplorer
{
public:
	CInstallInternetExplorer() {};
	~CInstallInternetExplorer() {};

	bool Install()
	{
		bool bOK = true;
		return bOK;
	};
};

class CInstallWelcome
{
public:
	CInstallWelcome() {};
	~CInstallWelcome() {};

	bool Install()
	{
		CString strURL = _T("C:\\Program Files\\3Guppies\\Welcome.htm");
		HINSTANCE hInst = ::ShellExecute(NULL/*hWnd*/, "open", strURL, NULL/*lpParameters*/, NULL/*lpDirecory*/, SW_NORMAL/*nShowCommand*/);
		bool bOK = ((UINT)hInst > HINSTANCE_ERROR);
		return bOK;
	};
};

static CString MyMsiGetProperty(MSIHANDLE hInstall, LPCTSTR pstrName)
{
	WCHAR Buffer[500] = {0};
	DWORD dwLength = sizeof(Buffer) / sizeof(Buffer[0]);
	UINT uiErrorCode = MsiGetPropertyW(hInstall, CComBSTR(pstrName), Buffer, &dwLength);
	if (uiErrorCode != ERROR_SUCCESS)
		return "";

	CString strProperty = Buffer;
	return strProperty;
}

extern "C" UINT __declspec(dllexport) __stdcall PluginInstall(MSIHANDLE hInstall)
{
	HWND hWnd = ::FindWindow("MsiDialogCloseClass", NULL);
	CString strProperties = MyMsiGetProperty(hInstall, "CustomActionData");
//j	::MessageBox(hWnd, strProperties, _T("CustomActionData"), MB_OK);
	bool bIE = (strProperties.Find(_T("ie='1'")) >= 0);
	bool bFF = (strProperties.Find(_T("ff='1'")) >= 0);
	bool bShell = (strProperties.Find(_T("shell='1'")) >= 0);
	if (bFF)
	{
		CInstallFirefox Firefox;
		if (!Firefox.Install())
			::MessageBox(hWnd, _T("The Firefox plugin could not be installed"), _T("Error"), MB_OK);
	}

	if (bIE)
	{
		CInstallInternetExplorer InternetExplorer;
		if (!InternetExplorer.Install())
			::MessageBox(hWnd, _T("The Internet Explorer plugin could not be installed"), _T("Error"), MB_OK);
	}

	if (bIE || bFF || bShell)
	{
		CInstallWelcome Welcome;
		Welcome.Install();
	}

	return ERROR_SUCCESS;
}
