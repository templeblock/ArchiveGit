// ****************************************************************************
//
//  File Name:			Frame.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				Eric VanHelene
//
//  Description:		Definition of the RFrame class
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
//  $Logfile:: /PM8/Framework/Source/Frame.cpp                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"Frame.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "FrameworkResourceIDs.h"
#include "DrawingSurface.h"
#include "ComponentView.h"
#include "Path.h"

#define kFrameSizeBaseline					::InchesToLogicalUnits( 2.0 )

//	line thicknesses
#define kFrameThinLineSize					::InchesToLogicalUnits( 1./72. )
#define kFrameRoundCornerLineSize			::InchesToLogicalUnits( 1./72. )
#define kFrameThickLineSize					::InchesToLogicalUnits( 1./32. )
#define kFrameMediumLineSize				::InchesToLogicalUnits( 1./48. )
#define kFramePictureLineSize				::InchesToLogicalUnits( 1./64. )
#define kFrameDoubleInnerLineSize			::InchesToLogicalUnits( 1./72. )
#define kFrameDoubleOuterLineSize			::InchesToLogicalUnits( 1./72. )
#define kFrameDoubleInterLineSize			::InchesToLogicalUnits( 1./24. )
#define kFrameDoubleInnerMediumLineSize		::InchesToLogicalUnits( 1./48. )
#define kFrameDoubleOuterMediumLineSize		::InchesToLogicalUnits( 1./48. )
#define kFrameDoubleInterMediumLineSize		::InchesToLogicalUnits( 1./16. )
#define kFrameDoubleInnerThickLineSize		::InchesToLogicalUnits( 1./32. )
#define kFrameDoubleOuterThickLineSize		::InchesToLogicalUnits( 1./32. )
#define kFrameDoubleInterThickLineSize		::InchesToLogicalUnits( 1./12 )
#define kFrameDropShadowInnerLineSize		::InchesToLogicalUnits( 1./72. )
#define kFrameDropShadowOuterLineSize		::InchesToLogicalUnits( 1./16. )

//	other feature sizes
#define kFrameRoundCornerRadius				::InchesToLogicalUnits( 1./12. )
#define kFramePictureTabWidth				::InchesToLogicalUnits( 1./16. )
#define kFramePictureTabLength				::InchesToLogicalUnits( 1./8. )

#define kNumPictureCornerVertices			7
#define kNumArcVertices						7

RFrameTypeMapEntry _FrameTypeMap[]	=
{
	RFrameTypeMapEntry( COMMAND_MENU_EFFECTS_FRAME_NONE, kNoFrame ),
	RFrameTypeMapEntry( COMMAND_MENU_EFFECTS_FRAME_THINLINE, kThinLineFrame ),
	RFrameTypeMapEntry( COMMAND_MENU_EFFECTS_FRAME_MEDIUMLINE, kMediumLineFrame ),
	RFrameTypeMapEntry( COMMAND_MENU_EFFECTS_FRAME_THICKLINE, kThickLineFrame ),
	RFrameTypeMapEntry( COMMAND_MENU_EFFECTS_FRAME_ROUNDCORNER, kRoundCornerFrame ),
	RFrameTypeMapEntry( COMMAND_MENU_EFFECTS_FRAME_DOUBLELINE, kDoubleLineFrame ),
	RFrameTypeMapEntry( COMMAND_MENU_EFFECTS_FRAME_DOUBLEMEDIUMLINE, kMediumDoubleLineFrame ),
	RFrameTypeMapEntry( COMMAND_MENU_EFFECTS_FRAME_DOUBLETHICKLINE, kThickDoubleLineFrame ),
	RFrameTypeMapEntry( COMMAND_MENU_EFFECTS_FRAME_DROPSHADOW, kDropShadowFrame ),
	RFrameTypeMapEntry( COMMAND_MENU_EFFECTS_FRAME_PICTURE, kPictureFrame )
} ;

// ****************************************************************************
//
//  Function Name:	RFrameTypeMapEntry::RFrameTypeMapEntry( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFrameTypeMapEntry::RFrameTypeMapEntry( YCommandID commandId, EFrameType frameType )
	: m_eFrameType( frameType ), m_CommandId( commandId )
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RFrameTypeMapEntry::GetCommandId( const EFrameType eType )
//
//  Description:		return the command id that matches the given frame type
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCommandID RFrameTypeMapEntry::GetCommandId( const EFrameType eType )
	{
	int	count	= sizeof( _FrameTypeMap ) / sizeof( _FrameTypeMap[0] );

	for( int i = 0; i < count; ++i )
		{
		if( _FrameTypeMap[i].m_eFrameType == eType )
			{
			return _FrameTypeMap[i].m_CommandId;
			}
		}
	TpsAssertAlways( "Invalid Frame Type" );
	return 0;
	}

// ****************************************************************************
//
//  Function Name:	RFrameTypeMapEntry::GetFrameType( const YCommandID m_eFrameType )
//
//  Description:		return the frame type that matches the given command id
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
EFrameType RFrameTypeMapEntry::GetFrameType( const YCommandID yID )
	{
	int	count	= sizeof( _FrameTypeMap ) / sizeof( _FrameTypeMap[0] );

	for( int i = 0; i < count; ++i )
		{
		if( _FrameTypeMap[i].m_CommandId == yID )
			{
			return _FrameTypeMap[i].m_eFrameType;
			}
		}
	TpsAssertAlways( "Invalid command id" );
	return kNoFrame;
	}


