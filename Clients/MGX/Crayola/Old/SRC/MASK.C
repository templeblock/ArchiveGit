//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
/*=======================================================================*\

	MASK.C - Mask Management routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include <math.h>
#include "mask.h"
#include "routines.h"

typedef struct _shapedesc
	{
	int     nPoints;
	int     iModes;
	LPPOINT lpPoints;
} SHAPEDESC, far *LPSHAPEDESC;

// Static prototypes
static BOOL AlphaBuildLine( LPMASK lpMask, int y, LPSHAPEDESC lpShapeList, int nShapes );
static long accuratelsqrt(long val);

#define GetMinMax( val1, val2, Min, Max ) \
{ if ((val1) < (val2)) \
	 {(Min) = (val1); (Max) = (val2);} \
else {(Min) = (val2); (Max) = (val1);} }

/************************************************************************/
LPMASK CALLBACK EXPORT MaskCreate (LPFRAME lpAlphaIn, int iMaxWidth,
								int iMaxHeight, BOOL fOn, BOOL fNoUndo)
/************************************************************************/
{
LPFRAME lpAlpha;
int dx, dy;
LPTR lp;
LPMASK lpMask;

lpAlpha = lpAlphaIn;
// Create an alpha channel
if (lpAlpha)
	{
	dx = FrameXSize(lpAlpha);
	dy = FrameYSize(lpAlpha);
	}
else
	{
	dx = iMaxWidth;
	dy = iMaxHeight;
	if ( !(lpAlpha = FrameOpen( FDT_GRAYSCALE, dx, dy, 75 )) )
		return( NULL );
	// Force all lines to be set properly
	FrameSetBackground( lpAlpha, (BYTE)( fOn ? 255 : 0 ) );
	}

if (!(lpMask = (LPMASK)Alloc((long)(sizeof(MASK)+dx))))
	{
	FrameClose(lpAlpha);
	return( NULL );
	}
lp = (LPTR)lpMask;
lpMask->mbuffer = lp + sizeof(MASK);
lpMask->bInvert = NO;
lpMask->bInvertState = NO;
PixmapSetup(&lpMask->Pixmap, lpAlpha, fNoUndo);
if (fOn || lpAlphaIn)
	SetRect(&lpMask->rMask, 0, 0, dx-1, dy-1);
else
	AstralSetRectEmpty(&lpMask->rMask);
return(lpMask);
}

/***********************************************************************/
LPFRAME CALLBACK EXPORT MaskCreateAlphaFrame (LPFRAME lpFrame)
/***********************************************************************/
//  This function creates a mask frame from a 24-bit color frame
//  whose upper-leftmost pixel contains the color to be used as
//  the transparency color.  The mask frame returned is a grayscale
//  frame containing black pixels in for all the transparent pixels
//  in the source frame, and white pixels for all other source colors.
{
    LPFRAME lpMask = NULL;

    if (FrameDepth (lpFrame)==3)
    {
        int Width  = FrameXSize (lpFrame);
        int Height = FrameYSize (lpFrame);
        RGBS White;
        White.red   = 0xFF;
        White.green = 0xFF;
        White.blue  = 0xFF;

        lpMask = FrameOpen (FDT_GRAYSCALE,Width,Height,
                            FrameResolution (lpFrame));
        if (lpMask)
        {
            RGBS TransparencyColor;
            LPTR lpSrcLine = FramePointer (lpFrame,0,0,FALSE);
            int x,y;

            FrameGetRGB (lpFrame,lpSrcLine,&TransparencyColor,1);
            for (y=0;y<Height;y++)
            {
                LPTR lpDstLine = FramePointer (lpMask,0,y,TRUE);

                FrameSetPixel (lpMask,0,lpDstLine,Width);
                lpSrcLine = FramePointer (lpFrame,0,y,FALSE);

                for (x=0;x<Width;x++)
                {
                    RGBS Pixel;

                    FrameGetRGB (lpFrame,lpSrcLine,&Pixel,1);
                    if (Pixel.red   != TransparencyColor.red ||
                        Pixel.green != TransparencyColor.green ||
                        Pixel.blue  != TransparencyColor.blue)
                    {
                        FrameSetRGB (lpMask,&White,lpDstLine + x,1);
                    }
                    lpSrcLine += 3;
                }
            }
        }
    }
    return lpMask;
}


/************************************************************************/
BOOL MaskAddPoints( LPMASK lpMask, LPPOINT pnts, LPINT nPoints, int nShapes,
					int iModes, LPRECT lpRect)
