// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

#define EDIT_HANDLE_SIZE 3

static LPSHAPE lpTransformShape, lpEditShape;
static RECT rShapes, rDispShapes;
static int PointGrabbed = -1;
static TFORM ShapeDispTForm, ShapeFileTForm;
static LPPOINT lpOutputPoints;
static BOOL fShapeCircle;

/************************************************************************/
VOID ShapeGetTForm(lpTForm)
/************************************************************************/
    LPTFORM lpTForm;
    {
    TrackGetTForm(lpTForm);
    }
    
/************************************************************************/
LPSHAPE ShapeMultiSelect(lpMask, lpRect)
/************************************************************************/
LPMASK lpMask;
LPRECT lpRect;
{
LPSHAPE lpNextShape, lpHeadShape, lpShape;

lpShape = lpMask->lpHeadShape;
lpHeadShape = NULL;
while (lpShape)
	{
	if ( PtInRect(lpRect, MAKEPOINT(lpShape->rShape.left)) &&
	     PtInRect(lpRect, MAKEPOINT(lpShape->rShape.right)) )
		{
		ShapeUnlink(lpMask, lpShape);
		if (!lpHeadShape)
			lpHeadShape = lpShape;
		else	{
			lpNextShape = lpHeadShape;
			while (lpNextShape->lpNext)
				lpNextShape = lpNextShape->lpNext;
			lpNextShape->lpNext = lpShape;
			}
		lpShape = lpMask->lpHeadShape;
		}
	else	lpShape = lpShape->lpNext;
	}

return(lpHeadShape);
}


/************************************************************************/
LPSHAPE ShapeSelect(lpMask, x, y)
/************************************************************************/
LPMASK lpMask;
int x, y;
{
LPSHAPE lpNextShape, lpShape;
POINT Point;

lpNextShape = lpMask->lpHeadShape;
lpShape = NULL;
Point.x = x;
Point.y = y;
while (lpNextShape)
	{
	if (PtInRect(&lpNextShape->rShape, Point))
		{
		if (lpShape)
		    {
		    if ((lpNextShape->rShape.top > lpShape->rShape.top &&
			 lpNextShape->rShape.bottom < lpShape->rShape.bottom) ||
			(lpNextShape->rShape.left > lpShape->rShape.left &&
			 lpNextShape->rShape.right < lpShape->rShape.right))
				{
				lpShape = lpNextShape;
				}
		    else
		    if (((long)RectWidth(&lpNextShape->rShape) *
			 (long)RectHeight(&lpNextShape->rShape)) <
			((long)RectWidth(&lpShape->rShape) *
			 (long)RectHeight(&lpShape->rShape)))
				{
				lpShape = lpNextShape;
				}
		    }
		else	lpShape = lpNextShape;
		}
	lpNextShape = lpNextShape->lpNext;
	}
return(lpShape);
}


/************************************************************************/
LPSHAPE ShapeUnlink(lpMask, lpShape)
/************************************************************************/
LPMASK lpMask;
LPSHAPE lpShape;
{
LPSHAPE lpNextShape, lpPrevShape;

lpNextShape = lpMask->lpHeadShape;
lpPrevShape = NULL;
while (lpNextShape)
	{
	if (lpNextShape == lpShape)
		{
		if (lpPrevShape)
			lpPrevShape->lpNext = lpShape->lpNext;
		else	lpMask->lpHeadShape = lpShape->lpNext;
		lpShape->lpNext = NULL;
		return(lpPrevShape);
		}
	lpPrevShape = lpNextShape;
	lpNextShape = lpNextShape->lpNext;
	}

return(NULL);
}


/************************************************************************/
VOID ShapeLink(lpMask, lpShape, lpPrevShape)
/************************************************************************/
LPMASK lpMask;
LPSHAPE lpShape;
LPSHAPE lpPrevShape;
{
LPSHAPE lpNextShape;

if (lpPrevShape)
	{
	lpNextShape = lpPrevShape->lpNext;
	lpPrevShape->lpNext = lpShape;
	}
else	{
	lpNextShape = lpMask->lpHeadShape;
	lpMask->lpHeadShape = lpShape;
	}
while (lpShape->lpNext)
	lpShape = lpShape->lpNext;
lpShape->lpNext = lpNextShape;
}


/************************************************************************/
void ShapeNewShapes(lpShapes)
/************************************************************************/
LPSHAPE lpShapes;
{
LPSHAPE lpNextShape;
LPPOINT lpPoint;
int nPoints, count;
LPPOINT lpOutputPoints;

lpOutputPoints = (LPPOINT)LineBuffer[0];
lpNextShape = lpShapes;
while (lpNextShape)
	{
	lpNextShape->rShape.top = 32767;
	lpNextShape->rShape.bottom = -32767;
	lpNextShape->rShape.left = 32767;
	lpNextShape->rShape.right = -32767;
	nPoints = ShapeConvertPoints(lpNextShape, lpOutputPoints);
	lpPoint = lpOutputPoints;
	count = nPoints;
	while (--count >= 0)
		{
		if ( lpPoint->x < lpNextShape->rShape.left )
			lpNextShape->rShape.left = lpPoint->x;
		if ( lpPoint->x > lpNextShape->rShape.right )
			lpNextShape->rShape.right = lpPoint->x;
		if ( lpPoint->y < lpNextShape->rShape.top )
			lpNextShape->rShape.top = lpPoint->y;
		if ( lpPoint->y > lpNextShape->rShape.bottom)
			lpNextShape->rShape.bottom = lpPoint->y;
		++lpPoint;
		}
	lpNextShape = lpNextShape->lpNext;
	}
}


