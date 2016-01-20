/*
// $Workfile: SPLITTER.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPLITTER.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:34   Fred
// Initial revision.
// 
//    Rev 1.0   24 Jun 1996 00:31:18   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "resource.h"
#include "splitter.h"

//////////////////////////////////////////////////////////////////////////
// CSplitterCursor

CSplitterCursor::CSplitterCursor()
{
	m_pWnd = NULL;
	m_fShowing = FALSE;
	m_fDragging = FALSE;
	m_nX = 0;
	m_nOffset = 0;
	m_hcSplitter = AfxGetApp()->LoadCursor(IDC_SPLITTER_CURSOR);
	m_hcPrevious = NULL;
}

CSplitterCursor::~CSplitterCursor()
{
}

void CSplitterCursor::StartShow(void)
{
	if (m_hcSplitter != NULL)
	{
		HCURSOR hcPrevious = SetCursor(m_hcSplitter);
		if (!m_fShowing)
		{
			m_hcPrevious = hcPrevious;
			m_fShowing = TRUE;
		}
	}
}

void CSplitterCursor::EndShow(void)
{
	if (m_fShowing)
	{
		SetCursor(m_hcPrevious);
		m_fShowing = FALSE;
	}
}

void CSplitterCursor::StartDrag(int nX, int nOffset, int nClipLeft, int nClipRight)
{
	if (!m_fDragging)
	{
		m_pWnd->SetCapture();
		m_fDragging = TRUE;
		m_nOffset = nOffset;
		m_nClipLeft = nClipLeft;
		m_nClipRight = nClipRight;
		SetX(nX);
		DrawDrag();
	}
}

void CSplitterCursor::Drag(int nX)
{
	if (m_fDragging)
	{
		DrawDrag();
		SetX(nX);
		DrawDrag();
	}
}

void CSplitterCursor::EndDrag(int nX)
{
	if (m_fDragging)
	{
		DrawDrag();
		SetX(nX);
		ReleaseCapture();
		m_fDragging = FALSE;
	}
}

void CSplitterCursor::SetX(int nX)
{
	m_nX = nX+m_nOffset;
	if (m_nX < m_nClipLeft)
	{
		m_nX = m_nClipLeft;
	}
	if (m_nX > m_nClipRight)
	{
		m_nX = m_nClipRight;
	}
}

void CSplitterCursor::DrawDrag(void)
{
	CDC* pDC = m_pWnd->GetDC();
	if (pDC != NULL)
	{
		CRect crRect;
		m_pWnd->GetClientRect(&crRect);
		crRect.left = m_nX-1;
		crRect.right = crRect.left+1;

		CBrush cbDrag;
		cbDrag.CreateStockObject(LTGRAY_BRUSH);
		pDC->SelectObject(&cbDrag);

		pDC->PatBlt(crRect.left, crRect.top, crRect.Width(), crRect.Height(), PATINVERT);

		m_pWnd->ReleaseDC(pDC);
	}
}