/************************************************************************/
{
LPSHAPE lpShapes, lpNextShape;

if (!lpMask)
	return(FALSE);
if ( !(lpShapes = MaskCreateShapes(pnts, nPoints, nShapes, iModes, YES, NULL)) )
	return( FALSE );
MaskAddShapes( lpMask, lpShapes );
if (lpRect)
	{
	AstralSetRectEmpty(lpRect);
	lpNextShape = lpShapes;
	while (lpNextShape)
		{
		AstralUnionRect(lpRect, lpRect, &lpNextShape->rShape);
		lpNextShape = lpNextShape->lpNext;
		}
	}
FreeUpShapes(lpShapes);
return(TRUE);
}

/************************************************************************/
BOOL MaskAddShapes( LPMASK lpMask, LPSHAPE lpShape)
/************************************************************************/
{
LPPOINT lpFirstPoint;
SHAPEDESC ShapeDesc;
RECT rMask;
int y, nPoints, dx, dy;

if ( !lpMask )
	return(FALSE);

if ( !lpShape )
	return( FALSE );

PixmapGetInfo(&lpMask->Pixmap, PMT_EDIT, &dx, &dy, NULL, NULL);
while ( lpShape )
	{ // Update the alpha channel
	// get number of points needed for this shape
	if ( !(nPoints = ShapeConvertPoints(lpShape, NULL)) )
		{
		lpShape = lpShape->lpNext;
		continue;
		}

	if (!(lpFirstPoint = (LPPOINT)Alloc((long)nPoints*sizeof(POINT))))
		{
		lpShape = lpShape->lpNext;
		continue;
		}

	ShapeConvertPoints(lpShape, lpFirstPoint);
	ShapeDesc.nPoints = nPoints;
	ShapeDesc.iModes = lpShape->iModes;
	ShapeDesc.lpPoints = lpFirstPoint;

	AstralUnionRect(&lpMask->rMask, &lpShape->rShape, &lpMask->rMask);
	BoundRect( &lpMask->rMask, 0, 0, dx-1, dy-1 );

	// Stuff each line of the alpha channel
	rMask = lpShape->rShape;
	BoundRect( &rMask, 0, 0, dx-1, dy-1 );
	for (y = rMask.top; y <= rMask.bottom; ++y)
		{
		AlphaBuildLine( lpMask, y, &ShapeDesc, 1 );
		}

	lpShape = lpShape->lpNext;
	}
return( TRUE );
}

//************************************************************************
//	Adds an ellipse insode the given rectangle into the mask
//************************************************************************
void MaskAddEllipse( LPMASK lpMask, LPRECT lpRect, int iModes)
//************************************************************************
{
	RECT rMask;
	int dx, dy, width, height;
	LFIXED center_x, center_y;
	int y, x1, x2;
	LFIXED a,b,binv;
	LPTR lpAlphaLine;
	LFIXED t1, y2;

	if ( !lpMask )
		return;
	ProgressBegin(1,0);
	PixmapGetInfo(&lpMask->Pixmap, PMT_EDIT, &dx, &dy, NULL, NULL);

	if (lpMask->bInvert)
	{
		if (iModes == SHAPE_ADD)
			iModes = SHAPE_SUBTRACT;
		else
		if (iModes == SHAPE_SUBTRACT)
			iModes = SHAPE_ADD;
	}

	AstralUnionRect(&lpMask->rMask, lpRect, &lpMask->rMask);
	BoundRect( &lpMask->rMask, 0, 0, dx-1, dy-1 );

	// Stuff each line of the alpha channel
	rMask = *lpRect;
	width = RectWidth(lpRect);
	height = RectHeight(lpRect);
	if (width<=0 || height<=0)
	{
		ProgressEnd();
		return;
	}

	center_x = FGET(width,  2);
	center_y = FGET(height-1, 2);

	a    = FGET(width,  2);
	binv = FGET(2, height);

	BoundRect( &rMask, 0, 0, dx-1, dy-1 );

	for (y = rMask.top; y<= rMask.bottom; ++y)
	{
		// compute xcenter(+-)a*sqrt(1-y^2/b^2)
		if (!(lpAlphaLine = PixmapPtr( &lpMask->Pixmap, PMT_EDIT, rMask.left, y, YES )))
			continue;

		y2 = ((long)(y-rMask.top)*UNITY - center_y);

#ifdef FIXMUL
		t1 = FIXMUL(y2, binv);
		t1 = -FIXMUL(t1,t1)+FUNITY;
#else
		t1 = fmul(y2, binv);
		t1 = UNITY-fmul(t1,t1);
#endif
		if (t1<0)
			continue;

#ifdef FIXMUL
		b = DBL2FIX(sqrt(DOUBLE(t1)));
		t1 = FIXMUL(a,b);
#else
		t1 = fmul(a,(long)(sqrt(t1/65536.0)*65536.0));
#endif
		x1 = (long)(center_x-t1) >> 16L;
		x2 = (long)(center_x+t1) >> 16L;

		if (x1 < 0)
			x1 = 0;

		if (x2 > width-1)
			x2 = width-1;

		if (iModes == SHAPE_ADD)
			set(lpAlphaLine+x1,(x2-x1)+1, 255);
		else if (iModes == SHAPE_SUBTRACT)
			clr(lpAlphaLine+x1,(x2-x1)+1);
		else
			negate(lpAlphaLine+x1,(x2-x1)+1);
	}
	ProgressEnd();
}