// ****************************************************************************
//
//  Function Name:	RFrame::GetFrame( const EFrameType eFrame ) const
//
//  Description:		return the frame object for the given frame type
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFrame* RFrame::GetFrame( const EFrameType eFrame, RComponentView* pView )
	{
	RFrame*	pFrame = NULL;
	switch ( eFrame )
		{
		default:
		case kNoFrame:		
			pFrame	= new RFrame;
			break;
		case kThinLineFrame:
			pFrame	= new RThinLineFrame;
			break;
		case kMediumLineFrame:
			pFrame	= new RMediumLineFrame;
			break;
		case kThickLineFrame:
			pFrame	= new RThickLineFrame;
			break;
		case kDoubleLineFrame:
			pFrame	= new RDoubleThinLineFrame;
			break;
		case kMediumDoubleLineFrame:
			pFrame	= new RDoubleMediumLineFrame;
			break;
		case kThickDoubleLineFrame:
			pFrame	= new RDoubleThickLineFrame;
			break;
		case kRoundCornerFrame:
			pFrame	= new RRoundCornerFrame;
			break;
		case kDropShadowFrame:
			pFrame	= new RDropShadowFrame;
			break;
		case kPictureFrame:
			pFrame	= new RPictureFrame;
			break;
		}

	//	If the size of the component is too small, we must shink the frame insets proportionally
	if ( pView )
		{
		RRealSize	size	= pView->GetBoundingRect().GetUntransformedSize();
		if ( (size.m_dx < kFrameSizeBaseline) || (size.m_dy < kFrameSizeBaseline) )
			{
			YRealDimension	minSize	= Min( size.m_dx, size.m_dy );
			YRealDimension	percent	= minSize / kFrameSizeBaseline;
			pFrame->m_TopLeftInset.m_dx		*= percent;
			pFrame->m_TopLeftInset.m_dy		*= percent;
			pFrame->m_BottomRightInset.m_dx	*= percent;
			pFrame->m_BottomRightInset.m_dy	*= percent;
			}
		}
	return pFrame;
	}


// ****************************************************************************
//
//  Function Name:	RFrame::RFrame( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFrame::RFrame( )
	: m_eFrameType( kNoFrame ),
	  m_TopLeftInset( 0, 0 ),
	  m_BottomRightInset( 0, 0 )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RFrame::~RFrame( )
//
//  Description:		dtor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RFrame::~RFrame( )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RFrame::GetFrameType( ) const
//
//  Description:		return the frame's type enum value
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
EFrameType RFrame::GetFrameType( ) const
	{
	return m_eFrameType;
	}

// ****************************************************************************
//
//  Function Name:	RFrame::DrawSquareEndLine( const RRealPoint& rTopLeft, const RRealPoint& rBottomRight, const R2dTransform& transform ) const
//
//  Description:		draw a line with square ends
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrame::DrawSquareEndLine( RDrawingSurface& drawingSurface, const RRealPoint& rTopLeft, const RRealPoint& rBottomRight, const R2dTransform& transform ) const
	{
	RRealRect rLineRect( rTopLeft, rBottomRight );

	// Convert the rectangle to a polygon of 4 points
	RIntPoint points[ 4 ];
	CreateRectPoly( RRealRect( rLineRect ), transform, &points[ 0 ] );

	//	make sure the output rect is not empty and that the lines which make it up
	//	will all be drawn in their entirety
	if ((points[0].m_x == points[1].m_x) || (points[0].m_x == points[2].m_x) )
		++points[0].m_x;
	
	if ((points[3].m_x == points[2].m_x) || (points[3].m_x == points[1].m_x) )
		++points[3].m_x;

	if ( (points[0].m_y == points[3].m_y) || (points[0].m_y == points[2].m_y) )
		++points[0].m_y;
	
	if( (points[1].m_y == points[2].m_y) || (points[1].m_y == points[3].m_y) )
		++points[1].m_y;

	// Draw the polygon
	drawingSurface.FillPolygon( points, 4 );
	}

