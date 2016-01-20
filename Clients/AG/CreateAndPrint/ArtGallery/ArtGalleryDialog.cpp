#include "stdafx.h"
#include "ArtGalleryDialog.h"
#include "WebPage.h"
#include "Utility.h"


/////////////////////////////////////////////////////////////////////////////
CArtGalleryDialog::CArtGalleryDialog(CCtp * pCtp)
{
	m_pCtp					= pCtp;
	m_pMonitorDirectory		= NULL;
	m_pDispatch				= NULL;
	m_pBrowser				= NULL;
	m_bProcessingInProgress	= false;
	m_bDownloadInProgress	= false;
	m_bModal				= false;
	m_szURL					= DEFAULT_CLIPART_URL;
	m_szTitle				= _T("Create & Print - Art Gallery");
	m_nShowCmd				= SW_SHOW;
	m_cx					= 400;
	m_cy					= 300;
	m_xpos					= 0;
	m_ypos					= 0;
}
/////////////////////////////////////////////////////////////////////////////
CArtGalleryDialog::~CArtGalleryDialog(void)
{
	if (IsWindow())
		DestroyWindow();

	FreeClipArtPopup();
}

/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryDialog::InitClipArtDownload(LPCTSTR szInfo, LPCTSTR szOptionString)
{
	m_bProcessingInProgress = true;
	m_szArtType = GetNamePairValue(szOptionString, ART_TYPE);
	m_szClipArtDownloadInfo = szInfo;
	//SetWindowShowCommand(SW_HIDE);
	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryDialog::SetWindowSize(int cx, int cy)
{
	if (cx < 10 || cy <10)
		return false;

	m_cx = cx;
	m_cy = cy;
	if (!IsWindow())
		return false;

	SetWindowPos(NULL, 0, 0, m_cx, m_cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

	return false;
}
/////////////////////////////////////////////////////////////////////////////
void CArtGalleryDialog::SetWindowPosition(int x, int y)
{
	m_xpos = x;
	m_ypos = y;
	if (IsWindow())
		SetWindowPos(NULL, m_xpos, m_ypos, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}
/////////////////////////////////////////////////////////////////////////////
void CArtGalleryDialog::SetDispatch(IDispatch* pDispatch)
{
	m_pDispatch = pDispatch;
}

///////////////////////////////////////////////////////////////////////////////
bool CArtGalleryDialog::SetURL(LPCSTR szURL, bool bNavigate)
{
	CString szTemp(szURL);
	szTemp.MakeLower();
	if (szTemp.Find(_T("http")) == -1)
		return false;
	
	m_szURL = szURL; //DEFAULT_CLIPART_URL; // szURL - JHC

	if (bNavigate && m_pBrowser != NULL)
	{
		CComVariant varFlags = navNoHistory |  navNoReadFromCache | navNoWriteToCache;
		CComVariant varTargetFrameName = "_self";
		HRESULT hr = m_pBrowser->Navigate(CComBSTR(szURL), 
									&varFlags, 
									&varTargetFrameName, 
									NULL/*&varPostData*/, 
									NULL/*&varHeaders*/);
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////
void CArtGalleryDialog::SetWindowTitle(LPCTSTR szTitle)
{
	m_szTitle = szTitle;
	if (IsWindow())
		SetWindowText(szTitle);
}

/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryDialog::SetWindowShowCommand(int nCmd)
{
	m_nShowCmd = nCmd;
	if (!IsWindow())
		return false;

	ShowWindow(m_nShowCmd);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryDialog::CallJavaScript(LPCTSTR szFunction, LPCTSTR szArg1, IWebBrowser2* pBrowser)
{
	if (NULL == pBrowser)
		return false;

	CComPtr<IDispatch> spDisp;
	HRESULT hr = pBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2> spHtmlDoc(spDisp);
	if (!spHtmlDoc)
		return false;

	CWebPage WebPage;
	
	if (!WebPage.SetDocument(spHtmlDoc))
		return false;

	if (CString(szArg1).IsEmpty())
	{
		if (!WebPage.CallJScript(szFunction))
			return false;
	}
	else
	{
		if (!WebPage.CallJScript(szFunction, szArg1))
			return false;
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryDialog::ClosePopup()
{
	if (!IsWindow())
		return false;

	SendMessage(WM_CLOSE, 0, 0);

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryDialog::LaunchClipArtPopup(HWND hwndParent, LPCTSTR szID)
{
	SetWindowShowCommand(SW_SHOW);
	m_szID = szID;
	DoModeless(hwndParent);
	
	if (!IsWindow())
		return false;

	if (!m_pMonitorDirectory)
		m_pMonitorDirectory = new CMonitorDirectory(m_hWnd, IDC_FILEDOWNLOAD);

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryDialog::ArtAddComplete(LPCTSTR szResults, bool bDeleteFile)
{
	CString szDownloadResults = m_szClipArtDownloadInfo;
	szDownloadResults += _T("^"); // add separator to delimit results
	szDownloadResults += szResults;
	CallJavaScript(_T("ClipArtDownloadComplete"), szDownloadResults, m_pBrowser);
	
	if (!m_szFileName.IsEmpty())
	{
		if (bDeleteFile)
			::DeleteFile(m_szFileName);
		m_szFileName.Empty();
	}
	
	m_szClipArtDownloadInfo.Empty();
	m_bProcessingInProgress = false;
	//ClosePopup();

	return true;
}
/////////////////////////////////////////////////////////////////////////////
void CArtGalleryDialog::FreeClipArtPopup()
{
	delete m_pMonitorDirectory;
	m_pMonitorDirectory = NULL;
	m_bProcessingInProgress = false;
	m_pBrowser = NULL;
}
/////////////////////////////////////////////////////////////////////////////
bool CArtGalleryDialog::RegisterCallback(PFNARTGALLERYCALLBACK pfnArtGalleryCallback, LPARAM lParam)
{
	if (!pfnArtGalleryCallback)
	{
		m_pfnArtGalleryCallback = NULL;
		return false;
	}

	m_pfnArtGalleryCallback = pfnArtGalleryCallback;
	m_pCallbackParam = lParam;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
void CArtGalleryDialog::UnregisterCallback()
{
	m_pfnArtGalleryCallback = NULL;
	m_pCallbackParam = 0;
}

/////////////////////////////////////////////////////////////////////////////
void CArtGalleryDialog::DoModeless(HWND hWndParent)
{
	if (IsWindow())
	{
		BringWindowToTop();
		ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);

		return;
	}

	while (hWndParent)
	{
		DWORD dwStyle = ::GetWindowLong(hWndParent, GWL_STYLE);
		bool bIsApp = ((dwStyle & WS_CAPTION) == WS_CAPTION);
		if (bIsApp)
			break;

		hWndParent = ::GetParent(hWndParent);
	}
	
	if (hWndParent)
	{
		// Sanity check; it should have a title
		char strTitle[MAX_PATH];
		strTitle[0] = '\0';
		::GetWindowText(hWndParent, strTitle, sizeof(strTitle));
		if (!strTitle[0])
			hWndParent = NULL;
	}

	// Create the dialog
	m_bModal = false;
	HWND hwnd = Create(hWndParent);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CArtGalleryDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyleEx(WS_EX_APPWINDOW|WS_EX_CONTROLPARENT/*dwRemove*/, 0/*dwAdd*/);

	DlgResize_Init(false/*bAddGripper*/, true/*bUseMinTrackSize*/, WS_CLIPCHILDREN/*dwForceStyle*/);
	
	// Create the web browser control
	m_BrowserWnd = GetDlgItem(IDC_EXPLORER2);
	if (!::IsWindow(m_BrowserWnd.m_hWnd))
		return TRUE;

	HRESULT hr = m_BrowserWnd.QueryControl(&m_pBrowser);
	ATLASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return IDOK;

	if (m_pDispatch)
		hr = m_BrowserWnd.SetExternalDispatch(m_pDispatch);
	ATLASSERT(SUCCEEDED(hr));
	
	if (m_pBrowser)
	{
		m_pBrowser->put_Silent(VARIANT_FALSE); // Allow dialog boxes such as security alerts.
		m_pBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);
		Connect(m_pBrowser); // Connect DWebBrowserEvents
	}

	SetURL(m_szURL, true/*bNavigate*/);
	SetWindowText(m_szTitle);
	SetWindowPos(NULL, m_xpos, m_ypos, m_cx, m_cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

	HICON hIcon = ::LoadIcon(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_AGICON));
	SetIcon(hIcon, true	 /*bBigIcon*/); // Big Icon
	SetIcon(hIcon, false /*bBigIcon*/); // Samll Icon
	
	
	// Suppress Drag/drop
	if (m_pBrowser)
		m_pBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);
	
	DragAcceptFiles(false);
	m_BrowserWnd.DragAcceptFiles(false);

	// Allow client to override attributes
	if (m_pfnArtGalleryCallback)
	{
		m_pfnArtGalleryCallback(m_hWnd, WM_INITDIALOG, m_pCallbackParam, (LPARAM)this, m_szID);
	}
	
	ShowWindow(m_nShowCmd);

	
	return IDOK;  // Let the system set the focus
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CArtGalleryDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pfnArtGalleryCallback)
	{
		m_pfnArtGalleryCallback(m_hWnd, WM_CLOSE, m_pCallbackParam, (LPARAM)this, m_szID);
	}
	
	if (m_pBrowser)
        Disconnect();

	m_pBrowser = NULL;
	m_BrowserWnd = NULL;

	if (m_bModal)
	{
		EndDialog(IDOK);
	}
	else
	{
		if (::IsWindow(m_hWnd))
			DestroyWindow();
	}

	
	FreeClipArtPopup();

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CArtGalleryDialog::OnFileDownload(UINT code, UINT id, HWND hWnd, BOOL& bHandled)
{
	CWaitCursor Wait;
	
	CString szDownloadResults = _T("error");

	if (m_bDownloadInProgress)
		return E_FAIL;

	m_bDownloadInProgress = true;

	char szTempPath[MAX_PATH];
	::GetTempPath(sizeof(szTempPath), szTempPath);

	CString szPath = CString(szTempPath) + CString("AgDownloads\\");
	::CreateDirectory(szPath, NULL);
	CString szWildcard("*.agd");

	WIN32_FIND_DATA FileInfo;
	HANDLE hFile = ::FindFirstFile((szPath + szWildcard), &FileInfo);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFile);
		CString szFile = FileInfo.cFileName;
		CString szFileName = szPath + szFile;
		m_szFileName = szPath + szFile.Left(szFile.GetLength() - 4);
		if (!::MoveFile(szFileName, m_szFileName)) // if the rename doesn't work...
		{
			::DeleteFile(szFileName); // get rid of the file
		}
		else if (m_pfnArtGalleryCallback)
		{
			m_pfnArtGalleryCallback(m_hWnd, WM_ARTGALLERY_DOWNLOADCOMPLETE, m_pCallbackParam, (LPARAM)this,  m_szFileName);
		}
	}

	m_bDownloadInProgress = false;

	return S_OK;
}
