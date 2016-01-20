// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/* Directional flags for automasking */
#define N	1
#define NE	2
#define E	4
#define SE	8
#define S	16
#define SW	32
#define W	64
#define NW	128
#define NOT_N	SW|S|SE
#define NOT_NE	W|SW|S
#define NOT_E	NW|W|SW
#define NOT_SE	W|NW|N
#define NOT_S	NW|N|NE
#define NOT_SW	N|NE|E
#define NOT_W	NE|E|SE
#define NOT_NW	E|SE|S

static BOOL fAbortAutoMask;

/************************************************************************/
void automask(x, y, lpVecptr, lpVeccnt, lpMx, lpMy, lpDx, lpDy)
/************************************************************************/
int x, y;
LPPPOINT lpVecptr;
LPINT lpVeccnt, lpMx, lpMy, lpDx, lpDy;
{
int dx, dy, mx, my, x1, x2, y1, y2;
int lmx, lmy, mvx, mvy, fdx, fdy, fx, fy, fmx, fmy;
int Direction, LastDirection, MinVectorSize;
LPPOINT vecptr;
int veccnt;
fAbortAutoMask = FALSE;
    
vecptr = *lpVecptr;
veccnt = *lpVeccnt;
dx = *lpDx;
dy = *lpDy;
mx = *lpMx;
my = *lpMy;

/* Undraw the guide line ... */
Dline( 0, mx, my, dx, dy, OFF );
fmx = fx = x;
fmy = fy = y;
if (veccnt)
	{
	fmx = (vecptr-1)->x;
	fmy = (vecptr-1)->y;
	}
mx = fmx;
my = fmy;
File2Display(&mx, &my);
	
/* Compute how far to the end point */
fdx = fx - fmx;
fdy = fy - fmy;
Direction = GoingAway( fdx, fdy );
LastDirection = direction( fdx, fdy );
MinVectorSize = Mask.AutoMinEdge;
lmx = lmy = 9999;
while ( (abs(fdx) > 3 || abs(fdy) > 3) && !fAbortAutoMask)
	{
	Direction = frame_edgevalue( fmx, fmy, Direction, fdx, fdy,
				     Mask.AutoSearchDepth );
	if ( !Direction )
	        Direction = LastDirection;
	else 
		LastDirection = Direction;
		    
	if ( Direction == N )
		{ fmy--; fdy++;            Direction = NOT_N; } 
	else
	if ( Direction == NE )
		{ fmy--; fdy++; fmx++; fdx--; Direction = NOT_NE; } 
	else
	if ( Direction == E )
		{ fmx++; fdx--; Direction = NOT_E; } 
	else
	if ( Direction == SE )
		{ fmy++; fdy--; fmx++; fdx--; Direction = NOT_SE; } 
	else
	if ( Direction == S )
		{ fmy++; fdy--;            Direction = NOT_S; } 
	else
	if ( Direction == SW )
		{ fmy++; fdy--; fmx--; fdx++; Direction = NOT_SW; } 
	else
	if ( Direction == W )
		{ fmx--; fdx++; Direction = NOT_W; } 
	else
	if ( Direction == NW )
		{ fmy--; fdy++; fmx--; fdx++; Direction = NOT_NW; }

	fmx = bound(fmx, 0, lpImage->npix-1);
       	fmy = bound(fmy, 0, lpImage->nlin-1);
	if ( --MinVectorSize > 0 )
		continue;

	mx = fmx;
	my = fmy;
	File2Display(&mx, &my);
		    
	if (fmx != (vecptr-1)->x || fmy != (vecptr-1)->y)
		{
		mvx = fmx - (vecptr-1)->x;
		mvy = fmy - (vecptr-1)->y;
		    
		if ( mvx == lmx && mvy == lmy )
			{ /* Replace the last vector */
			if (veccnt > 1)
				{
				x1 = (vecptr-2)->x; y1 = (vecptr-2)->y;
				x2 = (vecptr-1)->x; y2 = (vecptr-1)->y;
				File2Display(&x1, &y1);
				File2Display(&x2, &y2);
           			Dline(0, x1, y1, x2, y2, ON );
				}
			x1 = (vecptr-2)->x; y1 = (vecptr-2)->y;
			File2Display(&x1, &y1);
		        Dline(0, x1, y1, mx, my, ON );
			(vecptr-1)->x = fmx;
			(vecptr-1)->y =  fmy;
			}
		else 
			{ /* Capture the new vector */
			x1 = (vecptr-1)->x; y1 = (vecptr-1)->y;
			File2Display(&x1, &y1);
		        Dline(0, x1, y1, mx, my, ON );
		        /* If the new vector extends the last one, replace it */
			lmx = mvx;
			lmy = mvy;
			vecptr->x = fmx;
			vecptr->y = fmy;
			vecptr++;
			veccnt++;
			}
		}
	}

/* Draw a new guide line */
dx = x;
dy = y;
mx = (vecptr-1)->x;
my = (vecptr-1)->y;
File2Display(&dx, &dy);
File2Display(&mx, &my);
Dline( 0, mx, my, dx, dy, ON );

*lpVecptr = vecptr;
*lpVeccnt = veccnt;
*lpDx = dx;
*lpDy = dy;
*lpMx = mx;
*lpMy = my;
}
    
