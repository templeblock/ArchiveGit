// ****************************************************************************
//
//  File Name:			ImageApplication.cpp
//
//  Project:			Image Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RImageApplication class
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
//  $Logfile:: /PM8/ImageComp/Source/ImageApplication.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:21p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"ImageIncludes.h"

ASSERTNAME

#include	"ImageApplication.h"
#include	"ImageDocument.h"
// #include	"RenaissanceResource.h"
#include "DataTransfer.h"
#include "BitmapImage.h"
#include "VectorImage.h"
#include "ComponentTypes.h"

// Macro
CreateComponent( RImageApplication );

// ****************************************************************************
//
//  Function Name:	RImageApplication::CanPasteDataFormat( )
//
//  Description:		Checks to see if the component type created by this server
//							can paste data of the specified format
//
//  Returns:			TRUE if the data can be pasted
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageApplication::CanPasteDataFormat(YDataFormat yDataFormat, YComponentType& yComponentType) const
{
	// Return TRUE if we support a given format
	if (yDataFormat == kDibFormat || yDataFormat == kPictureFormat || yDataFormat == kEnhancedMetafileFormat)
	{				//	Must return the data so the equals operator can copy it.
					//	Otherwise, we get an assert because of a CString problem
		// Set the component type
		yComponentType = (LPCTSTR)kImageComponent;
		return TRUE;
	}
	else
		return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RImageApplication::GetSupportedComponentTypes( )
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
void RImageApplication::GetSupportedComponentTypes(RComponentTypeCollection& rComponentTypeCollection) const
{
	rComponentTypeCollection.InsertAtEnd( kImageComponent );
	rComponentTypeCollection.InsertAtEnd( kPSDPhotoComponent );
	rComponentTypeCollection.InsertAtEnd( kPhotoFrameComponent );
	//KDM 10/22/97 Leapfrog - Just use Image
	rComponentTypeCollection.InsertAtEnd( kNavigationButtonSetComponent );
	rComponentTypeCollection.InsertAtEnd( kIconButtonComponent );
	rComponentTypeCollection.InsertAtEnd( kBulletComponent );
	rComponentTypeCollection.InsertAtEnd( kPageDividerComponent );
	rComponentTypeCollection.InsertAtEnd( kAnimationComponent );
	rComponentTypeCollection.InsertAtEnd( kBackgroundComponent );
}

// ****************************************************************************
//
//  Function Name:	RImageApplication::CreateNewDocument( )
//
//  Description:		Creates a new document of the specified type
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* RImageApplication::CreateNewDocument(const RComponentAttributes& rComponentAttributes, const YComponentType& yComponentType, BOOLEAN fLoading)
{
	//	Create a special PSD Photo if requested.
	if ( yComponentType == kPSDPhotoComponent || yComponentType == kPhotoFrameComponent )
		return new RPSDPhotoDocument( this, rComponentAttributes, yComponentType, fLoading );
	else
		return new RImageDocument( this, rComponentAttributes, yComponentType, fLoading );
}

#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RImageApplication::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageApplication::Validate() const
{
	RComponentApplication::Validate();
	TpsAssertIsObject(RImageApplication, this);
}

#endif TPSDEBUG
