// HTMLDialog.h : Declaration of the CHTMLDialog
#pragma once
#include "resource.h"
#include "DWebBrowserEventsImpl.h"
#include "CM_Constants.h"


class CMsAddInBase;
class CHTMLDialog : public CDialogImpl<CHTMLDialog>, 
					public DWebBrowserEventsImpl
{
public:
	CHTMLDialog(CMsAddInBase* pAddin);
	~CHTMLDialog();

	enum { IDD = IDD_HTMLDIALOG };

	BEGIN_MSG_MAP(CHTMLDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)
		MESSAGE_HANDLER(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
		MESSAGE_HANDLER(UWM_CMACTIVATE, OnCMActivate)
		REFLECT_NOTIFICATIONS()

	END_MSG_MAP()

	void Popup(HWND hWndParent, IDispatch* pDispatch, LPCSTR pstrURL, RECT *pRect=NULL, bool bActivate=true);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCMActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

	void Close();
	void UpdateSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void SetDispatch(IDispatch* pDispatch);
	void SetURL(LPCSTR pstrURL, bool bNavigate);
	CString GetURL();
	bool CheckForPendingNavigation();

protected:

	virtual void BeforeNavigate2(IDispatch *pDisp, VARIANT *url, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, VARIANT_BOOL *Cancel);
	virtual void NavigateComplete2(IDispatch *pDisp, VARIANT *URL);
	virtual void DocumentComplete(IDispatch *pDisp, VARIANT *URL);
	virtual void DocumentReallyComplete(IDispatch *pDisp, VARIANT *URL);
	virtual void NavigateError(IDispatch *pDisp, VARIANT *URL, VARIANT *TargetFrameName, VARIANT *StatusCode, VARIANT_BOOL *Cancel);
	virtual void ProgressChange(long Progress, long ProgressMax);
	virtual void DownloadBegin();
	virtual void DownloadComplete();

	CString ExtractContentTypeFromUrl(LPCTSTR szUrl);

private:
	CMsAddInBase* m_pAddin;
	CComPtr<IDispatch> m_pDispatch;
	IWebBrowser2Ptr m_pBrowser;
	CString m_szURL;
	CString m_szContentType;
	CString m_szCurrentContentType;
	bool m_bBrowserError;
	bool m_bWaiting;
	HWND m_hBrowserWnd;
};
