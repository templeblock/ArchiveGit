#include "stdafx.h"
#include "Plugin.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

/////////////////////////////////////////////////////////////////////////////
CPlugin::CPlugin() :
	m_Download(this)
{
	m_pNPPInstance = NULL;
	m_hImageMemory = NULL;
	::SetRectEmpty(&m_HotRect);
}

/////////////////////////////////////////////////////////////////////////////
CPlugin::~CPlugin()
{
	Free();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPlugin::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	::SetClassLong(m_hWnd, GCL_STYLE, ::GetClassLong(m_hWnd, GCL_STYLE) | CS_DBLCLKS);

	SrcDownload(true/*bStart*/);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPlugin::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Free();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CPlugin::Free()
{
	if (m_hImageMemory)
		::GlobalFree(m_hImageMemory);

	if (m_pBrowser)
	{
		m_pBrowser.Release();
		m_pBrowser = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPlugin::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (!hDC)
	{
		bHandled = false;
		return S_OK;
	}

	// Say we handled it, but do nothing; OnPaint takes care of everything
	bHandled = true;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPlugin::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return S_OK;
}

#ifdef NOTUSED //j
/////////////////////////////////////////////////////////////////////////////
HRESULT CPlugin::OnDraw(ATL_DRAWINFO& di)
{
	return Draw(di.hdcDraw);
}
#endif NOTUSED //j

/////////////////////////////////////////////////////////////////////////////
LRESULT CPlugin::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT PaintStruct;
	HDC hDC = ::BeginPaint(m_hWnd, &PaintStruct);
	if (!hDC)
		return E_FAIL;

	::FillRect(hDC, &PaintStruct.rcPaint, (HBRUSH)::GetStockObject(WHITE_BRUSH));
	if (!PaintImage(hDC))
	{
		HBITMAP hBitmap = ::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDB_DEFAULT_BUTTON));
		HDC hMemDC = ::CreateCompatibleDC(hDC);
		HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
		BITMAP bm;
		::GetObject(hBitmap, sizeof(bm), &bm);

		RECT rect;
		::GetClientRect(m_hWnd, &rect);
		m_HotRect.left   = ((rect.right - rect.left) - bm.bmWidth) / 2;
		m_HotRect.top    = ((rect.bottom - rect.top) - bm.bmHeight) / 2;
		m_HotRect.right  = m_HotRect.left + bm.bmWidth;
		m_HotRect.bottom = m_HotRect.top + bm.bmHeight;

		::BitBlt(hDC, m_HotRect.left, m_HotRect.top, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
		::SelectObject(hMemDC, hOldBitmap);
		::DeleteObject(hBitmap);
		::DeleteDC(hMemDC);
	}

	::EndPaint(m_hWnd, &PaintStruct);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
bool CPlugin::PaintImage(HDC hPaintDC)
{
	if (!m_hImageMemory)
		return false;

	// Create an IStream from the file in memory
	IStream* pStreamIn = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(m_hImageMemory, false/*fDeleteOnRelease*/, &pStreamIn);
	if (FAILED(hr) || !pStreamIn)
		return false;

	// Create an IPicture from the IStream; the function opens JPEG's, BMP's, GIF's, WMF's, EMF's, and ICO's!!
	DWORD dwSize = ::GlobalSize(m_hImageMemory);
	IPicture* pPicture = NULL;
	hr = ::OleLoadPicture(pStreamIn, dwSize, false, IID_IPicture, (void**)&pPicture);

	// Either way we are done with the IStream and the memory
	pStreamIn->Release();

	if (FAILED(hr) || !pPicture)
		return false;

	// Get the screen dimensions for the HIMETRIC conversion
	HDC hDC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
	long lResolutionX = GetDeviceCaps(hDC, LOGPIXELSX);
	long lResolutionY = GetDeviceCaps(hDC, LOGPIXELSY);
	::DeleteDC(hDC);

	// Get width and height of the picture
	#define HIMETRIC_INCH 2540

	long hmWidth = 0;
	hr = pPicture->get_Width(&hmWidth);
	int bmWidth	= ::MulDiv(hmWidth, lResolutionX, HIMETRIC_INCH);

	long hmHeight = 0;
	hr = pPicture->get_Height(&hmHeight);
	int bmHeight	= ::MulDiv(hmHeight, lResolutionY, HIMETRIC_INCH);

	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	m_HotRect.left   = ((rect.right - rect.left) - bmWidth) / 2;
	m_HotRect.top    = ((rect.bottom - rect.top) - bmHeight) / 2;
	m_HotRect.right  = m_HotRect.left + bmWidth;
	m_HotRect.bottom = m_HotRect.top + bmHeight;

	// Render the picture into the DC
	hr = pPicture->Render(
		hPaintDC,		// HDC the device context on which to render the image
		m_HotRect.left,	// long left position of destination in HDC
		m_HotRect.top,	// long top position of destination in HDC
		bmWidth,		// long width of destination in HDC
		bmHeight,		// long height of destination in HDC
		0,				// OLE_XPOS_HIMETRIC left position in source picture
		hmHeight,		// OLE_YPOS_HIMETRIC top position in source picture
		hmWidth,		// OLE_XSIZE_HIMETRIC width in source picture
		-hmHeight,		// OLE_YSIZE_HIMETRIC height in source picture
		NULL			// LPCRECT pointer to destination for a metafile hdc
	);

	pPicture->Release();
	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPlugin::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	if (!::PtInRect(&m_HotRect, pt))
		return S_OK;
	
	if (m_pBrowser)
	{
		HWND hWnd = NULL;
		m_pBrowser->get_HWND((long*)&hWnd);
		if (!hWnd)
		{
			m_pBrowser.Release();
			m_pBrowser = NULL;
		}
	}
	
	if (!m_pBrowser)
	{
		HRESULT hResult = m_pBrowser.CoCreateInstance(CComBSTR("InternetExplorer.Application"));
		if (!m_pBrowser)
			return hResult;

		// Get the desired window size and location
		int w = GetOptionInt("width", 400);
		int h = GetOptionInt("height", 300);
		int l = GetOptionInt("left", -1);
		int t = GetOptionInt("top", -1);

		if (l < 0) l = (::GetSystemMetrics(SM_CXSCREEN) - w) / 2;
		if (l < 0) l = 0;
		if (t < 0) t = (::GetSystemMetrics(SM_CYSCREEN) - h) / 2;
		if (t < 0) t = 0;

		m_pBrowser->put_Width(w);
		m_pBrowser->put_Height(h);
		m_pBrowser->put_Left(l);
		m_pBrowser->put_Top(t);

		bool bToolBar = !!GetOptionInt("toolbar", true);
		m_pBrowser->put_ToolBar(bToolBar);
		bool bStatusBar = !!GetOptionInt("status", true);
		m_pBrowser->put_StatusBar(bStatusBar);
		bool bMenuBar = !!GetOptionInt("menubar", true);
		m_pBrowser->put_MenuBar(bMenuBar);
		bool bResizable = !!GetOptionInt("resizable", true);
		m_pBrowser->put_Resizable(bResizable);
		bool bAddressBar = !!GetOptionInt("location", true);
		m_pBrowser->put_AddressBar(bAddressBar);
		bool bScrollBars = !!GetOptionInt("scrollbars", false);
		// There is no call to set the scrollbars
	}
	
	CComVariant varFlags = navNoHistory;
	CComVariant varName = "_self";
	CComVariant varPostData;
	CComVariant varHeaders;
	m_pBrowser->Navigate(m_bstrURL, &varFlags, &varName, &varPostData, &varHeaders);

	VARIANT_BOOL bBusy = false; // Initialize this to true if you want to wait for the document to load
	while (bBusy)
		m_pBrowser->get_Busy(&bBusy);

	m_pBrowser->put_Visible(true);
	HWND hWnd = NULL;
	m_pBrowser->get_HWND((long*)&hWnd);
	if (hWnd)
		::BringWindowToTop(hWnd);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CPlugin::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Because the WM_SETCURSOR message is unreliable in a Netscape 6.2 plugin, we set the cursor in WM_MOUSEMOVE
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	if (!::PtInRect(&m_HotRect, pt))
	{
		bHandled = false;
		return E_FAIL;
	}
	
	::SetCursor(::LoadCursor(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(ID_HAND)));
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CPlugin::SrcDownload(bool bStart)
{
	if (m_bstrSrc.Length() <= 0)
		return S_OK;

	if (!m_Download.Init(0/*iFileType*/, CPlugin::MySrcDownloadCallback))
		return S_OK;

	m_Download.AddFile(CString(m_bstrSrc), CString(""), NULL);
	if (bStart)
		m_Download.Start();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
bool CALLBACK CPlugin::MySrcDownloadCallback(CPlugin* pPlugin, void* pDownloadVoid)
{
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (!pDownload) // The final callback when there are no more files to download
		return false;

	// If no download data, get out
	if (!pDownload->pData || !pDownload->dwSize)
		return false;

	// We will store the downloaded image in global memory

	// Free any previously downloaded image memory
	if (pPlugin->m_hImageMemory)
	{
		::GlobalFree(pPlugin->m_hImageMemory);
		pPlugin->m_hImageMemory = NULL;
	}

	// Allocate some global memory based on data size
	pPlugin->m_hImageMemory = ::GlobalAlloc(GMEM_MOVEABLE, pDownload->dwSize);
	if (!pPlugin->m_hImageMemory)
		return false;

	// Lock down the memory and get a pointer
	char* pMemory = (char*)::GlobalLock(pPlugin->m_hImageMemory);
	if (!pMemory)
	{
		::GlobalFree(pPlugin->m_hImageMemory);
		pPlugin->m_hImageMemory = NULL;
		return false;
	}

	// Make a copy of the downloaded image data
	::CopyMemory(pMemory, pDownload->pData, pDownload->dwSize);
	::GlobalUnlock(pPlugin->m_hImageMemory);

	// Force a repaint
	pPlugin->InvalidateRect(NULL);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CPlugin::GetSrc(BSTR* bstrSrcURL)
{
	*bstrSrcURL = m_bstrSrc.Copy();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CPlugin::PutSrc(BSTR bstrSrcURL)
{
	m_bstrSrc = bstrSrcURL;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CPlugin::GetURL(BSTR* bstrURL)
{
	*bstrURL = m_bstrURL.Copy();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CPlugin::PutURL(BSTR bstrURL)
{
	m_bstrURL = bstrURL;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CPlugin::GetContext(BSTR* bstrContext)
{
	*bstrContext = m_bstrContext.Copy();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CPlugin::PutContext(BSTR bstrContext)
{
	m_bstrContext = bstrContext;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
CString CPlugin::GetOption(LPCSTR pNameTarget)
{
	CString strOptions = CString(m_bstrContext);
	CString strNameTarget = pNameTarget;
	strOptions += _T(',');

	if (!strOptions.GetLength())
		return CString("");

	int iStart = 0;
	while (iStart >= 0)
	{
		int iEnd = strOptions.Find(_T(','), iStart);
		if (iEnd <= iStart)
			break;
		int iPair = strOptions.Find(_T('='), iStart);
		if (iPair > iStart && iPair < iEnd)
		{
			CString strName = strOptions.Mid(iStart, iPair-iStart);
			strName.TrimLeft();
			strName.TrimRight();
			strName.MakeLower();
			if (strName == strNameTarget)
			{
				iPair++;
				CString strValue = strOptions.Mid(iPair, iEnd-iPair);
				strValue.TrimLeft();
				strValue.TrimRight();
				return strValue;
			}
		}

		iStart = iEnd + 1;
	}

	return CString("");
}

/////////////////////////////////////////////////////////////////////////////
int CPlugin::GetOptionInt(LPCSTR pNameTarget, int iDefaultValue)
{
	CString strValue = GetOption(pNameTarget);
	if (strValue.IsEmpty())
		return iDefaultValue;

	return atoi(strValue);
}

/////////////////////////////////////////////////////////////////////////////
void CPlugin::SetNPPInstance(NPP pInstance)
{
	m_pNPPInstance = pInstance;
}
