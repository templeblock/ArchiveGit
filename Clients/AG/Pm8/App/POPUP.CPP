/*
// $Header: /PM8/App/POPUP.CPP 1     3/03/99 6:10p Gbeddow $
//
// popup.h : Pop-up window classes
//
// $Log: /PM8/App/POPUP.CPP $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 20    3/01/99 2:07p Lwilson
// Modified ShadowPalette and LineWidthPalette to cancel the current
// palette before calling the color palette.
// 
// 19    2/09/99 4:44p Rgrenfel
// Added support for the Made To Order toolbar popup menu for seals and
// timepieces.
// 
// 18    1/29/99 12:58p Lwilson
// Updated CColorPalette::SetColor to maintain Color and RColor versions
// 
// 17    1/27/99 1:44p Lwilson
// Added RColor support to color palette.
 * 
 * 16    1/19/99 2:57p Lwilson
 * Print Shop color dialog integration.
// 
// 15    1/14/99 3:44p Jfleischhauer
// added CPPhotoWorkshopMenuPalette class
// 
// 14    9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 13    7/14/98 6:34p Jayn
// 
// 12    5/14/98 10:54a Rlovejoy
// Pass in default fill color to gradient dialog.
// 
// 11    5/12/98 11:35a Rlovejoy
// Use strings from resource file.
// 
// 10    5/05/98 4:44p Rlovejoy
// Gradient button switch.
// 
// 9     4/30/98 4:48p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 8     4/29/98 6:32p Rlovejoy
// Handles gradient dialog.
// 
// 7     4/24/98 6:31p Fredf
// Properly uses TRANSPARENT_COLOR and UNDEFINED_COLOR.
// 
// 6     4/24/98 5:37p Rlovejoy
// Exteneded to show a gradient button.
// 
// 5     4/15/98 7:15p Fredf
// Fixed bug where palette might not be deselected.
// 
// 4     3/18/98 6:31p Fredf
// Added color button top to of popup menus.
// 
// 3     3/17/98 7:09p Fredf
// Added color button to shadow popup.
// 
// 2     2/17/98 9:02a Dennis
// Added send of WM_CANCELMODE message in OnCancel()
// 
//    Rev 1.0   14 Aug 1997 15:24:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:00   Fred
// Initial revision.
// 
//    Rev 1.16   23 Jul 1997 14:47:52   Jay
// Sets UsePalette(FALSE) for non-palette devices.
// 
//    Rev 1.15   21 Jul 1997 14:39:44   Jay
// Setting default shape didn't work if new project was made.
// 
//    Rev 1.14   02 Jul 1997 12:58:38   Fred
// Fixed problem with object/text popup duality
// 
//    Rev 1.13   10 Jun 1997 15:29:50   Jay
// New color dialog; saves custom colors.
// 
//    Rev 1.12   27 May 1997 11:08:02   Fred
// New outline selections
// 
//    Rev 1.11   23 May 1997 17:06:48   Fred
// New effects
// 
//    Rev 1.10   15 May 1997 16:48:36   Fred
// New text effects
// 
//    Rev 1.9   11 Apr 1997 11:23:38   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.8   17 Mar 1997 16:09:28   Jay
// Changed drawing of None for Text Shape popup.
// 
//    Rev 1.7   17 Mar 1997 14:48:16   Jay
// Warp shape palette and support.
// 
//    Rev 1.6   17 Mar 1997 09:40:12   Jay
// Popup warp field palette
// 
//    Rev 1.5   28 Jan 1997 14:10:32   Jay
// Shape drawing, default size, mouse activate, etc.
// 
//    Rev 1.4   27 Jan 1997 13:55:20   Jay
// Default object attributes and size.
// 
//    Rev 1.3   24 Jan 1997 17:05:44   Jay
// Shadow popup.
// 
//    Rev 1.2   24 Jan 1997 14:56:04   Jay
// Line palette
// 
//    Rev 1.1   24 Jan 1997 12:39:18   Jay
// Initial line palette; toggle tweaks, etc.
// 
//    Rev 1.0   23 Jan 1997 11:05:44   Jay
// New popup palettes
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "popup.h"

#include "util.h"
#include "utils.h"
#include "dibbrush.h"
#include "linewid.h"
#include "shapedlg.h"
#include "pmwcolor.h"
#include "graddlg.h"
#include "PhotoWorkshopResource.h"

extern CPalette *pOurPal;

#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define	COLOR_BTNHILIGHT	COLOR_BTNHIGHLIGHT
#endif
#endif

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
	TRACE("OnCancel\n");
	TRACE("GetOwner:\n");
   CWnd *pCWndOwner = GetOwner();

	// We do not want the capture anymore.
	TRACE("GetCapture:\n");
	if (GetCapture() == this)
	{
		ReleaseCapture();
	}

	TRACE("DestroyWindow:\n");
	// Just take us down.
	DestroyWindow();

	TRACE("Send CANCELMODE:\n");
   if(pCWndOwner)
      pCWndOwner->SendMessage(WM_CANCELMODE);
	TRACE("Exit OnCancel\n");
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
	m_fShowGradient = FALSE;
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

int CPopupPalette::SizeGradient(int nDefaultHeight)
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
	int nGradientBaseHeight = m_fHasGradient ? SizeColor(nFontHeight) : 0;

	int nNoneHeight = nNoneBaseHeight;
	int nMoreHeight = nMoreBaseHeight;
	int nColorHeight = nColorBaseHeight;
	int nGradientHeight = nGradientBaseHeight;

	CRect r;
	r.left = 0;
	r.top = 0;
	r.right = cpCount.x * czSize.cx + 2*nGap;
	r.bottom = nGap + nGradientHeight + nColorHeight + nNoneHeight + cpCount.y * czSize.cy + nMoreHeight + nGap;

	// Set the items rectangle.
	m_crItems.left = r.left + nGap;
	m_crItems.top = r.top + nGap + nGradientHeight + nColorHeight + nNoneHeight;
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

	// Set the Gradient rectangle.
	if (m_fHasGradient)
	{
		m_crGradient.left = r.left + nGap;
		m_crGradient.top = nGap + m_crColor.Height();
		m_crGradient.right = r.right - nGap;
		m_crGradient.bottom = m_crGradient.top + nGradientBaseHeight;
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

	CPalette* pOldPal = dc.SelectPalette(pOurPal, FALSE);
	dc.RealizePalette();

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

	if (m_fHasGradient)
	{
		DrawItem(&dc, ITEM_Gradient, m_crGradient, FALSE);
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
	else if (m_fHasGradient && m_crGradient.PtInRect(point))
	{
		SelectItem(ITEM_Gradient);
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
			else if (m_nSelectedItem == ITEM_Gradient)
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
		else if (nItem == ITEM_Gradient)
		{
			ASSERT(m_fHasGradient);
			crDraw = m_crGradient;
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
			pOldPal = dc.SelectPalette(pOurPal, FALSE);
			dc.RealizePalette();
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
		Util::SendWmCommand(pOwner, m_uID, this, 0);
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

void CPopupPalette::DrawGradient(CDC* pDC, CRect crDraw)
{
	CString csText;
	TRY
		csText.LoadString(IDS_PaletteGradient);
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

void CPopupPalette::OnGradient(void)
{
	ASSERT(!m_fHasGradient);
}

/////////////////////////////////////////////////////////////////////////////
// CColorPalette

BEGIN_MESSAGE_MAP(CColorPalette, CPopupPalette)
	//{{AFX_MSG_MAP(CColorPalette)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CColorPalette::CColorPalette()
{
	m_Color = UNDEFINED_COLOR;
	m_BlendColor = UNDEFINED_COLOR;
	m_nFillType = FillFormatV2::FillSolid;
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
	m_fHasGradient = m_fShowGradient;
}

COLOR CColorPalette::m_Colors[] =
{
	MAKE_COLOR(0, 0, 0, 0),				// 13-White   
	MAKE_COLOR(0, 0, 0, 255),			// 01-Black
	MAKE_COLOR(0, 0, 0, 127),			// 14-50%Black
	MAKE_COLOR(255, 255, 0, 0),		// 02-Blue
	MAKE_COLOR(128, 128, 0, 0),		// 15-50%Blue
	MAKE_COLOR(255, 0, 0, 0),			// 03-Cyan
	MAKE_COLOR(128, 0, 0, 0),			// 16-50%Cyan
	MAKE_COLOR(255, 0, 64, 0),			// 04-Teal
	MAKE_COLOR(128, 0, 25, 0),			// 17-50%Teal
	MAKE_COLOR(255, 0, 255, 0),		// 05-Green
	MAKE_COLOR(128, 0, 128, 0),		// 18-50%Green
	MAKE_COLOR(128, 0, 255, 0),		// 06-Chartreuse
	MAKE_COLOR(64, 0, 128, 0),			// 19-50%Chartreuse
	MAKE_COLOR(0, 0, 255, 0),			// 07-Yellow 
	MAKE_COLOR(0, 0, 128, 0),			// 20-50%Yellow         
	MAKE_COLOR(0, 128, 255, 0),		// 08-Orange
	MAKE_COLOR(0, 64, 128, 0),			// 21-50%Orange 
	MAKE_COLOR(0, 255, 255, 0),		// 09-Red
	MAKE_COLOR(0, 128, 128, 0),		// 22-50%Red
	MAKE_COLOR(0, 255, 64, 0),			// 10-Rose
	MAKE_COLOR(0, 128, 32, 0),			// 23-50%Rose
	MAKE_COLOR(0, 255, 0, 0),			// 11-Magenta
	MAKE_COLOR(0, 128, 0, 0),			// 24-50%Magenta
	MAKE_COLOR(64, 128, 0, 0),			// 12-Violet
	MAKE_COLOR(32, 64, 0, 0)			// 25-50%Violet
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
		Util::Draw3dBorder(
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
		Util::Draw3dBorder(
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
		Util::Draw3dBorder(
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
	else if (nItem == ITEM_Gradient)
   {
		Util::Draw3dBorder(
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
      DrawGradient(pDC, crInterior);
      pDC->SetBkMode(nOldBkMode);
   }
   else
   {
		Util::Draw3dBorder(
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
						
	   Brush.Color(COLORREF_FROM_COLOR(m_Colors[nItem]));
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

COLOR CColorPalette::GetColor(void)
{
	return m_Color;
}

RColor CColorPalette::GetRColor(void)
{
	return m_crColor;
}

COLOR CColorPalette::GetBlendColor(void)
{
	return m_BlendColor;
}

SHORT CColorPalette::GetFillType(void)
{
	return m_nFillType;
}

void CColorPalette::SetColor(const RColor& Color)
{
	m_Color   = ColorFromRColor( Color );
	m_crColor = Color;
}

void CColorPalette::SetColor(COLOR Color)
{
	m_crColor = RColorFromColor( Color );

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
	return m_Color == TRANSPARENT_COLOR;
}

void CColorPalette::SetNone(void)
{
	m_Color = TRANSPARENT_COLOR;
	m_nSelectedItem = ITEM_None;
}

void CColorPalette::OnOK(void)
{
	// Get the current index and convert it to a color.
	if (m_nSelectedItem == ITEM_None)
	{
		m_Color = TRANSPARENT_COLOR;
		m_nFillType = FillFormatV2::FillNone;
	}
	else if (m_nSelectedItem == ITEM_More)
	{
		OnMore();
		return;
	}
	else if (m_nSelectedItem == ITEM_Gradient)
	{
		OnGradient();
		return;
	}
	else if (m_nSelectedItem != -1)
	{
		m_Color = m_Colors[m_nSelectedItem];
		m_nFillType = FillFormatV2::FillSolid;
	}

	CPopupPalette::OnOK();
}

void CColorPalette::OnMore(void)
{
   OnCancel();

	CPmwColorDialog Dialog(colorref_from_color(m_Color));

	if (Dialog.DoModal() == IDOK)
	{
		m_Color = color_from_colorref(Dialog.GetColor());
		m_nFillType = FillFormatV2::FillSolid;
		NotifyOwner();

		// We should already be destroyed.
		ASSERT(m_hWnd == NULL);
	}
}

void CColorPalette::OnGradient(void)
{
   OnCancel();

	CGradientDlg Dialog;
	if (CFill::IsValidColor(m_Color))
	{
		Dialog.SetBaseColor(m_Color);
	}
	if (Dialog.DoModal() == IDOK)
	{
		m_Color = Dialog.GetBaseColor();
		m_BlendColor = Dialog.GetBlendColor();
		m_nFillType = Dialog.GetFillType();
		NotifyOwner();

		// We should already be destroyed.
		ASSERT(m_hWnd == NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGradientColorPalette

BEGIN_MESSAGE_MAP(CGradientColorPalette, CColorPalette)
	//{{AFX_MSG_MAP(CGradientColorPalette)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CGradientColorPalette::CGradientColorPalette()
{

}

BOOL CGradientColorPalette::Create(CWnd* pParent, UINT uID /*= 0*/)
{
	return CColorPalette::Create(pParent, uID);
}
	
