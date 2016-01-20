// ****************************************************************************
//
//  File Name:			RotateSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RRotateSelectionAction class
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
//  $Logfile:: /PM8/Framework/Source/RotateSelectionAction.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"RotateSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentAttributes.h"

YActionId	RRotateSelectionAction::m_ActionId( "RRotateSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RRotateSelectionAction::RRotateSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRotateSelectionAction::RRotateSelectionAction( RCompositeSelection* pCurrentSelection,
																const RRealPoint& centerOfRotation,
																YAngle angle )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_CenterOfRotation( centerOfRotation ),
	  m_Angle( angle ),
	  RUndoableAction( m_ActionId, STRING_UNDO_ROTATE_SELECTION, STRING_REDO_ROTATE_SELECTION )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionAction::RRotateSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRotateSelectionAction::RRotateSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( m_ActionId, script )
	{
	//	Retrieve the selection and offset
	pCurrentSelection->Read( script );
	script >> m_CenterOfRotation;
	script >> m_Angle;

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionAction::Do( )
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
BOOLEAN RRotateSelectionAction::Do( )
	{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Do the rotate
	DoRotate( m_Angle );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRotateSelectionAction::Undo( )
	{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Invalidate the current selection
	m_pCurrentSelection->Invalidate( );

	DoRotate( -m_Angle );
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRotateSelectionAction::Redo( )
	{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Do the rotate
	DoRotate( m_Angle );
	
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionAction::DoRotate( )
//
//  Description:		Actually does the move
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRotateSelectionAction::DoRotate( YAngle angle) const
	{
	// Iterate the selection, moving each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsRotatable( ) )
			( *iterator )->Rotate( m_CenterOfRotation, angle );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );
	}

// ****************************************************************************
//
//  Function Name:	RRotateSelectionAction::WriteScript( )
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
BOOLEAN RRotateSelectionAction::WriteScript( RScript& script ) const
	{
	if (RUndoableAction::WriteScript( script ))
		{
		m_pCurrentSelection->Write( script );
		script << m_CenterOfRotation;
		script << m_Angle;
		return TRUE;
		}
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RRotateSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRotateSelectionAction::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( RRotateSelectionAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
