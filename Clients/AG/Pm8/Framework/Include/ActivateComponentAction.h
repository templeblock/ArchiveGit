// ****************************************************************************
//
//  File Name:			ActivateComponentAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RActivateComponentAction class
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
//  $Logfile:: /PM8/Framework/Include/ActivateComponentAction.h               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_ACTIVATECOMPONENTACTION_H_
#define		_ACTIVATECOMPONENTACTION_H_

#ifndef		_TRANSPARENTUNDOABLEACTION_H_
#include		"TransparentUndoableAction.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RView;
class RComponentView;
class RCompositeSelection;

// ****************************************************************************
//
//  Class Name:	RActivateComponentAction
//
//  Description:	Activate the given component
//
// ****************************************************************************
//
class FrameworkLinkage RActivateComponentAction : public RTransparentUndoableAction
	{
	// Construction & Destruction
	public :
												RActivateComponentAction( RView* pParentView, RComponentView* pComponentView );
												RActivateComponentAction( RView* pParentView, RScript& script );

	//	Identifier
	public :
		static	YActionId				m_ActionId;

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Member data
	private :
		RView*								m_pParentView;
		RComponentView*					m_pComponentView;
		RCompositeSelection*				m_pCurrentSelection;

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

#endif	//	_ACTIVATECOMPONENTACTION_H_
