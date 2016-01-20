#include "stdafx.h"
#include "HTMLDialogEx.h"
#include "DocWindow.h"

#define ID_HTMLCONTROL 9999
//WNDPROC CHTMLDialogEx::m_wpOrigHtmlProc = (WNDPROC)0;

/////////////////////////////////////////////////////////////////////////////
CHTMLDialogEx::CHTMLDialogEx()
{
	m_pDocWnd = NULL;
	m_pDispatch = NULL;
	m_cx = 600;
	m_cy = 300;
	m_bModal = false;
}

/////////////////////////////////////////////////////////////////////////////
CHTMLDialogEx::~CHTMLDialogEx()
{
	//if (m_pDocWnd)
	//	m_pDocWnd->SetHTMLDlgHWND(NULL);
	m_pDocWnd = NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::SetDocWindow(CDocWindow* pDocWnd)
{
	m_pDocWnd = pDocWnd;
	m_bModal = (m_pDocWnd)?true:false;
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::DoModeless(HWND hWndParent)
{
	if (IsWindow())
	{
		BringWindowToTop();
		ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);

		return;
	}

	while (hWndParent)
	{
		DWORD dwStyle = ::GetWindowLong(hWndParent, GWL_STYLE);
		bool bIsApp = ((dwStyle & WS_CAPTION) == WS_CAPTION);
		if (bIsApp)
			break;

		hWndParent = ::GetParent(hWndParent);
	}
	
	if (hWndParent)
	{
		// Sanity check; it should have a title
		char strTitle[MAX_PATH];
		strTitle[0] = '\0';
		::GetWindowText(hWndParent, strTitle, sizeof(strTitle));
		if (!strTitle[0])
			hWndParent = NULL;
	}

	// Create the dialog
	Create(hWndParent);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rect = {0, 0, 0, 0};

	SetWindowPos(NULL, 0, 0, m_cx, m_cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
	GetClientRect(&rect);
	ModifyStyleEx(WS_EX_APPWINDOW|WS_EX_CONTROLPARENT/*dwRemove*/, 0/*dwAdd*/);

	// Create the web browser control
	CAxWindow wnd;
	wnd.Create(m_hWnd, rect, m_strURL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0/*WS_EX_CLIENTEDGE*/, ID_HTMLCONTROL);
	HRESULT hr = wnd.SetExternalDispatch(m_pDispatch);
	//{
	//	m_hWndCtrl = wnd.m_hWnd;
	//	ATLASSERT(::IsWindow(m_hWndCtrl));
	//	CHTMLDialogEx::m_wpOrigHtmlProc = (WNDPROC) ::SetWindowLong(m_hWndCtrl, 
	//		GWL_WNDPROC, (LONG) CHTMLDialogEx::HtmlCtrlSubclassProc); 
	//}

	if (m_pBrowser)
		m_pBrowser.Release();

	m_pBrowser = NULL;
	if (SUCCEEDED(hr))
	{
		hr = wnd.QueryControl(&m_pBrowser);
		if (m_pBrowser)
		{			
			hr = m_pBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);
	        Connect(m_pBrowser);
		}
	}

	DragAcceptFiles(false);
	wnd.DragAcceptFiles(false);

	//if (m_bModal && m_pDocWnd)
	//	m_pDocWnd->SetHTMLDlgHWND(m_hWnd);

	CenterWindow();
	ShowWindow(SW_SHOW);

	return IDOK;  // Let the system set the focus
}

//LRESULT APIENTRY CHTMLDialogEx::HtmlCtrlSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
//{ 
//    if (uMsg == WM_SYSKEYDOWN) 
//        return DLGC_WANTALLKEYS; 
// 
//	return CallWindowProc(CHTMLDialogEx::m_wpOrigHtmlProc, hwnd, uMsg, wParam, lParam); 
//} 
//
/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CHTMLDialogEx::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_pDispatch = NULL;
	m_strURL = "";

//	::SetWindowLong(m_hWndCtrl, GWL_WNDPROC, (LONG) CHTMLDialogEx::m_wpOrigHtmlProc); 

	if (m_pBrowser)
        Disconnect();

	if (m_bModal)
	{
		EndDialog(IDOK);
		//if (m_pDocWnd)
		//	m_pDocWnd->SetHTMLDlgHWND(NULL);
		return S_OK;
	}

	DestroyWindow();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::SetWindowSize(long cx, long cy)
{
	m_cx = cx;
	m_cy = cy;
}

/////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::SetDispatch(IDispatch* pDispatch)
{
	m_pDispatch = pDispatch;
}

///////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::SetURL(LPCSTR pstrURL, bool bNavigate)
{
	m_strURL = pstrURL;

	if (bNavigate && m_pBrowser != NULL)
	{
		CComVariant varFlags = navNoHistory;
		CComVariant varTargetFrameName = "_self";

		HRESULT hr = m_pBrowser->Navigate(CComBSTR(pstrURL), &varFlags, &varTargetFrameName, NULL/*&varPostData*/, NULL/*&varHeaders*/);
	}
}

///////////////////////////////////////////////////////////////////////////////
void CHTMLDialogEx::TitleChange(BSTR Text)
{
	CString strText = Text;

	if (strText.Left(7) == "file://" || strText.Left(7) == "http://")
		strText.Empty();

	if (strText.IsEmpty())
		return;

	SetWindowText(strText);
}
