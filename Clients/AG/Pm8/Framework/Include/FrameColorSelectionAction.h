// ****************************************************************************
//
//  File Name:			FrameColorSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Eric VanHelene
//
//  Description:		Declaration of the RFrameColorSelectionAction class
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
//  $Logfile:: /PM8/Framework/Include/FrameColorSelectionAction.h                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FRAMECOLORSELECTIONACTION_H_
#define		_FRAMECOLORSELECTIONACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef	RList<RSolidColor>				YFrameColorList;
typedef	YFrameColorList::YIterator		YFrameColorListIterator;

// ****************************************************************************
//
//  Class Name:	RFrameColorSelectionAction
//
//  Description:	Action to move a component
//
// ****************************************************************************
//
class FrameworkLinkage RFrameColorSelectionAction : public RUndoableAction
	{
	// Construction & Destruction
	public :
												RFrameColorSelectionAction( RCompositeSelection* pCurrentSelection, const RSolidColor rColor );
												RFrameColorSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );
		virtual void						Redo( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Member data

	//	Identifier
	public :
		static	YActionId				m_ActionId;

	private :
		RCompositeSelection*				m_pCurrentSelection;
		RCompositeSelection				m_OldSelection;
		RSolidColor							m_rNewFrameColor;
		YFrameColorList					m_yOldFrameColors;

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

#endif		// _FRAMECOLORSELECTIONACTION_H_
