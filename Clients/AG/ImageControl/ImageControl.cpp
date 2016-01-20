#include "stdafx.h"
#include "ImageControl.h"
#include "DocWindow.h"
#include "MessageBox.h"
#include "Utility.h"

/////////////////////////////////////////////////////////////////////////////
CImageControl::CImageControl()
{
#ifdef _CRTDBG_MAP_ALLOC
	// _CRTDBG_ALLOC_MEM_DF		enables debug heap allocations
	// _CRTDBG_CHECK_CRT_DF		c-runtime memory leaks will be dumped
	// _CRTDBG_LEAK_CHECK_DF	performs automatic leak checking at program exit
	// _CRTDBG_CHECK_ALWAYS_DF	calls _CrtCheckMemory at every allocation and deallocation request
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
	AtlEnableAllocationTracking();
#endif _CRTDBG_MAP_ALLOC

	m_bWindowOnly = true;
	m_pDocWindow = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CImageControl::~CImageControl()
{
	delete m_pDocWindow;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CImageControl::FinalConstruct()
{
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CImageControl::FinalRelease()
{
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CImageControl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	g_szAppName.Format(IDS_MSGBOXTITLE);
	CMessageBox::SetDefaults(m_hWnd, g_szAppName, NULL/*strHtmlFile*/);
	
	DWORD dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	::SetClassLong(m_hWnd, GCL_STYLE, ::GetClassLong(m_hWnd, GCL_STYLE) | CS_DBLCLKS);

	CRect rClient;
	::GetClientRect(m_hWnd, &rClient);

	HRESULT hr = ::RegisterDragDrop(m_hWnd, this);
	if (m_pDocWindow = new CDocWindow(*this))
	{ // Maintained to be the full size of the client area (when shown)
		CRect rect(0, 0, rClient.right, rClient.bottom);
		m_pDocWindow->Create(m_hWnd, rect, "", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, WS_EX_ACCEPTFILES);
		m_pDocWindow->SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE);
	}

	DWORD dwMajor = 0;
	DWORD dwMinor = 0;
	AtlGetCommCtrlVersion(&dwMajor, &dwMinor);
	bool bUsesVisualThemes = (dwMajor >= 6);
//j	CMessageBox::Message(String("CommCtrl version %ld.%ld", dwMajor, dwMinor));

//j	ActivateBrowser();
//j	SrcDownload(bStart);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CImageControl::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::RevokeDragDrop(m_hWnd);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CImageControl::OnDraw(ATL_DRAWINFO& di)
{
#ifndef NOTUSED
	// Set Clip region to the rectangle specified by di.prcBounds
	HRGN hRgnOld = NULL;
	if (::GetClipRgn(di.hdcDraw, hRgnOld) != 1)
		hRgnOld = NULL;

	bool bSelectOldRgn = false;
	CRect rc = *(RECT*)di.prcBounds;
	HRGN hRgnNew = ::CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	if (hRgnNew)
		bSelectOldRgn = (::SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);

	::Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
	::SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
	LPCTSTR pszText = _T("ImageControl");
	::TextOut(di.hdcDraw, (rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2, pszText, lstrlen(pszText));

	if (bSelectOldRgn)
		::SelectClipRgn(di.hdcDraw, hRgnOld);
#endif NOTUSED

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CImageControl::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return m_ColorScheme.OnCtlColor(uMsg, wParam);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CImageControl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pDocWindow)
		return S_OK;

	// wParam values: SIZE_MAXIMIZED, SIZE_MINIMIZED, SIZE_RESTORED
	if (wParam != SIZE_RESTORED)
		return S_OK;

	// Get the new client size
	int iWidth = LOWORD(lParam);
	int iHeight = HIWORD(lParam);

	ConfigureWindows(iWidth, iHeight);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CImageControl::ConfigureWindows(int iWidth, int iHeight)
{
	CRect rClient(0, 0, iWidth, iHeight);
	if (!iWidth && !iHeight)
		::GetClientRect(m_hWnd, &rClient);

	if (!m_pDocWindow)
		return;

	CRect rect(0, 0, rClient.right, rClient.bottom);
	m_pDocWindow->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
	m_pDocWindow->SetupPageView(true/*bSetScrollbars*/);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CImageControl::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (!hDC)
	{
		bHandled = false;
		return S_OK;
	}

	// Say we handled it, but do nothing; OnDraw takes care of everything
	bHandled = true;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CImageControl::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CImageControl::DoCommand(BSTR bstrCommand, BSTR bstrValue, long* plResult)
{
	bool bOK = (m_pDocWindow ? m_pDocWindow->DoCommand(CString(bstrCommand), CString(bstrValue)) : false);
	if (plResult) *plResult = bOK;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CImageControl::GetCommand(BSTR bstrCommand, long* plResult)
{
	// The return value of this function is a set of bitwise flags about the command
	// Bit 1 = Enabled
	// Bit 2 = Checked
	long lResult = (m_pDocWindow ? m_pDocWindow->GetCommand(CString(bstrCommand)) : 0);
	if (plResult) *plResult = lResult;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CImageControl::SetImage(long* plDIB, long* plMatrix, long* plResult)
{
	CImageObject* pBaseImage = (m_pDocWindow ? m_pDocWindow->OpenImage(NULL/*pszFileName*/, (const BITMAPINFOHEADER*)plDIB, (CMatrix*)plMatrix) : NULL);
	bool bSuccess = (pBaseImage != NULL);
	if (plResult)
		*plResult = bSuccess;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CImageControl::GetImage(long** pplDIB, long** pplMatrix, long* plResult)
{
	long lResult = false;
	CImageObject* pBaseImage = (m_pDocWindow ? m_pDocWindow->GetBaseObject() : NULL);
	if (pBaseImage)
	{
		BITMAPINFOHEADER* pDIB = pBaseImage->GetDib();

		COLORREF TransparentColor = pBaseImage->GetTransparentColor();
		if (TransparentColor != CLR_NONE)
			pDIB->biClrImportant  = 0xFF000000 | TransparentColor;

		*pplDIB = (long*)pDIB;
		*pplMatrix = (long*)&pBaseImage->GetMatrix();
		lResult = true;
	}

	if (plResult) *plResult = lResult;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IObjectSafety
HRESULT CImageControl::GetInterfaceSafetyOptions(REFIID riid, DWORD* pdwSupportedOptions, DWORD* pdwEnabledOptions)
{
	if (!pdwSupportedOptions || !pdwEnabledOptions)
		return E_POINTER;

	HRESULT hr = S_OK;
	if (riid == IID_IDispatch)
	{
		*pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
		*pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
	}
	else
	{
		*pdwSupportedOptions = 0;
		*pdwEnabledOptions = 0;
		hr = E_NOINTERFACE;
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// IObjectSafety
HRESULT CImageControl::SetInterfaceSafetyOptions(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions)
{
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// IDropTarget interface
STDMETHODIMP CImageControl::Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	if (!pDataObject)
		return S_OK;

	if (!m_pDocWindow)
		return S_OK;

	CComPtr<IEnumFORMATETC> pEnumFmt;
	HRESULT hr = pDataObject->EnumFormatEtc(DATADIR_GET, &pEnumFmt);
	TCHAR szBuf[100];
	FORMATETC Format1;
	while (pEnumFmt->Next(1, &Format1, NULL) == S_OK)
	{
		::GetClipboardFormatName(Format1.cfFormat, szBuf, sizeof(szBuf));
	}

	STGMEDIUM Medium;
	::ZeroMemory(&Medium, sizeof(Medium));
	FORMATETC Format;
	::ZeroMemory(&Format, sizeof(Format));
	Format.dwAspect = DVASPECT_CONTENT;
	Format.lindex  = -1;
	Format.tymed = TYMED_HGLOBAL;

	Format.cfFormat = CF_HDROP;
	hr = pDataObject->GetData(&Format, &Medium);
	if (FAILED(hr))
	{
		Format.cfFormat = ::RegisterClipboardFormat((LPSTR)"FileGroupDescriptor");
		hr = pDataObject->GetData(&Format, &Medium);
		if (FAILED(hr))
		{
			Format.cfFormat = ::RegisterClipboardFormat((LPSTR)"FileContents");
			hr = pDataObject->GetData(&Format, &Medium);
			if (FAILED(hr))
			{
				Format.cfFormat = CF_HDROP;
				Format.tymed = TYMED_FILE;
				hr = pDataObject->GetData(&Format, &Medium);
				if (FAILED(hr))
				return S_OK;
			}
		}
	}

	if (Format.cfFormat == CF_HDROP)
	{
		if (Format.tymed & TYMED_HGLOBAL)
		{
			// Get the HDROP data from the data object
			HGLOBAL hMemory = Medium.hGlobal;
			if (!hMemory)
				return S_OK;

			HDROP hDrop = (HDROP)::GlobalLock(hMemory);
			if (!hDrop)
			{
				::GlobalUnlock(hMemory);
				return S_OK;
			}

			// Read in the list of files here...
			char szFileName[MAX_PATH];
			int iFilesDropped = ::DragQueryFile(hDrop, -1, NULL, 0);
			for (int i = 0; i < iFilesDropped; i++)
			{
				int iStringLength = ::DragQueryFile(hDrop, i, NULL, 0);
				::DragQueryFile(hDrop, i, szFileName, MAX_PATH);

				CString strFileName = szFileName;
				CString strExtension = StrExtension(strFileName);

				// Process the dropped file here
				bool bJPG = (strExtension == ".jpg");
				m_pDocWindow->DoDropImage(strFileName, pt);
			}

			::GlobalUnlock(hMemory);
		}
		else
		if (Format.tymed & TYMED_FILE)
		{
			if (!Medium.lpszFileName)
				return S_OK;

			CString strFileName(Medium.lpszFileName);
			CString strExtension = StrExtension(strFileName);

			// Process the dropped file here
			bool bJPG = (strExtension == ".jpg");
			m_pDocWindow->DoDropImage(strFileName, pt);
		}
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CImageControl::DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) 
{
	*pdwEffect = DROPEFFECT_COPY;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CImageControl::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	*pdwEffect = DROPEFFECT_COPY;
	m_pDocWindow->DoDragOver(pt);
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CImageControl::DragLeave(void)
{
	m_pDocWindow->DoDragLeave();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CImageControl::DoDragDrop()
{
	DWORD dwEffect = 0;
	::DoDragDrop(this, this, DROPEFFECT_COPY, &dwEffect);
}

/////////////////////////////////////////////////////////////////////////////
// IDropSource interface
STDMETHODIMP CImageControl::QueryContinueDrag(BOOL fEscapePressed,  DWORD grfKeyState)
{
	if (!(grfKeyState & MK_LBUTTON))
		return DRAGDROP_S_DROP;

	if (fEscapePressed)
		return DRAGDROP_S_CANCEL;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CImageControl::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}
