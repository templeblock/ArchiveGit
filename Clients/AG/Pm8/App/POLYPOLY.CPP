/*
// $Header: /PM8/App/POLYPOLY.CPP 1     3/03/99 6:10p Gbeddow $
//
// $Workfile: POLYPOLY.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:10p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/POLYPOLY.CPP $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 11    6/24/98 3:08p Rlovejoy
// Deselect brush before fill to avoid leak.
// 
// 9     6/08/98 4:09p Rlovejoy
// Fixed bug which fills with "undefined" color.
// 
// 8     5/14/98 10:53a Rlovejoy
// Added overloaded GetBoundingBox() functions.
// 
// 7     5/12/98 11:35a Rlovejoy
// Added 'GetBoundingCircle()'
// 
// 6     5/08/98 12:03p Rlovejoy
// Added GetBoundingBox()
// 
// 5     5/07/98 10:37a Rlovejoy
// Fixed that last fix.
// 
// 4     5/06/98 5:23p Rlovejoy
// Finished fix to thick line fill.
// 
// 3     4/30/98 4:48p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 2     4/29/98 6:32p Rlovejoy
// Updated for new fill format.
// 
//    Rev 1.0   14 Aug 1997 15:24:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:58   Fred
// Initial revision.
// 
//    Rev 1.9   18 Jul 1997 13:18:56   Jay
// Open polygons don't work with PolyPolygon.
// 
//    Rev 1.8   25 Jun 1997 17:02:36   Jay
// Fixed PolyPolyline for 16-bit
// 
//    Rev 1.7   12 Mar 1997 14:53:58   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.6   11 Feb 1997 14:54:44   Jay
// Free draw tool
// 
//    Rev 1.5   29 Jan 1997 12:49:38   Jay
// Uses MAKE_PALETTERGB for outlines less than 2 pixels wide.
// 
//    Rev 1.4   17 Jan 1997 17:03:14   Jay
// New select for drawing objects.
// 
//    Rev 1.3   13 Jan 1997 15:25:36   Jay
// 16-bit now compiles.
// 
//    Rev 1.2   09 Jan 1997 16:54:24   Fred
//  
// 
//    Rev 1.1   06 Jan 1997 10:16:34   Fred
// Start of new text object
// 
//    Rev 1.0   14 Mar 1996 13:44:22   Jay
// Initial revision.
// 
//    Rev 1.1   08 Mar 1996 13:56:20   VSI
// Disabled stuff for MAC build -- not working for MAC
// 
//    Rev 1.0   16 Feb 1996 12:06:58   FRED
// Initial revision.
// 
//    Rev 1.11   12 Oct 1995 14:58:18   JAY
// Now breaks a fill/stroke polypoly into two polypolys.
// 
//    Rev 1.10   23 Jun 1995 11:39:16   JAY
// Got rid of some more overflows.
// 
//    Rev 1.9   25 May 1995 13:05:08   FRED
// Some simple optimizations.
// 
// 
//    Rev 1.8   22 May 1995 17:29:28   JAY
// More inflate goodies.
// 
//    Rev 1.7   10 May 1995 17:41:46   JAY
// Now handles a trivial bezier.
// 
//    Rev 1.6   09 May 1995 13:14:48   FRED
// Bezier drawing code uses 1/2 pixel instead of pixel termination.
// 
// 
//    Rev 1.5   09 May 1995 09:48:18   JAY
// Added UsePalette flag.
// 
//    Rev 1.4   09 May 1995 08:32:38   FRED
// Added Bezier support.
// 
// 
//    Rev 1.3   04 May 1995 12:29:32   FRED
// 
//    Rev 1.2   04 May 1995 10:30:28   JAY
// Some changes to improve metafile support.
// Added CreateStockObject() for NULL_BRUSH.
// 
//    Rev 1.1   03 May 1995 10:39:52   FRED
//  
// 
//    Rev 1.0   25 Apr 1995 10:35:20   FRED
// Initial revision.
*/

#include "stdafx.h"

#include <math.h>

#include "dibbrush.h"
#include "cfixed.h"
#include "polypoly.h"
#include "pagedefs.h"         // for MAKE_PALETTERGB() macro

/////////////////////////////////////////////////////////////////////////////
// CPolyPolygon

CPolyPolygon::CPolyPolygon(void)
{
   m_nContoursInPath = 0;
   m_nPointsInContour = 0;
   m_fUsePolyline = FALSE;
	m_fClosed = FALSE;
}

