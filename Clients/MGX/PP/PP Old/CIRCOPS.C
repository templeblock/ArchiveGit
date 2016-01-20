// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®TP5¯®BT5¯®RFA®FC¯Page ®PN¯ of ®FP¯ -- ppv11a ®VA$FI¯ -- 1:52 PM , May 31, 1989¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

extern long labs();	/* in the c library */
static int special, xc, yc;

/************************************************************************/
void ellipse(xcenter,ycenter,a0,b0,on_off,special_flag)
/************************************************************************/
int xcenter,ycenter,a0,b0,on_off,special_flag;
{
int x = 0;
int y = b0;
long a = a0;
long b = b0;
long asqrd = a * a;
long twoasqrd = 2 * asqrd;
long bsqrd = b * b;
long twobsqrd = 2 * bsqrd;
long d,dx,dy;

special = special_flag;
xc = xcenter;
yc = ycenter;
d = bsqrd - asqrd*b + asqrd/4L;
dx = 0;
dy = twoasqrd * b;

/*if(on_off)
	set4pix(0,0);
else	clr4pix(0,0);*/

while(dx<dy)
	{
	if(on_off)
		set4pix(x,y);
	else	clr4pix(x,y);
	if(d > 0L)
		{
		--y;
		dy -= twoasqrd;
		d -= dy;
		}
	++x;
	dx += twobsqrd;
	d += bsqrd + dx;
	}

d += (3L*(asqrd-bsqrd)/2L - (dx+dy)) / 2L;

while(y>=0)
	{
	if(on_off)
		set4pix(x,y);
	else	clr4pix(x,y);
	if(d < 0L)
		{
		++x;
		dx += twobsqrd;
		d += dx;
		}
	--y;
	dy -= twoasqrd;
	d += asqrd - dy;
	}
}


/************************************************************************/
static void set4pix( xp, yp )
/************************************************************************/
int xp, yp;
{
int ov, x, y;

if ( !special )
	{
	x = xc+xp; y = yc+yp; if ( ONSCREEN( x, y ) ) OVERLAY( x, y, ON );
	x = xc-xp; y = yc+yp; if ( xp && ONSCREEN( x, y ) ) OVERLAY( x, y, ON );
	x = xc+xp; y = yc-yp; if ( yp && ONSCREEN( x, y ) ) OVERLAY( x, y, ON );
	x = xc-xp; y = yc-yp; if ( xp && yp && ONSCREEN( x, y ) ) OVERLAY( x, y, ON );
	}
else
if ( special == 1 )
	{
	x = xc+xp; y = yc+yp; if ( ONIMAGE( x, y ) ) OVERLAY( x, y, ON );
	x = xc-xp; y = yc+yp; if ( xp && ONIMAGE( x, y ) ) OVERLAY( x, y, ON );
	x = xc+xp; y = yc-yp; if ( yp && ONIMAGE( x, y ) ) OVERLAY( x, y, ON );
	x = xc-xp; y = yc-yp; if ( xp && yp && ONIMAGE( x, y ) ) OVERLAY( x, y, ON );
	}
else	{ // Draw a hard green mask
	x = xc+xp; y = yc+yp;
//	if ( ONIMAGE(x, y)) screen(x,y,scrread(x,y)|HIBITS);
	if ( ONIMAGE(x, y)) screen(x,y,LGREEN);
	x = xc-xp; y = yc+yp;
	if ( ONIMAGE(x, y)) screen(x,y,LGREEN);
	x = xc+xp; y = yc-yp;
	if ( ONIMAGE(x, y)) screen(x,y,LGREEN);
	x = xc-xp; y = yc-yp;
	if ( ONIMAGE(x, y)) screen(x,y,LGREEN);
	}
}


/************************************************************************/
static void clr4pix( xp, yp )
/************************************************************************/
int xp, yp;
{
int ov, x, y;

if ( !special )
	{
	x = xc+xp; y = yc+yp; if ( ONSCREEN( x, y ) ) OVERLAY( x, y, OFF );
	x = xc-xp; y = yc+yp; if ( xp && ONSCREEN( x, y ) ) OVERLAY( x, y, OFF);
	x = xc+xp; y = yc-yp; if ( yp && ONSCREEN( x, y ) ) OVERLAY( x, y, OFF);
	x = xc-xp; y = yc-yp; if ( xp && yp && ONSCREEN( x, y ) ) OVERLAY( x, y, OFF );
	}
else
if ( special == 1 )
	{
	x = xc+xp; y = yc+yp; if ( ONIMAGE( x, y ) ) OVERLAY( x, y, OFF );
	x = xc-xp; y = yc+yp; if ( xp && ONIMAGE( x, y ) ) OVERLAY( x, y, OFF );
	x = xc+xp; y = yc-yp; if ( yp && ONIMAGE( x, y ) ) OVERLAY( x, y, OFF );
	x = xc-xp; y = yc-yp; if ( xp && yp && ONIMAGE( x, y ) ) OVERLAY( x, y, OFF );
	}
}


