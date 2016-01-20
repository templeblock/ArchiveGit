/*
// $Header: /PM8/App/WARPFLD.CPP 1     3/03/99 6:13p Gbeddow $
//
// $Workfile: WARPFLD.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WARPFLD.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     2/26/99 5:43p Gbeddow
// added some asserts in an effort to track down bug #9 but they weren't
// triggered, leaving the bug unresolved - it seems that some of the
// readymade projects have bad data
// 
//    Rev 1.0   14 Aug 1997 15:27:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:58   Fred
// Initial revision.
// 
//    Rev 1.10   27 May 1997 11:08:28   Fred
// Uses PI definitions in utils.h
// 
//    Rev 1.9   12 May 1997 11:08:08   Fred
// Changes to prevent overflow
// 
//    Rev 1.8   17 Apr 1997 15:35:24   Fred
// Rotated fill patterns
// 
//    Rev 1.7   19 Mar 1997 14:37:24   Fred
// Removed ASSERT
// 
//    Rev 1.6   18 Mar 1997 09:30:10   Fred
// More warp refinements
// 
//    Rev 1.5   17 Mar 1997 17:23:56   Fred
// New equations for BezierX() and BezierY() from Numerical Recipies
// 
//    Rev 1.4   12 Mar 1997 14:54:40   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.3   07 Feb 1997 08:56:46   Fred
// Added provisional code to not warp points outside of warp field
// 
//    Rev 1.2   16 Jan 1997 14:43:24   Fred
// Fixed bug in warp calculations
// 
//    Rev 1.1   16 Jan 1997 09:07:52   Fred
// Improved warp speed
// 
//    Rev 1.0   14 Mar 1996 13:48:44   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:10:30   FRED
// Initial revision.
// 
//    Rev 1.11   23 Jun 1995 11:39:24   JAY
// Got rid of some more overflows.
// 
//    Rev 1.10   23 Jun 1995 09:15:44   JAY
// More overflow (and other) protection.
// 
//    Rev 1.9   25 May 1995 13:04:28   FRED
// Simple optimizations.
// 
// 
//    Rev 1.8   13 May 1995 11:14:26   JAY
//  
// 
//    Rev 1.7   11 May 1995 09:37:12   JAY
// Changed CreateStruct to external class.
// 
//    Rev 1.6   09 May 1995 13:14:06   FRED
// Non linear warp field support.
// 
// 
//    Rev 1.5   04 May 1995 17:16:04   JAY
// Fixed-point. Drawing objects.
// 
//    Rev 1.4   28 Apr 1995 17:38:18   FRED
//  
// 
//    Rev 1.3   28 Apr 1995 13:49:12   FRED
//  
// 
//    Rev 1.2   27 Apr 1995 13:22:10   FRED
//  
// 
//    Rev 1.1   25 Apr 1995 16:02:26   FRED
// 
//    Rev 1.0   25 Apr 1995 10:33:44   FRED
// Initial revision.
*/

#include "stdafx.h"

#include <math.h>

#include "warpfld.h"
#include "outpath.h"
#include "pagedefs.h"
#include "cfixed.h"
#include "utils.h"

/////////////////////////////////////////////////////////////////////////////
// CWarpFieldEdge

CWarpFieldEdge::CWarpFieldEdge(void)
{
}

CWarpFieldEdge::~CWarpFieldEdge(void)
{
}

void CWarpFieldEdge::InitLine(long AX, long AY, long BX, long BY)
{
	Type(Line);
	m_Line.AX = AX;
	m_Line.AY = AY;
	m_Line.BX = BX;
	m_Line.BY = BY;
}

void CWarpFieldEdge::InitSpline(long AX, long AY, long BX, long BY, long CX, long CY)
{
	Type(Spline);
	m_Spline.AX = AX;
	m_Spline.AY = AY;
	m_Spline.BX = BX;
	m_Spline.BY = BY;
	m_Spline.CX = CX;
	m_Spline.CY = CY;
}

void CWarpFieldEdge::InitBezier(long AX, long AY, long BX, long BY, long CX, long CY, long DX, long DY)
{
	Type(Bezier);
	m_Bezier.AX = AX;
	m_Bezier.AY = AY;
	m_Bezier.BX = BX;
	m_Bezier.BY = BY;
	m_Bezier.CX = CX;
	m_Bezier.CY = CY;
	m_Bezier.DX = DX;
	m_Bezier.DY = DY;
}

