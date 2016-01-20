#include "stdafx.h"
#include "Ctp.h"
#include "resource.h"
#include "ToolsPanelPrint.h"
#include "Image.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CToolsPanelPrint::CToolsPanelPrint(CCtp* pMainWnd)
{
	m_pCtp = pMainWnd;
	m_nResID = IDD;
	m_hBitmap = NULL;

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
CToolsPanelPrint::~CToolsPanelPrint()
{
	if (m_hBitmap)
		::DeleteObject(m_hBitmap);
}

//////////////////////////////////////////////////////////////////////
HWND CToolsPanelPrint::Create(HWND hWndParent)
{
	_ASSERTE(m_hWnd == NULL);
	_AtlWinModule.AddCreateWndData(&m_thunk.cd, (CDialogImplBase*)this);

	HDC hDC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
	int nLogPixelsY = ::GetDeviceCaps(hDC, LOGPIXELSY);
	::DeleteDC(hDC);

	HINSTANCE hInst = _AtlBaseModule.GetResourceInstance();
	LPCSTR idDialog = MAKEINTRESOURCE(m_nResID);
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

					hWnd = ::CreateDialogIndirectParam(hInst, (DLGTEMPLATE*)pNewTemplate, hWndParent, (DLGPROC)CToolsPanelPrint::StartDialogProc, NULL);

					free(pNewTemplate);
				}

				UnlockResource(hTemplate);
			}

			::FreeResource(hTemplate);

		} while (0);
	}

	if (!hWnd)
		hWnd = ::CreateDialogParam(hInst, idDialog, hWndParent, (DLGPROC)CToolsPanelPrint::StartDialogProc, NULL);

	InitToolTips();

	_ASSERTE(m_hWnd == hWnd);
	return hWnd;
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelPrint::InitToolTips()
{
	m_ToolTip.Create(m_hWnd);
	m_ToolTip.Activate(true);
	DWORD c = m_ToolTip.GetDelayTime(TTDT_AUTOPOP); // How long if remains stationary
	DWORD a = m_ToolTip.GetDelayTime(TTDT_INITIAL); // before coming up
	DWORD b = m_ToolTip.GetDelayTime(TTDT_RESHOW); // between reappearances

	AddToolTip(IDC_BTN_PRINT, IDS_BTN_PRINTPROJECT);
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelPrint::ActivateNewDoc()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelPrint::UpdateControls()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelPrint::AddToolTip(UINT idControl, UINT idRes)
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
LRESULT CToolsPanelPrint::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (!hDC)
	{
		bHandled = false;
		return S_OK;
	}

	if (m_hBitmap)
	{
		CImage Image(_AtlBaseModule.GetResourceInstance(), IDR_PRINT_TOOLBAR_AG, "GIF");
		m_hBitmap = Image.GetBitmapHandle(true/*bTakeOver*/);
	}

	if (!m_hBitmap)
	{
		bHandled = false;
		return S_OK;
	}

	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, m_hBitmap);

	BITMAP bm;
	::GetObject(m_hBitmap, sizeof(bm), &bm);

	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	::FillRect(hDC, &rect, CAGBrush(241, 237, 230)); //CAGBrush(WHITE_BRUSH, eStock));

	//int dx1 = bm.bmWidth / 2;
	//int dx2 = bm.bmWidth - dx1;

	//for (int x=0; x < WIDTH(rect); x += dx2)
	//	::BitBlt(hDC, x, 0, dx2, bm.bmHeight, hMemDC, dx2, 0, SRCCOPY);

	// Paint the ends
	//PaintLeftEdge(hDC, dx1, bm.bmHeight, hMemDC);

	for (int x=0; x < WIDTH(rect); x += bm.bmWidth)
		::BitBlt(hDC, x, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelPrint::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_ButtonPrint.InitDialog(GetDlgItem(IDC_BTN_PRINT), IDR_TOOLBARBTN_PRINT);

	return IDOK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelPrint::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return m_ColorScheme.OnCtlColor(uMsg, wParam);
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelPrint::OnBtnPrint(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pCtp->GetDocWindow()->SendMessage(WM_COMMAND, IDC_DOC_PRINT);
	return S_OK;
}

