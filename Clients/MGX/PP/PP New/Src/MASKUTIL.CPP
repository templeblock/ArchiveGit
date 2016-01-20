//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
/*=======================================================================*\

	MASKUTIL.C - Mask Utility functions.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "pp.h"
#include "id.h"
 
typedef struct _shapedesc
	{
	int     nPoints;
	int     iModes;
	LPPOINT lpPoints;
} SHAPEDESC, far *LPSHAPEDESC;

// Static prototypes
static BOOL AlphaBuildLine( LPMASK lpMask, int y, LPSHAPEDESC lpShapeList);
static BOOL AlphaBuildAntiLine( LPMASK lpMask, int y, LPSHAPEDESC lpShapeList, LPTR lpInBuffer, long lInBufferSize);

LOCAL void dsort(double FAR * lpList, int num);

static long accuratelsqrt(long val);
static LFIXED accuratefsqrt(LFIXED val);
LOCAL BOOL mask_add_poly( LPMASK lpMask,LPCPolyList lpPolyList, LPCPoly lpPoly, int iMode, BOOL bAntialias);
#define GetMinMax( val1, val2, Min, Max ) \
{ if ((val1) < (val2)) \
	 {(Min) = (val1); (Max) = (val2);} \
else {(Min) = (val2); (Max) = (val1);} }

#define ANTI_RES 5
#define ANTI_SPACE .20
#define HALF_ANTI_SPACE .10

/************************************************************************/
BOOL MaskAddPoly( LPMASK lpMask, LPCPolyList lpPolyList, int iMode, BOOL bAntiAlias)
/************************************************************************/
{
	return(mask_add_poly(lpMask,lpPolyList,NULL,iMode,bAntiAlias));
}

/************************************************************************/
BOOL MaskAddPoly( LPMASK lpMask, LPCPoly lpPoly, int iMode, BOOL bAntiAlias)
/************************************************************************/
{
	return(mask_add_poly(lpMask,NULL,lpPoly,iMode,bAntiAlias));
}