void CWarpFieldEdge::FromData(const CWarpFieldEdgeData& Data)
{
	Type((EdgeType)Data.m_nType);
	m_Bezier.AX = Data.m_Bezier.AX;
	m_Bezier.AY = Data.m_Bezier.AY;
	m_Bezier.BX = Data.m_Bezier.BX;
	m_Bezier.BY = Data.m_Bezier.BY;
	m_Bezier.CX = Data.m_Bezier.CX;
	m_Bezier.CY = Data.m_Bezier.CY;
	m_Bezier.DX = Data.m_Bezier.DX;
	m_Bezier.DY = Data.m_Bezier.DY;
}

void CWarpFieldEdge::ToData(CWarpFieldEdgeData& Data)
{
	ASSERT(Type()==Line || Type()==Spline || Type()==Bezier);
	Data.m_nType = (SHORT)Type();
	Data.m_Bezier.AX = m_Bezier.AX;
	Data.m_Bezier.AY = m_Bezier.AY;
	Data.m_Bezier.BX = m_Bezier.BX;
	Data.m_Bezier.BY = m_Bezier.BY;
	Data.m_Bezier.CX = m_Bezier.CX;
	Data.m_Bezier.CY = m_Bezier.CY;
	Data.m_Bezier.DX = m_Bezier.DX;
	Data.m_Bezier.DY = m_Bezier.DY;
}

void CWarpFieldEdge::Transform(COutlinePathMatrix* pMatrix)
{
	ASSERT(pMatrix != NULL);
	
	switch (Type())
	{
		case Line:
		{
			pMatrix->Transform(&m_Line.AX, &m_Line.AY);
			pMatrix->Transform(&m_Line.BX, &m_Line.BY);
			break;
		}

		case Spline:
		{
			pMatrix->Transform(&m_Spline.AX, &m_Spline.AY);
			pMatrix->Transform(&m_Spline.BX, &m_Spline.BY);
			pMatrix->Transform(&m_Spline.CX, &m_Spline.CY);
			break;
		}

		case Bezier:
		{
			pMatrix->Transform(&m_Bezier.AX, &m_Bezier.AY);
			pMatrix->Transform(&m_Bezier.BX, &m_Bezier.BY);
			pMatrix->Transform(&m_Bezier.CX, &m_Bezier.CY);
			pMatrix->Transform(&m_Bezier.DX, &m_Bezier.DY);
			break;
		}
		
		default:
		{
			ASSERT(FALSE);
			break;
		}
	}
}

void CWarpFieldEdge::Path(COutlinePath* pPath, BOOL fMoveTo /*=FALSE*/)
{
	ASSERT(pPath != NULL);
	
	switch (Type())
	{
		case Line:
		{
			if (fMoveTo)
			{
				pPath->MoveTo(m_Line.AX, m_Line.AY);
			}
			pPath->LineTo(m_Line.BX, m_Line.BY);
			break;
		}
	
		case Spline:
		{
			if (fMoveTo)
			{
				pPath->MoveTo(m_Spline.AX, m_Spline.AY);
			}
			pPath->SplineTo(m_Spline.BX, m_Spline.BY, m_Spline.CX, m_Spline.CY);
			break;
		}
	
		case Bezier:
		{
			if (fMoveTo)
			{
				pPath->MoveTo(m_Bezier.AX, m_Bezier.AY);
			}
			pPath->BezierTo(m_Bezier.BX, m_Bezier.BY, m_Bezier.CX, m_Bezier.CY, m_Bezier.DX, m_Bezier.DY);
			break;
		}
			
		default:
		{
			ASSERT(FALSE);
			break;
		}
	}
}

long CWarpFieldEdge::X(long Y)
{
	switch (Type())
	{
		case Line:
		{
			if ((m_Line.AX == m_Line.BX)
			 || (m_Line.AY == m_Line.BY))
			{
				break;
			}
			
			return MulDivFixed(Y-m_Line.AY, m_Line.BX-m_Line.AX, m_Line.BY-m_Line.AY) + m_Line.AX;
		}

		case Spline:
		{
			return SplineX(Y);
			break;
		}

		case Bezier:
		{
			return BezierX(Y);
		}
		
		default:
		{
			ASSERT(FALSE);
			break;
		}
	}
	
	return m_Line.AX;
}

