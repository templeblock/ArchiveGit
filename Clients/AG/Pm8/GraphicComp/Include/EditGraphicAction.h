// ****************************************************************************
//
//  File Name:			EditGraphicAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the REditGraphicAction class
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
//  $Logfile:: /PM8/GraphicComp/Include/EditGraphicAction.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_EDITGRAPHICACTION_H_
#define		_EDITGRAPHICACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

class RGraphic;
class RGraphicDocument;

// ****************************************************************************
//
//  Class Name:	InfoMapEntry
//
//  Description:	Struct which collects graphic attributes.
//
// ****************************************************************************
//
struct InfoMapEntry
	{
	InfoMapEntry( const YComponentType& componentType, const RMBCString& filterString, const BOOLEAN fMonoColorable, const BOOLEAN fColorColorable );
	YComponentType		m_ComponentType;
	RMBCString			m_FilterString;
	BOOLEAN				m_fMonoColorable;		//	TRUE: the component can have a color applied if it is monochrome
	BOOLEAN				m_fColorColorable;	//	TRUE: the component can have a color applied if it is color
	};

//	symbolic defines that identify graphic attributes.
const BOOLEAN kHasColor = TRUE;

// ****************************************************************************
//
//  Class Name:	REditGraphicAction
//
//  Description:	This is the base class for Renaissance undoable xActions.
//
// ****************************************************************************
//
class REditGraphicAction : public RUndoableAction
	{
	// Construction & destruction
	public :
													REditGraphicAction( RGraphicDocument* pGraphicDocument );
													REditGraphicAction( RGraphicDocument* pGraphicDocument, RScript& script );
		virtual									~REditGraphicAction( );

	//	Identifier
	public :
		static	YActionId					m_ActionId;

	// Operations
	public :
		virtual	BOOLEAN						Do( );
		virtual	void							Undo( );
		virtual	void							SetAttributes( const InfoMapEntry& Info );

	// Scripting Operations
	public :
		virtual BOOLEAN						WriteScript( RScript& script ) const;

	//	Private Member Data
	private :
		RGraphic*								m_pGraphic;
		RGraphicDocument*						m_pGraphicDocument;

#ifdef	TPSDEBUG
	//	Debugging code
	public :
		virtual	void							Validate( ) const;
#endif
	};

#endif	//	_EDITGRAPHICACTION_H_
