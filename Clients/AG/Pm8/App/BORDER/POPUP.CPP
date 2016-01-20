/*
// $Header: /PM8/App/BORDER/POPUP.CPP 1     3/03/99 6:03p Gbeddow $
//
// popup.h : Pop-up window classes
//
// $Log: /PM8/App/BORDER/POPUP.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 1     4/22/98 11:44a Fredf
*/

#include "stdafx.h"
#include "border.h"
#include "popup.h"
#include "dibbrush.h"

BOOL Draw3dBorder(
      CDC* pDC,
      const CRect& crBorder,
      COLORREF clHighlight,
      COLORREF clShadow,
      COLORREF clBorderHighlight,
      COLORREF clBorderShadow,
      COLORREF clBackground,
      int nDepth /*=2*/,
      BOOL fDrawInsideBorder /*=FALSE*/,
      BOOL fDrawBackground /*=FALSE*/,
      BOOL fMask /*=FALSE*/)
{
   BOOL fResult = FALSE;
   int nContext = 0;
   CPen cpHighlight;
   CPen cpShadow;
   CPen cpBorderHighlight;
   CPen cpBorderShadow;
   CBrush cbBackground;
   
   TRY
   {
      if (pDC != NULL)
      {
         nContext = pDC->SaveDC();
         
         if (nContext != 0)
         {
            int i;
            
            // Draw 3D border.
            
            if (nDepth > 0)
            {
               // Draw shadow.
                        
               cpShadow.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clShadow);
               pDC->SelectObject(&cpShadow);
               
               for (i = 0; i < nDepth; i++)
               {
                  pDC->MoveTo(crBorder.left, crBorder.top+i);
                  pDC->LineTo(crBorder.right-1-i, crBorder.top+i);
               
                  pDC->MoveTo(crBorder.left+i, crBorder.top+nDepth);
                  pDC->LineTo(crBorder.left+i, crBorder.bottom-1-i);
               }
               
               // Draw highlight.
               
               cpHighlight.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clHighlight);
               pDC->SelectObject(&cpHighlight);
               
               for (i = 0; i < nDepth; i++)
               {
                  pDC->MoveTo(crBorder.left+i, crBorder.bottom-1-i);
                  pDC->LineTo(crBorder.right, crBorder.bottom-1-i);
               
                  pDC->MoveTo(crBorder.right-1-i, crBorder.top+i);
                  pDC->LineTo(crBorder.right-1-i, crBorder.bottom-nDepth);
               }
            }
            
            if (fDrawInsideBorder)
            {
               // Draw inside border.
            
               cpBorderShadow.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clBorderShadow);
               pDC->SelectObject(&cpBorderShadow);
            
               pDC->MoveTo(crBorder.left+nDepth, crBorder.top+nDepth);
               pDC->LineTo(crBorder.right-1-nDepth, crBorder.top+nDepth);
               
               pDC->MoveTo(crBorder.left+nDepth, crBorder.top+nDepth+1);
               pDC->LineTo(crBorder.left+nDepth, crBorder.bottom-nDepth-1);
               
               cpBorderHighlight.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clBorderHighlight);
               pDC->SelectObject(&cpBorderHighlight);
               
               pDC->MoveTo(crBorder.left+nDepth, crBorder.bottom-nDepth-1);
               pDC->LineTo(crBorder.right-nDepth, crBorder.bottom-nDepth-1);
               
               pDC->MoveTo(crBorder.right-nDepth-1, crBorder.top+nDepth);
               pDC->LineTo(crBorder.right-nDepth-1, crBorder.bottom-nDepth-1);
            }
            
            if (fDrawBackground)
            {
               // Fill background.
               
               if (cbBackground.CreateSolidBrush(clBackground))
               {
                  CRect crBackground(crBorder);
                  int nOffset = nDepth+(fDrawInsideBorder ? 1 : 0);
                  crBackground.InflateRect(-nOffset, -nOffset);
                  pDC->FillRect(crBackground, &cbBackground);
               }
            }
            
            pDC->RestoreDC(nContext);
            nContext = 0;
            
            fResult = TRUE;
         }
      }
   }
   CATCH_ALL(e)
   {
      fResult = FALSE;
      
      if (nContext != 0)
      {
         pDC->RestoreDC(nContext);
         nContext = 0;
      }
   }
   END_CATCH_ALL
   
   return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CPopupWnd

