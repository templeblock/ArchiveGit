// ****************************************************************************
//
//  File Name:			WindowView.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RWindowView class
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
//  $Logfile:: /PM8/Framework/Source/WindowView.cpp                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "WindowView.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"
#include "ScrollBar.h"
#include "ViewDrawingSurface.h"
#include "StandaloneDocument.h"
#include "ComponentView.h"
#include "AutoDrawingSurface.h"
#include "DataTransfer.h"
#include "Timer.h"

// Auto scroll constants
const YTickCount kDragScrollDelay = ::GetProfileInt( _T( "windows" ), _T( "DragScrollDelay" ), DD_DEFSCROLLDELAY );
const YTickCount kDragScrollInterval = ::GetProfileInt( _T( "windows" ), _T( "DragScrollInterval" ), DD_DEFSCROLLINTERVAL );
const YIntDimension kDragScrollInset = ::GetProfileInt( _T( "windows" ), _T( "DragScrollInset" ), DD_DEFSCROLLINSET );

// ****************************************************************************
//
//  Function Name:	RWindowView::RWindowView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RWindowView::RWindowView( )
	: m_pHorizontalScrollBar( NULL ),
	  m_pVerticalScrollBar( NULL ),
	  m_AutoScrollDirection( kNone ),
	  m_pAutoScrollTimer( NULL ),
	  m_ScreenDPI( 1, 1 )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::~RWindowView( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RWindowView::~RWindowView( )
	{
	// Delete the scroll bars
	delete m_pHorizontalScrollBar;
	delete m_pVerticalScrollBar;

	// delete the autoscroll timer
	delete m_pAutoScrollTimer;
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::Initialize
//
//  Description:		Initializer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::Initialize( RDocument* pDocument )
	{
	// Get the screen DPI and save it away
	RViewDrawingSurface drawingSurface( this );
	m_ScreenDPI = drawingSurface.GetDPI( );

	// Create the scroll bars
	m_pHorizontalScrollBar = new RScrollBar( kHorizontal, this );
	m_pVerticalScrollBar = new RScrollBar( kVertical, this );

	RView::Initialize( pDocument );
	}		

// ****************************************************************************
//
//  Function Name:	RWindowView::RWindowView( )
//
//  Description:		Render this views background
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::RenderBackground( RDrawingSurface& drawingSurface, const R2dTransform&, const RIntRect& rcRender ) const
	{
	// Fill in the background
	if( !drawingSurface.IsPrinting( ) )
		{
		RColor	backgroundColor	= GetBackgroundColor( );
		if ( backgroundColor.GetFillMethod() == RColor::kSolid )
			{
			RSolidColor	solid	= backgroundColor.GetSolidColor( );
			//	Check for something close to TRUE GRAY and alter it slightly
#ifdef _WINDOWS
			const YColorComponent	kHitBits			= 0xC0;
			const YColorComponent	kNewColor		= 0x77;
			const YColorComponent	kGrayColor		= 0x80;
#elif	defined( MAC )
			const YColorComponent	kHitBits			= 0xF000;
			const YColorComponent	kNewColor		= 0x7777;
			const YColorComponent	kGrayColor		= 0x8000;
#endif	//	_WINDOWS or MAC
			YColorComponent	redColor		= (solid.GetRed( ) & kHitBits);
			YColorComponent	greenColor	= (solid.GetGreen( ) & kHitBits);
			YColorComponent	blueColor	= (solid.GetBlue( ) & kHitBits);
			if ( (kGrayColor == redColor) && (redColor == greenColor) && (greenColor == blueColor) )
				{
				backgroundColor = RSolidColor( kNewColor, kNewColor, kNewColor );
				}
			}
		drawingSurface.SetForegroundMode( kNormal );
		drawingSurface.SetFillColor( backgroundColor );
		drawingSurface.FillRectangle( rcRender );
		}
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::OnXDisplayChange( )
//
//  Description:		Called when the display resolution changes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::OnXDisplayChange( )
	{
	// Get the screen DPI and save it away
	RViewDrawingSurface drawingSurface( this );
	m_ScreenDPI = drawingSurface.GetDPI( );

	// Reset the autodrawing surface
	::GetAutoDrawingSurface( ).Reset( );

	// Update the scrollbars
	UpdateScrollBars( kResize );

	// Call the base method
	RView::OnXDisplayChange( );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::OnXResize( )
//
//  Description:		Called when the view is resized
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::OnXResize( const RRealSize& )
	{
	ResetInsertionPoint( );

	UpdateScrollBars( kResize );
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
void RWindowView::ApplyTransform( R2dTransform& transform, BOOLEAN, BOOLEAN fIsPrinting ) const
	{
	// If we are not printing, apply a translation to account for scrolling
	if( !fIsPrinting && m_pHorizontalScrollBar )
		{
		// Get the amount we are scrolled in device units
		RRealSize scrollAmount( -m_pHorizontalScrollBar->GetPosition( ), -m_pVerticalScrollBar->GetPosition( ) );

		// Convert it to logical units
		::DeviceUnitsToLogicalUnits( scrollAmount, *this );

		// Apply the translate
		transform.PreTranslate( scrollAmount.m_dx, scrollAmount.m_dy );
		}
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::OnXActivate( const BOOLEAN fActivating )
//
//  Description:		handles cross platform view activation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::OnXActivate( BOOLEAN fActivating )
	{
	// Call the base method
	RView::OnXActivate( fActivating );

	//	remember the active view in the app
	if ( fActivating )
		::GetActiveView( ) = this;
	else
		::GetActiveView( ) = NULL;

	// Tell our document we are activating
	if( GetRDocument( ) )
		GetRDocument( )->OnXActivate( fActivating );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::SetMouseCapture( )
//
//  Description:		Captures the mouse to this view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::SetMouseCapture( )
	{
	WinCode( GetCWnd( ).SetCapture( ) );
	RView::SetMouseCapture( );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::ReleaseMouseCapture( )
//
//  Description:		Releases the mouse capture. Unnecessary on the Mac
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::ReleaseMouseCapture( )
	{
	RView::ReleaseMouseCapture( );
	WinCode( ReleaseCapture( ) );
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
RRealSize RWindowView::GetDPI( ) const
	{
	return RRealSize( m_ScreenDPI.m_dx, m_ScreenDPI.m_dy );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::GetCursorPosition( )
//
//  Description:		Gets the current cursor position within this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealPoint RWindowView::GetCursorPosition( ) const
	{
	// Get the absolute cursor position
	POINT	pt;
	::GetCursorPos( &pt );

	// Convert to client coordinates
	GetCWnd( ).ScreenToClient( &pt );
	RRealPoint cursorPosition( pt.x, pt.y );

	// Convert to logical units
	::DeviceUnitsToLogicalUnits( cursorPosition, *this );

	// Get our transform
 	R2dTransform transform;
	ApplyTransform( transform, FALSE, FALSE );

	// Invert the transform and convert the point to our coordinate system
	transform.Invert( );
	return cursorPosition * transform;
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::Invalidate( )
//
//  Description:		Invalidates the entire view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::Invalidate( BOOLEAN fErase )
	{
	//
	//	Only invalidate if we have a CWnd
	if( GetCWnd() )
		GetCWnd( ).Invalidate( fErase );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::InvalidateVectorRect( )
//
//  Description:		Invalidates the given vector rect in this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::InvalidateVectorRect( const RRealVectorRect& rect, BOOLEAN fErase )
	{
	//
	//	Only invalidate if we have a CWnd
	if( !GetCWnd() )
		return;

		// Get the bounding rect...
	RRealRect temp = rect.m_TransformedBoundingRect;

	// Get our transformation...
	R2dTransform	transform;
	ApplyTransform( transform, FALSE, FALSE );

	// Apply it to our bounds...
	temp *= transform;

	// Convert the bounds to device units
	::LogicalUnitsToDeviceUnits( temp, *this );

	// guard for roundoff...
		const YRealDimension	kRoundOffGuard	= 2.0;
	temp.Inflate( RRealSize( kRoundOffGuard, kRoundOffGuard ) );

	// Put it in a CRect and do the invalidate
	CRect crect( ::Round(temp.m_Left), ::Round(temp.m_Top), ::Round(temp.m_Right), ::Round(temp.m_Bottom) );
	GetCWnd( ).InvalidateRect( crect, fErase );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::ValidateVectorRect( )
//
//  Description:		Validates the given vector rect in this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::ValidateVectorRect( const RRealVectorRect& rect )
	{
	//
	//	Only validate if we have a CWnd
	if( !GetCWnd() )
		return;
		// Get the bounding rect...
	RRealVectorRect temp = rect;

	// Get our transformation...
	R2dTransform	transform;
	ApplyTransform( transform, FALSE, FALSE );

	// Apply it to our bounds...
	temp *= transform;

	// Convert the bounds to device units
	::LogicalUnitsToDeviceUnits( temp, *this );

	// Get the polygon points
	RRealPoint	realPolygon[ 4 ];
	temp.GetPolygonPoints( realPolygon );

	// Create a polygon region
	RIntPoint	polygon[4];
	polygon[0]	= realPolygon[0];
	polygon[1]	= realPolygon[1];
	polygon[2]	= realPolygon[2];
	polygon[3]	= realPolygon[3];
	CRgn	validateArea;
	validateArea.CreatePolygonRgn( reinterpret_cast<POINT*>( &polygon ), 4, ALTERNATE );

	// Validate the region
	GetCWnd( ).ValidateRgn( &validateArea );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::GetSize( )
//
//  Description:		Accessor
//
//  Returns:			The view size
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RWindowView::GetSize( ) const
	{
	// Get the client rect
	CRect rcClient;
	GetCWnd( ).GetClientRect( &rcClient );

	// Make a size out of it
	RRealSize size( rcClient.Width( ), rcClient.Height( ) );

	// Convert it to logical units
	::DeviceUnitsToLogicalUnits( size, *this );

	return size;
	}							

// ****************************************************************************
//
//  Function Name:	RWindowView::SetKeyFocus( )
//
//  Description:		Set the Windows key focus to be this view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::SetKeyFocus( )
	{
	GetCWnd().SetFocus( );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::IsEventPending( )
//
//  Description:		Check if there are any pending events in the Queue.
//							This query can be made during idle processing to check
//							if control should return to the framework.
//
//  Returns:			TRUE if the are system events pending
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RWindowView::IsEventPending( ) const
	{
	MSG	msg;
	return (BOOLEAN)PeekMessage( &msg, GetCWnd().GetSafeHwnd(), 0, 0, PM_NOYIELD|PM_NOREMOVE );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::IsKeyEventPending( )
//
//  Description:		Check if there are any pending key events in the Queue.
//
//  Returns:			TRUE if the next system event is a key event
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RWindowView::IsKeyEventPending( ) const
	{
	MSG	msg;
	while (PeekMessage( &msg, GetCWnd().GetSafeHwnd(), 0, 0, PM_NOYIELD|PM_NOREMOVE ))
		{
		if (msg.message == WM_KEYUP)	//	we want to flush this message
			PeekMessage( &msg, GetCWnd().GetSafeHwnd(), WM_KEYUP, WM_KEYUP, PM_NOYIELD|PM_REMOVE );
		else if ((msg.message == WM_CHAR) || (msg.message == WM_KEYDOWN))
			return TRUE;
		else
			return FALSE;
		}
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::GetViewableArea( )
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
RRealRect RWindowView::GetViewableArea( ) const
	{
	// Get the client rect from MFC
	RECT clientRect;
	GetCWnd( ).GetClientRect( &clientRect );

	// Convert to a Renaissance rect in logical units
	RRealRect viewableRect( clientRect );
	::DeviceUnitsToLogicalUnits( viewableRect, *this );

	// Offset to take into account scrolling
	if( m_pHorizontalScrollBar && m_pVerticalScrollBar )
		{
		RRealSize offset( m_pHorizontalScrollBar->GetPosition( ), m_pVerticalScrollBar->GetPosition( ) );
		::DeviceUnitsToLogicalUnits( offset, *this );
		viewableRect.Offset( offset );
		}

	return viewableRect;
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::GetUseableArea( )
//
//  Description:		Accessor
//
//  Returns:			The rectangle of this view that is useable after all
//							decarations have been rendered.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealRect RWindowView::GetUseableArea( const BOOLEAN /*fInsetFrame = TRUE*/ ) const
	{
	// For now, just return the viewable area
	return GetViewableArea( );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::OnHScroll( )
//
//  Description:		Called to do horizontal scrolling
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::OnHScroll( UINT nSBCode, int nPos )
	{
	YIntDimension scrollAmount = OnScroll( m_pHorizontalScrollBar, nSBCode, nPos );
	ScrollWindow( RRealSize( scrollAmount, 0 ) );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::OnVScroll( )
//
//  Description:		Called to do vertical scrolling
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::OnVScroll( UINT nSBCode, int nPos ) 
	{
	YIntDimension scrollAmount = OnScroll( m_pVerticalScrollBar, nSBCode, nPos );
	ScrollWindow( RRealSize( 0, scrollAmount ) );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::OnScroll( )
//
//  Description:		Called to do scrolling
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YIntDimension RWindowView::OnScroll( RScrollBar* pScrollBar, UINT nSBCode, int nPos )
	{
	RRealSize scrollAmount( 0, 0 );

	// We do scrolling in device units, so calculate the line scroll amount in device units
	RRealSize lineScrollAmount( kLineScrollAmount, kLineScrollAmount );
	::LogicalUnitsToScreenUnits( lineScrollAmount );

	switch( nSBCode )
		{
		case SB_THUMBTRACK :
			scrollAmount.m_dx = pScrollBar->SetPosition( nPos );
			break;

		case SB_LINEUP :
			scrollAmount.m_dx = pScrollBar->Scroll( -lineScrollAmount.m_dx );
			break;

		case SB_LINEDOWN :
			scrollAmount.m_dx = pScrollBar->Scroll( lineScrollAmount.m_dx );
			break;

		case SB_PAGEUP :
			scrollAmount.m_dx = pScrollBar->ScrollPageUp( );
			break;

		case SB_PAGEDOWN :
			scrollAmount.m_dx = pScrollBar->ScrollPageDown( );
			break;

		case SB_ENDSCROLL :
			UpdateScrollBars( kScroll );
			break;
		}

	::DeviceUnitsToLogicalUnits( scrollAmount, *this );

	return scrollAmount.m_dx;
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::ScrollWindow( )
//
//  Description:		Scrolls this window
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::ScrollWindow( const RRealSize& scrollAmount )
	{
	// Convert to device units
	RRealSize tempSize = scrollAmount;
	::LogicalUnitsToDeviceUnits( tempSize, *this );

	// Round
	RIntSize deviceSize( tempSize );

	GetCWnd( ).ScrollWindow( deviceSize.m_dx, deviceSize.m_dy, NULL, NULL );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::GetLowestViewSupportingData( )
//
//  Description:		Figures out the lowest hit view which can incorporate the
//							data in this data source.
//
//  Returns:			The hit view
//
//  Exceptions:		None
//
// ****************************************************************************
//
RView* RWindowView::GetLowestViewSupportingData( const RDataTransferSource& dataSource, const RRealPoint& inPoint, RRealPoint& outPoint )
	{
	// If component format is available, take it, otherwise call down to the base class
	if( dataSource.IsFormatAvailable( kComponentFormat ) )
		{
		outPoint = ConvertPointToLocalCoordinateSystem( inPoint );
		return this;
		}

	return RView::GetLowestViewSupportingData( dataSource, inPoint, outPoint );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::ActivateView( )
//
//  Description:		Activate the view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::ActivateView( )
	{
	GetCWnd( ).SetForegroundWindow( );
	CFrameWnd*	pParentFrame	= GetCWnd( ).GetParentFrame();
	if( pParentFrame->GetParentFrame( ) )
		if ( pParentFrame->GetParentFrame( )->GetActiveFrame() != pParentFrame )
			pParentFrame->ActivateFrame( );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::GetAutoScrollDirection( )
//
//  Description:		Gets the direction autoscrolling would occur in
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RWindowView::EDirection RWindowView::GetAutoScrollDirection( const RRealPoint& mousePoint ) const
	{
	EDirection autoScrollDirection = kNone;

	// Convert the drag scroll inset from pixels to logical units
	RRealSize dragScrollInset( kDragScrollInset, kDragScrollInset );
	::ScreenUnitsToLogicalUnits( dragScrollInset );

	// Get the viewable area of this view
	RRealRect viewableArea = GetViewableArea( );

	// Check y direction
	if( mousePoint.m_y - viewableArea.m_Top < dragScrollInset.m_dy )
		autoScrollDirection = kUp;
	else if( viewableArea.m_Bottom - mousePoint.m_y < dragScrollInset.m_dy )
		autoScrollDirection = kDown;

	// Check x direction
	if( mousePoint.m_x - viewableArea.m_Left < dragScrollInset.m_dx )
		autoScrollDirection |= kLeft;
	else if( viewableArea.m_Right - mousePoint.m_x < dragScrollInset.m_dx )
		autoScrollDirection |= kRight;

	return autoScrollDirection;
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::AutoScroll( )
//
//  Description:		Does autoscrolling
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::AutoScroll( const RRealPoint& mousePoint, YRealDimension scrollAmount, BOOLEAN fIgnoreTime, EDirection allowableDirections )
	{
	// Get the direction to autoscroll
	EDirection autoScrollDirection = GetAutoScrollDirection( mousePoint );

	// Check to see if we are still going in a common direction
	if( autoScrollDirection & m_AutoScrollDirection )
		{
		// Check to see if enough time has elapsed
		if( fIgnoreTime || ::RenaissanceGetTickCount( ) > m_NextAutoScrollTime )
			{
			RRealSize scrollResult( 0, 0 );

			// We do scrolling in device units, so calculate the auto scroll amount in device units
			RRealSize autoScrollAmount( scrollAmount, scrollAmount );
			::LogicalUnitsToScreenUnits( autoScrollAmount );

			// Do autoscrolling
			if( autoScrollDirection & kLeft && allowableDirections & kLeft )
				scrollResult.m_dx = m_pHorizontalScrollBar->Scroll( floor( -autoScrollAmount.m_dx ) );
			else if( autoScrollDirection & kRight && allowableDirections & kRight )
				scrollResult.m_dx = m_pHorizontalScrollBar->Scroll( ceil( autoScrollAmount.m_dx ) );

			if( autoScrollDirection & kUp && allowableDirections & kUp )
				scrollResult.m_dy = m_pVerticalScrollBar->Scroll( floor( -autoScrollAmount.m_dx ) );
			else if( autoScrollDirection & kDown && allowableDirections & kDown )
				scrollResult.m_dy = m_pVerticalScrollBar->Scroll( ceil( autoScrollAmount.m_dx ) );

			// Convert the amount scrolled back to logical units and do the scroll
			::DeviceUnitsToLogicalUnits( scrollResult, *this );
			RWindowView::ScrollWindow( scrollResult );

			// Reset the next scroll time
			m_NextAutoScrollTime = ::RenaissanceGetTickCount( ) + kDragScrollInterval;
			}
		}

	// If we are not, reset the next scroll time
	else
		m_NextAutoScrollTime = ::RenaissanceGetTickCount( ) + kDragScrollDelay;

	// Set the last autoscroll direction
	m_AutoScrollDirection = autoScrollDirection;
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::WillAutoScroll( )
//
//  Description:		Determines if this view will autoscroll
//
//  Returns:			TRUE if the view will autoscroll
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RWindowView::WillAutoScroll( const RRealPoint& mousePoint )
	{
	// Get the direction to autoscroll
	EDirection autoScrollDirection = GetAutoScrollDirection( mousePoint );

	// Check to see if we are still going in a common direction
	if( autoScrollDirection & m_AutoScrollDirection )
		{
		// Check to see if enough time has elapsed
		if( ::RenaissanceGetTickCount( ) > m_NextAutoScrollTime )
			{
			// Check to see if we can actually scroll any further
			if( autoScrollDirection & kLeft && m_pHorizontalScrollBar->CanScrollDown( ) )
				return TRUE;

			if( autoScrollDirection & kUp && m_pVerticalScrollBar->CanScrollDown( ) )
				return TRUE;

			if( autoScrollDirection & kRight && m_pHorizontalScrollBar->CanScrollUp( ) )
				return TRUE;

			if( autoScrollDirection & kDown && m_pVerticalScrollBar->CanScrollUp( ) )
				return TRUE;
			}
		}

	return FALSE;
	}

// ****************************************************************************
//
//  Class Name:		RAutoScrollTimer
//
//  Description:		Timer for autoscrolling
//
// ****************************************************************************
//
class RAutoScrollTimer : public RTimer
	{
	// Construction
	public :
											RAutoScrollTimer( uLONG timeOut, RWindowView* pView );

	// Timer callback
	private :
		virtual void					OnTimer( );

	// Members
	private :
		RWindowView*					m_pView;
	};

// ****************************************************************************
//
//  Function Name:	RAutoScrollTimer::OnTimer( )
//
//  Description:		Called when the timer elapses
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RAutoScrollTimer::RAutoScrollTimer( uLONG timeOut, RWindowView* pView )
	: RTimer( timeOut ),
	  m_pView( pView )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RAutoScrollTimer::OnTimer( )
//
//  Description:		Called when the timer elapses
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RAutoScrollTimer::OnTimer( )
	{
	// Tell the view
	m_pView->OnAutoScrollTimer( );
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::SetupAutoScrollTimer( )
//
//  Description:		Sets up the autoscroll timer for non-dragdrop autoscrolling
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::SetupAutoScrollTimer( const RRealPoint& mousePoint )
	{
	// Is there already a timer setup? If so, we are done
	if( m_pAutoScrollTimer )
		return;

	// Get the direction to autoscroll
	EDirection autoScrollDirection = GetAutoScrollDirection( mousePoint );

	// Only set the timer up if we have a scroll direction
	if( autoScrollDirection != kNone )
		{
		m_pAutoScrollTimer = new RAutoScrollTimer( kDragScrollInterval, this );
		m_AutoScrollDirection = autoScrollDirection;
		m_NextAutoScrollTime = ::RenaissanceGetTickCount( ) + kDragScrollDelay;
		}
	}

// ****************************************************************************
//
//  Function Name:	RWindowView::OnAutoScrollTimer( )
//
//  Description:		Called by the autoscroll timer when its timer elapses
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RWindowView::OnAutoScrollTimer( )
	{
	// Get the modifier key state
	YModifierKey modifierKeys = GetModifierKeyState( );

	// Get the current mouse position in both device coordinates and local coordinates
#ifdef _WINDOWS
	POINT	pt;
	::GetCursorPos( &pt );
	GetCWnd( ).ScreenToClient( &pt );
	RRealPoint devicePoint = RRealPoint( pt.x, pt.y );
	RRealPoint localPoint = devicePoint;
	::DeviceUnitsToLogicalUnits( localPoint, *this );
	localPoint = ConvertPointToLocalCoordinateSystem( localPoint );
#endif

	// Get the direction to autoscroll
	EDirection autoScrollDirection = GetAutoScrollDirection( localPoint );

	// If the left button is no longer down, or we are not moving in the same direction,
	// kill the timer.
	if( !( IsMouseCaptured() /*modifierKeys & kModifierLeftButton*/ ) || !( autoScrollDirection & m_AutoScrollDirection ) )
		{
		delete m_pAutoScrollTimer;
		m_pAutoScrollTimer = 0;
		}

	// Otherwise, we are good to autoscroll. Simulate a mouse move event
	else
		OnXMouseMessage( kMouseMove, devicePoint, modifierKeys );
	}
