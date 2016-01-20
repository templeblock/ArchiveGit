// ****************************************************************************
//
//  File Name:			ComponentSpecialEdgeAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				Michael Kelson
//
//  Description:		Definition of the RComponentSpecialEdgeAction class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.         
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "ComponentSpecialEdgeAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include "CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentDocument.h"

YActionId	RComponentSpecialEdgeAction::m_ActionId( "RComponentSpecialEdgeAction" );

// ****************************************************************************
//
//  Function Name:	RComponentSpecialEdgeAction::RComponentSpecialEdgeAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentSpecialEdgeAction::RComponentSpecialEdgeAction( RCompositeSelection* pCurrentSelection, const RSpecialEdgeSettings& fEdgeSettings )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_fNewEdgeSettings( fEdgeSettings ),
	  RUndoableAction( m_ActionId, STRING_UNDO_TRANSPARENT_EFFECT, STRING_REDO_TRANSPARENT_EFFECT )
	{
	// Iterate the selection, recording the shadow state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	RSpecialEdgeSettings edgeSettings;
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		{
		( *iterator )->GetSpecialEdgeSettings( edgeSettings );
		m_yOldEdgeStates.InsertAtEnd( edgeSettings );
		}
	}

// ****************************************************************************
//
//  Function Name:	RComponentSpecialEdgeAction::RComponentSpecialEdgeAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentSpecialEdgeAction::RComponentSpecialEdgeAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( m_ActionId, script )
	{
	//	Retrieve the selection and frame type
	pCurrentSelection->Read( script );

	//	Read the shadow settings
#if 0 // GCB 2/25/98 - if scriptability is needed >> will need to be written for RSoftShadowSettings
	script >> m_fNewShadowSettings;
#else
	UnimplementedCode();
#endif

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection		= *pCurrentSelection;
	}

// ****************************************************************************
//
//  Function Name:	RComponentSpecialEdgeAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE	: The action succeeded; script it
//						FALSE	: The action failed, do not script it
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentSpecialEdgeAction::Do( )
	{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Iterate the selection, changing the shadow settings of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsAlphaMaskable( ) )
			( *iterator )->SetSpecialEdgeSettings( m_fNewEdgeSettings );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentSpecialEdgeAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentSpecialEdgeAction::Undo( )
	{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the current selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, restoring the shadow state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	YEdgeStateListIterator yOldEdgeStateIterator = m_yOldEdgeStates.Start( );

	for( ; iterator != m_pCurrentSelection->End( ); ++iterator, ++yOldEdgeStateIterator )
		{
		// TpsAssert( yOldShadowStateIterator != m_yOldShadowStates.End(), "end of old frames list encountered before end of selected components" );
		if( (*iterator)->GetComponentAttributes( ).IsAlphaMaskable( ) )
			( *iterator )->SetSpecialEdgeSettings( (*yOldEdgeStateIterator) );
		}

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentSpecialEdgeAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentSpecialEdgeAction::Redo( )
	{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, changing the shadow state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsShadowable( ) )
			( *iterator )->SetSpecialEdgeSettings( m_fNewEdgeSettings );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
	
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RComponentSpecialEdgeAction::WriteScript( )
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
BOOLEAN RComponentSpecialEdgeAction::WriteScript( RScript& script ) const
	{
	if (RUndoableAction::WriteScript( script ))
		{
		m_pCurrentSelection->Write( script );

#if 0 // GCB 2/25/98 - if scriptability is needed << will need to be written for RSoftShadowSettings
		script << m_fNewShadowSettings;
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
//  Function Name:	RComponentSpecialEdgeAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentSpecialEdgeAction::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( RComponentSpecialEdgeAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
