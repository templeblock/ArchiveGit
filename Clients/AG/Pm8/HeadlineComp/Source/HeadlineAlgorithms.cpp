// ****************************************************************************
//
//  File Name:			HeadlineAlgorithms.cpp
//
//  Project:			Headine Component
//
//  Author:				mgd
//
//  Description:		Implentaration of Various Algorithms used in
//							the RHeadlineGraphic class
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
//  Copyright (C) 1996-1997 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlineAlgorithms.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "HeadlineIncludes.h"

ASSERTNAME

#include	"Bezier.h"
#include	"HeadlineAlgorithms.h"

//
// ComputeBezierZeroSlopeParams
// Compute the parameter(s) t at which the given bezier (after rotating to the given angle)
// has slope 0...
// returns the number of roots
//
int ComputeBezierZeroSlopeParams(
			const RRealPoint*	pBez,
			YAngle				angRotate,
			YFloatType*			pRoots )
{
	RRealPoint		bez[4];
	//
	// Rotate the bezier...
	R2dTransform	xformRotate;
	xformRotate.PreRotateAboutOrigin( angRotate );
	::TransformPoints( pBez, bez, 4, (const R2dTransform&)xformRotate );

	//
	// Compute the coefficients of the derivative of the bezier function...
	YFloatType	A, B, C;
	::ComputeBezierDerivativeCoefficients( bez[0].m_y, bez[1].m_y, bez[2].m_y, bez[3].m_y, A, B, C );
	//
	// Compute the roots of the derivative (i.e. where the slope is 0)
	int	nRoots( ::SolveQuadratic(A,B,C,pRoots) );
	//
	// Validate the roots (make sure they are NOT at either end and
	// they are within the defined range of the bezier)
	const YFloatType	kEpsilon = 0.001;
	if( nRoots > 1 )
		{
		//
		// sort them in time order...
		if( pRoots[1] < pRoots[0] )
			{
			YFloatType	dTmp	= pRoots[0];
			pRoots[0]		= pRoots[1];
			pRoots[1]		= dTmp;
			}
		if( pRoots[1]<kEpsilon || pRoots[1]>(1.0-kEpsilon) )
			--nRoots;
		}
	if( nRoots>0 && (pRoots[0]<kEpsilon || pRoots[0]>(1.0-kEpsilon)) )
		{
		--nRoots;
		if( nRoots>0 )
			pRoots[0] = pRoots[1];
		}
	//
	// Tell the caller how many roots there are...
	return nRoots;
}

//
// AppendExtrudedBezierPatch
// Build the operator and point arrays for the extrusion patch for the given bezier
//
void AppendExtrudedBezierPatch(
			const RRealPoint*		pBez,
			const R2dTransform&	xformOutput,
			const R2dTransform&	xformOutputExtruded,
			EOperationArray&		arGraphicOps,
			YFillMethodArray&		arFillMethods,
			EPathOperatorArray&	arOps,
			RRealPointArray&		arPts,
			YPointCountArray&		arPointCounts )
{
	//
	// Append the path definition for the extrusion to the given path...
	//
	// Now generate the path definition...
	arOps.InsertAtEnd( kMoveTo );								// (to set the start point)
	arOps.InsertAtEnd( kBezier );								// we'll be adding one bezier segment...
	arOps.InsertAtEnd( EPathOperator(1) );					// ...
	arPts.InsertAtEnd( pBez[0]*xformOutput );				// normal b0
	arPts.InsertAtEnd( pBez[1]*xformOutput );				// normal b1
	arPts.InsertAtEnd( pBez[2]*xformOutput );				// normal b2
	arPts.InsertAtEnd( pBez[3]*xformOutput );				// normal b3
	arOps.InsertAtEnd( kLine );								// (to connect to the start of the extruded bezier)
	arOps.InsertAtEnd( EPathOperator(1) );					// ...
	arOps.InsertAtEnd( kBezier );								// bezier segment
	arOps.InsertAtEnd( EPathOperator(1) );					// ...
	arPts.InsertAtEnd( pBez[3]*xformOutputExtruded );	// extruded b0 (remember, it's in reverse order!)
	arPts.InsertAtEnd( pBez[2]*xformOutputExtruded );	// extruded b1
	arPts.InsertAtEnd( pBez[1]*xformOutputExtruded );	// extruded b2
	arPts.InsertAtEnd( pBez[0]*xformOutputExtruded );	// extruded b3
	arOps.InsertAtEnd( kLine );								// (to close the path)
	arOps.InsertAtEnd( EPathOperator(1) );					// ...
	arPts.InsertAtEnd( pBez[0]*xformOutput );				// normal b0
	arOps.InsertAtEnd( kClose );
	arOps.InsertAtEnd( kEnd );
	arGraphicOps.InsertAtEnd( kPath );
	arFillMethods.InsertAtEnd( kStroked );
	arPointCounts.InsertAtEnd( 9 );
}

