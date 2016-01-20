// ****************************************************************************
//
//  File Name:			ChainedUndoableAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RUndoableAction class
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
//  $Logfile:: /PM8/Framework/Include/ChainedUndoableAction.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CHAINEDUNDOABLEACTION_H_
#define		_CHAINEDUNDOABLEACTION_H_

#include		"UndoableAction.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RChainedUndoableAction
//
//  Description:	This is the base class for chained actions, eg. Drag&Drop
//
// ****************************************************************************
//
class FrameworkLinkage RChainedUndoableAction : public RUndoableAction
	{
	// Construction & destruction
	public :
													RChainedUndoableAction( const YActionId& actionId, uWORD uwUndoId, uWORD uwRedoId );
													RChainedUndoableAction( const YActionId& actionId, RScript& script );
		virtual									~RChainedUndoableAction( );

	// Protected constructor
	protected :
													RChainedUndoableAction( const YActionId& actionId, uWORD uwUndoId, uWORD uwRedoId, RChainedUndoableAction* pChainedAction );

	// Operations
	public :
		virtual	void							Undo( );
		virtual	void							Redo( );

	// Implementation
	private :
		void										ChainedUndo( );
		void										ChainedRedo( );

	// Member data
	protected :
		RChainedUndoableAction*				m_pChainedAction;
	};


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _CHAINEDUNDOABLEACTION_H_
