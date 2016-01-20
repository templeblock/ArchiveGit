// ****************************************************************************
//
//  File Name:			HeadlinesCanned.h
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Interface to define cannned headline effects
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
//  $Logfile:: /PM8/HeadlineComp/Include/HeadlinesCanned.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_HEADLINESCANNED_H_
#define	_HEADLINESCANNED_H_

class RHeadlineGraphic;

// defines
//enum
//{
//	kWarpPath1 = 3,												// unused
// kWarpPath2,														// unused
//	kWarpPath3,														// unused
//	kWarpPath4	 													// unused
//};

// constants
const uWORD kDefaultHeadlineWidthInInchs = 5;
const uWORD kDefaultHeadlineHeightInInchs = 2;

// prototypes
		BOOLEAN			DefineHeadlineWarp( short, RHeadlineGraphic* );
		BOOLEAN			DefineHeadlinePath( short, RHeadlineGraphic* );

#endif	//	_HEADLINESCANNED_H_