// The global font for popup windows.
CFont CPopupWnd::m_Font;

BEGIN_MESSAGE_MAP(CPopupWnd, CWnd)
	//{{AFX_MSG_MAP(CPopupWnd)
	ON_WM_KILLFOCUS()
   ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
// Constructor.
*/

CPopupWnd::CPopupWnd()
{
}

/*
// Base-level create method for a popup window.
*/

BOOL CPopupWnd::Create(CRect& r, CWnd* pParent)
{
	return CWnd::CreateEx(0,
								 ::AfxRegisterWndClass(CS_SAVEBITS | CS_DBLCLKS,
                                               AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
								 NULL,
								 WS_BORDER | WS_POPUP,
								 r.left, r.top, r.Width(), r.Height(),
								 pParent->GetSafeHwnd(),
								 (HMENU)0);
}

/*
// Kill focus handler.
*/

void CPopupWnd::OnKillFocus(CWnd* pNewWnd)
{
	// We want the window to go away.
	OnCancel();
}

/*
// Erase background handler.
*/

BOOL CPopupWnd::OnEraseBkgnd(CDC* pDC)
{
   CBrush Brush;

   if (Brush.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE)))
   {
      CRect crClient;
      GetClientRect(crClient);

      pDC->FillRect(crClient, &Brush);

      Brush.DeleteObject();
   }

   return TRUE;
}

/*
// Base-level OnCancel handler.
*/

void CPopupWnd::OnCancel(void)
{
   CWnd  *pCWndOwner =  GetOwner();

	// Just take us down.
	DestroyWindow();

	// We do not want the capture anymore.
	if (GetCapture() == this)
	{
		ReleaseCapture();
	}
   if(pCWndOwner)
      pCWndOwner->SendMessage(WM_CANCELMODE);
}

/*
// Get the popup window font.
*/

CFont* CPopupWnd::GetPopupFont(void)
{
	if (m_Font.GetSafeHandle() == NULL)
	{
		LOGFONT logfont;
      memset(&logfont, 0, sizeof(logfont));
		// 8 point height Sans Serif font
		logfont.lfHeight = -8;
		logfont.lfWeight = FW_NORMAL;
		logfont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
		static char BASED_CODE szFaceName[] = "MS Sans Serif";
		lstrcpy(logfont.lfFaceName, szFaceName);

		VERIFY(m_Font.CreateFontIndirect(&logfont));
	}
	return &m_Font;
}

/*
// Get the metrics for the popup window font.
*/

