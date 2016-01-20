// ****************************************************************************
//
//  File Name:			RotateSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RRotateSelectionAction class
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
//  $Logfile:: /PM8/Framework/Include/RotateSelectionAction.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_ROTATESELECTIONACTION_H_
#define		_ROTATESELECTIONACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RCompositeSelection;

// ****************************************************************************
//
//  Class Name:	RRotateSelectionAction
//
//  Description:	Action to move a component
//
// ****************************************************************************
//
class FrameworkLinkage RRotateSelectionAction : public RUndoableAction
	{
	// Construction & Destruction
	public :
												RRotateSelectionAction( RCompositeSelection* pCurrentSelection,
																				const RRealPoint& centerOfRotation,
																				YAngle angle );

												RRotateSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );

	//	Identification
	public :
		static YActionId					m_ActionId;

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
		void									DoRotate( YAngle angle ) const;

	// Member data
	private :
		RCompositeSelection*				m_pCurrentSelection;
		RCompositeSelection				m_OldSelection;
		RRealPoint							m_CenterOfRotation;
		YAngle								m_Angle;

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

#endif		// _ROTATESELECTIONACTION_H_
