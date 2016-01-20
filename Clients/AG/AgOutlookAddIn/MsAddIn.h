#pragma once
#include "resource.h"
#include "MsAddInBase.h"
#include "AgOutlookAddIn.h"
#include "HTMLMenu.h"
#include "HTMLDialog.h"
#include "Download.h"
#include "Timer.h"
#include "HelperFunctions.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CMsAddIn

class CToolbar
{
public:
	CToolbar(void* pInspector)
	{
		m_pInspector = pInspector;
	}
	
	~CToolbar()
	{
		m_ButtonList.clear();
	}
	
	int NumButtons()
	{
		return m_ButtonList.size();
	}
	
	Office::_CommandBarButtonPtr GetButton(int nIndex)
	{
		return m_ButtonList[nIndex];
	}
	
	void AddButton(Office::_CommandBarButtonPtr spCmdButton)
	{
		m_ButtonList.push_back(spCmdButton);
	}

	void* GetInspector()
	{
		return m_pInspector;
	}

private:
	void* m_pInspector;
	vector<Office::_CommandBarButtonPtr> m_ButtonList;
};

extern _ATL_FUNC_INFO OnButtonClickInfo;
extern _ATL_FUNC_INFO OnAppItemSendInfo;
extern _ATL_FUNC_INFO OnNewInspectorInfo;
//jextern _ATL_FUNC_INFO OnAppNewMailInfo;

class ATL_NO_VTABLE CMsAddIn : 
 	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMsAddIn, &CLSID_MsAddIn>,
	public CComControl<CMsAddIn>,
	public ISupportErrorInfo,
	public IDispatchImpl<IMsAddIn, &IID_IMsAddIn, &LIBID_AgOutlookAddInLib>,
	public IDispatchImpl<_IDTExtensibility2, &IID__IDTExtensibility2, &LIBID_AddInDesignerObjects>,
	public IDispEventSimpleImpl<11, CMsAddIn, &__uuidof(Office::_CommandBarButtonEvents)>,
	public IDispEventSimpleImpl<22, CMsAddIn, &__uuidof(Outlook::ApplicationEvents)>,
	public IDispEventSimpleImpl<33, CMsAddIn, &__uuidof(Outlook::InspectorsEvents)>,
//j	public IDispEventSimpleImpl<44, CMsAddIn, &__uuidof(Outlook::ApplicationEvents)>,
	public CMsAddInBase
{
public:
	typedef IDispEventSimpleImpl<11, CMsAddIn, &__uuidof(Office::_CommandBarButtonEvents)> ButtonClickEvents;
	typedef IDispEventSimpleImpl<22, CMsAddIn, &__uuidof(Outlook::ApplicationEvents)> AppItemSendEvents;
	typedef IDispEventSimpleImpl<33, CMsAddIn, &__uuidof(Outlook::InspectorsEvents)> InspectorsEvents;
//j	typedef IDispEventSimpleImpl<44, CMsAddIn, &__uuidof(Outlook::ApplicationEvents)> AppNewMailEvents;

	CMsAddIn();
	~CMsAddIn();

	DECLARE_REGISTRY_RESOURCEID(IDR_ADDIN)

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CMsAddIn)
		COM_INTERFACE_ENTRY(IMsAddIn)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY2(IDispatch, IMsAddIn)
		COM_INTERFACE_ENTRY(_IDTExtensibility2)
	END_COM_MAP()

	BEGIN_SINK_MAP(CMsAddIn)
		SINK_ENTRY_INFO(11, __uuidof(Office::_CommandBarButtonEvents),/*dispid*/ 0x01, OnButtonClick, &OnButtonClickInfo)
		SINK_ENTRY_INFO(22, __uuidof(Outlook::ApplicationEvents),/*dispid*/ 0xf002, OnAppItemSend, &OnAppItemSendInfo)
		SINK_ENTRY_INFO(33, __uuidof(Outlook::InspectorsEvents),/*dispid*/ 0xf001, OnNewInspector, &OnNewInspectorInfo)
//j		SINK_ENTRY_INFO(44, __uuidof(Outlook::ApplicationEvents),/*dispid*/ 0xf003, OnAppNewMail, &OnAppNewMailInfo)
	END_SINK_MAP()

	BEGIN_MSG_MAP(CMsAddIn)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(UWM_HELP, OnHelp)
		MESSAGE_HANDLER(UWM_SHUTDOWNALL, OnShutdownAll)
		MESSAGE_HANDLER(UWM_UPDATERESOURCE, OnUpdateResource)
		MESSAGE_HANDLER(UWM_REMOVEADDIN, OnRemoveAddin)
	END_MSG_MAP()

	// For scripting security
