// ****************************************************************************
//
//  File Name:			MacPostScriptDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Definition of the RPostScriptDrawingSurface class
//
//  Portability:		Macintosh independent
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
//  $Logfile:: /PM8/Framework/Source/PostScriptDrawingSurface.cpp             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifdef MAC
	#include <stdio.h>
#endif

#include "FrameworkIncludes.h"

ASSERTNAME

#include "PostScriptDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#ifndef	IMAGER
#include "ApplicationGlobals.h"
#else
#include "GlobalBuffer.h"
#endif

#include "Bezier.h"
#include "QuadraticSpline.h"
#include "GradientFill.h"

/**  define postscript commands resource ids  **/
#ifdef TPSDEBUG
	#ifdef _WINDOWS
		#define	CMDSEP	"\n"
	#else
		#define	CMDSEP	"\r"
	#endif
#else
	#define	CMDSEP	" "
#endif

//
// comment out to use the dictionary for smaller file sizes...
#define	DEBUG_POSTSCRIPT


//
// set to TRUE to flush after each command
// set to FALSE to only flush when necessary
#define	FREQUENT_FLUSHER	FALSE


#if FREQUENT_FLUSHER
#define	FlushPostScript()	SendPostscript()
#else
#define	FlushPostScript()
#endif

#ifdef DEBUG_POSTSCRIPT
const	char	kszPrefix[]			=	"";
const	char	kszMoveTo[]			=	"moveto" CMDSEP;
const	char	kszLineTo[]			=	"lineto" CMDSEP;
const	char	kszCurveTo[]		=	"curveto" CMDSEP;
const	char	kszRMoveTo[]		=	"rmoveto" CMDSEP;
const	char	kszRLineTo[]		=	"rlineto" CMDSEP;
const	char	kszInitClip[]		=	"initclip" CMDSEP;
const	char	kszStroke[]			=	"stroke" CMDSEP;
const	char	kszFill[]			=	"fill" CMDSEP;
const	char	kszNewPath[]		=	"newpath" CMDSEP;
const	char	kszClosePath[]		=	"closepath" CMDSEP;
const	char	kszSetLineWidth[]	=	"setlinewidth" CMDSEP;
const	char	kszGRestore[]		=	"grestore" CMDSEP;
const	char	kszGSave[]			=	"gsave" CMDSEP;
const	char	kszClip[]			=	"clip" CMDSEP;
const	char	kszSetDash[]		=	"setdash" CMDSEP;
const	char	kszSetRGBColor[]	=	"setrgbcolor" CMDSEP;
const	char	kszDiv[]				=	"div" CMDSEP;
const	char	kszPostfix[]		=	"";
#else
const	char	kszPrefix[]			=	"/TPSDict 50 dict dup begin" CMDSEP
											"/bd { bind def } bind def" CMDSEP
											"/M { moveto } bd" CMDSEP
											"/L { lineto } bd" CMDSEP
											"/C { curveto } bd" CMDSEP
											"/RM { rmoveto } bd" CMDSEP
											"/RL { rlineto } bd" CMDSEP
											"/N { newpath } bd" CMDSEP
											"/CP { closepath } bd" CMDSEP
											"/S { stroke } bd" CMDSEP
											"/F { fill } bd" CMDSEP
											"/LW { setlinewidth } bd" CMDSEP
											"/GS { gsave } bd" CMDSEP
											"/GR { grestore } bd" CMDSEP
											"/CL { clip } bd" CMDSEP
											"/SD { setdash } bd" CMDSEP
											"/RGB { setrgbcolor } bd" CMDSEP
											"/D { div } bd" CMDSEP
											"end def" CMDSEP
											"TPSDict begin" CMDSEP;
