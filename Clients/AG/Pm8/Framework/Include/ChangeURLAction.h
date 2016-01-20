// ****************************************************************************
//
//  File Name:			ChangeURLAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Richard Grenfell
//
//  Description:		Declaration of the RChangeURLAction class.  This class
//							provides a swap of the current URL with the one saved.
//
//  Portability:		Platform independent
//
//  Copyright (C) 1998 Brøderbund Software, Inc., All Rights Reserved.
//
//  $Logfile:: $
//   $Author:: $
//     $Date:: $
// $Revision:: $
//
// ****************************************************************************

#ifndef		_CHANGEURLACTION_H_
#define		_CHANGEURLACTION_H_

#include	"UndoableAction.h"
#include "URL.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RComponentDocument;

// ****************************************************************************
//
//  Class Name:		RChangeURLAction
//
//  Description:		Changes the URL contained within the component.
//
// ****************************************************************************
class FrameworkLinkage RChangeURLAction : public RUndoableAction
{
	// Construction & destruction
	public:
													RChangeURLAction(RComponentDocument* pComponentDocument, const RURL& rUrl, BOOLEAN bAdd );
													RChangeURLAction(RComponentDocument* pComponentDocument, RScript& script );
		virtual									~RChangeURLAction( );

	//	Identifier
	public :
		static YActionId m_ActionId;

	// Operations
	public:
		virtual BOOLEAN						Do();
		virtual void							Undo();

	// Scripting Operations
	public :
		virtual BOOLEAN						WriteScript( RScript& script ) const;

	//	Private Member Data
	private:
		RComponentDocument *					m_pComponentDoc;
		RURL										m_URL;

#ifdef TPSDEBUG
	//	Debugging code
	public:
		virtual void Validate() const;
#endif
};


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_CHANGEURLACTION_H_
