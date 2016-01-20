// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

/************************************************************************/
LPSHAPE BuildShapes(pnts, nPoints, nShapes, fCircle )
/************************************************************************/
LPPOINT pnts;
LPINT nPoints;
int nShapes;
BOOL fCircle;
{
LPTR lpMemory;
LPSHAPE lpShape, lpNextShape, lpHeadShape;
LPPOINT lpPoints, lpFirstPoint;
int count, i;
long lMemSize;

if ( !( lpFirstPoint = pnts ) )
	return( NULL );

lpHeadShape = NULL;
for (i = 0; i < nShapes; ++i)
	{
	/* Get memory for the new shape structure */
	lMemSize = (long)sizeof(SHAPE) + ((long)sizeof(POINT) * nPoints[i]);
	if ( !(lpMemory = Alloc( lMemSize ) ) )
		return( NULL );

	/* Initialize the new shape structure */
	lpShape = (LPSHAPE)lpMemory;
	lpMemory += sizeof(SHAPE);
	lpShape->lpPoints = (LPPOINT)lpMemory;
	lpShape->fCircle = fCircle;
	lpShape->nPoints = nPoints[i];
	lpShape->rShape.top = 32767;
	lpShape->rShape.bottom = -32767;
	lpShape->rShape.left = 32767;
	lpShape->rShape.right = -32767;
	TInit(&lpShape->tform);
	lpShape->lpNext = NULL;

	/* Link the new shape to the bottom of the list */
	if ( !lpHeadShape )
		lpHeadShape = lpShape;
	else	{
		lpNextShape = lpHeadShape;
		while ( lpNextShape->lpNext )
			lpNextShape = lpNextShape->lpNext;
		lpNextShape->lpNext = lpShape;
		}

	/* Loop through all of the points and copy them to shape's point list */
	/* Estimate the number number edges we will create from this shape */
	/* Also, update the shape's outer bounds as we go */
	lpPoints = lpShape->lpPoints;
	count = nPoints[i];
	while ( --count >= 0 )
		{
		/* Copy each point into the new shape's point list */
		lpPoints->x = pnts->x;
		lpPoints->y = pnts->y;
		pnts++;

		/* Don't do any more with Bezier markers */
		if ( lpPoints->x == BEZIER_MARKER )
			{
			lpPoints++;
			continue;
			}

		/* Update the shape's outer bounds */
		if ( lpPoints->x < lpShape->rShape.left )
			lpShape->rShape.left = lpPoints->x;
		if ( lpPoints->x > lpShape->rShape.right )
			lpShape->rShape.right = lpPoints->x;
		if ( lpPoints->y < lpShape->rShape.top )
			lpShape->rShape.top = lpPoints->y;
		if ( lpPoints->y > lpShape->rShape.bottom)
			lpShape->rShape.bottom = lpPoints->y;
		lpPoints++;
		}
	}

return(lpHeadShape);
}


