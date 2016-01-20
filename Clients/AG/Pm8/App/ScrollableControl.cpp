// ****************************************************************************
//
//  File Name:		ScrollableControl.cpp
//
//  Project:		Renaissance Application Component
//
//  Author:			John Fleischhauer
//
//  Description:	Definition of the RScrollableControlDocument class
//
//	Portability:	Win32
//
//	Developed by:	Broderbund Software
//					500 Redwood Blvd
//					Novato, CA 94948
//
//  Copyright (C) 1998 Broderbund Software. All Rights Reserved.
//
// ****************************************************************************

//#include "RenaissanceIncludes.h"
#include "stdafx.h"

ASSERTNAME

#include "ScrollableControl.h"
#include "ScrollBar.h"
#include "CompositeSelection.h"
#include "TrackingFeedbackRenderer.h"
#include "UndoManager.h"

// ****************************************************************************
//
//  Function Name: RScrollControlView::RScrollControlView( )
//
//  Description:	 Constructor
//
//  Returns:		 Nothing
//
// ****************************************************************************
//
RScrollControlView::RScrollControlView( CDialog* pDialog, int nControlId, RControlDocument* pDocument ) :
	RControlView( pDialog, nControlId, pDocument ),
	m_fIsDragging( FALSE ),
	m_ZoomLevel( 1.0 ),
	m_bMaxZoomLevel( FALSE ),
	m_bMinZoomLevel( FALSE )
{
//	m_ZoomLevelMap.InsertAtEnd( 0.125 );
//	m_ZoomLevelMap.InsertAtEnd( 0.16 );
//	m_ZoomLevelMap.InsertAtEnd( 0.25 );
//	m_ZoomLevelMap.InsertAtEnd( 0.33 );
	m_ZoomLevelMap.InsertAtEnd( 0.5 );
	m_ZoomLevelMap.InsertAtEnd( 0.75 );

	m_ZoomLevelMap.InsertAtEnd( 1.0 );

	m_ZoomLevelMap.InsertAtEnd( 1.5 );
	m_ZoomLevelMap.InsertAtEnd( 2.0 );
	m_ZoomLevelMap.InsertAtEnd( 3.0 );
//	m_ZoomLevelMap.InsertAtEnd( 4.0 );
//	m_ZoomLevelMap.InsertAtEnd( 6.0 );
//	m_ZoomLevelMap.InsertAtEnd( 8.0 );

	EnableRenderCache( FALSE );
}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::ApplyTransform( )
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
void RScrollControlView::ApplyTransform( R2dTransform& transform, BOOLEAN fApplyParentsTransform, BOOLEAN fIsPrinting ) const
	{
	// Call the base method
	RControlView::ApplyTransform( transform, fApplyParentsTransform, fIsPrinting );

	// Center the panel in the view. First, get the viewable area of our parent
	RRealRect parentViewableArea = GetViewableArea( );
	transform.PreTranslate( parentViewableArea.Width() / 2.0, 
		parentViewableArea.Height() / 2.0 );

#if 0
	parentViewableArea.Offset( RRealSize( -parentViewableArea.m_Left, 
		-parentViewableArea.m_Top ) );

	// Get the size of the panel
	R2dTransform xform;
	xform.PostScale( m_ZoomLevel, m_ZoomLevel );
	RRealRect panelBoundingRect = parentViewableArea * xform;

	// Only do if we are not printing
	if( !fIsPrinting )
		{
		// Figure out how much to offset by to center the panel
		//	Start at the beginning of the printable area.
		RRealSize offset(0, 0);

//		if( ::AreFloatsLTE( panelBoundingRect.Width( ), parentViewableArea.Width( ) ) )
			offset.m_dx = -panelBoundingRect.m_Left + ( parentViewableArea.Width( ) - panelBoundingRect.Width( ) ) / 2;

//		if( ::AreFloatsLTE( panelBoundingRect.Height( ), parentViewableArea.Height( ) ) )
			offset.m_dy = -panelBoundingRect.m_Top + ( parentViewableArea.Height( ) - panelBoundingRect.Height( ) ) / 2;

		// Apply the offsets
		transform.PreTranslate( offset.m_dx, offset.m_dy );
		}
#endif
	}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::GetViewableArea( )
//
//  Description:		Accessor
//
//  Returns:			The rectangle into this view that is currently visible
//							due to scrolling.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RScrollControlView::GetViewableArea( ) const
	{
	RRealRect rViewRect = RWindowView::GetViewableArea( );
	rViewRect.Offset( RRealSize( -rViewRect.Width() / 2.0,
		-rViewRect.Height() / 2.0 ) );

	return rViewRect;
	}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::SetZoomLevel( )
