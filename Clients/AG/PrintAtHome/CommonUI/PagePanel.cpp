#include "stdafx.h"
#include "PagePanel.h"
#include "Ctp.h"
#include "resource.h"
#include "Image.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CPagePanel::CPagePanel(CCtp* pMainWnd) : 
	m_PageNavigator(pMainWnd)
{
	m_pCtp = pMainWnd;

	COLORREF ColorText = RGB(0,0,0); //j ::GetSysColor(COLOR_WINDOWTEXT);
	COLORREF ColorBgDialog = RGB(255,255,255); //j ::GetSysColor(COLOR_BTNFACE);
	COLORREF ColorBgControl = RGB(240,240,255); //j ::GetSysColor(COLOR_WINDOW);

	m_ColorScheme.m_ColorTextDialog		= ColorText;
	m_ColorScheme.m_ColorTextStatic		= ColorText;
	m_ColorScheme.m_ColorTextMsgBox		= ColorText;
	m_ColorScheme.m_ColorTextEdit		= ColorText;
	m_ColorScheme.m_ColorTextListbox	= ColorText;
	m_ColorScheme.m_ColorTextButton		= ColorText;
	m_ColorScheme.m_ColorBgDialog		= ColorBgDialog;
	m_ColorScheme.m_ColorBgStatic		= ColorBgDialog;
	m_ColorScheme.m_ColorBgMsgBox		= ColorBgDialog;
	m_ColorScheme.m_ColorBgEdit			= ColorBgControl;
	m_ColorScheme.m_ColorBgListbox		= ColorBgControl;
	m_ColorScheme.m_ColorBgButton		= ColorBgControl;
	m_ColorScheme.m_ColorBgScrollbar	= ColorBgDialog;
}

//////////////////////////////////////////////////////////////////////
CPagePanel::~CPagePanel()
{
}

//////////////////////////////////////////////////////////////////////
HWND CPagePanel::Create(HWND hWndParent)
{
	_ASSERTE(m_hWnd == NULL);
	_AtlWinModule.AddCreateWndData(&m_thunk.cd, (CDialogImplBase*)this);

	HDC hDC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
	int nLogPixelsY = ::GetDeviceCaps(hDC, LOGPIXELSY);
	::DeleteDC(hDC);

	HINSTANCE hInst = _AtlBaseModule.GetResourceInstance();
	LPCSTR idDialog = MAKEINTRESOURCE(IDD);
	HWND hWnd = NULL;
	if (nLogPixelsY > 96)
	{
		do // A convenient block we can break out of
		{
			HRSRC hResource = ::FindResource(hInst, idDialog, RT_DIALOG);
			if (!hResource)
				break;
			HGLOBAL hTemplate = ::LoadResource(hInst, hResource);
			if (!hTemplate)
				break;
			LPVOID pTemplate = ::LockResource(hTemplate);
			if (pTemplate)
			{
				int nSize = ::SizeofResource(hInst, hResource);
				LPVOID pNewTemplate = (DLGTEMPLATE*)malloc(nSize);
				if (pNewTemplate)
				{
					memcpy(pNewTemplate, pTemplate, nSize);
					WORD* pwPointsize = (WORD*)_DialogSizeHelper::GetFontSizeField((DLGTEMPLATE*)pNewTemplate);
					if (pwPointsize)
						*pwPointsize = ((*pwPointsize * 96) / nLogPixelsY);

					hWnd = ::CreateDialogIndirectParam(hInst, (DLGTEMPLATE*)pNewTemplate, hWndParent, (DLGPROC)CPagePanel::StartDialogProc, NULL);

					free(pNewTemplate);
				}

				UnlockResource(hTemplate);
			}

			::FreeResource(hTemplate);

		} while (0);
	}

	if (!hWnd)
		hWnd = ::CreateDialogParam(hInst, idDialog, hWndParent, (DLGPROC)CPagePanel::StartDialogProc, NULL);

	m_ToolTip.Create(m_hWnd);
	m_ToolTip.Activate(true);
	DWORD c = m_ToolTip.GetDelayTime(TTDT_AUTOPOP); // How long if remains stationary
	DWORD a = m_ToolTip.GetDelayTime(TTDT_INITIAL); // before coming up
	DWORD b = m_ToolTip.GetDelayTime(TTDT_RESHOW); // between reappearances
//j	m_ToolTip.SetDelayTime(TTDT_INITIAL, 250);

	AddToolTip(IDC_DOC_PAGENAV);
	AddToolTip(IDC_DOC_PAGEADD);
	AddToolTip(IDC_DOC_PAGEDELETE);
	AddToolTip(IDC_DOC_PAGEPREV);
	AddToolTip(IDC_DOC_PAGENEXT);
	AddToolTip(IDC_BTN_CARD, IDS_BTN_VIEWCARD);
	AddToolTip(IDC_BTN_ENVELOPE, IDS_BTN_VIEWENV);

	_ASSERTE(m_hWnd == hWnd);
	return hWnd;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPagePanel::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	COLORREF rgbLabelBackground = RGB(171,170,202);

	m_PageNavigator.InitDialog(
		GetDlgItem(IDC_DOC_PAGENAV),
		GetDlgItem(IDC_DOC_PAGENAME),
		GetDlgItem(IDC_DOC_PAGEPREV),
		GetDlgItem(IDC_DOC_PAGENEXT));

	m_ButtonPageAdd.InitDialog(GetDlgItem(IDC_DOC_PAGEADD), IDR_PAGEADD); //, NULL, RGB(255,0,255)/*clrTransparent*/, rgbLabelBackground/*clrBackground*/);
	m_ButtonPageDelete.InitDialog(GetDlgItem(IDC_DOC_PAGEDELETE), IDR_PAGEDELETE); //, NULL, RGB(255,0,255)/*clrTransparent*/, rgbLabelBackground/*clrBackground*/);
	m_ButtonNext.InitDialog(GetDlgItem(IDC_DOC_PAGENEXT), IDR_PAGENEXT);
	m_ButtonPrev.InitDialog(GetDlgItem(IDC_DOC_PAGEPREV), IDR_PAGEPREV);

	m_ButtonCard.InitDialog(GetDlgItem(IDC_BTN_CARD), IDR_CARD_BUTTON, BMPBTN_AUTOFIRE);
	m_ButtonEnvelope.InitDialog(GetDlgItem(IDC_BTN_ENVELOPE), IDR_ENV_BUTTON, BMPBTN_AUTOFIRE);

	return IDOK;
}