void CPopupWnd::GetPopupFontMetrics(TEXTMETRIC& tm)
{
	CFont* pFont = GetPopupFont();
	ASSERT(pFont != NULL);

	CClientDC dc(NULL);		// May happen before creation.
	CFont* pOldFont = dc.SelectObject(pFont);
	if (pOldFont != NULL)
	{
		dc.GetTextMetrics(&tm);
		dc.SelectObject(pOldFont);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPopupPalette

BEGIN_MESSAGE_MAP(CPopupPalette, CPopupWnd)
	//{{AFX_MSG_MAP(CPopupPalette)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPopupPalette::CPopupPalette()
{
	m_nSelectedItem = -1;
	m_uID = 0;
	m_hPalette = NULL;
}

BOOL CPopupPalette::SetPalette(HPALETTE hPalette)
{
	m_hPalette = hPalette;
	return TRUE;
}

int CPopupPalette::SizeNone(int nDefaultHeight)
{
	return nDefaultHeight;
}

int CPopupPalette::SizeMore(int nDefaultHeight)
{
	return nDefaultHeight;
}

int CPopupPalette::SizeColor(int nDefaultHeight)
{
	return nDefaultHeight;
}

BOOL CPopupPalette::Create(CWnd* pParent, UINT uID /*=0*/)
{
	// Compute the size of the palette.

	InitItems(pParent);

	CPoint cpCount = ItemCount();
	CSize czSize = ItemSize();

	TEXTMETRIC tm;
	GetPopupFontMetrics(tm);

	int nGap = 4;
	int nFontHeight = tm.tmHeight + 2*4;

	int nNoneBaseHeight = m_fHasNone ? SizeNone(nFontHeight) : 0;
	int nMoreBaseHeight = m_fHasMore ? SizeMore(nFontHeight) : 0;
	int nColorBaseHeight = m_fHasColor ? SizeColor(nFontHeight) : 0;

	int nNoneHeight = nNoneBaseHeight;
	int nMoreHeight = nMoreBaseHeight;
	int nColorHeight = nColorBaseHeight;

	CRect r;
	r.left = 0;
	r.top = 0;
	r.right = cpCount.x * czSize.cx + 2*nGap;
	r.bottom = nGap + nColorHeight + nNoneHeight + cpCount.y * czSize.cy + nMoreHeight + nGap;

	// Set the items rectangle.
	m_crItems.left = r.left + nGap;
	m_crItems.top = r.top + nGap + nColorHeight + nNoneHeight;
	m_crItems.right = r.right - nGap;
	m_crItems.bottom = r.bottom - nMoreHeight - nGap;

	// Set the Color rectangle.
	if (m_fHasColor)
	{
		m_crColor.left = r.left + nGap;
		m_crColor.top = nGap;
		m_crColor.right = r.right - nGap;
		m_crColor.bottom = m_crColor.top + nColorBaseHeight;
	}

	// Set the None rectangle.
	if (m_fHasNone)
	{
		m_crNone.left = r.left + nGap;
		m_crNone.top = m_crItems.top - nNoneHeight;
		m_crNone.right = r.right - nGap;
		m_crNone.bottom = m_crNone.top + nNoneBaseHeight;
	}

	// Set the More rectangle.
	if (m_fHasMore)
	{
		m_crMore.left = r.left + nGap;
		m_crMore.top = m_crItems.bottom;
		m_crMore.right = r.right - nGap;
		m_crMore.bottom = m_crMore.top + nMoreBaseHeight;
	}

	m_uID = uID;

	return CPopupWnd::Create(r, pParent);
}

/*
// OnPaint() is called when the window needs to be repainted.
*/

void CPopupPalette::OnPaint() 
{
	CPaintDC dc(this);

	CPoint cpCount = ItemCount();
	CSize czSize = ItemSize();

	CRect crDraw;
	int nItem;

	CPalette* pOldPal = NULL;
	if (m_hPalette != NULL)
	{
		pOldPal = dc.SelectPalette(CPalette::FromHandle(m_hPalette), FALSE);
		dc.RealizePalette();
	}

   DrawItemBackground(&dc);

   CPoint cpItem;

	for (nItem = 0, cpItem.y = 0, crDraw.top = m_crItems.top;
				cpItem.y < cpCount.y;
				cpItem.y++, crDraw.top += czSize.cy)
	{
		for (cpItem.x = 0, crDraw.left = m_crItems.left;
					cpItem.x < cpCount.x;
					cpItem.x++, crDraw.left += czSize.cx)
		{
			crDraw.right = crDraw.left + czSize.cx;
			crDraw.bottom = crDraw.top + czSize.cy;

			// Draw the item.
			DrawItem(&dc, nItem, crDraw, FALSE /*nItem == m_nSelectedItem*/);
			nItem++;
		}
	}

	if (m_fHasColor)
	{
		DrawItem(&dc, ITEM_Color, m_crColor, FALSE);
	}

	if (m_fHasNone)
	{
		DrawItem(&dc, ITEM_None, m_crNone, FALSE);
	}

	if (m_fHasMore)
	{
		DrawItem(&dc, ITEM_More, m_crMore, FALSE);
	}

	// Handle the selected item.
	if (m_nSelectedItem != -1)
	{
		RedrawItem(m_nSelectedItem, &dc);
	}

	if (pOldPal != NULL)
	{
		dc.SelectPalette(pOldPal, FALSE);
	}
}

void CPopupPalette::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_fHasColor && m_crColor.PtInRect(point))
	{
		SelectItem(ITEM_Color);
	}
	else if (m_fHasNone && m_crNone.PtInRect(point))
	{
		SelectItem(ITEM_None);
	}
	else if (m_fHasMore && m_crMore.PtInRect(point))
	{
		SelectItem(ITEM_More);
	}
	else if (m_crItems.PtInRect(point))
	{
		CSize czSize = ItemSize();

		SelectItem(CPoint((point.x - m_crItems.left)/czSize.cx,
								(point.y - m_crItems.top)/czSize.cy));
	}
	else
	{
		SelectItem(-1);
	}
}