//
// AppendExtrudedBezier
// Append extrusion patches for the given bezier to the given point/operator arrays
//
void AppendExtrudedBezier(
			const RRealPoint*		pBezPoints,
			const R2dTransform&	xformExtrude,
			const R2dTransform&	xformOutput,
			EOperationArray&		arGraphicOps,
			YFillMethodArray&		arFillMethods,
			EPathOperatorArray&	arOps,
			RRealPointArray&		arPts,
			YPointCountArray&		arPointCounts )
{
	RRealPoint	bez[4];
	RRealPoint	bezL[4];
	RRealPoint	bezR[4];
	RRealPoint*	pBez	= bez;
	RRealPoint*	pBezR	= bezR;
	bez[0]				= pBezPoints[0];
	bez[1]				= pBezPoints[1];
	bez[2]				= pBezPoints[2];
	bez[3]				= pBezPoints[3];

	//
	// Compute the untransformed bezier bounds...
	RRealRect	rcUntransformed;
	::GetBezierBounds( &bez[ 0 ], rcUntransformed ); 

	//
	// Compute the center point...
	RRealPoint	ptCenter( rcUntransformed.GetCenterPoint() );

	//
	// Transform the center point...
	RRealPoint	ptCenterExtruded( ptCenter * xformExtrude );

	//
	// Compute the angle from center to center...
	YAngle		angTest[4];
	int			nAngles(1);
	angTest[0] = ::ComputeAngle( ptCenter, ptCenterExtruded );

	//
	// Compute the parameters at which horizontal tangents exist...
	YFloatType	dRoots[4];
	int			nRoots( ::ComputeBezierZeroSlopeParams( bez, -angTest[0], dRoots ) );

	//
	// Compute the final test angles
	if( nRoots > 0 )
		{
		::TransformPoints( &bez[ 0 ], bezL, 4, xformExtrude );
		nAngles	= nRoots;
		while( nRoots-- )
			angTest[nRoots]	= ::ComputeAngle( ::PointOnBezier(&bez[ 0 ],dRoots[nRoots]), ::PointOnBezier(&bezL[ 0 ],dRoots[nRoots]) );
		nAngles = ::UniqueFloats( angTest, nAngles );
		}

	//
	// Compute the locations of points that we need to split the curves at to get the correct
	// extrusion patches...
	nRoots	= 0;
	while( nAngles-- )
		nRoots += ::ComputeBezierZeroSlopeParams( bez, -angTest[nAngles], dRoots+nRoots );
	nRoots = ::UniqueFloats( dRoots, nRoots );

	//
	// Build the extruded patches...
	R2dTransform			xformIdentity;
	R2dTransform			xformOutputExtruded( xformExtrude*xformOutput );
	YFloatType*				pRoot				= dRoots;
	YFloatType				dPrevRoot		= 0.0;
	while( nRoots-- )
		{
		RRealPoint*	pCurBez	= pBez;
		//
		// Split the bezier at the current root (remember to take into consideration the previous roots)...
		::SplitBezier( pBez, &bezL[ 0 ], pBezR, YFloatType((*pRoot)*(1.0-dPrevRoot)) );
		//
		// Append the extrusion patch for the left subdivision...
		::AppendExtrudedBezierPatch( bezL, xformOutput, xformOutputExtruded, arGraphicOps, arFillMethods, arOps, arPts, arPointCounts );
		//
		// Prepare for the next pass...
		RRealPoint*	pTmp	= pBez;
		pBez					= pBezR;
		pBezR					= pTmp;
		dPrevRoot			= *pRoot++;
		}
	//
	// Append the extrusion patch for the final subdivision...
	::AppendExtrudedBezierPatch( pBez, xformOutput, xformOutputExtruded, arGraphicOps, arFillMethods, arOps, arPts, arPointCounts );
}

