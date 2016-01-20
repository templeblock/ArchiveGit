// ****************************************************************************
//
//  File Name:			TrackingFeedbackRenderer.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RBitmapTrackingFeedbackRenderer class
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
//  $Logfile:: /PM8/Framework/Source/TrackingFeedbackRenderer.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "TrackingFeedbackRenderer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "OffscreenDrawingSurface.h"
#include "ComponentCollection.h"
#include "ViewDrawingSurface.h"
#include "WindowView.h"
#include "ScratchBitmapImage.h"
#include "ApplicationGlobals.h"
#include "StandaloneApplication.h"

const YTickCount kTimeThreshold = 60;
const int kMaxOffscreenSize = 1024 * 768;

// ****************************************************************************
//
//  Function Name:	RTrackingFeedbackRenderer::RTrackingFeedbackRenderer( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RTrackingFeedbackRenderer::~RTrackingFeedbackRenderer( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::RBitmapTrackingFeedbackRenderer( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBitmapTrackingFeedbackRenderer::RBitmapTrackingFeedbackRenderer( RView* pView )
	: m_pView( pView ),
	  m_pLastBitmap( NULL ),
	  m_pLastOffscreen( NULL ),
	  m_fTimeThresholdExceeded( FALSE ),
	  m_fUsedBitmapLast( FALSE )
	{
	ComputeRenderSettings( );
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::RBitmapTrackingFeedbackRenderer( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RBitmapTrackingFeedbackRenderer::~RBitmapTrackingFeedbackRenderer( )
	{
	DeleteLastBitmap( );
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::DeleteLastBitmap( )
//
//  Description:		Deletes the last bitmap and offscreen
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBitmapTrackingFeedbackRenderer::DeleteLastBitmap( )
	{
	// Release the image
	if( m_pLastOffscreen )
		m_pLastOffscreen->ReleaseImage( );

	// Delete the bitmap and offscreen
	delete m_pLastBitmap;
	m_pLastBitmap = NULL;

	delete m_pLastOffscreen;
	m_pLastOffscreen = NULL;
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::BeginTracking( )
//
//  Description:		Begins tracking
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBitmapTrackingFeedbackRenderer::BeginTracking( const RRealPoint& point )
	{
	// Render the initial feedback
	RenderInitialFeedback( point );
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::RenderInitialFeedback( )
//
//  Description:		Creates the initial bitmap and renders the initial feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBitmapTrackingFeedbackRenderer::RenderInitialFeedback( const RRealPoint& point )
	{
	// Get a drawing surface for the view
	RViewDrawingSurface drawingSurface( m_pView );

	// Get the view transform
	R2dTransform transform;
	m_pView->GetViewTransform( transform, drawingSurface, TRUE );

	// The the bounding rect of the new feedback
	m_LastLogicalRect = GetFeedbackBoundingRect( point );
	m_LastBoundingRect = m_LastLogicalRect * transform;
	m_LastPoint = point;

	// We can not use a bitmap if it is too big
	if( !::UseBitmapTrackingFeedback( ) || !::GetUseOffscreens( ) || m_LastBoundingRect.Width( ) * m_LastBoundingRect.Height( ) > kMaxOffscreenSize )
		{
		m_fUsedBitmapLast = FALSE;

		// Render direct to screen to draw the feedback
		ApplyDirectToScreenRenderSettings( drawingSurface );
		RenderFeedback( drawingSurface, transform, point );
		}

	// Otherwise, use the offscreen
	else
		{
		// Create a bitmap and offscreen for the new rect
		CreateBitmap( m_LastBoundingRect.WidthHeight( ), m_pLastBitmap, m_pLastOffscreen );

		// Draw in the new feedback
		R2dTransform newTransform = transform;
		newTransform.PostTranslate( -m_LastBoundingRect.m_Left, -m_LastBoundingRect.m_Top );
		ApplyBitmapRenderSettings( *m_pLastOffscreen );
		RenderFeedback( *m_pLastOffscreen, newTransform, point );

		// XOR the new offscreen onto the screen
		ApplyBitmapRenderSettings( drawingSurface );

		YTickCount startTime = ::RenaissanceGetTickCount( );
		drawingSurface.BlitDrawingSurface( *m_pLastOffscreen, m_LastBoundingRect.WidthHeight( ), m_LastBoundingRect, m_BitmapBlitMode );
		m_fTimeThresholdExceeded = ( ::RenaissanceGetTickCount( ) - startTime ) >= ( kTimeThreshold / 2 );

		m_fUsedBitmapLast = TRUE;
		}
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::ContinueTracking( )
//
//  Description:		Continues tracking
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBitmapTrackingFeedbackRenderer::ContinueTracking( const RRealPoint& point )
	{
	// Get a drawing surface for the view
	RViewDrawingSurface drawingSurface( m_pView );

	// Get the view transform
	R2dTransform transform;
	m_pView->GetViewTransform( transform, drawingSurface, TRUE );

	// Get the bounding rect of the new feedback in system coordinates
	m_LastLogicalRect = GetFeedbackBoundingRect( point );
	RIntRect newBoundingRect = m_LastLogicalRect * transform;

	// Union the old and new rects to get the final output rect
	RIntRect unionBoundingRect = RIntRect( ).Union( m_LastBoundingRect, newBoundingRect );

	// Determine if we can use a bitmap or not
	BOOLEAN fUseBitmap = TRUE;

	// Determine if we are autoscrolling
	BOOLEAN fWillAutoScroll = m_pView->WillAutoScroll( point );

	// We can not use a bitmap if we are autoscrolling
	if( fWillAutoScroll )
		fUseBitmap = FALSE;

	// Autoscroll anyway, to set up for next time
	else
		m_pView->AutoScroll( point );

	// If we are not autoscrolling, and the point is the same as last time, we can leave
	if( !fWillAutoScroll && ( point == m_LastPoint ) )
		return;

	// We can not use a bitmap if it is too big, or if we ever exceeded the time threshold
	if( !::UseBitmapTrackingFeedback( ) || !::GetUseOffscreens( ) || newBoundingRect.Width( ) * newBoundingRect.Height( ) > kMaxOffscreenSize || m_fTimeThresholdExceeded == TRUE )
		fUseBitmap = FALSE;

	// We used a bitmap last time, and can use one this time
	if( fUseBitmap && m_fUsedBitmapLast )
		{
		YTickCount startTime = ::RenaissanceGetTickCount( );

		// Create a bitmap and offscreen for the new rect
		RIntSize newBitmapSize = newBoundingRect.WidthHeight( );
		RScratchBitmapImage* pNewBitmap;
		ROffscreenDrawingSurface* pNewOffscreen;
		CreateBitmap( newBitmapSize, pNewBitmap, pNewOffscreen );

		// Create a bitmap and offscreen for the union rect
		RIntSize unionBitmapSize = unionBoundingRect.WidthHeight( );
		RScratchBitmapImage* pUnionBitmap;
		ROffscreenDrawingSurface* pUnionOffscreen;
		CreateBitmap( unionBitmapSize, pUnionBitmap, pUnionOffscreen );

		// Draw in the new feedback
		R2dTransform newTransform = transform;
		newTransform.PostTranslate( -newBoundingRect.m_Left, -newBoundingRect.m_Top );
		ApplyBitmapRenderSettings( *pNewOffscreen );
		RenderFeedback( *pNewOffscreen, newTransform, point );

		// Compute the offset necessary to position the feedback bitmaps correctly in the union bitmap
		RIntSize offset( -unionBoundingRect.m_Left, -unionBoundingRect.m_Top );

		// Copy the old feedback into the union bitmap
		pUnionOffscreen->BlitDrawingSurface( *m_pLastOffscreen, m_LastBoundingRect.WidthHeight( ), m_LastBoundingRect.Offset( offset ), kBlitSourceCopy );

		// Copy the new rectangle; we are about to modify it and we need it next time
		m_LastBoundingRect = newBoundingRect;

		// Put the new feedback into the union bitmap
		pUnionOffscreen->BlitDrawingSurface( *pNewOffscreen, newBitmapSize, newBoundingRect.Offset( offset ), m_BitmapBlitMode );
		
		// Put the union offscreen onto the screen
		ApplyBitmapRenderSettings( drawingSurface );
		
		drawingSurface.BlitDrawingSurface( *pUnionOffscreen, unionBitmapSize, unionBoundingRect, m_BitmapBlitMode );
		m_fTimeThresholdExceeded = m_fTimeThresholdExceeded || ( ( ::RenaissanceGetTickCount( ) - startTime ) >= kTimeThreshold );

		// Release and delete the last bitmap and offscreen
		DeleteLastBitmap( );

		// This iterations new feedback in the next iterations old feedback
		m_pLastOffscreen = pNewOffscreen;
		m_pLastBitmap = pNewBitmap;

		// Release and delete the union bitmap and offscreen
		pUnionOffscreen->ReleaseImage( );
		delete pUnionOffscreen;
		delete pUnionBitmap;
		}

	// We used a bitmap last time, but can not use one this time
	else if( m_fUsedBitmapLast && !fUseBitmap )
		{
		// XOR the last offscreen onto the screen
		ApplyBitmapRenderSettings( drawingSurface );
		drawingSurface.BlitDrawingSurface( *m_pLastOffscreen, m_LastBoundingRect.WidthHeight( ), m_LastBoundingRect, m_BitmapBlitMode );

		// Delete the last bitmap
		DeleteLastBitmap( );

		// Autoscroll
		m_pView->AutoScroll( point );

		// Get the new view transform
		transform.MakeIdentity( );
		m_pView->GetViewTransform( transform, drawingSurface, TRUE );

		// Render direct to screen to draw the new feedback
		ApplyDirectToScreenRenderSettings( drawingSurface );
		RenderFeedback( drawingSurface, transform, point );
		}

	// We did not use a bitmap last time, but can use one this time
	else if( !m_fUsedBitmapLast && fUseBitmap )
		{
		// Render direct to screen to remove the last feedback
		ApplyDirectToScreenRenderSettings( drawingSurface );
		YTickCount startTime = ::RenaissanceGetTickCount( );
		RenderFeedback( drawingSurface, transform, m_LastPoint );
		m_fTimeThresholdExceeded = m_fTimeThresholdExceeded || ( ( ::RenaissanceGetTickCount( ) - startTime ) >= ( kTimeThreshold / 2 ) );

		// Draw the new feedback with a bitmap
		RenderInitialFeedback( point );
		}

	// We did not use a bitmap last time, and can not use one this 
	else
		{
		// Render direct to screen to remove the last feedback
		ApplyDirectToScreenRenderSettings( drawingSurface );
		RenderFeedback( drawingSurface, transform, m_LastPoint );

		// Autoscroll
		m_pView->AutoScroll( point );

		// Get the new view transform
		transform.MakeIdentity( );
		m_pView->GetViewTransform( transform, drawingSurface, TRUE );

		// Render direct to screen to draw the new feedback
		ApplyDirectToScreenRenderSettings( drawingSurface );
		RenderFeedback( drawingSurface, transform, point );
		}

	m_LastPoint = point;
	m_fUsedBitmapLast = fUseBitmap;
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::EndTracking( )
//
//  Description:		Ends tracking
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBitmapTrackingFeedbackRenderer::EndTracking( )
	{
	// Get a drawing surface for the view
	RViewDrawingSurface drawingSurface( m_pView );

	// Get the view transform
	R2dTransform transform;
	m_pView->GetViewTransform( transform, drawingSurface, TRUE );

	Render( drawingSurface, transform );
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::Render( )
//
//  Description:		Renders the current state
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBitmapTrackingFeedbackRenderer::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const
	{
	if( m_fUsedBitmapLast && m_pLastOffscreen )
		{
		RIntRect tempRect = m_LastLogicalRect * transform;
		ApplyBitmapRenderSettings( drawingSurface );
		drawingSurface.BlitDrawingSurface( *m_pLastOffscreen, tempRect.WidthHeight( ), tempRect, m_BitmapBlitMode );
		}

	else
		{
		ApplyDirectToScreenRenderSettings( drawingSurface );
		RenderFeedback( drawingSurface, transform, m_LastPoint );
		}
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::CreateBitmap( )
//
//  Description:		Creates and intializes a bitmap and offscreen
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBitmapTrackingFeedbackRenderer::CreateBitmap( const RIntSize& size,
								  						    RScratchBitmapImage*& pBitmap,
															 ROffscreenDrawingSurface*& pOffscreen )
	{
	// Create and initialize the bitmap
	pBitmap = new RScratchBitmapImage;
	pBitmap->Initialize( size.m_dx, size.m_dy, 1 );

	// Create an offscreen
	pOffscreen = new ROffscreenDrawingSurface;

	// Set the bitmap into the offscreen
	pOffscreen->SetImage( pBitmap );

	// Fill the bitmap
	pOffscreen->SetFillColor( m_BitmapFillColor );
	pOffscreen->FillRectangle( size );
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::ComputeRenderSettings( )
//
//  Description:		Computes the colors and draw modes necessary to achieve
//							the best feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBitmapTrackingFeedbackRenderer::ComputeRenderSettings( )
	{
	// Get the feedback render settings from the view
	m_pView->GetFeedbackSettings( m_FeedbackColor, m_FeedbackDrawMode );

	// Calculate the bitmap settings
	if( m_FeedbackDrawMode == kXOR )
		{
		m_BitmapFillColor = kBlack;
		m_BitmapBlitMode = kBlitSourceXORDest;
		}

	else
		{
		m_BitmapFillColor = kWhite;
		m_BitmapBlitMode = kBlitSourceXNORDest;
		}
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::ApplyBitmapRenderSettings( )
//
//  Description:		Applys the computed render settings to the specified
//							drawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBitmapTrackingFeedbackRenderer::ApplyBitmapRenderSettings( RDrawingSurface& drawingSurface ) const
	{
	drawingSurface.SetForegroundMode( m_FeedbackDrawMode );
	drawingSurface.SetPenWidth( 1 );

	RSolidColor color;

	if( m_BitmapBlitMode == kBlitSourceXORDest )
		{
		drawingSurface.SetBackgroundColor( m_FeedbackColor );
		color = kWhite;
		}

	else
		{
		drawingSurface.SetForegroundColor( m_FeedbackColor );
		color = kBlack;
		}

	drawingSurface.SetPenColor( color );
	drawingSurface.SetFillColor( color );
	}

// ****************************************************************************
//
//  Function Name:	RBitmapTrackingFeedbackRenderer::ApplyDirectToScreenRenderSettings( )
//
//  Description:		Applys the computed render settings to the specified
//							drawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RBitmapTrackingFeedbackRenderer::ApplyDirectToScreenRenderSettings( RDrawingSurface& drawingSurface ) const
	{
	drawingSurface.SetForegroundMode( m_FeedbackDrawMode );
	drawingSurface.SetPenColor( m_FeedbackColor );
	drawingSurface.SetFillColor( m_FeedbackColor );
	drawingSurface.SetPenWidth( 1 );
	}

// ****************************************************************************
//
//  Function Name:	RDragDropTrackingFeedbackRenderer::RDragDropTrackingFeedbackRenderer( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDragDropTrackingFeedbackRenderer::RDragDropTrackingFeedbackRenderer( RView* pView,
																							 RComponentCollection* pComponentCollection,
																							 RRealSize dragOffset )
	: RBitmapTrackingFeedbackRenderer( pView ),
	  m_pComponentCollection( pComponentCollection ),
	  m_DragOffset( dragOffset )
	{
	// Save away the zero based collection bounding rect; we will need it to
	// calculate feedback bounding rects
	m_CollectionBoundingRect = RRealRect( pComponentCollection->GetBoundingRect( ).WidthHeight( ) );
	}

// ****************************************************************************
//
//  Function Name:	RDragDropTrackingFeedbackRenderer::GetFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rect necessary to contain the feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RDragDropTrackingFeedbackRenderer::GetFeedbackBoundingRect( const RRealPoint& point ) const
	{
	// Get the view transform
	R2dTransform transform;
	m_pView->GetViewTransform( transform, TRUE );

	// Calculate the offset between the center of the collection, and the center of drawing
	RRealSize offset = ( point + m_DragOffset ) - m_pComponentCollection->GetBoundingRect( ).GetCenterPoint( );

	// Make a new transform and apply the offset
	R2dTransform tempTransform = transform;
	tempTransform.PreTranslate( offset.m_dx, offset.m_dy );

	// Get the feedback bounding rect
	RRealRect boundingRect = m_pComponentCollection->GetTrackingFeedbackBoundingRect( tempTransform );

	// Convert back to logical units
	transform.Invert( );
	return boundingRect * transform;
	}

// ****************************************************************************
//
//  Function Name:	RDragDropTrackingFeedbackRenderer::RenderFeedback( )
//
//  Description:		Renders the feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDragDropTrackingFeedbackRenderer::RenderFeedback( RDrawingSurface& drawingSurface,
																		  const R2dTransform& transform,
																		  const RRealPoint& point ) const
	{
	drawingSurface.SetPenStyle( kSolidPen );

	// Calculate the offset between the center of the collection, and the center of drawing
	RRealSize offset = ( point + m_DragOffset ) - m_pComponentCollection->GetBoundingRect( ).GetCenterPoint( );

	// Make a new transform and apply the offset
	R2dTransform tempTransform = transform;
	tempTransform.PreTranslate( offset.m_dx, offset.m_dy );

	// Draw the feedback
	m_pComponentCollection->DrawDragTrackingRect( drawingSurface, tempTransform, *m_pView );
	}
