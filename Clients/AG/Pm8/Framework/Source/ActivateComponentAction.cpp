// ****************************************************************************
//
//  File Name:			ActivateComponentAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RActivateComponentAction class
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
//  $Logfile:: /PM8/Framework/Source/ActivateComponentAction.cpp              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ActivateComponentAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentView.h"
#include "Script.h"
#include "CompositeSelection.h"

YActionId	RActivateComponentAction::m_ActionId( "RActivateComponentAction" );

// ****************************************************************************
//
//  Function Name:	RActivateComponentAction::RActivateComponentAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RActivateComponentAction::RActivateComponentAction( RView* pParentView, RComponentView* pComponentView )
	: m_pParentView( pParentView ),
	  m_pComponentView( pComponentView ),
	  m_pCurrentSelection( pParentView->GetSelection( ) ),
	  RTransparentUndoableAction( m_ActionId )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RActivateComponentAction::RActivateComponentAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RActivateComponentAction::RActivateComponentAction( RView* pParentView, RScript& script )
	: m_pParentView( pParentView ),
	  m_pComponentView( NULL ),
	  m_pCurrentSelection( pParentView->GetSelection( ) ),
	  RTransparentUndoableAction( m_ActionId, script )
	{
	YComponentViewIndex	index;
	m_pCurrentSelection->Read( script );
	script >> index;
	m_pComponentView	= pParentView->GetComponentViewByIndex( index );
	}

// ****************************************************************************
//
//  Function Name:	RActivateComponentAction::Do( )
//
//  Description:		Activate the Component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RActivateComponentAction::Do( )
	{
	BOOLEAN	fActivated = TRUE;
	RUndoableAction::Do( );
	try
		{
		// Unselect everything
		m_pCurrentSelection->UnselectAll( );

		// Activate
		m_pParentView->ActivateComponentView( m_pComponentView );
		}
	catch( ... )
		{
		fActivated = FALSE;
		}

	return fActivated;
	}

// ****************************************************************************
//
//  Function Name:	RActivateComponentAction::Undo( )
//
//  Description:		Deactivate the component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RActivateComponentAction::Undo( )
	{
	RUndoableAction::Undo( );

	// Deactivate
	m_pParentView->DeactivateComponentView( );
	}

// ****************************************************************************
//
//  Function Name:	RActivateComponentAction::WriteScript( )
//
//  Description:		Write the script
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RActivateComponentAction::WriteScript( RScript& script ) const
	{
	if ( RTransparentUndoableAction::WriteScript( script ) )
		{
		YComponentViewIndex	index	= m_pParentView->GetIndexOfComponentView( m_pComponentView );
		m_pCurrentSelection->Read( script );
		script << index;
		return TRUE;
		}
	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RActivateComponentAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RActivateComponentAction::Validate( ) const
	{
	RUndoableAction::Validate();
	TpsAssertIsObject( RActivateComponentAction, this );
	TpsAssertValid( m_pParentView );
	TpsAssertValid( m_pComponentView );
	}

#endif	//	TPSDEBUG