/************************************************************************/
LPMASK BuildEdges( lpMask, lpShape )
/************************************************************************/
LPMASK lpMask;
LPSHAPE lpShape;
{
LPTR lpMemory;
LPSHAPE lpNextShape;
int nEdges;
long lMemSize;
int y;
LPPOINT lpEdges;
LPPPOINT lpEdgeMap;

if ( !lpImage )
	return( NULL );

/* If not passed in, allocate a mask structure to return to the caller */
if ( !lpMask )
	{
	lMemSize = (long)sizeof(MASK) + lpImage->npix + 
		((long)sizeof(LPPOINT) * lpImage->nlin);
	if ( !( lpMemory = Alloc( lMemSize ) ) )
		return( NULL );
	lpMask = (LPMASK)lpMemory;
	lpMemory += sizeof(MASK);
	lpMask->mbuffer = lpMemory;
	lpMemory += lpImage->npix;
	lpMask->mbufsize = lpImage->npix;
	lpMask->lpHeadShape = NULL;
	lpMask->lpEdges = NULL;
	lpMask->nEdges = 0;
	lpMask->lpEdgeMap = (LPPPOINT)lpMemory;
	lpMask->nEdgeMapEntries = lpImage->nlin;
	lpMask->lpMarquee = NULL;
	lpMask->nMarqueePoints = 0;
	lpMask->rMask.top = 32767;
	lpMask->rMask.bottom = -32767;
	lpMask->rMask.left = 32767;
	lpMask->rMask.right = -32767;
	}

if (lpShape)
	{
	/* Link the new shape to the bottom of the list */
	if ( !lpMask->lpHeadShape )
		lpMask->lpHeadShape = lpShape;
	else	{
		lpNextShape = lpMask->lpHeadShape;
		while ( lpNextShape->lpNext )
			lpNextShape = lpNextShape->lpNext;
		lpNextShape->lpNext = lpShape;
		}
	}
else	{ /* rebuild all edges */
	if (lpMask->lpEdges)
		FreeUp( (LPTR)lpMask->lpEdges );
	lpMask->lpEdges = NULL;
	lpMask->nEdges = 0;
	for (y = 0; y < lpMask->nEdgeMapEntries; ++y)
		lpMask->lpEdgeMap[y] = NULL;
	lpShape = lpMask->lpHeadShape;
	lpMask->rMask.top = 32767;
	lpMask->rMask.bottom = -32767;
	lpMask->rMask.left = 32767;
	lpMask->rMask.right = -32767;
	}

nEdges = BuildEdgesFromShapes(lpMask, lpShape, NULL, BuildEdgeProc, BuildEdgeProc, FALSE);
if (nEdges < 0)
	{
	mask_close(lpMask);
	return(NULL);
	}
qsort( (LPTR)lpMask->lpEdges, (long)lpMask->nEdges, sizeof(POINT),
	CompareEdges );

/* Clear the edge map, since we're going to rebuild it */
lclr( (LPTR)lpMask->lpEdgeMap,
	(long)sizeof(LPPPOINT) * lpMask->nEdgeMapEntries );

/* Setup the edge map with pointers into the list of vertical edges */
/* The map is used to quickly test coordinates for inside/outside the mask */
lpEdgeMap = lpMask->lpEdgeMap;
lpEdges = lpMask->lpEdges;
nEdges = lpMask->nEdges;
while ( --nEdges >= 0 )
	{
	if (lpEdges->y >= 0)
		{
		if ( lpEdges->y >= lpMask->nEdgeMapEntries )
			break; /* Off the end of the edge map */
		if ( !lpEdgeMap[ lpEdges->y ] ) /* Entry not set yet */
			lpEdgeMap[ lpEdges->y ] = lpEdges;
		}
	lpEdges++;
	}
return(lpMask);
}


/************************************************************************/
LPMASK mask_create( lpMask, pnts, nPoints, nShapes, fCircle )
/************************************************************************/
LPMASK lpMask;
LPPOINT pnts;
LPINT nPoints;
int nShapes;
BOOL fCircle;
{
LPSHAPE lpShape;

if ( !(lpShape = BuildShapes(pnts, nPoints, nShapes, fCircle)) )
	return( NULL );
lpMask = BuildEdges(lpMask, lpShape);
return(lpMask);
}


