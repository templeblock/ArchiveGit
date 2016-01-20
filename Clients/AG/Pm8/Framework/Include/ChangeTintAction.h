// ****************************************************************************
//
//  File Name:			EditLineAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				G. Brown
//
//  Description:		Declaration of the REditLineAction class
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
//  $Logfile:: /PM8/Framework/Include/ChangeTintAction.h                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CHANGETINTACTION_H_
#define		_CHANGETINTACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RComponentDocument;

// ****************************************************************************
//
//  Class Name:		RChangeTintAction
//
//  Description:		Changes the tint of a component
//
// ****************************************************************************
class RChangeTintAction : public RUndoableAction
{
	// Construction & destruction
	public:
													RChangeTintAction(RComponentDocument* pComponentDocument, YTint tint );
													RChangeTintAction(RComponentDocument* pComponentDocument, RScript& script );
		virtual									~RChangeTintAction();

	//	Identifier
	public :
		static YActionId						m_ActionId;

	// Operations
	public:
		virtual BOOLEAN						Do();
		virtual void							Undo();

	// Scripting Operations
	public :
		virtual BOOLEAN						WriteScript( RScript& script ) const;

	//	Private Member Data
	private:
		RComponentDocument*					m_pComponentDocument;
		YTint										m_Tint;

#ifdef TPSDEBUG
	//	Debugging code
	public:
		virtual void							Validate() const;
#endif
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_CHANGETINTACTION_H_
