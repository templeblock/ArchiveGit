//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"


//#define DO_COLOR_DISPLAY 1

// Static prototypes
static BOOL GetNeighbor( POINT P, int neighbor, LPPOINT N);

#define MAX_POINTS 16384
#define MIN_POINTS 2

static long sqdistance;
static int sR, sG, sB;
static BOOL  FonMask;
static LPGETPIXELPROC DoGetPixel;
static LPPUTPIXELPROC DoPutPixel;
static BOOL  fAbort;
static int  iLastY = -1;
static LPTR lpLastLine = NULL;

/************************************************************************/
inline int ChangeDirection( int Direction, int Delta )
/************************************************************************/
{
	Direction += Delta;
	if (Direction < 0)
		return( 8 + Direction);
	else if (Direction > 7)
		return(Direction - 8);
	return(Direction);
}

inline BOOL MoveToNeighbor( POINT P, int neighbor, LPPOINT N)
{
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
	else	
		return(FALSE);
return(TRUE);
}


/************************************************************************/
int Tracer( int x, int y, RECT iRect,
				LPGETPIXELPROC GetPixelProc,
				LPPUTPIXELPROC PutPixelProc,
				LPPOINT lpFirstPoint,
				int max_points, BOOL DoDisplay, LPINT npoints,
				LPADDPROC lpAddProc )