/************************************************************************/
BOOL mask_compute( lpMask, y, state )
/************************************************************************/
LPMASK lpMask;
int	y;
int	state;
{
LPPOINT	lpEdge;
LPTR	lpMaskBuff, m;
int	x, MaskBuffSize, count;

if ( !lpMask )
	return( NO );

lpEdge = NULL;
lpMaskBuff = lpMask->mbuffer;
MaskBuffSize = lpMask->mbufsize;
if ( lpMask->lpEdgeMap && y < lpMask->nEdgeMapEntries )
	lpEdge = lpMask->lpEdgeMap[y];

if ( !lpEdge ||			/* No edges for this y */
	y < lpMask->rMask.top || /* We're computing a line above the mask */
	y > lpMask->rMask.bottom ) /* We're computing a line below the mask */
	{
	if ( state > 0 )
		set( lpMaskBuff, MaskBuffSize, 255 );
	else	clr( lpMaskBuff, MaskBuffSize );
	return( NO );
	}

clr( lpMaskBuff, MaskBuffSize );

/* Walk through all of the vertical edges with this y coordinate */
/* and set the mask buffer */
while ( lpEdge->y == y )
	{  /* This assumes a dummy edge is always at the end */
	x = lpEdge->x;
	if ( x < MaskBuffSize )
		{
		if ( x < 0 )
			lpMaskBuff[ 0 ] ^= 1;
		else	lpMaskBuff[ x ] ^= 1;
		}
	lpEdge++;
	}

/* Run the edge flag fill algorithm over the line
Note: The edge flag algorithm requires that contours be drawn in the
following manner:
	for each scanline intersecting the edge, complement the left-most
	pixel whose centerpoint lies to the right of (has an x value greater
	than) the intersection point. */

m = lpMaskBuff;
count = MaskBuffSize;
state = (state > 0) ? 0xFF : 0;
while ( MaskBuffSize-- )
	{
	if ( *m )
		state ^= 0xFF;
	*m++ = state;
	}

return( YES );
}


/************************************************************************/
LPTR mask_point( lpMask, x )
/************************************************************************/
LPMASK lpMask;
int	x;
{
if ( !lpMask )
	return( NULL );
if ( x < 0 || x >= lpMask->mbufsize )
	x = 0;
return( &( lpMask->mbuffer[ x ] ) );
}


/************************************************************************/
LPPOINT mask_getedges( lpMask, y )
/************************************************************************/
LPMASK lpMask;
int	y;
{
if ( !lpMask )
	return( NULL );

if ( y < lpMask->rMask.top || y > lpMask->rMask.bottom )
	return( NULL ); /* We've got a line uttside of the mask */

if ( !lpMask->lpEdgeMap || y >= lpMask->nEdgeMapEntries )
	return( NULL ); /* We've got a line outside the edge map */

return( lpMask->lpEdgeMap[y] );
}

/************************************************************************/
VOID FreeUpShapes( lpShape )
/************************************************************************/
LPSHAPE lpShape;
{
LPSHAPE lpNextShape;

while ( lpShape )
	{
	lpNextShape = lpShape->lpNext;
	FreeUp( (LPTR)lpShape );
	lpShape = lpNextShape;
	}
}

/************************************************************************/
BOOL mask_close( lpMask )
/************************************************************************/
LPMASK lpMask;
{
if ( !lpMask )
	return( NO );

/* Free up the edge list */
if ( lpMask->lpEdges )
	FreeUp( (LPTR)lpMask->lpEdges );

/* Free up the marquee list */
if ( lpMask->lpMarquee )
	FreeUp( (LPTR)lpMask->lpMarquee );

/* Free up the shapes (and their point lists) */
FreeUpShapes(lpMask->lpHeadShape);

/* Free up the mask itself */
FreeUp( (LPTR)lpMask );
return( YES );
}


/************************************************************************/
VOID mask_link( lpMask, type )
/************************************************************************/
LPMASK	lpMask;
int	type;
{
LPFRAME lpFrame;

lpFrame = frame_set( NULL );
if ( lpMask && lpFrame )
	{
	if ( type & READ_MASK )
		lpFrame->ReadMask = lpMask;
	if ( type & WRITE_MASK )
		lpFrame->WriteMask = lpMask;
	}
}


/************************************************************************/
VOID mask_unlink( lpMask, type )
/************************************************************************/
LPMASK	lpMask;
int	type;
{
LPFRAME lpFrame;

lpFrame = frame_set( NULL );
if ( lpMask && lpFrame )
	{
	if ( lpFrame->ReadMask == lpMask && type & READ_MASK )
		lpFrame->ReadMask = NULL;
	if ( lpFrame->WriteMask == lpMask && type & WRITE_MASK )
		lpFrame->WriteMask = NULL;
	}
}


