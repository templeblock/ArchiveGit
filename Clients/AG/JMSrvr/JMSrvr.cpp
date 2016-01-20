// JMSrvr.cpp : Implementation of WinMain
#include "stdafx.h"
#include "JMSrvrDef.h"
#include "JMBrkr.h"


#define MUTEX_NAME		_T("{9E69517E-B337-437c-B3F8-2BBF87F5A84A}")
static CComPtr<IJMBrkr> g_spJMBrkr = NULL;
CString g_szAppName = String(IDS_PROJNAME);
//////////////////////////////////////////////////////////////////////////////////////////////////
bool HandleCommandLine()
{
	// Check command Line arguments. 
	// If command line arg is "o" or "O" then launch options/settings dialog.
	HRESULT hr = S_OK;
	CString szCmdLine = GetCommandLine();
	szCmdLine.MakeLower();
	if (szCmdLine.Find(_T("\\o"))>=0)
	{
		CComPtr<IJMBrkr> spJMBrkr = NULL;
		hr = spJMBrkr.CoCreateInstance(__uuidof(JMBrkr), NULL, CLSCTX_LOCAL_SERVER);
		if (NULL != spJMBrkr.p && SUCCEEDED(hr))
		{
			VARIANT_BOOL bSuccess = false;
			hr = spJMBrkr->ExecuteCommand(CComVariant(CMD_CMSETTINGS), &bSuccess);
		}
	}

	return SUCCEEDED(hr);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT JMInit()
{
	// Created a global instance of JMBrkr 
	// that will handle the system icon.
	HRESULT hRes = g_spJMBrkr.CoCreateInstance(__uuidof(JMBrkr), NULL, CLSCTX_LOCAL_SERVER);

	_ASSERTE(SUCCEEDED(hRes));
	if (SUCCEEDED(hRes))
	{
		CAuthenticate Auth;
		DWORD dwShow = 1;
		Auth.ReadShowIcon(dwShow);
		g_spJMBrkr->Visible((VARIANT_BOOL)dwShow);
		VARIANT_BOOL bSuccess = false;

		// Check command line arguments.
		HandleCommandLine();
	}
	
	return hRes;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT JMCleanUp()
{
	if (g_spJMBrkr)
		g_spJMBrkr = NULL;

	return S_OK;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
int CJMSrvrModule::WinMain(int nShowCmd) throw()
{
	// Check if another instance is running. If we are NOT able
	// to create the mutex then an instance is already running.
	if(!m_SingleInstance.Create(MUTEX_NAME))
	{
		// Check command line arguments.
		HandleCommandLine();
		return 0;
	}

	return CAtlExeModuleT<CJMSrvrModule>::WinMain(nShowCmd);
}
//////////////////////////////////////////////////////////////////////////////////////////////////	
HRESULT CJMSrvrModule::PreMessageLoop(int nShowCmd) throw( )
{
	CAtlExeModuleT< CJMSrvrModule >::RegisterServer(TRUE);       
	HRESULT hr = CAtlExeModuleT< CJMSrvrModule >::PreMessageLoop(nShowCmd);
    if (SUCCEEDED(hr))
    {
        hr = JMInit();
    }
    return hr;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CJMSrvrModule::PostMessageLoop() throw( )
{
    HRESULT hr = CAtlExeModuleT< CJMSrvrModule >::PostMessageLoop();
    if (SUCCEEDED(hr))
    {
        hr = JMCleanUp();
    }
    return hr;
}


CJMSrvrModule _Module;


//////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR lpCmdLine, int nShowCmd)
{	
	return _Module.WinMain(nShowCmd);;
}