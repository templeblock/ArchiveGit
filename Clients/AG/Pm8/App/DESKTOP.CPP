/*
// $Workfile: DESKTOP.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/DESKTOP.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 20    2/20/99 8:57p Psasse
// Support for Vertical Banners
// 
// 19    2/19/99 2:55p Johno
// Moved warning rect stuff from CWebEditDesktop into CDocEditDesktop
// (base class)
// 
// 18    1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// Preliminary separate panel backgrounds
// 
// 17    1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 16    9/25/98 11:07a Mwilson
// added support for instruction page
// 
// 15    6/22/98 5:13p Rlovejoy
// Save & restore DC in DrawBackground().
// 
// 14    6/02/98 11:59a Johno
// Tweeks
// 
// 13    5/31/98 5:27p Johno
// Functional HTML warning rectangle
// 
// 12    3/17/98 3:37p Fredf
// 
// 11    3/17/98 11:56a Jayn
// Master-pages, double-sided flag.
// 
// 10    3/10/98 5:24p Jayn
// Master page changes
// 
// 9     2/24/98 9:18a Dennis
// Added CWebEditDesktop class to prevent Web Backgrounds from being
// erased with white.
// 
// 8     12/09/97 8:31a Jstoner
// modified desktop so that label outline is dotted and shows thru objects
// drawn on top of desktop
// affects label and sticker projects
// 
// 7     11/26/97 10:45a Jstoner
// minor fixes to desktop
// 
// 6     11/25/97 8:54a Jstoner
// added new calendar type - cal with pic on same page
// 
// 5     11/21/97 4:33p Hforman
// pass rc to DrawGuideLine()
// 
// 4     11/20/97 1:59p Jstoner
// 
// 3     11/17/97 9:27a Jstoner
// 
// 2     11/14/97 11:40a Hforman
// add guideline drawing
// 
//    Rev 1.1   26 Oct 1997 18:58:04   JOE
// modified calendar desktop to handle picture/calendar
// 
//    Rev 1.0   14 Aug 1997 15:20:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:06   Fred
// Initial revision.
// 
//    Rev 1.10   04 Apr 1997 15:35:32   Jay
// Half-fold card and web projects
// 
//    Rev 1.9   01 Apr 1997 16:33:38   Jay
// Beginning of half-fold cards.
// 
//    Rev 1.8   10 Feb 1997 13:40:08   Jay
// Fixes, extensions
// 
//    Rev 1.7   07 Feb 1997 16:59:40   Jay
// Start of using a region for the gray; no flicker.
// 
//    Rev 1.6   31 Jan 1997 16:31:08   Jay
// Removed some GdiFlush's I put in for debugging.
// 
//    Rev 1.5   31 Jan 1997 15:50:50   Jay
// New desktop background color
// 
//    Rev 1.4   06 Jan 1997 15:44:52   Jay
// Got rid of some TRACEs.
// 
//    Rev 1.3   09 Oct 1996 09:28:24   Jay
// Fixed select glitches on card shadow.
// 
//    Rev 1.2   28 Jul 1996 13:37:50   Jay
//  
// 
//    Rev 1.1   28 Jul 1996 11:28:42   Jay
// Brochure stuff
// 
//    Rev 1.0   14 Mar 1996 13:36:24   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:04   FRED
// Initial revision.
// 
//    Rev 1.8   06 Oct 1995 10:11:56   JAY
// New print code.
// 
//    Rev 1.7   13 Sep 1995 11:48:12   JAY
// New 32-bit changes
// 
//    Rev 1.6   06 Aug 1995 09:41:52   JAY
// Banner desktop.
// 
//    Rev 1.5   20 Jul 1995 12:51:22   JAY
// Label project. New paper info stuff. etc
// 
//    Rev 1.4   13 Jul 1995 13:42:48   JAY
// Now correctly checks Fold instead of Orientation for drawing center line.
// 
//    Rev 1.3   12 Jul 1995 17:45:06   JAY
// New project sub-types (esp. for cards).
// 
//    Rev 1.2   12 Jul 1995 08:52:34   FRED
// Card panel labels use Arial instead of Arial Bold.
// 
// 
//    Rev 1.1   12 Jul 1995 08:24:04   JAY
// New card style.
// 
//    Rev 1.0   30 Jun 1995 16:37:32   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "carddoc.h"
#include "pmwview.h"
#include "prntsize.h"            // for compute_tile_dimensions()

#include "desktop.h"
#include "paper.h"
#include "printer.h"
#include "utils.h"
#include "util.h"
#include "caldoc.h"
#include "docdib.h"		// For CDIBInfo
#include "bkgobj.h"              // For CBackgroundObject
#include "bmptiler.h"

extern CPalette *pOurPal;

/////////////////////////////////////////////////////////////////////////////
// CDocEditDesktop

// The default gray is one of the grays in the PrintMaster palette.
COLORREF CDocEditDesktop::m_clDesktop = PALETTERGB(234, 234, 234);

CDocEditDesktop::CDocEditDesktop(CPmwView* pView)
{
/*
// I don't want to call SetView() here because virtual functions in
// constructors, etc.
*/
   m_pView = pView;
   mWarningPBOX.Empty();
}

void CDocEditDesktop::DrawBackground(LPCRECT pClip, RedisplayContext* rc)
{
   rc->toggle(FALSE, (LPRECT)pClip);

   EraseBackground(pClip, rc);
   DrawPage(pClip, rc);
   DrawGuides(pClip, rc);        // So we see them while drawing.

   rc->toggle(TRUE, (LPRECT)pClip);
}

void CDocEditDesktop::DrawForeground(LPCRECT pClip, RedisplayContext* rc)
{
/* We don't need to toggle here because it's already done. */
   DrawGuides(pClip, rc);
}

/*
// Erase the background.
*/

void CDocEditDesktop::EraseBackground(LPCRECT pClip, RedisplayContext* rc)
{
   /* Erase the extent. */

   if (pClip != NULL)
   {
#if 1
		CBitmap bmpBackground;
		if (Util::LoadResourceBitmap(bmpBackground, MAKEINTRESOURCE(IDB_DESKTOP_BACKGROUND), pOurPal))
		{
			// Got the bitmap. Get a CDC to play with.
			CDC* pDC = CDC::FromHandle(rc->destination_hdc);
			if (pDC->SaveDC() > 0)
			{
				// Ask for the desktop region.
				CRgn rgnBackground;
				if (GetDesktopRegion(rc, rgnBackground, pClip))
				{
					pDC->SelectClipRgn(&rgnBackground);
				}

				CRect crClient;
				m_pView->GetClientRect(crClient);

				// Tile the background.
				CBmpTiler Tiler;
				Tiler.TileBmpInRect(*pDC, crClient, bmpBackground);

				pDC->RestoreDC(-1);
			}
		}
#else

      CBrush Brush;
      Brush.CreateSolidBrush(m_clDesktop);

      CRgn rgnBackground;

      if (GetDesktopRegion(rc, rgnBackground, pClip))
      {
         FillRgn(rc->destination_hdc, (HRGN)rgnBackground.GetSafeHandle(), (HBRUSH)Brush.GetSafeHandle());
      }
      else
      {
         FillRect(rc->destination_hdc, pClip, (HBRUSH)Brush.GetSafeHandle());
      }
#endif
   }
}

/*
// Draw the page.
*/

void CDocEditDesktop::DrawPage(LPCRECT pClip, RedisplayContext* rc)
{
   PBOX pbox;

   if (GetPageBox(pbox))
   {
      DrawShadowBox(pbox, rc);
   }
}

