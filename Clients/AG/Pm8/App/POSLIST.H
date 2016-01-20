#include <math.h>

#define  ELEMENT_XFLIPPED        0x0001
#define  ELEMENT_YFLIPPED        0x0002
#define  ELEMENT_RELATIVE_SIZE   0x0004
#define  ELEMENT_REPEATED_X      0x0008
#define  ELEMENT_REPEATED_Y      0x0010
#define  ELEMENT_CENTERED_X      0x0020
#define  ELEMENT_CENTERED_Y      0x0040
#define  ELEMENT_ALIGN_LEFT      0x0080
#define  ELEMENT_ALIGN_RIGHT     0x0100
#define  ELEMENT_ALIGN_TOP       0x0200
#define  ELEMENT_ALIGN_BOTTOM    0x0400
#define  ELEMENT_FLOW_LEFT       0x0800
#define  ELEMENT_FLOW_RIGHT      0x1000
#define  ELEMENT_HIDDEN          0x8000

#define  OBJECT_FLAG_LINE        0x0001
#define  OBJECT_FLAG_XFLIPPED    0x0002
#define  OBJECT_FLAG_YFLIPPED    0x0004

#define  RoundD2I(d)    (int)(d + 0.5)

struct TRealPoint
{
   TRealPoint ()
   {
      X = Y = 0.0;
   }
   
   double   X;
   double   Y;
};

struct TRealBounds
{
   TRealBounds ()
   {
      Left = Top = Width = Height = 0.0;
   }
   
   double   Left;
   double   Top;
   double   Width;
   double   Height;
};

class TPositionListElement : public  CObject
{
public:
   TPositionListElement (const TRealBounds &ABounds, double AAngle, long AFlags)
   {
      Bounds = ABounds;
      Angle  = AAngle;
      Flags  = AFlags;
      
      if (IsLine ())
         Angle = 0.0;

      Mark = FALSE;
   }

   TPositionListElement (TPositionListElement &AElement)
   {
      Bounds = AElement.Bounds;
      Angle  = AElement.Angle;
      Flags  = AElement.Flags;
      
      Mark = FALSE;
   }
////////
   void
   Display (CRect *APreviewBounds, double XScale, double YScale, CDC *TCanvas)
   {
      CPen  *pOldPen, Pen (PS_SOLID, 2, RGB (0, 0, 0));

      if ((pOldPen = TCanvas->SelectObject (&Pen)) != NULL)
      {
         TRY
         {
            if (IsLine () != FALSE)
               DisplayLine (APreviewBounds, XScale, YScale, TCanvas);
            else
               DisplayRectangle (APreviewBounds, XScale, YScale, TCanvas);
         }
         END_TRY
      
         TCanvas->SelectObject (pOldPen);
      }
   }
////////
   double
   GetLeft (void)
   {
      return (GetExtent (Bounds, Angle, 0)).X;
   }

   double
   GetTop (void)
   {
      return (GetExtent (Bounds, Angle, 1)).Y;
   }

   double
   GetRight (void)
   {
      return (GetExtent (Bounds, Angle, 2)).X;
   }

   double
   GetBottom (void)
   {
      return (GetExtent (Bounds, Angle, 3)).Y;
   }

   double
   GetAngle (void)
   {
      return Angle;
   }
////////
   void
   SetLeft (double ALeft)
   {
      Bounds.Left = ALeft;
   }

   void
   SetTop (double ATop)
   {
      Bounds.Top = ATop;
   }

   void
   SetWidth (double AWidth)
   {
      Bounds.Width = AWidth;
   }

   void
   SetHeight (double AHeight)
   {
      Bounds.Height = AHeight;
   }
////////
   void
   AlignToLeft (double ALeft)
   {
      Bounds.Left = Bounds.Left + (ALeft - GetLeft ());
   }

   void
   AlignToCenter (double ACenter)
   {
      Bounds.Left = Bounds.Left + (ACenter - ((GetLeft () + GetRight ()) / 2.0));
   }

   void
   AlignToRight (double ARight)
   {
      Bounds.Left = Bounds.Left + (ARight - GetRight ());
   }

   void
   AlignToTop (double ATop)
   {
      Bounds.Top = Bounds.Top + (ATop - GetTop ());
   }

   void
   AlignToMiddle (double AMiddle)
   {
      Bounds.Top = Bounds.Top + (AMiddle - ((GetTop () + GetBottom ()) / 2.0));
   }

   void
   AlignToBottom (double ABottom)
   {
      Bounds.Top = Bounds.Top + (ABottom - GetBottom ());
   }
////////
   void
   SetBounds (TRealBounds ABounds)
   {
      SetLeft   (ABounds.Left);
      SetTop    (ABounds.Top);
      SetWidth  (ABounds.Width);
      SetHeight (ABounds.Height);  
   }
////////
   TRealBounds Bounds; 
   BOOL        Mark;
private:   
   long        Flags;
   double      Angle;
   
   BOOL
   IsLine (void)
   {
      return ((Flags & OBJECT_FLAG_LINE) != 0);
   }
   
