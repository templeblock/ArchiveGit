#ifndef __CTP_H_
#define __CTP_H_

#include "AxCtp.h"
#include "resource.h"
#include "CtlPanel.h"
#include "AGDoc.h"
#include "AGSym.h"
#include "AGDC.h"
#include "Bsc2.h"
#include "Font.h"


#define	dwSAFETY_OPTIONS	INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA


/////////////////////////////////////////////////////////////////////////////
// CCtp
//
class ATL_NO_VTABLE CCtp : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCtp, &CLSID_Ctp>,
	public CComControl<CCtp>,
	public IDispatchImpl<ICtp, &IID_ICtp, &LIBID_AXCTPLib>,
	public IPersistStreamInitImpl<CCtp>,
	public IOleControlImpl<CCtp>,
	public IOleObjectImpl<CCtp>,
	public IOleInPlaceActiveObjectImpl<CCtp>,
	public IViewObjectExImpl<CCtp>,
	public IOleInPlaceObjectWindowlessImpl<CCtp>,
	public IPersistPropertyBagImpl<CCtp>,
	public IObjectSafetyImpl<CCtp, dwSAFETY_OPTIONS>
{
public:
	CCtp()
	{
		m_bWindowOnly = TRUE;
		m_pCtlPanel = NULL;
		m_pClientDC = NULL;
		m_pAGDoc = NULL;
		m_pText = NULL;
		SetRect(&m_PageRect, 0, 0, 0, 0);
		SetRect(&m_ShadowRect, 0, 0, 0, 0);
		m_pDownloadData = NULL;
		m_dwDownloadSize = 0;
		m_dwDownloadMaxSize = 0;
		m_hBitmap = NULL;
		m_bHasFocus = false;
	}


DECLARE_REGISTRY_RESOURCEID(IDR_CTP)

BEGIN_COM_MAP(CCtp)
	COM_INTERFACE_ENTRY(ICtp)
	COM_INTERFACE_ENTRY(IDispatch)
//	COM_INTERFACE_ENTRY_IMPL(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObjectEx)
//	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject2, IViewObjectEx)
	COM_INTERFACE_ENTRY_IID(IID_IViewObject2, IViewObjectEx)
//	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject, IViewObjectEx)
	COM_INTERFACE_ENTRY_IID(IID_IViewObject, IViewObjectEx)
//	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
//	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleInPlaceObject, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IID(IID_IOleInPlaceObject, IOleInPlaceObjectWindowless)
//	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IID(IID_IOleWindow, IOleInPlaceObjectWindowless)
//	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
//	COM_INTERFACE_ENTRY_IMPL(IOleControl)
	COM_INTERFACE_ENTRY(IOleControl)
//	COM_INTERFACE_ENTRY_IMPL(IOleObject)
	COM_INTERFACE_ENTRY(IOleObject)
//	COM_INTERFACE_ENTRY_IMPL(IPersistStreamInit)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
//	COM_INTERFACE_ENTRY_IMPL_IID(IID_IPersist, IPersistPropertyBag)
	COM_INTERFACE_ENTRY_IID(IID_IPersist, IPersistPropertyBag)
//	COM_INTERFACE_ENTRY_IMPL(IPersistPropertyBag)
	COM_INTERFACE_ENTRY(IPersistPropertyBag)
//	COM_INTERFACE_ENTRY_IMPL(IObjectSafety)
	COM_INTERFACE_ENTRY(IObjectSafety)
END_COM_MAP()

BEGIN_PROPERTY_MAP(CCtp)
	PROP_ENTRY ("Fonts", 0, CLSID_NULL)
	PROP_ENTRY ("Src", 1, CLSID_NULL)
END_PROPERTY_MAP()


BEGIN_MSG_MAP(CCtp)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_CHAR, OnChar)
	MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
	MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
	MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
END_MSG_MAP()


// IViewObjectEx
	STDMETHOD(GetViewStatus)(DWORD *pdwStatus)
	{
		ATLTRACE(_T("IViewObjectExImpl::GetViewStatus\n"));
		*pdwStatus = 0;
		return S_OK;
	}

// IObjectSafety
	STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD *pdwSupportedOptions, DWORD *pdwEnabledOptions)
	{
		if (pdwSupportedOptions == NULL || pdwEnabledOptions == NULL)
			return E_POINTER;
		HRESULT hr = S_OK;
		if (riid == IID_IDispatch)
		{
			*pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
			*pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
		}
		else
		{
			*pdwSupportedOptions = 0;
			*pdwEnabledOptions = 0;
			hr = E_NOINTERFACE;
		}
		return hr;
	}
	STDMETHOD(SetInterfaceSafetyOptions)(REFIID /*riid*/, DWORD /*dwOptionSetMask*/, DWORD /*dwEnabledOptions*/)
	{
		return S_OK;
	}

// IBindStatusCallback
	STDMETHOD(OnProgress)(ULONG /*ulProgress*/, ULONG ulProgressMax, ULONG /*ulStatusCode*/, LPCWSTR /*szStatusText*/)
	{
		m_dwDownloadMaxSize = ulProgressMax;

		return S_OK;
	}