BOOL CDocEditDesktop::GetPageBox(PBOX& pbox)
{
   CPmwDoc* pDoc = m_pView->GetDocument();

   CPaperInfo* pPaperInfo = pDoc->GetPaperInfo();

   if (pPaperInfo != NULL)
   {
      CFixedRect Margins = pPaperInfo->Margins();
      CFixedPoint Dims = pPaperInfo->SlotDims();
      
      CFixedRect Bounds;

      if (pDoc->get_orientation() == LANDSCAPE)
      {
         GET_PMWAPP()->GetPrinterInfo(LANDSCAPE)->OrientRect(&Margins);
         
         CFixed t = Dims.x;
         Dims.x = Dims.y;
         Dims.y = t;
      }

      Bounds.Left = -Margins.Left;
      Bounds.Top = -Margins.Top;
      Bounds.Right = Bounds.Left + Dims.x;
      Bounds.Bottom  = Bounds.Top + Dims.y;

      pbox.x0 = MulFixed(Bounds.Left, PAGE_RESOLUTION);
      pbox.y0 = MulFixed(Bounds.Top, PAGE_RESOLUTION);
      pbox.x1 = MulFixed(Bounds.Right, PAGE_RESOLUTION);
      pbox.y1 = MulFixed(Bounds.Bottom, PAGE_RESOLUTION);

      return TRUE;
   }
   return FALSE;
}

void CDocEditDesktop::DrawShadowBox(PBOX pbox, RedisplayContext* rc)
{
   HDC hDC = rc->destination_hdc;
   SaveDC(hDC);

	CDesktopShape PageShape, ShadowShape;
	GetDesktopShapes(pbox, rc, PageShape, ShadowShape);

   // draw the page itself
	if (PageShape.GetSize() > 0)
	{
		SelectObject(hDC, GetStockObject(WHITE_BRUSH));    // NULL_BRUSH
		SelectObject(hDC, GetStockObject(BLACK_PEN));

		Polygon(hDC, PageShape.GetData(), PageShape.GetSize());
	}

   // draw a shadow for the page.
	if (ShadowShape.GetSize() > 0)
	{
		SelectObject(hDC, GetStockObject(DKGRAY_BRUSH));
		SelectObject(hDC, GetStockObject(NULL_PEN));

		Polygon(hDC, ShadowShape.GetData(), ShadowShape.GetSize());
	}

   RestoreDC(hDC, -1);
}

void CDocEditDesktop::GetDesktopShapes(PBOX pbox, RedisplayContext* rc, CDesktopShape& PageShape, CDesktopShape& ShadowShape)
{
   int nOriginX = rc->page_x_to_screen(0);
   int nOriginY = rc->page_y_to_screen(0);
   int nShadowX = rc->page_x_to_screen(PAGE_RESOLUTION/8) - nOriginX;
   int nShadowY = rc->page_y_to_screen(PAGE_RESOLUTION/8) - nOriginY;

   rc->pbox_to_screen(&pbox, TRUE);

   RECT rSlot;
   rc->convert_pbox(&pbox, &rSlot, NULL);

	// Compute the points for the page.
	PageShape.Add(CPoint(rSlot.left, rSlot.top));
	PageShape.Add(CPoint(rSlot.right, rSlot.top));
	PageShape.Add(CPoint(rSlot.right, rSlot.bottom));
	PageShape.Add(CPoint(rSlot.left, rSlot.bottom));

   // Figure out the points for the shadow.
	ShadowShape.Add(CPoint(rSlot.right, rSlot.top + nShadowY));
	ShadowShape.Add(CPoint(rSlot.right + nShadowX, rSlot.top + nShadowY));
	ShadowShape.Add(CPoint(rSlot.right + nShadowX, rSlot.bottom + nShadowY));
	ShadowShape.Add(CPoint(rSlot.left + nShadowX, rSlot.bottom + nShadowY));
	ShadowShape.Add(CPoint(rSlot.left + nShadowX, rSlot.bottom));
	ShadowShape.Add(CPoint(rSlot.right, rSlot.bottom));
}

/*
// Draw a panel.
*/

void CDocEditDesktop::DrawPanel(int nPanel, RedisplayContext* rc, BOOL fShadowed /*=FALSE*/)
{
   PBOX pbox;

   m_pView->GetDocument()->get_panel_world(&pbox, nPanel);

   if (fShadowed)
   {
      DrawShadowBox(pbox, rc);
   }
   else
   {
      rc->pbox_to_screen(&pbox, TRUE);

      RECT rPanel;
      rc->convert_pbox(&pbox, &rPanel, NULL);

   /* Draw the panel */

      Rectangle(rc->destination_hdc,
                rPanel.left,
                rPanel.top,
                rPanel.right,
                rPanel.bottom);
   }
}

//
// Draw an array of horizontal guides.
//

void CDocEditDesktop::DrawHorizontalGuides(Array* pGuideArray, RedisplayContext* rc, COLORREF Color /*=(COLORREF)-1*/)
{
	int nCount;
	if ((nCount = pGuideArray->count()) > 0)
	{
		DWORD* ptr = (DWORD*)pGuideArray->get_element(0);
		while (nCount-- > 0)
		{
			CPoint ptPos;
			ptPos.y = rc->page_y_to_screen(*ptr++);
			m_pView->DrawGuideLine(rc, CPmwView::GUIDE_HORIZ, ptPos, TRUE, Color);
		}
	}
}

//
// Draw an array of vertical guides.
//

void CDocEditDesktop::DrawVerticalGuides(Array* pGuideArray, RedisplayContext* rc, COLORREF Color /*=(COLORREF)-1*/)
{
	int nCount;
	if ((nCount = pGuideArray->count()) > 0)
	{
		DWORD* ptr = (DWORD*)pGuideArray->get_element(0);
		while (nCount-- > 0)
		{
			CPoint ptPos;
			ptPos.x = rc->page_x_to_screen(*ptr++);
			m_pView->DrawGuideLine(rc, CPmwView::GUIDE_VERT, ptPos, TRUE, Color);
		}
	}
}

