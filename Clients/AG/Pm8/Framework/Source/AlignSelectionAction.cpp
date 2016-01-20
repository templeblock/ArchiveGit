// ****************************************************************************
//
//  File Name:			AlignSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RAlignSelectionAction class
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
//  $Logfile:: /PM8/Framework/Source/AlignSelectionAction.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"AlignSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentAttributes.h"

YActionId	RAlignSelectionAction::m_ActionId( "RAlignSelectionAction" );

// ****************************************************************************
//
//  Function Name:	RAlignSelectionAction::RAlignSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAlignSelectionAction::RAlignSelectionAction( RCompositeSelection* pCurrentSelection, const EAlignHorizontal eHAlign, const EAlignVertical eVAlign, EAlignToWhat eToWhat )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  m_eHorizontalAlignment( eHAlign ),
	  m_eVerticalAlignment( eVAlign ),
	  m_eToWhat( eToWhat ),
	  RUndoableAction( m_ActionId, STRING_UNDO_ALIGN, STRING_REDO_ALIGN )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RAlignSelectionAction::RAlignSelectionAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAlignSelectionAction::RAlignSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script )
	: m_pCurrentSelection( pCurrentSelection ),
	  m_OldSelection( *pCurrentSelection ),
	  RUndoableAction( m_ActionId, script )
	{
	//	Retrieve the selection and offset
	pCurrentSelection->Read( script );
	uLONG ulTemp;

	script >> ulTemp;
	m_eHorizontalAlignment = (EAlignHorizontal) ulTemp;

	script >> ulTemp;
	m_eVerticalAlignment = (EAlignVertical) ulTemp;;

	script >> ulTemp;
	m_eToWhat = (EAlignToWhat) ulTemp;
	
	//	Read the list of offsets
	uLONG ulCount;
	script >> ulCount;

	RRealSize rOffset;
	for( uLONG ul = 0; ul < ulCount; ++ul )
		{
		script >> rOffset;
		m_OldOffsets.InsertAtEnd( rOffset );
		}

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
	}