/************************************************************************/
LPFRAME StrokeEllipse(HMGXBRUSH hBrush, LPRECT lpEllipseRect, LPRECT lpOutRect)
/************************************************************************/
{
	int	BrushFull, BrushHalf;
	LPFRAME lpFrame;
	RECT rMask, rMax;
	int width, height;
	LFIXED center_x, center_y;
	int y, x1, x2;
	int x1last, x2last;
	LFIXED a,binv;
	LFIXED t1, y2;
	int iSavedStrokeMethod;
	
	
	BrushFull = GetMgxBrushSize(hBrush);
	BrushHalf = BrushFull / 2;
	rMax = *lpEllipseRect;
	rMax.top -= BrushHalf;
	rMax.left -= BrushHalf;
	rMax.bottom += (BrushFull-BrushHalf);
	rMax.right += (BrushFull-BrushHalf);
						
	iSavedStrokeMethod = SetMgxBrushStrokeMethod(hBrush, SM_UNCONNECTED);
	if (!MgxOSBrushBegin(hBrush, lpEllipseRect, &rMax))
		return(NULL);
	// Stuff each line of the alpha channel
	rMask = *lpEllipseRect;
	width = RectWidth(lpEllipseRect);
	height = RectHeight(lpEllipseRect);
	if (width<=0 || height<=0)
		return(NULL);
	ProgressBegin(1,0);
	center_x = FGET(width,2);
	// compute a<<2 where a=(2*width-1)/4
	a = ((2*(long)width-1)<<2L)/4L;
	center_y = FGET(height-1,2);
	// compute (1/b)<<26 where b=(2*height-1)/4
#ifdef WIN32
	binv  = (FUNITY*4)/(2*height-1);
	x2last = x1last = WHOLE(center_x);
#else	
	binv  = ((4*UNITY)<<10L)/(2*height-1);
	x2last = (long)(x1last = center_x) >> 16L;
#endif	
	for (y = rMask.top; y<= rMask.bottom; ++y)
	{
#ifdef WIN32		
		AstralClockCursor(y-rMask.top, width, NO);
		// compute xcenter(+-)a*sqrt(1-y^2/b^2)
		// compyte y2 = yposition
		y2 = (FUNITY * (y-lpEllipseRect->top) - center_y);
		t1 = y2*binv;
		t1 = (FUNITY)-t1*t1;
		if (t1<0)
			continue;
		t1 = a*accuratelsqrt(t1);			 
		x2 = WHOLE(center_x+t1);
		x1 = WHOLE(center_x-t1);
#else
		AstralClockCursor(y-rMask.top, width, NO);
		// compute xcenter(+-)a*sqrt(1-y^2/b^2)
		// compyte y2 = yposition<<2
		y2 = ((long) ((long) (y-lpEllipseRect->top) * UNITY - center_y)) >> 14L;
		t1 = (long)(y2*binv) >> 13L;
		// so t1 is now <<15 
		t1 = (UNITY<<14L)-t1*t1;
		if (t1<0)
			continue;
		t1 = ((long) (a*(long)accuratelsqrt(t1))) >> 1L;			 
		// t1 is now <<16
		x2 = ((long) (center_x+t1)) >> 16L;
		x1 = ((long) (center_x-t1)) >> 16L;
#endif	
		if (x1<0)
			x1 = 0;
		if (x2>=width)
			x2 = width-1; 
		while (x1last>x1)
			MgxOSBrushStroke(rMask.left+x1last--, y);
		while (x1last<x1)
			MgxOSBrushStroke(rMask.left+x1last++, y-1);
		MgxOSBrushStroke(rMask.left+x1, y);
		while (x2last>x2)
			MgxOSBrushStroke(rMask.left+x2last--, y-1);
		while (x2last<x2)
			MgxOSBrushStroke(rMask.left+x2last++, y);
		MgxOSBrushStroke(rMask.left+x2, y);
	}
	x2 = x1 = ((long)center_x)>>16;
	while (x1last<=x1)
		MgxOSBrushStroke(rMask.left+x1last++, y-1);
	while (x2last>x2)
	 	MgxOSBrushStroke(rMask.left+x2last--, y-1);
	lpFrame = MgxOSBrushEnd(lpOutRect);
	SetMgxBrushStrokeMethod(hBrush, iSavedStrokeMethod);
	ProgressEnd();
	return(lpFrame);
}