//
//  Description:		Sets the zoom level of this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollControlView::SetZoomLevel( YZoomLevel zoomLevel, BOOLEAN fUpdateScrollBars )
	{

#if 1

	// Get the current view center point
	RRealRect viewableArea = GetViewableArea( );

	// Save the old view center point
	RRealPoint oldViewCenterPoint = viewableArea.GetCenterPoint( );

	BOOLEAN	fZoomingIn	= ::AreFloatsLT( GetZoomLevel(), zoomLevel );

	// Change the zoom level
	m_ZoomLevel = zoomLevel;

	// If we are updating the scroll bars, do it
	if( fUpdateScrollBars )
		UpdateScrollBars( kZoom );

	// Invalidate and update the view
	RWindowView::Invalidate( );
	XUpdateAllViews( kViewZoomChanged, 0 );

	// If we are updating the scroll bars, recenter the view
	if( fUpdateScrollBars && m_pHorizontalScrollBar )
		{
		//	Check if there is a scroll range in the scrollbars
		YIntDimension	horizPosition;
		YIntDimension	horizMinRange;
		YIntDimension	horizMaxRange;
		YIntDimension	vertPosition;
		YIntDimension	vertMinRange;
		YIntDimension	vertMaxRange;
		m_pHorizontalScrollBar->GetValues( horizMinRange, horizMaxRange, horizPosition );
		m_pVerticalScrollBar->GetValues( vertMinRange, vertMaxRange, vertPosition );

		//	Try to scroll IFF there is a range either in the horiz or vert direction
		if ( (horizMinRange != horizMaxRange) || (vertMinRange != vertMaxRange) )
			{
			RRealPoint newViewCenterPoint;
			RCompositeSelection*	pSelection	= GetSelection( );

			if (fZoomingIn && pSelection && !pSelection->IsEmpty())
				{
				RRealRect  tmpRect = pSelection->GetBoundingRect( );
				newViewCenterPoint = tmpRect.GetCenterPoint( );
				}
			else
				newViewCenterPoint = oldViewCenterPoint;

			// Scale to the screen DPI
			RRealSize deviceDPI = GetDPI( );

			R2dTransform transform;
			transform.PreScale( (YFloatType)deviceDPI.m_dx / kSystemDPI, (YFloatType)deviceDPI.m_dy / kSystemDPI );
			newViewCenterPoint *= transform;

			if ( horizMinRange != horizMaxRange )
				m_pHorizontalScrollBar->SetPosition( newViewCenterPoint.m_x );
			if ( vertMinRange != vertMaxRange )
				m_pVerticalScrollBar->SetPosition( newViewCenterPoint.m_y );
			}

		RWindowView::Invalidate( );
		}
#else

	// Change the zoom level
	m_ZoomLevel = zoomLevel;

	// If we are updating the scroll bars, do it
	if( fUpdateScrollBars )
		UpdateScrollBars( kZoom );

	// Invalidate and update the view
	RWindowView::Invalidate( );
	XUpdateAllViews( kViewZoomChanged, 0 );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::SubclassControl( CDialog* pdlg, int nctlid )
//
//  Description:		subclass the given control
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RScrollControlView::SubclassControl( CDialog* pDialog, int nControlId )
	{
	if (RControlView::SubclassControl( pDialog, nControlId ))
		{
		m_rUseableArea = GetViewableArea();
		return TRUE;
		}
	
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::GetScreenDPI( )
//
//  Description:		Accessor
//
//  Returns:			The DPI of the screen
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RScrollControlView::GetDPI( ) const
	{
	RRealSize dpi( GetScreenDPI() );

	return RRealSize( dpi.m_dx * m_ZoomLevel, dpi.m_dy * m_ZoomLevel );
	}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::GetZoomLevel( )
//
//  Description:		Accessor
//
//  Returns:			The current zoom level
//
//  Exceptions:		None
//
// ****************************************************************************
//
YZoomLevel RScrollControlView::GetZoomLevel( ) const
	{
	return m_ZoomLevel;
	}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::UpdateScrollBars( )
//
//  Description:		
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollControlView::UpdateScrollBars( EScrollUpdateReason scrollUpdateReason )
{
	static int nRecursionDepth = 0;
	TpsAssert( nRecursionDepth < 4, "Invalid recursion in scrolling." );
	nRecursionDepth++;

	RRealSize rViewSize( GetViewableArea().WidthHeight() );

	RRealRect rViewRect( rViewSize );
	RRealRect rComponentArea( CalcBoundingRectOfEmbeddedComponents( FALSE ) );

	RRealPoint pt( rViewRect.GetCenterPoint() );
	rComponentArea.Offset( RIntSize( pt.m_x, pt.m_y ) );

	::LogicalUnitsToDeviceUnits( rViewRect, *this );
	::LogicalUnitsToDeviceUnits( rComponentArea, *this );

	RRealSize  rViewArea = rViewRect.WidthHeight();

	// Calculate the union of the offset component bounding rect and the panel bounding rect
	RIntRect unionRect;
	unionRect.Union( rViewRect, rComponentArea );

	// Set the range of the scroll bars
	RIntSize scrollRange;

	// If the all components fit, remove the scroll bar
	if (rComponentArea.m_Left >= 0 && rComponentArea.m_Right <= rViewArea.m_dx )
		scrollRange.m_dx = scrollRange.m_dy = 0;
	else
		{
		scrollRange.m_dx = unionRect.m_Left; 
		scrollRange.m_dy = unionRect.m_Right;
		}

	// Update the scroll bars. Dont allow them to be removed unless we are at a recursion depth of 1, otherwise
	// we can get into an infinite recursion problem
	if( scrollUpdateReason != kScroll || ( scrollRange.m_dx == 0 && scrollRange.m_dy == 0 ) )
		{

		if( nRecursionDepth == 1 || scrollRange.m_dx != 0 || scrollRange.m_dy != 0 )
			{
			BOOLEAN fUpdateOnlyIfExtending = (BOOLEAN)( ( scrollUpdateReason == kChangeObjectPosition ) && !( scrollRange.m_dx == 0 && scrollRange.m_dy == 0 ) );

			// Update the scroll bars
			if (m_pHorizontalScrollBar)
				m_pHorizontalScrollBar->SetRange( scrollRange.m_dx, scrollRange.m_dy, rViewArea.m_dx, fUpdateOnlyIfExtending );
			}
		}

	// If the all components fit, remove the scroll bar
	if (rComponentArea.m_Top >= 0 && rComponentArea.m_Bottom <= rViewArea.m_dy )
		scrollRange.m_dx = scrollRange.m_dy = 0;
	else
		{
		scrollRange.m_dx = unionRect.m_Top;
		scrollRange.m_dy = unionRect.m_Bottom;
		}

	// Update the scroll bars. Dont allow them to be removed unless we are at a recursion depth of 1, otherwise
	// we can get into an infinite recursion problem
	if( scrollUpdateReason != kScroll || ( scrollRange.m_dx == 0 && scrollRange.m_dy == 0 ) )
		{
		
		if( nRecursionDepth == 1 || scrollRange.m_dx != 0 || scrollRange.m_dy != 0 )
			{
			BOOLEAN fUpdateOnlyIfExtending = (BOOLEAN)( ( scrollUpdateReason == kChangeObjectPosition ) && !( scrollRange.m_dx == 0 && scrollRange.m_dy == 0 ) );

			// Update the scroll bars
			if (m_pVerticalScrollBar)
				m_pVerticalScrollBar->SetRange( scrollRange.m_dx, scrollRange.m_dy, rViewArea.m_dy, fUpdateOnlyIfExtending );

			}
		}

	nRecursionDepth--;

	if (rViewSize != GetViewableArea().WidthHeight())
		UpdateScrollBars( scrollUpdateReason );
}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::ZoomIn( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollControlView::ZoomIn( )
{
	// Look through the zoom level map for the first zoom level greater than the
	// current zoom level
	RZoomLevelIterator iterator = m_ZoomLevelMap.Start( );
	for( ; iterator != m_ZoomLevelMap.End( ); ++iterator )
	{
		if( ( *iterator ) > GetZoomLevel( ) )
		{
			SetZoomLevel( ( *iterator ) );
			break;
		}
	}
	
	// set min / max flags
	m_bMinZoomLevel = FALSE;
	m_bMaxZoomLevel = ++iterator == m_ZoomLevelMap.End() || m_bMaxZoomLevel;
}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::ZoomOut( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollControlView::ZoomOut( )
{
	if (!m_ZoomLevelMap.Count( ))
		return;

	// Look through the zoom level map for the first zoom level less than the
	// current zoom level
	RZoomLevelIterator iterator = m_ZoomLevelMap.End( );

	do
	{
		if( ( *(--iterator) ) < GetZoomLevel( ) )
		{
			SetZoomLevel( ( *iterator ) );
			break;
		}

	} while (iterator != m_ZoomLevelMap.Start());

	// set min / max flags
	m_bMaxZoomLevel = FALSE;
	m_bMinZoomLevel = iterator == m_ZoomLevelMap.Start() || m_bMinZoomLevel;
}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::ConvertPointToLocalCoordinateSystem( )
//
//  Description:		Converts a point in our parents coordinate system to one
//							in our coordinate system
//
//  Returns:			The converted point
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealPoint RScrollControlView::ConvertPointToLocalCoordinateSystem( const RRealPoint& point ) const
	{
	R2dTransform transform;
	ApplyTransform( transform, FALSE, FALSE );
	
	if (m_fIsDragging /*m_pComponentCollection && m_pComponentCollection->Count( )*/)
	{
		// We are dragging so offset the local point
		// to account for our centering.
		YRealDimension xOffset, yOffset;
		transform.GetTranslation( xOffset, yOffset );

		xOffset = -xOffset / 2;
		yOffset = -yOffset / 2;

		transform.PostTranslate( xOffset, yOffset );
	}

	transform.Invert( );

	return point * transform;
	}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::OnXCharacter( )
//
//  Description:		Default handler for the Character message.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RScrollControlView::OnXCharacter( const RCharacter& character, YModifierKey modifierKeys )
{
	// If a component is active, route to that component
	if( m_pActiveComponent )
		{
		m_pActiveComponent->OnXCharacter( character, modifierKeys );
		return;
		}

	// Otherwise, we need to manipulate the selection on various keys
	switch( character )
		{
		case kTabCharacter :

			// Notify parent, or set focus to next control
 			break;

		case kReturnCharacter:
			// Notify parent
			break;

		case '+':
			ZoomIn();
			break;

		case '-':
			ZoomOut();
			break;

		default :
			RView::OnXCharacter( character, modifierKeys );
		}
}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::OnXLButtonDown( )
//
//  Description:		Default handler for the button down XEvent.
//
//							Check for any hit components.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollControlView::OnXLButtonDown( const RRealPoint& mousePoint, YModifierKey modifierKeys )
{
	CWnd& cWnd = GetCWnd();

	if (cWnd.GetFocus() != &cWnd)
		cWnd.SetFocus();

	RControlView::OnXLButtonDown( mousePoint, modifierKeys );

	if (GetSelection() && GetSelection()->Count( ))
		m_fIsDragging = TRUE;
}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::OnXLButtonUp( )
//
//  Description:		Default handler for the button up XEvent.
//
//							Will end any current mouse tracking.  If a component was hit
//							but never moved, it will activate it.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollControlView::OnXLButtonUp( const RRealPoint& mousePoint, YModifierKey modifierKeys )
{
	m_fIsDragging = FALSE;
	RControlView::OnXLButtonUp( mousePoint, modifierKeys );
}

#if 0
// ****************************************************************************
//
//  Function Name:	RScrollControlView::OnXDragEnter( )
//
//  Description:		Default handler for the drag enter XEvent.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YDropEffect RScrollControlView::OnXDragEnter( RDataTransferSource& dataSource, YModifierKey modifierKeys, const RRealPoint& point )
{
	m_fIsDragging = TRUE;

	return RControlView::OnXDragEnter( dataSource, modifierKeys, point );
}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::OnXDragLeave( )
//
//  Description:		Default handler for the drag leave XEvent.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollControlView::OnXDragLeave( )
{
	m_fIsDragging = FALSE;

	RControlView::OnXDragLeave();
}

// ****************************************************************************
//
//  Function Name:	RScrollControlView::OnXDrop( )
//
//  Description:		Default handler for the drag drop XEvent.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollControlView::OnXDrop( RDataTransferSource& dataSource, const RRealPoint& point, YDropEffect dropEffect )
{
	m_fIsDragging = FALSE;

	RControlView::OnXDrop( dataSource, point, dropEffect );
}
#endif

// ****************************************************************************
//
//  Function Name:	RScrollControlDocument::CreateView( )
//
//  Description:	Creates a new scrollable control view
//
//  Returns:		New view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RControlView* RScrollControlDocument::CreateView( CDialog* pDialog, int nControlId )
{
	return new RScrollControlView( pDialog, nControlId, this );
}

// ****************************************************************************
//
//  Function Name:	RScrollControlDocument::UndoCount()
//
//  Description:		Returns the current count of actions in the Undo Manager
//
//  Returns:			
//
// ****************************************************************************
//
YCounter RScrollControlDocument::UndoCount()
{
	return m_pUndoManager->CountUndoableActions();
}