//
// WarpTopBottom
// Vertically warp the given points in the given bounding rect between the two paths
//
void WarpTopBottomOn( RArray<RRealPoint>& arPoints, const RRealRect& rcBounds, RPath& top, RPath& bottom, YRealDimension penWidth )
{
	RRealSize						dim( rcBounds.WidthHeight() );
	RArray<RRealPoint>::YIterator	itrPoint = arPoints.Start();
	while( itrPoint != arPoints.End() )
		{
		YRealDimension	s( YRealDimension((*itrPoint).m_x - rcBounds.m_Left) / dim.m_dx );
		YRealDimension	t( YRealDimension((*itrPoint).m_y - rcBounds.m_Top) / dim.m_dy );
		TpsAssert( s>=0, "Negative distance along path." );
		TpsAssert( s<=1.0, "Distance along path greater than path length." );
		RRealPoint	ptTop( top.PointOnPath(s) );
		RRealPoint	ptBot( bottom.PointOnPath(s) );
		ptTop.m_y	+= penWidth;
		ptBot.m_y	-= penWidth;
		(*itrPoint)	= RRealPoint(ptTop.m_x*(1.0-t) + ptBot.m_x*t, ptTop.m_y*(1.0-t) + ptBot.m_y*t);
		++itrPoint;
		}
}

//
// WarpLeftRight
// Horizontally warp the given points in the given bounding rect between the two paths
//
void WarpLeftRightOn( RArray<RRealPoint>& arPoints, const RRealRect& rcBounds, RPath& left, RPath& right, YRealDimension penWidth )
{
	RRealSize						dim( rcBounds.WidthHeight() );
	RArray<RRealPoint>::YIterator	itrPoint = arPoints.Start();
	while( itrPoint != arPoints.End() )
		{
		YRealDimension	s( YRealDimension((*itrPoint).m_x - rcBounds.m_Left) / dim.m_dx );
		YRealDimension	t( YRealDimension((*itrPoint).m_y - rcBounds.m_Top) / dim.m_dy );
		TpsAssert( t>=0, "Negative distance along path." );
		TpsAssert( t<=1.0, "Distance along path greater than path length." );
		RRealPoint	ptLft( left.PointOnPath(t) );
		RRealPoint	ptRgt( right.PointOnPath(t) );
		ptLft.m_x	+= penWidth;
		ptRgt.m_x	-= penWidth;
		(*itrPoint)	= RRealPoint(ptLft.m_x*(1.0-s) + ptRgt.m_x*s, ptLft.m_y*(1.0-s) + ptRgt.m_y*s);
		++itrPoint;
		}
}

