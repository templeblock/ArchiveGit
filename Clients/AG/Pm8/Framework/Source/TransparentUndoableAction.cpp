// ****************************************************************************
//
//  File Name:			TransparentUndoableAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RTransparentUndoableAction class
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
//  $Logfile:: /PM8/Framework/Source/TransparentUndoableAction.cpp            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"TransparentUndoableAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RTransparentUndoableAction::RTransparentUndoableAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RTransparentUndoableAction::RTransparentUndoableAction( const YActionId& actionId )
	: RUndoableAction( actionId, 0, 0 )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RTransparentUndoableAction::RTransparentUndoableAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RTransparentUndoableAction::RTransparentUndoableAction( const YActionId& actionId, RScript& script )
	: RUndoableAction( actionId, script )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RTransparentUndoableAction::~RTransparentUndoableAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RTransparentUndoableAction::~RTransparentUndoableAction( )
	{
	;
	}


