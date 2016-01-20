// Hello
// JMHook.cpp : Defines the entry point for the DLL application.
//
// MouseHook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#define _COMPILING_44E531B1_14D3_11d5_A025_006067718D04
#include "JMHook.h"
#include "IPC.h"
#import "progid:AgOutlookAddIn.OEAddin"
#import "progid:AgOutlookAddIn.AOLAddin"

using namespace AgOutlookAddInLib;

#define IPC_CBTHOOK_ID "{CBT-8955-A5D9-4229-A898-74354F79A11B}"
#define IPC_SHELLHOOK_ID "{SHELL-8955-A5D9-4229-A898-74354F79A11B}"
#define IPC_HSERVER_ID "{AB0D34D2-2537-40cf-A8A8-7DA15B9CBFBE}"

#pragma data_seg(".JMShare")
HWND hOEWndServer = NULL;
HWND hAOLWndServer = NULL;
#pragma data_seg()
#pragma comment(linker, "/section:.JMShare,rws")

CIPC g_ipcCBTHook(IPC_CBTHOOK_ID);
CIPC g_ipcSHELLHook(IPC_SHELLHOOK_ID);
HHOOK hookSHELL = NULL; 
HHOOK hookCBT = NULL; 

HINSTANCE hInst;
BOOL bNeedCBTHook		= FALSE;
BOOL CBTHookInit		= FALSE;
BOOL SHELLHookInit		= FALSE;
BOOL ServerInit			= FALSE;
BOOL OEAddInCreated		= FALSE;
BOOL AOLAddInCreated	= FALSE;

HWND hwndOEBrowser		= NULL;
HWND hwndAOLBrowser		= NULL;

UINT UWM_OECREATE;
UINT UWM_OECOMPOSE;
UINT UWM_AOLCOMPOSE;
UINT UWM_SHUTDOWN;
UINT UWM_TBREMOVE;
UINT UWM_TBDESTROY;
UINT UWM_TBSHOW;
INT g_iHookType			= WH_SHELL;


static LRESULT CALLBACK CBTProc(UINT nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK SHELLProc(UINT nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD  Reason, LPVOID Reserved)
{
	switch(Reason)
	{
		case DLL_PROCESS_ATTACH:
		{
			// Only hook if this is the right process.
			TCHAR szModule[MAX_PATH];
			if (::GetModuleFileName(::GetModuleHandle(NULL), szModule, sizeof(szModule)))
			{
				PathStripPath(szModule);
				// NOTE: Temporarily disbaled AOL support until Tim gives go ahead - JHC
				if ( _tcsicmp( szModule, _T("msimn.exe" )) == 0 /*|| _tcsicmp( szModule, _T("waol.exe" )) == 0*/)
				{
					bNeedCBTHook = TRUE;
                   // ::MessageBox(NULL, "module"," DLL_PROCESS_ATTACH", IDOK);
				}	
				else
				if (_tcsicmp( szModule, _T("jmsrvr.exe" )) != 0)
				{
					return FALSE;
				}
			}

			hInst = hInstance;
			UWM_OECREATE	= RegisterWindowMessage(UWM_OECREATE_MSG);
			UWM_OECOMPOSE	= RegisterWindowMessage(UWM_OECOMPOSE_MSG);
            UWM_AOLCOMPOSE	= RegisterWindowMessage(UWM_AOLCOMPOSE_MSG);
			UWM_SHUTDOWN	= RegisterWindowMessage(UWM_SHUTDOWN_MSG);
			UWM_TBREMOVE	= RegisterWindowMessage(UWM_TBREMOVE_MSG);
			UWM_TBSHOW		= RegisterWindowMessage(UWM_TBSHOW_MSG);
			UWM_TBDESTROY	= RegisterWindowMessage(UWM_TBDESTROY_MSG);
			return TRUE;
		}

		case DLL_PROCESS_DETACH:
		{
			if (NULL != hookCBT)
				RemoveCBTHook();
			return TRUE;
		}
    }
    return TRUE;
}

/****************************************************************************/
__declspec(dllexport) BOOL InstallCBTHook(const DWORD dwThreadID)
{
	if(CBTHookInit)
		return FALSE; // already hooked!

	// Lock the shared resources
	g_ipcCBTHook.Lock();
	g_ipcCBTHook.CreateIPCMMF();

	hookCBT = ::SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTProc, hInst, dwThreadID);
	if(NULL != hookCBT)
	{
		// Into the shared resource, store the handle to the hook.
		DWORD dwData = (DWORD)hookCBT;
		g_ipcCBTHook.WriteIPCMMF((LPBYTE)&dwData, sizeof(dwData));
		CBTHookInit = TRUE;
	}
	
	// Ensure that we unlock the shared resources
	g_ipcCBTHook.Unlock();

	bNeedCBTHook = !CBTHookInit;
    return CBTHookInit; 

} // InstallCBTHook

