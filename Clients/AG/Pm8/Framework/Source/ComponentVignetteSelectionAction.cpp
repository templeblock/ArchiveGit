// ****************************************************************************
//
//  File Name:			ComponentVignetteSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				Greg Beddow
//
//  Description:		Definition of the RComponentVignetteSelectionAction class
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

#include	"ComponentVignetteSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentDocument.h"

YActionId	RComponentVignetteSelectionAction::m_ActionId( "RComponentVignetteSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RComponentVignetteSelectionAction::RComponentVignetteSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentVignetteSelectionAction::RComponentVignetteSelectionAction( RCompositeSelection* pCurrentSelection, const RSoftVignetteSettings& fVignetteSettings )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_fNewVignetteSettings( fVignetteSettings ),
	  RUndoableAction( m_ActionId, STRING_UNDO_ADD_VIGNETTE_ON_OFF, STRING_REDO_ADD_VIGNETTE_ON_OFF )
{
	// Iterate the selection, recording the vignette state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	RSoftVignetteSettings vignetteSettings;
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
	{
		( *iterator )->GetVignetteSettings( vignetteSettings );
		m_yOldVignetteStates.InsertAtEnd( vignetteSettings );
	}
}

// ****************************************************************************
//
//  Function Name:	RComponentVignetteSelectionAction::RComponentVignetteSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentVignetteSelectionAction::RComponentVignetteSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( m_ActionId, script )
{
	//	Retrieve the selection and frame type
	pCurrentSelection->Read( script );

	//	Read the vignette settings
#if 0 // GCB 3/20/98 - if scriptability is needed >> will need to be written for RSoftVignetteSettings
	script >> m_fNewVignetteSettings;
#else
	UnimplementedCode();
#endif

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
}

// ****************************************************************************
//
//  Function Name:	RComponentVignetteSelectionAction::Do( )
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
BOOLEAN RComponentVignetteSelectionAction::Do( )
{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Iterate the selection, changing the vignette settings of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsVignetteable( ) )
			( *iterator )->SetVignetteSettings( m_fNewVignetteSettings );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RComponentVignetteSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentVignetteSelectionAction::Undo( )
{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the current selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, restoring the vignette state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	YVignetteStateListIterator yOldVignetteStateIterator = m_yOldVignetteStates.Start( );

	for( ; iterator != m_pCurrentSelection->End( ); ++iterator, ++yOldVignetteStateIterator )
	{
		TpsAssert( yOldVignetteStateIterator != m_yOldVignetteStates.End(), "end of old frames list encountered before end of selected components" );
		if( (*iterator)->GetComponentAttributes( ).IsVignetteable( ) )
			( *iterator )->SetVignetteSettings( (*yOldVignetteStateIterator) );
	}

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
}

// ****************************************************************************
//
//  Function Name:	RComponentVignetteSelectionAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentVignetteSelectionAction::Redo( )
{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, changing the vignette state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsVignetteable( ) )
			( *iterator )->SetVignetteSettings( m_fNewVignetteSettings );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );
	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
	
	m_asLastAction = kActionRedo;
}

// ****************************************************************************
//
//  Function Name:	RComponentVignetteSelectionAction::WriteScript( )
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
BOOLEAN RComponentVignetteSelectionAction::WriteScript( RScript& script ) const
{
	if (RUndoableAction::WriteScript( script ))
	{
		m_pCurrentSelection->Write( script );

#if 0 // GCB 3/20/98 - if scriptability is needed << will need to be written for RSoftVignetteSettings
		script << m_fNewVignetteSettings;
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
//  Function Name:	RComponentVignetteSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentVignetteSelectionAction::Validate( ) const
{
	RAction::Validate();
	TpsAssertIsObject( RComponentVignetteSelectionAction, this );
	TpsAssertValid( m_pCurrentSelection );
}

#endif	//	TPSDEBUG
