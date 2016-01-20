// ****************************************************************************
//
//  File Name:			ReplaceComponentAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RReplaceComponentAction class
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
//  $Logfile:: /PM8/Framework/Include/ReplaceComponentAction.h                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_REPLACECOMPONENTACTION_H_
#define		_REPLACECOMPONENTACTION_H_

#include	"UndoableAction.h"
#include "View.h"
#include "CompositeSelection.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	RReplaceComponentAction
//
//  Description:	Action which replaces a component with another component
//
// ****************************************************************************
//
class FrameworkLinkage RReplaceComponentAction : public RUndoableAction
	{
	// Construction, destruction & Intialization
	public :
												RReplaceComponentAction( RCompositeSelection* pCurrentSelection,
																				 RComponentDocument* pOldComponent,
																				 RComponentDocument* pNewComponent,
																				 YResourceId undoRedoPairId,
																				 BOOLEAN fModifyBoundingRect = TRUE,
																				 BOOLEAN fCopyViewAttributes = TRUE );

		virtual								~RReplaceComponentAction( );

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void						Undo( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	//	Identifier
	public :
		static YActionId					m_ActionId;

	// Member data
	protected :
		RCompositeSelection*				m_pCurrentSelection;
		RComponentDocument*				m_pOldComponent;
		RComponentDocument*				m_pNewComponent;
		YComponentZPosition				m_ComponentZPosition;
		BOOLEAN								m_fAdjustSelection;
		BOOLEAN								m_fModifyBoundingRect;
		BOOLEAN								m_fCopyViewAttributes;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _REPLACECOMPONENTACTION_H_
