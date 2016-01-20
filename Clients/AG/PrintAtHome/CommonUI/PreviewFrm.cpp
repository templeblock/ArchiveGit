#include "stdafx.h"
#include "previewfrm.h"
#include "atlctrlw.h"
#include "AGDC.h"

CPreviewFrame::CPreviewFrame()
{
	//Init();
}

void CPreviewFrame::ResetWindowState()
{
	m_bPrintPreview = false;
	m_hWndToolbar = NULL;
	m_fPrevPage = false;
	m_fNextPage = true;
	m_fZoomIn = true;
	m_fZoomOut = false;
	m_nCurrentPage = 0;
	m_nMaxPage = 0;
}

LRESULT CPreviewFrame::OnCreate(LPCREATESTRUCT lpcs)
{
	//Create the Print Preview toolbar.
	ResetWindowState();
	m_hWndToolbar = CreateSimpleToolBarCtrl(m_hWnd, IDR_PRINTPREVIEW, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(m_hWndToolbar, NULL, TRUE);
	CreateSimpleStatusBar();

	// minimum window size settings
	RECT rect;
	GetWindowRect(&rect);
	m_ptMinTrackSize.x = rect.right - rect.left;
	m_ptMinTrackSize.y = rect.bottom - rect.top;

	UpdateToolBarState();
	return 0;
}

LRESULT CPreviewFrame::OnDestroy()
{
	return 0;
}

void CPreviewFrame::OnPrevPage(UINT uCode, int nID, HWND hwndCtrl)
{
	//// move to prev page
	if (m_pPreviewWnd != NULL)
	{
		if (m_pPreviewWnd->PrevPage())
		{
			m_nCurrentPage--;
			m_pPreviewWnd->Invalidate();

			// enable next page button
			m_fNextPage = true;

		}

		//// check to see if there is another page before this one.
		//// if not, disable the prev page button
		if (m_nCurrentPage == 0)
		{
			m_fPrevPage = false;
		}
		UpdateToolBarState();
	}
}

void CPreviewFrame::OnNextPage(UINT uCode, int nID, HWND hwndCtrl)
{
	//// move to prev page
	if (m_pPreviewWnd != NULL)
	{
		if (m_pPreviewWnd->NextPage())
		{
			m_nCurrentPage++;
			m_pPreviewWnd->Invalidate();

			// enable next page button
			m_fPrevPage = true;

		}

		//// check to see if there is another page before this one.
		//// if not, disable the prev page button
		if (m_nCurrentPage == m_nMaxPage)
		{
			m_fNextPage = false;
		}
		UpdateToolBarState();
	}
}

void CPreviewFrame::OnZoomIn(UINT uCode, int nID, HWND hwndCtrl)
{
	if (m_pPreviewWnd != NULL)
	{
		// for simplicity, we have four possible zoom sizes.
		int nCurrentZoom = m_pPreviewWnd->GetZoom();

		switch (nCurrentZoom)
		{
		case 0:
			nCurrentZoom = 200;
			break;
		case 200:
			nCurrentZoom = 400;
			break;
		case 400:
			nCurrentZoom = 800;
			break;
		}

		// set zoom & scrollbars and then redraw
		ZoomPrintPreview(nCurrentZoom);

		// turn on Zoom Out button
		m_fZoomOut = true;

		// turn off Zoom In button if we are at the
		// highest zoom
		if (nCurrentZoom == 800)
			m_fZoomIn = false;

		UpdateToolBarState();
	}
}

void CPreviewFrame::OnZoomOut(UINT uCode, int nID, HWND hwndCtrl)
{
	if (m_pPreviewWnd != NULL)
	{
		// for simplicity, we have four possible zoom sizes.
		int nCurrentZoom = m_pPreviewWnd->GetZoom();

		switch (nCurrentZoom)
		{
		case 800:
			nCurrentZoom = 400;
			break;
		case 400:
			nCurrentZoom = 200;
			break;
		case 200:
			nCurrentZoom = 0;
			break;
		}

		// set zoom & scrollbars and then redraw
		ZoomPrintPreview(nCurrentZoom);

		// turn on Zoom In button
		m_fZoomIn = true;

		// turn off Zoom Out button if we are at
		// full page view
		if (nCurrentZoom == 0)
			m_fZoomOut = false;

		UpdateToolBarState();
	}
}

void CPreviewFrame::OnFilePrint(UINT uCode, int nID, HWND hwndCtrl)
{
	DestroyWindow();
	::PostMessage(m_hMainWnd, WM_COMMAND, IDC_DOC_PRINT, 0L);
}

void CPreviewFrame::OnClose(UINT uCode, int nID, HWND hwndCtrl)
{
	DestroyWindow();
}


 ////******************************
 ////Helper functions
 ////******************************
void CPreviewFrame::ZoomPrintPreview(int nZoomFactor)
{
	if (m_pPreviewWnd != NULL)
	{
		// handle special case of full page view and exit early
		if (0 == nZoomFactor) 
		{
			m_pPreviewWnd->SetZoom(nZoomFactor);
			RECT rect;
			::GetClientRect(m_pPreviewWnd->m_hWnd, &rect);
			m_pPreviewWnd->SetScrollSize(rect.right, rect.bottom, FALSE);														
			m_pPreviewWnd->SetScrollOffset(0, 0);
			m_pPreviewWnd->ScrollWindowEx(0, 0, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
			return;
		}

		// handle all other cases
		int nOldZoom = m_pPreviewWnd->GetZoom();
		m_pPreviewWnd->SetZoom(nZoomFactor);

		// get current scrollbar position
		SCROLLINFO si;
		ZeroMemory(&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;
		m_pPreviewWnd->GetScrollInfo(SB_HORZ, &si);
		int x = si.nPos;

		ZeroMemory(&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;
		m_pPreviewWnd->GetScrollInfo(SB_VERT, &si);
		int y = si.nPos;

		// grab window coordinates and scale w/ zoom factor
		RECT rect;
		::GetClientRect(m_pPreviewWnd->m_hWnd, &rect);
		int scroll_size_x = rect.right + nZoomFactor; 
		int scroll_size_y = rect.bottom + nZoomFactor; 
		int old_size_x = rect.right + nOldZoom;
		int old_size_y = rect.bottom + nOldZoom;

		// calculate the new offset
		if (nOldZoom == 0)
		{
			// if previous zoom level was full page view, then set new
			// view to be at top center of the page.
			x = nZoomFactor / 2;
			y = 0;
		}
		else
		{
			// scroll to the previous position on the page
			x = (int)((float)x * (float)nZoomFactor / (float)nOldZoom);
			y = (int)((float)y * (float)nZoomFactor / (float)nOldZoom);
		}

		// scroll to maintain position
		m_pPreviewWnd->SetScrollSize(scroll_size_x, scroll_size_y, FALSE);														
		m_pPreviewWnd->SetScrollOffset(x, y);
		m_pPreviewWnd->ScrollWindowEx(-x, -y, SW_SCROLLCHILDREN | SW_ERASE | SW_INVALIDATE);
	}
}

void CPreviewFrame::SetWindowCoor(int x, int y)
{
	// add the menu bar, title bar and rebars to the Y size
	RECT window_rect, view_rect;
	GetClientRect(&window_rect);
	y += window_rect.bottom - view_rect.bottom;

	// resize the frame window to its new coordinates
	MoveWindow(0, 0, x, y); 
	CenterWindow();
}

void CPreviewFrame::SetPreviewWindow(CPrintPreviewWindowEx* pPreviewWnd)
{ 
	m_pPreviewWnd = pPreviewWnd;
	m_nMaxPage = m_pPreviewWnd->m_nMaxPage - 1;

	if (!m_nMaxPage)
		m_fNextPage = false;

	UpdateToolBarState();
}

void CPreviewFrame::UpdateToolBarState()
{
	::SendMessage(m_hWndToolbar, TB_ENABLEBUTTON, ID_ZOOM_IN, MAKELPARAM(m_fZoomIn, 0));
	::SendMessage(m_hWndToolbar, TB_ENABLEBUTTON, ID_ZOOM_OUT, MAKELPARAM(m_fZoomOut, 0));
	::SendMessage(m_hWndToolbar, TB_ENABLEBUTTON, ID_PREV_PAGE, MAKELPARAM(m_fPrevPage, 0));
	::SendMessage(m_hWndToolbar, TB_ENABLEBUTTON, ID_NEXT_PAGE, MAKELPARAM(m_fNextPage, 0));
}
