/*=======================================================================*\

	POLY.C - Bezier and line segments

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "pp.h"
#include "poly.h"
#include "pointutl.h"
#include "maskutil.h"

#define POLY_MAX_ALLOC	2048

LOCAL poly_get_bounds(LPCPoly lpPoly, LPRECT lpRect);
LOCAL BOOL point_tform(LPPOINT lpPoint, WORD wFlags, LPTFORM lpTForm);
LOCAL  BOOL check_tolerance(LPPOINT lpFirst, LPPOINT lpLast, int num, int Tolerance);

//=======================================================================
//		LOCAL INLINES
//=======================================================================

//=======================================================================
inline void CPoly::fix_cur_point()
//=======================================================================
{
	if (CurPointPos >= EntriesUsed || CurPoint >= Num)
	{
		CurPointPos = EntriesUsed-1;
		CurPoint = Num-1;
	}
	if (CurPointPos>=0)
	{
		if (lpPoints[CurPointPos].x == BEZIER_MARKER)
			CurPointPos++;
	}
}

//=======================================================================
//	   MEMBER FUNCTIONS
//=======================================================================

//=======================================================================
//	CPoly()
//		Constructor.
//=======================================================================
CPoly::CPoly()
//=======================================================================
{
	Num = 0;
	lpPoints = NULL;
//	TInit(&DisplayTForm);
	bClosed = NO;
	CurPoint = -1;
	CurPointPos = -1;
//	CurrentPointType = 0;
//	BezierOffset = 0;
	EntriesUsed = 0;
	EntriesAllocated = 0;
	
}

//=======================================================================
//	CPoly()
//		Destructor.
//=======================================================================
CPoly::~CPoly()
//=======================================================================
{
	if (lpPoints)
		FreeUp(lpPoints);
}

//=======================================================================
BOOL CPoly::IsA(LPTR lpClass)
//=======================================================================
{	
	return(lstrcmp((LPSTR)lpClass, "CPoly"));
}


//=======================================================================
//	int NumPoints()
//		Returns the number of control points + handles in the polygon.
//=======================================================================
int CPoly::NumPoints()
//=======================================================================
{
	return(Num);
}


//=======================================================================
//	int CurrentIndex()
//		Returns the number of the current point.
//		Returns -1 for "before" the first point.
//=======================================================================
int CPoly::CurrentIndex()
//=======================================================================
{
 	return(CurPoint);
}


//=======================================================================
//	LPPOINT GetPoint(int num, LPPOINT lpPoint)
//		num: Index of the point (includes handles). -1 for before first.
//		lpPoint: The point structure to fill.- Can be NULL.
//
//		Sets the current point to be num.
//		Returns the lpPoint passed in or NULL for failure.
//		Failure can result if there are no points or the index is beyond 
//			the last point in a NON-Closed polygon.
//=======================================================================
LPPOINT CPoly::GetPoint(int num, LPPOINT lpPoint)
//=======================================================================
{	
	int pos, index, dcur; 
	BOOL bForward;

	// go to begining?
	if (num < 0)
	{
		CurPoint = -1;
		CurPointPos = -1;
		return(lpPoint);
	}
	if (num == CurPoint)
		goto Found;
	// out of bounds
	if(!Num)
		return(NULL);
	// make it in range
	if (num >= Num) 
	{
		if(!bClosed)
			return(NULL);
		num = num%Num;
	}
	if (num == CurPoint)
		goto Found;
	// determine best direction to look
	dcur = num-CurPoint;
	if (dcur < 0) dcur = -dcur;
	if (CurPoint < 0)
		dcur = INT_MAX;
	if ((dcur > num) && (dcur > Num-num))
	{
		// look forward from here 
		index = CurPointPos;
		pos = CurPoint;
		bForward =((num-CurPoint) > 0);
	}
	else if (num > Num-num)
	{
		// start from end
		index = EntriesUsed;
		pos = Num;
		bForward = FALSE;
	}
	else
	{
		// start from begining
		index = - 1;
		pos = - 1;
		bForward = TRUE;
	}
	
	// search forward or back
	if (bForward)
	{	
		pos++;
		for(;pos <= num; pos++)
		{
			index++;
			if (lpPoints[index].x == BEZIER_MARKER)
				index++;
		}
	}
	else 
	{
		pos--;
		for(;pos >= num; pos--)
		{
			index--;
			if (lpPoints[index].x == BEZIER_MARKER)
				index--;
		}
	}
	CurPointPos=index;
	CurPoint=num;
Found:
	if (lpPoint)
		*lpPoint = lpPoints[CurPointPos];
	return(lpPoint);
}

//=======================================================================
//	LPPOINT LastPoint( LPPOINT lpPoint)
//		lpPoint: The point structure to fill with the last pt.- Can be NULL.
//
//		
//		Sets the current point to be the last point.
//		Returns the lpPoint passed in or NULL for failure.
//		Failure can result if there are no points or the index is beyond 
//			the last point in a NON-Closed polygon.
//=======================================================================
LPPOINT CPoly::LastPoint(LPPOINT lpPoint)
//=======================================================================
{	
	if(Num<=0 || EntriesUsed<=0)
		return(NULL);
	CurPointPos=EntriesUsed-1;
	CurPoint=Num-1;
	if (lpPoint)
		*lpPoint = lpPoints[CurPointPos];
	return(lpPoint);
}



//=======================================================================
//	LPPOINT NextPoint(LPPOINT lpPoint)
//		lpPoint: The point structure to fill. Can be NULL.
//
//		Increments the current point and returns the "new" current point.
//		Will wrap back to first point for closed shapes.
//   	Returns lpPoint or NULL (as in GetPoint()).
//=======================================================================
LPPOINT CPoly::NextPoint(LPPOINT lpPoint)
//=======================================================================
{
	// out of bounds?
	if(!Num)
		return(NULL);
	// make it in range
	if (CurPoint >= Num-1) 
	{
		if(!bClosed)
			return(NULL);
		CurPoint = -1;
		CurPointPos = -1;
	}
	
	// Find the next non marker point
	CurPointPos++;
	if (lpPoints[CurPointPos].x == BEZIER_MARKER)
		CurPointPos++;
	CurPoint++;
	if (lpPoint)
		*lpPoint = lpPoints[CurPointPos];
	return(lpPoint);
}

//=======================================================================
//	LPPOINT ListNextPoint(LPPOINT lpPoint)
//		lpPoint: The point structure to fill. Can be NULL.
//
//		Increments the current point and returns the "new" current point.
//		Returns NULL if current point is the last point (even for closed)
//   	Returns lpPoint or NULL (as in GetPoint()).
//=======================================================================
LPPOINT CPoly::ListNextPoint(LPPOINT lpPoint)
//=======================================================================
{
	// out of bounds?
	if(!Num || CurPoint >= Num-1)
		return(NULL);
	// Find the next non marker point
	CurPointPos++;
	if (lpPoints[CurPointPos].x == BEZIER_MARKER)
		CurPointPos++;
	CurPoint++;
	if (lpPoint)
		*lpPoint = lpPoints[CurPointPos];
	return(lpPoint);
}


//=======================================================================
//	LPPOINT CurrentPoint(LPPOINT lpPoint)
//		lpPoint: The point structure to fill.  Can NOT be NULL.
//
//		Sets *lpPoint to be the current point.
//   	Returns lpPoint or NULL (as in GetPoint()).
//=======================================================================
LPPOINT CPoly::CurrentPoint(LPPOINT lpPoint)
//=======================================================================
{
	
	if (!WITHIN(CurPointPos, 0, EntriesUsed-1) || !Num)
		return(NULL);
	*lpPoint = lpPoints[CurPointPos];
	return(lpPoint);
}

//=======================================================================
//	void CPoly::SetCurrentPoint(LPPOINT lpPoint);
//
//		changes the coords of the current point to *lpPoint.
//=======================================================================
void CPoly::SetCurrentPoint(LPPOINT lpPoint)
//=======================================================================
{
	if (!WITHIN(CurPointPos, 0, EntriesUsed-1) || !Num)
		return;
	lpPoints[CurPointPos] = *lpPoint;
}


//=======================================================================
//	LPPOINT PreviousPoint(LPPOINT lpPoint)
//		lpPoint: The point structure to fill. Can be NULL.
//
//		Decrements the current point and returns the "new" current point.
//   	Returns lpPoint or NULL (as in GetPoint()).
//=======================================================================
LPPOINT CPoly::PreviousPoint(LPPOINT lpPoint)
//=======================================================================
{
	// out of bounds?
	if(!Num)
		return(NULL);
	// make it in range
	if (CurPoint <= 0) 
	{
		if(!bClosed)
			return(NULL);
		// get the last point (will not be marker)
		CurPoint = Num-1;
		CurPointPos = EntriesUsed-1;
		goto Found;
	}
	
	// Find the previous non marker point
	CurPointPos--;
	if (lpPoints[CurPointPos].x == BEZIER_MARKER)
		CurPointPos--;
	CurPoint--;

Found:
	if (lpPoint)
		*lpPoint = lpPoints[CurPointPos];
	return(lpPoint);
}


//=======================================================================
//	LONG PointType()
//		returns flags for the current point:	POLY_CURVE_BEGIN 	
//												POLY_CURVE_END 		
//												POLY_CURVE_HANDLE1 
//												POLY_CURVE_HANDLE2 
//=======================================================================
LONG CPoly::PointType()
//=======================================================================
{
	int i;
	LONG flags;
	
	
	if(CurPointPos < 0)
		return(FALSE);
	i = get_curve_index(CurPointPos);
	switch(i)
	{
		case 1:
			flags = POLY_CURVE_BEGIN;
			// is it also an end?
			if (CurPointPos < 2)
				i = get_curve_index(EntriesUsed-1);
			else
				i = get_curve_index(CurPointPos-2);
			if (i && i < BEZIER_IN)
				flags |= POLY_CURVE_END;
		break;
		case 2:
			flags = POLY_CURVE_HANDLE1;
		break;
		case 3:
			flags = POLY_CURVE_HANDLE2; 
		break;
		case 4:
			flags = POLY_CURVE_END;
		break;
		default:
			flags = 0;
	}
	return(flags);
}


//=======================================================================
// 	BOOL CPoly::IsCurve()
//		Returns TRUE if the current point is an endpoint or control point
//		for a bezier.
//=======================================================================
BOOL CPoly::IsCurve()
//=======================================================================
{
	int i;
	
	if(CurPointPos < 0)
		return(FALSE);
	i = get_curve_index(CurPointPos);
	if (!i || i > BEZIER_IN+1)
		return(FALSE);
	return(TRUE);
}

//=======================================================================
//	WORD CPoly::IsHandle()
//		If the current point is a control point for a curve then it 
//		returns the handle number (1 for first, 2 for second, etc).
//		Otherwise it returns 0.
//=======================================================================
WORD CPoly::IsHandle()
//=======================================================================
{
	int i;
	
	if(CurPointPos < 0)
		return(FALSE);
	i = get_curve_index(CurPointPos);
	if (!i)
		return(FALSE);
	if(!WITHIN(i, 2, BEZIER_IN-1))
		return(FALSE);
	return(i-1);
}

//=======================================================================
//	WORD CPoly::IsCurveEndPoint()
//		If the current point is not an endpoint for a curve then it 
//		returns 0.  Otherwise it returns a combination of the flags: 
//		POLY_CURVE_BEGIN, and POLY_CURVE_END.
//=======================================================================
WORD CPoly::IsCurveEndPoint()
//=======================================================================
{
	int i;
		
	if(CurPointPos < 0)
		return(FALSE);
	i = get_curve_index(CurPointPos);
	if (!i)
		return(0);
	if (i==BEZIER_IN)
		return(POLY_CURVE_END);
	if (i != 1)
		return(0);
	// it is a begining, is it an end also?
	i = CurPointPos - i - 1;
	if (i<0) 
		i += EntriesUsed;
	i = get_curve_index(CurPointPos);
	if (i == BEZIER_IN)
		return(POLY_CURVE_BEGIN | POLY_CURVE_END);
	return(POLY_CURVE_BEGIN);
}	

//=======================================================================
//	WORD CPoly::GetCurveData();
//		Gets the user data for the curve containing the current point.
//		If the given point is both a beging and end of a curve then the
//			user data will be for the curve which it begins.
//		If the current point isn't in a curve the result is undefined.
//=======================================================================
WORD CPoly::GetCurveData()
//=======================================================================
{
	return(0);
}

//=======================================================================
//	BOOL CPoly::SetCurveData(WORD wData);
//		Sets the user data for the curve containing the current point.
//		If the given point is both a beging and end of a curve then the
//			user data will be for the curve which it begins.
//		If the current point isn't in a curve the result is undefined.
//=======================================================================
void CPoly::SetCurveData(WORD wData)
//=======================================================================
{

}

//=======================================================================
//	BOOL AddPoint(LPPOINT lpPoint)
//		Adds the given point to the end of the polygon.
//
//		Returns FALSE if the polygon was unable to add the new point.
//=======================================================================
BOOL CPoly::AddPoint(LPPOINT lpPoint)
//=======================================================================
{
	if(!poly_make_room(1))
		return(FALSE);	
	lpPoints[EntriesUsed] = *lpPoint;
	EntriesUsed++;
	Num++;
	return(TRUE);
			
}

//=======================================================================
//	BOOL DeletePoint()
//		Deletes the current point. 
//		The new current point is the next point (if there is one).
//		If the point is in any way associated with a bezier it will not
//			be deleted.
//=======================================================================
BOOL CPoly::DeletePoint()
//=======================================================================
{
	if (CurPointPos < 0)
		return(FALSE);
	if (get_curve_index(CurPointPos))
		return(FALSE);
	// delete the point(s)
	MoveMemory((LPTR)&lpPoints[CurPointPos], 
				(LPTR)&lpPoints[CurPointPos+1], 
				sizeof(POINT)*(EntriesUsed-(CurPointPos+1)));
	Num--;
	EntriesUsed--;
	fix_cur_point();
	return(TRUE);
}

//=======================================================================
//	BOOL AddBezierLine(LPPOINT lpPoint)
//		Adds the given BEZIER to the end of the polygon.
//		The first point in the new bezier is the end of the old polygon.
//		If there are no points in the shape the result is undefined
//
//		Returns FALSE if the polygon was unable to add the new points.
//=======================================================================
BOOL CPoly::AddBezierLine(LPPOINT lpHandle1, LPPOINT lpHandle2, LPPOINT lpEnd)
//=======================================================================
{
	if(!poly_make_room(BEZIER_IN))
		return(FALSE);	
	// add first point if needed
	if (Num <= 0)
	{
		lpPoints[0] = *lpHandle1;
		EntriesUsed=1;
		Num=1;
	}
	// add bezier marker (doen't increase Num)
	lpPoints[EntriesUsed] = lpPoints[EntriesUsed-1];
	lpPoints[EntriesUsed-1].x = BEZIER_MARKER;
	lpPoints[EntriesUsed-1].y = 0;
	if (CurPointPos == EntriesUsed-1)
		CurPointPos = EntriesUsed;
	EntriesUsed++;
		
	// add handles and endpoints
	lpPoints[EntriesUsed] = *lpHandle1;
	EntriesUsed++;
	Num++;
	lpPoints[EntriesUsed] = *lpHandle2;
	EntriesUsed++;
	Num++;
	lpPoints[EntriesUsed] = *lpEnd;
	EntriesUsed++;
	Num++;
	return(TRUE);
}


//=======================================================================
//	BOOL AddBezierLine(LPPOINT lpPoint)
//		Adds the given BEZIER to the end of the polygon.
//		If the polygon is not closed & no point is added after this 
//			then the result is undefined.
//
//		Returns FALSE if the polygon was unable to add the new points.
//=======================================================================
BOOL CPoly::AddBezierLine2(LPPOINT lpStart, LPPOINT lpHandle1, LPPOINT lpHandle2)
//=======================================================================
{
	if(!poly_make_room(BEZIER_IN))
		return(FALSE);	
	// add bezier marker (doen't increase Num)
	lpPoints[EntriesUsed].x = BEZIER_MARKER;
	lpPoints[EntriesUsed].y = 0;
	EntriesUsed++;
	// add handles and startpoint
	lpPoints[EntriesUsed] = *lpStart;
	EntriesUsed++;
	Num++;
	lpPoints[EntriesUsed] = *lpHandle1;
	EntriesUsed++;
	Num++;
	lpPoints[EntriesUsed] = *lpHandle2;
	EntriesUsed++;
	Num++;
	return(TRUE);
}

//=======================================================================
//	BOOL CPoly::InsertPoint(LPPOINT lpPoint)
//		Inserts the given point after the current point.  If the current 
//			point was set to -1 then it will go before the first point.
//		If the current point is a bezier start or first handle the point 
//			will be inserted as the new bezier end point.
//  	The new current point is the inserted point.
//
//		Returns FALSE for failure).
//=======================================================================
BOOL CPoly::InsertPoint(LPPOINT lpPoint)
//=======================================================================
{
	int i;
	
	if(!poly_make_room(1))
		return(FALSE);
	
	// in a bezier? - make it point to last handle
	if(i = get_curve_index(CurPointPos))
		if(i<BEZIER_IN-1)
		{
			// first or second point
			CurPointPos += BEZIER_IN-i-1;
			CurPoint += BEZIER_IN-i-1;
		}
	// get the new position
	CurPointPos++;
	CurPoint++;
	// move data to make room
	MoveMemory((LPTR)&lpPoints[CurPointPos+1], 
				(LPTR)&lpPoints[CurPointPos], 
				sizeof(POINT)*(EntriesUsed-(CurPointPos)));
	// add the point
	lpPoints[CurPointPos] = *lpPoint;
	Num++;
	EntriesUsed++;
	return(TRUE);
}

//=======================================================================
//	BOOL LineToBezier(LPPOINT lpHandle1, LPPOINT lpHandle1)
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
int CPoly::LineToBezier(LPPOINT lpHandle1, LPPOINT lpHandle2)
//=======================================================================
{
	int after, i;

	if(!poly_make_room(BEZIER_IN+1))
		return(0);
	
	after = CurPointPos;	   
	// valid point?
	if (after < 0 || ((after >= EntriesUsed-1) && !bClosed))
		return(TRUE);
	// in a bezier? - if its not an endpoint then do nothing.
	if(i = get_curve_index(after))
		if(i<BEZIER_IN)
			return(-1);
	// move data to make room
	if (after < EntriesUsed-1)
		MoveMemory((LPTR)&lpPoints[after+BEZIER_IN], 
					(LPTR)&lpPoints[after+1], 
					sizeof(POINT)*(EntriesUsed-(after+1)));
	// add bezier marker (doen't increase Num)
	lpPoints[after+1] = lpPoints[after];
	lpPoints[after].x = BEZIER_MARKER;
	lpPoints[after].y = 0;
	if (CurPointPos == after)
		CurPointPos = after+1;
	EntriesUsed++;
	after+=2;
	// add handles and endpoints
	lpPoints[after] = *lpHandle1;
	EntriesUsed++;
	Num++;
	after++;
	lpPoints[after] = *lpHandle2;
	EntriesUsed++;
	Num++;
	return(1);
}


//=======================================================================
//	void BezierToLine()
//		Converts the current point and its forward neighbor to a line.
//		If the segment is not a bezier then this will have no effect.
//		If its a bezier handle then the bezier containing it becomes a line.
//  	The new current point is the line start.
//
//		Returns TRUE if handles are deleted.
//=======================================================================
BOOL CPoly::BezierToLine()
//=======================================================================
{
	int i;
	
	if (CurPointPos < 1)
		return(FALSE);
	i = get_curve_index(CurPointPos);
	// no bezier, or bezier end 
	if (!i || i >= BEZIER_IN)
		return(FALSE);
	// if it a handle then special case.
	if (i > 1)
	{
		CurPointPos -= i-1;
		CurPoint -= i-1;
	}
	
	// get rid of marker
	lpPoints[CurPointPos-1] = lpPoints[CurPointPos];
	CurPointPos--;
	// move data
	MoveMemory((LPTR)&lpPoints[CurPointPos+1], 
				(LPTR)&lpPoints[CurPointPos+BEZIER_IN], 
				sizeof(POINT)*(EntriesUsed-(CurPointPos+BEZIER_IN)));
	EntriesUsed -= BEZIER_IN-1;
	Num -= BEZIER_IN-2;
	fix_cur_point();
	return(TRUE);
}


//=======================================================================
// BOOL CPoly::ForEach(LPPOLYPROC lpProc, LONG lData)
//
//		Applies the given function to every point in the polygon.
//		If any of the calls returns FALSE then the function returns FALSE
//=======================================================================
BOOL CPoly::ForEach(LPPOLYPROC lpProc, LONG lData)
//=======================================================================
{
	int i;
	int bezier;
	WORD bflags;
	LPPOINT lpPoint;
	
	bezier = get_curve_index(0);
	if (bezier)
		bezier = BEZIER_IN+1 - bezier;
	i=0;
	lpPoint = lpPoints;
	while(i<EntriesUsed)
	{
		switch(bezier)
		{
			case 1:
				bflags = POLY_CURVE_END;
				bezier--;
			break;
			case 2:
				bflags = POLY_CURVE_HANDLE2;
				bezier--;
			case 3:
				bflags = POLY_CURVE_HANDLE1;
				bezier--;
			break;
			case 4:
				bflags = POLY_CURVE_BEGIN;
				bezier--;
			break;
			default:
				bflags = 0;
		}
		if(lpPoint->x == BEZIER_MARKER)
		{
			bflags |= POLY_CURVE_BEGIN;
			lpPoint++;
			i++;
		}
		if (!(*lpProc)(lpPoint, bflags, lData))
			return(FALSE);
		lpPoint++;
		i++;
	}
	return(TRUE);
}


//=======================================================================
//	void CloseCPoly(BOOL bClosed)
//
//		Makes the polygon opened or closed
//=======================================================================
void CPoly::ClosePoly(BOOL bClosed)
//=======================================================================
{
	this->bClosed = bClosed;
}


//=======================================================================
//	BOOL IsClosed()
//		
//		Returns True if it is a closed polygon.
//=======================================================================
BOOL CPoly::IsClosed()
//=======================================================================
{
	return(bClosed);
}


//=======================================================================
// 	LPPOINT CPoly::GetData(LPPOINT lpStorage, LPINT lpiSize)
//		lpStorage: 	An array of points, filled by this fn.
//		iStorage:  The size of lpStorage in points.
//		iPointsToUse: The number of polygon points to attempt. -1 for 
//			entire shape from current point (closed shapes will wrap).
//			Any bezier which is started will be completed.
//		lpTForm: Applys the given tform to the data- can be null. (Note:
//			the current code assumes rotation and translation only!)
//		lpiOut: Upon exit it will contain the number of pixel points written.
//		lpStorage: Optional.  Memory to put the data into.
//		iStorage: Optional.  On call should have the size of lpStorage.	On Return 
//			It will contain the number of polygon points expanded.
//
//		This function expands any curves into points and puts the new 
//			polygon segment into lpStorage.
//		The fn wrorks from, and will update, the current point.
//
//		Note: SizeData must mirror this function.
//=======================================================================
LPPOINT CPoly::GetData(int iPointsToUse, LPTFORM lpTForm, LPINT lpiOut, LPPOINT lpStorage, LPINT lpiStorage)
//=======================================================================
{
	int iout=0;		// points output
	LPPOINT lpout;
	POINT curve[BEZIER_IN];
	int i, count, iStorage, iTempCurPointPos;
	
	if (lpiStorage)
	{
		iStorage = *lpiStorage;
		*lpiStorage = 0;
	}
	
	if (!iPointsToUse || !Num || (lpStorage && !lpiStorage))
		goto ExitNoneDone;
	if (!lpStorage)
	{           
		iStorage = SizeData(iPointsToUse, lpTForm);
		iStorage += MAX_BEZIER_OUT + 1;
		lpStorage = (LPPOINT)Alloc(sizeof(POINT)*(iStorage + 1));
		if (!lpStorage)
			goto ExitNoneDone;
	}
	
	// be sure we are at the begining
	if (CurPointPos < 0)
		CurPointPos = CurPoint = 0;
	
	// skip to begining or end of bezier
	if (i=get_curve_index(CurPointPos))
	{
		if (i==1)
			CurPointPos--;
		else if (i<BEZIER_IN)
		{
			CurPointPos += BEZIER_IN-i;
			CurPoint += BEZIER_IN-i;
		}
	}
		
	// need to do all points?
	if (iPointsToUse < 0)
		iPointsToUse = bClosed ? Num : (Num - CurPoint - 1);
	count = iPointsToUse;	// points remaining
	lpout = lpStorage;	
	while( count >= 0 && iout < iStorage)
	{
		// wrap 
		if (CurPointPos >= EntriesUsed)
		{
			CurPointPos = 0;
			CurPoint = 0;
			if (!bClosed)
				break;
			// connect end with current point (if bezier)
			if (lpPoints[CurPointPos].x == BEZIER_MARKER)
			{
				// add the start of bezier... but dont update current position
				if (lpTForm)
					Transformer(lpTForm, &lpPoints[CurPointPos+1], &lpout->x, &lpout->y);
				else
		 			*lpout = lpPoints[CurPointPos+1];
				lpout++;
				iout++;
				count--;
				continue;
			}
		}
	
		if (lpPoints[CurPointPos].x == BEZIER_MARKER)
			// bezier
		{
			// be sure we can handle it
			if (iout+MAX_BEZIER_OUT > iStorage)
				break;
			// get bezier points
			CurPointPos++;
			curve[0] = lpPoints[CurPointPos++]; // start
			curve[1] = lpPoints[CurPointPos++]; // handle1
			curve[2] = lpPoints[CurPointPos++]; // handle2
			CurPoint += 3;
			count -= 3;
			// dont do wrap now... save for next time
			iTempCurPointPos = CurPointPos;
			if (iTempCurPointPos >= EntriesUsed)
				iTempCurPointPos = 0;
			if (lpPoints[iTempCurPointPos].x == BEZIER_MARKER)
				curve[3] = lpPoints[iTempCurPointPos+1];
			else 
			{
				curve[3] = lpPoints[iTempCurPointPos++];
				CurPointPos++;
				CurPoint++;
				count--;
			}
			if (lpTForm)
			{
				Transformer(lpTForm, &curve[0], &curve[0].x, &curve[0].y);
				Transformer(lpTForm, &curve[1], &curve[1].x, &curve[1].y);
				Transformer(lpTForm, &curve[2], &curve[2].x, &curve[2].y);
				Transformer(lpTForm, &curve[3], &curve[3].x, &curve[3].y);
			}
			// expand curve
			i = CurveToPoints(curve, BEZIER_MARKER, lpout);
			iout += i;
			lpout += i;
		}	
		else
		{
			// point only
			if (lpTForm)
				Transformer(lpTForm, &lpPoints[CurPointPos], &lpout->x, &lpout->y);
			else
		 		*lpout = lpPoints[CurPointPos];
			lpout++;
			iout++;
			count--;
			CurPointPos++;
			CurPoint++;
		}
	}

	// wrap 
	if (CurPointPos >= EntriesUsed)
	{
		CurPointPos = 0;
		CurPoint = 0;
	}

	if (lpiStorage)
		*lpiStorage = iPointsToUse - count;
ExitNoneDone:
	// ensure that current point isn't a bezier marker
	if (WITHIN(CurPointPos, 0, EntriesUsed-1))
	{
		if (lpPoints[CurPointPos].x == BEZIER_MARKER)
			CurPointPos++;
	}
	*lpiOut = iout;
	return(lpStorage);
		
}

//=======================================================================
//	 Returns the number of points that GetData would use given the same 
//		iPointsToUse.
//=======================================================================
int CPoly::SizeData(int iPointsToUse, LPTFORM lpTForm)
//=======================================================================
{
	int iout=0;		// points output
	POINT curve[BEZIER_IN];
	int i, count, iTempCurPointPos;
	int curpoint = CurPoint;
	int curpointpos = CurPointPos;
	
	if (!Num)
		goto Done;

	// be sure we are at the begining
	if (curpointpos < 0)
		curpointpos = curpoint = 0;
	
	// skip to begining or end of bezier
	if (i=get_curve_index(curpointpos))
	{
		if (i==1)
			curpointpos--;
		else if (i<BEZIER_IN)
		{
			curpointpos += BEZIER_IN-i;
			curpoint += BEZIER_IN-i;
		}
	}
		
	// need to do all points?
	if (iPointsToUse < 0)
		iPointsToUse = bClosed ? Num : (Num - CurPoint - 1);
	count = iPointsToUse;	// points remaining
	while( count >= 0)
	{
		// wrap 
		if (curpointpos >= EntriesUsed)
		{
			curpointpos = 0;
			curpoint = 0;
			if (!bClosed)
				goto Done;
			if (lpPoints[curpointpos].x == BEZIER_MARKER)
			{
				iout++;	// for the joiner
				count--;
				continue;	
			}
		}
		
		if (lpPoints[curpointpos].x == BEZIER_MARKER)
		// bezier
		{
			// get bezier points
			curpointpos++;
			curve[0] = lpPoints[curpointpos++];
			curve[1] = lpPoints[curpointpos++];
			curve[2] = lpPoints[curpointpos++];
			curpoint += 3;
			count -= 3;
			
			iTempCurPointPos = curpointpos;
			if (iTempCurPointPos >= EntriesUsed)
				iTempCurPointPos = 0;
			if (lpPoints[iTempCurPointPos].x == BEZIER_MARKER)
				curve[3] = lpPoints[iTempCurPointPos+1];
			else 
			{
				curve[3] = lpPoints[iTempCurPointPos++];
				curpointpos++;
				curpoint++;
				count--;
			}
			// curve expansion may be dependant on resolution
			if (lpTForm)
			{
				Transformer(lpTForm, &curve[0], &curve[0].x, &curve[0].y);
				Transformer(lpTForm, &curve[1], &curve[1].x, &curve[1].y);
				Transformer(lpTForm, &curve[2], &curve[2].x, &curve[2].y);
				Transformer(lpTForm, &curve[3], &curve[3].x, &curve[3].y);
			}
			// expand curve
			i = CurveToPoints(curve, BEZIER_MARKER, NULL);
			iout += i;
		}	
		else
		{
			// point only
			iout++;
			count--;
			curpointpos++;
			curpoint++;
		}
	}
	
Done:
	return(iout);
		
}

//=======================================================================
// 	Geta the bounding rect for all the points in the rect
//=======================================================================
void CPoly::GetBounds(LPRECT lpBounds)
//=======================================================================
{
	int i;		
	
	AstralSetRectEmpty(lpBounds);
	for (i=0; i<EntriesUsed;i++)
	{
		if (lpPoints[i].x == BEZIER_MARKER)
			continue;
		if (lpPoints[i].x < lpBounds->left)
			lpBounds->left = lpPoints[i].x;
		if (lpPoints[i].x > lpBounds->right)
			lpBounds->right = lpPoints[i].x;
		if (lpPoints[i].y < lpBounds->top)
			lpBounds->top = lpPoints[i].y;
		if (lpPoints[i].y > lpBounds->bottom)
			lpBounds->bottom = lpPoints[i].y;
	}
}

//=======================================================================
//	BOOL CPoly::Copy(LPCPoly lpPoly)
//		copies all the data from *this into lpPoly.
//		Copies bClosed flag.
//	Returns FALSE if there is a memory failure.
//=======================================================================
BOOL CPoly::Copy(LPCPoly lpPoly)
//=======================================================================
{
	int i;
	POINT p1, p2, p3;
	
	if (!lpPoly)
		return(NULL);
	lpPoly->ClosePoly(bClosed);
	GetPoint(-1);
	for (i=0; i<Num; i++)
	{
		if (!NextPoint(&p1))
			return(TRUE);
		if (IsCurveEndPoint() & POLY_CURVE_BEGIN)
		{
			if (!NextPoint(&p2))
				return(TRUE);
			i++;
			if (!NextPoint(&p3))
				return(TRUE);
			i++;
			if (!lpPoly->AddBezierLine2(&p1, &p2, &p3))
				return(FALSE);
		} 
		else
		{
			if (!lpPoly->AddPoint(&p1))
				return(FALSE);
		}
	}
	return(TRUE);
}

//=======================================================================
//	int delete_point(int index, LPINT lpFirst)
//		index: the point to delete.
//		lpFirst: on return is set to first point deleted(if any).
//
//		Deletes the point.
//		If the point is in any way associated with a bezier it will not
//			be deleted.
//		Returns 0 for failure.
//=======================================================================
//int CPoly::delete_point(int index, LPINT lpFirst)
////=======================================================================
//{
//	int i;
//	int bezier = 0;
//	int num = 1;
//	
//	if (i=get_curve_index(index))
//		if (WITHIN(i, 2, BEZIER_IN-1))
//		{
//			// handle- make not a bezier
//			
//		}
//		else if (i==1)
//		{
//			// start of bezier
//		}
//		else if (i==BEZIER_IN)
//		{
//			// end of bezier
//		}
//		
//	// delete the point(s)
//	if (!WITHIN(index, 0, EntriesUsed-1))
//		return(0);
//	if (!WITHIN(index+num, 1, EntriesUsed))
//		return(0);
//	MoveMemory((LPTR)&lpPoints[index], 
//				(LPTR)&lpPoints[index+num+bezier], 
//				sizeof(POINT)*(EntriesUsed-(index+num+bezier)));
//	Num -= num;
//	EntriesUsed -= num+bezier;
//	return(num);
//}


//=======================================================================
//	int CPoly::insert_point(int index, LPPOINT lpPoint)
//		Inserts the given point after the given index.  If the indexed 
//			point is to -1 then it will go before the first point.
//		If the indexed point is a bezier start or first handle the point 
//			will be inserted as the new bezier end point.
//
//		Returns the index of the new point (-1 for failure).
//=======================================================================
//int CPoly::insert_point(int index, LPPOINT lpPoint)
//=======================================================================
//{
//	int i;
//	
//	if(!poly_make_room(1))
//		return(-1);
//	
//	// in a bezier? - make it point to last handle
//	if(i = get_curve_index(index))
//		if(i<BEZIER_IN-1)
//			index += BEZIER_IN-i-1;
//	
//	// move data to make room
//	if (index < EntriesUsed-1)
//		MoveMemory((LPTR)&lpPoints[index+2], 
//					(LPTR)&lpPoints[index+1], 
//					sizeof(POINT)*(EntriesUsed-(index+1)));
//	// add the point
//	lpPoints[index+1] = *lpPoint;
//	Num++;
//	EntriesUsed++;
//	return(index+1);
//}

//=======================================================================
//	int get_curve_index(int index);
//		begins at the index given and searches back for a bezier marker.
//		Returns 0 if there is no marker.  Otherwise it returns the 
//			distance back to the marker.
//=======================================================================
int CPoly::get_curve_index(int index)
//=======================================================================
{
 	int i;
	
	if(!Num)
		return(0);
	// back up BEZIER_IN points searching for bezier marker
	for (i=0;i<BEZIER_IN;i++)
	{
		index--;
		// need to wrap?
		if(index < 0)
			index = EntriesUsed-1;
		if (lpPoints[index].x == BEZIER_MARKER)
			return(i+1);
	}
	return(0);		 
}


//=======================================================================
//	BOOL CPoly::poly_make_room(int needed)
//		Ensures that there is at least 'needed' room at the end of lpPoint.
//
//		Returns FALSE if the memory can not be allocated.
//=======================================================================
BOOL CPoly::poly_make_room(int needed)
//=======================================================================
{
	int alloc;
	LPPOINT lpNewMem = NULL;
	
	needed = (needed + EntriesUsed) - EntriesAllocated;
	if (needed <= 0)
		return(TRUE);
	alloc = Max(needed, Min(EntriesAllocated/2, POLY_MAX_ALLOC));
	if (lpPoints)
		lpNewMem = (LPPOINT)AllocExtend((LPTR)lpPoints, sizeof(POINT)*(alloc+EntriesAllocated));
	if (!lpNewMem)
	{
		// get what we need
		lpNewMem = (LPPOINT)Alloc(sizeof(POINT)*(alloc+EntriesAllocated));
		if (!lpNewMem)
			return(FALSE);
		if (lpPoints)
		{
			copy((LPTR)lpPoints, (LPTR)lpNewMem, sizeof(POINT)*EntriesUsed);
			FreeUp(lpPoints);
		}
	}
	lpPoints = lpNewMem;
	EntriesAllocated += alloc;
	return(TRUE);
}


//************************************************************************
//		Removes duplicates and horizontal & vertical lines.
//************************************************************************
void CPoly::Squeeze()
//************************************************************************
{
	int i, iOutNum, iOutEntries;
	LPPOINT lpDst, lpSrc;
	POINT last, start, pt;
	BOOL bGotHoriz, bGotVert;
	
	iOutNum = iOutEntries = 0;
	lpDst = lpSrc = lpPoints;
	last.x = last.y = INT_MAX;
	start = last;
	bGotHoriz = bGotVert = FALSE;
	for (i=0; i<EntriesUsed; i++)
	{
		pt = *lpSrc++;
		if (pt.x == BEZIER_MARKER)
		{
			if (bGotHoriz || bGotVert)
			{
				// we just finished a run
				*lpDst++ = last;
				iOutNum++;
				iOutEntries++;
				bGotHoriz = bGotVert = FALSE;
			}
			// Just copy bezier
			*lpDst++ = pt;	// marker
			*lpDst++ = *lpSrc++;	// first point
			*lpDst++ = *lpSrc++;	// handle1
			*lpDst++ = *lpSrc++;	// handle2
			i += 3;
			iOutNum += 3;
			iOutEntries += 4;
			last.x = last.y = INT_MAX;
			bGotHoriz = bGotVert = FALSE;
		}
		else
		{
			if (pt.x == last.x && pt.y == last.y)
				continue;
			if ((bGotHoriz && (start.y==pt.y)) || (bGotVert && (start.x==pt.x)))
			{
					// still horiz or vert
					last = pt;
					continue;
			}
			if (bGotHoriz || bGotVert)
			{
				// we just finished a run
				*lpDst++ = last;
				iOutNum++;
				iOutEntries++;
				bGotHoriz = bGotVert = FALSE;
			}
			// check for new run
			if (last.x == pt.x)
			{
				bGotVert = TRUE;
				start = last;
				last = pt;
				continue;
			}
			else if (last.y == pt.y)
			{
				bGotHoriz = TRUE;
				start = last;
				last = pt;
				continue;
			}
			// no run... just add the current point
			last = pt;
			*lpDst++ = pt;
			iOutNum++;
			iOutEntries++;
		}
	}	// end for each entry
	// do we need to finish a run?
	if (iOutEntries > 0)
	{
		if ((last.x != lpDst[-1].x || last.y != lpDst[-1].y) && last.x != INT_MAX)
		{
			*lpDst++ = last;
			iOutNum++;
			iOutEntries++;
		}
	}
	Num = iOutNum;
	EntriesUsed = iOutEntries;
}	



//************************************************************************
// PARAMETERS:
//			int Tolerance- the maximum distance any point can deviate.
// DESCRIPTION:
//			Reduces the number of points by detecting colinear points 
//			and turning them into sinle line segments.
//************************************************************************
void CPoly::Squeeze(int Tolerance)
//************************************************************************
{
	int i, iOutNum, iOutEntries, num;
	LPPOINT lpDst, lpSrc;
	POINT start, last, end;
	
	if (EntriesUsed < 1)
		return;
	if (EntriesUsed < 3 || Tolerance==0)
	{
//		Squeeze();
		return;
	}
	
	lpDst = lpSrc = lpPoints;
	iOutNum = iOutEntries = 1;
	start = *lpDst++ = *lpSrc++;
	last = *lpSrc++;
	num = 1;	// points between start & lpSrc
	// loop: *lpSrc=lpPoints[i], 
	for (i=2; i<EntriesUsed; i++)
	{
		end = *lpSrc;
		if (!check_tolerance(&lpSrc[-num-1], lpSrc, num, Tolerance))
		{
			// we are at the end of our rope
			*lpDst++ = last;
			iOutEntries++;
			iOutNum++;
			start = last;
			last = end;
			num = 1;
		}
		else
		{
			// keep on truckin'
			last = end;
			num++;
		}
		lpSrc++;
	}
	// set new number of points
	Num = iOutNum;
	EntriesUsed = iOutEntries;
}	
		
		
//************************************************************************
//
// PARAMETERS:
//			int num-the number of points to check
// DESCRIPTION:
//			Checks all points starting at lpFirst+1, for tolerence 
//			to line lpFirst,lpLast.
// RETURNS:
//			True if Tolerance conditions are met.
//************************************************************************
LOCAL  BOOL check_tolerance(LPPOINT lpFirst, LPPOINT lpLast, int num, int Tolerance)
//************************************************************************
{
	int dx, dy, xmin, xmax, ymin, ymax;
	double x, y, tolerance;
	POINT start, test;
	
	
	if (num<1)
		return(TRUE);
	tolerance = .25 + Tolerance;
	start = *lpFirst;
	dx = lpLast->x - start.x;
	dy = lpLast->y - start.y;
	xmin = min(start.x-Tolerance, lpLast->x-Tolerance);
	xmax = max(start.x+Tolerance, lpLast->x+Tolerance);
	ymin = min(start.y-Tolerance, lpLast->y-Tolerance);
	ymax = max(start.y+Tolerance, lpLast->y+Tolerance);
	
	for (;num>0;num--)
	{
		test = *++lpFirst;
		if (dy)
		{
			x = (test.y-start.y)*(double)dx/dy + start.x;
			if (WITHIN(test.x, x-tolerance, x+tolerance))
				if (WITHIN(x, xmin, xmax))
					continue;
		}
		if (dx)
		{
			y = (test.x-start.x)*(double)dy/dx + start.y;
			if (WITHIN(test.y, y-tolerance, y+tolerance))
				if (WITHIN(y, ymin, ymax))
					continue;
		}
		break;
	}
	if (num>0)
		return(FALSE);
	return(TRUE);
}
	

//=======================================================================
// 	int CurveToPoints
//		lpPoints:  Bezier handle points.
//		type:  BEZIER_MARKER is the only type supported.
//		lpOutputPoints:  Result. - must be at least MAX_BEZIER_OUT or NULL
//		
//		returns the number of points written (or needed if !lpOutputPoints)
//=======================================================================
int CurveToPoints(LPPOINT lpPoints, int type, LPPOINT lpOutputPoints )
//=======================================================================
{
	int n, t, i, nOutPoints, lastx, lasty, x, y;
	int nMaxPoints;
	int nPoints;
	LFIXED tscale, fdx, fdy, tscalebase;
	FPOINT ptR[MAX_BEZIER_IN];
	LPFPOINT R;
	LPPOINT P;

	if (type != BEZIER_MARKER)
		return(0);
	nPoints = BEZIER_IN;
	
	// determine the maximum number of points we will want (may effect quality)
	x = y = INT_MAX; 
	lastx = lasty = INT_MIN;
	for (i=0;i<nPoints;i++)
	{
		x = min(x, lpPoints[i].x);
		y = min(y, lpPoints[i].y);
		lastx = max(lastx, lpPoints[i].x);
		lasty = max(lasty, lpPoints[i].y);
	}
	// get delta
	x = lastx-x;
	y = lasty-y;
	nMaxPoints = (x+y)/4;
	nMaxPoints = bound(nMaxPoints, 5, MAX_BEZIER_OUT);
	
//	Transformer(lpTForm, lpPoints, &lastx, &lasty);
	if (lpOutputPoints)
		*lpOutputPoints++ = *lpPoints; //lastx,lasty;
	nOutPoints = 1;
	
	tscalebase = FGET( 1, nMaxPoints );
	tscale = 0;
	
	for ( t=1; t<nMaxPoints; t++ )
	{
		R = ptR;
		P = lpPoints;
		n = nPoints;
		tscale += tscalebase;
		while (--n >= 0)
		{
//			FTransformer(lpTForm, P, &R->fx, &R->fy);
			R->fx = MAKEFIXED(P->x);
			R->fy = MAKEFIXED(P->y);
			R++;
			P++;
		}
		n = nPoints - 1;
		while ( --n >= 0 )
		{
			R = ptR;
			for ( i=0; i<=n; i++ )
			{
				fdx = (R+1)->fx - R->fx;
				fdy = (R+1)->fy - R->fy;
				R->fx += FIXMUL( fdx, tscale );
				R->fy += FIXMUL( fdy, tscale );
				R++;
			}
		}
		x = WHOLE(ptR[0].fx);
		y = WHOLE(ptR[0].fy);
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
	x = WHOLE(ptR[n].fx);
	y = WHOLE(ptR[n].fy);
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



//=======================================================================
//	Gets the max bounds for the poly list.
//	If there are no polygons or poly list then the rect is empty. 
//=======================================================================
BOOL PolyListGetBounds(LPCPolyList lpList, LPRECT lpRect)
//=======================================================================
{
	AstralSetRectEmpty(lpRect);
	if (lpList)
		lpList->ForEach1((LPLISTPROC1)poly_get_bounds, (LONG)lpRect);
	return(TRUE);
}

LOCAL poly_get_bounds(LPCPoly lpPoly, LPRECT lpRect)
{
	RECT rNew;
	
	lpPoly->GetBounds(&rNew);
	AstralUnionRect(lpRect, lpRect, &rNew);
	return(TRUE);
}


//=======================================================================
//	Frees all polygons in the poly list
//	Also frees the list itself
//=======================================================================
void FreePolyList(LPCPolyList lpPolyList)
//=======================================================================
{
	FreePolys(lpPolyList);
	if (lpPolyList)
		delete(lpPolyList);
}

	
//=======================================================================
//	Frees all polygons in the poly list
//	Does not free the list itself
//=======================================================================
void FreePolys(LPCPolyList lpPolyList)
//=======================================================================
{
	POSITION PolyPos;
	LPCPoly lpPoly;
	
	if (lpPolyList)
	{
		PolyPos = lpPolyList->GetHeadPosition();
		while(PolyPos)
			if (lpPoly = (LPCPoly)lpPolyList->GetNextPtr(PolyPos))
				delete(lpPoly);
	}
}

	

/************************************************************************/
//	Always returns TRUE. (For ForEach)
/************************************************************************/
BOOL  PolySqueeze(LPCPoly lpPoly)
/************************************************************************/
{
	if (!lpPoly)
		return(TRUE);
	lpPoly->Squeeze();
	return(TRUE);
}


