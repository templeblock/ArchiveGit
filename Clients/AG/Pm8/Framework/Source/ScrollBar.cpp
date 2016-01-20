// ****************************************************************************
//
//  File Name:			ScrollBar.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RScrollBar class
//
//  Portability:		Mostly platform independent, with some platform specific
//							calls to set the actual scroll information.
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
//  $Logfile:: /PM8/Framework/Source/ScrollBar.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "ScrollBar.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"WindowView.h"

const YIntDimension kMaxScrollRange = SHRT_MAX;
const YFloatType kPageScrollPercentage = 0.95;

// ****************************************************************************
//
//  Function Name:	RScrollBar::RScrollBar( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RScrollBar::RScrollBar( YScrollBarType bar, RWindowView* pView )
	: m_Bar( bar ),
	  m_Handle( pView->GetCWnd( ).GetSafeHwnd( ) ),
	  m_ScrollGranularity( 1 )
	{
	SetRange( 0, 0, 0, FALSE );
	}

// ****************************************************************************
//
//  Function Name:	RScrollBar::RScrollBar( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RScrollBar::RScrollBar( CScrollBar* pScrollBar )
	: m_Bar( kControl ),
	  m_Handle( pScrollBar->GetSafeHwnd( ) ),
	  m_ScrollGranularity( 1 )
	{
	SetRange( 0, 0, 0, FALSE );
	}

