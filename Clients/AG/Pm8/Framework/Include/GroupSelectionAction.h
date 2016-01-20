// ****************************************************************************
//
//  File Name:			GroupSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RGroupSelectionAction class
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
//  $Logfile:: /PM8/Framework/Include/GroupSelectionAction.h                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GROUPSELECTIONACTION_H_
#define		_GROUPSELECTIONACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifndef		_VIEW_H_
#include		"View.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDocument;
class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	RGroupSelectionAction
//
//  Description:	Action to add a new component to a document
//
// ****************************************************************************
//
class FrameworkLinkage RGroupSelectionAction : public RUndoableAction
	{
	// Construction, destruction & Intialization
	public :
												RGroupSelectionAction( RCompositeSelection* pCurrentSelection );
												RGroupSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );
		virtual								~RGroupSelectionAction( );

	//	Identifier
	public :
		static	YActionId				m_ActionId;

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Member data
	private :
		RDocument*							m_pParentDocument;
		RComponentDocument*				m_pGroupDocument;
		RCompositeSelection*				m_pCurrentSelection;
		RCompositeSelection				m_OldSelection;
		RRealRect							m_GroupBoundingRect;
		YZOrderState						m_OldZOrder;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _GROUPSELECTIONACTION_H_
