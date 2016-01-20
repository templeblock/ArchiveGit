#include "stdafx.h"
#include "ToolsCtlPanel.h"
#include "Ctp.h"
#include "AGPage.h"
#include "AGLayer.h"
#include "Version.h"
#include "resource.h"
#include "Image.h"

#define MyB(rgb)	((BYTE)(rgb))
#define MyG(rgb)	((BYTE)(((WORD)(rgb)) >> 8))
#define MyR(rgb)	((BYTE)((rgb)>>16))
#define MYRGB(hex)	RGB(MyR(hex), MyG(hex), MyB(hex))

//////////////////////////////////////////////////////////////////////
CToolsCtlPanel::CToolsCtlPanel(CCtp* pMainWnd)
{
	m_nResID = IDD;
	m_nBkgdResID = IDB_TOOLPANEL1_AG;
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
CToolsCtlPanel::~CToolsCtlPanel()
{
}

//////////////////////////////////////////////////////////////////////
HWND CToolsCtlPanel::Create(HWND hWndParent)
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

					hWnd = ::CreateDialogIndirectParam(hInst, (DLGTEMPLATE*)pNewTemplate, hWndParent, (DLGPROC)CToolsCtlPanel::StartDialogProc, NULL);

					free(pNewTemplate);
				}

				UnlockResource(hTemplate);
			}

			::FreeResource(hTemplate);

		} while (0);
	}

	if (!hWnd)
		hWnd = ::CreateDialogParam(hInst, idDialog, hWndParent, (DLGPROC)CToolsCtlPanel::StartDialogProc, NULL);

	_ASSERTE(m_hWnd == hWnd);
	return hWnd;
}


//////////////////////////////////////////////////////////////////////
void CToolsCtlPanel::InitToolTips()
{
	HWND hwnd = m_ToolTip.Create(m_hWnd);
	ATLASSERT(NULL != hwnd);
	m_ToolTip.Activate(true);
	DWORD c = m_ToolTip.GetDelayTime(TTDT_AUTOPOP); // How long if remains stationary
	DWORD a = m_ToolTip.GetDelayTime(TTDT_INITIAL); // before coming up
	DWORD b = m_ToolTip.GetDelayTime(TTDT_RESHOW); // between reappearances
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanel::ActivateNewDoc()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanel::UpdateControls()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanel::AddToolTip(UINT idControl, UINT idRes)
{
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hinst = _AtlBaseModule.GetResourceInstance();
	ti.hwnd = GetDlgItem(idControl);
	ti.uId = 0;
	::GetClientRect(ti.hwnd, &ti.rect);
	ti.lpszText = MAKEINTRESOURCE(idRes);
	BOOL bSuccess = m_ToolTip.AddTool(&ti);
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanel::AddToolTip(UINT idControl)
{
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hinst = _AtlBaseModule.GetResourceInstance();
	ti.hwnd = GetDlgItem(idControl);
	ti.uId = 0;
	::GetClientRect(ti.hwnd, &ti.rect);
	ti.lpszText = MAKEINTRESOURCE(idControl);
	BOOL bSuccess = m_ToolTip.AddTool(&ti);
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsCtlPanel::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (!hDC)
	{
		bHandled = false;
		return S_OK;
	}

	CImage Image(_AtlBaseModule.GetResourceInstance(), m_nBkgdResID, "GIF");
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

	int dx1 = bm.bmWidth / 2;
	int dx2 = bm.bmWidth - dx1;

	for (int x=0; x < WIDTH(rect); x += dx2)
		::BitBlt(hDC, x, 0, dx2, bm.bmHeight, hMemDC, dx2, 0, SRCCOPY);

	// Paint the ends
	//PaintLeftEdge(hDC, dx1, bm.bmHeight, hMemDC);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanel::PaintLeftEdge(HDC hdc, int ibmWidth, int ibmHeight, HDC hMemDC)
{
	::BitBlt(hdc, 0, 0, ibmWidth, ibmHeight, hMemDC, 0, 0, SRCCOPY);
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsCtlPanel::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return IDOK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsCtlPanel::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return m_ColorScheme.OnCtlColor(uMsg, wParam);
}


