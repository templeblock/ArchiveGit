//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes
static  int ChangeDirection( int Direction,int Delta );
static  int GetNeighbor( POINT P, int neighbor, LPPOINT N, LPLONG v );

#define MAX_POINTS 16384
#define MIN_POINTS 2

static long sqdistance;
static int sR, sG, sB;
static BOOL  FonMask;
static LPINMASKREGION DoInRegion;
static LPGETMASKPIXEL DoGetPixel;
static LPPUTMASKPIXEL DoPutPixel;
static BOOL  fAbort;
static int  iLastY = -1;
static LPTR lpLastLine = NULL;

/************************************************************************/
int Tracer(
/************************************************************************/
int 			x, 
int				y,
RECT 			iRect,
LPGETMASKPIXEL 	GetPixelProc,
LPPUTMASKPIXEL 	PutPixelProc,
LPINMASKREGION 	InRegionProc,
LPPOINT 		lpFirstPoint,
LPTR 			lpFirstCode,
int 			max_points,
BOOL 			DoDisplay,
LPINT 			npoints,
LPADDSHAPE		lpAddProc)
{
POINT PA, PB, PC, PS, LastP, to, from;
BOOL found, bHitStart;
long v, Lastv;
LPPOINT lpPoint;
LPTR lpCode;
int S, LastS, times, distance, num_points, status;

#ifdef DO_COLOR_DISPLAY
HPEN hOldPen, hRedPen;
static RGBS pens[6] = {255,0,0, 0,255,0, 0,0,255, 0,255,255, 255,0,255, 255,255,0};
static int thru = 0;
#endif

static BOOL Reverse[8][8] = {NO,NO,YES,YES,YES,YES,YES,NO,   // 0
			     NO,NO,NO,YES,YES,YES,YES,YES,   // 1
			     YES,NO,NO,NO,YES,YES,YES,YES,   // 2
			     YES,YES,NO,NO,NO,YES,YES,YES,   // 3
			     YES,YES,YES,NO,NO,NO,YES,YES,   // 4
			     YES,YES,YES,YES,NO,NO,NO,YES,   // 5
			     YES,YES,YES,YES,YES,NO,NO,NO,   // 6
			     NO,YES,YES,YES,YES,YES,NO,NO};  // 7

if (max_points < MIN_POINTS) // gotta have at least room for 2 points
	{
	Message( IDS_EMEMALLOC );
	return(2);
	}

*npoints = 0;

DoGetPixel = GetPixelProc;
DoInRegion = InRegionProc;
DoPutPixel = PutPixelProc;

// direct neighbors definition:  3 2 1
//								 4 P 0
//								 5 6 7

PA.x = x;
PA.y = y;
S = 6;
if (lpFirstCode)
	goto TextTracer;

/************************************************************************/
/* Step 0: Choose a point A in the contour such that its 4-neighbor is not in the set */								 
/************************************************************************/

found = FALSE;
distance = 30000;
/* try to go left to find point A */
for (PS.x = x, PS.y = y; PS.x >= iRect.left; --PS.x)
	{
	if (!GetNeighbor(PS, 4, &PB, &v) || !(*DoInRegion)(v))
		{
		if ((x-PS.x) < distance)
			{
			PA = PS;
			distance = x - PS.x;
			found = TRUE;
			S = 6;
			}
		break;
		}
	}

/* try to go right to find point A - neighbor 0 */
for (PS.x = x, PS.y = y; PS.x <= iRect.right; ++PS.x)
	{
	if (!GetNeighbor(PS, 0, &PB, &v) || !(*DoInRegion)(v))
		{
		if ((PS.x-x) < distance)
			{
			PA = PS;
			distance = PS.x - x;
			found = TRUE;
			S = 2;
			}
		break;
		}
	}

/* try to go up to find point A */
for (PS.x = x, PS.y = y; PS.y >= iRect.top; --PS.y)
	{
	if (!GetNeighbor(PS, 2, &PB, &v) || !(*DoInRegion)(v))
		{
		if ((y-PS.y) < distance)
			{
			PA = PS;
			distance = y - PS.y;
			found = TRUE;
			S = 4;
			}
		break;
		}
	}

/* try to go down to find point A */
for (PS.x = x, PS.y = y; PS.y <= iRect.bottom; ++PS.y)
	{
	if (!GetNeighbor(PS, 6, &PB, &v) || !(*DoInRegion)(v))
		{
		if ((PS.y-y) < distance)
			{
			PA = PS;
			distance = PS.y - y;
			found = TRUE;
			S = 0;
			}
		break;
		}
	}

if (!found)
	return(3);

TextTracer:

lpPoint = lpFirstPoint;
lpCode = lpFirstCode;

// setup start point
PC = PA;
if (lpCode)
	{
	(*DoGetPixel)(PC.x, PC.y, (LPLONG)&v);
	(*DoPutPixel)(PC.x, PC.y, v+1);
	*lpCode++ = 8;
	}
lpPoint->x = PC.x;
lpPoint->y = PC.y;
++lpPoint;
num_points = 1;
if (DoDisplay)
	{
#ifdef DO_COLOR_DISPLAY
	i = thru++ % 6;
	hRedPen = CreatePen( PS_SOLID, 1, RGB(pens[i].red, pens[i].green, pens[i].blue));
	hOldPen = SelectObject(Window.hDC, hRedPen);
#endif
	from.x = PC.x;
	from.y = PC.y;
//	dbg("from = %d %d", from.x, from.y);
	File2Display((LPINT)&from.x, (LPINT)&from.y);
	}
fAbort = FALSE;
LastS = S;
bHitStart = NO;
while (TRUE)
	{
	if (fAbort)
		break;
	found = FALSE;
	times = 0;
	while (!found && times < 3)
		{
		if (!lpFirstCode && CANCEL) // not doing text and ESC key
			{
			fAbort = TRUE;
			break;
			}
		if (GetNeighbor(PC, ChangeDirection(S,-1), &PB, &v) && (*DoInRegion)(v))
			{
			PC = PB;
			S = ChangeDirection(S, -2);
			found = TRUE;
			}
		else
		if (GetNeighbor(PC, S, &PB, &v) && (*DoInRegion)(v))
			{
			PC = PB;
			found = TRUE;
			}
		else
		if (GetNeighbor(PC, ChangeDirection(S,1), &PB, &v) && (*DoInRegion)(v))
			{
			PC = PB;
			found = TRUE;
			}
		else	S = ChangeDirection(S, 2);
		++times;
		}
	if (!found)
		{
//		dbg("breaking out");dbg(NULL);
		break;
		}
	if (found)
		{
		if (bHitStart && lpCode)
			{
			if (v != 1)
				break;
			bHitStart = NO;
			}
		++num_points;
		if (num_points > max_points)
			{
//			Message( IDS_EMEMALLOC );
			--num_points;
			fAbort = TRUE;
			}
		else
			{
			// Have we reversed direction?
			if (lpCode && num_points > 2 && Reverse[LastS][S])
				// yes, make it look like we visited again
				(*DoPutPixel)(LastP.x, LastP.y, Lastv+1);
			lpPoint->x = PC.x;
			lpPoint->y = PC.y;
			++lpPoint;
			if (lpCode)
				{
				(*DoPutPixel)(PC.x, PC.y, v+1);
				*lpCode++ = S;
				}
			LastP = PC;
			LastS = S;
			Lastv = v+1;
			if (DoDisplay)
				{
				to.x = PC.x;
				to.y = PC.y;
//				dbg("to = %d %d", to.x, to.y);
				File2Display((LPINT)&to.x, (LPINT)&to.y);
#ifndef DO_COLOR_DISPLAY
				Dline( 0, from.x, from.y, to.x, to.y, ON);
#else
//				for (dummy = 0; dummy < 30000; ++dummy);
//				for (dummy = 0; dummy < 30000; ++dummy);
//				for (dummy = 0; dummy < 30000; ++dummy);
				MoveToEx(Window.hDC, from.x, from.y, NULL);
				LineTo(Window.hDC, to.x, to.y);
#endif
				from = to;
				}
			}
		bHitStart = PC.x == PA.x && PC.y == PA.y;  // Hit start point?
		if (bHitStart && !lpCode)
			break;
		}
	}
if (num_points == 1)
	{
	++num_points;
	if (num_points > max_points)
		{
//		Message( IDS_EMEMALLOC );
		--num_points;
		fAbort = TRUE;
		}
	else
		{
		//	dbg("setting up another startpoint");dbg(NULL);
		// setup start point
		PC = PA;
		if (lpCode)
			{
			(*DoGetPixel)(PC.x, PC.y, (LPLONG)&v);
			(*DoPutPixel)(PC.x, PC.y, v+1);
			*lpCode = 0;
			}
		lpPoint->x = PC.x;
		lpPoint->y = PC.y;
	//	dbg("done setting up another startpoint");dbg(NULL);
		}
	}
#ifdef DO_COLOR_DISPLAY
if (DoDisplay)
	SelectObject(Window.hDC, hOldPen);
#endif
if (fAbort)
	{
	if ( DoDisplay )
		*npoints = num_points;
	if (num_points > max_points)
		return(2);
	else	return(1);
	}
*npoints = num_points;
if (lpAddProc)
	{
	status = (*lpAddProc)(lpFirstPoint, num_points);
	return(status);
	}
return(0);
}


