/*
// $Workfile: PROGRESS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROGRESS.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:08   Fred
// Initial revision.
// 
//    Rev 1.4   05 Aug 1996 18:55:32   Fred
//  
// 
//    Rev 1.3   01 Aug 1996 09:23:10   Fred
// More 16-bit changes
// 
//    Rev 1.2   31 Jul 1996 11:56:54   Fred
// 16-bit compatibility
// 
//    Rev 1.1   17 Jul 1996 13:55:22   FRED
// 16-bit changes
// 
//    Rev 1.0   09 Jul 1996 16:33:20   Fred
// Initial revision.
*/ 

#include "stdafx.h"
#include "util.h"
#include "progress.h"

#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define	COLOR_BTNHILIGHT	COLOR_BTNHIGHLIGHT
#endif
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMlsProgressBar

CMlsProgressBar::CMlsProgressBar()
{
	m_clBackground = GetSysColor(COLOR_WINDOW);
	m_clBackgroundText = GetSysColor(COLOR_WINDOWTEXT);
	m_clForeground = m_clBackgroundText;
	m_clForegroundText = m_clBackground;

	m_dPosition = 0.0;
	m_dMinimum = 0.0;
	m_dMaximum = 0.0;
}

CMlsProgressBar::~CMlsProgressBar()
{
}

