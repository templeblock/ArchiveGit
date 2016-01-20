// JMExplorerBand.h : Declaration of the CJMExplorerBand

#pragma once
#include "resource.h"       // main symbols
#include "AgOutlookAddIn.h"
#include <shlobj.h>
#include "MsAddInBase.h"
#include "webmailbase.h"
#include "utilitywindow.h"


extern HINSTANCE  g_hInst;
extern UINT       g_DllRefCount;

#define JMEB_CLASS_NAME _T("CJMExplorerBandClass")
#define MIN_SIZE_X   10
#define MIN_SIZE_Y   10

static LPCTSTR CLSID_JMEXPLORERBAND_STRING = _T("{F8B5C2ED-0329-413A-AEE6-4134E93C3E38}");
static LPCTSTR CLSID_JMEXPLORERBUTTON_STRING = _T("{855159E3-55D5-4a9b-BFC3-0813D7C8E141}");

// CJMExplorerBand

class ATL_NO_VTABLE CJMExplorerBand : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CJMExplorerBand, &CLSID_JMExplorerBand>,
	public CComControl<CJMExplorerBand>,
	public IDeskBand,
	public IPersistStream,
	public IInputObject,
	public IObjectWithSiteImpl<CJMExplorerBand>,
	public IDispatchImpl<IJMExplorerBand, &IID_IJMExplorerBand, &LIBID_AgOutlookAddInLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public CMsAddInBase

{
public:
	CJMExplorerBand();
	virtual ~CJMExplorerBand();


DECLARE_REGISTRY_RESOURCEID(IDR_JMEXPLORERBAND)

//DECLARE_NOT_AGGREGATABLE(CJMExplorerBand)

BEGIN_COM_MAP(CJMExplorerBand)
	COM_INTERFACE_ENTRY(IJMExplorerBand)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY_IID(IID_IDeskBand, IDeskBand)
	COM_INTERFACE_ENTRY(IPersist)
	COM_INTERFACE_ENTRY(IPersistStream)
	COM_INTERFACE_ENTRY_IID(IID_IInputObject, IInputObject)
	//COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY2(IDispatch, IJMExplorerBand)
END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	
	void FinalRelease() 
	{
	}

// IDeskBand
public:
	STDMETHOD(GetBandInfo)(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi);

// IObjectWithSite
public:
	STDMETHOD(SetSite)(IUnknown* pUnkSite);
	STDMETHOD(GetSite)(REFIID riid, void **ppvSite);

// IOleWindow
public:
	STDMETHOD(GetWindow)(HWND* phwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

// IDockingWindow
public:
	STDMETHOD(CloseDW)(unsigned long dwReserved);
	STDMETHOD(ResizeBorderDW)(const RECT* prcBorder, IUnknown* punkToolbarSite, BOOL fReserved);
	STDMETHOD(ShowDW)(BOOL fShow);

// IPersist
public:
	STDMETHOD(GetClassID)(CLSID *pClassID);

// IPersistStream
public:
	STDMETHOD(IsDirty)(void);
	STDMETHOD(Load)(IStream *pStm);
	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize);

// IInputObject methods
public:
	STDMETHOD (UIActivateIO) (BOOL, LPMSG);
	STDMETHOD (HasFocusIO) (void);
	STDMETHOD (TranslateAcceleratorIO) (LPMSG);

// IJMExplorerBand
public:
	STDMETHOD(Download)(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice, VARIANT varExternalLink);
	STDMETHOD(Download2)(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice);
	STDMETHOD(ExecuteCommand)(VARIANT varCmdId);

public:
	STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pvarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);

public:
	virtual int DisplayMessage(DWORD dwMsgID, UINT uType=MB_OK | MB_HELP | MB_TOPMOST);
	virtual void ShowHelp(DWORD dwHelpID=0);
	virtual bool HandleBrowserOptionsButton();
	bool OnHelp();
	static void CALLBACK UtilityCallback(LPARAM lParam1, LPARAM lParam2);

 //virtual functions that must be implemented.
protected:
	virtual bool LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject);
	virtual bool LaunchFeedbackMail(DWORD dwCmd);
	virtual bool EmbedHTMLAtSelection(const CString& strHTML);
	virtual bool EmbedHTMLAtEnd(const CString& strHTMLText);
	virtual bool DownloadComplete();
	//virtual void RefreshResource();
	

protected: 
	void SetURL(LPCSTR pstrURL, bool bNavigate);
	void ClientCleanup();
	bool Popup(int iType, int iButtonIndex, HWND hwndRebar, LPCTSTR pQueryString);
	bool LoadPage();
	CString GetURL();
	HRESULT Connect();
	HRESULT Disconnect();
	//bool GetIEFrameDoc(LPCTSTR szFrameId, IWebBrowser2 * pBrowser, IHTMLDocument2** pOutDoc, IWebBrowser2** pOutBrowser);
	bool IsHotmailCompose(IHTMLDocument2* pDoc);
	bool IsYahooCompose(IWebBrowser2 * pWB, IHTMLDocument2* pDoc, DWORD &dwVer);
	bool HandleNewDoc(IWebBrowser2 * pWB);
	bool HandleBeforeNavigate();
	bool IsMailClientValid(bool bShowMessage = false);
	bool SyncMailClient();

private:
	BOOL RegisterAndCreateWindow();
	void FocusChange(BOOL);
	LRESULT OnKillFocus(void);
	LRESULT OnSetFocus(void);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(LPARAM lParam);
	LRESULT OnPaint(void);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnShutdownAll(void);
	LRESULT OnUpdateResource();

	


private:
	CUtilityWindow m_UtilWnd;
	CWebMailBase *m_pMailClient;
	CMenu m_OptionsMenu;
	CString m_strURL;
	DWORD m_dwCookie;
	DWORD m_ClientID;
	DWORD m_dwBandID;
	DWORD m_dwViewMode;
	BOOL m_bShow;
	BOOL m_bFocus;
	BOOL m_bEnterHelpMode;
	HWND m_hWnd;
	HWND m_hwndParent;
    LPCSTR m_pszVerb;
    LPCTSTR m_pwszVerb;
	//CHTMLWnd m_HTMLWnd;
	CComPtr<IWebBrowser2> m_spWebBrowser;
	CAxWindow m_BrowserWnd;
	CComPtr<IDispatch> m_pDispatch;
	CComPtr<IWebBrowser2> m_spJMBrowser;
	CComPtr<IInputObjectSite> m_spSite;
	LPDISPATCH m_lpDocCompleteDisp;



    

    //CBandToolBarReflectorCtrl m_wndInvisibleChildWnd; // Used for Reflecting Messages

};

OBJECT_ENTRY_AUTO(__uuidof(JMExplorerBand), CJMExplorerBand)