//
// WarpTopBottom
// Vertically warp the given points in the given bounding rect between the two paths
//
void WarpTopBottomOn( RArray<RIntPoint>& arPoints, const RIntRect& rcBounds, RPath& top, RPath& bottom, YRealDimension penWidth )
{
	RRealSize						dim( rcBounds.WidthHeight() );
	RArray<RIntPoint>::YIterator	itrPoint = arPoints.Start();
	while( itrPoint != arPoints.End() )
		{
		YRealDimension	s( YRealDimension((*itrPoint).m_x - rcBounds.m_Left) / dim.m_dx );
		YRealDimension	t( YRealDimension((*itrPoint).m_y - rcBounds.m_Top) / dim.m_dy );
		TpsAssert( s>=0, "Negative distance along path." );
		TpsAssert( s<=1.0, "Distance along path greater than path length." );
		RRealPoint	ptTop( top.PointOnPath(s) );
		RRealPoint	ptBot( bottom.PointOnPath(s) );
		ptTop.m_y	+= penWidth;
		ptBot.m_y	-= penWidth;
		(*itrPoint)	= RIntPoint( RRealPoint(ptTop.m_x*(1.0-t) + ptBot.m_x*t, ptTop.m_y*(1.0-t) + ptBot.m_y*t) );
		++itrPoint;
		}
}

//
// WarpLeftRight
// Horizontally warp the given points in the given bounding rect between the two paths
//
void WarpLeftRightOn( RArray<RIntPoint>& arPoints, const RIntRect& rcBounds, RPath& left, RPath& right, YRealDimension penWidth )
{
	RRealSize						dim( rcBounds.WidthHeight() );
	RArray<RIntPoint>::YIterator	itrPoint = arPoints.Start();
	while( itrPoint != arPoints.End() )
		{
		YRealDimension	s( YRealDimension((*itrPoint).m_x - rcBounds.m_Left) / dim.m_dx );
		YRealDimension	t( YRealDimension((*itrPoint).m_y - rcBounds.m_Top) / dim.m_dy );
		TpsAssert( t>=0, "Negative distance along path." );
		TpsAssert( t<=1.0, "Distance along path greater than path length." );
		RRealPoint	ptLft( left.PointOnPath(t) );
		RRealPoint	ptRgt( right.PointOnPath(t) );
		ptLft.m_x	+= penWidth;
		ptRgt.m_x	-= penWidth;
		(*itrPoint)	= RIntPoint( RRealPoint(ptLft.m_x*(1.0-s) + ptRgt.m_x*s, ptLft.m_y*(1.0-s) + ptRgt.m_y*s) );
		++itrPoint;
		}
}

