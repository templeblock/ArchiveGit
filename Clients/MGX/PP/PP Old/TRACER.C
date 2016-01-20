// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

//#define InRegion(v) (CD24((v)) < maxdistance)

#define MAX_POINTS 5000

static long maxdistance;
static int sR, sG, sB;
static BOOL FonMask;
static LPROC DoInRegion;
static LPROC DoGetPixel;
static LPROC DoPutPixel;
static BOOL fAbort;

/************************************************************************/
long CD24(v)
/************************************************************************/
long v;
{
long rError, gError, bError;
RGBS rgb;

frame_getpixelRGB(NULL, v, &rgb);

rError = abs((int)rgb.red - sR);
rError *= rError;
gError = abs((int)rgb.green - sG);
gError *= gError;
bError = abs((int)rgb.blue - sB);
bError *= bError;
return(rError+gError+bError);
}


/************************************************************************/
BOOL InRegion(v)
/************************************************************************/
long v;
{
return(CD24((v)) < maxdistance);
}


/************************************************************************/
BOOL GetaPixel(x, y, lpPixel)
/************************************************************************/
int x, y;
LPLONG lpPixel;
{
LPTR lpLine;

if (!INFILE(x, y))
	return(FALSE);
if ( !(lpLine = CachePtr(0, x, y, NO)) )
	return(FALSE);
frame_getpixel(lpLine, lpPixel);
return(TRUE);
}


/************************************************************************/
int ChangeDirection(Direction, Delta)
/************************************************************************/
int Direction, Delta;
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
BOOL GetNeighbor(P, neighbor, N, v)
/************************************************************************/
POINT P;
int neighbor;
LPPOINT N;
LPLONG v;
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