/************************************************************************/
VOID ShapeTransformInit(lpShape, lpTForm)
/************************************************************************/
LPSHAPE lpShape;
LPTFORM lpTForm;
{
LPSHAPE lpNextShape;
TFORM EditTForm;

lpTransformShape = lpShape;
lpNextShape = lpTransformShape;
rShapes.top = 32767;
rShapes.bottom = -32767;
rShapes.left = 32767;
rShapes.right = -32767;
while (lpNextShape)
	{
	AstralUnionRect(&rShapes, &lpNextShape->rShape, &rShapes);
	lpNextShape = lpNextShape->lpNext;
	}

if ( lpTForm )
	EditTForm = *lpTForm;
else	{
	TInit(&EditTForm);
	}
TrackInit(ShapeTransformDraw, &EditTForm, &rShapes);
}


/************************************************************************/
VOID ShapeTransformDone()
/************************************************************************/
{
RECT rDummy;
TFORM EditTForm, ShapeTForm;
LPSHAPE lpNextShape;

TrackDone(&rDummy, TRUE);
ShapeGetTForm(&EditTForm);
lpNextShape = lpTransformShape;
while (lpNextShape)
	{
	TMultiply(&lpNextShape->tform, &EditTForm, &ShapeTForm);
	lpNextShape->tform = ShapeTForm;
	lpNextShape = lpNextShape->lpNext;
	}
ShapeNewShapes(lpTransformShape);
}


/************************************************************************/
VOID ShapeTransformMouseDown(x, y)
/************************************************************************/
int x, y;
{
TrackGrab(x, y);
}

/************************************************************************/
VOID ShapeTransformMouseUp(x, y)
/************************************************************************/
int x, y;
{
TrackUp(x, y);
}
   
/************************************************************************/
VOID ShapeTransformMouseMove(x, y)
/************************************************************************/
int x, y;
{
TrackMove(x, y);
}


/************************************************************************/
VOID ShapeTransformDraw(hDC, lpRect, on, num_handles)
/************************************************************************/
HDC hDC;
LPRECT lpRect;
BOOL on;
int num_handles;
{
TFORM DispTForm, EditTForm, ShapeTForm;
LPSHAPE lpNextShape;
LPPOINT lpPoint, lpNextPoint;
int count, nPoints;
LPPOINT lpOutputPoints;

if (!hDC)
	hDC = Window.hDC;

lpOutputPoints = (LPPOINT)LineBuffer[0];
TrackGetTForm(&EditTForm);
File2DispTForm(&EditTForm);
lpNextShape = lpTransformShape;
while (lpNextShape)
	{
	TMultiply(&lpNextShape->tform, &EditTForm, &DispTForm);
	ShapeTForm = lpNextShape->tform;
	lpNextShape->tform = DispTForm;
	nPoints = ShapeConvertPoints(lpNextShape, lpOutputPoints);
	lpNextShape->tform = ShapeTForm;
	count = nPoints;
	lpPoint = lpOutputPoints;
	while (--count >= 0)
		{
		if (count)
			lpNextPoint = lpPoint+1;
		else
			lpNextPoint = lpOutputPoints;
		Dline(hDC, lpPoint->x, lpPoint->y, lpNextPoint->x, lpNextPoint->y, ON);
		++lpPoint;
		}
	lpNextShape = lpNextShape->lpNext;
	}

TrackDispReset();
if (num_handles)
	TrackDrawHandles(hDC, lpRect, on, -1);
}


/************************************************************************/
VOID ShapeEditInit(lpShape)
/************************************************************************/
LPSHAPE lpShape;
{
LPPOINT lpPoint;
int count;

count = lpShape->nPoints;
lpPoint = lpShape->lpPoints;
fShapeCircle = NO;
while (--count >= 0)
	{
	if (lpPoint->x == BEZIER_MARKER)
		 {
		 fShapeCircle = YES;
		 break;
		 }
	}

PointGrabbed = -1;
lpEditShape = lpShape;
ShapeDispTForm = lpEditShape->tform;
File2DispTForm(&ShapeDispTForm);
ShapeFileTForm = ShapeDispTForm;
TInvert(&ShapeFileTForm);
}


/************************************************************************/
VOID ShapeEditDone()
/************************************************************************/
{
ShapeEditDraw(0, OFF);
ShapeNewShapes(lpEditShape);
}


