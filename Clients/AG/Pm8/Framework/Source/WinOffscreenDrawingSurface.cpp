// ****************************************************************************
//
//  File Name:			OffscreenDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				G. Brown, M. Houle
//
//  Description:		Declaration of the ROffscreenDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Source/WinOffscreenDrawingSurface.cpp           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "OffscreenDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "BitmapImage.h"
#include "VectorImage.h"
#include "DeviceDepBitmapImage.h"
#include "PrintManager.h"
#include "Printer.h"
#include	"Document.h"
#include "PrinterDrawingSurface.h"
#include "DefaultDrawingSurface.h"

#ifdef _WINDOWS

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::ROffscreenDrawingSurface()
//
//  Description:		Creates an offscreen DS compatible with the screen.
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
ROffscreenDrawingSurface::ROffscreenDrawingSurface( BOOLEAN fIsPrinting /* = FALSE */, BOOLEAN fClip /* = TRUE */, RDrawingSurface* pAttributeDrawingSurface /* = NULL */ )
: BaseDrawingSurface( FALSE, fIsPrinting ),
  m_fClip( fClip )
{
	m_pImage = NULL;	
	m_hOldBitmap = NULL;
	m_hOffscreenDC = NULL;	
	if (pAttributeDrawingSurface)
		CopyAttributes( *pAttributeDrawingSurface );
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::~ROffscreenDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
ROffscreenDrawingSurface::~ROffscreenDrawingSurface()
{
	TpsAssert(m_pImage == NULL, "Did not release image.");	
	
	// Clean up if we need to
	if ((m_hOffscreenDC) && (m_hOldBitmap))
	{
		::SelectObject(m_hOffscreenDC, m_hOldBitmap);
		::DeleteDC(m_hOffscreenDC);
	}
}
	
// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::SetImage()
//
//  Description:		Sets an image into the ODS
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void ROffscreenDrawingSurface::SetImage(RImage* pImage)
{	
	TpsAssert(m_pImage == NULL, "Previous image not released.");
	TpsAssert(pImage, "pImage = NULL.");

	//	Make sure we can access the image data.  If lock throws, we could not get the
	//		actual system handle.  This is only a problem with RBitmapImage currently.
	pImage->LockImage( );

	// Check to see if we've got an RBitmapImage or an RVectorImage
	RBitmapImage* pBitmapImage = dynamic_cast<RBitmapImage*>(pImage);
	RDeviceDependentBitmapImage* pDDBImage = dynamic_cast<RDeviceDependentBitmapImage*>(pImage);
	if (pBitmapImage || pDDBImage) 
	{
		SetBitmapImage(pImage);
		return;
	}

	RVectorImage* pVectorImage = dynamic_cast<RVectorImage*>(pImage);
	if (pVectorImage) 
	{
		SetVectorImage(pVectorImage);
		return;
	}
}
	
// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::SetBitmapImage()
//
//  Description:		Sets a bitmap image into the ODS
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void ROffscreenDrawingSurface::SetBitmapImage(RImage* pBitmapImage)
{
	HDC hMemDC = NULL;
	HBITMAP hNewBitmap = NULL;
	HBITMAP hOldBitmap = NULL;
	try
	{
		hMemDC = ::CreateCompatibleDC(NULL);
		if (!hMemDC) ThrowExceptionOnError();

		hOldBitmap = (HBITMAP)::SelectObject(hMemDC, (HBITMAP)pBitmapImage->GetSystemHandle());
		if (!hOldBitmap) ThrowExceptionOnError();
		
		BaseDrawingSurface::Initialize(hMemDC, hMemDC);
	}
	catch(YException)
	{
		DetachDCs();
		if (hMemDC) 
		{
			if (hOldBitmap) ::SelectObject(hMemDC, hOldBitmap);				
			::DeleteDC(hMemDC);
		}
		throw;
	}
		
	// Set the members
	m_pImage			= pBitmapImage;
	m_hOffscreenDC	= hMemDC;
	m_hOldBitmap	= hOldBitmap;	
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::SetVectorImage()
//
//  Description:		Sets a vector image into the ODS
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void ROffscreenDrawingSurface::SetVectorImage(RVectorImage* pVectorImage)
{
	HDC hEnhMetaDC = NULL;	
	RDrawingSurface* pDS = NULL;
	BOOLEAN fPrinterDS = FALSE;
	RPrinter* pDefaultPrinter = RPrintManager::GetPrintManager().GetDefaultPrinter();
	try
	{
		RRealSize rSizeInches(pVectorImage->GetSuggestedWidthInInches(), pVectorImage->GetSuggestedHeightInInches());
		RRealSize rSizeMillimeters(::InchesToMillimeters(rSizeInches.m_dx), ::InchesToMillimeters(rSizeInches.m_dy));

		// Get a reference DC		
		if (pDefaultPrinter && !pDefaultPrinter->IsTemplate())
		{
			pDS = pDefaultPrinter->CreateDrawingSurface( NULL );
			fPrinterDS = TRUE;
		}
		else
		{
			pDS = new RDefaultDrawingSurface;
		}

		// Create the metafile
		CRect cRect(0, 0, rSizeMillimeters.m_dx * 100.0, rSizeMillimeters.m_dy * 100.0);
		hEnhMetaDC = ::CreateEnhMetaFile(reinterpret_cast<HDC>(pDS->GetSurface()), NULL, &cRect, NULL);
		if (!hEnhMetaDC) ThrowExceptionOnError();

		// Get the attributes of the reference DC
		const RRealSize kDpi(static_cast<YRealDimension>(::GetDeviceCaps(reinterpret_cast<HDC>(pDS->GetSurface()), LOGPIXELSX)), static_cast<YRealDimension>(::GetDeviceCaps(reinterpret_cast<HDC>(pDS->GetSurface()), LOGPIXELSY)));		
		const RRealSize kDpmm(static_cast<YRealDimension>(::GetDeviceCaps(reinterpret_cast<HDC>(pDS->GetSurface()), HORZRES)) / static_cast<YRealDimension>(::GetDeviceCaps(reinterpret_cast<HDC>(pDS->GetSurface()), HORZSIZE)), static_cast<YRealDimension>(::GetDeviceCaps(reinterpret_cast<HDC>(pDS->GetSurface()), VERTRES)) / static_cast<YRealDimension>(::GetDeviceCaps(reinterpret_cast<HDC>(pDS->GetSurface()), VERTSIZE)));

		// Set the mapping mode in the metafile DC
		if (!::SetMapMode(hEnhMetaDC, MM_ANISOTROPIC)) ThrowExceptionOnError();
		if (!::SetWindowExtEx(hEnhMetaDC, rSizeInches.m_dx * kDpi.m_dx, rSizeInches.m_dy * kDpi.m_dy, NULL)) ThrowExceptionOnError();
		if (!::SetViewportExtEx(hEnhMetaDC, rSizeMillimeters.m_dx * kDpmm.m_dx, rSizeMillimeters.m_dy * kDpmm.m_dy, NULL)) ThrowExceptionOnError();

		// Set the stretchblt mode so color bitmaps in metafiles look good
		::SetStretchBltMode(hEnhMetaDC, COLORONCOLOR);

		BaseDrawingSurface::Initialize(hEnhMetaDC, hEnhMetaDC);

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
		pDS = NULL;

		// Set the printing flag so we don't clip
		m_fIsPrinting = TRUE;
	}
	catch(YException)		
	{
		DetachDCs();		
		if (hEnhMetaDC) ::CloseEnhMetaFile(hEnhMetaDC);
		if (fPrinterDS)
		{
			RPrinterDrawingSurface* pPDS = static_cast<RPrinterDrawingSurface*>(pDS);
			pDefaultPrinter->DestroyDrawingSurface(pPDS);	
		}
		else
		{
			delete pDS;
		}
		pDS = NULL;
		throw;
	}
		
	// Set the members
	m_pImage			= pVectorImage;
	m_hOffscreenDC	= hEnhMetaDC;	
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::ReleaseImage()
//
//  Description:		Releases an RImage from the DS
//
//  Returns:			A pointer to the RImage
//
//  Exceptions:		None
//
// ****************************************************************************
RImage* ROffscreenDrawingSurface::ReleaseImage()
{
	TpsAssert( m_pImage, "ReleaseImage called but no image class present");

	RImage*	pTempImage = m_pImage;

	// Check to see if we've got an RBitmapImage or an RVectorImage
	RBitmapImage* pBitmapImage = dynamic_cast<RBitmapImage*>(m_pImage);
	RDeviceDependentBitmapImage* pDDBImage = dynamic_cast<RDeviceDependentBitmapImage*>(m_pImage);
	if (pBitmapImage || pDDBImage )
	{
		DetachDCs();
		if ((m_hOffscreenDC) && (m_hOldBitmap))
		{		
			::SelectObject(m_hOffscreenDC, m_hOldBitmap);				
			::DeleteDC(m_hOffscreenDC);
		}
	}
	else
	{
		RVectorImage* pVectorImage = dynamic_cast<RVectorImage*>(m_pImage);
		if (pVectorImage)
		{		
			DetachDCs();	
			if (m_hOffscreenDC) 
			{
				HENHMETAFILE hEnhMetaFile = ::CloseEnhMetaFile(m_hOffscreenDC);		
			
				// Initialize the vector image with the metafile handle
				pVectorImage->Initialize((YImageHandle)hEnhMetaFile);			
			}	
		}
		else
		{
			TpsAssertAlways( "Invalid type of image put into the offscreen drawing surface" );
			NULL;
		}
	}

	m_pImage = NULL;
	m_hOffscreenDC = NULL;
	m_hOldBitmap = NULL;

	if ( pTempImage )
		pTempImage->UnlockImage( );

	return pTempImage;
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::GetDPI( )
//
//  Description:		Gets the bit depth of the current off screen image
//
//  Returns:			Do not know yet
//
//  Exceptions:		None
//
// ****************************************************************************
RIntSize ROffscreenDrawingSurface::GetDPI() const
{
	return BaseDrawingSurface::GetDPI();
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::GetBitDepth( )
//
//  Description:		Gets the bit depth of the current off screen image
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
uLONG ROffscreenDrawingSurface::GetBitDepth() const
{
	return BaseDrawingSurface::GetBitDepth();
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::GetSurface( )
//
//  Description:		Return an identifier to the Output Drawing Surface.
//
//  Returns:			m_pOffGrafPort
//
//  Exceptions:		None
//
// ****************************************************************************
//
long ROffscreenDrawingSurface::GetSurface( ) const
{
	return BaseDrawingSurface::GetSurface( );
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::GetClipRect( )
//
//  Description:		Gets the tightest bounding rectangle around the current
//							clipping region.
//
//  Returns:			A rect containing the current clip rect, in device units
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect ROffscreenDrawingSurface::GetClipRect( ) const
	{
	// Dont clip for metafiles. We run into problems when copying gradients to the clipboard.
	if( !m_fClip && dynamic_cast<RVectorImage*>(m_pImage) )
		return RIntRect( -SHRT_MAX, -SHRT_MAX, SHRT_MAX, SHRT_MAX );
	else
		return BaseDrawingSurface::GetClipRect( );
	}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::GetBitmapImage() const
//
//  Returns:			Returns a pointer to the RBitmapImage, if any, associated
//							with this object
//
//  Exceptions:		None
//
// ****************************************************************************
RBitmapImage* ROffscreenDrawingSurface::GetBitmapImage() const
{
	RBitmapImage* bitmapImage = dynamic_cast<RBitmapImage*>(m_pImage);
	return bitmapImage;
}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::IsVisible( const RIntPoint& pt )
//
//  Description:		return TRUE if the point is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN ROffscreenDrawingSurface::IsVisible( const RIntPoint& point )
	{
	if( m_fClip )
		return BaseDrawingSurface::IsVisible( point );

	else
		return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	ROffscreenDrawingSurface::IsVisible( const RIntRect& rc )
//
//  Description:		return TRUE if the rect is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN ROffscreenDrawingSurface::IsVisible( const RIntRect& rect )
	{
	if( m_fClip )
		return BaseDrawingSurface::IsVisible( rect );

	else
		return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RImage::ThrowExceptionOnError()
//
//  Description:		Throws a Renaissance exception based on the value
//							returned from the Win32 call ::GetLastError()
//
//  Returns:			
//
//  Exceptions:		kMemory; kDiskFull; kUnknownError
//
// ****************************************************************************
void ROffscreenDrawingSurface::ThrowExceptionOnError()
{
	DWORD uLastError = ::GetLastError();
	switch(uLastError)
	{
	case NO_ERROR:
		TpsAssertAlways("An error condition was encountered, but Windows did not set last error.");
		throw ::kUnknownError;

	case ERROR_OUTOFMEMORY:
	case ERROR_NOT_ENOUGH_MEMORY:
		throw ::kMemory;
	
	case ERROR_DISK_FULL:
		throw ::kDiskFull;		

	default:
		throw ::kUnknownError;
	}
}

#endif _WINDOWS