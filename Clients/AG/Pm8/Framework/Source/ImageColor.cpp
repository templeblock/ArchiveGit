//****************************************************************************
//
// File Name:   ImageColor.cpp
//
// Project:     Renaissance user interface
//
// Author:      Lance Wilson
//
// Description: Implementation of the IImageColorData, RImageColorData,
//              and RImageColor objects
//
// Portability: Platform Independent
//
// Developed by: Broderbund Software
//				     500 Redwood Blvd.
//				     Novato, CA 94948
//			        (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/ImageColor.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#include "FrameworkIncludes.h"
ASSERTNAME

#include "ImageColor.h"
#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "OffscreenDrawingSurface.h"
#include "MemoryMappedBuffer.h"
#include "BufferStream.h"
#include "ImageLibrary.h"
#include "BitmapImage.h"
#include "VectorImage.h"

const uLONG kDefaultTextureXDPI = 100;
const uLONG kDefaultTextureYDPI = 100;

const uLONG kDefaultPhotoXDPI = 200;
const uLONG kDefaultPhotoYDPI = 200;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// RImageColorData Implementation
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//
// Function Name:  RImageColorData
//
// Description:    Constructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
RImageColorData::RImageColorData() :
	m_pImage( NULL ),
	m_nRefCount( 0 ),
	m_nLastTint( kMaxTint )
{
}

//*****************************************************************************
//
// Function Name:  ~RImageColorData
//
// Description:    Destructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
RImageColorData::~RImageColorData()
{
	if (m_pImage)
		delete m_pImage;
}

// ****************************************************************************
//
//  Function Name:	RImageColorData::AddRef( )
//
//  Description:		Increments reference count
//
//  Returns:			reference count
//
//  Exceptions:		None
//
// ****************************************************************************
//
uLONG	RImageColorData::AddRef( void )
{
	RLock lock( m_CriticalSection );
	return ++m_nRefCount;
}

// ****************************************************************************
//
//  Function Name:	RImageColorData::Release( )
//
//  Description:		Decrements reference count, deleting the object
//                   when it reaches 0;
//
//  Returns:			reference count
//
//  Exceptions:		None
//
// ****************************************************************************
//
uLONG	RImageColorData::Release( void )
{
	TpsAssert( m_nRefCount > 0, "Release called to many times!" );

	m_CriticalSection.Lock();
	uLONG nRefCount = --m_nRefCount;
	m_CriticalSection.Unlock();

	if (nRefCount == 0L)
		delete this;

	return nRefCount;
}

