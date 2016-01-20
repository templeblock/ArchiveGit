// ****************************************************************************
//
//  File Name:			Bezier.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Functions to draw bezier curves
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/Bezier.cpp                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "Bezier.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

void FillBezier( const RRealPoint& b0, const RRealPoint& b1, const RRealPoint& b2, const RRealPoint& b3, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints );

// ****************************************************************************
//
//  Function Name:	::DoBezier2( )
//
//  Description:		Sets up to compute the points of a bezier curve.
//
//							Terminates when b0, b1, b2 are collinear and b0, b2, b3 are collinear
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void DoBezier( const RIntPoint* bezierPoints, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints )
	{
	pointCount	= 0;
	::FillBezier( bezierPoints[-1], bezierPoints[0], bezierPoints[1], bezierPoints[2], outPoints, pointCount, nMaxPoints );
	}

// ****************************************************************************
//
//  Function Name:	::FillBezier2( )
//
//  Description:		Computes the points of a bezier curve.
//
//							Terminates when b0, b1, b2 are collinear and b0, b2, b3 are collinear
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void FillBezier( const RIntPoint& b0, const RIntPoint& b1, const RIntPoint& b2, const RIntPoint& b3, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints )
	{
	RRealPoint	rb0( b0 );
	RRealPoint	rb1( b1 );
	RRealPoint	rb2( b2 );
	RRealPoint	rb3( b3 );
	::FillBezier( rb0, rb1, rb2, rb3, outPoints, pointCount, nMaxPoints );
	}

void FillBezier( const RRealPoint& b0, const RRealPoint& b1, const RRealPoint& b2, const RRealPoint& b3, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints )
{
	if( ::PointsAreCollinear(b0,b1,b2) && PointsAreCollinear(b0,b2,b3) )
		{
		if( pointCount < nMaxPoints )
			*outPoints++	= RIntPoint(b3);
		++pointCount;
		}
	else
		{
		RRealPoint	ptTmp[7];
		::SubdivideBezier( b0, b1, b2, b3, ptTmp );
		::FillBezier( ptTmp[0], ptTmp[1], ptTmp[2], ptTmp[3], outPoints, pointCount, nMaxPoints );
		::FillBezier( ptTmp[3], ptTmp[4], ptTmp[5], ptTmp[6], outPoints, pointCount, nMaxPoints );
		}
	}
void FillBezier( const RRealPoint& b0, const RRealPoint& b1, const RRealPoint& b2, const RRealPoint& b3, RRealPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints )
{
	if( ::PointsAreCollinear(b0,b1,b2) && PointsAreCollinear(b0,b2,b3) )
		{
		if( pointCount < nMaxPoints )
			*outPoints++	= b3;
		++pointCount;
		}
	else
		{
		RRealPoint	ptTmp[7];
		::SubdivideBezier( b0, b1, b2, b3, ptTmp );
		::FillBezier( ptTmp[0], ptTmp[1], ptTmp[2], ptTmp[3], outPoints, pointCount, nMaxPoints );
		::FillBezier( ptTmp[3], ptTmp[4], ptTmp[5], ptTmp[6], outPoints, pointCount, nMaxPoints );
		}
	}

// ****************************************************************************
//
// Function Name:		::SubdivideBezier( )
//
// Description:		Sub divide a bezier segment once
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void SubdivideBezier( const RIntPoint& ptStart, 
							 const RIntPoint& ptControl1, 
							 const RIntPoint& ptControl2, 
							 const RIntPoint& ptEnd, 
							 RIntPoint* pOutPoints ) 
	{
	RIntPoint					pt0, pt1, pt2;
	
	pOutPoints[0] = ptStart;
	pOutPoints[6] = ptEnd;
	::midpoint( pt0, ptStart, ptControl1 );
	pOutPoints[1] = pt0;
	::midpoint( pt1, ptControl1, ptControl2 );
	::midpoint( pt2, ptControl2, ptEnd );
	pOutPoints[5] = pt2;
	::midpoint( pt0, pt0, pt1 );
	pOutPoints[2] = pt0;
	::midpoint( pt1, pt1, pt2 );
	pOutPoints[4] = pt1;
	::midpoint( pt0, pt0, pt1 );
	pOutPoints[3] = pt0;
	}