const	char	kszMoveTo[]			=	"M" CMDSEP;
const	char	kszLineTo[]			=	"L" CMDSEP;
const	char	kszCurveTo[]		=	"C" CMDSEP;
const	char	kszRMoveTo[]		=	"RM" CMDSEP;
const	char	kszRLineTo[]		=	"RL" CMDSEP;
const	char	kszInitClip[]		=	"initclip" CMDSEP;
const	char	kszStroke[]			=	"S" CMDSEP;
const	char	kszFill[]			=	"F" CMDSEP;
const	char	kszNewPath[]		=	"N" CMDSEP;
const	char	kszClosePath[]		=	"CP" CMDSEP;
const	char	kszSetLineWidth[]	=	"LW" CMDSEP;
const	char	kszGRestore[]		=	"GR" CMDSEP;
const	char	kszGSave[]			=	"GS" CMDSEP;
const	char	kszClip[]			=	"CL" CMDSEP;
const	char	kszSetDash[]		=	"SD" CMDSEP;
const	char	kszSetRGBColor[]	=	"RGB" CMDSEP;
const	char	kszDiv[]				=	"D" CMDSEP;
const	char	kszPostfix[]		=	"end" CMDSEP;
#endif

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::~RPostScriptDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPostScriptDrawingSurface::~RPostScriptDrawingSurface( )
{
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::SetForegroundMode( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::SetForegroundMode( YDrawMode drawMode )
{
	// set mode 
	switch ( drawMode )
	{
		case kNormal:
			NULL;
			break;
		case kXOR:
			UnimplementedCode();
			break;
		default:
			TpsAssert( FALSE, "SetForegroundMode setting unknown");
			break;
	}

	// call the base method
	PostscriptBaseClass::SetForegroundMode( drawMode );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::SetPenWidth( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::SetPenWidth( YPenWidth penWidth )
{
	TpsAssert( (penWidth >= 1), "Setting a negative PenWidth");

	// no change no call 
	if ( penWidth == m_PenWidth )
		return;

	// set pen width 
	Append( penWidth );
	Append( kszSetLineWidth );
	FlushPostScript( );

	// call the base method
	PostscriptBaseClass::SetPenWidth( penWidth );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::SetPenStyle( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::SetPenStyle( YPenStyle penStyle )
{
	// no change no call 
	if ( penStyle == m_PenStyle )
		return;

	// set pen style 
	switch ( penStyle )
	{
		case kSolidPen:
			Append( "[] 0 " );
			break;
		case kDashPen:
			Append( "[4 2] 0 " );
			break;
		case kDotPen:
			Append( "[2 4] 0 " );
			break;
		case kDashDotPen:
			Append( "[6 4 2 4] 0 " );
			break;
		default:
			Append( "[] 0 " );
			TpsAssert( FALSE, "Invalid Pen Style");
			break;
	}
	Append( kszSetDash );
	FlushPostScript( );

	// call the base method
	PostscriptBaseClass::SetPenStyle( penStyle );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::SetPenColor( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::SetPenColor( const RColor& penColor )
{
	// no change no call 
	if ( penColor == m_PenColor )
		return;

	// mark pen color to be set 
	m_fPenDirty = TRUE;

	// call the base method
	PostscriptBaseClass::SetPenColor( penColor );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::SetFillColor( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::SetFillColor( const RColor& fillColor )
{
	// no change no call 
	if ( fillColor == m_FillColor )
		return;

	// mark fill color to be set 
	m_fBrushDirty = TRUE;

	// call the base method
	PostscriptBaseClass::SetFillColor( fillColor );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::BeginPage( )
//
//  Description:		Open print page. 
//
//  Returns:			Boolean indicating successful completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPostScriptDrawingSurface::BeginPage( )
{
#ifdef	MAC
	// call the base method
	if ( !PostscriptBaseClass::BeginPage( ) )
		return FALSE;
#endif	//	MAC

	// postscript for the duration
	StartupPostScript( );

	//
	// We're done...
	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::EndPage( )
//
//  Description:		Close print page.
//
//  Returns:			Boolean indicating successful completion
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RPostScriptDrawingSurface::EndPage( )
{
	//
	// send the postfix...
	Append( kszPostfix );
	//
	// make sure everything's sent...
	SendPostScript();
	//
	// done with postscript 
	ShutdownPostScript( );

#ifdef	MAC
	// call the base method
	PostscriptBaseClass::EndPage( );
#endif	//	MAC

	//
	// We're done...
	return TRUE;
}


// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::SetClipVectorRect( )
//
//  Description:		Sets a new clip rect that is the interesection of the
//							current clip rect and the given clip vector rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::SetClipVectorRect( const RIntVectorRect& clipRect )
	{
	//
	// Reset the current clipping rectangle
	Append( kszInitClip );
	//
	// Intersect the new rect with the default
	IntersectClipVectorRect( clipRect );
	}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::IntersectClipVectorRect( )
//
//  Description:		Sets a new clip rect that is the interesection of the
//							current clip rect and the given clip vector rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::IntersectClipVectorRect( const RIntVectorRect& clipRect )
	{
	// Get the polygon points
	RIntPoint polygon[ 4 ];
	clipRect.GetPolygonPoints( polygon );

	//
	// Begin the path
	BeginPath();
	//
	// Form the rectangle
	MoveTo( polygon[0] );
	LineTo( polygon[1] );
	LineTo( polygon[2] );
	LineTo( polygon[3] );
	//
	// Close the path
	EndPath();
	//
	// Intersect the current path with the current clip path
	Append( kszClip );
	//
	// Flush it
	FlushPostScript();
	}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::MoveTo( )
//
//  Description:		Changes the current drawing position to the given point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::MoveTo( const RIntPoint& point )
{
	// send postscript command 
	Append( point );
	Append( kszMoveTo );
	FlushPostScript( );

	// update current pen position
	m_ptPenPosition	= point; 
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::RMoveTo( )
//
//  Description:		Changes the current drawing position to the given point
//							relative to the current drawing position.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::RMoveTo( const RIntPoint& point )
{
	// send postscript command 
	Append( point );
	Append( kszRMoveTo );
	FlushPostScript( );

	// update current pen position
	m_ptPenPosition += point; 
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::LineTo( )
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
void RPostScriptDrawingSurface::LineTo( const RIntPoint& point )
{
	// send postscript command 
	Append( point );
	Append( kszLineTo );

	// draw it if we are not in a path
	Stroke( point );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::RLineTo( )
//
//  Description:		Draws a line using the current pen attributes from the
//							current drawing position to the given point relative to 
//							the current drawing position. The drawing position is updated.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::RLineTo( const RIntPoint& point )
{
	// send postscript command 
	Append( point );
	Append( kszRLineTo );

	// draw it if we are not in a path
	Stroke( m_ptPenPosition	+ point );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::CurveTo( )
//
//  Description:		Draws a curve using the current pen attributes from the
//							current drawing position to the given point. The drawing
//							position is updated.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::CurveTo( const RIntPoint* pPoints )
{
	Append( pPoints[0] );
	Append( pPoints[1] );
	Append( pPoints[2] );
	Append( kszCurveTo );

	// draw it if we are not in a path
	Stroke( pPoints[2] );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FrameRectangle( )
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
void RPostScriptDrawingSurface::FrameRectangle( YIntCoordinate left, YIntCoordinate top, YIntCoordinate right, YIntCoordinate bottom )
{
sLONG			inset = ( ( m_PenWidth == 1 ) ? m_PenWidth : ( m_PenWidth >> 1 ) );
RIntRect		psRect( left, top, right, bottom );
RIntPoint	ptPen( m_ptPenPosition ); 									// current pen position

	TpsAssert( ( right > left ), "Empty rectangle." );
	TpsAssert( ( bottom > top ), "Empty rectangle." );

	// inset rect to frame inside
	psRect.Inset( RIntSize( inset, inset ) );							// REVEIW RAH is this correct?

	// open a new path
	BeginPath();
	m_fPolyOpen = TRUE;
	//
	// trace rectangle
	MoveTo( psRect.m_TopLeft );											
	LineTo( psRect.m_Right, psRect.m_Top );
	LineTo( psRect.m_Right, psRect.m_Bottom );
	LineTo( psRect.m_Left, psRect.m_Bottom );
	//
	// close the path
	EndPath();
	m_fPolyOpen = FALSE;
	//
	// frame rectangle
	Stroke( ptPen );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FillRectangle( )
//
//  Description:		Draws a filled rectangle using the current fill color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FillRectangle( YIntCoordinate left, YIntCoordinate top, YIntCoordinate right, YIntCoordinate bottom )
{
RIntPoint	ptPen( m_ptPenPosition ); 									// current pen position

	TpsAssert( ( right > left ), "Empty rectangle." );
	TpsAssert( ( bottom > top ), "Empty rectangle." );

	// open a new path
	BeginPath();
	m_fPolyOpen = TRUE;

	// trace rectangle
	MoveTo( left, top );
	LineTo( right, top );
	LineTo( right, bottom );
	LineTo( left, bottom );

	// close the path
	EndPath();
	m_fPolyOpen = FALSE;

	// fill rectandle
	CreateBrush( );
	Append( kszFill );

	// reset pen position 
	//	MoveTo will call FlushPostScript()
	MoveTo( ptPen );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::Polyline( )
//
//  Description:		Draws a batch of lines. Maintain current pen position.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::Polyline( const RIntPoint* pVertices, YPointCount numVertices )
{
RIntPoint			ptPen( m_ptPenPosition ); 									// current pen position 

	TpsAssert( ( numVertices > 1 ), "Lines need at least 2 points." );
	TpsAssert( ( numVertices < kMaxPoints ), "Too many points." );

	m_fPolyOpen = TRUE;
	MoveTo( *pVertices );
	Append( pVertices+1, numVertices-1, 1, kszLineTo );
	m_fPolyOpen = FALSE;

	// draw it if we are not in a path
	Stroke( ptPen );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::PolylineTo( )
//
//  Description:		Draws a batch of lines. Updates pen position.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::PolylineTo( const RIntPoint* pVertices, YPointCount numVertices )
{
	TpsAssert( ( numVertices > 0 ), "Need at least 1 point." );
	TpsAssert( ( numVertices < kMaxPoints ), "Too many points." );

	m_fPolyOpen	= TRUE;
	Append( pVertices, numVertices, 1, kszLineTo );
	m_fPolyOpen	= TRUE;
	// draw it if we are not in a path
	Stroke( pVertices[numVertices-1] );
}


// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::Polygon( )
//
//  Description:		Draws a filled, framed polygon using the current fill
//							color and pen attributes
//
//  Returns:			Nothing
//
//  Exceptions:		throws memory exception
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::Polygon( const RIntPoint* pVertices, YPointCount numVertices )
{
RIntPoint		ptPen( m_ptPenPosition ); 									// current pen position		 

	TpsAssert( ( numVertices < kMaxPoints ), "Too many points." );

	//
	// Create the polygon path
	BeginPath();
	MoveTo( pVertices[0] );
	Append( pVertices+1, numVertices-1, 1, kszLineTo );
	EndPath();
	//
	// Frame and fill it...
	FillAndFramePath();
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FramePolygon( )
//
//  Description:		Draws a polygon using the current pen attributes. The
//							polygon is not filled.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FramePolygon( const RIntPoint* pVertices, YPointCount numVertices )
{
RIntPoint		ptPen( m_ptPenPosition ); 									// current pen position 	

	TpsAssert( ( numVertices < kMaxPoints ), "Too many points." );

	//
	// Create the polygon path
	BeginPath();
	MoveTo( pVertices[0] );
	Append( pVertices+1, numVertices-1, 1, kszLineTo );
	EndPath();
	//
	// frame it...
	FramePath();
	//
	// reset pen position 
	MoveTo( ptPen );															
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FillPolygon( )
//
//  Description:		Draws a filled polygon using the current fill color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FillPolygon( const RIntPoint* pVertices, YPointCount numVertices )
{
RIntPoint		ptPen( m_ptPenPosition ); 									// current pen position 	

	TpsAssert( ( numVertices < kMaxPoints ), "Too many points." );

	//
	// Create the polygon path
	BeginPath();
	MoveTo( pVertices[0] );
	Append( pVertices+1, numVertices-1, 1, kszLineTo );
	EndPath();
	//
	// fill it
	FillPath();
	//
	// reset pen position 
	MoveTo( ptPen );															
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::PolyPolygon( )
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
void RPostScriptDrawingSurface::PolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
{
#if 1
	BOOLEAN	fInPath	= m_fPathOpen;
	if( !fInPath )
		BeginPath();
	for ( int i = 0; i < polygonCount; i++ )
		{
		int	k = pointCountArray[i];
		MoveTo( pVertices[0] );
		Append( pVertices+1, k-1, 1, kszLineTo );
		pVertices += k;
		}
	if( !fInPath )
		{
		EndPath();
		FillAndFramePath();
		}
#else
	for ( int i = 0; i < polygonCount; i++ )
	{
		int	k = pointCountArray[i];
		Polygon( pVertices, k );
		pVertices += k;
	}
#endif
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FramePolyPolygon( )
//
//  Description:		Draws framed polygons using the current pen attributes.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FramePolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
{
#if 1
	BeginOpenPath();
	for ( int i = 0; i < polygonCount; i++ )
		{
		int	k = pointCountArray[i];
		MoveTo( pVertices[0] );
		Append( pVertices+1, k-1, 1, kszLineTo );
		pVertices += k;
		}
	EndOpenPath();
	FramePath();
#else
	for ( int i = 0; i < polygonCount; i++ )
	{
		int	k = pointCountArray[i];
		FramePolygon( pVertices, k );
		pVertices += k;
	}
#endif
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FillPolygon( )
//
//  Description:		Draws filled polygons using the current fill color.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
{
#if 1
	BeginOpenPath();
	for ( int i = 0; i < polygonCount; i++ )
		{
		int	k = pointCountArray[i];
		MoveTo( pVertices[0] );
		Append( pVertices+1, k-1, 1, kszLineTo );
		pVertices += k;
		}
	EndOpenPath();
	FillPath();
#else
	for ( int i = 0; i < polygonCount; i++ )
	{
		int	k = pointCountArray[i];
		FillPolygon( pVertices, k );
		pVertices += k;
	}
#endif
}
												
// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::PolyBezier( )
//
//  Description:		Frames and Fills a polybezier (nsegments*3)+1 points required
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::PolyBezier( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( numPoints>1 && ((numPoints-1)%3)==0, "Incorrect number of points passed to PolyBezier" );

	//
	// see if it's closed...
	BeginOpenPath();
	MoveTo( *pPoints );
	Append( pPoints+1, numPoints-1, 3, kszCurveTo );
	EndOpenPath();
	//
	// do it...
	FillAndFramePath();
	}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FramePolyBezier( )
//
//  Description:		Frames a polybezier (nsegments*3)+1 points required
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FramePolyBezier( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( numPoints>1 && ((numPoints-1)%3)==0, "Incorrect number of points passed to PolyBezier" );

	BeginOpenPath();
	MoveTo( *pPoints );
	Append( pPoints+1, numPoints-1, 3, kszCurveTo );
	EndOpenPath();
	//
	// do it...
	FramePath();
	}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FillPolyBezier( )
//
//  Description:		Frames and Fills a polybezier (nsegments*3)+1 points required
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FillPolyBezier( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( numPoints>1 && ((numPoints-1)%3)==0, "Incorrect number of points passed to PolyBezier" );

	BeginOpenPath();
	MoveTo( *pPoints );
	Append( pPoints+1, numPoints-1, 3, kszCurveTo );
	EndOpenPath();
	FillPath();
	}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::BeginPath( )
//
//  Description:		Sends the PS command to begin a new path. 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::BeginPath( )
{
	TpsAssert( !m_fPathOpen, "Path nesting is illegal." );

	Append( kszNewPath );
	FlushPostScript( );

	m_fPathOpen = TRUE;
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::EndPath( )
//
//  Description:		Sends the PS command to close the open path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::EndPath( )
{
	TpsAssert( m_fPathOpen, "Path not opened." );

	Append( kszClosePath );
	FlushPostScript( );

	m_fPathOpen = FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::BeginOpenPath( )
//
//  Description:		Sends the PS command to begin a new path. 
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::BeginOpenPath( )
{
	TpsAssert( !m_fPathOpen, "Path nesting is illegal." );
	m_fPathOpen = TRUE;
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::EndOpenPath( )
//
//  Description:		Sends the PS command to close the open path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::EndOpenPath( )
{
	TpsAssert( m_fPathOpen, "Path not opened." );
	m_fPathOpen = FALSE;
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::IntersectClipPath( )
//
//  Description:		adds the current path to the current clipping region
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::IntersectClipPath( )
{
	TpsAssert( !m_fPathOpen, "Path still open on intersectclippath request." );
	Append( kszClip );
	FlushPostScript();
}


// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FillPath( )
//
//  Description:		Fills the current path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FillPath( )
{
	TpsAssert( !m_fPathOpen, "Path still open on draw request." );

	CreateBrush( );
	Append( kszFill );
	FlushPostScript( );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FramePath( )
//
//  Description:		Frames the current path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FramePath( )
{
	TpsAssert( !m_fPathOpen, "Path still open on draw request." );

	Stroke( m_ptPenPosition );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FrameAndFillPath( )
//
//  Description:		Frames and fills the current path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FrameAndFillPath( )
{
	TpsAssert( !m_fPathOpen, "Path still open on draw request." );
	//
	// Save the path...
	RDrawingSurfaceState	SavedState( this );
	//
	// Frame it...
	FramePath();
	//
	// Restore the path
	SavedState.Restore();
	//
	// Fill it...
	FillPath();
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::FillAndFramePath( )
//
//  Description:		Frames and fills the current path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::FillAndFramePath( )
{
	TpsAssert( !m_fPathOpen, "Path still open on draw request." );
	//
	// Save the path...
	RDrawingSurfaceState	SavedState( this );
	//
	// Fill it...
	FillPath();
	//
	// Restore the path
	SavedState.Restore();
	//
	// Frame it...
	FramePath();
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::PolyBezierTo( )
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
void RPostScriptDrawingSurface::PolyBezierTo( const RIntPoint* pPoints, YPointCount pointCount )
{
	TpsAssert( ( pointCount < kMaxPoints ), "Too many points." );
	TpsAssert( ( pointCount % 3 == 0 ), "There must be exactly 3 points for each bezier." );

	Append( pPoints, pointCount, 3, kszCurveTo );
	//
	// stroke it if we're not in a path...
	Stroke( pPoints[pointCount-1] );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::PolyQuadraticSplineTo( )
//
//  Description:		Compute a bezier and call the above PolyBezierTo.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::PolyQuadraticSplineTo( const RIntPoint* pPoints, YPointCount pointCount )
{
	RIntPoint	bezierPoints[4];
	RIntPoint	splinePoints[3];
	TpsAssert( ( pointCount < kMaxPoints ), "Too many points." );
	TpsAssert( ( pointCount % 2 == 0 ), "There must be exactly 3 points for each bezier." );

	m_fPolyOpen	= TRUE;

	splinePoints[2] = pPoints[-1];
	while ( pointCount > 0 )
	{
		splinePoints[0]	= splinePoints[2];
		splinePoints[1]	= *pPoints++;
		splinePoints[2]	= *pPoints++;
		CreateBezierFromSpline( &splinePoints[ 0 ], bezierPoints );
		Append( &bezierPoints[1], 3, 3, kszCurveTo );
		pointCount -= 2;
	}

	m_fPolyOpen	= FALSE;
	//
	// stroke it if we're not in a path
	Stroke( pPoints[pointCount-1] );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::SetColorRGB( )
//
//  Description:		Sends a PS set color command
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::SetColorRGB( RColor rgbColor )
{
	Append( rgbColor );
	Append( kszSetRGBColor );
	FlushPostScript( );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::CreatePen( )
//
//  Description:		Creates a new pen if the pen attributes have changed, 
//							otherwise does nothing.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::CreatePen( )
{
	// set pen color if dirty
	if ( m_fPenDirty )
	{
		SetColorRGB( m_PenColor );
		m_fPenDirty = FALSE;
	}

	//
	// since pen color is fill color is foreground color we must keep them in sync 
	m_fPenDirty	= BOOLEAN( m_PenColor!=m_FillColor );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::CreateBrush( )
//
//  Description:		Creates a new brush if the brush attributes have changed, 
//							otherwise does nothing.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::CreateBrush( )
{
	// set brush color if dirty
	if ( m_fBrushDirty )
	{
		SetColorRGB( m_FillColor );
		m_fBrushDirty = FALSE;
	}
	//
	// since pen color is fill color is foreground color we must keep them in sync 
	m_fPenDirty	= BOOLEAN( m_PenColor!=m_FillColor );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::Stroke( )
//
//  Description:		Draw the current path and update current pen position
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::Stroke( const RIntPoint& point )
{
	if( !m_fPathOpen && !m_fPolyOpen )
		{
		CreatePen( );
	
		Append( kszStroke );
		//
		// Reset the current position...
		MoveTo( point );
		}
	FlushPostScript( );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::Append( )
//
//  Description:		Append the args into the global buffer and increment m_pubBufferEnd
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::Append( const char* szCommand )
{
int	nStrSize			= strlen( szCommand );
int	nCommandSize	= int(m_pubCommandEnd - m_ubCommandBuffer);

	// define abrv macros
	if ( !m_fPrefixed )
	{
		m_fPrefixed = TRUE;
		Append( kszPrefix );
	}

	//	Check if the buffer would overflow, flush it is would
	if ( (nCommandSize + nStrSize) >= sizeof(m_ubCommandBuffer) )
		SendPostScript( );

	strcat( (char*)m_pubCommandEnd, szCommand );
	m_pubCommandEnd	+=	nStrSize;
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::Append( )
//
//  Description:		Append the args into the global buffer and increment m_pubBufferEnd
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::Append( const int nArg0 )
{
char	tmp[32];	//	there is currently no way to have a number larger than 32 chars
	WinCode( wsprintf( tmp, "%d ", nArg0 ) );
	MacCode( sprintf( tmp, "%d ", nArg0 ) );
	Append( tmp );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::Append( )
//
//  Description:		Append the args into the global buffer and increment m_pubBufferEnd
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::Append( const RColor& clr )
{
	RSolidColor solidColor = clr.GetSolidColor( );

	char tmp[64];
	WinCode(	wsprintf(tmp,"%d %d %s%d %d %s%d %d %s", solidColor.GetRed(), kMaxColorComponent, kszDiv, solidColor.GetGreen(), kMaxColorComponent, kszDiv, solidColor.GetBlue(), kMaxColorComponent, kszDiv) );
	MacCode(	sprintf(tmp,"%d %d%s %d %d%s %d %d%s ", solidColor.GetRed(), kMaxColorComponent, kszDiv, solidColor.GetGreen(), kMaxColorComponent, kszDiv, solidColor.GetBlue(), kMaxColorComponent, kszDiv) );
	Append( tmp );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::Append( )
//
//  Description:		Append the args into the global buffer and increment m_pubBufferEnd
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::Append( const RIntPoint& pt )
{
	Append( pt.m_x );
	Append( pt.m_y );
}

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::Append( )
//
//  Description:		Append the args into the global buffer and increment m_pubBufferEnd
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::Append( const RIntPoint* pPoints, YPointCount pointCount, YPointCount pointsPerCommand, const char* szCommand )
{
YPointCount	nCmdPoints = 0;

	TpsAssert( pointCount>0, "No points!" );
	TpsAssert( !szCommand || pointsPerCommand>0, "Points per command is less than 1" );

	while ( pointCount-- )
	{
		Append( *pPoints++ );
		if( szCommand && ++nCmdPoints==pointsPerCommand )
			{
			Append( szCommand );
			nCmdPoints	= 0;
			}
	}
	TpsAssert( nCmdPoints==0, "Incorrect number of points" );
	//
	// if we're stuffing commmands, we should also stroke it (heh heh)
	if( szCommand )
		Stroke( pPoints[-1] );
}


// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::SaveSurfaceState( )
//
//  Description:		saves the state of this Drawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSurfaceState* RPostScriptDrawingSurface::SaveSurfaceState()
{
	return new RPostScriptSurfaceState( this );
}


// ****************************************************************************
//
//  Function Name:	RPostScriptSurfaceState::RPostScriptSurfaceState( )
//
//  Description:		saves the state of the given PostScriptDrawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPostScriptSurfaceState::RPostScriptSurfaceState( RPostScriptDrawingSurface* pDrawingSurface ) :
	RSurfaceState( pDrawingSurface )
{
	m_fPathOpen			= pDrawingSurface->m_fPathOpen;
	m_fPolyOpen			= pDrawingSurface->m_fPolyOpen;
	m_fPostScripting	= pDrawingSurface->m_fPostScripting;
	m_fPrefixed			= pDrawingSurface->m_fPrefixed;
	m_ptPenPosition	= pDrawingSurface->m_ptPenPosition;
	pDrawingSurface->Append( kszGSave );
}


// ****************************************************************************
//
//  Function Name:	RPostScriptSurfaceState::Restore( )
//
//  Description:		restore the previously saved state
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptSurfaceState::Restore()
{
	TpsAssert( m_pSurface!=NULL, "Attempting to restore an uninitialized or already restored state" );

	RPostScriptDrawingSurface* pDrawingSurface = dynamic_cast<RPostScriptDrawingSurface*>( m_pSurface );
	TpsAssert( pDrawingSurface!=NULL, "PostScriptSurfaceState restoring the state of a non-postscript drawing surface" );
	RSurfaceState::Restore();

	pDrawingSurface->m_fPathOpen			= m_fPathOpen;
	pDrawingSurface->m_fPolyOpen			= m_fPolyOpen;
	pDrawingSurface->m_fPostScripting	= m_fPostScripting;
	pDrawingSurface->m_fPrefixed			= m_fPrefixed;
	pDrawingSurface->m_ptPenPosition		= m_ptPenPosition;
	pDrawingSurface->Append( kszGRestore );
}


#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RPostScriptDrawingSurface::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPostScriptDrawingSurface::Validate( ) const
{
	PostscriptBaseClass::Validate( );
	TpsAssertIsObject( RPostScriptDrawingSurface, this );
	MacCode( TpsAssert( m_pGrafPort, "NULL graf port." ) );
}

#endif	// TPSDEBUG
