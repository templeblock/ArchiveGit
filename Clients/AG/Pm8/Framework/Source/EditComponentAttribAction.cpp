// ****************************************************************************
//
//  File Name:			EditComponentAttributesAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the REditComponentAttributesAction class
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
//  $Logfile:: /PM8/Framework/Source/EditComponentAttribAction.cpp            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "EditComponentAttribAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include "CompositeSelection.h"
#include "ComponentDocument.h"
#include "EditComponentAttribDialog.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"

YActionId	REditComponentAttributesAction::m_ActionId( "REditComponentAttributesAction" );

// ****************************************************************************
//
//  Function Name:	REditComponentAttributesAction::REditComponentAttributesAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REditComponentAttributesAction::REditComponentAttributesAction( RCompositeSelection* pCurrentSelection )
	: RUndoableAction( m_ActionId, STRING_UNDO_EDIT_COMPONENT_ATTRIBUTES, STRING_REDO_EDIT_COMPONENT_ATTRIBUTES ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_OldZOrder( pCurrentSelection->GetView( )->GetZOrderState( ) ),
	  m_fAttributesChanged( FALSE )
	{
	// Validate parameters
	TpsAssertValid( pCurrentSelection );
	TpsAssert( m_pCurrentSelection->Count( ) == 1, "Edit Component Attributes action only operates on one object." );

	// Get the component document. This is where attributes are stored
	m_pComponentDocument = ( *m_pCurrentSelection->Start( ) )->GetComponentDocument( );

	// Get and save the component attributes
	m_OldComponentAttributes = m_pComponentDocument->GetComponentAttributes( );
	m_NewComponentAttributes = m_OldComponentAttributes;

	// Create and do the Edit Component Attributes dialog
	REditComponentAttributesDialog dialog( m_OldComponentAttributes );
	if( dialog.DoModal( ) == IDOK )
		{
		// Get the new attributes
		dialog.FillInComponentAttributes( m_NewComponentAttributes );
		m_fAttributesChanged	= TRUE;
		}
	}

// ****************************************************************************
//
//  Function Name:	REditComponentAttributesAction::REditComponentAttributesAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REditComponentAttributesAction::REditComponentAttributesAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: RUndoableAction( m_ActionId, script ),
	  m_pCurrentSelection( pCurrentSelection )
	{
	//	Read in the selection
	m_pCurrentSelection->Read( script );

	// Get the component document. This is where attributes are stored
	m_pComponentDocument = ( *m_pCurrentSelection->Start( ) )->GetComponentDocument( );

	// Get and save the component attributes & ZOrder
	m_OldComponentAttributes = m_pComponentDocument->GetComponentAttributes( );
	m_OldZOrder	= pCurrentSelection->GetView( )->GetZOrderState( );

	//	Read in the new attributes
	m_NewComponentAttributes.Read( script );
	script >> m_fAttributesChanged;
	}

// ****************************************************************************
//
//  Function Name:	REditComponentAttributesAction::~REditComponentAttributesAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REditComponentAttributesAction::~REditComponentAttributesAction( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	REditComponentAttributesAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditComponentAttributesAction::Do( )
	{
	// Only do the action if something changed
	if( !m_fAttributesChanged )
		return FALSE;

	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Set the new attributes into the component document
	m_pComponentDocument->SetComponentAttributes( m_NewComponentAttributes );

	// If the Z-Layer has changed, remove the component and re-insert it
	if( m_OldComponentAttributes.GetZLayer( ) != m_NewComponentAttributes.GetZLayer( ) )
		{
		// Get the parent document
		RDocument* pParentDocument = m_pCurrentSelection->GetView( )->GetRDocument( );

		// Remove the component
		pParentDocument->RemoveComponent( m_pComponentDocument );

		// Re-Add it
		pParentDocument->AddComponent( m_pComponentDocument );
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	REditComponentAttributesAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditComponentAttributesAction::Undo( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Restore the component attributes
	m_pComponentDocument->SetComponentAttributes( m_OldComponentAttributes );

	// Restore the Z-order
	m_pCurrentSelection->GetView( )->SetZOrderState( m_OldZOrder );

	// Invalidate the document
	m_pComponentDocument->InvalidateAllViews( );
	}

// ****************************************************************************
//
//  Function Name:	REditComponentAttributesAction::Redo( )
//
//  Description:		Redoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditComponentAttributesAction::Redo( )
	{
	RUndoableAction::Redo( );
	}

// ****************************************************************************
//
//  Function Name:	REditComponentAttributesAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditComponentAttributesAction::WriteScript( RScript& script ) const
	{
	if ( RUndoableAction::WriteScript( script ) )
		{
		m_pCurrentSelection->Write( script );
		m_NewComponentAttributes.Write( script );
		script << m_fAttributesChanged;
		return TRUE;
		}
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	REditComponentAttributesAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditComponentAttributesAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( REditComponentAttributesAction, this );
	TpsAssertValid( m_pCurrentSelection );
	TpsAssertValid( m_pComponentDocument );
	TpsAssertValid( &m_OldComponentAttributes );
	}

#endif	//	TPSDEBUG