void CDocEditDesktop::DrawGuides(LPCRECT pClip, RedisplayContext* rc)
{
   CPmwDoc* pDoc = m_pView->GetDocument();

   HDC hDC = rc->destination_hdc;

   HPEN hPen = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));

   if (hPen != NULL)
   {
      SaveDC(hDC);
      SelectObject(hDC, hPen);
      SelectObject(hDC, GetStockObject(NULL_BRUSH));

      DrawPanels(rc);

      RestoreDC(hDC, -1);
      DeleteObject(hPen);
   }

   // draw Guidelines
   if (GET_PMWAPP()->ShowGuides())
   {
		// Draw master page guides first.
		ObjectList* pMasterItems = pDoc->GetMasterItems();
		if (pMasterItems != NULL)
		{
			DrawHorizontalGuides(pMasterItems->GetHorizGuides(), rc, CPmwView::m_clrMasterGuides);
			DrawVerticalGuides(pMasterItems->GetVertGuides(), rc, CPmwView::m_clrMasterGuides);
		}

		// Draw the normal page guides.
		DrawHorizontalGuides(pDoc->get_horiz_guides(), rc);
		DrawVerticalGuides(pDoc->get_vert_guides(), rc);
   }

	// Now draw the special text if we are on a Instruction page.
	if (pDoc->EditingInstructionPage())
	{
		// We want to label this guy as Instruction Page

		DWORD dwPage = pDoc->MasterPageIndex();
		UINT uID = IDS_InstructionPage;

		CString csText;
		TRY
		{
			Util::LoadString(uID, csText);
			PBOX PageBound;
			if (GetPageBox(PageBound))
			{
				CRect crPage;
				rc->pbox_to_screen(&PageBound, TRUE);
				if (rc->convert_pbox(&PageBound, crPage, NULL))
				{
					HDC hDC = rc->destination_hdc;
					CSize czText;
					if (::GetTextExtentPoint32(hDC, csText, csText.GetLength(), &czText))
					{
						// Set the parameters for the text draw.
						int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT);
						COLORREF clOldText = ::SetTextColor(hDC, RGB(0, 0, 0));
						CRect crDraw;
						int nGap = rc->page_y_to_screen(PAGE_RESOLUTION/8)
										- rc->page_y_to_screen(0);

						crDraw.left = (crPage.left + crPage.right - czText.cx)/2;
						crDraw.right = crDraw.left+czText.cx;

						// Draw on bottom of page.
						crDraw.top = crPage.bottom+nGap;
						crDraw.bottom = crDraw.top+czText.cy;
						::DrawText(hDC,
									  csText,
									  csText.GetLength(),
									  &crDraw,
									  DT_LEFT | DT_NOCLIP);

						// Draw on top of page.
						crDraw.bottom = crPage.top-nGap;
						crDraw.top = crDraw.bottom-czText.cy;
						::DrawText(hDC,
									  csText,
									  csText.GetLength(),
									  &crDraw,
									  DT_LEFT | DT_NOCLIP);

						// Restore our changed parameters.
						::SetTextColor(hDC, clOldText);
						::SetBkMode(hDC, nOldBkMode);
					}
				}
			}
		}
		END_TRY
	}
	// If we have a warning rect, draw that now too
	// This is set by the design checker dialog, 
	// and therefor is only possible for projects supporting HTML.
	// CWebView::AfterPositionChange() is a different method used only for web projects
	// for highlighting objects that are dragged to overlap ().
   if (!mWarningPBOX.IsEmpty())
   {
      if (rc->IsScreen() == TRUE)
      {
         HDC hDC = rc->destination_hdc;
         
         CPen Pen;
         Pen.CreatePen(PS_INSIDEFRAME, WARNING_RECT_WIDTH, RGB(255, 0, 0));
			HPEN hOldPen;
			if ((hOldPen = (HPEN)::SelectObject(hDC, Pen.GetSafeHandle())) != NULL)
			{
	         CBrush   Brush;
            Brush.CreateStockObject(NULL_BRUSH);
            HBRUSH   hOldBrush;
            if ((hOldBrush = (HBRUSH)::SelectObject(hDC, Brush.GetSafeHandle())) != NULL)
            {
               CRect crDrawBox;
	            PBOX pBox = mWarningPBOX;
	            rc->pbox_to_screen(&pBox, TRUE);
	            if (rc->convert_pbox(&pBox, &crDrawBox, NULL))
	            {
                  ::Rectangle (hDC, crDrawBox.left, crDrawBox.top, crDrawBox.right, crDrawBox.bottom);
               }
               ::SelectObject(hDC, hOldBrush);
            }
			   ::SelectObject(hDC, hOldPen);
		   }
	   }				
   }
}

/*
// Draw all panels that should be drawn.
*/

void CDocEditDesktop::DrawPanels(RedisplayContext* rc, BOOL fShadowed /*=FALSE*/)
{
/* Draw all panels. */
   int nPanels = m_pView->GetDocument()->number_of_panels();
   for (int nPanel = 0; nPanel < nPanels; nPanel++)
   {
      DrawPanel(nPanel, rc, fShadowed);
   }
}

/*
// Set the view we're attached to.
*/

void CDocEditDesktop::SetView(CPmwView* pView)
{
   m_pView = pView;
}

/*
// Return a region we can paint to erase the part that's not the page.
*/

BOOL CDocEditDesktop::GetDesktopRegion(RedisplayContext* rc, CRgn& Region, LPCRECT pClip /*=NULL*/)
{
   // Start with the passed clip rect.
   if (pClip == NULL)
   {
      pClip = &rc->destination_rect;
   }
   Region.CreateRectRgn(pClip->left, pClip->top, pClip->right, pClip->bottom);

   // Create an exclude region for the page.
   PBOX Bound;
   if (GetPageBox(Bound))
	{
		CDesktopShape PageShape, ShadowShape;
		GetDesktopShapes(Bound, rc, PageShape, ShadowShape);

		if (PageShape.GetSize() > 0)
		{
			CRgn crgnPage;
			crgnPage.CreatePolygonRgn(PageShape.GetData(), PageShape.GetSize(), ALTERNATE);
         Region.CombineRgn(&Region, &crgnPage, RGN_DIFF);
		}

		if (ShadowShape.GetSize() > 0)
		{
			CRgn crgnShadow;
			crgnShadow.CreatePolygonRgn(ShadowShape.GetData(), ShadowShape.GetSize(), ALTERNATE);
         Region.CombineRgn(&Region, &crgnShadow, RGN_DIFF);
		}
   }

   return TRUE;
}

void
CDocEditDesktop::SetWarningRect(PBOX pb)
{
   mWarningPBOX = pb;
}

/////////////////////////////////////////////////////////////////////////////
// CCardEditDesktop

CCardEditDesktop::CCardEditDesktop(CPmwView* pView)
   : CDocEditDesktop(pView)
{
}

/*
// Draw all panels that should be drawn.
*/

void CCardEditDesktop::DrawPanels(RedisplayContext* rc, BOOL fShadowed /*=FALSE*/)
{
   ASSERT(!fShadowed);
   int nPanel = m_pView->GetDocument()->get_current_panel();
   ASSERT(nPanel != -1);
}

/*
// Draw the page while editing a card.
*/

void CCardEditDesktop::DrawPage(LPCRECT pClip, RedisplayContext* rc)
{
   int nPanel;
   if ((nPanel = m_pView->GetDocument()->get_current_panel()) >= 0)
   {
      // Only draw the panel we're editing.
      DrawPanel(nPanel, rc, TRUE);
   }
}

/*
// Draw a panel.
*/

void CCardEditDesktop::DrawPanel(int nPanel, RedisplayContext* rc, BOOL fShadowed /*=FALSE*/)
{
   CDocEditDesktop::DrawPanel(nPanel, rc, fShadowed);
   if (nPanel == CARD_PANEL_Inside)
   {
      CCardDoc* pDoc = (CCardDoc*)m_pView->GetDocument();
      PBOX World;

      pDoc->get_panel_world(&World, nPanel);

      rc->pbox_to_screen(&World, TRUE);
      CRect crPanel;
      if (rc->convert_pbox(&World, &crPanel, NULL))
      {
      /* We want to draw the dashed line for the fold. */
         HDC hDC = rc->destination_hdc;
         HPEN hPen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));

         if (hPen != NULL)
         {
            SaveDC(hDC);
            SelectObject(hDC, hPen);

            if (pDoc->FoldType() == CARD_SideFold)
            {
            /* Draw the line down the middle. */
               int xCenter = (crPanel.left + crPanel.right)/2;
               MoveToEx(hDC, xCenter, crPanel.top+1, NULL);
               LineTo(hDC, xCenter, crPanel.bottom-1);
            }
            else
            {
            /* Draw the line across the middle. */
               int yCenter = (crPanel.top + crPanel.bottom)/2;
               MoveToEx(hDC, crPanel.left+1, yCenter, NULL);
               LineTo(hDC, crPanel.right-1, yCenter);
            }
            RestoreDC(hDC, -1);

            DeleteObject(hPen);
         }
      }
   }
}

