// ****************************************************************************
//
//  File Name:			DeactivateComponentAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RDeactivateComponentAction class
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
//  $Logfile:: /PM8/Framework/Include/DeactivateComponentAction.h             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DEACTIVATECOMPONENTACTION_H_
#define		_DEACTIVATECOMPONENTACTION_H_

#ifndef		_TRANSPARENTUNDOABLEACTION_H_
#include		"TransparentUndoableAction.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RView;
class RComponentView;

// ****************************************************************************
//
//  Class Name:	RDeactivateComponentAction
//
//  Description:	Deactivate the given component
//
// ****************************************************************************
//
class FrameworkLinkage RDeactivateComponentAction : public RTransparentUndoableAction
	{
	// Construction & Destruction
	public :
												RDeactivateComponentAction( RView* pParentView );
												RDeactivateComponentAction( RView* pParentView, RScript& script );

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
		RView*								m_pParentView;
		RComponentView*					m_pComponentView;

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

#endif	//	_DEACTIVATECOMPONENTACTION_H_
