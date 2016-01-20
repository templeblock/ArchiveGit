/*=======================================================================*\

	EDITPOLY.C - Bezier and line segments with selection flags

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "pp.h"
#include "editpoly.h"

#define EDITPOLY_MAX_ALLOC	2048

//=======================================================================
//	CEditPoly::CEditPoly()
//		Constructor.
//=======================================================================
CEditPoly::CEditPoly() : CPoly()
//=======================================================================
{
	lpFlags = NULL;
	FlagsAllocated = 0;
	FlagsUsed = 0;
}

//=======================================================================
//	CEditPoly::~CEditPoly()
//		Destructor.
//=======================================================================
CEditPoly::~CEditPoly()
//=======================================================================
{
	if (lpFlags)
		FreeUp(lpFlags);
}

//=======================================================================
BOOL CEditPoly::IsA(LPTR lpClass)
//=======================================================================
{	
	return(lstrcmp((LPSTR)lpClass, "CEditPoly") ? TRUE: CPoly::IsA(lpClass));
}

//=======================================================================
//	BOOL CEditPoly::AddPoint(LPPOINT lpPoint)
//		Adds the given point to the end of the polygon.
//
//		Returns FALSE if the polygon was unable to add the new point.
//=======================================================================
BOOL CEditPoly::AddPoint(LPPOINT lpPoint)
//=======================================================================
{
	int current;
	
	if (!CPoly::AddPoint(lpPoint))
		return(FALSE);
	if (!make_room(Num))
	{
		// error - clean up
		current = CurPoint;
		CPoly::LastPoint();
		CPoly::DeletePoint();
		CPoly::GetPoint(current);
		return(FALSE);
	}
	lpFlags[FlagsUsed] = 0;
	FlagsUsed++;
	return(TRUE);
}


//=======================================================================
//	BOOL DeletePoint()
//		Deletes the current point. 
//		The new current point is the next point (if there is one).
//		If the point is in any way associated with a bezier it will not
//			be deleted.
//=======================================================================
BOOL CEditPoly::DeletePoint()
//=======================================================================
{
	int index;
	
	// delete point/points
	index = CurPoint;
	if (!CPoly::DeletePoint())
		return(FALSE);
	// fix up flags
	MoveMemory((LPTR)&lpFlags[index], 
	  			(LPTR)&lpFlags[index+1], 
	  			sizeof(*lpFlags)*(FlagsUsed-(index+1)));
	FlagsUsed--;
	return(TRUE);
}

//=======================================================================
//	BOOL CEditPoly::AddBezierLine(LPPOINT lpHandle1, LPPOINT lpHandle2, LPPOINT lpEnd)
//		Adds the given BEZIER to the end of the polygon.
//		The first point in the new bezier is the end of the old polygon.
//		If there are no points in the shape the result is undefined
//
//		Returns FALSE if the polygon was unable to add the new points.
//=======================================================================
BOOL CEditPoly::AddBezierLine(LPPOINT lpHandle1, LPPOINT lpHandle2, LPPOINT lpEnd)
//=======================================================================
{
	int current;
	
	if (!CPoly::AddBezierLine(lpHandle1, lpHandle2, lpEnd))
		return(FALSE);
	if (!make_room(Num))
	{
		// error - clean up
		current = CurPoint;
		CPoly::GetPoint(Num-BEZIER_IN);
		if (!CPoly::BezierToLine())
			return(FALSE);	//*** ERROR - should never happen
		CPoly::NextPoint();
		CPoly::DeletePoint();
		CPoly::GetPoint(current);
		return(FALSE);
	}
	lpFlags[FlagsUsed] = 0;
	FlagsUsed++;
	lpFlags[FlagsUsed] = 0;
	FlagsUsed++;
	lpFlags[FlagsUsed] = 0;
	FlagsUsed++;
	return(TRUE);
}

//=======================================================================
//=======================================================================
BOOL CEditPoly::AddBezierLine2(LPPOINT lpStart, LPPOINT lpHandle1, LPPOINT lpHandle2)
//=======================================================================
{
	int current;
	
	if (!CPoly::AddBezierLine2(lpStart, lpHandle1, lpHandle2))
		return(FALSE);
	if (!make_room(Num))
	{
		// error - clean up
		current = CurPoint;
		CPoly::GetPoint(Num-BEZIER_IN+1);
		if (!CPoly::BezierToLine())
			return(FALSE);	//*** ERROR - should never happen
		CPoly::NextPoint();
		CPoly::DeletePoint();
		CPoly::GetPoint(current);
		return(FALSE);
	}
	lpFlags[FlagsUsed] = 0;
	FlagsUsed++;
	lpFlags[FlagsUsed] = 0;
	FlagsUsed++;
	lpFlags[FlagsUsed] = 0;
	FlagsUsed++;
	return(TRUE);
}

//=======================================================================
//	BOOL CEditPoly::InsertPoint(LPPOINT lpPoint)
//		Inserts the given point after the current point.  If the current 
//			point was set to -1 then it will go before the first point.
//		If the current point is a bezier start or first handle the point 
//			will be inserted as the new bezier end point.
//  	The new current point is the inserted point.
//
//		Returns FALSE for failure.
//=======================================================================
BOOL CEditPoly::InsertPoint(LPPOINT lpPoint)
//=======================================================================
{
	POINT p1, p2;
	
	if (!CPoly::InsertPoint(lpPoint))
		return(FALSE);
	// make room
	if (!make_room(Num))
	{
		// error - clean up
		if (!IsCurve())
			CPoly::DeletePoint();
		else
		{
			// must be the endpoint (but NOT the start of another)
			// convert to line
			if (!CPoly::PreviousPoint(&p1)) return(FALSE);
			if (!CPoly::PreviousPoint(&p2)) return(FALSE);
			if (!CPoly::BezierToLine()) return(FALSE);
			// delete added point
			CPoly::NextPoint();
			CPoly::DeletePoint();
			// restore bezier
			CPoly::PreviousPoint();
			CPoly::LineToBezier(&p1, &p2);
		}
		return(FALSE);
	}
	// fix flags
	MoveMemory((LPTR)&lpFlags[CurPoint+1], 
	  			(LPTR)&lpFlags[CurPoint], 
				sizeof(*lpFlags)*(FlagsUsed-CurPoint));
	// clear flag
	lpFlags[CurPoint] = 0;
	FlagsUsed++;
	return(TRUE);
}


//=======================================================================
//	BOOL CEditPoly::LineToBezier(LPPOINT lpHandle1, LPPOINT lpHandle2)
//		Converts the current point and its forward neighbor to a bezier.
//		If the current point is already the begining of a bezier or a 
//			bezier handle then nothing occurs.
//		If the current point is the last point in an open polygon then
//			the bezier will not be added.	
//  	The current point is left were it was (the first point of bezier).
//	
//		Returns 1 if sucessful.
//		Returns	0 if the polygon was unable to add the new points.
//		Returns	-1 if no new points were added (see above).
//=======================================================================
int CEditPoly::LineToBezier(LPPOINT lpHandle1, LPPOINT lpHandle2)
//=======================================================================
{
	int ret;
	
	// add handles
	ret = CPoly::LineToBezier(lpHandle1, lpHandle2);
	if (ret < 1)
		return(ret);
	if (!make_room(Num))
	{
		// error - clean up
		CPoly::BezierToLine();
		return(0);
	}
	// fix flags
	if (CurPoint + 1 < FlagsUsed)
	{
		MoveMemory((LPTR)&lpFlags[CurPoint+3], 
		  			(LPTR)&lpFlags[CurPoint+1], 
	  				sizeof(*lpFlags)*(FlagsUsed-CurPoint-1));
	}
	// clear flags
	lpFlags[CurPoint+1] = 0;
	FlagsUsed++;
	lpFlags[CurPoint+2] = 0;
	FlagsUsed++;
	return(1);
}

//=======================================================================
//	void CEditPoly::BezierToLine()
//		Converts the current point and its forward neighbor to a line.
//		If the segment is not a bezier then this will have no effect.
//		If its a bezier handle then the bezier containing it becomes a line.
//  	The current new current point is the line start.
//
//		Returns TRUE if handles are deleted.
//=======================================================================
BOOL CEditPoly::BezierToLine()
//=======================================================================
{
	if (!CPoly::BezierToLine())
		return(FALSE);
	// the two points following the current point were deleted
	MoveMemory((LPTR)&lpFlags[CurPoint+1], 
		  			(LPTR)&lpFlags[CurPoint+BEZIER_IN-1], 
					sizeof(*lpFlags)*(FlagsUsed-(CurPoint+BEZIER_IN-1)));
	FlagsUsed -= 2;
	return(TRUE);
}

//=======================================================================
//	void CEditPoly::SetFlags(WORD wFlags)
//		Sets the flags for the current point to wFlags.
//=======================================================================
void CEditPoly::SetFlags(WORD wFlags)
//=======================================================================
{
	if (WITHIN(CurPoint,0,FlagsUsed-1))
		lpFlags[CurPoint] = wFlags;
}

//=======================================================================
//	void CEditPoly::SetFlags(WORD wFlags, int index)
//		Sets the flags for the given point to wFlags.
//=======================================================================
void CEditPoly::SetFlags(WORD wFlags, int index)
//=======================================================================
{
	if (index >= 0)
	{
		if (index >= FlagsUsed)
			index = index%FlagsUsed;	
		lpFlags[index] = wFlags;
	}
}

//=======================================================================
//	WORD CEditPoly::GetFlags()
//		Returns the flags for the current point.
//=======================================================================
WORD CEditPoly::GetFlags()
//=======================================================================
{
	if (!WITHIN(CurPoint,0,FlagsUsed-1))
		return(0);
	return(lpFlags[CurPoint]);
}

//=======================================================================
//	WORD CEditPoly::GetFlags(int index)
//		Returns the flags for the given point.
//=======================================================================
WORD CEditPoly::GetFlags(int index)
//=======================================================================
{
	if (index >= 0)
	{
		if (index >= FlagsUsed)
			index = index%FlagsUsed;	
		return(lpFlags[index]);
	}
	return(0);
}

//=======================================================================
//	BOOL CEditPoly::make_room(int size)
//	
//	 	Ensures that the lpFlags array is at least size elements.
//		Returns FALSE for failure.
//=======================================================================
BOOL CEditPoly::make_room(int size)
//=======================================================================
{
	int alloc;
	LPWORD lpNewMem = NULL;
	
	alloc = size - FlagsAllocated;
	if (alloc <= 0)
		return(TRUE);
	alloc = Max(alloc, Min(FlagsAllocated/2, EDITPOLY_MAX_ALLOC));
	if (lpFlags)
		lpNewMem = (LPWORD)AllocExtend((LPTR)lpFlags, sizeof(WORD)*(alloc+FlagsAllocated));
	if (!lpNewMem)
	{
		// get what we need
		lpNewMem = (LPWORD)Alloc(sizeof(WORD)*(alloc+FlagsAllocated));
		if (!lpNewMem)
			return(FALSE);
		if (lpFlags)
		{
			copy((LPTR)lpFlags, (LPTR)lpNewMem, sizeof(WORD)FlagsUsed);
			FreeUp(lpFlags);
		}
	}
	lpFlags = lpNewMem;
	FlagsAllocated += alloc;
	return(TRUE);
}


