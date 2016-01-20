// ****************************************************************************
//
//  File Name:			Object.cpp
//
//  Project:			Renaissance framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Definition of the RObject class
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
//  $Logfile:: /PM8/Framework/Source/Object.cpp                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RObject::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RObject::Validate( ) const
	{
	TpsAssertIsObject( RObject, this );
	TpsAssert( m_DebugTag == kDebugTag, "Invalid RObject." );

	//	On Windows, assert that I can read and write the memory that
	//	this object occupies
	WinDebug( TpsAssert( IsBadReadPtr(this, sizeof(*this))==FALSE, "Cannot read from all of this objects data" ) );
	//	cannot perform the IsBadWritePtr because of the const object class.  Eventually,
	//	assign this to a non const member to test.
	// WinDebug( TpsAssert( IsBadWritePtr(this, sizeof(*this))==FALSE, "Cannot write to all of this objects data" ) );
	}

#endif	// TPSDEBUG