CPolyPolygon::~CPolyPolygon(void)
{
}

void CPolyPolygon::Draw(CDC* pDC, CFill* pFillObject, int nLineWidth, COLORREF clLineColor, BOOL fLineColorExists, int nFillMode, BOOL fUsePalette /*=TRUE*/)
{
   if ((m_nContoursInPath > 0)
    && (m_nPointsInContour == 0)
    && (!m_fFirstPoint))
   {
      CPen cpLine;
      CBrush cbFill;

      int nContext = pDC->SaveDC();

      if (nContext != 0)
      {
         pDC->SetPolyFillMode(nFillMode);

         if (pFillObject->FillColorExists())
         {
				CBrush* pOldBrush = NULL;
            Brush()->UsePalette(fUsePalette);
            if (Brush()->Color(pFillObject->GetFillColor()))
            {
               pOldBrush = pDC->SelectObject(Brush()->Brush());
            }

            if (!m_fClosed || ((nLineWidth >= 0) && fLineColorExists))
            {
            /*
            // Some HP drivers have a bug that causes a GPF if you attempt
            // to do a PolyPolygon with both a fill and stroke.
            // To work around that, we will break such cases into two
            // PolyPolygon() calls.
            */
					if (!m_fUsePolyline)
					{
						// Restore the brush before we do this
						if (pOldBrush)
						{
							pDC->SelectObject(pOldBrush);
						}
						if (pFillObject->SelectClipRegion(pDC, this))
						{
							pFillObject->DoFill(pDC, this);
							pFillObject->RestoreClipRegion(pDC);
						}
					}
               cbFill.CreateStockObject(NULL_BRUSH);
               pDC->SelectObject(&cbFill);
            }
         }
         else
         {
            cbFill.CreateStockObject(NULL_BRUSH);
            pDC->SelectObject(&cbFill);
         }

         if ((nLineWidth >= 0) && (fLineColorExists))
         {
            // If the line width is < 2 pixels, Windows won't dither it; it'll
            // map standard RGB COLORREFs to the nearest system color. We want
            // to use the colors in the palette if possible, so we map the RGB
            // to a PALETTERGB with our special macro. Then Windows will pick
            // the closest color in the palette. Note that on devices without
            // palette support this has no effect.

            if (nLineWidth < 2)
            {
               clLineColor = MAKE_PALETTERGB(clLineColor);
            }

            if (cpLine.CreatePen(PS_INSIDEFRAME, nLineWidth, clLineColor))
            {
               pDC->SelectObject(&cpLine);
            }
         }
         else
         {
            if (cpLine.CreateStockObject(NULL_PEN))
            {
               pDC->SelectObject(&cpLine);
            }
         }

         if (!m_fClosed || m_fUsePolyline)
         {
            PolyPolyline(pDC);
         }
         else
         {
				if (nLineWidth != -1)
				{
					pDC->PolyPolygon(Points(), Counts(), m_nContoursInPath);
				}
				else if (pFillObject->FillColorExists())
				{
					if (pFillObject->SelectClipRegion(pDC, this))
					{
						pFillObject->DoFill(pDC, this);
						pFillObject->RestoreClipRegion(pDC);
					}
				}
         }

//         TRACE("%d Contours, %d Points\n", m_nContoursInPath, m_Points.GetElementCount());
      }
      
      if (nContext != 0)
      {
      /* Always RestoreDC() with -1 to avoid problems in metafiles. */
         pDC->RestoreDC(-1 /*nContext*/);
      }
   }
}
               
void CPolyPolygon::Draw(CDC* pDC)
{
   if ((m_nContoursInPath > 0)
    && (m_nPointsInContour == 0)
    && (!m_fFirstPoint))
   {
      if (m_fUsePolyline)
      {
         PolyPolyline(pDC);
      }
      else
      {
         pDC->PolyPolygon(Points(), Counts(), m_nContoursInPath);
      }
   }
}
               
void CPolyPolygon::StartContour(long AX, long AY)
{
   EndContour(FALSE);
   m_fFirstPoint = TRUE;
   m_FirstPointX = AX;
   m_FirstPointY = AY;
}

void CPolyPolygon::EndContour(BOOL fClose)
{
	if (m_nPointsInContour > 0)
	{
		if (fClose)
		{
			m_fClosed = TRUE;
#if 0
			if ((m_LastPointX != m_FirstPointX)
				 || (m_LastPointY != m_FirstPointY))
			{
				if (!m_fUsePolyline)
				{
					AddPoint(RoundFixed(m_FirstPointX), RoundFixed(m_FirstPointY));
				}
			}
#endif      
		}
		AddCount();
   }
}

