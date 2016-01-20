/* (c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.  This material is
   confidential and a trade secret.  Permission to use this work for any purpose
   must be obtained in writing from MICROGRAFX, 1303 Arapaho, Rd., Richardson,
   Tx.  75081.

*******************************************************************************
*******************************************************************************

								   FitPoly

*******************************************************************************
*******************************************************************************
*/

#include "pp.h"
#include "pointutl.h"

/* ********************************* History ******************************** */
// Most of the code adapted from mgxgre\fitbez.cpp
/* ******************************** Constants ******************************* */


#define MAX_BUFFER_SIZE	((UINT)(65536L / sizeof (POINT)))
#define MAX_CONTINUITY_INDEX	20
#define FXR_PRECISION_FACTOR	65536L
#define FXR_ONE_HALF			0x8000L
#define MAX_ERROR			   4
#define PI 3.141592
#define TWOPI 6.283184

/* ********************************** Macros ******************************** */

//#define NEAR_ZERO(Value)		(-1.0E-13 < (Value) && (Value) < 1.0E-13)
#define NEAR_ZERO(Value)		(-1.0E-10 < (Value) && (Value) < 1.0E-10)

// for setting DPOINTS
#define SETPT(p1, p2) 	{p1.x = p2.x;p1.y= p2.y;}

/* ********************************** Types ********************************* */

typedef double FLOAT_REAL;
typedef double   FIXED_REAL;

typedef signed long coordi;
typedef double coordf;

typedef struct
{
	coordi		r;
	int			theta;
}POLAR_POINT;

typedef struct
{
	POINT	  		cartesian;
	POLAR_POINT  	polar;
}
POINT_DUO;

typedef struct
{
	UINT	  DiscontinuityAngle;
	double	DiscontinuityRatio;
}
CONTINUITY_RECORD;

/* ******************************* Local Data ******************************* */

static CONTINUITY_RECORD FAR ContinuityTable[] =
{
	{1800, 0x7FFFFFFFL},	/* no continuity */
	{1400, 0x7FFFFFFFL},
	{1380, 0x7FFFFFFFL},
	{1357, 0x7FFFFFFFL},
	{1300, 0x7FFFFFFFL},
	{1200, 0x7FFFFFFFL},
	{1100, 0x7FFFFFFFL},
	{1000, 0x7FFFFFFFL},
	{ 900, 0x7FFFFFFFL},
	{ 800, 0x7FFFFFFFL},
	{ 700, 0x7FFFFFFFL},
	{ 600, 0x7FFFFFFFL},
	{ 500, 0x7FFFFFFFL},
	{ 400, 0x7FFFFFFFL},
	{ 300, 0x7FFFFFFFL},
	{ 200, 0x7FFFFFFFL},
	{ 150, 0x7FFFFFFFL},
	{ 100, 0x7FFFFFFFL},
	{ 50, 0x7FFFFFFFL},
	{ 10,  0x7FFFFFFFL},
	{0, 0x7FFFFFFFL}  /* full continuity */
};


LOCAL long ApproxVectorLength (LPPOINT ptlA,LPPOINT ptlB);
LOCAL void fit_piece (LPCPoly lpPoly, int start, int nSamples,
		LPPOINT SectionDirection/*[2]*/, coordi FAR *lpPathLength, LPPOINT BezList/*[4]*/);
LOCAL coordf GetError (int* pError_i, LPCPoly lpPoly, int start,int nSamples, coordi FAR *lpPathLength,LPPOINT BezList/*[4]*/);
LOCAL BOOL fit_continuous_section (LPCPoly lpPoly, int start, int nSamples, LPPOINT SectionDirection/*[2]*/, LPCPoly lpBezPoly, coordi FAR *lpPathLength, coordf Tolerance);
LOCAL BOOL generate_normal_bezier_list(LPCPoly lpPoly, UINT *lpDiscontinuityList, UINT nDiscontinuities,LPCPoly lpBezPoly, coordf Tolerance);
LOCAL void cartesian_to_polar (LPPOINT Origin,POINT_DUO *lpPoint);
LOCAL UINT relative_interior_angle (POLAR_POINT P0, POLAR_POINT P1);
LOCAL UINT identify_discontinuities(LPCPoly lpPoly,FIXED_REAL DiscontinuityRatio,
		UINT DiscontinuityAngle, UINT FAR *lpDiscontinuityList);
