/*
// $Header: /PM8/App/RULER.CPP 1     3/03/99 6:11p Gbeddow $
//
// ruler.h : Ruler class
//
// $Log: /PM8/App/RULER.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 3     6/02/98 12:09p Cboggio
// Code changes for Metric rulers (centimetres instead of inches) for
// localization
// 
// 2     11/14/97 11:36a Hforman
// added guide creation
// 
//    Rev 1.0   14 Aug 1997 15:25:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:20   Fred
// Initial revision.
// 
//    Rev 1.10   05 Jun 1997 08:52:26   Jay
// Don't draw lines if not active.
// 
//    Rev 1.9   12 May 1997 10:37:52   Jay
// Ruler lines are actually ints -> cast now fixes it for 32-bit.
// 
//    Rev 1.8   12 Mar 1997 15:52:18   Jay
// Rotation, etc.
// 
//    Rev 1.7   25 Feb 1997 16:21:20   Jay
// Got rid of an ASSERT.
// 
//    Rev 1.6   19 Feb 1997 16:53:24   Jay
// Snapping
// 
//    Rev 1.5   10 Feb 1997 13:40:12   Jay
// Fixes, extensions
// 
//    Rev 1.4   07 Feb 1997 16:58:38   Jay
// Ruler origin (for cards, etc); offscreen drawing.
// 
//    Rev 1.3   07 Feb 1997 14:51:54   Jay
// Nows adds shadow marks for objects while moving/resizing.
// 
//    Rev 1.2   06 Feb 1997 17:07:04   Jay
// Cursor lines
// 
//    Rev 1.1   06 Feb 1997 12:57:26   Jay
// Ruler now displays marks and numbers.
// 
//    Rev 1.0   06 Feb 1997 09:13:26   Jay
// Rulers!
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "ruler.h"
#include "util.h"

#include <afxpriv.h>

#ifdef LOCALIZE
#include <math.h>
#include "clocale.h"
#endif

#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define  COLOR_BTNHILIGHT  COLOR_BTNHIGHLIGHT
#endif
#endif

CFont CRuler::m_cfXRuler;
CFont CRuler::m_cfYRuler;

/*
// Constructor for a ruler.
*/

CRuler::CRuler(CPmwView* pView, int nType)
{
   m_pView = pView;
   m_nType = nType;
	m_LastUnit = 0;
}

/*
// Destructor for a ruler.
*/

CRuler::~CRuler()
{
}

BOOL CRuler::Create(UINT uID)
{
   return INHERITED::Create(NULL,
                            "Ruler",
                            WS_CHILD | WS_CLIPSIBLINGS,
                            CRect(0, 0, 0, 0),
                            m_pView,
                            uID);
}

/*
// Update a ruler's size.
*/

void CRuler::Update(void)
{
   // Get the client rect.
   CRect crClient;
   m_pView->GetClientRect(crClient);

   // Update our position.
   CRect crPosition;

   TEXTMETRIC tm;

   GetFontMetrics(tm);

   int nHeight = tm.tmHeight + 5 + 6;
   int nWidth = nHeight;

   switch (m_nType)
   {
      case Horizontal:
      {
         crPosition.left = crClient.left + nWidth;    // Leave room for Y ruler.
         crPosition.top = crClient.top;
         crPosition.right = crClient.right;
         crPosition.bottom = crPosition.top + nHeight;
         break;
      }
      case Vertical:
      {
         crPosition.left = crClient.left;
         crPosition.top = crClient.top + nHeight;     // Leave room for X ruler.
         crPosition.right = crPosition.left + nWidth;
         crPosition.bottom = crClient.bottom;
         break;
      }
      case Corner:
      {
         crPosition.left = crClient.left;
         crPosition.top = crClient.top;
         crPosition.right = crPosition.left + nWidth;
         crPosition.bottom = crPosition.top + nHeight;
         break;
      }
      default:
      {
         // Bad ruler type!
         ASSERT(FALSE);
         break;
      }
   }

   SetWindowPos(NULL,
                crPosition.left,
                crPosition.top,
                crPosition.Width(),
                crPosition.Height(),
                SWP_NOACTIVATE|SWP_NOZORDER);
   Invalidate();
}

/*
// Get the font to use for horizontal rulers.
*/

CFont& CRuler::GetXFont(void)
{
   if (m_cfXRuler.GetSafeHandle() == NULL)
   {
      // We need to initialize the font.
      LOGFONT logFont;
      memset(&logFont, 0, sizeof(logFont));

      // Since design guide says toolbars are fixed height so is the font.
      logFont.lfHeight = -9;
      logFont.lfWeight = FW_NORMAL;
      logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;

      CString strDefaultFont;
      strDefaultFont = "Arial";         // Cheat a little (for now!)
      lstrcpy(logFont.lfFaceName, strDefaultFont);

      if (!m_cfXRuler.CreateFontIndirect(&logFont))
      {
         TRACE0("Could not create font for rulers.\n");
         ASSERT(FALSE);
      }
   }

   return m_cfXRuler;
}

