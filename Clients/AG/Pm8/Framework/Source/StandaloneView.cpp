// ****************************************************************************
//
//  File Name:			CommonStandaloneView.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RStandaloneView class
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
//  $Logfile:: /PM8/Framework/Source/StandaloneView.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"StandaloneView.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FrameworkResourceIDs.h"
#include "AutoDrawingSurface.h"
#include "ApplicationGlobals.h"
#include "StandaloneApplication.h"
#include "ScrollBar.h"
#include "MfcDataTransfer.h"
#include "StandaloneDocument.h"

// Command Map
RCommandMap<RStandaloneView, RView> RStandaloneView::m_CommandMap;

RCommandMap<RStandaloneView, RView>::RCommandMap( )
	{
	HandleAndUpdateCommandRange( COMMAND_MENU_ZOOM_START, COMMAND_MENU_ZOOM_END, RStandaloneView::OnZoom, RStandaloneView::OnUpdateZoom );
	HandleAndUpdateCommand( COMMAND_MENU_VIEW_FITTOWINDOW, RStandaloneView::OnFitToWindow, RStandaloneView::OnUpdateFitToWindow );
	HandleAndUpdateCommand( COMMAND_MENU_VIEW_FITOBJECTSTOWINDOW, RStandaloneView::OnFitObjectsToWindow, RStandaloneView::OnUpdateFitObjectsToWindow );
	HandleAndUpdateCommand( COMMAND_MENU_VIEW_ZOOMIN, RStandaloneView::OnZoomIn, RStandaloneView::OnUpdateZoomIn );
	HandleAndUpdateCommand( COMMAND_MENU_VIEW_ZOOMOUT, RStandaloneView::OnZoomOut, RStandaloneView::OnUpdateZoomOut );
	}

#ifdef _WINDOWS

// Generate code necessary for MFC runtime class information
IMPLEMENT_DYNAMIC(RStandaloneView, CView)

#endif

