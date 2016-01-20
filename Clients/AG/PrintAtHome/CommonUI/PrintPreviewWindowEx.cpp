// PrintPreviewWindowEx.cpp

#include "stdafx.h"
#include "PrintPreviewWindowEx.h"

void CPrintPreviewWindowEx::DoPaint(CDCHandle dc)
{
	const int X_OFFSET = -10;
	const int Y_OFFSET = -10;

	RECT rcClient;
	GetClientRect(&rcClient);
	RECT rcArea = rcClient;
	rcArea.right += m_nZoom;
	rcArea.bottom += m_nZoom;
	RECT rcTotal = rcArea;
	::InflateRect(&rcArea, X_OFFSET, Y_OFFSET);
	if (rcArea.left > rcArea.right)
		rcArea.right = rcArea.left;
	if (rcArea.top > rcArea.bottom)
		rcArea.bottom = rcArea.top;
	
	RECT rc;
	rc = rcArea;
	GetPageRect(rcArea, &rc);
	dc.FillRect(&rcTotal, (HBRUSH)LongToPtr(COLOR_BTNSHADOW+1));
	dc.SelectClipRgn(NULL);
	dc.FillRect(&rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

	CPrintPreviewWindow::DoPaint(dc.m_hDC, rc);
}

// maintains the user's current scroll position during a window resize
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
	if (width == 0 && height == 0)
		width = height = 1;  // width & height must have non-zero values or we'll fail with an ASSERT

	// calculate the new offset
	if (m_nOldZoom == 0)
	{
		// if previous zoom level was full page view, then set new
		// view to be at center of the page.
		x = m_nZoom / 2;
		y = 0;
	}

	SetScrollSize(width, height, FALSE);
	SetScrollOffset(x, y);
	ScrollWindowEx(-x, -y, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
	
	m_nOldZoom = m_nZoom;

	return 0;
}

