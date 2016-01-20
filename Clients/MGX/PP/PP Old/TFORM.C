// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

/***********************************************************************/
void TInit( lpTForm )
/***********************************************************************/
LPTFORM lpTForm;
{
lpTForm->ax = lpTForm->by = 65536L;
lpTForm->bx = lpTForm->cx = lpTForm->ay = lpTForm->cy = 0L;
}


/***********************************************************************/
void TMultiply( lpTFormS1, lpTFormS2, lpTFormD)
/***********************************************************************/
LPTFORM lpTFormS1, lpTFormS2, lpTFormD;
{
lpTFormD->ax = FMUL(lpTFormS1->ax, lpTFormS2->ax) +
			   FMUL(lpTFormS1->ay, lpTFormS2->bx);
lpTFormD->bx = FMUL(lpTFormS1->bx, lpTFormS2->ax) +
  			   FMUL(lpTFormS1->by, lpTFormS2->bx);
lpTFormD->cx = FMUL(lpTFormS1->cx, lpTFormS2->ax) +
 			   FMUL(lpTFormS1->cy, lpTFormS2->bx) +
 			   lpTFormS2->cx;
lpTFormD->ay = FMUL(lpTFormS1->ax, lpTFormS2->ay) +
 			   FMUL(lpTFormS1->ay, lpTFormS2->by);
lpTFormD->by = FMUL(lpTFormS1->bx, lpTFormS2->ay) +
			   FMUL(lpTFormS1->by, lpTFormS2->by);
lpTFormD->cy = FMUL(lpTFormS1->cx, lpTFormS2->ay) +
			   FMUL(lpTFormS1->cy, lpTFormS2->by) +
			   lpTFormS2->cy;
}


/***********************************************************************/
void TMove( lpTForm, tx, ty )
/***********************************************************************/
LPTFORM lpTForm;
int tx, ty;
{
lpTForm->cx += TOFIXED(tx);
lpTForm->cy += TOFIXED(ty);
}


/***********************************************************************/
void TScale( lpTForm, sx, sy )
/***********************************************************************/
LPTFORM lpTForm;
FIXED sx, sy;
{
if ( sx && (sx != UNITY) )
	{
	lpTForm->ax = FMUL( lpTForm->ax, sx );
	lpTForm->bx = FMUL( lpTForm->bx, sx );
	lpTForm->cx = FMUL( lpTForm->cx, sx );
	}

if ( sy && (sy != UNITY) )
	{
	lpTForm->ay = FMUL( lpTForm->ay, sy );
	lpTForm->by = FMUL( lpTForm->by, sy );
	lpTForm->cy = FMUL( lpTForm->cy, sy );
	}
}


/***********************************************************************/
void TShearX( lpTForm, height, dx )
/***********************************************************************/
LPTFORM lpTForm;
int height, dx;
{
FIXED b;
unsigned int h;

h = lsqrt(((long)height * (long)height) + ((long)dx * (long)dx));
b = FGET(dx, h);

lpTForm->ax = lpTForm->ax + FMUL( lpTForm->ay, b );
lpTForm->bx = lpTForm->bx + FMUL( lpTForm->by, b );
lpTForm->cx = lpTForm->cx + FMUL( lpTForm->cy, b );
}

/***********************************************************************/
void TShearY( lpTForm, width, dy )
/***********************************************************************/
LPTFORM lpTForm;
int width, dy;
{
FIXED a;
unsigned int h;

h = lsqrt(((long)width * (long)width) + ((long)dy * (long)dy));
a = FGET(dy, h);

lpTForm->ay = FMUL( lpTForm->ax, a ) + lpTForm->ay;
lpTForm->by = FMUL( lpTForm->bx, a ) + lpTForm->by;
lpTForm->cy = FMUL( lpTForm->cx, a ) + lpTForm->cy;
}

