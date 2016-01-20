// ****************************************************************************
//
//  File Name:			ComponentVignetteSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of the RComponentVignetteSelectionAction class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef		_COMPONENTVIGNETTESELECTIONACTION_H_
#define		_COMPONENTVIGNETTESELECTIONACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifndef		_SOFTVIGNETTESETTINGS_H_
#include		"SoftVignetteSettings.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef	RList<RSoftVignetteSettings>		YVignetteStateList;
typedef	YVignetteStateList::YIterator		YVignetteStateListIterator;

// ****************************************************************************
//
//  Class Name:	RComponentVignetteSelectionAction
//
//  Description:	Action to vignette a component
//
// ****************************************************************************
//
class FrameworkLinkage RComponentVignetteSelectionAction : public RUndoableAction
	{
	// Construction & Destruction
	public :
												RComponentVignetteSelectionAction( RCompositeSelection* pCurrentSelection, const RSoftVignetteSettings& fVignetteSettings );
												RComponentVignetteSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );

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
		RSoftVignetteSettings			m_fNewVignetteSettings;
		YVignetteStateList				m_yOldVignetteStates;

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

#endif		// _COMPONENTVIGNETTESELECTIONACTION_H_