// ****************************************************************************
//
//  Function Name:	RFrame::DrawSquareEndRect( RDrawingSurface& drawingSurface, const RRealRect& rRect, const R2dTransform& transform ) const
//
//  Description:		draw a rect with square ends
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrame::DrawSquareEndRect( RDrawingSurface& drawingSurface, const RRealRect& rRect, const R2dTransform& transform ) const
	{
	drawingSurface.FrameRectangle( RRealRect( rRect ), transform );
	return;

	YIntDimension yOriginalPenWidth = drawingSurface.GetPenWidth();
	RIntSize rUsePenWidth( yOriginalPenWidth, 0 );
	RIntSize rMinPenWidth( 1, 1 );
	YIntDimension& yUsePenWidth = rUsePenWidth.m_dx;
	YIntDimension& yMinPenWidth = rMinPenWidth.m_dx;
	
	//	get logical minimum and actual size of pens
	::DeviceUnitsToLogicalUnits( rUsePenWidth, drawingSurface );
	::DeviceUnitsToLogicalUnits( rMinPenWidth, drawingSurface );

	//	make sure scaled pen with will be at least 1 device pixel wide 
	yUsePenWidth = Max( yUsePenWidth, yMinPenWidth );
	
	//	use square end rect-lines for window aligned rects
	DrawSquareEndLine( drawingSurface, rRect.m_TopLeft, RRealPoint(rRect.m_Right ,rRect.m_Top + yUsePenWidth), transform );
	DrawSquareEndLine( drawingSurface, RRealPoint(rRect.m_Right - yUsePenWidth, rRect.m_Top), RRealPoint(rRect.m_Right, rRect.m_Bottom), transform );
	DrawSquareEndLine( drawingSurface, RRealPoint(rRect.m_Left, rRect.m_Bottom - yUsePenWidth), rRect.m_BottomRight, transform );
	DrawSquareEndLine( drawingSurface, RRealPoint(rRect.m_Left, rRect.m_Top), RRealPoint(rRect.m_Left + yUsePenWidth, rRect.m_Bottom), transform );

#if 0
	//	This is an in progress attempt to fix frame rect drawing.
	//	The polygon points transform poorly when rotated so this approach appears unworkable
	RSolidColor yOriginalFillColor = drawingSurface.GetFillColor();
	RSolidColor yPenColor = drawingSurface.GetPenColor();

	RIntSize rPenWidth;
	rPenWidth.m_dx = yUsePenWidth;
	rPenWidth.m_dy = yUsePenWidth;

	//	draw in a zero width pen so that we may only fill inside the given rect
	drawingSurface.SetPenWidth( 0 );
	drawingSurface.SetFillColor( yPenColor );

	//	define a polygon from the given rect
	RIntPoint		ptFrame[11];
	const YCounter yFramePoints = 11;
	RIntRect rInnerFrame( rRect );

	//	get outer part of rect
	ptFrame[0] = rRect.m_TopLeft;
	ptFrame[1] = RIntPoint( rRect.m_Right, rRect.m_Top );
	ptFrame[2] = rRect.m_BottomRight;
	ptFrame[3] = RIntPoint( rRect.m_Left, rRect.m_Bottom );
	ptFrame[4] = rRect.m_TopLeft;

	//	shrink the rect because lines draw from the center of their coords
	rInnerFrame.Inset( rPenWidth );	

	//	get the inside rect bounds
	ptFrame[5] = rInnerFrame.m_TopLeft;
	ptFrame[6] = RIntPoint( rInnerFrame.m_Left, rInnerFrame.m_Bottom );
	ptFrame[7] = rInnerFrame.m_BottomRight;
	ptFrame[8] = RIntPoint( rInnerFrame.m_Right, rInnerFrame.m_Top );
	ptFrame[9] = rInnerFrame.m_TopLeft;
	ptFrame[10] = ptFrame[0];

	// draw the frame
	TransformPoints( ptFrame, yFramePoints, transform );
	WinCode( HDC hdc = (HDC)drawingSurface.GetSurface() );
	WinCode( int nFillMode = ::SetPolyFillMode( hdc, ALTERNATE ) );
	drawingSurface.FillPolygon( ptFrame, yFramePoints );	
	WinCode( nFillMode = ::SetPolyFillMode( hdc, nFillMode ) );
	drawingSurface.SetPenWidth( yOriginalPenWidth );
	drawingSurface.SetFillColor( yOriginalFillColor );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RFrame::GetInset
//
//  Description:		Return the insets of the frame
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrame::GetInset( RRealSize& topLeft, RRealSize& bottomRight ) const
	{
	topLeft		= m_TopLeftInset;
	bottomRight	= m_BottomRightInset;
	}

// ****************************************************************************
//
//  Function Name:	RFrame::GetInsetComponentRect( YComponentBoundingRect& boundingRect )
//
//  Description:		Inset the given component rect so that the frame fits around it.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrame::GetInsetComponentRect( YComponentBoundingRect& componentBoundingRect )
	{
	RRealSize topLeftInset;
	RRealSize bottomRightInset;
	GetInset( topLeftInset, bottomRightInset );

	componentBoundingRect.Inset( topLeftInset, bottomRightInset );
	}

// ****************************************************************************
//
//  Function Name:	RFrame::GetDeviceUnit( drawingSurface, YIntDimension ydim ) const
//
//  Description:		return ydim converted to device coords
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YRealDimension RFrame::GetDeviceUnit( const R2dTransform& transform, const YRealDimension& ydim ) const
	{
	YRealDimension	rotation, xScale, yScale;
	transform.Decompose( rotation, xScale, yScale );
	//	prevent zero thickness lines from being drawn since the frame would disappear
	YRealDimension	minValue	= ::Min( ydim * xScale, ydim * yScale );
	return ::Round( Max( (YRealDimension)1.0, minValue ) );
	}

// ****************************************************************************
//
//  Function Name:	RFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrame::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender
						  , const RSolidColor& /*rFrameColor*/, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	drawingSurface.SetForegroundMode( kNormal );
	drawingSurface.RestoreDefaults( );
	
	//	Fill the background.
	// Dont bother if we are in coloring book mode or we filling the frame.
	if( !drawingSurface.GetColoringBookMode( ) && (eDrawType & kFrameFillInside) )
		{
		//	Prep the color for use if its a gradient.
		//	Don't include the frame inset in the output rect since it was already included  
		// by our caller.
		rBackgroundColor *= transform;
		rBackgroundColor.SetBoundingRect( rcRender );
		drawingSurface.SetFillColor( rBackgroundColor );		

		// Render
		drawingSurface.FillRectangle( RRealRect( rcRender ), transform );
		}
	}

// ****************************************************************************
//
//  Function Name:	RSingleLineFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSingleLineFrame::DrawSingleLineFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
											const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	drawingSurface.SetForegroundMode( kNormal );
	drawingSurface.RestoreDefaults( );

	// Get the pen width
	YPenWidth penWidth = GetDeviceUnit( transform, m_TopLeftInset.m_dx );

	// Get the inside rect, compensating for the line thickness
	RRealRect rcRenderInside = rcRender;
	rcRenderInside.Inset( RRealSize( m_TopLeftInset.m_dx/2.0, m_TopLeftInset.m_dy/2.0 ) );
	
	//	Fill the background.
	// Dont bother if we are in coloring book mode or we aren't filling the frame.
	if( !drawingSurface.GetColoringBookMode( ) && (eDrawType & kFrameFillInside) )
		{
		//	Prep the color for use if its a gradient.
		//	Don't include the frame inset in the output rect since it was already included  
		// by our caller.
		rBackgroundColor *= transform;
		rBackgroundColor.SetBoundingRect( rcRender );
		drawingSurface.SetFillColor( rBackgroundColor );		

		// Render
		drawingSurface.FillRectangle( rcRenderInside, transform );
		}
	
	//	Draw the outside of the frame.
	if ( eDrawType & kFrameDrawOutside )
		{
		//	Draw the frame.
		drawingSurface.SetPenColor( rFrameColor );
		drawingSurface.SetFillColor( rFrameColor );

		// draw the frame
		drawingSurface.SetPenWidth( penWidth );
		DrawSquareEndRect( drawingSurface, rcRenderInside, transform );
		}
	}

