#pragma once
#include "stdafx.h"
#include <atlframe.h>
#include "resource.h"
#include "DWebBrowserEventsImpl.h"

class CAddressBookDialog : public CAxDialogImpl<CAddressBookDialog>,
						public CDialogResize<CAddressBookDialog>,
						public DWebBrowserEventsImpl
{
public:
	CAddressBookDialog();
	~CAddressBookDialog();

	enum { IDD = IDD_ADDRESSBOOK_DIALOG };

	BEGIN_MSG_MAP(CAddressBookDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CAddressBookDialog>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CAddressBookDialog)
		DLGRESIZE_CONTROL(IDC_EXPLORER1, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()
	
private:
	bool m_bShowOnInit;
	bool m_bWaiting;
	bool m_bBrowserError;
	IDispatch * m_pDisp;
	CComPtr<IWebBrowser2> m_pBrowser;
	CAxWindow m_BrowserWnd;
	CString m_szURL;
	CString m_szBaseURL;
	DWORD m_dwResultStatus;
	DWORD m_dwMode;

protected:
	bool DisplayMessage(DWORD dwMsgId);
	bool ProcessOptions(LPCTSTR lpszOptions);
	LPCTSTR GetBaseURL();

	virtual void BeforeNavigate2(IDispatch *pDisp, VARIANT *url, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *Cancel);
	virtual void NavigateComplete2(IDispatch *pDisp, VARIANT *URL);
	virtual void DocumentComplete(IDispatch *pDisp, VARIANT *URL);
	virtual void DocumentReallyComplete(IDispatch *pDisp, VARIANT *URL);
	virtual void NavigateError(IDispatch *pDisp, VARIANT *URL, VARIANT *TargetFrameName, VARIANT *StatusCode, VARIANT_BOOL *Cancel);
	virtual void ProgressChange(long Progress, long ProgressMax);
	virtual void DownloadBegin();


public:
	void SetDispatch(IDispatch* pDisp);	
	BOOL SetURL(LPCTSTR lpszURL, bool bNavigate=false);
	BOOL Show(HWND hWndParent, BOOL bShow, int iWidth=0, int iHeight=0, LPCTSTR szURL=NULL);
	CString GetURL();
	bool Initialize(HWND hWndParent, IDispatch* pDispatch,  bool bShow=true);
	void Load(LPCTSTR lpszURL);
	void SetSize(int iHeight, int iWidth);
	bool ShowPage(IWebBrowser2* pWB, UINT nId);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