LOCAL void BezierEvaluate(coordf t, LPPOINT BezPoints, UINT Order, LPDPOINT lpOutPoint);
LOCAL void BernsteinValues (coordf *pBernstein_vals, coordf t);
 




//************************************************************************
//		BezCompressPoly
// PARAMETERS:
//			LPCPoly lpPoly-		The input polygon (assumed to be all lines)
//			int iContinuity-	??
//			int Tolerance-		??
// DESCRIPTION:
//			Compresses the given poly list into beziers.
//			It is up to the caller to delete the returned CPoly!
// RETURNS:
//			The bezier based polygon or NULL for error 
//************************************************************************
LPCPoly BezCompressPoly(LPCPoly lpPoly, int iContinuity, int Tolerance)
//************************************************************************
{
	LPCPoly 	lpBezPoly = NULL;
	UINT		Angle,nSamples;
	UINT FAR*   lpDiscontinuityList = NULL;
	UINT		nDiscontinuities, nMaxBezierPoints;
	FIXED_REAL 	Ratio;
	POINT 		pt1, pt2;
	BOOL 		bAdded = FALSE;

	iContinuity = Bound(iContinuity, 0, MAX_CONTINUITY_INDEX);
	nSamples = lpPoly->NumPoints();
	if (!nSamples || nSamples > MAX_BUFFER_SIZE)
		goto ExitError;
	lpPoly->GetPoint(0, &pt1);
	lpPoly->NextPoint(&pt2);
	
	Ratio = ContinuityTable[iContinuity].DiscontinuityRatio;
	Angle = ContinuityTable[iContinuity].DiscontinuityAngle;
	nMaxBezierPoints = (nSamples - 1) * 3 + 1;
	if (nMaxBezierPoints > MAX_BUFFER_SIZE)
		nMaxBezierPoints = MAX_BUFFER_SIZE;

	
	// get the new poly 
	lpBezPoly = new CPoly;
	if (!lpBezPoly)
		goto ExitError;
	lpBezPoly->ClosePoly(lpPoly->IsClosed());
	if (iContinuity == 0 || nSamples < 5 || (nSamples == 2 && PtEq(pt1, pt2)))
	{
		if (!lpPoly->Copy(lpBezPoly))
			goto ExitError;
		return (lpBezPoly);
	}
	
	// if its closed add first point to end
	if(lpPoly->IsClosed())
	{
		lpPoly->LastPoint(&pt2);
		if (!PtEq(pt1, pt2))
		{
			lpPoly->AddPoint(&pt1);
			bAdded = TRUE;
		}
	}
	nSamples = lpPoly->NumPoints();
	
	lpDiscontinuityList = (UINT *)Alloc(sizeof(UINT) * nSamples);
	if (!lpDiscontinuityList)
		goto ExitError;

	
	if (iContinuity == MAX_CONTINUITY_INDEX)
		nDiscontinuities = 0;
	else
	{
		nDiscontinuities = identify_discontinuities(lpPoly, Ratio, 
							Angle, lpDiscontinuityList);
	}
	if (!generate_normal_bezier_list(lpPoly, 
				lpDiscontinuityList, nDiscontinuities, lpBezPoly, Tolerance))
		goto ExitError;
	
	
	if(lpDiscontinuityList)
		FreeUp((LPTR)lpDiscontinuityList);
	if (bAdded)
	{
	 	lpPoly->LastPoint();
		lpPoly->DeletePoint();
	}
	return (lpBezPoly);
	
ExitError:
	if(lpDiscontinuityList)
		FreeUp((LPTR)lpDiscontinuityList);
	if (lpBezPoly)
		delete lpBezPoly;
	if (bAdded)
	{
	 	lpPoly->LastPoint();
		lpPoly->DeletePoint();
	}
	return(NULL);
}



