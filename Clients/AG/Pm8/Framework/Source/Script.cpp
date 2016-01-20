// ****************************************************************************
//
//  File Name:			Script.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RScript class
//
//  Portability:		cross platform
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
//  $Logfile:: /PM8/Framework/Source/Script.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include		"Script.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
// Function Name:		RScript::RScript( )
//
// Description:		ctor;
//
// Returns:				Nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
RScript::RScript( )
	{
	NULL;
	}



// ****************************************************************************
//
// Function Name:		RScript::~RScript( )
//
// Description:		ctor;
//
// Returns:				Nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
RScript::~RScript( )
	{
	NULL;
	}



#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RScript::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RScript::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RScript, this );
	}

#endif	// TPSDEBUG