// ****************************************************************************
//
// Function Name:		::SubdivideBezier( )
//
// Description:		Sub divide a bezier segment once
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void SubdivideBezier( const RRealPoint& ptStart, 
							 const RRealPoint& ptControl1, 
							 const RRealPoint& ptControl2, 
							 const RRealPoint& ptEnd, 
							 RRealPoint* pOutPoints ) 
	{
	RRealPoint					pt0, pt1, pt2;
	
	pOutPoints[0] = ptStart;
	pOutPoints[6] = ptEnd;
	::midpoint( pt0, ptStart, ptControl1 );
	pOutPoints[1] = pt0;
	::midpoint( pt1, ptControl1, ptControl2 );
	::midpoint( pt2, ptControl2, ptEnd );
	pOutPoints[5] = pt2;
	::midpoint( pt0, pt0, pt1 );
	pOutPoints[2] = pt0;
	::midpoint( pt1, pt1, pt2 );
	pOutPoints[4] = pt1;
	::midpoint( pt0, pt0, pt1 );
	pOutPoints[3] = pt0;
	}


// ****************************************************************************
//
// Function Name:		::SubdivideBezier( )
//
// Description:		Sub divide a bezier segment recursively
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void SubdivideBezier( const RRealPoint& ptStart, 
							 const RRealPoint& ptControl1, 
							 const RRealPoint& ptControl2, 
							 const RRealPoint& ptEnd, 
							 RRealPoint* pOutPoints,
							 YPointCount nrSegments ) 
	{
	YPointCount 		nrByHalf = ( nrSegments / 2 );
	RRealPoint* 		pOut2Points = pOutPoints + ( 3 * nrByHalf );
	RRealPoint			points[ 8 ];

	TpsAssert( ( ( nrByHalf * 2 ) == nrSegments ), "Beziers must be subdivided in even increments." );
	::SubdivideBezier( ptStart, ptControl1, ptControl2, ptEnd, points );
	if ( nrSegments > 2 )
		{
		::SubdivideBezier( points[0], points[1], points[2], points[3], pOutPoints, nrByHalf );
		::SubdivideBezier( points[3], points[4], points[5], points[6], pOut2Points, nrByHalf );
		}
	else
		{
		for ( int j = 0; j < 7; j++ )
			pOutPoints[j] = points[j];
		}
	}


