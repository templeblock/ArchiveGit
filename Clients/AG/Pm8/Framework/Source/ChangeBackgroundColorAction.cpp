// ****************************************************************************
//
//  File Name:			ChangeBackgroundColorAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				Eric VanHelene
//
//  Description:		Definition of the RChangeBackgroundColorAction class
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
//  $Logfile:: /PM8/Framework/Source/ChangeBackgroundColorAction.cpp          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ChangeBackgroundColorAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include "ApplicationGlobals.h"
#include "Document.h"
#include "Graphic.h"
#include "FileStream.h"
#include "FrameworkResourceIds.h"
#include "StorageSupport.h"
#include "PaneView.h"
#include "ComponentView.h"
#include "ComponentAttributes.h"

YActionId RChangeBackgroundColorAction::m_ActionId( "RChangeBackgroundColorAction" );

// ****************************************************************************
//
//  Function Name:	RChangeBackgroundColorAction::RChangeBackgroundColorAction()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

RChangeBackgroundColorAction::RChangeBackgroundColorAction( const RColor& rColor )
: RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_COLOR, STRING_REDO_CHANGE_COLOR ),
	m_Color( rColor )
{		
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangeBackgroundColorAction::RChangeBackgroundColorAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChangeBackgroundColorAction::RChangeBackgroundColorAction( RScript& script)
	: RUndoableAction( m_ActionId, script )
{
	//	Read new color.
	RColor	color;
	script >> color;
	m_Color = color;
	
	//	Read the list of old colors
	uLONG ulCount;
	script >> ulCount;

	RColor rOldColor;
	for( uLONG ul = 0; ul < ulCount; ++ul )
		{
		script >> rOldColor;
		m_OldColors.InsertAtEnd( rOldColor );
		}
}

// ****************************************************************************
//
//  Function Name:	RChangeSelectionBackgroundColorAction::RChangeSelectionBackgroundColorAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChangeSelectionBackgroundColorAction::RChangeSelectionBackgroundColorAction( RCompositeSelection* pCurrentSelection, RScript& script)
	: RChangeBackgroundColorAction( script ),
	m_pCurrentSelection( pCurrentSelection ),
	m_OldSelection( *pCurrentSelection )
{
	pCurrentSelection->Read( script );

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
}


// ****************************************************************************
//
//  Function Name:	RChangeSelectionBackgroundColorAction::RChangeSelectionBackgroundColorAction()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

RChangeSelectionBackgroundColorAction::RChangeSelectionBackgroundColorAction( RCompositeSelection* pCurrentSelection, const RColor& rColor )
: RChangeBackgroundColorAction( rColor ),
	m_pCurrentSelection( pCurrentSelection ),
	m_OldSelection( *pCurrentSelection )
{		
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangeSelectionBackgroundColorAction::WriteScript( )
//
//  Description:		Write the action data to a script.
//
//  Returns:			TRUE if sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RChangeSelectionBackgroundColorAction::WriteScript( RScript& script ) const
{
	BOOLEAN fSuccess = RUndoableAction::WriteScript( script );
	if (fSuccess)
		{
		m_pCurrentSelection->Write( script );

		return TRUE;
		}

	return FALSE;
}


// ****************************************************************************
//
//  Function Name:	RChangePaneViewBackgroundColorAction::RChangePaneViewBackgroundColorAction()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

RChangePaneViewBackgroundColorAction::RChangePaneViewBackgroundColorAction( RPaneView* pView, const RColor& rColor )
: RChangeBackgroundColorAction( rColor ),
	m_pView( pView )
{		
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangePaneViewBackgroundColorAction::RChangePaneViewBackgroundColorAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChangePaneViewBackgroundColorAction::RChangePaneViewBackgroundColorAction( RPaneView* pView, RScript& script)
	: RChangeBackgroundColorAction( script ),
	m_pView( pView )
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangeBackgroundColorAction::~RChangeBackgroundColorAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChangeBackgroundColorAction::~RChangeBackgroundColorAction( )
{	
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangeBackgroundColorAction::Do( )
//
//  Description:		Perform the action.
//
//  Returns:			TRUE if action performed sucessfully
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RChangeBackgroundColorAction::Do()
	{	
	//	Setup the state properly
	BOOLEAN fSuccess = RUndoableAction::Do( );

	if ( fSuccess )
		{
		//	Make sure the prior state lists are empty.
		m_OldColors.Empty();
		}

	return fSuccess;
	}

// ****************************************************************************
//
//  Function Name:	RChangeBackgroundColorAction::SaveOldColor( const RColor& color )
//
//  Description:		save the given colors into the member lists.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeBackgroundColorAction::SaveOldColor( const RColor& color )
	{	
	m_OldColors.InsertAtEnd( color );
	}

// ****************************************************************************
//
//  Function Name:	RChangeBackgroundColorAction::SetColor( RPaneView* pView, const RColor& color )
//
//  Description:		set the given colors into the given view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeBackgroundColorAction::SetColor( RPaneView* pView, const RColor& color )
	{	
	//	Install new settings.
		pView->SetBackgroundColor( color );
	}

// ****************************************************************************
//
//  Function Name:	RChangeBackgroundColorAction::WriteScript( )
//
//  Description:		Write the action data to a script.
//
//  Returns:			TRUE if sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RChangeBackgroundColorAction::WriteScript( RScript& script ) const
{
	if ( RUndoableAction::WriteScript( script ) )
		{
		//	Write the new color and color state flag.
		script << m_Color;
	
		//	Write the list of old colors.
		script << m_OldColors.Count();
		RColorList::YIterator colorsIterator = m_OldColors.Start( );
		for( ; colorsIterator != m_OldColors.End( ); ++colorsIterator )
			script << ( *colorsIterator );

		return TRUE;
		}

	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RChangeBackgroundColorAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeBackgroundColorAction::Redo( )
	{
	RUndoableAction::Redo();
	}

// ****************************************************************************
//
//  Function Name:	RChangeBackgroundColorAction::Undo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeBackgroundColorAction::Undo( )
	{
	RUndoableAction::Undo();
	}

// ****************************************************************************
//
//  Function Name:	RChangeSelectionBackgroundColorAction::Do( )
//
//  Description:		Perform the action.
//
//  Returns:			TRUE if action performed sucessfully
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RChangeSelectionBackgroundColorAction::Do()
	{	
	//	Setup the state properly
	BOOLEAN fSuccess = RChangeBackgroundColorAction::Do( );

	if ( fSuccess )
		{
		//	Get current settings and set new color.
		RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
		for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
//			if( (*iterator)->GetComponentAttributes( ).CanHaveBackgroundColor( ) )
				{
				//	Save old settings.
				SaveOldColor( (*iterator)->GetBackgroundColor() );

				//	Install new settings.
				SetColor( (*iterator), m_Color );
				}

		// Invalidate the selection
		m_pCurrentSelection->Invalidate( );
		}

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	return fSuccess;
	}

// ****************************************************************************
//
//  Function Name:	RChangeSelectionBackgroundColorAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeSelectionBackgroundColorAction::Undo()
{	
	//	Setup the state properly
	RUndoableAction::Undo( );

	// Invalidate the current selection
	m_pCurrentSelection->Invalidate(  );

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, setting the color of each selected component
	TpsAssert( m_OldColors.Count() == m_pCurrentSelection->Count(), "number of colors does not match number of things to offset" );

	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	RColorList::YIterator colorIterator = m_OldColors.Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator, ++colorIterator )
//		if( (*iterator)->GetComponentAttributes( ).CanHaveBackgroundColor( ) )
			{			
			//	Install new settings.
			SetColor( (*iterator), (*colorIterator) );
			}

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );

	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

}

// ****************************************************************************
//
//  Function Name:	RChangeSelectionBackgroundColorAction::Redo( )
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeSelectionBackgroundColorAction::Redo( )
	{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate( );

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, changing the color of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start( );
	for( ; iterator != m_pCurrentSelection->End( ); ++iterator )
//		if( (*iterator)->GetComponentAttributes( ).CanHaveBackgroundColor( ) )
			{			
			//	Install new settings.
			SetColor( (*iterator), m_Color );
			}

	// Invalidate the selection
	m_pCurrentSelection->Invalidate( );
	
	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews( kLayoutChanged, 0 );

	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RChangePaneViewBackgroundColorAction::Do( )
//
//  Description:		Perform the action.
//
//  Returns:			TRUE if action performed sucessfully
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RChangePaneViewBackgroundColorAction::Do()
	{	
	//	Setup the state properly
	BOOLEAN fSuccess = RChangeBackgroundColorAction::Do( );

	if ( fSuccess )
		{
		//	Get current settings.
		RColor	color( m_pView->GetBackgroundColor() );
		
		//	Install new settings.
		SetColor( m_pView, m_Color );

		//	Save old settings.
		m_Color = color;

		// Invalidate the selection
		m_pView->Invalidate( );
		}

	//	Update the view that its layout has changed
	m_pView->GetParentView( )->XUpdateAllViews( kLayoutChanged, 0 );

	return fSuccess;
	}
// ****************************************************************************
//
//  Function Name:	RChangePaneViewBackgroundColorAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangePaneViewBackgroundColorAction::Undo()
{	
	Do();
	RChangeBackgroundColorAction::Undo( );
}

// ****************************************************************************
//
//  Function Name:	RChangePaneViewBackgroundColorAction::Redo( )
//
//  Description:		re-do the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangePaneViewBackgroundColorAction::Redo( )
	{
	RUndoableAction::Redo( );
	}


#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RChangeBackgroundColorAction::Validate( )
//
//  Description:		Verify that the object is valid.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeBackgroundColorAction::Validate( ) const
{
	RUndoableAction::Validate( );
	TpsAssertIsObject( RChangeBackgroundColorAction, this );
}

#endif	// TPSDEBUG
