// ****************************************************************************
//
//  File Name:			QuadraticSpline.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
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
//  $Logfile:: /PM8/Framework/Source/QuadraticSpline.cpp                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "QuadraticSpline.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage


// ****************************************************************************
//
//  Function Name:	::ExpandPolyQuadraticSpline( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntPoint* ExpandPolyQuadraticSpline( const RIntPoint& ptStart, const RIntPoint* pptSpline, YPointCount numPoints, YPointCount& numExpanded )
{
	YPointCount	nMaxExpanded	= YPointCount( ::GetGlobalBufferSize()/sizeof(RIntPoint) );
	int			nTries=2;
	RIntPoint*	pptExpanded	= NULL;
	while( !pptExpanded && nTries-- )
		{
		pptExpanded		= reinterpret_cast<RIntPoint*>(::GetGlobalBuffer(uLONG(nMaxExpanded)*sizeof(RIntPoint)));
		RIntPoint*			ppt				= pptExpanded;
		YPointCount			npts				= numPoints;
		const RIntPoint*	pptTmp			= pptSpline + 1;
		RIntPoint			ptPrev			= ptStart;
		*ppt++									= ptStart;
		numExpanded								= 1;
		while( npts>=2 )
			{
			::FillQuadraticSpline( ptPrev, *pptTmp, *(pptTmp+1), ppt, numExpanded, nMaxExpanded );
			ptPrev	= *(pptTmp+1);
			npts		-= 2;
			pptTmp	+= 2;
			}
		if( numExpanded > nMaxExpanded )
			{
			::ReleaseGlobalBuffer( reinterpret_cast<uBYTE*>(pptExpanded) );
			pptExpanded		= NULL;
			nMaxExpanded	= numExpanded;
			}
		}
	if( !pptExpanded )
		throw kMemory;
	return pptExpanded;
}

// ****************************************************************************
//
//  Function Name:	::ExpandPolyQuadraticSpline( )
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealPoint* ExpandPolyQuadraticSpline( const RRealPoint& ptStart, const RRealPoint* pptSpline, YPointCount numPoints, YPointCount& numExpanded )
{
	YPointCount	nMaxExpanded	= YPointCount( ::GetGlobalBufferSize()/sizeof(RRealPoint) );
	int			nTries=2;
	RRealPoint*	pptExpanded	= NULL;
	while( !pptExpanded && nTries-- )
		{
		pptExpanded		= reinterpret_cast<RRealPoint*>(::GetGlobalBuffer(uLONG(nMaxExpanded)*sizeof(RRealPoint)));
		RRealPoint*			ppt				= pptExpanded;
		YPointCount			npts				= numPoints;
		const RRealPoint*	pptTmp			= pptSpline + 1;
		RRealPoint			ptPrev			= ptStart;
		*ppt++									= ptStart;
		numExpanded								= 1;
		while( npts>=2 )
			{
			::FillQuadraticSpline( ptPrev, *pptTmp, *(pptTmp+1), ppt, numExpanded, nMaxExpanded );
			ptPrev	= *(pptTmp+1);
			npts		-= 2;
			pptTmp	+= 2;
			}
		if( numExpanded > nMaxExpanded )
			{
			::ReleaseGlobalBuffer( reinterpret_cast<uBYTE*>(pptExpanded) );
			pptExpanded		= NULL;
			nMaxExpanded	= numExpanded;
			}
		}
	if( !pptExpanded )
		throw kMemory;
	return pptExpanded;
}

// ****************************************************************************
//
//  Function Name:	::FillQuadraticSpline( )
//
//  Description:		Computes the points of a quadratic spline curve.
//
//							Same as algorithm 4 for bezier.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void FillQuadraticSpline( const RIntPoint& ptStart, const RIntPoint& ptControl, const RIntPoint& ptEnd, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints )
{
	RRealPoint	rptStart( ptStart );
	RRealPoint	rptCtrl( ptControl );
	RRealPoint	rptEnd( ptEnd );
	::FillQuadraticSpline( rptStart, rptCtrl, rptEnd, outPoints, pointCount, nMaxPoints );
}

