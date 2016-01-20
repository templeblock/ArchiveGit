// ****************************************************************************
//
//  File Name:			OrderSelectionActions.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the order selection action classes
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
//  $Logfile:: /PM8/Framework/Source/OrderSelectionActions.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"OrderSelectionActions.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentAttributes.h"

YActionId RBringSelectionForwardAction::m_ActionId( "RBringSelectionForwardAction" );
YActionId RSendSelectionBackwardAction::m_ActionId( "RSendSelectionBackwardAction" );
YActionId RBringSelectionToFrontAction::m_ActionId( "RBringSelectionToFrontAction" );
YActionId RSendSelectionToBackAction::m_ActionId( "RSendSelectionToBackAction" );

// ****************************************************************************
//
//  Function Name:	ROrderSelectionActionBase::ROrderSelectionActionBase( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
ROrderSelectionActionBase::ROrderSelectionActionBase( RCompositeSelection* pCurrentSelection,
																		const YActionId& actionId,
																		uWORD uwUndoId,
																		uWORD uwRedoId )

	: RUndoableAction( actionId, uwUndoId, uwRedoId ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( pCurrentSelection->GetView( ) )
	{
	// Sort the selection
	m_pCurrentSelection->SortByZOrder( );

	// Save the selection
	m_OldSelection = *pCurrentSelection;

	// Save the z-order
	m_OldZOrder = m_OldSelection.GetView( )->GetZOrderState( );
	}

// ****************************************************************************
//
//  Function Name:	ROrderSelectionActionBase::ROrderSelectionActionBase( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
ROrderSelectionActionBase::ROrderSelectionActionBase( RCompositeSelection* pCurrentSelection,
																		const YActionId& actionId,
																		RScript& script )
	: RUndoableAction( actionId, script ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( pCurrentSelection->GetView( ) )
	{
	// Sort the selection
	m_pCurrentSelection->SortByZOrder( );

	// Save the selection
	m_OldSelection = *pCurrentSelection;

	// Save the z-order
	m_OldZOrder = m_OldSelection.GetView( )->GetZOrderState( );
	}

// ****************************************************************************
//
//  Function Name:	ROrderSelectionActionBase::Do( )
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
BOOLEAN ROrderSelectionActionBase::Do( )
	{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Do the ordering
	DoOrdering( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	ROrderSelectionActionBase::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void ROrderSelectionActionBase::Undo( )
	{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection
	*m_pCurrentSelection = m_OldSelection;

	// Restore the z-order
	m_pCurrentSelection->GetView( )->SetZOrderState( m_OldZOrder );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate(  );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );
	}

// ****************************************************************************
//
//  Function Name:	ROrderSelectionActionBase::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void ROrderSelectionActionBase::Redo( )
	{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection
	*m_pCurrentSelection = m_OldSelection;

	RUndoableAction::Redo( );
	}

// ****************************************************************************
//
//  Function Name:	ROrderSelectionActionBase::WriteScript( )
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
BOOLEAN ROrderSelectionActionBase::WriteScript( RScript& ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RBringSelectionForwardAction::RBringSelectionForwardAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBringSelectionForwardAction::RBringSelectionForwardAction( RCompositeSelection* pSelection ) :
	ROrderSelectionActionBase( pSelection, m_ActionId, STRING_UNDO_BRING_FORWARD, STRING_REDO_BRING_FORWARD )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RBringSelectionForwardAction::RBringSelectionForwardAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBringSelectionForwardAction::RBringSelectionForwardAction( RCompositeSelection* pSelection, RScript& script ) :
	ROrderSelectionActionBase( pSelection, m_ActionId, script )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RBringSelectionForwardAction::DoOrdering( )
//
//  Description:		Called to do the actual ordering
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBringSelectionForwardAction::DoOrdering( )
	{
	// Get the parent view
	RView* pParentView = m_pCurrentSelection->GetView( );

	// Get the objects that can move forward
	YComponentViewCollection collection;
	pParentView->GetComponentsToBringForward( collection );

	// Iterate the collection moving each object
	YComponentViewIterator iterator = collection.Start( );
	for( ; iterator != collection.End( ); ++iterator )
		pParentView->BringForward( *iterator );
	}

// ****************************************************************************
//
//  Function Name:	RSendSelectionBackwardAction::RSendSelectionBackwardAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSendSelectionBackwardAction::RSendSelectionBackwardAction( RCompositeSelection* pSelection ) :
	ROrderSelectionActionBase( pSelection, m_ActionId, STRING_UNDO_SEND_BACKWARD, STRING_REDO_SEND_BACKWARD )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSendSelectionBackwardAction::RSendSelectionBackwardAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSendSelectionBackwardAction::RSendSelectionBackwardAction( RCompositeSelection* pSelection, RScript& script ) :
	ROrderSelectionActionBase( pSelection, m_ActionId, script )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSendSelectionBackwardAction::DoOrdering( )
//
//  Description:		Called to do the actual ordering
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSendSelectionBackwardAction::DoOrdering( )
	{
	// Get the parent view
	RView* pParentView = m_pCurrentSelection->GetView( );

	// Get the objects that can move forward
	YComponentViewCollection collection;
	pParentView->GetComponentsToSendBackward( collection );

	// Iterate the collection moving each object
	YComponentViewIterator iterator = collection.Start( );
	for( ; iterator != collection.End( ); ++iterator )
		pParentView->SendBackward( *iterator );
	}

// ****************************************************************************
//
//  Function Name:	RBringSelectionToFrontAction::RBringSelectionToFrontAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBringSelectionToFrontAction::RBringSelectionToFrontAction( RCompositeSelection* pSelection ) :
	ROrderSelectionActionBase( pSelection, m_ActionId, STRING_UNDO_BRING_TOFRONT, STRING_REDO_BRING_TOFRONT )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RBringSelectionToFrontAction::RBringSelectionToFrontAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBringSelectionToFrontAction::RBringSelectionToFrontAction( RCompositeSelection* pSelection, RScript& script ) :
	ROrderSelectionActionBase( pSelection, m_ActionId, script )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RBringSelectionToFrontAction::DoOrdering( )
//
//  Description:		Called to do the actual ordering
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBringSelectionToFrontAction::DoOrdering( )
	{
	// Get the parent view
	RView* pParentView = m_pCurrentSelection->GetView( );

	// Sort the selection by z-order
	m_pCurrentSelection->SortByZOrder( );

	// Iterate the selection moving each object
	YSelectionIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( !( *iterator )->GetComponentAttributes( ).IsLocked( ) )
			pParentView->BringToFront( *iterator );
	}

// ****************************************************************************
//
//  Function Name:	RSendSelectionToBackAction::RSendSelectionToBackAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSendSelectionToBackAction::RSendSelectionToBackAction( RCompositeSelection* pSelection ) :
	ROrderSelectionActionBase( pSelection, m_ActionId, STRING_UNDO_SEND_TOBACK, STRING_REDO_SEND_TOBACK )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSendSelectionToBackAction::RSendSelectionToBackAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSendSelectionToBackAction::RSendSelectionToBackAction( RCompositeSelection* pSelection, RScript& script ) :
	ROrderSelectionActionBase( pSelection, m_ActionId, script )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSendSelectionToBackAction::DoOrdering( )
//
//  Description:		Called to do the actual ordering
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSendSelectionToBackAction::DoOrdering( )
	{
	// Get the parent view
	RView* pParentView = m_pCurrentSelection->GetView( );

	// Sort the selection by z-order
	m_pCurrentSelection->SortByZOrder( );

	// Iterate the selection moving each object
	YSelectionIterator iterator = m_pCurrentSelection->End( );
	do
		{
		--iterator;
		if( !( *iterator )->GetComponentAttributes( ).IsLocked( ) )
			pParentView->SendToBack( *iterator );
		}
	while( iterator != m_pCurrentSelection->Start( ) );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	ROrderSelectionActionBase::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void ROrderSelectionActionBase::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( ROrderSelectionActionBase, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
