// *****************************************************************************
//
// File name:			MacBitmapImage.cpp
//
// Project:				Renaissance Framework
//
// Author:				R. Hood
//
// Description:		Mac-specific definition of the RBitmapImage class members
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
//  $Logfile:: /PM8/Framework/Source/MacBitmapImage.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "BitmapImage.h"
#include "GpDrawingSurface.h"
#include "OffscreenDrawingSurface.h"
#include "PolyPolygon.h"
#include "ExceptionHandling.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

const	uLONG    		kBitmapFlags				= ( TBIT_ZeroInit | TBIT_LongAlignRows | TBIT_DefaultColorTable | TBIT_16ByteAlignRows );


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
RBitmapImage::RBitmapImage( BOOLEAN fStoreOnDisk ) : m_hBitmap( NULL )
{
	TpsAssert( !fStoreOnDisk, "Bitmap disk store not allowed on Mac.");
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
RBitmapImage::RBitmapImage( const RBitmapImage& rhs )
{
TRECT			trc;
uLONG 		uWidthInPixels = rhs.GetWidthInPixels();
uLONG 		uHeightInPixels = rhs.GetHeightInPixels();
RBitmapImage* prhs = const_cast<RBitmapImage*>( &rhs );
HTBIT			hbit = reinterpret_cast<HTBIT>( prhs->GetSystemHandle() );

	m_hBitmap = NULL;
	if ( hbit == NULL)
		return;

	trc.top = trc.left = 0;
	trc.bottom = uHeightInPixels;
	trc.right = uWidthInPixels;
	Initialize( uWidthInPixels, uHeightInPixels, rhs.GetBitDepth() );
	if ( m_hBitmap == NULL)
		return;
		
	::tbitBlitToBitmap( hbit, m_hBitmap, &trc, &trc, TBIT_SrcCopy, FALSE );
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
	if ( m_hBitmap != NULL )
		::tbitFree( m_hBitmap );
}


// ****************************************************************************
//
//  Function Name:	RBitmapImage::Initialize()
//
//  Description:		(Re)creates the bitmap with the given attributes
//
//  Returns:			
//
//  Exceptions:		kMemory
//
// ****************************************************************************
void RBitmapImage::Initialize( uLONG uWidthInPixels, uLONG uHeightInPixels, uLONG uBitDepth, uLONG uXDpi, uLONG uYDpi )
{
TRECT			trc;
HTBIT			hbit;

	TpsAssert( ( uWidthInPixels > 0 ), "Bitmap cannot have 0 width.");
	TpsAssert( ( uHeightInPixels > 0 ), "Bitmap cannot have 0 height.");
	TpsAssert( ( uBitDepth > 0 ), "Bitmap cannot have 0 BitDepth.");
	TpsAssert( ( uXDpi > 0 ), "Bitmap cannot have 0 X-Dpi.");
	TpsAssert( ( uYDpi > 0 ), "Bitmap cannot have 0 Y-Dpi.");
	TpsAssert( ( uXDpi == 72 ), "Bitmap must have 72 X-Dpi on Mac.");
	TpsAssert( ( uYDpi == 72 ), "Bitmap must have 72 Y-Dpi on Mac.");

	trc.top = trc.left = 0;
	trc.bottom = uHeightInPixels;
	trc.right = uWidthInPixels;
	hbit = ::tbitAlloc( uBitDepth, &trc, kBitmapFlags );
	if ( hbit == NULL ) 
		throw kMemory;

	if ( m_hBitmap != NULL )
		::tbitFree( m_hBitmap );
	m_hBitmap = hbit;
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
//  Exceptions:		kMemory
//
// ****************************************************************************
void RBitmapImage::Initialize( RDrawingSurface& rDS, uLONG uWidthInPixels, uLONG uHeightInPixels )
{
uLONG 	uBitDepth = rDS.GetBitDepth();
RIntSize dpi = rDS.GetDPI( );

	TpsAssert( ( uWidthInPixels > 0 ), "Bitmap cannot have 0 width.");
	TpsAssert( ( uHeightInPixels > 0 ), "Bitmap cannot have 0 height.");

	Initialize( uWidthInPixels, uHeightInPixels, uBitDepth, dpi.m_dx, dpi.m_dy );
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
//  Exceptions:		kMemory
//
// ****************************************************************************
void RBitmapImage::Initialize( YImageHandle yHandle )
{
	UnimplementedCode();
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
//  Exceptions:		kMemory
//
// ****************************************************************************
void RBitmapImage::Initialize( YResourceId nId )
{
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::Initialize()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RBitmapImage::Initialize( void* pRawData )
{	
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	Uninitialize()
//
//  Description:		Clears all the internal bitmap data
//
//  Returns:			
//
//  Exceptions:		none
//
// ****************************************************************************
void RBitmapImage::Uninitialize( )
{
	// Release all memory associated with this image and reset all internal parameters
	if ( m_hBitmap != NULL )
		::tbitFree( m_hBitmap );
	m_hBitmap = NULL;	
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetSystemHandle()
//
//  Description:		Returns the system handle of the bitmap image
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YImageHandle RBitmapImage::GetSystemHandle() const
{
	UntestedCode();
	return (YImageHandle)m_hBitmap;
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
void RBitmapImage::operator=( const RImage& rhs )
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
void RBitmapImage::operator=( const RBitmapImage& rhs )
{
	// Check for assignment to self
	if (this == &rhs) return;

	// Copy the data buffer
	UnimplementedCode();
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
	if ( m_hBitmap != NULL )
		return ::tbitGetDepth( m_hBitmap );
	return 0;
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
	return 72;							// hardcoded in the tps bitmap library
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
	return 72;							// hardcoded in the tps bitmap library
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
void RBitmapImage::Render( RDrawingSurface& rDS, const RIntRect& rDestRect )
{
	RIntRect rSrcRect( 0, 0, GetWidthInPixels(), GetHeightInPixels() );
	Render( rDS, rSrcRect, rDestRect );
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
void RBitmapImage::Render( RDrawingSurface& rDS, const RIntRect& rSrcRect, const RIntRect& rDestRect )
{
	TpsAssert( rDS.GetSurface(), "Drawing surface has no device." );
	TpsAssert( ( m_hBitmap != NULL ), "Bitmap is empty.");

	if ( rDS.IsPrinting() )
	{
		UnimplementedCode();
	}	
	else
	{	
		ROffscreenDrawingSurface	rBitmapDS;
		rBitmapDS.SetImage( this );
		rDS.BlitDrawingSurface( rBitmapDS, rSrcRect, rDestRect, kBlitSourceCopy );
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
void RBitmapImage::RenderWithMask( RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rDestRect )
{
	RIntRect rSrcRect( 0, 0, GetWidthInPixels(), GetHeightInPixels() );
	RenderWithMask( rDS, rMaskBitmap, rSrcRect, rDestRect );
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
void RBitmapImage::RenderWithMask( RDrawingSurface& rDS, RBitmapImageBase& rMaskBitmap, const RIntRect& rSrcRect, const RIntRect& rDestRect )
{
	TpsAssert( rMaskBitmap.GetWidthInPixels() == GetWidthInPixels(), "Mask width does not match bitmap width." );
	TpsAssert( rMaskBitmap.GetHeightInPixels() == GetHeightInPixels(), "Mask height does not match bitmap height." );
	TpsAssert( rDS.GetSurface(), "rDS.GetSurface() = NULL." );

	if ( rDS.IsPrinting() )
	{
		UnimplementedCode();
	}
	else
	{	
		ROffscreenDrawingSurface	rMaskDS, rSourceDS;
		rMaskDS.SetImage( &rMaskBitmap );
		rSourceDS.SetImage( this );

		// 1) Cut a hole in the destination drawing surface
		rDS.BlitDrawingSurface( rMaskDS, rSrcRect, rDestRect, kBlitNotSourceAnd );
		
		// 2) Isolate this image using the mask
		rSourceDS.BlitDrawingSurface( rMaskDS, rSrcRect, rSrcRect, kBlitSourceAnd );
				
		// 3) Copy the isolated image to the output of this drawing surface
		rDS.BlitDrawingSurface( rSourceDS, rSrcRect, rDestRect, kBlitSourcePaint );		

		rMaskDS.ReleaseImage();
		rSourceDS.ReleaseImage();
	}
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
void RBitmapImage::Write( RArchive& rArchive ) const
{
	UnimplementedCode();
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
void RBitmapImage::Read( RArchive& rArchive )
{
	UnimplementedCode();
}



// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetWidthInInches()
//
//  Description:		Returns the width of the bitmap, in inches
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
uLONG RBitmapImage::GetIndexOfColor( const RSolidColor& transparentColor ) const
{
	UnimplementedCode( );
	return 0;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetWidthInInches()
//
//  Description:		Returns the width of the bitmap, in inches
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//YRealDimension RBitmapImage::GetWidthInInches( ) const
//{
//	return ::PixelsToInches( GetWidthInPixels(), GetXDpi() );
//}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetHeightInInches()
//
//  Description:		Returns the height of the bitmap, in inches
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
//YRealDimension RBitmapImage::GetHeightInInches( ) const
//{
//	return ::PixelsToInches( GetHeightInPixels(), GetYDpi() );
//}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetWidthInPixels()
//
//  Description:		Returns the width of the bitmap, in pixels
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
uLONG RBitmapImage::GetWidthInPixels( ) const
{
TRECT		trc;
	if ( m_hBitmap != NULL )
	{
		::tbitGetRect( m_hBitmap, &trc );
		return trcWidth( &trc );
	}
	return 0;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::GetHeightInPixels()
//
//  Description:		Returns the height of the bitmap, in pixels
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
uLONG RBitmapImage::GetHeightInPixels( ) const
{
TRECT		trc;
	if ( m_hBitmap != NULL )
	{
		::tbitGetRect( m_hBitmap, &trc );
		return trcHeight( &trc );
	}
	return 0;
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
	return GetImageData( m_hBitmap );
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
	return GetImageDataSize( m_hBitmap );
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
	return RRealSize(::InchesToLogicalUnits(static_cast<YRealDimension>(GetWidthInPixels()) / static_cast<YRealDimension>(kDefaultXDpi)),
		::InchesToLogicalUnits(static_cast<YRealDimension>(GetHeightInPixels()) / static_cast<YRealDimension>(kDefaultYDpi)));
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::Lock()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
LPVOID RBitmapImage::Lock( )
{
LPVOID		pDevice = NULL;

	if ( m_hBitmap != NULL )
		if ( ::tbitLock( m_hBitmap ) )
			::tbitGetDevice( m_hBitmap, &pDevice);
	return pDevice;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::Unlock()
//
//  Description:		Free the bitmap from the device
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RBitmapImage::Unlock( )
{
	if ( m_hBitmap != NULL )
		::tbitUnlock( m_hBitmap );
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
void RBitmapImage::Copy( RDataTransferTarget& rDataTarget, YDataFormat yDataFormat ) const
{	
	UnimplementedCode();
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
void RBitmapImage::Paste( const RDataTransferSource& rDataSource, YDataFormat yDataFormat )
{	
	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::SkipZeroBits()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YIntCoordinate RBitmapImage::SkipZeroBits( uBYTE* pBytes, YIntCoordinate xLeft, YIntCoordinate xRight ) const
{
	//
	// Look at the bits in the first byte...
	pBytes			+= ( xLeft >> 3 );
	uBYTE	ubMask	= uBYTE( 1 << ( 7 - ( xLeft & 0x07 ) ) );
	uBYTE	ubData	= *pBytes;
	while( ubMask && xLeft < xRight )
	{
		if( ubData & ubMask )
			return xLeft;
		++xLeft;
		ubMask >>= 1;
	}
	//
	// Look at whole bytes...
	while( xLeft < xRight && !*++pBytes )
	{
		xLeft += 8;
	}
	//
	// Look at the bits in the last byte
	ubMask	= 0x80;
	ubData	= *pBytes;
	while( ubMask && xLeft < xRight )
	{
		if( ubData & ubMask )
			return xLeft;
		++xLeft;
		ubMask >>= 1;
	}
	return xRight;
}

// ****************************************************************************
//
//  Function Name:	RBitmapImage::SkipOneBits()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
YIntCoordinate RBitmapImage::SkipOneBits( uBYTE* pBytes, YIntCoordinate xLeft, YIntCoordinate xRight ) const
{
	//
	// Look at the bits in the first byte...
	pBytes			+= ( xLeft >> 3 );
	uBYTE	ubMask	= uBYTE( 1 << ( 7 - ( xLeft & 0x07 ) ) );
	uBYTE	ubData	= *pBytes;
	while( ubMask && xLeft < xRight )
	{
		if ( !(ubData & ubMask) )
			return xLeft;
		++xLeft;
		ubMask >>= 1;
	}
	//
	// Look at whole bytes...
	while( xLeft < xRight && (*++pBytes) == 0x0FF )
	{
		xLeft += 8;
	}
	//
	// Look at the bits in the last byte
	ubMask	= 0x80;
	ubData	= *pBytes;
	while( ubMask && xLeft < xRight )
	{
		if ( !(ubData & ubMask) )
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
void* RBitmapImage::GetImageData( void* pRawData )
{
HTBIT		hBitmap = static_cast<HTBIT>( pRawData ); 
	TpsAssert( pRawData, "pRawData = NULL." );
	return ::tbitGetBaseAddr( hBitmap );
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
uLONG RBitmapImage::GetImageDataSize( void* pRawData )
{
HTBIT		hBitmap = static_cast<HTBIT>( pRawData ); 
TRECT		trc;

	TpsAssert( pRawData, "pRawData = NULL." );
	::tbitGetRect( hBitmap, &trc );
	return GetImageDataSize( ( trc.right - trc.left ), ( trc.bottom - trc.top ), ::tbitGetDepth( hBitmap ) );
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
uLONG RBitmapImage::GetImageDataSize( uLONG uWidth, uLONG uHeight, uLONG uBitDepth )
{
	return( uHeight * GetBytesPerRow( uWidth, uBitDepth ) );
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
void* RBitmapImage::GetColorData( void* pRawData )
{
	TpsAssert( pRawData, "pRawData = NULL." );
	UnimplementedCode();
	return NULL;
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
uLONG RBitmapImage::GetColorDataSize( void* pRawData )
{	
	UnimplementedCode();
	return 0L;
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
uLONG RBitmapImage::GetColorDataSize( uLONG uBitDepth )
{
	UnimplementedCode();
	return 0;
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
uLONG RBitmapImage::GetBytesPerRow( uLONG uWidth, uLONG uBitDepth )
{	
	return ::tbitCalcRowBytes( uWidth, uBitDepth, kBitmapFlags );
}