//	BEGIN_CATEGORY_MAP(CMsAddIn)
//		IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
//		IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
//	END_CATEGORY_MAP()

	// _IDTExtensibility2
	STDMETHODIMP OnConnection(IDispatch* Application, ext_ConnectMode ConnectMode, IDispatch* AddInInst, SAFEARRAY** custom);
	STDMETHODIMP OnDisconnection(ext_DisconnectMode RemoveMode, SAFEARRAY** custom);
	STDMETHODIMP OnAddInsUpdate(SAFEARRAY** custom);
	STDMETHODIMP OnStartupComplete(SAFEARRAY** custom);
	STDMETHODIMP OnBeginShutdown(SAFEARRAY** custom);

	// IMsAddIn
	STDMETHOD_(void, OnButtonClick)(Office::_CommandBarButton* Ctrl, VARIANT_BOOL* CancelDefault);
	STDMETHOD_(void, OnAppItemSend)(IDispatch* pDispatch,VARIANT_BOOL* CancelDefault);
	STDMETHOD_(void, OnNewInspector)(_Inspector* Inspector);
//j	STDMETHOD_(void, OnAppNewMail)();

	STDMETHOD(Download)(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice, VARIANT varExternalLink);
	STDMETHOD(Download2)(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath,  VARIANT varPathPrice);
	STDMETHOD(ExecuteCommand)(VARIANT varCmdId);


	static void CALLBACK MyItemClose(LPARAM lParam1, LPARAM lParam2);
	bool ToolbarCreate(Office::_CommandBarsPtr& pCommandBars, LPCSTR pName, Outlook::_InspectorPtr pInspector);
	CToolbar* ToolbarFind(void* pInspector);
	Office::_CommandBarButtonPtr ToolbarCreateButton(Office::CommandBarControlsPtr& pCommandBar, int index);
	void ToolbarDestroy(void* pInspector);
	void PopupMenu(HWND hWndParent, IDispatch* pDispatch, int x, int y, LPCSTR pstrURL);
	void PopupDialog(HWND hWndParent, IDispatch* pDispatch, LPCSTR pstrURL);
	void Popup(int iType, Office::_CommandBarButton* Ctrl, LPCSTR pQueryString);
	bool LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject);
	bool LaunchFeedbackMail(DWORD dwCmd);
	bool EmbedHTMLAtEnd(const CString& strHTMLText);
	bool EmbedHTMLAtSelection(const CString& strHTML);
	bool ShowExplorerToolbars(bool bShow);
	bool ShowInspectorToolbars(bool bShow);
	IHTMLDocument2Ptr GetHTMLDocument2(Outlook::_InspectorPtr pInspector, bool bConvert);
	bool IsInspectorInEditMode(Outlook::_InspectorPtr pInspector);
	Outlook::_ExplorerPtr GetActiveExplorer();
	Outlook::_InspectorPtr GetActiveInspector();
	Outlook::_MailItemPtr GetMailItem(Outlook::_InspectorPtr pInspector);

	HRESULT FinalConstruct()
	{
		RECT rect;
        rect.left = rect.right = rect.top = rect.bottom = 0;

        // Create a hidden window 
        HWND hwnd = Create (NULL, rect, "MSAddinHiddenWindow", WS_POPUP);
		if (hwnd == NULL)
		{
			::MessageBox(NULL, "Create failed in MSAddin - Final Construct", "MSAddin", IDOK);
			return S_OK;
		}

		// Save reference to window handle
		m_hClientWnd = hwnd;

		// Register this mail client with JMSrvr.
		ServerRegister(hwnd, CLIENT_TYPE_OUTLOOK);
		
		return S_OK;
	}

	void FinalRelease()
	{
		if (!m_bSrvrShutdown)
			ServerUnregister(m_hWnd);
	}
	
	// ISupportsErrorInfo
	STDMETHODIMP InterfaceSupportsErrorInfo(REFIID riid)
	{
		static const IID* arr[] = 
		{
			&IID_IMsAddIn
		};
		for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i],riid))
				return S_OK;
		}
		return S_FALSE;
	}

private:
	vector<CToolbar*> m_ToolbarList;
	Outlook::_ApplicationPtr m_pOutlook;
	Outlook::_InspectorsPtr m_pInspectors;
	bool m_bConnected;
	DWORD m_dwButtonEventCookie;
	CString m_strHTMLText;
	POINT m_PointLastClick;
	DWORD m_dwTimeLastClick;

public:
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnShutdownAll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUpdateResource(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnRemoveAddin(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);


};
