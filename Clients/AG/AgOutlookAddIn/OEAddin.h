// OEAddin.h : Declaration of the COEAddin
#pragma once
#include <map>
#include "resource.h"       // main symbols
#include "AgOutlookAddIn.h"
#include "HTMLMenu.h"
#include "HTMLDialog.h"
#include "MsAddInBase.h"
#include "FlashManager.h"
#include "JMHook.h"
#include "HelperFunctions.h"

#define UWM_REFRESH_MSG _T("UWM_REFRESH_MSG-33E531B1_14D3_11d5_A025_006067718D04")

extern CComPtr<IOEAddin> g_spOEAddin;
extern HWND g_hOEAddin;

static UINT UWM_OECREATE	= ::RegisterWindowMessage(UWM_OECREATE_MSG);
static UINT UWM_OECOMPOSE	= ::RegisterWindowMessage(UWM_OECOMPOSE_MSG);
static UINT UWM_TBREMOVE	= ::RegisterWindowMessage(UWM_TBREMOVE_MSG);
static UINT UWM_TBSHOW		= ::RegisterWindowMessage(UWM_TBSHOW_MSG);
static UINT UWM_TBDESTROY	= ::RegisterWindowMessage(UWM_TBDESTROY_MSG);
static UINT UWM_MOVEBAND	= ::RegisterWindowMessage(UWM_MOVEBAND_MSG);
static UINT UWM_REFRESH		= ::RegisterWindowMessage(UWM_REFRESH_MSG);

static const UINT TYPE_TOPPARENT    = 0x0000;
static const UINT TYPE_REBAR		= 0x0001;
static const UINT TYPE_TOOLBAR	    = 0x0002;

static const UINT OWNER_OEBROWSER	= 0x0001;
static const UINT OWNER_COMPOSE		= 0x0002;
static const UINT OWNER_IEFRAME		= 0x0003;

//static const TCHAR OE_BROWSER_CLASS[]			= _T("Outlook Express Browser Class");
//static const TCHAR OE_COMPOSEWINDOW_CLASS[]		= _T("ATH_Note");
static const TCHAR OE_SIZABLEREBARWND_CLASS[]	= _T("SizableRebar");
static const TCHAR OE_REBARWINDOW32_CLASS[]		= _T("ReBarWindow32");
static const TCHAR OE_IESERVER_CLASS[]			= _T("Internet Explorer_Server");
static const TCHAR OE_SYSTAB32_CLASS[]			= _T("SysTabControl32");
static const TCHAR IE_IEFRAME_CLASS[]			= _T("IEFrame");				// Top level IE window
static const TCHAR IE_SHELLDOCVIEW_CLASS[]		= _T("Shell DocObject View");	// Parent to IE's server
static const TCHAR IE_WORKERW_CLASS[]			= _T("WorkerW");				// Parent to IE's rebar


class CJMToolbar;
//*********************************************************
// COEAddin
//*********************************************************
struct OEInfoType
{
	LONG uStatus;
	HWND hTopParent;
	CJMToolbar* pToolbar; 	
}; 
typedef std::map <HWND, OEInfoType*> OEInfoMapType;
typedef  BOOL(* PFNCLEARHOOKHANDLE)();

