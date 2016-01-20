/*
// $Header: /PM8/App/OUTPATH.CPP 1     3/03/99 6:08p Gbeddow $
//
// $Workfile: OUTPATH.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/OUTPATH.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 7     1/27/99 1:42p Lwilson
// Updated GetRPath() to better translate the path into RPath coordinate
// system.
// 
// 6     1/25/99 9:58a Lwilson
// Initial path component support
// 
// 5     11/06/98 4:14p Mwilson
// fixed line width problem
// 
// 4     5/06/98 1:36p Rlovejoy
// Partially fixed thick line drawing bug.
// 
// 3     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 2     4/29/98 6:31p Rlovejoy
// Updated to use fill objects.
// 
//    Rev 1.0   14 Aug 1997 15:23:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:30   Fred
// Initial revision.
// 
//    Rev 1.14   15 May 1997 16:43:16   Fred
// Overflow prevention
// 
//    Rev 1.13   19 Feb 1997 16:52:32   Jay
// Fixed PtInPath for unclosed paths.
// 
//    Rev 1.12   11 Feb 1997 14:54:38   Jay
// Free draw tool
// 
//    Rev 1.11   07 Feb 1997 08:50:28   Fred
// Removed a test
// 
//    Rev 1.10   31 Jan 1997 15:51:08   Jay
// Some new toolbar bitmaps.
// 
//    Rev 1.9   29 Jan 1997 14:18:20   Jay
// Outline path and PtInPath
// 
//    Rev 1.8   20 Jan 1997 14:07:06   Fred
// Inflate speedups
// 
//    Rev 1.7   17 Jan 1997 17:03:08   Jay
// New select for drawing objects.
// 
//    Rev 1.6   16 Jan 1997 09:00:56   Fred
// Fixed bug where closing line was not warped
// 
//    Rev 1.5   09 Jan 1997 16:54:22   Fred
//  
// 
//    Rev 1.4   06 Jan 1997 10:16:26   Fred
// Start of new text object
// 
//    Rev 1.3   01 Nov 1996 17:11:46   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.2   04 Sep 1996 16:34:24   Jay
// Fixes for Inflate filling in!
// 
//    Rev 1.1   24 May 1996 16:15:54   Fred
// TRACEx
// 
//    Rev 1.0   12 Apr 1996 10:54:32   Jay
// Initial revision.
# 
#    Rev 1.2   12 Apr 1996 10:53:00   Jay
# No change.
# 
#    Rev 1.1   14 Mar 1996 13:52:40   Jay
// 
//    Rev 1.0   16 Feb 1996 12:08:04   FRED
// Initial revision.
// 
//    Rev 1.29   22 Aug 1995 06:50:54   JAY
// Fixed mmsystem include for DEBUG build.
// 
//    Rev 1.28   19 Aug 1995 14:48:40   FRED
// Inflate now works with flipped objects
// 
//    Rev 1.27   23 Jun 1995 16:21:06   JAY
// Now makes sure things aren't too small.
// 
//    Rev 1.26   23 Jun 1995 11:39:26   JAY
// Got rid of some more overflows.
// 
//    Rev 1.25   23 Jun 1995 09:15:30   JAY
// More overflow (and other) protection.
// 
//    Rev 1.24   22 Jun 1995 16:06:00   JAY
// Fixed some bugs.
// 
//    Rev 1.23   30 May 1995 13:47:52   JAY
// More valiant attempts to exorcise all bugs from the inflate code.
// 
//    Rev 1.22   26 May 1995 11:46:08   JAY
// More inflate stuff (subdivide, etc.).
// 
//    Rev 1.21   25 May 1995 14:04:32   JAY
// Got rid of malloc.
// 
//    Rev 1.20   25 May 1995 10:56:28   JAY
// Timing output.
// 
//    Rev 1.19   25 May 1995 10:00:04   JAY
// Converted to beziers.
// 
//    Rev 1.18   24 May 1995 14:45:04   JAY
// Added angles to spline elements.
// 
//    Rev 1.17   24 May 1995 12:08:26   JAY
// Improvements.
// 
//    Rev 1.16   22 May 1995 17:29:44   JAY
// More inflate goodies.
// 
//    Rev 1.15   18 May 1995 10:51:50   FRED
// Code to get bounding box of path.
// 
// 
//    Rev 1.14   18 May 1995 07:45:14   JAY
// More inflate nonsense.
// 
//    Rev 1.13   16 May 1995 07:52:52   JAY
// Flipping functions in matrix.
// 
//    Rev 1.12   13 May 1995 11:14:42   JAY
//  
// 
//    Rev 1.11   11 May 1995 08:53:04   FRED
// Added COutlinePath::Flatten(). Inflate calls
// Flatten() before inflating.
// 
// 
//    Rev 1.10   10 May 1995 12:34:38   FRED
// Fixed bug in Draw2() where border was not
// drawing if no fill color was specified.
// 
// 
//    Rev 1.9   09 May 1995 09:48:20   JAY
// Added UsePalette flag.
// 
//    Rev 1.8   09 May 1995 08:03:46   JAY
// Added some initial mitering stuff.
// 
//    Rev 1.7   08 May 1995 09:09:34   FRED
// Added Draw2() which draws using Inflate() and other nifty stuff.
// 
// 
//    Rev 1.6   05 May 1995 16:49:28   JAY
// Exciting new additions to OutlinePaths!
// 
//    Rev 1.5   04 May 1995 17:16:06   JAY
// Fixed-point. Drawing objects.
// 
//    Rev 1.4   03 May 1995 10:39:50   FRED
//  
// 
//    Rev 1.3   28 Apr 1995 13:49:18   FRED
//  
// 
//    Rev 1.2   27 Apr 1995 13:21:32   FRED
//  
// 
//    Rev 1.1   25 Apr 1995 16:02:32   FRED
// 
//    Rev 1.0   25 Apr 1995 10:33:48   FRED
// Initial revision.
*/

#include "stdafx.h"
ASSERTNAME

#include "outpath.h"
#include "pathit.h"
#include "warpfld.h"
#include "polypoly.h"
#include "pagedefs.h"
#include "file.h"
#include "refresh.h"
#include "pmgfont.h"
#include "tfengine.h"

// Framework support
#include "Path.h"

#include <math.h>

//#define _DEBUG_OUTPATH
#ifdef _DEBUG_OUTPATH
#include <mmsystem.h>
static DWORD dwArcTangents;
static DWORD dwArcTangentTime;

static CFixed near MyArcTangent(CFixed dy, CFixed dx)
{
   CFixed lResult;
   dwArcTangentTime -= timeGetTime();
   lResult = ArcTangent2(dy, dx);
   dwArcTangentTime += timeGetTime();
   dwArcTangents++;
   return lResult;
}

#define ArcTangent2(dy, dx) MyArcTangent(dy, dx)
#endif

static CFixed near SafeDivFixed(CFixed lNum, CFixed lDenom)
{
   CFixed lAbsNum = (lNum < 0) ? -lNum : lNum;
   CFixed lAbsDenom = (lDenom < 0) ? -lDenom : lDenom;

   if (lAbsDenom == 0
       || (FixedInteger(lAbsDenom) == 0
           && (lAbsNum >= 0x3fff8000L
               || ((unsigned)FixedInteger(lAbsNum+0x8000))*2 >= FixedFraction(lAbsDenom))))
   {
      return ((lNum ^ lDenom) < 0) ? -0x7fffffffL : 0x7fffffffL;
   }
   return DivFixed(lNum, lDenom);
}

/////////////////////////////////////////////////////////////////////////////
// COutlinePathMatrix

COutlinePathMatrix::COutlinePathMatrix(void)
{
   Identity();
}

COutlinePathMatrix::COutlinePathMatrix(long A, long B, long C, long D, long X, long Y)
{
   Set(  A, B,
         C, D,
         X, Y  );
}

COutlinePathMatrix::~COutlinePathMatrix(void)
{
}

void COutlinePathMatrix::Set(long A, long B, long C, long D, long X, long Y)
{
   m_A = A;
   m_B = B;
   m_C = C;
   m_D = D;
   m_X = X;
   m_Y = Y;
}

void COutlinePathMatrix::Identity(void)
{
   Set(  MakeFixed(1), MakeFixed(0),
         MakeFixed(0), MakeFixed(1),
         MakeFixed(0), MakeFixed(0) );
}

void COutlinePathMatrix::ScaleBy(long X, long Y)
{
   COutlinePathMatrix Matrix(    X,             MakeFixed(0),
                                 MakeFixed(0),  Y,
                                 MakeFixed(0),  MakeFixed(0)      );
   Concatenate(&Matrix);
}

void COutlinePathMatrix::RotateBy(long T)
{
   // Note: signs of Sine() are reversed because Y axis is reversed
   COutlinePathMatrix Matrix(    Cosine(T),     -Sine(T),
                                 Sine(T),       Cosine(T),
                                 MakeFixed(0),  MakeFixed(0)      );
   Concatenate(&Matrix);
}

void COutlinePathMatrix::TranslateBy(long X, long Y)
{
   COutlinePathMatrix Matrix(    MakeFixed(1),  MakeFixed(0),
                                 MakeFixed(0),  MakeFixed(1),
                                 X,             Y                 );
   Concatenate(&Matrix);
}

void COutlinePathMatrix::XFlip(long Min, long Max)
{
   ScaleBy(-MakeFixed(1), MakeFixed(1));
   TranslateBy(Max+Min, MakeFixed(0));
}

void COutlinePathMatrix::YFlip(long Min, long Max)
{
   ScaleBy(MakeFixed(1), -MakeFixed(1));
   TranslateBy(MakeFixed(0), Max+Min);
}

void COutlinePathMatrix::Concatenate(COutlinePathMatrix* M)
{
   Set(  MulFixed(m_A,M->m_A)+MulFixed(m_B,M->m_C),         MulFixed(m_A,M->m_B)+MulFixed(m_B,M->m_D),
         MulFixed(m_C,M->m_A)+MulFixed(m_D,M->m_C),         MulFixed(m_C,M->m_B)+MulFixed(m_D,M->m_D),
         MulFixed(m_X,M->m_A)+MulFixed(m_Y,M->m_C)+M->m_X,  MulFixed(m_X,M->m_B)+MulFixed(m_Y,M->m_D)+M->m_Y   );
}

void COutlinePathMatrix::Transform(long* X, long* Y)
{
   long NewX = MulFixed(*X,m_A)+MulFixed(*Y,m_C)+m_X;
   long NewY = MulFixed(*X,m_B)+MulFixed(*Y,m_D)+m_Y;

   *X = NewX;
   *Y = NewY;
}

/////////////////////////////////////////////////////////////////////////////
// COutlinePathArray

COutlinePathArray::COutlinePathArray(void) :
   CDynamicArray(sizeof(long), 100, 65520/sizeof(long))
{
}

COutlinePathArray::~COutlinePathArray(void)
{
}

void COutlinePathArray::End(void)
{
   OutlinePathEndRecord Record;
   Record.dwElement = OPE_End;
   AddElement(&Record, 1);
}

void COutlinePathArray::Close(void)
{
   OutlinePathCloseRecord Record;
   Record.dwElement = OPE_Close;
   AddElement(&Record, 1);
}

void COutlinePathArray::MoveTo(long AX, long AY)
{
   OutlinePathMoveToRecord Record;
   Record.dwElement = OPE_MoveTo;
   Record.AX = AX;
   Record.AY = AY;
   AddElement(&Record, 3);
}

void COutlinePathArray::LineTo(long AX, long AY)
{
   OutlinePathLineToRecord Record;
   Record.dwElement = OPE_LineTo;
   Record.AX = AX;
   Record.AY = AY;
   AddElement(&Record, 3);
}

void COutlinePathArray::SplineTo(long AX, long AY, long BX, long BY)
{
   OutlinePathSplineToRecord Record;
   Record.dwElement = OPE_SplineTo;
   Record.AX = AX;
   Record.AY = AY;
   Record.BX = BX;
   Record.BY = BY;
   AddElement(&Record, 5);
}

void COutlinePathArray::BezierTo(long AX, long AY, long BX, long BY, long CX, long CY)
{
   OutlinePathBezierToRecord Record;
   Record.dwElement = OPE_BezierTo;
   Record.AX = AX;
   Record.AY = AY;
   Record.BX = BX;
   Record.BY = BY;
   Record.CX = CX;
   Record.CY = CY;
   AddElement(&Record, 7);
}

/////////////////////////////////////////////////////////////////////////////
// COutlinePathIterator

COutlinePathIterator::COutlinePathIterator(COutlinePathArray* pArray)
{
   m_pArray = pArray;
   m_pArrayData = (long*)m_pArray->GetElement(0);

   m_pCurrentPath = NULL;
   m_pCurrentComponent = NULL;
   m_nCurrentPoints = 0;
   m_nCurrentPoint = 0;
   m_pDiscoveredEOP = NULL;
}

void COutlinePathIterator::Fixup(void)
{
   long* pNewData = (long*)m_pArray->GetElement(0);

   if (m_pArrayData != pNewData)
   {
      if (m_pCurrentPath != NULL)
      {
         m_pCurrentPath = pNewData + (m_pCurrentPath - m_pArrayData);
      }

      if (m_pCurrentComponent != NULL)
      {
         m_pCurrentComponent = pNewData + (m_pCurrentComponent - m_pArrayData);
      }

      m_pArrayData = pNewData;
   }

/* EOP is now Unknown. */
   m_pDiscoveredEOP = NULL;
}

/*
// Move to the first path in the array.
*/

void COutlinePathIterator::FirstPath(void)
{
/* Start with the first path. */

   m_pCurrentPath = (long*)m_pArray->GetElement(0);
   m_pDiscoveredEOP = FALSE;

/* And establish the first component. */

   FirstPoint();
}

/*
// Move to the next path in the array.
*/

BOOL COutlinePathIterator::NextPath(void)
{
/* Go to the end of path (Close, End or next MoveTo). */

   ToEOP();

/*
// Advance to component past this.
// Note that if we are at End, this will do nothing.
*/

   if (AtClose())
   {
      NextComponent();
   }

/* Remember the start of the path. */

   m_pCurrentPath = m_pCurrentComponent;
   m_pDiscoveredEOP = NULL;

/* Say whether this is the end or not. */

   return !AtEnd();
}

/*
// Go to the first point.
*/

CFixedPoint* COutlinePathIterator::FirstPoint(void)
{
   SetNewComponent(m_pCurrentPath);
   return CurrentPoint();
}

/*
// Return the current point.
*/

CFixedPoint* COutlinePathIterator::CurrentPoint(void)
{
   if (m_pCurrentComponent == NULL || m_nCurrentPoint >= m_nCurrentPoints)
   {
   /* This handles the 0 points case as well. */
      return NULL;
   }

   return ComponentPoint(m_pCurrentComponent, m_nCurrentPoint);
}

/*
// Are we at the end of a path piece?
*/

BOOL COutlinePathIterator::AtEOP(void) const
{
   return AtEnd()
         || AtClose()
         || (*m_pCurrentComponent == COutlinePathArray::OPE_MoveTo
             && m_pCurrentComponent != m_pCurrentPath);
}

/*
// Move to the next point.
*/

CFixedPoint* COutlinePathIterator::NextPoint(void)
{
   if (++m_nCurrentPoint == m_nCurrentPoints)
   {
   /* We hit the end. */

      NextComponent();

      if (AtEOP())
      {
      /* Wrap to the beginning. */
         return FirstPoint();
      }
   }
   return CurrentPoint();
}

/*
// Set a new component.
*/

void COutlinePathIterator::SetNewComponent(long* pComponent)
{
   m_pCurrentComponent = pComponent;
   m_nCurrentPoints = ComponentPoints();
   m_nCurrentPoint = 0;

   if (AtEOP())
   {
      m_pDiscoveredEOP = m_pCurrentComponent;
   }
}

/*
// Is this (or the current) component valid?
*/

BOOL COutlinePathIterator::IsValidComponent(long* pComponent /*=NULL*/)
{
   if (pComponent == NULL)
   {
      pComponent = m_pCurrentComponent;
   }

   return   (pComponent != NULL)
         && (*pComponent >= COutlinePathArray::OPE_End)
         && (*pComponent < COutlinePathArray::OPE_ElementCount);
}

long* COutlinePathIterator::GetNextComponent(void)
{
   int nDelta = 0;

   if (IsValidComponent())
   {
      static int nBumpIt[] =
      {
         0,          // End: Stay here forever.
         1,          // Close: Me
         1+1*2,      // MoveTo: Me and my 1 point
         1+1*2,      // LineTo: Me and my 1 point
         1+2*2,      // SplineTo: Me and my 2 points
         1+3*2,      // BezierTo: Me and my 3 points
      };

      nDelta = nBumpIt[(int)*m_pCurrentComponent];
   }
   return m_pCurrentComponent + nDelta;
}