//************************************************************************
LPSHAPE MaskCreateShapes( LPPOINT pnts, LPINT nPoints, int nShapes, int iModes,BOOL bClosed,LPTFORM lpTForm )
//************************************************************************
{
LPTR lpMemory;
LPSHAPE lpShape, lpNextShape, lpHeadShape;
LPPOINT lpPoints;
int count, i, x, y;
long lMemSize;

if ( !pnts )
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
	lpShape->nPoints = nPoints[i];
	lpShape->iModes = iModes;
	AstralSetRectEmpty(&lpShape->rShape);
	if (lpTForm)
		lpShape->tform = *lpTForm;
	else
		TInit(&lpShape->tform);
	lpShape->bClosed = bClosed;
	lpShape->lpNext = NULL;

	/* Link the new shape to the bottom of the list */
	if ( !lpHeadShape )
		lpHeadShape = lpShape;
	else
		{
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
		if (IS_BEZIER(lpPoints))
			{
			lpPoints++;
			continue;
			}
	
		Transformer(&lpShape->tform, lpPoints, &x, &y);
		/* Update the shape's outer bounds */
		if ( x < lpShape->rShape.left )
			lpShape->rShape.left = x;
		if ( x > lpShape->rShape.right )
			lpShape->rShape.right = x;
		if ( y < lpShape->rShape.top )
			lpShape->rShape.top = y;
		if ( y > lpShape->rShape.bottom)
			lpShape->rShape.bottom = y;
		lpPoints++;
		}
	}

return(lpHeadShape);
}

