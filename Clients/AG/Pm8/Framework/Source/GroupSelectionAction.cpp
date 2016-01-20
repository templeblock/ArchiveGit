// ****************************************************************************
//
//  File Name:			GroupSelectionAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RGroupSelectionAction class
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
//  $Logfile:: /PM8/Framework/Source/GroupSelectionAction.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "GroupSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include "ApplicationGlobals.h"
#include "ComponentDocument.h"
#include "StandaloneApplication.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentTypes.h"
#include "ComponentManager.h"

YActionId	RGroupSelectionAction::m_ActionId( "RGroupSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RGroupSelectionAction::RGroupSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGroupSelectionAction::RGroupSelectionAction( RCompositeSelection* pCurrentSelection )
	: RUndoableAction( m_ActionId, STRING_UNDO_GROUP_SELECTION, STRING_REDO_GROUP_SELECTION ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_OldZOrder( pCurrentSelection->GetView( )->GetZOrderState( ) )
	{
	// Validate parameters
	TpsAssertValid( pCurrentSelection );

	//	Search the m_OldSelection and remove any components that are not in the kNormalZLayer
	YSelectionIterator	iterator		= m_OldSelection.Start( );
	YSelectionIterator	iteratorEnd	= m_OldSelection.End( );
	for ( ; iterator != iteratorEnd; )
		{
		RComponentView*	pView	= (*iterator);
		if ( pView->GetComponentAttributes().GetZLayer() != kNormalZLayer )
			{
			m_OldSelection.Unselect( pView, FALSE );
			//	Something remove, so reset and start over
			iterator		= m_OldSelection.Start( );
			iteratorEnd	= m_OldSelection.End( );
			}
		else	//	Valid so advance
			++iterator;
		}

	// Sort the selection by z-order first
	m_OldSelection.SortByZOrder( );

	// Get the bounding rect of the selected objects
	m_GroupBoundingRect = RComponentCollection( m_OldSelection ).GetBoundingRect( );

	// Get the parent document
	m_pParentDocument = pCurrentSelection->GetView( )->GetRDocument( );

	// Create a new group component
	m_pGroupDocument = ::GetComponentManager( ).CreateNewComponent( kGroupComponent,
																						 m_pParentDocument,
																						 pCurrentSelection->GetView( ),
																						 RComponentAttributes::GetDefaultComponentAttributes( kGroupComponent ),
																						 m_GroupBoundingRect,
																						 FALSE );

	// If we couldnt create the component, throw a memory exception
	if( !m_pGroupDocument )
		::ThrowException( kMemory );
	}

// ****************************************************************************
//
//  Function Name:	RGroupSelectionAction::RGroupSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGroupSelectionAction::RGroupSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: RUndoableAction( m_ActionId, script ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_OldZOrder( pCurrentSelection->GetView( )->GetZOrderState( ) )
	{
	}

// ****************************************************************************
//
//  Function Name:	RGroupSelectionAction::~RGroupSelectionAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGroupSelectionAction::~RGroupSelectionAction( )
	{
	if( (m_asLastAction == kActionUndo ) || (m_asLastAction == kActionNotDone) )
		delete m_pGroupDocument;
	}

// ****************************************************************************
//
//  Function Name:	RGroupSelectionAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGroupSelectionAction::Do( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	// Bail if the component was'nt created properly
	if( !m_pGroupDocument )
		return FALSE;																			

	// Get the z position of the top most component to use  as the group components z-position
	RView* pParentView = m_pCurrentSelection->GetView( );
	YSelectionIterator iterator = m_OldSelection.Start( );
	YComponentZPosition groupZPosition = pParentView->GetComponentZPosition( *iterator );

	// Make a component collection
	RComponentCollection componentCollection ( m_OldSelection );

	// Unselect the current selection
	m_pCurrentSelection->UnselectAll( );

	// Add the group component
	m_pParentDocument->AddComponent( m_pGroupDocument );

	// Transfer the components to the group
	m_pGroupDocument->TransferComponents( m_pParentDocument,
													  componentCollection,
													  componentCollection.GetBoundingRect( ),
													  RRealRect( m_GroupBoundingRect.WidthHeight( ) ) );

	// Combine the component attributes of the collection with that of the group
	RComponentAttributes attributes = m_pGroupDocument->GetComponentAttributes( );
	componentCollection.CombineAttributes( attributes );
	m_pGroupDocument->SetComponentAttributes( attributes );

	// Set the z position of the group component
	pParentView->SetComponentZPosition( m_pGroupDocument->GetView( pParentView ), groupZPosition );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Select the group component
	m_pCurrentSelection->Select( m_pGroupDocument, TRUE );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RGroupSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupSelectionAction::Undo( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Undo( );

	// Make sure the component was created properly
	TpsAssert( m_pGroupDocument, "No group document" );

	// Remove the current selection
	m_pCurrentSelection->Remove( );

	// Make a component collection
	RComponentCollection componentCollection ( m_OldSelection );

	// Transfer the components out of the group
	m_pParentDocument->TransferComponents( m_pGroupDocument,
														componentCollection,
														componentCollection.GetBoundingRect( ),
														m_GroupBoundingRect );

	// Remove the group component
	m_pParentDocument->RemoveComponent( m_pGroupDocument );

	// Restore the Z-order
	m_pCurrentSelection->GetView( )->SetZOrderState( m_OldZOrder );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Reset the current selection and render it
	*m_pCurrentSelection = m_OldSelection;
	m_pCurrentSelection->Render( );
	}

// ****************************************************************************
//
//  Function Name:	RGroupSelectionAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGroupSelectionAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RGroupSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupSelectionAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RGroupSelectionAction, this );
	}

#endif	//	TPSDEBUG