/************************************************************************/
BOOL mask_active()
/************************************************************************/
{
LPFRAME lpFrame;

if ( lpFrame = frame_set( NULL ) )
	if ( lpFrame->WriteMask )
	return( TRUE );
return( FALSE );
}


/************************************************************************/
BOOL mask_rect( lpRect )
/************************************************************************/
LPRECT lpRect;
{
LPMASK	lpMask;
LPFRAME lpFrame;

if ( !lpImage )
	return( FALSE );

/* Start with the assumption of the entire image */
lpRect->top = 0; lpRect->bottom = lpImage->nlin - 1;
lpRect->left = 0; lpRect->right = lpImage->npix - 1;
if ( lpFrame = frame_set( NULL ) )
	if ( lpMask = lpFrame->WriteMask )
	{
	if ( !Mask.IsOutside )
	    {
	    *lpRect = lpMask->rMask;
	    BoundRect(lpRect, 0, 0, lpImage->npix-1, lpImage->nlin-1);
   	    }
	return( TRUE );
	}
return( FALSE );
}


/************************************************************************/
BOOL IsDisplayMasked( x, y )
/************************************************************************/
int x, y;
{
int X, Y;

X = x;
Y = y;
Display2File( &X, &Y );
return( IsMasked( X, Y ) );
}


/************************************************************************/
BOOL IsMasked( x, y )
/************************************************************************/
int x, y;
{
LPFRAME lpFrame;
LPMASK lpMask;
LPPOINT lpEdge;
BOOL State;

if ( !(lpFrame = frame_set( NULL )) )
	return( NO );
if ( !(lpMask = lpFrame->WriteMask) )
	return( NO );
if ( !lpMask->lpEdgeMap )
	return( NO );
if ( y >= lpMask->nEdgeMapEntries )
	return( NO );
if ( y < 0)
  	return( NO );
if ( !(lpEdge = lpMask->lpEdgeMap[y]) )
	return( NO ); /* No edges for this y */

State = NO;
while ( x >= lpEdge->x )
	{
	if ( y != lpEdge->y )
		break; /* This assumes a dummy edge is always at the end */
	lpEdge++;
	State = !State;
	}
return( State );
}

#ifdef C_CODE
/************************************************************************/
VOID MaskLoad( x, y, dx, lpM, onMask )
/************************************************************************/
int x, y, dx;
LPTR lpM;
int onMask;
{
LPFRAME lpFrame;
LPMASK lpMask;
LPPOINT lpEdge;
BOOL State;
int xend, xe;

set(lpM, dx, !onMask);
if ( !(lpFrame = frame_set( NULL )) )
	return;
if ( !(lpMask = lpFrame->WriteMask) )
	return;
if ( !lpMask->lpEdgeMap )
	return;
if ( y >= lpMask->nEdgeMapEntries )
	return;
if (y < 0)
	return;
if ( !(lpEdge = lpMask->lpEdgeMap[y]) )
	return; /* No edges for this y */

State = NO;
xend = x + dx - 1;
while ( x <= xend )
    {
    if (x < lpEdge->x) /* we've hit an edge boundary */
	{
        xe = min(lpEdge->x-1, xend);
	while (x <= xe)
	    {
	    if (State)
	        *lpM = onMask;
	    ++lpM;
	    ++x;
	    }
	}
    /* look for start of mask bits on */
    if ( y != lpEdge->y )
	break; /* This assumes a dummy edge is always at the end */
    lpEdge++;
    State = !State;
    }
}
#endif