/***********************************************************************/
void TRotate( lpTForm, anglex, angley )
/***********************************************************************/
LPTFORM lpTForm;
int anglex, angley;
{
FIXED sinx, cosx, siny, cosy;
FIXED ax, bx, cx;

SinCos( anglex, &sinx, &cosx );
SinCos( angley, &siny, &cosy );

ax = lpTForm->ax;
bx = lpTForm->bx;
cx = lpTForm->cx;

if ( siny || cosx )
	{
	lpTForm->ax = FMUL( ax, cosx ) - FMUL( lpTForm->ay, siny );
	lpTForm->bx = FMUL( bx, cosx ) - FMUL( lpTForm->by, siny );
	lpTForm->cx = FMUL( cx, cosx ) - FMUL( lpTForm->cy, siny );
	}

if ( cosy || sinx )
	{
	lpTForm->ay = FMUL( ax, sinx ) + FMUL( lpTForm->ay, cosy );
	lpTForm->by = FMUL( bx, sinx ) + FMUL( lpTForm->by, cosy );
	lpTForm->cy = FMUL( cx, sinx ) + FMUL( lpTForm->cy, cosy );
	}
}


/***********************************************************************/
void TInvert( lpTForm )
/***********************************************************************/
LPTFORM lpTForm;
{
FIXED ax, bx, cx, ay, by, cy;
long term1, term2, term3;

ax = lpTForm->ax; ay = lpTForm->ay;
bx = lpTForm->bx; by = lpTForm->by;
cx = lpTForm->cx; cy = lpTForm->cy;

/* ax*by - bx*ay */	term1 = FMUL( ax, by ) - FMUL( bx, ay );
/* cy*bx - cx*by */	term2 = FMUL( cy, bx ) - FMUL( cx, by );
/* cx*ay - cy*ax */	term3 = FMUL( cx, ay ) - FMUL( cy, ax );

lpTForm->ax = FGET(    by, term1 );
lpTForm->bx = FGET(   -bx, term1 );
lpTForm->cx = FGET( term2, term1 );

lpTForm->ay = FGET(   -ay, term1 );
lpTForm->by = FGET(    ax, term1 );
lpTForm->cy = FGET( term3, term1 );
}


/***********************************************************************/
void FTransformer( lpTForm, lpPoint, lpx, lpy )
/***********************************************************************/
LPTFORM lpTForm;
LPPOINT lpPoint;
LPLONG lpx, lpy;
{
long x, y;

x = lpPoint->x;
y = lpPoint->y;

*lpx = (lpTForm->ax * x) + (lpTForm->bx * y) + lpTForm->cx;
*lpy = (lpTForm->ay * x) + (lpTForm->by * y) + lpTForm->cy;
}


/***********************************************************************/
void Transformer( lpTForm, lpPoint, lpx, lpy )
/***********************************************************************/
LPTFORM lpTForm;
LPPOINT lpPoint;
LPINT lpx, lpy;
{
long x, y;
FIXED fx, fy;

x = lpPoint->x;
y = lpPoint->y;

fx = (lpTForm->ax * x) + (lpTForm->bx * y) + lpTForm->cx + HALF;
fy = (lpTForm->ay * x) + (lpTForm->by * y) + lpTForm->cy + HALF;
*lpx = HIWORD( fx );
*lpy = HIWORD( fy );
}


/************************************************************************/
void TransformRect( lpTForm, lpRect, lpNewRect )
/************************************************************************/
LPTFORM lpTForm;
LPRECT lpRect, lpNewRect;
{
int x1, x2, x3, x4, y1, y2, y3, y4;
POINT Point;

Point.x = lpRect->left;
Point.y = lpRect->top;
Transformer( lpTForm, &Point, &x1, &y1 );

Point.x = lpRect->right;
Point.y = lpRect->top;
Transformer( lpTForm, &Point, &x2, &y2 );

Point.x = lpRect->right;
Point.y = lpRect->bottom;
Transformer( lpTForm, &Point, &x3, &y3 );

Point.x = lpRect->left;
Point.y = lpRect->bottom;
Transformer( lpTForm, &Point, &x4, &y4 );

lpNewRect->left   = min( x1, min( x2, min( x3, x4 ) ) );
lpNewRect->right  = max( x1, max( x2, max( x3, x4 ) ) );
lpNewRect->top    = min( y1, min( y2, min( y3, y4 ) ) );
lpNewRect->bottom = max( y1, max( y2, max( y3, y4 ) ) );
}
