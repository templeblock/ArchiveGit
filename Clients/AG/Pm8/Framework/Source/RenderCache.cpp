// ****************************************************************************
//
//  File Name:			RenderCache.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RRenderCache class
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/RenderCache.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "RenderCache.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "OffscreenDrawingSurface.h"
#include "ScratchBitmapImage.h"
#include "ApplicationGlobals.h"

#if 1
const YTickCount kDefaultRenderThreshold = 333;	//	1/3 second
#else
const YTickCount kDefaultRenderThreshold = 0;
#endif

const int kMaxOffscreenSize = 1024 * 768;

//		Uncomment to disable the render cache
//#define NO_RENDERCACHEENABLE	1	 
#define	RENDERCACHE_INITIALLY_ON	1
// ****************************************************************************
//
// Function Name:		RRenderCache::RRenderCache
//
// Description:		Constructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RRenderCache::RRenderCache( )
#ifdef RENDERCACHE_INITIALLY_ON // REVIEW GCB 3/6/98
	: m_fEnabled( TRUE ),
#else
	: m_fEnabled( FALSE ),
#endif
	  m_fValid( FALSE ),
	  m_pBitmap( NULL ),
	  m_pMaskBitmap( NULL ),
	  m_LastRenderTime( 0 ),
	  m_RenderThreshold( kDefaultRenderThreshold )
	{
	;
	}

// ****************************************************************************
//
// Function Name:		RRenderCache::~RRenderCache
//
// Description:		Destructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RRenderCache::~RRenderCache( )
	{
	DeleteOffscreens( );
	}

// ****************************************************************************
//
// Function Name:		RRenderCache::DeleteOffscreens
//
// Description:		Deletes the cached offscreen and mask
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RRenderCache::DeleteOffscreens( )
	{
	if (m_pBitmap != NULL)
		{
		delete m_pBitmap;
		m_pBitmap = NULL;
		}

	if (m_pMaskBitmap)
		{
		delete m_pMaskBitmap;
		m_pMaskBitmap = NULL;
		}

	m_LastRenderTime = 0;
	}

// ****************************************************************************
//
// Function Name:		RRenderCache::Render( )
//
// Description:		Renders the cached object, using the cache if it is valid
//							and enabled.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RRenderCache::Render( RDrawingSurface& drawingSurface,
								   const RRealSize& size,
								   const R2dTransform& transform,
									const RIntRect& rcRender )
	{
#ifndef	IMAGER
	// If we are not printing, and the cache is enabled, (and we are using offscreens) use it
	if( drawingSurface.IsPrinting( ) == FALSE && m_fEnabled && ::GetUseOffscreens( ) )
		{
		// Check if we have an offscreen
		if( m_pBitmap )
			{
			// If the cache is still valid, render with it
			if( IsCacheValid( drawingSurface, size, transform ) )
				{
				RenderCache( drawingSurface, size, transform );
				return;
				}

			// The cache is invalid, delete it
			else
				DeleteOffscreens( );
			}

		// We dont have an offscreen. If we exceeded the render threshold or we
		// haven't yet performed a render, create and render a cached bitmap. -RIP-
		if( (m_LastRenderTime == 0) || (m_LastRenderTime >= m_RenderThreshold) )
			{
			// Create the offscreens
			CreateOffscreens( drawingSurface, size, transform );

			// If the create succeeded, render the cache
			if( m_pBitmap )
				RenderCache( drawingSurface, size, transform );

			// Otherwise go direct to screen
			else
				{
				RenderData( drawingSurface, size, transform, rcRender );
				}

			// -RIP- Verify that we needed a render cache.  If not, throw it away
			// to save the memory.  This is OK since it won't be regenerated the
			// next time we render anyway.
			if (m_LastRenderTime < m_RenderThreshold)
				{
				DeleteOffscreens();
				}
			}
		else
			{
			// We didnt exceed the render threshold, time another render
			YTickCount startTime = ::RenaissanceGetTickCount( );
			RenderData( drawingSurface, size, transform, rcRender );
			m_LastRenderTime = ::RenaissanceGetTickCount( ) - startTime;
			}
		}

	// If we are printing or the cache is not enabled, just render normally
	else
#endif	//	IMAGER
		RenderData( drawingSurface, size, transform, rcRender );
	}

