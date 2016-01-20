// *****************************************************************************
//
// File name:		FixFlaw.cpp
//
// Project:			Framework
//
// Author:			Bob Gotsch / John Fleischhauer
//
// Description:		FixFlaw declaration - class to encapsulate fix flaw features
//
// Portability:		Windows
//
// Copyright (C) 1998 Broderbund Software, Inc.  All Rights Reserved.
//
// *****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "BitmapImage.h"
#include "FixFlaw.h"
#include "OffscreenDrawingSurface.h"

// define the following to add soft edges on the mask for remove shine
#define SOFT_EDGES_ON_SHINE_MASK

// Windows color component order
enum
{
	kBlueComponent = 0,
	kGreenComponent,
	kRedComponent	
};	

static YFloatType GProximityTweakFactor = 0.9;
static YFloatType GRednessTweakFactor   = 1.0;
static YFloatType GWeightTweakFactor    = 0.5;
static sLONG GSquaredProximityDropoffProfile[] = {100, 95, 90, 83, 77, 69, 58, 47, 36, 23, 9};

// ****************************************************************************
//
//  Function Name:	RFixFlaw::RFixFlaw()
//
//  Description:	constructor
//
//  Returns:		
//
// ****************************************************************************
//
RFixFlaw::RFixFlaw( RBitmapImage& rBitmap, const RIntRect& rRect, RImageLibrary& rLibrary )
	: m_rBitmap( rBitmap ),
	  m_rRect( rRect ),
	  m_rLibrary( rLibrary )
{
}	

// ****************************************************************************
//
//  Function Name:	RFixFlaw::~RFixFlaw()
//
//  Description:	destructor
//
//  Returns:		
//
// ****************************************************************************
//
RFixFlaw::~RFixFlaw()
{
}	

// ****************************************************************************
//
//  Function Name:	RFixFlaw::RemoveEncircledChroma()
//
//  Description:	public method to remove red eye or pet eye
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RFixFlaw::RemoveEncircledChroma( const EDiscreteHue eHue )
{
	BOOLEAN bRet = FALSE;

	// get raw pixel data
	uBYTE *pPixelData = GetBitmapPixels( m_rBitmap, m_rRect );
	if ( pPixelData )
	{
		uLONG uBitmapWidthInBytes = RBitmapImage::GetBytesPerRow( m_rBitmap.GetWidthInPixels(), 24 );
		bRet = DoRemoveEncircledChroma ( pPixelData, m_rRect.Width(), uBitmapWidthInBytes, eHue );
	}

	return bRet;
}	

// ****************************************************************************
//
//  Function Name:	RFixFlaw::RemoveScratch()
//
//  Description:	public method to remove scratch / dust
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RFixFlaw::RemoveScratch()
{
	return DoMaskedBlur();

#ifdef TPSDEBUG
//	TestRectPosition();
//	return TRUE;
#endif
}	