void CPopupPalette::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Capture the mouse until the button is released.
	SetCapture();
}

void CPopupPalette::OnLButtonUp(UINT nFlags, CPoint point)
{
	// The user let go. Release the capture we have.
	ReleaseCapture();

	// If something is selected do OnOK.
	if (m_nSelectedItem == -1)
	{
		OnCancel();
	}
	else
	{
		OnOK();
	}
}

void CPopupPalette::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CPoint cpSelect = ItemToXY(m_nSelectedItem);
	int nNewSelect = m_nSelectedItem;
	CPoint cpCount = ItemCount();

	switch (nChar)
	{
		case VK_RETURN:
		{
			// Done!
			OnOK();
			break;
		}
		case VK_ESCAPE:
		{
			// Done!
			OnCancel();
			break;
		}
		case VK_UP:
		{
			if (m_nSelectedItem == ITEM_Color)
			{
				// Nothing to do.
			}
			else if (m_nSelectedItem == ITEM_None)
			{
				if (m_fHasColor)
				{
					nNewSelect = ITEM_Color;
				}
			}
			else if (m_fHasMore && m_nSelectedItem == -1)
			{
				nNewSelect = ITEM_More;
			}
			else if (cpSelect.y == 0)
			{
				if (m_fHasNone)
				{
					nNewSelect = ITEM_None;
				}
			}
			else
			{
				if (m_nSelectedItem == -1 || m_nSelectedItem == ITEM_More)
				{
					cpSelect.y = cpCount.y;
				}
				cpSelect.y--;
				if (cpSelect.x < 0)
				{
					cpSelect.x = 0;
				}
				nNewSelect = XYToItem(cpSelect);
			}
			break;
		}
		case VK_DOWN:
		{
			if (m_nSelectedItem == ITEM_More)
			{
				// Nothing to do.
			}
			else if (m_fHasColor && m_nSelectedItem == -1)
			{
				nNewSelect = ITEM_Color;
			}
			else if (m_fHasNone && m_nSelectedItem == -1)
			{
				nNewSelect = ITEM_None;
			}
			else if (m_fHasNone && m_nSelectedItem == ITEM_Color)
			{
				nNewSelect = ITEM_None;
			}
			else if (cpSelect.y == cpCount.y-1)
			{
				if (m_fHasMore)
				{
					nNewSelect = ITEM_More;
				}
			}
			else
			{
				if (m_nSelectedItem == ITEM_Color || m_nSelectedItem == ITEM_None || m_nSelectedItem == -1)
				{
					cpSelect.y = -1;
				}

				if (cpSelect.y < cpCount.y-1)
				{
					cpSelect.y++;
					if (cpSelect.x < 0)
					{
						cpSelect.x = 0;
					}
				}
				nNewSelect = XYToItem(cpSelect);
			}
			break;
		}
		case VK_LEFT:
		{
			if (cpSelect.x == 0 || m_nSelectedItem >= ITEM_None)
			{
				// Nothing to do.
			}
			else
			{
				if (cpSelect.x < 0)
				{
					cpSelect.x = cpCount.x;
				}
				cpSelect.x--;
				if (cpSelect.y < 0)
				{
					cpSelect.y = 0;
				}
				nNewSelect = XYToItem(cpSelect);
			}
			break;
		}
		case VK_RIGHT:
		{
			if (cpSelect.x == cpCount.x-1 || m_nSelectedItem >= ITEM_None)
			{
				// Nothing to do.
			}
			else
			{
				cpSelect.x++;
				if (cpSelect.y < 0)
				{
					cpSelect.y = 0;
				}
				nNewSelect = XYToItem(cpSelect);
			}
			break;
		}
		default:
		{
			CPopupWnd::OnKeyDown(nChar, nRepCnt, nFlags);
			break;
		}
	}

	if (nNewSelect != m_nSelectedItem)
	{
		SelectItem(nNewSelect);
	}
}

