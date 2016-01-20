// MsAddInBase.h : Declaration of the CMsAddInBase
#pragma once

#include "resource.h"       // main symbols
#include "AgOutlookAddIn.h"
#include "HTMLMenu.h"
#include "HTMLDialog.h"
#include "FunctionObject.h"
#include "FlashManager.h"
#include "PhotoManager.h"
#include "PreviewDialog.h"
#include "UsageManager.h"
#include "Authenticate.h"
#include "StationeryManager.h"
#include "SecurityMgr.h"
#include "tom.h"
#include "smartcache.h"
#include "Async.h"
#include "HelperFunctions.h"
#include "EcardDialog.h"


#pragma comment(lib, "oleacc.lib")

#define POPUP_DIALOG 0
#define POPUP_MENU 1
#define CONTENT_PAY		0x0004
#define CONTENT_NEW		0x0001
#define DEFAULT_BROWSE_WIDTH 325
#define DEFAULT_BROWSE_HEIGHT 530


static const int ASSET_GROUP_NORMAL			= 0x0000;
static const int ASSET_GROUP_DEFAULTSTA		= 0x0001;
static const int ASSET_GROUP_FAVORITES		= 0x0002;

static const int CMD_SHOW_OPTIONSMENU		= 0x0001;
static const int CMD_SHOW_ECARDMENU			= 0x0002;
static const int CMD_SHOW_GAMESMENU			= 0x0003;

static LPCTSTR JAZZYMAIL_ATTRIBUTE				= "CreataMail";
static LPCTSTR JM_ATTRIBUTE						= "JM_P";
static LPCTSTR JMU_ATTRIBUTE					= "JM_U";
static LPCTSTR JMINSERT_ATTRIBUTE				= "JM_INS";

static const WORD OPTIONS_DISABLE_NONE			= 0x0000;
static const WORD OPTIONS_DISABLE_CLEAR			= 0x0001;

static LPCTSTR MSG_STATIONERY_STRING = _T("DoubleClickToAddYourPersonalMessage.");

#ifdef _DEBUG
	#define CONTROL (GetAsyncKeyState(VK_CONTROL)<0)
#else
	#define CONTROL false
#endif _DEBUG

static UINT UWM_HTML_GETOBJECT	= ::RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));

class CMsAddInBase : public CAsync
{
public:
friend CHTMLDialog;
friend CPreviewDialog;

	/////////////////////////////////////////////////////////////////////////////
	CMsAddInBase();
	virtual ~CMsAddInBase();
	/////////////////////////////////////////////////////////////////////////////
	
	virtual void HandleShutdown();
	virtual int DisplayMessage(DWORD dwMsgID, UINT uType=MB_ICONWARNING | MB_OK | MB_HELP | MB_TOPMOST);
	virtual BOOL WriteHtmlToDocument(BSTR bstrHtml, IHTMLDocument2Ptr spDoc);
	virtual bool HandleOptionsButton();
	virtual bool HandleBrowserOptionsButton();
	virtual HRESULT HandleDownloadRequest(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice, VARIANT varExternalLink);
	bool HandleGamesButton();
	bool HandleECardsButton();
	bool GetTrayIconStatus(BOOL& bShow);
	DWORD GetClientType();
	CSmartCache& GetSmartCache();
	bool SetTrayIconStatus(BOOL bShow);
	void SetClient(DWORD dwType);
	void SetExpress(bool bExpress);
	void SetDownloadParms(DownloadParms &dParms);
	void SetLastCommand(WORD wCmd);
	bool HasProductContent(IHTMLDocument2 *pDoc);
	bool IsDefaultStationeryNeeded();
	bool IsUserPaidMember();
	bool WriteHTMLFile(const CString& strHTMLFileName, LPCTSTR pstrHtml);
	CString ReadHTML(const CString strDownloadSrc, const CString strDownloadDst, bool bPreview);
	CString ReadHTMLFile(const CString& strHTMLFileName);
	bool RemoveEmptyTags(IHTMLDocument2Ptr spDoc);
	BOOL GetHtmlText(IHTMLDocument2Ptr spDoc, CString& szHtml);
	BOOL SetHtmlText(IHTMLDocument2Ptr spDoc, const CString& szText);
	bool SetFocusToText(IHTMLDocument2Ptr spDoc, LPCSTR szText);
	bool SetJazzyMailAttribute(IHTMLDocument2Ptr spDoc);
	bool SetJMAttribute(IHTMLElement* pElem);
	bool RemoveJazzyMailAttribute(IHTMLDocument2Ptr spDoc);
	BOOL ScrollIntoView(IHTMLDocument2Ptr spDoc);
	bool ReplaceAllText(IHTMLDocument2Ptr spDoc, LPCTSTR szText, LPCTSTR szReplace,bool bExpand=true,  bool bRemoveJMAttribute=true);
	BOOL RemoveJMAttribute(IHTMLElement* pElem);
	bool IsAttributePresent(IHTMLElement* pElem, LPCTSTR szAttribute);
	bool IsComment(IHTMLElement* pElem, LPCTSTR szCommentTag);
	bool IsJazzyMailTag(IHTMLElement* pElem, LPCTSTR szName, CString &szOuterHtml);
	bool VerifySecurity(IHTMLDocument2Ptr spDoc, const CString &szHtml);
	BOOL SetDocEditFields(IHTMLDocument2Ptr spDoc);
	bool HandleOptionsCommand(WORD wCommand, int iSource);

protected:

