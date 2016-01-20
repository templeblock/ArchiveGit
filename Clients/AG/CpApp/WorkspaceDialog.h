#pragma once

class CWorkspaceDialog : public CDHtmlDialog
{
public:
	CWorkspaceDialog(CWnd* pParent, LPCSTR pstrURL, RECT* pRect, LPCSTR pstrName);
	~CWorkspaceDialog();

	enum { IDD = IDD_HTMLDIALOG };

	static CWorkspaceDialog* CreateModeless(CWnd* pParent, LPCSTR pstrURL, RECT* pRect, LPCSTR pstrName);
	void Close();

protected:
	// CDHtmlDialog overrides
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnDocumentComplete(LPCTSTR strURL) 
	{
		CDHtmlDialog::OnDocumentComplete((IDispatch*)this, strURL);

		CComQIPtr<IHTMLDocument2> pDocument;
		HRESULT hr = GetDHtmlDocument(&pDocument);
		CComQIPtr<IHTMLElement> pElement;
		pDocument->get_body(&pElement);

		// Convert and disable scroll bars.
		CComQIPtr<IHTMLBodyElement> pBody(pElement);
		pBody->put_scroll(CComBSTR("yes"));
	}
	STDMETHOD(ShowContextMenu)(DWORD dwID, POINT* ppt, IUnknown* pcmdtReserved, IDispatch* pdispReserved);

	// CDialog overrides
	virtual void OnOK();
	virtual void OnCancel();
	virtual void OnClose();
	virtual void OnSysCommand(UINT nID, LPARAM lParam);
	virtual void OnDropFiles(HDROP);

	// Message map declarations
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()

private:
	CString m_strUrl;
	RECT* m_pRect;
	HICON m_hIcon;
	CString m_strName;
};
