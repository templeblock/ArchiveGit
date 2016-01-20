// *****************************************************************************
//
// File name:			WinImageLibrary.cpp
//
// Project:				Image Component
//
// Author:				G. Brown
//
// Description:		Windows-specific definition of the RImageLibrary class members
//
// Portability:		Windows only
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
//  $Logfile:: /PM8/Framework/Source/WinImageLibrary.cpp                       $
//   $Author:: Lwilson                                                        $
//     $Date:: 3/08/99 7:16p                                                  $
// $Revision:: 2                                                              $
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#ifdef _WINDOWS

#include "ImageLibrary.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "BitmapImage.h"
#include "VectorImage.h"
#include "OffscreenDrawingSurface.h"
#include "DefaultDrawingSurface.h"
#include "PolyPolygon.h"
#include "DibOutline.h"
#include "Buffer.h"
#include "MemoryMappedBuffer.h"
#include "BufferStream.h"
#include "FileStream.h"
#include "Gear.h"
#include "FPXAPI.H"
#include "PrintManager.h"
#include "Printer.h"
#include "Document.h"
#include "PrinterDrawingSurface.h"
#include "FrameworkResourceIDs.h"
#include "GIFReader.h"
#include "FixFlaw.h"
#include "GifAnimator.h"
//#include "TPSDebugBlit.h"

#pragma pack(push, 2)
#include "Png.h"
#pragma pack(pop)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Define a table to facilitate mapping of EImageFormat values to Accusoft export defines
// NOTE We do not have corresponding entries for metafile formats because Accusoft does
// not support writing metafiles).  See ImageLibrary.h for the definition of EImageFormat,
// from which these defines are mapped.
const int kAccusoftExportFormat[8] = {	IG_SAVE_UNKNOWN, 
													IG_SAVE_BMP_UNCOMP,
													IG_SAVE_PNG,
													IG_SAVE_BMP_RLE,
													IG_SAVE_JPG,
													IG_SAVE_TIF_UNCOMP,
													IG_SAVE_FPX_JPG,
													IG_SAVE_GIF };

const YJpegCompressionQuality kDefaultFpxCompressionQuality = 100 ;

const char kFPXExtension[] = "IGFpx32s.dll" ;
const char kFPXLicense[]   = "BRODERBUND" ;
const char kFPXAuth[]      = "ESTTFB" ;

bool gbFPXInitialized = FALSE;

#undef	FrameworkLinkage
#define	FrameworkLinkage

const HIGEAR kInvalidHandle = 0xFFFFFFFF;

uWORD RImageLibrary::m_uwExtFlags = 0 ;

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
	m_nLastException = ::kNoError;
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
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::GetImageDimensions()
//
//  Description:		Returns the dimensions of an image
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RRealSize RImageLibrary::GetImageDimensions( const RBuffer& rBuffer )
{
	RRealSize szResult( 0, 0 );

	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	AT_ERRCOUNT tErrCount = ::IG_info_get_mem(rBuffer.GetBuffer(), rBuffer.GetBufferSize(), kDefaultImageIndex, &tFileType, &tCompression, &tDib);

	if (!tErrCount)
	{
		szResult.m_dx = tDib.biWidth;
		szResult.m_dy = tDib.biHeight;
	}

	return szResult;
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
	// Convert RMBCString to a char*
	LPCSZ pImagePath = (LPCSZ)rImagePath;
	
	// Clear the error stack
	ClearLastException();

	UINT uPageCount;
	AT_ERRCOUNT tErrCount = ::IG_page_count_get((char*)pImagePath, &uPageCount);
	if (tErrCount > 0)
		return 0;
	else
		return (YImageIndex)uPageCount;
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
	// Clear the error stack
	ClearLastException();

	UINT uPageCount;
	AT_ERRCOUNT tErrCount = ::IG_page_count_get_mem(rBuffer.GetBuffer(), rBuffer.GetBufferSize(), &uPageCount);
	if (tErrCount > 0)
		return 0;
	else
		return (YImageIndex)uPageCount;
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
	// Convert RMBCString to a char*
	LPCSZ pImagePath = (LPCSZ)rImagePath;

	// Clear the error stack
	ClearLastException();

	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	
	RImage* pImage = NULL;
	try
	{
		// Ensure flash pix extension is loaded or we can't get information on them.
		if (!gbFPXInitialized)
		{
			LoadExtension( kImageExtFPX, kFPXExtension, kFPXLicense, kFPXAuth ) ;
			gbFPXInitialized = TRUE;
		}
		AT_ERRCOUNT tErrCount = ::IG_info_get((char*)pImagePath, &tFileType, &tCompression, &tDib);	
		if (tErrCount > 0)
		{
			ThrowExceptionOnLibraryError();			
		}
		else if (!IsSupportedFormat(tFileType))
		{
			throw ::kDataFormatInvalid;
		}
		else if (tFileType == IG_FORMAT_WMF || tFileType == IG_FORMAT_XRX)
		{
			pImage = LoadVectorImage(rImagePath);
		}
		else if (tFileType == IG_FORMAT_FPX && yIndex <= 1)
		{
			yIndex = GetMaxIndex( rImagePath ) ;
			yIndex = (yIndex > 6 ? yIndex - 5 : 1) ;
			pImage = LoadBitmapImage(rImagePath, yIndex);
		}
		else if (IG_FORMAT_PNG == tFileType)
		{
			RImportFileBuffer rBuffer( rImagePath );
			pImage = LoadPNGImage( rBuffer, yIndex );
		}
		else
		{
			pImage = LoadBitmapImage(rImagePath, yIndex);
		}
	}
	catch(YException nException)
	{
		m_nLastException = nException;
		delete pImage;
		pImage = NULL;
	}

	return pImage;
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
	// Clear the error stack
	ClearLastException();

	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	
	RImage* pImage = NULL;
	try
	{
		AT_ERRCOUNT tErrCount = ::IG_info_get_mem(rBuffer.GetBuffer(), rBuffer.GetBufferSize(), yIndex, &tFileType, &tCompression, &tDib);
		if (tErrCount > 0)
		{
			ThrowExceptionOnLibraryError();
		}
		else if (!IsSupportedFormat(tFileType))
		{
			throw ::kDataFormatInvalid;
		}
		else if (tFileType == IG_FORMAT_WMF || tFileType == IG_FORMAT_XRX)
		{
			pImage = LoadVectorImage(rBuffer);
		}
		else if (IG_FORMAT_PNG == tFileType)
		{
			pImage = LoadPNGImage( rBuffer, yIndex );
		}
		else
		{
			pImage = LoadBitmapImage(rBuffer, yIndex);
		}
	}
	catch(YException nException)
	{
		m_nLastException = nException;
		delete pImage;
		pImage = NULL;
	}
	catch (...)
	{
		delete pImage;
		pImage = NULL;
	}

	return pImage;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ImportPreviewImage()
//
//  Description:		Loads a smaller version of the Image for previewing
//							purposes.
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImage* RImageLibrary::ImportPreviewImage(const RMBCString& rImagePath, const RIntSize& rMaxSize, YImageIndex yIndex)
{
	// Convert RMBCString to a char*
	LPCSZ pImagePath = (LPCSZ)rImagePath;

	// Clear the error stack
	ClearLastException();

	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	try
	{
		// Ensure flash pix extension is loaded or we can't get information on them.
		if (!gbFPXInitialized)
		{
			LoadExtension( kImageExtFPX, kFPXExtension, kFPXLicense, kFPXAuth ) ;
			gbFPXInitialized = TRUE;
		}
		if (::IG_info_get((char*)pImagePath, &tFileType, &tCompression, &tDib) > 0) ThrowExceptionOnLibraryError();
	}
	catch(YException nException)
	{
		m_nLastException = nException;
		return NULL;
	}

	// Check the type here: if we are being asked to import a preview of a metafile, just load the actual image
	if (tFileType == IG_FORMAT_WMF || tFileType == IG_FORMAT_XRX) 
		return ImportImage(rImagePath, yIndex);
	else if (tFileType == IG_FORMAT_FPX)
	{
		yIndex = GetMaxIndex( rImagePath ) ;
		yIndex = (yIndex > 3 ? yIndex - 2 : 1) ;

		// Import the bitmap
		return ImportImage( rImagePath, yIndex );
	}
	
	// Compare the aspect ratios and determine the largest import size that 
	// will fit inside the size given and retain the aspect ratio of the original
	RIntSize rImportSize;
	YFloatType yARi = (YFloatType)tDib.biWidth / (YFloatType)tDib.biHeight;
	YFloatType yARp = (YFloatType)rMaxSize.m_dx / (YFloatType)rMaxSize.m_dy;
	if (yARi > yARp)
	{
		rImportSize.m_dx = rMaxSize.m_dx;
		rImportSize.m_dy = (YIntDimension)((YFloatType)rMaxSize.m_dx / yARi);
	}
	else
	{
		rImportSize.m_dx = (YIntDimension)((YFloatType)rMaxSize.m_dy * yARi);
		rImportSize.m_dy = rMaxSize.m_dy;
	}

	// If the actual bitmap dimensions are smaller than the maximum allowed,
	// import the actual bitmap, not a preview
	if (rImportSize.m_dx > tDib.biWidth || rImportSize.m_dy > tDib.biHeight)
	{
		// Import the bitmap
		return ImportImage(rImagePath, yIndex);
	}
	else
	{
		// Set the load size
		::IG_load_size_set(rImportSize.m_dx, rImportSize.m_dy);		

		// Import the bitmap
		RImage* pNewImage = ImportImage(rImagePath, yIndex);
			
		// Reset the load size to normal
		::IG_load_size_set(0, 0);

		return pNewImage;
	}
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ImportPreviewImage()
//
//  Description:		Loads a smaller version of the Image for previewing
//							purposes.
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImage* RImageLibrary::ImportPreviewImage(const RBuffer& rBuffer, const RIntSize& rMaxSize, YImageIndex yIndex)
{
	// Clear the error stack
	ClearLastException();

	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	try
	{
		if (::IG_info_get_mem(rBuffer.GetBuffer(), rBuffer.GetBufferSize(), yIndex, &tFileType, &tCompression, &tDib) > 0) ThrowExceptionOnLibraryError();
	}
	catch(YException nException)
	{
		m_nLastException = nException;
		return NULL;
	}
	
	// Check the type here: if we are being asked to import a preview of a metafile, just load the actual image
	if (tFileType == IG_FORMAT_WMF || tFileType == IG_FORMAT_XRX) return ImportImage(rBuffer, yIndex);
	
	// Compare the aspect ratios and determine the largest import size that 
	// will fit inside the size given and retain the aspect ratio of the original
	RIntSize rImportSize;
	YFloatType yARi = (YFloatType)tDib.biWidth / (YFloatType)tDib.biHeight;
	YFloatType yARp = (YFloatType)rMaxSize.m_dx / (YFloatType)rMaxSize.m_dy;
	if (yARi > yARp)
	{
		rImportSize.m_dx = rMaxSize.m_dx;
		rImportSize.m_dy = (YIntDimension)((YFloatType)rMaxSize.m_dx / yARi);
	}
	else
	{
		rImportSize.m_dx = (YIntDimension)((YFloatType)rMaxSize.m_dy * yARi);
		rImportSize.m_dy = rMaxSize.m_dy;
	}

	// If the actual bitmap dimensions are smaller than the maximum allowed,
	// import the actual bitmap, not a preview
	if (rImportSize.m_dx > tDib.biWidth || rImportSize.m_dy > tDib.biHeight)
	{
		// Import the bitmap
		return ImportImage(rBuffer, yIndex);
	}
	else
	{
		// Set the load size
		::IG_load_size_set(rImportSize.m_dx, rImportSize.m_dy);		

		// Import the bitmap
		RImage* pNewImage = ImportImage(rBuffer, yIndex);
			
		// Reset the load size to normal
		::IG_load_size_set(0, 0);

		return pNewImage;
	}
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
	BOOLEAN fResult = FALSE;
	if (!fResult)
	{
		RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(&rImage);
		if (pBitmap) fResult = SaveBitmapImage(*pBitmap, rImagePath, eFormat);
	}
	if (!fResult)
	{
		RVectorImage* pVector = dynamic_cast<RVectorImage*>(&rImage);
		if (pVector) fResult = SaveVectorImage(*pVector, rImagePath, eFormat);
	}

	return fResult;
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
	BOOLEAN fResult = FALSE;
	if (!fResult)
	{
		RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(&rImage);
		if (pBitmap) fResult = SaveBitmapImage(*pBitmap, rBuffer, eFormat);
	}
	if (!fResult)
	{
		RVectorImage* pVector = dynamic_cast<RVectorImage*>(&rImage);
		if (pVector) fResult = SaveVectorImage(*pVector, rBuffer, eFormat);
	}

	return fResult;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ExportImage()
//
//  Description:	Exports the given image data to file in the 
//                  format specified
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::ExportImage(const RBuffer& rBuffer, const RMBCString& rImagePath, EImageFormat eFormat)
{
	// Clear the error stack
	ClearLastException();

	BOOLEAN bResult = TRUE ;
	HIGEAR hIGear = NULL ;

	try
	{
		AT_DIB	tDib;
		AT_MODE	tFileType, tCompression;
		AT_ERRCOUNT tErrCount ;
			
		// Ensure flash pix extension is loaded or we can't get information on them.
		if (!gbFPXInitialized)
		{
			LoadExtension( kImageExtFPX, kFPXExtension, kFPXLicense, kFPXAuth ) ;
			gbFPXInitialized = TRUE;
		}

		// Get the image information
		tErrCount = ::IG_info_get_mem(rBuffer.GetBuffer(), 
			rBuffer.GetBufferSize(), kDefaultImageIndex, &tFileType, &tCompression, &tDib);	
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// See if we are exporting to the same file type.  If so, just
		// copy one buffer an export buffer with the specified filename.
		if ((kAccusoftExportFormat[eFormat] & tFileType) == tFileType)
		{
			RExportFileBuffer rExBuffer( rImagePath ) ;
			rExBuffer.Resize( rBuffer.GetBufferSize() );			
			memcpy(rExBuffer.GetBuffer(), rBuffer.GetBuffer(), rBuffer.GetBufferSize());			

			return TRUE ;
		}

		AT_LMODE tFormat = kAccusoftExportFormat[eFormat] ;
		UINT nPageNumber = 0 ;

		if (tFileType == IG_FORMAT_FPX)
		{
			// Set the JPEG compression quality
			tErrCount =  IG_image_control_set(IG_CONTROL_FPX_QUALITY, (LPVOID) kDefaultFpxCompressionQuality);
			tErrCount += IG_image_control_set(IG_CONTROL_FPX_SAVE_FROM_DIB, FALSE);
			tErrCount += IG_page_count_get_mem(rBuffer.GetBuffer(), 
				rBuffer.GetBufferSize(), &nPageNumber);

			if (tErrCount > 0) ThrowExceptionOnLibraryError();

			if (nPageNumber > 4)
				nPageNumber -= 3;
			else
				nPageNumber	= 1;
		}

		// Obtain an AccuSoft image gear handle for the image
		tErrCount = ::IG_load_mem( rBuffer.GetBuffer(), 
			rBuffer.GetBufferSize(), nPageNumber, 0, &hIGear ) ;	
		if (tErrCount > 0) ThrowExceptionOnLibraryError();


		// Convert RMBCString to a char*
		LPCSZ pImagePath = (LPCSZ) rImagePath;

		// Export the file
		tErrCount = ::IG_save_file( hIGear, (char *) pImagePath, tFormat );
		if (tErrCount > 0) ThrowExceptionOnLibraryError();
	}
	catch(YException nException)
	{
		m_nLastException = nException;
		bResult = FALSE ;
	}

	if (hIGear)
	{
		::IG_image_delete( hIGear ) ;
	}

	return bResult ;
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
	// Convert RMBCString to a char*
	LPCSZ pImagePath = (LPCSZ)rImagePath;

	// Clear the error stack
	ClearLastException();

	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	AT_ERRCOUNT tErrCount = ::IG_info_get((char*)pImagePath, &tFileType, &tCompression, &tDib);
	if (tErrCount > 0) return FALSE;
		
	if (tFileType == IG_FORMAT_JPG)
		return TRUE;
	else
		return FALSE;
}



// ****************************************************************************
//
//  Function Name:	RImageLibrary::IsGifFile()
//
//  Description:	Determins if the specified file is a GIF formatted file.
//
//  Returns:		TRUE if the file is in GIF format, FALSE otherwise.
//
//  Exceptions:		Clears the last exception.  Possible file access exceptions.
//
// ****************************************************************************
BOOLEAN RImageLibrary::IsGifFile(const RMBCString& rImagePath)
{
	// Convert RMBCString to a char*
	LPCSZ pImagePath = (LPCSZ)rImagePath;

	// Clear the error stack
	ClearLastException();

	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	AT_ERRCOUNT tErrCount = ::IG_info_get((char*)pImagePath, &tFileType, &tCompression, &tDib);
	if (tErrCount > 0)
		return FALSE;
		
	if (tFileType == IG_FORMAT_GIF)
		return TRUE;
	else
		return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::GetImageFormat()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
EImageFormat RImageLibrary::GetImageFormat(const RMBCString& rImagePath)
	{
	// Clear the error stack
	ClearLastException();

	// Convert RMBCString to a char*
	LPCSZ pImagePath = (LPCSZ)rImagePath;

	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	AT_ERRCOUNT tErrCount = ::IG_info_get( (char *) pImagePath, &tFileType, &tCompression, &tDib);
	if (tErrCount > 0) return kImageFormatUnknown;
		
	switch( tFileType )
		{
		case IG_FORMAT_JPG :
			return kImageFormatJPG;

		case IG_FORMAT_BMP :
			return kImageFormatBMP;

		case IG_FORMAT_PNG :
			return kImageFormatPNG;

		case IG_FORMAT_FPX :
			return kImageFormatFPX;

		case IG_FORMAT_WMF :
			return kImageFormatWMF;

		case IG_FORMAT_XRX :
			return kImageFormatXRX;

		case IG_FORMAT_TIF :
			return kImageFormatTIF;

		case IG_FORMAT_PCD :
			return kImageFormatPCD;

		case IG_FORMAT_GIF :
			return kImageFormatGIF;
			
		case IG_FORMAT_EPS:	// GCB 12/17/97 supported for opening pre-5.0 projects (which can contain EPS)
			return kImageFormatEPS;

		case IG_FORMAT_PSD:	// RIP 12/17/98 supported for opening PM projects and content.
			return kImageFormatPSD;

		case IG_FORMAT_PCX:
			return kImageFormatPCX;

		case IG_FORMAT_WPG:
			return kImageFormatWPG;

		default :
			TpsAssertAlways( "Unknown image format" );
			return kImageFormatBMP;
		};
	}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::GetImageFormat()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
EImageFormat RImageLibrary::GetImageFormat(const RBuffer& rBuffer)
	{
	// Clear the error stack
	ClearLastException();

	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	AT_ERRCOUNT tErrCount = ::IG_info_get_mem(rBuffer.GetBuffer(), rBuffer.GetBufferSize(), kDefaultImageIndex, &tFileType, &tCompression, &tDib);
	if (tErrCount > 0) return kImageFormatUnknown;
		
	switch( tFileType )
		{
		case IG_FORMAT_JPG :
			return kImageFormatJPG;

		case IG_FORMAT_BMP :
			return kImageFormatBMP;

		case IG_FORMAT_PNG :
			return kImageFormatPNG;

		case IG_FORMAT_FPX :
			return kImageFormatFPX;

		case IG_FORMAT_WMF :
			return kImageFormatWMF;

		case IG_FORMAT_XRX :
			return kImageFormatXRX;

		case IG_FORMAT_TIF :
			return kImageFormatTIF;

		case IG_FORMAT_PCD :
			return kImageFormatPCD;

		case IG_FORMAT_GIF :
			return kImageFormatGIF;
			
		case IG_FORMAT_EPS: // GCB 12/17/97 supported for opening pre-5.0 projects (which can contain EPS)
			return kImageFormatEPS;

		case IG_FORMAT_PSD:	// RIP 12/17/98 supported for opening PM projects and content.
			return kImageFormatPSD;

		case IG_FORMAT_PCX:
			return kImageFormatPCX;

		case IG_FORMAT_WPG:
			return kImageFormatWPG;

		default :
			TpsAssertAlways( "Unknown image format" );
			return kImageFormatBMP;
		};
	}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::IsFileSupported()
//
//  Description:		Determines if the specified path is a supported image file.
//
//  Returns:			TRUE if the file is supported
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageLibrary::IsFileSupported(const RMBCString& rImagePath)
	{
	// Convert RMBCString to a char*
	LPCSZ pImagePath = (LPCSZ)rImagePath;

	// Clear the error stack
	ClearLastException();

	// Get image info
	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	AT_ERRCOUNT tErrCount = ::IG_info_get((char*)pImagePath, &tFileType, &tCompression, &tDib);
	if (tErrCount > 0) return FALSE;

	return IsSupportedFormat( tFileType );
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
void RImageLibrary::Brighten(RBitmapImage& rBitmap, YBrightness nBrightness)
{
	if (nBrightness == kDefaultBrightness || nBrightness < kMinBrightness || nBrightness > kMaxBrightness) return;

	AT_ERRCOUNT tErrCount;
	HIGEAR hIGear = kInvalidHandle;

	try
	{
		// Get a handle from the Dib
		AT_DIB* pDib = (AT_DIB*)rBitmap.GetRawData();
		tErrCount = ::IG_image_import_DIB(pDib, &hIGear);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// Apply the brightness
		AT_MODE tMode;
		if (rBitmap.GetBitDepth() > 8)
			tMode = IG_CONTRAST_PIXEL;	
		else
			tMode = IG_CONTRAST_PALETTE;

		tErrCount = ::IG_IP_contrast_adjust(hIGear, NULL, tMode, 1.0, (double)nBrightness);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// Update the bitmap
		rBitmap.UpdatePalette();

		// Delete the Accusoft handle
		::IG_image_delete(hIGear);
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;

		// Delete the Accusoft handle
		if (hIGear != kInvalidHandle) ::IG_image_delete(hIGear);
	}
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
void RImageLibrary::Contrast(RBitmapImage& rBitmap, YContrast nContrast)
{
	if (nContrast == kDefaultContrast || nContrast < kMinContrast || nContrast > kMaxContrast) return;

	AT_ERRCOUNT tErrCount;
	HIGEAR hIGear = kInvalidHandle;
	try
	{
		// Map the value of nContrast to a value that can be used by the Accusoft
		// function
		double sContrast = (double)nContrast / 100.0;
		
		// Get a handle from the Dib
		AT_DIB* pDib = (AT_DIB*)rBitmap.GetRawData();
		tErrCount = ::IG_image_import_DIB(pDib, &hIGear);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// Apply the contrast
		AT_MODE tMode;
		if (rBitmap.GetBitDepth() > 8)
			tMode = IG_CONTRAST_PIXEL;	
		else
			tMode = IG_CONTRAST_PALETTE;

		tErrCount = ::IG_IP_contrast_adjust(hIGear, NULL, tMode, sContrast, 0.0);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// Update the bitmap (NOTE This is called for the benefit of 8bpp images,
		// which are not currently supported externally via this API.  8bpp images
		// are supported by the code, however)
		rBitmap.UpdatePalette();

		// Delete the Accusoft handle
		::IG_image_delete(hIGear);
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;

		// Delete the Accusoft handle
		if (hIGear != kInvalidHandle) ::IG_image_delete(hIGear);
	}
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::WritePltt( ... )
//
//  Description:		Write the given bitmaps palette to the given file.
//
//  Returns:			Nothing
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::WritePlttData( const RBitmapImage& rBitmap, const RMBCString& rOutputFileName )
{
	uLONG uBitDepth = rBitmap.GetBitDepth();
	switch(uBitDepth)
	{
	case 8:
		{
		RGBQUAD* pColorData = (RGBQUAD*)RBitmapImage::GetColorData(rBitmap.GetRawData());
		uLONG uColorTableEntries = RBitmapImage::GetColorDataSize(rBitmap.GetRawData()) / sizeof(RGBQUAD);
		
		CFile data;
		data.Open( rOutputFileName, CFile::modeCreate | CFile::modeWrite );
		
		// Write out the palette entries.
		CString dataOut;
		for (uLONG uEntry = 0; uEntry < uColorTableEntries; uEntry++)
			{
			dataOut.Format( "0x%04x,", *((PULONG)(pColorData + uEntry)) << 8 );
			data.Write( dataOut, dataOut.GetLength( ) );
			dataOut.Format( "0x%04x,", *((PULONG)(pColorData + uEntry)) >> 8 );
			data.Write( dataOut, dataOut.GetLength( ) );

			//	Divide every line after 9 entries since this is what DevStudio does.
			if ( uEntry && 0 == uEntry % 9 )
				{
				dataOut.Format( "\n" );
				data.Write( dataOut, dataOut.GetLength( ) );
				}
			}

		dataOut.Format( "\n" );
		data.Write( dataOut, dataOut.GetLength( ) );
		data.Close( );
		}
		break;
	default:
		UnimplementedCode( );
	}
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::Tint()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::Tint(RBitmapImage& rBitmap, RSolidColor& rColor)
{
	const RSolidColor crBlack( kBlack );

	if (rColor == crBlack) 
		return;

	// Build LUTs
   YLUTComponent red_min   = rColor.GetRed();
   YLUTComponent green_min = rColor.GetGreen();
   YLUTComponent blue_min  = rColor.GetBlue();

   uWORD dxRed   = 255 - red_min;
   uWORD dxGreen = 255 - green_min;
	uWORD dxBlue  = 255 - blue_min;

	YLUTComponent red[255], blue[255], green[255];

	for (int i = 0; i < 256; i++)
	{
		red[i]   = red_min + (i * dxRed / 255);
		green[i] = green_min + (i * dxGreen / 255);
		blue[i]  = blue_min + (i * dxBlue / 255);
	}

	RBitmapImage rSrcBitmap( rBitmap );
	ApplyLUTsRGB( rSrcBitmap, rBitmap, red, green, blue );
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
void RImageLibrary::Tint(RBitmapImage& rBitmap, YTint nTint)
{
	if (nTint >= kMaxTint || nTint < kMinTint) return;

	try
	{
		uLONG uBitDepth = rBitmap.GetBitDepth();
		switch(uBitDepth)
		{
		case 1:
		case 4:
		case 8:
			{			
				RGBQUAD* pColorData = (RGBQUAD*)RBitmapImage::GetColorData(rBitmap.GetRawData());
				uLONG uColorTableEntries = RBitmapImage::GetColorDataSize(rBitmap.GetRawData()) / sizeof(RGBQUAD);
				
				// Adjust only the palette entries			
				for (uLONG uEntry = 0; uEntry < uColorTableEntries; uEntry++)
				{
					// Modify the data
					RSolidColor rPaletteEntry(pColorData[uEntry].rgbRed, pColorData[uEntry].rgbGreen, pColorData[uEntry].rgbBlue);
					rPaletteEntry.LightenBy((YRealDimension)(kMaxTint - nTint) / (YRealDimension)kMaxTint);
					pColorData[uEntry].rgbRed = rPaletteEntry.GetRed();
					pColorData[uEntry].rgbGreen = rPaletteEntry.GetGreen();
					pColorData[uEntry].rgbBlue = rPaletteEntry.GetBlue();
				}

				// Update the bitmap
				rBitmap.UpdatePalette();
			}
			break;

		case 16:
		case 32:
			{
				RGBQUAD* pColorData = (RGBQUAD*)RBitmapImage::GetColorData(rBitmap.GetRawData());
				uLONG uColorTableEntries = RBitmapImage::GetColorDataSize(rBitmap.GetRawData()) / sizeof(RGBQUAD);

				uLONG uMaxRed, uMaxGreen, uMaxBlue;
				uBYTE uRedShift, uGreenShift, uBlueShift;
				if (uColorTableEntries > 0)
				{
					// Define the masks
					const uLONG uLowBit = 0x00000001;
					uRedShift = uGreenShift = uBlueShift = 0;

					uMaxRed = *((uLONG*)&pColorData[0]);
					while (!(uMaxRed & uLowBit)) {uMaxRed >>= 1; uRedShift++;}
					uMaxGreen = *((uLONG*)&pColorData[1]);
					while (!(uMaxGreen & uLowBit)) {uMaxGreen >>= 1; uGreenShift++;}
					uMaxBlue = *((uLONG*)&pColorData[2]);
					while (!(uMaxBlue & uLowBit)) {uMaxBlue >>= 1; uBlueShift++;}
				}
				else
				{
					// Use default masks
					if (uBitDepth == 16)
					{
						// Default 16bpp mask is 5-5-5 with the high bit unused
						uMaxRed = uMaxGreen = uMaxBlue = 0x0000001F;
						uRedShift = 10;
						uGreenShift = 5;
						uBlueShift = 0;
					}
					else
					{
						// Default 32bpp mask is 8-8-8 with the hig byte unused
						uMaxRed = uMaxGreen = uMaxBlue = 0x000000FF;
						uRedShift = 16;
						uGreenShift = 8;
						uBlueShift = 0;
					}				
				}

				// Make sure that we can support the color scheme
				if (uMaxRed > kMaxColorComponent || uMaxGreen > kMaxColorComponent || uMaxBlue > kMaxColorComponent)
				{
					TpsAssertAlways("Unsupported color mask.  Cannot tint bitmap.");
					return;
				}

				// Adjust the actual bit data
				uBYTE* pImageData = (uBYTE*)RBitmapImage::GetImageData(rBitmap.GetRawData());
				uLONG uImageDataSize = RBitmapImage::GetImageDataSize(rBitmap.GetRawData());

				// Tint the pixels
				uLONG uIncrement = uBitDepth / 8;
				for (uLONG uIndex = 0; uIndex < uImageDataSize; uIndex += uIncrement)
				{
					RSolidColor rPixel;
					uLONG uPixel;
					if (uBitDepth == 16)
						uPixel = (uLONG)*((uWORD*)&pImageData[uIndex]);
					else
						uPixel = *((uLONG*)&pImageData[uIndex]);
					
					// Scale the color value
					YFloatType yRedScale = (YFloatType)kMaxColorComponent / (YFloatType)uMaxRed;
					YFloatType yGreenScale = (YFloatType)kMaxColorComponent / (YFloatType)uMaxGreen;
					YFloatType yBlueScale = (YFloatType)kMaxColorComponent / (YFloatType)uMaxBlue;

					// Set the color
					YColorComponent yRed = (YColorComponent)::Round((YFloatType)((uPixel >> uRedShift) & uMaxRed) * yRedScale);
					YColorComponent yGreen = (YColorComponent)::Round((YFloatType)((uPixel >> uGreenShift) & uMaxGreen) * yGreenScale);
					YColorComponent yBlue = (YColorComponent)::Round((YFloatType)((uPixel >> uBlueShift) & uMaxBlue) * yBlueScale);
					rPixel = RSolidColor(yRed, yGreen, yBlue);

					// Tint the pixel
					rPixel.LightenBy((YRealDimension)(kMaxTint - nTint) / (YRealDimension)kMaxTint);

					// Unscale the new color value and apply
					uPixel = (((uLONG)::Round((YFloatType)rPixel.GetRed() / yRedScale) << uRedShift) 
						| ((uLONG)::Round((YFloatType)rPixel.GetGreen() / yGreenScale) << uGreenShift) 
						| ((uLONG)::Round((YFloatType)rPixel.GetBlue() / yBlueScale) << uBlueShift));
					if (uBitDepth == 16)
						*((uWORD*)&pImageData[uIndex]) = (uWORD)uPixel;
					else
						*((uLONG*)&pImageData[uIndex]) = uPixel;
				}
			}
			break;

		case 24:
			{
				// Adjust the actual bit data
				uBYTE* pImageData = (uBYTE*)RBitmapImage::GetImageData(rBitmap.GetRawData());
				uLONG uImageDataSize = RBitmapImage::GetImageDataSize(rBitmap.GetRawData());
				for (uLONG uIndex = 0; uIndex < uImageDataSize; uIndex += 3)
				{
					RSolidColor rPixel(pImageData[uIndex + 0], pImageData[uIndex + 1], pImageData[uIndex + 2]);
					rPixel.LightenBy((YRealDimension)(kMaxTint - nTint) / (YRealDimension)kMaxTint);
					pImageData[uIndex + 0] = rPixel.GetRed();
					pImageData[uIndex + 1] = rPixel.GetGreen();
					pImageData[uIndex + 2] = rPixel.GetBlue();
				}
			}
			break;

		default:
			TpsAssertAlways("Unsupported bit depth in RBitmapImage::ApplyTint().");
		}
	}
	catch(...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::Sharpen()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::Sharpen(RBitmapImage& rBitmap, YSharpness nSharpness)
{	
	if (nSharpness == kDefaultSharpness || nSharpness < kMinSharpness || nSharpness > kMaxSharpness) return;

	AT_ERRCOUNT tErrCount;
	HIGEAR hIGear = kInvalidHandle;
	try
	{
		// Get a handle from the Dib
		AT_DIB* pDib = (AT_DIB*)rBitmap.GetRawData();
		tErrCount = ::IG_image_import_DIB(pDib, &hIGear);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// If the value of nSharpness is negative, we invert it and call
		// IG_IP_smooth(); if nSharpness is positive, we call IG_IP_sharpen()
		if (nSharpness > 0)
		{
			// Old Apply the sharpness
			//tErrCount = ::IG_IP_sharpen(hIGear, NULL, (INT)nSharpness);
			//if (tErrCount > 0) ThrowExceptionOnLibraryError();

			// Accusoft sharpen parameters do not step noticeably, so
			// make the best of each of the 4 possible nSharpness values.
			switch ((INT)nSharpness)
			{
				case 1:
					tErrCount = ::IG_IP_sharpen(hIGear, NULL, 2);					
					break;
				case 2:
					tErrCount = ::IG_IP_sharpen(hIGear, NULL, 4);
					break;
				case 3:
					tErrCount = ::IG_IP_sharpen(hIGear, NULL, 5);
					break;
				case 4:
					// hit this one twice
					tErrCount = ::IG_IP_sharpen(hIGear, NULL, 5);
					if (tErrCount > 0) 
						ThrowExceptionOnLibraryError();
					tErrCount = ::IG_IP_sharpen(hIGear, NULL, 5);
					break;
			}
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
		}
		else
		{
			// Apply the smoothness
			tErrCount = ::IG_IP_smooth(hIGear, NULL, (INT)-nSharpness);
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
		}
		
		::IG_image_delete(hIGear);
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;

		// Delete the Accusoft handle
		if (hIGear != kInvalidHandle) ::IG_image_delete(hIGear);
	}
}
// ****************************************************************************
//
//  Function Name:	RImageLibrary::ArtisticEffects()
//
//  Description:		Applies an artistic effect to the image.
//
//  Returns:			Nothing			
//
//  Exceptions:		Library		
//
// ****************************************************************************
void RImageLibrary::ArtisticEffects(RBitmapImage& rBitmap, YArtisticEffect nEffect, YArtisticIntensity nIntensity)
{	
	if(nEffect == kDefaultArtisticEffect) return;

	AT_ERRCOUNT tErrCount;
	HIGEAR hIGear  = kInvalidHandle;
	try
	{
		// Get a handle from the Dib
		AT_DIB* pDib = (AT_DIB*)rBitmap.GetRawData();
		tErrCount = ::IG_image_import_DIB(pDib, &hIGear);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		//WARNING!!!
		//	There is a bug in the Accusoft ImageGear library that causes the libarary to
		//	crash with certain effects on very small bitmaps.  Tests have shown that 9 pixels
		//	is the smallest allowable dimension that the **effects we currently support**,
		//	at the **intensity levels we currently use**.  Some effects and some intensities
		//	will work on smaller bitmaps, but it seems (at this time) to be a wasted effort to
		//	special case all the effects at all intensities when we are talking about such
		//	small bitmaps.
		if ( pDib->biWidth < 9 || pDib->biHeight < 9 )
		{
			::IG_image_delete(hIGear);
			return;
		}

		switch(nEffect)
		{
			case 1: // Blur
				{
					tErrCount = ::IG_FX_blur(hIGear, NULL, nIntensity);
				}
				break;
			case 2: // Diffuse
				{
					//	we need to apply this to a slightly larger bitmap because of an Accusoft bug
					//	[see comment in function header for ApplyEffectToExpandedDIB()]
					ApplyEffectToExpandedDIB( hIGear, nEffect, nIntensity, (UINT)nIntensity + 1, (UINT)nIntensity + 1, 0 );
//					ApplyEffectToExpandedDIB( hIGear, nEffect, nIntensity, 16, 16, 0 );
				}
				break;
			case 3: // Emboss
				{
					tErrCount = ::IG_FX_emboss(hIGear, NULL, (DOUBLE)nIntensity, IG_COMPASS_N);
				}
				break;

			case 4: // Noise
				{
					tErrCount = ::IG_FX_noise(hIGear, NULL, IG_NOISE_LINEAR, nIntensity, 1, 1.0,
														IG_COLOR_COMP_RGB);
				}
				break;

			case 5: // Pixelate
				{
					// Get the size of the bitmap
					AT_DIMENSION nOldWidth, nOldHeight;
					UINT nBpp;
					IG_image_dimensions_get(hIGear, &nOldWidth, &nOldHeight, &nBpp);

					// Get the small side of the image to use for scaling in the pixelation
					long nSmallSide = (nOldWidth < nOldHeight ? nOldWidth : nOldHeight);

					// calculate the scaled block size
					UINT nBlockSize = nSmallSide / (nIntensity * 2);
					if ( nBlockSize < 1 )
					{
						nBlockSize = 1;
					}

					//	we need to apply this to a slightly larger bitmap because of an Accusoft bug
					//	[see comment in function header for ApplyEffectToExpandedDIB()]
					UINT nExtraWidth  = nBlockSize - (nOldWidth  % nBlockSize) + 1;
					UINT nExtraHeight = nBlockSize - (nOldHeight % nBlockSize) + 1;

					// apply the effect to the expanded bitmap
					ApplyEffectToExpandedDIB( hIGear, nEffect, nIntensity, nExtraWidth, nExtraHeight, nBlockSize );
				}
				break;
			case 6: // Posterize
				{
					tErrCount = ::IG_FX_posterize(hIGear, NULL, nIntensity, IG_COLOR_COMP_RGB);
				}
				break;
			case 7: // Emboss w/Stitch pattern
				{
					tErrCount = ::IG_FX_stitch(hIGear, NULL, IG_COMPASS_NE, nIntensity);
				}
				break;
		default:
				TRACE("Unsupported effect index in RImageLibrary::ArtisticEffects.");
		}
		if (tErrCount > 0) ThrowExceptionOnLibraryError();
				
		::IG_image_delete(hIGear);
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;

		// Delete the Accusoft handle
		if (hIGear  != kInvalidHandle) ::IG_image_delete(hIGear);
	}
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ApplyEffectToExpandedDIB()
//
//  Description:		Applies an artistic effect to the image after making the image bigger
//
//							There is a "peculiarity" in the Accusoft ImageGear library
//							(I would call it a "bug," but Accusoft claims that the behavior
//							is intentional and correct).  The ImageGear Pixelate effect will
//							create only as many "large pixels" as it can at full size 
//							within the given image dimensions (e.g., if you specify a
//							block size of 10 for a 87 x 87 bitmap, you get 8 large pixels
//							in each direction, leaving an un-pixelated border of 7 pixels
//							at the right and bottom). (Actually, it's worse than that: you
//							need a 91 x 91 bitmap to coax Accusoft to give you 9 x 9 grid of
//							blocks of size 10.)
//
//							The Diffuse effect has a similar "peculiarity," although it is
//							not as predictable.  Tacking on nIntesity + 1 pixels in each direction
//							seems to work.
//
//							In any case, to get the effect we want, we create a second DIB that
//							is large enough to insure that our entire image will be affected,
//							then copy back the portion of the new image that fits our original
//							bitmap dimensions.
//
//  Returns:			Nothing			
//
//  Exceptions:		Library		
//
// ****************************************************************************
void RImageLibrary::ApplyEffectToExpandedDIB(HIGEAR hIGear, YArtisticEffect nEffect, YArtisticIntensity nIntensity, UINT nExtraWidth, UINT nExtraHeight, UINT nBlockSize )
{
	AT_ERRCOUNT tErrCount;
	HIGEAR hIGear2 = kInvalidHandle;			// for our expanded DIB

	try
	{
		// Get the size of the existing bitmap
		AT_DIMENSION nOldWidth, nOldHeight;
		UINT nBpp;
		::IG_image_dimensions_get(hIGear, &nOldWidth, &nOldHeight, &nBpp);
		TpsAssert( nBpp == 24, "Effects will only be applied to 24-bit images" );
		if ( nBpp != 24 ) ThrowExceptionOnLibraryError();

		// create a new image large enough to insure that we will affect the entire image
		AT_DIMENSION nNewWidth  = nOldWidth  + nExtraWidth;
		AT_DIMENSION nNewHeight = nOldHeight + nExtraHeight;

		tErrCount = ::IG_image_create_DIB( nNewWidth, nNewHeight, nBpp, NULL, &hIGear2 );
		if (tErrCount > 0) ThrowExceptionOnLibraryError();
		
		// some variables for 24-bit manipulations
		const AT_DIMENSION nOldRowBytes = nOldWidth * 3;
		const AT_DIMENSION nNewRowBytes = nNewWidth * 3;

		// scan line buffer
		AT_PIXEL *pBuffer = new AT_PIXEL[nNewRowBytes];

		AT_DIMENSION x, y, x2, y2;
		for ( y = 0; y < nOldHeight; y++ )
		{
			// get our original pixel data one row at a time
			tErrCount = ::IG_DIB_row_get( hIGear, 0, y, nOldWidth, pBuffer, IG_PIXEL_UNPACKED );
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
			
			// fill in the extra pixels by "mirroring" the end of the row
			x2 = nOldRowBytes - 3;
			for ( x = nOldRowBytes; x < nNewRowBytes; x += 3 )
			{
				memcpy( pBuffer + x, pBuffer + x2, 3 );
				x2 -= 3;
				if ( x2 < 0 )						// watch out for really small bitmaps
				{
					x2 = nOldRowBytes - 3;		// and keep recycling
				}
			}
			
			// write the expanded scan line into the new DIB
			tErrCount = ::IG_DIB_row_set( hIGear2, 0, y, nNewWidth, pBuffer, IG_PIXEL_UNPACKED );
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
		}

		// now fill in the extra rows by mirroring the bottom scan lines
		y2 = nOldHeight - 1;
		for ( ; y < nNewHeight; y++ )
		{
			tErrCount = ::IG_DIB_row_get( hIGear2, 0, y2, nNewWidth, pBuffer, IG_PIXEL_UNPACKED );
			if (tErrCount > 0) ThrowExceptionOnLibraryError();

			tErrCount = ::IG_DIB_row_set( hIGear2, 0, y, nNewWidth, pBuffer, IG_PIXEL_UNPACKED );
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
			y2--;
			if ( y2 < 0 )					// watch out for really small bitmaps
			{
				y2 = nOldHeight - 1;		// and keep recycling
			}
		}
			
		switch ( nEffect )
		{
		case 2:				// diffuse
			tErrCount = ::IG_FX_diffuse(hIGear2, NULL, nIntensity);
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
			break;

		case 5:				// pixelate
			tErrCount = ::IG_FX_pixelate(hIGear2, NULL, nBlockSize, nBlockSize, IG_RESAMPLE_IN_AVE, IG_RESAMPLE_OUT_SQUARE, 0, NULL);
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
			break;

		default:
			TRACE("Unsupported effect index in RImageLibrary::ApplyEffectToExpandedDIB.");
			break;
		}

		// write back the appropriate amount of pixelated data into our original bitmap
		for ( y = 0; y < nOldHeight; y++ )
		{
			tErrCount = ::IG_DIB_row_get( hIGear2, 0, y, nOldWidth, pBuffer, IG_PIXEL_UNPACKED );
			if (tErrCount > 0) ThrowExceptionOnLibraryError();

			tErrCount = ::IG_DIB_row_set( hIGear, 0, y, nOldWidth, pBuffer, IG_PIXEL_UNPACKED );
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
		}

		::IG_image_delete(hIGear);
		delete[] pBuffer;
	}

	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;
		if (hIGear2 != kInvalidHandle) ::IG_image_delete(hIGear2);
		throw;
	}
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ApplyLUTsRGB()
//
//  Description:	Uses Accusoft library to apply RGB LUTs to bitmap
//
//  Returns:		Nothing
//
// ****************************************************************************
void RImageLibrary::ApplyLUTsRGB(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, YLUTComponent *pRedLUT, YLUTComponent *pGreenLUT, YLUTComponent *pBlueLUT )
{	
	AT_ERRCOUNT tErrCount;
	HIGEAR hIGear = kInvalidHandle;
	try
	{
		// Get a handle from the Dib
		AT_DIB* pDib = (AT_DIB*)rSrcBitmap.GetRawData();
		tErrCount = ::IG_image_import_DIB(pDib, &hIGear);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// set the LUTs
		tErrCount = ::IG_display_LUT_set( hIGear, (LPAT_PIXEL)pRedLUT, (LPAT_PIXEL)pGreenLUT, (LPAT_PIXEL)pBlueLUT );
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// set the destination bitmap into an offscreen DS		
		ROffscreenDrawingSurface rODS;
		rODS.SetImage( &rDestBitmap );		
		HDC hDC = (HDC)rODS.GetSurface();
		if ( hDC )
		{
			//WARNING!!!
			//	there is a bug in the Accusoft ImageGear library that causes the image to be
			//	shrunken by one pixel when IG_display_image() is used with the default device
			//	rectangle (ie, none specified).  However, if you use IG_device_rect_set() and
			//	tell ImageGear that the output rect is 1 pixel higher than it actually is, the
			//	blit is correct.
			//
			AT_RECT rect;
			rect.left   = 0;
			rect.top    = 0;
			rect.right  = rSrcBitmap.GetWidthInPixels();
			rect.bottom = rSrcBitmap.GetHeightInPixels() + 1;	// !!

			tErrCount = ::IG_device_rect_set( hIGear, &rect );
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
			
			tErrCount = ::IG_display_image( hIGear, hDC );
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
		}

		// release the offscreen bitmap
		rODS.ReleaseImage();
				
		::IG_image_delete(hIGear);
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;

		// Delete the Accusoft handle
		if (hIGear != kInvalidHandle) ::IG_image_delete(hIGear);
	}
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ConvertToGrayscale()
//
//  Description:	Uses Accusoft library to convert a 32-bit image to grayscale
//
//  Returns:		Nothing
//
// ****************************************************************************
void RImageLibrary::ConvertToGrayscale( RBitmapImage& rBitmap )
{	
	AT_ERRCOUNT tErrCount;
	HIGEAR hIGear = kInvalidHandle;
	try
	{
		// Get a handle from the Dib
		AT_DIB* pDib = (AT_DIB*)rBitmap.GetRawData();
		tErrCount = ::IG_image_import_DIB(pDib, &hIGear);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// remember the size of header + data
		uLONG uSize = rBitmap.GetRawDataSize();

		// do that grayscale thang you do
		tErrCount = ::IG_IP_convert_to_gray( hIGear );
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// Accusoft converts to 8-bit grayscale, so we'll promote it back to 24				
		tErrCount = ::IG_IP_color_promote( hIGear, IG_PROMOTE_TO_24 );
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// get the new DIB from Accusoft and copy it to our image data
		AT_DIB* pNewDib;
		tErrCount = ::IG_image_export_DIB( hIGear, &pNewDib );
		memcpy( pDib, pNewDib, uSize );

		// note: IG_image_export_DIB() deletes up the hIGear
		// ::IG_image_delete(hIGear);
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;

		// Delete the Accusoft handle
		if (hIGear != kInvalidHandle) ::IG_image_delete(hIGear);
	}
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::RemoveEncircledChromaRed()
//
//  Description:	Used for "red eye" removal in photos
//
//  Returns:		TRUE on success, FALSE on error or no effect
//
// ****************************************************************************
BOOLEAN	RImageLibrary::RemoveEncircledChromaRed( RBitmapImage& rBitmap, const RIntRect& rRect )
{
	RFixFlaw rFixFlaw( rBitmap, rRect, *this );
	return rFixFlaw.RemoveEncircledChroma( kDiscreteHueRed );
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::RemoveEncircledChromaAny()
//
//  Description:	Used for "pet eye" removal in photos
//
//  Returns:		TRUE on success, FALSE on error or no effect
//
// ****************************************************************************
BOOLEAN	RImageLibrary::RemoveEncircledChromaAny( RBitmapImage& rBitmap, const RIntRect& rRect )
{
	RFixFlaw rFixFlaw( rBitmap, rRect, *this );
	return rFixFlaw.RemoveEncircledChroma( kDiscreteHueAny );
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::RemoveScratch()
//
//  Description:	Used to remove dust / scratches from photos
//
//  Returns:		TRUE on success, FALSE on error or no effect
//
// ****************************************************************************
BOOLEAN RImageLibrary::RemoveScratch( RBitmapImage& rBitmap, const RIntRect& rRect )
{
	RFixFlaw rFixFlaw( rBitmap, rRect, *this );
	return rFixFlaw.RemoveScratch();
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::RemoveShine()
//
//  Description:	Used to reduce "shiney faces" in photos
//
//  Returns:		TRUE on success, FALSE on error or no effect
//
// ****************************************************************************
BOOLEAN RImageLibrary::RemoveShine( RBitmapImage& rBitmap, const RIntRect& rRect )
{
	RFixFlaw rFixFlaw( rBitmap, rRect, *this );
	return rFixFlaw.RemoveShine();
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
void RImageLibrary::Crop(RBitmapImage& rBitmap, RRealRect& rCropArea)
{	
	TpsAssert(rCropArea.m_Left >= 0.0 && rCropArea.m_Left < 1.0, "Invalid crop rect.");
	TpsAssert(rCropArea.m_Top >= 0.0 && rCropArea.m_Top < 1.0, "Invalid crop rect.");
	TpsAssert(rCropArea.m_Right > 0.0 && rCropArea.m_Right <= 1.0, "Invalid crop rect.");
	TpsAssert(rCropArea.m_Bottom > 0.0 && rCropArea.m_Bottom <= 1.0, "Invalid crop rect.");

	// We don't need to crop anything
	if (rCropArea.m_Left == 0.0 && rCropArea.m_Top == 0.0 && rCropArea.m_Right == 1.0 && rCropArea.m_Bottom == 1.0) return;

	try
	{		
		// NOTE Accusoft's function does not seem to function as documented:
		// All IG_IP... functions are supposed to affect the image pixels directly,
		// as opposed to affecting only the manner in which the image is displayed
		// using the Accusoft display function; however, IG_IP_crop() seems to be 
		// an exception to this.  This function replaces the Accusoft version.	
		
		// Determine the new width and height of the bitmap
		YRealDimension yBitmapWidth = (YRealDimension)rBitmap.GetWidthInPixels();
		YRealDimension yBitmapHeight = (YRealDimension)rBitmap.GetHeightInPixels();		
		uLONG uNewWidth  = ::Round( rCropArea.m_Right * yBitmapWidth );
		uLONG uNewHeight = ::Round( rCropArea.m_Bottom * yBitmapHeight );
		
		// Create a temporary bitmap based on the original into which to copy the
		// cropped image (we do this by setting the source bitmap into an offscreen
		// drawing surface and then initializing the bitmap based on it)
		RBitmapImage rDestBitmap;
		ROffscreenDrawingSurface rODS;
		rODS.SetImage(&rBitmap);
		try
			{
			rDestBitmap.Initialize(rODS, uNewWidth, uNewHeight);
			rODS.ReleaseImage();
			}
		catch( ... )
			{
			rODS.ReleaseImage();
			throw;
			}

		// Crop the image
		ROffscreenDrawingSurface rDestDS;		
		rDestDS.SetImage(&rDestBitmap);

		RIntRect rDestRect(0, 0, uNewWidth, uNewHeight);	
		RIntRect rSrcRect((int)(rCropArea.m_Left * yBitmapWidth), (int)(rCropArea.m_Top * yBitmapHeight), (int)(rCropArea.m_Left * yBitmapWidth + uNewWidth), (int)(rCropArea.m_Top * yBitmapHeight + uNewHeight));
		rBitmap.Render(rDestDS, rSrcRect, rDestRect);		
		rDestDS.ReleaseImage();

		// Copy the temp bitmap into the source bitmap
		rBitmap = rDestBitmap;

		// Crop the alpha channel if one exists
		if (rBitmap.GetAlphaChannel())
			Crop( *rBitmap.GetAlphaChannel(), rCropArea );
	}
	catch(...)
	{
	}
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ChangeBitDepth()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::ChangeBitDepth(RBitmapImage& rBitmap, uLONG uNewBitDepth)
{
	RBitmapImage rNewBitmap;
	rNewBitmap.Initialize(rBitmap.GetWidthInPixels(), rBitmap.GetHeightInPixels(), uNewBitDepth);

	// Copy over the original image's palette if converting to under 24-bit.
	if (uNewBitDepth < 24)
	{
		uLONG uOldBitDepth = rBitmap.GetBitDepth();
		if (uOldBitDepth < uNewBitDepth)
		{
			uWORD uNumberOfEntriesNew = (uWORD)(rNewBitmap.GetColorDataSize( uNewBitDepth ) / sizeof(RGBQUAD));
			uWORD uNumberOfEntriesOld = (uWORD)(rBitmap.GetColorDataSize( uOldBitDepth ) / sizeof(RGBQUAD));
			BITMAPINFO *pBitmapInfoNew = (BITMAPINFO*)rNewBitmap.GetRawData();
			BITMAPINFO *pBitmapInfoOld = (BITMAPINFO*)rBitmap.GetRawData();
			// Initialize the palette to 0's to clear it.
			memset( &(pBitmapInfoNew->bmiColors), 0, sizeof(RGBQUAD)*uNumberOfEntriesNew );

			// Copy over the data from the old image.
			for (uWORD nIndex = 0; nIndex < uNumberOfEntriesOld; nIndex++)
			{
				pBitmapInfoNew->bmiColors[nIndex].rgbRed = pBitmapInfoOld->bmiColors[nIndex].rgbRed;
				pBitmapInfoNew->bmiColors[nIndex].rgbGreen = pBitmapInfoOld->bmiColors[nIndex].rgbGreen;
				pBitmapInfoNew->bmiColors[nIndex].rgbBlue = pBitmapInfoOld->bmiColors[nIndex].rgbBlue;
			}
		}
	}

	// Copy the original bitmap
	ROffscreenDrawingSurface rODS;	
	rODS.SetImage(&rNewBitmap);

	// Restore the image's content.
	if (rBitmap.IsTransparent())
	{
		// Fill the background with the transparent color.
		RIntRect rImageRect( 0, 0, rNewBitmap.GetWidthInPixels(), rNewBitmap.GetHeightInPixels() );
		rODS.SetFillColor( rBitmap.GetTransparentColor() );
		rODS.FillRectangle( rImageRect );
	}

	rBitmap.Render(rODS, RIntRect(0, 0, rBitmap.GetWidthInPixels(), rBitmap.GetHeightInPixels()));

	rODS.ReleaseImage();

	// Copy the 24bpp image into the original
	rBitmap = rNewBitmap;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::Rotate()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::Rotate(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, RBitmapImage& rMaskBitmap, YAngle yRotation)
{
	// RotateBitmap() is a special case of TransformBitmap(), so we just call TransformBitmap().
	RIntSize rSrcSize(rSrcBitmap.GetWidthInPixels(), rSrcBitmap.GetHeightInPixels());
	
	R2dTransform rTransform;
	rTransform.PreRotateAboutPoint(rSrcSize.m_dx / 2, rSrcSize.m_dy / 2, yRotation);	
	RIntVectorRect rDestVectorRect(0, 0, rSrcSize.m_dx, rSrcSize.m_dy);
	rDestVectorRect *= rTransform;
	rTransform.PostTranslate(-rDestVectorRect.m_TransformedBoundingRect.m_Left, -rDestVectorRect.m_TransformedBoundingRect.m_Top);

	Transform(rSrcBitmap, rDestBitmap, rMaskBitmap, rTransform);
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::TransformBitmap()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::Transform(RBitmapImage& rSrcBitmap, RBitmapImage& rDestBitmap, RBitmapImage& rMaskBitmap, const R2dTransform& rTransform)
{
	// REVIEW GCB 5/8/98 - there are still occasionally times when a 1-bit
	// mask is being transformed so we must use the old, slower bitmap transform
	// in these cases - task for later is to add fast 1-bit transform code?

	uLONG	bitDepth = rSrcBitmap.GetBitDepth();
 	if (bitDepth == 8 || bitDepth == 24)
	{
		// fast bitmap transform: supports 8-bit and 24-bit only

		YAngle			rotation;
		YScaleFactor	xScale;
		YScaleFactor	yScale;
		rTransform.Decompose( rotation, (YRealDimension) xScale, (YRealDimension) yScale );

		RCTransform( rSrcBitmap, rDestBitmap, rMaskBitmap, rotation, xScale, yScale );
	}
	else
	{
		// slower bitmap transform: supports all bit depths

		TRACE( "Warning: Not 8-bit or 24-bit: Using old, slower bitmap transform\n" );

		const uBYTE uMask1Bit[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};		
		const uBYTE uMask4Bit[2] = {0xF0, 0x0F};
		
		R2dTransform	rIdentityTransform;
		YAngle			rotation;
		YRealDimension	xScale;
		YRealDimension	yScale;
		rTransform.Decompose( rotation, xScale, yScale );
		if (rTransform == rIdentityTransform)
		{
			// If we've been given the identity transform, simply copy the source
			// bitmap into the destination bitmap
			rDestBitmap = rSrcBitmap;		
			
			// Set all the pixels in the mask to white
			rMaskBitmap.Initialize(rDestBitmap.GetWidthInPixels(), rDestBitmap.GetHeightInPixels(), 1);
			memset(RBitmapImage::GetImageData(rMaskBitmap.GetRawData()), 0xFF, RBitmapImage::GetImageDataSize(rMaskBitmap.GetRawData()));
		}
		else if (rotation == 0.0)
		{
			// Determine the size for the destination bitmap
			RIntVectorRect rDestVectorRect(0, 0, rSrcBitmap.GetWidthInPixels(), rSrcBitmap.GetHeightInPixels());
			rDestVectorRect *= rTransform;
			RIntRect rDestRect = rDestVectorRect.m_TransformedBoundingRect;

			// Initialize the destination bitmap and the monochrome mask
			ROffscreenDrawingSurface rODS;
			rODS.SetImage(&rSrcBitmap);
			try
				{
				rDestBitmap.Initialize(rODS, rDestRect.Width(), rDestRect.Height());
				rODS.ReleaseImage();
				}
			catch( ... )
				{
				rODS.ReleaseImage();
				throw;
				}

			// Determine the destination rect based on the scale members		
			if (xScale < 0)
			{
				// Swap left and right members
				YIntCoordinate yTempLeft = rDestRect.m_Left;
				rDestRect.m_Left = rDestRect.m_Right;
				rDestRect.m_Right = yTempLeft;
			}

			if (yScale < 0)
			{
				// Swap top and bottom members
				YIntCoordinate yTempTop = rDestRect.m_Top;
				rDestRect.m_Top = rDestRect.m_Bottom;
				rDestRect.m_Bottom = yTempTop;
			}

			// Copy the source image into the dest image
			rODS.SetImage(&rDestBitmap);
			if (rSrcBitmap.IsTransparent())
			{
				rSrcBitmap.DisableTransparency();
				rSrcBitmap.Render(rODS, rDestRect);
				rSrcBitmap.EnableTransparency();
			}
			else
			{
				rSrcBitmap.Render(rODS, rDestRect);
			}

			rODS.ReleaseImage();
			
			// Set all the pixels in the mask to white
			rMaskBitmap.Initialize(rDestBitmap.GetWidthInPixels(), rDestBitmap.GetHeightInPixels(), 1);
			memset(RBitmapImage::GetImageData(rMaskBitmap.GetRawData()), 0xFF, RBitmapImage::GetImageDataSize(rMaskBitmap.GetRawData()));
		}
		else
		{
			// Determine the size of the destination bitmap
			RIntSize rSrcSize(rSrcBitmap.GetWidthInPixels(), rSrcBitmap.GetHeightInPixels());

			// Copy the transform
			R2dTransform rBitmapTransform = rTransform;

			// Our coordinates are bottom-up instead of top-down
			rBitmapTransform.PreTranslate(0.0, rSrcSize.m_dy);
			rBitmapTransform.PreScale(1.0, -1.0);

			// Determine the size for the destination bitmap		
			RIntVectorRect rDestVectorRect(0, 0, rSrcSize.m_dx, rSrcSize.m_dy);
			rDestVectorRect *= rBitmapTransform;	
			
			// Compute the pixel width of the destination bitmap
			RIntSize rDestSize;
			rDestSize.m_dx = rDestVectorRect.m_TransformedBoundingRect.Width();
			rDestSize.m_dy = rDestVectorRect.m_TransformedBoundingRect.Height();		

			// Define an offscreen drawing surface to do some work on the bitmaps
			ROffscreenDrawingSurface rODS;

			// Initialize the destination bitmap and the monochrome mask
			rODS.SetImage(&rSrcBitmap);
			try
				{
				//	next line can fail badly - see bug 3107
				rDestBitmap.Initialize(rODS, rDestSize.m_dx, rDestSize.m_dy);
				rODS.ReleaseImage();
				}
			catch( ... )
				{
				rODS.ReleaseImage();
				throw;
				}

			rMaskBitmap.Initialize(rDestSize.m_dx, rDestSize.m_dy, 1);

			// Set the image data to all 0's
			memset(RBitmapImage::GetImageData(rDestBitmap.GetRawData()), 0x00, RBitmapImage::GetImageDataSize(rDestBitmap.GetRawData()));
			memset(RBitmapImage::GetImageData(rMaskBitmap.GetRawData()), 0x00, RBitmapImage::GetImageDataSize(rMaskBitmap.GetRawData()));

			// Convert all single-index image data to 2d (double-index) image data
			uBYTE** ppSourceImage = RBitmapImage::Get2dImageData(rSrcBitmap.GetRawData());
			uBYTE** ppDestImage = RBitmapImage::Get2dImageData(rDestBitmap.GetRawData());
			uBYTE** ppMaskImage = RBitmapImage::Get2dImageData(rMaskBitmap.GetRawData());

			// Invert the transform
			rBitmapTransform.Invert();
			
			// Iterate over the destination bitmap to map pixels back to the source bitmap
			RIntPoint rSrcPoint, rDestPoint;
			RIntRect rSrcRect(rSrcSize);
			for (rDestPoint.m_y = 0; rDestPoint.m_y < rDestSize.m_dy; rDestPoint.m_y++)
			{
				for (rDestPoint.m_x = 0; rDestPoint.m_x < rDestSize.m_dx; rDestPoint.m_x++)
				{
					// Apply the inverse transform to the destination point to find the
					// corresponding source point
					RIntPoint rDibPoint(rDestPoint.m_x, rDestSize.m_dy - rDestPoint.m_y);
					rSrcPoint = rDibPoint * rBitmapTransform;
					if (rSrcRect.PointInRect(rSrcPoint))
					{
							// Set the pixels in the mask
						uLONG uMaskByte = rDestPoint.m_x >> 3;
						ppMaskImage[rDestPoint.m_y][uMaskByte] = (uBYTE)(ppMaskImage[rDestPoint.m_y][uMaskByte] | uMask1Bit[~rDestPoint.m_x & 0x07]);
						
						// Set the pixels in the actual bitmap	
						uLONG uDestByte, uSrcByte;				
						switch (rSrcBitmap.GetBitDepth())
						{			
						case 1:
							{			
								uDestByte = rDestPoint.m_x >> 3;
								uSrcByte = rSrcPoint.m_x >> 3;
								if (ppSourceImage[rSrcPoint.m_y][uSrcByte] & uMask1Bit[(~rSrcPoint.m_x) & 0x07])
								{
									// If the bit is set in the source, set it in the destination
									ppDestImage[rDestPoint.m_y][uDestByte] = (uBYTE)(ppDestImage[rDestPoint.m_y][uDestByte] | uMask1Bit[~rDestPoint.m_x & 0x07]);
								}
								break;
							}

						case 4:
							{
								uSrcByte = rSrcPoint.m_x >> 1; // Divide by 2
								uBYTE uSourceIndex = (uBYTE)(rSrcPoint.m_x & 0x01); // Get remainder	
								uDestByte = rDestPoint.m_x >> 1; // Divide by 2
								uBYTE uDestIndex = (uBYTE)(rDestPoint.m_x & 0x01); // Get remainder

								uBYTE uSourcePixel = ppSourceImage[rSrcPoint.m_y][uSrcByte];
								if (uDestIndex && !uSourceIndex)
								{						
									uSourcePixel >>= 4;					
								}
								else if (!uDestIndex && uSourceIndex)
								{
									uSourcePixel <<= 4;
								}

								ppDestImage[rDestPoint.m_y][uDestByte] = (uBYTE)((ppDestImage[rDestPoint.m_y][uDestByte] & (~uMask4Bit[uDestIndex])) | (uBYTE)(uSourcePixel & (uMask4Bit[uDestIndex])));
							}
							break;

						case 8:
							ppDestImage[rDestPoint.m_y][rDestPoint.m_x] = ppSourceImage[rSrcPoint.m_y][rSrcPoint.m_x];
							break;
						
						case 16:
							// Cast to short
							uDestByte = rDestPoint.m_x * 2;
							uSrcByte = rSrcPoint.m_x * 2;
							*((uWORD*)&ppDestImage[rDestPoint.m_y][uDestByte]) = *((uWORD*)&ppSourceImage[rSrcPoint.m_y][uSrcByte]);
							break;

						case 24:
							// Cast to short/byte
							uDestByte = rDestPoint.m_x * 3;
							uSrcByte = rSrcPoint.m_x * 3;
							*((uWORD*)&ppDestImage[rDestPoint.m_y][uDestByte]) = *((uWORD*)&ppSourceImage[rSrcPoint.m_y][uSrcByte]);
							*((uBYTE*)&ppDestImage[rDestPoint.m_y][uDestByte + 2]) = *((uBYTE*)&ppSourceImage[rSrcPoint.m_y][uSrcByte + 2]);
							break;

						case 32:
							// Cast to short
							uDestByte = rDestPoint.m_x * 4;
							uSrcByte = rSrcPoint.m_x * 4;
							*((uLONG*)&ppDestImage[rDestPoint.m_y][uDestByte]) = *((uLONG*)&ppSourceImage[rSrcPoint.m_y][uSrcByte]);
							break;
						}
					}
				}
			}

			// Clean up
			delete [] ppSourceImage;
			delete [] ppDestImage;	
			delete [] ppMaskImage;
		}
	}

	// Transform the alpha channel if there is one.
	RBitmapImage* pSrcAlphaChannel = rSrcBitmap.GetAlphaChannel();

	if (pSrcAlphaChannel)
	{
		RBitmapImage* pDestAlphaChannel = rDestBitmap.GetAlphaChannel();

		if (!pDestAlphaChannel)
		{
			pDestAlphaChannel = new RBitmapImage(); 
			rDestBitmap.SetAlphaChannel( pDestAlphaChannel );
		}

		if (pDestAlphaChannel)
		{
			Transform( *pSrcAlphaChannel, *pDestAlphaChannel, rMaskBitmap, rTransform );
			rMaskBitmap = *pDestAlphaChannel;
		}
	}

}


/****************************************************************************
 *
 *  Function Name:	RImageLibrary::RCTransform()
 *
 * Parameters:					Type	Description
 *				rSrcBitmap		read	input image
 *				rDstBitmap		write	output image (created by this routine)
 *				rMaskBitmap		write	mask image (created by this routine)
 *				rotationAngle	read	angle to rotate the source image
 *				xScale			read	amount to scale the source image horizontally
 *				yScale			read	amount to scale the source image vertically
 *
 * Description: Rotates and scales an image using point filtering.
 *
 *  Author: Rob Cook
 *          March 30, 1998	
 *
 ****************************************************************************/

#define ABS(x) ( ((x) >= 0 ) ? (x) : -(x) )
#define ROUND(x) ( floor( (x) + 0.5 ) )

void RImageLibrary::RCTransform(	RBitmapImage&	rSrcBitmap,
											RBitmapImage&	rDestBitmap,
											RBitmapImage&	rMaskBitmap,
											YAngle			rotationAngle,
											YScaleFactor	xScale,
											YScaleFactor	yScale
											)
{
	RImageLibrary::RCTransform( rSrcBitmap, rDestBitmap, rMaskBitmap,
									 rotationAngle, xScale, yScale, NULL, NULL );
}


/****************************************************************************
 *
 *  Function Name:	RImageLibrary::RCTransform()
 *
 * Parameters:						Type	Description
 *				rSrcBitmap			read	input image
 *				rDstBitmap			write	output image (created by this routine)
 *				rMaskBitmap			write	mask image (created by this routine)
 *				rotationAngle		read	angle to rotate the source image
 *				xScale				read	amount to scale the source image horizontally
 *				yScale				read	amount to scale the source image vertically
 *				tileRightOffset	read	offset in pixels (x and y) to the next
 *												tile to the right of the source
 *				tileBelowOffset	read	offset in pixels (x and y) to the next
 *												tile below the source
 *
 * Description: Rotates and scales an image using point filtering.
 *
 *  Author: Rob Cook
 *          March 30, 1998	
 *
 ****************************************************************************/
#define NONDEBUGARRAYS
//#undef NONDEBUGARRAYS

#ifdef NONDEBUGARRAYS
#pragma warning( disable : 4701 )	// Avoid possibly uninitialized warnings for
												// prePixelStep, preRowStep, preWidth, preHeight, cc and ss
void RImageLibrary::RCTransform(	RBitmapImage&	rSrcBitmap,
											RBitmapImage&	rDestBitmap,
											RBitmapImage&	rMaskBitmap,
											YAngle			rotationAngle,
											YScaleFactor	xScale,
											YScaleFactor	yScale,
											RIntSize*		tileRightOffset,
											RIntSize*		tileBelowOffset
											)
{
	uLONG			bitDepth = rSrcBitmap.GetBitDepth();
	long			srcWidth,  preWidth,  dstWidth;
	long			srcHeight, preHeight, dstHeight;
	uBYTE			*srcData, *preData,  *dstData, *maskData;
	uBYTE						 *preRow,   *dstRow,  *maskRow;
	long			srcRowStep, srcPixelStep;
	long			preRowStep, prePixelStep;
	long			dstBytesPerRow, dstBytesPerPixel;
	long			maskBytesPerRow;
	long			i, x, y;
	long			numerator, denominator, numeratorDelta;
	long			xSrc, xSrcRow;
	long			ySrc, ySrcRow;
	double		pi = atan2( 0.0, -1.0 );
	double		angle = rotationAngle;
	long			quadrant;
	long			*cx=0, *cy=0, *sx=0, *sy=0;
	long			*rowMin=0, *rowMax=0;
	long			*xIndexLut=0, *yIndexLut=0;
	long			Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;
	double		preLeft, preRight, preTop, preBottom;
	double		c, s;
	long			cc, ss;			// Used in tile offset prerotation. Only set to -1, 0, or 1.
	long			xLeft, xRight, xTop, xBottom;
	long			yLeft, yRight, yTop, yBottom;
	long			xTileRight, xTileBelow;
	long			yTileRight, yTileBelow;
	long			xTileRightOffset, xTileBelowOffset;
	long			yTileRightOffset, yTileBelowOffset;
	BOOLEAN		xScaleIsNegative=FALSE;
	int			leftMaskByte, rightMaskByte, leftMaskOffset, rightMaskOffset;		// Used only in building mask.
	const uBYTE uMask1BitClearLeft[8]  = {0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};
	const uBYTE	uMask1BitClearRight[8] = {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
	YScaleFactor	tempScale;


	srcWidth		 = rSrcBitmap.GetWidthInPixels();
	srcHeight	 = rSrcBitmap.GetHeightInPixels();
	srcData		 = ( uBYTE * ) RBitmapImage::GetImageData( rSrcBitmap.GetRawData() );
	srcRowStep	 = RBitmapImage::GetBytesPerRow( srcWidth, bitDepth );
	srcPixelStep = ( bitDepth == 8 ) ? 1 : 3;

	/*******************************   Rotation and Scale   *******************************
	 *
	 * Scaling by S in x and by T in y and then rotating by an angle moves the point
	 * (x,y) in the source image to the point (x',y') in the destination image:
	 *
    *			x' = c*S*x - s*T*y
	 *			y' = c*T*y + s*S*x
	 *
	 * where
	 *			s = sin(angle)
	 *			c = cos(angle)
	 *
	 * This routine assumes that the images are in a right-handed coordinate system,
	 * (+x=right, +y=up), and that positive angles indicate a counter-clockwise rotation.
	 * The routine does not do any translation. As a result, the pixel at the center of the
	 * source image ends up at the center of the destination image.
	 *
	 * The main rotation code only rotates images by up to ±45 degrees. For angles
	 * outside this range, we prerotate by 90, 180, or 270 degrees to put the remaining
	 * angle into the ±45 degree range. The prerotation by a multiple of 90 degress
	 * can be done with simple row and column copy operations. 
	 *
	 * In order to do this prerotation, we have to determine the angle's quadrant.
	 * Because the goal here is to perform rotations of ±45 degrees, we define quadrant
	 * in the following special way:
	 *
	 *
	 *
	 *         135 degrees     45 degrees
	 *               \           /
	 *                \   Q1    /
	 *                 \       /
	 *                  \     /
	 *                   \   /      Q0
	 *                    \ /
	 *          Q2         X--------------------> 0 degrees
	 *                    / \
	 *                   /   \                      +y
	 *                  /     \     Q0              ^
	 *                 /       \                    |
	 *                /    Q3   \                   |
	 *               /           \                  |
	 *         225 degrees    315 degrees           +--------> +x
	 *
	 *
	 *
	 *
	 * This routine has 8 steps:
	 *		1. Set up. Divide the angle into a prerotation by a multiple of 90 degrees
	 *       and a rotation angle that is between -45 degrees and 45 degrees. Adjust
	 *		   the src index parameters to account for prerotation. Calculate the image
	 *			width and height after prerotation.
	 *		2. Transform the corner points and compute the dst image bounds.
	 *		3. Allocate and initialize the various computation tables.
	 *		4. Determine the paths of the left edges of the resulting image,
	 *       using a Bresenham-style line drawing technique. These edge path
	 *       locations are stored in the rowMin[] array since they represent
	 *       the left side of the image on each scanline. Also, continue
	 *       the edge paths for one extra pixel to determine the starting location
	 *       of the next tile (in case the result is to be tiled with itself).
	 *		5. Determine the paths of the right edges of the resulting image,
	 *       The paths are built to mesh with a translated version of the left
	 *       edge paths for tiling. The right edge paths are stored in the rowMax[]
	 *       array since they represent the right side of the image on each scanline.
	 *		6. Fill in the destination image. For each scanline, loop from rowMin to
	 *       rowMax. The value of the pixel is looked up in the source image using
	 *       an inverse transform.
	 *		7. Build the mask image from rowMin and rowMax arrays.
	 *		8. Calculate and return the tile offsets.
	 */

	
	//										**************
	//										*   Step 1   *
	//										**************

	// Error checking
 	TpsAssert( bitDepth == 8 || bitDepth == 24, "rSrcBitmap not 8 or 24 bits deep." );
	TpsAssert( xScale != 0 && yScale != 0, "scale factor is 0." );


	 /* A vertical flip is indicated by yScale < 0. Note a vertical flip is equivalent
	 * to a horizontal flip and a rotation of 180 degrees. We take this approach so
	 * that the rest of the code only has to deal with horizontal flips, which simplifies
	 * the code a bit.
	 */
	if ( yScale < 0.0 ) {
		xScale = -xScale;
		yScale = -yScale;
		angle += pi;
	}


	/* A negative xScale parameter indicates a flip. The flip is a x-flip in the
	 * destination image, which is equivalent to doing an x-flip in the source image
	 * and and rotating by the negative of the angle. As an example, consider the
	 * following original (unscaled, unrotated, unflipped) image:
	 *
	 *				.....
	 *				.....
	 *				.....
	 *				fghij
	 *				abcde
	 *
	 * where
	 *
	 *		a-e = one row of pixels
	 *		f-j = the next row of pixels
	 *		... = other pixels
	 *
	 * This image rotated about 30 degrees looks like this:
	 *
	 *		         ..
	 *				 ....
	 *				.....
	 *		      ....ij
	 *		       fghde
	 *		       abc
	 *
	 *
	 * An x-flip of this rotated image looks like this:
	 *
	 *		        ..
	 *				  ....
	 *				  .....
	 *		       ji....
	 *		       edhgf
	 *		         cba
	 *
	 *
	 * This is equivalent to first changing the rotation of the image to -30 degrees,
	 * which looks like this:
	 *
	 *		        ..
	 *				  ....
	 *				  .....
	 *		       fg....
	 *		       abhij
	 *		         cde
	 *
	 * and then flipping the source rows: abcde -> edcba and fghij -> jihgf.
	 *
	 * We perform the source x-flip flip part of the calculation by adjusting the source
	 * access parameters so that they access the rows backwards. This means that this:
	 *
	 *		first pixel
	 *			|
	 *			V
	 *			abcde
	 *			----> direction of access
	 *
	 * becomes instead:
	 *
	 *		    first pixel
	 *			    |
	 *			    V
	 *			abcde
	 *			<---- direction of access
	 *
	 * This x-flip operation is performed as part of the prerotation. There are
	 * additional comments about how mangling the source access parameters under
	 * the prerotation section below.
	 */
	preData = srcData;
	if ( xScale < 0 ) {
		preData		     += ( srcWidth - 1 ) * srcPixelStep;
		srcPixelStep     = -srcPixelStep;
		angle				  = -angle;
		xScale			  = -xScale;
		xScaleIsNegative = TRUE;
	}


	/* Compute the amount of prerotation. This is nearest multiple of 90 degrees and is
	 * referred to here as the rotation quadrant.
	 */
	quadrant = ROUND( angle / ( pi / 2.0 ) );


	/* Calculate the residual angle after the prerotation. The resulting angle
	 * will be between -45 degrees and 45 degrees (-pi/4 and pi/4).
	 */
	angle -= ( pi / 2.0 ) * quadrant;


	/* Force the quadrant to be in the range 0-3.
	 */
	quadrant = quadrant % 4;
	if ( quadrant < 0 ) quadrant += 4;

 	/*	A pre-rotation of 90 degress or 270 degress interchanges the width and the
	 * height of the source image. For example, if the source image is 50 pixels
	 * high and 100 pixels wide, the pre-rotation produces an image that is 100
	 * pixels high and 50 pixels wide.
	 *
	 * Instead of doing the work of actually prerotating the image, which would
	 * take time and require a place to put the result, we can get the same effect
	 * simply by changing the parameters that are used to index into the source
	 * source. If we were to actually create a prerotated image, pixel (x,y) in that
	 * image would be accessed as follows:
	 *
	 *			prerotatedImageData[ y * bytesPerRow + x * bytesPerPixel ]
    *
	 * This pixel in the prerotated image corresponds to a pixel in the original
	 * source image. This pixel in the source image may be accessed by:
	 *		- setting up "preData" to point to the pixel in the source image that
	 *		  corresponds to pixel (x,y) in the prerotated image,
	 *		- setting up the index parameter "prePixelStep" to be the distance
	 *      in bytes between pixels in the source image that correspond to
	 *		  to adjacent pixels in x in the prerotated image, and
	 *		- setting up the index parameter "preRowStep" to be the distance
	 *      in bytes between pixels in the source image that correspond to
	 *		  to adjacent pixels in y in the prerotated image.
	 * In this way, the pixel (x,y) in the prerotated image may be obtained directly
	 * from the source as follows:
	 *
	 *			preData[ y * preRowStep + x * prePixelStep ]
	 *
	 * See detailed comments below in the description of each quadrant.
	 */
	switch ( quadrant ) {
	case 0:
		/* Quadrant 0: source indexing is "normal" (left-to-right rows in bottom-to-top order):
		 *
		 *			.....						.....
		 *			.....						.....
		 *	dst:	.....				src:	.....
		 *			fghij						fghij
		 *			abcde						abcde
		 */
		prePixelStep = srcPixelStep;
		preRowStep   = srcRowStep;
		preWidth		 = srcWidth;
		preHeight	 = srcHeight;
		break;

	case 1:
		/* Quadrant 1: Do the pre-rotation by reading top-to-bottom columns in
		 * left-to-right order:
		 *
		 *			.....						af...
		 *			.....						bg...
		 *	dst:	.....				src:	ch...
		 *			fghij						di...
		 *			abcde						ej...
		 */
		preData		+= ( srcHeight - 1 ) * srcRowStep;
		prePixelStep = -srcRowStep;
		preRowStep   = srcPixelStep;
		preWidth		 = srcHeight;
		preHeight	 = srcWidth;
		tempScale	 = xScale;
		xScale		 = yScale;
		yScale		 = tempScale;
		break;

	case 2:
		/* Quadrant 2: Do the pre-rotation by reading right-to-left rows in
		 * top-to-bottom order:
		 *
		 *			.....						edcba
		 *			.....						jihgf
		 *	dst:	.....				src:	.....
		 *			fghij						.....
		 *			abcde						.....
		 */
		preData		+= (srcHeight-1) * srcRowStep + (srcWidth-1) * srcPixelStep;
		prePixelStep = -srcPixelStep;
		preRowStep   = -srcRowStep;
		preWidth		 = srcWidth;
		preHeight	 = srcHeight;
		break;

	case 3:
		/* Quadrant 3: Do the pre-rotation by reading top-to-bottom columns in
		 * right-to-left order.
		 *
		 *			.....						...je
		 *			.....						...id
		 *	dst:	.....				src:	...hc
		 *	    	fghij				      ...gb
		 *			abcde						...fa
		 */
		preData		+= (srcWidth-1) * srcPixelStep;
		prePixelStep = srcRowStep;
		preRowStep   = -srcPixelStep;
		preWidth		 = srcHeight;
		preHeight	 = srcWidth;
		tempScale	 = xScale;
		xScale		 = yScale;
		yScale		 = tempScale;
		break;
	}

 	c  = cos( angle );
	s  = sin( angle );

	/* Force small angles to zero to avoid 1-pixel roundoff errors. "Small" means that
	 * the largest shift is less than half a pixel.
	 */
	if ( ABS( s * xScale * preWidth ) < 0.5 && ABS( s * xScale * preHeight ) < 0.5 &&
		  ABS( s * yScale * preWidth ) < 0.5 && ABS( s * yScale * preHeight ) < 0.5 ) {
		angle = 0;
		c = 1;
		s = 0;
	}

	//										**************
	//										*   Step 2   *
	//										**************

	/* Transform the corner points. In the prerotated and scaled source image,
	 * they look like this:
	 *
	 *          A-------B   <---- preTop
	 *          |       |
	 *          |   +   |
	 *          |       |
	 *          C-------D   <---- preBottom
	 *
	 *				^       ^
	 *          |       |
	 *      preLeft   preRight
	 *
	 *
	 * The center of the rectangle is at (0,0) and is indicated by "+".
	 *
	 * To get the corner points, we start by calculating the preLeft and preRight (the
	 * coordinates of the left and right edges of the prerotated and scaled source image),
	 * and preTop and preBottom.
	 *
	 * This is a bit tricky because the scaled pixels to mesh together in tiles. This
	 * means that we want to scale the outer edges of the end pixels (not their centers),
	 * but that we then want to come in a half pixel to the pixel centers of the scaled
	 * end pixels. That's because we use the pixel centers as the location of the pixel
	 * for calculating the rotation. In other words, the mapping looks like this:
	 *
	 * :
	 *                    +---+---+---+---+
	 *                    | a | b | c | d |   <-- original, unscaled source pixels
	 *                    +---+---+---+---+
	 *           THIS     | |             |
	 *    +---------------+ |             +---------------+
	 *    |                 |                             |
	 *    |                 |                             |
	 *    |                 |                             |
	 *    |       NOT THIS  |                             |
	 *    |     +-----------+                             |
	 *    |     |                                         |
	 *    |     |                                         |
	 *    V     V                                         V
	 *    +---+---+---+---+---+---+---+---+---+---+---+---+
	 *    | a | a | a | b | b | b | c | c | c | d | d | d |  <-- scaled source pixels
	 *    +---+---+---+---+---+---+---+---+---+---+---+---+
	 *      ^                                           ^
	 *      |                                           |
	 *   preLeft                                     preRight
	 *
	 * Note how this would mesh perfectly with pixels "efgh" on the right.
	 *
	 * One more point: the center of rotation is the midpoint of the image. If there
	 * are an even number of pixels in each row, we rotate around the midpoint of the
	 * middle two pixels. If there are an odd number of pixels in each row, we rotate
	 * around the center of the middle pixel.
	 *
	 * Example of even number of pixels (scaled source width = xScale * preWidth = 8):
	 *
	 *                   rotate around
	 *                    this point
	 *                       |
	 *                       V
	 *       +---+---+---+---+---+---+---+---+
	 *       |   |   |   |   |   |   |   |   |
	 *       +---+---+---+---+---+---+---+---+
	 *         ^                           ^
	 *         |                           |
	 *      preLeft                     preRight
	 *     (d = 3.5)                   (d = 3.5)
    *
	 * d = distance from center of rotation
	 *
	 *
	 *
	 * Example of odd number of pixels (scaled source width = xScale * preWidth = 9):
	 *
	 *                     rotate around
	 *                      this point
	 *                         |
	 *                         V
	 *       +---+---+---+---+---+---+---+---+---+
	 *       |   |   |   |   |   |   |   |   |   |
	 *       +---+---+---+---+---+---+---+---+---+
	 *         ^                               ^
	 *         |                               |
	 *      preLeft                         preRight
	 *     (d = 4.0)                       (d = 4.0)
    *
	 */

#if 0
	preLeft   = ROUND( xScale * -preWidth  / 2.0 ) + 0.5;
	preRight  = ROUND( xScale *  preWidth  / 2.0 ) - 0.5;
	preBottom = ROUND( yScale * -preHeight / 2.0 ) + 0.5;
	preTop    = ROUND( yScale *  preHeight / 2.0 ) - 0.5;
#else
	// GCB 5/21/98 - fix from Rob Cook for array access violation bug that
	// occured further down on rowMin when scaling factors were very small
	preLeft   = ( ROUND( xScale * -preWidth  ) / 2.0 ) + 0.5;
	preRight  = ( ROUND( xScale *  preWidth  ) / 2.0 ) - 0.5;
	preBottom = ( ROUND( yScale * -preHeight ) / 2.0 ) + 0.5;
	preTop    = ( ROUND( yScale *  preHeight ) / 2.0 ) - 0.5;

	if ( preLeft   > -0.5 ) preLeft   = -0.5;
	if ( preRight  <  0.5 ) preRight  =  0.5;
	if ( preBottom > -0.5 ) preBottom = -0.5;
	if ( preTop    <  0.5 ) preTop    =  0.5;
#endif

	/* Rotate the corner points A, B, and C. We want to force opposite edges to mesh
	 * perfectly for tiling, so we compute the fourth corner D from the other three,
	 * noting that D = B + (C-A).
	 */
	Ax = ROUND( c * preLeft   - s * preTop );
	Ay = ROUND( c * preTop    + s * preLeft );
	Bx = ROUND( c * preRight  - s * preTop );
	By = ROUND( c * preTop    + s * preRight );
	Cx = ROUND( c * preLeft   - s * preBottom );
	Cy = ROUND( c * preBottom + s * preLeft );
 
	Dx = Bx + Cx - Ax;
	Dy = By + Cy - Ay;

	/* Set the bounds of the destination image. We have to take into account
	 * the sign of the rotation angle,
    *
	 * When the image is rotated counter-clockwise (angle>0), the
	 * result looks like this:
	 *
	 *						 -B
	 *					---- |
	 *				A--      |
	 *          |    +   |
	 *           |       -D
	 *           |   ----
	 *            C--
	 *
	 * When it is rotated clockwise, the result looks like this:
	 *
	 *          A--
	 *          |   ----
	 *         |        -B
	 *         |    +    |
	 *         C--      |
	 *            ----  |
	 *                -D
	 *
	 */
	if ( angle >= 0.0 ) {
		xLeft   = Ax;
		yLeft   = Ay;
		xRight  = Dx;
		yRight  = Dy;
		xTop    = Bx;
		yTop    = By;
		xBottom = Cx;
		yBottom = Cy;
	} else {
		xLeft   = Cx;
		yLeft   = Cy;
		xRight  = Bx;
		yRight  = By;
		xTop    = Ax;
		yTop    = Ay;
		xBottom = Dx;
		yBottom = Dy;
	}

	dstWidth	 = xRight - xLeft   + 1;
	dstHeight = yTop   - yBottom + 1;

	dstBytesPerPixel = ( bitDepth == 8 ) ? 1 : 3;
	 
	//										**************
	//										*   Step 3   *
	//										**************

	/* Allocate the destination and mask images.
	 */
	try
		{
		rDestBitmap.Initialize(dstWidth, dstHeight, bitDepth);

		// The code needs to take into the account that the source
		// file may have been created with a greyscale palette.
		// In order to insure the source and destination bitmaps
		// have exactly the same color tables, let's copy it verbatim.
		// MCK	4/14/98
		if (bitDepth == 8)
		{
			void	*	pSourceColorTable = rSrcBitmap.GetColorData(rSrcBitmap.GetRawData());
			void	*	pDestColorTable   = rDestBitmap.GetColorData(rDestBitmap.GetRawData());
			int			nColorTableSize   = rDestBitmap.GetColorDataSize(8);

			memcpy( pDestColorTable, pSourceColorTable, nColorTableSize );
		}

		rMaskBitmap.Initialize(dstWidth, dstHeight, 1);
		}
	catch( ... )
		{
		throw;
		}

	/* Allocate the rowMin[] and rowMax[] arrays. These keep track of the left
	 * and right edges of the image at each scanline.
	 */
	try
		{
		rowMin = (long *) new char[dstHeight*sizeof(long)];
		rowMax = (long *) new char[dstHeight*sizeof(long)];
		}
	catch( ... )
		{
		throw;
		}

	
	/* Allocate and initialize the index lookup tables. These are a computational
	 * optimization to avoid having to do multiplications in an inner loop. They store
	 * the number of bytes to the next row or column in the prerotated source image.
	 */
	try
		{
		xIndexLut = (long *) new char[preWidth *sizeof(long)];
		yIndexLut = (long *) new char[preHeight*sizeof(long)];
		}
	catch( ... )
		{
		throw;
		}

	for ( x = 0; x < preWidth; x++ ) {
		xIndexLut[x] = x * prePixelStep;
	}

	for ( y = 0; y < preHeight; y++ ) {
		yIndexLut[y] = y * preRowStep;
	}

	/* Allocate and initialize the cosine and sine lookup tables. These are a
	 * computational optimization to avoid having to do trigonometric functions
	 * and multiplications in an inner loop. The inner loop contains an inverse
	 * transform from the dst image back to the source image, so the table is
	 * indexed by the dst pixel location. The scale factor is also included in
	 * the value. Note that inverse scale factor is used here because we are doing
	 * an inverse mapping. Also, we multiply by 256 and store the result as a fixed
	 * point number with 8 bits of fraction.
	 *
	 * We index these arrays from the left starting at 0. The distance from the
	 * center of each pixel to the center of rotation is the index of that pixel
	 * minus (dstWidth-1)/2. To illustrate this, let's look again at the center-
	 * of-rotation diagram:
	 *
	 *                             rotate around
	 *                               this point
	 *                                   |
	 *                                   |
	 *                                   V
	 *           0      1      2      3      4      5      6      7     <-- pixel index
	 *       +------+------+------+------+------+------+------+------+
	 *       |      |      |      |      |      |      |      |      |
	 *       |      |      |      |      |      |      |      |      |
	 *       |      |      |      |      |      |      |      |      |
	 *       +------+------+------+------+------+------+------+------+		distance from center
	 *         -3.5   -2.5   -1.5   -0.5   +0.5   +1.5   +2.5   +3.5    <-- of pixel to center
	 *           ^                                                ^         of rotation
	 *           |                                                |
	 *           |                                                |
	 *			center of                                       center of
	 *			left pixel                                      right pixel
	 *
	 *
	 * dstWidth = 8. so ( dstWidth-1)/2 = 3.5
	 * index for preLeft = 0
	 * distance from center of preLeft to center of rotation is 0 - 3.5 = -3.5
	 *
	 * index for preRight = 7
	 * distance from center of preRight to center of rotation is 7 - 3.5 = +3.5
	 */
	try
		{
		cx = (long *) new char[dstWidth*sizeof(long)];
		cy = (long *) new char[dstHeight*sizeof(long)];
		sx = (long *) new char[dstHeight*sizeof(long)];
		sy = (long *) new char[dstWidth*sizeof(long)];
		}
	catch( ... )
		{
		throw;
		}

	for ( x = 0; x < dstWidth; x++ ) {
		cx[x] = ROUND( 256.0 * c * ( x - ( dstWidth - 1.0 ) / 2.0 ) / xScale );
		sy[x] = ROUND( 256.0 * s * ( x - ( dstWidth - 1.0 ) / 2.0 ) / yScale );
	}

	for ( y = 0; y < dstHeight; y++ ) {
		cy[y] = ROUND( 256.0 * c * ( y - ( dstHeight - 1.0 ) / 2.0 ) / yScale );
		sx[y] = ROUND( 256.0 * s * ( y - ( dstHeight - 1.0 ) / 2.0 ) / xScale );
	}


	//										**************
	//										*   Step 4   *
	//										**************
	 
	/* Using a Bresenham algorithm, draw a line from the Left corner to the Top corner.
	 * The leftmost pixel on the edge on each scanline is stored in rowMin[] for
	 * that scanline. We step one more pixel beyond Top with the Bresenham, and call
	 * this point "TileRight", which is where pixel "Left" should be location in the
	 * next tile.
	 *
	 * For this section, we use:
	 *		L = left corner
	 *		R = right corner
	 *		T = top corner
	 *		B = bottom corner
	 *
	 *                --T
	 *            ----   |
	 *         L--       |
	 *          |    +    |
	 *          |       --R
	 *           |  ----
	 *				 B--
	 *
	 *
	 * If we draw a line from L to T, then for each scanline y along the way, the
	 * line crosses that scanline at x given by:
	 *
	 *				x-xL = (xT-xL) * (y-yL)/(yT-yL)
	 *
	 * But this crossing point corresponds to the midpoint of each desired run
	 * of pixels. If we use these points as the leftmost pixel on that scanline,
	 * we get this edge:
	 *
	 *                            T
	 *                     ------
	 *               ------
	 *         L-----
	 *
	 * when what we want is this edge:
	 *
	 *                        ---T
	 *                  ------
	 *            ------
	 *         L--
	 *
	 * To get this edge, we need to round the result so that the points Left and
	 * Right are in the middle of pixel runs:
	 *
	 *		x-xL = round( ( (y-yL)*(xT-xL) / ( yT-yL) )
	 *			  = floor( ( (y-yL)*(xT-xL) + (yT-yL)/2 ) / ( yT-yL) )
	 *
	 * where
	 *
	 *		xL = xLeft
	 *		yL = yLeft
	 *		xT = xTop
	 *		yT = yTop
	 *
	 * We can set this up as a Bresenham-type algorithm as follows:
	 *
	 *    x-xL = x(y) = numerator(y) / denominator
	 *                = ( numerator(yL) + y * numeratorStep ) / denominator
	 *
	 * where
	 *		denominator   = yT-yL
	 *		numeratorStep = xT-xL
	 *		numerator at y=yL is denominator/2
	 *
	 * The numerator and denominator can then be tracked separately in order
	 * to avoid having to do a division at every pixel.
	 */

	denominator = yTop - yLeft;
	numeratorDelta = xTop - xLeft;
	numerator = denominator / 2;
	if ( denominator == 0 ) {
		numeratorDelta = 0;
		numerator = -1;
	}
	x = 0;
	for ( y = yLeft; y <= yTop; y++ ) {
		rowMin[ y - yBottom ] = x;
		numerator += numeratorDelta;
		while ( numerator >= denominator ) {
			numerator -= denominator;
			x++;
		}
	}

	/* The location of the left corner of the next tile to the right can be determined
	 * by finding the location of the next pixel past Top in the row. There are three
	 * possbilities for this location, which are labeled in the diagram as 1, 2, and 3,
	 * with T = top = the top corner pixel.
	 *
	 *
	 *      +---+---+---+---+
	 *      |   |   |   |   |
	 *      +---+---+---+---+
	 *      |   | 1 | 2 |   |
	 *      +---+---+---+---+
	 *      |   | T | 3 |   |
	 *      +---+---+---+---+
	 *      |   |   |   |   |
	 *      +---+---+---+---+
	 *
	 * Remember that we are using the Bresenham to calculate not the next pixel location
	 * but rather the leftmost pixel in each row. After the y loop above exits, x is left
	 * set to the leftmost pixel in the next row. There are three cases for where this
	 * might be, which correspond to the three possible locations for the next tile to the
	 * right. In the diagram, "m" indicates the minimum or leftmost pixel in each row and
	 * "T" indicates the top pixel. Pixels marked with a "-" indicate other pixels in the row.
	 *
	 * Case 1: x = xTop
	 *
	 *      +---+---+---+---+
	 *      |   |   | x |   |
	 *      +---+---+---+---+
	 *      |   |   | T |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *
	 *		In case 1, the location of the first pixel of the next tile is simply "x".
	 *
	 *
	 * Case 2: x = xTop + 1
	 *
	 *      +---+---+---+---+
	 *      |   |   |   | x |
	 *      +---+---+---+---+
	 *      |   |   | T |   |
	 *      +---+---+---+---+
	 *      |   |   | m |   |
	 *      +---+---+---+---+
	 *      |   |   | m |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      | m |   |   |   |
	 *      +---+---+---+---+
	 *
	 *      +---+---+---+---+---+
	 *      |   |   |   |   |   |
	 *      +---+---+---+---+---+
	 *      |   |   |   |   | x |
	 *      +---+---+---+---+---+
	 *      |   |   |   | T | + |
	 *      +---+---+---+---+---+
	 *      |   | m | - |   |   |
	 *      +---+---+---+---+---+
	 *      | m |   |   |   |   |
	 *      +---+---+---+---+---+
	 *
	 *    In this second example of case 2, we don't really know whether the location
	 *    of the first pixel of the next tile should be "x" or the point marked "+".
	 *    But the choice is not critical, so we always use "x" in case 2.
	 *
	 *
	 * Case 3: x > xTop + 1
	 *
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   |   |   |   |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   |   |   |   |   |   | x |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   |   |   |   | T | + | - |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   | m | - | - |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      | m | - | - |   |   |   |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *
	 *    In case 3, we want the location not of x but of the next pixel in the top
	 *    scanline. This turns out to be the pixel marked "+" in the diagram.
	 *
	 */
	xTileRight = xTop;
	yTileRight = yTop;
	if ( x == xTop - xLeft ) {
		yTileRight++;
	} else if ( x == xTop - xLeft + 1 ) {
		xTileRight++;
		yTileRight++;
	} else {
		xTileRight++;
	}
	xTileRightOffset = xTileRight - xLeft;
	yTileRightOffset = yTileRight - yLeft;


	/* Draw the line from Left to Bottom, filling in the rowMin array.
	 */
	denominator = yLeft - yBottom;
	numeratorDelta = xBottom - xLeft;
	numerator = denominator / 2;
	if ( denominator == 0 ) {
		numeratorDelta = 0;
		numerator = -1;
	}
	x = 0;
	for ( y = yLeft; y >= yBottom; y-- ) {
		rowMin[ y - yBottom ] = x;
		numerator += numeratorDelta;
		while ( numerator >= denominator ) {
			numerator -= denominator;
			x++;
		}
	}

	/* Put the left corner of the next tile below at the location of the next
	 * pixel in the row. Again there are three cases (see comments above).
	 * B = the bottom corner pixel.
	 *
	 *      +---+---+---+---+
	 *      |   |   |   |   |
	 *      +---+---+---+---+
	 *      |   | B | 3 |   |
	 *      +---+---+---+---+
	 *      |   | 1 | 2 |   |
	 *      +---+---+---+---+
	 *      |   |   |   |   |
	 *      +---+---+---+---+
	 *
	 * The three cases are the exact mirror of the three cases described
	 * earlier for the tile to the right.
	 *
	 * Case 1: x = xBottom
	 *
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   |   | B |   |
	 *      +---+---+---+---+
	 *      |   |   | x |   |
	 *      +---+---+---+---+
	 *
	 *		In case 1, the location of the first pixel of the next tile is simply "x".
	 *
	 * Case 2: x = xBottom + 1
	 *
	 *      +---+---+---+---+
	 *      | m |   |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   |   | m |   |
	 *      +---+---+---+---+
	 *      |   |   | m |   |
	 *      +---+---+---+---+
	 *      |   |   | B |   |
	 *      +---+---+---+---+
	 *      |   |   |   | x |
	 *      +---+---+---+---+
	 *
	 *      +---+---+---+---+---+
	 *      | m |   |   |   |   |
	 *      +---+---+---+---+---+
	 *      |   | m | - |   |   |
	 *      +---+---+---+---+---+
	 *      |   |   |   | B | + |
	 *      +---+---+---+---+---+
	 *      |   |   |   |   | x |
	 *      +---+---+---+---+---+
	 *      |   |   |   |   |   |
	 *      +---+---+---+---+---+
	 *
	 *    In this second example of case 2, we don't really know whether the location
	 *    of the first pixel of the next tile should be "x" or the point marked "+".
	 *    But the choice is not critical, so we always use "x" in case 2.
	 *
	 *
	 * Case 3: x > xBottom + 1
	 *
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      | m |   |   |   |   |   |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   | - | - |   |   |   |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   | m | - | - |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   |   |   |   | B | + | - |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   |   |   |   |   |   | x |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *
	 *    In case 3, we want the location not of x but of the next pixel in the top
	 *    scanline. This turns out to be the pixel marked "+" in the diagram.
	 *
	 */
	xTileBelow = xBottom;
	yTileBelow = yBottom;
	if ( x == xBottom - xLeft ) {
		yTileBelow--;
	} else if ( x == xTop - xLeft + 1 ) {
		xTileBelow++;
		yTileBelow--;
	} else {
		xTileBelow++;
	}
	xTileBelowOffset = xTileBelow - xLeft;
	yTileBelowOffset = yTileBelow - yLeft;


	//										**************
	//										*   Step 5   *
	//										**************

	/* Draw the line from the Bottom corner to the Right corner. The rightmost
	 * pixel on the edge on each scanline is stored in rowMax[] for that scanline.
	 * 
	 * If the output of this routine is tiled, we want to ensure that this line
	 * fits perfectly with the line from the Left corner to the Top corner, which
	 * we drew earlier. So we draw this line by starting at TileBelow and using
	 * as template a translated copy of the line from the Left corner to the Top
	 * corner. The rowMax for each scanline is the pixel just to the left of the
	 * translated copy.
    *
	 * Let us look at each of the 3 cases (corresponding to the 3 possible
	 * locations of TileBelow described above):
	 *
	 * Case 1: xTileBelow = xBottom, yTileBelow = yBottom - 1
	 *
	 *               bbbR
	 *           bbbbtttt
	 *			Bbbbtttt
	 *       *ttt
	 *
	 *		In the diagram, B = Bottom corner, R = right corner, * = the point TileBelow,
	 *    bbb... = the desired line from Bottom to Right
	 *		and ttt...= a shifted copy of the line from Left to Top
	 *
	 * Case 2: xTileBelow = xBottom + 1, yTileBelow = yBottom - 1
	 *
	 *                bbR
	 *            bbbbtttt
	 *			Bbbbbtttt
	 *        *ttt
	 *
	 *
	 * Case 3: xTileBelow = xBottom + 1, yTileBelow = yBottom
	 *
	 *                bbR
	 *            bbbbtttt
	 *			 bbbbtttt
	 *       B*ttt
	 *
	 * All three of these cases work the same way: we just translate the top
	 * row by TileOffset. This gives the first pixel of the next tile on that
	 * scanline, so we subtract one and that gives us the last pixel of our tile.
	 *
	 */
	for ( y = yBottom; y < yRight; y++ ) {
		rowMax[ y - yBottom ] = rowMin[ y - yBottom - yTileBelowOffset ] + xTileBelowOffset - 1;
	}
	// We have to handle the last point separately because if yTileBelow>yTileBottom, then
	// then the row with the Right corner will correspond to a rowMin row that is
	// one above the Top corner and we would try to access rowMin[-1].
	rowMax[ yRight  - yBottom ] = xRight  - xLeft;

	/* Draw the line from the Top corner to the Right corner. The rightmost
	 * pixel on the edge on each scanline is stored in rowMax[] for that scanline.
	 *
	 * There are again 3 cases:
	 *
	 * Case 1: yTileRight = yTop + 1,  xTileRight = xTop
	 *
	 *                  *
	 *                  Tl
	 *                  rl
	 *                   rl
	 *                   rl
	 *                    rl
	 *                    R
	 *
	 *		In the diagram, * = the point tileRight, rrr... = the desired line from Top to Right
	 *		and lll...= a shifted copy of the line from Left to Bottom.
	 *
	 * Case 2: yTileRight = yTop + 1,  xTileRight = xTop + 1
	 *
	 *                   *
	 *                  Tl
	 *                   rl
	 *                   rl
	 *                    rl
	 *                    rl
	 *                     R
	 *
	 *
	 * Case 3: yTileRight = yTop, xTileRight = xTop + 1.
	 *
	 *                  T*
	 *                  rl
	 *                   rl
	 *                   rl
	 *                    rl
	 *                    Rl
	 *
	 *
	 */
	for ( y = yTop; y > yRight; y-- ) {
		rowMax[ y - yBottom ] = rowMin[ y - yBottom - yTileRightOffset ] + xTileRightOffset - 1;
	}


	//										**************
	//										*   Step 6   *
	//										**************

	/* Alright, already. Enough preliminaries. It's time to move some pixels!
	 */

	dstData = (uBYTE*)RBitmapImage::GetImageData( rDestBitmap.GetRawData() );
	dstBytesPerRow = RBitmapImage::GetBytesPerRow( dstWidth, bitDepth );

	// Clear all the pixels in the dst.
	memset(dstData, 0x00, RBitmapImage::GetImageDataSize(rDestBitmap.GetRawData()));

	/* For every pixel in the destination image, inverse map back to the source
	 * image and copy the corresponding pixel.
	 *
	 * We loop through every pixel in the destination image using the rowMin[]
	 * and rowMax[] arrays to tell us the first and last pixel in the destination
	 * for each scanline.
	 *
	 *
	 * A few words about the inverse transform (from dst to src):
	 *
	 * In principle, the inverse transform is a rotation by -angle
	 * and a scale by 1/xScale and 1/yScale:
	 *		xSrc = ( cos(-angle) * x - sin(-angle) * y ) / xScale
	 *		ySrc = ( sin(-angle) * x + cos(-angle) * y ) / yScale
	 *
	 * In practice:
	 *		- note that any prerotation and flipping is done automagically
	 *		  by setting up the source index parameters properly,
	 *		- we have to subtract from x and y the center of rotation in dst,
	 *		  This is included in the calculations of cx, cy, sx, and sy.
	 *		- we have to add to xSrc and ySrc the center of rotation (in src),
	 *      which is ( preWidth/2, preHeight/2 ).
	 *		- we have to clamp the result to the source image because some
	 *		  edge pixels may map to a location slightly outside the source.
	 *		  This is especially true if the scale factors are small, which
	 *		  makes small roundoff errors in the dst become large in the src.
	 *
	 * Furthermore, for computational efficiency:
	 *		- the y part of the calculations are done outside of the x loop.
	 *		- the trig scale values are premultiplied and stored in
	 *		  the cx. cy. sx. and sy tables. These are stored in a fixed point
	 *		  format with 8 bits of fraction, so we have to shift the final
	 *		  result by 8 bits.
	 *		- the values of preWidth/2 and preHeight/2 are shifted by
	 *		  8 bits (multipled by 256) when they are added to sx and cy to
	 *		  make them fixed point numbers.
	 *		- the offset into the source image should be
	 *				ySrc * preRowStep + xSrc * prePixelStep
	 *		  These multiplications are precomputed and stored in xIndexLut[]
	 *		  and yIndexLut[].
	 */
	for ( y = 0; y < dstHeight; y++ ) {
		RCursor().SpinCursor();

		long xMin = rowMin[y];
		long xMax = rowMax[y];
		dstRow = &dstData [ y * dstBytesPerRow + xMin * dstBytesPerPixel ];
		xSrcRow = preWidth  * 128 + sx[y];
		ySrcRow = preHeight * 128 + cy[y];
		for ( x = xMin; x <= xMax; x++ ) {
			xSrc = ( xSrcRow + cx[x] ) >> 8;
			ySrc = ( ySrcRow - sy[x] ) >> 8;
			if ( xSrc < 0 ) xSrc = 0;
			if ( ySrc < 0 ) ySrc = 0;
			if ( xSrc >= preWidth  ) xSrc = preWidth  - 1;
			if ( ySrc >= preHeight ) ySrc = preHeight - 1;
			preRow = &preData[ xIndexLut[ xSrc ] + yIndexLut[ ySrc ] ];
			for ( i = 0; i < dstBytesPerPixel; i++ ) {
				*dstRow++ = *preRow++;
			}
		}
	}


	//										**************
	//										*   Step 7   *
	//										**************

	/* Build the mask image.
	 *
	 * The mask is a one-bit image, so each byte in the mask represents 8 pixels.  This
	 * is illustrated below. Pixels inside the rotated image are denoted with "*", and
	 * pixels outside the rotated image are denoted with "-". The pixels are shown in
	 * their byte groups of 8.
	 *
	 *     -------- -------- --****** ******** *****--- --------
	 *
	 * The resulting mask bits are:
	 *     00000000 00000000 00111111 11111111 11111000 00000000
	 *
	 * and the corresponding mask byte values are:
	 *       0x00     0x00     0x3F     0xFF     0xF8     0x00
	 *
	 * There are 5 byte regions in each row:
	 *			1. bytes entirely outside the mask to the left
	 *			2. the byte containing the left edge pixel
	 *			3. bytes entirely inside the mask
	 *			4. the byte containing the right edge pixel
	 *			5. bytes entirely outside the mask to the right
	 * Note that the left and right edge pixels may be in the same byte, in which
	 * case the middle region is null.
	 *
	 * Initially all the pixels in the mask are set to "on"; we then clear the pixels
	 * that are outside the interior of the rotated image. We choose this approach
	 * (rather than clearing all the bits and then setting those inside) because there
	 * are usually more pixels inside the rotated image than outside of it.
	 */

	maskBytesPerRow = RBitmapImage::GetBytesPerRow( dstWidth, 1 );
	maskData = (uBYTE*)RBitmapImage::GetImageData( rMaskBitmap.GetRawData() );
	maskRow  = maskData;

	// Set all of the bits.
	memset( maskData, 0xFF, RBitmapImage::GetImageDataSize(rMaskBitmap.GetRawData()));

	for ( y = 0; y < dstHeight; y++ ) {
		RCursor().SpinCursor();

		// Determine the locations of the 5 regions.
		leftMaskByte    = rowMin[y] >> 3;
		rightMaskByte   = rowMax[y] >> 3;
		leftMaskOffset  = rowMin[y] - (  leftMaskByte << 3 );
		rightMaskOffset = rowMax[y] - ( rightMaskByte << 3 );

		// Region 1: Clear the bytes that are entirely left of the interior.
		for ( x = 0; x < leftMaskByte; x++ )
			maskRow[x] = 0;

		// Region 2: In the byte containing the left edge of the rotated image (rowMin[y]),
		//           clear the bits that are left of the interior.
		maskRow[leftMaskByte]  &= uMask1BitClearLeft[leftMaskOffset];

		// Region 3: Interior bits have already been set outside of the loop.

		// Region 4: In the byte containing the right edge of the rotated image (rowMax[y]),
		//           clear the bits that are right of the interior.
		maskRow[rightMaskByte] &= uMask1BitClearRight[rightMaskOffset];

		// Region 5: Clear the bytes that are entirely right of the interior.
		for ( x = rightMaskByte + 1; x < maskBytesPerRow; x++ )
			maskRow[x] = 0;

		maskRow += maskBytesPerRow;
	}

	//										**************
	//										*   Step 8   *
	//										**************

	/* Report the tile offsets. Up until this point in the code, the tile offset
	 * values have represented the offset to the next tiles right and below in
	 * the output space. We need to adjust these offsets to represent the destination
	 * offsets that correspond to the next tiles right and left in source space.
	 * This can be done by applying the original quadrant prerotation. Negative
	 * angles act like the previous quadrant, as illustrated below. A negative xScale
	 * causes the right tile offset to be negated.
	 *
	 * abcd... are pixel identifiers
	 * R = where the left corner pixel in dst ("L") goes in the next tile right in dst space
	 * B = where the left corner pixel in dst ("L") goes in the next tile below in dst space
	 * S = where the top left pixel in src ("a") goes in the next tile right in src space
	 * T = where the top left pixel in src ("a") goes in the next tile below in src space
	 *
	 *
	 * The original, unprerotated source looks like this:
	 *
	 *	      abcdeS
	 *	src:  fghij
	 *       .....
	 *       .....
	 *       .....
	 *       T
	 *
	 * What follows are the gory details for all 4 quadrants for angle>0 and xScale>0.
	 * This is tedious but useful for understanding what's going on and for debugging.
	 * In each case we show:
	 *			- a preroated source image, labeling S and T plus some of the pixel
	 *         locations (a-j) in the image
	 *			- one view of the destination image that shows the mapping of those
	 *			  pixels from the prerotated source
	 *			- another view of the destination image with the points L=left,
	 *			  R=tileRight, and B=tileBelow points indicated.
	 *
	 *******
	 *
	 *	Quadrant 0, angle>0, xScale>0:
	 *
	 *          ..R            deS
	 *			L.... 			abcij             abcdeS
	 *	dst:  ..... 	dst:  fgh..       pre:  fghij
	 *       ......         ......            .....
	 *        .....          .....            .....
	 *        ...            ...              .....
	 *        B              T                T
	 *
	 * note: S-a = R-L
	 *       T-a = B-L
	 *
	 *******
	 *
	 *	Quadrant 1, angle>0, xScale>0:
	 *
	 *          ..R         S  ..             S
	 *			L.... 			ej...             ej...
	 *	dst:  ..... 	dst:  di...       pre:  di...
	 *       ......         ch....            ch...
	 *        .....          bg...T           bg....
	 *        ...            af.              af....T
	 *        B              
	 *
	 * note: S-a = -(B-L)
	 *       T-a =   R-L
	 *
	 *******
	 *
	 *	Quadrant 2, angle>0, xScale>0:
	 *
	 *                          T
	 *          ..R            ..                 T
	 *			L.... 			.....             .....
	 *	dst:  ..... 	dst:  .....       pre:  .....
	 *       ......         ....gf            .....
	 *        .....          jihba            jihgf
	 *        ...           Sedc             Sedcba
	 *        B
	 *
	 * note: S-a = -(R-L)
	 *       T-a = -(B-L)
	 *
	 *
	 *******
	 *
	 *	Quadrant 3, angle>0, xScale>0:
	 *
	 *          ..R            fa
	 *			L....         T...gb            T...fa
	 *	dst:  ..... 	dst:  ...hc       pre:  ...gb
	 *       ......         ....id            ...hc
	 *        .....          ...je            ...id
	 *        ...            ... S            ...je
	 *        B                                   S
	 *
	 * note: S-a =   B-L
	 *       T-a = -(R-L)
	 *
	 *
	 *******
	 * Now let's look at two examples of angle < 0 to illustrate why
	 * it's the same case as the angle > 0 case for quadrant-1.
	 *******
	 *
	 *	Quadrant 0, angle<0, xScale>0:
	 *
	 *        R
	 *			... 			   abc               abcdeS
	 *	dst:  ..... 	dst:  fghdeS      pre:  fghij
	 *       .....          ...ij             .....
	 *      ......         ......             .....
	 *      L....          .....              .....
	 *        ...B        T  ...              T
	 *
	 * note: S-a =   B-L
	 *       T-a = -(R-L)
	 *			which is just like quadrant 3, angle>0
	 *
	 *******
	 *
	 *	Quadrant 1, angle<0, xScale>0:
	 *
	 *        R              S                S
	 *			... 			   ej.               ej...
	 *	dst:  ..... 	dst:  di...       pre:  di...
	 *       .....          ch...             ch...
	 *      ......         bg....             bg...
	 *      L....          af...              af...T
	 *        ...B           ...T
	 *
	 * note: S-a = R-L
	 *       T-a = B-L
	 *			which is just like quadrant 0, angle>0
	 *
	 *******
	 * Finally, let's look at an example of xScale<0 to illustrate why
	 * it's the same case as the xScale>0 case with the S offset negated.
	 *******
	 *
	 *
	 *	Quadrant 0, angle>0, xScale<0:
	 *
	 *          ..R            ba
	 *			L.... 		  Sedcgf            Sedcba
	 *	dst:  ..... 	dst:  jih..       pre:  jihgf
	 *       ......         ......            .....
	 *        .....          .....            .....
	 *        ...            ... T            .....
	 *        B                                   T
	 *
	 * note: S-a = -(R-L)
	 *       T-a =   B-L
	 *			which is just like quadrant 0, angle>0, xScale>0 except that S is negated.
	 *
	 */


	if ( angle < 0 ) {
		quadrant = ( quadrant + 3 ) % 4;
	}

	if ( quadrant == 0 ) {
		cc =  1;
		ss =  0;
	} else if ( quadrant == 1 ) {
		cc =  0;
		ss = -1;
	} else if ( quadrant == 2 ) {
		cc = -1;
		ss =  0;
	} else if ( quadrant == 3 ) {
		cc =  0;
		ss =  1;
	}

	if ( tileRightOffset ) {
		tileRightOffset->m_dx =  cc * xTileRightOffset + ss * xTileBelowOffset;
		tileRightOffset->m_dy =  cc * yTileRightOffset + ss * yTileBelowOffset;
		if ( xScaleIsNegative ) {
			tileRightOffset->m_dx = -tileRightOffset->m_dx;
			tileRightOffset->m_dy = -tileRightOffset->m_dy;
		}
	}
	
	if ( tileBelowOffset ) {
		tileBelowOffset->m_dx = -ss * xTileRightOffset + cc * xTileBelowOffset;
		tileBelowOffset->m_dy = -ss * yTileRightOffset + cc * yTileBelowOffset;
	}


	// Cleanup...
	if ( rowMin )
		delete[] rowMin;
	if ( rowMax )
		delete[] rowMax;
	if ( xIndexLut )
		delete[] xIndexLut;
	if ( yIndexLut )
		delete[] yIndexLut;
	if ( cx )
		delete[] cx;
	if ( cy )
		delete[] cy;
	if ( sx )
		delete[] sx;
	if ( sy )
		delete[] sy;

	return;
}
#pragma warning( default : 4701 )


#else


void RImageLibrary::RCTransform(	RBitmapImage&	rSrcBitmap,
											RBitmapImage&	rDestBitmap,
											RBitmapImage&	rMaskBitmap,
											YAngle			rotationAngle,
											YScaleFactor	xScale,
											YScaleFactor	yScale,
											RIntSize*		tileRightOffset,
											RIntSize*		tileBelowOffset
											)
{
	uLONG			bitDepth = rSrcBitmap.GetBitDepth();
	long			srcWidth,  preWidth,  dstWidth;
	long			srcHeight, preHeight, dstHeight;
	long			srcRowStep, srcPixelStep;
	long			preRowStep, prePixelStep;
	long			dstBytesPerRow, dstBytesPerPixel;
	long			maskBytesPerRow;
	long			i, x, y;
	long			numerator, denominator, numeratorDelta;
	long			xSrc, xSrcRow;
	long			ySrc, ySrcRow;
	double		pi = atan2( 0.0, -1.0 );
	double		angle = rotationAngle;
	long			quadrant;
	long			Ax, Ay, Bx, By, Cx, Cy, Dx, Dy;
	double		preLeft, preRight, preTop, preBottom;
	double		c, s;
	long			cc, ss;			// Used in tile offset prerotation. Only set to -1, 0, or 1.
	long			xLeft, xRight, xTop, xBottom;
	long			yLeft, yRight, yTop, yBottom;
	long			xTileRight, xTileBelow;
	long			yTileRight, yTileBelow;
	long			xTileRightOffset, xTileBelowOffset;
	long			yTileRightOffset, yTileBelowOffset;
	BOOLEAN		xScaleIsNegative=FALSE;
	int			leftMaskByte, rightMaskByte, leftMaskOffset, rightMaskOffset;		// Used only in building mask.
	const uBYTE uMask1BitClearLeft[8]  = {0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};
	const uBYTE	uMask1BitClearRight[8] = {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};
	YScaleFactor	tempScale;
	uBYTE			*srcData=0, *preData=0,  *dstData=0, *maskData=0;
	uBYTE			*preRow=0,   *dstRow=0,  *maskRow=0;
#ifdef NONDEBUGARRAYS
	long			*cx=0, *cy=0, *sx=0, *sy=0;
	long			*rowMin=0, *rowMax=0;
	long			*xIndexLut=0, *yIndexLut=0;
#else
	// Redefine the arrays as CArrays to detect out of range access.
	CArray<long,long>		cx, cy, sx, sy;
	CArray<long,long>		rowMin, rowMax;
	CArray<long,long>		xIndexLut, yIndexLut;
#endif


	srcWidth		 = rSrcBitmap.GetWidthInPixels();
	srcHeight	 = rSrcBitmap.GetHeightInPixels();
	srcData		 = ( uBYTE * ) RBitmapImage::GetImageData( rSrcBitmap.GetRawData() );
	srcRowStep	 = RBitmapImage::GetBytesPerRow( srcWidth, bitDepth );
	srcPixelStep = ( bitDepth == 8 ) ? 1 : 3;

	/*******************************   Rotation and Scale   *******************************
	 *
	 * Scaling by S in x and by T in y and then rotating by an angle moves the point
	 * (x,y) in the source image to the point (x',y') in the destination image:
	 *
    *			x' = c*S*x - s*T*y
	 *			y' = c*T*y + s*S*x
	 *
	 * where
	 *			s = sin(angle)
	 *			c = cos(angle)
	 *
	 * This routine assumes that the images are in a right-handed coordinate system,
	 * (+x=right, +y=up), and that positive angles indicate a counter-clockwise rotation.
	 * The routine does not do any translation. As a result, the pixel at the center of the
	 * source image ends up at the center of the destination image.
	 *
	 * The main rotation code only rotates images by up to ±45 degrees. For angles
	 * outside this range, we prerotate by 90, 180, or 270 degrees to put the remaining
	 * angle into the ±45 degree range. The prerotation by a multiple of 90 degress
	 * can be done with simple row and column copy operations. 
	 *
	 * In order to do this prerotation, we have to determine the angle's quadrant.
	 * Because the goal here is to perform rotations of ±45 degrees, we define quadrant
	 * in the following special way:
	 *
	 *
	 *
	 *         135 degrees     45 degrees
	 *               \           /
	 *                \   Q1    /
	 *                 \       /
	 *                  \     /
	 *                   \   /      Q0
	 *                    \ /
	 *          Q2         X--------------------> 0 degrees
	 *                    / \
	 *                   /   \                      +y
	 *                  /     \     Q0              ^
	 *                 /       \                    |
	 *                /    Q3   \                   |
	 *               /           \                  |
	 *         225 degrees    315 degrees           +--------> +x
	 *
	 *
	 *
	 *
	 * This routine has 8 steps:
	 *		1. Set up. Divide the angle into a prerotation by a multiple of 90 degrees
	 *       and a rotation angle that is between -45 degrees and 45 degrees. Adjust
	 *		   the src index parameters to account for prerotation. Calculate the image
	 *			width and height after prerotation.
	 *		2. Transform the corner points and compute the dst image bounds.
	 *		3. Allocate and initialize the various computation tables.
	 *		4. Determine the paths of the left edges of the resulting image,
	 *       using a Bresenham-style line drawing technique. These edge path
	 *       locations are stored in the rowMin[] array since they represent
	 *       the left side of the image on each scanline. Also, continue
	 *       the edge paths for one extra pixel to determine the starting location
	 *       of the next tile (in case the result is to be tiled with itself).
	 *		5. Determine the paths of the right edges of the resulting image,
	 *       The paths are built to mesh with a translated version of the left
	 *       edge paths for tiling. The right edge paths are stored in the rowMax[]
	 *       array since they represent the right side of the image on each scanline.
	 *		6. Fill in the destination image. For each scanline, loop from rowMin to
	 *       rowMax. The value of the pixel is looked up in the source image using
	 *       an inverse transform.
	 *		7. Build the mask image from rowMin and rowMax arrays.
	 *		8. Calculate and return the tile offsets.
	 */

	
	//										**************
	//										*   Step 1   *
	//										**************

	// Error checking
 	TpsAssert( bitDepth == 8 || bitDepth == 24, "rSrcBitmap not 8 or 24 bits deep." );
	TpsAssert( xScale != 0 && yScale != 0, "scale factor is 0." );


	 /* A vertical flip is indicated by yScale < 0. Note a vertical flip is equivalent
	 * to a horizontal flip and a rotation of 180 degrees. We take this approach so
	 * that the rest of the code only has to deal with horizontal flips, which simplifies
	 * the code a bit.
	 */
	if ( yScale < 0.0 ) {
		xScale = -xScale;
		yScale = -yScale;
		angle += pi;
	}


	/* A negative xScale parameter indicates a flip. The flip is a x-flip in the
	 * destination image, which is equivalent to doing an x-flip in the source image
	 * and and rotating by the negative of the angle. As an example, consider the
	 * following original (unscaled, unrotated, unflipped) image:
	 *
	 *				.....
	 *				.....
	 *				.....
	 *				fghij
	 *				abcde
	 *
	 * where
	 *
	 *		a-e = one row of pixels
	 *		f-j = the next row of pixels
	 *		... = other pixels
	 *
	 * This image rotated about 30 degrees looks like this:
	 *
	 *		         ..
	 *				 ....
	 *				.....
	 *		      ....ij
	 *		       fghde
	 *		       abc
	 *
	 *
	 * An x-flip of this rotated image looks like this:
	 *
	 *		        ..
	 *				  ....
	 *				  .....
	 *		       ji....
	 *		       edhgf
	 *		         cba
	 *
	 *
	 * This is equivalent to first changing the rotation of the image to -30 degrees,
	 * which looks like this:
	 *
	 *		        ..
	 *				  ....
	 *				  .....
	 *		       fg....
	 *		       abhij
	 *		         cde
	 *
	 * and then flipping the source rows: abcde -> edcba and fghij -> jihgf.
	 *
	 * We perform the source x-flip flip part of the calculation by adjusting the source
	 * access parameters so that they access the rows backwards. This means that this:
	 *
	 *		first pixel
	 *			|
	 *			V
	 *			abcde
	 *			----> direction of access
	 *
	 * becomes instead:
	 *
	 *		    first pixel
	 *			    |
	 *			    V
	 *			abcde
	 *			<---- direction of access
	 *
	 * This x-flip operation is performed as part of the prerotation. There are
	 * additional comments about how mangling the source access parameters under
	 * the prerotation section below.
	 */
	preData = srcData;
	if ( xScale < 0 ) {
		preData		     += ( srcWidth - 1 ) * srcPixelStep;
		srcPixelStep     = -srcPixelStep;
		angle				  = -angle;
		xScale			  = -xScale;
		xScaleIsNegative = TRUE;
	}


	/* Compute the amount of prerotation. This is nearest multiple of 90 degrees and is
	 * referred to here as the rotation quadrant.
	 */
	quadrant = ROUND( angle / ( pi / 2.0 ) );


	/* Calculate the residual angle after the prerotation. The resulting angle
	 * will be between -45 degrees and 45 degrees (-pi/4 and pi/4).
	 */
	angle -= ( pi / 2.0 ) * quadrant;


	/* Force the quadrant to be in the range 0-3.
	 */
	quadrant = quadrant % 4;
	if ( quadrant < 0 ) quadrant += 4;


 	/*	A pre-rotation of 90 degress or 270 degress interchanges the width and the
	 * height of the source image. For example, if the source image is 50 pixels
	 * high and 100 pixels wide, the pre-rotation produces an image that is 100
	 * pixels high and 50 pixels wide.
	 *
	 * Instead of doing the work of actually prerotating the image, which would
	 * take time and require a place to put the result, we can get the same effect
	 * simply by changing the parameters that are used to index into the source
	 * source. If we were to actually create a prerotated image, pixel (x,y) in that
	 * image would be accessed as follows:
	 *
	 *			prerotatedImageData[ y * bytesPerRow + x * bytesPerPixel ]
    *
	 * This pixel in the prerotated image corresponds to a pixel in the original
	 * source image. This pixel in the source image may be accessed by:
	 *		- setting up "preData" to point to the pixel in the source image that
	 *		  corresponds to pixel (x,y) in the prerotated image,
	 *		- setting up the index parameter "prePixelStep" to be the distance
	 *      in bytes between pixels in the source image that correspond to
	 *		  to adjacent pixels in x in the prerotated image, and
	 *		- setting up the index parameter "preRowStep" to be the distance
	 *      in bytes between pixels in the source image that correspond to
	 *		  to adjacent pixels in y in the prerotated image.
	 * In this way, the pixel (x,y) in the prerotated image may be obtained directly
	 * from the source as follows:
	 *
	 *			preData[ y * preRowStep + x * prePixelStep ]
	 *
	 * See detailed comments below in the description of each quadrant.
	 */
	switch ( quadrant ) {
	case 0:
		/* Quadrant 0: source indexing is "normal" (left-to-right rows in bottom-to-top order):
		 *
		 *			.....						.....
		 *			.....						.....
		 *	dst:	.....				src:	.....
		 *			fghij						fghij
		 *			abcde						abcde
		 */
		prePixelStep = srcPixelStep;
		preRowStep   = srcRowStep;
		preWidth		 = srcWidth;
		preHeight	 = srcHeight;
		break;

	case 1:
		/* Quadrant 1: Do the pre-rotation by reading top-to-bottom columns in
		 * left-to-right order:
		 *
		 *			.....						af...
		 *			.....						bg...
		 *	dst:	.....				src:	ch...
		 *			fghij						di...
		 *			abcde						ej...
		 */
		preData		+= ( srcHeight - 1 ) * srcRowStep;
		prePixelStep = -srcRowStep;
		preRowStep   = srcPixelStep;
		preWidth		 = srcHeight;
		preHeight	 = srcWidth;
		tempScale	 = xScale;
		xScale		 = yScale;
		yScale		 = tempScale;
		break;

	case 2:
		/* Quadrant 2: Do the pre-rotation by reading right-to-left rows in
		 * top-to-bottom order:
		 *
		 *			.....						edcba
		 *			.....						jihgf
		 *	dst:	.....				src:	.....
		 *			fghij						.....
		 *			abcde						.....
		 */
		preData		+= (srcHeight-1) * srcRowStep + (srcWidth-1) * srcPixelStep;
		prePixelStep = -srcPixelStep;
		preRowStep   = -srcRowStep;
		preWidth		 = srcWidth;
		preHeight	 = srcHeight;
		break;

	case 3:
		/* Quadrant 3: Do the pre-rotation by reading top-to-bottom columns in
		 * right-to-left order.
		 *
		 *			.....						...je
		 *			.....						...id
		 *	dst:	.....				src:	...hc
		 *	    	fghij				      ...gb
		 *			abcde						...fa
		 */
		preData		+= (srcWidth-1) * srcPixelStep;
		prePixelStep = srcRowStep;
		preRowStep   = -srcPixelStep;
		preWidth		 = srcHeight;
		preHeight	 = srcWidth;
		tempScale	 = xScale;
		xScale		 = yScale;
		yScale		 = tempScale;
		break;
	}


 	c  = cos( angle );
	s  = sin( angle );

	/* Force small angles to zero to avoid 1-pixel roundoff errors. "Small" means that
	 * the largest shift is less than half a pixel.
	 */
	if ( ABS( s * xScale * preWidth ) < 0.5 && ABS( s * xScale * preHeight ) < 0.5 &&
		  ABS( s * yScale * preWidth ) < 0.5 && ABS( s * yScale * preHeight ) < 0.5 ) {
		angle = 0;
		c = 1;
		s = 0;
	}

	//										**************
	//										*   Step 2   *
	//										**************

	/* Transform the corner points. In the prerotated and scaled source image,
	 * they look like this:
	 *
	 *          A-------B   <---- preTop
	 *          |       |
	 *          |   +   |
	 *          |       |
	 *          C-------D   <---- preBottom
	 *
	 *				^       ^
	 *          |       |
	 *      preLeft   preRight
	 *
	 *
	 * The center of the rectangle is at (0,0) and is indicated by "+".
	 *
	 * To get the corner points, we start by calculating the preLeft and preRight (the
	 * coordinates of the left and right edges of the prerotated and scaled source image),
	 * and preTop and preBottom.
	 *
	 * This is a bit tricky because the scaled pixels to mesh together in tiles. This
	 * means that we want to scale the outer edges of the end pixels (not their centers),
	 * but that we then want to come in a half pixel to the pixel centers of the scaled
	 * end pixels. That's because we use the pixel centers as the location of the pixel
	 * for calculating the rotation. In other words, the mapping looks like this:
	 *
	 * :
	 *                    +---+---+---+---+
	 *                    | a | b | c | d |   <-- original, unscaled source pixels
	 *                    +---+---+---+---+
	 *           THIS     | |             |
	 *    +---------------+ |             +---------------+
	 *    |                 |                             |
	 *    |                 |                             |
	 *    |                 |                             |
	 *    |       NOT THIS  |                             |
	 *    |     +-----------+                             |
	 *    |     |                                         |
	 *    |     |                                         |
	 *    V     V                                         V
	 *    +---+---+---+---+---+---+---+---+---+---+---+---+
	 *    | a | a | a | b | b | b | c | c | c | d | d | d |  <-- scaled source pixels
	 *    +---+---+---+---+---+---+---+---+---+---+---+---+
	 *      ^                                           ^
	 *      |                                           |
	 *   preLeft                                     preRight
	 *
	 * Note how this would mesh perfectly with pixels "efgh" on the right.
	 *
	 * One more point: the center of rotation is the midpoint of the image. If there
	 * are an even number of pixels in each row, we rotate around the midpoint of the
	 * middle two pixels. If there are an odd number of pixels in each row, we rotate
	 * around the center of the middle pixel.
	 *
	 * Example of even number of pixels (scaled source width = xScale * preWidth = 8):
	 *
	 *                   rotate around
	 *                    this point
	 *                       |
	 *                       V
	 *       +---+---+---+---+---+---+---+---+
	 *       |   |   |   |   |   |   |   |   |
	 *       +---+---+---+---+---+---+---+---+
	 *         ^                           ^
	 *         |                           |
	 *      preLeft                     preRight
	 *     (d = 3.5)                   (d = 3.5)
    *
	 * d = distance from center of rotation
	 *
	 *
	 *
	 * Example of odd number of pixels (scaled source width = xScale * preWidth = 9):
	 *
	 *                     rotate around
	 *                      this point
	 *                         |
	 *                         V
	 *       +---+---+---+---+---+---+---+---+---+
	 *       |   |   |   |   |   |   |   |   |   |
	 *       +---+---+---+---+---+---+---+---+---+
	 *         ^                               ^
	 *         |                               |
	 *      preLeft                         preRight
	 *     (d = 4.0)                       (d = 4.0)
    *
	 */

#if 0
	preLeft   = ROUND( xScale * -preWidth  / 2.0 ) + 0.5;
	preRight  = ROUND( xScale *  preWidth  / 2.0 ) - 0.5;
	preBottom = ROUND( yScale * -preHeight / 2.0 ) + 0.5;
	preTop    = ROUND( yScale *  preHeight / 2.0 ) - 0.5;
#else
	// GCB 5/21/98 - fix from Rob Cook for array access violation bug that
	// occured further down on rowMin when scaling factors were very small
	preLeft   = ( ROUND( xScale * -preWidth  ) / 2.0 ) + 0.5;
	preRight  = ( ROUND( xScale *  preWidth  ) / 2.0 ) - 0.5;
	preBottom = ( ROUND( yScale * -preHeight ) / 2.0 ) + 0.5;
	preTop    = ( ROUND( yScale *  preHeight ) / 2.0 ) - 0.5;

	if ( preLeft   > -0.5 ) preLeft   = -0.5;
	if ( preRight  <  0.5 ) preRight  =  0.5;
	if ( preBottom > -0.5 ) preBottom = -0.5;
	if ( preTop    <  0.5 ) preTop    =  0.5;
#endif

	/* Rotate the corner points A, B, and C. We want to force opposite edges to mesh
	 * perfectly for tiling, so we compute the fourth corner D from the other three,
	 * noting that D = B + (C-A).
	 */
	Ax = ROUND( c * preLeft   - s * preTop );
	Ay = ROUND( c * preTop    + s * preLeft );
	Bx = ROUND( c * preRight  - s * preTop );
	By = ROUND( c * preTop    + s * preRight );
	Cx = ROUND( c * preLeft   - s * preBottom );
	Cy = ROUND( c * preBottom + s * preLeft );
 
	Dx = Bx + Cx - Ax;
	Dy = By + Cy - Ay;

	/* Set the bounds of the destination image. We have to take into account
	 * the sign of the rotation angle,
    *
	 * When the image is rotated counter-clockwise (angle>0), the
	 * result looks like this:
	 *
	 *						 -B
	 *					---- |
	 *				A--      |
	 *          |    +   |
	 *           |       -D
	 *           |   ----
	 *            C--
	 *
	 * When it is rotated clockwise, the result looks like this:
	 *
	 *          A--
	 *          |   ----
	 *         |        -B
	 *         |    +    |
	 *         C--      |
	 *            ----  |
	 *                -D
	 *
	 */
	if ( angle >= 0.0 ) {
		xLeft   = Ax;
		yLeft   = Ay;
		xRight  = Dx;
		yRight  = Dy;
		xTop    = Bx;
		yTop    = By;
		xBottom = Cx;
		yBottom = Cy;
	} else {
		xLeft   = Cx;
		yLeft   = Cy;
		xRight  = Bx;
		yRight  = By;
		xTop    = Ax;
		yTop    = Ay;
		xBottom = Dx;
		yBottom = Dy;
	}

	dstWidth	 = xRight - xLeft   + 1;
	dstHeight = yTop   - yBottom + 1;

	dstBytesPerPixel = ( bitDepth == 8 ) ? 1 : 3;
	 
	//										**************
	//										*   Step 3   *
	//										**************

	/* Allocate the destination and mask images.
	 */
	rDestBitmap.Initialize(dstWidth, dstHeight, bitDepth);

	// The code needs to take into the account that the source
	// file may have been created with a greyscale palette.
	// In order to insure the source and destination bitmaps
	// have exactly the same color tables, let's copy it verbatim.
	// MCK	4/14/98
	if (bitDepth == 8)
	{
		void	*	pSourceColorTable = rSrcBitmap.GetColorData(rSrcBitmap.GetRawData());
		void	*	pDestColorTable   = rDestBitmap.GetColorData(rDestBitmap.GetRawData());
		int			nColorTableSize   = rDestBitmap.GetColorDataSize(8);

		memcpy( pDestColorTable, pSourceColorTable, nColorTableSize );
	}

	rMaskBitmap.Initialize(dstWidth, dstHeight, 1);

	/* Allocate the rowMin[] and rowMax[] arrays. These keep track of the left
	 * and right edges of the image at each scanline.
	 */
#ifdef NONDEBUGARRAYS
	rowMin = new long[dstHeight];
	rowMax = new long[dstHeight];
#else
	rowMin.SetSize( dstHeight );
	rowMax.SetSize( dstHeight );
#endif
	
	/* Allocate and initialize the index lookup tables. These are a computational
	 * optimization to avoid having to do multiplications in an inner loop. They store
	 * the number of bytes to the next row or column in the prerotated source image.
	 */
#ifdef NONDEBUGARRAYS
	xIndexLut = new long[preWidth];
	yIndexLut = new long[preHeight];
#else
	xIndexLut.SetSize( preWidth );
	yIndexLut.SetSize( preHeight );
#endif

	for ( x = 0; x < preWidth; x++ ) {
		xIndexLut[x] = x * prePixelStep;
	}

	for ( y = 0; y < preHeight; y++ ) {
		yIndexLut[y] = y * preRowStep;
	}

	/* Allocate and initialize the cosine and sine lookup tables. These are a
	 * computational optimization to avoid having to do trigonometric functions
	 * and multiplications in an inner loop. The inner loop contains an inverse
	 * transform from the dst image back to the source image, so the table is
	 * indexed by the dst pixel location. The scale factor is also included in
	 * the value. Note that inverse scale factor is used here because we are doing
	 * an inverse mapping. Also, we multiply by 256 and store the result as a fixed
	 * point number with 8 bits of fraction.
	 *
	 * We index these arrays from the left starting at 0. The distance from the
	 * center of each pixel to the center of rotation is the index of that pixel
	 * minus (dstWidth-1)/2. To illustrate this, let's look again at the center-
	 * of-rotation diagram:
	 *
	 *                             rotate around
	 *                               this point
	 *                                   |
	 *                                   |
	 *                                   V
	 *           0      1      2      3      4      5      6      7     <-- pixel index
	 *       +------+------+------+------+------+------+------+------+
	 *       |      |      |      |      |      |      |      |      |
	 *       |      |      |      |      |      |      |      |      |
	 *       |      |      |      |      |      |      |      |      |
	 *       +------+------+------+------+------+------+------+------+		distance from center
	 *         -3.5   -2.5   -1.5   -0.5   +0.5   +1.5   +2.5   +3.5    <-- of pixel to center
	 *           ^                                                ^         of rotation
	 *           |                                                |
	 *           |                                                |
	 *			center of                                       center of
	 *			left pixel                                      right pixel
	 *
	 *
	 * dstWidth = 8. so ( dstWidth-1)/2 = 3.5
	 * index for preLeft = 0
	 * distance from center of preLeft to center of rotation is 0 - 3.5 = -3.5
	 *
	 * index for preRight = 7
	 * distance from center of preRight to center of rotation is 7 - 3.5 = +3.5
	 */
#ifdef NONDEBUGARRAYS
	cx = new long[dstWidth];
	cy = new long[dstHeight];
	sx = new long[dstHeight];
	sy = new long[dstWidth];
#else
	cx.SetSize( dstWidth );
	cy.SetSize( dstHeight );
	sx.SetSize( dstHeight );
	sy.SetSize( dstWidth );
#endif

	for ( x = 0; x < dstWidth; x++ ) {
		cx[x] = ROUND( 256.0 * c * ( x - ( dstWidth - 1.0 ) / 2.0 ) / xScale );
		sy[x] = ROUND( 256.0 * s * ( x - ( dstWidth - 1.0 ) / 2.0 ) / yScale );
	}

	for ( y = 0; y < dstHeight; y++ ) {
		cy[y] = ROUND( 256.0 * c * ( y - ( dstHeight - 1.0 ) / 2.0 ) / yScale );
		sx[y] = ROUND( 256.0 * s * ( y - ( dstHeight - 1.0 ) / 2.0 ) / xScale );
	}


	//										**************
	//										*   Step 4   *
	//										**************
	 
	/* Using a Bresenham algorithm, draw a line from the Left corner to the Top corner.
	 * The leftmost pixel on the edge on each scanline is stored in rowMin[] for
	 * that scanline. We step one more pixel beyond Top with the Bresenham, and call
	 * this point "TileRight", which is where pixel "Left" should be location in the
	 * next tile.
	 *
	 * For this section, we use:
	 *		L = left corner
	 *		R = right corner
	 *		T = top corner
	 *		B = bottom corner
	 *
	 *                --T
	 *            ----   |
	 *         L--       |
	 *          |    +    |
	 *          |       --R
	 *           |  ----
	 *				 B--
	 *
	 *
	 * If we draw a line from L to T, then for each scanline y along the way, the
	 * line crosses that scanline at x given by:
	 *
	 *				x-xL = (xT-xL) * (y-yL)/(yT-yL)
	 *
	 * But this crossing point corresponds to the midpoint of each desired run
	 * of pixels. If we use these points as the leftmost pixel on that scanline,
	 * we get this edge:
	 *
	 *                            T
	 *                     ------
	 *               ------
	 *         L-----
	 *
	 * when what we want is this edge:
	 *
	 *                        ---T
	 *                  ------
	 *            ------
	 *         L--
	 *
	 * To get this edge, we need to round the result so that the points Left and
	 * Right are in the middle of pixel runs:
	 *
	 *		x-xL = round( ( (y-yL)*(xT-xL) / ( yT-yL) )
	 *			  = floor( ( (y-yL)*(xT-xL) + (yT-yL)/2 ) / ( yT-yL) )
	 *
	 * where
	 *
	 *		xL = xLeft
	 *		yL = yLeft
	 *		xT = xTop
	 *		yT = yTop
	 *
	 * We can set this up as a Bresenham-type algorithm as follows:
	 *
	 *    x-xL = x(y) = numerator(y) / denominator
	 *                = ( numerator(yL) + y * numeratorStep ) / denominator
	 *
	 * where
	 *		denominator   = yT-yL
	 *		numeratorStep = xT-xL
	 *		numerator at y=yL is denominator/2
	 *
	 * The numerator and denominator can then be tracked separately in order
	 * to avoid having to do a division at every pixel.
	 */

	denominator = yTop - yLeft;
	numeratorDelta = xTop - xLeft;
	numerator = denominator / 2;
	if ( denominator == 0 ) {
		numeratorDelta = 0;
		numerator = -1;
	}
	x = 0;
	for ( y = yLeft; y <= yTop; y++ ) {
		rowMin[ y - yBottom ] = x;
		numerator += numeratorDelta;
		while ( numerator >= denominator ) {
			numerator -= denominator;
			x++;
		}
	}

	/* The location of the left corner of the next tile to the right can be determined
	 * by finding the location of the next pixel past Top in the row. There are three
	 * possbilities for this location, which are labeled in the diagram as 1, 2, and 3,
	 * with T = top = the top corner pixel.
	 *
	 *
	 *      +---+---+---+---+
	 *      |   |   |   |   |
	 *      +---+---+---+---+
	 *      |   | 1 | 2 |   |
	 *      +---+---+---+---+
	 *      |   | T | 3 |   |
	 *      +---+---+---+---+
	 *      |   |   |   |   |
	 *      +---+---+---+---+
	 *
	 * Remember that we are using the Bresenham to calculate not the next pixel location
	 * but rather the leftmost pixel in each row. After the y loop above exits, x is left
	 * set to the leftmost pixel in the next row. There are three cases for where this
	 * might be, which correspond to the three possible locations for the next tile to the
	 * right. In the diagram, "m" indicates the minimum or leftmost pixel in each row and
	 * "T" indicates the top pixel. Pixels marked with a "-" indicate other pixels in the row.
	 *
	 * Case 1: x = xTop
	 *
	 *      +---+---+---+---+
	 *      |   |   | x |   |
	 *      +---+---+---+---+
	 *      |   |   | T |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *
	 *		In case 1, the location of the first pixel of the next tile is simply "x".
	 *
	 *
	 * Case 2: x = xTop + 1
	 *
	 *      +---+---+---+---+
	 *      |   |   |   | x |
	 *      +---+---+---+---+
	 *      |   |   | T |   |
	 *      +---+---+---+---+
	 *      |   |   | m |   |
	 *      +---+---+---+---+
	 *      |   |   | m |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      | m |   |   |   |
	 *      +---+---+---+---+
	 *
	 *      +---+---+---+---+---+
	 *      |   |   |   |   |   |
	 *      +---+---+---+---+---+
	 *      |   |   |   |   | x |
	 *      +---+---+---+---+---+
	 *      |   |   |   | T | + |
	 *      +---+---+---+---+---+
	 *      |   | m | - |   |   |
	 *      +---+---+---+---+---+
	 *      | m |   |   |   |   |
	 *      +---+---+---+---+---+
	 *
	 *    In this second example of case 2, we don't really know whether the location
	 *    of the first pixel of the next tile should be "x" or the point marked "+".
	 *    But the choice is not critical, so we always use "x" in case 2.
	 *
	 *
	 * Case 3: x > xTop + 1
	 *
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   |   |   |   |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   |   |   |   |   |   | x |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   |   |   |   | T | + | - |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   | m | - | - |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      | m | - | - |   |   |   |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *
	 *    In case 3, we want the location not of x but of the next pixel in the top
	 *    scanline. This turns out to be the pixel marked "+" in the diagram.
	 *
	 */
	xTileRight = xTop;
	yTileRight = yTop;
	if ( x == xTop - xLeft ) {
		yTileRight++;
	} else if ( x == xTop - xLeft + 1 ) {
		xTileRight++;
		yTileRight++;
	} else {
		xTileRight++;
	}
	xTileRightOffset = xTileRight - xLeft;
	yTileRightOffset = yTileRight - yLeft;


	/* Draw the line from Left to Bottom, filling in the rowMin array.
	 */
	denominator = yLeft - yBottom;
	numeratorDelta = xBottom - xLeft;
	numerator = denominator / 2;
	if ( denominator == 0 ) {
		numeratorDelta = 0;
		numerator = -1;
	}
	x = 0;
	for ( y = yLeft; y >= yBottom; y-- ) {
		rowMin[ y - yBottom ] = x;
		numerator += numeratorDelta;
		while ( numerator >= denominator ) {
			numerator -= denominator;
			x++;
		}
	}

	/* Put the left corner of the next tile below at the location of the next
	 * pixel in the row. Again there are three cases (see comments above).
	 * B = the bottom corner pixel.
	 *
	 *      +---+---+---+---+
	 *      |   |   |   |   |
	 *      +---+---+---+---+
	 *      |   | B | 3 |   |
	 *      +---+---+---+---+
	 *      |   | 1 | 2 |   |
	 *      +---+---+---+---+
	 *      |   |   |   |   |
	 *      +---+---+---+---+
	 *
	 * The three cases are the exact mirror of the three cases described
	 * earlier for the tile to the right.
	 *
	 * Case 1: x = xBottom
	 *
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   |   | B |   |
	 *      +---+---+---+---+
	 *      |   |   | x |   |
	 *      +---+---+---+---+
	 *
	 *		In case 1, the location of the first pixel of the next tile is simply "x".
	 *
	 * Case 2: x = xBottom + 1
	 *
	 *      +---+---+---+---+
	 *      | m |   |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   | m |   |   |
	 *      +---+---+---+---+
	 *      |   |   | m |   |
	 *      +---+---+---+---+
	 *      |   |   | m |   |
	 *      +---+---+---+---+
	 *      |   |   | B |   |
	 *      +---+---+---+---+
	 *      |   |   |   | x |
	 *      +---+---+---+---+
	 *
	 *      +---+---+---+---+---+
	 *      | m |   |   |   |   |
	 *      +---+---+---+---+---+
	 *      |   | m | - |   |   |
	 *      +---+---+---+---+---+
	 *      |   |   |   | B | + |
	 *      +---+---+---+---+---+
	 *      |   |   |   |   | x |
	 *      +---+---+---+---+---+
	 *      |   |   |   |   |   |
	 *      +---+---+---+---+---+
	 *
	 *    In this second example of case 2, we don't really know whether the location
	 *    of the first pixel of the next tile should be "x" or the point marked "+".
	 *    But the choice is not critical, so we always use "x" in case 2.
	 *
	 *
	 * Case 3: x > xBottom + 1
	 *
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      | m |   |   |   |   |   |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   | - | - |   |   |   |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   | m | - | - |   |   |   |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   |   |   |   | B | + | - |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *      |   |   |   |   |   |   |   |   | x |   |   |
	 *      +---+---+---+---+---+---+---+---+---+---+---+
	 *
	 *    In case 3, we want the location not of x but of the next pixel in the top
	 *    scanline. This turns out to be the pixel marked "+" in the diagram.
	 *
	 */
	xTileBelow = xBottom;
	yTileBelow = yBottom;
	if ( x == xBottom - xLeft ) {
		yTileBelow--;
	} else if ( x == xTop - xLeft + 1 ) {
		xTileBelow++;
		yTileBelow--;
	} else {
		xTileBelow++;
	}
	xTileBelowOffset = xTileBelow - xLeft;
	yTileBelowOffset = yTileBelow - yLeft;


	//										**************
	//										*   Step 5   *
	//										**************

	/* Draw the line from the Bottom corner to the Right corner. The rightmost
	 * pixel on the edge on each scanline is stored in rowMax[] for that scanline.
	 * 
	 * If the output of this routine is tiled, we want to ensure that this line
	 * fits perfectly with the line from the Left corner to the Top corner, which
	 * we drew earlier. So we draw this line by starting at TileBelow and using
	 * as template a translated copy of the line from the Left corner to the Top
	 * corner. The rowMax for each scanline is the pixel just to the left of the
	 * translated copy.
    *
	 * Let us look at each of the 3 cases (corresponding to the 3 possible
	 * locations of TileBelow described above):
	 *
	 * Case 1: xTileBelow = xBottom, yTileBelow = yBottom - 1
	 *
	 *               bbbR
	 *           bbbbtttt
	 *			Bbbbtttt
	 *       *ttt
	 *
	 *		In the diagram, B = Bottom corner, R = right corner, * = the point TileBelow,
	 *    bbb... = the desired line from Bottom to Right
	 *		and ttt...= a shifted copy of the line from Left to Top
	 *
	 * Case 2: xTileBelow = xBottom + 1, yTileBelow = yBottom - 1
	 *
	 *                bbR
	 *            bbbbtttt
	 *			Bbbbbtttt
	 *        *ttt
	 *
	 *
	 * Case 3: xTileBelow = xBottom + 1, yTileBelow = yBottom
	 *
	 *                bbR
	 *            bbbbtttt
	 *			 bbbbtttt
	 *       B*ttt
	 *
	 * All three of these cases work the same way: we just translate the top
	 * row by TileOffset. This gives the first pixel of the next tile on that
	 * scanline, so we subtract one and that gives us the last pixel of our tile.
	 *
	 */
	for ( y = yBottom; y < yRight; y++ ) {
		rowMax[ y - yBottom ] = rowMin[ y - yBottom - yTileBelowOffset ] + xTileBelowOffset - 1;
	}
	// We have to handle the last point separately because if yTileBelow>yTileBottom, then
	// then the row with the Right corner will correspond to a rowMin row that is
	// one above the Top corner and we would try to access rowMin[-1].
	rowMax[ yRight  - yBottom ] = xRight  - xLeft;

	/* Draw the line from the Top corner to the Right corner. The rightmost
	 * pixel on the edge on each scanline is stored in rowMax[] for that scanline.
	 *
	 * There are again 3 cases:
	 *
	 * Case 1: yTileRight = yTop + 1,  xTileRight = xTop
	 *
	 *                  *
	 *                  Tl
	 *                  rl
	 *                   rl
	 *                   rl
	 *                    rl
	 *                    R
	 *
	 *		In the diagram, * = the point tileRight, rrr... = the desired line from Top to Right
	 *		and lll...= a shifted copy of the line from Left to Bottom.
	 *
	 * Case 2: yTileRight = yTop + 1,  xTileRight = xTop + 1
	 *
	 *                   *
	 *                  Tl
	 *                   rl
	 *                   rl
	 *                    rl
	 *                    rl
	 *                     R
	 *
	 *
	 * Case 3: yTileRight = yTop, xTileRight = xTop + 1.
	 *
	 *                  T*
	 *                  rl
	 *                   rl
	 *                   rl
	 *                    rl
	 *                    Rl
	 *
	 *
	 */
	for ( y = yTop; y > yRight; y-- ) {
		rowMax[ y - yBottom ] = rowMin[ y - yBottom - yTileRightOffset ] + xTileRightOffset - 1;
	}


	//										**************
	//										*   Step 6   *
	//										**************

	/* Alright, already. Enough preliminaries. It's time to move some pixels!
	 */

	dstData = (uBYTE*)RBitmapImage::GetImageData( rDestBitmap.GetRawData() );
	dstBytesPerRow = RBitmapImage::GetBytesPerRow( dstWidth, bitDepth );

	// Clear all the pixels in the dst.
	memset(dstData, 0x00, RBitmapImage::GetImageDataSize(rDestBitmap.GetRawData()));

	/* For every pixel in the destination image, inverse map back to the source
	 * image and copy the corresponding pixel.
	 *
	 * We loop through every pixel in the destination image using the rowMin[]
	 * and rowMax[] arrays to tell us the first and last pixel in the destination
	 * for each scanline.
	 *
	 *
	 * A few words about the inverse transform (from dst to src):
	 *
	 * In principle, the inverse transform is a rotation by -angle
	 * and a scale by 1/xScale and 1/yScale:
	 *		xSrc = ( cos(-angle) * x - sin(-angle) * y ) / xScale
	 *		ySrc = ( sin(-angle) * x + cos(-angle) * y ) / yScale
	 *
	 * In practice:
	 *		- note that any prerotation and flipping is done automagically
	 *		  by setting up the source index parameters properly,
	 *		- we have to subtract from x and y the center of rotation in dst,
	 *		  This is included in the calculations of cx, cy, sx, and sy.
	 *		- we have to add to xSrc and ySrc the center of rotation (in src),
	 *      which is ( preWidth/2, preHeight/2 ).
	 *		- we have to clamp the result to the source image because some
	 *		  edge pixels may map to a location slightly outside the source.
	 *		  This is especially true if the scale factors are small, which
	 *		  makes small roundoff errors in the dst become large in the src.
	 *
	 * Furthermore, for computational efficiency:
	 *		- the y part of the calculations are done outside of the x loop.
	 *		- the trig scale values are premultiplied and stored in
	 *		  the cx. cy. sx. and sy tables. These are stored in a fixed point
	 *		  format with 8 bits of fraction, so we have to shift the final
	 *		  result by 8 bits.
	 *		- the values of preWidth/2 and preHeight/2 are shifted by
	 *		  8 bits (multipled by 256) when they are added to sx and cy to
	 *		  make them fixed point numbers.
	 *		- the offset into the source image should be
	 *				ySrc * preRowStep + xSrc * prePixelStep
	 *		  These multiplications are precomputed and stored in xIndexLut[]
	 *		  and yIndexLut[].
	 */
	for ( y = 0; y < dstHeight; y++ ) {
		RCursor().SpinCursor();

		long xMin = rowMin[y];
		long xMax = rowMax[y];
		dstRow = &dstData [ y * dstBytesPerRow + xMin * dstBytesPerPixel ];
		xSrcRow = preWidth  * 128 + sx[y];
		ySrcRow = preHeight * 128 + cy[y];
		for ( x = xMin; x <= xMax; x++ ) {
			xSrc = ( xSrcRow + cx[x] ) >> 8;
			ySrc = ( ySrcRow - sy[x] ) >> 8;
			if ( xSrc < 0 ) xSrc = 0;
			if ( ySrc < 0 ) ySrc = 0;
			if ( xSrc >= preWidth  ) xSrc = preWidth  - 1;
			if ( ySrc >= preHeight ) ySrc = preHeight - 1;
			preRow = &preData[ xIndexLut[ xSrc ] + yIndexLut[ ySrc ] ];
			for ( i = 0; i < dstBytesPerPixel; i++ ) {
				*dstRow++ = *preRow++;
			}
		}
	}


	//										**************
	//										*   Step 7   *
	//										**************

	/* Build the mask image.
	 *
	 * The mask is a one-bit image, so each byte in the mask represents 8 pixels.  This
	 * is illustrated below. Pixels inside the rotated image are denoted with "*", and
	 * pixels outside the rotated image are denoted with "-". The pixels are shown in
	 * their byte groups of 8.
	 *
	 *     -------- -------- --****** ******** *****--- --------
	 *
	 * The resulting mask bits are:
	 *     00000000 00000000 00111111 11111111 11111000 00000000
	 *
	 * and the corresponding mask byte values are:
	 *       0x00     0x00     0x3F     0xFF     0xF8     0x00
	 *
	 * There are 5 byte regions in each row:
	 *			1. bytes entirely outside the mask to the left
	 *			2. the byte containing the left edge pixel
	 *			3. bytes entirely inside the mask
	 *			4. the byte containing the right edge pixel
	 *			5. bytes entirely outside the mask to the right
	 * Note that the left and right edge pixels may be in the same byte, in which
	 * case the middle region is null.
	 *
	 * Initially all the pixels in the mask are set to "on"; we then clear the pixels
	 * that are outside the interior of the rotated image. We choose this approach
	 * (rather than clearing all the bits and then setting those inside) because there
	 * are usually more pixels inside the rotated image than outside of it.
	 */

	maskBytesPerRow = RBitmapImage::GetBytesPerRow( dstWidth, 1 );
	maskData = (uBYTE*)RBitmapImage::GetImageData( rMaskBitmap.GetRawData() );
	maskRow  = maskData;

	// Set all of the bits.
	memset( maskData, 0xFF, RBitmapImage::GetImageDataSize(rMaskBitmap.GetRawData()));

	for ( y = 0; y < dstHeight; y++ ) {
		RCursor().SpinCursor();

		// Determine the locations of the 5 regions.
		leftMaskByte    = rowMin[y] >> 3;
		rightMaskByte   = rowMax[y] >> 3;
		leftMaskOffset  = rowMin[y] - (  leftMaskByte << 3 );
		rightMaskOffset = rowMax[y] - ( rightMaskByte << 3 );

		// Region 1: Clear the bytes that are entirely left of the interior.
		for ( x = 0; x < leftMaskByte; x++ )
			maskRow[x] = 0;

		// Region 2: In the byte containing the left edge of the rotated image (rowMin[y]),
		//           clear the bits that are left of the interior.
		maskRow[leftMaskByte]  &= uMask1BitClearLeft[leftMaskOffset];

		// Region 3: Interior bits have already been set outside of the loop.

		// Region 4: In the byte containing the right edge of the rotated image (rowMax[y]),
		//           clear the bits that are right of the interior.
		maskRow[rightMaskByte] &= uMask1BitClearRight[rightMaskOffset];

		// Region 5: Clear the bytes that are entirely right of the interior.
		for ( x = rightMaskByte + 1; x < maskBytesPerRow; x++ )
			maskRow[x] = 0;

		maskRow += maskBytesPerRow;
	}

	//										**************
	//										*   Step 8   *
	//										**************

	/* Report the tile offsets. Up until this point in the code, the tile offset
	 * values have represented the offset to the next tiles right and below in
	 * the output space. We need to adjust these offsets to represent the destination
	 * offsets that correspond to the next tiles right and left in source space.
	 * This can be done by applying the original quadrant prerotation. Negative
	 * angles act like the previous quadrant, as illustrated below. A negative xScale
	 * causes the right tile offset to be negated.
	 *
	 * abcd... are pixel identifiers
	 * R = where the left corner pixel in dst ("L") goes in the next tile right in dst space
	 * B = where the left corner pixel in dst ("L") goes in the next tile below in dst space
	 * S = where the top left pixel in src ("a") goes in the next tile right in src space
	 * T = where the top left pixel in src ("a") goes in the next tile below in src space
	 *
	 *
	 * The original, unprerotated source looks like this:
	 *
	 *	      abcdeS
	 *	src:  fghij
	 *       .....
	 *       .....
	 *       .....
	 *       T
	 *
	 * What follows are the gory details for all 4 quadrants for angle>0 and xScale>0.
	 * This is tedious but useful for understanding what's going on and for debugging.
	 * In each case we show:
	 *			- a preroated source image, labeling S and T plus some of the pixel
	 *         locations (a-j) in the image
	 *			- one view of the destination image that shows the mapping of those
	 *			  pixels from the prerotated source
	 *			- another view of the destination image with the points L=left,
	 *			  R=tileRight, and B=tileBelow points indicated.
	 *
	 *******
	 *
	 *	Quadrant 0, angle>0, xScale>0:
	 *
	 *          ..R            deS
	 *			L.... 			abcij             abcdeS
	 *	dst:  ..... 	dst:  fgh..       pre:  fghij
	 *       ......         ......            .....
	 *        .....          .....            .....
	 *        ...            ...              .....
	 *        B              T                T
	 *
	 * note: S-a = R-L
	 *       T-a = B-L
	 *
	 *******
	 *
	 *	Quadrant 1, angle>0, xScale>0:
	 *
	 *          ..R         S  ..             S
	 *			L.... 			ej...             ej...
	 *	dst:  ..... 	dst:  di...       pre:  di...
	 *       ......         ch....            ch...
	 *        .....          bg...T           bg....
	 *        ...            af.              af....T
	 *        B              
	 *
	 * note: S-a = -(B-L)
	 *       T-a =   R-L
	 *
	 *******
	 *
	 *	Quadrant 2, angle>0, xScale>0:
	 *
	 *                          T
	 *          ..R            ..                 T
	 *			L.... 			.....             .....
	 *	dst:  ..... 	dst:  .....       pre:  .....
	 *       ......         ....gf            .....
	 *        .....          jihba            jihgf
	 *        ...           Sedc             Sedcba
	 *        B
	 *
	 * note: S-a = -(R-L)
	 *       T-a = -(B-L)
	 *
	 *
	 *******
	 *
	 *	Quadrant 3, angle>0, xScale>0:
	 *
	 *          ..R            fa
	 *			L....         T...gb            T...fa
	 *	dst:  ..... 	dst:  ...hc       pre:  ...gb
	 *       ......         ....id            ...hc
	 *        .....          ...je            ...id
	 *        ...            ... S            ...je
	 *        B                                   S
	 *
	 * note: S-a =   B-L
	 *       T-a = -(R-L)
	 *
	 *
	 *******
	 * Now let's look at two examples of angle < 0 to illustrate why
	 * it's the same case as the angle > 0 case for quadrant-1.
	 *******
	 *
	 *	Quadrant 0, angle<0, xScale>0:
	 *
	 *        R
	 *			... 			   abc               abcdeS
	 *	dst:  ..... 	dst:  fghdeS      pre:  fghij
	 *       .....          ...ij             .....
	 *      ......         ......             .....
	 *      L....          .....              .....
	 *        ...B        T  ...              T
	 *
	 * note: S-a =   B-L
	 *       T-a = -(R-L)
	 *			which is just like quadrant 3, angle>0
	 *
	 *******
	 *
	 *	Quadrant 1, angle<0, xScale>0:
	 *
	 *        R              S                S
	 *			... 			   ej.               ej...
	 *	dst:  ..... 	dst:  di...       pre:  di...
	 *       .....          ch...             ch...
	 *      ......         bg....             bg...
	 *      L....          af...              af...T
	 *        ...B           ...T
	 *
	 * note: S-a = R-L
	 *       T-a = B-L
	 *			which is just like quadrant 0, angle>0
	 *
	 *******
	 * Finally, let's look at an example of xScale<0 to illustrate why
	 * it's the same case as the xScale>0 case with the S offset negated.
	 *******
	 *
	 *
	 *	Quadrant 0, angle>0, xScale<0:
	 *
	 *          ..R            ba
	 *			L.... 		  Sedcgf            Sedcba
	 *	dst:  ..... 	dst:  jih..       pre:  jihgf
	 *       ......         ......            .....
	 *        .....          .....            .....
	 *        ...            ... T            .....
	 *        B                                   T
	 *
	 * note: S-a = -(R-L)
	 *       T-a =   B-L
	 *			which is just like quadrant 0, angle>0, xScale>0 except that S is negated.
	 *
	 */


	if ( angle < 0 ) {
		quadrant = ( quadrant + 3 ) % 4;
	}

	if ( quadrant == 0 ) {
		cc =  1;
		ss =  0;
	} else if ( quadrant == 1 ) {
		cc =  0;
		ss = -1;
	} else if ( quadrant == 2 ) {
		cc = -1;
		ss =  0;
	} else if ( quadrant == 3 ) {
		cc =  0;
		ss =  1;
	}

	if ( tileRightOffset ) {
		tileRightOffset->m_dx =  cc * xTileRightOffset + ss * xTileBelowOffset;
		tileRightOffset->m_dy =  cc * yTileRightOffset + ss * yTileBelowOffset;
		if ( xScaleIsNegative ) {
			tileRightOffset->m_dx = -tileRightOffset->m_dx;
			tileRightOffset->m_dy = -tileRightOffset->m_dy;
		}
	}
	
	if ( tileBelowOffset ) {
		tileBelowOffset->m_dx = -ss * xTileRightOffset + cc * xTileBelowOffset;
		tileBelowOffset->m_dy = -ss * yTileRightOffset + cc * yTileBelowOffset;
	}


#ifdef NONDEBUGARRAYS
	// Cleanup...
	delete[] rowMin;
	delete[] rowMax;
	delete[] xIndexLut;
	delete[] yIndexLut;
	delete[] cx;
	delete[] cy;
	delete[] sx;
	delete[] sy;
#endif
}
#endif

// GCB 3/17/98 - the 3 alpha blend methods:

// the LOOKUP TABLE method:
// uses a 64K table and is as fast as the bitshift method but
// is the most accurate (same accuracy as slower divide method)
#define LOOKUP_TABLE_ALPHA_BLEND_METHOD	0

// the SHIFT method:
// right-shift 8 does a divide by 256 instead of by 255
// which is much faster than real divide (but same speed as lookup table)
// and the error of roughly 1 part in 256 only occurs in the
// semi-transparent areas where it's somewhat difficult to detect
#define SHIFT_ALPHA_BLEND_METHOD				1

// the DIVIDE method:
// much slower than bitshift but the most accurate
// (but same accuracy as faster lookup table method)
#define DIVIDE_ALPHA_BLEND_METHOD			2

// for comparitive timings, see the comments below in the function:
//void RImageLibrary::AlphaBlend(
//	const RSolidColor&		solidColor,
//	RBitmapImage&				rDstBitmap,		const RIntRect&	dstRect,
//	const RBitmapImage&		rAlphaBitmap,	const RIntRect&	alphaRect ) const


#define ALPHA_BLEND_METHOD		LOOKUP_TABLE_ALPHA_BLEND_METHOD
//#define ALPHA_BLEND_METHOD		SHIFT_ALPHA_BLEND_METHOD
//#define ALPHA_BLEND_METHOD		DIVIDE_ALPHA_BLEND_METHOD


#if ALPHA_BLEND_METHOD == LOOKUP_TABLE_ALPHA_BLEND_METHOD

static const uBYTE alphalut[256][256] =
{
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 29,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 30,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 31,
  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 32,
  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 33,
  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 34,
  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34, 35,
  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 36,
  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37,
  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 38,
  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 39,
  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 40,
  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 41,
  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 42,
  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43,
  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44,
  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45,
  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46,
  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47,
  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 48,
  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49,
  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 50,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 51,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 51, 51, 51, 51, 52,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 52, 52, 52, 52, 53,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 53, 53, 53, 53, 54,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 53, 54, 54, 54, 54, 55,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 54, 55, 55, 55, 55, 56,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 56, 57,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 59, 60,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 61, 62,
  0,  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 62, 63,
  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 64,
  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 63, 64, 64, 64, 65,
  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 66,
  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 65, 66, 66, 66, 67,
  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 65, 66, 66, 66, 66, 67, 67, 67, 68,
  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 63, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 66, 66, 66, 66, 67, 67, 67, 67, 68, 68, 68, 69,
  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 63, 64, 64, 64, 65, 65, 65, 65, 66, 66, 66, 66, 67, 67, 67, 68, 68, 68, 68, 69, 69, 69, 70,
  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 23, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 65, 66, 66, 66, 67, 67, 67, 67, 68, 68, 68, 69, 69, 69, 69, 70, 70, 70, 71,
  0,  0,  0,  0,  1,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 66, 66, 66, 66, 67, 67, 67, 68, 68, 68, 68, 69, 69, 69, 70, 70, 70, 70, 71, 71, 71, 72,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 34, 34, 34, 34, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 46, 46, 46, 46, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 66, 66, 66, 66, 67, 67, 67, 68, 68, 68, 68, 69, 69, 69, 70, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 31, 31, 31, 31, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 49, 49, 49, 49, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 65, 66, 66, 66, 67, 67, 67, 67, 68, 68, 68, 69, 69, 69, 69, 70, 70, 70, 71, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 65, 66, 66, 66, 67, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 70, 71, 71, 71, 72, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83,
  0,  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 83, 84,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 85,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 86,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 87,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 88,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 87, 88, 88, 89,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 87, 88, 88, 88, 89, 89, 90,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 86, 86, 86, 87, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 73, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 90, 91, 91, 92,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 31, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 55, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 86, 86, 86, 87, 87, 87, 88, 88, 88, 89, 89, 90, 90, 90, 91, 91, 91, 92, 92, 93,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 75, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 94,
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6,  7,  7,  7,  8,  8,  8,  9,  9, 10, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 42, 42, 42, 43, 43, 43, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 51, 51, 51, 52, 52, 52, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 61, 61, 61, 62, 62, 62, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 78, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 86, 86, 86, 87, 87, 87, 88, 88, 89, 89, 89, 90, 90, 90, 91, 91, 92, 92, 92, 93, 93, 93, 94, 94, 95,
  0,  0,  0,  1,  1,  1,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 21, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 33, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 44, 44, 44, 45, 45, 45, 46, 46, 47, 47, 47, 48, 48, 48, 49, 49, 50, 50, 50, 51, 51, 51, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 57, 57, 57, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 62, 62, 62, 63, 63, 64, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 68, 69, 69, 70, 70, 70, 71, 71, 71, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 79, 79, 79, 80, 80, 80, 81, 81, 82, 82, 82, 83, 83, 83, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96,
  0,  0,  0,  1,  1,  1,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  9,  9,  9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 54, 55, 55, 55, 56, 56, 57, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 65, 65, 65, 66, 66, 66, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 76, 76, 76, 77, 77, 77, 78, 78, 79, 79, 79, 80, 80, 81, 81, 81, 82, 82, 82, 83, 83, 84, 84, 84, 85, 85, 85, 86, 86, 87, 87, 87, 88, 88, 89, 89, 89, 90, 90, 90, 91, 91, 92, 92, 92, 93, 93, 93, 94, 94, 95, 95, 95, 96, 96, 97,
  0,  0,  0,  1,  1,  1,  2,  2,  3,  3,  3,  4,  4,  4,  5,  5,  6,  6,  6,  7,  7,  8,  8,  8,  9,  9,  9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 31, 31, 31, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 41, 41, 41, 42, 42, 43, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 48, 49, 49, 49, 50, 50, 51, 51, 51, 52, 52, 53, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 58, 59, 59, 59, 60, 60, 61, 61, 61, 62, 62, 63, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 68, 69, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 73, 74, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 93, 94, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98,
  0,  0,  0,  1,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 31, 31, 31, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 41, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 45, 46, 46, 46, 47, 47, 48, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 52, 53, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 57, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 62, 62, 62, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67, 67, 68, 68, 69, 69, 69, 70, 70, 71, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 90, 91, 91, 92, 92, 92, 93, 93, 93, 94, 94, 95, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99,
  0,  0,  0,  1,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  5,  6,  6,  7,  7,  7,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 63, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 85, 86, 86, 87, 87, 87, 88, 88, 89, 89, 89, 90, 90, 90, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 98, 99, 99,100,
  0,  0,  0,  1,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  5,  6,  6,  7,  7,  7,  8,  8,  9,  9,  9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 51, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 57, 58, 58, 59, 59, 59, 60, 60, 60, 61, 61, 62, 62, 62, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 85, 86, 86, 87, 87, 87, 88, 88, 89, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99, 99, 99,100,100,101,
  0,  0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,  5,  5,  6,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 62, 62, 62, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 80, 81, 81, 82, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 90, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 98, 99, 99,100,100,100,101,101,102,
  0,  0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,  5,  5,  6,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 31, 31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 35, 36, 36, 37, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 69, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 73, 74, 74, 75, 75, 75, 76, 76, 77, 77, 77, 78, 78, 79, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 90, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 98, 99, 99,100,100,100,101,101,102,102,103,
  0,  0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,  5,  5,  6,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19, 19, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 35, 36, 36, 37, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 57, 58, 58, 59, 59, 59, 60, 60, 61, 61, 61, 62, 62, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 73, 73, 73, 74, 74, 75, 75, 75, 76, 76, 77, 77, 77, 78, 78, 79, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 90, 91, 91, 92, 92, 92, 93, 93, 94, 94, 95, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99, 99, 99,100,100,101,101,101,102,102,103,103,104,
  0,  0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,  5,  5,  6,  6,  7,  7,  7,  8,  8,  9,  9,  9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 35, 35, 35, 36, 36, 37, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 51, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 77, 77, 77, 78, 78, 79, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 95, 96, 96, 97, 97, 98, 98, 98, 99, 99,100,100,100,101,101,102,102,102,103,103,104,104,105,
  0,  0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,  5,  5,  6,  6,  7,  7,  7,  8,  8,  9,  9,  9, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19, 19, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 41, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 51, 51, 51, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 59, 59, 59, 60, 60, 61, 61, 61, 62, 62, 63, 63, 64, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 69, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 73, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 98, 99, 99,100,100,101,101,101,102,102,103,103,103,104,104,105,105,106,
  0,  0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  5,  5,  5,  6,  6,  7,  7,  7,  8,  8,  9,  9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 26, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 31, 31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 41, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 57, 57, 57, 58, 58, 59, 59, 60, 60, 60, 61, 61, 62, 62, 62, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 73, 73, 73, 74, 74, 75, 75, 75, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 80, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 99, 99, 99,100,100,101,101,101,102,102,103,103,104,104,104,105,105,106,106,107,
  0,  0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  5,  5,  5,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 19, 19, 19, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 30, 30, 30, 31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 41, 41, 41, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 69, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 77, 77, 77, 78, 78, 79, 79, 80, 80, 80, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 85, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 99, 99, 99,100,100,101,101,102,102,102,103,103,104,104,105,105,105,106,106,107,107,108,
  0,  0,  0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  5,  5,  5,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 26, 26, 26, 27, 27, 28, 28, 29, 29, 29, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 35, 35, 35, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 41, 41, 41, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 61, 61, 61, 62, 62, 63, 63, 64, 64, 64, 65, 65, 66, 66, 67, 67, 67, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 73, 73, 73, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 79, 79, 79, 80, 80, 81, 81, 82, 82, 82, 83, 83, 84, 84, 85, 85, 85, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99, 99,100,100,100,101,101,102,102,103,103,103,104,104,105,105,106,106,106,107,107,108,108,109,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  6,  6,  6,  7,  7,  8,  8,  9,  9,  9, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 31, 31, 31, 32, 32, 33, 33, 34, 34, 34, 35, 35, 36, 36, 37, 37, 37, 38, 38, 39, 39, 40, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 59, 59, 59, 60, 60, 61, 61, 62, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 69, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 75, 75, 75, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99, 99,100,100,100,101,101,102,102,103,103,103,104,104,105,105,106,106,106,107,107,108,108,109,109,110,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  6,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 26, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 30, 31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 37, 38, 38, 39, 39, 40, 40, 40, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 57, 58, 58, 59, 59, 60, 60, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 67, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 77, 77, 77, 78, 78, 79, 79, 80, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 87, 87, 87, 88, 88, 89, 89, 90, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99, 99,100,100,100,101,101,102,102,103,103,104,104,104,105,105,106,106,107,107,107,108,108,109,109,110,110,111,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  7,  8,  8,  9,  9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 57, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 68, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 75, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 90, 91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99, 99,100,100,101,101,101,102,102,103,103,104,104,104,105,105,106,106,107,107,108,108,108,109,109,110,110,111,111,112,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 35, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 77, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99, 99,100,100,101,101,101,102,102,103,103,104,104,105,105,105,106,106,107,107,108,108,109,109,109,110,110,111,111,112,112,113,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 97, 98, 98, 99, 99,100,100,101,101,101,102,102,103,103,104,104,105,105,105,106,106,107,107,108,108,109,109,109,110,110,111,111,112,112,113,113,114,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,101,102,102,103,103,104,104,105,105,105,106,106,107,107,108,108,109,109,110,110,110,111,111,112,112,113,113,114,114,115,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,100,101,101,102,102,103,103,104,104,105,105,105,106,106,107,107,108,108,109,109,110,110,110,111,111,112,112,113,113,114,114,115,115,116,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,100,101,101,102,102,103,103,104,104,105,105,105,106,106,107,107,108,108,109,109,110,110,111,111,111,112,112,113,113,114,114,115,115,116,116,117,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,105,106,106,107,107,108,108,109,109,110,110,111,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,106,107,107,108,108,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,123,124,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,125,126,
  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,125,126,126,127,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,125,126,126,127,128,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,125,126,126,127,127,128,129,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,125,126,126,127,127,128,128,129,130,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,125,126,126,127,127,128,128,129,129,130,131,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,125,126,126,127,127,128,128,129,129,130,130,131,132,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,120,121,121,122,122,123,123,124,124,125,125,126,126,127,127,128,128,129,129,130,130,131,131,132,133,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 80, 81, 81, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,112,112,113,114,114,115,115,116,116,117,117,118,118,119,119,120,120,121,121,122,122,123,124,124,125,125,126,126,127,127,128,128,129,129,130,130,131,131,132,132,133,134,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 36, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 79, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,108,108,109,109,110,110,111,111,112,112,113,113,114,114,115,115,116,117,117,118,118,119,119,120,120,121,121,122,122,123,123,124,124,125,126,126,127,127,128,128,129,129,130,130,131,131,132,132,133,133,134,135,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 72, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,120,120,121,121,122,122,123,123,124,124,125,125,126,126,127,128,128,129,129,130,130,131,131,132,132,133,133,134,134,135,136,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 78, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,109,109,110,110,111,111,112,112,113,113,114,114,115,116,116,117,117,118,118,119,119,120,120,121,121,122,123,123,124,124,125,125,126,126,127,127,128,128,129,130,130,131,131,132,132,133,133,134,134,135,135,136,137,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 66, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 84, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,106,106,107,107,108,108,109,109,110,110,111,112,112,113,113,114,114,115,115,116,116,117,117,118,119,119,120,120,121,121,122,122,123,123,124,125,125,126,126,127,127,128,128,129,129,130,130,131,132,132,133,133,134,134,135,135,136,136,137,138,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 31, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 67, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 83, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 89, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,103,103,104,104,105,105,106,106,107,107,108,109,109,110,110,111,111,112,112,113,113,114,115,115,116,116,117,117,118,118,119,119,120,121,121,122,122,123,123,124,124,125,125,126,127,127,128,128,129,129,130,130,131,131,132,133,133,134,134,135,135,136,136,137,137,138,139,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 62, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 88, 89, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,101,101,102,102,103,103,104,104,105,105,106,107,107,108,108,109,109,110,110,111,112,112,113,113,114,114,115,115,116,116,117,118,118,119,119,120,120,121,121,122,122,123,124,124,125,125,126,126,127,127,128,129,129,130,130,131,131,132,132,133,133,134,135,135,136,136,137,137,138,138,139,140,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 63, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99,100,100,101,101,102,102,103,103,104,105,105,106,106,107,107,108,108,109,110,110,111,111,112,112,113,113,114,115,115,116,116,117,117,118,118,119,119,120,121,121,122,122,123,123,124,124,125,126,126,127,127,128,128,129,129,130,131,131,132,132,133,133,134,134,135,136,136,137,137,138,138,139,139,140,141,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 45, 45, 46, 46, 47, 47, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 64, 64, 65, 65, 66, 66, 67, 67, 68, 69, 69, 70, 70, 71, 71, 72, 72, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 86, 87, 87, 88, 89, 89, 90, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 96, 97, 98, 98, 99, 99,100,100,101,101,102,103,103,104,104,105,105,106,106,107,108,108,109,109,110,110,111,111,112,113,113,114,114,115,115,116,116,117,118,118,119,119,120,120,121,121,122,123,123,124,124,125,125,126,126,127,128,128,129,129,130,130,131,131,132,133,133,134,134,135,135,136,136,137,138,138,139,139,140,140,141,142,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9, 10, 10, 11, 11, 12, 12, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 26, 27, 28, 28, 29, 29, 30, 30, 31, 31, 32, 33, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, 39, 40, 40, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 49, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 56, 56, 57, 57, 58, 58, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 65, 65, 66, 66, 67, 67, 68, 68, 69, 70, 70, 71, 71, 72, 72, 73, 74, 74, 75, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 86, 87, 88, 88, 89, 89, 90, 90, 91, 91, 92, 93, 93, 94, 94, 95, 95, 96, 97, 97, 98, 98, 99, 99,100,100,101,102,102,103,103,104,104,105,105,106,107,107,108,108,109,109,110,111,111,112,112,113,113,114,114,115,116,116,117,117,118,118,119,120,120,121,121,122,122,123,123,124,125,125,126,126,127,127,128,128,129,130,130,131,131,132,132,133,134,134,135,135,136,136,137,137,138,139,139,140,140,141,141,142,143,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  5,  5,  6,  6,  7,  7,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 14, 14, 15, 15, 16, 16, 17, 18, 18, 19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 29, 30, 31, 31, 32, 32, 33, 33, 34, 35, 35, 36, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 46, 47, 48, 48, 49, 49, 50, 50, 51, 51, 52, 53, 53, 54, 54, 55, 55, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 62, 62, 63, 63, 64, 64, 65, 66, 66, 67, 67, 68, 68, 69, 70, 70, 71, 71, 72, 72, 73, 73, 74, 75, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 83, 83, 84, 84, 85, 85, 86, 86, 87, 88, 88, 89, 89, 90, 90, 91, 92, 92, 93, 93, 94, 94, 95, 96, 96, 97, 97, 98, 98, 99, 99,100,101,101,102,102,103,103,104,105,105,106,106,107,107,108,108,109,110,110,111,111,112,112,113,114,114,115,115,116,116,117,118,118,119,119,120,120,121,121,122,123,123,124,124,125,125,126,127,127,128,128,129,129,130,131,131,132,132,133,133,134,134,135,136,136,137,137,138,138,139,140,140,141,141,142,142,143,144,
  0,  0,  1,  1,  2,  2,  3,  3,  4,  5,  5,  6,  6,  7,  7,  8,  9,  9, 10, 10, 11, 11, 12, 13, 13, 14, 14, 15, 15, 16, 17, 17, 18, 18, 19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 31, 31, 32, 32, 33, 34, 34, 35, 35, 36, 36, 37, 38, 38, 39, 39, 40, 40, 41, 42, 42, 43, 43, 44, 44, 45, 46, 46, 47, 47, 48, 48, 49, 50, 50, 51, 51, 52, 52, 53, 54, 54, 55, 55, 56, 56, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 63, 63, 64, 64, 65, 65, 66, 67, 67, 68, 68, 69, 69, 70, 71, 71, 72, 72, 73, 73, 74, 75, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 83, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 89, 90, 90, 91, 92, 92, 93, 93, 94, 94, 95, 96, 96, 97, 97, 98, 98, 99,100,100,101,101,102,102,103,104,104,105,105,106,106,107,108,108,109,109,110,110,111,112,112,113,113,114,114,115,116,116,117,117,118,118,119,119,120,121,121,122,122,123,123,124,125,125,126,126,127,127,128,129,129,130,130,131,131,132,133,133,134,134,135,135,136,137,137,138,138,139,139,140,141,141,142,142,143,143,144,145,
  0,  0,  1,  1,  2,  2,  3,  4,  4,  5,  5,  6,  6,  7,  8,  8,  9,  9, 10, 10, 11, 12, 12, 13, 13, 14, 14, 15, 16, 16, 17, 17, 18, 18, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 26, 27, 28, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 34, 35, 36, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 46, 47, 48, 48, 49, 49, 50, 50, 51, 52, 52, 53, 53, 54, 54, 55, 56, 56, 57, 57, 58, 58, 59, 60, 60, 61, 61, 62, 62, 63, 64, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 72, 72, 73, 73, 74, 75, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 83, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 89, 90, 91, 91, 92, 92, 93, 93, 94, 95, 95, 96, 96, 97, 97, 98, 99, 99,100,100,101,101,102,103,103,104,104,105,105,106,107,107,108,108,109,109,110,111,111,112,112,113,113,114,115,115,116,116,117,117,118,119,119,120,120,121,121,122,123,123,124,124,125,125,126,127,127,128,128,129,129,130,131,131,132,132,133,133,134,135,135,136,136,137,137,138,139,139,140,140,141,141,142,143,143,144,144,145,146,
  0,  0,  1,  1,  2,  2,  3,  4,  4,  5,  5,  6,  6,  7,  8,  8,  9,  9, 10, 10, 11, 12, 12, 13, 13, 14, 14, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 29, 30, 31, 31, 32, 32, 33, 34, 34, 35, 35, 36, 36, 37, 38, 38, 39, 39, 40, 40, 41, 42, 42, 43, 43, 44, 44, 45, 46, 46, 47, 47, 48, 49, 49, 50, 50, 51, 51, 52, 53, 53, 54, 54, 55, 55, 56, 57, 57, 58, 58, 59, 59, 60, 61, 61, 62, 62, 63, 63, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 70, 71, 72, 72, 73, 73, 74, 74, 75, 76, 76, 77, 77, 78, 78, 79, 80, 80, 81, 81, 82, 83, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 89, 90, 91, 91, 92, 92, 93, 93, 94, 95, 95, 96, 96, 97, 98, 98, 99, 99,100,100,101,102,102,103,103,104,104,105,106,106,107,107,108,108,109,110,110,111,111,112,112,113,114,114,115,115,116,117,117,118,118,119,119,120,121,121,122,122,123,123,124,125,125,126,126,127,127,128,129,129,130,130,131,132,132,133,133,134,134,135,136,136,137,137,138,138,139,140,140,141,141,142,142,143,144,144,145,145,146,147,
  0,  0,  1,  1,  2,  2,  3,  4,  4,  5,  5,  6,  6,  7,  8,  8,  9,  9, 10, 11, 11, 12, 12, 13, 13, 14, 15, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 31, 31, 32, 33, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 49, 50, 51, 51, 52, 52, 53, 53, 54, 55, 55, 56, 56, 57, 58, 58, 59, 59, 60, 60, 61, 62, 62, 63, 63, 64, 65, 65, 66, 66, 67, 67, 68, 69, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 76, 76, 77, 77, 78, 78, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 89, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 96, 97, 98, 98, 99, 99,100,100,101,102,102,103,103,104,105,105,106,106,107,107,108,109,109,110,110,111,112,112,113,113,114,114,115,116,116,117,117,118,118,119,120,120,121,121,122,123,123,124,124,125,125,126,127,127,128,128,129,130,130,131,131,132,132,133,134,134,135,135,136,136,137,138,138,139,139,140,141,141,142,142,143,143,144,145,145,146,146,147,148,
  0,  0,  1,  1,  2,  2,  3,  4,  4,  5,  5,  6,  7,  7,  8,  8,  9,  9, 10, 11, 11, 12, 12, 13, 14, 14, 15, 15, 16, 16, 17, 18, 18, 19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 35, 36, 36, 37, 37, 38, 39, 39, 40, 40, 41, 42, 42, 43, 43, 44, 44, 45, 46, 46, 47, 47, 48, 49, 49, 50, 50, 51, 52, 52, 53, 53, 54, 54, 55, 56, 56, 57, 57, 58, 59, 59, 60, 60, 61, 61, 62, 63, 63, 64, 64, 65, 66, 66, 67, 67, 68, 68, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 75, 76, 77, 77, 78, 78, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 89, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 96, 97, 98, 98, 99, 99,100,101,101,102,102,103,104,104,105,105,106,106,107,108,108,109,109,110,111,111,112,112,113,113,114,115,115,116,116,117,118,118,119,119,120,120,121,122,122,123,123,124,125,125,126,126,127,127,128,129,129,130,130,131,132,132,133,133,134,134,135,136,136,137,137,138,139,139,140,140,141,141,142,143,143,144,144,145,146,146,147,147,148,149,
  0,  0,  1,  1,  2,  2,  3,  4,  4,  5,  5,  6,  7,  7,  8,  8,  9, 10, 10, 11, 11, 12, 12, 13, 14, 14, 15, 15, 16, 17, 17, 18, 18, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 30, 30, 31, 31, 32, 32, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 50, 50, 51, 51, 52, 52, 53, 54, 54, 55, 55, 56, 57, 57, 58, 58, 59, 60, 60, 61, 61, 62, 62, 63, 64, 64, 65, 65, 66, 67, 67, 68, 68, 69, 70, 70, 71, 71, 72, 72, 73, 74, 74, 75, 75, 76, 77, 77, 78, 78, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 90, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 97, 97, 98, 98, 99,100,100,101,101,102,102,103,104,104,105,105,106,107,107,108,108,109,110,110,111,111,112,112,113,114,114,115,115,116,117,117,118,118,119,120,120,121,121,122,122,123,124,124,125,125,126,127,127,128,128,129,130,130,131,131,132,132,133,134,134,135,135,136,137,137,138,138,139,140,140,141,141,142,142,143,144,144,145,145,146,147,147,148,148,149,150,
  0,  0,  1,  1,  2,  2,  3,  4,  4,  5,  5,  6,  7,  7,  8,  8,  9, 10, 10, 11, 11, 12, 13, 13, 14, 14, 15, 15, 16, 17, 17, 18, 18, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 31, 31, 32, 33, 33, 34, 34, 35, 36, 36, 37, 37, 38, 39, 39, 40, 40, 41, 42, 42, 43, 43, 44, 45, 45, 46, 46, 47, 47, 48, 49, 49, 50, 50, 51, 52, 52, 53, 53, 54, 55, 55, 56, 56, 57, 58, 58, 59, 59, 60, 60, 61, 62, 62, 63, 63, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 71, 71, 72, 72, 73, 74, 74, 75, 75, 76, 76, 77, 78, 78, 79, 79, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 90, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 97, 97, 98, 98, 99,100,100,101,101,102,103,103,104,104,105,105,106,107,107,108,108,109,110,110,111,111,112,113,113,114,114,115,116,116,117,117,118,119,119,120,120,121,121,122,123,123,124,124,125,126,126,127,127,128,129,129,130,130,131,132,132,133,133,134,135,135,136,136,137,137,138,139,139,140,140,141,142,142,143,143,144,145,145,146,146,147,148,148,149,149,150,151,
  0,  0,  1,  1,  2,  2,  3,  4,  4,  5,  5,  6,  7,  7,  8,  8,  9, 10, 10, 11, 11, 12, 13, 13, 14, 14, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 35, 36, 36, 37, 38, 38, 39, 39, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 50, 50, 51, 51, 52, 53, 53, 54, 54, 55, 56, 56, 57, 57, 58, 59, 59, 60, 60, 61, 61, 62, 63, 63, 64, 64, 65, 66, 66, 67, 67, 68, 69, 69, 70, 70, 71, 72, 72, 73, 73, 74, 75, 75, 76, 76, 77, 78, 78, 79, 79, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 90, 90, 91, 91, 92, 92, 93, 94, 94, 95, 95, 96, 97, 97, 98, 98, 99,100,100,101,101,102,103,103,104,104,105,106,106,107,107,108,109,109,110,110,111,112,112,113,113,114,115,115,116,116,117,118,118,119,119,120,121,121,122,122,123,123,124,125,125,126,126,127,128,128,129,129,130,131,131,132,132,133,134,134,135,135,136,137,137,138,138,139,140,140,141,141,142,143,143,144,144,145,146,146,147,147,148,149,149,150,150,151,152,
  0,  0,  1,  1,  2,  3,  3,  4,  4,  5,  6,  6,  7,  7,  8,  9,  9, 10, 10, 11, 12, 12, 13, 13, 14, 15, 15, 16, 16, 17, 18, 18, 19, 19, 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 30, 30, 31, 31, 32, 33, 33, 34, 34, 35, 36, 36, 37, 37, 38, 39, 39, 40, 40, 41, 42, 42, 43, 43, 44, 45, 45, 46, 46, 47, 48, 48, 49, 49, 50, 51, 51, 52, 52, 53, 54, 54, 55, 55, 56, 57, 57, 58, 58, 59, 60, 60, 61, 61, 62, 63, 63, 64, 64, 65, 66, 66, 67, 67, 68, 69, 69, 70, 70, 71, 72, 72, 73, 73, 74, 75, 75, 76, 76, 77, 78, 78, 79, 79, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 87, 87, 88, 88, 89, 90, 90, 91, 91, 92, 93, 93, 94, 94, 95, 96, 96, 97, 97, 98, 99, 99,100,100,101,102,102,103,103,104,105,105,106,106,107,108,108,109,109,110,111,111,112,112,113,114,114,115,115,116,117,117,118,118,119,120,120,121,121,122,123,123,124,124,125,126,126,127,127,128,129,129,130,130,131,132,132,133,133,134,135,135,136,136,137,138,138,139,139,140,141,141,142,142,143,144,144,145,145,146,147,147,148,148,149,150,150,151,151,152,153,
  0,  0,  1,  1,  2,  3,  3,  4,  4,  5,  6,  6,  7,  7,  8,  9,  9, 10, 10, 11, 12, 12, 13, 13, 14, 15, 15, 16, 16, 17, 18, 18, 19, 19, 20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 35, 36, 36, 37, 38, 38, 39, 39, 40, 41, 41, 42, 42, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 50, 50, 51, 51, 52, 53, 53, 54, 54, 55, 56, 56, 57, 57, 58, 59, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 65, 66, 67, 67, 68, 68, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 76, 76, 77, 77, 78, 79, 79, 80, 80, 81, 82, 82, 83, 83, 84, 85, 85, 86, 86, 87, 88, 88, 89, 89, 90, 91, 91, 92, 93, 93, 94, 94, 95, 96, 96, 97, 97, 98, 99, 99,100,100,101,102,102,103,103,104,105,105,106,106,107,108,108,109,109,110,111,111,112,112,113,114,114,115,115,116,117,117,118,118,119,120,120,121,121,122,123,123,124,125,125,126,126,127,128,128,129,129,130,131,131,132,132,133,134,134,135,135,136,137,137,138,138,139,140,140,141,141,142,143,143,144,144,145,146,146,147,147,148,149,149,150,150,151,152,152,153,154,
  0,  0,  1,  1,  2,  3,  3,  4,  4,  5,  6,  6,  7,  7,  8,  9,  9, 10, 10, 11, 12, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 18, 19, 20, 20, 21, 21, 22, 23, 23, 24, 24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 32, 33, 34, 34, 35, 35, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 43, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 49, 50, 51, 51, 52, 52, 53, 54, 54, 55, 55, 56, 57, 57, 58, 58, 59, 60, 60, 61, 62, 62, 63, 63, 64, 65, 65, 66, 66, 67, 68, 68, 69, 69, 70, 71, 71, 72, 72, 73, 74, 74, 75, 75, 76, 77, 77, 78, 79, 79, 80, 80, 81, 82, 82, 83, 83, 84, 85, 85, 86, 86, 87, 88, 88, 89, 89, 90, 91, 91, 92, 93, 93, 94, 94, 95, 96, 96, 97, 97, 98, 99, 99,100,100,101,102,102,103,103,104,105,105,106,106,107,108,108,109,110,110,111,111,112,113,113,114,114,115,116,116,117,117,118,119,119,120,120,121,122,122,123,124,124,125,125,126,127,127,128,128,129,130,130,131,131,132,133,133,134,134,135,136,136,137,137,138,139,139,140,141,141,142,142,143,144,144,145,145,146,147,147,148,148,149,150,150,151,151,152,153,153,154,155,
  0,  0,  1,  1,  2,  3,  3,  4,  4,  5,  6,  6,  7,  7,  8,  9,  9, 10, 11, 11, 12, 12, 13, 14, 14, 15, 15, 16, 17, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29, 29, 30, 31, 31, 32, 33, 33, 34, 34, 35, 36, 36, 37, 37, 38, 39, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 45, 46, 47, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 53, 54, 55, 55, 56, 56, 57, 58, 58, 59, 59, 60, 61, 61, 62, 63, 63, 64, 64, 65, 66, 66, 67, 67, 68, 69, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 75, 76, 77, 77, 78, 78, 79, 80, 80, 81, 81, 82, 83, 83, 84, 85, 85, 86, 86, 87, 88, 88, 89, 89, 90, 91, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 97, 98, 99, 99,100,100,101,102,102,103,104,104,105,105,106,107,107,108,108,109,110,110,111,111,112,113,113,114,115,115,116,116,117,118,118,119,119,120,121,121,122,122,123,124,124,125,126,126,127,127,128,129,129,130,130,131,132,132,133,133,134,135,135,136,137,137,138,138,139,140,140,141,141,142,143,143,144,144,145,146,146,147,148,148,149,149,150,151,151,152,152,153,154,154,155,156,
  0,  0,  1,  1,  2,  3,  3,  4,  4,  5,  6,  6,  7,  8,  8,  9,  9, 10, 11, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 41, 42, 43, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 49, 50, 51, 51, 52, 52, 53, 54, 54, 55, 56, 56, 57, 57, 58, 59, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 65, 66, 67, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 73, 74, 75, 75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 81, 82, 83, 83, 84, 84, 85, 86, 86, 87, 88, 88, 89, 89, 90, 91, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 97, 98, 99, 99,100,100,101,102,102,103,104,104,105,105,106,107,107,108,108,109,110,110,111,112,112,113,113,114,115,115,116,116,117,118,118,119,120,120,121,121,122,123,123,124,124,125,126,126,127,128,128,129,129,130,131,131,132,132,133,134,134,135,136,136,137,137,138,139,139,140,140,141,142,142,143,144,144,145,145,146,147,147,148,148,149,150,150,151,152,152,153,153,154,155,155,156,157,
  0,  0,  1,  1,  2,  3,  3,  4,  4,  5,  6,  6,  7,  8,  8,  9,  9, 10, 11, 11, 12, 13, 13, 14, 14, 15, 16, 16, 17, 17, 18, 19, 19, 20, 21, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 35, 36, 37, 37, 38, 39, 39, 40, 40, 41, 42, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 53, 54, 55, 55, 56, 57, 57, 58, 58, 59, 60, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 71, 72, 73, 73, 74, 74, 75, 76, 76, 77, 78, 78, 79, 79, 80, 81, 81, 82, 83, 83, 84, 84, 85, 86, 86, 87, 87, 88, 89, 89, 90, 91, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 97, 98, 99, 99,100,100,101,102,102,103,104,104,105,105,106,107,107,108,109,109,110,110,111,112,112,113,114,114,115,115,116,117,117,118,118,119,120,120,121,122,122,123,123,124,125,125,126,127,127,128,128,129,130,130,131,131,132,133,133,134,135,135,136,136,137,138,138,139,140,140,141,141,142,143,143,144,144,145,146,146,147,148,148,149,149,150,151,151,152,153,153,154,154,155,156,156,157,158,
  0,  0,  1,  1,  2,  3,  3,  4,  4,  5,  6,  6,  7,  8,  8,  9,  9, 10, 11, 11, 12, 13, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 19, 20, 21, 21, 22, 23, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 29, 30, 31, 31, 32, 33, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 39, 40, 41, 41, 42, 43, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 54, 55, 56, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 67, 68, 69, 69, 70, 71, 71, 72, 72, 73, 74, 74, 75, 76, 76, 77, 77, 78, 79, 79, 80, 81, 81, 82, 82, 83, 84, 84, 85, 86, 86, 87, 87, 88, 89, 89, 90, 91, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 97, 98, 99, 99,100,101,101,102,102,103,104,104,105,106,106,107,107,108,109,109,110,110,111,112,112,113,114,114,115,115,116,117,117,118,119,119,120,120,121,122,122,123,124,124,125,125,126,127,127,128,129,129,130,130,131,132,132,133,134,134,135,135,136,137,137,138,139,139,140,140,141,142,142,143,144,144,145,145,146,147,147,148,149,149,150,150,151,152,152,153,154,154,155,155,156,157,157,158,159,
  0,  0,  1,  1,  2,  3,  3,  4,  5,  5,  6,  6,  7,  8,  8,  9, 10, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 35, 36, 37, 37, 38, 38, 39, 40, 40, 41, 42, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 53, 54, 55, 55, 56, 57, 57, 58, 58, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 65, 66, 67, 67, 68, 69, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 75, 76, 77, 77, 78, 79, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 85, 86, 87, 87, 88, 89, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 97, 98, 99, 99,100,101,101,102,102,103,104,104,105,106,106,107,107,108,109,109,110,111,111,112,112,113,114,114,115,116,116,117,117,118,119,119,120,121,121,122,122,123,124,124,125,126,126,127,128,128,129,129,130,131,131,132,133,133,134,134,135,136,136,137,138,138,139,139,140,141,141,142,143,143,144,144,145,146,146,147,148,148,149,149,150,151,151,152,153,153,154,154,155,156,156,157,158,158,159,160,
  0,  0,  1,  1,  2,  3,  3,  4,  5,  5,  6,  6,  7,  8,  8,  9, 10, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 35, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 54, 55, 56, 56, 57, 58, 58, 59, 59, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 71, 72, 73, 73, 74, 75, 75, 76, 77, 77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 83, 84, 85, 85, 86, 87, 87, 88, 89, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95, 95, 96, 97, 97, 98, 99, 99,100,101,101,102,102,103,104,104,105,106,106,107,107,108,109,109,110,111,111,112,113,113,114,114,115,116,116,117,118,118,119,119,120,121,121,122,123,123,124,125,125,126,126,127,128,128,129,130,130,131,131,132,133,133,134,135,135,136,137,137,138,138,139,140,140,141,142,142,143,143,144,145,145,146,147,147,148,149,149,150,150,151,152,152,153,154,154,155,155,156,157,157,158,159,159,160,161,
  0,  0,  1,  1,  2,  3,  3,  4,  5,  5,  6,  6,  7,  8,  8,  9, 10, 10, 11, 12, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 55, 56, 57, 57, 58, 59, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 67, 68, 69, 69, 70, 71, 71, 72, 73, 73, 74, 74, 75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 81, 82, 83, 83, 84, 85, 85, 86, 87, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95, 95, 96, 97, 97, 98, 99, 99,100,101,101,102,102,103,104,104,105,106,106,107,108,108,109,109,110,111,111,112,113,113,114,114,115,116,116,117,118,118,119,120,120,121,121,122,123,123,124,125,125,126,127,127,128,128,129,130,130,131,132,132,133,134,134,135,135,136,137,137,138,139,139,140,141,141,142,142,143,144,144,145,146,146,147,148,148,149,149,150,151,151,152,153,153,154,155,155,156,156,157,158,158,159,160,160,161,162,
  0,  0,  1,  1,  2,  3,  3,  4,  5,  5,  6,  7,  7,  8,  8,  9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 21, 21, 22, 23, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 31, 32, 33, 33, 34, 35, 35, 36, 37, 37, 38, 38, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 54, 55, 56, 56, 57, 58, 58, 59, 60, 60, 61, 62, 62, 63, 63, 64, 65, 65, 66, 67, 67, 68, 69, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 76, 76, 77, 77, 78, 79, 79, 80, 81, 81, 82, 83, 83, 84, 85, 85, 86, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 93, 94, 95, 95, 96, 97, 97, 98, 99, 99,100,100,101,102,102,103,104,104,105,106,106,107,108,108,109,109,110,111,111,112,113,113,114,115,115,116,116,117,118,118,119,120,120,121,122,122,123,124,124,125,125,126,127,127,128,129,129,130,131,131,132,132,133,134,134,135,136,136,137,138,138,139,139,140,141,141,142,143,143,144,145,145,146,147,147,148,148,149,150,150,151,152,152,153,154,154,155,155,156,157,157,158,159,159,160,161,161,162,163,
  0,  0,  1,  1,  2,  3,  3,  4,  5,  5,  6,  7,  7,  8,  9,  9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 19, 20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 46, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 55, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 63, 63, 64, 64, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 73, 74, 75, 75, 76, 77, 77, 78, 79, 79, 80, 81, 81, 82, 82, 83, 84, 84, 85, 86, 86, 87, 88, 88, 89, 90, 90, 91, 91, 92, 93, 93, 94, 95, 95, 96, 97, 97, 98, 99, 99,100,100,101,102,102,103,104,104,105,106,106,107,108,108,109,109,110,111,111,112,113,113,114,115,115,116,117,117,118,118,119,120,120,121,122,122,123,124,124,125,126,126,127,127,128,129,129,130,131,131,132,133,133,134,135,135,136,136,137,138,138,139,140,140,141,142,142,143,144,144,145,145,146,147,147,148,149,149,150,151,151,152,153,153,154,154,155,156,156,157,158,158,159,160,160,161,162,162,163,164,
  0,  0,  1,  1,  2,  3,  3,  4,  5,  5,  6,  7,  7,  8,  9,  9, 10, 11, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 45, 46, 47, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55, 56, 56, 57, 58, 58, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 67, 68, 69, 69, 70, 71, 71, 72, 73, 73, 74, 75, 75, 76, 77, 77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 86, 86, 87, 88, 88, 89, 89, 90, 91, 91, 92, 93, 93, 94, 95, 95, 96, 97, 97, 98, 99, 99,100,100,101,102,102,103,104,104,105,106,106,107,108,108,109,110,110,111,111,112,113,113,114,115,115,116,117,117,118,119,119,120,121,121,122,122,123,124,124,125,126,126,127,128,128,129,130,130,131,132,132,133,133,134,135,135,136,137,137,138,139,139,140,141,141,142,143,143,144,144,145,146,146,147,148,148,149,150,150,151,152,152,153,154,154,155,155,156,157,157,158,159,159,160,161,161,162,163,163,164,165,
  0,  0,  1,  1,  2,  3,  3,  4,  5,  5,  6,  7,  7,  8,  9,  9, 10, 11, 11, 12, 13, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 33, 34, 35, 35, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 55, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 67, 67, 68, 69, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 83, 84, 85, 85, 86, 87, 87, 88, 89, 89, 90, 91, 91, 92, 93, 93, 94, 95, 95, 96, 96, 97, 98, 98, 99,100,100,101,102,102,103,104,104,105,106,106,107,108,108,109,110,110,111,111,112,113,113,114,115,115,116,117,117,118,119,119,120,121,121,122,123,123,124,124,125,126,126,127,128,128,129,130,130,131,132,132,133,134,134,135,136,136,137,138,138,139,139,140,141,141,142,143,143,144,145,145,146,147,147,148,149,149,150,151,151,152,152,153,154,154,155,156,156,157,158,158,159,160,160,161,162,162,163,164,164,165,166,
  0,  0,  1,  1,  2,  3,  3,  4,  5,  5,  6,  7,  7,  8,  9,  9, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55, 56, 56, 57, 58, 58, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 75, 76, 77, 77, 78, 79, 79, 80, 81, 81, 82, 83, 83, 84, 85, 85, 86, 87, 87, 88, 89, 89, 90, 91, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 98, 98, 99,100,100,101,102,102,103,104,104,105,106,106,107,108,108,109,110,110,111,111,112,113,113,114,115,115,116,117,117,118,119,119,120,121,121,122,123,123,124,125,125,126,127,127,128,129,129,130,130,131,132,132,133,134,134,135,136,136,137,138,138,139,140,140,141,142,142,143,144,144,145,146,146,147,148,148,149,149,150,151,151,152,153,153,154,155,155,156,157,157,158,159,159,160,161,161,162,163,163,164,165,165,166,167,
  0,  0,  1,  1,  2,  3,  3,  4,  5,  5,  6,  7,  7,  8,  9,  9, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 56, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 67, 67, 68, 69, 69, 70, 71, 71, 72, 73, 73, 74, 75, 75, 76, 77, 77, 78, 79, 79, 80, 81, 81, 82, 83, 83, 84, 84, 85, 86, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 98, 98, 99,100,100,101,102,102,103,104,104,105,106,106,107,108,108,109,110,110,111,112,112,113,113,114,115,115,116,117,117,118,119,119,120,121,121,122,123,123,124,125,125,126,127,127,128,129,129,130,131,131,132,133,133,134,135,135,136,137,137,138,139,139,140,140,141,142,142,143,144,144,145,146,146,147,148,148,149,150,150,151,152,152,153,154,154,155,156,156,157,158,158,159,160,160,161,162,162,163,164,164,165,166,166,167,168,
  0,  0,  1,  1,  2,  3,  3,  4,  5,  5,  6,  7,  7,  8,  9,  9, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 33, 34, 35, 35, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55, 56, 56, 57, 58, 58, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 86, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 98, 98, 99,100,100,101,102,102,103,104,104,105,106,106,107,108,108,109,110,110,111,112,112,113,113,114,115,115,116,117,117,118,119,119,120,121,121,122,123,123,124,125,125,126,127,127,128,129,129,130,131,131,132,133,133,134,135,135,136,137,137,138,139,139,140,141,141,142,143,143,144,145,145,146,147,147,148,149,149,150,151,151,152,153,153,154,155,155,156,157,157,158,159,159,160,161,161,162,163,163,164,165,165,166,167,167,168,169,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  8,  9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 56, 56, 57, 58, 58, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 86, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 98, 98, 99,100,100,101,102,102,103,104,104,105,106,106,107,108,108,109,110,110,111,112,112,113,114,114,115,116,116,117,118,118,119,120,120,121,122,122,123,124,124,125,126,126,127,128,128,129,130,130,131,132,132,133,134,134,135,136,136,137,138,138,139,140,140,141,142,142,143,144,144,145,146,146,147,148,148,149,150,150,151,152,152,153,154,154,155,156,156,157,158,158,159,160,160,161,162,162,163,164,164,165,166,166,167,168,168,169,170,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  8,  9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 56, 57, 57, 58, 59, 59, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 67, 67, 68, 69, 69, 70, 71, 71, 72, 73, 73, 74, 75, 75, 76, 77, 77, 78, 79, 79, 80, 81, 81, 82, 83, 83, 84, 85, 85, 86, 87, 87, 88, 89, 89, 90, 91, 91, 92, 93, 93, 94, 95, 95, 96, 97, 97, 98, 99, 99,100,101,101,102,103,103,104,105,105,106,107,107,108,109,109,110,111,111,112,113,114,114,115,116,116,117,118,118,119,120,120,121,122,122,123,124,124,125,126,126,127,128,128,129,130,130,131,132,132,133,134,134,135,136,136,137,138,138,139,140,140,141,142,142,143,144,144,145,146,146,147,148,148,149,150,150,151,152,152,153,154,154,155,156,156,157,158,158,159,160,160,161,162,162,163,164,164,165,166,166,167,168,168,169,170,171,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  8,  9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 31, 32, 33, 33, 34, 35, 35, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55, 56, 57, 58, 58, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 86, 87, 87, 88, 89, 89, 90, 91, 91, 92, 93, 93, 94, 95, 95, 96, 97, 97, 98, 99, 99,100,101,101,102,103,103,104,105,105,106,107,107,108,109,109,110,111,111,112,113,113,114,115,116,116,117,118,118,119,120,120,121,122,122,123,124,124,125,126,126,127,128,128,129,130,130,131,132,132,133,134,134,135,136,136,137,138,138,139,140,140,141,142,142,143,144,145,145,146,147,147,148,149,149,150,151,151,152,153,153,154,155,155,156,157,157,158,159,159,160,161,161,162,163,163,164,165,165,166,167,167,168,169,169,170,171,172,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  8,  9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 33, 34, 35, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 56, 56, 57, 58, 59, 59, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 67, 67, 68, 69, 69, 70, 71, 71, 72, 73, 73, 74, 75, 75, 76, 77, 78, 78, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 86, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95, 96, 97, 97, 98, 99, 99,100,101,101,102,103,103,104,105,105,106,107,107,108,109,109,110,111,111,112,113,113,114,115,116,116,117,118,118,119,120,120,121,122,122,123,124,124,125,126,126,127,128,128,129,130,130,131,132,132,133,134,135,135,136,137,137,138,139,139,140,141,141,142,143,143,144,145,145,146,147,147,148,149,149,150,151,151,152,153,154,154,155,156,156,157,158,158,159,160,160,161,162,162,163,164,164,165,166,166,167,168,168,169,170,170,171,172,173,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  8,  9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55, 56, 57, 58, 58, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 73, 73, 74, 75, 75, 76, 77, 77, 78, 79, 79, 80, 81, 81, 82, 83, 83, 84, 85, 85, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95, 96, 96, 97, 98, 98, 99,100,100,101,102,103,103,104,105,105,106,107,107,108,109,109,110,111,111,112,113,113,114,115,116,116,117,118,118,119,120,120,121,122,122,123,124,124,125,126,126,127,128,128,129,130,131,131,132,133,133,134,135,135,136,137,137,138,139,139,140,141,141,142,143,143,144,145,146,146,147,148,148,149,150,150,151,152,152,153,154,154,155,156,156,157,158,158,159,160,161,161,162,163,163,164,165,165,166,167,167,168,169,169,170,171,171,172,173,174,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  8,  9, 10, 10, 11, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 19, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 39, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 54, 54, 55, 56, 56, 57, 58, 59, 59, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 82, 83, 83, 84, 85, 85, 86, 87, 87, 88, 89, 89, 90, 91, 91, 92, 93, 94, 94, 95, 96, 96, 97, 98, 98, 99,100,100,101,102,102,103,104,105,105,106,107,107,108,109,109,110,111,111,112,113,113,114,115,115,116,117,118,118,119,120,120,121,122,122,123,124,124,125,126,126,127,128,129,129,130,131,131,132,133,133,134,135,135,136,137,137,138,139,140,140,141,142,142,143,144,144,145,146,146,147,148,148,149,150,150,151,152,153,153,154,155,155,156,157,157,158,159,159,160,161,161,162,163,164,164,165,166,166,167,168,168,169,170,170,171,172,172,173,174,175,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  8,  9, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 31, 31, 32, 33, 33, 34, 35, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 53, 54, 55, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 66, 66, 67, 68, 69, 69, 70, 71, 71, 72, 73, 73, 74, 75, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 86, 86, 87, 88, 89, 89, 90, 91, 91, 92, 93, 93, 94, 95, 95, 96, 97, 98, 98, 99,100,100,101,102,102,103,104,104,105,106,106,107,108,109,109,110,111,111,112,113,113,114,115,115,116,117,118,118,119,120,120,121,122,122,123,124,124,125,126,126,127,128,129,129,130,131,131,132,133,133,134,135,135,136,137,138,138,139,140,140,141,142,142,143,144,144,145,146,147,147,148,149,149,150,151,151,152,153,153,154,155,155,156,157,158,158,159,160,160,161,162,162,163,164,164,165,166,167,167,168,169,169,170,171,171,172,173,173,174,175,176,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  9,  9, 10, 11, 11, 12, 13, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 20, 21, 22, 22, 23, 24, 24, 25, 26, 27, 27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 49, 49, 50, 51, 52, 52, 53, 54, 54, 55, 56, 56, 57, 58, 59, 59, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 67, 68, 68, 69, 70, 70, 71, 72, 72, 73, 74, 74, 75, 76, 77, 77, 78, 79, 79, 80, 81, 81, 82, 83, 83, 84, 85, 86, 86, 87, 88, 88, 89, 90, 90, 91, 92, 93, 93, 94, 95, 95, 96, 97, 97, 98, 99, 99,100,101,102,102,103,104,104,105,106,106,107,108,108,109,110,111,111,112,113,113,114,115,115,116,117,118,118,119,120,120,121,122,122,123,124,124,125,126,127,127,128,129,129,130,131,131,132,133,133,134,135,136,136,137,138,138,139,140,140,141,142,142,143,144,145,145,146,147,147,148,149,149,150,151,152,152,153,154,154,155,156,156,157,158,158,159,160,161,161,162,163,163,164,165,165,166,167,167,168,169,170,170,171,172,172,173,174,174,175,176,177,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  8,  9,  9, 10, 11, 11, 12, 13, 13, 14, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 32, 33, 34, 34, 35, 36, 36, 37, 38, 39, 39, 40, 41, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 48, 49, 50, 50, 51, 52, 53, 53, 54, 55, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 66, 67, 67, 68, 69, 69, 70, 71, 71, 72, 73, 73, 74, 75, 76, 76, 77, 78, 78, 79, 80, 80, 81, 82, 83, 83, 84, 85, 85, 86, 87, 87, 88, 89, 90, 90, 91, 92, 92, 93, 94, 94, 95, 96, 97, 97, 98, 99, 99,100,101,101,102,103,104,104,105,106,106,107,108,108,109,110,110,111,112,113,113,114,115,115,116,117,117,118,119,120,120,121,122,122,123,124,124,125,126,127,127,128,129,129,130,131,131,132,133,134,134,135,136,136,137,138,138,139,140,141,141,142,143,143,144,145,145,146,147,147,148,149,150,150,151,152,152,153,154,154,155,156,157,157,158,159,159,160,161,161,162,163,164,164,165,166,166,167,168,168,169,170,171,171,172,173,173,174,175,175,176,177,178,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  7,  7,  8,  9,  9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 46, 47, 47, 48, 49, 49, 50, 51, 51, 52, 53, 54, 54, 55, 56, 56, 57, 58, 58, 59, 60, 61, 61, 62, 63, 63, 64, 65, 65, 66, 67, 68, 68, 69, 70, 70, 71, 72, 73, 73, 74, 75, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 82, 83, 84, 84, 85, 86, 87, 87, 88, 89, 89, 90, 91, 91, 92, 93, 94, 94, 95, 96, 96, 97, 98, 98, 99,100,101,101,102,103,103,104,105,105,106,107,108,108,109,110,110,111,112,113,113,114,115,115,116,117,117,118,119,120,120,121,122,122,123,124,124,125,126,127,127,128,129,129,130,131,131,132,133,134,134,135,136,136,137,138,138,139,140,141,141,142,143,143,144,145,146,146,147,148,148,149,150,150,151,152,153,153,154,155,155,156,157,157,158,159,160,160,161,162,162,163,164,164,165,166,167,167,168,169,169,170,171,171,172,173,174,174,175,176,176,177,178,179,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  7,  7,  8,  9,  9, 10, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 31, 31, 32, 33, 33, 34, 35, 36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 62, 63, 64, 64, 65, 66, 67, 67, 68, 69, 69, 70, 71, 72, 72, 73, 74, 74, 75, 76, 76, 77, 78, 79, 79, 80, 81, 81, 82, 83, 84, 84, 85, 86, 86, 87, 88, 88, 89, 90, 91, 91, 92, 93, 93, 94, 95, 96, 96, 97, 98, 98, 99,100,100,101,102,103,103,104,105,105,106,107,108,108,109,110,110,111,112,112,113,114,115,115,116,117,117,118,119,120,120,121,122,122,123,124,124,125,126,127,127,128,129,129,130,131,132,132,133,134,134,135,136,136,137,138,139,139,140,141,141,142,143,144,144,145,146,146,147,148,148,149,150,151,151,152,153,153,154,155,156,156,157,158,158,159,160,160,161,162,163,163,164,165,165,166,167,168,168,169,170,170,171,172,172,173,174,175,175,176,177,177,178,179,180,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  7,  7,  8,  9,  9, 10, 11, 12, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 19, 20, 21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 36, 37, 38, 39, 39, 40, 41, 41, 42, 43, 44, 44, 45, 46, 46, 47, 48, 48, 49, 50, 51, 51, 52, 53, 53, 54, 55, 56, 56, 57, 58, 58, 59, 60, 61, 61, 62, 63, 63, 64, 65, 66, 66, 67, 68, 68, 69, 70, 70, 71, 72, 73, 73, 74, 75, 75, 76, 77, 78, 78, 79, 80, 80, 81, 82, 83, 83, 84, 85, 85, 86, 87, 88, 88, 89, 90, 90, 91, 92, 92, 93, 94, 95, 95, 96, 97, 97, 98, 99,100,100,101,102,102,103,104,105,105,106,107,107,108,109,110,110,111,112,112,113,114,114,115,116,117,117,118,119,119,120,121,122,122,123,124,124,125,126,127,127,128,129,129,130,131,132,132,133,134,134,135,136,136,137,138,139,139,140,141,141,142,143,144,144,145,146,146,147,148,149,149,150,151,151,152,153,154,154,155,156,156,157,158,158,159,160,161,161,162,163,163,164,165,166,166,167,168,168,169,170,171,171,172,173,173,174,175,176,176,177,178,178,179,180,181,
  0,  0,  1,  2,  2,  3,  4,  4,  5,  6,  7,  7,  8,  9,  9, 10, 11, 12, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 19, 20, 21, 22, 22, 23, 24, 24, 25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 44, 45, 46, 47, 47, 48, 49, 49, 50, 51, 52, 52, 53, 54, 54, 55, 56, 57, 57, 58, 59, 59, 60, 61, 62, 62, 63, 64, 64, 65, 66, 67, 67, 68, 69, 69, 70, 71, 72, 72, 73, 74, 74, 75, 76, 77, 77, 78, 79, 79, 80, 81, 82, 82, 83, 84, 84, 85, 86, 87, 87, 88, 89, 89, 90, 91, 92, 92, 93, 94, 94, 95, 96, 97, 97, 98, 99, 99,100,101,102,102,103,104,104,105,106,107,107,108,109,109,110,111,112,112,113,114,114,115,116,117,117,118,119,119,120,121,122,122,123,124,124,125,126,127,127,128,129,129,130,131,132,132,133,134,134,135,136,137,137,138,139,139,140,141,142,142,143,144,144,145,146,147,147,148,149,149,150,151,152,152,153,154,154,155,156,157,157,158,159,159,160,161,162,162,163,164,164,165,166,167,167,168,169,169,170,171,172,172,173,174,174,175,176,177,177,178,179,179,180,181,182,
  0,  0,  1,  2,  2,  3,  4,  5,  5,  6,  7,  7,  8,  9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 35, 36, 37, 38, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 50, 51, 52, 53, 53, 54, 55, 55, 56, 57, 58, 58, 59, 60, 61, 61, 62, 63, 63, 64, 65, 66, 66, 67, 68, 68, 69, 70, 71, 71, 72, 73, 73, 74, 75, 76, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 83, 84, 85, 86, 86, 87, 88, 88, 89, 90, 91, 91, 92, 93, 94, 94, 95, 96, 96, 97, 98, 99, 99,100,101,101,102,103,104,104,105,106,106,107,108,109,109,110,111,111,112,113,114,114,115,116,116,117,118,119,119,120,121,122,122,123,124,124,125,126,127,127,128,129,129,130,131,132,132,133,134,134,135,136,137,137,138,139,139,140,141,142,142,143,144,144,145,146,147,147,148,149,149,150,151,152,152,153,154,155,155,156,157,157,158,159,160,160,161,162,162,163,164,165,165,166,167,167,168,169,170,170,171,172,172,173,174,175,175,176,177,177,178,179,180,180,181,182,183,
  0,  0,  1,  2,  2,  3,  4,  5,  5,  6,  7,  7,  8,  9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 20, 21, 22, 23, 23, 24, 25, 25, 26, 27, 28, 28, 29, 30, 31, 31, 32, 33, 33, 34, 35, 36, 36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 44, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 51, 52, 53, 54, 54, 55, 56, 57, 57, 58, 59, 59, 60, 61, 62, 62, 63, 64, 64, 65, 66, 67, 67, 68, 69, 69, 70, 71, 72, 72, 73, 74, 75, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 82, 83, 84, 85, 85, 86, 87, 88, 88, 89, 90, 90, 91, 92, 93, 93, 94, 95, 95, 96, 97, 98, 98, 99,100,101,101,102,103,103,104,105,106,106,107,108,108,109,110,111,111,112,113,114,114,115,116,116,117,118,119,119,120,121,121,122,123,124,124,125,126,126,127,128,129,129,130,131,132,132,133,134,134,135,136,137,137,138,139,139,140,141,142,142,143,144,145,145,146,147,147,148,149,150,150,151,152,152,153,154,155,155,156,157,158,158,159,160,160,161,162,163,163,164,165,165,166,167,168,168,169,170,171,171,172,173,173,174,175,176,176,177,178,178,179,180,181,181,182,183,184,
  0,  0,  1,  2,  2,  3,  4,  5,  5,  6,  7,  7,  8,  9, 10, 10, 11, 12, 13, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 44, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 58, 58, 59, 60, 60, 61, 62, 63, 63, 64, 65, 66, 66, 67, 68, 68, 69, 70, 71, 71, 72, 73, 74, 74, 75, 76, 76, 77, 78, 79, 79, 80, 81, 81, 82, 83, 84, 84, 85, 86, 87, 87, 88, 89, 89, 90, 91, 92, 92, 93, 94, 95, 95, 96, 97, 97, 98, 99,100,100,101,102,103,103,104,105,105,106,107,108,108,109,110,111,111,112,113,113,114,115,116,116,117,118,118,119,120,121,121,122,123,124,124,125,126,126,127,128,129,129,130,131,132,132,133,134,134,135,136,137,137,138,139,140,140,141,142,142,143,144,145,145,146,147,148,148,149,150,150,151,152,153,153,154,155,155,156,157,158,158,159,160,161,161,162,163,163,164,165,166,166,167,168,169,169,170,171,171,172,173,174,174,175,176,177,177,178,179,179,180,181,182,182,183,184,185,
  0,  0,  1,  2,  2,  3,  4,  5,  5,  6,  7,  8,  8,  9, 10, 10, 11, 12, 13, 13, 14, 15, 16, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 51, 51, 52, 53, 53, 54, 55, 56, 56, 57, 58, 59, 59, 60, 61, 62, 62, 63, 64, 64, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 72, 73, 74, 75, 75, 76, 77, 78, 78, 79, 80, 80, 81, 82, 83, 83, 84, 85, 86, 86, 87, 88, 88, 89, 90, 91, 91, 92, 93, 94, 94, 95, 96, 97, 97, 98, 99, 99,100,101,102,102,103,104,105,105,106,107,107,108,109,110,110,111,112,113,113,114,115,115,116,117,118,118,119,120,121,121,122,123,124,124,125,126,126,127,128,129,129,130,131,132,132,133,134,134,135,136,137,137,138,139,140,140,141,142,142,143,144,145,145,146,147,148,148,149,150,150,151,152,153,153,154,155,156,156,157,158,159,159,160,161,161,162,163,164,164,165,166,167,167,168,169,169,170,171,172,172,173,174,175,175,176,177,177,178,179,180,180,181,182,183,183,184,185,186,
  0,  0,  1,  2,  2,  3,  4,  5,  5,  6,  7,  8,  8,  9, 10, 11, 11, 12, 13, 13, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 22, 22, 23, 24, 24, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 35, 36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 44, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 63, 63, 64, 65, 66, 66, 67, 68, 68, 69, 70, 71, 71, 72, 73, 74, 74, 75, 76, 77, 77, 78, 79, 79, 80, 81, 82, 82, 83, 84, 85, 85, 86, 87, 88, 88, 89, 90, 90, 91, 92, 93, 93, 94, 95, 96, 96, 97, 98, 99, 99,100,101,101,102,103,104,104,105,106,107,107,108,109,110,110,111,112,112,113,114,115,115,116,117,118,118,119,120,121,121,122,123,123,124,125,126,126,127,128,129,129,130,131,132,132,133,134,134,135,136,137,137,138,139,140,140,141,142,143,143,144,145,145,146,147,148,148,149,150,151,151,152,153,154,154,155,156,156,157,158,159,159,160,161,162,162,163,164,165,165,166,167,167,168,169,170,170,171,172,173,173,174,175,176,176,177,178,178,179,180,181,181,182,183,184,184,185,186,187,
  0,  0,  1,  2,  2,  3,  4,  5,  5,  6,  7,  8,  8,  9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 28, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 36, 36, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 44, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 53, 53, 54, 55, 56, 56, 57, 58, 58, 59, 60, 61, 61, 62, 63, 64, 64, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 72, 73, 74, 75, 75, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 84, 85, 86, 86, 87, 88, 89, 89, 90, 91, 92, 92, 93, 94, 95, 95, 96, 97, 98, 98, 99,100,101,101,102,103,103,104,105,106,106,107,108,109,109,110,111,112,112,113,114,115,115,116,117,117,118,119,120,120,121,122,123,123,124,125,126,126,127,128,129,129,130,131,131,132,133,134,134,135,136,137,137,138,139,140,140,141,142,143,143,144,145,145,146,147,148,148,149,150,151,151,152,153,154,154,155,156,157,157,158,159,159,160,161,162,162,163,164,165,165,166,167,168,168,169,170,171,171,172,173,173,174,175,176,176,177,178,179,179,180,181,182,182,183,184,185,185,186,187,188,
  0,  0,  1,  2,  2,  3,  4,  5,  5,  6,  7,  8,  8,  9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 28, 28, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 51, 51, 52, 53, 54, 54, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 63, 63, 64, 65, 65, 66, 67, 68, 68, 69, 70, 71, 71, 72, 73, 74, 74, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 83, 83, 84, 85, 85, 86, 87, 88, 88, 89, 90, 91, 91, 92, 93, 94, 94, 95, 96, 97, 97, 98, 99,100,100,101,102,103,103,104,105,105,106,107,108,108,109,110,111,111,112,113,114,114,115,116,117,117,118,119,120,120,121,122,123,123,124,125,126,126,127,128,128,129,130,131,131,132,133,134,134,135,136,137,137,138,139,140,140,141,142,143,143,144,145,146,146,147,148,148,149,150,151,151,152,153,154,154,155,156,157,157,158,159,160,160,161,162,163,163,164,165,166,166,167,168,168,169,170,171,171,172,173,174,174,175,176,177,177,178,179,180,180,181,182,183,183,184,185,186,186,187,188,189,
  0,  0,  1,  2,  2,  3,  4,  5,  5,  6,  7,  8,  8,  9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 23, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 38, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 58, 58, 59, 60, 61, 61, 62, 63, 64, 64, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 73, 73, 74, 75, 76, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 84, 85, 86, 87, 87, 88, 89, 90, 90, 91, 92, 93, 93, 94, 95, 96, 96, 97, 98, 99, 99,100,101,102,102,103,104,105,105,106,107,108,108,109,110,111,111,112,113,114,114,115,116,116,117,118,119,119,120,121,122,122,123,124,125,125,126,127,128,128,129,130,131,131,132,133,134,134,135,136,137,137,138,139,140,140,141,142,143,143,144,145,146,146,147,148,149,149,150,151,152,152,153,154,154,155,156,157,157,158,159,160,160,161,162,163,163,164,165,166,166,167,168,169,169,170,171,172,172,173,174,175,175,176,177,178,178,179,180,181,181,182,183,184,184,185,186,187,187,188,189,190,
  0,  0,  1,  2,  2,  3,  4,  5,  5,  6,  7,  8,  8,  9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 23, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 44, 44, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 53, 53, 54, 55, 56, 56, 57, 58, 59, 59, 60, 61, 62, 62, 63, 64, 65, 65, 66, 67, 68, 68, 69, 70, 71, 71, 72, 73, 74, 74, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 83, 83, 84, 85, 86, 86, 87, 88, 89, 89, 90, 91, 92, 92, 93, 94, 95, 95, 96, 97, 98, 98, 99,100,101,101,102,103,104,104,105,106,107,107,108,109,110,110,111,112,113,113,114,115,116,116,117,118,119,119,120,121,122,122,123,124,125,125,126,127,128,128,129,130,131,131,132,133,134,134,135,136,137,137,138,139,140,140,141,142,143,143,144,145,146,146,147,148,149,149,150,151,152,152,153,154,155,155,156,157,158,158,159,160,161,161,162,163,164,164,165,166,167,167,168,169,170,170,171,172,173,173,174,175,176,176,177,178,179,179,180,181,182,182,183,184,185,185,186,187,188,188,189,190,191,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  6,  7,  8,  9,  9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 36, 36, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 51, 51, 52, 53, 54, 54, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 63, 64, 64, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 73, 73, 74, 75, 76, 76, 77, 78, 79, 79, 80, 81, 82, 82, 83, 84, 85, 85, 86, 87, 88, 88, 89, 90, 91, 91, 92, 93, 94, 94, 95, 96, 97, 97, 98, 99,100,100,101,102,103,103,104,105,106,106,107,108,109,109,110,111,112,112,113,114,115,115,116,117,118,118,119,120,121,121,122,123,124,124,125,126,127,128,128,129,130,131,131,132,133,134,134,135,136,137,137,138,139,140,140,141,142,143,143,144,145,146,146,147,148,149,149,150,151,152,152,153,154,155,155,156,157,158,158,159,160,161,161,162,163,164,164,165,166,167,167,168,169,170,170,171,172,173,173,174,175,176,176,177,178,179,179,180,181,182,182,183,184,185,185,186,187,188,188,189,190,191,192,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  6,  7,  8,  9,  9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 28, 28, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 52, 52, 53, 54, 55, 56, 56, 57, 58, 59, 59, 60, 61, 62, 62, 63, 64, 65, 65, 66, 67, 68, 68, 69, 70, 71, 71, 72, 73, 74, 74, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 83, 84, 84, 85, 86, 87, 87, 88, 89, 90, 90, 91, 92, 93, 93, 94, 95, 96, 96, 97, 98, 99, 99,100,101,102,102,103,104,105,105,106,107,108,108,109,110,111,112,112,113,114,115,115,116,117,118,118,119,120,121,121,122,123,124,124,125,126,127,127,128,129,130,130,131,132,133,133,134,135,136,136,137,138,139,140,140,141,142,143,143,144,145,146,146,147,148,149,149,150,151,152,152,153,154,155,155,156,157,158,158,159,160,161,161,162,163,164,164,165,166,167,168,168,169,170,171,171,172,173,174,174,175,176,177,177,178,179,180,180,181,182,183,183,184,185,186,186,187,188,189,189,190,191,192,193,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  6,  7,  8,  9,  9, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 28, 28, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 44, 44, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 53, 54, 54, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 63, 63, 64, 65, 66, 66, 67, 68, 69, 69, 70, 71, 72, 73, 73, 74, 75, 76, 76, 77, 78, 79, 79, 80, 81, 82, 82, 83, 84, 85, 85, 86, 87, 88, 89, 89, 90, 91, 92, 92, 93, 94, 95, 95, 96, 97, 98, 98, 99,100,101,101,102,103,104,104,105,106,107,108,108,109,110,111,111,112,113,114,114,115,116,117,117,118,119,120,120,121,122,123,124,124,125,126,127,127,128,129,130,130,131,132,133,133,134,135,136,136,137,138,139,139,140,141,142,143,143,144,145,146,146,147,148,149,149,150,151,152,152,153,154,155,155,156,157,158,159,159,160,161,162,162,163,164,165,165,166,167,168,168,169,170,171,171,172,173,174,174,175,176,177,178,178,179,180,181,181,182,183,184,184,185,186,187,187,188,189,190,190,191,192,193,194,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  6,  7,  8,  9,  9, 10, 11, 12, 13, 13, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 22, 22, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 58, 58, 59, 60, 61, 61, 62, 63, 64, 65, 65, 66, 67, 68, 68, 69, 70, 71, 71, 72, 73, 74, 74, 75, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 84, 85, 86, 87, 87, 88, 89, 90, 91, 91, 92, 93, 94, 94, 95, 96, 97, 97, 98, 99,100,100,101,102,103,104,104,105,106,107,107,108,109,110,110,111,112,113,113,114,115,116,117,117,118,119,120,120,121,122,123,123,124,125,126,126,127,128,129,130,130,131,132,133,133,134,135,136,136,137,138,139,139,140,141,142,143,143,144,145,146,146,147,148,149,149,150,151,152,152,153,154,155,156,156,157,158,159,159,160,161,162,162,163,164,165,165,166,167,168,169,169,170,171,172,172,173,174,175,175,176,177,178,178,179,180,181,182,182,183,184,185,185,186,187,188,188,189,190,191,191,192,193,194,195,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  6,  7,  8,  9,  9, 10, 11, 12, 13, 13, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 22, 23, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 32, 33, 33, 34, 35, 36, 36, 37, 38, 39, 39, 40, 41, 42, 43, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 52, 53, 53, 54, 55, 56, 56, 57, 58, 59, 59, 60, 61, 62, 63, 63, 64, 65, 66, 66, 67, 68, 69, 69, 70, 71, 72, 73, 73, 74, 75, 76, 76, 77, 78, 79, 79, 80, 81, 82, 83, 83, 84, 85, 86, 86, 87, 88, 89, 89, 90, 91, 92, 93, 93, 94, 95, 96, 96, 97, 98, 99, 99,100,101,102,102,103,104,105,106,106,107,108,109,109,110,111,112,112,113,114,115,116,116,117,118,119,119,120,121,122,122,123,124,125,126,126,127,128,129,129,130,131,132,132,133,134,135,136,136,137,138,139,139,140,141,142,142,143,144,145,146,146,147,148,149,149,150,151,152,152,153,154,155,156,156,157,158,159,159,160,161,162,162,163,164,165,166,166,167,168,169,169,170,171,172,172,173,174,175,176,176,177,178,179,179,180,181,182,182,183,184,185,186,186,187,188,189,189,190,191,192,192,193,194,195,196,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  6,  7,  8,  9, 10, 10, 11, 12, 13, 13, 14, 15, 16, 16, 17, 18, 19, 20, 20, 21, 22, 23, 23, 24, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 43, 44, 44, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 53, 54, 54, 55, 56, 57, 57, 58, 59, 60, 61, 61, 62, 63, 64, 64, 65, 66, 67, 67, 68, 69, 70, 71, 71, 72, 73, 74, 74, 75, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 84, 85, 86, 87, 88, 88, 89, 90, 91, 91, 92, 93, 94, 95, 95, 96, 97, 98, 98, 99,100,101,101,102,103,104,105,105,106,107,108,108,109,110,111,112,112,113,114,115,115,116,117,118,118,119,120,121,122,122,123,124,125,125,126,127,128,129,129,130,131,132,132,133,134,135,135,136,137,138,139,139,140,141,142,142,143,144,145,146,146,147,148,149,149,150,151,152,152,153,154,155,156,156,157,158,159,159,160,161,162,163,163,164,165,166,166,167,168,169,169,170,171,172,173,173,174,175,176,176,177,178,179,180,180,181,182,183,183,184,185,186,186,187,188,189,190,190,191,192,193,193,194,195,196,197,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  6,  7,  8,  9, 10, 10, 11, 12, 13, 13, 14, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 23, 24, 24, 25, 26, 27, 27, 28, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 44, 45, 45, 46, 47, 48, 48, 49, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 58, 59, 59, 60, 61, 62, 62, 63, 64, 65, 66, 66, 67, 68, 69, 69, 70, 71, 72, 72, 73, 74, 75, 76, 76, 77, 78, 79, 79, 80, 81, 82, 83, 83, 84, 85, 86, 86, 87, 88, 89, 90, 90, 91, 92, 93, 93, 94, 95, 96, 97, 97, 98, 99,100,100,101,102,103,104,104,105,106,107,107,108,109,110,111,111,112,113,114,114,115,116,117,118,118,119,120,121,121,122,123,124,125,125,126,127,128,128,129,130,131,132,132,133,134,135,135,136,137,138,138,139,140,141,142,142,143,144,145,145,146,147,148,149,149,150,151,152,152,153,154,155,156,156,157,158,159,159,160,161,162,163,163,164,165,166,166,167,168,169,170,170,171,172,173,173,174,175,176,177,177,178,179,180,180,181,182,183,184,184,185,186,187,187,188,189,190,191,191,192,193,194,194,195,196,197,198,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  7,  7,  8,  9, 10, 10, 11, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 28, 28, 29, 30, 31, 31, 32, 33, 34, 35, 35, 36, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 45, 46, 46, 47, 48, 49, 49, 50, 51, 52, 53, 53, 54, 55, 56, 56, 57, 58, 59, 60, 60, 61, 62, 63, 63, 64, 65, 66, 67, 67, 68, 69, 70, 71, 71, 72, 73, 74, 74, 75, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 85, 85, 86, 87, 88, 88, 89, 90, 91, 92, 92, 93, 94, 95, 95, 96, 97, 98, 99, 99,100,101,102,103,103,104,105,106,106,107,108,109,110,110,111,112,113,113,114,115,116,117,117,118,119,120,120,121,122,123,124,124,125,126,127,127,128,129,130,131,131,132,133,134,135,135,136,137,138,138,139,140,141,142,142,143,144,145,145,146,147,148,149,149,150,151,152,152,153,154,155,156,156,157,158,159,159,160,161,162,163,163,164,165,166,167,167,168,169,170,170,171,172,173,174,174,175,176,177,177,178,179,180,181,181,182,183,184,184,185,186,187,188,188,189,190,191,191,192,193,194,195,195,196,197,198,199,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  7,  7,  8,  9, 10, 10, 11, 12, 13, 14, 14, 15, 16, 17, 18, 18, 19, 20, 21, 21, 22, 23, 24, 25, 25, 26, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 36, 36, 37, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 58, 59, 60, 61, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 69, 70, 71, 72, 72, 73, 74, 75, 76, 76, 77, 78, 79, 80, 80, 81, 82, 83, 83, 84, 85, 86, 87, 87, 88, 89, 90, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 98, 99,100,101,101,102,103,104,105,105,106,107,108,109,109,110,111,112,112,113,114,115,116,116,117,118,119,120,120,121,122,123,123,124,125,126,127,127,128,129,130,130,131,132,133,134,134,135,136,137,138,138,139,140,141,141,142,143,144,145,145,146,147,148,149,149,150,151,152,152,153,154,155,156,156,157,158,159,160,160,161,162,163,163,164,165,166,167,167,168,169,170,170,171,172,173,174,174,175,176,177,178,178,179,180,181,181,182,183,184,185,185,186,187,188,189,189,190,191,192,192,193,194,195,196,196,197,198,199,200,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  7,  7,  8,  9, 10, 11, 11, 12, 13, 14, 14, 15, 16, 17, 18, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 26, 27, 28, 29, 29, 30, 31, 32, 33, 33, 34, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 58, 59, 59, 60, 61, 62, 63, 63, 64, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 85, 85, 86, 87, 88, 89, 89, 90, 91, 92, 93, 93, 94, 95, 96, 96, 97, 98, 99,100,100,101,102,103,104,104,105,106,107,107,108,109,110,111,111,112,113,114,115,115,116,117,118,119,119,120,121,122,122,123,124,125,126,126,127,128,129,130,130,131,132,133,134,134,135,136,137,137,138,139,140,141,141,142,143,144,145,145,146,147,148,148,149,150,151,152,152,153,154,155,156,156,157,158,159,160,160,161,162,163,163,164,165,166,167,167,168,169,170,171,171,172,173,174,174,175,176,177,178,178,179,180,181,182,182,183,184,185,186,186,187,188,189,189,190,191,192,193,193,194,195,196,197,197,198,199,200,201,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  7,  7,  8,  9, 10, 11, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 19, 20, 21, 22, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 30, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 44, 45, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 53, 54, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 68, 69, 70, 71, 72, 72, 73, 74, 75, 76, 76, 77, 78, 79, 80, 80, 81, 82, 83, 83, 84, 85, 86, 87, 87, 88, 89, 90, 91, 91, 92, 93, 94, 95, 95, 96, 97, 98, 99, 99,100,101,102,102,103,104,105,106,106,107,108,109,110,110,111,112,113,114,114,115,116,117,118,118,119,120,121,121,122,123,124,125,125,126,127,128,129,129,130,131,132,133,133,134,135,136,137,137,138,139,140,141,141,142,143,144,144,145,146,147,148,148,149,150,151,152,152,153,154,155,156,156,157,158,159,160,160,161,162,163,163,164,165,166,167,167,168,169,170,171,171,172,173,174,175,175,176,177,178,179,179,180,181,182,182,183,184,185,186,186,187,188,189,190,190,191,192,193,194,194,195,196,197,198,198,199,200,201,202,
  0,  0,  1,  2,  3,  3,  4,  5,  6,  7,  7,  8,  9, 10, 11, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 27, 28, 29, 30, 31, 31, 32, 33, 34, 35, 35, 36, 37, 38, 39, 39, 40, 41, 42, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 85, 85, 86, 87, 88, 89, 89, 90, 91, 92, 93, 93, 94, 95, 96, 97, 97, 98, 99,100,101,101,102,103,104,105,105,106,107,108,109,109,110,111,112,113,113,114,115,116,117,117,118,119,120,121,121,122,123,124,124,125,126,127,128,128,129,130,131,132,132,133,134,135,136,136,137,138,139,140,140,141,142,143,144,144,145,146,147,148,148,149,150,151,152,152,153,154,155,156,156,157,158,159,160,160,161,162,163,163,164,165,166,167,167,168,169,170,171,171,172,173,174,175,175,176,177,178,179,179,180,181,182,183,183,184,185,186,187,187,188,189,190,191,191,192,193,194,195,195,196,197,198,199,199,200,201,202,203,
  0,  0,  1,  2,  3,  4,  4,  5,  6,  7,  8,  8,  9, 10, 11, 12, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 68, 69, 70, 71, 72, 72, 73, 74, 75, 76, 76, 77, 78, 79, 80, 80, 81, 82, 83, 84, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 92, 93, 94, 95, 96, 96, 97, 98, 99,100,100,101,102,103,104,104,105,106,107,108,108,109,110,111,112,112,113,114,115,116,116,117,118,119,120,120,121,122,123,124,124,125,126,127,128,128,129,130,131,132,132,133,134,135,136,136,137,138,139,140,140,141,142,143,144,144,145,146,147,148,148,149,150,151,152,152,153,154,155,156,156,157,158,159,160,160,161,162,163,164,164,165,166,167,168,168,169,170,171,172,172,173,174,175,176,176,177,178,179,180,180,181,182,183,184,184,185,186,187,188,188,189,190,191,192,192,193,194,195,196,196,197,198,199,200,200,201,202,203,204,
  0,  0,  1,  2,  3,  4,  4,  5,  6,  7,  8,  8,  9, 10, 11, 12, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 41, 42, 43, 44, 45, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 53, 54, 55, 56, 57, 57, 58, 59, 60, 61, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 69, 70, 71, 72, 73, 73, 74, 75, 76, 77, 77, 78, 79, 80, 81, 82, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 98, 99,100,101,102,102,103,104,105,106,106,107,108,109,110,110,111,112,113,114,114,115,116,117,118,118,119,120,121,122,123,123,124,125,126,127,127,128,129,130,131,131,132,133,134,135,135,136,137,138,139,139,140,141,142,143,143,144,145,146,147,147,148,149,150,151,151,152,153,154,155,155,156,157,158,159,159,160,161,162,163,164,164,165,166,167,168,168,169,170,171,172,172,173,174,175,176,176,177,178,179,180,180,181,182,183,184,184,185,186,187,188,188,189,190,191,192,192,193,194,195,196,196,197,198,199,200,200,201,202,203,204,205,
  0,  0,  1,  2,  3,  4,  4,  5,  6,  7,  8,  8,  9, 10, 11, 12, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 21, 21, 22, 23, 24, 25, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 63, 64, 65, 66, 67, 67, 68, 69, 70, 71, 71, 72, 73, 74, 75, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 92, 93, 94, 95, 96, 96, 97, 98, 99,100,100,101,102,103,104,105,105,106,107,108,109,109,110,111,112,113,113,114,115,116,117,117,118,119,120,121,121,122,123,124,125,126,126,127,128,129,130,130,131,132,133,134,134,135,136,137,138,138,139,140,141,142,142,143,144,145,146,147,147,148,149,150,151,151,152,153,154,155,155,156,157,158,159,159,160,161,162,163,163,164,165,166,167,168,168,169,170,171,172,172,173,174,175,176,176,177,178,179,180,180,181,182,183,184,184,185,186,187,188,189,189,190,191,192,193,193,194,195,196,197,197,198,199,200,201,201,202,203,204,205,206,
  0,  0,  1,  2,  3,  4,  4,  5,  6,  7,  8,  8,  9, 10, 11, 12, 12, 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 21, 22, 23, 24, 25, 25, 26, 27, 28, 29, 30, 30, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 47, 48, 49, 50, 51, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 69, 69, 70, 71, 72, 73, 73, 74, 75, 76, 77, 77, 78, 79, 80, 81, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 99, 99,100,101,102,103,103,104,105,106,107,107,108,109,110,111,112,112,113,114,115,116,116,117,118,119,120,120,121,122,123,124,125,125,126,127,128,129,129,130,131,132,133,133,134,135,136,137,138,138,139,140,141,142,142,143,144,145,146,146,147,148,149,150,150,151,152,153,154,155,155,156,157,158,159,159,160,161,162,163,163,164,165,166,167,168,168,169,170,171,172,172,173,174,175,176,176,177,178,179,180,181,181,182,183,184,185,185,186,187,188,189,189,190,191,192,193,194,194,195,196,197,198,198,199,200,201,202,202,203,204,205,206,207,
  0,  0,  1,  2,  3,  4,  4,  5,  6,  7,  8,  8,  9, 10, 11, 12, 13, 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 30, 31, 32, 33, 34, 35, 35, 36, 37, 38, 39, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 53, 54, 55, 56, 57, 57, 58, 59, 60, 61, 61, 62, 63, 64, 65, 66, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 92, 93, 94, 95, 96, 97, 97, 98, 99,100,101,101,102,103,104,105,106,106,107,108,109,110,110,111,112,113,114,115,115,116,117,118,119,119,120,121,122,123,123,124,125,126,127,128,128,129,130,131,132,132,133,134,135,136,137,137,138,139,140,141,141,142,143,144,145,146,146,147,148,149,150,150,151,152,153,154,154,155,156,157,158,159,159,160,161,162,163,163,164,165,166,167,168,168,169,170,171,172,172,173,174,175,176,177,177,178,179,180,181,181,182,183,184,185,185,186,187,188,189,190,190,191,192,193,194,194,195,196,197,198,199,199,200,201,202,203,203,204,205,206,207,208,
  0,  0,  1,  2,  3,  4,  4,  5,  6,  7,  8,  9,  9, 10, 11, 12, 13, 13, 14, 15, 16, 17, 18, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 27, 27, 28, 29, 30, 31, 31, 32, 33, 34, 35, 36, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 59, 59, 60, 61, 62, 63, 63, 64, 65, 66, 67, 68, 68, 69, 70, 71, 72, 72, 73, 74, 75, 76, 77, 77, 78, 79, 80, 81, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 90, 91, 92, 93, 94, 95, 95, 96, 97, 98, 99, 99,100,101,102,103,104,104,105,106,107,108,109,109,110,111,112,113,113,114,115,116,117,118,118,119,120,121,122,122,123,124,125,126,127,127,128,129,130,131,131,132,133,134,135,136,136,137,138,139,140,140,141,142,143,144,145,145,146,147,148,149,149,150,151,152,153,154,154,155,156,157,158,159,159,160,161,162,163,163,164,165,166,167,168,168,169,170,171,172,172,173,174,175,176,177,177,178,179,180,181,181,182,183,184,185,186,186,187,188,189,190,190,191,192,193,194,195,195,196,197,198,199,199,200,201,202,203,204,204,205,206,207,208,209,
  0,  0,  1,  2,  3,  4,  4,  5,  6,  7,  8,  9,  9, 10, 11, 12, 13, 14, 14, 15, 16, 17, 18, 18, 19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 93, 93, 94, 95, 96, 97, 98, 98, 99,100,101,102,102,103,104,105,106,107,107,108,109,110,111,112,112,113,114,115,116,116,117,118,119,120,121,121,122,123,124,125,126,126,127,128,129,130,130,131,132,133,134,135,135,136,137,138,139,140,140,141,142,143,144,144,145,146,147,148,149,149,150,151,152,153,154,154,155,156,157,158,158,159,160,161,162,163,163,164,165,166,167,168,168,169,170,171,172,172,173,174,175,176,177,177,178,179,180,181,182,182,183,184,185,186,186,187,188,189,190,191,191,192,193,194,195,196,196,197,198,199,200,200,201,202,203,204,205,205,206,207,208,209,210,
  0,  0,  1,  2,  3,  4,  4,  5,  6,  7,  8,  9,  9, 10, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 33, 33, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 47, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 57, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 67, 68, 69, 70, 71, 71, 72, 73, 74, 75, 76, 76, 77, 78, 79, 80, 81, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 91, 91, 92, 93, 94, 95, 95, 96, 97, 98, 99,100,100,101,102,103,104,105,105,106,107,108,109,110,110,111,112,113,114,115,115,116,117,118,119,119,120,121,122,123,124,124,125,126,127,128,129,129,130,131,132,133,134,134,135,136,137,138,139,139,140,141,142,143,143,144,145,146,147,148,148,149,150,151,152,153,153,154,155,156,157,158,158,159,160,161,162,163,163,164,165,166,167,167,168,169,170,171,172,172,173,174,175,176,177,177,178,179,180,181,182,182,183,184,185,186,187,187,188,189,190,191,191,192,193,194,195,196,196,197,198,199,200,201,201,202,203,204,205,206,206,207,208,209,210,211,
  0,  0,  1,  2,  3,  4,  4,  5,  6,  7,  8,  9,  9, 10, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33, 34, 34, 35, 36, 37, 38, 39, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 59, 59, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 69, 69, 70, 71, 72, 73, 73, 74, 75, 76, 77, 78, 78, 79, 80, 81, 82, 83, 83, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 93, 93, 94, 95, 96, 97, 98, 98, 99,100,101,102,103,103,104,105,106,107,108,108,109,110,111,112,113,113,114,115,116,117,118,118,119,120,121,122,123,123,124,125,126,127,128,128,129,130,131,132,133,133,134,135,136,137,138,138,139,140,141,142,142,143,144,145,146,147,147,148,149,150,151,152,152,153,154,155,156,157,157,158,159,160,161,162,162,163,164,165,166,167,167,168,169,170,171,172,172,173,174,175,176,177,177,178,179,180,181,182,182,183,184,185,186,187,187,188,189,190,191,192,192,193,194,195,196,197,197,198,199,200,201,202,202,203,204,205,206,207,207,208,209,210,211,212,
  0,  0,  1,  2,  3,  4,  5,  5,  6,  7,  8,  9, 10, 10, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 29, 30, 30, 31, 32, 33, 34, 35, 35, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45, 45, 46, 47, 48, 49, 50, 50, 51, 52, 53, 54, 55, 55, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 70, 71, 71, 72, 73, 74, 75, 76, 76, 77, 78, 79, 80, 81, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 91, 91, 92, 93, 94, 95, 96, 96, 97, 98, 99,100,101,101,102,103,104,105,106,106,107,108,109,110,111,111,112,113,114,115,116,116,117,118,119,120,121,121,122,123,124,125,126,126,127,128,129,130,131,131,132,133,134,135,136,136,137,138,139,140,141,142,142,143,144,145,146,147,147,148,149,150,151,152,152,153,154,155,156,157,157,158,159,160,161,162,162,163,164,165,166,167,167,168,169,170,171,172,172,173,174,175,176,177,177,178,179,180,181,182,182,183,184,185,186,187,187,188,189,190,191,192,192,193,194,195,196,197,197,198,199,200,201,202,202,203,204,205,206,207,207,208,209,210,211,212,213,
  0,  0,  1,  2,  3,  4,  5,  5,  6,  7,  8,  9, 10, 10, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 31, 31, 32, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 57, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 67, 68, 69, 70, 71, 72, 73, 73, 74, 75, 76, 77, 78, 78, 79, 80, 81, 82, 83, 83, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 93, 93, 94, 95, 96, 97, 98, 99, 99,100,101,102,103,104,104,105,106,107,108,109,109,110,111,112,113,114,114,115,116,117,118,119,120,120,121,122,123,124,125,125,126,127,128,129,130,130,131,132,133,134,135,135,136,137,138,139,140,140,141,142,143,144,145,146,146,147,148,149,150,151,151,152,153,154,155,156,156,157,158,159,160,161,161,162,163,164,165,166,167,167,168,169,170,171,172,172,173,174,175,176,177,177,178,179,180,181,182,182,183,184,185,186,187,187,188,189,190,191,192,193,193,194,195,196,197,198,198,199,200,201,202,203,203,204,205,206,207,208,208,209,210,211,212,213,214,
  0,  0,  1,  2,  3,  4,  5,  5,  6,  7,  8,  9, 10, 10, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 21, 21, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 53, 54, 55, 56, 57, 58, 59, 59, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 69, 69, 70, 71, 72, 73, 74, 75, 75, 76, 77, 78, 79, 80, 80, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 91, 91, 92, 93, 94, 95, 96, 96, 97, 98, 99,100,101,102,102,103,104,105,106,107,107,108,109,110,111,112,112,113,114,115,116,117,118,118,119,120,121,122,123,123,124,125,126,127,128,129,129,130,131,132,133,134,134,135,136,137,138,139,139,140,141,142,143,144,145,145,146,147,148,149,150,150,151,152,153,154,155,155,156,157,158,159,160,161,161,162,163,164,165,166,166,167,168,169,170,171,172,172,173,174,175,176,177,177,178,179,180,181,182,182,183,184,185,186,187,188,188,189,190,191,192,193,193,194,195,196,197,198,198,199,200,201,202,203,204,204,205,206,207,208,209,209,210,211,212,213,214,215,
  0,  0,  1,  2,  3,  4,  5,  5,  6,  7,  8,  9, 10, 11, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 27, 27, 28, 29, 30, 31, 32, 33, 33, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 55, 55, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 65, 66, 66, 67, 68, 69, 70, 71, 72, 72, 73, 74, 75, 76, 77, 77, 78, 79, 80, 81, 82, 83, 83, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 99, 99,100,101,102,103,104,105,105,106,107,108,109,110,110,111,112,113,114,115,116,116,117,118,119,120,121,121,122,123,124,125,126,127,127,128,129,130,131,132,132,133,134,135,136,137,138,138,139,140,141,142,143,144,144,145,146,147,148,149,149,150,151,152,153,154,155,155,156,157,158,159,160,160,161,162,163,164,165,166,166,167,168,169,170,171,171,172,173,174,175,176,177,177,178,179,180,181,182,182,183,184,185,186,187,188,188,189,190,191,192,193,193,194,195,196,197,198,199,199,200,201,202,203,204,204,205,206,207,208,209,210,210,211,212,213,214,215,216,
  0,  0,  1,  2,  3,  4,  5,  5,  6,  7,  8,  9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 33, 34, 34, 35, 36, 37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 45, 46, 47, 48, 49, 50, 51, 51, 52, 53, 54, 55, 56, 57, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 68, 69, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 85, 85, 86, 87, 88, 89, 90, 91, 91, 92, 93, 94, 95, 96, 97, 97, 98, 99,100,101,102,102,103,104,105,106,107,108,108,109,110,111,112,113,114,114,115,116,117,118,119,119,120,121,122,123,124,125,125,126,127,128,129,130,131,131,132,133,134,135,136,137,137,138,139,140,141,142,142,143,144,145,146,147,148,148,149,150,151,152,153,154,154,155,156,157,158,159,159,160,161,162,163,164,165,165,166,167,168,169,170,171,171,172,173,174,175,176,177,177,178,179,180,181,182,182,183,184,185,186,187,188,188,189,190,191,192,193,194,194,195,196,197,198,199,199,200,201,202,203,204,205,205,206,207,208,209,210,211,211,212,213,214,215,216,217,
  0,  0,  1,  2,  3,  4,  5,  5,  6,  7,  8,  9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33, 34, 35, 35, 36, 37, 38, 39, 40, 41, 41, 42, 43, 44, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53, 53, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 76, 76, 77, 78, 79, 80, 81, 82, 82, 83, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 99,100,100,101,102,103,104,105,106,106,107,108,109,110,111,111,112,113,114,115,116,117,117,118,119,120,121,122,123,123,124,125,126,127,128,129,129,130,131,132,133,134,135,135,136,137,138,139,140,141,141,142,143,144,145,146,147,147,148,149,150,151,152,153,153,154,155,156,157,158,159,159,160,161,162,163,164,164,165,166,167,168,169,170,170,171,172,173,174,175,176,176,177,178,179,180,181,182,182,183,184,185,186,187,188,188,189,190,191,192,193,194,194,195,196,197,198,199,200,200,201,202,203,204,205,206,206,207,208,209,210,211,212,212,213,214,215,216,217,218,
  0,  0,  1,  2,  3,  4,  5,  6,  6,  7,  8,  9, 10, 11, 12, 12, 13, 14, 15, 16, 17, 18, 18, 19, 20, 21, 22, 23, 24, 24, 25, 26, 27, 28, 29, 30, 30, 31, 32, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 65, 66, 66, 67, 68, 69, 70, 71, 72, 73, 73, 74, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 85, 85, 86, 87, 88, 89, 90, 91, 91, 92, 93, 94, 95, 96, 97, 97, 98, 99,100,101,102,103,103,104,105,106,107,108,109,109,110,111,112,113,114,115,115,116,117,118,119,120,121,121,122,123,124,125,126,127,127,128,129,130,131,132,133,133,134,135,136,137,138,139,139,140,141,142,143,144,145,146,146,147,148,149,150,151,152,152,153,154,155,156,157,158,158,159,160,161,162,163,164,164,165,166,167,168,169,170,170,171,172,173,174,175,176,176,177,178,179,180,181,182,182,183,184,185,186,187,188,188,189,190,191,192,193,194,194,195,196,197,198,199,200,200,201,202,203,204,205,206,206,207,208,209,210,211,212,212,213,214,215,216,217,218,219,
  0,  0,  1,  2,  3,  4,  5,  6,  6,  7,  8,  9, 10, 11, 12, 12, 13, 14, 15, 16, 17, 18, 18, 19, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 29, 30, 31, 31, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 50, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 69, 70, 71, 72, 73, 74, 75, 75, 76, 77, 78, 79, 80, 81, 81, 82, 83, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 99,100,100,101,102,103,104,105,106,106,107,108,109,110,111,112,113,113,114,115,116,117,118,119,119,120,121,122,123,124,125,125,126,127,128,129,130,131,132,132,133,134,135,136,137,138,138,139,140,141,142,143,144,144,145,146,147,148,149,150,150,151,152,153,154,155,156,157,157,158,159,160,161,162,163,163,164,165,166,167,168,169,169,170,171,172,173,174,175,176,176,177,178,179,180,181,182,182,183,184,185,186,187,188,188,189,190,191,192,193,194,194,195,196,197,198,199,200,201,201,202,203,204,205,206,207,207,208,209,210,211,212,213,213,214,215,216,217,218,219,220,
  0,  0,  1,  2,  3,  4,  5,  6,  6,  7,  8,  9, 10, 11, 12, 13, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 45, 46, 47, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 70, 71, 71, 72, 73, 74, 75, 76, 77, 78, 78, 79, 80, 81, 82, 83, 84, 84, 85, 86, 87, 88, 89, 90, 91, 91, 92, 93, 94, 95, 96, 97, 97, 98, 99,100,101,102,103,104,104,105,106,107,108,109,110,110,111,112,113,114,115,116,117,117,118,119,120,121,122,123,123,124,125,126,127,128,129,130,130,131,132,133,134,135,136,136,137,138,139,140,141,142,143,143,144,145,146,147,148,149,149,150,151,152,153,154,155,156,156,157,158,159,160,161,162,162,163,164,165,166,167,168,169,169,170,171,172,173,174,175,175,176,177,178,179,180,181,182,182,183,184,185,186,187,188,188,189,190,191,192,193,194,195,195,196,197,198,199,200,201,201,202,203,204,205,206,207,208,208,209,210,211,212,213,214,214,215,216,217,218,219,220,221,
  0,  0,  1,  2,  3,  4,  5,  6,  6,  7,  8,  9, 10, 11, 12, 13, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 31, 32, 33, 33, 34, 35, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53, 53, 54, 55, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 65, 66, 67, 67, 68, 69, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 79, 80, 80, 81, 82, 83, 84, 85, 86, 87, 87, 88, 89, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 99,100,100,101,102,103,104,105,106,107,107,108,109,110,111,112,113,114,114,115,116,117,118,119,120,121,121,122,123,124,125,126,127,127,128,129,130,131,132,133,134,134,135,136,137,138,139,140,141,141,142,143,144,145,146,147,148,148,149,150,151,152,153,154,154,155,156,157,158,159,160,161,161,162,163,164,165,166,167,168,168,169,170,171,172,173,174,174,175,176,177,178,179,180,181,181,182,183,184,185,186,187,188,188,189,190,191,192,193,194,195,195,196,197,198,199,200,201,201,202,203,204,205,206,207,208,208,209,210,211,212,213,214,215,215,216,217,218,219,220,221,222,
  0,  0,  1,  2,  3,  4,  5,  6,  6,  7,  8,  9, 10, 11, 12, 13, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 27, 27, 28, 29, 30, 31, 32, 33, 34, 34, 35, 36, 37, 38, 39, 40, 41, 41, 42, 43, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 54, 55, 55, 56, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 69, 70, 71, 72, 73, 74, 75, 76, 76, 77, 78, 79, 80, 81, 82, 83, 83, 84, 85, 86, 87, 88, 89, 90, 90, 91, 92, 93, 94, 95, 96, 97, 97, 98, 99,100,101,102,103,104,104,105,106,107,108,109,110,111,111,112,113,114,115,116,117,118,118,119,120,121,122,123,124,125,125,126,127,128,129,130,131,132,132,133,134,135,136,137,138,139,139,140,141,142,143,144,145,146,146,147,148,149,150,151,152,153,153,154,155,156,157,158,159,160,160,161,162,163,164,165,166,167,167,168,169,170,171,172,173,174,174,175,176,177,178,179,180,181,181,182,183,184,185,186,187,188,188,189,190,191,192,193,194,195,195,196,197,198,199,200,201,202,202,203,204,205,206,207,208,209,209,210,211,212,213,214,215,216,216,217,218,219,220,221,222,223,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  7,  8,  9, 10, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 20, 21, 21, 22, 23, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 43, 44, 45, 46, 47, 48, 49, 50, 50, 51, 52, 53, 54, 55, 56, 57, 57, 58, 59, 60, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 70, 71, 72, 72, 73, 74, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 91, 92, 93, 93, 94, 95, 96, 97, 98, 99,100,101,101,102,103,104,105,106,107,108,108,109,110,111,112,113,114,115,115,116,117,118,119,120,121,122,122,123,124,125,126,127,128,129,130,130,131,132,133,134,135,136,137,137,138,139,140,141,142,143,144,144,145,146,147,148,149,150,151,151,152,153,154,155,156,157,158,158,159,160,161,162,163,164,165,166,166,167,168,169,170,171,172,173,173,174,175,176,177,178,179,180,180,181,182,183,184,185,186,187,187,188,189,190,191,192,193,194,195,195,196,197,198,199,200,201,202,202,203,204,205,206,207,208,209,209,210,211,212,213,214,215,216,216,217,218,219,220,221,222,223,224,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  7,  8,  9, 10, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 27, 28, 29, 30, 30, 31, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 43, 44, 45, 45, 46, 47, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 65, 66, 67, 67, 68, 69, 70, 71, 72, 73, 74, 75, 75, 76, 77, 78, 79, 80, 81, 82, 82, 83, 84, 85, 86, 87, 88, 89, 90, 90, 91, 92, 93, 94, 95, 96, 97, 97, 98, 99,100,101,102,103,104,105,105,106,107,108,109,110,111,112,112,113,114,115,116,117,118,119,120,120,121,122,123,124,125,126,127,127,128,129,130,131,132,133,134,135,135,136,137,138,139,140,141,142,142,143,144,145,146,147,148,149,150,150,151,152,153,154,155,156,157,157,158,159,160,161,162,163,164,165,165,166,167,168,169,170,171,172,172,173,174,175,176,177,178,179,180,180,181,182,183,184,185,186,187,187,188,189,190,191,192,193,194,195,195,196,197,198,199,200,201,202,202,203,204,205,206,207,208,209,210,210,211,212,213,214,215,216,217,217,218,219,220,221,222,223,224,225,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  7,  8,  9, 10, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 32, 33, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 76, 77, 77, 78, 79, 80, 81, 82, 83, 84, 85, 85, 86, 87, 88, 89, 90, 91, 92, 93, 93, 94, 95, 96, 97, 98, 99,100,101,101,102,103,104,105,106,107,108,109,109,110,111,112,113,114,115,116,116,117,118,119,120,121,122,123,124,124,125,126,127,128,129,130,131,132,132,133,134,135,136,137,138,139,140,140,141,142,143,144,145,146,147,148,148,149,150,151,152,153,154,155,155,156,157,158,159,160,161,162,163,163,164,165,166,167,168,169,170,171,171,172,173,174,175,176,177,178,179,179,180,181,182,183,184,185,186,187,187,188,189,190,191,192,193,194,194,195,196,197,198,199,200,201,202,202,203,204,205,206,207,208,209,210,210,211,212,213,214,215,216,217,218,218,219,220,221,222,223,224,225,226,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9, 10, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24, 24, 25, 26, 27, 28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 69, 70, 71, 72, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 81, 82, 83, 84, 85, 86, 87, 88, 89, 89, 90, 91, 92, 93, 94, 95, 96, 97, 97, 98, 99,100,101,102,103,104,105,105,106,107,108,109,110,111,112,113,113,114,115,116,117,118,119,120,121,121,122,123,124,125,126,127,128,129,129,130,131,132,133,134,135,136,137,137,138,139,140,141,142,143,144,145,145,146,147,148,149,150,151,152,153,154,154,155,156,157,158,159,160,161,162,162,163,164,165,166,167,168,169,170,170,171,172,173,174,175,176,177,178,178,179,180,181,182,183,184,185,186,186,187,188,189,190,191,192,193,194,194,195,196,197,198,199,200,201,202,202,203,204,205,206,207,208,209,210,210,211,212,213,214,215,216,217,218,218,219,220,221,222,223,224,225,226,227,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9, 10, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 29, 30, 31, 32, 33, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 59, 60, 61, 62, 63, 64, 65, 66, 67, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 76, 77, 78, 79, 80, 81, 82, 83, 84, 84, 85, 86, 87, 88, 89, 90, 91, 92, 92, 93, 94, 95, 96, 97, 98, 99,100,101,101,102,103,104,105,106,107,108,109,109,110,111,112,113,114,115,116,117,118,118,119,120,121,122,123,124,125,126,126,127,128,129,130,131,132,133,134,135,135,136,137,138,139,140,141,142,143,143,144,145,146,147,148,149,150,151,152,152,153,154,155,156,157,158,159,160,160,161,162,163,164,165,166,167,168,168,169,170,171,172,173,174,175,176,177,177,178,179,180,181,182,183,184,185,185,186,187,188,189,190,191,192,193,194,194,195,196,197,198,199,200,201,202,202,203,204,205,206,207,208,209,210,211,211,212,213,214,215,216,217,218,219,219,220,221,222,223,224,225,226,227,228,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 93, 94, 95, 96, 96, 97, 98, 99,100,101,102,103,104,105,105,106,107,108,109,110,111,112,113,114,114,115,116,117,118,119,120,121,122,123,123,124,125,126,127,128,129,130,131,132,132,133,134,135,136,137,138,139,140,140,141,142,143,144,145,146,147,148,149,149,150,151,152,153,154,155,156,157,158,158,159,160,161,162,163,164,165,166,167,167,168,169,170,171,172,173,174,175,176,176,177,178,179,180,181,182,183,184,184,185,186,187,188,189,190,191,192,193,193,194,195,196,197,198,199,200,201,202,202,203,204,205,206,207,208,209,210,211,211,212,213,214,215,216,217,218,219,220,220,221,222,223,224,225,226,227,228,229,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 92, 93, 94, 95, 96, 97, 98, 99,100,101,101,102,103,104,105,106,107,108,109,110,110,111,112,113,114,115,116,117,118,119,119,120,121,122,123,124,125,126,127,128,128,129,130,131,132,133,134,135,136,137,138,138,139,140,141,142,143,144,145,146,147,147,148,149,150,151,152,153,154,155,156,156,157,158,159,160,161,162,163,164,165,165,166,167,168,169,170,171,172,173,174,174,175,176,177,178,179,180,181,182,183,184,184,185,186,187,188,189,190,191,192,193,193,194,195,196,197,198,199,200,201,202,202,203,204,205,206,207,208,209,210,211,211,212,213,214,215,216,217,218,219,220,220,221,222,223,224,225,226,227,228,229,230,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 96, 97, 98, 99,100,101,102,103,104,105,105,106,107,108,109,110,111,112,113,114,115,115,116,117,118,119,120,121,122,123,124,125,125,126,127,128,129,130,131,132,133,134,134,135,136,137,138,139,140,141,142,143,144,144,145,146,147,148,149,150,151,152,153,154,154,155,156,157,158,159,160,161,162,163,163,164,165,166,167,168,169,170,171,172,173,173,174,175,176,177,178,179,180,181,182,182,183,184,185,186,187,188,189,190,191,192,192,193,194,195,196,197,198,199,200,201,202,202,203,204,205,206,207,208,209,210,211,211,212,213,214,215,216,217,218,219,220,221,221,222,223,224,225,226,227,228,229,230,231,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,100,101,102,103,104,105,106,107,108,109,110,110,111,112,113,114,115,116,117,118,119,120,121,121,122,123,124,125,126,127,128,129,130,131,131,132,133,134,135,136,137,138,139,140,141,141,142,143,144,145,146,147,148,149,150,151,151,152,153,154,155,156,157,158,159,160,161,161,162,163,164,165,166,167,168,169,170,171,171,172,173,174,175,176,177,178,179,180,181,181,182,183,184,185,186,187,188,189,190,191,191,192,193,194,195,196,197,198,199,200,201,201,202,203,204,205,206,207,208,209,210,211,211,212,213,214,215,216,217,218,219,220,221,221,222,223,224,225,226,227,228,229,230,231,232,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 95, 96, 97, 98, 99,100,101,102,103,104,105,105,106,107,108,109,110,111,112,113,114,115,116,116,117,118,119,120,121,122,123,124,125,126,127,127,128,129,130,131,132,133,134,135,136,137,137,138,139,140,141,142,143,144,145,146,147,148,148,149,150,151,152,153,154,155,156,157,158,158,159,160,161,162,163,164,165,166,167,168,169,169,170,171,172,173,174,175,176,177,178,179,180,180,181,182,183,184,185,186,187,188,189,190,190,191,192,193,194,195,196,197,198,199,200,201,201,202,203,204,205,206,207,208,209,210,211,211,212,213,214,215,216,217,218,219,220,221,222,222,223,224,225,226,227,228,229,230,231,232,233,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,100,101,102,103,104,105,106,107,108,109,110,111,111,112,113,114,115,116,117,118,119,120,121,122,122,123,124,125,126,127,128,129,130,131,132,133,133,134,135,136,137,138,139,140,141,142,143,144,144,145,146,147,148,149,150,151,152,153,154,155,156,156,157,158,159,160,161,162,163,164,165,166,167,167,168,169,170,171,172,173,174,175,176,177,178,178,179,180,181,182,183,184,185,186,187,188,189,189,190,191,192,193,194,195,196,197,198,199,200,200,201,202,203,204,205,206,207,208,209,210,211,211,212,213,214,215,216,217,218,219,220,221,222,222,223,224,225,226,227,228,229,230,231,232,233,234,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,105,106,107,108,109,110,111,112,113,114,115,116,117,117,118,119,120,121,122,123,124,125,126,127,128,129,129,130,131,132,133,134,135,136,137,138,139,140,141,141,142,143,144,145,146,147,148,149,150,151,152,152,153,154,155,156,157,158,159,160,161,162,163,164,164,165,166,167,168,169,170,171,172,173,174,175,176,176,177,178,179,180,181,182,183,184,185,186,187,188,188,189,190,191,192,193,194,195,196,197,198,199,199,200,201,202,203,204,205,206,207,208,209,210,211,211,212,213,214,215,216,217,218,219,220,221,222,223,223,224,225,226,227,228,229,230,231,232,233,234,235,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 99,100,101,102,103,104,105,106,107,108,109,110,111,111,112,113,114,115,116,117,118,119,120,121,122,123,124,124,125,126,127,128,129,130,131,132,133,134,135,136,136,137,138,139,140,141,142,143,144,145,146,147,148,149,149,150,151,152,153,154,155,156,157,158,159,160,161,161,162,163,164,165,166,167,168,169,170,171,172,173,173,174,175,176,177,178,179,180,181,182,183,184,185,186,186,187,188,189,190,191,192,193,194,195,196,197,198,198,199,200,201,202,203,204,205,206,207,208,209,210,211,211,212,213,214,215,216,217,218,219,220,221,222,223,223,224,225,226,227,228,229,230,231,232,233,234,235,236,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,105,106,107,108,109,110,111,112,113,114,115,116,117,118,118,119,120,121,122,123,124,125,126,127,128,129,130,131,131,132,133,134,135,136,137,138,139,140,141,142,143,144,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,158,159,160,161,162,163,164,165,166,167,168,169,170,171,171,172,173,174,175,176,177,178,179,180,181,182,183,184,184,185,186,187,188,189,190,191,192,193,194,195,196,197,197,198,199,200,201,202,203,204,205,206,207,208,209,210,210,211,212,213,214,215,216,217,218,219,220,221,222,223,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,
  0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
};

#endif // ALPHA_BLEND_METHOD == LOOKUP_TABLE_ALPHA_BLEND_METHOD


// ****************************************************************************
//
//  Function Name:	void RImageLibrary::AlphaBlend(
//								const RBitmapImage&	rSrcBitmap,		const RIntRect&	srcRect,
//								RBitmapImage&			rDstBitmap,		const RIntRect&	dstRect,
//								const RBitmapImage&	rAlphaBitmap,	const RIntRect&	alphaRect) const
//
//  Description:		Blend (with transparency) srcRect pixels in 24-bit
//							rSrcBitmap, thru alphaRect pixels in 8-bit alpha mask
//							rAlphaBitmap, into dstRect pixels in 24-bit rDstBitmap.
//
//							All bitmaps are assumed to be the pixel depths indicated,
//							srcRect, dstRect and alphaRect are assumed to be the same
//							size and all bitmaps are assumed to be at least as big as
//							the lower-right extent of srcRect, dstRect and alphaRect.
//
//  Returns:			None
//
//  Exceptions:		None
//
//  Author:				Greg Beddow
//
// ****************************************************************************
//
void RImageLibrary::AlphaBlend(
	const RBitmapImage&		rSrcBitmap,		const RIntRect&	srcRect,
	RBitmapImage&				rDstBitmap,		const RIntRect&	dstRect,
	const RBitmapImage&		rAlphaBitmap,	const RIntRect&	alphaRect ) const
{
	uLONG width = srcRect.Width();
	uLONG height = srcRect.Height();

	// make sure:
	// srcRect,dstRect,alphaRect width and height are positive and same size,
	// rSrcBitmap and rDstBitmap are 24 bits deep, rAlphaBitmap is 8 bits deep,
	// all bitmaps >= lower-right extents of srcRect,dstRect,alphaRect
	TpsAssert( srcRect.Width() >= 0 && srcRect.Height() >= 0, "Negative srcRect width or height!" );
	TpsAssert( dstRect.Width() >= 0 && dstRect.Height() >= 0, "Negative dstRect width or height!" );
	TpsAssert( width == (uLONG)dstRect.Width() && height == (uLONG)dstRect.Height(), "dstRect not same size as srcRect." );
	TpsAssert( width == (uLONG)alphaRect.Width() && height == (uLONG)alphaRect.Height(), "alphaRect not same size as srcRect." );
	TpsAssert( alphaRect.Width() >= 0 && alphaRect.Height() >= 0, "Negative alphaRect width or height!" );
	TpsAssert( rSrcBitmap.GetBitDepth() == 24, "rSrcBitmap not 24 bits deep." );
	TpsAssert( rDstBitmap.GetBitDepth() == 24, "rDstBitmap not 24 bits deep." );
	TpsAssert( rAlphaBitmap.GetBitDepth() == 8, "rAlphaBitmap not 8 bits deep." );
	TpsAssert( srcRect.m_Right <= (YIntCoordinate)rSrcBitmap.GetWidthInPixels() && srcRect.m_Bottom <= (YIntCoordinate)rSrcBitmap.GetHeightInPixels(), "rSrcBitmap too small for srcRect." );
	TpsAssert( dstRect.m_Right <= (YIntCoordinate)rDstBitmap.GetWidthInPixels() && dstRect.m_Bottom <= (YIntCoordinate)rDstBitmap.GetHeightInPixels(), "rDstBitmap too small for dstRect." );
	TpsAssert( alphaRect.m_Right <= (YIntCoordinate)rAlphaBitmap.GetWidthInPixels() && alphaRect.m_Bottom <= (YIntCoordinate)rAlphaBitmap.GetHeightInPixels(), "rAlphaBitmap too small for alphaRect." );

	uLONG srcBytesPerRow = RBitmapImage::GetBytesPerRow( rSrcBitmap.GetWidthInPixels(), 24 );
	uLONG dstBytesPerRow = RBitmapImage::GetBytesPerRow( rDstBitmap.GetWidthInPixels(), 24 );
	uLONG alphaBytesPerRow = RBitmapImage::GetBytesPerRow( rAlphaBitmap.GetWidthInPixels(), 8 );

	// get pointers to source, destination and alpha image data
	// if biHeight field is >=0 data is bottom-up, else top-down

	void* pSrcRawData = rSrcBitmap.GetRawData();
	const uBYTE* pSrcRow = (uBYTE*)RBitmapImage::GetImageData( pSrcRawData );
	if (((BITMAPINFOHEADER*)pSrcRawData)->biHeight >= 0)
		pSrcRow += ((rSrcBitmap.GetHeightInPixels() - srcRect.m_Bottom) * srcBytesPerRow) + (3 * srcRect.m_Left);
	else
		pSrcRow += (srcRect.m_Top * srcBytesPerRow) + (3 * srcRect.m_Left);

	void* pDstRawData = rDstBitmap.GetRawData();
	uBYTE* pDstRow = (uBYTE*)RBitmapImage::GetImageData( pDstRawData );
	if (((BITMAPINFOHEADER*)pDstRawData)->biHeight >= 0)
		pDstRow += ((rDstBitmap.GetHeightInPixels() - dstRect.m_Bottom) * dstBytesPerRow) + (3 * dstRect.m_Left);
	else
		pDstRow += (dstRect.m_Top * dstBytesPerRow) + (3 * dstRect.m_Left);

	void* pAlphaRawData = rAlphaBitmap.GetRawData();
	const uBYTE* pAlphaRow = (uBYTE*)RBitmapImage::GetImageData( pAlphaRawData );
	if (((BITMAPINFOHEADER*)pAlphaRawData)->biHeight >= 0)
		pAlphaRow += ((rAlphaBitmap.GetHeightInPixels() - alphaRect.m_Bottom) * alphaBytesPerRow) + alphaRect.m_Left;
	else
		pAlphaRow += (alphaRect.m_Top * alphaBytesPerRow) + alphaRect.m_Left;

	for (uLONG row = 0; row < height; row++)
	{
		RCursor().SpinCursor();

		const uBYTE* pSrc = pSrcRow;
		uBYTE* pDst = pDstRow;
		const uBYTE* pAlpha = pAlphaRow;

		for (uLONG column = 0; column < width; column++)
		{
			uLONG alpha = *pAlpha++;

#if ALPHA_BLEND_METHOD == LOOKUP_TABLE_ALPHA_BLEND_METHOD
			// the LOOKUP TABLE method:
			// uses a 64K table and is as fast as the bitshift method but
			// is the most accurate (same accuracy as slower divide method)
			uLONG alphaComplement = 0xFF - alpha;
			const uBYTE* lut     = &alphalut[alpha][0];
			const uBYTE* lutComp = &alphalut[alphaComplement][0];
			*pDst = lut[*pSrc++] + lutComp[*pDst];
			pDst++;
			*pDst = lut[*pSrc++] + lutComp[*pDst];
			pDst++;
			*pDst = lut[*pSrc++] + lutComp[*pDst];
			pDst++;
#elif ALPHA_BLEND_METHOD == SHIFT_ALPHA_BLEND_METHOD
			// the SHIFT method:
			// right-shift 8 does a divide by 256 instead of by 255
			// which is much faster than real divide (but same speed as lookup table)
			// and the error of roughly 1 part in 256 only occurs in the
			// semi-transparent areas where it's somewhat difficult to detect
			if (alpha == 0)				// source is fully transparent
				pDst += 3, pSrc += 3;
			else if (alpha == 0xFF)		// source is fully opaque
				*pDst++ = *pSrc++, *pDst++ = *pSrc++, *pDst++ = *pSrc++;
			else								// source is semi-transparent
			{
				uLONG alphaComplement = 0xFF - alpha;
				*pDst = ((*pSrc++ * alpha) >> 8) + ((*pDst * alphaComplement) >> 8);
				pDst++;
				*pDst = ((*pSrc++ * alpha) >> 8) + ((*pDst * alphaComplement) >> 8);
				pDst++;
				*pDst = ((*pSrc++ * alpha) >> 8) + ((*pDst * alphaComplement) >> 8);
				pDst++;
			}
#elif ALPHA_BLEND_METHOD == DIVIDE_ALPHA_BLEND_METHOD
			// the DIVIDE method:
			// much slower than bitshift but the most accurate
			// (but same accuracy as faster lookup table method)
			if (alpha == 0)				// source is fully transparent
				pDst += 3, pSrc += 3;
			else if (alpha == 0xFF)		// source is fully opaque
				*pDst++ = *pSrc++, *pDst++ = *pSrc++, *pDst++ = *pSrc++;
			else								// source is semi-transparent
			{
				uLONG alphaComplement = 0xFF - alpha;
				*pDst = ((*pSrc++ * alpha) / 255) + ((*pDst * alphaComplement) / 255);
				pDst++;
				*pDst = ((*pSrc++ * alpha) / 255) + ((*pDst * alphaComplement) / 255);
				pDst++;
				*pDst = ((*pSrc++ * alpha) / 255) + ((*pDst * alphaComplement) / 255);
				pDst++;
			}
#else
			TpsAssertAlways( "Invalid ALPHA_BLEND_METHOD" );
#endif
		}

		pSrcRow += srcBytesPerRow;
		pDstRow += dstBytesPerRow;
		pAlphaRow += alphaBytesPerRow;
	}
}

// ****************************************************************************
//
//  Function Name:	void RImageLibrary::AlphaBlend(
//								const RSolidColor&	solidColor,
//								RBitmapImage&			rDstBitmap,		const RIntRect&	dstRect,
//								const RBitmapImage&	rAlphaBitmap,	const RIntRect&	alphaRect) const
//
//  Description:		Blend (with transparency) solidColor thru alphaRect pixels
//							in 8-bit alpha mask rAlphaBitmap, into dstRect pixels in
//							8-bit or 24-bit rDstBitmap. If rDstBitmap is 8 bits deep
//							solidColor and rDstBitmap are both assumed to be monchrome.
//							If rDstBitmap is 24 bits deep solidColor and rDstBitmap are
//							both assumed to be full color.
//
//							All bitmaps are assumed to be the pixel depths indicated,
//							srcRect, dstRect and alphaRect are assumed to be the same
//							size and all bitmaps are assumed to be at least as big as
//							the lower-right extent of srcRect, dstRect and alphaRect.
//
//  Returns:			None
//
//  Exceptions:		None
//
//  Author:				Greg Beddow
//
// ****************************************************************************
//
void RImageLibrary::AlphaBlend(
	const RSolidColor&		solidColor,
	RBitmapImage&				rDstBitmap,		const RIntRect&	dstRect,
	const RBitmapImage&		rAlphaBitmap,	const RIntRect&	alphaRect ) const
{
	uLONG width = dstRect.Width();
	uLONG height = dstRect.Height();
	uLONG dstDepth = rDstBitmap.GetBitDepth();
	uLONG dstBytesPerPixel = (dstDepth == 8 ? 1 : 3);

	// make sure:
	// dstRect,alphaRect width and height are positive and same size,
	// rDstBitmap is 8 or 24 bits deep, rAlphaBitmap is 8 bits deep,
	// all bitmaps >= lower-right extents of dstRect,alphaRect
	TpsAssert( dstRect.Width() >= 0 && dstRect.Height() >= 0, "Negative dstRect width or height!" );
	TpsAssert( width == (uLONG)alphaRect.Width() && height == (uLONG)alphaRect.Height(), "alphaRect not same size as dstRect." );
	TpsAssert( alphaRect.Width() >= 0 && alphaRect.Height() >= 0, "Negative alphaRect width or height!" );
	TpsAssert( dstDepth == 8 || dstDepth == 24, "rDstBitmap not 8 or 24 bits deep." );
	TpsAssert( rAlphaBitmap.GetBitDepth() == 8, "rAlphaBitmap not 8 bits deep." );
	TpsAssert( dstRect.m_Right <= (YIntCoordinate)rDstBitmap.GetWidthInPixels() && dstRect.m_Bottom <= (YIntCoordinate)rDstBitmap.GetHeightInPixels(), "rDstBitmap too small for dstRect." );
	TpsAssert( alphaRect.m_Right <= (YIntCoordinate)rAlphaBitmap.GetWidthInPixels() && alphaRect.m_Bottom <= (YIntCoordinate)rAlphaBitmap.GetHeightInPixels(), "rAlphaBitmap too small for alphaRect." );

	uLONG dstBytesPerRow = RBitmapImage::GetBytesPerRow( rDstBitmap.GetWidthInPixels(), dstDepth );
	uLONG alphaBytesPerRow = RBitmapImage::GetBytesPerRow( rAlphaBitmap.GetWidthInPixels(), 8 );

	uBYTE srcRed = solidColor.GetRed();
	uBYTE srcGreen = solidColor.GetGreen();
	uBYTE srcBlue = solidColor.GetBlue();
	uBYTE srcMonochrome = srcRed;

	// get pointers to destination and alpha image data
	// if biHeight field is >=0 data is bottom-up, else top-down

	void* pDstRawData = rDstBitmap.GetRawData();
	uBYTE* pDstRow = (uBYTE*)RBitmapImage::GetImageData( pDstRawData );
	if (((BITMAPINFOHEADER*)pDstRawData)->biHeight >= 0)
		pDstRow += ((rDstBitmap.GetHeightInPixels() - dstRect.m_Bottom) * dstBytesPerRow) + (dstBytesPerPixel * dstRect.m_Left);
	else
		pDstRow += (dstRect.m_Top * dstBytesPerRow) + (dstBytesPerPixel * dstRect.m_Left);

	void* pAlphaRawData = rAlphaBitmap.GetRawData();
	const uBYTE* pAlphaRow = (uBYTE*)RBitmapImage::GetImageData( pAlphaRawData );
	if (((BITMAPINFOHEADER*)pAlphaRawData)->biHeight >= 0)
		pAlphaRow += ((rAlphaBitmap.GetHeightInPixels() - alphaRect.m_Bottom) * alphaBytesPerRow) + alphaRect.m_Left;
	else
		pAlphaRow += (alphaRect.m_Top * alphaBytesPerRow) + alphaRect.m_Left;

	for (uLONG row = 0; row < height; row++)
	{
		RCursor().SpinCursor();

		uBYTE* pDst = pDstRow;
		const uBYTE* pAlpha = pAlphaRow;

		for (uLONG column = 0; column < width; column++)
		{
			uLONG alpha = *pAlpha++;

#if ALPHA_BLEND_METHOD == LOOKUP_TABLE_ALPHA_BLEND_METHOD
			// the LOOKUP TABLE method:
			// average ticks (for comparison with other methods): 10 ticks
			// uses a 64K table and is as fast as the bitshift method but
			// is the most accurate (same accuracy as slower divide method)
			uLONG alphaComplement = 0xFF - alpha;
			if (dstDepth == 8)
			{
				*pDst = alphalut[srcMonochrome][alpha] + alphalut[*pDst][alphaComplement];
				pDst++;
			}
			else // dstDepth == 24
			{
				const uBYTE* lut     = &alphalut[alpha][0];
				const uBYTE* lutComp = &alphalut[alphaComplement][0];
				*pDst = lut[srcBlue] + lutComp[*pDst];
				pDst++;
				*pDst = lut[srcGreen] + lutComp[*pDst];
				pDst++;
				*pDst = lut[srcRed] + lutComp[*pDst];
				pDst++;
			}
#elif ALPHA_BLEND_METHOD == SHIFT_ALPHA_BLEND_METHOD
			// the SHIFT method:
			// average ticks (for comparison with other methods): 10 ticks
			// right-shift 8 does a divide by 256 instead of by 255
			// which is much faster than real divide (but same speed as lookup table)
			// and the error of roughly 1 part in 256 only occurs in the
			// semi-transparent areas where it's somewhat difficult to detect
			if (dstDepth == 8)
			{
				if (alpha == 0)				// source is fully transparent
					pDst++;
				else if (alpha == 0xFF)		// source is fully opaque
					*pDst++ = srcMonochrome;
				else								// source is semi-transparent
				{
					uLONG alphaComplement = 0xFF - alpha;
					*pDst = ((srcMonochrome * alpha) >> 8) + ((*pDst * alphaComplement) >> 8);
					pDst++;
				}
			}
			else // dstDepth == 24
			{
				if (alpha == 0)				// source is fully transparent
					pDst += 3;
				else if (alpha == 0xFF)		// source is fully opaque
					*pDst++ = srcBlue, *pDst++ = srcGreen, *pDst++ = srcRed;
				else								// source is semi-transparent
				{
					uLONG alphaComplement = 0xFF - alpha;
					*pDst = ((srcBlue * alpha) >> 8) + ((*pDst * alphaComplement) >> 8);
					pDst++;
					*pDst = ((srcGreen * alpha) >> 8) + ((*pDst * alphaComplement) >> 8);
					pDst++;
					*pDst = ((srcRed * alpha) >> 8) + ((*pDst * alphaComplement) >> 8);
					pDst++;
				}
			}
#elif ALPHA_BLEND_METHOD == DIVIDE_ALPHA_BLEND_METHOD
			// the DIVIDE method:
			// average ticks (for comparison with other methods): 45 ticks
			// much slower than bitshift but the most accurate
			// (but same accuracy as faster lookup table method)
			if (dstDepth == 8)
			{
				if (alpha == 0)				// source is fully transparent
					pDst++;
				else if (alpha == 0xFF)		// source is fully opaque
					*pDst++ = srcMonochrome;
				else								// source is semi-transparent
				{
					uLONG alphaComplement = 0xFF - alpha;
					*pDst = ((srcMonochrome * alpha) / 255) + ((*pDst * alphaComplement) / 255);
					pDst++;
				}
			}
			else
			{
				if (alpha == 0)				// source is fully transparent
					pDst += 3;
				else if (alpha == 0xFF)		// source is fully opaque
					*pDst++ = srcBlue, *pDst++ = srcGreen, *pDst++ = srcRed;
				else								// source is semi-transparent
				{
					uLONG alphaComplement = 0xFF - alpha;
					*pDst = ((srcBlue * alpha) / 255) + ((*pDst * alphaComplement) / 255);
					pDst++;
					*pDst = ((srcGreen * alpha) / 255) + ((*pDst * alphaComplement) / 255);
					pDst++;
					*pDst = ((srcRed * alpha) / 255) + ((*pDst * alphaComplement) / 255);
					pDst++;
				}
			}
#else
			TpsAssertAlways( "Invalid ALPHA_BLEND_METHOD" );
#endif
		}

		pDstRow += dstBytesPerRow;
		pAlphaRow += alphaBytesPerRow;
	}

#if 0
{
	// create look up table
	FILE* outFile = fopen( "alphalut.dat", "wt" );
	for (uLONG i = 0; i < 256; ++i)
	{
		for (uLONG j = 0; j < 256; ++j)
			fprintf( outFile, "%#3d,", (i * j) / 255 );
		fprintf( outFile, "\n" );
	}
	fclose( outFile );
}
#endif
}

// ****************************************************************************
//
//  Function Name:	void RImageLibrary::AlphaBlend(
//								const RBitmapImage&	rSrcBitmap, const RIntRect&	srcRect,
//								RBitmapImage&			rDstBitmap, const RIntRect&	dstRect ) const
//
//  Description:		Composite srcRect pixels from 8-bit alpha mask rSrcBitmap
//							into dstRect pixels in 8-bit alpha mask rDstBitmap.
//
//							All bitmaps are assumed to be the pixel depths indicated,
//							srcRect and dstRect are assumed to be the same size and
//							all bitmaps are assumed to be at least as big as the
//							lower-right extent of srcRect, dstRect.
//
//  Returns:			None
//
//  Exceptions:		None
//
//  Author:				Greg Beddow
//
// ****************************************************************************
//
void RImageLibrary::AlphaBlend(
	const RBitmapImage&		rSrcBitmap, const RIntRect&	srcRect,
	RBitmapImage&				rDstBitmap, const RIntRect&	dstRect ) const
{
	uLONG width = dstRect.Width();
	uLONG height = dstRect.Height();

	// make sure:
	// srcRect,dstRect width and height are positive and same size,
	// rSrcBitmap and rDstBitmap are 8 bits deep,
	// rSrcBitmap,rDstBitmap size >= lower-right extents of srcRect,dstRect
	TpsAssert( dstRect.Width() >= 0 && dstRect.Height() >= 0, "Negative dstRect width or height!" );
	TpsAssert( width == (uLONG)srcRect.Width() && height == (uLONG)srcRect.Height(), "srcRect not same size as dstRect." );
	TpsAssert( rSrcBitmap.GetBitDepth() == 8, "rSrcBitmap not 8 bits deep." );
	TpsAssert( rDstBitmap.GetBitDepth() == 8, "rDstBitmap not 8 bits deep." );
	TpsAssert( srcRect.m_Right <= (YIntCoordinate)rSrcBitmap.GetWidthInPixels() && srcRect.m_Bottom <= (YIntCoordinate)rSrcBitmap.GetHeightInPixels(), "rSrcBitmap too small for srcRect." );
	TpsAssert( dstRect.m_Right <= (YIntCoordinate)rDstBitmap.GetWidthInPixels() && dstRect.m_Bottom <= (YIntCoordinate)rDstBitmap.GetHeightInPixels(), "rDstBitmap too small for dstRect." );

	uLONG srcBytesPerRow = RBitmapImage::GetBytesPerRow( rSrcBitmap.GetWidthInPixels(), 8 );
	uLONG dstBytesPerRow = RBitmapImage::GetBytesPerRow( rDstBitmap.GetWidthInPixels(), 8 );

	// get pointers to source and destination image data
	// if biHeight field is >=0 data is bottom-up, else top-down

	void* pSrcRawData = rSrcBitmap.GetRawData();
	uBYTE* pSrcRow = (uBYTE*)RBitmapImage::GetImageData( pSrcRawData );
	if (((BITMAPINFOHEADER*)pSrcRawData)->biHeight >= 0)
		pSrcRow += ((rSrcBitmap.GetHeightInPixels() - srcRect.m_Bottom) * srcBytesPerRow) + srcRect.m_Left;
	else
		pSrcRow += (srcRect.m_Top * srcBytesPerRow) + srcRect.m_Left;

	void* pDstRawData = rDstBitmap.GetRawData();
	uBYTE* pDstRow = (uBYTE*)RBitmapImage::GetImageData( pDstRawData );
	if (((BITMAPINFOHEADER*)pDstRawData)->biHeight >= 0)
		pDstRow += ((rDstBitmap.GetHeightInPixels() - dstRect.m_Bottom) * dstBytesPerRow) + dstRect.m_Left;
	else
		pDstRow += (dstRect.m_Top * dstBytesPerRow) + dstRect.m_Left;

	for (uLONG row = 0; row < height; row++)
	{
		RCursor().SpinCursor();

		uBYTE* pDst = pDstRow;
		const uBYTE* pSrc = pSrcRow;

		for (uLONG column = 0; column < width; column++)
		{
			uLONG alpha = *pSrc++;

#if ALPHA_BLEND_METHOD == LOOKUP_TABLE_ALPHA_BLEND_METHOD
			// the LOOKUP TABLE method:
			// uses a 64K table and is as fast as the bitshift method but
			// is the most accurate (same accuracy as slower divide method)
			*pDst = alphalut[alpha][*pDst];
			pDst++;
#elif ALPHA_BLEND_METHOD == SHIFT_ALPHA_BLEND_METHOD
			// the SHIFT method:
			// right-shift 8 does a divide by 256 instead of by 255
			// which is much faster than real divide (but same speed as lookup table)
			// and the error of roughly 1 part in 256 only occurs in the
			// semi-transparent areas where it's somewhat difficult to detect
			if (alpha == 0)				// source is fully transparent
				*pDst++ = 0;
			else if (alpha == 0xFF)		// source is fully opaque
				*pDst++;
			else								// source is semi-transparent
			{
				*pDst = (alpha * *pDst) >> 8;
				pDst++;
			}
#elif ALPHA_BLEND_METHOD == DIVIDE_ALPHA_BLEND_METHOD
			// the DIVIDE method:
			// much slower than bitshift but the most accurate
			// (but same accuracy as faster lookup table method)
			if (alpha == 0)				// source is fully transparent
				*pDst++ = 0;
			else if (alpha == 0xFF)		// source is fully opaque
				*pDst++;
			else								// source is semi-transparent
			{
				*pDst = (alpha * *pDst) / 255;
				pDst++;
			}
#else
			TpsAssertAlways( "Invalid ALPHA_BLEND_METHOD" );
#endif
		}

		pSrcRow += srcBytesPerRow;
		pDstRow += dstBytesPerRow;
	}
}

/* Routine:		void RImageLibrary::Blur
 *
 * Parameters:					Type	Description
 *				rSrcBitmap		read	base address of the input image
 *				rDstBitmap		write	base address of the output image (can = in)
 *				blurRadius		read	radius (in pixels) of the blur filter
 *				blurType			read	type of blur: in, out, or both
 *				blurFilter		read	type of filter: box or Gaussian
 *
 * Description: Blurs an image using a box filter.
 *
 * Author: Rob Cook
 */
void RImageLibrary::Blur(
	RBitmapImage&		rSrcBitmap,
	RBitmapImage&		rDstBitmap,
	uLONG					blurRadius,
	typeOfBlur			blurType,
	typeOfFilter		filterType)
{
	divisionLut = 0;
	scanlineBuffer = 0;

	try {
		uLONG	width  = rSrcBitmap.GetWidthInPixels();
		uLONG	height = rSrcBitmap.GetHeightInPixels();
		uBYTE	*pSrcRow = (uBYTE*)RBitmapImage::GetImageData( rSrcBitmap.GetRawData() );
		uBYTE	*pDstRow = (uBYTE*)RBitmapImage::GetImageData( rDstBitmap.GetRawData() );
		uLONG	divisionLutSize;
		uLONG	scanlineBufferSize;
		uBYTE	*div;
		int	i, j, w, r;
		RBitmapImage* pTmpBitmap;

//		TpsAssert( filterType == boxFilter || filterType == gaussianFilter, "invalid filter type" );
		TpsAssert( blurType == blurIn || blurType == blurOut || blurType == blurInAndOut, "invalid blur type" );

		// be sure rSrcBitmap and rDstBitmap are 8 bits deep and are the same size
		TpsAssert( rSrcBitmap.GetBitDepth() == 8, "rSrcBitmap not 8 bits deep." );
		TpsAssert( rDstBitmap.GetBitDepth() == 8, "rDstBitmap not 8 bits deep." );
		TpsAssert( width == rDstBitmap.GetWidthInPixels() && height == rDstBitmap.GetHeightInPixels(), "rDstBitmap not same size as rSrcBitmap." );

		/* Allocate and init division lut.
		 *
		 * This table is indexed by "sum", a value that is the sum of "w" pixels,
		 *		where w = width of the blur = 2 * r + 1,
		 *		where r = radius of the blur.
		 * The entry in the "sum"th location of the table is sum/bw.
		 *
		 * The smallest possible sum is 0, and largest possible sum is 255*w, so the
		 * table has 255*w+1 entries.
		 *
		 * Setting up the table must be done carefully in order to round properly.  In order
		 * to see what we need to do, let's look at the transition points of the table.  
		 *		If sum = n*w + r, we round down to n.
		 *			This is because (n*w+r)/w = n + r/(2r+1), and r/(2r+1) < 1/2
		 *		If sum = n*w + r + 1, we round up to n+1.
		 *			This is because (n*w+r+1)/w = n + (r+1)/(2r+1), and (r+1)/(2r+1) > 1/2
		 * This means that:
		 *		the first "r+1" entries are 0
		 *		the next "w" entries are 1
		 *		the next "w" entries are 2
		 *		...
		 *		the next "w" entries are 254
		 *		and the last "r+1" entries are 255
		 * We can see that this ends up properly at the high end of the table, because
		 *		( (255*w+1) - r ) / w = ( (254*w+2*r+1) - r ) / w
		 *				= 254 + (r+1)/w,
		 * which rounds up to 255, while the previous entry rounds down to 254.
		 *
		 * Note that we build an r=1 table for the Gaussian filter because it is implemented
		 * as a convolution of r=1 box filters.
		 */
		r = ( filterType == boxFilter ) ? blurRadius : 1;
		w = 2 * r + 1;
		divisionLutSize = 255 * w + 1;
		divisionLut = ( uBYTE *) new char[divisionLutSize];
		div = divisionLut;
		for ( j = 0; j < r + 1; j++ )
		*div++ = 0;
		for ( i = 1; i < 255; i++ )
			for ( j = 0; j < w; j++ )
				*div++ = i;
		for ( j = 0; j < r + 1; j++ )
			*div++ = 255;

		/* Allocate scanline buffer.  This is the size of the largest scanline (horizontal or
		 * vertical) plus a buffer of "blurRadius" on either side.
		 */
		scanlineBufferSize = 2 * blurRadius + ( ( width > height ) ? width : height );
		scanlineBuffer = ( uBYTE *) new char[ scanlineBufferSize ];
		

		/* There are 3 types of blur: blurring "in", blurring "out" and blurring "in and out".
		 * The meaning of these can be illustrated by looking at what happens to a scanline
		 * that contains a hard edge that is white (255) on the left and black (0) on the right:
		 *
		 *				+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
		 *				| 255 | 255 | 255 | 255 | 255 |  0  |  0  |  0  |  0  |  0  |
		 *				+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
		 *				   A     B     C     D     E     F     G     H     I     J
		 *
		 * Blurring "in and out" is the standard blur operation.  Each input pixel is replaced
		 * by the average of all the pixels within a distance of "blurRadius" pixels in x or y.
		 * For a box filter with blurRadius = 2, for example, we replace pixel "E" with the value
		 * of (C+D+E+F+G)/5, etc..  The result is:
		 *
		 *				+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
		 *				| 255 | 255 | 255 | 204 | 153 | 102 |  51 |  0  |  0  |  0  |
		 *				+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
		 *				   A     B     C     D     E     F     G     H     I     J
		 *
		 * Blurring "in" means blurring toward the white pixels, so that none of the black
		 * pixels are changed.  For a box filter with blurRadius = 2, the result is:
		 *
		 *				+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
		 *				| 255 | 204 | 153 | 102 |  51 |  0  |  0  |  0  |  0  |  0  |
		 *				+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
		 *
		 * Blurring "out" means blurring toward the black pixels, so that none of the white
		 * pixels are changed.  For a box filter with blurRadius = 2, the result is:
		 *
		 *				+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
		 *				| 255 | 255 | 255 | 255 | 255 | 204 | 153 | 102 |  51 |  0  |
		 *				+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
		 *
		 * In order to produce the effect of blurring "in", we have to move the edge toward
		 * the white by a distance of 2 pixels before we blur.  This is accomplished with an
		 * "erosion" operation (see comments in the Erode routine).
		 *
		 * Similarly, in order to produce the effect of blurring "out", we have to move the edge toward
		 * the black by a distance of 2 pixels before we blur.  This is accomplished with an
		 * "dilation" operation (see comments in the Dilate routine).
		 */

		if ( blurType == blurIn ) {
			RImageLibrary::Erode ( rSrcBitmap, rDstBitmap, blurRadius );
			pTmpBitmap = &rDstBitmap;
		} else if ( blurType == blurOut ) {
			RImageLibrary::Dilate ( rSrcBitmap, rDstBitmap, blurRadius );
			pTmpBitmap = &rDstBitmap;
		} else {
			/* If we're not eroding or dilating then the blur takes the
			 * original src image as input. Otherwise, it takes the
			 * output of the erosion or dilation, which is in the dst.
			 */
			pTmpBitmap = &rSrcBitmap;
		}

		if ( filterType == boxFilter )
			RImageLibrary::BoxBlur	( *pTmpBitmap, rDstBitmap, blurRadius );
		else if ( filterType == gaussianFilter )
			RImageLibrary::GaussianBlur ( *pTmpBitmap, rDstBitmap, blurRadius );


	}
	catch(...)
	{
	}

	delete[] divisionLut;
	delete[] scanlineBuffer;

}

/* Routine:		RImageLibrary::BoxBlur
 *
 * Parameters:					Type	Description
 *				rSrcBitmap		read	base address of the input image
 *				rDstBitmap		write	base address of the output image (can = in)
 *				blurRadius		read	radius (in pixels) of the blur filter
 *
 * Description: Blurs an image using a box filter.
 */
void RImageLibrary::BoxBlur(
	RBitmapImage&	rSrcBitmap,
	RBitmapImage&	rDstBitmap,
	uLONG				blurRadius)
{
	uBYTE	*from, *to;					// We use "from" and "to" for the input and output of
	uBYTE	*fromStart, *toStart;	//		each pass to avoid confusing them with "src" and
											//		"dst", which are the input and output of the routine.
	uBYTE *left, *top;				// Pointer to the beginning of the filter
	uBYTE *buf;							// Pointer into the scanline buffer
	uLONG	x, y;							// Loop counters
	uLONG	runningTotal;
	uLONG	blurWidth;

	uLONG	width  = rSrcBitmap.GetWidthInPixels();
	uLONG	height = rSrcBitmap.GetHeightInPixels();
	uBYTE	*pSrcRow = (uBYTE*)RBitmapImage::GetImageData( rSrcBitmap.GetRawData() );
	uBYTE	*pDstRow = (uBYTE*)RBitmapImage::GetImageData( rDstBitmap.GetRawData() );
	uLONG	bytesPerRow = RBitmapImage::GetBytesPerRow( width, 8 );

	blurWidth = 1 + 2 * blurRadius;
	 
	/* The box filter is separable, so we can do the computation as a 2-pass operation.
	 * First, we blur in X, they we blurv in Y.
	 */

	// X pass
	fromStart = pSrcRow;
	toStart	 = pDstRow;
	for ( y = 0; y < height; y++ ) {
		// Step 1. Read this row from the inpute image into the scanline buffer.
		from = fromStart;
		buf = scanlineBuffer + blurRadius;
		memcpy( buf, from, width);

		/* Oh, what should we do at the edges of the image?  Here are some options:
		 *		1. Assume the image is black outside of the image.  This has the
		 *			generally undesirable effect of making the edges of the blurred
		 *			image fade to black.
		 *		2. Assume the image is white outside of the image.  This has the
		 *			generally undesirable effect of making the edges of the blurred
		 *			image fade to white.
		 *		3. Assume the edge pixel repeats indefinitely.  This avoids the
		 *			problem of the image fading to black or white at the edges, but
		 *			has the undesirable effect of making the image less blurry at the
		 *			edges.
		 *		4. Assume the plane is tiled with mirror images of the image, as
		 *			illustrated for this 3x3 image:
		 *				image pixels:	ABC
		 *									DEF
		 *									GHI
		 *				image with its mirrors shown in lower case:
		 *									ihgghiihg
		 *									feddeffed
		 *									cbaabccba
		 *									cbaABCcba
		 *									fedDEFfed
		 *									ihgGHIihg
		 *									ihgghiihg
		 *									feddeffed
		 *									cbaabccba
		 * This last alternative seems to be the best overall choice, so that's what
		 * we do.  If the blur radius is larger than the image, the calculations used
		 * here won't do exactly the right thing, but in that case, the image is so blurry
		 * that who can tell?
		 */
		// Replicate pixels to create a left border in the scanline buffer.
		buf = scanlineBuffer;
		from = buf + ( 2 * blurRadius - 1 );
		for ( x = 0; x < blurRadius; x++ )
			*buf++ = *from--;

		// Replicate pixels to create a right border in the scanline buffer.
		buf = scanlineBuffer + width + 2 * blurRadius - 1;
		from = buf - ( 2 * blurRadius - 1 );
		for ( x = 0; x < blurRadius; x++ )
			*buf-- = *from++;

		/* Step 2. Blur the scanline buffer, putting the result in the output image.
		 *
		 * Each pixel in the scanline has a "blur neighborhood" consisting of the pixels
		 * within "blurRadius" pixels of it. Each pixel is replaced by the average of the pixels
		 * in its blur neighborhood. For example, for pixel "A" and a blur radius of 3,
		 * A is replaced by the average of the 7 pixels marked X, as shown below:
		 *
		 *				+----+----+----+----+----+----+----+----+----+----+
		 *				|    | X1 | X2 | X3 | X4 | X5 | X6 | X7 |    |    |
		 *				+----+----+----+----+----+----+----+----+----+----+
		 *                <--------------- A -------------->
		 *					A = ( X1 + X2 + X3 + X4 + X5 + X6 + X7 ) / 7
		 *
		 * The straighforward approach to performing this calculation involves performing
		 * 6 additions and 1 division for every pixel in the scanline.  
		 *
		 * Instead, we can take advantage of the fact that each time we move one pixel to the right,
		 * the blur neighborhood shifts one pixel to the right too, so that the blur
		 * neighborhoods of the two pixels are the same except for at the edges.  For example:
		 *
		 *				+----+----+----+----+----+----+----+----+----+----+
		 *          |    | X1 | X2 | X3 | X4 | X5 | X6 | X7 | X8 |    |
		 *				+----+----+----+----+----+----+----+----+----+----+
		 *                <--------------- A -------------->
		 *                    <--------------- B -------------->
		 *					A = ( X1 + X2 + X3 + X4 + X5 + X6 + X7      ) / 7
		 *					B = (      X2 + X3 + X4 + X5 + X6 + X7 + X8 ) / 7
		 *
		 * so that
		 *		B = A + ( X8 - X1 ) / 7
		 * or 2 additions and 1 division per pixel.  Furthermore, since we are dividing by a constant,
		 * the division can be conviently done with a small lookup table, which just requires one addition
		 * to index into the table.  In general, the straightforward approach requires
		 *		2 * dilationRadius
		 * additions and 1 division per pixel and the optimized approach requires
		 *		3
		 * additions per pixel.  A nonpipelined division operation on a Pentium requires 4
		 * ticks and an addition requires 1, so the speed up is from ( 2 * dilationRadius + 4 )
		 * ticks to 3 ticks, for a relative speedup of approximately (ignoring loop overhead, etc.):
		 *		( 2 * dilationRadius + 4 ) / 3
		 * For example,
		 *			  r		  speedup
		 *			  1			 2X
		 *			  2			 2.7X
		 *			  5			 4.7X
		 *			 10			 8X
		 *			100			68X
		 */

		// Calculate the sum that's used in calculating the first blurred pixel.
		runningTotal = 0;
		from = scanlineBuffer;
		left = scanlineBuffer;
		for ( x = 0; x < blurWidth; x++ )
			runningTotal += *from++;
		
		// Now loop through, blurring the rest of the pixels in the scanline:
		to = toStart;
		for ( x = 0; x < width; x++ ) {
			*to++ = divisionLut[ runningTotal ];	// = runningTotal / blurWidth
			runningTotal += *from++;					// Add in the arriving pixel on the right.
			runningTotal -= *left++;					// Subtract out the deparing pixel on the left.
		}
		fromStart += bytesPerRow;
		toStart   += bytesPerRow;
	}

	// Y pass
	fromStart = pDstRow;
	toStart	 = pDstRow;
	for ( x = 0; x < width; x++ ) {
		// Step 1. Read this column into the scanline buffer.
		from = fromStart;
		buf = scanlineBuffer + blurRadius;
		for ( y = 0; y < height; y++ ) {
			*buf++ = *from;
			from += bytesPerRow;
		}

		// Replicate pixels to create a left border in the scanline buffer.
		buf = scanlineBuffer;
		from = buf + ( 2 * blurRadius - 1 );
		for ( y = 0; y < blurRadius; y++ )
			*buf++ = *from--;

		// Replicate pixels to create a right border in the scanline buffer.
		buf = scanlineBuffer + height + 2 * blurRadius - 1;
		from = buf - ( 2 * blurRadius - 1 );
		for ( y = 0; y < blurRadius; y++ )
			*buf-- = *from++;
		
		// Step 2. Blur the scanline buffer.  See commments in the X pass section.
		// Calculate the sum that's used in calculating the first blurred pixel.
		runningTotal = 0;
		from = scanlineBuffer;
		top = scanlineBuffer;
		for ( y = 0; y < blurWidth; y++ )
			runningTotal += *from++;
		
		// Now loop through, blurring the rest of the pixels in the scanline:
		to = toStart;
		for ( y = 0; y < height; y++ ) {
			*to = divisionLut[ runningTotal ];	// = runningTotal / blurWidth
			to += bytesPerRow;
			runningTotal += *from++;				// Add in the arriving pixel on the bottom (right).
			runningTotal -= *top++;					// Subtract out the deparing pixel on the top (left).
		}
		fromStart++;
		toStart++;
	}

}
									
#pragma warning( disable : 4701 )	// Assert placed below to flag if this is a real problem.

/* Routine:		RImageLibrary::GaussianBlur
 *
 * Parameters:					Type	Description
 *				rSrcBitmap		read	base address of the input image
 *				rDstBitmap		write	base address of the output image (can = in)
 *				blurRadius		read	radius (in pixels) of the blur filter
 *
 * Description: Blurs an image using a Gaussian filter.
 */
void RImageLibrary::GaussianBlur (
	RBitmapImage&	rSrcBitmap,
	RBitmapImage&	rDstBitmap,
	uLONG				blurRadius)
{
	uBYTE	*from, *to;					// We use "from" and "to" for the input and output of
	uBYTE	*fromStart, *toStart;	//		each pass to avoid confusing them with "src" and
											//		"dst", which are the input and output of the routine.
	uBYTE *buf;							// Pointer into the scanline buffer
	uLONG	i, x, y;						// Loop counters
	uLONG	thisPixel, nextPixel;	// Longs, not bytes, so we have room for sums
	uLONG blurWidth;

	uLONG width = rSrcBitmap.GetWidthInPixels();
	uLONG height = rSrcBitmap.GetHeightInPixels();
	uBYTE* pSrcRow = (uBYTE*)RBitmapImage::GetImageData( rSrcBitmap.GetRawData() );
	uBYTE* pDstRow = (uBYTE*)RBitmapImage::GetImageData( rDstBitmap.GetRawData() );
	uLONG bytesPerRow = RBitmapImage::GetBytesPerRow( width, 8 );

	blurWidth = 1 + 2 * blurRadius;

	/* The Gaussian filter is separable, so we can do the computation as a 2-pass operation.
	 * First, we blur in X, they we blurv in Y.
	 */

	// X pass
	fromStart = pSrcRow;
	toStart	 = pDstRow;
	for ( y = 0; y < height; y++ )
	{
		// Step 1. Read this row from the inpute image into the scanline buffer.
		from = fromStart;
		buf = scanlineBuffer + blurRadius;
		memcpy( buf, from, width);

		// Replicate pixels to create a left border in the scanline buffer.
		buf = scanlineBuffer;
		from = buf + ( 2 * blurRadius - 1 );
		for ( x = 0; x < blurRadius; x++ ) {
			*buf++ = *from--;
		}

		// Replicate pixels to create a right border in the scanline buffer.
		buf = scanlineBuffer + width + 2 * blurRadius - 1;
		from = buf - ( 2 * blurRadius - 1 );
		for ( x = 0; x < blurRadius; x++ ) {
			*buf-- = *from++;
		}

		/* Step 2. Blur the scanline buffer, putting the result in the output image.
		 *
		 * Each pixel in the scanline has a "blur neighborhood" consisting of the pixels
		 * within "blurRadius" pixels of it. Each pixel is replaced by a weighted average
		 * of the pixels in its blur neighborhood. For example, for pixel "A" and a blur radius of 3,
		 * A is replaced by the weighted average of the 7 pixels marked X, as shown below:
		 *
		 *				+----+----+----+----+----+----+----+----+----+----+
		 *          |    | X1 | X2 | X3 | X4 | X5 | X6 | X7 |    |    |
		 *				+----+----+----+----+----+----+----+----+----+----+
		 *                <--------------- A -------------->
		 *					A = ( G(3)*X1 + G(2)*X2 + G(1)*X3 + G(0)*X4 + G(1)*X5 + G(2)*X6 + G(3)*X7 )
		 * where
		 *		G(x) = the value of the Gaussian at distance x from the center.
		 *			 = exp ( - x*x / r*r )
		 *		r = radius of the Gaussian
		 *
		 * This is a very expensive calculation involving an exponential function.  But since the radius
		 * is the same for every pixel in the image and the distances are multiples of a pixel width, we
		 * can precompute a table of Gaussian coefficients and just index it inside the pixel loop.
		 * But even with this optimization, we are still faced in this example with 7 multiplications,
		 * 6 additions, and 7 lookups per pixel.  If we do this using fixed point arithmetic, we save having
		 * to do floating point multiplications and having to convert pixel values to floating point, but
		 * we add in an 8-bit shift at the end.
		 *
		 * But there is a better way.  It turns out that a Gaussian filter is a convolution of box filters.
		 * This means that we can calculate a Gaussian or radius r by successively applying a 1-pixel-radius
		 * box filter r times. In other words, after the first pass we have in the above example,
		 *		P1	= ( X1 + X2 + X3 ) / 3
		 *		P2	= ( X2 + X3 + X4 ) / 3
		 *		P3	= ( X3 + X4 + X5 ) / 3
		 *		P4	= ( X4 + X5 + X6 ) / 3
		 *		P5	= ( X5 + X6 + X7 ) / 3
		 *	then after the second pass box filter
		 *		Q1	= ( P1 + P2 + P3 ) / 3
		 *		Q2	= ( P2 + P3 + P4 ) / 3
		 *		Q3	= ( P3 + P4 + P5 ) / 3
		 *	and finally after the third pass
		 *		A	= ( Q1 + Q2 + Q3 ) / 3
		 * The effect is to calculate:
		 *		X = ( X1 + 3*X2 + 6*X3 + 7*X4 + 6*X5 + 2*X6 + X7 ) / 27
		 *
		 * Because the filters overlap, the P and Q calculations shown above are used as intermediate values
		 * for more than one pixel. In general, there are are "blurRadius" passes, each of which uses 2 additions
		 * and 1 division per pixel, for a total cost of
		 *		blurRadius * ( 2 additions + 1 division )
		 * per pixel, and since we can build a small lookup table to do the division by 3, the division
		 * becomes a lookup with the same cost as an addition, so the cost is actually
		 *		blurRadius * 3
		 * additions per pixel.  A nonpipelined multiplication or operation on a Pentium requires 4
		 * ticks and an addition requires 1, so the speed up is from ( 2 * blurRadius + 1 ) * ( 4 + 1 + 1 )
		 * ticks to blurRadius * 3 ticks, for a relative speedup of approximately (ignoring loop overhead, etc.):
		 *		( 12 * blurRadius + 6 ) / ( 3 * blurRadius)
		 * or about 4.
		 * For example,
		 *			  r		  speedup
		 *			  1			  6X
		 *			  2			  5X
		 *			  5			4.4X
		 *			 10			4.2X
		 *			100			4.0X
		 */

		// Ensure the blur radius is non-zero or thisPixel ends up undefined below.
		ASSERT( blurRadius > 0 );
		// We do a Gaussian blur of radius R by doing R successive box blurs of radius 1.
		for ( i = 0; i < blurRadius; i++ ) {
			// We have enough border that we can leave the left and right edge pixels unchanged.  This
			// makes dealing with the edge conditions a little bit faster.
			from = scanlineBuffer;
			thisPixel = *from;

			/* We don't include the edge pixels in the loop, so:
			 * start the loop when we read pixels 0, 1, 2
			 * end the loop when we read pixels n-3, n-2, n-1, where n = width + blurWidth (# pixels in scanlineBuffer)
			 */
			for ( x = 0; x < width + blurWidth - 2; x++ ) {
				nextPixel = from[0] + from[1] + from[2];
				nextPixel = divisionLut[ nextPixel ];	// nextPixel /= 3
				*from++ = thisPixel;
				thisPixel = nextPixel;
			}
			*from++ = thisPixel;

		}

		// Copy the result to the output.
		buf = scanlineBuffer + blurRadius;
		to = toStart;
		for ( x = 0; x < width; x++ ) {
			*to++ = *buf++;
		}

		fromStart += bytesPerRow;
		toStart   += bytesPerRow;
	}

	// Y pass
	fromStart = pDstRow;
	toStart	 = pDstRow;
	for ( x = 0; x < width; x++ )
	{
		// Step 1. Read this column into the scanline buffer.
		from = fromStart;
		buf = scanlineBuffer + blurRadius;
		for ( y = 0; y < height; y++ ) {
			*buf++ = *from;
			from += bytesPerRow;
		}

		// Replicate pixels to create a left border in the scanline buffer.
		buf = scanlineBuffer;
		from = buf + ( 2 * blurRadius - 1 );
		for ( y = 0; y < blurRadius; y++ ) {
			*buf++ = *from--;
		}

		// Replicate pixels to create a right border in the scanline buffer.
		buf = scanlineBuffer + height + 2 * blurRadius - 1;
		from = buf - ( 2 * blurRadius - 1 );
		for ( y = 0; y < blurRadius; y++ ) {
			*buf-- = *from++;
		}

		// Step 2. Blur the scanline buffer.  See commments in the X pass section.
		// We do a Gaussian blur of radius R by doing R successive box blurs of radius 1.
		for ( i = 0; i < blurRadius; i++ ) {
			// We have enough border that we can leave the left and right edge pixels unchanged.  This
			// makes dealing with the edge conditions a little bit faster.
			from = scanlineBuffer;
			nextPixel = *from;

			for ( y = 1; y < height + blurWidth - 2; y++ ) {
				nextPixel = from[0] + from[1] + from[2];
				nextPixel = divisionLut[ nextPixel ];	// nextPixel /= 3
				*from++ = thisPixel;
				thisPixel = nextPixel;
			}
			*from++ = thisPixel;
		}

		// Copy the result to the output.
		buf = scanlineBuffer + blurRadius;
		to = toStart;
		for ( y = 0; y < height; y++ ) {
			*to = *buf++;
			to += bytesPerRow;
		}

		fromStart++;
		toStart++;
	}
}
#pragma warning( default : 4701 )

/* Routine:		RImageLibrary::Dilate
 *
 * Parameters:					Type	Description
 *				rSrcBitmap		read	base address of the input image
 *				rDstBitmap		write	base address of the output image (can = in)
 *				dilationRadius	read	radius (in pixels) of the blur filter
 *
 * Description: Dilates an image.  Each pixel is set to the largest value in its neighborhood.
 *
 */
void RImageLibrary::Dilate (
	RBitmapImage&	rSrcBitmap,
	RBitmapImage&	rDstBitmap,
	uLONG				dilationRadius)
{
	uBYTE	*from, *to;					// We use "from" and "to" for the input and output of
	uBYTE	*fromStart, *toStart;	//		each pass to avoid confusing them with "src" and
											//		"dst", which are the input and output of the routine.
	uBYTE *left, *top;				// Pointer to the beginning of the filter
	uBYTE *buf;							// Pointer into the scanline buffer
	uBYTE *pp;							// Inner loop pixel pointer
	uLONG	x, y;							// Loop counters
	uLONG	xx, yy;						// Inner loop counters
	uBYTE	dilatedValue;
	uLONG	dilationWidth;
	
	uLONG width = rSrcBitmap.GetWidthInPixels();
	uLONG height = rSrcBitmap.GetHeightInPixels();

	uBYTE* pSrcRow = (uBYTE*)RBitmapImage::GetImageData( rSrcBitmap.GetRawData() );
	uBYTE* pDstRow = (uBYTE*)RBitmapImage::GetImageData( rDstBitmap.GetRawData() );

	uLONG bytesPerRow = RBitmapImage::GetBytesPerRow( width, 8 );

	dilationWidth = 1 + 2 * dilationRadius;
	 
	/* This dilation operation is separable, so we can do the computation as a 2-pass operation.
	 * First, we dilate in X, they we dilate in Y.
	 */

	// X pass
	fromStart = pSrcRow;
	toStart	 = pDstRow;
	for ( y = 0; y < height; y++ )
	{
		RCursor().SpinCursor();

		// Step 1. Read row "y" from the input image into the scanline buffer.

		/* For computational ease, we create a border of black pixels around the
		 * the image.  Since these black (0) pixels can never be larger than any other
		 * pixel values, we can then run through our pixel loop without having to do any
		 * special case coding for the edges.  The black pixels in the border can
		 * simply be included in the calculation of the maximum, since they are guaranteed
		 * not to alter the result.
		 */

		// Create a border of black pixels on the left so that the edge will not dilate.
		memset( scanlineBuffer, 0, dilationRadius );
		// Copy pixels in the middle.
		memcpy( &scanlineBuffer[dilationRadius], fromStart, width);
		// Create a border of black pixels on the right.
		memset( &scanlineBuffer[width+dilationRadius], 0, dilationRadius );


		/* Step 2. Dilate the scanline buffer, putting the result in the output image.
		 *
		 * Each pixel in the scanline has a "dilation neighborhood" consisting of the pixels
		 * within "dilationRadius" pixels of it. Each pixel is replaced by the largest pixel
		 * in its dilation neighborhood. For example, for pixel "A" and a dilation radius of 3,
		 * A is replaced by the largest of the 7 pixels marked X, as shown below:
		 *
		 *				+----+----+----+----+----+----+----+----+----+----+
		 *          |    | X1 | X2 | X3 | X4 | X5 | X6 | X7 |    |    |
		 *				+----+----+----+----+----+----+----+----+----+----+
		 *                <--------------- A -------------->
		 *					A = max( X1, X2, X3, X4, X5, X6, X7 )
		 *
		 * The straighforward approach to performing this calculation involves performing
		 * 6 comparisons for every pixel in the scanline.  
		 *
		 * Instead, we can take advantage of the fact that each time we move one pixel to the right,
		 * the dilation neighborhood shifts one pixel to the right too, so that the dilation
		 * neighborhoods of the two pixels are the same except for at the edges.  For example:
		 *
		 *				+----+----+----+----+----+----+----+----+----+----+
		 *          |    | X1 | X2 | X3 | X4 | X5 | X6 | X7 | X8 |    |
		 *				+----+----+----+----+----+----+----+----+----+----+
		 *                <--------------- A -------------->
		 *                    <--------------- B -------------->
		 *					A = max (X1:X7) = max( X1, X2, X3, X4, X5, X6, X7 )
		 *					B = max (X2:X8) = max(     X2, X3, X4, X5, X6, X7, X8 )
		 *
		 * If X1<A, then
		 *		max(X2:X7) = max(X1:X7) = A
		 * so that
		 *		B = max(X2:X8) = max(A:X8)
		 * which is only one comparison.  If X1>A, then we have to perform the full set of 6
		 * comparisons.  But the chances that X1>A is about 1/7.  So the expected number of
		 * operations per pixel is
		 *		(1/7)*6 + (6/7)*1 = 12/7
		 * In the dilationRadius=3 example, this is more than 3 times faster than the straightforward
		 * approach.  In general, the straightforward approach requires
		 *		2 * dilationRadius
		 * comparisons per pixel and the optimized approach requires
		 *		2 * dilationRadius * ( 2 / ( 2 * dilationRadius + 1 ) )
		 * comparisons, for a relative speedup of approximately (ignoring loop overhead, etc.):
		 *		dilationRadius + 0.5
		 * For example,
		 *			  r		   speedup
		 *			  1			  1.5X
		 *			  2			  2.5X
		 *			  5			  5.5X
		 *			 10			 10.5X
		 *			100			100.5X
		 *
		 */

		// Calculate the value of the first dilated pixel:
		dilatedValue = 0;
		from	 = scanlineBuffer;
		for ( x = 0; x < dilationWidth; x++ )
		{
			if ( *from > dilatedValue ) dilatedValue = *from;
			from++;
		}
		
		// Now loop through, dilating the rest of the pixels in the scanline:
		left = scanlineBuffer;
		to = toStart;
		for ( x = 0; x < width; x++ )
		{
			*to++ = dilatedValue;
			if ( *left++ >= dilatedValue ) {
				/* If the pixel leaving the dilation neighborhood is not smaller than the current max,
				 * then we have to recompute the max from scratch.
				 */
				dilatedValue = 0;
				pp = left;
				for ( xx = 0; xx < dilationWidth; xx++ )
				{
					if ( *pp > dilatedValue ) dilatedValue = *pp;
					pp++;
				}
			} else {
				/* If the pixel leaving the dilation neighborhood is smaller than the current max,
				 * then we just have to look at the pixel moving into the neighborhood.
				 */
				if ( *from > dilatedValue ) dilatedValue = *from;
			}
			from++;
		}
		fromStart += bytesPerRow;
		toStart += bytesPerRow;
	}

	// Y pass
	fromStart = pDstRow;
	toStart	 = pDstRow;
	for ( x = 0; x < width; x++ )
	{
		RCursor().SpinCursor();

		// Step 1. Read column "x" into the scanline buffer.

		// Create a border of black pixels on the left so that the edge will not dilate.
		memset( scanlineBuffer, 0, dilationRadius );

		// Copy pixels in the middle.
		buf  = scanlineBuffer + dilationRadius;
		from = fromStart;
		for ( y = 0; y < height; y++ ) {
			*buf++ = *from;
			from += bytesPerRow;
		}

		// Create a border of black pixels on the right.
		memset( &scanlineBuffer[height+dilationRadius], 0, dilationRadius );

		// Step 2. Dilate the scanline buffer.  See commments in the X pass section.
		// Calculate the value of the first dilated pixel:
		dilatedValue = 0;
		from = scanlineBuffer;
		for ( y = 0; y < dilationWidth; y++ )
		{
			if ( *from > dilatedValue ) dilatedValue = *from;
			from++;
		}
		
		// Now loop through, eroding the rest of the pixels in the scanline:
		to = toStart;
		top = scanlineBuffer;
		for ( y = 0; y < height; y++ )
		{
			*to = dilatedValue;

			if ( *top++ >= dilatedValue ) {
				dilatedValue = 0;
				pp = top;
				for ( yy = 0; yy < dilationWidth; yy++ )
				{
					if ( *pp > dilatedValue ) dilatedValue = *pp;
					pp++;
				}
			} else {
				if ( *from > dilatedValue ) dilatedValue = *from;
			}
			to += bytesPerRow;
			from++;
		}
		fromStart++;
		toStart++;
	}
}

/* Routine:		RImageLibrary::Erode
 *
 * Parameters:					Type	Description
 *				rSrcBitmap		read	base address of the input image
 *				rDstBitmap		write	base address of the output image (can = in)
 *				erosionRadius	read	radius (in pixels) of the blur filter
 *
 * Description: Erodes an image.  Each pixel is set to the smallest value in its neighborhood.
 *
 */
void RImageLibrary::Erode (
	RBitmapImage&	rSrcBitmap,
	RBitmapImage&	rDstBitmap,
	uLONG				erosionRadius)
{
	uBYTE	*from, *to;					// We use "from" and "to" for the input and output of
	uBYTE	*fromStart, *toStart;	//		each pass to avoid confusing them with "src" and
											//		"dst", which are the input and output of the routine.
	uBYTE *left, *top;				// Pointer to the beginning of the filter
	uBYTE *buf;							// Pointer into the scanline buffer
	uBYTE *pp;							// Inner loop pixel pointer
	uLONG	x, y;							// Loop counters
	uLONG	xx, yy;						// Inner loop counters
	uBYTE	erodedValue;
	uLONG	erosionWidth;
	
	uLONG width = rSrcBitmap.GetWidthInPixels();
	uLONG height = rSrcBitmap.GetHeightInPixels();
	uBYTE* pSrcRow = (uBYTE*)RBitmapImage::GetImageData( rSrcBitmap.GetRawData() );
	uBYTE* pDstRow = (uBYTE*)RBitmapImage::GetImageData( rDstBitmap.GetRawData() );
	uLONG bytesPerRow = RBitmapImage::GetBytesPerRow( width, 8 );

	erosionWidth = 1 + 2 * erosionRadius;
	 
	/* This erosion operation is separable, so we can do the computation as a 2-pass operation.
	 * First, we dilate in X, they we dilate in Y.
	 */

	// X pass
	fromStart		 = pSrcRow;
	toStart			 = pDstRow;
	for ( y = 0; y < height; y++ )
	{
		RCursor().SpinCursor();

		// Step 1. Read row "y" from the input image into the scanline buffer.

		// Create a border of black pixels on the left so that the edge will erode properly.
		memset( scanlineBuffer, 0, erosionRadius );
		// Copy pixels in the middle.
		memcpy( &scanlineBuffer[erosionRadius], fromStart, width);
		// Create a border of black pixels on the right.
		memset( &scanlineBuffer[width+erosionRadius], 0, erosionRadius );


		// Step 2. Erode the scanline buffer.  See commments in the X pass section of the dilation routine.
		// Calculate the value of the first eroded pixel:
		erodedValue = 255;
		from	 = scanlineBuffer;
		for ( x = 0; x < erosionWidth; x++ )
		{
			if ( *from < erodedValue ) erodedValue = *from;
			from++;
		}
		
		// Now loop through, eroding the rest of the pixels in the scanline:
		left = scanlineBuffer;
		to = toStart;
		for ( x = 0; x < width; x++ )
		{
			*to++ = erodedValue;
			if ( *left++ <= erodedValue ) {
				erodedValue = 255;
				pp = left;
				for ( xx = 0; xx < erosionWidth; xx++ )
				{
					if ( *pp < erodedValue ) erodedValue = *pp;
					pp++;
				}
			} else {
				if ( *from < erodedValue ) erodedValue = *from;
			}
			from++;
		}
		fromStart += bytesPerRow;
		toStart += bytesPerRow;
	}

	// Y pass
	fromStart = pDstRow;
	toStart	 = pDstRow;
	for ( x = 0; x < width; x++ )
	{
		RCursor().SpinCursor();

		// Step 1. Read column "x" into the scanline buffer.

		// Create a border of black pixels on the left so that the edge will erode properly.
		memset( scanlineBuffer, 0, erosionRadius );

		// Copy pixels in the middle.
		buf  = scanlineBuffer + erosionRadius;
		from = fromStart;
		for ( y = 0; y < height; y++ ) {
			*buf++ = *from;
			from += bytesPerRow;
		}

		// Create a border of black pixels on the right.
		memset( &scanlineBuffer[height+erosionRadius], 0, erosionRadius );


		// Step 2. Erode the scanline buffer.  See commments in the X pass section of the dilation routine.
		// Calculate the value of the first eroded pixel:
		erodedValue = 255;
		from = scanlineBuffer;
		for ( y = 0; y < erosionWidth; y++ )
		{
			if ( *from < erodedValue ) erodedValue = *from;
			from++;
		}
		
		// Now loop through, eroding the rest of the pixels in the scanline:
		to = toStart;
		top = scanlineBuffer;
		for ( y = 0; y < height; y++ )
		{
			*to = erodedValue;

			if ( *top++ <= erodedValue ) {
				erodedValue = 255;
				pp = top;
				for ( yy = 0; yy < erosionWidth; yy++ )
				{
					if ( *pp < erodedValue ) erodedValue = *pp;
					pp++;
				}
			} else {
				if ( *from < erodedValue ) erodedValue = *from;
			}
			to += bytesPerRow;
			from++;
		}
		fromStart++;
		toStart++;
	}
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
void RImageLibrary::GetOffscreenOutline(RBitmapImage& rBitmap, const RSolidColor& transparentColor, RPolyPolygon& outline )
{
	try
	{
		BITMAPINFO* pBitmapInfo = (BITMAPINFO*)rBitmap.GetRawData();
		DIBINFO	dibInfo;

		dibInfo.pBits		= static_cast<uBYTE*>(RBitmapImage::GetImageData(pBitmapInfo));
		dibInfo.Width		= rBitmap.GetWidthInPixels( );
		dibInfo.Height		= rBitmap.GetHeightInPixels( );

		dibInfo.TransparentColor = static_cast<COLORREF>( rBitmap.GetIndexOfColor( transparentColor ) );

		switch ( rBitmap.GetBitDepth( ) )
			{
			case 1 :
			case 4 :
			default :
				TpsAssertAlways( "Unsupported Bit Depth" );
				return;
			case 8 :
				dibInfo.BytesPerPixel		= 1;
				break;
			case 16 :
				dibInfo.BytesPerPixel		= 2;
				break;
			case 24 :
				dibInfo.BytesPerPixel		= 3;
				break;
			case 32 :
				dibInfo.BytesPerPixel		= 4;
				break;
			}
		dibInfo.BytesPerRow = RBitmapImage::GetBytesPerRow(pBitmapInfo->bmiHeader.biWidth, 
			pBitmapInfo->bmiHeader.biBitCount);

		OutlineDibShapes( dibInfo, outline );
	}
	catch(...)
	{
	}
}

// .
// ****************************************************************************
//
//  Function Name:	PNGReader()
//
//  Description:    Static callback function used to provide 
//                  external data to the png reader
//
//  Returns:			
//
//  Exceptions:		kReadFailure
//
// ****************************************************************************
static void PNGReader( png_structp pStruct, png_bytep pData, png_size_t nSize )
{
	RStream* pIoData = (RStream *) png_get_io_ptr( pStruct );

	if (pIoData)
		pIoData->Read( nSize, pData );
	else
		throw kReadFailure;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::LoadPNGImage()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RBitmapImage* RImageLibrary::LoadPNGImage( const RBuffer& rBuffer, YImageIndex nIndex )
{
	RBitmapImage* pImage = NULL;
	RBitmapImage* pMaskImage = NULL;

	// Create the read structure.
	png_structp pPNG = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	png_infop pInfo  = png_create_info_struct( pPNG );
	
	try
	{
		// Set the read function.
		RBufferStream rStream( rBuffer.GetBuffer(), rBuffer.GetBufferSize() );
		png_set_read_fn( pPNG, (voidp) &rStream, PNGReader );
		png_read_info( pPNG, pInfo );

		if (pInfo->num_trans)
		{
			// Expand to 24-bit
			png_set_expand( pPNG );		
			png_read_update_info( pPNG, pInfo );
		}
		
		if (pPNG->bit_depth == 16)
		{
			png_set_strip_16( pPNG );	// Strip 16-bit to 8-bit.
			png_read_update_info( pPNG, pInfo );
		}

		if (pInfo->color_type & PNG_COLOR_TYPE_RGB)
		{
			png_set_bgr( pPNG );
		}

		int nChannels = png_get_channels( pPNG, pInfo );
		int nBitDepth = pInfo->bit_depth * nChannels;

		// Allocate image
		pImage = new RBitmapImage();
		pImage->Initialize( pInfo->width, pInfo->height, nBitDepth );

		// Build the row pointer list
		LPBYTE pImageData = (LPBYTE) pImage->GetImageData( pImage->GetRawData() ); 
		LPBYTE* row_pointers = new LPBYTE[pInfo->height];

		for (int i = pInfo->height - 1; i >= 0; i--)
		{
			row_pointers[i] = pImageData;
			pImageData += pInfo->rowbytes;
		}

		// Read the image
		png_read_image( pPNG, row_pointers );
		delete[] row_pointers;

		if (pInfo->color_type & PNG_COLOR_MASK_ALPHA)
		{
			TpsAssert( nBitDepth == 32, "Mask not supported at this bit depth" );

			// Allocate the mask image
			pMaskImage = new RBitmapImage();
			pMaskImage->Initialize( pInfo->width, pInfo->height, 8, kDefaultXDpi, kDefaultYDpi, kUseGrayScalePalette );

			// Allocate the new image
			RBitmapImage* pNewImage = new RBitmapImage();
			pNewImage->Initialize( pInfo->width, pInfo->height, nBitDepth - 8 );
			pNewImage->SetAlphaChannel( pMaskImage );

			// Save the current image pointer, 
			// and replace it with the new one.
			RBitmapImage* pBufferedImage = pImage;
			pImage = pNewImage;

			LPBYTE pSrcData   = (LPBYTE) pBufferedImage->GetImageData( pBufferedImage->GetRawData() ); 
			LPBYTE pImageData = (LPBYTE) pImage->GetImageData( pImage->GetRawData() ); 
			LPBYTE pMaskData  = (LPBYTE) pMaskImage->GetImageData( pMaskImage->GetRawData() ); 

			uLONG  nImageRowBytes = pImage->GetBytesPerRow( pInfo->width, pImage->GetBitDepth() );
			uLONG  nMaskRowBytes  = pImage->GetBytesPerRow( pInfo->width, pMaskImage->GetBitDepth() );

			for (int y = 0 ; y < pInfo->height; y++)
			{
				LPBYTE pSrcRow   = pSrcData   + y * pInfo->rowbytes;
				LPBYTE pImageRow = pImageData + y * nImageRowBytes;
				LPBYTE pMaskRow  = pMaskData  + y * nMaskRowBytes;
				

				for (int x = pInfo->width; x > 0 ; x--)
				{
					pImageRow[0] = pSrcRow[0]; 
					pImageRow[1] = pSrcRow[1]; 
					pImageRow[2] = pSrcRow[2];
					*pMaskRow++  = pSrcRow[3];

					pImageRow += 3;
					pSrcRow   += 4;
				}

			} // end for

			delete pBufferedImage;

			// Currently, there is only support for 1-bit alpha 
			// channels, so drop the bit depth on the mask image
			ChangeBitDepth( *pMaskImage, 1 );
		}

		if (pInfo->valid & PNG_INFO_PLTE)
		{
			RGBQUAD* pColorData = (RGBQUAD*) pImage->GetColorData( pImage->GetRawData() );
			if (pColorData) //j
			{
				// Read the palette data
				for (int i = 0; i < pInfo->num_palette; i++)
				{
					pColorData[i].rgbRed   = pInfo->palette[i].red;
					pColorData[i].rgbGreen = pInfo->palette[i].green;
					pColorData[i].rgbBlue  = pInfo->palette[i].blue;
					pColorData[i].rgbReserved = 0;
				}
			}
		}
	}
	catch (...)
	{
		if (pImage)
		{
			delete pImage;
			pImage = NULL;
		}

		TpsAssertAlways( "PNG Loader failed!" );
	}

	png_destroy_read_struct( &pPNG, &pInfo, (png_infopp) NULL );

	return pImage;
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
	//TpsAssert(yIndex == kDefaultImageIndex, "Multi-page files not available yet.");
	//IGE_PAGE_NOT_PRESENT
	//IGE_PAGE_INVALID
	//IGE_PAGE_COULD_NOT_BE_READ

	// Clear the error stack
	ClearLastException();

	// Convert RMBCString to a char*
	LPCSZ pImagePath = (LPCSZ)rImagePath;
	
	RBitmapImage* pNewBitmap = new RBitmapImage(TRUE);
	
	HIGEAR hIGear = kInvalidHandle;
	AT_ERRCOUNT tErrCount;
	try
	{
#if 0
		tErrCount = ::IG_load_file((char*)pImagePath, &hIGear);
#else
		//
		// REVIEW - This method loads in FlashPix images correctly
		// where as the previous method did not.  Note: It was copied
		// from AccuSoft's FlashPix example. (10/20/97 Lance) 
		//
		HFILE fd = _lopen( (char*)pImagePath, OF_READ ) ;	
		
		if (fd == HFILE_ERROR)
		{
			throw kAccessDenied ;
		}

		try
		{
			tErrCount = IG_load_FD( fd, 0, yIndex, 1, &hIGear );
		}
		catch (...)
		{
			tErrCount = 1;
		}

		_lclose( fd ) ;
#endif

		if (tErrCount > 0) ThrowExceptionOnLibraryError();



		// Get a pointer to the Dib
		AT_DIB* pDib;
		tErrCount = ::IG_image_DIB_pntr_get(hIGear, &pDib);
			if (tErrCount > 0) ThrowExceptionOnLibraryError();	

		// Initialize the bitmap
		pNewBitmap->Initialize(pDib);

		// Save the tranparancy data
//		SaveImageTransparancyColor(hIGear, pNewBitmap);	// Disabled since GIF should now always be buffered.

		// Delete the Accusoft handle
		::IG_image_delete(hIGear);		
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;

		delete pNewBitmap;
		if (hIGear != kInvalidHandle) ::IG_image_delete(hIGear);
		return NULL;
	}

	return pNewBitmap;
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
	// Clear the error stack
	ClearLastException();

	RBitmapImage* pNewBitmap = new RBitmapImage(TRUE);

	HIGEAR hIGear = kInvalidHandle;
	AT_ERRCOUNT tErrCount;
	try
	{
		tErrCount = ::IG_load_mem(rBuffer.GetBuffer(), rBuffer.GetBufferSize(), (UINT)yIndex, 0, &hIGear);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// Get a pointer to the Dib
		AT_DIB* pDib;
		tErrCount = ::IG_image_DIB_pntr_get(hIGear, &pDib);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();	

		// Initialize the bitmap
		if(GetImageFormat(rBuffer)== kImageFormatJPG)
		{
			pNewBitmap->Initialize(pDib, rBuffer.GetBuffer(),rBuffer.GetBufferSize());
		}
		else
		{
			pNewBitmap->Initialize(pDib);
		}

		// Save the tranparancy data
		SaveImageTransparancyColor(hIGear, pNewBitmap, rBuffer);

		// Delete the Accusoft handle
		::IG_image_delete(hIGear);
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;

		delete pNewBitmap;
		if (hIGear != kInvalidHandle) ::IG_image_delete(hIGear);
		return NULL;
	}

	
	return pNewBitmap;
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
	RImportFileBuffer rBuffer(rImagePath);
	return LoadVectorImage(rBuffer);
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
	HENHMETAFILE hEnhMetaFile = NULL;

	// See if it is a Windows metafile
	if (!hEnhMetaFile)
	{		
		// Set default width and height
		METAFILEPICT tMP;
		tMP.mm =		MM_ANISOTROPIC;
		tMP.xExt =	(sLONG)(::InchesToMillimeters((YFloatType)kDefaultVectorImageWidth /* / kSystemDPI */ ) * 100.0);				
		tMP.yExt =	(sLONG)(::InchesToMillimeters((YFloatType)kDefaultVectorImageHeight /* / kSystemDPI */) * 100.0);
		tMP.hMF =	NULL;

		// Convert the Windows metafile to an enhanced metafile
		hEnhMetaFile = ::SetWinMetaFileBits(rBuffer.GetBufferSize(), rBuffer.GetBuffer(), NULL, &tMP);
	}
		
	// If it's not a Windows metafile, check to see if it is an Aldus placeable metafile
	if (!hEnhMetaFile)
	{
		// Check for the presence of an Aldus placeable header
		RAPMFileHeader* pFileHeader = reinterpret_cast<RAPMFileHeader*>(rBuffer.GetBuffer());
		if (pFileHeader->uKey == kAldusPlaceableKey)
		{
			// Create a METAFILEPICT structure from the header and
			// use the info to create the enhanced metafile				
			SHORT sWidth =	(SHORT)(pFileHeader->rBoundingBox.Right - pFileHeader->rBoundingBox.Left);
			SHORT sHeight = (SHORT)(pFileHeader->rBoundingBox.Bottom - pFileHeader->rBoundingBox.Top);
			WORD uMUsPerInch = pFileHeader->sInch;			

			METAFILEPICT tMP;
			tMP.mm = MM_ANISOTROPIC;
			tMP.xExt = (uLONG)(::InchesToMillimeters((YFloatType)sWidth / (YFloatType)uMUsPerInch) * 100.0);
			tMP.yExt = (uLONG)(::InchesToMillimeters((YFloatType)sHeight / (YFloatType)uMUsPerInch) * 100.0);
			tMP.hMF = NULL;

			// Create an enhanced metafile from the bits
			hEnhMetaFile = ::SetWinMetaFileBits(rBuffer.GetBufferSize()- sizeof(RAPMFileHeader), rBuffer.GetBuffer() + sizeof(RAPMFileHeader), NULL, &tMP);
		}
	}

	// See if it is an enhanced metafile
	if (!hEnhMetaFile) hEnhMetaFile = ::SetEnhMetaFileBits(rBuffer.GetBufferSize(), rBuffer.GetBuffer());

	RVectorImage* pVector = NULL;
	if (hEnhMetaFile)
	{
		// It is an enhanced metafile; initialize this object
		pVector = new RVectorImage;
		pVector->Initialize((YImageHandle)hEnhMetaFile);			
	}
	return pVector;
}	

// ****************************************************************************
//
//  Function Name:	RImageLibrary::SaveBitmapToFlashPix()
//
//  Description:		This is currently a HACK, as the SaveBitmapImage
//							method cannot save a FlashPix image due to the
//                   Accusoft IF_save_file_calc_size function not
//                   handling this format type.
//
//  Returns:			TRUE if successful; otherwise FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RImageLibrary::SaveBitmapToFlashPix( RBitmapImage& rBitmap, const RMBCString& rImagePath )
{
	// Clear the error stack
	ClearLastException();

	AT_ERRCOUNT tErrCount;
	HIGEAR hIGear = kInvalidHandle;	
	try
	{
		// Ensure that the flash pix extension has been initialized.
		if (!gbFPXInitialized)
		{
			LoadExtension( kImageExtFPX, kFPXExtension, kFPXLicense, kFPXAuth ) ;
			gbFPXInitialized = TRUE;
		}

		// Create an Accusoft handle for the bitmap
		AT_DIB* pDib = (AT_DIB*)rBitmap.GetRawData();
		tErrCount = ::IG_image_import_DIB(pDib, &hIGear);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// Set the JPEG compression quality
		tErrCount =  IG_image_control_set(IG_CONTROL_FPX_QUALITY, (LPVOID) 100);
		tErrCount += IG_image_control_set(IG_CONTROL_FPX_SAVE_FROM_DIB, TRUE);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		CString strPath = rImagePath.operator CString( ) ;
		tErrCount =  ::IG_save_file( hIGear, strPath.GetBuffer(0), kAccusoftExportFormat[kImageFormatFPX] );
		if (tErrCount > 0) ThrowExceptionOnLibraryError();
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;

		// Clean up		
		if (hIGear != kInvalidHandle) ::IG_image_delete(hIGear);
		return FALSE;
	}
	
	return TRUE;
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
	if (kImageFormatFPX == eFormat)
	{
		// This is currently a HACK, as the SaveBitmapImage method 
		// cannot save a FlashPix image due to the Accusoft 
		// IF_save_file_calc_size function not handling this 
		// format type.		
		return SaveBitmapToFlashPix( rBitmap, rImagePath ) ;
	}

	RExportFileBuffer rBuffer(rImagePath);
	return SaveBitmapImage(rBitmap, rBuffer, eFormat);
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
	// Clear the error stack
	ClearLastException();

	AT_ERRCOUNT tErrCount;
	HIGEAR hIGear = kInvalidHandle;	

	try
	{
		// Create an Accusoft handle for the bitmap
		AT_DIB* pDib = (AT_DIB*)rBitmap.GetRawData();
		tErrCount = ::IG_image_import_DIB(pDib, &hIGear);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// Set the JPEG compression quality
		tErrCount = ::IG_image_control_set(IG_CONTROL_JPG_QUALITY, (LPVOID)kDefaultJpegCompressionQuality);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();
		
		// if saving a GIF file, must be no more than 8-bits per pixel
		if( kImageFormatGIF == eFormat && pDib->biBitCount > 8 )
		{
			tErrCount = ::IG_IP_color_reduce_median_cut( hIGear, FALSE, 256 );
			if (tErrCount > 0) ThrowExceptionOnLibraryError();
		}

		// Get the amount of memory required to export the image
		LONG sFileSize;
		tErrCount = ::IG_save_file_size_calc(hIGear, kAccusoftExportFormat[eFormat], &sFileSize);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();

		// Resize the memory-mapped buffer
		TpsAssert(sFileSize > 0, "::IG_file_size_calc() returned a negative file size.");
		rBuffer.Resize((uLONG)sFileSize);

		// Export the image
		DWORD uActualSize;
		tErrCount = ::IG_save_mem(hIGear, rBuffer.GetBuffer(), 0, sFileSize, 1, 0, (AT_LMODE)kAccusoftExportFormat[eFormat], &uActualSize);
		if (tErrCount > 0) ThrowExceptionOnLibraryError();
		
		// Clean up		
		::IG_image_delete(hIGear);
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;

		// Clean up		
		if (hIGear != kInvalidHandle) ::IG_image_delete(hIGear);
		return FALSE;
	}
	
	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	SaveVectorImage::SaveVectorImage()
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
	RExportFileBuffer rBuffer(rImagePath);
	return SaveVectorImage(rVector, rBuffer, eFormat);
}

// ****************************************************************************
//
//  Function Name:	SaveVectorImage::SaveVectorImage()
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
	RDrawingSurface* pDS = NULL;
	BOOLEAN fPrinterDS = FALSE;
	RPrinter* pDefaultPrinter = RPrintManager::GetPrintManager().GetDefaultPrinter();
	if (pDefaultPrinter && !pDefaultPrinter->IsTemplate())
	{
		pDS = pDefaultPrinter->CreateDrawingSurface( NULL );
		fPrinterDS = TRUE;
	}
	else
	{
		pDS = new RDefaultDrawingSurface;		
	}
	TpsAssert(pDS, "Could not create reference drawing surface.");		

	BOOLEAN fResult = FALSE;
	try
	{
		switch(eFormat)
		{
		case kImageFormatWMF:
			{
				// Compute the number of bytes we need to store the Windows metafile
				UINT uBufferSize = ::GetWinMetaFileBits((HENHMETAFILE)rVector.GetSystemHandle(), 0, NULL, MM_ANISOTROPIC, (HDC)pDS->GetSurface());

				// Resize the buffer
				rBuffer.Resize(uBufferSize);

				// Export the metafile
				UINT uResult = ::GetWinMetaFileBits((HENHMETAFILE)rVector.GetSystemHandle(), uBufferSize, rBuffer.GetBuffer(), MM_ANISOTROPIC, (HDC)pDS->GetSurface());
				if (!uResult) ::ThrowExceptionOnOSError();
				fResult = TRUE;
			}
			break;

		case kImageFormatEMF:
			{
				// Compute the number of bytes we need to store the enhanced metafile
				UINT uBufferSize = ::GetEnhMetaFileBits((HENHMETAFILE)rVector.GetSystemHandle(), 0, NULL);

				// Resize the buffer
				rBuffer.Resize(uBufferSize);

				// Export the metafile
				UINT uResult = ::GetEnhMetaFileBits((HENHMETAFILE)rVector.GetSystemHandle(), rBuffer.GetBufferSize(), rBuffer.GetBuffer());
				if (!uResult) ::ThrowExceptionOnOSError();
				fResult = TRUE;
			}
			break;

		case kImageFormatXRX:
			{
				// Compute the number of bytes we need to store the Windows metafile
				UINT uBufferSize = ::GetWinMetaFileBits((HENHMETAFILE)rVector.GetSystemHandle(), 0, NULL, MM_ANISOTROPIC, (HDC)pDS->GetSurface());

				// Create a buffer and size it appropriately			
				rBuffer.Resize(uBufferSize + sizeof(RAPMFileHeader));

				// Get the enhanced metafile header
				ENHMETAHEADER tEnhMetaFileHeader;
				::GetEnhMetaFileHeader((HENHMETAFILE)rVector.GetSystemHandle(), sizeof(ENHMETAHEADER), &tEnhMetaFileHeader);

				// Fill in an Aldus Placable Metafile header
				RAPMFileHeader* pAPMFileHeader = reinterpret_cast<RAPMFileHeader*>(rBuffer.GetBuffer());
				pAPMFileHeader->uKey = kAldusPlaceableKey;
				pAPMFileHeader->hMF = NULL;
				pAPMFileHeader->sInch = static_cast<WORD>(::InchesToMillimeters( 1 ) * 100);
				pAPMFileHeader->uReserved = 0;
				pAPMFileHeader->rBoundingBox.Left = static_cast<short>(tEnhMetaFileHeader.rclFrame.left);
				pAPMFileHeader->rBoundingBox.Top = static_cast<short>(tEnhMetaFileHeader.rclFrame.top);
				pAPMFileHeader->rBoundingBox.Right = static_cast<short>(tEnhMetaFileHeader.rclFrame.right);
				pAPMFileHeader->rBoundingBox.Bottom = static_cast<short>(tEnhMetaFileHeader.rclFrame.bottom);
				
				// Calcuate the APM checksum: start with the first word; XOR in each of the other 9 words
				WORD* pWord = reinterpret_cast<WORD*>(pAPMFileHeader);
				pAPMFileHeader->sChecksum = *pWord;			
				for(WORD i = 1; i <= 9; i++) pAPMFileHeader->sChecksum ^= pWord[i];

				// Export the metafile
				UINT uResult = ::GetWinMetaFileBits((HENHMETAFILE)rVector.GetSystemHandle(), uBufferSize, rBuffer.GetBuffer() + sizeof(RAPMFileHeader), MM_ANISOTROPIC, (HDC)pDS->GetSurface());
				if (!uResult) ::ThrowExceptionOnOSError();	
				fResult = TRUE;
			}
			break;
		}		
	}
	catch(YException nException)
	{
		// Set the last exception thrown
		m_nLastException = nException;
		fResult = FALSE;
	}
	
	// Clean up		
	if (fPrinterDS)
	{
		RPrinterDrawingSurface* pPDS = static_cast<RPrinterDrawingSurface*>(pDS);
		pDefaultPrinter->DestroyDrawingSurface(pPDS);	
	}
	else
	{
		delete pDS;
	}
	
	return fResult;
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
	AT_ERRCODE tErrCode;
	LONG sValue1, sValue2;
	::IG_error_get(0, NULL, 0, NULL, &tErrCode, &sValue1, &sValue2);
	
	try
	{
		switch(tErrCode)
		{			
		case IGE_OUT_OF_MEMORY:
			throw ::kMemory;

		case IGE_NO_BITMAP_FOUND:
		case IGE_INVALID_FILE_TYPE:
		case IGE_CANT_DETECT_FORMAT:
		case IGE_INVALID_IMAGE_FORMAT:
		case IGE_INVALID_BITCOUNT_FOR_FORMAT:
		case IGE_BAD_FILE_FORMAT:
			throw ::kDataFormatInvalid;

		case IGE_LZW_EXTENSION_NOT_LOADED:
			throw ::kUsesLZWCompression;

		case IGE_FILE_CANT_OPEN:
		case IGE_FILE_CANT_BE_OPENED:
		case IGE_FILE_CANT_BE_CREATED:		
		case IGE_FILE_CANT_SAVE:
		case IGE_CANT_READ_FILE:
			throw ::kAccessDenied;

		case IGE_FILE_INVALID_FILENAME:
			throw ::kInvalidPath;

		case IGE_EVAL_DLL_TIMEOUT_HAS_EXPIRED:
			throw ::kAccusoftEvalDLLExpired;

		default:
			throw ::kUnknownError;
		}
	}
	catch(YException nException)
	{
		m_nLastException = nException;
		throw;
	}	
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::GetLastException()
//
//  Description:		Returns the last exception thrown by the library
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YException RImageLibrary::GetLastException()
{
	return m_nLastException;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ClearLastException()
//
//  Description:		Clears the last exception thrown by the library.
//							Also clears the Accusoft error stack
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::ClearLastException()
{
	// Clear the error stack
	::IG_error_clear();

	m_nLastException = ::kNoError;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::ClearLastException()
//
//  Description:		Returns TRUE if the Accusoft format given is supported
//							by Renaissance.
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::IsSupportedFormat(int nAccusoftFormat)
{
	switch (nAccusoftFormat)
	{
	case IG_FORMAT_BMP:
	case IG_FORMAT_DIB:
	case IG_FORMAT_JPG:
	// case IG_FORMAT_PJPEG:
						// Accusoft 6.6.12 uses the same constant 
						// for JPG and PJPG
	case IG_FORMAT_PCD:
	case IG_FORMAT_PNG:
	case IG_FORMAT_TIF:
	case IG_FORMAT_WMF:
	case IG_FORMAT_XRX:
	case IG_FORMAT_EPS:	// GCB 12/17/97 supported for opening 
								// pre-5.0 projects (which can contain EPS)
	case IG_FORMAT_PSD:	// RIP 12/17/98 Added for Print Master support.
	case IG_FORMAT_PCX:
	case IG_FORMAT_WPG:
		return TRUE;
		break;
	
	case IG_FORMAT_FPX:

		if (m_uwExtFlags & kImageExtFPX)
			return TRUE;
		break;

	case IG_FORMAT_GIF:

		if (m_uwExtFlags & kImageExtGIF)
			return TRUE ;
		break;

	default:
		break;
	}

	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::LoadExtension()
//
//  Description:	Loads the specified accusoft extension
//
//  Returns:		TRUE if successful; otherwise FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RImageLibrary::LoadExtension( EImageExtFlag eExtFlag, LPCSTR szExtension, LPCSTR szLicense, LPCSTR szAuth )
{
	try
	{
		::IG_ext_unload( (LPSTR) szExtension ) ;

		if (!::IG_ext_load( (LPSTR) szExtension, (LPSTR) szLicense, (LPSTR) szAuth ))
		{
			m_uwExtFlags |= eExtFlag ;
			return TRUE ;
		}

		ThrowExceptionOnLibraryError() ;
	}
	catch(YException)
	{
	}

	return FALSE ;
}

// ****************************************************************************
//
//  Function Name:	RImageLibrary::IsFormatSupported()
//
//  Description:	Determines if the specified format is supported.
//
//  Returns:		TRUE if the format is supported	by Renaissance; 
//                  otherwise FALSE	
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::IsFormatSupported(EImageFormat eFormat)
{
	return IsSupportedFormat( kAccusoftExportFormat[eFormat] & 0x0000FFFF ) ;
}

// Disabled the possibly uninitialized warning since crTransparent and
// nTransparentIndex are enabled by IG_display_transparent_get or
// ReadTransparentIndex and bGIF doesn't change between these two calls.
#pragma warning( disable : 4701 )
// ****************************************************************************
//
//  Function Name:	RImageLibrary::SaveImageTransparancyColor()
//
//  Description:		Get the alpha data from the HIGEAR image
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::SaveImageTransparancyColor(HIGEAR hIGear, RBitmapImage* pNewBitmap, const RBuffer& rBuffer)
{
	BOOLEAN fResult = FALSE;
	BOOL	bTransparent;
	int		nTransparentIndex;
	int		nGifPaletteSize;

	// Determine if the image format is GIF based.
	AT_DIB	tDib;
	AT_MODE	tFileType, tCompression;
	AT_RGB crTransparent;
	AT_ERRCOUNT tError = ::IG_info_get_mem(rBuffer.GetBuffer(), rBuffer.GetBufferSize(), 1, &tFileType, &tCompression, &tDib);

	if (!tError)
	{
		bool bGIF = (tFileType == IG_FORMAT_GIF);
		if (bGIF)
		{
			// Construct a GIF reader so that we can extract the transparent index from
			// the memory buffer of GIF data.
			RGifReader GIFReader( rBuffer.GetBuffer(), rBuffer.GetBufferSize() );
			// Get the Transparent index.  (If the image isn't GIF no transparent index is found.)
			bTransparent = (GIFReader.ReadTransparentIndex( &nTransparentIndex, &nGifPaletteSize ) != FALSE);
		}
		else
		{
			tError = IG_display_transparent_get(hIGear, &crTransparent, &bTransparent);
		}

		// If the image was transparent, then process the image.
		if(!tError && bTransparent)
		{
			RSolidColor rcTransparent;
			if (bGIF)
			{
				rcTransparent = pNewBitmap->MakeTransparentColorUnique( nTransparentIndex, nGifPaletteSize );
			}
			else
			{
				rcTransparent = RSolidColor(crTransparent.r, crTransparent.g, crTransparent.b);
			}
			pNewBitmap->SetTransparentColor(rcTransparent);
			fResult = TRUE;
		}
	}
	return fResult;
}
#pragma warning( default : 4701 )
// ****************************************************************************
//
//  Function Name:	RImageLibrary::IsAnimatedGIF()
//
//  Description:		Returns whether the image buffer provided stores an animated GIF.
//
//  Returns:			TRUE if the buffer is an animated GIF image.  FALSE if not.
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageLibrary::IsAnimatedGIF(const RBuffer& rBuffer)
{
	RGifReader GIFReader( rBuffer.GetBuffer(), rBuffer.GetBufferSize() );

	return GIFReader.IsAnimatedGIF();
}


// ****************************************************************************
//
//  Function Name:	RImageDocument::CreateGIFReader()
//
//  Description:		Creates a GIF reader object and attaches the raw data of
//							the image file to it.  This allows external processes to
//							view the GIF data without the support of the component.
//
//  Returns:			A pointer to the new GIF reader, or NULL on failure.
//							It is the caller's responsibility to destroy the GIF reader
//							created.
//
//  Exceptions:		
//
// ****************************************************************************
RGifReader *RImageLibrary::CreateGIFReader(const RBuffer& rBuffer)
{
	RGifReader *pGifReader = new RGifReader( rBuffer.GetBuffer(), rBuffer.GetBufferSize() );

	return pGifReader;
}


// ****************************************************************************
//
//  Function Name:	RImageDocument::CreateGIFReader()
//
//  Description:		Creates a GIF animator object and attaches the raw data of
//							the image file to it.  This allows external processes to
//							view the GIF animation without the support of the component.
//
//  Returns:			A pointer to the new GIF animator, or NULL on failure.
//							It is the caller's responsibility to destroy the GIF
//							animator created.
//
//  Exceptions:		
//
// ****************************************************************************
CPGIFAnimator *RImageLibrary::CreateGIFAnimator(const RBuffer& rBuffer)
{
	CPGIFAnimator *pGifAnimator = new CPGIFAnimator( rBuffer.GetBuffer(), rBuffer.GetBufferSize() );

	return pGifAnimator;
}


// ****************************************************************************
//
//  Function Name:	RImageLibrary::AntiAliasWith50PercentReduction( )
//
//  Description:		Modifies the supplied image by reducing it by 50% in size
//							and averaging the pixels so that an anti-aliased effect is
//							produced.
//
//  Returns:			Nothing.
//
//  Exceptions:		
//
// ****************************************************************************
void RImageLibrary::AntiAliasWith50PercentReduction( RBitmapImage &rBitmap )
{
	// This algorithm is currently implemented for 24 bit images only.  To
	// support other bit depths, further code needs to be written.
	ASSERT( rBitmap.GetBitDepth() == 24 );
	if (rBitmap.GetBitDepth() != 24)
	{
		return;
	}

	// All bits except the lowest two of each byte.
	const uLONG kMask = 0x00FCFCFC;
	const uLONG kLowMask = 0x00030303;

	// Cache the image sizes.
	uLONG lWidth = rBitmap.GetWidthInPixels();
	uLONG lHeight = rBitmap.GetHeightInPixels();
	uLONG lNewWidth = lWidth / 2;
	uLONG lNewHeight = lHeight / 2;

	// Determine the number of bytes per row of image data.
	uLONG srcBytesPerRow = RBitmapImage::GetBytesPerRow( lWidth, 24 );
	uLONG dstBytesPerRow = RBitmapImage::GetBytesPerRow( lNewWidth, 24 );

	// Create a new image at the new size which will hold the final image.
	RBitmapImage rNewBitmap;
	rNewBitmap.Initialize( lNewWidth, lNewHeight, 24 );

	// Initialize our pointers to the image data.
	void* pSrcRawData = rBitmap.GetRawData();
	const uBYTE* pSrcRow = (uBYTE*)RBitmapImage::GetImageData( pSrcRawData );

	void* pDstRawData = rNewBitmap.GetRawData();
	uBYTE* pDstRow = (uBYTE*)RBitmapImage::GetImageData( pDstRawData );

	// walk the image two rows at a time and merge the image data.
	const uBYTE* pSrc1;
	const uBYTE* pSrc2;
	uBYTE* pDst = pDstRow;
	uLONG lAliasedColor;		// We accumulate the colors into this value.
	uLONG lAliasedLowBits;

	for (uLONG lRow = 0; lRow < lNewHeight; lRow++)
	{
		RCursor().SpinCursor();	// If this does take a while, make sure the cursor spins.

		// We are working with two rows at a time, so get both.
		pSrc1 = pSrcRow;
		pSrcRow += srcBytesPerRow;
		pSrc2 = pSrcRow;
		// Also get the row we will be writing into.
		pDst = pDstRow;
		for (uLONG lColumn = 0; lColumn < lNewWidth; lColumn++)
		{
			// Accumulate the four pixels which make up the new pixel into the
			// aliased color value.
			lAliasedColor = (*((uLONG*)pSrc1) & kMask);
			lAliasedLowBits = (*((uLONG*)pSrc1) & kLowMask);
			pSrc1 += 3;	// Get the neighbor pixel.
			lAliasedColor += (*((uLONG*)pSrc1) & kMask);
			lAliasedLowBits += (*((uLONG*)pSrc1) & kLowMask);
			pSrc1 += 3;	// Get the next passes pixel.
			lAliasedColor += (*((uLONG*)pSrc2) & kMask);
			lAliasedLowBits += (*((uLONG*)pSrc2) & kLowMask);
			pSrc2 += 3;	// Get the neighbor pixel.
			lAliasedColor += (*((uLONG*)pSrc2) & kMask);
			lAliasedLowBits += (*((uLONG*)pSrc2) & kLowMask);
			pSrc2 += 3;	// Get the next passes pixel.

			// Divide the accumulated value by 4 to get the average color.
			lAliasedColor = lAliasedColor >> 2;
			lAliasedLowBits = lAliasedLowBits >> 2;
			lAliasedLowBits &= kLowMask;	// Ensure we don't sweep down invalid bits.

			// Clear the existing bits in the RGB slots.
			*((uLONG*)pDst) &= 0xFF000000;
			// Now that the correct bits are cleared, OR in the final data.
			*((uLONG*)pDst) |= lAliasedColor;
			*((uLONG*)pDst) |= lAliasedLowBits;
			// Increment to the next pixel to set.
			pDst += 3;
		}

		// Increment the row pointers.
		pSrcRow += srcBytesPerRow;
		pDstRow += dstBytesPerRow;
	}

	// We're complete, replace the original bitmap with the new one.
	rBitmap = rNewBitmap;
}

#endif //_WINDOWS