/************************************************************************/
int BuildEdgesFromShapes( lpMask, lpShape, lpInEdges, lpDxEdgeProc, lpDyEdgeProc, fEstimate)
/************************************************************************/
LPMASK  lpMask;
LPSHAPE lpShape;
LPPOINT lpInEdges;
LPROC lpDxEdgeProc;
LPROC lpDyEdgeProc;
BOOL fEstimate;
{
int count, nEdges, nTotalEdges, dx, dy, nShapeEdges, nPoints;
LPPOINT lpPoint, lpFirstPoint, lpNextPoint, lpEdges;

lpEdges = lpInEdges;
nTotalEdges = 0;
/* Loop on all of the passed shapes */
while ( lpShape )
	{
	if ( !(lpPoint = lpShape->lpPoints ) )
		{ /* If no points for this shape... */
		/* Go on to the next shape */
		lpShape = lpShape->lpNext;
		continue;
		}

	nEdges = 0;
	/* Loop through the shape's points and copy them on the */
	/* assumption that there may be Beziers that need expanding */
	lpFirstPoint = (LPPOINT)LineBuffer[0];
	nPoints = ShapeConvertPoints(lpShape, lpFirstPoint);
	if (lpMask)
		{
		/* Loop through the point list and compute all the edges, */
		/* Only add them to the lpEdges list if we're not estimating */
		count = nPoints;
		lpPoint = lpFirstPoint;
		while ( --count >= 0 )
			{
			if ( count )
				lpNextPoint = lpPoint+1;
			else	
				lpNextPoint = lpFirstPoint;
			nEdges += abs( lpPoint->y - lpNextPoint->y );
			++lpPoint;
			}
	
		/* Put a dummy edge on the end to facilitate mask testing for last line */
		nEdges += 4;
	
		/* If we don't have an edge list yet, allocate enough edge structures */
		/* Otherwise, extend the existing allocation */
		if ( !lpMask->lpEdges )
			{
			lpMask->lpEdges = (LPPOINT)Alloc( (long)sizeof(POINT) * nEdges);
			lpEdges = lpMask->lpEdges;
			}
		else	
			{
			nEdges += lpMask->nEdges;
			lpMask->lpEdges = (LPPOINT)AllocExtend( (LPTR)lpMask->lpEdges,
				(long)sizeof(POINT) * nEdges);
			lpEdges = lpMask->lpEdges + lpMask->nEdges;
			}
		if ( !lpMask->lpEdges )
			return( -1 );
		}
	
	nShapeEdges = 0;
	count = nPoints;
	lpPoint = lpFirstPoint;
	while ( --count >= 0 )
		{
		if ( count )
			lpNextPoint = lpPoint+1;
		else	lpNextPoint = lpFirstPoint;

		if ( !fEstimate )
			{ /* Create a series of edges for each vector */
			nEdges = BuildEdgesFromVector( lpEdges, lpPoint,
				lpNextPoint, lpDxEdgeProc, lpDyEdgeProc );
			lpEdges += nEdges;
			nTotalEdges += nEdges;
			nShapeEdges += nEdges;
			}
		else	{ /* Estimate the edges for each vector */
			dx = abs( lpPoint->x - lpNextPoint->x ) + 1;
			dy = abs( lpPoint->y - lpNextPoint->y ) + 1;
			nTotalEdges += ( 2 * min(dx,dy) + 1 );
			}
		lpPoint++;
		}

	if (lpMask)
		{
		lpMask->nEdges += nShapeEdges;
		AstralUnionRect(&lpMask->rMask, &lpShape->rShape, &lpMask->rMask);
		}
	
	/* Go on to the next shape */
	lpShape = lpShape->lpNext;
	}

return( nTotalEdges );
}

static int EdgeCount, EdgePrevX, EdgePrevY, EdgeLastX, EdgeLastY;
static LPPOINT EdgeList;