BOOL CCardEditDesktop::GetPageBox(PBOX& pbox)
{
   // Return the bound for the current panel.
   CPmwDoc* pDoc = m_pView->GetDocument();
   pDoc->get_panel_world(&pbox, pDoc->get_current_panel());

   return TRUE;
}

CPhotoProjectsEditDesktop::CPhotoProjectsEditDesktop(CPmwView* pView)
   : CLabelEditDesktop(pView)
{
}

/////////////////////////////////////////////////////////////////////////////
// CLabelEditDesktop

CLabelEditDesktop::CLabelEditDesktop(CPmwView* pView)
   : CDocEditDesktop(pView)
{
}

void CLabelEditDesktop::DrawShadowBox(PBOX pbox, RedisplayContext* rc)
{
   rc->pbox_to_screen(&pbox, TRUE);

   RECT rSlot;
   rc->convert_pbox(&pbox, &rSlot, NULL);

   CPmwDoc* pDoc = m_pView->GetDocument();
   CPaperInfo* pPaperInfo = pDoc->GetPaperInfo();
	
   pPaperInfo->DrawSlot(rc->destination_hdc, rSlot, pDoc->get_orientation() == LANDSCAPE, TRUE, NULL, m_clDesktop, TRUE);
}

/*
// Return a region we can paint to erase the part that's not the page.
*/

