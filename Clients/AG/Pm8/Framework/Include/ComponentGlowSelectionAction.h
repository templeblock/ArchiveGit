// ****************************************************************************
//
//  File Name:			ComponentGlowSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of the RComponentGlowSelectionAction class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef		_COMPONENTGLOWSELECTIONACTION_H_
#define		_COMPONENTGLOWSELECTIONACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifndef		_SOFTGLOWSETTINGS_H_
#include		"SoftGlowSettings.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef	RList<RSoftGlowSettings>			YGlowStateList;
typedef	YGlowStateList::YIterator			YGlowStateListIterator;

// ****************************************************************************
//
//  Class Name:	RComponentGlowSelectionAction
//
//  Description:	Action to glow a component
//
// ****************************************************************************
//
class FrameworkLinkage RComponentGlowSelectionAction : public RUndoableAction
	{
	// Construction & Destruction
	public :
												RComponentGlowSelectionAction( RCompositeSelection* pCurrentSelection, const RSoftGlowSettings& fGlowSettings );
												RComponentGlowSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );

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
		RSoftGlowSettings					m_fNewGlowSettings;
		YGlowStateList						m_yOldGlowStates;

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

#endif		// _COMPONENTGLOWSELECTIONACTION_H_
