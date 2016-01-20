#include "stdafx.h"
#include "Label.h"
#include "Utility.h"

/////////////////////////////////////////////////////////////////////////////
CLabel::CLabel()
{
	m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_hBackgroundBrush = ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
	m_bTransparentDraw = false;

	::ZeroMemory(&m_lf, sizeof(m_lf));
	::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(m_lf), &m_lf);

	m_hFont = ::CreateFontIndirect(&m_lf);
	m_iAngle = 0;
}

/////////////////////////////////////////////////////////////////////////////
CLabel::~CLabel()
{
	if (m_hFont)
		::DeleteObject(m_hFont);
	if (m_hBackgroundBrush)
		::DeleteObject(m_hBackgroundBrush);
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::InitDialog(HWND hWndLabel)
{
	if (hWndLabel)
		SubclassWindow(hWndLabel);
//k	SetFontBold(true);
//k	SetSunken(true);
//k	SetTextColor(RGB(255,0,0));
//k	SetFontUnderline(true);
//k	SetFontItalic(true);
//k	SetBorder(true);
//k	SetFontSize(5);
//k	SetBackgroundColor(RGB(0,255,255));
//k	SetBackgroundColor(CLR_NONE);
//k	SetFontName("Cataneo BT");
}

/////////////////////////////////////////////////////////////////////////////
void CLabel::SetText(LPCSTR strText)
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
void CLabel::SetFontName(LPCTSTR strFont)
{	
	strcpy_s(m_lf.lfFaceName, _countof(m_lf.lfFaceName), strFont);
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
