// ****************************************************************************
//
//  File Name:			ChainedUndoableAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RUndoableAction class
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
//  $Logfile:: /PM8/Framework/Source/ChainedUndoableAction.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "ChainedUndoableAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "UndoManager.h"

// ****************************************************************************
//
//  Function Name:	RChainedUndoableAction::RChainedUndoableAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChainedUndoableAction::RChainedUndoableAction( const YActionId& actionId, uWORD uwUndoId, uWORD uwRedoId )
	: RUndoableAction( actionId, uwUndoId, uwRedoId ),
	  m_pChainedAction( NULL )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RChainedUndoableAction::RChainedUndoableAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChainedUndoableAction::RChainedUndoableAction( const YActionId& actionId, uWORD uwUndoId, uWORD uwRedoId, RChainedUndoableAction* pChainedAction )
	: RUndoableAction( actionId, uwUndoId, uwRedoId ),
	  m_pChainedAction( pChainedAction )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RChainedUndoableAction::RChainedUndoableAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChainedUndoableAction::RChainedUndoableAction( const YActionId& actionId, RScript& script )
	: RUndoableAction( actionId, script ),
	  m_pChainedAction( NULL )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RChainedUndoableAction::~RChainedUndoableAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChainedUndoableAction::~RChainedUndoableAction( )
	{
	if( m_pChainedAction )
		{
		//	if the undo managers was never set, this was within the same document.
		//		just delete the action. otherwise just NULL out the chained action reference
		//		back to this action.
		if ( m_pChainedAction->m_pUndoManager == NULL )
			delete m_pChainedAction;
		else
			m_pChainedAction->m_pChainedAction = NULL;
		}
	}

// ****************************************************************************
//
//  Function Name:	RChainedUndoableAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChainedUndoableAction::Undo( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Undo( );

	// If we have a chained action, and it is the next action to be undone, tell
	// its undo manager to undo it
	if ( m_pChainedAction )
		{
		//	If the undo managers was not initialized, the documents are the same so just call Undo
		if ( m_pChainedAction->m_pUndoManager == NULL )
			m_pChainedAction->Undo( );
		else if( m_pChainedAction->GetState( ) != RUndoableAction::kActionUndo && m_pChainedAction->m_pUndoManager->GetNextActionToBeUndone( ) == m_pChainedAction )
			m_pChainedAction->m_pUndoManager->UndoAction( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RChainedUndoableAction::Redo( )
//
//  Description:		Redoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChainedUndoableAction::Redo( )
	{
	m_asLastAction = kActionRedo;

	// If we have a chained action, and it is the next action to be redone, tell
	// its undo manager to redo it
	if ( m_pChainedAction )
		{
		//	if the undo managers was never set, this was within the same document.
		//		just call redo
		if( m_pChainedAction->m_pUndoManager == NULL )
			m_pChainedAction->Redo( );
		else if( m_pChainedAction->GetState( ) != RUndoableAction::kActionRedo && m_pChainedAction->m_pUndoManager->GetNextActionToBeRedone( ) == m_pChainedAction )
			m_pChainedAction->m_pUndoManager->RedoAction( );
		}
	}