   BOOL
   IsFlipped (void)
   {
      return ((Flags & OBJECT_FLAG_XFLIPPED) != 0) ^ ((Flags & OBJECT_FLAG_YFLIPPED) != 0);
   }

   int
   Quadrant (double AAngle);

   TRealPoint
   GetExtent (const TRealBounds ABounds, double AAngle, int AAdjust);

   TRealPoint
   Rotate (double X, double Y, double OX, double OY)
   {
      TRealPoint  Result;

      Result.X = (OX + (X - OX) * cos (Angle) + (Y - OY) * sin (Angle));
      Result.Y = (OY + (Y - OY) * cos (Angle) - (X - OX) * sin (Angle));

      return Result;
   }

   void
   DisplayLine (CRect *APreviewBounds, double XScale, double YScale, CDC *TCanvas);

   void
   DisplayRectangle (CRect *APreviewBounds, double XScale, double YScale, CDC *TCanvas);
};


class TPositionList : public CObArray 
{
public:
   TPositionList (double ANormalizedPageWidth, double ANormalizedPageHeight, TRealBounds PageRect)
   {
	   m_NormalizedPageWidth  = ANormalizedPageWidth;
	   m_NormalizedPageHeight = ANormalizedPageHeight;
      m_PageRect = PageRect;
   }

   TPositionList (void)
   {
      TRealBounds rb;

      TPositionList (0.0, 0.0, rb);
   }

   ~TPositionList ()
   {
      Kill ();
   }

   void
   Assign (TPositionList *AList);

   void
   Display (CRect *APreviewBounds, CDC *TCanvas)
   {
      int                  i;
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->Display (APreviewBounds, m_NormalizedPageWidth, m_NormalizedPageHeight, TCanvas);
      }
   }

   TRealBounds
   GetListBounds (void);

   TRealBounds
   GetPageBounds (void);
////
   TRealBounds
   GetPageRect (void)
   {
      return m_PageRect;;
   }

   double
   GetPageRectLeft (void)
   {
      return m_PageRect.Left;
   }

   double
   GetPageRectTop (void)
   {
      return m_PageRect.Top;
   }
   
   double
   GetPageRectWidth (void)
   {
      return m_PageRect.Width;
   }

   double
   GetPageRectHeight ()
   {
      return m_PageRect.Height;
   }

   double
   GetPageRectRight (void)
   {
      return m_PageRect.Left + m_PageRect.Width;
   }
////
   void
   AlignToLeftOfPage (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetPageBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToLeft (Bounds.Left);
      }
   }

   void
   AlignToCenterOfPage (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetPageBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToCenter (Bounds.Left + (Bounds.Width / 2.0));
      }
   }

   void
   AlignToRightOfPage (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetPageBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToRight (Bounds.Left + Bounds.Width);
      }
   }

   void
   AlignToTopOfPage (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetPageBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToTop (Bounds.Top);
      }
   }

   void
   AlignToMiddleOfPage (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetPageBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToMiddle (Bounds.Top + Bounds.Height / 2.0);
      }
   }

   void
   AlignToBottomOfPage (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetPageBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToBottom (Bounds.Top + Bounds.Height);
      }
   }
////////
   void
   AlignToLeftOfBounds (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetListBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToLeft (Bounds.Left);
      }
   }

   void
   AlignToCenterOfBounds (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetListBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToCenter (Bounds.Left + (Bounds.Width / 2.0));
      }
   }

   void
   AlignToRightOfBounds (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetListBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToRight (Bounds.Left + Bounds.Width);
      }
   }

   void
   AlignToTopOfBounds (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetListBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToTop (Bounds.Top);
      }
   }

   void
   AlignToMiddleOfBounds (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetListBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToMiddle (Bounds.Top + Bounds.Height / 2.0);
      }
   }

   void
   AlignToBottomOfBounds (void)
   {
      int                  i;
      TPositionListElement *p;
      TRealBounds          Bounds;

      Bounds = GetListBounds ();
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToBottom (Bounds.Top + Bounds.Height);
      }
   }
////////
   void
   AlignToLeft (double ALeft)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToLeft (ALeft);
      }
   }

   void
   AlignToCenter (double ACenter)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToCenter (ACenter);
      }
   }

   void
   AlignToRight (double ARight)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToRight (ARight);
      }
   }

   void
   AlignToTop (double ATop)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToTop (ATop);
      }
   }

   void
   AlignToMiddle (double AMiddle)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToMiddle (AMiddle);
      }
   }

   void
   AlignToBottom (double ABottom)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->AlignToBottom (ABottom);
      }
   }
////////
   void
   SetBounds (TRealBounds ABounds)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->SetBounds (ABounds);
      }
   }

   void
   SetLeft (double ALeft)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->SetLeft (ALeft);
      }
   }

   void
   SetTop (double ATop)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->SetTop (ATop);
      }
   }

   void
   SetWidth (double AWidth)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->SetWidth (AWidth);
      }
   }

   void
   SetHeight (double AHeight)
   {
      int                  i;
      TPositionListElement *p;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->SetHeight (AHeight);
      }
   }
