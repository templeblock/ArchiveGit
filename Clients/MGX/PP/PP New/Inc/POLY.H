#ifndef __poly_h__
#define __poly_h__
/*=======================================================================*\

	POLY.H - Bezier and line segments

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "pplist.h"
#include "imgutil.h"

#define BEZIER_IN 4
#define MAX_BEZIER_OUT 25
#define BEZIER_MARKER (INT_MAX-1)

// point type flags 
#define POLY_CURVE_BEGIN 	0x0001
#define POLY_CURVE_END 		0x0002
#define POLY_CURVE_HANDLE1 	0x0004
#define POLY_CURVE_HANDLE2 	0x0008
#define POLY_CURVE_HANDLE 	(POLY_CURVE_HANDLE1|POLY_CURVE_HANDLE2)
#define POLY_CURVE 		(POLY_CURVE_BEGIN|POLY_CURVE_END|POLY_CURVE_HANDLE)


typedef CPPList CPolyList;
typedef CPolyList FAR *LPCPolyList;
typedef CPolyList CEditPolyList;
typedef CEditPolyList FAR *LPCEditPolyList;

typedef BOOL (FAR *LPPOLYPROC)(LPPOINT lpPont, WORD wFlags, LONG lData);

class CPoly : public CPPRoot
{
public:
	CPoly();
	virtual ~CPoly();
	// query functions
	virtual BOOL IsA(LPTR lpClass);
	int NumPoints();
	int CurrentIndex();
	LONG PointType();
	BOOL IsCurve();
	WORD IsHandle();
	WORD IsCurveEndPoint();
	BOOL IsClosed();
	void ClosePoly(BOOL bClosed);
	// basic point functions
	LPPOINT GetPoint(int num, LPPOINT lpPoint = NULL);
	LPPOINT LastPoint(LPPOINT lpPoint = NULL);
	LPPOINT NextPoint(LPPOINT lpPoint = NULL);		// wraps if closed poly
	LPPOINT ListNextPoint(LPPOINT lpPoint = NULL);	// no wrap
	LPPOINT CurrentPoint(LPPOINT lpPoint);
	void SetCurrentPoint(LPPOINT lpPoint);
	LPPOINT PreviousPoint(LPPOINT lpPoint = NULL);
	virtual BOOL AddPoint(LPPOINT lpPoint);
	virtual BOOL DeletePoint();
	virtual BOOL InsertPoint(LPPOINT lpPoint);
	// bezier functions
	virtual BOOL AddBezierLine(LPPOINT lpHandle1, LPPOINT lpHandle2, LPPOINT lpEnd);
	virtual BOOL AddBezierLine2(LPPOINT lpStart, LPPOINT lpHandle1, LPPOINT lpHandle2);
	virtual int LineToBezier(LPPOINT lpHandle1, LPPOINT lpHandle2);
	virtual BOOL BezierToLine();
	// advanced and helper functions
	virtual BOOL Copy(class CPoly FAR* lpPoly);
	WORD GetCurveData();
	void SetCurveData(WORD wData);
	BOOL ForEach(LPPOLYPROC lpProc, LONG lData);
	LPPOINT GetData(int iPointsToUse, LPTFORM lpTForm, LPINT lpiOut, LPPOINT lpStorage = NULL, LPINT lpiStorage = NULL);
	int SizeData(int iPointsToUse, LPTFORM lpTForm);
	void GetBounds(LPRECT rBounds);
	virtual void Squeeze();
	virtual void Squeeze(int Tolerance);

protected:
	int Num;						// points+handles (markers not included)
	int CurPoint;					// What is the current point number
private:
	int get_curve_index(int index);
	BOOL poly_make_room(int needed);
	void fix_cur_point();
	int CurPointPos;				// What is the current point index from lpPoints
	LPPOINT lpPoints;				// The Point List
	BOOL bClosed;					// Is it a closed shape?
//	POINTTYPE CurrentPointType;		// Current points type
//	int BezierOffset;				// how far back is the marker? (besier points only)
	int EntriesUsed;				// How many entried in lpPoints (include markers)
	int EntriesAllocated;			// How many entries allocated
};

typedef CPoly FAR *LPCPoly;

// Poly Helper routines 
BOOL PolySqueeze(LPCPoly lpPoly);
BOOL PolyTForm(LPCPoly, LPTFORM lpTForm);
LPCPoly MakeRectPoly(LPCPoly lpPoly, LPRECT lpRect, BOOL bExpand = NO);
BOOL PreviousControlPoint(LPCPoly lpPoly, LPPOINT lpPoint = NULL);	 
BOOL NextControlPoint(LPCPoly lpPoly, LPPOINT lpPoint = NULL);
int CurveToPoints(LPPOINT lpPoints, int type, LPPOINT lpOutputPoints );


// PolyList Helper routines 
void FreePolyList(LPCPolyList lpPolyList);
void FreePolys(LPCPolyList lpPolyList);
BOOL PolyListGetBounds(LPCPolyList lpList, LPRECT lpRect);
#endif /* __poly_h__ */
