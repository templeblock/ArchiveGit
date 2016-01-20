// ****************************************************************************
//
//  File Name:			ComponentView.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RComponentView class
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
//  $Logfile:: /PM8/Framework/Source/ComponentView.cpp                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ComponentView.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ComponentDocument.h"
#include "ChunkyStorage.h"
#include "SingleSelection.h"
#include "Frame.h"
#include "DeactivateComponentAction.h"
#include "TintSettings.h"
#include "ChangeTintAction.h"
#include "FrameworkResourceIds.h"
#include "DrawingSurface.h"
#include "Path.h"
#include "Menu.h"
#include "TileAttributesExt.h"
#include "BackgroundColorSettings.h"
#include "ImageLibrary.h"
#include "OffscreenDrawingSurface.h"
#include "BitmapImage.h"
#include "SoftShadowSettings.h"
#include "SoftGlowSettings.h"
#include "SoftVignetteSettings.h"
#include "ComponentTypes.h"
#include "ScratchBitmapImage.h"
#include "ComponentSpecialEdgeAction.h"
#include "EdgeOutlineSettings.h"

const int kDefaultMinimumWidth = ::InchesToLogicalUnits( 0.25 );
const int kDefaultMinimumHeight = ::InchesToLogicalUnits( 0.25 );

//	Version of component view data when aspect info was added. 
const YVersion			kAspectCompensationRequiredVersion( kDefaultDataVersion + 1 );

//	Version of component view data when aspect info was added. 
const YVersion			kAspectCompensation2RequiredVersion( kDefaultDataVersion + 2 );

//	Version of component view data when aspect info was added. 
const YVersion			kTileAttributesRequiredVersion( kDefaultDataVersion + 3 );

//	Version of component view data when aspect info was added. 
const YVersion			kFrameColorRequiredVersion( kDefaultDataVersion + 4 );

//	Current version of component view data. 
const YVersion			kCurrentComponentViewRequiredVersion( kFrameColorRequiredVersion );

//	Bits set in the file for instance variables
const	uLONG		kComponentIsHorzFlipped	= 0x01;
const uLONG		kComponentIsVertFlipped	= 0x02;

// Command Map
RCommandMap<RComponentView, RPaneView> RComponentView::m_CommandMap;

RCommandMap<RComponentView,RPaneView>::RCommandMap( )
	{
	;
	}


