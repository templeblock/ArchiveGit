// ****************************************************************************
//
//  File Name:			Action.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RAction class
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
//  $Logfile:: /PM8/Framework/Source/Action.cpp                               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"Action.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RAction::RAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction::RAction( const YActionId& actionId )
	: m_ActionId( actionId ) 
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RAction::RAction( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction::RAction( const YActionId& actionId, RScript& /* script */ ) 
	: m_ActionId( actionId )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RAction::~RAction( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction::~RAction( )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RDeleteTextSelectionAction::GetActionId( )
//
//  Description:		Return the identifier for this action
//
//  Returns:			Static pointer to action identifier
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YActionId& RAction::GetActionId( ) const
	{
	return m_ActionId;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RAction::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RAction::Validate( ) const
	{
	RObject::Validate();
	}

#endif	//	TPSDEBUG
