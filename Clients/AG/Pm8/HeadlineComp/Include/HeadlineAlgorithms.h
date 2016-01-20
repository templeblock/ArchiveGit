// ****************************************************************************
//
//  File Name:			HeadlineAlgorithms.h
//
//  Project:			Headine Component
//
//  Author:				R. Hood
//
//  Description:		Declaration of Various Algorithm templates used in
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
//  $Logfile:: /PM8/HeadlineComp/Include/HeadlineAlgorithms.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HEADLINEALGORITHMS_H_
#define		_HEADLINEALGORITHMS_H_

	int 			ComputeBezierZeroSlopeParams( const RRealPoint* pBez, YAngle angRotate, YFloatType* pRoots );
	void			AppendExtrudedBezierPatch( const RRealPoint* pBez, const R2dTransform& xformOutput,
														const R2dTransform& xformOutputExtruded, EOperationArray& arGraphicOps,
														YFillMethodArray& arFillMethods, EPathOperatorArray& arOps,
														RRealPointArray& arPts, YPointCountArray& arPointCounts );
	void			AppendExtrudedBezier( const RRealPoint* pBezPoints, const R2dTransform& xformExtrude,
												 const R2dTransform& xformOutput, EOperationArray& arGraphicOps,
												 YFillMethodArray& arFillMethods, EPathOperatorArray& arOps,
												 RRealPointArray& arPts, YPointCountArray& arPointCounts );

	void 			WarpTopBottomOn( RArray<RRealPoint>& arPoints, const RRealRect& rcBounds, RPath& top, RPath& bottom, YRealDimension penWidth );
	void 			WarpTopBottomAt( RArray<RRealPoint>& arPoints, const RRealRect& rcBounds, RPath& top, RPath& bottom, YRealDimension penWidth, BOOLEAN fRecurse = TRUE );
	void 			WarpLeftRightOn( RArray<RRealPoint>& arPoints, const RRealRect& rcBounds, RPath& left, RPath& right, YRealDimension penWidth );
	void 			WarpLeftRightAt( RArray<RRealPoint>& arPoints, const RRealRect& rcBounds, RPath& left, RPath& right, YRealDimension penWidth, BOOLEAN fRecurse = TRUE );
	void 			WarpTopBottomOn( RArray<RIntPoint>& arPoints, const RIntRect& rcBounds, RPath& top, RPath& bottom, YRealDimension penWidth );
	void 			WarpTopBottomAt( RArray<RIntPoint>& arPoints, const RIntRect& rcBounds, RPath& top, RPath& bottom, YRealDimension penWidth );
	void 			WarpLeftRightOn( RArray<RIntPoint>& arPoints, const RIntRect& rcBounds, RPath& left, RPath& right, YRealDimension penWidth );
	void 			WarpLeftRightAt( RArray<RIntPoint>& arPoints, const RIntRect& rcBounds, RPath& left, RPath& right, YRealDimension penWidth );

#endif		// _HEADLINEALGORITHMS_H_
