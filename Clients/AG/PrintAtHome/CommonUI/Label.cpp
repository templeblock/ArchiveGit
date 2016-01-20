#include "stdafx.h"
#include "Label.h"
#include "Utility.h"

#define TIMER_FLASH_LABEL 1

/////////////////////////////////////////////////////////////////////////////
CLabel::CLabel()
{
	m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_hBackgroundBrush = ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
	m_bTransparentDraw = false;

	::ZeroMemory(&m_lf, sizeof(m_lf));
	::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(m_lf), &m_lf);

	m_hFont = ::CreateFontIndirect(&m_lf);
	m_idTimer = NULL;
	m_bFlashState = false;
	m_bLink = true;
	m_hLinkCursor = NULL;
	m_FlashType = None;
	m_hFlashBrush = NULL;
	m_iAngle = 0;
//j	SetFontRotation(180);
}

/////////////////////////////////////////////////////////////////////////////
CLabel::~CLabel()
{
	if (m_hFont)
		::DeleteObject(m_hFont);
	if (m_hBackgroundBrush)
		::DeleteObject(m_hBackgroundBrush);
	if (m_hFlashBrush)
		::DeleteObject(m_hFlashBrush);
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::InitDialog(HWND hWndLabel)
{
	if (hWndLabel)
		SubclassWindow(hWndLabel);
//k	SetFontBold(true);
//k	SetSunken(true);
//k	SetTextColor(RGB(255,0,0));
//k	FlashText(true);
//k	SetLink(true);
//k	SetFontUnderline(true);
//k	SetFontItalic(true);
//k	SetBorder(true);
//k	SetFontSize(5);
//k	SetBackgroundColor(RGB(0,255,255));
//k	SetBackgroundColor(CLR_NONE);
//k	SetFontName("Cataneo BT");
//k	FlashBackground(true, RGB(0,64,64));
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetText(const CString& strText)
{
	SetWindowText(strText);
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetFontBold(bool bBold)
{	
	m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont();
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetFontUnderline(bool bSet)
{	
	m_lf.lfUnderline = bSet;
	ReconstructFont();
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetFontItalic(bool bSet)
{
	m_lf.lfItalic = bSet;
	ReconstructFont();
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetSunken(bool bSet)
{
	if (!bSet)
		ModifyStyles(m_hWnd, WS_EX_STATICEDGE/*dwRemove*/, 0/*dwAdd*/, true/*bExtended*/, SWP_DRAWFRAME);
	else
		ModifyStyles(m_hWnd, 0/*dwRemove*/, WS_EX_STATICEDGE/*dwAdd*/, true/*bExtended*/, SWP_DRAWFRAME);
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetBorder(bool bSet)
{
	if (!bSet)
		ModifyStyles(m_hWnd, WS_BORDER/*dwRemove*/, 0/*dwAdd*/, false/*bExtended*/, SWP_DRAWFRAME);
	else
		ModifyStyles(m_hWnd, 0/*dwRemove*/, WS_BORDER/*dwAdd*/, false/*bExtended*/, SWP_DRAWFRAME);
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetFontSize(int iSize)
{
	m_lf.lfHeight = -iSize;
	ReconstructFont();
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetFontRotation(int iAngle)
{
	m_iAngle = iAngle;
	m_lf.lfEscapement = m_lf.lfOrientation = (m_iAngle * 10);
	ReconstructFont();		
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetBackgroundColor(COLORREF clrBackground)
{
	m_bTransparentDraw = (clrBackground == CLR_NONE);
	if (m_bTransparentDraw)
		ModifyStyles(m_hWnd, 0/*dwRemove*/, WS_EX_TRANSPARENT/*dwAdd*/, true/*bExtended*/);
	else
		ModifyStyles(m_hWnd, WS_EX_TRANSPARENT/*dwRemove*/, 0/*dwAdd*/, true/*bExtended*/);
	
	if (m_hBackgroundBrush)
		::DeleteObject(m_hBackgroundBrush);
	m_hBackgroundBrush = (m_bTransparentDraw ? (HBRUSH)::GetStockObject(NULL_BRUSH) : ::CreateSolidBrush(clrBackground));
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetFontName(const CString& strFont)
{	
	strcpy_s(m_lf.lfFaceName, strFont);
	ReconstructFont();
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CLabel::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (hDC)
	{
		::SelectObject(hDC, m_hFont);
		::SetTextColor(hDC, m_clrText);
		::SetBkMode(hDC, TRANSPARENT);

		if (m_iAngle)
		{
			int nAlign = ::SetTextAlign(hDC, TA_BASELINE);
			RECT rc;
			GetClientRect(&rc);
			POINT pt;
			::GetViewportOrgEx(hDC, &pt);
			::SetViewportOrgEx(hDC, WIDTH(rc)/2, HEIGHT(rc)/2, NULL);
		//j To restore
		//j	::SetViewportOrgEx(hDC, pt.x/2, pt.y/2, NULL);
		//j	::SetTextAlign(hDC, nAlign);
		}
	}

	if (m_FlashType == Background && !m_bFlashState)
		return (LRESULT)m_hFlashBrush;
	else
		return (LRESULT)m_hBackgroundBrush;
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::Redraw()
{
	if (!IsWindow())
		return;

	::RedrawWindow(m_hWnd, NULL/*lpRectUpdate*/, NULL/*hRgnUpdate*/, RDW_INVALIDATE /*| RDW_UPDATENOW | RDW_ERASE*/);
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::ReconstructFont()
{
	if (m_hFont)
		::DeleteObject(m_hFont);
	m_hFont = ::CreateFontIndirect(&m_lf);
	if (IsWindow())
		SetFont(m_hFont);
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::FlashText(bool bActivate)
{
	if (m_idTimer)
	{
		SetWindowText(m_strFlashSavedText);
		KillTimer(m_idTimer);
		m_idTimer = NULL;
	}

	if (bActivate)
	{
		char strText[MAX_PATH];
		strText[0] = '\0';
		GetWindowText(strText, sizeof(strText));
		m_strFlashSavedText = strText;
		m_bFlashState = false;
		m_idTimer = TIMER_FLASH_LABEL;
		SetTimer(m_idTimer, 500 /*, NULL*/);
		m_FlashType = Text;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::FlashBackground(bool bActivate, COLORREF clrFlashBackground)
{
	if (m_idTimer)
	{
		KillTimer(m_idTimer);
		m_idTimer = NULL;
	}

	if (bActivate)
	{
		if (m_hFlashBrush)
			::DeleteObject(m_hFlashBrush);
		m_hFlashBrush = ::CreateSolidBrush(clrFlashBackground);

		m_bFlashState = false;
		m_idTimer = TIMER_FLASH_LABEL;
		SetTimer(m_idTimer, 500 /*, NULL*/);
		m_FlashType = Background;
	}
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CLabel::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam != TIMER_FLASH_LABEL)
		return S_OK;
		
	m_bFlashState = !m_bFlashState;

	switch (m_FlashType)
	{
		case Text:
		{
			SetWindowText(m_bFlashState ? "" : m_strFlashSavedText);
			break;
		}

		case Background:
		{
			InvalidateRect(NULL, true);
//j			UpdateWindow();
			break;
		}
	}
	
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetLink(bool bLink)
{
	m_bLink = bLink;

	if (bLink)
		ModifyStyles(m_hWnd, 0/*dwRemove*/, SS_NOTIFY/*dwAdd*/, false/*bExtended*/);
	else
		ModifyStyles(m_hWnd, SS_NOTIFY/*dwRemove*/, 0/*dwAdd*/, false/*bExtended*/);
	Redraw();
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetUrl(LPCTSTR szUrl)
{
	m_szUrl = szUrl;

}
/////////////////////////////////////////////////////////////////////////////
LRESULT CLabel::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bLink)
		return S_OK;

	if (m_szUrl.IsEmpty())
	{
		char strLink[MAX_PATH];
		strLink[0] = '\0';
		GetWindowText(strLink, sizeof(strLink));
		ShellExecute(NULL, "open", strLink, NULL, NULL, SW_SHOWNORMAL);
	}
	else
		ShellExecute(NULL, "open", m_szUrl, NULL, NULL, SW_SHOWNORMAL);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CLabel::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_hLinkCursor)
	{
		::SetCursor(m_hLinkCursor);
		return true;
	}

	bHandled = false;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetLinkCursor(HCURSOR hCursor)
{
	m_hLinkCursor = hCursor;
}