/*
// Get the font to use for vertical rulers.
*/

CFont& CRuler::GetYFont(void)
{
   if (m_cfYRuler.GetSafeHandle() == NULL)
   {
      // We need to initialize the font.
      LOGFONT logFont;
      memset(&logFont, 0, sizeof(logFont));

      // Since design guide says toolbars are fixed height so is the font.
      logFont.lfHeight = -9;
      logFont.lfWeight = FW_NORMAL;
      logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;

      logFont.lfEscapement = logFont.lfOrientation = 900;

      CString strDefaultFont;
      strDefaultFont = "Arial";         // Cheat a little (for now!)
      lstrcpy(logFont.lfFaceName, strDefaultFont);

      if (!m_cfYRuler.CreateFontIndirect(&logFont))
      {
         TRACE0("Could not create font for rulers.\n");
         ASSERT(FALSE);
      }
   }

   return m_cfYRuler;
}

void CRuler::GetFontMetrics(TEXTMETRIC& tm)
{
   CFont& Font = GetXFont();

   CClientDC dc(NULL);     // May happen before creation.
   CFont* pOldFont = dc.SelectObject(&Font);
   if (pOldFont != NULL)
   {
      dc.GetTextMetrics(&tm);
      dc.SelectObject(pOldFont);
   }
}

BEGIN_MESSAGE_MAP(CRuler, CWnd)
   //{{AFX_MSG_MAP(CRuler)
   ON_WM_PAINT()
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
   ON_WM_RBUTTONDOWN()
   ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifdef LOCALIZE
static double ComputeMetricUnit(double Space, double Unit, int *factor)
{
   if (Space < Unit/10.0)
   {
	  *factor = 10;
	  return Unit/10.0;
   }
   if (Space < Unit/5.0)
   {
	  *factor = 5;
      return Unit/5.0;
   }
   if (Space < Unit/2.0)
   {
	  *factor = 2;
      return Unit/2.0;
   }
   // Does not fit here.
  *factor = 0;
   return 0.0;
}

PCOORD ComputeMetricPosition(double dUnit, int MarkNum)
{
	PCOORD Pos;
	double dPos = dUnit* (double) MarkNum;
	Pos = (PCOORD) Round(dPos);
	return Pos;
}
#endif

static PCOORD ComputeUnit(PCOORD Space, PCOORD Unit)
{
   if (Space < Unit/8)
   {
      return Unit/8;
   }
   if (Space < Unit/4)
   {
      return Unit/4;
   }
   if (Space < Unit/2)
   {
      return Unit/2;
   }
   // Does not fit here.
   return 0;
}

void CRuler::OnPaint() 
{
   CPaintDC PaintDC(this);

   // Get our client rect.
   CRect crClient;
   GetClientRect(crClient);

   // Prepare for off-screen drawing.
   CDC* pDC = NULL;
   CDC dc;
   CBitmap Bitmap;
   CBitmap* pOldBitmap = NULL;

   if (dc.CreateCompatibleDC(&PaintDC))
   {
      if (Bitmap.CreateCompatibleBitmap(&PaintDC,
                                        crClient.Width(),
                                        crClient.Height()))
      {
         pOldBitmap = dc.SelectObject(&Bitmap);
         pDC = &dc;
      }
      else
      {
         dc.DeleteDC();
      }
   }

   if (pDC == NULL)
   {
      pDC = &PaintDC;
      // Restore any lines currently shown.
      ToggleLines(pDC, m_cwaLines);
   }

   // Make all of our brushes and pens.
   CBrush cbBackground;
   cbBackground.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

   CBrush cbInner;
   cbInner.CreateSolidBrush(GetSysColor(COLOR_WINDOW));

   CPen cpBlack;
   cpBlack.CreateStockObject(BLACK_PEN);

   CPen cpHighlight;
   cpHighlight.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT));

   CPen cpShadow;
   cpShadow.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));

   CPen cpFace;
   cpFace.CreatePen(PS_SOLID, 0, GetSysColor(COLOR_BTNFACE));

   // Draw the fill for the ruler.