BOOL CPolyPolygon::ContinueContour(void)
{
   if (m_nPointsInContour == 0)
   {
      if (!m_fFirstPoint)
      {
         return FALSE;
      }
      
      AddPoint(RoundFixed(m_FirstPointX), RoundFixed(m_FirstPointY));
      m_LastPointX = m_FirstPointX;
      m_LastPointY = m_FirstPointY;
      m_fFirstPoint = FALSE;
   }
   
   return TRUE;
}

void CPolyPolygon::End(void)
{
   EndContour(FALSE);
}

void CPolyPolygon::Close(void)
{
   EndContour(TRUE);
}

void CPolyPolygon::MoveTo(long AX, long AY)
{
   StartContour(AX, AY);
}

void CPolyPolygon::LineTo(long AX, long AY)
{
   if (ContinueContour())
   {
      AddPoint(RoundFixed(AX), RoundFixed(AY));
      m_LastPointX = AX;
      m_LastPointY = AY;
   }
}

void CPolyPolygon::SplineTo(long AX, long AY, long BX, long BY)
{
   if (ContinueContour())
   {
      ConvertSpline(m_LastPointX, m_LastPointY, AX, AY, BX, BY);
      m_LastPointX = BX;
      m_LastPointY = BY;
   }
}

void CPolyPolygon::BezierTo(long AX, long AY, long BX, long BY, long CX, long CY)
{
   if (ContinueContour())
   {
      ConvertBezier(m_LastPointX, m_LastPointY, AX, AY, BX, BY, CX, CY);
      m_LastPointX = CX;
      m_LastPointY = CY;
   }
}

void CPolyPolygon::ConvertSpline(long AX, long AY, long BX, long BY, long CX, long CY)
{
   long M0X, M0Y, M1X, M1Y, M2X, M2Y;
   
   // Check for termination condition: Points A, B, and C are
   // colinear to within 1/2 pixel. This uses appoximate distance
   // algorithm from Graphics Gems II, pp. 10-13.
   
   M0X = (CX-AX) >> 4;
   M0Y = (CY-AY) >> 4;
   
   M1X = (M0X >= 0) ? M0X : -M0X;
   M1Y = (M0Y >= 0) ? M0Y : -M0Y;
   
   M2X = (M1X+M1Y+((((M1X < M1Y) ? M1X : M1Y)+1)>>1))>>1;
   
   M2Y = MulFixed(BY-AY, M0X)-MulFixed(BX-AX, M0Y);
   if (M2Y < 0) M2Y = -M2Y;
   
   if (M2X >= M2Y)
   {
      AddPoint(RoundFixed(CX), RoundFixed(CY));
      return;
   }

   M0X = MidPointFixed(AX, BX+1);
   M0Y = MidPointFixed(AY, BY+1);
   
   M1X = MidPointFixed(BX, CX+1);
   M1Y = MidPointFixed(BY, CY+1);
   
   M2X = MidPointFixed(M0X, M1X+1);
   M2Y = MidPointFixed(M0Y, M1Y+1);
   
   ConvertSpline(AX, AY, M0X, M0Y, M2X, M2Y);
   ConvertSpline(M2X, M2Y, M1X, M1Y, CX, CY);
}

