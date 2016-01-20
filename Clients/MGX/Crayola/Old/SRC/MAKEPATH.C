//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

#define EXTRA_X 10
#define EXTRA_Y 10
#define DO_DISPLAY FALSE

// Static prototypes
static BOOL AddShape(LPPOINT lpFirstPoint, int nPoints);
static BOOL InMaskRegion(long v);
static BOOL GetMaskPixel(int x, int y, LPLONG lpPixel);
static BOOL PutMaskPixel(int x, int y, long Pixel);
static LPSHAPE VectorizeFrame(LPPOINT lpFirstPoint, LPTR lpFirstCode,int max_points, LPINT InOut);

static LPSHAPE lpHeadShape, lpTailShape;
static numShapes, MaxNoCrashShapes;
static RECT rArea;
static height, width;
static LPFRAME lpBitsFrame;  // always an 8-bit frame

//************************************************************************/
//	Creates shapes from the mask inside the rect.
//	If lpRect == NULL then the entire mask is used.
//	cutoff is value used to determine edges in mask.
//	lpFrame MUST be an 8-bit frame.	
//	on return InOut will be 0 bits are cleared outside shapes 1 if set.
//************************************************************************/
LPSHAPE GetFramePath(LPRECT lpRect, LPFRAME lpFrame, BYTE cutoff, LPINT InOut)
//************************************************************************/
{
	int max_points, y, x, frameWidth, frameHeight;
	int left,right;
	LPPOINT lpFirstPoint;
	LPTR lpFirstCode, lpSrc, lpDst;
	LPSHAPE lpShape;
	TFORM tform;
	BOOL bInvert;
	BYTE in, out;
	
	lpBitsFrame = NULL;
	lpFirstPoint = NULL;
	lpShape = NULL;
	// setup buffer for points
	if (FrameDepth(lpFrame) != 1)
		goto Exit;
	max_points = 13000;
	GetDefInt( MaxNoCrashShapes, 999);
	numShapes = 0;
	while (!lpFirstPoint)
		if ( !(lpFirstPoint = (LPPOINT)Alloc((long)max_points * 
			(long)(sizeof(POINT)+sizeof(BYTE)))) )
		{
			max_points /= 2;
			if (max_points < 3)
			    break;
		}
	if (max_points < 3)
	{
		Message( IDS_EMEMALLOC );
		goto Exit;
	}
	lpFirstCode = (LPTR)(lpFirstPoint + max_points);

	frameWidth = FrameXSize(lpFrame);
	frameHeight = FrameYSize(lpFrame);
	
	// set up frame rect
	if (lpRect)
	{
		rArea = *lpRect;
		InflateRect(&rArea, 1, 1);
		lpRect->left = Max(rArea.left, 0);
		lpRect->top = Max(rArea.top, 0);
		lpRect->right = Min(rArea.right, frameWidth-1);
		lpRect->bottom = Min(rArea.bottom, frameHeight-1);
	}
	else
	{
		rArea.left = rArea.top = -1;
		rArea.right = frameWidth;
		rArea.bottom = frameHeight;
	}
//	if (!rArea.left)
//		rArea.left--;
//	if (!rArea.top)
//		rArea.top--;
//	if (rArea.right == frameWidth - 1 )
//		rArea.right++;
//	if (rArea.bottom == frameHeight - 1 )
//		rArea.bottom++;
	width = RectWidth(&rArea);
	height = RectHeight(&rArea);
	// setup working frame
	lpBitsFrame = FrameOpen(FDT_GRAYSCALE,width,height, 1);
	if (!lpBitsFrame)
		goto Exit;
	left = Max(rArea.left,0);
	right = Min(rArea.right,frameWidth-1);
	// invert frame if needed to ensure 0-1 transition for tracer
	if (rArea.left < 0 || rArea.top < 0)
		bInvert = FALSE;
	else
	{
		lpSrc = FramePointer(lpFrame, rArea.left, rArea.top, NO);
		bInvert =  (*lpSrc < cutoff) ? 0:1;
	}
	in = bInvert ? 0:1;
	out = bInvert ? 1:0;
	for(y=rArea.top; y<=rArea.bottom; y++)
	{
		if (!WITHIN(y, 0, frameHeight-1))
		{
			lpDst = FramePointer(lpBitsFrame, 0, y-rArea.top, YES);
			if (!lpDst)	goto Exit;  
			set(lpDst, width, out);
			continue;
		}
		lpSrc = FramePointer(lpFrame, left, y, NO);
		lpDst = FramePointer(lpBitsFrame, 0, y-rArea.top, YES);
		if (!lpSrc || !lpDst)
			goto Exit;  
		if (left > rArea.left)
			*(lpDst++) = out;
		for (x=0;x<(right-left+1);x++) 
			*(lpDst++) = (*(lpSrc++) < cutoff) ? out:in;
		if (right < rArea.right)
			*lpDst = out;
	}	
	// do the trace
	lpShape = VectorizeFrame(lpFirstPoint, lpFirstCode, max_points, InOut);
	if (bInvert)
		*InOut = !(*InOut);
Exit:
	if (lpFirstPoint)
		FreeUp((LPTR)lpFirstPoint);
	if (lpBitsFrame)						   
		FrameClose(lpBitsFrame);
	// Move lpShape to rArea.left,rArea.top
	TInit(&tform);
	TMove(&tform, rArea.left, rArea.top);
	ShapeApplyTransform(lpShape,&tform);
	return(lpShape);
}