long CWarpFieldEdge::Y(long X)
{
	switch (Type())
	{
		case Line:
		{
			if ((m_Line.AX == m_Line.BX)
			 || (m_Line.AY == m_Line.BY))
			{
				break;
			}
			
			return MulDivFixed(X-m_Line.AX, m_Line.BY-m_Line.AY, m_Line.BX-m_Line.AX) + m_Line.AY;
		}

		case Spline:
		{
			return SplineY(X);
		}

		case Bezier:
		{
			return BezierY(X);
		}
		
		default:
		{
			ASSERT(FALSE);
			break;
		}
	}
	
	return m_Line.AY;
}

long CWarpFieldEdge::SplineX(long Y)
{
	ASSERT(FALSE);

	long AX = m_Bezier.AX;
	long AY = m_Bezier.AY;
	long BX = m_Bezier.BX;
	long BY = m_Bezier.BY;
	long CX = m_Bezier.CX;
	long CY = m_Bezier.CY;
	
	long M0X, M0Y, M1X, M1Y;
	BOOL fFound = FALSE;
	
	if (AX > CX)
	{
		M0X = AX;
		M0Y = AY;
		AX = CX;
		AY = CY;
		CX = M0X;
		CY = M0Y;
	}
	
	if (Y < AY)
	{
		return AX;
	}
	
	if (Y > CY)
	{
		return CX;
	}
	
	while (!fFound)
	{
		// Compute the current midpoint.
		
		M0X = MidPointFixed(AX, BX+1);
		M0Y = MidPointFixed(AY, BY+1);
		
		M1X = MidPointFixed(BX, CX+1);
		M1Y = MidPointFixed(BY, CY+1);
		
		// Reuse BX, BY
		
		BX = MidPointFixed(M0X, M1X+1);
		BY = MidPointFixed(M0Y, M1Y+1);
		
		// Compute the delta from the desired Y.
		
		long Delta = BY-Y;
		if (Delta < 0) Delta = -Delta;
		
		if (Delta < (1 << 2))
		{
			fFound = TRUE;
		}
		else
		{
			// Search the left or right subcurve.
			
			if (Y < BY)
			{
				CX = BX;
				CY = BY;
				
				BX = M0X;
				BY = M0Y;
			}
			else
			{
				AX = BX;
				AY = BY;
				
				BX = M1X;
				BY = M1Y;
			}
		}
	};
	
	return BX;
}

long CWarpFieldEdge::SplineY(long X)
{
	ASSERT(FALSE);

	long AX = m_Bezier.AX;
	long AY = m_Bezier.AY;
	long BX = m_Bezier.BX;
	long BY = m_Bezier.BY;
	long CX = m_Bezier.CX;
	long CY = m_Bezier.CY;
	
	long M0X, M0Y, M1X, M1Y;
	BOOL fFound = FALSE;
	
	if (AX > CX)
	{
		M0X = AX;
		M0Y = AY;
		AX = CX;
		AY = CY;
		CX = M0X;
		CY = M0Y;
	}
	
	if (X < AX)
	{
		return AY;
	}
	
	if (X > CX)
	{
		return CY;
	}
	
	while (!fFound)
	{
		// Compute the current midpoint.
		
		M0X = MidPointFixed(AX, BX+1);
		M0Y = MidPointFixed(AY, BY+1);
		
		M1X = MidPointFixed(BX, CX+1);
		M1Y = MidPointFixed(BY, CY+1);
		
		// Reuse BX, BY
		
		BX = MidPointFixed(M0X, M1X+1);
		BY = MidPointFixed(M0Y, M1Y+1);
		
		// Compute the delta from the desired X.
		
		long Delta = BX-X;
		if (Delta < 0) Delta = -Delta;
		
		if (Delta < (1 << 2))
		{
			fFound = TRUE;
		}
		else
		{
			// Search the left or right subcurve.
			
			if (X < BX)
			{
				CX = BX;
				CY = BY;
				
				BX = M0X;
				BY = M0Y;
			}
			else
			{
				AX = BX;
				AY = BY;
				
				BX = M1X;
				BY = M1Y;
			}
		}
	};
	
	return BY;
}

