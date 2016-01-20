// ****************************************************************************
//
//  File Name:			CopySelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RCopySelectionAction class
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
//  $Logfile:: /PM8/Framework/Include/CopySelectionAction.h                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COPYSELECTIONACTION_H_
#define		_COPYSELECTIONACTION_H_

#ifndef		_DELETESELCTIONACTION_H_
#include		"DeleteSelectionAction.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RDataTransferTarget;

// ****************************************************************************
//
//  Class Name:	RCopySelectionAction
//
//  Description:	Copys the current selection to the clipboard
//
// ****************************************************************************
//
class FrameworkLinkage RCopySelectionAction : public RAction
	{
	// Construction, destruction & Intialization
	public :
												RCopySelectionAction( RCompositeSelection* pCurrentSelection, RDataTransferTarget* pDataTransferTarget, BOOLEAN fDelayRenderComponents );
												RCopySelectionAction( RCompositeSelection* pCurrentSelection, RDataTransferTarget* pDataTransferTarget, BOOLEAN fDelayRenderComponents, RScript& script );

	//	Identifier
	public :
		static	YActionId				m_ActionId;

	// Operations
	public :
		virtual BOOLEAN					Do( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Member data
	private :
		RCompositeSelection*				m_pCurrentSelection;
		RDataTransferTarget*				m_pDataTransferTarget;
		BOOLEAN								m_fDelayRenderComponents;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RCutSelectionAction
//
//  Description:	Cuts the current selection to the clipboard
//
// ****************************************************************************
//
class FrameworkLinkage RCutSelectionAction : public RDeleteSelectionAction
	{
	// Construction, destruction & Intialization
	public :
												RCutSelectionAction( RCompositeSelection* pCurrentSelection );

	//	Identifier
	public :
		static	YActionId				m_ActionId;

	// Operations
	public :
		virtual BOOLEAN					Do( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

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

#endif		// _COPYSELECTIONACTION_H_
