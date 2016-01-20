/*=======================================================================*\

	FRMTFORM.C - Transformation Matrix Management.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include "framelib.h"
#include "macros.h"

/***********************************************************************/
void TInit( LPTFORM lpTForm )
/***********************************************************************/
{
lpTForm->ax = lpTForm->by = FUNITY;
lpTForm->bx = lpTForm->cx = lpTForm->ay = lpTForm->cy = 0L;
}


/***********************************************************************/
void TMultiply( LPTFORM lpTFormS1, LPTFORM lpTFormS2, LPTFORM lpTFormD)
/***********************************************************************/
{
lpTFormD->ax = FIXMUL(lpTFormS1->ax, lpTFormS2->ax) +
			   FIXMUL(lpTFormS1->ay, lpTFormS2->bx);
lpTFormD->bx = FIXMUL(lpTFormS1->bx, lpTFormS2->ax) +
  			   FIXMUL(lpTFormS1->by, lpTFormS2->bx);
lpTFormD->cx = FIXMUL(lpTFormS1->cx, lpTFormS2->ax) +
 			   FIXMUL(lpTFormS1->cy, lpTFormS2->bx) +
 			   lpTFormS2->cx;
lpTFormD->ay = FIXMUL(lpTFormS1->ax, lpTFormS2->ay) +
 			   FIXMUL(lpTFormS1->ay, lpTFormS2->by);
lpTFormD->by = FIXMUL(lpTFormS1->bx, lpTFormS2->ay) +
			   FIXMUL(lpTFormS1->by, lpTFormS2->by);
lpTFormD->cy = FIXMUL(lpTFormS1->cx, lpTFormS2->ay) +
			   FIXMUL(lpTFormS1->cy, lpTFormS2->by) +
			   lpTFormS2->cy;
}


/***********************************************************************/
void TMove( LPTFORM lpTForm, int tx, int ty )
/***********************************************************************/
{
lpTForm->cx += MAKEFIXED(tx);
lpTForm->cy += MAKEFIXED(ty);
}

/***********************************************************************/
void TFMove( LPTFORM lpTForm, LFIXED fx, LFIXED fy )
/***********************************************************************/
{
lpTForm->cx += fx;
lpTForm->cy += fy;
}


/***********************************************************************/
void TScale( LPTFORM lpTForm, LFIXED sx, LFIXED sy )
/***********************************************************************/
{
if ( sx && (sx != FUNITY) )
	{
	lpTForm->ax = FIXMUL( lpTForm->ax, sx );
	lpTForm->bx = FIXMUL( lpTForm->bx, sx );
	lpTForm->cx = FIXMUL( lpTForm->cx, sx );
	}

if ( sy && (sy != FUNITY) )
	{
	lpTForm->ay = FIXMUL( lpTForm->ay, sy );
	lpTForm->by = FIXMUL( lpTForm->by, sy );
	lpTForm->cy = FIXMUL( lpTForm->cy, sy );
	}
}


/***********************************************************************/
void TShearX( LPTFORM lpTForm, int height, int dx )
/***********************************************************************/
{
LFIXED b;
unsigned int h;

h = lsqrt(((long)height * (long)height) + ((long)dx * (long)dx));
b = FGET(dx, h);

lpTForm->ax += FIXMUL( lpTForm->ay, b );
lpTForm->bx += FIXMUL( lpTForm->by, b );
lpTForm->cx += FIXMUL( lpTForm->cy, b );
}

/***********************************************************************/
void TShearY( LPTFORM lpTForm, int width, int dy )
/***********************************************************************/
{
LFIXED a;
unsigned int h;

h = lsqrt(((long)width * (long)width) + ((long)dy * (long)dy));
a = FGET(dy, h);

lpTForm->ay += FIXMUL( lpTForm->ax, a );
lpTForm->by += FIXMUL( lpTForm->bx, a );
lpTForm->cy += FIXMUL( lpTForm->cx, a );
}