//=======================================================================
//=======================================================================
BOOL PolyTForm(LPCPoly lpPoly, LPTFORM lpTForm)
//=======================================================================
{
	lpPoly->ForEach((LPPOLYPROC)point_tform, (LONG)lpTForm);
	return(TRUE);
}

LOCAL BOOL point_tform(LPPOINT lpPoint, WORD wFlags, LPTFORM lpTForm)
{
	Transformer(lpTForm, lpPoint, &lpPoint->x, &lpPoint->y);
	return(TRUE);
}

//=======================================================================
//	Goes to the previous non-handle point
//	if it fails then the currnet position is not changed
//=======================================================================
BOOL PreviousControlPoint(LPCPoly lpPoly, LPPOINT lpPoint)
//=======================================================================
{
	POINT pt;
	int i;
	
	for (i=0; i<BEZIER_IN-1;i++)
	{
		if (!lpPoly->PreviousPoint(&pt))
		{
			while (i-- > 0)
				lpPoly->NextPoint();
			return(FALSE);
		}
		if (!lpPoly->IsHandle())
		{
			if (lpPoint)
				*lpPoint = pt;
			return(TRUE);
		}
	}
	// Should never happen!
	while (i-- > 0)
		lpPoly->NextPoint();
	return(FALSE);
}