BOOL CLabelEditDesktop::GetDesktopRegion(RedisplayContext* rc, CRgn& Region, LPCRECT pClip /*=NULL*/)
{
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CBannerEditDesktop

CBannerEditDesktop::CBannerEditDesktop(CPmwView* pView)
   : CDocEditDesktop(pView)
{
}

/*
// Draw the page while editing a banner.
*/

BOOL CBannerEditDesktop::GetPageBox(PBOX& pbox)
{
   CPmwDoc* pDoc = m_pView->GetDocument();
   PPNT Dims = pDoc->get_dimensions();
   CPaperInfo* pPaperInfo = pDoc->GetPaperInfo();

   CFixedRect Margins = pPaperInfo->Margins();

   /* Note the left/top and right/bottom swap since it's landscape. */

	if(pDoc->DocumentRecord()->GetProjectSubtype() == BANNER_Vertical)
	{
		pbox.x0 = 0-MulFixed(Margins.Left, PAGE_RESOLUTION);
		pbox.y0 = 0-MulFixed(Margins.Top, PAGE_RESOLUTION);
		pbox.x1 = Dims.x + MulFixed(Margins.Right, PAGE_RESOLUTION);
		pbox.y1 = Dims.y + MulFixed(Margins.Bottom, PAGE_RESOLUTION);

	}
	else
	{
		pbox.x0 = 0-MulFixed(Margins.Top, PAGE_RESOLUTION);
		pbox.y0 = 0-MulFixed(Margins.Left, PAGE_RESOLUTION);
		pbox.x1 = Dims.x + MulFixed(Margins.Bottom, PAGE_RESOLUTION);
		pbox.y1 = Dims.y + MulFixed(Margins.Right, PAGE_RESOLUTION);
	}

   return TRUE;
}

void CBannerEditDesktop::DrawGuides(LPCRECT pClip, RedisplayContext* rc)
{
//   CDocEditDesktop::DrawGuides(pClip, rc);

   CPmwDoc* pDoc = m_pView->GetDocument();

   HDC hDC = rc->destination_hdc;

   PCOORD Width, Height;
   PCOORD FirstWidth, FirstHeight;

   compute_tile_dimensions(pDoc, 100.0,
                           &Width, &Height,
                           &FirstWidth, &FirstHeight);

   /*
   // Compute the margins.
   */

   CFixedRect Margins = pDoc->GetPaperInfo()->Margins();

   /* Note the left/top and right/bottom swap since it's landscape. */

   PBOX MarginBox;

	if(pDoc->DocumentRecord()->GetProjectSubtype() == BANNER_Vertical)
	{
		MarginBox.x0 = MulFixed(Margins.Left, PAGE_RESOLUTION);
		MarginBox.y0 = MulFixed(Margins.Top, PAGE_RESOLUTION);
		MarginBox.x1 = MulFixed(Margins.Right, PAGE_RESOLUTION);
		MarginBox.y1 = MulFixed(Margins.Bottom, PAGE_RESOLUTION);
	}
	else
	{
		MarginBox.x0 = MulFixed(Margins.Top, PAGE_RESOLUTION);
		MarginBox.y0 = MulFixed(Margins.Left, PAGE_RESOLUTION);
		MarginBox.x1 = MulFixed(Margins.Bottom, PAGE_RESOLUTION);
		MarginBox.y1 = MulFixed(Margins.Right, PAGE_RESOLUTION);
	}
   /* Create the pen to use for drawing lines. */

   HPEN hPen = CreatePen(PS_DOT, 1, RGB(192, 192, 192));

   if (hPen != NULL)
   {
      SaveDC(hDC);
      SelectObject(hDC, hPen);

      PPNT Dims = pDoc->get_dimensions();
      PCOORD x0 = rc->source_pbox.x0;
      PCOORD y0 = rc->source_pbox.y0;
      PCOORD x1 = rc->source_pbox.x1;
      PCOORD y1 = rc->source_pbox.y1;

      if (x0 < 0 - MarginBox.x0)
      {
         x0 = 0 - MarginBox.x0;
      }

      if (y0 < 0 - MarginBox.y0)
      {
         y0 = 0 - MarginBox.y0;
      }

      if (x1 > Dims.x + MarginBox.x1)
      {
         x1 = Dims.x + MarginBox.x1;
      }

      if (y1 > Dims.y + MarginBox.y1)
      {
         y1 = Dims.y + MarginBox.y1;
      }
		if(pDoc->DocumentRecord()->GetProjectSubtype() == BANNER_Vertical)
		{
			for (PCOORD y = FirstHeight; y < y1; y += Height)
			{
			/* Only draw within viewed source. */
				if (y < y0)
				{
					continue;
				}

				PBOX pbox;
				pbox.x0 = x0;
				pbox.x1 = x1;
				pbox.y0 = y;
				pbox.y1 = y;
				rc->pbox_to_screen(&pbox, TRUE);

				CRect crDraw;

				if (rc->convert_pbox(&pbox, &crDraw, NULL))
				{
					MoveToEx(hDC, crDraw.left, crDraw.bottom, NULL);
					LineTo(hDC, crDraw.right, crDraw.bottom);
				}
			}
		}
		else
		{
			for (PCOORD x = FirstWidth; x < x1; x += Width)
			{
			/* Only draw within viewed source. */
				if (x < x0)
				{
					continue;
				}

				PBOX pbox;
				pbox.x0 = x;
				pbox.x1 = x;
				pbox.y0 = y0;
				pbox.y1 = y1;
				rc->pbox_to_screen(&pbox, TRUE);

				CRect crDraw;

				if (rc->convert_pbox(&pbox, &crDraw, NULL))
				{
					MoveToEx(hDC, crDraw.left, crDraw.top, NULL);
					LineTo(hDC, crDraw.left, crDraw.bottom);
				}
			}
		}
      RestoreDC(hDC, -1);
      DeleteObject(hPen);
   }

   CDocEditDesktop::DrawGuides(pClip, rc);
}

/////////////////////////////////////////////////////////////////////////////
// CBrochureEditDesktop

CBrochureEditDesktop::CBrochureEditDesktop(CPmwView* pView)
   : CDocEditDesktop(pView)
{
}

void CBrochureEditDesktop::DrawGuides(LPCRECT pClip, RedisplayContext* rc)
{
   // do this at end, since we want guidelines on top
   //   CDocEditDesktop::DrawGuides(pClip, rc);

   CPmwDoc* pDoc = m_pView->GetDocument();

   // Get the number of panels.
   int nPanels = pDoc->DocumentRecord()->GetProjectSubtype();
   if (nPanels > 1)
   {
      /* Create the pen to use for drawing lines. */
      HPEN hPen = CreatePen(PS_DOT, 1, RGB(192, 192, 192));
      if (hPen != NULL)
      {
         HDC hDC = rc->destination_hdc;

         SaveDC(hDC);
         SelectObject(hDC, hPen);

         /*
         // Compute the margins.
         */

         CFixedRect Margins = pDoc->GetPaperInfo()->Margins();

         /* Note the left/top and right/bottom swap since it's landscape. */

         PBOX MarginBox;
         MarginBox.x0 = MulFixed(Margins.Top, PAGE_RESOLUTION);
         MarginBox.y0 = MulFixed(Margins.Left, PAGE_RESOLUTION);
         MarginBox.x1 = MulFixed(Margins.Bottom, PAGE_RESOLUTION);
         MarginBox.y1 = MulFixed(Margins.Right, PAGE_RESOLUTION);

         if (pDoc->get_orientation() == LANDSCAPE)
         {
            // Reverse everything.
            PCOORD t;

            t = MarginBox.x0;
            MarginBox.x0 = MarginBox.y0;
            MarginBox.y0 = t;

            t = MarginBox.x1;
            MarginBox.x1 = MarginBox.y1;
            MarginBox.y1 = t;
         }

         PPNT Dims = pDoc->get_dimensions();
         PCOORD x0 = rc->source_pbox.x0;
         PCOORD y0 = rc->source_pbox.y0;
         PCOORD x1 = rc->source_pbox.x1;
         PCOORD y1 = rc->source_pbox.y1;

         if (x0 < 0)
         {
            x0 = 0;
         }

         if (y0 < 0)
         {
            y0 = 0;
         }

         if (x1 > Dims.x)
         {
            x1 = Dims.x;
         }

         if (y1 > Dims.y)
         {
            y1 = Dims.y;
         }

         Dims.x += MarginBox.x0 + MarginBox.x1;
         Dims.y += MarginBox.y0 + MarginBox.y1;

         for (int nPanel = 1; nPanel < nPanels; nPanel++)
         {
            PBOX pbox;

            if (pDoc->get_orientation() == PORTRAIT)
            {
               PCOORD y = scale_pcoord(Dims.y, nPanel, nPanels) - MarginBox.y0;
               if (y > y1)
               {
                  break;
               }
               if (y < y0)
               {
                  continue;
               }
               pbox.y0 = y;
               pbox.y1 = y;
               pbox.x0 = x0;
               pbox.x1 = x1;
            }
            else
            {
               PCOORD x = scale_pcoord(Dims.x, nPanel, nPanels) - MarginBox.x0;

               // Only draw within viewed source.

               if (x > x1)
               {
                  break;
               }

               if (x < x0)
               {
                  continue;
               }

               pbox.x0 = x;
               pbox.x1 = x;
               pbox.y0 = y0;
               pbox.y1 = y1;
            }

            rc->pbox_to_screen(&pbox, FALSE);

            CRect crDraw;

            if (rc->convert_pbox(&pbox, &crDraw, NULL))
            {
               MoveToEx(hDC, crDraw.left, crDraw.top, NULL);
               LineTo(hDC, crDraw.right, crDraw.bottom);
            }
         }

         RestoreDC(hDC, -1);
         DeleteObject(hPen);
      }
   }

   CDocEditDesktop::DrawGuides(pClip, rc);
}



/////////////////////////////////////////////////////////////////////////////
// CCalendarEditDesktop

CCalendarEditDesktop::CCalendarEditDesktop(CPmwView* pView)
   : CDocEditDesktop(pView)
{
}

/*
// Draw the page while editing a Calendar.
*/

BOOL CCalendarEditDesktop::GetPageBox(PBOX& pbox)
{
	CPmwDoc* pDoc = m_pView->GetDocument();
	PPNT Dims = pDoc->get_dimensions();
	PCOORD x = 0;
	PCOORD y = 0;

	short pic = ((CCalendarDoc*)pDoc)->GetPictureStyle();
	if (pic == CAL_PICTURE_TOP || pic == CAL_PICTURE_LEFT)
	{
		DWORD dwAtPage = pDoc->CurrentPageIndex();
		
		// for first and last page of multi-page calendar with pictures,
		// half-of the page area is used...arbitrarily decided top half.
		if (dwAtPage == 0 || dwAtPage == pDoc->NumberOfPages()- 1)
		{
			if (pic == CAL_PICTURE_TOP)
			{
				x = 0;
				y = Dims.y / 2;
			}
			else if (pic == CAL_PICTURE_LEFT)
			{
				y = 0;
				x = Dims.x / 2;
			}
		}
	}

	CPaperInfo* pPaperInfo = pDoc->GetPaperInfo();

  CFixedRect Margins = pPaperInfo->Margins();

  /* Note the left/top and right/bottom swap since it's landscape. */
  pbox.x0 = x-MulFixed(Margins.Top, PAGE_RESOLUTION);
  pbox.y0 = y-MulFixed(Margins.Left, PAGE_RESOLUTION);
  pbox.x1 = Dims.x + MulFixed(Margins.Bottom, PAGE_RESOLUTION);
  pbox.y1 = Dims.y + MulFixed(Margins.Right, PAGE_RESOLUTION);

  return TRUE;
}

void CCalendarEditDesktop::DrawGuides(LPCRECT pClip, RedisplayContext* rc)
{
//   CDocEditDesktop::DrawGuides(pClip, rc);

   CPmwDoc* pDoc = m_pView->GetDocument();

   HDC hDC = rc->destination_hdc;

   PCOORD Width, Height;
   PCOORD FirstWidth, FirstHeight;

   compute_tile_dimensions(pDoc, 100.0,
                           &Width, &Height,
                           &FirstWidth, &FirstHeight);

	BOOL bDoublePageSize = FALSE;
   /*
   // Compute the margins.
   */

   CFixedRect Margins = pDoc->GetPaperInfo()->Margins();

   /* Note the left/top and right/bottom swap since it's landscape. */

   PBOX MarginBox;
   MarginBox.x0 = MulFixed(Margins.Top, PAGE_RESOLUTION);
   MarginBox.y0 = MulFixed(Margins.Left, PAGE_RESOLUTION);
   MarginBox.x1 = MulFixed(Margins.Bottom, PAGE_RESOLUTION);
   MarginBox.y1 = MulFixed(Margins.Right, PAGE_RESOLUTION);

	PPNT Dims = pDoc->get_dimensions();

	HPEN hPenS = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));

	PBOX pboxBox;

	pboxBox.x0 = 0;
	pboxBox.x1 = Dims.x;
	pboxBox.y0 = 0;
	pboxBox.y1 = Dims.y;
   /* Create the pen to use for drawing lines. */
   HPEN hPen = CreatePen(PS_DOT, 1, RGB(192, 192, 192));

   if (hPen != NULL)
   {
      SaveDC(hDC);
      SelectObject(hDC, hPen);

      PCOORD x0 = rc->source_pbox.x0;
      PCOORD y0 = rc->source_pbox.y0;
      PCOORD x1 = rc->source_pbox.x1;
      PCOORD y1 = rc->source_pbox.y1;

		// correct for margins
      if (x0 < 0 - MarginBox.x0)
      {
         x0 = 0 - MarginBox.x0;
      }

      if (y0 < 0 - MarginBox.y0)
      {
         y0 = 0 - MarginBox.y0;
      }

      if (x1 > Dims.x + MarginBox.x1)
      {
         x1 = Dims.x + MarginBox.x1;
      }

      if (y1 > Dims.y + MarginBox.y1)
      {
         y1 = Dims.y + MarginBox.y1;
      }

//		::SetBkColor(hDC, m_clDesktop);			
		TEXTMETRIC tm;
		GetTextMetrics (hDC, &tm);
		int iCharWidth = tm.tmAveCharWidth;
		int iCharHeight = tm.tmHeight;
		DWORD dwAtPage = pDoc->CurrentPageIndex();

		int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT);

		short picture = ((CCalendarDoc*)pDoc)->GetPictureStyle();	
		PBOX pbox;
		CRect crDraw;
		int page;

		switch (picture)
		{
			case CAL_PICTURE_LEFT:
			{
				pbox.x0 = Dims.x / 2; //FirstWidth;
				pbox.x1 = Dims.x / 2;// FirstWidth;
				pbox.y0 = y0;
				pbox.y1 = y1;
				rc->pbox_to_screen(&pbox, TRUE);

				// draw dashed line
				if (rc->convert_pbox(&pbox, &crDraw, NULL))
				{
					if (dwAtPage > 0 && dwAtPage < pDoc->NumberOfPages()-1)
					{
						MoveToEx(hDC, crDraw.left, crDraw.top, NULL);
						LineTo(hDC, crDraw.left, crDraw.bottom);
					}

					if (dwAtPage == 0)
					{
						page = dwAtPage + 1;
						CString strR;
						strR.Format("%d", page);
						TextOut(hDC, crDraw.left + (iCharWidth*3), crDraw.top - iCharHeight, (LPCTSTR)strR, strR.GetLength());
						pboxBox.x0 = pboxBox.x1 / 2;
						bDoublePageSize = FALSE;
					}
					else if (dwAtPage == pDoc->NumberOfPages()-1)
					{
						page = dwAtPage * 2;
						CString t;
						t.Format("%d", page);
						TextOut(hDC, crDraw.left + (iCharWidth*3), crDraw.top - iCharHeight, (LPCTSTR)t, t.GetLength());
						pboxBox.x0 = pboxBox.x1 / 2;
						bDoublePageSize = FALSE;
					}
					else
					{
						page = dwAtPage * 2;
						// add page numbers
						CString strR;
						CString strL;
						strR.Format("%d", page);
						strL.Format("%d", page + 1);
						int rlen = strR.GetLength();
						int llen = strL.GetLength();
						TextOut(hDC, crDraw.left - ((iCharWidth*3)+(iCharWidth*rlen)), crDraw.top - iCharHeight, (LPCTSTR)strR, rlen);
						TextOut(hDC, crDraw.left + (iCharWidth*3), crDraw.top - iCharHeight, (LPCTSTR)strL, llen);
						bDoublePageSize = TRUE;
					}
				}
				break;
			}
			case CAL_PICTURE_TOP:
			{
				pbox.x0 = x0;
				pbox.x1 = x1;
				pbox.y0 = Dims.y / 2;//FirstHeight;
				pbox.y1 = Dims.y / 2; //FirstHeight;
				rc->pbox_to_screen(&pbox, TRUE);

				// draw dashed line
				if (rc->convert_pbox(&pbox, &crDraw, NULL))
				{
					if (dwAtPage > 0 && dwAtPage < pDoc->NumberOfPages()-1)
					{
						MoveToEx(hDC, crDraw.left, crDraw.bottom, NULL);
						LineTo(hDC, crDraw.right, crDraw.bottom);
					}
								
					// add page numbers
					CString strU;
					CString strL;

					if (dwAtPage == 0)
					{
						page = dwAtPage +1;
						strL.Format("%d", page);
						int llen = strL.GetLength();
						TextOut(hDC, crDraw.left - iCharWidth*(llen+3), crDraw.bottom + iCharHeight, (LPCTSTR)strL, 1);
						pboxBox.y0 = pboxBox.y1 / 2;
						bDoublePageSize = FALSE;
					}
					else if (dwAtPage == pDoc->NumberOfPages()-1)
					{
						page = dwAtPage * 2;
						CString t;
						t.Format("%d", page);
						int tlen = t.GetLength();
						TextOut(hDC, crDraw.left - iCharWidth*(tlen+3), crDraw.bottom + iCharHeight, (LPCTSTR)t, t.GetLength());
						pboxBox.y0 = pboxBox.y1 / 2;
						bDoublePageSize = FALSE;
					}
					else
					{
						page = dwAtPage * 2;
						strL.Format("%d", page + 1);
						strU.Format("%d", page);
						int rlen = strL.GetLength();
						int ulen = strU.GetLength();
						TextOut(hDC, crDraw.left - iCharWidth*(rlen+3), crDraw.bottom + iCharHeight, (LPCTSTR)strL, rlen);
						TextOut(hDC, crDraw.left - iCharWidth*(ulen+3), crDraw.bottom - iCharHeight*2, (LPCTSTR)strU, ulen);
						bDoublePageSize = TRUE;
					}
				}
				break;
			}
			case CAL_PICTURE_NONE:
			case CAL_PICTURE_SAME:
			{
				bDoublePageSize = FALSE;
				pbox.x0 = x0;
				pbox.x1 = x1;
				pbox.y0 = FirstHeight;
				pbox.y1 = FirstHeight;
				rc->pbox_to_screen(&pbox, TRUE);
				// draw dashed line
				if (rc->convert_pbox(&pbox, &crDraw, NULL))
				{
					// add page numbers
//					CString strText;
//					sprintf(strText.GetBuffer(10), "%d", dwAtPage+1);
//					strText.ReleaseBuffer();
//					int len = strText.GetLength();
//					TextOut(hDC, crDraw.left - iCharWidth*(len + 1), crDraw.bottom - iCharHeight, strText, len);
				}
				break;
			}		
			default:
				break;
		}

		::SetBkMode(hDC, nOldBkMode);

	 	 // setup margin drawing box here, modify in special cases below

		if (hPenS)
		{
			SelectObject(hDC, hPenS);
						
			if (bDoublePageSize) // 2 pages
			{

				PBOX pboxCal;
				PBOX pboxPic;
							
				PPNT half;
				half.x = Dims.x / 2;
				half.y = Dims.y / 2;

				pboxPic.x0 = 0;
				pboxPic.y0 = 0;
				pboxCal.x1 = Dims.x;
				pboxCal.y1 = Dims.y;

				if (picture == CAL_PICTURE_TOP)
				{
					pboxPic.x1 = Dims.x;
					pboxPic.y1 = (Dims.y / 2) - MarginBox.x1; // bottom margin

					pboxCal.x0 = 0;
					pboxCal.y0 = half.y + MarginBox.x0; // top margin
				}
				else // pic is on left
				{
					pboxPic.x1 = (Dims.x / 2) - MarginBox.y1; // right margin
					pboxPic.y1 = Dims.y;

					pboxCal.x0 = half.x + MarginBox.y0; // left margin
					pboxCal.y0 = 0;
				}
				DrawPBOX(&pboxPic, rc);
				DrawPBOX(&pboxCal, rc);
			}
			else // single page
			{
				DrawPBOX(&pboxBox, rc);
			}
		}
		RestoreDC(hDC, -1);
		DeleteObject(hPen);
		DeleteObject(hPenS);
   }
}

