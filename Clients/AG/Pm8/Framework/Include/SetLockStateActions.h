// ****************************************************************************
//
//  File Name:			SetLockStateActions.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the Lock and Unlock actions
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
//  $Logfile:: /PM8/Framework/Include/SetLockStateActions.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_LOCKUNLOCKACTIONS_H_
#define		_LOCKUNLOCKACTIONS_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RComponentDocument;

typedef RArray<BOOLEAN> YLockStateCollection;
typedef YLockStateCollection::YIterator YLockStateIterator;

// ****************************************************************************
//
//  Class Name:	RLockSelectionAction
//
//  Description:	Action to lock or unlock a selection
//
// ****************************************************************************
//
class FrameworkLinkage RSetLockStateAction : public RUndoableAction
	{
	// Construction, destruction & Intialization
	public :
												RSetLockStateAction( RCompositeSelection* pCurrentSelection,
																			BOOLEAN fLock,
																			const YActionId& actionId,
																			uWORD uwUndoId,
																			uWORD uwRedoId );

												RSetLockStateAction( RCompositeSelection* pCurrentSelection,
																			BOOLEAN fLock,
																			RScript& script,
																			const YActionId& actionId );

		virtual								~RSetLockStateAction( ) = 0;

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );
		virtual void						Redo( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Member data
	private :
		RCompositeSelection*				m_pCurrentSelection;
		RCompositeSelection				m_OldSelection;
		BOOLEAN								m_fLock;
		YLockStateCollection				m_LockStateCollection;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RLockSelectionAction
//
//  Description:	Action to lock a selection
//
// ****************************************************************************
//
class FrameworkLinkage RLockSelectionAction : public RSetLockStateAction
	{
	// Construction, destruction & Intialization
	public :
												RLockSelectionAction( RCompositeSelection* pCurrentSelection );
												RLockSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );

	//	Identifier
	public :
		static	YActionId				m_ActionId;
	};

// ****************************************************************************
//
//  Class Name:	RUnlockSelectionAction
//
//  Description:	Action to unlock a selection
//
// ****************************************************************************
//
class FrameworkLinkage RUnlockSelectionAction : public RSetLockStateAction
	{
	// Construction, destruction & Intialization
	public :
												RUnlockSelectionAction( RCompositeSelection* pCurrentSelection );
												RUnlockSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );

	//	Identifier
	public :
		static	YActionId				m_ActionId;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _LOCKUNLOCKACTIONS_H_