// ****************************************************************************
//
//  Function Name:	RComponentView::RComponentView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView::RComponentView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView )
	: RPaneView( pDocument, pParentView ),
	  m_eActiveState( kInactive ),
	  m_eFrameType( kNoFrame ),
	  m_pFrame( NULL ),
	  m_pOutlinePath( NULL ),
	  m_rFrameColor( kBlack ),
	  m_fRenderActiveFrame( TRUE ),
	  m_BoundingRect( boundingRect ),
	  m_pRenderCache( NULL ),
	  m_pTileAttributes( NULL ),
	  m_AspectCompensation( 0, 0 ),
	  m_fHorizontallyFlipped( FALSE ),
	  m_fVerticallyFlipped( FALSE ),
	  m_pTileAttributesExt( NULL )
	{
	m_pOutlinePath	= new RPath;	
	m_pRenderCache = new RComponentRenderCache( this );
	m_pFrame			= RFrame::GetFrame( m_eFrameType, this );

	m_FullSoftEffectsBoundingRect = m_BoundingRect;
	AdjustVectorRectForSoftEffect( m_FullSoftEffectsBoundingRect );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::~RComponentView( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentView::~RComponentView( )
	{
	delete m_pFrame;
	delete m_pOutlinePath;	
	delete m_pRenderCache;
	delete m_pTileAttributes;
	delete m_pTileAttributesExt;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::HitTest( )
//
//  Description:		Determines if the specified point lies within this component
//
//  Returns:			TRUE:		The point is within this component
//							FALSE:	The point is not within this component
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::HitTest( const RRealPoint& point ) const
	{
	return m_BoundingRect.PointInRect( point );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::IsIntersecting( )
//
//  Description:		Determines if specified rectangle interestects this
//							component
//
//  Returns:			TRUE if the rectangle intersects
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::IsIntersecting( const RRealRect& rect ) const
	{
	return m_BoundingRect.IsIntersecting( rect );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::IsInRect( )
//
//  Description:		Determines the this component is entirely withing the
//							specified rectangle
//
//  Returns:			TRUE if this component is within the rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::IsInRect( const RRealRect& rect ) const
	{
	return m_BoundingRect.IsInRect( rect );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetComponentDocument( )
//
//  Description:		Cast the GetRDocument to a ComponentDocument
//
//  Returns:			see description
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentDocument* RComponentView::GetComponentDocument( ) const
	{
	TpsAssertIsObject( RComponentDocument, GetRDocument( ) );
	return static_cast<RComponentDocument*>( GetRDocument( ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetBoundingRect( )
//
//  Description:		Accessor
//
//  Returns:			The bounding rect of this component view
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YComponentBoundingRect& RComponentView::GetBoundingRect( ) const
	{
	return m_BoundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetFullSoftEffectsBoundingRect( )
//
//  Description:		Accessor
//
//  Returns:			The full soft effects bounding rect of this component view
//							which, if this component view has a soft shadow or glow
//							and wants it rendered outside its bounding rect, will be
//							larger than the bounding rect.
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YComponentBoundingRect& RComponentView::GetFullSoftEffectsBoundingRect( ) const
	{
	return m_FullSoftEffectsBoundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetInteriorBoundingRect( )
//
//  Description:		Accessor
//
//  Returns:			The interior bounding rect of this component view, taking
//							into account the space used by the view
//
//  Exceptions:		None
//
// ****************************************************************************
//
YComponentBoundingRect RComponentView::GetInteriorBoundingRect( ) const
	{
	RFrame* pFrame = GetFrame( );
	YComponentBoundingRect interiorBoundingRect( GetBoundingRect( ) );
	pFrame->GetInsetComponentRect( interiorBoundingRect );

	return interiorBoundingRect;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetBoundingRect( )
//
//  Description:		Sets the bounding rect of this component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::SetBoundingRect( const YComponentBoundingRect& boundingRect )
	{
	// Invalidate the area we used to occupy
	Invalidate( );

	// Calculate how much we need to scale any child components
	RRealSize oldSize = m_BoundingRect.GetUntransformedSize( );
	RRealSize newSize = boundingRect.GetUntransformedSize( );
	RRealSize scaleFactor( newSize.m_dx / oldSize.m_dx, newSize.m_dy / oldSize.m_dy );

	// Scale the embedded components
	YComponentViewIterator iterator = m_ComponentViewCollection.Start( );
	for( ; iterator != m_ComponentViewCollection.End( ); ++iterator )
		{
		(*iterator)->m_BoundingRect.ScaleAboutPoint( RRealPoint( 0, 0 ), scaleFactor );
		(*iterator)->m_FullSoftEffectsBoundingRect = (*iterator)->m_BoundingRect;
		AdjustVectorRectForSoftEffect( (*iterator)->m_FullSoftEffectsBoundingRect );
		}

	// Copy the bounding rect to m_BoundingRect and m_FullSoftEffectsBoundingRect
	// then adjust m_FullSoftEffectsBoundingRect outward if necessary
	m_BoundingRect = boundingRect;
	m_FullSoftEffectsBoundingRect = m_BoundingRect;
	AdjustVectorRectForSoftEffect( m_FullSoftEffectsBoundingRect );

	// Invalidate the area we now occupy
	Invalidate( );

	//	We must also Undefine any Outline computations
	UndefineOutlinePath( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetFrameType( )
//
//  Description:		Returns the frame type for this component
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
EFrameType RComponentView::GetFrameType( ) const
	{
	return m_eFrameType;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetFrame( )
//
//  Description:		Returns the frame for this component
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFrame* RComponentView::GetFrame( ) const
	{
	return m_pFrame;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetFrame( RFrame::EFrameType eNewFrame )
//
//  Description:		Install the given frame in this component
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::SetFrame( EFrameType eNewFrame )
	{
	//	Do nothing if were setting the frame we have.
	//	This can happen if were constructing.
	if( m_eFrameType != eNewFrame )
		{
		//	Create the new frame first to make sure we can...
		RFrame* pNewFrame	= RFrame::GetFrame( eNewFrame, this );

		//	Set the frame. Make sure the framed object's aspect ratio will be preserved.
		//	Change the object size so that when the frame is applied the object 
		//	will have the correct aspect ratio.
								
		//	Get the current and new frame's insets.
		RRealSize currentTopLeftInset;
		RRealSize currentBottomRightInset;
		RRealSize newTopLeftInset;
		RRealSize newBottomRightInset;
		RFrame* pFrame = GetFrame( );
		pFrame->GetInset( currentTopLeftInset, currentBottomRightInset );
		pNewFrame->GetInset( newTopLeftInset, newBottomRightInset );
				
		//	Get the framed and unframed sizes and aspect ratios.
	//	YComponentBoundingRect boundingRect( m_BoundingRect );

		//	Unframed size is current size plus current frame insets.
		//	Add back in aspect compensation previously removed to make framed object aspect correct.
		//	Note that m_AspectCompensation has to be negative to keep the framed object smaller than the unframed object.
		RRealSize rUnframedSize = m_BoundingRect.WidthHeight( );
		rUnframedSize.m_dx += currentTopLeftInset.m_dx + currentBottomRightInset.m_dx - m_AspectCompensation.m_dx;
		rUnframedSize.m_dy += currentTopLeftInset.m_dy + currentBottomRightInset.m_dy - m_AspectCompensation.m_dy;
		YRealDimension rCorrectAspect = rUnframedSize.m_dy / rUnframedSize.m_dx;

		//	New framed size is unframed size minus new frame insets.
		RRealSize rNewFramedSize = rUnframedSize;
		rNewFramedSize.m_dx -= newTopLeftInset.m_dx + newBottomRightInset.m_dx;
		rNewFramedSize.m_dy -= newTopLeftInset.m_dy + newBottomRightInset.m_dy;
		YRealDimension rNewAspect = rNewFramedSize.m_dy / rNewFramedSize.m_dx;

		//	Reset the bounding rect to its unframed size by pulling out prior aspect compensation.
		RRealSize currentSize = m_BoundingRect.WidthHeight( );
		currentSize.m_dx -= m_AspectCompensation.m_dx;
		currentSize.m_dy -= m_AspectCompensation.m_dy;
		Resize( currentSize );

		//	Shrink object in x dimension by aspect correct difference between frame with object and frame without.
		//	Use the identity (unframed y object size)/(unframed x object size)
		//	== (unframed y object size - y aspect compensation)/(unframed x object size - x aspect compensation) 
		//	to calculate x and y aspect compensations.
		m_AspectCompensation.m_dx = ( ((YRealDimension)rNewFramedSize.m_dy)/rCorrectAspect - rNewFramedSize.m_dx);
		m_AspectCompensation.m_dy = rNewFramedSize.m_dx * rCorrectAspect - rNewFramedSize.m_dy;

		//	Use either the x or y compensation; whichever is negative. Remove the other compensation
		//	since they do the same thing with different (x or y) dimensions.
		if ( 0 < m_AspectCompensation.m_dx )
			m_AspectCompensation.m_dx = 0;
		if ( 0 < m_AspectCompensation.m_dy )
			m_AspectCompensation.m_dy = 0;
		TpsAssert( ( 0 >= m_AspectCompensation.m_dx ) && (  0 >= m_AspectCompensation.m_dy ), "Component enlarged by aspect compensation." );

		//	Save the new frame type.
		m_eFrameType	= eNewFrame;
		m_pFrame			= pNewFrame;
		delete pFrame;

		//	Remember the new bounding rect size.
		currentSize = m_BoundingRect.WidthHeight( );
		currentSize.m_dx += m_AspectCompensation.m_dx;
		currentSize.m_dy += m_AspectCompensation.m_dy;
		Resize( currentSize );

	#ifdef TPSDEBUG
		//	Apply the aspect compensation.
		rUnframedSize.m_dx += m_AspectCompensation.m_dx;
		rUnframedSize.m_dy += m_AspectCompensation.m_dy;

		//	Test above code:
		rNewFramedSize = rUnframedSize;
		rNewFramedSize.m_dx -= newTopLeftInset.m_dx + newBottomRightInset.m_dx;
		rNewFramedSize.m_dy -= newTopLeftInset.m_dy + newBottomRightInset.m_dy;
		rNewAspect = rNewFramedSize.m_dy / rNewFramedSize.m_dx;
		TpsAssert( Abs( rNewAspect - rCorrectAspect ) < .005, "Component reduction did not correct aspect size." );
	#endif

		// Notify anyone who cares that the component has been framed
		OnFrameComponent( this );

		// Invalidate the render cache
		InvalidateRenderCache( );
		}

	// If we are turning Frames ON, Disable the RenderCache
	if ( eNewFrame != kNoFrame )
		m_pRenderCache->Enable( FALSE );

	//	Otherwise, reset it to its default state
	else	
		m_pRenderCache->Enable( EnableRenderCacheByDefault( ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetFrameColor( ) const
//
//  Description:		Returns the component's frame color
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSolidColor RComponentView::GetFrameColor( ) const
	{
	return m_rFrameColor;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetFrameColor( const RColor rNewColor )
//
//  Description:		set the component's frame color
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::SetFrameColor( const RSolidColor rNewColor )
	{
	m_rFrameColor = rNewColor;

	Invalidate( );

	// Invalidate the render cache
	InvalidateRenderCache( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetBackgroundColor( const RColor rBackgroundColor )
//
//  Description:		Install the given background color
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::SetBackgroundColor( const RColor& rBackgroundColor )
	{
	BOOLEAN fHadColor = HasBackgroundColor( );
	RPaneView::SetBackgroundColor( rBackgroundColor );

	//	If it previously did not have a color, Undefine the path
	if( !fHadColor )
		UndefineOutlinePath( );

	// Invalidate the render cache
	InvalidateRenderCache( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::ApplyTransform( )
//
//  Description:		Adds this views transform operations to the given
//							transform.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::ApplyTransform( R2dTransform& transform, BOOLEAN fApplyParentsTransform, BOOLEAN fIsPrinting ) const
	{
	RPaneView::ApplyTransform( transform, fApplyParentsTransform, fIsPrinting );

	// Position the component
	transform = m_BoundingRect.GetTransform( ) * transform;

#define	ADJUST_TRANSFORM_FOR_FRAME		1
#ifdef	ADJUST_TRANSFORM_FOR_FRAME
	//	If I don't have a frame, don't bother playing with any transforms...
	if ( GetFrameType() != kNoFrame )
		{
		RFrame*		pFrame	= GetFrame( );
		//	Adjust for the frame here
		RRealSize	topLeftInset;
		RRealSize	bottomRightInset;
		//	Compute the insets from the frame	
		pFrame->GetInset( topLeftInset, bottomRightInset );
		//
		//	Remove any flip transformation from the frame if they are present
		if ( m_fHorizontallyFlipped )
			{
			YRealDimension	tmp		= bottomRightInset.m_dx;
			bottomRightInset.m_dx	= topLeftInset.m_dx;
			topLeftInset.m_dx			= tmp;
			}
		if ( m_fVerticallyFlipped )
			{
			YRealDimension	tmp		= bottomRightInset.m_dy;
			bottomRightInset.m_dy	= topLeftInset.m_dy;
			topLeftInset.m_dy			= tmp;
			}
		//
		//		If this is a group (if it has a view collection),
		//		we don't need to offset if topLeft == bottomRight we just need to scale.
		//		If topLeft != bottomRight, we scale and offset by 1/2 the difference of
		//			bottomRight - topLeft
		if ( m_ComponentViewCollection.Count() > 0 )
			{
			R2dTransform	xForm( m_BoundingRect.GetTransform() );
			YRealDimension	rotation;
			YRealDimension	xScale, yScale;
			xForm.Decompose( rotation, xScale, yScale );

			//	Compute the center point for any flipping that may occur
			RRealSize	fullSize( m_BoundingRect.GetUntransformedSize( ) );
			RRealPoint	centerPoint( fullSize.m_dx/2.0, fullSize.m_dy/2.0 );
			RRealSize	newSize( fullSize.m_dx - bottomRightInset.m_dx - topLeftInset.m_dx, fullSize.m_dy - bottomRightInset.m_dy - topLeftInset.m_dy );
			transform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, 1.0/xScale, 1.0/yScale );
			transform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, newSize.m_dx/fullSize.m_dx, newSize.m_dy/fullSize.m_dy );
			transform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, xScale, yScale );
			topLeftInset.m_dx	= -(bottomRightInset.m_dx - topLeftInset.m_dx) / 2.0;
			topLeftInset.m_dy	= -(bottomRightInset.m_dy - topLeftInset.m_dy) / 2.0;

			}
		//	Offset the transform to account the frame.
		transform.PreTranslate( topLeftInset.m_dx, topLeftInset.m_dy );
		}	//	FrameType != kNoFrame
#endif	// ADJUST_TRANSFORM_FOR_FRAME
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::RenderBackground( )
//
//  Description:		Render this component's background. 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::RenderBackground( RDrawingSurface& /*drawingSurface*/, const R2dTransform& /*transform*/, const RIntRect& ) const
	{
	//	Prevent RPaneView::RenderBackground from drawing the component's background by doing nothing. 
	//	It will be drawn in RComponentView::Render.
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::Render( )
//
//  Description:		Render this view into the destination rectangle, preserving
//							aspect ratio
//
//  Returns:			The rectangle that the component was rendered into
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RComponentView::Render( RDrawingSurface& drawingSurface, const RRealRect& destRect, const RRealSize& extraScale ) const
	{
	// Build a transform
	R2dTransform transform;

	// Get the component size
	RRealSize componentSize = GetReferenceSize( );

	// Calculate an aspect correct scale factor
	RRealSize scaleFactor( destRect.Width( ) / componentSize.m_dx, destRect.Height( ) / componentSize.m_dy );
	if( scaleFactor.m_dx < scaleFactor.m_dy )
		scaleFactor.m_dy = scaleFactor.m_dx;
	else
		scaleFactor.m_dx = scaleFactor.m_dy;

	// Compute the new component size
	RRealSize newSize( componentSize );
	newSize.Scale( scaleFactor );

	// Get the center point of the destination rect
	RRealPoint centerPoint = destRect.GetCenterPoint( );

	// Compute the real destination rectangle
	RRealRect newRect( RRealPoint( centerPoint.m_x - ( newSize.m_dx / 2 ), centerPoint.m_y - ( newSize.m_dy / 2 ) ), newSize );

	// Position the component
	transform.PreTranslate( newRect.m_Left, newRect.m_Top );

	// Size the component
	transform.PreScale( scaleFactor.m_dx * extraScale.m_dx, scaleFactor.m_dy * extraScale.m_dy );

	// Render the component
	Render( drawingSurface, transform, newRect );

	return newRect;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::Render( )
//
//  Description:		Render this view. 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
	{
	// If we have a render cache, render with it. Otherwise render directly
	if( m_pRenderCache  && !HasSoftEffects())
		m_pRenderCache->Render( drawingSurface, GetReferenceSize( ), transform, rcRender );
	else
		DoRender( drawingSurface, transform, rcRender );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::DoRender( )
//
//  Description:		Renderer called by the render cache
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::DoRender( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
{
	//	Reset the defaults before this component draws.
	drawingSurface.RestoreDefaults();

	//	Get our frame.
	RRealSize					topLeftInset( 0, 0 );
	RRealSize					bottomRightInset( 0, 0 );
	R2dTransform				frameTransform( transform );

	RFrame*						pFrame			= GetFrame( );
	YComponentBoundingRect	frameRect		= GetBoundingRect( );

	//	Get the component scale from the component bounding rect.
	YAngle				rotation;
	YRealDimension		xViewToFrameScale;
	YRealDimension		yViewToFrameScale;
	BOOLEAN				fFlipped = frameRect.GetTransform().Decompose( rotation, xViewToFrameScale, yViewToFrameScale );

	//	If I don't have a frame, don't bother playing with any transforms...
	if ( (GetFrameType() != kNoFrame) || HasBackgroundColor( ) )
	{
		//	Compute the insets from the frame	
		pFrame->GetInset( topLeftInset, bottomRightInset );
		RRealSize	realTopLeft( topLeftInset );
		RRealSize	realBottomRight( bottomRightInset );

		//	If there are components INSIDE of this one, we must unscale the transform 
		//	since it was modified in the ApplyTransform
		if ( m_ComponentViewCollection.Count() > 0 )
		{
			RRealSize	fullSize( frameRect.GetUntransformedSize() );
			RRealPoint	centerPoint( fullSize.m_dx/2.0, fullSize.m_dy/2.0 );
			RRealSize	newSize( fullSize.m_dx - bottomRightInset.m_dx - topLeftInset.m_dx, fullSize.m_dy - bottomRightInset.m_dy - topLeftInset.m_dy );
			frameTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, 1.0/xViewToFrameScale, 1.0/yViewToFrameScale );
			frameTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, fullSize.m_dx/newSize.m_dx, fullSize.m_dy/newSize.m_dy );
			frameTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, xViewToFrameScale, yViewToFrameScale );
			//
			//		Adjust the realBottomRight and realTopLeft to account for group translate
			realTopLeft.m_dx	= -(realBottomRight.m_dx - realTopLeft.m_dx) / 2.0;
			realTopLeft.m_dy	= -(realBottomRight.m_dy - realTopLeft.m_dy) / 2.0;
			realBottomRight	= realTopLeft;
		}

		//	If the component is flipped in any ways, remove the flipping..
		if ( m_fHorizontallyFlipped || m_fVerticallyFlipped )
		{
			//
			//		Remove the componentBounds transform from the passed in transform
			R2dTransform	invTransform			= frameRect.GetTransform( );
			invTransform.Invert( );
			R2dTransform	viewTransform			= invTransform * frameTransform;
			//	Reset frame transform
			frameTransform	= frameRect.GetTransform( );
			//	Compute the center point for any flipping that may occur
			RRealPoint centerPoint = frameRect.GetCenterPoint( );
			//	Remove any flip transformation from the frame if they are present
			if ( m_fHorizontallyFlipped )
			{
				frameTransform.PostScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, -1.0, 1.0 );
				YRealDimension	tmp	= realBottomRight.m_dx;
				realBottomRight.m_dx	= realTopLeft.m_dx;
				realTopLeft.m_dx		= -tmp;
			}
			if ( m_fVerticallyFlipped )
			{
				frameTransform.PostScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, 1.0, -1.0 );
				YRealDimension	tmp	= realBottomRight.m_dy;
				realBottomRight.m_dy	= realTopLeft.m_dy;
				realTopLeft.m_dy		= -tmp;
			}
			//
			//		If the transform when decomposed said it was flipped, we must take the angle and
			//		take its opposite
			if ( fFlipped )
			{
				frameTransform.PostRotateAboutPoint( centerPoint.m_x, centerPoint.m_y, -2*rotation );
			}
			//
			//		Put the component bounds transform back in.
			frameTransform = frameTransform * viewTransform;
		}
		//	Offset to proper location
		frameTransform.PreTranslate( -realTopLeft.m_dx, -realTopLeft.m_dy );
	}	

	//	Remove component scaling from the transform the frame will draw with.
	RRealSize	frameSize		= frameRect.GetUntransformedSize( );
	RRealRect	rExteriorRect( frameSize );

	DoRenderBackground( drawingSurface, transform );

	//	If we have a background color have the frame draw it.
	RColor rBackgroundColor;

#if 0
	if ( HasBackgroundColor() )
	{
		rBackgroundColor = GetBackgroundColor( );

		if ( GetFrameType() == kNoFrame )
		{
			RPath				path;
			(const_cast<RComponentView*>(this))->CalculateBorderPath( &path );
			RRealSize		pathSize( path.GetPathSize( ) );
			R2dTransform	pathTransform = frameRect.GetTransform();
			pathTransform.Invert( );
			pathTransform	= pathTransform * frameTransform;
			RColor			fillColor;
			if ( drawingSurface.GetColoringBookMode() )
				fillColor = RSolidColor( kWhite );	//	White fill
			else
				fillColor = rBackgroundColor;


//			RRealRect rColorRect( frameRect.m_TransformedBoundingRect.WidthHeight() );
			RRealRect rColorRect( GetReferenceSize() );
			R2dTransform xform = transform; //frameTransform;

			YAngle angle;
			YRealDimension xScale, yScale;
			GetBoundingRect().GetTransform().Decompose( angle, xScale, yScale );

			xScale = Abs( xScale );
			yScale = Abs( yScale );

			if (!AreFloatsEqual( xScale, 1.0 ) || !AreFloatsEqual( xScale, 1.0 ))
			{
				RRealPoint pt( rColorRect.GetCenterPoint() );

				R2dTransform rTransform;
				rTransform.PreScaleAboutPoint( pt.m_x, pt.m_y, xScale, yScale );
				rColorRect *= rTransform;

				xform.PreScaleAboutPoint( pt.m_x, pt.m_y, 1 / xScale, 1 / yScale );
			}

			fillColor.SetBoundingRect( rColorRect );
			fillColor *= xform;

			drawingSurface.SetFillColor( fillColor );
			drawingSurface.SetTint( GetComponentDocument( )->GetTint( ) );
			path.Render( drawingSurface, pathTransform, rExteriorRect, kFilled );
		}
		else
		{
			RRealRect rRect( rExteriorRect );
			R2dTransform xform( frameTransform );

			YAngle angle;
			YRealDimension xScale, yScale;
			GetBoundingRect().GetTransform().Decompose( angle, xScale, yScale );
		
			xScale = Abs( xScale );
			yScale = Abs( yScale );

			if (!AreFloatsEqual( xScale, 1.0 ) || !AreFloatsEqual( xScale, 1.0 ))
			{
				RRealPoint pt( rRect.GetCenterPoint() );
			
				R2dTransform rTransform;
				rTransform.PreScaleAboutPoint( pt.m_x, pt.m_y, xScale, yScale );
				rRect *= rTransform;

				xform.PreScaleAboutPoint( pt.m_x, pt.m_y, 1 / xScale, 1 / yScale );
			}

			drawingSurface.SetTint( GetComponentDocument( )->GetTint( ) );
			pFrame->Render( drawingSurface, xform, rRect, m_rFrameColor, rBackgroundColor, RFrame::kFrameFillInside );
//			pFrame->Render( drawingSurface, frameTransform, rExteriorRect, m_rFrameColor, rBackgroundColor, RFrame::kFrameFillInside );
		}
	}
#endif

	// Scope this section because we need curState destroyed before we render the frame
	{
		// Save the current drawing surface state
		RDrawingSurfaceState	curState( &drawingSurface );

 		// If we are not printing, set the clip region. We cant do this if we are printing, as
		// certain printers dont handle non-rectangular clip regions well and components can
		// be rotated.
		if (!drawingSurface.IsPrinting())
		{
			// GCB 6/3/98 - fix clipping bug in behind color rendering when there
			// was a shadow or glow with a behind color and double-outline frame
			R2dTransform tempTransform( transform );
			if (GetFrameType() != kNoFrame)
			{
				RFrame* pFrame = GetFrame();
				RRealSize topLeft, bottomRight;
				pFrame->GetInset( topLeft, bottomRight );
				tempTransform.PreTranslate( -topLeft.m_dx, -topLeft.m_dy );
			}

			SetClipRegion( drawingSurface, tempTransform );
		}

		// Call the base method to render
		RPaneView::Render( drawingSurface, transform, rcRender );
	}

	//	Draw the outside of the frame.
	if ( GetFrameType() != kNoFrame )
		pFrame->Render( drawingSurface, frameTransform, rExteriorRect, m_rFrameColor, rBackgroundColor, RFrame::kFrameDrawOutside );
}

// ****************************************************************************
//
//  Function Name:	RComponentView::DoRenderBackground( )
//
//  Description:		Background rendering
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::DoRenderBackground( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const
	{
	// REVIEW GCB 4/19/98 - this is derived from DoRender() above
	// it's a mess, but so was DoRender() and it's not clear how to untangle it

	//	Get our frame.
	RRealSize					topLeftInset( 0, 0 );
	RRealSize					bottomRightInset( 0, 0 );
	R2dTransform				frameTransform( transform );

	RFrame*						pFrame			= GetFrame( );
	YComponentBoundingRect	frameRect		= GetBoundingRect( );

	//	Get the component scale from the component bounding rect.
	YAngle				rotation;
	YRealDimension		xViewToFrameScale;
	YRealDimension		yViewToFrameScale;
	BOOLEAN				fFlipped = frameRect.GetTransform().Decompose( rotation, xViewToFrameScale, yViewToFrameScale );

	//	If I don't have a frame, don't bother playing with any transforms...
	if ( (GetFrameType() != kNoFrame) || HasBackgroundColor( ) )
	{
		//	Compute the insets from the frame	
		pFrame->GetInset( topLeftInset, bottomRightInset );
		RRealSize	realTopLeft( topLeftInset );
		RRealSize	realBottomRight( bottomRightInset );

		//	If there are components INSIDE of this one, we must unscale the transform 
		//	since it was modified in the ApplyTransform
		if ( m_ComponentViewCollection.Count() > 0 )
		{
			RRealSize	fullSize( frameRect.GetUntransformedSize() );
			RRealPoint	centerPoint( fullSize.m_dx/2.0, fullSize.m_dy/2.0 );
			RRealSize	newSize( fullSize.m_dx - bottomRightInset.m_dx - topLeftInset.m_dx, fullSize.m_dy - bottomRightInset.m_dy - topLeftInset.m_dy );
			frameTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, 1.0/xViewToFrameScale, 1.0/yViewToFrameScale );
			frameTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, fullSize.m_dx/newSize.m_dx, fullSize.m_dy/newSize.m_dy );
			frameTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, xViewToFrameScale, yViewToFrameScale );
			//
			//		Adjust the realBottomRight and realTopLeft to account for group translate
			realTopLeft.m_dx	= -(realBottomRight.m_dx - realTopLeft.m_dx) / 2.0;
			realTopLeft.m_dy	= -(realBottomRight.m_dy - realTopLeft.m_dy) / 2.0;
			realBottomRight	= realTopLeft;
		}

		//	If the component is flipped in any ways, remove the flipping..
		if ( m_fHorizontallyFlipped || m_fVerticallyFlipped )
		{
			//
			//		Remove the componentBounds transform from the passed in transform
			R2dTransform	invTransform			= frameRect.GetTransform( );
			invTransform.Invert( );
			R2dTransform	viewTransform			= invTransform * frameTransform;
			//	Reset frame transform
			frameTransform	= frameRect.GetTransform( );
			//	Compute the center point for any flipping that may occur
			RRealPoint centerPoint = frameRect.GetCenterPoint( );
			//	Remove any flip transformation from the frame if they are present
			if ( m_fHorizontallyFlipped )
			{
				frameTransform.PostScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, -1.0, 1.0 );
				YRealDimension	tmp	= realBottomRight.m_dx;
				realBottomRight.m_dx	= realTopLeft.m_dx;
				realTopLeft.m_dx		= -tmp;
			}
			if ( m_fVerticallyFlipped )
			{
				frameTransform.PostScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, 1.0, -1.0 );
				YRealDimension	tmp	= realBottomRight.m_dy;
				realBottomRight.m_dy	= realTopLeft.m_dy;
				realTopLeft.m_dy		= -tmp;
			}
			//
			//		If the transform when decomposed said it was flipped, we must take the angle and
			//		take its opposite
			if ( fFlipped )
			{
				frameTransform.PostRotateAboutPoint( centerPoint.m_x, centerPoint.m_y, -2*rotation );
			}
			//
			//		Put the component bounds transform back in.
			frameTransform = frameTransform * viewTransform;
		}
		//	Offset to proper location
		frameTransform.PreTranslate( -realTopLeft.m_dx, -realTopLeft.m_dy );
	}	

	//	Remove component scaling from the transform the frame will draw with.
	RRealSize	frameSize		= frameRect.GetUntransformedSize( );
	RRealRect	rExteriorRect( frameSize );

	//	If we have a background color have the frame draw it.
	RColor rBackgroundColor;
	if ( HasBackgroundColor() )
	{
		YTint nOldTint   = drawingSurface.GetTint();
		rBackgroundColor = GetBackgroundColor( );

		if ( GetFrameType() == kNoFrame )
		{
			RPath				path;
			(const_cast<RComponentView*>(this))->CalculateBorderPath( &path );
			RRealSize		pathSize( path.GetPathSize( ) );
			R2dTransform	pathTransform = frameRect.GetTransform();
			pathTransform.Invert( );
			pathTransform	= pathTransform * frameTransform;
			RColor			fillColor;
			if ( drawingSurface.GetColoringBookMode() )
				fillColor = RSolidColor( kWhite );	//	White fill
			else
				fillColor = rBackgroundColor;


//			RRealRect rColorRect( frameRect.m_TransformedBoundingRect.WidthHeight() );
			RRealRect rColorRect( GetReferenceSize() );
			R2dTransform xform = transform; //frameTransform;

			YAngle angle;
			YRealDimension xScale, yScale;
			GetBoundingRect().GetTransform().Decompose( angle, xScale, yScale );

			xScale = Abs( xScale );
			yScale = Abs( yScale );

			if (!AreFloatsEqual( xScale, 1.0 ) || !AreFloatsEqual( xScale, 1.0 ))
			{
				RRealPoint pt( rColorRect.GetCenterPoint() );

				R2dTransform rTransform;
				rTransform.PreScaleAboutPoint( pt.m_x, pt.m_y, xScale, yScale );
				rColorRect *= rTransform;

				xform.PreScaleAboutPoint( pt.m_x, pt.m_y, 1 / xScale, 1 / yScale );
			}

			fillColor.SetBoundingRect( rColorRect );
			fillColor *= xform;

			drawingSurface.SetFillColor( fillColor );
			drawingSurface.SetTint( GetComponentDocument( )->GetTint( ) );
			path.Render( drawingSurface, pathTransform, rExteriorRect, kFilled );
		}
		else
		{
			RRealRect rRect( rExteriorRect );
			R2dTransform xform( frameTransform );

			YAngle angle;
			YRealDimension xScale, yScale;
			GetBoundingRect().GetTransform().Decompose( angle, xScale, yScale );
		
			xScale = Abs( xScale );
			yScale = Abs( yScale );

			if (!AreFloatsEqual( xScale, 1.0 ) || !AreFloatsEqual( xScale, 1.0 ))
			{
				RRealPoint pt( rRect.GetCenterPoint() );
			
				R2dTransform rTransform;
				rTransform.PreScaleAboutPoint( pt.m_x, pt.m_y, xScale, yScale );
				rRect *= rTransform;

				xform.PreScaleAboutPoint( pt.m_x, pt.m_y, 1 / xScale, 1 / yScale );
			}

			drawingSurface.SetTint( GetComponentDocument( )->GetTint( ) );
			pFrame->Render( drawingSurface, xform, rRect, m_rFrameColor, rBackgroundColor, RFrame::kFrameFillInside );
//			pFrame->Render( drawingSurface, frameTransform, rExteriorRect, m_rFrameColor, rBackgroundColor, RFrame::kFrameFillInside );
		}

		drawingSurface.SetTint( nOldTint );
	}
}

// ****************************************************************************
//
//  Function Name:	RComponentView::RenderFrame( )
//
//  Description:		Frame rendering
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::RenderFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RSolidColor& frameColor ) const
{
	// REVIEW GCB 6/1/98 - this is derived from DoRender() above
	// it's a mess, but so was DoRender() and it's not clear how to untangle it

	//	If I don't have a frame, don't bother
	if ((GetFrameType() != kNoFrame))
	{
		//	Get our frame.
		RRealSize					topLeftInset( 0, 0 );
		RRealSize					bottomRightInset( 0, 0 );
		R2dTransform				frameTransform( transform );

		RFrame*						pFrame			= GetFrame( );
		YComponentBoundingRect	frameRect		= GetBoundingRect( );

		//	Get the component scale from the component bounding rect.
		YAngle				rotation;
		YRealDimension		xViewToFrameScale;
		YRealDimension		yViewToFrameScale;
		BOOLEAN				fFlipped = frameRect.GetTransform().Decompose( rotation, xViewToFrameScale, yViewToFrameScale );

		//	Compute the insets from the frame	
		pFrame->GetInset( topLeftInset, bottomRightInset );
		RRealSize	realTopLeft( topLeftInset );
		RRealSize	realBottomRight( bottomRightInset );

		//	If there are components INSIDE of this one, we must unscale the transform 
		//	since it was modified in the ApplyTransform
		if ( m_ComponentViewCollection.Count() > 0 )
		{
			RRealSize	fullSize( frameRect.GetUntransformedSize() );
			RRealPoint	centerPoint( fullSize.m_dx/2.0, fullSize.m_dy/2.0 );
			RRealSize	newSize( fullSize.m_dx - bottomRightInset.m_dx - topLeftInset.m_dx, fullSize.m_dy - bottomRightInset.m_dy - topLeftInset.m_dy );
			frameTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, 1.0/xViewToFrameScale, 1.0/yViewToFrameScale );
			frameTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, fullSize.m_dx/newSize.m_dx, fullSize.m_dy/newSize.m_dy );
			frameTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, xViewToFrameScale, yViewToFrameScale );
			//
			//		Adjust the realBottomRight and realTopLeft to account for group translate
			realTopLeft.m_dx	= -(realBottomRight.m_dx - realTopLeft.m_dx) / 2.0;
			realTopLeft.m_dy	= -(realBottomRight.m_dy - realTopLeft.m_dy) / 2.0;
			realBottomRight	= realTopLeft;
		}

		//	If the component is flipped in any ways, remove the flipping..
		if ( m_fHorizontallyFlipped || m_fVerticallyFlipped )
		{
			//
			//		Remove the componentBounds transform from the passed in transform
			R2dTransform	invTransform			= frameRect.GetTransform( );
			invTransform.Invert( );
			R2dTransform	viewTransform			= invTransform * frameTransform;
			//	Reset frame transform
			frameTransform	= frameRect.GetTransform( );
			//	Compute the center point for any flipping that may occur
			RRealPoint centerPoint = frameRect.GetCenterPoint( );
			//	Remove any flip transformation from the frame if they are present
			if ( m_fHorizontallyFlipped )
			{
				frameTransform.PostScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, -1.0, 1.0 );
				YRealDimension	tmp	= realBottomRight.m_dx;
				realBottomRight.m_dx	= realTopLeft.m_dx;
				realTopLeft.m_dx		= -tmp;
			}
			if ( m_fVerticallyFlipped )
			{
				frameTransform.PostScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, 1.0, -1.0 );
				YRealDimension	tmp	= realBottomRight.m_dy;
				realBottomRight.m_dy	= realTopLeft.m_dy;
				realTopLeft.m_dy		= -tmp;
			}
			//
			//		If the transform when decomposed said it was flipped, we must take the angle and
			//		take its opposite
			if ( fFlipped )
			{
				frameTransform.PostRotateAboutPoint( centerPoint.m_x, centerPoint.m_y, -2*rotation );
			}
			//
			//		Put the component bounds transform back in.
			frameTransform = frameTransform * viewTransform;
		}
		//	Offset to proper location
		frameTransform.PreTranslate( -realTopLeft.m_dx, -realTopLeft.m_dy );

		//	Remove component scaling from the transform the frame will draw with.
		RRealSize	frameSize		= frameRect.GetUntransformedSize( );
		RRealRect	rExteriorRect( frameSize );

		RColor rBackgroundColor( GetBackgroundColor( ) );
		pFrame->Render( drawingSurface, frameTransform, rExteriorRect, frameColor, rBackgroundColor, RFrame::kFrameDrawOutside );
	}
}

#ifndef TPSDEBUG
// Turn off global optimizations to fix tiling offset problem (At least for Leapfrog)
#pragma optimize( "g", off )
#endif
// ****************************************************************************
//
//  Function Name:	RComponentView::RenderDocument( )
//
//  Description:		Render this view's document.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::RenderDocument( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender ) const
{
	drawingSurface.SetForegroundMode( kNormal );

	if (IsViewTiled())
	{
		//
		//	Get the size of the view, and the Interior Size
		RRealSize		viewSize( GetBoundingRect( ).WidthHeight( ) );
		RRealSize		renderSize( GetReferenceSize( ) );
		//	First, check to see if this tile will be visible (larger than 1 pixel)
		//		If it is not visible, don't bother doing anything
		//RRealSize		deviceSize	= viewSize * transform;
		//if ( (deviceSize.m_dx <= 1) || (deviceSize.m_dy <= 1) )
		//	return;

		// Create a tiling render cache
		RTilingRenderCache tilingRenderCache( this );

		// Get the tile attributes
		RTileAttributes rAttributes = GetTileAttributes();
		RTileAttributesExt rAttributesExt = GetTileAttributesExt();

		RRealSize		transformTranslate;
		//
		//	Create a new transform to be the base for all tiles
		R2dTransform	tileTransform( transform );
		//
		//	Get the amount of space that each tile will occupy on the screen (in logical units)
		RRealSize		imageTileSize( rAttributes.m_rTileSize.m_dx+rAttributes.m_rTileSpacing.m_dx, rAttributes.m_rTileSize.m_dy+rAttributes.m_rTileSpacing.m_dy );
		// Compute an offset so that all tiles meet in the center if specified. (the default)
		if (rAttributesExt.GetTilingStyle() == TS_CENTERED)
		{
			transformTranslate.m_dx	= (fmod( viewSize.m_dx, imageTileSize.m_dx ) - imageTileSize.m_dx - rAttributes.m_rTileSpacing.m_dx ) / 2.0;
			transformTranslate.m_dy	= (fmod( viewSize.m_dy, imageTileSize.m_dy ) - imageTileSize.m_dy - rAttributes.m_rTileSpacing.m_dy ) / 2.0;
		}
		//
		//	Add the scaling from view size to tile size into the transform
		//tileTransform.PreScale( rAttributes.m_rTileSize.m_dx/viewSize.m_dx, rAttributes.m_rTileSize.m_dy/viewSize.m_dy );
		tileTransform.PreScale( imageTileSize.m_dx/viewSize.m_dx, imageTileSize.m_dy/viewSize.m_dy );
		//
		//	Offset the transform to account for the above calculation (if performed)
		tileTransform.PreTranslate( transformTranslate.m_dx, transformTranslate.m_dy );
		//
		//	Transform the component origin(0,0) to device coordinates and make sure
		//		that it falls on an absolute device boundry
		RRealPoint	rptOrigin( RRealPoint(0,0)*tileTransform );
		rptOrigin.m_x	= rptOrigin.m_x - floor( rptOrigin.m_x );
		rptOrigin.m_y	= rptOrigin.m_y - floor( rptOrigin.m_y );
		tileTransform.PostTranslate( -rptOrigin.m_x, -rptOrigin.m_y );
		//
		//	What is the offset for each Render
		RRealSize		offsetSize( renderSize * tileTransform );
		offsetSize.m_dx	= floor( offsetSize.m_dx );
		offsetSize.m_dy	= floor( offsetSize.m_dy );
		R2dTransform	invTileTransform( tileTransform );
		invTileTransform.Invert( );
		offsetSize *= invTileTransform;
		RRealSize		transformScale( rAttributes.m_rTileSize.m_dx/imageTileSize.m_dx, rAttributes.m_rTileSize.m_dy/imageTileSize.m_dy );
		//
		//	Compute the number of tiles
		RIntSize			tileCount( (viewSize.m_dx/imageTileSize.m_dx/transformScale.m_dx)+1, (viewSize.m_dy/imageTileSize.m_dy/transformScale.m_dy)+1 );
		//
		//	Loop across the viewSize until we have covered the surface
		//		and then cover the next row until we are done
		transformTranslate.m_dy	= 0;
		for( int iRow = 0; iRow <= tileCount.m_dy; ++iRow )
		{
			transformTranslate.m_dx	= 0;
			for( int iCol = 0; iCol <= tileCount.m_dx; ++iCol )
			{
				R2dTransform	renderTransform( tileTransform );
				//
				//	Offset to proper location
				renderTransform.PreTranslate( transformTranslate.m_dx, transformTranslate.m_dy );
				//
				//	Translate from imageTileSize to tileSize
				renderTransform.PreScale( transformScale.m_dx, transformScale.m_dy );

				//	Make sure the tile is actually in the rcRender
				RRealVectorRect	deviceViewRect( RRealVectorRect(viewSize) * renderTransform );
				//	Render the document and possible shadow
				if ( rcRender.IsIntersecting( RIntRect( deviceViewRect.m_TransformedBoundingRect ) ) )
					tilingRenderCache.Render( drawingSurface, renderSize, renderTransform, rcRender );
				//
				//		Adjust to end loop and/or setup for next render
				transformTranslate.m_dx	+= offsetSize.m_dx;
			}
			transformTranslate.m_dy	+= offsetSize.m_dy;
		}
	}
	else
	{
		// Tell the document to render
		RenderDocumentAndSoftEffects(drawingSurface, transform, rcRender, GetReferenceSize());
	}
}

// ****************************************************************************
//
//  Function Name:	RComponentView::RenderDocumentAndSoftEffects( )
//
//  Description:		Make room for the shadow or glow, if needed, and draw the
//							document with glow, shadow, vignette (soft edge) and
//							alpha mask
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::RenderDocumentAndSoftEffects(
	RDrawingSurface&			drawingSurface,
	const R2dTransform&		transform,
	const RIntRect&			rcRender,
	const RRealSize&			size ) const
{
	R2dTransform	docTransform( transform );
	BOOLEAN			revertToNoSoftEffects( FALSE );

	// render the "behind soft effect" (such as the silhouette of a headline)
	GetRDocument()->RenderBehindSoftEffect( drawingSurface, docTransform, rcRender, size );

	// If this is an active text block (being edited)
	// don't render soft effects.
	YComponentType componentType = GetComponentDocument()->GetComponentType();
	if (IsActive() && (componentType == kTextComponent || componentType == kReturnAddressComponent))
		revertToNoSoftEffects = TRUE;

	if (!revertToNoSoftEffects && (HasShadow() || HasGlow() || HasVignette() || HasAlphaMask()))
	{
		YRealDimension yShadowXOffset( 0.0 );
		YRealDimension yShadowYOffset( 0.0 );
		R2dTransform	shadowTransform;
		YPercentage		shadowEdgeScale( 1.0 );
		uLONG				vignetteBlurRadius( 0 );

		// If this view has a glow and it wants the glow inside its bounds,
		// shrink and offset the view before rendering its glow and original
		// appearance.
		if (HasGlow() && WantsGlowAndShadowInsideBounds())
		{
			// x and y glow scaling uses glow edge softness as percent of scaling range
			// x and y offsets use x and y scaling of the "soft effect dimension"
			// (generally the min of the component dimensions, except for text and
			// headlines where it is the average line height)
			YPercentage glowScale( BilinearInterpolateValue(
				kMinGlowEdgeSoftness,
				kMidPointGlowEdgeSoftness,
				kMaxGlowEdgeSoftness,
				m_pGlowSettings->m_fGlowEdgeSoftness ) * 4.0 );
			YRealDimension scaledXObj;
			YRealDimension scaledYObj;
			if (size.m_dx < size.m_dy)
			{
				scaledXObj = size.m_dx / (glowScale + 1.0);
				scaledYObj = size.m_dy - (size.m_dx - scaledXObj);
			}
			else
			{
				scaledYObj = size.m_dy / (glowScale + 1.0);
				scaledXObj = size.m_dx - (size.m_dy - scaledYObj);
			}
			YRealDimension yGlowXScale( scaledXObj / size.m_dx );
			YRealDimension yGlowYScale( scaledYObj / size.m_dy );
			//
			// use proportional (aspect-correct) scaling
			if (yGlowXScale < yGlowYScale)
				yGlowYScale = yGlowXScale;
			else
				yGlowXScale = yGlowYScale;

			YRealDimension yGlowXOffset( size.m_dx * (1.0 - yGlowXScale) / 2.0 );
			YRealDimension yGlowYOffset( size.m_dy * (1.0 - yGlowYScale) / 2.0 );
			//
			//	position the view to account for the glow
			docTransform.PreTranslate( yGlowXOffset, yGlowYOffset );
			//
			//	Shrink the view so it and it's glow fit in the original rcRender.
			docTransform.PreScale( yGlowXScale, yGlowYScale );
		}

		// If this view has a shadow and it wants the shadow inside its bounds,
		// shrink and offset the view before rendering its shadow and original
		// appearance. If the view has a shadow but doesn't want the shadow
		// inside its bounds, just set up the shadow transform
		if (HasShadow())
		{
			// x and y shadow scaling uses shadowAngle plus shadowOffset as percent of scaling range
			// x and y offsets use x and y scaling of the "soft effect dimension"
			// (generally the min of the component dimensions, except for text and
			// headlines where it is the average line height)
			YRealDimension cosa( ::cos( m_pShadowSettings->m_fShadowAngle ) );
			YRealDimension sina( ::sin( m_pShadowSettings->m_fShadowAngle ) );
			YPercentage shadowScale = BilinearInterpolateValue(
				kMinShadowOffset,
				kMidPointShadowOffset,
				kMaxShadowOffset,
				m_pShadowSettings->m_fShadowOffset );

			YRealDimension softEffectDimension( SoftEffectDimension( size ) );
			YRealDimension shadowBlurRadius( 0.0 );

			if (m_pShadowSettings->m_fShadowEdgeSoftness > 0.0)
			{
				shadowEdgeScale = BilinearInterpolateValue(
					kMinShadowEdgeSoftness,
					kMidPointShadowEdgeSoftness,
					kMaxShadowEdgeSoftness,
					m_pShadowSettings->m_fShadowEdgeSoftness );

				if (WantsGlowAndShadowInsideBounds())
					shadowBlurRadius = softEffectDimension * shadowEdgeScale;
			}

			YRealDimension hyp( softEffectDimension * shadowScale );
			yShadowXOffset = cosa * hyp;
			yShadowYOffset = sina * hyp;
			YRealDimension yShadowYCenteringOffset( 0.0 );
			YRealDimension yShadowXCenteringOffset( 0.0 );
			YRealDimension yShadowXScale( (::Abs( yShadowXOffset ) + shadowBlurRadius) / size.m_dx );
			YRealDimension yShadowYScale( (::Abs( yShadowYOffset ) + shadowBlurRadius) / size.m_dy );

			if (WantsGlowAndShadowInsideBounds())
			{
				// use proportional (aspect-correct) scaling
				if (yShadowXScale > yShadowYScale)
					yShadowYScale = yShadowXScale;
				else
					yShadowXScale = yShadowYScale;

				// recalculate shadow offsets in post-shadow-scaling units
				softEffectDimension *= (1.0 - yShadowXScale);
				shadowBlurRadius = softEffectDimension * shadowEdgeScale;
				hyp = softEffectDimension * shadowScale;
				yShadowXOffset = cosa * hyp;
				yShadowYOffset = sina * hyp;

				if (size.m_dx < size.m_dy)
					yShadowYCenteringOffset = ((yShadowYScale * size.m_dy) - (::Abs( yShadowYOffset ) + shadowBlurRadius)) / 2.0;
				else
					yShadowXCenteringOffset = ((yShadowXScale * size.m_dx) - (::Abs( yShadowXOffset ) + shadowBlurRadius)) / 2.0;

				//	Shrink the view so it and it's shadow fit in the original rcRender.
				docTransform.PreScale( 1.0 - yShadowXScale, 1.0 - yShadowYScale );

				// Offset the view so it's centered
				docTransform.PreTranslate( yShadowXCenteringOffset, yShadowYCenteringOffset );
			}

			//	If component is flipped, special case creation of the shadow Transform
			//	Remove any flip transformation from the frame if they are present
			if ( m_fHorizontallyFlipped )
				yShadowXOffset	= -yShadowXOffset;
			if ( m_fVerticallyFlipped )
				yShadowYOffset	= -yShadowYOffset;

			if (WantsGlowAndShadowInsideBounds())
			{
				// position the view to account for the shadow
				if (yShadowXOffset < 0)
					docTransform.PreTranslate( -yShadowXOffset, 0 );
				if (yShadowYOffset < 0)
					docTransform.PreTranslate( 0, -yShadowYOffset );
			}

			//	Create the Shadow transform
			shadowTransform = docTransform;
			shadowTransform.PreTranslate( yShadowXOffset, yShadowYOffset );
		}

		// if drawingSurface is not an offscreen drawing surface or it doesn't
		// have an RBitmapImage just render without soft effects
		// (this is how soft effects rendering was disabled for PressWriter 2.0 and Web Site Designer 1.0)
		ROffscreenDrawingSurface* destinationOffscreenDS = dynamic_cast<ROffscreenDrawingSurface*>(&drawingSurface);
		if (!destinationOffscreenDS)
			revertToNoSoftEffects = TRUE;
		RBitmapImage* destinationBitmap = NULL;
		if (destinationOffscreenDS)
		{
			destinationBitmap = destinationOffscreenDS->GetBitmapImage();
			if (!destinationBitmap)
				revertToNoSoftEffects = TRUE;
		}

		// if we're in coloring book mode, revert to no soft effects
		if (drawingSurface.GetColoringBookMode())
			revertToNoSoftEffects = TRUE;

		if (!revertToNoSoftEffects)
		{
			ROffscreenDrawingSurface*	maskOffscreenDS = NULL;
			RBitmapImage*					maskBitmap = NULL;
			ROffscreenDrawingSurface*	srcOffscreenDS = NULL;
			RBitmapImage*					srcBitmap = NULL;
			ROffscreenDrawingSurface*	alphaOffscreenDS = NULL;
			RBitmapImage*					alphaComponentBitmap = NULL;
			RBitmapImage*					alphaMaskBitmap = NULL;
			RBitmapImage*					alphaTransformedMask = NULL;
			RBitmapImage*					alphaBogusMask = NULL;
			RBitmapImage*					alphaClipMask = NULL;
			RIntRect							alphaDstRect;

			// determine bitmapRect
			RIntSize bitmapSize( destinationBitmap->GetWidthInPixels(), destinationBitmap->GetHeightInPixels() );
			RIntRect bitmapRect( bitmapSize );

			try
			{
				// create 8-bit maskOffscreenDS,maskBitmap at bitmapRect size
				maskOffscreenDS = new ROffscreenDrawingSurface( drawingSurface.IsPrinting(), TRUE, &drawingSurface );
				maskBitmap = new RBitmapImage;
				maskBitmap->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 8, kDefaultXDpi, kDefaultYDpi, kUseGrayScalePalette );
				maskOffscreenDS->SetImage( maskBitmap );

				if (HasAlphaMask())
				{
					// GCB 5/29/98
					// set up alphaComponentBitmap and alphaMaskBitmap, which may be
					// used by shadow & glow below, but delay actual rendering of
					// alphaComponentBitmap,alphaMaskBitmap until after shadow & glow

					alphaOffscreenDS = new ROffscreenDrawingSurface( drawingSurface.IsPrinting(), TRUE, &drawingSurface );
					alphaComponentBitmap = new RBitmapImage;
					alphaMaskBitmap = new RBitmapImage;
					alphaTransformedMask = new RBitmapImage;
					alphaBogusMask = new RBitmapImage;

					// Create 24-bit alphaComponentBitmap and init to white
					alphaComponentBitmap->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 24 );
					
					alphaOffscreenDS->SetImage( alphaComponentBitmap );
					alphaOffscreenDS->SetFillColor( RSolidColor( kWhite ) );
					alphaOffscreenDS->FillRectangle( bitmapRect );

					// save behind color, set to none and restore later so it doesn't
					// interfere with rendering the alpha mask or document rendering
					RColor oldBackgroundColor = GetBackgroundColor();
					const_cast<RComponentView*>(this)->SetBackgroundColor( RColor() );

					// Render current component into alphaComponentBitmap
					GetRDocument()->Render( *alphaOffscreenDS, docTransform, bitmapRect, size );

					// Setup alphaMaskBitmap 
					alphaMaskBitmap->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 8, kDefaultXDpi, kDefaultYDpi, kUseGrayScalePalette );

					// Compute the destination rect
					RRealVectorRect rComponentRect(size);
					rComponentRect *= docTransform;	
					alphaDstRect = rComponentRect.m_TransformedBoundingRect;

					// Get the size of the bitmap
					RIntSize rBitmapSize(m_pAlphaMask->GetWidthInPixels(), m_pAlphaMask->GetHeightInPixels());

					// Scale the transform so that we are mapping pixels to device units	
					R2dTransform rBitmapTransform = docTransform;
					rBitmapTransform.PreScale((YFloatType)size.m_dx / (YFloatType)rBitmapSize.m_dx, (YFloatType)size.m_dy / (YFloatType)rBitmapSize.m_dy);
					
					// Translate the origin so that the rotated bitmap is mapped into the right
					// place in the destination bitmap
					rBitmapTransform.PostTranslate(-alphaDstRect.m_Left, -alphaDstRect.m_Top);

					// Transform (rotate and/or flip) the bitmap
					RImageLibrary().Transform(*m_pAlphaMask, *alphaTransformedMask, *alphaBogusMask, rBitmapTransform);

					RIntRect alphaSrcRect( 0, 0,
											 alphaTransformedMask->GetWidthInPixels(), 
											 alphaTransformedMask->GetHeightInPixels() );

					alphaOffscreenDS->ReleaseImage();
					alphaOffscreenDS->SetImage( alphaMaskBitmap );
					alphaTransformedMask->Render( *alphaOffscreenDS, alphaSrcRect, alphaDstRect );

					// create an alpha mask to limit the effect to the components
					// clipping region
					alphaClipMask = new RBitmapImage;
					alphaClipMask->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 8, kDefaultXDpi, kDefaultYDpi, kUseGrayScalePalette );

					void * pClipMaskRawData   = alphaClipMask->GetRawData();
					::memset( RBitmapImage::GetImageData(pClipMaskRawData), 0x00, 
							  RBitmapImage::GetImageDataSize( pClipMaskRawData ) );

					alphaOffscreenDS->ReleaseImage();
					alphaOffscreenDS->SetImage( alphaClipMask );
					RenderShadowInSingleLayer( *alphaOffscreenDS, docTransform, bitmapRect, size, RSolidColor( kWhite ));

					// restore behind color
					const_cast<RComponentView*>(this)->SetBackgroundColor( oldBackgroundColor );

					// Combine the alpha mask with the clipping mask

					BYTE *	pClipMaskData		 = (BYTE *)RBitmapImage::GetImageData( alphaClipMask->GetRawData() );
					BYTE *  pAlphaMaskData		 = (BYTE *)RBitmapImage::GetImageData( alphaMaskBitmap->GetRawData() );
					int		size				 = RBitmapImage::GetImageDataSize( alphaClipMask->GetRawData() );

					for (int i=0; i<size; i++)
					{
						if(pClipMaskData[i] == 0)
							pAlphaMaskData[i] = 0;
					}
				}

				if (HasVignette())
				{
					// vignette blur radius is percentage of the "soft effect dimension"
					// (generally the min of the component dimensions, except for text and
					// headlines where it is the average line height)
					// with vignetteEdgeSoftness as the percentage
					// and if vignetteEdgeSoftness=0 then radius=0 ("hard edge" vignette),
					// else minimum radius = 1
					if (m_pVignetteSettings->m_fVignetteEdgeSoftness > 0.0)
					{
						YPercentage edgeScale( BilinearInterpolateValue(
							kMinVignetteEdgeSoftness,
							kMidPointVignetteEdgeSoftness,
							kMaxVignetteEdgeSoftness,
							m_pVignetteSettings->m_fVignetteEdgeSoftness ) );
						YRealDimension softEffectDimension( SoftEffectDimension( size ) );
						RRealSize softEffectSize( softEffectDimension, softEffectDimension );
						RRealVectorRect softEffectRect( softEffectSize );
						softEffectRect *= docTransform;
						vignetteBlurRadius = ::Round( softEffectRect.Width() * edgeScale );
						vignetteBlurRadius = ::Max( (uLONG)1, vignetteBlurRadius );
					}
				}

				if (HasGlow())
				{
					// init maskBitmap to black
					::memset( RBitmapImage::GetImageData( maskBitmap->GetRawData()), 0x00, RBitmapImage::GetImageDataSize( maskBitmap->GetRawData() ) );

					// render the document to maskOffscreenDS
					// glow opacity is 0-255 gray scale color calculated as sum of the
					// glow opacity color percentage + glow opacity white percentage
					// at the current opacity setting
					YPercentage glowOpacityColorPercentage( BilinearInterpolateValue(
						kMinGlowOpacityColor,
						kMidPointGlowOpacityColor,
						kMaxGlowOpacityColor,
						m_pGlowSettings->m_fGlowOpacity ) );
					YPercentage glowOpacityWhitePercentage( BilinearInterpolateValue(
						kMinGlowOpacityWhite,
						kMidPointGlowOpacityWhite,
						kMaxGlowOpacityWhite,
						m_pGlowSettings->m_fGlowOpacity ) );
					YPercentage glowOpacityPercentageSum( glowOpacityColorPercentage + glowOpacityWhitePercentage );
					if (::AreFloatsEqual( glowOpacityPercentageSum, 1.0 ))
						glowOpacityPercentageSum = 1.0; // guard against floating point imprecision
					YColorComponent glowOpacityColorComponent( kMaxColorComponent * glowOpacityPercentageSum );
					RSolidColor glowOpacityColor( glowOpacityColorComponent, glowOpacityColorComponent, glowOpacityColorComponent );
					RenderShadowInSingleLayer( *maskOffscreenDS, docTransform, bitmapRect, size, glowOpacityColor );

					if (!HasBackgroundColor())
					{
						// if we don't have a behind color...
						if (HasAlphaMask())
						{
							// if we have an alpha mask, composite it with the glow
							RImageLibrary().AlphaBlend( *alphaMaskBitmap, bitmapRect, *maskBitmap, bitmapRect );

							// alpha mask doesn't include the frame so render the frame
							// into maskBitmap
							if (GetFrameType() != kNoFrame)
								RenderFrame( *maskOffscreenDS, docTransform, glowOpacityColor );
						}
						else if (vignetteBlurRadius > 0)
						{
							// if we have a vignette, blur glow inward
							RImageLibrary().Blur( *maskBitmap, *maskBitmap, vignetteBlurRadius, blurIn, boxFilter );

							// reapply frame - they're so thin they tend to disappear when vignetted
							if (GetFrameType() != kNoFrame)
								RenderFrame( *maskOffscreenDS, docTransform, glowOpacityColor );
						}
					}

					// blur glow edges outward in maskOffscreenDS
					// blur radius is percentage of the "soft effect dimension"
					// (generally the min of the component dimensions, except for text and
					// headlines where it is the average line height)
					// with glowEdgeSoftness as the percentage
					YPercentage edgeScale( BilinearInterpolateValue(
						kMinGlowEdgeSoftness,
						kMidPointGlowEdgeSoftness,
						kMaxGlowEdgeSoftness,
						m_pGlowSettings->m_fGlowEdgeSoftness ) );
					YRealDimension softEffectDimension( SoftEffectDimension( size ) );
					RRealSize softEffectSize( softEffectDimension, softEffectDimension );
					RRealVectorRect softEffectRect( softEffectSize );
					softEffectRect *= docTransform;
					uLONG blurRadius = ::Round( softEffectRect.Width() * edgeScale );
					blurRadius = ::Max( (uLONG)1, blurRadius );
					RImageLibrary().Blur( *maskBitmap, *maskBitmap, blurRadius, blurOut, boxFilter );

#if 0 // debug: set to 1 to display maskOffscreenDS to destinationOffscreenDS
					destinationOffscreenDS->BlitDrawingSurface( *maskOffscreenDS, bitmapRect, bitmapRect, kBlitSourceCopy );
#else
					// blend from user-selected glow color + white
					// into destinationBitmap (belonging to destinationOffscreenDS)
					// thru maskBitmap
					YPercentage whiteMixPercentage( glowOpacityWhitePercentage / glowOpacityColorPercentage );
					RSolidColor glowColor( m_pGlowSettings->m_fGlowColor );
					glowColor.LightenBy( whiteMixPercentage );

#if 0
	clock_t start = clock();
#endif
					RImageLibrary().AlphaBlend( glowColor, *destinationBitmap, bitmapRect, *maskBitmap, bitmapRect );
#if 0
	clock_t finish = clock();
	TRACE("Glow Blend required: %f ticks\n", (double)finish - start);
#endif

#endif // 0

					// if there's a behind color or frame, and no shadow, render the
					// behind color or frame again in front of the glow (they were
					// already rendered, but behind the glow, from DoRender(); if
					// there's a behind color or frame, and a shadow, the behind
					// color and frame will be rendered below after the shadow
					if (!HasShadow())
					{
						if (HasBackgroundColor())
							DoRenderBackground( *destinationOffscreenDS, docTransform );
						if (GetFrameType() != kNoFrame)
							RenderFrame( *destinationOffscreenDS, docTransform, m_rFrameColor );
					}
				}

				if (HasShadow())
				{
					// init maskBitmap to black
					::memset( RBitmapImage::GetImageData( maskBitmap->GetRawData()), 0x00, RBitmapImage::GetImageDataSize( maskBitmap->GetRawData() ) );

					// render the shadow to maskOffscreenDS:
					// shadow opacity is 0-255 gray scale color calculated as sum of the
					// shadow opacity color percentage + shadow opacity black percentage
					// at the current opacity setting
					YPercentage shadowOpacityColorPercentage( BilinearInterpolateValue(
						kMinShadowOpacityColor,
						kMidPointShadowOpacityColor,
						kMaxShadowOpacityColor,
						m_pShadowSettings->m_fShadowOpacity ) );
					YPercentage shadowOpacityBlackPercentage( BilinearInterpolateValue(
						kMinShadowOpacityBlack,
						kMidPointShadowOpacityBlack,
						kMaxShadowOpacityBlack,
						m_pShadowSettings->m_fShadowOpacity ) );
					YPercentage shadowOpacityPercentageSum( shadowOpacityColorPercentage + shadowOpacityBlackPercentage );
					if (::AreFloatsEqual( shadowOpacityPercentageSum, 1.0 ))
						shadowOpacityPercentageSum = 1.0; // guard against floating point imprecision
					YColorComponent shadowOpacityColorComponent( kMaxColorComponent * shadowOpacityPercentageSum );
					RSolidColor shadowOpacityColor( shadowOpacityColorComponent, shadowOpacityColorComponent, shadowOpacityColorComponent );
					RenderShadowInSingleLayer( *maskOffscreenDS, shadowTransform, bitmapRect, size, shadowOpacityColor );

					if (!HasBackgroundColor())
					{
						// if we don't have a behind color...
						if (HasAlphaMask())
						{
							// if we have an alpha mask, composite it with the shadow
							RRealSize shadowOffset( yShadowXOffset, yShadowYOffset );
							YRealDimension	rotation, xShadowScale, yShadowScale;
							shadowTransform.Decompose( rotation, xShadowScale, yShadowScale );
							shadowOffset.m_dx *= xShadowScale;
							shadowOffset.m_dy *= yShadowScale;

							RIntRect shadowDstRect( bitmapRect );
							shadowDstRect.Offset( RIntSize( shadowOffset ) );
							shadowDstRect.Intersect( bitmapRect, shadowDstRect );

							RRealSize negShadowOffset( -yShadowXOffset, -yShadowYOffset );
							negShadowOffset.m_dx *= xShadowScale;
							negShadowOffset.m_dy *= yShadowScale;
							RIntRect alphaRect( shadowDstRect );
							alphaRect.Offset( RIntSize( negShadowOffset ) );

							RImageLibrary().AlphaBlend( *alphaMaskBitmap, alphaRect, *maskBitmap, shadowDstRect );

							// alpha mask doesn't include the frame so render the frame
							// into maskBitmap
							if (GetFrameType() != kNoFrame)
								RenderFrame( *maskOffscreenDS, shadowTransform, shadowOpacityColor );
						}
						else if (vignetteBlurRadius > 0)
						{
							// if we have a vignette, blur shadow inward
							RImageLibrary().Blur( *maskBitmap, *maskBitmap, vignetteBlurRadius, blurIn, boxFilter );

							// reapply frame - they're so thin they tend to disappear when vignetted
							if (GetFrameType() != kNoFrame)
								RenderFrame( *maskOffscreenDS, shadowTransform, shadowOpacityColor );
						}
					}

					// blur shadow edges inward and outward in maskOffscreenDS
					// blur radius is percentage of the "soft effect dimension"
					// (generally the min of the component dimensions, except for text and
					// headlines where it is the average line height)
					// with shadowEdgeSoftness as the percentage
					// and if shadowEdgeSoftness=0 then radius=0 ("hard edge" shadow),
					// else minimum radius = 1
					if (m_pShadowSettings->m_fShadowEdgeSoftness > 0.0)
					{
						YRealDimension softEffectDimension( SoftEffectDimension( size ) );
						RRealSize softEffectSize( softEffectDimension, softEffectDimension );
						RRealVectorRect softEffectRect( softEffectSize );
						softEffectRect *= shadowTransform;
						uLONG blurRadius = ::Round( softEffectRect.Width() * shadowEdgeScale );
						blurRadius = ::Max( (uLONG)1, blurRadius );
						RImageLibrary().Blur( *maskBitmap, *maskBitmap, blurRadius, blurInAndOut, boxFilter );
					}

#if 0 // debug: set to 1 to display maskOffscreenDS to destinationOffscreenDS
					destinationOffscreenDS->BlitDrawingSurface( *maskOffscreenDS, bitmapRect, bitmapRect, kBlitSourceCopy );
#else
					// blend from user-selected shadow color + black
					// into destinationBitmap (belonging to destinationOffscreenDS)
					// thru maskBitmap
					YPercentage blackMixPercentage( shadowOpacityBlackPercentage / shadowOpacityColorPercentage );
					RSolidColor shadowColor( m_pShadowSettings->m_fShadowColor );
					shadowColor.DarkenBy( blackMixPercentage );
					RImageLibrary().AlphaBlend( shadowColor, *destinationBitmap, bitmapRect, *maskBitmap, bitmapRect );

					// if there's a behind color or frame, render the behind color
					// and frame again in front of the shadow (they were already
					// rendered, but behind the shadow, from DoRender()
					if (HasBackgroundColor())
						DoRenderBackground( *destinationOffscreenDS, docTransform );
					if (GetFrameType() != kNoFrame)
						RenderFrame( *destinationOffscreenDS, docTransform, m_rFrameColor );
#endif // 0
				}


#if 1 // debug: set to 0 to see ONLY the shadow
				if (HasAlphaMask())
				{
#if 0
	clock_t start = clock();
#endif
					// Blend alphaComponentBitmap into destinationBitmap thru alphaMaskBitmap
					RImageLibrary().AlphaBlend( *alphaComponentBitmap, bitmapRect, *destinationBitmap, bitmapRect, *alphaMaskBitmap, bitmapRect );
#if 0
	clock_t finish = clock();
	TRACE("Alpha Mask Blend required: %f ticks\n", (double)finish - start);
#endif
				}
#endif // 1

				if (vignetteBlurRadius > 0)
				{
					// save behind color, set to none and restore later so it doesn't
					// interfere with rendering the vignette mask or document rendering
					RColor oldBackgroundColor = GetBackgroundColor();
					const_cast<RComponentView*>(this)->SetBackgroundColor( RColor() );

					// init maskBitmap to black, render the document
					// white (fully opaque) to maskOffscreenDS and blur inward
					::memset( RBitmapImage::GetImageData( maskBitmap->GetRawData()), 0x00, RBitmapImage::GetImageDataSize( maskBitmap->GetRawData() ) );
					RenderShadowInSingleLayer( *maskOffscreenDS, docTransform, bitmapRect, size, kWhite );
					RImageLibrary().Blur( *maskBitmap, *maskBitmap, vignetteBlurRadius, blurIn, boxFilter );

#if 0 // debug: set to 1 to display maskOffscreenDS to destinationOffscreenDS
					destinationOffscreenDS->BlitDrawingSurface( *maskOffscreenDS, bitmapRect, bitmapRect, kBlitSourceCopy );
#else
					// create 24-bit srcOffscreenDS,srcBitmap and init to white
					srcOffscreenDS = new ROffscreenDrawingSurface( drawingSurface.IsPrinting(), TRUE, &drawingSurface );
					srcBitmap = new RBitmapImage;
					srcBitmap->Initialize( bitmapSize.m_dx, bitmapSize.m_dy, 24 );
					srcOffscreenDS->SetImage( srcBitmap );
					::memset( RBitmapImage::GetImageData( srcBitmap->GetRawData()), 0xFF, RBitmapImage::GetImageDataSize( srcBitmap->GetRawData() ) );

					// render the document into srcBitmap
					GetRDocument()->Render( *srcOffscreenDS, docTransform, bitmapRect, size );

					// blend from srcBitmap
					// into destinationBitmap (belonging to destinationOffscreenDS)
					// thru maskBitmap
					RImageLibrary().AlphaBlend( *srcBitmap, bitmapRect, *destinationBitmap, bitmapRect, *maskBitmap, bitmapRect );
#endif // 0

					// restore behind color
					const_cast<RComponentView*>(this)->SetBackgroundColor( oldBackgroundColor );
				}
			}
			catch (...)
			{
				TpsAssertAlways( "an unknown error occurred - can't render soft effect." );
				revertToNoSoftEffects = TRUE;
			}

			// clean up
			if (maskOffscreenDS)
				maskOffscreenDS->ReleaseImage( );
			delete maskOffscreenDS;
			delete maskBitmap;
			if (srcOffscreenDS)
				srcOffscreenDS->ReleaseImage( );
			delete srcOffscreenDS;
			delete srcBitmap;
			if (alphaOffscreenDS)
				alphaOffscreenDS->ReleaseImage( );
			delete alphaOffscreenDS;
			delete alphaComponentBitmap;
			delete alphaMaskBitmap;
			delete alphaTransformedMask;
			delete alphaBogusMask;
			delete alphaClipMask;
		}

		if (HasShadow() && revertToNoSoftEffects)
		{
			// Save the current drawing surface state and expand clip area to contain the shadow
			RDrawingSurfaceState	curState( &drawingSurface );
			drawingSurface.SetClipRect( rcRender );

			// Draw the single layer shadow
			RenderShadowInSingleLayer( drawingSurface, shadowTransform, rcRender, size, m_pShadowSettings->m_fShadowColor );
		}
	}

#if 1 // debug: set to 0 to see ONLY the shadow/glow
	// If we're reverting to no soft effects or we haven't already rendered the
	// document above with an alpha mask or vignette, render the document now
	if ((!HasAlphaMask() && !HasVignette()) || revertToNoSoftEffects)
		GetRDocument()->Render( drawingSurface, docTransform, rcRender, size );
#endif // 1
}

#ifndef TPSDEBUG
// Turn global optimizations back on now that we are beyond the trouble point.
#pragma optimize( "g", on )
#endif

// ****************************************************************************
//
//  Function Name:	void RComponentView::RenderShadowInSingleLayer(
//								RDrawingSurface&			drawingSurface,
//								const R2dTransform&		shadowTransform,
//								const RIntRect&			rcRender,
//								const RRealSize&			size,
//								const RSolidColor&		shadowColor) const
//
//  Description:		Renders a shadow for this view into a single layer.
//
//							If this is a normal (non-grouped) component, or there is
//							a behind color or frame, the shadow is rendered normally.
//
//							If the component has subcomponents (such as a grouped
//							component would have) all subcomponent shadows are
//							rendered into a single layer. If subcomponents have their
//							own subcomponents, all subcomponent shadows are rendered
//							recursively.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::RenderShadowInSingleLayer(
	RDrawingSurface&		drawingSurface,
	const R2dTransform&	transform,
	const RIntRect&		rcRender,
	const RRealSize&		size,
	const RSolidColor&	shadowColor ) const
{
	// make sure we're not in coloring book mode for shadow rendering
	BOOLEAN oldColoringBookMode = drawingSurface.GetColoringBookMode();
	drawingSurface.SetColoringBookMode( FALSE );

	YComponentViewIterator iterator = m_ComponentViewCollection.Start( );
	if (iterator == m_ComponentViewCollection.End() || HasBackgroundColor() ||
		GetFrameType() != kNoFrame )
	{
		// normal component or behind color or frame: render shadow
		GetRDocument()->RenderShadow( drawingSurface, transform, rcRender, size, shadowColor );
	}
	else
	{
		// grouped component: render each subcomponent shadow
		for (; iterator != m_ComponentViewCollection.End(); ++iterator)
		{
			// combine subcomponent transform with shadow transform
			R2dTransform tempTransform( transform );
			(*iterator)->ApplyTransform( tempTransform, FALSE, drawingSurface.IsPrinting() );

			// use subcomponent untransformed size
			RRealSize componentSize( (*iterator)->GetBoundingRect().GetUntransformedSize() );

			(*iterator)->RenderShadowInSingleLayer( drawingSurface, tempTransform, rcRender, componentSize, shadowColor );
		}
	}

	drawingSurface.SetColoringBookMode( oldColoringBookMode );
}

// ****************************************************************************
//
//  Function Name:	RComponentView::WantsGlowAndShadowInsideBounds() const
//
//  Description:		Determines if the component view wants its glow and
//							shadow effects to render inside its bounds.
//
//  Returns:			TRUE if the component view wants its glow and shadow
//							effects to render inside its bounds, else FALSE.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::WantsGlowAndShadowInsideBounds() const
{
	// subclasses (such as headlines) should override if they want glow & shadow inside their bounds
	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	YRealDimension RComponentView::SoftEffectDimension(
//								const RRealSize& size ) const
//
//  Description:		Returns the YRealDimension that should be used for
//							determining the x and y offsets of a soft effect on this
//							component.
//
//							This implementation just returns the minimum dimension in
//							the size parameter which corresponds to the minimum
//							dimension of the component. Subclasses like text and
//							headlines should override to return an average line height
//							instead.
//
//  Exceptions:		None
//
// ****************************************************************************
//
YRealDimension RComponentView::SoftEffectDimension( const RRealSize& size ) const
{
	return ::Min( ::Abs( size.m_dx ), ::Abs( size.m_dy ) );
}

// ****************************************************************************
//
//  Function Name:	void RComponentView::AdjustRectForSoftEffect(
//								RRealRect&	rect,
//								RRealSize&	softEffectOffsetSize ) const
//
//  Description:		If this component has a soft shadow or glow and wants it
//							to render outside its bounding rect, rect is adjusted
//							outward enough to contain the soft effect, and
//							softEffectOffsetSize is unconditionally set to the amount
//							of offset adjustment, if any, that was made to the
//							top,left of rect.
//
//							Useful for setting up an offscreen bitmap of the correct
//							size for components with soft effects that render outside
//							their bounds.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::AdjustRectForSoftEffect(
	RRealRect&	rect,
	RRealSize&	softEffectOffsetSize ) const
{
	// adjust for this component
	RRealVectorRect	vectorRect( rect );
	YRealDimension		leftOffset, topOffset, rightOffset, bottomOffset;

	GetRectOffsetsForSoftEffect( vectorRect, leftOffset, topOffset, rightOffset, bottomOffset );

	rect.AddPointToRect( RRealPoint( vectorRect.m_TopLeft.m_x - leftOffset, vectorRect.m_TopLeft.m_y - topOffset ) );
	rect.AddPointToRect( RRealPoint( vectorRect.m_BottomRight.m_x + rightOffset, vectorRect.m_BottomRight.m_y + bottomOffset ) );

	softEffectOffsetSize.m_dx = leftOffset;
	softEffectOffsetSize.m_dy = topOffset;
}

// ****************************************************************************
//
//  Function Name:	void RComponentView::AdjustRectForSoftEffectOrVignette(
//								RRealRect&	rect,
//								RRealSize&	softEffectOrVignetteOffsetSize ) const
//
//  Description:		If this component has a soft shadow or glow and wants it
//							to render outside its bounding rect, or even if the view
//							has a vignette (soft edge), rect is adjusted outward
//							enough to account for the soft effect, and
//							softEffectOrVignetteOffsetSize is unconditionally set to
//							the amount of offset adjustment, if any, that was made to
//							the top,left of rect.
//
//							Useful for setting up an offscreen bitmap of the correct
//							size for components with soft effects that render outside
//							their bounds.
//
//							Adjusting the offscreen bitmap rect outward to account for
//							a vignette (which is actually an inward-only effect) is
//							sometimes needed for rendering to the screen when only part
//							of the component needs to be updated.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::AdjustRectForSoftEffectOrVignette(
	RRealRect&	rect,
	RRealSize&	softEffectOrVignetteOffsetSize ) const
{
	RRealVectorRect	vectorRect( rect );
	YRealDimension		leftOffset, topOffset, rightOffset, bottomOffset;

	// adjust for shadow & glow...
	GetRectOffsetsForSoftEffect( vectorRect, leftOffset, topOffset, rightOffset, bottomOffset );

	// adjust for vignette (soft edge)...
	YRealDimension		vignetteLeftOffset( 0.0 ),	vignetteRightOffset( 0.0 );
	YRealDimension		vignetteTopOffset( 0.0 ),	vignetteBottomOffset( 0.0 );

	if (HasVignette() && m_pVignetteSettings->m_fVignetteEdgeSoftness > 0.0)
	{
		RRealSize			size( vectorRect.WidthHeight() );

		YPercentage edgeScale( BilinearInterpolateValue(
			kMinVignetteEdgeSoftness,
			kMidPointVignetteEdgeSoftness,
			kMaxVignetteEdgeSoftness,
			m_pVignetteSettings->m_fVignetteEdgeSoftness ) );
		uLONG vignetteBlurRadius;
		// GCB 8/28/98 - this calculation isn't accurate for text and headlines
		// because their soft effects size/offset is based on an average line
		// height rather than min component dimension but we don't have a transform
		// available here -- at worst, the rect just ends up getting adjusted a
		// little bigger than it needs to be
		vignetteBlurRadius = ::Round( RComponentView::SoftEffectDimension( size ) * edgeScale );
		vignetteBlurRadius = ::Max( (uLONG)1, vignetteBlurRadius );
		uLONG vignetteBlurDiameter = vignetteBlurRadius * 2;

		vignetteLeftOffset = vignetteBlurDiameter;
		vignetteTopOffset = vignetteBlurDiameter;
		vignetteRightOffset = vignetteBlurDiameter;
		vignetteBottomOffset = vignetteBlurDiameter;
	}

	// combine shadow & glow offsets with vignette offsets
	leftOffset =	::Max( leftOffset,	vignetteLeftOffset );
	rightOffset =	::Max( rightOffset,	vignetteRightOffset );
	topOffset =		::Max( topOffset,		vignetteTopOffset );
	bottomOffset =	::Max( bottomOffset,	vignetteBottomOffset );

	rect.AddPointToRect( RRealPoint( vectorRect.m_TopLeft.m_x - leftOffset, vectorRect.m_TopLeft.m_y - topOffset ) );
	rect.AddPointToRect( RRealPoint( vectorRect.m_BottomRight.m_x + rightOffset, vectorRect.m_BottomRight.m_y + bottomOffset ) );

	softEffectOrVignetteOffsetSize.m_dx = leftOffset;
	softEffectOrVignetteOffsetSize.m_dy = topOffset;
}

// ****************************************************************************
//
//  Function Name:	void RComponentView::AdjustVectorRectForSoftEffect(
//								RRealVectorRect& vectorRect ) const
//
//  Description:		If the view has a soft shadow or glow and wants it to
//							render outside its bounding rect, vectorRect is adjusted
//							outward enough to contain the soft effect. Useful for
//							maintaining an RRealVectorRect that represents the full
//							soft effects bounds of a component.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::AdjustVectorRectForSoftEffect( RRealVectorRect& vectorRect ) const
{
	// adjust for this component
	YRealDimension leftOffset, topOffset, rightOffset, bottomOffset;

	GetRectOffsetsForSoftEffect( vectorRect, leftOffset, topOffset, rightOffset, bottomOffset );

	YRealDimension width( vectorRect.Width() );
	YRealDimension height( vectorRect.Height() );
	if (width && height)
	{
		RRealSize scaleFactor(
			(width + leftOffset + rightOffset) / width,
			(height + topOffset + bottomOffset) / height );
		vectorRect.UnrotateAndScaleAboutPoint( vectorRect.GetCenterPoint(), scaleFactor );
	}
}

// ****************************************************************************
//
//  Function Name:	void RComponentView::GetRectOffsetsForSoftEffect(
//								const RRealVectorRect&	vectorRect,
//								YRealDimension&			leftOffset,
//								YRealDimension&			topOffset,
//								YRealDimension&			rightOffset,
//								YRealDimension&			bottomOffset ) const
//
//  Description:		If the view has a soft shadow or glow and wants it to
//							render outside its bounding rect, leftOffset, topOffset,
//							rightOffset and bottomOffset are unconditionally set to
//							the amount of offset adjustment, if any, that needs to be
//							made to vectorRect to contain the soft effect. Useful
//							for maintaining an RRealVectorRect that represents the full
//							soft effects bounds of a component.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::GetRectOffsetsForSoftEffect(
	const RRealVectorRect&	vectorRect,
	YRealDimension&			leftOffset,
	YRealDimension&			topOffset,
	YRealDimension&			rightOffset,
	YRealDimension&			bottomOffset ) const
{
	RRealSize			size( vectorRect.WidthHeight() );

	// GCB 8/28/98 - this calculation isn't accurate for text and headlines
	// because their soft effects size/offset is based on an average line height
	// rather than min component dimension but we don't have a transform available
	//	here -- at worst, the rect just ends up getting adjusted a little bigger
	// than it needs to be
	YRealDimension		softEffectDimension( RComponentView::SoftEffectDimension( size ) );

	YRealDimension		glowLeftOffset( 0.0 ),		glowRightOffset( 0.0 );
	YRealDimension		glowTopOffset( 0.0 ),		glowBottomOffset( 0.0 );
	YRealDimension		shadowLeftOffset( 0.0 ),	shadowRightOffset( 0.0 );
	YRealDimension		shadowTopOffset( 0.0 ),		shadowBottomOffset( 0.0 );

	if (HasGlow() && !WantsGlowAndShadowInsideBounds())
	{
		YPercentage glowScale( BilinearInterpolateValue(
			kMinGlowEdgeSoftness,
			kMidPointGlowEdgeSoftness,
			kMaxGlowEdgeSoftness,
			m_pGlowSettings->m_fGlowEdgeSoftness ) * 2.0 );

		glowLeftOffset = glowRightOffset = glowTopOffset = glowBottomOffset =
			softEffectDimension * glowScale;
	}

	if (HasShadow() && !WantsGlowAndShadowInsideBounds())
	{
		YRealDimension cosa( ::cos( m_pShadowSettings->m_fShadowAngle ) );
		YRealDimension sina( ::sin( m_pShadowSettings->m_fShadowAngle ) );
		YPercentage shadowScale( BilinearInterpolateValue(
			kMinShadowOffset,
			kMidPointShadowOffset,
			kMaxShadowOffset,
			m_pShadowSettings->m_fShadowOffset ) );

		YRealDimension hyp( softEffectDimension * shadowScale );
		YRealDimension shadowXOffset( cosa * hyp );
		YRealDimension shadowYOffset( sina * hyp );
		YRealDimension yShadowXScale( ::Abs( shadowXOffset / size.m_dx ) );
		YRealDimension yShadowYScale( ::Abs( shadowYOffset / size.m_dy ) );

		// symmetric left-right and top-bottom offsets not strictly necessary
		// if there's only a shadow but are easier to work with
		if (shadowXOffset < 0)
			shadowLeftOffset = shadowRightOffset = -shadowXOffset;
		else
			shadowRightOffset = shadowLeftOffset = shadowXOffset;
		if (shadowYOffset < 0)
			shadowTopOffset = shadowBottomOffset = -shadowYOffset;
		else
			shadowBottomOffset = shadowTopOffset = shadowYOffset;

		if (m_pShadowSettings->m_fShadowEdgeSoftness > 0.0)
		{
			YPercentage edgeScale( BilinearInterpolateValue(
				kMinShadowEdgeSoftness,
				kMidPointShadowEdgeSoftness,
				kMaxShadowEdgeSoftness,
				m_pShadowSettings->m_fShadowEdgeSoftness ) );

			// make sure shadow blur radius doesn't clip
			uLONG blurRadius( ::Round( softEffectDimension * edgeScale ) + 4 );

			shadowLeftOffset += blurRadius;
			shadowTopOffset += blurRadius;
			shadowRightOffset += blurRadius;
			shadowBottomOffset += blurRadius;
		}
	}

	YRealDimension maxLeft =	::Max( glowLeftOffset,		shadowLeftOffset );
	YRealDimension maxRight =	::Max( glowRightOffset,		shadowRightOffset );
	YRealDimension maxTop =		::Max( glowTopOffset,		shadowTopOffset );
	YRealDimension maxBottom =	::Max( glowBottomOffset,	shadowBottomOffset );

	YRealDimension maxOffset = maxLeft;
	maxOffset = ::Max( maxOffset, maxRight );
	maxOffset = ::Max( maxOffset, maxTop );
	maxOffset = ::Max( maxOffset, maxBottom );

	leftOffset = rightOffset =	topOffset = bottomOffset =	maxOffset;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::Offset( )
//
//  Description:		Offsets the view by the given amount
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::Offset( const RRealSize& offset, BOOLEAN fInvalidate )
	{
	// Invalidate the area we used to occupy
	if( fInvalidate )
		Invalidate( );

	// Offset
	m_BoundingRect.Offset( offset );
	m_FullSoftEffectsBoundingRect = m_BoundingRect;
	AdjustVectorRectForSoftEffect( m_FullSoftEffectsBoundingRect );

	// Invalidate the area we now occupy
	if( fInvalidate )
		Invalidate( );

	//	We must also Undefine any Outline computations
	UndefineOutlinePath( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::Rotate( )
//
//  Description:		Rotates the view about the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::Rotate( const RRealPoint& centerOfRotation, YAngle angle )
	{
	// Invalidate the area we used to occupy
	Invalidate( );

	// Rotate
	m_BoundingRect.RotateAboutPoint( centerOfRotation, angle );
	m_FullSoftEffectsBoundingRect = m_BoundingRect;
	AdjustVectorRectForSoftEffect( m_FullSoftEffectsBoundingRect );

	// Invalidate the area we now occupy
	Invalidate( );

	//	We must also Undefine any Outline computations
	UndefineOutlinePath( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::Rotate( )
//
//  Description:		Rotates the view about the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RComponentView::RotateAboutCenter( YAngle angle )
	{
	Rotate(GetBoundingRect().GetCenterPoint(), angle);
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::Resize( )
//
//  Description:		Resizes this view to the specified size
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::Resize( const RRealSize& newSize )
	{
	// Get the current size
	RRealSize currentSize = m_BoundingRect.WidthHeight( );

	// Figure out a scale factor
	RRealSize scaleFactor( newSize.m_dx / currentSize.m_dx, newSize.m_dy / currentSize.m_dy );

	// Scale about the center point
	Scale( m_BoundingRect.GetCenterPoint( ), scaleFactor, FALSE );

	//	We must also Undefine any Outline computations
	UndefineOutlinePath( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::Scale( )
//
//  Description:		Scales the view about the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::Scale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio )
	{
	// Scale
	if( GetParentView( ) )
		m_BoundingRect.UnrotateAndScaleAboutPoint( centerOfScaling, ApplyResizeConstraint( scaleFactor ), GetMinimumSize( ), GetMaximumSize( ), fMaintainAspectRatio );
	else
		m_BoundingRect.UnrotateAndScaleAboutPoint( centerOfScaling, ApplyResizeConstraint( scaleFactor ) );

	m_FullSoftEffectsBoundingRect = m_BoundingRect;
	AdjustVectorRectForSoftEffect( m_FullSoftEffectsBoundingRect );

	//	We must also Undefine any Outline computations
	UndefineOutlinePath( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::AlternateScale( )
//
//  Description:		Scales the view about the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::AlternateScale( const RRealPoint& centerOfScaling, const RRealSize& scaleFactor, BOOLEAN fMaintainAspectRatio )
	{
	//	Just call the original scale method
	Scale( centerOfScaling, scaleFactor, fMaintainAspectRatio );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::Invalidate( )
//
//  Description:		Invalidates this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::Invalidate( BOOLEAN fErase )
	{
	// Invalidate everything
	if( GetParentView( ) )
		GetParentView( )->InvalidateVectorRect( m_FullSoftEffectsBoundingRect, fErase );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetSize( )
//
//  Description:		Accessor
//
//  Returns:			The transformed view size
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RComponentView::GetSize( ) const
	{
	return m_BoundingRect.WidthHeight( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetReferenceSize( )
//
//  Description:		Accessor
//
//  Returns:			The view size against which the view transform is applied.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RComponentView::GetReferenceSize( ) const
	{
	return GetInteriorBoundingRect( ).GetUntransformedSize( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetMinimumSize( )
//
//  Description:		Accessor
//
//  Returns:			The minimum size this view is allowed
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RComponentView::GetMinimumSize( ) const
	{
	RComponentDocument* pDoc = GetComponentDocument();
	
	//	Get the base min size.
	RRealSize rMinimumSize = pDoc->GetMinObjectSize( pDoc->GetComponentType() );
	
	//	Account for frame. Enlarge object in an aspect correct fashion.
	//	Get the inset.
	RFrame* pFrame = GetFrame( );
	RRealSize topLeftInset;
	RRealSize bottomRightInset;
	pFrame->GetInset( topLeftInset, bottomRightInset );
	
	//	Make the minimum size larger.
	YRealDimension rCorrectAspect = rMinimumSize.m_dy / rMinimumSize.m_dx;
	rMinimumSize.m_dx += topLeftInset.m_dx + bottomRightInset.m_dx;
	rMinimumSize.m_dy += topLeftInset.m_dy + bottomRightInset.m_dy;
	YRealDimension rNewAspect = rMinimumSize.m_dy / rMinimumSize.m_dx;
	
	//	Make the minimum size aspect correct.
	if (rNewAspect > rCorrectAspect)
		rMinimumSize.m_dy += rMinimumSize.m_dy * (rNewAspect - rCorrectAspect);
	else
		rMinimumSize.m_dx += rMinimumSize.m_dx * (rCorrectAspect - rNewAspect);

	return rMinimumSize;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetMaximumSize( )
//
//  Description:		Accessor
//
//  Returns:			The maximum size this view is allowed
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RComponentView::GetMaximumSize( ) const
	{
	RComponentDocument* pDoc = GetComponentDocument();
	
	//	Get the base max size.
	RRealSize rMaximumSize = pDoc->GetMaxObjectSize( pDoc->GetComponentType() );
	
	//	Account for frame. Enlarge object in an aspect correct fashion.
	//	Get the inset.
	RFrame* pFrame = GetFrame( );
	RRealSize topLeftInset;
	RRealSize bottomRightInset;
	pFrame->GetInset( topLeftInset, bottomRightInset );
	
	//	Make the maximum size smaller.
	YRealDimension rCorrectAspect = rMaximumSize.m_dy / rMaximumSize.m_dx;
	rMaximumSize.m_dx -= topLeftInset.m_dx + bottomRightInset.m_dx;
	rMaximumSize.m_dy -= topLeftInset.m_dy + bottomRightInset.m_dy;
	YRealDimension rNewAspect = rMaximumSize.m_dy / rMaximumSize.m_dx;
	
	//	Make the maximum size aspect correct.
	if (rNewAspect > rCorrectAspect)
		rMaximumSize.m_dy -= rMaximumSize.m_dy * (rNewAspect - rCorrectAspect);
	else
		rMaximumSize.m_dx -= rMaximumSize.m_dx * (rCorrectAspect - rNewAspect);

	return rMaximumSize;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::MaintainAspectRatioIsDefault( )
//
//  Description:		Returns TRUE if the default behavior when resizing is to
//							maintain the components aspect ratio. The opposite
//							behavior is obtained by holding down the control key.
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::MaintainAspectRatioIsDefault( ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::CanChangeDefaultAspect( )
//
//  Description:		Returns TRUE if the user is allowed to use the control key
//							to reverse the maintain aspect default
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::CanChangeDefaultAspect( ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::CanResizeHorizontal( )
//
//  Description:		Returns TRUE if the user is allowed to resize this component
//							horizontally from the scale dialog
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::CanResizeHorizontal( ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::CanResizeVertical( )
//
//  Description:		Returns TRUE if the user is allowed to resize this component
//							horizontally from the scale dialog
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::CanResizeVertical( ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetInterfaceId( )
//
//  Description:		Return TRUE if the view can supports an Interface type for
//							the given commandId.
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::GetInterfaceId( YCommandID commandId, YInterfaceId& interfaceId ) const
	{
	static	CommandIdToInterfaceId componentViewIterfaceMap[] =
		{
			{ COMMAND_MENU_EFFECTS_TINT_100, kTintSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_TINT_90,  kTintSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_TINT_80,  kTintSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_TINT_70,  kTintSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_TINT_60,  kTintSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_TINT_50,  kTintSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_TINT_40,  kTintSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_TINT_30,  kTintSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_TINT_20,  kTintSettingsInterfaceId },
			{ COMMAND_MENU_EFFECTS_TINT_10,  kTintSettingsInterfaceId }
		} ;

	for (int i = 0; i < sizeof(componentViewIterfaceMap)/sizeof(CommandIdToInterfaceId); ++i )
		{
		if ( commandId == componentViewIterfaceMap[i].commandId )
			{
			interfaceId = componentViewIterfaceMap[i].interfaceId;
			return static_cast<BOOLEAN>(interfaceId != kInvalidInterfaceId);
			}
		}

	//	Nothing found
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetInterface( )
//
//  Description:		Return The Interface pointer if the given component can support
//							the desired interface.
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RInterface* RComponentView::GetInterface( YInterfaceId interfaceId ) const
	{
	static	InterfaceToCreateFunction interfaceCreationMap[] =
		{
			{ kBackgroundColorSettingsInterfaceId, RBackgroundColorSettingsInterface::CreateInterface },
			{ kTintSettingsInterfaceId, RTintSettingsInterfaceImp::CreateInterface }
		} ;

	TpsAssert( interfaceId != kInvalidInterfaceId, "Invalid InterfaceId passed to GetInterface" )
	for (int i = 0; i < sizeof(interfaceCreationMap)/sizeof(InterfaceToCreateFunction); ++i )
		{
		if ( interfaceId == interfaceCreationMap[i].interfaceId )
			return interfaceCreationMap[i].creator( this );
		}

	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetComponentAttributes( )
//
//  Description:		Gets the component attributes struct for this component
//							from the document and returns it.
//
//  Returns:			See above.
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RComponentAttributes& RComponentView::GetComponentAttributes( ) const
	{
	RComponentDocument* pDocument = dynamic_cast<RComponentDocument*>( GetRDocument( ) );
	TpsAssert( pDocument, "Document is not a component document." );
	return pDocument->GetComponentAttributes( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::ActivateComponent( )
//
//  Description:		Activates the component to which this view is attached.
//
//  Returns:			TRUE if the component should remain active
//							FALSE if the component should be immedialty deactivated
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::ActivateComponent( )
	{
	TpsAssert( m_eActiveState == kInactive, "Activating an already activated component." );

	m_eActiveState = kActive;

	// Activate our document
	GetRDocument( )->OnXActivate( TRUE );

	RRealSize	 topLeftInset( 0, 0 );
	RRealSize	 bottomRightInset( 0, 0 );
	//	Setup the interior frame...
	RFrame* pFrame = GetFrame( );
	//		Get the TopLeft inserts.
	pFrame->GetInset( topLeftInset, bottomRightInset );

	//	Get the width and invalidate the bounds - kActiveTextFrameInsetAmount
	RRealVectorRect	invalRect( m_FullSoftEffectsBoundingRect.GetUntransformedSize( ) );
	RRealSize			invalSize( kActiveFrameWidth, kActiveFrameWidth );
	R2dTransform		transform;
	GetViewTransform( transform, TRUE );
	transform.Invert( );
	invalSize	*= transform;
	if ( invalSize.m_dx <= 0 )	invalSize.m_dx	= -invalSize.m_dx;
	if ( invalSize.m_dy <= 0 )	invalSize.m_dy	= -invalSize.m_dy;
	invalSize.m_dx	+= topLeftInset.m_dx + bottomRightInset.m_dx;
	invalSize.m_dy	+= topLeftInset.m_dy + bottomRightInset.m_dy;
	invalRect.Inflate( invalSize );
	InvalidateVectorRect( invalRect );
	}

// ****************************************************************************
//							 
//  Function Name:	RComponentView::DeactivateComponent( )
//
//  Description:		Deactivates the component to which this view is attached.
//
//  Returns:			TODO: What does this return?
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::DeactivateComponent( )
	{
	TpsAssert( m_eActiveState == kActive, "Deactivating an inactive component." );

	m_eActiveState = kInactive;

	// Deactivate our document
	GetRDocument( )->OnXActivate( FALSE );

	RRealSize	 topLeftInset( 0, 0 );
	RRealSize	 bottomRightInset( 0, 0 );
	//	Setup the interior frame...
	RFrame* pFrame = GetFrame( );
	//		Get the TopLeft inserts.
	pFrame->GetInset( topLeftInset, bottomRightInset );

	//	Get the width and invalidate the bounds - kActiveTextFrameInsetAmount
	RRealVectorRect	invalRect( m_FullSoftEffectsBoundingRect.GetUntransformedSize( ) );
	RRealSize			invalSize( kActiveFrameWidth, kActiveFrameWidth );
	R2dTransform		transform;
	GetViewTransform( transform, TRUE );
	transform.Invert( );
	invalSize	*= transform;
	if ( invalSize.m_dx <= 0 )	invalSize.m_dx	= -invalSize.m_dx;
	if ( invalSize.m_dy <= 0 )	invalSize.m_dy	= -invalSize.m_dy;
	invalSize.m_dx	+= topLeftInset.m_dx + bottomRightInset.m_dx;
	invalSize.m_dy	+= topLeftInset.m_dy + bottomRightInset.m_dy;
	invalRect.Inflate( invalSize );
	InvalidateVectorRect( invalRect );
	}


// ****************************************************************************
//
//  Function Name:	RComponentView::CreateScriptAction( )
//
//  Description:		Try to create an action from the script
//
//  Returns:			The pointer to the action or NULL
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAction* RComponentView::CreateScriptAction( const YActionId& actionId, RScript& script )
	{
	RAction*	pAction = NULL;
	//	If there is an active component, let it try to process the command.
	if( m_pActiveComponent )
		pAction = m_pActiveComponent->CreateScriptAction( actionId, script );

	if ( !pAction )
		{
		if ( actionId == RDeactivateComponentAction::m_ActionId )
			pAction	= new RDeactivateComponentAction( GetParentView( ), script );
		else
			pAction	= RView::CreateScriptAction( actionId, script );
		}

	return pAction;
	}

// ****************************************************************************
//							 
//  Function Name:	RComponentView::DrawSelectionFrame( )
//
//  Description:		Draws a selection frame about the component. Does not
//							include resize or rotate handles
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::DrawSelectionFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const
	{
	// Convert our bounding rect into a selection rect
	YComponentBoundingRect boundingRect = m_BoundingRect;
	RSingleSelection::SelectionRectFromObjectRect( boundingRect, this, TRUE );

	// Draw it
	drawingSurface.FrameVectorRect( boundingRect, transform );

#ifdef TPSDEBUG
	if ((HasShadow() || HasGlow()) && !WantsGlowAndShadowInsideBounds())
	{
		YComponentBoundingRect softEffectsBoundingRect = m_FullSoftEffectsBoundingRect;
		RSingleSelection::SelectionRectFromObjectRect( softEffectsBoundingRect, this, TRUE );
		YPenStyle oldPenStyle = drawingSurface.GetPenStyle();
		drawingSurface.SetPenStyle( kDashPen );
		drawingSurface.FrameVectorRect( softEffectsBoundingRect, transform );
		drawingSurface.SetPenStyle( oldPenStyle );
	}
#endif
	}

// ****************************************************************************
//							 
//  Function Name:	RComponentView::HitTestSelectionFrame( )
//
//  Description:		Hit tests the selection from drawn abovve
//
//  Returns:			TRUE if the frame was hit
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::HitTestSelectionFrame( const RRealPoint& point ) const
	{
	// Convert our bounding rect into a selection rect
	YComponentBoundingRect boundingRect = m_BoundingRect;
	RSingleSelection::SelectionRectFromObjectRect( boundingRect, this, TRUE );

	// Hit test it
	return boundingRect.PointInRect( point );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::RenderTrackingFeedback( )
//
//  Description:		Renders tracking feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::RenderTrackingFeedback( RDrawingSurface& drawingSurface,
															const R2dTransform& transform,
															const RView& rView ) const
	{
	// Convert our bounding rect into a selection rect
	YComponentBoundingRect boundingRect( m_BoundingRect.GetUntransformedSize( ) );
	boundingRect *= transform;
	RSingleSelection::SelectionRectFromDeviceObjectRect( boundingRect, TRUE );

	// Draw it
	drawingSurface.FrameVectorRect( boundingRect, R2dTransform( ) );
	RenderInternalTrackingFeedback( drawingSurface, transform, rView );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::RenderTrackingFeedback( )
//
//  Description:		Renders tracking feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::RenderInternalTrackingFeedback( RDrawingSurface&, const R2dTransform&, const RView& ) const
	{
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetTrackingFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rectangle within which tracking feedback
//							will be drawn.
//
//  Returns:			Bounding rect
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RComponentView::GetTrackingFeedbackBoundingRect( const R2dTransform& transform ) const
	{
	// Convert our bounding rect into a selection rect
	YComponentBoundingRect boundingRect( m_BoundingRect.GetUntransformedSize( ) );
	boundingRect *= transform;
	RSingleSelection::SelectionRectFromDeviceObjectRect( boundingRect, TRUE );

	return boundingRect.m_TransformedBoundingRect.Inflate( RRealSize( 1, 1 ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::ApplyTint( )
//
//  Description:		Apply a Tint to the view's data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::ApplyTint( YTint tint )
	{
	RComponentDocument*	pDocument	= GetComponentDocument( );
	RChangeTintAction*	pAction		= new RChangeTintAction( pDocument, tint );

	pDocument->SendAction( pAction );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetColor( )
//
//  Description:		Get the color of the view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::GetColor( RColor& color ) const
	{
//	TpsAssertAlways( "ComponentView called, This should have been handled above this" );
	//	Black is choosen because it is plain...
	color = RColor( kBlack );
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::ApplyColor( )
//
//  Description:		Apply a color to the view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::ApplyColor( const RColor& )
	{
	TpsAssertAlways( "ComponentView called, This should have been handled above this" );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::ApplyResizeConstraint( )
//
//  Description:		Constrains the scale factors as appropriate
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RComponentView::ApplyResizeConstraint( const RRealSize& scaleFactor ) const
	{
	return scaleFactor;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetUseableArea( )
//
//  Description:		Accessor
//
//  Returns:			The rectangle of this component that is useable after all
//							decarations such as frames have been rendered.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RComponentView::GetUseableArea( const BOOLEAN fInsetFrame /*= TRUE*/ ) const
	{
	// Get the bounding rect of this component as a starting point
	RRealRect useableArea( GetBoundingRect( ).GetUntransformedSize( ) );
	
	if ( fInsetFrame )
		{
		// Get the amount the frame is inset and inset the useable area
		RFrame* pFrame = GetFrame( );
		YComponentBoundingRect interiorBoundingRect( GetBoundingRect( ) );
		pFrame->GetInsetComponentRect( interiorBoundingRect );
		useableArea = RRealRect( interiorBoundingRect.GetUntransformedSize( ) );
		}

	// Make sure the useable area is valid
	TpsAssert( useableArea.Width( ) > 0 && useableArea.Height( ) > 0, "Useable rect is invalid" );

	return useableArea;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::IsFlippedHorizontal( )
//
//  Description:		Accessor
//
//  Returns:			m_fHorizontallyFlipped
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::IsFlippedHorizontal( ) const
	{
	return m_fHorizontallyFlipped;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::IsFlippedVertical( )
//
//  Description:		Accessor
//
//  Returns:			m_fVerticallyFlipped
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::IsFlippedVertical( ) const
	{
	return m_fVerticallyFlipped;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::ClearFlippedFlags( )
//
//  Description:		Clears the flipped flags for the view
//
//  Returns:			Nothing.
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::ClearFlippedFlags( )
	{
	m_fHorizontallyFlipped	= FALSE;
	m_fVerticallyFlipped		= FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::AddOutline( )
//
//  Description:		Add the outine of this component into the collection
//
//  Returns:			TRUE on success
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
void RComponentView::AddOutline( YAvoidancePathCollection& avoidanceCollection, const R2dTransform& transform, BOOLEAN fBlock )
	{
	TpsAssert( m_pOutlinePath, "No OutlinePath was ever defined for this object" );

	RPath*					pNewPath = NULL;
	try
		{
		//	If this component does NOT have a frame AND does NOT have a background color
		//		AND there are sub components, let them add themselves
		if ( (GetFrameType()==kNoFrame) && !HasBackgroundColor( ) && (m_ComponentViewCollection.Count()>0) )
			{
			R2dTransform	newTransform	= m_BoundingRect.GetTransform( ) * transform;
			ComputeAvoidancePaths( NULL, avoidanceCollection, newTransform, fBlock );
			}
		else
			{
			//	If I only need the block
			if ( fBlock )
				{
				RPath						blockPath;
				CalculateBorderPath( &blockPath );
				pNewPath		= new RPath( blockPath, transform );
				}
			else
				{
				//	If the path is not valid, try to compute it
				if ( !m_pOutlinePath->IsDefined( ) )
					CalculateOutlinePath( );
				//	If the path is valid, include it...
				if ( m_pOutlinePath->IsDefined( ) )
					pNewPath = new RPath( *m_pOutlinePath, transform );
				}
				if ( pNewPath )
					avoidanceCollection.InsertAtEnd( pNewPath );
			}
		//	There must be a collection count AND no frame
		}
	catch( ... )
		{
		delete pNewPath;
		throw;
		}
	}


// ****************************************************************************
//
//  Function Name:	RComponentView::CalculateBorderPath( )
//
//  Description:		Calculate the Border path for this object.
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentView::CalculateBorderPath( RPath* pPath )
	{
	EPathOperatorArray	opArray;
	RIntPointArray			ptArray;

	//	At this level all we can do is frame with a rectangle that encompases the bounds
	//	of the component view.
	opArray.InsertAtEnd( kMoveTo );
	opArray.InsertAtEnd( kLine );
	opArray.InsertAtEnd( (EPathOperator)4 );
	opArray.InsertAtEnd( kClose );
	opArray.InsertAtEnd( kEnd );

	ptArray.InsertAtEnd( m_BoundingRect.m_TopLeft );
	ptArray.InsertAtEnd( m_BoundingRect.m_TopRight );
	ptArray.InsertAtEnd( m_BoundingRect.m_BottomRight );
	ptArray.InsertAtEnd( m_BoundingRect.m_BottomLeft );
	ptArray.InsertAtEnd( m_BoundingRect.m_TopLeft );

	pPath->Undefine( );
	pPath->Define( opArray, ptArray );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::CalculateOutlinePath( )
//
//  Description:		Calculate the outline path for this object.
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentView::CalculateOutlinePath( )
	{
	TpsAssert( m_pOutlinePath, "CalculateOutlinePath called but path is null" );

	CalculateBorderPath( m_pOutlinePath );
	}


// ****************************************************************************
//
//  Function Name:	RComponentView::IntersectsRect( )
//
//  Description:		Return if this component intersects the given rectangle in
//							its parents space...
//
//  Returns:			TRUE if the rectangle intersects the component
//
//  Exceptions:		File
//
// ****************************************************************************
//
BOOLEAN RComponentView::IntersectsRect( const RRealRect& targetRect ) const
	{
	return targetRect.IsIntersecting( GetBoundingRect( ).m_TransformedBoundingRect );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::IntersectsFullSoftEffectsRect()
//
//  Description:		Tests whether the full soft effects bounding rect of this
//							component intersects the given rectangle in its parent's
//							space.
//
//  Returns:			TRUE if the rectangle intersects the full soft effects
//							bounding rect.
//
//  Exceptions:		File
//
// ****************************************************************************
//
BOOLEAN RComponentView::IntersectsFullSoftEffectsRect( const RRealRect& targetRect ) const
	{
	return targetRect.IsIntersecting( GetFullSoftEffectsBoundingRect().m_TransformedBoundingRect );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::UndefineOutlinePath( )
//
//  Description:		Undefine the outline path for this object.
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentView::UndefineOutlinePath( )
	{
	TpsAssert( m_pOutlinePath, "UndefineOutlinePath called but path is null" );
	m_pOutlinePath->Undefine( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::TileView( )
//
//  Description:		Sets the tile attributes
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RComponentView::TileView(	const RTileAttributes& rTileAttributes,
								const RTileAttributesExt *prTileAttributesExt )
{
	if (!m_pTileAttributes) m_pTileAttributes = new RTileAttributes;
	*m_pTileAttributes = rTileAttributes;
	if (!m_pTileAttributesExt)
		m_pTileAttributesExt = new RTileAttributesExt;
	if (prTileAttributesExt != NULL)
		*m_pTileAttributesExt = *prTileAttributesExt;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::UntileView( )
//
//  Description:		Resets the tile attributes
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
void RComponentView::UntileView( )
{
	delete m_pTileAttributes;
	m_pTileAttributes = NULL;
	delete m_pTileAttributesExt;
	m_pTileAttributesExt = NULL;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::IsViewTiled( )
//
//  Description:		Returns the tile state
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
BOOLEAN RComponentView::IsViewTiled( ) const
{
	if (m_pTileAttributes)
		return TRUE;
	else
		return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetTileAttributes( )
//
//  Description:		Returns the tile attributes
//
//  Returns:			
//
//  Exceptions:		
//
// ****************************************************************************
RTileAttributes RComponentView::GetTileAttributes( ) const
{
	TpsAssert(m_pTileAttributes, "Component view is not tiled.");
	return *m_pTileAttributes;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetTileAttributesExt( )
//
//  Description:	Provides the extended tile attributes
//
//  Returns:		The extended tile attributes class
//
//  Exceptions:		
//
// ****************************************************************************
RTileAttributesExt RComponentView::GetTileAttributesExt( ) const
{
	TpsAssert(m_pTileAttributesExt, "Component view is not tiled.");
	return *m_pTileAttributesExt;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::IsRenderActiveFrame( )
//
//  Description:		return if the active frame should be rendered
//
//  Returns:			m_fRenderActiveFrame
//
//  Exceptions:		None
//
// ****************************************************************************
BOOLEAN RComponentView::IsRenderActiveFrame( ) const
{	
	return m_fRenderActiveFrame;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetRenderActiveFrame( )
//
//  Description:		Sets the RenderActiveFrame member
//
//  Returns:			BOOLEAN
//
//  Exceptions:		None
//
// ****************************************************************************
void RComponentView::SetRenderActiveFrame( BOOLEAN fRender )
{	
	m_fRenderActiveFrame = fRender;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::Write( )
//
//  Description:		Writes this views data to the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentView::Write( RChunkStorage& storage ) const
	{
	// Call the base method
	RPaneView::Write( storage );

	//	Set the current component view chunk data version.
	storage.GetCurrentChunkCurrentDataVersion( ) = kCurrentComponentViewRequiredVersion;

	// Write the bounding rect
	storage << m_BoundingRect;

	//	write the frame type
	storage << static_cast< uLONG >( m_eFrameType );	

	// Write out some dummy bytes
	uLONG	ulFlags	= 0;
	ulFlags	|=	( m_fHorizontallyFlipped )? kComponentIsHorzFlipped : 0;
	ulFlags	|=	( m_fVerticallyFlipped )?   kComponentIsVertFlipped : 0;
	storage << ulFlags;

	uLONG uReserved = 0;
	storage << uReserved;

	//	Write the aspect compensation.
	storage << m_AspectCompensation;

	// Write the tile attributes
	BOOLEAN fIsViewTiled = IsViewTiled();
	storage << fIsViewTiled;
	if (fIsViewTiled)
		{
		storage << *m_pTileAttributes;
		}

	//	Write our frame color.
	storage << m_rFrameColor;

	// This needs to be at the end since Tiled attribute Write creates a sub-chunk.
	if (fIsViewTiled)
		{
		m_pTileAttributesExt->Write( storage );
		}
	}

// ****************************************************************************
//											  
//  Function Name:	RComponentDocument::Read( )
//
//  Description:		Reads from the specified storage
//
//  Returns:			Nothing
//
//  Exceptions:		File
//
// ****************************************************************************
//
void RComponentView::Read( RChunkStorage& storage )
	{
	// Call the base method
	RPaneView::Read( storage );

	// Make sure we are not reading an old data file
	if( storage.GetCurrentChunkCurrentDataVersion( ) < kCurrentComponentViewRequiredVersion )
		{
		TpsAssertAlways( "This build will only read files created by build 101 and later." );
		throw kDataFormatTooOld;
		}

	// Read the bounding rect and apply to m_BoundingRect and m_FullSoftEffectsBoundingRect
	// then adjust m_FullSoftEffectsBoundingRect outward if necessary
	YComponentBoundingRect	rect;
	storage >> rect;
	m_BoundingRect = rect;
	m_FullSoftEffectsBoundingRect = rect;
	AdjustVectorRectForSoftEffect( m_FullSoftEffectsBoundingRect );

	//	Read the frame type
	uLONG ulFrameType;
	storage >> ulFrameType;
		
	// Read in some dummy bytes
	uLONG	ulFlags;
	storage >> ulFlags;
	m_fHorizontallyFlipped	= (ulFlags & kComponentIsHorzFlipped)? TRUE : FALSE;
	m_fVerticallyFlipped		= (ulFlags & kComponentIsVertFlipped)? TRUE : FALSE;

	uLONG uReserved;
	storage >> uReserved;
	
	// Read the frame aspect compensation
	storage >> m_AspectCompensation;

	// Delete any previous tile attributes
	delete m_pTileAttributes;
	m_pTileAttributes = NULL;
	delete m_pTileAttributesExt;
	m_pTileAttributesExt = NULL;

	// Read the tile attributes
	BOOLEAN fIsViewTiled;
	storage >> fIsViewTiled;
	if (fIsViewTiled)
		{
		m_pTileAttributes = new RTileAttributes;
		storage >> *m_pTileAttributes;
		}

	// Read the frame color
	storage >> m_rFrameColor;

	SetFrame( static_cast< EFrameType > ( ulFrameType ) );

	//	Reset the bounding Rect to be the one read from the storage
	SetBoundingRect( rect );

	// This needs to be at the end since Tiled attribute Write creates a sub-chunk.
	if (fIsViewTiled)
		{
		m_pTileAttributesExt = new RTileAttributesExt;
		m_pTileAttributesExt->Read( storage );
		}
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::CopyViewAttributes( )
//
//  Description:		Copies view attributes from the specified view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::CopyViewAttributes( RComponentView* pSource )
	{
	SetFrame( pSource->m_eFrameType );
	m_fHorizontallyFlipped = pSource->m_fHorizontallyFlipped;
	m_fVerticallyFlipped = pSource->m_fVerticallyFlipped;
	m_rFrameColor = pSource->m_rFrameColor;
	m_rBackgroundColor = pSource->m_rBackgroundColor;
	SetShadowSettings( *(pSource->m_pShadowSettings) );
	SetGlowSettings( *(pSource->m_pGlowSettings) );
	SetVignetteSettings( *(pSource->m_pVignetteSettings) );
	SetEdgeOutlineSettings( *(pSource->m_pEdgeOutlineSettings) );
	m_AspectCompensation = pSource->m_AspectCompensation;

	// The tint isnt a view attribute, but it should have been, so copy it here
	GetComponentDocument( )->SetTint( pSource->GetComponentDocument( )->GetTint( ), kApplyToChildren );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::CopyBoundingRect( )
//
//  Description:		Copies the bounding rect of another component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::CopyBoundingRect( RComponentView* pSource )
	{
	SetBoundingRect( pSource->GetBoundingRect( ) );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::CopyTransform( )
//
//  Description:		Copies the transform of another component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::CopyTransform( RComponentView* pSource )
	{
	// Get the components size
	RRealSize size = GetBoundingRect( ).GetUntransformedSize( );

	// Make a component bounding rect out of that size
	YComponentBoundingRect newBoundingRect( size );

	// Multiply it by the source components transform
	newBoundingRect *= pSource->GetBoundingRect( ).GetTransform( );

	// This matches the top left corners; we want to match the centers. Offset
	RRealSize offset( pSource->GetBoundingRect( ).GetCenterPoint( ) - newBoundingRect.GetCenterPoint( ) );
	newBoundingRect.Offset( offset );

	// Set the new bounding rect
	SetBoundingRect( newBoundingRect );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::OnDragEnter( )
//
//  Description:		Called by the framework when the mouse first enters the
//							non-scrolling region of the drop target window.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YDropEffect RComponentView::OnXDragEnter( RDataTransferSource&, YModifierKey, const RRealPoint& )
	{
	return kDropEffectNone;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::OnDragOver( )
//
//  Description:		Called by the framework during a drag operation when the
//							mouse is moved over the drop target window.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YDropEffect RComponentView::OnXDragOver( RDataTransferSource&, YModifierKey, const RRealPoint& )
	{
	return kDropEffectNone;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::OnDragLeave( )
//
//  Description:		Called by the framework during a drag operation when the
//							mouse is moved out of the valid drop area for that window.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::OnXDragLeave( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::OnDrop( )
//
//  Description:		Called by the framework when the user releases a data
//							object over a valid drop target.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::OnXDrop( RDataTransferSource&, const RRealPoint&, YDropEffect )
	{
	TpsAssertAlways( "How did we get here?" );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::CanSingleClickActivate( )
//
//  Description:		Called by the framework to determine if this component
//							supports single-click activation at the specified mouse
//							point.
//
//  Returns:			TRUE if single-click activation should occur
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::CanSingleClickActivate( const RRealPoint& ) const
	{
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::FlipHorizontal( )
//
//  Description:		Flips this component about the specified y axis
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::FlipHorizontal( YRealCoordinate yAxis )
	{
	// Get the center point of the component
	RRealPoint centerPoint = GetBoundingRect( ).GetCenterPoint( );

	// Do a scale
	m_BoundingRect.ScaleAboutPoint( RRealPoint( yAxis, centerPoint.m_y ), RRealSize( -1, 1 ) );
	m_FullSoftEffectsBoundingRect = m_BoundingRect;
	AdjustVectorRectForSoftEffect( m_FullSoftEffectsBoundingRect );

	//	We must also Undefine any Outline computations
	UndefineOutlinePath( );
	//	Invert the flipped flag
	m_fHorizontallyFlipped	= !m_fHorizontallyFlipped;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::FlipVertical( )
//
//  Description:		Flips this component about the specified x axis
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::FlipVertical( YRealCoordinate xAxis )
	{
	// Get the center point of the component
	RRealPoint centerPoint = GetBoundingRect( ).GetCenterPoint( );

	// Do a scale
	m_BoundingRect.ScaleAboutPoint( RRealPoint( centerPoint.m_x, xAxis ), RRealSize( 1, -1 ) );
	m_FullSoftEffectsBoundingRect = m_BoundingRect;
	AdjustVectorRectForSoftEffect( m_FullSoftEffectsBoundingRect );

	//	We must also Undefine any Outline computations
	UndefineOutlinePath( );
	//	Invert the flipped flag
	m_fVerticallyFlipped	= !m_fVerticallyFlipped;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetContextMenu( )
//
//  Description:		Gets the context menu to use when this is the only view
//							selected
//
//  Returns:			Pointer to a context menu. The caller adoptes this pointer.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPopupMenu* RComponentView::GetContextMenu( ) const
	{
	return new RPopupMenu( MENU_CONTEXT_DEFAULT_COMPONENT );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::InvalidateRenderCache( )
//
//  Description:		Invalidates this components render cache
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::InvalidateRenderCache( )
	{
	if( m_pRenderCache )
		m_pRenderCache->Invalidate( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetRenderCacheThreshold( )
//
//  Description:		Sets the render threshold of the render cache
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::SetRenderCacheThreshold( YTickCount renderThreshold )
	{
	if( m_pRenderCache )
		m_pRenderCache->SetRenderThreshold( renderThreshold );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::EnableRenderCache( )
//
//  Description:		Enables/disables the render cache
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::EnableRenderCache( BOOLEAN fEnable )
	{
	if( m_pRenderCache )
		m_pRenderCache->Enable( fEnable );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::FitToParent( )
//
//  Description:		Resizes a component view so that the component view completely
//							occupies the area of the parent
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::FitToParent()
{
	TpsAssert(GetParentView(), "Component view has no parent view.");
	RRealSize rParentSize(GetParentView()->GetSize());
	RRealVectorRect rParentRect(rParentSize);
	rParentRect.RotateAboutCenter(-GetRotationAngle());

	//	If this pane has bleed, acount for it here
	if ( HasPaneBleed() )
		rParentRect.Inflate( m_BleedAmount );

	RRealSize rViewSize(rParentRect.m_TransformedBoundingRect.WidthHeight());
	Resize(rViewSize);

	// Center the view
	YComponentBoundingRect rBoundingRect = GetBoundingRect();
	RRealPoint rViewCenter = rBoundingRect.GetCenterPoint();
	Offset(RRealSize(rParentSize.m_dx / 2 - rViewCenter.m_x, rParentSize.m_dy / 2 - rViewCenter.m_y));
}

// ****************************************************************************
//
//  Function Name:	RComponentView::FitInsideParent( )
//
//  Description:		Resizes a component's view so that it fits (aspect-correct)
//							within the parent
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::FitInsideParent()
{
	TpsAssert(GetParentView(), "Component view has no parent view.");	
	RRealSize rViewSize = GetReferenceSize();
	RRealSize rParentSize = GetParentView()->GetSize();	
	rViewSize.FitInside(rParentSize);
	Resize(rViewSize);

	// Center the view
	RRealPoint rViewCenter = GetBoundingRect().GetCenterPoint();
	Offset(RRealSize(rParentSize.m_dx / 2 - rViewCenter.m_x, rParentSize.m_dy / 2 - rViewCenter.m_y));
}

// ****************************************************************************
//
//  Function Name:	RComponentView::FitFullSoftEffectsInsideParent( )
//
//  Description:		Resizes a component's view so that it and its full soft
//							effects fit (aspect-correct) within the parent
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::FitFullSoftEffectsInsideParent()
{
	TpsAssert(GetParentView(), "Component view has no parent view.");	
	RRealSize rViewSize = GetReferenceSize();
	RRealSize rParentSize = GetParentView()->GetSize();	

	// Scale down for any soft effects
	YComponentBoundingRect boundingRect( GetBoundingRect() );
	YComponentBoundingRect fullSoftEffectsBoundingRect( GetFullSoftEffectsBoundingRect() );
	RRealSize scaledParentSize( rParentSize );
	scaledParentSize.Scale( RRealSize(
		boundingRect.Width() / fullSoftEffectsBoundingRect.Width(),
		boundingRect.Height() / fullSoftEffectsBoundingRect.Height() ) );

	rViewSize.FitInside(scaledParentSize);
	Resize(rViewSize);

	// Center the view
	RRealPoint rViewCenter = GetBoundingRect().GetCenterPoint();
	Offset(RRealSize(rParentSize.m_dx / 2 - rViewCenter.m_x, rParentSize.m_dy / 2 - rViewCenter.m_y));
}

// ****************************************************************************
//
//  Function Name:	RComponentView::FitOutsideParent( )
//
//  Description:		Resizes a component's view so that the parent view fits
//							(aspect-correct) within it
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::FitOutsideParent()
{
	TpsAssert(GetParentView(), "Component view has no parent view.");
	RRealSize rViewSize = GetReferenceSize();
	RRealSize rParentSize = GetParentView()->GetSize();	
	rViewSize.FitOutside(rParentSize);
	Resize(rViewSize);

	// Center the view
	RRealPoint rViewCenter = GetBoundingRect().GetCenterPoint();
	Offset(RRealSize(rParentSize.m_dx / 2 - rViewCenter.m_x, rParentSize.m_dy / 2 - rViewCenter.m_y));
}

// ****************************************************************************
//
//  Function Name:	RComponentView::HasShadow( ) const
//
//  Description:		Returns TRUE if the view has a shadow
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::HasShadow( ) const
{
	return m_pShadowSettings->m_fShadowOn;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::HasShadowOnThisOrSubViews( ) const
//
//  Description:		Returns TRUE if the view, or any of its subviews, has a shadow
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::HasShadowOnThisOrSubViews( ) const
{
	if (HasShadow())
		return TRUE;

	YComponentViewIterator iterator = m_ComponentViewCollection.Start( );
	for (; iterator != m_ComponentViewCollection.End(); ++iterator)
	{
		if ((*iterator)->HasShadowOnThisOrSubViews())
			return TRUE;
	}

	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::HasAlphaMask( ) const
//
//  Description:		Returns TRUE if the view has a mask
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::HasAlphaMask( ) const
{
	return (m_pAlphaMask && (m_dwMaskID != 0));
}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetSpecialEdgeSettings( RSoftShadowSettings& fShadowSettings ) const
//
//  Description:		Retrieve the view's Special Edge settings
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::GetSpecialEdgeSettings( RSpecialEdgeSettings& fEdgeSettings ) const
{
	fEdgeSettings = RSpecialEdgeSettings( m_pAlphaMask, m_dwMaskID, m_kSpecialEdgeType );
}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetSpecialEdgeSettings( const RSoftShadowSettings& fShadowSettings )
//
//  Description:		Sets the view's Special Edge settings
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::SetSpecialEdgeSettings( const RSpecialEdgeSettings& fEdgeSettings )
{
	m_pAlphaMask		= fEdgeSettings.pAlphaMask;
	m_dwMaskID			= fEdgeSettings.dwMaskID;
	m_kSpecialEdgeType	= fEdgeSettings.kSpecialEdgeType;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetShadowSettings( RSoftShadowSettings& fShadowSettings ) const
//
//  Description:		Retrieve the view's shadow settings
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::GetShadowSettings( RSoftShadowSettings& fShadowSettings ) const
{
	fShadowSettings = *m_pShadowSettings;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetShadowSettings( const RSoftShadowSettings& fShadowSettings )
//
//  Description:		set the view's shadow settings
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::SetShadowSettings( const RSoftShadowSettings& fShadowSettings )
{
	if (*m_pShadowSettings != fShadowSettings)
	{
		*m_pShadowSettings = fShadowSettings;

		m_FullSoftEffectsBoundingRect = m_BoundingRect;
		AdjustVectorRectForSoftEffect( m_FullSoftEffectsBoundingRect );

		InvalidateRenderCache();
	}
}

// ****************************************************************************
//
//  Function Name:	RComponentView::HasGlow( ) const
//
//  Description:		Returns TRUE if the view has a glow
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::HasGlow( ) const
{
	return m_pGlowSettings->m_fGlowOn;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::HasGlowOnThisOrSubViews( ) const
//
//  Description:		Returns TRUE if the view, or any of its subviews, has a glow
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::HasGlowOnThisOrSubViews( ) const
{
	if (HasGlow())
		return TRUE;

	YComponentViewIterator iterator = m_ComponentViewCollection.Start( );
	for (; iterator != m_ComponentViewCollection.End(); ++iterator)
	{
		if ((*iterator)->HasGlowOnThisOrSubViews())
			return TRUE;
	}

	return FALSE;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetGlowSettings( RSoftGlowSettings& fGlowSettings ) const
//
//  Description:		retrieve the view's glow settings
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::GetGlowSettings( RSoftGlowSettings& fGlowSettings ) const
{
	fGlowSettings = *m_pGlowSettings;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetGlowSettings( const RSoftGlowSettings& fGlowSettings )
//
//  Description:		set the view's glow settings
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::SetGlowSettings( const RSoftGlowSettings& fGlowSettings )
{
	if (*m_pGlowSettings != fGlowSettings)
	{
		*m_pGlowSettings = fGlowSettings;

		m_FullSoftEffectsBoundingRect = m_BoundingRect;
		AdjustVectorRectForSoftEffect( m_FullSoftEffectsBoundingRect );

		InvalidateRenderCache();
	}
}

// ****************************************************************************
//
//  Function Name:	RComponentView::HasVignette( ) const
//
//  Description:		Returns TRUE if the view has a vignette
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::HasVignette( ) const
{
	return m_pVignetteSettings->m_fVignetteOn;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetVignetteSettings( RSoftVignetteSettings& fVignetteSettings ) const
//
//  Description:		retrieve the view's vignette settings
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::GetVignetteSettings( RSoftVignetteSettings& fVignetteSettings ) const
{
	fVignetteSettings = *m_pVignetteSettings;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetVignetteSettings( const RSoftVignetteSettings& fVignetteSettings )
//
//  Description:		set the view's vignette settings
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::SetVignetteSettings( const RSoftVignetteSettings& fVignetteSettings )
{
	if (*m_pVignetteSettings != fVignetteSettings)
	{
		// if the component view has nested views (such as a grouped component
		// would have) make the settings in the nested views too
		YComponentViewIterator iterator = m_ComponentViewCollection.Start();
		for (; iterator != m_ComponentViewCollection.End(); ++iterator)
			(*iterator)->SetVignetteSettings( fVignetteSettings );

		*m_pVignetteSettings = fVignetteSettings;

		InvalidateRenderCache();
	}
}

// ****************************************************************************
//
//  Function Name:	RComponentView::HasEdgeOutline( ) const
//
//  Description:		Returns TRUE if the view has a EdgeOutline
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::HasEdgeOutline( ) const
{
	return m_pEdgeOutlineSettings->m_bEdgeOutlineOn;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::GetEdgeOutlineSettings( RSoftEdgeOutlineSettings& edgeOutlineSettings ) const
//
//  Description:		retrieve the view's EdgeOutline settings
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::GetEdgeOutlineSettings( REdgeOutlineSettings& edgeOutlineSettings ) const
{
	edgeOutlineSettings = *m_pEdgeOutlineSettings;
}

// ****************************************************************************
//
//  Function Name:	RComponentView::SetEdgeOutlineSettings( const RSoftEdgeOutlineSettings& edgeOutlineSettings )
//
//  Description:		set the view's EdgeOutline settings
//
//  Returns:			None
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::SetEdgeOutlineSettings( const REdgeOutlineSettings& edgeOutlineSettings )
{
	if (*m_pEdgeOutlineSettings != edgeOutlineSettings)
	{
		// if the component view has nested views (such as a grouped component
		// would have) make the settings in the nested views too
		YComponentViewIterator iterator = m_ComponentViewCollection.Start();
		for (; iterator != m_ComponentViewCollection.End(); ++iterator)
			(*iterator)->SetEdgeOutlineSettings( edgeOutlineSettings );

		*m_pEdgeOutlineSettings = edgeOutlineSettings;

		InvalidateRenderCache();
	}
}

// ****************************************************************************
//
//  Function Name:	RComponentView::OnXDisplayChange( )
//
//  Description:		Called when the display resolution changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::OnXDisplayChange( )
	{
	InvalidateRenderCache( );
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::EnableRenderCacheByDefault( )
//
//  Description:		Accessor
//
//  Returns:			TRUE if the render cache is enabled by default for this
//							component
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::EnableRenderCacheByDefault( ) const
	{
	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RComponentView::HasSoftEffects( )
//
//  Description:		Accessor
//
//  Returns:			TRUE if this component has soft effects, else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentView::HasSoftEffects( ) const
{
	if (HasShadowOnThisOrSubViews() || HasGlowOnThisOrSubViews() || HasVignette() || HasAlphaMask())
		return TRUE;
	else
		return FALSE;
}

// ****************************************************************************
//
// Function Name:		RComponentRenderCache::RComponentRenderCache( )
//
// Description:		Constructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RComponentRenderCache::RComponentRenderCache( const RComponentView* pComponent )
	: m_pComponent( pComponent )
	{
	;
	}

// ****************************************************************************
//
// Function Name:		RComponentRenderCache::RenderData( )
//
// Description:		Renders this graphic
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RComponentRenderCache::RenderData( RDrawingSurface& drawingSurface,
													 const RRealSize&,
													 const R2dTransform& transform,
													 const RIntRect& rcRender ) const
	{
	// Call the component to render
	m_pComponent->DoRender( drawingSurface, transform, rcRender );
	}

// ****************************************************************************
//
// Function Name:		RTilingRenderCache::RTilingRenderCache( )
//
// Description:		Constructor
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
RTilingRenderCache::RTilingRenderCache( const RComponentView* pComponent )
	: m_pComponent( pComponent )
	{
	// Always generate the offscreen for tiling
	SetRenderThreshold( 0 );
	}

// ****************************************************************************
//
// Function Name:		RTilingRenderCache::RenderData( )
//
// Description:		Renders this graphic
//
// Returns:				Nothing
//
// Exceptions:			None
//
// ****************************************************************************
//
void RTilingRenderCache::RenderData( RDrawingSurface& drawingSurface,
												 const RRealSize& size,
												 const R2dTransform& transform,
												 const RIntRect& rcRender ) const
	{
	// Call the component to render
	m_pComponent->RenderDocumentAndSoftEffects( drawingSurface, transform, rcRender, size );
	}

// ****************************************************************************
//
// Function Name:		RTilingRenderCache::AllocateCacheBitmaps( )
//
// Description:		Allocates and initializes the bitmaps used for the cache.
//
// Returns:				A pointer to the cacheable bitmap allocated.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RBitmapImageBase * RTilingRenderCache::AllocateCacheBitmap( RDrawingSurface& referenceSurface, uLONG uWidthInPixels, uLONG uHeightInPixels )
{
	RScratchBitmapImage *pBitmap;

	pBitmap = new RScratchBitmapImage;
	if (pBitmap != NULL)
	{
		pBitmap->Initialize( referenceSurface, uWidthInPixels, uHeightInPixels );
//		SetCacheBitmap( pBitmap );
	}

	return pBitmap;
}

// ****************************************************************************
//
// Function Name:		RTilingRenderCache::AllocateCacheMasks( )
//
// Description:		Allocates and initializes the bitmaps used for the cache masks.
//
// Returns:				A pointer to the cacheable mask bitmap allocated.
//
// Exceptions:			Memory
//
// ****************************************************************************
//
RBitmapImageBase * RTilingRenderCache::AllocateCacheMask( uLONG uWidthInPixels, uLONG uHeightInPixels )
{
	RScratchBitmapImage *pMask;

	pMask = new RScratchBitmapImage;
	if (pMask != NULL)
	{
		pMask->Initialize( uWidthInPixels, uHeightInPixels, 1 );
//		SetCacheMask( pMask );
	}

	return pMask;
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RComponentView::Validate( )
//
//  Description:		Validate the object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentView::Validate( ) const
	{
	RPaneView::Validate( );
	TpsAssertIsObject( RComponentView, this );
	}

#endif	//	TPSDEBUG