/*
// Move to the next component.
*/

long* COutlinePathIterator::NextComponent(void)
{
   SetNewComponent(GetNextComponent());
   return m_pCurrentComponent;
}

/*
// Move to the beginning of the current path.
*/

void COutlinePathIterator::ToBOP(void)
{
   SetNewComponent(m_pCurrentPath);
}

/*
// Find the end of the current path.
// It is assumed that the current component is somewhere within the path
// in question.
*/

void COutlinePathIterator::ToEOP(void)
{
   while (m_pDiscoveredEOP == NULL)
   {
      NextComponent();
   }
   m_pCurrentComponent = m_pDiscoveredEOP;
}

/*
// Return the number of points in a (or the current) component.
*/

int COutlinePathIterator::ComponentPoints(long* pComponent /*=NULL*/)
{
   if (pComponent == NULL)
   {
      pComponent = m_pCurrentComponent;
   }

   if (IsValidComponent(pComponent))
   {
      static int nPoints[] =
      {
         0,       // End
         0,       // Close
         1,       // MoveTo
         1,       // LineTo
         2,       // SplineTo
         3,       // BezierTo
      };
      return nPoints[(int)*pComponent];
   }
   return 0;
}

/////////////////////////////////////////////////////////////////////////////
// COutlinePath

//static CFixed lDefaultMiterLimit = MakeFixed(0);
static CFixed lDefaultMiterLimit = MakeFixed(2);

COutlinePath::COutlinePath(void)
{
// m_lMiterLimit = MakeFixed(1);
   m_lMiterLimit = lDefaultMiterLimit;
   m_fUsePolyline = FALSE;
   m_fIsWarped = FALSE;
}

COutlinePath::~COutlinePath(void)
{
}

BOOL COutlinePath::Assign(COutlinePath* pPath)
{
   BOOL fResult = CDynamicArray::Assign(pPath);

   if (fResult)
   {
      UsePolyline(pPath->UsePolyline());
      pPath->UsePolyline(FALSE);
      IsWarped(pPath->IsWarped());
      pPath->IsWarped(FALSE);
   }

   return fResult;
}

BOOL COutlinePath::Copy(const COutlinePath* pPath)
{
   BOOL fResult = CDynamicArray::Copy(pPath);

   if (fResult)
   {
      UsePolyline(pPath->UsePolyline());
      IsWarped(pPath->IsWarped());
   }

   return fResult;
}

BOOL COutlinePath::GetRPath( RPath& rPath, const R2dTransform& transform ) const
{
	EPathOperatorArray	opArray;	// the RPath operations
	RIntPointArray			ptArray;	// the RPath points
	int						nRepCount = 0;

	rPath.Undefine();

	// Note: GetElement() is not a const function.  But because no class data
	// is being changed this function can be left const, and a non-const this
	// pointer can be used for the call.
	long* pElement = (long*)const_cast<COutlinePath *>( this )->GetElement(0);
	long  nLastElement = -1;

   while (pElement != NULL)
   {
		long nCurElement = *(pElement++);

      switch (nCurElement)
      {
         case OPE_End:
         {
				opArray.InsertAtEnd( kEnd );
				rPath.MergePoints( opArray, ptArray );
				RIntRect rBounds = rPath.GetBoundingRect();
//				RIntSize rSize = rPath.GetPathSize();

				R2dTransform xform( transform );
				xform.PreTranslate( -rBounds.m_Left, 0 );
				xform.PreTranslate( 0, Abs( rBounds.m_Bottom ) );
				xform.PreScale( 1, -1 );

				rPath = RPath( rPath, xform );

            return TRUE;
         }

         case OPE_Close:
         {
           	opArray.InsertAtEnd( kClose );
            break;
         }

         case OPE_MoveTo:
         {
				RIntPoint pt( pElement[0], pElement[1] );
				pElement += 2;

				ptArray.InsertAtEnd( pt );
				opArray.InsertAtEnd( kMoveTo );
            break;
         }

         case OPE_LineTo:
         {
				RIntPoint pt( pElement[0], pElement[1] );
				pElement += 2;

				ptArray.InsertAtEnd( pt );

				if (OPE_LineTo != nLastElement)
				{
					nRepCount = 1;
					opArray.InsertAtEnd( kLine );
					opArray.InsertAtEnd( (EPathOperator) nRepCount );
				}
				else
				{
					// The last item in the list should be a rep count
					TpsAssert( opArray[opArray.Count() - 2] == kLine, "Corrupt path data!" );
					opArray[opArray.Count() - 1] = (EPathOperator) ++nRepCount;
				}

            break;
         }

         case OPE_SplineTo:
         {
				RIntPoint pt1( pElement[0], pElement[1] );
				RIntPoint pt2( pElement[2], pElement[3] );
				pElement += 4;

				ptArray.InsertAtEnd( pt1 );
				ptArray.InsertAtEnd( pt2 );

				if (OPE_SplineTo != nLastElement)
				{
					nRepCount = 1;
  					opArray.InsertAtEnd( kQuadraticSpline );
					opArray.InsertAtEnd( (EPathOperator) nRepCount );
				}
				else
				{
					// The last item in the list should be a rep count
					TpsAssert( opArray[opArray.Count() - 2] == kQuadraticSpline, "Corrupt path data!" );
					opArray[opArray.Count() - 1] = (EPathOperator) ++nRepCount;
				}

            break;
         }

         case OPE_BezierTo:
         {
				RIntPoint pt1( pElement[0], pElement[1] );
				RIntPoint pt2( pElement[2], pElement[3] );
				RIntPoint pt3( pElement[4], pElement[5] );
				pElement += 6;

				ptArray.InsertAtEnd( pt1 );
				ptArray.InsertAtEnd( pt2 );
				ptArray.InsertAtEnd( pt3 );

				if (OPE_BezierTo != nLastElement)
				{
					nRepCount = 1;
  					opArray.InsertAtEnd( kBezier );
					opArray.InsertAtEnd( (EPathOperator) nRepCount );
				}
				else
				{
					// The last item in the list should be a rep count
					TpsAssert( opArray[opArray.Count() - 2] == kBezier, "Corrupt path data!" );
					opArray[opArray.Count() - 1] = (EPathOperator) ++nRepCount;
				}

            break;
         }

         default:
         {
            ASSERT(FALSE);
            return FALSE;
         }
      }

		nLastElement = nCurElement;
   }

	return FALSE;  // Exits with TRUE in OPE_End case handler
}

/*
// Helper function for PtInPath.
*/

static int IntersectLines(CFixedPoint p0, CFixedPoint p1, CFixedPoint po, CFixedPoint p)
{
	// This line intersection is from Graphics Gems III (IV.6)

	int nIntersections = 0;
	double dAx, dAy, dBx, dBy, dCx, dCy;

	dAx = MakeDouble(p1.x - p0.x);
	dAy = MakeDouble(p1.y - p0.y);
	dBx = MakeDouble(p.x - po.x);
	dBy = MakeDouble(p.y - po.y);
	dCx = MakeDouble(p0.x - p.x);
	dCy = MakeDouble(p0.y - p.y);

	double dDenom = dAy*dBx - dAx*dBy;
	// A zero denominator implies collinear. We drop that case.
	if (fabs(dDenom) > 0.001)
	{
		double dNumX = dBy*dCx - dBx*dCy;
		double dNumY = dAx*dCy - dAy*dCx;
		if (dDenom > 0)
		{
			if (dNumX >= 0 && dNumX < dDenom && dNumY >= 0 && dNumY < dDenom)
			{
				nIntersections++;
			}
		}
		else
		{
			if (dNumX <= 0 && dNumX > dDenom && dNumY <= 0 && dNumY > dDenom)
			{
				nIntersections++;
			}
		}
	}
	return nIntersections;
}


/*
// Return whether a point is within this path or not.
*/

BOOL COutlinePath::PtInPath(CFixedPoint p)
{
   // Compute a point known to be outside the path.
   CFixedRect StartRect;
   QuickBoundingBox(&StartRect);

   CFixedPoint po;
   po.x = StartRect.Left - MakeFixed(10);
   po.y = StartRect.Top - MakeFixed(10);

   // We now two points defining a line that we can intersect
   // with the path components. Since one of the points is outside,
   // an odd number of intersections implies "insideness"; an even number
   // implies "outsideness".

   int nIntersections = 0;

   COutlinePath Path;
   Flatten(&Path);
   COutlinePathIterator it(&Path);

   it.FirstPath();            // Move to the first path.

   while (!it.AtEnd())
   {
      BOOL fCurrent = FALSE;
      CFixedPoint p0;
		CFixedPoint FirstPoint;

      while (!it.AtEOP())
      {
         switch (*(it.CurrentComponent()))
         {
            case OPE_MoveTo:
            {
               ASSERT(!fCurrent);

               fCurrent = TRUE;
               p0 = *(it.CurrentPoint());
					FirstPoint = p0;
               break;
            }

            case OPE_LineTo:
            {
               ASSERT(fCurrent);

               CFixedPoint p1 = *(it.CurrentPoint());

               // We have a line defined by (p0, p1).
					nIntersections += IntersectLines(p0, p1, po, p);

               p0 = p1;
               break;
            }

            case OPE_SplineTo:
            {
               ASSERT(fCurrent);

               CFixedPoint p1 = *(it.CurrentPoint());
               CFixedPoint p2 = *(it.NextPoint());

               // We have a spline defined by (p0, p1, p2).

               p0 = p2;
               break;
            }

            case OPE_BezierTo:
            {
               ASSERT(fCurrent);

               CFixedPoint p1 = *(it.CurrentPoint());
               CFixedPoint p2 = *(it.NextPoint());
               CFixedPoint p3 = *(it.NextPoint());

               // We have a bezier defined by (p0, p1, p2, p3).

               p0 = p3;
               break;
            }
            default:
            {
               break;
            }
         }
         it.NextComponent();
      }

		// Close the path for intersection purposes if not already closed.
		if (fCurrent && (p0.x != FirstPoint.x || p0.y != FirstPoint.y))
		{
			nIntersections += IntersectLines(p0, FirstPoint, po, p);
		}

      it.NextPath();
   }

   // Odd is inside, even is outside.
   return (nIntersections & 1) != 0;
}

void COutlinePath::Draw(CDC* pDC, COLORREF clFillColor, BOOL fFillColorExists, int nLineWidth, COLORREF clLineColor, BOOL fLineColorExists, int nFillMode, BOOL fUsePalette /*= TRUE*/)
{
	CFill* pFillObject = DeriveFillType(FillFormatV1::FillSolid);
	pFillObject->SetFillColorref(clFillColor, fFillColorExists);
	Draw(
		pDC,
		pFillObject,
		nLineWidth,
		clLineColor,
		fLineColorExists,
//			WINDING,
		ALTERNATE,
		fUsePalette);
	delete pFillObject;
}

void COutlinePath::Draw(CDC* pDC, CFill* pFillObject, int nLineWidth, COLORREF clLineColor, BOOL fLineColorExists, int nFillMode, BOOL fUsePalette /*=TRUE*/)
{
   CPolyPolygon Polygon;
   Polygon.UsePolyline(m_fUsePolyline);

   long* pElement = (long*)GetElement(0);

   while (pElement != NULL)
   {
      switch (*(pElement++))
      {
         case OPE_End:
         {
            Polygon.End();
            Polygon.Draw(pDC, pFillObject, nLineWidth == NoLine ? -1 : nLineWidth, clLineColor, fLineColorExists, nFillMode, fUsePalette);
            return;
         }

         case OPE_Close:
         {
            Polygon.Close();
            break;
         }

         case OPE_MoveTo:
         {
            long AX = *(pElement++);
            long AY = *(pElement++);
            Polygon.MoveTo(AX, AY);
            break;
         }

         case OPE_LineTo:
         {
            long AX = *(pElement++);
            long AY = *(pElement++);
            Polygon.LineTo(AX, AY);
            break;
         }

         case OPE_SplineTo:
         {
            long AX = *(pElement++);
            long AY = *(pElement++);
            long BX = *(pElement++);
            long BY = *(pElement++);
            Polygon.SplineTo(AX, AY, BX, BY);
            break;
         }

         case OPE_BezierTo:
         {
            long AX = *(pElement++);
            long AY = *(pElement++);
            long BX = *(pElement++);
            long BY = *(pElement++);
            long CX = *(pElement++);
            long CY = *(pElement++);
            Polygon.BezierTo(AX, AY, BX, BY, CX, CY);
            break;
         }

         default:
         {
            ASSERT(FALSE);
            return;
         }
      }
   }
}

void COutlinePath::Draw2(CDC* pDC, COLORREF clFillColor, BOOL fFillColorExists, int nLineWidth, COLORREF clLineColor, BOOL fLineColorExists, int nFillMode, BOOL fUsePalette /*=TRUE*/)
{
	CFill* pFillObject = DeriveFillType(FillFormatV1::FillSolid);
	pFillObject->SetFillColorref(clFillColor, fFillColorExists);
	Draw2(
		pDC,
		pFillObject,
		nLineWidth,
		clLineColor,
		fLineColorExists,
//			WINDING,
		ALTERNATE,
		fUsePalette);
	delete pFillObject;
}

void COutlinePath::Draw2(CDC* pDC, CFill* pFillObject, int nLineWidth, COLORREF clLineColor, BOOL fLineColorExists, int nFillMode, BOOL fUsePalette /*=TRUE*/)
{
   if (!fLineColorExists || (nLineWidth == NoLine))
   {
      // There is no border to draw, simply fill the interior.
      Draw(pDC, pFillObject, NoLine, RGB(0,0,0), FALSE, nFillMode, fUsePalette);
   }
   else
   {
		BOOL fIsClosed = IsClosed();

      // There is a border. If it is "thin", use a single pixel GDI brush to
      // render it. Otherwise, use Inflate().

      if (!fIsClosed || (nLineWidth == -1) || (nLineWidth == 0) || (nLineWidth == 1))
      {
			if(nLineWidth < 0)
				nLineWidth = -nLineWidth;

			if (nLineWidth <= 1)
			{
				// Use a single pixel pen to draw the border.
				nLineWidth = 0;
			}
         // Fill the interior and use a pen to draw the border.
         Draw(pDC, pFillObject, nLineWidth, clLineColor, fLineColorExists, nFillMode, fUsePalette);
      }
      else
      {
         // Compute two polygons: one for the border and one for the interior.
         COutlinePath Path;
         Path.Copy(this);
         Path.Inflate(-MakeFixed(nLineWidth));
         Path.Draw(pDC, pFillObject, NoLine, RGB(0,0,0), FALSE, nFillMode, fUsePalette);
         
         if (fLineColorExists)
         {
				CFill* pFlatFillObject = DeriveFillType(FillFormatV1::FillSolid);
				pFlatFillObject->SetFillColorref(clLineColor, fLineColorExists);
            Path.Reverse();
            Path.Append(this);
            Path.Draw(pDC, pFlatFillObject, NoLine, RGB(0,0,0), FALSE, nFillMode, fUsePalette);
				delete pFlatFillObject;
         }
      }
   }
}

void COutlinePath::Draw(CDC* pDC)
{
   CPolyPolygon Polygon;
   Polygon.UsePolyline(m_fUsePolyline);

   long* pElement = (long*)GetElement(0);

   while (pElement != NULL)
   {
      switch (*(pElement++))
      {
         case OPE_End:
         {
				Polygon.End();
            Polygon.Draw(pDC);
            return;
         }

         case OPE_Close:
         {
            Polygon.Close();
            break;
         }

         case OPE_MoveTo:
         {
            long AX = *(pElement++);
            long AY = *(pElement++);
            Polygon.MoveTo(AX, AY);
            break;
         }

         case OPE_LineTo:
         {
            long AX = *(pElement++);
            long AY = *(pElement++);
            Polygon.LineTo(AX, AY);
            break;
         }

         case OPE_SplineTo:
         {
            long AX = *(pElement++);
            long AY = *(pElement++);
            long BX = *(pElement++);
            long BY = *(pElement++);
            Polygon.SplineTo(AX, AY, BX, BY);
            break;
         }

         case OPE_BezierTo:
         {
            long AX = *(pElement++);
            long AY = *(pElement++);
            long BX = *(pElement++);
            long BY = *(pElement++);
            long CX = *(pElement++);
            long CY = *(pElement++);
            Polygon.BezierTo(AX, AY, BX, BY, CX, CY);
            break;
         }

         default:
         {
            ASSERT(FALSE);
            return;
         }
      }
   }
}