long CWarpFieldEdge::BezierX(long Y)
{
	double T;

	// Clip Y coordinate to defined warp field range.
	// Allowing Y to go outside of this range can cause
	// can cause roots to be found which are not what the
	// code expects.
	if (m_Bezier.AY < m_Bezier.DY)
	{
		if (Y < m_Bezier.AY)
		{
			Y = m_Bezier.AY;
		}
		if (Y > m_Bezier.DY)
		{
			Y = m_Bezier.DY;
		}
	}
	else
	{
		if (Y > m_Bezier.AY)
		{
			Y = m_Bezier.AY;
		}
		if (Y < m_Bezier.DY)
		{
			Y = m_Bezier.DY;
		}
	}

	double YB = MakeDouble(m_Bezier.BY-m_Bezier.AY);
	double YC = MakeDouble(m_Bezier.CY-m_Bezier.AY);
	double YD = MakeDouble(m_Bezier.DY-m_Bezier.AY);
	double YY = MakeDouble(Y-m_Bezier.AY);

	double BA = 3*(YB-YC)+YD;
	double BB = 3*(YC-2*YB);
	double BC = 3*YB;
	double BD = -YY;

	if (fabs(BA) < 1e-5)
	{
		// Speed along axis is aproximately constant.
		T = (fabs(YD) >= 1e-5) ? YY/YD : YD/2;
	}
	else
	{
		double A = BB/BA;
		double B = BC/BA;
		double C = BD/BA;

		double Q = (A*A-3*B)/9;
		double R = (A*(2*A*A-9*B)+27*C)/54;
		double P = R*R-Q*Q*Q;

		double ADIV3 = A/3;

		if (P < 0.0)
		{
			// Cubic has three real roots.
			double THETA = acos(R/pow(Q, 3.0/2.0));
			double THETADIV3 = THETA/3;
			double SQRTQ = -2*sqrt(Q);
			double S1 = SQRTQ*cos(THETADIV3)-ADIV3;
			double S2 = SQRTQ*cos(THETADIV3+PI*(2.0/3.0))-ADIV3;
			double S3 = SQRTQ*cos(THETADIV3-PI*(2.0/3.0))-ADIV3;

			T = S1;
			if (fabs(S2-0.5) < fabs(T-0.5))
			{
				T = S2;
			}
			if (fabs(S3-0.5) < fabs(T-0.5))
			{
				T = S3;
			}
		}
		else
		{
			// Cubic has one real root.
			double CA = pow(fabs(R)+sqrt(P), 1.0/3.0);
			if (R > 0)
			{
				CA = -CA;
			}
			double CB = (CA == 0.0) ? 0 : Q/CA;
			double S1 = (CA+CB)-ADIV3;

			T = S1;
		}
	}

	// Compute X at time T.
	double AX = MakeDouble(m_Bezier.AX);
	double BX = MakeDouble(m_Bezier.BX);
	double CX = MakeDouble(m_Bezier.CX);
	double DX = MakeDouble(m_Bezier.DX);

	double X = AX+T*((3*(-AX+BX))+T*((3*(AX-(BX+BX)+CX))+T*(-AX+3*(BX-CX)+DX)));

	return MakeFixed(X);
}

