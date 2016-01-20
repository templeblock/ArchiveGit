// ****************************************************************************
//
//  File Name:			HeadlineApplication.cpp
//
//  Project:			Headline Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RHeadlineApplication class
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
//  $Logfile:: /PM8/HeadlineComp/Source/HeadlineApplication.cpp               $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"HeadlineIncludes.h"

ASSERTNAME

#include	"HeadlineApplication.h"
#include	"HeadlineDocument.h"
//#include	"HeadlineView.h"
//#include	"RenaissanceResource.h"
#include "ComponentTypes.h"

CreateComponent( RHeadlineApplication );

// ****************************************************************************
//
//  Function Name:	RHeadlineApplication::GetSupportedComponentTypes( )
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
void RHeadlineApplication::GetSupportedComponentTypes( RComponentTypeCollection& componentTypeCollection ) const
	{
	componentTypeCollection.InsertAtEnd( kHeadlineComponent );
	componentTypeCollection.InsertAtEnd( kSpecialHeadlineComponent );
	componentTypeCollection.InsertAtEnd( kVerticalHeadlineComponent );
	componentTypeCollection.InsertAtEnd( kVerticalSpecialHeadlineComponent );
	}

// ****************************************************************************
//
//  Function Name:	RHeadlineApplication::CreateNewDocument( )
//
//  Description:		Creates a new document of the specified type
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* RHeadlineApplication::CreateNewDocument( const RComponentAttributes& componentAttributes, const YComponentType& componentType, BOOLEAN fLoading )
	{
	RComponentAttributes newComponentAttributes( componentAttributes );
#if 0 // GCB 3/9/98 - PS6 headlines are now shadowable
	newComponentAttributes.SetShadowable( FALSE );
#endif

	return new RHeadlineDocument( this, newComponentAttributes, componentType, fLoading );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RHeadlineApplication::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RHeadlineApplication::Validate( ) const
	{
	RComponentApplication::Validate( );
	TpsAssertIsObject( RHeadlineApplication, this );
	}

#endif	//	TPSDEBUG
