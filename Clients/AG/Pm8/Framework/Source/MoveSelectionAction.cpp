// ****************************************************************************
//
//  File Name:			MoveSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RMoveSelectionActionBase class
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
//  $Logfile:: /PM8/Framework/Source/MoveSelectionAction.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"MoveSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentAttributes.h"

YActionId RMoveSelectionAction::m_ActionId( "RMoveSelectionAction" );
YActionId RNudgeSelectionAction::m_ActionId( "RNudgeSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RMoveSelectionActionBase::RMoveSelectionActionBase( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMoveSelectionActionBase::RMoveSelectionActionBase( RCompositeSelection* pCurrentSelection,
																	 const RRealSize& offset,
																	 const YActionId& actionId )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_Offset( offset ),
	  RUndoableAction( actionId, STRING_UNDO_MOVE_COMPONENT, STRING_REDO_MOVE_COMPONENT )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMoveSelectionAction::RMoveSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMoveSelectionAction::RMoveSelectionAction( RCompositeSelection* pCurrentSelection, const RRealSize& offset )
	: RMoveSelectionActionBase( pCurrentSelection, offset, m_ActionId )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RNudgeSelectionAction::RNudgeSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RNudgeSelectionAction::RNudgeSelectionAction( RCompositeSelection* pCurrentSelection )
	: RMoveSelectionActionBase( pCurrentSelection, RRealSize( 0, 0 ), m_ActionId )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMoveSelectionActionBase::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE	: The action succeeded; script it
//							FALSE	: The action failed, do not script it
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMoveSelectionActionBase::Do( )
	{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Do the move
	DoMove( m_Offset );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RMoveSelectionActionBase::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMoveSelectionActionBase::Undo( )
	{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the current selection
	m_pCurrentSelection->Invalidate(  );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Do the move
	DoMove( RRealSize( -m_Offset.m_dx, -m_Offset.m_dy ) );
	}

// ****************************************************************************
//
//  Function Name:	RMoveSelectionActionBase::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMoveSelectionActionBase::Redo( )
	{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Do the move
	DoMove( m_Offset );
	
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RMoveSelectionActionBase::DoMove( )
//
//  Description:		Actually does the move
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMoveSelectionActionBase::DoMove( const RRealSize& offset ) const
	{
	// Dont do anything if the offset it empty; this will happen when we
	// create a nudge action
	if( offset == RRealSize( 0, 0 ) )
		return;

	// Iterate the selection, moving each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsMovable( ) )
			( *iterator )->Offset( offset );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );
	}

// ****************************************************************************
//
//  Function Name:	RNudgeSelectionAction::Nudge( )
//
//  Description:		Adds a nudge
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RNudgeSelectionAction::Nudge( const RRealSize& nudgeAmount )
	{
	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Move
	DoMove( nudgeAmount );

	// Update the offset
	m_Offset.m_dx += nudgeAmount.m_dx;
	m_Offset.m_dy += nudgeAmount.m_dy;
	}

// ****************************************************************************
//
//  Function Name:	RMoveSelectionActionBase::WriteScript( )
//
//  Description:		Writes the action to a script
//
//  Returns:			TRUE	: The action succeeded; script it
//							FALSE	: The action failed, do not script it
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMoveSelectionActionBase::WriteScript( RScript& script ) const
	{
	if (RUndoableAction::WriteScript( script ))
		{
		m_pCurrentSelection->Write( script );
		script << m_Offset;
		return TRUE;
		}
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RMoveSelectionActionBase::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMoveSelectionActionBase::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( RMoveSelectionActionBase, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
