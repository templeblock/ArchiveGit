// ****************************************************************************
//
//  File Name:			ResizeSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RResizeSelectionAction class
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
//  $Logfile:: /PM8/Framework/Include/ResizeSelectionAction.h                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_RESIZESELECTIONACTION_H_
#define		_RESIZESELECTIONACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef RList<YComponentBoundingRect>			YBoundingRectCollection;
typedef YBoundingRectCollection::YIterator	YBoundingRectIterator;

// ****************************************************************************
//
//  Class Name:	RResizeSelectionAction
//
//  Description:	Action to move a component
//
// ****************************************************************************
//
class FrameworkLinkage RResizeSelectionAction : public RUndoableAction
	{
	// Construction & Destruction
	public :
												RResizeSelectionAction( RCompositeSelection* pCurrentSelection,
																				const RRealPoint& centerOfScaling,
																				const RRealSize& scaleFactor,
																				BOOLEAN fMaintainAspectRatio );

												RResizeSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );

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
		RRealPoint							m_CenterOfScaling;
		RRealSize							m_ScaleFactor;
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

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _RESIZESELECTIONACTION_H_