long CWarpFieldEdge::BezierY(long X)
{
	double T;

	// Clip X coordinate to defined warp field range.
	// Allowing X to go outside of this range can cause
	// can cause roots to be found which are not what the
	// code expects.
	if (m_Bezier.AX < m_Bezier.DX)
	{
		if (X < m_Bezier.AX)
		{
			X = m_Bezier.AX;
		}
		if (X > m_Bezier.DX)
		{
			X = m_Bezier.DX;
		}
	}
	else
	{
		if (X > m_Bezier.AX)
		{
			X = m_Bezier.AX;
		}
		if (X < m_Bezier.DX)
		{
			X = m_Bezier.DX;
		}
	}

	double XB = MakeDouble(m_Bezier.BX-m_Bezier.AX);
	double XC = MakeDouble(m_Bezier.CX-m_Bezier.AX);
	double XD = MakeDouble(m_Bezier.DX-m_Bezier.AX);
	double XX = MakeDouble(X-m_Bezier.AX);

	double BA = 3*(XB-XC)+XD;
	double BB = 3*(XC-2*XB);
	double BC = 3*XB;
	double BD = -XX;

	if (fabs(BA) < 1e-5)
	{
		// Speed along axis is aproximately constant.
		T = (fabs(XD) >= 1e-5) ? XX/XD : XD/2;
	}
	else
	{
		double A = BB/BA;
		double B = BC/BA;
		double C = BD/BA;

		double Q = (A*A-3*B)/9;
		double R = (A*(2*A*A-9*B)+27*C)/54;
		double P = R*R-Q*Q*Q;

		double ADIV3 = A/3;

		if (P < 0.0)
		{
			// Cubic has three real roots.
			double THETA = acos(R/pow(Q, 3.0/2.0));
			double THETADIV3 = THETA/3;
			double SQRTQ = -2*sqrt(Q);
			double S1 = SQRTQ*cos(THETADIV3)-ADIV3;
			double S2 = SQRTQ*cos(THETADIV3+PI*(2.0/3.0))-ADIV3;
			double S3 = SQRTQ*cos(THETADIV3-PI*(2.0/3.0))-ADIV3;

			T = S1;
			if (fabs(S2-0.5) < fabs(T-0.5))
			{
				T = S2;
			}
			if (fabs(S3-0.5) < fabs(T-0.5))
			{
				T = S3;
			}
		}
		else
		{
			// Cubic has one real root.
			double CA = pow(fabs(R)+sqrt(P), 1.0/3.0);
			if (R > 0)
			{
				CA = -CA;
			}
			double CB = (CA == 0.0) ? 0 : Q/CA;
			double S1 = (CA+CB)-ADIV3;

			T = S1;
		}
	}

	// Compute Y at time T.
	double AY = MakeDouble(m_Bezier.AY);
	double BY = MakeDouble(m_Bezier.BY);
	double CY = MakeDouble(m_Bezier.CY);
	double DY = MakeDouble(m_Bezier.DY);

	double Y = AY+T*((3*(-AY+BY))+T*((3*(AY-(BY+BY)+CY))+T*(-AY+3*(BY-CY)+DY)));

	return MakeFixed(Y);
}

/////////////////////////////////////////////////////////////////////////////
// CWarpFieldBounds

void CWarpFieldBounds::FromData(const CWarpFieldBoundsData& Data)
{
	Type((BoundsType)Data.m_nType);
	m_Edge[0].FromData(Data.m_Edge[0]);
	m_Edge[1].FromData(Data.m_Edge[1]);
	m_Edge[2].FromData(Data.m_Edge[2]);
	m_Edge[3].FromData(Data.m_Edge[3]);
}

void CWarpFieldBounds::ToData(CWarpFieldBoundsData& Data)
{
	ASSERT(Type()==X || Type()==Y || Type()==XY);
	Data.m_nType = (SHORT)Type();
	m_Edge[0].ToData(Data.m_Edge[0]);
	m_Edge[1].ToData(Data.m_Edge[1]);
	m_Edge[2].ToData(Data.m_Edge[2]);
	m_Edge[3].ToData(Data.m_Edge[3]);
}

void CWarpFieldBounds::Transform(COutlinePathMatrix* pMatrix)
{
	ASSERT(pMatrix != NULL);
	
	for (int nEdge = 0; nEdge < 4; nEdge++)
	{
		Edge(nEdge)->Transform(pMatrix);
	}
}

void CWarpFieldBounds::Path(COutlinePath* pPath)
{
	ASSERT(pPath != NULL);
	
	pPath->Free();
	Edge(0)->Path(pPath, TRUE);
	Edge(1)->Path(pPath, FALSE);
	Edge(2)->Path(pPath, FALSE);
	Edge(3)->Path(pPath, FALSE);
	pPath->Close();
	pPath->End();
}

BOOL CWarpFieldBounds::Linear(void)
{
	BOOL fResult = FALSE;
	
	BOOL fTopBottomLines = Edge(0)->Linear() && Edge(2)->Linear();
	BOOL fLeftRightLines = Edge(1)->Linear() && Edge(3)->Linear();
	
	switch (Type())
	{
		case X:
		{
			fResult = fLeftRightLines;
			break;
		}
		
		case Y:
		{
			fResult = fTopBottomLines;
			break;
		}
		
		case XY:
		{
			fResult = fLeftRightLines && fTopBottomLines;
			break;
		}
	
		default:
		{
//j			ASSERT(FALSE);
			break;
		}
	}
	
	return fResult;
}