// pDC->FillRect(crClient, &cbBackground);

   // Draw the line around the ruler.
   CPen cpFrame;
   cpFrame.CreateStockObject(BLACK_PEN);

   CPen* pOldPen = pDC->SelectObject(&cpFrame);
   if (pOldPen != NULL)
   {
      pDC->MoveTo(crClient.left, crClient.bottom-1);
      pDC->LineTo(crClient.right-1, crClient.bottom-1);
      pDC->LineTo(crClient.right-1, crClient.top-1);
      pDC->SelectObject(pOldPen);
   }

   //
   // Draw the ruler content.
   //

   RedisplayContext* rc = m_pView->get_rc();
   CPmwDoc* pDoc = m_pView->GetDocument();

   // Get the current panel extent.
   PBOX Panel;
   pDoc->get_panel_world(&Panel, pDoc->get_current_panel());

   PBOX pbox = Panel;
   rc->pbox_to_screen(&pbox, TRUE);

   CRect crPanel;
   rc->convert_pbox(&pbox, &crPanel, NULL);

   // Switch coordinate systems.
   // Compute the translation value as we do this.
   CPoint Adjust;
   Adjust.x = -crPanel.left;
   Adjust.y = -crPanel.top;
   m_pView->ClientToScreen(crPanel);
   ScreenToClient(crPanel);
   Adjust.x += crPanel.left;
   Adjust.y += crPanel.top;

   // Compute the units we will be using.
   // The ruler length corresponds to an axis of the source_pbox.
   // We can use this fact

   CRect crUnit;
   PBOX UnitPbox;
   crUnit.left = 0;
   crUnit.top = 0;
   crUnit.right = 8;
   crUnit.bottom = 8;
   rc->screen_to_pbox(&crUnit, &UnitPbox);

   static long lScaleToTry[] =
   {
      PAGE_RESOLUTION*1L,
      PAGE_RESOLUTION*2L,
      PAGE_RESOLUTION*4L,
      PAGE_RESOLUTION*8L,
      PAGE_RESOLUTION*12L,
      PAGE_RESOLUTION*24L,
      PAGE_RESOLUTION*48L
   };

   PCOORD Unit = 0;
   PCOORD Mark;

   for (int n = 0; n < sizeof(lScaleToTry)/sizeof(lScaleToTry[0]); n++)
   {
      Mark = lScaleToTry[n];
      if ((Unit = ComputeUnit(UnitPbox.x1 - UnitPbox.x0, Mark)) != 0)
      {
         break;
      }
   }

   if (Unit == 0)
   {
//    ASSERT(FALSE);
      Unit = Mark = 96L*PAGE_RESOLUTION;
   }


#ifdef LOCALIZE
#define METRIC_PAGE_RESOLUTION  ((long)(((double)PAGE_RESOLUTION)/INCHES_TO_CENTIMETRES))

   static int nScales[] = {1L,5L,10L,20L,50L,100L};
/*
   static double dScaleToTry[] =
   {
       (double) (PAGE_RESOLUTION*1L)/INCHES_TO_CENTIMETRES,
       (double) (PAGE_RESOLUTION*2L)/INCHES_TO_CENTIMETRES,
       (double) (PAGE_RESOLUTION*5L)/INCHES_TO_CENTIMETRES,
       (double) (PAGE_RESOLUTION*10L)/INCHES_TO_CENTIMETRES,
       (double) (PAGE_RESOLUTION*20L)/INCHES_TO_CENTIMETRES,
       (double) (PAGE_RESOLUTION*50L)/INCHES_TO_CENTIMETRES,
       (double) (PAGE_RESOLUTION*100L)/INCHES_TO_CENTIMETRES
   };
*/
   double dUnit = 0.0;
   double dMark = (double) Mark;

   int factor = 0;
   int Scale = 0;
   int RulerLen = UnitPbox.x1 - UnitPbox.x0;

   if (CurrentLocale.UseMetric())
   {
		for (int n = 0; n < sizeof(nScales)/sizeof(nScales[0]); n++)
		{
			dMark = Round((double) nScales[n]* ((double) PAGE_RESOLUTION/INCHES_TO_CENTIMETRES));
			if ((dUnit = ComputeMetricUnit((double)(UnitPbox.x1 - UnitPbox.x0), dMark, &factor)) != 0.0)
			{
				Scale = nScales[n];
				break;
			}
		}


		if (dUnit == 0.0)
		{
//		  ASSERT(FALSE);
	      dUnit = dMark = 100L*METRIC_PAGE_RESOLUTION;
		  factor = 0;
		  Scale = 0;
		}
 
	   Unit = Round(dUnit);
   	   Mark = Round(dMark);
   }

