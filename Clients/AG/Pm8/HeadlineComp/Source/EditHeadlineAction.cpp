// ****************************************************************************
//
//  File Name:			EditGraphicAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the REditGraphicAction class
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
//  $Logfile:: /PM8/HeadlineComp/Source/EditHeadlineAction.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"HeadlineIncludes.h"

ASSERTNAME

#include "EditHeadlineAction.h"
#include "EditHeadlineDlg.h"
#include "ApplicationGlobals.h"
#include "HeadlineDocument.h"
// #include "RenaissanceResource.h"
#include "HeadlineApplication.h"
#include "ComponentView.h"
#include "ComponentTypes.h"

YActionId	REditHeadlineAction::m_ActionId( "REditHeadlineAction" );

// ****************************************************************************
//
//  Function Name:	REditHeadlineAction::REditHeadlineAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
#include "HeadlineInterfaceImp.h"
REditHeadlineAction::REditHeadlineAction( RHeadlineDocument* pHeadlineDocument )
	: m_pHeadlineDocument( pHeadlineDocument ),
	  m_fHeadlineChanged( FALSE ),
	  RUndoableAction( m_ActionId, STRING_UNDO_EDIT_HEADLINE, STRING_REDO_EDIT_HEADLINE )
	{
	m_pHeadlineDocument->GetHeadlineData( &m_HeadlineDataStruct );

	REditHeadlineDlg	dialog( &m_HeadlineDataStruct, &m_pHeadlineDocument->HeadlineObject() );

	if ( dialog.DoModal() )
		{
		m_fHeadlineChanged = TRUE;
		dialog.FillData( &m_HeadlineDataStruct );
		}
	}	

// ****************************************************************************
//
//  Function Name:	REditHeadlineAction::REditHeadlineAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REditHeadlineAction::REditHeadlineAction( RHeadlineDocument* pHeadlineDocument, RScript& script )
	: m_pHeadlineDocument( pHeadlineDocument ),
	  m_fHeadlineChanged( TRUE ),
	  RUndoableAction( m_ActionId, script )
	{
	m_pHeadlineDocument->GetHeadlineData( &m_HeadlineDataStruct );
	}	

// ****************************************************************************
//
//  Function Name:	REditHeadlineAction::~REditHeadlineAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REditHeadlineAction::~REditHeadlineAction( )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	REditHeadlineAction::Do( )
//
//  Description:		Perform the action.
//
//  Returns:			TRUE: this does nothing other than set the state.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditHeadlineAction::Do( )
	{
	if ( !m_fHeadlineChanged )
		return FALSE;

	HeadlineDataStruct		currentHeadlineData;
	m_pHeadlineDocument->GetHeadlineData( &currentHeadlineData );
	m_pHeadlineDocument->SetHeadlineData( &m_HeadlineDataStruct );
	m_HeadlineDataStruct	= currentHeadlineData;

	// If this is a banner headline, we need to resize ourself
	if( m_pHeadlineDocument->GetComponentType( ) == kSpecialHeadlineComponent )
		{
		// Get the headline size
		RRealSize graphicSize = m_pHeadlineDocument->HeadlineObject( ).GetGraphic( )->GetGraphicSize( );

		// Make the component that size
		static_cast<RComponentView*>( m_pHeadlineDocument->GetActiveView( ) )->Resize( graphicSize );
		}

	// Invalidate the render cache
	static_cast<RComponentView*>( m_pHeadlineDocument->GetActiveView( ) )->InvalidateRenderCache( );

	// Notify our parent that we were editted
	RDocument*	pParentDocument	= m_pHeadlineDocument->GetParentDocument( );

	if( pParentDocument )
		pParentDocument->XUpdateAllViews( kLayoutChanged, 0 );

	return RUndoableAction::Do( );
	}

// ****************************************************************************
//
//  Function Name:	REditHeadlineAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditHeadlineAction::Undo( )
	{
	Do();
	RUndoableAction::Undo( );
	}

// ****************************************************************************
//
//  Function Name:	REditGraphicAction::WriteScript( )
//
//  Description:		Write the action data to a script.
//
//  Returns:			TRUE if sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN REditHeadlineAction::WriteScript( RScript& /* script */) const
	{
	return TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	REditHeadlineAction::Validate( )
//
//  Description:		Verify that the object is valid.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void REditHeadlineAction::Validate( ) const
	{
	RUndoableAction::Validate( );

	}

#endif	// TPSDEBUG