/* ***************************** Local Functions **************************** */
LOCAL long ApproxVectorLength (LPPOINT ptlA,LPPOINT ptlB)
{
   long lDx;
   long lDy;

   if ((lDy = ptlA->y - ptlB->y) < 0) {
	  lDy = -lDy;
   }
   if ((lDx = ptlA->x - ptlB->x) < 0) {
	  lDx = -lDx;
   }
   if (lDx > lDy) {
	  return((long)(lDx + (lDy / 2)));
   }
   else {
	  return((long)(lDy + (lDx / 2)));
   }
}


LOCAL void fit_piece (LPCPoly lpPoly, int start, int nSamples,
		LPPOINT SectionDirection/*[2]*/, coordi FAR *lpPathLength, LPPOINT BezList/*[4]*/)
{
	UINT	i, j;

	POINT 	newSamples [5];
	coordf	t;
	coordf	total;
	coordi	newPath [4];
	coordi	FAR *lpPath;
	int		nTotal;
	FLOAT_REAL M [2][3];
	BYTE	RowBuff[3 * sizeof(FLOAT_REAL)];
	DPOINT V0, Vn, V1_m_V0, Vn_m_1_m_Vn;
	POINT pt1;
	CPoly NewPoly;
	LPCPoly lpUsePoly;
	
	lpPoly->GetPoint(start,&pt1);
	SETPT(V0, pt1);
	lpPoly->GetPoint(start+nSamples - 1,&pt1);
	SETPT(Vn, pt1);
	SETPT(V1_m_V0, SectionDirection[0]);
	Vn_m_1_m_Vn.x = -SectionDirection [1].x;
	Vn_m_1_m_Vn.y = -SectionDirection [1].y;

	coordf D0dotD0 = DDotProduct(&V1_m_V0,&V1_m_V0);
	coordf D1dotD1 = DDotProduct(&Vn_m_1_m_Vn, &Vn_m_1_m_Vn);
	coordf D0dotD1 = DDotProduct(&V1_m_V0, &Vn_m_1_m_Vn);
	for (i = 0; i < 2; ++i)
		for (j = 0; j < 3; ++j)
			M[i][j] = 0.0;


	if (nSamples == 3)
	{
		lpPoly->GetPoint(start, &newSamples[0]);
		lpPoly->NextPoint(&newSamples[2]);
		lpPoly->NextPoint(&newSamples[4]);
		newSamples [1].x = (newSamples [0].x + newSamples [2].x) / 2;
		newSamples [1].y = (newSamples [0].y + newSamples [2].y) / 2;
		newSamples [3].x = (newSamples [2].x + newSamples [4].x) / 2;
		newSamples [3].y = (newSamples [2].y + newSamples [4].y) / 2;
		for (i = 0; i < 4; i++)
			newPath[i] = ApproxVectorLength(&newSamples[i],&newSamples[i+1]);
		for (i=0; i <5; i++)
			NewPoly.AddPoint(&newSamples[i]);
		lpUsePoly = &NewPoly;
		lpPath = newPath;
		nTotal = 4;
		start = 0;
	}
	else
	{
		lpUsePoly = lpPoly;
	  	lpPath = lpPathLength;
	  	nTotal = nSamples - 1;
	}
	
	for (i = 0, total = 0.0; i < nTotal; i++)
		total += (coordf) (lpPath [i]);

	lpUsePoly->GetPoint(start+1, &pt1);
	for (i = 1, t = 0.0; i < nTotal; i++)
	{
		coordf  Bernstein [4];
		DPOINT D_i;

		t += (coordf)lpPath [i - 1] / total;
		BernsteinValues (Bernstein, t);
		M[0][0] += Bernstein [1] * Bernstein [1] * D0dotD0;
		M[0][1] += Bernstein [1] * Bernstein [2] * D0dotD1;
		M[1][1] += Bernstein [2] * Bernstein [2] * D1dotD1;
		D_i.x = V0.x*(Bernstein[0]+Bernstein[1])+Vn.x*(Bernstein[2]+Bernstein[3]);
		D_i.y = V0.y*(Bernstein[0]+Bernstein[1])+Vn.y*(Bernstein[2]+Bernstein[3]);
		D_i.x -= pt1.x;
		D_i.y -= pt1.y;
		M[0][2] -= Bernstein [1] * DDotProduct(&D_i,&V1_m_V0);
		M[1][2] -= Bernstein [2] * DDotProduct(&D_i,&Vn_m_1_m_Vn);
		lpUsePoly->NextPoint(&pt1);
	}
	M[1][0] = M[0][1];
	if (M[0][0] == 0.0)
	{
		memmove(RowBuff, (LPSTR)M[0], (3 * sizeof(FLOAT_REAL)));
		memmove((LPSTR)M[0], (LPSTR)M[1], (3 * sizeof(FLOAT_REAL)));
		memmove((LPSTR)M[1], RowBuff, (3 * sizeof(FLOAT_REAL)));
	}
	coordf Determinant = M[1][1] * M[0][0] - M[1][0] * M[0][1];

	if (NEAR_ZERO(Determinant) || NEAR_ZERO(M[0][0]))
	{
		BezList [1].x =  (int)((V0.x * (2.0 / 3.0)) + (Vn.x * (1.0 / 3.0)));
		BezList [1].x =  (int)((V0.y * (2.0 / 3.0)) + (Vn.y * (1.0 / 3.0)));
		BezList [2].x =  (int)((V0.x * (1.0 / 3.0)) + (Vn.x * (2.0 / 3.0)));
		BezList [2].y =  (int)((V0.y * (1.0 / 3.0)) + (Vn.y * (2.0 / 3.0)));
	}																	 
	else
	{
		coordf Lamda2 = (M[1][2] * M[0][0] - M[1][0] * M[0][2]) / Determinant;
		coordf Lamda1 = (M[0][2] - (Lamda2 * M[0][1])) / M[0][0];
		lpUsePoly->GetPoint(start, &BezList[1]);
		if (Lamda1 >= 0.0)
		{
			BezList [1].x += (int)(V1_m_V0.x * Lamda1);
			BezList [1].y += (int)(V1_m_V0.y * Lamda1);
		}
		lpUsePoly->GetPoint(start+nTotal, &BezList[2]);
		
		if (Lamda2 >= 0.0)
		{
			BezList[2].x += int(Vn_m_1_m_Vn.x * Lamda2);
			BezList[2].y += int(Vn_m_1_m_Vn.y * Lamda2);
		}
	}
	lpUsePoly->GetPoint(start+nTotal, &BezList[3]);
}