// ****************************************************************************
//
// Function Name:		RRenderCache::IsCacheValid( )
//
// Description:		Determines if the current cache is valid
//
// Returns:				TRUE if the cache is valid
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RRenderCache::IsCacheValid( RDrawingSurface& drawingSurface, const RRealSize& size, const R2dTransform& transform ) const
	{
	// Check the valid flag
	if( !m_fValid )
		return FALSE;

	// Check the tint
	if( drawingSurface.GetTint( ) != m_LastTint )
		return FALSE;

	// Check the size
	if( size != m_LastSize )
		return FALSE;

	// Check the scale and rotate factors of the transform
	return transform.AreScaleAndRotateEqual( m_LastTransform );
	}

// ****************************************************************************
//
// Function Name:		RRenderCache::RenderCache( )
//
// Description:		Renders the cached offscreen
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RRenderCache::RenderCache( RDrawingSurface& drawingSurface,
										  const RRealSize& size,
										  const R2dTransform& transform ) const
	{
	// Get the size of the bitmap
	RIntVectorRect boundingRect( static_cast<RIntSize>( size ) );
	boundingRect *= transform;
	RIntSize bitmapSize = boundingRect.m_TransformedBoundingRect.WidthHeight( );

	// Get the position of the data
	RIntPoint topLeft = RIntPoint( boundingRect.m_TransformedBoundingRect.m_Left, boundingRect.m_TransformedBoundingRect.m_Top );

	// Set the monochrome colors
	drawingSurface.SetForegroundColor( kBlack );
	drawingSurface.SetBackgroundColor( kWhite );

	// Blit the offscreen
	m_pBitmap->RenderWithMask( drawingSurface, *m_pMaskBitmap, RIntRect( topLeft, bitmapSize ) );
	}

// ****************************************************************************
//
// Function Name:		RRenderCache::CreateOffscreen( )
//
// Description:		Creates the cached offscreen bitmap and mask.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RRenderCache::CreateOffscreens( RDrawingSurface& referenceSurface,
												 const RRealSize& size,
												 const R2dTransform& transform )
	{
	try
		{
		// Get the bounding rect of the bitmap
		RIntVectorRect boundingRect( static_cast<RIntSize>( size ) );
		boundingRect *= transform;
		RIntSize bitmapSize = boundingRect.m_TransformedBoundingRect.WidthHeight( );
		RIntRect bitmapRect( bitmapSize );

		// If the bitmap is larger than our max size, bail
		if( bitmapSize.m_dx * bitmapSize.m_dy > kMaxOffscreenSize )
			return;

		// Make a new transform that will position the data correctly in the bitmap
		R2dTransform tempTransform = transform;
		tempTransform.PostTranslate( -boundingRect.m_TransformedBoundingRect.m_Left, -boundingRect.m_TransformedBoundingRect.m_Top );

		// Create the normal bitmap
#if 0
#ifdef USE_SCRATCHBITMAPIMAGE // REVIEW GCB 3/6/98 - this may need to change to an RBitmapImage
		m_pBitmap = new RScratchBitmapImage;
		m_pBitmap->Initialize( referenceSurface, bitmapSize.m_dx, bitmapSize.m_dy );
#else
		m_pBitmap = new RBitmapImage;
		m_pBitmap->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 24 );
#endif
#else
		RBitmapImageBase * pbmMainImage = AllocateCacheBitmap( referenceSurface, bitmapSize.m_dx, bitmapSize.m_dy );
		SetCacheBitmap( pbmMainImage );
#endif
		ROffscreenDrawingSurface offscreenDrawingSurface;
		offscreenDrawingSurface.SetImage( m_pBitmap );
		offscreenDrawingSurface.CopyAttributes( referenceSurface );
		// RIP - Fill with black initially to avoid the need for an extra black
		// background image to generate the mask bitmap.
		offscreenDrawingSurface.SetFillColor( RSolidColor( kBlack ) );
		offscreenDrawingSurface.FillRectangle( bitmapRect );
		// Render in the component that we are caching.
		// Time it for the next time we need to perform a full render.
		YTickCount startTime = ::RenaissanceGetTickCount( );
		RenderData( offscreenDrawingSurface, size, tempTransform, bitmapRect );
		m_LastRenderTime = ::RenaissanceGetTickCount( ) - startTime;
//		offscreenDrawingSurface.ReleaseImage( );	// RIP - Released below.

		// Create a white background version of the image as well.
//		RScratchBitmapImage blackMask;
//		blackMask.Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 1);
#if 0
#ifdef USE_SCRATCHBITMAPIMAGE // REVIEW GCB 3/6/98 - this may need to change to an RBitmapImage
		RScratchBitmapImage whiteMask;
		whiteMask.Initialize( referenceSurface, bitmapSize.m_dx, bitmapSize.m_dy );
#else
		RBitmapImage whiteMask;
		whiteMask.Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 24);
