/*
// $Workfile: PANEL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PANEL.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:34   Fred
// Initial revision.
// 
//    Rev 1.9   14 Aug 1996 11:38:26   Fred
// Project Finder
// 
//    Rev 1.8   03 Aug 1996 16:43:18   Fred
//  
// 
//    Rev 1.7   02 Aug 1996 15:02:22   Fred
// 16-bit changes
// 
//    Rev 1.6   02 Aug 1996 14:23:18   FRED
// Initialize font to NULL
// 
//    Rev 1.5   02 Aug 1996 13:11:16   Fred
// More 16-bit changes
// 
//    Rev 1.4   01 Aug 1996 09:22:32   Fred
// More 16-bit changes
// 
//    Rev 1.3   31 Jul 1996 11:58:04   Fred
// 16-bit compatibility
// 
//    Rev 1.2   28 Jul 1996 07:46:00   Fred
// More album control stuff
// 
//    Rev 1.1   23 Jul 1996 08:06:50   Fred
// Uses util
// 
//    Rev 1.0   05 Jun 1996 08:54:24   Fred
// Initial revision.
*/ 

#include "stdafx.h"
#include "util.h"
#include "panel.h"

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
// CMlsPanel

WNDPROC CMlsPanel::m_pfnSuperWndProc = NULL;

CMlsPanel::CMlsPanel()
{
	m_hFont = NULL;
}

CMlsPanel::~CMlsPanel()
{
	DeleteFont();
}

void CMlsPanel::DeleteFont(void)
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