/************************************************************************/
static int BuildEdgesFromVector( lpEdge, pnt1, pnt2, lpDxEdgeProc, lpDyEdgeProc)
/************************************************************************/
LPPOINT lpEdge;
LPPOINT pnt1, pnt2;
LPROC lpDxEdgeProc;
LPROC lpDyEdgeProc;
{
LPPOINT pnt0;

/* Check for any passed null pointers */
if ( !lpEdge || !pnt1 || !pnt2 )
	return( NULL );

/* Order the points by min y */
if ( pnt1->y > pnt2->y )
	{
	pnt0 = pnt1;
	pnt1 = pnt2;
	pnt2 = pnt0;
	}

/* Initialize the static state variables use by lpEdgeProc() */
EdgeList = lpEdge;
EdgeCount= 0;
EdgePrevX = pnt1->x;
EdgePrevY = pnt1->y;
EdgeLastX = pnt2->x;
EdgeLastY = pnt2->y;
if (abs(EdgePrevX-EdgeLastX) > abs(EdgePrevY-EdgeLastY))
	ddaline( pnt1->x, pnt1->y, pnt2->x, pnt2->y, lpDxEdgeProc);
else	ddaline( pnt1->x, pnt1->y, pnt2->x, pnt2->y, lpDyEdgeProc);
return( EdgeCount );
}


/************************************************************************/
static void BuildEdgeProc( x, y )
/************************************************************************/
int x, y;
{ /* Procedure to generate edges used for testing in or out of a mask */
if ( y != EdgePrevY )
	{ /* Don't generate anything unless y changes (and never on bottom y) */
	EdgeList->x = EdgePrevX;
	EdgeList->y = EdgePrevY;
	EdgeList++;
	EdgeCount++;
	EdgePrevY = y;
	}
EdgePrevX = x;
}


/************************************************************************/
void BuildMarqueeProc( x, y )
/************************************************************************/
int x, y;
{ /* Procedure to generate edges for drawing a marquee */
int dy;

if ( !lpImage )
	return;

/*if ( !RectIntersect( xy, &lpImage->DispRect ) )
	return;*/
if ( (dy = y - EdgePrevY) && x != EdgePrevX )
	{ /* both X and Y are changing */
	EdgeList->x = EdgePrevX;
	EdgeList->y = EdgePrevY;
	EdgeList++;
	EdgeCount++;
	if ( dy == 1 || dy == -1 )
		{
		EdgeList->x = x;
		EdgeList->y = EdgePrevY;
		}
	else	{
		EdgeList->x = EdgePrevX;
		EdgeList->y = y;
		}
	EdgeList++;
	EdgeCount++;
	EdgePrevX = x;
	EdgePrevY = y;
	}
if ( y == EdgeLastY && x == EdgeLastX )
	{ /* We hit the endpoint */
	EdgeList->x = EdgePrevX;
	EdgeList->y = EdgePrevY;
	EdgeList++;
	EdgeCount++;
	EdgeList->x = x;
	EdgeList->y = y;
	EdgeList++;
	EdgeCount++;
	EdgeList->x = PENUP_MARKER; /* Insert a pen up marker */
	EdgeList->y = PENUP_MARKER;
	EdgeList++;
	EdgeCount++;
	}
}

/************************************************************************/
void BuildDyMarqueeProc( x, y )
/************************************************************************/
int x, y;
{ /* Procedure to generate edges for drawing a marquee */
int dy;

if ( !lpImage )
	return;

/*if ( !RectIntersect( xy, &lpImage->DispRect ) )
	return;*/
if ( (dy = y - EdgePrevY) && x != EdgePrevX )
	{ /* both X and Y are changing */
	EdgeList->x = EdgePrevX;
	EdgeList->y = EdgePrevY;
	EdgeList++;
	EdgeCount++;
	
	/* make change in y first */
	EdgeList->x = EdgePrevX;
	EdgeList->y = y;
	EdgeList++;
	EdgeCount++;
	EdgePrevX = x;
	EdgePrevY = y;
	}
if ( y == EdgeLastY && x == EdgeLastX )
	{ /* We hit the endpoint */
	EdgeList->x = EdgePrevX;
	EdgeList->y = EdgePrevY;
	EdgeList++;
	EdgeCount++;
	EdgeList->x = x;
	EdgeList->y = y;
	EdgeList++;
	EdgeCount++;
	EdgeList->x = PENUP_MARKER; /* Insert a pen up marker */
	EdgeList->y = PENUP_MARKER;
	EdgeList++;
	EdgeCount++;
	}
}

