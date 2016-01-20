// ****************************************************************************
//
//  File Name:			ImageDocument.cpp
//
//  Project:			Image Component
//
//  Author:				Greg Brown
//
//  Description:		Declaration of the RImageDocument class
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
//  $Logfile:: /PM8/ImageComp/Source/ImageDocument.cpp                        $
//   $Author:: Rgrenfel                                                       $
//     $Date:: 3/08/99 1:06p                                                  $
// $Revision:: 2                                                              $
//
// ****************************************************************************

#include	"ImageIncludes.h"

ASSERTNAME

#include	"ImageDocument.h"
#include	"ImageView.h"
#include "ApplicationGlobals.h"
#include "ResourceManager.h"
#include "ChunkyStorage.h"
#include "StorageSupport.h"
#include "DataTransfer.h"
#include "CompositeSelection.h"
#include "Frame.h"
#include "DrawingSurface.h"
#include "OffscreenDrawingSurface.h"
#include "Configuration.h"
#include "ImageInterface.h"
#include "DataInterface.h"
#include "CollectionManager.h"
#include "RenaissanceResource.h"
#include "GraphicCompResource.h"
#include "FrameworkResourceIDs.h"
#include "path.h"
#include "ComponentTypes.h"
#include	"EdgeOutlineSettings.h"
#include "StandaloneApplication.h"
#include "GifAnimator.h"

#define SET_CROPRECT_FOR_IMPORT

const uLONG			kConvertVectorXDpi =		96; //288;
const uLONG			kConvertVectorYDpi =		96; //288;

// If a vector image is to be converted to a bitmap for editing, this is
// the maximum size allowed without confirmation by the user
const uLONG			kWarnConvertVectorSize =	0x0100000;
const uLONG			kConvertedVectorBitDepth =	24;

const YChunkTag	kImageChunkID =			0x03160001;
const YChunkTag	kImagePathChunkID =		0x03160002;

// written by Print Shop 6.0 and later
const YChunkTag	kPartialPathChunkID =	0x03160003;
const YChunkTag	kImageCropPathID    =	0x03160004;
const YChunkTag kImageFrameFillColor =  0x03160005;
const YChunkTag kImageFrameFillIdent =  0x03160006;
const YChunkTag	kPropertyChunkID =			'PROP';

const int			kMinBrushWidth = 1;
const RSolidColor	crBlack( kBlack );

