// ****************************************************************************
//
//  File Name:			SetHeadlineTextAction.cpp
//
//  Project:			Renaissance Headline Component
//
//  Author:				M. Houle
//
//  Description:		Definition of the RSetHeadlineTextAction class
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
//  $Logfile:: /PM8/HeadlineComp/Source/SetHeadlineTextAction.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"HeadlineIncludes.h"

ASSERTNAME

#include "SetHeadlineTextAction.h"
#include "HeadlineDocument.h"
#include "HeadlineCompResource.h"

YActionId	RSetHeadlineTextAction::m_ActionId( "RSetHeadlineTextAction" );

// ****************************************************************************
//
//  Function Name:	RSetHeadlineTextAction::RSetHeadlineTextAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSetHeadlineTextAction::RSetHeadlineTextAction( RHeadlineDocument* pHeadlineDocument, const RMBCString& text )
	: m_pHeadlineDocument( pHeadlineDocument ),
	  m_Text( text ),
	  RUndoableAction( m_ActionId, STRING_UNDO_REPLACETEXT, STRING_REDO_REPLACETEXT )
	{
	;
	}	

// ****************************************************************************
//
//  Function Name:	RSetHeadlineTextAction::RSetHeadlineTextAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSetHeadlineTextAction::RSetHeadlineTextAction( RHeadlineDocument* pHeadlineDocument, RScript& script )
	: m_pHeadlineDocument( pHeadlineDocument ),
	  RUndoableAction( m_ActionId, script )
	{
	NULL;
	}	

// ****************************************************************************
//
//  Function Name:	RSetHeadlineTextAction::~RSetHeadlineTextAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSetHeadlineTextAction::~RSetHeadlineTextAction( )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RSetHeadlineTextAction::Do( )
//
//  Description:		Perform the action.
//
//  Returns:			TRUE: this does nothing other than set the state.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RSetHeadlineTextAction::Do( )
	{
	RMBCString	oldText;
	m_pHeadlineDocument->HeadlineObject( ).GetText( oldText );
	m_pHeadlineDocument->HeadlineObject( ).SetText( m_Text, TRUE );
	m_Text	= oldText;

	m_pHeadlineDocument->InvalidateAllViews( );
	return RUndoableAction::Do( );
	}

// ****************************************************************************
//
//  Function Name:	RSetHeadlineTextAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSetHeadlineTextAction::Undo( )
	{
	Do();
	RUndoableAction::Undo( );
	}

// ****************************************************************************
//
//  Function Name:	RSetHeadlineTextAction::WriteScript( )
//
//  Description:		Write the action data to a script.
//
//  Returns:			TRUE if sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RSetHeadlineTextAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RSetHeadlineTextAction::Validate( )
//
//  Description:		Verify that the object is valid.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSetHeadlineTextAction::Validate( ) const
	{
	RUndoableAction::Validate( );
	}

#endif	// TPSDEBUG
