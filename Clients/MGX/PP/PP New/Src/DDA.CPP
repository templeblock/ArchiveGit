// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

#include "pp.h"
#include "macros.h"

/************************************************************************/
void ddaline( int x1, int y1, int x2, int y2, void ( *lpPointFunction )( int x, int y ) )
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
	{ dy = FUNITY; dx = FGET( x, y ); }
else	{ dx = FUNITY; dy = FGET( y, x ); }

/* Initialize the ax accumulator with half the dx increment */
/* plus one half for rounding off */
ax = FHALF; //( dx + FUNITY )/2;
//ax &= 0xFFFFL;
ay = FHALF; //( dy + FUNITY )/2;
//ay &= 0xFFFFL;

x = x1;
y = y1;
while (1)
	{
	(*lpPointFunction)( x, y ); /* use x, y */
	if ( x == x2 && y == y2 )
		break;
	if ( ( ax += dx ) > FUNITY )
		{ ax -= FUNITY; x += ix; }
	if ( ( ay += dy ) > FUNITY )
		{ ay -= FUNITY; y += iy; }
	}
}
