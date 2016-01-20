/*=======================================================================*\

	POINTUTL.H - For basic point and bezier utilities

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#ifndef _POINTUTL_H_
#define _POINTUTL_H_

void SplitBezier(LPPOINT BezierIn, LPPOINT BezierOut1, LPPOINT BezierOut2, double T);
double BezierClosest(LPPOINT lpFoundPt, LPPOINT lpTestPt, LPPOINT Bezier);
void ComputeBezierHandles(LPPOINT lpLine, LPPOINT lpHandles1, LPPOINT lpHandles2);
void SplitLine(LPPOINT start, LPPOINT end, LPPOINT out1, LPPOINT out2);

inline LONG DistanceSq(LPPOINT a, LPPOINT b)
{
	int dx, dy;
	dx = (a->x-b->x);
	dy = (a->y-b->y);
	return(dx*(LONG)dx+dy*(LONG)dy);
}

inline LONG DistanceSq(LPPOINT a)
{
	return(a->x*(LONG)a->x+a->y*(LONG)a->y);
}

inline LONG DotProduct(LPPOINT a, LPPOINT b)
{
	return(a->x*(LONG)b->x+a->y*(LONG)b->y);
}

// p1 = p2-p3
inline void PtSub(POINT &p1,POINT &p2,POINT &p3)
{
	p1.x = p2.x-p3.x;
	p1.y = p2.y-p3.y;
}

inline BOOL PtEq(POINT &p1,POINT &p2) \
{
	return (p1.x == p2.x && p1.y == p2.y);
}


typedef struct _dpoint
{
	double	x;
	double	y;
} DPOINT;
typedef DPOINT FAR *LPDPOINT;


inline double DDistanceSq(LPDPOINT a, LPDPOINT b)
{
	double dx, dy;
	dx = (a->x-b->x);
	dy = (a->y-b->y);
	return(dx*dx+dy*dy);
}

inline double DDotProduct(LPDPOINT a, LPDPOINT b)
{
	return(a->x*b->x+a->y*b->y);
}

#endif // _POINTUTL_H_