// ****************************************************************************
//
//  Function Name:	RFixFlaw::RemoveShine()
//
//  Description:	public method to remove shine
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RFixFlaw::RemoveShine()
{
	BOOLEAN bRet = FALSE;

#ifdef SOFT_EDGES_ON_SHINE_MASK

	// get a copy of the subject image area
	RBitmapImage *pCopy = CopyRectAreaOfBitmap();
	if ( pCopy )
	{
		// get pointer to raw pixel data
		uBYTE *pPixelData = GetBitmapPixels( *pCopy, RIntRect( m_rRect.WidthHeight() ) );
		if ( pPixelData )
		{
			// create elliptical mask
			RBitmapImage *pMaskBitmap = CreateMask( TRUE );
			if ( pMaskBitmap )
			{
				// raw mask data
				uBYTE *pMaskData = GetMaskPixels( pMaskBitmap );
				if ( pMaskData )
				{
					// remove the shine
					uLONG uBitmapWidthInBytes     = RBitmapImage::GetBytesPerRow( pCopy->GetWidthInPixels(), 24 );
					uLONG uMaskBitmapWidthInBytes = RBitmapImage::GetBytesPerRow( pMaskBitmap->GetWidthInPixels(), 8 );

					bRet = DoRemoveShine( pPixelData, m_rRect.WidthHeight(), uBitmapWidthInBytes, pMaskData, m_rRect.WidthHeight(), uMaskBitmapWidthInBytes );
					if ( bRet )
					{
						// soft-edge mask blend
						m_rLibrary.AlphaBlend( *pCopy, RIntRect(m_rRect.WidthHeight()), m_rBitmap, m_rRect, *pMaskBitmap, RIntRect(m_rRect.WidthHeight()) );
					}
				}

				delete pMaskBitmap;
			}
		}
		delete pCopy;
	}

#else

	// get pointer to the raw pixel data
	uBYTE *pPixelData = GetBitmapPixels( m_rBitmap, m_rRect );
	if ( pPixelData )
	{
		// get elliptical mask
		RBitmapImage *pMaskBitmap = CreateMask();
		if ( pMaskBitmap )
		{
			// raw mask data
			uBYTE *pMaskData = GetMaskPixels( pMaskBitmap );
			if ( pMaskData )
			{
				// remove the shine
				uLONG uBitmapWidthInBytes     = RBitmapImage::GetBytesPerRow( pCopy->GetWidthInPixels(), 24 );
				uLONG uMaskBitmapWidthInBytes = RBitmapImage::GetBytesPerRow( pMaskBitmap->GetWidthInPixels(), 8 );

				bRet = DoRemoveShine( pPixelData, m_rRect, uBitmapWidthInBytes, pMaskData, m_rRect, uMaskBitmapWidthInBytes );
			}

			delete pMaskBitmap;
		}
	}

#endif		// SOFT_EDGES_ON_SHINE_MASK

	return bRet;
}	