void CCalendarEditDesktop::DrawPBOX(PBOX * pBox, RedisplayContext* rc)
{
	HDC hDC = rc->destination_hdc;
	CRect crDrawBox;
	rc->pbox_to_screen(pBox, TRUE);
	if (rc->convert_pbox(pBox, &crDrawBox, NULL))
	{
		// draw box
		MoveToEx(hDC, crDrawBox.left, crDrawBox.top, NULL);
		LineTo(hDC, crDrawBox.right, crDrawBox.top);
		LineTo(hDC, crDrawBox.right, crDrawBox.bottom);
		LineTo(hDC, crDrawBox.left, crDrawBox.bottom);
		LineTo(hDC, crDrawBox.left, crDrawBox.top);
	}				
}

void CLabelEditDesktop::DrawBackground(LPCRECT pClip, RedisplayContext* rc)
{
   rc->toggle(FALSE, (LPRECT)pClip);
   EraseBackground(pClip, rc);
   // draw slot with solid pen and white brush
	 PBOX pbox;
   if (GetPageBox(pbox))
   {
		 rc->pbox_to_screen(&pbox, TRUE);
		 RECT rSlot;
		rc->convert_pbox(&pbox, &rSlot, NULL);

		CPmwDoc* pDoc = m_pView->GetDocument();
		CPaperInfo* pPaperInfo = pDoc->GetPaperInfo();
		pPaperInfo->DrawSlot(rc->destination_hdc, rSlot, pDoc->get_orientation() == LANDSCAPE, TRUE, NULL, m_clDesktop, FALSE);
	 }
	 
   DrawGuides(pClip, rc);        // So we see them while drawing.

   rc->toggle(TRUE, (LPRECT)pClip);
}

