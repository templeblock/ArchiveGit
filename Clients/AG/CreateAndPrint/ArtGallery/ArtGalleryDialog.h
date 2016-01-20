#pragma once
#include "resource.h"
#include <atlframe.h>
#include "DWebBrowserEventsImpl.h"
#include "MonitorDirectory.h"


static const UINT WM_ARTGALLERY_DOWNLOADCOMPLETE	= WM_APP + 1;

static LPCTSTR CLIENT_CTP			= _T("ctp"); // Main app
static LPCTSTR CLIENT_IME			= _T("ime"); // Image Editor

static LPCTSTR ART_TYPE				= _T("arttype");	 // type of art, i.e. borders...
static LPCTSTR ART_TYPE_BORDERS		= _T("xn");			// clipart borders
static LPCTSTR ART_TYPE_GENERAL		= _T("xj");			// clipart clipart other than borders

static LPCTSTR DEFAULT_CLIPART_URL		= _T("http://www.americangreetings.com/cnp/cpclipart.pd");
typedef void (CALLBACK * PFNARTGALLERYCALLBACK)(HWND hwnd, UINT uMsgId, LPARAM lParam, LPARAM lArtGalleryPtr, LPCTSTR szValue);
typedef void (CALLBACK * PFNARTSETUPCALLBACK)(HWND hParent, PFNARTGALLERYCALLBACK pfnArtGalleryCallback,  LPARAM lArtGalParam, LPARAM lParam, LPCTSTR szValue);

class CCtp;
///////////////////////////////////////////////////////////////////////////////////////////////////////
class CArtGalleryDialog :	public CAxDialogImpl<CArtGalleryDialog>,
						    public CDialogResize<CArtGalleryDialog>,
						    public DWebBrowserEventsImpl
{
public:
	CArtGalleryDialog(CCtp * pCCtp);
	virtual ~CArtGalleryDialog(void);

	enum { IDD = IDD_ARTGALLERY_DIALOG };

	BEGIN_MSG_MAP(CArtGalleryDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		COMMAND_ID_HANDLER(IDC_FILEDOWNLOAD, OnFileDownload)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CArtGalleryDialog>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CArtGalleryDialog)
		DLGRESIZE_CONTROL(IDC_EXPLORER2, DLSZ_SIZE_X|DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFileDownload(UINT code, UINT id, HWND hWnd, BOOL& bHandled);


	void SetDispatch(IDispatch* pDispatch);
	void SetWindowPosition(int x, int y);
	void SetWindowTitle(LPCTSTR szTitle);
	bool SetURL(LPCSTR szURL, bool bNavigate=true);
	bool SetWindowSize(int cx, int cy);
	bool SetWindowShowCommand(int nCmd);
	


public:
	bool ClosePopup();
	bool LaunchClipArtPopup(HWND hwndParent, LPCTSTR szId);
	bool InitClipArtDownload(LPCTSTR szInfo, LPCTSTR szOptionString);	
	bool ArtAddComplete(LPCTSTR szResults, bool bDeleteFile = true);	
	bool CallJavaScript(LPCTSTR szFunction, LPCTSTR szArg1, IWebBrowser2* pBrowser=NULL);
	bool RegisterCallback(PFNARTGALLERYCALLBACK pfnArtGalleryCallback, LPARAM lParam);
	void UnregisterCallback();
	void FreeClipArtPopup();
	void DoModeless(HWND hWndParent);

	
	bool IsModal() const
		{ return m_bModal; }
	bool IsProcessing() const
		{ return m_bProcessingInProgress; }
	bool IsBorders()
	{ 
		return !m_szArtType.CompareNoCase(ART_TYPE_BORDERS);
	}
	void SetClipArtDownloadInfo(LPCTSTR szInfo)
		{ m_szClipArtDownloadInfo = szInfo; }
	CString& GetClipArtDownloadInfo()
		{ return m_szClipArtDownloadInfo; }
	
	CCtp* GetCtp()
		{return m_pCtp;}


protected:
	CMonitorDirectory * m_pMonitorDirectory;
	CString m_szClipArtDownloadInfo;
	bool m_bProcessingInProgress;
	bool m_bDownloadInProgress;
	CCtp * m_pCtp;
	IDispatch * m_pDispatch;
	CComPtr<IWebBrowser2> m_pBrowser;
	CAxWindow m_BrowserWnd;
	CString m_szURL;
	CString m_szID;
	CString m_szTitle;
	CString m_szFileName;
	CString m_szArtType;
	PFNARTGALLERYCALLBACK m_pfnArtGalleryCallback;
	LPARAM m_pCallbackParam;
	int m_nShowCmd;
	int m_cx;
	int m_cy;
	int m_xpos;
	int m_ypos;
	bool m_bModal;


};