// ****************************************************************************
//
//  Function Name:	RStandaloneView::RStandaloneView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneView::RStandaloneView( )
	: m_ZoomLevel( 1.0 ),
	  m_fFitToWindow( TRUE )
	{
	FrameworkView::SetView( this );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::~RStandaloneView( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneView::~RStandaloneView( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::Initialize
//
//  Description:		Initializer
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::Initialize( RDocument* pDocument )
	{
	RWindowView::Initialize( pDocument );

	RecalculateFitToWindow( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::SetZoomLevel( )
//
//  Description:		Sets the zoom level of this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::SetZoomLevel( YZoomLevel zoomLevel, BOOLEAN fUpdateScrollBars )
	{
	// Change the zoom level
	m_ZoomLevel = zoomLevel;

	// If we are updating the scroll bars, do it
	if( fUpdateScrollBars )
		UpdateScrollBars( kZoom );

	// Invalidate and update the view
	RWindowView::Invalidate( );
	XUpdateAllViews( kViewZoomChanged, 0 );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::GetZoomLevel( )
//
//  Description:		Accessor
//
//  Returns:			The current zoom level
//
//  Exceptions:		None
//
// ****************************************************************************
//
YZoomLevel RStandaloneView::GetZoomLevel( ) const
	{
	return m_ZoomLevel;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::AddClipRegion( )
//
//  Description:		Creates a new clip region that is the intersection of the
//							current clip region and a region that will clip all output
//							to the bounds of this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::SetClipRegion( RDrawingSurface&, const R2dTransform& ) const
	{
	// Dont need to do anything. If the drawing surface was created from a paint
	// message the clip region is already set. Otherwise it was created with a 
	// clip region the size of our window by default
	return;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::GetScreenDPI( )
//
//  Description:		Accessor
//
//  Returns:			The DPI of the screen
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealSize RStandaloneView::GetDPI( ) const
	{
	return RRealSize( m_ScreenDPI.m_dx * m_ZoomLevel, m_ScreenDPI.m_dy * m_ZoomLevel );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnZoom( )
//
//  Description:		Command handler
//
//  Returns:			TRUE if the command was handled
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnZoom( YCommandID commandId )
	{
	YZoomLevelIterator iterator = m_ZoomLevelMap.Start( );
	for( ; iterator != m_ZoomLevelMap.End( ); ++iterator )
		{
		if( ( *iterator ).m_CommandId == commandId )
			{
			m_fFitToWindow = FALSE;
			SetZoomLevel( ( *iterator ).m_ZoomLevel );
			return;
			}
		}

	TpsAssertAlways( "Did not find a zoom command match." );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnUpdateZoom( )
//
//  Description:		Command updater
//
//  Returns:			TRUE if the command was updated
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnUpdateZoom( RCommandUI& commandUI ) const
	{
	YZoomLevelIterator iterator = m_ZoomLevelMap.Start( );
	for( ; iterator != m_ZoomLevelMap.End( ); ++iterator )
		{
		if( ( *iterator ).m_CommandId == commandUI.GetCommandID( ) )
			{
			commandUI.Enable( );

			if( !m_fFitToWindow && GetZoomLevel( ) == ( *iterator ).m_ZoomLevel )
				commandUI.SetCheck( RCommandUI::kCheckMark );
			else
				commandUI.SetCheck( RCommandUI::kNoCheck );

			return;
			}
		}

	TpsAssertAlways( "Did not find a zoom command match." );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnFitToWindow( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnFitToWindow( )
	{
	m_fFitToWindow = TRUE;
	RecalculateFitToWindow( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnUpdateFitToWindow( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnUpdateFitToWindow( RCommandUI& commandUI ) const
	{
	commandUI.Enable( );

	if( m_fFitToWindow )
		commandUI.SetCheck( RCommandUI::kCheckMark );
	else
		commandUI.SetCheck( RCommandUI::kNoCheck );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnFitObjectsToWindow( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnFitObjectsToWindow( )
	{
	m_fFitToWindow = FALSE;
	RecalculateFitObjectsToWindow( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnUpdateFitObjectsToWindow( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnUpdateFitObjectsToWindow( RCommandUI& commandUI ) const
	{
	commandUI.Enable( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnZoomIn( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnZoomIn( )
	{
	// Look through the zoom level map for the first zoom level greater than the
	// current zoom level
	YZoomLevelIterator iterator = m_ZoomLevelMap.Start( );
	for( ; iterator != m_ZoomLevelMap.End( ); ++iterator )
		{
		if( ( *iterator ).m_ZoomLevel > GetZoomLevel( ) )
			{
			m_fFitToWindow = FALSE;
			SetZoomLevel( ( *iterator ).m_ZoomLevel );
			break;
			}
		}
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnUpdateZoomIn( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnUpdateZoomIn( RCommandUI& commandUI ) const
	{
	// We can zoom in if the current zoom level is smaller than the last zoom level
	YZoomLevelIterator iterator = m_ZoomLevelMap.End( );
	if( m_ZoomLevelMap.Count( ) > 0 && GetZoomLevel( ) < ( *--iterator ).m_ZoomLevel )
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnZoomOut( )
//
//  Description:		Command handler
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnZoomOut( )
	{
	YZoomLevel zoomLevel( 1.0 );

	// If the current zoom level is > the largest zoom level, just use it
	YZoomLevelIterator	zoomMapEnd	= m_ZoomLevelMap.End( );
	if( AreFloatsGT( GetZoomLevel( ), ( *--zoomMapEnd ).m_ZoomLevel ) )
		zoomLevel = ( *--zoomMapEnd ).m_ZoomLevel;

	// Otherwise, look through the zoom level map for the first zoom
	// level greater than or equal to the current zoom level. Then use
	// the one before it.
	else
		{
		YZoomLevelIterator iterator = m_ZoomLevelMap.Start( );
		for( ; iterator != m_ZoomLevelMap.End( ); ++iterator )
			if( ( *iterator ).m_ZoomLevel >= GetZoomLevel( ) )
				if( iterator != m_ZoomLevelMap.Start( ) )
					{
					zoomLevel = ( *( --iterator ) ).m_ZoomLevel;
					break;
					}
		}

	m_fFitToWindow = FALSE;
	SetZoomLevel( zoomLevel );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnUpdateZoomOut( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnUpdateZoomOut( RCommandUI& commandUI ) const
	{
	// We can zoom out if the current zoom level is larger than the first zoom level
	if( m_ZoomLevelMap.Count( ) > 0 && GetZoomLevel( ) > ( *m_ZoomLevelMap.Start( ) ).m_ZoomLevel )
		commandUI.Enable( );
	else
		commandUI.Disable( );
	}

#ifdef _WINDOWS

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnDraw( )
//
//  Description:		Called by MFC to redraw this StandaloneView. This should 
//							only be called for print and printpreview
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnDraw( CDC* )
	{
	TpsAssertAlways( "CView printing fuctions should not be called." );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnPreparePrinting( )
//
//  Description:		Must be implemented to enable MFC printing
//
//  Returns:			TRUE if printing should continue
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOL RStandaloneView::OnPreparePrinting( CPrintInfo* )
	{
	TpsAssertAlways( "CView printing fuctions should not be called." );
	return FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::GetCWnd( )
//
//  Description:		Gets the top level CWnd
//
//  Returns:			The CWnd&
//
//  Exceptions:		None
//
// ****************************************************************************
//
CWnd& RStandaloneView::GetCWnd( ) const
	{
	//TpsAssertIsObject( CWnd, "Bogus CWnd." );
	return *(CWnd*)this;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::OnXResize( )
//
//  Description:		Called when the view is resized
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::OnXResize( const RRealSize& size )
	{
	if( m_fFitToWindow )
		RecalculateFitToWindow( );

	RWindowView::OnXResize( size );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::InitialUpdate( )
//
//  Description:		Called to initialize a view after its document has been
//							created and loaded
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::InitialUpdate( )
	{
	RecalculateFitToWindow( );

#ifdef _WINDOWS
	// Update the frame
	GetParentFrame( )->InitialUpdateFrame( GetDocument( ), TRUE );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneDocument::GetStandaloneDocument( )
//
//  Description:		Accessor
//
//  Returns:			Standalone document
//
//  Exceptions:		None
//
// ****************************************************************************
//
RStandaloneDocument* RStandaloneView::GetStandaloneDocument( ) const
	{
	TpsAssertIsObject( RStandaloneDocument, GetRDocument( ) );
	return static_cast<RStandaloneDocument*>( GetRDocument( ) );
	}

#endif _WINDOWS

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RStandaloneView::Validate( )
//
//  Description:		Validate the object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::Validate( ) const
	{
	RView::Validate( );
	TpsAssertIsObject( RStandaloneView, this );
	}

#endif	//	TPSDEBUG
