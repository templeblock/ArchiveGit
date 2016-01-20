/*
// $Header: /PM8/App/OUTPATH.H 1     3/03/99 6:08p Gbeddow $
//
// $Workfile: OUTPATH.H $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/OUTPATH.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 3     1/25/99 9:58a Lwilson
// Initial path component support
// 
// 2     4/29/98 6:31p Rlovejoy
// Updated to use fill objects.
// 
//    Rev 1.0   14 Aug 1997 15:23:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:54   Fred
// Initial revision.
// 
//    Rev 1.7   11 Feb 1997 14:54:46   Jay
// Free draw tool
// 
//    Rev 1.6   07 Feb 1997 08:49:46   Fred
//  
// 
//    Rev 1.5   29 Jan 1997 14:18:18   Jay
// Outline path and PtInPath
// 
//    Rev 1.4   20 Jan 1997 14:07:06   Fred
// Inflate speedups
// 
//    Rev 1.3   17 Jan 1997 17:03:16   Jay
// New select for drawing objects.
// 
//    Rev 1.2   09 Jan 1997 16:54:26   Fred
//  
// 
//    Rev 1.1   04 Sep 1996 16:34:24   Jay
// Fixes for Inflate filling in!
// 
//    Rev 1.0   14 Mar 1996 13:48:10   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:18   FRED
// Initial revision.
// 
//    Rev 1.18   19 Aug 1995 14:48:38   FRED
// Inflate now works with flipped objects
// 
//    Rev 1.17   22 Jun 1995 16:05:52   JAY
// Fixed some bugs.
// 
//    Rev 1.16   25 May 1995 14:04:26   JAY
// Got rid of malloc.
// 
//    Rev 1.15   25 May 1995 10:00:30   JAY
// Converted to beziers.
// 
//    Rev 1.14   24 May 1995 14:45:18   JAY
// Added angles to spline elements.
// 
//    Rev 1.13   24 May 1995 12:08:20   JAY
// Improvements.
// 
//    Rev 1.12   22 May 1995 17:29:36   JAY
// More inflate goodies.
// 
//    Rev 1.11   18 May 1995 10:51:30   FRED
// Code to get bounding box of path.
// 
// 
//    Rev 1.10   18 May 1995 07:45:16   JAY
// More inflate nonsense.
// 
//    Rev 1.9   16 May 1995 07:52:36   JAY
// Flipping functions in matrix.
// 
//    Rev 1.8   11 May 1995 08:53:46   FRED
// Added COutlinePath::Flatten(). Inflate calls
// Flatten() before inflating.
// 
// 
//    Rev 1.7   09 May 1995 09:48:20   JAY
// Added UsePalette flag.
// 
//    Rev 1.6   09 May 1995 08:03:34   JAY
// Added some initial mitering stuff.
// 
//    Rev 1.5   08 May 1995 09:09:52   FRED
// Added Draw2() which draws using Inflate() and other nifty stuff.
// 
// 
//    Rev 1.4   05 May 1995 16:49:30   JAY
// Exciting new additions to OutlinePaths!
// 
//    Rev 1.3   04 May 1995 17:16:06   JAY
// Fixed-point. Drawing objects.
// 
//    Rev 1.2   03 May 1995 10:39:52   FRED
//  
// 
//    Rev 1.1   27 Apr 1995 13:20:14   FRED
// 
//    Rev 1.0   25 Apr 1995 10:33:50   FRED
// Initial revision.
*/

class COutlinePath;
class COutlinePathArray;
class COutlinePathMatrix;

#ifndef __OUTPATH_H__
#define __OUTPATH_H__

#include "cfixed.h"
#include "redisp.h"
#include "dynarray.h"
#include "fill.h"

class COutlinePathIterator;
class CWarpField;

/////////////////////////////////////////////////////////////////////////////
// COutlinePathMatrix

class COutlinePathMatrix
{
public:
	COutlinePathMatrix(void);
	COutlinePathMatrix(long A, long B, long C, long D, long X, long Y);
	~COutlinePathMatrix(void);
public:
	void Set(long A, long B, long C, long D, long X, long Y);
	void Identity(void);
	void ScaleBy(long X, long Y);
	void RotateBy(long T);
	void TranslateBy(long X, long Y);
	void XFlip(long Min, long Max);
	void YFlip(long Min, long Max);
	void Concatenate(COutlinePathMatrix* pMatrix);
public:
	void Transform(long* X, long* Y);
public:
	long m_A, m_B;
	long m_C, m_D;
	long m_X, m_Y;
};
	
/////////////////////////////////////////////////////////////////////////////
// COutlinePathArray