void COutlinePath::Warp(CWarpField* pWarpField, COutlinePath* pDestination /*=NULL*/)
{
   BOOL fInPlace = FALSE;

   if (pDestination == NULL)
   {
      pDestination = new COutlinePath;
      fInPlace = TRUE;
   }

   if (pDestination != NULL)
   {
      // Polyline flag is copied over.
      pDestination->UsePolyline(UsePolyline());

      pDestination->Free();

      long* pElement = (long*)GetElement(0);

      long StartX = 0;
      long StartY = 0;
      long LastX = 0;
      long LastY = 0;

      while (pElement != NULL)
      {
         switch (*(pElement++))
         {
            case OPE_End:
            {
               pDestination->End();
               pElement = NULL;
               break;
            }

            case OPE_Close:
            {
               if (!UsePolyline())
               {
                  if ((StartX != LastX) || (StartY != LastY))
                  {
                     pWarpField->LineTo(pDestination, StartX, StartY);
                  }
               }
               pDestination->Close();
               break;
            }

            case OPE_MoveTo:
            {
               long AX = StartX = LastX = *(pElement++);
               long AY = StartY = LastY = *(pElement++);
               pWarpField->MoveTo(pDestination, AX, AY);
               break;
            }

            case OPE_LineTo:
            {
               long AX = LastX = *(pElement++);
               long AY = LastY = *(pElement++);
               pWarpField->LineTo(pDestination, AX, AY);
               break;
            }

            case OPE_SplineTo:
            {
               long AX = *(pElement++);
               long AY = *(pElement++);
               long BX = LastX = *(pElement++);
               long BY = LastY = *(pElement++);
               pWarpField->SplineTo(pDestination, AX, AY, BX, BY);
               break;
            }

            case OPE_BezierTo:
            {
               long AX = *(pElement++);
               long AY = *(pElement++);
               long BX = *(pElement++);
               long BY = *(pElement++);
               long CX = LastX = *(pElement++);
               long CY = LastY = *(pElement++);
               pWarpField->BezierTo(pDestination, AX, AY, BX, BY, CX, CY);
               break;
            }

            default:
            {
               ASSERT(FALSE);
               pDestination->Free();
               pElement = NULL;
               break;
            }
         }
      }

      // Set Warped flag is we warped into a non-linear field.
      pDestination->IsWarped(!(pWarpField->Linear()));

      if (fInPlace)
      {
         Assign(pDestination);
      }
   }

   if (fInPlace)
   {
      if (pDestination != NULL)
      {
         delete pDestination;
         pDestination = NULL;
      }
   }
}

void COutlinePath::Transform(COutlinePathMatrix* pMatrix, COutlinePath* pDestination /*=NULL*/)
{
   if (pDestination != NULL)
   {
      pDestination->Free();
   }

   if (pDestination != NULL)
   {
      // Polyline and Warp flags are copied over.
      pDestination->UsePolyline(UsePolyline());
      pDestination->IsWarped(IsWarped());
   }

   long* pElement = (long*)GetElement(0);

   while (pElement != NULL)
   {
      switch (*(pElement++))
      {
         case OPE_End:
         {
            if (pDestination != NULL)
            {
               pDestination->End();
            }
            return;
         }

         case OPE_Close:
         {
            if (pDestination != NULL)
            {
               pDestination->Close();
            }
            break;
         }

         case OPE_MoveTo:
         {
            if (pDestination != NULL)
            {
               long AX = *(pElement++);
               long AY = *(pElement++);
               pMatrix->Transform(&AX, &AY);
               pDestination->MoveTo(AX, AY);
            }
            else
            {
               pMatrix->Transform(pElement+0, pElement+1);
               pElement += 2;
            }
            break;
         }

         case OPE_LineTo:
         {
            if (pDestination != NULL)
            {
               long AX = *(pElement++);
               long AY = *(pElement++);
               pMatrix->Transform(&AX, &AY);
               pDestination->LineTo(AX, AY);
            }
            else
            {
               pMatrix->Transform(pElement+0, pElement+1);
               pElement += 2;
            }
            break;
         }

         case OPE_SplineTo:
         {
            if (pDestination != NULL)
            {
               long AX = *(pElement++);
               long AY = *(pElement++);
               long BX = *(pElement++);
               long BY = *(pElement++);
               pMatrix->Transform(&AX, &AY);
               pMatrix->Transform(&BX, &BY);
               pDestination->SplineTo(AX, AY, BX, BY);
            }
            else
            {
               pMatrix->Transform(pElement+0, pElement+1);
               pMatrix->Transform(pElement+2, pElement+3);
               pElement += 4;
            }
            break;
         }

         case OPE_BezierTo:
         {
            if (pDestination != NULL)
            {
               long AX = *(pElement++);
               long AY = *(pElement++);
               long BX = *(pElement++);
               long BY = *(pElement++);
               long CX = *(pElement++);
               long CY = *(pElement++);
               pMatrix->Transform(&AX, &AY);
               pMatrix->Transform(&BX, &BY);
               pMatrix->Transform(&CX, &CY);
               pDestination->BezierTo(AX, AY, BX, BY, CX, CY);
            }
            else
            {
               pMatrix->Transform(pElement+0, pElement+1);
               pMatrix->Transform(pElement+2, pElement+3);
               pMatrix->Transform(pElement+4, pElement+5);
               pElement += 6;
            }
            break;
         }

         default:
         {
            ASSERT(FALSE);
            if (pDestination != NULL)
            {
               pDestination->Free();
            }
            return;
         }
      }
   }
}

/*
// This computes whether a given outline path is closed or not.
// This assumes that all contours have the same closed status.
*/

BOOL COutlinePath::IsClosed(void)
{
   long* pElement = (long*)GetElement(0);

   while (pElement != NULL)
   {
      switch (*(pElement++))
      {
         case OPE_End:
         {
            return FALSE;
         }

         case OPE_Close:
         {
				return TRUE;
         }

         case OPE_MoveTo:
         {
				pElement += 2;
            break;
         }

         case OPE_LineTo:
         {
				pElement += 2;
            break;
         }

         case OPE_SplineTo:
         {
				pElement += 4;
            break;
         }

         case OPE_BezierTo:
         {
				pElement += 6;
            break;
         }

         default:
         {
            ASSERT(FALSE);
            return FALSE;
         }
      }
   }

	return FALSE;
}

/*
// Compute the point B moved "in" by distance nDistance.
// The points A and C are on either side of B.
// Returns TRUE if mitered points are valid; otherwise, FALSE.
*/

static void near ComputeInflatedPoint(const CFixedPoint& Point,
                                 CFixed lThetaAB, CFixed lThetaCB,
                                 CFixed lDistance,
                                 CFixedPoint* pNewPoint)
{
   CFixed lTheta = MidPointFixed(lThetaAB, lThetaCB);

   CFixed lSine;
   if (lTheta == lThetaAB || (lSine = Sine(lTheta-lThetaAB)) == MakeFixed(0))
   {
      *pNewPoint = Point;
      return;
   }

   ASSERT(lSine != MakeFixed(0));
   CFixed lQ = SafeDivFixed(lDistance, lSine);

/* Compute the actual point. */

   pNewPoint->x = Point.x + MulFixed(lQ, Cosine(lTheta));
   pNewPoint->y = Point.y + MulFixed(lQ, Sine(lTheta));
}

/*
// Inflate a path by the given amount.
// If lAmount is negative, the path is "un-inflated" ("deflated"?).
*/

void COutlinePath::Inflate(CFixed lAmount, COutlinePath* pDestination /*=NULL*/)
{
   BOOL fInPlace = FALSE;

   if (pDestination == NULL)
   {
      pDestination = new COutlinePath;
      fInPlace = TRUE;
   }

#if 0
   CFixedRect StartRect;

   if (lAmount < 0)
   {
      QuickBoundingBox(&StartRect);
   }
#endif

   if (pDestination != NULL)
   {
//    Flatten();

//    DWORD dwStartTime = timeGetTime();

      COutlinePathIterator spit(this);

      spit.FirstPath();

   /* Begin iterating paths. */

      int i = 0;
      static int nSubpath = -1;

      if (IsWarped())
      {
         CWarpSubpathInflator Inflator;

         while (!spit.AtEnd())
         {
            if (i == nSubpath || nSubpath == -1)
            {
               Inflator.Inflate(&spit, pDestination, lAmount, m_lMiterLimit);
            }
            spit.NextPath();
            i++;
         }
      }
      else
      {
         CSubpathInflator Inflator;

         while (!spit.AtEnd())
         {
            if (i == nSubpath || nSubpath == -1)
            {
               Inflator.Inflate(&spit, pDestination, lAmount, m_lMiterLimit);
            }
            spit.NextPath();
            i++;
         }
      }

      pDestination->End();

//    DWORD dwEndTime = timeGetTime();
//    od("Time to inflate: %lu\r\n",
//       dwEndTime - dwStartTime);

      // Polyline and Warp flags are copied over.
      pDestination->UsePolyline(UsePolyline());
      pDestination->IsWarped(IsWarped());

      if (fInPlace)
      {
         Assign(pDestination);
         delete pDestination;
      }
   }
}

/*
// Reverse a path. This changes the orientation of the path by traversing
// it in the opposite direction.
*/

void COutlinePath::Reverse(COutlinePath* pDestination /*=NULL*/)
{
   BOOL fInPlace = FALSE;

   if (pDestination == NULL)
   {
      pDestination = new COutlinePath;
      fInPlace = TRUE;
   }

   if (pDestination != NULL)
   {
      DWORD dwElementCount = GetElementCount();

      pDestination->Allocate(dwElementCount);
      pDestination->SetElementCount(dwElementCount);

      long* pDestElement = ((long*)pDestination->GetElement(0)) + dwElementCount;

   /* Add the end at the end of this path data. */

      *--pDestElement = OPE_End;

   /* Now traverse paths in the source path data until done. */

      COutlinePathIterator it(this);

      it.FirstPath();            // Move to the first path.

      while (!it.AtEnd())
      {
         it.ToEOP();

         if (it.AtClose())
         {
         /* We need to close this guy */
            *--pDestElement = OPE_Close;
         }

         it.ToBOP();

         BOOL fCurrent = FALSE;
         CFixedPoint CP;

         while (!it.AtEOP())
         {
            switch (*(it.CurrentComponent()))
            {
               case OPE_MoveTo:
               {
                  ASSERT(!fCurrent);

                  fCurrent = TRUE;
                  CP = *(it.CurrentPoint());
                  break;
               }

               case OPE_LineTo:
               {
                  ASSERT(fCurrent);

               /* Add the LineTo into the array. */

                  *--pDestElement = CP.y;
                  *--pDestElement = CP.x;

                  *--pDestElement = OPE_LineTo;

                  CP = *(it.CurrentPoint());

                  break;
               }

               case OPE_SplineTo:
               {
                  ASSERT(fCurrent);

               /* Add the SplineTo into the array. */

                  *--pDestElement = CP.y;
                  *--pDestElement = CP.x;

                  CFixedPoint* pP = it.CurrentPoint();
                  *--pDestElement = pP->y;
                  *--pDestElement = pP->x;

                  *--pDestElement = OPE_SplineTo;

                  CP = *(it.NextPoint());
                  break;
               }

               case OPE_BezierTo:
               {
                  ASSERT(fCurrent);

               /* Add the BezierTo into the array. */

                  *--pDestElement = CP.y;
                  *--pDestElement = CP.x;

                  CFixedPoint* pP = it.CurrentPoint();
                  *--pDestElement = pP->y;
                  *--pDestElement = pP->x;

                  pP = it.NextPoint();
                  *--pDestElement = pP->y;
                  *--pDestElement = pP->x;

                  *--pDestElement = OPE_BezierTo;

                  CP = *(it.NextPoint());
                  break;
               }
               default:
               {
                  break;
               }
            }
            it.NextComponent();
         }

         if (fCurrent)
         {
         /* Insert the initial MoveTo. */

            *--pDestElement = CP.y;
            *--pDestElement = CP.x;

            *--pDestElement = OPE_MoveTo;
         }

         it.NextPath();
      }

      // Polyline and Warp flags are copied over.
      pDestination->UsePolyline(UsePolyline());
      pDestination->IsWarped(IsWarped());

      ASSERT(pDestElement == (long*)pDestination->GetElement(0));

      if (fInPlace)
      {
         Assign(pDestination);
      }
   }

   if (fInPlace)
   {
      if (pDestination != NULL)
      {
         delete pDestination;
         pDestination = NULL;
      }
   }
}

static CFixed Distance(CFixed dx, CFixed dy)
{
   if (dx < 0)
   {
      dx = -dx;
   }
   if (dy < 0)
   {
      dy = -dy;
   }

   CFixed lMin;

   if (dx < dy)
   {
      lMin = dx;
   }
   else
   {
      lMin = dy;
   }

   return dx + dy - (lMin >> 1);
}

static void near GetBezierBBox(const CFixedPoint* pPoints, CFixedRect& BBox)
{
   CFixed MinX, MaxX;
   CFixed MinY, MaxY;

   MaxX = MinX = pPoints[0].x;
   MaxY = MinY = pPoints[0].y;

   CFixed c;

   if ((c = pPoints[1].x) < MinX)
   {
      MinX = c;
   }
   else
   {
      MaxX = c;
   }

   if ((c = pPoints[1].y) < MinY)
   {
      MinY = c;
   }
   else
   {
      MaxY = c;
   }

   if ((c = pPoints[2].x) < MinX)
   {
      MinX = c;
   }
   else if (c > MaxX)
   {
      MaxX = c;
   }

   if ((c = pPoints[2].y) < MinY)
   {
      MinY = c;
   }
   else if (c > MaxY)
   {
      MaxY = c;
   }

   if ((c = pPoints[3].x) < MinX)
   {
      MinX = c;
   }
   else if (c > MaxX)
   {
      MaxX = c;
   }

   if ((c = pPoints[3].y) < MinY)
   {
      MinY = c;
   }
   else if (c > MaxY)
   {
      MaxY = c;
   }

   BBox.Left = MinX;
   BBox.Top = MinY;
   BBox.Right = MaxX;
   BBox.Bottom = MaxY;
}

static BOOL near NotTooSmall(CFixedPoint* pPoints)
{
   CFixedRect BBox;

   GetBezierBBox(pPoints, BBox);

   if (BBox.Width() < MakeFixed(3) && BBox.Height() < MakeFixed(3))
   {
   /* Linearize it. */
      pPoints[1].x = pPoints[2].x = MidPointFixed(pPoints[0].x, pPoints[3].x);
      pPoints[1].y = pPoints[2].y = MidPointFixed(pPoints[0].y, pPoints[3].y);
      return TRUE;      // Changed!
   }
   else
   {
      CFixed lDx = pPoints[2].x - pPoints[1].x;
      CFixed lDy = pPoints[2].y - pPoints[1].y;
      if (lDx < 0)
      {
         lDx = -lDx;
      }
      if (lDy < 0)
      {
         lDy = -lDy;
      }     
      if (lDx < 0x8000 && lDy < 0x8000)
      {
      /* Collapse the control points. */
         pPoints[1].x = pPoints[2].x = MidPointFixed(pPoints[1].x, pPoints[2].x);
         pPoints[1].y = pPoints[2].y = MidPointFixed(pPoints[1].y, pPoints[2].y);
         return TRUE;      // Changed!
      }
   }
   return FALSE;
}

static BOOL fWantAcute = TRUE;
static BOOL fDoAcuteCheck = TRUE;

static BOOL near CurveNeedsSplitting(CCurveElement* pElement, CFixed lAmount)
{
   if (!fWantAcute)
   {
      return FALSE;
   }

   CFixedRect BBox;
   GetBezierBBox(&pElement->m_A, BBox);

   if (BBox.Width() < MakeFixed(1) && BBox.Height() < MakeFixed(1))
   {
      return FALSE;
   }

/*
// Check the second derivatives to see if this is an S-curve.
*/

   CFixed x0 = pElement->m_A.x - (pElement->m_B.x << 1) + pElement->m_C.x;
   CFixed y0 = pElement->m_A.y - (pElement->m_B.y << 1) + pElement->m_C.y;
   CFixed x1 = pElement->m_B.x - (pElement->m_C.x << 1) + pElement->m_D.x;
   CFixed y1 = pElement->m_B.y - (pElement->m_C.y << 1) + pElement->m_D.y;

   if (x0 != 0 && x1 != 0 && y0 != 0 && y1 != 0)
   {
      if (((x0 ^ x1) < 0) && ((y0 ^ y1) < 0))
      {
      /* Second derivative components differ in sign somehow. */
//       TRACE0("S-curve!\r\n");
         return TRUE;
      }
   }

/*
// Check angles to see if this acute.
*/

   if (fDoAcuteCheck)
   {
      CFixed lThetaA = pElement->m_lThetaA;
      CFixed lThetaD = pElement->m_lThetaD;

      CFixed lDelta = lThetaA - lThetaD;

      while (lDelta < 0)
      {
         lDelta += MakeFixed(360);
      }
      while (lDelta >= MakeFixed(360))
      {
         lDelta -= MakeFixed(360);
      }

      if ((lDelta < MakeFixed(120) && lAmount < 0)
          || (lDelta > MakeFixed(240) && lAmount > 0))
      {
//       TRACE3("Acute: %08lx (%08lx to %08lx)\r\n",
//             lDelta, pElement->m_lThetaA, pElement->m_lThetaD);
         return TRUE;
      }
   }
   return FALSE;
}

