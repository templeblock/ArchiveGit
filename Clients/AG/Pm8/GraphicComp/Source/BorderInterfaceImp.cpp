// ****************************************************************************
//
//  File Name:			BorderInterfaceImp.cpp
//
//  Project:			Renaissance Graphic Component
//
//  Author:				S. Athanas
//
//  Description:		Implentation of the RBorderInterfaceImp class
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
//  $Logfile:: /PM8/GraphicComp/Source/BorderInterfaceImp.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "GraphicIncludes.h"

ASSERTNAME

#include "BorderInterfaceImp.h"
#include "BorderView.h"
#include "GraphicDocument.h"

// ****************************************************************************
//
//  Function Name:	RBorderInterfaceImp::RBorderInterfaceImp( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBorderInterfaceImp::RBorderInterfaceImp( const RBorderView* pView )
	: RBorderInterface( pView ),
	  m_pBorderView( pView )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RBorderInterfaceImp::SetBorderThickness( )
//
//  Description:		Sets the borders thickness
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBorderInterfaceImp::SetBorderThickness( BOOLEAN fWide )
	{
	const_cast<RBorderView*>(m_pBorderView)->SetWideBorder( fWide );
	}

// ****************************************************************************
//
//  Function Name:	RBorderInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RBorderInterfaceImp::CreateInterface( const RComponentView* pView )
	{
	TpsAssertIsObject( RBorderView, pView );
	return new RBorderInterfaceImp( static_cast<const RBorderView*>( pView ) );
	}
