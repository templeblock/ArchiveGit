/*
// $Header: /PM8/App/POSLIST.CPP 1     3/03/99 6:10p Gbeddow $
//
// Position dialog.
//
// $Log: /PM8/App/POSLIST.CPP $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 2     6/23/98 2:38p Rlovejoy
// Put parentheses around positioning macro paramter.
// 
//    Rev 1.0   14 Aug 1997 15:24:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:00   Fred
// Initial revision.
// 
//    Rev 1.3   27 May 1997 11:08:28   Fred
// Uses PI definitions in utils.h
// 
//    Rev 1.2   23 Jan 1997 14:52:58   johno
// Changes, fixes per Fred
// (not finished)
// 
//    Rev 1.1   23 Dec 1996 11:42:06   johno
// 
//    Rev 1.0   19 Dec 1996 14:43:44   johno
// Initial revision.
// 
*/

#include "stdafx.h"

#include "poslist.h"
#include "utils.h"

int
TPositionListElement::Quadrant (double AAngle)
{
   int   Result;
   
   while (AAngle < 0.0)
   {
      AAngle += PI2;
   }
   
   while (AAngle > PI2)
   {
      AAngle -= PI2;
   }

   if (AAngle < 1.0 * (PI / 2.0))
      Result = 0;
   else
   if (AAngle < 2.0 * (PI / 2.0))
      Result = 1;
   else 
   if (AAngle < 3.0 * (PI / 2.0))
      Result = 2;
   else
      Result = 3;

   return Result;
}

TRealPoint
TPositionListElement::GetExtent (const TRealBounds ABounds, double AAngle, int AAdjust)
{
   int         Q;
   TRealPoint  P, Result;
   double      W;
   double      H;

   Q = (Quadrant (AAngle) + AAdjust) & 0x0003;
   W = ABounds.Width / 2.0;
   H = ABounds.Height / 2.0;
   
   P.X = -W;
   P.Y = -H;
   
   if ((Q == 1) || (Q == 2))
      P.X = P.X + ABounds.Width;

   if ((Q == 2) || (Q == 3))
      P.Y = P.Y + ABounds.Height;

   Result.X = ABounds.Left + W + (P.X * cos (AAngle) + P.Y * sin(AAngle));
   Result.Y = ABounds.Top  + H + (P.Y * cos (AAngle) - P.X * sin(AAngle));

   return Result;
}

#define  XCompute(d) (RoundD2I (((d) / XScale) * (APreviewBounds->right - APreviewBounds->left)))
#define  YCompute(d) (RoundD2I (((d) / YScale) * (APreviewBounds->bottom - APreviewBounds->top)))

void
TPositionListElement::DisplayLine (CRect *APreviewBounds, double XScale, double YScale, CDC *TCanvas)
{
   if (IsFlipped () == FALSE)
   {
      TCanvas->MoveTo (XCompute (Bounds.Left), YCompute (Bounds.Top));
      TCanvas->LineTo (XCompute (Bounds.Left + Bounds.Width), YCompute (Bounds.Top + Bounds.Height));
   }
   else
   {
      TCanvas->MoveTo (XCompute (Bounds.Left + Bounds.Width), YCompute (Bounds.Top));
      TCanvas->LineTo (XCompute (Bounds.Left), YCompute (Bounds.Top + Bounds.Height));
   }
}

void
TPositionListElement::DisplayRectangle (CRect *APreviewBounds, double XScale, double YScale, CDC *TCanvas)
{
   double      OriginX, OriginY;
   POINT       Points [4];
   TRealPoint  P;

HBRUSH   hBrush, hOldBrush;
hBrush = (HBRUSH)GetStockObject (HOLLOW_BRUSH);
hOldBrush = (HBRUSH)SelectObject (TCanvas->m_hDC, hBrush);

   OriginX = Bounds.Left + (Bounds.Width  / 2.0);
   OriginY = Bounds.Top  + (Bounds.Height / 2.0);

   P = Rotate (Bounds.Left, Bounds.Top, OriginX, OriginY);
   Points [0].x = XCompute (P.X);
   Points [0].y = YCompute (P.Y);

   P = Rotate (Bounds.Left + Bounds.Width, Bounds.Top, OriginX, OriginY);
   Points [1].x = XCompute (P.X);
   Points [1].y = YCompute (P.Y);

   P = Rotate (Bounds.Left + Bounds.Width, Bounds.Top + Bounds.Height, OriginX, OriginY);
   Points [2].x = XCompute (P.X);
   Points [2].y = YCompute (P.Y);

   P = Rotate (Bounds.Left, Bounds.Top + Bounds.Height, OriginX, OriginY);
   Points [3].x = XCompute (P.X);
   Points [3].y = YCompute (P.Y);
   
   TCanvas->Polygon (Points, 4);

SelectObject (TCanvas->m_hDC, hOldBrush);
}
///////////////////////////////////////////
TRealBounds
TPositionList::GetPageBounds (void)
{
   TRealBounds Result;

	Result.Left = 0.0;
	Result.Top = 0.0;
	Result.Width = m_NormalizedPageWidth;
	Result.Height = m_NormalizedPageHeight;

   return Result;
}

