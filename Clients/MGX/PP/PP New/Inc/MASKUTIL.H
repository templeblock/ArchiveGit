//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _MASKUTIL_H_
#define _MASKUTIL_H_

#include "mask.h"
#include "pixmap.h"
#include "poly.h"

/*==========================================================*\
	Shape structure definition
\*==========================================================*/

//typedef struct _shape
//{
//	int		iModes; 	// Add?, Subtract?, Reverse?, Circle?
////	LPCPoly	lpPoly;		// The polygon
//	RECT    rShape;
//	TFORM   tform;		// for output only
////	BOOL	bClosed;
//} SHAPE, far *LPSHAPE;
//typedef	LPSHAPE far *LPPSHAPE;

// Shape Mode Constants
#define SHAPE_REVERSE	1
#define SHAPE_ADD		2
#define SHAPE_SUBTRACT	3
#define SHAPE_DEFAULT SHAPE_ADD

// max output points for expanded bezier
#define MAX_BEZIER 25

// max input points for bezier (controll + end points)
#define MAX_BEZIER_IN 5
#define BEZIER_MARKER (INT_MAX-1)
#define NEWPOLY_MARKER (INT_MAX-2)

#ifdef USING_FIXED16
#define FBEZIER_MARKER (INT_MAX-1)*UNITY
#define FNEWPOLY_MARKER (INT_MAX-2)*UNITY
#else
#define FBEZIER_MARKER (LONG_MAX-1)
#define FNEWPOLY_MARKER (LONG_MAX-2)
#endif

#define	IS_BEZIER(lpPoint) \
	 (lpPoint->x == BEZIER_MARKER)

// points in bezier structur itsself
#define BEZIER_POINTS(lpMarker) \
	lpMarker->y

BOOL MaskAddPoly( LPMASK lpMask, LPCPolyList lpPolyList, int iMode, BOOL bAntiAlias);
BOOL MaskAddPoly( LPMASK lpMask, LPCPoly lpPoly, int iMode, BOOL bAntiAlias);
void MaskAddEllipse( LPMASK lpMask, LPRECT lpRect, int iModes);
LPFRAME StrokeEllipse( LPTR hMgxBrush, LPRECT lpEllipseRect, LPRECT lpOutRect);
int ExpandPoints(LPPOINT lpPoint, LPPOINT lpOutputPoint, int count, LPTFORM lpTForm, LPPOINT lpFirst, LPPOINT lpLast);
int GetBezierPoints(LPPOINT lpOutPoints, LPPOINT lpBezPoint, LPPOINT lpFirst, LPPOINT lpLast);
int BezierToPoints( LPTFORM lpTForm, LPPOINT lpPoints, int nPoints, int type, LPPOINT lpOutputPoints);
#endif // _MASKUTIL_H_