LOCAL coordf GetError (int* pError_i, LPCPoly lpPoly, int start,int nSamples, coordi FAR *lpPathLength,LPPOINT BezList/*[4]*/)
{
	UINT	i;
	coordf  t;
	coordf  total;
	coordf  LargestError;
	POINT pt;

	for (i = 0, total = 0.0; i < (nSamples - 1); i++)
	  total += (coordf) (lpPathLength [i]);

	LargestError = -1.0;

	t = (coordf)lpPathLength[0] / total;
	lpPoly->GetPoint(start+2, &pt);
	for (i = 2; i < nSamples - 2; i++)
	{
		DPOINT D, dpt1;
		coordf  Error_D;

		t += (coordf)lpPathLength [i - 1] / total;
		BezierEvaluate(t, BezList, 4, &dpt1);
		D.x = pt.x - dpt1.x;
		D.y = pt.y - dpt1.y;
		Error_D = sqrt(D.x * D.x + D.y * D.y);
		if (Error_D > LargestError)
		{
			LargestError = Error_D;
			*pError_i = i;
		}
		lpPoly->NextPoint(&pt);
	}
	return LargestError;
}


//************************************************************************
//
// PARAMETERS:
//
// DESCRIPTION:
//   This function fits a piecewise Bezier curve to a continuous section
//   of the sample data.  Tangent vectors at section endpoints are
//   constrained by direction vectors in 'SectionDirection[2]'.
//
//   Direction vectors at section endpoints are given in the argument
//   list.  Direction vectors at interior knots pass through shape
//   points which immediately precede and follow the knot.
//
// RETURNS:
//	FALSE if there is an error.
//************************************************************************
LOCAL BOOL fit_continuous_section (LPCPoly lpPoly, int start, int nSamples, LPPOINT SectionDirection/*[2]*/, LPCPoly lpBezPoly, coordi FAR *lpPathLength, coordf Tolerance)
//************************************************************************
{

	UINT	nPieces = 1;
	coordf  LargestError;
	POINT BezList [4];
	POINT Dir [2];
	POINT pt1, pt2;


	lpPoly->GetPoint(start, &BezList [0]);

	if (nSamples == 2)
	{
		lpPoly->NextPoint(&BezList[3]);
		BezList [1].x = (coordi) (((2.0 / 3.0) * BezList[0].x) + ((1.0 / 3.0) * BezList[3].x));
		BezList [1].y = (coordi) (((2.0 / 3.0) * BezList[0].y) + ((1.0 / 3.0) * BezList[3].y));
		BezList [2].x = (coordi) (((1.0 / 3.0) * BezList[0].x) + ((2.0 / 3.0) * BezList[3].x));
		BezList [2].y = (coordi) (((1.0 / 3.0) * BezList[0].y) + ((2.0 / 3.0) * BezList[3].y));
	}
	else
	  	fit_piece(lpPoly,start,nSamples,SectionDirection,lpPathLength,BezList);

	if (nSamples > 7)
	{
	  	int Error_i;

	  	LargestError = GetError(&Error_i,lpPoly,start,nSamples,lpPathLength,BezList);

		if (LargestError > Tolerance)
		{
			Dir[0] = SectionDirection[0];
			lpPoly->GetPoint(start+Error_i-1, &pt1);
			lpPoly->NextPoint(); lpPoly->NextPoint(&pt2);
			PtSub(Dir[1], pt2, pt1);
			if (!fit_continuous_section (lpPoly, start,
			                                    Error_i + 1,
			                                    Dir,
			                                    lpBezPoly,
			                                    lpPathLength,
				                                    Tolerance))
				return(FALSE);
				
		   	Dir [0] = Dir [1];
		   	Dir [1] = SectionDirection [1];
		   	if (!fit_continuous_section (lpPoly, start + Error_i,
		                                      nSamples - Error_i,
		                                      Dir,
		                                      lpBezPoly,
		                                      lpPathLength + Error_i,
		                                      Tolerance))
				return(FALSE);
		}
		else if (!lpBezPoly->AddBezierLine(&BezList[1],&BezList[2],&BezList[3]))
			return(FALSE);
	}		
	else if (!lpBezPoly->AddBezierLine(&BezList[1],&BezList[2],&BezList[3]))
		return(FALSE);

	return(TRUE);
}