//************************************************************************
//	Applys the given Transform to the shapes in the list
//************************************************************************
void ShapeApplyTransform(LPSHAPE lpHeadShape, LPTFORM lpTForm)
//*********************	***************************************************
{
	int count;
	LPPOINT lpPoint;
	
	// check for identity transform
	if (ISNULLTFORM(lpTForm))
			return;
	while (lpHeadShape)
	{
		count = lpHeadShape->nPoints;
		lpPoint = lpHeadShape->lpPoints;
		while(--count>=0)
		{
				
			if (!IS_BEZIER(lpPoint))
				Transformer(lpTForm, lpPoint, (LPINT)&lpPoint->x, (LPINT)&lpPoint->y);
			lpPoint++;
		}
		lpHeadShape = lpHeadShape->lpNext;
	}
}

/************************************************************************/
//	Makes shapes from a lpBitsFrame which must be 0's & 1's.
//	Sets InOut to be 0 if bits are clear outside of shapes & 1 if they are set.
/************************************************************************/
static LPSHAPE VectorizeFrame(LPPOINT lpFirstPoint, LPTR lpFirstCode,int max_points, LPINT InOut)
/************************************************************************/
{
	LPTR lpPixel;
	int px, py,num_points, status;
	BYTE pix1, pix2;
	RECT rShapes;
	int total, part;
	int inout;

	ProgressBegin(1,0);	
	status = 0;
	lpHeadShape = lpTailShape = NULL;
	rShapes.left = rShapes.top = 0;
	rShapes.right = width-1;
	rShapes.bottom = height-1;
	total = height;
	part = 0;
	inout = -1;
	for (py = 0; py < height; ++py)						   
	{
		if(!(lpPixel = FramePointer(lpBitsFrame, 0,Max(py-1,0), NO)))
			goto ExitFalse;
		pix1 = *lpPixel;
		AstralClockCursor( part++, total, NO);
		for (px = 0; px < width; ++px)
	    {
			if (!(lpPixel = FramePointer(lpBitsFrame, px,py,NO)))
				goto ExitFalse;
	        pix2 = *lpPixel;
			if (inout < 0)
				inout = (pix1==0&&pix2==1) ? 0:((pix1==1&&pix2==0) ? 1:-1);
	        if (pix1 == 0 && pix2 == 1) // always start on left side of contour
	        {
	            if ( !(status = Tracer(px,py, rShapes, GetMaskPixel, PutMaskPixel,
	                            InMaskRegion, lpFirstPoint, lpFirstCode, max_points, 
	                            DO_DISPLAY, &num_points, AddShape)) )
				{
//		        	if ( !(status = TraceHoles(rShapes, GetMaskPixel, PutMaskPixel,
//					 	InMaskRegion, lpFirstPoint, lpFirstCode, max_points,
//						DO_DISPLAY, &num_points, AddShape)) )
//							break;
				}
	    		if ( status == 2)
	        		break;
	       	}
	       	pix1 = pix2;
	    }
	    if ( status )
	        break;
	}
	*InOut = inout;
	ProgressEnd();
	return(lpHeadShape);

ExitFalse:
	if (lpHeadShape)
		FreeUpShapes(lpHeadShape);
	ProgressEnd();
	return(NULL);	
}

/************************************************************************/
static BOOL AddShape(LPPOINT lpFirstPoint, int nPoints)
/************************************************************************/
{
	LPPOINT lpInPoint, lpOutPoint;
//	LPSHAPE lpNextShape;
	int iCount;
	TFORM tform;
	LPSHAPE  lpShape = NULL;
	
	lpInPoint = lpFirstPoint;
	lpOutPoint = (LPPOINT)LineBuffer[0];
	
	iCount = nPoints;
	while (--iCount >= 0)
		{
		*lpOutPoint++ = *lpInPoint++;
		}
	iCount = nPoints;
	MaskSqueeze((LPPOINT)LineBuffer[0], &iCount, 1, NO);
	TInit(&tform);
	TMove(&tform, rArea.left, rArea.top);
	if (AvailableMemory() > 131073)
		lpShape = MaskCreateShapes((LPPOINT)LineBuffer[0], &iCount, 1, SHAPE_REVERSE, YES, &tform);
	if (!lpShape)
		{
		Message( IDS_EMEMALLOC );
		FreeUpShapes(lpHeadShape);
		lpHeadShape = lpTailShape = NULL;
		return(2);
		}
	numShapes++;
	if (numShapes>MaxNoCrashShapes)
		{
		if (AstralOKCancel(IDS_ETOOCOMPLEX) != IDOK)
			{
			FreeUpShapes(lpHeadShape);
			lpHeadShape = lpTailShape = NULL;
			}
		return(2);
		}
	TInit(&lpShape->tform);
	if (lpTailShape)
	{
		lpTailShape->lpNext = lpShape;
		lpTailShape = lpShape;
	}
	else
	 	lpHeadShape = lpTailShape = lpShape;
	return(0);
}

/************************************************************************/
static BOOL InMaskRegion(long v)
/************************************************************************/
{
	return(v != 0);
}


/************************************************************************/
static BOOL PutMaskPixel(int x, int y, long Pixel)
/************************************************************************/
{
	LPTR lpData;
	
	if ( x >= 0 && x < width && y >= 0 && y < height )
		if (lpData = FramePointer(lpBitsFrame, x,y,YES))
		{
			*lpData = (BYTE)Pixel;
			return(TRUE);
		}
	return(FALSE);
}


/************************************************************************/
static BOOL GetMaskPixel(int x, int y, LPLONG lpPixel)
/************************************************************************/
{
	LPTR lpData;
	
	if ( x >= 0 && x < width && y >= 0 && y < height )
		if (lpData = FramePointer(lpBitsFrame, x,y,NO))
		{
			*lpPixel = *lpData;
			return(TRUE);
		}
	*lpPixel = 0;
	return(FALSE);
}

