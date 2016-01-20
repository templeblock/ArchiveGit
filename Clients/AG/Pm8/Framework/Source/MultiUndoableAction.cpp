// ****************************************************************************
//
//  File Name:			MultiUndoableAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RMultiUndoableAction class
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
//  $Logfile:: /PM8/Framework/Source/MultiUndoableAction.cpp                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"MultiUndoableAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "MultiUndoManager.h"

YActionId	RMultiUndoableAction::m_ActionId( "RMultiUndoableAction" );

// ****************************************************************************
//
//  Function Name:	RMultiUndoableAction::RMultiUndoableAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMultiUndoableAction::RMultiUndoableAction( RMultiUndoManager* pManager, uWORD uwUndoId, uWORD uwRedoId )
	: RUndoableAction( m_ActionId, uwUndoId, uwRedoId ),
	  m_pManager( pManager )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMultiUndoableAction::RMultiUndoableAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMultiUndoableAction::RMultiUndoableAction( RMultiUndoManager* pManager, RScript& script )
	: RUndoableAction( m_ActionId, script ),
	  m_pManager( pManager )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMultiUndoableAction::~RMultiUndoableAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMultiUndoableAction::~RMultiUndoableAction( )
	{
	delete m_pManager;
	}

// ****************************************************************************
//
//  Function Name:	RMultiUndoableAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMultiUndoableAction::Undo( )
	{
	RUndoableAction::Undo( );
	m_pManager->UndoAllAction( );
	}

// ****************************************************************************
//
//  Function Name:	RMultiUndoableAction::Redo( )
//
//  Description:		Process a Redo command 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMultiUndoableAction::Redo( )
	{
	m_asLastAction = kActionRedo;
	m_pManager->RedoAllAction( );
	}

// ****************************************************************************
//
//  Function Name:	RMultiUndoableAction::WriteScript( )
//
//  Description:		Write the script to someplace..
//
//  Returns:			TRUE
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMultiUndoableAction::WriteScript( RScript& /* script */) const
	{
	UnimplementedCode( );
	return	TRUE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RMultiUndoableAction::Validate( )
//
//  Description:		Validate the object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMultiUndoableAction::Validate( ) const
	{
	RUndoableAction::Validate();
	TpsAssertValid( m_pManager );
	}

#endif	//	TPSDEBUG