void CGradientColorPalette::InitItems(CWnd* pParent)
{
	m_cpItemCount = CPoint(5, 5);
	m_czItemSize = CSize(18, 18);
	m_fHasNone = FALSE;
	m_fHasMore = TRUE;
	m_fHasColor = FALSE;
	m_fHasGradient = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CLineWidthPalette

BEGIN_MESSAGE_MAP(CLineWidthPalette, CPopupPalette)
	//{{AFX_MSG_MAP(CLineWidthPalette)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CLineWidthPalette::LineWidth ObjectLineWidths[] =
{
	{ IDS_HAIRLINE,	OutlineFormat::Light,		OutlineFormat::Absolute,	MakeFixed(0) },
	{ IDS_POINT,		OutlineFormat::Inflate,		OutlineFormat::Absolute,	MakeFixed(1) },
	{ IDS_POINT,		OutlineFormat::Inflate,		OutlineFormat::Absolute,	MakeFixed(2) },
	{ IDS_POINT,		OutlineFormat::Inflate,		OutlineFormat::Absolute,	MakeFixed(4) },
	{ IDS_POINT,		OutlineFormat::Inflate,		OutlineFormat::Absolute,	MakeFixed(8) }
};

#define NUMBER_OF_OBJECT_LINE_WIDTHS	(sizeof(ObjectLineWidths)/sizeof(ObjectLineWidths[0]))

CLineWidthPalette::CLineWidthPalette()
{
	m_pLineWidths = &(ObjectLineWidths[0]);
	m_nLineWidths = NUMBER_OF_OBJECT_LINE_WIDTHS;
}

BOOL CLineWidthPalette::Create(CWnd* pParent, UINT uID /*=0*/)
{
	return CPopupPalette::Create(pParent, uID);
}

void CLineWidthPalette::InitItems(CWnd* pParent)
{
	m_cpItemCount = CPoint(1, m_nLineWidths);

	// Compute the size of the widest name string. Since the strings
	// can be localized, we cannot assume one string (e.g., "Hairline")
	// is the widest.

	TEXTMETRIC tm;
	GetPopupFontMetrics(tm);

	CFont* pFont = GetPopupFont();
	ASSERT(pFont != NULL);

	CClientDC dc(NULL);		// Will happen before creation.
	CString csFormat;
	CString csText;
	int nMaxWidth = 0;

	CFont* pOldFont = dc.SelectObject(pFont);

	for (int nWidth = 0; nWidth < m_nLineWidths; nWidth++)
	{
		TRY
		{
			csFormat.LoadString(m_pLineWidths[nWidth].m_uName);
			csText.Format(csFormat, 1234);
			CSize czSize = Util::GetTextExtent(&dc, csText);
			if (czSize.cx > nMaxWidth)
			{
				nMaxWidth = czSize.cx;
			}
		}
		END_TRY
	}

	if (pOldFont != NULL)
	{
		dc.SelectObject(pOldFont);
	}

	m_czItemSize = CSize(nMaxWidth + 10*tm.tmAveCharWidth, tm.tmHeight + 2*4);

	m_fHasColor = TRUE;
	m_fHasNone = TRUE;
	m_fHasMore = TRUE;
	m_fHasGradient = FALSE;
}

void CLineWidthPalette::DrawItemBackground(CDC* pDC)
{
	CRect crDraw(m_crItems);

	if (m_fHasNone)
	{
		crDraw.top = m_crNone.top;
	}

	Util::Draw3dBorder(
							 pDC,
							 crDraw,
							 GetSysColor(COLOR_BTNHILIGHT),
							 GetSysColor(COLOR_BTNSHADOW),
							 GetSysColor(COLOR_BTNFACE),
							 GetSysColor(COLOR_BTNTEXT),
							 GetSysColor(COLOR_WINDOW),
							 1,
							 TRUE,
							 TRUE,
							 FALSE);
}

void CLineWidthPalette::DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected)
{
	CRect crInterior = crDraw;
	crInterior.InflateRect(-1, -1);

   if (nItem == ITEM_None)
   {
		// Draw the fill pattern.
		crInterior.InflateRect(-2, -2);

      int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
      DrawNone(pDC, crInterior);
      pDC->SetBkMode(nOldBkMode);
   }
	else if (nItem == ITEM_More)
   {
		Util::Draw3dBorder(
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
		Util::Draw3dBorder(
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
		// Draw the fill pattern.
		crInterior.InflateRect(-2, -2);

		TEXTMETRIC tmFont;
		GetPopupFontMetrics(tmFont);

		CBrush cbFill;
		CPen cpOutline;
		
		int nContext = pDC->SaveDC();
		if (nContext != 0)
		{
			SHORT nStyle = m_pLineWidths[nItem].m_nStyle;
			SHORT nWidthType = m_pLineWidths[nItem].m_nWidthType;
			CFixed lWidth = m_pLineWidths[nItem].m_lWidth;

			CRect crWidthBounds(crInterior);
			int nWidth = 0;

			if ((nStyle == OutlineFormat::Inflate) || (nStyle == OutlineFormat::Light))
			{
				if (nWidthType == OutlineFormat::Relative)
				{
					nWidth = (int)MulFixed(lWidth, 400);	// oooh - magic number
				}
				else
				{
					nWidth = FixedInteger(lWidth);
				}

				if (cbFill.CreateSolidBrush(RGB(0,0,0)))
				{
					if (cpOutline.CreatePen(PS_SOLID, 0, RGB(0,0,0)))
					{
						pDC->SelectObject(&cbFill);
						pDC->SelectObject(&cpOutline);
						
						int nYResolution = pDC->GetDeviceCaps(LOGPIXELSY);
						int nHeight = MulDiv(nWidth, nYResolution, 72);
						
						if (nHeight <= 0)
						{
							nHeight = 1;
						}
						
						if (nHeight > tmFont.tmHeight)
						{
							nHeight = tmFont.tmHeight;
						}
						
						if (nHeight > crInterior.Height())
						{
							nHeight = crInterior.Height();
						}
						
						crWidthBounds.top += (crInterior.Height()-nHeight)/2;
						crWidthBounds.bottom = crWidthBounds.top+nHeight;
			
						crWidthBounds.left = crWidthBounds.left+1;
						crWidthBounds.right = crWidthBounds.left+4*tmFont.tmAveCharWidth;
						
						pDC->Rectangle(crWidthBounds);
					}
				}
			}
			else
			{
				switch (nStyle)
				{
					case OutlineFormat::Contour1:
					{
						int nHeight = 7;

						crWidthBounds.top += (crInterior.Height()-nHeight)/2;
						crWidthBounds.bottom = crWidthBounds.top+nHeight;
			
						crWidthBounds.left = crWidthBounds.left+1;
						crWidthBounds.right = crWidthBounds.left+4*tmFont.tmAveCharWidth;

						crWidthBounds.bottom = crWidthBounds.top+2;

						Util::FillRectangleWithColor(*pDC, crWidthBounds, RGB(0,0,0));

						crWidthBounds.top = crWidthBounds.bottom;
						crWidthBounds.bottom = crWidthBounds.top+5;

						Util::FillRectangleWithColor(*pDC, crWidthBounds, RGB(192,192,192));
						
						break;
					}
					case OutlineFormat::Contour2:
					{
						int nHeight = 8;

						crWidthBounds.top += (crInterior.Height()-nHeight)/2;
						crWidthBounds.bottom = crWidthBounds.top+nHeight;
			
						crWidthBounds.left = crWidthBounds.left+1;
						crWidthBounds.right = crWidthBounds.left+4*tmFont.tmAveCharWidth;

						crWidthBounds.bottom = crWidthBounds.top+2;

						Util::FillRectangleWithColor(*pDC, crWidthBounds, RGB(0,0,0));

						crWidthBounds.top = crWidthBounds.bottom;
						crWidthBounds.bottom = crWidthBounds.top+4;

						Util::FillRectangleWithColor(*pDC, crWidthBounds, RGB(192,192,192));

						crWidthBounds.top = crWidthBounds.bottom;
						crWidthBounds.bottom = crWidthBounds.top+2;

						Util::FillRectangleWithColor(*pDC, crWidthBounds, RGB(0,0,0));
						
						break;
					}
					case OutlineFormat::Calligraphy:
					{
						crWidthBounds.left = crWidthBounds.left+1;
						crWidthBounds.right = crWidthBounds.left+4*tmFont.tmAveCharWidth;

						crWidthBounds.InflateRect(0, -1);

						int nHeight = crWidthBounds.Height();
						int nWidth = crWidthBounds.Width();

						if (nWidth > nHeight)
						{
							crWidthBounds.left += (nWidth-nHeight)/2;
							crWidthBounds.right = crWidthBounds.left+nHeight;
						}
						else
						{
							crWidthBounds.top += (nHeight-nWidth)/2;
							crWidthBounds.bottom = crWidthBounds.top+nWidth;
						}

						pDC->SetMapMode(MM_TEXT);

						CPen Pen;
						if (Pen.CreatePen(PS_SOLID, 2, RGB(0,0,0)))
						{
							pDC->SelectObject(&Pen);

							pDC->MoveTo(crWidthBounds.right, crWidthBounds.top);
							pDC->LineTo(crWidthBounds.left, crWidthBounds.bottom);

							pDC->SelectStockObject(NULL_PEN);
						}

						break;
					}
				}
			}

			CString csFormat;
			CString csText;

			TRY
			{
				csFormat.LoadString(m_pLineWidths[nItem].m_uName);
				csText.Format(csFormat, nWidth);
			}
			END_TRY

			CRect crText(crInterior);
			crText.left = crWidthBounds.right+tmFont.tmAveCharWidth/2;

			DrawString(pDC, csText, crText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

			pDC->RestoreDC(nContext);
		}
   }

	COLORREF clPen;

	if (nItem == ITEM_More || nItem == ITEM_Color)
	{
		crDraw.left--;
		crDraw.top--;
		clPen =::GetSysColor(fSelected ? COLOR_HIGHLIGHT : COLOR_BTNFACE);
	}
	else
	{
		// Draw the highlight if required.
		crDraw.left += 3;
		crDraw.top += 3;
		crDraw.right -= 3;
		crDraw.bottom -= 3;
		clPen = ::GetSysColor(fSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW);
	}

   CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, clPen);
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

void CLineWidthPalette::GetWidth(SHORT& nStyle, SHORT& nWidthType, CFixed& lWidth)
{
	nStyle = m_nStyle;
	nWidthType = m_nWidthType;
	lWidth = m_lWidth;
}

void CLineWidthPalette::SetWidth(SHORT nStyle, SHORT nWidthType, CFixed lWidth)
{
	m_nStyle = nStyle;
	m_nWidthType = nWidthType;
	m_lWidth = lWidth;

	if (m_nStyle == OutlineFormat::None)
	{
		SetNone();
	}
	else
	{
		m_nSelectedItem = -1;
		for (int nWidth = 0; nWidth < m_nLineWidths; nWidth++)
		{
			if ((nStyle == m_pLineWidths[nWidth].m_nStyle)
			 && (nWidthType == m_pLineWidths[nWidth].m_nWidthType)
			 && (lWidth == m_pLineWidths[nWidth].m_lWidth))
			{
				m_nSelectedItem = nWidth;
				break;
			}
		}
	}

	// Note that this does not affect the appearance of the palette.
	// We assume this routine is called before the palette is shown.
}

BOOL CLineWidthPalette::IsNone(void)
{
	return m_nStyle == OutlineFormat::None;
}

void CLineWidthPalette::SetNone(void)
{
	m_nStyle = OutlineFormat::None;
	m_nWidthType = OutlineFormat::Absolute;
	m_lWidth = MakeFixed(0);
	m_nSelectedItem = ITEM_None;
}

void CLineWidthPalette::OnOK(void)
{
	// Get the current index and convert it to a color.
	if (m_nSelectedItem == ITEM_None)
	{
		m_nStyle = OutlineFormat::None;
		m_nWidthType = OutlineFormat::Absolute;
		m_lWidth = MakeFixed(0);
	}
	else if (m_nSelectedItem == ITEM_More)
	{
		OnMore();
		return;
	}
	else if (m_nSelectedItem == ITEM_Color)
	{
		OnColor();
		return;
	}
	else if (m_nSelectedItem != -1)
	{
		m_nStyle = m_pLineWidths[m_nSelectedItem].m_nStyle;
		m_nWidthType = m_pLineWidths[m_nSelectedItem].m_nWidthType;
		m_lWidth = m_pLineWidths[m_nSelectedItem].m_lWidth;
	}

	CPopupPalette::OnOK();
}

void CLineWidthPalette::OnMore(void)
{
   OnCancel();

	int nWidth;
	if ((m_nStyle == OutlineFormat::Inflate) && (m_nWidthType == OutlineFormat::Absolute))
	{
		nWidth = FixedInteger(m_lWidth);
	}
	else
	{
		nWidth = 0;
	}

	CChooseLineWidthDialog Dialog(nWidth, 72, FALSE);

	if (Dialog.DoModal() == IDOK)
	{
		if (Dialog.m_nLineWidth < 0)
		{
			m_nStyle = OutlineFormat::None;
			m_nWidthType = OutlineFormat::Absolute;
			m_lWidth = MakeFixed(0);
		}
		else if (Dialog.m_nLineWidth == 0)
		{
			m_nStyle = OutlineFormat::Light;
			m_nWidthType = OutlineFormat::Absolute;
			m_lWidth = MakeFixed(0);
		}
		else
		{
			m_nStyle = OutlineFormat::Inflate;
			m_nWidthType = OutlineFormat::Absolute;
			m_lWidth = MakeFixed(Dialog.m_nLineWidth);
		}

		NotifyOwner();

		// We should already be destroyed.
		ASSERT(m_hWnd == NULL);
	}
}

void CLineWidthPalette::OnColor(void)
{
   OnCancel();

	CWnd* pOwner = GetOwner();
	Util::SendWmCommand(pOwner, ID_OUTLINE_COLOR, this, 0);
}

/////////////////////////////////////////////////////////////////////////////
// CTextLineWidthPalette

BEGIN_MESSAGE_MAP(CTextLineWidthPalette, CLineWidthPalette)
	//{{AFX_MSG_MAP(CTextLineWidthPalette)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CLineWidthPalette::LineWidth TextLineWidths[] =
{
	{ IDS_HAIRLINE,		OutlineFormat::Light,			OutlineFormat::Absolute,	MakeFixed(0) },
	{ IDS_THIN,				OutlineFormat::Inflate,			OutlineFormat::Relative,	MakeFixed(TEXT_OUTLINE_THIN_FRACTION) },
	{ IDS_MEDIUM,			OutlineFormat::Inflate,			OutlineFormat::Relative,	MakeFixed(TEXT_OUTLINE_MEDIUM_FRACTION) },
	{ IDS_THICK,			OutlineFormat::Inflate,			OutlineFormat::Relative,	MakeFixed(TEXT_OUTLINE_THICK_FRACTION) },
	{ IDS_CONTOUR1,		OutlineFormat::Contour1,		OutlineFormat::Relative,	MakeFixed(0) },
	{ IDS_CONTOUR2,		OutlineFormat::Contour2,		OutlineFormat::Relative,	MakeFixed(0) },
	{ IDS_CALLIGRAPHY,	OutlineFormat::Calligraphy,	OutlineFormat::Relative,	MakeFixed(0) },
	{ IDS_POINT,			OutlineFormat::Inflate,			OutlineFormat::Absolute,	MakeFixed(1) },
	{ IDS_POINT,			OutlineFormat::Inflate,			OutlineFormat::Absolute,	MakeFixed(2) },
	{ IDS_POINT,			OutlineFormat::Inflate,			OutlineFormat::Absolute,	MakeFixed(4) },
	{ IDS_POINT,			OutlineFormat::Inflate,			OutlineFormat::Absolute,	MakeFixed(8) }
};

#define NUMBER_OF_TEXT_LINE_WIDTHS		(sizeof(TextLineWidths)/sizeof(TextLineWidths[0]))

CTextLineWidthPalette::CTextLineWidthPalette()
{
	m_pLineWidths = &(TextLineWidths[0]);
	m_nLineWidths = NUMBER_OF_TEXT_LINE_WIDTHS;
}

/////////////////////////////////////////////////////////////////////////////
// CShadowPalette

BEGIN_MESSAGE_MAP(CShadowPalette, CPopupPalette)
	//{{AFX_MSG_MAP(CShadowPalette)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CShadowPalette::ShadowInformation ObjectShadowInformation[] =
{
	{ IDS_DROP_DOWN_RIGHT,		ShadowFormat::Drop,		ShadowFormat::Relative,	 MakeFixed(OBJECT_SHADOW_FRACTION),	-MakeFixed(OBJECT_SHADOW_FRACTION) },
	{ IDS_DROP_DOWN_LEFT,		ShadowFormat::Drop,		ShadowFormat::Relative,	-MakeFixed(OBJECT_SHADOW_FRACTION),	-MakeFixed(OBJECT_SHADOW_FRACTION) },
	{ IDS_DROP_UP_RIGHT,			ShadowFormat::Drop,		ShadowFormat::Relative,	 MakeFixed(OBJECT_SHADOW_FRACTION),	 MakeFixed(OBJECT_SHADOW_FRACTION) },
	{ IDS_DROP_UP_LEFT,			ShadowFormat::Drop,		ShadowFormat::Relative,	-MakeFixed(OBJECT_SHADOW_FRACTION),	 MakeFixed(OBJECT_SHADOW_FRACTION) },
};

#define NUMBER_OF_OBJECT_SHADOWS		(sizeof(ObjectShadowInformation)/sizeof(ObjectShadowInformation[0]))

CShadowPalette::CShadowPalette()
{
	m_pShadowInformation = &(ObjectShadowInformation[0]);
	m_nShadowInformation = NUMBER_OF_OBJECT_SHADOWS;
}

BOOL CShadowPalette::Create(CWnd* pParent, UINT uID /*=0*/)
{
	return CPopupPalette::Create(pParent, uID);
}

void CShadowPalette::InitItems(CWnd* pParent)
{
	m_cpItemCount = CPoint(1, m_nShadowInformation);

	// Compute the size of the widest name string. Since the strings
	// can be localized, we cannot assume one string is the widest.

	TEXTMETRIC tm;
	GetPopupFontMetrics(tm);

	CFont* pFont = GetPopupFont();
	ASSERT(pFont != NULL);

	CClientDC dc(NULL);		// Will happen before creation.
	CString csFormat;
	CString csText;
	int nMaxWidth = 0;

	CFont* pOldFont = dc.SelectObject(pFont);

	for (int nShadow = 0; nShadow < m_nShadowInformation; nShadow++)
	{
		TRY
		{
			csFormat.LoadString(m_pShadowInformation[nShadow].m_uName);
			csText.Format(csFormat);
			CSize czSize = Util::GetTextExtent(&dc, csText);
			if (czSize.cx > nMaxWidth)
			{
				nMaxWidth = czSize.cx;
			}
		}
		END_TRY
	}

	if (pOldFont != NULL)
	{
		dc.SelectObject(pOldFont);
	}

	m_czItemSize = CSize(nMaxWidth + 10*tm.tmAveCharWidth, tm.tmHeight + 2*4);

	m_fHasColor = TRUE;
	m_fHasNone = TRUE;
	m_fHasMore = FALSE;
	m_fHasGradient = FALSE;
}

void CShadowPalette::DrawItemBackground(CDC* pDC)
{
	CRect crDraw(m_crItems);

	if (m_fHasNone)
	{
		crDraw.top = m_crNone.top;
	}

	Util::Draw3dBorder(
							 pDC,
							 crDraw,
							 GetSysColor(COLOR_BTNHILIGHT),
							 GetSysColor(COLOR_BTNSHADOW),
							 GetSysColor(COLOR_BTNFACE),
							 GetSysColor(COLOR_BTNTEXT),
							 GetSysColor(COLOR_WINDOW),
							 1,
							 TRUE,
							 TRUE,
							 FALSE);
}

void CShadowPalette::DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected)
{
	CRect crInterior = crDraw;
	crInterior.InflateRect(-1, -1);

   if (nItem == ITEM_None)
   {
		// Draw the fill pattern.
		crInterior.InflateRect(-2, -2);

      int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
      DrawNone(pDC, crInterior);
      pDC->SetBkMode(nOldBkMode);
   }
	else if (nItem == ITEM_Color)
   {
			Util::Draw3dBorder(
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
		// Draw the fill pattern.
		crInterior.InflateRect(-2, -2);

		TEXTMETRIC tmFont;
		GetPopupFontMetrics(tmFont);

		CBrush cbFill;
		CPen cpOutline;
		
		int nContext = pDC->SaveDC();
		if (nContext != 0)
		{
			SHORT nStyle = m_pShadowInformation[nItem].m_nStyle;
			SHORT nOffsetType = m_pShadowInformation[nItem].m_nOffsetType;
			CFixed lXOffset = m_pShadowInformation[nItem].m_lXOffset;
			CFixed lYOffset = m_pShadowInformation[nItem].m_lYOffset;

			CRect crImage(crInterior);
			crImage.InflateRect(0, -1);
			crImage.left = crImage.left+1;
			crImage.right = crImage.left+4*tmFont.tmAveCharWidth;

			if ((nStyle == ShadowFormat::Drop) || (nStyle == ShadowFormat::Extruded))
			{
				int nHeight = crImage.Height();
				int nWidth = crImage.Width();

				if (nWidth > nHeight)
				{
					crImage.left += (nWidth-nHeight)/2;
					crImage.right = crImage.left+nHeight;
				}
				else
				{
					crImage.top += (nHeight-nWidth)/2;
					crImage.bottom = crImage.top+nWidth;
				}

				int nCount = (crImage.Width()+3)/7;
				int nSize = crImage.Width()-nCount;
				int nX = crImage.left+((lXOffset >= 0) ? nCount : 0);
				int nY = crImage.top+((lYOffset <= 0) ? nCount : 0);
				int nXInc = lXOffset >= 0 ? -1 : 1;
				int nYInc = lYOffset <= 0 ? -1 : 1;
				CRect crRect(nX, nY, nX+nSize, nY+nSize);

				for (int nStep = 0; nStep <= nCount; nStep++)
				{
					if ((nStyle == ShadowFormat::Extruded)
					 || (nStep == 0)
					 || (nStep == nCount))
					{
						Util::DrawBorderWithColor(*pDC, crRect, (nStep == nCount) ? RGB(0,0,0) : RGB(128,128,128), 1);
					}
					crRect.OffsetRect(nXInc, nYInc);
				}
			}

			CString csFormat;
			CString csText;

			TRY
			{
				csFormat.LoadString(m_pShadowInformation[nItem].m_uName);
				csText.Format(csFormat);
			}
			END_TRY

			CRect crText(crInterior);
			crText.left = crImage.right+tmFont.tmAveCharWidth/2;

			DrawString(pDC, csText, crText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

			pDC->RestoreDC(nContext);
		}
   }

	COLORREF clPen;

	if (nItem == ITEM_Color)
	{
		crDraw.left--;
		crDraw.top--;
		clPen =::GetSysColor(fSelected ? COLOR_HIGHLIGHT : COLOR_BTNFACE);
	}
	else
	{
		// Draw the highlight if required.
		crDraw.left += 3;
		crDraw.top += 3;
		crDraw.right -= 3;
		crDraw.bottom -= 3;
		clPen = ::GetSysColor(fSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW);
	}

   CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, clPen);
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

void CShadowPalette::GetShadow(SHORT& nStyle, SHORT& nOffsetType, CFixed& lXOffset, CFixed& lYOffset)
{
	nStyle = m_nStyle;
	nOffsetType = m_nOffsetType;
	lXOffset = m_lXOffset;
	lYOffset = m_lYOffset;
}

void CShadowPalette::SetShadow(SHORT nStyle, SHORT nOffsetType, CFixed lXOffset, CFixed lYOffset)
{
	m_nStyle = nStyle;
	m_nOffsetType = nOffsetType;
	m_lXOffset = lXOffset;
	m_lYOffset = lYOffset;

	if (m_nStyle == ShadowFormat::None)
	{
		SetNone();
	}
	else
	{
		m_nSelectedItem = -1;
		for (int nShadow = 0; nShadow < m_nShadowInformation; nShadow++)
		{
			if ((nStyle == m_pShadowInformation[nShadow].m_nStyle)
			 && (nOffsetType == m_pShadowInformation[nShadow].m_nOffsetType)
			 && (lXOffset == m_pShadowInformation[nShadow].m_lXOffset)
			 && (lYOffset == m_pShadowInformation[nShadow].m_lYOffset))
			{
				m_nSelectedItem = nShadow;
				break;
			}
		}
	}

	// Note that this does not affect the appearance of the palette.
	// We assume this routine is called before the palette is shown.
}

BOOL CShadowPalette::IsNone(void)
{
	return m_nStyle == ShadowFormat::None;
}

void CShadowPalette::SetNone(void)
{
	m_nStyle = ShadowFormat::None;
	m_nOffsetType = ShadowFormat::Absolute;
	m_lXOffset = MakeFixed(0);
	m_lYOffset = MakeFixed(0);
	m_nSelectedItem = ITEM_None;
}

void CShadowPalette::OnOK(void)
{
	// Get the current index and convert it to a color.
	if (m_nSelectedItem == ITEM_None)
	{
		m_nStyle = ShadowFormat::None;
		m_nOffsetType = ShadowFormat::Absolute;
		m_lXOffset = MakeFixed(0);
		m_lYOffset = MakeFixed(0);
	}
	else if (m_nSelectedItem == ITEM_Color)
	{
		OnColor();
		return;
	}
	else if (m_nSelectedItem != -1)
	{
		m_nStyle = m_pShadowInformation[m_nSelectedItem].m_nStyle;
		m_nOffsetType = m_pShadowInformation[m_nSelectedItem].m_nOffsetType;
		m_lXOffset = m_pShadowInformation[m_nSelectedItem].m_lXOffset;
		m_lYOffset = m_pShadowInformation[m_nSelectedItem].m_lYOffset;
	}

	CPopupPalette::OnOK();
}

void CShadowPalette::OnColor(void)
{
	OnCancel();

	CWnd* pOwner = GetOwner();
	Util::SendWmCommand(pOwner, ID_SHADOW_COLOR, this, 0);
}

/////////////////////////////////////////////////////////////////////////////
// CTextShadowPalette

BEGIN_MESSAGE_MAP(CTextShadowPalette, CShadowPalette)
	//{{AFX_MSG_MAP(CTextShadowPalette)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CShadowPalette::ShadowInformation TextShadowInformation[] =
{
	{ IDS_DROP_DOWN_RIGHT,		ShadowFormat::Drop,		ShadowFormat::Relative,	 MakeFixed(TEXT_SHADOW_FRACTION),	-MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_DROP_DOWN_LEFT,		ShadowFormat::Drop,		ShadowFormat::Relative,	-MakeFixed(TEXT_SHADOW_FRACTION),	-MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_DROP_UP_RIGHT,			ShadowFormat::Drop,		ShadowFormat::Relative,	 MakeFixed(TEXT_SHADOW_FRACTION),	 MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_DROP_UP_LEFT,			ShadowFormat::Drop,		ShadowFormat::Relative,	-MakeFixed(TEXT_SHADOW_FRACTION),	 MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_EXTRUDED_DOWN_RIGHT,	ShadowFormat::Extruded,	ShadowFormat::Relative,	 MakeFixed(TEXT_SHADOW_FRACTION),	-MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_EXTRUDED_DOWN_LEFT,	ShadowFormat::Extruded,	ShadowFormat::Relative,	-MakeFixed(TEXT_SHADOW_FRACTION),	-MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_EXTRUDED_UP_RIGHT,	ShadowFormat::Extruded,	ShadowFormat::Relative,	 MakeFixed(TEXT_SHADOW_FRACTION),	 MakeFixed(TEXT_SHADOW_FRACTION) },
	{ IDS_EXTRUDED_UP_LEFT,		ShadowFormat::Extruded,	ShadowFormat::Relative,	-MakeFixed(TEXT_SHADOW_FRACTION),	 MakeFixed(TEXT_SHADOW_FRACTION) }
};

#define NUMBER_OF_TEXT_SHADOWS		(sizeof(TextShadowInformation)/sizeof(TextShadowInformation[0]))

CTextShadowPalette::CTextShadowPalette()
{
	m_pShadowInformation = &(TextShadowInformation[0]);
	m_nShadowInformation = NUMBER_OF_TEXT_SHADOWS;
}

/////////////////////////////////////////////////////////////////////////////
// CShapePalette

CShapeFile CShapePalette::m_ShapeFile;

BEGIN_MESSAGE_MAP(CShapePalette, CPopupPalette)
	//{{AFX_MSG_MAP(CShapePalette)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CShapePalette::CShapePalette()
{
	m_nShape = -1;
}

BOOL CShapePalette::Create(CWnd* pParent, UINT uID /*=0*/)
{
	return CPopupPalette::Create(pParent, uID);
}

void CShapePalette::InitItems(CWnd* pParent)
{
	m_cpItemCount = CPoint(6, 6);
	m_czItemSize = CSize(ShapeTileWidth+2*3, ShapeTileHeight+2*3);

	m_fHasNone = FALSE;
	m_fHasMore = TRUE;
	m_fHasGradient = FALSE;

	LoadShapes();

	if (m_nShape == -1)
	{
		m_nShape = 0;
	}
	NotifyOwner();

	ASSERT(m_ShapeFile.ShapeCount() >= (m_cpItemCount.x*m_cpItemCount.y));
}

void CShapePalette::DrawItemBackground(CDC* pDC)
{
	CRect crDraw(m_crItems);

	Util::Draw3dBorder(
							 pDC,
							 crDraw,
							 GetSysColor(COLOR_BTNHILIGHT),
							 GetSysColor(COLOR_BTNSHADOW),
							 GetSysColor(COLOR_BTNFACE),
							 GetSysColor(COLOR_BTNTEXT),
							 GetSysColor(COLOR_WINDOW),
							 1,
							 TRUE,
							 TRUE,
							 FALSE);
}

void CShapePalette::DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected)
{
	CPoint cpItem = ItemToXY(nItem);

	CRect crInterior = crDraw;
	crInterior.InflateRect(-1, -1);

   if (nItem == ITEM_More)
   {
		Util::Draw3dBorder(
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
	else if (nItem == ITEM_None)
   {
		// No NONE!
		ASSERT(FALSE);
   }
   else
   {
		// Draw the interior
		crInterior.InflateRect(-3, -3);

		CFixed fxShapeBoundsWidth = MakeFixed(crInterior.Width());
		CFixed fxShapeBoundsHeight = MakeFixed(crInterior.Height());

		CShapeEntry* pShapeEntry;
		if (nItem < m_ShapeFile.ShapeCount() && (pShapeEntry = m_ShapeFile.Shape(nItem)) != NULL)
		{
			CFixed fxPathWidth = pShapeEntry->m_Bounds.Width();
			CFixed fxPathHeight = pShapeEntry->m_Bounds.Height();

			if ((fxPathWidth != 0) && (fxPathHeight != 0))
			{
				// Compute the path dimensions of the path to draw in the preview.
				// The aspect of the path is preserved.

				if (DivFixed(fxPathHeight, fxPathWidth) > DivFixed(fxShapeBoundsHeight, fxShapeBoundsWidth))
				{
					fxPathWidth = MulDivFixed(fxPathWidth, fxShapeBoundsHeight, fxPathHeight);
					fxPathHeight = fxShapeBoundsHeight;
				}
				else
				{
					fxPathHeight = MulDivFixed(fxPathHeight, fxShapeBoundsWidth, fxPathWidth);
					fxPathWidth = fxShapeBoundsWidth;
				}

				// Compute the matrix for transforming the path to the desired size.
				// By convention, the path has been normalized to the unit square.

				COutlinePathMatrix Matrix;
				Matrix.Identity();

				Matrix.ScaleBy(fxPathWidth, fxPathHeight);
				
				// Adjust the matrix to center the shape within the preview box.
				
				Matrix.TranslateBy(MakeFixed(crInterior.left)+(fxShapeBoundsWidth-fxPathWidth)/2,
										 MakeFixed(crInterior.top)+(fxShapeBoundsHeight-fxPathHeight)/2);

				// Transform the path and draw it.

				COutlinePath Path;
				Path.Copy(&(pShapeEntry->m_Path));

				Path.Transform(&Matrix);

				Path.Draw2(
					pDC,
					RGB(128,128,128),
					TRUE,
					0,
					RGB(0,0,0),
					TRUE,
					WINDING,
					FALSE);
			}
      }
   }

	COLORREF clHighlight;

	if (nItem == ITEM_More)
	{
		crDraw.left--;
		crDraw.top--;
		clHighlight = ::GetSysColor(fSelected ? COLOR_HIGHLIGHT : COLOR_BTNFACE);
	}
	else
	{
		crDraw.left += 2;
		crDraw.top += 2;
		crDraw.right -= 2;
		crDraw.bottom -= 2;
		clHighlight = ::GetSysColor(fSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW);
	}

   CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, clHighlight);
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

void CShapePalette::OnOK(void)
{
	// Get the current index and convert it to a shape.
	if (m_nSelectedItem == ITEM_More)
	{
		// Do More!
		OnMore();
	}
	else if (m_nSelectedItem != -1)
	{
		m_nShape = m_nSelectedItem;
		NotifyOwner();
	}

	CPopupPalette::OnOK();
}

void CShapePalette::LoadShapes(void)
{
	// We assume that there are some shapes in the list.
	// So a count of zero means no shapes loaded.
	if (m_ShapeFile.ShapeCount() == 0)
	{
		CString csDatFile = GetGlobalPathManager()->ExpandPath("PMWSHAPE.DAT");
		CString csBinFile = GetGlobalPathManager()->ExpandPath("PMWSHAPE.BIN");
		m_ShapeFile.Init(csDatFile, csBinFile);
		ASSERT(m_ShapeFile.ShapeCount() != 0);
	}
}

void CShapePalette::OnMore(void)
{
   OnCancel();

	CShapeDialog Dialog(&m_ShapeFile);
	Dialog.m_nChosen = m_nShape;

	if (Dialog.DoModal() == IDOK)
	{
		m_nShape = Dialog.m_nChosen;
		NotifyOwner();

		// We should already be destroyed.
		ASSERT(m_hWnd == NULL);
	}
}

void CShapePalette::NotifyOwner(void)
{
	CPmwView* pView = (CPmwView*)GetOwner();
	pView->SetShapeEntry(m_ShapeFile.Shape(m_nShape));
}

/////////////////////////////////////////////////////////////////////////////
// CTextShapePalette

CWarpFile CTextShapePalette::m_ShapeFile;

BEGIN_MESSAGE_MAP(CTextShapePalette, CPopupPalette)
	//{{AFX_MSG_MAP(CTextShapePalette)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTextShapePalette::CTextShapePalette()
{
	m_nShape = -1;
}

void CTextShapePalette::SetShape(LPCSTR pName)
{
	m_nSelectedItem = -1;

	if (pName != NULL)
	{
		// Look through the shapes for the shape with this name.
		int nShapes = m_cpItemCount.x*m_cpItemCount.y;
		for (int nShape = 0; nShape < nShapes; nShape++)
		{
			CWarpField* pWarpField = m_ShapeFile.WarpField(nShape);
			if (pWarpField != NULL && pWarpField->Name().CompareNoCase(pName) == 0)
			{
				m_nSelectedItem = nShape;
				break;
			}
		}
	}
	// Note that this does not affect the appearance of the palette.
	// We assume this routine is called before the palette is shown.
}

CWarpField* CTextShapePalette::GetShape(void)
{
	ASSERT(m_nShape != -1);
	return (m_nShape == -1) ? NULL : m_ShapeFile.WarpField(m_nShape);
}

BOOL CTextShapePalette::Create(CWnd* pParent, UINT uID /*=0*/)
{
	if (!CPopupPalette::Create(pParent, uID))
	{
		return FALSE;
	}

   return TRUE;
}

int CTextShapePalette::SizeNone(int nDefaultHeight)
{
	return ItemSize().cy + 4;
}

void CTextShapePalette::InitItems(CWnd* pParent)
{
	m_cpItemCount = CPoint(4, 8);

	TEXTMETRIC tm;
	GetPopupFontMetrics(tm);

	m_czItemSize = CSize(8*tm.tmAveCharWidth+2*3, tm.tmHeight*2/*+2*3*/);

	m_fHasColor = FALSE;
	m_fHasNone = TRUE;
	m_fHasMore = FALSE;           // Make TRUE if necessary!
	m_fHasGradient = FALSE;

	LoadShapes();

	ASSERT(m_ShapeFile.WarpFieldCount() >= (m_cpItemCount.x*m_cpItemCount.y));
}

void CTextShapePalette::DrawItemBackground(CDC* pDC)
{
	CRect crDraw(m_crItems);

	Util::Draw3dBorder(
							 pDC,
							 crDraw,
							 GetSysColor(COLOR_BTNHILIGHT),
							 GetSysColor(COLOR_BTNSHADOW),
							 GetSysColor(COLOR_BTNFACE),
							 GetSysColor(COLOR_BTNTEXT),
							 GetSysColor(COLOR_WINDOW),
							 1,
							 TRUE,
							 TRUE,
							 FALSE);
}

void CTextShapePalette::DrawItem(CDC* pDC, int nItem, CRect crDraw, BOOL fSelected)
{
	CPoint cpItem = ItemToXY(nItem);

	CRect crInterior = crDraw;
	crInterior.InflateRect(-1, -1);

   if (nItem == ITEM_None)
   {
		Util::Draw3dBorder(
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

#if 0
		CPen cpPen;
		CBrush cbBrush;

		if (cpPen.CreateStockObject(BLACK_PEN))
		{
			if (cbBrush.CreateSolidBrush(RGB(128,128,128)))
			{
				int nContext = pDC->SaveDC();

				CSize czItem = ItemSize();
            czItem.cx -= 2*3;
            czItem.cy -= 2*3;
				crInterior.left = (crInterior.left+crInterior.right-czItem.cx)/2;
				crInterior.right = crInterior.left+czItem.cx;
				crInterior.top = (crInterior.top+crInterior.bottom-czItem.cy)/2;
				crInterior.bottom = crInterior.top+czItem.cy;

				if (nContext != 0)
				{
					pDC->SelectObject(&cpPen);
					pDC->SelectObject(&cbBrush);
					pDC->Rectangle(crInterior);

					pDC->RestoreDC(nContext);
				}
			}
		}
#else
      int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
      DrawNone(pDC, crInterior);
      pDC->SetBkMode(nOldBkMode);
#endif
   }
	else if (nItem == ITEM_More)
   {
		// No MORE!
		ASSERT(FALSE);
   }
   else
   {
		// Draw the interior
		crInterior.InflateRect(-3, -3);

		CFixed fxShapeBoundsWidth = MakeFixed(crInterior.Width());
		CFixed fxShapeBoundsHeight = MakeFixed(crInterior.Height());

		CWarpField* pWarpField;
		if (nItem < m_ShapeFile.WarpFieldCount() && (pWarpField = m_ShapeFile.WarpField(nItem)) != NULL)
		{
			TEXTMETRIC tmFont;
			GetPopupFontMetrics(tmFont);

			// Draw warp field preview. If there is no warp field, use a rectangle.

			CRect crWarpBounds(crInterior);

			crWarpBounds.right--;
			crWarpBounds.bottom--;

			pWarpField->Transform(crWarpBounds);

			COutlinePath Path;
			pWarpField->TransformedBounds()->Path(&Path);
			Path.Draw2(pDC,
						  RGB(128,128,128),
						  TRUE,
						  0,
						  RGB(0,0,0),
						  TRUE,
						  WINDING,
						  FALSE);
		}
   }

	COLORREF clHighlight;

	if (nItem == ITEM_None)
	{
		crDraw.left--;
		crDraw.top--;
		clHighlight = ::GetSysColor(fSelected ? COLOR_HIGHLIGHT : COLOR_BTNFACE);
	}
	else
	{
		crDraw.left += 2;
		crDraw.top += 2;
		crDraw.right -= 2;
		crDraw.bottom -= 2;
		clHighlight = ::GetSysColor(fSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW);
	}

   CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, clHighlight);
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

void CTextShapePalette::OnOK(void)
{
	// Get the current index and convert it to a shape.
#if 0
	if (m_nSelectedItem == ITEM_More)
	{
		// Do More!
		OnMore();
	}
	else
#endif
	if (m_nSelectedItem != -1)
	{
		m_nShape = (m_nSelectedItem == ITEM_None) ? -1 : m_nSelectedItem;
		CPopupPalette::OnOK();
	}
}

void CTextShapePalette::LoadShapes(void)
{
	// We assume that there are some shapes in the list.
	// So a count of zero means no shapes loaded.
	if (m_ShapeFile.WarpFieldCount() == 0)
	{
		CString csFile = GetGlobalPathManager()->ExpandPath("PMW.WRP");
		m_ShapeFile.Init(csFile);
		ASSERT(m_ShapeFile.WarpFieldCount() != 0);
	}
}

BOOL CTextShapePalette::IsNone(void)
{
	return m_nShape == -1;
}

void CTextShapePalette::SetNone(void)
{
	m_nShape = -1;
	m_nSelectedItem = ITEM_None;
}

void CTextShapePalette::OnMore(void)
{
   OnCancel();

#if 0
	CShapeDialog Dialog(&m_ShapeFile);
	Dialog.m_nChosen = m_nShape;

	if (Dialog.DoModal() == IDOK)
	{
		m_nShape = Dialog.m_nChosen;
		NotifyOwner();

		// We should already be destroyed.
		ASSERT(m_hWnd == NULL);
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////
//
// CPPhotoWorkshopMenuPalette
//
/////////////////////////////////////////////////////////////////////////////

// IDs for the menu item strings which will appear in the popup menu
const int CPPhotoWorkshopMenuPalette::m_nMenuChoices[] = 
{
	IDS_PHOTOWORKSHOP_MENU_CROP,
	IDS_PHOTOWORKSHOP_MENU_ADJUSTCOLOR,
	IDS_PHOTOWORKSHOP_MENU_BRIGHTFOCUS,
	IDS_PHOTOWORKSHOP_MENU_FIXFLAW,
	IDS_PHOTOWORKSHOP_MENU_ARTEFFECTS,
};

// how many items are there?
const int CPPhotoWorkshopMenuPalette::m_nNumberOfChoices = sizeof(m_nMenuChoices) / sizeof(m_nMenuChoices[0]);


// ****************************************************************************
//
//  Function Name:	CPPhotoWorkshopMenuPalette::CPPhotoWorkshopMenuPalette()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
// ****************************************************************************
//
CPPhotoWorkshopMenuPalette::CPPhotoWorkshopMenuPalette()
	:	m_nChoiceIndex(0)
{
}

// ****************************************************************************
//
//  Function Name:	CPPhotoWorkshopMenuPalette::Create()
//
//  Description:		Creates the popup window
//
//  Returns:			Success flag
//
// ****************************************************************************
//
BOOL CPPhotoWorkshopMenuPalette::Create( CWnd* pParent, UINT uID )
{
	return CPopupPalette::Create( pParent, uID );
}

// ****************************************************************************
//
//  Function Name:	CPPhotoWorkshopMenuPalette::InitItems()
//
//  Description:		Initializes the popup window
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPPhotoWorkshopMenuPalette::InitItems( CWnd* pParent )
{
	m_cpItemCount = CPoint( 1, m_nNumberOfChoices );

	// Compute the size of the widest name string. Since the strings
	// can be localized, we cannot assume one string is the widest.

	TEXTMETRIC tm;
	GetPopupFontMetrics( tm );

	CFont* pFont = GetPopupFont();
	ASSERT( pFont != NULL );

	CClientDC dc( NULL );		// Will happen before creation.
	CString csFormat;
	CString csText;
	int nMaxWidth = 0;

	CFont* pOldFont = dc.SelectObject( pFont );

	// load the menu choice strings long enough to "measure" them
	for (int i = 0; i < m_nNumberOfChoices; i++)
	{
		csFormat.LoadString( m_nMenuChoices[i] );
		csText.Format( csFormat );
		CSize czSize = Util::GetTextExtent( &dc, csText );
		if( czSize.cx > nMaxWidth )
		{
			nMaxWidth = czSize.cx;
		}
	}

	if( pOldFont )
	{
		dc.SelectObject( pOldFont );
	}

	// make the width a bit wider than the longest string
	m_czItemSize = CSize( nMaxWidth + (4 * tm.tmAveCharWidth), tm.tmHeight + 8 );

	// no fancy stuff
	m_fHasColor		= FALSE;
	m_fHasNone		= FALSE;
	m_fHasMore		= FALSE;
	m_fHasGradient	= FALSE;
}

// ****************************************************************************
//
//  Function Name:	CPPhotoWorkshopMenuPalette::DrawItemBackground()
//
//  Description:		Draws the background for the popup window items
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPPhotoWorkshopMenuPalette::DrawItemBackground( CDC* pDC )
{
	CRect crDraw( m_crItems );
	Util::Draw3dBorder(	pDC,
								crDraw,
								GetSysColor(COLOR_BTNHILIGHT),
								GetSysColor(COLOR_BTNSHADOW),
								GetSysColor(COLOR_BTNFACE),
								GetSysColor(COLOR_BTNTEXT),
								GetSysColor(COLOR_WINDOW),
								1,
								TRUE,
								TRUE,
								FALSE );
}

// ****************************************************************************
//
//  Function Name:	CPPhotoWorkshopMenuPalette::DrawItem()
//
//  Description:		Puts the menu items wher you cna see 'em
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPPhotoWorkshopMenuPalette::DrawItem( CDC* pDC, int nItem, CRect crDraw, BOOL fSelected )
{
	CRect crInterior = crDraw;
	crInterior.InflateRect( -3, -3 );

	TEXTMETRIC tmFont;
	GetPopupFontMetrics( tmFont );

	int nContext = pDC->SaveDC();
	if (nContext != 0)
	{
		CString csFormat;
		CString csText;

		// load the string resource
		csFormat.LoadString( m_nMenuChoices[nItem] );
		csText.Format( csFormat );

		// center it
		CRect crText( crInterior );
		crText.left = (2 * tmFont.tmAveCharWidth) + 1;

		// draw it
		DrawString( pDC, csText, crText, DT_LEFT | DT_SINGLELINE | DT_VCENTER );

		pDC->RestoreDC( nContext );
	}

	// draw the highlight
	COLORREF color;
	crDraw.left += 3;
	crDraw.top += 3;
	crDraw.right -= 3;
	crDraw.bottom -= 3;
	color = ::GetSysColor( fSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW );

   CPen Pen;
	Pen.CreatePen( PS_SOLID, 1, color );
	CPen* pOldPen = pDC->SelectObject( &Pen );

	if ( pOldPen )
	{
		CBrush Brush;
		Brush.CreateStockObject( NULL_BRUSH );
		CBrush* pOldBrush = pDC->SelectObject( &Brush );
		if ( pOldBrush )
		{
			pDC->Rectangle( crDraw );
			pDC->SelectObject( pOldBrush );
		}
		pDC->SelectObject( pOldPen );
	}
}

// ****************************************************************************
//
//  Function Name:	CPPhotoWorkshopMenuPalette::OnOK()
//
//  Description:		Called when the user clicks in the window
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPPhotoWorkshopMenuPalette::OnOK()
{
	// remember the selected item index
	m_nChoiceIndex = m_nSelectedItem;

	CPopupPalette::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
//
// CPMadeToOrderMenuPalette
//
/////////////////////////////////////////////////////////////////////////////

// IDs for the menu item strings which will appear in the popup menu
const int CPMadeToOrderMenuPalette::m_nMenuChoices[] =
{
	IDS_ADD_CUSTOMGRAPHICS_SEAL,
	IDS_ADD_CUSTOMGRAPHICS_TIMEPIECES
};

// how many items are there?
const int CPMadeToOrderMenuPalette::m_nNumberOfChoices = sizeof(m_nMenuChoices) / sizeof(m_nMenuChoices[0]);


// ****************************************************************************
//
//  Function Name:	CPMadeToOrderMenuPalette::CPMadeToOrderMenuPalette()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
// ****************************************************************************
//
CPMadeToOrderMenuPalette::CPMadeToOrderMenuPalette()
	:	m_nChoiceIndex(0)
{
}

// ****************************************************************************
//
//  Function Name:	CPMadeToOrderMenuPalette::Create()
//
//  Description:		Creates the popup window
//
//  Returns:			Success flag
//
// ****************************************************************************
//
BOOL CPMadeToOrderMenuPalette::Create( CWnd* pParent, UINT uID )
{
	return CPopupPalette::Create( pParent, uID );
}

// ****************************************************************************
//
//  Function Name:	CPMadeToOrderMenuPalette::InitItems()
//
//  Description:		Initializes the popup window
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPMadeToOrderMenuPalette::InitItems( CWnd* pParent )
{
	m_cpItemCount = CPoint( 1, m_nNumberOfChoices );

	// Compute the size of the widest name string. Since the strings
	// can be localized, we cannot assume one string is the widest.

	TEXTMETRIC tm;
	GetPopupFontMetrics( tm );

	CFont* pFont = GetPopupFont();
	ASSERT( pFont != NULL );

	CClientDC dc( NULL );		// Will happen before creation.
	CString csFormat;
	CString csText;
	int nMaxWidth = 0;

	CFont* pOldFont = dc.SelectObject( pFont );

	// load the menu choice strings long enough to "measure" them
	for (int i = 0; i < m_nNumberOfChoices; i++)
	{
		csFormat.LoadString( m_nMenuChoices[i] );
		csText.Format( csFormat );
		CSize czSize = Util::GetTextExtent( &dc, csText );
		if( czSize.cx > nMaxWidth )
		{
			nMaxWidth = czSize.cx;
		}
	}

	if( pOldFont )
	{
		dc.SelectObject( pOldFont );
	}

	// make the width a bit wider than the longest string
	m_czItemSize = CSize( nMaxWidth + (4 * tm.tmAveCharWidth), tm.tmHeight + 8 );

	// no fancy stuff
	m_fHasColor		= FALSE;
	m_fHasNone		= FALSE;
	m_fHasMore		= FALSE;
	m_fHasGradient	= FALSE;
}

// ****************************************************************************
//
//  Function Name:	CPMadeToOrderMenuPalette::DrawItemBackground()
//
//  Description:		Draws the background for the popup window items
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPMadeToOrderMenuPalette::DrawItemBackground( CDC* pDC )
{
	CRect crDraw( m_crItems );
	Util::Draw3dBorder(	pDC,
								crDraw,
								GetSysColor(COLOR_BTNHILIGHT),
								GetSysColor(COLOR_BTNSHADOW),
								GetSysColor(COLOR_BTNFACE),
								GetSysColor(COLOR_BTNTEXT),
								GetSysColor(COLOR_WINDOW),
								1,
								TRUE,
								TRUE,
								FALSE );
}

// ****************************************************************************
//
//  Function Name:	CPMadeToOrderMenuPalette::DrawItem()
//
//  Description:		Puts the menu items wher you cna see 'em
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPMadeToOrderMenuPalette::DrawItem( CDC* pDC, int nItem, CRect crDraw, BOOL fSelected )
{
	CRect crInterior = crDraw;
	crInterior.InflateRect( -3, -3 );

	TEXTMETRIC tmFont;
	GetPopupFontMetrics( tmFont );

	int nContext = pDC->SaveDC();
	if (nContext != 0)
	{
		CString csFormat;
		CString csText;

		// load the string resource
		csFormat.LoadString( m_nMenuChoices[nItem] );
		csText.Format( csFormat );

		// center it
		CRect crText( crInterior );
		crText.left = (2 * tmFont.tmAveCharWidth) + 1;

		// draw it
		DrawString( pDC, csText, crText, DT_LEFT | DT_SINGLELINE | DT_VCENTER );

		pDC->RestoreDC( nContext );
	}

	// draw the highlight
	COLORREF color;
	crDraw.left += 3;
	crDraw.top += 3;
	crDraw.right -= 3;
	crDraw.bottom -= 3;
	color = ::GetSysColor( fSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW );

   CPen Pen;
	Pen.CreatePen( PS_SOLID, 1, color );
	CPen* pOldPen = pDC->SelectObject( &Pen );

	if ( pOldPen )
	{
		CBrush Brush;
		Brush.CreateStockObject( NULL_BRUSH );
		CBrush* pOldBrush = pDC->SelectObject( &Brush );
		if ( pOldBrush )
		{
			pDC->Rectangle( crDraw );
			pDC->SelectObject( pOldBrush );
		}
		pDC->SelectObject( pOldPen );
	}
}

// ****************************************************************************
//
//  Function Name:	CPMadeToOrderMenuPalette::OnOK()
//
//  Description:		Called when the user clicks in the window
//
//  Returns:			Nothing
//
// ****************************************************************************
//
void CPMadeToOrderMenuPalette::OnOK()
{
	// remember the selected item index
	m_nChoiceIndex = m_nSelectedItem;

	CPopupPalette::OnOK();
}
