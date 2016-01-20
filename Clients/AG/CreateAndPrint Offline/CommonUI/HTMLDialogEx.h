// HTMLDialogEx.h : Declaration of the CHTMLDialog
#pragma once
#include "resource.h"
#include "DWebBrowserEventsImpl.h"

class CDocWindow;

class CHTMLDialogEx :	public CAxDialogImpl<CHTMLDialogEx>,
						public DWebBrowserEventsImpl
{
public:
	CHTMLDialogEx();
	~CHTMLDialogEx();

	enum { IDD = IDD_HTMLDIALOG };

	BEGIN_MSG_MAP(CHTMLDialogEx)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()

	void TitleChange(BSTR Text);

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void SetDispatch(IDispatch* pDispatch);
	void SetURL(LPCSTR pstrURL, bool bNavigate);
	void SetWindowSize(long cx, long cy);
	void SetDocWindow(CDocWindow* pDocWnd);
	bool IsModal() { return m_bModal; }

	void DoModeless(HWND hWndParent);

//	static LRESULT APIENTRY HtmlCtrlSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CComPtr<IDispatch> m_pDispatch;
	CComPtr<IWebBrowser2> m_pBrowser;
	CString m_strURL;
	long m_cx;
	long m_cy;
	bool m_bModal;
//	static WNDPROC m_wpOrigHtmlProc;
//	HWND m_hWndCtrl;

public:
	CDocWindow * m_pDocWnd;
};