void CLabelEditDesktop::DrawForeground(LPCRECT pClip, RedisplayContext* rc)
{
	/* We don't need to toggle here because it's already done. */
	DrawPage(pClip, rc); // draws slot with dot pen and null brush
  DrawGuides(pClip, rc);
}

// CWebEditDesktop Methods

CWebEditDesktop::CWebEditDesktop(CPmwView* pView)
   : CDocEditDesktop(pView)
{
}

void CWebEditDesktop::DrawBackground(LPCRECT pClip, RedisplayContext* rc)
{
   CRgn     rgnBackground;
   BOOL     bCreatedRgn = FALSE;
   CPmwDoc* pDoc = m_pView->GetDocument();
   CDC      dc;

   // Get ready to use DC
   dc.Attach(rc->destination_hdc);

   int nSavedDC = dc.SaveDC();
   ASSERT(nSavedDC);

   if (nSavedDC)
	{
		// Setup clipping region for Web Page background so that it is excluded
		CBackgroundObject *pBackgroundObject = pDoc->GetBackgroundObject();
		if(pBackgroundObject && pBackgroundObject->HasBackground())
			{
				CRect crBackgroundArea, crIntersection, crClip = *pClip;

				PBOX pboxScreenBound = pBackgroundObject->get_unrotated_bound();
   			rc->pbox_to_screen(&pboxScreenBound, TRUE);
				// Convert PBOX to CRect
				rc->convert_pbox(&pboxScreenBound, &crBackgroundArea, NULL);

				crIntersection.IntersectRect(crClip, crBackgroundArea);


				if (rgnBackground.CreateRectRgn(crIntersection.left,
													  crIntersection.top,
													  crIntersection.right,
													  crIntersection.bottom))
				{
	//            hrgnOldRegion = ::GetClipRgn(rc->destination_hdc);
					dc.SelectClipRgn(&rgnBackground, RGN_DIFF);
					bCreatedRgn = TRUE;
				}
			}


		// Do default Desktop Draw Behavior
		CDocEditDesktop::DrawBackground(pClip, rc);
	}

   // restore clipping region
   if (nSavedDC)
   {
      dc.RestoreDC(nSavedDC);
   }
   if(bCreatedRgn)
   {
      rgnBackground.DeleteObject();
   }

   // Done with DC
   dc.Detach();
}

/////////////////////////////////////////////////////////////////////////////
// CNewsletterEditDesktop Methods

CNewsletterEditDesktop::CNewsletterEditDesktop(CPmwView* pView)
   : CDocEditDesktop(pView)
{
}

void CNewsletterEditDesktop::DrawGuides(LPCRECT pClip, RedisplayContext* rc)
{
	CDocEditDesktop::DrawGuides(pClip, rc);
	// Now draw the special text if we are on a master page.
	CPmwDoc* pDoc = m_pView->GetDocument();
	int nSided = pDoc->GetSided();
	if (pDoc->EditingMasterPage())
	{
		// We want to label this guy.
		// There are three possibilities:
		// (1) On left master page: draw "left master page" string left of page.
		// (2) On right master page: draw "right master page" string right of page.
		// (3) On master page (single-sided): draw "master page" string right of page.
		// So, the first thing we need to do is determine what text we need to
		// show (what kind of page we are on).

		DWORD dwPage = pDoc->MasterPageIndex();
		UINT uID;

		if (nSided == DocumentRecord::SingleSided)
		{
			uID = IDS_MasterPage;
			ASSERT(dwPage == 1);		// Single-sided uses right master page.
		}
		else
		{
			// Must be double-sided.
			ASSERT(nSided == DocumentRecord::DoubleSided);

			uID = (dwPage == 0) ? IDS_LeftMasterPage : IDS_RightMasterPage;
		}

		CString csText;
		TRY
		{
			Util::LoadString(uID, csText);
			PBOX PageBound;
			if (GetPageBox(PageBound))
			{
				CRect crPage;
				rc->pbox_to_screen(&PageBound, TRUE);
				if (rc->convert_pbox(&PageBound, crPage, NULL))
				{
					HDC hDC = rc->destination_hdc;
					CSize czText;
					if (::GetTextExtentPoint32(hDC, csText, csText.GetLength(), &czText))
					{
						// Set the parameters for the text draw.
						int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT);
						COLORREF clOldText = ::SetTextColor(hDC, RGB(0, 0, 0));
						CRect crDraw;
#if 0
						crDraw.top = crPage.top;
						if (dwPage == 0)
						{
							// Draw on the left.
							crDraw.left = crPage.left - 16 - czText.cx;
						}
						else
						{
							// Draw on the right.
							ASSERT(dwPage == 1);
							crDraw.left = crPage.right + 16;
						}
#endif
						int nGap = rc->page_y_to_screen(PAGE_RESOLUTION/8)
										- rc->page_y_to_screen(0);
						crDraw.left = (crPage.left + crPage.right - czText.cx)/2;
						crDraw.right = crDraw.left+czText.cx;

						// Draw on bottom of page.
						crDraw.top = crPage.bottom+nGap;
						crDraw.bottom = crDraw.top+czText.cy;
						::DrawText(hDC,
									  csText,
									  csText.GetLength(),
									  &crDraw,
									  DT_LEFT | DT_NOCLIP);

						// Draw on top of page.
						crDraw.bottom = crPage.top-nGap;
						crDraw.top = crDraw.bottom-czText.cy;
						::DrawText(hDC,
									  csText,
									  csText.GetLength(),
									  &crDraw,
									  DT_LEFT | DT_NOCLIP);

						// Restore our changed parameters.
						::SetTextColor(hDC, clOldText);
						::SetBkMode(hDC, nOldBkMode);
					}
				}
			}
		}
		END_TRY
	}
}

