// ****************************************************************************
//
//  File Name:			ChangeColorAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RChangeColorAction class
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
//  $Logfile:: /PM8/GraphicComp/Source/ChangeColorAction.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#include	"ChangeColorAction.h"

#include "Script.h"
#include "ApplicationGlobals.h"
#include "Document.h"
#include "Graphic.h"
#include "FileStream.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"

YActionId RChangeColorAction::m_ActionId( "RChangeColorAction" );

// ****************************************************************************
//
//  Function Name:	RChangeColorAction::RChangeColorAction()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//

RChangeColorAction::RChangeColorAction( RDocument* pDocument, RGraphic* pGraphic, const RColor& color )
: RUndoableAction( m_ActionId, STRING_UNDO_CHANGE_COLOR, STRING_REDO_CHANGE_COLOR ),
	m_pDocument( pDocument ),
	m_pGraphic( pGraphic ),
	m_Color( color )
{		
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangeColorAction::RChangeColorAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChangeColorAction::RChangeColorAction( RDocument* pDocument, RGraphic* pGraphic, RScript& script)
: RUndoableAction( m_ActionId, script ),
	m_pDocument( pDocument ),
	m_pGraphic( pGraphic )
{
	script >> m_Color;
}

// ****************************************************************************
//
//  Function Name:	RChangeColorAction::~RChangeColorAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RChangeColorAction::~RChangeColorAction( )
{	
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RChangeColorAction::Do( )
//
//  Description:		Perform the action.
//
//  Returns:			TRUE if action performed sucessfully
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RChangeColorAction::Do()
{	
	RColor	color( m_pGraphic->GetMonochromeColor() );
	m_pGraphic->SetMonochromeColor( m_Color );
	m_Color = color;

	m_pDocument->InvalidateAllViews( );
	(static_cast<RComponentView*>(m_pDocument->GetActiveView( )))->InvalidateRenderCache( );
	
	// Call the base method
	return RUndoableAction::Do();
}

// ****************************************************************************
//
//  Function Name:	RChangeColorAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeColorAction::Undo()
{	
	Do();
	RUndoableAction::Undo( );
}

// ****************************************************************************
//
//  Function Name:	RChangeColorAction::WriteScript( )
//
//  Description:		Write the action data to a script.
//
//  Returns:			TRUE if sucessful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RChangeColorAction::WriteScript( RScript& script ) const
{
	if ( RUndoableAction::WriteScript( script ) )
	{
		script << m_Color;
		return TRUE;
	}
	return FALSE;
}
#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RChangeColorAction::Validate( )
//
//  Description:		Verify that the object is valid.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RChangeColorAction::Validate( ) const
{
	RUndoableAction::Validate( );
	TpsAssertIsObject( RGraphic, m_pGraphic );
	TpsAssertIsObject( RDocument, m_pDocument );
	TpsAssert( m_pGraphic->IsMonochrome(), "Setting color of non-monochrome graphic" );
}

#endif	// TPSDEBUG