//************************************************************************
//
// PARAMETERS:
//
// DESCRIPTION:
//   Sample data is partitioned into continuous sections and passed
//   to function fit_continuous_section() for processing.  Direction
//   vectors for section endpoints are also computed and passed to
//   fit_continuous_section().
//
//   At the beginning of a section, direction vectors pass through first
//   and second sample points respectively.  At the end of a section,
//   direction vectors pass through the next-to-last and last points
//   respectively.
//
// RETURNS:
//		FALSE if there is an error.
//************************************************************************
LOCAL BOOL generate_normal_bezier_list(LPCPoly lpPoly, UINT *lpDiscontinuityList, UINT nDiscontinuities,LPCPoly lpBezPoly, coordf Tolerance)
//************************************************************************
{
	auto	BOOL	bContinuousClosed;
	auto	UINT	nPieces = 0;
	auto	UINT	nRemaining, iLoKnot, iHiKnot;
	auto 	UINT 	i, j;
	auto	POINT	Direction[2];
	coordi			*lpPathLength;
	POINT 	pt1, pt2;
	int nSamples;
	BOOL bRet;
	
	lpPoly->GetPoint(0,&pt1);
   	if (!lpBezPoly->AddPoint(&pt1))
		return(FALSE);
	nSamples = lpPoly->NumPoints();

	lpPathLength = (coordi *)Alloc((nSamples - 1) * sizeof (coordi));
	if (lpPathLength == NULL)
		return(FALSE);
	if (nDiscontinuities == 0)
	{
		lpPoly->GetPoint(1,&pt1);
		lpPoly->GetPoint(nSamples - 2,&pt2);
		Direction[0].x = pt1.x - pt2.x;
		Direction[0].y = pt1.y - pt2.y;
		Direction[1] = Direction[0];
		
		lpPoly->GetPoint(0,&pt1);
		lpPoly->NextPoint(&pt2);
		for (i = 0; i < nSamples - 1; i++)
		{
			lpPathLength[i] = ApproxVectorLength (&pt1, &pt2);
			pt1 = pt2;
			lpPoly->NextPoint(&pt2);
		}
		bRet = fit_continuous_section(lpPoly, 0, nSamples, Direction,lpBezPoly,lpPathLength,Tolerance);
	}
	else /* (nDiscontinuities > 1) */
	{
		if (lpDiscontinuityList[0] != 0)
		{
		  	bContinuousClosed = TRUE;
			lpPoly->GetPoint(1,&pt1);
			lpPoly->GetPoint(nSamples-2,&pt2);
		}
		else
		{
		  	bContinuousClosed = FALSE;
			lpPoly->GetPoint(1,&pt1);
			lpPoly->PreviousPoint(&pt2);
		  	++lpDiscontinuityList;
		  	--nDiscontinuities;
		}
		Direction[0].x = pt1.x - pt2.x;
		Direction[0].y = pt1.y - pt2.y;
		iHiKnot = *lpDiscontinuityList++;
		lpPoly->GetPoint(iHiKnot,&pt1);
		lpPoly->PreviousPoint(&pt2);
		Direction[1].x = pt1.x - pt2.x;
		Direction[1].y = pt1.y - pt2.y;

		lpPoly->GetPoint(0,&pt1);
		lpPoly->NextPoint(&pt2);
		for (i = 0; i < iHiKnot; i++)
		{
		 	lpPathLength[i]=ApproxVectorLength(&pt1, &pt2);
			pt1 = pt2;
			lpPoly->NextPoint(&pt2);
		}
		if (!(bRet = fit_continuous_section (lpPoly, 0/*from*/, (iHiKnot+1) /*num*/,
		   				Direction, lpBezPoly, lpPathLength, Tolerance)))
		   goto Done;
		nRemaining = nDiscontinuities - 1;
		while (nRemaining > 0)
		{
			iLoKnot = iHiKnot;
			iHiKnot = *lpDiscontinuityList++;
			lpPoly->GetPoint(iHiKnot,&pt1);
			lpPoly->NextPoint(&pt2);
			Direction[0].x = pt1.x - pt2.x;
			Direction[0].y = pt1.y - pt2.y;

			lpPoly->GetPoint(iLoKnot,&pt1);
			lpPoly->NextPoint(&pt2);
			Direction[0].x = pt2.x - pt1.x;
			Direction[0].y = pt2.y - pt1.y;

			for (i = 0, j = iLoKnot; i < (iHiKnot - iLoKnot); i++, j++)
			{
				lpPathLength [i] = ApproxVectorLength (&pt1,&pt2);
				pt1 = pt2;
				lpPoly->NextPoint(&pt2);
			}

			if (!fit_continuous_section(lpPoly,iLoKnot,	(iHiKnot - iLoKnot + 1),
				 			Direction,lpBezPoly, lpPathLength, Tolerance))
		   		goto Done;
			--nRemaining;
		}
		if (bContinuousClosed)
		{
			iLoKnot = iHiKnot;
			lpPoly->GetPoint(iLoKnot+1,&pt1);
			lpPoly->PreviousPoint(&pt2);
			Direction[0].x = pt1.x - pt2.x;
			Direction[0].y = pt1.y - pt2.y;
										
			lpPoly->GetPoint(1,&pt1);
			lpPoly->GetPoint(nSamples - 2,&pt2);
			Direction[1].x = pt1.x - pt2.x;
			Direction[1].y = pt1.y - pt2.y;

			lpPoly->GetPoint(iLoKnot,&pt1);
			lpPoly->NextPoint(&pt2);
			for (i = 0, j = iLoKnot; i < (nSamples - iLoKnot - 1); i++, j++)
			{
			 	lpPathLength [i] = ApproxVectorLength (&pt1, &pt2);
				pt1 = pt2;
				lpPoly->NextPoint(&pt2);
			}

			bRet = fit_continuous_section(lpPoly, iLoKnot, (nSamples - iLoKnot), Direction,
							lpBezPoly, lpPathLength, Tolerance);
		}
	}
Done:
	if (lpPathLength)	
		FreeUp((LPTR)lpPathLength);
	return (bRet);
}