/************************************************************************/
int Tracer(x, y, iRect, GetPixelProc, PutPixelProc, InRegionProc, lpFirstPoint, lpFirstCode, max_points, DoDisplay, npoints)
/************************************************************************/
int x, y;
RECT iRect;
LPROC GetPixelProc, PutPixelProc, InRegionProc;
LPPOINT lpFirstPoint;
LPTR lpFirstCode;
int max_points;
BOOL DoDisplay;
LPINT npoints;
{
POINT PA, PB, PC, PS, to, from;
BOOL first, found;
long v, size_per_point;
LPPOINT lpPoint;
LPTR lpCode;
int S;
LPTR lpLine;
int times, distance;
int num_points;

DoGetPixel = GetPixelProc;
DoInRegion = InRegionProc;
DoPutPixel = PutPixelProc;

/* direct neighbors definition:  3 2 1
				 4 P 0
				 5 6 7  */

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

lpPoint = lpFirstPoint;
lpCode = lpFirstCode;
if (lpCode)
	*lpCode++ = 8;

PC = PA;
lpPoint->x = PC.x;
lpPoint->y = PC.y;
++lpPoint;
num_points = 1;
if (DoDisplay)
	{
	from.x = PC.x;
	from.y = PC.y;
	File2Display(&from.x, &from.y);
	}
fAbort = FALSE;
first = TRUE;
while ((PA.x != PC.x || PA.y != PC.y) || first)
	{
	if (/*AstralAbort() || */fAbort)
		{
		fAbort = TRUE;
		break;
		}
	found = FALSE;
	times = 0;
	while (!found && times < 3)
		{
//		if (AstralAbort())
//			{
//			fAbort = TRUE;
//			break;
//			}
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
	if (found)
		{
		++num_points;
		if (num_points > max_points)
			{
			fAbort = TRUE;
			}
		else	{
			lpPoint->x = PC.x;
			lpPoint->y = PC.y;
			++lpPoint;
			if (lpCode)
				{
				(*DoPutPixel)(PC.x, PC.y, v+1);
				*lpCode++ = S;
				}
			if (DoDisplay)
				{
				to.x = PC.x;
				to.y = PC.y;
				File2Display(&to.x, &to.y);
				Dline( 0, from.x, from.y, to.x, to.y, ON);
				from = to;
				}
			}
		}
	first = FALSE;
	}
*npoints = num_points;
if (fAbort)
	{
	if (num_points > max_points)
		return(2);
	else	return(1);
	}

return(0);
}

/************************************************************************/
int TraceHoles(iRect, GetPixelProc, PutPixelProc, InRegionProc, lpFirstPoint, lpFirstCode, max_points, DoDisplay, npoints)
/************************************************************************/
RECT iRect;
LPROC GetPixelProc, PutPixelProc, InRegionProc;
LPPOINT lpFirstPoint;
LPTR lpFirstCode;
int max_points;
BOOL DoDisplay;
LPINT npoints;
{
LPPOINT lpHPoint, lpPoint;
POINT P, T;
BYTE c, c0;
long valueA, valueB, valueC;
LPTR lpHCode, lpCode;
int p;
int num_points;

lpPoint = lpFirstPoint;
lpCode = lpFirstCode;
for (p = 0; p < *npoints; ++p)
	{
	P = *lpPoint++;
	c = *lpCode++;
	if (c == 8)  /* start point */
		{
		c0 = c;
		P = *lpPoint++;
		c = *lpCode++;
		}
	/* look for downward arc */
	if (c0 > 4 && c0 < 7 && c > 5 && c < 7)
		{
		/* scan right */
		T = P;
		while (TRUE)
			{
			if (!(*GetPixelProc)(T.x, P.y, (LPLONG)&valueA))
				break;
			if (!(*GetPixelProc)(T.x+1, P.y, (LPLONG)&valueB))
				break;
			if (!(*GetPixelProc)(T.x+2, P.y, (LPLONG)&valueC))
				break;
			if ((valueA == 0 && valueB == 1) ||
				(valueA == 0 && valueB == 2 && valueC == 0))
				{
				lpHPoint = lpFirstPoint + *npoints;
				lpHCode = lpFirstCode + *npoints;
				Tracer(T.x+1, T.y, iRect, GetPixelProc, PutPixelProc, InRegionProc, lpHPoint, lpHCode, max_points-*npoints, DoDisplay, &num_points);
				*npoints += num_points;
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
return(0);
}


/************************************************************************/
DoTracer(x, y)
/************************************************************************/
{
LPTR lpLine;
long v;
RGBS rgb;
RECT mRect, iRect;
int Frange;
int status;
int num_points;
int max_points;
int npoints[1];
LPPOINT lpFirstPoint = NULL;

max_points = MAX_POINTS;
while (lpFirstPoint == NULL)
	{
	lpFirstPoint = (LPPOINT)Alloc((long)max_points * (long)sizeof(POINT));
	if (!lpFirstPoint)
		{
		max_points /= 2;
		if (max_points < 3)
			break;
		}
	}
if (max_points < 3)
	{
	Print("not enough memory available for wonder wand");
	return;
	}
mRect.left = bound(x - 1, 0, lpImage->npix-1);
mRect.right = bound(x + 1, 0, lpImage->npix-1);
mRect.top = bound(y - 1, 0, lpImage->nlin-1);
mRect.bottom = bound(y + 1, 0, lpImage->nlin-1);
// Must not do a Display2File inside measure!!!
Measure(&mRect, &rgb,YES);
sR = (int)rgb.red;
sG = (int)rgb.green;
sB = (int)rgb.blue;
Frange = TOGRAY(Mask.WandRange);
maxdistance = ((long)Frange*(long)Frange) * 3;
iRect.top = iRect.left = 0;
iRect.right = lpImage->npix - 1;
iRect.bottom = lpImage->nlin - 1;
status = Tracer(x, y, iRect, GetaPixel, NULL, InRegion, lpFirstPoint, NULL, max_points, TRUE, &num_points);
if (status == 0)
	{
	npoints[0] = num_points;
	MaskUndraw(lpFirstPoint, npoints, 1, YES);
	SetNewMask(lpFirstPoint, npoints, 1, NO);
	Marquee(YES);
	}
FreeUp((LPTR)lpFirstPoint);
}
