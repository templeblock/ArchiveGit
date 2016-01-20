// Ctp.h : Declaration of the CCtp

#ifndef __CTP_H_
#define __CTP_H_

#include "resource.h"
#include "CtlPanel.h"
#include "AGDoc.h"
#include "AGSym.h"
#include "AGDC.h"
#include "Font.h"

#ifdef NETSCAPE
	#include "NpApi.h"
#else
	#include "..\AxCtp\AxCtp.h"
	#include "..\AxCtp\Bsc2.h"
#endif NETSCAPE

#ifdef NETSCAPE

class CCtp : public CWindowImpl<CCtp>
{
public:

#else

#define dwSAFETY_OPTIONS	INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA

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
	DECLARE_REGISTRY_RESOURCEID(IDR_CTP)

	BEGIN_COM_MAP(CCtp)
		COM_INTERFACE_ENTRY(ICtp)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IViewObjectEx)
		COM_INTERFACE_ENTRY_IID(IID_IViewObject2, IViewObjectEx)
		COM_INTERFACE_ENTRY_IID(IID_IViewObject, IViewObjectEx)
		COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY_IID(IID_IOleInPlaceObject, IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY_IID(IID_IOleWindow, IOleInPlaceObjectWindowless)
		COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
		COM_INTERFACE_ENTRY(IOleControl)
		COM_INTERFACE_ENTRY(IOleObject)
		COM_INTERFACE_ENTRY(IPersistStreamInit)
		COM_INTERFACE_ENTRY_IID(IID_IPersist, IPersistPropertyBag)
		COM_INTERFACE_ENTRY(IPersistPropertyBag)
		COM_INTERFACE_ENTRY(IObjectSafety)
	END_COM_MAP()

	BEGIN_PROPERTY_MAP(CCtp)
		PROP_ENTRY("Fonts", 0, CLSID_NULL)
		PROP_ENTRY("Src", 1, CLSID_NULL)
	END_PROPERTY_MAP()
#endif NETSCAPE

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
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

public:
	CCtp();
	void CreateBackPage();
	void DrawEditRect(CAGDC* pDC);
	void FileData(BYTE* pBytes, DWORD dwLen);
	void FileEnd();
	void FileStart();
	void FontData(const char* pszFontFile, BYTE* pBytes, DWORD dwLen);
	void FontEnd(const char* pszFontFile);
	void FontStart(const char* pszFontFile);

	CFontList& GetFontList() { return (m_FontList); }
	CAGSymImage* GetImage(int nID);
	CAGText* GetText()
		{ return (m_pText); }
	bool HasFocus()
		{ return (m_bHasFocus); }
	void NewPage();
	void StartEdit(CAGSymText* pText, POINT Pt, bool bClick);
	void StopEdit();

	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT OnDraw(ATL_DRAWINFO& di);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
//j#ifndef NETSCAPE
		HDC hDC = (HDC)wParam;
		HWND hParent = GetParent();
//j		::SendMessage(hParent, WM_ERASEBKGND, (WPARAM)hDC, 0);

		POINT pt;
		pt.x = 0;
		pt.y = 0;
		MapWindowPoints(hParent, &pt, 1);
		OffsetWindowOrgEx(hDC, pt.x, pt.y, &pt);
		SetWindowOrgEx(hDC, pt.x, pt.y, NULL);
//j#endif NETSCAPE

		if (hDC)
		{
			RECT rClient;
			GetClientRect(&rClient);
			FillRect(hDC, &rClient, (HBRUSH)::GetStockObject(WHITE_BRUSH));
			//j HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
			//j FillRect(hDC, &rClient, hBrush);
			//j ::DeleteObject(hBrush);
		}

		bHandled = true;
		return S_OK;
	}

	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
#ifndef NETSCAPE
		InPlaceActivate(OLEIVERB_UIACTIVATE);
#endif NETSCAPE
		return 0;
	}

	void StartDownloadFont(const char* pszFontName)
	{
#ifdef NETSCAPE
		char szFontURL[_MAX_PATH];
		lstrcpy(szFontURL, m_szFontURL);
		lstrcat(szFontURL, pszFontName);

		NPN_GetURL(m_pNPPInstance, szFontURL, NULL);
#else
		USES_CONVERSION;
		char szFontURL[_MAX_PATH];
		LPSTR string = W2A(m_bstrFonts);
		lstrcpy(szFontURL, string);
		lstrcat(szFontURL, pszFontName);
		if (string && lstrlen(string) > 0)
		{
			bool bRelativeURL = false;
			if (!strchr(string, ':'))
				bRelativeURL = true;

			m_dwDownloadMaxSize = 0;
			m_dwFontDownloadSize = 0;
			FontStart(pszFontName);
			lstrcpy(m_szFontDownload, pszFontName);

			CComBSTR bstr = szFontURL;
			CBindStatusCallback2<CCtp>::Download(this, OnFontData, bstr, m_spClientSite, bRelativeURL);
		}
#endif NETSCAPE
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		ATL_DRAWINFO di;
		PAINTSTRUCT ps;
		BeginPaint(&ps);
		di.hdcDraw = ps.hdc;
		RECT ClientRect;
		GetClientRect(&ClientRect);
		di.prcBounds = (const RECTL*)&ClientRect;
		SaveDC(di.hdcDraw);
		OnDraw(di);
		RestoreDC(di.hdcDraw, -1);
		EndPaint(&ps);

		bHandled = true;
		return true;
	}

