// ****************************************************************************
//
//  File Name:			ChangeURLAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				Richard Grenfell
//
//  Description:		Declaration of the RChangeURLAction class.  This class
//							provides a swap of the current URL with the one saved.
//
//  Portability:		Platform independent
//
//  Copyright (C) 1998 Brøderbund Software, Inc., All Rights Reserved.
//
//  $Logfile:: $
//   $Author:: $
//     $Date:: $
// $Revision:: $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ChangeURLAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
//#include "FileStream.h"
#include "FrameworkResourceIds.h"
#include "ComponentDocument.h"

YActionId RChangeURLAction::m_ActionId( "RChangeURLAction" );

// ****************************************************************************
//
//  Function Name:	RChangeURLAction::RChangeURLAction()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

RChangeURLAction::RChangeURLAction( RComponentDocument* pComponentDocument, const RURL& rUrl, BOOLEAN bAdd )
: RUndoableAction( m_ActionId, (bAdd) ? STRING_UNDO_ADD_URL : STRING_UNDO_CHANGE_URL, (bAdd) ? STRING_REDO_ADD_URL : STRING_REDO_CHANGE_URL ),
	m_pComponentDoc( pComponentDocument ),
	m_URL( rUrl )
{		
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangeURLAction::RChangeURLAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChangeURLAction::RChangeURLAction( RComponentDocument* pComponentDocument, RScript& script )
: RUndoableAction( m_ActionId, script ),
	m_pComponentDoc( pComponentDocument )
{
#if 0
	script >> m_URL;
#else
	UnimplementedCode();
#endif
}

// ****************************************************************************
//
//  Function Name:	RChangeURLAction::~RChangeURLAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChangeURLAction::~RChangeURLAction( )
{	
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangeURLAction::Do( )
//
//  Description:		Perform the action.
//
//  Returns:			TRUE if action performed sucessfully
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RChangeURLAction::Do()
{	
	// Find out what the current URL is for the object.
	RURL	rUrl( m_pComponentDoc->GetURL() );
	// Set the component to the stored URL.
	m_pComponentDoc->SetURL( m_URL );
	// Keep the old URL for next pass.
	m_URL = rUrl;

	// Since URLs are invisible to the user, no invalidation is necessary.

	// Call the base method
	return RUndoableAction::Do();
}

// ****************************************************************************
//
//  Function Name:	RChangeURLAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeURLAction::Undo()
{
	// We don't do anything special, so just let the Do work for us.
	Do();
	RUndoableAction::Undo( );
}

// ****************************************************************************
//
//  Function Name:	RChangeURLAction::WriteScript( )
//
//  Description:		Write the action data to a script.
//
//  Returns:			TRUE if sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RChangeURLAction::WriteScript( RScript& script ) const
{
	if ( RUndoableAction::WriteScript( script ) )
	{
#if 0	// RIP 10/14/98 - if scriptability is needed << must be written for RURL.
		script << m_URL;
#else
		UnimplementedCode();
#endif
		return TRUE;
	}
	return FALSE;
}
#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RChangeURLAction::Validate( )
//
//  Description:		Verify that the object is valid.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeURLAction::Validate( ) const
{
	RUndoableAction::Validate( );

	// A URL is allowed to be blank, and is never in an "invalid" state, so
	// there is no need to test it.
}

#endif	// TPSDEBUG
