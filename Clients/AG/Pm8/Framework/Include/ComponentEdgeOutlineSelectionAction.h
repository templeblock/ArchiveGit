// ****************************************************************************
//
//  File Name:			ComponentEdgeOutlineSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Wynn
//
//  Description:		Declaration of the RComponentEdgeOutlineSelectionAction class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef		_COMPONENTEDGEOUTLINESELECTIONACTION_H_
#define		_COMPONENTEDGEOUTLINESELECTIONACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifndef		_EDGEOUTLINESETTINGS_H_
#include		"EdgeOutlineSettings.h"
#endif

typedef	RList<REdgeOutlineSettings>		YEdgeOutlineStateList;
typedef	YEdgeOutlineStateList::YIterator		YEdgeOutlineStateListIterator;

// ****************************************************************************
//
//  Class Name:	RComponentEdgeOutlineSelectionAction
//
//  Description:	Action to vignette a component
//
// ****************************************************************************
//
class FrameworkLinkage RComponentEdgeOutlineSelectionAction : public RUndoableAction
	{
	// Construction & Destruction
	public :
												RComponentEdgeOutlineSelectionAction( RCompositeSelection* pCurrentSelection, const REdgeOutlineSettings& edgeOutlineSettings );
												RComponentEdgeOutlineSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );

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
		REdgeOutlineSettings				m_newEdgeOutlineSettings;
		YEdgeOutlineStateList			m_oldEdgeOutlineStates;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#endif		// _COMPONENTVIGNETTESELECTIONACTION_H_