#endif // end of LOCALIZE section


   switch (m_nType)
   {
      case Horizontal:
      {
         // Draw the 3D borders and the inner common lines.

         CRect crInner = crClient;
         crInner.top += 3;
         crInner.bottom -= 5;
         crInner.right--;        // for right border.

         CPen* pOldPen;
         pOldPen = pDC->SelectObject(&cpHighlight);
         // Top 3D border
         pDC->MoveTo(crClient.left, crClient.top);
         pDC->LineTo(crClient.right-1, crClient.top);

         // Bottom containing line
         pDC->MoveTo(crClient.left, crInner.bottom);
         pDC->LineTo(crClient.right-1, crInner.bottom);

         // Bottom 3D border
         pDC->SelectObject(&cpShadow);
         pDC->MoveTo(crClient.left, crClient.bottom-2);
         pDC->LineTo(crClient.right-1, crClient.bottom-2);

         // Bump highs.
         pDC->SelectObject(&cpFace);
         pDC->MoveTo(crClient.left, crClient.top+1);
         pDC->LineTo(crClient.right-1, crClient.top+1);
         pDC->MoveTo(crClient.left, crClient.top+2);
         pDC->LineTo(crClient.right-1, crClient.top+2);
         pDC->MoveTo(crClient.left, crClient.bottom-3);
         pDC->LineTo(crClient.right-1, crClient.bottom-3);
         pDC->MoveTo(crClient.left, crClient.bottom-4);
         pDC->LineTo(crClient.right-1, crClient.bottom-4);

         // Draw the inner space.

         if (crPanel.left >= crClient.left)
         {
            crInner.left = (crPanel.left > crClient.right)
                              ? crClient.right
                              : crPanel.left;

            if (crPanel.left > crClient.left)
            {
               // We need to draw gray on the left.

               pDC->SelectObject(&cpShadow);
               pDC->MoveTo(crClient.left, crInner.top);
               pDC->LineTo(crInner.left, crInner.top);

               pDC->MoveTo(crClient.left, crInner.bottom-1);
               pDC->LineTo(crInner.left, crInner.bottom-1);

               CRect crFill;
               crFill.left = crClient.left;
               crFill.top = crInner.top+1;
               crFill.right = crInner.left;
               crFill.bottom = crInner.bottom-1;
               pDC->FillRect(crFill, &cbBackground);
            }

            // Draw the line between the gray and non-gray area.
            pDC->SelectObject(&cpBlack);
            pDC->MoveTo(crPanel.left, crInner.top);
            pDC->LineTo(crPanel.left, crInner.bottom);

            // This line is drawn.
            crInner.left++;
         }
         if (crPanel.right < crClient.right-1)
         {
            crInner.right = (crPanel.right < crClient.left)
                              ? crClient.left
                              : crPanel.right;
            if (crPanel.right < crClient.right-2)
            {
               // We need to draw gray on the right.

               pDC->SelectObject(&cpShadow);
               pDC->MoveTo(crInner.right, crInner.top);
               pDC->LineTo(crClient.right-1, crInner.top);

               pDC->MoveTo(crInner.right, crInner.bottom-1);
               pDC->LineTo(crClient.right-1, crInner.bottom-1);

               CRect crFill;
               crFill.left = crInner.right;
               crFill.top = crInner.top+1;
               crFill.right = crClient.right-1;
               crFill.bottom = crInner.bottom-1;
               pDC->FillRect(crFill, &cbBackground);
            }

            // Draw the line between the gray and non-gray area.
            pDC->SelectObject(&cpBlack);
            pDC->MoveTo(crPanel.right-1, crInner.top);
            pDC->LineTo(crPanel.right-1, crInner.bottom);

            // This line is drawn.
            crInner.right--;
         }

         if (crPanel.left <= crClient.right && crPanel.right >= crClient.left)
         {
            // We need to draw in the middle.

            pDC->SelectObject(&cpBlack);
            pDC->MoveTo(crInner.left, crInner.top);
            pDC->LineTo(crInner.right, crInner.top);
            pDC->MoveTo(crInner.left, crInner.bottom-1);
            pDC->LineTo(crInner.right, crInner.bottom-1);

            crInner.top++;
            crInner.bottom--;

            pDC->FillRect(crInner, &cbInner);
         }

         pDC->SelectObject(&cpBlack);

         // Go through and draw all tick marks.

         BOOL fGray = -314;         // Not TRUE or FALSE!

         PCOORD x0 = rc->source_pbox.x0;
         // Align the x coordinate.
         x0 += Unit-1;
         PCOORD lMod = (x0-Panel.x0) % Unit;
         if (lMod < 0)
         {
            lMod += Unit;
         }
         x0 -= lMod;

         int y = (crInner.top + crInner.bottom)/2 + 1;

#ifdef LOCALIZE
		 m_BaseX0 = x0;
		 long CountMarks = 0;
#endif
         while (x0 < rc->source_pbox.x1)
         {
            if (x0 != Panel.x0 && x0 != Panel.x1)
            {
               BOOL fNewGray = (x0 < Panel.x0) || (x0 > Panel.x1);

               if (fNewGray != fGray)
               {
                  fGray = fNewGray;
                  pDC->SelectObject(fGray ? &cpShadow : &cpBlack);
               }

               PCOORD AbsX0 = x0 - Panel.x0;
               if (AbsX0 < 0)
               {
                  AbsX0 = -AbsX0;
               }
               int x = rc->page_x_to_screen(x0) + Adjust.x;
               PCOORD lMod = AbsX0 % Mark;

#ifdef LOCALIZE
               int PrintVal;
			   if (CurrentLocale.UseMetric())
			   {
				   double dPrintVal;
 				   double dMaxDiff = 1.0/(2*factor);
			

				   dPrintVal = (double) AbsX0/((double)PAGE_RESOLUTION/INCHES_TO_CENTIMETRES);
				   double dDiff = dPrintVal - floor(dPrintVal);
				   PrintVal = (int) Round(dPrintVal);
				   if (factor && PrintVal && ((dDiff < dMaxDiff) || (1.0-dDiff < dMaxDiff)))
				   {
					   if (Scale > 1)
					   	    lMod = PrintVal%Scale;
					   else 
						    lMod = 0;
				   }
				   else lMod = 1;			   
			   }
			   
#endif
               if (lMod == 0)
               {
                  char cbBuffer[10];
#ifdef LOCALIZE
				  if (CurrentLocale.UseMetric())
				  {
                      sprintf(cbBuffer, "%d", PrintVal);
				  }
				  else
#endif
	                  sprintf(cbBuffer, "%d", (int)(AbsX0/PAGE_RESOLUTION));
                  int nStrlen = strlen(cbBuffer);

                  // We want to draw a number here.
                  CFont* pOldFont = pDC->SelectObject(&GetXFont());
                  if (pOldFont != NULL)
                  {
                     COLORREF clOldBk = pDC->SetBkColor(GetSysColor(fGray ? COLOR_BTNFACE : COLOR_WINDOW));
                     COLORREF clOldText = pDC->SetTextColor(GetSysColor(fGray ? COLOR_BTNSHADOW : COLOR_BTNTEXT));

                     // Compute the size of the text.
                     CSize czText = pDC->GetTextExtent(cbBuffer, nStrlen);

                     // Draw the text.
                     pDC->ExtTextOut(x-czText.cx/2,
                                   y - czText.cy/2,
                                   0,
                                   NULL,
                                   cbBuffer,
                                   nStrlen,
                                   NULL);

                     pDC->SetTextColor(clOldText);
                     pDC->SetBkColor(clOldBk);

                     pDC->SelectObject(pOldFont);
                  }
               }
               else
               {
                  int nHeight;

                  switch (lMod)
                  {
                     case PAGE_RESOLUTION:
                     case PAGE_RESOLUTION/2:
                     {
                        nHeight = 2;
                        break;
                     }
                     default:
                     {
                        nHeight = 1;
                        break;
                     }
                  }
                  pDC->MoveTo(x, y-nHeight);
                  pDC->LineTo(x, y+nHeight);
               }
            }
#ifdef LOCALIZE
			if (CurrentLocale.UseMetric())
				x0 = m_BaseX0 + ComputeMetricPosition(dUnit, ++CountMarks);
			else
#endif
                x0 += Unit;
         }

         pDC->SelectObject(pOldPen);

         break;
      }
      case Vertical:
      {
         // Draw the 3D borders and the inner common lines.

         CRect crInner = crClient;
         crInner.left += 3;
         crInner.right -= 5;
         crInner.bottom--;       // for bottom border.

         CPen* pOldPen;
         pOldPen = pDC->SelectObject(&cpHighlight);
         // Left 3D border
         pDC->MoveTo(crClient.left, crClient.top);
         pDC->LineTo(crClient.left, crClient.bottom-1);

         // Right containing line
         pDC->MoveTo(crInner.right, crClient.top);
         pDC->LineTo(crInner.right, crClient.bottom-1);

         // Bottom 3D border
         pDC->SelectObject(&cpShadow);
         pDC->MoveTo(crClient.right-2, crClient.top);
         pDC->LineTo(crClient.right-2, crClient.bottom-1);

         // Bump highs.
         pDC->SelectObject(&cpFace);
         pDC->MoveTo(crClient.left+1, crClient.top);
         pDC->LineTo(crClient.left+1, crClient.bottom-1);
         pDC->MoveTo(crClient.left+2, crClient.top);
         pDC->LineTo(crClient.left+2, crClient.bottom-1);
         pDC->MoveTo(crClient.right-3, crClient.top);
         pDC->LineTo(crClient.right-3, crClient.bottom-1);
         pDC->MoveTo(crClient.right-4, crClient.top);
         pDC->LineTo(crClient.right-4, crClient.bottom-1);

         // Draw the inner space.

         if (crPanel.top >= crClient.top)
         {
            // We need to draw gray on the top.
            crInner.top = (crPanel.top > crClient.bottom)
                              ? crClient.bottom
                              : crPanel.top;

            if (crPanel.top > crClient.top)
            {
               pDC->SelectObject(&cpShadow);
               pDC->MoveTo(crInner.left, crClient.top);
               pDC->LineTo(crInner.left, crInner.top);

               pDC->MoveTo(crInner.right-1, crClient.top);
               pDC->LineTo(crInner.right-1, crInner.top);

               CRect crFill;
               crFill.left = crInner.left+1;
               crFill.top = crClient.top;
               crFill.right = crInner.right-1;
               crFill.bottom = crInner.top;
               pDC->FillRect(crFill, &cbBackground);
            }

            // Draw the line between the gray and non-gray area.
            pDC->SelectObject(&cpBlack);
            pDC->MoveTo(crInner.left, crPanel.top);
            pDC->LineTo(crInner.right, crPanel.top);

            // This line is drawn.
            crInner.top++;
         }
         if (crPanel.bottom < crClient.bottom-1)
         {
            crInner.bottom = (crPanel.bottom < crClient.top)
                              ? crClient.top
                              : crPanel.bottom;
            if (crPanel.bottom < crClient.bottom-2)
            {
               // We need to draw gray on the bottom.

               pDC->SelectObject(&cpShadow);
               pDC->MoveTo(crInner.left, crInner.bottom);
               pDC->LineTo(crInner.left, crClient.bottom-1);

               pDC->MoveTo(crInner.right-1, crInner.bottom);
               pDC->LineTo(crInner.right-1, crClient.bottom-1);

               CRect crFill;
               crFill.left = crInner.left+1;
               crFill.top = crInner.bottom;
               crFill.right = crInner.right-1;
               crFill.bottom = crClient.bottom-1;
               pDC->FillRect(crFill, &cbBackground);
            }

            // Draw the line between the gray and non-gray area.
            pDC->SelectObject(&cpBlack);
            pDC->MoveTo(crInner.left, crPanel.bottom-1);
            pDC->LineTo(crInner.right, crPanel.bottom-1);

            // This line is drawn.
            crInner.bottom--;
         }

         if (crPanel.top <= crClient.bottom && crPanel.bottom >= crClient.top)
         {
            // We need to draw in the middle.

            pDC->SelectObject(&cpBlack);
            pDC->MoveTo(crInner.left, crInner.top);
            pDC->LineTo(crInner.left, crInner.bottom);

            pDC->MoveTo(crInner.right-1, crInner.top);
            pDC->LineTo(crInner.right-1, crInner.bottom);

            crInner.left++;
            crInner.right--;

            pDC->FillRect(crInner, &cbInner);
         }

         BOOL fGray = -314;         // Not TRUE or FALSE!

         // Go through and draw all tick marks.

         PCOORD y0 = rc->source_pbox.y0;
         // Align the y coordinate.
         y0 += Unit-1;
         PCOORD lMod = (y0-Panel.y0) % Unit;
         if (lMod < 0)
         {
            lMod += Unit;
         }
         y0 -= lMod;

         int x = (crInner.left + crInner.right)/2 + 1;

#ifdef LOCALIZE
		 m_BaseY0 = y0;
		 long CountMarks = 0;
#endif

         while (y0 < rc->source_pbox.y1)
         {
            if (y0 != Panel.y0 && y0 != Panel.y1)
            {
               BOOL fNewGray = (y0 < Panel.y0 || y0 > Panel.y1);

               if (fNewGray != fGray)
               {
                  fGray = fNewGray;
                  pDC->SelectObject(fGray ? &cpShadow : &cpBlack);
               }

               PCOORD AbsY0 = y0 - Panel.y0;
               if (AbsY0 < 0)
               {
                  AbsY0 = -AbsY0;
               }
               int y = rc->page_y_to_screen(y0) + Adjust.y;
               PCOORD lMod = AbsY0 % Mark;

#ifdef LOCALIZE
               int PrintVal;
			   if (CurrentLocale.UseMetric())
			   {
				   double dPrintVal;
 				   double dMaxDiff = 1.0/(2*factor);
			

				   dPrintVal = (double) AbsY0/((double)PAGE_RESOLUTION/INCHES_TO_CENTIMETRES);
				   double dDiff = dPrintVal - floor(dPrintVal);
				   PrintVal = (int) Round(dPrintVal);
				   if (factor && PrintVal && ((dDiff < dMaxDiff) || (1.0-dDiff < dMaxDiff)))
				   {
					   if (Scale > 1)
					   	    lMod = PrintVal%Scale;
					   else 
						    lMod = 0;
				   }
				   else lMod = 1;			   
			   }
			   
#endif

               if (lMod == 0)
               {
                  char cbBuffer[10];
#ifdef LOCALIZE
				  if (CurrentLocale.UseMetric())
				  {
                      sprintf(cbBuffer, "%d", PrintVal);
				  }
				  else
#endif
	                  sprintf(cbBuffer, "%d", (int)(AbsY0/PAGE_RESOLUTION));
                  int nStrlen = strlen(cbBuffer);

                  // We want to draw a number here.
                  CFont* pOldFont = pDC->SelectObject(&GetYFont());
                  if (pOldFont != NULL)
                  {
                     COLORREF clOldBk = pDC->SetBkColor(GetSysColor(fGray ? COLOR_BTNFACE : COLOR_WINDOW));
                     COLORREF clOldText = pDC->SetTextColor(GetSysColor(fGray ? COLOR_BTNSHADOW : COLOR_BTNTEXT));

                     // Compute the size of the text.
                     CSize czText = pDC->GetTextExtent(cbBuffer, nStrlen);

                     // Draw the text.
                     pDC->ExtTextOut(x - czText.cy/2,
                                   y + czText.cx/2,
                                   0,
                                   NULL,
                                   cbBuffer,
                                   nStrlen,
                                   NULL);

                     pDC->SetTextColor(clOldText);
                     pDC->SetBkColor(clOldBk);

                     pDC->SelectObject(pOldFont);
                  }
               }
               else
               {
                  int nWidth;
                  switch (lMod)
                  {
                     case PAGE_RESOLUTION:
                     case PAGE_RESOLUTION/2:
                     {
                        nWidth = 2;
                        break;
                     }
                     default:
                     {
                        nWidth = 1;
                        break;
                     }
                  }
                  pDC->MoveTo(x-nWidth, y);
                  pDC->LineTo(x+nWidth, y);
               }
            }
#ifdef LOCALIZE
			if (CurrentLocale.UseMetric())
				y0 = m_BaseY0 + ComputeMetricPosition(dUnit, ++CountMarks);
			else
#endif
				y0 += Unit;
         }

         pDC->SelectObject(pOldPen);

         break;
      }
      case Corner:
      {
         // Draw the 3D borders.
         CPen* pOldPen;
         pOldPen = pDC->SelectObject(&cpHighlight);
#if 1
         pDC->MoveTo(crClient.left, crClient.top);
         pDC->LineTo(crClient.right-1, crClient.top);
         pDC->MoveTo(crClient.left, crClient.top);
         pDC->LineTo(crClient.left, crClient.bottom-1);

         pDC->SelectObject(&cpShadow);
         pDC->MoveTo(crClient.left, crClient.bottom-2);
         pDC->LineTo(crClient.right-1, crClient.bottom-2);
         pDC->MoveTo(crClient.right-2, crClient.top);
         pDC->LineTo(crClient.right-2, crClient.bottom-1);

         CRect crFill = crClient;
         crFill.left = crClient.left+1;
         crFill.top = crClient.top+1;
         crFill.right = crClient.right-2;
         crFill.bottom = crClient.bottom-2;
         pDC->FillRect(crFill, &cbBackground);
#else
         // Connected corners.
         // Looks cool, but doesn't really work visually.
         pDC->FillRect(crClient, &cbBackground);

         pDC->SelectObject(cpHighlight);
         pDC->MoveTo(crClient.left, crClient.bottom);
         pDC->LineTo(crClient.left, crClient.top);
         pDC->LineTo(crClient.right, crClient.top);

         pDC->MoveTo(crClient.right-5, crClient.bottom);
         pDC->LineTo(crClient.right-5, crClient.bottom-5);
         pDC->LineTo(crClient.right, crClient.bottom-5);

         pDC->SelectObject(cpShadow);
         pDC->MoveTo(crClient.left+3, crClient.bottom);
         pDC->LineTo(crClient.left+3, crClient.top+3);
         pDC->LineTo(crClient.right, crClient.top+3);

         pDC->MoveTo(crClient.right-6, crClient.bottom);
         pDC->LineTo(crClient.right-6, crClient.bottom-6);
         pDC->LineTo(crClient.right, crClient.bottom-6);

         pDC->MoveTo(crClient.right-2, crClient.bottom);
         pDC->LineTo(crClient.right-2, crClient.bottom-2);
         pDC->LineTo(crClient.right, crClient.bottom-2);

         pDC->SelectObject(cpBlack);
         pDC->MoveTo(crClient.right-1, crClient.bottom);
         pDC->LineTo(crClient.right-1, crClient.bottom-1);
         pDC->LineTo(crClient.right, crClient.bottom-1);
#endif

         pDC->SelectObject(pOldPen);

         break;
      }
      default:
      {
         // Bad type!
         ASSERT(FALSE);
         break;
      }
   }

   // Restore any lines currently shown.
   ToggleLines(pDC, m_cwaLines);

   if (pDC != &PaintDC)
   {
      PaintDC.BitBlt(crClient.left,
                     crClient.top,
                     crClient.Width(),
                     crClient.Height(),
                     pDC,
                     0,
                     0,
                     SRCCOPY);

      ASSERT(pDC == &dc);
      dc.SelectObject(pOldBitmap);
      Bitmap.DeleteObject();
      dc.DeleteDC();
   }
	// Remember what unit we last drew with.
	m_LastUnit = Unit;
