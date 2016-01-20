// ****************************************************************************
//
//  File Name:			PathApplication.cpp
//
//  Project:			Path Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RPathApplication class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software.
//							500 Redwood Blvd
//							Novato, CA 94948
//							(415) 382-4400
//
//  Copyright (C) 1999 Broderbund Software, Inc. All Rights Reserved.
//
//  $Logfile:: /PM8/PathComp/Source/PathApplication.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:23p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"PathIncludes.h"
ASSERTNAME

#include	"PathApplication.h"
#include	"PathDocument.h"
#include "ComponentTypes.h"

// Macro
CreateComponent( RPathApplication );

// ****************************************************************************
//
//  Function Name:	RPathApplication::GetSupportedComponentTypes( )
//
//  Description:		Fills in the specified collection with the component types
//							which this application supports
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathApplication::GetSupportedComponentTypes( RComponentTypeCollection& componentTypeCollection ) const
{
	componentTypeCollection.InsertAtEnd( kPathComponent );
}

// ****************************************************************************
//
//  Function Name:	RPathApplication::CreateNewDocument( )
//
//  Description:		Creates a new document of the specified type
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* RPathApplication::CreateNewDocument( const RComponentAttributes& componentAttributes, const YComponentType& componentType, BOOLEAN fLoading )
{
	if (componentType == kPathComponent)
	{
		return new RPathDocument( this, componentAttributes, componentType, fLoading );		
	}
	else
	{
		TpsAssertAlways("Bad component type in RPathApplication::CreateNewDocument().");
		return NULL;
	}
}

#ifdef TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPathApplication::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPathApplication::Validate( ) const
{
	RComponentApplication::Validate( );
	TpsAssertIsObject( RPathApplication, this );
}

#endif	//	TPSDEBUG
