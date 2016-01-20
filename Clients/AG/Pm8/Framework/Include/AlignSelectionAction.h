// ****************************************************************************
//
//  File Name:			AlignSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RAlignSelectionAction class
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
//  $Logfile:: /PM8/Framework/Include/AlignSelectionAction.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_ALIGNSELECTIONACTION_H_
#define		_ALIGNSELECTIONACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class ROffsetList: public RList<RRealSize> {};

// ****************************************************************************
//
//  Class Name:	RAlignSelectionAction
//
//  Description:	Action to move a component
//
// ****************************************************************************
//
class FrameworkLinkage RAlignSelectionAction : public RUndoableAction
	{
	public :
		enum	EAlignHorizontal 
				{
					kAlignNoneHorz = 0,
					kAlignLeft,
					kAlignCenterHorz,
					kAlignRight
				};

		enum	EAlignVertical 
				{
					kAlignNoneVert = 0,
					kAlignTop,
					kAlignCenterVert,
					kAlignBottom
				};

		enum EAlignToWhat
				{
					kToEachOther = 0,
					kToParentView
				};

	// Construction & Destruction
	public :
												RAlignSelectionAction( RCompositeSelection* pCurrentSelection, const EAlignHorizontal eHAlign, const EAlignVertical eVAlign, EAlignToWhat eToWhat );
												RAlignSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );

	//	Identifier
	public :
		static	YActionId				m_ActionId;

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
		ROffsetList							m_OldOffsets;				//	list of offsets used to align components to their aligned positions
		EAlignHorizontal					m_eHorizontalAlignment;	//	horizontal alignment to apply to selected components
		EAlignVertical						m_eVerticalAlignment;	//	vertical alignment to apply to selected components
		EAlignToWhat						m_eToWhat;					//	What to align to.

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

#endif		// _ALIGNSELECTIONACTION_H_
