// ****************************************************************************
//
//  File Name:			DrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas & M. Houle
//
//  Description:		Definition of the RDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Source/DrawingSurface.cpp                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "DrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifdef IMAGER

	#include "GlobalBuffer.h"

	inline YPercentage 	GetGradientDisplayPrecision()	{ return( 0.98 ); }
	inline YPercentage 	GetGradientPrintPrecision() 	{ return( 1.0 ); }
	inline YIntDimension	GetMaxGradientDPI() 				{ return( 72 ); }

#else

	#include "ApplicationGlobals.h"

#endif

#include	"Bezier.h"
#include "QuadraticSpline.h"
#include	"GradientFill.h"
#include "ImageColor.h"
#include "Utilities.h"
#include	"ClipPolygon.h"
#include "OffscreenDrawingSurface.h"
#include "PrinterDrawingSurface.h"
#include "Configuration.h"
#include "ScratchBitmapImage.h"
#include "Path.h"
#include "View.h"

#ifdef		TPSDEBUG
#include	"DrawingSurface.inl"
#endif	// TPSDEBUG


BOOLEAN _DrawCharactersAsGlyphs( RDrawingSurface& ds, const RIntPoint& topLeft, YCounter count, LPCUBYTE pszText );

// ****************************************************************************
//
//  Class Name:		RPushFillColor
//
//  Description:		Class to save the state of the fill color, apply a
//							transform, and put it back.
//
// ****************************************************************************
//
class RPushFillColor
	{
	// Construction & destruction
	public :
								RPushFillColor( const RColor& color,
													 const R2dTransform& transform,
													 RDrawingSurface* pDrawingSurface );

								~RPushFillColor( );

	// Members
	private :
		RColor				m_OldColor;
		RDrawingSurface*	m_pDrawingSurface;
	};

// ****************************************************************************
//
//  Function Name:	RPushFillColor::RPushFillColor( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPushFillColor::RPushFillColor( const RColor& color,
										  const R2dTransform& transform,
										  RDrawingSurface* pDrawingSurface )
   : m_OldColor( pDrawingSurface->GetFillColor( ) ),
	  m_pDrawingSurface( pDrawingSurface )
	{
	// Set the new transformed fill color
	m_pDrawingSurface->SetFillColor( color, transform );
	}

// ****************************************************************************
//
//  Function Name:	RPushFillColor::~RPushFillColor( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPushFillColor::~RPushFillColor( )
	{
	// Reset the old fill color
	m_pDrawingSurface->SetFillColor( m_OldColor );
	}

// ****************************************************************************
//
//  Class Name:		RPushPenColor
//
//  Description:		Class to save the state of the fill color, apply a
//							transform, and put it back.
//
// ****************************************************************************
//
class RPushPenColor
	{
	// Construction & destruction
	public :
								RPushPenColor( const RColor& color,
													const R2dTransform& transform,
													RDrawingSurface* pDrawingSurface );

								~RPushPenColor( );

	// Members
	private :
		RColor				m_OldColor;
		RDrawingSurface*	m_pDrawingSurface;
	};

// ****************************************************************************
//
//  Function Name:	RPushPenColor::RPushPenColor( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPushPenColor::RPushPenColor( const RColor& color,
										const R2dTransform& transform,
										RDrawingSurface* pDrawingSurface )
   : m_OldColor( pDrawingSurface->GetPenColor( ) ),
	  m_pDrawingSurface( pDrawingSurface )
	{
	// Set the new transformed fill color
	m_pDrawingSurface->SetPenColor( color, transform );
	}

