// *****************************************************************************
//
// File name:			MacBitmapImportLibrary.cpp
//
// Project:				Bitmap Component
//
// Author:				R. Hood
//
// Description:		Mac-specific definition of the RBitmapImportLibrary class members
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
//  $Logfile:: /PM8/Framework/Source/MacBitmapImportLibrary.cpp               $
//   $Author:: Gbeddow                                                         $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#ifdef MAC

#include "BitmapImportLibrary.h"
#include "BitmapImage.h"
#include "OffscreenDrawingSurface.h"
#include "PolyPolygon.h"
#include "DibOutline.h"
#include "Buffer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage


// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::GetMaxBitmapIndex()
//
//  Description:		Returns the highest index of the Bitmaps contained in
//							the file
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YBitmapIndex RBitmapImportLibrary::GetMaxBitmapIndex(const RMBCString& rBitmapPath)
{
	UnimplementedCode();
	return 0;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::GetBitmapInfo()
//
//  Description:		Returns a data structure contatining information about the
//							Bitmap at index yIndex
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RBitmapImportLibrary::GetBitmapInfo(RBitmapInfo& rBitmapInfo, const RMBCString& rBitmapPath, YBitmapIndex yIndex)
{	
	UnimplementedCode();
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::QueryImportBitmap()
//
//  Description:		Returns TRUE if the Bitmap can be imported or FALSE if
//							the type is not recognized
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RBitmapImportLibrary::QueryImportBitmap(const RMBCString& rBitmapPath)
{
	UnimplementedCode();
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::RImportBitmap()
//
//  Description:		Loads the Bitmap from memory and returns a pointer to an RBitmap
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::ImportBitmap(RBitmapImage& rBitmap, const RMBCString& rBitmapPath, YBitmapIndex yIndex)
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::RImportBitmap()
//
//  Description:		Loads the Bitmap and returns a pointer to an RBitmap
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::ImportBitmap(RBitmapImage& rBitmap, RBuffer& rBuffer, YBitmapIndex yIndex)
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::ImportPreviewBitmap()
//
//  Description:		Loads a smaller version of the bitmap for previewing
//							purposes.
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::ImportPreviewBitmap(RBitmapImage& rBitmap, const RMBCString& rBitmapPath, const RIntSize& rMaxSize, YBitmapIndex yIndex)
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::QueryExportBitmap()
//
//  Description:		Returns TRUE if the given bitmap can be exported in the
//							format specified
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RBitmapImportLibrary::QueryExportBitmap(RBitmapImage& rBitmap, YExportFormat yExportFormat)
{
	UnimplementedCode();
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::ExportBitmap()
//
//  Description:		Exports the given RBitmap to memory in the format specified
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::ExportBitmap(RBitmapImage& rBitmap, RBuffer& rBuffer, YExportFormat yExportFormat)
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::ExportBitmap()
//
//  Description:		Exports the given RBitmap to a file in the format specified
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::ExportBitmap(RBitmapImage& rBitmap, const RMBCString& rBitmapPath, YExportFormat yExportFormat)
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::QueryApplyBrightness()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RBitmapImportLibrary::QueryApplyBrightness(RBitmapImage& rBitmap)
{	
	if (rBitmap.GetBitDepth() <= 4)
		return FALSE;
	else
		return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::QueryApplyContrast()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RBitmapImportLibrary::QueryApplyContrast(RBitmapImage& rBitmap)
{
	if (rBitmap.GetBitDepth() <= 4)
		return FALSE;
	else
		return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::QuerySharpenBitmap()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RBitmapImportLibrary::QuerySharpen(RBitmapImage& rBitmap)
{	
	if (rBitmap.GetBitDepth() > 8)
		return TRUE;
	else
		return FALSE;
}

// ****************************************************************************
//
//  Function Name:	ApplyBrightness()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::ApplyBrightness(RBitmapImage& rBitmap, YBrightness yBrightness)
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::ApplyContrast()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::ApplyContrast(RBitmapImage& rBitmap, YContrast yContrast)
{	
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::ApplyTint()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::ApplyTint(RBitmapImage& rBitmap, YTint yTint)
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::SharpenBitmap()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::Sharpen(RBitmapImage& rBitmap, YSharpness ySharpness)
{	
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::Crop()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::Crop(RBitmapImage& rBitmap, RRealRect& rCropArea)
{	
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImportLibrary::Rotate()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImportLibrary::Rotate(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, RBitmapImage& rMaskBitmap, YAngle yRotation)
{
	UnimplementedCode();
}

void RBitmapImportLibrary::FlipHorizontal(RBitmapImage& rBitmap)
{
	UnimplementedCode();
}

void RBitmapImportLibrary::FlipVertical(RBitmapImage& rBitmap)
{
	UnimplementedCode();
}

void RBitmapImportLibrary::GetOffscreenOutline(RBitmapImage& rBitmap, const RSolidColor& transparentColor, RPolyPolygon& outline )
{
	UnimplementedCode();
}


#ifdef TPSDEBUG
void RBitmapImportLibrary::Validate() const
{
}
#endif //TPSDEBUG

#endif 