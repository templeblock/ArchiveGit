// AgOutlookAddIn.cpp : Implementation of DLL Exports.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "AgOutlookAddIn.h"
#include "AgOutlookAddIn_i.c"
#include "MsAddIn.h"
#include "OEAddin.h"
#include "JMExplorerBand.h"
#include "HelperFunctions.h"
#include "Msiquery.h"
#include "BrowserHelperObj.h"
#include "JMToolband.h"
#include "Regkeys.h"

#include <ole2.h>
#include <comcat.h>
#include <olectl.h>

///////////////////////////////////////////////////////////////////////////
//   global variables
///////////////////////////////////////////////////////////////////////////
HINSTANCE   g_hInst;
CComModule _Module;
CString g_szAppName = String(IDS_PROJNAME);

//j CComModule& _Module = (*_pModule);

///////////////////////////////////////////////////////////////////////////
//   private function prototypes
///////////////////////////////////////////////////////////////////////////
BOOL RegisterBand(LPCTSTR szCLSID, LPCTSTR szTitle);
BOOL RegisterComCat(CLSID clsid, CATID catID);
BOOL UnregisterComCat(CLSID clsid, CATID CatID);
BOOL UnregisterBand(LPCTSTR szCLSID);
BOOL RegisterToolBand(LPCTSTR szCLSID, LPCTSTR lpszTitle);
BOOL UnregisterToolBand(LPCTSTR clsid);
BOOL IsToolbandRegistered(LPCTSTR szCLSID);
BOOL RegisterBHO(LPCTSTR szCLSID, LPCTSTR lpszTitle);
BOOL UnregisterBHO(LPCTSTR szCLSID);
BOOL RegisterIEButton(LPCTSTR szCLSID, LPCTSTR lpszTitle);
BOOL UnregisterIEButton(LPCTSTR szCLSID);

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_MsAddIn, CMsAddIn)
	OBJECT_ENTRY(CLSID_OEAddin, COEAddin)
	OBJECT_ENTRY(CLSID_JMExplorerBand, CJMExplorerBand)
	OBJECT_ENTRY(CLSID_JMToolband, CJMToolband)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
	{
		TCHAR pszLoader[MAX_PATH];
		GetModuleFileName(NULL, pszLoader, MAX_PATH);
		_tcslwr(pszLoader);
		if (_tcsstr(pszLoader, _T("explorer.exe"))) 
			return FALSE;

		
		g_hInst = hInstance;
        _Module.Init(ObjectMap, hInstance, &LIBID_AgOutlookAddInLib);
        DisableThreadLibraryCalls(hInstance);
		::InitCommonControls();
    }
    else if (dwReason == DLL_PROCESS_DETACH)
	{
		_Module.Term();
	}
    return TRUE;    // ok
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
    HRESULT hr = _Module.RegisterServer(true);

	// Register the vertical explorer bar object.
    if(!RegisterBand(CLSID_JMEXPLORERBAND_STRING, _T("&Creata Mail")))
		return SELFREG_E_CLASS;

	// Register the component categories for the explorer bar object.
    if(!RegisterComCat(CLSID_JMExplorerBand, CATID_InfoBand))
       return SELFREG_E_CLASS;

	// Register BHO 
    if(!RegisterBHO(CLSID_BROWSERHELPEROBJ_STRING, g_szAppName))
       return SELFREG_E_CLASS;

	// Register IE icon button
    if(!RegisterIEButton(CLSID_JMEXPLORERBAND_STRING, g_szAppName))
       return SELFREG_E_CLASS;

	// Currenly toolband is not used, so we need to check to see
	// if it was registered by previous versions. If so, we unregister.
	if (IsToolbandRegistered(CLSID_JMTOOLBAND_STRING))
		UnregisterToolBand(CLSID_JMTOOLBAND_STRING);

	// Register the tool band object. -- currently not used - JHC 
  /*  if(!RegisterToolBand(CLSID_JMTOOLBAND_STRING, g_szAppName))
		return SELFREG_E_CLASS;*/

    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	UnregisterComCat(CLSID_JMExplorerBand, CATID_InfoBand);
	UnregisterBand(CLSID_JMEXPLORERBAND_STRING);
	UnregisterBHO(CLSID_BROWSERHELPEROBJ_STRING);
	UnregisterIEButton(CLSID_JMEXPLORERBAND_STRING);
	//UnregisterToolBand(CLSID_JMTOOLBAND_STRING);
	return _Module.UnregisterServer(true);
}

