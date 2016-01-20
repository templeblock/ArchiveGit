// ****************************************************************************
//
//  File Name:			EditLineAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				G. Brown
//
//  Description:		Definition of the REditLineAction class
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
//  $Logfile:: /PM8/Framework/Source/ChangeTintAction.cpp               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ChangeTintAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"ComponentDocument.h"
#include "ComponentView.h"
#include "FrameworkResourceIds.h"

YActionId RChangeTintAction::m_ActionId( "RChangeTintAction" );

// ****************************************************************************
//
//  Function Name:	RChangeTintAction::RChangeTintAction()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

// REVIEW The strings here should not be bitmap strings
RChangeTintAction::RChangeTintAction( RComponentDocument* pComponentDocument, YTint tint )
	: RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_TINT, STRING_REDO_CHANGE_TINT ),
	  m_pComponentDocument( pComponentDocument ),
	  m_Tint( tint )
{		
	TpsAssert((tint >= kMinTint) && (tint <= kMaxTint), "Invalid tint value.");
}

// ****************************************************************************
//
//  Function Name:	REditLineAction::REditLineAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChangeTintAction::RChangeTintAction(RComponentDocument* pComponentDocument, RScript& script)
	: RUndoableAction(m_ActionId, script),
	  m_pComponentDocument(pComponentDocument)
{	
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangeTintAction::~RChangeTintAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChangeTintAction::~RChangeTintAction( )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangeTintAction::Do( )
//
//  Description:		Perform the action.
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RChangeTintAction::Do()
{	
	YTint	tint	= m_pComponentDocument->GetTint();
	m_pComponentDocument->SetTint(m_Tint);
	m_Tint = tint;
	
	(static_cast< RComponentView* >(m_pComponentDocument->GetActiveView()) )->InvalidateRenderCache( );

	// Call the base method
	return RUndoableAction::Do();
}

// ****************************************************************************
//
//  Function Name:	RChangeTintAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeTintAction::Undo()
{	
	Do( );
	RUndoableAction::Undo( );
}

// ****************************************************************************
//
//  Function Name:	RChangeTintAction::WriteScript( )
//
//  Description:		Write the action data to a script.
//
//  Returns:			TRUE if sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RChangeTintAction::WriteScript( RScript& /* script */ ) const
{
	return TRUE;
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RChangeTintAction::Validate( )
//
//  Description:		Verify that the object is valid.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeTintAction::Validate( ) const
{
	RUndoableAction::Validate( );
}
#endif	// TPSDEBUG
