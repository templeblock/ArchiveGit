// ****************************************************************************
//
//  File Name:			MultiUndoManager.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RMultiUndoManager class
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
//  $Logfile:: /PM8/Framework/Source/MultiUndoManager.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "MultiUndoManager.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

const	YCounter	kMaxMultiUndoCount = 32000;

// ****************************************************************************
//
//  Function Name:	RMultiUndoManager::RMultiUndoManager( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMultiUndoManager::RMultiUndoManager( RUndoManager* pPreviousManager )
	: m_pPreviousUndoManager( pPreviousManager )
	{
	m_MaximumUndos = kMaxMultiUndoCount;
	}

// ****************************************************************************
//
//  Function Name:	RMultiUndoManager::~RMultiUndoManager
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMultiUndoManager::~RMultiUndoManager( )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RMultiUndoManager::UndoAllAction
//
//  Description:		Undo all of the actions in the Undo list
//
//  Returns:			Nothing
//
//  Exceptions:		Whatever an action may throw
//
// ****************************************************************************
//
void RMultiUndoManager::UndoAllAction( )
	{
	// Until the undo pointer wraps around...
	while (m_CurrentUndo != m_UndoList.End())
		UndoAction( );
	}

// ****************************************************************************
//
//  Function Name:	RMultiUndoManager::RedoAllAction
//
//  Description:		Redo all of the actions in the Undo list
//
//  Returns:			Nothing
//
//  Exceptions:		Whatever an action may throw
//
// ****************************************************************************
//
void RMultiUndoManager::RedoAllAction( )
	{
	TpsAssert( m_CurrentUndo == m_UndoList.End(), "Current Undo Pointer is not at start of list.");

	// Until the undo pointer wraps around...
	RUndoList::YIterator	endOfList = m_UndoList.End();
	--endOfList;
	while (m_CurrentUndo != endOfList)
		RedoAction( );
	}

// ****************************************************************************
//
//  Function Name:	RMultiUndoManager::GetPreviousUndoManager
//
//  Description:		Return a pointer to the previous undo manager
//
//  Returns:			m_pPreviousUndoManager
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUndoManager* RMultiUndoManager::GetPreviousUndoManager( ) const
	{
	return m_pPreviousUndoManager;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RMultiUndoManager::Validate
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMultiUndoManager::Validate( ) const
	{
	RUndoManager::Validate( );
	TpsAssert( m_UndoList.Count() <= 100,"There are over 100 entries in this MultiUndo layer. Should this be done differently" );
	}

#endif	//	TPSDEBUG
