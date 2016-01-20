// ****************************************************************************
//
//  File Name:			SetHeadlineTextAction.h
//
//  Project:			Renaissance Headline Component
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RSetHeadlineTextAction class
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
//  $Logfile:: /PM8/HeadlineComp/Include/SetHeadlineTextAction.h              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_SETHEADLINETEXTACTION_H_
#define		_SETHEADLINETEXTACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

class RHeadlineDocument;

// ****************************************************************************
//
//  Class Name:		REditHeadlineAction
//
//  Description:	This class allows the do/undo/redo of Editing a headline.
//
// ****************************************************************************
//
class RSetHeadlineTextAction : public RUndoableAction
	{
	// Construction & destruction
	public :
													RSetHeadlineTextAction( RHeadlineDocument* pHeadlineDocument, const RMBCString& text );
													RSetHeadlineTextAction( RHeadlineDocument* pHeadlineDocument, RScript& script );
		virtual									~RSetHeadlineTextAction( );

	//	Identifier
	public :
		static	YActionId					m_ActionId;

	// Operations
	public :
		virtual	BOOLEAN						Do( );
		virtual	void							Undo( );

	// Scripting Operations
	public :
		virtual BOOLEAN						WriteScript( RScript& script ) const;

	//	Private Member Data
	private :
		RHeadlineDocument*					m_pHeadlineDocument;
		RMBCString								m_Text;

#ifdef	TPSDEBUG
	//	Debugging code
	public :
		virtual	void							Validate( ) const;
#endif
	};

#endif	//	_SETHEADLINETEXTACTION_H_