/****************************************************************************/
__declspec(dllexport) BOOL InstallSHELLHook(bool bUseCBT)
{
	if(SHELLHookInit)
		return FALSE; // already hooked!

	// Lock the shared resources
	g_ipcSHELLHook.Lock();
	g_ipcSHELLHook.CreateIPCMMF();

	g_iHookType = WH_SHELL;
	if (bUseCBT) 
		g_iHookType = WH_CBT;
	hookSHELL = ::SetWindowsHookEx(g_iHookType, (HOOKPROC)SHELLProc, hInst, 0);
	if(NULL != hookSHELL)
	{
		// Into the shared resource, store the handle to the hook.
		DWORD dwData = (DWORD)hookSHELL;
		g_ipcSHELLHook.WriteIPCMMF((LPBYTE)&dwData, sizeof(dwData));
		SHELLHookInit = TRUE;
	}
	
	// Ensure that we unlock the shared resources
	g_ipcSHELLHook.Unlock();

    return SHELLHookInit; 
} // InstallSHELLHook

/****************************************************************************/
__declspec(dllexport) BOOL RemoveToolbars(HWND hwndRebar)
{
	if (::IsWindow(hOEWndServer))
	{
		// Tell OEAddin deactivate all toolbars
		::PostMessage(hOEWndServer, UWM_TBREMOVE, 0, (LPARAM)hwndRebar);
		return TRUE;
	}
	return FALSE;
} // RemoveToolbar

/****************************************************************************/
__declspec(dllexport) BOOL ForceShutdown()
{
	bool bSuccess = false;
	if (::IsWindow(hOEWndServer))
	{
		// Tell OEAddin to shutdown
		::PostMessage(hOEWndServer, UWM_SHUTDOWN, 0, 0);
		 bSuccess = true;
	}
	if (::IsWindow(hAOLWndServer))
	{
		// Tell AOLAddin to shutdown
		::PostMessage(hAOLWndServer, UWM_SHUTDOWN, 0, 0);
		bSuccess = true;
	}
	return bSuccess;
} // Shutdown

/****************************************************************************/
__declspec(dllexport) BOOL ShowToolbars()
{
	if (::IsWindow(hOEWndServer))
	{
		// Tell OEAddin deactivate all toolbars
		::PostMessage(hOEWndServer, UWM_TBSHOW, 0, 0);
		return TRUE;
	}
	return FALSE;
} // RemoveToolbar

/****************************************************************************/
__declspec(dllexport) BOOL RemoveCBTHook()
{
	if (!hookCBT)
		return TRUE;

	BOOL unhooked = ::UnhookWindowsHookEx(hookCBT);

	if(unhooked)
	{
		hOEWndServer = NULL;
		hAOLWndServer = NULL;
	}

	return unhooked;
} // RemoveHook

/****************************************************************************/
__declspec(dllexport) BOOL RemoveSHELLHook()
{
	if (!hookSHELL)
		return TRUE;
	
	BOOL unhooked = ::UnhookWindowsHookEx(hookSHELL);
	RemoveCBTHook();

	return unhooked;
} // RemoveHook

/****************************************************************************
*                                   CBTProc
* Inputs:
*       int nCode: Code value
*	WPARAM wParam:
*	LPARAM lParam:
* Result: LRESULT
*       Either 0 or the result of CallNextHookEx
* Effect: 
*       Hook processing function. If the message is a mouse-move message,
*	posts the coordinates to the parent window
****************************************************************************/
static LRESULT CALLBACK CBTProc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	// This function is called from the HOOKED process only
	//if(hookCBT == NULL)
	//{
	//	// Read the data from the shared resources
	//	DWORD dwData = 0, dwSize = sizeof(dwData);
	//	g_ipcCBTHook.Lock();
	//	g_ipcCBTHook.OpenIPCMMF();
	//	g_ipcCBTHook.ReadIPCMMF((LPBYTE)&dwData, dwSize);
	//	g_ipcCBTHook.Unlock();
	//	hookCBT = (HHOOK)dwData;
	//}

	if (!hookCBT)
		return S_OK;

	// if nCode is -1 or not HCBT_CREATEWND release to next hook down chain. 
	if (nCode < 0 || nCode != HCBT_CREATEWND)
		return ::CallNextHookEx(hookCBT, nCode, wParam, lParam);

	if (OEAddInCreated && ::IsWindow(hOEWndServer))
	{
		HWND hWnd = (HWND)wParam;
		char szClass[40];
		::GetClassName(hWnd, szClass, sizeof(szClass) - 1);
		if(!lstrcmp(szClass, "ReBarWindow32"))
		{
			PostMessage(hOEWndServer, UWM_OECREATE, 0, (LPARAM)hWnd);
		} 
		else if(!lstrcmp(szClass, "Internet Explorer_Server"))
		{
			PostMessage(hOEWndServer, UWM_OECOMPOSE, 0, (LPARAM)hWnd);
		} 
	}
    else if (AOLAddInCreated && ::IsWindow(hAOLWndServer))
    {
        HWND hWnd = (HWND)wParam;
		char szClass[40];
		::GetClassName(hWnd, szClass, sizeof(szClass) - 1);
		if(!lstrcmp(szClass, "Internet Explorer_Server"))
		{
            //::MessageBox(NULL, "Internet Explorer_Server"," CBTProc", IDOK);		
            PostMessage(hAOLWndServer, UWM_AOLCOMPOSE, 0, (LPARAM)hWnd);
		} 
    }
	
	
    return ::CallNextHookEx(hookCBT, nCode, wParam, lParam);
}

