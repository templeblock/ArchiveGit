// ****************************************************************************
//
//  File Name:			ScaleSelectionToParentAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RScaleSelectionToParentAction class
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
//  $Logfile:: /PM8/Framework/Source/ScaleSelectionToParentAction.cpp         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ScaleSelectionToParentAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentDocument.h"
#include "ComponentAttributes.h"
#include "ComponentTypes.h"

YActionId	RScaleSelectionToParentAction::m_ActionId( "RScaleSelectionToParentAction" );

// ****************************************************************************
//
//  Function Name:	RScaleSelectionToParentAction::RScaleSelectionToParentAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RScaleSelectionToParentAction::RScaleSelectionToParentAction( RCompositeSelection* pCurrentSelection,
																BOOLEAN fMaintainAspectRatio )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_fMaintainAspectRatio( fMaintainAspectRatio ),
	  RUndoableAction( m_ActionId, STRING_UNDO_RESIZE_SELECTION, STRING_REDO_RESIZE_SELECTION )
	{
	;
	}


// ****************************************************************************
//
//  Function Name:	RScaleSelectionToParentAction::RScaleSelectionToParentAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RScaleSelectionToParentAction::RScaleSelectionToParentAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( m_ActionId, script )
	{
	//	Retrieve the selection and offset
	pCurrentSelection->Read( script );
	script >> m_fMaintainAspectRatio;

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
	}

// ****************************************************************************
//
//  Function Name:	RScaleSelectionToParentAction::Do( )
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
BOOLEAN RScaleSelectionToParentAction::Do( )
	{
	RView*		pParentView	= m_pCurrentSelection->GetView( );
	RRealRect	rParentRect( pParentView->GetUseableArea(FALSE) );

	//	Setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	// Iterate the selection, moving each selected object
	RCompositeSelection::YIterator	iterator	= m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		{
		if( (*iterator)->GetComponentAttributes( ).IsMovable( ) && 
				(*iterator)->GetComponentAttributes( ).IsResizable( ) )
			{
			
			// Save the old bounding rect
			m_BoundingRectCollection.InsertAtEnd( ( *iterator )->GetBoundingRect( ) );

			if(m_fMaintainAspectRatio == 0)	// Stretch not Scale!
			{
				//	Cannot scale the line components
				RComponentDocument*		pComponent	= (static_cast<RComponentView*>( *iterator ))->GetComponentDocument();
				const YComponentType&	compType	= pComponent->GetComponentType( );
				if ( compType != kHorizontalLineComponent && compType != kVerticalLineComponent )
					( *iterator )->SetBoundingRect( YComponentBoundingRect(rParentRect) );
			}

			// Added 7 Jan 1997 - Mike Taber. To allow for Scaling to panel
			if(m_fMaintainAspectRatio != 0)	// Scale not Stretch!
			{
				RRealSize szOffset( rParentRect.m_Left, rParentRect.m_Top );
				( *iterator )->FitInsideParent();
				( *iterator )->Offset( szOffset, FALSE );
/*				// Get the component size
				RRealSize componentSize = ( *iterator )->GetBoundingRect( ).m_TransformedBoundingRect.WidthHeight( );
				// Calculate an aspect correct scale factor
				RRealSize scaleFactor( rParentRect.Width( ) / componentSize.m_dx, rParentRect.Height( ) / componentSize.m_dy );
				if( scaleFactor.m_dx < scaleFactor.m_dy )
					scaleFactor.m_dy = scaleFactor.m_dx;
				else
					scaleFactor.m_dx = scaleFactor.m_dy;
				
				// Get the center of the selection and use it as the center of scaling
				RRealPoint centerPoint = ( *iterator )->GetBoundingRect( ).GetCenterPoint( );
				
				// Do the scale
				( *iterator )->Scale( centerPoint, scaleFactor, m_fMaintainAspectRatio );				
*/			}

			
			}

		}

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	//	Tell the view that its layout has changed
	pParentView->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	pParentView->UpdateScrollBars( kChangeObjectPosition );

	// Only return TRUE to add the action to the undo list if something changed size
	return TRUE;
	}												

// ****************************************************************************
//
//  Function Name:	RScaleSelectionToParentAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScaleSelectionToParentAction::Undo( )
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
//  Function Name:	RScaleSelectionToParentAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScaleSelectionToParentAction::Redo( )
	{
	RView*		pParentView	= m_pCurrentSelection->GetView( );
	RRealRect	rParentRect( pParentView->GetUseableArea(FALSE) );

	// Erase the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, moving each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		if( (*iterator)->GetComponentAttributes( ).IsMovable( ) &&
				(*iterator)->GetComponentAttributes( ).IsResizable( ) )
			{
			( *iterator )->SetBoundingRect( YComponentBoundingRect(rParentRect) );
			}

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
//  Function Name:	RScaleSelectionToParentAction::WriteScript( )
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
BOOLEAN RScaleSelectionToParentAction::WriteScript( RScript& script ) const
	{
	if (RUndoableAction::WriteScript( script ))
		{
		m_pCurrentSelection->Write( script );
		script << m_fMaintainAspectRatio;
		return TRUE;
		}
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RScaleSelectionToParentAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScaleSelectionToParentAction::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( RScaleSelectionToParentAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
