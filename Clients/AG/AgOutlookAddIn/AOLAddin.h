// AOLAddin.h : Declaration of the CAOLAddin
#pragma once
#include "resource.h"       // main symbols
#include "AgOutlookAddIn.h"
#include "HTMLMenu.h"
#include "HTMLDialog.h"
#include "MsAddInBase.h"
#include "FlashManager.h"
#include "JMHook.h"
#include "HelperFunctions.h"

extern HWND g_hAOLAddin;
extern CComPtr<IAOLAddin> g_spAOLAddin;


static UINT UWM_AOLCOMPOSE	= ::RegisterWindowMessage(UWM_AOLCOMPOSE_MSG);
#define UWM_AOLSMILEY_MSG _T("UWM_AOLSMILEY_MSG-94A97052-249C-4174-8312-7BC4B869A24B")
static UINT UWM_AOLSMILEY	= ::RegisterWindowMessage(UWM_AOLSMILEY_MSG);


static const TCHAR AOL_COMPOSE_TITLE[]	    = _T("Write Mail");
static const TCHAR AOL_REPLYCOMPOSE_TITLE[]	= _T("Re:");
static const TCHAR AOL_FWDCOMPOSE_TITLE[]	= _T("Fwd:");
static const TCHAR AOL_SMILEY_TITLE[]		= _T("Insert Smiley");
static const TCHAR AOL_SENDNOW_TITLE[]		= _T("Send Now");

static const TCHAR AOL_IEPARENT_CLASS[]		= _T("ATL-69F3D1D8");
static const TCHAR AOL_IESERVER_CLASS[]		= _T("Internet Explorer_Server");
static const TCHAR AOL_SMILEY_CLASS[]		= _T("_AOL_Icon");

// CAOLAddin
class ATL_NO_VTABLE CAOLAddin : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAOLAddin, &CLSID_AOLAddin>,
	public CComControl<CAOLAddin>,
	public IDispatchImpl<IAOLAddin, &IID_IAOLAddin, &LIBID_AgOutlookAddInLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public ISupportErrorInfo,
    public CMsAddInBase
{
public:

    CAOLAddin();
    ~CAOLAddin();


DECLARE_REGISTRY_RESOURCEID(IDR_AOLADDIN)

DECLARE_NOT_AGGREGATABLE(CAOLAddin)

BEGIN_COM_MAP(CAOLAddin)
	COM_INTERFACE_ENTRY(IAOLAddin)
	COM_INTERFACE_ENTRY2(IDispatch, IAOLAddin)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()



BEGIN_MSG_MAP(CAOLAddin)
	MESSAGE_HANDLER(UWM_AOLCOMPOSE, OnIEServer)
	MESSAGE_HANDLER(UWM_SHUTDOWN, OnForceShutdown)
	MESSAGE_HANDLER(UWM_LAUNCH, OnLaunch)
	MESSAGE_HANDLER(UWM_UPDATESIZE, OnUpdateSize)
	MESSAGE_HANDLER(UWM_CMACTIVATE, OnCMActivate)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(UWM_HELP, OnHelp)
	MESSAGE_HANDLER(UWM_REMOVEADDIN, OnRemoveAddin)
	MESSAGE_HANDLER(UWM_SHUTDOWNALL, OnShutdownAll)
	MESSAGE_HANDLER(UWM_UPDATERESOURCE, OnUpdateResource)
	MESSAGE_HANDLER(UWM_AOLSMILEY, OnAOLSmiley)
	MESSAGE_HANDLER(UWM_MAILSEND, OnMailSend)
	CHAIN_MSG_MAP(CComControl<CAOLAddin>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] = 
		{
			&IID_IAOLAddin,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IAOLAddin
public:
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
		LPCTSTR pszText = _T("ATL 7.0 : AOLAddin");
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
        HWND hwnd = Create (NULL, rect, "AOLAddinHiddenWindow", WS_POPUP);
		if (hwnd == NULL)
		{
			::MessageBox(NULL, "Create failed in AOLAddin - Final Construct"," AOLAddin", IDOK);
			return S_OK;
		}
		 
		// Save reference to window handle
		m_hClientWnd = hwnd;

		// Register this mail client with JMSrvr.
		ServerRegister(hwnd, CLIENT_TYPE_AOL);

        // Initialize global OEAddin.
		if (NULL == g_spAOLAddin.p)
		{
			g_spAOLAddin.Attach(this);
			g_spAOLAddin.p->AddRef();
			g_hAOLAddin = m_hWnd;
		}


		// Verify user status.
		VerifyUserStatus();


		return S_OK;
	}
	
	void FinalRelease() 
	{
		/*if (!m_bSrvrShutdown)
			ServerUnregister(m_hWnd);*/
			
		CleanUp();
	}

protected:
	void CleanUp();
	void Popup(int iType, int iButtonIndex, HWND hwndRebar, LPCTSTR pQueryString, bool bActivate=true);
	void UpdateComposeWindowHandle();
	bool LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject);
	bool LaunchFeedbackMail(DWORD dwCmd);
	bool EmbedHTMLAtEnd(const CString& strHTMLText);
	bool EmbedHTMLAtSelection(const CString& strHTML);
	bool GetTitle(HWND hwndCompose, CString &szSubject);
	bool SetSubject(HWND hwndCompose, CString &szSubject);
	bool IsComposeWindow(HWND hwndCompose);
    bool FindComposeWindow(HWND hwndIEServer, HWND& hwndCompose);
    bool LoadPage(int nCat=-1, bool bActivate=true);
	bool SubclassAOLCompose();


public:

	LRESULT OnIEServer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnForceShutdown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnRemoveAddin(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnShutdownAll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUpdateResource(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLaunch(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUpdateSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnCMActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnAOLSmiley(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnMailSend(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	
	STDMETHOD(Download2)(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice);
	STDMETHOD(GetWindowHandle)(OLE_HANDLE* hWnd);
	STDMETHOD(SetActiveCompose)(OLE_HANDLE hwndCompose, UINT State);
	STDMETHOD(OnItemSend)();
	STDMETHOD(OnShutDown)();
	STDMETHOD(ExecuteCommand)(VARIANT varCmdId);
	STDMETHOD(SetTopParent)(OLE_HANDLE hwnd);

	virtual void HandleShutdown();
	virtual int DisplayMessage(DWORD dwMsgID, UINT uType=MB_OK | MB_HELP | MB_TOPMOST);


protected:
	HWND m_hActiveCompose;
	HWND m_hActiveIE;
	bool m_bWaitingForCompose;
	static CString m_strHTMLText;
	
public:
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

};

OBJECT_ENTRY_AUTO(__uuidof(AOLAddin), CAOLAddin)
