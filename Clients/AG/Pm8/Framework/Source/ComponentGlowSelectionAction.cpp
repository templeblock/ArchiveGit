// ****************************************************************************
//
//  File Name:			ComponentGlowSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				Greg Beddow
//
//  Description:		Definition of the RComponentGlowSelectionAction class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software Inc. All Rights Reserved.
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ComponentGlowSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentDocument.h"

YActionId	RComponentGlowSelectionAction::m_ActionId( "RComponentGlowSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RComponentGlowSelectionAction::RComponentGlowSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentGlowSelectionAction::RComponentGlowSelectionAction( RCompositeSelection* pCurrentSelection, const RSoftGlowSettings& fGlowSettings )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_fNewGlowSettings( fGlowSettings ),
	  RUndoableAction( m_ActionId, STRING_UNDO_ADD_GLOW_ON_OFF, STRING_REDO_ADD_GLOW_ON_OFF )
	{
	// Iterate the selection, recording the glow state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	RSoftGlowSettings glowSettings;
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		{
		( *iterator )->GetGlowSettings( glowSettings );
		m_yOldGlowStates.InsertAtEnd( glowSettings );
		}
	}

// ****************************************************************************
//
//  Function Name:	RComponentGlowSelectionAction::RComponentGlowSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentGlowSelectionAction::RComponentGlowSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( m_ActionId, script )
	{
	//	Retrieve the selection and frame type
	pCurrentSelection->Read( script );

	//	Read the glow settings
#if 0 // GCB 2/25/98 - if scriptability is needed >> will need to be written for RSoftGlowSettings
	script >> m_fNewGlowSettings;
#else
	UnimplementedCode();
#endif

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
	}

// ****************************************************************************
//
//  Function Name:	RComponentGlowSelectionAction::Do( )
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
BOOLEAN RComponentGlowSelectionAction::Do( )
	{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Iterate the selection, changing the glow settings of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsGlowable( ) )
			( *iterator )->SetGlowSettings( m_fNewGlowSettings );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentGlowSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentGlowSelectionAction::Undo( )
	{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Invalidate before changing the glow settings
	m_pCurrentSelection->Invalidate( );

	// Iterate the selection, restoring the glow state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	YGlowStateListIterator yOldGlowStateIterator = m_yOldGlowStates.Start( );

	for( ; iterator != m_pCurrentSelection->End( ); ++iterator, ++yOldGlowStateIterator )
		{
		TpsAssert( yOldGlowStateIterator != m_yOldGlowStates.End(), "end of old frames list encountered before end of selected components" );
		if( (*iterator)->GetComponentAttributes( ).IsGlowable( ) )
			( *iterator )->SetGlowSettings( (*yOldGlowStateIterator) );
		}

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate after changing the glow settings
	m_pCurrentSelection->Invalidate( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentGlowSelectionAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentGlowSelectionAction::Redo( )
	{
	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Invalidate before changing the glow settings
	m_pCurrentSelection->Invalidate( );

	// Iterate the selection, changing the glow state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsGlowable( ) )
			( *iterator )->SetGlowSettings( m_fNewGlowSettings );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate after changing the glow settings
	m_pCurrentSelection->Invalidate( );
	
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RComponentGlowSelectionAction::WriteScript( )
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
BOOLEAN RComponentGlowSelectionAction::WriteScript( RScript& script ) const
	{
	if (RUndoableAction::WriteScript( script ))
		{
		m_pCurrentSelection->Write( script );

#if 0 // GCB 2/25/98 - if scriptability is needed << will need to be written for RSoftGlowSettings
		script << m_fNewGlowSettings;
#else
		UnimplementedCode();
#endif
		
		return TRUE;
		}
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RComponentGlowSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentGlowSelectionAction::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( RComponentGlowSelectionAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
