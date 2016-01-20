#pragma once
#pragma comment(lib, "oleacc.lib")
#include "stdafx.h"
#include "JMSrvrDef.h"
#include <atlframe.h>
#include "resource.h"
#include "Authenticate.h"
#include "DWebBrowserEventsImpl.h"


class CRegisterDialog : public CAxDialogImpl<CRegisterDialog>,
						public CDialogResize<CRegisterDialog>,
						public DWebBrowserEventsImpl
{
public:
	CRegisterDialog();
	~CRegisterDialog();

	enum { IDD = IDD_REGISTERDIALOG };

	BEGIN_MSG_MAP(CRegisterDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_HELP, OnHelp)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CRegisterDialog>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CRegisterDialog)
		DLGRESIZE_CONTROL(IDC_EXPLORER1, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()
	
private:
	bool m_bShowOnInit;
	bool m_bRegistrationInProgress;
	bool m_bWaiting;
	bool m_bBrowserError;
	CComPtr<IDispatch> m_pAddIn;
	CComPtr<IWebBrowser2> m_pBrowser;
	CAxWindow m_BrowserWnd;
	CAuthenticate m_Authenticate;
	CString m_strURL;
	CString m_szBaseURL;
//j	CString m_szBrowse;
//j	CString m_szUpgrade;
//j	CString m_szUsage;
//j	CString m_szHelp;
	DWORD m_dwResultStatus;
	DWORD m_dwMode;

protected:
	void ShowHelp();
	void SetBaseURL(LPCTSTR szUrl);
	bool DisplayMessage(DWORD dwMsgId);
	bool AuthPagePrep(CString &szAuthUrl, bool bShow);
	bool SecureSocket();
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
	void SetDispatch(IDispatch* pAddIn);	
	bool Register(DWORD dwMode);
	bool ShowRegisterPage();
	bool ShowSignInPage();
	bool IsUserPaidMember();
	bool ShowRegisterPrompt();
	BOOL SetURL(LPCTSTR pstrURL, BOOL bNavigate);
	BOOL Show(BOOL bShow, int iWidth=0, int iHeight=0);
	CString GetURL();
	bool IsRegistrationdNeeded();
	bool IsAuthenticateNeeded();
	bool IsRegistrationInProgress();
	bool StartAuthenticate();
	bool OnAuthenticate(DWORD dwCustomerNum,
						LPCTSTR szUserId, 
						LPCTSTR szHost, 
						LPCTSTR szUpgrade, 
						LPCTSTR szUsage,
						LPCTSTR szHelp,
						LPCTSTR szBrowse,
						LPCTSTR szOptions,
						DWORD dwUserStatus,
						BOOL bShow,
						DWORD dwResultStatus);
	bool Initialize(HWND hWndParent, IDispatch* pDispatch,  bool bShow=true);
	DWORD GetUserStatus();
	void Load(LPCTSTR pstrURL);
	void SetSize(int iHeight, int iWidth);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnHelp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
