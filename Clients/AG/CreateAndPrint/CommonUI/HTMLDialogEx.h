// HTMLDialogEx.h : Declaration of the CHTMLDialog
#pragma once
#include "resource.h"
#include "DWebBrowserEventsImpl.h"
#include <atlframe.h>

typedef void (CALLBACK * PFNBROWSERCALLBACK)(HWND hwnd, UINT uMsgId, LPARAM lParam, LPCTSTR szValue);

class CHTMLDialogEx :	public CAxDialogImpl<CHTMLDialogEx>,
						public CDialogResize<CHTMLDialogEx>,
						public DWebBrowserEventsImpl
{
public:
	CHTMLDialogEx();
	virtual ~CHTMLDialogEx();
	CHTMLDialogEx(const CString& strURL, BOOL bRes, HWND hWnd);

	enum { IDD = IDD_HTMLDIALOG };

	BEGIN_MSG_MAP(CHTMLDialogEx)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		//REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CHTMLDialogEx>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CHTMLDialogEx)
		DLGRESIZE_CONTROL(IDC_EXPLORER1, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	void TitleChange(BSTR Text);

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void SetDispatch(IDispatch* pDispatch);
	void SetURL(LPCSTR pstrURL, bool bNavigate);
	void SetWindowSize(int cx, int cy);
	void SetWindowPosition(int x, int y);
	void SetWindowTitle(LPCTSTR szTitle);
	bool SetWindowShowCommand(int nCmd);
	bool IsModal() { return m_bModal; }
	void DoModeless(HWND hWndParent);

	void RegisterCallback(PFNBROWSERCALLBACK pfnBrowserCallback, LPARAM lParam, LPCTSTR szValue);
	void UnregisterCallback();
	bool CallJavaScript(LPCTSTR szFunction, LPCTSTR szArg1);

	HHOOK GetKeyboardHook()
		{ return m_hKeyboardHook; }

	HWND GetBrowserWindow()
		{return m_hWnd; }

	IWebBrowser2 * GetBrowser()
		{return m_spBrowser;}

	bool HasFocus()
		{ return m_bHasFocus; }

	bool IsActive()
	{return m_bIsActive;}

protected:
	void Free();
	void Init();
	void ResourceToURL(const LPCTSTR szURL);
	static LRESULT CALLBACK BrowserKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

protected:
	static CHTMLDialogEx* g_pHtmlDialog;
	HHOOK m_hKeyboardHook;
	HWND m_hWndParent;
	IDispatch * m_pDispatch;
	CComPtr<IWebBrowser2> m_spBrowser;
	CAxWindow m_BrowserWnd;
	CString m_strURL;
	CString m_szID;
	CString m_szTitle;
	PFNBROWSERCALLBACK m_pfnBrowserCallback;
	LPARAM m_pCallbackParam;
	int m_nShowCmd;
	int m_cx;
	int m_cy;
	int m_xpos;
	int m_ypos;
	bool m_bModal;
	bool m_bHasFocus;
	bool m_bIsActive;
};
