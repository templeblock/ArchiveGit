// ****************************************************************************
//
//  File Name:			TransparentUndoableAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RTransparentUndoableAction class
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//						One Gateway Center, Suite 800
//						Newton, MA 02158
//						(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/TransparentUndoableAction.h             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_TRANSPARENTUNDOABLEACTION_H_
#define		_TRANSPARENTUNDOABLEACTION_H_

#ifndef	_UNDOABLEACTION_H_
#include	"UndoableAction.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:		RTransparentUndoableAction
//
//  Description:	This is the base class for Renaissance undoable xActions.
//
// ****************************************************************************
//
class FrameworkLinkage RTransparentUndoableAction : public RUndoableAction
	{
	// Construction & destruction
	public :
													RTransparentUndoableAction( const YActionId& actionId );
													RTransparentUndoableAction( const YActionId& actionId, RScript& script );
		virtual									~RTransparentUndoableAction( );

	// Operations
	public :
		virtual	BOOLEAN						IsTransparent( ) const;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RUndoableAction::IsTransparent( )
//
//  Description:		Determines whether or not this action is transparent or not.
//
//  Returns:			FALSE, as RUndoableActions are by definition transparent
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline BOOLEAN RTransparentUndoableAction::IsTransparent( ) const
	{
	return TRUE;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _TRANSPARENTUNDOABLEACTION_H_