//////////////////////////////////////////////////////////////////////
void CPagePanel::AddToolTip(UINT idControl, UINT idRes)
{
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hinst = _AtlBaseModule.GetResourceInstance();
	ti.hwnd = GetDlgItem(idControl);
	ti.uId = 0;
	::GetClientRect(ti.hwnd, &ti.rect);
	ti.lpszText = (idRes == -1 ? MAKEINTRESOURCE(idControl) : MAKEINTRESOURCE(idRes));
	m_ToolTip.AddTool(&ti);
}

//////////////////////////////////////////////////////////////////////
void CPagePanel::ActivateNewDoc()
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	CAGDoc* pAGDoc = (!pDocWindow ? NULL : pDocWindow->GetDoc());
	m_PageNavigator.ActivateNewDoc(pAGDoc); // Call this even if pAGDoc is NULL

	bool bShow = (!pAGDoc ? false : (pAGDoc->AllowNewPages() | (pDocWindow->SpecialsEnabled() ? pAGDoc->NeedsCardBack() : 0)));
	::ShowWindow(GetDlgItem(IDC_DOC_PAGEADD), (bShow ? SW_SHOW : SW_HIDE));
	::ShowWindow(GetDlgItem(IDC_DOC_PAGEDELETE), (bShow ? SW_SHOW : SW_HIDE));

	bool bAllowEnvWizard = (pAGDoc && pAGDoc->AllowEnvelopeWizard());
	bool bHasLinkedDoc = pDocWindow->HasLinkedDoc();
	bool bIsEnvelope = (pAGDoc && (pAGDoc->GetDocType() == DOC_ENVELOPE));
	bShow = (!pAGDoc ? false : (bAllowEnvWizard ? (bIsEnvelope && !bHasLinkedDoc ? false : true) : false)); //&& bIsEnvelope && !bHasLinkedDoc ? false : true));

	m_ButtonCard.Check(!bIsEnvelope);
	m_ButtonEnvelope.Check(bIsEnvelope);

	::ShowWindow(GetDlgItem(IDC_BTN_CARD), (bShow ? SW_SHOW : SW_HIDE));
	::ShowWindow(GetDlgItem(IDC_BTN_ENVELOPE), (bShow ? SW_SHOW : SW_HIDE));
}

