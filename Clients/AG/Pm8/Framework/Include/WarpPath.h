// ****************************************************************************
//
//  File Name:			WarpPath.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Function prototypes for dealing with Warp Paths
//								(warp envelopes, follow paths, text blocks).
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
//  $Logfile:: /PM8/Framework/Include/WarpPath.h                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_WARPPATH_H_
#define		_WARPPATH_H_

#ifndef		_PATH_H_
#include		"Path.h"
#endif

//	Prototypes

FrameworkLinkage BOOLEAN	LoadWarpPathData( sWORD swWarpId, RIntPointArray* pPArray,  EPathOperatorArray* pOArray,
																				 RIntPointArray* pP1Array, EPathOperatorArray* pO1Array,
																				 RIntPointArray* pP2Array, EPathOperatorArray* pO2Array,
																				 uWORD* pOrientation = NULL, uWORD* pPlacement = NULL );

FrameworkLinkage BOOLEAN	LoadFollowPathData( sWORD swWarpId, RIntPointArray* pPArray );


#endif	//	_WARPPPATH_H_

