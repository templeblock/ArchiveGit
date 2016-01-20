// ****************************************************************************
//
//  File Name:			DeactivateComponentAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RDeactivateComponentAction class
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
//  $Logfile:: /PM8/Framework/Source/DeactivateComponentAction.cpp            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"DeactivateComponentAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentView.h"
#include "CompositeSelection.h"

YActionId	RDeactivateComponentAction::m_ActionId( "RDeactivateComponentAction" );

// ****************************************************************************
//
//  Function Name:	RDeactivateComponentAction::RDeactivateComponentAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDeactivateComponentAction::RDeactivateComponentAction( RView* pParentView )
	: m_pParentView( pParentView ),
	  RTransparentUndoableAction( m_ActionId )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RDeactivateComponentAction::RDeactivateComponentAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDeactivateComponentAction::RDeactivateComponentAction( RView* pParentView, RScript& script )
	: m_pParentView( pParentView ),
	  RTransparentUndoableAction( m_ActionId, script )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RDeactivateComponentAction::Do( )
//
//  Description:		Deactivate the Component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDeactivateComponentAction::Do( )
	{
	RUndoableAction::Do( );
	m_pComponentView = m_pParentView->DeactivateComponentView( );
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RDeactivateComponentAction::Undo( )
//
//  Description:		Activate the component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeactivateComponentAction::Undo( )
	{
	RUndoableAction::Undo( );

	if( m_pComponentView )
		{
		m_pParentView->GetSelection( )->UnselectAll( );
		m_pParentView->ActivateComponentView( m_pComponentView );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDeactivateComponentAction::WriteScript( )
//
//  Description:		Write the script
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDeactivateComponentAction::WriteScript( RScript& script ) const
	{
	return RTransparentUndoableAction::WriteScript( script );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RDeactivateComponentAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeactivateComponentAction::Validate( ) const
	{
	RUndoableAction::Validate();
	TpsAssertIsObject( RDeactivateComponentAction, this );
	TpsAssertValid( m_pParentView );
	TpsAssertValid( m_pComponentView );
	}

#endif	//	TPSDEBUG
