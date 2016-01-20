// ****************************************************************************
//
//  File Name:			Action.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas, M. Houle
//
//  Description:		Declaration of the RAction class
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
//  $Logfile:: /PM8/Framework/Include/Action.h                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_ACTION_H_
#define		_ACTION_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

typedef	RMBCString		YActionId;

class RScript;

// ****************************************************************************
//
//  Class Name:	RAction
//
//  Description:	This is the base class for Renaissance Actions. This class
//						assumes that all actions are scriptable. Rather than use
//						a virtual base class, we are putting undo/redo functionality
//						into this base class, and deriving RUndoableAction from it.
//						This is a bit of a hack, but is much faster than the
//						alternative
//
// ****************************************************************************
//
class FrameworkLinkage RAction : public RObject
	{
	// Construction & destruction
	public :
												RAction( const YActionId& actionId );
												RAction( const YActionId& actionId, RScript& script );
		virtual								~RAction( );

	//	Identification
	public :
		const YActionId&					GetActionId( ) const;

	// Operations
	public :
		virtual BOOLEAN					IsUndoable( ) const;

		virtual BOOLEAN					Do( ) = 0;

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const = 0;

	//	Members
	private :
		const YActionId&					m_ActionId;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const = 0;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RAction::IsUndoable( )
//
//  Description:		Determines whether or not this action is undoable or not.
//
//  Returns:			FALSE, as RActions are not undoable by default
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RAction::IsUndoable( ) const
	{
	return FALSE;
	}


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _ACTION_H_