LOCAL void cartesian_to_polar (LPPOINT Origin,POINT_DUO *lpPoint)
{
   auto	coordi		  dx;
   auto	coordi		  dy;
   auto	coordi		  Large;
   auto	coordi		  Small;

//   auto	DOUBLE_INT	  r_squared;

   dx = lpPoint->cartesian.x - Origin->x;
   dy = lpPoint->cartesian.y - Origin->y;
   lpPoint->polar.theta = (int)((atan2(dy, dx)*3600)/TWOPI);

   dx = dx > 0 ? dx : -dx;
   dy = dy > 0 ? dy : -dy;

   if (dx > dy)
   {
	 Large = dx;
	 Small = dy;
   }
   else
   {
	 Large = dy;
	 Small = dx;
   }

   lpPoint->polar.r = Large + (Small / 2) - (Small / 16);
//   r_squared = ((DOUBLE_INT)dx * dx) + ((DOUBLE_INT)dy * dy);
//   lpPoint->polar.r = (int)
//	 ((SquareRoot (r_squared) + FXR_ONE_HALF) / FXR_PRECISION_FACTOR);
}

LOCAL UINT relative_interior_angle (POLAR_POINT P0, POLAR_POINT P1)
{
   /*
   Relative interior angle is the positive angle less than or equal
   to 180 degrees made by the two polar vectors 'P0' and 'P1'.  The
   return value is in units of tenth-degree.
   */
   auto	UINT RelativePositive;

   RelativePositive = ((P1.theta + 3600) - P0.theta) % 3600;
   return (RelativePositive > 1800) ?
	 3600 - RelativePositive : RelativePositive;
}


