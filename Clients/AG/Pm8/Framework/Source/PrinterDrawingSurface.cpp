// ****************************************************************************
//
//  File Name:			PrinterDrawingSurface.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Definition of the RPrinterDrawingSurface class
//
//  Portability:		Cross platform
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
//  $Logfile:: /PM8/Framework/Source/PrinterDrawingSurface.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "PrinterDrawingSurface.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::CreatePen( )
//
//  Description:		Creates a new pen and selects into the dc if the pen
//							attributes have changed, otherwise does nothing.
//
//  Returns:			Nothing
//
//  Exceptions:		kPrintingCanceled
//
// ****************************************************************************
//
void RPrinterDrawingSurface::CreatePen( )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::CreatePen( );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::CreateBrush( )
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
void RPrinterDrawingSurface::CreateBrush( )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::CreateBrush( );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::MoveTo( )
//
//  Description:		Changes the current drawing position to the given point.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::MoveTo( const RIntPoint& point )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::MoveTo( point );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::SolidLineTo( )
//
//  Description:		Draws a solid line, using the updating the current point
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::SolidLineTo( const RIntPoint& point )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::SolidLineTo( point );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::SolidFillRectangle( )
//
//  Description:		Fills a solid rectangle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::SolidFillRectangle( const RIntRect& rect )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::SolidFillRectangle( rect );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::SolidPolyline( )
//
//  Description:		Draws a bunch of solid lines
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::SolidPolyline( const RIntPoint* pPoints, YPointCount numPoints )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::SolidPolyline( pPoints, numPoints );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::SolidPolylineTo( )
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
void RPrinterDrawingSurface::SolidPolylineTo( const RIntPoint* pPoints, YPointCount numPoints )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::SolidPolylineTo( pPoints, numPoints );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::SolidPolygon( )
//
//  Description:		Fills and frames a solid polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::SolidPolygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::SolidPolygon( pVertices, numVertices );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::SolidFillPolygon( )
//
//  Description:		Fills a solid polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::SolidFillPolygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::SolidFillPolygon( pVertices, numVertices );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::SolidPolyPolygon( )
//
//  Description:		Fills and frames a solid poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::SolidPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::SolidPolyPolygon( pVertices, pointCountArray, polygonCount );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::SolidFillPolyPolygon( )
//
//  Description:		Fills a solid poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::SolidFillPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::SolidFillPolyPolygon( pVertices, pointCountArray, polygonCount );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::SolidFramePolyPolygon( )
//
//  Description:		Frames a solid poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::SolidFramePolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::SolidFramePolyPolygon( pVertices, pointCountArray, polygonCount );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::DeviceInvertRectangle( )
//
//  Description:		Inverts a rectangle
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::DeviceInvertRectangle( const RIntRect& rect )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::DeviceInvertRectangle( rect );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::DeviceInvertPolygon( )
//
//  Description:		Inverts a polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::DeviceInvertPolygon( const RIntPoint* pVertices, YPointCount numVertices )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::DeviceInvertPolygon( pVertices, numVertices );
	}

// ****************************************************************************
//
//  Function Name:	RPrinterDrawingSurface::DeviceInvertPolyPolygon( )
//
//  Description:		Inverts a poly polygon
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPrinterDrawingSurface::DeviceInvertPolyPolygon( const RIntPoint* pVertices, YPointCount* pointCountArray, YPolygonCount polygonCount )
	{
	CheckPrintingCanceled( );
	PrinterDrawingSurfaceBaseClass::DeviceInvertPolyPolygon( pVertices, pointCountArray, polygonCount );
	}
