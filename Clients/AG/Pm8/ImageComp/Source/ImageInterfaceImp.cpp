//****************************************************************************
//
// File Name:		ImageInterfaceImp.cpp
//
// Project:			Renaissance Image Component
//
// Author:			Mike Heydlauf
//
// Description:	Implementation of the image interface.
//
// Portability:	Platform Independant
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/ImageComp/Source/ImageInterfaceImp.cpp                   $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:21p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "ImageIncludes.h"

ASSERTNAME

#include "ImageInterfaceImp.h"
#include "ImageDocument.h"
#include "ImageView.h"
#include "Path.h"

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::RImageInterfaceImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RImageInterfaceImp::RImageInterfaceImp(const RImageView* pImageView)
:	RImageInterface(pImageView ),
	m_pImageDocument(const_cast<RImageDocument*>(pImageView->GetImageDocument()))
{
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RImageInterfaceImp::CreateInterface(const RComponentView* pView)
{
	TpsAssertIsObject(RImageView, pView);
	return new RImageInterfaceImp(static_cast<const RImageView*>(pView));
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::GetImage( )
//
//  Description:		Gets the image of a component
//
//  Returns:			the components image
//
//  Exceptions:		None
//
// ****************************************************************************
//
RImage* RImageInterfaceImp::CopySourceImage()
{
	RImageLibrary rLibrary;
	RImage* pImage = rLibrary.ImportImage( m_pImageDocument->m_rInternalDataBuffer );

	RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>( pImage );

	if (pBitmap)
	{
		REditImageInterfaceImp rInterface( (RImageView *)
			m_pImageDocument->GetActiveView() );

		RClippingPath rPath;
		rInterface.GetClippingPath( rPath );
		pBitmap->SetRPath( &rPath );
	}

	return pImage;
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::GetImage( )
//
//  Description:		Gets the image of a component
//
//  Returns:			the components image
//
//  Exceptions:		None
//
// ****************************************************************************
//
RImage* RImageInterfaceImp::GetImage()
{
	return m_pImageDocument->GetImage();
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::SetImage( )
//
//  Description:		Sets the image of a component
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
void RImageInterfaceImp::SetImage(const RImage& rImage, BOOLEAN fResize, BOOLEAN fRetainPathname)
{
	m_pImageDocument->SetImage(rImage, fResize, fRetainPathname);
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::SetImage( )
//
//  Description:		Sets the image of a component
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
void RImageInterfaceImp::SetImage(IBufferInterface* pImageData, BOOLEAN fResize, BOOLEAN fLinkData)
{
	m_pImageDocument->SetImage(pImageData, fResize, fLinkData);
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::Load( )
//
//  Description:		Read in the data into the associated Image.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
BOOLEAN RImageInterfaceImp::Load(RStorage& rStorage, uLONG uLength, YException& hException, RMBCString* partialPath)
{
	return m_pImageDocument->Load(rStorage, uLength, hException, partialPath);
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::Export( )
//
//  Description:		Export the data into the specified format.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
BOOLEAN RImageInterfaceImp::Export( const RMBCString& rPath, EImageFormat eFormat ) 
{
	return m_pImageDocument->Export( rPath, eFormat );
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::Load( )
//
//  Description:		Read in the data into the associated Image.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
BOOLEAN RImageInterfaceImp::Load(const RMBCString& rPath, YException& hException)
{
	return m_pImageDocument->Load(rPath, hException);
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::Load( )
//
//  Description:		Read in the data into the associated Image for preview only.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory, Disk Errors
//
// ****************************************************************************
//
BOOLEAN RImageInterfaceImp::LoadForPreview(const RMBCString& rPath, const RIntSize& rMaxSize, YException& hException)
{
	return m_pImageDocument->LoadForPreview(rPath, rMaxSize, hException);
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::IsPreview( )
//
//  Description:		Returns TRUE if this component was initialized via the
//							LoadPreview() function rather than the Load() function.
//
//  Returns:			Nothing
//
//  Exceptions:		
//
// ****************************************************************************
//
BOOLEAN RImageInterfaceImp::IsPreview() const
{
	return m_pImageDocument->IsPreview();
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::GetPath( )
//
//  Description:		Returns the path of the image, if loaded from a file
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
const RMBCString&	RImageInterfaceImp::GetPath() const
{
	return m_pImageDocument->GetPath();
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::QueryEditImage( )
//
//  Description:		Returns TRUE if effects can be applied to the image
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
BOOLEAN RImageInterfaceImp::QueryEditImage()
{
	return m_pImageDocument->QueryEditImage();
}

// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::MakeImageEditable( )
//
//  Description:		Converts the image into a format which supports applying effects
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
BOOLEAN RImageInterfaceImp::MakeImageEditable()
{
	return m_pImageDocument->MakeImageEditable();
}



// ****************************************************************************
//
//  Function Name:	RImageInterfaceImp::QueryIsWebImageFormat()
//
//  Description:		Find out the size of the raw image.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageInterfaceImp::QueryRawImageSize( RIntSize *pSize )
{
	m_pImageDocument->QueryRawImageSize( pSize );
}


// ****************************************************************************
//
//  Function Name:	REditImageInterfaceImp::REditImageInterfaceImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
REditImageInterfaceImp::REditImageInterfaceImp(const RImageView* pImageView)
:	REditImageInterface(pImageView),
	m_pImageDocument(const_cast<RImageDocument*>(pImageView->GetImageDocument()))
{
}

// ****************************************************************************
//
//  Function Name:	REditImageInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* REditImageInterfaceImp::CreateInterface(const RComponentView* pView)
{
	TpsAssertIsObject(RImageView, pView);
	return new REditImageInterfaceImp(static_cast<const RImageView*>(pView));
}

// ****************************************************************************
//
//  Function Name:	REditImageInterfaceImp::EnableEditResolution() 
//
//  Description:		Prepares the component for editing
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
void REditImageInterfaceImp::EnableEditResolution(const RIntSize& rMaxEditSize)
{
	m_pImageDocument->EnableEditResolution(rMaxEditSize);
}

// ****************************************************************************
//
//  Function Name:	REditImageInterfaceImp::DisableEditResolution() 
//
//  Description:		Releases the component from edit mode
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
void REditImageInterfaceImp::DisableEditResolution()
{
	m_pImageDocument->DisableEditResolution();
}

// ****************************************************************************
//
//  Function Name:	REditImageInterfaceImp::GetImageEffects() 
//
//  Description:		Gets the image Effects
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
const RImageEffects& REditImageInterfaceImp::GetImageEffects()
{	
	return m_pImageDocument->GetImageEffects();
}

// ****************************************************************************
//
//  Function Name:	REditImageInterfaceImp::SetImageEffects( )
//
//  Description:		Sets the image Effects
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
void REditImageInterfaceImp::SetImageEffects(const RImageEffects& rEffects)
{
	m_pImageDocument->SetImageEffects(rEffects);
}

// ****************************************************************************
//
//  Function Name:	REditImageInterfaceImp::ResetImageEffects( )
//
//  Description:		Clears the image attributes
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
void REditImageInterfaceImp::ResetImageEffects()
{
	m_pImageDocument->ResetImageEffects();
}

// ****************************************************************************
//
//  Function Name:	REditImageInterfaceImp::ApplyImageEffects( )
//
//  Description:		Applies the image Effects
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
void REditImageInterfaceImp::ApplyImageEffects()
{
	// Get the size of the old image and view
	RRealSize rOldViewSize = m_pImageDocument->GetActiveView()->GetSize();
	RRealSize rOldImageSize = m_pImageDocument->GetImage()->GetSizeInLogicalUnits();		

#if 0
	if (dynamic_cast<RBitmapImage*>(m_pImageDocument->GetImage()))
	{
		const RImageEffects& rEffects = m_pImageDocument->GetImageEffects();
		rOldImageSize.m_dx /= rEffects.m_rCropArea.m_Right;
		rOldImageSize.m_dy /= rEffects.m_rCropArea.m_Bottom;
	}
#endif

	// Apply the attributes
	m_pImageDocument->ApplyImageEffects();	
	
	// Determine the size of the new image and view
	RRealSize rNewViewSize;
	RRealSize rNewImageSize = m_pImageDocument->GetImage()->GetSizeInLogicalUnits();
	rNewViewSize.m_dx = rOldViewSize.m_dx * (rNewImageSize.m_dx / rOldImageSize.m_dx);
	rNewViewSize.m_dy = rOldViewSize.m_dy * (rNewImageSize.m_dy / rOldImageSize.m_dy);

	// Resize the view
	m_pImageDocument->ResizeView(rNewViewSize, FALSE);
}

// ****************************************************************************
//
//  Function Name:	REditImageInterfaceImp::SetClippingPath( )
//
//  Description:		Sets the clipping path into the components
//                   image.
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
void REditImageInterfaceImp::SetClippingPath( const RClippingPath* path )
{
	// Get a pointer to the image document's image.  Note,
	// we should be able to assume that it is a RBitmapImage,
	// as this interface is only provided by image components
	// with RBitmapImages.
	RBitmapImage* pImage = dynamic_cast<RBitmapImage*>(
		m_pImageDocument->GetImage() );
	TpsAssert( pImage, "Invalid image component!" );

	if (path)
	{
		RClippingPath rPath( *path );
		m_pImageDocument->SetCropRectFromPath( &rPath, TRUE );
		pImage->SetRPath( &rPath );
	}
	else if (!pImage->PathFromImport())
	{
		m_pImageDocument->SetCropRectFromPath( NULL, FALSE );
		pImage->DeleteRPath();
	}

	else
		return;

	ApplyImageEffects();
}

// ****************************************************************************
//
//  Function Name:	REditImageInterfaceImp::GetClippingPath( )
//
//  Description:		Returns the component image's clipping path
//                   (if any).
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
BOOLEAN REditImageInterfaceImp::GetClippingPath( RClippingPath& rPath )
{
	// Get a pointer to the image document's image.  Note,
	// normally we can assume that it is a RBitmapImage
	// as this interface is only provided by image components,
	// but if this is a placeholder from a layout there is no
	// RBitmapImage so just return FALSE in that case.
	RBitmapImage* pImage = dynamic_cast<RBitmapImage*>(
		m_pImageDocument->GetImage() );
	if (!pImage)
		return FALSE;

	rPath.Undefine();
	RClippingPath* pPath = pImage->GetClippingRPath();

	if (pPath)
	{
		const RImageEffects& rEffects = GetImageEffects();
		RRealSize rSize = RImageLibrary().GetImageDimensions(
			m_pImageDocument->m_rInternalDataBuffer );

		R2dTransform xform;
		xform.PostScale( rSize.m_dx, rSize.m_dy );
		RRealRect rRect( rEffects.m_rCropArea * xform );

		xform.MakeIdentity();
		xform.PostTranslate( rRect.m_Left, rSize.m_dy - rRect.m_Bottom - rRect.m_Top );
		rPath = RClippingPath( *pPath, xform );

		return TRUE;
	}

	return FALSE;
}


// ****************************************************************************
//
//  Function Name:	RImageTypeInterfaceImp::RImageTypeInterfaceImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RImageTypeInterfaceImp::RImageTypeInterfaceImp(const RImageView* pImageView)
:	RImageTypeInterface(pImageView),
	m_pImageDocument(const_cast<RImageDocument*>(pImageView->GetImageDocument()))
{
}


// ****************************************************************************
//
//  Function Name:	RImageTypeInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RImageTypeInterfaceImp::CreateInterface(const RComponentView* pView)
{
	TpsAssertIsObject(RImageView, pView);
	return new RImageTypeInterfaceImp(static_cast<const RImageView*>(pView));
}



// ****************************************************************************
//
//  Function Name:	RImageTypeInterfaceImp::QueryImageIsAnAnimatedGIF()
//
//  Description:		Determines if the image is an animated GIF image.
//
//  Returns:			TRUE if an animated GIF.  FALSE otherwise.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageTypeInterfaceImp::QueryImageIsAnAnimatedGIF()
{
	return m_pImageDocument->QueryImageIsAnAnimatedGIF();
}



// ****************************************************************************
//
//  Function Name:	RImageTypeInterfaceImp::CreateGIFAnimator()
//
//  Description:		Provides an allocated GIF animator for use by the caller.
//
//  Returns:			A pointer to the GIF animator created.
//
//  Exceptions:		None
//
// ****************************************************************************
//
CPGIFAnimator *RImageTypeInterfaceImp::CreateGIFAnimator()
{
	return m_pImageDocument->CreateGIFAnimator();
}


// ****************************************************************************
//
//  Function Name:	RImageTypeInterfaceImp::QueryIsWebImageFormat()
//
//  Description:		Checks to see if the raw image data is in a Web format.
//
//  Returns:			TRUE if the raw data is in JPEG or GIF format. FALSE otherwise.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageTypeInterfaceImp::QueryIsWebImageFormat()
{
	return m_pImageDocument->QueryIsWebImageFormat();
}


// ****************************************************************************
//
//  Function Name:	RImageTypeInterfaceImp::QueryImageFormat()
//
//  Description:		Provides the format type of the raw image data.
//
//  Returns:			The format type of the raw embedded data.
//
//  Exceptions:		None
//
// ****************************************************************************
//
EImageFormat RImageTypeInterfaceImp::QueryImageFormat()
{
	return m_pImageDocument->QueryImageFormat();
}


//////////////////////////////////////////////////////////////////////////////////////
// RImagePropertiesImp
//////////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RImagePropertiesImp::RImagePropertiesImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RImagePropertiesImp::RImagePropertiesImp( const RComponentView* pView )
	: IObjectProperties( pView )
	, m_pDocument( (RImageDocument *) pView->GetComponentDocument() )
{
}

// ****************************************************************************
//
//  Function Name:	RPathInterfaceImp::CreateInterface( )
//
//  Description:		Constructor function.
//
//  Returns:			new RInterface
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
RInterface* RImagePropertiesImp::CreateInterface( const RComponentView* pView )
{
	TpsAssertIsObject( RImageView, pView );
	return new RImagePropertiesImp( static_cast<const RComponentView*>( pView ) );
}

BOOLEAN RImagePropertiesImp::FindProperty( YInterfaceId iid, RInterface** ppInterface )
{
	switch (iid)
	{
	case kFillColorAttribInterfaceId:
		*ppInterface = new RImageFillColorImp( m_pDocument->m_rTintColor, m_pDocument );
		break;

	default:
		*ppInterface = NULL;
	}

	return (*ppInterface != NULL);
}


//////////////////////////////////////////////////////////////////////////////////////
// RImageFillColorImp
//////////////////////////////////////////////////////////////////////////////////////

// ****************************************************************************
//
//  Function Name:	RImageFillColorImp::RImageFillColorImp( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RImageFillColorImp::RImageFillColorImp( RColor& crColor, RComponentDocument* pDocument )
	: RColorImp( crColor, pDocument )
{
}

// ****************************************************************************
//
//  Function Name:	RImageFillColorImp::IsColorTypeSupported( )
//
//  Description:		Determines if the specifed fill method is supported.
//
//  Returns:			TRUE if the method is supported; otherwise FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageFillColorImp::IsColorTypeSupported( RColor::EFillMethod eMethod )
{
	if (RColor::kSolid == eMethod)
		return TRUE;

	return FALSE;
}