class COutlinePathArray : public CDynamicArray
{
	friend class COutlinePathIterator;
public:
	enum OutlinePathElement
	{
		OPE_End			=	0,
		OPE_Close		=	1,
		OPE_MoveTo		=	2,			// <AX,AY>
		OPE_LineTo		=	3,			// <AX,AY>
		OPE_SplineTo	=	4,			// <AX,AY,BX,BY>
		OPE_BezierTo	=	5,			// <AX,AY,BX,BY,CX,CY>
		OPE_ElementCount
	};
	struct OutlinePathEndRecord			{ long dwElement; };
	struct OutlinePathCloseRecord			{ long dwElement; };
	struct OutlinePathMoveToRecord		{ long dwElement; long AX, AY; };
	struct OutlinePathLineToRecord		{ long dwElement; long AX, AY; };
	struct OutlinePathSplineToRecord		{ long dwElement; long AX, AY, BX, BY; };
	struct OutlinePathBezierToRecord		{ long dwElement; long AX, AY, BX, BY, CX, CY; };
public:
	COutlinePathArray(void);
	~COutlinePathArray(void);
public:
	void End(void);
	void Close(void);
	void MoveTo(long AX, long AY);
	void LineTo(long AX, long AY);
	void SplineTo(long AX, long AY, long BX, long BY);
	void BezierTo(long AX, long AY, long BX, long BY, long CX, long CY);
};

/////////////////////////////////////////////////////////////////////////////
// COutlinePath

class COutlinePath : public COutlinePathArray
{
public:
	COutlinePath(void);
	~COutlinePath(void);
public:

	enum
	{
		NoLine = -32767
	};
	
	void Draw(CDC* pDC, COLORREF clFillColor, BOOL fFillColorExists, int nLineWidth, COLORREF clLineColor, BOOL fLineColorExists, int nFillMode, BOOL fUsePalette = TRUE);
	void Draw(CDC* pDC, CFill* pFillObject, int nLineWidth, COLORREF clLineColor, BOOL fLineColorExists, int nFillMode, BOOL fUsePalette = TRUE);
	void Draw2(CDC* pDC, COLORREF clFillColor, BOOL fFillColorExists, int nLineWidth, COLORREF clLineColor, BOOL fLineColorExists, int nFillMode, BOOL fUsePalette = TRUE);
	void Draw2(CDC* pDC, CFill* pFillObject, int nLineWidth, COLORREF clLineColor, BOOL fLineColorExists, int nFillMode, BOOL fUsePalette = TRUE);
	// Simpler form of draw. Pen and brush set outside.
	void Draw(CDC* pDC);
	void Warp(CWarpField* pWarpField, COutlinePath* pDestination = NULL);
	void Transform(COutlinePathMatrix* pMatrix, COutlinePath* pDestination = NULL);

	void SetMiterLimit(CFixed lLimit = 0)
		{ m_lMiterLimit = lLimit; }

	// The "UsePolyline" flag is basically a kludge to force the drawing code to
	// use Polyline instead of Polygon. This turns out to be useful for drawing
	// the cursor in a warp field.
	BOOL UsePolyline(void) const
		{ return m_fUsePolyline; }
	void UsePolyline(BOOL fUsePolyline)
		{ m_fUsePolyline = fUsePolyline; }

	// The is warped flag indicates whether the path has been put through the
	// warping meat grinder. Such paths are composed of lots of little bezier
	// curves which can be approximated as being linear. This fact can be used
	// to reduce the inflate process to about 9% of the general purpose case.
	BOOL IsWarped(void) const
		{ return m_fIsWarped; }
	void IsWarped(BOOL fIsWarped)
		{ m_fIsWarped = fIsWarped; }

	// This computes whether a given outline path is closed or not.
	BOOL IsClosed(void);

	// Inflate a path by the given amount.
	// If lAmount is negative, the path is "un-inflated" ("deflated"?).
	void Inflate(CFixed lAmount, COutlinePath* pDestination = NULL);

/*
// Reverse a path. This changes the orientation of the path by traversing
// it in the opposite direction.
*/

	void Reverse(COutlinePath* pDestination = NULL);

/*
// Append another path onto this one.
*/

	void Append(const COutlinePath* pOtherPath);
	
/*
// Convert a path to all lines.
*/

	void Flatten(COutlinePath* pDestination = NULL);
	void FlattenSpline(COutlinePath* pDestination, long AX, long AY, long BX, long BY, long CX, long CY, long GY);
	void FlattenBezier(COutlinePath* pDestination, long AX, long AY, long BX, long BY, long CX, long CY, long DX, long DY);
	
/*
// Compute the bounding box of the path.
//
//		BoundingBox() computes an accurate bounding box for the path.
//		QuickBoundingBox() computes a bounding box using the convex hull for any curves.
*/

	void BoundingBox(CFixedRect* pRect);
	void QuickBoundingBox(CFixedRect* pRect);

