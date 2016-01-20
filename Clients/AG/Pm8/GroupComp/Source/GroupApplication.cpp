// ****************************************************************************
//
//  File Name:			GroupApplication.cpp
//
//  Project:			Group Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGroupApplication class
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
//  $Logfile:: /PM8/GroupComp/Source/GroupApplication.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GroupIncludes.h"

ASSERTNAME

#include	"GroupApplication.h"
#include	"GroupDocument.h"
#include "ComponentTypes.h"

// Macro
CreateComponent( RGroupApplication );

// ****************************************************************************
//
//  Function Name:	RGroupApplication::GetSupportedComponentTypes( )
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
void RGroupApplication::GetSupportedComponentTypes( RComponentTypeCollection& componentTypeCollection ) const
	{
	componentTypeCollection.InsertAtEnd( kGroupComponent );
	componentTypeCollection.InsertAtEnd( kLogoComponent );
	componentTypeCollection.InsertAtEnd( kInitialCapComponent );
	componentTypeCollection.InsertAtEnd( kSmartNumberComponent );
	componentTypeCollection.InsertAtEnd( kSealComponent );
	componentTypeCollection.InsertAtEnd( kTimepieceComponent );
	componentTypeCollection.InsertAtEnd( kSignatureComponent );
	}

// ****************************************************************************
//
//  Function Name:	RGroupApplication::CreateNewDocument( )
//
//  Description:		Creates a new document of the specified type
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* RGroupApplication::CreateNewDocument( const RComponentAttributes& componentAttributes, const YComponentType& componentType, BOOLEAN fLoading )
	{
	return new RGroupDocument( this, componentAttributes, componentType, fLoading );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RGroupApplication::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupApplication::Validate( ) const
	{
	RComponentApplication::Validate( );
	TpsAssertIsObject( RGroupApplication, this );
	}

#endif	//	TPSDEBUG
