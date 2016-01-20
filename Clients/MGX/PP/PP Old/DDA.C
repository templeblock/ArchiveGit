// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

/************************************************************************/
void ddaline( x1, y1, x2, y2, lpPointFunction )
/************************************************************************/
int x1, y1, x2, y2;
LPROC lpPointFunction;
{
int x, y, ix, iy;
FIXED dx, dy, ax, ay;

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
