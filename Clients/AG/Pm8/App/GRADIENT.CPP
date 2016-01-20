/*
// $Workfile: GRADIENT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/GRADIENT.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:33:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:38   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:36:26   JAY
// Reverted. New series.
// 
//    Rev 1.3   22 Jun 1994 18:54:22   FRED
// Does not include "INSTALL.H"
// 
// 
//    Rev 1.2   22 Jun 1994 11:06:06   FRED
// Fixed bug where SetColor() could call
// InvalidateRect() will null window handle.
// 
// 
//    Rev 1.1   21 Jun 1994 14:40:56   FRED
// Now derived from CFrameWnd instead of CWnd.
// 
// 
//    Rev 1.0   15 Jun 1994 19:00:52   FRED
// Initial revision.
*/ 

#pragma warning(disable:4100)
#pragma warning(disable:4505)
#pragma warning(disable:4704)

#ifndef NOSTRICT
#define STRICT
#endif

#include "stdafx.h"
#include "gradient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGradientWindow

CGradientWindow::CGradientWindow()
{
	m_bStartRed   = 0;
	m_bStartGreen = 0;
	m_bStartBlue  = 0;
	m_bEndRed     = 0;
	m_bEndGreen   = 0;
	m_bEndBlue    = 0;
}

CGradientWindow::~CGradientWindow()
{
}

void CGradientWindow::SetColor(COLORREF crStartColor, COLORREF crEndColor)
{
	m_bStartRed   = GetRValue(crStartColor);
	m_bStartGreen = GetGValue(crStartColor);
	m_bStartBlue  = GetBValue(crStartColor);
	m_bEndRed     = GetRValue(crEndColor);
	m_bEndGreen   = GetGValue(crEndColor);
	m_bEndBlue    = GetBValue(crEndColor);
	
	if (m_hWnd != NULL)
	{
		InvalidateRect(NULL);
	}
}

BOOL CGradientWindow::Create(
	DWORD dwExtStyle,
	LPCSTR lpszClassName,
	LPCSTR lpszWindowName,
	DWORD dwStyle,
	const RECT& rcPosition,
	CWnd* pParentWnd,
	UINT nID,
	CCreateContext* pContext)
{
	if (lpszClassName == NULL)
	{
		lpszClassName = "AfxFrameOrView";
	}
		
	return CreateEx(
		dwExtStyle,
		lpszClassName,
		lpszWindowName,
		dwStyle,
		rcPosition.left,
		rcPosition.top,
		rcPosition.right-rcPosition.left,
		rcPosition.bottom-rcPosition.top,
		pParentWnd->GetSafeHwnd(),
		(HMENU)nID,
		(LPSTR)pContext);
}
		
BEGIN_MESSAGE_MAP(CGradientWindow, CFrameWnd)
	//{{AFX_MSG_MAP(CGradientWindow)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCDESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGradientWindow message handlers


void CGradientWindow::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting

	CRect crWindow;
	GetClientRect(crWindow);
			
	CRect crBand(dcPaint.m_ps.rcPaint);
	crBand.bottom = crBand.top;
	
	COLORREF crCurrentColor = RGB(0,0,0);
	COLORREF crNextColor;
	CBrush Brush;
		
	for (int y = dcPaint.m_ps.rcPaint.top; y < dcPaint.m_ps.rcPaint.bottom; y++)
	{
		crNextColor = RGB(ScaleColor(m_bStartRed, m_bEndRed, crWindow.top, y, crWindow.bottom-1),
								ScaleColor(m_bStartGreen, m_bEndGreen, crWindow.top, y, crWindow.bottom-1),
								ScaleColor(m_bStartBlue, m_bEndBlue, crWindow.top, y, crWindow.bottom-1));
		
		if (crNextColor != crCurrentColor)
		{
			if (!crBand.IsRectEmpty())
			{
				Brush.CreateSolidBrush(crCurrentColor);
				dcPaint.FillRect(crBand, &Brush);
				Brush.DeleteObject();
				crBand.top = crBand.bottom;
			}
			
			crCurrentColor = crNextColor;
		}
		
		crBand.bottom++;
	}
	
	if (!crBand.IsRectEmpty())
	{
		Brush.CreateSolidBrush(crCurrentColor);
		dcPaint.FillRect(crBand, &Brush);
		Brush.DeleteObject();
	}
}

BOOL CGradientWindow::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CGradientWindow::OnNcDestroy()
{
	if (AfxGetApp()->m_pMainWnd == this)
	{
		PostQuitMessage(0);
	}
	
	CFrameWnd::OnNcDestroy();
}

BYTE CGradientWindow::ScaleColor(BYTE bStart, BYTE bEnd, int nMinimum, int nCurrent, int nMaximum)
{
	if (bStart == bEnd)
	{
		return bStart;
	}
	
	if (bStart < bEnd)
	{
		return bStart+MulDiv(bEnd-bStart, nCurrent-nMinimum, nMaximum);
	}
	
	else
	{
		return bStart-MulDiv(bStart-bEnd, nCurrent-nMinimum, nMaximum);
	}
}
