// ****************************************************************************
//
//  File Name:			NewComponentAction.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RNewComponentAction class
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
//  $Logfile:: /PM8/Framework/Source/NewComponentAction.cpp                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "NewComponentAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentDocument.h"
#include "StandaloneApplication.h"
#include "FrameworkResourceIds.h"
#include "Script.h"
#include "ApplicationGlobals.h"
#include "ComponentView.h"
#include "CompositeSelection.h"

YActionId	RNewComponentAction::m_ActionId( "RNewComponentAction" );

// ****************************************************************************
//
//  Function Name:	RNewComponentAction::RNewComponentAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
RNewComponentAction::RNewComponentAction( RComponentDocument* pComponentDocument,
														RDocument* pParentDocument,
														RCompositeSelection* pCurrentSelection,
														YResourceId undoRedoPairId )
	: RUndoableAction( m_ActionId, undoRedoPairId, undoRedoPairId + 1 ),
	  m_pParentDocument( pParentDocument ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_pComponentDocument( pComponentDocument ),
	  m_pActiveView( NULL )
	{
	// Validate parameters
	TpsAssertValid( pParentDocument );
	TpsAssertValid( pCurrentSelection );
	}

// ****************************************************************************
//
//  Function Name:	RNewComponentAction::~RNewComponentAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RNewComponentAction::~RNewComponentAction( )
	{
	if( (m_asLastAction == kActionUndo ) || (m_asLastAction == kActionNotDone) )
		{
		m_pComponentDocument->FreeDocumentContents( );
		delete m_pComponentDocument;
		}
	}

// ****************************************************************************
//
//  Function Name:	RNewComponentAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RNewComponentAction::Do( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	//	Deactivate any active component
	m_pActiveView = m_pParentDocument->GetActiveView()->DeactivateComponentView( );

	// Add the component
	m_pParentDocument->AddComponent( m_pComponentDocument );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Unselect all components and select this one
	m_pCurrentSelection->UnselectAll( FALSE );
	m_pCurrentSelection->Select( m_pComponentDocument, FALSE );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RNewComponentAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		Memory
//
// ****************************************************************************
//
void RNewComponentAction::Undo( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Unselect everything
	m_pCurrentSelection->UnselectAll( FALSE );

	// Remove the component
	m_pParentDocument->RemoveComponent( m_pComponentDocument );

	//	Activate any active component
	m_pParentDocument->GetActiveView()->ActivateComponentView( m_pActiveView );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );
	}

// ****************************************************************************
//
//  Function Name:	RNewComponentAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TODO: return value?
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RNewComponentAction::WriteScript( RScript& script ) const
	{
	if ( RUndoableAction::WriteScript( script ) )
		{
		m_pCurrentSelection->Write( script );
		return TRUE;
		}
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RNewComponentAction::GetComponentDocument()
//
//  Description:		Returns a pointer to the new component document we 
//							just created
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RComponentDocument* RNewComponentAction::GetComponentDocument()
{
	return m_pComponentDocument;
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RNewComponentAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
///
// ****************************************************************************
//
void RNewComponentAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RNewComponentAction, this );
	if( m_pComponentDocument )
		TpsAssertValid( m_pComponentDocument );
	TpsAssertValid( m_pParentDocument );
	}

#endif	//	TPSDEBUG