// ****************************************************************************
//
//  Function Name:	::FillQuadraticSpline( )
//
//  Description:		Computes the points of a quadratic spline curve.
//
//							Same as algorithm 4 for bezier.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void FillQuadraticSpline( const RRealPoint& ptStart, const RRealPoint& ptControl, const RRealPoint& ptEnd, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints )
{
	if( ::PointsAreCollinear(ptStart,ptControl,ptEnd) )
	{
		if( pointCount < nMaxPoints )
			*outPoints++ = RIntPoint(ptEnd);
		++pointCount;
	}
	else
	{
		RRealPoint	ptTmp[5];
		::SubdivideQuadraticSpline( ptStart, ptControl, ptEnd, ptTmp );
		::FillQuadraticSpline( ptTmp[0], ptTmp[1], ptTmp[2], outPoints, pointCount, nMaxPoints );
		::FillQuadraticSpline( ptTmp[2], ptTmp[3], ptTmp[4], outPoints, pointCount, nMaxPoints );
	}
}


// ****************************************************************************
//
//  Function Name:	::FillQuadraticSpline( )
//
//  Description:		Computes the points of a quadratic spline curve.
//
//							Same as algorithm 4 for bezier.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void FillQuadraticSpline( const RRealPoint& ptStart, const RRealPoint& ptControl, const RRealPoint& ptEnd, RRealPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints )
{
	if( ::PointsAreCollinear( ptStart, ptControl, ptEnd ) )
	{
		if( pointCount < nMaxPoints )
			*outPoints++ = ptEnd;
		++pointCount;
	}
	else
	{
		RRealPoint	ptTmp[5];
		::SubdivideQuadraticSpline( ptStart, ptControl, ptEnd, ptTmp );
		::FillQuadraticSpline( ptTmp[0], ptTmp[1], ptTmp[2], outPoints, pointCount, nMaxPoints );
		::FillQuadraticSpline( ptTmp[2], ptTmp[3], ptTmp[4], outPoints, pointCount, nMaxPoints );
	}
}


// ****************************************************************************
//
// Function Name:		::SubdivideQuadraticSpline( )
//
// Description:		Sub divide a quadratic spline segment once
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void SubdivideQuadraticSpline( const RIntPoint& ptStart, 
							 			 const RIntPoint& ptControl, 
										 const RIntPoint& ptEnd, 
										 RIntPoint* pOutPoints )
{
RIntPoint						pt1, pt2, pt3;
	
	pOutPoints[0] = ptStart;
	pOutPoints[4] = ptEnd;
	::midpoint( pt1, ptStart, ptControl );
	pOutPoints[1] = pt1;
	::midpoint( pt3, ptControl, ptEnd );
	pOutPoints[3] = pt3;
	::midpoint( pt2, pt1, pt3 );
	pOutPoints[2] = pt2;
}

// ****************************************************************************
//
// Function Name:		::SubdivideQuadraticSpline( )
//
// Description:		Sub divide a quadratic spline segment once
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void SubdivideQuadraticSpline( const RRealPoint& ptStart, 
							 			 const RRealPoint& ptControl, 
										 const RRealPoint& ptEnd, 
										 RRealPoint* pOutPoints )
{
RRealPoint						pt1, pt2, pt3;
	
	pOutPoints[0] = ptStart;
	pOutPoints[4] = ptEnd;
	::midpoint( pt1, ptStart, ptControl );
	pOutPoints[1] = pt1;
	::midpoint( pt3, ptControl, ptEnd );
	pOutPoints[3] = pt3;
	::midpoint( pt2, pt1, pt3 );
	pOutPoints[2] = pt2;
}


// ****************************************************************************
//
// Function Name:		::SubdivideQuadraticSpline( )
//
// Description:		Sub divide a quadratic spline segment recursively
//
// Returns:				Nothing
//
// Exceptions:			None
//
//
// ****************************************************************************
//
void SubdivideQuadraticSpline( const RRealPoint& ptStart, 
							 			 const RRealPoint& ptControl, 
										 const RRealPoint& ptEnd, 
										 RRealPoint* pOutPoints,
										 YPointCount nrSegments )
{
YPointCount 			nrByHalf = ( nrSegments / 2 );
RRealPoint* 			pOut2Points = pOutPoints + ( 2 * nrByHalf );
RRealPoint				points[ 8 ];

	TpsAssert( ( ( nrByHalf * 2 ) == nrSegments ), "Quadratics must be subdivided in even increments." );
	::SubdivideQuadraticSpline( ptStart, ptControl, ptEnd, points );
	if ( nrSegments > 2 )
	{
		::SubdivideQuadraticSpline( points[0], points[1], points[2], pOutPoints, nrByHalf );
		::SubdivideQuadraticSpline( points[2], points[3], points[4], pOut2Points, nrByHalf );
	}
	else
	{
		for ( int j = 0; j < 5; j++ )
			pOutPoints[j] = points[j];
	}
}


	
