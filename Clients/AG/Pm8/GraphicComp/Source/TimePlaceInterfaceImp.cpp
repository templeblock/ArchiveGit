//****************************************************************************
//
// File Name:		TimepiecePlacementInterfaceImp.cpp
//
// Project:			Renaissance Graphic Component
//
// Author:			G. Brown
//
// Description:	Implementation of the Timepiece Placement Interface
//
// Portability:	Platform Independant
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/GraphicComp/Source/TimePlaceInterfaceImp.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "GraphicIncludes.h"

ASSERTNAME

#include "TimePlaceInterfaceImp.h"
#include "GraphicDocument.h"
#include "GraphicView.h"
#include "Psd3TimepieceGraphic.h"

// ****************************************************************************
//
//  Function Name:	RTimepiecePlacementInterfaceImp::RTimepiecePlacementInterfaceImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RTimepiecePlacementInterfaceImp::RTimepiecePlacementInterfaceImp( const RGraphicView* pGraphicView )
	: RTimepiecePlacementInterface( pGraphicView ),
	  m_pGraphicDocument( const_cast<RGraphicDocument*>( pGraphicView->GetGraphicDocument( ) ) ),
	  m_pTimepiece ( dynamic_cast<RPsd3TimepieceGraphic*>( m_pGraphicDocument->GetGraphic( ) ) )
{
	TpsAssert(m_pTimepiece, "Graphic component does not contain a timepiece graphic.");
}

// ****************************************************************************
//
//  Function Name:	RTimepiecePlacementInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RTimepiecePlacementInterfaceImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RGraphicView, pView );
	return new RTimepiecePlacementInterfaceImp( static_cast<const RGraphicView*>( pView ) );
}

// ****************************************************************************
//
//  Function Name:	RTimepiecePlacementInterfaceImp::GetTimepiecePlacement( )
//
//  Description:		Returns the timepiece placement
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
const RRealRect& RTimepiecePlacementInterfaceImp::GetTimepiecePlacement( ) const
{	
	return m_pTimepiece->GetClockRect();
}