//=======================================================================
//	Goes to the next previous non-handle point
//	if it fails then the currnet position is not changed
//=======================================================================
BOOL NextControlPoint(LPCPoly lpPoly, LPPOINT lpPoint)
//=======================================================================
{
	POINT pt;
	int i;
	
	for (i=0; i<BEZIER_IN-1;i++)
	{
		if (!lpPoly->NextPoint(&pt))
		{
			while (i-- > 0)
				lpPoly->PreviousPoint();
			return(FALSE);
		}
		if (!lpPoly->IsHandle())
		{
			if (lpPoint)
				*lpPoint = pt;
			return(TRUE);
		}
	}
	// Should never happen!
	while (i-- > 0)
		lpPoly->PreviousPoint();
	return(FALSE);
}

//=======================================================================
//	Makes a CLOSED polygon which mimics the rectangle.
//	Returns NULL for failure.
//  If bExpand then it expands the lower right by 1 (so mask will mask
//	exactally the rect)
//=======================================================================
LPCPoly MakeRectPoly(LPCPoly lpPoly, LPRECT lpRect, BOOL bExpand)
//=======================================================================
{
	POINT pt;
	
//	bExpand = NO;
	
	lpPoly->ClosePoly(YES);
	pt.x = lpRect->left;
	pt.y = lpRect->top;
	if (!lpPoly->AddPoint(&pt))
		return(NULL);
	pt.x = lpRect->right;
	if (bExpand)
		pt.x++;
	if (!lpPoly->AddPoint(&pt))
		return(NULL);
	pt.y = lpRect->bottom;
	if (bExpand)
		pt.y++;
	if (!lpPoly->AddPoint(&pt))
		return(NULL);
	pt.x = lpRect->left;
	if (!lpPoly->AddPoint(&pt))
		return(NULL);
	return(lpPoly);
}