/************************************************************************/
int TraceHoles(
/************************************************************************/
RECT 			iRect,
LPGETMASKPIXEL 	GetPixelProc,
LPPUTMASKPIXEL	PutPixelProc, 
LPINMASKREGION	InRegionProc,
LPPOINT 		lpFirstPoint,
LPTR 			lpFirstCode,
int 			max_points,
BOOL 			DoDisplay,
LPINT 			npoints,
LPADDSHAPE 		lpAddProc)
{
LPPOINT lpPoint, lpHPoint;
POINT P, T;
BYTE c, c0;
long valueA, valueB, valueC;
LPTR lpCode, lpHCode;
int p, status;
int num_points;

#define IGNORE 0
#define CLEAR 1

max_points -= *npoints;
status = 0;

lpPoint = lpFirstPoint;
lpCode = lpFirstCode;
// scan first to find all holes containing contours that
// have not been visited yet
for (p = 0; p < *npoints && !status; ++p)
	{
	P = *lpPoint++;
	c = *lpCode++;
	/* look for downward arc; skip over start point (c = 8)  */
	if (c0 >= 4 && c0 <= 7 && c >= 5 && c <= 7)
		{
		/* scan right */
		T = P;
		// Do a prepass scanning for 01 combinations first
		// since they are more important to get done first
		// since most of the time they share the same contour
		// with a contour that has already been visited
		// i.e. value > 1
		while (TRUE)
			{
			if (!(*GetPixelProc)(T.x, T.y, (LPLONG)&valueA))
				break;
			if (!(*GetPixelProc)(T.x+1, T.y, (LPLONG)&valueB))
				break;
			if (valueA == 0 && valueB == 1)
				{
				lpHPoint = lpFirstPoint + *npoints;
				lpHCode = lpFirstCode + *npoints;
				status = Tracer(T.x+1, T.y, iRect, GetPixelProc, PutPixelProc, InRegionProc, lpHPoint, lpHCode, max_points, DoDisplay, &num_points, lpAddProc);
				if (status != 2)
					{
					status = 0;
					*npoints += num_points;
					max_points -= num_points;
					}
				break;
				}
			else 
				{
				if (!(*GetPixelProc)(T.x+2, T.y, (LPLONG)&valueC))
					break;
				if ((valueA==0 && valueB>=2 && valueC==0) ||
				    (valueA==1 && valueB==2 && valueC==0))
					break;
				}
			++T.x;
			}
		}
	c0 = c;
	}

lpPoint = lpFirstPoint;
lpCode = lpFirstCode;
// do normal hole scan
for (p = 0; p < *npoints && !status; ++p)
	{
	P = *lpPoint++;
	c = *lpCode++;
	/* look for downward arc; skip over start point (c = 8)  */
	if (c0 >= 4 && c0 <= 7 && c >= 5 && c <= 7)
		{
		/* scan right */
		T = P;
		while (TRUE)
			{
			if (!(*GetPixelProc)(T.x, T.y, (LPLONG)&valueA))
				break;
			if (!(*GetPixelProc)(T.x+1, T.y, (LPLONG)&valueB))
				break;
			if (!(*GetPixelProc)(T.x+2, T.y, (LPLONG)&valueC))
				break;
			if ((valueA == 0 && valueB == 1) ||
				(valueA == 0 && valueB == 2 && valueC == 0))
				{
				lpHPoint = lpFirstPoint + *npoints;
				lpHCode = lpFirstCode + *npoints;
				status = Tracer(T.x+1, T.y, iRect, GetPixelProc, PutPixelProc, InRegionProc, lpHPoint, lpHCode, max_points, DoDisplay, &num_points, lpAddProc);
				if (status != 2)
					{
					status = 0;
					*npoints += num_points;
					max_points -= num_points;
					}
				break;
				}
			else if ((valueA == 0 && valueB > 2 && valueC == 0) ||
				 (valueA == 1 && valueB == 2 && valueC == 0))
				break;
			++T.x;
			}
		}
	c0 = c;
	}

return(status);
}

/************************************************************************/
static int ChangeDirection(
/************************************************************************/
int 	Direction,
int		Delta)
{
Direction += Delta;
if (Direction < 0)
	Direction = 8 + Direction;
else
if (Direction > 7)
	Direction = Direction - 8;
return(Direction);
}


/************************************************************************/
static BOOL GetNeighbor(
/************************************************************************/
POINT 	P,
int 	neighbor,
LPPOINT N,
LPLONG 	v)
{
BOOL status;

*N = P;
if (neighbor == 0)
	++N->x;
else if (neighbor == 1)
	{
	++N->x;
	--N->y;
	}
else if (neighbor == 2)
	{
	--N->y;
	}
else if (neighbor == 3)
	{
	--N->x;
	--N->y;
	}
else if (neighbor == 4)
	{
	--N->x;
	}
else if (neighbor == 5)
	{
	--N->x;
	++N->y;
	}
else if (neighbor == 6)
	++N->y;
else if (neighbor == 7)
	{
	++N->x;
	++N->y;
	}
else	return(FALSE);

status = (*DoGetPixel)(N->x, N->y, v);
return(status);
}

