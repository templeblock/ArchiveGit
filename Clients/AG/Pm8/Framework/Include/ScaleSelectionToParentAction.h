// ****************************************************************************
//
//  File Name:			ScaleSelectionToParentAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RScaleSelectionToParentAction class
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
//  $Logfile:: /PM8/Framework/Include/ScaleSelectionToParentAction.h          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SCALESELECTIONTOPARENTACTION_H_
#define		_SCALESELECTIONTOPARENTACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

typedef RList<YComponentBoundingRect>			YBoundingRectCollection;
typedef YBoundingRectCollection::YIterator	YBoundingRectIterator;

// ****************************************************************************
//
//  Class Name:	ScaleSelectionToParentAction
//
//  Description:	Action to move a component
//
// ****************************************************************************
//
class FrameworkLinkage RScaleSelectionToParentAction : public RUndoableAction
	{
	// Construction & Destruction
	public :
												RScaleSelectionToParentAction( RCompositeSelection* pCurrentSelection,
																				BOOLEAN fMaintainAspectRatio );

												RScaleSelectionToParentAction( RCompositeSelection* pCurrentSelection, RScript& script );

	//	Identifier
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

	// Member data
	private :
		RCompositeSelection*				m_pCurrentSelection;
		RCompositeSelection				m_OldSelection;
		YBoundingRectCollection			m_BoundingRectCollection;
		BOOLEAN								m_fMaintainAspectRatio;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#endif		// _SCALESELECTIONTOPARENTACTION_H_