#ifdef LOCALIZE
	m_dLastUnit = dUnit;
#endif
}

LRESULT CRuler::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
   UpdateCursorShadow();
   return 0;
}

void CRuler::UpdateCursorShadow(void)
{
   if (m_nType != Corner)
   {
      CPmwDoc* pDoc = m_pView->GetDocument();
      RedisplayContext* rc = m_pView->get_rc();

      CRulerLineArray cwaCurrent;
      cwaCurrent.Copy(m_cwaLines);
      m_cwaLines.RemoveAll();

      // Compute the origin translation.
      // This assumes that only a translation is going on (no funny Window or
      // Viewport extents for the windows).

		if (AfxGetMainWnd()->IsWindowEnabled())
		{
			POINT OriginOffset;
			OriginOffset.x = 0;
			OriginOffset.y = 0;
			m_pView->ClientToScreen(&OriginOffset);
			ScreenToClient(&OriginOffset);

			BOOL fUseCursor = TRUE;

			if (pDoc->selected_objects() != NULL)
			{
				// We have some objects selected.
				FLAGS RefreshFlags = 0;

				PMGPageObject* pObject;
				for (pObject = pDoc->selected_objects();
					  pObject != NULL;
					  pObject = pObject->next_selected_object())
				{
					RefreshFlags = pObject->get_refresh_flags();
					if (RefreshFlags & (REFRESH_FLAG_moving|REFRESH_FLAG_sizing))
					{
						break;
					}
				}

				if (RefreshFlags & (REFRESH_FLAG_moving|REFRESH_FLAG_sizing))
				{
					PBOX pbox;
					if (RefreshFlags & REFRESH_FLAG_moving)
					{
						// Moving selected objects.
						pDoc->GetSelectBound(&pbox);
					}
					else
					{
						ASSERT(pObject != NULL);
						pbox = pObject->get_bound();
					}

					// Add a line for each end of the box.
					CRect crBox;
					rc->pbox_to_screen(&pbox, TRUE);
					if (rc->convert_pbox(&pbox, &crBox, NULL))
					{
						crBox.OffsetRect(OriginOffset.x, OriginOffset.y);
						if (m_nType == Horizontal)
						{
							m_cwaLines.Add((WORD)crBox.left);
							m_cwaLines.Add((WORD)crBox.right);
						}
						else
						{
							m_cwaLines.Add((WORD)crBox.top);
							m_cwaLines.Add((WORD)crBox.bottom);
						}
					}

					fUseCursor = FALSE;
				}
			}

			// If we want to shadow the cursor, do that now.
			if (fUseCursor)
			{
				// We want to follow the cursor.
				POINT p;
				GetCursorPos(&p);

				CRect crClient;
				m_pView->GetClientRect(crClient);
				m_pView->ScreenToClient(&p);

				if (crClient.PtInRect(p))
				{
					PPNT Cursor;
					// Get the cursor position in page coordinates (with snapping, etc.)
					m_pView->GetCursorPagePos(&Cursor);

#ifdef LOCALIZE
					if (!CurrentLocale.UseMetric())
#endif
						rc->ppnt_to_screen(Cursor, &p);

					// Adjust by the origin.
					p.x += OriginOffset.x;
					p.y += OriginOffset.y;

					// Add this line.
					m_cwaLines.Add((WORD)(m_nType == Horizontal ? p.x : p.y));
				}
			}
		}

      // Now we have the old and new lines.
      // Construct the change array.
      // This seems grossly slow, but there should never be more than
      // a few (<4) lines at a time

      CRulerLineArray cwaToggle;
      for (int i = 0; i < m_cwaLines.GetSize(); i++)
      {
         WORD w = m_cwaLines.GetAt(i);
         BOOL fFound = FALSE;

         // See if the line is in the current array.
         for (int j = 0; j < cwaCurrent.GetSize(); j++)
         {
            if (cwaCurrent.GetAt(j) == w)
            {
               fFound = TRUE;
               cwaCurrent.RemoveAt(j);
               break;
            }
         }
         if (!fFound)
         {
            cwaToggle.Add(w);
         }
      }
      // Now add any lines left in the Current array.
      cwaToggle.Append(cwaCurrent);

      if (cwaToggle.GetSize() > 0)
      {
         // Get a client dc to use.
         CClientDC dc(this);

         // Toggle these lines.
         ToggleLines(&dc, cwaToggle);
      }
   }
}

