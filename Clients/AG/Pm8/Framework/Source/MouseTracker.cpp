// ****************************************************************************
//
//  File Name:			MouseTracker.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RMouseTracker class
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
//  $Logfile:: /PM8/Framework/Source/MouseTracker.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"MouseTracker.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"View.h"
#include "ViewDrawingSurface.h"

// ****************************************************************************
//
//  Function Name:	RMouseTracker::RMouseTracker( )
//
//  Description:		Default constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMouseTracker::RMouseTracker( ) : m_fTracking( FALSE ), m_pView( NULL )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMouseTracker::RMouseTracker( )
//
//  Description:		constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMouseTracker::RMouseTracker( RView* pView )
	: m_fTracking( FALSE ),
	  m_pView( pView )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMouseTracker::~RMouseTracker( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMouseTracker::~RMouseTracker( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMouseTracker::Initialize( )
//
//  Description:		Initializer
//
//							NOTE:  This function is necessary since when a view is
//							instanciated with an imbedded tracker, it does not really
//							know what what it is.  This causes problems where Multiple
//							inheritence	stores the wrong pointer to the RView class
//							thereby crashing the program when it is (in)correctly used.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMouseTracker::Initialize( RView *pView )
	{
	m_pView = pView;
	}

// ****************************************************************************
//
//  Function Name:	RMouseTracker::BeginTracking( )
//
//  Description:		Called when tracking begins; ie. when the mouse button goes
//							down.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMouseTracker::BeginTracking( const RRealPoint& /* mousePoint */, YModifierKey )
	{
	TpsAssert( m_pView != NULL, "The Mouse Tracker has never been initialized");
	TpsAssert( m_fTracking == FALSE, "Already tracking" );

	m_fTracking = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RMouseTracker::EndTracking( )
//
//  Description:		Called when tracking ends; ie. when the mouse button goes
//							up.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMouseTracker::EndTracking( const RRealPoint&, YModifierKey  )
	{
	TpsAssert( m_fTracking == TRUE, "Not tracking" );

	m_fTracking = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RMouseTracker::CancelTracking( )
//
//  Description:		Called when tracking is canceled. 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMouseTracker::CancelTracking( )
	{
	// End tracking
	TpsAssert( m_fTracking == TRUE, "Not tracking" );
	m_pView->ReleaseMouseCapture( );
	m_fTracking = FALSE;

	// Erase the old dragging feedback
	Render( );
	}

// ****************************************************************************
//
//  Function Name:	RMouseTracker::Render( )
//
//  Description:		Renders the tracker feedback
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMouseTracker::Render( ) const
	{
	// Get a drawing surface and transform
	RViewDrawingSurface drawingSurface( m_pView );
	R2dTransform transform;
	m_pView->GetViewTransform( transform, drawingSurface, TRUE );
	m_pView->SetClipRegion( drawingSurface, transform );

	// Render
	Render( drawingSurface, transform );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RMouseTracker::Validate( )
//
//  Description:		Called when tracking begins; ie. when the mouse button goes
//							down.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMouseTracker::Validate(  ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RView, m_pView );
	}

#endif	//	TPSDEBUG