// ****************************************************************************
//
//  File Name:			FlipSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RFlipSelectionAction class
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
//  $Logfile:: /PM8/Framework/Source/FlipSelectionAction.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"FlipSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentAttributes.h"

YActionId RFlipSelectionHorizontalAction::m_ActionId( "RFlipSelectionHorizontalAction" );
YActionId RFlipSelectionVerticalAction::m_ActionId( "RFlipSelectionVerticalAction" );
YActionId RFlipSelectionBothAction::m_ActionId( "RFlipSelectionBothAction" );

// ****************************************************************************
//
//  Function Name:	RFlipSelectionAction::RFlipSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFlipSelectionAction::RFlipSelectionAction( RCompositeSelection* pCurrentSelection,
														  const YActionId& actionId,
														  uWORD uwUndoId,
														  uWORD uwRedoId )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( actionId, uwUndoId, uwRedoId )
	{
	// Calculate the selection center point
	m_SelectionCenterPoint = pCurrentSelection->GetBoundingRect( ).GetCenterPoint( );
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionAction::RFlipSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFlipSelectionAction::RFlipSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script, const YActionId& actionId )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( actionId, script )
	{
	//	Retrieve the selection and offset
	pCurrentSelection->Read( script );

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionAction::Do( )
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
BOOLEAN RFlipSelectionAction::Do( )
	{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Iterate the selection, flipping each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsFlippable( ) )
			ApplyFlip( *iterator );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFlipSelectionAction::Undo( )
	{
	// Remove the current selection
	m_pCurrentSelection->Invalidate(  );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	Do( );
	m_asLastAction = kActionUndo;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFlipSelectionAction::Redo( )
	{
	// Remove the current selection
	m_pCurrentSelection->Invalidate(  );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	Do( );
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionAction::WriteScript( )
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
BOOLEAN RFlipSelectionAction::WriteScript( RScript& ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionHorizontalAction::RFlipSelectionHorizontalAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFlipSelectionHorizontalAction::RFlipSelectionHorizontalAction( RCompositeSelection* pCurrentSelection )
	: RFlipSelectionAction( pCurrentSelection, m_ActionId, STRING_UNDO_FLIP_HORIZONTAL, STRING_REDO_FLIP_HORIZONTAL )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionHorizontalAction::RFlipSelectionHorizontalAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFlipSelectionHorizontalAction::RFlipSelectionHorizontalAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: RFlipSelectionAction( pCurrentSelection, script, m_ActionId )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionHorizontalAction::ApplyFlip( )
//
//  Description:		Called to apply the flip to the specified component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFlipSelectionHorizontalAction::ApplyFlip( RComponentView* pComponentView ) const
	{
	pComponentView->FlipHorizontal( m_SelectionCenterPoint.m_x );
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionVerticalAction::RFlipSelectionVerticalAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFlipSelectionVerticalAction::RFlipSelectionVerticalAction( RCompositeSelection* pCurrentSelection )
	: RFlipSelectionAction( pCurrentSelection, m_ActionId, STRING_UNDO_FLIP_VERTICAL, STRING_REDO_FLIP_VERTICAL )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionVerticalAction::RFlipSelectionVerticalAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFlipSelectionVerticalAction::RFlipSelectionVerticalAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: RFlipSelectionAction( pCurrentSelection, script, m_ActionId )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionVerticalAction::ApplyFlip( )
//
//  Description:		Called to apply the flip to the specified component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFlipSelectionVerticalAction::ApplyFlip( RComponentView* pComponentView ) const
	{
	pComponentView->FlipVertical( m_SelectionCenterPoint.m_y );
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionBothAction::RFlipSelectionBothAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFlipSelectionBothAction::RFlipSelectionBothAction( RCompositeSelection* pCurrentSelection )
	: RFlipSelectionAction( pCurrentSelection, m_ActionId, STRING_UNDO_FLIP_BOTH, STRING_REDO_FLIP_BOTH )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionBothAction::RFlipSelectionBothAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFlipSelectionBothAction::RFlipSelectionBothAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: RFlipSelectionAction( pCurrentSelection, script, m_ActionId )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RFlipSelectionBothAction::ApplyFlip( )
//
//  Description:		Called to apply the flip to the specified component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFlipSelectionBothAction::ApplyFlip( RComponentView* pComponentView ) const
	{
	pComponentView->FlipHorizontal( m_SelectionCenterPoint.m_x );
	pComponentView->FlipVertical( m_SelectionCenterPoint.m_y );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFlipSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFlipSelectionAction::Validate( ) const
	{
	RUndoableAction::Validate( );
	TpsAssertIsObject( RFlipSelectionAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