/************************************************************************/
long EdgeValue8( x, y, FromDir, dx, dy, depth )
/************************************************************************/
int x, y, FromDir, dx, dy, depth;
{
int DontGo, Value, Direction, NewValue, Go1, Go2;
LPTR lpLine1, lpLine2;

//if (fAbortAutoMask || AstralAbort())
if (fAbortAutoMask)
    {
    fAbortAutoMask = TRUE;
    return(0);
    }
DontGo = FromDir | GoingAway( dx, dy );
Value = 0;
Direction = 0;

if ( !(DontGo & N) )
	{
	if (INFILE(x-1, y-1) && INFILE(x+1, y-1))
	    {
	    lpLine1 = CachePtr(0, x-1, y-1, NO);
	    if (lpLine1)
	        NewValue = ( (int)*lpLine1 - (int)*(lpLine1+2));  /* x-1 and x+1 */
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	NewValue = abs(NewValue);
	if ( depth )
		NewValue += EdgeValue8( x, y-1, NOT_N, dx, dy+1, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = N;
		}
	else
	if ( NewValue == Value )
		Direction |= N;
	}
if ( !(DontGo & NE) )
	{
	if (INFILE(x, y-1) && INFILE(x+1, y))
	    {
	    lpLine1 = CachePtr(0, x, y-1, NO);
	    lpLine2 = CachePtr(0, x+1, y, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = ( (int)*lpLine1 - (int)*lpLine2 );
	    else
	        NewValue = 1;
	    }
    else
	    NewValue = 1;
	NewValue = abs(NewValue);
	if ( depth )
		NewValue += EdgeValue8( x+1, y-1, NOT_NE, dx-1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NE;
		}
	else
	if ( NewValue == Value )
		Direction |= NE;
	}
if ( !(DontGo & E) )
	{
	if (INFILE(x+1, y-1) && INFILE(x+1, y+1))
	    {
	    lpLine1 = CachePtr(0, x+1, y-1, NO);
	    lpLine2 = CachePtr(0, x+1, y+1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = ( (int)*lpLine1 - (int)*lpLine2 );
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	NewValue = abs(NewValue);
	if ( depth )
		NewValue += EdgeValue8( x+1, y, NOT_E, dx-1, dy, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = E;
		}
	else
	if ( NewValue == Value )
		Direction |= E;
	}
if ( !(DontGo & SE) )
	{
	if (INFILE(x+1, y) && INFILE(x, y+1))
	    {
	    lpLine1 = CachePtr(0, x+1, y, NO);
	    lpLine2 = CachePtr(0, x, y+1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = ( (int)*lpLine1 - (int)*lpLine2 );
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	NewValue = abs(NewValue);
	if ( depth )
		NewValue += EdgeValue8( x+1, y+1, NOT_SE, dx-1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SE;
		}
	else
	if ( NewValue == Value )
		Direction |= SE;
	}
if ( !(DontGo & S) )
	{
	if (INFILE(x+1, y+1) && INFILE(x-1, y+1))
	    {
	    lpLine1 = CachePtr(0, x+1, y+1, NO);
	    lpLine2 = CachePtr(0, x-1, y+1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = ( (int)*lpLine1 - (int)*lpLine2 );
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	NewValue = abs(NewValue);
	if ( depth )
		NewValue += EdgeValue8( x, y+1, NOT_S, dx, dy-1, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = S;
		}
	else
	if ( NewValue == Value )
		Direction |= S;
	}
if ( !(DontGo & SW) )
	{
	if (INFILE(x, y+1) && INFILE(x-1, y))
	    {
	    lpLine1 = CachePtr(0, x, y+1, NO);
	    lpLine2 = CachePtr(0, x-1, y, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = ( (int)*lpLine1 - (int)*lpLine2 );
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	NewValue = abs(NewValue);
	if ( depth )
		NewValue += EdgeValue8( x-1, y+1, NOT_SW, dx+1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SW;
		}
	else
	if ( NewValue == Value )
		Direction |= SW;
	}
if ( !(DontGo & W) )
	{
	if (INFILE(x-1, y+1) && INFILE(x-1, y-1))
	    {
	    lpLine1 = CachePtr(0, x-1, y+1, NO);
	    lpLine2 = CachePtr(0, x-1, y-1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = ( (int)*lpLine1 - (int)*lpLine2 );
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	NewValue = abs(NewValue);
	if ( depth )
		NewValue += EdgeValue8( x-1, y, NOT_W, dx+1, dy, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = W;
		}
	else
	if ( NewValue == Value )
		Direction |= W;
	}
if ( !(DontGo & NW) )
	{
	if (INFILE(x, y-1) && INFILE(x-1, y))
	    {
	    lpLine1 = CachePtr(0, x, y-1, NO);
	    lpLine2 = CachePtr(0, x-1, y, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = ( (int)*lpLine1 - (int)*lpLine2 );
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	NewValue = abs(NewValue);
	if ( depth )
		NewValue += EdgeValue8( x-1, y-1, NOT_NW, dx+1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NW;
		}
	else
	if ( NewValue == Value )
		Direction |= NW;
	}

if ( depth == Mask.AutoSearchDepth )
	{ /* The search is over */
	/* Break any ties by heading for the destination */
	Go1 = direction( dx, dy );
	if ( Value < 1  ) return( 0 );
	Go2 = Go1;			  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	}
else	return( Value );
}


/************************************************************************/
long Distance16(v1, v2)
/************************************************************************/
    WORD v1, v2;
    {
    long rError, gError, bError;
    
    rError = abs((int)MaxiR(v1)-(int)MaxiR(v2));
    rError *= rError;
    gError = abs((int)MaxiG(v1)-(int)MaxiG(v2));
    gError *= gError;
    bError = abs((int)MaxiB(v1)-(int)MaxiB(v2));
    bError *= bError;
    return(rError+gError+bError);
    }
    
/************************************************************************/
long Distance24(lpV1, lpV2)
/************************************************************************/
    LPTR lpV1, lpV2;
    {
    long rError, gError, bError;
    
    rError = abs((int)*lpV1++ - (int)*lpV2++);
    rError *= rError;
    gError = abs((int)*lpV1++ - (int)*lpV2++);
    gError *= gError;
    bError = abs((int)*lpV1 - (int)*lpV2);
    bError *= bError;
    return(rError+gError+bError);
    }
    
/************************************************************************/
long EdgeValue16( x, y, FromDir, dx, dy, depth )
/************************************************************************/
int x, y, FromDir, dx, dy, depth;
{
int DontGo, Direction, Go1, Go2;
long NewValue, Value;
LPWORD lpLine1, lpLine2;

//if (fAbortAutoMask || AstralAbort())
if (fAbortAutoMask)
    {
    fAbortAutoMask = TRUE;
    return(0);
    }
DontGo = FromDir | GoingAway( dx, dy );
Value = 0;
Direction = 0;

if ( !(DontGo & N) )
	{
	if (INFILE(x-1, y-1) && INFILE(x+1, y-1))
	    {
	    lpLine1 = (LPWORD)CachePtr(0, x-1, y-1, NO);
	    if (lpLine1)
	        NewValue = Distance16(*lpLine1, *(lpLine1+2));  /* x-1 and x+1 */
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue16( x, y-1, NOT_N, dx, dy+1, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = N;
		}
	else
	if ( NewValue == Value )
		Direction |= N;
	}
if ( !(DontGo & NE) )
	{
	if (INFILE(x, y-1) && INFILE(x+1, y))
	    {
	    lpLine1 = (LPWORD)CachePtr(0, x, y-1, NO);
	    lpLine2 = (LPWORD)CachePtr(0, x+1, y, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance16(*lpLine1, *lpLine2);
	    else
	        NewValue = 1;
	    }
    else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue16( x+1, y-1, NOT_NE, dx-1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NE;
		}
	else
	if ( NewValue == Value )
		Direction |= NE;
	}
if ( !(DontGo & E) )
	{
	if (INFILE(x+1, y-1) && INFILE(x+1, y+1))
	    {
	    lpLine1 = (LPWORD)CachePtr(0, x+1, y-1, NO);
	    lpLine2 = (LPWORD)CachePtr(0, x+1, y+1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance16(*lpLine1, *lpLine2);
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue16( x+1, y, NOT_E, dx-1, dy, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = E;
		}
	else
	if ( NewValue == Value )
		Direction |= E;
	}
if ( !(DontGo & SE) )
	{
	if (INFILE(x+1, y) && INFILE(x, y+1))
	    {
	    lpLine1 = (LPWORD)CachePtr(0, x+1, y, NO);
	    lpLine2 = (LPWORD)CachePtr(0, x, y+1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance16(*lpLine1, *lpLine2);
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue16( x+1, y+1, NOT_SE, dx-1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SE;
		}
	else
	if ( NewValue == Value )
		Direction |= SE;
	}
if ( !(DontGo & S) )
	{
	if (INFILE(x+1, y+1) && INFILE(x-1, y+1))
	    {
	    lpLine1 = (LPWORD)CachePtr(0, x+1, y+1, NO);
	    lpLine2 = (LPWORD)CachePtr(0, x-1, y+1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance16(*lpLine1, *lpLine2);
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue16( x, y+1, NOT_S, dx, dy-1, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = S;
		}
	else
	if ( NewValue == Value )
		Direction |= S;
	}
if ( !(DontGo & SW) )
	{
	if (INFILE(x, y+1) && INFILE(x-1, y))
	    {
	    lpLine1 = (LPWORD)CachePtr(0, x, y+1, NO);
	    lpLine2 = (LPWORD)CachePtr(0, x-1, y, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance16(*lpLine1, *lpLine2);
 	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue16( x-1, y+1, NOT_SW, dx+1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SW;
		}
	else
	if ( NewValue == Value )
		Direction |= SW;
	}
if ( !(DontGo & W) )
	{
	if (INFILE(x-1, y+1) && INFILE(x-1, y-1))
	    {
	    lpLine1 = (LPWORD)CachePtr(0, x-1, y+1, NO);
	    lpLine2 = (LPWORD)CachePtr(0, x-1, y-1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance16(*lpLine1, *lpLine2);
 	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue16( x-1, y, NOT_W, dx+1, dy, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = W;
		}
	else
	if ( NewValue == Value )
		Direction |= W;
	}
if ( !(DontGo & NW) )
	{
	if (INFILE(x, y-1) && INFILE(x-1, y))
	    {
	    lpLine1 = (LPWORD)CachePtr(0, x, y-1, NO);
	    lpLine2 = (LPWORD)CachePtr(0, x-1, y, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance16(*lpLine1, *lpLine2);
 	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue16( x-1, y-1, NOT_NW, dx+1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NW;
		}
	else
	if ( NewValue == Value )
		Direction |= NW;
	}

if ( depth == Mask.AutoSearchDepth )
	{ /* The search is over */
	/* Break any ties by heading for the destination */
	Go1 = direction( dx, dy );
	if ( Value < 1  ) return( 0 );
	Go2 = Go1;			  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	}
else	return( Value );
}

/************************************************************************/
long EdgeValue24( x, y, FromDir, dx, dy, depth )
/************************************************************************/
int x, y, FromDir, dx, dy, depth;
{
int DontGo, Direction, Go1, Go2;
long NewValue, Value;
LPTR lpLine1, lpLine2;

//if (fAbortAutoMask || AstralAbort())
if (fAbortAutoMask)
    {
    fAbortAutoMask = TRUE;
    return(0);
    }
DontGo = FromDir | GoingAway( dx, dy );
Value = 0;
Direction = 0;

if ( !(DontGo & N) )
	{
	if (INFILE(x-1, y-1) && INFILE(x+1, y-1))
	    {
	    lpLine1 = CachePtr(0, x-1, y-1, NO);
	    if (lpLine1)
	        NewValue = Distance24(lpLine1, lpLine1+6);  /* x-1 and x+1 */
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue24( x, y-1, NOT_N, dx, dy+1, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = N;
		}
	else
	if ( NewValue == Value )
		Direction |= N;
	}
if ( !(DontGo & NE) )
	{
	if (INFILE(x, y-1) && INFILE(x+1, y))
	    {
	    lpLine1 = CachePtr(0, x, y-1, NO);
	    lpLine2 = CachePtr(0, x+1, y, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance24(lpLine1, lpLine2);
	    else
	        NewValue = 1;
	    }
    else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue24( x+1, y-1, NOT_NE, dx-1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NE;
		}
	else
	if ( NewValue == Value )
		Direction |= NE;
	}
if ( !(DontGo & E) )
	{
	if (INFILE(x+1, y-1) && INFILE(x+1, y+1))
	    {
	    lpLine1 = CachePtr(0, x+1, y-1, NO);
	    lpLine2 = CachePtr(0, x+1, y+1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance24(lpLine1, lpLine2);
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue24( x+1, y, NOT_E, dx-1, dy, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = E;
		}
	else
	if ( NewValue == Value )
		Direction |= E;
	}
if ( !(DontGo & SE) )
	{
	if (INFILE(x+1, y) && INFILE(x, y+1))
	    {
	    lpLine1 = CachePtr(0, x+1, y, NO);
	    lpLine2 = CachePtr(0, x, y+1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance24(lpLine1, lpLine2);
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue24( x+1, y+1, NOT_SE, dx-1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SE;
		}
	else
	if ( NewValue == Value )
		Direction |= SE;
	}
if ( !(DontGo & S) )
	{
	if (INFILE(x+1, y+1) && INFILE(x-1, y+1))
	    {
	    lpLine1 = CachePtr(0, x+1, y+1, NO);
	    lpLine2 = CachePtr(0, x-1, y+1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance24(lpLine1, lpLine2);
	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue24( x, y+1, NOT_S, dx, dy-1, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = S;
		}
	else
	if ( NewValue == Value )
		Direction |= S;
	}
if ( !(DontGo & SW) )
	{
	if (INFILE(x, y+1) && INFILE(x-1, y))
	    {
	    lpLine1 = CachePtr(0, x, y+1, NO);
	    lpLine2 = CachePtr(0, x-1, y, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance24(lpLine1, lpLine2);
 	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue24( x-1, y+1, NOT_SW, dx+1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SW;
		}
	else
	if ( NewValue == Value )
		Direction |= SW;
	}
if ( !(DontGo & W) )
	{
	if (INFILE(x-1, y+1) && INFILE(x-1, y-1))
	    {
	    lpLine1 = CachePtr(0, x-1, y+1, NO);
	    lpLine2 = CachePtr(0, x-1, y-1, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance24(lpLine1, lpLine2);
 	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue24( x-1, y, NOT_W, dx+1, dy, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = W;
		}
	else
	if ( NewValue == Value )
		Direction |= W;
	}
if ( !(DontGo & NW) )
	{
	if (INFILE(x, y-1) && INFILE(x-1, y))
	    {
	    lpLine1 = CachePtr(0, x, y-1, NO);
	    lpLine2 = CachePtr(0, x-1, y, NO);
	    if (lpLine1 && lpLine2) 
	        NewValue = Distance24(lpLine1, lpLine2);
 	    else
	        NewValue = 1;
	    }
	else
	    NewValue = 1;
	if ( depth )
		NewValue += EdgeValue24( x-1, y-1, NOT_NW, dx+1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NW;
		}
	else
	if ( NewValue == Value )
		Direction |= NW;
	}

if ( depth == Mask.AutoSearchDepth )
	{ /* The search is over */
	/* Break any ties by heading for the destination */
	Go1 = direction( dx, dy );
	if ( Value < 1  ) return( 0 );
	Go2 = Go1;			  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	}
else	return( Value );
}

/************************************************************************/
int GoingAway( dx, dy )
/************************************************************************/
int dx, dy;
{
int direction;

if (abs(dy) > abs(dx))
    {
    if (dx > 0)
        {
        if (dy > 0)
            direction = NOT_S | W;   		/* not S */
        else if (dy < 0)
        	direction = NOT_NE | SE; 		/* not NE */
        else 
        	direction = NOT_E | N | S;		/* not E*/
        }
    else if (dx < 0)
        {
        if (dy > 0)
            direction = NOT_SW | NW;		/* not SW */
        else if (dy < 0)
            direction = NOT_N | E;			/* not N*/
        else
            direction = NOT_W | N | S; 		/* not W */
        }
    else
        {
        if (dy > 0)
            direction = NOT_S | E | W;		/* not S */
        else
            direction = NOT_N | E | W;	 	/* not N */
        }
    }
else
    {
    if (dx > 0)
        {
        if (dy > 0)
            direction = NOT_SE | SW;		/* not SE */
        else if (dy < 0)
        	direction = NOT_E | S;			/* not E */
        else 
        	direction = NOT_E | N | S;		/* not E*/
        }
    else if (dx < 0)
        {
        if (dy > 0)
            direction = NOT_W | N;			/* not W */
        else if (dy < 0)
            direction = NOT_NW | NE;		/* not NW */
        else
            direction = NOT_W | N | S;		/* not W*/
        }
    else
        {
        if (dy > 0)
            direction = NOT_S | E | W;		/* not S */
        else
            direction = NOT_N | E | W;		/* not N */
        }
    }
return(direction);
}


/************************************************************************/
int direction( dx, dy )
/************************************************************************/
int dx, dy;
{
int direction;

if (abs(dy) > abs(dx))
    {
    if (dx > 0)
        {
        if (dy > 0)
            direction = S;
        else if (dy < 0)
        	direction = NE;
        else 
        	direction = E;
        }
    else if (dx < 0)
        {
        if (dy > 0)
            direction = SW;
        else if (dy < 0)
            direction = N;
        else
            direction = W;
        }
    else
        {
        if (dy > 0)
            direction = S;
        else
            direction = N;
        }
    }
else
    {
    if (dx > 0)
        {
        if (dy > 0)
            direction = SE;
        else if (dy < 0)
        	direction = E;
        else 
        	direction = E;
        }
    else if (dx < 0)
        {
        if (dy > 0)
            direction = W;
        else if (dy < 0)
            direction = NW;
        else
            direction = W;
        }
    else
        {
        if (dy > 0)
            direction = S;
        else
            direction = N;
        }
    }
return(direction);
}