/************************************************************************/
VOID ShapeEditMouseDown(x, y)
/************************************************************************/
int x, y;
{
int count, i;
LPPOINT lpPoint;
RECT rHandle;
int x1, y1;
POINT Point;

count = lpEditShape->nPoints;
lpPoint = lpEditShape->lpPoints;
PointGrabbed = -1;
Point.x = x;
Point.y = y;
for (i = 0; i < count; ++i)
	{
	if (lpPoint->x != BEZIER_MARKER)
		{
		Transformer(&ShapeDispTForm, lpPoint, &x1, &y1);
		rHandle.left = x1 - EDIT_HANDLE_SIZE;
		rHandle.right = x1 + EDIT_HANDLE_SIZE;
		rHandle.top = y1 - EDIT_HANDLE_SIZE;
		rHandle.bottom = y1 + EDIT_HANDLE_SIZE;
		if (PtInRect(&rHandle, Point))
			{
			PointGrabbed = i;
			break;
			}
		}
	++lpPoint;
	}
}

/************************************************************************/
VOID ShapeEditMouseUp(x, y)
/************************************************************************/
int x, y;
{
}
   
/************************************************************************/
VOID ShapeEditMouseMove(x, y)
/************************************************************************/
int x, y;
{
LPPOINT lpPoint;
POINT newPoint;

if (PointGrabbed >= 0)
	{
	lpPoint = lpEditShape->lpPoints;
	ShapeDrawPoint(OFF);
	newPoint.x = x;
	newPoint.y = y;
	Transformer(&ShapeFileTForm, &newPoint,
		&lpPoint[PointGrabbed].x, &lpPoint[PointGrabbed].y);
	ShapeDrawPoint(ON);
	}
}


/************************************************************************/
VOID ShapeEditDraw(hDC, on)
/************************************************************************/
HDC hDC;
BOOL on;
{
LPPOINT lpPoint, lpNextPoint;
int x1, y1, count, nPoints;
RECT rHandle;
TFORM ShapeTForm;
LPPOINT lpOutputPoints;

if (!hDC)
	hDC = Window.hDC;

lpOutputPoints = (LPPOINT)LineBuffer[0];
ShapeTForm = lpEditShape->tform;
lpEditShape->tform = ShapeDispTForm;
nPoints = ShapeConvertPoints(lpEditShape, lpOutputPoints);
lpEditShape->tform = ShapeTForm;
count = nPoints;
lpPoint = lpOutputPoints;
while (--count >= 0)
	{
	if (count)
		lpNextPoint = lpPoint+1;
	else
		lpNextPoint = lpOutputPoints;
	Dline(hDC, lpPoint->x, lpPoint->y, lpNextPoint->x, lpNextPoint->y, ON);
	++lpPoint;
	}

count = lpEditShape->nPoints;
lpPoint = lpEditShape->lpPoints;
while (--count >= 0)
	{
	if (lpPoint->x != BEZIER_MARKER)
		{
		Transformer(&ShapeDispTForm, lpPoint, &x1, &y1);
		rHandle.left = x1 - EDIT_HANDLE_SIZE;
		rHandle.right = x1 + EDIT_HANDLE_SIZE + 1;
		rHandle.top = y1 - EDIT_HANDLE_SIZE;
		rHandle.bottom = y1 + EDIT_HANDLE_SIZE + 1;
		Drect(hDC, &rHandle, ON );
		}
	++lpPoint;
	}
}


/************************************************************************/
ShapeDrawPoint(on)
/************************************************************************/
BOOL on;
{
int prevPoint, nextPoint, x1, y1, x2, y2;
RECT rHandle;
LPPOINT lpPoint;

if (fShapeCircle)
	ShapeEditDraw(0,on);
else	{
	lpPoint = lpEditShape->lpPoints;
	if (PointGrabbed == 0)
		prevPoint = lpEditShape->nPoints - 1;
	else	prevPoint = PointGrabbed - 1;
	if (PointGrabbed == (lpEditShape->nPoints - 1))
		nextPoint = 0;
	else	nextPoint = PointGrabbed + 1;

	/* undraw the handle */
	Transformer(&ShapeDispTForm, &lpPoint[PointGrabbed], &x1, &y1);
	rHandle.left = x1 - EDIT_HANDLE_SIZE;
	rHandle.right = x1 + EDIT_HANDLE_SIZE + 1;
	rHandle.top = y1 - EDIT_HANDLE_SIZE;
	rHandle.bottom = y1 + EDIT_HANDLE_SIZE + 1;
	Drect(0,  &rHandle, ON );

	/* undraw the two vectors connecting to PointGrabbed */
	if (prevPoint > PointGrabbed)
		{
		Transformer(&ShapeDispTForm, &lpPoint[nextPoint], &x2, &y2);
		Dline(0, x1, y1, x2, y2, NO);
		Transformer(&ShapeDispTForm, &lpPoint[prevPoint], &x2, &y2);
		Dline(0, x2, y2, x1, y1, NO);
		}
	else	{
		Transformer(&ShapeDispTForm, &lpPoint[prevPoint], &x2, &y2);
		Dline(0,x2, y2, x1, y1, NO);
		Transformer(&ShapeDispTForm, &lpPoint[nextPoint], &x2, &y2);
		Dline(0,x1, y1, x2, y2, NO);
		}
	}
}