// ****************************************************************************
//
//  Function Name:	RFixFlaw::DoRemoveEncircledChroma()
//
//  Description:	
// 		for red eye reduction (hue hRed) or pet eye reduction (hue hAny)
// 		pass in pointer to 24-bit bitmap exactly the square size spanned by circular cursor
// 		result overwrites input bitmap, which is ultimately burned back into photo
// 		OR, instead of copying square piece of picture, point to start within picture, and
// 		use row byte width of whole picture.
// 		size must be correct, otherwise will write off the end of Bitmap
// 		square can be as large as 32K on a side, but I 	can't do anything useful if less than 2 pixels square
// 		symmetrical, so scanline ordering (upward or downward) doesn't matter.
// 		return success flag 
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RFixFlaw::DoRemoveEncircledChroma( uBYTE *pPixelData, uLONG uSquareSize, uLONG uBitmapWidthInBytes, EDiscreteHue hueSelector )
{
	YFloatType neutralizationFrac;
	YFloatType chromaFrac;						// range 0.0 to 1.0
	YFloatType proximityWeight = 0.0;		// range 0.0 to 1.0
	uBYTE *pPixelR, *pPixelG, *pPixelB;
	uLONG x, y;
	uLONG squaredDistance, squaredMaxRange;
	sLONG rangePercent;
	sLONG center, offsetX, offsetY;
	BOOLEAN isEven = FALSE; 
	
	uBYTE *pBitmapMovable = pPixelData;
	
	if ( ! (uBitmapWidthInBytes >= (uSquareSize * 3)) )
	{
		return FALSE;						// This is a caller ERROR
	}
	
	// can't do anything useful if less than 2.  
	// Also avoid zero divide.
	if (uSquareSize < 2 )
	{
		if ( uSquareSize == 1 )		// a one-pixel square - fake an effect for the goobers
		{
			pPixelR = pBitmapMovable + kRedComponent;
			pPixelG = pBitmapMovable + kGreenComponent;
			pPixelB = pBitmapMovable + kBlueComponent;
			
			if ( hueSelector == kDiscreteHueRed )
			{
				chromaFrac = Redness (*pPixelR, *pPixelG, *pPixelB);
			}
			else
			{
				chromaFrac = 1.0;
			}
			neutralizationFrac = proximityWeight * chromaFrac;
			NeutralizeBy (pPixelR, pPixelG, pPixelB, neutralizationFrac);
		}
		return TRUE;						// Not an error.
	}
		
	if ( ! (uSquareSize & 1) )
	{
		isEven = TRUE;
	}

	center = uSquareSize / 2;
	squaredMaxRange = center * center + isEven; // square of distance from center avoids squareroot
	
	for (y = 0; y < uSquareSize; y++)	// rows loop
	{
		for (x = 0; x < uSquareSize; x++)	// columns loop
		{				
			// get proximity
			offsetX = x - center;
			offsetY = y - center;
			if (isEven)
			{
				if (offsetX >= 0)  // offsetX >= 0
					offsetX++;
				if (offsetY >= 0)
					offsetY++;
			}
			
			squaredDistance = offsetX*offsetX + offsetY*offsetY;

			// If outside range, don't do any neutralization.
			// In fact, don't do any work getting pixel values.
			if (! (squaredDistance > squaredMaxRange))
			{
				rangePercent = (squaredDistance * 100) / squaredMaxRange;

				proximityWeight = LookupProfile (rangePercent, GSquaredProximityDropoffProfile);
				
				proximityWeight *= GProximityTweakFactor;
				
				// get pixel components
				pPixelR = pBitmapMovable + kRedComponent;
				pPixelG = pBitmapMovable + kGreenComponent;
				pPixelB = pBitmapMovable + kBlueComponent;
			
				// get filter hue
				switch (hueSelector)
				{
					case kDiscreteHueRed:
						chromaFrac = Redness (*pPixelR, *pPixelG, *pPixelB);
						// tweak factors are not used until demonstrated necessary
						// chromaFrac *= GRednessTweakFactor
						break;

					case kDiscreteHueAny:
					default:
						chromaFrac = 1.0;
						// tweak factors are not used until demonstrated necessary
						// proximityWeight *= GProximityTweakFactor;
						break;				
				}

				neutralizationFrac = proximityWeight * chromaFrac;
				NeutralizeBy (pPixelR, pPixelG, pPixelB, neutralizationFrac);	// writes into bitmap
			}
			
			pBitmapMovable += 3;
		}
		pBitmapMovable += (uBitmapWidthInBytes - (uSquareSize * 3));
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::DoMaskedBlur()
//
//  Description:	
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RFixFlaw::DoMaskedBlur()
{
	YSharpness nSmoothFactor = -4;
	
	if ( m_rRect.Width() < 3 || m_rRect.Height() < 3 )
	{
		return TRUE;			// do nothing, too small
	}
	
	if ( m_rRect.Width() < 9 || m_rRect.Height() < 9 )
	{
		nSmoothFactor = -1;	// small square, reduce factor
	}
	
	BOOLEAN bRet = FALSE;

	// get a copy of the subject image area
	RBitmapImage *pCopy = CopyRectAreaOfBitmap();

	if ( pCopy )
	{
		// for now, use Accusoft smoothing
		m_rLibrary.Sharpen( *pCopy, nSmoothFactor ); 

		// get pointer to the raw pixels of the blurred copy
		uBYTE *pBlurPixelData = GetBitmapPixels( *pCopy, RIntRect( m_rRect.WidthHeight() ) );
		if ( pBlurPixelData )
		{
			// create an elliptical mask
			RBitmapImage *pMaskBitmap = CreateMask();
			if ( pMaskBitmap )
			{
				// get pointer to raw mask data
				uBYTE *pMaskData = GetMaskPixels( pMaskBitmap );
				if ( pMaskData )
				{
					// finally, get raw pixels for the subject area of our original image
					uBYTE *pOrigPixelData = GetBitmapPixels( m_rBitmap, m_rRect );
					if ( pOrigPixelData )
					{
						// replace original pixels with blurred pixels, within masked ellipse
						uLONG uOrigBitmapWidthInBytes = RBitmapImage::GetBytesPerRow( m_rBitmap.GetWidthInPixels(), 24 );
						uLONG uBlurBitmapWidthInBytes = RBitmapImage::GetBytesPerRow( pCopy->GetWidthInPixels(), 24 );
						uLONG uMaskBitmapWidthInBytes = RBitmapImage::GetBytesPerRow( pMaskBitmap->GetWidthInPixels(), 8 );

						bRet = ReplaceMaskedPixels( pOrigPixelData, m_rRect, uOrigBitmapWidthInBytes, pBlurPixelData, m_rRect.WidthHeight(), uBlurBitmapWidthInBytes, pMaskData, m_rRect.WidthHeight(), uMaskBitmapWidthInBytes );
					}
				}

				delete pMaskBitmap;
			}
		}

		delete pCopy;
	}

	return bRet;
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::DoRemoveShine()
//
//  Description:	
// 		for reduction of shiney area
// 		pass in pointer to 24-bit target bitmap
// 		pass in pointer to 8-bit mask that exactly matches target bitmap in pixel size
//			black (0) masks, all non-zero pixels allow the effect.
// 		bitmaps should be close to minimal size bounding region of interest
// 		result overwrites input bitmap, which is ultimately burned back into photo
// 		OR, instead of copying target piece of picture, point to start within picture, and
// 		use row byte width of whole picture.
// 		scanline ordering (upward or downward) doesn't matter, but must match between mask and bitmap
// 		limited to 20000 pixels across and down
// 		returns success flag
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RFixFlaw::DoRemoveShine( uBYTE *pPixelData, const RIntRect& rBmpRect, uLONG uBmpWidthInBytes, uBYTE *pMaskData, const RIntRect& rMaskRect, uLONG uMaskBmpWidthInBytes )
{
	uBYTE *pMaskMovable;
	uLONG x, y;
	uBYTE avgRed, avgGrn, avgBlu;
	sLONG avgBrightness;
	uBYTE *pPixelR, *pPixelG, *pPixelB;
	uLONG totalR, totalG, totalB, sampleCount;
	
	uBYTE *pBitmapMovable;
	uLONG uBmpWidth  = rBmpRect.Width();
	uLONG uBmpHeight = rBmpRect.Height();
	uLONG uMaskWidth  = rMaskRect.Width();
	uLONG uMaskHeight = rMaskRect.Height();

	if ( ( (uLONG)uBmpWidth * (uLONG)uBmpHeight ) > (uLONG)23000000 )
	{
		return FALSE;	// cannot handle total for average
						// maybe later implement adaptive subsampling
	}
	
	// check bytewidths sufficient
	if ( ! ( uBmpWidthInBytes >= (uBmpWidth * 3) ) )
	{
		return FALSE;						// This is a caller ERROR
	}
	if (! ( uMaskBmpWidthInBytes >= uMaskWidth ) )
	{
		return FALSE;						// This is a caller ERROR
	}

	// check mask size equal to bitmap size
	if ( ! ( (uMaskWidth == uBmpWidth) && (uMaskHeight == uBmpHeight) ) )
	{
		return FALSE;						// This is a caller ERROR
	}
		
	// average color accumulation pass
	totalR = totalG = totalB = sampleCount = 0L;
	pBitmapMovable  = pPixelData;
	pMaskMovable = pMaskData;
	for (y = 0; y < uBmpHeight; y++)	// rows loop
	{
		for (x = 0; x < uBmpWidth; x++)	// columns loop
		{
			// is this pixel unmasked?
			if (*pMaskMovable)
			{
				// get pixel components
				pPixelR = pBitmapMovable + kRedComponent;
				pPixelG = pBitmapMovable + kGreenComponent;
				pPixelB = pBitmapMovable + kBlueComponent;
				
				totalR += *pPixelR;
				totalG += *pPixelG;
				totalB += *pPixelB;
				
				sampleCount++;
			}
			pBitmapMovable += 3;
			pMaskMovable +=1;
		}
		pBitmapMovable += uBmpWidthInBytes - (uBmpWidth * 3);
		pMaskMovable += uMaskBmpWidthInBytes - uMaskWidth;
	}

	if ( sampleCount < 1 )
	{
		return FALSE;	// image too small?  bad mask??
	}

	// do averages, by component 
	avgRed = totalR / sampleCount;
	avgGrn = totalG / sampleCount;
	avgBlu = totalB / sampleCount;
	avgBrightness = ((sLONG)avgRed + (sLONG)avgGrn + (sLONG)avgBlu) / 3;
	
	// pixel modification pass
	pBitmapMovable  = pPixelData;
	pMaskMovable = pMaskData;
	for (y = 0; y < uBmpHeight; y++)	// rows loop
	{
		for (x = 0; x < uBmpWidth; x++)	// columns loop
		{
			// is this pixel unmasked?
			if (*pMaskMovable)
			{
				// get pixel components
				pPixelR = pBitmapMovable + kRedComponent;
				pPixelG = pBitmapMovable + kGreenComponent;
				pPixelB = pBitmapMovable + kBlueComponent;
				
				// mix down and write pixels
				MixDownWith( pPixelR, pPixelG, pPixelB, avgRed, avgGrn, avgBlu, avgBrightness, GWeightTweakFactor);
			}		

			pBitmapMovable += 3;
			pMaskMovable +=1;
		}

		pBitmapMovable += uBmpWidthInBytes - (uBmpWidth * 3);
		pMaskMovable += uMaskBmpWidthInBytes - uMaskWidth;
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::GetBitmapPixels()
//
//  Description:	Finds start of area to be processed
//
//  Returns:		
//
// ****************************************************************************
//
uBYTE *RFixFlaw::GetBitmapPixels( RBitmapImage& rBitmap, const RIntRect& rRect )
{
	// make sure:
	// rBitmap is 24 bits deep;
	// bitmap >= lower-right extent of rRect
	//
	TpsAssert( rRect.Width() >= 0 && rRect.Height() >= 0, "Negative rect width or height!" );
	TpsAssert( rBitmap.GetBitDepth() == 24, "rSrcBitmap not 24 bits deep." );
	TpsAssert( rRect.m_Right <= (YIntCoordinate)rBitmap.GetWidthInPixels() && rRect.m_Bottom <= (YIntCoordinate)rBitmap.GetHeightInPixels(), "rBitmap too small for rRect." );

	// get pointer to source data
	// if biHeight field is >=0 data is bottom-up, else top-down
	void *pSrcRawData = rBitmap.GetRawData();
	uBYTE *pPixelData = (uBYTE*)rBitmap.GetImageData( pSrcRawData );

	uLONG uBitmapWidthInBytes = RBitmapImage::GetBytesPerRow( rBitmap.GetWidthInPixels(), 24 );

	if (((BITMAPINFOHEADER*)pSrcRawData)->biHeight >= 0)
	{
		pPixelData += ((rBitmap.GetHeightInPixels() - rRect.m_Bottom) * uBitmapWidthInBytes) + (3 * rRect.m_Left);
	}
	else
	{
		pPixelData += (rRect.m_Top * uBitmapWidthInBytes) + (3 * rRect.m_Left);
	}

	return pPixelData;
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::CreateMask()
//
//  Description:	Generates an 8-bit elliptical mask within the area of m_rRect
//
//  Returns:		pointer to the new mask bitmap
//
// ****************************************************************************
//
RBitmapImage *RFixFlaw::CreateMask( BOOLEAN bSoftEdge )
{
	TpsAssert( m_rRect.Width() >= 0 && m_rRect.Height() >= 0, "Negative rect width or height!" );

	RBitmapImage *pMaskBitmap = NULL;
	ROffscreenDrawingSurface rODS;
	try
	{
		// create and initialize the bitmap
		pMaskBitmap = new RBitmapImage;

		try
		{
			pMaskBitmap->Initialize( m_rRect.Width(), m_rRect.Height(), 8 );
		}
		catch( ... )
		{
			delete pMaskBitmap;
			pMaskBitmap = NULL;
			throw;
		}

		// set the image into the offscreen
		rODS.SetImage( pMaskBitmap );


		// fill in the panel with the background color
		rODS.SetFillColor( kBlack );
		rODS.FillRectangle( RIntRect( m_rRect.WidthHeight() ) );

		rODS.SetFillColor( kWhite );
		rODS.FillEllipse( RIntRect( m_rRect.WidthHeight() ) );
	}

	catch( ... )
	{
		;
	}

	if ( bSoftEdge )
	{
		if ( m_rRect.Width() > 8 && m_rRect.Height() > 8 )
		{
			m_rLibrary.Blur( *pMaskBitmap, *pMaskBitmap, 2, blurIn, boxFilter );
		}
	}

	// cleanup
	rODS.ReleaseImage();

	return pMaskBitmap;
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::GetMaskPixels()
//
//  Description:	
//
//  Returns:		pointer to the passed mask image's raw pixel data
//
// ****************************************************************************
//
uBYTE *RFixFlaw::GetMaskPixels( RBitmapImage *pMaskBitmap )
{
	// get pointer to mask data
	void* pMaskRawData = pMaskBitmap->GetRawData();
	return (uBYTE *)pMaskBitmap->GetImageData( pMaskRawData );
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::CopyRectAreaofBitmap()
//
//  Description:	Makes a copy of the portion of the bitmap bounded by m_rRect
//
//  Returns:		a pointer to the new copy
//
// ****************************************************************************
//
RBitmapImage *RFixFlaw::CopyRectAreaOfBitmap()
{
	RBitmapImage *pBitmap = NULL;
	ROffscreenDrawingSurface rODS;
	try
	{
		// create and initialize the bitmap
		pBitmap = new RBitmapImage;

		try
		{
			pBitmap->Initialize( m_rRect.Width(), m_rRect.Height(), 24 );
		}
		catch( ... )
		{
			delete pBitmap;
			pBitmap = NULL;
			throw;
		}

		// set the image into the offscreen
		rODS.SetImage( pBitmap );
		m_rBitmap.Render( rODS, m_rRect, RIntRect( m_rRect.WidthHeight() ) );
	}

	catch( ... )
	{
		;
	}

	// cleanup
	rODS.ReleaseImage();

	return pBitmap;
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::ReplaceMaskedPixels()
//
//  Description:	relpaces old pixels with new pixels inside the masked area
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RFixFlaw::ReplaceMaskedPixels( uBYTE *pOldPixels, const RIntRect& rOldRect, uLONG uOldBmpWidthInBytes, uBYTE *pNewPixels, const RIntRect& rNewRect, uLONG uNewBmpWidthInBytes, uBYTE *pMaskData, const RIntRect& rMaskRect, uLONG uMaskBmpWidthInBytes )
{
	uLONG uOldBmpWidth  = rOldRect.Width();
	uLONG uOldBmpHeight = rOldRect.Height();
	uLONG uNewBmpWidth  = rNewRect.Width();
	uLONG uNewBmpHeight = rNewRect.Height();
	uLONG uMaskWidth    = rMaskRect.Width();
	uLONG uMaskHeight   = rMaskRect.Height();

	// check bytewidths sufficient
	if ( ! ( uOldBmpWidthInBytes >= (uOldBmpWidth * 3) ) )
	{
		return FALSE;
	}

	if ( ! ( uNewBmpWidthInBytes >= (uNewBmpWidth * 3) ) )
	{
		return FALSE;
	}

	if (! ( uMaskBmpWidthInBytes >= uMaskWidth ) )
	{
		return FALSE;
	}

	// check mask size and bitmap sizes all equal
	if ( ! ( (uMaskWidth == uOldBmpWidth) && (uMaskHeight == uOldBmpHeight) ) )
	{
		return FALSE;
	}
	if ( ! ( (uMaskWidth == uNewBmpWidth) && (uMaskHeight == uNewBmpHeight) ) )
	{
		return FALSE;
	}
		
	for ( uLONG y = 0; y < uOldBmpHeight; y++ )
	{
		for ( uLONG x = 0; x < uOldBmpWidth; x++ )
		{
			// is this pixel unmasked?
			if ( *pMaskData )
			{
				memcpy( pOldPixels, pNewPixels, 3 );
			}
			pOldPixels += 3;
			pNewPixels += 3;
			pMaskData++;
		}

		pOldPixels += uOldBmpWidthInBytes - ( uOldBmpWidth * 3 );
		pNewPixels += uNewBmpWidthInBytes - ( uNewBmpWidth * 3 );
		pMaskData += uMaskBmpWidthInBytes - uMaskWidth;
	}

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::MixDownWith()
//
//  Description:	
// 		helper function
// 		pixel weight factor  0.0 thru 1.0, probably .33 to .5
//
//  Returns:		
//
// ****************************************************************************
//
void RFixFlaw::MixDownWith( uBYTE *pR, uBYTE *pG, uBYTE *pB, uBYTE avgR, uBYTE avgG, uBYTE avgB, sLONG avgBrightness, YFloatType pixelWeightFactor)
{
	sLONG r, g, b, equivBrightness;
	
	if (pixelWeightFactor < 0.0) pixelWeightFactor = .5; // unexpected, but just fix reasonably
	if (pixelWeightFactor > 1.0) pixelWeightFactor = .5;
	
	r = *pR; g = *pG; b = *pB;
	equivBrightness = (r + g + g) / 3;
	
	if (equivBrightness > avgBrightness)	// only look at hilite colors -- lighter than average
	{
		// weighted mix of pixel component with same component of avg color, and modify pixel
		// rounding would best preserve hue, but may be unnecessary
		*pR = r - (sLONG)((r - avgR) * pixelWeightFactor);
		*pG = g - (sLONG)((g - avgG) * pixelWeightFactor);
		*pB = b - (sLONG)((b - avgB) * pixelWeightFactor);
	}
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::NeutralizeBy()
//
//  Description:	
// 		helper function
// 		input neutralization is a fraction, range 0.0 to 1.0.
// 		result overwrites input pointer values
//
//  Returns:		
//
// ****************************************************************************
//
void RFixFlaw::NeutralizeBy( uBYTE *pR, uBYTE *pG, uBYTE *pB, YFloatType neutralization )
{
	sLONG r, g, b;
	sLONG equivBrightness;		// assume integer is good enough
	
	// check range and just fix it
	if (neutralization < 0.0) 
		neutralization = 0.0;
	else
		if (neutralization > 1.0)
			neutralization = 1.0;
			
	r = *pR; g = *pG; b = *pB;
	
	equivBrightness = ((sLONG)r + (sLONG)g + (sLONG)b) / 3; //don't bother with luminance or rounding

	// neutralize and modify pixel
	// rounding would best preserve hue, but may be unnecessary
	*pR = r + (sLONG)((equivBrightness - r) * neutralization);
	*pG = g + (sLONG)((equivBrightness - g) * neutralization);
	*pB = b + (sLONG)((equivBrightness - b) * neutralization);
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::Redness()
//
//  Description:	
// 			helper function
// 			result is a fraction, range 0.0 to 1.0
//
//  Returns:		
//
// ****************************************************************************
//
YFloatType RFixFlaw::Redness( uBYTE r, uBYTE g, uBYTE b )
{
	uBYTE minComponentVal = b;
	sLONG componentTotal;
	
	if (g < minComponentVal)
		minComponentVal = g;
	if (r < minComponentVal)	// if red is the minimum component, there is zero redness
		return (0.0);
		
	// remove min component val (whiteness) from all
	r -= minComponentVal;
	g -= minComponentVal;
	b -= minComponentVal;
	
	componentTotal = r + g + b;
	
	// watch out for zero divide, if color was white, black, or gray
	if (componentTotal == 0)
		return (0.0);
	
	return ( (YFloatType) r / (YFloatType) componentTotal);	 
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::LookupProfile()
//
//  Description:	
// 		helper function
// 		input integer percentage, 0 thru 100
// 		imput profile array of 11 elements, 0 thru 100
// 		output float in the range 0.0 to 1.0
//
//  Returns:		
//
// ****************************************************************************
//
YFloatType RFixFlaw::LookupProfile( sLONG percentage, sLONG *profile )
{
	sLONG quantizedIndex;
	sLONG indexRemainder;
	sLONG base, next;
	
	if (percentage < 0) percentage = 0;
	if (percentage > 100) percentage = 100;
	
	quantizedIndex = percentage / 10;  // 0 thru 10
	indexRemainder = percentage - quantizedIndex * 10; // 0 thru 9
	if (! indexRemainder)
		return (.01 * profile [quantizedIndex]);
	else
	{
		base = profile [quantizedIndex];
		next = profile [quantizedIndex + 1];
		
		return (.01 * (base + (next - base) * indexRemainder / 10));
	}

}

#ifdef TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFixFlaw::TestRectPosition()
//
//  Description:	test routine; draws black squares on bitmap
//
//  Returns:		
//
// ****************************************************************************
//
BOOLEAN RFixFlaw::TestRectPosition()
{
	TpsAssert( m_rRect.Width() >= 0 && m_rRect.Height() >= 0, "Negative rect width or height!" );
	TpsAssert( m_rBitmap.GetBitDepth() == 24, "rSrcBitmap not 24 bits deep." );
	TpsAssert( m_rRect.m_Right <= (YIntCoordinate)m_rBitmap.GetWidthInPixels() && m_rRect.m_Bottom <= (YIntCoordinate)m_rBitmap.GetHeightInPixels(), "rBitmap too small for rRect." );

	uLONG uBytesPerRow = RBitmapImage::GetBytesPerRow( m_rBitmap.GetWidthInPixels(), 24 );

	// get pointer to source data
	// if biHeight field is >=0 data is bottom-up, else top-down
	void* pSrcRawData = m_rBitmap.GetRawData();
	uBYTE* pSrcRow = (uBYTE*)RBitmapImage::GetImageData( pSrcRawData );
	if (((BITMAPINFOHEADER*)pSrcRawData)->biHeight >= 0)
		pSrcRow += ((m_rBitmap.GetHeightInPixels() - m_rRect.m_Bottom) * uBytesPerRow) + (3 * m_rRect.m_Left);
	else
		pSrcRow += (m_rRect.m_Top * uBytesPerRow) + (3 * m_rRect.m_Left);

	for (YIntDimension row = 0; row < m_rRect.Height(); row++)
	{
		uBYTE* pSrc = pSrcRow;
		for (YIntDimension column = 0; column < m_rRect.Width(); column++)
		{
			*pSrc++ = 0;
			*pSrc++ = 0;
			*pSrc++ = 0;
		}

		pSrcRow += uBytesPerRow;
	}

	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RFixFlaw::Validate( )
//
//  Description:	Validates the object
//
//  Returns:		Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFixFlaw::Validate() const
{
	RObject::Validate( );
	TpsAssertIsObject( RFixFlaw, this );
}

#endif  // TPSDEBUG