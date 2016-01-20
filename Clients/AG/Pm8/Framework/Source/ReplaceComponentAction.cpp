// ****************************************************************************
//
//  File Name:			ReplaceComponentAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RReplaceComponentAction class
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
//  $Logfile:: /PM8/Framework/Source/ReplaceComponentAction.cpp               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "ReplaceComponentAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FrameworkResourceIds.h"
#include "CompositeSelection.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ComponentTypes.h"

YActionId RReplaceComponentAction::m_ActionId( "RReplaceComponentAction" );

// ****************************************************************************
//
//  Function Name:	RReplaceComponentAction::RReplaceComponentAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RReplaceComponentAction::RReplaceComponentAction( RCompositeSelection* pCurrentSelection,
																  RComponentDocument* pOldComponent,
																  RComponentDocument* pNewComponent,
																  YResourceId undoRedoPairId,
																  BOOLEAN fModifyBoundingRect,
																  BOOLEAN fCopyViewAttributes )
	: RUndoableAction( m_ActionId, undoRedoPairId, undoRedoPairId + 1 ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_pOldComponent( pOldComponent ),
	  m_pNewComponent( pNewComponent ),
	  m_fModifyBoundingRect( fModifyBoundingRect ),
	  m_fCopyViewAttributes( fCopyViewAttributes )
	{
	// Get pointers to the component views
	RComponentView* pOldComponentView = static_cast<RComponentView*>( pOldComponent->GetActiveView( ) );
	RComponentView* pNewComponentView = static_cast<RComponentView*>( pNewComponent->GetActiveView( ) );

	// Get the z position of the old component
	m_ComponentZPosition = m_pCurrentSelection->GetView( )->GetComponentZPosition( pOldComponentView );

	// Copy the view attributes
	if( m_fCopyViewAttributes )
		pNewComponentView->CopyViewAttributes( pOldComponentView );

	// Copy the component attributes
	pNewComponent->SetComponentAttributes( pOldComponent->GetComponentAttributes( ) );
	pNewComponent->SetURL( pOldComponent->GetURL() );

	if( m_fModifyBoundingRect )
		{
		// If the components are of the same type, we can just use the same bounding rect
		if( pOldComponent->GetComponentType( ) == pNewComponent->GetComponentType( ) )
			pNewComponentView->CopyBoundingRect( pOldComponentView );

		// If the components are of different types, we need to reset the new component to 
		// its default size, and multiply that by the transform of the old component
		else
			{
			// Reset the component back to its default size
			pNewComponent->ResetToDefaultSize( pOldComponent->GetParentDocument( ) );

			// Copy the transform
			pNewComponentView->CopyTransform( pOldComponentView );
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RReplaceComponentAction::~RReplaceComponentAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RReplaceComponentAction::~RReplaceComponentAction( )
	{
	// Delete the component for good
	delete m_pNewComponent;
	}

// ****************************************************************************
//
//  Function Name:	RReplaceComponentAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RReplaceComponentAction::Do( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	// Unselect everything
	m_pCurrentSelection->Invalidate( );
	m_pCurrentSelection->UnselectAll( FALSE );

	// Get the parent document of the old component
	RDocument* pParentDocument = m_pOldComponent->GetParentDocument( );
		// Remove the old component
	pParentDocument->RemoveComponent( m_pOldComponent );
		//	Get a pointer to the new component
	RComponentView* pNewComponentView = static_cast<RComponentView*>( m_pNewComponent->GetActiveView( ) );
	RComponentView* pOldComponentView = static_cast<RComponentView*>( m_pOldComponent->GetActiveView( ) );
	//	If modifying the bounding rect, copy it here also for The BannerHeadline
	if( m_fModifyBoundingRect && 
			((m_pNewComponent->GetComponentType()==kSpecialHeadlineComponent) ||
			 (m_pNewComponent->GetComponentType()==kVerticalSpecialHeadlineComponent)) )
		{
		// But only if the component types are the same
		if( m_pOldComponent->GetComponentType( ) == m_pNewComponent->GetComponentType( ) )
			pNewComponentView->CopyBoundingRect( pOldComponentView );
		}

	// Add the new component
	pParentDocument->AddComponent( m_pNewComponent );

	//	Set the size to the current size.  This will allow certain objects
	//	to reset there internal parameters that require the component to be
	//	linked to a document
	pNewComponentView->SetBoundingRect( pNewComponentView->GetBoundingRect() );

	// Set the z position of the new component
	m_pCurrentSelection->GetView( )->SetComponentZPosition( pNewComponentView, m_ComponentZPosition );

	// Select the new component
	m_pCurrentSelection->Select( pNewComponentView, FALSE );
	m_pCurrentSelection->Invalidate( );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	// Switch the components
	RComponentDocument* pTemp = m_pOldComponent;
	m_pOldComponent = m_pNewComponent;
	m_pNewComponent = pTemp;

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RReplaceComponentAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RReplaceComponentAction::Undo( )
	{
	// Call Do to do the switch
	Do( );

	// Call the base method to setup the state properly
	RUndoableAction::Undo( );
	}

// ****************************************************************************
//
//  Function Name:	RReplaceComponentAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RReplaceComponentAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RReplaceComponentAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RReplaceComponentAction::Validate( ) const
	{
	RUndoableAction::Validate( );
	TpsAssertIsObject( RReplaceComponentAction, this );
	}

#endif	//	TPSDEBUG