/************************************************************************/
void BuildDxMarqueeProc( x, y )
/************************************************************************/
int x, y;
{ /* Procedure to generate edges for drawing a marquee */
int dy;

if ( !lpImage )
	return;

/*if ( !RectIntersect( xy, &lpImage->DispRect ) )
	return;*/
if ( (dy = y - EdgePrevY) && x != EdgePrevX )
	{ /* both X and Y are changing */
	EdgeList->x = EdgePrevX;
	EdgeList->y = EdgePrevY;
	EdgeList++;
	EdgeCount++;
	
	/* make change in x first */
	EdgeList->x = x;
	EdgeList->y = EdgePrevY;
	EdgeList++;
	EdgeCount++;
	EdgePrevX = x;
	EdgePrevY = y;
	}
if ( y == EdgeLastY && x == EdgeLastX )
	{ /* We hit the endpoint */
	EdgeList->x = EdgePrevX;
	EdgeList->y = EdgePrevY;
	EdgeList++;
	EdgeCount++;
	EdgeList->x = x;
	EdgeList->y = y;
	EdgeList++;
	EdgeCount++;
	EdgeList->x = PENUP_MARKER; /* Insert a pen up marker */
	EdgeList->y = PENUP_MARKER;
	EdgeList++;
	EdgeCount++;
	}
}


/************************************************************************/
int CompareEdges( lpEdge1, lpEdge2 )
/************************************************************************/
LPPOINT lpEdge1, lpEdge2;
{
/* Sort by y */
if ( lpEdge1->y > lpEdge2->y )
	return( +1 );
if ( lpEdge1->y < lpEdge2->y )
	return( -1 );
/* Sort by x */
if ( lpEdge1->x > lpEdge2->x )
	return( +1 );
if ( lpEdge1->x < lpEdge2->x )
	return( -1 );
return( 0 );
}


/************************************************************************/
int BezierToPoints( lpTForm, lpPoints, nPoints, lpOutputPoints)
/************************************************************************/
LPTFORM lpTForm;
LPPOINT lpPoints;
int nPoints;
LPPOINT lpOutputPoints;
{
#define MAX_INPUTPOINTS 10
int n, t, dx, dy, i;
FIXED tscale;
POINT ptR[MAX_INPUTPOINTS];
LPPOINT R, P, lpFirstOutputPoint;

if ( nPoints > MAX_INPUTPOINTS )
	nPoints == MAX_INPUTPOINTS;
lpFirstOutputPoint = lpOutputPoints;

Transformer(lpTForm, lpPoints, &lpOutputPoints->x, &lpOutputPoints->y);
lpOutputPoints++;
for ( t=1; t<MAX_BEZIER; t++ )
	{
	R = ptR;
	P = lpPoints;
	n = nPoints;
	while (--n >= 0)
		{
		Transformer(lpTForm, P, &R->x, &R->y);
		R++;
		P++;
		}
	tscale = FGET( t, MAX_BEZIER );
	n = nPoints - 1;
	while ( --n >= 0 )
		{
		R = ptR;
		for ( i=0; i<=n; i++ )
			{
			dx = (R+1)->x - R->x;
			dy = (R+1)->y - R->y;
			R->x += FMUL( dx, tscale );
			R->y += FMUL( dy, tscale );
			R++;
			}
		}
	lpOutputPoints->x = ptR[0].x;
	lpOutputPoints->y = ptR[0].y;
	lpOutputPoints++;
	}

n = nPoints - 1;
lpOutputPoints->x = ptR[n].x;
lpOutputPoints->y = ptR[n].y;
lpOutputPoints++;
return( lpOutputPoints - lpFirstOutputPoint );
}

