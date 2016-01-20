#pragma once
#include "stdafx.h"
#include "resource.h"
#include "JMSrvr.h"
#include "ShellIconHelper.h"
#include "Authenticate.h"
#include "RegisterDialog.h"
#include "utilitywindow.h"
#include <vector>
#include "HelperFunctions.h"
#include "OptionsPropertysheet.h"

//#include "AutoUpdate.h"


struct ClientInfoType
{
	HWND hwnd;
	DWORD Type; 	
}; 

typedef std::vector<ClientInfoType> CLIENTLIST;

static const int	EVENT_ID = 315;					//any arbitary value
static const int	DISPID_AUTOUPDATE_COMPLETE = 1;	//disp id of AutoUpdateComplete...
static _ATL_FUNC_INFO OnAutoUpdateCompleteInfo	= {CC_STDCALL, VT_EMPTY, 1,	{ VT_UI4}};

// CJMBrkr
class ATL_NO_VTABLE CJMBrkr : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CJMBrkr, &CLSID_JMBrkr>,
	public ISupportErrorInfo,
	public IDispatchImpl<IJMBrkr, &IID_IJMBrkr, &LIBID_JMSrvrLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public CShellIconHelper<CJMBrkr>, // Use the shell icon helper class
	public IDispEventSimpleImpl<EVENT_ID, CJMBrkr, &DIID__IUpdateObjEvents>
{
public:
	CJMBrkr();
	~CJMBrkr();
	void Shutdown();

	DECLARE_REGISTRY_RESOURCEID(IDR_JMBRKR)

	DECLARE_NOT_AGGREGATABLE(CJMBrkr)

	DECLARE_CLASSFACTORY_SINGLETON(CJMBrkr)

	BEGIN_COM_MAP(CJMBrkr)
		COM_INTERFACE_ENTRY(IJMBrkr)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
	END_COM_MAP()

	BEGIN_MSG_MAP(CJMBrkr)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_USER, OnUserCommand)
	END_MSG_MAP()

	// For scripting security
	BEGIN_CATEGORY_MAP(CJMBrkr)
		IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
		IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
	END_CATEGORY_MAP()

	// Sink Map
	BEGIN_SINK_MAP(CJMBrkr)
		SINK_ENTRY_INFO(EVENT_ID, DIID__IUpdateObjEvents, DISPID_AUTOUPDATE_COMPLETE, OnAutoUpdateComplete, &OnAutoUpdateCompleteInfo)
	END_SINK_MAP()

	// Methods
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);
	STDMETHOD(Visible)(VARIANT_BOOL bVisible);
	STDMETHOD(Animate)(VARIANT_BOOL bAnimate, UINT nDuration);
	STDMETHOD(SetText)(BSTR szText);
	STDMETHOD(Register)(LONG Mode, VARIANT_BOOL* bSuccess);
	STDMETHOD(OnAuthenticate)(VARIANT varCustomerNum, 
							  VARIANT varUserId, 
							  VARIANT varHost, 
							  VARIANT varUpgradeUrl, 
							  VARIANT varUsageUrl, 
							  VARIANT varHelp, 
							  VARIANT varBrowse,
							  VARIANT varOptions,
							  VARIANT varUserStatus, 
							  VARIANT varShow, 
							  VARIANT verResultStatus);
	STDMETHOD(Show)(VARIANT varShow, VARIANT varWidth, VARIANT varHeight);
	STDMETHOD(ExecuteCommand)(VARIANT varId, VARIANT_BOOL* bSuccess);
	STDMETHOD(RegisterClient)(OLE_HANDLE Handle, VARIANT varType, OLE_HANDLE* phUtilWnd);
	STDMETHOD(UnregisterClient)(OLE_HANDLE Handle);
	STDMETHOD(KillAll)(VARIANT varId);

	// Properties
	STDMETHOD(get_Version)(VARIANT* pVal);
	STDMETHOD(get_SignInUrl)(VARIANT varId, VARIANT* pVal);
	STDMETHOD(put_SignInUrl)(VARIANT varId, VARIANT newVal);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{				
		m_CurrentIcon = 0;
		m_bAnimate = false;

		bool bUse16Colors = false;
		OSVERSIONINFO v;
		memset(&v, 0, sizeof(v));
		v.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (::GetVersionEx(&v))
		{
		//j	CString strMsg;
		//j	strMsg.Format("Windows%s version %d.%d", (VER_PLATFORM_WIN32_NT ? "NT" : ""), v.dwMajorVersion, v.dwMinorVersion);
		//j	MessageBox(strMsg, "Test");
			if (v.dwPlatformId != VER_PLATFORM_WIN32_NT)
				bUse16Colors = true;
			else
			if (v.dwMajorVersion < 5)
				bUse16Colors = true;

			if (v.dwMajorVersion == 5 && v.dwMinorVersion < 1)
				bUse16Colors = true;

			if (v.dwMajorVersion < 5)
				m_bUseCBTHook = true;
		}

		// Configure the shell, then create it by calling SetShellVisible
		SetShellTipText(g_szAppName);
		SetShellIcon(bUse16Colors ? IDI_LOGO16 : IDI_LOGO);
		SetShellVisible(true);
		InstallHook();
		m_RegisterDialog.SetDispatch(this);

		UTILITYCALLBACK pCallBack = (UTILITYCALLBACK)CJMBrkr::UtilityCallback;
		m_UtilWnd.Initialize(NULL, pCallBack, (LPARAM)this);

		//HRESULT hr = m_spUpdateObj.CoCreateInstance(CComBSTR("AgOutlookAddIn.UpdateObj"), NULL);
		//hr = DispEventAdvise(m_spUpdateObj, &DIID__IUpdateObjEvents);
	
        return S_OK;
	}
	
	void FinalRelease() 
	{
		HRESULT hr;
		if (m_spUpdateObj)
			hr = DispEventUnadvise(m_spUpdateObj);

		// Get rid of the icon out of the system tray.
		SetShellVisible(false);
		RemoveHook();
	}

	// Sink handlers
	//void __stdcall OnAutoUpdateComplete(long lResult);
	STDMETHOD_(void, OnAutoUpdateComplete)(DWORD dwResult);

public:
	CLIENTLIST m_Clients;
	void ShowUpgradePage(int iSource);
	bool ProcessExecuteCommand(int iCmd);
	bool RemoveClient(HWND hwnd);
	bool RemoveHook();
	bool InstallHook();
	BOOL LaunchOptimizationWizard();
	BOOL CheckForUpdates();
	BOOL ManageOptions();
	BOOL UpdatePreferences();
	BOOL DisableClient(HWND hwnd, DWORD dwType);
	BOOL BroadcastMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void CALLBACK UtilityCallback(UINT uMsg, LPARAM lParam1, LPARAM lParam2);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUserCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	BOOL ShowFeedbackPage();
	BOOL PostMessagebyClientType(UINT uType, UINT uMsg, WPARAM wParam, LPARAM lParam);
	bool RefreshRegistration();
	

private:
	CComPtr<AgOutlookAddInLib::IUpdateObj> m_spUpdateObj;
	CUtilityWindow m_UtilWnd;
	CComPtr<IDispatch> m_pDispatch;
	CRegisterDialog m_RegisterDialog;
	bool m_bUseCBTHook;
	BOOL m_bAnimate;
	WORD m_CurrentIcon;
	HINSTANCE m_hJMHookInst;
	CAuthenticate m_Authenticate;
	COptionsPropertySheet m_OptPropsheet;

};

OBJECT_ENTRY_AUTO(__uuidof(JMBrkr), CJMBrkr)
