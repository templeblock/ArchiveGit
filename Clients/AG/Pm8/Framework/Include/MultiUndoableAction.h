// ****************************************************************************
//
//  File Name:			MultiUndoableAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RMultiUndoableAction class
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
//  $Logfile:: /PM8/Framework/Include/MultiUndoableAction.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MULTIUNDOABLEACTION_H_
#define		_MULTIUNDOABLEACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class	RMultiUndoManager;

// ****************************************************************************
//
//  Class Name:		RMultiUndoableAction
//
//  Description:	This class will allow the grouping of Undoable actions into
//						and action to be performed as a single group.
//
// ****************************************************************************
//
class FrameworkLinkage RMultiUndoableAction : public RUndoableAction
	{
	// Construction & destruction
	public :
													RMultiUndoableAction( RMultiUndoManager* pManager, uWORD uwUndoId, uWORD uwRedoId );
													RMultiUndoableAction( RMultiUndoManager* pManager, RScript& script );
		virtual									~RMultiUndoableAction( );

	//	Identifier
	public :
		static	YActionId					m_ActionId;

	// Operations
	public :
		virtual void							Undo( );
		virtual void							Redo( );

	// Scripting Operations
	public :
		virtual BOOLEAN						WriteScript( RScript& script ) const;

	//	Private Member Data
	private :
		RMultiUndoManager*					m_pManager;

#ifdef	TPSDEBUG
	//	Debugging code
	public :
		virtual	void							Validate( ) const;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _MULTIUNDOABLEACTION_H_
