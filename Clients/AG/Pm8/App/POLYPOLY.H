/*
// $Header: /PM8/App/POLYPOLY.H 1     3/03/99 6:10p Gbeddow $
//
// $Workfile: POLYPOLY.H $
// $Revision: 1 $
// $Date: 3/03/99 6:10p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/POLYPOLY.H $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 5     5/14/98 10:53a Rlovejoy
// Added overloaded GetBoundingBox() functions.
// 
// 4     5/12/98 11:35a Rlovejoy
// Added 'GetBoundingCircle()'
// 
// 3     5/08/98 12:03p Rlovejoy
// Added GetBoundingBox()
// 
// 2     4/29/98 6:32p Rlovejoy
// Updated for new fill format.
// 
//    Rev 1.0   14 Aug 1997 15:24:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:22   Fred
// Initial revision.
// 
//    Rev 1.4   11 Feb 1997 14:54:48   Jay
// Free draw tool
// 
//    Rev 1.3   17 Jan 1997 17:03:20   Jay
// New select for drawing objects.
// 
//    Rev 1.2   09 Jan 1997 16:56:18   Fred
//  
// 
//    Rev 1.1   23 Aug 1996 07:19:50   Jay
// Fixed the "NT clipping bug". RoundFixed was not sign-extending correctly.
// 
//    Rev 1.0   14 Mar 1996 13:42:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:22   FRED
// Initial revision.
// 
//    Rev 1.4   25 May 1995 13:04:58   FRED
// Some simple optimizations.
// 
// 
//    Rev 1.3   09 May 1995 09:48:20   JAY
// Added UsePalette flag.
// 
//    Rev 1.2   09 May 1995 08:32:20   FRED
// Added Bezier support.
// 
// 
//    Rev 1.1   03 May 1995 10:39:50   FRED
//  
// 
//    Rev 1.0   25 Apr 1995 10:33:54   FRED
// Initial revision.
*/

class CPolyPolygon;
class CPolyPolygonPointArray;
class CPolyPolygonCountArray;

#ifndef __POLYPOLY_H__
#define __POLYPOLY_H__

#include "dynarray.h"
#include "dibbrush.h"
#include "fill.h"

/////////////////////////////////////////////////////////////////////////////
// CPolyPolygonPointArray

class CPolyPolygonPointArray : public CDynamicArray
{
public:
   CPolyPolygonPointArray(void);
   ~CPolyPolygonPointArray(void);
public:
   BOOL AddPoint(POINT p)
      { return AddElement(&p); }
   POINT GetPoint(DWORD dwIndex)
      { return *(LPPOINT)GetElement(dwIndex); }
};

/////////////////////////////////////////////////////////////////////////////
// CPolyPolygonCountArray

class CPolyPolygonCountArray : public CDynamicArray
{
public:
   CPolyPolygonCountArray(void);
   ~CPolyPolygonCountArray(void);
public:
   BOOL AddCount(int c)
      { return AddElement(&c); }
   int GetCount(DWORD dwIndex)
      { return *(LPINT)GetElement(dwIndex); }
};

/////////////////////////////////////////////////////////////////////////////
// CPolyPolygon

class CPolyPolygon
{
	friend class CFill;
	friend class CFlatFill;
	friend class CGradientFill;
public:
   CPolyPolygon(void);
   ~CPolyPolygon(void);
public:
   void Draw(CDC* pDC, CFill* pFillObject, int nLineWidth, COLORREF clLineColor, BOOL fLineColorExists, int nFillMode, BOOL fUsePalette = TRUE);
   // Simpler form of Draw. Pen and brush already set.
   void Draw(CDC* pDC);
public:
   BOOL UsePolyline(void)
      { return m_fUsePolyline; }
   void UsePolyline(BOOL fUsePolyline)
      { m_fUsePolyline = fUsePolyline; }
   BOOL Closed(void)
      { return m_fClosed; }
   void Close(void);
   void End(void);
   void MoveTo(long AX, long AY);
   void LineTo(long AX, long AY);
   void SplineTo(long AX, long AY, long BX, long BY);
   void BezierTo(long AX, long AY, long BX, long BY, long CX, long CY);
	void GetBoundingRect(CRect& rct);
	void GetBoundingBox(CPoint ptBox[], ANGLE dRot);
	void GetBoundingBox(CPoint ptBox[], ANGLE dRot, long& nWidth, long& nHeight);
	void GetBoundingBox(CPoint ptBox[], ANGLE dRot, int& nFirstCorner);
	void GetBoundingCircle(CPoint& ptCenter, long& nRadius);
protected:
   void StartContour(long AX, long AY);
   void EndContour(BOOL fClose);
   BOOL ContinueContour(void);
   void ConvertSpline(long AX, long AY, long BX, long BY, long CX, long CY);
   void ConvertBezier(long AX, long AY, long BX, long BY, long CX, long CY, long DX, long DY);
protected:
   CPolyPolygonPointArray m_Points;
   CPolyPolygonCountArray m_Counts;
   WORD m_nContoursInPath;
   WORD m_nPointsInContour;
   BOOL m_fFirstPoint;
   long m_FirstPointX;
   long m_FirstPointY;
   long m_LastPointX;
   long m_LastPointY;
   CDibPatternBrush m_DibBrush;
	BOOL m_fClosed;
	long m_nBoundWidth, m_nBoundHeight;
	int m_nFirstCorner;
protected:
   LPPOINT Points(void)
      { return (LPPOINT)(m_Points.GetElement(0)); }
   LPINT Counts(void)
      { return (LPINT)(m_Counts.GetElement(0)); }
	WORD ContoursInPath()
		{ return m_nContoursInPath; }
   void AddPoint(int x, int y)
      { POINT p; p.x = x; p.y = y; AddPoint(p); }
   void AddPoint(POINT p)
      { m_Points.AddPoint(p); m_nPointsInContour++; }
   void AddCount(void)
      { m_Counts.AddCount(m_nPointsInContour); m_nPointsInContour = 0; m_nContoursInPath++; }
   static int RoundFixed(long f)
      { return (int)(short int)HIWORD(f+0x8000L); }
   CDibPatternBrush* Brush(void)
      { return &m_DibBrush; }

   void PolyPolyline(CDC* pDC);     // Draw a PolyPolyline.
   BOOL m_fUsePolyline;
};

#endif