TRealBounds
TPositionList::GetListBounds (void)
{
   int                  i;
   double               Right = 0.0, Bottom = 0.0;
   TRealBounds          Result;
   TPositionListElement *p;

   for (i = 0; i < GetSize (); i ++)
   {
      p = (TPositionListElement*)GetAt (i);

      if ((i == 0) || (p->GetLeft () < Result.Left))
         Result.Left =  p->GetLeft ();
      
      if ((i == 0) || (p->GetTop () < Result.Top))
         Result.Top =  p->GetTop ();

      if ((i == 0) || (p->GetRight () > Right))
         Right =  p->GetRight ();

      if ((i == 0) || (p->GetBottom () > Bottom))
         Bottom =  p->GetBottom ();
   }

   Result.Width  = Right - Result.Left;
   Result.Height = Bottom - Result.Top;
   
   return Result;
}

void
TPositionList::SpaceWidthsEvenly (const TRealBounds ABounds)
{
   int                  i, Count;
   double               Width, Gap, Current;   
   TPositionListElement *Element;

   ClearAllMarks ();
   if ((Count = GetSize ()) > 1)
   {
      Width = GetSumOfWidthExtents ();
      Gap = (ABounds.Width - Width) / (double)(Count - 1);
      Current = ABounds.Left;
      Element = GetUnmarkedElementWithMinimumLeft ();
      while (Element != NULL)
      {
         Element->AlignToLeft (Current);
			Current = Current + (Element->GetRight () - Element->GetLeft ()) + Gap;
			Element = GetUnmarkedElementWithMinimumLeft ();
      }
   }
   else
   {
      for (i = 0; i < Count; i ++)
      {
         Element = (TPositionListElement*)GetAt (i);
         Element->AlignToCenter (ABounds.Left + (ABounds.Width / 2.0));
      }
   }
}

void
TPositionList::SpaceHeightsEvenly (const TRealBounds ABounds)
{
   int                  i, Count;
   double               Height, Gap, Current;   
   TPositionListElement *Element;

   ClearAllMarks ();
   if ((Count = GetSize ()) > 1)
   {
      Height = GetSumOfHeightExtents ();
      Gap = (ABounds.Height - Height) / (double)(Count - 1);
      Current = ABounds.Top;
      Element = GetUnmarkedElementWithMinimumTop ();
      while (Element != NULL)
      {
		Element->AlignToTop (Current);
		Current = Current + (Element->GetBottom () - Element->GetTop ()) + Gap;
		Element = GetUnmarkedElementWithMinimumTop ();
      }
   }
   else
   {
      for (i = 0; i < Count; i ++)
      {
         Element = (TPositionListElement*)GetAt (i);
         Element->AlignToMiddle (ABounds.Top + (ABounds.Height / 2.0));
      }
   }
}

TPositionListElement *
TPositionList::GetUnmarkedElementWithMinimumLeft ()
{
   int                  i;
   TPositionListElement *p1 = NULL, *p2;

   for (i = 0; i < GetSize (); i ++)
   {
      p2 = (TPositionListElement*)GetAt (i);
      
      if (p2->Mark == FALSE)
      {
         if (p1 == NULL)
            p1 = p2;
         else
         if (p2->GetLeft () < p1->GetLeft ())
            p1 = p2;
      }
   }

   if (p1 != NULL)
         p1->Mark = TRUE;

   return p1;
}

TPositionListElement *
TPositionList::GetUnmarkedElementWithMinimumTop ()
{
   int                  i;
   TPositionListElement *p1 = NULL, *p2;

   for (i = 0; i < GetSize (); i ++)
   {
      p2 = (TPositionListElement*)GetAt (i);
      
      if (p2->Mark == FALSE)
      {
         if (p1 == NULL)
            p1 = p2;
         else
         if (p2->GetTop () < p1->GetTop ())
            p1 = p2;
      }
   }

   if (p1 != NULL)
         p1->Mark = TRUE;

   return p1;
}

void
TPositionList::Assign (TPositionList *AList)
{
   int                  i;
   TPositionListElement *p, *pNew;

   Kill ();

	m_NormalizedPageWidth  = AList->NormalizedPageWidth ();
	m_NormalizedPageHeight = AList->NormalizedPageHeight ();
   m_PageRect = AList->GetPageRect ();

   for (i = 0; i < AList->GetSize (); i ++)
   {
      p = (TPositionListElement*)(AList->GetAt (i));
      pNew = new TPositionListElement (*p);
      Add (pNew);
   }
}
