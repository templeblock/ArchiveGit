// ****************************************************************************
//
//  File Name:			MoveSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RMoveSelectionActionBase class
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
//  $Logfile:: /PM8/Framework/Include/MoveSelectionAction.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MOVESELECTIONACTION_H_
#define		_MOVESELECTIONACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RMoveSelectionActionBase
//
//  Description:	Base class for move selection actions
//
// ****************************************************************************
//
class FrameworkLinkage RMoveSelectionActionBase : public RUndoableAction
	{
	// Construction
	public :
												RMoveSelectionActionBase( RCompositeSelection* pCurrentSelection,
																				  const RRealSize& offset,
																				  const YActionId& actionId );

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );
		virtual void						Redo( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Implementation
	protected :
		void									DoMove( const RRealSize& offset ) const;

	// Member data
	protected :
		RCompositeSelection*				m_pCurrentSelection;
		RCompositeSelection				m_OldSelection;
		RRealSize							m_Offset;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RMoveSelectionAction
//
//  Description:	Action to move a component
//
// ****************************************************************************
//
class FrameworkLinkage RMoveSelectionAction : public RMoveSelectionActionBase
	{
	// Construction
	public :
												RMoveSelectionAction( RCompositeSelection* pCurrentSelection, const RRealSize& offset );

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

// ****************************************************************************
//
//  Class Name:	RNudgeSelectionAction
//
//  Description:	Action to nudge a component
//
// ****************************************************************************
//
class FrameworkLinkage RNudgeSelectionAction : public RMoveSelectionActionBase
	{
	// Construction
	public :
												RNudgeSelectionAction( RCompositeSelection* pCurrentSelection );

	// Operations
	public :
		void									Nudge( const RRealSize& nudgeAmount );

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _MOVESELECTIONACTION_H_
