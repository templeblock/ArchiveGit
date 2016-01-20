// ****************************************************************************
//
//  File Name:			BreakGroupAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RBreakGroupAction class
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
//  $Logfile:: /PM8/Framework/Source/BreakGroupAction.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "BreakGroupAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "GroupSelectionAction.h"
#include "ApplicationGlobals.h"
#include "ComponentDocument.h"
#include "StandaloneApplication.h"
#include "ComponentApplication.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentTypes.h"

YActionId	RBreakGroupAction::m_ActionId( "RBreakGroupAction" );

// ****************************************************************************
//
//  Function Name:	RBreakGroupAction::RBreakGroupAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBreakGroupAction::RBreakGroupAction( RCompositeSelection* pCurrentSelection )
	: RUndoableAction( m_ActionId, STRING_UNDO_BREAKGROUP, STRING_REDO_BREAKGROUP ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection )
	{
	// Validate parameters
	TpsAssertValid( pCurrentSelection );
	TpsAssert( m_pCurrentSelection->Count( ) == 1, "Break Group action only operates on one object." );
	TpsAssertIsObject( RComponentDocument, ( *m_pCurrentSelection->Start( ) )->GetRDocument( ) );

	m_pGroupDocument = static_cast<RComponentDocument*>( ( *m_pCurrentSelection->Start( ) )->GetRDocument( ) );
	TpsAssert( m_pGroupDocument->GetComponentType( ) == kGroupComponent, "Not a group." );

	// Get the parent document
	m_pParentDocument = pCurrentSelection->GetView( )->GetRDocument( );

	// Get the bounding rect of the group
	YSelectionBoundingRect boundingRect;
	pCurrentSelection->GetSelectionBoundingRect( boundingRect );
	m_GroupBoundingRect = boundingRect.m_TransformedBoundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RBreakGroupAction::RBreakGroupAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBreakGroupAction::RBreakGroupAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: RUndoableAction( m_ActionId, script ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RBreakGroupAction::~RBreakGroupAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBreakGroupAction::~RBreakGroupAction( )
	{
	if( m_asLastAction == kActionDo || m_asLastAction == kActionRedo )
		delete m_pGroupDocument;
	}

// ****************************************************************************
//
//  Function Name:	RBreakGroupAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RBreakGroupAction::Do( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	// Get the group components view
	m_pGroupView = *m_pCurrentSelection->Start( );

	// Unselect the group component
	m_pCurrentSelection->UnselectAll( );

	RView* pParentView = m_pCurrentSelection->GetView( );

	// Get the z-position of the group
	YComponentZPosition groupZPosition = pParentView->GetComponentZPosition( m_pGroupView );

	// Remove the components from the group document
	YComponentIterator iterator = m_pGroupDocument->GetComponentCollectionStart( );
	while( iterator != m_pGroupDocument->GetComponentCollectionEnd( ) )
		{
		// Get the view that is in the group
		RComponentView* pView = dynamic_cast<RComponentView*>( (*iterator)->GetView( m_pGroupView ) );
		TpsAssert( pView, "Not a component view." );

		// Make a copy of the iterator, as we are going to delete it next
		YComponentIterator tempIterator = iterator;
		++tempIterator;

		// Get the component document
		RComponentDocument* pComponent = *iterator;

		// Remove the component from the group
		m_pGroupDocument->RemoveComponent( pComponent );

		// NULL out the views parent
		pView->SetParentView( NULL );

		// Now add it to its original parent docuemnt
		m_pParentDocument->AddComponent( pComponent );

		// Set the correct z-position
		pParentView->SetComponentZPosition( pComponent->GetView( pParentView ), groupZPosition );

		// Increment the z-position for the next component
		groupZPosition++;

		// Move the view back to the coordinate space of its parent view
		YComponentBoundingRect temp = pView->GetBoundingRect( );
		temp *= m_pGroupView->GetBoundingRect( ).GetTransform( );
		pView->SetBoundingRect( temp );

		// Select the component
		m_pCurrentSelection->Select( pView, TRUE );

		// Reassign the iterator
		iterator = tempIterator;
		}

	// Remove the group component
	m_pParentDocument->RemoveComponent( m_pGroupDocument );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Copy the selection so that we can undo
	m_OldSelection = *m_pCurrentSelection;
							 
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RBreakGroupAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBreakGroupAction::Undo( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Undo( );

	// Add the group component
	m_pParentDocument->AddComponent( m_pGroupDocument );

	// Unselect the current selection
	m_pCurrentSelection->UnselectAll( );

	// Go through all components in the selection
	RCompositeSelection::YIterator iterator = m_OldSelection.Start( );
	for( ; iterator != m_OldSelection.End( ); ++iterator )
		{
		// Get the component document
		RComponentDocument* pComponentDocument = dynamic_cast<RComponentDocument*>( ( *iterator )->GetRDocument( ) );
		TpsAssert( pComponentDocument, "Invalid document." );

		// Remove the component from its current document
		m_pParentDocument->RemoveComponent( pComponentDocument );

		// NULL out the views parent
		( *iterator )->SetParentView( NULL );

		// Add it to the group component
		m_pGroupDocument->AddComponent( pComponentDocument );

		// Move the component back to the groups coordinate space
		R2dTransform transform = m_pGroupView->GetBoundingRect( ).GetTransform( );
		transform.Invert( );
		YComponentBoundingRect temp = ( *iterator )->GetBoundingRect( );
		temp *= transform;
		( *iterator )->SetBoundingRect( temp );
		}

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Select the group component
	m_pCurrentSelection->Select( m_pGroupDocument, TRUE );
	}

// ****************************************************************************
//
//  Function Name:	RBreakGroupAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RBreakGroupAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}


#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RBreakGroupAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBreakGroupAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RBreakGroupAction, this );
	}

#endif	//	TPSDEBUG
