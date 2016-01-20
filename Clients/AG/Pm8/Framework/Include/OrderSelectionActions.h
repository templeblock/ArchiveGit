// ****************************************************************************
//
//  File Name:			OrderSelectionActions.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the order selection action classes
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
//  $Logfile:: /PM8/Framework/Include/OrderSelectionActions.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_ORDERSELECTIONACTIONS_H_
#define		_ORDERSELECTIONACTIONS_H_

#include		"UndoableAction.h"
#include		"CompositeSelection.h"
#include		"View.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	ROrderSelectionActionBase
//
//  Description:	Base class for the order selection actions
//
// ****************************************************************************
//
class ROrderSelectionActionBase : public RUndoableAction
	{
	// Construction & Destruction
	public :
												ROrderSelectionActionBase( RCompositeSelection* pCurrentSelection,
																					const YActionId& actionId,
																					uWORD uwUndoId,
																					uWORD uwRedoId);

												ROrderSelectionActionBase( RCompositeSelection* pCurrentSelection,
																					const YActionId& actionId,
																					RScript& script );

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );
		virtual void						Redo( );
		virtual void						DoOrdering( ) = 0;

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Member data
	protected :
		RCompositeSelection*				m_pCurrentSelection;

	// Member data
	private :
		RCompositeSelection				m_OldSelection;
		YZOrderState						m_OldZOrder;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RBringSelectionForwardAction
//
//  Description:	Action to move a selection forward in the z-order
//
// ****************************************************************************
//
class RBringSelectionForwardAction : public ROrderSelectionActionBase
	{
	// Construction & Destruction
	public :
												RBringSelectionForwardAction( RCompositeSelection* pCurrentSelection );
												RBringSelectionForwardAction( RCompositeSelection* pCurrentSelection,	RScript& script );
	//	Identifier
	public :
		static YActionId					m_ActionId;

	// Operations
	public :
		virtual void						DoOrdering( );
	};

// ****************************************************************************
//
//  Class Name:	RSendSelectionBackwardAction
//
//  Description:	Action to move a selection forward in the z-order
//
// ****************************************************************************
//
class RSendSelectionBackwardAction : public ROrderSelectionActionBase
	{
	// Construction & Destruction
	public :
												RSendSelectionBackwardAction( RCompositeSelection* pCurrentSelection );
												RSendSelectionBackwardAction( RCompositeSelection* pCurrentSelection,	RScript& script );
	//	Identifier
	public :
		static YActionId					m_ActionId;

	// Operations
	public :
		virtual void						DoOrdering( );
	};

// ****************************************************************************
//
//  Class Name:	RBringSelectionToFrontAction
//
//  Description:	Action to move a selection to the front of the z-order
//
// ****************************************************************************
//
class RBringSelectionToFrontAction : public ROrderSelectionActionBase
	{
	// Construction & Destruction
	public :
												RBringSelectionToFrontAction( RCompositeSelection* pCurrentSelection );
												RBringSelectionToFrontAction( RCompositeSelection* pCurrentSelection,	RScript& script );
	//	Identifier
	public :
		static YActionId					m_ActionId;

	// Operations
	public :
		virtual void						DoOrdering( );
	};

// ****************************************************************************
//
//  Class Name:	RSendSelectionToBackAction
//
//  Description:	Action to move a selection to the back of the z-order
//
// ****************************************************************************
//
class RSendSelectionToBackAction : public ROrderSelectionActionBase
	{
	// Construction & Destruction
	public :
												RSendSelectionToBackAction( RCompositeSelection* pCurrentSelection );
												RSendSelectionToBackAction( RCompositeSelection* pCurrentSelection,	RScript& script );
	//	Identifier
	public :
		static YActionId					m_ActionId;

	// Operations
	public :
		virtual void						DoOrdering( );
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _ORDERSELECTIONACTIONS_H_