/////////////////////////////////////////////////////////////////////////////
// CWarpField

void CWarpField::Transform(const CRect& crBounds)
{
	if (m_crBounds != crBounds)
	{
		m_crBounds = crBounds;

		m_Matrix.Identity();
		m_Matrix.ScaleBy(
			MakeFixed(m_crBounds.Width()/1000.0),
			MakeFixed(m_crBounds.Height()/1000.0));
		m_Matrix.TranslateBy(
			MakeFixed(m_crBounds.left),
			MakeFixed(m_crBounds.top));
			
		m_Transformed.Init(&m_Original);
		
		for (int nEdge = 0; nEdge < 4; nEdge++)
		{
			m_Transformed.Edge(nEdge)->Transform(&m_Matrix);
		}
	}
}
	
void CWarpField::MoveTo(COutlinePath* pPath, long AX, long AY)
{
	ASSERT(pPath != NULL);
	
	m_CurrentPoint.x = AX;
	m_CurrentPoint.y = AY;
	
	WarpPoint(&AX, &AY);
	pPath->MoveTo(AX, AY);
}

void CWarpField::LineTo(COutlinePath* pPath, long AX, long AY)
{
	ASSERT(pPath != NULL);
	
	if (Linear())
	{
		m_CurrentPoint.x = AX;
		m_CurrentPoint.y = AY;
	
		WarpPoint(&AX, &AY);
		pPath->LineTo(AX, AY);
	}
	else
	{
		WarpLine(pPath, m_CurrentPoint.x, m_CurrentPoint.y, AX, AY);
		
		m_CurrentPoint.x = AX;
		m_CurrentPoint.y = AY;
	}
}

void CWarpField::SplineTo(COutlinePath* pPath, long AX, long AY, long BX, long BY)
{
	ASSERT(pPath != NULL);
	
	if (Linear())
	{
		m_CurrentPoint.x = BX;
		m_CurrentPoint.y = BY;
		
		WarpPoint(&AX, &AY);
		WarpPoint(&BX, &BY);
		pPath->SplineTo(AX, AY, BX, BY);
	}
	else
	{
		WarpSpline(pPath, m_CurrentPoint.x, m_CurrentPoint.y, AX, AY, BX, BY);
		
		m_CurrentPoint.x = BX;
		m_CurrentPoint.y = BY;
	}
}

void CWarpField::BezierTo(COutlinePath* pPath, long AX, long AY, long BX, long BY, long CX, long CY)
{
	ASSERT(pPath != NULL);
	
	if (Linear())
	{
		m_CurrentPoint.x = CX;
		m_CurrentPoint.y = CY;
		
		WarpPoint(&AX, &AY);
		WarpPoint(&BX, &BY);
		WarpPoint(&CX, &CY);
		pPath->BezierTo(AX, AY, BX, BY, CX, CY);
	}
	else
	{
		WarpBezier(pPath, m_CurrentPoint.x, m_CurrentPoint.y, AX, AY, BX, BY, CX, CY);
		
		m_CurrentPoint.x = CX;
		m_CurrentPoint.y = CY;
	}
}

void CWarpField::WarpPoint(long* pAX, long* pAY)
{
	ASSERT(pAX != NULL);
	ASSERT(pAY != NULL);

	if ((*pAX < MakeFixed(m_crBounds.left)-MakeFixed(m_crBounds.Width())/2)
	 || (*pAX > MakeFixed(m_crBounds.right)+MakeFixed(m_crBounds.Width())/2)
	 || (*pAY < MakeFixed(m_crBounds.top)-MakeFixed(m_crBounds.Height())/2)
	 || (*pAY > MakeFixed(m_crBounds.bottom)+MakeFixed(m_crBounds.Height())/2))
	{
		return;
	}

	switch (m_Transformed.Type())
	{
		case CWarpFieldBounds::X:
		{
			long lLeft = m_Transformed.Edge(3)->X(*pAY);
			long lRight = m_Transformed.Edge(1)->X(*pAY);
			long lX = MulDivFixed(
								*pAX-MakeFixed(m_crBounds.left),
								lRight-lLeft,
								MakeFixed(m_crBounds.Width())) + lLeft;
								
			*pAX = lX;
			break;
		}
		
		case CWarpFieldBounds::Y:
		{
			long lTop = m_Transformed.Edge(0)->Y(*pAX);
			long lBottom = m_Transformed.Edge(2)->Y(*pAX);
			long lY = MulDivFixed(
								*pAY-MakeFixed(m_crBounds.top),
								lBottom-lTop,
								MakeFixed(m_crBounds.Height())) + lTop;
								
			*pAY = lY;
			break;
		}
		
		case CWarpFieldBounds::XY:
		{
			break;
		}

		default:
		{
//j			ASSERT(FALSE);
			break;
		}
	}
}

