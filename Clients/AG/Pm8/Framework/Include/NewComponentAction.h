// ****************************************************************************
//
//  File Name:			NewComponentAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RNewComponentAction class
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
//  $Logfile:: /PM8/Framework/Include/NewComponentAction.h                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_NEWCOMPONENTACTION_H_
#define		_NEWCOMPONENTACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDocument;
class RComponentDocument;
class RComponentView;
class RStandaloneApplicationBase;
class RComponentAttributes;
class RCompositeSelection;

// ****************************************************************************
//
//  Class Name:	RNewComponentAction
//
//  Description:	Action to add a new component to a document
//
// ****************************************************************************
//
class FrameworkLinkage RNewComponentAction : public RUndoableAction
	{
	// Construction, destruction & Intialization
	public :
												RNewComponentAction( RComponentDocument* pComponentDocument,
																			RDocument* pParentDocument,
																			RCompositeSelection* pCurrentSelection,
																			YResourceId undoRedoPairId );

		virtual								~RNewComponentAction( );

	//	Identifier
	public :
		static YActionId					m_ActionId;

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );

	// Accessors
	public:
		virtual RComponentDocument*	GetComponentDocument();
		
	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Member data
	private :
		RComponentDocument*				m_pComponentDocument;
		RDocument*							m_pParentDocument;
		RCompositeSelection*				m_pCurrentSelection;
		RComponentView*					m_pActiveView;

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

#endif		// _NEWCOMPONENTACTION_H_
