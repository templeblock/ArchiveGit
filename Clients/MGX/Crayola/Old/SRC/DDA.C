// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include <windows.h>
#include "types.h"
#include "routines.h"
#include "macros.h"


/************************************************************************/
void ddaline( int x1, int y1, int x2, int y2, LPPOINTFUNC lpPointFunction )
/************************************************************************/
{
int x, y, ix, iy;
LFIXED dx, dy, ax, ay;

if ( x1 > x2 )
	{ x = x1 - x2; ix = -1; }
else	{ x = x2 - x1; ix = 1; }

if ( y1 > y2 )
	{ y = y1 - y2; iy = -1; }
else	{ y = y2 - y1; iy = 1; }

if ( x < y )
	{ dy = UNITY; dx = FGET( x, y ); }
else	{ dx = UNITY; dy = FGET( y, x ); }

/* Initialize the ax accumulator with half the dx increment */
/* plus one half for rounding off */
ax = HALF; //( dx + UNITY )/2;
//ax &= 0xFFFFL;
ay = HALF; //( dy + UNITY )/2;
//ay &= 0xFFFFL;

x = x1;
y = y1;
while (1)
	{
	(*lpPointFunction)( x, y ); /* use x, y */
	if ( x == x2 && y == y2 )
		break;
	if ( ( ax += dx ) > UNITY )
		{ ax -= UNITY; x += ix; }
	if ( ( ay += dy ) > UNITY )
		{ ay -= UNITY; y += iy; }
	}
}

/************************************************************************/
void ddazigzag( int x1, int y1, int x2, int y2, int iSize, LPPOINTFUNC lpPointFunction )
/************************************************************************/
{
int x11, y11, x12, y12;
int _cdecl rand(void);

x11 = rand() % max( iSize, (x2 - x1)/2 );
y11 = rand() % max( iSize, (y2 - y1)/2 );
ddaline( x1, y1, x11, y11, lpPointFunction );
x12 = rand() % max( iSize, (x2 - x11)/2 );
y12 = rand() % max( iSize, (y2 - y11)/2 );
ddaline( x11, y11, x12, y12, lpPointFunction );
ddaline( x12, y12, x2, y2, lpPointFunction );
}