//************************************************************************
// DESCRIPTION:
//   Discontinuity at a particular vertex is a function of its two
//   adjoining edges.  If the acute angle between the two edges is less
//   than 'DiscontinuityAngle' or if the length ratio between them
//   is greater than 'DiscontinuityRatio', the vertex is identified as
//   a curve discontinuity.  Loosely, these two conditions identify
//   "rapid" changes in direction or magnitude of the curve's tangent
//   vector.
//
//   If the shape polygon closes on itself, the first and last vertex
//   are treated as one and are tested in the usual manner for
//   discontinuity.  Note that it is possible for a closed polygon to
//   have zero discontinuities or to have an initial discontinuity not
//   coincident with the polygon's terminal points.  If the
//   first entry in the discontinuity list is not zero, the polygon is
//   necessarily closed and continuous at terminal points.
//************************************************************************
LOCAL UINT identify_discontinuities(LPCPoly lpPoly,FIXED_REAL DiscontinuityRatio,
  				UINT DiscontinuityAngle, UINT FAR *lpDiscontinuityList)
//************************************************************************
{
	auto	int				 Angle;
	auto	unsigned int		nDiscontinuities;
	auto	unsigned int		iCentral;
	auto	unsigned int		iNext;
	auto	unsigned int		iLast;
	auto	POINT_DUO		   S0;
	auto	POINT_DUO		   S1;
	auto	POINT_DUO		   S2;
	auto	double			  Ratio;
	int nSamples = lpPoly->NumPoints();
	POINT pt1, pt2;

	iLast = nSamples - 1;
	lpPoly->GetPoint(0, &pt1);
	lpPoly->GetPoint(iLast, &pt2);
	if (PtEq(pt1, pt2))
	{
		lpDiscontinuityList[0] = ~0;
		nDiscontinuities = 0;
		lpPoly->PreviousPoint(&S0.cartesian);
		S1.cartesian = pt1;
		iCentral = 0;
		iNext = 1;
	}
	else
	{
		lpDiscontinuityList[0] = 0;
		nDiscontinuities = 1;
		S0.cartesian = pt1;
		lpPoly->GetPoint(1, &S1.cartesian);
		iCentral = 1;
		iNext = 2;
	}
	lpPoly->GetPoint(iNext, &pt1);
	for (;iNext<nSamples; iNext++)
	{
		cartesian_to_polar(&S1.cartesian, &S0);
		S2.cartesian = pt1;
		cartesian_to_polar(&S1.cartesian, &S2);
		Ratio =
		 (S2.polar.r > S0.polar.r) ?
		 ((double)S2.polar.r * FXR_PRECISION_FACTOR) / S0.polar.r :
		 ((double)S0.polar.r * FXR_PRECISION_FACTOR) / S2.polar.r;
		Angle = relative_interior_angle(S2.polar, S0.polar);
		if (Angle <= (int)DiscontinuityAngle || (FIXED_REAL)Ratio >= DiscontinuityRatio)
		   lpDiscontinuityList[nDiscontinuities++] = iCentral;
		S0 = S1;
		S1 = S2;
		++iCentral;
		lpPoly->NextPoint(&pt1);
	}
	if (lpDiscontinuityList[0] == 0)
		lpDiscontinuityList[nDiscontinuities++] = iLast;
	return (nDiscontinuities);
}

