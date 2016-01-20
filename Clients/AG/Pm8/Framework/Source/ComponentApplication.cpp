// ****************************************************************************
//
//  File Name:			ComponentApplication.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RComponentApplication class
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
//  $Logfile:: /PM8/Framework/Source/ComponentApplication.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ComponentApplication.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RComponentApplication::CreateScriptAction( )
//
//  Description:		Try to create an action from the script
//
//  Returns:			The pointer to the action or NULL
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction* RComponentApplication::CreateScriptAction( const YActionId& actionId, RScript& script )
	{
	// Apparently no one wants this command. Return FALSE.
	return RApplication::CreateScriptAction( actionId, script );
	}

// ****************************************************************************
//
//  Function Name:	RComponentApplication::CanPasteDataFormat( )
//
//  Description:		Checks to see if the component type created by this server
//							can paste data of the specified format
//
//  Returns:			TRUE if the data can be pasted
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentApplication::CanPasteDataFormat( YDataFormat, YComponentType& ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentApplication::RComponentApplication( )
//
//  Description:		Determines if this is a non-standard component which should
//							be appended to the component menu.
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentApplication::AppendToComponentMenu( ) const
	{
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RComponentApplication::Validate( )
//
//  Description:		Validate the object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentApplication::Validate( ) const
	{
	RApplication::Validate( );
	TpsAssertIsObject( RComponentApplication, this );
	}

#endif		//	TPSDEBUG
