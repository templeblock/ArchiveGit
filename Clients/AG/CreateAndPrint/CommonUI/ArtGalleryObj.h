#pragma once
#include "HTMLDialogEx.h"
#include "MonitorDirectory.h"

static LPCTSTR ID_ARTGALLERY			= _T("artgallery");
static LPCTSTR DEFAULT_CLIPART_URL		= _T("http://www.americangreetings.com/cnp/ArtGallery.html");
///////////////////////////////////////////////////////////////////////////////////////////////////////
class CArtGalleryObj
{
public:
	CArtGalleryObj(void);
	bool SetURL(LPCTSTR);
	bool ShowPopup(int nCmd);
	bool SetPopupSize(int nWidth, int nHeight);
	bool ClosePopup();


public:
	virtual ~CArtGalleryObj(void);
	void FreeClipArtPopup();
	bool LaunchClipArtPopup(HWND hwndParent, IDispatch * pDisp);
	bool InitClipArtDownload(LPCTSTR szInfo);	
	bool ClipArtDownloadComplete(LPCTSTR szResults);
	bool RegisterCallback(PFNBROWSERCALLBACK pfnBrowserCallback, LPARAM lParam, LPCTSTR szValue);
	

	bool IsProcessing() const
		{ return m_bProcessingInProgress; }
	void SetClipArtDownloadInfo(LPCTSTR szInfo)
		{ m_szClipArtDownloadInfo = szInfo; }
	CString& GetClipArtDownloadInfo()
		{ return m_szClipArtDownloadInfo; }

protected:
	CHTMLDialogEx m_ClipArtDialog;
	CMonitorDirectory * m_pMonitorDirectory;
	HWND m_hClipArtDialog;
	CString m_szClipArtURL;
	CString m_szClipArtDownloadInfo;
	bool m_bProcessingInProgress;


};
