#include "stdafx.h"
#include "ArtGalleryObj.h"

CArtGalleryObj::CArtGalleryObj(void)
{
	m_hClipArtDialog		= NULL;
	m_pMonitorDirectory		= NULL;
	m_bProcessingInProgress	= false;
	m_szClipArtURL			= DEFAULT_CLIPART_URL;

}

CArtGalleryObj::~CArtGalleryObj(void)
{
	FreeClipArtPopup();
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryObj::InitClipArtDownload(LPCTSTR szInfo)
{
	m_bProcessingInProgress = true;
	m_szClipArtDownloadInfo = szInfo;
	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryObj::SetURL(LPCTSTR szURL)
{
	CString szTemp(szURL);
	szTemp.MakeLower();
	if (szTemp.Find(_T("http")) == -1)
		return false;

	m_szClipArtURL = szURL;
	m_ClipArtDialog.SetURL(szURL, true);

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryObj::ShowPopup(int nCmd)
{
	return m_ClipArtDialog.SetWindowShowCommand(nCmd);
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryObj::SetPopupSize(int nWidth, int nHeight)
{
	if (nWidth < 10 || nHeight < 10)
		return false;

	m_ClipArtDialog.SetWindowSize(nWidth, nHeight);

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryObj::ClosePopup()
{
	if (!::IsWindow(m_hClipArtDialog))
		return false;

	::SendMessage(m_hClipArtDialog, WM_CLOSE, 0, 0);

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryObj::LaunchClipArtPopup(HWND hwndParent, IDispatch * pDisp)
{
	m_pMonitorDirectory = new CMonitorDirectory(hwndParent, IDC_FILEDOWNLOAD);

	m_ClipArtDialog.SetDispatch(pDisp);
	m_ClipArtDialog.SetURL(m_szClipArtURL, false);
	m_ClipArtDialog.SetWindowTitle(_T("Create & Print - Art Gallery"));
	m_ClipArtDialog.DoModeless(hwndParent);

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryObj::ClipArtDownloadComplete(LPCTSTR szResults)
{
	CString szDownloadResults = m_szClipArtDownloadInfo;
	szDownloadResults += _T("^"); // add separator to delimit results
	szDownloadResults += szResults;
	m_ClipArtDialog.CallJavaScript(_T("ClipArtDownloadComplete"), szDownloadResults);
	m_szClipArtDownloadInfo.Empty();
	m_bProcessingInProgress = false;
	return true;
}
/////////////////////////////////////////////////////////////////////////////
void CArtGalleryObj::FreeClipArtPopup()
{
	m_hClipArtDialog = NULL;
	delete m_pMonitorDirectory;
	m_pMonitorDirectory = NULL;
	m_bProcessingInProgress = false;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryObj::RegisterCallback(PFNBROWSERCALLBACK pfnBrowserCallback, LPARAM lParam, LPCTSTR szValue)
{
	m_ClipArtDialog.RegisterCallback(pfnBrowserCallback, lParam, szValue);
	return true;
}