// ****************************************************************************
//
//  Function Name:	RPushPenColor::~RPushPenColor( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPushPenColor::~RPushPenColor( )
	{
	// Reset the old fill color
	m_pDrawingSurface->SetPenColor( m_OldColor );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::LineTo( )
//
//  Description:		Draws a line using the current pen attributes from the
//							current drawing position to the given point. The drawing
//							position is updated.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::LineTo( const RIntPoint& point )
	{
	CreatePen( );

	RIntPoint	ptEnd( point );
	uWORD			uwClip	= 0;
	BOOLEAN		bVisible	= TRUE;

	if( m_uwCurrentPointClip || IsPoint32(GetCurrentPoint()) || IsPoint32(ptEnd) )
		{
		RIntPoint	ptStart( GetCurrentPoint() );
		RIntRect		rcClip( GetClipRect() );
		uwClip		= GetPointClipEdges( ptEnd, rcClip );
		bVisible		= DoClipLineSegment( ptStart, m_uwCurrentPointClip, ptEnd, uwClip, rcClip );
		if( bVisible && m_uwCurrentPointClip != 0 )
			{
			//
			// it's visible, but the start point was initially clipped, so we need
			// to start with a moveto...
			MoveTo( ptStart );
			}
		}
	//
	// Draw the line...
	if( bVisible )
		SolidLineTo( ptEnd );
	//
	// Set the new current point and clip edges...
	SetCurrentPoint( point, uwClip );
	}

// ****************************************************************************
//
//  Function Name:	Draw3dRect::LineTo( )
//
//  Description:		Draws a rectangle with the top and left sides in 
//                   the color specified by crTopLeft and the bottom 
//                   and right sides in the color specified by 
//                   crBottomRight.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RDrawingSurface::Draw3dRect( RIntRect& rect, const RSolidColor& crTopLeft, const RSolidColor& crBottomRight ) 
{
	RColor crOldColor = GetPenColor() ;

	SetPenColor( crTopLeft ) ;
	MoveTo( RIntPoint( rect.m_Left, rect.m_Bottom ) ) ;
	LineTo( RIntPoint( rect.m_Left, rect.m_Top ) ) ;
	LineTo( RIntPoint( rect.m_Right, rect.m_Top ) ) ;

	SetPenColor( crBottomRight ) ;
	LineTo( RIntPoint( rect.m_Right, rect.m_Bottom ) ) ;
	LineTo( RIntPoint( rect.m_Left, rect.m_Bottom ) ) ;

	SetPenColor( crOldColor ) ;
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::Draw3dRect( )
//
//  Description:		Draws a rectangle with the top and left sides in 
//                   the color specified by crTopLeft and the bottom 
//                   and right sides in the color specified by 
//                   crBottomRight.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RDrawingSurface::Draw3dRect( int x, int y, int cx, int cy, const RSolidColor& crTopLeft, const RSolidColor& crBottomRight )
{
	RIntRect rect( x, y, x + cx, y + cy ) ;
	Draw3dRect( rect, crTopLeft, crBottomRight ) ;
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::Polyline( )
//
//  Description:		Draws a batch of lines
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::Polyline( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( numPoints >= 2, "Need at least 2 points." );

	if( IsPoly32(pPoints,numPoints) )
		{
		//
		// it contains 32bit coordinates, so we need to do special handling
		Polyline32( pPoints, numPoints );
		}
	else
		{
		CreatePen( );
		SolidPolyline( pPoints, numPoints );
		SetCurrentPoint( pPoints[numPoints-1] );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::PolylineTo( )
//
//  Description:		Draws a batch of lines, using and updating the current
//							position.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::PolylineTo( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( numPoints < kMaxPoints, "Too many points." );

	if( IsPoly32(pPoints,numPoints) )
		{
		//
		// Contains 32bit coords...
		// NOTE: Sets the current point...
		PolylineTo32( pPoints, numPoints );
		}

	CreatePen( );

	if( m_uwCurrentPointClip || IsPoint32(GetCurrentPoint()) )
		{
		//
		// Start point is 32bit or was clipped, but polyline isn't, so we only
		// need to clip the first line segment...
		SolidLineTo( *pPoints );
		++pPoints;
		--numPoints;
		}

	if( numPoints > 0 )
		{
		SolidPolylineTo( pPoints, numPoints );
		SetCurrentPoint( pPoints[numPoints-1] );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::PolyPolyline( )
//
//  Description:		Draws a bunch of polylines. The default implementation
//							loops and calls Polyline.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::PolyPolyline( const RIntPoint* pPoints, YPointCount* pointCountArray, YPolylineCount polylineCount )
	{
	for( YPolylineCount i = 0; i < polylineCount; i++ )
		{
		Polyline( pPoints, *pointCountArray );
		pPoints += *pointCountArray;
		pointCountArray++;
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FrameRectangle( )
//
//  Description:		Draws a rectangle using the current pen attributes. The
//							rectangle is not filled.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FrameRectangle( const RIntRect& rect )
	{
	CreatePen( );
	//
	// Rectangle does NOT modify the current point...
	uWORD	uwPrevClip;
	RIntPoint	ptPrevCur	= GetCurrentPoint( &uwPrevClip );
	MoveTo( rect.m_TopLeft );
	LineTo( RIntPoint( rect.m_Right - 1, rect.m_Top ) );
	LineTo( RIntPoint( rect.m_Right - 1, rect.m_Bottom - 1 ) );
	LineTo( RIntPoint( rect.m_Left, rect.m_Bottom - 1 ) );
	LineTo( rect.m_TopLeft );
	SetCurrentPoint( ptPrevCur, uwPrevClip );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FrameRectangle( )
//
//  Description:		Renders a rectangle through a transform
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FrameRectangle( const RRealRect& rect, const R2dTransform& transform )
	{
	RPushPenColor pushPenColor( GetPenColor( ), transform, this );

	// Convert the rectangle to a polygon of 4 points
	RIntPoint points[ 4 ];
	CreateRectPoly( rect, transform, &points[ 0 ] );

	// Draw the polygon
	FramePolygon( points, 4 );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FrameRectangleInside( )
//
//  Description:		Renders a rectangle inset by the current pen width.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FrameRectangleInside(
			const RRealRect&		rect,					// @Parm the rectangle to frame
			const R2dTransform&	transform )			// @Parm the transform to apply to the rectangle
{
	RPushFillColor pushFillColor( GetPenColor( ), transform, this );

	RRealVectorRect	rcOuter	= RRealVectorRect(rect) * transform;
	RRealVectorRect	rcInner	= rcOuter;
	YPenWidth			nWidth	= GetPenWidth();
	RRealSize			szInset( -YRealDimension(nWidth), -YRealDimension(nWidth) );
	rcInner.Inflate( szInset );

	RRealVectorRect	rcEdge;
	RIntPoint			ptPoly[4];

	//
	// "top" edge
	rcEdge.m_TopLeft		= rcInner.m_TopLeft;
	rcEdge.m_TopRight		= rcOuter.m_TopLeft;
	rcEdge.m_BottomLeft	= rcInner.m_TopRight;
	rcEdge.m_BottomRight	= rcOuter.m_TopRight;
	CreateRectPoly( rcEdge, &ptPoly[ 0 ] );
	FillPolygon( ptPoly, 4 );

	//
	// "left" edge
	rcEdge.m_BottomLeft	= rcInner.m_BottomLeft;
	rcEdge.m_BottomRight	= rcOuter.m_BottomLeft;
	CreateRectPoly( rcEdge, &ptPoly[ 0 ] );
	FillPolygon( ptPoly, 4 );

	//
	// "bottom" edge
	rcEdge.m_TopLeft		= rcInner.m_BottomRight;
	rcEdge.m_TopRight		= rcOuter.m_BottomRight;
	CreateRectPoly( rcEdge, &ptPoly[ 0 ] );
	FillPolygon( ptPoly, 4 );

	//
	// "right" edge
	rcEdge.m_BottomLeft	= rcInner.m_TopRight;
	rcEdge.m_BottomRight	= rcOuter.m_TopRight;
	CreateRectPoly( rcEdge, &ptPoly[ 0 ] );
	FillPolygon( ptPoly, 4 );
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillRectangle( )
//
//  Description:		Draws a filled rectangle using the current fill color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillRectangle( const RIntRect& rect )
	{
	// Clip the rectangle...
	RIntRect	rcFill( rect );
	if( IsRect32(rcFill) )
		{
		if( !ClipRectangle(rcFill,GetClipRect()) )
			return;
		}

	// Create the brush
	CreateBrush( );

	// Fill using the appropriate fill method
	switch( m_FillColor.GetFillMethod( ) )
		{
		case RColor::kSolid :
			SolidFillRectangle( rcFill );
			break;

		case RColor::kGradient :
		case RColor::kBitmap :
			if( m_fColoringBookModeOn )
				SolidFillRectangle( rcFill );
			else
				GradientFillRectangle( rcFill );
			break;

		case RColor::kTransparent :
			// Do nothing
			break;
		}

	// If coloring book mode is on, we now need to do a frame
	if( m_fColoringBookModeOn )
		FrameRectangle( rcFill );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillRectangle( )
//
//  Description:		Renders a rectangle through a transform
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillRectangle( const RRealRect& rect, const R2dTransform& transform )
	{
	RPushFillColor pushFillColor( GetFillColor( ), transform, this );

	// Convert the rectangle to a polygon of 4 points
	RIntPoint points[ 4 ];
	CreateRectPoly( rect, transform, &points[ 0 ] );

	// Draw the polygon
	FillPolygon( points, 4 );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::InvertRectangle( )
//
//  Description:		Draws a filled rectangle using the current fill color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::InvertRectangle( const RIntRect& rect )
	{
	RIntRect	rcInvert( rect );

	if( IsRect32(rcInvert) )
		{
		if( !ClipRectangle(rcInvert,GetClipRect()) )
			return;
		}

	DeviceInvertRectangle( rcInvert );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::InvertRectangle( )
//
//  Description:		Inverts a rectangle through a transform
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::InvertRectangle( const RRealRect& rect, const R2dTransform& transform )
	{
	// Convert the rectangle to a polygon of 4 points
	RIntPoint points[ 4 ];
	CreateRectPoly( rect, transform, &points[ 0 ] );

	// Draw the polygon
	InvertPolygon( points, 4 );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::Polygon( )
//
//  Description:		Draws a filled, framed polygon using the current fill
//							color and pen attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::Polygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	TpsAssert( numVertices < kMaxPoints, "Too many points." );
	TpsAssert( numVertices >= 2, "Must have at least 2 points." );

	CreatePen( );
	CreateBrush( );

	if( IsPoly32(pVertices,numVertices) )
		Polygon32( pVertices, numVertices );

	else
		{
		switch( m_FillColor.GetFillMethod( ) )
			{
			case RColor::kSolid :
				SolidPolygon( pVertices, numVertices );
				break;

			case RColor::kGradient :
			case RColor::kBitmap :
				if( m_fColoringBookModeOn )
					SolidPolygon( pVertices, numVertices );
				else
					GradientFillPolygon( pVertices, numVertices );

			case RColor::kTransparent :
				FramePolygon( pVertices, numVertices );
			}
		}

	if( m_fColoringBookModeOn )
		FramePolygon( pVertices, numVertices );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FramePolygon( )
//
//  Description:		Draws a framed polygon using the current pen attributes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FramePolygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	TpsAssert( numVertices < kMaxPoints, "Too many points." );
	TpsAssert( numVertices >= 2, "Polygons must have at least 2 vertices." );

	CreatePen( );

	// Draw the polygon as a polyline
	Polyline( pVertices, numVertices );

	// If necessary, connect the first and the last point
	if( numVertices>0 && *pVertices != *( pVertices + numVertices - 1 ) )
		{
		MoveTo( pVertices[numVertices-1] );
		LineTo( pVertices[0] );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillPolygon( )
//
//  Description:		Draws a filled polygon using the current fill color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillPolygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	TpsAssert( numVertices < kMaxPoints, "Too many points." );
	TpsAssert( numVertices >= 2, "Must have at least 2 points." );

	CreateBrush( );

	if( IsPoly32(pVertices,numVertices) )
		FillPolygon32( pVertices, numVertices );

	else
		{
		switch( m_FillColor.GetFillMethod( ) )
			{
			case RColor::kSolid :
				SolidFillPolygon( pVertices, numVertices );
				break;

			case RColor::kGradient :
			case RColor::kBitmap :
				//	without this check, we get infinite recursion (see bug 2481)
				if( m_fColoringBookModeOn )
					SolidFillPolygon( pVertices, numVertices );
				else
					GradientFillPolygon( pVertices, numVertices );
				break;

			case RColor::kTransparent :
				// Do nothing
				break;
			}
		}

	if( m_fColoringBookModeOn )
		FramePolygon( pVertices, numVertices );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::InvertPolygon( )
//
//  Description:		Inverts a polygon.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::InvertPolygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	TpsAssert( numVertices < kMaxPoints, "Too many points." );
	TpsAssert( numVertices >= 2, "Must have at least 2 points." );

	if( IsPoly32(pVertices,numVertices) )
		InvertPolygon32( pVertices, numVertices );
	else
		DeviceInvertPolygon( pVertices, numVertices );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::PolyPolygon( )
//
//  Description:		Draws filled, framed polygons using the current fill
//							color and pen attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::PolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	// If we are only drawing one polygon, just call polygon instead
	if( polygonCount == 1 )
		{
		Polygon( pVertices, *pointCountArray );
		return;
		}

#ifdef TPSDEBUG
	// Make sure we dont have too many points
	int nNumPoints = 0;
	for( int i = 0; i < polygonCount; i++ )
		nNumPoints += pointCountArray[ i ];
	TpsAssert( nNumPoints < kMaxPoints, "Too many points." );
#endif

	CreatePen( );
	CreateBrush( );
	
	if( IsPolyPoly32(pVertices,pointCountArray,polygonCount) )
		PolyPolygon32( pVertices, pointCountArray, polygonCount );

	switch( m_FillColor.GetFillMethod( ) )
		{
		case RColor::kSolid :
			SolidPolyPolygon( pVertices, pointCountArray, polygonCount );
			break;

		case RColor::kGradient :
		case RColor::kBitmap :
			if( m_fColoringBookModeOn )
				SolidPolyPolygon( pVertices, pointCountArray, polygonCount );
			else
				GradientFillPolyPolygon( pVertices, pointCountArray, polygonCount );
			SolidFramePolyPolygon( pVertices, pointCountArray, polygonCount );
			break;

		case RColor::kTransparent :
			FramePolyPolygon( pVertices, pointCountArray, polygonCount );
			break;
		}

	if( m_fColoringBookModeOn )
		FramePolyPolygon( pVertices, pointCountArray, polygonCount );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FramePolyPolygon( )
//
//  Description:		Draws framed polygons using the current pen attributes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FramePolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	// If we are only drawing one polygon, just call polygon instead
	if( polygonCount == 1 )
		{
		FramePolygon( pVertices, *pointCountArray );
		return;
		}

#ifdef TPSDEBUG
	int nNumPoints = 0;
	for( int i = 0; i < polygonCount; i++ )
		nNumPoints += pointCountArray[ i ];
	TpsAssert( nNumPoints < kMaxPoints, "Too many points." );
#endif

	CreatePen( );

	if( IsPolyPoly32(pVertices,pointCountArray,polygonCount) )
		FramePolyPolygon32( pVertices, pointCountArray, polygonCount );
	else
		SolidFramePolyPolygon( pVertices, pointCountArray, polygonCount );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillPolyPolygon( )
//
//  Description:		Draws filled polygons using the current fill color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	// If we are only drawing one polygon, just call polygon instead
	if( polygonCount == 1 )
		{
		FillPolygon( pVertices, *pointCountArray );
		return;
		}

#ifdef TPSDEBUG
	int nNumPoints = 0;
	for( int i = 0; i < polygonCount; i++ )
		nNumPoints += pointCountArray[ i ];
	TpsAssert( nNumPoints < kMaxPoints, "Too many points." );
#endif

	CreateBrush( );

	if( IsPolyPoly32(pVertices,pointCountArray,polygonCount) )
		FillPolyPolygon32( pVertices, pointCountArray, polygonCount );

	else
		{
		switch( m_FillColor.GetFillMethod( ) )
			{
			case RColor::kSolid :
				SolidFillPolyPolygon( pVertices, pointCountArray, polygonCount );
				break;

			case RColor::kGradient :
			case RColor::kBitmap :
				if( m_fColoringBookModeOn )
					SolidFillPolyPolygon( pVertices, pointCountArray, polygonCount );
				else
					GradientFillPolyPolygon( pVertices, pointCountArray, polygonCount );
				break;

			case RColor::kTransparent :
				// Do nothing
				break;
			}
		}

	if( m_fColoringBookModeOn )
		FramePolyPolygon( pVertices, pointCountArray, polygonCount );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::InvertPolyPolygon( )
//
//  Description:		Inverts a Poly polygons
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::InvertPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	// If we are only drawing one polygon, just call polygon instead
	if( polygonCount == 1 )
		{
		InvertPolygon( pVertices, *pointCountArray );
		return;
		}

#ifdef TPSDEBUG
	int nNumPoints = 0;
	for( int i = 0; i < polygonCount; i++ )
		nNumPoints += pointCountArray[ i ];
	TpsAssert( nNumPoints < kMaxPoints, "Too many points." );
#endif

	if( IsPolyPoly32(pVertices,pointCountArray,polygonCount) )
		InvertPolyPolygon32( pVertices, pointCountArray, polygonCount );
	else
		DeviceInvertPolyPolygon( pVertices, pointCountArray, polygonCount );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FrameVectorRect( )
//
//  Description:		Renders a vector rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FrameVectorRect( const RIntVectorRect& vectorRect )
	{
	// Copy the points to a temp buffer
	RIntPoint polygon[ 4 ];
	CreateRectPoly( vectorRect, &polygon[ 0 ] );

	// Frame the polygon
	FramePolygon( polygon, 4 );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FrameVectorRect( )
//
//  Description:		Renders a vector rect through a transform
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FrameVectorRect( const RRealVectorRect& vectorRect, const R2dTransform& transform )
	{
	RPushPenColor pushPenColor( GetPenColor( ), transform, this );

	// Transform the points and copy them to a temp buffer
	RIntPoint polygon[ 4 ];
	CreateRectPoly( vectorRect, transform, &polygon[ 0 ] );

	// Frame the polygon
	FramePolygon( polygon, 4 );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillVectorRect( )
//
//  Description:		Renders a vector rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillVectorRect( const RIntVectorRect& vectorRect )
	{
	// Copy the points to a temp buffer
	RIntPoint polygon[ 4 ];
	CreateRectPoly( vectorRect, &polygon[ 0 ] );

	// Frame the polygon
	FillPolygon( polygon, 4 );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillVectorRect( )
//
//  Description:		Renders a vector rect through a transform
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillVectorRect( const RRealVectorRect& vectorRect, const R2dTransform& transform )
	{
	RPushFillColor pushFillColor( GetFillColor( ), transform, this );

	// Transform the points and copy them to a temp buffer
	RIntPoint polygon[ 4 ];
	CreateRectPoly( vectorRect, transform, &polygon[ 0 ] );

	// Frame the polygon
	FillPolygon( polygon, 4 );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::InvertVectorRect( )
//
//  Description:		Inverts a vector rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::InvertVectorRect( const RIntVectorRect& vectorRect )
	{
	// Copy the points to a temp buffer
	RIntPoint polygon[ 4 ];
	CreateRectPoly( vectorRect, &polygon[ 0 ] );

	// Frame the polygon
	InvertPolygon( polygon, 4 );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::InvertVectorRect( )
//
//  Description:		Inverts a vector rect through a transform
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::InvertVectorRect( const RRealVectorRect& vectorRect, const R2dTransform& transform )
	{
	// Transform the points and copy them to a temp buffer
	RIntPoint polygon[ 4 ];
	CreateRectPoly( vectorRect, transform, &polygon[ 0 ] );

	// Frame the polygon
	InvertPolygon( polygon, 4 );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::PolyBezier( )
//
//  Description:		Frames and Fills a polybezier (nsegments*3)+1 points required
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::PolyBezier( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( pPoints != NULL, "NULL bezier points pointer" );
	TpsAssert( numPoints>1 && ((numPoints-1)%3)==0, "Incorrect number of points" );

	// Get a global buffer to expand points into
	YPointCount	numExpanded;
	RIntPoint*	pptExpanded	= ExpandPolyBezier( *pPoints, pPoints + 1, numPoints - 1, numExpanded );
	//
	// Draw it...
	Polygon( pptExpanded, numExpanded );
	//
	// Release the global buffer
	::ReleaseGlobalBuffer( (uBYTE*)pptExpanded );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::PolyBezierTo( )
//
//  Description:		Draws a series of bezier curves. The first curve is drawn
//							from the current position to the third	point by using the
//							first two points as control points. For each subsequent
//							curve, the function needs exactly three more points, and
//							uses the ending point of the previous curve as the
//							starting point for the next.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::PolyBezierTo( const RIntPoint* pPoints, YPointCount pointCount )
	{
	TpsAssert( pPoints != NULL, "NULL bezier points pointer" );
	TpsAssert( pointCount>1 && ((pointCount)%3)==0, "Incorrect number of points" );

	YPointCount	numExpanded;
	RIntPoint*	pptExpanded	= ExpandPolyBezier( GetCurrentPoint( ), pPoints, pointCount, numExpanded );
	//
	// Draw it...
	Polyline( pptExpanded, numExpanded );
	//
	// Release the global buffer...
	::ReleaseGlobalBuffer( (uBYTE*)pptExpanded );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FramePolyBezier( )
//
//  Description:		Frames a polybezier (nsegments*3)+1 points required
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FramePolyBezier( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( pPoints != NULL, "NULL bezier points pointer" );
	TpsAssert( numPoints>1 && ((numPoints-1)%3)==0, "Incorrect number of points" );

	YPointCount	numExpanded;
	RIntPoint*	pptExpanded	= ExpandPolyBezier( *pPoints, pPoints+1, numPoints-1, numExpanded );
	//
	// Draw it...
	Polyline( pptExpanded, numExpanded );
	//
	// Release the global buffer...
	::ReleaseGlobalBuffer( (uBYTE*)pptExpanded );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillPolyBezier( )
//
//  Description:		Frames a polybezier (nsegments*3)+1 points required
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillPolyBezier( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( pPoints != NULL, "NULL bezier points pointer" );
	TpsAssert( numPoints>1 && ((numPoints-1)%3)==0, "Incorrect number of points" );

	YPointCount	numExpanded;
	RIntPoint*	pptExpanded	= ExpandPolyBezier( *pPoints, pPoints+1, numPoints-1, numExpanded );
	//
	// Draw it...
	FillPolygon( pptExpanded, numExpanded );
	//
	// Release the global buffer...
	::ReleaseGlobalBuffer( (uBYTE*)pptExpanded );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::PolyQuadraticSplineTo( )
//
//  Description:		Draws a series of quadratic splines by creating bezier
//							curves and caling PolyBezierTo.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::PolyQuadraticSplineTo( const RIntPoint* pPoints, YPointCount pointCount )
	{
	RIntPoint	splinePoints[3];
	RIntPoint	bezierPoints[4];
	TpsAssert( ( pointCount < kMaxPoints ), "Too many points." );
	TpsAssert( ( pointCount % 2 == 0 ), "There must be exactly 3 points for each bezier." );

	for ( int i = 0; i < pointCount; i += 2 )
		{
		splinePoints[0]	= pPoints[i-1];
		splinePoints[1]	= pPoints[i];
		splinePoints[2]	= pPoints[i+1];
		CreateBezierFromSpline( &splinePoints[ 0 ], &bezierPoints[ 0 ] );
		PolyBezierTo( (bezierPoints+1), 3 );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::Ellipse( )
//
//  Description:		Frame and Fill the given ellipse
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::Ellipse( const RIntRect& rect )
{
	RIntPoint		ptBez[13];
	RIntVectorRect	vrc( rect );
	YPointCount		nPoints	= CreateEllipsePolyBezier( vrc, ptBez );
	PolyBezier( ptBez, nPoints );
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FrameEllipse( )
//
//  Description:		Frame the given ellipse
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FrameEllipse( const RIntRect& rect )
{
	RIntPoint		ptBez[13];
	RIntVectorRect	vrc( rect );
	YPointCount		nPoints	= CreateEllipsePolyBezier( vrc, ptBez );
	FramePolyBezier( ptBez, nPoints );
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillEllipse( )
//
//  Description:		Fill the given ellipse
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillEllipse( const RIntRect& rect )
{
	RIntPoint		ptBez[13];
	RIntVectorRect	vrc( rect );
	YPointCount		nPoints	= CreateEllipsePolyBezier( vrc, ptBez );
	FillPolyBezier( ptBez, nPoints );
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::Ellipse( )
//
//  Description:		Frame and Fill the given ellipse after transformation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::Ellipse( const RRealRect& rect, const R2dTransform& transform )
{
	RIntPoint		ptBez[13];
	RIntVectorRect	vrc		= RRealVectorRect(rect) * transform;
	YPointCount		nPoints	= CreateEllipsePolyBezier( vrc, ptBez );
	PolyBezier( ptBez, nPoints );
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FrameEllipse( )
//
//  Description:		Frame the given ellipse after transformation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FrameEllipse( const RRealRect& rect, const R2dTransform& transform )
{
	RPushPenColor pushPenColor( GetPenColor( ), transform, this );

	RIntPoint		ptBez[13];
	RIntVectorRect	vrc		= RRealVectorRect(rect) * transform;
	YPointCount		nPoints	= CreateEllipsePolyBezier( vrc, ptBez );
	FramePolyBezier( ptBez, nPoints );
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillEllipse( )
//
//  Description:		Fill the given ellipse after transformation
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillEllipse( const RRealRect& rect, const R2dTransform& transform )
{
	RPushFillColor pushFillColor( GetFillColor( ), transform, this );

	RIntPoint		ptBez[13];
	RIntVectorRect	vrc		= RRealVectorRect(rect) * transform;
	YPointCount		nPoints	= CreateEllipsePolyBezier( vrc, ptBez );
	FillPolyBezier( ptBez, nPoints );
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetFillColor( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::SetFillColor( const RColor& fillColor )
	{
	// If the fill color is changing, mark the brush as dirty
	if( fillColor != m_FillColor )
		{
		m_fBrushDirty = TRUE;
		m_FillColor = fillColor;
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetFillColor( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::SetFillColor( const RColor& fillColor, const R2dTransform& transform )
	{
	RColor tempColor = fillColor * transform;

	// If the fill color is changing, mark the brush as dirty
	if( tempColor != m_FillColor )
		{
		m_fBrushDirty = TRUE;
		m_FillColor = tempColor;
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetPenColor( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::SetPenColor( const RColor& penColor )
	{
	TpsAssert( penColor.GetFillMethod( ) == RColor::kSolid, "Only solid pens supported." );

	// If the pen color is changing, and we are not in coloring book mode, mark the pen as dirty
	if( !m_fColoringBookModeOn && m_PenColor != penColor )
		{
		m_fPenDirty = TRUE;
		m_PenColor = penColor;
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetPenColor( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::SetPenColor( const RColor& penColor, const R2dTransform& transform )
	{
	TpsAssert( penColor.GetFillMethod( ) == RColor::kSolid, "Only solid pens supported." );
	
	RColor tempColor = penColor * transform;

	// If the pen color is changing, and we are not in coloring book mode, mark the pen as dirty
	if( !m_fColoringBookModeOn && m_PenColor != penColor )
		{
		m_fPenDirty = TRUE;
		m_PenColor = tempColor;
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetFontColor( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::SetFontColor( const RColor& fontColor )
	{
	//	if color book mode, don't let color be changed from default of black
	if ( !m_fColoringBookModeOn && ( fontColor != m_FontColor ) )
		{
		MacCode( m_fFontDirty = TRUE );
		m_FontColor = fontColor;
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetFont( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::SetFont( const YFontInfo& fontInfo )
	{
	// If the fill color is changing, mark the pen as dirty
	if( !m_fFontSet || (fontInfo != m_FontInfo) )
		{
		m_fFontDirty = TRUE;
		m_fFontSet = TRUE;
		m_FontInfo = fontInfo;
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetFont( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::SetFont( const YFontDescriptor& fdesc, const R2dTransform& transform )
	{
	YFontInfo fontInfo = fdesc.pRFont->GetInfo() * transform;
	SetFont( fontInfo );
	SetFontColor( fdesc.color );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetTintColor()
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RDrawingSurface::SetTintColor( const RSolidColor& tintColor )
{
	//	don't tint if we're in coloring book mode
	if(!m_fColoringBookModeOn)
	{
		m_TintColor = tintColor;
		m_fBrushDirty = TRUE;
		m_fPenDirty = TRUE;
	}
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetTintColor( )
//
//  Description:		Gets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RSolidColor& RDrawingSurface::GetTintColor( )
{
	return m_TintColor;
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetTint()
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::SetTint(YTint tint)
	{
	//	don't tint if we're in coloring book mode
	if(!m_fColoringBookModeOn)
		{
		TpsAssert((tint >= kMinTint) && (tint <= kMaxTint), "Invalid tint value.");
		m_Tint = tint;
		m_fBrushDirty = TRUE;
		m_fPenDirty = TRUE;
		}
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetTint( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YTint RDrawingSurface::GetTint( ) const
	{
	return m_Tint;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GradientFillRectangle( )
//
//  Description:		Fill a rectangle with the current gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::GradientFillRectangle( const RIntRect& rect )
	{
	//
	// Intersect the rectangle with the clip rectangle...
	RIntRect	rcPoly( rect );
	rcPoly.Intersect( rcPoly, GetClipRect() );
	if( rcPoly.IsEmpty() )
		{
		//
		// nothing to draw!
		return;
		}
	//
	// Fill it...
	R2dTransform	Identity;
	RColor			fillColor = m_FillColor;

	if (RColor::kBitmap == m_FillColor.GetFillMethod( ))
		fillColor.GetImageColor()->Fill( *this, rcPoly, Identity );
	else
		fillColor.GetGradientFill()->DrawGradient( *this, rcPoly, Identity, GetGradientPrecision() );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GradientFillRectangle( )
//
//  Description:		Fill a rectangle with the current gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::GradientFillRectangle( const RIntRect& rect, const R2dTransform& transform )
	{
	RIntPoint	pts[4];
	YPointCount	cnt	= NumElements( pts );
	CreateRectPoly( rect, transform, &pts[ 0 ] );
	GradientFillPolyPolygon( pts, &cnt, 1, transform );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GradientFillRectangle( )
//
//  Description:		Fill a rectangle with the current gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::GradientFillRectangle( YIntCoordinate left, YIntCoordinate top, YIntCoordinate right, YIntCoordinate bottom )
	{
	RIntRect		rect( left, top, right, bottom );
	GradientFillRectangle( rect );
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GradientFillVectorRect( )
//
//  Description:		Fill a vector rectangle with the current gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::GradientFillVectorRect( const RIntVectorRect& vectorRect )
	{
	RIntPoint	pts[4];
	YPointCount	cnt	= NumElements( pts );
	CreateRectPoly( vectorRect, &pts[ 0 ] );
	GradientFillPolyPolygon( pts, &cnt, 1 );
	}

// ****************************************************************************
//
//  Virtual
//
//  Function Name:	RDrawingSurface::GradientFillVectorRect( )
//
//  Description:		Fill a vector rectangle with the current gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::GradientFillVectorRect( const RIntVectorRect& vectorRect, const R2dTransform& transform )
	{
	RIntPoint	pts[4];
	YPointCount	cnt	= NumElements( pts );
	CreateRectPoly( vectorRect, transform, &pts[ 0 ] );
	GradientFillPolyPolygon( pts, &cnt, 1, transform );
	}

// ****************************************************************************
//
//  Virtual
//
//  Function Name:	RDrawingSurface::GradientFillPolygon( )
//
//  Description:		Fill a polygon with the current gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::GradientFillPolygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	GradientFillPolyPolygon( pVertices, &numVertices, 1 );
	}

// ****************************************************************************
//
//  Virtual
//
//  Function Name:	RDrawingSurface::GradientFillPolygon( )
//
//  Description:		Fill a polygon with the current gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::GradientFillPolygon( const RIntPoint* pVertices, YPointCount numVertices, const R2dTransform& transform )
	{
	GradientFillPolyPolygon( pVertices, &numVertices, 1, transform );
	}

// ****************************************************************************
//
//  Virtual
//
//  Function Name:	RDrawingSurface::GradientFillPolyPolygon( )
//
//  Description:		Fill a polygon with the current gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::GradientFillPolyPolygon( const RIntPoint* pVertices, YPointCount* pNumVertices, YPolygonCount nPolys )
	{
	R2dTransform	Identity;
	GradientFillPolyPolygon( pVertices, pNumVertices, nPolys, Identity );
	}

// ****************************************************************************
//
//  Virtual
//
//  Function Name:	RDrawingSurface::GradientFillPolyPolygon( )
//
//  Description:		Fill a rectangle with the current gradient
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::GradientFillPolyPolygon( const RIntPoint* pVertices, YPointCount* pnumVertices, YPolygonCount nPolys, const R2dTransform& GradientTransform )
	{
//	TpsAssert( m_FillColor.GetGradientFill() != NULL, "color is not a gradient" );
	if ( m_fColoringBookModeOn )
		{
		FillPolyPolygon( pVertices, pnumVertices, nPolys );
		return;
		}

	RIntRect				rcPoly;
	YPolygonCount		n					= nPolys;
	const RIntPoint*	ppt				= pVertices;
	YPointCount*		pcnt				= pnumVertices;
	YPointCount			nTotalPoints	= 0;
	rcPoly.m_Left	= ppt->m_x;
	rcPoly.m_Right	= ppt->m_x;
	rcPoly.m_Top		= ppt->m_y;
	rcPoly.m_Bottom	= ppt->m_y;
	while( n-- )
		{
		YPointCount	cnt	= *pcnt++;
		nTotalPoints		+= cnt;
		while( cnt-- )
			{
			if( ppt->m_x < rcPoly.m_Left )	rcPoly.m_Left		= ppt->m_x;
			if( ppt->m_x > rcPoly.m_Right )	rcPoly.m_Right		= ppt->m_x;
			if( ppt->m_y < rcPoly.m_Top )		rcPoly.m_Top		= ppt->m_y;
			if( ppt->m_y > rcPoly.m_Bottom )	rcPoly.m_Bottom	= ppt->m_y;
			++ppt;
			}
		}

	//
	// Now intersect it with the clip rectangle...
	rcPoly.Intersect( rcPoly, GetClipRect() );
	if ( rcPoly.IsEmpty() )
		return;

	if( HasPathSupport() )
		{
		RDrawingSurfaceState	CurState( this );
		BeginPath();
		PolyPolygon( pVertices, pnumVertices, nPolys );
		EndPath();
		IntersectClipPath();
		RColor	fillColor = m_FillColor;

		if (RColor::kBitmap == m_FillColor.GetFillMethod( ))
			fillColor.GetImageColor()->Fill( *this, rcPoly, GradientTransform );
		else
			fillColor.GetGradientFill()->DrawGradient( *this, rcPoly, GradientTransform, GetGradientPrecision() );
		}// has path support
	else
		{
// Bitmaps are now working on the Mac so we can comment this out
//#ifdef IMAGER
//		static int kfNotUseGradientBitmaps = TRUE;
//#else
		static short kfNotUseGradientBitmaps = FALSE;
//#endif
		short kfForceGradientBitmaps = (RColor::kBitmap == m_FillColor.GetFillMethod());

		if ( (IsPrinting( ) && !kfForceGradientBitmaps ) || kfNotUseGradientBitmaps )
			{
			RColor	fillColor = m_FillColor;
			fillColor.GetGradientFill()->DrawGradient( *this, rcPoly, pVertices, pnumVertices, nPolys, GradientTransform, GetGradientPrecision() );
			return;
			}
		else
			{
			ROffscreenDrawingSurface	dsGradient;
			ROffscreenDrawingSurface	dsShape;
			RBitmapImageBase*				pbmGradient = NULL;
			RBitmapImageBase*				pbmShape = NULL;
			RIntRect							rcBand						= rcPoly;
			int								nBandWidth					= rcBand.Width();
			int								nBandHeight					= rcBand.Height();
			RIntSize							szSurfaceDPI				= GetDPI();
			BOOLEAN							fInitialized				= FALSE;
			const int						kSurfaceColorBitDepth	= 24;
			const int						kSurfaceMonoBitDepth		= 1;
			const	int						kMinRectDimension			= 16;
			const	int						kShrinkFactor				= 4;
			YIntDimension					nCurrentXOffset			= 0;
			YIntDimension					nCurrentYOffset			= 0;
			YIntDimension					nRequiredXOffset			= 0;
			YIntDimension					nRequiredYOffset			= 0;
			RIntPoint*						pOffsetVertices			= const_cast<RIntPoint*>(pVertices);

			YRealDimension xScale = 1.0;
			YRealDimension yScale = 1.0;

			try
				{
				//
				// Create the offscreen bitmaps
				while( !fInitialized )
					{
					try
						{												
						fInitialized = TRUE;
						if( IsPrinting( ) )
							{							
							uLONG uHiResPhoto = 0;
							RRenaissanceUserPreferences().GetLongValue( 
								RRenaissanceUserPreferences::kHiResPhoto, uHiResPhoto );

							// limit the bitmap resolution to either
							// kLoResBitmapXDpi, kLoResBitmapYDpi or kHiResBitmapXDpi, kHiResBitmapYDpi
							// depending on user preference in registry
							YRealDimension bitmapXDpi = kLoResBitmapXDpi;
							YRealDimension bitmapYDpi = kLoResBitmapYDpi;
							if (uHiResPhoto)
							{
								bitmapXDpi = kHiResBitmapXDpi;
								bitmapYDpi = kHiResBitmapYDpi;
							}
							if (szSurfaceDPI.m_dx > bitmapXDpi)
							{
								xScale = float( bitmapXDpi ) / szSurfaceDPI.m_dx;
								szSurfaceDPI.m_dx = bitmapXDpi;
							}
							if (szSurfaceDPI.m_dy > bitmapYDpi)
							{
								yScale = float( bitmapYDpi ) / szSurfaceDPI.m_dy;
								szSurfaceDPI.m_dy = bitmapYDpi;
							}
							nBandWidth  = ::Round( nBandWidth * xScale );
							nBandHeight = ::Round( nBandHeight * yScale );

							pbmGradient = new RBitmapImage;
							static_cast<RBitmapImage*>( pbmGradient )->Initialize( nBandWidth, nBandHeight, kSurfaceColorBitDepth, szSurfaceDPI.m_dx, szSurfaceDPI.m_dy );

							pbmShape = new RBitmapImage;
							static_cast<RBitmapImage*>( pbmShape )->Initialize( nBandWidth, nBandHeight, kSurfaceMonoBitDepth, szSurfaceDPI.m_dx, szSurfaceDPI.m_dy );
							}
						else
							{
							pbmGradient = new RScratchBitmapImage;
							pbmGradient->Initialize( *this, nBandWidth, nBandHeight );														

							pbmShape = new RScratchBitmapImage;
							static_cast<RScratchBitmapImage*>( pbmShape )->Initialize( nBandWidth, nBandHeight, 1 );
							}
						}
					catch(YException yException)
						{
						if( yException!=kMemory )
							throw;
												
						fInitialized = FALSE;
						}
					
					if( !fInitialized )
						{
						//	Shrink the rectangle.  If it gets too small, then we bail...
						// NOTE: the bitmaps will be destroyed by the RBitmap destructor, or
						// when we reinitialize...
						//	Compute a new height...
						nBandHeight	= (nBandHeight / kShrinkFactor) + 1;
						if( nBandHeight < kMinRectDimension )
							return;	//	Gradient too small...
						}
					}

				//
				// Set the band coordinates...
				rcBand.m_Left   = ::Round( rcPoly.m_Left * xScale );
				rcBand.m_Top    = ::Round( rcPoly.m_Top * yScale );
				rcBand.m_Bottom = rcBand.m_Top + nBandHeight;
				rcBand.m_Right	 = rcBand.m_Left + nBandWidth;

				//
				//	If there is a tint, apply it to the dsPattern
				dsGradient.SetTint( GetTint( ) );

				//
				// Determine the output rect...
				R2dTransform xform;
				xform.PostScale( 1 / xScale, 1 / yScale );
				RIntRect	rcBandOut( 
					::Round( rcBand.m_Left   / xScale ),
					::Round( rcBand.m_Top    / yScale ),
					::Round( rcBand.m_Right  / xScale ),
					::Round( rcBand.m_Bottom / yScale )	);

				//
				// Adjust the offsets for the offscreens...
				nRequiredXOffset	= -rcBand.m_Left;
				nRequiredYOffset	= -rcBand.m_Top;
				rcBand.Offset( RIntSize(nRequiredXOffset,nRequiredYOffset) );

				//
				// Save the clip rectangle for the band drawing surfaces...
				RIntRect	rcClip = rcBand;

				//
				//	Loop through to draw the gradient
				do
					{
					//
					// Setup the transform for the band..
					R2dTransform	xformBand( GradientTransform );
					xformBand.PostScale( xScale, yScale );
					xformBand.PostTranslate( YRealDimension(nRequiredXOffset), YRealDimension(nRequiredYOffset) );
					//
					// Draw the gradient into the gradient band...
					try
						{
						//
						// Draw into the gradient bitmap...
						dsGradient.SetImage( pbmGradient );
						//
						// Setup the clip rectangles for the band drawing surfaces...
						dsGradient.SetClipRect( rcClip );
						//
						// Draw the gradient
						RColor	fillColor = m_FillColor;
						if (RColor::kBitmap == m_FillColor.GetFillMethod( ))
							fillColor.GetImageColor()->Fill( dsGradient, rcBand, xformBand );
						else
							fillColor.GetGradientFill()->DrawGradient( dsGradient, rcBand, xformBand, GetGradientPrecision() );

						//
						// Release the bitmap from the drawing surface...
						dsGradient.ReleaseImage();
						}
					catch(YException)
						{
						dsGradient.ReleaseImage();
						throw;
						}
					try
						{
						//
						// Draw into the mask bitmap
						dsShape.SetImage( pbmShape );
						dsShape.SetClipRect( rcClip );
						//
						// Draw the shape into the mask band...
						// First, clear the background.  Remember: FillRectangle will only fill the
						// INTERIOR, so we need to adjust the bounds first...
						RIntRect	rcInterior( rcBand );
						rcInterior.Inflate( RIntSize(1,1) );
						dsShape.SetFillColor( RSolidColor( kBlack ) );	// 0 bits
						dsShape.FillRectangle( rcInterior );
						dsShape.SetFillColor( RSolidColor( kWhite ));		// 1 bits
						//
						// Offset the vertices...
						RIntPoint*	pptOffset	= pOffsetVertices;
						YPointCount	nPts			= nTotalPoints;
						while( nPts-- )
							{
							pptOffset->m_x = pptOffset->m_x * xScale + nRequiredXOffset;
							pptOffset->m_y = pptOffset->m_y * yScale + nRequiredYOffset;
							++pptOffset;
							}
						nCurrentXOffset	= nRequiredXOffset;
						nCurrentYOffset	= nRequiredYOffset;
						//
						// Draw the shape
						dsShape.FillPolyPolygon( pVertices, pnumVertices, nPolys );
						//
						// Fix the vertices...
						pptOffset	= pOffsetVertices;
						nPts			= nTotalPoints;
						while( nPts-- )
							{
							pptOffset->m_x = (pptOffset->m_x - nCurrentXOffset) / xScale;
							pptOffset->m_y = (pptOffset->m_y - nCurrentYOffset) / yScale;
							++pptOffset;
							}
						nCurrentXOffset	= 0;
						nCurrentYOffset	= 0;
						//
						// Release the bitmap from the drawing surface...
						dsShape.ReleaseImage();
						}
					catch(YException)
						{
						dsShape.ReleaseImage();
						throw;
						}

					//
					// xbmGradient and xbmMask are ready to print (or display)
					// so do it...
					pbmGradient->RenderWithMask(*this, *pbmShape, rcBand, rcBandOut);

					//
					//	Setup for next loop
					rcBandOut.m_Top		+= nBandHeight / yScale;
					rcBandOut.m_Bottom	+= nBandHeight / yScale;
					//
					// Setup the next offset...
					nRequiredYOffset		-= nBandHeight;

					if( rcBandOut.m_Bottom > rcPoly.m_Bottom )
						{
						rcBand.m_Bottom	-= (rcBandOut.m_Bottom * yScale - rcPoly.m_Bottom);
						rcBandOut.m_Bottom = rcPoly.m_Bottom;
						}
					}
				while ( rcBandOut.m_Top < rcBandOut.m_Bottom );
				}// try it
			catch( ... )
				{
				//
				// Fix the vertices...
				if( nCurrentXOffset || nCurrentYOffset )
					{
					RIntPoint*	pptOffset	= pOffsetVertices;
					YPointCount	nPts			= nTotalPoints;
					while( nPts-- )
						{
						pptOffset->m_x -= nCurrentXOffset;
						pptOffset->m_y -= nCurrentYOffset;
						++pptOffset;
						}
					}
				dsGradient.ReleaseImage();
				dsShape.ReleaseImage();

				delete pbmGradient;
				delete pbmShape;

				throw;
				}// catch any errors

			delete pbmGradient;
			delete pbmShape;

			}// not printing
		}// no path support
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::ExpandPolyBezier( )
//
//  Description:		Create the polyline representing the given polybezier
//
//  Returns:			RIntPoint* pptExpanded
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntPoint* RDrawingSurface::ExpandPolyBezier( const RIntPoint& startPoint, const RIntPoint* pPoints, YPointCount numPoints, YPointCount& numExpanded )
	{
	TpsAssert( pPoints != NULL, "NULL Bezier points pointer passed to ExpandPolyBezier" );
	TpsAssert( numPoints>1 && ((numPoints)%3)==0, "Incorrect number of points passed to ExpandPolyBezier" );

	//
	// Get a global buffer...
	YPointCount	nMaxPoints	= YPointCount(::GetGlobalBufferSize()/sizeof(RIntPoint));
	RIntPoint*	pptExpanded	= NULL;
	int			nTries		= 2;
	while( !pptExpanded && nTries-- )
		{
		//
		// for each segment in the polybezier, DoBezier2() will append the points to the given
		// buffer and bump the buffer pointer.  It assumes the bezier is begun at the point in
		// (pptExpanded-1)...
		pptExpanded = reinterpret_cast<RIntPoint*>( ::GetGlobalBuffer(uLONG(nMaxPoints)*sizeof(RIntPoint)) );
		const RIntPoint*	parPoints	= pPoints;
		RIntPoint*			pptTmp		= pptExpanded;
		RIntPoint			ptStart		= startPoint;
		*pptTmp++							= ptStart;
		YPointCount			nPts			= numPoints;
		numExpanded							= 1;
		while( nPts>=3 )
			{
			::FillBezier( ptStart, *parPoints, *(parPoints+1), *(parPoints+2), pptTmp, numExpanded, nMaxPoints );
			ptStart			= *(parPoints+2);
			nPts				-= 3;
			parPoints		+= 3;
			}
		if( numExpanded > nMaxPoints )
			{
			//
			// Overflowed, so try again with a new buffer...
			::ReleaseGlobalBuffer( reinterpret_cast<uBYTE*>(pptExpanded) );
			pptExpanded = NULL;
			nMaxPoints	= numExpanded;
			}
		}
	if( !pptExpanded )
		throw kMemory;
	return pptExpanded;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::ExpandPolyBezier( )
//
//  Description:		Create the polyline representing the given polybezier
//
//  Returns:			RRealPoint* pptExpanded
//
//  Exceptions:		None
//
// ****************************************************************************
//
RRealPoint* RDrawingSurface::ExpandPolyBezier( const RRealPoint& startPoint, const RRealPoint* pPoints, YPointCount numPoints, YPointCount& numExpanded )
	{
	TpsAssert( pPoints != NULL, "NULL Bezier points pointer passed to ExpandPolyBezier" );
	TpsAssert( numPoints>1 && ((numPoints)%3)==0, "Incorrect number of points passed to ExpandPolyBezier" );

	//
	// Get a global buffer...
	YPointCount	nMaxPoints	= YPointCount(::GetGlobalBufferSize()/sizeof(RRealPoint));
	RRealPoint*	pptExpanded	= NULL;
	int			nTries		= 2;
	while( !pptExpanded && nTries-- )
		{
		//
		// for each segment in the polybezier, DoBezier2() will append the points to the given
		// buffer and bump the buffer pointer.  It assumes the bezier is begun at the point in
		// (pptExpanded-1)...
		pptExpanded = reinterpret_cast<RRealPoint*>( ::GetGlobalBuffer(uLONG(nMaxPoints)*sizeof(RRealPoint)) );
		const RRealPoint*	parPoints	= pPoints;
		RRealPoint*			pptTmp		= pptExpanded;
		RRealPoint			ptStart		= startPoint;
		*pptTmp++							= ptStart;
		YPointCount			nPts			= numPoints;
		numExpanded							= 1;
		while( nPts>=3 )
			{
			::FillBezier( ptStart, *parPoints, *(parPoints+1), *(parPoints+2), pptTmp, numExpanded, nMaxPoints );
			ptStart			= *(parPoints+2);
			nPts				-= 3;
			parPoints		+= 3;
			}
		if( numExpanded > nMaxPoints )
			{
			//
			// Overflowed, so try again with a new buffer...
			::ReleaseGlobalBuffer( reinterpret_cast<uBYTE*>(pptExpanded) );
			pptExpanded = NULL;
			nMaxPoints	= numExpanded;
			}
		}
	if( !pptExpanded )
		throw kMemory;
	return pptExpanded;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::CreateEllipsePolyBezier( )
//
//  Description:		Create the polybezier representing the given ellipse
//							requires a buffer large enough to hold 13 points.
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YPointCount RDrawingSurface::CreateEllipsePolyBezier( const RIntVectorRect& rc, RIntPoint* pptBez )
{
	//
	// The following magic number (kOffsetFactor) was arrived at empirically.  It provides for
	// the best possible match between the polybezier output and the Windows GDI Ellipse() output
	// using a normal rectangle...
	const	YRealDimension	kScaleFactor( 554.0/1000.0 );
	const RRealSize		kControlPointScale( kScaleFactor, kScaleFactor );
	//
	// Intersections with the rectangle...
	midpoint( pptBez[0],	rc.m_BottomLeft,	rc.m_TopLeft );
	midpoint( pptBez[3],	rc.m_TopLeft,		rc.m_TopRight );
	midpoint( pptBez[6],	rc.m_TopRight,		rc.m_BottomRight );
	midpoint( pptBez[9],	rc.m_BottomRight,	rc.m_BottomLeft );
	pptBez[12]	= pptBez[0];
	//
	// Control points
	pptBez[ 1]	= pptBez[ 0] + (rc.m_TopLeft-pptBez[ 0]).Scale( kControlPointScale );
	pptBez[ 2]	= pptBez[ 3] + (rc.m_TopLeft-pptBez[ 3]).Scale( kControlPointScale );
	pptBez[ 4]	= pptBez[ 3] + (rc.m_TopRight-pptBez[ 3]).Scale( kControlPointScale );
	pptBez[ 5]	= pptBez[ 6] + (rc.m_TopRight-pptBez[ 6]).Scale( kControlPointScale );
	pptBez[ 7]	= pptBez[ 6] + (rc.m_BottomRight-pptBez[ 6]).Scale( kControlPointScale );
	pptBez[ 8]	= pptBez[ 9] + (rc.m_BottomRight-pptBez[ 9]).Scale( kControlPointScale );
	pptBez[10]	= pptBez[ 9] + (rc.m_BottomLeft-pptBez[ 9]).Scale( kControlPointScale );
	pptBez[11]	= pptBez[12] + (rc.m_BottomLeft-pptBez[12]).Scale( kControlPointScale );

	return 13;
}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::CreateArcPolyBezier( )
//
//  Description:		Create the polybezier representing the given arc
//							requires a buffer large enough to hold 7 points.
//
//  Returns:			7
//
//  Exceptions:		None
//
// ****************************************************************************
//
YPointCount RDrawingSurface::CreateArcPolyBezier( const RIntVectorRect& rc, RIntPoint* pptBez )
{
	//
	// The following magic number (kOffsetFactor) was arrived at empirically.  It provides for
	// the best possible match between the polybezier output and the Windows GDI Ellipse() output
	// using a normal rectangle...
	const	YRealDimension	kScaleFactor( 554.0/1000.0 );
	const RRealSize		kControlPointScale( kScaleFactor, kScaleFactor );
	//
	// Intersections with the rectangle...
	pptBez[0] = rc.m_BottomLeft;
	midpoint( pptBez[3],	rc.m_TopLeft,		rc.m_TopRight );
	pptBez[6] = rc.m_BottomRight;

	//
	// Control points
	pptBez[ 1]	= pptBez[ 0] + (rc.m_TopLeft-pptBez[ 0]).Scale( kControlPointScale );
	pptBez[ 2]	= pptBez[ 3] + (rc.m_TopLeft-pptBez[ 3]).Scale( kControlPointScale );
	pptBez[ 4]	= pptBez[ 3] + (rc.m_TopRight-pptBez[ 3]).Scale( kControlPointScale );
	pptBez[ 5]	= pptBez[ 6] + (rc.m_TopRight-pptBez[ 6]).Scale( kControlPointScale );

	return 7;
}



//
// 32 bit coordinate support
//

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::ClipHorizontalEdge( RIntPoint& pt, const RIntSize& vecDir, YIntCoordinate yEdge, const RIntRect& rcClip )
//
//  Description:		Clip the given vector's start point to the given rectangle at the given horizontal line
//
//  Returns:			uWORD uwNextClip -- flag specifying next clip necessary
//							pt is modified as necessary...
//							vecDir is NOT modified!
//
//  Exceptions:		None
//
// ****************************************************************************
//
uWORD RDrawingSurface::ClipHorizontalEdge( RIntPoint& pt, const RIntSize& vecDir, YIntCoordinate yEdge, const RIntRect& rcClip )
{
	pt.m_x	+= vecDir.m_dx * (yEdge-pt.m_y) / vecDir.m_dy;
	pt.m_y	= yEdge;
	if( pt.m_x < rcClip.m_Left )
		return kClipLeft;
	if( pt.m_x >= rcClip.m_Right )
		return kClipRight;
	return 0;
}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::ClipVerticalEdge( RIntPoint& pt, const RIntSize& vecDir, YIntCoordinate xEdge, const RIntRect& rcClip )
//
//  Description:		Clip the given vector's start point to the given rectangle at the given vertical line
//
//  Returns:			uWORD uwNextClip -- flag specifying next clip necessary
//							pt is modified as necessary...
//							vecDir is NOT modified!
//
//  Exceptions:		None
//
// ****************************************************************************
//
uWORD RDrawingSurface::ClipVerticalEdge( RIntPoint& pt, const RIntSize& vecDir, YIntCoordinate xEdge, const RIntRect& rcClip )
{
	pt.m_y	+= vecDir.m_dy * (xEdge-pt.m_x) / vecDir.m_dx;
	pt.m_x	= xEdge;
	if( pt.m_y < rcClip.m_Top )
		return kClipTop;
	if( pt.m_y >= rcClip.m_Bottom )
		return kClipBottom;
	return 0;
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::ClipVector( RIntPoint& pt, const RIntSize& vecDir, uWORD uwClip, const RIntRect& rcClip )
//
//  Description:		Clip the given vector's start point to the given rectangle
//
//  Returns:			uWORD uwNextClip -- flag specifying next clip necessary
//							pt is modified as necessary...
//							vecDir is NOT modified!
//
//  Exceptions:		None
//
// ****************************************************************************
//
uWORD RDrawingSurface::ClipVector( RIntPoint& pt, const RIntSize& vecDir, uWORD uwClip, const RIntRect& rcClip )
{
	//
	// Clip the point...
	if( uwClip & kClipLeft )
		{
		//
		// point is to the left of the clip rectangle...
		uwClip = ClipVerticalEdge( pt, vecDir, rcClip.m_Left, rcClip );
		}
	else if( uwClip & kClipRight )
		{
		//
		// point is to the right of the clip rectangle...
		uwClip	= ClipVerticalEdge( pt, vecDir, rcClip.m_Right, rcClip );
		}
	if( uwClip & kClipTop )
		{
		//
		// point is above the clip rectangle...
		uwClip	= ClipHorizontalEdge( pt, vecDir, rcClip.m_Top, rcClip );
		}
	else if( uwClip & kClipBottom )
		{
		//
		// point is below the clip rectangle...
		uwClip	= ClipHorizontalEdge( pt, vecDir, rcClip.m_Bottom, rcClip );
		}
	return uwClip;
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::DoClipLineSegment( RIntPoint& pt0, uWORD uwClip0, RIntPoint& pt1, uWORD uwClip1, const RIntRect& rcClip )
//
//  Description:		Clip the given linesegment using the given clip edges
//
//  Returns:			TRUE if the resultant linesegment is visible else FALSE
//							pt0 and pt1 are modified as necessary...
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDrawingSurface::DoClipLineSegment( RIntPoint& pt0, uWORD uwClip0, RIntPoint& pt1, uWORD uwClip1, const RIntRect& rcClip )
{
	if( uwClip0==0 && uwClip1==0 )
		{
		//
		// neither point is clipped...
		return TRUE;
		}
	//
	// One of the points is clipped, so make sure both have been initialized properly...
	if( !uwClip0 )
		uwClip0	= GetPointClipEdges( pt0, rcClip );
	if( !uwClip1 )
		uwClip1	= GetPointClipEdges( pt1, rcClip );

	if( (uwClip0&uwClip1) != 0 )
		{
		//
		// both points are off the same side of the rectangle, so it's not visible...
		return FALSE;
		}

	if( (uwClip0|uwClip1) == 0 )
		{
		//
		// both points are within the clip rectangle, so it's completely visible...
		return TRUE;
		}

	//
	// Need to do some clipping...
	RIntSize	vecDir( pt1 - pt0 );
	//
	// Clip the first point.
	uwClip0	= ClipVector( pt0, vecDir, uwClip0, rcClip );
	if( uwClip0 != 0 )
		{
		//
		// Couldn't find an intersection with the clip rectangle, so there's nothing to
		// do...
		return FALSE;
		}
	//
	// Now the second point.  We know that since we found an intersection
	// for the first point, we WILL find an intersection for this one...
	vecDir.m_dx	= -vecDir.m_dx;
	vecDir.m_dy	= -vecDir.m_dy;
	uwClip1	= ClipVector( pt1, vecDir, uwClip1, rcClip );
	TpsAssert( uwClip1==0, "Couldn't find second intersection!" );
	//
	// it's visible!
	return TRUE;
}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::ClipLineSegment( RIntPoint& pt0, RIntPoint& pt1, const RIntRect& rcClip )
//
//  Description:		Clip the given linesegment
//
//  Returns:			TRUE if the resultant linesegment is visible else FALSE
//							pt0 and pt1 are modified as necessary...
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDrawingSurface::ClipLineSegment( RIntPoint& pt0, RIntPoint& pt1, const RIntRect& rcClip )
{
	return DoClipLineSegment( pt0, GetPointClipEdges(pt0,rcClip), pt1, GetPointClipEdges(pt1,rcClip), rcClip );
}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetCurrentPoint( const RIntPoint& pt, uWORD uwClipEdges )
//
//  Description:		Set the current point and its clip edges for the drawing surface
//
//  Returns:			TRUE if the point is considered visible (not clipped) else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDrawingSurface::SetCurrentPoint( const RIntPoint& pt, uWORD uwClipEdges )
{
	m_ptCurrent				= pt;
	m_uwCurrentPointClip	= uwClipEdges;
	return BOOLEAN( uwClipEdges==0 );
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::SetCurrentPoint( const RIntPoint& pt )
//
//  Description:		Set the current point for the drawing surface
//
//  Returns:			TRUE if the point is visible else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDrawingSurface::SetCurrentPoint( const RIntPoint& pt )
{
	m_ptCurrent	= pt;
	if( IsPoint32(pt) )
		m_uwCurrentPointClip	= GetPointClipEdges( pt, GetClipRect() );
	else
		m_uwCurrentPointClip = 0;
	return BOOLEAN(m_uwCurrentPointClip==0);
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::ClipRectangle( RIntRect& rc, const RIntRect& rcClip )
//
//  Description:		Clip the rectangle to the given clip rectangle
//
//  Returns:			TRUE if the rectangle is visible else FALSE
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDrawingSurface::ClipRectangle( RIntRect& rc, const RIntRect& rcClip )
{
	rc.m_Left	= Max( rc.m_Left, rcClip.m_Left );
	rc.m_Right	= Min( rc.m_Right, rcClip.m_Right );
	rc.m_Top		= Max( rc.m_Top, rcClip.m_Top );
	rc.m_Bottom	= Min( rc.m_Bottom, rcClip.m_Bottom );
	return BOOLEAN( rc.m_Left<rc.m_Right && rc.m_Top<rc.m_Bottom );
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::Polyline32(const RIntPoint* pPoints, YPointCount pointCount)
//
//  Description:		Draw the given polyline assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::Polyline32(const RIntPoint* pPoints, YPointCount pointCount )
	{
	//
	// make sure we have something to draw...
	if( pointCount < 1 )
		{
		//
		// nope, so just return...
		return;
		}

	//
	// Set the current point...
	MoveTo( *pPoints );
	//
	// Draw the rest of it...
	PolylineTo32( pPoints+1, pointCount-1 );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::PolylineTo32(const RIntPoint* pPoints, YPointCount pointCount)
//
//  Description:		Continue the given polyline assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::PolylineTo32( const RIntPoint* pPoints, YPointCount numPoints )
	{
	//
	// Draw the line segments...
	while( numPoints-- > 0 )
		LineTo( *pPoints++ );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::PolyPolyline32(const RIntPoint* pPoints, YPointCount* pointCountArray, YPolylineCount polylineCount )
//
//  Description:		Draw the given polypolyline assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::PolyPolyline32( const RIntPoint* pPoints, YPointCount* pointCountArray, YPolylineCount polylineCount )
	{
	while( polylineCount-- )
		{
		Polyline32( pPoints, *pointCountArray );
		pPoints += *pointCountArray;
		++pointCountArray;
		}
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::Polygon32( const RIntPoint* pVertices, YPointCount numVertices )
//
//  Description:		Fill and Frame the given polygon assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::Polygon32( const RIntPoint* pVertices, YPointCount numVertices )
	{
	FillPolygon32( pVertices, numVertices );
	FramePolygon32( pVertices, numVertices );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FramePolygon32( const RIntPoint* pVertices, YPointCount numVertices )
//
//  Description:		Frame the given polygon assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FramePolygon32( const RIntPoint* pVertices, YPointCount numVertices )
	{
	Polyline32( pVertices, numVertices );
	//
	// Close it...
	if( numVertices > 0 && pVertices[numVertices-1] != pVertices[0] )
		LineTo( pVertices[numVertices-1] );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillPolygon32( const RIntPoint* pVertices, YPointCount numVertices )
//
//  Description:		Fill the given polygon assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillPolygon32( const RIntPoint* pVertices, YPointCount numVertices )
	{
	FillPolyPolygon32( pVertices, &numVertices, 1 );
	
	if ( m_fColoringBookModeOn )
		{
		FramePolygon32( pVertices, numVertices );
		}
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::InvertPolygon32( const RIntPoint* pVertices, YPointCount numVertices )
//
//  Description:		Invert the given polygon assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::InvertPolygon32( const RIntPoint* pVertices, YPointCount numVertices )
	{
	UnimplementedCode();
	numVertices;
	pVertices;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::PolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
//
//  Description:		Frame and Fill the given polypolygon assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::PolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	FillPolyPolygon32( pVertices, pointCountArray, polygonCount );
	FramePolyPolygon32( pVertices, pointCountArray, polygonCount );
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FramePolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
//
//  Description:		Frame the given polypolygon assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FramePolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	while( polygonCount-- )
		{
		YPointCount	nPoints	= *pointCountArray++;
		Polyline32( pVertices, nPoints );
		//
		// Close it...
		if( nPoints>0 && pVertices[0]!=pVertices[nPoints-1] )
			LineTo( pVertices[0] );
		pVertices += nPoints;
		}
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::FillPolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
//
//  Description:		Fill the given polypolygon assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::FillPolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	//
	// Get the clip polygon...
	RIntPoint	ptClipPoly[ 4 ];
	YPointCount	nClipPoints	= 4;
	CreateRectPoly( GetClipRect(), &ptClipPoly[ 0 ] );
	//
	// Get a global buffer to hold the clipped points...
	uBYTE*	pBuffer	= ::GetGlobalBuffer();
	if( pBuffer )
		{
		YPolygonCount	ncntMaxClipped	= 100;
		uLONG				nBufferSize		= ::GetGlobalBufferSize();
		uLONG				nCountSize		= uLONG(ncntMaxClipped * sizeof(YPointCount));
		uLONG				nPointSize		= nBufferSize - nCountSize;
		RIntPoint*		pptClipped		= (RIntPoint*)pBuffer;
		YPointCount*	pcntClipped		= (YPointCount*)(pBuffer + nPointSize);
		YPointCount		nptMaxClipped	= YPointCount( nPointSize / sizeof(RIntPoint) );
		YPolygonCount	nClipped			= ClipPolygon( pVertices, pointCountArray, polygonCount, ptClipPoly, &nClipPoints, 1,
																	pptClipped, nptMaxClipped, pcntClipped, ncntMaxClipped );
		if( nClipped>0 )
			FillPolyPolygon( pptClipped, pcntClipped, nClipped );
		::ReleaseGlobalBuffer( pBuffer );
		}

	if ( m_fColoringBookModeOn )
		{
		FramePolyPolygon32( pVertices, pointCountArray, polygonCount );
		}
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::InvertPolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
//
//  Description:		Invert the given polypolygon assuming it contains 32bit coordinates
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::InvertPolyPolygon32( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	UnimplementedCode();
	polygonCount;
	pointCountArray;
	pVertices;
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetActualFontDimensions()
//
//  Description:		return the actual font dimensions used when output
//
//  Returns:			RIntSize dimensions
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::IntersectClipRect( const RRealRect& clipRect, const R2dTransform& transform )
{
	//	Copy to be able to adjust it...
	RRealVectorRect rect( clipRect );

	// Convert it to device units
	rect *= transform;

	// Intersect the rect with the existing clip rect
	RIntVectorRect	iRect( rect );
	IntersectClipVectorRect( iRect );
}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetActualTextSize( YCounter nChars, uBYTE* pChars )
//
//  Description:		return the width and height of the given characters if drawn using the current font
//
//  Returns:			RIntSize dimensions
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RDrawingSurface::GetActualTextSize( YCounter nChars, const uBYTE* pChars )
	{
	UnimplementedCode();
	nChars;
	pChars;
	return RIntSize(0,0);
	}

// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetActualFontDimensions()
//
//  Description:		return the actual font dimensions used when output
//
//  Returns:			RIntSize dimensions
//
//  Exceptions:		None
//
// ****************************************************************************
//
YFontDimensions RDrawingSurface::GetActualFontDimensions()
	{
	UnimplementedCode();
	YFontDimensions	yDim;
	yDim.m_Overhang	= 0;
	yDim.m_Ascent		= 0;
	yDim.m_Descent		= 0;
	yDim.m_Leading		= 0;
	return yDim;
	}

//
// @MFunc Draw text within the given rectangle
//
// @RDesc nothing
//
void RDrawingSurface::PlaceTextInRect(
			const RRealRect&		rcLogical,	// @Parm the logical rectangle to draw in
			const RMBCString&		szText,		// @Parm the text to draw
			const R2dTransform&	transform,	// @Parm the transformation to use
			uWORD						uwOptions,	// @Parm where in the rectangle to draw the text
			RRealRect*				prcOutput )	// @Parm optional storage for the output bounding rectangle
	{
	if( szText.GetStringLength() < 1 )
		{
		if( prcOutput )
			*prcOutput = RRealRect(RRealSize(0.0,0.0));
		return;
		}

	R2dTransform		xformText;
	YAngle				rotation;
	YRealDimension		xScale;
	YRealDimension		yScale;
	transform.Decompose( rotation, xScale, yScale );
	xformText.PreRotateAboutOrigin( rotation );

	RRealVectorRect	rcDevice( RRealVectorRect(rcLogical) * transform );
	RIntSize				textDeviceSize	= GetActualTextSize( szText.GetStringLength(), szText );
	YFontDimensions	fdDevice			= GetActualFontDimensions();
	//
	// Compute the start point offset...
	YRealDimension	yOffset;
	YRealDimension	xOffset;

	switch( uwOptions&kPlaceTextVerticalPos )
		{
		default:
		case kPlaceTextTop:
			yOffset	= YRealDimension(fdDevice.m_Ascent);
			break;

		case kPlaceTextBottom:
			yOffset	= rcDevice.Height() - fdDevice.m_Descent;
			if( uwOptions & kPlaceTextWithLeading )
				yOffset -= YRealDimension(fdDevice.m_Leading);
			break;

		case kPlaceTextCenterV:
			{
			YRealDimension	yFontHeight	= YRealDimension(fdDevice.m_Ascent + fdDevice.m_Descent);
			if( uwOptions & kPlaceTextWithLeading )
				yFontHeight += YRealDimension( fdDevice.m_Leading );
			YRealDimension	yFontCenter	= YRealDimension( fdDevice.m_Ascent - yFontHeight/2.0 );
			yOffset = (rcDevice.Height()/2.0) + yFontCenter;
			break;
			}
		}
	switch( uwOptions&kPlaceTextHorizontalPos )
		{
		default:
		case kPlaceTextLeft:
			xOffset	= 0.0;
			break;

		case kPlaceTextRight:
			xOffset	= rcDevice.Width() - textDeviceSize.m_dx;
			break;

		case kPlaceTextCenterH:
			xOffset	= (rcDevice.Width()-textDeviceSize.m_dx)/2.0;
			break;
		}

	//	If printing, check if we are imaging across the clip region (which will not
	//		work for PCL drivers.  If that is the case, we must render the characters
	//		individually.
	BOOLEAN	fUseGlyphs = FALSE;
	if ( IsPrinting( ) && dynamic_cast<RPrinterDrawingSurface*>(this) )
	{
		RIntRect	clipRect		= GetClipRect( );
		RIntRect	outputRect	= rcDevice.m_TransformedBoundingRect;
		clipRect.Intersect( outputRect, clipRect );
		RPrinterDrawingSurface*	pPrinterSurface = static_cast<RPrinterDrawingSurface*>( this );
		if ( !pPrinterSurface->fIsPostScript() && (outputRect != clipRect) )
			fUseGlyphs	= TRUE;
	}

	//	If _DrawCharactersAsGlyphs returns FALSE, I must try to just draw the characters
	if ( fUseGlyphs )
		fUseGlyphs = _DrawCharactersAsGlyphs( *this, rcDevice.m_TopLeft + RRealSize(xOffset,yOffset) * xformText, szText.GetStringLength(), szText );
	if ( !fUseGlyphs )
		DrawCharacters( rcDevice.m_TopLeft + RRealSize(xOffset,yOffset) * xformText, szText.GetStringLength(), szText );
	if( prcOutput )
		{
		RRealSize		logSize( textDeviceSize.m_dx/xScale, textDeviceSize.m_dy/yScale );
		YRealDimension	yLogAscent( fdDevice.m_Ascent/yScale );
		RRealSize		logOffset( xOffset/xScale, yOffset/yScale - yLogAscent );
		*prcOutput	= RRealRect( rcLogical.m_TopLeft+logOffset, logSize );
		}
	}


BOOLEAN _DrawCharactersAsGlyphs( RDrawingSurface& ds, const RIntPoint& topLeft, YCounter count, LPCUBYTE pszText )
	{
	BOOLEAN	fRetVal		= TRUE;
	BOOLEAN	fColoring	= ds.GetColoringBookMode( );
	RFont*	pFont			= NULL;
	RPath*	pGlyphPath	= NULL;
 	RIntRect	rRender(-16000, -16000, 16000, 16000 );

	try
		{
		//	Preserve the current surface state.
		RDrawingSurfaceState	CurState( &ds );
		ds.SetColoringBookMode( FALSE );

		//	Set the rendering colors.
		ds.SetFillColor( ds.GetFontColor( ) );
		ds.SetPenColor( ds.GetFontColor( ) );

		//	Create the font...
		const YRealDimension	scaleFactor	= 10.;
		YFontInfo	fontInfo = ds.GetFont( );
		fontInfo.height	*=	scaleFactor;
		pFont	= RFont::CreateFont( fontInfo, FALSE );

		R2dTransform	transform;
		//	Setup transform to account for initial x,y offset including ascent
		transform.PreTranslate( topLeft.m_x, topLeft.m_y );
		//	Account for paths being Cartisian based.
		transform.PreScale( 1, -1 );
		//	acount for above 10* scale
		transform.PreScale( 1./scaleFactor, 1./scaleFactor );

		//	For each character in the buffer,
		for ( int i = 0; i < count; ++i )
			{
			RCharacter character( pszText[i] );
			pGlyphPath	= pFont->GetGlyphPath( character );
			if (pGlyphPath)	//	Can't get glyph path, assume it is a space character
				{
				R2dTransform	renderTransform( transform );
				pGlyphPath->UndefineLastTransform( );
				pGlyphPath->Render( ds, renderTransform, rRender, kFilledAndStroked );
				}
			//	
			//	Adjust transform for next character
			transform.PreTranslate( pFont->GetCharacterWidth(character), 0 );
			}
		}
	catch( ... )
		{
		fRetVal = FALSE;
		}

	ds.SetColoringBookMode( fColoring );
	delete pFont;
	return fRetVal;
	}


// ****************************************************************************
//
//  Function Name:	RDrawingSurface::CopySurfaceAttributes
//
//  Description:		Copies the attributes of the specified surface into this
//							surface.
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDrawingSurface::CopyAttributes( const RDrawingSurface& drawingSurface )
	{
	SetTint( drawingSurface.GetTint( ) );
	SetColoringBookMode( drawingSurface.GetColoringBookMode() );
	SetFullPanelBitmapPrintingMode( drawingSurface.GetFullPanelBitmapPrintingMode() );

	// REVIEW - STA - Copy other attributes here
	}



// ****************************************************************************
//
//  Function Name:	RDrawingSurface::GetGradientPrecision( )
//
//  Description:		Accessor
//
//  Returns:			Percent precision that the current gradient will be rendered at
//
//  Exceptions:		None
//
// ****************************************************************************
//
YPercentage RDrawingSurface::GetGradientPrecision( ) const
	{
	if( IsPrinting( ) )
		return ::GetGradientPrintPrecision();
	else
		return ::GetGradientDisplayPrecision();
	}

//
// Surface state support
//

// ****************************************************************************
//
//  Function Name:	RSurfaceState::RSurfaceState( RDrawingSurface* pDrawingSurface )
//
//  Description:		Save the state of the given drawing surface
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSurfaceState::RSurfaceState( RDrawingSurface* pDrawingSurface ) :
	m_pSurface( NULL ),
	m_pGradientFill( NULL )
{
	TpsAssert( pDrawingSurface!=NULL, "Attempt to save the state of a NULL drawing surface" );
	m_pSurface					= pDrawingSurface;
	m_PenColor					= m_pSurface->GetPenColor();
	m_PenWidth					= m_pSurface->GetPenWidth();
	m_PenStyle					= m_pSurface->GetPenStyle();
	m_ForegroundMode			= m_pSurface->GetForegroundMode();
	m_FillColor					= m_pSurface->GetFillColor();
	m_FontColor					= m_pSurface->GetFontColor();
	m_fFontSet					= m_pSurface->m_fFontSet;
	if( m_fFontSet )
		m_FontInfo				= m_pSurface->GetFont();
}

// ****************************************************************************
//
//  Function Name:	RSurfaceState::~RSurfaceState( )
//
//  Description:		does nothing.  the state must have already been restored
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSurfaceState::~RSurfaceState()
{
	TpsAssert( m_pSurface==NULL && m_pGradientFill==NULL, "Deleting an unrestored surface state" );
}


// ****************************************************************************
//
//  Function Name:	RSurfaceState::Restore( )
//
//  Description:		Restore the state of the drawing surface
//
//  Returns:			nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RSurfaceState::Restore()
{
	TpsAssert( m_pSurface!=NULL, "Attempt to restore an uninitialized surface state" );
	m_pSurface->SetPenColor( m_PenColor );
	m_pSurface->SetPenWidth( m_PenWidth );
	m_pSurface->SetPenStyle( m_PenStyle );
	m_pSurface->SetForegroundMode( m_ForegroundMode );
	m_pSurface->SetFillColor( m_FillColor );
	m_pSurface->SetFontColor( m_FontColor );
	if( m_fFontSet )
		m_pSurface->SetFont( m_FontInfo );
	else
		m_pSurface->m_fFontSet	= FALSE;
	//
	// so we don't try to restore again!
	m_pSurface			= NULL;
}


//
// @Func Get the device width of the given logical pen width
//
// @RDesc the device width of the pen
//
// @Comm unless the logical width less than or equal to 0.0, this will
//       return a minimum pen width of 1
//
YPenWidth TransformedPenWidth(
			YRealDimension			logicalWidth,	// @Parm the logical width of the pen
			const R2dTransform&	transform )		// @Parm the transform that will be used during output
{
	if( logicalWidth <= 0.0 )
		return YPenWidth(0);

	RRealSize			szPen( logicalWidth, logicalWidth );
	RRealRect			rcPen( szPen );
	RRealVectorRect	rcvecPen( RRealVectorRect(rcPen) * transform );
	RRealSize			szTransformedPen( rcvecPen.WidthHeight() );
	YIntDimension		nWidth( YIntDimension(::Max(szTransformedPen.m_dx,szTransformedPen.m_dy)) );
	return YPenWidth( ::Max(nWidth,YIntDimension(1)) );
}