void CPolyPolygon::ConvertBezier(long AX, long AY, long BX, long BY, long CX, long CY, long DX, long DY)
{
   long M0X, M0Y, M1X, M1Y, M2X, M2Y;
   
   // Check for termination condition: Points A, B, C, and D are
   // colinear to within 1/2 pixel. This uses appoximate distance
   // algorithm from Graphics Gems II, pp. 10-13.
   
   M0X = (DX-AX) >> 4;
   M0Y = (DY-AY) >> 4;
   
   M1X = (M0X >= 0) ? M0X : -M0X;
   M1Y = (M0Y >= 0) ? M0Y : -M0Y;
   
   M2X = (M1X+M1Y+((((M1X < M1Y) ? M1X : M1Y)+1)>>1))>>1;
   
   M2Y = MulFixed(BY-AY, M0X)-MulFixed(BX-AX, M0Y);
   if (M2Y < 0) M2Y = -M2Y;
   
   if (M2X >= M2Y)
   {
      M2Y = MulFixed(CY-AY, M0X)-MulFixed(CX-AX, M0Y);
      if (M2Y < 0) M2Y = -M2Y;
      
      if (M2X >= M2Y)
      {
         AddPoint(RoundFixed(DX), RoundFixed(DY));
         return;
      }
   }
   
   M0X = MidPointFixed(AX, BX+1);
   M0Y = MidPointFixed(AY, BY+1);
   
   M1X = MidPointFixed(BX, CX+1);
   M1Y = MidPointFixed(BY, CY+1);
   
   M2X = MidPointFixed(CX, DX+1);
   M2Y = MidPointFixed(CY, DY+1);
   
   // Reuse BX, BY
   
   BX = MidPointFixed(M0X, M1X+1);
   BY = MidPointFixed(M0Y, M1Y+1);
   
   // Reuse CX, CY
   
   CX = MidPointFixed(M1X, M2X+1);
   CY = MidPointFixed(M1Y, M2Y+1);
   
   // reuse M1X and M1Y
   
   M1X = MidPointFixed(BX, CX+1);
   M1Y = MidPointFixed(BY, CY+1);
   
   ConvertBezier(AX, AY, M0X, M0Y, BX, BY, M1X, M1Y);
   ConvertBezier(M1X, M1Y, CX, CY, M2X, M2Y, DX, DY);
}

void CPolyPolygon::GetBoundingRect(CRect& rct)
{
	// Return the smallest rectangle enclosing all points
	int nPoints = (int)m_Points.GetElementCount();
	rct.SetRect(0x7fffffff, 0x7fffffff, -0x7fffffff, -0x7fffffff);	// max, min
	for (int i = 0; i < nPoints; i++)
	{
		// Check left, top, right, bottom
		POINT pt = *(POINT*)m_Points.GetElement(i);
		if (pt.x < rct.left) rct.left = pt.x;
		if (pt.y < rct.top) rct.top = pt.y;
		if (pt.x > rct.right) rct.right = pt.x;
		if (pt.y > rct.bottom) rct.bottom = pt.y;
	}
}

void CPolyPolygon::GetBoundingBox(CPoint ptBox[], ANGLE dRot, long& nWidth, long& nHeight)
{
	GetBoundingBox(ptBox, dRot);
	nWidth = m_nBoundWidth;
	nHeight = m_nBoundHeight;
}

void CPolyPolygon::GetBoundingBox(CPoint ptBox[], ANGLE dRot, int& nFirstCorner)
{
	GetBoundingBox(ptBox, dRot);
	nFirstCorner = m_nFirstCorner;
}

