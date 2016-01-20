// ****************************************************************************
//
//  File Name:			QuadraticSpline.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Functions to draw Quadratic Spline curves
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
//  $Logfile:: /PM8/Framework/Include/QuadraticSpline.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

typedef float YQuadraticSplineType;

FrameworkLinkage RIntPoint* ExpandPolyQuadraticSpline( const RIntPoint& ptStart, const RIntPoint* splinePoints, YPointCount nPoints, YPointCount& pointCount );
FrameworkLinkage RRealPoint* ExpandPolyQuadraticSpline( const RRealPoint& ptStart, const RRealPoint* splinePoints, YPointCount nPoints, YPointCount& pointCount );
FrameworkLinkage void FillQuadraticSpline( const RIntPoint& ptStart, const RIntPoint& ptControl, const RIntPoint& ptEnd, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints );
FrameworkLinkage void FillQuadraticSpline( const RRealPoint& ptStart, const RRealPoint& ptControl, const RRealPoint& ptEnd, RIntPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints );
FrameworkLinkage void FillQuadraticSpline( const RRealPoint& ptStart, const RRealPoint& ptControl, const RRealPoint& ptEnd, RRealPoint*& outPoints, YPointCount& pointCount, YPointCount nMaxPoints );

void SubdivideQuadraticSpline( const RIntPoint& ptStart, 
							 			 const RIntPoint& ptControl, 
										 const RIntPoint& ptEnd, 
										 RIntPoint* pOutPoints );
void SubdivideQuadraticSpline( const RRealPoint& ptStart, 
							 			 const RRealPoint& ptControl, 
										 const RRealPoint& ptEnd, 
										 RRealPoint* pOutPoints );
void SubdivideQuadraticSpline( const RRealPoint& ptStart, 
							 			 const RRealPoint& ptControl, 
										 const RRealPoint& ptEnd, 
										 RRealPoint* pOutPoints,
										 YPointCount nrSegments );

// ****************************************************************************
//
//  Function Name:	::CreateBezierFromSpline( )
//
//  Description:		Create a bezier that will create the given quadratic spline.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
template<class S, class T>
void	CreateBezierFromSpline( const RPoint<S>* pSplinePoints, RPoint<T>* pBezierPoints )
{
	RRealPoint	controlTimes2( RRealPoint(pSplinePoints[1]).Scale(RRealSize(2.0,2.0)) );
	//
	//	First and last points are the same
	pBezierPoints[0]	= pSplinePoints[0];
	pBezierPoints[3]	= pSplinePoints[2];
	//
	//	Second and third points are computed as such...
	RRealSize	oneThird( 1.0/3.0, 1.0/3.0 );
	pBezierPoints[1]	= RPoint<T>( (RRealPoint(pSplinePoints[0])+controlTimes2).Scale(oneThird) );
	pBezierPoints[2]	= RPoint<T>( (RRealPoint(pSplinePoints[2])+controlTimes2).Scale(oneThird) );
}
	
