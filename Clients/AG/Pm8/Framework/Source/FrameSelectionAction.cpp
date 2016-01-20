// ****************************************************************************
//
//  File Name:			FrameSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				Eric VanHelene
//
//  Description:		Definition of the RFrameSelectionAction class
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
//  $Logfile:: /PM8/Framework/Source/FrameSelectionAction.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"FrameSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentDocument.h"

YActionId	RFrameSelectionAction::m_ActionId( "RFrameSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RFrameSelectionAction::RFrameSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFrameSelectionAction::RFrameSelectionAction( RCompositeSelection* pCurrentSelection, const EFrameType& frame )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_eNewFrameType( frame ),
	  RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_FRAME, STRING_REDO_CHANGE_FRAME )
	{
	// Iterate the selection, recording the frame of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		m_yOldFrames.InsertAtEnd( ( *iterator )->GetFrameType( ) );
	}

// ****************************************************************************
//
//  Function Name:	RFrameSelectionAction::RFrameSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFrameSelectionAction::RFrameSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( m_ActionId, script )
	{
	//	Retrieve the selection and frame type
	pCurrentSelection->Read( script );

	//	Read the frame type
	uLONG ulTemp;
	script >> ulTemp;
	m_eNewFrameType = static_cast< EFrameType > ( ulTemp );

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
	}

// ****************************************************************************
//
//  Function Name:	RFrameSelectionAction::Do( )
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
BOOLEAN RFrameSelectionAction::Do( )
	{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection. Framed components may shrink to preserve
	//	aspect ratio so invalidate before they shrink.
	m_pCurrentSelection->Invalidate( );

	// Iterate the selection, changing the frame of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentDocument( )->IsFrameable( ) )
			( *iterator )->SetFrame( m_eNewFrameType );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );
	
	// Invalidate the selection. Invalidate again even though we invalidated
	//	above since the object may grow when a frame is removed if the object
	//	shrunk to preserve aspect ratio when the frame was added.
	m_pCurrentSelection->Invalidate( );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RFrameSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrameSelectionAction::Undo( )
	{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the current selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, restoring the frame of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	YFrameTypeArrayIterator yOldFrameIterator = m_yOldFrames.Start( );

	for( ; iterator != m_pCurrentSelection->End( ); ++iterator, ++yOldFrameIterator )
		{
		TpsAssert( yOldFrameIterator != m_yOldFrames.End(), "end of old frames list encountered before end of selected components" );
		if( (*iterator)->GetComponentDocument( )->IsFrameable( ) )
			( *iterator )->SetFrame( (*yOldFrameIterator) );
		}

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
	}

// ****************************************************************************
//
//  Function Name:	RFrameSelectionAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrameSelectionAction::Redo( )
	{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, changing the frame of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentDocument( )->IsFrameable( ) )
			( *iterator )->SetFrame( m_eNewFrameType );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
	
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RFrameSelectionAction::WriteScript( )
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
BOOLEAN RFrameSelectionAction::WriteScript( RScript& script ) const
	{
	if (RUndoableAction::WriteScript( script ))
		{
		m_pCurrentSelection->Write( script );

		script << static_cast< uLONG >( m_eNewFrameType );
		
		return TRUE;
		}
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFrameSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrameSelectionAction::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( RFrameSelectionAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