/*
// Toggle the lines in the passed array.
*/

void CRuler::ToggleLines(CDC* pDC, CRulerLineArray& cwaToggle)
{
   CRect crClient;
   GetClientRect(crClient);

   // We want to toggle!
   int nOldROP2 = pDC->SetROP2(R2_NOT);

   for (int i = 0; i < cwaToggle.GetSize(); i++)
   {
      // Get the line to toggle.
		int w = (int)cwaToggle.GetAt(i);

      if (m_nType == Horizontal)
      {
         // Horizontal.
         pDC->MoveTo(w, crClient.top + 4);
         pDC->LineTo(w, crClient.bottom - 6);
      }
      else
      {
         // Vertical.
         pDC->MoveTo(crClient.left + 4, w);
         pDC->LineTo(crClient.right - 6, w);
      }
   }

   pDC->SetROP2(nOldROP2);
}

void CRuler::OnRButtonDown(UINT nFlags, CPoint point)
{
	Util::SendWmCommand(m_pView, GetDlgCtrlID(), this, 0);
}

void CRuler::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Tell view to create guideline
   int type = (m_nType == Horizontal) ? CPmwView::GUIDE_VERT : CPmwView::GUIDE_HORIZ;
   m_pView->CreateGuide(type);
}

/////////////////////////////////////////////////////////////////////////////
// CRulerLineArray

#ifndef WIN32
int CRulerLineArray::Append(const CRulerLineArray& src)
{
   ASSERT_VALID(this);
   ASSERT(this != &src);   // cannot append to itself

   int nOldSize = m_nSize;
   SetSize(m_nSize + src.m_nSize);

   memcpy(m_pData + nOldSize, src.m_pData, src.m_nSize * sizeof(WORD));

   return nOldSize;
}

void CRulerLineArray::Copy(const CRulerLineArray& src)
{
   ASSERT_VALID(this);
   ASSERT(this != &src);   // cannot append to itself

   SetSize(src.m_nSize);

   memcpy(m_pData, src.m_pData, src.m_nSize * sizeof(WORD));
}
#endif