/************************************************************************/
static BOOL AlphaBuildLine( LPMASK lpMask, int y, LPSHAPEDESC lpShapeList, int nShapes )
/************************************************************************/
{
int i, x, ix, iModes, iCount, miny, maxy, dx, yFirst, minx, maxx;
LPPOINT lpFirstPoint, lpLastPoint, lpPoint, lpNextPoint;
long lDx, lDy, lDy1;
LPTR lpAlphaLine, lpBuffer, lpB, lpA;
BYTE State, c, Start;
BOOL bDown;

if ( !lpMask )
	return( NO );
if ( !(lpAlphaLine = PixmapPtr( &lpMask->Pixmap, PMT_EDIT, 0, y, YES )) )
	return( NO );

lpBuffer = lpMask->mbuffer;
PixmapGetInfo(&lpMask->Pixmap, PMT_EDIT, &dx, NULL, NULL, NULL);

// loop on all shapes and get all edges for this y
for (i = 0; i < nShapes; ++i)
	{
	Start = 0;
	if ( !(iCount = lpShapeList[i].nPoints) )
		continue;
	iModes = lpShapeList[i].iModes;
	if (lpMask->bInvert)
		{
		if (iModes == SHAPE_ADD)
			iModes = SHAPE_SUBTRACT;
		else
		if (iModes == SHAPE_SUBTRACT)
			iModes = SHAPE_ADD;
		}
	lpPoint = lpFirstPoint = lpShapeList[i].lpPoints;
	lpLastPoint = lpFirstPoint + (iCount - 1);

	// loop on the points in this shape, and set the edges in the buffer
	clr( lpBuffer, dx );

	// special case horizontal vectors and 1 pixel high rectangles
	yFirst = lpPoint->y; 
	minx = maxx = lpPoint->x;		
	--iCount;
	++lpPoint;
	while (--iCount >= 0)
		{
		if (lpPoint->y != yFirst)
			break;
		minx = min(minx, lpPoint->x);
		maxx = max(maxx, lpPoint->x);
		++lpPoint;
		}
	// do we have a horizontal line?
	if (iCount < 0)
		{
		if ( minx >= 0 && minx < dx )
			lpBuffer[ minx ]++;
		else if (minx < 0)
			Start ^= 255;
		if ( maxx >= 0 && maxx < dx )
			lpBuffer[ maxx ]++;
		else if (maxx < 0)
			Start ^= 255;
		}
	
	iCount = lpShapeList[i].nPoints;
	lpPoint = lpFirstPoint;
	bDown = ( lpFirstPoint->y > lpLastPoint->y );

	while (--iCount >= 0)
		{
		// get next point in vector list
		if ( iCount )
				lpNextPoint = lpPoint+1;
		else	lpNextPoint = lpFirstPoint;

		// get y-extents of this vector
		GetMinMax(lpPoint->y, lpNextPoint->y, miny, maxy);

		// if outside our y, or horizontal, discard immediately	(single point ok)
		if ( y < miny || y > maxy ||
			(miny == maxy && lpNextPoint->x != lpPoint->x))
			{
			++lpPoint;
			continue;
			}

		// find intersection of this vector and horizontal at y
		if ( !(lDx = lpNextPoint->x - lpPoint->x) )
			x = lpPoint->x; // vertical
		else
			{ // calc intersection point
			if ( !(lDy1 = y - lpPoint->y) )
				x = lpPoint->x;
			else
				{
				lDy = lpNextPoint->y - lpPoint->y;
				lDy1 *= lDx;
				// adjust for rounding
				if (lDy1 < 0)
						lDy1 -= abs(lDy)/2L;
				else	lDy1 += abs(lDy)/2L;
				x = (lDy1 / lDy) + lpPoint->x;
				}
			}

		// Bump the edge count
		if ( x >= 0 && x < dx )
			{
			lpBuffer[ x ]++;
			// For each vector start, compare direction with the most recent end
			// If the same direction, bump the edge count again
			if ( y == lpPoint->y )
				if ( bDown == ( lpNextPoint->y > lpPoint->y ))
					lpBuffer[ x ]++;
			}
		else if (x<0)
			{
			Start ^= 255;
			// For each vector start, compare direction with the most recent end
			// If the same direction, bump start value again
			if ( y == lpPoint->y )
				if ( bDown == ( lpNextPoint->y > lpPoint->y ))
					Start ^= 255;
			}

		// Save the most recent end direction
		if ( y == lpNextPoint->y )
			bDown = ( lpNextPoint->y > lpPoint->y );

		// found the possible bottom of an apex on the next point
		++lpPoint;
		}

	// Do a fill on the buffer as we set the bytes in the alpha channel
	State = Start;
	lpA = lpAlphaLine;
	lpB = lpBuffer;
	for ( ix=0; ix<dx; ix++ )
		{
		if ( !(c = *lpB++) )
			c = State;
		else
			{
			if ( c & 1 )
				State ^= 255;
			c = 255;
			}
		if ( iModes == SHAPE_ADD )
			{
			if ( c )
				*lpA |= c;
			}
		else
		if ( iModes == SHAPE_SUBTRACT )
			{
			if ( c )
				*lpA &= ~c; // beware the squiggle in XyWrite
			}
		else
		// if ( iModes == SHAPE_REVERSE )
			{
			*lpA ^= c;
			}
		lpA++;
		}
	}

return( YES );
}

/************************************************************************/
BOOL CALLBACK EXPORT MaskClose( LPMASK lpMask )
/************************************************************************/
{
if ( !lpMask )
	return( NO );

/* Free up the alpha frame */
PixmapFree(&lpMask->Pixmap);

/* Free up the mask itself */
FreeUp( (LPTR)lpMask );
return( YES );
}


/************************************************************************/
BOOL MaskRect( LPMASK lpMask, LPRECT lpRect )
/************************************************************************/
{
LPFRAME lpAlpha;

if ( !lpMask || !(lpAlpha = lpMask->Pixmap.EditFrame))
	{
	SetRectEmpty( lpRect );
	return( FALSE );
	}

// Start with the assumption of the entire image
lpRect->left = 0; lpRect->right = FrameXSize(lpAlpha) - 1;
lpRect->top = 0; lpRect->bottom = FrameYSize(lpAlpha) - 1;

if ( lpMask->bInvert )
	return( TRUE );

*lpRect = lpMask->rMask;
BoundRect( lpRect, 0, 0, FrameXSize(lpAlpha)-1, FrameYSize(lpAlpha)-1 );
return( TRUE );
}