/************************************************************************/
LOCAL BOOL mask_add_poly( LPMASK lpMask,LPCPolyList lpPolyList, LPCPoly lpPoly, int iMode, BOOL bAntialias)
/************************************************************************/
{
	POSITION pos;
	LPPOINT lpFirstPoint;
	SHAPEDESC ShapeDesc;
	RECT rShape;
	int y, nPoints, dx, dy;
	FRMDATATYPE type;
	LPTR lpBuffer = NULL;
	long lBufferSize;
	
	if ( !lpMask )
		return(FALSE);
	if ( !lpPolyList && !lpPoly)
		return( FALSE );
	
	PixmapGetInfo(&lpMask->Pixmap, PMT_EDIT, &dx, &dy, NULL, &type);
	if (type == FDT_LINEART)
		bAntialias = FALSE;
	
	if (lpPolyList)
	{
		pos = lpPolyList->GetHeadPosition();
		lpPoly = (LPCPoly)lpPolyList->GetNextPtr(pos);
	}
	while(lpPoly)
	{
	
		// expand
		lpFirstPoint = lpPoly->GetData(-1, NULL, &nPoints);
		if (lpFirstPoint)
		{	
			if (bAntialias)
			{
				// get buffer to speed antialiasing
				lBufferSize = nPoints*ANTI_RES*sizeof(double);
#ifdef WIN32
				lBufferSize = min(lBufferSize, 65400);
#endif
				if (lBufferSize <= dx) 
				{
					lpBuffer = lpMask->mbuffer;
					lBufferSize = dx;
				}
				if (!(lpBuffer = Alloc(lBufferSize)))
					lBufferSize = 0;
			}
			ShapeDesc.nPoints = nPoints;
			ShapeDesc.iModes = iMode;
			ShapeDesc.lpPoints = lpFirstPoint;
			lpPoly->GetBounds(&rShape);
			rShape.right--;
			rShape.bottom--;
			AstralUnionRect(&lpMask->rMask, &rShape, &lpMask->rMask);
			BoundRect( &lpMask->rMask, 0, 0, dx-1, dy-1 );
			// Stuff each line of the alpha channel
			BoundRect( &rShape, 0, 0, dx-1, dy-1 );
			for (y = rShape.top; y <= rShape.bottom; ++y)
			{
				if (bAntialias)
					AlphaBuildAntiLine( lpMask, y, &ShapeDesc, lpBuffer, lBufferSize); 
				else
					AlphaBuildLine( lpMask, y, &ShapeDesc); 
			}
			FreeUp(lpFirstPoint);
			if (lpBuffer && lpBuffer != lpMask->mbuffer)
				FreeUp(lpBuffer);
		}
		if (lpPolyList)
			lpPoly = (LPCPoly)lpPolyList->GetNextPtr(pos);
		else
			lpPoly = NULL;
	}
	
	return(TRUE);
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
	LFIXED a,binv,b;
	LPTR lpAlphaLine;
	LFIXED t1, y2;
	FRMDATATYPE type;
	BOOL bLineArt = FALSE;
		
	if ( !lpMask )
		return;
	ProgressBegin(1,0);
	PixmapGetInfo(&lpMask->Pixmap, PMT_EDIT, &dx, &dy, NULL, &type);
	
	if (type == FDT_LINEART)
		bLineArt = TRUE;
	
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

// Due to accuracy problems we need to do special work for 16.16 Fixed 
// numbers. 
#ifdef USING_FIXED16
	// get (2/height) << 8 because it can be very small
	binv = FGET(512, height);
#else // not using USING_FIXED16 case
	binv = FGET(2, height);
#endif 

	BoundRect( &rMask, 0, 0, dx-1, dy-1 );

	for (y = rMask.top; y<= rMask.bottom; ++y)
	{
		// compute xcenter(+-)a*sqrt(1-y^2/b^2)
		
		if (bLineArt)
			lpAlphaLine = PixmapPtrRaw(&lpMask->Pixmap, PMT_EDIT, 0, y, YES);
		else
			lpAlphaLine = PixmapPtr(&lpMask->Pixmap, PMT_EDIT, rMask.left, y, YES, width);
		if (!lpAlphaLine)
			continue;
			
		y2 = -center_y + (y-rMask.top)*FUNITY;
		
#ifdef USING_FIXED16
		y2>>=8;

		FMUL(y2, binv);
		-FMUL(t1,t1)+FUNITY;

		if (t1<0)
			continue;

		DBL2FIX(sqrt(DOUBLE(t1)));
		FMUL(a,b);
#else

		t1 = y2 * binv; 
		t1 = -( t1 * t1 ) + FUNITY;

		if (t1<0)
			continue;

		b = sqrt( t1 ); 
		t1 = a * b; 

#endif 

		x1 = WHOLE(center_x-t1);
		x2 = WHOLE(center_x+t1);

		if (x1 < 0)
			x1 = 0;

		if (x2 > width-1)
			x2 = width-1;
		if (bLineArt)
		{
			// lineart starts at 0
			if (iModes == SHAPE_ADD)
				LineartSetBits(lpAlphaLine, x1+rMask.left, (x2-x1)+1);
			else if (iModes == SHAPE_SUBTRACT)
				LineartClearBits(lpAlphaLine, x1+rMask.left,(x2-x1)+1);
			else
				LineartNegateBits(lpAlphaLine, x1+rMask.left,(x2-x1)+1);
		}
		else
		{
			if (iModes == SHAPE_ADD)
				set(lpAlphaLine+x1,(x2-x1)+1, 255);
			else if (iModes == SHAPE_SUBTRACT)
				clr(lpAlphaLine+x1,(x2-x1)+1);
			else
				negate(lpAlphaLine+x1,(x2-x1)+1);
		}
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
	
// Due to accuracy problems we need to do special work for 16.16 Fixed 
// numbers. 
#ifdef USING_FIXED16
	// compute a<<2 where a=(2*width-1)/4
	a = ((2*(long)width-1)<<2L)/4L;
	center_y = FGET(height-1,2);
	// compute (1/b)<<26 where b=(2*height-1)/4
	binv  = ((4*UNITY)<<10L)/(2*height-1);
#else // not using USING_FIXED16 case
	// compute a=(2*width-1)/4
	a = ((2*(long)width-1))/4L;
	center_y = FGET(height-1,2);
	// compute (1/b) where b=(2*height-1)/4
	binv  = (FUNITY*4)/(2*height-1);
#endif 
	x2last = x1last = WHOLE(center_x);
	for (y = rMask.top; y<= rMask.bottom; ++y)
	{
		AstralClockCursor(y-rMask.top, width, NO);
#ifdef USING_FIXED16
		// compute xcenter(+-)a*sqrt(1-y^2/b^2)
		// compyte y2 = yposition<<2
		y2 = ((long)(y-lpEllipseRect->top)*UNITY - center_y)>>14L;
		t1 = (y2*binv)>>13L;
		// so t1 is now <<15 
		t1 = (UNITY<<14L)-t1*t1;
		if (t1<0)
			continue;
		t1 = (a*(long)accuratelsqrt(t1))>>1L;			 
		// t1 is now <<16
#else // not using USING_FIXED16 case
		// compute xcenter(+-)a*sqrt(1-y^2/b^2)
		// compyte y2 = yposition
		y2 = (FUNITY * (y-lpEllipseRect->top) - center_y);
		t1 = y2*binv;
		t1 = (FUNITY)-t1*t1;
		if (t1<0)
			continue;
		t1 = a*accuratefsqrt(t1);
#endif			 
		x2 = WHOLE(center_x+t1);
		x1 = WHOLE(center_x-t1);
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
	x2 = x1 = WHOLE(center_x);
	while (x1last<=x1)
		MgxOSBrushStroke(rMask.left+x1last++, y-1);
	while (x2last>x2)
	 	MgxOSBrushStroke(rMask.left+x2last--, y-1);
	lpFrame = MgxOSBrushEnd(lpOutRect);
	SetMgxBrushStrokeMethod(hBrush, iSavedStrokeMethod);
	ProgressEnd();
	return(lpFrame);
}

/************************************************************************/
static BOOL AlphaBuildLine( LPMASK lpMask, int y, LPSHAPEDESC lpShapeList)
/************************************************************************/
{
	int x, ix, iModes, iCount, miny, maxy, dx, minx, maxx;
	LPPOINT lpFirstPoint, lpLastPoint, lpPoint, lpNextPoint;
	long lDx, lDy;
	double fy, fDy1;
	LPTR lpAlphaLine, lpBuffer, lpB, lpA;
	BYTE State, Start;
	LPFRAME lpFrame;
	FRMDATATYPE type;
	BOOL bLineArt = FALSE;

	if ( !lpMask )
		return( NO );


	lpBuffer = lpMask->mbuffer;
	PixmapGetInfo(&lpMask->Pixmap, PMT_EDIT, &dx, NULL, NULL, &type);

	if (type == FDT_LINEART)
		bLineArt = TRUE;
	

	// edges for this y
	Start = 0;
	if ( !(iCount = lpShapeList->nPoints) )
		return(TRUE);
	iModes = lpShapeList->iModes;
	if (lpMask->bInvert)
	{
		if (iModes == SHAPE_ADD)
			iModes = SHAPE_SUBTRACT;
		else
		if (iModes == SHAPE_SUBTRACT)
			iModes = SHAPE_ADD;
	}

	// loop on the points in this shape, and set the edges in the buffer
	clr( lpBuffer, dx );

 	lpFirstPoint = lpShapeList->lpPoints;
	lpLastPoint = lpFirstPoint + (iCount - 1);
	lpPoint = lpFirstPoint;
	fy = y+.5;
	while (--iCount >= 0)
	{
		// get next point in vector list
		if ( iCount )
				lpNextPoint = lpPoint+1;
		else	lpNextPoint = lpFirstPoint;

		// get y-extents of this vector
		GetMinMax(lpPoint->y, lpNextPoint->y, miny, maxy);

		// if doesn't cross fy or horizontal then discard
		if ( y < miny || y >= maxy || miny == maxy) //(miny == maxy  && lpNextPoint->x != lpPoint->x))
		{
			++lpPoint;
			continue;
		}

		// find intersection of this vector and horizontal at fy
		if ( !(lDx = lpNextPoint->x - lpPoint->x) )
			x = lpPoint->x; // vertical
		else
		{ // calc intersection point
			fDy1 = fy - lpPoint->y;
		   	lDy = lpNextPoint->y - lpPoint->y;
		   	fDy1 *= lDx;
		   	// round up
		   	x = (int)((fDy1 / lDy) + lpPoint->x + .5);
		}

		// Bump the edge count
		if ( x >= 0 && x < dx )
			lpBuffer[ x ]++;
		else if (x<0)
			Start ^= 255;

		++lpPoint;
	}

	// get the line
   if (bLineArt)
		lpAlphaLine = PixmapPtrRaw(&lpMask->Pixmap, PMT_EDIT, 0, y, YES);
	else
		lpAlphaLine = PixmapPtr(&lpMask->Pixmap, PMT_EDIT, 0, y, YES);
	if (!lpAlphaLine)
		return(NO);

	// Do a fill on the buffer as we set the bytes in the alpha channel
	if (!bLineArt)
	{
		State = Start;
		lpA = lpAlphaLine;
		lpB = lpBuffer;
		for ( ix=0; ix<dx; ix++ )
		{
			if (*lpB++ & 1)
				State ^= 255;
			if (State)
			{
				if ( iModes == SHAPE_ADD )
					*lpA = 0xff;
				else if ( iModes == SHAPE_SUBTRACT )
					*lpA = 0x00; // beware the squiggle in XyWrite
				else // if ( iModes == SHAPE_REVERSE )
				   	*lpA ^= 0xff;
			}
			lpA++;
		}
	}
	else //bLineArt
	{
		// do line art
		int begin;
		State = Start;
		lpB = lpBuffer;
		
		begin = 0;
		for ( ix=0; ix<dx; ix++ )
		{
			if (!(*lpB++ & 1))
				continue;
			// found a change
			if (State)
			{
				// it is an off transition ...backfill (not including this point) 
				if ( iModes == SHAPE_ADD )
					LineartSetBits(lpAlphaLine, begin, ix-begin);
				else if ( iModes == SHAPE_SUBTRACT )
					LineartClearBits(lpAlphaLine, begin, ix-begin);
				else // if ( iModes == SHAPE_REVERSE )
					LineartNegateBits(lpAlphaLine, begin, ix-begin);
			}
			else
				begin = ix;
			State ^= 255;
		}
		
		// fill in last segment
		if (State)
		{
			// it is an off transition ...backfill 
			if ( iModes == SHAPE_ADD )
				LineartSetBits(lpAlphaLine, begin, ix-begin);
			else if ( iModes == SHAPE_SUBTRACT )
				LineartClearBits(lpAlphaLine, begin, ix-begin);
			else // if ( iModes == SHAPE_REVERSE )
				LineartNegateBits(lpAlphaLine, begin, ix-begin);
		}
	}

	return( YES );
}



//************************************************************************
// 	lpInBuffer is a buffer to hold the line crossings for ANTI_RES lines
//	If it is null or too small one will be allocated.  This Is expensive!!
/************************************************************************/
static BOOL AlphaBuildAntiLine( LPMASK lpMask, int y, LPSHAPEDESC lpShapeList, LPTR lpInBuffer, long lInBufferSize)
/************************************************************************/
{
	int a, ix, iModes, iCount, miny, maxy, dx, minx, maxx;
	int cross, lastx;
	int val, v, lastval;
	int index[ANTI_RES];
	LPPOINT lpFirstPoint, lpLastPoint, lpPoint, lpNextPoint;
	long lDy, lDx, l;
	double dDy1;
	double dy, d, dval, x;
	LPTR lpAlphaLine, lpA;
	double * lpB[ANTI_RES];
	double * lpBuffer[ANTI_RES];
	double xx[ANTI_RES];
	BYTE c, lastc;
	BYTE Start[ANTI_RES];
	BYTE State[ANTI_RES];

	if ( !lpMask )
		return( NO );
	if ( !(lpAlphaLine = PixmapPtr( &lpMask->Pixmap, PMT_EDIT, 0, y, YES )) )
		return( NO );

	PixmapGetInfo(&lpMask->Pixmap, PMT_EDIT, &dx, NULL, NULL, NULL);

	iModes = lpShapeList->iModes;
	if (lpMask->bInvert)
	{
		if (iModes == SHAPE_ADD)
			iModes = SHAPE_SUBTRACT;
		else
		if (iModes == SHAPE_SUBTRACT)
			iModes = SHAPE_ADD;
	}
	
	if ( !(iCount = lpShapeList->nPoints) )
		return(TRUE);
	// get edge count for this y (also keep some track of start in case we have a solid line)
	cross = 0;
	Start[0] = 0;
	lpPoint = lpFirstPoint = lpShapeList->lpPoints;
	lpLastPoint = lpFirstPoint + (iCount - 1);
	minx = maxx = lpPoint->x;		
	while (--iCount >= 0)
	{
		// get next point in vector list
		if ( iCount )
			lpNextPoint = lpPoint+1;
		else
			lpNextPoint = lpFirstPoint;

		// get y-extents of this vector
		GetMinMax(lpPoint->y, lpNextPoint->y, miny, maxy);

		// if outside our y, or horizontal, discard immediately	(single point ok)
		// or if past largest x
		if ( y < miny || y >= maxy || (miny == maxy) || (lpPoint->x > dx &&  lpNextPoint->x > dx))
		{
			++lpPoint;
			continue;
		}
		// is it in below x range?
		if (lpPoint->x < 0 &&  lpNextPoint->x < 0) 
		{
			Start[0] ^= 255;
			++lpPoint;
			continue;
		}
		// probable crossing
		cross++;		
		++lpPoint;
	}
	
	// do we need to do anything?
	if (!cross)
	{
		if (!Start[0])
			return(TRUE);
		if ( iModes == SHAPE_ADD )
			set(lpAlphaLine, dx, 255);
		else if ( iModes == SHAPE_SUBTRACT )
			clr(lpAlphaLine, dx);
		else // if ( iModes == SHAPE_REVERSE )
		{
			for ( ix=0; ix<dx; ix++ )
				*lpAlphaLine++ ^= 255;
		}
		return(TRUE);
	}
	// add one for 'closing' cross.
	cross++;
	// allocate buffers for x lists (if needed)
	l = cross*sizeof(double);
	if (l*ANTI_RES <= lInBufferSize)
	{
		for (a=0; a<ANTI_RES;a++)
			lpBuffer[a] = (double FAR *)(lpInBuffer+l*a);
	}
	else	
	{
		for (a=0; a<ANTI_RES;a++)
		{
			lpBuffer[a] = (double *)Alloc(l);
			if (!lpBuffer[a])
			{
				for (;a>0;a--)
					FreeUp((LPTR)lpBuffer[a]);
				Message(IDS_EMEMALLOC);
				return(FALSE);
			}
		}
	}

	// get all edges for this y
	for (a=0; a<ANTI_RES;a++)
	{
		Start[a] = 0;
		index[a] = 0;
	}
	iCount = lpShapeList->nPoints;
	lpPoint = lpFirstPoint = lpShapeList->lpPoints;
	lpLastPoint = lpFirstPoint + (iCount - 1);
	// loop on the points in this shape, and set the edges in the buffer
	while (--iCount >= 0)
	{
		// get next point in vector list
		if ( iCount )
			lpNextPoint = lpPoint+1;
		else
			lpNextPoint = lpFirstPoint;

		// get y-extents of this vector
		GetMinMax(lpPoint->y, lpNextPoint->y, miny, maxy);

		// if outside our y, or horizontal
		if ( y < miny || y >= maxy || miny == maxy)
		{
			++lpPoint;
			continue;
		}

		// find intersection of this vector and horizontal at y
		if ( !(lDx = lpNextPoint->x - lpPoint->x) )
		{
			for (a=0; a<ANTI_RES;a++)
				xx[a] = lpPoint->x; // vertical
		}
		else
		{ // calc intersection points
			lDy = lpNextPoint->y - lpPoint->y;
			dy = y+HALF_ANTI_SPACE;
			for (a=0; a<ANTI_RES;a++)
			{
				dDy1 = (dy - lpPoint->y)*lDx;
				xx[a] = (dDy1 / lDy) + lpPoint->x;
				dy += ANTI_SPACE;
			}
		}

		// Bump the edge count (some of this could move out of loop
		for (a=0; a<ANTI_RES;a++)
		{
			x = xx[a];
			if ( x >= 0 && x < dx )
				lpBuffer[a][index[a]++] =  x;
			else if (x<0)
				Start[a] ^= 255;
		}
		++lpPoint;
	}
	
	// prepare lists
	for (a=0; a<ANTI_RES;a++)
	{
		// sort all x's
		dsort(lpBuffer[a], index[a]);
		lpBuffer[a][index[a]] = dx+1000;
		State[a] = Start[a];
		lpB[a] = lpBuffer[a];
		// update states for x's less than 0
		while(*lpB[a] <= 0.0)
		{
		 	Start[a] ^= 255;
			*lpB[a]++;
		}
	}
	lpA = lpAlphaLine;
	lastx = -1;
	// Do it .. walk the lists
	while(lastx < dx)
	{
		// find the next integer point to examine
		ix = 1000+dx;
		for (a=0; a<ANTI_RES;a++)  
			ix = min(ix, (int)*lpB[a]);
		// compute values for each x
		if (ix > dx)
			ix = dx;	
		lastval = 0;
		val = 0;
		for (a=0; a<ANTI_RES;a++)
		{
			lastval += State[a];
			// is there any crossing?
			if (*lpB[a] < ix+1)
			{
				// is there more than 1 crossing?
				if (lpB[a][1] >= ix+1)
				{
					// usual case of 1 crossing
					v = (int)(((double)(ix+1) - *lpB[a]++)*255);
					if (State[a])
						v = 255 - v;
					State[a] ^= 255;
					val += v;
				}
				else
				{
					// more than 1 crossing... a bit more complex
					dval = (lpB[a][1] - *lpB[a])*255;
					*lpB[a]++;
					if (State[a]) 
						dval = 255 - dval;
					State[a] ^= 255;
					while(*lpB[a] < ix+1)
					{
						if (!State[a])
						{
							d = min(lpB[a][1], (double)(ix+1));
							dval += (d - *lpB[a])*255;
						}
						*lpB[a]++;
						State[a] ^= 255;
					}
					val += (int)dval;
				}
			}
			else // no crossing
				val += State[a];
		} // for each 'channel'
		lastc = lastval / ANTI_RES; 
		c = val / ANTI_RES; 
		cross = ix-lastx-1;
		// fill in the outpout data 
		if ( iModes == SHAPE_ADD )
		{
			if (cross > 0)
			{
				if (lastc  == 255)
				{
					set(lpA, cross, 255);
					lpA += cross;
				}
				else if (lastc)
					while(cross--)
						*lpA++ |= lastc;
				else
					lpA += cross;
			}
			if (c)
				*lpA |= c;
		}
		else if ( iModes == SHAPE_SUBTRACT )
		{
			if (cross > 0)
			{
				lastc = ~lastc;
				if (!lastc)
				{
					clr(lpA, cross);
					lpA += cross;
				}
				else if (lastc != 255)
					while(cross--)
						*lpA++ &= lastc;
				else
					lpA += cross;
			}
			if (c)
				*lpA &= ~c; // beware the squiggle in XyWrite
		}
		else // if ( iModes == SHAPE_REVERSE )
		{
			if (cross > 0)
			{
				if (lastc)
					while(cross--)
						*lpA++ ^= lastc;
				else
					lpA += cross;
			}
			if (c)
				*lpA ^= c;
		}
		lpA++;
		lastx = ix;
	}
	
	if (lpBuffer[0] != (double *)lpInBuffer)
		for (a=0; a<ANTI_RES;a++)
			FreeUp(lpBuffer[a]);

	return( YES);
}

static long accuratelsqrt(long val)
{
	double fval;
	
	fval = (double)val;
	return((long)sqrt(fval));
}

static LFIXED accuratefsqrt(LFIXED val)
{
	double fval;
	
	fval = DOUBLE(val);
	return(val = DBL2FIX(sqrt(fval)));
}


// sorts list of doubles into ascending order
// uses heapsort from Numerical recipies in C
/***********************************************************************/
LOCAL void dsort(double FAR * lpList, int num)
/***********************************************************************/
{
	int l,j,ir,i;
	double d;
	
	if (num < 2)
		return;
	l = num>>1;
	ir = num-1;
	while(1)
	{
		if(l>0)
			d = lpList[--l];
		else
		{
			d = lpList[ir];
			lpList[ir] = lpList[0];
			if(--ir == 0)
			{
				lpList[0] = d;
				return;
			}
		}
		i=l;
		j=l*2+1;
		while(j<=ir)
		{
			if (j<ir && lpList[j] < lpList[j+1])
				j++;
			if(d<lpList[j])
			{
				lpList[i] = lpList[j];
				j += (i=j)+1;
			}
			else
				j = ir+1;
		}	
		lpList[i]=d;
	}
}