/***********************************************************************/
void TRotate( LPTFORM lpTForm, LFIXED anglex, LFIXED angley )
/***********************************************************************/
{
LFIXED sinx, cosx, siny, cosy;
LFIXED ax, bx, cx;

SinCos( anglex, &sinx, &cosx );
SinCos( angley, &siny, &cosy );

ax = lpTForm->ax;
bx = lpTForm->bx;
cx = lpTForm->cx;

if ( siny || cosx )
	{
	lpTForm->ax = FIXMUL( ax, cosx ) - FIXMUL( lpTForm->ay, siny );
	lpTForm->bx = FIXMUL( bx, cosx ) - FIXMUL( lpTForm->by, siny );
	lpTForm->cx = FIXMUL( cx, cosx ) - FIXMUL( lpTForm->cy, siny );
	}

if ( cosy || sinx )
	{
	lpTForm->ay = FIXMUL( ax, sinx ) + FIXMUL( lpTForm->ay, cosy );
	lpTForm->by = FIXMUL( bx, sinx ) + FIXMUL( lpTForm->by, cosy );
	lpTForm->cy = FIXMUL( cx, sinx ) + FIXMUL( lpTForm->cy, cosy );
	}
}

/***********************************************************************/
void TInvert( LPTFORM lpTForm )
/***********************************************************************/
{
LFIXED ax, bx, cx, ay, by, cy;
LFIXED term1, term2, term3;

ax = lpTForm->ax; ay = lpTForm->ay;
bx = lpTForm->bx; by = lpTForm->by;
cx = lpTForm->cx; cy = lpTForm->cy;

/* ax*by - bx*ay */	term1 = FIXMUL( ax, by ) - FIXMUL( bx, ay );
/* cy*bx - cx*by */	term2 = FIXMUL( cy, bx ) - FIXMUL( cx, by );
/* cx*ay - cy*ax */	term3 = FIXMUL( cx, ay ) - FIXMUL( cy, ax );
lpTForm->ax = FGET(    by, term1 );
lpTForm->bx = FGET(   -bx, term1 );
lpTForm->cx = FGET( term2, term1 );

lpTForm->ay = FGET(   -ay, term1 );
lpTForm->by = FGET(    ax, term1 );
lpTForm->cy = FGET( term3, term1 );
}

/***********************************************************************/
void FTransformer( LPTFORM lpTForm, LPPOINT lpPoint, LPLFIXED lpx, LPLFIXED lpy )
/***********************************************************************/
{
long x, y;

x = lpPoint->x;
y = lpPoint->y;

*lpx = (lpTForm->ax * x) + (lpTForm->bx * y) + lpTForm->cx;
*lpy = (lpTForm->ay * x) + (lpTForm->by * y) + lpTForm->cy;
}


/***********************************************************************/
void Transformer( LPTFORM lpTForm, LPPOINT lpPoint, LPINT lpx, LPINT lpy )
/***********************************************************************/
{
long x, y;
LFIXED fx, fy;

x = lpPoint->x;
y = lpPoint->y;

#ifndef USING_FIXED16

fx = (lpTForm->ax * x) + (lpTForm->bx * y) + lpTForm->cx;
fy = (lpTForm->ay * x) + (lpTForm->by * y) + lpTForm->cy;
*lpx = FROUND(fx);
*lpy = FROUND(fy);

#else

fx = (lpTForm->ax * x) + (lpTForm->bx * y) + lpTForm->cx + HALF;
fy = (lpTForm->ay * x) + (lpTForm->by * y) + lpTForm->cy + HALF;
*lpx = (INT16)HIWORD( fx );
*lpy = (INT16)HIWORD( fy );

#endif
}


/************************************************************************/
void TransformRect( LPTFORM lpTForm, LPRECT lpRect, LPRECT lpNewRect )
/************************************************************************/
{
int x1, x2, x3, x4, y1, y2, y3, y4;
POINT Point;

// check for identity transform
if (ISNULLTFORM(lpTForm))
	{
		*lpNewRect = *lpRect;
		return;
	}			

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

