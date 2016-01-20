#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "stkdsktp.h"

#include "paper.h"



/////////////////////////////////////////////////////////////////////////////
// CStickerEditDesktop

CStickerEditDesktop::CStickerEditDesktop(CPmwView* pView)
   : CDocEditDesktop(pView)
{
}

void CStickerEditDesktop::DrawShadowBox(PBOX pbox, RedisplayContext* rc)
{
	// do nothing here	
}

/*
// Return a region we can paint to erase the part that's not the page.
*/

BOOL CStickerEditDesktop::GetDesktopRegion(RedisplayContext* rc, CRgn& Region, LPCRECT pClip /*=NULL*/)
{
   return FALSE;
}

BOOL CStickerEditDesktop::GetPageBox(PBOX& pbox)
{
	CPmwDoc* pDoc = m_pView->GetDocument();
	pDoc->get_panel_world(&pbox, 0);
	return TRUE;
}

void CStickerEditDesktop::DrawPanel(int nPanel, RedisplayContext* rc, BOOL fShadowed /*=FALSE*/)
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


void CStickerEditDesktop::DrawBackground(LPCRECT pClip, RedisplayContext* rc)
{
   rc->toggle(FALSE, (LPRECT)pClip);

   EraseBackground(pClip, rc);
   CPmwDoc* pDoc = m_pView->GetDocument();

   CPaperInfo* pPaperInfo = pDoc->GetPaperInfo();
	 // draw page with white pen and white brush for background
	 PBOX pbox;
   if (GetPageBox(pbox))
   {
			rc->pbox_to_screen(&pbox, TRUE);

			RECT rSlot;
			rc->convert_pbox(&pbox, &rSlot, NULL);
 			CDC dc;
			dc.Attach(rc->destination_hdc);

			pPaperInfo->DrawPage(&dc, rSlot, pDoc->get_orientation() == LANDSCAPE, NULL, DRAW_BACKGROUND);
			dc.Detach();
	 }

   DrawGuides(pClip, rc);        // So we see them while drawing.

   rc->toggle(TRUE, (LPRECT)pClip);
}

void CStickerEditDesktop::DrawForeground(LPCRECT pClip, RedisplayContext* rc)
{
	/* We don't need to toggle here because it's already done. */

	 CPmwDoc* pDoc = m_pView->GetDocument();

   CPaperInfo* pPaperInfo = pDoc->GetPaperInfo();
	
	 // draw page slots with dot pen and null brush so labels can always be seen
	 PBOX pbox;
   if (GetPageBox(pbox))
   {
			rc->pbox_to_screen(&pbox, TRUE);

			RECT rSlot;
			rc->convert_pbox(&pbox, &rSlot, NULL);
			CDC dc;
			dc.Attach(rc->destination_hdc);
 	
			pPaperInfo->DrawPage(&dc, rSlot, pDoc->get_orientation() == LANDSCAPE, NULL, DRAW_FOREGROUND);
			dc.Detach();
	 }

	 DrawGuides(pClip, rc);
}

void CStickerEditDesktop::DrawGuides(LPCRECT pClip, RedisplayContext* rc)
{
   CPmwDoc* pDoc = m_pView->GetDocument();

   HDC hDC = rc->destination_hdc;

   
   // draw Guidelines
   if (GET_PMWAPP()->ShowGuides())
   {
      CPoint ptPos(0,0);
      DWORD* ptr;
      Array* pGuideArray = pDoc->get_horiz_guides();
      for (int i = 0; i < pGuideArray->count(); i++)
      {
         ptr = (DWORD*)pGuideArray->get_element(i);
         ptPos.y = rc->page_y_to_screen(*ptr);
         m_pView->DrawGuideLine(rc, CPmwView::GUIDE_HORIZ, ptPos, TRUE);
      }

      pGuideArray = pDoc->get_vert_guides();
      for (i = 0; i < pGuideArray->count(); i++)
      {
         ptr = (DWORD*)pGuideArray->get_element(i);
         ptPos.x = rc->page_x_to_screen(*ptr);
         m_pView->DrawGuideLine(rc, CPmwView::GUIDE_VERT, ptPos, TRUE);
      }
   }
}