BEGIN_MESSAGE_MAP(CMlsPanel, CWnd)
	//{{AFX_MSG_MAP(CMlsPanel)
	ON_WM_CREATE()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_GETTEXT, OnGetText)
	ON_MESSAGE(WM_GETTEXTLENGTH, OnGetTextLength)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_PAINT()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMlsPanel::Initialize(UINT nID, CWnd* pParent, LPCSTR pszText /*=NULL*/)
{
	CString csText;
	TRY
	{
		CWnd* pControl = pParent->GetDlgItem(nID);
		if (pControl != NULL)
		{
			CRect crControl;
			pControl->GetWindowRect(crControl);
			pParent->ScreenToClient(crControl);
			if (pszText == NULL)
			{
				pControl->GetWindowText(csText);
			}
			else
			{
				csText = pszText;
			}
			Create(NULL, csText, pControl->GetStyle(), crControl, pParent, nID, NULL);
			SetWindowPos(pControl, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			SetFont(pControl->GetFont());
			pControl->DestroyWindow();
		}
	}
	END_TRY
}

void CMlsPanel::UnpackProperties(void)
{
	CString csText;
	CString csStyle;
	TRY
	{
		// Extract the properties.
		GetWindowText(csText);
		m_Properties.ExtractProperties(csText);
		SetWindowText(csText);
			
		// Unpack the panel's style.
		m_Properties.AsString(0, csStyle, "", TRUE);
		UnpackStyle(csStyle);
	}
	END_TRY
}

void CMlsPanel::UnpackStyle(const CString& csStyle)
{
	// Unpack the border styles (N, F, S, or R).
	m_OutBorderStyle = None;
	m_InBorderStyle = None;

	if (csStyle.GetLength() > 0)
	{
		     if (csStyle[0] == 'N') m_OutBorderStyle = None;
		else if (csStyle[0] == 'F') m_OutBorderStyle = Frame;
		else if (csStyle[0] == 'S') m_OutBorderStyle = Sunken;
		else if (csStyle[0] == 'R') m_OutBorderStyle = Raised;
		else if (csStyle[0] == 'B') m_OutBorderStyle = Bump;
		else if (csStyle[0] == 'D') m_OutBorderStyle = Dip;
		else m_OutBorderStyle = None;
	}

	if (csStyle.GetLength() > 1)
	{
		     if (csStyle[1] == 'N') m_InBorderStyle = None;
		else if (csStyle[1] == 'F') m_InBorderStyle = Frame;
		else if (csStyle[1] == 'S') m_InBorderStyle = Sunken;
		else if (csStyle[1] == 'R') m_InBorderStyle = Raised;
		else if (csStyle[1] == 'B') m_InBorderStyle = Bump;
		else if (csStyle[1] == 'D') m_InBorderStyle = Dip;
		else m_InBorderStyle = None;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMlsPanel helpers

void CMlsPanel::DrawBorder(CDC& dc, CRect& crRect, BorderStyle nStyle, BOOL fOutBorder)
{
	if (!crRect.IsRectEmpty())
	{
		switch (nStyle)
		{
			case None:
			{
				break;
			}
			case Frame:
			{
				// Draw frame border.
				//
				//    B------------B
				//    |            |
				//    |            |
				//    |            |
				//    |            |
				//    B------------B

				Util::DrawBorderWithColor(dc, crRect, GetSysColor(COLOR_BTNTEXT), 1);
				crRect.InflateRect(-1, -1);
				break;
			}
			case Sunken:
			{
				// Draw sunken 3D border:
				//
				//    D-----------DW
				//    |            |
				//    |            |
				//    |            |
				//    D            |
				//    W------------W

				CRect crSide;

				// D horizontal
				crSide.SetRect(crRect.left, crRect.top, crRect.right-1, crRect.top+1);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

				// D vertical
				crSide.SetRect(crRect.left, crRect.top+1, crRect.left+1, crRect.bottom-1);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

				// W horizontal
				crSide.SetRect(crRect.left, crRect.bottom-1, crRect.right, crRect.bottom);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

				// W vertical
				crSide.SetRect(crRect.right-1, crRect.top, crRect.right, crRect.bottom-1);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

				crRect.InflateRect(-1, -1);

				break;
			}
			case Raised:
			{
				// Draw raised 3D border:
				//
				//    W-----------WD
				//    |            |
				//    |            |
				//    |            |
				//    W            |
				//    D------------D

				CRect crSide;

				// W horizontal
				crSide.SetRect(crRect.left, crRect.top, crRect.right-1, crRect.top+1);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

				// W vertical
				crSide.SetRect(crRect.left, crRect.top+1, crRect.left+1, crRect.bottom-1);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

				// D horizontal
				crSide.SetRect(crRect.left, crRect.bottom-1, crRect.right, crRect.bottom);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

				// D vertical
				crSide.SetRect(crRect.right-1, crRect.top, crRect.right, crRect.bottom-1);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

				crRect.InflateRect(-1, -1);

				break;
			}
			case Bump:
			{
				// Draw a bump.
				//
				//    W------------W
				//    D------------D

				int nY = fOutBorder ? crRect.top : crRect.bottom-2;

				CRect crSide;

				// W horizontal
				crSide.SetRect(crRect.left, nY, crRect.right, nY+1);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

				// D horizontal
				crSide.SetRect(crRect.left, nY+1, crRect.right, nY+2);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

				if (fOutBorder)
				{
					crRect.top += 2;
				}
				else
				{
					crRect.bottom -= 2;
				}

				break;
			}
			case Dip:
			{
				// Draw a dip.
				//
				//    D------------D
				//    W------------W

				int nY = fOutBorder ? crRect.top : crRect.bottom-2;

				CRect crSide;

				// W horizontal
				crSide.SetRect(crRect.left, nY, crRect.right, nY+1);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

				// D horizontal
				crSide.SetRect(crRect.left, nY+1, crRect.right, nY+2);
				Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

				if (fOutBorder)
				{
					crRect.top += 2;
				}
				else
				{
					crRect.bottom -= 2;
				}

				break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMlsPanel message handlers


int CMlsPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (INHERITED::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CString csText;
	CString csStyle;

	TRY
	{
		// Get the control's text and extract the properties.
		SetWindowText(lpCreateStruct->lpszName);
		csText = lpCreateStruct->lpszName;
		m_Properties.ExtractProperties(csText);
		SetWindowText(csText);

		// Unpack the panel's style.
		m_Properties.AsString(0, csStyle, "", TRUE);
		UnpackStyle(csStyle);
	}
	END_TRY

	return 0;
}

LRESULT CMlsPanel::OnSetText(WPARAM wParam, LPARAM lParam)
{
	BOOL fResult = FALSE;
	TRY
	{
		if (strcmp(m_csText, (LPCSTR)lParam) != 0)
		{
			m_csText = (LPCSTR)lParam;
			Invalidate();
		}
		fResult = TRUE;
	}
	END_TRY
	return (LRESULT)fResult;
}

LRESULT CMlsPanel::OnGetText(WPARAM wParam, LPARAM lParam)
{
	int nCount = (int)wParam;
	if (nCount > 0)
	{
		LPSTR pszBuffer = (LPSTR)lParam;

		nCount--;
		if (nCount > m_csText.GetLength())
		{
			nCount = m_csText.GetLength();
		}

		if (nCount > 0)
		{
			strncpy(pszBuffer, m_csText, nCount);
		}

		pszBuffer[nCount] = '\0';
		nCount++;
	}

	return nCount;
}

LRESULT CMlsPanel::OnGetTextLength(WPARAM wParam, LPARAM lParam)
{
	return m_csText.GetLength();
}

LRESULT CMlsPanel::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	HFONT hNewFont = NULL;
	if (wParam != 0)
	{
		LOGFONT LogFont;
		if (::GetObject((HFONT)wParam, sizeof(LogFont), &LogFont) != 0)
		{
			hNewFont = CreateFontIndirect(&LogFont);
		}
	}
		
	DeleteFont();
	m_hFont = hNewFont;
	
	if (LOWORD(lParam) != 0)
	{
		Invalidate();
	}
	
	return 0;
}

LRESULT CMlsPanel::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)(UINT)m_hFont;
}

// OnPaint() is called when the panel needs to be repainted.

void CMlsPanel::OnPaint() 
{
	CPaintDC dc(this);
	DrawPanel(&dc);
}

// DrawPanel() is the main function for drawing a panel.

void CMlsPanel::DrawPanel(CDC* pDC)
{
	// Upack the state of the panel.
	BOOL fDisabled = !IsWindowEnabled();

	// Create a bitmap which will be used to draw the panel image.
	// When the bitmap is complete, it will be drawn onto the panel.
	CRect crPanel;
	GetClientRect(crPanel);

	CBitmap bmPanel;
	bmPanel.CreateCompatibleBitmap(pDC, crPanel.Width(), crPanel.Height());

	CDC dcPanel;
	dcPanel.CreateCompatibleDC(pDC);

	dcPanel.SelectObject(&bmPanel);

	// Initialize the Panel bitmap to the Panel face color.
	Util::FillRectangleWithColor(dcPanel, crPanel, GetSysColor(COLOR_BTNFACE));

	// Compute the area available for content.
	CRect crContent(crPanel);

	// Compute the text dimensions and its nominal position.
	CRect crText;
	crText.SetRectEmpty();
	CString csText;

	if (!crContent.IsRectEmpty())
	{
		GetWindowText(csText);
		if (!csText.IsEmpty())
		{
			// Get the font which we are using for the text.
			CFont* pFont = GetFont();
			if (pFont != NULL)
			{
				// Select the font we are going to use for drawing the text.
				dcPanel.SelectObject(pFont);

				// Compute the extent of the text.
				crText.SetRect(0, 0, crContent.Width(), crContent.Height());
				int nTextHeight = Util::DrawText(&dcPanel, csText, crText, DT_CALCRECT | DT_WORDBREAK);

				// Build the rectangle that has the dimensions of the text.
				crText.bottom = nTextHeight;

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
	}

	// Draw the text onto the Panel image at the computed position.
	if (!crText.IsRectEmpty())
	{
		// Draw the text.
		dcPanel.SetTextColor(GetSysColor(COLOR_BTNTEXT));
		dcPanel.SetBkMode(TRANSPARENT);
		Util::DrawText(&dcPanel, csText, crText, DT_CENTER | DT_WORDBREAK);
	}

	// If the Panel is disabled, indicate this by embossing the bitmap.
	if (fDisabled)
	{
		Util::Emboss(dcPanel, crPanel, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHILIGHT));
	}

	// Draw the panel border(s).
	CRect crDraw(crPanel);
	DrawBorder(dcPanel, crDraw, m_OutBorderStyle, TRUE);
	DrawBorder(dcPanel, crDraw, m_InBorderStyle, FALSE);

	// Copy the bitmap onto the Panel.
	pDC->BitBlt(crPanel.left, crPanel.top, crPanel.Width(), crPanel.Height(), &dcPanel, 0, 0, SRCCOPY);

	dcPanel.DeleteDC();
	bmPanel.DeleteObject();
}

void CMlsPanel::OnEnable(BOOL bEnable) 
{
	// WM_ENABLE is send AFTER the WS_DISABLED bit is set.
	if (bEnable == IsWindowEnabled())
	{
		Default();
		Invalidate();
	}
}