void CPolyPolygon::GetBoundingBox(CPoint ptBox[], ANGLE dRot)
{
	// Rotate the points in the polygon back to center,
	// find a bounding box, the rotate back

	// Find the center point
	PPNT ptCenter = {0, 0};
	int nPoints = (int)m_Points.GetElementCount()-1;
	int i;
	for (i = 0; i < nPoints; i++)
	{
		ptCenter.x += ((POINT*)m_Points.GetElement(i))->x;
		ptCenter.y += ((POINT*)m_Points.GetElement(i))->y;
	}
	ptCenter.x /= nPoints;
	ptCenter.y /= nPoints;

	// Build an array of PPNTs
	PPNT *ptArray = new PPNT[nPoints];
	for (i = 0; i < nPoints; i++)
	{
		ptArray[i].x = ((POINT*)m_Points.GetElement(i))->x;
		ptArray[i].y = ((POINT*)m_Points.GetElement(i))->y;
	}

	// Rotate the points back
	rotate_points(ptArray, nPoints, ptCenter, -dRot);

	// Get the bounding rect
	PBOX rctBound;
	compute_enclosing_bound(ptArray, nPoints, &rctBound);
	m_nBoundWidth = rctBound.x1 - rctBound.x0;
	m_nBoundHeight = rctBound.y1 - rctBound.y0;
	delete [] ptArray;

	// Rotate bound back
	PPNT rctEnclose[4] = {
		{rctBound.x0, rctBound.y0},
		{rctBound.x0, rctBound.y1},
		{rctBound.x1, rctBound.y1},
		{rctBound.x1, rctBound.y0}
	};
	rotate_points(rctEnclose, 4, ptCenter, dRot);

	// Find 4 points with lowest & highest x,y values		
	long nLeftX = 0x7fffffff;
	long nRightX = -0x7fffffff;
	long nTopY = 0x7fffffff;
	long nBottomY = -0x7fffffff;
	int nLeftIndex = 0;
	int nRightIndex = 0;
	int nTopIndex = 0;
	int nBottomIndex = 0;
	for (i = 0; i < 4; i++)
	{
		// Check left
		PPNT pt = rctEnclose[i];
		if (pt.x <= nLeftX) 
		{
			// Get topmost left point
			if (pt.x < nLeftX || pt.y < rctEnclose[nLeftIndex].y)
			{
				nLeftX = pt.x;
				nLeftIndex = i;
			}
		}
	}
	for (i = 0; i < 4; i++)
	{
		// Check top
		PPNT pt = rctEnclose[i];
		if (pt.y <= nTopY) 
		{
			// Get rightmost top point
			if (pt.y < nTopY || pt.x > rctEnclose[nTopIndex].x)
			{
				nTopY = pt.y;
				nTopIndex = i;
			}
		}
	}
	for (i = 0; i < 4; i++)
	{
		// Check right
		PPNT pt = rctEnclose[i];
		if (pt.x >= nRightX)
		{
			// Get bottomost right point
			if (pt.x > nRightX || pt.y > rctEnclose[nRightIndex].y)
			{
				nRightX = pt.x;
				nRightIndex = i;
			}
		}
	}
	for (i = 0; i < 4; i++)
	{
		// Check bottom
		PPNT pt = rctEnclose[i];
		if (pt.y >= nBottomY)
		{
			// Get leftmost bottom point
			if (pt.y > nBottomY || pt.x < rctEnclose[nBottomIndex].x)
			{
				nBottomY = pt.y;
				nBottomIndex = i;
			}
		}
	}

	// Assign to points
	ptBox[0] = *(POINT*)&rctEnclose[nLeftIndex];
	ptBox[1] = *(POINT*)&rctEnclose[nBottomIndex];
	ptBox[2] = *(POINT*)&rctEnclose[nRightIndex];
	ptBox[3] = *(POINT*)&rctEnclose[nTopIndex];

	// Find first corner
	if (nLeftIndex == 0) m_nFirstCorner = 0;
	if (nBottomIndex == 0) m_nFirstCorner = 1;
	if (nRightIndex == 0) m_nFirstCorner = 2;
	if (nTopIndex == 0) m_nFirstCorner = 3;
}

void CPolyPolygon::GetBoundingCircle(CPoint& ptCenter, long& nRadius)
{
	// Find the center point
	int nPoints = (int)m_Points.GetElementCount()-1;
	int i;
	ptCenter.x = 0;
	ptCenter.y = 0;
	for (i = 0; i < nPoints; i++)
	{
		ptCenter.x += ((POINT*)m_Points.GetElement(i))->x;
		ptCenter.y += ((POINT*)m_Points.GetElement(i))->y;
	}
	ptCenter.x /= nPoints;
	ptCenter.y /= nPoints;

	// Point furthest away from center determines the radius
	long nDist, nMaxDist = -1;
	int nMaxPointIndex;
	for (i = 0; i < nPoints; i++)
	{
		CPoint pt = *(POINT*)m_Points.GetElement(i);
		CPoint ptDiff = pt - ptCenter;
		if ((nDist = ptDiff.x*ptDiff.x + ptDiff.y*ptDiff.y) > nMaxDist)
		{
			nMaxDist = nDist;
			nMaxPointIndex = i;
		}
	}

	// Compute the radius
	nRadius = (long)sqrt((double)nMaxDist);
}

/*
// Draw a PolyPolyline (in both 32- and 16-bit).
*/

void CPolyPolygon::PolyPolyline(CDC* pDC)
{
#ifdef WIN32
   pDC->PolyPolyline(Points(), (DWORD*)Counts(), m_nContoursInPath);
#else
   LPPOINT pPoints = Points();
   int* pCounts = (int*)Counts();
   for (WORD n = 0; n < m_nContoursInPath; n++)
   {
      pDC->Polyline(pPoints, *pCounts);
      pPoints += *pCounts++;
   }
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CPolyPolygonPointArray

CPolyPolygonPointArray::CPolyPolygonPointArray(void) :
   CDynamicArray(sizeof(POINT), 100, 65520/sizeof(POINT))
{
}

CPolyPolygonPointArray::~CPolyPolygonPointArray(void)
{
}

/////////////////////////////////////////////////////////////////////////////
// CPolyPolygonCountArray

CPolyPolygonCountArray::CPolyPolygonCountArray(void) :
   CDynamicArray(sizeof(int), 10, 65520/sizeof(int))
{
}

CPolyPolygonCountArray::~CPolyPolygonCountArray(void)
{
}
