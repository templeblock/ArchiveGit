// *****************************************************************************
//
// File name:			WinBitmapImage.cpp
//
// Project:				Renaissance Framework
//
// Author:				G. Brown
//
// Description:		Windows-specific definition of the RBitmapImage class members
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
//  $Logfile:: /PM8/Framework/Source/WinBitmapImage.cpp                       $
//   $Author:: Lwilson                                                        $
//     $Date:: 3/08/99 7:16p                                                  $
// $Revision:: 2                                                              $
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#ifdef	MAC
	#error	"This file can only be compilied under Windows
#endif	//	MAC

#include "BitmapImage.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "MemoryMappedBuffer.h"
#include "DcDrawingSurface.h"
#include "ChunkyStorage.h"
#include "DataTransfer.h"
#include "OffscreenDrawingSurface.h"
#include "DefaultDrawingSurface.h"
#include "FileStream.h"
#include "ApplicationGlobals.h"
#include "ResourceManager.h"
#include "Path.h"
#include "View.h"	// Access to lo resolution bitmap DPI definition.

const DWORD kDefaultRedMask16 =		0x00007C00;
const DWORD kDefaultGreenMask16 =	0x000003E0;
const DWORD kDefaultBlueMask16 =		0x0000001F;

const DWORD kDefaultRedMask32 =		0x00FF0000;
const DWORD kDefaultGreenMask32 =	0x0000FF00;
const DWORD kDefaultBlueMask32 =		0x000000FF;