////////
   void
   ScaleWidth (double AScale, TPositionList *pAReferenceList)
   {
      int                  i;
      TPositionListElement *p1, *p2;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p1 = (TPositionListElement*)GetAt (i);
         p2 = (TPositionListElement*)pAReferenceList->GetAt (i);
         p1->SetWidth (p2->Bounds.Width * AScale);
      }
   }

   void
   ScaleHeight (double AScale, TPositionList *pAReferenceList)
   {
      int                  i;
      TPositionListElement *p1, *p2;
      
      for (i = 0; i < GetSize (); i ++)
      {
         p1 = (TPositionListElement*)GetAt (i);
         p2 = (TPositionListElement*)pAReferenceList->GetAt (i);
         p1->SetHeight (p2->Bounds.Height * AScale);
      }
   }
////////
   void
   SetWidthToSmallest (void)
   {
      int                  i;
      double               Width = GetSmallestWidth ();
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->SetWidth (Width);
      }
   }

   void
   SetWidthToLargest (void)
   {
      int                  i;
      double               Width = GetLargestWidth ();
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->SetWidth (Width);
      }
   }

   void
   SetHeightToSmallest (void)
   {
      int                  i;
      double               Height = GetSmallestHeight ();
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->SetHeight (Height);
      }
   }

   void
   SetHeightToLargest (void)
   {
      int                  i;
      double               Height = GetLargestHeight ();
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->SetHeight (Height);
      }
   }
////////
   void
   SpaceWidthsEvenlyInBounds (void)
   {
      SpaceWidthsEvenly (GetListBounds ());
   }

   void
   SpaceWidthsEvenlyInPage (void)
   {
      SpaceWidthsEvenly (GetPageBounds ());
   }

   void
   SpaceHeightsEvenlyInBounds (void)
   {
      SpaceHeightsEvenly (GetListBounds ());
   }

   void
   SpaceHeightsEvenlyInPage (void)
   {
      SpaceHeightsEvenly (GetPageBounds ());
   }
////////
   double
   GetSmallestWidth (void)
   {
      int                  i;
      double               Result = 0.0, Width;
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         Width = p->Bounds.Width;
         if ((i == 0) || (Width < Result))
            Result = Width;
      }

      return Result;
   }

   double
   GetLargestWidth (void)
   {
      int                  i;
      double               Result = 0.0, Width;
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         Width = p->Bounds.Width;
         if ((i == 0) || (Width > Result))
            Result = Width;
      }

      return Result;
   }

   double
   GetSmallestHeight (void)
   {
      int                  i;
      double               Result = 0.0, Height;
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         Height = p->Bounds.Height;
         if ((i == 0) || (Height < Result))
            Result = Height;
      }

      return Result;
   }

   double
   GetLargestHeight (void)
   {
      int                  i;
      double               Result = 0.0, Height;
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         Height = p->Bounds.Height;
         if ((i == 0) || (Height > Result))
            Result = Height;
      }

      return Result;
   }
////////
   double
   GetSumOfWidthExtents (void)
   {
      int                  i;
      double               Result = 0.0;
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         Result += p->GetRight () - p->GetLeft ();
      }

      return Result;
   }

   double
   GetSumOfHeightExtents (void)
   {
      int                  i;
      double               Result = 0.0;
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         Result += p->GetBottom () - p->GetTop ();
      }

      return Result;
   }
////////
   void
   ClearAllMarks ()
   {
      int                  i;
      TPositionListElement *p;

      for (i = 0; i < GetSize (); i ++)
      {
         p = (TPositionListElement*)GetAt (i);
         p->Mark = FALSE;
      }
   }
////////
   double
   NormalizedPageWidth (void)
   {
      return m_NormalizedPageWidth;
   }

   double
   NormalizedPageHeight (void)
   {
      return m_NormalizedPageHeight;
   }
////////
   double
   XRelativeToDots (double X)
   {
      return (X / NormalizedPageWidth () * GetPageRectWidth ()) + GetPageRectLeft ();
   }

   double
   XRelativeToInches (double X)
   {
      return XRelativeToDots (X) / 1800.0;
   }

   double
   YRelativeToDots (double Y)
   {
      return (Y / NormalizedPageHeight () * GetPageRectHeight ()) + GetPageRectTop ();
   }

   double
   YRelativeToInches (double Y)
   {
      return YRelativeToDots (Y) / 1800.0;
   }
////////
private:
   double      m_NormalizedPageWidth, m_NormalizedPageHeight;
   TRealBounds m_PageRect;

   void
   Kill (void)
   {
      TPositionListElement *p;
      while (GetSize () > 0)
      {
         p = (TPositionListElement*)GetAt (0);
         delete p;
         RemoveAt (0);
      }
   }
   
   TPositionListElement *
   GetUnmarkedElementWithMinimumLeft ();

   TPositionListElement *
   GetUnmarkedElementWithMinimumTop ();

   void
   SpaceWidthsEvenly (const TRealBounds ABounds);

   void
   SpaceHeightsEvenly (const TRealBounds ABounds);
};
