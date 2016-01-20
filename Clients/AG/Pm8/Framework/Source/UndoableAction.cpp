// ****************************************************************************
//
//  File Name:			UndoableAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RUndoableAction class
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
//  $Logfile:: /PM8/Framework/Source/UndoableAction.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"UndoableAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"

// ****************************************************************************
//
//  Function Name:	RUndoableAction::RUndoableAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUndoableAction::RUndoableAction( const YActionId& actionId, uWORD uwUndoId, uWORD uwRedoId )
	: RAction( actionId ),
	  m_asLastAction( kActionNotDone ),
	  m_uwUndoStringId( uwUndoId ),
	  m_uwRedoStringId( uwRedoId ),
	  m_pUndoManager( NULL )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RUndoableAction::RUndoableAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUndoableAction::RUndoableAction( const YActionId& actionId, RScript& script )
	: RAction( actionId, script ),
	  m_asLastAction( kActionNotDone ),
	  m_pUndoManager( NULL )
	{
	script >> m_uwUndoStringId >> m_uwRedoStringId;
	}

// ****************************************************************************
//
//  Function Name:	RUndoableAction::~RUndoableAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUndoableAction::~RUndoableAction( )
	{
	;
	}


// ****************************************************************************
//
//  Function Name:	RUndoableAction::Do( )
//
//  Description:		Perform the action.
//
//  Returns:			TRUE: this does nothing other than set the state.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RUndoableAction::Do( )
	{
	m_asLastAction = kActionDo;
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RUndoableAction::Undo( )
//
//  Description:		Process a Undo command.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoableAction::Undo( )
	{
	m_asLastAction = kActionUndo;
	}

// ****************************************************************************
//
//  Function Name:	RUndoableAction::Redo( )
//
//  Description:		Process a Redo command (default is to call do).
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoableAction::Redo( )
	{
#ifdef TPSDEBUG
	TpsAssert( Do( ), "Do( ) returned FALSE when called from Redo( )" );
#else
	Do( );
#endif

	m_asLastAction = kActionRedo;
	}

// ****************************************************************************
//
//  Function Name:	RUndoableAction::WriteScript( )
//
//  Description:		Process a Redo command (default is to call do).
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RUndoableAction::WriteScript( RScript& script ) const
	{
	script << m_uwUndoStringId << m_uwRedoStringId;
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RUndoableAction::GetUndoStringId( )
//
//  Description:		Return the value of the UndoString Id.
//
//  Returns:			m_uwUndoStringId.
//
//  Exceptions:		None
//
// ****************************************************************************
//
uWORD RUndoableAction::GetUndoStringId( ) const
	{
	return m_uwUndoStringId;
	}

// ****************************************************************************
//
//  Function Name:	RUndoableAction::GetRedoStringId( )
//
//  Description:		Return the value of the UndoString Id.
//
//  Returns:			m_uwUndoStringId.
//
//  Exceptions:		None
//
// ****************************************************************************
//
uWORD RUndoableAction::GetRedoStringId( ) const
	{
	return m_uwRedoStringId;
	}

// ****************************************************************************
//
//  Function Name:	RUndoableAction::GetState( )
//
//  Description:		Return the current state of the actin
//
//  Returns:			m_asLastAction;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUndoableAction::EActionState RUndoableAction::GetState( ) const
	{
	return m_asLastAction;
	}


// ****************************************************************************
//
//  Function Name:	RUndoableAction::GetUndoManager( )
//
//  Description:		Return the undo manager
//
//  Returns:			m_pUndoManager;
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUndoManager* RUndoableAction::GetUndoManager( ) const
	{
	return m_pUndoManager;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RUndoableAction::Validate( )
//
//  Description:	Validate the object.
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoableAction::Validate( ) const
	{
	RAction::Validate();
	TpsAssert( ( m_asLastAction >= kActionNotDone ) && ( m_asLastAction <= kActionRedo ), "Invalid Last Action State" );
	}

#endif	//	TPSDEBUG
