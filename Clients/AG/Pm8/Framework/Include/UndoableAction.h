// ****************************************************************************
//
//  File Name:			UndoableAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas, M. Houle
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
//  $Logfile:: /PM8/Framework/Include/UndoableAction.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_UNDOABLEACTION_H_
#define		_UNDOABLEACTION_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RUndoManager;

// ****************************************************************************
//
//  Class Name:	RUndoableAction
//
//  Description:	This is the base class for Renaissance undoable xActions.
//
// ****************************************************************************
//
class FrameworkLinkage RUndoableAction : public RAction
	{
	// Construction & destruction
	public :
													RUndoableAction( const YActionId& actionId, uWORD uwUndoId, uWORD uwRedoId );
													RUndoableAction( const YActionId& actionId, RScript& script );
		virtual									~RUndoableAction( );

	// Operations
	public :
		virtual	BOOLEAN						Do( );
		virtual	void							Undo( ) = 0;
		virtual	void							Redo( );

		virtual	BOOLEAN						IsUndoable( ) const;
		virtual	BOOLEAN						IsTransparent( ) const;

		uWORD										GetUndoStringId( ) const;
		uWORD										GetRedoStringId( ) const;

	// Scripting Operations
	public :
		virtual BOOLEAN						WriteScript( RScript& script ) const = 0;

	//	State Information
	public :
		enum	EActionState { kActionNotDone = 0, kActionDo, kActionUndo, kActionRedo };
 		EActionState							GetState( ) const;
		RUndoManager*							GetUndoManager( ) const;
		void									SetUndoManager(RUndoManager *manager )
		{
			m_pUndoManager = manager;
		}

	//	Member Data
	protected :
		EActionState							m_asLastAction;
		uWORD										m_uwUndoStringId;
		uWORD										m_uwRedoStringId;
		RUndoManager*							m_pUndoManager;

	friend class RUndoManager;

#ifdef	TPSDEBUG
	//	Debugging code
	public :
		virtual	void							Validate( ) const = 0;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************
	
// ****************************************************************************
//
//  Function Name:	RUndoableAction::IsUndoable( )
//
//  Description:		Determines whether or not this action is undoable or not.
//
//  Returns:			TRUE, as RUndoableActions are by definition undoable
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RUndoableAction::IsUndoable( ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RUndoableAction::IsTransparent( )
//
//  Description:		Determines whether or not this action is transparent or not.
//
//  Returns:			FALSE, as RUndoableActions are by definition not transparent
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RUndoableAction::IsTransparent( ) const
	{
	return FALSE;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _UNDOABLEACTION_H_
