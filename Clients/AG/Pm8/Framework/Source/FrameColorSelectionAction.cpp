// ****************************************************************************
//
//  File Name:			FrameColorSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				Eric VanHelene
//
//  Description:		Definition of the RFrameColorSelectionAction class
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
//  $Logfile:: /PM8/Framework/Source/FrameColorSelectionAction.cpp            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"FrameColorSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentDocument.h"

YActionId	RFrameColorSelectionAction::m_ActionId( "RFrameColorSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RFrameColorSelectionAction::RFrameColorSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFrameColorSelectionAction::RFrameColorSelectionAction( RCompositeSelection* pCurrentSelection, const RSolidColor rColor )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_rNewFrameColor( rColor ),
	  RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_FRAME_COLOR, STRING_REDO_CHANGE_FRAME_COLOR )
	{
	// Iterate the selection, recording the frame of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		m_yOldFrameColors.InsertAtEnd( ( *iterator )->GetFrameColor( ) );
	}

// ****************************************************************************
//
//  Function Name:	RFrameColorSelectionAction::RFrameColorSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFrameColorSelectionAction::RFrameColorSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( m_ActionId, script )
	{
	//	Retrieve the selection and frame type
	pCurrentSelection->Read( script );

	//	Read the frame type
	script >> m_rNewFrameColor;

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
	}

// ****************************************************************************
//
//  Function Name:	RFrameColorSelectionAction::Do( )
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
BOOLEAN RFrameColorSelectionAction::Do( )
	{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Iterate the selection, changing the frame of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentDocument( )->IsFrameable( ) )
			( *iterator )->SetFrameColor( m_rNewFrameColor );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );
	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RFrameColorSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrameColorSelectionAction::Undo( )
	{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the current selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, restoring the frame of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	YFrameColorListIterator yOldFrameIterator = m_yOldFrameColors.Start( );

	for( ; iterator != m_pCurrentSelection->End( ); ++iterator, ++yOldFrameIterator )
		{
		TpsAssert( yOldFrameIterator != m_yOldFrameColors.End(), "end of old frames list encountered before end of selected components" );
		if( (*iterator)->GetComponentDocument( )->IsFrameable( ) )
			( *iterator )->SetFrameColor( (*yOldFrameIterator) );
		}

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );
	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
	}

// ****************************************************************************
//
//  Function Name:	RFrameColorSelectionAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrameColorSelectionAction::Redo( )
	{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, changing the frame of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentDocument( )->IsFrameable( ) )
			( *iterator )->SetFrameColor( m_rNewFrameColor );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );
	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
	
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RFrameColorSelectionAction::WriteScript( )
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
BOOLEAN RFrameColorSelectionAction::WriteScript( RScript& script ) const
	{
	if (RUndoableAction::WriteScript( script ))
		{
		m_pCurrentSelection->Write( script );

		script << m_rNewFrameColor;
		
		return TRUE;
		}
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFrameColorSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrameColorSelectionAction::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( RFrameColorSelectionAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