// ****************************************************************************
//
//  Function Name:	RThinLineFrame::RThinLineFrame( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RThinLineFrame::RThinLineFrame( )
	: RSingleLineFrame( )
	{
	m_eFrameType			= kThinLineFrame;
	m_TopLeftInset.m_dx	= kFrameThinLineSize;
	m_TopLeftInset.m_dy	= m_TopLeftInset.m_dx;
	m_BottomRightInset	= m_TopLeftInset;
	}

// ****************************************************************************
//
//  Function Name:	RThinLineFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RThinLineFrame::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender
									 , const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	DrawSingleLineFrame( drawingSurface, transform, rcRender, rFrameColor, rBackgroundColor, eDrawType );
	}


// ****************************************************************************
//
//  Function Name:	RMediumLineFrame::RMediumLineFrame( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMediumLineFrame::RMediumLineFrame( )
	: RSingleLineFrame( )
	{
	m_eFrameType			= kMediumLineFrame;
	m_TopLeftInset.m_dx	= kFrameMediumLineSize;
	m_TopLeftInset.m_dy	= m_TopLeftInset.m_dx;
	m_BottomRightInset	= m_TopLeftInset;
	}

// ****************************************************************************
//
//  Function Name:	RMediumLineFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMediumLineFrame::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform
										, const RIntRect& rcRender, const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	DrawSingleLineFrame( drawingSurface, transform, rcRender, rFrameColor, rBackgroundColor, eDrawType );
	}



// ****************************************************************************
//
//  Function Name:	RThickLineFrame::RThickLineFrame( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RThickLineFrame::RThickLineFrame( )
	: RSingleLineFrame( )
	{
	m_eFrameType			= kThickLineFrame;
	m_TopLeftInset.m_dx	= kFrameThickLineSize;
	m_TopLeftInset.m_dy	= m_TopLeftInset.m_dx;
	m_BottomRightInset	= m_TopLeftInset;
	}

// ****************************************************************************
//
//  Function Name:	RThickLineFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RThickLineFrame::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender
									  , const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	DrawSingleLineFrame( drawingSurface, transform, rcRender, rFrameColor, rBackgroundColor, eDrawType );
	}

// ****************************************************************************
//
//  Function Name:	RDoubleLineFrame::DrawDoubleLineFrame( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDoubleLineFrame::DrawDoubleLineFrame( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
										const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	drawingSurface.SetForegroundMode( kNormal );
	drawingSurface.RestoreDefaults( );

	// Get the pen width (1/5th) total inset
	YPenWidth	logicalPenWidth	= m_TopLeftInset.m_dx / m_LineToGapRatio;
	YPenWidth	penWidth				= GetDeviceUnit( transform, logicalPenWidth );

	// Get the inside rect, compensating for the line thickness
	RRealRect rcRenderInside = rcRender;
	rcRenderInside.Inset( RRealSize( logicalPenWidth/2.0, logicalPenWidth/2.0 ) );
	
	//	Fill the background.
	// Dont bother if we are in coloring book mode or we arent filling the frame.
	if( !drawingSurface.GetColoringBookMode( ) && (eDrawType & kFrameFillInside)  )
		{
		//	Prep the color for use if its a gradient.
		//	Don't include the frame inset in the output rect since it was already included  
		// by our caller.
		rBackgroundColor *= transform;
		rBackgroundColor.SetBoundingRect( rcRender );
		drawingSurface.SetFillColor( rBackgroundColor );		

		// Render
		drawingSurface.FillRectangle( rcRenderInside, transform );
		}
	
	//	We're done if we aren't drawing the outside of the frame.
	if ( !(eDrawType & kFrameDrawOutside) )
		return;
	
	//	Draw the frame.
	drawingSurface.SetPenColor( rFrameColor );
	drawingSurface.SetFillColor( rFrameColor );

	// draw the frame
	drawingSurface.SetPenWidth( penWidth );
	drawingSurface.FrameRectangle( RRealRect( rcRenderInside ), transform );
		//compute gap amount
	YRealDimension	gapAmount	= m_TopLeftInset.m_dx - logicalPenWidth;
	rcRenderInside.Inset( RRealSize(gapAmount, gapAmount) );
	
	drawingSurface.FrameRectangle( RRealRect( rcRenderInside ), transform );
	}

// ****************************************************************************
//
//  Function Name:	RDoubleThinLineFrame::RDoubleThinLineFrame( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDoubleThinLineFrame::RDoubleThinLineFrame( )
	: RDoubleLineFrame( )
	{
	m_eFrameType			= kDoubleLineFrame;
	m_TopLeftInset.m_dx	= kFrameDoubleInnerLineSize + kFrameDoubleInterLineSize + kFrameDoubleOuterLineSize;
	m_TopLeftInset.m_dy	= m_TopLeftInset.m_dx;
	m_BottomRightInset	= m_TopLeftInset;
	m_LineToGapRatio		= m_TopLeftInset.m_dx / kFrameDoubleInnerLineSize;
	}


// ****************************************************************************
//
//  Function Name:	RDoubleThinLineFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDoubleThinLineFrame::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender
										, const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	DrawDoubleLineFrame( drawingSurface, transform, rcRender, rFrameColor, rBackgroundColor, eDrawType );
	}


// ****************************************************************************
//
//  Function Name:	RDoubleMediumLineFrame::RDoubleMediumLineFrame( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDoubleMediumLineFrame::RDoubleMediumLineFrame( )
	: RDoubleLineFrame( )
	{
	m_eFrameType			= kMediumDoubleLineFrame;
	m_TopLeftInset.m_dx	= kFrameDoubleInnerMediumLineSize + kFrameDoubleInterMediumLineSize + kFrameDoubleOuterMediumLineSize;
	m_TopLeftInset.m_dy	= m_TopLeftInset.m_dx;
	m_BottomRightInset	= m_TopLeftInset;
	m_LineToGapRatio		= m_TopLeftInset.m_dx / kFrameDoubleInnerMediumLineSize;
	}