void CPopupPalette::SelectItem(CPoint cpItem)
{
	CPoint cpCount = ItemCount();

	if (cpItem.x < 0) cpItem.x = 0;
	else if (cpItem.x >= cpCount.x) cpItem.x = cpCount.x-1;

	if (cpItem.y < 0) cpItem.y = 0;
	else if (cpItem.y >= cpCount.y) cpItem.y = cpCount.y-1;

	SelectItem(XYToItem(cpItem));
}

void CPopupPalette::SelectItem(int nSelection)
{
	int nOldSelection = m_nSelectedItem;
	m_nSelectedItem = nSelection;

	if (m_nSelectedItem != nOldSelection)
	{
		RedrawItem(nOldSelection);
		RedrawItem(m_nSelectedItem);
	}
}

void CPopupPalette::RedrawItem(int nItem, CDC* pDC /*=NULL*/)
{
	CPoint cpCount = ItemCount();

	if (nItem >= ITEM_None || (nItem >= 0 && nItem < cpCount.x*cpCount.y))
	{
		CRect crDraw;

		if (nItem == ITEM_None)
		{
			ASSERT(m_fHasNone);
			crDraw = m_crNone;
		}
		else if (nItem == ITEM_More)
		{
			ASSERT(m_fHasMore);
			crDraw = m_crMore;
		}
		else if (nItem == ITEM_Color)
		{
			ASSERT(m_fHasColor);
			crDraw = m_crColor;
		}
		else
		{
			CSize czSize = ItemSize();

			CPoint p = ItemToXY(nItem);

			crDraw.left = m_crItems.left + p.x*czSize.cx;
			crDraw.top = m_crItems.top + p.y*czSize.cy;
			crDraw.right = crDraw.left + czSize.cx;
			crDraw.bottom = crDraw.top + czSize.cy;
		}

		CClientDC dc(this);
		CPalette* pOldPal = NULL;
		if (pDC == NULL)
		{
			pDC = &dc;
			if (m_hPalette != NULL)
			{
				pOldPal = dc.SelectPalette(CPalette::FromHandle(m_hPalette), FALSE);
				dc.RealizePalette();
			}
		}

		DrawItem(pDC, nItem, crDraw, nItem == m_nSelectedItem);

		if (pOldPal != NULL)
		{
			dc.SelectPalette(pOldPal, FALSE);
		}
	}
}

CPoint CPopupPalette::ItemToXY(int nItem)
{
	CPoint cpCount = ItemCount();
	if (nItem < 0 || nItem >= cpCount.x*cpCount.y)
	{
		return CPoint(-1, -1);
	}

	return CPoint(nItem % cpCount.x, nItem / cpCount.x);
}

int CPopupPalette::XYToItem(CPoint p)
{
	return p.y*ItemCount().x + p.x;
}

void CPopupPalette::OnOK(void)
{
	// Let our owner know that a value has been set.
	NotifyOwner();
	// And take the window down.
	DestroyWindow();
}

void CPopupPalette::NotifyOwner(void)
{
	CWnd* pOwner = GetOwner();

	ASSERT(m_uID != 0);

	if (pOwner != NULL && m_uID != 0)
	{
	   pOwner->SendMessage(WM_COMMAND, MAKEWPARAM(m_uID, 0), (LPARAM)(GetSafeHwnd()));
	}
}

void CPopupPalette::DrawString(CDC* pDC, CString& csText, CRect crDraw, int nMethod)
{
   CFont* pOldFont = pDC->SelectObject(GetPopupFont());
   if (pOldFont != NULL)
   {
      pDC->DrawText(csText,
					     csText.GetLength(),
					     crDraw,
					     nMethod);
      pDC->SelectObject(pOldFont);
   }
}