void CWarpField::UnwarpPoint(long* pAX, long* pAY)
{
	ASSERT(pAX != NULL);
	ASSERT(pAY != NULL);

	switch (m_Transformed.Type())
	{
		case CWarpFieldBounds::X:
		{
			long lLeft = m_Transformed.Edge(3)->X(*pAY);
			long lRight = m_Transformed.Edge(1)->X(*pAY);
			if ((*pAX >= lLeft) && (*pAX <= lRight))
			{
				*pAX = MulDivFixed(
								*pAX-lLeft,
								MakeFixed(m_crBounds.Width()),
								lRight-lLeft) + MakeFixed(m_crBounds.left);
			}
			break;
		}
		
		case CWarpFieldBounds::Y:
		{
			long lTop = m_Transformed.Edge(0)->Y(*pAX);
			long lBottom = m_Transformed.Edge(2)->Y(*pAX);
			if ((*pAY >= lTop) && (*pAY <= lBottom))
			{
				*pAY = MulDivFixed(
								*pAY-lTop,
								MakeFixed(m_crBounds.Height()),
								lBottom-lTop) + MakeFixed(m_crBounds.top);
			}
			break;
		}
		
		case CWarpFieldBounds::XY:
		{
			break;
		}

		default:
		{
			ASSERT(FALSE);
			break;
		}
	}
}

void CWarpField::WarpLine(COutlinePath* pPath, long AX, long AY, long BX, long BY)
{
	WarpBezier(
				  pPath,
				  AX,                    AY,
				  OneThirdFixed(AX, BX), OneThirdFixed(AY, BY),
				  OneThirdFixed(BX, AX), OneThirdFixed(BY, AY),
				  BX,                    BY);
}

void CWarpField::WarpSpline(COutlinePath* pPath, long AX, long AY, long BX, long BY, long CX, long CY)
{
	WarpBezier(
				  pPath,
				  AX,                    AY,
				  OneThirdFixed(BX, AX), OneThirdFixed(BY, AY),
				  OneThirdFixed(BX, CX), OneThirdFixed(BY, CY),
				  CX,                    CY);
}

static BOOL NeedsSplitting(long AX, long AY, long BX, long BY, long CX, long CY, long DX, long DY)
{
	// Test minimum size.
	long dX = DX-AX;
	long dY = DY-AY;
	if (dX < 0)
	{
		dX = -dX;
	}
	if (dY < 0)
	{
		dY = -dY;
	}
	if ((dX < MakeFixed(5)) && (dY < MakeFixed(5)))
	{
		return FALSE;
	}

	// Test angles.

	static int nAngle = 2;

	CFixed ThetaAD = ArcTangent2(DY - AY, DX - AX);
	CFixed ThetaAB = ArcTangent2(BY - AY, BX - AX);
	CFixed ThetaCD = ArcTangent2(DY - CY, DX - CX);

	CFixed Delta1 = ThetaAB - ThetaAD;
	if (Delta1 < 0)
	{
		Delta1 = - Delta1;
	}
	CFixed Delta2 = ThetaCD - ThetaAD;
	if (Delta2 < 0)
	{
		Delta2 = - Delta2;
	}
	return (Delta1 >= MakeFixed(nAngle)) || (Delta2 >= MakeFixed(nAngle));
}

#define MAX_WARP_BEZIER_DEPTH	(20)

