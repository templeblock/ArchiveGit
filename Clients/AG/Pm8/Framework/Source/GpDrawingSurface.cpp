// ****************************************************************************
//
//  File Name:			GpDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				R. Hood
//
//  Description:		Declaration of the RGpDrawingSurface class
//
//  Portability:		Macintosh dependent
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
//  $Logfile:: /PM8/Framework/Source/GpDrawingSurface.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#ifdef IMAGER
#include "GlobalBuffer.h"
#else
#include "ApplicationGlobals.h"
#endif

#include "GpDrawingSurface.h"
#include	"GradientFill.h"
#include "BitmapImage.h"
#include	"ClipPolygon.h"
#include	"OffscreenDrawingSurface.h"

Pattern 		RGpDrawingSurface::m_PatternStyles[] = 
				{
					{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },			// kSolidPen
					{ 0xCC, 0x33, 0xCC, 0x33, 0xCC, 0x33, 0xCC, 0x33 },			//	kDashPen
					{ 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },			// kDotPen
					{ 0xCC, 0x33, 0xAA, 0x55, 0xCC, 0x33, 0xAA, 0x55 }				// kDashDotPen
				};


// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::Initialize( )
//
//  Description:		Initialization
//
//  Returns:			Boolean
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGpDrawingSurface::Initialize( const GrafPtr pPort ) 
{
RGBColor			rgbColor;

	TpsAssert( pPort, "NULL graf port ptr" );
	m_pGrafPort = pPort;
	::GetPort( &m_pSavedPort );
	::SetPort( m_pGrafPort );
	::PenSize( m_PenWidth, m_PenWidth ); 
	m_PenColor = RSolidColor( kBlack );
	m_FillColor = RSolidColor( kBlack );
	rgbColor = (RGBColor)m_FillColor.GetSolidColor( );
	::RGBForeColor( &rgbColor ); 
	::PenPat( &m_PatternStyles[kSolidPen] );	

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::CreatePen( )
//
//  Description:		Creates a new pen and selects into the dc if the pen
//							attributes have changed, otherwise does nothing.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::CreatePen( )
{
	// set pen color if dirty
	if ( m_fPenDirty )
	{
		RGBColor	rgbColor = (RGBColor)m_PenColor.GetSolidColor( );
		::RGBForeColor( &rgbColor ); 
		m_fPenDirty = FALSE;
	}

	// since pen color is fill color is foreground color we must keep them in sync 
	if (  m_PenColor != m_FillColor )
		m_fBrushDirty = TRUE;
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::CreateBrush( )
//
//  Description:		Creates a new brush and selects into the dc if the brush
//							attributes have changed, otherwise does nothing.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::CreateBrush( )
	{
	// set brush color if dirty
	if ( m_fBrushDirty )
		if ( m_FillColor.GetFillMethod( ) == RColor::kSolid )
		{
			RGBColor	rgbColor = (RGBColor)m_FillColor.GetSolidColor( );
			::RGBForeColor( &rgbColor ); 
			m_fBrushDirty = FALSE;
		}

	// since pen color is fill color is foreground color we must keep them in sync 
	if ( m_PenColor != m_FillColor )
		m_fPenDirty = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::CreateFont( )
//
//  Description:		Creates a new font and selects into the dc if the font has
//							been changed, otherwise does nothing.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::CreateFont( )
{
	TpsAssert( m_fFontSet, "Creating a font before it was set into the device." );

	if( m_fFontDirty )
	{
		//	Set the font name
		::TextFont( RFont::GetMacFontFamilyId( m_FontInfo.sbName ) );
		//	Set the size
		::TextSize( m_FontInfo.height );
		//	Set the style
		::TextFace( RFont::GetMacStyleBits( m_FontInfo.attributes ) );
		//	Set the flags that identify the font has been set and is no longer dirty
		m_fFontDirty	= FALSE;
	}
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::MoveTo( )
//
//  Description:		Changes the current drawing position to the given point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::MoveTo( const RIntPoint& point )
	{
	::MoveTo( point.m_x, point.m_y );
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SolidLineTo( )
//
//  Description:		Draws a solid line, using the updating the current point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SolidLineTo( const RIntPoint& point )
	{
	::LineTo( ( point.m_x - m_PenWidthOffset ), ( point.m_y - m_PenWidthOffset ) );
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SolidFillRectangle( )
//
//  Description:		Fills a solid rectangle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SolidFillRectangle( const RIntRect& rect )
	{
	Rect	qdRect;

	qdRect.top = rect.m_Top;
	qdRect.left = rect.m_Left;
	qdRect.bottom = rect.m_Bottom;
	qdRect.right = rect.m_Right;
	
	::FillRect( &qdRect, &qd.black );
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SolidPolyline( )
//
//  Description:		Draws a bunch of solid lines
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SolidPolyline( const RIntPoint* pPoints, YPointCount numPoints )
	{
	Point			ptPen;

	TpsAssert( ( numPoints > 1 ), "Lines need at least 2 points." );
	TpsAssert( ( numPoints < kMaxPoints ), "Too many points." );

	::GetPen( &ptPen );

	::MoveTo( ( pPoints->m_x - m_PenWidthOffset ), ( pPoints->m_y - m_PenWidthOffset ) );
	
	for ( int i = 1; i < numPoints; i++ )
	{
		pPoints++;
		::LineTo( ( pPoints->m_x - m_PenWidthOffset ), ( pPoints->m_y - m_PenWidthOffset ) );
	}

	::MoveTo( ptPen.h, ptPen.v );
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SolidPolylineTo( )
//
//  Description:		Draws a bunch of solid lines, using and updating the
//							current point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SolidPolylineTo( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( ( numPoints > 0 ), "Need at least 1 point." );
	TpsAssert( ( numPoints < kMaxPoints ), "Too many points." );

	for ( int i = 0; i < numPoints; i++ )
		{
		::LineTo( ( pPoints->m_x - m_PenWidthOffset ), ( pPoints->m_y - m_PenWidthOffset ) );
		pPoints++;
		}
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SolidPolygon( )
//
//  Description:		Fills and frames a solid polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SolidPolygon( const RIntPoint* pPoints, YPointCount numPoints )
	{
	PolyHandle		hPoly;

	TpsAssert( ( numPoints < kMaxPoints ), "Too many points." );

	if ( numPoints == 2 )
		Polyline( pPoints, numPoints );
	else
		{
		hPoly = CreatePolygon( pPoints, numPoints );
		
		CreateBrush( );
		::FillPoly( hPoly, &qd.black );

		CreatePen( );

		// offset polygon to simulate windows center pixel stroke
		if ( m_PenWidthOffset != 0 )	  
			::OffsetPoly( hPoly, -m_PenWidthOffset, -m_PenWidthOffset );

		::FramePoly( hPoly );
	
		::DisposeHandle( (Handle)hPoly );
		}
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SolidFillPolygon( )
//
//  Description:		Fills a solid polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SolidFillPolygon( const RIntPoint* pPoints, YPointCount numPoints )
	{
	PolyHandle		hPoly;

	TpsAssert( ( numPoints < kMaxPoints ), "Too many points." );

	if ( numPoints == 2 )
		{
		Polyline( pPoints, numPoints );										// REVEIW RAH should use fill color
		}
	else
		{
		hPoly = CreatePolygon( pPoints, numPoints );
	
		CreateBrush( );

		::FillPoly( hPoly, &qd.black );
	
		::DisposeHandle( (Handle)hPoly );
		}
	}
	
// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SolidPolyPolygon( )
//
//  Description:		Fills and frames a solid poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SolidPolyPolygon( const RIntPoint* pPoints, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	FillPolyPolygon( pPoints, pointCountArray, polygonCount );
	FramePolyPolygon( pPoints, pointCountArray, polygonCount );
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SolidFillPolyPolygon( )
//
//  Description:		Fills a solid poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SolidFillPolyPolygon( const RIntPoint* pPoints, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	PolyHandle		hPoly;
	int				k = *pointCountArray;

	if ( polygonCount > 1 )
		{
		hPoly = CreatePolygon( pPoints, k );
		for ( int i = 1; i < polygonCount; i++ )
			{
			pPoints += k;
			k = pointCountArray[i];
			AppendPolygonToPolygon( hPoly, pPoints, k );
			}
	
		::FillPoly( hPoly, &qd.black );
	
		::DisposeHandle( (Handle)hPoly );
		}
	else
		FillPolygon( pPoints, k );
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SolidFramePolyPolygon( )
//
//  Description:		Frames a solid poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SolidFramePolyPolygon( const RIntPoint* pPoints, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	for ( int i = 0; i < polygonCount; i++ )
		{
		int	k = pointCountArray[i];
		FramePolygon( pPoints, k );
		pPoints += k;
		}
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::DeviceInvertRectangle( )
//
//  Description:		Inverts a rectangle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::DeviceInvertRectangle( const RIntRect& rect )
	{
	Rect	qdRect;

	qdRect.top = rect.m_Top;
	qdRect.left = rect.m_Left;
	qdRect.bottom = rect.m_Bottom;
	qdRect.right = rect.m_Right;
	::InvertRect( &qdRect );
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::DeviceInvertPolygon( )
//
//  Description:		Inverts a polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::DeviceInvertPolygon( const RIntPoint* pPoints, YPointCount numPoints )
	{
	PolyHandle		hPoly;

	TpsAssert( ( numPoints < kMaxPoints ), "Too many points." );

	if ( numPoints == 2 )
		{
		TpsAssertAlways( "Cannot invert a 2point polygon" );
		}
	else
		{
		hPoly = CreatePolygon( pPoints, numPoints );
	
		::InvertPoly( hPoly );
	
		::DisposeHandle( (Handle)hPoly );
		}
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::DeviceInvertPolyPolygon( )
//
//  Description:		Inverts a poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::DeviceInvertPolyPolygon( const RIntPoint* pPoints, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	PolyHandle		hPoly;
	int				k = *pointCountArray;

	if ( polygonCount > 1 )
		{
		hPoly = CreatePolygon( pPoints, k );
		for ( int i = 1; i < polygonCount; i++ )
			{
			pPoints += k;
			k = pointCountArray[i];
			AppendPolygonToPolygon( hPoly, pPoints, k );
			}
		
		::InvertPoly( hPoly );
	
		::DisposeHandle( (Handle)hPoly );
		}
	else
		InvertPolygon( pPoints, k );
	}
	
// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SetForegroundMode( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SetForegroundMode( YDrawMode drawMode )
{
	switch ( drawMode )
	{
		case kNormal:
			::PenMode( patCopy );
			break;
		case kXOR:
			::PenMode( patXor );
			break;
		default:
			TpsAssert( FALSE, "SetForegroundMode setting unknown");
			break;
	}

	// Call the base method
	RDrawingSurface::SetForegroundMode( drawMode );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SetPenWidth( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SetPenWidth( YPenWidth penWidth )
{
	TpsAssert( (penWidth >= 0), "Setting a negative PenWidth");

	// set pen width 
	if ( penWidth != m_PenWidth )
	{
		::PenSize( penWidth, penWidth );					//	must be square for similarities to windows
		if ( penWidth > 1 )
			m_PenWidthOffset = penWidth / 2;
		else
			m_PenWidthOffset = 0;
	}

	// Call the base method
	RDrawingSurface::SetPenWidth( penWidth );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SetPenStyle( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SetPenStyle( YPenStyle penStyle )
{
	TpsAssert( (penStyle < (sizeof(m_PatternStyles) / sizeof(Pattern))), "Invalid Pen Style");

	// set pen style 
	if ( penStyle != m_PenStyle )
		::PenPat( &m_PatternStyles[penStyle] );

	// Call the base method
	RDrawingSurface::SetPenStyle( penStyle );
}


// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::GetDPI( )
//
//  Description:		Gets the DPI of the device
//
//  Returns:			A size object containing the device DPI
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RGpDrawingSurface::GetDPI( ) const
{
short			x, y;
RIntSize 	dpi;

	::ScreenRes( &x, &y );
	dpi.m_dx = x;
	dpi.m_dy = y;

	return dpi;
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::GetBitDepth( )
//
//  Description:		Gets the bit depth of the current screen device
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
uLONG RGpDrawingSurface::GetBitDepth() const
{
short			version = ( (CGrafPtr)m_pGrafPort )->portVersion;
uLONG			depth = 1;

	if ( ( version & 0x8000 ) != 0 )
		if ( ( version & 0x4000 ) != 0 )
		{
			GDHandle	hDevice = ::GetMainDevice();
			depth = (**(**hDevice).gdPMap).pixelSize;
		}
	return depth;
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::CreatePolygon( )
//
//  Description:		Create a polygon from a list of points
//
//  Returns:			Handle to a mac polygon structure
//
//  Exceptions:		throws memory exception
//
// ****************************************************************************
//
PolyHandle RGpDrawingSurface::CreatePolygon( const RIntPoint* pPoints, YPointCount numPoints )
{
PolyHandle				hPoly;
PolyPtr					pPoly;
PointPtr					pTempPoints;
Rect						*pQdRect;
YPointCount 			nrPoints = 1;

	TpsAssert( ( numPoints >= 3 ), "Polygons must have at least 3 vertices." );
	TpsAssert( ( numPoints < 8189 ), "Polygons must be less than 32k." );

	// create mac polygon handle
	if ( (hPoly = (PolyHandle)::NewHandle( (sizeof(Point) * numPoints) + sizeof(short) + sizeof(Rect) + (sizeof(Point) * 3) )) == NULL )
		throw kMemory;
	HLock( (Handle)hPoly );
	pPoly = *hPoly;

	// initialize bounding rect
	pQdRect = &(pPoly->polyBBox);
	pQdRect->top = pPoints->m_y;
	pQdRect->left = pPoints->m_x;
	pQdRect->bottom = pPoints->m_y;
	pQdRect->right = pPoints->m_x;

	// initialize point list
	pTempPoints = pPoly->polyPoints;
	pTempPoints->h = pPoints->m_x;
	pTempPoints->v = pPoints->m_y;

	// add vertices to poly point list 
	for ( int i = 1; i < numPoints; i++ )
	{
		pPoints++;
		if ( pTempPoints->h != pPoints->m_x || pTempPoints->v != pPoints->m_y )
		{
			pTempPoints++;
			pTempPoints->h = pPoints->m_x;
			pTempPoints->v = pPoints->m_y;
			if ( pTempPoints->v < pQdRect->top )						// update bounding box
				pQdRect->top = pTempPoints->v;
			if ( pTempPoints->h < pQdRect->left )
				pQdRect->left = pTempPoints->h;
			if ( pTempPoints->v > pQdRect->bottom )
				pQdRect->bottom = pTempPoints->v;
			if ( pTempPoints->h > pQdRect->right )
				pQdRect->right = pTempPoints->h;
			nrPoints++;
		}
	}

	// close polygon
	if ( pPoly->polyPoints->h != pPoints->m_x || pPoly->polyPoints->v != pPoints->m_y )
	{
		pTempPoints++;
		pTempPoints->h = pPoly->polyPoints->h;
		pTempPoints->v = pPoly->polyPoints->v;
		nrPoints++;
	}

	// a little debugging check
#ifdef TPSDEBUG
	pTempPoints++;
	pTempPoints->h = 0xA3;
	pTempPoints->v = 0xA3;
#endif

	pPoly->polySize = (sizeof(Point) * nrPoints) + sizeof(short) + sizeof(Rect);
	HUnlock( (Handle)hPoly );
	return hPoly;
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::CreatePolygonSlowly( )
//
//  Description:		Create a polygon from a list of points using toolbox calls
//							Usefull for debugging dolphins.
//
//  Returns:			Handle to a mac polygon structure
//
//  Exceptions:		throws memory exception
//
// ****************************************************************************
//
PolyHandle RGpDrawingSurface::CreatePolygonSlowly( const RIntPoint* pPoints, YPointCount numPoints )
{
const RIntPoint		*pTempPoints = pPoints;
PolyHandle				hPoly;

	TpsAssert( numPoints >= 3, "Polygons must have at least 3 vertices." );

	if ( (hPoly = ::OpenPoly( )) == NULL )
		throw kMemory;

	::MoveTo( pTempPoints->m_x, pTempPoints->m_y );
	for ( int i = 1; i < numPoints; i++ )
	{
		pTempPoints++;
		::LineTo( pTempPoints->m_x, pTempPoints->m_y );
	}
	if ( pPoints->m_x != pTempPoints->m_x || pPoints->m_y != pTempPoints->m_y )
		::LineTo( pPoints->m_x, pPoints->m_y );
	::ClosePoly( );

	return hPoly;
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::AppendPolygonToPolygon( )
//
//  Description:		Modify a polygon handle to append a polygon from a list of points  
//
//  Returns:			Nothing
//
//  Exceptions:		throws memory exception
//
// ****************************************************************************
//
void RGpDrawingSurface::AppendPolygonToPolygon( PolyHandle hPoly, const RIntPoint* pPoints, YPointCount numPoints )
{
PolyPtr					pPoly;
PointPtr					pTempPoints;
Point 					ptStart, ptEnd;
Rect						*pQdRect;
YPointCount 			nrPoints;

	TpsAssert( ( hPoly != NULL ), "Invalid polygon handle." );
	TpsAssert( ( GetHandleSize((Handle)hPoly) > 0 ), "Bad polygon handle." );
	TpsAssert( ( numPoints >= 3 ), "Polygons must have at least 3 vertices." );
	TpsAssert( ( numPoints < 8189 ), "Polygons must be less than 32k." );
	TpsAssert( ( (GetHandleSize((Handle)hPoly) + (sizeof(Point) * numPoints) + (sizeof(Point) * 2)) < 32768 ), "Polygons must be less than 32k." );

	// resize mac polygon handle
	::SetHandleSize( (Handle)hPoly, GetHandleSize((Handle)hPoly) + (sizeof(Point) * numPoints) + (sizeof(Point) * 2) );
	if ( ::MemError() != noErr )
		throw kMemory;

	// initialize local poly values
	HLock( (Handle)hPoly );
	pPoly = *hPoly;
	pQdRect = &(pPoly->polyBBox);
	nrPoints = (pPoly->polySize - sizeof(short) - sizeof(Rect)) / sizeof(Point);
	pTempPoints = pPoly->polyPoints + nrPoints;
	ptStart = *(pTempPoints - 1);
	ptEnd.h = pPoints->m_x;
	ptEnd.v = pPoints->m_y;
	pTempPoints->h = pPoints->m_x;
	pTempPoints->v = pPoints->m_y;
	nrPoints++;

	// add vertices to poly point list 
	for ( int i = 1; i < numPoints; i++ )
	{
		pPoints++;
		if ( pTempPoints->h != pPoints->m_x || pTempPoints->v != pPoints->m_y )
		{
			pTempPoints++;
			pTempPoints->h = pPoints->m_x;
			pTempPoints->v = pPoints->m_y;
			if ( pTempPoints->v < pQdRect->top )						// update bounding box
				pQdRect->top = pTempPoints->v;
			if ( pTempPoints->h < pQdRect->left )
				pQdRect->left = pTempPoints->h;
			if ( pTempPoints->v > pQdRect->bottom )
				pQdRect->bottom = pTempPoints->v;
			if ( pTempPoints->h > pQdRect->right )
				pQdRect->right = pTempPoints->h;
			nrPoints++;
		}
	}

	// close polygon
	if ( ptEnd.h != pPoints->m_x || ptEnd.v != pPoints->m_y )
	{
		pTempPoints++;
		pTempPoints->h = ptEnd.h;
		pTempPoints->v = ptEnd.v;
		nrPoints++;
	}
	pTempPoints++;
	pTempPoints->h = ptStart.h;
	pTempPoints->v = ptStart.v;
	nrPoints++;

	// a little debugging check
#ifdef TPSDEBUG
	pTempPoints++;
	pTempPoints->h = 0xA3;
	pTempPoints->v = 0xA3;
#endif

	pPoly->polySize = (sizeof(Point) * nrPoints) + sizeof(short) + sizeof(Rect);
	HUnlock( (Handle)hPoly );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::BeginPath( )
//
//  Description:		Paths. Not supported.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::BeginPath( )
{
	TpsAssert( NULL, "Paths not supported." );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::EndPath( )
//
//  Description:		Paths. Not supported.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::EndPath( )
{
	TpsAssert( NULL, "Paths not supported." );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::BeginOpenPath( )
//
//  Description:		Paths. Not supported.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::BeginOpenPath( )
{
	TpsAssert( NULL, "Paths not supported." );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::EndOpenPath( )
//
//  Description:		Paths. Not supported.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::EndOpenPath( )
{
	TpsAssert( NULL, "Paths not supported." );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::FillPath( )
//
//  Description:		Paths. Not supported.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::FillPath( )
{
	TpsAssert( NULL, "Paths not supported." );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::FramePath( )
//
//  Description:		Paths. Not supported.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::FramePath( )
{
	TpsAssert( NULL, "Paths not supported." );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::FrameAndFillPath( )
//
//  Description:		Paths. Not supported by default.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::FrameAndFillPath( )
{
	TpsAssert( NULL, "Paths not supported." );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::FillAndFramePath( )
//
//  Description:		Paths. Not supported by default.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::FillAndFramePath( )
{
	TpsAssert( NULL, "Paths not supported." );
}


// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::IntersectClipPath( )
//
//  Description:		Paths. Not supported by default.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::IntersectClipPath( )
{
	TpsAssert( NULL, "Paths not supported." );
}



// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::IsVisible( const RIntPoint& pt )
//
//  Description:		return TRUE if the point is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGpDrawingSurface::IsVisible( const RIntPoint& pt )
	{
	Point		point;
	GrafPtr	pGrafPort = (GrafPtr)GetSurface();
	TpsAssert( ( pGrafPort != NULL ), "Nil surface device pointer." );
	point.h	= pt.m_x;
	point.v	= pt.m_y;
	return ::PtInRgn( point, pGrafPort->clipRgn );
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::IsVisible( const RIntRect& rc )
//
//  Description:		return TRUE if the rect is visible else FALSE
//
//  Returns:			BOOL fVisible
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RGpDrawingSurface::IsVisible( const RIntRect& rc )
	{
	Rect		rect;
	GrafPtr	pGrafPort = (GrafPtr)GetSurface();
	TpsAssert( ( pGrafPort != NULL ), "Nil surface device pointer." );
	::SetRect( &rect, rc.m_Left, rc.m_Top, rc.m_Right, rc.m_Bottom );
	return ::RectInRgn( &rect, pGrafPort->clipRgn );
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::GetClipRect( )
//
//  Description:		Gets the tightest bounding rectangle around the current
//							clipping region.
//
//  Returns:			A rect containing the current clip rect.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RGpDrawingSurface::GetClipRect( ) const
{
GrafPtr		pGrafPort = (GrafPtr)GetSurface();
Rect 			clipRect = (**pGrafPort->clipRgn).rgnBBox;
RIntRect 	rect( clipRect.left, clipRect.top, clipRect.right, clipRect.bottom );
	TpsAssert( ( pGrafPort != NULL ), "Nil surface device pointer." );

	return rect;
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SetClipRect( )
//
//  Description:		Sets the current clip rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SetClipRect( const RIntRect& clipRect )
{
GrafPtr		pGrafPort = (GrafPtr)GetSurface();
GrafPtr		holdPort;
Rect			qdRect;

	TpsAssert( ( pGrafPort != NULL ), "Nil surface device pointer." );
	TpsAssert( ( !clipRect.IsEmpty() ), "Empty clip rect." );
	::GetPort( &holdPort );   
	::SetPort( pGrafPort );
	::SetRect( &qdRect, clipRect.m_Left, clipRect.m_Top, clipRect.m_Right, clipRect.m_Bottom );
	::ClipRect( &qdRect );
	::SetPort( holdPort );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::IntersectClipRect( )
//
//  Description:		Sets a new clip rect that is the interesection of the
//							current clip rect and the given clip rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::IntersectClipRect( const RIntRect& newRect )
{
RIntRect		clipRect = GetClipRect();

	clipRect.Intersect( clipRect, newRect );
	SetClipRect( clipRect );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SetClipVectorRect( )
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
void RGpDrawingSurface::SetClipVectorRect( const RIntVectorRect& vectRect )
{
	SetClipRect( vectRect.m_TransformedBoundingRect );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::IntersectClipVectorRect( )
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
void RGpDrawingSurface::IntersectClipVectorRect( const RIntVectorRect& vectRect )
{
RIntRect		clipRect = GetClipRect();
RIntRect		newRect = vectRect.m_TransformedBoundingRect;

	clipRect.Intersect( clipRect, newRect );
	SetClipRect( clipRect );
}
												
// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::DrawFocusRect()
//
//  Description:		Image the characters to the the Drawing Surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::DrawFocusRect( const RIntRect rcRect )
{
GrafPtr		pGrafPort = (GrafPtr)GetSurface();

	TpsAssert( ( pGrafPort != NULL ), "Nil surface device pointer." );

	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::DrawCharacters( )
//
//  Description:		Image the characters to the the Drawing Surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::DrawCharacters( const RIntPoint& point, YCounter count, LPCUBYTE pCharacters, int* pWidths )
{
	TpsAssert( pCharacters, "No Characters were passed to DrawCharacters" );

	//CreateFont( );

	UnimplementedCode();
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::GetTextSize( )
//
//  Description:		return the dimensions of the given text if drawn to the surface
//
//  Returns:			RIntSize dimensions
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RGpDrawingSurface::GetActualTextSize( YCounter nChars, const uBYTE* pChars )
{
	UnimplementedCode();
	return RIntSize( 0, 0 );
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::GetActualFontDimensions()
//
//  Description:		return the actual font dimensions used when output
//
//  Returns:			RIntSize dimensions
//
//  Exceptions:		None
//
// ****************************************************************************
//
YFontDimensions RGpDrawingSurface::GetActualFontDimensions()
{
	YFontDimensions	fd;
	UnimplementedCode();
	fd.m_Overhang	= 0;
	fd.m_Ascent		= 0;
	fd.m_Descent	= 0;
	fd.m_Leading	= 0;
	return fd;
}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::BlitDrawingSurface( )
//
//  Description:		Copy from one the given drawing surface to this one
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RGpDrawingSurface::BlitDrawingSurface( const RDrawingSurface& rSrcDS, RIntRect rSrcRect, RIntRect rDestRect, EBlitMode eBlitMode )
{
short			mode;
Rect			srcRect, destRect;
GrafPtr		pSrcGrafPort, pDestGrafPort;
GrafPtr		savePort;
RGBColor		whiteColor, blackColor, foreColor, backColor;

	TpsAssertValid( this );
	TpsAssertIsObject( RGpDrawingSurface, this );
	TpsAssertIsObject(RGpDrawingSurface, &rSrcDS);

	pSrcGrafPort = (GrafPtr)rSrcDS.GetSurface();
	pDestGrafPort = (GrafPtr)GetSurface();
	TpsAssert( ( pSrcGrafPort != NULL ), "Source device is nil.");
	TpsAssert( ( pDestGrafPort != NULL ), "Destination device is nil.");

	// Set the port to the dest port
	GetPort(&savePort);
	SetPort(pDestGrafPort);

	switch ( eBlitMode )
	{
		case kBlitSourceCopy :
			mode = srcCopy;
			break;
		case kBlitSourceAnd :
			mode = srcBic;
			break;
		case kBlitSourcePaint :
			mode = srcOr;
			break;
		case kBlitNotSourceAnd :
			mode = notSrcBic;
			break;
		case kBlitMergePaint :
			mode = notSrcOr;
			break;
		case kBlitSourceErase :
		default:
			TpsAssertAlways( "Invalid blit mode" );
			break;
	}
	::SetRect( &srcRect, rSrcRect.m_Left, rSrcRect.m_Top, rSrcRect.m_Right, rSrcRect.m_Bottom );
	::SetRect( &destRect, rDestRect.m_Left, rDestRect.m_Top, rDestRect.m_Right, rDestRect.m_Bottom );

	::GetBackColor( &backColor );
	::GetForeColor( &foreColor );
	whiteColor.red = 0xFFFF;
	whiteColor.green = 0xFFFF;
	whiteColor.blue = 0xFFFF;
	blackColor.red = 0x0000;
	blackColor.green = 0x0000;
	blackColor.blue = 0x0000;
	::RGBForeColor( &blackColor ); 
	::RGBBackColor( &whiteColor ); 

	::CopyBits( &(pSrcGrafPort->portBits), &(pDestGrafPort->portBits), &srcRect, &destRect, mode, nil );

	::RGBForeColor( &foreColor ); 
	::RGBBackColor( &backColor ); 

	// Restore the saved port
	SetPort(savePort);
}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SetForegroundColor( )
//
//  Description:		Sets the foreground color for monochrome bitmaps
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SetForegroundColor( const RSolidColor& color )
	{
	::RGBForeColor( &(YPlatformColor)color );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SetBackgroundColor( )
//
//  Description:		Sets the background color for monochrome bitmaps
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::SetBackgroundColor( const RSolidColor& color )
	{
	::RGBBackColor( &(YPlatformColor)color );
	}

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::SaveSurfaceState( )
//
//  Description:		saves the state of this Drawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSurfaceState* RGpDrawingSurface::SaveSurfaceState()
{
	return new RGpSurfaceState( this );
}
	
// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::RestoreDefaults()
//
//  Description:		Restores the default drawing surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::RestoreDefaults()
{
	RDrawingSurface::RestoreDefaults( );
	// reset clip region
	SetClipRect( RIntRect( -16384, -16384, 16384, 16384 ) );
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RGpDrawingSurface::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpDrawingSurface::Validate( ) const
{
	RDrawingSurface::Validate( );
	TpsAssertIsObject( RGpDrawingSurface, this );
	TpsAssert( m_pGrafPort, "NULL graf port." );
}

#endif	// TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RGpSurfaceState::RGpSurfaceState( )
//
//  Description:		saves the state of the given GpDrawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGpSurfaceState::RGpSurfaceState( RGpDrawingSurface* pDrawingSurface ) :
	RSurfaceState( pDrawingSurface )
{
	UnimplementedCode();
}


// ****************************************************************************
//
//  Function Name:	RGpSurfaceState::Restore( )
//
//  Description:		restore the previously saved state
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGpSurfaceState::Restore()
{
	TpsAssert( m_pSurface!=NULL, "Attempting to restore an uninitialized or already restored state" );

	RGpDrawingSurface* pDrawingSurface = dynamic_cast<RGpDrawingSurface*>( m_pSurface );
	TpsAssert( pDrawingSurface!=NULL, "GpScriptSurfaceState restoring the state of a non-gp drawing surface" );
	RSurfaceState::Restore();

	UnimplementedCode();
}