//
// WarpTopBottom
// Vertically warp the given points in the given bounding rect between the two paths
//
// Note: When warping to an envelope, the margin inset
// is applied before the warp.  Applying it elsewhere
// will add further distortion to the graphic so that it 
// will no longer match the warp envelope.  Below is an 
// example of the distortion that occurs by insetting the 
// margin rect after the warp process.
//
//        *-----------------------*  
//       /|                      /|
//      / +                     / +
//     /                       /
//  + /                     + /
//  |/                      |/
//  *-----------------------*
//
// * - is the points in the warp paths
// + - is the points after being scaled to the inset outline rect.
//
// Notice the difference in the angles between the warp points
// and the inset points.
//
void WarpTopBottomAt( RArray<RRealPoint>& arPoints, const RRealRect& rcBounds, RPath& top, RPath& bottom, YRealDimension penWidth, BOOLEAN fRecurse )
{
	const YRealDimension kMaxSExit = 1.01 ;

	RArray<RRealPoint> arOrigPoints( arPoints ) ;
	YRealDimension sExit = kMaxSExit ; 

	RRealPoint	ptTopStart( top.GetStartPoint() );
	RRealPoint	ptBotStart( bottom.GetStartPoint() );
	RRealPoint	ptTopEnd( top.GetEndPoint() );
	RRealPoint	ptBotEnd( bottom.GetEndPoint() );
	RRealSize	topDelta( ptTopEnd - ptTopStart ) ;
	RRealSize	botDelta( ptBotEnd - ptBotStart );
	RRealSize	dim( rcBounds.WidthHeight() );

	RArray<RRealPoint>::YIterator	itrPoint = arPoints.Start();

	//
	// The pen width value needs to be increased in the x 
	// direction if there is any skewing in the x direction.
	//
	//           /----------------------------------------
	//          /                                        
	//         /\                                       
	//        /   P         /---------------------     
	//       /_phi/ e      /                          
	//      /         n   /                          
	//     /----xPen----\/                          
	//    /             /---------------------
	//   / \                                       
	//  /   | theta                               
	// /---------------------------------------
	//
	YAngle theta = atan2( ptBotStart.m_y - ptTopStart.m_y, ptTopStart.m_x - ptBotStart.m_x ) ;
	YAngle phi = (kPI / 2) - theta ;
	YRealDimension xPenWidth = penWidth / cos( phi ) ;
	if (xPenWidth < 0.0) xPenWidth = -xPenWidth ;

	// Determine new starting x positions
	ptTopStart.m_x += xPenWidth ; ptTopEnd.m_x -= xPenWidth ;
	ptBotStart.m_x += xPenWidth ;	ptBotEnd.m_x -= xPenWidth ;

	// 
	// Move the starting points along until the y delta between
	// the two paths is great enough to contain the margins.
	//
	ptTopStart = top.PointAtX( ptTopStart.m_x );
	ptBotStart = bottom.PointAtX( ptBotStart.m_x );

	while (ptTopStart.m_y + penWidth > ptBotStart.m_y - penWidth)
	{
		ptTopStart.m_x += 0.01 * topDelta.m_dx;
		ptBotStart.m_x += 0.01 * botDelta.m_dx;

		if (ptTopStart.m_x > ptTopEnd.m_x || ptBotStart.m_x > ptBotEnd.m_x)
		{
			// The paths cannot contain the specified margins, so
			// create a new top warp path that is equal to the bottom
			// path but offset vertically the margin distance.
			R2dTransform xform ;
			xform.PostTranslate( 0, -penWidth * 2 );
			RPath rPath = RPath( bottom, xform );

			// Go recursive with new path data
			WarpTopBottomAt( arPoints, rcBounds, rPath, bottom, penWidth ) ;
			return ;
		}
		
		ptTopStart = top.PointAtX( ptTopStart.m_x );
		ptBotStart = bottom.PointAtX( ptBotStart.m_x );
	}

	// Determine new path deltas.
	topDelta = ptTopEnd - ptTopStart;
	botDelta = ptBotEnd - ptBotStart;

	while( itrPoint != arPoints.End() )
		{
		RRealPoint& ptRef = *itrPoint ;
		YRealDimension	s( YRealDimension(ptRef.m_x - rcBounds.m_Left) / dim.m_dx );
		YRealDimension	t( YRealDimension(ptRef.m_y - rcBounds.m_Top) / dim.m_dy );

		TpsAssert( s>=0, "Negative distance along path." );
		TpsAssert( s<=1.0, "Distance along path greater than path length." );
		RRealPoint	ptTop( top.PointAtX( ptTopStart.m_x + ( s * topDelta.m_dx ) ) );
		RRealPoint	ptBot( bottom.PointAtX( ptBotStart.m_x + ( s * botDelta.m_dx ) ) );
		
		// Adjust t to account for a pen width margin by mapping the
		// current value (0.0 - 1.0) into a value between cyMin - cyMax.
		YRealDimension cyMin( penWidth / (ptBot.m_y - ptTop.m_y) ), cyMax( 1.0 - cyMin );
		YRealDimension ty = (cyMin * (1.0 - t)) + (cyMax * t) ;

		ptRef.m_x = ptTop.m_x * ( 1.0 - ty ) + ptBot.m_x * ty ; // Warp in the x direction
		ptRef.m_y = ptTop.m_y * ( 1.0 - ty ) + ptBot.m_y * ty ; // Warp in the y direction

		// If the point has crossed either path, we need to save the s
		// point so we can later determine where the intersection 
		// occured. Note: 0.001 is used as a fudge factor when comparing 
		// the new point to the path bounds to account for floating 
		// point equality.
		if (s < sExit && (ptRef.m_y - 0.001 > ptBot.m_y - penWidth || ptRef.m_y + 0.001 < ptTop.m_y + penWidth))
			{
			sExit = s ;
			}

		++itrPoint;
		}

	if (fRecurse && sExit < kMaxSExit)
	{
		//
		// The point has crossed the path so we need to adjust 
		// the bounding rect and recursively call this function
		// with the new data.  To get the new bounding box, we need
		// to increase it's width so at this s point, we will still
		// be in bounds.
		//
		RRealPoint ptTop, ptBot;

		do 
		{
			sExit -= 0.01;
			ptTop = top.PointAtX( ptTopStart.m_x + ( sExit * topDelta.m_dx ) );
			ptBot = bottom.PointAtX( ptBotStart.m_x + ( sExit * botDelta.m_dx ) );

		} while (sExit > 0.0 && ptTop.m_y + penWidth > ptBot.m_y - penWidth) ;
		
		// Use smallest value that is greater then 0
		if (sExit < 0.00) sExit += 0.01;

		TpsAssert( sExit > 0.0, "Invalid path intersect value!" ) ;
		rcBounds.m_Right = rcBounds.m_Left + rcBounds.Width() / sExit ;
		WarpTopBottomAt( arOrigPoints, rcBounds, top, bottom, penWidth, FALSE ) ;
		arPoints = arOrigPoints;
	}
}

