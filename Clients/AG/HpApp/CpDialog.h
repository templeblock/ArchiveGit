#pragma once

#include "AutoUpdate.h"
#include "Download.h"


#define WM_INSTALLBEGIN WM_USER + 10

class CWorkspaceDialog;

class CCpDialog : public CDHtmlDialog
{
public:
	CCpDialog(CWnd* pParent = NULL);
	~CCpDialog();

	enum { IDD = IDD_CPDESKTOP };

	LRESULT OnInstallBegin(WPARAM wParam, LPARAM lParam);

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
	virtual void OnPaint();
	virtual HCURSOR OnQueryDragIcon();

	// Event handlers
	HRESULT OnAutoUpdate(IHTMLElement* pElement);
	HRESULT OnLaunch(IHTMLElement* pElement);
	HRESULT OnHelp(IHTMLElement* pElement);
	HRESULT OnMyStartup(IHTMLElement* pElement);
	HRESULT OnLaunchApp(IHTMLElement* pElement);

	HRESULT WindowOpen(IHTMLElement* pElement, int l, int t, int w, int h, LPCSTR pstrName);

	// Message map declarations
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()

	// Local methods
	bool GetFileTimes(LPCSTR strFilePath, FILETIME& TimeCreation, FILETIME& TimeAccessed, FILETIME& TimeModfied);
	bool SetFileTimes(LPCSTR strFilePath, FILETIME& TimeCreation, FILETIME& TimeAccessed, FILETIME& TimeModfied);
	DWORD UpdateFolder(LPCSTR strSrcFolder, LPCSTR strDstFolder, bool bIncludeSubFolders = true);
	bool CheckForNewContent(CString& strContentRootFolder, CString& strAppRootFolder, bool bForceCreate);
	DWORD CheckForUiUpdates(CString& strAppRootFolder);
	bool SaveAppVariablesFile(CString& strContentRootFolder, CString& strAppRootFolder);
	bool SaveContentVariablesFile(CString& strContentRootFolder, CString& strAppRootFolder);
	CString GetHomeFolder();
	CString String(UINT nFormatID, ...);

	void DoAutoUpdate();
	bool SetCheckUpdateDialog(CString val);
	bool ShowCheckUpdateDialog();
	CString GetShowWelcomeScreen();


protected:
	CSimpleArray<CWorkspaceDialog*> m_WorkspaceArray;
	HICON m_hIcon;

public:
	CAutoUpdate m_AutoUpdate;
	CDownload   m_Download;
};