void CWarpField::WarpBezier(COutlinePath* pPath, long AX, long AY, long BX, long BY, long CX, long CY, long DX, long DY, int nDepth /*=0*/)
{
	long	AXW = AX,
			AYW = AY,
			BXW = BX,
			BYW = BY,
			CXW = CX,
			CYW = CY,
			DXW = DX,
			DYW = DY;

	WarpPoint(&AXW, &AYW);
	WarpPoint(&BXW, &BYW);
	WarpPoint(&CXW, &CYW);
	WarpPoint(&DXW, &DYW);

#ifdef _DEBUG
	if (nDepth >= MAX_WARP_BEZIER_DEPTH)
	{
//		ASSERT(FALSE);
		TRACE("###### SCOTTIE: CAP'N! THE WARP DRIVE IS OVERHEAT'N! ######\n");
		TRACE("A %08lx %08lx   B %08lx %08lx   C %08lx %08lx   D %08lx %08lx\n", AX, AY, BX, BY, CX, CY, DX, DY);
		TRACE("A %08lx %08lx   B %08lx %08lx   C %08lx %08lx   D %08lx %08lx\n", AXW, AYW, BXW, BYW, CXW, CYW, DXW, DYW);
	}
#endif

	if ((nDepth < MAX_WARP_BEZIER_DEPTH) && (NeedsSplitting(AXW, AYW, BXW, BYW, CXW, CYW, DXW, DYW)))
	{
		CFixedPoint MAB, MBC, MCD, MABC, MBCD, M;

		MAB.x = MidPointFixed(AX, BX);
		MAB.y = MidPointFixed(AY, BY);
		MBC.x = MidPointFixed(BX, CX);
		MBC.y = MidPointFixed(BY, CY);
		MCD.x = MidPointFixed(CX, DX);
		MCD.y = MidPointFixed(CY, DY);

		MABC.x = MidPointFixed(MAB.x, MBC.x);
		MABC.y = MidPointFixed(MAB.y, MBC.y);

		MBCD.x = MidPointFixed(MBC.x, MCD.x);
		MBCD.y = MidPointFixed(MBC.y, MCD.y);

		M.x = MidPointFixed(MABC.x, MBCD.x);
		M.y = MidPointFixed(MABC.y, MBCD.y);

		WarpBezier(pPath, AX, AY, MAB.x, MAB.y, MABC.x, MABC.y, M.x, M.y, nDepth+1);
		WarpBezier(pPath, M.x, M.y, MBCD.x, MBCD.y, MCD.x, MCD.y, DX, DY, nDepth+1);
	}
	else
	{
		pPath->BezierTo(BXW, BYW, CXW, CYW, DXW, DYW);
	}
}

/////////////////////////////////////////////////////////////////////////////
// WarpFieldRecord

/*
// The creator for a warp field record.
*/

ERRORCODE WarpFieldRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */
	WarpFieldRecord* pRecord;
	if ((pRecord = new WarpFieldRecord(number, type, owner, where)) == NULL)
	{
		return ERRORCODE_Memory;
	}
	*record = (DatabaseRecordPtr)pRecord;
	WarpField_CreateStruct* cs = (WarpField_CreateStruct*)creation_data;
	if (cs != NULL)
	{
		pRecord->Name(cs->m_pszName);
		*(pRecord->BoundsData()) = cs->m_BoundsData;
	}
	return ERRORCODE_None;
}

/*
// The constructor for a warp field record.
*/

WarpFieldRecord::WarpFieldRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&record, 0, sizeof(record));
}

/*
// Destructor for a warp field record.
*/

WarpFieldRecord::~WarpFieldRecord()
{
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE WarpFieldRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read_record(&record, sizeof(record))) == ERRORCODE_None
			&& (error = device->read_cstring(m_csName)) == ERRORCODE_None)
	{
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE WarpFieldRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->write_record(&record, sizeof(record))) == ERRORCODE_None
			&& (error = device->write_cstring(m_csName)) == ERRORCODE_None)
	{
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE WarpFieldRecord::SizeofData(StorageDevicePtr device)
{
	return device->size_record(sizeof(record))
 				+ device->size_cstring(m_csName);
}

/*
// Assign method.
*/

ERRORCODE WarpFieldRecord::assign(DatabaseRecordRef srecord)
{
/* Assign the base record first. */
	return DatabaseRecord::assign(srecord);
}