/****************************************************************************/
static LRESULT CALLBACK SHELLProc(UINT nCode, WPARAM wParam, LPARAM lParam)
{
	if (hookSHELL == NULL)
	{
		// Read the data from the shared resources
		DWORD dwData = 0, dwSize = sizeof(dwData);
		g_ipcSHELLHook.Lock();
		g_ipcSHELLHook.OpenIPCMMF();
		g_ipcSHELLHook.ReadIPCMMF((LPBYTE)&dwData, dwSize);
		g_ipcSHELLHook.Unlock();
		hookSHELL = (HHOOK)dwData;
	}

	// if nCode is -1 or not HCBT_CREATEWND  or if this process has been CBT hooked already 
	// then release to next hook down chain. 
	if (nCode < 0 || (nCode != HCBT_CREATEWND && g_iHookType==WH_CBT) || NULL != hookCBT)
	{
		return ::CallNextHookEx(hookSHELL, nCode, wParam, lParam);
	}

    //::MessageBox(NULL, "Step 1"," ShellProc", IDOK);

	if (bNeedCBTHook && !hookCBT)
	{
		// Create OEAddin if this Outlook Express main
		// or compose window (ATH_Note).
		HWND hWnd = (HWND)wParam;
		char szClass[40];

        // Get window title
        char szTitle[15];
		int nSize = ::GetWindowText(hWnd, szTitle, sizeof(szTitle));
	
        

        // If the new window is an Outlook Express window then create OEAddin. Else if its an AOL window
        // then Creata AOLAddin.
		::GetClassName(hWnd, szClass, sizeof(szClass) - 1);
		if(!OEAddInCreated && !hOEWndServer && (!lstrcmp(szClass, OE_BROWSER_CLASS) || !lstrcmp(szClass, OE_COMPOSEWINDOW_CLASS)))				
		{			
            hwndOEBrowser = hWnd;
			CComPtr<IOEAddin> spOEAddin = NULL;
			HRESULT hRes = spOEAddin.CoCreateInstance(__uuidof(OEAddin));
			if (SUCCEEDED(hRes))
			{
				OEAddInCreated = TRUE;
				hRes = spOEAddin->GetWindowHandle((OLE_HANDLE*)&hOEWndServer);
				//::MessageBox(NULL, "OE_BROWSER_CLASS"," ShellProc", IDOK);
			}
			//_ASSERTE(SUCCEEDED(hRes) && (NULL != hOEWndServer));
		}
        else if (!AOLAddInCreated && !hAOLWndServer && !lstrcmp(szClass, AOL_BROWSER_CLASS))				
		{	
			//::MessageBox(NULL, "AOL_BROWSER_CLASS 2"," ShellProc", IDOK);
			if (!lstrcmp(szTitle, AOL_BROWSER_TITLE))
			{
				hwndAOLBrowser = hWnd;
				CComPtr<IAOLAddin> spAOLAddin = NULL;
				HRESULT hRes = spAOLAddin.CoCreateInstance(__uuidof(AOLAddin));
				if (SUCCEEDED(hRes))
				{
					AOLAddInCreated = TRUE;
					hRes = spAOLAddin->GetWindowHandle((OLE_HANDLE*)&hAOLWndServer);
					if (SUCCEEDED(hRes))
						spAOLAddin->SetTopParent((OLE_HANDLE)hwndAOLBrowser);

					//::MessageBox(NULL, "AOL_BROWSER_CLASS"," ShellProc", IDOK);
				}
			}
			//_ASSERTE(SUCCEEDED(hRes) && (NULL != hOEWndServer));
		}

		if (bNeedCBTHook && (OEAddInCreated || AOLAddInCreated))
		{
			// Install CBT Hook for this thread
			DWORD dwThreadId = ::GetCurrentThreadId();
			InstallCBTHook(dwThreadId);	
			//::MessageBox(NULL, "InstallCBTHook"," ShellProc", IDOK);
		}
	}
	
    return ::CallNextHookEx(hookSHELL, nCode, wParam, lParam);
}