// ****************************************************************************
//
//  Function Name:	RDoubleMediumLineFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDoubleMediumLineFrame::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender
												, const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	DrawDoubleLineFrame( drawingSurface, transform, rcRender, rFrameColor, rBackgroundColor, eDrawType );
	}


// ****************************************************************************
//
//  Function Name:	RDoubleThickLineFrame::RDoubleThickLineFrame( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDoubleThickLineFrame::RDoubleThickLineFrame( )
	: RDoubleLineFrame( )
	{
	m_eFrameType			= kThickDoubleLineFrame;
	m_TopLeftInset.m_dx	= kFrameDoubleInnerThickLineSize + kFrameDoubleInterThickLineSize + kFrameDoubleOuterThickLineSize;
	m_TopLeftInset.m_dy	= m_TopLeftInset.m_dx;
	m_BottomRightInset	= m_TopLeftInset;
	m_LineToGapRatio		= m_TopLeftInset.m_dx / kFrameDoubleInnerThickLineSize;
	}

// ****************************************************************************
//
//  Function Name:	RDoubleThickLineFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDoubleThickLineFrame::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender
											  , const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	DrawDoubleLineFrame( drawingSurface, transform, rcRender, rFrameColor, rBackgroundColor, eDrawType );
	}



// ****************************************************************************
//
//  Function Name:	RRoundCornerFrame::RRoundCornerFrame( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRoundCornerFrame::RRoundCornerFrame( )
	: RFrame( ),
	  m_WidthToRadiusRatio( 1.0 )
	{
	m_eFrameType			= kRoundCornerFrame;
	m_TopLeftInset.m_dx	= kFrameRoundCornerRadius / 2 + kFrameRoundCornerLineSize;
	m_TopLeftInset.m_dy	= m_TopLeftInset.m_dx;
	m_BottomRightInset	= m_TopLeftInset;
	m_WidthToRadiusRatio	= m_TopLeftInset.m_dx / kFrameRoundCornerRadius;
	}

// ****************************************************************************
//
//  Function Name:	RRoundCornerFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RRoundCornerFrame::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender,
											const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	drawingSurface.SetForegroundMode( kNormal );
	drawingSurface.RestoreDefaults( );

	YRealDimension	radius		= (m_TopLeftInset.m_dx * m_WidthToRadiusRatio);

	// Get the pen width
	YPenWidth	logicalWidth	= m_TopLeftInset.m_dx - radius;
	YPenWidth	penWidth			= GetDeviceUnit( transform, logicalWidth );

	// Get the inside rect, compensating for the line thickness
	RRealRect rcRenderInside = rcRender;
	rcRenderInside.Inset( RRealSize(logicalWidth/2, logicalWidth/2) );

