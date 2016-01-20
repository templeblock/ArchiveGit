#include "stdafx.h"
#include "PrintPreviewWindowEx.h"

/////////////////////////////////////////////////////////////////////////////
void CPrintPreviewWindowEx::DoPaint(CDCHandle dc)
{
	const int X_OFFSET = -10;
	const int Y_OFFSET = -10;

	CRect rcClient;
	GetClientRect(&rcClient);
	dc.FillRect(&rcClient, CAGBrush(::GetSysColor(COLOR_BTNSHADOW)));
	
	CRect rcPaintArea = rcClient;
	rcPaintArea.right += m_nZoom;
	rcPaintArea.bottom += m_nZoom;

	rcPaintArea.InflateRect(X_OFFSET, Y_OFFSET);
	if (rcPaintArea.left > rcPaintArea.right)
		rcPaintArea.right = rcPaintArea.left;
	if (rcPaintArea.top > rcPaintArea.bottom)
		rcPaintArea.bottom = rcPaintArea.top;
	
	CRect rcPageRect = rcPaintArea;
	GetPageRect(rcPaintArea, &rcPageRect);
	dc.FillRect(&rcPageRect, CAGBrush(m_BackgroundColor));
//j	dc.SelectClipRgn(ClipRegion);

	CPrintPreviewWindow::DoPaint(dc, rcPageRect);
}

/////////////////////////////////////////////////////////////////////////////
void CPrintPreviewWindowEx::OnDestroy()
{
}

/////////////////////////////////////////////////////////////////////////////
// Maintains the user's current scroll position during a window resize
LRESULT CPrintPreviewWindowEx::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// get current scrollbar position
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;
	GetScrollInfo(SB_HORZ, &si);
	int x = si.nPos;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS;
	GetScrollInfo(SB_VERT, &si);
	int y = si.nPos;

	// get new window height and width
	WORD window_width = LOWORD(lParam);
	WORD window_height = HIWORD(lParam);

	// call base class handler
	CScrollImpl<CPrintPreviewWindowEx>::OnSize(uMsg, wParam, lParam, bHandled);

	// re-set scroll bars
	WORD width = window_width + m_nZoom;
	WORD height = window_height + m_nZoom;
	if (!width && !height)
		width = height = 1;  // width & height must have non-zero values or we'll fail with an ASSERT

	// calculate the new offset
	if (!m_nOldZoom)
	{
		// if previous zoom level was full page view, then set new
		// view to be at center of the page.
		x = m_nZoom / 2;
		y = m_nZoom / 2;
	}

	SetScrollSize(width, height, FALSE);
	SetScrollOffset(x, y);
	ScrollWindowEx(-x, -y, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
	
	m_nOldZoom = m_nZoom;

	return 0;
}