// ****************************************************************************
//
//  Function Name:	RAlignSelectionAction::Do( )
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
BOOLEAN RAlignSelectionAction::Do( )
	{
	//	Get the view of the selection
	RView*	pView	= m_pCurrentSelection->GetView( );

	//	Setup the state properly
	RUndoableAction::Do( );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	//	Get list of offsets that implement the alignment
	ROffsetList rAlignmentOffsets;


	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );

	RRealRect rAllRect(0,0,0,0);
	RRealRect rCurrentRect(0,0,0,0);
	if ( m_eToWhat == kToParentView )
		{
		rAllRect	= pView->GetUseableArea( FALSE );
		}
	else	//	To Each other
		{
		// Iterate the selection, find the destination rect for all selected components
		BOOLEAN	fFirstFound = FALSE;
		for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
			{
			if( (*iterator)->GetComponentAttributes( ).IsMovable( ) )
				{
				rCurrentRect = (*iterator)->GetBoundingRect().m_TransformedBoundingRect ; //RIntRect( (*iterator)->GetBoundingRect( ).m_TopLeft, (*iterator)->GetBoundingRect( ).WidthHeight() );
				if ( fFirstFound )
					rAllRect.Union( rAllRect, rCurrentRect );
				else
					{
					rAllRect = rCurrentRect;
					fFirstFound = TRUE;
					}
				}	//	IsMovable
			}	//	For loop
		}	//	toEachOther

	// Iterate the selection, find the destination offset, move the component
	RRealSize rOffset;
	m_OldOffsets.Empty();
	iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
		{
		rOffset	= RRealSize( 0, 0 );

		if( (*iterator)->GetComponentAttributes( ).IsMovable( ) )
			{
			rCurrentRect = (*iterator)->GetBoundingRect().m_TransformedBoundingRect;//RIntRect( (*iterator)->GetBoundingRect( ).m_TopLeft, (*iterator)->GetBoundingRect( ).WidthHeight() );

			switch( m_eHorizontalAlignment )
				{
				case kAlignNoneHorz:
					break;

				case kAlignCenterHorz:
					rOffset.m_dx = rAllRect.GetCenterPoint().m_x - rCurrentRect.GetCenterPoint().m_x;
					break;
				
				case kAlignRight:
					rOffset.m_dx = rAllRect.m_Right - rCurrentRect.m_Right;
					break;
				
				case kAlignLeft:
					rOffset.m_dx = rAllRect.m_Left - rCurrentRect.m_Left;
					break;
				
				default:
					TpsAssertAlways( "invalid alignment" );
					break;
				}

			switch( m_eVerticalAlignment )
				{
				case kAlignNoneVert:
					break;

				case kAlignTop:
					rOffset.m_dy = rAllRect.m_Top - rCurrentRect.m_Top;
					break;

				case kAlignBottom:
					rOffset.m_dy = rAllRect.m_Bottom - rCurrentRect.m_Bottom;
					break;
				
				case kAlignCenterVert:
					rOffset.m_dy = rAllRect.GetCenterPoint().m_y - rCurrentRect.GetCenterPoint().m_y;
					break;
				
				default:
					TpsAssertAlways( "invalid alignment" );
					break;
				}

			//	Move the component
			( *iterator )->Offset( rOffset );
			}
		
		//	Remember the offset;
		m_OldOffsets.InsertAtEnd( rOffset );
		}

	//	Update the view that its layout has changed
	pView->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	pView->UpdateScrollBars( kChangeObjectPosition );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RAlignSelectionAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RAlignSelectionAction::Undo( )
	{
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the current selection
	m_pCurrentSelection->Invalidate(  );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, moving each selected object
	TpsAssert( m_OldOffsets.Count() == m_pCurrentSelection->Count(), "number of offsets does not match number of things to offset" );

	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	ROffsetList::YIterator offsetIterator = m_OldOffsets.Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator, ++offsetIterator )
		if( (*iterator)->GetComponentAttributes( ).IsMovable( ) )
			( *iterator )->Offset( RRealSize( -( *offsetIterator ).m_dx, -( *offsetIterator ).m_dy ) );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
	}

// ****************************************************************************
//
//  Function Name:	RAlignSelectionAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RAlignSelectionAction::Redo( )
	{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, moving each selected object
	TpsAssert( m_OldOffsets.Count() == m_pCurrentSelection->Count(), "number of offsets does not match number of things to offset" );

	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	ROffsetList::YIterator offsetIterator = m_OldOffsets.Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator, ++offsetIterator )
		if( (*iterator)->GetComponentAttributes( ).IsMovable( ) )
			( *iterator )->Offset( RRealSize( ( *offsetIterator ).m_dx, ( *offsetIterator ).m_dy ) );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	// Update the scroll bars
	m_pCurrentSelection->GetView( )->UpdateScrollBars( kChangeObjectPosition );

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
	
	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RAlignSelectionAction::WriteScript( )
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
BOOLEAN RAlignSelectionAction::WriteScript( RScript& script ) const
	{
	if (RUndoableAction::WriteScript( script ))
		{
		m_pCurrentSelection->Write( script );
		script << (uLONG)m_eHorizontalAlignment;
		script << (uLONG)m_eVerticalAlignment;
		script << (uLONG)m_eToWhat;
	
		//	Write the list of offsets
		script << m_OldOffsets.Count();
		ROffsetList::YIterator offsetIterator = m_OldOffsets.Start( );
		for( ; offsetIterator != m_OldOffsets.End( ); ++offsetIterator )
			script << ( *offsetIterator );

		return TRUE;
		}
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RAlignSelectionAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RAlignSelectionAction::Validate( ) const
	{
	RAction::Validate();
	TpsAssertIsObject( RAlignSelectionAction, this );
	TpsAssertValid( m_pCurrentSelection );
	}

#endif	//	TPSDEBUG