LOCAL void BezierEvaluate(coordf t, LPPOINT BezPoints, UINT Order, LPDPOINT lpOutPoint)
{
	coordf  fact;
	coordf  t1;
	DPOINT EvalPt;
	DPOINT Temp;
	int     n_choose_i;
	UINT    i;

	t1 = 1.0 - t;
	fact = 1.0;

	n_choose_i = 1;

	SETPT(Temp, BezPoints[0]);
	EvalPt.x = Temp.x * t1;
	EvalPt.y = Temp.y * t1;

	for (i = 1; i < Order - 1; i++)
	{
		SETPT(Temp, BezPoints[i]);
		fact *= t;
		n_choose_i = n_choose_i * (Order - i) / i;
		EvalPt.x = (EvalPt.x + Temp.x * (fact * n_choose_i))*t1;
		EvalPt.y = (EvalPt.y + Temp.y * (fact * n_choose_i))*t1;
	}
	SETPT(Temp, BezPoints[Order - 1]);
	EvalPt.x = EvalPt.x + Temp.x * (fact * t);
	EvalPt.y = EvalPt.y + Temp.y * (fact * t);
	
	*lpOutPoint = EvalPt;
}


LOCAL void BernsteinValues (coordf *pBernstein_vals, coordf t)
{
   coordf t1;
   coordf t2;
   coordf t1_2;

   t2 = t * t;
   t1 = 1.0 - t;
   t1_2 = t1 * t1;

   pBernstein_vals [0] = t1_2 * t1;
   pBernstein_vals [1] = 3.0 * t * t1_2;
   pBernstein_vals [2] = 3.0 * t2 * t1;
   pBernstein_vals [3] = t2 * t;
}
