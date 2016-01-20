// ****************************************************************************
//
//  File Name:			ComponentShadowSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				Eric VanHelene
//
//  Description:		Definition of the RComponentShadowSelectionAction class
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
//  $Logfile:: /PM8/Framework/Source/ComponentShadowSelectionAction.cpp            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ComponentShadowSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentDocument.h"

YActionId	RComponentShadowSelectionAction::m_ActionId( "RComponentShadowSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RComponentShadowSelectionAction::RComponentShadowSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentShadowSelectionAction::RComponentShadowSelectionAction( RCompositeSelection* pCurrentSelection, const RSoftShadowSettings& fShadowSettings )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_fNewShadowSettings( fShadowSettings ),
	  RUndoableAction( m_ActionId, STRING_UNDO_ADD_SHADOW, STRING_REDO_ADD_SHADOW )
	{
	// Iterate the selection, recording the shadow state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	RSoftShadowSettings shadowSettings;
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		{
		( *iterator )->GetShadowSettings( shadowSettings );
		m_yOldShadowStates.InsertAtEnd( shadowSettings );
		}
	}

// ****************************************************************************
//
//  Function Name:	RComponentShadowSelectionAction::RComponentShadowSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentShadowSelectionAction::RComponentShadowSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
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
	m_OldSelection			= *pCurrentSelection;
	}

// ****************************************************************************
//
//  Function Name:	RComponentShadowSelectionAction::Do( )
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
BOOLEAN RComponentShadowSelectionAction::Do( )
	{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Iterate the selection, changing the shadow settings of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsShadowable( ) )
			( *iterator )->SetShadowSettings( m_fNewShadowSettings );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentShadowSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentShadowSelectionAction::Undo( )
	{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Invalidate before changing the shadow settings
	m_pCurrentSelection->Invalidate( );

	// Iterate the selection, restoring the shadow state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	YShadowStateListIterator yOldShadowStateIterator = m_yOldShadowStates.Start( );

	for( ; iterator != m_pCurrentSelection->End( ); ++iterator, ++yOldShadowStateIterator )
		{
		TpsAssert( yOldShadowStateIterator != m_yOldShadowStates.End(), "end of old frames list encountered before end of selected components" );
		if( (*iterator)->GetComponentAttributes( ).IsShadowable( ) )
			( *iterator )->SetShadowSettings( (*yOldShadowStateIterator) );
		}

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate after changing the shadow settings
	m_pCurrentSelection->Invalidate( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentShadowSelectionAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentShadowSelectionAction::Redo( )
	{
	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Invalidate before changing the shadow settings
	m_pCurrentSelection->Invalidate( );

	// Iterate the selection, changing the shadow state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsShadowable( ) )
			( *iterator )->SetShadowSettings( m_fNewShadowSettings );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Invalidate after changing the shadow settings
	m_pCurrentSelection->Invalidate( );
	
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RComponentShadowSelectionAction::WriteScript( )
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
BOOLEAN RComponentShadowSelectionAction::WriteScript( RScript& script ) const
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
//  Function Name:	RComponentShadowSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentShadowSelectionAction::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( RComponentShadowSelectionAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