BEGIN_MESSAGE_MAP(CMlsProgressBar, CWnd)
	//{{AFX_MSG_MAP(CMlsProgressBar)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMlsProgressBar::Initialize(UINT nID, CWnd* pParent)
{
	CWnd* pControl = pParent->GetDlgItem(nID);
	if (pControl != NULL)
	{
		CRect crControl;
		pControl->GetWindowRect(crControl);
		pParent->ScreenToClient(crControl);
		Create(NULL, "", pControl->GetStyle(), crControl, pParent, nID, NULL);
		SetWindowPos(pControl, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		pControl->GetWindowText(m_csFormat);
		pControl->DestroyWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMlsProgressBar helpers

// DrawProgressBar() is the main function for drawing a progress bar.

void CMlsProgressBar::DrawProgressBar(CDC* pDC)
{
	// Create a bitmap which will be used to draw the progress bar image.
	// When the bitmap is complete, it will be drawn onto the progress bar.
	CRect crProgressBar;
	GetClientRect(crProgressBar);

	CBitmap bmProgressBar;
	bmProgressBar.CreateCompatibleBitmap(pDC, crProgressBar.Width(), crProgressBar.Height());

	CDC dcProgressBar;
	dcProgressBar.CreateCompatibleDC(pDC);

	dcProgressBar.SelectObject(&bmProgressBar);

	// Compute the area available for content.
	CRect crContent(crProgressBar);

	// Compute the text dimensions and its nominal position.
	CRect crText;
	crText.SetRectEmpty();
	CString csText;

	if (!crContent.IsRectEmpty())
	{
		double dPercent;

		// Reformat the text to include the percentage.
		if (m_dPosition <= m_dMinimum)
		{
			dPercent = 0.0;
		}
		else if (m_dPosition >= m_dMaximum)
		{
			dPercent = 1.0;
		}
		else
		{
			dPercent = (m_dPosition-m_dMinimum)/(m_dMaximum-m_dMinimum);
		}

		csText.Format(m_csFormat, (int)((dPercent*100.0)+0.5));

		if (!csText.IsEmpty())
		{
			// Get the font which we are using for the text.
			CFont* pFont = GetParent()->GetFont();
			if (pFont != NULL)
			{
				// Select the font we are going to use for drawing the text.
				dcProgressBar.SelectObject(pFont);

				// Compute the extent of the text.
				crText.SetRect(0, 0, crContent.Width(), crContent.Height());
#ifdef WIN32
				CSize czText = dcProgressBar.GetTextExtent(csText);
#else
				CSize czText = dcProgressBar.GetTextExtent((LPCSTR)csText, csText.GetLength());
#endif
				crText.right = czText.cx;
				crText.bottom = czText.cy;

				// Clip to content dimensions.
				if (crText.Width() > crContent.Width())
				{
					crText.right = crContent.Width();
				}
				if (crText.Height() > crContent.Height())
				{
					crText.bottom = crContent.Height();
				}

				// Center text rectangle within content rectangle.
				crText.OffsetRect(
					crContent.left+(crContent.Width()-crText.Width())/2,
					crContent.top+(crContent.Height()-crText.Height())/2);
			}
		}

		// Draw the progress colors in the bitmap.
		int nProgress = (int)((((double)crContent.Width())*dPercent)+0.5);

		// Draw the foreground color.
		CRect crForeground(crContent);
		crForeground.right = nProgress;
		Util::FillRectangleWithColor(dcProgressBar, crForeground, m_clForeground);

		// Draw the background color.
		CRect crBackground(crContent);
		crBackground.left = nProgress;
		Util::FillRectangleWithColor(dcProgressBar, crBackground, m_clBackground);

		// Draw text.
		if (!csText.IsEmpty())
		{
			dcProgressBar.SetBkMode(TRANSPARENT);
			dcProgressBar.SetTextColor(m_clForegroundText);
#ifdef WIN32
			dcProgressBar.ExtTextOut(crText.left, crText.top, ETO_CLIPPED, crForeground, csText, NULL);
#else
			dcProgressBar.ExtTextOut(crText.left, crText.top, ETO_CLIPPED, crForeground, (LPCSTR)csText, csText.GetLength(), NULL);
#endif
			dcProgressBar.SetTextColor(m_clBackgroundText);
#ifdef WIN32
			dcProgressBar.ExtTextOut(crText.left, crText.top, ETO_CLIPPED, crBackground, csText, NULL);
#else
			dcProgressBar.ExtTextOut(crText.left, crText.top, ETO_CLIPPED, crBackground, (LPCSTR)csText, csText.GetLength(), NULL);
#endif
		}

		// Draw the progress bar border.
		CRect crBorder(crProgressBar);
		DrawBorder(dcProgressBar, crBorder);

		// Copy the bitmap onto the progress bar.
		pDC->BitBlt(crProgressBar.left, crProgressBar.top, crProgressBar.Width(), crProgressBar.Height(), &dcProgressBar, 0, 0, SRCCOPY);

		dcProgressBar.DeleteDC();
		bmProgressBar.DeleteObject();
	}
}

void CMlsProgressBar::DrawBorder(CDC& dc, CRect& crRect)
{
	// Draw the sunken 3D border:
	//
	//    D-----------DW
	//    |B---------BL|
	//    ||          ||
	//    ||          ||
	//    |B          ||
	//    DL----------L|
	//    W------------W

	int nWidth = crRect.Width();
	int nHeight = crRect.Height();

	if ((nWidth >= 2) && (nHeight >= 2))
	{
		CRect crSide;

		// D horizontal
		crSide.SetRect(0, 0, nWidth-1, 1);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

		// D vertical
		crSide.SetRect(0, 1, 1, nHeight-1);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

		// B horizontal
		crSide.SetRect(1, 1, nWidth-2, 2);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

		// B vertical
		crSide.SetRect(1, 2, 2, nHeight-2);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

		// W horizontal
		crSide.SetRect(0, nHeight-1, nWidth, nHeight);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// W vertical
		crSide.SetRect(nWidth-1, 0, nWidth, nHeight-1);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// L horizontal
		crSide.SetRect(1, nHeight-2, nWidth-1, nHeight-1);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNFACE));

		// L vertical
		crSide.SetRect(nWidth-2, 1, nWidth-1, nHeight-2);
		Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNFACE));
	}
}

void CMlsProgressBar::SetRange(double dMinimum, double dMaximum)
{
	m_dMinimum = dMinimum;
	m_dMaximum = dMaximum;
	Invalidate();
}

void CMlsProgressBar::SetPosition(double dPosition)
{
	m_dPosition = dPosition;

	// Force a redraw.
	Invalidate();
	UpdateWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CMlsProgressBar message handlers

// OnPaint() is called when the progress bar needs to be repainted.

void CMlsProgressBar::OnPaint() 
{
	CPaintDC dc(this);
	DrawProgressBar(&dc);
}
