// *****************************************************************************
//
// File name:			DeviceDepBitmapImage.cpp
//
// Project:				Renaissance Framework
//
// Author:				S. Athanas
//
// Description:		Definition of the RDeviceDependentBitmapImage class
//
// Portability:		Windows
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
//  $Logfile:: /PM8/Framework/Source/DeviceDepBitmapImage.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "DeviceDepBitmapImage.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "OffscreenDrawingSurface.h"

const uLONG kDefaultDDBXDpi = 200;
const uLONG kDefaultDDBYDpi = 200;

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::RDeviceDependentBitmapImage()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		kResource
//
// ****************************************************************************
//
RDeviceDependentBitmapImage::RDeviceDependentBitmapImage( BOOLEAN )
	: m_hBitmap( NULL )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::~RDeviceDependentBitmapImage()
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDeviceDependentBitmapImage::~RDeviceDependentBitmapImage( )
	{
	Delete( );
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Delete( )
//
//  Description:		Deletes the bitmap if it exists
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeviceDependentBitmapImage::Delete( )
	{
	if( m_hBitmap )
		::DeleteObject( m_hBitmap );
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::GetWidthInPixels()
//
//  Description:		Returns the width of the bitmap, in pixels
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
uLONG RDeviceDependentBitmapImage::GetWidthInPixels( ) const
	{
	if (!m_hBitmap) return 0;

	return (uLONG) m_Size.m_dx ;
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::GetHeightInPixels()
//
//  Description:		Returns the height of the bitmap, in pixels
//
//  Returns:			Nothing	
//
//  Exceptions:		None
//
// ****************************************************************************
uLONG RDeviceDependentBitmapImage::GetHeightInPixels( ) const
	{
	if (!m_hBitmap) return 0;

	return (uLONG) m_Size.m_dy ;
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Initialize( )
//
//  Description:		Initializer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeviceDependentBitmapImage::Initialize( YImageHandle )
	{
	UnimplementedCode( );
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Initialize( )
//
//  Description:		Initializer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeviceDependentBitmapImage::Initialize( uLONG uWidthInPixels, uLONG uHeightInPixels, uLONG uBitDepth )
	{
	Delete( );

	m_Size.m_dx = uWidthInPixels;
	m_Size.m_dy = uHeightInPixels;

	m_hBitmap = ::CreateBitmap( uWidthInPixels, uHeightInPixels, 1, uBitDepth, NULL );

	if( !m_hBitmap )
		throw kResource;
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Initialize( )
//
//  Description:		Initializer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeviceDependentBitmapImage::Initialize( RDrawingSurface& drawingSurface, uLONG uWidthInPixels, uLONG uHeightInPixels )
	{
	Delete( );

	m_Size.m_dx = uWidthInPixels;
	m_Size.m_dy = uHeightInPixels;

	HDC hDC = (HDC)drawingSurface.GetSurface( );

	m_hBitmap = ::CreateCompatibleBitmap( hDC, uWidthInPixels, uHeightInPixels );

	if( !m_hBitmap )
		throw kResource;
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Render()
//
//  Description:		Displays the image on the drawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeviceDependentBitmapImage::Render( RDrawingSurface& drawingSurface, const RIntRect& rDestRect )
	{
//	ROffscreenDrawingSurface rBitmapDS;
//	rBitmapDS.SetImage( this );
//	drawingSurface.BlitDrawingSurface( rBitmapDS, m_Size, rDestRect, kBlitSourceCopy );
//	rBitmapDS.ReleaseImage( );

	RIntRect rSrcRect( 0, 0, GetWidthInPixels(), GetHeightInPixels() );
	Render( drawingSurface, rSrcRect, rDestRect ) ;
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Render()
//
//  Description:		Displays the image on the drawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RDeviceDependentBitmapImage::Render( RDrawingSurface& rDS, const RIntRect& rSrcRect, const RIntRect& rDestRect )
	{
		TpsAssert(rDS.GetSurface(), "Drawing surface has no DC.");

		ROffscreenDrawingSurface rBitmapDS;
		rBitmapDS.SetImage(this);
		rDS.BlitDrawingSurface(rBitmapDS, rSrcRect, rDestRect, kBlitSourceCopy);
		rBitmapDS.ReleaseImage();
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::RenderWithMask()
//
//  Description:		Displays the unmasked portion of the image on the
//							drawing surface
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RDeviceDependentBitmapImage::RenderWithMask(RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rDestRect)
{
	RIntRect rSrcRect(0, 0, GetWidthInPixels(), GetHeightInPixels());
	RenderWithMask(rDS, rMaskBitmap, rSrcRect, rDestRect);
}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Render()
//
//  Description:		Displays the image on the drawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeviceDependentBitmapImage::RenderWithMask( RDrawingSurface& drawingSurface, RBitmapImageBase& mask, const RIntRect& rSrcRect, const RIntRect& rDestRect )
	{
	TpsAssert( drawingSurface.IsPrinting( ) == FALSE, "Can not use DDBs for printing." );
	TpsAssertIsObject( RDeviceDependentBitmapImage, &mask );
	
	ROffscreenDrawingSurface rMaskDS, rSourceDS;				
	
	// Set the source image into the source DS
	rSourceDS.SetImage(this);			

	rMaskDS.SetImage(&mask);

	// 1) Cut a hole in the destination drawing surface
	drawingSurface.BlitDrawingSurface(rMaskDS, rSrcRect, rDestRect, kBlitNotSourceAnd);
	
	// 2) Isolate this image using the mask
	rSourceDS.BlitDrawingSurface(rMaskDS, rSrcRect, rSrcRect, kBlitSourceAnd);
			
	// 3) Copy the isolated image to the output of this drawing surface
	drawingSurface.BlitDrawingSurface(rSourceDS, rSrcRect, rDestRect, kBlitSourcePaint);		

	// Release the images		
	rSourceDS.ReleaseImage();
	rMaskDS.ReleaseImage();
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Read()
//
//  Description:		Reads
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeviceDependentBitmapImage::Read( RArchive& )
	{	
	UnimplementedCode( );
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Write()
//
//  Description:		Writes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeviceDependentBitmapImage::Write( RArchive& ) const
	{	
	UnimplementedCode( );
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Copy()
//
//  Description:		Copies to a data target
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeviceDependentBitmapImage::Copy( RDataTransferTarget&, YDataFormat ) const
	{
	UnimplementedCode( );
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::Paste()
//
//  Description:		Pastes from a data source
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDeviceDependentBitmapImage::Paste( const RDataTransferSource&, YDataFormat )
	{
	UnimplementedCode( );
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::GetSystemHandle( )
//
//  Description:		Returns the HBITMAP
//
//  Returns:			HBITMAP
//
//  Exceptions:		None
//
// ****************************************************************************
//
YImageHandle RDeviceDependentBitmapImage::GetSystemHandle( ) const
	{
	return (YImageHandle)m_hBitmap;
	}

// ****************************************************************************
//
//  Function Name:	RDeviceDependentBitmapImage::GetSizeInLogicalUnits( )
//
//  Description:		Returns the size in RLUs
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RDeviceDependentBitmapImage::GetSizeInLogicalUnits( ) const
{
	return RRealSize(::InchesToLogicalUnits(static_cast<YRealDimension>(GetWidthInPixels()) / static_cast<YRealDimension>(kDefaultDDBXDpi)),
		::InchesToLogicalUnits(static_cast<YRealDimension>(GetHeightInPixels()) / static_cast<YRealDimension>(kDefaultDDBYDpi)));
}