// ****************************************************************************
//
//  File Name:			ResizeSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RResizeSelectionAction class
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
//  $Logfile:: /PM8/Framework/Source/ResizeSelectionAction.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ResizeSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentAttributes.h"

YActionId	RResizeSelectionAction::m_ActionId( "RResizeSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RResizeSelectionAction::RResizeSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RResizeSelectionAction::RResizeSelectionAction( RCompositeSelection* pCurrentSelection,
																const RRealPoint& centerOfScaling,
																const RRealSize& scaleFactor,
																BOOLEAN fMaintainAspectRatio )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_CenterOfScaling( centerOfScaling ),
	  m_ScaleFactor( scaleFactor ),
	  m_fMaintainAspectRatio( fMaintainAspectRatio ),
	  RUndoableAction( m_ActionId, STRING_UNDO_RESIZE_SELECTION, STRING_REDO_RESIZE_SELECTION )
	{
	;
	}


// ****************************************************************************
//
//  Function Name:	RResizeSelectionAction::RResizeSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RResizeSelectionAction::RResizeSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( m_ActionId, script )
	{
	//	Retrieve the selection and offset
	pCurrentSelection->Read( script );
	script >> m_CenterOfScaling;
	script >> m_ScaleFactor;
	script >> m_fMaintainAspectRatio;

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
	}

// ****************************************************************************
//
//  Function Name:	RResizeSelectionAction::Do( )
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
BOOLEAN RResizeSelectionAction::Do( )
	{
	//	Setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	BOOLEAN fSomethingChangedSize = FALSE;

	// Iterate the selection, moving each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		{
		if( (*iterator)->GetComponentAttributes( ).IsResizable( ) )
			{
			// Save the old bounding rect
			m_BoundingRectCollection.InsertAtEnd( ( *iterator )->GetBoundingRect( ) );

			// Do the scale
			( *iterator )->Scale( m_CenterOfScaling, m_ScaleFactor, m_fMaintainAspectRatio );

			// Did it change size?
			YBoundingRectIterator	rectIterator	= m_BoundingRectCollection.End( );
			if( ( *(--rectIterator) ).WidthHeight( ) != ( *iterator )->GetBoundingRect( ).WidthHeight( ) )
				fSomethingChangedSize = TRUE;
			}
		}

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );

	// Only return TRUE to add the action to the undo list if something changed size
	return fSomethingChangedSize;
	}												

// ****************************************************************************
//
//  Function Name:	RResizeSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResizeSelectionAction::Undo( )
	{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Remove the current selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection and the bounding rect collection, restoring the bounding rects
	// I have to do this instead of just scaling by the inverse, because an object that was
	// part of a multiple selection might not have really resized by the specified scale
	// factor; there is a limit to how small an object may become.
	RCompositeSelection::YIterator selectionIterator = m_pCurrentSelection->Start( );
	YBoundingRectIterator boundingRectIterator = m_BoundingRectCollection.Start( );

	for( ; selectionIterator != m_pCurrentSelection->End( ); ++selectionIterator )
		if( (*selectionIterator)->GetComponentAttributes( ).IsResizable( ) )
			{
			( *selectionIterator )->SetBoundingRect( *boundingRectIterator );
			++boundingRectIterator;
			}

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );
	}

// ****************************************************************************
//
//  Function Name:	RResizeSelectionAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResizeSelectionAction::Redo( )
	{
	// Erase the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, moving each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsResizable( ) )
			( *iterator )->Scale( m_CenterOfScaling, m_ScaleFactor, m_fMaintainAspectRatio );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView( )->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );
	
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RResizeSelectionAction::WriteScript( )
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
BOOLEAN RResizeSelectionAction::WriteScript( RScript& script ) const
	{
	if (RUndoableAction::WriteScript( script ))
		{
		m_pCurrentSelection->Write( script );
		script << m_CenterOfScaling;
		script << m_ScaleFactor;
		script << m_fMaintainAspectRatio;
		return TRUE;
		}
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RResizeSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RResizeSelectionAction::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( RResizeSelectionAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
