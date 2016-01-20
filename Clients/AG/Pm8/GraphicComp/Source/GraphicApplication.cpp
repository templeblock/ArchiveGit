// ****************************************************************************
//
//  File Name:			GraphicApplication.cpp
//
//  Project:			Graphic Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGraphicApplication class
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
//  $Logfile:: /PM8/GraphicComp/Source/GraphicApplication.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GraphicIncludes.h"

ASSERTNAME

#include	"GraphicApplication.h"
#include	"GraphicDocument.h"
#include	"GraphicView.h"
#include "ComponentTypes.h"

// Macro
CreateComponent( RGraphicApplication );

// ****************************************************************************
//
//  Function Name:	RGraphicApplication::GetSupportedComponentTypes( )
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
void RGraphicApplication::GetSupportedComponentTypes( RComponentTypeCollection& componentTypeCollection ) const
{
	componentTypeCollection.InsertAtEnd( kSquareGraphicComponent );
	componentTypeCollection.InsertAtEnd( kRowGraphicComponent );
	componentTypeCollection.InsertAtEnd( kColumnGraphicComponent );
	componentTypeCollection.InsertAtEnd( kBackdropGraphicComponent );
	componentTypeCollection.InsertAtEnd( kHorizontalLineComponent );
	componentTypeCollection.InsertAtEnd( kVerticalLineComponent );
	componentTypeCollection.InsertAtEnd( kBorderComponent );
	componentTypeCollection.InsertAtEnd( kMiniBorderComponent );
	componentTypeCollection.InsertAtEnd( kCGMGraphicComponent );
}

// ****************************************************************************
//
//  Function Name:	RGraphicApplication::CanPasteDataFormat( )
//
//  Description:		Returns TRUE if pasting of the given data format is supported
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RGraphicApplication::CanPasteDataFormat( YDataFormat dataFormat, YComponentType& componentType ) const
{
	if (dataFormat == kPsd3SquareGraphicFormat || dataFormat == kPsd5SquareGraphicFormat)
	{				//	Must return the data so the equals operator can copy it.
					//	Otherwise, we get an assert because of a CString problem
		componentType = (LPCTSTR)kSquareGraphicComponent;
		return TRUE;
	}
	else if (dataFormat == kPsd3ColumnGraphicFormat || dataFormat == kPsd5ColumnGraphicFormat)
	{
		componentType = (LPCTSTR)kColumnGraphicComponent;
		return TRUE;
	}
	else if (dataFormat == kPsd3RowGraphicFormat || dataFormat == kPsd5RowGraphicFormat)
	{
		componentType = (LPCTSTR)kRowGraphicComponent;
		return TRUE;
	}	
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RGraphicApplication::CreateNewDocument( )
//
//  Description:		Creates a new document of the specified type
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* RGraphicApplication::CreateNewDocument( const RComponentAttributes& componentAttributes, const YComponentType& componentType, BOOLEAN fLoading )
{
	if (	componentType == kSquareGraphicComponent || 
			componentType == kRowGraphicComponent || 
			componentType == kColumnGraphicComponent || 
			componentType == kTimepieceComponent || 
			componentType == kBackdropGraphicComponent ||
			componentType == kHorizontalLineComponent || 
			componentType == kVerticalLineComponent ||
			componentType == kBorderComponent || 
			componentType == kMiniBorderComponent ||
			componentType == kCGMGraphicComponent)
	{
		return new RGraphicDocument(this, componentAttributes, componentType, fLoading);		
	}
	else
	{
		TpsAssertAlways("Bad component type in RGraphicApplication::CreateNewDocument().");
		return NULL;
	}
}

#ifdef TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RGraphicApplication::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGraphicApplication::Validate( ) const
{
	RComponentApplication::Validate( );
	TpsAssertIsObject( RGraphicApplication, this );
}

#endif	//	TPSDEBUG