	BOOL Assign(COutlinePath* pPath);
	BOOL Copy(const COutlinePath* pPath);
	BOOL GetRPath( RPath& rPath, const R2dTransform& transform ) const;

	// Return whether a point is within this path or not.
	BOOL PtInPath(CFixedPoint p);

protected:
	CFixed m_lMiterLimit;

protected:
	// constants used by FlattenSpline()
	enum
	{
		PIXELSIZE	=	64,			// number of units per pixel. It has to be a power of two
		PIXSHIFT		=	6,				// should be 2log of PIXELSIZE
		ERRDIV		=	16,			// maximum error is  (pixel/ERRDIV)
		ERRSHIFT		=	4,				// = 2log(ERRDIV), define only if ERRDIV is a power of 2
		
		ONE			=	0x40,			// constants for 26.6 arithmetic
		HALF			=	0x20,
		HALFM			=	0x1f,
		
		// The maximum number of vectors a spline segment is broken down into is 2 ^ MAXGY
		// MAXGY can at most be: (31 - (15 + PIXSHIFT = 21)) / 2

		MAXGY			=	5,
		MAXMAXGY		=	8,				// related to MAXVECTORS

		MAXVECTORS	=	257			// must be at least 257  = (2 ^ MAXMAXGY) + 1
	};

protected:
	BOOL m_fUsePolyline;
	BOOL m_fIsWarped;
};


/////////////////////////////////////////////////////////////////////////////
// CCurveIntersector

struct CCurveIntersection
{
	CFixed m_tA;			// Intersection time for curve A.
	CFixed m_tB;			// Intersection time for curve B.
};

struct CCurveIntersectElement
{
	CFixedPoint m_Points[4];				// 4*16
	CFixedRect m_BBox;						// 2*16
	CFixed m_t0;								// 4
	CFixed m_t1;								// 4
	BOOL m_fIsFlat;							// 2
};

class CCurveIntersector : public Array
{
public:
	CCurveIntersector();
	~CCurveIntersector();

	void IntersectCurves(const CFixedPoint* pCurveA, const CFixedPoint* pCurveB);
	int Intersections(void)
		{ return count(); }
	CCurveIntersection* Intersection(int nIntersection)
		{ return (CCurveIntersection*)get_element(nIntersection); }

protected:

	enum
	{
		MAX_ELEMENTS = 4*(16+1)			// 16 depth max.
	};

	void near RecursivelyIntersect(const CCurveIntersectElement* pCurveA,
									  const CCurveIntersectElement* pCurveB);

	CCurveIntersectElement* near AllocateElements(int nCount = 2);

	static void near SplitCurve(const CCurveIntersectElement* pCurveA,
									CCurveIntersectElement* pCurveALeft,
									CCurveIntersectElement* pCurveARight);
	static void near CurveBBox(CCurveIntersectElement* pCurve);
	static BOOL near BBoxesOverlap(const CFixedRect& BBoxA, const CFixedRect& BBoxB);
	static BOOL near CurvesOverlap(const CCurveIntersectElement* pCurveA, const CCurveIntersectElement* pCurveB);
	static void near SetCurveFlat(CCurveIntersectElement* pCurve);

	int m_nElementIndex;
	CCurveIntersectElement* m_pElements;
};

/////////////////////////////////////////////////////////////////////////////
// CCurveElement

class CCurveElement
{
public:
	int m_nNext;

	CFixedPoint m_A;
	CFixedPoint m_B;
	CFixedPoint m_C;
	CFixedPoint m_D;

	CFixed m_lThetaA;			// Slope pointing out of A (usually B to A).
	CFixed m_lThetaD;		// Slope pointing out of D (usually C to D).
};

/////////////////////////////////////////////////////////////////////////////
// CCurveArray

class CCurveArray : public CDynamicArray
{
public:
	CCurveArray();
	int Curves(void)
		{ return (int)GetElementCount(); }
	CCurveElement* Curve(int nCurve)
		{ return (CCurveElement*)GetElement((DWORD)nCurve); }

	/* Add a line. */
	int AddLine(CFixedPoint A, CFixedPoint B, int nNext = -1);
	/* Add a spline. */
	int AddSpline(CFixedPoint A, CFixedPoint B, CFixedPoint C);
	/* Add a bezier. */
	int AddBezier(CFixed AX, CFixed AY,
					  CFixed BX, CFixed BY,
					  CFixed CX, CFixed CY,
					  CFixed DX, CFixed DY,
					  int nNext = -1,
					  CFixed lThetaA = -1L,
					  CFixed lThetaD = -1L);
};

/////////////////////////////////////////////////////////////////////////////
// CSubpathInflator

class CSubpathInflator
{
public:
	CSubpathInflator();

	void Inflate(COutlinePathIterator* pSource, COutlinePath* pDestination, CFixed lAmount, CFixed lMiterLimit = 0);