//
// Draw the page.
//

void CNewsletterEditDesktop::GetDesktopShapes(PBOX pbox, RedisplayContext* rc, CDesktopShape& PageShape, CDesktopShape& ShadowShape)
{
	CPmwDoc* pDoc = m_pView->GetDocument();
	// Compute how big the fold is.
	CPoint cpFold = GetFoldSize(rc);

	int nSided = pDoc->GetSided();
	if (nSided == DocumentRecord::SingleSided || cpFold.x <=0 || cpFold.y <= 0)
	{
		CDocEditDesktop::GetDesktopShapes(pbox, rc, PageShape, ShadowShape);
	}
	else
	{
		// Do something special for left/right.
		int nLeft = GetLRStatus();

		int nOriginX = rc->page_x_to_screen(0);
		int nOriginY = rc->page_y_to_screen(0);
		int nShadowX = rc->page_x_to_screen(PAGE_RESOLUTION/8) - nOriginX;
		int nShadowY = rc->page_y_to_screen(PAGE_RESOLUTION/8) - nOriginY;

		rc->pbox_to_screen(&pbox, TRUE);

		RECT rSlot;
		rc->convert_pbox(&pbox, &rSlot, NULL);

		if (nLeft)
		{
			// Create a page with a folder left corner.
			// Compute the points for the page.
			PageShape.Add(CPoint(rSlot.left+cpFold.x, rSlot.top));
			PageShape.Add(CPoint(rSlot.right, rSlot.top));
			PageShape.Add(CPoint(rSlot.right, rSlot.bottom));
			PageShape.Add(CPoint(rSlot.left, rSlot.bottom));
			PageShape.Add(CPoint(rSlot.left, rSlot.top+cpFold.y));

			// Figure out the points for the shadow.
			ShadowShape.Add(CPoint(rSlot.right, rSlot.top + nShadowY));
			ShadowShape.Add(CPoint(rSlot.right + nShadowX, rSlot.top + nShadowY));
			ShadowShape.Add(CPoint(rSlot.right + nShadowX, rSlot.bottom + nShadowY));
			ShadowShape.Add(CPoint(rSlot.left + nShadowX, rSlot.bottom + nShadowY));
			ShadowShape.Add(CPoint(rSlot.left + nShadowX, rSlot.bottom));
			ShadowShape.Add(CPoint(rSlot.right, rSlot.bottom));
		}
		else
		{
			// Compute the points for the page.
			PageShape.Add(CPoint(rSlot.left, rSlot.top));
			PageShape.Add(CPoint(rSlot.right-cpFold.x, rSlot.top));
			PageShape.Add(CPoint(rSlot.right, rSlot.top+cpFold.y));
			PageShape.Add(CPoint(rSlot.right, rSlot.bottom));
			PageShape.Add(CPoint(rSlot.left, rSlot.bottom));
			PageShape.Add(CPoint(rSlot.left, rSlot.top+cpFold.y));

			// Figure out the points for the shadow.
			ShadowShape.Add(CPoint(rSlot.right, rSlot.top+cpFold.y));
			ShadowShape.Add(CPoint(rSlot.right + nShadowX, rSlot.top+cpFold.y + nShadowY));
			ShadowShape.Add(CPoint(rSlot.right + nShadowX, rSlot.bottom + nShadowY));
			ShadowShape.Add(CPoint(rSlot.left + nShadowX, rSlot.bottom + nShadowY));
			ShadowShape.Add(CPoint(rSlot.left + nShadowX, rSlot.bottom));
			ShadowShape.Add(CPoint(rSlot.right, rSlot.bottom));
		}
	}
}

//
// Get the 
//

int CNewsletterEditDesktop::GetLRStatus(void)
{
	int nLeft;
	CPmwDoc* pDoc = m_pView->GetDocument();
	if (pDoc->EditingMasterPage())
	{
		// Index 0 is left master page
		// Index 1 is right master page
		nLeft = (pDoc->MasterPageIndex() & 1) ^ 1;
	}
	else
	{
		// Page 1 (index 0) is right.
		// Page 2 (index 1) is left.
		// Page 3 (index 2) is right.
		// ...
		nLeft = (pDoc->CurrentPageIndex() & 1);
	}
	return nLeft;
}

void CNewsletterEditDesktop::DrawPage(LPCRECT pClip, RedisplayContext* rc)
{
	CDocEditDesktop::DrawPage(pClip, rc);
	CPmwDoc* pDoc = m_pView->GetDocument();
	int nSided = pDoc->GetSided();
	if (nSided != DocumentRecord::SingleSided)
	{
		PBOX pbox;
		if (GetPageBox(pbox))
		{
			// Draw the flap in the corner.
			ASSERT(nSided == DocumentRecord::DoubleSided);

			int nLeft = GetLRStatus();
			CPoint cpFold = GetFoldSize(rc);
			if (cpFold.x > 0 && cpFold.y > 0)
			{
				rc->pbox_to_screen(&pbox, TRUE);

				RECT rSlot;
				rc->convert_pbox(&pbox, &rSlot, NULL);

				HDC hDC = rc->destination_hdc;

				CPen cpBlack;
				cpBlack.CreateStockObject(BLACK_PEN);
				HPEN hOldPen;
				if ((hOldPen = (HPEN)::SelectObject(hDC, cpBlack.GetSafeHandle())) != NULL)
				{
					if (nLeft)
					{
						// Draw left corner.
						POINT p;
						::MoveToEx(hDC, rSlot.left, rSlot.top+cpFold.y, &p);
						::LineTo(hDC, rSlot.left+cpFold.x, rSlot.top+cpFold.y);
						::LineTo(hDC, rSlot.left+cpFold.x, rSlot.top);
					}
					else
					{
						// Draw right corner.
						POINT p;
						::MoveToEx(hDC, rSlot.right, rSlot.top+cpFold.y, &p);
						::LineTo(hDC, rSlot.right-cpFold.x, rSlot.top+cpFold.y);
						::LineTo(hDC, rSlot.right-cpFold.x, rSlot.top);
					}
					::SelectObject(hDC, hOldPen);
				}
			}
		}
	}
}

//
// Compute how big the fold is.
//

CPoint CNewsletterEditDesktop::GetFoldSize(RedisplayContext* rc)
{
   CPaperInfo* pPaperInfo = m_pView->GetDocument()->GetPaperInfo();

	PCOORD FoldSize;

   if (pPaperInfo != NULL)
   {
		// Pick minimum of all margins.
      CFixedRect Margins = pPaperInfo->Margins();
		CFixed l = Margins.Left;
		if (Margins.Right < l) l = Margins.Right;
		if (Margins.Top < l) l = Margins.Top;
		if (Margins.Bottom < l) l = Margins.Bottom;
		FoldSize = InchesToPage(l);
	}
	else
	{
		FoldSize = 3*PAGE_RESOLUTION/8;
	}

	return CPoint(rc->page_x_to_screen(FoldSize) - rc->page_x_to_screen(0) - 2,
					  rc->page_y_to_screen(FoldSize) - rc->page_y_to_screen(0) - 2);
}