#if 0
	const YCounter yFramePoints = 37;
	RRealPoint ptFrame[yFramePoints];
	
	//	get the start of the corners as an offset; stretch it out by doubling so that the curve is clearer without
	//	expanding the interior offset
	RRealSize rShortOffset( 2 * radius, 2 * radius );
	RRealSize rLongOffset( rcRenderInside.Width() - rShortOffset.m_dx, rcRenderInside.Height() - rShortOffset.m_dy );
	R2dTransform rCornerPositionTransform;

	//	fill in the frame's control points
	//	get the left wall
	midpoint( ptFrame[2],	rcRenderInside.m_TopLeft,	RRealPoint(rcRenderInside.m_Left, rcRenderInside.m_Bottom) );
	ptFrame[1] = ptFrame[3] = ptFrame[2];
	ptFrame[1].m_y = rcRenderInside.m_Top + rLongOffset.m_dy;
	ptFrame[3].m_y = rcRenderInside.m_Top + rShortOffset.m_dy;
	ptFrame[4] = ptFrame[3];
	ptFrame[0] = ptFrame[1];
	
	//	get the top wall
	midpoint( ptFrame[11],	rcRenderInside.m_TopLeft,	RRealPoint(rcRenderInside.m_Right, rcRenderInside.m_Top) );
	ptFrame[10] = ptFrame[12] = ptFrame[11];
	ptFrame[10].m_x = rcRenderInside.m_Left + rShortOffset.m_dx;
	ptFrame[12].m_x = rcRenderInside.m_Left + rLongOffset.m_dx;
	ptFrame[9] = ptFrame[10];
	ptFrame[13] = ptFrame[12];

	//	get the right wall
	midpoint( ptFrame[20],	RRealPoint(rcRenderInside.m_Right, rcRenderInside.m_Top),	rcRenderInside.m_BottomRight );
	ptFrame[19] = ptFrame[21] = ptFrame[20];
	ptFrame[19].m_y = rcRenderInside.m_Top + rShortOffset.m_dy;
	ptFrame[21].m_y = rcRenderInside.m_Top + rLongOffset.m_dy;
	ptFrame[18] = ptFrame[19];
	ptFrame[22] = ptFrame[21];

	//	get the bottom wall
	midpoint( ptFrame[29],	RRealPoint(rcRenderInside.m_Left, rcRenderInside.m_Bottom),	rcRenderInside.m_BottomRight );
	ptFrame[28] = ptFrame[30] = ptFrame[29];
	ptFrame[28].m_x = rcRenderInside.m_Left + rLongOffset.m_dx;
	ptFrame[30].m_x = rcRenderInside.m_Left + rShortOffset.m_dx;
	ptFrame[27] = ptFrame[28];
	ptFrame[31] = ptFrame[30];

	//	connect the corners
	ptFrame[35] = ptFrame[36] = ptFrame[0];

	//	get the corners 
	RRealPoint rTempPt;
	RRealPoint rTempPt2;
	RRealPoint rTempPt3;
	
	//	get the top left corner
	midpoint( rTempPt2,		ptFrame[4],		ptFrame[9] );
	midpoint( rTempPt3,		rcRenderInside.m_TopLeft,	rTempPt2 );
	midpoint( rTempPt,		rTempPt3,		rcRenderInside.m_TopLeft );
	midpoint( ptFrame[5],	ptFrame[4],		rTempPt );
	midpoint( ptFrame[8],	rTempPt,			ptFrame[9] );
	
	midpoint( rTempPt,		ptFrame[8],		rcRenderInside.m_TopLeft );
	midpoint( ptFrame[6],	ptFrame[5],		rTempPt );
	midpoint( rTempPt,		ptFrame[5],		rcRenderInside.m_TopLeft );
	midpoint( ptFrame[7],	ptFrame[8],		rTempPt );

	//	get the top right corner
	midpoint( rTempPt2,		ptFrame[13],	ptFrame[18] );
	midpoint( rTempPt3,		rTempPt2,		RRealPoint(rcRenderInside.m_Right, rcRenderInside.m_Top) );
	midpoint( rTempPt,		rTempPt3,		RRealPoint(rcRenderInside.m_Right, rcRenderInside.m_Top) );
	midpoint( ptFrame[14],	ptFrame[13],	rTempPt );
	midpoint( ptFrame[17],	rTempPt,			ptFrame[18] );
	
	midpoint( rTempPt,		ptFrame[17],	RRealPoint(rcRenderInside.m_Right, rcRenderInside.m_Top) );
	midpoint( ptFrame[15],	ptFrame[14],	rTempPt );
	midpoint( rTempPt,		ptFrame[14],	RRealPoint(rcRenderInside.m_Right, rcRenderInside.m_Top) );
	midpoint( ptFrame[16],	ptFrame[17],	rTempPt );
	
	//	get the bottom right corner
	midpoint( rTempPt2,		ptFrame[22],	ptFrame[27] );
	midpoint( rTempPt3,		rTempPt2,		rcRenderInside.m_BottomRight );
	midpoint( rTempPt,		rTempPt3,		rcRenderInside.m_BottomRight );
	midpoint( ptFrame[23],	rTempPt,			ptFrame[22] );
	midpoint( ptFrame[26],	ptFrame[27],	rTempPt );

	midpoint( rTempPt,		ptFrame[27],	rcRenderInside.m_BottomRight );
	midpoint( ptFrame[24],	rTempPt,			ptFrame[23] );
	midpoint( rTempPt,		ptFrame[22],	rcRenderInside.m_BottomRight );
	midpoint( ptFrame[25],	rTempPt,			ptFrame[26] );

	//	get the bottom left corner
	midpoint( rTempPt2,		ptFrame[31],	ptFrame[0] );
	midpoint( rTempPt3,		RRealPoint(rcRenderInside.m_Left, rcRenderInside.m_Bottom),	rTempPt2 );
	midpoint( rTempPt,		RRealPoint(rcRenderInside.m_Left, rcRenderInside.m_Bottom),	rTempPt3 );
	midpoint( ptFrame[32],	rTempPt,			ptFrame[31] );
	midpoint( ptFrame[35],	ptFrame[0],		rTempPt );

	midpoint( rTempPt,		RRealPoint(rcRenderInside.m_Left, rcRenderInside.m_Bottom),	ptFrame[35] );
	midpoint( ptFrame[33],	rTempPt,			ptFrame[32] );
	midpoint( rTempPt,		RRealPoint(rcRenderInside.m_Left, rcRenderInside.m_Bottom),	ptFrame[32] );
	midpoint( ptFrame[34],	rTempPt,			ptFrame[35] );

	// Transform the frame.
	RIntPoint ptFrameTransformed[yFramePoints];
	TransformPoints( ptFrame, ptFrameTransformed, yFramePoints, transform );

	//	Fill the background.
	// Dont bother if we are in coloring book mode or we arent filling the frame.
	if( !drawingSurface.GetColoringBookMode( ) && (eDrawType & kFrameFillInside)  )
		{
		//	Prep the color for use if its a gradient.
		//	Don't include the frame inset in the output rect since it was already included  
		// by our caller.
		rBackgroundColor *= transform;
		rBackgroundColor.SetBoundingRect( rcRender );
		drawingSurface.SetFillColor( rBackgroundColor );		

		// Render
		drawingSurface.FillPolyBezier( ptFrameTransformed, yFramePoints );
		}
	
	if ( eDrawType & kFrameDrawOutside )
		{
		//	Draw the frame.
		drawingSurface.SetPenColor( rFrameColor );
		drawingSurface.SetPenWidth( penWidth );
		drawingSurface.FramePolyBezier( ptFrameTransformed, yFramePoints );
		}