class ATL_NO_VTABLE COEAddin : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COEAddin, &CLSID_OEAddin>,
	public CComControl<COEAddin>,
	public IDispatchImpl<IOEAddin, &IID_IOEAddin, &LIBID_AgOutlookAddInLib>,
	public ISupportErrorInfo,
	public CMsAddInBase
{
public:
	COEAddin();
	~COEAddin();

	DECLARE_REGISTRY_RESOURCEID(IDR_OEADDIN)

	DECLARE_NOT_AGGREGATABLE(COEAddin)

	BEGIN_COM_MAP(COEAddin)
		COM_INTERFACE_ENTRY(IOEAddin)
		COM_INTERFACE_ENTRY2(IDispatch, IOEAddin)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		
	END_COM_MAP()

	BEGIN_PROP_MAP(COEAddin)
		PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
		PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
		// Example entries
		// PROP_ENTRY("Property Description", dispid, clsid)
		// PROP_PAGE(CLSID_StockColorPage)
	END_PROP_MAP()

	BEGIN_MSG_MAP(COEAddin)
		MESSAGE_HANDLER(UWM_OECREATE, OnOERebar)
		MESSAGE_HANDLER(UWM_OECOMPOSE, OnIEServer)
		MESSAGE_HANDLER(UWM_MOVEBAND, OnMoveBand)
		MESSAGE_HANDLER(UWM_TBDESTROY, OnTBDestroy)
		MESSAGE_HANDLER(UWM_TBREMOVE, OnOERemoveToolbars)
		MESSAGE_HANDLER(UWM_SHUTDOWN, OnForceShutdown)
		MESSAGE_HANDLER(UWM_REFRESH, OnRefresh)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(UWM_HELP, OnHelp)
		MESSAGE_HANDLER(UWM_REMOVEADDIN, OnRemoveAddin)
		MESSAGE_HANDLER(UWM_SHUTDOWNALL, OnShutdownAll)
		MESSAGE_HANDLER(UWM_UPDATERESOURCE, OnUpdateResource)
		CHAIN_MSG_MAP(CComControl<COEAddin>)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	// For scripting security
//	BEGIN_CATEGORY_MAP(COEAddin)
//		IMPLEMENTED_CATEGORY(CATID_SafeForScripting)
//		IMPLEMENTED_CATEGORY(CATID_SafeForInitializing)
//	END_CATEGORY_MAP()

	// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] = 
		{
			&IID_IOEAddin,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IOEAddin
	HRESULT OnDraw(ATL_DRAWINFO& di)
	{
		RECT& rc = *(RECT*)di.prcBounds;
		// Set Clip region to the rectangle specified by di.prcBounds
		HRGN hRgnOld = NULL;
		if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
			hRgnOld = NULL;
		bool bSelectOldRgn = false;

		HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

		if (hRgnNew != NULL)
		{
			bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
		}

		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("ATL 7.0 : COEAddin");
		TextOut(di.hdcDraw, 
			(rc.left + rc.right) / 2, 
			(rc.top + rc.bottom) / 2, 
			pszText, 
			lstrlen(pszText));

		if (bSelectOldRgn)
			SelectClipRgn(di.hdcDraw, hRgnOld);

		return S_OK;
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		RECT rect;
        rect.left = rect.right = rect.top = rect.bottom = 0;

        // Create a hidden window 
        HWND hwnd = Create (NULL, rect, "OEAddinHiddenWindow", WS_POPUP);
		if (hwnd == NULL)
		{
			::MessageBox(NULL, "Create failed in OEAddin - Final Construct"," OEAddin", IDOK);
			return S_OK;
		}
		 
		// Save reference to window handle
		m_hClientWnd = hwnd;

		// Register this mail client with JMSrvr.
		ServerRegister(hwnd, CLIENT_TYPE_EXPRESS);

		// Initialize global OEAddin.
		if (NULL == g_spOEAddin.p)
		{
			g_spOEAddin.Attach(this);
			g_spOEAddin.p->AddRef();
			g_hOEAddin = m_hWnd;
		}

		// Verify user status.
		VerifyUserStatus();


		return S_OK;
	}
	
	void FinalRelease() 
	{
		if (!m_bSrvrShutdown)
			ServerUnregister(m_hWnd);
			
		CleanUp();
	}

protected:
	void CleanUp();
	void Popup(int iType, int iButtonIndex, HWND hwndRebar, LPCTSTR pQueryString);
	void EraseAll();
	void HideAll();
	BOOL RemoveEntry(HWND hwndRebar);
	bool LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject);
	bool LaunchFeedbackMail(DWORD dwCmd);
	bool EmbedHTMLAtEnd(const CString& strHTMLText);
	bool EmbedHTMLAtSelection(const CString& strHTML);
	bool GetTitle(HWND hwndCompose, CString &szSubject);
	bool SetSubject(HWND hwndCompose, CString &szSubject);
	bool IsComposeWindow(HWND hwndCompose);

public:
	BOOL CreateToolbar(HWND hwndParent, HWND hwndRebar, UINT OwnerClass);
	BOOL GetToolbarFromRebar(HWND hwndRebar, OEInfoType *pToolbarInfo);
	BOOL GetToolbarFromHandle(HWND hwndToolbar, OEInfoType * pToolbarInfo);
	BOOL GetToolbarFromTopParent(HWND hwndTopParent, OEInfoType * pToolbarInfo);
	UINT GetRebarTopParent(HWND hRebar, HWND &hTopParent);

	LRESULT OnOERebar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnIEServer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTBDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOERemoveToolbars(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnForceShutdown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMoveBand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRefresh(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHotmailCompose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRemoveAddin(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnShutdownAll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUpdateResource(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	STDMETHOD(OnOEButtonClick)(ULONG ulButton_ID, OLE_HANDLE hwndRebar, VARIANT_BOOL* bSuccess);
	STDMETHOD(Download)(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice, VARIANT varExternalLink);
	STDMETHOD(Download2)(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice);
	STDMETHOD(RemoveToolbar)(OLE_HANDLE hwndOwner, VARIANT_BOOL* bSuccess);
	STDMETHOD(GetToolbar)(OLE_HANDLE hwndRebar, LONG* ToolbarInfo, VARIANT_BOOL* bSuccess);
	STDMETHOD(GetWindowHandle)(OLE_HANDLE* hWnd);
	STDMETHOD(SetActiveCompose)(OLE_HANDLE hwndCompose, UINT State);
	STDMETHOD(OnItemSend)();
	STDMETHOD(OnShutDown)();
	STDMETHOD(ExecuteCommand)(VARIANT varCmdId);

protected:
	BOOL m_bRefreshTimerInUse;
	HWND m_hRefreshCompose;
	HWND m_hActiveCompose;
	HWND m_hActiveIE;
	bool m_bWaitingForCompose;
	static CString m_strHTMLText;
	OEInfoMapType m_mapOEInfo;
	
public:
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

OBJECT_ENTRY_AUTO(__uuidof(OEAddin), COEAddin)
