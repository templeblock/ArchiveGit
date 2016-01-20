// ****************************************************************************
//
//  File Name:			PasteAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RPasteAction class
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
//  $Logfile:: /PM8/Framework/Include/PasteAction.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PASTEACTION_H_
#define		_PASTEACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifndef		_COMPONENTCOLLECTION_H_
#include		"ComponentCollection.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDocument;
class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	RPasteActionBase
//
//  Description:	Base class for PasteAction and DropAction
//
// ****************************************************************************
//
class FrameworkLinkage RPasteActionBase : public RUndoableAction
	{
	// Construction, destruction & Intialization
	public :
												RPasteActionBase( RDocument* pDocument, const YActionId& actionId );
												RPasteActionBase( RDocument* pDocument, RScript& script, const YActionId& actionId );
		virtual								~RPasteActionBase( ) = 0;

	// Initializer
	protected :
		void									Initialize( const RRealPoint& pasteCenterPoint );

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Implementation
	private :
		void									SetNewComponentPositions( RComponentCollection& componentCollection ) const;

	// Member data
	protected :
		RDocument*							m_pDocument;
		RCompositeSelection*				m_pCurrentSelection;
		RCompositeSelection				m_OldSelection;
		RComponentCollection				m_PastedComponentCollection;
		RRealPoint							m_PasteCenterPoint;
		RComponentCollection				m_ReplacedComponentCollection;
		BOOLEAN								m_fWarnAboutConvertedComponents;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RPasteAction
//
//  Description:	Pastes
//
// ****************************************************************************
//
class FrameworkLinkage RPasteAction : public RPasteActionBase
	{
	// Construction, destruction & Intialization
	public :
												RPasteAction( RDocument* pDocument );
												RPasteAction( RDocument* pDocument, RScript& script );

	// Implementation
	private :
		void									DoPaste( );

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

// ****************************************************************************
//
//  Class Name:	RPasteHereAction
//
//  Description:	Pastes
//
// ****************************************************************************
//
class FrameworkLinkage RPasteHereAction : public RPasteActionBase
	{
	// Construction, destruction & Intialization
	public :
												RPasteHereAction( RDocument* pDocument, const RRealPoint& point );

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

// ****************************************************************************
//
//  Class Name:	RPasteHereAction
//
//  Description:	Pastes
//
// ****************************************************************************
//
class FrameworkLinkage RPasteComponentHereAction : public RPasteActionBase
	{
	// Construction, destruction & Intialization
	public :
												RPasteComponentHereAction( RDocument* pDocument, RComponentDocument* pComponent, const RRealPoint& point );

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

// ****************************************************************************
//
//  Class Name:	RDropAction
//
//  Description:	Drops
//
// ****************************************************************************
//
class FrameworkLinkage RDropAction : public RPasteActionBase
	{
	// Construction, destruction & Intialization
	public :
												RDropAction( RDocument* pDocument, RComponentCollection& componentCollection, const RRealPoint& dropPoint, BOOLEAN fConvertedComponent );

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

#endif		// _PASTEACTION_H_
