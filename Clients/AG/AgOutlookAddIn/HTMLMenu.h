// HTMLMenu.h : Declaration of the CHTMLMenu
#pragma once
#include "resource.h"
#include "RegKeys.h"
#include "SmartCache.h"

class CHTMLMenu : 
	public CDialogImpl<CHTMLMenu>
{
public:
	CHTMLMenu()
	{
		m_hWndApp = NULL;
		m_pDispatch = NULL;
	}

	~CHTMLMenu()
	{
	}

	enum { IDD = IDD_HTMLMENU };

	BEGIN_MSG_MAP(CHTMLMenu)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_NCACTIVATE, OnNCActivate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()

	void Popup(HWND hWndParent, IDispatch* pDispatch, int x, int y, LPCSTR pstrURL)
	{
		CString strURL = GetURL();
		Close();

		if (strURL == CString(pstrURL))
			return;

		SetDispatch(pDispatch);
		SetURL(pstrURL);
		Create(hWndParent);
		SetWindowPos(HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE);
		ShowWindow(SW_SHOW);
	}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_hWndApp = ::GetActiveWindow();

		// Set the IE registry settings that control caching
		// Never check for new conent unless it doesn;t exist in the cache
		m_SmartCache.SetSyncMode();

		RECT rect;
		::GetClientRect(m_hWnd, &rect);

		// Create the web browser control
		CAxWindow wnd;
		wnd.Create(m_hWnd, rect, m_strURL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0/*WS_EX_CLIENTEDGE*/);
		HRESULT hr = wnd.SetExternalDispatch(m_pDispatch);
		if (SUCCEEDED(hr))
			hr = wnd.QueryControl(&m_spBrowser);

		return 1;  // Let the system set the focus
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DestroyWindow();
		m_hWnd = NULL;
		m_pDispatch = NULL;
		m_strURL = "";
		return S_OK;
	}

	void Close()
	{
		if (m_hWnd && ::IsWindow(m_hWnd))
			SendMessage(WM_CLOSE);
	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNCActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL fActive = (BOOL)wParam;

		if (!fActive)
		{
			m_SmartCache.RestoreSyncMode();

			Close();
			::SetActiveWindow(m_hWndApp);
		}

		return 0;
	}

	void SetDispatch(IDispatch* pDispatch)
	{
		m_pDispatch = pDispatch;
	}

	void SetURL(LPCSTR pstrURL)
	{
		m_strURL = pstrURL;
	}

	CString GetURL()
	{
		return m_strURL;
	}

private:
	HWND m_hWndApp;
	CComPtr<IDispatch> m_pDispatch;
	IWebBrowser2Ptr m_spBrowser;
	CString m_strURL;
	CSmartCache m_SmartCache;
};