static void near SplitCurveAt(const CFixedPoint* pCurve, CFixed lTime,
                         CFixedPoint* pNewLeft,
                         CFixedPoint* pNewRight)
{
   CFixedPoint A = pCurve[0];
   CFixedPoint B = pCurve[1];
   CFixedPoint C = pCurve[2];
   CFixedPoint D = pCurve[3];

   pNewLeft[0] = A;
   pNewRight[3] = D;

   pNewLeft[1].x = A.x + MulFixed(B.x - A.x, lTime);
   pNewLeft[1].y = A.y + MulFixed(B.y - A.y, lTime);

   pNewRight[2].x = C.x + MulFixed(D.x - C.x, lTime);
   pNewRight[2].y = C.y + MulFixed(D.y - C.y, lTime);

   CFixedPoint MBC;

   MBC.x = B.x + MulFixed(C.x - B.x, lTime);
   MBC.y = B.y + MulFixed(C.y - B.y, lTime);

   pNewLeft[2].x = pNewLeft[1].x + MulFixed(MBC.x - pNewLeft[1].x, lTime);
   pNewLeft[2].y = pNewLeft[1].y + MulFixed(MBC.y - pNewLeft[1].y, lTime);

   pNewRight[1].x = MBC.x + MulFixed(pNewRight[2].x - MBC.x, lTime);
   pNewRight[1].y = MBC.y + MulFixed(pNewRight[2].y - MBC.y, lTime);

   pNewLeft[3].x = pNewRight[0].x = pNewLeft[2].x + MulFixed(pNewRight[1].x - pNewLeft[2].x, lTime);
   pNewLeft[3].y = pNewRight[0].y = pNewLeft[2].y + MulFixed(pNewRight[1].y - pNewLeft[2].y, lTime);

   /* But... */
   NotTooSmall(pNewLeft);
   /* ...and... */
   NotTooSmall(pNewRight);
}

/*
// Append another path onto this one.
*/

void COutlinePath::Append(const COutlinePath* pOtherPath)
{
   DWORD dwCount = GetElementCount();

   if (*(long*)GetElement(dwCount-1) == OPE_End)
   {
   /* Lop off the End at the end. */
      SetElementCount(dwCount-1);
   }

/*
// We access the members directly because the access functions don't like
// 'const' objects.
*/
   AddElement(pOtherPath->m_pArray, pOtherPath->m_dwCount);
}


void COutlinePath::Flatten(COutlinePath* pDestination /*=NULL*/)
{
   BOOL fInPlace = FALSE;

   if (pDestination == NULL)
   {
      pDestination = new COutlinePath;
      fInPlace = TRUE;
   }

   if (pDestination != NULL)
   {
      CFixedPoint StartPoint;
      CFixedPoint CurrentPoint;
      
      long* pElement = (long*)GetElement(0);

      while (pElement != NULL)
      {
         switch (*(pElement++))
         {
            case OPE_End:
            {
               pDestination->End();
               pElement = NULL;
               break;
            }

            case OPE_Close:
            {
               pDestination->Close();
               CurrentPoint.x = StartPoint.x;
               CurrentPoint.y = StartPoint.y;
               break;
            }

            case OPE_MoveTo:
            {
               long AX = *(pElement++);
               long AY = *(pElement++);
               pDestination->MoveTo(AX, AY);
               CurrentPoint.x = AX;
               CurrentPoint.y = AY;
               StartPoint.x = AX;
               StartPoint.y = AY;
               break;
            }

            case OPE_LineTo:
            {
               long AX = *(pElement++);
               long AY = *(pElement++);
               pDestination->LineTo(AX, AY);
               CurrentPoint.x = AX;
               CurrentPoint.y = AY;
               break;
            }

            case OPE_SplineTo:
            {
               long AX = *(pElement++);
               long AY = *(pElement++);
               long BX = *(pElement++);
               long BY = *(pElement++);
               FlattenSpline(pDestination, CurrentPoint.x, CurrentPoint.y, AX, AY, BX, BY, -1);
               CurrentPoint.x = BX;
               CurrentPoint.y = BY;
               break;
            }

            case OPE_BezierTo:
            {
               long AX = *(pElement++);
               long AY = *(pElement++);
               long BX = *(pElement++);
               long BY = *(pElement++);
               long CX = *(pElement++);
               long CY = *(pElement++);
               FlattenBezier(pDestination, CurrentPoint.x, CurrentPoint.y, AX, AY, BX, BY, CX, CY);
               CurrentPoint.x = CX;
               CurrentPoint.y = CY;
               break;
            }

            default:
            {
               ASSERT(FALSE);
               pDestination->Free();
               pElement = NULL;
               break;
            }
         }
      }
      
      if (fInPlace)
      {
         Assign(pDestination);
      }
   }

   if (fInPlace)
   {
      if (pDestination != NULL)
      {
         delete pDestination;
         pDestination = NULL;
      }
   }
}

void COutlinePath::FlattenSpline(COutlinePath* pDestination, long AX, long AY, long BX, long BY, long CX, long CY, long GY)
{
   long GX, DX, DY, DDX, DDY;

   // On the first call (GY < 0), we need to convert the 16.16 numbers coming in to the 26.6
   // format used internally.

   if (GY < 0)
   {
      AX >>= 10;
      AY >>= 10;
      BX >>= 10;
      BY >>= 10;
      CX >>= 10;
      CY >>= 10;
   }

   // Start calculating the first and 2nd order differences
   
   DDX = (DX = (AX-BX))-BX+CX;   // = alpha-x = half of ddx, DX = AX - BX
   DDY = (DY = (AY-BY))-BY+CY;   // = alpha-y = half of ddx, DY = AY - BY
   
   // The calculation is not finished but these intermediate results are useful

   if (GY < 0)
   {
      // calculate amount of steps necessary = 1 << GY
      // calculate the error, GX and GY used a temporaries
      
      GX  = DDX < 0 ? -DDX : DDX;
      GY  = DDY < 0 ? -DDY : DDY;
      
      // Approximate GX = sqrt (ddx * ddx + ddy * ddy) = Euclididan distance, DDX = ddx/2 here
      
      GX += GX > GY ? GX+GY : GY+GY;   // GX = 2*distance = error = GX/8

      // Error = GX/8, but since GY = 1 below, error = GX/8/4 = GX >> 5, => GX = error << 5

      for (GY = 1; GX > (PIXELSIZE << (5-ERRSHIFT)); GX >>= 2) // GX = GX >> 2
      {
         GY++;    // GY used for temporary purposes
      }
      
      // Now GY contains the number of subdivisions necessary, number of vectors == (1 << GY)
      
      if (GY > MAXMAXGY) 
      {
         GY = MAXMAXGY;    // out of range => Set to maximum possible
      }
   }

   if (GY > MAXGY) 
   {
      // Subdivide, this is numerically stable
      
      long MIDX = ((long)AX+BX+BX+CX+2) >> 2;
      long MIDY = ((long)AY+BY+BY+CY+2) >> 2;
      
      FlattenSpline(pDestination,   AX,   AY, MidPointFixed(AX, BX+1), MidPointFixed(AY, BY+1), MIDX, MIDY, GY-1);
      FlattenSpline(pDestination, MIDX, MIDY, MidPointFixed(CX, BX+1), MidPointFixed(CY, BY+1),   CX,   CY, GY-1);
   }
   else
   {
      unsigned int nDivisions = 1 << GY;
   
      long lNSquaredShift = GY+GY;        // GY = n shift, lNSquaredShift = n*n shift

      // Finish calculations of 1st and 2nd order differences

      GY++;
            
      DX   = DDX-(DX << GY);     // alpha + beta * n
      DDX += DDX;
      DY   = DDY-(DY << GY);
      DDY += DDY;

      GX = (long) AX << lNSquaredShift;         // AX * (n*n)
      GY = (long) AY << lNSquaredShift;         // AY * (n*n)
      
      // GX and GY used for real now

      // OK, now we have the 1st and 2nd order differences,
      // so we go ahead and do the forward differencing loop.
      
      long lRound = 1L << (lNSquaredShift-1);
      
      do
      {
         GX += DX;   // add first order difference to x coordinate
         DX += DDX;  // add 2nd order difference to first order difference
         
         GY += DY;   // do the same thing for y
         DY += DDY;

         pDestination->LineTo(
            (((GX + lRound) >> lNSquaredShift) << 10),
            (((GY + lRound) >> lNSquaredShift) << 10));
      } while (--nDivisions);
   }
}

void COutlinePath::FlattenBezier(COutlinePath* pDestination, long AX, long AY, long BX, long BY, long CX, long CY, long DX, long DY)
{
   long M0X, M0Y, M1X, M1Y, M2X, M2Y;
   
   // Check for termination condition: Points A, B, C, and D are
   // colinear to within 1 pixel. This uses appoximate distance
   // algorithm from Graphics Gems II, pp. 10-13.
   
   M0X = DX-AX;
   M0Y = DY-AY;
   
   M1X = (M0X >= 0) ? M0X : -M0X;
   M1Y = (M0Y >= 0) ? M0Y : -M0Y;
   
   M2X = (M1X+M1Y+(((M1X < M1Y) ? M1X : M1Y)+1)/2);
   
   M2Y = MulFixed(BY-AY, M0X)-MulFixed(BX-AX, M0Y);
   if (M2Y < 0) M2Y = -M2Y;
   
   if (M2X > M2Y)
   {
      M2Y = MulFixed(CY-AY, M0X)-MulFixed(CX-AX, M0Y);
      if (M2Y < 0) M2Y = -M2Y;
      
      if (M2X > M2Y)
      {
         pDestination->LineTo(DX, DY);
         return;
      }
   }
   
   M0X = (AX+BX+1)/2;
   M0Y = (AY+BY+1)/2;
   
   M1X = (BX+CX+1)/2;
   M1Y = (BY+CY+1)/2;
   
   M2X = (CX+DX+1)/2;
   M2Y = (CY+DY+1)/2;
   
   // Reuse BX, BY
   
   BX = (M0X+M1X+1)/2;
   BY = (M0Y+M1Y+1)/2;
   
   // Reuse CX, CY
   
   CX = (M1X+M2X+1)/2;
   CY = (M1Y+M2Y+1)/2;
   
   // reuse M1X and M1Y
   
   M1X = (BX+CX+1)/2;
   M1Y = (BY+CY+1)/2;
   
   FlattenBezier(pDestination, AX, AY, M0X, M0Y, BX, BY, M1X, M1Y);
   FlattenBezier(pDestination, M1X, M1Y, CX, CY, M2X, M2Y, DX, DY);
}

void COutlinePath::BoundingBox(CFixedRect* pRect)
{
   COutlinePath FlatPath;
   Flatten(&FlatPath);
   FlatPath.QuickBoundingBox(pRect);
}