//////////////////////////////////////////////////////////////////////
LRESULT CPagePanel::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (!hDC)
	{
		bHandled = false;
		return S_OK;
	}

	CImage Image(_AtlBaseModule.GetResourceInstance(), IDR_NAVTOOLBAR_BG, "GIF");
	HBITMAP hBitmap = Image.GetBitmapHandle();
	if (!hBitmap)
	{
		bHandled = false;
		return S_OK;
	}

	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	BITMAP bm;
	::GetObject(hBitmap, sizeof(bm), &bm);

	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	::FillRect(hDC, &rect, CAGBrush(WHITE_BRUSH, eStock));
	::BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPagePanel::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return m_ColorScheme.OnCtlColor(uMsg, wParam);
}


////////////////////////////////////////////////////////////////////
LRESULT CPagePanel::OnPageAdd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	if (!pDocWindow)
		return S_OK;

	pDocWindow->OnPageAdd(wNotifyCode, wID, hWndCtl, bHandled);
	return S_OK;
}

////////////////////////////////////////////////////////////////////
LRESULT CPagePanel::OnSwitchToCard(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	if (!pDocWindow)
		return S_OK;

	CAGDoc* pAGDoc = pDocWindow->GetDoc();
	if (!pAGDoc)
		return S_OK;

	if (pAGDoc->GetDocType() != DOC_ENVELOPE)
		m_ButtonCard.Check(true);

	pDocWindow->OnDocMatchCard(wNotifyCode, wID, hWndCtl, bHandled);
	return S_OK;
}

////////////////////////////////////////////////////////////////////
LRESULT CPagePanel::OnSwitchToEnvelope(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	if (!pDocWindow)
		return S_OK;

	CAGDoc* pAGDoc = pDocWindow->GetDoc();
	if (!pAGDoc)
		return S_OK;

	if (pAGDoc->GetDocType() == DOC_ENVELOPE)
		m_ButtonEnvelope.Check(true);

	pDocWindow->OnDocEnvelopeWiz(wNotifyCode, wID, hWndCtl, bHandled);
	return S_OK;
}

////////////////////////////////////////////////////////////////////
LRESULT CPagePanel::OnPageDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	if (!pDocWindow)
		return S_OK;

	pDocWindow->OnPageDelete(wNotifyCode, wID, hWndCtl, bHandled);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPagePanel::OnPagePrev(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int nLastPage = m_PageNavigator.GetPage();
	int nPage = m_PageNavigator.ChangePage(-1/*iDelta*/);
	if (nPage < 0)
		return E_FAIL;

	PageChange(nLastPage, nPage);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPagePanel::OnPageNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int nLastPage = m_PageNavigator.GetPage();
	int nPage = m_PageNavigator.ChangePage(1/*iDelta*/);
	if (nPage < 0)
		return E_FAIL;

	PageChange(nLastPage, nPage);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPagePanel::OnPageChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{ // Notification from the PageNavigator
	LPARAM lParam = (LPARAM)hWndCtl;
	int nLastPage = LOWORD(lParam);
	int nPage = HIWORD(lParam);

	PageChange(nLastPage, nPage);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
bool CPagePanel::PageChange(int nLastPage, int nPage)
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	if (!pDocWindow)
		return false;

	CAGDoc* pAGDoc = pDocWindow->GetDoc();
	if (!pAGDoc)
		return false;

	if (nLastPage >= 0 && nPage >= 0)
	{ // CheckPoint the page change
		// Do the symbol unselect now, because it may CheckPoint the end of a text edit
		pDocWindow->SymbolUnselect(true/*bClearPointer*/);
		pDocWindow->GetUndo().CheckPoint(IDC_DOC_PAGENAV, NULL, nLastPage, nPage);
	}
	
	nPage = m_PageNavigator.GetPage();
	if (nPage >= 0 && nPage < MAX_AGPAGE)
	{
		pAGDoc->SetCurrentPageNum(nPage);
		pDocWindow->ActivateNewPage();
	}

	pDocWindow->SetFocus();
	return true;
}
