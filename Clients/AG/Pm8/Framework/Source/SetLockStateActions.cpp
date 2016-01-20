// ****************************************************************************
//
//  File Name:			SetLockStateActions.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Lock and unlock actions
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
//  $Logfile:: /PM8/Framework/Source/SetLockStateActions.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "SetLockStateActions.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include "CompositeSelection.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "FrameworkResourceIds.h"

YActionId RLockSelectionAction::m_ActionId( "RLockSelectionAction" );
YActionId RUnlockSelectionAction::m_ActionId( "RUnlockSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RSetLockStateAction::RSetLockStateAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSetLockStateAction::RSetLockStateAction( RCompositeSelection* pCurrentSelection,
														BOOLEAN fLock,
														const YActionId& actionId,
														uWORD uwUndoId,
														uWORD uwRedoId )

	: RUndoableAction( actionId, uwUndoId, uwRedoId ),
	  m_OldSelection( *pCurrentSelection ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_fLock( fLock )
	{
	// Validate parameters
	TpsAssertValid( pCurrentSelection );
	}

// ****************************************************************************
//
//  Function Name:	RSetLockStateAction::RSetLockStateAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSetLockStateAction::RSetLockStateAction( RCompositeSelection* pCurrentSelection,
														BOOLEAN fLock,
														RScript& script,
														const YActionId& actionId )
	: RUndoableAction( actionId, script ),
	  m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_fLock( fLock )
	{
	// Validate parameters
	TpsAssertValid( pCurrentSelection );
	}

// ****************************************************************************
//
//  Function Name:	RSetLockStateAction::~RSetLockStateAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSetLockStateAction::~RSetLockStateAction( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSetLockStateAction::Do( )
//
//  Description:		Does the action
//
//  Returns:			TRUE if the action was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RSetLockStateAction::Do( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Go through each selected object and set the lock state
	YSelectionIterator iterator = m_pCurrentSelection->Start( );
	RComponentAttributes componentAttributes;
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		{
		RComponentDocument* pDocument = static_cast<RComponentDocument*>( ( *iterator )->GetRDocument( ) );
		componentAttributes = pDocument->GetComponentAttributes( );
		m_LockStateCollection.InsertAtEnd( componentAttributes.IsLocked( ) );
		componentAttributes.SetLocked( m_fLock );
		pDocument->SetComponentAttributes( componentAttributes );
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RLockSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSetLockStateAction::Undo( )
	{
	// Call the base method to setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Restore the old selection
	*m_pCurrentSelection = m_OldSelection;

	// Go through each selected object and reset the lock state
	YSelectionIterator selectionIterator = m_pCurrentSelection->Start( );
	YLockStateIterator lockStateIterator = m_LockStateCollection.Start( );
	RComponentAttributes componentAttributes;

	for( ; selectionIterator != m_pCurrentSelection->End( ); ++selectionIterator, ++lockStateIterator )
		{
		RComponentDocument* pDocument = static_cast<RComponentDocument*>( ( *selectionIterator )->GetRDocument( ) );
		componentAttributes = pDocument->GetComponentAttributes( );
		componentAttributes.SetLocked( *lockStateIterator );
		pDocument->SetComponentAttributes( componentAttributes );
		}
	}

// ****************************************************************************
//
//  Function Name:	RLockSelectionAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSetLockStateAction::Redo( )
	{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Go through each selected object and set the lock state
	YSelectionIterator iterator = m_pCurrentSelection->Start( );
	RComponentAttributes componentAttributes;
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		{
		RComponentDocument* pDocument = static_cast<RComponentDocument*>( ( *iterator )->GetRDocument( ) );
		componentAttributes = pDocument->GetComponentAttributes( );
		componentAttributes.SetLocked( m_fLock );
		pDocument->SetComponentAttributes( componentAttributes );
		}
	
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RSetLockStateAction::WriteScript( )
//
//  Description:		Fills in the action from the script
//
//  Returns:			TRUE if scripting was sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RSetLockStateAction::WriteScript( RScript& script ) const
	{
	if( RUndoableAction::WriteScript( script ) )
		{
		m_pCurrentSelection->Write( script );
		script << static_cast<uLONG>( m_fLock );
		return TRUE;
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RLockSelectionAction::RLockSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RLockSelectionAction::RLockSelectionAction( RCompositeSelection* pCurrentSelection )
	: RSetLockStateAction( pCurrentSelection, TRUE, m_ActionId, STRING_UNDO_LOCK, STRING_REDO_LOCK )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RLockSelectionAction::RLockSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RLockSelectionAction::RLockSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: RSetLockStateAction( pCurrentSelection, TRUE, script, m_ActionId )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RUnlockSelectionAction::RUnlockSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUnlockSelectionAction::RUnlockSelectionAction( RCompositeSelection* pCurrentSelection )
	: RSetLockStateAction( pCurrentSelection, FALSE, m_ActionId, STRING_UNDO_UNLOCK, STRING_REDO_UNLOCK )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RUnlockSelectionAction::RUnlockSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUnlockSelectionAction::RUnlockSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: RSetLockStateAction( pCurrentSelection, FALSE, script, m_ActionId )
	{
	;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RSetLockStateAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSetLockStateAction::Validate( ) const
	{
	RAction::Validate( );
	TpsAssertIsObject( RSetLockStateAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
