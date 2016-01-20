// ****************************************************************************
//
//  File Name:			DeleteSelectionAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RDeleteAction class
//
//							REVIEW: If multiple views are ever to be supported, this
//									  action will have to be re-written.
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
//  $Logfile:: /PM8/Framework/Source/DeleteSelectionAction.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "DeleteSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentDocument.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"

YActionId RDeleteSelectionAction::m_ActionId( "RDeleteSelectionAction" );
YActionId RDragDropDeleteSelectionAction::m_ActionId( "RDragDropDeleteSelectionAction" );
YActionId RDeleteComponentAction::m_ActionId( "RDeleteComponentAction" );

// ****************************************************************************
//
//  Function Name:	RDeleteAction::RDeleteAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDeleteAction::RDeleteAction( RCompositeSelection* pCurrentSelection,
										const YActionId& actionId )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_OldZOrder( pCurrentSelection->GetView( )->GetZOrderState( ) ),
	  RUndoableAction( actionId, STRING_UNDO_DELETE, STRING_REDO_DELETE )
	{
	// Validate parameters
	TpsAssertValid( pCurrentSelection );
	}

// ****************************************************************************
//
//  Function Name:	RDeleteAction::~RDeleteAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDeleteAction::~RDeleteAction( )
	{
	// Delete them for good
	if( ( m_asLastAction == kActionDo ) || ( m_asLastAction == kActionRedo ) )
		m_DeletedComponents.DeleteAllComponents( );
	}

// ****************************************************************************
//
//  Function Name:	RDeleteAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDeleteAction::Do( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	m_pCurrentSelection->Invalidate( );

	// Iterate the collection, deleting each object that is not locked
	YComponentIterator componentIterator = m_ComponentsToDelete.Start( );
	for( ; componentIterator != m_ComponentsToDelete.End( ); ++componentIterator )
		{
		RComponentDocument* pComponent = ( *componentIterator );
		TpsAssert( pComponent->GetParentDocument( ) == m_OldSelection.GetView( )->GetRDocument( ), "Invalid parent document" );

		if( ShouldDelete( pComponent ) )
			{
			m_DeletedComponents.InsertAtEnd( pComponent );
			pComponent->GetParentDocument( )->RemoveComponent( pComponent );
			}
		}

	// Unselect anything that was deleted
	componentIterator = m_DeletedComponents.Start( );
	for( ; componentIterator != m_DeletedComponents.End( ); ++componentIterator )
		if( m_pCurrentSelection->IsSelected( static_cast<RComponentView*>( ( *componentIterator )->GetActiveView( ) ) ) )
			m_pCurrentSelection->Unselect( static_cast<RComponentView*>( ( *componentIterator )->GetActiveView( ) ), FALSE );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RDeleteAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeleteAction::Undo( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the current selection
	m_pCurrentSelection->Invalidate( );

	// Undelete the objects
	YComponentIterator iterator = m_DeletedComponents.Start( );
	for( ; iterator != m_DeletedComponents.End( ); ++iterator )
		m_OldSelection.GetView( )->GetRDocument( )->AddComponent( *iterator );

	// Restore the old selection state
	*m_pCurrentSelection = m_OldSelection;

	// Restore the old Z-Order
	m_pCurrentSelection->GetView( )->SetZOrderState( m_OldZOrder );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );

	//	Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	m_DeletedComponents.Empty( );
	}

// ****************************************************************************
//
//  Function Name:	RDeleteAction::Redo( )
//
//  Description:		Redoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeleteAction::Redo( )
	{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	RUndoableAction::Redo( );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );
	}

// ****************************************************************************
//
//  Function Name:	RDeleteAction::ShouldDelete( )
//
//  Description:		Called to determine if the specified component should be
//							deleted
//
//  Returns:			TRUE if the component should be deleted
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDeleteAction::ShouldDelete( RComponentDocument* pComponent ) const
	{
	if( !pComponent->GetComponentAttributes( ).IsDeletable( ) )
		return FALSE;

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RDeleteAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDeleteAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RDeleteSelectionAction::RDeleteSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDeleteSelectionAction::RDeleteSelectionAction( RCompositeSelection* pCurrentSelection )
	: RDeleteAction( pCurrentSelection, m_ActionId )
  	{
	m_ComponentsToDelete = RComponentCollection( *pCurrentSelection );
	}

// ****************************************************************************
//
//  Function Name:	RDragDropDeleteSelectionAction::RDragDropDeleteSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragDropDeleteSelectionAction::RDragDropDeleteSelectionAction( RCompositeSelection* pCurrentSelection )
	: RDeleteAction( pCurrentSelection, m_ActionId )
  	{
	m_ComponentsToDelete = RComponentCollection( *pCurrentSelection );
	}

// ****************************************************************************
//
//  Function Name:	RDeleteComponentAction::RDeleteComponentAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDeleteComponentAction::RDeleteComponentAction( RCompositeSelection* pCurrentSelection,
																RComponentDocument* pComponent )
	: RDeleteAction( pCurrentSelection, m_ActionId )
  	{
	m_ComponentsToDelete.InsertAtEnd( pComponent );
	}

// ****************************************************************************
//
//  Function Name:	RDragDropDeleteSelectionAction::ShouldDelete( )
//
//  Description:		Called to determine if the specified component should be
//							deleted
//
//  Returns:			TRUE if the component should be deleted
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDragDropDeleteSelectionAction::ShouldDelete( RComponentDocument* pComponent ) const
	{
	if( !pComponent->GetComponentAttributes( ).IsDeletable( ) || !pComponent->GetComponentAttributes( ).IsMovable( ) )
		return FALSE;

	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RDeleteAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeleteAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RDeleteAction, this );
	}

#endif	//	TPSDEBUG