#endif
#else
		RBitmapImageBase * pWhiteBitmap = AllocateCacheBitmap( referenceSurface, bitmapSize.m_dx, bitmapSize.m_dy );
#endif

		// Select the mask bitmaps into offscreen drawing surfaces
		ROffscreenDrawingSurface whiteOffscreen;
		whiteOffscreen.SetImage( pWhiteBitmap );	

		// Fill the backgrounds of the masks
		whiteOffscreen.SetFillColor( RSolidColor( kWhite ) );
		whiteOffscreen.FillRectangle( bitmapRect );

		// Render the data into the masks
		RenderData( whiteOffscreen, size, tempTransform, bitmapRect );

		// Combine the 2 masks into 1 to ensure that pure black and pure  white lines are not lost
#if 0
#ifdef USE_SCRATCHBITMAPIMAGE // MDH 3/9/98 - This bitmap should be the same type as the m_pBitmap
		m_pMaskBitmap = new RScratchBitmapImage;
		m_pMaskBitmap->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 1 );
#else
		m_pMaskBitmap = new RBitmapImage;
		m_pMaskBitmap->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 1 );
#endif
#else
		RBitmapImageBase * pbmMask = AllocateCacheMask( bitmapSize.m_dx, bitmapSize.m_dy );
		SetCacheMask( pbmMask );
#endif

		ROffscreenDrawingSurface maskOffscreen;
		maskOffscreen.SetImage( m_pMaskBitmap );
		maskOffscreen.BlitDrawingSurface( offscreenDrawingSurface, bitmapRect, bitmapRect, kBlitSourceCopy );
		maskOffscreen.BlitDrawingSurface( whiteOffscreen, bitmapRect, bitmapRect, kBlitMergePaint );

		// Release the images
		offscreenDrawingSurface.ReleaseImage( );
		whiteOffscreen.ReleaseImage( );
		maskOffscreen.ReleaseImage( );

		// Save the transform and size used to create the bitmaps
		m_LastTransform = transform;
		m_LastSize = size;

		// Save the tint
		m_LastTint = referenceSurface.GetTint( );

		// We are now valid
		m_fValid = TRUE;

		// Clean up our temporary bitmap
		if (pWhiteBitmap)
			{
			delete pWhiteBitmap;
			}
		}

	catch( ... )
		{
		DeleteOffscreens( );
		}
	}

// ****************************************************************************
//
// Function Name:		RRenderCache::Enable( )
//
// Description:		Enables or disable this cache
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//

// unreferenced parameters
#pragma warning( disable : 4100 )

