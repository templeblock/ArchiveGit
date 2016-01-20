// ****************************************************************************
//
//  File Name:			DeleteSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RDeleteSelectionAction class
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
//  $Logfile:: /PM8/Framework/Include/DeleteSelectionAction.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DELETESELECTIONACTION_H_
#define		_DELETESELECTIONACTION_H_

#include		"UndoableAction.h"
#include		"View.h"
#include		"CompositeSelection.h"
#include		"ComponentCollection.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RDeleteAction
//
//  Description:	Base class for delete selection actions
//
// ****************************************************************************
//
class FrameworkLinkage RDeleteAction : public RUndoableAction
	{
	// Construction, destruction & Intialization
	public :
												RDeleteAction( RCompositeSelection* pCurrentSelection,
																	const YActionId& actionId );

		virtual								~RDeleteAction( );

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );
		virtual void						Redo( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Implementation
	private :
		virtual BOOLEAN					ShouldDelete( RComponentDocument* pComponent ) const;

	// Member data
	protected :
		RCompositeSelection*				m_pCurrentSelection;
		RCompositeSelection				m_OldSelection;
		RComponentCollection				m_ComponentsToDelete;
		RComponentCollection				m_DeletedComponents;
		YZOrderState						m_OldZOrder;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RDeleteComponentAction
//
//  Description:	Action to delete a single component
//
// ****************************************************************************
//
class FrameworkLinkage RDeleteComponentAction : public RDeleteAction
	{
	// Construction, destruction & Intialization
	public :
												RDeleteComponentAction( RCompositeSelection* pCurrentSelection,
																				RComponentDocument* pComponent );

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

// ****************************************************************************
//
//  Class Name:	RDeleteSelectionAction
//
//  Description:	Normal delete selection action
//
// ****************************************************************************
//
class FrameworkLinkage RDeleteSelectionAction : public RDeleteAction
	{
	// Construction, destruction & Intialization
	public :
												RDeleteSelectionAction( RCompositeSelection* pCurrentSelection );

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

// ****************************************************************************
//
//  Class Name:	RDragDropDeleteSelectionAction
//
//  Description:	Delete selection action used for the end of a drag&drop
//
// ****************************************************************************
//
class FrameworkLinkage RDragDropDeleteSelectionAction : public RDeleteAction
	{
	// Construction, destruction & Intialization
	public :
												RDragDropDeleteSelectionAction( RCompositeSelection* pCurrentSelection );

	//	Identifier
	public :
		static YActionId					m_ActionId;

	// Implementation
	virtual BOOLEAN						ShouldDelete( RComponentDocument* pComponentView ) const;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _DELETESELECTIONACTION_H_