#else
	EPathOperatorArray	operators;
	RIntPointArray			points;

	//	Top Line
	operators.InsertAtEnd( kMoveTo );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Left + radius, rcRenderInside.m_Top ) );
	operators.InsertAtEnd( kLine );
	operators.InsertAtEnd( (EPathOperator)1 );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Right - radius, rcRenderInside.m_Top ) );
	//	Top Right Round Corner
	operators.InsertAtEnd( kQuadraticSpline );
	operators.InsertAtEnd( (EPathOperator)1 );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Right, rcRenderInside.m_Top ) );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Right, rcRenderInside.m_Top + radius ) );
	//	Right Line
	operators.InsertAtEnd( kLine );
	operators.InsertAtEnd( (EPathOperator)1 );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Right, rcRenderInside.m_Bottom - radius ) );
	//	Bottom Right Round Corner
	operators.InsertAtEnd( kQuadraticSpline );
	operators.InsertAtEnd( (EPathOperator)1 );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Right, rcRenderInside.m_Bottom ) );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Right - radius, rcRenderInside.m_Bottom ) );
	//	Bottom Line
	operators.InsertAtEnd( kLine );
	operators.InsertAtEnd( (EPathOperator)1 );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Left + radius, rcRenderInside.m_Bottom ) );
	//	Bottom Left Round Corner
	operators.InsertAtEnd( kQuadraticSpline );
	operators.InsertAtEnd( (EPathOperator)1 );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Left, rcRenderInside.m_Bottom ) );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Left, rcRenderInside.m_Bottom - radius ) );
	//	Left Line
	operators.InsertAtEnd( kLine );
	operators.InsertAtEnd( (EPathOperator)1 );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Left, rcRenderInside.m_Top + radius ) );
	//	Top Left Round Corner
	operators.InsertAtEnd( kQuadraticSpline );
	operators.InsertAtEnd( (EPathOperator)1 );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Left, rcRenderInside.m_Top ) );
	points.InsertAtEnd( RRealPoint( rcRenderInside.m_Left + radius, rcRenderInside.m_Top ) );
	//	Done
	operators.InsertAtEnd( kClose );
	operators.InsertAtEnd( kEnd );

	RPath	path;
	path.Define( operators, points );

	//	Fill the background.
	// Dont bother if we are in coloring book mode or we arent filling the frame.
	if( !drawingSurface.GetColoringBookMode( ) && (eDrawType & kFrameFillInside)  )
		{
		//	Prep the color for use if its a gradient.
		//	Don't include the frame inset in the output rect since it was already included  
		// by our caller.
		rBackgroundColor *= transform;
		rBackgroundColor.SetBoundingRect( rcRender );
		drawingSurface.SetFillColor( rBackgroundColor );		

		// Render
		path.Render( drawingSurface, transform, rcRender, kFilled );
		}
	
	if ( eDrawType & kFrameDrawOutside )
		{
		//	Draw the frame.
		drawingSurface.SetPenColor( rFrameColor );
		drawingSurface.SetPenWidth( penWidth );
		path.Render( drawingSurface, transform, rcRender, kStroked );
		}

#endif	// 0
	}


// ****************************************************************************
//
//  Function Name:	RDropShadowFrame::RDropShadowFrame( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDropShadowFrame::RDropShadowFrame( )
	: RFrame( )
	{
	m_eFrameType = kDropShadowFrame;
	m_TopLeftInset.m_dx		= kFrameDropShadowInnerLineSize;
	m_TopLeftInset.m_dy		= m_TopLeftInset.m_dx;
	m_BottomRightInset.m_dx	= kFrameDropShadowInnerLineSize + kFrameDropShadowOuterLineSize;
	m_BottomRightInset.m_dy	= m_BottomRightInset.m_dx;
	}

// ****************************************************************************
//
//  Function Name:	RDropShadowFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDropShadowFrame::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender
										, const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	drawingSurface.SetForegroundMode( kNormal );
	drawingSurface.RestoreDefaults( );

	// Get the pen width
	YPenWidth penWidth = GetDeviceUnit( transform, m_TopLeftInset.m_dx );

	// Get the inside rect, compensating for the line thickness
	RRealSize  temp( m_TopLeftInset.m_dx/2, m_TopLeftInset.m_dx/2 );
	RRealPoint rLineThicknessCompensation( temp.m_dx, temp.m_dy );
	
	RRealRect		rcRenderTemp	= rcRender;
	YRealDimension	outsideWidth	= m_BottomRightInset.m_dx - m_TopLeftInset.m_dx;
	RRealSize		rUseSize( outsideWidth, outsideWidth );
	rcRenderTemp.Inset( rUseSize );
	rcRenderTemp.AddPointToRect( RRealPoint( rcRender.m_Left, rcRender.m_Top ) + rLineThicknessCompensation );
	drawingSurface.SetPenWidth( GetDeviceUnit( transform, m_TopLeftInset.m_dx ) );
	
	//	Fill the background.
	// Dont bother if we are in coloring book mode or we arent filling the frame.
	if( !drawingSurface.GetColoringBookMode( ) && (eDrawType & kFrameFillInside)  )
		{
		//	Prep the color for use if its a gradient.
		//	Don't include the frame inset in the output rect since it was already included  
		// by our caller.
		rBackgroundColor *= transform;
		rBackgroundColor.SetBoundingRect( rcRender );
		drawingSurface.SetFillColor( rBackgroundColor );		

		// Render
		drawingSurface.FillRectangle( rcRenderTemp, transform );
		}
	
	//	We're done if we aren't drawing the outside of the frame.
	if ( !(eDrawType & kFrameDrawOutside) )
		return;
	
	//	Draw the frame.
	drawingSurface.SetPenColor( rFrameColor );
	drawingSurface.SetFillColor( rFrameColor );
	drawingSurface.FrameRectangle( RRealRect( rcRenderTemp ), transform );

	//	draw the shadow
	drawingSurface.SetPenWidth( GetDeviceUnit( transform, outsideWidth) );
	rLineThicknessCompensation = RRealPoint( outsideWidth/2.0, outsideWidth/ 2.0 );
	RRealPoint rNegativeLineThicknessCompensation = RRealPoint( -outsideWidth / 2.0, outsideWidth / 2.0 );

	//	draw bottom border
	DrawSquareEndLine( drawingSurface, RRealPoint(rcRenderTemp.m_Left + outsideWidth, rcRenderTemp.m_Bottom),
								RRealPoint(rcRender.m_Right, rcRender.m_Bottom), transform );
	
	//	drop right border
	DrawSquareEndLine( drawingSurface, RRealPoint(rcRenderTemp.m_Right, rcRenderTemp.m_Top + m_BottomRightInset.m_dx - m_TopLeftInset.m_dx),
								RRealPoint(rcRender.m_Right, rcRender.m_Bottom), transform );
	}


// ****************************************************************************
//
//  Function Name:	RPictureFrame::RPictureFrame( )
//
//  Description:		ctor
//
//  Returns:			none
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPictureFrame::RPictureFrame( )
	: RFrame( )
	{
	m_eFrameType				= kPictureFrame;
	m_TopLeftInset.m_dx	= kFramePictureTabWidth / 2.0;
	m_TopLeftInset.m_dy	= m_TopLeftInset.m_dx;
	m_BottomRightInset	= m_TopLeftInset;
	}