/************************************************************************/
BOOL MaskRectUpdate( LPMASK lpMask, LPRECT lpRect )
/************************************************************************/
{
LPFRAME lpAlphaFrame;
int dx, dy;

if ( !lpMask || !(lpAlphaFrame = lpMask->Pixmap.EditFrame))
	{
	SetRectEmpty( lpRect );
	return( FALSE );
	}
AstralCursor( IDC_WAIT );
dx = FrameXSize(lpAlphaFrame);
dy = FrameYSize(lpAlphaFrame);
SetRect(lpRect, 0, 0, dx-1, dy-1);
if (!MaskRectDwindle(lpMask, lpRect, NULL))
	{// assume the entire image and return FALSE
	SetRect( &lpMask->rMask, 0, 0, dx-1, dy-1 );
	*lpRect = lpMask->rMask;
	AstralCursor( NULL );
	return( FALSE );
	}

lpMask->rMask = *lpRect;
if ( lpMask->bInvert )
	{ // Pass back the entire image if the mask is inverted
	SetRect( lpRect, 0, 0, dx-1, dy-1 );
	}
AstralCursor( NULL );
return( TRUE );
}

/************************************************************************/
BOOL MaskRectDwindle( LPMASK lpMask, LPRECT lpRect, LPFRAME lpAlphaFrame )
/************************************************************************/
{
long lx, lx1, lx2, lCount;
int x1, x2, y, y1, y2, dx, dy, depth;
BOOL bLeftEdge, bRightEdge;
LPTR lp;
RECT rAlpha;

if ( !lpAlphaFrame && (!lpMask || !(lpAlphaFrame = lpMask->Pixmap.EditFrame)))
	return( FALSE );

SetRect(&rAlpha, 0, 0, FrameXSize(lpAlphaFrame)-1, FrameYSize(lpAlphaFrame)-1);
if (!AstralIntersectRect(lpRect, lpRect, &rAlpha))
	return(FALSE);

dx = RectWidth(lpRect);
dy = RectHeight(lpRect);
if ( !(depth = FrameDepth(lpAlphaFrame)) )
	depth = 1;

for ( y1 = lpRect->top; y1 <= lpRect->bottom; y1++ )
	{
	if ( !(lp = FramePointer( lpAlphaFrame, lpRect->left, y1, NO )) )
		break;
	lCount = (long)dx * depth;
	while ( --lCount >= 0 )
		{
		if ( *lp++ )
			goto FoundTop;
		}
	}

FoundTop:

if ( y1 > lpRect->bottom ) // If the mask is empty, ...
	return(FALSE);

for ( y2 = lpRect->bottom; y2 > y1; y2-- )
	{
	if ( !(lp = FramePointer( lpAlphaFrame, lpRect->left, y2, NO )) )
		break;
	lCount = (long)dx * depth;
	while ( --lCount >= 0 )
		{
		if ( *lp++ )
			goto FoundBottom;
		}
	}

FoundBottom:

lx1 = +999999999;
lx2 = -999999999;
lCount = (long)dx * depth;
bLeftEdge = NO;
bRightEdge = NO;
for ( y = y1; y <= y2; y++ )
	{
	if ( !(lp = FramePointer( lpAlphaFrame, lpRect->left, y, NO )) )
		break;
	if ( !bRightEdge )
		{ // If its worth doing a search for the right most...
		for ( lx = lCount-1; lx > 0; lx-- )
			{ // Do a byte search
			if ( *(lp+lx) )
				break;
			}
		if ( lx > lx2 )
			{
			lx2 = lx;
			if ( lCount - lx2 <= depth )
				bRightEdge = YES;
			}
		}
	if ( !bLeftEdge )
		{ // If its worth doing a search for the left most...
		for ( lx = 0; lx < lCount-1; lx++ )
			{ // Do a byte search
			if ( *(lp+lx) )
				break;
			}
		if ( lx < lx1 )
			{
			lx1 = lx;
			if ( lx1 < depth )
				bLeftEdge = YES;
			}
		}
	if ( bLeftEdge && bRightEdge )
		break;
	}

// Convert byte counts to pixel counts
x1 = lpRect->left + (lx1 / depth);
x2 = lpRect->left + (lx2 / depth);

// Set the mask rectangle
SetRect( lpRect, x1, y1, x2, y2 );
return(TRUE);
}

