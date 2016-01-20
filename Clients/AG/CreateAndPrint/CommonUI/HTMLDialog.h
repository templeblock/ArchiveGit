#pragma once

class CHtmlDialog  
{
public:
	CHtmlDialog(UINT nResID, HWND hWnd = NULL);
	CHtmlDialog(const CString& strURL, BOOL bRes = false, HWND hWnd = NULL);
	virtual ~CHtmlDialog();

	bool DoModal();
	IHTMLWindow2* DoModeless();
	CString GetReturnString();
	void SetDlgOptions(const CString& strOptions);
	void SetWindowSize(int nWidth, int nHeight);
	void SetWindowPosition(int nLeft, int nTop);
	void SetScrollbars(bool bScrollbars);
	void SetResizable(bool bResizable);
	void SetStatusBar(bool bStatusBar);
	void SetCenter(bool bCenter);
	void SetRaisedEdge(bool bRaisedEdge);
	void SetValue(const CString& strControlName, const CString& strValue);
	bool GetControlValue(const CString& strControlName, CString& szValue);

private:
	void CommonConstruct();
	void ResourceToURL(const CString& strURL);

protected:
	HWND m_hWndParent;
	CComVariant m_varReturn;
	CString m_strURL;
	CString m_strArguments;
	CString m_strOptions;
};