/************************************************************************/
{                                	
	POINT PA, PB, PC, to, from;
	BOOL found, bHitStart;
	long v;
	LPPOINT lpPoint;
	int i,S,times, num_points, status;
	int FoundS;

	#ifdef DO_COLOR_DISPLAY
	HPEN hOldPen, hRedPen;
	static RGBS pens[6] = {255,0,0, 0,255,0, 0,0,255, 0,255,255, 255,0,255, 255,255,0};
	static int thru = 0;
	#endif


	if (max_points < MIN_POINTS) // gotta have at least room for 2 points
	{
		Message( IDS_EMEMALLOC );
		return(2);
	}

	*npoints = 0;

	DoGetPixel = GetPixelProc;
	DoPutPixel = PutPixelProc;

	// direct neighbors definition:  3 2 1
	//								 4 P 0
	//								 5 6 7

	PA.x = x;
	PA.y = y;
	S = 6;

	lpPoint = lpFirstPoint;

	// setup start point
	PC = PA;
	(*DoGetPixel)(PC.x, PC.y, &v);
	(*DoPutPixel)(PC.x, PC.y, v+1);
	lpPoint->x = PC.x;
	lpPoint->y = PC.y;
	++lpPoint;
	num_points = 1;
	if (DoDisplay)
	{
	#ifdef DO_COLOR_DISPLAY
		i = thru++ % 6;
		hRedPen = CreatePen( PS_SOLID, 1, RGB(pens[i].red, pens[i].green, pens[i].blue));
		hOldPen = (HPEN)SelectObject(Window.hDC, hRedPen);
	#endif
		from.x = PC.x-1;
		from.y = PC.y-1;
	//	dbg("from = %d %d", from.x, from.y);
		File2Display(GetActiveDoc(), (LPINT)&from.x, (LPINT)&from.y);
	}
	fAbort = FALSE;
	bHitStart = NO;
	while (TRUE)
	{
		if (fAbort)
			break;
		found = FALSE;
		times = 0;
		while (!found && times < 3)
		{
			if (CANCEL) // not doing text and ESC key
			{
				fAbort = TRUE;
				break;
			}
			if (GetNeighbor(PC, FoundS = ChangeDirection(S,-2), &PB))
			{
				PC = PB;
				S = FoundS;
				found = TRUE;
			}
			else
			if (GetNeighbor(PC, FoundS = S, &PB))
			{
				PC = PB;
				found = TRUE;
			}
			else	
				S = ChangeDirection(S, 2);
			++times;
		}
		if (!found)
		{
	//		dbg("breaking out");dbg(NULL);
			break;
		}
		if (bHitStart)
			break;
		++num_points;
		if (num_points > max_points)
		{
	//		Message( IDS_EMEMALLOC );
			MessageBeep(-1);
			--num_points;
			fAbort = TRUE;
			break;
		}
		lpPoint->x = PC.x;
		lpPoint->y = PC.y;
		++lpPoint;
		// make it known that we have been here 
		switch (FoundS)
		{
			case 0:
			case 1:
				(*DoPutPixel)(PC.x-1, PC.y-1, 2/*v+1*/);
			break;
			case 2:
			case 3:
				(*DoPutPixel)(PC.x-1, PC.y, 2/*v+1*/);
			break;
			case 4:
			case 5:
				(*DoPutPixel)(PC.x, PC.y, 2/*v+1*/);
			break;
			case 6:
			case 7:
				(*DoPutPixel)(PC.x, PC.y-1, 2/*v+1*/);
			break;
		}
		
		if (DoDisplay)
		{
			to.x = PC.x-1;
			to.y = PC.y-1;
	//		dbg("to = %d %d", to.x, to.y);
			File2DisplayEx(GetActiveDoc(), (LPINT)&to.x, (LPINT)&to.y, NO);
	#ifndef DO_COLOR_DISPLAY
			Dline( 0, from.x, from.y, to.x, to.y, ON);
	#else
		 	{
	//			int dummy;
	//			for (dummy = 0; dummy < 30000; ++dummy);
	//			for (dummy = 0; dummy < 30000; ++dummy);
	//			for (dummy = 0; dummy < 30000; ++dummy);
			}            
			MoveTo(Window.hDC, from.x, from.y);
			LineTo(Window.hDC, to.x, to.y);
	#endif
			from = to;
		}
		bHitStart = PC.x == PA.x && PC.y == PA.y;  // Hit start point?
	}
	#ifdef DO_COLOR_DISPLAY
	if (DoDisplay)
	{
		SelectObject(Window.hDC, hOldPen);
	}
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
// P is the point to ceck
// R is the 'real point to check
/************************************************************************/
static BOOL GetNeighbor( POINT P, int neighbor, LPPOINT N )
/************************************************************************/
{

	// what should the world look like AFTER the move
	// The 1'st element is the pixel which must be on
	// The 2'nd element is the pixel which must be off
	// -1 is for the pixel itself
	static BOOL DLook[4][2] = {
			3,4,//0
			4,-1,//2
			-1,2,//4
			2,3,};//6
	POINT NR;
	long l;
	int index;

	// chenck the actual point
	MoveToNeighbor( P, neighbor, N);
	if (!(*DoGetPixel)(N->x, N->y, &l))
		l = 0;
	index = neighbor/2;
	if (!l && DLook[index][0] == -1)
		return(FALSE);	// we needed to be on
	if (l && DLook[index][1] == -1)
		return(FALSE); 	// we needed to be off
	if (!l)
	{
		// check above, to the left & aboveleft
		if 	((!(*DoGetPixel)(N->x  , N->y-1, &l) || !l) &&
			 (!(*DoGetPixel)(N->x-1, N->y  , &l) || !l) &&
			 (!(*DoGetPixel)(N->x-1, N->y-1, &l) || !l))
			return(FALSE);
	}
	// the point is OK, check state
					
	// check the pixel which must be on	
	if ((l=DLook[index][0]) != -1)
	{				
		MoveToNeighbor(*N, l, &NR);
		if (!(*DoGetPixel)(NR.x, NR.y, &l) || !l)
			return(FALSE);
	}
	// check the pixel which must be off
	if ((l=DLook[index][1]) != -1)
	{				
		MoveToNeighbor(*N, l, &NR);
		if ((*DoGetPixel)(NR.x, NR.y, &l) && l)
			return(FALSE);
	}
	return(TRUE);
}

