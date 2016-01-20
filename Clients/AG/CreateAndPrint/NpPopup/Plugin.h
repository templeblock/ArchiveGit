#pragma once

#include "resource.h"
#include "Download.h"
#include "NpApi.h"

class CPlugin : public CWindowImpl<CPlugin>
{
public:
	BEGIN_MSG_MAP(CPlugin)
		DEFAULT_REFLECTION_HANDLER()
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

public:
	CPlugin();
	~CPlugin();
	static bool CALLBACK MySrcDownloadCallback(CPlugin* pPlugin, void* pDownloadVoid);
	CDownload& GetDownload()
		{ return m_Download; }

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//j	HRESULT OnDraw(ATL_DRAWINFO& di);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT SrcDownload(bool bStart);

	HRESULT GetSrc(BSTR* bstrSrcURL);
	HRESULT PutSrc(BSTR bstrSrcURL);
	HRESULT GetURL(BSTR* bstrURL);
	HRESULT PutURL(BSTR bstrURL);
	HRESULT GetContext(BSTR* bstrContext);
	HRESULT PutContext(BSTR bstrContext);

	CString GetOption(LPCSTR pNameTarget);
	int GetOptionInt(LPCSTR pNameTarget, int iDefaultValue);
	bool PaintImage(HDC hPaintDC);
	void SetNPPInstance(NPP pInstance);

protected:
	void Free();

protected:
	CComPtr<IWebBrowser2> m_pBrowser;
	CComBSTR m_bstrSrc;
	CComBSTR m_bstrURL;
	CComBSTR m_bstrContext;
	CDownload m_Download;
	HGLOBAL m_hImageMemory;
	RECT m_HotRect;

public:
	NPP m_pNPPInstance;
};