/////////////////////////////////////////////////////////////////////////////
UINT WINAPI SetMaintenanceFlag(MSIHANDLE hInstall)
{
	/////////////////////////////////////////////////////////////////////////
	// Set value for Install Status key to indicate Maintenance dialog
	// is displayed. This is used to flag when maintenance mode is in progress.
	// It is cleared by PreventUninstall().
	//
	// NOTE: This is not the Maintenance mode (MSIRUNMODE_MAINTENANCE) that is 
	// defined by MSI which would also be set during a Remove from the 
	// Add or Remove Program section.
	////////////////////////////////////////////////////////////////////////
	SetInstallStatus(UNINSTALL_STATUS_MAINTENANCE);
	return 1; // Continue with the uninstall
}

/////////////////////////////////////////////////////////////////////////////
UINT WINAPI PreventUninstall(MSIHANDLE hInstall)
{
#ifdef NOTUSED
	CString strMsg;
	strMsg.Format("Are you really, really sure you want to uninstall %s?", g_szAppName);
	if (MessageBox(hWndSetup, strMsg, g_szAppName, MB_YESNO) != IDYES)
		return false; // Do not continue uninstalling
#endif NOTUSED

	// DllUnregisterServer is called to ensure that all vestige of Creata Mail COM
	// objects' registration is removed upon uinstall. This fixes the problem where
	// CreataMail would still appear in Explorer Bar menu after uinstall.  
	DllUnregisterServer();

	/*BOOL bMaintMode = MsiGetMode(hInstall, MSIRUNMODE_MAINTENANCE); 
	::MessageBox(NULL, String("TEST bMaintMode = %X", bMaintMode), "Install Test", MB_OK);*/
	
	// If AutoUpdate is not in progress and Uninstall is not in Maintenance mode then
	// the survey page is displayed. Otherwise, if Autoupdate is in progress the status 
	// is now cleared. If Uninstall is in Maintenance mode then Maintenance mode status 
	// is now cleared.
	DWORD dwUpdateStatus = 0;
	DWORD dwInstallStatus = 0;
	bool bInstallStatKeyFound = GetInstallStatus(dwInstallStatus);
	CAutoUpdate::GetAutoUpdateStatus(dwUpdateStatus);

	if (dwUpdateStatus == AUTOUPDATE_STATUS_DONE)
	{
#ifdef NOTUSED
		if (!bInstallStatKeyFound || dwInstallStatus != UNINSTALL_STATUS_MAINTENANCE)
		{
			CString szURL;
			CAuthenticate Auth;
			if (Auth.ReadHost(szURL))
			{
				szURL += DEFAULT_UNINSTALL;
				DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szURL, NULL, NULL, SW_SHOWNORMAL);	
			}
		}
#endif NOTUSED
	}
	else
		CAutoUpdate::SetAutoUpdateStatus(AUTOUPDATE_STATUS_DONE);

	if (dwInstallStatus == UNINSTALL_STATUS_MAINTENANCE)
		SetInstallStatus(UNINSTALL_STATUS_DONE);

	return 1; // Continue with the uninstall
}
/////////////////////////////////////////////////////////////////////////////
void WINAPI OptimizationWizard(MSIHANDLE hInstall)
{
	CComPtr<IJMBrkr> spBrkr;
	HRESULT hr = spBrkr.CoCreateInstance(CComBSTR("JMSrvr.JMBrkr"), NULL, CLSCTX_LOCAL_SERVER);
	if (SUCCEEDED(hr))
		hr = spBrkr->ExecuteCommand(CMD_OPTIMIZATIONWIZARD);

	return; // Continue with the install
}
/////////////////////////////////////////////////////////////////////////////
UINT WINAPI EnableClients(DWORD dwClientType)
{
	HRESULT hr = S_OK;
	BOOL bExpressRegOK	= true;
	BOOL bOutlookRegOK	= true;
	BOOL bIERegOK		= true;
	BOOL bAOLRegOK		= true;
	if (dwClientType & CLIENT_TYPE_EXPRESS)
	{
		hr = _Module.RegisterServer(true, &CLSID_OEAddin);
		bExpressRegOK = SUCCEEDED(hr);
	}

	if (dwClientType & CLIENT_TYPE_OUTLOOK)
	{
		hr = _Module.RegisterServer(true, &CLSID_MsAddIn);
		bOutlookRegOK = SUCCEEDED(hr);
	}

	// Temp, Until AOL supported - JHC
	/*if (dwClientType & CLIENT_TYPE_AOL)
	{
		hr = _Module.RegisterServer(true, &CLSID_AOLAddin);
		bAOLRegOK = SUCCEEDED(hr);
	}*/

	if (dwClientType & CLIENT_TYPE_IE)
	{		
		BOOL bBandRegOK		= RegisterBand(CLSID_JMEXPLORERBAND_STRING, _T("&Creata Mail"));
		BOOL bComCatRegOK	= RegisterComCat(CLSID_JMExplorerBand, CATID_InfoBand);
		BOOL bBHORegOK		= RegisterBHO(CLSID_BROWSERHELPEROBJ_STRING, g_szAppName);
		BOOL bIEButtonRegOK = RegisterIEButton(CLSID_JMEXPLORERBAND_STRING, g_szAppName);
		bIERegOK = bBandRegOK && bComCatRegOK && bBHORegOK && bIEButtonRegOK;
		_Module.RegisterServer(true, &CLSID_JMExplorerBand);
	}
	

	return (bExpressRegOK && bOutlookRegOK && bIERegOK && bAOLRegOK);
}
/////////////////////////////////////////////////////////////////////////////
UINT WINAPI DisableClients(DWORD dwClientType)
{
	if (dwClientType & CLIENT_TYPE_EXPRESS)
		_Module.UnregisterServer(&CLSID_OEAddin);

	if (dwClientType & CLIENT_TYPE_OUTLOOK)
		_Module.UnregisterServer(&CLSID_MsAddIn);

	if (dwClientType & CLIENT_TYPE_AOL)
		_Module.UnregisterServer(&CLSID_AOLAddin);

	if (dwClientType & CLIENT_TYPE_IE)
	{		
		UnregisterBand(CLSID_JMEXPLORERBAND_STRING);
		UnregisterComCat(CLSID_JMExplorerBand, CATID_InfoBand);
		UnregisterBHO(CLSID_BROWSERHELPEROBJ_STRING);
		UnregisterIEButton(CLSID_JMEXPLORERBAND_STRING); 
		_Module.UnregisterServer(&CLSID_JMExplorerBand);
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////////////
//   RegisterBand
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
// As an example, the full registry entries for an HTML-capable Explorer Bar 
// with a default width of 291 (0x123) pixels are shown here.
//
//
//	HKEY_CLASSES_ROOT
//		CLSID
//			{Your Band Object's CLSID GUID}
//				(Default) = Menu Text String
//				InProcServer32
//					(Default) = DLL Pathname
//					ThreadingModel= Apartment
//				Instance
//					CLSID = {4D5C8C2A-D075-11D0-B416-00C04FB90376}
//					InitPropertyBag
//						Url= Your HTML File
//
//	HKEY_LOCAL_MACHINE
//		Software
//			Microsoft
//				Internet Explorer
//					Explorer Bars
//						{Your Band Object's CLSID GUID}
//							BarSize= 23 01 00 00 00 00 00 00
//
////////////////////////////////////////////////////////////////////////////
BOOL RegisterBand(LPCTSTR szCLSID, LPCTSTR lpszTitle)
{
	CRegKey regkey;
	CString szClsidKey;
	szClsidKey.Format(_T("CLSID\\%38s\\"), szCLSID);

	// Set menu title for explorer bar.
	if (regkey.Create(HKEY_CLASSES_ROOT, szClsidKey) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(NULL, lpszTitle) != ERROR_SUCCESS)
		return false;

	// Add default width for explorer bar
	szClsidKey.Format(_T("Software\\Microsoft\\Internet Explorer\\Explorer Bars\\%38s"), szCLSID);
	if (regkey.Create (REGKEY_APPROOT, szClsidKey) != ERROR_SUCCESS)
		return false;

	// Bar width requires eight bytes and is placed in the registry as a binary value. 
	// The first four bytes specify the size in pixels, in hexadecimal format, starting
	// from the leftmost byte. The last four bytes are reserved and should be set to zero.
	BYTE BarSize[8] = {0x68, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	if (regkey.SetBinaryValue(_T("BarSize"), BarSize, 8) != ERROR_SUCCESS)
		return false;
    
    return true;
}

///////////////////////////////////////////////////////////////////////////
//   UnregisterBand
///////////////////////////////////////////////////////////////////////////
BOOL UnregisterBand(LPCTSTR szCLSID)
{
	CRegKey regkey;
	CString szClsidKey;
	szClsidKey.Format(_T("CLSID\\%38s\\"), szCLSID);

	// Reset menu title for explorer bar.
	if (regkey.Open(HKEY_CLASSES_ROOT, szClsidKey) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(NULL, "") != ERROR_SUCCESS)
		return false;

	// Remove support for HTML in eplorer bar. Remove special CLSID: "{4D5C8C2A-D075-11D0-B416-00C04FB90376}".
	/*regkey.DeleteSubKey( _T("\\Instance\\InitPropertyBag"));*/
	//regkey.DeleteSubKey( _T("\\Instance"));

	regkey.Close();

	//Remove explorer bar settings
	szClsidKey.Format(_T("Software\\Microsoft\\Internet Explorer\\Explorer Bars\\%38s"), szCLSID);
	if (regkey.Open(REGKEY_APPROOT, szClsidKey) != ERROR_SUCCESS)
		return false;

	regkey.DeleteValue(_T("BarSize"));
	regkey.Close();

	if (regkey.Open(REGKEY_APPROOT, _T("Software\\Microsoft\\Internet Explorer\\Explorer Bars")) != ERROR_SUCCESS)
		return false;

	szClsidKey.Format(_T("%38s"), szCLSID);
	regkey.DeleteSubKey(szClsidKey);

	regkey.Close();
   
    return true;
}

///////////////////////////////////////////////////////////////////////////
//   RegisterComCat
//////////////////////////////////////////////////////////////////////////
BOOL RegisterComCat(CLSID clsid, CATID CatID)
{
    CComPtr<ICatRegister>   spCatReg;
    HRESULT hr = S_OK ;
           
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
                          NULL, 
                          CLSCTX_INPROC_SERVER, 
                          IID_ICatRegister, 
                          (LPVOID*)&spCatReg);
    
    if(SUCCEEDED(hr))
       hr = spCatReg->RegisterClassImplCategories(clsid, 1, &CatID);	
    
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////
//   UnregisterComCat
//
//	 Note: This does not remove the "Implemented Categories" key from 
//         HKEY_CLASSES_ROOT\CLSID\<Your GUID>\Implemented Categories. 
//		   However, it is necessary to remove this key for the unregister to
//         succeed without error. So, the JMExplorerBand.rgs file will handle
//		   the removal of "Implemented Categories" key.
//////////////////////////////////////////////////////////////////////////
BOOL UnregisterComCat(CLSID clsid, CATID CatID)
{
    CComPtr<ICatRegister>   spCatReg;
    HRESULT hr = S_OK ;
           
    hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
                          NULL, 
                          CLSCTX_INPROC_SERVER, 
                          IID_ICatRegister, 
                          (LPVOID*)&spCatReg);
    
    if(SUCCEEDED(hr))
		hr = spCatReg->UnRegisterClassImplCategories(clsid, 1, &CatID);	
    
    return SUCCEEDED(hr);
}

///////////////////////////////////////////////////////////////////////////
//   RegisterToolBand 
///////////////////////////////////////////////////////////////////////////
BOOL RegisterToolBand(LPCTSTR szCLSID, LPCTSTR lpszTitle)
{
	CRegKey regkey;
	CString szClsidKey;
	szClsidKey.Format(_T("CLSID\\%38s\\"), szCLSID);

	// Set menu title for explorer bar.
	if (regkey.Create(HKEY_CLASSES_ROOT, szClsidKey) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(NULL, lpszTitle) != ERROR_SUCCESS)
		return false;

	regkey.Close();
	
	// Register toolband with IE
	szClsidKey = _T("Software\\Microsoft\\Internet Explorer\\Toolbar");
	if (regkey.Create(REGKEY_APPROOT, szClsidKey) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(szCLSID, g_szAppName) != ERROR_SUCCESS)
		return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////
//   UnregisterToolBand 
///////////////////////////////////////////////////////////////////////////
BOOL UnregisterToolBand(LPCTSTR szCLSID)
{
	// Register toolband with IE
	CRegKey regkey;
	if (regkey.Open(REGKEY_APPROOT, _T("Software\\Microsoft\\Internet Explorer\\Toolbar")) != ERROR_SUCCESS)
		return false;

	if (regkey.DeleteValue(szCLSID) != ERROR_SUCCESS)
		return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////
//   IsToolbandRegistered 
///////////////////////////////////////////////////////////////////////////
BOOL IsToolbandRegistered(LPCTSTR szCLSID)
{
	CRegKey regkey;
	CString szClsidKey;

	// is szCLSID registered with IE
	szClsidKey = _T("Software\\Microsoft\\Internet Explorer\\Toolbar");
	if (regkey.Open(REGKEY_APPROOT, szClsidKey) != ERROR_SUCCESS)
		return false;

	DWORD dwLength = MAX_PATH;
	TCHAR szBuffer[MAX_PATH];
	if (regkey.QueryStringValue(szCLSID, szBuffer, &dwLength) != ERROR_SUCCESS)
		return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////
//   RegisterBHO 
///////////////////////////////////////////////////////////////////////////
BOOL RegisterBHO(LPCTSTR szCLSID, LPCTSTR lpszTitle)
{
	CRegKey regkey;
	CString szClsidKey;
	szClsidKey.Format(_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\%38s"), szCLSID);

	// Set title for explorer bar bho.
	// Must use HKEY_LOCAL_MACHINE.
	if (regkey.Create(HKEY_LOCAL_MACHINE, szClsidKey) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(NULL, lpszTitle) != ERROR_SUCCESS)
		return false;

	regkey.Close();
	
    return true;
}

///////////////////////////////////////////////////////////////////////////
//   UnregisterBHO
///////////////////////////////////////////////////////////////////////////
BOOL UnregisterBHO(LPCTSTR szCLSID)
{
	// Must use HKEY_LOCAL_MACHINE
	CRegKey regkey;
	if (regkey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects")) != ERROR_SUCCESS)
		return false;

	regkey.DeleteSubKey(szCLSID);
	regkey.Close();
	
    return true;
}

///////////////////////////////////////////////////////////////////////////
//   RegisterIEButton 
///////////////////////////////////////////////////////////////////////////
BOOL RegisterIEButton(LPCTSTR szCLSID, LPCTSTR lpszTitle)
{
	CRegKey regkey;
	CString szClsidKey;
	szClsidKey.Format(_T("Software\\Microsoft\\Internet Explorer\\Extensions\\%38s"), CLSID_JMEXPLORERBUTTON_STRING);

	// Register clsid for button. make visible by default. Set button text. 
	// Set full path for icon.
	if (regkey.Create(REGKEY_APPROOT, szClsidKey) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(_T("Default Visible"), _T("Yes")) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(_T("ButtonText"), lpszTitle) != ERROR_SUCCESS)
		return false;

	// Get install path. Use it to determine icon path. Set path to 3 color icon.
	CString szPath;
	if (!GetInstallPath(szPath))
		return false;

#ifdef _DEBUG
	szPath = _T("C:\\My Projects\\AgApplications\\AgOutlookAddIn\\setup\\");
#endif _DEBUG


	CString szIconPath = szPath;
	szIconPath += _T("Hot.ico");
	if (regkey.SetStringValue(_T("HotIcon"), szIconPath) != ERROR_SUCCESS)
		return false;

	
	szIconPath = szPath;
	//szIconPath += _T("Gray.ico");
	szIconPath += _T("Hot.ico");
	// Set path to gray scale icon file
	if (regkey.SetStringValue(_T("Icon"), szIconPath) != ERROR_SUCCESS) 
		return false;

	// Add support to turn on/off Creata Mail explorerbar.
	if (regkey.SetStringValue(_T("CLSID"), _T("{E0DD6CAB-2D10-11D2-8F1A-0000F87ABD16}")) != ERROR_SUCCESS)
		return false;

	if (regkey.SetStringValue(_T("BANDCLSID"), szCLSID) != ERROR_SUCCESS)
		return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////
//   UnregisterIEButton 
///////////////////////////////////////////////////////////////////////////
BOOL UnregisterIEButton(LPCTSTR szCLSID)
{
	CRegKey regkey;

	if (regkey.Open(REGKEY_APPROOT, _T("Software\\Microsoft\\Internet Explorer\\Extensions")) != ERROR_SUCCESS)
		return false;

	if (regkey.DeleteSubKey(CLSID_JMEXPLORERBUTTON_STRING) != ERROR_SUCCESS)
		return false;

    return true;
}