#ifdef NETSCAPE
	BOOL GetAmbientDisplayName(BSTR&)
		{ return false; }

	void GetAmbientUserMode(BOOL &bUserMode)
		{ bUserMode = true; }

	void SetFontURL(const char* pszFontURL)
		{ lstrcpy(m_szFontURL, pszFontURL); }

	void SetNPPInstance(NPP pInstance)
		{ m_pNPPInstance = pInstance; }

#else
	void OnData(CBindStatusCallback<CCtp>*  pbsc, BYTE* pBytes, DWORD dwSize)
	{
		FileData(pBytes, dwSize);

		if (m_dwDownloadMaxSize > 0 && m_dwDownloadMaxSize == m_dwDownloadSize)
			FileEnd();
	}

	void OnFontData(CBindStatusCallback<CCtp>* pbsc, BYTE* pBytes, DWORD dwSize)
	{
		FontData(m_szFontDownload, pBytes, dwSize);
		m_dwFontDownloadSize += dwSize;

		if (m_dwDownloadMaxSize > 0 && m_dwDownloadMaxSize == m_dwFontDownloadSize)
			FontEnd(m_szFontDownload);
	}

	STDMETHOD(get_Src)(BSTR* pstrSrc)
	{
		*pstrSrc = m_bstrSrc.Copy();
		return S_OK;
	}

	STDMETHOD(put_Src)(BSTR strSrc)
	{
		USES_CONVERSION;
		m_bstrSrc = strSrc;
		LPSTR string = W2A(m_bstrSrc);

		if (string && lstrlen(string) > 0)
		{
			bool bRelativeURL = false;
			if (!strchr(string, ':'))
				bRelativeURL = true;

			m_dwDownloadMaxSize = 0;
			FileStart();

			CBindStatusCallback2<CCtp>::Download(this, OnData, m_bstrSrc, m_spClientSite, bRelativeURL);
		}
		return S_OK;
	}

	STDMETHOD(get_Fonts)(BSTR* pstrFonts)
	{
		*pstrFonts = m_bstrFonts.Copy();
		return S_OK;
	}

	STDMETHOD(put_Fonts)(BSTR strFonts)
	{
		m_bstrFonts = strFonts;
		return S_OK;
	}

// IViewObjectEx
	STDMETHOD(GetViewStatus)(DWORD* pdwStatus)
	{
		ATLTRACE(_T("IViewObjectExImpl::GetViewStatus\n"));
		*pdwStatus = 0;
		return S_OK;
	}

// IObjectSafety
	STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD* pdwSupportedOptions, DWORD* pdwEnabledOptions)
	{
		if (!pdwSupportedOptions || !pdwEnabledOptions)
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

	STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
	{
		return S_OK;
	}

// IBindStatusCallback
	STDMETHOD(OnProgress)(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCWSTR szStatusText)
	{
		m_dwDownloadMaxSize = ulProgressMax;

		return S_OK;
	}
#endif NETSCAPE

protected:
	CCtlPanel* m_pCtlPanel;
	CAGClientDC* m_pClientDC;
	CAGDoc* m_pAGDoc;
	CAGSymText* m_pText;
	RECT m_PageRect;
	RECT m_ShadowRect;
	CAGMatrix m_ViewMatrix;
	bool m_bHasFocus;
	HBITMAP m_hBitmap;
	CFontList m_FontList;
	DWORD m_dwDownloadSize;
	DWORD m_dwDownloadMaxSize;
	BYTE* m_pDownloadData;
	FONTDOWNLOADARRAY m_FontDownloadArray;

#ifdef NETSCAPE	
	char m_szFontURL[_MAX_PATH];
	NPP m_pNPPInstance;
#else
	CComBSTR m_bstrFonts;
	CComBSTR m_bstrSrc;
	char m_szFontDownload[_MAX_FNAME];
	DWORD m_dwFontDownloadSize;
#endif NETSCAPE	
};

#endif __CTP_H_