const	int kOutline =						1;
const int kFrame =						2;
const int kMaxClipRecords =			1100;	
const int kBytesPerRecord =			26;
const int kBytesFollowingSelector = 24;
const int kSections =					1; 
const int kScalingFactor =				0x01000000;
const int kFillRuleRecord =			0x0600;
const BYTE kByteFillRuleRecord =		0x06;
// ****************************************************************************
//
//  Function Name:	RBitmapImage::RBitmapImage()
//
//  Description:		Constructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RBitmapImage::RBitmapImage(BOOLEAN fStoreOnDisk)
	: m_pDibBuffer( NULL ),
	  m_hBitmap( NULL ), 
	  m_rcTransparentColor( kWhite ), 
	  m_bTransparent( FALSE ),
	  m_nLockCount( 0 ),
	  m_pAlphaChannel( NULL )
{
	if (fStoreOnDisk)
		m_pDibBuffer = new RTempFileBuffer;
	else
		m_pDibBuffer = new RPageFileBuffer;

	m_hBitmap = NULL;		
	m_pClipPath = NULL;
	m_pFramePath = NULL;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::RBitmapImage()
//
//  Description:		Constructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RBitmapImage::RBitmapImage(const RBitmapImage& rhs)
	: m_pDibBuffer( NULL ), 
	  m_hBitmap( NULL ), 
	  m_rcTransparentColor( rhs.m_rcTransparentColor ),
	  m_bTransparent( rhs.m_bTransparent ),
	  m_nLockCount( 0 ),
	  m_pAlphaChannel( NULL )
{
	// See what kind of buffer the original has
	if (!m_pDibBuffer)
	{
		RTempFileBuffer* pTempFileBuffer = dynamic_cast<RTempFileBuffer*>(rhs.m_pDibBuffer);
		if (pTempFileBuffer) m_pDibBuffer = new RTempFileBuffer;
	}
	if (!m_pDibBuffer)
	{
		RPageFileBuffer* pPageFileBuffer = dynamic_cast<RPageFileBuffer*>(rhs.m_pDibBuffer);
		if (pPageFileBuffer) m_pDibBuffer = new RPageFileBuffer;
	}
	// Copy the Dib buffer
	m_pDibBuffer->Resize(rhs.m_pDibBuffer->GetBufferSize());	
	memcpy(m_pDibBuffer->GetBuffer(), rhs.m_pDibBuffer->GetBuffer(), rhs.m_pDibBuffer->GetBufferSize());	

	// Create the paths if present
	m_pClipPath = NULL;

	if(rhs.m_pClipPath)
	{
		m_pClipPath = new RClippingPath( *rhs.m_pClipPath );
	}

	m_pFramePath = NULL;
	if(rhs.m_pFramePath)
	{
		m_pFramePath = new RPath(*rhs.m_pFramePath);
	}
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::~RBitmapImage()
//
//  Description:		Destructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RBitmapImage::~RBitmapImage()
{	
	Uninitialize();
	delete m_pDibBuffer;
	delete m_pClipPath;
	delete m_pFramePath;
	delete m_pAlphaChannel;
}	

// ****************************************************************************
//
//  Function Name:	RBitmapImage::Initialize()
//
//  Description:		(Re)creates the bitmap with the given attributes
//
//  Returns:			
//
//  Exceptions:		kMemory; kDiskFull; kUnknownError
//
// ****************************************************************************
void RBitmapImage::Initialize(uLONG uWidthInPixels, uLONG uHeightInPixels, uLONG uBitDepth, uLONG uXDpi /* = kDefaultXDpi */, uLONG uYDpi /* = kDefaultYDpi */, PaletteOption paletteOption /* = kUseApplicationPalette */)
{
	TpsAssert(uWidthInPixels > 0, "Bitmap cannot have 0 width.");
	TpsAssert(uHeightInPixels > 0, "Bitmap cannot have 0 height.");
	TpsAssert(((uBitDepth == 1) || (uBitDepth == 4) || (uBitDepth == 8) || (uBitDepth == 16) || (uBitDepth == 24) || (uBitDepth == 32)), "Invalid bit depth.");

	// Must always call Uninitialize() to clean up any previous DIB section
	Uninitialize();
		
	uLONG uColorDataSize = GetColorDataSize(uBitDepth);
	uLONG uImageDataSize = GetImageDataSize(uWidthInPixels, uHeightInPixels, uBitDepth);
	m_pDibBuffer->Resize(sizeof(BITMAPINFOHEADER) + uColorDataSize + uImageDataSize);
	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();

	// Fill in the BITMAPINFO structure	
	pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBitmapInfo->bmiHeader.biWidth = (LONG)uWidthInPixels;
	pBitmapInfo->bmiHeader.biHeight = (LONG)uHeightInPixels;
	pBitmapInfo->bmiHeader.biPlanes = 1;
	pBitmapInfo->bmiHeader.biBitCount = (WORD)uBitDepth;
	pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	pBitmapInfo->bmiHeader.biSizeImage = (DWORD)uImageDataSize;
	pBitmapInfo->bmiHeader.biXPelsPerMeter = (LONG)::DpiToPelsPerMeter(uXDpi);
	pBitmapInfo->bmiHeader.biYPelsPerMeter = (LONG)::DpiToPelsPerMeter(uYDpi);
	pBitmapInfo->bmiHeader.biClrUsed = 0;
	pBitmapInfo->bmiHeader.biClrImportant = 0;
		
	switch (uBitDepth)
	{
	case 1:
		{
			RGBQUAD* pRgbQuad = (RGBQUAD*)GetColorData(pBitmapInfo);	
			pRgbQuad[0] = GetRgbQuad(0x00, 0x00, 0x00);
			pRgbQuad[1] = GetRgbQuad(0xFF, 0xFF, 0xFF);
		}
		break;

	case 4:
		{
			RGBQUAD* pRgbQuad = (RGBQUAD*)GetColorData(pBitmapInfo);	
			TpsAssertAlways("4bpp bitmap palettes are black and white.");
			uBYTE uEntry;
			for (uEntry = 0; uEntry < 8; uEntry++) pRgbQuad[uEntry] = GetRgbQuad(0x00, 0x00, 0x00);
			for (uEntry = 8; uEntry < 16; uEntry++) pRgbQuad[uEntry] = GetRgbQuad(0xFF, 0xFF, 0xFF);
		}
		break;

	case 8:
		{
			uWORD uwNumAppPaletteEntries;
			RGBQUAD* pRgbQuad = (RGBQUAD*)GetColorData(pBitmapInfo);	
			if (paletteOption == kUseGrayScalePalette)
			{
				// Fill in the color table with a linear gray scale ramp
				for (int i = 0; i < 256; i++)
					pRgbQuad[i] = GetRgbQuad(i, i, i);
			}
			else
			{
				// Copy in the Application screen palette.
				//	Use tbitGetColorTable so that we get a copy of the colors
				//	that have not been modified by selection into a dc.
				RGBQUAD*	pAppPaletteRGB = (RGBQUAD*)tmemDeRef( tbitGetColorTable( &uwNumAppPaletteEntries ) );
				TpsAssert(uwNumAppPaletteEntries == 256, "Bad application palette.");
				for (uWORD uEntry = 0; uEntry < 256; uEntry++) pRgbQuad[uEntry] = pAppPaletteRGB[uEntry];
			}
		}
		break;

	case 16:
		{
			pBitmapInfo->bmiHeader.biCompression = BI_BITFIELDS;
			RGBQUAD* pRgbQuad = (RGBQUAD*)GetColorData(pBitmapInfo);	
			*((DWORD*)&pRgbQuad[0]) = kDefaultRedMask16;
			*((DWORD*)&pRgbQuad[1]) = kDefaultGreenMask16;
			*((DWORD*)&pRgbQuad[2]) = kDefaultBlueMask16;
		}
		break;

	case 32:
		{
			pBitmapInfo->bmiHeader.biCompression = BI_BITFIELDS;
			RGBQUAD* pRgbQuad = (RGBQUAD*)GetColorData(pBitmapInfo);	
			*((DWORD*)&pRgbQuad[0]) = kDefaultRedMask32;
			*((DWORD*)&pRgbQuad[1]) = kDefaultGreenMask32;
			*((DWORD*)&pRgbQuad[2]) = kDefaultBlueMask32;
		}
		break;
	}
}

// ****************************************************************************
//
//  Function Name:	Initialize()
//
//  Description:		(Re)creates the bitmap to be compatible with the given
//							drawing surface
//
//  Returns:			
//
//  Exceptions:		kMemory; kDiskFull; kUnknownError
//
// ****************************************************************************
void RBitmapImage::Initialize(RDrawingSurface& rDS, uLONG uWidthInPixels, uLONG uHeightInPixels)
{	
	// Must always call Uninitialize() to clean up any previous DIB section
	Uninitialize();

	// Create an HBITMAP compatible with the DC of the DS at the size given	
	HDC hDC = (HDC)rDS.GetSurface();
	TpsAssert(hDC, "rDS has no DC.");
	
	HBITMAP hBitmap = NULL;
	try
	{
		TpsAssert(uWidthInPixels > 0, "Bitmap cannot have 0 width.");
		TpsAssert(uHeightInPixels > 0, "Bitmap cannot have 0 height.");
		hBitmap = ::CreateCompatibleBitmap(hDC, uWidthInPixels, uHeightInPixels);		
		if (!hBitmap) throw ::kMemory;		

		// Get a DIB-format header from the DDB
		BITMAPINFO tBitmapInfo;
		tBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		tBitmapInfo.bmiHeader.biBitCount = 0;

		if (!::GetDIBits(hDC, hBitmap, 0, 0, NULL, &tBitmapInfo, DIB_RGB_COLORS))
			throw ::kMemory;
		
		// Compute the size of the color table and image data
		m_pDibBuffer->Resize(GetDibSize(&tBitmapInfo));				

		//	Copy the tBitmapInfo.bmiHeader to pRawData
		memcpy(m_pDibBuffer->GetBuffer(), &tBitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

		// Get the bits for real
		if (!::GetDIBits(hDC, hBitmap, 0, 
			tBitmapInfo.bmiHeader.biHeight, GetImageData(m_pDibBuffer->GetBuffer()), 
			(BITMAPINFO*)m_pDibBuffer->GetBuffer(), DIB_RGB_COLORS)) throw ::kMemory;
		
		// Clean up
		::DeleteObject(hBitmap);		
	}
	catch(YException)
	{		
		if (hBitmap)
			::DeleteObject(hBitmap);			
		throw;
	}
}

// ****************************************************************************
//
//  Function Name:	Initialize()
//
//  Description:		Initializes the RBitmapImage to be compatible with the
//							YImageHandle.  Does not take ownership of the resource
//							bitmap.
//
//  Returns:			
//
//  Exceptions:		kMemory; kDiskFull; kUnknownError
//
// ****************************************************************************
void RBitmapImage::Initialize(YImageHandle yHandle)
{
	// Must always call Uninitialize() to clean up any previous DIB section
	Uninitialize();

	HBITMAP			hBitmap		= (HBITMAP)yHandle; // Cast to HBITMAP

#ifdef	TPSDEBUG
static	bool bTest = 0;
if ( bTest )
{
DIBSECTION		bmInfo;
::GetObject( hBitmap, sizeof( DIBSECTION ), &bmInfo );
HDC		hdc		= ::GetDC( HWND_DESKTOP );
HDC		hdcTmp	= ::CreateCompatibleDC( hdc );
HBITMAP	hbmOld	= (HBITMAP)::SelectObject( hdcTmp, hBitmap );
::BitBlt( hdc, 0, 0, bmInfo.dsBm.bmWidth, bmInfo.dsBm.bmHeight, hdcTmp, 0, 0, SRCCOPY ); 
::SelectObject( hdcTmp, hbmOld );
::DeleteDC( hdcTmp );
::ReleaseDC( HWND_DESKTOP, hdc );
}
#endif	//TPSDEBUG

	// Get a DIB-format header from the DDB
	BITMAPINFO tBitmapInfo;
	tBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	tBitmapInfo.bmiHeader.biBitCount = 0;

	RDefaultDrawingSurface rDefaultDS;
	if (!::GetDIBits((HDC)rDefaultDS.GetSurface(), hBitmap, 0, 0, NULL, &tBitmapInfo, DIB_RGB_COLORS))
		throw ::kMemory;
	
	// Compute the size of the color table and image data
	m_pDibBuffer->Resize(GetDibSize(&tBitmapInfo));				

	//	Copy the tBitmapInfo.bmiHeader to pRawData
	memcpy(m_pDibBuffer->GetBuffer(), &tBitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

	// Get the bits for real
	if (!::GetDIBits((HDC)rDefaultDS.GetSurface(), hBitmap, 0, 
		tBitmapInfo.bmiHeader.biHeight, GetImageData(m_pDibBuffer->GetBuffer()), 
		(BITMAPINFO*)m_pDibBuffer->GetBuffer(), DIB_RGB_COLORS)) throw ::kMemory;
	
}

// ****************************************************************************
//
//  Function Name:	Initialize()
//
//  Description:		Initializes the bitmap with DIB data from the resource file
//
//  Returns:			
//
//  Exceptions:		kMemory; kDiskFull; kUnknownError
//
// ****************************************************************************
void RBitmapImage::Initialize(YResourceId nId)
{
	// Must always call Uninitialize() to clean up any previous DIB section
		//	Uninitialize will be called with the below call to Initialize

	// Get the resource bitmap
	YResourceBitmap hBitmap = ::GetResourceManager().GetResourceBitmap(nId);

	// Initialize the DIB section
	Initialize(reinterpret_cast<YImageHandle>(hBitmap));

	// Free the resource bitmap
	::GetResourceManager().FreeResourceBitmap(hBitmap);
}

// ****************************************************************************
//
//  Function Name:	Initialize()
//
//  Description:		Initializes the RBitmapImage as a copy of the Dib pointed
//							to by pRawData
//
//  Returns:			
//
//  Exceptions:		kMemory; kDiskFull; kUnknownError
//
// ****************************************************************************
void RBitmapImage::Initialize(void* pRawData)
{
	TpsAssert(GetDibSize(pRawData) > 0, "Not a valid Windows DIB.");

	// Must always call Uninitialize() to clean up any previous DIB section
	Uninitialize();

	// Initialize the bitmap by copying the data and initializing the DibSection
	m_pDibBuffer->Resize(GetDibSize(pRawData));
	memcpy(m_pDibBuffer->GetBuffer(), pRawData, m_pDibBuffer->GetBufferSize());
}
// ****************************************************************************
//
//  Function Name:	Initialize()
//
//  Description:		Initializes the RBitmapImage as a copy of the Dib pointed
//							to by pRawData. Detects and saves the clipping path.
//
//  Returns:			
//
//  Exceptions:		kMemory; kDiskFull; kUnknownError
//
// ****************************************************************************
void RBitmapImage::Initialize(void* pRawData, void* pBuffer, int nBufferSize)
{
	Initialize(pRawData);

	// First look for the 1st path resouce.

	if(FindPathInData(pBuffer, nBufferSize, kOutline))
		{
		m_pClipPath = (RClippingPath *) CreateRPath();

		// We found a path resource, lets see if the next one is a frame.

		if(FindPathInData(pBuffer, nBufferSize, kFrame))
		{
			m_pFramePath = CreateRPath();
		}
	}
}

// ****************************************************************************
//
//  Function Name:	Uninitialize()
//
//  Description:		Clears all the internal bitmap data
//
//  Returns:			
//
//  Exceptions:		kMemory; kDiskFull; kUnknownError
//
// ****************************************************************************
void RBitmapImage::Uninitialize()
{
	TpsAssert( m_hBitmap == NULL, "Uninitializing bitmap that is still Locked" );
	if (m_pDibBuffer)
		m_pDibBuffer->Empty();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::Render()
//
//  Description:		Displays the image on the drawing surface
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::Render(RDrawingSurface& rDS, const RIntRect& rDestRect)
{
	RIntRect rSrcRect(0, 0, GetWidthInPixels(), GetHeightInPixels());
	Render(rDS, rSrcRect, rDestRect);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::Render()
//
//  Description:		Displays the image on the drawing surface
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::Render(RDrawingSurface& rDS, const RIntRect& rSrcRect, const RIntRect& rDestRect)
{
	TpsAssert(rDS.GetSurface(), "Drawing surface has no DC.");
	TpsAssert(m_pDibBuffer->GetBuffer(), "Raw data buffer is empty.");

	if (m_pAlphaChannel)
	{
		TpsAssert( m_pAlphaChannel->GetBitDepth() == 1, "Unsupported bit depth!" );
		RenderWithMask( rDS, *m_pAlphaChannel, rSrcRect, rDestRect );
	}
	else if(m_bTransparent)
	{
		RenderTransparent( rDS, rSrcRect, rDestRect );
	}
	else if (rDS.IsPrinting())
	{
		RenderSection( rDS, rSrcRect, rDestRect );
	}	
	else
	{	
		ROffscreenDrawingSurface rBitmapDS;

		rBitmapDS.SetImage(this);			
		rDS.BlitDrawingSurface(rBitmapDS, rSrcRect, rDestRect, kBlitSourceCopy);
		rBitmapDS.ReleaseImage();
	}
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::RenderWithMask()
//
//  Description:		Displays the unmasked portion of the image on the
//							drawing surface
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::RenderWithMask(RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rDestRect)
{
	RIntRect rSrcRect(0, 0, GetWidthInPixels(), GetHeightInPixels());
	RenderWithMask(rDS, rMaskBitmap, rSrcRect, rDestRect);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::RenderWithMask()
//
//  Description:		Displays the unmasked portion of the image on the
//							drawing surface
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::RenderWithMask(RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rSrcRect, const RIntRect& rDestRect)
{
	TpsAssert(rMaskBitmap.GetWidthInPixels() == GetWidthInPixels(), "Mask width does not match bitmap width.");
	TpsAssert(rMaskBitmap.GetHeightInPixels() == GetHeightInPixels(), "Mask height does not match bitmap height.");
	TpsAssert(rDS.GetSurface(), "rDS.GetSurface() = NULL.");
	TpsAssertIsObject( RBitmapImage, &rMaskBitmap );

	LockImage( );

	try
	{
		HDC hOutputDC = (HDC)rDS.GetSurface();	
		if (rDS.IsPrinting() && !rDS.GetFullPanelBitmapPrintingMode())
		{
			// When printing and not in full panel bitmap printing mode,
			// we scan through the mask a line at a time because we
			// can't mask blit to a printing drawing surface
			uBYTE** ppMaskData = Get2dImageData(static_cast<RBitmapImage&>( rMaskBitmap ).GetRawData());
			YFloatType yScaleX = (YFloatType)rDestRect.Width() / (YFloatType)rSrcRect.Width();
			YFloatType yScaleY = (YFloatType)rDestRect.Height() / (YFloatType)rSrcRect.Height();

			YIntCoordinate ySrcY, yDestX, yDestY;
			YIntDimension yDestLineWidth, yDestLineHeight;
			
			yDestLineHeight = (YIntDimension)yScaleY + 1;
			for(ySrcY = rSrcRect.m_Top; ySrcY < rSrcRect.m_Bottom; ++ySrcY)
			{	
				RCursor().SpinCursor();

				uBYTE* pMaskBmpLine = ppMaskData[ySrcY] + (rSrcRect.m_Left >> 3);
				YIntCoordinate	yLeft = rSrcRect.m_Left;
				yDestY = (YIntCoordinate)((YRealCoordinate)ySrcY * yScaleY);		
				while(yLeft < rSrcRect.m_Right)
				{
					//
					// Find the first non-zero mask pixel...
					yLeft = SkipZeroBits(pMaskBmpLine, yLeft, rSrcRect.m_Right);
					if(yLeft < rSrcRect.m_Right)
					{
						//
						// find the last (+1) non-zero mask pixel
						YIntCoordinate	yRight = SkipOneBits(pMaskBmpLine, yLeft, rSrcRect.m_Right);
						//
						// Draw the image
						yDestX = (YIntCoordinate)((YRealCoordinate)yLeft * yScaleX);
						yDestLineWidth = (YIntCoordinate)((YRealCoordinate)(yRight - yLeft) * yScaleX) + 1;

						RIntRect rSR, rDR;
						rSR.m_Left		= yLeft;
						rSR.m_Top		= ySrcY;
						rSR.m_Right		= yRight;
						rSR.m_Bottom	= ySrcY + 1;

						rDR.m_Left		= rDestRect.m_Left + yDestX;
						rDR.m_Top		= rDestRect.m_Bottom - yDestY;
						rDR.m_Right		= rDR.m_Left + yDestLineWidth;
						rDR.m_Bottom	= rDR.m_Top + yDestLineHeight;

						RenderSection(rDS, rSR, rDR);
						//
						// Reset for next pass...
						yLeft	= yRight;
					}
				}
			}

			delete ppMaskData;
		}
		else
		{	
			ROffscreenDrawingSurface rMaskDS, rSourceDS;				
			RBitmapImage rTempBitmap;

			// Select in our image.  Don't do this later, we can't test the DS without an image selected.
			rSourceDS.SetImage( this );
			
			// Is the drawing surface or bitmap 8-bit?
			if ((GetBitDepth() <= 8) || (rSourceDS.GetBitDepth() <= 8))
			{
				// Ok, we need to use a temporary bitmap which has been created on
				// the DS to paint with since then it will have the correctly mapped palette.
				rTempBitmap.Initialize(GetWidthInPixels(), GetHeightInPixels(), rSourceDS.GetBitDepth());

				// OK, we've set up our temporary image, so release the bitmap from
				// our source drawing surface and select it into a temporary DS.
				rSourceDS.ReleaseImage();
				ROffscreenDrawingSurface rODS;
				rODS.SetImage(this);

				// Set up our temporary image in the source DS and copy over the bitmap.
				rSourceDS.SetImage(&rTempBitmap);
				rSourceDS.BlitDrawingSurface( rODS, rSrcRect, rSrcRect, kBlitSourceCopy );

				// Release the original bitmap again, we don't require its services further.
				rODS.ReleaseImage();
			}
			rMaskDS.SetImage(&rMaskBitmap);

			RCursor().SpinCursor();

			// 1) Cut a hole in the destination drawing surface
			rDS.BlitDrawingSurface(rMaskDS, rSrcRect, rDestRect, kBlitNotSourceAnd);
			
			// 2) Isolate this image using the mask
			rSourceDS.BlitDrawingSurface(rMaskDS, rSrcRect, rSrcRect, kBlitSourceAnd);
					
			// 3) Copy the isolated image to the output of this drawing surface
			rDS.BlitDrawingSurface(rSourceDS, rSrcRect, rDestRect, kBlitSourcePaint);		

			// Release the images		
			rSourceDS.ReleaseImage();
			rMaskDS.ReleaseImage();
		}
	}
	catch( ... )
	{
		;	//	Just do nothing... more...
	}

	UnlockImage( );
}
// ****************************************************************************
//
//  Function Name:	RBitmapImage::RenderAsObject()
//
//  Description:		Implements the Photo Objects requirment. Displays the area defined
//							by the clipping path onto the	drawing surface.
//							
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void	RBitmapImage::RenderAsObject(	RDrawingSurface& rDS, 
										RBitmapImageBase& rMaskBitmap,
										const RIntRect& rDestRect,
										const RRealSize& rSize, 
										const R2dTransform& 
										rTransform, 
										RPath* path,
										bool bUseHiResBitmap )
{
	LockImage( );

	try
	{
		RIntRect rSrcRect(0, 0, GetWidthInPixels(), GetHeightInPixels());

		TpsAssert(rMaskBitmap.GetWidthInPixels() == GetWidthInPixels(), "Mask width does not match bitmap width.");
		TpsAssert(rMaskBitmap.GetHeightInPixels() == GetHeightInPixels(), "Mask height does not match bitmap height.");
		TpsAssert(rDS.GetSurface(), "rDS.GetSurface() = NULL.");
		TpsAssertIsObject( RBitmapImage, &rMaskBitmap );

		// Modify the mask so we can use it to hold the filled rPath.
		//	kBlack makes everything transparent, then open up just the clipping area
		// with the path data in kWhite.

		ROffscreenDrawingSurface rMaskDS;				
		rMaskDS.SetImage(&rMaskBitmap);
		rMaskDS.SetFillColor(RSolidColor(kBlack));
		RIntRect rMaskRect(0, 0, GetWidthInPixels(), GetHeightInPixels());
		rMaskDS.FillRectangle(rMaskRect);

		// Make a copy of the transform

		R2dTransform pTransform = rTransform;

		//Render the kWhite filled path into the maskDS using a black brush 2 pixels wide to stroke/shrink the mask.

		// Adjust the path transform so that it will not draw bigger than the image
		// if limited by our maximum printing resolution for images.
		if (rDS.IsPrinting())
		{
			R2dTransform rDPITransform = rTransform;
			
			// Scale the transform so that we are mapping pixels to device units	
			rDPITransform.PreScale((YFloatType)rSize.m_dx / (YFloatType)GetWidthInPixels(), (YFloatType)rSize.m_dy / (YFloatType)GetHeightInPixels());
			
			// Translate the origin so that the rotated bitmap is mapped into the right place in the destination bitmap
			rDPITransform.PostTranslate(-rDestRect.m_Left, -rDestRect.m_Top);

			// Determine our DPI.
			RRealVectorRect rBitmapRect(0, 0, GetWidthInPixels(), GetHeightInPixels());
			rBitmapRect *= rDPITransform;
			RRealSize rSizeInInches(::LogicalUnitsToInches(rSize.m_dx), ::LogicalUnitsToInches(rSize.m_dy));
			RRealSize rDpi((YIntDimension)((YRealDimension)rBitmapRect.Width() / rSizeInInches.m_dx), (YIntDimension)((YRealDimension)rBitmapRect.Height() / rSizeInInches.m_dy));
			// Remove the offsets so we can align the render into the rMaskDS (rMaskBitmap)
			pTransform.PostTranslate(-rDestRect.m_Left, -rDestRect.m_Top );

			// limit the bitmap resolution to either
			// kLoResBitmapXDpi, kLoResBitmapYDpi or kHiResBitmapXDpi, kHiResBitmapYDpi
			// depending on user preference in registry
			YRealDimension bitmapXDpi = kLoResBitmapXDpi;
			YRealDimension bitmapYDpi = kLoResBitmapYDpi;
			if (bUseHiResBitmap)
			{
				bitmapXDpi = kHiResBitmapXDpi;
				bitmapYDpi = kHiResBitmapYDpi;
			}
			if (rDpi.m_dx > bitmapXDpi) pTransform.PostScale((YFloatType)bitmapXDpi / (YFloatType)rDpi.m_dx, 1.0);
			if (rDpi.m_dy > bitmapYDpi) pTransform.PostScale(1.0, (YFloatType)bitmapYDpi / (YFloatType)rDpi.m_dy);
		}
		else
		{
			// Remove the offsets so we can align the render into the rMaskDS (rMaskBitmap)
			pTransform.PostTranslate(-rDestRect.m_Left, -rDestRect.m_Top );
		}
	
		RIntRect NotUsed(0,0,0,0);	// Bounds checker complains about passing a NULL to this param.
		path->Render(rMaskDS, rSize, pTransform, NotUsed, kFilledAndStroked, kBlack, kWhite, 2);

		if (rDS.IsPrinting() && !rDS.GetFullPanelBitmapPrintingMode())
		{
			// When printing and not in full panel bitmap printing mode,
			// we scan through the mask a line at a time because we
			// can't mask blit to a printing drawing surface

			uBYTE** ppMaskData = Get2dImageData(static_cast<RBitmapImage&>( rMaskBitmap ).GetRawData());		
			YFloatType yScaleX = (YFloatType)rDestRect.Width() / (YFloatType)rSrcRect.Width();
			YFloatType yScaleY = (YFloatType)rDestRect.Height() / (YFloatType)rSrcRect.Height();

			YIntCoordinate ySrcY, yDestX, yDestY;
			YIntDimension yDestLineWidth, yDestLineHeight;
			
			yDestLineHeight = (YIntDimension)yScaleY + 1;

			for(ySrcY = rSrcRect.m_Top; ySrcY < rSrcRect.m_Bottom; ++ySrcY)
			{	
				RCursor( ).SpinCursor( );

				uBYTE* pMaskBmpLine = ppMaskData[ySrcY] + (rSrcRect.m_Left >> 3);
				YIntCoordinate	yLeft = rSrcRect.m_Left;
				yDestY = (YIntCoordinate)((YRealCoordinate)ySrcY * yScaleY);		
				while(yLeft < rSrcRect.m_Right)
				{
					// Find the first non-zero mask pixel...

					yLeft = SkipZeroBits(pMaskBmpLine, yLeft, rSrcRect.m_Right);
					if(yLeft < rSrcRect.m_Right)
					{
						// Find the last (+1) non-zero mask pixel

						YIntCoordinate	yRight = SkipOneBits(pMaskBmpLine, yLeft, rSrcRect.m_Right);

						// Draw the image

						yDestX = (YIntCoordinate)((YRealCoordinate)yLeft * yScaleX);
						yDestLineWidth = (YIntCoordinate)((YRealCoordinate)(yRight - yLeft) * yScaleX) + 1;

						RIntRect rSR, rDR;
						rSR.m_Left		= yLeft;
						rSR.m_Top		= ySrcY;
						rSR.m_Right		= yRight;
						rSR.m_Bottom	= ySrcY + 1;

						rDR.m_Left		= rDestRect.m_Left + yDestX;
						rDR.m_Top		= rDestRect.m_Bottom - yDestY;
						rDR.m_Right		= rDR.m_Left + yDestLineWidth;
						rDR.m_Bottom	= rDR.m_Top + yDestLineHeight;

						Render(rDS, rSR, rDR);

						// Reset for next pass...

						yLeft	= yRight;
					}
				}
			}
			delete ppMaskData;
		}
		else // Not printing
		{
			// Set the source image into the source DS

			ROffscreenDrawingSurface rSourceDS;				
			rSourceDS.SetImage(this);			

			// Now blit the image using the modified mask

			RCursor( ).SpinCursor( ); //WB What does this have to do with the mask blits?????

			// 1) Cut a hole in the destination drawing surface
			rDS.BlitDrawingSurface(rMaskDS, rSrcRect, rDestRect, kBlitNotSourceAnd);
			
			// 2) Isolate this image using the mask
			rSourceDS.BlitDrawingSurface(rMaskDS, rSrcRect, rSrcRect, kBlitSourceAnd);
					
			// 3) Copy the isolated image to the output of this drawing surface
			rDS.BlitDrawingSurface(rSourceDS, rSrcRect, rDestRect, kBlitSourcePaint);

			// To test that the path can be correctly aligned, scaled and rotated to the image.
			//	path->Render(rDS, rSize, rTransform, rSrcRect, kFilledAndStroked, kBlack,  kBlack);

			// Release the images		
			rSourceDS.ReleaseImage();
		}
		rMaskDS.ReleaseImage();
	}
	catch( ... )
	{
		;	//	Do nothing else for now...
	}

	UnlockImage( );
}
// ****************************************************************************
//
//  Function Name:	RBitmapImage::Read()
//
//  Description:		(Re)creates the bitmap from the information in the chunk
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::Read(RArchive& rArchive)
{	
	// Must always call Uninitialize() to clean up any previous DIB section
	Uninitialize();

	// Read in the buffer object
	rArchive >> *m_pDibBuffer;

	// Initialize the DibSection
//	InitializeDibSection();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::Write()
//
//  Description:		Writes a bitmap out to the given chunk
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::Write(RArchive& rArchive) const
{	
	rArchive << *m_pDibBuffer;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::Copy()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::Copy(RDataTransferTarget& rDataTarget, YDataFormat yDataFormat) const
{	
	RBitmapImage* pThis = const_cast<RBitmapImage*>(this);

	void* pRawData = pThis->m_pDibBuffer->GetBuffer();
	uLONG uRawDataSize = pThis->m_pDibBuffer->GetBufferSize();

	if(yDataFormat == kDibFormat)
	{
		// Allocate a chunk of memory large enough for the bitmap
		HGLOBAL hGlobal = ::GlobalAlloc(GHND, (YDataLength)uRawDataSize);
		if(!hGlobal) throw ::kMemory;

		// Copy the raw data
		void* pDestData = ::GlobalLock(hGlobal);
		memcpy(pDestData, pRawData, uRawDataSize);
		::GlobalUnlock(hGlobal);

		// Set the data
		rDataTarget.SetData(kDibFormat, hGlobal);
	}
	else if(yDataFormat == kBitmapFormat)
	{
		RDefaultDrawingSurface rDefaultDS;

		// Create a bitmap compatible with the screen
		HBITMAP hBitmap = ::CreateCompatibleBitmap((HDC)rDefaultDS.GetSurface(), pThis->GetWidthInPixels(),
			pThis->GetHeightInPixels());

		// Set the bitmap bits
		::SetDIBits(NULL, hBitmap, 0, pThis->GetHeightInPixels(), GetImageData(pRawData),
			(BITMAPINFO*)pRawData, DIB_RGB_COLORS);

		// Set the data
		rDataTarget.SetData(kBitmapFormat, hBitmap);
	}
	else
	{
		TpsAssertAlways("Unexpected format in Copy.");
	}
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::Paste()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		kMemory; kDiskFull; kUnknownError
//
// ****************************************************************************
void RBitmapImage::Paste(const RDataTransferSource& rDataSource, YDataFormat yDataFormat)
{
	// Must always call Uninitialize() to clean up any previous DIB section
	Uninitialize();

	if (yDataFormat == kDibFormat)
	{
		YDataLength yDataLength;
		uBYTE* pSrcRawData;
		rDataSource.GetData(kDibFormat, pSrcRawData, yDataLength);		
		m_pDibBuffer->Resize(yDataLength);			
		memcpy(m_pDibBuffer->GetBuffer(), pSrcRawData, yDataLength);
	}
	else if (yDataFormat == kBitmapFormat)
	{		
		// Convert the HBITMAP to a DIB
		// Get the bitmap handle
		HBITMAP hBitmap = static_cast<HBITMAP>(rDataSource.GetData(kBitmapFormat));

		// Get a DIB-format header from the DDB
		BITMAPINFO tBitmapInfo;
		tBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		tBitmapInfo.bmiHeader.biBitCount = 0;
		if (!::GetDIBits(NULL, hBitmap, 0, 0, NULL, &tBitmapInfo, DIB_RGB_COLORS))
			::ThrowExceptionOnOSError();
		
		// Compute the size of the color table and image data	
		m_pDibBuffer->Resize(GetDibSize(&tBitmapInfo));
		memcpy(m_pDibBuffer->GetBuffer(), &tBitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER));

		// Get the bits for real
		BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();
		if (!::GetDIBits(NULL, hBitmap, 0, tBitmapInfo.bmiHeader.biHeight, 
			GetImageData(pBitmapInfo), pBitmapInfo, DIB_RGB_COLORS)) 
			::ThrowExceptionOnOSError();
	}
	else
	{
		TpsAssertAlways("Unexpected format in Paste.");
		return;
	}	

	// Initialize the DibSection
//	InitializeDibSection();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::UpdatePalette()
//
//  Description:		Updates the DibSection after direct pixel manipulation
//							(necessary if the image is palettized).
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::UpdatePalette()
{
	// Update the DibSection if we need to
	if (GetColorDataSize(GetRawData()) > 0)
	{		
//		TpsAssert(m_hBitmap, "No DIB section to update.");
//		if (m_hBitmap) 
//		{
//			::DeleteObject(m_hBitmap);
//			m_hBitmap = NULL;
//		}
//
//		InitializeDibSection();
		;
	}
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::operator=()
//
//  Description:		operator=
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::operator=(const RImage& rhs)
{
	// Make a copy of this bitmap
	const RBitmapImage* pBitmap = dynamic_cast<const RBitmapImage*>(&(const_cast<RImage&>(rhs)));
	TpsAssert(pBitmap, "Bad operand in RBitmapImage::operator=().");	

	// Check for assignment to self
	if (this == pBitmap) return;

	// Call the base member
	RImage::operator=(rhs);

	// Call the RBitmapImage member
	operator=(*pBitmap);	
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::operator=()
//
//  Description:		operator=
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::operator=(const RBitmapImage& rhs)
{
	// Check for assignment to self
	if (this == &rhs) return;

	// Must always call Uninitialize() to clean up any previous DIB section
	Uninitialize();

	// Copy the Dib buffer
	m_pDibBuffer->Resize(rhs.m_pDibBuffer->GetBufferSize());	
	memcpy(m_pDibBuffer->GetBuffer(), rhs.m_pDibBuffer->GetBuffer(), rhs.m_pDibBuffer->GetBufferSize());
//	InitializeDibSection();	
}


	// ****************************************************************************
	//
	//  Function Name:	GetMaskAndShiftValues
	//
	//  Description:		The Mask and shift values for the given color mask
	//
	//  Returns:			Nothing
	//
	//  Exceptions:		Nothign
	//
	// ****************************************************************************
	static void GetMaskAndShiftValues( uLONG ulComponent, uLONG& mask, uLONG& shift )
	{
		shift	= 0;

		//	If ulComponent is 0x0, there is no porition of this color in the bitmap
		if ( ulComponent != 0 )
		{
			//	Shift (and count) until hi-bit is set and then mask off the byte
			while ( !(ulComponent & 0x80000000) )
			{
				ulComponent <<= 1;
				++shift;
			}
			mask = ulComponent;
		}
		else
			mask = 0;
	}
// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetIndexOfColor()
//
//  Description:		Returns the index (or RGB) of the color
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetIndexOfColor( const RSolidColor& transparentColor ) const
{
	uLONG			index			= 0;
	uLONG			ulBitDepth	= GetBitDepth( );
	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();

	if ( ulBitDepth <= 8 )
	{
		TpsAssert( (ulBitDepth == 1) || (ulBitDepth == 4) || (ulBitDepth == 8), "Untested Bit Depth" );
		uLONG uColorDataSize		= GetColorDataSize(ulBitDepth);
		uWORD uNumberOfEntries	= (uWORD)(uColorDataSize / sizeof(RGBQUAD));

		if (uNumberOfEntries > 0)
		{
			BYTE ucRed		= GetRValue( transparentColor );
			BYTE ucGreen	= GetGValue( transparentColor );
			BYTE ucBlue		= GetBValue( transparentColor );

			for (uWORD uEntry = 0; uEntry < uNumberOfEntries; ++uEntry)
			{
				if (	(pBitmapInfo->bmiColors[uEntry].rgbRed == ucRed) &&
						(pBitmapInfo->bmiColors[uEntry].rgbGreen == ucGreen) &&
						(pBitmapInfo->bmiColors[uEntry].rgbBlue == ucBlue) )
					return uEntry;
			}		
		}
		
		TpsAssertAlways( "Transparent Color Not Found" );
	}
	else //	ulBitDepth > 8
	{
		// If biCompression is BI_BITFIELDS, there are 3 color table entries
		// representing three bitmasks that specify the red, green, and blue
		// components of each pixel
		if (pBitmapInfo->bmiHeader.biCompression != BI_BITFIELDS)
			index	= static_cast<uLONG>( static_cast<COLORREF>( transparentColor ) );
		else
		{
#define	RED_COLOR_MASK		0
#define	GREEN_COLOR_MASK	1
#define	BLUE_COLOR_MASK	2

			uLONG	redColorMask, greenColorMask, blueColorMask;
			uLONG	redColorShift, greenColorShift, blueColorShift;

			GetMaskAndShiftValues( *(uLONG*)(&pBitmapInfo->bmiColors[RED_COLOR_MASK]), redColorMask, redColorShift );
			GetMaskAndShiftValues( *(uLONG*)(&pBitmapInfo->bmiColors[GREEN_COLOR_MASK]), greenColorMask, greenColorShift );
			GetMaskAndShiftValues( *(uLONG*)(&pBitmapInfo->bmiColors[BLUE_COLOR_MASK]), blueColorMask, blueColorShift );

			uLONG		redValue		= (transparentColor.GetRed( ) << 24) & redColorMask;
			uLONG		greenValue	= (transparentColor.GetGreen( ) << 24) & greenColorMask;
			uLONG		blueValue	= (transparentColor.GetBlue( ) << 24) & blueColorMask;
			redValue		= redValue >> redColorShift;
			greenValue	= greenValue >> greenColorShift;
			blueValue	= blueValue >> blueColorShift;
			index	= (redValue | greenValue | blueValue);
		}
	}

	return index;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetWidthInPixels()
//
//  Description:		Returns the width of the bitmap, in pixels
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetWidthInPixels( ) const
{
	if (!m_pDibBuffer) return 0;
	
	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();
	return (uLONG)pBitmapInfo->bmiHeader.biWidth;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetHeightInPixels()
//
//  Description:		Returns the height of the bitmap, in pixels
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetHeightInPixels( ) const
{
	if (!m_pDibBuffer) return 0;

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();
	return (uLONG)pBitmapInfo->bmiHeader.biHeight;	
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetBitDepth()
//
//  Description:		Returns m_uBitDepth
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetBitDepth( ) const
{
	if (!m_pDibBuffer) return 0;

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();
	return (uLONG)pBitmapInfo->bmiHeader.biBitCount;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetXDpi()
//
//  Description:		Returns m_uXDpi
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetXDpi( ) const
{
	if (!m_pDibBuffer) return 0;

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();
	return ::PelsPerMeterToDpi(pBitmapInfo->bmiHeader.biXPelsPerMeter);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetYDpi()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetYDpi( ) const
{
	if (!m_pDibBuffer) return 0;

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();
	return ::PelsPerMeterToDpi(pBitmapInfo->bmiHeader.biYPelsPerMeter);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetSystemHandle()
//
//  Description:		Returns the system handle of the vector image, an HBITMAP
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YImageHandle RBitmapImage::GetSystemHandle() const
{
	TpsAssert( m_hBitmap, "Cannot get the system handle until the image has been Locked with LockImage" );
	return (YImageHandle)m_hBitmap;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetSizeInLogicalUnits( )
//
//  Description:		Returns the size in RLUs
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RBitmapImage::GetSizeInLogicalUnits( ) const
{
	return RRealSize(::InchesToLogicalUnits(static_cast<YRealDimension>(GetWidthInPixels()) / static_cast<YRealDimension>(kDefaultDIBXDpi)),
		::InchesToLogicalUnits(static_cast<YRealDimension>(GetHeightInPixels()) / static_cast<YRealDimension>(kDefaultDIBYDpi)));
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetRawData()
//
//  Description:		Returns a pointer to the raw data buffer
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void* RBitmapImage::GetRawData() const
{	
	return m_pDibBuffer->GetBuffer();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetDibBufferSize()
//
//  Description:		Returns the size of the raw data buffer
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG	RBitmapImage::GetRawDataSize() const
{	
	return m_pDibBuffer->GetBufferSize();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::InitializeDibSection()
//
//  Description:		Core member initializer function; all other Initialize()
//							functions should call this with a pointer to an 
//							appropriately defined RTemporaryStorage
//
//  Returns:			
//
//  Exceptions:		kMemory
//
// ****************************************************************************
void RBitmapImage::InitializeDibSection()
{	
	TpsAssert(!m_hBitmap, "RBitmapImage object already has a DIB section.  You may have temp file leaks in Windows 95.");

	// Create the DibSection from the raw data in m_pRawData
	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();				
	void* pBitmapData;
	
	RDefaultDrawingSurface rDefaultDS;
	m_hBitmap = ::CreateDIBSection((HDC)rDefaultDS.GetSurface(),
		pBitmapInfo,
		DIB_RGB_COLORS,
		&pBitmapData, 
		(HANDLE)m_pDibBuffer->GetBufferHandle(),
		sizeof(BITMAPINFOHEADER) + GetColorDataSize(pBitmapInfo));
	if (!m_hBitmap) throw kMemory;
}

YIntCoordinate RBitmapImage::SkipZeroBits( uBYTE* pBytes, YIntCoordinate xLeft, YIntCoordinate xRight ) const
{
	//
	// Look at the bits in the first byte...
	pBytes			+= (xLeft>>3);
	uBYTE	ubMask	= uBYTE(1<<(7-(xLeft&0x07)));
	uBYTE	ubData	= *pBytes;
	while( ubMask && xLeft<xRight )
		{
		if( ubData & ubMask )
			return xLeft;
		++xLeft;
		ubMask >>= 1;
		}
	//
	// Look at whole bytes...
	while( xLeft<xRight && !*++pBytes )
		{
		xLeft += 8;
		}
	//
	// Look at the bits in the last byte
	ubMask	= 0x80;
	ubData	= *pBytes;
	while( ubMask && xLeft<xRight )
		{
		if( ubData & ubMask )
			return xLeft;
		++xLeft;
		ubMask >>= 1;
		}
	return xRight;
}

YIntCoordinate RBitmapImage::SkipOneBits( uBYTE* pBytes, YIntCoordinate xLeft, YIntCoordinate xRight ) const
{
	//
	// Look at the bits in the first byte...
	pBytes			+= (xLeft>>3);
	uBYTE	ubMask	= uBYTE(1<<(7-(xLeft&0x07)));
	uBYTE	ubData	= *pBytes;
	while( ubMask && xLeft<xRight )
		{
		if( !(ubData & ubMask) )
			return xLeft;
		++xLeft;
		ubMask >>= 1;
		}
	//
	// Look at whole bytes...
	while( xLeft<xRight && (*++pBytes)==0x0FF )
		{
		xLeft += 8;
		}
	//
	// Look at the bits in the last byte
	ubMask	= 0x80;
	ubData	= *pBytes;
	while( ubMask && xLeft<xRight )
		{
		if( !(ubData & ubMask) )
			return xLeft;
		++xLeft;
		ubMask >>= 1;
		}
	return xRight;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetImageData()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetDibSize(void* pRawData)
{
	TpsAssert(pRawData, "pRawData = NULL.");
	
	return sizeof(BITMAPINFOHEADER) + GetColorDataSize(pRawData) + GetImageDataSize(pRawData);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetImageData()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void* RBitmapImage::GetImageData(void* pRawData)
{
	TpsAssert(pRawData, "pRawData = NULL.");
	return (uBYTE*)pRawData + sizeof(BITMAPINFOHEADER) + GetColorDataSize(pRawData);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetImageDataSize()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetImageDataSize(void* pRawData)
{
	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)pRawData;
	return GetImageDataSize((uLONG)pBitmapInfoHeader->biWidth,
		(uLONG)pBitmapInfoHeader->biHeight, 
		(uLONG)pBitmapInfoHeader->biBitCount);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetImageDataSize()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetImageDataSize(uLONG uWidth, uLONG uHeight, uLONG uBitDepth)
{
	return uHeight * GetBytesPerRow(uWidth, uBitDepth);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetColorData()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void* RBitmapImage::GetColorData(void* pRawData)
{
	TpsAssert(pRawData, "pRawData = NULL.");
	if (GetColorDataSize(pRawData) == 0) 
		return NULL;
	else 
		return (uBYTE*)pRawData + sizeof(BITMAPINFOHEADER);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetColorDataSize()
//
//  Description:		Returns the size of the bitmap's color table in bytes
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetColorDataSize(void* pRawData)
{	
	TpsAssert(pRawData, "pRawData = NULL.");
	BITMAPINFOHEADER* pBitmapInfoHeader = (BITMAPINFOHEADER*)pRawData;
	
	uLONG uColorTableEntries = 0;
	switch (pBitmapInfoHeader->biBitCount)
	{
	case 1:
	case 4:
	case 8:
		// If the number of colors used is given, use that number;
		// otherwise, use the maximum number of colors for the given 
		// bit depth
		if (pBitmapInfoHeader->biClrUsed > 0) 
			uColorTableEntries = (uLONG)pBitmapInfoHeader->biClrUsed;
		else
			uColorTableEntries = GetColorDataSize(pBitmapInfoHeader->biBitCount)
			/ sizeof(RGBQUAD);
		break;

	case 24:
		break;

	case 16:
	case 32:
		// If biCompression is BI_BITFIELDS, there are 3 color table entries
		// representing three bitmasks that specify the red, green, and blue
		// components of each pixel
		if (pBitmapInfoHeader->biCompression == BI_BITFIELDS)
			uColorTableEntries = 3;
		break;

	default:
		TpsAssertAlways("Invalid bit depth in BITMAPINFOHEADER.");
	}

	return uColorTableEntries * sizeof(RGBQUAD);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetColorDataSize()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetColorDataSize(uLONG uBitDepth)
{
	if (uBitDepth > 8)
		return 0;
	else
		return (0x01 << uBitDepth) * sizeof(RGBQUAD);
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetBytesPerRow()
//
//  Description:		Returns the number of bytes required to store one row
//							of image data
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetBytesPerRow(uLONG uWidth, uLONG uBitDepth)
{	
	if (uBitDepth == 1)
		return ((uWidth + 31) & ~31) / 8;
	else if (uBitDepth == 4) 
		return ((uWidth + 7) & ~7) / 2;
	else
		return (uWidth * uBitDepth / 8 + 3) & ~3;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::Get2DImageData()
//
//  Description:		Returns a 2 dimensional array representing the bitmap's
//							image bits
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uBYTE** RBitmapImage::Get2dImageData(void* pRawData)
{
	TpsAssert(pRawData, "pRawData = NULL.");	

	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)pRawData;
	uBYTE** ppImageData = new uBYTE*[pBitmapInfo->bmiHeader.biHeight];
	sLONG sBytesPerRow = (sLONG)GetBytesPerRow((uLONG)pBitmapInfo->bmiHeader.biWidth, (uLONG)pBitmapInfo->bmiHeader.biBitCount);	
	sLONG sByteIndex = 0;
	uBYTE* pImageData = (uBYTE*)GetImageData(pBitmapInfo);
	for (sLONG sRow = 0; sRow < pBitmapInfo->bmiHeader.biHeight; sRow++)
	{
		ppImageData[sRow] = &pImageData[sByteIndex];
		sByteIndex += sBytesPerRow;
	}

	return ppImageData;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetRgbQuad()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RGBQUAD RBitmapImage::GetRgbQuad( BYTE sRed, BYTE sGreen, BYTE sBlue )
{
	RGBQUAD tRgbQuad;
	tRgbQuad.rgbBlue =		sBlue;
	tRgbQuad.rgbGreen =		sGreen;
	tRgbQuad.rgbRed =			sRed;
	tRgbQuad.rgbReserved =	0x00;
	
	return tRgbQuad;
}

//****************************************************************************
//
//  Function Name:	RBitmapImage::FindPathInData()
//
//  Description:	Finds the clipping path embedded in a (JPG) file, sets a pointer
//						to and lenght for the data (m_pClipBuffer and m_pClipBufferLen).
//						See the Adobe Clipping Path Resource description in the Adobe
//						SDK for technical info on the Clipping Path Resource.
//
//  Returns:		TRUE if found
//
//  Exceptions:		
//
//	 Notes:			Maximum number of clip records is kMaxClipRecords.
//						
//
//****************************************************************************
BOOL	RBitmapImage::FindPathInData(void* pBuffer, int nBufferSize, const int nType)
{
	const char*	k_szSignature = "8BIM";

	char*		pClipResource = NULL;
	int		nClipRecords = 0;
	int		nLenToSearch = nBufferSize;
	int		nSearched = 0;

	while(nSearched < nLenToSearch)
	{
		char *pSigChar = (char*)memchr(pBuffer, *k_szSignature, nBufferSize);

		int n = (int)pSigChar - (int)pBuffer;	// bytes searched
		pBuffer = (char*)pBuffer + ++n;			// point to byte after current search
		nBufferSize -= n;								// keep track of whats left in the buffer
		nSearched += n;								// keep track of searched lenght

		if(pSigChar) 	// First char in the Photoshop resource signature
		{
			if(memcmp(pSigChar, k_szSignature, (sizeof(k_szSignature)-1)) == 0) 	// Photoshop resource signature
			{
				char *pResourceID = pSigChar;
				pResourceID += 4;
				WORD wTemp = *((WORD *) pResourceID); // opps need to swap bytes
				WORD nID = (LOBYTE(wTemp) << 8) + HIBYTE(wTemp);
				if(nID < 2999 && nID > 1999) // Photoshop clipping path resource
				{
					char *pClippingPathName= pResourceID+3;
					if((nType == kOutline)||(strcmp(pClippingPathName,"FILL") == 0 && nType == kFrame))
					{
						// Lets find out how many records are in this path
						pResourceID++;
						pResourceID++;
						int nClippingPathNameLen = *pResourceID;
						char *pNumberOfRecords = (pClippingPathName) + (nClippingPathNameLen +1);

						DWORD* p = (DWORD*)pNumberOfRecords;
						DWORD  dwRecords = *p;

						// Align the data if necessary: 0x00AABB00 becomes 0xAABB and 0xCCDD0000 becomes 0xCCDD.  
						if(HIBYTE(HIWORD(dwRecords)) == 0)
							dwRecords = dwRecords << 8; 
						WORD wRecords = (HIWORD(dwRecords));

						// Byte swap: 0xAABB becomes 0xBBAA
						wRecords = (LOBYTE(wRecords) << 8) + HIBYTE(wRecords);
						nClipRecords = (wRecords -1)/kBytesPerRecord;  
						if(nClipRecords > kMaxClipRecords)
							break; // ... were outta here

						// Now lets set a pointer to the 1st record (the fill rule record).
						char *pFirstRecord = (pClippingPathName) + (nClippingPathNameLen +5);
						char* pSelectorRecord = (char*)memchr(pFirstRecord, kByteFillRuleRecord, 5 * kBytesPerRecord);

						pSelectorRecord++;
						int i;
						for(i = 0; i < kBytesFollowingSelector; i++, pSelectorRecord++)
						{
							if(*pSelectorRecord != 0) break;
						}
						if(i == kBytesFollowingSelector)
						{
							// Sanity Check...
							pSelectorRecord -= kBytesPerRecord;
							WORD*	wpExpectedValue = (WORD*)pSelectorRecord;
							if(*wpExpectedValue == kFillRuleRecord)
							{
								pClipResource = pSelectorRecord; // this is the start of the clipping data
								break;
							}
							else break; // the expected value was bogus
						}
						else break; // didn't find the fill rule record 
					}
				}
			}
			else if(nSearched > 18000*2)	// 18000 is the max observed with an "ornate frame", we double it.
				break;				// were outta here!
		}
		else break; // back to the while loop
	}
	if(!pClipResource) // no usable path
		return FALSE;
	m_nClipBufferLen = (nClipRecords * kBytesPerRecord) + kBytesPerRecord;
	m_pClipBuffer = pClipResource;

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::MakeTransparentColorUnique
//
//  Description:	Modifies the image palette to ensure that the transparent
//					index color is different from all other palette colors for
//					the image.
//
//  Returns:		The new transparent color.
//
//  Exceptions:		None.
//
// ****************************************************************************
RSolidColor	RBitmapImage::MakeTransparentColorUnique( int nTransparentIndex, int nUsedPaletteSize )
{
	RSolidColor	transparentColor;	// The transparent color will be the RColor
									// concept of transparent unless initialized below.
	uLONG		ulBitDepth	= GetBitDepth();
	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();

	// Only process images which contain palette information.  A palette index
	// is meaningless for any other image type.
	if ( ulBitDepth <= 8 )
	{
		uLONG uColorDataSize = GetColorDataSize( ulBitDepth );
		uWORD uNumberOfEntries = (uWORD)(uColorDataSize / sizeof(RGBQUAD));

		TpsAssert( uNumberOfEntries >= nUsedPaletteSize, "GIF Palette larger than image palette" );

		if (uNumberOfEntries > 0)
		{
			// Get the color palette for the image, and the current color for
			// tranparency.
			uLONG *pColorData = (uLONG*)GetColorData(pBitmapInfo);
			COLORREF crTransparent = pColorData[nTransparentIndex];

			// Check to see if the transparent color is already unique.
			BOOLEAN bFound = FALSE;
			for (uWORD uEntry = 0; (uEntry < nUsedPaletteSize) && !bFound; uEntry++)
			{
				// Process the entry as long as it isn't the transparent index slot.
				if (uEntry != (uWORD)nTransparentIndex)
				{
					if ((pColorData[uEntry] & 0x00FFFFFF) == crTransparent)
					{
						bFound = TRUE;
					}
				}
			}

			// If the color was found then we need to generate a new color.  So
			// loop until we have a color that isn't found in the index.
			uWORD uIntensity = 0;
			while (bFound)
			{
				// reset ourselves to a not-found state.
				bFound = FALSE;
				// Ok, I'm going to walk the transparent color through the grays
				// since that is likely to look best if the image is taken
				// out of the program and read by a system that doesn't support
				// transparency.  Since there are at most 256 entries in the
				// palette, and we know we had a duplicate, this is guaranteed
				// to generate a unique value somwhere between 0 and 255.
				crTransparent = RGB(uIntensity,uIntensity,uIntensity);
				for (uWORD uEntry = 0; (uEntry < nUsedPaletteSize) && !bFound; uEntry++)
				{
					// Process the entry as long as it isn't the transparent index slot.
					if (uEntry != (uWORD)nTransparentIndex)
					{
						if ((pColorData[uEntry] & 0x00FFFFFF) == crTransparent)
						{
							bFound = TRUE;
						}
					}
				}
				// If we found that intensity level, bump it up by one.
				if (bFound)
				{
					uIntensity++;
				}
			}

			// We now have a unique color, so set up our return value and set
			// the palette slot to our new color.
			pColorData[nTransparentIndex] = crTransparent;
			int Blue = crTransparent & 0xFF;
			int Green = (crTransparent & 0xFF00) >> 8;
			int Red = (crTransparent & 0xFF0000) >> 16;
			transparentColor = RSolidColor( Red, Green, Blue );
		}
	}

	return transparentColor;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::CreateRPath()
//
//  Description:		Create an RPath from the clipping path data
//
//  Returns:			RPath on success, NULL on failure
//
//  Exceptions:
//
// ****************************************************************************
RPath*	RBitmapImage::CreateRPath()
{



	int x = GetWidthInPixels();
	int y = GetHeightInPixels();
	WORD wNumPoints;
	int	nClipRecords = m_nClipBufferLen / kBytesPerRecord; // number of records in this path
	nClipRecords--;	// adjust for zero base
	int nRecord = 0;
	//int m_nSubPaths = 0;	// number of objects within a named clipping resource

	char* pNextRecord; 
	m_ptArray->RemoveAll();
	m_nSubPaths = 0;
	// Extract the points from the raw data
	
	while(nClipRecords > nRecord)
	{
		pNextRecord =+ (m_pClipBuffer + (++nRecord * kBytesPerRecord));

		// Point the the number of points entry

		pNextRecord++, pNextRecord++;

		// Get the number of points from the data

		WORD* wp = (WORD*)pNextRecord;
		WORD wTemp = *wp;
		WORD wPoints = (LOBYTE(wTemp) << 8) + HIBYTE(wTemp);
		wNumPoints = wPoints * 3;
		while(wPoints--)
		{
			pNextRecord =+ (m_pClipBuffer + (++nRecord * kBytesPerRecord));
			//
			// First two bytes of each record is the selector (See Adobe docs)
			//WORD* wp = (WORD*)pNextRecord;
			//WORD wTemp = *wp;
			//WORD wSelector = (LOBYTE(wTemp) << 8) + HIBYTE(wTemp);
			// 
			char * pPoint = (pNextRecord + 2);	// the knot follows the selector
			DWORD* pKnot = (DWORD*)pPoint;		// point to first knot set
			CPoint ptTemp;
			long dwKnot;	

			// Each record has three path points: two control points and an anchor.
			// Scale these path points from into our point array.
			for (int i = 0; i < 3; i++)
			{
				dwKnot = *pKnot;
				WORD w2 = (LOWORD(dwKnot));
				WORD w1 = (HIWORD(dwKnot));
				w1 = (LOBYTE(w1) << 8) + HIBYTE(w1);
				w2 = (LOBYTE(w2) << 8) + HIBYTE(w2);
				dwKnot = (w2 << 16) + (w1);
				dwKnot = kScalingFactor - dwKnot; // flip only the y axis.
				ptTemp.y = dwKnot/(kScalingFactor/y);
				pKnot++;

				dwKnot = *pKnot;
				w2 = (LOWORD(dwKnot));
				w1 = (HIWORD(dwKnot));
				w1 = (LOBYTE(w1) << 8) + HIBYTE(w1);
				w2 = (LOBYTE(w2) << 8) + HIBYTE(w2);
				dwKnot = (w2 << 16) + (w1);
				ptTemp.x = dwKnot/(kScalingFactor/x);
				m_ptArray[m_nSubPaths].Add(ptTemp);
				pKnot++;
			}
		}
		// Save the first triplette in this path at the end of the array

		CPoint pt;
		for(int i = 0; i < 3; i++)
		{
			pt = m_ptArray[m_nSubPaths][i];
			m_ptArray[m_nSubPaths].Add(pt);
		}
		if(++m_nSubPaths >= kMaxPaths) // Sanity check on the number of paths we are dealing with...
		{
			TRACE("Error: RBitmapImage::CreateClippingRPath() exceeded the max number of paths\n\r");
			return NULL;
		}
	}
	return RPathFromPoints();
}
#ifdef IM_GONE
// ****************************************************************************
//
//  Function Name:	RBitmapImage::SetClipRPathFromCArray()
//
//  Description:		Create an RPath from a CArray of points:

//  Returns:			TRUE on success
//
//  Exceptions:		
//
// ****************************************************************************
BOOL	RBitmapImage::SetClipRPathFromCArray(CArray<CPoint,CPoint>pArray)
{
	BOOL	bResult = TRUE;

	delete m_pClipPath;		
	m_ptArray->RemoveAll();
	m_nSubPaths = 1;
	m_ptArray->Copy(pArray);
	m_pClipPath = (RClippingPath *) RPathFromPoints();
	return bResult;
}
#endif
// ****************************************************************************
//
//  Function Name:	RBitmapImage::RPathFromPoints()
//
//  Description:		Create an RPath from an array of points:
//							Extracts m_nSubpaths into a single named RPath.
//							m_ptArray[m_nSubPaths][]
//
//  Returns:			RPath on success, NULL on failure
//
//  Exceptions:		kUnknownError (on "new RPath()")
//
// ****************************************************************************
RPath*	RBitmapImage::RPathFromPoints()
{
	RClippingPath*	path = NULL;
	CPoint pt(0, 0);					// intermediate point for xlate to RIntPoint
	RIntPoint point(0, 0);			// store the point for the RPath

	EPathOperatorArray	opArray;	// the RPath operations
	RIntPointArray			ptArray;	// the RPath points
	if(!m_nSubPaths)
	{
		return NULL;						// need at least one sub-path
	}
	m_nSubPaths--;						// number of path objects in this named path

	while(m_nSubPaths  != -1)
	{
		int numTriplets  = (m_ptArray[m_nSubPaths].GetSize()/3) -1;
		if(!numTriplets)
		{
			return NULL;				// need at least two points
		}
		int i = 0;

		// Step 1 MOVE_TO

		opArray.InsertAtEnd(kMoveTo);
		pt = m_ptArray[m_nSubPaths][1];	// the "move to"  point 
		point.m_x = pt.x;
		point.m_y = pt.y;
		ptArray.InsertAtEnd(point);

		// Step 2 FIRST BEZIER TO 

		opArray.InsertAtEnd(kBezier);
		opArray.InsertAtEnd((EPathOperator)1);

		pt = m_ptArray[m_nSubPaths][i];point.m_x = pt.x;point.m_y = pt.y;
		ptArray.InsertAtEnd(point);
		pt = m_ptArray[m_nSubPaths][++i];point.m_x = pt.x;point.m_y = pt.y;
		ptArray.InsertAtEnd(point);
		pt = m_ptArray[m_nSubPaths][i];point.m_x = pt.x;point.m_y = pt.y;
		ptArray.InsertAtEnd(point);

		// Step 3 NEXT BEZIER'S TO

		while(numTriplets)
		{
			--numTriplets;
			opArray.InsertAtEnd(kBezier);
			opArray.InsertAtEnd((EPathOperator)1);

			pt = m_ptArray[m_nSubPaths][++i];point.m_x = pt.x;point.m_y = pt.y;
			ptArray.InsertAtEnd(point);
			pt = m_ptArray[m_nSubPaths][++i];point.m_x = pt.x;point.m_y = pt.y;
			ptArray.InsertAtEnd(point);
			pt = m_ptArray[m_nSubPaths][++i];point.m_x = pt.x;point.m_y = pt.y;
			ptArray.InsertAtEnd(point);
		}
		// Step 4 CLOSE_PATH (AND INDEX FOR NEXT)

		opArray.InsertAtEnd(kClose);
		m_nSubPaths--;
	}
	opArray.InsertAtEnd(kEnd);

	// Create a path object

	try
	{
		if(opArray.Count() != 0 && ptArray.Count() != 0)
		{
			if(ptArray.Count() > 512)
			{
				path = new RClippingPath( 512, TRUE );
			}				
			else
			{
				path = new RClippingPath( TRUE );
			}				

			path->Undefine();
			if(path->Define(opArray, ptArray))
			{
				RIntSize rBitmapSize(GetWidthInPixels(),GetHeightInPixels());
				path->SetPathSize(rBitmapSize); // scale the path into the bitmap...
			}				
			else
			{
				delete path;
				path = NULL;
			}
		}
	}
	catch( ... )
	{
		delete path;
		throw;
	}
	// Cleanup
	
	opArray.Empty();
	ptArray.Empty();
	return path; 
}

// ****************************************************************************
//
//  Function Name:	RImage::RenderTransparent()
//
//  Description:		Render the bitmap out with arbitrary rotation with the
//							transparent color taken into account so that it renders
//							transparently.
//
//  Returns:			None.
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::RenderTransparent( RDrawingSurface &rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rDestRect )
{
	RBitmapImage *prBackfilledBitmap = NULL;
	RBitmapImage *prCopyBitmap = NULL;

	TpsAssert(rMaskBitmap.GetWidthInPixels() == GetWidthInPixels(), "Mask width does not match bitmap width.");
	TpsAssert(rMaskBitmap.GetHeightInPixels() == GetHeightInPixels(), "Mask height does not match bitmap height.");
	TpsAssert(rDS.GetSurface(), "rDS.GetSurface() = NULL.");
	TpsAssertIsObject( RBitmapImage, &rMaskBitmap );

	try
	{
		RIntRect rSrcRect(0, 0, GetWidthInPixels(), GetHeightInPixels());

		// Create a bitmap from the mask and the proxy which has the masked areas
		// turned into the transparent color.
		prBackfilledBitmap = new RBitmapImage;
		prBackfilledBitmap->Initialize( GetWidthInPixels(), GetHeightInPixels(), GetBitDepth() );
		prCopyBitmap = new RBitmapImage;
		prCopyBitmap->Initialize( GetWidthInPixels(), GetHeightInPixels(), GetBitDepth() );

		// Lock the images into drawing surfaces.
		ROffscreenDrawingSurface rImageDS;
		rImageDS.SetImage( this );
		ROffscreenDrawingSurface rCopyDS;
		rCopyDS.SetImage( prCopyBitmap );
		ROffscreenDrawingSurface rMaskDS;
		rMaskDS.SetImage( &rMaskBitmap );
		ROffscreenDrawingSurface rBackfilledDS;
		rBackfilledDS.SetImage( prBackfilledBitmap );

		// 1) Make a temporary copy of the image.  Unfortunately, the original
		//		image seems to be palette sensative to the bitwise Ands and Ors so
		//		we need to work with all fresh images.
		rCopyDS.BlitDrawingSurface(rImageDS, rSrcRect, rSrcRect, kBlitSourceCopy );

		// 2) Fill the image we are creating with the transparent color.
		rBackfilledDS.SetFillColor( m_rcTransparentColor );
		rBackfilledDS.FillRectangle( rSrcRect );

		// 3) Cut a hole in the transparent color to receive the image.
		rBackfilledDS.BlitDrawingSurface(rMaskDS, rSrcRect, rSrcRect, kBlitNotSourceAnd);

		// 4) Cut out the edges around the image.  This masks around the actual image.
		rCopyDS.BlitDrawingSurface(rMaskDS, rSrcRect, rSrcRect, kBlitSourceAnd);

		// 5) Plunk the image in the hole without disturbing the edges.
		rBackfilledDS.BlitDrawingSurface(rCopyDS, rSrcRect, rSrcRect, kBlitSourcePaint);

		// 6) Put the newly constructed image with full transparent coloring into the proxy.
		rImageDS.BlitDrawingSurface( rBackfilledDS, rSrcRect, rSrcRect, kBlitSourceCopy );

		// Free up the images.  This must be done before the render so that the
		// proxy image (this) can be selected into a DC for painting.
		rImageDS.ReleaseImage();
		rBackfilledDS.ReleaseImage();
		rCopyDS.ReleaseImage();
		rMaskDS.ReleaseImage();

		// Blit the proxy now which has the transparent color correctly in place.
		Render( rDS, rDestRect );
	}
	catch (...)
	{
		// something went wrong - fail to render.
		TpsAssertAlways( "Couldn't render image transparently." );
	}

	// Clean up.
	delete prBackfilledBitmap;
	delete prCopyBitmap;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::SetMaskBitmap()
//
//  Description:		Sets the mask bitmap
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void	RBitmapImage::SetAlphaChannel( RBitmapImage* pAlphaChannel )
{
	if (m_pAlphaChannel)
		delete m_pAlphaChannel;

	m_pAlphaChannel = pAlphaChannel;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::SetRPath()
//
//  Description:		Sets the clipping path using an existing path from another RImage.
//							Note: bImport defaults to TRUE
//
//  Returns:			TRUE if successful
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL	RBitmapImage::SetRPath(RClippingPath* path)
{
	BOOL	bResult = FALSE;
	
	if (m_pClipPath && m_pClipPath->PathFromImport() != path->PathFromImport())
		return FALSE;

	if (path->IsDefined())
	{	
		delete m_pClipPath;
		m_pClipPath = new RClippingPath( *path );

		if(m_pClipPath)
			bResult = TRUE;
	}

	return bResult;
}
// ****************************************************************************
//
//  Function Name:	RBitmapImage::DeleteRPath()
//
//  Description:		Deletes the non-imported rpath
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL	RBitmapImage::DeleteRPath()
{
	if(m_pClipPath && !m_pClipPath->PathFromImport())
	{
		delete m_pClipPath;
		m_pClipPath = NULL;

		return TRUE;
	}

	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::SetFrameRPath()
//
//  Description:		Sets the clipping path for the frame path from another RImage.
//
//  Returns:			TRUE if successful
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL	RBitmapImage::SetFrameRPath(RPath* path)
{
	BOOL	bResult = FALSE;
	
	if(!m_pFramePath && path->IsDefined())
	{	
		m_pFramePath = new RPath(*path);
		if(m_pFramePath)
		{
			bResult = TRUE;
		}
	}
	return bResult;
}
// ****************************************************************************
//
//  Function Name:	RBitmapImage::PathFromImport()
//
//  Description:
//
//  Returns:			TRUE if path was derived from an imported file
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL	RBitmapImage::PathFromImport()
{
	if(m_pClipPath)
	{
		return m_pClipPath->PathFromImport();
	}

	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RImage::RenderTransparent()
//
//  Description:		Paints the image to the device respecting any scaling
//							implied by the source and destination rectangles.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RBitmapImage::RenderTransparent( RDrawingSurface& rDS, const RIntRect& rSrcRect, const RIntRect& rDestRect )
{
	TpsAssert(rDS.GetSurface(), "Drawing surface has no DC.");
	TpsAssert(m_pDibBuffer->GetBuffer(), "Raw data buffer is empty.");

	// Get the bit depth so we can understand the raw data format.
	uLONG nBitDepth = GetBitDepth();

	TpsAssert( (nBitDepth==1) || (nBitDepth == 4) || (nBitDepth == 8) || (nBitDepth == 24), "RenderTransparent only handles 4, 8 and 24 bit images." );
	if ((nBitDepth!=1) && (nBitDepth!=4) && (nBitDepth!=8) && (nBitDepth!=24))
	{
		RenderSection( rDS, rSrcRect, rDestRect );
		return;
	}

	// Determine the dimmensions of the image.
	uLONG nImageWidth = GetWidthInPixels();

	// Get the raw data so that we can walk the image transparently.
	uBYTE *pImageData = (uBYTE *)GetImageData( m_pDibBuffer->GetBuffer() );

	// Set the transparent index value if we are dealing with a palette based image.
	int nTransIndex = 0;
	if ((nBitDepth == 1) || (nBitDepth == 4) || (nBitDepth == 8))
	{
		nTransIndex = GetIndexOfColor( m_rcTransparentColor );
	}

	// Determine the scaling factor to the destination.
	YFloatType yScaleX = (YFloatType)rDestRect.Width() / (YFloatType)rSrcRect.Width();
	YFloatType yScaleY = (YFloatType)rDestRect.Height() / (YFloatType)rSrcRect.Height();

	// Determine the width of the image in bytes.  (Bit depth dependent)
	uWORD nBytesWide = nImageWidth;
	switch (nBitDepth)
	{
		case 1:
			// 1-bit images store 8 pixels per byte.
			// Add seven to the image width before dividing by 8 to round up.
			nBytesWide = (nBytesWide + 7) / 8;
			break;
		case 4:
			// 4-bit images store two pixels per byte.
			// Add one to the image width before dividing by half to round up.
			nBytesWide = (nBytesWide + 1) / 2;
			break;
		case 8:	// Bytes are 1:1, so just break out successfully.
			break;
		case 24:
			// 24 bit images store three bytes for each pixel.
			nBytesWide *= 3;
			break;
		default:
			TpsAssertAlways( "Unsupported bit depth when determining image width." );
			break;
	}
	// Ensure DWORD alignment.
	nBytesWide = (nBytesWide + 3) & 0xfffc;

	// Skip the transparent regions looking for wedges we can blit.
	YIntCoordinate yDestYLast = -1;	// Ensure we paint our first line 1 pixel high.
	for(YIntCoordinate ySrcY = rSrcRect.m_Top; ySrcY < rSrcRect.m_Bottom; ++ySrcY)
	{
		// Set our position to the row data we are currently reading.
		YIntCoordinate ySrcYOffset = ySrcY * nBytesWide;
		uBYTE* pBmpLine = &pImageData[ySrcYOffset];

		// Initialize our starting point in the data line.
		YIntCoordinate	yLeft = rSrcRect.m_Left;

		// Calculate the destination line height based on its last position and
		// its current position.
		YIntCoordinate yDestY = (YIntCoordinate)((YRealCoordinate)ySrcY * yScaleY);
		YIntDimension yDestLineHeight = yDestY - yDestYLast;
		yDestYLast = yDestY;

		// If the destination line is less than 1, then don't bother to draw this line...
		if (yDestLineHeight < 1)
		{
			continue;
		}

		while(yLeft < rSrcRect.m_Right)
		{
			//
			// Find the first non-transparent colored pixel...
			if (nBitDepth <= 8)
			{
				yLeft = SkipTransparentPixels( pBmpLine, yLeft, rSrcRect.m_Right, nTransIndex, nBitDepth );
			}
			else	// 24-bit version.
			{
				yLeft = SkipTransparentPixels( pBmpLine, yLeft, rSrcRect.m_Right );
			}
			if(yLeft < rSrcRect.m_Right)
			{
				//
				// find the last opaque pixel
				YIntCoordinate	yRight;
				if (nBitDepth <= 8)
				{
					yRight = SkipOpaquePixels( pBmpLine, yLeft, rSrcRect.m_Right, nTransIndex, nBitDepth );
				}
				else	// 24-bit version.
				{
					yRight = SkipOpaquePixels( pBmpLine, yLeft, rSrcRect.m_Right );
				}
				//
				// Draw the image
				YIntCoordinate yDestX = (YIntCoordinate)((YRealCoordinate)yLeft * yScaleX);
				YIntDimension yDestLineWidth = (YIntDimension)((YRealCoordinate)(yRight - yLeft) * yScaleX);
				if (yDestLineWidth == 0)
				{
					// Don't allow for no pixel output.  At least one pixel has to be drawn.
					yDestLineWidth = 1;
				}

				RIntRect rSR, rDR;
				rSR.m_Left		= yLeft;
				rSR.m_Top		= ySrcY;
				rSR.m_Right		= yRight;
				rSR.m_Bottom	= ySrcY + 1;

				rDR.m_Left		= rDestRect.m_Left + yDestX;
				rDR.m_Top		= rDestRect.m_Bottom - yDestY - 1;
				rDR.m_Right		= rDR.m_Left + yDestLineWidth;
				rDR.m_Bottom	= rDR.m_Top + yDestLineHeight;

				RenderSection(rDS, rSR, rDR);
				// TPSDebugBlitDS( rDS, rDestRect.Width(), rDestRect.Height() );

				//
				// Reset for next pass...
				yLeft	= yRight;
			}
		}
	}
}


// ****************************************************************************
//
//  Function Name:	RImage::SkipTransparentPixels()
//
//  Description:		Skips any pixels which match the transparent color.  This
//							version only supports 8-bit images.
//
//  Returns:			The coordinate at which the first non-transparent pixel resides.
//
//  Exceptions:		None
//
// ****************************************************************************
YIntCoordinate RBitmapImage::SkipTransparentPixels( uBYTE *pBmpLine, YIntCoordinate xLeft, YIntCoordinate xRight, WORD nTransIndex, uLONG nBitDepth ) const
{
	YIntCoordinate yLeft = xLeft;

	while( yLeft < xRight )
	{
		// Get the correct pixel value from the line.
		WORD nPixelValue;
		if (nBitDepth == 1)
		{
			// Get the byte which holds the pixel in question.
			BYTE nPixelByte = pBmpLine[yLeft / 8];
			// Determine which bit we are dealing with.
			int nBit = yLeft & 0x00000007;
			// Put the desired bit into the low order slot.
			nPixelByte = nPixelByte >> (7 - nBit);
			// Set our pixel value.
			nPixelValue = nPixelByte & 0x01;
		}
		else if (nBitDepth == 4)
		{
			BYTE nPixelByte = pBmpLine[yLeft / 2];
			if (yLeft % 2)
			{
				// We have a remainder, so we want the low order nybble.
				nPixelValue = nPixelByte & 0x0F;
			}
			else
			{
				// No remainder, so we are on an odd nybble which is the high order bit.
				nPixelValue = nPixelByte & 0xF0;
				nPixelValue = nPixelValue >> 4;
			}
		}
		else if (nBitDepth == 8)
		{
			nPixelValue = pBmpLine[yLeft];
		}
		else
		{
			TpsAssertAlways( "Invalid bit depth while skipping transparent pixels during image render." );
		}

		// Is the pixel opaque?
		if (nPixelValue != nTransIndex)
		{
			return yLeft;
		}
		else	// If we didn't find an opaque pixel, increment the counter.
		{
			yLeft++;
		}
	}
	return xRight;
}	



// ****************************************************************************
//
//  Function Name:	RImage::SkipTransparentPixels()
//
//  Description:		Skips any pixels which match the transparent color set for
//							the image.  This version only supports 24-bit images.
//
//  Returns:			The coordinate at which the first non-transparent pixel resides.
//
//  Exceptions:		None
//
// ****************************************************************************
YIntCoordinate RBitmapImage::SkipTransparentPixels( uBYTE *pBmpLine, YIntCoordinate xLeft, YIntCoordinate xRight ) const
{
	YIntCoordinate yLeft = xLeft;
	while( yLeft < xRight )
	{
		// Determine the first byte of the color at the specified pixel location.
		YIntCoordinate yFirstByte = yLeft*3;

		// Determine the colors at the specified pixel.
		BYTE Blue	= pBmpLine[yFirstByte];
		BYTE Green	= pBmpLine[yFirstByte+1];
		BYTE Red		= pBmpLine[yFirstByte+2];

		// Deconstruct the RColor into its components for comparison.
		BYTE TransBlue		= GetBValue(m_rcTransparentColor);
		BYTE TransGreen	= GetGValue(m_rcTransparentColor);
		BYTE TransRed		= GetRValue(m_rcTransparentColor);

		// If the color is not a match, then we have an opaque pixel.
		if ((Blue != TransBlue) || (Green != TransGreen) || (Red != TransRed))
		{
			return yLeft;
		}
		
		// If we didn't find an opaque pixel, increment the counter.
		yLeft++;
	}

	return xRight;
}	


// ****************************************************************************
//
//  Function Name:	RImage::SkipOpaquePixels
//
//  Description:		Skips any pixels which do not match the transparent color.
//							This version only handles 8-bit images.
//
//  Returns:			The coordinate of the first transparent pixel found.
//
//  Exceptions:		None
//
// ****************************************************************************
YIntCoordinate RBitmapImage::SkipOpaquePixels( uBYTE *pBmpLine, YIntCoordinate xLeft, YIntCoordinate xRight, WORD nTransIndex, uLONG nBitDepth ) const
{
	YIntCoordinate yLeft = xLeft;

	while( yLeft < xRight )
	{
		// Get the correct pixel value from the line.
		WORD nPixelValue;
		if (nBitDepth == 1)
		{
			// Get the byte which holds the pixel in question.
			BYTE nPixelByte = pBmpLine[yLeft / 8];
			// Determine which bit we are dealing with.
			int nBit = yLeft & 0x00000007;
			// Put the desired bit into the low order slot.
			nPixelByte = nPixelByte >> (7 - nBit);
			// Set our pixel value.
			nPixelValue = nPixelByte & 0x01;
		}
		else if (nBitDepth == 4)
		{
			BYTE nPixelByte = pBmpLine[yLeft / 2];
			if (yLeft % 2)
			{
				// We have a remainder, so we want the low order nybble.
				nPixelValue = nPixelByte & 0x0F;
			}
			else
			{
				// No remainder, so we are on an odd nybble which is the high order bit.
				nPixelValue = nPixelByte & 0xF0;
				nPixelValue = nPixelValue >> 4;
			}
		}
		else if (nBitDepth == 8)
		{
			nPixelValue = pBmpLine[yLeft];
		}
		else
		{
			TpsAssertAlways( "Invalid bit depth while skipping transparent pixels during image render." );
		}

		// Is the pixel transparent?
		if (nPixelValue == nTransIndex)
		{
			return yLeft;
		}
		else	// If we didn't find a transpernt pixel, increment the counter.
		{
			yLeft++;
		}
	}
	return xRight;
}	



// ****************************************************************************
//
//  Function Name:	RImage::SkipOpaquePixels
//
//  Description:		Skips any pixels which do not match the transparent color
//							set for the image.
//
//							This version only handles 24-bit images.
//
//  Returns:			The coordinate of the first transparent pixel found.
//
//  Exceptions:		None
//
// ****************************************************************************
YIntCoordinate RBitmapImage::SkipOpaquePixels( uBYTE *pBmpLine, YIntCoordinate xLeft, YIntCoordinate xRight ) const
{
	YIntCoordinate yLeft = xLeft;
	while( yLeft < xRight )
	{
		// Determine the first byte of the color at the specified pixel location.
		YIntCoordinate yFirstByte = yLeft*3;

		// Determine the colors at the specified pixel.
		BYTE Blue	= pBmpLine[yFirstByte];
		BYTE Green	= pBmpLine[yFirstByte+1];
		BYTE Red		= pBmpLine[yFirstByte+2];

		// Deconstruct the RColor into its components for comparison.
		BYTE TransBlue		= GetBValue(m_rcTransparentColor);
		BYTE TransGreen	= GetGValue(m_rcTransparentColor);
		BYTE TransRed		= GetRValue(m_rcTransparentColor);

		// If the color is a match, then we have a transparent pixel.
		if ((Blue == TransBlue) && (Green == TransGreen) && (Red == TransRed))
		{
			return yLeft;
		}
		
		// If we didn't find a transparent pixel, increment the counter.
		yLeft++;
	}

	return xRight;
}	

// ****************************************************************************
//
//  Function Name:	RImage::RenderSection()
//
//  Description:		Renders a piece of the image performing a stretch blit from
//							the source rectangle to the destination rectangle.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
void RBitmapImage::RenderSection( RDrawingSurface& rDS, const RIntRect& rSrcRect, const RIntRect& rDestRect )
{
	TpsAssert(rDS.GetSurface(), "Drawing surface has no DC.");
	TpsAssert(m_pDibBuffer->GetBuffer(), "Raw data buffer is empty.");

	HDC hOutputDC = (HDC)rDS.GetSurface();
	int sOldStretchMode = ::SetStretchBltMode(hOutputDC, COLORONCOLOR);
	// We always use SRCCOPY when printing
	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();
	::StretchDIBits(
		hOutputDC,
		rDestRect.m_Left,
		rDestRect.m_Top,
		rDestRect.Width(),
		rDestRect.Height(),
		rSrcRect.m_Left,
		rSrcRect.m_Top,
		rSrcRect.Width(),
		rSrcRect.Height(),
		GetImageData(pBitmapInfo),
		pBitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY);
	::SetStretchBltMode(hOutputDC, sOldStretchMode);
}	

// ****************************************************************************
//
//  Function Name:	RBitmapImage::RenderToScreenOrPrinter()
//
//  Description:		Render to the screen using StretchBlt or to the printer
//							using StretchDIBits. StretchBlt works OK on most printers
//							but for at least one (the HP 693) the image is lightened,
//							and for at least one other (the HP 600) the image is not
//							printed at all, presumably due to palette or other screen
//							dependencies.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
void RBitmapImage::RenderToScreenOrPrinter( RDrawingSurface& rSrcDS, RDrawingSurface& rDstDS, const RIntRect& rSrcRect, const RIntRect& rDestRect )
{
	TpsAssert( rDstDS.GetSurface(), "Destination drawing surface has no DC." );
	HDC hOutputDC = (HDC)rDstDS.GetSurface();

	int sOldStretchMode = ::SetStretchBltMode( hOutputDC, COLORONCOLOR );

	if (!rDstDS.IsPrinting())
	{
		TpsAssert( rSrcDS.GetSurface(), "Source drawing surface has no DC." );
		HDC hSourceDC = (HDC)rSrcDS.GetSurface();

		::StretchBlt(
			hOutputDC, 
			rDestRect.m_Left,
			rDestRect.m_Top,
			rDestRect.Width(),
			rDestRect.Height(),
			hSourceDC,
			rSrcRect.m_Left,
			rSrcRect.m_Top,
			rSrcRect.Width(),
			rSrcRect.Height(),
			kBlitSourceCopy );
	}
	else
	{
		TpsAssert( m_pDibBuffer->GetBuffer(), "Raw data buffer is empty." );
		BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();

		// We always use SRCCOPY when printing
		int fResult = ::StretchDIBits(
			hOutputDC,
			rDestRect.m_Left,
			rDestRect.m_Top,
			::Abs( rDestRect.Width() ),
			::Abs( rDestRect.Height() ),
			rSrcRect.m_Left,
			rSrcRect.m_Top,
			::Abs( rSrcRect.Width() ),
			::Abs( rSrcRect.Height() ),
			GetImageData( pBitmapInfo ),
			pBitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY );

		//	If we failed to draw, flip the top and bottom of the source rect if
		// necessary and recursively subdivide on smaller images.
		if (fResult == GDI_ERROR)
		{
			// if biHeight field is >=0 data is bottom-up, else top-down
			RIntRect rTempSrcRect( rSrcRect );
			if (((BITMAPINFOHEADER*)GetRawData())->biHeight >= 0)
			{
				int temp;
				temp = rTempSrcRect.m_Top, rTempSrcRect.m_Top = rTempSrcRect.m_Bottom, rTempSrcRect.m_Bottom = temp;
			}

			RenderToPrinterSubdivide( rSrcDS, rDstDS, rTempSrcRect, rDestRect );
		}
	}

	::SetStretchBltMode( hOutputDC, sOldStretchMode );
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::RenderToPrinterSubdivide()
//
//  Description:		Render to the printer using StretchDIBits. If it fails,
//							recursively subdivide the image and try again until it
//							succeeds or the subdivided image is very small.
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
void RBitmapImage::RenderToPrinterSubdivide( RDrawingSurface& rSrcDS, RDrawingSurface& rDstDS, const RIntRect& rSrcRect, const RIntRect& rDestRect )
{
	TpsAssert( rDstDS.GetSurface(), "Destination drawing surface has no DC." );
	HDC hOutputDC = (HDC)rDstDS.GetSurface();

	int sOldStretchMode = ::SetStretchBltMode( hOutputDC, COLORONCOLOR );

	TpsAssert( m_pDibBuffer->GetBuffer(), "Raw data buffer is empty." );
	BITMAPINFO* pBitmapInfo = (BITMAPINFO*)m_pDibBuffer->GetBuffer();

	// We always use SRCCOPY when printing
	int fResult = ::StretchDIBits(
		hOutputDC,
		rDestRect.m_Left,
		rDestRect.m_Top,
		::Abs( rDestRect.Width() ),
		::Abs( rDestRect.Height() ),
		rSrcRect.m_Left,
		rSrcRect.m_Top,
		::Abs( rSrcRect.Width() ),
		::Abs( rSrcRect.Height() ),
		GetImageData( pBitmapInfo ),
		pBitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY );

	//	If we failed to draw, recursively subdivide on smaller images.
	// Ground the recursion by stopping when the bitmap is tiny.
	if (fResult == GDI_ERROR && (rDestRect.Width() > 1) && (rDestRect.Height() > 1))
	{
		// Try breaking the image up into smaller chunks and blit again
		RIntRect rSrcRectUL, rSrcRectLL, rSrcRectUR, rSrcRectLR;
		RIntRect rDestRectUL, rDestRectLL, rDestRectUR, rDestRectLR;
		
		// if biHeight field is >=0 data is bottom-up, else top-down
		BOOLEAN bottomUp = FALSE;
		if (((BITMAPINFOHEADER*)GetRawData())->biHeight >= 0)
			bottomUp = TRUE;
		int temp;

		// Upper-left rects
		rSrcRectUL.m_Left = rSrcRect.m_Left;
		rSrcRectUL.m_Top = rSrcRect.m_Top;
		rSrcRectUL.m_Right = rSrcRect.m_Left + (rSrcRect.Width() / 2);
		rSrcRectUL.m_Bottom = rSrcRect.m_Top + (rSrcRect.Height() / 2);

		rDestRectUL.m_Left = rDestRect.m_Left;
		rDestRectUL.m_Top = rDestRect.m_Top;
		rDestRectUL.m_Right = rDestRect.m_Left + (rDestRect.Width() / 2);
		rDestRectUL.m_Bottom = rDestRect.m_Top + (rDestRect.Height() / 2);
					
		if (bottomUp)
			temp = rSrcRectUL.m_Top, rSrcRectUL.m_Top = rSrcRectUL.m_Bottom, rSrcRectUL.m_Bottom = temp;

		RenderToPrinterSubdivide( rSrcDS, rDstDS, rSrcRectUL, rDestRectUL );

		// Lower-left rects
		rSrcRectLL.m_Left = rSrcRect.m_Left;
		rSrcRectLL.m_Top = rSrcRect.m_Top + (rSrcRect.Height() / 2);
		rSrcRectLL.m_Right = rSrcRect.m_Left + (rSrcRect.Width() / 2);
		rSrcRectLL.m_Bottom = rSrcRect.m_Bottom;

		rDestRectLL.m_Left = rDestRect.m_Left;
		rDestRectLL.m_Top = rDestRect.m_Top + (rDestRect.Height() / 2);
		rDestRectLL.m_Right = rDestRect.m_Left + (rDestRect.Width() / 2);
		rDestRectLL.m_Bottom = rDestRect.m_Bottom;

		if (bottomUp)
			temp = rSrcRectLL.m_Top, rSrcRectLL.m_Top = rSrcRectLL.m_Bottom, rSrcRectLL.m_Bottom = temp;

		RenderToPrinterSubdivide( rSrcDS, rDstDS, rSrcRectLL, rDestRectLL );

		// Upper-right rects
		rSrcRectUR.m_Left = rSrcRect.m_Left + (rSrcRect.Width() / 2);
		rSrcRectUR.m_Top = rSrcRect.m_Top;
		rSrcRectUR.m_Right = rSrcRect.m_Right;
		rSrcRectUR.m_Bottom = rSrcRect.m_Top + (rSrcRect.Height() / 2);
		
		rDestRectUR.m_Left = rDestRect.m_Left + (rDestRect.Width() / 2);
		rDestRectUR.m_Top = rDestRect.m_Top;
		rDestRectUR.m_Right = rDestRect.m_Right;
		rDestRectUR.m_Bottom = rDestRect.m_Top + (rDestRect.Height() / 2);

		if (bottomUp)
			temp = rSrcRectUR.m_Top, rSrcRectUR.m_Top = rSrcRectUR.m_Bottom, rSrcRectUR.m_Bottom = temp;

		RenderToPrinterSubdivide( rSrcDS, rDstDS, rSrcRectUR, rDestRectUR );

		// Lower-right rects
		rSrcRectLR.m_Left = rSrcRect.m_Left + (rSrcRect.Width() / 2);
		rSrcRectLR.m_Top = rSrcRect.m_Top + (rSrcRect.Height() / 2);
		rSrcRectLR.m_Right = rSrcRect.m_Right;
		rSrcRectLR.m_Bottom = rSrcRect.m_Bottom;
		
		rDestRectLR.m_Left = rDestRect.m_Left + (rDestRect.Width() / 2);
		rDestRectLR.m_Top = rDestRect.m_Top + (rDestRect.Height() / 2);
		rDestRectLR.m_Right = rDestRect.m_Right;
		rDestRectLR.m_Bottom = rDestRect.m_Bottom;

		if (bottomUp)
			temp = rSrcRectLR.m_Top, rSrcRectLR.m_Top = rSrcRectLR.m_Bottom, rSrcRectLR.m_Bottom = temp;

		RenderToPrinterSubdivide( rSrcDS, rDstDS, rSrcRectLR, rDestRectLR );
	}

	::SetStretchBltMode( hOutputDC, sOldStretchMode );
}

// ****************************************************************************
//
//  Function Name:	RImage::LockImage()
//
//  Description:		Allocates the DIBSection Handle
//
//  Returns:			Nothing.
//
//  Exceptions:		kResourceException
//
// ****************************************************************************
void RBitmapImage::LockImage( )
{ 
	if( (m_nLockCount++) == 0 )
	{
		TpsAssert( m_hBitmap == NULL, "Locking an image that is currently locked" );
		InitializeDibSection();
	}
}


// ****************************************************************************
//
//  Function Name:	RImage::UnlockImage()
//
//  Description:		Frees the DIBSection Handle
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
void RBitmapImage::UnlockImage( )
{
	TpsAssert( m_hBitmap, "Unlocking an image that was never Locked" );
	TpsAssert( m_nLockCount > 0, "Unlocking an image too many times." );

	if ( (--m_nLockCount) == 0 )
	{
		// Release all memory associated with this image and reset all internal parameters
		if (m_hBitmap) 
			::DeleteObject(m_hBitmap);	
		m_hBitmap = NULL;	
	}
}
