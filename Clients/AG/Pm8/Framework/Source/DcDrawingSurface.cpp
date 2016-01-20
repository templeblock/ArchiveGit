// ****************************************************************************
//
//  File Name:			DcDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RDcDrawingSurface class
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
//  $Logfile:: /PM8/Framework/Source/DcDrawingSurface.cpp                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "DcDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"
#include	"GradientFill.h"
#include "BitmapImage.h"
#include "VectorImage.h"
#include "QuadraticSpline.h"
#include	"ClipPolygon.h"
#include	"OffscreenDrawingSurface.h"
#include	"Tpsbit.h"

#ifdef		TPSDEBUG
#include	"DcDrawingSurface.inl"
#endif	// TPSDEBUG

#ifdef	IMAGER
#error	"Imager is not supported with this code base anymore"
#endif

const int kFillPolyMode = ALTERNATE;

const RIntRect	g_rc16BitCoordinates( -32768, -32768, 32767, 32767 );

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::RDcDrawingSurface( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDcDrawingSurface::RDcDrawingSurface( BOOLEAN fUsePathSupport, BOOLEAN fIsPrinting )
	: RDrawingSurface( fIsPrinting ),
	  m_hOutputDC( NULL ),
	  m_hAttributeDC( NULL ),
	  m_OrigFont( NULL ),
	  m_fUseScreenPalette( TRUE ),
	  m_OrigPalette( NULL ),
	  m_fUsePathSupport( fUsePathSupport )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::~RDcDrawingSurface( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDcDrawingSurface::~RDcDrawingSurface( )
	{
	DetachDCs( );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::DetachDCs( )
//
//  Description:		Called to remove the association between this drawing
//							surface, and its dcs.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::DetachDCs( )
	{
	// Clean up pens and brushes
	if( m_hOutputDC )
		{
		HGDIOBJ hGDIObject = NULL;
#ifndef DONT_USE_CACHE		
		// Select stock brush
		hGDIObject = ::SelectObject( m_hOutputDC, ::GetStockObject( BLACK_BRUSH ) );
		TpsAssert(hGDIObject, "Could not select original brush into DC.");
		RGDIBrush	brush( (HBRUSH)hGDIObject );
		GetBrushCache().ReleaseObject( brush );
		
		// Select stock pen
		hGDIObject = ::SelectObject( m_hOutputDC, ::GetStockObject( BLACK_PEN ) );
		TpsAssert(hGDIObject, "Could not select original pen into DC.");
		RGDIPen		pen( (HPEN)hGDIObject );
		GetPenCache().ReleaseObject( pen );
		
		// Select original font
		if (m_OrigFont)
			{
			hGDIObject = ::SelectObject( m_hOutputDC, m_OrigFont );
			TpsAssert(hGDIObject, "Could not select original font into DC");
			RGDIFont		font( (HFONT)hGDIObject );
			GetFontCache().ReleaseObject( font );
			}
#else		//	DONT_USE_CACHE
		// Select stock brush
		hGDIObject = ::SelectObject( m_hOutputDC, ::GetStockObject( BLACK_BRUSH ) );
		TpsAssert(hGDIObject, "Could not select original brush into DC.");
		::DeleteObject( hGDIObject );

		// Select stock pen
		hGDIObject = ::SelectObject( m_hOutputDC, ::GetStockObject( BLACK_PEN ) );
		TpsAssert(hGDIObject, "Could not select original pen into DC.");
		::DeleteObject( hGDIObject );		
		
		// Select original font
		if (m_OrigFont)
			{
			hGDIObject = ::SelectObject( m_hOutputDC, m_OrigFont );
			TpsAssert(hGDIObject, "Could not select original font into DC");
			::DeleteObject( hGDIObject );
			}
#endif	//	DONT_USE_CACHE
		m_OrigFont = NULL;
		m_fFontSet = FALSE;

		//	Remove the application palette.
		//	Restore the original palette to the output dc.
		if ( m_OrigPalette )
			{
			HPALETTE hPalette;
			
			//	Select the appropriate palette.
			if ( m_fUseScreenPalette )
				hPalette = (HPALETTE)tbitGetScreenPalette();
			else
				hPalette = (HPALETTE)tbitGetPrinterPalette();

			m_OrigPalette = ::SelectPalette( m_hOutputDC, m_OrigPalette, FALSE );
			TpsAssert( m_OrigPalette, "Failed to install the original palette." );
			TpsAssert( hPalette == m_OrigPalette, "The application palette was prematurely removed from the output dc." );
			if ( m_OrigPalette )
				::RealizePalette( m_hOutputDC );
			m_OrigPalette = NULL;
			m_fUseScreenPalette = TRUE;
			}
		}
	
	::SetTextAlign( m_hOutputDC, TA_TOP );
	::SetTextAlign( m_hAttributeDC, TA_TOP );

	// NULL out the dcs
	m_hOutputDC = m_hAttributeDC = NULL;
	}


// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::Initialize( )
//
//  Description:		Init function
//
//  Returns:			TRUE if the function succeeded, FALSE otherwise
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RDcDrawingSurface::Initialize( HDC hOutputDC, HDC hAttributeDC )
	{
	TpsAssert( hOutputDC, "NULL output DC" );

	m_hOutputDC = hOutputDC;

	// Assign the attribute dc. The attribute dc is the dc used to retreive
	//	device characteristics from. For example, for print preview the screen
	// would be used at the output dc, and the printer would be used as the
	// attribute dc.
	if( hAttributeDC )
		m_hAttributeDC = hAttributeDC;
	else
		m_hAttributeDC = hOutputDC;

	//	Set up the Text Attributes..
	::SetBkMode( m_hOutputDC, TRANSPARENT );
	::SetTextAlign(  m_hOutputDC, TA_BASELINE );
	if ( hAttributeDC )
		{
		::SetBkMode( m_hAttributeDC, TRANSPARENT );
		::SetTextAlign( m_hAttributeDC, TA_BASELINE );
		}

	//	Setup the application palette.
	//	Save the current palette from the given dc.
	//	We only have an 8 bit palette for now so don't use one for other screen depths.
	//	Don't use a palette if we haven't started the tps libs yet ( they load our palette ).
	if ( (8 == GetBitDepth()) && tbitStarted( ) )
		{
		//	Select the appropriate palette to use.
		HPALETTE hPalette;
		if ( m_fUseScreenPalette )
			hPalette = (HPALETTE)tbitGetScreenPalette();
		else
			hPalette = (HPALETTE)tbitGetPrinterPalette();

		m_OrigPalette = ::SelectPalette( m_hOutputDC, hPalette, FALSE );
		TpsAssert( m_OrigPalette, "Failed to install the application palette." );
		if ( m_OrigPalette )
			::RealizePalette( m_hOutputDC );
		}

	//	Set all 'Created' variables to Dirty so they will be created again
	m_fPenDirty		= TRUE;
	m_fBrushDirty	= TRUE;
	m_fFontSet		= FALSE;
	m_fFontDirty	= TRUE;
	m_PenColor		= RColor( kBlack );
	m_PenWidth		= 1;
	m_PenStyle		= kSolidPen;
	m_FillColor		= RColor( kWhite );
	m_FontColor		= RColor( kBlack );

	//	Force the background and Foreground color to black and white
	::SetBkColor(m_hOutputDC, RGB(0xFF,0xFF,0xFF) );
	::SetTextColor(m_hOutputDC, RGB(0,0,0) );

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::CreatePen( )
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
void RDcDrawingSurface::CreatePen( )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );
	TpsAssert( m_PenColor.GetFillMethod( ) == RColor::kSolid, "Only solid pens supported." );

	// If it is a solid pen, create it
	if( m_fPenDirty && m_PenColor.GetFillMethod( ) == RColor::kSolid )
		{
		RSolidColor solidColor = m_PenColor.GetSolidColor( );

		if (m_TintColor != RSolidColor( kBlack ))
		{
		   YColorComponent red   = m_TintColor.GetRed();
			YColorComponent green = m_TintColor.GetGreen();
			YColorComponent blue  = m_TintColor.GetBlue();

			red   += solidColor.GetRed() * short(255 - red) / 255;
			green += solidColor.GetGreen() * short(255 - green) / 255;
			blue  += solidColor.GetBlue() * short(255 - blue) / 255;

			solidColor = RSolidColor( red, green, blue );
		}

		if ( m_Tint < kMaxTint )
			solidColor.LightenBy( (YRealDimension)(kMaxTint-m_Tint) / (YRealDimension)kMaxTint );

		RGDIPenInfo	penInfo( m_PenStyle, m_PenWidth, PALETTERGB(	solidColor.GetRed(),
																					solidColor.GetGreen(),
																					solidColor.GetBlue() ) );
		RGDIPen&		newPen = GetPenCache().GetObject( penInfo );		
		RGDIPen		pen( (HPEN)::SelectObject( m_hOutputDC, newPen.GetPen() ) );
		GetPenCache().ReleaseObject( pen );
		}

	m_fPenDirty = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::CreateBrush( )
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
void RDcDrawingSurface::CreateBrush( )
	{
	// Assert the surface 
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	// If it is a solid brush, create it
	if( m_fBrushDirty && m_FillColor.GetFillMethod( ) == RColor::kSolid )
		{
		RSolidColor solidColor = m_FillColor.GetSolidColor( );

		if (m_TintColor != RSolidColor( kBlack ))
		{
		   YColorComponent red   = m_TintColor.GetRed();
			YColorComponent green = m_TintColor.GetGreen();
			YColorComponent blue  = m_TintColor.GetBlue();

			red   += solidColor.GetRed() * short(255 - red) / 255;
			green += solidColor.GetGreen() * short(255 - green) / 255;
			blue  += solidColor.GetBlue() * short(255 - blue) / 255;

			solidColor = RSolidColor( red, green, blue );
		}

		if( m_Tint < kMaxTint )
			solidColor.LightenBy( (YRealDimension)(kMaxTint-m_Tint) / (YRealDimension)kMaxTint );

		RGDIBrush&	newBrush = GetBrushCache().GetObject(PALETTERGB(solidColor.GetRed(),
																						solidColor.GetGreen(),
																						solidColor.GetBlue() ) );

		RGDIBrush	brush( (HBRUSH)::SelectObject( m_hOutputDC, newBrush.GetBrush() ) );
		GetBrushCache().ReleaseObject( brush );
		}

	m_fBrushDirty = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::CreateFont( )
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
void RDcDrawingSurface::CreateFont( )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );
	TpsAssert( m_fFontSet, "Creating a font before it was set into the device." );
	TpsAssert( m_FontColor.GetFillMethod( ) == RColor::kSolid, "Font color must be solid." );

	if( m_fFontDirty )
		{
		RGDIFont&	newFont = GetFontCache().GetObject( m_FontInfo );		
		RGDIFont		font( (HFONT)::SelectObject( m_hOutputDC, newFont.GetFont() ) );
		GetFontCache().ReleaseObject( font );
		if (m_OrigFont == NULL)
			m_OrigFont = font.GetFont();

		//	Set the flags that identify the font has been set and is no longer dirty
		m_fFontDirty	= FALSE;
		}

	RSolidColor fontColor( m_FontColor.GetSolidColor( ) );
	//
	//	Check if the font color is solid - black.  If it is, change it to the Windows Text color
	//		if we are not printing
	if ( !IsPrinting() && (fontColor == RSolidColor(kBlack)) )
		fontColor	= RSolidColor( COLORREF(::GetSysColor(COLOR_WINDOWTEXT)) );

	if ( m_Tint < kMaxTint )
		fontColor.LightenBy( (YRealDimension)(kMaxTint-m_Tint) / (YRealDimension)kMaxTint );

	::SetTextColor(m_hOutputDC, PALETTERGB(fontColor.GetRed(),
														fontColor.GetGreen(),
														fontColor.GetBlue() ) );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::MoveTo( )
//
//  Description:		Changes the current drawing position to the given point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::MoveTo( const RIntPoint& point )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	if( SetCurrentPoint( point ) )
		::MoveToEx( m_hOutputDC, point.m_x, point.m_y, NULL );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SolidLineTo( )
//
//  Description:		Draws a solid line, using the updating the current point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SolidLineTo( const RIntPoint& point )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	::LineTo( m_hOutputDC, point.m_x, point.m_y );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SolidFillRectangle( )
//
//  Description:		Fills a solid rectangle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SolidFillRectangle( const RIntRect& rect )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	// Fill the rectangle with a region. Do this because I had all sorts of problems
	// doing it with Rectangle with a NULL pen, and FillRect will not use the ROP mode.
	HRGN hRegion = ::CreateRectRgn( rect.m_Left, rect.m_Top, rect.m_Right, rect.m_Bottom );

	if( !hRegion )
		throw kResource;

	::PaintRgn( m_hOutputDC, hRegion );
	::DeleteObject( hRegion );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SolidPolyline( )
//
//  Description:		Draws a bunch of solid lines
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SolidPolyline( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	::Polyline( m_hOutputDC, (POINT*)pPoints, numPoints );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SolidPolylineTo( )
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
void RDcDrawingSurface::SolidPolylineTo( const RIntPoint* pPoints, YPointCount numPoints )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	::PolylineTo( m_hOutputDC, (POINT*)pPoints, numPoints );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SolidPolygon( )
//
//  Description:		Fills and frames a solid polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SolidPolygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	::Polygon( m_hOutputDC, (POINT*)pVertices, numVertices );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SolidFillPolygon( )
//
//  Description:		Fills a solid polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SolidFillPolygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	// Select a NULL pen, as we dont want a border
	HGDIOBJ hOldPen = ::SelectObject( m_hOutputDC, ::GetStockObject( NULL_PEN ) );

	if( !hOldPen )
		throw kResource;

	// Draw the polygon
	::Polygon( m_hOutputDC, (POINT*)pVertices, numVertices );

	// Select the old pen
	::SelectObject( m_hOutputDC, hOldPen );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SolidPolyPolygon( )
//
//  Description:		Fills and frames a solid poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SolidPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	::PolyPolygon( m_hOutputDC, (POINT*)pVertices, pointCountArray, polygonCount );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SolidFillPolyPolygon( )
//
//  Description:		Fills a solid poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SolidFillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	// Select a NULL pen, as we dont want a border
	HGDIOBJ hOldPen = ::SelectObject( m_hOutputDC, ::GetStockObject( NULL_PEN ) );

	if( !hOldPen )
		throw kResource;

	// Draw the polygon
	::PolyPolygon( m_hOutputDC, (POINT*)pVertices, pointCountArray, polygonCount );

	// Select the old pen
	::SelectObject( m_hOutputDC, hOldPen );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SolidFramePolyPolygon( )
//
//  Description:		Frames a solid poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SolidFramePolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	// Select a NULL brush, as we dont want to fill
	HGDIOBJ hOldBrush = ::SelectObject( m_hOutputDC, ::GetStockObject( NULL_BRUSH ) );

	if( !hOldBrush )
		throw kResource;

	::PolyPolygon( m_hOutputDC, (POINT*)pVertices, pointCountArray, polygonCount );

	// Select the old brush
	::SelectObject( m_hOutputDC, hOldBrush );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::DeviceInvertRectangle( )
//
//  Description:		Inverts a rectangle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::DeviceInvertRectangle( const RIntRect& rect )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	CRect rc( rect );
	::InvertRect( m_hOutputDC, &rc );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::DeviceInvertPolygon( )
//
//  Description:		Inverts a polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::DeviceInvertPolygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	//	Create the Region
	HRGN	hRgn = ::CreatePolygonRgn( (POINT*)pVertices, numVertices, kFillPolyMode  );

	if( !hRgn )
		throw kResource;

	//	Invert the Region
	::InvertRgn( m_hOutputDC, hRgn );

	//	Delete the Region
	::DeleteObject( hRgn );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::DeviceInvertPolyPolygon( )
//
//  Description:		Inverts a poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::DeviceInvertPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	//	Create the Region
	HRGN hRgn = ::CreatePolyPolygonRgn( (POINT*)pVertices, (int*)pointCountArray, polygonCount, kFillPolyMode  );

	if( !hRgn )
		throw kResource;

	//	Invert the Region
	::InvertRgn( m_hOutputDC, hRgn );

	//	Delete the Region
	::DeleteObject( hRgn );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::IntersectClipPath( )
//
//  Description:		adds the current path to the current clipping region
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::IntersectClipPath( )
{
	TpsAssert( m_fUsePathSupport, "Path support should not be used on this surface." );
	::SelectClipPath( m_hOutputDC, RGN_AND );
	//
	// Reset the clip edges for the current point...
	SetCurrentPoint( GetCurrentPoint() );
}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::FillPath( )
//
//  Description:		Fills the current path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::FillPath( )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	TpsAssert( m_fUsePathSupport, "Path support should not be used on this surface." );
	CreateBrush( );

	// Select a NULL pen, as we dont want a border
	HGDIOBJ hOldPen = ::SelectObject( m_hOutputDC, ::GetStockObject( NULL_PEN ) );

	::FillPath( m_hOutputDC );

	// Select the old pen
	::SelectObject( m_hOutputDC, hOldPen );

	if ( m_fColoringBookModeOn )
		{
		FramePath( );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::FramePath( )
//
//  Description:		Frames the current path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::FramePath( )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );
	TpsAssert( m_fUsePathSupport, "Path support should not be used on this surface." );

	CreatePen( );

	// Select a NULL brush, as we dont want to fill
	HGDIOBJ hOldBrush = ::SelectObject( m_hOutputDC, ::GetStockObject( NULL_BRUSH ) );

	::StrokePath( m_hOutputDC );

	// Select the old brush
	::SelectObject( m_hOutputDC, hOldBrush );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::FrameAndFillPath( )
//
//  Description:		Frames and fills the current path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::FrameAndFillPath( )
	{
	FramePath( );
	FillPath( );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::FillAndFramePath( )
//
//  Description:		Frames and fills the current path.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::FillAndFramePath( )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );
	TpsAssert( m_fUsePathSupport, "Path support should not be used on this surface." );

	CreatePen( );
	CreateBrush( );
	::StrokeAndFillPath( m_hOutputDC );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SetForegroundMode( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SetForegroundMode( YDrawMode drawMode )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	// Call the base method
	RDrawingSurface::SetForegroundMode( drawMode );

	::SetROP2( m_hOutputDC, drawMode );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SetPenWidth( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SetPenWidth( YPenWidth penWidth )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	// If the pen width is changing, mark the pen as dirty
	if( penWidth != m_PenWidth )
		{
		m_fPenDirty = TRUE;
		// Call the base method
		RDrawingSurface::SetPenWidth( penWidth );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SetPenStyle( )
//
//  Description:		Sets surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SetPenStyle( YPenStyle penStyle )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	// If the pen style is changing, mark the pen as dirty
	if( penStyle != m_PenStyle )
		{
		m_fPenDirty = TRUE;
		// Call the base method
		RDrawingSurface::SetPenStyle( penStyle );
		}
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::RestoreDefaults()
//
//  Description:		Restores the default drawing surface attributes
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::RestoreDefaults()
	{
	RDrawingSurface::RestoreDefaults( );
	//
	//		Reset the Background mode for text AND the text alignment
	::SetBkMode( m_hOutputDC, TRANSPARENT );
	::SetBkMode( m_hAttributeDC, TRANSPARENT );
	::SetTextAlign( m_hOutputDC, TA_BASELINE );
	::SetTextAlign( m_hAttributeDC, TA_BASELINE );

	//	Force the background and Foreground color to black and white
	::SetBkColor(m_hOutputDC, RGB(0xFF,0xFF,0xFF) );
	::SetTextColor(m_hOutputDC, RGB(0,0,0) );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::GetDPI( )
//
//  Description:		Gets the DPI of the device
//
//  Returns:			A size object containing the device DPI
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RDcDrawingSurface::GetDPI( ) const
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	RIntSize dpi;

	dpi.m_dx = ::GetDeviceCaps( m_hAttributeDC, LOGPIXELSX );
	dpi.m_dy = ::GetDeviceCaps( m_hAttributeDC, LOGPIXELSY );

	return dpi;
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::GetBitDepth( )
//
//  Description:		Gets the bit depth of the device
//
//  Returns:			
//
//  Exceptions:		None
//
// ****************************************************************************
uLONG RDcDrawingSurface::GetBitDepth() const
{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	int sBitDepth = ::GetDeviceCaps(m_hAttributeDC, BITSPIXEL);
	int sBitPlanes = ::GetDeviceCaps(m_hAttributeDC, PLANES);

	return (uLONG)(sBitDepth * sBitPlanes);
}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::GetClipRect( )
//
//  Description:		Gets the tightest bounding rectangle around the current
//							clipping region.
//
//  Returns:			A rect containing the current clip rect, in device units
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntRect RDcDrawingSurface::GetClipRect( ) const
	{
	RECT clipRect;
	int nResult = ::GetClipBox( m_hOutputDC, &clipRect );
	
	TpsAssert( ERROR != nResult, "RDcDrawingSurface::GetClipBox failed" );
	TpsAssert( (clipRect.right >= clipRect.left) 
		&& (clipRect.bottom >= clipRect.top)
		, "RDcDrawingSurface::GetClipRect cliprect invalid" );

	return RIntRect( clipRect.left, clipRect.top, clipRect.right, clipRect.bottom );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SetClipRect( )
//
//  Description:		Sets the current clip rect
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::SetClipRect( const RIntRect& clipRect )
	{
	RIntRect	rcClip( clipRect );
	//	The rcClip contains 2 RIntPoints which are identical to the POINT structure
	::LPtoDP( m_hOutputDC, reinterpret_cast<POINT*>(&rcClip), 2 );
	if( !ClipRectangle(rcClip,g_rc16BitCoordinates) )
		{
		//
		// none of the rectangle is contained in the 16bit coordinate space
		rcClip	= RIntRect(0,0,0,0);
		}
	HRGN region = ::CreateRectRgn( rcClip.m_Left, rcClip.m_Top, rcClip.m_Right, rcClip.m_Bottom );
	::SelectClipRgn( m_hOutputDC, region );
	::DeleteObject( region );
	//
	// Reset the clip edges for the current point...
	SetCurrentPoint( GetCurrentPoint() );
	}



// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::IntersectClipRect( )
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
void RDcDrawingSurface::IntersectClipRect( const RIntRect& clipRect )
	{
	RIntRect	rcClip( clipRect );
	if( !ClipRectangle(rcClip,g_rc16BitCoordinates) )
		{
		//
		// none of the rectangle is contained in the 16bit coordinate space
		rcClip	= RIntRect(0,0,0,0);
		}

	::IntersectClipRect( m_hOutputDC, rcClip.m_Left, rcClip.m_Top, rcClip.m_Right, rcClip.m_Bottom );
	//
	// Reset the clip edges for the current point...
	SetCurrentPoint( GetCurrentPoint() );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SetClipVectorRect( )
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
void RDcDrawingSurface::SetClipVectorRect( const RIntVectorRect& clipRect )
	{
	//
	// Create the region...
	HRGN			hRegion;
	RIntPoint	poly[4];
	clipRect.GetPolygonPoints( poly );
	//	Convert logical points to device points
	::LPtoDP( m_hOutputDC, reinterpret_cast<POINT*>(poly), sizeof(poly)/sizeof(POINT) );

	if( IsPoly32(poly,4) )
		{
		//
		// Get the clip polygon...
		RIntPoint		poly2[ 8 ];
		YPointCount		nPoly( 4 );
		RIntPoint		ptClipPoly[ 4 ];
		YPointCount		nClipPoints( 4 );
		CreateRectPoly( g_rc16BitCoordinates, &ptClipPoly[ 0 ] );
		YPolygonCount	ncntMaxClipped	= 1;
		RIntPoint*		pptClipped		= poly2;
		YPointCount*	pcntClipped		= &nPoly;
		YPointCount		nptMaxClipped	= 8;
		YPolygonCount	nClipped			= ClipPolygon( poly, &nPoly, 1, ptClipPoly, &nClipPoints, 1, poly2, 8, &nPoly, 1 );
		if( nClipped==1 && nPoly>0 )
			{
			hRegion	= ::CreatePolygonRgn( reinterpret_cast<POINT*>(poly2), nPoly, kFillPolyMode );
			}
		else
			{
			hRegion	= ::CreateRectRgn( 0, 0, 0, 0 );
			}
		}
	else
		{
		// Create a region
		hRegion = ::CreatePolygonRgn( reinterpret_cast<POINT*>(poly), 4, kFillPolyMode );
		}
	//
	// Select it as the current clip region
	::SelectClipRgn( m_hOutputDC, hRegion );
	//
	// Delete it
	::DeleteObject( hRegion );
	//
	// Reset the clip edges for the current point...
	SetCurrentPoint( GetCurrentPoint() );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::IntersectClipVectorRect( )
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
void RDcDrawingSurface::IntersectClipVectorRect( const RIntVectorRect& clipRect )
	{
	//
	// Create the region...
	HRGN			hRegion;
	RIntPoint	poly[4];
	clipRect.GetPolygonPoints( poly );
	//	Convert logical points to device points
	::LPtoDP( m_hOutputDC, reinterpret_cast<POINT*>(poly), sizeof(poly)/sizeof(POINT) );

	if( IsPoly32(poly,4) )
		{
		//
		// Get the clip polygon...
		RIntPoint		poly2[ 8 ];
		YPointCount		nPoly( 4 );
		RIntPoint		ptClipPoly[ 4 ];
		YPointCount		nClipPoints( 4 );
		CreateRectPoly( g_rc16BitCoordinates, &ptClipPoly[ 0 ] );
		YPolygonCount	ncntMaxClipped	= 1;
		RIntPoint*		pptClipped		= poly2;
		YPointCount*	pcntClipped		= &nPoly;
		YPointCount		nptMaxClipped	= 8;
		YPolygonCount	nClipped			= ClipPolygon( poly, &nPoly, 1, ptClipPoly, &nClipPoints, 1, poly2, 8, &nPoly, 1 );
		if( nClipped==1 && nPoly>0 )
			{
			hRegion	= ::CreatePolygonRgn( reinterpret_cast<POINT*>(poly2), nPoly, kFillPolyMode );
			}
		else
			{
			hRegion	= ::CreateRectRgn( 0, 0, 0, 0 );
			}
		}
	else
		{
		// Create a region
		hRegion = ::CreatePolygonRgn( reinterpret_cast<POINT*>(poly), 4, kFillPolyMode );
		}
	//
	// Intersect it with the current clip region
	::ExtSelectClipRgn( m_hOutputDC, hRegion, RGN_AND );
	//
	// Delete it
	::DeleteObject( hRegion );
	//
	// Reset the clip edges for the current point...
	SetCurrentPoint( GetCurrentPoint() );
	}
												
// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::DrawCharacters( )
//
//  Description:		Image the characters to the the Drawing Surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::DrawCharacters( const RIntPoint& point, YCounter count, LPCUBYTE pCharacters, int* pWidths )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );
	TpsAssert( pCharacters, "No Characters were passed to DrawCharacters" );

	RIntRect clipRect = GetClipRect( );
	CRect rc = clipRect;

	CreateFont( );
	// REVIEW MDH
	// COLORREF color = SetTextColor( m_hOutputDC, m_FontColor );
	ExtTextOut( m_hOutputDC, point.m_x, point.m_y, ETO_CLIPPED, &rc, (TCHAR*)pCharacters, count, pWidths );	
	// SetTextColor( m_hOutputDC, color );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::DrawFocusRect()
//
//  Description:		Image the characters to the the Drawing Surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::DrawFocusRect( const RIntRect rcRect )
{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." ) ;
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." ) ;

	CRect rect( rcRect ) ;
//	rect.OffsetRect( 1, 1 ) ;

	COLORREF crColor   = ::SetTextColor( m_hOutputDC, RGB( 0, 0, 0 ) ) ;
	COLORREF crBkColor = ::SetBkColor( m_hOutputDC, GetFillColor().GetSolidColor() ) ;
//	int      nBkMode   = ::SetBkMode( m_hOutputDC, OPAQUE ) ;

	::DrawFocusRect( m_hOutputDC, rect ) ;
	::SetTextColor( m_hOutputDC, crColor ) ;
	::SetBkColor( m_hOutputDC, crBkColor ) ;
//	::SetBkMode( m_hOutputDC, nBkMode ) ;
}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::GetTextSize( )
//
//  Description:		return the dimensions of the given text if drawn to the surface
//
//  Returns:			RIntSize dimensions
//
//  Exceptions:		None
//
// ****************************************************************************
//
RIntSize RDcDrawingSurface::GetActualTextSize( YCounter nChars, const uBYTE* pChars )
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );
	TpsAssert( pChars!=NULL, "NULL character pointer passed to GetTextSize" );
	TpsAssert( nChars>0, "Zero character count passed to GetTextSize" );

	SIZE	sz;
	CreateFont( );
	::GetTextExtentPoint32( m_hOutputDC, (LPCTSTR)pChars, int(nChars), &sz );
	return RIntSize( YIntDimension(sz.cx), YIntDimension(sz.cy) );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::GetActualFontDimensions()
//
//  Description:		return the actual font dimensions used when output
//
//  Returns:			RIntSize dimensions
//
//  Exceptions:		None
//
// ****************************************************************************
//
YFontDimensions RDcDrawingSurface::GetActualFontDimensions()
	{
	TpsAssert( m_hOutputDC != NULL, "NULL output dc." );
	TpsAssert( m_hAttributeDC != NULL, "NULL attribute dc." );

	TEXTMETRIC			tm;
	YFontDimensions	fd;

	CreateFont( );
	::GetTextMetrics( m_hOutputDC, &tm );
	fd.m_Overhang	= YFontSize( tm.tmOverhang );
	fd.m_Ascent		= YFontSize( tm.tmAscent );
	fd.m_Descent	= YFontSize( tm.tmDescent );
	fd.m_Leading	= YFontSize( tm.tmExternalLeading );
	return fd;
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::BlitDrawingSurface( )
//
//  Description:		Copy from one the given drawing surface to this one
//							honoring the transparent color
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************

void RDcDrawingSurface::BlitDrawingSurface(const RDrawingSurface& rSrcDS, RIntRect rSrcRect, RIntRect rDestRect, const RSolidColor& transColor)
{
	TpsAssert(m_hOutputDC, "NULL output DC.");
	TpsAssert(m_hAttributeDC, "NULL attribute DC.");

	TpsAssertIsObject(RDcDrawingSurface, &rSrcDS);
	TpsAssert(rSrcDS.GetSurface(), "rSrcDS m_hOutputDC is NULL.");
	int nMode = ::SetStretchBltMode(m_hOutputDC, COLORONCOLOR);

	// Stretch the bits with a transparency to the screen (2nd pass)
	
	HBITMAP hBitmap  = CreateCompatibleBitmap(m_hOutputDC, rDestRect.Width(), rDestRect.Height());
	HDC hdcImage   = CreateCompatibleDC(m_hOutputDC);
	HBITMAP bmOld = (HBITMAP)SelectObject(hdcImage, hBitmap);

	// Stretch the source bits into the dest rect
	
	StretchBlt(hdcImage, 0,	0, rDestRect.Width(), rDestRect.Height(),
		(HDC)rSrcDS.GetSurface(),
		rSrcRect.m_Left, rSrcRect.m_Top, rSrcRect.Width(), rSrcRect.Height(),
		SRCCOPY);

	// Create a mask

	HBITMAP hBitmapMask  = CreateBitmap(rDestRect.Width(), rDestRect.Height(), 1, 1, NULL);
	HDC hdcImageMask   = CreateCompatibleDC(m_hOutputDC);
	HBITMAP bmOldMask = (HBITMAP)SelectObject(hdcImageMask, hBitmapMask);

	// Use the transparent color to fix the mask

	BYTE red =	 GetRValue(transColor);
	BYTE green = GetGValue(transColor);
	BYTE blue =  GetBValue(transColor);
	COLORREF crTransparent = RGB(red, green, blue);
	COLORREF cColorOld = SetBkColor(hdcImage, crTransparent);
	BitBlt(hdcImageMask, 0, 0, rDestRect.Width(), rDestRect.Height(), hdcImage, 0, 0, SRCCOPY);
	SetBkColor(hdcImage, cColorOld);

	// Create an off screen (OS) dc with the contents of the destination dc to work with 
	
	HBITMAP hBitmapOS  = CreateCompatibleBitmap(m_hOutputDC,rDestRect.Width(), rDestRect.Height());
	HDC hdcImageOS   = CreateCompatibleDC(m_hOutputDC);
	HBITMAP bmOldOS = (HBITMAP)SelectObject(hdcImageOS, hBitmapOS);
	BitBlt(hdcImageOS, 0, 0, rDestRect.Width(), rDestRect.Height(), m_hOutputDC, rDestRect.m_Left,rDestRect.m_Top, SRCCOPY);
	
	// Now we SRCINVERT image to OS, SCRAND mask to OS then SRCINVERT image to OS
	
	BitBlt(hdcImageOS, 0, 0, rDestRect.Width(), rDestRect.Height(), hdcImage,		0, 0, SRCINVERT);
	BitBlt(hdcImageOS, 0, 0, rDestRect.Width(), rDestRect.Height(), hdcImageMask,	0, 0, SRCAND);
	BitBlt(hdcImageOS, 0, 0, rDestRect.Width(), rDestRect.Height(), hdcImage,		0, 0, SRCINVERT);

	// Finally (whew); copy the off screen image to the screen. VOLA!, it's transparent!.

	BitBlt(m_hOutputDC, rDestRect.m_Left,rDestRect.m_Top, rDestRect.Width(), rDestRect.Height(), hdcImageOS, 0, 0, SRCCOPY);
				
	// Clean up the resources we used...

	DeleteObject(SelectObject(hdcImageOS, bmOldOS));
	DeleteDC(hdcImageOS);

	DeleteObject(SelectObject(hdcImageMask, bmOldMask));
	DeleteDC(hdcImageMask);

	DeleteObject(SelectObject(hdcImage, bmOld));
	DeleteDC(hdcImage);

	::SetStretchBltMode(m_hOutputDC, nMode);
}
// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::BlitDrawingSurface( )
//
//  Description:		Copy from one the given drawing surface to this one
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
void RDcDrawingSurface::BlitDrawingSurface(const RDrawingSurface& rSrcDS, RIntRect rSrcRect, RIntRect rDestRect, EBlitMode eBlitMode)
{
	TpsAssert(m_hOutputDC, "NULL output DC." );
	TpsAssert(m_hAttributeDC, "NULL attribute DC." );

	TpsAssertIsObject(RDcDrawingSurface, &rSrcDS);
	TpsAssert(rSrcDS.GetSurface(), "rSrcDS m_hOutputDC is NULL.");

	int sOldStretchMode = ::SetStretchBltMode(m_hOutputDC, COLORONCOLOR);
	BOOL fResult = ::StretchBlt(m_hOutputDC, 
		rDestRect.m_Left,
		rDestRect.m_Top,
		rDestRect.Width(),
		rDestRect.Height(),
		(HDC)rSrcDS.GetSurface(),
		rSrcRect.m_Left,
		rSrcRect.m_Top,
		rSrcRect.Width(),
		rSrcRect.Height(),
		eBlitMode);
	
	//	If we failed to draw recur on smaller images. Ground the recursion
	//	by stopping when the bitmap is tiny.
	if (!fResult && (rDestRect.Width() > 1) && (rDestRect.Height() > 1))
	{
		// Try breaking the image up into smaller chunks and blit again
		RIntRect rSrcRectUL, rSrcRectLL, rSrcRectUR, rSrcRectLR;
		RIntRect rDestRectUL, rDestRectLL, rDestRectUR, rDestRectLR;
		
		// Upper-left rects
		rSrcRectUL.m_Left = rSrcRect.m_Left;
		rSrcRectUL.m_Top = rSrcRect.m_Top;
		rSrcRectUL.m_Right = rSrcRect.m_Left + rSrcRect.Width() / 2;
		rSrcRectUL.m_Bottom = rSrcRect.m_Top + rSrcRect.Height() / 2;
		
		rDestRectUL.m_Left = rDestRect.m_Left;
		rDestRectUL.m_Top = rDestRect.m_Top;
		rDestRectUL.m_Right = rDestRect.m_Left + rDestRect.Width() / 2;
		rDestRectUL.m_Bottom = rDestRect.m_Top + rDestRect.Height() / 2;
		
		BlitDrawingSurface(rSrcDS, rSrcRectUL, rDestRectUL, eBlitMode);		

		// Lower-left rects
		rSrcRectLL.m_Left = rSrcRect.m_Left;
		rSrcRectLL.m_Top = rSrcRect.m_Top + rSrcRect.Height() / 2;
		rSrcRectLL.m_Right = rSrcRect.m_Left + rSrcRect.Width() / 2;
		rSrcRectLL.m_Bottom = rSrcRect.m_Bottom;
		
		rDestRectLL.m_Left = rDestRect.m_Left;
		rDestRectLL.m_Top = rDestRect.m_Top + rDestRect.Height() / 2;
		rDestRectLL.m_Right = rDestRect.m_Left + rDestRect.Width() / 2;
		rDestRectLL.m_Bottom = rDestRect.m_Bottom;

		BlitDrawingSurface(rSrcDS, rSrcRectLL, rDestRectLL, eBlitMode);

		// Upper-right rects
		rSrcRectUR.m_Left = rSrcRect.m_Left + rSrcRect.Width() / 2;
		rSrcRectUR.m_Top = rSrcRect.m_Top;
		rSrcRectUR.m_Right = rSrcRect.m_Right;
		rSrcRectUR.m_Bottom = rSrcRect.m_Top + rSrcRect.Height() / 2;
		
		rDestRectUR.m_Left = rDestRect.m_Left + rDestRect.Width() / 2;
		rDestRectUR.m_Top = rDestRect.m_Top;
		rDestRectUR.m_Right = rDestRect.m_Right;
		rDestRectUR.m_Bottom = rDestRect.m_Top + rDestRect.Height() / 2;

		BlitDrawingSurface(rSrcDS, rSrcRectUR, rDestRectUR, eBlitMode);

		// Lower-right rects
		rSrcRectLR.m_Left = rSrcRect.m_Left + rSrcRect.Width() / 2;
		rSrcRectLR.m_Top = rSrcRect.m_Top + rSrcRect.Height() / 2;
		rSrcRectLR.m_Right = rSrcRect.m_Right;
		rSrcRectLR.m_Bottom = rSrcRect.m_Bottom;
		
		rDestRectLR.m_Left = rDestRect.m_Left + rDestRect.Width() / 2;
		rDestRectLR.m_Top = rDestRect.m_Top + rDestRect.Height() / 2;
		rDestRectLR.m_Right = rDestRect.m_Right;
		rDestRectLR.m_Bottom = rDestRect.m_Bottom;

		BlitDrawingSurface(rSrcDS, rSrcRectLR, rDestRectLR, eBlitMode);
	}

	::SetStretchBltMode(m_hOutputDC, sOldStretchMode);
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
void RDcDrawingSurface::SetForegroundColor( const RSolidColor& color )
	{
	::SetTextColor( m_hOutputDC, color );
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
void RDcDrawingSurface::SetBackgroundColor( const RSolidColor& color )
	{
	::SetBkColor( m_hOutputDC, color );
	}

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::SaveSurfaceState( )
//
//  Description:		saves the state of this Drawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RSurfaceState* RDcDrawingSurface::SaveSurfaceState()
{
	return new RDcSurfaceState( this );
}

// ****************************************************************************
//
//  Function Name:	RDcSurfaceState::RDcSurfaceState( )
//
//  Description:		saves the state of the given DcDrawing surface
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RDcSurfaceState::RDcSurfaceState( RDcDrawingSurface* pDrawingSurface ) :
	RSurfaceState( pDrawingSurface )
{
	HDC	hdc		= (HDC)pDrawingSurface->GetSurface();
	m_nSaveID		= ::SaveDC( hdc );
	m_fPenDirty		= pDrawingSurface->m_fPenDirty;
	m_fBrushDirty	= pDrawingSurface->m_fBrushDirty;
	m_fFontDirty	= pDrawingSurface->m_fFontDirty;
	m_fFontSet		= pDrawingSurface->m_fFontSet;

	RGDIPen		pen( (HPEN)::SelectObject( hdc, ::GetStockObject(NULL_PEN) ) );
	GetPenCache().UseObject( pen );
	::SelectObject( hdc, pen.GetPen() );

	RGDIBrush	brush( (HBRUSH)::SelectObject( hdc, ::GetStockObject(NULL_BRUSH) ) );
	GetBrushCache().UseObject( brush );
	::SelectObject( hdc, brush.GetBrush() );

	RGDIFont		font( (HFONT)::SelectObject( hdc, ::GetStockObject(ANSI_FIXED_FONT) ) );
	GetFontCache().UseObject( font );
	::SelectObject( hdc, font.GetFont() );
}


// ****************************************************************************
//
//  Function Name:	RDcSurfaceState::Restore( )
//
//  Description:		restore the previously saved state
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcSurfaceState::Restore()
{
	TpsAssert( m_pSurface!=NULL, "Attempting to restore an uninitialized or already restored state" );

	RDcDrawingSurface* pDrawingSurface = dynamic_cast<RDcDrawingSurface*>( m_pSurface );
	TpsAssert( pDrawingSurface!=NULL, "DcScriptSurfaceState restoring the state of a non-dc drawing surface" );
	RSurfaceState::Restore();

	pDrawingSurface->m_fPenDirty		= m_fPenDirty;
	pDrawingSurface->m_fBrushDirty	= m_fBrushDirty;
	pDrawingSurface->m_fFontDirty		= m_fFontDirty;
	pDrawingSurface->m_fFontSet		= m_fFontSet;

	HDC	hdc	= (HDC)pDrawingSurface->GetSurface();
	GetPenCache().ReleaseObject( RGDIPen((HPEN)::SelectObject(hdc,::GetStockObject(NULL_PEN))) );
	GetBrushCache().ReleaseObject( RGDIBrush((HBRUSH)::SelectObject(hdc,::GetStockObject(NULL_BRUSH))) );
	GetFontCache().ReleaseObject( RGDIFont((HFONT)::SelectObject(hdc,::GetStockObject(ANSI_FIXED_FONT))) );
	::RestoreDC( hdc, m_nSaveID );
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RDcDrawingSurface::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RDcDrawingSurface::Validate( ) const
	{
	RDrawingSurface::Validate( );
	TpsAssertIsObject( RDcDrawingSurface, this );
	}

#endif	// TPSDEBUG