// ****************************************************************************
//
//  Function Name:	RPictureFrame::Render( )
//
//  Description:		draw the frame 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPictureFrame::Render( RDrawingSurface& drawingSurface, const R2dTransform& transform, const RIntRect& rcRender
									, const RSolidColor& rFrameColor, RColor& rBackgroundColor, const EFrameDrawType eDrawType /*= kFrameAndFill*/ ) const
	{
	drawingSurface.SetForegroundMode( kNormal );
	drawingSurface.RestoreDefaults( );

	// Get the pen width
	YPenWidth	logicalWidth	= m_TopLeftInset.m_dx * 2 * kFramePictureLineSize / kFramePictureTabWidth;
	YPenWidth	penWidth			= GetDeviceUnit( transform, logicalWidth );

	// Get the inside rect, compensating for the line thickness
	RRealRect rcRenderInside = rcRender;
	rcRenderInside.Inset( RRealSize( logicalWidth/2.0, logicalWidth/2.0) );
	
	//	Fill the background.
	// Dont bother if we are in coloring book mode or we arent filling the frame.
	if( !drawingSurface.GetColoringBookMode( ) && (eDrawType & kFrameFillInside)  )
		{
		//	Prep the color for use if its a gradient.
		//	Don't include the frame inset in the output rect since it was already included  
		// by our caller.
		rBackgroundColor *= transform;
		rBackgroundColor.SetBoundingRect( rcRender );
		drawingSurface.SetFillColor( rBackgroundColor );		

		// Render
		RRealRect	insideRect	= rcRender;
		insideRect.Inset( m_TopLeftInset );
		drawingSurface.FillRectangle( insideRect, transform );
		}
	
	//	We're done if we aren't drawing the outside of the frame.
	if ( !(eDrawType & kFrameDrawOutside) )
		return;
	
	//	Draw the frame.
	drawingSurface.SetPenColor( rFrameColor );
	drawingSurface.SetPenWidth( penWidth );
	drawingSurface.SetFillColor( rFrameColor );

	YRealDimension	pictureTabWidth	= m_TopLeftInset.m_dx * 2;
	YRealDimension	pictureTabLength	= pictureTabWidth * kFramePictureTabLength / kFramePictureTabWidth;

	// define the frames' polygon as an array of points drawn in the following order:
	//	0,6------------1
	//	|				  /
	//	|	3--------2
	//	|	|
	//	|	|
	//	|	|
	//	|	4
	// | /
	//	5
	//
	RRealPoint pPointsTopLeft[ kNumPictureCornerVertices ] = 
		{ RRealPoint(rcRenderInside.m_Left, rcRenderInside.m_Top )
		, RRealPoint(rcRenderInside.m_Left + pictureTabLength + pictureTabWidth, rcRenderInside.m_Top)
		, RRealPoint(rcRenderInside.m_Left + pictureTabLength, rcRenderInside.m_Top + pictureTabWidth )
		, RRealPoint(rcRenderInside.m_Left + pictureTabWidth, rcRenderInside.m_Top + pictureTabWidth )
		, RRealPoint(rcRenderInside.m_Left + pictureTabWidth, rcRenderInside.m_Top + pictureTabLength )
		, RRealPoint(rcRenderInside.m_Left, rcRenderInside.m_Top + pictureTabLength + pictureTabWidth )
		, RRealPoint(rcRenderInside.m_Left, rcRenderInside.m_Top)
		};
	
	//	copy the top left frame so that we may transform the corners independently
	RRealPoint pPointsTopRight[ kNumPictureCornerVertices ];
	RRealPoint pPointsBottomRight[ kNumPictureCornerVertices ];
	RRealPoint pPointsBottomLeft[ kNumPictureCornerVertices ];
	for( YPointCount i = 0; i < kNumPictureCornerVertices; ++i )
		{
		pPointsTopRight[i] = pPointsTopLeft[i];
		pPointsBottomRight[i] = pPointsTopLeft[i];
		pPointsBottomLeft[i] = pPointsTopLeft[i];
		}

	RIntPoint pTransformedPoints[ kNumPictureCornerVertices ];
	RRealPoint centerPoint = rcRenderInside.GetCenterPoint( );

	//	draw the other corners by rotating the first corner
	R2dTransform tempTransform;

	//	draw the top left
	TransformPoints( &pPointsTopLeft[ 0 ], &pTransformedPoints[ 0 ], kNumPictureCornerVertices, transform );
	drawingSurface.FillPolygon( pTransformedPoints, kNumPictureCornerVertices );

	//	draw the top right
	tempTransform = transform;
	tempTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, -1, 1 );
	TransformPoints( &pPointsTopRight[ 0 ], &pTransformedPoints[ 0 ], kNumPictureCornerVertices, const_cast<const R2dTransform&>( tempTransform ) );
	drawingSurface.FillPolygon( pTransformedPoints, kNumPictureCornerVertices );

	//	draw the bottom right
	tempTransform = transform;
	tempTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, -1, -1 );
	TransformPoints( &pPointsBottomRight[ 0 ], &pTransformedPoints[ 0 ], kNumPictureCornerVertices, const_cast<const R2dTransform&>( tempTransform ) );
	drawingSurface.FillPolygon( pTransformedPoints, kNumPictureCornerVertices );

	//	draw the bottom left
	tempTransform = transform;
	tempTransform.PreScaleAboutPoint( centerPoint.m_x, centerPoint.m_y, 1, -1 );
	TransformPoints( &pPointsBottomLeft[ 0 ], &pTransformedPoints[ 0 ], kNumPictureCornerVertices, const_cast<const R2dTransform&>( tempTransform ) );
	drawingSurface.FillPolygon( pTransformedPoints, kNumPictureCornerVertices );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RFrame::Validate( )
//
//  Description:		Validate the object.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RFrame::Validate( ) const
	{
	TpsAssertIsObject( RFrame, this );
	}

#endif	//	TPSDEBUG
