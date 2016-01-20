// ****************************************************************************
//
//  File Name:			GroupDocument.cpp
//
//  Project:			Group Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGroupDocument class
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
//  $Logfile:: /PM8/GroupComp/Source/GroupDocument.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GroupIncludes.h"

ASSERTNAME

#include	"GroupDocument.h"
#include	"GroupView.h"

// ****************************************************************************
//
//  Function Name:	RGroupDocument::RGroupDocument( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGroupDocument::RGroupDocument( RApplication* pApp,
										  const RComponentAttributes& componentAttributes,
										  const YComponentType& componentType,
										  BOOLEAN fLoading )
	: RComponentDocument( pApp, componentAttributes, componentType, fLoading )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RGroupDocument::CreateView( )
//
//  Description:		Creates a new view on this document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView* RGroupDocument::CreateView( const YComponentBoundingRect& boundingRect, RView* pParentView )
	{
	return new RGroupView( boundingRect, this, pParentView );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RGroupDocument::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupDocument::Validate( ) const
	{
	RComponentDocument::Validate( );
	TpsAssertIsObject( RGroupDocument, this );
	}
									
#endif	//	TPSDEBUG
