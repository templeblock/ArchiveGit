// ****************************************************************************
//
//  File Name:			EditComponentAttributesAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the REditComponentAttributesAction class
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
//  $Logfile:: /PM8/Framework/Include/EditComponentAttribAction.h             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_EDITCOMPONENTATTRIBUTESACTION_H_
#define		_EDITCOMPONENTATTRIBUTESACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPONENTATTRIBUTES_H_
#include		"ComponentAttributes.h"
#endif

#ifndef		_VIEW_H_
#include		"View.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RCompositeSelection;
class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	REditComponentAttributesAction
//
//  Description:	Action to add a new component to a document
//
// ****************************************************************************
//
class FrameworkLinkage REditComponentAttributesAction : public RUndoableAction
	{
	// Construction, destruction & Intialization
	public :
												REditComponentAttributesAction( RCompositeSelection* pCurrentSelection );
												REditComponentAttributesAction( RCompositeSelection* pCurrentSelection, RScript& script );
		virtual								~REditComponentAttributesAction( );

	//	Identifier
	public :
		static	YActionId					m_ActionId;

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
		BOOLEAN								m_fAttributesChanged;
		RComponentAttributes				m_OldComponentAttributes;
		RComponentAttributes				m_NewComponentAttributes;
		RCompositeSelection*				m_pCurrentSelection;
		RComponentDocument*				m_pComponentDocument;
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

#endif		// _EDITCOMPONENTATTRIBUTESACTION_H_