void COutlinePath::QuickBoundingBox(CFixedRect* pRect)
{
   BOOL fFirstPoint = TRUE;
   
   pRect->Left = 0;
   pRect->Top = 0;
   pRect->Right = 0;
   pRect->Bottom = 0;
   
   long* pElement = (long*)GetElement(0);

   while (pElement != NULL)
   {
      int nPoints;
      
      switch (*(pElement++))
      {
         case OPE_End:
         {
            pElement = NULL;
            nPoints = 0;
            break;
         }

         case OPE_Close:
         {
            nPoints = 0;
            break;
         }

         case OPE_MoveTo:
         case OPE_LineTo:
         {
            nPoints = 1;
            break;
         }
         
         case OPE_SplineTo:
         {
            nPoints = 2;
            break;
         }

         case OPE_BezierTo:
         {
            nPoints = 3;
            break;
         }

         default:
         {
            ASSERT(FALSE);
            pElement = NULL;
            nPoints = 0;
            break;
         }
      }
      
      while (nPoints-- > 0)
      {
         long X = *(pElement++);
         long Y = *(pElement++);
         
         if (fFirstPoint)
         {
            pRect->Left = pRect->Right = X;
            pRect->Top = pRect->Bottom = Y;
            fFirstPoint = FALSE;
         }
         else
         {
            if (X < pRect->Left)    pRect->Left = X;
            if (X > pRect->Right)   pRect->Right = X;
            if (Y < pRect->Top)     pRect->Top = Y;
            if (Y > pRect->Bottom)  pRect->Bottom = Y;
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// CCurveIntersector

CCurveIntersector::CCurveIntersector() : Array(sizeof(CCurveIntersection))
{
   m_pElements = NULL;
   m_pElements = new CCurveIntersectElement[MAX_ELEMENTS];
   m_nElementIndex = 0;
}

CCurveIntersector::~CCurveIntersector()
{
   delete [] m_pElements;
}

// Helper routines.

/* Allocate two elements from the curve element stack. */
CCurveIntersectElement* near CCurveIntersector::AllocateElements(int nCount /*=2*/)
{
   ASSERT(m_pElements != NULL);
   ASSERT(m_nElementIndex + nCount <= MAX_ELEMENTS);

   CCurveIntersectElement* pElement = m_pElements + m_nElementIndex;
   m_nElementIndex += nCount;
   return pElement;
}

void near CCurveIntersector::CurveBBox(CCurveIntersectElement* pCurve)
{
   GetBezierBBox(pCurve->m_Points, pCurve->m_BBox);
}

BOOL near CCurveIntersector::BBoxesOverlap(const CFixedRect& BBoxA, const CFixedRect& BBoxB)
{
   return   BBoxA.Right >= BBoxB.Left
         && BBoxB.Right >= BBoxA.Left
         && BBoxA.Bottom >= BBoxB.Top
         && BBoxB.Bottom >= BBoxA.Top;
}

BOOL near CCurveIntersector::CurvesOverlap(const CCurveIntersectElement* pCurveA, const CCurveIntersectElement* pCurveB)
{
   return BBoxesOverlap(pCurveA->m_BBox, pCurveB->m_BBox);
}

void near CCurveIntersector::SetCurveFlat(CCurveIntersectElement* pCurve)
{
   pCurve->m_fIsFlat = FALSE;

   if (pCurve->m_t0 == pCurve->m_t1)
   {
      pCurve->m_fIsFlat = TRUE;
   }
   else if (pCurve->m_BBox.Width() < MakeFixed(2)
                && pCurve->m_BBox.Height() < MakeFixed(2))
   {
   /* Within box of two pixels. Check for basic linearity. */

#if 0
   /*
   // Check first derivatives to make sure they go in the same basic
   // direction. This is not perfect but may help.
   */

      CFixed x0 = pCurve->m_Points[1].x - pCurve->m_Points[0].x;
      CFixed y0 = pCurve->m_Points[1].y - pCurve->m_Points[0].y;
      CFixed x1 = pCurve->m_Points[3].x - pCurve->m_Points[2].x;
      CFixed y1 = pCurve->m_Points[3].y - pCurve->m_Points[2].y;

      if ((x0 == 0 || x1 == 0 || (x0 ^ x1) >= 0)
            && (y0 == 0 || y1 == 0 || (y0 ^ y1) >= 0))
#endif
      {
      /*
      // We may also want to check second derivatives to make sure we
      // don't have an s-curve.
      */
         pCurve->m_fIsFlat = TRUE;
      }
   }
}

/*
// Split a curve.
*/

void near CCurveIntersector::SplitCurve(const CCurveIntersectElement* pCurve,
                                     CCurveIntersectElement* pCurveLeft,
                                     CCurveIntersectElement* pCurveRight)
{
   CFixedPoint A = pCurve->m_Points[0];
   CFixedPoint B = pCurve->m_Points[1];
   CFixedPoint C = pCurve->m_Points[2];
   CFixedPoint D = pCurve->m_Points[3];

   CFixedPoint MAB, MBC, MCD, MABC, MBCD, M;

   MAB.x = MidPointFixed(A.x, B.x);
   MAB.y = MidPointFixed(A.y, B.y);
   MBC.x = MidPointFixed(B.x, C.x);
   MBC.y = MidPointFixed(B.y, C.y);
   MCD.x = MidPointFixed(C.x, D.x);
   MCD.y = MidPointFixed(C.y, D.y);

   MABC.x = MidPointFixed(MAB.x, MBC.x);
   MABC.y = MidPointFixed(MAB.y, MBC.y);

   MBCD.x = MidPointFixed(MBC.x, MCD.x);
   MBCD.y = MidPointFixed(MBC.y, MCD.y);

   M.x = MidPointFixed(MABC.x, MBCD.x);
   M.y = MidPointFixed(MABC.y, MBCD.y);

   pCurveLeft->m_Points[0]  = A;
   pCurveLeft->m_Points[1]  = MAB;
   pCurveLeft->m_Points[2]  = MABC;
   pCurveLeft->m_Points[3]  = M;

   pCurveRight->m_Points[0] = M;
   pCurveRight->m_Points[1] = MBCD;
   pCurveRight->m_Points[2] = MCD;
   pCurveRight->m_Points[3] = D;

/* Compute the new bounding boxes. */

   CurveBBox(pCurveLeft);
   CurveBBox(pCurveRight);

/*
// Compute the new times.
// Note the rounding for the middle times to allow t0 and t1 to always converge.
*/

   pCurveLeft->m_t0 = pCurve->m_t0;
   CFixed lSum = pCurve->m_t0 + pCurve->m_t1;
   pCurveLeft->m_t1 = lSum >> 1;             // Round down for this guy.
   pCurveRight->m_t0 = (lSum+1) >> 1;        // Round up for this guy.
   pCurveRight->m_t1 = pCurve->m_t1;

/* Compute the new "flatness". */

   SetCurveFlat(pCurveLeft);
   SetCurveFlat(pCurveRight);
}

/*
// Recursively intersect curves.
*/

void near CCurveIntersector::RecursivelyIntersect(const CCurveIntersectElement* pCurveA,
                                                  const CCurveIntersectElement* pCurveB)
{
   int nElementSave = m_nElementIndex;

   CCurveIntersectElement* pCurveALeft;
   CCurveIntersectElement* pCurveARight;
   CCurveIntersectElement* pCurveBLeft;
   CCurveIntersectElement* pCurveBRight;

/*
// Split curve A if it is not already flat.
*/

   if (!pCurveA->m_fIsFlat)
   {
      pCurveALeft = AllocateElements();
      pCurveARight = pCurveALeft+1;

      SplitCurve(pCurveA, pCurveALeft, pCurveARight);
   }

/*
// Split curve B if it is not already flat.
*/

   if (!pCurveB->m_fIsFlat)
   {
      pCurveBLeft = AllocateElements();
      pCurveBRight = pCurveBLeft+1;

      SplitCurve(pCurveB, pCurveBLeft, pCurveBRight);
   }

/*
// Now we can handle the intersections (if any).
*/

   if (pCurveA->m_fIsFlat)
   {
      if (pCurveB->m_fIsFlat)
      {
      /*
      // A and B are both flat. This is the terminating condition.
      // We do line intersection of the line connecting points A and C of
      // both curves.
      */
         CCurveIntersection Int;

      /* Special case the endpoint intersections. */

         BOOL fAdded = FALSE;

         if (pCurveA->m_Points[0] == pCurveB->m_Points[3])
         {
         /* Connected his end to my start. */
            Int.m_tA = pCurveA->m_t0;
            Int.m_tB = pCurveB->m_t1;
            add_element(&Int);
            fAdded = TRUE;
         }

         if (pCurveA->m_Points[3] == pCurveB->m_Points[0])
         {
         /* Connected my end to his start. */
            Int.m_tA = pCurveA->m_t1;
            Int.m_tB = pCurveB->m_t0;
            add_element(&Int);
            fAdded = TRUE;
         }

         if (!fAdded)
         {
         /*
         // We switch to doubles since the numbers can overflow.
         // Note that we do not normalize. Everything divides out later.
         */

            double Adx =  double(pCurveA->m_Points[3].x - pCurveA->m_Points[0].x);
            double Ady =  double(pCurveA->m_Points[3].y - pCurveA->m_Points[0].y);
            double Bdx =  double(pCurveB->m_Points[3].x - pCurveB->m_Points[0].x);
            double Bdy =  double(pCurveB->m_Points[3].y - pCurveB->m_Points[0].y);
            double ABdx = double(pCurveB->m_Points[0].x - pCurveA->m_Points[0].x);
            double ABdy = double(pCurveB->m_Points[0].y - pCurveA->m_Points[0].y);

            double det = Bdx*Ady - Bdy*Adx;

            if (det != 0.0)
            {
					double d0 = (Bdx*ABdy - Bdy*ABdx)/det;
					double d1 = (Adx*ABdy - Ady*ABdx)/det;

					// We're going to be converting to fixed point numbers, so
					// clip to a reasonable range.
					if (d0 < -32000.0)	d0 = -32000.0;
					if (d0 > 32000.0)		d0 = 32000.0;
					if (d1 < -32000.0)	d1 = -32000.0;
					if (d1 > 32000.0)		d1 = 32000.0;

               CFixed s = MakeFixed(d0);
               CFixed t = MakeFixed(d1);

               if (s >= MakeFixed(0)
                && s <= MakeFixed(1)
                && t >= MakeFixed(0)
                && t <= MakeFixed(1))
               {
                  Int.m_tA = pCurveA->m_t0 + MulFixed(s, pCurveA->m_t1 - pCurveA->m_t0);
                  Int.m_tB = pCurveB->m_t0 + MulFixed(t, pCurveB->m_t1 - pCurveB->m_t0);

                  add_element(&Int);
               }
            }
         }
      }
      else
      {
      /* Only A. Both sides of B. */

         if (CurvesOverlap(pCurveA, pCurveBLeft))
         {
            RecursivelyIntersect(pCurveA, pCurveBLeft);
         }
         if (CurvesOverlap(pCurveA, pCurveBRight))
         {
            RecursivelyIntersect(pCurveA, pCurveBRight);
         }
      }
   }
   else
   {
      if (pCurveB->m_fIsFlat)
      {
      /* Only B. Both sides of A. */
         if (CurvesOverlap(pCurveALeft, pCurveB))
         {
            RecursivelyIntersect(pCurveALeft, pCurveB);
         }
         if (CurvesOverlap(pCurveARight, pCurveB))
         {
            RecursivelyIntersect(pCurveARight, pCurveB);
         }
      }
      else
      {
      /* Both side of both A and B. */
         if (CurvesOverlap(pCurveALeft, pCurveBLeft))
         {
            RecursivelyIntersect(pCurveALeft, pCurveBLeft);
         }
         if (CurvesOverlap(pCurveALeft, pCurveBRight))
         {
            RecursivelyIntersect(pCurveALeft, pCurveBRight);
         }
         if (CurvesOverlap(pCurveARight, pCurveBLeft))
         {
            RecursivelyIntersect(pCurveARight, pCurveBLeft);
         }
         if (CurvesOverlap(pCurveARight, pCurveBRight))
         {
            RecursivelyIntersect(pCurveARight, pCurveBRight);
         }
      }
   }

// Pop the elements we allocated.
   m_nElementIndex = nElementSave;
}

void CCurveIntersector::IntersectCurves(const CFixedPoint* pCurveA, const CFixedPoint* pCurveB)
{
   empty_array();

   CCurveIntersectElement ElementA;
   CCurveIntersectElement ElementB;

   memcpy(&ElementA.m_Points, pCurveA, sizeof(ElementA.m_Points));
   memcpy(&ElementB.m_Points, pCurveB, sizeof(ElementB.m_Points));

   CurveBBox(&ElementA);
   CurveBBox(&ElementB);

   if (CurvesOverlap(&ElementA, &ElementB))
   {
      CSubpathInflator::m_dwIntersectCalled++;

      ElementA.m_t0 =
         ElementB.m_t0 = MakeFixed(0);
      ElementA.m_t1 =
         ElementB.m_t1 = MakeFixed(1);

      SetCurveFlat(&ElementA);
      SetCurveFlat(&ElementB);

      RecursivelyIntersect(&ElementA, &ElementB);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CCurveElement

/////////////////////////////////////////////////////////////////////////////
// CCurveArray

CCurveArray::CCurveArray()
   : CDynamicArray(sizeof(CCurveElement), 32, 65520/sizeof(CCurveElement))
{
}

int CCurveArray::AddLine(CFixedPoint A, CFixedPoint B, int nNext /*=-1*/)
{
   CFixed lTheta = ArcTangent2(B.y - A.y, B.x - A.x);
   CFixedPoint Point;
   Point.x = MidPointFixed(A.x, B.x);
   Point.y = MidPointFixed(A.y, B.y);

   return AddBezier(
            A.x, A.y,
            Point.x, Point.y,
            Point.x, Point.y,
            B.x, B.y,
            nNext,
            lTheta - MakeFixed(180), lTheta);
}

int CCurveArray::AddSpline(CFixedPoint A, CFixedPoint B, CFixedPoint C)
{
   return AddBezier(
            A.x, A.y,
            OneThirdFixed(B.x, A.x), OneThirdFixed(B.y, A.y), 
            OneThirdFixed(B.x, C.x), OneThirdFixed(B.y, C.y), 
            C.x, C.y);
}

int CCurveArray::AddBezier(
   CFixed AX, CFixed AY,
   CFixed BX, CFixed BY,
   CFixed CX, CFixed CY,
   CFixed DX, CFixed DY,
   int nNext /*=-1*/,
   CFixed lThetaA /*=-1L*/,
   CFixed lThetaD /*=-1L*/)
{
   CCurveElement Element;

   Element.m_nNext = nNext;
   Element.m_A.x = AX;
   Element.m_A.y = AY;
   Element.m_B.x = BX;
   Element.m_B.y = BY;
   Element.m_C.x = CX;
   Element.m_C.y = CY;
   Element.m_D.x = DX;
   Element.m_D.y = DY;

   if (NotTooSmall(&Element.m_A))
   {
   /* Changed some points. Recompute the angles. */
      lThetaA = lThetaD = -1L;
   }

   if (lThetaA == -1L)
   {
      CFixed OtherX = Element.m_B.x, OtherY = Element.m_B.y;
      if (Element.m_A.x == OtherX && Element.m_A.y == OtherY)
      {
         OtherX = Element.m_C.x;
         OtherY = Element.m_C.y;
         if (Element.m_A.x == OtherX && Element.m_A.y == OtherY)
         {
            OtherX = Element.m_D.x;
            OtherY = Element.m_D.y;
         }
      }
      lThetaA = ArcTangent2(Element.m_A.y - OtherY, Element.m_A.x - OtherX);
   }

   if (lThetaD == -1L)
   {
      CFixed OtherX = Element.m_C.x, OtherY = Element.m_C.y;
      if (Element.m_D.x == OtherX && Element.m_D.y == OtherY)
      {
         OtherX = Element.m_B.x;
         OtherY = Element.m_B.y;
         if (Element.m_D.x == OtherX && Element.m_D.y == OtherY)
         {
            OtherX = Element.m_A.x;
            OtherY = Element.m_A.y;
         }
      }
      lThetaD = ArcTangent2(Element.m_D.y - OtherY, Element.m_D.x - OtherX);
   }

   Element.m_lThetaA = lThetaA;
   Element.m_lThetaD = lThetaD;

// TRACE("Add %d: %08lx,%08lx,%08lx,%08lx,%08lx,%08lx,%08lx,%08lx; %08lx,%08lx\r\n",
//       Curves(), AX, AY, BX, BY, CX, CY, DX, DY, lThetaA, lThetaD);

   AddElement(&Element);
   return Curves()-1;
}

/////////////////////////////////////////////////////////////////////////////
// CSubpathInflator

DWORD CSubpathInflator::m_dwIntersectionFound;
DWORD CSubpathInflator::m_dwIntersectCalled;
DWORD CSubpathInflator::m_dwTrivialIntersections;

CSubpathInflator::CSubpathInflator()
{
}

static DWORD dwFlattened;

void CSubpathInflator::Inflate(COutlinePathIterator* pSource, COutlinePath* pDestination, CFixed lAmount, CFixed lMiterLimit /*=0*/)
{
   m_Subpaths.RemoveAll();
   m_Curves.Free();

   m_dwIntersectionFound = 0;
   m_dwIntersectCalled = 0;
   m_dwTrivialIntersections = 0;
   dwFlattened = 0;
#ifdef _DEBUG_OUTPATH
   dwArcTangents = 0;
   dwArcTangentTime = 0;

   DWORD dwStartTime = 0;
   DWORD dwGetTime = 0;
   DWORD dwInflateTime = 0;
   DWORD dwConnectTime = 0;
   DWORD dwIntersectTime = 0;
   DWORD dwPutTime = 0;

   dwStartTime = timeGetTime();
#endif

   if (GetSubpath(pSource))
   {
#ifdef _DEBUG_OUTPATH
      dwGetTime = timeGetTime();
#endif
      InflateSubpath(lAmount);
#ifdef _DEBUG_OUTPATH
      dwInflateTime = timeGetTime();
#endif
      ConnectSubpath(lAmount, lMiterLimit);
#ifdef _DEBUG_OUTPATH
      dwConnectTime = timeGetTime();
#endif

      IntersectSubpath();
#ifdef _DEBUG_OUTPATH
      dwIntersectTime = timeGetTime();
#endif

      PutSubpath(pDestination);
#ifdef _DEBUG_OUTPATH
      dwPutTime = timeGetTime();
#endif
   }
// TRACE("Ints - calls: %ld, triv: %ld, nontriv: %ld, flat: %ld\r\n",
//       m_dwIntersectCalled,
//       m_dwTrivialIntersections,
//       m_dwIntersectionFound,
//       dwFlattened);
#ifdef _DEBUG_OUTPATH
   TRACE("get:%lu, inf:%lu, con:%lu, int:%lu, put:%lu\r\n",
         dwGetTime - dwStartTime,
         dwInflateTime - dwGetTime,
         dwConnectTime - dwInflateTime,
         dwIntersectTime - dwConnectTime,
         dwPutTime - dwIntersectTime);
   TRACE("ArcTangents: %lu, time: %lu\r\n", dwArcTangents, dwArcTangentTime);
#endif
}

BOOL CSubpathInflator::GetSubpath(COutlinePathIterator* pSource)
{
   BOOL fCurrent = FALSE;
   CFixedPoint cp;
   CFixedPoint FirstPoint;

   while (!pSource->AtEOP())
   {
      switch (*(pSource->CurrentComponent()))
      {
         case COutlinePath::OPE_MoveTo:
         {
            ASSERT(!fCurrent);

            fCurrent = TRUE;
            cp = *(pSource->CurrentPoint());
            FirstPoint = cp;
            break;
         }

         case COutlinePath::OPE_LineTo:
         {
            ASSERT(fCurrent);

         /* Add the LineTo into the array. */

            CFixedPoint p = *(pSource->CurrentPoint());

            if (p != cp)
            {
               m_Curves.AddLine(cp, p);
               cp = p;
            }

            break;
         }

         case COutlinePath::OPE_SplineTo:
         {
            ASSERT(fCurrent);

            CFixedPoint B = *(pSource->CurrentPoint());
            CFixedPoint C = *(pSource->NextPoint());

            /* See if the curve is almost linear already. */

            if (cp != B || B != C)
            {
               m_Curves.AddSpline(cp, B, C);
               cp = C;
            }

            break;
         }

         case COutlinePath::OPE_BezierTo:
         {
            ASSERT(fCurrent);

            CFixedPoint B = *(pSource->CurrentPoint());
            CFixedPoint C = *(pSource->NextPoint());
            CFixedPoint D = *(pSource->NextPoint());

            if (cp != B || B != C || C != D)
            {
               m_Curves.AddBezier(cp.x, cp.y, B.x, B.y, C.x, C.y, D.x, D.y);
               cp = D;
            }

            break;
         }

         default:
         {
            break;
         }
      }
      pSource->NextComponent();
   }

   if (fCurrent)
   {
      if (cp != FirstPoint)
      {
      /* Make sure we're closed. */
         m_Curves.AddLine(cp, FirstPoint);
      }
   /*
   // Now go through and set all the next pointers.
   */

      int nCurves = m_Curves.Curves();
      int nCurve;
      CCurveElement* pCurve;

      for (nCurve = 0; nCurve < nCurves-1; nCurve++)
      {
         pCurve = m_Curves.Curve(nCurve);
         pCurve->m_nNext = nCurve+1;
      }

      pCurve = m_Curves.Curve(nCurve);
      pCurve->m_nNext = 0;

      m_Subpaths.Add(0);
      m_nPathIsFilled = PathIsFilled(0);
//    TRACE("Original Filled is %d\n", m_nPathIsFilled);
      ASSERT(m_nPathIsFilled != 0);
   }
   return fCurrent;
}

void CSubpathInflator::InflateSubpath(CFixed lAmount)
{
/* Run through all curves and inflate them. */

   for (int nCurve = 0; nCurve < m_Curves.Curves(); )
   {
      CCurveElement *pElement = m_Curves.Curve(nCurve);

      BOOL fNeedsSplitting = CurveNeedsSplitting(pElement, lAmount);

      CFixedPoint A = pElement->m_A;
      CFixedPoint B = pElement->m_B;
      CFixedPoint C = pElement->m_C;
      CFixedPoint D = pElement->m_D;

      if (fNeedsSplitting)
      {
         dwFlattened++;
         CFixedPoint MAB, MBC, MCD, MABC, MBCD, M;

         MAB.x = MidPointFixed(A.x, B.x);
         MAB.y = MidPointFixed(A.y, B.y);
         MBC.x = MidPointFixed(B.x, C.x);
         MBC.y = MidPointFixed(B.y, C.y);
         MCD.x = MidPointFixed(C.x, D.x);
         MCD.y = MidPointFixed(C.y, D.y);

         MABC.x = MidPointFixed(MAB.x, MBC.x);
         MABC.y = MidPointFixed(MAB.y, MBC.y);

         MBCD.x = MidPointFixed(MBC.x, MCD.x);
         MBCD.y = MidPointFixed(MBC.y, MCD.y);

         M.x = MidPointFixed(MABC.x, MBCD.x);
         M.y = MidPointFixed(MABC.y, MBCD.y);

         pElement->m_B = MAB;
         pElement->m_C = MABC;
         pElement->m_D = M;

         CFixed lThetaD = pElement->m_lThetaD;
         pElement->m_lThetaD = ArcTangent2(MBCD.y - MABC.y, MBCD.x- MABC.x);

         int nNext = pElement->m_nNext;
         pElement->m_nNext = m_Curves.Curves();

         m_Curves.AddBezier(M.x, M.y,
                            MBCD.x, MBCD.y,
                            MCD.x, MCD.y,
                            D.x, D.y,
                            nNext,
                            pElement->m_lThetaD - MakeFixed(180),
                            lThetaD);
      }
      else
      {
         CFixed lThetaA = pElement->m_lThetaA;
         CFixed lThetaD = pElement->m_lThetaD;

         CFixed lThetaBC;
         CFixed lThetaCB;

         if (B == C)
         {
            lThetaBC = lThetaD;
            lThetaCB = lThetaA;
         }
         else
         {
            lThetaBC = ArcTangent2(C.y - B.y, C.x - B.x);
            lThetaCB = lThetaBC + MakeFixed(180);
         }

   //    TRACE("lThetaA = %08lx, lThetaD = %08lx\r\n", lThetaA, lThetaD);

      /*
      // Inflate A.
      */

         ComputeInflatedPoint(A,
                              lThetaA, lThetaA + MakeFixed(180),
                              lAmount,
                              &pElement->m_A);
      /*
      // Inflate B.
      */

         ComputeInflatedPoint(B,
                              lThetaA, lThetaBC,
                              lAmount,
                              &pElement->m_B);
      /*
      // Inflate C.
      */

         ComputeInflatedPoint(C,
                              lThetaCB, lThetaD,
                              lAmount,
                              &pElement->m_C);
      /*
      // Inflate D.
      */

         ComputeInflatedPoint(D,
                              lThetaD - MakeFixed(180), lThetaD,
                              lAmount,
                              &pElement->m_D);

         NotTooSmall(&pElement->m_A);
         nCurve++;
      }
   }
}

void CSubpathInflator::ConnectSubpath(CFixed lAmount /*=0*/, CFixed lMiterLimit /*=0*/)
{
   const int nFirstCurve = 0;

   int nCurve = nFirstCurve;

   do
   {
      CCurveElement* pElement = m_Curves.Curve(nCurve);
      int nNext = pElement->m_nNext;
      CCurveElement* pNextElement = m_Curves.Curve(nNext);

      if (pElement->m_D != pNextElement->m_A)
      {
         CFixed lThetaThis = pElement->m_lThetaD + MakeFixed(180);
         CFixed lThetaNext = pNextElement->m_lThetaA + MakeFixed(180);

         CFixed lDelta = lThetaNext-lThetaThis;
         while (lDelta < 0)
         {
            lDelta += MakeFixed(360);
         }
         while (lDelta >= MakeFixed(360))
         {
            lDelta -= MakeFixed(360);
         }
         CFixed lTheta = MidPointFixed(lThetaThis, lThetaNext);

         CFixed lTan = Tangent(lTheta-lThetaThis);

         if (lTan == 0
            || lTan == 0x7FFFFFFF
            || lTan == -0x7FFFFFFF
            || !((lDelta > MakeFixed(180) && (lAmount > 0))
                 || (lDelta < MakeFixed(180) && (lAmount < 0))))
         {
         /* Just join them. */
            int nNewCurve = m_Curves.AddLine(pElement->m_D, pNextElement->m_A, nNext);

            pElement = m_Curves.Curve(nCurve);
            pElement->m_nNext = nNewCurve;
         }
         else
         {
            ASSERT(lTan != 0x7FFFFFFF && lTan != -0x7FFFFFFF);
            CFixed lQ = SafeDivFixed(lAmount, lTan);

            CFixed lAbsQ = (lQ < 0) ? -lQ : lQ;
            CFixed lAbsAmount = (lAmount < 0) ? -lAmount : lAmount;
            CFixed lNewMiterLimit = MulFixed(lMiterLimit, lAbsAmount);
            CFixedPoint NextA = pNextElement->m_A;

            int nNewBase = m_Curves.Curves();

            if (lNewMiterLimit == 0 || lAbsQ <= lNewMiterLimit)
            {
            /* Miter them (join them in a point). */

               CFixedPoint JoinPoint;
               JoinPoint.x = pElement->m_D.x + MulFixed(lQ, Cosine(lThetaThis));
               JoinPoint.y = pElement->m_D.y + MulFixed(lQ, Sine(lThetaThis));

               pElement->m_nNext = nNewBase;
               m_Curves.AddLine(pElement->m_D, JoinPoint, nNewBase+1);
               m_Curves.AddLine(JoinPoint, NextA, nNext);
            }
            else
            {
            /* Exceeds miter limit. Chop off the miter. */
               lQ = (lQ < 0) ? -lNewMiterLimit : lNewMiterLimit;
               CFixedPoint NewA;
               CFixedPoint NewB;

               NewA.x = pElement->m_D.x + MulFixed(lQ, Cosine(lThetaThis));
               NewA.y = pElement->m_D.y + MulFixed(lQ, Sine(lThetaThis));

               NewB.x = pNextElement->m_A.x + MulFixed(lQ, Cosine(lThetaNext));
               NewB.y = pNextElement->m_A.y + MulFixed(lQ, Sine(lThetaNext));

               pElement->m_nNext = nNewBase;
               m_Curves.AddLine(pElement->m_D, NewA, nNewBase+1);
               m_Curves.AddLine(NewA, NewB, nNewBase+2);
               m_Curves.AddLine(NewB, NextA, nNext);
            }
         }
      }

      nCurve = nNext;
   } while (nCurve != nFirstCurve);
}

void CSubpathInflator::DumpSubpath(int nSubpath)
{
   TRACE("Path %d of %d\n", nSubpath, m_Subpaths.GetSize());
   int nFirstCurve = m_Subpaths.GetAt(nSubpath);
   int nCurve = nFirstCurve;
   int nAcross = 0;
   do
   {
      CCurveElement* pCurve = m_Curves.Curve(nCurve);
      TRACE("%04d ", nCurve);
      if (++nAcross == 12)
      {
         nAcross = 0;
         TRACE("\n");
      }
      nCurve = pCurve->m_nNext;
   } while (nCurve != nFirstCurve);

   if (nAcross != 0)
   {
      TRACE("\n");
   }
}

void CSubpathInflator::IntersectSubpath(void)
{
   CCurveIntersector Intersector;

   for (int nSubpath = 0; nSubpath < m_Subpaths.GetSize(); nSubpath++)
   {
      int nFirstCurve = m_Subpaths.GetAt(nSubpath);
      int nCurve = nFirstCurve;
      int nPrevCurve = -1;
      do
      {
         CCurveElement* pCurve = m_Curves.Curve(nCurve);
         int nNextCurve = pCurve->m_nNext;
         int nSearchCurve = nNextCurve;
         int nPrevSearch = nCurve;
         while (nSearchCurve != nFirstCurve)
         {
            CCurveElement* pSearchCurve = m_Curves.Curve(nSearchCurve);
            int nNextSearch = pSearchCurve->m_nNext;
            ASSERT(nSearchCurve != nCurve);

            Intersector.IntersectCurves(&pCurve->m_A, &pSearchCurve->m_A);
            int nIntersections = Intersector.Intersections();

            for (int nIntersection = 0; nIntersection < nIntersections; nIntersection++)
            {
               CCurveIntersection* pIntersection = Intersector.Intersection(nIntersection);
               int nConnected = 0;
               if (pIntersection->m_tA == MakeFixed(1)
                        && pIntersection->m_tB == MakeFixed(0))
               {
                  nConnected = 1;
               }
               else if (pIntersection->m_tA == MakeFixed(0)
                       && pIntersection->m_tB == MakeFixed(1))
               {
                  nConnected = -1;
               }

               if (!(nConnected == 1 && pCurve->m_nNext == nSearchCurve)
                     && !(nConnected == -1 && pSearchCurve->m_nNext == nCurve))
               {
                  m_dwIntersectionFound++;
                  if (nConnected)
                  {
                     TRACE1("nConnected == %d\r\n", nConnected);
                  }
               /*
               // We found an intersection.
               // Split the two curves.
               // pCurve is A, pNextCurve is B.
               */

                  if (pIntersection->m_tB == MakeFixed(0))
                  {
                     nSearchCurve = nPrevSearch;
                     pSearchCurve = m_Curves.Curve(nSearchCurve);
                     pIntersection->m_tB = MakeFixed(1);
                  }

                  int nThisA = nCurve;

                  if (pIntersection->m_tA == MakeFixed(0))
                  {
#if 0
                     if ((nThisA = nPrevCurve) == -1)
                     {
                        nThisA = PreviousCurveOf(nCurve);
                     }
                     pCurve = m_Curves.Curve(nThisA);
                     pIntersection->m_tA = MakeFixed(1);
#endif
                     TRACE0("m_tA == 0\r\n");
//                   pIntersection->m_tA++;
                  }
//                else
                  {
                     int nNextA = pCurve->m_nNext;
                     int nNextB = pSearchCurve->m_nNext;

                     int nLinkA, nLinkB;

                     CFixedPoint AIntersection;
                     CFixedPoint BIntersection;

                     if (pIntersection->m_tA == MakeFixed(1))
                     {
                     /* */
                        nLinkB = nNextA;
                        AIntersection = pCurve->m_D;
                     }
                     else
                     {
                        CFixedPoint NewLeftA[4];
                        CFixedPoint NewRightA[4];

                        SplitCurveAt(&pCurve->m_A, pIntersection->m_tA,
                                     NewLeftA, NewRightA);

                        nLinkB = m_Curves.AddBezier(NewRightA[0].x, NewRightA[0].y,
                                                    NewRightA[1].x, NewRightA[1].y,
                                                    NewRightA[2].x, NewRightA[2].y,
                                                    NewRightA[3].x, NewRightA[3].y,
                                                    nNextA);

                        pCurve = m_Curves.Curve(nThisA);
                        pSearchCurve = m_Curves.Curve(nSearchCurve);

                        memcpy(&pCurve->m_A, NewLeftA, sizeof(NewLeftA));
                        AIntersection = NewRightA[0];
                     }

                     if (pIntersection->m_tB == MakeFixed(1))
                     {
                     /* No need for intersection. */
                        nLinkA = nNextB;
                        BIntersection = pSearchCurve->m_D;
                     }
                     else
                     {
                        CFixedPoint NewLeftB[4];
                        CFixedPoint NewRightB[4];

                        SplitCurveAt(&pSearchCurve->m_A, pIntersection->m_tB,
                                     NewLeftB, NewRightB);

                        nLinkA = m_Curves.AddBezier(NewRightB[0].x, NewRightB[0].y,
                                                    NewRightB[1].x, NewRightB[1].y,
                                                    NewRightB[2].x, NewRightB[2].y,
                                                    NewRightB[3].x, NewRightB[3].y,
                                                    nNextB);

                        pCurve = m_Curves.Curve(nThisA);
                        pSearchCurve = m_Curves.Curve(nSearchCurve);

                        memcpy(&pSearchCurve->m_A, NewLeftB, sizeof(NewLeftB));
                        BIntersection = NewRightB[0];
                     }

                     pCurve->m_nNext = nLinkA;
                     pSearchCurve->m_nNext = nLinkB;

                     CCurveElement* pNextA = m_Curves.Curve(nLinkA);
                     CCurveElement* pNextB = m_Curves.Curve(nLinkB);

                     pCurve->m_D.x
                        = pNextA->m_A.x
                        = pSearchCurve->m_D.x
                        = pNextB->m_A.x
                        = MidPointFixed(AIntersection.x, BIntersection.x);

                     pCurve->m_D.y
                        = pNextA->m_A.y
                        = pSearchCurve->m_D.y
                        = pNextB->m_A.y
                        = MidPointFixed(AIntersection.y, BIntersection.y);

                     m_Subpaths.Add(nLinkB);

                     nNextSearch = nNextCurve = nLinkA;
                  }

                  break;
               }
               else
               {
                  m_dwTrivialIntersections++;
               }
            }
            nPrevSearch = nSearchCurve;
            nSearchCurve = nNextSearch;
         }
         nPrevCurve = nCurve;
         nCurve = nNextCurve;
      } while (nCurve != nFirstCurve);
   }
}

void CSubpathInflator::PutSubpath(COutlinePath* pDestination)
{
   int nSubpaths = m_Subpaths.GetSize();

   for (int nSubpath = 0; nSubpath < nSubpaths; nSubpath++)
   {
      int nFilled = PathIsFilled(nSubpath);
      if (nFilled == m_nPathIsFilled || nFilled == 0)
      {
         int nFirstCurve = (int)m_Subpaths.GetAt(nSubpath);

         BOOL fFirstPoint = TRUE;
         long* pFirstCurve = NULL;
         int nCurve = nFirstCurve;

         do
         {
            CCurveElement* pCurve = m_Curves.Curve(nCurve);

            if (fFirstPoint)
            {
               fFirstPoint = FALSE;
               pDestination->MoveTo(pCurve->m_A.x, pCurve->m_A.y);
            }

            pDestination->BezierTo(pCurve->m_B.x, pCurve->m_B.y,
                                   pCurve->m_C.x, pCurve->m_C.y,
                                   pCurve->m_D.x, pCurve->m_D.y);


            nCurve = pCurve->m_nNext;
         } while (nCurve != nFirstCurve);

         pDestination->Close();
      }
   }
}

void CSubpathInflator::GetExtremeComponents(
   int nSubpath,
   int& nMinXComponent, int& nMinXIndex,
   int& nMinYComponent, int& nMinYIndex,
   int& nMaxXComponent, int& nMaxXIndex,
   int& nMaxYComponent, int& nMaxYIndex)

{
   int nFirstCurve = (int)m_Subpaths.GetAt(nSubpath);

   int nCurve = nFirstCurve;
   nMinXComponent =
   nMinYComponent =
   nMaxXComponent =
   nMaxYComponent = -1;
   CFixed nMinX, nMinY, nMaxX, nMaxY;
   int nIndex = 0;
   CCurveElement* pCurve;

   do
   {
      if (nIndex == 0)
      {
         pCurve = m_Curves.Curve(nCurve);
      }

      CFixed nX = ((CFixedPoint*)&pCurve->m_A)[nIndex].x;
      CFixed nY = ((CFixedPoint*)&pCurve->m_A)[nIndex].y;

      if (nMinXComponent == -1)
      {
         nMinXComponent =
         nMinYComponent =
         nMaxXComponent =
         nMaxYComponent = nCurve;
         nMinX =
         nMaxX = nX;
         nMinY =
         nMaxY = nY;
         nMinXIndex =
         nMinYIndex =
         nMaxXIndex =
         nMaxYIndex = nIndex;
      }
      else
      {
         if (nX < nMinX)
         {
            nMinXComponent = nCurve;
            nMinX = nX;
            nMinXIndex = nIndex;
         }
         if (nY < nMinY)
         {
            nMinYComponent = nCurve;
            nMinY = nY;
            nMinYIndex = nIndex;
         }
         if (nX > nMaxX)
         {
            nMaxXComponent = nCurve;
            nMaxX = nX;
            nMaxXIndex = nIndex;
         }
         if (nY > nMaxY)
         {
            nMaxYComponent = nCurve;
            nMaxY = nY;
            nMaxYIndex = nIndex;
         }
      }

   /* Move to next point as needed. */

      if (++nIndex == 3)
      {
         nIndex = 0;
         nCurve = pCurve->m_nNext;
      }
   } while (nCurve != nFirstCurve || nIndex != 0);
}

void CSubpathInflator::PreviousPoint(int& nComponent, int& nIndex)
{
   if (nIndex == 0)
   {
   /* Find the previous component. */
      int nComponentToFind = nComponent;

      CCurveElement* pCurve = m_Curves.Curve(nComponent);
      while (pCurve->m_nNext != nComponentToFind)
      {
         nComponent = pCurve->m_nNext;
         pCurve = m_Curves.Curve(nComponent);
      }
      nIndex = 3;    // Index of last point in a curve.
   }
// And back off a point.
   nIndex--;
}

void CSubpathInflator::NextPoint(int& nComponent, int& nIndex)
{
   if (++nIndex == 3)
   {
   /* Move to the next curve. */
      CCurveElement* pCurve = m_Curves.Curve(nComponent);
      nComponent = pCurve->m_nNext;
      nIndex = 0;
   }
}

int CSubpathInflator::PathIsFilled(int nSubPath)
{
   int nMinXComponent, nMinYComponent, nMaxXComponent, nMaxYComponent;
   int nMinXIndex, nMinYIndex, nMaxXIndex, nMaxYIndex;

   GetExtremeComponents(
      nSubPath,
      nMinXComponent, nMinXIndex,
      nMinYComponent, nMinYIndex,
      nMaxXComponent, nMaxXIndex,
      nMaxYComponent, nMaxYIndex);

   int nMinXWinding = PathWinding(nMinXComponent, nMinXIndex);
   int nMinYWinding = PathWinding(nMinYComponent, nMinYIndex);
   int nMaxXWinding = PathWinding(nMaxXComponent, nMaxXIndex);
   int nMaxYWinding = PathWinding(nMaxYComponent, nMaxYIndex);
// TRACE("MinX: %2d, MinY: %2d, MaxX: %2d, MaxY: %2d\n",
//       nMinXWinding,
//       nMinYWinding,
//       nMaxXWinding,
//       nMaxYWinding);

   int nResult = nMinXWinding
               + nMinYWinding
               + nMaxXWinding
               + nMaxYWinding;

   return nResult < 0
               ? -1
               : nResult > 0
                  ? 1
                  : 0;
}

int CSubpathInflator::PathWinding(int nComponent, int nIndex)
{
   CFixed lTheta = 0;

   if (nComponent != -1)
   {
      int nPreviousComponent = nComponent;
      int nPreviousIndex = nIndex;
      PreviousPoint(nPreviousComponent, nPreviousIndex);

      int nNextComponent = nComponent;
      int nNextIndex = nIndex;
      NextPoint(nNextComponent, nNextIndex);

      CCurveElement* pCurve = m_Curves.Curve(nComponent);
      CFixedPoint B = ((CFixedPoint*)&pCurve->m_A)[nIndex];

      while (nPreviousComponent != nNextComponent
             || nPreviousIndex != nNextIndex)
      {
         CCurveElement* pPreviousCurve = m_Curves.Curve(nPreviousComponent);
         CFixedPoint A = ((CFixedPoint*)&pPreviousCurve->m_A)[nPreviousIndex];

         CCurveElement* pNextCurve = m_Curves.Curve(nNextComponent);
         CFixedPoint C = ((CFixedPoint*)&pNextCurve->m_A)[nNextIndex];

         if (A == B)
         {
         /* We need to go back. */
            PreviousPoint(nPreviousComponent, nPreviousIndex);
         }
         else if (C == B)
         {
         /* We need to go back. */
            NextPoint(nNextComponent, nNextIndex);
         }
         else
         {
            CFixed lThetaAB = ArcTangent2(A.y - B.y, A.x - B.x);
#if 0
            if (lThetaAB > MakeFixed(90))
            {
               lThetaAB -= MakeFixed(360);
            }
            if (lThetaAB < MakeFixed(-90))
            {
               lThetaAB += MakeFixed(360);
            }
#endif
            CFixed lThetaBC = ArcTangent2(C.y - B.y, C.x - B.x);
#if 0
            if (lThetaBC > MakeFixed(90))
            {
               lThetaBC -= MakeFixed(360);
            }
            if (lThetaBC < MakeFixed(-90))
            {
               lThetaBC += MakeFixed(360);
            }
#endif

            if ((lTheta = lThetaBC - lThetaAB) != 0)
            {
               if (lTheta > MakeFixed(180))
               {
                  lTheta -= MakeFixed(360);
               }
               if (lTheta < MakeFixed(-180))
               {
                  lTheta += MakeFixed(360);
               }
               break;
            }

            PreviousPoint(nPreviousComponent, nPreviousIndex);
            if (nPreviousComponent == nNextComponent
                && nPreviousIndex == nNextIndex)
            {
               break;
            }

            NextPoint(nNextComponent, nNextIndex);
         }
      }
   }

   return (lTheta < 0)
            ? -1
            : (lTheta > 0)
               ? 1
               : 0;
}


/////////////////////////////////////////////////////////////////////////////
// FF 1/20/97 - The following classes are used to inflate a path that has
// been warped. The original versions of these classes are still listed above
// and are used for inflating paths in the general case such as those used for
// shapes (stars, skulls, etc.) However, after a path has been warped, it is
// a reasonable approximation to assume that the elements in the path are
// essentially linear. This is important, because warping generates A LOT of
// small elements and inflation is an N^2 algorithm. By assuming linearity,
// the work per element is greatly reduced. Profile results show that the
// inflation time can be reduced to 9% of its original length.

static void near WarpSplitCurveAt(
   const CFixedPoint* pCurve, CFixed lTime,
   CFixedPoint* pNewLeft,
   CFixedPoint* pNewRight)
{
   CFixedPoint A = pCurve[0];
   CFixedPoint D = pCurve[1];

   pNewLeft[0] = A;
   pNewRight[1] = D;

   pNewLeft[1].x = pNewRight[0].x = A.x + MulFixed(D.x - A.x, lTime);
   pNewLeft[1].y = pNewRight[0].y = A.y + MulFixed(D.y - A.y, lTime);
}

/////////////////////////////////////////////////////////////////////////////
// CWarpCurveIntersector

CWarpCurveIntersector::CWarpCurveIntersector() : Array(sizeof(CWarpCurveIntersection))
{
   m_pElements = NULL;
   m_pElements = new CWarpCurveIntersectElement[MAX_ELEMENTS];
   m_nElementIndex = 0;
}

CWarpCurveIntersector::~CWarpCurveIntersector()
{
   delete [] m_pElements;
}

// Helper routines.

/* Allocate two elements from the curve element stack. */
CWarpCurveIntersectElement* near CWarpCurveIntersector::AllocateElements(int nCount /*=2*/)
{
   ASSERT(m_pElements != NULL);
   ASSERT(m_nElementIndex + nCount <= MAX_ELEMENTS);

   CWarpCurveIntersectElement* pElement = m_pElements + m_nElementIndex;
   m_nElementIndex += nCount;
   return pElement;
}

void CWarpCurveIntersector::IntersectCurves(const CFixedPoint* pCurveA, const CFixedPoint* pCurveB)
{
   if (count() > 0)
   {
      empty_array();
   }

   CWarpCurveIntersectElement ElementA;
   CWarpCurveIntersectElement ElementB;

   ElementA.m_Points[0] = pCurveA[0];
   ElementA.m_Points[1] = pCurveA[1];
   ElementB.m_Points[0] = pCurveB[0];
   ElementB.m_Points[1] = pCurveB[1];

   // Check if the bounding boxes of the curves overlap.
   // If not, then they curves cannot intersect.

   CFixed lMinA, lMaxA, lMinB, lMaxB;

   lMinA = ElementA.m_Points[0].x;
   lMaxA = ElementA.m_Points[1].x;
   if (lMinA > lMaxA)
   {
      CFixed t = lMinA;
      lMinA = lMaxA;
      lMaxA = t;
   }

   lMinB = ElementB.m_Points[0].x;
   lMaxB = ElementB.m_Points[1].x;
   if (lMinB > lMaxB)
   {
      CFixed t = lMinB;
      lMinB = lMaxB;
      lMaxB = t;
   }

   if ((lMinA > lMaxB) || (lMinB > lMaxA))
   {
      return;
   }

   lMinA = ElementA.m_Points[0].y;
   lMaxA = ElementA.m_Points[1].y;
   if (lMinA > lMaxA)
   {
      CFixed t = lMinA;
      lMinA = lMaxA;
      lMaxA = t;
   }

   lMinB = ElementB.m_Points[0].y;
   lMaxB = ElementB.m_Points[1].y;
   if (lMinB > lMaxB)
   {
      CFixed t = lMinB;
      lMinB = lMaxB;
      lMaxB = t;
   }

   if ((lMinA > lMaxB) || (lMinB > lMaxA))
   {
      return;
   }

   ElementA.m_t0 = ElementB.m_t0 = MakeFixed(0);
   ElementA.m_t1 = ElementB.m_t1 = MakeFixed(1);

   // For warped paths, we assume A and B are both flat and we do line
   // intersection of the line connecting the endpoints of both curves.

   int nElementSave = m_nElementIndex;

   CWarpCurveIntersection Int;

   // Special case the endpoint intersections.

   BOOL fAdded = FALSE;

   if (ElementA.m_Points[0] == ElementB.m_Points[1])
   {
      // Connected his end to my start.
      Int.m_tA = ElementA.m_t0;
      Int.m_tB = ElementB.m_t1;
      add_element(&Int);
      fAdded = TRUE;
   }

   if (ElementA.m_Points[1] == ElementB.m_Points[0])
   {
      // Connected my end to his start.
      Int.m_tA = ElementA.m_t1;
      Int.m_tB = ElementB.m_t0;
      add_element(&Int);
      fAdded = TRUE;
   }

   if (!fAdded)
   {
      // We switch to doubles since the numbers can overflow.
      // Note that we do not normalize. Everything divides out later.

      double Adx =  double(ElementA.m_Points[1].x - ElementA.m_Points[0].x);
      double Ady =  double(ElementA.m_Points[1].y - ElementA.m_Points[0].y);
      double Bdx =  double(ElementB.m_Points[1].x - ElementB.m_Points[0].x);
      double Bdy =  double(ElementB.m_Points[1].y - ElementB.m_Points[0].y);
      double ABdx = double(ElementB.m_Points[0].x - ElementA.m_Points[0].x);
      double ABdy = double(ElementB.m_Points[0].y - ElementA.m_Points[0].y);

      double det = Bdx*Ady - Bdy*Adx;

      if (det != 0.0)
      {
			double d0 = (Bdx*ABdy - Bdy*ABdx)/det;
			double d1 = (Adx*ABdy - Ady*ABdx)/det;

			// We're going to be converting to fixed point numbers, so
			// clip to a reasonable range.
			if (d0 < -32000.0)	d0 = -32000.0;
			if (d0 > 32000.0)		d0 = 32000.0;
			if (d1 < -32000.0)	d1 = -32000.0;
			if (d1 > 32000.0)		d1 = 32000.0;

         CFixed s = MakeFixed(d0);
         CFixed t = MakeFixed(d1);

         if (s >= MakeFixed(0)
          && s <= MakeFixed(1)
          && t >= MakeFixed(0)
          && t <= MakeFixed(1))
         {
            Int.m_tA = ElementA.m_t0 + MulFixed(s, ElementA.m_t1 - ElementA.m_t0);
            Int.m_tB = ElementB.m_t0 + MulFixed(t, ElementB.m_t1 - ElementB.m_t0);

            add_element(&Int);
         }
      }
   }

   // Pop the elements we allocated.
   m_nElementIndex = nElementSave;
}

/////////////////////////////////////////////////////////////////////////////
// CWarpCurveArray

CWarpCurveArray::CWarpCurveArray()
   : CDynamicArray(sizeof(CWarpCurveElement), 32, 65520/sizeof(CWarpCurveElement))
{
}

int CWarpCurveArray::AddLine(CFixedPoint A, CFixedPoint B, int nNext /*=-1*/)
{
   CFixed lTheta = ArcTangent2(B.y - A.y, B.x - A.x);
   return AddBezier(
      A.x, A.y,
      A.x, A.y,
      B.x, B.y,
      B.x, B.y,
      nNext,
      lTheta - MakeFixed(180), lTheta);
}

int CWarpCurveArray::AddSpline(CFixedPoint A, CFixedPoint B, CFixedPoint C)
{
   return AddBezier(
      A.x, A.y,
      A.x, A.y,
      C.x, C.y,
      C.x, C.y);
}

int CWarpCurveArray::AddBezier(
   CFixed AX, CFixed AY,
   CFixed BX, CFixed BY,
   CFixed CX, CFixed CY,
   CFixed DX, CFixed DY,
   int nNext /*=-1*/,
   CFixed lThetaA /*=-1L*/,
   CFixed lThetaD /*=-1L*/)
{
   CWarpCurveElement Element;

   Element.m_nNext = nNext;
   Element.m_A.x = AX;
   Element.m_A.y = AY;
   Element.m_D.x = DX;
   Element.m_D.y = DY;

   if (lThetaA == -1L)
   {
      CFixed OtherX = BX, OtherY = BY;
      if (Element.m_A.x == OtherX && Element.m_A.y == OtherY)
      {
         OtherX = CX;
         OtherY = CY;
         if (Element.m_A.x == OtherX && Element.m_A.y == OtherY)
         {
            OtherX = DX;
            OtherY = DY;
         }
      }
      lThetaA = ArcTangent2(Element.m_A.y - OtherY, Element.m_A.x - OtherX);
   }

   if (lThetaD == -1L)
   {
      CFixed OtherX = CX, OtherY = CY;
      if (Element.m_D.x == OtherX && Element.m_D.y == OtherY)
      {
         OtherX = BX;
         OtherY = BY;
         if (Element.m_D.x == OtherX && Element.m_D.y == OtherY)
         {
            OtherX = AX;
            OtherY = AY;
         }
      }
      lThetaD = ArcTangent2(Element.m_D.y - OtherY, Element.m_D.x - OtherX);
   }

   Element.m_lThetaA = lThetaA;
   Element.m_lThetaD = lThetaD;

   AddElement(&Element);

   return Curves()-1;
}

/////////////////////////////////////////////////////////////////////////////
// CWarpSubpathInflator

CWarpSubpathInflator::CWarpSubpathInflator()
{
}

void CWarpSubpathInflator::Inflate(COutlinePathIterator* pSource, COutlinePath* pDestination, CFixed lAmount, CFixed lMiterLimit /*=0*/)
{
   m_Subpaths.RemoveAll();
   m_Curves.Free();

   if (GetSubpath(pSource))
   {
      InflateSubpath(lAmount);
      ConnectSubpath(lAmount, lMiterLimit);
      IntersectSubpath();
      PutSubpath(pDestination);
   }
}

BOOL CWarpSubpathInflator::GetSubpath(COutlinePathIterator* pSource)
{
   BOOL fCurrent = FALSE;
   CFixedPoint cp;
   CFixedPoint FirstPoint;

   while (!pSource->AtEOP())
   {
      switch (*(pSource->CurrentComponent()))
      {
         case COutlinePath::OPE_MoveTo:
         {
            ASSERT(!fCurrent);

            fCurrent = TRUE;
            cp = *(pSource->CurrentPoint());
            FirstPoint = cp;
            break;
         }

         case COutlinePath::OPE_LineTo:
         {
            // All curves should be beziers which are close to linear.
            ASSERT(FALSE);
            break;
         }

         case COutlinePath::OPE_SplineTo:
         {
            // All curves should be beziers which are close to linear.
            ASSERT(FALSE);
            break;
         }

         case COutlinePath::OPE_BezierTo:
         {
            ASSERT(fCurrent);

            CFixedPoint B = *(pSource->CurrentPoint());
            CFixedPoint C = *(pSource->NextPoint());
            CFixedPoint D = *(pSource->NextPoint());

            if (cp != B || B != C || C != D)
            {
               m_Curves.AddBezier(cp.x, cp.y, B.x, B.y, C.x, C.y, D.x, D.y);
               cp = D;
            }

            break;
         }

         default:
         {
            break;
         }
      }
      pSource->NextComponent();
   }

   if (fCurrent)
   {
      if (cp != FirstPoint)
      {
         // Make sure we're closed.
         m_Curves.AddLine(cp, FirstPoint);
      }

      // Now go through and set all the next pointers.

      int nCurves = m_Curves.Curves();
      int nCurve;
      CWarpCurveElement* pCurve;

      for (nCurve = 0; nCurve < nCurves-1; nCurve++)
      {
         pCurve = m_Curves.Curve(nCurve);
         pCurve->m_nNext = nCurve+1;
      }

      pCurve = m_Curves.Curve(nCurve);
      pCurve->m_nNext = 0;

      m_Subpaths.Add(0);

      m_nPathIsFilled = PathIsFilled(0);
      ASSERT(m_nPathIsFilled != 0);
   }

   return fCurrent;
}

void CWarpSubpathInflator::InflateSubpath(CFixed lAmount)
{
   // Run through all curves and inflate them.

   for (int nCurve = 0; nCurve < m_Curves.Curves(); nCurve++)
   {
      CWarpCurveElement *pElement = m_Curves.Curve(nCurve);

      CFixedPoint A = pElement->m_A;
      CFixedPoint D = pElement->m_D;

      CFixed lThetaA = pElement->m_lThetaA;
      CFixed lThetaD = pElement->m_lThetaD;

      // Inflate A.

      ComputeInflatedPoint(
         A,
         lThetaA, lThetaA + MakeFixed(180),
         lAmount,
         &pElement->m_A);

      // Inflate D.

      ComputeInflatedPoint(
         D,
         lThetaD - MakeFixed(180), lThetaD,
         lAmount,
         &pElement->m_D);
   }
}

void CWarpSubpathInflator::ConnectSubpath(CFixed lAmount /*=0*/, CFixed lMiterLimit /*=0*/)
{
   const int nFirstCurve = 0;

   int nCurve = nFirstCurve;

   do
   {
      CWarpCurveElement* pElement = m_Curves.Curve(nCurve);
      int nNext = pElement->m_nNext;
      CWarpCurveElement* pNextElement = m_Curves.Curve(nNext);

      if (pElement->m_D != pNextElement->m_A)
      {
         CFixed lThetaThis = pElement->m_lThetaD + MakeFixed(180);
         CFixed lThetaNext = pNextElement->m_lThetaA + MakeFixed(180);

         CFixed lDelta = lThetaNext-lThetaThis;
         while (lDelta < 0)
         {
            lDelta += MakeFixed(360);
         }
         while (lDelta >= MakeFixed(360))
         {
            lDelta -= MakeFixed(360);
         }
         CFixed lTheta = MidPointFixed(lThetaThis, lThetaNext);

         CFixed lTan = Tangent(lTheta-lThetaThis);

         if (lTan == 0
          || lTan == 0x7FFFFFFF
          || lTan == -0x7FFFFFFF
          || !((lDelta > MakeFixed(180) && (lAmount > 0))
            || (lDelta < MakeFixed(180) && (lAmount < 0))))
         {
            // Just join them.
            int nNewCurve = m_Curves.AddLine(pElement->m_D, pNextElement->m_A, nNext);

            pElement = m_Curves.Curve(nCurve);
            pElement->m_nNext = nNewCurve;
         }
         else
         {
            ASSERT(lTan != 0x7FFFFFFF && lTan != -0x7FFFFFFF);
            CFixed lQ = SafeDivFixed(lAmount, lTan);

            CFixed lAbsQ = (lQ < 0) ? -lQ : lQ;
            CFixed lAbsAmount = (lAmount < 0) ? -lAmount : lAmount;
            CFixed lNewMiterLimit = MulFixed(lMiterLimit, lAbsAmount);
            CFixedPoint NextA = pNextElement->m_A;

            int nNewBase = m_Curves.Curves();

            if (lNewMiterLimit == 0 || lAbsQ <= lNewMiterLimit)
            {
               // Miter them (join them in a point).

               CFixedPoint JoinPoint;
               JoinPoint.x = pElement->m_D.x + MulFixed(lQ, Cosine(lThetaThis));
               JoinPoint.y = pElement->m_D.y + MulFixed(lQ, Sine(lThetaThis));

               pElement->m_nNext = nNewBase;
               m_Curves.AddLine(pElement->m_D, JoinPoint, nNewBase+1);
               m_Curves.AddLine(JoinPoint, NextA, nNext);
            }
            else
            {
               // Exceeds miter limit. Chop off the miter.
               lQ = (lQ < 0) ? -lNewMiterLimit : lNewMiterLimit;
               CFixedPoint NewA;
               CFixedPoint NewB;

               NewA.x = pElement->m_D.x + MulFixed(lQ, Cosine(lThetaThis));
               NewA.y = pElement->m_D.y + MulFixed(lQ, Sine(lThetaThis));

               NewB.x = pNextElement->m_A.x + MulFixed(lQ, Cosine(lThetaNext));
               NewB.y = pNextElement->m_A.y + MulFixed(lQ, Sine(lThetaNext));

               pElement->m_nNext = nNewBase;
               m_Curves.AddLine(pElement->m_D, NewA, nNewBase+1);
               m_Curves.AddLine(NewA, NewB, nNewBase+2);
               m_Curves.AddLine(NewB, NextA, nNext);
            }
         }
      }

      nCurve = nNext;
   } while (nCurve != nFirstCurve);
}

void CWarpSubpathInflator::IntersectSubpath(void)
{
   CWarpCurveIntersector Intersector;

   for (int nSubpath = 0; nSubpath < m_Subpaths.GetSize(); nSubpath++)
   {
      int nFirstCurve = m_Subpaths.GetAt(nSubpath);
      int nCurve = nFirstCurve;
      int nPrevCurve = -1;
      do
      {
         CWarpCurveElement* pCurve = m_Curves.Curve(nCurve);
         int nNextCurve = pCurve->m_nNext;
         int nSearchCurve = nNextCurve;
         int nPrevSearch = nCurve;
         while (nSearchCurve != nFirstCurve)
         {
            CWarpCurveElement* pSearchCurve = m_Curves.Curve(nSearchCurve);
            int nNextSearch = pSearchCurve->m_nNext;
            ASSERT(nSearchCurve != nCurve);

            Intersector.IntersectCurves(&pCurve->m_A, &pSearchCurve->m_A);
            int nIntersections = Intersector.Intersections();

            for (int nIntersection = 0; nIntersection < nIntersections; nIntersection++)
            {
               CWarpCurveIntersection* pIntersection = Intersector.Intersection(nIntersection);
               int nConnected = 0;
               if (pIntersection->m_tA == MakeFixed(1) && pIntersection->m_tB == MakeFixed(0))
               {
                  nConnected = 1;
               }
               else if (pIntersection->m_tA == MakeFixed(0) && pIntersection->m_tB == MakeFixed(1))
               {
                  nConnected = -1;
               }

               if (!(nConnected == 1 && pCurve->m_nNext == nSearchCurve)
                && !(nConnected == -1 && pSearchCurve->m_nNext == nCurve))
               {
                  // We found an intersection.
                  // Split the two curves.
                  // pCurve is A, pNextCurve is B.

                  if (pIntersection->m_tB == MakeFixed(0))
                  {
                     nSearchCurve = nPrevSearch;
                     pSearchCurve = m_Curves.Curve(nSearchCurve);
                     pIntersection->m_tB = MakeFixed(1);
                  }

                  int nThisA = nCurve;

                  int nNextA = pCurve->m_nNext;
                  int nNextB = pSearchCurve->m_nNext;

                  int nLinkA, nLinkB;

                  CFixedPoint AIntersection;
                  CFixedPoint BIntersection;

                  if (pIntersection->m_tA == MakeFixed(1))
                  {
                     nLinkB = nNextA;
                     AIntersection = pCurve->m_D;
                  }
                  else
                  {
                     CFixedPoint NewLeftA[2];
                     CFixedPoint NewRightA[2];

                     WarpSplitCurveAt(&pCurve->m_A, pIntersection->m_tA, NewLeftA, NewRightA);
                     nLinkB = m_Curves.AddBezier(NewRightA[0].x, NewRightA[0].y,
                                                 NewRightA[0].x, NewRightA[0].y,
                                                 NewRightA[1].x, NewRightA[1].y,
                                                 NewRightA[1].x, NewRightA[1].y,
                                                 nNextA);

                     pCurve = m_Curves.Curve(nThisA);
                     pSearchCurve = m_Curves.Curve(nSearchCurve);

                     memcpy(&pCurve->m_A, NewLeftA, sizeof(NewLeftA));
                     AIntersection = NewRightA[0];
                  }

                  if (pIntersection->m_tB == MakeFixed(1))
                  {
                     nLinkA = nNextB;
                     BIntersection = pSearchCurve->m_D;
                  }
                  else
                  {
                     CFixedPoint NewLeftB[2];
                     CFixedPoint NewRightB[2];

                     WarpSplitCurveAt(&pSearchCurve->m_A, pIntersection->m_tB, NewLeftB, NewRightB);
                     nLinkA = m_Curves.AddBezier(NewRightB[0].x, NewRightB[0].y,
                                                 NewRightB[0].x, NewRightB[0].y,
                                                 NewRightB[1].x, NewRightB[1].y,
                                                 NewRightB[1].x, NewRightB[1].y,
                                                 nNextB);

                     pCurve = m_Curves.Curve(nThisA);
                     pSearchCurve = m_Curves.Curve(nSearchCurve);

                     memcpy(&pSearchCurve->m_A, NewLeftB, sizeof(NewLeftB));
                     BIntersection = NewRightB[0];
                  }

                  pCurve->m_nNext = nLinkA;
                  pSearchCurve->m_nNext = nLinkB;

                  CWarpCurveElement* pNextA = m_Curves.Curve(nLinkA);
                  CWarpCurveElement* pNextB = m_Curves.Curve(nLinkB);

                  pCurve->m_D.x
                     = pNextA->m_A.x
                     = pSearchCurve->m_D.x
                     = pNextB->m_A.x
                     = MidPointFixed(AIntersection.x, BIntersection.x);

                  pCurve->m_D.y
                     = pNextA->m_A.y
                     = pSearchCurve->m_D.y
                     = pNextB->m_A.y
                     = MidPointFixed(AIntersection.y, BIntersection.y);

                  m_Subpaths.Add(nLinkB);

                  nNextSearch = nNextCurve = nLinkA;

                  break;
               }
            }

            nPrevSearch = nSearchCurve;
            nSearchCurve = nNextSearch;
         }

         nPrevCurve = nCurve;
         nCurve = nNextCurve;
      } while (nCurve != nFirstCurve);
   }
}

void CWarpSubpathInflator::PutSubpath(COutlinePath* pDestination)
{
   int nSubpaths = m_Subpaths.GetSize();

   for (int nSubpath = 0; nSubpath < nSubpaths; nSubpath++)
   {
      int nFilled = PathIsFilled(nSubpath);
      if (nFilled == m_nPathIsFilled || nFilled == 0)
      {
         int nFirstCurve = (int)m_Subpaths.GetAt(nSubpath);

         BOOL fFirstPoint = TRUE;
         long* pFirstCurve = NULL;
         int nCurve = nFirstCurve;

         do
         {
            CWarpCurveElement* pCurve = m_Curves.Curve(nCurve);

            if (fFirstPoint)
            {
               fFirstPoint = FALSE;
               pDestination->MoveTo(pCurve->m_A.x, pCurve->m_A.y);
            }

            pDestination->LineTo(pCurve->m_D.x, pCurve->m_D.y);

            nCurve = pCurve->m_nNext;
         } while (nCurve != nFirstCurve);

         pDestination->Close();
      }
   }
}

void CWarpSubpathInflator::GetExtremeComponents(
   int nSubpath,
   int& nMinXComponent, int& nMinXIndex,
   int& nMinYComponent, int& nMinYIndex,
   int& nMaxXComponent, int& nMaxXIndex,
   int& nMaxYComponent, int& nMaxYIndex)

{
   int nFirstCurve = (int)m_Subpaths.GetAt(nSubpath);

   int nCurve = nFirstCurve;
   nMinXComponent =
   nMinYComponent =
   nMaxXComponent =
   nMaxYComponent = -1;
   CFixed nMinX, nMinY, nMaxX, nMaxY;
   int nIndex = 0;
   CWarpCurveElement* pCurve;

   do
   {
      if (nIndex == 0)
      {
         pCurve = m_Curves.Curve(nCurve);
      }

      CFixed nX = ((CFixedPoint*)&pCurve->m_A)[nIndex].x;
      CFixed nY = ((CFixedPoint*)&pCurve->m_A)[nIndex].y;

      if (nMinXComponent == -1)
      {
         nMinXComponent =
         nMinYComponent =
         nMaxXComponent =
         nMaxYComponent = nCurve;
         nMinX =
         nMaxX = nX;
         nMinY =
         nMaxY = nY;
         nMinXIndex =
         nMinYIndex =
         nMaxXIndex =
         nMaxYIndex = nIndex;
      }
      else
      {
         if (nX < nMinX)
         {
            nMinXComponent = nCurve;
            nMinX = nX;
            nMinXIndex = nIndex;
         }
         if (nY < nMinY)
         {
            nMinYComponent = nCurve;
            nMinY = nY;
            nMinYIndex = nIndex;
         }
         if (nX > nMaxX)
         {
            nMaxXComponent = nCurve;
            nMaxX = nX;
            nMaxXIndex = nIndex;
         }
         if (nY > nMaxY)
         {
            nMaxYComponent = nCurve;
            nMaxY = nY;
            nMaxYIndex = nIndex;
         }
      }

      // Move to next point as needed.

      if (++nIndex == 1)
      {
         nIndex = 0;
         nCurve = pCurve->m_nNext;
      }
   } while (nCurve != nFirstCurve || nIndex != 0);
}

void CWarpSubpathInflator::PreviousPoint(int& nComponent, int& nIndex)
{
   if (nIndex == 0)
   {
      // Find the previous component.
      int nComponentToFind = nComponent;

      CWarpCurveElement* pCurve = m_Curves.Curve(nComponent);
      while (pCurve->m_nNext != nComponentToFind)
      {
         nComponent = pCurve->m_nNext;
         pCurve = m_Curves.Curve(nComponent);
      }
      nIndex = 1;    // Index of last point in a curve.
   }
   // And back off a point.
   nIndex--;
}

void CWarpSubpathInflator::NextPoint(int& nComponent, int& nIndex)
{
   // Move to the next curve.
   CWarpCurveElement* pCurve = m_Curves.Curve(nComponent);
   nComponent = pCurve->m_nNext;
   nIndex = 0;
}

int CWarpSubpathInflator::PathIsFilled(int nSubPath)
{
   int nMinXComponent, nMinYComponent, nMaxXComponent, nMaxYComponent;
   int nMinXIndex, nMinYIndex, nMaxXIndex, nMaxYIndex;

   GetExtremeComponents(
      nSubPath,
      nMinXComponent, nMinXIndex,
      nMinYComponent, nMinYIndex,
      nMaxXComponent, nMaxXIndex,
      nMaxYComponent, nMaxYIndex);

   int nMinXWinding = PathWinding(nMinXComponent, nMinXIndex);
   int nMinYWinding = PathWinding(nMinYComponent, nMinYIndex);
   int nMaxXWinding = PathWinding(nMaxXComponent, nMaxXIndex);
   int nMaxYWinding = PathWinding(nMaxYComponent, nMaxYIndex);

   int nResult = nMinXWinding
               + nMinYWinding
               + nMaxXWinding
               + nMaxYWinding;

   return nResult < 0
               ? -1
               : nResult > 0
                  ? 1
                  : 0;
}

int CWarpSubpathInflator::PathWinding(int nComponent, int nIndex)
{
   CFixed lTheta = 0;

   if (nComponent != -1)
   {
      int nPreviousComponent = nComponent;
      int nPreviousIndex = nIndex;
      PreviousPoint(nPreviousComponent, nPreviousIndex);

      int nNextComponent = nComponent;
      int nNextIndex = nIndex;
      NextPoint(nNextComponent, nNextIndex);

      CWarpCurveElement* pCurve = m_Curves.Curve(nComponent);
      CFixedPoint B = ((CFixedPoint*)&pCurve->m_A)[nIndex];

      while (nPreviousComponent != nNextComponent || nPreviousIndex != nNextIndex)
      {
         CWarpCurveElement* pPreviousCurve = m_Curves.Curve(nPreviousComponent);
         CFixedPoint A = ((CFixedPoint*)&pPreviousCurve->m_A)[nPreviousIndex];

         CWarpCurveElement* pNextCurve = m_Curves.Curve(nNextComponent);
         CFixedPoint C = ((CFixedPoint*)&pNextCurve->m_A)[nNextIndex];

         if (A == B)
         {
            // We need to go back.
            PreviousPoint(nPreviousComponent, nPreviousIndex);
         }
         else if (C == B)
         {
            // We need to go back.
            NextPoint(nNextComponent, nNextIndex);
         }
         else
         {
            CFixed lThetaAB = ArcTangent2(A.y - B.y, A.x - B.x);
            CFixed lThetaBC = ArcTangent2(C.y - B.y, C.x - B.x);

            if ((lTheta = lThetaBC - lThetaAB) != 0)
            {
               if (lTheta > MakeFixed(180))
               {
                  lTheta -= MakeFixed(360);
               }
               if (lTheta < MakeFixed(-180))
               {
                  lTheta += MakeFixed(360);
               }
               break;
            }

            PreviousPoint(nPreviousComponent, nPreviousIndex);
            if (nPreviousComponent == nNextComponent && nPreviousIndex == nNextIndex)
            {
               break;
            }

            NextPoint(nNextComponent, nNextIndex);
         }
      }
   }

   return (lTheta < 0)
            ? -1
            : (lTheta > 0)
               ? 1
               : 0;
}
