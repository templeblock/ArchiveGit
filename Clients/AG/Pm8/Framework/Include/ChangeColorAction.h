// ****************************************************************************
//
//  File Name:			ChangeColorAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RChangeColorAction class
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
//  $Logfile:: /PM8/Framework/Include/ChangeColorAction.h               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CHANGECOLORACTION_H_
#define		_CHANGECOLORACTION_H_

#pragma pack(push, BYTE_ALIGNMENT)

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

class RDocument;
class RGraphic;

// ****************************************************************************
//
//  Class Name:		RChangeColorAction
//
//  Description:		Changes the tint of a component
//
// ****************************************************************************
class FrameworkLinkage RChangeColorAction : public RUndoableAction
{
	// Construction & destruction
	public:
													RChangeColorAction(RDocument* pDocument, RGraphic* pComponentDocument, const RColor& color );
													RChangeColorAction(RDocument* pDocument, RGraphic* pComponentDocument, RScript& script );
		virtual									~RChangeColorAction( );

	//	Identifier
	public :
		static YActionId m_ActionId;

	// Operations
	public:
		virtual BOOLEAN						Do();
		virtual void							Undo();

	// Scripting Operations
	public :
		virtual BOOLEAN						WriteScript( RScript& script ) const;

	//	Private Member Data
	private:
		RDocument*								m_pDocument;
		RGraphic*								m_pGraphic;
		RColor									m_Color;

#ifdef TPSDEBUG
	//	Debugging code
	public:
		virtual void Validate() const;
#endif
};

#pragma pack(pop)

#endif	//	_CHANGECOLORACTION_H_