// ICtp
public:
	LRESULT OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);

	void OnData(CBindStatusCallback<CCtp> * /*pbsc*/, BYTE *pBytes, DWORD dwSize)
	{
		FileData(pBytes, dwSize);

		if (m_dwDownloadMaxSize > 0 && m_dwDownloadMaxSize == m_dwDownloadSize)
			FileEnd();
	}

	void OnFontData(CBindStatusCallback<CCtp> *pbsc, BYTE *pBytes, DWORD dwSize)
	{
		FontData(m_szFontDownload, pBytes, dwSize);
		m_dwFontDownloadSize += dwSize;

		if (m_dwDownloadMaxSize > 0 && m_dwDownloadMaxSize == m_dwFontDownloadSize)
			FontEnd(m_szFontDownload);
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	HRESULT OnDraw(ATL_DRAWINFO &di);

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL &bHandled)
	{
		HDC hDC = (HDC) wParam;
		HWND hParent = GetParent();

		POINT pt;
		pt.x = 0;
		pt.y = 0;
		MapWindowPoints (hParent, &pt, 1);
		OffsetWindowOrgEx (hDC, pt.x, pt.y, &pt);
  
		::SendMessage(hParent, WM_ERASEBKGND, (WPARAM) hDC, 0);
		SetWindowOrgEx(hDC, pt.x, pt.y, NULL);
		bHandled = TRUE;
		return TRUE;
	}

	LRESULT OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
	{
		InPlaceActivate(OLEIVERB_UIACTIVATE);
		return 0;
	}

	LRESULT OnKeyDown (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnKeyUp (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnLButtonDblClk (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnLButtonDown (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnLButtonUp (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnMouseMove (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnKillFocus (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnSetFocus (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);
	LRESULT OnTimer (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/);



	STDMETHOD(get_Src)(BSTR *pstrSrc)
	{
		*pstrSrc = m_bstrSrc.Copy();
		return S_OK;
	}

	STDMETHOD(put_Src)(BSTR strSrc)
	{
		USES_CONVERSION;
		m_bstrSrc = strSrc;
		LPSTR string = W2A (m_bstrSrc);

		if (string != NULL && lstrlen (string) > 0)
		{
			bool bRelativeURL = false;
			if (strchr (string, ':') == NULL)
				bRelativeURL = true;

			m_dwDownloadMaxSize = 0;
			FileStart();

			CBindStatusCallback2<CCtp>::Download(this, OnData, m_bstrSrc,
			  m_spClientSite, bRelativeURL);
		}
		return S_OK;
	}

	STDMETHOD(get_Fonts)(BSTR *pstrFonts)
	{
		*pstrFonts = m_bstrFonts.Copy();
		return S_OK;
	}

	STDMETHOD(put_Fonts)(BSTR strFonts)
	{
		m_bstrFonts = strFonts;
		return S_OK;
	}

	void CreateBackPage();
	void DrawEditRect(CAGDC *pDC);
	void FileData(BYTE *pBytes, DWORD dwLen);
	void FileEnd();
	void FileStart();
	void FontData(const char *pszFontFile, BYTE *pBytes, DWORD dwLen);
	void FontEnd(const char *pszFontFile);
	void FontStart(const char *pszFontFile);
	CFontList &GetFontList()					{ return (m_FontList); }
	CAGSymImage *GetImage(int nID);
	CAGText *GetText()							{ return (m_pText); }
	bool HasFocus()								{ return (m_bHasFocus); }
	void NewPage();

	void StartDownloadFont(const char *pszFontName)
	{
		USES_CONVERSION;
		char szFontURL[_MAX_PATH];
		LPSTR string = W2A (m_bstrFonts);
		lstrcpy (szFontURL, string);
		lstrcat (szFontURL, pszFontName);
		if (string != NULL && lstrlen (string) > 0)
		{
			bool bRelativeURL = false;
			if (strchr (string, ':') == NULL)
				bRelativeURL = true;

			m_dwDownloadMaxSize = 0;
			m_dwFontDownloadSize = 0;
			FontStart(pszFontName);
			lstrcpy (m_szFontDownload, pszFontName);

			CComBSTR bstr = szFontURL;
			CBindStatusCallback2<CCtp>::Download(this, OnFontData, bstr,
			  m_spClientSite, bRelativeURL);
		}
	}
	void StartEdit(CAGSymText *pText, POINT Pt, bool bClick);
	void StopEdit();

protected:
	CCtlPanel			*m_pCtlPanel;
	CAGClientDC			*m_pClientDC;
	CAGDoc				*m_pAGDoc;
	CAGSymText			*m_pText;
	RECT				m_PageRect;
	RECT				m_ShadowRect;
	CAGMatrix			m_ViewMatrix;
	CComBSTR			m_bstrSrc;
	CComBSTR			m_bstrFonts;
	BYTE				*m_pDownloadData;
	DWORD				m_dwDownloadSize;
	DWORD				m_dwDownloadMaxSize;
	HBITMAP				m_hBitmap;
	CFontList			m_FontList;
	FONTDOWNLOADARRAY	m_FontDownloadArray;
	char				m_szFontDownload[_MAX_FNAME];
	DWORD				m_dwFontDownloadSize;
	bool				m_bHasFocus;
};

#endif //__CTP_H_