//
// WarpLeftRight
// Horizontally warp the given points in the given bounding rect between the two paths
//
// Note: When warping to an envelope, the margin inset
// is applied before the warp.  Applying it elsewhere
// will add further distortion to the graphic so that it 
// will no longer match the warp envelope.  Below is an 
// example of the distortion that occurs by insetting the 
// margin rect after the warp process.
//
//        *-----------------------*  
//       /|                      /|
//      / +                     / +
//     /                       /
//  + /                     + /
//  |/                      |/
//  *-----------------------*
//
// * - is the points in the warp paths
// + - is the points after being scaled to the inset outline rect.
//
// Notice the difference in the angles between the warp points
// and the inset points.
//
void WarpLeftRightAt( RArray<RRealPoint>& arPoints, const RRealRect& rcBounds, RPath& left, RPath& right, YRealDimension penWidth, BOOLEAN fRecurse )
{
	const YRealDimension kMaxTExit = 1.01 ;

	RArray<RRealPoint> arOrigPoints( arPoints ) ;
	YRealDimension tExit = kMaxTExit ; 

	RRealPoint	ptLeftStart( left.GetStartPoint() );
	RRealPoint	ptRightStart( right.GetStartPoint() );
	RRealPoint	ptLeftEnd( left.GetEndPoint() );
	RRealPoint	ptRightEnd( right.GetEndPoint() );
	RRealSize	leftDelta( ptLeftEnd - ptLeftStart );
	RRealSize	rightDelta( ptRightEnd - ptRightStart );
	RRealSize	dim( rcBounds.WidthHeight() );

	//
	// The pen width value needs to be increased in the y 
	// direction if there is any skewing in the y direction.
	//
	// ^
	// |           /----------------------------------------
	// |          /                                        
	// |         /\                                       
	// |        /   P         /---------------------     
	// |       /_phi/ e      /                          
	// |      /         n   /                          
	// |     /----yPen----\/                          
	// |    /             /---------------------
	// |   / \                                       
	// |  /   | theta                               
	// | /---------------------------------------
	// x
	//   y-------------------------------------------------->
	//
	YAngle theta = atan2( ptRightStart.m_y - ptLeftStart.m_y, ptLeftStart.m_x - ptRightStart.m_x ) ;
	YAngle phi = (kPI / 2) - theta ;
	YRealDimension yPenWidth = penWidth / sin( phi ) ;
	if (yPenWidth < 0.0) yPenWidth = -yPenWidth ;

	// Determine new starting x positions
	ptLeftStart.m_y  += yPenWidth ; ptLeftEnd.m_y  -= yPenWidth ;
	ptRightStart.m_y += yPenWidth ; ptRightEnd.m_y -= yPenWidth ;

	// 
	// Move the starting points along until the x delta between
	// the two paths is great enough to contain the margins.
	//
	ptLeftStart  = left.PointAtY( ptLeftStart.m_y );
	ptRightStart = right.PointAtY( ptRightStart.m_y );
	
	while (ptLeftStart.m_x + penWidth > ptRightStart.m_x - penWidth)
	{
		ptLeftStart.m_y += 0.01 * leftDelta.m_dy;
		ptRightStart.m_y += 0.01 * rightDelta.m_dy;

		if (ptLeftStart.m_y > ptLeftEnd.m_y || ptRightStart.m_y > ptRightEnd.m_y)
		{
			// The paths cannot contain the specified margins, so
			// create a new right warp path that is equal to the left
			// path but offset horizontally the margin distance.
			R2dTransform xform ;
			xform.PostTranslate( penWidth * 2, 0 );
			RPath rPath = RPath( left, xform );

			// Go recursive with new path data
			WarpLeftRightAt( arPoints, rcBounds, left, rPath, penWidth ) ;
			return ;
		}
		
		ptLeftStart = left.PointAtY( ptLeftStart.m_y );
		ptRightStart = right.PointAtY( ptRightStart.m_y );
	}

	// Determine new path deltas.
	leftDelta = ptLeftEnd - ptLeftStart;
	rightDelta = ptRightEnd - ptRightStart;

	
	RArray<RRealPoint>::YIterator	itrPoint = arPoints.Start();

	while( itrPoint != arPoints.End() )
		{
		RRealPoint& ptRef = *itrPoint ;
		YRealDimension	s( YRealDimension( ptRef.m_x - rcBounds.m_Left) / dim.m_dx );
		YRealDimension	t( YRealDimension( ptRef.m_y - rcBounds.m_Top) / dim.m_dy );

		TpsAssert( t>=0, "Negative distance along path." );
		TpsAssert( t<=1.0, "Distance along path greater than path length." );
		RRealPoint	ptLft( left.PointAtY( ptLeftStart.m_y + ( t * leftDelta.m_dy ) ) );
		RRealPoint	ptRgt( right.PointAtY( ptRightStart.m_y + ( t * rightDelta.m_dy ) ) );

		// Adjust s to account for a pen width margin by mapping the
		// current value (0.0 - 1.0) into a value between cxMin - cxMax.
		YRealDimension cxMin( penWidth / (ptRgt.m_x - ptLft.m_x) ), cxMax( 1.0 - cxMin );
		YRealDimension sx = (cxMin * (1.0 - s)) + (cxMax * s) ;

		ptRef.m_x = ptLft.m_x * ( 1.0 - sx ) + ptRgt.m_x * sx;
		ptRef.m_y = ptLft.m_y * ( 1.0 - sx ) + ptRgt.m_y * sx;

		// If the point has crossed either path, we need to save the t
		// point so we can later determine where the intersection 
		// occured. Note: 0.001 is used as a fudge factor when comparing 
		// the new point to the path bounds to account for floating 
		// point equality.
		if (t < tExit && (ptRef.m_x - 0.001 > ptRgt.m_x - penWidth || ptRef.m_x + 0.001 < ptLft.m_x + penWidth))
			tExit = t ;

		++itrPoint;
		}

	if (fRecurse && tExit < kMaxTExit)
	{
		//
		// The point has crossed the path so we need to adjust 
		// the bounding rect and recursively call this function
		// with the new data.  To get the new bounding box, we need
		// to increase it's height so at this t point, we will still
		// be in bounds.
		//
		RRealPoint ptLeft, ptRight;

		do 
		{
			tExit -= 0.01;
			ptLeft = left.PointAtY( ptLeftStart.m_y + ( tExit * leftDelta.m_dy ) );
			ptRight = right.PointAtY( ptRightStart.m_y + ( tExit * rightDelta.m_dy ) );

		} while (tExit > 0.0 && ptLeft.m_x + penWidth > ptRight.m_x - penWidth) ;
		
		// Use smallest value that is greater then 0
		if (tExit < 0.00) tExit += 0.01;

		rcBounds.m_Bottom = rcBounds.m_Top + rcBounds.Height() / tExit ;
		WarpLeftRightAt( arOrigPoints, rcBounds, left, right, penWidth, FALSE ) ;
		arPoints = arOrigPoints;
	}
}