/************************************************************************/
int IsMasked( LPMASK lpMask, int x, int y )
/************************************************************************/
{
BYTE State;

if ( !MaskLoad( lpMask, x, y, 1, &State ) )
	return( 0 );
return( State );
}


/************************************************************************/
BOOL MaskLoad( LPMASK lpMask, int x, int y, int dx, LPTR lpM )
/************************************************************************/
{
BOOL bRet;

if ( !(bRet = MaskLoader( lpMask, x, y, dx, lpM, NO, CR_COPY )) )
	clr( lpM, dx );
return( bRet );
}


/************************************************************************/
BOOL MaskLoader( LPMASK lpMask, int x, int y, int iCount, LPTR lpM,
				BOOL bInvert, COMBINE_ROP ROP)
/************************************************************************/
{
LPFRAME lpAlpha;
LPTR lp;
int iCountMax, Xsize, iCountClear = 0;
int lastx;
BYTE val;

if (bInvert)
	val = 0xff;
else
	val = 0x00;
	
if ( !lpMask )
{
	set( lpM, iCount, ~val); //squiggle
	return(YES);
}
if ( !(lpAlpha = lpMask->Pixmap.EditFrame) )
{
	set( lpM, iCount, ~val); //squiggle
	return( YES );
}
if ( y < 0 || y >= lpAlpha->Ysize )
{
	if (ROP == CR_COPY)
	{
		set(lpM, iCount, val);
		return(YES);
	}
	return( NO);
}
Xsize = lpAlpha->Xsize;
lastx = x + iCount - 1;
if ( lastx < 0 || x >= Xsize )
{
	if (ROP == CR_COPY)
	{
		set(lpM, iCount, val);
		return(YES);
	}
	return( NO);
}
if (x < 0)
	{
	iCountClear = -x;
	x = 0;
	}
lp = FramePointer( lpAlpha, x, y, NO );
if (iCountClear)
	{
	if (ROP == CR_COPY)
		set(lpM, iCountClear, val);
	lpM += iCountClear;
	iCount -= iCountClear;
	iCountClear = 0;
	}
iCountMax = Xsize - x;
if (iCount > iCountMax)
	{
	if (ROP == CR_COPY)
		iCountClear = iCount - iCountMax;
	iCount = iCountMax;
	}
if (lp)
{
	bInvert = (bInvert ^ lpMask->bInvert);
	CombineData( lp, lpM, iCount, bInvert, ROP );
}
else if (ROP == CR_COPY)
	set(lpM, iCount, val);
else
	return(NO);
	
if (iCountClear)
	if (ROP == CR_COPY)
		set(lpM+iCount, iCountClear, val);

return(YES);
}


/************************************************************************/
int ShapeConvertPoints(
/************************************************************************/
LPSHAPE lpShape,
LPPOINT lpOutputPoint)
{
	return(ExpandPoints(lpShape->lpPoints, 
					lpOutputPoint,
					lpShape->nPoints, 
					&lpShape->tform,
					lpShape->lpPoints,		   
					&(lpShape->lpPoints[lpShape->nPoints-1])));
}


//************************************************************************/
//	lpFirst and lpLast should be the first and last points in the entire
//	list.  They are needed for wrap around.
//************************************************************************/
int ExpandPoints(
/************************************************************************/
LPPOINT 	lpPoint,
LPPOINT 	lpOutputPoint,
int 		count,
LPTFORM 	lpTForm,
LPPOINT 	lpFirst,
LPPOINT 	lpLast)
{
int nPoints, nBezier, nTotalBezier, n, type;
POINT bezierPoints[MAX_BEZIER_IN];

nPoints = 0;
while ( --count >= 0 )
	{
	if (IS_BEZIER(lpPoint))
		{
		type = lpPoint->x;
		nBezier = BEZIER_POINTS(lpPoint);
		// get bezier points with wrap around
		nTotalBezier = GetBezierPoints(bezierPoints, lpPoint, lpFirst, lpLast);
		lpPoint++;
		count -= nBezier;
		lpPoint += nBezier;
		n = BezierToPoints( lpTForm, bezierPoints, nTotalBezier,
			type, lpOutputPoint );
		nPoints += n;
		if (lpOutputPoint)
			lpOutputPoint += n;
		}
	else
		{
		if (lpOutputPoint)
			{
			Transformer(lpTForm, lpPoint,
				(LPINT)&lpOutputPoint->x, (LPINT)&lpOutputPoint->y);
			lpOutputPoint++;
			}
		/* Update the shape's outer bounds */
		++nPoints;
		lpPoint++;
		}
	}

return(nPoints);
}