// ****************************************************************************
//
//  Function Name:	RImageColorData::QueryInterface( )
//
//  Description:		if the interface is supported, creates/assigns 
//                   the interface specified by the interface id into 
//                   ppvObject; 
//
//  Returns:			TRUE if successful; otherwise FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageColorData::QueryInterface( YInterfaceId id, void ** ppvObject )
{
	m_CriticalSection.Lock();

	*ppvObject = NULL;

	if (kUnknownInterfaceId == id)
		*ppvObject = (RIUnknown *) this;
	
	else if (kImageColorDataInterface == id)
		*ppvObject = (IImageColorData *) this;

	else
		return FALSE;

	m_CriticalSection.Unlock();
	((RIUnknown *) *ppvObject)->AddRef();

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RImageColorData::Initialize( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageColorData::Initialize( const RBitmapImage* pImage )
{
	TpsAssert( m_nRefCount, "Invalid Reference Count!" );
	TpsAssert( !m_pImage, "Object already Initialized!" );

	RLock lock( m_CriticalSection );
	m_pImage = new RBitmapImage( *pImage );

	RImageLibrary().ExportImage( *m_pImage, m_rInternalDataBuffer, kImageFormatBMP );
}

// ****************************************************************************
//
//  Function Name:	RImageColorData::Initialize( )
//
//  Description:		
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageColorData::Initialize( RBuffer& rBuffer )
{
	TpsAssert( m_nRefCount, "Invalid Reference Count!" );
	TpsAssert( !m_pImage, "Object already Initialized!" );

	RLock lock( m_CriticalSection );
	m_rInternalDataBuffer.Resize( rBuffer.GetBufferSize() );
	memcpy( m_rInternalDataBuffer.GetBuffer(), rBuffer.GetBuffer(), rBuffer.GetBufferSize() );

	m_pImage = static_cast<RBitmapImage*>( RImageLibrary().ImportImage( rBuffer ) );
}


// ****************************************************************************
//
//  Function Name:	RImageColorData::Read( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageColorData::Read( RArchive& archive )
{
	RLock lock( m_CriticalSection );

	archive >> m_rInternalDataBuffer;

	RImageLibrary rLibrary;
	EImageFormat eFormat = rLibrary.GetImageFormat( m_rInternalDataBuffer );

	if (kImageFormatUnknown == eFormat)
		throw rLibrary.GetLastException();
}	

// ****************************************************************************
//
//  Function Name:	RImageColorData::Write( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageColorData::Write( RArchive& archive ) const
{
	RLock lock( const_cast<RImageColorData*>( this )->m_CriticalSection );
	TpsAssert( m_rInternalDataBuffer.GetBufferSize(), "Cannot write uninitialized data!" );

	const_cast<RImageColorData*>( this )->CompressInternalData( );
	archive << m_rInternalDataBuffer;
}

// ****************************************************************************
//
//  Function Name:	RImageColorData::Render( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageColorData::Render( RDrawingSurface& ds, const RIntRect& rcBounds, const R2dTransform& transform )
{
	RLock lock( m_CriticalSection );
	TpsAssert( m_nRefCount, "Invalid Reference Count!" );
	TpsAssert( m_pImage, "Invalid image!" );

	YAngle angle;
	YRealDimension xScale, yScale;
	transform.Decompose( angle, xScale, yScale );

	RBitmapImage* pImage = m_pImage;

	// Determine the output rect size
	RRealSize rImageSize( GetSizeInLogicalUnits() );
	rImageSize.m_dx *= xScale; if (rImageSize.m_dy < 0) rImageSize.m_dy = -rImageSize.m_dy;
	rImageSize.m_dy *= yScale; if (rImageSize.m_dx < 0) rImageSize.m_dx = -rImageSize.m_dx;

	RIntRect rcDestRect( RIntSize( rImageSize.m_dx, rImageSize.m_dy ) );
	RIntRect rcTileRect( 0, 0, pImage->GetWidthInPixels() - 1, 
		pImage->GetHeightInPixels() - 1 );

	YRealDimension xOffset( 0.0 );

	if (FALSE) // Change to check for flag for center alignment
	{
		// Offset the destination rect so that the center 
		// point will be the center point of the tile as well.
		RRealPoint ptCenter = rcBounds.GetCenterPoint( );
		YRealDimension yOffset = ptCenter.m_y / rcDestRect.Height();
		xOffset = ptCenter.m_x / rcDestRect.Width();

		yOffset = 0.5 - (yOffset - int( yOffset ));
		xOffset = 0.5 - (xOffset - int( xOffset ));

		if (yOffset > 0) yOffset -= 1.0;
		if (xOffset > 0) xOffset -= 1.0;

		// Adjust the destination rect to account for centering
		rcDestRect.Offset( RIntSize( xOffset * rImageSize.m_dx, 
			yOffset * rImageSize.m_dy ) );
	}

	// Save the DS state
	RDrawingSurfaceState	curState( &ds );
	ds.IntersectClipRect( rcBounds );

	//
	// Tile the image
	//
	ROffscreenDrawingSurface dsMem;
	dsMem.SetImage( pImage );

	// Position the destination rect to the top left
	rcDestRect.Offset( RIntSize( rcBounds.m_Left, rcBounds.m_Top ) );

	while (rcDestRect.m_Top < rcBounds.m_Bottom)
	{
		while (rcDestRect.m_Left < rcBounds.m_Right)
		{
			ds.BlitDrawingSurface( dsMem, rcTileRect, rcDestRect, kBlitSourceCopy );
			rcDestRect.m_Left  += rImageSize.m_dx - 1; // Subtracting one cause there seems
			rcDestRect.m_Right += rImageSize.m_dx - 1; //  to be an off by one error somewhere.
		}

		rcDestRect.m_Top    += rImageSize.m_dy;
		rcDestRect.m_Bottom += rImageSize.m_dy;
		rcDestRect.m_Left    = rcBounds.m_Left + (xOffset * rImageSize.m_dx) ;
		rcDestRect.m_Right   = rcDestRect.m_Left + rImageSize.m_dx;
	}

	dsMem.ReleaseImage();

	if (pImage != m_pImage)
		delete pImage;				// We created a copy, so delete it.
}

//*****************************************************************************
//
// Function Name:  RImageColor::CompressInternalData
//
// Description:    Compresses the data into the internal data format.
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RImageColorData::CompressInternalData( )
{
	RImageLibrary rLibrary;
	EImageFormat eFormat = kImageFormatBMP;
	
	if (m_rInternalDataBuffer.GetBufferSize())
		eFormat = rLibrary.GetImageFormat( m_rInternalDataBuffer );

	if (eFormat != kImageFormatJPG && eFormat != kImageFormatPNG)
	{
		// Default to PNG
		eFormat = kImageFormatPNG;

		if (!m_pImage)
		{
			// Reimport the image
			m_pImage = (RBitmapImage *) RImageLibrary().ImportImage( m_rInternalDataBuffer );
		}

		uLONG ulBitDepth = m_pImage->GetBitDepth(); 

		if (ulBitDepth <= 8)
		{
			// Use PNG for 8-bit or less
			eFormat = kImageFormatPNG;
		}
		else if (ulBitDepth != 24)
		{
			// Accusoft does not support 16bpp or 32bpp images.  
			// If the image is a 16bpp or 32bpp bitmap, we need 
			// to convert it to 24bpp here
			rLibrary.ChangeBitDepth( *m_pImage, 24 );
		}

		// Export the image into the internal data buffer
		if (!rLibrary.ExportImage(*m_pImage, m_rInternalDataBuffer, eFormat)) 
			throw rLibrary.GetLastException();
	}
}

//*****************************************************************************
//
// Function Name:  RImageColor::GetDataLength
//
// Description:    Determines the size of the data to be serialized.
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
uLONG	RImageColorData::GetDataLength() const
{
	const_cast<RImageColorData*>( this )->CompressInternalData();

	return m_rInternalDataBuffer.GetBufferSize();
}

//*****************************************************************************
//
// Function Name:  RImageColor::GetSizeInLogicalUnits
//
// Description:    Applies the transform to the base transform
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
RRealSize RImageColorData::GetSizeInLogicalUnits()
{
	if (!m_pImage)
		m_pImage = (RBitmapImage *) RImageLibrary().ImportImage( m_rInternalDataBuffer );

	return RRealSize( 
		::InchesToLogicalUnits(static_cast<YRealDimension>(m_pImage->GetWidthInPixels()) / static_cast<YRealDimension>(kDefaultTextureXDPI)),
		::InchesToLogicalUnits(static_cast<YRealDimension>(m_pImage->GetHeightInPixels()) / static_cast<YRealDimension>(kDefaultTextureYDPI)) );
}

//*****************************************************************************
//
// Function Name:  RImageColor::GetImage
//
// Description:    Returns a proxy image, creating a new one
//                 if necessary.
//
// Returns:		    
//	 RBitmapImage*	 Proxy image if succesful; otherwise NULL.
//
// Exceptions:	    None
//
//*****************************************************************************
//
RBitmapImage* RImageColorData::GetImage( YTint tint )
{
	if (tint != m_nLastTint && m_pImage && kMaxTint != m_nLastTint)
	{
		// We need to reload the image data if we are going to a different tint value.
		// Note: this is not necessary when the current tint value is max tint.
		TpsAssert( m_rInternalDataBuffer.GetBufferSize(), "Invalid internal buffer!" );
	
		delete m_pImage;
		m_pImage = NULL;
	}

	if (!m_pImage)
	{
		// Reimport the image
		m_pImage = (RBitmapImage *) RImageLibrary().ImportImage( m_rInternalDataBuffer );
	}

	if (tint != m_nLastTint)
	{
		if (kMaxTint != tint)
		{
			// Tint the image
			RImageLibrary rLibrary;
			rLibrary.Tint( *m_pImage, tint );
		}

		m_nLastTint = tint;
	}


	return m_pImage;

/*	try
	{
		if (tint != m_nLastTint)
		{
			if (m_pProxyImage)
				delete m_pProxyImage;

			m_pProxyImage = new RBitmapImage( *m_pImage );

			RImageLibrary rLibrary;
			rLibrary.Tint( *m_pProxyImage, tint );

			m_nLastTint = tint;
		}

	}
	catch (...)
	{
		return m_pImage;
	}

	return m_pProxyImage;
*/
}

// ****************************************************************************
//
//  Function Name:	RImageColorData::CreateInstance( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageColorData::CreateInstance( YInterfaceId id, void ** ppvObject )
{
	RImageColorData* pObject = new RImageColorData();

	if (pObject && !pObject->QueryInterface( id, ppvObject ))
	{
		delete pObject;
		return FALSE;
	}

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
// RImageColor Implementation
///////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
//
// Function Name:  RImageColor
//
// Description:    Constructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
RImageColor::RImageColor( uWORD uwFlags /*= kCentered | kTiled*/ ) :
	m_ulID( (uLONG) -1L ),
	m_uwIndex( 0 ),
	m_uwFlags( uwFlags ),
	m_pImageData( NULL ),
	m_pProxyImage( NULL ),
	m_pProxyMask( NULL ),
	m_nTint( kMaxTint ),
	m_nLastAngle( 0.0 )
{
}

//*****************************************************************************
//
// Function Name:  RImageColor
//
// Description:    Constructor 
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
RImageColor::RImageColor( uLONG ulID, const RImage* pImage ) :
	m_ulID( ulID ),
	m_uwIndex( 0 ),
	m_uwFlags( kCentered | kTiled ),
	m_pImageData( NULL ),
	m_pProxyImage( NULL ),
	m_pProxyMask( NULL ),
	m_nTint( kMaxTint ),
	m_nLastAngle( 0.0 )
{
	RImageColorData::CreateInstance( kImageColorDataInterface, (void **) &m_pImageData );
	m_pImageData->Initialize( dynamic_cast<const RBitmapImage *>( pImage ) );
}

//*****************************************************************************
//
// Function Name:  RImageColor::Initialize
//
// Description:    Loads the image from storage
//
// Returns:		    
//	  BOOLEN	   TRUE if successful; otherwise FALSE
//
// Exceptions:	   None
//
//*****************************************************************************
//
BOOLEAN	RImageColor::Initialize( uLONG ulID, RBuffer& rBuffer )
{
	try
	{
		m_ulID = ulID;

		RImageColorData::CreateInstance( kImageColorDataInterface, (void **) &m_pImageData );
		m_pImageData->Initialize( rBuffer );
	}
	catch (YException)
	{
		return FALSE;		
	}

	return TRUE;
}

//*****************************************************************************
//
// Function Name:  RImageColor::Initialize
//
// Description:    Loads the image from storage
//
// Returns:		    
//	  BOOLEN	   TRUE if successful; otherwise FALSE
//
// Exceptions:	   None
//
//*****************************************************************************
//
BOOLEAN	RImageColor::Initialize( RMBCString rPath )
{
	RImportFileBuffer rImportBuffer( rPath );
	return Initialize( 0L, rImportBuffer );
}

//*****************************************************************************
//
// Function Name:  RImageColor::Initialize
//
// Description:    Loads the image from another image
//
// Returns:		    
//	  BOOLEN	   TRUE if successful; otherwise FALSE
//
// Exceptions:	   None
//
//*****************************************************************************
//
BOOLEAN	RImageColor::Initialize( const RBitmapImage* pImage )
{
	try
	{
		m_ulID = 0L;

		RImageColorData::CreateInstance( kImageColorDataInterface, (void **) &m_pImageData );
		m_pImageData->Initialize( pImage );
	}
	catch (YException)
	{
		return FALSE;		
	}

	return TRUE;
}

//*****************************************************************************
//
// Function Name:  RImageColor
//
// Description:    Constructor (Copy)
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
RImageColor::RImageColor( const RImageColor& rhs ) :
	m_pProxyImage( NULL ),
	m_pProxyMask( NULL ),
	m_nLastAngle( 0.0 )
{
	operator=( rhs );
}

//*****************************************************************************
//
// Function Name:  ~RImageColor
//
// Description:    Destructor (Default).
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
RImageColor::~RImageColor()
{
	if (m_pImageData)
		m_pImageData->Release();

	if (m_pProxyImage)
		delete m_pProxyImage;

	if (m_pProxyMask)
		delete m_pProxyMask;}

//*****************************************************************************
//
// Function Name:  RImageColor::operator=
//
// Description:    Assignment operator
//
// Returns:		    reference to self
//
// Exceptions:	    None
//
//*****************************************************************************
//
const RImageColor& RImageColor::operator=( const RImageColor& rhs )
{
	m_pImageData = rhs.m_pImageData;
	m_pImageData->AddRef();

	m_ulID     = rhs.m_ulID;
	m_uwIndex  = rhs.m_uwIndex;
	m_uwFlags  = rhs.m_uwFlags;
	m_nTint    = rhs.m_nTint;
	m_rcBounds = rhs.m_rcBounds;
	m_Transform = rhs.m_Transform;
	m_BaseTransform = rhs.m_BaseTransform;

	return *this;
}

//*****************************************************************************
//
// Function Name:  RImageColor::operator==
//
// Description:    Equality operator
//
// Returns:		    TRUE if items are equal; otherwise FALSE
//
// Exceptions:	    None
//
//*****************************************************************************
//
BOOLEAN RImageColor::operator==( const RImageColor& rhs ) const
{
	return BOOLEAN ( 
		m_ulID          == rhs.m_ulID    &&
		m_uwIndex       == rhs.m_uwIndex &&
		m_uwFlags       == rhs.m_uwFlags &&
		m_nTint         == rhs.m_nTint   &&
		m_BaseTransform == rhs.m_BaseTransform );
}

//*****************************************************************************
//
// Function Name:  RImageColor::operator!=
//
// Description:    Non-equality operator
//
// Returns:		    TRUE if items are NOT equal; otherwise FALSE
//
// Exceptions:	    None
//
//*****************************************************************************
//
BOOLEAN RImageColor::operator!=( const RImageColor& rhs ) const
{
	return BOOLEAN(!operator==( rhs ));
}

//*****************************************************************************
//
// Function Name:  RImageColor::Read
//
// Description:    
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RImageColor::Read( RArchive& archive )
{
	if (m_pImageData)
	{
		m_pImageData->Release();
		m_pImageData = NULL;
	}

	archive >> m_ulID;
	archive >> m_uwIndex;
	archive >> m_uwFlags;
	archive >> m_nTint;
	archive >> m_BaseTransform;

	// 01/11/99 Lance - The following code is a hack.  For some reason, there are some
	// color objects out there where the flags were uninitialized before serialization.  
	// Because there was only one flags configuration before this fix, we can assign
	// the correct value here.
	if (0xcdcd == m_uwFlags)
		m_uwFlags = kCentered | kTiled;

	RImageColorData::CreateInstance( kImageColorDataInterface, (void **) &m_pImageData );
	m_pImageData->Read( archive );

	uLONG ulPadding;
	archive >> ulPadding;
	archive >> ulPadding;
}

//*****************************************************************************
//
// Function Name:  RImageColor::Write
//
// Description:    
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RImageColor::Write( RArchive& archive ) const
{
	TpsAssert( m_pImageData, "Cannot write unitialized data!" );

	archive << m_ulID;
	archive << m_uwIndex;
	archive << m_uwFlags;
	archive << m_nTint;
	archive << m_BaseTransform;

	m_pImageData->Write( archive );

	uLONG ulPadding = 0L;
	archive << ulPadding;
	archive << ulPadding;

}

// ****************************************************************************
//
//  Function Name:	SetBoundingRect( )
//
//  Description:		Sets the fill bounding rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageColor::SetBoundingRect( const RIntRect& rcBounds )
{
	m_rcBounds = rcBounds;
}

// ****************************************************************************
//
//  Function Name:	Fill( )
//
//  Description:		Fill the given rectangle in the given drawing 
//                   surface with this bitmap
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageColor::Fill( RDrawingSurface& ds, const RIntRect& rcBounds, R2dTransform transform ) const
{
	TpsAssert( m_pImageData, "Invalid image data!" );

	// Intersect the bounds rect with the clip region
	RDrawingSurfaceState	curState( &ds );
	ds.IntersectClipRect( rcBounds );

	if (kTiled & m_uwFlags)
	{
		RenderTiled( ds, rcBounds, transform );
	}
	else
	{
		Render( ds, rcBounds, transform );
	}
}

void RImageColor::RenderTiled( RDrawingSurface& ds, const RIntRect& rcBounds, R2dTransform transform ) const
{

	// We need the angle from the combination of all matrices
	R2dTransform xform = m_BaseTransform * m_Transform * transform;

	YAngle angle;
	YRealDimension xScale, yScale;
	xform.Decompose( angle, xScale, yScale );

	// Now build the transform used for the rendering proces
	xform = m_Transform * transform;
	xform.PostTranslate( rcBounds.m_Left, rcBounds.m_Top );

	// Determine the starting rect in logical units
	//
	RRealSize rImageSize( m_pImageData->GetSizeInLogicalUnits() );
	rImageSize *= m_BaseTransform;

	if (rImageSize.m_dx < 0)
		rImageSize.m_dx = -rImageSize.m_dx;
	if (rImageSize.m_dy < 0)
		rImageSize.m_dy = -rImageSize.m_dy;

	RRealRect rcSrcRect( 
		m_rcBounds.m_Left, 
		m_rcBounds.m_Top,
		m_rcBounds.m_Left + rImageSize.m_dx,
		m_rcBounds.m_Top  + rImageSize.m_dy );

	RRealSize rOffset( 0.0, 0.0 );

	if (m_uwFlags & kCentered)
	{
		RRealSize offset;

		if (rImageSize.m_dx < m_rcBounds.Width())
			offset.m_dx	= (fmod( m_rcBounds.Width(), rImageSize.m_dx ) ) / 2.0 - rImageSize.m_dx;
		else
			offset.m_dx = m_rcBounds.Width() / 2 - rImageSize.m_dx / 2;

		if (rImageSize.m_dy < m_rcBounds.Height())
			offset.m_dy	= (fmod( m_rcBounds.Height(), rImageSize.m_dy ) ) / 2.0 - rImageSize.m_dy;
		else
			offset.m_dy = m_rcBounds.Height() / 2 - rImageSize.m_dy / 2;

		rcSrcRect.Offset( offset );
	}

	// Render
	//

	// Remove logical to device scaling from scale factors.
	RRealSize scale( 
		xScale * kSystemDPI / kDefaultTextureXDPI,
		yScale * kSystemDPI / kDefaultTextureXDPI );

	RBitmapImage* pSrcImage = const_cast<RImageColor *>( this )->GetProxy( 
		ds, angle, scale.m_dx, scale.m_dy );

	RIntRect rcTileRect( 0, 0, 
		pSrcImage->GetWidthInPixels(), 
		pSrcImage->GetHeightInPixels() );

	// Determine the position of the first tile in device units
	RRealVectorRect rcOutRect( rcSrcRect );
	rcOutRect *= xform;

	RIntSize szTileCount( rcOutRect.WidthHeight() );

	RIntRect rcStartTile( rcTileRect );
	rcStartTile.Offset( RIntSize(
		rcOutRect.m_TransformedBoundingRect.m_Left,
		rcOutRect.m_TransformedBoundingRect.m_Top ) );

	rcOutRect = m_rcBounds;
	rcOutRect *= xform;

	// Added two to the tile count, 1 for rounding errors, and one to
	// help for the case when there is skewing due to limited resoulution.
	szTileCount.m_dx = rcOutRect.Width()  / szTileCount.m_dx + 2;
	szTileCount.m_dy = rcOutRect.Height() / szTileCount.m_dy + 2;

	for (int i = 0; i <= szTileCount.m_dy; i++)
	{
		RIntRect rcDestRect( rcStartTile );

		for (int j = 0; j <= szTileCount.m_dx; j++)
		{
			if (rcBounds.IsIntersecting( rcDestRect ))
			{
				if (angle < 0.001 && angle > -0.001)
					pSrcImage->Render( ds, rcDestRect );
				else
					pSrcImage->RenderWithMask( ds, *m_pProxyMask, rcDestRect );
			}

			rcDestRect.Offset( m_ptXOffset );
		}

		rcStartTile.Offset( m_ptYOffset );
	}
}

void RImageColor::Render( RDrawingSurface& ds, const RIntRect& rcBounds, R2dTransform transform ) const
{
	// We need the angle from the combination of all matrices
	R2dTransform xform = m_BaseTransform * m_Transform * transform;

	YAngle angle;
	YRealDimension xScale, yScale;
	xform.Decompose( angle, xScale, yScale );

	// Now build the transform used for the rendering process
	xform = m_Transform * transform;
//	xform.PostTranslate( rcBounds.m_Left, rcBounds.m_Top );

	RRealSize rImageSize( m_pImageData->GetSizeInLogicalUnits() );
	rImageSize.m_dx *= 0.5; rImageSize.m_dy *= 0.5;  // Photo fills are twice the dpi

	RRealSize rAspectScalar( 
		(float) m_rcBounds.Width()  / rImageSize.m_dx, 
		(float) m_rcBounds.Height() / rImageSize.m_dy ); 

	rAspectScalar.m_dx = Max( rAspectScalar.m_dx, rAspectScalar.m_dy );
	rAspectScalar.m_dy = rAspectScalar.m_dx;
	rImageSize.Scale( rAspectScalar );

	// Determine the destination rect by passing the 
	// scaled image through the transformation matrix
	RRealVectorRect xlatedDestRect( rImageSize );
	xlatedDestRect *= xform;
	RIntRect rcDestRect( xlatedDestRect.m_TransformedBoundingRect );

	// Add aspect correct and remove logical to device scaling from scale factors.
	xScale *= rAspectScalar.m_dx * kSystemDPI / kDefaultPhotoXDPI;
	yScale *= rAspectScalar.m_dy * kSystemDPI / kDefaultPhotoYDPI;

	if (xScale > 2.0)
	{	
		yScale = 2.0 * yScale / xScale;
		xScale = 2.0;
	}

	if (yScale > 2.0)
	{
		xScale = 2.0 * xScale / yScale;
		yScale = 2.0;
	}

	RBitmapImage* pSrcImage = const_cast<RImageColor *>( this )->GetProxy( 
		ds, angle, xScale, yScale );

	RIntVectorRect xlatedBounds( m_rcBounds );
	xlatedBounds *= xform;
	RIntRect rcOutRect( xlatedBounds.m_TransformedBoundingRect );

	if (m_uwFlags & kCentered)
		rcDestRect.CenterRectInRect( rcOutRect );
	else
		rcDestRect.Offset( rcOutRect.m_TopLeft - rcDestRect.m_TopLeft );

	// We need to determine the rcBounds passed in, in relation
	// to our dest rect.  To do this, we will determine the offset
	// to be used to convert the rcBounds into dest rect space.
	RIntSize offset( -rcDestRect.m_Left, -rcDestRect.m_Top );

	if ( rcDestRect.m_Top > rcDestRect.m_Bottom) // Handle bottom-up images
		offset.m_dy = -rcDestRect.m_Bottom;

	// Deteremine rcBounds placement (in 
	// percentages) within our destination rect.
	RRealRect rSrcValues( 
		(float) (rcBounds.m_Left   + offset.m_dx) / Abs( rcDestRect.Width() ),
		(float) (rcBounds.m_Top    + offset.m_dy)  / Abs( rcDestRect.Height() ),
		(float) (rcBounds.m_Right  + offset.m_dx) / Abs( rcDestRect.Width() ),
		(float) (rcBounds.m_Bottom + offset.m_dy)  / Abs( rcDestRect.Height() ) );

	// Convert source values into
	// an actual sourece rect.
	RIntRect rcSrcRect(
		::Round( pSrcImage->GetWidthInPixels()  * rSrcValues.m_Left ),
		::Round( pSrcImage->GetHeightInPixels() * rSrcValues.m_Top ),
		::Round( pSrcImage->GetWidthInPixels()  * rSrcValues.m_Right ),
		::Round( pSrcImage->GetHeightInPixels() * rSrcValues.m_Bottom ) );


	if (!AreFloatsEqual( angle, 0.0 ))
		pSrcImage->Render( ds, rcSrcRect, rcBounds );
	else
		pSrcImage->RenderWithMask( ds, *m_pProxyMask, rcSrcRect, rcBounds );
}

//*****************************************************************************
//
// Function Name:  RImageColor::ApplyTransform
//
// Description:    Applies the transform to the base transform
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RImageColor::ApplyTransform( const R2dTransform& transform )
{
	m_Transform = /*m_BaseTransform */ transform;
}

//*****************************************************************************
//
// Function Name:  RImageColor::GetTint
//
// Description:    Returns the colors current tint
//
// Returns:		    ^
//
// Exceptions:	    None
//
//*****************************************************************************
//
YTint	RImageColor::GetTint( ) const
{
	return m_nTint;
}

//*****************************************************************************
//
// Function Name:  RImageColor::SetTint
//
// Description:    Sets the colors current tint
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RImageColor::SetTint( YTint tint )
{
	m_nTint = tint;
}

//*****************************************************************************
//
// Function Name:  RImageColor::GetFlags
//
// Description:    Returns the color's flags
//
// Returns:		    ^
//
// Exceptions:	    None
//
//*****************************************************************************
//
uWORD	RImageColor::GetFlags() const
{
	return m_uwFlags;
}

//*****************************************************************************
//
// Function Name:  RImageColor::SetTint
//
// Description:    Sets the color's flags
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
//
void RImageColor::SetFlags( uWORD wFlags )
{
	m_uwFlags = wFlags;
}


//*****************************************************************************
//
// Function Name:  RImageColor::GetImage
//
// Description:    Returns a pointer to the image data
//
// Returns:		    
//	 RBitmapImage*	 image if succesful; otherwise NULL.
//
// Exceptions:	    None
//
//*****************************************************************************
//
const RBitmapImage* RImageColor::GetImage( )
{
	if (m_pImageData)
		return m_pImageData->GetImage();

	return NULL;
}

//*****************************************************************************
//
// Function Name:  RImageColor::GetProxy
//
// Description:    Returns the proxy image, creating a new one
//                 if necessary.
//
// Returns:		    
//	 RBitmapImage*	 Proxy image if succesful; otherwise NULL.
//
// Exceptions:	    None
//
//*****************************************************************************
//
RBitmapImage* RImageColor::GetProxy( RDrawingSurface& ds, YAngle angle, YRealDimension xScale, YRealDimension yScale )
{
	try
	{
		YAngle baseAngle;
		YRealDimension baseXScale, baseYScale;
		m_BaseTransform.Decompose( baseAngle, baseXScale, baseYScale );

//		if (baseXScale < 0)
//			angle = -angle;

		if (!m_pProxyImage || angle != m_nLastAngle)
		{
			if (!m_pProxyImage)
				m_pProxyImage = new RBitmapImage;

			if (!m_pProxyMask)
				m_pProxyMask = new RBitmapImage;

			YPercentage flTint = ds.GetTint() / 100.0;
			RBitmapImage* pSrcImage = m_pImageData->GetImage( m_nTint * flTint );

//			RImageLibrary rLibrary;
//			rLibrary.Rotate( *pSrcImage, *m_pProxyImage, *m_pProxyMask, angle );

			RImageLibrary().RCTransform( *pSrcImage, *m_pProxyImage, *m_pProxyMask, 
				angle, xScale, yScale, &m_ptXOffset, &m_ptYOffset );

			m_nLastAngle  = angle;

			// Adjust offsets for differnet coordinate space 
			// (Y positive going down )
			m_ptXOffset.m_dy = -m_ptXOffset.m_dy;
			m_ptYOffset.m_dy = -m_ptYOffset.m_dy;

			if (baseXScale < 0)
			{
				m_ptYOffset.m_dx = -m_ptYOffset.m_dx;
				m_ptYOffset.m_dy = -m_ptYOffset.m_dy;
			}
		}
	}
	catch (...)
	{
		return NULL;
	}

	return m_pProxyImage;
}

//*****************************************************************************
//
// Function Name:  RImageColor::GetProxy
//
// Description:    Determines the amount of space the object will
//                 take when serialized.
//
// Returns:		    
//	 uLONG          data size
//
// Exceptions:	    None
//
//*****************************************************************************
//
uLONG	RImageColor::GetDataLength()
{
	ULONG ulSize = 
		sizeof( m_ulID )    +
		sizeof( m_uwIndex ) +
		sizeof( m_uwFlags ) +
		sizeof( m_nTint )   +
		sizeof( m_BaseTransform );

	if (m_pImageData)
	{
		ulSize += m_pImageData->GetDataLength();
	}

	// Add padding in
	ulSize += sizeof(	uLONG ) * 2;

	// REVIEW 06/01/98 Lance - this uses some internal
	// knowledge of the buffer serializes, this should
	// probably be modified to be less dependant on
	// that information.
	//
	// When the image serializes, it writes out the
	// buffer size first, so we need to account for
	// that here.
	ulSize += sizeof( uLONG );

	return ulSize;
}