/************************************************************************/
int ShapeConvertPoints(lpShape, lpOutputPoint)
/************************************************************************/
LPSHAPE lpShape;
LPPOINT lpOutputPoint;
{
int count, nPoints, nBezier, n;
LPPOINT lpPoint;

count = lpShape->nPoints;
lpPoint = lpShape->lpPoints;
nPoints = 0;
while ( --count >= 0 )
	{
	if ( lpPoint->x == BEZIER_MARKER )
		{
		nBezier = lpPoint->y;
		lpPoint++;
		n = BezierToPoints( &lpShape->tform, lpPoint, nBezier,
			lpOutputPoint );
		nPoints += n;
		lpOutputPoint += n;
		lpPoint += nBezier;
		count -= nBezier;
		}
	else	{
		Transformer(&lpShape->tform, lpPoint,
			&lpOutputPoint->x, &lpOutputPoint->y);
		/* Update the shape's outer bounds */
		++nPoints;
		lpOutputPoint++;
		lpPoint++;
		}
	}

return(nPoints);
}


/************************************************************************/
void SetNewMask( lpPoint, count, nShapes, fCircle )
/************************************************************************/
LPPOINT lpPoint;
LPINT count;
int nShapes;
BOOL fCircle;
{
LPMASK lpMask;
LPFRAME lpFrame;

if ( !lpImage )
	return;

if ( !(lpFrame = frame_set( NULL )) )
	return;

lpMask = lpFrame->WriteMask;
lpMask = mask_create( lpMask, lpPoint, count, nShapes, fCircle );
if ( !lpMask )
	{
	MessageBeep(0);
	return;
	}
mask_link( lpMask, WRITE_MASK );

lpImage->MarqueeStatus = MARQUEE_NOT_READY;
BuildMarquee();
lpImage->MarqueeStatus = MARQUEE_CYCLE;
}


/************************************************************************/
void SetDrawnMask( lpPoint, count, fCircle )
/************************************************************************/
LPPOINT lpPoint;
int count;
BOOL fCircle;
{
LPPOINT	lpFirstPoint, lpOutPoint;
int i, x, y, xlast, ylast, xlastout, ylastout, iOutCount;
int nPoints[1], nBezierPoints = 0;

lpOutPoint = lpFirstPoint = lpPoint;
iOutCount = 0;
xlast = ylast = xlastout = ylastout = 32767;
for (i=0; i<count; i++)
	{
	x = lpPoint->x;
	y = lpPoint->y;
	if (x == BEZIER_MARKER)
		{
		lpOutPoint->x = x;
		lpOutPoint->y = y;
		nBezierPoints = y;
		lpOutPoint++;
		iOutCount++;
		xlast = ylast = xlastout = ylastout = 32767;
		}
	else
	if ((abs(x-xlast) + abs(y-ylast) != 1) || nBezierPoints)
		{ /* Don't bother with 1 pixel vectors */
		xlast = x;
		ylast = y;
		Display2File( &x, &y ); // Ted uses a special Display2File here
		if ( abs(x-xlastout) + abs(y-ylastout) != 1 || nBezierPoints)
			{ /* Don't bother with 1 pixel hires vectors */
			xlastout = x;
			ylastout = y;
			lpOutPoint->x = x;
			lpOutPoint->y = y;
			lpOutPoint++;
			iOutCount++;
			}
		if (nBezierPoints)
			--nBezierPoints;
		}
	lpPoint++;
	}

nPoints[0] = iOutCount;
SetNewMask( lpFirstPoint, nPoints, 1, fCircle );
Marquee(YES);
}

/************************************************************************/
void RemoveMask()
/************************************************************************/
{
LPFRAME lpFrame;
LPMASK lpMask;
RECT maskRect;

if (!(lpFrame = frame_set(NULL)))
    return;
if ( lpMask = lpFrame->WriteMask )
    { /* unlink and close any mask */
    maskRect = lpMask->rMask;
    File2DispRect(&maskRect, &maskRect);
    InflateRect(&maskRect, 1, 1);
    mask_unlink( lpMask, WRITE_MASK );
    mask_close( lpMask );
    PaintImage(Window.hDC, &maskRect, NULL);
    }
}

