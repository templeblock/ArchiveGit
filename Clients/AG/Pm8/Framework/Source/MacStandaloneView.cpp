// ****************************************************************************
//
//  File Name:			MacStandaloneView.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RStandaloneView class
//
//  Portability:		MacSpecific
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
//  $Logfile:: /PM8/Framework/Source/MacStandaloneView.cpp                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"StandaloneView.h"

// ****************************************************************************
//
//  Function Name:	RStandaloneView::ActivateSelf( )
//
//  Description:		Called by the framework when the view is activated.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::ActivateSelf( )
{
	//	Set the default commander to be the super commander (ie. the window)
	SetDefaultCommander( this );
	///xxx	REVIEW	MDH (mdh) Should I call LView::ActivateSelf();
}


// ****************************************************************************
//
//  Function Name:	RStandaloneView::DeactivateSelf( )
//
//  Description:		Called by the framework when the view is activated.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::DeactivateSelf( )
{
	LView::DeactivateSelf();
}


// ****************************************************************************
//
//  Function Name:	RStandaloneView::DrawSelf( )
//
//  Description:		Component API to draw yourself.
//
//  Returns:			TODO: What does this return?
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::DrawSelf( )
	{
	RgnHandle	rgnUpdate = GetLocalUpdateRgn( );
	if (rgnUpdate && !EmptyRgn(rgnUpdate) )
		{
		Rect		rcLocal = (**rgnUpdate).rgnBBox;
		RIntRect	rcRender( rcLocal.left, rcLocal.top, rcLocal.right, rcLocal.bottom );

		//	Call the framework to image
		RViewDrawingSurface		drawingSurface( this );
		OnXRender( drawingSurface, rcRender );
		}
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::ObeyCommand( )
//
//  Description:		Respond to Menu Commands.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
Boolean
RStandaloneView::ObeyCommand(
	CommandT	inCommand,
	void		*ioParam)
{
	Boolean		cmdHandled	= OnXCommand( (YCommandID)inCommand );

	if (!cmdHandled)
		cmdHandled = LCommander::ObeyCommand(inCommand, ioParam);
	
	return cmdHandled;
}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::FindCommandStatus( )
//
//  Description:		Calculate the status of the given menu items.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void
RStandaloneView::FindCommandStatus(
	CommandT	inCommand,
	Boolean		&outEnabled,
	Boolean		&outUsesMark,
	Char16		&outMark,
	Str255		outName)
	{
	outEnabled	= TRUE;
	outUsesMark	= FALSE;
#if 0
	switch (inCommand) 
		{
	
		// Return menu item status according to command messages.
		// Any that you don't handle will be passed to LCommander

		default:
			LCommander::FindCommandStatus(inCommand, outEnabled,
												outUsesMark, outMark, outName);
			break;
		}
#endif

	return;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::Invalidate( )
//
//  Description:		Invalidates the entire view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::Invalidate( BOOLEAN fErase )
	{
	Rect	rtInval = { -32000, -32000, 32000, 32000 };	//	Very large rectangle... will be clipped by quickdraw...
	InvalRect( &rtInval );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::InvalidateRect( )
//
//  Description:		Invalidates the given rectangle in this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::InvalidateRect( const RRealRect& rect, BOOLEAN fErase )
	{
	Rect	rtInval;
	//	Convert to device units here
	SetRect(&rtInval, ::Round(rect.m_Left), ::Round(rect.m_Top), ::Round(rect.m_Right), ::Round(rect.m_Bottom) );
	InvalRect( &rtInval );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::ValidateRect( )
//
//  Description:		Validate the given rectangle in this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::ValidateRect( const RRealRect& rect )
	{
	Rect	rtValid;
	//	Convert to device units here
	SetRect(&rtValid, ::Round(rect.m_Left), ::Round(rect.m_Top), ::Round(rect.m_Right), ::Round(rect.m_Bottom) );
	ValidRect( &rtValid );
	}


// ****************************************************************************
//
//  Function Name:	RStandaloneView::InvalidateVectorRect( )
//
//  Description:		Invalidates the given vector rect in this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::InvalidateVectorRect( const RRealVectorRect& rect, BOOLEAN fErase )
	{
	// Get the bounding rect and inflate it
	RRealRect temp = rect.m_TransformedBoundingRect;
	temp.Inflate( RIntSize( 1, 1 ) );

	// Just invalidate the bounding rect
	InvalidateRect( temp, fErase );
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::ValidateVectorRect( )
//
//  Description:		Validates the given vector rect in this view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::ValidateVectorRect( const RRealVectorRect& rect )
	{
	// Convert to device units
	RRealVectorRect temp = rect;
	::LogicalUnitsToDeviceUnits( temp, *this );
//	temp.Inflate( RIntSize( 1, 1 ) );

	// Get the polygon points
	RRealPoint polygon[ 4 ];
	temp.GetPolygonPoints( polygon );

	// Create a polyogn region
	RIntPoint	polygon[4];
	polygon[0]	= realPolygon[0];
	polygon[1]	= realPolygon[1];
	polygon[2]	= realPolygon[2];
	polygon[3]	= realPolygon[3];

	UnimplementedCode();
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::GetFrameworkView( )
//
//  Description:		Converts to a TView reference. This is a virtual casting
//							operator used to convert from an RView to a TView&. This
//							can not be done automatically because of the inheritance
//							hierarchy.
//
//  Returns:			The TView&
//
//  Exceptions:		None
//
// ****************************************************************************
//
LCommandoView& RStandaloneView::GetCommandoView( ) const
	{
	TpsAssertIsObject( LCommandoView, "Bogus LCommandoView." );
	return *(LCommandoView*)this;
	}

// ****************************************************************************
//
//  Function Name:	RStandaloneView::ClickSelf( )
//
//  Description:		Process a click in the view
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RStandaloneView::ClickSelf( const SMouseDownEvent& inMouseDown )
	{
	//	Convert to framework coordinates and call framework
	ProcessMouseDown( inMouseDown.wherePort, (YModifierKey)inMouseDown.macEvent.modifiers );
	}