void CPopupPalette::DrawNone(CDC* pDC, CRect crDraw)
{
	CString csText;
	TRY
		csText.LoadString(IDS_PaletteNone);
	END_TRY

	DrawString(pDC, csText, crDraw, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CPopupPalette::DrawMore(CDC* pDC, CRect crDraw)
{
	CString csText;
	TRY
		csText.LoadString(IDS_PaletteMore);
	END_TRY

	DrawString(pDC, csText, crDraw, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CPopupPalette::DrawColor(CDC* pDC, CRect crDraw)
{
	CString csText;
	TRY
		csText.LoadString(IDS_PaletteColor);
	END_TRY

	DrawString(pDC, csText, crDraw, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void CPopupPalette::OnMore(void)
{
	ASSERT(!m_fHasMore);
}

void CPopupPalette::OnColor(void)
{
	ASSERT(!m_fHasColor);
}

/////////////////////////////////////////////////////////////////////////////
// CColorPalette

BEGIN_MESSAGE_MAP(CColorPalette, CPopupPalette)
	//{{AFX_MSG_MAP(CColorPalette)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CColorPalette::CColorPalette()
{
}

BOOL CColorPalette::Create(CWnd* pParent, UINT uID /*=0*/)
{
	return CPopupPalette::Create(pParent, uID);
}

void CColorPalette::InitItems(CWnd* pParent)
{
	m_cpItemCount = CPoint(5, 5);
	m_czItemSize = CSize(18, 18);
	m_fHasNone = TRUE;
	m_fHasMore = TRUE;
	m_fHasColor = FALSE;
}

COLORREF CColorPalette::m_Colors[] =
{
	RGB(255, 255, 255),			// 13-White   
	RGB(0, 0, 0),					// 01-Black
	RGB(128, 128, 128),			// 14-50%Black
	RGB(0, 0, 255),				// 02-Blue
	RGB(127, 127, 255),			// 15-50%Blue
	RGB(0, 255, 255),				// 03-Cyan
	RGB(127, 255, 255),			// 16-50%Cyan
	RGB(0, 255, 191),				// 04-Teal
	RGB(127, 255, 230),			// 17-50%Teal
	RGB(0, 255, 0),				// 05-Green
	RGB(127, 255, 127),			// 18-50%Green
	RGB(127, 255, 0),				// 06-Chartreuse
	RGB(191, 255, 127),			// 19-50%Chartreuse
	RGB(255, 255, 0),				// 07-Yellow 
	RGB(255, 255, 127),			// 20-50%Yellow
	RGB(255, 127, 0),				// 08-Orange
	RGB(255, 191, 127),			// 21-50%Orange
	RGB(255, 0, 0),				// 09-Red
	RGB(255, 127, 127),			// 22-50%Red
	RGB(255, 0, 191),				// 10-Rose
	RGB(255, 127, 223),			// 23-50%Rose
	RGB(255, 0, 255),				// 11-Magenta
	RGB(255, 127, 255),			// 24-50%Magenta
	RGB(191, 127, 255),			// 12-Violet
	RGB(223, 191, 255)			// 25-50%Violet
};

void CColorPalette::DrawItemBackground(CDC* pDC)
{
	// We have nothing to do.
}

void CColorPalette::DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected)
{
	CRect crInterior = crDraw;
	crInterior.InflateRect(-1, -1);

   if (nItem == ITEM_None)
   {
		Draw3dBorder(
			 pDC,
			 crInterior,
			 GetSysColor(COLOR_BTNHILIGHT),
			 GetSysColor(COLOR_BTNSHADOW),
			 GetSysColor(COLOR_BTNFACE),
			 GetSysColor(COLOR_BTNTEXT),
			 GetSysColor(COLOR_WINDOW),
			 1,
			 TRUE,
			 TRUE,
			 FALSE);

		// Draw the fill pattern.
		crInterior.InflateRect(-2, -2);

      int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
      DrawNone(pDC, crInterior);
      pDC->SetBkMode(nOldBkMode);
   }
	else if (nItem == ITEM_More)
   {
		Draw3dBorder(
			 pDC,
			 crInterior,
			 GetSysColor(COLOR_BTNSHADOW),
			 GetSysColor(COLOR_BTNHILIGHT),
			 GetSysColor(COLOR_BTNTEXT),
			 GetSysColor(COLOR_BTNFACE),
			 GetSysColor(COLOR_BTNFACE),
			 1,
			 TRUE,
			 TRUE,
			 FALSE);

		// Draw the fill pattern.
		crInterior.InflateRect(-2, -2);

      int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
      DrawMore(pDC, crInterior);
      pDC->SetBkMode(nOldBkMode);
   }
	else if (nItem == ITEM_Color)
   {
		Draw3dBorder(
			 pDC,
			 crInterior,
			 GetSysColor(COLOR_BTNSHADOW),
			 GetSysColor(COLOR_BTNHILIGHT),
			 GetSysColor(COLOR_BTNTEXT),
			 GetSysColor(COLOR_BTNFACE),
			 GetSysColor(COLOR_BTNFACE),
			 1,
			 TRUE,
			 TRUE,
			 FALSE);

		// Draw the fill pattern.
		crInterior.InflateRect(-2, -2);

      int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
      DrawColor(pDC, crInterior);
      pDC->SetBkMode(nOldBkMode);
   }
   else
   {
		Draw3dBorder(
			 pDC,
			 crInterior,
			 GetSysColor(COLOR_BTNHILIGHT),
			 GetSysColor(COLOR_BTNSHADOW),
			 GetSysColor(COLOR_BTNFACE),
			 GetSysColor(COLOR_BTNTEXT),
			 0,
			 1,
			 TRUE,
			 FALSE,
			 FALSE);

		// Draw the fill pattern.
		crInterior.InflateRect(-2, -2);

	   CDibPatternBrush Brush;
		if (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
		{
			Brush.UsePalette(TRUE);
		}
		else
		{
			Brush.UsePalette(FALSE);
		}
						
	   Brush.Color(m_Colors[nItem]);
	   pDC->FillRect(crInterior, Brush.Brush());
   }

	// Draw the highlight if required.
   crDraw.left--;
   crDraw.top--;
   CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, ::GetSysColor(fSelected ? COLOR_HIGHLIGHT : COLOR_BTNFACE));
	CPen* pOldPen = pDC->SelectObject(&Pen);

	if (pOldPen != NULL)
	{
		CBrush Brush;
		Brush.CreateStockObject(NULL_BRUSH);
		CBrush* pOldBrush = pDC->SelectObject(&Brush);
		if (pOldBrush != NULL)
		{
			pDC->Rectangle(crDraw);
			pDC->SelectObject(pOldBrush);
		}
		pDC->SelectObject(pOldPen);
	}
}

COLORREF CColorPalette::GetColor(void)
{
	return m_Color;
}

void CColorPalette::SetColor(COLORREF Color)
{
	if (Color == TRANSPARENT_COLOR)
	{
		SetNone();
	}
	else
	{
		// Set the actual color value.
		m_Color = Color;

		m_nSelectedItem = -1;
		for (int i = 0; i < sizeof(m_Colors)/sizeof(m_Colors[0]); i++)
		{
			if (m_Colors[i] == m_Color)
			{
				m_nSelectedItem = i;
				break;
			}
		}

		// Note that this does not affect the appearance of the palette.
		// We assume this routine is called before the palette is shown.
	}
}

BOOL CColorPalette::IsNone(void)
{
	return m_Color == UNDEFINED_COLOR;
}

void CColorPalette::SetNone(void)
{
	m_Color = UNDEFINED_COLOR;
	m_nSelectedItem = ITEM_None;
}

void CColorPalette::OnOK(void)
{
	// Get the current index and convert it to a color.
	if (m_nSelectedItem == ITEM_None)
	{
		m_Color = UNDEFINED_COLOR;
	}
	else if (m_nSelectedItem == ITEM_More)
	{
		OnMore();
	}
	else if (m_nSelectedItem != -1)
	{
		m_Color = m_Colors[m_nSelectedItem];
	}

	CPopupPalette::OnOK();
}

void CColorPalette::OnMore(void)
{
   OnCancel();

	CColorDialog Dialog(m_Color, CC_FULLOPEN, NULL);

	if (Dialog.DoModal() == IDOK)
	{
		m_Color = Dialog.GetColor();
		NotifyOwner();

		// We should already be destroyed.
		ASSERT(m_hWnd == NULL);
	}
}


