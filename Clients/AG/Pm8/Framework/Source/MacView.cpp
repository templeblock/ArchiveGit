// ****************************************************************************
//
//  File Name:			MacView.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RView class
//
//  Portability:		Mac Specific
//
//  Developed by:		Turning Point Software.
//						One Gateway Center, Suite 800
//						Newton, MA 02158
//						(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/MacView.cpp                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

//	Platform Independent includes

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "View.h"
#include "Document.h"

//----------------------------------------------------------------------------------------
// Constants;
	
#undef Inherited
#define Inherited RObject

// ****************************************************************************
//
//  Function Name:	RView::WindowQDPointToViewRPoint( )
//
//  Description:		Translate a Window based QDPoint to a View based RPoint.
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntPoint RView::WindowQDPointToViewRPoint( const Point& ptQD ) const
	{
	Point			ptLocal			= ptQD;
	LCommandoView&	PlatformView	= GetCommandoView( );
	PlatformView.PortToLocalPoint( ptLocal );
	RIntPoint		viewPoint( ptLocal.h, ptLocal.v );
	return viewPoint;
	}

// ****************************************************************************
//
//  Function Name:	RView::ViewRPointToWindowQDPoint( )
//
//  Description:		Translate a View based RPoint to a Window based QD Point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
Point RView::ViewRPointToWindowQDPoint( const RIntPoint& rptView ) const
	{
	LCommandoView&	PlatformView	= GetCommandoView( );
	Point			ptQD;
	ptQD.h = rptView.m_x;
	ptQD.v = rptView.m_y;
	PlatformView.LocalToPortPoint( ptQD );
	return ptQD;
	}


// ****************************************************************************
//
//  Function Name:	RView::ProcessMouseDown( )
//
//  Description:		Process a Mouse Down Message.
//
//  Returns:			the Modifiers
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::ProcessMouseDown( const Point ptPortMouse, YModifierKey yModifiers )
	{
	Point		ptQDMouse		= ptPortMouse;
	LView&		platformView	= GetCommandoView();
	if (!platformView.FocusDraw())
		///xxx	TODO: Throw some sort of unknown system error here.
		return;

	RIntPoint	rptMouseLocal = WindowQDPointToViewRPoint( ptQDMouse );

	SetupMouseDownState( rptMouseLocal, yModifiers );
	OnXLButtonDown( rptMouseLocal, yModifiers );

#if 	ONLY_PROCESS_MOUSE_IF_CAPTURED
	//	If the mouse is captured keep the processing for the MouseMove events
	if (IsMouseCaptured())
#endif
	{

		//	Mac Processing.  While the Mouse Is Still down, send MouseMove events
		//	whenever the mouse moves.  After StillDown returns False, 
			//
			//		I am not sure if the double still down is still necessary, but it 
			//	use to be because some mouses would occationally ping that the mouse
			//	button was up when it really wasn't... but it wouldn't lie twice...
		while ( StillDown() || StillDown() )
			{
			RIntPoint	rptNextMouse;

			//	Get the current mouse coordinate convert it to Window coordinates and
			//	then back to the current view.
			GetMouse(&ptQDMouse);
			platformView.LocalToPortPoint( ptQDMouse );
			rptNextMouse = WindowQDPointToViewRPoint( ptQDMouse );
	
			if (rptMouseLocal != rptNextMouse)
				{
				OnXMouseMove( rptNextMouse, yModifiers );
				rptMouseLocal = rptNextMouse;
				}
			}
		}
	OnXLButtonUp( rptMouseLocal, yModifiers );
	//	Reset the mouseDown time to be the current time to measure the time between
	//	mouse up and mouse down, not between mouse down and mouse down
	m_MouseDownTime	= RenaissanceGetTickCount();
	}

// ****************************************************************************
//
//  Function Name:	RView::SetKeyFocus( )
//
//  Description:		Set the keyfocus so that characters are routed to this view.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RView::SetKeyFocus( )
	{
	OnXSetKeyFocus( );
	}

// ****************************************************************************
//
//  Function Name:	RView::IsEventPending( )
//
//  Description:		Check if there are any pending events in the Queue.
//
//  Returns:			TRUE if the next system event is a key event
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::IsEventPending( ) const
	{
	EventRecord evt;
	return (BOOLEAN)EventAvail( everyEvent, &evt );
	}

// ****************************************************************************
//
//  Function Name:	RView::IsKeyEventPending( )
//
//  Description:		Check if there are any pending key events in the Queue.
//
//  Returns:			TRUE if the next system event is a key event
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RView::IsKeyEventPending( ) const
	{
	EventRecord evt;
	if ( EventAvail( everyEvent, &evt ))
		return (BOOLEAN)(evt.what & (keyDownMask | autoKeyMask));
	return FALSE;
	}