void RRenderCache::Enable( BOOLEAN fEnable )
	{
// REVIEW GCB 3/6/98
#ifndef	NO_RENDERCACHEENABLE
	m_fEnabled = fEnable;
#endif
	}

// ****************************************************************************
//
// Function Name:		RRenderCache::Invalidate( )
//
// Description:		Invalidates this cache
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RRenderCache::Invalidate( )
	{
	m_fValid = FALSE;
	DeleteOffscreens( );
	}

// ****************************************************************************
//
// Function Name:		RRenderCache::SetRenderThreshold( )
//
// Description:		Sets the render threshold for this cache. The cache will
//							not be used unless an uncached render exceeds the render
//							threshold.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RRenderCache::SetRenderThreshold( YTickCount renderThreshold )
	{
	TpsAssert( renderThreshold == 0, "Setting render threshold to non-zero value - soft effects will not be available!" );
	m_RenderThreshold = renderThreshold;
	}


// ****************************************************************************
//
// Function Name:		RRenderCache::AllocateCacheBitmap( )
//
// Description:		Allocates and initializes the bitmaps used for the cache.
//
// Returns:				A pointer to the cacheable bitmap allocated.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RBitmapImageBase * RRenderCache::AllocateCacheBitmap( RDrawingSurface& referenceSurface, uLONG uWidthInPixels, uLONG uHeightInPixels )
{
#ifdef USE_SCRATCHBITMAPIMAGE
	RScratchBitmapImage *pBitmap;
	pBitmap = new RScratchBitmapImage;
	if (pBitmap != NULL)
	{
		pBitmap->Initialize( referenceSurface, uWidthInPixels, uHeightInPixels );
//		SetCacheBitmap( pBitmap );
	}
#else
	RBitmapImage *pBitmap;
	pBitmap = new RBitmapImage;
	if (pBitmap != NULL)
	{
		pBitmap->Initialize( uWidthInPixels, uHeightInPixels, 24 );
//		SetCacheBitmap( pBitmap );
	}
#endif

	return pBitmap;
}

// ****************************************************************************
//
// Function Name:		RRenderCache::AllocateCacheMask( )
//
// Description:		Allocates and initializes the bitmaps used for the cache masks.
//
// Returns:				A pointer to the cacheable mask bitmap allocated.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RBitmapImageBase * RRenderCache::AllocateCacheMask( uLONG uWidthInPixels, uLONG uHeightInPixels )
{
#ifdef USE_SCRATCHBITMAPIMAGE
	RScratchBitmapImage *pMask;
	pMask = new RScratchBitmapImage;
#else
	RBitmapImage *pMask;
	pMask = new RBitmapImage;
#endif
	if (pMask != NULL)
	{
		pMask->Initialize( uWidthInPixels, uHeightInPixels, 1 );
//		SetCacheMask( pMask );
	}

	return pMask;
}

// ****************************************************************************
//
// Function Name:		RRenderCache::SetCacheBitmap( )
//
// Description:		Sets the stored cached bitmap to the one specified.
//
// Returns:				None
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RRenderCache::SetCacheBitmap( RBitmapImageBase *pBitmap )
{
	if (m_pBitmap)
	{
		delete m_pBitmap;
	}
	m_pBitmap = pBitmap;
}

// ****************************************************************************
//
// Function Name:		RRenderCache::SetCacheMask( )
//
// Description:		Sets teh stored cache mask to the one specified.
//
// Returns:				None
//
// Exceptions:			Memory
//
// ****************************************************************************
//
void RRenderCache::SetCacheMask( RBitmapImageBase *pMask )
{
	if (m_pMaskBitmap)
	{
		delete m_pMaskBitmap;
	}
	m_pMaskBitmap = pMask;
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RRenderCache::Validate( )
//
// Description:		Validates the object
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RRenderCache::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RRenderCache, this );
	}

#endif		// TPSDEBUG