//
// WarpTopBottom
// Vertically warp the given points in the given bounding rect between the two paths
//
void WarpTopBottomAt( RArray<RIntPoint>& arPoints, const RIntRect& rcBounds, RPath& top, RPath& bottom, YRealDimension penWidth )
{
	RRealPoint	ptTopStart( top.GetStartPoint() );
	RRealSize	topDelta( top.GetEndPoint() - ptTopStart );
	RRealPoint	ptBotStart( bottom.GetStartPoint() );
	RRealSize	botDelta( bottom.GetEndPoint() - ptBotStart );
	RRealSize	dim( rcBounds.WidthHeight() );
	RArray<RIntPoint>::YIterator	itrPoint = arPoints.Start();
	while( itrPoint != arPoints.End() )
		{
		YRealDimension	s( YRealDimension((*itrPoint).m_x - rcBounds.m_Left) / dim.m_dx );
		YRealDimension	t( YRealDimension((*itrPoint).m_y - rcBounds.m_Top) / dim.m_dy );
		TpsAssert( s>=0, "Negative distance along path." );
		TpsAssert( s<=1.0, "Distance along path greater than path length." );
		RRealPoint	ptTop( top.PointAtX( ptTopStart.m_x + ( s * topDelta.m_dx ) ) );
		RRealPoint	ptBot( bottom.PointAtX( ptBotStart.m_x + ( s * botDelta.m_dx ) ) );
		ptTop.m_y	+= penWidth;
		ptBot.m_y	-= penWidth;
		(*itrPoint)	= RIntPoint( RRealPoint( ( ptTop.m_x * ( 1.0 - t ) + ptBot.m_x * t ), ( ptTop.m_y * ( 1.0 - t ) + ptBot.m_y * t ) ) );
		++itrPoint;
		}
}

