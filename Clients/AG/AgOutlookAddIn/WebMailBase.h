#pragma once
#include "MsAddInBase.h"

class CJMExplorerBand;

class CWebMailBase :
	public CMsAddInBase
{
public:
	CWebMailBase(CJMExplorerBand * pExplorerBar);
	virtual ~CWebMailBase(void);

public:
	void SetWebbrowser(IWebBrowser2 * pWeb);	
	CPreviewDialog& GetPreviewDialog();
	CStationeryManager& GetStationeryMgr();
	CSmartCache& GetSmartCache();
	bool AddPromo();
	void SetWebmailVersion(DWORD dwVer){m_dwVersion=dwVer;};

	// Pure virtual functions that must be implemented by derived classes. 
public:
	virtual bool LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject);
	virtual bool LaunchFeedbackMail(DWORD dwCmd);
	virtual bool EmbedHTMLAtSelection(const CString& strHTML);
	virtual bool EmbedHTMLAtEnd(const CString& strHTMLText);
	virtual bool EmbedHTMLAtSelectionBase(const CString& strHTMLText, IHTMLDocument2Ptr spDoc);
	virtual bool OnNewMessage();
	virtual bool Mark(IHTMLElement * pElem);
	virtual int DisplayMessage(DWORD dwMsgID, UINT uType=MB_ICONWARNING | MB_OK | MB_HELP | MB_TOPMOST);
	virtual bool DownloadComplete();
	virtual bool OnSendEvent();
	virtual bool UndoSendEvent();
	virtual bool IsSendEvent(IHTMLDocument2 *pDoc)=0;
	virtual bool IsComposeView()=0;


protected:
	virtual void AutoUpdateDone(DWORD dwResult){};
	virtual BOOL WriteHtmlToDocument(BSTR bstrHtml, IHTMLDocument2Ptr spDoc=NULL);
	virtual bool GetComposeInterfaces(bool bSilent);
	virtual bool IsNewComposeMessage()=0;

protected:
	bool GetIEFrameDoc(LPCTSTR szFrameId, IWebBrowser2 * pBrowser, IHTMLDocument2** pOutDoc, IWebBrowser2** pOutBrowser);
	bool AddMetaDataToDoc(METATAGLIST &List);
	bool IsTextPresent(IHTMLDocument2 *pDoc);
	bool GetBodyFromSelection(IHTMLDocument2* pDoc, CComPtr<IHTMLElement> &spBodyElem);
	bool GetDocFromSelection(IHTMLDocument2* pMainDoc, CComPtr<IHTMLDocument2> &spTargetDoc);
	bool GetLastIEWindow(IWebBrowser2 * pBrowser, HWND &hLastIEServer, bool bCheckParent=true);
	bool GetDocFromWindow(HWND hwnd, CComPtr<IHTMLDocument2> &spDoc, CComPtr<IWebBrowser2> &spBrowser);
	bool GetIEObjectDoc(IWebBrowser2 * pBrowser, CComPtr<IHTMLDocument2> &spDoc, CComPtr<IWebBrowser2> &spBrowser);

protected:
	CJMExplorerBand * m_pExplorerBar;
	HWND m_hWnd;
	CComPtr<IWebBrowser2> m_spWebBrowser;
	CComPtr<IHTMLDocument2> m_spComposeDoc;
	CComPtr<IWebBrowser2> m_spComposeBrowser;
	CComPtr<IHTMLDocument2> m_spOverrideComposeDoc;
	DWORD m_dwVersion;
	bool m_bOverride;
};
