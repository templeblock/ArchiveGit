// ****************************************************************************
//
//  File Name:			SelectionRectTracker.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RSelectionRectTracker class
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
//  $Logfile:: /PM8/Framework/Source/SelectionRectTracker.cpp                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "SelectionRectTracker.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ViewDrawingSurface.h"
#include "StandaloneView.h"

// ****************************************************************************
//
//  Function Name:	RSelectionRectTracker::RSelectionRectTracker( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionRectTracker::RSelectionRectTracker( RView* pView ) : RMouseTracker( pView )
	{
	m_pFeedbackRenderer = new RSelectionRectTrackingFeedbackRenderer( this, pView );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionRectTracker::~RSelectionRectTracker( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionRectTracker::~RSelectionRectTracker( )
	{
	delete m_pFeedbackRenderer;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionRectTracker::BeginTracking( )
//
//  Description:		Called to begin tracking; ie when the mouse button goes
//							down
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionRectTracker::BeginTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Call the base method first
	RMouseTracker::BeginTracking( mousePoint, modifierKeys );

	// Save the selection anchor point
	m_SelectionAnchorPoint = m_LastPoint = mousePoint;

	// Save the modifier keys
	m_MouseDownModifierKeys = modifierKeys;

	// Render
	m_pFeedbackRenderer->BeginTracking( mousePoint );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionRectTracker::ContinueTracking( )
//
//  Description:		Called to continue tracking; ie when the mouse moves
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionRectTracker::ContinueTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Call the base method first
	RMouseTracker::ContinueTracking( mousePoint, modifierKeys );

	// Draw feedback
	m_pFeedbackRenderer->ContinueTracking( mousePoint );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionRectTracker::EndTracking( )
//
//  Description:		Called to end tracking; ie when the mouse button goes up
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionRectTracker::EndTracking( const RRealPoint& mousePoint, YModifierKey modifierKeys )
	{
	// Call the base method first
	RMouseTracker::EndTracking( mousePoint, modifierKeys );

	// Render feedback
	m_pFeedbackRenderer->EndTracking( );

	// Get the selection rect
	RRealRect selectionRect = GetSelectionRect( mousePoint );

	// If the modifier key was down, toggle select the rectangle, otherwise just select it
	if( m_MouseDownModifierKeys & kToggleSelectionModifierKey )
		m_pView->ToggleSelectRectangle( selectionRect );
	else
		m_pView->SelectRectangle( selectionRect );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionRectTracker::Render( )
//
//  Description:		Renders the tracker feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionRectTracker::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform ) const
	{
	m_pFeedbackRenderer->Render( drawingSurface, transform );
	}

// ****************************************************************************
//
//  Function Name:	RSelectionRectTracker::GetSelectionRect( )
//
//  Description:		Computes a selection rect based on the anchor point and
//							the specified point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RSelectionRectTracker::GetSelectionRect( const RRealPoint& mousePoint ) const
	{
	RRealRect selectionRect;

	if( mousePoint.m_x > m_SelectionAnchorPoint.m_x )
		{
		selectionRect.m_Left = m_SelectionAnchorPoint.m_x;
		selectionRect.m_Right = mousePoint.m_x;
		}
	else
		{
		selectionRect.m_Left = mousePoint.m_x;
		selectionRect.m_Right = m_SelectionAnchorPoint.m_x;
		}

	if( mousePoint.m_y > m_SelectionAnchorPoint.m_y )
		{
		selectionRect.m_Top = m_SelectionAnchorPoint.m_y;
		selectionRect.m_Bottom = mousePoint.m_y;
		}
	else
		{
		selectionRect.m_Top = mousePoint.m_y;
		selectionRect.m_Bottom = m_SelectionAnchorPoint.m_y;
		}

	return selectionRect;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionRectTrackingFeedbackRenderer::RSelectionRectTrackingFeedbackRenderer( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSelectionRectTrackingFeedbackRenderer::RSelectionRectTrackingFeedbackRenderer( RSelectionRectTracker* pSelectionRectTracker,
																										  RView* pView )
	: RBitmapTrackingFeedbackRenderer( pView ),
	  m_pSelectionRectTracker( pSelectionRectTracker )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionRectTrackingFeedbackRenderer::GetFeedbackBoundingRect( )
//
//  Description:		Gets the bounding rect necessary to contain the feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RSelectionRectTrackingFeedbackRenderer::GetFeedbackBoundingRect( const RRealPoint& point ) const
	{
	// Get the selection rect
	RRealRect selectionRect = m_pSelectionRectTracker->GetSelectionRect( point );

	// Convert to device units
	::LogicalUnitsToDeviceUnits( selectionRect, *m_pView );

	// Bump the right and bottom by a pixel
	selectionRect.m_Right++;
	selectionRect.m_Bottom++;

	// Convert back to logical units
	::DeviceUnitsToLogicalUnits( selectionRect, *m_pView );

	return selectionRect;
	}

// ****************************************************************************
//
//  Function Name:	RSelectionRectTrackingFeedbackRenderer::RenderFeedback( )
//
//  Description:		Renders the feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionRectTrackingFeedbackRenderer::RenderFeedback( RDrawingSurface& drawingSurface,
																				 const R2dTransform& transform,
																				 const RRealPoint& point ) const
	{
	// Setup the drawing surface
	drawingSurface.SetPenStyle( kDotPen );

	// Get the selection rect
	RRealRect selectionRect = m_pSelectionRectTracker->GetSelectionRect( point );

	// Render the feedback
	drawingSurface.FrameRectangle( selectionRect, transform );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RSelectionRectTracker::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSelectionRectTracker::Validate( ) const
	{
	RMouseTracker::Validate( );
	TpsAssertIsObject( RSelectionRectTracker, this );
	}
	
#endif	//	TPSDEBUG