/************************************************************************/
//	fills lpOutPoint with bezier points (using wrap around)
//	lpBezPoints should include bezier marker.
//	lpFirst, lpLast should be the first and last points overall.
/************************************************************************/
int GetBezierPoints(LPPOINT lpOutPoints, LPPOINT lpBezPoint, LPPOINT lpFirst, LPPOINT lpLast)
/************************************************************************/
{
	int nBezier, nTotalBezier;
	int j;
		
	if (IS_BEZIER(lpFirst))
		lpFirst++;
	nBezier = BEZIER_POINTS(lpBezPoint);
	nTotalBezier = BEZIER_TOTAL_POINTS(lpBezPoint);
	// get bezier points with wrap around
	j=0;
	while(j < nTotalBezier)
	{
		if (lpBezPoint == lpLast)
			lpBezPoint = lpFirst;
		else
			lpBezPoint++;
		if (IS_BEZIER(lpBezPoint))
			continue;	
		*lpOutPoints++ = *lpBezPoint;
		j++;
	}
	return(j);
}

/************************************************************************/
//	type == BEZIER_MARKER -> end point is included in nPoints.
//	type == OPEN_BEZIER_MARKER -> end point is point AFTER nPoints.
/************************************************************************/
int BezierToPoints( LPTFORM lpTForm, LPPOINT lpPoints, int nPoints, int type, LPPOINT lpOutputPoints )
/************************************************************************/
{
int n, t, i, nOutPoints, lastx, lasty, x, y;
LFIXED tscale, fdx, fdy;
FPOINT ptR[MAX_BEZIER_IN];
LPFPOINT R;
LPPOINT P;

if ( nPoints > MAX_BEZIER_IN )
	nPoints = MAX_BEZIER_IN;
Transformer(lpTForm, lpPoints, &lastx, &lasty);
if (lpOutputPoints)
	{
	lpOutputPoints->x = lastx;
	lpOutputPoints->y = lasty;
	lpOutputPoints++;
	}
nOutPoints = 1;
for ( t=1; t<MAX_BEZIER; t++ )
	{
	R = ptR;
	P = lpPoints;
	n = nPoints;
	while (--n >= 0)
		{
		if (IS_BEZIER(P))
			P++;
		FTransformer(lpTForm, P, &R->fx, &R->fy);
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
			fdx = (R+1)->fx - R->fx;
			fdy = (R+1)->fy - R->fy;
#ifdef FIXMUL			
			R->fx += FIXMUL( fdx, tscale );
			R->fy += FIXMUL( fdy, tscale );
#else			
			R->fx += FMUL( fdx, tscale );
			R->fy += FMUL( fdy, tscale );
#endif			
			R++;
			}
		}
#ifdef FIXMUL			
	x = FIXMUL(ptR[0].fx, 1);
	y = FIXMUL(ptR[0].fy, 1);
#else	
	x = FMUL(ptR[0].fx, 1);
	y = FMUL(ptR[0].fy, 1);
#endif			
	if (x != lastx || y != lasty)
		{
		lastx = x;
		lasty = y;
		if (lpOutputPoints)
			{
			lpOutputPoints->x = lastx;
			lpOutputPoints->y = lasty;
			lpOutputPoints++;
			}
		++nOutPoints;
		}
	}

n = nPoints - 1;
#ifdef FIXMUL			
x = FIXMUL(ptR[n].fx, 1);
y = FIXMUL(ptR[n].fy, 1);
#else	
x = FMUL(ptR[n].fx, 1);
y = FMUL(ptR[n].fy, 1);
#endif			
if (x != lastx || y != lasty)
	{
	if (lpOutputPoints)
		{
		lpOutputPoints->x = x;
		lpOutputPoints->y = y;
		}
	++nOutPoints;
	}
return( nOutPoints );
}

/************************************************************************/
void FreeUpShapes( LPSHAPE lpShape )
/************************************************************************/
{
LPSHAPE lpNextShape;

while ( lpShape )
	{
	lpNextShape = lpShape->lpNext;
	FreeUp( (LPTR)lpShape );
	lpShape = lpNextShape;
	}
}


static long accuratelsqrt(long val)
{
	double fval;
	
	fval = val;
	return((long)sqrt(val));
}

