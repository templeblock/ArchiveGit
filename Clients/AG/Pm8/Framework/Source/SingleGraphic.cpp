// ****************************************************************************
//
//  File Name:			SingleGraphic.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RSingleGraphic class
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
//  $Logfile:: /PM8/Framework/Source/SingleGraphic.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "SingleGraphic.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "DrawingSurface.h"
#include "Bezier.h"
#include "QuadraticSpline.h"
#include	"GradientFill.h"
#include "ChunkyStorage.h"
#include "ImageLibrary.h"
#include "PolyPolygon.h"
#include "DefaultDrawingSurface.h"
#include "OffscreenDrawingSurface.h"
#include "BitmapImage.h"

#ifndef	IMAGER
	#include "StandaloneApplication.h"	//	This is for the Global Buffer
#else		//	IMAGER
	//
	//		If including GlobalBuffer.h, It is the responsibility of the application to call
	//		StartupGlobalBuffer and ShutdownGlobalBuffer.
	#include "GlobalBuffer.h"
#endif	//	IMAGER

const uLONG		kOperatorArrayGrowSize	= 1024;
const	uLONG		kPointArrayGrowSize		= 4096;

// ****************************************************************************
//
// Function Name:		Transform( const Iterator& itrStart, const Iterator& itrEnd, const R2dTransform& xform )
//
// Description:		apply the given transformation to the items starting at itrStart and up to
//                   but not including itrEnd.
//
// Returns:				nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
template<class TIter>
void Transform( const TIter& start, const TIter& end, const R2dTransform& xform )
{
	TIter	itr = start;
	while( itr != end )
		{
		(*itr) *= xform;
		++itr;
		}
}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::RSingleGraphic( )
//
// Description:		Constructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RSingleGraphic::RSingleGraphic( BOOLEAN fCreateRender )
	:	m_GraphicSize( 0.0, 0.0 ),
		m_OperationsStream( kOperatorArrayGrowSize ),
		m_PointCountsStream( kOperatorArrayGrowSize ),
		m_PathOperatorsStream( kOperatorArrayGrowSize ),
	  	m_OriginalPointsStream( kPointArrayGrowSize ),
	  	m_TransformedIntPointsStream( kPointArrayGrowSize ),
	  	m_TransformedRealPointsStream( kPointArrayGrowSize ),
		m_fAdjustLineWidth( FALSE ),
		m_pRenderCache( NULL ),
		m_fColorTransformInvalid( TRUE )
	{
	if ( fCreateRender )
		m_pRenderCache = new RSingleGraphicRenderCache( this );
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::~RSingleGraphic( )
//
// Description:		Destructor
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RSingleGraphic::~RSingleGraphic( )
	{
	delete m_pRenderCache;
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::RSingleGraphic( )
//
// Description:		Copy Constructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RSingleGraphic::RSingleGraphic( const RSingleGraphic &rhs ) 
	: m_GraphicSize( rhs.m_GraphicSize ),
	  m_fAdjustLineWidth(FALSE),
	  m_pRenderCache( NULL ),
	  m_fColorTransformInvalid( TRUE )

	{
	m_pRenderCache = new RSingleGraphicRenderCache( this );	

	*this = rhs;
	}

// ****************************************************************************
//
// Function Name:		RGraphic::operator=( )
//
// Description:		assignment operator
//
// Returns:				reference to self
//
// Exceptions:			None
//
// ****************************************************************************
//
RSingleGraphic& RSingleGraphic::operator=( const RSingleGraphic& rhs )
{
	// Call the base method
	RGraphic::operator=( rhs );

	// Copy the data
	m_OperationsStream			= rhs.m_OperationsStream;
	m_ColorsStream					= rhs.m_ColorsStream;
	m_PenWidthsStream				= rhs.m_PenWidthsStream;
	m_PenStylesStream				= rhs.m_PenStylesStream;
	m_FillMethodsStream			= rhs.m_FillMethodsStream;
	m_PointCountsStream			= rhs.m_PointCountsStream;
	m_PathOperatorsStream		= rhs.m_PathOperatorsStream;
	m_OriginalPointsStream		= rhs.m_OriginalPointsStream;
	m_TransformedIntPointsStream	= rhs.m_TransformedIntPointsStream;
	m_TransformedRealPointsStream	= rhs.m_TransformedRealPointsStream;
	m_GradientFillStream			= rhs.m_GradientFillStream;
	m_GradientRampStream			= rhs.m_GradientRampStream;
	m_GradientIndexStream		= rhs.m_GradientIndexStream;
	m_GraphicSize					= rhs.m_GraphicSize;
	m_fMonochrome					= rhs.m_fMonochrome;
	m_LastTransform				= rhs.m_LastTransform;
	m_fAdjustLineWidth			= rhs.m_fAdjustLineWidth;
	m_fColorTransformInvalid	= TRUE;	//	Assume we want to recompute this always.

	return *this;
}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::DoApplyTransform( )
//
// Description:		Copies the original points while applying the given
//							transform to them all.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphic::DoApplyTransform( const R2dTransform& transform, BOOLEAN fIntegerAlso )
	{
	if( fIntegerAlso )
		{
		//
		// If the transformed integer points don't exist, then let's copy the xformd reals in first...
		if( m_TransformedIntPointsStream.Count() == 0 )
			{
			//
			// NOTE: We might be able to optimize this using a SetSize() member to arrays...
			::Append( m_TransformedIntPointsStream, m_TransformedRealPointsStream.Start(), m_TransformedRealPointsStream.End() );
			}
		else
			{
			TpsAssert( m_TransformedIntPointsStream.Count() == m_TransformedRealPointsStream.Count(), "int point array is of different size" );
			}
		}
	//
	//	If the transforms are equal, don't do anything
	if (m_LastTransform != transform)
		{
		//
		//	We are altering the transform. We will want to recompute the color and gradients
		m_fColorTransformInvalid = TRUE;
		//
		// Apply the given transformation to all points...
		// Attempt to optimize it:  if the only thing that changed was the translation
		// then we can simply adjust the already transformed points
		if ( m_LastTransform.AreScaleAndRotateEqual( transform ) )
			{
			//
			// Only the translation is different, so we can compute the delta and
			// adjust the transformed points directly...
			RRealPointArrayIterator	itrOriginal	= m_OriginalPointsStream.Start();
			if( itrOriginal != m_OriginalPointsStream.End() )
				{
				//
				// First, we compute the offset...
				RRealPointArrayIterator	itrReal		= m_TransformedRealPointsStream.Start();
				RRealPoint					ptNewReal	= (*itrOriginal) * transform;
				RRealSize					realOffset	= ptNewReal - (*itrReal);
				if( fIntegerAlso )
					{
					RIntPointArrayIterator	itrInt		= m_TransformedIntPointsStream.Start();
					RIntPoint					ptNewInt		= ptNewReal;
					RIntSize						intOffset	= ptNewInt - (*itrInt);
					//
					// Now offset the points...
					while( itrInt != m_TransformedIntPointsStream.End() )
						{
						(*itrInt)	+= intOffset;
						++itrInt;
						}
					}
				//
				// Now offset the realpoints...
				while( itrReal != m_TransformedRealPointsStream.End() )
					{
					(*itrReal)	+= realOffset;
					++itrReal;
					}
				}
			}
		else
			{
			//	Transform is sufficiently different to allow
			//	the points to be altered with the transform
			RRealPointArrayIterator	itrOriginal	= m_OriginalPointsStream.Start();;
			RRealPointArrayIterator	itrReal		= m_TransformedRealPointsStream.Start(); 
			while( itrReal != m_TransformedRealPointsStream.End() )
				{
				(*itrReal)	= (*itrOriginal) * transform;
				++itrOriginal;
				++itrReal;
				}
			//
			// if we're doing integer as well, then copy the points in...
			if( fIntegerAlso )
				{
				RIntPointArrayIterator	itrInt		= m_TransformedIntPointsStream.Start();
				itrReal										= m_TransformedRealPointsStream.Start();
				while( itrReal != m_TransformedRealPointsStream.End() )
					{
					(*itrInt)	= (*itrReal);
					++itrInt;
					++itrReal;
					}
				}
			}
		//
		// Finally, we can copy the transform...
		m_LastTransform	= transform;
		}

	if ( m_fColorTransformInvalid )
		{
		//
		// Apply the given transformation to all gradients...
		YGradientFillArrayIterator	itrGradientFill	= m_GradientFillStream.Start();
		while( itrGradientFill < m_GradientFillStream.End() )
			{
			(*itrGradientFill).ApplyTransform( transform );
			++itrGradientFill;
			}
		//
		// Apply the given transformation to all colors...
		RColorArrayIterator	itrColors = m_ColorsStream.Start();
		while( itrColors < m_ColorsStream.End() )
			{
			(*itrColors) *= transform;
			++itrColors;
			}

		m_fColorTransformInvalid = FALSE;
		}
 	}


// ****************************************************************************
//
// Function Name:		RSingleGraphic::ApplyTransform( )
//
// Description:		Copies the original points while applying the given
//							transform to them all.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphic::ApplyTransform( const R2dTransform& transform )
	{
	DoApplyTransform( transform, TRUE );
 	}


// ****************************************************************************
//
// Function Name:		RSingleGraphic::Render( )
//
// Description:		Renders the graphic into the given drawing surface.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphic::Render( RDrawingSurface& drawingSurface,
									  const RRealSize& size,
									  const R2dTransform& transform,
									  const RIntRect& render,
									  const RColor& monochromeColor,
									  BOOLEAN fImageAsMask )
	{
	// If we are imaging a mask, or we dont have a render cache, call the render directly
	if( fImageAsMask || m_pRenderCache == NULL )
		DoRender( drawingSurface, size, transform, monochromeColor, fImageAsMask );

	// Otherwise let the render cache render
	else
		m_pRenderCache->RenderGraphic( drawingSurface, size, transform, render, monochromeColor );
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::Render( )
//
// Description:		Renders the graphic into the given drawing surface.
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphic::DoRender( RDrawingSurface& drawingSurface,
										 const RRealSize& size,
									    const R2dTransform& transform,
									    const RColor& monochromeColor,
									    BOOLEAN fImageAsMask )
	{
	TpsAssertValid( this );
	TpsAssertValid( ( &drawingSurface ) );

	// Make a copy of the transform
	R2dTransform resultantTransform = CalcRenderXform( size, transform );

	// Initialize the surface attributes
	if ( drawingSurface.GetColoringBookMode() )
		{
		drawingSurface.SetPenColor( RSolidColor( kBlack ) );
		drawingSurface.SetFillColor( RColor(RSolidColor(kWhite)) );
		}
	else
		{
		if( IsMonochrome( ) || fImageAsMask )
			{
			drawingSurface.SetPenColor( monochromeColor );
			drawingSurface.SetFillColor( monochromeColor );
			}
		else
			{
			drawingSurface.SetPenColor( RSolidColor( kBlack ) );
			drawingSurface.SetFillColor( RColor(RSolidColor(kBlack)) );
			}
		}

	drawingSurface.SetPenWidth( 1 );

	// Apply the resultant transform to the points
	ApplyTransform( resultantTransform );

	// If the surface does not have built in path support, and the scale and rotate components
	// of the transform have not changed, use our cache.
	BOOLEAN fCacheValid = FALSE;
	RIntSize offset;

	// setup array iterators
	RIntPointArrayIterator			iRIntPointIterator		= m_TransformedIntPointsStream.Start();
	RRealPointArrayIterator			iRRealPointIterator		= m_TransformedRealPointsStream.Start();
	EOperationArrayIterator			iEOperationIterator		= m_OperationsStream.Start();
	EPathOperatorArrayIterator		iEPathOperatorIterator	= m_PathOperatorsStream.Start();
	RColorArrayIterator				iRColorIterator			= m_ColorsStream.Start();
	YPenWidthArrayIterator			iYPenWidthIterator		= m_PenWidthsStream.Start();
	YPenStyleArrayIterator			iYPenStyleIterator		= m_PenStylesStream.Start();
	YFillMethodArrayIterator		iYFillMethodIterator		= m_FillMethodsStream.Start();
	YPointCountArrayIterator		iYPointCountIterator		= m_PointCountsStream.Start();
	YGradientIndexArrayIterator	itrGradientIndex			= m_GradientIndexStream.Start();

	EOperation operation;

	// Draw
	while( ( operation = *iEOperationIterator ) != kEndGraphic )
		{
		++iEOperationIterator;
		switch( operation )
			{
			case kSetLineWidth :
				{
				YIntDimension	pen = CalcPenWidth( *iYPenWidthIterator, size, transform );
				++iYPenWidthIterator;

				drawingSurface.SetPenWidth( pen );
				break;
				}

			case kSetLineColor :
				{
				// Get the line color
				RColor lineColor = *iRColorIterator;
				++iRColorIterator;

				// If we are imaging as a mask, ignore this
				//	Don't draw colored lines in coloring book mode;
				//	it might confuse the little kiddies.
				if( fImageAsMask || drawingSurface.GetColoringBookMode() )
					break;

				// If it is a monochrome graphic, replace black with the monochrome color
				if( IsMonochrome( ) && lineColor == RColor( RSolidColor( kBlack ) ) )
					drawingSurface.SetPenColor( monochromeColor );
				else
					drawingSurface.SetPenColor( lineColor );
				break;
				}

			case kSetFillColor :
				{
				// Get the fill color
				RColor fillColor = *iRColorIterator;
				++iRColorIterator;

				// If we are imaging as a mask, ignore this
				if( fImageAsMask || drawingSurface.GetColoringBookMode() )
					break;

				// If it is a monochrome graphic, replace black with the monochrome color
				if( IsMonochrome( ) && fillColor == RColor( RSolidColor( kBlack ) ) )
					drawingSurface.SetFillColor( monochromeColor );
				else
					drawingSurface.SetFillColor( fillColor );
				break;
				}

			case kSetGradient:
				{
				//
				// Select the given gradient into the drawing surface...
				YGradientIndex	idxGradient	= *itrGradientIndex++;
				//
				// If we are imaging as a mask, ignore this
				if( fImageAsMask )
					break;

				RColor	fillColor;
				if ( !drawingSurface.GetColoringBookMode( ) )
				{
//					RRealSize		graphicSize = GetGraphicSize();
//					R2dTransform	transform;
//					transform.PreTranslate( 0, graphicSize.m_dy );
//					transform.PreScale( 1, -1 );
					RGradientFill	gradient = m_GradientFillStream.Start()[idxGradient];
//					gradient.SetGradientAngle( - gradient.GetGradientAngle() );
//					gradient.ApplyTransform( transform );
					fillColor	= RColor(gradient);
				}
				else
					fillColor	= RSolidColor( kWhite );
				drawingSurface.SetFillColor( fillColor );
				break;
				}

			case kSetLineStyle :
				// Ignore set line style. It doesnt appear to be used.
				++iYPenStyleIterator;
				break;

			case kPolyline :
				drawingSurface.Polyline( &(*iRIntPointIterator), *iYPointCountIterator );
				iRIntPointIterator += *iYPointCountIterator;
				iRRealPointIterator += *iYPointCountIterator;
				++iYPointCountIterator;
				break;

			case kPolygon :
				{
				// Get the fill method and point count
				YFillMethod fillMethod = *iYFillMethodIterator;
				++iYFillMethodIterator;
				YPointCount pointCount = *iYPointCountIterator;
				++iYPointCountIterator;

				switch( fillMethod )
					{
					case kFilled:
						{
						drawingSurface.FillPolygon( &(*iRIntPointIterator), pointCount );
						break;
						}

					case kStroked:
						{
						if( *iRIntPointIterator == *(iRIntPointIterator + pointCount - 1) )
							{
							//
							// it's closed, we can just call FramePolygon
							drawingSurface.FramePolygon( &(*iRIntPointIterator), pointCount );
							}
						else
							{
							//
							// it's not closed, we have to draw the lines individually
							drawingSurface.Polyline( &(*iRIntPointIterator), pointCount );
							}
						break;
						}

					case kFilledAndStroked:
						{
						if ( *iRIntPointIterator == *(iRIntPointIterator + pointCount - 1) )
							{
							//
							// it's closed, we can just call Polygon
							drawingSurface.Polygon( &(*iRIntPointIterator), pointCount );
							}
						else
							{
							//
							// it's not closed, we have to fill and then draw the lines individually
							if ( pointCount > 2 )
								drawingSurface.FillPolygon( &(*iRIntPointIterator), pointCount );
							drawingSurface.Polyline( &(*iRIntPointIterator), pointCount );
							}
						break;
						}

					case kStrokedAndFilled:
						{
						drawingSurface.Polyline( &(*iRIntPointIterator), pointCount );
						if ( pointCount > 2 )
							drawingSurface.FillPolygon( &(*iRIntPointIterator), pointCount );
						break;
						}

					case kGradFilled:
						{
						drawingSurface.FillPolygon( &(*iRIntPointIterator), pointCount );
						break;
						}

					case kGradFilledAndStroked:
						{
						drawingSurface.FillPolygon( &(*iRIntPointIterator), pointCount );
						drawingSurface.Polyline( &(*iRIntPointIterator), pointCount );
						break;
						}

					case kStrokedAndGradFilled:
						{
						drawingSurface.Polyline( &(*iRIntPointIterator), pointCount );
						drawingSurface.FillPolygon( &(*iRIntPointIterator), pointCount );
						break;
						}
					}

				iRIntPointIterator += pointCount;
				iRRealPointIterator += pointCount;
				break;
				}
			case kEllipse:
			{
				// Get the fill method and point count
				YFillMethod fillMethod = *iYFillMethodIterator;
				++iYFillMethodIterator;
				YPointCount pointCount = *iYPointCountIterator;
				++iYPointCountIterator;

				TpsAssert( pointCount == 2, "Invalid number of points in list for Ellipse" );

				// Get the bounding rectangle of the elipse.
				RIntPoint *pPointArray = &(*iRIntPointIterator);
				RIntRect rectEllipse( pPointArray[0], pPointArray[1] );

				switch( fillMethod )
				{
					case kFilled:
					{
						drawingSurface.FillEllipse(rectEllipse);
						break;
					}

					case kStroked:
					{
						drawingSurface.FrameEllipse(rectEllipse);
						break;
					}

					case kFilledAndStroked:
					{
						drawingSurface.Ellipse(rectEllipse);
						break;
					}

					case kStrokedAndFilled:
					{
						drawingSurface.FrameEllipse(rectEllipse);
						drawingSurface.FillEllipse(rectEllipse);
						break;
					}
				}

				iRIntPointIterator += pointCount;
				iRRealPointIterator += pointCount;
				break;
			}
			case kPath :
				{
				YPathOperatorArraySize operatorArraySize;
				YFillMethod fillMethod = *iYFillMethodIterator;
				++iYFillMethodIterator;
				YPointCount pointCount = *iYPointCountIterator;
				++iYPointCountIterator;

				operatorArraySize = RPath::RenderPathPoints( drawingSurface, &(*iEPathOperatorIterator), &(*iRIntPointIterator), &(*iRRealPointIterator), fillMethod, fCacheValid, offset );

				iEPathOperatorIterator += operatorArraySize;
				iRIntPointIterator += pointCount;
				iRRealPointIterator += pointCount;
				break;
				}

			default :
				TpsAssert( NULL, "Unrecognized opcode." );
			}
		}
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::GetFirstBox()
//
// Description:		Returns the first box encountered in the parsing process
//							(used by Timepieces)
//
// Returns:				
//
// Exceptions:			
//
// ****************************************************************************
RIntRect RSingleGraphic::GetFirstBox()
{
	TpsAssert(m_fFirstBox, "Graphic has no box elements.");

	return m_rFirstBox;
}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::MergeWith( const R2dTransform& transform, RSingleGraphic& graphic )
//
// Description:		apply the given transformation to this graphic and append the resultant
//                   data to the given graphic.
//
// Returns:				nothing
//
// Exceptions:			memory
//
// ****************************************************************************
//
void RSingleGraphic::MergeWith( const R2dTransform& transform, RSingleGraphic& graphic ) 
	{
	graphic.AppendGraphic( *this, transform );
	}


// ****************************************************************************
//
// Function Name:		RSingleGraphic::AppendGraphic( RSingleGraphic& gr, const R2dTransform& transform )
//
// Description:		Apply the given transform to the given graphic and append the resultant
//                   data to this graphic...
//
// Returns:				nothing
//
// Exceptions:			memory
//
// ****************************************************************************
//
void RSingleGraphic::AppendGraphic( RSingleGraphic& graphic, const R2dTransform& transform )
	{
	//
	// Get the bounding rect for the graphic...
	// This MUST precede the transformation
	RRealRect	rcGraphicBounds( graphic.m_GraphicSize );
	rcGraphicBounds *= transform;
	//
	// Apply the given transformation to the graphic
	graphic.DoApplyTransform( transform, FALSE );
	//
	// Create the resultant graphic's bounding rect
	RRealRect	rcNewBounds( m_GraphicSize );
	rcNewBounds.Union( rcNewBounds, rcGraphicBounds );
	//
	// Set the new combined size...
	m_GraphicSize = rcNewBounds.WidthHeight();
	//
	// Create a transformation that will adjust everything to be relative to the
	// new origin...
	RIntPoint		ptTopLeft	= rcNewBounds.m_TopLeft;
	R2dTransform	xformNewOrigin;
	xformNewOrigin.PreTranslate( -ptTopLeft.m_x, -ptTopLeft.m_y );
	//
	// Append the simple data...
	m_PathOperatorsStream.Append( graphic.m_PathOperatorsStream.Start(), graphic.m_PathOperatorsStream.End() );
	m_PointCountsStream.Append( graphic.m_PointCountsStream.Start(), graphic.m_PointCountsStream.End() );
	m_FillMethodsStream.Append( graphic.m_FillMethodsStream.Start(), graphic.m_FillMethodsStream.End() );
	m_PenStylesStream.Append( graphic.m_PenStylesStream.Start(), graphic.m_PenStylesStream.End() );
	m_PenWidthsStream.Append( graphic.m_PenWidthsStream.Start(), graphic.m_PenWidthsStream.End() );
	m_ColorsStream.Append( graphic.m_ColorsStream.Start(), graphic.m_ColorsStream.End() );
	m_OriginalPointsStream.Append( graphic.m_TransformedRealPointsStream.Start(), graphic.m_TransformedRealPointsStream.End() );
	//
	// Transform all points...
	Transform( m_OriginalPointsStream.Start(), m_OriginalPointsStream.End(), (const R2dTransform&)xformNewOrigin );
	//
	// Reset the last transformation to empty...
	m_LastTransform.MakeIdentity();
	m_TransformedIntPointsStream.Empty();
	m_TransformedRealPointsStream.Empty();
//NOTE: points are copied in ApplyTransform()	::Append( m_TransformedIntPointsStream, m_OriginalPointsStream.Start(), m_OriginalPointsStream.End() );
	m_TransformedRealPointsStream = m_OriginalPointsStream;
	//
	// Append the gradient data...
	// We need to make some adjustments, so before we do anything else we'll get the
	// index of the first appended ramp...
	YCounter	idxFirstAppended	= YCounter(m_GradientRampStream.Count());
	//
	// Now we can append the ramps...
	m_GradientRampStream.Append( graphic.m_GradientRampStream.Start(), graphic.m_GradientRampStream.End() );
	//
	// Append the ramp indices.  Remember: we need to adjust these since they
	// now reference ramps that were appended to our ramps...
	YGradientIndexArrayIterator	itrIdx	= graphic.m_GradientIndexStream.Start();
	while( itrIdx != graphic.m_GradientIndexStream.End() )
		{
		m_GradientIndexStream.InsertAtEnd( (*itrIdx) + idxFirstAppended );
		++itrIdx;
		}
	//
	// Append the gradient fills, these also need to be adjusted so that they reference
	// the ramps in this graphic...
	YGradientFillArrayIterator	itrGrad		= graphic.m_GradientFillStream.Start();
	while( itrGrad != graphic.m_GradientFillStream.End() )
		{
		//
		// append the gradient fill
		m_GradientFillStream.InsertAtEnd( *itrGrad );
		//
		// get a pointer to where we inserted it...
		YGradientFillArrayIterator	itrInserted = m_GradientFillStream.End();
		--itrInserted;
		//
		// Get the original ramp index
		// We know it's an array, so we can do simple pointer math...
		const RGradientRamp*	pRamp			= (*itrGrad).GetRamp();
		const RGradientRamp*	pFirstRamp	= &(*graphic.m_GradientRampStream.Start());
		YCounter					idxRamp		= YCounter( pRamp - pFirstRamp );
		//
		// reset the ramp to reference the new copy...
		(*itrInserted).SetRamp( &(m_GradientRampStream.Start()[idxRamp + idxFirstAppended]) );
		//
		// adjust the base transformation...
		(*itrInserted).SetBaseTransform( (*itrInserted).GetBaseTransform() * transform );
		//
		// advance to the next gradient...
		++itrGrad;
		}
	//
	// Now merge the operations.
	// Remember: We need to remove our end marker first...
	EOperationArrayIterator	itrOperator = m_OperationsStream.End();
	if( itrOperator != m_OperationsStream.Start()  && *(itrOperator-1)==kEndGraphic )
		m_OperationsStream.RemoveAt( --itrOperator );
	//
	// Append the operations...
	m_OperationsStream.Append( graphic.m_OperationsStream.Start(), graphic.m_OperationsStream.End() );
	//
	// Make sure we have an endgraphic operator (we may not, since we
	// could have merged an empty graphic).  Also: if we're empty, and the merged graphic
	// was empty, then the operations stream would be empty, so we can't indiscriminately
	// back up from the end!
	itrOperator = m_OperationsStream.End();
	if( itrOperator==m_OperationsStream.Start() || *(itrOperator-1)!=kEndGraphic )
		m_OperationsStream.InsertAtEnd( kEndGraphic );
	//
	// if either graphic had color, then the resultant graphic does, otherwise
	// we're still monochrome
	m_fMonochrome	= BOOLEAN( m_fMonochrome && graphic.m_fMonochrome );
	}


// ****************************************************************************
//
// Function Name:		RSingleGraphic::Undefine( )
//
// Description:		
//
// Returns:				Nothing.
//
// Exceptions:			kMemory
//
// ****************************************************************************
//
void RSingleGraphic::Undefine( ) 
	{
	m_OperationsStream.Empty();
	m_OperationsStream.InsertAtEnd( kEndGraphic );
	m_ColorsStream.Empty();
	m_PenWidthsStream.Empty();
	m_PenStylesStream.Empty();
	m_FillMethodsStream.Empty();
	m_PointCountsStream.Empty();
	m_PathOperatorsStream.Empty();
	m_OriginalPointsStream.Empty();
	m_TransformedIntPointsStream.Empty();
	m_TransformedRealPointsStream.Empty();
	m_GradientFillStream.Empty();
	m_GradientRampStream.Empty();
	m_GradientIndexStream.Empty();
	m_GraphicSize = RRealSize( 0.0, 0.0 );
	m_fMonochrome = FALSE;
	m_LastTransform.MakeIdentity();	
	m_fAdjustLineWidth = FALSE;
	}


// ****************************************************************************
//
// Function Name:		RSingleGraphic::Undefine( )
//
// Description:		
//
// Returns:				Nothing.
//
// Exceptions:			kMemory
//
// ****************************************************************************
//
void RSingleGraphic::Undefine( const RSolidColor& stroke, const RSolidColor& fill ) 
	{
	m_OperationsStream.Empty();
	m_OperationsStream.InsertAtEnd( kSetLineWidth );
	m_OperationsStream.InsertAtEnd( kSetLineColor );
	m_OperationsStream.InsertAtEnd( kSetFillColor );
	m_OperationsStream.InsertAtEnd( kEndGraphic );
	m_ColorsStream.Empty();
	m_ColorsStream.InsertAtEnd( RColor( stroke ) );
	m_ColorsStream.InsertAtEnd( RColor( fill ) );
	m_PenWidthsStream.Empty();
	m_PenWidthsStream.InsertAtEnd( 1 );
	m_PenStylesStream.Empty();
	m_FillMethodsStream.Empty();
	m_PointCountsStream.Empty();
	m_PathOperatorsStream.Empty();
	m_OriginalPointsStream.Empty();
	m_TransformedIntPointsStream.Empty();
	m_TransformedRealPointsStream.Empty();
	m_GradientFillStream.Empty();
	m_GradientRampStream.Empty();
	m_GradientIndexStream.Empty();
	m_GraphicSize = RRealSize( 0.0, 0.0 );
	m_fMonochrome = FALSE;
	m_LastTransform.MakeIdentity();	
	m_fAdjustLineWidth = FALSE;
	}


// ****************************************************************************
//
// Function Name:		RSingleGraphic::SetDefaultColors( )
//
// Description:		Set the default stroke and fill colors of the graphic,
//        				which are the 1st 2 colors in the colors stream.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphic::SetDefaultColors( const RSolidColor& stroke, 
													const RColor& fill ) 
	{
	RColorArrayIterator	colorz = m_ColorsStream.Start();
	TpsAssert( ( m_ColorsStream.Count() > 0 ), "Empty colors stream." );
	*colorz = RColor( stroke );
	++colorz;
	*colorz = fill;
	m_fColorTransformInvalid = TRUE;	//	Since we are forcing the colors into the stream
												//	we will need to force a recalc in ApplyTransforms.
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::SetDefaultPenWidth( )
//
// Description:		Set the default stroke width of the graphic,
//        				which is the 1st entry in the pen width stream stream.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphic::SetDefaultPenWidth( YPenWidth width ) 
	{
	YPenWidthArrayIterator	widthz = m_PenWidthsStream.Start();
	TpsAssert( ( m_PenWidthsStream.Count() > 0 ), "Empty pen widths stream." );
	*widthz = width;
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::GetRenderCache( )
//
// Description:		
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RRenderCache& RSingleGraphic::GetRenderCache( )
	{
	TpsAssert(m_pRenderCache, "Graphic has no render cache.");
	return *m_pRenderCache;
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::SetFillMethods( )
//
// Description:		Set all the fill methods in the graphic to the specified method.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphic::SetFillMethods( YFillMethod method ) 
	{
	for ( YFillMethodArrayIterator methodz = m_FillMethodsStream.Start(); methodz != m_FillMethodsStream.End(); ++methodz )
		{
		*methodz = method;
		}
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::SetFillMethods( )
//
// Description:		Set the number of fill methods specified to the specified method.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphic::SetFillMethods( YCounter nrMethods, YFillMethod method ) 
	{
	YCounter count = nrMethods;
	
	for ( YFillMethodArrayIterator methodz = m_FillMethodsStream.Start(); ( methodz != m_FillMethodsStream.End() && count > 0 ); ++methodz )
		{
		*methodz = method;
		count--;
		}
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::GetOutlinePath( )
//
// Description:		Compute and return the path the encompases the path of this graphic
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
BOOLEAN RSingleGraphic::GetOutlinePath(RPath& path, const RRealSize& size, const R2dTransform& transform)
	{
	ROffscreenDrawingSurface* pOffscreen = NULL;
	RBitmapImage* pBitmap = NULL;

	RDefaultDrawingSurface surface;
		
	try
		{
		const	RSolidColor		kTransparentColor( kWhite );
		const	YIntDimension	kMaximumGraphicSize	= 256;
		const YIntDimension	kMinimumGraphicSize	= 64;

		int					tmpDimension;
		RIntSize				scaledSize		= GetGraphicSize( );
		YIntDimension		scaleFactor		= 0;
		//
		//	Compute a scale factor to allow a good outline to be generated
		tmpDimension	= scaledSize.m_dx;
		while ( tmpDimension > kMaximumGraphicSize )
			++scaleFactor, tmpDimension >>= 1;
		while ( tmpDimension < kMinimumGraphicSize )
			--scaleFactor, tmpDimension <<= 1;
		tmpDimension	= (scaleFactor<0)? scaledSize.m_dy << -scaleFactor : scaledSize.m_dy >> scaleFactor;
		while ( tmpDimension > kMaximumGraphicSize )
			++scaleFactor, tmpDimension >>= 1;
		while ( tmpDimension < kMinimumGraphicSize )
			--scaleFactor, tmpDimension <<= 1;
		scaledSize.m_dx = (scaleFactor<0)? scaledSize.m_dx << -scaleFactor : scaledSize.m_dx >> scaleFactor;
		scaledSize.m_dy = (scaleFactor<0)? scaledSize.m_dy << -scaleFactor : scaledSize.m_dy >> scaleFactor;

		pOffscreen	= new ROffscreenDrawingSurface( FALSE );
		pBitmap		= new RBitmapImage;

		pBitmap->Initialize( surface, scaledSize.m_dx, scaledSize.m_dy );
		pOffscreen->SetImage( pBitmap );

		pOffscreen->SetFillColor( kTransparentColor );
		RIntRect	offRect( scaledSize );
		pOffscreen->FillRectangle( offRect );

		//	Get the original graphic size and then modify the transform to account for the change in the size
		RIntSize			graphicSize		= GetGraphicSize( );
		R2dTransform	renderTransform;
		renderTransform.PreScale( YRealDimension(scaledSize.m_dx)/YRealDimension(graphicSize.m_dx), YRealDimension(scaledSize.m_dy)/YRealDimension(graphicSize.m_dy));
		Render( *pOffscreen, graphicSize, renderTransform, RIntRect(-SHRT_MAX,-SHRT_MAX,SHRT_MAX,SHRT_MAX), RSolidColor( kBlack ), TRUE  );

#ifdef	TPSDEBUG
static	BOOLEAN	fDrawTest = 0;
if ( fDrawTest )
	{
	surface.SetFillColor( RSolidColor( kWhite ) );
	surface.FillRectangle( offRect );
	surface.BlitDrawingSurface( *pOffscreen, offRect, offRect, kBlitSourceCopy );
	}
#endif	//	TPSDEBUG

		pOffscreen->ReleaseImage( );

		//	Get the outline of the bitmap
		RPolyPolygon	outlinePoly;
		RImageLibrary rLibrary;
		rLibrary.GetOffscreenOutline( *pBitmap, kTransparentColor, outlinePoly );

		delete pOffscreen;
		delete pBitmap;

		int			nPolyCount	= outlinePoly.GetPointIndexCount( );
		int*			pPointCount	= outlinePoly.GetPointIndexCollection( );
		RIntPoint*	pPoints		= outlinePoly.GetPointCollection( );

#ifdef	TPSDEBUG
if ( fDrawTest )
	{
	surface.SetPenColor( RSolidColor( kWhite ) );
	surface.SetPenWidth( 2 );
	surface.FramePolyPolygon( pPoints, pPointCount, nPolyCount );
	}
#endif	//	TPSDEBUG

		//
		//		Setup the transform to adjust all computed points
		R2dTransform	polyTransform	= transform;
		polyTransform.PreScale( size.m_dx / YRealDimension(scaledSize.m_dx), size.m_dy / YRealDimension(scaledSize.m_dy) );
		polyTransform.PreTranslate( 0, scaledSize.m_dy );
		polyTransform.PreScale( 1, -1 );
		outlinePoly.ApplyTransform( polyTransform );

		//	Merge the points.  Note fClose is FALSE since all outlines should already be closed
		//		internally
 		path.MergePoints( pPoints, pPointCount, nPolyCount, FALSE );
		}
	catch( ... )
		{
		if ( pOffscreen )
			pOffscreen->ReleaseImage( );

		delete pOffscreen;
		delete pBitmap;

		return FALSE;
		}
	
	return TRUE;
	}


// ****************************************************************************
//
// Function Name:		RTempSingleGraphic::Read( )
//
// Description:		Nothing
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RTempSingleGraphic::Read( RChunkStorage& /* rStorage */ )
{
	TpsAssertAlways( "RTempSingleGraphics may not be read!" );
}

// ****************************************************************************
//
// Function Name:		RTempSingleGraphic::Write( )
//
// Description:		
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RTempSingleGraphic::Write( RChunkStorage& /* rStorage */) const
{
	TpsAssertAlways( "RTempSingleGraphics may not be written!" );
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
// Function Name:		RSingleGraphic::Validate( )
//
// Description:		Validates the object
//
// Returns:				nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphic::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RSingleGraphic, this );

	TpsAssert( ( m_OperationsStream.Count() > 0 ), "Invalid graphic." );
	TpsAssert( ( m_PointCountsStream.Count() > 0 ), "Invalid graphic." );
	TpsAssert( ( m_OriginalPointsStream.Count() > 0 ), "Invalid graphic." );
	TpsAssert( ( m_TransformedRealPointsStream.Count() > 0 ), "Invalid graphic." );
	}

#endif		// TPSDEBUG

// ****************************************************************************
//
// Function Name:		RSingleGraphic::CalcPenWidth( )
//
// Description:		Calculates the Renaissance pen width given a graphic pen
//							width.
//
// Returns:				The calculated Renaissance pen width. This value can be
//							passed into any drawing surface function taking a pen
//							with
//
// Exceptions:			None
//
// ****************************************************************************
//
YIntDimension RSingleGraphic::CalcPenWidth( YPenWidth graphicPenWidth,
														  const RRealSize& outputLogicalSize,
														  const R2dTransform& renderTransform ) const
	{
	// This is the algorithm for calculating pen widths:
	//
	//		penWidth = ( graphicPenWidth / 20 ) * ( differentialScale * outputScaling )
	//
	// differentialScale = 8 for square graphics, 2 for rows and columns, and 1 for others.
	// outputScaling = outputNarrowSize / graphicNarrowSize
	//
	// graphicNarrowSize = 576 for square graphics, and 144 for row / column graphics
	// Thus, for square graphics, ( differentialScale * outputScaling ) = ( 8 * outputNarrowSize / 576 ) = outputNarrowSize / 72
	// for row / column graphics, ( differentialScale * outputScaling ) = ( 2 * outputNarrowSize / 144 ) = outputNarrowSize / 72
	// So the algorithm for square, row and column graphics is :
	//
	//		penWidth = ( graphicPenWidth / 20 ) * ( outputNarrowSize / 72 )
	//
	// The flag m_fAdjustLineWidth is set if the graphic should scale using the above algorithm
	//
	// For graphics without m_fAdjustLineWidth set, differentialScale = 1, so the algorithm reduces to:
	//		
	//		penWidth = ( graphicPenWidth / 20 ) * ( outputNarrowSize / graphicNarrowSize )
	//

	// Calculate the narrow output size
	RRealVectorRect vectorRect( outputLogicalSize );
	vectorRect *= renderTransform;
	YRealDimension outputNarrowSize = ::Min( vectorRect.Width( ), vectorRect.Height( ) );

	// Start with the common part of the equation
	YRealDimension penWidth = graphicPenWidth / 20.0;

	// If m_fAdjustLineWidth is set, we adjust the line widths using the first algorithm
	if( m_fAdjustLineWidth )
		penWidth *= ( outputNarrowSize / 72.0 );

	// Otherwise, we adjust it using the second algorithm
	else
		{
		// Get the graphic narrow size
		RRealSize graphicSize = GetGraphicSize( );
		YRealDimension graphicNarrowSize = ::Min( graphicSize.m_dx, graphicSize.m_dy );

		penWidth *= ( outputNarrowSize / graphicNarrowSize );
		}

	// Round to an integer pen width
	YIntDimension roundedPenWidth = ::Round( penWidth );

	// Make sure we always have a pen at least one unit wide
	if( roundedPenWidth == 0 )
		roundedPenWidth = 1;

	return roundedPenWidth;
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphic::CalcRenderXform( )
//
// Description:		
//
// Returns:				R2dTransform
//
// Exceptions:			None
//
// ****************************************************************************
//
R2dTransform RSingleGraphic::CalcRenderXform( const RRealSize& size, const R2dTransform& transform ) const
	{
	R2dTransform	xform = transform;
	RRealSize		scaleFactor( size.m_dx / m_GraphicSize.m_dx, size.m_dy / m_GraphicSize.m_dy );

	// Scale the graphic
	xform.PreScale( scaleFactor.m_dx, scaleFactor.m_dy );

	// Psd 3 graphics set the origin equal to the bottom left of the graphic.
	// We draw with the origin equal to the top left of the screen. Correct for this here.
	xform.PreTranslate( 0, m_GraphicSize.m_dy );
	xform.PreScale( 1, -1 );

	return xform;
	}

// ****************************************************************************
//
//  Function Name:	RSingleGraphic::OnXDisplayChange( )
//
//  Description:		Called when the display resolution changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSingleGraphic::OnXDisplayChange( )
	{
	m_pRenderCache->Invalidate( );
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphicRenderCache::RSingleGraphicRenderCache( )
//
// Description:		Constructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RSingleGraphicRenderCache::RSingleGraphicRenderCache( RSingleGraphic* pGraphic )
	: m_pGraphic( pGraphic ),
	  m_MonochromeColor( kBlack )
	{
	// Graphic caching is off by default
	Enable( FALSE );
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphicRenderCache::RenderGraphic( )
//
// Description:		Renders the cached graphic if it is valid and enabled.
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphicRenderCache::RenderGraphic( RDrawingSurface& drawingSurface,
															  const RRealSize& size,
															  const R2dTransform& transform,
															  const RIntRect& rcRender,
															  const RColor& monochromeColor )
	{
	// If the monochrome color is not the same as last time, invalidate the cache
	if( monochromeColor != m_MonochromeColor )
		Invalidate( );

	// Save the monochrome color so we can get it on the way back up
	m_MonochromeColor = monochromeColor;

	// Call the base method to render
	RRenderCache::Render( drawingSurface, size, transform, rcRender );
	}

// ****************************************************************************
//
// Function Name:		RSingleGraphicRenderCache::RenderData( )
//
// Description:		Renders this graphic
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RSingleGraphicRenderCache::RenderData( RDrawingSurface& drawingSurface,
														  const RRealSize& size,
														  const R2dTransform& transform,
														  const RIntRect& ) const
	{
	// Call the graphic to render
	m_pGraphic->DoRender( drawingSurface, size, transform, m_MonochromeColor, FALSE );
	}
