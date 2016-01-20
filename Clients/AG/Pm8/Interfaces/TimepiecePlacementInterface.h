// ****************************************************************************
//
//  File Name:			TimepiecePlacementInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the RTimepiecePlacementInterface class
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
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Interfaces/TimepiecePlacementInterface.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_TIMEPIECEPLACEMENTINTERFACE_H_
#define	_TIMEPIECEPLACEMENTINTERFACE_H_

#ifndef	_INTERFACE_H_
#include	"Interface.h"
#endif

const	YInterfaceId	kTimepiecePlacementInterfaceId	= 90;

// ****************************************************************************
//
//  Class Name:	RTimepiecePlacementInterface
//
//  Description:	A pure virtual class interfacing with a timepiece graphic
//
// ****************************************************************************
//
class RTimepiecePlacementInterface : public RInterface
	{
	//	Construction & Destruction
	public :
													RTimepiecePlacementInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Operations
	public :
		virtual const RRealRect&			GetTimepiecePlacement( ) const = 0;
	} ;

#endif	//_TIMEPIECEPLACEMENTINTERFACE_H_