	virtual bool EmbedHTMLAtSelectionBase(const CString& strHTMLText, IHTMLDocument2Ptr spDoc);
	virtual bool Mark(IHTMLElement * pElem);
	virtual bool DownloadComplete();
	virtual bool AddDefaultStationery(IHTMLDocument2 *pDoc);
	virtual void ShowHelp(DWORD dwHelpID=0);
	virtual void AutoUpdateDone(DWORD dwResult);
	virtual	bool HandleDownloadCommand(WORD wCommand, DownloadParms* pOverrideDownloadParms);
	bool ServerRegister(HWND hwnd, DWORD dwClientType);
	bool ServerUnregister(HWND hwnd);
	bool ProcessExecuteCommand(int nCmdId);
	bool CanAccess();
	bool ShowTrayIcon();
	bool ExecuteDownload(WORD wCommand, int iCategory, bool bFreeContent, bool bPayingMember,
		int iProductId, CString& strName, CString& strDownload,
		int iWidth, int iHeight, CString& strFlashPath,
		int iPathPrice, CString& strExternalLink);	
	void DownloadFiles();
	static bool CALLBACK DownloadCallback(LPARAM lParam, void* pDownloadVoid);
	bool SetFlashPlayParam(IHTMLDocument2Ptr spDoc);
	bool ResetContentEditable(IHTMLDocument2 *pDoc);
	bool HandleOnItemClose();
	BOOL HandleOnItemSend(IHTMLDocument2Ptr spDoc);
	bool EmbedHTMLAtEndBase(IHTMLDocument2Ptr spDoc, const CString& strHTMLText);
	CString PrepareHTML(CString& strHTMLBody, const CString& strDownloadSrc, bool bPreview);
	CString CreateFlashTag(const CString& strURLIn, const CString& strWidth, const CString& strHeight, bool bAddJMAttribute);
	CString CreateReference();
	bool EmbedReference();
	bool EmbedHTMLFile(const CString& strHTMLFileName);
	void ActivateJazzyMailDocument(IHTMLDocument2Ptr spDoc, bool bSetAttribute);
	void DeactivateJazzyMailDocument(IHTMLDocument2Ptr spDoc);
	void ActivateDocumentWindow(IHTMLDocument2Ptr spDoc);
	void DocLoadStartTimer(LPTIMECALLBACK pTimerProc, DWORD dwData1, DWORD dwData2, DWORD dwData3);		
	BOOL DefaultStationery();
	BOOL ClearCurrentStationery();
	BOOL ClearDefaultStationery();
	bool ReferAFriend();
	BOOL AddToFavorites(const CString& strSrcPath, const CString& strCategory, const CString& strNameIn);
	BOOL RemoveFromFavorites(const CString& strCategory, const CString& strNameIn);
	BOOL ShowPreview(const CString& strDownloadSrc, const CString& strDownloadDst);
	bool TurnOnDOMHandling(IHTMLDocument2Ptr spDoc, bool bOn);
	bool IsJazzyMail(IHTMLDocument2Ptr spDoc);
	BOOL GetFavoritesPath(CString& strFavoritesPath);
	BOOL MarkNonJazzyMailTextForPreservation(IHTMLDocument2Ptr spDoc);
	bool IsRegistrationdNeeded();
	bool IsAuthenticateNeeded();
	bool VerifyUserStatus();
	void ShowMyAccount();
	void ShowFeedbackPage();
	void ShowUpgradePage(int iSource, int iPath=0, int iProdnum=0);
	CComPtr<ITextDocument> GetPlainTextDocument(HWND hCompose);
	IHTMLDocument2Ptr GetHTMLDocument2(HWND hCompose);
	bool SetUnselectableAttribute(IHTMLDocument2* pDoc, LPCTSTR szTagName, LPCTSTR szValue);
	bool SaveDownloadInfo();
	WORD TrackOptionsMenu(WORD wDisableFlags=OPTIONS_DISABLE_NONE);
	int GetProductNum(LPCTSTR szFilePath);
	int GetPath(int iPathPrice){return ((iPathPrice & 0x7FFFFC00) >> 10);};

	/////////////////////////////////////////////////////////////////////////////
	// Pure virtual functions that must be implemented by derived classes. 
	virtual bool LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject) = 0;
	virtual bool LaunchFeedbackMail(DWORD dwCmd) = 0;
	virtual bool EmbedHTMLAtSelection(const CString& strHTML) = 0;
	virtual bool EmbedHTMLAtEnd(const CString& strHTMLText)= 0;

protected:
	HWND m_hJMSrvrUtilWnd;
	HWND m_hClientWnd;
	HWND m_hTopParent;
	HINSTANCE m_hMSAAInst;
	CSecurityMgr m_Security;
	CFlashManager m_FlashManager;
	CPhotoManager m_PhotoManager;
	CAuthenticate m_Authenticate;
	CStationeryManager m_StationeryMgr;
	IHTMLDocument2Ptr m_spHotmailDoc;
	CHTMLMenu m_HTMLMenu;
	CHTMLDialog m_HTMLDialog;
	CPreviewDialog m_PreviewDialog;
	CECardDialog m_ECardDlg;
	CUsageManager m_Usage;
	CSmartCache m_SmartCache;
	DownloadParms m_DownloadParms;
	CComBSTR m_bsBookmark;
	WORD m_wLastCommand;
	DWORD m_dwLoggingOpt;
	DWORD m_dwLogLevel;
	DWORD m_dwClient;
	bool m_bExpress;
	bool m_bSrvrShutdown;
};