//
// WarpLeftRight
// Horizontally warp the given points in the given bounding rect between the two paths
//
void WarpLeftRightAt( RArray<RIntPoint>& arPoints, const RIntRect& rcBounds, RPath& left, RPath& right, YRealDimension penWidth )
{
	RRealPoint	ptLeftStart( left.GetStartPoint() );
	RRealSize	leftDelta( left.GetEndPoint() - ptLeftStart );
	RRealPoint	ptRightStart( right.GetStartPoint() );
	RRealSize	rightDelta( right.GetEndPoint() - ptRightStart );
	RRealSize	dim( rcBounds.WidthHeight() );
	RArray<RIntPoint>::YIterator	itrPoint = arPoints.Start();
	while( itrPoint != arPoints.End() )
		{
		YRealDimension	s( YRealDimension((*itrPoint).m_x - rcBounds.m_Left) / dim.m_dx );
		YRealDimension	t( YRealDimension((*itrPoint).m_y - rcBounds.m_Top) / dim.m_dy );
		TpsAssert( t>=0, "Negative distance along path." );
		TpsAssert( t<=1.0, "Distance along path greater than path length." );
		RRealPoint	ptLft( left.PointAtY( ptLeftStart.m_y + ( t * leftDelta.m_dy ) ) );
		RRealPoint	ptRgt( right.PointAtY( ptRightStart.m_y + ( t * rightDelta.m_dy ) ) );
		ptLft.m_x	+= penWidth;
		ptRgt.m_x	-= penWidth;
		(*itrPoint)	= RIntPoint( RRealPoint( ( ptLft.m_x * ( 1.0 - s ) + ptRgt.m_x * s ), ( ptLft.m_y * ( 1.0 - s ) + ptRgt.m_y * s ) ) );
		++itrPoint;
		}
}

