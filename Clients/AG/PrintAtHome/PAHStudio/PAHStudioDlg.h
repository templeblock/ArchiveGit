// PAHStudioDlg.h : header file
//
#pragma once



class CPAHStudioDlgAutoProxy;


// CPAHStudioDlg dialog
class CPAHStudioDlg : public CDHtmlDialog
{
	DECLARE_DYNAMIC(CPAHStudioDlg);
	friend class CPAHStudioDlgAutoProxy;

// Construction
public:
	CPAHStudioDlg(CWnd* pParent = NULL, LPCTSTR szFilePath = NULL);	// standard constructor
	virtual ~CPAHStudioDlg();

	// Dialog Data
	enum { IDD = IDD_PAHStudio_DIALOG, IDH = IDR_HTML_PAHStudio_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);
	HRESULT OnButtonHelp(IHTMLElement *pElement);
	//void __stdcall OnPluginReady();

public:
	static bool CALLBACK MyBrokerCallback(WPARAM wParam, LPARAM lParam, IDispatch * pDisp);
	HRESULT OnMouseOverElement(IHTMLElement *phtmlElement);
	virtual void OnDocumentComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	virtual void OnNavigateComplete(LPDISPATCH pDisp, LPCTSTR szUrl);
	virtual BOOL CanAccessExternal();

	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);



// Implementation
protected:
	CPAHStudioDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;
	BOOL CanExit();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg LRESULT OnPluginReady(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()

public:
	bool LoadFile();
	bool SetPlugin(IDispatch *pDisp);
	void SetFile(LPCTSTR szFilePath);

protected:
	bool Wait(DWORD dwExpire, bool bBusyOverride=false);
	bool GetHtmlDoc(IHTMLDocument2 ** pDoc);
	// IDocHostUIHandler methods
	STDMETHOD(GetDropTarget)(IDropTarget *pDropTarget, IDropTarget **ppDropTarget);
	



protected:
	CString m_szFilePath;
	CComPtr<ICtp> m_spCtp;
	CComPtr<IBroker> m_spBroker;
	
public:
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