// ****************************************************************************
//
//  Function Name:	CreateBezierFromArc( )
//
//  Description:		Computes the bezier points required to represent the given arc.
//                   will the interior angle (signed!) from a0 to a2 passing through a1...
//
//  Returns:			TRUE if the arc data can be represented by a bezier else FALSE (interior
//                   angle >= kPI)
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN CreateBezierFromArc( const RRealPoint& a0, const RRealPoint& a2, const RRealPoint& center, YAngle angInterior,
										RRealPoint& b0, RRealPoint& b1, RRealPoint& b2, RRealPoint& b3 )
{
	//
	// This only works if the angle is less than PI...
	if( ::Abs( angInterior ) < kPI )
		{
		//
		// Compute the intersection Q of the tangents at a0 and a2...
		RRealPoint	a0Perp( a0.m_x-(a0.m_y-center.m_y), a0.m_y+(a0.m_x-center.m_x) );
		RRealPoint	a2Perp( a2.m_x-(a2.m_y-center.m_y), a2.m_y+(a2.m_x-center.m_x) );
		RRealPoint	Q;
		if( GetLineSegmentIntersection(a0,a0Perp,a2,a2Perp,Q,BOOLEAN(FALSE)) )
			{
			//
			// Got Q, so go ahead and compute the midpoint P of the arc...
			YRealDimension	rdRadius( a0.Distance(center) );
			YAngle			leg1( ::atan2( a0.m_y - center.m_y, a0.m_x - center.m_x ) );
			RRealPoint		P( center.m_x + rdRadius*::cos(leg1+angInterior/2.0), center.m_y + rdRadius*::sin(leg1+angInterior/2.0) );
			//
			// Set the known points...
			b0	= a0;
			b3	= a2;
			//
			// Compute the parameter s for b1 and b2 on the lines b0Q and b3Q respectively
			double	den	= 2.0*Q.m_x - (b0.m_x + b3.m_x);
			double	num	= 2.0*P.m_x - (b0.m_x + b3.m_x);;
			if( AreFloatsEqual(den,0.0) || AreFloatsEqual(num,0.0) )
				{
				//
				// the endpoints of the arc are on the same horizontal line.
				// Since the equation works equally well on either coordinate
				// set, we can switch to the Y coordinate to finish...
				den	= 2.0*Q.m_y - (b0.m_y + b3.m_y);
				num	= 2.0*P.m_y - (b0.m_y + b3.m_y);
				}
			double	s = (4.0/3.0) * (num/den);
			// Compute b1 and b2...
			RRealSize	vecb0Q( Q-a0 );
			RRealSize	vecb3Q( Q-a2 );
			b1	= RRealPoint( b0.m_x + vecb0Q.m_dx*s, b0.m_y + vecb0Q.m_dy*s );
			b2	= RRealPoint( b3.m_x + vecb3Q.m_dx*s, b3.m_y + vecb3Q.m_dy*s );
			//
			// Done...
			return TRUE;
			}
		}
	//
	// interior angle is greater than or equal to PI...
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	CreatePolyBezierFromArc( )
//
//  Description:		Computes the bezier points required to represent the arc through
//                   the given three points...
//
//  Returns:			TRUE if the arc data can be represented by a polybezier else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN CreatePolyBezierFromArc( const RRealPoint& a0, const RRealPoint& a1, const RRealPoint& a2, RArray<RRealPoint>& arPolyBezier )
{
	RRealPoint	ptCenter;
	YAngle		angInterior;
	arPolyBezier.Empty();
	if( !ComputeArcMetrics(a0,a1,a2,ptCenter,angInterior) )
		{
		//
		// the three points might form a line.  check it...
		if( PointsAreCollinear(a0,a1,a2) )
			{
			//
			// the points form a line, so the bezier is simple...
			// (NOTE: we assume valid data (i.e. a1 is between a0 and a2), but the test is lengthy so we don't check it...

			arPolyBezier.InsertAtEnd( a0 );
			arPolyBezier.InsertAtEnd( a1 );
			arPolyBezier.InsertAtEnd( a1 );
			arPolyBezier.InsertAtEnd( a2 );
			return TRUE;
			}
		//
		// Not collinear and couldn't get metrics.  This can't be good...
		}
	else
		{
		//
		// Got the arc metrics...
		RRealPoint	b0;
		RRealPoint	b1;
		RRealPoint	b2;
		RRealPoint	b3;
		if( ::Abs(angInterior) >= (kPI-0.001) )
			{
			//
			// We need 2 bezier segments to represent this arc...
			// Split the arc in the middle and do it twice...
			YRealDimension	rdRadius( ptCenter.Distance(a0) );
			RRealPoint	pt( YRealDimension(a0.m_x + rdRadius*::cos(angInterior/2.0)), YRealDimension(a0.m_y + rdRadius*::sin(angInterior/2.0)) );
			angInterior	/= 2.0;
			if( CreateBezierFromArc(a0,pt,ptCenter,angInterior,b0,b1,b2,b3) )
				{
				arPolyBezier.InsertAtEnd( b0 );
				arPolyBezier.InsertAtEnd( b1 );
				arPolyBezier.InsertAtEnd( b2 );
				arPolyBezier.InsertAtEnd( b3 );
				if( CreateBezierFromArc(pt,a2,ptCenter,angInterior,b0,b1,b2,b3) )
					{
					//
					// b0 here is the same as b3 above...
					arPolyBezier.InsertAtEnd( b1 );
					arPolyBezier.InsertAtEnd( b2 );
					arPolyBezier.InsertAtEnd( b3 );
					return TRUE;
					}
				//
				// else not a good sign...
				}
			//
			// else not a good sign...
			}
		else
			{
			//
			// We can represent this arc with one bezier segment...
			if( CreateBezierFromArc(a0,a2,ptCenter,angInterior,b0,b1,b2,b3) )
				{
				arPolyBezier.InsertAtEnd( b0 );
				arPolyBezier.InsertAtEnd( b1 );
				arPolyBezier.InsertAtEnd( b2 );
				arPolyBezier.InsertAtEnd( b3 );
				return TRUE;
				}
			//
			// else not a good sign...
			}
		}
	return FALSE;
}
