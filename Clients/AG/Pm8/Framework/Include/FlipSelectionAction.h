// ****************************************************************************
//
//  File Name:			FlipSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RFlipSelectionAction class
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
//  $Logfile:: /PM8/Framework/Include/FlipSelectionAction.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FLIPSELECTIONACTION_H_
#define		_FLIPSELECTIONACTION_H_

#include		"UndoableAction.h"
#include		"CompositeSelection.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RComponentView;

// ****************************************************************************
//
//  Class Name:	RFlipSelectionAction
//
//  Description:	Base class for actions which flip components
//
// ****************************************************************************
//
class FrameworkLinkage RFlipSelectionAction : public RUndoableAction
	{
	// Construction & Destruction
	public :
												RFlipSelectionAction( RCompositeSelection* pCurrentSelection,
																			 const YActionId& actionId,
																			 uWORD uwUndoId,
																			 uWORD uwRedoId );

												RFlipSelectionAction( RCompositeSelection* pCurrentSelection,
																			 RScript& script,
																			 const YActionId& actionId );

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );
		virtual void						Redo( );
		virtual void						ApplyFlip( RComponentView* pComponentView ) const = 0;


	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Member data
	protected :
		RRealPoint							m_SelectionCenterPoint;

	// Member data
	private :
		RCompositeSelection*				m_pCurrentSelection;
		RCompositeSelection				m_OldSelection;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RFlipSelectionHorizontalAction
//
//  Description:	Action which flips a component about a vertical line
//
// ****************************************************************************
//
class FrameworkLinkage RFlipSelectionHorizontalAction : public RFlipSelectionAction
	{
	// Construction & Destruction
	public :
												RFlipSelectionHorizontalAction( RCompositeSelection* pCurrentSelection );
												RFlipSelectionHorizontalAction( RCompositeSelection* pCurrentSelection, RScript& script );

	// Operations
	public :
		virtual void						ApplyFlip( RComponentView* pComponentView ) const;

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

// ****************************************************************************
//
//  Class Name:	RFlipSelectionVerticalAction
//
//  Description:	Action which flips a component about a horizontal line
//
// ****************************************************************************
//
class FrameworkLinkage RFlipSelectionVerticalAction : public RFlipSelectionAction
	{
	// Construction & Destruction
	public :
												RFlipSelectionVerticalAction( RCompositeSelection* pCurrentSelection );
												RFlipSelectionVerticalAction( RCompositeSelection* pCurrentSelection, RScript& script );

	// Operations
	public :
		virtual void						ApplyFlip( RComponentView* pComponentView ) const;

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

// ****************************************************************************
//
//  Class Name:	RFlipSelectionBothAction
//
//  Description:	Action which flips a component about both axis
//
// ****************************************************************************
//
class FrameworkLinkage RFlipSelectionBothAction : public RFlipSelectionAction
	{
	// Construction & Destruction
	public :
												RFlipSelectionBothAction( RCompositeSelection* pCurrentSelection );
												RFlipSelectionBothAction( RCompositeSelection* pCurrentSelection, RScript& script );

	// Operations
	public :
		virtual void						ApplyFlip( RComponentView* pComponentView ) const;

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _FLIPSELECTIONACTION_H_
