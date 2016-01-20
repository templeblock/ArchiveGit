#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "Dib.h"
#include "MessageBox.h"

/////////////////////////////////////////////////////////////////////////////
CImageEdit::CImageEdit()
{
	m_pDIB = NULL;
	m_pfnArtSetupCallback = NULL;
	m_Matrix.Unity();
	m_idTab = IDD_IME_MAIN_TAB;
	m_hIcon = (HICON)::LoadIcon(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_IME_IMAGEEDIT));
	m_bIsClipArt = false;
	m_pArtGalleryDialog = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CImageEdit::CImageEdit(const BITMAPINFOHEADER* pDIB, const CMatrix& Matrix, UINT idTab)
{
	m_pDIB = pDIB;
	m_Matrix = Matrix;
	m_idTab = idTab;
	m_hIcon = (HICON)::LoadIcon(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDI_IME_IMAGEEDIT));
	m_bIsClipArt = false;
	m_pArtGalleryDialog = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CImageEdit::~CImageEdit()
{
}
/////////////////////////////////////////////////////////////////////////////
bool CImageEdit::RegisterArtSetupCallback(PFNARTSETUPCALLBACK pfnArtSetupCallback, LPARAM lParam)
{
	m_pfnArtSetupCallback = pfnArtSetupCallback;
	m_pArtSetupCallbackParam = lParam;

	return true;
}

//////////////////////////////////////////////////////////////////////////////
void CImageEdit::UnregisterArtSetupCallback()
{
	m_pfnArtSetupCallback = NULL;
	m_pArtSetupCallbackParam = 0;
}

//////////////////////////////////////////////////////////////////////////////
void CImageEdit::ArtSetup(PFNARTGALLERYCALLBACK pfnArtGalleryCallback, LPARAM lArtGalParam)
{
	if (m_pfnArtSetupCallback)
	{
		m_pfnArtSetupCallback(m_hWnd, pfnArtGalleryCallback, lArtGalParam,  m_pArtSetupCallbackParam, CLIENT_IME);
	}
}

/////////////////////////////////////////////////////////////////////////////
void StandardizeHyperLink(CHyperLink& HyperLink)
{
//	CLogFont LogFont;
//	LogFont.SetCaptionFont();
//	LogFont.SetBold();
//	HFONT hFont1 = LogFont.CreateFontIndirect();

//	CFont Font;
//	HFONT hFont2 = Font.CreatePointFont(15/*nPointSize*/, _T("Courier"), NULL/*hDC*/, true/*bBold*/, true/*bItalic*/);
//	Font.Detach();
//	HyperLink.SetLinkFont(hFont1);
//	HyperLink.m_hFontNormal = hFont1;

	HyperLink.SetHyperLinkExtendedStyle(HLINK_UNDERLINEHOVER | HLINK_COMMANDBUTTON | HLINK_NOTOOLTIP);
	HyperLink.m_clrLink = RGB(0,0,0);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CImageEdit::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	SetMsgHandled(false);
	DoDataExchange(DDX_LOAD);

	DlgResize_Init();
	m_ptMinTrackSize.x /= 2;

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, true);			// Set big icon
	SetIcon(m_hIcon, false);		// Set small icon

	ModifyStyle(0/*dwRemove*/, WS_CLIPCHILDREN/*dwAdd*/);

//j	StandardizeHyperLink(m_btnStartOver);
//j	StandardizeHyperLink(m_btnSave);

	m_TabControl.Init(m_idTab);
//j	m_TabControl.Uxtheme_EnableThemeDialogTexture(m_view1, ETDT_ENABLETAB);
	InitializeImageControl();

	CToolTipCtrl ToolTip = GetToolTipCtrl();
	ToolTip.SetTitle(1/*Info*/, "Image Editor Tip");

	return true;  // return true  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
bool CImageEdit::InitializeImageControl()
{
	if (m_ImageControl)
		return true;

	HWND hControl = GetDlgItem(IDC_IME_IMAGE_CONTROL);
	if (!hControl)
		return false;

	CComPtr<IUnknown> punk;
	HRESULT hr = AtlAxGetControl(hControl, &punk);
	m_ImageControl = punk;

	// Need to init the control with the selected bitmap
	if (m_ImageControl && m_pDIB)
		long lRes = m_ImageControl->SetImage((long*)m_pDIB, (long*)&m_Matrix);

	return (SUCCEEDED(hr) && m_ImageControl);
}

/////////////////////////////////////////////////////////////////////////////
bool CImageEdit::DoCommand(LPCTSTR strCommand, LPCTSTR strValue)
{
	try {
		if (InitializeImageControl())
			return !!m_ImageControl->DoCommand(CComBSTR(strCommand), CComBSTR(strValue));
	}
	catch(...)
	{
		return false;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
long CImageEdit::GetCommand(LPCTSTR strCommand)
{
	try {
		if (InitializeImageControl())
			return m_ImageControl->GetCommand(CComBSTR(strCommand));
	}
	catch(...)
	{
		return 0;
	}
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
bool CImageEdit::GetImage(BITMAPINFOHEADER*& pDIB, CMatrix& Matrix)
{
	if (!m_ImageControl)
		return false;

	try {
		long* plDIB = NULL;
		long* plMatrix = NULL;
		long lRes = m_ImageControl->GetImage(&plDIB, &plMatrix);
		pDIB = DibCopy((BITMAPINFOHEADER*)plDIB);
		Matrix = *(CMatrix*)plMatrix;
	}
	catch(...)
	{
		return false;
	}
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CImageEdit::OnStartOver(UINT uCode, int nID, HWND hwndCtrl)
{
	if (GetCommand("GetModified"))
	{
//j		int iResponse = CMessageBox::Message("The image has been modifed.  Would you like to start over and lose all of your changes?", MB_YESNO);
		int iResponse = MessageBox("The image has been modifed.  Would you like to start over and lose all of your changes?", "Create & Print Image Editor", MB_YESNO);
		if (iResponse != IDYES)
			return;
	}

	DoCommand(_T("StartOver"), _T(""));
	m_TabControl.Reset();
}

/////////////////////////////////////////////////////////////////////////////
void CImageEdit::OnImageSave(UINT uCode, int nID, HWND hwndCtrl)
{
	bool bOK = DoCommand(_T("FileTab"), _T("Save"));
}

/////////////////////////////////////////////////////////////////////////////
void CImageEdit::OnZoomIn(UINT uCode, int nID, HWND hwndCtrl)
{
	DoCommand(_T("ZoomIn"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CImageEdit::OnZoomOut(UINT uCode, int nID, HWND hwndCtrl)
{
	DoCommand(_T("ZoomOut"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CImageEdit::OnZoomFull(UINT uCode, int nID, HWND hwndCtrl)
{
	DoCommand(_T("ZoomFull"), _T(""));
}

/////////////////////////////////////////////////////////////////////////////
void CImageEdit::OnDone(UINT uCode, int nID, HWND hwndCtrl)
{
	if (m_TabControl.CanApply())
	{
//j		int iResponse = CMessageBox::Message("Would you like to apply your most recent changes before closing?", MB_YESNO);
		int iResponse = MessageBox("Would you like to apply your most recent changes before closing?", "Create & Print Image Editor", MB_YESNO);
		if (iResponse == IDYES)
			m_TabControl.Apply();
		else
			m_TabControl.Reset();
	}
	
	EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
void CImageEdit::OnCancel(UINT uCode, int nID, HWND hwndCtrl)
{
	EndDialog(IDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
void CImageEdit::OnClose()
{
	EndDialog(IDCANCEL);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CImageEdit::OnGetObject(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)this;
}