// ****************************************************************************
//
//  Function Name:	RScrollBar::SetRange( )
//
//  Description:		Sets the range of the specified scroll bar, in logical
//							units.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollBar::SetRange( YIntDimension minRange, YIntDimension maxRange, YIntDimension pageSize, BOOLEAN fUpdateOnlyIfExtending )
	{
	TpsAssert( maxRange >= minRange, "Max scrolling range must be > min scrolling range." );
	TpsAssert( ( minRange == 0 && maxRange == 0 ) || pageSize >= 0 && pageSize <= ( maxRange - minRange + 1), "Invalid page size" );

	m_PageSize = pageSize;

	minRange = m_MinScrollRange = fUpdateOnlyIfExtending ? min( m_MinScrollRange, minRange ) : minRange;
	maxRange = m_MaxScrollRange = fUpdateOnlyIfExtending ? max( m_MaxScrollRange, maxRange ) : maxRange;

	TpsAssert( ( minRange == 0 && maxRange == 0 ) || pageSize >= 0 && pageSize <= ( maxRange - minRange + 1 ), "Invalid page size" );

	// The Windows and MacOS have a 16bit limit on scrolling values.
	// Figure out how many logical units each scrolling unit is
	m_ScrollGranularity = max ( ( ::Abs( minRange ) / kMaxScrollRange ) + 1, ( ::Abs( maxRange ) / kMaxScrollRange ) + 1 );

	// Set both the position and range for the appropriate scroll bar. This is a platform specific call
#ifdef	_WINDOWS
	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof( scrollInfo );
	scrollInfo.fMask = SIF_RANGE | SIF_PAGE;
	scrollInfo.nMin = minRange / m_ScrollGranularity;
	scrollInfo.nMax = maxRange / m_ScrollGranularity;
	scrollInfo.nPage = pageSize / m_ScrollGranularity;
	::SetScrollInfo( m_Handle, m_Bar, &scrollInfo, TRUE );
#endif	// _WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RScrollBar::SetPosition( )
//
//  Description:		Scrolls to the specified position
//
//  Returns:			The amount the window needs to be scrolled
//
//  Exceptions:		None
//
// ****************************************************************************
//
YIntDimension RScrollBar::SetPosition( YIntDimension position )
	{
	// If the new position is out of range, fix it up so that it stays in range
	if( position + m_PageSize > m_MaxScrollRange + 1 )
		position = m_MaxScrollRange - m_PageSize + 1;

	if( position < m_MinScrollRange )
		position = m_MinScrollRange;

	// Save the current position
	YIntDimension oldPosition = GetPosition( );

	// Set the position for the appropriate scroll bar. This is a platform specific call
#ifdef	_WINDOWS
	SCROLLINFO scrollInfo;
	scrollInfo.cbSize = sizeof( scrollInfo );
	scrollInfo.fMask = SIF_POS;
	scrollInfo.nPos = position;
	::SetScrollInfo( m_Handle, m_Bar, &scrollInfo, TRUE );
#endif	// _WINDOWS

	// Return the amount we actually scrolled
	return oldPosition - GetPosition( );
	}

// ****************************************************************************
//
//  Function Name:	RScrollBar::Scroll( )
//
//  Description:		Called to scroll up/left
//
//  Returns:			The amount the window needs to be scrolled
//
//  Exceptions:		None
//
// ****************************************************************************
//
YIntDimension RScrollBar::Scroll( YIntDimension scrollAmount )
	{
	return SetPosition( ( GetPosition( ) + scrollAmount ) / m_ScrollGranularity );
	}

// ****************************************************************************
//
//  Function Name:	RScrollBar::ScrollPageUp( )
//
//  Description:		Called to scroll up/left by a page
//
//  Returns:			The amount the window needs to be scrolled
//
//  Exceptions:		None
//
// ****************************************************************************
//
YIntDimension RScrollBar::ScrollPageUp( )
	{
	return SetPosition( ( GetPosition( ) - (YIntDimension)( m_PageSize * kPageScrollPercentage ) ) / m_ScrollGranularity );
	}

// ****************************************************************************
//
//  Function Name:	RScrollBar::ScrollPageDown( )
//
//  Description:		Called to scroll down/right by a page
//
//  Returns:			The amount the window needs to be scrolled
//
//  Exceptions:		None
//
// ****************************************************************************
//
YIntDimension RScrollBar::ScrollPageDown( )
	{
	return SetPosition( ( GetPosition( ) + (YIntDimension)( m_PageSize * kPageScrollPercentage ) ) / m_ScrollGranularity );
	}

// ****************************************************************************
//
//  Function Name:	RScrollBar::GetPosition( )
//
//  Description:		Accessor
//
//  Returns:			The current scroll position
//
//  Exceptions:		None
//
// ****************************************************************************
//
YIntDimension RScrollBar::GetPosition( ) const
	{
#ifdef	_WINDOWS
	return ::GetScrollPos( m_Handle, m_Bar ) * m_ScrollGranularity;
#endif	// _WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RScrollBar::GetPosition( )
//
//  Description:		Accessor
//
//  Returns:			The current scroll position
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollBar::GetValues( YIntDimension& minRange, YIntDimension& maxRange, YIntDimension& position ) const
{
	minRange	= m_MinScrollRange;
	maxRange	= m_MaxScrollRange;
	position	= GetPosition();
}

// ****************************************************************************
//
//  Function Name:	RScrollBar::CanScrollUp( )
//
//  Description:		Accessor
//
//  Returns:			TRUE if this scrollbar can be scrolled up any further
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RScrollBar::CanScrollUp( ) const
	{
	return static_cast<BOOLEAN>( GetPosition( ) + m_PageSize < m_MaxScrollRange );
	}

// ****************************************************************************
//
//  Function Name:	RScrollBar::CanScrollDown( )
//
//  Description:		Accessor
//
//  Returns:			TRUE if this scrollbar can be scrolled down any further
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RScrollBar::CanScrollDown( ) const
	{
	return static_cast<BOOLEAN>( GetPosition( ) > m_MinScrollRange );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RScrollBar::Validate( )
//
//  Description:		Validate the object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RScrollBar::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RScrollBar, this );
	TpsAssert( m_Bar == kHorizontal || m_Bar == kVertical, "Invalid scroll bar" );
	}

#endif	//	TPSDEBUG