// ****************************************************************************
//
//  Function Name:	RImageDocument::RImageDocument()
//
//  Description:		Constructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImageDocument::RImageDocument(RApplication* pApp, const RComponentAttributes& rComponentAttributes, const YComponentType& componentType, BOOLEAN fLoading)
	: RComponentDocument(pApp, rComponentAttributes, componentType, fLoading)
	, m_rTintColor( RSolidColor( kBlack ) )
	, m_fLinkedData( FALSE )
{	
	m_pImage = NULL;
	m_fResourcesReleased = FALSE;
	
	// Initialize the proxy and the image effects
	ResetProxy();
	ResetImageEffects();
	
	// Load the placeholder from the resource file
	m_rPlaceholderBitmap.Initialize(static_cast<YResourceId>(PLACEHOLDER_IMAGE_COMPONENT));

	// Get the users print preferences
	uLONG uHiResPhotos;
	RRenaissanceUserPreferences rConfig;
	if (!rConfig.GetLongValue(RRenaissanceUserPreferences::kHiResPhoto, uHiResPhotos))
		m_fHiResPhotos = FALSE;
	else
		m_fHiResPhotos = static_cast<BOOLEAN>(uHiResPhotos);

	// Default m_fPreview anf m_fEditResolution to FALSE
	m_fPreview = FALSE;
	m_fEditResolution = FALSE;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::~RImageDocument( )
//
//  Description:		Destructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImageDocument::~RImageDocument()
{
	delete m_pImage;	
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::Render()
//
//  Description:		Called to render the documents data into an output target
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::Render(RDrawingSurface& rDS, const R2dTransform& rTransform, const RIntRect& rRender, const RRealSize& rSize) const
{
	// Call a non-const version of render	
	const_cast<RImageDocument*>(this)->Render(rDS, rTransform, rRender, rSize);
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::WritePltt( ... )
//
//  Description:		Write the given bitmaps palette to the given file.
//
//  Returns:			Nothing
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::WritePlttData( const RMBCString& rOutputFileName )
{
	RImage *pImage = GetImage();
	if (pImage)
	{
		RBitmapImage* pBitmapImage = dynamic_cast<RBitmapImage*>(pImage);
		m_rLibrary.WritePlttData( *pBitmapImage, rOutputFileName );
	}
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::Render()
//
//  Description:		Called to render the documents data into an output target
//							(This is a non-const version of the function used to avoid
//							multiple const_casts)
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::Render(RDrawingSurface& rDS, const R2dTransform& rTransform, const RIntRect&, const RRealSize& rSize)
{
	RImage *pImage = GetImage();
	if ( !pImage )
		pImage = &m_rPlaceholderBitmap;

	pImage->LockImage( );

	RBitmapImage* pBitmapImage = dynamic_cast<RBitmapImage*>(pImage);
	if (pBitmapImage)
	{
		RenderBitmap(*pBitmapImage, rDS, rTransform, rSize);
	}
	else
	{
		RVectorImage* pVectorImage	= dynamic_cast<RVectorImage*>(pImage);
		if (pVectorImage)
		{
			RenderVector(*pVectorImage, rDS, rTransform, rSize);
		}
		else
		{
			TpsAssertAlways("Bad m_pImage in RImageDocument.");
		}
	}



	pImage->UnlockImage( );
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::RenderShadow( )
//
//  Description:		Called to render the document's shadow
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageDocument::RenderShadow( RDrawingSurface& drawingSurface,
														const R2dTransform& transform,
														const RIntRect& rcRender,
														const RRealSize& rSize,
														const RSolidColor& shadowColor ) const
{
	RComponentView* pView = dynamic_cast<RComponentView*>(GetActiveView());
	RImage *pImage = (const_cast<RImageDocument*>(this))->GetImage();
	RBitmapImage* pBitmapImage = dynamic_cast<RBitmapImage*>(pImage);

	REdgeOutlineSettings edgeOutlineSettings;
	if (pView)
		pView->GetEdgeOutlineSettings( edgeOutlineSettings );

	if (pView && pView->HasBackgroundColor())
	{
		// if we have an active view and it has a behind color,
		// render rectangular shadow as though cast from the behind color
		RColor rOldFill = drawingSurface.GetFillColor( );
		drawingSurface.SetFillColor( shadowColor );
		drawingSurface.FillRectangle( RRealRect( rSize ), transform );
		drawingSurface.SetFillColor( rOldFill );
	}
	else if (pBitmapImage && !edgeOutlineSettings.m_bEdgeOutlineOn)
	{
		RPath* rPath = pBitmapImage->GetClippingRPath();
		if (rPath)
			rPath->Render( drawingSurface, rSize, transform, rcRender, kFilled, shadowColor, shadowColor );
		else
		{
			RColor rOldFill = drawingSurface.GetFillColor( );
			drawingSurface.SetFillColor( shadowColor );
			drawingSurface.FillRectangle( RRealRect( rSize ), transform );
			drawingSurface.SetFillColor( rOldFill );
		}
	}
	else // vector image or placeholder
	{
		// let inherited member function handle it
		RDocument::RenderShadow( drawingSurface, transform, rcRender, rSize, shadowColor );
	}

	if (pView && pView->GetFrameType() != kNoFrame && !pView->HasBackgroundColor())
	{
		// if we have an active view and it has a frame,
		// render shadow as though cast from the frame
		pView->RenderFrame( drawingSurface, transform, shadowColor );
	}
}
// ****************************************************************************
//
//  Function Name:	RImageDocument::RenderBitmap()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::RenderBitmap(RBitmapImage& rBitmap, RDrawingSurface& rDS, const R2dTransform& rTransform, const RRealSize& rSize)
{
	// Set up the drawing surface
	rDS.SetForegroundMode(kNormal);

	// Make a local copy of the transform so we can munge it if necessary (edge outlines)
	R2dTransform rxTransform = rTransform;

	// Compute the destination rect
	RRealVectorRect rComponentRect(rSize);

	// Check for edge outline
	RComponentView* pComponentView = static_cast<RComponentView*>(GetActiveView());	
	REdgeOutlineSettings edgeOutlineSettings;
	pComponentView->GetEdgeOutlineSettings( edgeOutlineSettings );

	if(edgeOutlineSettings.m_bEdgeOutlineOn)
	{
		// Resize the component to accommodate the edge outline
		YRealDimension yFudge = 1.0; // no fudge
		YRealDimension yPenFactor = .0025 * (edgeOutlineSettings.m_nEdgeOutlineWidth + 1);
		YRealDimension	yScale = yFudge - yPenFactor; 

		RRealPoint midPt( rSize.m_dx/2.0, rSize.m_dy/2.0 );
		R2dTransform rOutlineTransform = rTransform;
		rxTransform.PreScaleAboutPoint( midPt.m_x, midPt.m_y, yScale, yScale );
	}

	rComponentRect *= rxTransform;	
	// Scale the transform so that we are mapping pixels to device units	
	R2dTransform rBitmapTransform = rxTransform;
	RIntSize rComponentSize(rComponentRect.Width(), rComponentRect.Height());	
	RIntRect rDeviceRect = rComponentRect.m_TransformedBoundingRect;
	RPath* rPath		= rBitmap.GetClippingRPath();
	RPath* rFramePath	= rBitmap.GetFrameRPath();

	// See if we need to recreate the proxy
	if (rSize != m_rLastSize || 
		! m_rLastTransform.AreScaleAndRotateEqual( rxTransform ) ||
		GetTint() != m_nLastTint || edgeOutlineSettings != m_nLastEdgeOutline)	
	{
		// Get the size of the bitmap
		RIntSize rBitmapSize(rBitmap.GetWidthInPixels(), rBitmap.GetHeightInPixels());
		
		// Scale the transform so that we are mapping pixels to device units	
		rBitmapTransform.PreScale((YFloatType)rSize.m_dx / (YFloatType)rBitmapSize.m_dx, (YFloatType)rSize.m_dy / (YFloatType)rBitmapSize.m_dy);
		
		// Translate the origin so that the rotated bitmap is mapped into the right place in the destination bitmap
		rBitmapTransform.PostTranslate(-rDeviceRect.m_Left, -rDeviceRect.m_Top);
#if 0
		// RIP - Reworked the limits which were taken out to avoid
		//			the "scewing" bug by Greg.  I've changed the following to preserve
		//			the image's aspect which avoids the skew.  This limit avoids
		//			creating proxies over the image size in higher zoom modes.
		//
		//			However, this provides limited improvement in speed and causes
		//			extremely ugly edges in rotated images and has therefor been left
		//			disabled for the time being.
		//
		// Do we want to limit the size of the destination bitmap?
		if((rComponentSize.m_dx > rBitmapSize.m_dx || rComponentSize.m_dy > rBitmapSize.m_dy) && !rPath && !rDS.IsPrinting())
		{
			YFloatType nFactor = 1.0;
			if ((rComponentSize.m_dx / rBitmapSize.m_dx) > (rBitmapSize.m_dy / rComponentSize.m_dy))
			{
				nFactor = (YFloatType)rBitmapSize.m_dx / (YFloatType)rComponentSize.m_dx;
			}
			else
			{
				nFactor = (YFloatType)rBitmapSize.m_dy / (YFloatType)rComponentSize.m_dy;
			}

			// Scale the transform down so the destination bitmap is the same DPI as the original
			rBitmapTransform.PostScale(nFactor, nFactor);
		}
#endif
		// if we're printing, limit the bitmap resolution to either
		// kLoResBitmapXDpi, kLoResBitmapYDpi or kHiResBitmapXDpi, kHiResBitmapYDpi
		// depending on user preference in registry
		if (rDS.IsPrinting()/* && !rPath*/)
		{
			YRealDimension bitmapXDpi = kLoResBitmapXDpi;
			YRealDimension bitmapYDpi = kLoResBitmapYDpi;
			if (m_fHiResPhotos)
			{
				bitmapXDpi = kHiResBitmapXDpi;
				bitmapYDpi = kHiResBitmapYDpi;
			}
			RRealVectorRect rBitmapRect(0, 0, rBitmapSize.m_dx, rBitmapSize.m_dy);
			rBitmapRect *= rBitmapTransform;
			RRealSize rSizeInInches(::LogicalUnitsToInches(rSize.m_dx), ::LogicalUnitsToInches(rSize.m_dy));
			RRealSize rDpi((YIntDimension)((YRealDimension)rBitmapRect.Width() / rSizeInInches.m_dx), (YIntDimension)((YRealDimension)rBitmapRect.Height() / rSizeInInches.m_dy));
			if (rDpi.m_dx > bitmapXDpi) rBitmapTransform.PostScale((YFloatType)bitmapXDpi / (YFloatType)rDpi.m_dx, 1.0);
			if (rDpi.m_dy > bitmapYDpi) rBitmapTransform.PostScale(1.0, (YFloatType)bitmapYDpi / (YFloatType)rDpi.m_dy);
		}
		// Transform (rotate and/or flip) the bitmap
		m_rLibrary.Transform(rBitmap, m_rProxyBitmap, m_rMaskBitmap, rBitmapTransform);

		// Color tint the bitmap
		RSolidColor rTintColor = m_rTintColor.GetSolidColor();
		m_rLibrary.Tint(m_rProxyBitmap, rTintColor);		

		// Tint the bitmap
		m_rLibrary.Tint(m_rProxyBitmap, GetTint());		
		
		// Set the history members
		m_rLastSize = rSize;
		m_rLastTransform = rxTransform;
		m_nLastTint = GetTint();
		m_nLastEdgeOutline = edgeOutlineSettings;
	}

	if(rBitmap.IsTransparent())
	{
		RSolidColor rColor = rBitmap.GetTransparentColor();
		m_rProxyBitmap.SetTransparentColor(rColor);
	}

	YAngle yRotation;
	YRealDimension	yXScale, yYScale;	
	rxTransform.Decompose(yRotation, yXScale, yYScale);

	YIntDimension yPenWidth = 0;

	// If we are going to use an outline calculate the pen size
	if(edgeOutlineSettings.m_bEdgeOutlineOn)	
	{
		// Scale the outline pen to the resultant transform
		YRealDimension yScaler = .40; // 10% per side viewwable
		YRealDimension	ySmallSide = (rComponentRect.Width() < rComponentRect.Height() ? rComponentRect.Width() : rComponentRect.Height());
		YRealDimension yIndex = (edgeOutlineSettings.m_nEdgeOutlineWidth + 1) * yScaler;
		YRealDimension	logWidth(ySmallSide * yIndex); // yIndex % of the small side
		YPenWidth		penSize( TransformedPenWidth(logWidth/2,rxTransform)/* + 2*/ );

		// WB 5/8/98 Kludge to adjust for printing. Trying to adjust to BobGo's tests. (get the printed
		// outline as "fat" as the outline in the headlines).
		if (rDS.IsPrinting())
		{
			if(penSize <= 1)
				penSize = 2;  // So we can see it during a print preview
			else
			{
				penSize *= .60;			// When Preference is "Always"
				RIntSize dpi = rDS.GetDPI();
				if(dpi.m_dx == 300)
					penSize *= .40;		// When Preference is "Never" and surface is really a printer
			}
		}
		yPenWidth = ::Max(penSize, YIntDimension(1));
	}
	if(rPath)
	{
		if(edgeOutlineSettings.m_bEdgeOutlineOn)
		{
			// This creates a "behind" outline where 1/2 of the edge is visible
			rPath->Render(rDS, rSize, rxTransform, rDeviceRect, kStroked, edgeOutlineSettings.m_rcEdgeOutlineColor,  kBlack, yPenWidth);
			m_rProxyBitmap.RenderAsObject(rDS, m_rMaskBitmap, rDeviceRect, rSize, rxTransform, rPath, (m_fHiResPhotos != FALSE) );
		}
		else
		{
//			m_rProxyBitmap.RenderAsObject(rDS, m_rMaskBitmap, rDeviceRect, rSize, rxTransform, rPath, m_fHiResPhotos );
			if (rFramePath)
			{
				// In order to insure the fill color/texture covers the
				// clipping path completely, we are inflating the coverage
				// area by 8% around the origin and then drawing the frame
				// on top.
				RRealPoint midPt( rSize.m_dx/2.0, rSize.m_dy/2.0 );
				R2dTransform rInflatedTransform = rxTransform;
				rInflatedTransform.PreScaleAboutPoint( midPt.m_x, midPt.m_y, 1.08, 1.08 );
				rFramePath->Render(rDS, rSize, rInflatedTransform, rDeviceRect, kFilled, kBlack, m_FrameFillColor);
			}
			m_rProxyBitmap.RenderAsObject(rDS, m_rMaskBitmap, rDeviceRect, rSize, rxTransform, rPath, (m_fHiResPhotos != FALSE) );
		}
	}
	else if (yRotation == 0.0)
	{
		m_rProxyBitmap.Render(rDS, rDeviceRect);
	}
	else if (rBitmap.IsTransparent())
	{
		// Transparent renders are performed by this render automatically now.
		m_rProxyBitmap.Render(rDS, rDeviceRect);
	}
	else
	{
		m_rProxyBitmap.RenderWithMask(rDS, m_rMaskBitmap, rDeviceRect);
	}
	if(!rPath && edgeOutlineSettings.m_bEdgeOutlineOn) // The "no path" edge outline
	{
		// Setup the pen for this DS
		YIntDimension yOriginalPenWidth = rDS.GetPenWidth();
		rDS.SetPenColor(edgeOutlineSettings.m_rcEdgeOutlineColor);
		rDS.SetPenWidth(yPenWidth);

		// Calculate the "oneUnit" value
		R2dTransform	invTransform = rxTransform;
		invTransform.Invert( );

		//	Remove the offset before computing the oneUnit in logical units (could be a BIG offset!)
		YRealDimension	xOffset, yOffset;
		invTransform.GetTranslation( xOffset, yOffset );
		invTransform.PostTranslate( -xOffset, -yOffset );
		RRealSize oneUnit = RRealSize( 1.0, 2.0 ) * invTransform;

		// Calculate the outline rectangle and stroke it with the current pen/color
		RRealRect rOutlineRect(RRealSize(rSize.m_dx-oneUnit.m_dx, rSize.m_dy-oneUnit.m_dy));
		rDS.FrameRectangle(rOutlineRect, rxTransform); // in case its rotated...
		rDS.SetPenWidth(yOriginalPenWidth);
	}
}
// ****************************************************************************
//
//  Function Name:	RImageDocument::RenderVector()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::RenderVector(RVectorImage& rVector, RDrawingSurface& rDS, const R2dTransform& rTransform, const RRealSize& rSize)
{
	// Set up the drawing surface
	rDS.SetForegroundMode(kNormal);
	
	// Compute the destination rect
	RRealVectorRect rComponentRect(rSize);
	rComponentRect *= rTransform;	
	RIntSize rComponentSize(rComponentRect.Width(), rComponentRect.Height());	
	RIntRect rDeviceRect = rComponentRect.m_TransformedBoundingRect;

	YAngle rotation;
	YRealDimension	xScale;
	YRealDimension	yScale;
	rTransform.Decompose(rotation, xScale, yScale);

	if (rotation == 0.0 && m_rTintColor == crBlack)
	{		
		if (xScale < 0)
		{
			// Swap left and right members
			YIntCoordinate yTempLeft = rDeviceRect.m_Left;
			rDeviceRect.m_Left = rDeviceRect.m_Right;
			rDeviceRect.m_Right = yTempLeft;
		}

		if (yScale < 0)
		{
			// Swap top and bottom members
			YIntCoordinate yTempTop = rDeviceRect.m_Top;
			rDeviceRect.m_Top = rDeviceRect.m_Bottom;
			rDeviceRect.m_Bottom = yTempTop;
		}

		// Draw the metafile
		rVector.Render(rDS, rDeviceRect);
	}
	else
	{
		// See if we need to recreate the proxy
		if (rSize != m_rLastSize || !m_rLastTransform.AreScaleAndRotateEqual(rTransform))
		{
			// Make sure that we don't exceed the maximum size dictated by the maximum DPI
			const YIntDimension kMaxDeviceWidth = (YIntDimension)(rVector.GetWidthInInches() * (YRealDimension)kConvertVectorXDpi);
			const YIntDimension kMaxDeviceHeight = (YIntDimension)(rVector.GetHeightInInches() * (YRealDimension)kConvertVectorYDpi);

			// Copy the vector onto the bitmap
			RBitmapImage rBitmap;
			YIntDimension yBitmapWidth = (rComponentSize.m_dx < kMaxDeviceWidth) ? rComponentSize.m_dx : kMaxDeviceWidth;
			YIntDimension yBitmapHeight = (rComponentSize.m_dy < kMaxDeviceHeight) ? rComponentSize.m_dy : kMaxDeviceHeight;

			// Create a 24bpp image to send to the printer
			if (rDS.IsPrinting() || m_rTintColor != crBlack)
				rBitmap.Initialize(yBitmapWidth, yBitmapHeight, 24);
			else
				rBitmap.Initialize(rDS, yBitmapWidth, yBitmapHeight);

				
			// Copy the vector image onto the proxy
			ROffscreenDrawingSurface rODS;
			rODS.SetImage(&rBitmap);
			
			rVector.Render(rODS, RIntRect(0, 0, yBitmapWidth, yBitmapHeight));
			rODS.ReleaseImage();

			// Color tint the image
			RSolidColor rTintColor = m_rTintColor.GetSolidColor();
			m_rLibrary.Tint(rBitmap, rTintColor);		

			// Re-create the mask here: We need to create 2 masks and combine (AND) them
			// so that pure white and pure black lines are not erased.  We then rotate the
			// mask and AND it with its own rotated mask to get the final mask we use in
			// rendering the output
			RBitmapImage rMask0, rMask1;
			rMask0.Initialize(yBitmapWidth, yBitmapHeight, 1);
			rMask1.Initialize(yBitmapWidth, yBitmapHeight, 1);		
			RIntRect rMaskRect(0, 0, yBitmapWidth, yBitmapHeight);

			// Select the bitmaps into offscreen drawing surfaces
			ROffscreenDrawingSurface rODS0, rODS1;		
			rODS0.SetImage(&rMask0);
			rODS1.SetImage(&rMask1);	

			// Make the first mask black
			rODS0.SetFillColor(RSolidColor(kBlack));
			rODS0.FillRectangle(rMaskRect);

			// Make the second mask white		
			rODS1.SetFillColor(RSolidColor(kWhite));
			rODS1.FillRectangle(rMaskRect);

			// Draw the picture on the offscreen surfaces	
			rVector.Render(rODS0, rMaskRect);
			rVector.Render(rODS1, rMaskRect);

			// Combine the 2 masks into 1 to ensure that pure black and pure 
			// white lines are not lost
			RBitmapImage rMask;
			rMask.Initialize(yBitmapWidth, yBitmapHeight, 1);		
			ROffscreenDrawingSurface rMaskDS;
			rMaskDS.SetImage(&rMask);
			rMaskDS.BlitDrawingSurface(rODS0, rMaskRect, rMaskRect, kBlitSourceCopy);
			rMaskDS.BlitDrawingSurface(rODS1, rMaskRect, rMaskRect, kBlitMergePaint);					
			rMaskDS.ReleaseImage();
			rODS0.ReleaseImage();
			rODS1.ReleaseImage();

			// Scale the transform so that we are mapping pixels to device units	
			R2dTransform rBitmapTransform = rTransform;
			rBitmapTransform.PreScale((YFloatType)rSize.m_dx / (YFloatType)rBitmap.GetWidthInPixels(), (YFloatType)rSize.m_dy / (YFloatType)rBitmap.GetHeightInPixels());
			
			// Translate the origin so that the rotated bitmap is mapped into the right
			// place in the destination bitmap
			rBitmapTransform.PostTranslate(-rDeviceRect.m_Left, -rDeviceRect.m_Top);
			
			// Transform the bitmaps (we reuse the rMask0 and rMask1 bitmaps because we're done
			// with them above)
			m_rLibrary.Transform(rBitmap, m_rProxyBitmap, rMask0, rBitmapTransform);			
			m_rLibrary.Transform(rMask, m_rMaskBitmap, rMask1, rBitmapTransform);

			// AND rMask1 and m_rMaskBitmap together (we reuse rODS0 and rODS1 because we're
			// also done with them above)		
			RIntRect rMaskBitmapRect(0, 0, m_rMaskBitmap.GetWidthInPixels(), m_rMaskBitmap.GetHeightInPixels());
			rODS0.SetImage(&m_rMaskBitmap);
			rODS1.SetImage(&rMask1);
			rODS0.BlitDrawingSurface(rODS1, rMaskBitmapRect, rMaskBitmapRect, kBlitSourceAnd);
			rODS0.ReleaseImage();
			rODS1.ReleaseImage();

			// Set the history members
			m_rLastSize = rSize;
			m_rLastTransform.MakeIdentity();
		}
		
		// Display the rotated image with the mask		
		m_rProxyBitmap.RenderWithMask(rDS, m_rMaskBitmap, rDeviceRect);
	}
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::CreateView( )
//
//  Description:		Creates a new view on this document
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RComponentView* RImageDocument::CreateView(const YComponentBoundingRect& rBoundingRect, RView* pParentView)
{
	return new RImageView(rBoundingRect, this, pParentView);
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::Read()
//
//  Description:		Read in a bitmap from storage
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::Read(RChunkStorage& rStorage, EReadReason eReason)
{		
	// Call the base method
	RComponentDocument::Read(rStorage, eReason);

	// REVIEW 5/4/97 GKB We shouldn't need to do this since we should
	// only be calling Read() on an empty (uninitialized) component.
	m_fPreview = FALSE;
	m_fEditResolution = FALSE;

	// Search for image chunks and instantiate appropriately	
	RChunkStorage::RChunkSearcher rSearcher;
	rSearcher = rStorage.Start(kImageChunkID, !kRecursive);
	if (!rSearcher.End())
	{
		// Read the effects
		rStorage >> m_rEffects.m_nBrightness;
		rStorage >> m_rEffects.m_nContrast;
		rStorage >> m_rEffects.m_nSharpness;
		rStorage >> m_rEffects.m_rCropArea;

		// Create and read in the buffer
		rStorage >> m_rInternalDataBuffer;
		if (m_rInternalDataBuffer.GetBufferSize() > 0)
			ApplyImageEffects();

		rStorage.SelectParentChunk();
	}	

	rSearcher = rStorage.Start(kImagePathChunkID, !kRecursive);
	if (!rSearcher.End())
	{
		// Read the path
		rStorage >> m_rPath;
		rStorage.SelectParentChunk();
	}

	rSearcher = rStorage.Start(kImageCropPathID, !kRecursive);
	if (!rSearcher.End())
	{

		RBitmapImage* pImage = dynamic_cast<RBitmapImage*>( m_pImage );

		if (pImage)
		{
			// Read the crop path
			RClippingPath rPath;
			rStorage >> rPath;
			pImage->SetRPath( &rPath );
		}

		rStorage.SelectParentChunk();
	}


	rSearcher = rStorage.Start(kImageFrameFillColor, !kRecursive);
	if (!rSearcher.End())
	{
		// Read the fill color...
		rStorage >> m_FrameFillColor;
		rStorage.SelectParentChunk();
	}

	// Search for property information	
	//
	rSearcher = rStorage.Start(kPropertyChunkID, !kRecursive);

	if (!rSearcher.End())
	{
		rStorage >> m_rTintColor;
		rStorage.SelectParentChunk();
	}	

	// MCK 6/2/98 - Hack Alert!
	// If the following TAG in the archive exists, we MUST be
	// a photo frame, because we only WRITE it if we have
	// the component type of kPhotoFrameComponent.
	// When we find this tag in our archive stream, we
	// reset our component type to kPhotoFrameComponent.
	//
	rSearcher = rStorage.Start(kImageFrameFillIdent, !kRecursive);
	if (!rSearcher.End())
	{
		DWORD	dwUnused;
		rStorage >> dwUnused;
		rStorage.SelectParentChunk();
		SetComponentType( kPhotoFrameComponent );
	}

	// REVIEW GCB 6/12/98 - workaround for bad save-by-ref QSLs
	// these are the QSLs that bug 1190 refers to that take you to the Import tab
	// this code should be removed after the QSLs have been resaved to fix this
	RPSDPhotoDocument* thisRPSDPhotoDocument = dynamic_cast<RPSDPhotoDocument*>(this);
	if (!thisRPSDPhotoDocument && m_rInternalDataBuffer.GetBufferSize() == 0 && !m_rPath.IsEmpty())
	{
		char	szDrive[_MAX_DRIVE];
		char	szDir  [_MAX_DIR];
		char	szFname[_MAX_FNAME];
		char	szExt  [_MAX_EXT];

		::_splitpath((LPCSTR)m_rPath, szDrive, szDir, szFname, szExt);

		char* pszDir = szDir;
		char* photosStr = "\\Photos\\";

		if (::strnicmp( pszDir, photosStr, ::strlen(photosStr) ) == 0)
		{
			pszDir += ::strlen( photosStr );

			RMBCString m_rPartialPath = RMBCString(pszDir) + RMBCString(szFname) + RMBCString(szExt);

			BOOL success = TRUE;
			BOOL keepTrying = TRUE;
			try
			{
				RMBCString pathPrefix;
				RCollectionManager::TheCollectionManager().UpdateImageByReferencePathPrefix( NULL );
				for (; keepTrying; )
				{
					if (RCollectionManager::TheCollectionManager().GetImageByReferencePathPrefix( NULL, pathPrefix ))
						keepTrying = FALSE;
					else
					{
						if (RAlert().AskUserForCdSwap() == kAlertCancel)
							keepTrying = success = FALSE;
						else
							RCollectionManager::TheCollectionManager().UpdateImageByReferencePathPrefix( NULL );
					}
				}
				// If we succeeded, load the image
				if (success)
				{
					RMBCString fullPath(pathPrefix);
					fullPath += m_rPartialPath;

					// HACK ALERT!!!!!
					// Because some of the images in QSLs were not resaved when the
					// PHOTO directories were changed, we have to special case the following
					// directories:
					// 
					//     Major Class             DOS Old             DOS New
					// 
					// 1.  Crafts & Hobbies:       Craft&Hob           Craft&Ho
					// 2.  People/Famous People    FamPeople           FamPeopl
					// 3.  People/Fictitious       Fictitious          Fiction
					// 4.  Animals/Fish&Sea        Fish&Sea            Aquatic
					// 5.  Rec&Lei/Bea&Swim        Beachoc             Bea&Swim
					// 6.  Architecture/Struct     Otherst             Struct
					//

					char	szDrive[_MAX_DRIVE];
					char	szDir  [_MAX_DIR];
					char    szDirN [_MAX_DIR];
					char	szFname[_MAX_FNAME];
					char	szExt  [_MAX_EXT];

					_splitpath((LPCSTR)fullPath, szDrive, szDir, szFname, szExt);

					char szOldPath1[] = "\\Photos\\Craft&Hob" ;
					char szNewPath1[] = "\\Photos\\Craft&Ho " ;

					char szOldPath2[] = "\\Photos\\People\\FamPeople" ;
					char szNewPath2[] = "\\Photos\\People\\FamPeopl " ;

					char szOldPath3[] = "\\Photos\\People\\Fictitious";
					char szNewPath3[] = "\\Photos\\People\\Fiction   ";

					char szOldPath4[] = "\\Photos\\Animals\\Fish&Sea";
					char szNewPath4[] = "\\Photos\\Animals\\Aquatic ";

					char szOldPath5[] = "\\Photos\\Rec&Lei\\Beachoc";
					char szNewPath5[] = "\\Photos\\Rec&Lei\\Bea&Swim\\";

					char szOldPath6[] = "\\Photos\\Arch\\Otherst";
					char szNewPath6[] = "\\Photos\\Arch\\Struct ";

					if (strnicmp(szOldPath1, szDir, strlen(szOldPath1)) == 0)
					{
						strncpy(szDir, szNewPath1, strlen(szNewPath1));
					}
					else if (strnicmp(szOldPath2, szDir, strlen(szOldPath2)) == 0)
					{
						strncpy(szDir, szNewPath2, strlen(szNewPath2));
					}
					else if (strnicmp(szOldPath3, szDir, strlen(szOldPath3)) == 0)
					{
						strncpy(szDir, szNewPath3, strlen(szNewPath3));
					}
					else if (strnicmp(szOldPath4, szDir, strlen(szOldPath4)) == 0)
					{
						strncpy(szDir, szNewPath4, strlen(szNewPath4));
					}
					else if (strnicmp(szOldPath5, szDir, strlen(szOldPath5)) == 0)
					{
						strncpy(szDir, szNewPath5, strlen(szNewPath5));
					}
					else if (strnicmp(szOldPath6, szDir, strlen(szOldPath6)) == 0)
					{
						strncpy(szDir, szNewPath6, strlen(szNewPath6));
					}

					size_t	c1=0, c2=0;

					for (c1=0; c1 < strlen(szDir); c1++)
					{
						if (szDir[c1] != ' ')
							szDirN[c2++] = szDir[c1];
					}
					szDirN[c2] = '\0';

					fullPath  = RMBCString(szDrive);
					fullPath += RMBCString(szDirN);
					fullPath += RMBCString(szFname);
					fullPath += RMBCString(szExt);

					//
					// END HACK ALERT!!!!!
					//

					RStorage storage( fullPath, kRead );
					YStreamLength fileSize = storage.GetStorageStream()->GetSize();
					YException hException;
					Load( storage, fileSize, hException, NULL, FALSE );
				}
			}
			catch (...)
			{
				RAlert().AlertUser( STRING_ERROR_OPENIMGERR_FILENOTFOUND );
			}
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::Write()
//
//  Description:		Write out a bitmap to storage
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::Write(RChunkStorage& rStorage, EWriteReason reason) const
{
	TpsAssert(!m_fPreview, "Calling Write() on an Image component containing a preview image.");
	TpsAssert(!m_fEditResolution, "Calling Write() on an Image component using Edit resolution.");

	// Call the base member
	RComponentDocument::Write(rStorage, reason);

	// If the image buffer is not already in compressed format, we need to compressed it now
	if( const_cast<RImageDocument*>( this )->IsCompressed( ) == FALSE )
		const_cast<RImageDocument*>( this )->CompressInternalData( );

	rStorage.AddChunk(kImageChunkID);
	{
		// Write the effects
		rStorage << m_rEffects.m_nBrightness;
		rStorage << m_rEffects.m_nContrast;
		rStorage << m_rEffects.m_nSharpness;
		rStorage << m_rEffects.m_rCropArea;

		if (m_fLinkedData || reason == kPrintShopQslPhotoSaveByRef)
		{
			// Data is saved elsewhere so 
			// write out an empty buffer.
			RTempFileBuffer emptyBuffer;
			rStorage << emptyBuffer;
		}
		else
		{
			// Write out the image buffer	
			rStorage << m_rInternalDataBuffer;
		}
	}
	rStorage.SelectParentChunk();

	rStorage.AddChunk(kImagePathChunkID);
	{
		rStorage << m_rPath;
	}
	rStorage.SelectParentChunk();

	rStorage.AddChunk(kImageFrameFillColor);
	{
		rStorage << m_FrameFillColor;
	}
	rStorage.SelectParentChunk();

	// Write object properties
	//
	rStorage.AddChunk( kPropertyChunkID );
	rStorage << m_rTintColor;
	rStorage.SelectParentChunk();

	// MCK 6/2/98 - Hack Alert
	// Because of a copy/paste bug with PressWriter, we need 
	// to "lie" about being a Photo Frame in certain serialization
	// instances. This essentially unused tag is ONLY written
	// if the component type is a kPhotoFrameComponent so that
	// we can reset the component type correctly when 
	// READING.

	if (GetComponentType() == kPhotoFrameComponent)
	{
		rStorage.AddChunk(kImageFrameFillIdent);
		{
			DWORD	dwUnused = 0;
			rStorage << dwUnused;
		}
		rStorage.SelectParentChunk();
	}


	RBitmapImage* pImage = dynamic_cast<RBitmapImage*>( m_pImage );

	if (pImage)
	{
		// Read the crop path
		RClippingPath* pPath = pImage->GetClippingRPath();

#if 0	//	commented this out, because the image may have been altered in the
		//	IED, in which case it lost it's ability to reload from disk, but it
		//	still has a clipping path to retain; the PathFromImport flag ought
		//	to still be a valid way to discern the clipping path's origin - jdf
		if (!pImage->PathFromImport() && pPath)
		{
			rStorage.AddChunk(kImageCropPathID);
			pPath->Write( rStorage );
			rStorage.SelectParentChunk();
		}
#endif
		if ( pPath && pPath->IsDefined() )
		{
			rStorage.AddChunk( kImageCropPathID );
			rStorage << *pPath;
			rStorage.SelectParentChunk();
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::ReleaseResources()
//
//  Description:		Free up all GDI resources that can be reacquired later
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RImageDocument::ReleaseResources( ) 
{
	//	Free our proxy and release the primary image; m_pImage is either a 
	//	dibsection or metafile.
	ResetProxy( );
	delete m_pImage;
	
	//	Clean up the unused image pointer.
	m_pImage = NULL;

	//	Remember to reload the image data.
	m_fResourcesReleased = TRUE;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::InvalidateAllViews( )
//
//  Description:		Invalidate all of the views associated with this document.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageDocument::InvalidateAllViews( BOOLEAN fErase )
{
	RComponentDocument::InvalidateAllViews( fErase );

	// Something significant changed, 
	// so reset the the proxy
	ResetProxy( );
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::ResetToDefaultSize()
//
//  Description:		Resets this component to be the default size for the
//							specified parent document
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RImageDocument::ResetToDefaultSize(RDocument*)
{
	RImage *pImage = GetImage();
	if (pImage)
	{
		ResizeView(pImage->GetSizeInLogicalUnits(), TRUE);
	}
}


// ****************************************************************************
//
//  Function Name:	RImageDocument::GetDefaultSize()
//
//  Description:		Gets the default size for the loaded image.
//
//  Returns:			TRUE on success, FALSE if the image can't be found.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RImageDocument::GetDefaultSize( const RRealSize &sizePanel, RRealSize *psizeDefault )
{
	RImage *pImage = GetImage();
	if (pImage)
	{
		*psizeDefault = pImage->GetSizeInLogicalUnits();
		return TRUE;
	}

	return FALSE;
}


// ****************************************************************************
//
//  Function Name:	RImageDocument::Paste( )
//
//  Description:		Retrieves data in the specified format from the clipboard
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
void RImageDocument::Paste(YDataFormat yDataFormat, const RDataTransferSource& rDataTransferSource)
{	
	// Delete the previous image
	delete m_pImage;
	m_pImage = NULL;

	RImage* pNewImage = NULL;
	switch(yDataFormat)
	{
	case kDibFormat:
		{
			pNewImage = new RBitmapImage(TRUE);						
		}
		break;

	case kPictureFormat:
	case kEnhancedMetafileFormat:
		{
			pNewImage = new RVectorImage;
		}
		break;

	default:
		TpsAssertAlways("Invalid format given for paste.");		
	}
	
	try
	{
		// Load the image data from the clipboard
		pNewImage->Paste(rDataTransferSource, yDataFormat);

		// Set the image into the component
		SetImage(*pNewImage);

		// Delete the pasted image since SetImage() already made a copy of it
		delete pNewImage;
	}
	catch(YException)
	{
		delete pNewImage;
		throw;
	}
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::GetImage()
//
//  Description:		Returns m_pImage
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RImage* RImageDocument::GetImage()
{
	// Ensure that the image resources haven't been freed.
	if (m_fResourcesReleased)
	{
		try
		{
			ApplyImageEffects();
		}
		catch(YException)
		{
			// The image may be a placeholder, don't crash.
			m_fResourcesReleased = FALSE;	// Don't cycle on this.
		}
	}
	return m_pImage;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::SetImage()
//
//  Description:		Sets a COPY of the image in rImage into the component.  We 
//							use a copy because if the original image was 16 - 32bpp, it must
//							be converted to a JPEG and re-constructed.  To maintain 
//							consistency, we require ALL images to be copied.
//
//  Returns:			
//
//  Exceptions:		RImageLibrary exceptions
//
// ****************************************************************************
void RImageDocument::SetImage(IBufferInterface* pImageData, BOOLEAN fResize, BOOLEAN fLinkData)
{
	pImageData->Read( m_rInternalDataBuffer );
	pImageData->GetFileName( m_rPath );

	m_fLinkedData = fLinkData;
	m_fPreview = FALSE;

	// Recreate the DIB section	
	ResetImageEffects();
	ApplyImageEffects();

	if (fResize)
	{
		RImage *pImage = GetImage();
		if (pImage) //j
			ResizeView(pImage->GetSizeInLogicalUnits(), TRUE);
	}
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::SetImage()
//
//  Description:		Sets a COPY of the image in rImage into the component.  We 
//							use a copy because if the original image was 16 - 32bpp, it must
//							be converted to a JPEG and re-constructed.  To maintain 
//							consistency, we require ALL images to be copied.
//
//  Returns:			
//
//  Exceptions:		RImageLibrary exceptions
//
// ****************************************************************************
void RImageDocument::SetImage(const RImage& rImage, BOOLEAN fResize, BOOLEAN fRetainPathname)
{
	// Need to create this non-const temporary or we do not compile under VC4
	RImage* pTemp = const_cast<RImage*>( &rImage );

	// save the clipping path
	RClippingPath  rClippingPath;
	RClippingPath* pClippingPath = NULL;
	
//	RBitmapImage* pIncomingBitmap = dynamic_cast<RBitmapImage*>(pTemp);
//	if( pIncomingBitmap )
//	{
//		pClippingPath = pIncomingBitmap->GetClippingRPath();
//	}

	// If its a bitmap image, get the raw data and copy it into the buffer
	if( dynamic_cast<RBitmapImage*>( pTemp ) )
		{
		const RBitmapImage* pBitmap = static_cast<const RBitmapImage*>( &rImage );
		m_rInternalDataBuffer.Resize( pBitmap->GetRawDataSize( ) );
		memcpy( m_rInternalDataBuffer.GetBuffer( ), pBitmap->GetRawData( ), pBitmap->GetRawDataSize( ) );

		pClippingPath = ((RBitmapImage *) pTemp)->GetClippingRPath();

		if (pClippingPath /*&& m_pImage == &rImage*/)
			{
			rClippingPath = *pClippingPath;
			pClippingPath = &rClippingPath;
			}
		}

	// If its a vector image, export it as a metafile directly into the buffer
	else if( dynamic_cast<const RVectorImage*>( pTemp ) )
		m_rLibrary.ExportImage( const_cast<RImage&>( rImage ), m_rInternalDataBuffer, kImageFormatEMF );

	else
		TpsAssertAlways( "Unknown image format." );

	// Recreate the DIB section	
	ResetImageEffects();
	ApplyImageEffects( pClippingPath );
	RImage *pImage = GetImage();
	if (fResize)
		ResizeView(pImage->GetSizeInLogicalUnits(), TRUE);

	// copy the clipping path
/*	if ( pClippingPath && pClippingPath->IsDefined() )
	{
		RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(pImage);
		if (!pBitmap->SetRPath( pClippingPath ))
		{
			;//TRACE("Failed to set a new path in the image\n\r");
		}
	}
*/
	// We don't know where this image came from, so set m_fPreview to FALSE and empty the path
	m_fPreview = FALSE;
	m_fLinkedData = FALSE;

//	m_rPath.Empty();
	if ( ! fRetainPathname )
	{
		m_rPath.Empty();
	}
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::IsCompressed()
//
//  Description:		Determines if the internal data buffer is in compressed
//							format.
//
//  Returns:			
//
//  Exceptions:		RImageLibrary exceptions
//
// ****************************************************************************
BOOLEAN RImageDocument::IsCompressed()
	{
	RImage *pImage = GetImage();
	// If the image is a vector image, its considered compressed
	if( pImage && dynamic_cast<RVectorImage*>( pImage ) )
		return TRUE;
	
	// Get the size of the buffer
	// If 0 then we have null data - a placeholder - Image is considered compressed
	if(m_rInternalDataBuffer.GetBufferSize() == 0)
		return TRUE;

	// Otherwise check to see if its a JPG, PNG or GIF
	EImageFormat imageFormat = m_rLibrary.GetImageFormat( m_rInternalDataBuffer );
	if( imageFormat == kImageFormatJPG ||
		imageFormat == kImageFormatPNG ||
		imageFormat == kImageFormatGIF )
		return TRUE;

	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RImageDocument::CompressInternalData()
//
//  Description:		Converts the internal image buffer to a JPEG.
//
//  Returns:			
//
//  Exceptions:		RImageLibrary exceptions
//
// ****************************************************************************
void RImageDocument::CompressInternalData( )
{
	// Import the buffer into a temp image
	RImage* pTempImage = m_rLibrary.ImportImage( m_rInternalDataBuffer );

	// Determine the format of the image to export into
	EImageFormat eInternalDataFormat = kImageFormatUnknown;
	if (eInternalDataFormat == kImageFormatUnknown)
	{		
		RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>(pTempImage);
		if (pBitmap)
		{
			switch(pBitmap->GetBitDepth())
			{
			case 1:
			case 4:
			case 8:				
				eInternalDataFormat = kImageFormatPNG;
				break;

			case 16:
			case 32:
				// Accusoft does not support 16bpp or 32bpp images.  If the image is a 16bpp 
				// or 32bpp bitmap, we need to convert it to 24bpp here
				m_rLibrary.ChangeBitDepth(*pBitmap, 24);
				eInternalDataFormat = kImageFormatJPG;
				break;

			case 24:				
				eInternalDataFormat = kImageFormatJPG;
				break;

			default:
				TpsAssertAlways("Unsupported bit depth in RImageDocument::CompressInternalData().");
				eInternalDataFormat = kImageFormatBMP;
			}
		}
	}

	if (eInternalDataFormat == kImageFormatUnknown)
	{
		if (dynamic_cast<RVectorImage*>(pTempImage)) eInternalDataFormat = kImageFormatEMF;
	}

	TpsAssert(eInternalDataFormat != kImageFormatUnknown, "Unsupported image type in RImageDocument::CompressInternalData().");		

	// Export the image into the internal data buffer
	if (!m_rLibrary.ExportImage(*pTempImage, m_rInternalDataBuffer, eInternalDataFormat)) throw m_rLibrary.GetLastException();

	// Cleanup
	delete pTempImage;

	// Recreate the DIB section	
	ApplyImageEffects();

	// Invalidate
	InvalidateAllViews( );
	}

// ****************************************************************************
//
//  Function Name:	RImageDocument::Export()
//
//  Description:	Assumes that the data contained in the RStorage object
//						is in a valid image format and simply exports the
//						data from the storage object to the file in the 
//						specified format
//
//  Returns:		TRUE if successful; otherwise FALSE	
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN	RImageDocument::Export(const RMBCString& rPath, EImageFormat eFormat)
{
	RImageLibrary rLibrary ;
	return rLibrary.ExportImage( m_rInternalDataBuffer, rPath, eFormat ) ;
}

// unreferenced parameters
#pragma warning( disable : 4100 )

// ****************************************************************************
//
//  Function Name:	RImageDocument::Load()
//
//  Description:		Assumes that the data contained in the RStorage object
//							is in the appropriate format to be stored in the user data 
//							file (i.e. the internal data buffer) and simply copies the
//							data from the storage object into the buffer.
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageDocument::Load(RStorage& rStorage, uLONG uLength, YException& hException, RMBCString* partialPath /* = NULL */, BOOLEAN fResizeToDefaultSize /* = TRUE */ )
{
	// Delete the previous image
	delete m_pImage;
	m_pImage = NULL;

	try
	{
		m_rInternalDataBuffer.Resize(uLength);
		rStorage.Read(uLength, m_rInternalDataBuffer.GetBuffer());
		
		ResetImageEffects();
		ApplyImageEffects();
		if (fResizeToDefaultSize)
		{
			RImage *pImage = GetImage();
			ResizeView(pImage->GetSizeInLogicalUnits(), TRUE);
		}
	}
	catch(YException hEx)
	{
		hException = hEx;
		return FALSE;
	}

	m_fPreview = FALSE;
	m_rPath.Empty();
	return TRUE;
}

// Unreferenced parameters
#pragma warning( default : 4100 )

// ****************************************************************************
//
//  Function Name:	RImageDocument::Load()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageDocument::Load(const RMBCString& rPath, YException& hException)
{
	//	Delete the previous image
	//	delete m_pImage;
	//	m_pImage = NULL;

	m_rPath.Empty();

	try
	{
		if (kImageFormatFPX == m_rLibrary.GetImageFormat( rPath ))
		{
			RImage* pNewImage = m_rLibrary.ImportImage( rPath );
			if (!pNewImage) throw m_rLibrary.GetLastException() ;

			RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>( pNewImage );

			if (!pBitmap || (pBitmap->GetBitDepth() != 8 && pBitmap->GetBitDepth() != 24))
			{
				SetImage( *pNewImage, TRUE );
				delete pNewImage;
			}
			else
			{
				EImageFormat eFormat( pBitmap ? kImageFormatBMP : kImageFormatEMF ) ;
				m_rLibrary.ExportImage( *pNewImage, m_rInternalDataBuffer, eFormat );

				delete m_pImage;
				m_pImage  = pNewImage ;

				ResizeView(m_pImage->GetSizeInLogicalUnits(), TRUE);			
			}

			return TRUE;
		}

		// Load the image into m_rInternalDataBuffer
		RImportFileBuffer rImportBuffer(rPath);
		m_rInternalDataBuffer.Resize(rImportBuffer.GetBufferSize());
		memcpy(m_rInternalDataBuffer.GetBuffer(), rImportBuffer.GetBuffer(), rImportBuffer.GetBufferSize());

		ResetImageEffects();
		ApplyImageEffects();

		ResizeView(m_pImage->GetSizeInLogicalUnits(), TRUE);			
	}
	catch(YException hEx)
	{			
		hException = hEx;

		return FALSE;
	}

	m_fPreview = FALSE;
	m_rPath = rPath ;

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::LoadForPreview()
//
//  Description:		Loads the image from the given file but limits it to the 
//							maximum size given.  Used for loading previews
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageDocument::LoadForPreview(const RMBCString& rPath, const RIntSize& rMaxSize, YException& hException)
{

	// Delete the previous image
	delete m_pImage;
	m_pImage = NULL;
	m_rPath  = rPath;

	RImage* pNewImage = NULL ;
	
	try
	{
		// NOTE	LoadForPreview() cannot and does not differentiate between image types
		//			when loading previews.  A smaller version of the image data is loaded 
		//			directly from the image file which results in a much smaller load time
		//			than if the full size image was actually loaded in and then reduced in 
		//			size.  As such, when an image is loaded for preview, it will not be in
		//			the appropriate format for a Renaissance user data file.  The Load() 
		//			function must be called to retrieve the full size image and store it 
		//			in the appropriate internal format.
		if (m_rLibrary.IsGifFile( rPath ))
		{
			// Load the image into a buffer so that the load is buffer based
			// and is processed correctly to allow for the GIF transparency handling.
			RImportFileBuffer rImportBuffer(rPath);
			pNewImage = m_rLibrary.ImportPreviewImage(rImportBuffer, rMaxSize);
		}
		else
		{
			pNewImage = m_rLibrary.ImportPreviewImage(rPath, rMaxSize);
		}
		if (!pNewImage) throw m_rLibrary.GetLastException() ;

		EImageFormat eFormat( dynamic_cast<RBitmapImage*>( pNewImage ) ? kImageFormatBMP : kImageFormatEMF ) ;
		m_rLibrary.ExportImage( *pNewImage, m_rInternalDataBuffer, eFormat );

		m_pImage  = pNewImage ;
		pNewImage = NULL ;

		ResetImageEffects() ;
// GCB 3/4/98 - the new fast rotation code only supports 8bpp and 24bpp so
// call ApplyImageEffects() (which this routine used to do via SetImage())
// instead of ResetProxy() to ensure proper bit depth of the DIB.
// ApplyImageEffects() calls ResetProxy() so the proxy still gets reset.
#if 0
		ResetProxy() ;
#else
		RBitmapImage* pBitmap = dynamic_cast<RBitmapImage*>( m_pImage );
		if (pBitmap && pBitmap->GetBitDepth( ) != 8 && pBitmap->GetBitDepth( ) != 24)
			ApplyImageEffects();
#endif

		ResizeView(m_pImage->GetSizeInLogicalUnits(), FALSE);	//	Don't fit inside the panel. There is no real panel for preview
	}
	catch(YException hEx)
	{
		if (pNewImage) delete pNewImage;
		hException = hEx;

		return FALSE;
	}

	m_fPreview = TRUE;

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::IsPreview()
//
//  Description:		Returns TRUE if the image was loaded by LoadForPreview()
//							
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageDocument::IsPreview() const
{
	return m_fPreview;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::GetPath( )
//
//  Description:		Returns the path of the image, if loaded from a file
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
const RMBCString&	RImageDocument::GetPath() const
{
	return m_rPath;
}


// ****************************************************************************
//
//  Function Name:	RImageDocument::QueryEditImage( )
//
//  Description:		Returns TRUE if effects can be applied to the image
//							(i.e. the image is a bitmap, not a metafile)
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
BOOLEAN RImageDocument::QueryEditImage()
{
	RImage *pImage = GetImage();
	return (dynamic_cast<RVectorImage*>(pImage)) ? FALSE : TRUE;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::MakeImageEditable( )
//
//  Description:		Converts the image into a format which supports applying effects
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
BOOLEAN RImageDocument::MakeImageEditable()
{
	// If the image is a metafile, we need to convert it to a bitmap before we can edit it.
	RImage *pImage = GetImage();
	RVectorImage* pVector = dynamic_cast<RVectorImage*>(pImage);
	RVectorImage rVector = *pVector;
	if (pVector)
	{
		// Go ahead and convert the image to a bitmap
		const uLONG kBitmapWidth = (uLONG)(pVector->GetWidthInInches() * (YRealDimension)kConvertVectorXDpi);
		const uLONG kBitmapHeight = (uLONG)(pVector->GetHeightInInches() * (YRealDimension)kConvertVectorYDpi);

		RAlert rAlert;
		uLONG uImageSize = RBitmapImage::GetImageDataSize(kBitmapWidth, kBitmapHeight, kConvertedVectorBitDepth);
		if (uImageSize > kWarnConvertVectorSize && rAlert.QueryUser(STRING_WARN_CONVERT_METAFILE_TO_BITMAP) == kAlertNo) return FALSE;

		// Put up a wait cursor while the conversion is in progress
		RWaitCursor rWaitCursor;

		RBitmapImage rBitmap;
		ROffscreenDrawingSurface rODS;
		try
		{
			// Initialize the bitmap
			rBitmap.Initialize(kBitmapWidth, kBitmapHeight, kConvertedVectorBitDepth);
				
			// Copy the vector image onto the proxy		
			rODS.SetImage(&rBitmap);		
			rODS.SetFillColor(RSolidColor(kWhite));
			rODS.FillRectangle(RIntRect(0, 0, kBitmapWidth, kBitmapHeight));		
			pVector->Render(rODS, RIntRect(0, 0, kBitmapWidth, kBitmapHeight));
			rODS.ReleaseImage();

			// Set the bitmap image into the document
			SetImage(rBitmap, FALSE);
		}
		catch(YException nException)
		{
			RAlert rAlert;
			switch(nException)
			{
			case ::kMemory:				
				rAlert.AlertUser(STRING_ERROR_MIE_INSUFFICIENT_MEMORY);				
				break;

			case ::kDiskFull:				
				rAlert.AlertUser(STRING_ERROR_MIE_DISK_FULL);
				break;

			default:				
				rAlert.AlertUser(STRING_ERROR_MIE_GENERIC);
			}

			// Make sure we set the original image back into the document			
			SetImage(rVector);			

			return FALSE;
		}
	}	

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::EnableEditResolution()
//
//  Description:		Tell the document to generate a smaller DIB internally
//							so that edits can be performed more quickly
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::EnableEditResolution(const RIntSize& rMaxEditSize)
{
	TpsAssert(!m_fPreview, "Should not call EnableEditResolution() on a preview image.");
	m_fEditResolution = TRUE;
	
	// Store the unedited image data in a small buffer which can be quickly reloaded
	RImage* pImage = m_rLibrary.ImportPreviewImage(m_rInternalDataBuffer, rMaxEditSize);
	m_rLibrary.ExportImage(*pImage, m_rEditDataBuffer, kImageFormatBMP);
	delete pImage;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::DisableEditResolution()
//
//  Description:		Tell the document not to use the smaller preview image any longer
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::DisableEditResolution()
{	
	m_fEditResolution = FALSE;
	m_rEditDataBuffer.Empty();
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::SetImageEffects()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::SetImageEffects(const RImageEffects& rEffects)
{	
	m_rEffects = rEffects;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::GetImageEffects()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
const RImageEffects& RImageDocument::GetImageEffects()
{
	return m_rEffects;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::ClearImageEffects()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::ResetImageEffects()
{
	// Remove clipping path if there is one
	RBitmapImage*  pBitmap = dynamic_cast<RBitmapImage*>(m_pImage);
	if (pBitmap) pBitmap->DeleteRPath();

	m_rEffects.m_nBrightness =		kDefaultBrightness;
	m_rEffects.m_nContrast =		kDefaultContrast;
	m_rEffects.m_nSharpness =		kDefaultSharpness;
	m_rEffects.m_rCropArea =		kDefaultCropArea;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::ResetProxy()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
void RImageDocument::ResetProxy()
{
	// Reset our internal parameters
	m_rLastSize = RRealSize(0, 0);
	m_rLastTransform.MakeIdentity();
	m_nLastTint = kMaxTint;
	m_rProxyBitmap.Uninitialize();
	m_rMaskBitmap.Uninitialize();
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::ResizeView()
//
//  Description:		Make sure that the image fits on the panel
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::ResizeView(RRealSize rNewViewSize, BOOLEAN fFitInsidePanel)
{
	RComponentView* pComponentView = static_cast<RComponentView*>(GetActiveView());	

	if( pComponentView->GetParentView( ) && fFitInsidePanel )
	{
		// Compare object size to panel size
		RRealSize rPanelSize = pComponentView->GetParentView( )->GetSize();
		if (rNewViewSize.m_dx > rPanelSize.m_dx || rNewViewSize.m_dy > rPanelSize.m_dy) rNewViewSize.FitInside(rPanelSize);	
	}

	// Resize the view
	pComponentView->Resize(rNewViewSize);
}
// ****************************************************************************
//
//  Function Name:	RImageDocument::ApplyImageEffects()
//
//  Description:		
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::ApplyImageEffects( RClippingPath* pClipPath )
{
//j	TpsAssert(m_rInternalDataBuffer.GetBufferSize() > 0, "No internal image data.");
	if (m_rInternalDataBuffer.GetBufferSize() <= 0) //j
		return; //j
	
	// Make a copy of the current images clipping path to use in the effects dlg.

	RClippingPath* pOldPath = NULL; 
	RClippingPath* pNewPath = NULL; 
	RBitmapImage*  pBitmap = dynamic_cast<RBitmapImage*>(m_pImage);

	if (pBitmap && !pClipPath)
	{
		pOldPath = pBitmap->GetClippingRPath();

		if (pOldPath && pOldPath->IsDefined())
		{
			pNewPath = new RClippingPath( *pOldPath );
		}
	}


	// Delete the previous image
	delete m_pImage;
	m_pImage = NULL;
	
	// Reset the proxy
	ResetProxy();
	
	// Re-create the image from the buffer	
	if (m_fEditResolution)
		m_pImage = m_rLibrary.ImportImage(m_rEditDataBuffer);
	else
		m_pImage = m_rLibrary.ImportImage(m_rInternalDataBuffer);
	
//j	if (!m_pImage) throw m_rLibrary.GetLastException();

	pBitmap = dynamic_cast<RBitmapImage*>(m_pImage);

	if (pBitmap)
	{

#ifdef SET_CROPRECT_FOR_IMPORT
		// If there is a path, we need to set the crop 
		// rect, and translate the path to it's origin.
		RClippingPath* pPath = pBitmap->GetClippingRPath();
		if (pPath) SetCropRectFromPath( pPath, TRUE );

		if (!pPath && pClipPath)
		{
			SetCropRectFromPath( pClipPath, TRUE );
			pBitmap->SetRPath( pClipPath );
		}
#else
		if (pClipPath)
		{
			SetCropRectFromPath( pClipPath, TRUE );
			pBitmap->SetRPath( pClipPath );
		}
#endif

		// GCB 3/4/98 - the new fast rotation code only supports 8bpp and 24bpp so
		// if the user has edited the image and it's not 24bpp, convert to 24bpp,
		// otherwise if it's less than 8bpp, convert to 8bpp
		if (m_rEffects.m_nBrightness != kDefaultBrightness ||
			m_rEffects.m_nContrast != kDefaultContrast ||
			m_rEffects.m_nSharpness != kDefaultSharpness && 
			pBitmap->GetBitDepth() != 24)
		{
			m_rLibrary.ChangeBitDepth(*pBitmap, 24);
		}
//		else if (pBitmap->GetBitDepth() < 8)
//		{
//			m_rLibrary.ChangeBitDepth(*pBitmap, 8);
//		}

		// Apply the current effects
		if (pBitmap->GetBitDepth() == 24)
		{
			m_rLibrary.Brighten(*pBitmap, m_rEffects.m_nBrightness); 
			m_rLibrary.Contrast(*pBitmap, m_rEffects.m_nContrast);
			m_rLibrary.Sharpen(*pBitmap, m_rEffects.m_nSharpness);		
		}
		m_rLibrary.Crop(*pBitmap, m_rEffects.m_rCropArea);

		// Copy the copy of the clipping path to the new image
		if (pNewPath)
		{
			if (!pBitmap->SetRPath( pNewPath ))
			{
				;//TRACE("Failed to set a new path in the image\n\r");
			}

			delete pNewPath;
		}
	}	
	
	//	We have now loaded our resources.
	m_fResourcesReleased = FALSE;
}

// ****************************************************************************
//
//  Function Name:	RImageDocument::QueryImageIsAnAnimatedGIF()
//
//  Description:		Provides whether the image referenced by this document is
//							an animated GIF image.
//
//  Returns:			TRUE if the image is animated, FALSE if not.
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageDocument::QueryImageIsAnAnimatedGIF()
{
	BOOLEAN bIsAnimation = FALSE;

	// If we are a bitmap image, we can see if it is an animated GIF.
	RBitmapImage *pImage = dynamic_cast<RBitmapImage *>(GetImage());
	if (pImage != NULL)
	{
		// OK, the image is a bitmap image, that means it could be a GIF file.
		RMBCString rstrFile = GetPath();
		RMBCString rstrExtension;
		if (rstrFile.GetStringLength() > 3)
		{
			rstrExtension = rstrFile.Tail( 3 );
			rstrExtension.ToUpper();
		}
		else
		{
			// This must be an internal file, so put it out as it came in.
			if (pImage->GetBitDepth() > 8)
				rstrExtension = "JPG";
			else
				rstrExtension = "GIF";
		}

		RMBCString rstrGIF( "GIF" );

		if (rstrExtension == rstrGIF)
		{
			// OK, the image is believed to be GIF format, so let's see if it's animated.
			bIsAnimation = m_rLibrary.IsAnimatedGIF( m_rInternalDataBuffer );
		}
	}

	return bIsAnimation;
}


// ****************************************************************************
//
//  Function Name:	RImageDocument::CreateGIFAnimator()
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
CPGIFAnimator * RImageDocument::CreateGIFAnimator()
{
	CPGIFAnimator *pGifAnimator = NULL;

	// If we are a bitmap image, we can see if it is an animated GIF.
	RBitmapImage *pImage = dynamic_cast<RBitmapImage *>(GetImage());
	if (pImage != NULL)
	{
		// Create A Gif Reader based on the internal data buffer.
		pGifAnimator = m_rLibrary.CreateGIFAnimator( m_rInternalDataBuffer );
	}

	return pGifAnimator;
}


// ****************************************************************************
//
//  Function Name:	RImageDocument::QueryIsWebImageFormat()
//
//  Description:		Checks if the internal image buffer is in a Web compatible format.
//
//  Returns:			TRUE if the image is in JPEG or GIF format, FALSE if not.
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RImageDocument::QueryIsWebImageFormat()
{
	BOOLEAN bIsWebCompatible = FALSE;

	// If we are a bitmap image, we can see if it is an animated GIF.
	RBitmapImage *pImage = dynamic_cast<RBitmapImage *>(GetImage());
	if (pImage != NULL)
	{
		// If the image is a vector image, it is obviously not in JPEG or GIF format.
		// So we are going to make sure it isn't in Vector format.
		if( dynamic_cast<RVectorImage*>( pImage ) == NULL )
		{
			// Get the size of the buffer
			// If 0 then we have null data - a placeholder - Image isn't JPG or GIF.
			if(m_rInternalDataBuffer.GetBufferSize() != 0)
			{
				// OK, we have data so check to see if its a JPG or GIF
				EImageFormat imageFormat = m_rLibrary.GetImageFormat( m_rInternalDataBuffer );
				if( imageFormat == kImageFormatJPG || imageFormat == kImageFormatGIF )
				{
					bIsWebCompatible = TRUE;
				}
			}
		}
	}

	return bIsWebCompatible;
}


// ****************************************************************************
//
//  Function Name:	RImageDocument::QueryRawImageSize()
//
//  Description:		Fills in the size pointer provided with the actual size of
//							the raw image data.
//
//							Nothing is done if the image isn't in a format where size
//							is specifically defined.  (Vector format for instance.)
//
//  Returns:			None.
//
//  Exceptions:		
//
// ****************************************************************************
void RImageDocument::QueryRawImageSize( RIntSize *pSize )
{
	// Ensure that we avoid crashing at this level!
	ASSERT( pSize != NULL );
	if (pSize == NULL)
	{
		return;
	}

	// If we are a bitmap image, we can see if it is an animated GIF.
	RBitmapImage *pImage = dynamic_cast<RBitmapImage *>(GetImage());
	if (pImage != NULL)
	{
		// If the image is a vector image, it is obviously not in JPEG or GIF format.
		// So we are going to make sure it isn't in Vector format.
		if( dynamic_cast<RVectorImage*>( pImage ) == NULL )
		{
			// Get the size of the buffer
			// If 0 then we have null data - a placeholder - Image isn't JPG or GIF.
			if(m_rInternalDataBuffer.GetBufferSize() != 0)
			{
				// OK, we have data so check to see if its a JPG or GIF
				RRealSize rSize = m_rLibrary.GetImageDimensions( m_rInternalDataBuffer );
				pSize->m_dx = (int)rSize.m_dx;
				pSize->m_dy = (int)rSize.m_dy;
			}
		}
	}
}


// ****************************************************************************
//
//  Function Name:	RImageDocument::QueryImageFormat()
//
//  Description:		Provides the image format for the raw data buffer.
//
//  Returns:			The format of the image data in the raw buffer.  If the
//							raw data is not in a bitmap image format, then it returns
//							kImageFormatUnknown.
//
//  Exceptions:		
//
// ****************************************************************************
EImageFormat RImageDocument::QueryImageFormat()
{
	EImageFormat eFormat = kImageFormatUnknown;

	// If we are a bitmap image, we can see if it is an animated GIF.
	RBitmapImage *pImage = dynamic_cast<RBitmapImage *>(GetImage());
	if (pImage != NULL)
	{
		// If the image is a vector image, it is obviously not in JPEG or GIF format.
		// So we are going to make sure it isn't in Vector format.
		if( dynamic_cast<RVectorImage*>( pImage ) == NULL )
		{
			// Get the size of the buffer
			// If 0 then we have null data - a placeholder - Image isn't JPG or GIF.
			if(m_rInternalDataBuffer.GetBufferSize() != 0)
			{
				// OK, we have data so check to see if its a JPG or GIF
				eFormat = m_rLibrary.GetImageFormat( m_rInternalDataBuffer );
			}
		}
	}

	return eFormat;
}


// ****************************************************************************
//
//  Function Name:	RImageDocument::SetCropRectFromPath( )
//
//  Description:		Sets the clipping path into the components
//                   image.
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
//
void RImageDocument::SetCropRectFromPath( RPath* path, BOOLEAN fUpdatePath )
{
	// Get a pointer to the image document's image.  Note,
	// we should be able to assume that it is a RBitmapImage,
	// as this interface is only provided by image components
	// with RBitmapImages.
	RBitmapImage* pImage = dynamic_cast<RBitmapImage*>( GetImage() );
	TpsAssert( pImage, "Invalid image component!" );

#ifdef SET_CROPRECT_FOR_IMPORT
	if (path)
	{
#else
	// Currently only user-defined clipping paths should set the crop
	// rect.  Imported clip rects do not crop to the clipping path
	// bounding rect...for now.
	if (path && !pImage->PathFromImport())
	{
#endif
		RIntRect rRect( path->GetBoundingRect() );
//		RRealSize rSize( pImage->GetWidthInPixels(), pImage->GetHeightInPixels() );
		RRealSize rSize = RImageLibrary().GetImageDimensions(	m_rInternalDataBuffer );

		// Determine the cropping rect.  Remember to
		// acount for the path being upside down.
//		m_rEffects.m_rCropArea.m_Left   = rRect.m_Left / rSize.m_dx;
//		m_rEffects.m_rCropArea.m_Top    = (rSize.m_dy - rRect.Height() - rRect.m_Top) / rSize.m_dy;
//		m_rEffects.m_rCropArea.m_Right  = rRect.Width() / rSize.m_dx;
//		m_rEffects.m_rCropArea.m_Bottom = rRect.Height() / rSize.m_dy;
		m_rEffects.m_rCropArea.m_Left   = max( rRect.m_Left / rSize.m_dx, 0.0 );
		m_rEffects.m_rCropArea.m_Top    = max( (rSize.m_dy - rRect.Height() - rRect.m_Top) / rSize.m_dy, 0.0 );
		m_rEffects.m_rCropArea.m_Right  = min( rRect.Width() / rSize.m_dx, 1.0 );
		m_rEffects.m_rCropArea.m_Bottom = min( rRect.Height() / rSize.m_dy, 1.0 );
		
		R2dTransform xform;
		xform.PostTranslate( -rRect.m_Left, -rRect.m_Top );
//			-rEffects.m_rCropArea.m_Left * rSize.m_dx,
//			-rEffects.m_rCropArea.m_Top  * rSize.m_dy );

		if (fUpdatePath)
		{
			*path = RPath( *path, xform );
			path->SetPathSize( RIntSize(
				m_rEffects.m_rCropArea.m_Right  * rSize.m_dx,
				m_rEffects.m_rCropArea.m_Bottom * rSize.m_dy ) );
		}
			
	}
	else if (!pImage->PathFromImport())
		m_rEffects.m_rCropArea = kDefaultCropArea;
}

#ifdef	TPSDEBUG
// ****************************************************************************
//
//  Function Name:	RImageDocument::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ***************************************************************************
void RImageDocument::Validate() const
{
	RComponentDocument::Validate();
	TpsAssertIsObject(RImageDocument, this);
}									
#endif TPSDEBUG


// ****************************************************************************
//
//  Function Name:	RPSDPhotoDocument::RPSDPhotoDocument()
//
//  Description:		Constructor
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RPSDPhotoDocument::RPSDPhotoDocument(RApplication* pApp, const RComponentAttributes& rComponentAttributes, const YComponentType& componentType, BOOLEAN fLoading)
	: RImageDocument(pApp, rComponentAttributes, componentType, fLoading)
{	
}


// ****************************************************************************
//
//  Function Name:	RPSDPhotoDocument::Load()
//
//  Description:		Assumes that the data contained in the RStorage object
//							is in the appropriate format to be stored in the user data 
//							file (i.e. the internal data buffer) and simply copies the
//							data from the storage object into the buffer.
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RPSDPhotoDocument::Load(RStorage& rStorage, uLONG uLength, YException& hException, RMBCString* partialPath /* = NULL */, BOOLEAN fResizeToDefaultSize /* = TRUE */)
{
	BOOLEAN retVal = RImageDocument::Load(rStorage, uLength, hException, partialPath, fResizeToDefaultSize);
	if (partialPath)
		m_rPartialPath = *partialPath;
	return retVal;
}


// ****************************************************************************
//
//  Function Name:	RPSDPhotoDocument::Read()
//
//  Description:		Read in a bitmap from storage
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RPSDPhotoDocument::Read(RChunkStorage& rStorage, EReadReason eReason)
{		
	// Call the inherited method
	RImageDocument::Read(rStorage, eReason);

	// Read the QSL save-by-ref partial path, if any
	RChunkStorage::RChunkSearcher rSearcher = rStorage.Start(kPartialPathChunkID, !kRecursive);
	if (!rSearcher.End())
	{
		rStorage >> m_rPartialPath;
		rStorage.SelectParentChunk();
	}

	// if image buffer size==0 and there's a QSL save-by-ref partial path,
	// try to load from the partial path
	if (m_rInternalDataBuffer.GetBufferSize() == 0 && !m_rPartialPath.IsEmpty())
	{
		BOOL success = TRUE;
		BOOL keepTrying = TRUE;
		try
		{
			RMBCString pathPrefix;
			RCollectionManager::TheCollectionManager().UpdateImageByReferencePathPrefix( NULL );
			for (; keepTrying; )
			{
				if (RCollectionManager::TheCollectionManager().GetImageByReferencePathPrefix( NULL, pathPrefix ))
					keepTrying = FALSE;
				else
				{
					if (RAlert().AskUserForCdSwap() == kAlertCancel)
						keepTrying = success = FALSE;
					else
						RCollectionManager::TheCollectionManager().UpdateImageByReferencePathPrefix( NULL );
				}
			}
			// If we succeeded, load the image
			if (success)
			{
				RMBCString fullPath(pathPrefix);
				fullPath += m_rPartialPath;

				// HACK ALERT!!!!!
				// Because some of the images in QSLs were not resaved when the
				// PHOTO directories were changed, we have to special case the following
				// directories:
				// 
				//     Major Class             DOS Old             DOS New
				// 
				// 1.  Crafts & Hobbies:       Craft&Hob           Craft&Ho
				// 2.  People/Famous People    FamPeople           FamPeopl
				// 3.  People/Fictitious       Fictitious          Fiction
				// 4.  Animals/Fish&Sea        Fish&Sea            Aquatic
				// 5.  Rec&Lei/Bea&Swim        Beachoc             Bea&Swim
				// 6.  Architecture/Struct     Otherst             Struct
				//

				char	szDrive[_MAX_DRIVE];
				char	szDir  [_MAX_DIR];
				char    szDirN [_MAX_DIR];
				char	szFname[_MAX_FNAME];
				char	szExt  [_MAX_EXT];

				_splitpath((LPCSTR)fullPath, szDrive, szDir, szFname, szExt);

				char szOldPath1[] = "\\Photos\\Craft&Hob" ;
				char szNewPath1[] = "\\Photos\\Craft&Ho " ;

				char szOldPath2[] = "\\Photos\\People\\FamPeople" ;
				char szNewPath2[] = "\\Photos\\People\\FamPeopl " ;

				char szOldPath3[] = "\\Photos\\People\\Fictitious";
				char szNewPath3[] = "\\Photos\\People\\Fiction   ";

				char szOldPath4[] = "\\Photos\\Animals\\Fish&Sea";
				char szNewPath4[] = "\\Photos\\Animals\\Aquatic ";

				char szOldPath5[] = "\\Photos\\Rec&Lei\\Beachoc";
				char szNewPath5[] = "\\Photos\\Rec&Lei\\Bea&Swim\\";

				char szOldPath6[] = "\\Photos\\Arch\\Otherst";
				char szNewPath6[] = "\\Photos\\Arch\\Struct ";

				if (strnicmp(szOldPath1, szDir, strlen(szOldPath1)) == 0)
				{
					strncpy(szDir, szNewPath1, strlen(szNewPath1));
				}
				else if (strnicmp(szOldPath2, szDir, strlen(szOldPath2)) == 0)
				{
					strncpy(szDir, szNewPath2, strlen(szNewPath2));
				}
				else if (strnicmp(szOldPath3, szDir, strlen(szOldPath3)) == 0)
				{
					strncpy(szDir, szNewPath3, strlen(szNewPath3));
				}
				else if (strnicmp(szOldPath4, szDir, strlen(szOldPath4)) == 0)
				{
					strncpy(szDir, szNewPath4, strlen(szNewPath4));
				}
				else if (strnicmp(szOldPath5, szDir, strlen(szOldPath5)) == 0)
				{
					strncpy(szDir, szNewPath5, strlen(szNewPath5));
				}
				else if (strnicmp(szOldPath6, szDir, strlen(szOldPath6)) == 0)
				{
					strncpy(szDir, szNewPath6, strlen(szNewPath6));
				}

				size_t	c1=0, c2=0;

				for (c1=0; c1 < strlen(szDir); c1++)
				{
					if (szDir[c1] != ' ')
						szDirN[c2++] = szDir[c1];
				}
				szDirN[c2] = '\0';

				fullPath  = RMBCString(szDrive);
				fullPath += RMBCString(szDirN);
				fullPath += RMBCString(szFname);
				fullPath += RMBCString(szExt);

				//
				// END HACK ALERT!!!!!
				//


#if 0
				// 
				// Append the PHOTO.LOG file with file names which are
				// opened by reference.
				//
				RMBCString szLogFile = ::GetApplication().GetApplicationPath();
				szLogFile += "PHOTO.LOG";

				CStdioFile logFile;
				
				logFile.Open( szLogFile, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText );
				logFile.SeekToEnd();
				logFile.WriteString( (LPCTSTR)fullPath );
				logFile.WriteString( "\n" );
				logFile.Close();
#endif

				RStorage storage( fullPath, kRead );
				YStreamLength fileSize = storage.GetStorageStream()->GetSize();
				YException hException;
				Load( storage, fileSize, hException, NULL, FALSE );
			}
		}
		catch (...)
		{
			RAlert().AlertUser( STRING_ERROR_OPENIMGERR_FILENOTFOUND );
		}
	}
}

// ****************************************************************************
//
//  Function Name:	RPSDPhotoDocument::Write()
//
//  Description:		Write out a bitmap to storage
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RPSDPhotoDocument::Write(RChunkStorage& rStorage, EWriteReason reason) const
{
	// make sure that if we're saving a QSL we have a partial path
	if (reason == kPrintShopQslPhotoSaveByRef && m_rPartialPath.IsEmpty())
	{
		RAlert().AlertUser(STRING_ERROR_SAVE_AS_READYMADE_NO_PARTIAL_PATH);
#if 0
		// GCB 4/15/98 - for some reason this throw is not caught even though
		// there appears to be a catch at RNativeFileFormat::Save() so just
		// comment it out for now so that asset production can proceed
		throw;
#endif
	}

	// Call the inherited method
	RImageDocument::Write(rStorage, reason);

	rStorage.AddChunk(kPartialPathChunkID);
	rStorage << m_rPartialPath;
	rStorage.SelectParentChunk();
}

