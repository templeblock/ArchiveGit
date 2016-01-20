// ****************************************************************************
//
//  File Name:			Selection.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RSelection class
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
//  $Logfile:: /PM8/Framework/Source/Selection.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "Selection.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "View.h"

// ****************************************************************************
//
//  Function Name:	RSelection::RSelection( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelection::RSelection( RView* pView ) : m_pView( pView )
	{
	;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RSelection::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelection::Validate( )	const
	{
	RObject::Validate( );
	TpsAssertIsObject( RSelection, this );
	TpsAssertValid( m_pView );
	}

#endif	// TPSDEBUG