	static DWORD m_dwIntersectionFound;
	static DWORD m_dwIntersectCalled;
	static DWORD m_dwTrivialIntersections;

protected:

	BOOL GetSubpath(COutlinePathIterator* pSource);
	void InflateSubpath(CFixed lAmount);
	void ConnectSubpath(CFixed lAmount = 0, CFixed lMiterLimit = 0);
	void IntersectSubpath(void);
	void PutSubpath(COutlinePath* pDestination);
	int PathIsFilled(int nSubPath);
	int PathWinding(int nComponent, int nIndex);
	void GetExtremeComponents(
		int nSubpath,
		int& nMinXComponent, int& nMinXIndex,
		int& nMinYComponent, int& nMinYIndex,
		int& nMaxXComponent, int& nMaxXIndex,
		int& nMaxYComponent, int& nMaxYIndex);
	void NextPoint(int& nComponent, int& nIndex);
	void PreviousPoint(int& nComponent, int& nIndex);

	void DumpSubpath(int nSubpath);

	CWordArray m_Subpaths;
	CCurveArray m_Curves;
	int m_nPathIsFilled;
};




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

/////////////////////////////////////////////////////////////////////////////
// CWarpCurveIntersector

struct CWarpCurveIntersection
{
	CFixed m_tA;			// Intersection time for curve A.
	CFixed m_tB;			// Intersection time for curve B.
};

struct CWarpCurveIntersectElement
{
	CFixedPoint m_Points[2];
	CFixedRect m_BBox;
	CFixed m_t0;
	CFixed m_t1;
};

class CWarpCurveIntersector : public Array
{
public:
	CWarpCurveIntersector();
	~CWarpCurveIntersector();

	void IntersectCurves(const CFixedPoint* pCurveA, const CFixedPoint* pCurveB);
	int Intersections(void)
		{ return count(); }
	CWarpCurveIntersection* Intersection(int nIntersection)
		{ return (CWarpCurveIntersection*)get_element(nIntersection); }

protected:

	enum
	{
		MAX_ELEMENTS = 4*(16+1)			// 16 depth max.
	};

	CWarpCurveIntersectElement* near AllocateElements(int nCount = 2);

	int m_nElementIndex;
	CWarpCurveIntersectElement* m_pElements;
};

/////////////////////////////////////////////////////////////////////////////
// CWarpCurveElement

class CWarpCurveElement
{
public:
	int m_nNext;

	CFixedPoint m_A;
	CFixedPoint m_D;

	CFixed m_lThetaA;		// Slope pointing out of A (usually B to A).
	CFixed m_lThetaD;		// Slope pointing out of D (usually C to D).
};

/////////////////////////////////////////////////////////////////////////////
// CWarpCurveArray

class CWarpCurveArray : public CDynamicArray
{
public:
	CWarpCurveArray();
	int Curves(void)
		{ return (int)GetElementCount(); }
	CWarpCurveElement* Curve(int nCurve)
		{ return (CWarpCurveElement*)GetElement((DWORD)nCurve); }

	/* Add a line. */
	int AddLine(CFixedPoint A, CFixedPoint B, int nNext = -1);
	/* Add a spline. */
	int AddSpline(CFixedPoint A, CFixedPoint B, CFixedPoint C);
	/* Add a bezier. */
	int AddBezier(
		CFixed AX, CFixed AY,
		CFixed BX, CFixed BY,
		CFixed CX, CFixed CY,
		CFixed DX, CFixed DY,
		int nNext = -1,
		CFixed lThetaA = -1L,
		CFixed lThetaD = -1L);
};

/////////////////////////////////////////////////////////////////////////////
// CWarpSubpathInflator

class CWarpSubpathInflator
{
public:
	CWarpSubpathInflator();

	void Inflate(COutlinePathIterator* pSource, COutlinePath* pDestination, CFixed lAmount, CFixed lMiterLimit = 0);

protected:

	BOOL GetSubpath(COutlinePathIterator* pSource);
	void InflateSubpath(CFixed lAmount);
	void ConnectSubpath(CFixed lAmount = 0, CFixed lMiterLimit = 0);
	void IntersectSubpath(void);
	void PutSubpath(COutlinePath* pDestination);
	int PathIsFilled(int nSubPath);
	int PathWinding(int nComponent, int nIndex);
	void GetExtremeComponents(
		int nSubpath,
		int& nMinXComponent, int& nMinXIndex,
		int& nMinYComponent, int& nMinYIndex,
		int& nMaxXComponent, int& nMaxXIndex,
		int& nMaxYComponent, int& nMaxYIndex);
	void NextPoint(int& nComponent, int& nIndex);
	void PreviousPoint(int& nComponent, int& nIndex);

	CWordArray m_Subpaths;
	CWarpCurveArray m_Curves;
	int m_nPathIsFilled;
};

#endif
