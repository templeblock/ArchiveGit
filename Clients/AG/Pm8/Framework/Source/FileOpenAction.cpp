// ****************************************************************************
//
//  File Name:			FileOpenAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				E. VanHelene
//
//  Description:		Implementation of the RFileOpenAction class
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
//  $Logfile:: /PM8/Framework/Source/FileOpenAction.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "FileOpenAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include "StandaloneDocument.h"
#include "ApplicationGlobals.h"
#include "StandaloneApplication.h"

YActionId	RFileOpenAction::m_ActionId( "RFileOpenAction" );

// ****************************************************************************
//
//  Function Name:	RFileOpenAction::RFileOpenAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RFileOpenAction::RFileOpenAction( const RMBCString& rFilename )
	: RAction( m_ActionId ), m_rFilename( rFilename )
	{ 
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RFileOpenAction::RFileOpenAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RFileOpenAction::RFileOpenAction( RScript& script )
	: RAction( m_ActionId, script )
	{
	script >> m_rFilename;
	}

// ****************************************************************************
//
//  Function Name:	RFileOpenAction::~RFileOpenAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFileOpenAction::~RFileOpenAction( )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RFileOpenAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RFileOpenAction::Do( )
	{
	TpsAssert( !m_rFilename.IsEmpty( ), "invalid filename for new document" )

	try
		{
		// Create a new document
		//RStandaloneDocument* pDocument = ::GetApplication( ).CreateNewDocument( m_rFilename );
		}

	catch( ... )
		{
		// REVIEW: What else do we do here?
		return FALSE;
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RFileOpenAction::GetFileName( )
//
//  Description:		returns the filename file to open
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString& RFileOpenAction::GetFileName( ) 
	{
	return m_rFilename;
	}

// ****************************************************************************
//
//  Function Name:	RFileOpenAction::WriteScript( ) const
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RFileOpenAction::WriteScript( RScript& script ) const
	{
	script << const_cast<RFileOpenAction*>(this)->GetFileName();
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFileOpenAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
///
// ****************************************************************************
//
void RFileOpenAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RFileOpenAction, this );
	}

#endif	//	TPSDEBUG
