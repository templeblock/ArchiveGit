// *****************************************************************************
//
// File name:			MacImageLibrary.cpp
//
// Project:				Image Component
//
// Author:				G. Brown
//
// Description:		Mac-specific definition of the RImageLibrary class members
//
// Portability:		Mac only
//
// Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
// Client:				Broderbund Software, Inc.
//
// Copyright (c) 1995-1996 Turning Point Software.  All rights reserved.
//
//  $Logfile:: /PM8/Framework/Source/MacImageLibrary.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#ifdef MAC

#include "ImageLibrary.h"
#include "BitmapImage.h"
#include "VectorImage.h"
#include "OffscreenDrawingSurface.h"
#include "PolyPolygon.h"
#include "Buffer.h"
//#include "MemoryMappedBuffer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RImageLibrary::RImageLibrary()
//
//  Description:		Constructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImageLibrary::RImageLibrary()
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::~RImageLibrary()
//
//  Description:		Destructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImageLibrary::~RImageLibrary()
{
	NULL;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::GetMaxBitmapIndex()
//
//  Description:		Returns the highest index of the Bitmaps contained in
//							the file
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YImageIndex RImageLibrary::GetMaxIndex(const RMBCString& rImagePath)
{
	return 0;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::GetMaxBitmapIndex()
//
//  Description:		Returns the highest index of the Bitmaps contained in
//							the file
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YImageIndex RImageLibrary::GetMaxIndex(const RBuffer& rBuffer)
{	
	return 0;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::RImportImage()
//
//  Description:		Loads the Image from memory and returns a pointer to an RImage
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImage* RImageLibrary::ImportImage(const RMBCString& rImagePath, YImageIndex yIndex)
{
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::RImportImage()
//
//  Description:		Loads the Image and returns a pointer to an RImage
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImage* RImageLibrary::ImportImage(const RBuffer& rBuffer, YImageIndex yIndex)
{
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ImportPreviewImage()
//
//  Description:		Loads a smaller version of the Image for previeMacg
//							purposes.
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImage* RImageLibrary::ImportPreviewImage(const RMBCString& rImagePath, const RIntSize& rMaxSize, YImageIndex yIndex)
{
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ImportPreviewImage()
//
//  Description:		Loads a smaller version of the Image for previeMacg
//							purposes.
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImage* RImageLibrary::ImportPreviewImage(const RBuffer& rBuffer, const RIntSize& rMaxSize, YImageIndex yIndex)
{
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ExportImage()
//
//  Description:		Exports the given RImage to a file in the format specified
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::ExportImage(RImage& rImage, const RMBCString& rImagePath, EImageFormat eFormat)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ExportImage()
//
//  Description:		Exports the given RImage to memory in the format specified
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::ExportImage(RImage& rImage, RBuffer& rBuffer, EImageFormat eFormat)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::Brighten()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::Brighten(RBitmapImage& rImage, YBrightness yBrightness)
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ApplyContrast()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::Contrast(RBitmapImage& rImage, YContrast yContrast)
{	
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ApplyTint()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::Tint(RBitmapImage& rImage, YTint yTint)
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::SharpenBitmap()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::Sharpen(RBitmapImage& rImage, YSharpness ySharpness)
{	
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::Crop()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::Crop(RBitmapImage& rImage, RRealRect& rCropArea)
{	
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::GetOffscreenOutline()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::GetOffscreenOutline(RBitmapImage& rImage, const RSolidColor& transparentColor, RPolyPolygon& outline )
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::LoadBitmapImage()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RBitmapImage* RImageLibrary::LoadBitmapImage(const RMBCString& rImagePath, YImageIndex yIndex)
{
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::LoadBitmapImage()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RBitmapImage* RImageLibrary::LoadBitmapImage(const RBuffer& rBuffer, YImageIndex yIndex)
{
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::LoadVectorImage()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RVectorImage* RImageLibrary::LoadVectorImage(const RMBCString& rImagePath)
{
	return NULL;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::LoadVectorImage()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RVectorImage* RImageLibrary::LoadVectorImage(const RBuffer& rBuffer)
{
	return NULL;
}	
	
// ****************************************************************************
//
//  Function Name:	RImageLibrary::IsJpegFile()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::IsJpegFile(const RMBCString& rImagePath)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::SaveBitmapImage()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::SaveBitmapImage(RBitmapImage& rBitmap, const RMBCString& rImagePath, EImageFormat eFormat)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::SaveBitmapImage()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::SaveBitmapImage(RBitmapImage& rBitmap, RBuffer& rBuffer, EImageFormat eFormat)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	SaveVectorImage::LoadVectorImage()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::SaveVectorImage(RVectorImage& rVector, const RMBCString& rImagePath, EImageFormat eFormat)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	SaveVectorImage::LoadVectorImage()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::SaveVectorImage(RVectorImage& rVector, RBuffer& rBuffer, EImageFormat eFormat)
{
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	ThrowExceptionOnLibraryError()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::ThrowExceptionOnLibraryError()
{	
}

#endif //MAC